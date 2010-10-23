<%inherit file="/html/resource/show.mako"/>
<%page args="req, model_ns, model, child_groups"/>
${parent.body(req, model_ns, model, child_groups)}

<%def name="external_css()">
    <link rel="stylesheet" type="text/css" href="/static/site.css"/>
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
                    flot_data.push({data: flot_series,
                        label: stat_name});

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
                        flot_data.push({data: flot_series,
                            label: cname + '-' + stat_name});
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
            <% url = '"/regression/%s/results?precision&recall"' % model.id %>

            $.ajax({
                url: ${url},
                method: 'GET',
                dataType: 'json',
                success: function(data) {onDataReceived(
                    data, "#reg_plot_${model.id}", ${url});}
            });
        });


    </script>
</%def>
