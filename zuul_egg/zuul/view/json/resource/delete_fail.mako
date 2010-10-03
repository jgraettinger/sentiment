<%! import simplejson %>
<%page args="req, model_ns, exception"/>
<%
    req.resp.content_type = 'application/json'
    req.resp.content_type_params = {'charset': 'utf8'}

    context.write(simplejson.dumps(
        {'status': 'ERROR', 'reason': str(exception)}))
%>
