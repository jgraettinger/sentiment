<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>

<%inherit file="/model.mako"/>

<%def name="controller()">
    <% return 'cluster_document' %>
</%def>

<%def name="model_description()">
    Cluster / Document Relation
</%def>

<%def name="instance_description(model)">
    (No Description)
</%def>

<%def name="column_headers()"> 
    <td>Clustering ID</td>
    <td>Document ID</td>
    <td>Cluster ID</td>
    <td>Probability</td>
    <td>Is Explicit</td>
</%def>

<%def name="as_row(req, model)">
    <%
        m = model.flatten()

        clsg_uri = req.make_url(controller = 'clustering',
            action = 'show', id = m['clustering_id'])
        doc_uri = req.make_url(controller = 'document',
            action = 'show', id = m['document_id'])
        clus_uri = req.make_url(controller = 'cluster',
            action = 'show', id = m['cluster_id'])
    %>
    <td>${h.tags.link_to(m['clustering_id'], clsg_uri)|n}</td>
    <td>${h.tags.link_to(m['document_id'],    doc_uri)|n}</td>
    <td>${h.tags.link_to(m['cluster_id'],    clus_uri)|n}</td>
    <td>${m['probability']}</td>
    <td>${m['explicit']}</td>
</%def>

<%def name="form_elements(req, model)">
    <%
        m = model.flatten() if model else {}

        document_id = req.POST.get('document_id', m.get('document_id', ''))
        clustering_id = req.POST.get('clustering_id', m.get('clustering_id', ''))
        cluster_id = req.POST.get('cluster_id', m.get('cluster_id', ''))
        probability = req.POST.get('probability', m.get('probability', ''))
        explicit = req.POST.get('explicit', m.get('explicit', ''))
    %>
    Document-ID:   ${h.tags.text(name = 'document_id', value = document_id)}<br>
    Clustering-ID:  ${h.tags.text(name = 'clustering_id', value = clustering_id)}<br>
    Cluster-ID:  ${h.tags.text(name = 'cluster_id', value = cluster_id)}<br>
    Probability:  ${h.tags.text(name = 'probability', value = probability)}<br>
    Explicit:  ${h.tags.text(name = 'explicit', value = explicit)}<br>
</%def>

<%def name="show_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'index',
            document_id = model.document_id,
            clustering_id = model.clustering_id,
            cluster_id = model.cluster_id)
    %>
</%def>

<%def name="edit_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'edit',
            document_id = model.document_id,
            clustering_id = model.clustering_id,
            cluster_id = model.cluster_id)
    %>
</%def>

<%def name="update_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'update', _method = 'PUT',
            document_id = model.document_id,
            clustering_id = model.clustering_id,
            cluster_id = model.cluster_id)
    %>
</%def>

<%def name="delete_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'delete',
            document_id = model.document_id,
            clustering_id = model.clustering_id,
            cluster_id = model.cluster_id)
    %>
</%def>


