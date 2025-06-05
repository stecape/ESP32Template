
#ifndef HMI_h
#define HMI_h

#include "time.h"
#include <stdbool.h>

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
	float HMIValue;
	float Value;
} _Act;

typedef struct {
	float Min;
	float Max;
} _Limit;

typedef struct {
	_Set Set;
	_Limit Limit;
	int Decimals;
	bool Init;
} Set;

typedef struct {
	_Act Act;
	_Limit Limit;
	int Decimals;
} Act;

typedef struct {
	_Set Set;
	_Act Act;
	_Limit Limit;
	int Decimals;
	bool Init;
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
	bool Q;
} Alarm;

typedef struct {
	float Error;
	Set kP;
	Set Gp;
	Set Ti;
	Set Td;
	float Correction;
	Set PidMin;
	Set PidMax;
	float PidOut;
	float Reference;
	float RawOut;
	Set OutMin;
	Set OutMax;
	float Out;
	float kpError;
	float ProportionalCorrection;
	float IntegralCorrection;
	float DerivativeCorrection;
	float OutSat;
	Set OutGradient;
	Set SetpointGradient;
	Set Taw;
	float AntiWindupContribute;
	float Set;
	float Act;
	float ManualRef;
	bool ManualMode;
	bool Stop;
} PID;

typedef struct {
	Set _0;
	Set _1;
	Set _2;
	Set _3;
	Set _4;
	Set _5;
} DataSet;

typedef struct {
	DataSet Times;
	DataSet Values;
	Act Output;
} Interpolation;


typedef struct {
	LogicSelection Heating;
	LogicSelection Mode;
	Set PowerReference;
	Set TemperatureReference;
	Interpolation Profile;
	PID PID;
	Act ActualTemperature;
	Act ActualPower;
} _HMI;

extern _HMI HMI;
extern _HMI PLC;

extern int id[185];
extern int type[185];
extern void *HMI_pointer[185];
extern void *PLC_pointer[185];

#endif
  