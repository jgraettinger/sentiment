
from cluster.feature_selection import InformationGainSelector
from cluster import sparse_features
import random
import unittest

class TestInformationGainSelector(unittest.TestCase):

    def test_simple(self):

        self.igain = InformationGainSelector(0, 0.7, 3, 0.02)

        self.igain.add_observation(
            sparse_features({1: 1, 2: 1}), [0.9, 0.5, 0.1])
        self.igain.add_observation(
            sparse_features({2: 1, 3: 1}), [0.1, 0.9, 0.5])
        self.igain.add_observation(
            sparse_features({2: 1, 3: 1}), [0.5, 0.1, 0.9])

        features = [i[0] for i in self.igain.select_features()]

        self.assertEquals(len(features), 2)
        self.assertEquals(features[0], 1)
        self.assertEquals(features[1], 3)
        return

    def test_backoff(self):

        igain = InformationGainSelector(0, 1.1, 3, 0.01)

        igain.add_observation(
            sparse_features({1: 1, 3: 1,}), [1, 0])
        igain.add_observation(
            sparse_features({1: 0.5, 2: 1,}), [0, 1])

        norm = sum(i[1] for i in igain.select_features())
        print [(k, v / norm) for k, v in igain.select_features()]
        igain.reset()

        igain.add_observation(
            sparse_features({1: 1, 3: 1,}), [0.8, 0.2])
        igain.add_observation(
            sparse_features({1: 0.5, 2: 1,}), [0.2, 0.8])

        norm = sum(i[1] for i in igain.select_features())
        print [(k, v / norm) for k, v in igain.select_features()]
        igain.reset()

        igain.add_observation(
            sparse_features({1: 1, 3: 1,}), [0.51, 0.49])
        igain.add_observation(
            sparse_features({1: 0.5, 2: 1,}), [0.49, 0.51])

        norm = sum(i[1] for i in igain.select_features())
        print [(k, v / norm) for k, v in igain.select_features()]
        igain.reset()

    def test_regression(self):

        random.seed(42)
        self.igain = InformationGainSelector(0, 0.7, 100, 0.0)

        for i in xrange(100):

            feat1 = dict((int(random.normalvariate(24.0, 2)), 1
                ) for i in xrange(5))
            feat2 = dict((int(random.normalvariate(26.0, 2)), 1
                ) for i in xrange(5))

            self.igain.add_observation(
                sparse_features(feat1), [0.7, 0.3])
            self.igain.add_observation(
                sparse_features(feat2), [0.3, 0.7])

        self.assertEquals(
            [28, 21, 29, 27, 22, 23, 20],
            [i[0] for i in self.igain.select_features()])
        return
