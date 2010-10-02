<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>

<%inherit file="/model.mako"/>

<%def name="controller()">
    <% return 'document' %>
</%def>

<%def name="model_description()">
    Document
</%def>

<%def name="instance_description(model)">
    Document &quot;${model.title|h}&quot;
</%def>

<%def name="column_headers()"> 
    <td>Identifier</td><td>Title</td><td>Author</td><td>Content</td>
</%def>

<%def name="as_row(req, model)">
    <td>${model.id|h}</td>
    <td>${h.tags.link_to(model.title, self.show_uri(req, model))|n}</td>
    <td>${model.author|h}</td>
    <td>${model.content[:150] + '...'|h}</td>
</%def>

<%def name="form_elements(req, doc)">
    <%
        m = model.flatten() if model else {}
        m.update(req.str_POST)
        m = dict((k, str(v).decode('utf8')) for k,v in m.items())
    %>
    Title:   ${h.tags.text(name = 'title', value = m.get('title', ''))}<br>
    Author:  ${h.tags.text(name = 'author', value = m.get('author', ''))}<br>
    Content: ${h.tags.textarea(name = 'content', rows = "50", cols = "80",
        content = m.get('content', ''))}<br>
</%def>

