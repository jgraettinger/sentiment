import math
from cluster import sparse_vectorspace_featurizer
from cluster import dense_vectorspace_featurizer

class TfIdfVectorFeaturizer(sparse_vectorspace_featurizer):

    def __init__(self):
        sparse_vectorspace_featurizer.__init__(self)
        self._itab = {}
        self._df = {}

    def accumulate_doc_frequency(self, content):
        tokens = set(content.encode('utf8').split())

        for tok in tokens:
            t_id = self._itab.setdefault(tok, len(self._itab))
            self._df[t_id] = 1 + self._df.get(t_id, 0)

    def _py_featurize(self, doc_sample):
        tokens = doc_sample.content.split()

        feat = {}
        for tok in tokens:
            t_id = self._itab[tok]

            feat[t_id] = feat.get(t_id, 0) + (1.0 / self._df[t_id])

        norm = 1.0 / sum(feat.values())
        return [(i, j * norm) for i,j in feat.items()]

class TfVectorFeaturizer(sparse_vectorspace_featurizer):

    def __init__(self):
        sparse_vectorspace_featurizer.__init__(self)
        self._itab = {}

    def _py_featurize(self, doc_sample):
        tokens = doc_sample.content.split()
        feat = {}
        for tok in tokens:
            t_id = self._itab.setdefault(tok, len(self._itab))

            feat[t_id] = feat.get(t_id, 0) + 1.0

        norm = 1.0 / sum(feat.values())
        return [(i, j * norm) for i,j in feat.items()]
