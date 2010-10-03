

import getty
import sqlalchemy.orm
import sqlalchemy as sa

from webob import Response
from webob.dec import wsgify
from webob.exc import HTTPNotFound

from mako.lookup import TemplateLookup

import zuul.wsgi_route
from zuul.model import ClusterDocument
from zuul.controller import common, secure_token, orm_session
from zuul.view.template_resolver import TemplateResolver


class ClusterDocumentAction(object):

    @getty.requires(
        app = zuul.wsgi_route.WSGIWare,
        templates = TemplateLookup)
    def __init__(self, app, templates):
        self.app = app
        self.template_resolver = TemplateResolver(
            templates, ['cluster_document', 'resource'])

    def render(self, req, view_def, **kwargs):
        template = self.template_resolver.resolve(req, template)

        req.resp = Response()
        req.resp.body = template.render(
            req = req,
            model_ns = '/model/cluster_document.mako',
            template_resolver = self.template_resolver,
            **kwargs)
        return req.resp

###############################################################################
###    Non-Mutating Actions

class query(ClusterDocumentAction):
    @wsgify
    def __call__(self, req):

        q = req.session.query(ClusterDocument)

        if 'clustering_id' in req.GET:
            q = q.filter(ClusterDocument.clustering_id.in_(
                req.GET.getall('clustering_id')))
        if 'document_id' in req.GET:
            q = q.filter(ClusterDocument.document_id.in_(
                req.GET.getall('document_id')))
        if 'cluster_id' in req.GET:
            q = q.filter(ClusterDocument.cluster_id.in_(
                req.GET.getall('cluster_id')))

        req.query = q
        try:
            return req.get_response(self.app)
        except sa.orm.exc.NoResultFound:
            raise HTTPNotFound('No Such Cluster / Document Relation')

class index(ClusterDocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'index.mako', models = req.query)

class new(ClusterDocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'new.mako')

class edit(ClusterDocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'edit.mako', model = req.query.one())

###############################################################################
###    Mutating Actions

class create(ClusterDocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = dict((k, v) for k, v in req.POST.items() if k[0] != '_')
            inst = ClusterDocument(**fields)
            req.session.add(inst)
            req.session.commit()
            return self.render(req, 'create_okay.mako', model = inst)

        except Exception, e:
            return self.render(req, 'create_fail.mako', exception = e)

class update(ClusterDocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = ClusterDocument.validate_update(req.POST)
            row_count = req.query.update(fields, synchronize_session = False)
            req.session.commit()
            return self.render(req, 'update_okay.mako', row_count = row_count)

        except Exception, e:
            return self.render(req, 'update_fail.mako', exception = e)


class delete(ClusterDocumentAction):
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
    common.create_relation_routes(dispatcher, 'cluster_document') 

    bind = lambda action, stack: \
        dispatcher.bind_stack('cluster_document', action, stack)

    bind('index',  [orm_session.add_session, secure_token.add_token, query, index])
    bind('new',    [secure_token.add_token, new])
    bind('create', [orm_session.add_session, secure_token.check_and_add_token, create])
    bind('edit',   [orm_session.add_session, secure_token.add_token, query, edit])
    bind('update', [orm_session.add_session, secure_token.check_and_add_token, query, update])
    bind('delete', [orm_session.add_session, secure_token.check_and_add_token, query, delete])

