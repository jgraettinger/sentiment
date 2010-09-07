from clustering import Clustering
from cluster import Cluster
from document import Document
from clustering_document import ClusteringDocument
from cluster_document import ClusterDocument

import sqlalchemy as sa
import sqlalchemy.orm
import getty

class ORM(object):

    @getty.requires(
        db_uri = getty.Config('db_uri'),
        echo_sql = getty.Config('echo_sql'),
    )
    def __init__(self, db_uri, echo_sql):
        self.engine = sa.create_engine(db_uri, echo = echo_sql)

        # Enable enforcement of SQLite Foreign Keys
        self.engine.execute('PRAGMA foreign_keys = ON;')

        self.meta = sa.MetaData()

        Clustering.define_orm_mapping(self)
        Cluster.define_orm_mapping(self)
        Document.define_orm_mapping(self)
        ClusteringDocument.define_orm_mapping(self)
        ClusterDocument.define_orm_mapping(self)

        # build tables
        self.meta.create_all(self.engine)
        # obtain session
        self.session_factory = sa.orm.sessionmaker(bind = self.engine)
        return

    @property
    def session(self):
        return self.session_factory()

