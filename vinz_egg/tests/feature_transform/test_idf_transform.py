
from vinz.features import DenseFeatures, SparseFeatures
from vinz.feature_transform import IdfTransform
import unittest

class TestIdfTransform(unittest.TestCase):

    def test_dense(self):

        dataset = [[1.0] * 10 for i in xrange(10)]
        for i in xrange(10):
            for j in xrange(i):
                dataset[i][j] = 0.0

        idf_transform = IdfTransform(0.89, 2)

        idf_transform.train_transform(
            [DenseFeatures(i) for i in dataset], [[] for i in dataset])

        self._common(idf_transform.transform(
            DenseFeatures([1.0] * 10)).as_dict())

    def test_sparse(self):

        dataset = [[1.0] * 10 for i in xrange(10)]
        for i in xrange(10):
            for j in xrange(i):
                dataset[i][j] = 0.0

        for i in xrange(10):
            dataset[i] = dict(enumerate(dataset[i]))

        idf_transform = IdfTransform(0.89, 2)

        idf_transform.train_transform(
            [SparseFeatures(i) for i in dataset], [[] for i in dataset])

        self._common(idf_transform.transform(
            SparseFeatures(dict((i, 1.0) for i in xrange(10)))).as_dict())

    def _common(self, v):

        # 0 is filtered (< 2 occurrences)
        self.assertTrue(0 not in v)
        # 8, 9 are filtered (occurs in > 89% of samples)
        self.assertTrue(8 not in v)
        self.assertTrue(9 not in v)

        # Idf weighting
        self.assertTrue(v[1] > v[5])
        self.assertTrue(v[5] > v[7])

        # Unit length
        self.assertEquals(round(sum(i * i for i in v.values()), 4), 1.0)

