noinst_LTLIBRARIES = libfx2.la

libfx2_la_SOURCES = draw.c pig.c rcinput.c math.c

libfx2_la_LDFLAGS = -rpath $(PLUGINDIR) -module -avoid-version

libfx2_la_LIBADD = \
	@PNG_LIBS@

libfx2_la_CPPFLAGS = -fPIC

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install --strip-program=$(STRIP) -s -m 0644 .libs/libfx2.so $(DESTDIR)$(PLUGINDIR)

