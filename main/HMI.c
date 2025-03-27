#include "HMI.h"

_HMI HMI = {
	.BatteryLevel = {
		.Decimals = 0,
		.Act.HMIValue = 0,
		.Act.Value = 0,
		.Limit.Min = 0,
		.Limit.Max = 100,
	},
	.Light = {
		.Command = 0,
		.Status = 0,
	},
	.Temperature = {
		.Decimals = 0,
		.Set.InputValue = 0,
		.Set.Value = 0,
		.Limit.Min = 20,
		.Limit.Max = 70,
	},
	.Pressure = {
		.Decimals = 1,
		.Set.InputValue = 0,
		.Set.Value = 0,
		.Act.HMIValue = 0,
		.Act.Value = 0,
		.Limit.Min = 0,
		.Limit.Max = 10,
	},
};

_HMI PLC = {
	.BatteryLevel = {
		.Decimals = 0,
		.Act.HMIValue = 0,
		.Act.Value = 0,
		.Limit.Min = 0,
		.Limit.Max = 100,
	},
	.Light = {
		.Command = 0,
		.Status = 0,
	},
	.Temperature = {
		.Decimals = 0,
		.Set.InputValue = 0,
		.Set.Value = 0,
		.Limit.Min = 20,
		.Limit.Max = 70,
	},
	.Pressure = {
		.Decimals = 1,
		.Set.InputValue = 0,
		.Set.Value = 0,
		.Act.HMIValue = 0,
		.Act.Value = 0,
		.Limit.Min = 0,
		.Limit.Max = 10,
	},
};

int id[19] = {
	11,
	14,
	15,
	13,
	16,
	18,
	19,
	23,
	24,
	25,
	26,
	27,
	32,
	33,
	34,
	35,
	36,
	37,
	38
};

int type[19] = {
	REAL,
	REAL,
	REAL,
	INT,
	REAL,
	INT,
	INT,
	REAL,
	REAL,
	INT,
	REAL,
	REAL,
	INT,
	REAL,
	REAL,
	REAL,
	REAL,
	REAL,
	REAL
};

void *HMI_pointer[19] = {
	(void*)&HMI.BatteryLevel.Act.HMIValue,
	(void*)&HMI.BatteryLevel.Limit.Min,
	(void*)&HMI.BatteryLevel.Limit.Max,
	(void*)&HMI.BatteryLevel.Decimals,
	(void*)&HMI.BatteryLevel.Act.Value,
	(void*)&HMI.Light.Command,
	(void*)&HMI.Light.Status,
	(void*)&HMI.Temperature.Set.InputValue,
	(void*)&HMI.Temperature.Limit.Min,
	(void*)&HMI.Temperature.Decimals,
	(void*)&HMI.Temperature.Set.Value,
	(void*)&HMI.Temperature.Limit.Max,
	(void*)&HMI.Pressure.Decimals,
	(void*)&HMI.Pressure.Set.InputValue,
	(void*)&HMI.Pressure.Set.Value,
	(void*)&HMI.Pressure.Act.HMIValue,
	(void*)&HMI.Pressure.Limit.Min,
	(void*)&HMI.Pressure.Limit.Max,
	(void*)&HMI.Pressure.Act.Value
};

void *PLC_pointer[19] = {
	(void*)&PLC.BatteryLevel.Act.HMIValue,
	(void*)&PLC.BatteryLevel.Limit.Min,
	(void*)&PLC.BatteryLevel.Limit.Max,
	(void*)&PLC.BatteryLevel.Decimals,
	(void*)&PLC.BatteryLevel.Act.Value,
	(void*)&PLC.Light.Command,
	(void*)&PLC.Light.Status,
	(void*)&PLC.Temperature.Set.InputValue,
	(void*)&PLC.Temperature.Limit.Min,
	(void*)&PLC.Temperature.Decimals,
	(void*)&PLC.Temperature.Set.Value,
	(void*)&PLC.Temperature.Limit.Max,
	(void*)&PLC.Pressure.Decimals,
	(void*)&PLC.Pressure.Set.InputValue,
	(void*)&PLC.Pressure.Set.Value,
	(void*)&PLC.Pressure.Act.HMIValue,
	(void*)&PLC.Pressure.Limit.Min,
	(void*)&PLC.Pressure.Limit.Max,
	(void*)&PLC.Pressure.Act.Value
};