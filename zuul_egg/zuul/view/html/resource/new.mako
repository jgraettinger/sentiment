<%inherit file="/site.mako"/>
<%page args="req, model_ns"/>
<% 
    model_ns = local.get_namespace(model_ns)
    uri = req.make_url(action = 'create')
%>
${model_ns.view_form(req, uri, None)}
