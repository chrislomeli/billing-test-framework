#
ROOT            = ../../../..
ORACLE_HOME     = /u01/app/oracle/product/9.2.0/

RELEASE = $(ROOT)/release
RELEASE_SRC = $(RELEASE)/shared/src

PORTAL_ROOT=$(RELEASE)/portal
PROJECT_ROOT    = $(ROOT)/server/tools/cpptools
CPPLIB_ROOT     = $(PROJECT_ROOT)/lib

LIB = $(CPPLIB_ROOT)/libbsixml.a

DEFINES = -DSUNOS5 -DSUNOS5_6  -DREENTRANT

LIBSRC= \
	tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp \
	xmloutput.cpp tinyxmlinterface.cpp 

INCLUDES = -I. \
        -I$(PROJECT_ROOT) \
        -I$(PORTAL_ROOT)/include

include $(RELEASE_SRC)/pin.mk

clean::
	 @if [ -d SunWS_cache ]; then rm -r SunWS_cache; fi   


