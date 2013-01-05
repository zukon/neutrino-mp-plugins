AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = pacman

pacman_SOURCES = maze.c somain.c

pacman_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

pacman_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install --strip-program=$(STRIP) -s -m 0755 .libs/pacman $(DESTDIR)$(PLUGINDIR)/pacman.so
	install -m 0644 $(srcdir)/pacman.cfg $(DESTDIR)$(PLUGINDIR)

