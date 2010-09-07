
from cluster.feature_transform import CompactTransform
from cluster.features import SparseFeatures, DenseFeatures
import unittest

class TestCompactTransform(unittest.TestCase):

    def _generator():

        for i in xrange(1000):
            if not i % 5: continue

            if i % 3:
                yield (i, 1)

            yield (i, i)

    def test_simple(self):

        c_transform = CompactTransform()

        c_transform.train(_generator())

        c_transform.transform( DenseFeatures(
            [0, 1, 2, 3, 4, 5]

        # Project features only onto the larger component
        pca_transform = PCAProjTransform(1)

        pca_transform.train_transform(
            (DenseFeatures(i), []) for i in dataset)

        # should be almost -1 (inverse of principal component)
        v1 = pca_transform.transform(
            DenseFeatures([0.68, 0.74])).as_list()[0]

        self.assertTrue( abs(v1 + 1) < 0.01)

        # should be almost 0 (orthogonal to principal component)
        v0 = pca_transform.transform(
            DenseFeatures([-0.74, 0.68])).as_list()[0]

        self.assertTrue( abs(v0) < 0.01)

