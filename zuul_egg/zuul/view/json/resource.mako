<%!
    import cjson, base64
%>

<%def name="index(req, model_ns, models)">
    <% model_ns = local.get_namespace(model_ns) %>
    ${model_ns.as_json(models)}
</%def>

<%def name="show(req, model_ns, model, child_groups)">
    <% model_ns = local.get_namespace(model_ns) %>
    ${model_ns.as_json([model])}
</%def>

<%def name="create_okay(req, model_ns, model)">
    {"status": "OK"}
</%def>

<%def name="update_okay(req, model_ns, row_count)">
    ${cjson.encode(dict(status = 'OK', updated_row_count = row_count))}
</%def>

<%def name="delete_okay(req, model_ns, row_count)">
    ${cjson.encode(dict(status = 'OK', deleted_row_count = row_count))}
</%def>

<%def name="create_fail(req, model_ns, exception)">
    ${cjson.encode(dict(status = 'ERROR', reason = str(exception)))}
</%def>

<%def name="update_fail(req, model_ns, exception)">
    ${cjson.encode(dict(status = 'ERROR', reason = str(exception)))}
</%def>

<%def name="delete_fail(req, model_ns, exception)">
    ${cjson.encode(dict(status = 'ERROR', reason = str(exception)))}
</%def>

