
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

N_ITERATIONS = 100

def iterate_and_report(coord, sample_classes, class_sample_size):

    entropy = coord.expect_and_maximize()

    # stats over samples to track
    prob, prec, recall = {}, {}, {}
    for k in class_sample_size:
        prob[k] = prec[k] = recall[k] = 0

    for uid, likelihood, class_probs in coord.sample_state():

        for class_id in sample_classes[uid]:
            # aggregate precision & recall
            prec[class_id] += class_probs[class_id][0]
            recall[class_id] += class_probs[class_id][0]

        # aggregate total class probability
        for class_id, (class_prob, is_hard) in class_probs.items():
            prob[class_id] += class_prob

    for class_id, n_samples in class_sample_size.items():
        # normalize precision by total class prob
        prec[class_id] /= prob[class_id]
        # normalize recall by # of class samples
        recall[class_id] /= n_samples 

    return entropy, prec, recall, prob

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

def main(opts):

    # Set up injector
    inj = vinz.module.Module().configure(getty.Injector())

    # Override specified configuration parameters
    for conf in opts.config_overrides:
        assert '=' in conf, "Configurations must be of form 'name = value'"

        name, value = [i.strip() for i in conf.split('=')]
        if value.isdigit():
            value = int(value)
        elif value.replace('.', '').isdigit():
            value = float(value)

        inj.bind_instance(getty.Config, with_annotation = name, to = value)

    # Connect to regression database
    db = sqlite3.connect(opts.regression_database, isolation_level = None)

    # Obtain a clusterer instance
    clusterer = inj.get_instance(
        vinz.cluster.Clusterer, annotation = opts.clusterer_type)

    # If given, obtain feature-transform instance
    feature_transform = None
    if opts.feature_transform:
        feature_transform = inj.get_instance(
            vinz.feature_transform.FeatureTransform,
            annotation = opts.feature_transform)

    coord = ClusteringCoordinator(clusterer, feature_transform, inj)

    # {sample-uid: [class-id]}
    sample_classes = {}
    # {class-id: # of samples}
    class_sample_size = {}

    # Load classes & samples
    for class_id in opts.classes:

        class_sample_size[class_id] = 0
        coord.add_cluster(class_id)

        for uid, type, weight, attributes in db.execute("""
            select uid, type, weight, attributes from sample join sample_class on
                uid = sample_uid and class_name = ?""", (class_id,)):

            uid = str(uid)
            if uid not in sample_classes:
                sample_classes[uid] = []
                attributes = simplejson.loads(attributes)

                # TODO: More principled initialization
                if class_sample_size[class_id] < 2:
                    s_prob = {class_id: (1.0, True)}
                else:
                    s_prob = {}

                coord.add_sample(uid, type, weight, s_prob, **attributes)

            sample_classes[uid].append(class_id)
            class_sample_size[class_id] += 1.0

        assert class_sample_size[class_id], "No samples w/ class %s" % class_id

    # Run regression iterations
    for i in xrange(N_ITERATIONS):
        stats = iterate_and_report(coord, sample_classes, class_sample_size)
        print simplejson.dumps(stats)

    return


opt_parser = optparse.OptionParser()

opt_parser.add_option('-d', '--database', action = 'store',
    type = 'string', dest = 'regression_database')
opt_parser.add_option('-c', '--clusterer', action = 'store',
    type = 'string', dest = 'clusterer_type',
    default = 'SparseGaussEmClusterer')
opt_parser.add_option('-f', '--feature-transform', action = 'store',
    type = 'string', dest = 'feature_transform')
opt_parser.add_option('-n', '--class-name', action = 'append',
    type = 'string', dest = 'classes', default = [])
opt_parser.add_option('-o', '--config-override', action = 'append',
    type = 'string', dest = 'config_overrides', default = [])

opts, _ = opt_parser.parse_args()

if not opts.regression_database:
    opt_parser.error("Regression database must be provided")
if not opts.classes or len(opts.classes) < 2:
    opt_parser.error("At least two class-names are required")

main(opts)

