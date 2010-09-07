
import formencode
import formencode.validators as fva

from base import BaseModel

class Clustering(BaseModel):

    class Schema(formencode.Schema):
        id = fva.Int(if_missing = None)
        name = fva.UnicodeString(strip = True, not_empty = True, max = 128)

    @classmethod
    def define_orm_mapping(kls, orm):
        import sqlalchemy.orm
        import sqlalchemy as _

        tab = _.Table('clustering', orm.meta,
            _.Column('id', _.Integer, primary_key = True),
            _.Column('name', _.String,
                index = True, unique = True, nullable = False),
        )
        orm.clustering = sqlalchemy.orm.mapper(kls, tab)
        return

