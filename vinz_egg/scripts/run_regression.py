
import sys
import math
import simplejson
import sqlite3
import getty

import vinz.module
from vinz.cluster import Clusterer
from vinz.feature_transform import FeatureTransform
from vinz.clustering_coordinator import ClusteringCoordinator

if len(sys.argv) < 6:
    print "Usage: %s <regression-db> <clusterer-type> <feature-transform-type>"\
        " <class-id> <class-id> ..." % sys.argv[0]
    sys.exit(-1)

db = sqlite3.connect(sys.argv[1], isolation_level = None)

inj = vinz.module.Module().configure(getty.Injector())
clusterer = inj.get_instance(
    vinz.cluster.Clusterer, annotation = sys.argv[2])
feature_transform = inj.get_instance(
    vinz.feature_transform.FeatureTransform, annotation = sys.argv[3])
coord = ClusteringCoordinator(clusterer, feature_transform, inj)

# {sample-uid: [class-id]}
sample_classes = {}
# {class-id: # of samples}
class_sample_size = {}

for class_id in sys.argv[4:]:

    print "Loading samples for class %s" % class_id
    class_sample_size[class_id] = 0

    coord.add_cluster(class_id)

    for uid, type, weight, attributes in db.execute("""
        select uid, type, weight, attributes from sample join sample_class on
            uid = sample_uid and class_name = ?""", (class_id,)):

        uid = str(uid)

        if uid not in sample_classes:
            sample_classes[uid] = []
            attributes = simplejson.loads(attributes)

            if class_sample_size[class_id] < 2:
                s_prob = {class_id: (1.0, True)}
            else:
                s_prob = {}

            coord.add_sample(uid, type, weight, s_prob, **attributes)

        sample_classes[uid].append(class_id)
        class_sample_size[class_id] += 1.0


#for uid in sample_classes:
#    print coord._clusterer.get_sample_probabilities(uid)

def iterate_and_report():

    global sample_classes, class_sample_size, coord

    entropy = coord.expect_and_maximize()

    total_class_prob = dict((k, 0) for k in class_sample_size)
    class_prec = dict((k, 0) for k in class_sample_size)
    class_recall = dict((k, 0) for k in class_sample_size)

    for uid, likelihood, class_probs in coord.sample_state():

        for class_id in sample_classes[uid]:
            # aggregate precision & recall
            class_prec[class_id] += class_probs[class_id][0]
            class_recall[class_id] += class_probs[class_id][0]

        # aggregate total class probability
        for class_id, (prob, is_hard) in class_probs.items():
            total_class_prob[class_id] += prob

    for class_id, n_samples in class_sample_size.items():
        # normalize precision by total class prob
        class_prec[class_id] /= total_class_prob[class_id]
        # normalize recall by # of class samples
        class_recall[class_id] /= n_samples 

    return entropy, class_prec, class_recall


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


N_ITERATIONS = 200

for i in xrange(N_ITERATIONS):

    entropy, prec, recall = iterate_and_report()

    print prec, recall



