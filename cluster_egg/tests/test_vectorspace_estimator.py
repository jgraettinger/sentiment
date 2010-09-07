
import cluster
from featurize import TfVectorFeaturizer
import unittest

class TestVectorspaceEstimator(unittest.TestCase):

    def test_sparse_estimator(self):
        doc_probs = [
            ('foo bar one',   0.057886989572649789),
            ('bar baz two',   0.057886989572649789),
            ('baz foo three', 0.88422602085470037),
        ]
        doc_probs = [
            (cluster.document_sample(i, '', i), j) for i,j in doc_probs]

        feat = TfVectorFeaturizer()
        est = cluster.sparse_vectorspace_estimator(feat, -1)

        print feat.featurize(doc_probs[0][0])

        est.reset_estimator()
        for doc, prob in doc_probs:
            est.add_sample_probability( feat.featurize(doc), prob)

        est.prepare_estimator()

        for doc, prob in doc_probs:
            print est.estimate_sample( feat.featurize(doc))


    def test_foo(self):
        doc_probs = [
            ('aaa bbb', 0.51),
            ('aaa bbb c', 0.5),
            ('ddd eee c', 0.5),
            ('ddd eee', 0.5),
        ]

        doc_probs = [
            (cluster.document_sample(i, '', i), j) for i,j in doc_probs]

        feat = TfVectorFeaturizer()
        est = cluster.sparse_vectorspace_estimator(feat, -1)

        print feat.featurize(doc_probs[0][0])

        est.reset_estimator()
        for doc, prob in doc_probs:
            est.add_sample_probability( feat.featurize(doc), prob)

        est.prepare_estimator()

        for doc, prob in doc_probs:
            print doc.content, est.estimate_sample( feat.featurize(doc))
