<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>

<%inherit file="/model.mako"/>

<%def name="controller()">
    <% return 'cluster' %>
</%def>

<%def name="model_description()">
    Cluster
</%def>

<%def name="instance_description(model)">
    Cluster &quot;${model.name|h}&quot; (Clustering ${model.clustering_id|h})
</%def>

<%def name="column_headers()"> 
    <td>Identifier</td><td>Clustering ID</td><td>Name</td>
</%def>

<%def name="as_row(req, model)">
    <%
        clus_uri = req.make_url(controller = 'clustering',
            action = 'show', id = model.clustering_id)
    %>
    <td>${model.id|h}</td>
    <td>${h.tags.link_to(model.clustering_id, clus_uri)|n}</td>
    <td>${h.tags.link_to(model.name, self.show_uri(req, model))|n}</td>
</%def>

<%def name="form_elements(req, model)">
    <%
        m = model.flatten() if model else {}
        clus_id = req.POST.get('clustering_id', m.get('clustering_id', ''))
        name = req.POST.get('name', m.get('name', ''))
    %>
    Clustering-ID:  ${h.tags.text(name = 'clustering_id', value = clus_id)}<br>
    Name: ${h.tags.textarea(name = 'name', content = name)}<br>
</%def>

