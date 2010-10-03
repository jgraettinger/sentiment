
import getty
import sqlalchemy.orm
import sqlalchemy as sa

from webob import Response
from webob.dec import wsgify
from webob.exc import HTTPNotFound

from mako.lookup import TemplateLookup

import zuul.wsgi_route
from zuul.model import Document
from zuul.controller import common, secure_token, orm_session
from zuul.view.template_resolver import TemplateResolver


class DocumentAction(object):

    @getty.requires(
        app = zuul.wsgi_route.WSGIWare,
        templates = TemplateLookup)
    def __init__(self, app, templates):
        self.app = app
        self.template_resolver = TemplateResolver(
            templates, ['document', 'resource'])

    def render(self, req, template, **kwargs):
        template = self.template_resolver.resolve(req, template)

        req.resp = Response()
        req.resp.body = template.render(
            req = req,
            model_ns = '/model/document.mako',
            template_resolver = self.template_resolver,
            **kwargs)
        return req.resp


###############################################################################
###    Non-Mutating Actions

class query(DocumentAction):
    @wsgify
    def __call__(self, req):

        q = req.session.query(Document)

        if 'id' in req.route_args:
            q = q.filter_by(id = req.route_args['id'])
        elif 'id' in req.GET:
            q = q.filter(Document.id.in_(req.GET.getall('id')))

        if 'title' in req.GET:
            q = q.filter(Document.title.in_(req.GET.getall('title')))
        if 'author' in req.GET:
            q = q.filter(Document.author.in_(req.GET.getall('author')))

        req.query = q
        try:
            return req.get_response(self.app)
        except sa.orm.exc.NoResultFound:
            raise HTTPNotFound('No such Document')

class index(DocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'index.mako', models = req.query)

class new(DocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'new.mako')

class edit(DocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'edit.mako', model = req.query.one())

class show(DocumentAction):
    @wsgify
    def __call__(self, req):

        inst = req.query.one()
        return self.render(req, 'show.mako', model = inst,
            child_groups = [
                ('/model/clustering.mako', inst.clusterings)])

###############################################################################
###    Mutating Actions

class create(DocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = dict((k, v) for k, v in req.POST.items() if k[0] != '_')
            inst = Document(**fields)
            req.session.add(inst)
            req.session.commit()
            return self.render(req, 'create_okay.mako', model = inst)

        except Exception, e:
            return self.render(req, 'create_fail.mako', exception = e)

class update(DocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = Document.validate_update(req.POST)
            row_count = req.query.update(fields, synchronize_session = False)
            req.session.commit()
            return self.render(req, 'update_okay.mako', row_count = row_count)

        except Exception, e:
            return self.render(req, 'update_fail.mako', exception = e)

class delete(DocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            row_count = req.query.delete(synchronize_session = False)
            req.session.commit()
            return self.render(req, 'delete_okay.mako', row_count = row_count)

        except Exception, e:
            return self.render(req, 'delete_fail.mako', exception = e)

###############################################################################
###    Action <=> URL Bindings

def bind_actions(dispatcher):

    common.create_resource_routes(dispatcher, 'document')

    bind = lambda action, stack: \
        dispatcher.bind_stack('document', action, stack)

    bind('index',  [orm_session.add_session, secure_token.add_token, query, index])
    bind('new',    [secure_token.add_token, new])
    bind('create', [orm_session.add_session, secure_token.check_and_add_token, create])
    bind('show',   [orm_session.add_session, secure_token.add_token, query, show])
    bind('edit',   [orm_session.add_session, secure_token.add_token, query, edit])
    bind('update', [orm_session.add_session, secure_token.check_and_add_token, query, update])
    bind('delete', [orm_session.add_session, secure_token.check_and_add_token, query, delete])

