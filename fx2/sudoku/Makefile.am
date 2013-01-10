AM_CPPFLAGS = -I$(srcdir)/../lib -I$(top_srcdir)/include

noinst_PROGRAMS = sudoku

sudoku_SOURCES = misc.cpp menu.cpp sudoku.cpp board.cpp somain.cpp

sudoku_LDFLAGS = \
	-rpath $(PLUGINDIR) -R$(TARGET_PLUGINDIR)

sudoku_LDADD = \
	-lfx2

install-exec-local:
	install -d $(DESTDIR)$(PLUGINDIR)
	install -d $(DESTDIR)$(CONFIGDIR)
	$(LIBTOOL) --mode=install install --strip-program=$(STRIP) -s -m 0755 sudoku $(DESTDIR)$(PLUGINDIR)/sudoku.so
	install -m 0644 $(srcdir)/sudoku.cfg $(DESTDIR)$(PLUGINDIR)

