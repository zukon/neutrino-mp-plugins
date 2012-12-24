
AM_CPPFLAGS =  \
	@FREETYPE_CFLAGS@ \
	-I$(top_srcdir)/include

noinst_PROGRAMS = input

input_SOURCES = \
	input.c \
	gfx.c \
	io.c \
	inputd.c \
	text.c

input_LDADD = \
	@FREETYPE_LIBS@

install-exec-local:
	install -d $(DESTDIR)/bin
	install -m 0755 input $(DESTDIR)/bin/input
