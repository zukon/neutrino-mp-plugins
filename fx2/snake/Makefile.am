AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = snake

snake_SOURCES = snake.c somain.c

snake_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

snake_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install --strip-program=$(STRIP) -s -m 0755 snake $(DESTDIR)$(PLUGINDIR)/snake.so
	install -m 0644 $(srcdir)/snake.cfg $(DESTDIR)$(PLUGINDIR)

