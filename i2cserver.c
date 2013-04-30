#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <linux/i2c-dev.h>

volatile unsigned int timer = 0;
volatile int i2c_fd;
char *deviceName = "/dev/i2c-0";
int address = 0x10;
unsigned char buf[10];
struct timeval tv;
int val;
unsigned int x, y, laser;
const gchar *gbuffer;
unsigned char pattern[255];
char *temp;

int main() {
    if ((i2c_fd = open(deviceName, O_RDWR)) < 0) {
	printf("Failed to open i2c port\n");
	exit(254);
    }

    if (ioctl(i2c_fd, I2C_SLAVE, address) < 0) {
	printf("Unable to get bus access to talk to slave\n");
	exit(1);
    }

    for (;;) {
	tv.tv_sec = 0;
	tv.tv_usec = 50000;

	do {
	    val = select(0, NULL, NULL, NULL, &tv);
	} while (val != 0 && errno == EINTR);

	if (feof(stdin) != 0) {
	    printf("EOF\n");
	    exit(0);
	}

	if (fgets(pattern, sizeof(pattern), stdin) == NULL) {
	    printf("Failed to read from stdin\n");
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

	if ((write(i2c_fd, buf, 4)) != 4) {
	    printf("Error writing to i2c slave\n");
	    gbuffer = g_strerror(errno);
	    printf(gbuffer);
	    printf("\n");
	    write(i2c_fd, buf, 4);
	}

	printf("X = %u - Y = %u - Laser = %u\n", x, y, laser);
    }
}
