<%namespace name="ui_flash" file="/ui_flash.mako"/>
<%page args="req, model_ns, model"/>
<%
    model_ns = local.get_namespace(model_ns)

    req.resp.status = 303
    req.resp.location = model_ns.show_uri(req, model)
    ui_flash.add_pending_flash(req, 'Started Regression Run')
%>
