import urllib
import sqlite3
import cjson
import sys

terms = sys.argv[1:] 
args = {'lang': 'en', 'rpp': '100'}.items()
args.extend(('q', i) for i in terms)

uri_base = "http://search.twitter.com/search.json"

db = sqlite3.connect('test.db', isolation_level = None)
db.executescript("""
    create table if not exists document (
        id integer primary key autoincrement,
        content text not null,
        norm_content text not null
    );
""")
db.execute('begin')

next_page = '?%s' % urllib.urlencode(args)

while next_page: 
    
    full_uri = uri_base + next_page
    print full_uri
    content = cjson.decode(urllib.urlopen(full_uri).read())
    
    for result in content['results']:
        db.execute(
            'insert into document (id, content) values (?, ?)',
            (result['id'], result['text']))
    
    next_page = content.get('next_page')
    
db.execute('end')

