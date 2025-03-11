#ifndef HMI_h
#define HMI_h

#include "time.h"

#define REAL 1
#define INT 3
#define BOOL 4
#define STRING 5
#define TIMESTAMP 6

typedef struct {
	float InputValue;
	float Value;
} _Set;

typedef struct {
	float Value;
} _Act;

typedef struct {
	float Min;
	float Max;
} _Limit;

typedef struct {
	_Set Set;
	_Limit Limit;
} Set;

typedef struct {
	_Act Act;
	_Limit Limit;
} Act;

typedef struct {
	_Set Set;
	_Act Act;
	_Limit Limit;
} SetAct;

typedef struct {
	int Command;
	int Status;
} LogicSelection;

typedef struct {
	int Status;
} LogicStatus;

typedef struct {
	int Status;
	int Reaction;
	time_t Ts;
} Alarm;

typedef struct {
	Act BatteryLevel;
	LogicSelection Light;
	Set Temperature;
} _HMI;
extern _HMI HMI;

extern int id[9];
extern int type[9];
extern void *pointer[9];

#endif