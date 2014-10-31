#
ROOT            = ../../../..
ORACLE_HOME	= /u01/app/oracle/product/9.2.0/

RELEASE = $(ROOT)/release
RELEASE_SRC = $(RELEASE)/shared/src
THIRD_PARTY = $(ROOT)/third_party

PORTAL_ROOT=$(THIRD_PARTY)/portal
PROJECT_ROOT    = $(ROOT)/server/tools/cpptools
CPPLIB_ROOT     = $(PROJECT_ROOT)/lib

LIB = $(CPPLIB_ROOT)/libcppfsys.a


LIBSRC =  \
	f_CommandFileReader.cpp \
	f_Dir.cpp  \
	f_File.cpp \
	f_FileName.cpp \
	f_FileReader.cpp \
	f_FileRecordReader.cpp \
	f_PropertiesReader.cpp \
	f_Spooler.cpp



INCLUDES = -I. \
	-I$(PROJECT_ROOT) \
	-I$(PORTAL_ROOT)/include

include $(RELEASE_SRC)/pin.mk


clean::
	@if [ -d SunWS_cache ]; then rm -r SunWS_cache; fi	


