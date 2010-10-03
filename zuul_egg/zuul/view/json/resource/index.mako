<%! import simplejson %>
<%page args="req, model_ns, models"/>
<%
    req.resp.content_type = 'application/json'
    req.resp.content_type_params = {'charset': 'utf8'}

    context.write('[')
    for ind, model in enumerate(models):
        enc_mod = simplejson.dumps(model.flatten())

        if ind:
            context.write(',\r\n%s' % enc_mod)
        else:
            context.write(enc_mod)

    context.write(']')
%>
