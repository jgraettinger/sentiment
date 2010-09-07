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
        title = req.str_POST.get('title', m.get('title', '')).decode('utf8')
        author = req.str_POST.get('author', m.get('author', '').decode('utf8'))
        content = req.str_POST.get('content', m.get('content', '').decode('utf8'))
    %>
    Title:   ${h.tags.text(name = 'title', value = title)}<br>
    Author:  ${h.tags.text(name = 'author', value = author)}<br>
    Content: ${h.tags.textarea(name = 'content', rows = "50", cols = "80", content = content)}<br>
</%def>



