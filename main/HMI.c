#include "HMI.h"

Act BatteryLevel = {
	.Act.Value = 0, 
	.Limit.Max = 0, 
	.Limit.Min = 0, 
};

int id[3] = {
	596,
	605,
	606
};

int type[3] = {
	REAL,
	REAL,
	REAL
};

void *pointer[3] = {
	(void*)&BatteryLevel.Act.Value,
	(void*)&BatteryLevel.Limit.Max,
	(void*)&BatteryLevel.Limit.Min
};