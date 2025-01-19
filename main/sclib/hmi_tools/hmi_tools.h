#ifndef hmi_tools_h
#define hmi_tools_h

#include "../../HMI.h"

void sclib_logic (LogicSelection *logic_selection);
void sclib_logic_SR (LogicSelection *logic_selection, int resetNotAllowed, int resetForced, int setNotAllowed, int setForced);
void sclib_logic_2 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced);
void sclib_logic_3 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced);
void sclib_logic_4 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced);
void sclib_logic_5 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced);
void sclib_logic_6 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced, int _6NotAllowed, int _6Forced);
void sclib_logic_7 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced, int _6NotAllowed, int _6Forced, int _7NotAllowed, int _7Forced);
void sclib_logic_8 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced, int _6NotAllowed, int _6Forced, int _7NotAllowed, int _7Forced, int _8NotAllowed, int _8Forced);
void sclib_logic_generic(LogicSelection *logic_selection, uint8_t *force, uint8_t *NotAllowed);
//void sclib_analogic(void *Analogic, int force, float forceValue, int NotAllowed);

#endif