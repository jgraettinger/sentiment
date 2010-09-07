
import formencode
import formencode.validators as fva

from base import BaseModel

class Document(BaseModel):

    class Schema(formencode.Schema):
        id = fva.Int(if_missing = None)
        title   = fva.UnicodeString(strip = True, not_empty = True, max = 256)
        author  = fva.UnicodeString(strip = True, not_empty = True, max = 128)
        content = fva.UnicodeString(strip = True, not_empty = True, max = 4096)

    @classmethod
    def define_orm_mapping(kls, orm):
        import sqlalchemy.orm
        import sqlalchemy as _

        tab = _.Table('document', orm.meta,
            _.Column('id', _.Integer, primary_key = True),
            _.Column('title', _.String,
                index = True, unique = True, nullable = False),
            _.Column('author', _.String,
                index = True, nullable = False),
            _.Column('content', _.UnicodeText, nullable = False),
        )
        orm.document = sqlalchemy.orm.mapper(kls, tab)
        return

