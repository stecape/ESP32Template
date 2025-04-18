
#include "HMI.h"

_HMI HMI = {

	.BatteryLevel = {
		.Act.HMIValue = 0,
		.Decimals = 0,
		.Act.Value = 0,
		.Limit.Max = 100,
		.Limit.Min = 0,
	},
	.Light = {
		.Status = 0,
		.Command = 0,
	},
	.Temperature = {
		.Decimals = 0,
		.Init = false,
		.Limit.Min = 20,
		.Set.InputValue = 0,
		.Limit.Max = 70,
		.Set.Value = 0,
	},
	.Pressure = {
		.Decimals = 0,
		.Init = false,
		.Limit.Min = 0,
		.Limit.Max = 10,
		.Set.InputValue = 0,
		.Set.Value = 0,
		.Act.HMIValue = 0,
		.Act.Value = 0,
	},
	.LightOn = {
		.Reaction = 0,
		.Status = 0,
		.Ts = 0,
		.Q = false,
	},
};

_HMI PLC = {

	.BatteryLevel = {
		.Act.HMIValue = 0,
		.Decimals = 0,
		.Act.Value = 0,
		.Limit.Max = 100,
		.Limit.Min = 0,
	},
	.Light = {
		.Status = 0,
		.Command = 0,
	},
	.Temperature = {
		.Decimals = 0,
		.Init = false,
		.Limit.Min = 20,
		.Set.InputValue = 0,
		.Limit.Max = 70,
		.Set.Value = 0,
	},
	.Pressure = {
		.Decimals = 0,
		.Init = false,
		.Limit.Min = 0,
		.Limit.Max = 10,
		.Set.InputValue = 0,
		.Set.Value = 0,
		.Act.HMIValue = 0,
		.Act.Value = 0,
	},
	.LightOn = {
		.Reaction = 0,
		.Status = 0,
		.Ts = 0,
		.Q = false,
	},
};

int id[25] = {
	3,
	4,
	6,
	7,
	8,
	10,
	11,
	15,
	16,
	17,
	18,
	19,
	20,
	22,
	26,
	27,
	28,
	29,
	30,
	31,
	32,
	34,
	35,
	36,
	37
};

int type[25] = {
	REAL,
	INT,
	REAL,
	REAL,
	REAL,
	INT,
	INT,
	INT,
	BOOL,
	REAL,
	REAL,
	REAL,
	REAL,
	INT,
	BOOL,
	REAL,
	REAL,
	REAL,
	REAL,
	REAL,
	REAL,
	INT,
	INT,
	TIMESTAMP,
	BOOL
};

void *HMI_pointer[25] = {
	(void*)&HMI.BatteryLevel.Act.HMIValue,
	(void*)&HMI.BatteryLevel.Decimals,
	(void*)&HMI.BatteryLevel.Act.Value,
	(void*)&HMI.BatteryLevel.Limit.Max,
	(void*)&HMI.BatteryLevel.Limit.Min,
	(void*)&HMI.Light.Status,
	(void*)&HMI.Light.Command,
	(void*)&HMI.Temperature.Decimals,
	(void*)&HMI.Temperature.Init,
	(void*)&HMI.Temperature.Limit.Min,
	(void*)&HMI.Temperature.Set.InputValue,
	(void*)&HMI.Temperature.Limit.Max,
	(void*)&HMI.Temperature.Set.Value,
	(void*)&HMI.Pressure.Decimals,
	(void*)&HMI.Pressure.Init,
	(void*)&HMI.Pressure.Limit.Min,
	(void*)&HMI.Pressure.Limit.Max,
	(void*)&HMI.Pressure.Set.InputValue,
	(void*)&HMI.Pressure.Set.Value,
	(void*)&HMI.Pressure.Act.HMIValue,
	(void*)&HMI.Pressure.Act.Value,
	(void*)&HMI.LightOn.Reaction,
	(void*)&HMI.LightOn.Status,
	(void*)&HMI.LightOn.Ts,
	(void*)&HMI.LightOn.Q
};

void *PLC_pointer[25] = {
	(void*)&PLC.BatteryLevel.Act.HMIValue,
	(void*)&PLC.BatteryLevel.Decimals,
	(void*)&PLC.BatteryLevel.Act.Value,
	(void*)&PLC.BatteryLevel.Limit.Max,
	(void*)&PLC.BatteryLevel.Limit.Min,
	(void*)&PLC.Light.Status,
	(void*)&PLC.Light.Command,
	(void*)&PLC.Temperature.Decimals,
	(void*)&PLC.Temperature.Init,
	(void*)&PLC.Temperature.Limit.Min,
	(void*)&PLC.Temperature.Set.InputValue,
	(void*)&PLC.Temperature.Limit.Max,
	(void*)&PLC.Temperature.Set.Value,
	(void*)&PLC.Pressure.Decimals,
	(void*)&PLC.Pressure.Init,
	(void*)&PLC.Pressure.Limit.Min,
	(void*)&PLC.Pressure.Limit.Max,
	(void*)&PLC.Pressure.Set.InputValue,
	(void*)&PLC.Pressure.Set.Value,
	(void*)&PLC.Pressure.Act.HMIValue,
	(void*)&PLC.Pressure.Act.Value,
	(void*)&PLC.LightOn.Reaction,
	(void*)&PLC.LightOn.Status,
	(void*)&PLC.LightOn.Ts,
	(void*)&PLC.LightOn.Q
};