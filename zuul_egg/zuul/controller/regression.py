
import getty
import eventlet
import simplejson

import sqlalchemy.orm
import sqlalchemy as sa

from webob import Response
from webob.dec import wsgify
from webob.exc import HTTPNotFound

from mako.lookup import TemplateLookup

import zuul.wsgi_route
from zuul.model import Regression
from zuul.controller import common, secure_token, orm_session


class RegressionAction(object):

    @getty.requires(
        app = zuul.wsgi_route.WSGIWare,
        templates = TemplateLookup)
    def __init__(self, app, templates):
        self.app = app
        self.template_lookup = templates

    def render(self, req, view_def, **kwargs):
        template = self.template_lookup.get_template(
            '/%s/resource.mako' % req.route_args['format'])
        #template = self.template_lookup.get_template(
        #    '/%s/regression.mako' % req.route_args['format'])

        req.resp = Response()
        req.resp.body = template.get_def(view_def).render(
            req = req, model_ns = '/model/regression.mako', **kwargs)
        return req.resp

###############################################################################
###    Non-Mutating Actions

class query(RegressionAction):
    @wsgify
    def __call__(self, req):

        q = req.session.query(Regression)

        if 'id' in req.route_args:
            q = q.filter_by(id = req.route_args['id'])
        elif 'id' in req.GET:
            q = q.filter(Regression.id.in_(req.GET.getall('id')))

        if 'class_name' in req.GET:
            class_names = ', '.join(sorted(req.GET.getall('class_name')))
            q = q.filter_by(class_names = class_names)

        req.query = q
        try:
            return req.get_response(self.app)
        except sa.orm.exc.NoResultFound:
            raise HTTPNotFound('No Such Regression')

class index(RegressionAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'index', models = req.query)

class new(RegressionAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'new')

class edit(RegressionAction):
    @wsgify
    def __call__(self, req):
        return self.render(req, 'edit', model = req.query.one())

class show(RegressionAction):
    @wsgify
    def __call__(self, req):

        inst = req.query.one()
        return self.render(req, 'show', model = inst,
            child_groups = [])

class results(RegressionAction):
    @wsgify
    def __call__(self, req):

        inst = req.query.one()

        results = {'series_length': inst.iteration_count}
        if 'precision' in req.GET:
            results['precision'] = inst.precision
        if 'recall' in req.GET:
            results['recall'] = inst.recall
        if 'fmeasure' in req.GET:
            results['fmeasure'] = {}
            
            for label in inst.precision:
                results['fmeasure'][label] = [(p[0], p[1] * r[1]) for \
                    p, r in zip(inst.precision[label], inst.recall[label])]

        resp = Response()
        resp.content_type = 'application/json'
        resp.content_type_params = {'charset': 'utf8'}
        resp.body = simplejson.dumps(results)
        return resp

###############################################################################
###    Mutating Actions

class create(RegressionAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = dict((k, v) for k, v in req.POST.items() if k[0] != '_')
            inst = Regression(**fields)
            req.session.add(inst)
            req.session.commit()
            return self.render(req, 'create_okay', model = inst)

        except Exception, e:
            return self.render(req, 'create_fail', exception = e)

class update(RegressionAction):
    @wsgify
    def __call__(self, req):

        try:
            fields = Regression.validate_update(req.POST)
            row_count = req.query.update(fields, synchronize_session = False)
            req.session.commit()
            return self.render(req, 'update_okay', row_count = row_count)

        except Exception, e:
            return self.render(req, 'update_fail', exception = e)

class delete(RegressionAction):
    @wsgify
    def __call__(self, req):

        try:
            row_count = req.query.delete(synchronize_session = False)
            req.session.commit()
            return self.render(req, 'delete_okay', row_count = row_count)

        except Exception, e:
            return self.render(req, 'delete_fail', exception = e)

class run(RegressionAction):
    @wsgify
    def __call__(self, req):

        inst = req.query.one()
        if inst.is_running():
            return self.render(req, 'run_fail',
                exception = AssertionError('Regression is already running'))
        try:
            thread = eventlet.spawn(inst.run)
            return self.render(req, 'run_okay', model = inst, thread = thread)
        except Exception, e:
            return self.render(req, 'run_fail', exception = e)

###############################################################################
###    Action <=> URL Bindings

def bind_actions(dispatcher):

    common.create_resource_routes(dispatcher, 'regression')

    dispatcher.mapper.connect('/regression/{id}.{format}',
        controller = 'regression', action = 'run',
        conditions = dict(method = ['RUN']))

    dispatcher.mapper.connect('/regression/{id}/results',
        controller = 'regression', action = 'results',
        conditions = dict(method = ['GET']))

    bind = lambda action, stack: \
        dispatcher.bind_stack('regression', action, stack)

    bind('index',  [orm_session.add_session, secure_token.add_token, query, index])
    bind('new',    [secure_token.add_token, new])
    bind('create', [orm_session.add_session, secure_token.check_and_add_token, create])
    bind('show',   [orm_session.add_session, secure_token.add_token, query, show])
    bind('edit',   [orm_session.add_session, secure_token.add_token, query, edit])
    bind('update', [orm_session.add_session, secure_token.check_and_add_token, query, update])
    bind('delete', [orm_session.add_session, secure_token.check_and_add_token, query, delete])
    bind('run',    [orm_session.add_session, secure_token.check_and_add_token, query, run])
    bind('results',[orm_session.add_session, query, results])

