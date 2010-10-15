<%inherit file="/site.mako"/>
<%page args="req, model_ns, model, child_groups"/>
<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>
<% model_ns = local.get_namespace(model_ns) %>
${model_ns.view_instance(req, model)}

%for child_ns, children in child_groups:
    <% child_ns = local.get_namespace(child_ns) %>
    ${child_ns.view_collection(req, children)}

%endfor

${h.tags.link_to('Index', model_ns.index_uri(req))|n}
