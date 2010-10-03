<%inherit file="/site.mako"/>
<%page args="req, model_ns, model, child_groups"/>
<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>
<% model_ns = local.get_namespace(model_ns) %>
<h3>${model_ns.instance_description(model)}</h3>
${model_ns.view_instance(req, model)}

%for child_ns, children in child_groups:
    <% child_ns = local.get_namespace(child_ns) %>

    <br><br>
    <h3>${child_ns.model_description()}</h3>
    ${child_ns.view_collection(req, children)}

%endfor

<br><br> 
${h.tags.link_to('Index', model_ns.index_uri(req))|n}
