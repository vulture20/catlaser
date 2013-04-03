CC       = /usr/bin/gcc
CFLAGS   = `pkg-config --libs --cflags glib-2.0`

all: catlaser

catlaser: catlaser.c
	$(CC) $(CFLAGS) catlaser.c -o catlaser

clean:
	rm -rf *.o catlaser
