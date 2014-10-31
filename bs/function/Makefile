#
ROOT            = ../../../../..
ORACLE_HOME	= /u01/app/oracle/product/9.2.0/

THIRD_PARTY = $(ROOT)/third_party/portal
RELEASE = $(ROOT)/release
RELEASE_SRC = $(RELEASE)/shared/src

PROJECT_ROOT    = $(ROOT)/server/tools/cpptools
CPPLIB_ROOT     = $(PROJECT_ROOT)/lib

LIB = $(CPPLIB_ROOT)/libbsfunction.a

LIBSRC =  \
	bs_CollectAccount.cpp \
	bs_LoadUsage.cpp \
	bs_CancelAccount.cpp \
	bs_DepositAccountAuths.cpp \
	bs_ActivateLEC.cpp \
	bs_AdjustCashBalance.cpp \
	bs_AdjustItem.cpp \
	bs_AdjustResourceBalance.cpp \
	bs_ChargebackAccount.cpp \
	bs_CreateBillingAccount.cpp \
	bs_ReceiveCheckPayment.cpp \
	bs_RefundAccount.cpp \
	bs_SimulateACHReturn.cpp \
	bs_SimulateChargeback.cpp \
	bs_SignupClient.cpp \
	bs_UpdateContactInfo.cpp \
	bs_UpdatePayinfo.cpp \
	bs_UpdateStatus.cpp \
	bs_CreateLECRecourse.cpp

INCLUDES = -I. \
	-I$(PROJECT_ROOT) \
	-I$(THIRD_PARTY)/include

include $(RELEASE_SRC)/pin.mk


clean::
	@if [ -d SunWS_cache ]; then rm -r SunWS_cache; fi	


