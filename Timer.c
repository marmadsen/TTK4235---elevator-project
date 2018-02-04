#include <sys/time.h>
#include <time.h>

#include "Timer.h"

/// --- DISCALIMER --- ///

/*Dette er ikke originale funksjoner fra våre egne glupe sinn, dette er hentet fra
din (Anders) implementasjon av tripCounter. Men som du selv har sagt:
"Hvorfor gjøre arbeid selv, når bedre, fungerende løsninger finnes"*/


/// --- CONFIG --- ///
/// --- LOCAL VARIABLES --- ///
static  double          timerEndTime;
static  int             timerActive;

/// --- LOCAL FUNCTIONS --- ///
static double get_wall_time(void){
    struct timeval time;
    gettimeofday(&time, NULL);
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

/// --- TIMER FUNCTIONS --- ///
void timer_start(void) {
	timerEndTime    = get_wall_time() + DURATION;
    timerActive     = 1;
}


void timer_stop(void){
    timerActive = 0;
}

int timer_isTimeOut() {
	return timerActive  &&  (get_wall_time() > timerEndTime);
}