
from cluster import sparse_features

import intern_table
import getty
import math
import re

class TfIdfFeaturizer(object):

    @getty.requires(intern_table = intern_table.InternTable)
    def __init__(self, intern_table):
        self._itab = intern_table
        self._df = {}

    def accumulate_doc_frequency(self, content):
        tokens = content.encode('utf8').split()

        for tok in set(tokens):
            t_id = self._itab.get_id(tok)

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
            feat[t_id] = feat.get(t_id, 0) + (1.0 / self._df[t_id])

        norm = 1.0 / sum(feat.values())
        return sparse_features(dict(
            (i, j * norm) for i, j in feat.items()))

class TfFeaturizer(object):

    @getty.requires(intern_table = intern_table.InternTable)
    def __init__(self, intern_table):
        self._itab = intern_table

    def featurize(self, content):
        tokens = content.encode('utf8').split()

        feat = {}
        for tok in tokens:
            t_id = self._itab.get_id(tok)

            if t_id not in feat:
                self._itab.add_reference(t_id)
                feat[t_id] = 1
            else:
                feat[t_id] += 1

        norm = 1.0 / sum(feat.values())
        return sparse_features(dict(
            (i, j * norm) for i, j in feat.items()))

