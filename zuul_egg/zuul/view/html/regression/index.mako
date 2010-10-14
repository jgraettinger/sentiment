<%inherit file="/html/resource/index.mako"/>
<%page args="req, model_ns, models"/>
${parent.body(req, model_ns, models)}

<%def name="external_javascript()">
    <script language="javascript" type="text/javascript" src="/static/jquery-1.4.2.min.js"></script>
    <script language="javascript" type="text/javascript" src="/static/flot-0.6/jquery.flot.js"></script>
</%def>
<%def name="head_javascript()">
    <script id="source" language="javascript" type="text/javascript">

        function onDataReceived(data, div_id, uri) {

            var flot_data = []
            for(var label in data.precision)
            {
                prec = data.precision[label]
                recall = data.recall[label]
                fmeasure = []

                for(var i = 0; i != prec.length; ++i)
                {
                    fmeasure.push([i, 2 * prec[i] * recall[i] / (prec[i] + recall[i])]);
                }
                flot_data.push({data: fmeasure})
            }

            // and plot all we got
            $.plot($(div_id), flot_data);

            if(!data.is_running)
                return;

            // schedule an update of the plot
            setTimeout( function() {
                    $.ajax({
                        url: uri,
                        method: 'GET',
                        dataType: 'json',
                        success: function(data) {onDataReceived(data, div_id, uri);}
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
                success: function(data) {onDataReceived(data, "#reg_plot_${model.id}", ${url});}
            });

        %endfor
        });

    </script>
</%def>
