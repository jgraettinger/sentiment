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
    <td>${model.content[:50] + '...'|h}</td>
</%def>

<%def name="form_elements(req, doc)">
    <%
        m = model.flatten() if model else {}
        title = req.POST.get('title', m.get('title', ''))
        author = req.POST.get('author', m.get('author', ''))
        content = req.POST.get('content', m.get('content', ''))
    %>
    Title:   ${h.tags.text(name = 'title', value = title)}<br>
    Author:  ${h.tags.text(name = 'author', value = author)}<br>
    Content: ${h.tags.textarea(name = 'content', content = content)}<br>
</%def>



