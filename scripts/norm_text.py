
import urllib
import re
from schema import cluster_db

split_chars = """\r\n\t ~`!@#$%^&*()_-+={[}]|\:;"'<,>.?/"""
re_split = re.compile('\\' + '|\\'.join(split_chars))

def normalize_text(text):
    text = urllib.unquote(text)
    text = ' '.join(t for t in text.split() if not t.startswith('http') and not t.startswith('@'))
    return ' '.join(t for t in re_split.split(text.lower()) if t and t != 'rt')

db = cluster_db.connect('test.db')
db.execute('begin')

for (id, content) in db.execute('select id, content from document').fetchall():
    db.execute('update document set norm_content = ? where id = ?',
        (normalize_text(content), id))

db.execute('end')

