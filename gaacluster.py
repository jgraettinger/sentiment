
import sqlite3
from cluster import GAAClusterer

db = sqlite3.connect('test.db', isolation_level = None)

intern_tab = {}
def featurize(text):
    tokens = text.split()

    feat = {}
    for ind in xrange(len(tokens)):
        un_tok_id = intern_tab.setdefault(tokens[ind], len(intern_tab))
        feat[un_tok_id] = feat.get(un_tok_id, 0) + 1

        if ind == len(tokens) - 1: continue

        bi_tok_id = intern_tab.setdefault(' '.join(tokens[ind:ind+2]), len(intern_tab))
        feat[bi_tok_id] = feat.get(bi_tok_id, 0) + 1

    return feat 

def dendrogram_repr(node, indent = 0):
    if node.left_child:
        sep = '\n' + '  ' * indent
        return sep.join([
            'dendrogram(%d,' % node.item,
            '  %s,' % dendrogram_repr(node.left_child,  indent + 1),
            '  %s,' % dendrogram_repr(node.right_child, indent + 1),
            ')'])
    else:
        return 'dendrogram(%d)' % node.item

clusterer = GAAClusterer()

print "clustering..."
query = db.execute('select id, norm_content from document')
dendrogram = clusterer.cluster((ind, featurize(txt)) for ind, txt in query)

open('clusters.repr', 'w').write(dendrogram_repr(dendrogram))

