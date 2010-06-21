import sys

class Singleton(object):
    pass
class Dynamic(object):
    pass

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

        # Validate arguments
        for key in self.req:
            assert key in init.func_code.co_varnames, key
        for key in init.func_code.co_varnames[1:]:
            assert key in self.req, key

        kls_dict = sys._getframe(1).f_locals
        kls_dict.setdefault(
            '__getty__', InjectionState()
        ).req = self.req
        return init

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
        if '__getty__' in kls.__dict__:
            inj_state = kls.__getty__

            for r_name, r_kls in inj_state.req.items():
                ctor_args[r_name] = self.get_instance(
                    r_kls, inj_state.annot.get(r_name, None))

        new_inst = kls(**ctor_args)

        if scope == Singleton:
            self._bindings[(key_kls, annotation)] = (kls, new_inst)

        return new_inst 

    def bind(self, key_kls, to, with_annotation = None, scope = Dynamic):
        self._bindings[(key_kls, with_annotation)] = (to, scope)

    def bind_instance(self, key_kls, to, with_annotation = None):
        self._bindings[(key_kls, with_annotation)] = (type(to), to)

requires = RequirementDescription
annotation = AnnotationDescription

