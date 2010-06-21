import cluster
import unittest
import common
import random

class TestInformationGainSelector(unittest.TestCase):
    
    def setUp(self):
        self.igain = cluster.information_gain_selector()
    
    def test_foo(self):

        self.igain.add_observation(
            cluster.sparse_features({1: 1, 2: 1}), [0.9, 0.1])
        self.igain.add_observation(
            cluster.sparse_features({2: 1, 3: 1}), [0.1, 0.9])

        print self.igain.get_information_gain()

    def test_bar(self):
        random.seed(32)

        for i in xrange(10000):
            self.igain.add_observation(
                cluster.sparse_features( dict(
                    (int(random.normalvariate(24, 3)), 1) for i in xrange(5))),
                    [0.9, 0.1]
            )
            
            self.igain.add_observation(
                cluster.sparse_features( dict(
                    (int(random.normalvariate(26, 3)), 1) for i in xrange(5))),
                    [0.1, 0.9]
            )
        
        print sorted(self.igain.get_information_gain().items())
