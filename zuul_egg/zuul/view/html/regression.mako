
<%inherit file="/html/resource.mako"/>

<%page expression_filter='h'/>
<%!
    import base64
    import webob.exc
    import webhelpers.html.tags
    import webhelpers.html as h
%>

<%def name="run_okay(req, model_ns, model, thread)">
    <% model_ns = local.get_namespace(model_ns) %>
    <%
        req.resp.status = 303
        req.resp.location = model_ns.show_uri(req, model)
        self.set_ui_flash(req.resp, 'Started Regression Run')
    %>
</%def>

<%def name="run_fail(req, model_ns, exception)">
    <h3>Error: ${str(exception)|h}</h3><br>
    ${self.index(req, model_ns, req.query)}
</%def>

