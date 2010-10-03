<%! import simplejson %>
<%page args="req, model_ns, model"/>
<%
    req.resp.content_type = 'application/json'
    req.resp.content_type_params = {'charset': 'utf8'}

    model = model.flatten()
    context.write(simplejson.dumps(model))
%>
