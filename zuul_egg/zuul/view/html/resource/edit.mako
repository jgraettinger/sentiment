<%inherit file="/site.mako"/>
<%page args="req, model_ns, model"/>
<% 
    model_ns = local.get_namespace(model_ns)
    uri = model_ns.update_uri(req, model)
%>
${model_ns.view_form(req, uri, model)}
