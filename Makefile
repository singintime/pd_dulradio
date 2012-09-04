NAME=dulradio
CFLAGS=-std=c99 -DPD -O3 -Wall -W -Wshadow -Wstrict-prototypes -Wno-unused -Wno-parentheses -Wno-switch
INCLUDES=-I/usr/include/pdextended

UNIVERSAL=-arch i386 -arch ppc
DARWINCFLAGS = $(CFLAGS) -DDARWIN $(UNIVERSAL) -pedantic
DARWININCLUDES=-I/Applications/Pd-extended.app/Contents/Resources/include
DARWIN_LIBS=$(UNIVERSAL)

OS=$(shell uname -s)
ARCH=$(shell uname -m)

ifeq ($(ARCH), x86_64)
  CFLAGS+= -fPIC
endif

linux: $(NAME).pd_linux
.SUFFIXES: .pd_linux
.c.pd_linux:
	gcc $(CFLAGS) $(INCLUDES) -o $*.o -c $*.c
	ld -export_dynamics -shared -o $*.pd_linux $*.o
	strip --strip-unneeded $*.pd_linux

darwin: $(NAME).pd_darwin
.SUFFIXES: .pd_darwin
.c.pd_darwin:
	cc  $(DARWINCFLAGS) $(DARWININCLUDES) -o $*.o -c $*.c
	cc -bundle -undefined suppress -flat_namespace $(DARWIN_LIBS) -o $*.pd_darwin $*.o

clean:
	rm *.o
	rm $(NAME).pd*
