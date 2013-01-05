AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = tetris

tetris_SOURCES = board.c somain.c

tetris_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

tetris_LDADD = \
	-lfx2

if !BOXTYPE_DREAMBOX

tetris_LDFLAGS += @CURL_LIBS@

endif

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install --strip-program=$(STRIP) -s -m 0755 tetris $(DESTDIR)$(PLUGINDIR)/tetris.so
	install -m 0644 $(srcdir)/tetris.cfg $(DESTDIR)$(PLUGINDIR)
	install -m 0644 $(srcdir)/games.cfg $(DESTDIR)$(PLUGINDIR)

