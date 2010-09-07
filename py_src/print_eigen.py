
import cluster.featurization
import cluster.normalization
import cluster.estimation
import cluster.feature_transform

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
featurizer = inj.get_instance(cluster.featurization.basic_featurize.TfIdfFeaturizer)


#### TERM DISTRIBUTION

term_doc_counts = {}
term_freq_counts = {}

for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    tokens = normalizer.normalize(doc_content).encode('utf8').split()

    for tok in set(tokens):
        term_doc_counts[tok] = term_doc_counts.get(tok, 0) + 1
    for tok in tokens:
        term_freq_counts[tok] = term_freq_counts.get(tok, 0) + 1

for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    tokens = normalizer.normalize(doc_content).encode('utf8').split()

    for tok in set(tokens):
        term_doc_counts[tok] = term_doc_counts.get(tok, 0) + 1
    for tok in tokens:
        term_freq_counts[tok] = term_freq_counts.get(tok, 0) + 1


term_freq_counts = [(j, i) for (i, j) in term_freq_counts.iteritems()]
term_freq_counts.sort(reverse = True)

for cnt, term in term_freq_counts[:150]:
    print "%s\t%d\t%d" % (term, cnt, term_doc_counts[term])


### DF
for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    featurizer.accumulate_doc_frequency(normalizer.normalize(doc_content))
for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    featurizer.accumulate_doc_frequency(normalizer.normalize(doc_content))

#feat_transform = inj.get_instance(cluster.feature_transform.PCAProjIGainCutoffTransform)
#feat_transform = inj.get_instance(cluster.feature_transform.PCAIGainCutoffTransform)
feat_transform = inj.get_instance(cluster.feature_transform.PCAProjTransform)

#### Feature Selection
train_feats, train_probs = [], []
for doc_uid, doc_content in iterate_docs('all_pos.txt', 'pos'):
    train_feats.append( featurizer.featurize( normalizer.normalize(doc_content)))
    train_probs.append( [1, 0])

for doc_uid, doc_content in iterate_docs('all_neg.txt', 'neg'):
    train_feats.append( featurizer.featurize( normalizer.normalize(doc_content)))
    train_probs.append( [0, 1])

print "FEATURE SELECTION: "
print feat_transform.train_transform(train_feats, train_probs)


for i in xrange(10):

    print "eigenvalue: %s" % feat_transform.get_eigenvalue(i)
    
    evec = list(feat_transform.get_eigenvector(i))
    evec.sort(key = lambda v: -v[1])

    evec = [(intern_tbl.get_str(i), j) for i, j in evec[:20]]
    
    print "eigenvector:\n\t%s" % evec



