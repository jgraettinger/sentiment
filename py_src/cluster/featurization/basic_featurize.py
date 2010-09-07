
from cluster.features import SparseFeatures

import intern_table
import getty
import math
import re

class TfIdfFeaturizer(object):

    @getty.requires(intern_table = intern_table.InternTable)
    def __init__(self, intern_table):
        self._itab = intern_table
        self._df = {}
        self._n_docs = 1.0

    def accumulate_doc_frequency(self, content):
        tokens = content.encode('utf8').split()
        self._n_docs += 1

        for tok in set(tokens):
            t_id = self._itab.add_token(tok)

            if t_id not in self._df:
                self._itab.add_reference(t_id)
                self._df[t_id] = 1
            else:
                self._df[t_id] += 1

        return

    def featurize(self, content):
        tokens = content.encode('utf8').split()

        feat = {}
        for tok in tokens:
            t_id = self._itab.get_id(tok)
            feat[t_id] = feat.get(t_id, 0) + 1

        feat = dict((i, math.log(j) + 1) for i,j in feat.items())
        #feat = dict((i, j) for i,j in feat.items())

        feat = dict((i, j * math.log(self._n_docs / self._df[i])) for i,j in feat.items())

        norm = math.sqrt(sum(i * i for i in feat.values()))

        feat = dict((i, j / norm) for i, j in feat.items())

        return SparseFeatures(feat)

class TfFeaturizer(object):

    @getty.requires(intern_table = intern_table.InternTable)
    def __init__(self, intern_table):
        self._itab = intern_table

    def featurize(self, content):
        tokens = content.encode('utf8').split()

        feat = {}
        for tok in tokens:
            t_id = self._itab.add_token(tok)

            if t_id not in feat:
                self._itab.add_reference(t_id)
                feat[t_id] = 1
            else:
                feat[t_id] += 1

        feat = dict((i, j) for i,j in feat.items())

        norm = math.sqrt(sum(i * i for i in feat.values()))
        feat = dict((i, j / norm) for i, j in feat.items())

        return SparseFeatures(feat)

