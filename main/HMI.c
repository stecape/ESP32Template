#include "HMI.h"

_HMI HMI = {
	.BatteryLevel = {
		.Act.Value = 0,
		.Limit.Max = 0,
		.Limit.Min = 0,
	},
};

int id[3] = {
	3,
	5,
	6
};

int type[3] = {
	REAL,
	REAL,
	REAL
};

void *pointer[3] = {
	(void*)&HMI.BatteryLevel.Act.Value,
	(void*)&HMI.BatteryLevel.Limit.Min,
	(void*)&HMI.BatteryLevel.Limit.Max
};