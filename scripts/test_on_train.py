
import cluster.featurization
import cluster.normalization
import cluster.estimation
import cluster.feature_selection

import bootstrap

import urllib
import random
import sys
import re
import os

if len(sys.argv) != 2:
    print "Usage: %s /path/to/dataset" % sys.argv[0]
    sys.exit(-1)

cmem_empty = {'pos': 0, 'neg': 0}

def iterate_docs(relative_path, klass):

    subdoc_re = re.compile(r'<dmoz_subdoc>(.+?)</dmoz_subdoc>', re.DOTALL)

    for ind, m in enumerate(
        subdoc_re.finditer(
            open(os.path.join(sys.argv[1], relative_path)
                ).read())):
        yield '%s_%04d' % (klass, ind), m.group(1)

inj = bootstrap.bootstrap()
intern_tbl = inj.get_instance(cluster.featurization.InternTable)
normalizer = inj.get_instance(cluster.normalization.Normalizer)
featurizer = inj.get_instance(cluster.featurization.Featurizer)
feat_selector = inj.get_instance(cluster.feature_selection.FeatureSelector)

feat_selector.reset()

#### Feature Selection
for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
    feat_selector.add_observation( doc_feats, [1, 0])

for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
    feat_selector.add_observation( doc_feats, [0, 1])

print "FEATURE SELECTION: "
print feat_selector.prepare_selector()

#### Training
pos_estimator = inj.get_instance(cluster.estimation.Estimator)
neg_estimator = inj.get_instance(cluster.estimation.Estimator)

for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
    doc_feats = feat_selector.filter_features(doc_feats)

    pos_estimator.add_observation(doc_feats, 0)
    neg_estimator.add_observation(doc_feats, 1)

for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
    doc_feats = feat_selector.filter_features(doc_feats)

    pos_estimator.add_observation(doc_feats, 1)
    neg_estimator.add_observation(doc_feats, 0)

#### Decode
score = {'pos-pos': 0, 'pos-neg': 0, 'neg-pos': 0, 'neg-neg': 0}

print "NEG EST"
neg_estimator.prepare_estimator()
print

print "POS EST"
pos_estimator.prepare_estimator()
print

for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
    doc_feats = feat_selector.filter_features(doc_feats)

    n_p = neg_estimator.estimate(doc_feats)
    p_p = pos_estimator.estimate(doc_feats)

    if p_p > n_p:
        score['neg-pos'] += 1
    else:
        score['neg-neg'] += 1

for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))
    doc_feats = feat_selector.filter_features(doc_feats)

    n_p = neg_estimator.estimate(doc_feats)
    p_p = pos_estimator.estimate(doc_feats)

    if p_p > n_p:
        score['pos-pos'] += 1
    else:
        score['pos-neg'] += 1

print "SCORE:"
print score

