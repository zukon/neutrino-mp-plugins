AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = yahtzee

yahtzee_SOURCES = yahtzee.c somain.c

yahtzee_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

yahtzee_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install --strip-program=$(STRIP) -s -m 0755 yahtzee $(DESTDIR)$(PLUGINDIR)/yahtzee.so
	install -m 0644 $(srcdir)/yahtzee.cfg $(DESTDIR)$(PLUGINDIR)

