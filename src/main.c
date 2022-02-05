#include <stdio.h>
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <string.h>

#include "cctrl.h"


static void arg_parser(char* input) {
    if ( strcmp(input, "toggle_motor\n") == 0 )  {
	    cctrl_toggle_motors();
	    printf("# Motor wurde gestartet\n");
    } else if ( strcmp(input, "rise\n") == 0 ) {
	    printf("# Drone steigt...\n");
	    vector3d v = {0,0,10};
	    cctrl_move(v, 1);
    } else if ( strcmp(input, "calibrate\n") == 0 ) {
	    printf("# Drone wird calibriert...\n");
	    cctrl_calibrate_gyro();
    }
}


int main(void) {
	if (cctrl_init()){
	  return(-1);
	}

	while(1){
		char input[20] = "";

		printf("$ > ");
		fgets(input, 20, stdin);

		arg_parser(&input[0]);
	}
    }

