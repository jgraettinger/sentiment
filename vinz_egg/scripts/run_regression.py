
import sys
import math
import sqlite3
import getty

import vinz.module
from vinz.cluster import Clusterer
from vinz.feature_transform import FeatureTransform
from vinz.clustering_coordinator import ClusteringCoordinator

if len(sys.argv) < 6:
    print "Usage: %s <regression-db> <clusterer-type> <feature-transform-type>"
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

for class_id in sys.argv[4:]:

    print "Loading samples for class %s" % class_id

    for sample_uid, sample_type, weight, attributes in db.execute("""
        select uid, type, weight, attributes from sample join sample_class on
            uid = sample_uid and class_name = ?""", (class_id,)):

        if sample_uid not in sample_classes:
            sample_classes[sample_uid] = []
            coord.add_sample(sample_uid, sample_type, weight, **attributes)

        sample_classes[sample_uid].append(class_id)



class_ids = sys.argv[2:]




