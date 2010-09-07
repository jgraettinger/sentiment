
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

#### Initialize
clus = inj.get_instance(cluster.Clusterer)

clus.add_cluster('pos',
    inj.get_instance(cluster.estimation.Estimator))
clus.add_cluster('neg',
    inj.get_instance(cluster.estimation.Estimator))

pos_docs, neg_docs = set(), set()

#### Train
for ind, (doc_uid, doc_content) in enumerate(iterate_docs('all_pos.txt', 'pos')):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))

    pos_docs.add( doc_uid)

    if ind < 5:
        clus.add_sample(doc_uid, doc_feats, {'pos': (1, 1)})
    else:
        clus.add_sample(doc_uid, doc_feats, {'pos': (1, 0)})

for ind, (doc_uid, doc_content) in enumerate(iterate_docs('all_neg.txt', 'neg')):
    doc_feats = featurizer.featurize( normalizer.normalize(doc_content))

    neg_docs.add( doc_uid)

    if ind < 5:
        clus.add_sample(doc_uid, doc_feats, {'neg': (1, 1)})
    else:
        clus.add_sample(doc_uid, doc_feats, {'neg': (1, 0)})

#### Iterate

for i in xrange(15):
    feat_count =  clus.feature_selection()
    clus.expect_and_maximize()

    #### Decode
    score = {'pos-pos': 0, 'pos-neg': 0, 'neg-pos': 0, 'neg-neg': 0}

    for pos_doc in pos_docs:

        doc_probs = clus.get_sample_probabilities(pos_doc)

        if doc_probs['pos'][0] > doc_probs['neg'][0]:
            score['pos-pos'] += 1
        else:
            score['pos-neg'] += 1

    for neg_doc in neg_docs:

        doc_probs = clus.get_sample_probabilities(neg_doc)

        if doc_probs['pos'][0] > doc_probs['neg'][0]:
            score['neg-pos'] += 1
        else:
            score['neg-neg'] += 1

    print "Feature count: %d, Scores: %r" % (feat_count, score)

