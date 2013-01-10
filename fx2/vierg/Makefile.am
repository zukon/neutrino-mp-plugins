AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = vierg

vierg_SOURCES = board.c somain.c

vierg_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

vierg_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install --strip-program=$(STRIP) -s -m 0755 vierg $(DESTDIR)$(PLUGINDIR)/vierg.so
	install -m 0644 $(srcdir)/vierg.cfg $(DESTDIR)$(PLUGINDIR)

