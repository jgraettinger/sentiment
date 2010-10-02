<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>


<%def name="view_form(req, uri, model)">
    ${h.tags.form(uri)}
        ${h.tags.hidden('_secure_token', req.secure_token)}
        ${self.form_elements(req, model)}
        ${h.tags.submit(value = 'Save', name = '_commit')}
    ${h.tags.end_form()}
</%def>

<%def name="view_collection(req, models)">
    ${self.as_table(req, models)|n}
</%def>

<%def name="view_instance(req, model)">
    ${self.view_collection(req, [model])|n}
</%def>

<%def name="as_table(req, models)">
    <table>

    ## Column headers
    <tr>
    <td></td>
    ${self.column_headers()}
    </tr>

    <% empty = True %>
    %for model in models:
        <tr>
        <td>${self.delete_link(req, model)}</td>
        ${self.as_row(req, model)}
        <td>${self.edit_link(req, model)}</td>
        </tr>
        <% empty = False %>
    %endfor

    %if empty:
        <tr><td>(Empty...)</td></tr>
    %endif

    </table>
</%def>

<%def name="column_headers()"> 
    Invalid
</%def>

<%def name="as_row(req, model)">
    Invalid
</%def>


<%def name="edit_link(req, model)">
    ${h.tags.link_to('edit', self.edit_uri(req, model))}
</%def>

<%def name="delete_link(req, model)">
    <% uri = self.delete_uri(req, model) %>
    ${h.tags.form(uri, method = 'DELETE')}
        ${h.tags.hidden('_secure_token', req.secure_token)}
        ${h.tags.submit(value = 'Delete', name = '_commit')}
    ${h.tags.end_form()}
</%def>

<%def name="index_uri(req)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'index', id = None)
    %>
</%def>

<%def name="new_uri(req)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'new')
    %>
</%def>

<%def name="create_uri(req)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'create')
    %>
</%def>

<%def name="show_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'show',
            id = model.id)
    %>
</%def>

<%def name="edit_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'edit',
            id = model.id)
    %>
</%def>

<%def name="update_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'update',
            id = model.id,
            _method = 'PUT')
    %>
</%def>

<%def name="delete_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'delete',
            id = model.id)
    %>
</%def>

${self.body()}
