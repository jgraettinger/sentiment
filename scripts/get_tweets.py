
import urllib
import cjson
import sys


uri = 'http://localhost:8080/'

query_terms = sys.argv[1:]

# create / look up clustering-id
clustering_name = ('twitter: %s' % \
    ' '.join(query_terms)).title()

stoken = urllib.urlopen(uri + 'secure_token').read()
print urllib.urlopen(uri + 'clustering.json', urllib.urlencode(
    dict(name = clustering_name, _secure_token = stoken))).read()

clustering_id = cjson.decode( urllib.urlopen(uri + 'clustering.json?name=' + \
    urllib.quote(clustering_name)).read())[0]

def query_twitter(query_terms):
    
    args = {'lang': 'en', 'rpp': '10'}.items()
    args.extend(('q', i) for i in query_terms)

    uri_base = "http://search.twitter.com/search.json"

    next_page = '?%s' % urllib.urlencode(args)

    while next_page: 

        full_uri = uri_base + next_page
        print full_uri
        content = cjson.decode(urllib.urlopen(full_uri).read())

        for result in content['results']:
            import pdb; pdb.set_trace()

        next_page = content.get('next_page')
        break

# query_twitter(query_terms)


