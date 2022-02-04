#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <pthread.h>


static int serial;
static pthread_t thread_id_serial_write_loop;

static unsigned char buf[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x99};


static void* _cctrl_serial_write_loop(){
	while(1){
		write(serial, buf, 8);
		usleep(100000);
	}
}

int cctrl_init() {
	serial = open("/dev/serial0", O_RDWR | O_NOCTTY );
	if (serial == -1){
	  printf ("Error: Serialverbindung konnte nicht aufgebaut werden.");
	  return(-1);
	};

	struct termios options;
	tcgetattr(serial, &options);
	cfsetspeed(&options, B19200);
	options.c_cflag &= ~CSTOPB;
	options.c_cflag |= CLOCAL;
	options.c_cflag |= CREAD;
	cfmakeraw(&options);
	tcsetattr(serial, TCSANOW, &options);

	pthread_create(&thread_id_serial_write_loop, NULL, _cctrl_serial_write_loop, NULL);

	return 0;
}

void cctrl_toggle_motors(){
	buf[5] = 0x01;
	buf[6] = 0x01;
	sleep(1);
	buf[5] = 0x00;
	buf[6] = 0x00;
}

void cctrl_rise(int speed, int time){
	buf[3] += speed;
	sleep(time);
	buf[3] -= speed;
}

