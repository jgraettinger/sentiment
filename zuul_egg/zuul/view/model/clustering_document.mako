<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>

<%inherit file="/model.mako"/>

<%def name="controller()">
    <% return 'clustering_document' %>
</%def>

<%def name="model_description()">
    Clustering / Document Relation
</%def>

<%def name="instance_description(model)">
    (No Description)
</%def>

<%def name="column_headers()"> 
    <td>Clustering ID</td><td>Document ID</td>
</%def>

<%def name="as_row(req, model)">
    <%
        clus_uri = req.make_url(controller = 'clustering',
            action = 'show', id = model.clustering_id)
        doc_uri = req.make_url(controller = 'document',
            action = 'show', id = model.document_id)
    %>
    <td>${h.tags.link_to(model.clustering_id, clus_uri)|n}</td>
    <td>${h.tags.link_to(model.document_id, doc_uri)|n}</td>
</%def>

<%def name="form_elements(req, model)">
    <%
        m = model.flatten() if model else {}

        document_id = req.POST.get('document_id', m.get('document_id', ''))
        clustering_id = req.POST.get('clustering_id', m.get('clustering_id', ''))
    %>
    Document-ID:   ${h.tags.text(name = 'document_id', value = document_id)}<br>
    Clustering-ID:  ${h.tags.text(name = 'clustering_id', value = clustering_id)}<br>
</%def>

<%def name="show_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'index',
            document_id = model.document_id,
            clustering_id = model.clustering_id)
    %>
</%def>

<%def name="edit_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'edit',
            document_id = model.document_id,
            clustering_id = model.clustering_id)
    %>
</%def>

<%def name="update_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'update', _method = 'PUT',
            document_id = model.document_id,
            clustering_id = model.clustering_id)
    %>
</%def>

<%def name="delete_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'delete',
            document_id = model.document_id,
            clustering_id = model.clustering_id)
    %>
</%def>


