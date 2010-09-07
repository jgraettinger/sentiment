
import paste.evalexception
import wsgi_route
import getty

inj = getty.Injector()
inj.bind_instance(getty.Config,
    with_annotation = 'db_uri', to = 'sqlite:///:memory:')
inj.bind_instance(getty.Config,
    with_annotation = 'echo_sql', to = True)
inj.bind_instance(getty.Injector, to = inj)

inj.bind(wsgi_route.Dispatcher, scope = getty.Singleton)

# instantiate templating engine
import mako.lookup

inj.bind_instance(mako.lookup.TemplateLookup,
    to = mako.lookup.TemplateLookup(
        directories = ['zuul/view'],
        input_encoding = 'utf-8',
        output_encoding = 'utf-8',
        format_exceptions = True,
        #filesystem_checks = False,
        ))

# Set up the ORM
import model.meta
inj.bind(model.meta.ORM, scope = getty.Singleton) 

# Set up routes, & bind those routes to actions
import controller.meta
inj.get_instance(controller.meta.DispatchMapping)

# Bootstrap the application
application = inj.get_instance(wsgi_route.Dispatcher)
application = paste.evalexception.EvalException(application)

# start the server
import gevent.wsgi
gevent.wsgi.WSGIServer(('localhost', 8080), application).serve_forever()

