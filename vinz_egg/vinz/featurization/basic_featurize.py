
import math
import getty

import intern_table
from vinz.features import SparseFeatures, DenseFeatures

class TfFeaturizer(object):

    @getty.requires(intern_table = intern_table.InternTable)
    def __init__(self, intern_table):
        self._itab = intern_table

    def featurize(self, doc):
        tokens = doc['content'].split()

        feat = {}
        for tok in tokens:
            t_id = self._itab.add_token(tok)

            if t_id not in feat:
                self._itab.add_reference(t_id)
                feat[t_id] = 1
            else:
                feat[t_id] += 1

        return SparseFeatures(feat)


class PassthroughDenseFeaturizer(object):

    def featurize(self, sample):
        return DenseFeatures(sample['features'])


"""

IDF needs to be re-defined as a (chained) feature transform


class TfIdfFeaturizer(object):

    @getty.requires(intern_table = intern_table.InternTable)
    def __init__(self, intern_table):
        self._itab = intern_table
        self._df = {}
        self._n_docs = 1.0

    def add_sample(self, doc):
        tokens = doc['content'].split()
        self._n_docs += 1

        for tok in set(tokens):
            t_id = self._itab.add_token(tok)
            self._itab.add_reference(t_id)
            self._df[t_id] = self._df.get(t_id, 0) + 1

        return

    def drop_sample(self, doc):
        tokens = doc['content'].split()
        self._n_docs -= 1

        for tok in set(tokens):
            t_id = self._itab.get_id(tok)
            self._itab.drop_reference(t_id)
            self._df[t_id] -= 1

            if not self._df[t_id]:
                del self._df[t_id]

        return

    def featurize(self, doc):
        tokens = doc['content'].split()

        feat = {}
        for tok in tokens:
            t_id = self._itab.get_id(tok)
            feat[t_id] = feat.get(t_id, 0) + 1

        feat = dict((i, math.log(j) + 1) for i,j in feat.items())
        feat = dict((i, j * math.log(self._n_docs / self._df[i])) for i,j in feat.items())
        norm = math.sqrt(sum(i * i for i in feat.values()))
        feat = dict((i, j / norm) for i, j in feat.items())
        return SparseFeatures(feat)
"""

