
import routes.middleware
import getty

class WSGIWare(object):
    """
    Dependency-injection placeholder class, which represents
    a callable, downstream WSGI application.
    """
    pass

class Dispatcher(object):

    @getty.requires(
        injector = getty.Injector,
        mapper = routes.Mapper)
    def __init__(self, injector, mapper):

        self.mapper = mapper
        self.injector = injector

        self._stacks = {}
        self._apps = {}

        self._route_ware = routes.middleware.RoutesMiddleware(
            self.__call_inner, self.mapper)
        return

    def bind_stack(self, controller, action, stack):

        assert (controller, action) not in self._stacks
        self._stacks[(controller, action)] = stack

    def _instantiate_stack(self, controller, action):

        stack = self._stacks[(controller, action)]

        app = None
        for factory in stack[::-1]:

            if isinstance(factory, type):

                # factory is a class; obtain from injector
                self.injector.bind_instance(WSGIWare, to = app)
                app = self.injector.get_instance(factory)

            else:

                # factory is a callable; call directly
                app = factory(app)

        self._apps[(controller, action)] = app
        return app

    def __call__(self, environ, start_resp):
        return self._route_ware(environ, start_resp)

    def __call_inner(self, environ, start_resp):
        route_args = environ['wsgiorg.routing_args'][1]
        route_args.setdefault('format', 'html')

        environ['webob.adhoc_attrs'] = {}
        environ['webob.adhoc_attrs']['route_args'] = route_args
        environ['webob.adhoc_attrs']['make_url'] = \
            environ['wsgiorg.routing_args'][0].current

        app_key = (route_args['controller'], route_args['action'])

        wsgi_app = self._apps.get(app_key, None)
        if not wsgi_app:
            wsgi_app = self._instantiate_stack(*app_key)

        return wsgi_app(environ, start_resp)

