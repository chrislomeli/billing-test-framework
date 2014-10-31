#
ROOT            = ../../../..
ORACLE_HOME	= /u01/app/oracle/product/9.2.0/

RELEASE = $(ROOT)/release
RELEASE_SRC = $(RELEASE)/shared/src
THIRD_PARTY = $(ROOT)/third_party

PORTAL_ROOT=$(THIRD_PARTY)/portal
PROJECT_ROOT    = $(ROOT)/server/tools/cpptools
CPPLIB_ROOT     = $(PROJECT_ROOT)/lib

LIB = $(CPPLIB_ROOT)/libcpptype.a

LIBSRC =  \
	c_AccountBalance.cpp \
	c_BillType.cpp \
	c_PayType.cpp \
	c_CacheMap.cpp \
	c_Currency.cpp \
	c_DateTime.cpp \
	c_Decimal.cpp \
	c_EnumMap.cpp \
	c_Name.cpp \
	c_NvRecord.cpp \
	c_Phone.cpp \
	c_Poid.cpp \
	c_Resource.cpp \
	c_RuleSet.cpp \
	c_Status.cpp 

INCLUDES = -I. \
	-I$(PROJECT_ROOT) \
	-I$(PORTAL_ROOT)/include

include $(RELEASE_SRC)/pin.mk


clean::
	@if [ -d SunWS_cache ]; then rm -r SunWS_cache; fi	


