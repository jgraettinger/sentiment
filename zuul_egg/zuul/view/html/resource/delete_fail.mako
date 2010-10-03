<%namespace name="ui_flash" file="/ui_flash.mako"/>
<%page args="req, model_ns, exception, template_resolver"/>
<%
    ui_flash.add_flash(req, str(exception))

    template_resolver.resolve(req, 'index.mako').render_context(
        context, req = req, model_ns = model_ns, models = req.query)
%>
