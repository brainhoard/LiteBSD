#
# OpenSSL/crypto/cms/Makefile
#

DIR=	cms
TOP=	../..
CC=	cc
INCLUDES= -I.. -I$(TOP) -I../../include
CFLAG=-g
MAKEFILE=	Makefile
AR=		ar r

CFLAGS= $(INCLUDES) $(CFLAG)

GENERAL=Makefile
TEST=
APPS=

LIB=$(TOP)/libcrypto.a
LIBSRC= cms_lib.c cms_asn1.c cms_att.c cms_io.c cms_smime.c cms_err.c \
	cms_sd.c cms_dd.c cms_cd.c cms_env.c cms_enc.c cms_ess.c \
	cms_pwri.c cms_kari.c
LIBOBJ= cms_lib.o cms_asn1.o cms_att.o cms_io.o cms_smime.o cms_err.o \
	cms_sd.o cms_dd.o cms_cd.o cms_env.o cms_enc.o cms_ess.o \
	cms_pwri.o cms_kari.o

SRC= $(LIBSRC)

EXHEADER=  cms.h
HEADER=	cms_lcl.h $(EXHEADER)

ALL=    $(GENERAL) $(SRC) $(HEADER)

top:
	(cd ../..; $(MAKE) DIRS=crypto SDIRS=$(DIR) sub_all)

test:

all:	lib

lib:	$(LIBOBJ)
	$(AR) $(LIB) $(LIBOBJ)
	$(RANLIB) $(LIB) || echo Never mind.
	@touch lib

files:
	$(PERL) $(TOP)/util/files.pl Makefile >> $(TOP)/MINFO

links:
	@$(PERL) $(TOP)/util/mklink.pl ../../include/openssl $(EXHEADER)
	@$(PERL) $(TOP)/util/mklink.pl ../../test $(TEST)
	@$(PERL) $(TOP)/util/mklink.pl ../../apps $(APPS)

install:
	@[ -n "$(INSTALLTOP)" ] # should be set by top Makefile...
	@headerlist="$(EXHEADER)"; for i in $$headerlist ; \
	do  \
	(cp $$i $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i; \
	chmod 644 $(INSTALL_PREFIX)$(INSTALLTOP)/include/openssl/$$i ); \
	done;

tags:
	ctags $(SRC)

tests:

lint:
	lint -DLINT $(INCLUDES) $(SRC)>fluff

update: depend

depend:
	@[ -n "$(MAKEDEPEND)" ] # should be set by upper Makefile...
	$(MAKEDEPEND) -- $(CFLAG) $(INCLUDES) $(DEPFLAG) -- $(PROGS) $(LIBSRC)

dclean:
	$(PERL) -pe 'if (/^# DO NOT DELETE THIS LINE/) {print; exit(0);}' $(MAKEFILE) >Makefile.new
	mv -f Makefile.new $(MAKEFILE)

clean:
	rm -f *.o *.obj lib tags core .pure .nfs* *.old *.bak fluff

# DO NOT DELETE THIS LINE -- make depend depends on it.

cms_asn1.o: cms.h cms_asn1.c cms_lcl.h
cms_att.o: cms.h cms_att.c cms_lcl.h
cms_cd.o: ../../e_os.h ../cryptlib.h cms_cd.c cms_lcl.h
cms_dd.o: ../../e_os.h ../cryptlib.h cms_dd.c cms_lcl.h
cms_enc.o: ../../e_os.h ../cryptlib.h cms_enc.c cms_lcl.h
cms_env.o: ../../e_os.h ../asn1/asn1_locl.h ../cryptlib.h cms_env.c cms_lcl.h
cms_err.o: cms_err.c
cms_ess.o: ../../e_os.h ../cryptlib.h cms_ess.c cms_lcl.h
cms_io.o: cms.h cms_io.c cms_lcl.h
cms_kari.o: ../../e_os.h ../asn1/asn1_locl.h ../cryptlib.h cms_kari.c cms_lcl.h
cms_lib.o: cms.h cms_lcl.h cms_lib.c
cms_pwri.o: ../../e_os.h ../asn1/asn1_locl.h ../cryptlib.h cms_lcl.h cms_pwri.c
cms_sd.o: ../../e_os.h ../asn1/asn1_locl.h ../cryptlib.h cms_lcl.h cms_sd.c
cms_smime.o: ../../e_os.h ../asn1/asn1_locl.h ../cryptlib.h cms_lcl.h
cms_smime.o: cms_smime.c
