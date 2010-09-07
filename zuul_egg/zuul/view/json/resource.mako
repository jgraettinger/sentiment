<%!
    import cjson, base64
%>

<%def name="index(req, model_ns, models)">
    <%
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        context.write('[')
        for ind, model in enumerate(models):
            enc_mod = cjson.encode(model.flatten())

            if ind:
                context.write(',\r\n%s' % enc_mod)
            else:
                context.write(enc_mod)

        context.write(']')
    %>
</%def>

<%def name="show(req, model_ns, model, child_groups)">
    <%
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        model = model.flatten()
        context.write(cjson.encode(model))
    %>
</%def>

<%def name="create_okay(req, model_ns, model)">
    <% 
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        req.session.refresh(model)
        model = model.flatten()

        context.write(cjson.encode({'status': 'OK', 'model': model}))
    %>
</%def>

<%def name="update_okay(req, model_ns, row_count)">
    <% 
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        context.write(cjson.encode({'status': 'OK', 'updated_row_count': row_count}))
    %>
</%def>

<%def name="delete_okay(req, model_ns, row_count)">
    <% 
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        context.write(cjson.encode({'status': 'OK', 'deleted_row_count': row_count}))
    %>
</%def>

<%def name="create_fail(req, model_ns, exception)">
    <% 
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        context.write(cjson.encode({'status': 'ERROR', 'reason': str(exception)}))
    %>
</%def>

<%def name="update_fail(req, model_ns, exception)">
    <% 
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        context.write(cjson.encode({'status': 'ERROR', 'reason': str(exception)}))
    %>
</%def>

<%def name="delete_fail(req, model_ns, exception)">
    <% 
        req.resp.content_type = 'application/json'
        req.resp.content_type_params = {'charset': 'utf8'}

        context.write(cjson.encode({'status': 'ERROR', 'reason': str(exception)}))
    %>
</%def>

