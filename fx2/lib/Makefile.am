noinst_LTLIBRARIES = libfx2.la

libfx2_la_SOURCES = draw.c pig.c rcinput.c math.c

libfx2_la_LDFLAGS = -rpath $(PLUGINDIR) -module -avoid-version

install-exec-local:
	install -d $(DESTDIR)/lib
	install --strip-program=$(STRIP) -s -m 0644 .libs/libfx2.so $(DESTDIR)/lib

