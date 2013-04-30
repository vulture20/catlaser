CC       = /usr/bin/gcc
CFLAGS   = `pkg-config --libs --cflags glib-2.0`

all: catlaser joytest joyclient i2cserver

catlaser: catlaser.c
	$(CC) $(CFLAGS) catlaser.c -o catlaser

joytest: joytest.c
	$(CC) $(CFLAGS) joytest.c -o joytest

joyclient: joyclient.c
	$(CC) $(CFLAGS) joyclient.c -o joyclient

i2cserver: i2cserver.c
	$(CC) $(CFLAGS) i2cserver.c -o i2cserver

clean:
	rm -rf *.o catlaser joytest joyclient i2cserver
