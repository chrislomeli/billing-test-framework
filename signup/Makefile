#
ROOT            = ../../../../../..
#ORACLE_HOME     = /usr/local/oracle/oracle-8.1.7
ORACLE_HOME	= /u01/app/oracle/product/9.2.0/
#
SERVER_ROOT	= $(ROOT)/server
BS_ROOT		= $(ROOT)/server/bs
PORTAL_ROOT     = $(ROOT)/server/bs/portal
#
PROJECT_ROOT    = $(BS_ROOT)/cpptools
CPP_ROOT	= $(PROJECT_ROOT)
CPPLIB_ROOT     = $(CPP_ROOT)/lib

LIB = $(CPPLIB_ROOT)/libpostp.a


LIBSRC =  \
	bs_PostpSession.cpp


INCLUDES = -I.. \
	-I$(ROOT)/server -I$(ROOT)/server/shared -I$(ROOT)/shared \
	-I$(ROOT)/server/db/include -I$(ROOT)/shared/include \
	-I$(ROOT)/shared/cstr \
	-I$(ROOT)/server/ember/signup \
	-I$(ROOT)/server/ember/jrpc \
	-I$(ROOT)/server/ember/jrpc/lib_simple \
	-I$(ROOT)/server/bs/cpptools/shared \
	-I$(PROJECT_ROOT)/shared \
	-I$(PORTAL_ROOT)/include_72

include $(ROOT)/shared/pin.mk


clean::
	@if [ -d SunWS_cache ]; then rm -r SunWS_cache; fi	


