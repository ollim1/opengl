TARGET=main

CC=gcc
PKGS=sdl2 gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) $(shell sdl2-config --libs) -DGL_GLEXT_PROTOTYPES -lm
LDLIBS+=$(shell pkg-config --libs $(PKGS)) $(shell sdl2-config --libs) 

SOURCES=main.c shader_t.c camera_t.c mouse_t.c fileload.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
