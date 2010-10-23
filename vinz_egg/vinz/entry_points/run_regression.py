
import sys
import math
import simplejson
import sqlite3
import optparse
import getty

import vinz.module
from vinz.cluster import Clusterer
from vinz.feature_transform import FeatureTransform
from vinz.clustering_coordinator import ClusteringCoordinator


def iterate_and_report(coord, sample_classes, class_sample_size):

    entropy = coord.expect_and_maximize()

    # stats over samples to track
    tot_clus_prob, prec, recall = {}, {}, {}
    for k in class_sample_size:
        tot_clus_prob[k] = prec[k] = recall[k] = 0

    for sample in coord.samples:

        clus_prob = sample.cluster_probabilities

        for clus_id in sample_classes[sample.uid]:
            # aggregate precision & recall
            prec[clus_id] += clus_prob[clus_id][0]
            recall[clus_id] += clus_prob[clus_id][0]

        # aggregate total class probability
        for clus_id, (prob, is_hard) in clus_prob.items():
            tot_clus_prob[clus_id] += prob

    for clus_id, n_samples in class_sample_size.items():
        # normalize precision by total class prob
        prec[clus_id] /= tot_clus_prob[clus_id]
        # normalize recall by # of class samples
        recall[clus_id] /= n_samples

    stats = dict(
        entropy = entropy,
        log_likelihood = sum(
            math.log(s.probability) for s in coord.clusterer.samples),
        precision = prec,
        recall = recall,
        prior_probabilities = coord.clusterer.cluster_set.priors)

    return stats

def anneal(coord, sample_classes):

    # identify the least-likely 'soft' sample
    soft_samples = [s for s in coord.clusterer.samples if not \
        any(j[1] for (i,j) in s.cluster_probabilities.items())]

    #sample = min(soft_samples, key = lambda s: s.probability)
    sample_samples = sorted(soft_samples, key = lambda s: s.probability)
    sample = soft_samples[int(len(soft_samples) * 0.1)]

    # extract class with hightest estimator probability
    est_class = max(sample.cluster_probabilities.items(),
        key = lambda i: i[1][0])[0]

    # set hard probability for class membership(s)
    prob = {}
    for cname in sample_classes[sample.uid]:
        prob[cname] = (1, True)

    sample.cluster_probabilities = prob

    if est_class not in prob:
        # was estimated to be in a cluster that it's not
        coord.anneal_cluster(est_class)
        for cname in prob:
            coord.anneal_cluster(cname)

    return

def run_regression(
    iteration_count,
    regression_database,
    config_overrides,
    clusterer,
    feature_transform,
    class_names,
    ):

    # Set up injector
    inj = vinz.module.Module().configure(getty.Injector())

    # Override specified configuration parameters
    for conf_name, conf_value in config_overrides.items():
        inj.bind_instance(getty.Config,
            with_annotation = conf_name, to = conf_value)

    # Connect to regression database
    db = sqlite3.connect(regression_database, isolation_level = None)

    # Obtain a clusterer instance
    clusterer = inj.get_instance(
        vinz.cluster.Clusterer, annotation = clusterer)

    # If given, obtain feature-transform instance
    if feature_transform:
        feature_transform = inj.get_instance(
            vinz.feature_transform.FeatureTransform,
            annotation = feature_transform)

    coord = ClusteringCoordinator(clusterer, feature_transform, inj)

    # {sample-uid: [class-id]}
    sample_classes = {}
    # {class-id: # of samples}
    class_sample_size = {}

    # Load classes & samples
    for clus_id in class_names:
        clus_id = str(clus_id)

        class_sample_size[clus_id] = 0
        coord.add_cluster(clus_id)

        for uid, type, weight, attributes in db.execute("""
            select uid, type, weight, attributes from
                sample join sample_class on
                uid = sample_uid and class_name = ?""", (clus_id,)):

            uid = str(uid)
            if uid not in sample_classes:
                sample_classes[uid] = []
                attributes = simplejson.loads(attributes)

                sample = coord.add_sample(uid, type,  **attributes)
                sample.weight = weight

            # TODO: More principled initialization
            if class_sample_size[clus_id] < 2:
                sample = coord.clusterer.get_sample(uid)
                prob = sample.cluster_probabilities
                prob[clus_id] = (1.0, True)
                sample.cluster_probabilities = prob

            sample_classes[uid].append(clus_id)
            class_sample_size[clus_id] += 1.0

        assert class_sample_size[clus_id], \
            "No samples w/ class %s" % clus_id

    # Run regression iterations
    for i in xrange(iteration_count):
        stats = iterate_and_report(coord, sample_classes, class_sample_size)
        print simplejson.dumps(stats)
        sys.stdout.flush()

        if i and not i % 10:
            anneal(coord, sample_classes)

    return

def main():

    opt_parser = optparse.OptionParser()

    opt_parser.add_option('-i', '--iteration-count', action = 'store',
        type = 'int', dest = 'iteration_count', default = 150)
    opt_parser.add_option('-d', '--database', action = 'store',
        type = 'string', dest = 'regression_database')
    opt_parser.add_option('-c', '--clusterer', action = 'store',
        type = 'string', dest = 'clusterer',
        default = 'SparseGaussEmClusterer')
    opt_parser.add_option('-f', '--feature-transform', action = 'store',
        type = 'string', dest = 'feature_transform')
    opt_parser.add_option('-n', '--class-name', action = 'append',
        type = 'string', dest = 'class_names', default = [])
    opt_parser.add_option('-o', '--config-override', action = 'store',
        type = 'string', dest = 'config_overrides', default = '{}')

    opts, _ = opt_parser.parse_args()
 
    if not opts.regression_database:
        opt_parser.error("Regression database must be provided")
    if not opts.class_names or len(opts.class_names) < 2:
        opt_parser.error("At least two class-names are required")

    opts.config_overrides = simplejson.loads(opts.config_overrides)
    assert isinstance(opts.config_overrides, dict), \
        "Config overrides must be JSON map"

    run_regression(**opts.__dict__)

