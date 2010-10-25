
from vinz.feature_transform import PCAProjTransform
from vinz.features import DenseFeatures, SparseFeatures
import unittest

class TestPCAProjectorTransform(unittest.TestCase):

    def test_dense(self):

        # principal components of this dataset
        #  are [-0.6779, -0.7352] (eigen 1.28) & 
        #      [-0.7352, 0.6779]  (eigen 0.05)
        # mean is {1: 1.81, 5: 1.91}

        dataset = [
            (2.5, 2.4),
            (0.5, 0.7),
            (2.2, 2.9),
            (1.9, 2.2),
            (3.1, 3.0),
            (2.3, 2.7),
            (2, 1.6),
            (1, 1.1),
            (1.5, 1.6),
            (1.1, 0.9),
        ]

        # Project features only onto the larger component
        pca_transform = PCAProjTransform(2)

        pca_transform.train_transform(
            [DenseFeatures(i) for i in dataset], [[] for i in dataset])

        # test vector along first principal component
        v = pca_transform.transform(
            DenseFeatures([2.4878, 2.6452])).as_list()

        self.assertTrue( abs(v[1] / v[0]) < 0.001)

        # test vector along second principal component
        v = pca_transform.transform(
            DenseFeatures([1.075, 2.588])).as_list()

        self.assertTrue( abs(v[0] / v[1]) < 0.001)

    def test_sparse(self):

        # principal components of this dataset
        #  are [-0.6779, -0.7352] (eigen 1.28) & 
        #      [-0.7352, 0.6779]  (eigen 0.05)
        # mean is {1: 1.81, 5: 1.91}

        dataset = [
            {1: 2.5, 5: 2.4, 0: 0.0, 2: 0.0},
            {1: 0.5, 5: 0.7},
            {1: 2.2, 5: 2.9},
            {1: 1.9, 5: 2.2, 3: 0.0},
            {1: 3.1, 5: 3.0},
            {1: 2.3, 5: 2.7, 2: 0.0},
            {1: 2,   5: 1.6},
            {1: 1,   5: 1.1, 0: 0.0},
            {1: 1.5, 5: 1.6, 3: 0.0},
            {1: 1.1, 5: 0.9},
        ]

        # Project features only onto the larger component
        pca_transform = PCAProjTransform(2)

        pca_transform.train_transform(
            [SparseFeatures(i) for i in dataset], [[] for i in dataset])

        # test vector along first principal component
        v = pca_transform.transform(
            SparseFeatures({1: 2.4878, 5: 2.6452, 3: 0.0})).as_list()

        self.assertTrue( abs(v[1] / v[0]) < 0.001)

        # test vector along second principal component
        v = pca_transform.transform(
            SparseFeatures({1: 1.075, 5: 2.588, 0: 0.0})).as_list()

        self.assertTrue( abs(v[0] / v[1]) < 0.001)

