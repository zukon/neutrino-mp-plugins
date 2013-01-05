AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = soko

soko_SOURCES = board.c somain.c

soko_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

soko_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install --strip-program=$(STRIP) -s -m 0755 .libs/soko $(DESTDIR)$(PLUGINDIR)/soko.so
	install -m 0644 $(srcdir)/soko.cfg $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(DATADIR)/sokoban
	install -m 0644 $(srcdir)/*.xsb $(DESTDIR)$(DATADIR)/sokoban

