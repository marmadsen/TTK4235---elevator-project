#include "Queue.h"
#include "elev.h"

/// --- CONFIG --- ///
/// --- LOCAL VARIABLES --- ///
static int queue_matrix[N_FLOORS][N_BUTTONS] =
	{ 	{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0}
	};

static int mostRecentFloor;
static int mostRecentDirection;
static int targetFloor = -1;


/// --- LOCAL FUNCTIONS --- ///
static int selectNextTargetFloor(void){
	int iter;
	switch(mostRecentDirection){
	case 1:

		for(iter = mostRecentFloor + 1; iter < N_FLOORS; iter++){
			if(queue_matrix[iter][0] || queue_matrix[iter][2]){
				return iter;
			}
		}
		for(iter = N_FLOORS - 1; iter >= 0; iter--){
			if(queue_matrix[iter][1] || queue_matrix[iter][2]){
				return iter;
			}
		}
		for(iter = 0; iter <= mostRecentFloor; iter++){
			if(queue_matrix[iter][0]){
				return iter;
			}
		}
		return -1;

	case -1:

		for(iter = mostRecentFloor - 1; iter >= 0; iter--){
			if(queue_matrix[iter][1] || queue_matrix[iter][2]){
				return iter;
			}
		}
		for(iter = 0; iter < N_FLOORS; iter++){
			if(queue_matrix[iter][0] || queue_matrix[iter][2]){
				return iter;
			}
		}
		for(iter = N_FLOORS - 1; iter >= mostRecentFloor; iter--){
			if(queue_matrix[iter][1]){
				return iter;
			}
		}
		return -1;

	default: return -1;
	}
}

/// --- QUEUE FUNCTIONS --- ///
void queue_setMostRecentFloor(int value){
	mostRecentFloor = value;
}
int queue_getMostRecentFloor(void){
	return mostRecentFloor;
}

void queue_setMostRecentDirection(int value){
	mostRecentDirection = value;
}

int queue_getMostRecentDirection(void){
	return mostRecentDirection;
}

void queue_resetTargetFloor(void){
	targetFloor = -1;
}

int queue_getTargetFloor(void){
	return targetFloor;
}

void queue_addNewOrder(int button, int floor){
	queue_matrix[floor][button] = 1;
	targetFloor = selectNextTargetFloor();	
}

void queue_removeOrder(int floor){
	for(int button = 0; button < N_BUTTONS; button++){
		queue_matrix[floor][button] = 0;
	}
	targetFloor = selectNextTargetFloor();
}

