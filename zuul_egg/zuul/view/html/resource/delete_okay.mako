<%namespace name="ui_flash" file="/ui_flash.mako"/>
<%page args="req, model_ns, row_count"/>
<%
    model_ns = local.get_namespace(model_ns)

    req.resp.status = 303
    req.resp.location = model_ns.index_uri(req)
    ui_flash.add_pending_flash(req,
        'Deleted %d Rows' % row_count)
%>
