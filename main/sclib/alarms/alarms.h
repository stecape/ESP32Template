#ifndef alarms_h
#define alarms_h

#include "../../HMI.h"
#include <stdbool.h>

typedef enum {
  ALARM_STATUS_INACTIVE = 0,  // Alarm is inactive
  ALARM_STATUS_ACTIVE = 1,    // Alarm is active
  ALARM_STATUS_ACKNOWLEDGED = 2, // Alarm has been acknowledged
  ALARM_STATUS_GONE = 3      // The cause is gone, but the alarm is still active, waiting for acknowledgment
} AlarmStatus;

typedef enum {
  ALARM_REACTION_NO_REACTION = 0,         // No reaction
  ALARM_REACTION_FAULT = 1,        // Fault
  ALARM_REACTION_WARNING = 2,      // Just a warning
  ALARM_REACTION_NOTIFICATION = 3  // General notification
} AlarmReaction;

void alarms_ack(void);
void check_alarms(void);
void alarm(Alarm *alarm, bool execute, AlarmReaction reaction);

#endif