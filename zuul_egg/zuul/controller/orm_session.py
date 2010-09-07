
import zuul.model.meta
import zuul.wsgi_route

from webob.dec import wsgify
from webob import Request, Response

import getty

class ORMSessionAction(object):

    @getty.requires(
        orm = zuul.model.meta.ORM,
        app = zuul.wsgi_route.WSGIWare)
    def __init__(self, orm, app):
        self.orm = orm
        self.app = app

class add_session(ORMSessionAction):
    @wsgify
    def __call__(self, req):
        req.session = self.orm.session
        return self.app

