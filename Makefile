NAME=dulradio
CFLAGS=-O3 -Wall
DARWIN_CFLAGS=-DDARWIN
WIN_CFLAGS=-DBUILDING_DLL=1
DARWIN_ARCH=-arch i386 -arch ppc
INCLUDES=-I/usr/include/pdextended
DARWIN_INCLUDES=-I/Applications/Pd-extended.app/Contents/Resources/include
WIN_PD_PATH=/c/Program\ Files\ \(x86\)/pd/
WIN_INCLUDES=-I$(WIN_PD_PATH)include
WIN_LIBS=-L$(WIN_PD_PATH)bin/pd.lib
WIN_DLLS=$(WIN_PD_PATH)bin/pd.dll

ARCH=$(shell uname -m)
ifeq ($(ARCH), x86_64)
  CFLAGS+= -fPIC
endif

all: $(shell uname -s)

Linux: $(NAME).pd_linux
.SUFFIXES: .pd_linux
.c.pd_linux:
	gcc $(CFLAGS) $(INCLUDES) -o bin/$*.o -c $*.c
	ld -E -shared -o bin/$*.pd_linux bin/$*.o
	strip --strip-unneeded bin/$*.pd_linux

Darwin: $(NAME).pd_darwin
.SUFFIXES: .pd_darwin
.c.pd_darwin:
	cc $(CFLAGS) $(DARWIN_CFLAGS) $(DARWIN_ARCH) $(DARWIN_INCLUDES) -o bin/$*.o -c $*.c
	cc -bundle -undefined suppress -flat_namespace $(DARWIN_ARCH) -o bin/$*.pd_darwin bin/$*.o

Cygwin: $(NAME).dll
.SUFFIXES: .dll
.c.dll:
	gcc $(CFLAGS) $(WIN_CFLAGS) -o bin/$*.o -c $*.c
	ld -E -shared $(WIN_LIBS) -o $*.dll bin/$*.o $(WIN_DLLS)

clean:
	rm bin/*
