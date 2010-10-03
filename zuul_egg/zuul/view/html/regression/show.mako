<%inherit file="/html/resource/show.mako"/>
<%page args="req, model_ns, model, child_groups"/>
${parent.body(req, model_ns, model, child_groups)}

<%def name="external_javascript()">
    <script language="javascript" type="text/javascript" src="/static/jquery-1.4.2.min.js"></script>
    <script language="javascript" type="text/javascript" src="/static/flot-0.6/jquery.flot.js"></script>
</%def>
<%def name="head_javascript()">
    <script id="source" language="javascript" type="text/javascript">

        function onDataReceived(data, divid, uri) {
            // extract the first coordinate pair so you can see that
            // data is now an ordinary Javascript object

            var flot_data = []
            for(var label in data.fmeasure)
            {
                flot_data.push({data: data.fmeasure[label]})
            }

            // and plot all we got
            $.plot($(divid), flot_data);
        }

        <% url = '"/regression/%s/results?fmeasure"' % model.id %>
        $.ajax({
            url: ${url},
            method: 'GET',
            dataType: 'json',
            success: function(data) {onDataReceived(data, "#reg_plot_${model.id}", ${url});}
        });

    </script>
</%def>
