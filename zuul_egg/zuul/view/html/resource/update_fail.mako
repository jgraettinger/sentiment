<%namespace name="ui_flash" file="/ui_flash.mako"/>
<%page args="req, model_ns, model, exception, template_resolver"/>
<%
    ui_flash.add_flash(req, str(exception))

    template_resolver.resolve(req, 'show.mako').render_context(
        context, req = req, model_ns = model_ns, model = model)
%>
