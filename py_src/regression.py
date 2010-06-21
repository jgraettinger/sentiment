
import urllib
import random
import featurize
import cluster
import sys
import re
import os

if len(sys.argv) != 2:
    print "Usage: %s /path/to/dataset" % sys.argv[0]
    sys.exit(-1)

split_chars = """\r\n\t ~`!@#$%^&*()_-+={[}]|\:;"'<,>.?/"""
re_split = re.compile('\\' + '|\\'.join(split_chars))

cmem_empty = {'pos': 0, 'neg': 0}

def normalize_text(text):
    text = urllib.unquote(text)
    text = ' '.join(t for t in text.split() if not t.startswith('http') and not t.startswith('@'))
    text = ' '.join(t for t in re_split.split(text.lower()) if t and t != 'rt')
    return text

def iterate_docs(relative_path, klass):

    subdoc_re = re.compile(r'<dmoz_subdoc>(.+?)</dmoz_subdoc>', re.DOTALL)

    for ind, m in enumerate(
        subdoc_re.finditer(
            open(os.path.join(sys.argv[1], relative_path)
                ).read())):
        yield '%s_%d' % (klass, ind), normalize_text(m.group(1))


feat = cluster.cached_featurizer(
    featurize.TfVectorFeaturizer())

clus = cluster.em_cluster()
clus.add_cluster('pos',
    cluster.unigram_lm_estimator(feat))
clus.add_cluster('neg',
    cluster.unigram_lm_estimator(feat))

pos_iter = iterate_docs('all_pos.txt', 'pos')
neg_iter = iterate_docs('all_neg.txt', 'neg')

docs = set()
random.seed(32)

for doc_uid, doc_content in pos_iter:

    h_mem = {}
    if not random.randint(0, 20):
        h_mem = {'pos': 1}

    clus.add_sample(
        cluster.document_sample(doc_uid, '', doc_content),
        {'neg': 0, 'pos': 0}, h_mem)
    docs.add(doc_uid)

for doc_uid, doc_content in neg_iter:

    h_mem = {}
    if not random.randint(0, 20):
        h_mem = {'neg': 1}

    clus.add_sample(
        cluster.document_sample(doc_uid, '', doc_content),
        {'neg': 0, 'pos': 0}, h_mem)
    docs.add(doc_uid)

for i in xrange(10):
    clus.iterate()

counts = {}
for d_uid in sorted(docs):
    cls = d_uid.split('_')[0]

    m = max((j,i) for (i,j) in \
        clus.get_sample_probabilities(d_uid).items())

    key = '%s-%s' % (cls, m[1])
    counts[key] = counts.get(key, 0) + 1

print counts
