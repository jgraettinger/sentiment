
import getty
import wsgi_route

import model.meta
from webob.dec import wsgify
from webob import Request, Response

class ORMSessionAction(object):

    @getty.requires(
        orm = model.meta.ORM,
        app = wsgi_route.WSGIWare)
    def __init__(self, orm, app):
        self.orm = orm
        self.app = app

class add_session(ORMSessionAction):
    @wsgify
    def __call__(self, req):
        req.session = self.orm.session
        return self.app

