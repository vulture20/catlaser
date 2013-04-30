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
#include <linux/joystick.h>

#define JOY_DEV "/dev/input/js0"

volatile unsigned int timer = 0;
volatile int i2c_fd;
char *deviceName = "/dev/i2c-0";
int address = 0x10;
unsigned char buf[10];
struct timeval tv;
int val;
unsigned short x, y, laser;
const gchar *gbuffer;

int main() {
    int joy_fd, *axis=NULL, num_of_axis=0, num_of_buttons=0, x;
    char *button=NULL, name_of_joystick[80];
    struct js_event js;

    if ((joy_fd = open(JOY_DEV, O_RDONLY)) == -1) {
	printf("Couldn't open joystick\n");
	exit(255);
    }

    ioctl(joy_fd, JSIOCGAXES, &num_of_axis);
    ioctl(joy_fd, JSIOCGBUTTONS, &num_of_buttons);
    ioctl(joy_fd, JSIOCGNAME(80), &name_of_joystick);

    axis = (int *) calloc(num_of_axis, sizeof(int));
    button = (char *) calloc(num_of_buttons, sizeof(char));

    printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n",
	name_of_joystick,
	num_of_axis,
	num_of_buttons);

    fcntl(joy_fd, F_SETFL, O_NONBLOCK);

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

	read(joy_fd, &js, sizeof(struct js_event));

	switch (js.type & ~JS_EVENT_INIT) {
	    case JS_EVENT_AXIS: axis[js.number] = js.value;
				break;
	    case JS_EVENT_BUTTON: button[js.number] = js.value;
				break;
	}

	x = (axis[0]+32767)/364;
	y = (axis[1]+32767)/364;
	laser = button[18];

	buf[0] = 0;
	buf[1] = x;
	buf[2] = y;
	buf[3] = laser;

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
