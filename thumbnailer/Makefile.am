NULL =

if USE_INTERNAL_THUMBNAILER

bin_PROGRAMS = eom-thumbnailer
eom_thumbnailer_SOURCES = \
	eom-thumbnailer.c	\
	$(NULL)
eom_thumbnailer_CPPFLAGS = \
	-I$(top_srcdir)		\
	-I$(top_builddir)	\
	$(AM_CPPFLAGS)		\
	$(NULL)
eom_thumbnailer_CFLAGS = \
	$(THUMBNAILER_CFLAGS)	\
	$(DISABLE_DEPRECATED)	\
	$(WARN_CFLAGS)		\
	$(AM_CFLAGS)		\
	$(NULL)
eom_thumbnailer_LDFLAGS = \
	$(AM_LDFLAGS)		\
	-lm			\
	$(THUMBNAILER_LIBS)	\
	$(NULL)

endif

thumbnailerdir = $(datadir)/thumbnailers
thumbnailer_DATA = eom-thumbnailer.thumbnailer

EXTRA_DIST = \
	eom-thumbnailer.thumbnailer.in \
	$(NULL)

CLEANFILES = \
	$(thumbnailer_DATA)	\
	$(NULL)

-include $(top_srcdir)/git.mk
