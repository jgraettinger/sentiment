<%! import simplejson %>
<%page args="req, model_ns, row_count"/>
<%
    req.resp.content_type = 'application/json'
    req.resp.content_type_params = {'charset': 'utf8'}

    context.write(simplejson.dumps(
        {'status': 'OK', 'updated_row_count': row_count}))
%>
