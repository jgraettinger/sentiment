import os
import cgi
import cjson
import random
from schema import cluster_db
from featurize import TfIdfVectorFeaturizer
import cluster
import BaseHTTPServer

db = cluster_db.connect('test.db')
active_clusters = db.execute('select id, name from cluster').fetchall()
print active_clusters

tf_idf_feat = TfIdfVectorFeaturizer()
for content, in db.execute('select norm_content from document'):
    tf_idf_feat.accumulate_doc_frequency(content)

featurizer = cluster.cached_featurizer(tf_idf_feat)

clusterer = cluster.em_cluster()
for clus_id, clus_name in active_clusters:
    clusterer.add_cluster(
        str(clus_id),
        cluster.sparse_vectorspace_estimator(featurizer, -1))

db.execute('begin')
for doc_ind, (doc_id, content) in enumerate(db.execute(
        'select id, norm_content from document').fetchall()):

    clusters = [(str(c[0]), c[1]) for c in db.execute(
        'select cluster_id, factor from membership '+
            'where not hard and doc_id = ?', (doc_id,))]
    hard_clusters = [(str(c[0]), c[1]) for c in db.execute(
        'select cluster_id, factor from membership '+
            'where hard and doc_id = ?', (doc_id,))]

    if not clusters and not hard_clusters:
        #ind = random.randint(0, len(active_clusters)-1)
        clusters = [(str(c[0]), 1 if doc_ind == i else 0) for i, c in enumerate(active_clusters)]

    clusterer.add_sample(
        cluster.document_sample(str(doc_id), '', content.encode('utf8')),
        clusters, hard_clusters,
    )
db.execute('end')

for i in xrange(1):
    print "=" * 10
    clusterer.iterate()

vals = {}
for doc_id, in db.execute('select id from document'):
    vals[str(doc_id)] = clusterer.get_sample_probabilities(str(doc_id))

open('doc_clusters.json', 'w').write(cjson.encode(vals))

class HTTPHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    
    def do_GET(self):

        self.send_response(200)
        self.end_headers()

        uri = cgi.urlparse.urlparse(self.path)
        args = cgi.parse_qs(uri.query)

        if uri.path == '/clusters':
            self.wfile.write(cjson.encode(dict((str(i), j) for i,j in \
                db.execute('select id, name from cluster'))))

        elif uri.path == '/cluster_docs':
            c_id = args['cluster_uid'][0]

            all = []
            for doc_id, in db.execute('select id from document'):
                doc_id = str(doc_id)
                all.append( (-clusterer.get_sample_probabilities(doc_id)[c_id], doc_id))

            all.sort()
            all = [i[1] for i in all[:int(args['count'][0])]]

            self.wfile.write(cjson.encode(
                {'uid': c_id, 'document_uids': all}))

        elif uri.path == '/doc_content':
            d_id = args['document_uid'][0]

            doc = {
                'uid': args['document_uid'][0],
                'content': db.execute(
                    'select content from document where id = ?',
                    (d_id,)).fetchone()[0],
            }

            self.wfile.write(cjson.encode(doc))
        
        elif uri.path == '/doc_cluster_link':
            d_id = args['document_uid'][0]
            c_id = args['cluster_uid'][0]

            dsamp = cluster.document_sample(d_id, '',
                db.execute("select norm_content from document where "\
                    "id = %s" % d_id).fetchone()[0].encode('utf8'))
            
            clusters = clusterer.get_sample_probabilities(d_id)
            hard_clusters = {c_id: 1}

            clusterer.drop_sample(d_id)
            clusterer.add_sample(
                dsamp, clusters, hard_clusters)

            self.wfile.write('[]')

        elif uri.path == '/iterate':
            clusterer.iterate()
            self.wfile.write('[]')

        else:

            try:
                content = open(self.path[1:]).read()
            except Exception, e:
                content = str(e) 
    
            self.end_headers()
            self.wfile.write(content)
        
        return

httpd = BaseHTTPServer.HTTPServer(('', 8081), HTTPHandler)
while True:
    httpd.handle_request()

