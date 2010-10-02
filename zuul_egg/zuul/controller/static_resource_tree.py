
import zuul.wsgi_route
import paste.fileapp

from webob import Response
from webob.dec import wsgify
from webob.exc import HTTPNotFound

import getty

class static_resource_tree(paste.fileapp.DirectoryApp):
    def __init__(self):
        full_path = zuul.__path__[0] + '/../static_resources'
        paste.fileapp.DirectoryApp.__init__(self, full_path)
        return

def bind_actions(dispatcher):

    dispatcher.mapper.connect(None, '/static/{path_info:.*}',
        controller = 'static', action = 'static')

    dispatcher.bind_stack('static', 'static',
        [static_resource_tree])

