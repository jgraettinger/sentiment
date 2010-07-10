
from cluster.estimation import GaussianEstimator
from cluster import dense_features
import unittest
import math
import random

class TestGaussianEstimator(unittest.TestCase):

    def test_single(self):

        random.seed(32)
        est = GaussianEstimator(3)

        # rotate around z-axis by 45 degrees
        #  In rotated coord-space, x & y are
        #  correlated, but z is independent
        angle = math.pi / 4 

        m = [
            [math.cos(angle), -math.sin(angle), 0],
            [math.sin(angle),  math.cos(angle), 0],
            [0,                0,               1],
        ]

        for i in xrange(500):

            x = random.normalvariate(5,  10)
            y = random.normalvariate(1,  1)
            z = random.normalvariate(10, 0.1)

            n_x = x * m[0][0] + y * m[0][1] + z * m[0][2]
            n_y = x * m[1][0] + y * m[1][1] + z * m[1][2]
            n_z = x * m[2][0] + y * m[2][1] + z * m[2][2]

            est.add_observation(
                dense_features([n_x, n_y, n_z]), 1)

        est.prepare_estimator()

        # Check co-var matrix & determinant
        
        est_pos = []
        for i in xrange(500):

            x = random.normalvariate(5,  10)
            y = random.normalvariate(1,  1)
            z = random.normalvariate(10, 0.1)

            n_x = x * m[0][0] + y * m[0][1] + z * m[0][2]
            n_y = x * m[1][0] + y * m[1][1] + z * m[1][2]
            n_z = x * m[2][0] + y * m[2][1] + z * m[2][2]

            est_pos.append( est.estimate(
                dense_features([n_x, n_y, n_z])))

        angle = math.pi / 3 

        m = [
            [math.cos(angle), -math.sin(angle), 0],
            [math.sin(angle),  math.cos(angle), 0],
            [0,                0,               1],
        ]
        est_null = []
        for i in xrange(500):

            x = random.normalvariate(5,  10)
            y = random.normalvariate(1,  1)
            z = random.normalvariate(10, 0.1)

            n_x = x * m[0][0] + y * m[0][1] + z * m[0][2]
            n_y = x * m[1][0] + y * m[1][1] + z * m[1][2]
            n_z = x * m[2][0] + y * m[2][1] + z * m[2][2]

            est_null.append( est.estimate(
                dense_features([n_x, n_y, n_z])))

        self.assertTrue(sum(est_pos) > sum(est_null))
        return

