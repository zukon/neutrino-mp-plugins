AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = sol

sol_SOURCES = solboard.c somain.c

sol_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

sol_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install --strip-program=$(STRIP) -s -m 0755 sol $(DESTDIR)$(PLUGINDIR)/sol.so
	install -m 0644 $(srcdir)/sol.cfg $(DESTDIR)$(PLUGINDIR)

