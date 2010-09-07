<%page expression_filter='h'/>

<%!
    import cjson, base64
    import webob.exc
    import webhelpers.html.tags
    import webhelpers.html as h
%>

<%def name="set_ui_flash(resp, msg)">
    <%
        resp.set_cookie('ui_flash', base64.b64encode(msg))
    %>
</%def>

<%def name="display_ui_flash(req)">
    <%
        flash = base64.b64decode(req.cookies.get('ui_flash', ''))

        if flash:
            req.resp.set_cookie('ui_flash', '')
    %>
    <h3>${flash}</h3>
</%def>

<%def name="index(req, model_ns, models)">
    <% model_ns = local.get_namespace(model_ns) %>
    <html>
        ${display_ui_flash(req)}

        ${model_ns.as_table(req, models)}

        ${h.tags.link_to('New...', req.make_url(action = 'new'))|n}
    </html>
</%def>

<%def name="new(req, model_ns)">
    <%
        model_ns = local.get_namespace(model_ns)
        uri = req.make_url(action = 'create')
    %>
    <html>
        ${display_ui_flash(req)}
        ${model_ns.as_form(req, uri, None)}
    <html>
</%def>

<%def name="edit(req, model_ns, model)">
    <%
        model_ns = local.get_namespace(model_ns)
        uri = model_ns.update_uri(req, model)
    %>
    <html>
        ${display_ui_flash(req)}
        ${model_ns.as_form(req, uri, model)}
    <html>
</%def>

<%def name="show(req, model_ns, model, child_groups)">
    <% model_ns = local.get_namespace(model_ns) %>
    <html>
        ${display_ui_flash(req)}
        <h3>${model_ns.instance_description(model)}</h3>
        ${model_ns.as_table(req, [model])}

        %for child_ns, children in child_groups:
            <% child_ns = local.get_namespace(child_ns) %>

            <br><br>
            <h3>${child_ns.model_description()}</h3>
            ${child_ns.as_table(req, children)}

        %endfor
        
        <br><br> 
        ${h.tags.link_to('Index', model_ns.index_uri(req))|n}
    </html>

</%def>

<%def name="create_okay(req, model_ns, model)">
    <% model_ns = local.get_namespace(model_ns) %>
    <%
        req.resp.status = 303
        req.resp.location = model_ns.show_uri(req, model)
        set_ui_flash(req.resp, 'Created')
    %>
</%def>

<%def name="update_okay(req, model_ns, row_count)">
    <% model_ns = local.get_namespace(model_ns) %>
    <%
        req.resp.status = 303
        if 'id' in req.route_args:
            req.resp.location = req.make_url(action = 'show')
            set_ui_flash(req.resp1, 'Updated')
        else:
            req.resp.location = model_ns.index_uri(req)
            set_ui_flash(req.resp, 'Updated %d Rows' % row_count)
    %>
</%def>

<%def name="delete_okay(req, model_ns, row_count)">
    <% model_ns = local.get_namespace(model_ns) %>
    <%
        req.resp.status = 303
        req.resp.location = model_ns.index_uri(req)
        set_ui_flash(req.resp, 'Deleted %d Rows' % row_count)
    %>
</%def>

<%def name="create_fail(req, model_ns, exception)">
    <h3>Error: ${str(exception)|h}</h3><br>
    ${self.new(req, model_ns)}
</%def>

<%def name="update_fail(req, model_ns, exception)">
    <% model = req.query.limit(2).all() %>
    <h3>Error: ${str(exception)|h}</h3><br>
    %if len(model) == 1:
        ${self.edit(req, model_ns, model[0])}
    %else:
        ${self.index(req, model_ns, req.query)}
    %endif
</%def>

<%def name="delete_fail(req, model_ns, exception)">
    <h3>Error: ${str(exception)|h}</h3><br>
</%def>

