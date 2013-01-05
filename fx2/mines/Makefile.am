AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = mines

mines_SOURCES = board.c somain.c

mines_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

mines_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install --strip-program=$(STRIP) -s -m 0755 .libs/mines $(DESTDIR)$(PLUGINDIR)/mines.so
	install -m 0644 $(srcdir)/mines.cfg $(DESTDIR)$(PLUGINDIR)

