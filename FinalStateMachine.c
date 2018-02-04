#include "FinalStateMachine.h"
#include "elev.h"
#include "Timer.h"
#include "Queue.h"


typedef enum {
	S_None,
	S_Initialization, 
	S_Stop,
	S_Unloading,
	S_Moving,
	S_EmergencyStopBetweenFloors,
	S_EmergencyStopInFloor,
} FSMState;

/// --- CONFIG --- ///

/// --- VARIABLES --- ///
static          FSMState    state               = S_None;


/// --- LOCAL FUNCTIONS --- ///

static void fsm_killButtonLightsInFloor(int floor){
	if(floor == 0){
			elev_set_button_lamp(0, floor, 0);
			elev_set_button_lamp(2, floor, 0);
		} else if(floor == N_FLOORS - 1){
			elev_set_button_lamp(1, floor, 0);
			elev_set_button_lamp(2, floor, 0);
		} else {
			for(int i = 0; i < N_BUTTONS; i++){
				elev_set_button_lamp(i, floor, 0);
			}
		}
}

/// --- FSM FUNCTIONS --- ///
void fsm_evInitialize(void){
	elev_init();
	elev_set_motor_direction(DIRN_DOWN);
	queue_setMostRecentDirection(-1);
	state = S_Initialization;
}

void fsm_evTargetFloorSensorActive(void){
	switch(state){
	case S_Initialization:
	
		elev_set_motor_direction(DIRN_STOP);
		state = S_Stop;
		break;

	case S_Moving:

		elev_set_motor_direction(DIRN_STOP);
		state = S_Stop;
		break;

	default: break;
	}
}

void fsm_evInTargetFloor(void){
	switch(state){
	case S_Stop:

		elev_set_door_open_lamp(1);
		timer_start();
		queue_removeOrder(queue_getMostRecentFloor());
		fsm_killButtonLightsInFloor(queue_getMostRecentFloor());
		state = S_Unloading;
		break;

	default: break;
	}
}

void fsm_evTimerIsTimeOut(void){
	switch(state){
	case S_Unloading:

		timer_stop();
		elev_set_door_open_lamp(0);
		state = S_Stop;
		break;

	default: break;
	}
}

void fsm_evNewTargetFloor(void){
	switch(state){
	case S_Stop:
		if(queue_getMostRecentFloor() < queue_getTargetFloor()){
			elev_set_motor_direction(DIRN_UP);
			queue_setMostRecentDirection(1);
		}else if(queue_getMostRecentFloor() > queue_getTargetFloor()){
			elev_set_motor_direction(DIRN_DOWN);
			queue_setMostRecentDirection(-1);
		}else{
			elev_set_motor_direction((-1)*queue_getMostRecentDirection());
			queue_setMostRecentDirection((-1)*queue_getMostRecentDirection());
		}
		state = S_Moving;
		break;

	default: break;
	}
}

void fsm_evStopButtonPressedInFloor(void){
	elev_set_stop_lamp(1);
	elev_set_motor_direction(DIRN_STOP);
	elev_set_door_open_lamp(1);
	for (int i = 0; i < N_FLOORS; i++)
	{
		queue_removeOrder(i);
		fsm_killButtonLightsInFloor(i);
	}
	queue_resetTargetFloor();
	state = S_EmergencyStopInFloor;
}

void fsm_evStopButtonPressedBetweenFloors(void){
	elev_set_stop_lamp(1);
	elev_set_motor_direction(DIRN_STOP);
	for (int i = 0; i < N_FLOORS; i++)
	{
		queue_removeOrder(i);
		fsm_killButtonLightsInFloor(i);
	}
	queue_resetTargetFloor();
	state = S_EmergencyStopBetweenFloors;
}

void fsm_evStopButtonUnpressed(void){
	elev_set_stop_lamp(0);
	switch(state){
	case S_EmergencyStopInFloor:

		state = S_Stop;
		fsm_evInTargetFloor();
		break;

	case S_EmergencyStopBetweenFloors:
	
		state = S_Stop;
		break;
	
	default: break;
	}
}
