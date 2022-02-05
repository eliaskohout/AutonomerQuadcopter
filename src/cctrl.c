#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>


static int serial;
static pthread_t thread_id_serial_write_loop;

static char buf[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x99};
// buf[1] ändern als Ausgleich fürs abdriften nach rechts/links
// buf[2] ändern als Ausgleich fürs abdriften nach vorne/hinten

typedef struct {
	int x; int y; int z;
} vector3d;


static void _cctrl_log( char* str ){
	printf("%s", str);
}

static void* _cctrl_serial_write_loop(){
	while(1){
		write(serial, buf, 8);
		usleep(100000);
	}
}

int cctrl_init() {
	serial = open("/dev/serial0", O_RDWR | O_NOCTTY );
	if (serial == -1){
	  _cctrl_log("Error (cctrl_init): Serialverbindung konnte nicht aufgebaut werden.");
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

	pthread_create(&thread_id_serial_write_loop, NULL, &_cctrl_serial_write_loop, NULL);

	return 0;
}

void cctrl_calibrate_gyro(){
	buf[5] = 0x80;
	buf[6] = 0x00;
	sleep(4);
	buf[5] = 0x00;
}

void cctrl_toggle_motors(){
	buf[5] = 0x01;
	buf[6] = 0x01;
	sleep(1);
	buf[5] = 0x00;
	buf[6] = 0x00;
}

static void* _cctrl_move_vertically(void* parm){
    pthread_detach(pthread_self());
	int* param = (int*) parm;
	int speed = param[0];
	int seconds = param[1];

	buf[3] += speed;
	buf[6] += speed;
	sleep(seconds);
	buf[3] -= speed;
	buf[6] -= speed;
    pthread_exit(NULL);
}

static void* _cctrl_move_sideways(void* parm){
    pthread_detach(pthread_self());
	int* param = (int*) parm;
	int speed = param[0];
	int seconds = param[1];

	buf[1] += speed;
	sleep(seconds);
	buf[1] -= speed;
    pthread_exit(NULL);
}

static void* _cctrl_move_ahead(void* parm){
    pthread_detach(pthread_self());
	int* param = (int*) parm;
	int speed = param[0];
	int seconds = param[1];

	buf[2] += speed;
	sleep(seconds);
	buf[2] -= speed;
    pthread_exit(NULL);
}

void cctrl_move( vector3d* v, int seconds){
	int x_speed = (v -> x / seconds) * 1;
	int y_speed = (v -> y / seconds) * 1;
	int z_speed = (v -> z / seconds) * 1;

	static int param[2];  // static damit es von den Threads (z insbesondere) gelesen werden kann
	pthread_t thread_id;

    param[1] = seconds;
    param[0] = x_speed;
	pthread_create(&thread_id, NULL, &_cctrl_move_ahead, (void *) param); // x

	param[0] = y_speed;
	pthread_create(&thread_id, NULL, &_cctrl_move_sideways, (void *) param); // y

	param[0] = z_speed;
	pthread_create(&thread_id, NULL, &_cctrl_move_vertically, (void *) param); // z
}

