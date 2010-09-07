
import formencode
import formencode.validators as fva

from base import BaseModel

class ClusterDocument(BaseModel):

    class Schema(formencode.Schema):
        clustering_id = fva.Int(not_empty = True)
        document_id   = fva.Int(not_empty = True)
        cluster_id    = fva.Int(not_empty = True)
        probability   = fva.Number(min = 0, max = 1.0, default = 0)
        explicit      = fva.StringBool(default = False)

    @classmethod
    def define_orm_mapping(kls, orm):
        import sqlalchemy.orm
        import sqlalchemy as _

        tab = _.Table('cluster_document', orm.meta,
            _.Column('clustering_id', _.Integer, nullable = False),
            _.Column('document_id', _.Integer, nullable = False),
            _.Column('cluster_id', _.Integer, _.ForeignKey(
                'cluster.id', ondelete = 'CASCADE')),
            _.Column('probability', _.Float, nullable = False),
            _.Column('explicit', _.Boolean, nullable = False),
            _.PrimaryKeyConstraint('clustering_id', 'document_id', 'cluster_id'),
            _.ForeignKeyConstraint(
                ['clustering_id', 'document_id'],
                ['clustering_document.clustering_id',
                 'clustering_document.document_id'],
                ondelete = 'CASCADE'),
        )
        orm.cluster_document = sqlalchemy.orm.mapper(kls, tab)

        orm.cluster_document.add_property('clustering_document',
            sqlalchemy.orm.relationship( orm.clustering_document,
                backref = 'cluster_documents', passive_deletes = True))
        orm.cluster_document.add_property('cluster',
            sqlalchemy.orm.relationship( orm.cluster,
                backref = 'documents', passive_deletes = True))
        return

