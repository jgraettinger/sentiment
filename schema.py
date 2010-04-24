
import sqlite3

class cluster_db(sqlite3.Connection):
    
    @staticmethod
    def connect(database):
        
        conn = sqlite3.connect(database,
            isolation_level = None, factory = cluster_db)

        conn.executescript("""
        create table if not exists document (
            id integer primary key autoincrement,
            content text not null,
            norm_content text
        );

        create table if not exists cluster (
            id integer primary key autoincrement,
            name text not null
        );

        create table if not exists membership (
            doc_id      integer not null,
            cluster_id  integer not null,
            factor      float not null,
            hard        bool default false,

            primary key (doc_id, cluster_id)
        );
        """)

        return conn
        
