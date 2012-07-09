# make the dulradio Pd external
#
CFLAGS= -fPIC -O3 -Wall -c
LDFLAGS= -lc -lm -export-dynamic -shared
INCLUDES= -I/usr/include/pdextended

all:
	gcc $(CFLAGS) $(INCLUDES) -o dulradio.o dulradio.c
	gcc $(LDFLAGS) -o bin/dulradio.pd_linux dulradio.o
