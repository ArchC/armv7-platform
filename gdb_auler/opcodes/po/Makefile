# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
# Copyright (C) 2003 Free Software Foundation, Inc.
#
# This file may be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.

PACKAGE = opcodes
VERSION = 2.16.91

SHELL = /bin/sh


srcdir = ../../../../gdb-6.4/opcodes/po
top_srcdir = ../../../../gdb-6.4/opcodes
VPATH = ../../../../gdb-6.4/opcodes/po

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
POFILES =  fr.po sv.po tr.po es.po da.po de.po id.po pt_BR.po ro.po nl.po fi.po vi.po ga.po
GMOFILES =  fr.gmo sv.gmo tr.gmo es.gmo da.gmo de.gmo id.gmo pt_BR.gmo ro.gmo nl.gmo fi.gmo vi.gmo ga.gmo
DISTFILES = ChangeLog Makefile.in.in POTFILES.in $(PACKAGE).pot \
stamp-cat-id $(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
	../../../../gdb-6.4/opcodes/alpha-dis.c \
	../../../../gdb-6.4/opcodes/alpha-opc.c \
	../../../../gdb-6.4/opcodes/arc-dis.c \
	../../../../gdb-6.4/opcodes/arc-ext.c \
	../../../../gdb-6.4/opcodes/arc-opc.c \
	../../../../gdb-6.4/opcodes/arm-dis.c \
	../../../../gdb-6.4/opcodes/avr-dis.c \
	../../../../gdb-6.4/opcodes/bfin-dis.c \
	../../../../gdb-6.4/opcodes/cgen-asm.c \
	../../../../gdb-6.4/opcodes/cgen-dis.c \
	../../../../gdb-6.4/opcodes/cgen-opc.c \
	../../../../gdb-6.4/opcodes/cgen-ops.h \
	../../../../gdb-6.4/opcodes/cgen-types.h \
	../../../../gdb-6.4/opcodes/cris-dis.c \
	../../../../gdb-6.4/opcodes/cris-opc.c \
	../../../../gdb-6.4/opcodes/crx-dis.c \
	../../../../gdb-6.4/opcodes/crx-opc.c \
	../../../../gdb-6.4/opcodes/d10v-dis.c \
	../../../../gdb-6.4/opcodes/d10v-opc.c \
	../../../../gdb-6.4/opcodes/d30v-dis.c \
	../../../../gdb-6.4/opcodes/d30v-opc.c \
	../../../../gdb-6.4/opcodes/disassemble.c \
	../../../../gdb-6.4/opcodes/dis-buf.c \
	../../../../gdb-6.4/opcodes/dis-init.c \
	../../../../gdb-6.4/opcodes/dlx-dis.c \
	../../../../gdb-6.4/opcodes/fr30-asm.c \
	../../../../gdb-6.4/opcodes/fr30-desc.c \
	../../../../gdb-6.4/opcodes/fr30-desc.h \
	../../../../gdb-6.4/opcodes/fr30-dis.c \
	../../../../gdb-6.4/opcodes/fr30-ibld.c \
	../../../../gdb-6.4/opcodes/fr30-opc.c \
	../../../../gdb-6.4/opcodes/fr30-opc.h \
	../../../../gdb-6.4/opcodes/frv-asm.c \
	../../../../gdb-6.4/opcodes/frv-desc.c \
	../../../../gdb-6.4/opcodes/frv-desc.h \
	../../../../gdb-6.4/opcodes/frv-dis.c \
	../../../../gdb-6.4/opcodes/frv-ibld.c \
	../../../../gdb-6.4/opcodes/frv-opc.c \
	../../../../gdb-6.4/opcodes/frv-opc.h \
	../../../../gdb-6.4/opcodes/h8300-dis.c \
	../../../../gdb-6.4/opcodes/h8500-dis.c \
	../../../../gdb-6.4/opcodes/h8500-opc.h \
	../../../../gdb-6.4/opcodes/hppa-dis.c \
	../../../../gdb-6.4/opcodes/i370-dis.c \
	../../../../gdb-6.4/opcodes/i370-opc.c \
	../../../../gdb-6.4/opcodes/i386-dis.c \
	../../../../gdb-6.4/opcodes/i860-dis.c \
	../../../../gdb-6.4/opcodes/i960-dis.c \
	../../../../gdb-6.4/opcodes/ia64-asmtab.c \
	../../../../gdb-6.4/opcodes/ia64-asmtab.h \
	../../../../gdb-6.4/opcodes/ia64-dis.c \
	../../../../gdb-6.4/opcodes/ia64-gen.c \
	../../../../gdb-6.4/opcodes/ia64-opc-a.c \
	../../../../gdb-6.4/opcodes/ia64-opc-b.c \
	../../../../gdb-6.4/opcodes/ia64-opc.c \
	../../../../gdb-6.4/opcodes/ia64-opc-d.c \
	../../../../gdb-6.4/opcodes/ia64-opc-f.c \
	../../../../gdb-6.4/opcodes/ia64-opc.h \
	../../../../gdb-6.4/opcodes/ia64-opc-i.c \
	../../../../gdb-6.4/opcodes/ia64-opc-m.c \
	../../../../gdb-6.4/opcodes/ip2k-asm.c \
	../../../../gdb-6.4/opcodes/ip2k-desc.c \
	../../../../gdb-6.4/opcodes/ip2k-desc.h \
	../../../../gdb-6.4/opcodes/ip2k-dis.c \
	../../../../gdb-6.4/opcodes/ip2k-ibld.c \
	../../../../gdb-6.4/opcodes/ip2k-opc.c \
	../../../../gdb-6.4/opcodes/ip2k-opc.h \
	../../../../gdb-6.4/opcodes/iq2000-asm.c \
	../../../../gdb-6.4/opcodes/iq2000-desc.c \
	../../../../gdb-6.4/opcodes/iq2000-desc.h \
	../../../../gdb-6.4/opcodes/iq2000-dis.c \
	../../../../gdb-6.4/opcodes/iq2000-ibld.c \
	../../../../gdb-6.4/opcodes/iq2000-opc.c \
	../../../../gdb-6.4/opcodes/iq2000-opc.h \
	../../../../gdb-6.4/opcodes/m10200-dis.c \
	../../../../gdb-6.4/opcodes/m10200-opc.c \
	../../../../gdb-6.4/opcodes/m10300-dis.c \
	../../../../gdb-6.4/opcodes/m10300-opc.c \
	../../../../gdb-6.4/opcodes/m32c-asm.c \
	../../../../gdb-6.4/opcodes/m32c-desc.c \
	../../../../gdb-6.4/opcodes/m32c-desc.h \
	../../../../gdb-6.4/opcodes/m32c-dis.c \
	../../../../gdb-6.4/opcodes/m32c-ibld.c \
	../../../../gdb-6.4/opcodes/m32c-opc.c \
	../../../../gdb-6.4/opcodes/m32c-opc.h \
	../../../../gdb-6.4/opcodes/m32r-asm.c \
	../../../../gdb-6.4/opcodes/m32r-desc.c \
	../../../../gdb-6.4/opcodes/m32r-desc.h \
	../../../../gdb-6.4/opcodes/m32r-dis.c \
	../../../../gdb-6.4/opcodes/m32r-ibld.c \
	../../../../gdb-6.4/opcodes/m32r-opc.c \
	../../../../gdb-6.4/opcodes/m32r-opc.h \
	../../../../gdb-6.4/opcodes/m32r-opinst.c \
	../../../../gdb-6.4/opcodes/m68hc11-dis.c \
	../../../../gdb-6.4/opcodes/m68hc11-opc.c \
	../../../../gdb-6.4/opcodes/m68k-dis.c \
	../../../../gdb-6.4/opcodes/m68k-opc.c \
	../../../../gdb-6.4/opcodes/m88k-dis.c \
	../../../../gdb-6.4/opcodes/maxq-dis.c \
	../../../../gdb-6.4/opcodes/mcore-dis.c \
	../../../../gdb-6.4/opcodes/mcore-opc.h \
	../../../../gdb-6.4/opcodes/mips16-opc.c \
	../../../../gdb-6.4/opcodes/mips-dis.c \
	../../../../gdb-6.4/opcodes/mips-opc.c \
	../../../../gdb-6.4/opcodes/mmix-dis.c \
	../../../../gdb-6.4/opcodes/mmix-opc.c \
	../../../../gdb-6.4/opcodes/ms1-asm.c \
	../../../../gdb-6.4/opcodes/ms1-desc.c \
	../../../../gdb-6.4/opcodes/ms1-desc.h \
	../../../../gdb-6.4/opcodes/ms1-dis.c \
	../../../../gdb-6.4/opcodes/ms1-ibld.c \
	../../../../gdb-6.4/opcodes/ms1-opc.c \
	../../../../gdb-6.4/opcodes/ms1-opc.h \
	../../../../gdb-6.4/opcodes/ns32k-dis.c \
	../../../../gdb-6.4/opcodes/openrisc-asm.c \
	../../../../gdb-6.4/opcodes/openrisc-desc.c \
	../../../../gdb-6.4/opcodes/openrisc-desc.h \
	../../../../gdb-6.4/opcodes/openrisc-dis.c \
	../../../../gdb-6.4/opcodes/openrisc-ibld.c \
	../../../../gdb-6.4/opcodes/openrisc-opc.c \
	../../../../gdb-6.4/opcodes/openrisc-opc.h \
	../../../../gdb-6.4/opcodes/or32-dis.c \
	../../../../gdb-6.4/opcodes/or32-opc.c \
	../../../../gdb-6.4/opcodes/pdp11-dis.c \
	../../../../gdb-6.4/opcodes/pdp11-opc.c \
	../../../../gdb-6.4/opcodes/pj-dis.c \
	../../../../gdb-6.4/opcodes/pj-opc.c \
	../../../../gdb-6.4/opcodes/ppc-dis.c \
	../../../../gdb-6.4/opcodes/ppc-opc.c \
	../../../../gdb-6.4/opcodes/s390-dis.c \
	../../../../gdb-6.4/opcodes/s390-mkopc.c \
	../../../../gdb-6.4/opcodes/s390-opc.c \
	../../../../gdb-6.4/opcodes/sh64-dis.c \
	../../../../gdb-6.4/opcodes/sh64-opc.c \
	../../../../gdb-6.4/opcodes/sh64-opc.h \
	../../../../gdb-6.4/opcodes/sh-dis.c \
	../../../../gdb-6.4/opcodes/sh-opc.h \
	../../../../gdb-6.4/opcodes/sparc-dis.c \
	../../../../gdb-6.4/opcodes/sparc-opc.c \
	../../../../gdb-6.4/opcodes/sysdep.h \
	../../../../gdb-6.4/opcodes/tic30-dis.c \
	../../../../gdb-6.4/opcodes/tic4x-dis.c \
	../../../../gdb-6.4/opcodes/tic54x-dis.c \
	../../../../gdb-6.4/opcodes/tic54x-opc.c \
	../../../../gdb-6.4/opcodes/tic80-dis.c \
	../../../../gdb-6.4/opcodes/tic80-opc.c \
	../../../../gdb-6.4/opcodes/v850-dis.c \
	../../../../gdb-6.4/opcodes/v850-opc.c \
	../../../../gdb-6.4/opcodes/vax-dis.c \
	../../../../gdb-6.4/opcodes/w65-dis.c \
	../../../../gdb-6.4/opcodes/w65-opc.h \
	../../../../gdb-6.4/opcodes/xstormy16-asm.c \
	../../../../gdb-6.4/opcodes/xstormy16-desc.c \
	../../../../gdb-6.4/opcodes/xstormy16-desc.h \
	../../../../gdb-6.4/opcodes/xstormy16-dis.c \
	../../../../gdb-6.4/opcodes/xstormy16-ibld.c \
	../../../../gdb-6.4/opcodes/xstormy16-opc.c \
	../../../../gdb-6.4/opcodes/xstormy16-opc.h \
	../../../../gdb-6.4/opcodes/xtensa-dis.c \
	../../../../gdb-6.4/opcodes/z8k-dis.c \
	../../../../gdb-6.4/opcodes/z8kgen.c \
	../../../../gdb-6.4/opcodes/z8k-opc.h

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

$(srcdir)/$(PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(PACKAGE) --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --files-from=$(srcdir)/POTFILES.in
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
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

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

POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' -e '/^[ 	]*$$/d' \
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in \
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

POTFILES.in: # ../Makefile
	cd .. && $(MAKE) po/POTFILES.in

Makefile: Make-in ../config.status POTFILES
	cd .. \
	  && CONFIG_FILES=$(subdir)/Makefile.in:$(subdir)/Make-in \
	     CONFIG_HEADERS= $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
