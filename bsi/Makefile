#
ROOT            = ../../../..
ORACLE_HOME	= /u01/app/oracle/product/9.2.0/

RELEASE = $(ROOT)/release
RELEASE_SRC = $(RELEASE)/shared/src
THIRD_PARTY = $(ROOT)/third_party

PORTAL_ROOT =$(THIRD_PARTY)/portal
PROJECT_ROOT = $(ROOT)/server/tools/cpptools
CPPLIB_ROOT = $(PROJECT_ROOT)/lib


LIB = $(CPPLIB_ROOT)/libbsi.a


LIBSRC =  \
	bsi_Client.cpp \
	GPGInterface.cpp \
	bsi_util.cpp \


INCLUDES = -I. \
	-I$(PROJECT_ROOT) \
	-I$(PORTAL_ROOT)/include

include $(RELEASE_SRC)/pin.mk


clean::
	@if [ -d SunWS_cache ]; then rm -r SunWS_cache; fi	


