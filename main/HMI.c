#include "HMI.h"

_HMI HMI = {
	.BatteryLevel = {
		.Act.Value = 0,
		.Limit.Max = 0,
		.Limit.Min = 0,
	},
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
	(void*)&HMI.BatteryLevel.Act.Value,
	(void*)&HMI.BatteryLevel.Limit.Max,
	(void*)&HMI.BatteryLevel.Limit.Min
};