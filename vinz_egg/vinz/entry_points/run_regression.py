
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

    priors = coord.clusterer.cluster_set.priors

    return entropy, prec, recall, priors

def anneal():

    min_li, min_uid, min_probs, min_cur_clus, min_act_clus = \
        None, None, None, None, None

    for cur_uid, cur_li, probs in coord.sample_state():

        # it's already fixed
        if any(i[1] for i in probs.values()):
            continue

        if min_li is None or cur_li < min_li:
            min_uid = cur_uid
            min_li = cur_li
            min_probs = probs

            min_cur_clus = max((j, i) for i, j in probs.items())[1]
            min_act_clus = 'class_%s' % cur_uid.split('_')[1]

    print min_uid, min_li

    min_probs[min_act_clus] = (1, True)
    x, y = sample_positions[min_uid]
    coord.drop_sample(min_uid)
    coord.add_sample(min_uid, 'dense_passthrough', 1,
        min_probs, features = [x, y])

    coord.anneal_cluster(min_cur_clus)
    coord.anneal_cluster(min_act_clus)
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

                # TODO: More principled initialization
                if class_sample_size[clus_id] < 2:
                    s_prob = {clus_id: (1.0, True)}
                else:
                    s_prob = {}

                sample = coord.add_sample(uid, type,  **attributes)
                sample.weight = weight
                sample.cluster_probabilities = s_prob

            sample_classes[uid].append(clus_id)
            class_sample_size[clus_id] += 1.0

        assert class_sample_size[clus_id], \
            "No samples w/ class %s" % clus_id

    # Run regression iterations
    for i in xrange(iteration_count):
        stats = iterate_and_report(coord, sample_classes, class_sample_size)
        print simplejson.dumps(stats)
        sys.stdout.flush()

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

