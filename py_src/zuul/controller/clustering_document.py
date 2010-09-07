
import getty
import model
import wsgi_route

from webob import Response
from webob.dec import wsgify
from webob.exc import HTTPNotFound

import sqlalchemy.orm
import sqlalchemy as sa

from mako.lookup import TemplateLookup

from controller import common, secure_token, orm_session

class ClusteringDocumentAction(object):

    @getty.requires(
        app = wsgi_route.WSGIWare,
        templates = TemplateLookup)
    def __init__(self, app, templates):
        self.app = app
        self.template_lookup = templates

    def render(self, req, view_def, **kwargs):
        template = self.template_lookup.get_template(
            '/%s/resource.mako' % req.route_args['format'])

        req.resp = Response()
        req.resp.body = template.get_def(view_def).render(
            req = req, model_ns = '/model/clustering_document.mako', **kwargs)
        return req.resp

###############################################################################
###    Non-Mutating Actions

class query(ClusteringDocumentAction):
    @wsgify
    def __call__(self, req):

        q = req.session.query(model.ClusteringDocument)

        if 'clustering_id' in req.GET:
            q = q.filter(model.ClusteringDocument.clustering_id.in_(
                req.GET.getall('clustering_id')))
        if 'document_id' in req.GET:
            q = q.filter(model.ClusteringDocument.document_id.in_(
                req.GET.getall('document_id')))

        req.query = q
        try:
            return req.get_response(self.app)
        except sa.orm.exc.NoResultFound:
            raise HTTPNotFound('No Such Clustering / Document Relation')

class index(ClusteringDocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'index', models = req.query)

class new(ClusteringDocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'new')

class edit(ClusteringDocumentAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'edit', model = req.query.one())

###############################################################################
###    Mutating Actions

class create(ClusteringDocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = dict((k, v) for k, v in req.POST.items() if k[0] != '_')
            inst = model.ClusteringDocument(**fields)
            req.session.add(inst)
            req.session.commit()
            return self.render(req, 'create_okay', model = inst)

        except Exception, e:
            return self.render(req, 'create_fail', exception = e)

class update(ClusteringDocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = model.ClusteringDocument.validate_update(req.POST)
            row_count = req.query.update(fields, synchronize_session = False)
            req.session.commit()
            return self.render(req, 'update_okay', row_count = row_count)

        except Exception, e:
            return self.render(req, 'update_fail', exception = e)


class delete(ClusteringDocumentAction):
    @wsgify
    def __call__(self, req):

        try:
            row_count = req.query.delete(synchronize_session = False)
            req.session.commit()
            return self.render(req, 'delete_okay', row_count = row_count)

        except Exception, e:
            return self.render(req, 'delete_fail', exception = e)

###############################################################################
###    Action <=> URL Bindings

def bind_actions(dispatcher):

    common.create_relation_routes(dispatcher, 'clustering_document') 

    bind = lambda action, stack: \
        dispatcher.bind_stack('clustering_document', action, stack)

    bind('index',  [orm_session.add_session, secure_token.add_token, query, index])
    bind('new',    [secure_token.add_token, new])
    bind('create', [orm_session.add_session, secure_token.check_and_add_token, create])
    bind('edit',   [orm_session.add_session, secure_token.add_token, query, edit])
    bind('update', [orm_session.add_session, secure_token.check_and_add_token, query, update])
    bind('delete', [orm_session.add_session, secure_token.check_and_add_token, query, delete])

