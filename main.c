#include <stdio.h>
#include <unistd.h>

#include "elev.h"
#include "Timer.h"
#include "FinalStateMachine.h"
#include "Queue.h"


/// --- STATIC VARIABLES --- ///

//Matrix for all buttons on every floor, containing {prev, curr} status.
static int buttonStatusMatrix[N_FLOORS][N_BUTTONS][2] = 
{
    {{0,0}, {0,0}, {0,0}},
    {{0,0}, {0,0}, {0,0}},
    {{0,0}, {0,0}, {0,0}},
    {{0,0}, {0,0}, {0,0}}
};

static int stopButtonStatus[2] = {0,0};

typedef enum {
    BE_None,
    BE_Pressed,
    BE_Unpressed,
} ButtonEvent;

/// --- LOCAL FUNCTIONS --- ///

static ButtonEvent getButtonEvent(int button, int floor){
    buttonStatusMatrix[floor][button][0] = buttonStatusMatrix[floor][button][1];
    buttonStatusMatrix[floor][button][1] = elev_get_button_signal(button, floor);
    if(buttonStatusMatrix[floor][button][0] != buttonStatusMatrix[floor][button][1]){
        if(buttonStatusMatrix[floor][button][1]){
            return BE_Pressed;
        }else{
            return BE_Unpressed;
        }
    }else{
        return BE_None;
    }
}

static ButtonEvent getStopButtonEvent(void){
    stopButtonStatus[0] = stopButtonStatus[1];
    stopButtonStatus[1] = elev_get_stop_signal();
    if(stopButtonStatus[0] != stopButtonStatus[1]){
        if(stopButtonStatus[1]){
            return BE_Pressed;
        }else{
            return BE_Unpressed;
        }
    }else{
        return BE_None;
    }
}

static void initialize(void){
     while(1){
        if (elev_get_floor_sensor_signal() != -1)
        {
            fsm_evTargetFloorSensorActive();
            break;
        }
    }
}

static void floorDetectionLogic(void){
    switch(elev_get_floor_sensor_signal()){
    case 0:
        elev_set_floor_indicator(0);
        queue_setMostRecentFloor(0);
        break;
    case 1:
        elev_set_floor_indicator(1);
        queue_setMostRecentFloor(1);
        break;
    case 2:
        elev_set_floor_indicator(2);
        queue_setMostRecentFloor(2);
        break;
    case 3:
        elev_set_floor_indicator(3);
        queue_setMostRecentFloor(3);
        break;
    default: break;
    }
}

static void buttonDetectionLogic(void){
    for(int floor = 0; floor < N_FLOORS; floor++){
        for(int button = 0; button < 3; button++){
            if(!((floor == 0 && button == 1) || (floor == 3 && button == 0))){
                switch(getButtonEvent(button, floor)){
                    case BE_Pressed:
                        elev_set_button_lamp(button, floor, 1);
                        queue_addNewOrder(button, floor);
                    default: break;
                }
            }
        }
    }

}

/// --- EventManager --- ///

int main() {

    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }


    fsm_evInitialize();
    initialize();

    while(1){
        floorDetectionLogic();
        buttonDetectionLogic();

        if(elev_get_floor_sensor_signal() != queue_getTargetFloor() && queue_getTargetFloor() != -1){
            fsm_evNewTargetFloor();
        }

        if(elev_get_floor_sensor_signal() == queue_getTargetFloor() && queue_getTargetFloor() != -1){
            queue_setMostRecentFloor(elev_get_floor_sensor_signal());
            fsm_evTargetFloorSensorActive();
            fsm_evInTargetFloor();

        }

        if(timer_isTimeOut()){
            fsm_evTimerIsTimeOut();
        }

        switch(getStopButtonEvent()){
            case BE_Pressed:
                if (elev_get_floor_sensor_signal() != -1)
                {
                    fsm_evStopButtonPressedInFloor();
                }else{
                    fsm_evStopButtonPressedBetweenFloors();
                }

                while(elev_get_stop_signal()) {
                }

            case BE_Unpressed:
                fsm_evStopButtonUnpressed();
            default: break;        
        }

        usleep(10000);
    }

    return 0;
}