AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = tank

tank_SOURCES = board.c somain.c

tank_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

tank_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install --strip-program=$(STRIP) -s -m 0755 .libs/tank $(DESTDIR)$(PLUGINDIR)/tank.so
	install -m 0644 $(srcdir)/tank.cfg $(DESTDIR)$(PLUGINDIR)

