<%namespace name="ui_flash" file="/ui_flash.mako"/>
<%page args="req, model_ns, exception, template_resolver"/>
<%
    ui_flash.add_flash(req, str(exception))

    temp = list(req.query.limit(2))

    # If there's just one instance, show an edit page for it
    if len(temp) == 1:
        template_resolver.resolve(req, 'edit.mako').render_context(
            context, req = req, model_ns = model_ns, model = temp[0])
    # Otherwise, show the index
    else:
        template_resolver.resolve(req, 'index.mako').render_context(
            context, req = req, model_ns = model_ns, models = req.query)
%>
