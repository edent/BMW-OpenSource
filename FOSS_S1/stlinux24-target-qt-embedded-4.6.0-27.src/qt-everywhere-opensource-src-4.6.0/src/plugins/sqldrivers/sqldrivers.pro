TEMPLATE = subdirs

contains(sql-plugins, psql)	: SUBDIRS += psql
contains(sql-plugins, mysql)	: SUBDIRS += mysql
contains(sql-plugins, odbc)	: SUBDIRS += odbc
contains(sql-plugins, tds)	: SUBDIRS += tds
contains(sql-plugins, oci)	: SUBDIRS += oci
contains(sql-plugins, db2)	: SUBDIRS += db2
contains(sql-plugins, sqlite)	: SUBDIRS += sqlite
contains(sql-plugins, sqlite2)	: SUBDIRS += sqlite2
contains(sql-plugins, ibase)	: SUBDIRS += ibase

contains(S60_VERSION, 3.1)|contains(S60_VERSION, 3.2)|contains(S60_VERSION, 5.0) {
    symbian:contains(CONFIG, system-sqlite): SUBDIRS += sqlite_symbian
    }
