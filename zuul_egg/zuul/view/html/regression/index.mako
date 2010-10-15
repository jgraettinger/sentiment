<%inherit file="/html/resource/index.mako"/>
<%page args="req, model_ns, models"/>
${parent.body(req, model_ns, models)}

<%def name="external_css()">
    <link rel="stylesheet" type="text/css" href="static/site.css"/>
</%def>
<%def name="external_javascript()">
    <script language="javascript" type="text/javascript" src="/static/jquery-1.4.2.min.js"></script>
    <script language="javascript" type="text/javascript" src="/static/flot-0.6/jquery.flot.js"></script>
</%def>
<%def name="head_javascript()">
    <script id="source" language="javascript" type="text/javascript">

        function onDataReceived(data, div_id, uri) {

            var flot_data = []
            for(var i in data.class_names)
            {
                var label = data.class_names[i];

                prec = data.stats.precision[label];
                recall = data.stats.recall[label];
                fmeasure = [];

                for(var j in prec)
                {
                    fmeasure.push([j,
                        2 * prec[j] * recall[j] / (prec[j] + recall[j])]);
                }
                flot_data.push({data: fmeasure});
            }

            // and plot all we got
            $.plot($(div_id), flot_data, {
                xaxis: {min: 0.0, max: data.series_length},
                yaxis: {min: 0.0, max: 1.0}});

            if(!data.is_running)
                return;

            // schedule an update of the plot
            setTimeout( function() {
                $.ajax({
                    url: uri,
                    method: 'GET',
                    dataType: 'json',
                    success: function(data) {
                        onDataReceived(data, div_id, uri);}
                });
            }, 1000);
        }

        $(document).ready( function() {
        %for model in models:
            <% url = '"/regression/%s/results?precision&recall"' % model.id %>

            $.ajax({
                url: ${url},
                method: 'GET',
                dataType: 'json',
                success: function(data) {onDataReceived(
                    data, "#reg_plot_${model.id}", ${url});}
            });

        %endfor
        });

    </script>
</%def>
