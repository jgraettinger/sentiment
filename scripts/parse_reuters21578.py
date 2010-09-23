
import HTMLParser 
import simplejson
import sqlite3
import sys

if len(sys.argv) <= 3:
    print "usage: %s <sample-database> <input-sgm>" % sys.argv[0]
    sys.exit(-1)

db = sqlite3.connect(sys.argv[1], isolation_level = None)

db.executescript("""
pragma foreign_keys = on;

create table if not exists sample (
    uid text        primary key not null,
    type text       not null,
    weight real     default 1,
    attributes text not null
);

create table if not exists sample_class (
    sample_uid text references sample(uid) on delete cascade,
    class_name text not null,
    primary key (sample_uid, class_name)
);
""")


class ReutersExporter(HTMLParser.HTMLParser):
    """
    This class is ugly as sin. Cheers!

    Let that be a lesson to you, if you're
    thinking of SGML as an export format!
    """

    def __init__(self, db):
        HTMLParser.HTMLParser.__init__(self)
        self.db = db
        self.doc_ind = 0
        self.new_doc()
        return

    def new_doc(self):
        self.doc_ind += 1
        self.attributes = {}
        self.classes = []
        self.cur_tag = None
        return

    def export_doc(self):
        if not self.attributes.get('content'):
            return

        global db

        uid = 'r21578_%d' % self.doc_ind
        attr = simplejson.dumps(self.attributes)

        db.execute('insert into sample (uid, type, weight, attributes) values (?, ?, ?, ?)',
            (uid, 'reuters', 1, attr))

        for cls in set(self.classes):
            db.execute('insert into sample_class (sample_uid, class_name) values (?, ?)',
                (uid, cls))

        return

    def handle_starttag(self, tag, attrs):

        if tag == 'reuters':
            if self.attributes:
                self.export_doc()
            self.new_doc()
            self.attributes.update(attrs)

        elif tag == 'd':
            return

        elif tag in ('date', 'topics', 'places', 'people',
            'orgs', 'exchanges', 'companies', 'title', 'body'):

            self.cur_tag = tag
            self.cur_data = ''

        else:
            self.cur_tag = None

    def handle_endtag(self, tag):
        if tag == 'd':
            self.cur_data += '|'
            return

        self.cur_data = self.cur_data.strip().decode('utf8', 'ignore')
        if self.cur_tag in ('topics', 'places', 'people', 'orgs', 'exchanges', 'companies'):
            new_classes = [t for t in self.cur_data.split('|') if t]
            for t in new_classes:
                self.classes.append( '%s_%s' % (self.cur_tag, t))
        elif self.cur_tag == 'date':
            self.attributes['date'] = self.cur_data
        elif self.cur_tag == 'title':
            self.attributes['title'] = self.cur_data
        elif self.cur_tag == 'body':
            self.attributes['content'] = self.cur_data

        self.cur_tag = None
        return

    def handle_data(self, data):
        if not self.cur_tag:
            return

        self.cur_data += data
        return

    def handle_charref(self, d):
        self.handle_data(chr(int(d)))

    def handle_entityref(self, d):
        if d == 'lt':
            self.handle_data('<')
        elif d == 'amp':
            self.handle_data('&')
        else:
            print "entref %r" % d

db.execute('begin')
parser = ReutersExporter(db)


for arg in sys.argv[2:]:

    sgm_in = open(arg)

    while True:
        buf = sgm_in.read(8192)
        if not buf:
            break
        parser.feed(buf)

db.execute('commit')

