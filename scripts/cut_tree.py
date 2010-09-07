
import sqlite3
from cluster import dendrogram

db = sqlite3.connect('test.db', isolation_level = None)
node = eval(open('clusters.repr').read())

def cut_tree(node, depth1, depth2, depth3, cuts, cur_depth = 0, id = 1):
    
    if cur_depth == depth3:

        id2 = id  >> (depth3 - depth2)
        id1 = id2 >> (depth2 - depth1)

        cuts1 = cuts.setdefault(id1, {})
        cuts1.setdefault(id2, []).append(
            db.execute('select norm_content from document where id = ?',
                (node.item,)).fetchone()[0])
        return
    
    if not node.left_child:

        cuts.setdefault('shallow', {}
            ).setdefault('shallow', []
            ).append(
            db.execute('select norm_content from document where id = ?',
                (node.item,)).fetchone()[0])
        return

    cut_tree(node.left_child, depth1, depth2, depth3, cuts, cur_depth + 1, id * 2)
    cut_tree(node.right_child, depth1, depth2, depth3, cuts, cur_depth + 1, id * 2 + 1)
    return

cuts = {}
cut_tree(node, 3, 6, 8, cuts)

for key, values in cuts.iteritems():
    print '<table border="2">'
    for key, val2 in values.iteritems():
        print "<tr><td>"
        print "<br>".join(val2).encode('utf8')
        print "</tr></td>"
    print "</table>"

