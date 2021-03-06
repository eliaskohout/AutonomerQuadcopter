#include <stdio.h>
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <string.h>
#include <stdlib.h>

#include "cctrl.h"

static void split_arguments( char* input, char** out) {
	int anzahl_argumente = 0;
	int input_length = strlen(input);

	for (int i = 0; i<input_length; i++){
		if( input[i] == ' ' ){
			anzahl_argumente++;
			input[i] = '\0';
			out[anzahl_argumente-1] = &input[i+1];
		}
		if(anzahl_argumente == 10) { break; }
	}
}

static void arg_parser(char* input) {
	char* a_arg1 = strtok(input, " ");
	char* a_arg2 = strtok(NULL, " ");
	char* a_arg3 = strtok(NULL, " ");

    	if ( strcmp(a_arg1, "toggle_motor\n") == 0 )  {
	    	cctrl_toggle_motors();
	    	printf("# Motor wurde gestartet\n");
    	} else if ( strcmp(a_arg1, "rise") == 0 ) {
		vector3d v = {0,0, (int)strtol(a_arg2, NULL, 10) };
+               cctrl_move(&v, (int)strtol(a_arg3, NULL, 10) );
+               printf("# Drone steigt/ sinkt...\n");
    	} else if ( strcmp(a_arg1, "calibrate\n") == 0 ) {
	    	printf("# Drone wird kalibriert...\n");
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

