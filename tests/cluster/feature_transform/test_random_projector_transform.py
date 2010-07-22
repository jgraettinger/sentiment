
from cluster.feature_transform import RandomProjTransform
from cluster.features import DenseFeatures, SparseFeatures
import unittest
import random
import math

class TestRandomProjectorTransform(unittest.TestCase):

    def test_sparse_dense_equality(self):

        rand_transform = RandomProjTransform(15)

        l1 = rand_transform.transform(
            DenseFeatures([i for i in xrange(100)])).as_list()

        l2 = rand_transform.transform(
            SparseFeatures(dict((i, i) for i in xrange(100)))).as_list()

        self.assertEquals(l1, l2)
        return

    def test_dist(self):

        rand_transform = RandomProjTransform(50)

        orig_vecs = [
            [random.random() for i in xrange(300)] \
                for i in xrange(300)]

        proj_vecs = [
            rand_transform.transform(DenseFeatures(v)).as_list() \
                for v in orig_vecs]

        # expected reduction in euclidean distance
        factor = math.sqrt(300.0 / 50.0)

        for i in xrange(300):
            
            l_orig = math.sqrt( sum(
                t * t for t in orig_vecs[i]))

            l_proj = math.sqrt( sum(
                t * t for t in proj_vecs[i]))

            print "%s vs %s" % (l_orig, l_proj)

        """
        for i in xrange(300):
            for j in xrange(300):
                if i == j: continue

                ovec1 = orig_vecs[i]
                ovec2 = orig_vecs[j]

                pvec1 = proj_vecs[i]
                pvec2 = proj_vecs[j]

                d_orig = math.sqrt( sum(
                    (t1 - t2) ** 2 for (t1, t2) in zip(ovec1, ovec2)))
                d_proj = math.sqrt( sum(
                    (t1 - t2) ** 2 for (t1, t2) in zip(pvec1, pvec2)))

                print "%s vs %s" % (d_orig, d_proj)
        """ 
