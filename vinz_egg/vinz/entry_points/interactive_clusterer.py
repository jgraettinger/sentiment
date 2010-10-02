
import sys
import simplejson
import getty

import vinz.module
import vinz.cluster
import vinz.feature_transform

from vinz.clustering_coordinator import ClusteringCoordinator

if len(sys.argv) != 2:
    print sys.stderr, "Usage: %s <clustering-configuration>" % sys.argv[0]

inj = vinz.module.Module.configure(getty.Injector())

clusterer = inj.get_instance(
    vinz.cluster.Clusterer, with_annotation = sys.argv[0])
feature_transform = inj.get_instance(
    vinz.feature_transform.FeatureTransform, with_annotation = sys.argv[0])

coord = ClusteringCoordinator(clusterer, feature_transform, inj)

while True:

    cmd, arg = sys.stdin.readline().strip().split(1)

    if   cmd == 'add_sample':
        coord.add_sample(**simplejson.loads(arg))
        print 'OK'
    elif cmd == 'drop_sample':
        coord.drop_sample(arg)
        print 'OK'
    elif cmd == 'add_cluster':
        coord.add_cluster(arg)
        print 'OK'
    elif cmd == 'drop_cluster':
        coord.drop_cluster(arg)
        print 'OK'
    elif cmd == 'anneal_cluster':
        coord.anneal_cluster(arg)
        print 'OK'
    elif cmd == 'expect_and_maximize':
        print coord.expect_and_maximize()
    elif cmd == 'sample_state':
        for state in coord.sample_state():
            print simplejson.dumps(state)
        print 'END'

