
AM_CPPFLAGS =  \
	-DWWEATHER \
	@FREETYPE_CFLAGS@ \
	@CURL_CFLAGS@ \
	@PNG_CFLAGS@ \
	-I$(top_srcdir)/include

noinst_PROGRAMS = tuxwetter

tuxwetter_SOURCES = \
	tuxwetter.c \
	fb_display.c \
	gfx.c \
	gif.c \
	gifdecomp.c \
	http.c \
	io.c \
	jpeg.c \
	parser.c \
	php.c \
	pngw.c \
	resize.c \
	text.c

tuxwetter_LDADD = \
	@FREETYPE_LIBS@ \
	@CURL_LIBS@ \
	@PNG_LIBS@ \
	-ljpeg

if ENABLE_GIFLIB
tuxwetter_LDADD += -lgif
else
tuxwetter_LDADD += -lungif
endif


install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	install -d $(DESTDIR)$(CONFIGDIR)/tuxwetter
	install -m 0755 tuxwetter $(DESTDIR)$(PLUGINDIR)/tuxwetter.so
	install -m 0644 $(srcdir)/tuxwetter.cfg $(DESTDIR)$(PLUGINDIR)
	install -m 0644 $(srcdir)/tuxwetter.conf $(srcdir)/tuxwetter.mcfg $(srcdir)/convert.list $(srcdir)/startbild.jpg $(DESTDIR)$(CONFIGDIR)/tuxwetter
