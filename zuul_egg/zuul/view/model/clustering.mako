<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>

<%inherit file="/model.mako"/>

<%def name="controller()">
    <% return 'clustering' %>
</%def>

<%def name="model_description()">
    Clustering
</%def>

<%def name="instance_description(model)">
    Clustering &quot;${model.name|h}&quot;
</%def>

<%def name="column_headers()"> 
    <td>Identifier</td><td>Name</td>
</%def>

<%def name="as_row(req, model)">
    <td>${model.id|h}</td>
    <td>${h.tags.link_to(model.name, self.show_uri(req, model))|n}</td>
</%def>

<%def name="form_elements(req, model)">
    <%
        m = model.flatten() if model else {}
        name = req.POST.get('name', m.get('name', ''))
    %>
    Name: ${h.tags.textarea(name = 'name', content = name)}<br>
</%def>


