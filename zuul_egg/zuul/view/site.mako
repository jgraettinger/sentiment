<%page args="req, **kwargs"/>
<%namespace name="ui_flash" file="/ui_flash.mako"/>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
<title>${self.head_title()}</title>
${self.external_css()}
${self.external_javascript()}
${self.head_tags()}
${self.head_javascript()}
</head>
<body>
%if req.headers.get('X-Requested-With') != 'XMLHttpRequest':
    ${ui_flash.apply_pending_flashes(req)}
%endif
${ui_flash.display_flashes(req)}
${self.body(req = req, **kwargs)}
</body>
</html>

<%def name="head_title()"></%def>
<%def name="external_css()"></%def>
<%def name="external_javascript()"></%def>
<%def name="head_tags()"></%def>
<%def name="head_javascript()"></%def>
