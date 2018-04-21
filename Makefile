ECHO	= /bin/echo -e
SHELL	= /bin/sh
RM	= /bin/rm -f
CC	= gcc
STRIP	= strip

GIT_VERSION := $(shell git --no-pager describe --tags --dirty |sed 's/\([^-]*-g\)/r\1/;s/-/./g')

CFLAGS	= -g -Os -std=c11 -I./include -Wall -Wstrict-prototypes -Wconversion
CFLAGS += -DVERSION=\"$(GIT_VERSION)\"
CFLAGS	+= -Wmissing-prototypes -Wshadow -Wextra -Wunused
LDFLAGS	= -lusb-1.0

PROGS = lan7800-led-ctl

all:	$(PROGS)

clean:
	$(RM) src/*.o
tidy:
	$(RM) src/*.o $(PROGS)

install: $(PROGS)
	@$(ECHO) "\t==> Installing programs to $(DESTDIR)/usr/bin"
	@install -m 0755 -d $(DESTDIR)/usr/bin
	@install -m 0755 -t $(DESTDIR)/usr/bin $(PROGS)

pack:
	@$(ECHO) "Cleaning up ..." ; \
	$(RM) src/*.o $(PROGS)
	@$(ECHO) "Creating package ..." ; \
	cd .. ; \
	tar c -J -f lan7800-led-ctl-$(GIT_VERSION).tar.xz lan7800-led-ctl

# Generic instructions
src/%.o: src/%.c
	@$(ECHO) "\t--> Compiling `basename $<`"
	@$(CC) $(CFLAGS) -c $< -o $@

# Specific programs
lan7800-led-ctl:	src/lan7800-led-ctl.o
	@$(ECHO) "\t==> Linking objects to output file $@ $(GIT_VERSION)\n"
	@$(CC) $(CFLAGS) $(LDFLAGS) $+ -o $@
	@$(STRIP) $@
