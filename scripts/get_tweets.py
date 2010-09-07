
import restful
import urllib
import sys

clustering_name = sys.argv[1]
query_terms = sys.argv[2]

serv = restful.Service('http://localhost:8080')
serv.push_post_param('_secure_token', serv.get('/secure_token'))

# resources on the server we'll manipulate
clustering_res = serv.clone().push_path('/clustering.json')
document_res = serv.clone().push_path('/document.json')
clus_doc_res = serv.clone().push_path('/clustering_document.json')

# Query or create the clustering instance
try:
    clustering = clustering_res.get('', {'name': clustering_name})[0]
except IndexError:
    clustering = clustering_res.post({'name': clustering_name})['model']

# create / look up clustering-id
print 'Clustering: %s' % clustering


def query_twitter(query_terms):

    s = restful.Service('http://search.twitter.com/search.json')

    next_page = '?%s' % urllib.urlencode(
        [('lang', 'en'), ('rpp', '100'), ('q', query_terms)])

    while next_page:

        content = s.get(next_page)

        for result in content['results']:

            try:
                doc = document_res.post( dict(
                    title = str(result['id']),
                    author = result['from_user'],
                    content = result['text']))['model']
            except KeyError:
                doc = document_res.get('', {'title': str(result['id'])})[0]

            clus_doc_res.post( dict(
                document_id = doc['id'],
                clustering_id = clustering['id']))

        next_page = content.get('next_page')
        print next_page

query_twitter(query_terms)

