
import cluster
import unittest
import random

class CoordFeaturizer(cluster.dense_vectorspace_featurizer):
    def _py_featurize(self, doc_sample):
        res = [float(i) for i in doc_sample.content.split()]
        return res

class TestGaussianEstimator(unittest.TestCase):

    def setUp(self):
        self.feat = CoordFeaturizer()
        self.est = cluster.gaussian_estimator(self.feat)
        self._id = 0
        random.seed(32)
        return

    def test_single(self):

        for i in xrange(500):
            self.add_sample(
                [random.normalvariate(5, 0.7)], random.random())

        self.est.prepare_estimator()

        # distribution is centered around mean, with
        #  (roughly) appropriate drop-off at various
        #  std-deviations from the mean
        self.assertEquals( 0.60,
            round(self.est_sample([5]), 2))
        self.assertEquals( 0.32,
            round(self.est_sample([5 - 0.7]), 2))
        self.assertEquals( 0.08,
            round(self.est_sample([5 + 2 * 0.7]), 2))

    def test_joint(self):

        for i in xrange(500):
            self.add_sample([
                random.normalvariate(5, 0.7),
                random.normalvariate(2, 3),
                random.normalvariate(10, 2)])

        self.est.prepare_estimator()

        # distribution peaks here
        self.assertEquals( 0.016,
            round(self.est_sample([5, 2, 10]), 3))

        # shifting any of the dimensions by their std deviation
        #  causes (roughly) the same amount of probability drop-off
        self.assertEquals( 0.009,
            round(self.est_sample([5.7, 2, 10]), 3))
        self.assertEquals( 0.010,
            round(self.est_sample([5, 5, 10]), 3))
        self.assertEquals( 0.009,
            round(self.est_sample([5, 2, 8]), 3))

    def test_weight(self):

        for i in xrange(500):
            self.add_sample([random.normalvariate(10, 5)], 1.0)
            self.add_sample([random.normalvariate(0,  1)], 0.2)

        self.est.prepare_estimator()

        # distribution is centered around 8
        p7 = self.est_sample([7])
        p8 = self.est_sample([8])
        p9 = self.est_sample([9])

        self.assertTrue(p8 > p7)
        self.assertTrue(p8 > p9)

    def add_sample(self, vals, weight = 1.0):
        self._id += 1
        self.est.add_sample_probability(
            self.feat.featurize(
                cluster.document_sample(
                    str(self._id), '', ' '.join(str(i) for i in vals))
            ), weight
        )

    def est_sample(self, vals):
        self._id += 1
        return self.est.estimate_sample(
            self.feat.featurize(
                cluster.document_sample(
                    str(self._id), '', ' '.join(str(i) for i in vals))
            )
        )

