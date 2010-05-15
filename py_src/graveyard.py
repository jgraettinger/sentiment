
documents = {}
for ind, (content,) in enumerate(db.execute(
        'select content from document order by id asc')):

    if content not in seen_texts:
        seen_texts.add(content)
        documents[ind] = content

def render_viz_inner(documents, node, into, id = 1):
    left_text = urllib.unquote(documents[node.left_child.item])
    left_text = ' '.join(t for t in re_split.split(left_text) if t)
    right_text = urllib.unquote(documents[node.right_child.item])
    right_text = ' '.join(t for t in re_split.split(right_text) if t)
    print >> into, 'id_%d -> id_%d [ label = "%s" ];' % (
        id, id * 2, left_text.replace('"', "'").encode('utf8'))
    print >> into, 'id_%d -> id_%d [ label = "%s" ];' % (
        id, id * 2 + 1, right_text.replace('"', "'").encode('utf8'))
    if node.left_child.left_child:
        render_viz_inner(documents, node.left_child, into, id * 2)
    if node.right_child.right_child:
        render_viz_inner(documents, node.right_child, into, id * 2 + 1)

def render_viz(documents, node, into):
    
    print >> into, """
    digraph clustering {
        rankdir = UD;
        node [shape = circle];
    """
    
    render_viz_inner(documents, node, into)
    
    print >> into, "}"

