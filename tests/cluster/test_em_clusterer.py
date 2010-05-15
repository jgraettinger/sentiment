
import cluster
from featurize import TfVectorFeaturizer
import unittest

class TestEMClusterer(unittest.TestCase):
   
    def test_stablizes(self):

        clusters = ['c1', 'c2', 'c3']

        docs = {
            'd1': ('foo bar one',   {'c1': 1.0}),
            'd2': ('foo bar two',   {'c1': 1.0}),
            'd3': ('bar baz three', {'c2': 1.0}),
            'd4': ('bar baz four',  {'c2': 1.0}),
            'd5': ('baz foo five',  {'c3': 1.0}),
            'd6': ('baz foo siz',   {'c3': 1.0}),
        }
        clus = self._cluster(clusters, docs, 50)

        res = round(1.0 / len(clusters), 8)

        [self.assertEquals(round(p, 8), res) for \
            d in docs for (c, p) in clus.get_sample_probabilities(d).items()]

        return
    
    def test_local_pull(self):
        
        clusters = ['c1', 'c2']
 
        docs = {
            'd1': ('aaa bbb', {'c1': 1.1, 'c2': 1.0}),
            'd2': ('bbb ccc', {'c1': 1.0, 'c2': 1.0}),
            'd3': ('ccc ddd', {'c1': 1.0, 'c2': 1.0}),
            'd4': ('ddd eee', {'c1': 1.0, 'c2': 1.1}),
        }
        clus = self._cluster(clusters, docs, 1)

        d2 = clus.get_sample_probabilities('d1')
        d3 = clus.get_sample_probabilities('d2')
        self.assertTrue(d2['c1'] > d2['c2'])
        self.assertTrue(d3['c1'] < d3['c2'])
        return

    def test_destabalizes(self):
        
        clusters = ['c1', 'c2']

        docs = {
            'd1': ('aaa a bbb ccc', {'c1': 1.1, 'c2': 1.0}),
            'd2': ('aaa b bbb ccc', {'c1': 1.0, 'c2': 1.0}),
            'd3': ('ddd c eee ccc', {'c1': 1.0, 'c2': 1.0}),
            'd4': ('ddd d eee ccc', {'c1': 1.0, 'c2': 1.1}),
        }
        clus = self._cluster(clusters, docs, 100)

        for d in sorted(docs):
            print clus.get_sample_probabilities(d)
    
    def _cluster(self, clusters, docs, count):

        feat = TfVectorFeaturizer()
        clus = cluster.em_cluster()

        for c_uid in clusters:
            clus.add_cluster(c_uid,
                cluster.sparse_vectorspace_estimator(feat, -1))

        for d_uid, (d_txt, d_mem)  in docs.items():
            clus.add_sample(
                cluster.document_sample(d_uid, '', d_txt),
                dict((c_uid, d_mem.get(c_uid, 0.0)) for c_uid in clusters), {})

        for i in xrange(count):
            clus.iterate()

        return clus

