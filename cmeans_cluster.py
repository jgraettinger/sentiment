import os
import random
from schema import cluster_db
from featurize import BigramFeaturizer
import cluster

db = cluster_db.connect('test.db')
active_clusters = db.execute('select id, name from cluster').fetchall()
print active_clusters

featurizer = cluster.cached_featurizer(
    BigramFeaturizer())

clusterer = cluster.em_cluster()
for clus_id, clus_name in active_clusters:
    clusterer.add_cluster(
        str(clus_id),
        cluster.sparse_vectorspace_estimator(featurizer, 0.075))

db.execute('begin')
for doc_id, content in db.execute(
        'select id, norm_content from document').fetchall():

    clusters = [(str(c[0]), c[1]) for c in db.execute(
        'select cluster_id, factor from membership '+
            'where not hard and doc_id = ?', (doc_id,))]
    hard_clusters = [(str(c[0]), c[1]) for c in db.execute(
        'select cluster_id, factor from membership '+
            'where hard and doc_id = ?', (doc_id,))]

    if not clusters and not hard_clusters:
        ind = random.randint(0, len(active_clusters)-1)
        clusters = [(str(c[0]), 1 if ind == i else 0) for i, c in enumerate(active_clusters)]

    clusterer.add_sample(
        cluster.document_sample(str(doc_id), '', content.encode('utf8')),
        clusters, hard_clusters,
    )
db.execute('end')

for i in xrange(1):
    print "=" * 10
    clusterer.iterate()

