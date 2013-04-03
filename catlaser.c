// http://stackoverflow.com/questions/5755181/need-help-applying-timer-in-c-in-linux
// http://www.agilart.com/blog/tmp102-raspberry-pi

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <linux/i2c-dev.h>

volatile unsigned int timer = 0;
volatile int fd;			// File descriptors
FILE* patternfd;			//
char *deviceName = "/dev/i2c-0";	// Name of the port we will be using
int address = 0x10;			// Address of Arduino
unsigned char buf[10];			// Buffer for data being read/written on the i2c bus
char *fileName = "pattern.txt";		// Name of the pattern file
unsigned char pattern[255];		// Buffer for the pattern data read from the pattern file
unsigned int x, y, laser;		// X- and Y-Coordinates and state of the laser
char *temp;
const gchar *gbuffer;

void main(int argc, char **argv) {
    struct timeval tv;
    int val;

    if (argc > 1) {
	fileName = argv[1];
    }

    if ((fd = open(deviceName, O_RDWR)) < 0) {	// Open port for reading and writing
	printf("Failed to open i2c port\n");
	exit(1);
    }

    if (ioctl(fd, I2C_SLAVE, address) < 0) {	// Set the port options and set the address of the device we wish to speak to
	printf("Unable to get bus access to talk to slave\n");
	exit(2);
    }

    if ((patternfd = fopen(fileName, "r")) < 0) {
	printf("Failed to open pattern file\n");
	exit(3);
    }

    for (;;) {
	tv.tv_sec = 0;
	tv.tv_usec = 50000;

	do {
	    val = select(0, NULL, NULL, NULL, &tv);
	} while (val != 0 && errno == EINTR);

	if (feof(patternfd) != 0) {
	    printf("EOF\n");
	    exit(0);
	}

	if (fgets(pattern, sizeof(pattern), patternfd) == NULL) {
	    printf("Failed to read from pattern file\n");
	    exit(4);
	}

	temp = strtok(pattern, ":");
	x = atoi(temp);
	temp = strtok(NULL, ":");
	y = atoi(temp);
	temp = strtok(NULL, ":");
	laser = atoi(temp);

	buf[0] = 0;
	buf[1] = x & 0xFF;
	buf[2] = y & 0xFF;
	buf[3] = laser & 0xFF;

	if ((write(fd, buf, 4)) != 4) {
	    printf("Error writing to i2c slave\n");
	    gbuffer = g_strerror(errno);
	    printf(gbuffer);
	    printf("\n");
	    write(fd, buf, 4);
//	    exit(5);
	}

	printf("Timer = %u - X = %u - Y = %u - Laser = %u\n", ++timer, x, y, laser);
    }
}
