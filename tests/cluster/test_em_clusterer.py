
import cluster
from featurize import TfVectorFeaturizer, TfIdfVectorFeaturizer
import unittest
import random

class CoordFeaturizer(cluster.dense_vectorspace_featurizer):
    def _py_featurize(self, doc_sample):
        res = [float(i) for i in doc_sample.content.split()]
        return res

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
        clus = self._cluster(clusters, docs, 25)

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

        clusters = ['c1', 'c2', 'c3']

        docs = {
            'd1': ('a b', {'c1': 1.01, 'c2': 1.0, 'c3': 1.0}),
            'd2': ('c d', {'c1': 1.0, 'c2': 1.01, 'c3': 1.0}),
            'd3': ('f e', {'c1': 1.0, 'c2': 1.0, 'c3': 1.01}),
        }
        clus = self._cluster(clusters, docs, 50)

        d1 = clus.get_sample_probabilities('d1')
        d2 = clus.get_sample_probabilities('d2')
        d3 = clus.get_sample_probabilities('d3')

        self.assertTrue(d1['c1'] > d2['c1']+0.1 and d1['c1'] > d3['c1']+0.1)
        self.assertTrue(d2['c2'] > d1['c2']+0.1 and d2['c2'] > d3['c2']+0.1)
        self.assertTrue(d3['c3'] > d1['c3']+0.1 and d3['c3'] > d2['c3']+0.1)

    def test_synthetic_gaussian(self):

        random.seed(32)
        clusters = ['a', 'b', 'c']
        docs = {}

        for cls, cnt in enumerate([100, 50, 25]):

            for i in xrange(cnt):
                docs['d_c%d_%d' % (cls, i)] = (
                    '%s %s %s' % (
                        random.normalvariate(5 if cls == 0 else 0, 1.5),
                        random.normalvariate(4 if cls == 1 else 0, 1),
                        random.normalvariate(2 if cls == 2 else 0, 0.5),
                    ), dict((c, random.random()) for c in clusters))

        clus = cluster.em_cluster()

        for c_uid in clusters:
            clus.add_cluster(c_uid,
                cluster.gaussian_estimator(CoordFeaturizer()))

        for d_uid, (d_txt, d_mem) in docs.iteritems():
            clus.add_sample(
                cluster.document_sample(d_uid, '', d_txt), d_mem, {})

        for i in xrange(20):
            clus.iterate()

        counts = {}
        for doc in sorted(docs):
            cls = doc.split('_')[1]

            m = max((j,i) for (i,j) in \
                clus.get_sample_probabilities(doc).items())

            key = '%s-%s' % (cls, m[1])
            counts[key] = counts.get(key, 0) + 1

        self.assertEquals(counts, {'c1-b': 50, 'c0-b': 1, 'c0-c': 99, 'c2-a': 25})

    def test_synthetic_vectorspace(self):
        
        random.seed(32)
        clusters = ['a', 'b', 'c']
        docs = {}

        for cls, cnt in enumerate([100, 50, 25]):

            for i in xrange(cnt):
                word_cnt = random.randint(8, 100)

                terms = []
                for j in xrange(word_cnt * 0.2):
                    terms.append(int(random.expovariate(1.0 / 10)) * 3 + cls)
                for j in xrange(word_cnt * 0.8):
                    terms.append(int(random.expovariate(1.0 / 30)))

                docs['d_c%d_%d' % (cls, i)] = (
                    ' '.join(str(i) for i in terms),
                    dict((c, random.random()) for c in clusters))

        feat = TfVectorFeaturizer()

        clus = cluster.em_cluster()

        for c_uid in clusters:
            clus.add_cluster(c_uid,
                cluster.unigram_lm_estimator(feat))

        for d_uid, (d_txt, d_mem) in docs.iteritems():

            if d_uid == 'd_c0_0':
                clus.add_sample(
                    cluster.document_sample(d_uid, '', d_txt), d_mem, {'a': 1.0})
            elif d_uid == 'd_c1_0':
                clus.add_sample(
                    cluster.document_sample(d_uid, '', d_txt), d_mem, {'b': 1.0})
            elif d_uid == 'd_c2_0':
                clus.add_sample(
                    cluster.document_sample(d_uid, '', d_txt), d_mem, {'c': 1.0})
            else:
                clus.add_sample(
                    cluster.document_sample(d_uid, '', d_txt), d_mem, {})

        for i in xrange(50):
            clus.iterate()

        counts = {}
        for d_uid, (d_txt, d_mem) in sorted(docs.items()):
            cls = d_uid.split('_')[1]

            print "%s %r:\r\n\t%r" % (d_uid, d_txt, clus.get_sample_probabilities(d_uid))

            m = max((j,i) for (i,j) in \
                clus.get_sample_probabilities(d_uid).items())

            key = '%s-%s' % (cls, m[1])
            counts[key] = counts.get(key, 0) + 1

        print counts


    def _cluster(self, clusters, docs, count, feat = None):

        if not feat:
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

