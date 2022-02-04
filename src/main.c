#include <stdio.h>
#include <unistd.h>  /* UNIX Standard Definitions      */
#include <string.h>

#include "cctrl.h"


static void arg_parser(char* input) {
    if ( strcmp(input, "m_start\n") == 0 )  {
	    cctrl_toggle_motors();
	    printf("# Motor wurde gestartet\n");
    } else if ( strcmp(input, "rise\n") == 0 ) {
	    cctrl_rise(20, 1);
	    printf("# Drone steigt...\n");
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

