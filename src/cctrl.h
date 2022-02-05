#ifndef CCTRL_H
#define CCTRL_H

typedef struct {
	int x; int y; int z;
} vector3d;

int cctrl_init();
void cctrl_toggle_motors();
void cctrl_calibrate_gyro();
void cctrl_move( vector3d* v, int seconds);

#endif

