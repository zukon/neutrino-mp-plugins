AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = solitair

solitair_SOURCES = \
	Block.cpp \
	Buffer.cpp \
	Card.cpp \
	Foundation.cpp \
	Hand.cpp \
	Slot.cpp \
	Table.cpp \
	Tableau.cpp \
	Wastepile.cpp \
	somain.cpp \
	pnm_file.cpp \
	pnm_res.cpp \
	backbuffer.cpp

solitair_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

solitair_LDADD = \
	-lfx2 -lz

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install --strip-program=$(STRIP) -s -m 0755 solitair $(DESTDIR)$(PLUGINDIR)/solitair.so
	install -m 0644 $(srcdir)/solitair.cfg $(DESTDIR)$(PLUGINDIR)

