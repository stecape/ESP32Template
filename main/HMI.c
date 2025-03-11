#include "HMI.h"

_HMI HMI = {
	.BatteryLevel = {
		.Act.Value = 0,
		.Limit.Min = 0,
		.Limit.Max = 0,
	},
	.Light = {
		.Command = 0,
		.Status = 0,
	},
	.Temperature = {
		.Set.InputValue = 0,
		.Set.Value = 0,
		.Limit.Min = 0,
		.Limit.Max = 0,
	},
};

int id[9] = {
	3,
	5,
	6,
	8,
	9,
	12,
	14,
	15,
	16
};

int type[9] = {
	REAL,
	REAL,
	REAL,
	INT,
	INT,
	REAL,
	REAL,
	REAL,
	REAL
};

void *pointer[9] = {
	(void*)&HMI.BatteryLevel.Act.Value,
	(void*)&HMI.BatteryLevel.Limit.Min,
	(void*)&HMI.BatteryLevel.Limit.Max,
	(void*)&HMI.Light.Command,
	(void*)&HMI.Light.Status,
	(void*)&HMI.Temperature.Set.InputValue,
	(void*)&HMI.Temperature.Set.Value,
	(void*)&HMI.Temperature.Limit.Min,
	(void*)&HMI.Temperature.Limit.Max
};