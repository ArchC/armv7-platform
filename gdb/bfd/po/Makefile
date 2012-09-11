# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
# Copyright 2001, 2003 Free Software Foundation, Inc.
#
# This file may be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = bfd
VERSION = 2.16.91

SHELL = /bin/sh


srcdir = ../../../../gdb-6.4/bfd/po
top_srcdir = ../../../../gdb-6.4/bfd
VPATH = ../../../../gdb-6.4/bfd/po

prefix = /Users/rafael/mc404/install
exec_prefix = ${prefix}
datadir = $(prefix)/
localedir = $(datadir)/locale
gnulocaledir = $(prefix)/share/locale
gettextsrcdir = $(prefix)/share/gettext/po
subdir = po

DESTDIR =

INSTALL = /opt/local/bin/ginstall -c
INSTALL_DATA = /opt/local/bin/ginstall -c -m 644
MKINSTALLDIRS = $(srcdir)/../../mkinstalldirs

CC = gcc
GENCAT = @GENCAT@
GMSGFMT = PATH=../src:$$PATH 
MSGFMT = 
XGETTEXT = PATH=../src:$$PATH :
MSGMERGE = PATH=../src:$$PATH msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -g -O2 
CPPFLAGS = 

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = cat-id-tbl.c
POFILES =  fr.po tr.po ja.po es.po sv.po da.po zh_CN.po ro.po rw.po vi.po
GMOFILES =  fr.gmo tr.gmo ja.gmo es.gmo sv.gmo da.gmo zh_CN.gmo ro.gmo rw.gmo vi.gmo
DISTFILES = ChangeLog Makefile.in.in SRC-POTFILES.in BLD-POTFILES.in $(PACKAGE).pot \
stamp-cat-id $(POFILES) $(GMOFILES) $(SOURCES)

# Note - the following line gets processed by bfd/configure and amended
# to contain the full list of source dir POTFILES.
SRC-POTFILES = \
	../../../../gdb-6.4/bfd/aix386-core.c \
	../../../../gdb-6.4/bfd/aix5ppc-core.c \
	../../../../gdb-6.4/bfd/aout0.c \
	../../../../gdb-6.4/bfd/aout32.c \
	../../../../gdb-6.4/bfd/aout64.c \
	../../../../gdb-6.4/bfd/aout-adobe.c \
	../../../../gdb-6.4/bfd/aout-arm.c \
	../../../../gdb-6.4/bfd/aout-cris.c \
	../../../../gdb-6.4/bfd/aoutf1.h \
	../../../../gdb-6.4/bfd/aout-ns32k.c \
	../../../../gdb-6.4/bfd/aout-sparcle.c \
	../../../../gdb-6.4/bfd/aout-target.h \
	../../../../gdb-6.4/bfd/aout-tic30.c \
	../../../../gdb-6.4/bfd/aoutx.h \
	../../../../gdb-6.4/bfd/archive64.c \
	../../../../gdb-6.4/bfd/archive.c \
	../../../../gdb-6.4/bfd/archures.c \
	../../../../gdb-6.4/bfd/armnetbsd.c \
	../../../../gdb-6.4/bfd/bfd.c \
	../../../../gdb-6.4/bfd/bfdio.c \
	../../../../gdb-6.4/bfd/bfdwin.c \
	../../../../gdb-6.4/bfd/binary.c \
	../../../../gdb-6.4/bfd/bout.c \
	../../../../gdb-6.4/bfd/cache.c \
	../../../../gdb-6.4/bfd/cf-i386lynx.c \
	../../../../gdb-6.4/bfd/cf-sparclynx.c \
	../../../../gdb-6.4/bfd/cisco-core.c \
	../../../../gdb-6.4/bfd/coff64-rs6000.c \
	../../../../gdb-6.4/bfd/coff-alpha.c \
	../../../../gdb-6.4/bfd/coff-apollo.c \
	../../../../gdb-6.4/bfd/coff-arm.c \
	../../../../gdb-6.4/bfd/coff-aux.c \
	../../../../gdb-6.4/bfd/coffcode.h \
	../../../../gdb-6.4/bfd/coffgen.c \
	../../../../gdb-6.4/bfd/coff-go32.c \
	../../../../gdb-6.4/bfd/coff-h8300.c \
	../../../../gdb-6.4/bfd/coff-h8500.c \
	../../../../gdb-6.4/bfd/coff-i386.c \
	../../../../gdb-6.4/bfd/coff-i860.c \
	../../../../gdb-6.4/bfd/coff-i960.c \
	../../../../gdb-6.4/bfd/cofflink.c \
	../../../../gdb-6.4/bfd/coff-m68k.c \
	../../../../gdb-6.4/bfd/coff-m88k.c \
	../../../../gdb-6.4/bfd/coff-maxq.c \
	../../../../gdb-6.4/bfd/coff-mips.c \
	../../../../gdb-6.4/bfd/coff-or32.c \
	../../../../gdb-6.4/bfd/coff-rs6000.c \
	../../../../gdb-6.4/bfd/coff-sh.c \
	../../../../gdb-6.4/bfd/coff-sparc.c \
	../../../../gdb-6.4/bfd/coff-stgo32.c \
	../../../../gdb-6.4/bfd/coff-svm68k.c \
	../../../../gdb-6.4/bfd/coffswap.h \
	../../../../gdb-6.4/bfd/coff-tic30.c \
	../../../../gdb-6.4/bfd/coff-tic4x.c \
	../../../../gdb-6.4/bfd/coff-tic54x.c \
	../../../../gdb-6.4/bfd/coff-tic80.c \
	../../../../gdb-6.4/bfd/coff-u68k.c \
	../../../../gdb-6.4/bfd/coff-w65.c \
	../../../../gdb-6.4/bfd/coff-we32k.c \
	../../../../gdb-6.4/bfd/coff-z8k.c \
	../../../../gdb-6.4/bfd/corefile.c \
	../../../../gdb-6.4/bfd/cpu-alpha.c \
	../../../../gdb-6.4/bfd/cpu-arc.c \
	../../../../gdb-6.4/bfd/cpu-arm.c \
	../../../../gdb-6.4/bfd/cpu-avr.c \
	../../../../gdb-6.4/bfd/cpu-bfin.c \
	../../../../gdb-6.4/bfd/cpu-cr16c.c \
	../../../../gdb-6.4/bfd/cpu-cris.c \
	../../../../gdb-6.4/bfd/cpu-crx.c \
	../../../../gdb-6.4/bfd/cpu-d10v.c \
	../../../../gdb-6.4/bfd/cpu-d30v.c \
	../../../../gdb-6.4/bfd/cpu-dlx.c \
	../../../../gdb-6.4/bfd/cpu-fr30.c \
	../../../../gdb-6.4/bfd/cpu-frv.c \
	../../../../gdb-6.4/bfd/cpu-h8300.c \
	../../../../gdb-6.4/bfd/cpu-h8500.c \
	../../../../gdb-6.4/bfd/cpu-hppa.c \
	../../../../gdb-6.4/bfd/cpu-i370.c \
	../../../../gdb-6.4/bfd/cpu-i386.c \
	../../../../gdb-6.4/bfd/cpu-i860.c \
	../../../../gdb-6.4/bfd/cpu-i960.c \
	../../../../gdb-6.4/bfd/cpu-ia64.c \
	../../../../gdb-6.4/bfd/cpu-ip2k.c \
	../../../../gdb-6.4/bfd/cpu-iq2000.c \
	../../../../gdb-6.4/bfd/cpu-m10200.c \
	../../../../gdb-6.4/bfd/cpu-m10300.c \
	../../../../gdb-6.4/bfd/cpu-m32c.c \
	../../../../gdb-6.4/bfd/cpu-m32r.c \
	../../../../gdb-6.4/bfd/cpu-m68hc11.c \
	../../../../gdb-6.4/bfd/cpu-m68hc12.c \
	../../../../gdb-6.4/bfd/cpu-m68k.c \
	../../../../gdb-6.4/bfd/cpu-m88k.c \
	../../../../gdb-6.4/bfd/cpu-maxq.c \
	../../../../gdb-6.4/bfd/cpu-mcore.c \
	../../../../gdb-6.4/bfd/cpu-mips.c \
	../../../../gdb-6.4/bfd/cpu-mmix.c \
	../../../../gdb-6.4/bfd/cpu-ms1.c \
	../../../../gdb-6.4/bfd/cpu-msp430.c \
	../../../../gdb-6.4/bfd/cpu-ns32k.c \
	../../../../gdb-6.4/bfd/cpu-openrisc.c \
	../../../../gdb-6.4/bfd/cpu-or32.c \
	../../../../gdb-6.4/bfd/cpu-pdp11.c \
	../../../../gdb-6.4/bfd/cpu-pj.c \
	../../../../gdb-6.4/bfd/cpu-powerpc.c \
	../../../../gdb-6.4/bfd/cpu-rs6000.c \
	../../../../gdb-6.4/bfd/cpu-s390.c \
	../../../../gdb-6.4/bfd/cpu-sh.c \
	../../../../gdb-6.4/bfd/cpu-sparc.c \
	../../../../gdb-6.4/bfd/cpu-tic30.c \
	../../../../gdb-6.4/bfd/cpu-tic4x.c \
	../../../../gdb-6.4/bfd/cpu-tic54x.c \
	../../../../gdb-6.4/bfd/cpu-tic80.c \
	../../../../gdb-6.4/bfd/cpu-v850.c \
	../../../../gdb-6.4/bfd/cpu-vax.c \
	../../../../gdb-6.4/bfd/cpu-w65.c \
	../../../../gdb-6.4/bfd/cpu-we32k.c \
	../../../../gdb-6.4/bfd/cpu-xstormy16.c \
	../../../../gdb-6.4/bfd/cpu-xtensa.c \
	../../../../gdb-6.4/bfd/cpu-z8k.c \
	../../../../gdb-6.4/bfd/demo64.c \
	../../../../gdb-6.4/bfd/dwarf1.c \
	../../../../gdb-6.4/bfd/dwarf2.c \
	../../../../gdb-6.4/bfd/ecoff.c \
	../../../../gdb-6.4/bfd/ecofflink.c \
	../../../../gdb-6.4/bfd/ecoffswap.h \
	../../../../gdb-6.4/bfd/efi-app-ia32.c \
	../../../../gdb-6.4/bfd/efi-app-ia64.c \
	../../../../gdb-6.4/bfd/elf32-am33lin.c \
	../../../../gdb-6.4/bfd/elf32-arc.c \
	../../../../gdb-6.4/bfd/elf32-arm.c \
	../../../../gdb-6.4/bfd/elf32-avr.c \
	../../../../gdb-6.4/bfd/elf32-bfin.c \
	../../../../gdb-6.4/bfd/elf32.c \
	../../../../gdb-6.4/bfd/elf32-cr16c.c \
	../../../../gdb-6.4/bfd/elf32-cris.c \
	../../../../gdb-6.4/bfd/elf32-crx.c \
	../../../../gdb-6.4/bfd/elf32-d10v.c \
	../../../../gdb-6.4/bfd/elf32-d30v.c \
	../../../../gdb-6.4/bfd/elf32-dlx.c \
	../../../../gdb-6.4/bfd/elf32-fr30.c \
	../../../../gdb-6.4/bfd/elf32-frv.c \
	../../../../gdb-6.4/bfd/elf32-gen.c \
	../../../../gdb-6.4/bfd/elf32-h8300.c \
	../../../../gdb-6.4/bfd/elf32-hppa.c \
	../../../../gdb-6.4/bfd/elf32-hppa.h \
	../../../../gdb-6.4/bfd/elf32-i370.c \
	../../../../gdb-6.4/bfd/elf32-i386.c \
	../../../../gdb-6.4/bfd/elf32-i860.c \
	../../../../gdb-6.4/bfd/elf32-i960.c \
	../../../../gdb-6.4/bfd/elf32-ip2k.c \
	../../../../gdb-6.4/bfd/elf32-iq2000.c \
	../../../../gdb-6.4/bfd/elf32-m32c.c \
	../../../../gdb-6.4/bfd/elf32-m32r.c \
	../../../../gdb-6.4/bfd/elf32-m68hc11.c \
	../../../../gdb-6.4/bfd/elf32-m68hc12.c \
	../../../../gdb-6.4/bfd/elf32-m68hc1x.c \
	../../../../gdb-6.4/bfd/elf32-m68k.c \
	../../../../gdb-6.4/bfd/elf32-m88k.c \
	../../../../gdb-6.4/bfd/elf32-mcore.c \
	../../../../gdb-6.4/bfd/elf32-mips.c \
	../../../../gdb-6.4/bfd/elf32-ms1.c \
	../../../../gdb-6.4/bfd/elf32-msp430.c \
	../../../../gdb-6.4/bfd/elf32-openrisc.c \
	../../../../gdb-6.4/bfd/elf32-or32.c \
	../../../../gdb-6.4/bfd/elf32-pj.c \
	../../../../gdb-6.4/bfd/elf32-ppc.c \
	../../../../gdb-6.4/bfd/elf32-s390.c \
	../../../../gdb-6.4/bfd/elf32-sh64.c \
	../../../../gdb-6.4/bfd/elf32-sh64-com.c \
	../../../../gdb-6.4/bfd/elf32-sh.c \
	../../../../gdb-6.4/bfd/elf32-sh-symbian.c \
	../../../../gdb-6.4/bfd/elf32-sparc.c \
	../../../../gdb-6.4/bfd/elf32-v850.c \
	../../../../gdb-6.4/bfd/elf32-vax.c \
	../../../../gdb-6.4/bfd/elf32-xstormy16.c \
	../../../../gdb-6.4/bfd/elf32-xtensa.c \
	../../../../gdb-6.4/bfd/elf64-alpha.c \
	../../../../gdb-6.4/bfd/elf64.c \
	../../../../gdb-6.4/bfd/elf64-gen.c \
	../../../../gdb-6.4/bfd/elf64-hppa.c \
	../../../../gdb-6.4/bfd/elf64-hppa.h \
	../../../../gdb-6.4/bfd/elf64-mips.c \
	../../../../gdb-6.4/bfd/elf64-mmix.c \
	../../../../gdb-6.4/bfd/elf64-ppc.c \
	../../../../gdb-6.4/bfd/elf64-s390.c \
	../../../../gdb-6.4/bfd/elf64-sh64.c \
	../../../../gdb-6.4/bfd/elf64-sparc.c \
	../../../../gdb-6.4/bfd/elf64-x86-64.c \
	../../../../gdb-6.4/bfd/elf-bfd.h \
	../../../../gdb-6.4/bfd/elf.c \
	../../../../gdb-6.4/bfd/elfcode.h \
	../../../../gdb-6.4/bfd/elfcore.h \
	../../../../gdb-6.4/bfd/elf-eh-frame.c \
	../../../../gdb-6.4/bfd/elf-hppa.h \
	../../../../gdb-6.4/bfd/elflink.c \
	../../../../gdb-6.4/bfd/elf-m10200.c \
	../../../../gdb-6.4/bfd/elf-m10300.c \
	../../../../gdb-6.4/bfd/elfn32-mips.c \
	../../../../gdb-6.4/bfd/elf-strtab.c \
	../../../../gdb-6.4/bfd/elf-vxworks.c \
	../../../../gdb-6.4/bfd/elfxx-mips.c \
	../../../../gdb-6.4/bfd/elfxx-sparc.c \
	../../../../gdb-6.4/bfd/epoc-pe-arm.c \
	../../../../gdb-6.4/bfd/epoc-pei-arm.c \
	../../../../gdb-6.4/bfd/format.c \
	../../../../gdb-6.4/bfd/freebsd.h \
	../../../../gdb-6.4/bfd/genlink.h \
	../../../../gdb-6.4/bfd/go32stub.h \
	../../../../gdb-6.4/bfd/hash.c \
	../../../../gdb-6.4/bfd/hp300bsd.c \
	../../../../gdb-6.4/bfd/hp300hpux.c \
	../../../../gdb-6.4/bfd/hpux-core.c \
	../../../../gdb-6.4/bfd/i386aout.c \
	../../../../gdb-6.4/bfd/i386bsd.c \
	../../../../gdb-6.4/bfd/i386dynix.c \
	../../../../gdb-6.4/bfd/i386freebsd.c \
	../../../../gdb-6.4/bfd/i386linux.c \
	../../../../gdb-6.4/bfd/i386lynx.c \
	../../../../gdb-6.4/bfd/i386mach3.c \
	../../../../gdb-6.4/bfd/i386msdos.c \
	../../../../gdb-6.4/bfd/i386netbsd.c \
	../../../../gdb-6.4/bfd/i386os9k.c \
	../../../../gdb-6.4/bfd/ieee.c \
	../../../../gdb-6.4/bfd/ihex.c \
	../../../../gdb-6.4/bfd/init.c \
	../../../../gdb-6.4/bfd/irix-core.c \
	../../../../gdb-6.4/bfd/libaout.h \
	../../../../gdb-6.4/bfd/libbfd.c \
	../../../../gdb-6.4/bfd/libbfd.h \
	../../../../gdb-6.4/bfd/libcoff.h \
	../../../../gdb-6.4/bfd/libecoff.h \
	../../../../gdb-6.4/bfd/libhppa.h \
	../../../../gdb-6.4/bfd/libieee.h \
	../../../../gdb-6.4/bfd/libnlm.h \
	../../../../gdb-6.4/bfd/liboasys.h \
	../../../../gdb-6.4/bfd/libpei.h \
	../../../../gdb-6.4/bfd/libxcoff.h \
	../../../../gdb-6.4/bfd/linker.c \
	../../../../gdb-6.4/bfd/lynx-core.c \
	../../../../gdb-6.4/bfd/m68k4knetbsd.c \
	../../../../gdb-6.4/bfd/m68klinux.c \
	../../../../gdb-6.4/bfd/m68knetbsd.c \
	../../../../gdb-6.4/bfd/m88kmach3.c \
	../../../../gdb-6.4/bfd/m88kopenbsd.c \
	../../../../gdb-6.4/bfd/mach-o.c \
	../../../../gdb-6.4/bfd/mach-o.h \
	../../../../gdb-6.4/bfd/merge.c \
	../../../../gdb-6.4/bfd/mipsbsd.c \
	../../../../gdb-6.4/bfd/mmo.c \
	../../../../gdb-6.4/bfd/netbsd.h \
	../../../../gdb-6.4/bfd/newsos3.c \
	../../../../gdb-6.4/bfd/nlm32-alpha.c \
	../../../../gdb-6.4/bfd/nlm32.c \
	../../../../gdb-6.4/bfd/nlm32-i386.c \
	../../../../gdb-6.4/bfd/nlm32-ppc.c \
	../../../../gdb-6.4/bfd/nlm32-sparc.c \
	../../../../gdb-6.4/bfd/nlm64.c \
	../../../../gdb-6.4/bfd/nlm.c \
	../../../../gdb-6.4/bfd/nlmcode.h \
	../../../../gdb-6.4/bfd/nlmswap.h \
	../../../../gdb-6.4/bfd/nlm-target.h \
	../../../../gdb-6.4/bfd/ns32k.h \
	../../../../gdb-6.4/bfd/ns32knetbsd.c \
	../../../../gdb-6.4/bfd/oasys.c \
	../../../../gdb-6.4/bfd/opncls.c \
	../../../../gdb-6.4/bfd/osf-core.c \
	../../../../gdb-6.4/bfd/pc532-mach.c \
	../../../../gdb-6.4/bfd/pdp11.c \
	../../../../gdb-6.4/bfd/pe-arm.c \
	../../../../gdb-6.4/bfd/pef.c \
	../../../../gdb-6.4/bfd/pef.h \
	../../../../gdb-6.4/bfd/pef-traceback.h \
	../../../../gdb-6.4/bfd/pe-i386.c \
	../../../../gdb-6.4/bfd/pei-arm.c \
	../../../../gdb-6.4/bfd/peicode.h \
	../../../../gdb-6.4/bfd/pei-i386.c \
	../../../../gdb-6.4/bfd/pei-mcore.c \
	../../../../gdb-6.4/bfd/pei-mips.c \
	../../../../gdb-6.4/bfd/pei-ppc.c \
	../../../../gdb-6.4/bfd/pei-sh.c \
	../../../../gdb-6.4/bfd/pe-mcore.c \
	../../../../gdb-6.4/bfd/pe-mips.c \
	../../../../gdb-6.4/bfd/pe-ppc.c \
	../../../../gdb-6.4/bfd/pe-sh.c \
	../../../../gdb-6.4/bfd/ppcboot.c \
	../../../../gdb-6.4/bfd/reloc16.c \
	../../../../gdb-6.4/bfd/reloc.c \
	../../../../gdb-6.4/bfd/riscix.c \
	../../../../gdb-6.4/bfd/sco5-core.c \
	../../../../gdb-6.4/bfd/section.c \
	../../../../gdb-6.4/bfd/simple.c \
	../../../../gdb-6.4/bfd/som.c \
	../../../../gdb-6.4/bfd/som.h \
	../../../../gdb-6.4/bfd/sparclinux.c \
	../../../../gdb-6.4/bfd/sparclynx.c \
	../../../../gdb-6.4/bfd/sparcnetbsd.c \
	../../../../gdb-6.4/bfd/srec.c \
	../../../../gdb-6.4/bfd/stabs.c \
	../../../../gdb-6.4/bfd/stab-syms.c \
	../../../../gdb-6.4/bfd/sunos.c \
	../../../../gdb-6.4/bfd/syms.c \
	../../../../gdb-6.4/bfd/targets.c \
	../../../../gdb-6.4/bfd/tekhex.c \
	../../../../gdb-6.4/bfd/trad-core.c \
	../../../../gdb-6.4/bfd/vax1knetbsd.c \
	../../../../gdb-6.4/bfd/vaxbsd.c \
	../../../../gdb-6.4/bfd/vaxnetbsd.c \
	../../../../gdb-6.4/bfd/versados.c \
	../../../../gdb-6.4/bfd/version.h \
	../../../../gdb-6.4/bfd/vms.c \
	../../../../gdb-6.4/bfd/vms-gsd.c \
	../../../../gdb-6.4/bfd/vms.h \
	../../../../gdb-6.4/bfd/vms-hdr.c \
	../../../../gdb-6.4/bfd/vms-misc.c \
	../../../../gdb-6.4/bfd/vms-tir.c \
	../../../../gdb-6.4/bfd/xcofflink.c \
	../../../../gdb-6.4/bfd/xcoff-target.h \
	../../../../gdb-6.4/bfd/xsym.c \
	../../../../gdb-6.4/bfd/xsym.h \
	../../../../gdb-6.4/bfd/xtensa-isa.c \
	../../../../gdb-6.4/bfd/xtensa-modules.c 

# Note - the following line gets processed by bfd/configure and amended
# to contain the full list of build dir POTFILES.
BLD-POTFILES = \
	../bfdver.h \
	../elf32-ia64.c \
	../elf32-target.h \
	../elf64-ia64.c \
	../elf64-target.h \
	../peigen.c \
	../pepigen.c \
	../targmatch.h 

CATALOGS = 
CATOBJEXT = 
INSTOBJEXT = 

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-yes

all-yes: $(CATALOGS) # $(PACKAGE).pot
all-no:

$(srcdir)/$(PACKAGE).pot: $(SRC-POTFILES) $(BLD-POTFILES)
	$(XGETTEXT) --default-domain=$(PACKAGE) \
	  --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --files-from=$(srcdir)/SRC-POTFILES.in 
	$(XGETTEXT) --default-domain=$(PACKAGE) \
	  --directory=.. \
	  --directory=. \
	  --add-comments --keyword=_ --keyword=N_ \
	  --join-existing \
	  --files-from=$(srcdir)/BLD-POTFILES.in
	rm -f $(srcdir)/$(PACKAGE).pot
	mv $(PACKAGE).po $(srcdir)/$(PACKAGE).pot

$(srcdir)/cat-id-tbl.c: stamp-cat-id; @:
$(srcdir)/stamp-cat-id: $(PACKAGE).pot
	rm -f cat-id-tbl.tmp
	sed -f ../intl/po2tbl.sed $(srcdir)/$(PACKAGE).pot \
		| sed -e "s/@PACKAGE NAME@/$(PACKAGE)/" > cat-id-tbl.tmp
	if cmp -s cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; then \
	  rm cat-id-tbl.tmp; \
	else \
	  echo cat-id-tbl.c changed; \
	  rm -f $(srcdir)/cat-id-tbl.c; \
	  mv cat-id-tbl.tmp $(srcdir)/cat-id-tbl.c; \
	fi
	cd $(srcdir) && rm -f stamp-cat-id && echo timestamp > stamp-cat-id


install: install-exec install-data
install-exec:
install-info:
install-data: install-data-yes
install-data-no: all
install-data-yes: all
	if test -r $(MKINSTALLDIRS); then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	else \
	  $(top_srcdir)/mkinstalldirs $(DESTDIR)$(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  if test -r $(MKINSTALLDIRS); then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(top_srcdir)/mkinstalldirs $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "gettext"; then \
	  if test -r $(MKINSTALLDIRS); then \
	    $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  else \
	    $(top_srcdir)/mkinstalldirs $(DESTDIR)$(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(PACKAGE)$(INSTOBJEXT).m; \
	done
	rm -f $(DESTDIR)$(gettextsrcdir)/po-Makefile.in.in

check: all

cat-id-tbl.o: ../intl/libgettext.h

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in *.mo *.msg *.cat *.cat.m
	rm -f SRC-POTFILES BLD-POTFILES 

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES) SRC-POTFILES.in BLD-POTFILES.in

distdir = ../$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir: update-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(PACKAGE).pot
	PATH=`pwd`/../src:$$PATH; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  mv $$lang.po $$lang.old.po; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.old.po $(PACKAGE).pot -o $$lang.po; then \
	    rm -f $$lang.old.po; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$lang.po; \
	    mv $$lang.old.po $$lang.po; \
	  fi; \
	done

SRC-POTFILES: SRC-POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' \
	            -e '/^[ 	]*$$/d' \
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

BLD-POTFILES: BLD-POTFILES.in
	  ( rm -f $@-t $@ \
	    && (sed -e '/^#/d' \
	            -e '/^[ 	]*$$/d' \
		    -e "s@.*@	../& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

SRC-POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/SRC-POTFILES.in

BLD-POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/BLD-POTFILES.in

# Note - The presence of SRC-POTFILES and BLD-POTFILES as dependencies
# here breaks the implementation of the 'distclean' rule for maintainers.
# This is because if 'make distclean' is run in the BFD directory, the
# Makefile there will be deleted before 'distclean' is made here, and so
# the dependency SRC-POTFILES -> SRC-POTFILES.in -> ../Makefile cannot
# be satisfied.
#
# The SRC-POTFILES and BLD-POTFILES dependencies cannot be removed,
# however since it is necessary that these files be built during
# *configure* time, so that configure can insert them into the
# po/Makefile that it is creating, so that the Makefile will have
# the correct dependencies.
Makefile: Make-in ../config.status SRC-POTFILES BLD-POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/Makefile.in:$(subdir)/Make-in \
	     CONFIG_HEADERS= $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
