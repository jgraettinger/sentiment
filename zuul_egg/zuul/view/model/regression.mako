<%inherit file="/model.mako"/>
<%!
    import webhelpers.html.tags
    import webhelpers.html as h
    import simplejson
%>

<%def name="controller()">
    <% return 'regression' %>
</%def>

<%def name="model_description()">
    Regression
</%def>

<%def name="instance_description(model)">
    Regression ${model.id} over ${model.class_names|h}
</%def>

<%def name="column_headers()"> 
    <td>Identifier</td>
    <td>Regression DB</td>
    <td>Class Names</td>
    <td>Clusterer</td>
    <td>Feature Transform</td>
    <td>Vinz Version</td>
    <td>Config Overrides</td>
    <td></td>
    <td>Results</td>
</%def>

<%def name="as_row(req, model)">
    <%
        conf_overrides = simplejson.dumps(
            model.config_overrides_dict, indent = 1)
    %>
    <td>${model.id|h}</td>
    <td>${model.regression_database|h}</td>
    <td>
    %for cname in model.class_names_list:
        ${cname|h}<br>
    %endfor
    </td>
    <td>${model.clusterer|h}</td>
    <td>${model.feature_transform|h}</td>
    <td>${model.vinz_version|h}</td>
    <td>${conf_overrides|h}</td>
    <td>${self.run_link(req, model)}</td>
    <td><div id="reg_plot_${model.id}" style="width:300px;height:150px;"></div></td>
</%def>

<%def name="form_elements(req, model)">
    <%
        m = model.flatten() if model else {}
        m.update(req.str_POST)
        m = dict((k, str(v).decode('utf8')) for k,v in m.items())
    %>
    Regression DB:  ${h.tags.text(
        name = 'regression_database', size = "80",
        value = m.get('regression_database', ''))}<br>

    Class Names: ${h.tags.text(
        name = 'class_names', size = "80",
        value = m.get('class_names', ''))}<br>

    Clusterer: ${h.tags.text(
        name = 'clusterer', size = "30",
        value = m.get('clusterer', 'SparseGaussEmClusterer'))}<br>

    Feature Transform: ${h.tags.text(
        name = 'feature_transform', size = "30",
        value = m.get('feature_transform', ''))}<br>

    Vinz Version: ${h.tags.text(
        name = 'vinz_version',
        value = m.get('vinz_version', 'Vinz'))}</br>

    Config Overrides: ${h.tags.textarea(
        name = 'config_overrides',
        rows = "10", cols = "80",
        content = m.get('config_overrides', '{}'))}</br>

    Iteration Count: ${h.tags.text(
        name = 'iteration_count',
        value = m.get('iteration_count', '100'))}<br>
</%def>

<%def name="run_link(req, model)">
    <% uri = self.run_uri(req, model) %>
    %if model.is_running:
        Running...
    %else:
        ${h.tags.form(uri, method = 'RUN')}
            ${h.tags.hidden('_secure_token', req.secure_token)}
            ${h.tags.submit(value = 'Run', name = '_commit')}
        ${h.tags.end_form()}
    %endif
</%def>

<%def name="run_uri(req, model)">
    <%
        return req.make_url(
            controller = self.controller(),
            action = 'run',
            id = model.id)
    %>
</%def>

