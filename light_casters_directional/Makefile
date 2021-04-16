TARGET=main

CC=gcc
PKGS=sdl2 gl
CFLAGS+=$(shell pkg-config --cflags $(PKGS)) $(shell sdl2-config --libs) -DGL_GLEXT_PROTOTYPES -lm
LDLIBS+=$(shell pkg-config --libs $(PKGS)) $(shell sdl2-config --libs) 
SRCDIR=src
BUILDDIR=build

SOURCES=$(shell find $(SRCDIR) -name '*.c')
OBJS=$(patsubst %.c,%.o,$(SOURCES:$(SRCDIR)/%=$(BUILDDIR)/%))

$(TARGET): $(OBJS)
	mkdir -p $(@D)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS) $(LDLIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	rm -f $(TARGET) $(OBJS)
