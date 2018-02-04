#pragma once

void queue_setMostRecentFloor(int value);
int queue_getMostRecentFloor(void);

void queue_setMostRecentDirection(int value);
int queue_getMostRecentDirection(void);

void queue_resetTargetFloor(void);
int queue_getTargetFloor(void);

void queue_addNewOrder(int button, int floor);
void queue_removeOrder(int floor);

