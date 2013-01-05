AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = lemmings

lemmings_SOURCES = somain.c sprite.c lemm.c pic.c sound.c double.c

lemmings_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

lemmings_LDADD = \
	-lfx2 -lz -lpthread

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install --strip-program=$(STRIP) -s -m 0755 .libs/lemmings $(DESTDIR)$(PLUGINDIR)/lemmings.so
	install -m 0644 $(srcdir)/lemmings.cfg $(DESTDIR)$(PLUGINDIR)

