<%namespace name="ui_flash" file="/ui_flash.mako"/>
<%page args="req, model_ns, row_count"/>
<% 
    model_ns = local.get_namespace(model_ns)

    req.resp.status = 303
    if 'id' in req.route_args:
        req.resp.location = req.make_url(action = 'show')
        ui_flash.add_pending_flash(req, 'Updated')
    else:
        req.resp.location = model_ns.index_uri(req)
        ui_flash.add_pending_flash(req,
            'Updated %d Rows' % row_count)
%>
