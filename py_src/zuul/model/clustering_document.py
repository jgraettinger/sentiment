
import formencode
import formencode.validators as fva

from model.base import BaseModel

class ClusteringDocument(BaseModel):

    class Schema(formencode.Schema):
        clustering_id = fva.Int(not_empty = True)
        document_id   = fva.Int(not_empty = True)

    @classmethod
    def define_orm_mapping(kls, orm):
        import sqlalchemy.orm
        import sqlalchemy as _

        tab = _.Table('clustering_document', orm.meta,
            _.Column('clustering_id', _.Integer, _.ForeignKey(
                'clustering.id', ondelete = 'CASCADE')),
            _.Column('document_id', _.Integer, _.ForeignKey(
                'document.id', ondelete = 'CASCADE')),
            _.PrimaryKeyConstraint('clustering_id', 'document_id'),
        )
        orm.clustering_document = sqlalchemy.orm.mapper(kls, tab)

        orm.document.add_property('clusterings',
            sqlalchemy.orm.relationship(orm.clustering, secondary = tab,
                backref = sqlalchemy.orm.backref('documents', lazy = 'dynamic'),
                passive_deletes = True, lazy = 'dynamic'))

        orm.clustering_document.add_property('document',
            sqlalchemy.orm.relationship( orm.document,
                backref = 'clustering_relations', passive_deletes = True))
        orm.clustering_document.add_property('clustering',
            sqlalchemy.orm.relationship( orm.clustering,
                backref = 'document_relations', passive_deletes = True))

        return

