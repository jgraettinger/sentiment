
from vinz.feature_transform import RandomProjTransform
from vinz.features import DenseFeatures, SparseFeatures
import unittest
import random
import math

class TestRandomProjectorTransform(unittest.TestCase):

    def setUp(self):
        random.seed(32)

    def test_sparse_dense_equality(self):

        rand_transform = RandomProjTransform(15)

        l1 = rand_transform.transform(
            DenseFeatures([i for i in xrange(100)])).as_list()

        l2 = rand_transform.transform(
            SparseFeatures(dict((i, i) for i in xrange(100)))).as_list()

        self.assertEquals(l1, l2)
        return

    def test_dist(self):

        n_input_features = 300
        n_output_features = 30
        n_samples = 100
        rand_transform = RandomProjTransform(n_output_features)

        orig_vecs = [
            [1.0 - 2.0 * random.random() for i in xrange(n_input_features)] \
                for i in xrange(n_samples)]

        # unit length
        for vec in orig_vecs:
            norm = 1.0 / math.sqrt( sum(v*v for v in vec))
            for i in xrange(len(vec)):
                vec[i] *= norm

        proj_vecs = [
            rand_transform.transform(DenseFeatures(v)).as_list() \
                for v in orig_vecs]

        print proj_vecs[1]
        print rand_transform.transform(DenseFeatures(orig_vecs[1])).as_list()

        mat_r = rand_transform.get_mat_r()

        # Mean & deviation of K row-vecs
        for k in xrange(n_output_features):
            print "vector ", k
            vals = [mat_r[(i, k)] for i in xrange(n_input_features)]
            mean = sum(vals) / len(vals)
            print "\tMean: ", mean
            std_dev = math.sqrt( sum((v - mean) ** 2 for v in vals) / len(vals))
            print "\tStd-dev: ", std_dev

        # expected reduction in euclidean distance
        factor = math.sqrt(n_output_features / float(n_input_features))

        """
        for i in xrange(n_samples):
            
            l_orig = math.sqrt( sum(
                t * t for t in orig_vecs[i]))

            l_proj = math.sqrt( sum(
                t * t for t in proj_vecs[i]))

            print "%s vs %s" % (l_orig, factor * l_proj)
        """

         
        for i in xrange(n_samples):
            for j in xrange(n_samples):
                if i == j: continue

                ovec1 = orig_vecs[i]
                ovec2 = orig_vecs[j]

                pvec1 = proj_vecs[i]
                pvec2 = proj_vecs[j]

                d_orig = math.sqrt( sum(
                    ((t1 - t2) ** 2.0) for (t1, t2) in zip(ovec1, ovec2)))
                d_proj = math.sqrt( sum(
                    ((t1 - t2) ** 2.0) for (t1, t2) in zip(pvec1, pvec2)))

                print "%s vs %s" % (d_orig, factor * d_proj)
        

