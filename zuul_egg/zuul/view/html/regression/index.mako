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

            if(data.is_running)
            {
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

            if(!data.has_results)
            {
                // placeholder plot
                $.plot($(div_id), [{data: []}], {
                    xaxis: {min: 0.0, max: data.series_length},
                    yaxis: {min: 0.0, max: 1.0},
                });
                return;
            }

            var flot_data = [];
            for(var stat_name in data.stats)
            {
                var stat_val = data.stats[stat_name];

                if(stat_val instanceof Array)
                {
                    var flot_series = [];

                    // flat statistic (not per-class)
                    for(var j in stat_val)
                    {
                        flot_series.push([j, stat_val[j]]);
                    }
                    flot_data.push({data: flot_series});

                } else {

                    // statistic is per-class
                    for(var i in data.class_names)
                    {
                        var cname = data.class_names[i];
                        var sub_val = stat_val[cname];
                        var flot_series = [];

                        for(var j in sub_val)
                        {
                            flot_series.push([j, sub_val[j]]);
                        }
                        flot_data.push({data: flot_series});
                    }
                }
            }

            // plot all we got
            $.plot($(div_id), flot_data, {
                xaxis: {min: 0.0, max: data.series_length},
                yaxis: {min: 0.0, max: 1.0},
                legend: {position: "se", backgroundOpacity: 0.3},
            });
        }

        $(document).ready( function() {
        %for model in models:
            <% url = '"/regression/%s/results?fmeasure"' % model.id %>

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
