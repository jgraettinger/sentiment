
import math
import unittest
from vinz.features import SparseFeatures
from vinz.estimation import InnerProductEstimator

class TestInnerProductEstimator(unittest.TestCase):

    def test_mean(self):

        est = InnerProductEstimator()

        feat = {1: 0.1, 2: 0.7, 5: 1.6, 12: 3.2, 18: 0.9}

        norm_feat = 1.0 / math.sqrt(sum(v * v for v in feat.values()))
        norm_feat = dict((k, v * norm_feat) for k, v in feat.items())

        est = InnerProductEstimator()
        est.add_observation(SparseFeatures(feat), 1.0, False)
        est.prepare_estimator()

        # sanity check mean normalization
        self.assertEquals(norm_feat, est.get_mean())

        est.reset()

        est.add_observation(SparseFeatures(
            {1: 0.1, 2: 0.7, 3: 0.3}), 1.0, False)
        est.add_observation(SparseFeatures(
            {4: 0.1, 5: 0.3, 2: 0.7}), 0.5, False)
        est.prepare_estimator()

        mean = est.get_mean()

        self.assertEquals([1, 2, 3, 4, 5], sorted(mean.keys()))

        # Check that probability is factored into mean contribution
        self.assertEquals(mean[1], mean[4] * 2)
        self.assertEquals(mean[3], mean[5] * 2)

    def test_estimation(self):

        angle = math.pi / 8
        feat1 = {1: math.cos( angle) * 2, 2: math.sin( angle) * 2}
        feat2 = {1: math.cos(-angle) * 3, 2: math.sin(-angle) * 3}

        est = InnerProductEstimator()
        est.add_observation(SparseFeatures(feat1), 1.0, False)
        est.add_observation(SparseFeatures(feat2), 1.0, False)
        est.prepare_estimator()

        # Mean is along axis 1
        self.assertEquals({1: 1, 2: 0}, est.get_mean())

        # Validate inner product
        self.assertAlmostEquals(est.inner_product(
            SparseFeatures(feat1)), math.cos(angle))
        self.assertAlmostEquals(est.inner_product(
            SparseFeatures(feat2)), math.cos(angle))

        # Validate distance monotonically increases
        d1 = est.distance(SparseFeatures(
            {1: math.cos(angle *  0.25) * 2, 2: math.sin(angle *  0.25) * 2}))
        d2 = est.distance(SparseFeatures(
            {1: math.cos(angle * -0.50) * 3, 2: math.sin(angle * -0.50) * 3}))
        d3 = est.distance(SparseFeatures(
            {1: math.cos(angle * -0.75) * 4, 2: math.sin(angle * -0.75) * 4}))
        d4 = est.distance(SparseFeatures(
            {1: math.cos(angle) * 5, 2: math.sin(angle) * 5}))

        self.assertTrue(d1 < d2)
        self.assertTrue(d2 < d3)
        self.assertTrue(d3 < d4)

        # Symmetric inputs means they are one std-dev away
        self.assertAlmostEquals(
            est.distance(SparseFeatures(feat1)), est.get_std_deviation())

        # Check spherical gaussian estimation
        sq_std_dev = est.get_std_deviation() ** 2
        sq_dist = est.distance(SparseFeatures(feat1)) ** 2

        expect = math.exp(-1.0 * sq_dist / (2 * sq_std_dev))
        expect = expect / math.sqrt(2 * math.pi * sq_std_dev)
        
        self.assertAlmostEquals(expect, est.estimate(SparseFeatures(feat1)))

