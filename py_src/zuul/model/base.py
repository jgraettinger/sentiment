
class BaseModel(object):

    def __init__(self, **kwargs):

        schema = getattr(type(self), '_schema_new', None)
        if not schema:
            schema = type(self).Schema()
            type(self)._schema_new = schema

        self.__dict__.update(schema.to_python(kwargs))
        return

    @classmethod
    def validate_update(cls, args):

        schema = getattr(cls, '_schema_update', None)
        if not schema:
            schema = cls.Schema(ignore_key_missing = True)
            cls._schema_update = schema

        # strip any added, defaulted arguments (eg, 'id')
        args = dict((k, v) for k, v in args.items() if k[0] != '_')
        vargs = schema.to_python(args)
        return dict((k, v) for k, v in vargs.items() if k in args)

    def flatten(self):

        schema = getattr(type(self), '_schema_flatten', None)
        if not schema:
            schema = type(self).Schema(
                allow_extra_fields = True, filter_extra_fields = True)
            type(self)._schema_flatten = schema

        return schema.from_python(self.__dict__)

