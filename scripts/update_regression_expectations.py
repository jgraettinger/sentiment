import simplejson
import restful

srv = restful.Service('http://localhost:8080')
srv.push_post_param('_secure_token', srv.get('/secure_token'))

for reg in srv.get('/regression.json'):
    results = srv.get('/regression/%d/results?precision&recall&fmeasure&log_likelihood' % reg['id'])

    if not results['has_results']:
        print "Regression '%s' hasn't been run" % reg['name']
        continue

    expect = {}

    stats = results['stats']
    for stat_name, stat_val in stats.items():
        if isinstance(stat_val, dict):
            expect[stat_name] = {}
            for sub_name, sub_val in stat_val.items():
                expect[stat_name][sub_name] = sub_val[-1]
        else:
            expect[stat_name] = stat_val[-1]

    reg['expected_results'] = simplejson.dumps(expect)
    print srv.put(reg, '/regression/%d.json' % reg['id'])

