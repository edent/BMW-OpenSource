TARGET	 = qsqlodbc

HEADERS		= ../../../sql/drivers/odbc/qsql_odbc.h
SOURCES		= main.cpp \
		  ../../../sql/drivers/odbc/qsql_odbc.cpp

unix {
	!contains( LIBS, .*odbc.* ) {
	    LIBS 	*= $$QT_LFLAGS_ODBC
	}
}

win32 {
	!win32-borland:LIBS	*= -lodbc32
    	win32-borland:LIBS	*= $(BCB)/lib/PSDK/odbc32.lib
}

include(../qsqldriverbase.pri)
