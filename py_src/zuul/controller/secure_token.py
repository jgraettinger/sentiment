
import getty
import wsgi_route

from webob.exc import HTTPForbidden
from webob.dec import wsgify
from webob import Request, Response

class SecureTokenAction(object):

    @getty.requires(app = wsgi_route.WSGIWare)
    def __init__(self, app):
        self.app = app

class add_token(SecureTokenAction):
    @wsgify
    def __call__(self, req):
        req.secure_token = 'foobarbaz'
        return self.app

class check_and_add_token(SecureTokenAction):
    @wsgify
    def __call__(self, req):

        if '_secure_token' not in req.params:
            raise HTTPForbidden("""
                The request did not include a secure token.

                Secure tokens are required to prevent certain
                kinds of cross-site request forgery attacks.""")

        token = req.params['_secure_token']

        if token != 'foobarbaz':
            raise HTTPForbidden("""
                The request included an invalid secure token.

                Secure tokens are required to prevent certain
                kinds of cross-site request forgery attacks.""")

        req.secure_token = 'foobarbaz'
        return self.app

class return_token(SecureTokenAction):
    @wsgify
    def __call__(self, req):
        return Response(req.secure_token)

def bind_actions(dispatcher):
    dispatcher.mapper.connect('secure_token', '/secure_token',
        controller = 'secure_token', action = 'get')

    dispatcher.bind_stack('secure_token', 'get',
        [add_token, return_token])

