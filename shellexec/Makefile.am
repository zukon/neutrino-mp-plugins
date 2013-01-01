
AM_CPPFLAGS =  \
	@FREETYPE_CFLAGS@ \
	-I$(top_srcdir)/include

noinst_PROGRAMS = shellexec

shellexec_SOURCES = \
	shellexec.c \
	text.c \
	gfx.c \
	io.c

shellexec_LDADD = \
	@FREETYPE_LIBS@

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install -d $(DESTDIR)/bin
	install -m 0755 shellexec $(DESTDIR)$(PLUGINDIR)/shellexec.so
	install -m 0644 $(srcdir)/shellexec.conf $(DESTDIR)$(CONFIGDIR)
	install -m 0644 $(srcdir)/shellexec.cfg $(DESTDIR)$(PLUGINDIR)
	ln -s $(PLUGINDIR)/shellexec.so $(DESTDIR)/bin/shellexec
