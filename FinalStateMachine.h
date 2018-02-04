#pragma once

void fsm_evInitialize(void);
void fsm_evTargetFloorSensorActive(void);
void fsm_evInTargetFloor(void);
void fsm_evTimerIsTimeOut(void);
void fsm_evNewTargetFloor(void);
void fsm_evStopButtonPressedInFloor(void);
void fsm_evStopButtonPressedBetweenFloors(void);
void fsm_evStopButtonUnpressed(void);
