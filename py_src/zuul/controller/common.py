
import cjson
import base64

from webob.dec import wsgify

def create_resource_routes(dispatcher, res_name):

    m = dispatcher.mapper

    m.redirect('/%s/' % res_name, '/%s.html' % res_name)

    m.resource(res_name, res_name)

    m.connect('update_%s' % res_name, '/%s.{format}' % res_name,
        controller = res_name, action = 'update',
        format = 'html', conditions = {'method': ['PUT']})

    return

def create_relation_routes(dispatcher, rel_name):

    m = dispatcher.mapper

    m.redirect('/%s/' % rel_name, '/%s.html' % rel_name)

    m.connect('list_%s' % rel_name, '/%s.{format}' % rel_name,
        controller = rel_name, action = 'index',
        format = 'html', conditions = {'method': ['GET']})

    m.connect('new_%s' % rel_name, '/%s/new.{format}' % rel_name,
        controller = rel_name, action = 'new', 
        format = 'html', conditions = {'method': ['GET']})

    m.connect('create_%s' % rel_name, '/%s.{format}' % rel_name,
        controller = rel_name, action = 'create',
        format = 'html', conditions = {'method': ['POST']})

    m.connect('edit_%s' % rel_name, '/%s/edit.{format}' % rel_name,
        controller = rel_name, action = 'edit', 
        format = 'html', conditions = {'method': ['GET']})

    m.connect('update_%s' % rel_name, '/%s.{format}' % rel_name,
        controller = rel_name, action = 'update',
        format = 'html', conditions = {'method': ['PUT']})

    m.connect('delete_%s' % rel_name, '/%s.{format}' % rel_name,
        controller = rel_name, action = 'delete', 
        format = 'html', conditions = {'method': ['DELETE']})

    return

