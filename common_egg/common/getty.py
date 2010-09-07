import sys
import types

class Singleton(object):
    pass
class Dynamic(object):
    pass

class Config(object):
    def __init__(self, annotation):
        self.annotation = annotation

class InjectionState(object):
    __slots__ = ['inst', 'annot', 'req']

    def __init__(self):
        self.inst = None
        self.annot = {}
        self.req = {}

class AnnotationDescription(object):
    def __init__(self, **kwargs):
        self.annot = kwargs

    def __call__(self, init):

        # Validate arguments
        for key in self.annot:
            assert key in init.func_code.co_varnames, key

        kls_dict = sys._getframe(1).f_locals
        kls_dict.setdefault(
            '__getty__', InjectionState()
        ).annot = self.annot
        return init

class RequirementDescription(object):
    def __init__(self, **kwargs):
        self.req = kwargs

    def __call__(self, init):

        # injected args are in func arg list
        for key in self.req:
            assert key in init.func_code.co_varnames, key
        # no un-injected func args
        for key in init.func_code.co_varnames[1:init.func_code.co_argcount]:
            assert key in self.req, key

        kls_dict = sys._getframe(1).f_locals
        istate = kls_dict.setdefault(
            '__getty__', InjectionState())

        for kw, val in self.req.items():
            if isinstance(val, Config):
                istate.req[kw] = Config
                istate.annot[kw] = val.annotation
            else:
                istate.req[kw] = val

        return init

class Extension(object):

    def __init__(self, kls):
        self.istate = kls.__getty__ = InjectionState()

    def requires(self, **kwargs):

        for kw, val in kwargs.items():
            if isinstance(val, Config):
                self.istate.req[kw] = Config
                self.istate.annot[kw] = val.annotation
            else:
                self.istate.req[kw] = val

    def annotation(self, **kwargs):
        self.istate.annot = kwargs

class Injector(object):

    def __init__(self):
        self._bindings = {}

    def get_instance(self, key_kls, annotation = None):

        if annotation and (key_kls, annotation) not in self._bindings:
            raise RuntimeError("No bound class %r with annotation %r" % (
                key_kls, annotation))

        kls, scope = self._bindings.get(
            (key_kls, annotation), (key_kls, Dynamic))

        if scope not in (Dynamic, Singleton):
            # scope is already instance
            return scope

        ctor_args = {}

        # Look for __getty__ decorations of kls, or
        #  a base of kls *sharing kls's __init__ method*
        for cur_kls in kls.mro():

            if not isinstance(cur_kls.__init__, types.MethodType):
                break

            if cur_kls.__init__.im_func is not kls.__init__.im_func:
                # different (non-inherited) __init__ methods
                break

            if '__getty__' in cur_kls.__dict__:
                inj_state = kls.__getty__

                for r_name, r_kls in inj_state.req.items():
                    ctor_args[r_name] = self.get_instance(
                        r_kls, inj_state.annot.get(r_name, None))
                break

        new_inst = kls(**ctor_args)

        if scope == Singleton:
            self._bindings[(key_kls, annotation)] = (kls, new_inst)

        return new_inst 

    def bind(self, key_kls, to = None, with_annotation = None, scope = Dynamic):
        to = key_kls if to is None else to
        self._bindings[(key_kls, with_annotation)] = (to, scope)

    def bind_instance(self, key_kls, to, with_annotation = None):
        self._bindings[(key_kls, with_annotation)] = (type(to), to)

requires = RequirementDescription
annotation = AnnotationDescription

