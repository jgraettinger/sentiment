<%inherit file="/model.mako"/>
<%!
    import webhelpers.html.tags
    import webhelpers.html as h
    import simplejson
%>

<%def name="controller()">
    <% return 'regression' %>
</%def>

<%def name="view_collection(req, models)">
    <%
        by_class = {}
        for m in models:
            by_class.setdefault(
                tuple(m.class_names_list), []).append(m)
    %>
    <table class="collection">
    %for classes, models in by_class.items():

        <tr><td colspan="3">
        <div class="collection_header">
                ${", ".join(classes).title()|h}
        </div></td></tr>

        %for model in models:
            <%
                conf_overrides = simplejson.dumps(
                    model.config_overrides_dict, indent = 1)

                show_uri = self.show_uri(req, model)
            %>
            <tr>
                <td class="collection_column collection_column_left">
                    <a href="${show_uri}">${model.name|h}</a><br>
                    ${model.clusterer|h}<br>
                    ${model.feature_transform or '<No Feature Transform>'|h}<br>
                    ${model.vinz_version|h}<br>
                </td>
                <td class="collection_column">
                    ${conf_overrides|h}<br>
                </td>
                <td class="collection_column collection_column_right">
                    <div class="regression_collection_plot" id="reg_plot_${model.id}">
                    </div>
                    <center>
                    <% reg_results = model.regression_results() %>
                    %if reg_results and all(i[2]>= i[1] for i in reg_results):
                        <span style="color: green">Passes</span>
                    %elif reg_results:
                        <span style="color: red">Fails</span>
                    %else:
                        ${self.run_link(req, model)}
                    %endif
                    </center>
                </td>
            </tr>
        %endfor
    %endfor
    </table>
</%def>

<%def name="view_instance(req, model)">
    <%
        conf_overrides = simplejson.dumps(
            model.config_overrides_dict, indent = 1)
    %>
    <div class="regression_instance_plot" id="reg_plot_${model.id}">
    </div>
    <div style="float: right">
        ${self.view_regression_results_table(req, model)}
    </div>
    <table class="collection">
        <tr><td colspan="3"><div class="collection_header">
                ${", ".join(model.class_names_list).title()|h}
        </div></td></tr>
        <tr>
            <td class="collection_column collection_column_left">
                ${model.clusterer|h}<br>
                ${model.feature_transform|h}<br>
                ${model.vinz_version|h}<br>
            </td>
            <td class="collection_column">
                ${conf_overrides|h}<br>
            </td>
            <td class="collection_column collection_column_right">
                <center>${self.run_link(req, model)}</center>
            </td>
        </tr>
    </table>
</%def>

<%def name="view_regression_results_table(req, model)">
    <% results = model.regression_results() %>
    <table class="results">
        <tr><td colspan="3" class="results_header">
            Regression Results
        </td></tr>
        %if not results:
        <tr><td colspan="3" class="results_cell_red">
                <i>Test hasn't yet run...</i>
        </td></tr>
        %endif
        %for test_name, exp_value, act_value in results:
        <tr>
            <%
                style = 'results_cell_green'
                if act_value < exp_value:
                    style = 'results_cell_red'
            %>
            <td class="${style}">
                ${test_name|h}
            </td>
            <td class="${style}">
                ${'%0.4f' % exp_value|h}
            </td>
            <td class="${style}">
                ${'%0.4f' % act_value|h}
            </td>
        </tr>
        %endfor
    </table>
</%def>

<%def name="model_description()">
    Regression
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

    Name: ${h.tags.text(
        name = 'name', size = "80",
        value = m.get('name', ''))}<br>

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

    Expected Results: ${h.tags.textarea(
        name = 'expected_results',
        rows = "10", cols = "80",
        content = m.get('expected_results', '{}'))}</br>

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

