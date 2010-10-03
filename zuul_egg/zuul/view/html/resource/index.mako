<%inherit file="/site.mako"/>
<%page args="req, model_ns, models"/>
<%!
    import webhelpers.html.tags
    import webhelpers.html as h
%>
${local.get_namespace(model_ns).view_collection(req, models)}
${h.tags.link_to('New...', req.make_url(action = 'new'))}
