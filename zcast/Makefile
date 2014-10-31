#
ROOT		= ../../../../../..
ORACLE_HOME	= /usr/local/oracle/oracle-8.1.7
#
SERVER_ROOT	= $(ROOT)/server
BS_ROOT		= $(ROOT)/server/bs
PORTAL_ROOT	= $(ROOT)/server/bs/portal
#
PROJECT_ROOT	= $(BS_ROOT)/cpptools
CPP_ROOT	= $(PROJECT_ROOT)
CPPLIB_ROOT	= $(CPP_ROOT)/lib

LIB = $(CPPLIB_ROOT)/libzcast.a

LIBSRC =  \
	zcastSession.cpp

INCLUDES = -I. \
	-I$(CPP_ROOT)/shared \
	-I$(PORTAL_ROOT)/include_72 \
	-I$(ORACLE_HOME)/rdbms/demo \
	-I$(ORACLE_HOME)/rdbms/public \
	-I$(ORACLE_HOME)/plsql/public \
	-I$(ORACLE_HOME)/network/public

LIBS = $(ORACLE_HOME)/lib/libclntsh.so

include $(ROOT)/shared/pin.mk


clean::
	@if [ -d SunWS_cache ]; then rm -r SunWS_cache; fi      



