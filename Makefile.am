ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

if ENABLE_JPEG
jpeg_DIRS = jpegutils
endif

SUBDIRS = po $(jpeg_DIRS) cut-n-paste src man plugins help data doc

if ENABLE_THUMBNAILER
SUBDIRS += thumbnailer
endif

EXTRA_DIST = 		\
	AUTHORS			\
	autogen.sh		\
	ChangeLog		\
	COPYING			\
	NEWS			\
	README

DISTCLEANFILES =

DISTCHECK_CONFIGURE_FLAGS =      \
	--disable-schemas-install    \
	--enable-gtk-doc    \
	--enable-compile-warnings=no \
	CFLAGS='-Wno-deprecated-declarations'

distuninstallcheck_listfiles = find . -type f -print | grep -v /share/mate/help/ | grep -v \.omf

MAINTAINERCLEANFILES = \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/config.guess \
	$(srcdir)/config.h.in \
	$(srcdir)/config.sub \
	$(srcdir)/depcomp \
	$(srcdir)/gtk-doc.make \
	$(srcdir)/install-sh \
	$(srcdir)/ltmain.sh \
	$(srcdir)/missing \
	$(srcdir)/mkinstalldirs \
	$(srcdir)/omf.make \
	$(srcdir)/xmldocs.make

# This is the last commit before the ChangeLog was converted
CHANGELOG_START = 6a7944721e667a5c2d87e841917d850aea885019

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

-include $(top_srcdir)/git.mk
