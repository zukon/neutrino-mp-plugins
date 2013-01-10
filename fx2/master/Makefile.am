AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = master

master_SOURCES = board.c somain.c

master_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

master_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install --strip-program=$(STRIP) -s -m 0755 master $(DESTDIR)$(PLUGINDIR)/master.so
	install -m 0644 $(srcdir)/master.cfg $(DESTDIR)$(PLUGINDIR)

