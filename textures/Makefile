TARGET=main

CC=gcc
PKGS=sdl2 gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) $(shell sdl2-config --libs) -DGL_GLEXT_PROTOTYPES -lm
LDLIBS+=$(shell pkg-config --libs $(PKGS)) $(shell sdl2-config --libs) 

SOURCES=main.c
OBJS=$(patsubst %.c,%.o,$(SOURCES))

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LDLIBS)

%.o: %.c stb_image.h
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
