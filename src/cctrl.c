#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

static const int CCTRL_X_ADJUST = 0; // Summand für buf[2] (Bewegung vorne/hinten)
static const int CCTRL_Y_ADJUST = 0; // Summand für buf[1] (Bewegung rechts/links)

static const int CCTRL_MOVE_X_ADJUST = 10; // Multiplikator für x in cctrl_move()
static const int CCTRL_MOVE_Y_ADJUST = 10; // Multiplikator für y in cctrl_move()
static const int CCTRL_MOVE_Z_ADJUST = 10; // Multiplikator für z in cctrl_move()

static int serial;
static pthread_t thread_id_serial_write_loop;

static char buf[] = {0x66, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x99};

typedef struct {
	int x; int y; int z;
} vector3d;


static void _cctrl_log( char* str ){
	printf("%s", str);
}

static void* _cctrl_serial_write_loop(){
	/*
	   Muss als Thread ausgeführt werden. Schreibt buf über den serial-Port, um die Befehle
	   an die Drone zu übermitteln.
	*/
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

	buf[2] += CCTRL_X_ADJUST;
	buf[1] += CCTRL_Y_ADJUST;
	pthread_create(&thread_id_serial_write_loop, NULL, &_cctrl_serial_write_loop, NULL);

	return 0;
}

void cctrl_calibrate_gyro(){
	/*
	   Setzt für eine Sekunde die Werte von buf sodass das Gyroskop kalibriert wrid.
	*/
	buf[5] = 0x80;
	buf[6] = 0x80;
	sleep(2);
	buf[5] = 0x00;
	buf[6] = 0x00;
}

void cctrl_toggle_motors(){
	/*
	   Setzt für eine Sekunde die Werte von buf sodass die Motoren gestartet werden.
	   Laufen die Motoren schon, werden sie wieder gestoppt. Die Motoren müssen laufen,
	   um die Drone zu starten.
	*/
	buf[5] = 0x01;
	buf[6] = 0x01;
	sleep(1);
	buf[5] = 0x00;
	buf[6] = 0x00;
}

static void* _cctrl_move_vertically(void* parm){
	/*
	   Muss als Thread ausgeführt werden. Ändert die Werte in buf für ein bestimmte
	   Zeit sodass sich die Drone steigt/ sinkt.
	   ::void* param:: Pointer der auf ein int-Array zeigt. Das Array hat zwei
	   		   Parameter.
			   param[0] - Der Wert um den buf erhöht wird.
			   param[1] - Die Sekunden bis der Wert von buf zurückgesetzt
			   wird.
	*/
	pthread_detach(pthread_self());
	int* param = (int*) parm;
	int speed = (abs(param[0]) < 127) ? param[0] : (param[0] > 0) ? 127 : -127;
	int seconds = param[1];

	buf[3] += speed;
	buf[6] += speed;
	sleep(seconds);
	buf[3] -= speed;
	buf[6] -= speed;
	pthread_exit(NULL);
}

static void* _cctrl_move_sideways(void* parm){
	/*
	   Muss als Thread ausgeführt werden. Ändert die Werte in buf für ein bestimmte
	   Zeit sodass sich die Drone sich nach rechts/ links bewegt.
	   ::void* param:: Pointer der auf ein int-Array zeigt. Das Array hat zwei
	   		   Parameter.
			   param[0] - Der Wert um den buf erhöht wird.
			   param[1] - Die Sekunden bis der Wert von buf zurückgesetzt
			   wird.
	*/
	pthread_detach(pthread_self());
	int* param = (int*) parm;
	int speed = (abs(param[0]) < 127) ? param[0] : (param[0] > 0) ? 127 : -127;
	int seconds = param[1];

	buf[1] += speed;
	sleep(seconds);
	buf[1] -= speed;
	pthread_exit(NULL);
}

static void* _cctrl_move_ahead(void* parm){
	/*
	   Muss als Thread ausgeführt werden. Ändert die Werte in buf für ein bestimmte
	   Zeit sodass sich die Drone sich nach vorne/ hinten bewegt.
	   ::void* param:: Pointer der auf ein int-Array zeigt. Das Array hat zwei
	   		   Parameter.
			   param[0] - Der Wert um den buf erhöht wird.
			   param[1] - Die Sekunden bis der Wert von buf zurückgesetzt
			   wird.
	*/
	pthread_detach(pthread_self());
	int* param = (int*) parm;
	int speed = (abs(param[0]) < 127) ? param[0] : (param[0] > 0) ? 127 : -127;
	int seconds = param[1];

	buf[2] += speed;
	sleep(seconds);
	buf[2] -= speed;
	pthread_exit(NULL);
}

void cctrl_move( vector3d* v, int seconds){
	int x_speed = (v -> x / seconds) * CCTRL_MOVE_X_ADJUST;
	int y_speed = (v -> y / seconds) * CCTRL_MOVE_Y_ADJUST;
	int z_speed = (v -> z / seconds) * CCTRL_MOVE_Z_ADJUST;

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

