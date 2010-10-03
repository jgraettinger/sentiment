<%!
    import base64
    import simplejson
%>
<%def name="apply_pending_flashes(req)">
    <%
        cookie_flashes = simplejson.loads(base64.b64decode(
            req.cookies.get('ui_flashes') or 'W10='))

        req.ui_flashes = getattr(req,
            'ui_flashes', []) + cookie_flashes

        if cookie_flashes:
            req.resp.set_cookie('ui_flashes', '')
    %>
</%def>
<%def name="add_pending_flash(req, msg)">
    <%
        pending_flashes = getattr(req.resp, 'pending_ui_flashes', [])
        pending_flashes.append(msg)
        req.resp.pending_ui_flashes = pending_flashes

        req.resp.set_cookie('ui_flashes',
            base64.b64encode(simplejson.dumps(pending_flashes)))
    %>
</%def>
<%def name="add_flash(req, msg)">
    <%
        flashes = getattr(req, 'ui_flashes', [])
        flashes.append(msg)
        req.ui_flashes = flashes
    %>
</%def>
<%def name="display_flashes(req)">
    <% flashes = getattr(req, 'ui_flashes', []) %>
    %for flash in flashes:
        <div class="ui-flash">${flash}</div>
    %endfor
</%def>
