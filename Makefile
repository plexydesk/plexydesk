CC ?= gcc
CXX ?= g++

PREFIX ?= /usr/local
LIBDIR ?= $(PREFIX)/lib
INCLUDEDIR ?= $(PREFIX)/include/plexy

BASE_CFLAGS = -std=c11 -O2 -fPIC -Iinclude -Iinclude/stb
BASE_CXXFLAGS = -std=c++20 -O2 -fPIC -Iinclude -Iinclude/stb -Isrc

CFLAGS += $(BASE_CFLAGS)
CXXFLAGS += $(BASE_CXXFLAGS)

LIBPLEXY_SRC = src/client/plexy.c
CANVAS_SRC = src/canvas/canvas_api.cpp \
             src/canvas/canvas_context.cpp \
             src/canvas/canvas_gpu.cpp \
             src/canvas/shaders.cpp \
             src/canvas/render.cpp \
             src/canvas/stb_image_impl.cpp \
             src/canvas/text.cpp \
             src/canvas/layout.cpp \
             src/canvas/style.cpp \
             src/canvas/theme_light.cpp \
             src/canvas/events.cpp \
             src/canvas/anim.cpp
LIBPLEXYUI_SRC = src/client/plexy_ui.c

CANVAS_PKG_CFLAGS = $(shell pkg-config --cflags freetype2 egl gbm libdrm)
CANVAS_PKG_LIBS = $(shell pkg-config --libs freetype2 egl gbm libdrm)

all: lib/libplexy.so lib/libplexycanvas.so lib/libplexyui.so

lib/libplexy.so: $(LIBPLEXY_SRC) include/plexy.h include/plexy_protocol.h
	@mkdir -p lib
	$(CC) -shared -fPIC $(LIBPLEXY_SRC) $(CFLAGS) -lrt -o $@

lib/libplexycanvas.so: $(CANVAS_SRC) include/plexy_canvas.h
	@mkdir -p lib
	$(CXX) -shared -fPIC $(CANVAS_SRC) $(CXXFLAGS) $(CANVAS_PKG_CFLAGS) $(CANVAS_PKG_LIBS) -lGLEW -lGL -o $@

lib/libplexyui.so: $(LIBPLEXYUI_SRC) include/plexy_ui.h include/plexy_canvas.h include/plexy_protocol.h lib/libplexycanvas.so lib/libplexy.so
	@mkdir -p lib
	$(CC) -shared -fPIC $(LIBPLEXYUI_SRC) $(CFLAGS) -Llib -lplexycanvas -lplexy -lrt -Wl,-rpath,'$$ORIGIN' -o $@

install: all
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(INCLUDEDIR)
	install -m 755 lib/libplexy.so $(DESTDIR)$(LIBDIR)/
	install -m 755 lib/libplexycanvas.so $(DESTDIR)$(LIBDIR)/
	install -m 755 lib/libplexyui.so $(DESTDIR)$(LIBDIR)/
	install -m 644 include/plexy.h $(DESTDIR)$(INCLUDEDIR)/
	install -m 644 include/plexy_protocol.h $(DESTDIR)$(INCLUDEDIR)/
	install -m 644 include/plexy_canvas.h $(DESTDIR)$(INCLUDEDIR)/
	install -m 644 include/plexy_ui.h $(DESTDIR)$(INCLUDEDIR)/
	install -m 644 include/plexy_widget.h $(DESTDIR)$(INCLUDEDIR)/

clean:
	rm -f lib/libplexy.so lib/libplexycanvas.so lib/libplexyui.so

.PHONY: all clean install
