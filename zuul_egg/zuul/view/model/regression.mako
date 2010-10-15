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
    %for classes, models in by_class.items():

        <div class="collection_header">
            ${", ".join(classes).title()|h}
        </div>

        %for model in models:
            <%
                conf_overrides = simplejson.dumps(
                    model.config_overrides_dict, indent = 1)

                show_uri = self.show_uri(req, model)
            %>
            <div class="collection_item">
                <div class="collection_colum">
                    ${model.clusterer|h}
                </div>
                <div class="collection_colum">
                    ${model.feature_transform|h}
                </div>
                <div class="collection_colum">
                    ${model.vinz_version|h}
                </div>
                <div class="collection_colum">
                    ${conf_overrides|h}
                </div>
                <div class="collection_colum">
                    ${self.run_link(req, model)}
                </div>
                
                <a href="${show_uri}">
                <div class="regression_collection_plot" id="reg_plot_${model.id}">
                </div>
                </a>
            </div>
        %endfor
    %endfor
</%def>

<%def name="view_instance(req, model)">
    <%
        conf_overrides = simplejson.dumps(
            model.config_overrides_dict, indent = 1)
    %>
    <div class="regression_instance_plot" id="reg_plot_${model.id}">
    </div>
    <div class="collection_header">
        ${", ".join(model.class_names_list).title()|h}
    </div>
    <div class="collection_item">
        <div class="collection_colum">
            ${model.clusterer|h}
        </div>
        <div class="collection_colum">
            ${model.feature_transform|h}
        </div>
        <div class="collection_colum">
            ${model.vinz_version|h}
        </div>
        <div class="collection_colum">
            ${conf_overrides|h}
        </div>
        <div class="collection_colum">
            ${self.run_link(req, model)}
        </div>
    </div>
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

