
class BaseModel(object):

    def __init__(self, **args):
        args = self.validate_new(args)

        for k, v in args.items():
            setattr(self, k, v)
        return

    @classmethod
    def validate_new(cls, args):

        schema = getattr(cls, '_schema', None)
        if not schema:
            schema = cls.Schema()
            cls._schema = schema

        args = cls.validate_common(args)
        return schema.to_python(args)

    @classmethod
    def validate_update(cls, args):

        schema = getattr(cls, '_schema_update', None)
        if not schema:
            schema = cls.Schema(ignore_key_missing = True)
            cls._schema_update = schema

        args = dict((k, v) for k, v in args.items() if k[0] != '_')
        args = cls.validate_common(args)
        vargs = schema.to_python(args)
        # strip any added, defaulted arguments (eg, 'id')
        return dict((k, v) for k, v in vargs.items() if k in args)

    @classmethod
    def validate_common(cls, args):
        return args

    def flatten(self):

        schema = getattr(type(self), '_schema', None)
        if not schema:
            schema = type(self).Schema()
            type(self)._schema = schema

        args = dict((k, getattr(self, k)) for k in schema.fields)
        return schema.from_python(args)

