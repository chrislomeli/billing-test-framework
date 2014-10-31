#
ROOT		= ../../..
ORACLE_HOME	= /u01/app/oracle/product/9.2.0/
#
RELEASE = $(ROOT)/release
RELEASE_SRC = $(RELEASE)/shared/src
PORTAL_ROOT=$(RELEASE)/portal
#

#order is important here
SUBDIRS=  \
	xml \
	http \
	bsi \
	bs/obj \
	bs/function \
	ctype \
	parser \
	logCpp \
	logSimple \
	fsys \
	oci++ \
	pin \
	tcp \
	crypt 


include $(RELEASE_SRC)/pin.mk

tags::
	$(DOSUBDIRS)

clean::
	@if [ -d lib/SunWS_cache ]; then rm -r lib/SunWS_cache; fi      


