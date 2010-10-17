import simplejson
import restful

srv = restful.Service('http://localhost:8080')
srv.push_post_param('_secure_token', srv.get('/secure_token'))

for reg in srv.get('/regression.json'):
    results = srv.get('/regression/%d/results?precision&recall' % reg['id'])

    if not results['stats']['precision']:
        print "Regression '%s' hasn't been run" % reg['name']
        continue

    expect = {'precision': {}, 'recall': {}}
    for class_name, series in results['stats']['precision'].items():
        expect['precision'][class_name] = series[-1]
    for class_name, series in results['stats']['recall'].items():
        expect['recall'][class_name] = series[-1]

    reg['expected_results'] = simplejson.dumps(expect)
    print srv.put(reg, '/regression/%d.json' % reg['id'])

