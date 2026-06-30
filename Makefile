CC ?= gcc

PREFIX    ?= /usr/local
LIBDIR    ?= $(PREFIX)/lib
INCDIR    ?= $(PREFIX)/include/plexy

BASE_CFLAGS = -std=c11 -O2 -fPIC -Iinclude
CFLAGS += $(BASE_CFLAGS)

SRC = src/client/plexy.c

all: lib/libplexy.so

lib/libplexy.so: $(SRC) include/plexy.h include/plexy_protocol.h
	@mkdir -p lib
	$(CC) -shared -fPIC $(SRC) $(CFLAGS) -lrt -o $@

install: all
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(INCDIR)
	install -m 755 lib/libplexy.so      $(DESTDIR)$(LIBDIR)/
	install -m 644 include/plexy.h      $(DESTDIR)$(INCDIR)/
	install -m 644 include/plexy_protocol.h $(DESTDIR)$(INCDIR)/

clean:
	rm -f lib/libplexy.so

.PHONY: all install clean
