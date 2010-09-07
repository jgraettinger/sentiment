
import formencode
import formencode.validators as fva

from base import BaseModel

class Cluster(BaseModel):

    class Schema(formencode.Schema):
        id = fva.Int(if_missing = None)
        clustering_id = fva.Int(not_empty = True)
        name = fva.UnicodeString(strip = True, not_empty = True, max = 128)

    @classmethod
    def define_orm_mapping(kls, orm):
        import sqlalchemy.orm
        import sqlalchemy as _

        tab = _.Table('cluster', orm.meta,
            _.Column('id', _.Integer, primary_key = True),
            _.Column('clustering_id', _.Integer, _.ForeignKey(
                'clustering.id', ondelete = 'CASCADE'), nullable = False),
            _.Column('name', _.String, nullable = False),
            _.UniqueConstraint('clustering_id', 'name'),
        )
        orm.cluster = sqlalchemy.orm.mapper(kls, tab)

        orm.clustering.add_property('clusters', sqlalchemy.orm.relationship(
            orm.cluster, backref = 'clustering',
            passive_deletes = True, lazy = 'dynamic'))
        return

