#include "../../HMI.h"
#include "../../services/mqtt/mqtt.h"
#include <math.h>

#define _SET 1
#define _RESET 2

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif

static size_t HMI_array_length = ARRAY_SIZE(id);

void sclib_logic (LogicSelection *logic_selection) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
    ESP_LOGI("DEBUG", "Status initialization: %d", logic_selection->Status);
  }
  //management
  for (int i = 0; i < 8; i++) {
    if (1 & (logic_selection->Command >> i)) {
      ESP_LOGI("DEBUG", "Received Value: %d", logic_selection->Command);
      logic_selection->Status = logic_selection->Command;
      int command = 0;
      logic_selection->Command = command;
      break;
    }
  }
}

void sclib_logic_SR (LogicSelection *logic_selection, int resetNotAllowed, int resetForced, int setNotAllowed, int setForced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (resetForced){
    logic_selection->Status = _RESET;
  }
  else if (setForced && !resetForced){
    logic_selection->Status = _SET;
  }
  else {
    switch (logic_selection->Command) {
      case _SET:
        if (!setNotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case _RESET:
        if (!resetNotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

void sclib_logic_2 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (_1Forced){
    logic_selection->Status = 1;
  }
  else if (_2Forced && !_1Forced){
    logic_selection->Status = 2;
  }
  else {
    switch (logic_selection->Command) {
      case 1:
        if (!_1NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 2:
        if (!_2NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

void sclib_logic_3 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (_1Forced){
    logic_selection->Status = 1;
  }
  else if (_2Forced && !_1Forced){
    logic_selection->Status = 2;
  }
  else if (_3Forced && !_1Forced && !_2Forced){
    logic_selection->Status = 4;
  }
  else {
    switch (logic_selection->Command) {
      case 1:
        if (!_1NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 2:
        if (!_2NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 4:
        if (!_3NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

void sclib_logic_4 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (_1Forced){
    logic_selection->Status = 1;
  }
  else if (_2Forced && !_1Forced){
    logic_selection->Status = 2;
  }
  else if (_3Forced && !_1Forced && !_2Forced){
    logic_selection->Status = 4;
  }
  else if (_4Forced && !_1Forced && !_2Forced && !_3Forced){
    logic_selection->Status = 8;
  }
  else {
    switch (logic_selection->Command) {
      case 1:
        if (!_1NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 2:
        if (!_2NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 4:
        if (!_3NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 8:
        if (!_4NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

void sclib_logic_5 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (_1Forced){
    logic_selection->Status = 1;
  }
  else if (_2Forced && !_1Forced){
    logic_selection->Status = 2;
  }
  else if (_3Forced && !_1Forced && !_2Forced){
    logic_selection->Status = 4;
  }
  else if (_4Forced && !_1Forced && !_2Forced && !_3Forced){
    logic_selection->Status = 8;
  }
  else if (_5Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced){
    logic_selection->Status = 16;
  }
  else {
    switch (logic_selection->Command) {
      case 1:
        if (!_1NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 2:
        if (!_2NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 4:
        if (!_3NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 8:
        if (!_4NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 16:
        if (!_5NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

void sclib_logic_6 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced, int _6NotAllowed, int _6Forced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (_1Forced){
    logic_selection->Status = 1;
  }
  else if (_2Forced && !_1Forced){
    logic_selection->Status = 2;
  }
  else if (_3Forced && !_1Forced && !_2Forced){
    logic_selection->Status = 4;
  }
  else if (_4Forced && !_1Forced && !_2Forced && !_3Forced){
    logic_selection->Status = 8;
  }
  else if (_5Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced){
    logic_selection->Status = 16;
  }
  else if (_6Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced && !_5Forced){
    logic_selection->Status = 32;
  }
  else {
    switch (logic_selection->Command) {
      case 1:
        if (!_1NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 2:
        if (!_2NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 4:
        if (!_3NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 8:
        if (!_4NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 16:
        if (!_5NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 32:
        if (!_6NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

void sclib_logic_7 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced, int _6NotAllowed, int _6Forced, int _7NotAllowed, int _7Forced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (_1Forced){
    logic_selection->Status = 1;
  }
  else if (_2Forced && !_1Forced){
    logic_selection->Status = 2;
  }
  else if (_3Forced && !_1Forced && !_2Forced){
    logic_selection->Status = 4;
  }
  else if (_4Forced && !_1Forced && !_2Forced && !_3Forced){
    logic_selection->Status = 8;
  }
  else if (_5Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced){
    logic_selection->Status = 16;
  }
  else if (_6Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced && !_5Forced){
    logic_selection->Status = 32;
  }
  else if (_7Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced && !_5Forced && !_6Forced){
    logic_selection->Status = 64;
  }
  else {
    switch (logic_selection->Command) {
      case 1:
        if (!_1NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 2:
        if (!_2NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 4:
        if (!_3NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 8:
        if (!_4NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 16:
        if (!_5NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 32:
        if (!_6NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 64:
        if (!_7NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

void sclib_logic_8 (LogicSelection *logic_selection, int _1NotAllowed, int _1Forced, int _2NotAllowed, int _2Forced, int _3NotAllowed, int _3Forced, int _4NotAllowed, int _4Forced, int _5NotAllowed, int _5Forced, int _6NotAllowed, int _6Forced, int _7NotAllowed, int _7Forced, int _8NotAllowed, int _8Forced) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int command = 0;
  if (_1Forced){
    logic_selection->Status = 1;
  }
  else if (_2Forced && !_1Forced){
    logic_selection->Status = 2;
  }
  else if (_3Forced && !_1Forced && !_2Forced){
    logic_selection->Status = 4;
  }
  else if (_4Forced && !_1Forced && !_2Forced && !_3Forced){
    logic_selection->Status = 8;
  }
  else if (_5Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced){
    logic_selection->Status = 16;
  }
  else if (_6Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced && !_5Forced){
    logic_selection->Status = 32;
  }
  else if (_7Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced && !_5Forced && !_6Forced){
    logic_selection->Status = 64;
  }
  else if (_8Forced && !_1Forced && !_2Forced && !_3Forced && !_4Forced && !_5Forced && !_6Forced && !_7Forced){
    logic_selection->Status = 128;
  }
  else {
    switch (logic_selection->Command) {
      case 1:
        if (!_1NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 2:
        if (!_2NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 4:
        if (!_3NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 8:
        if (!_4NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 16:
        if (!_5NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 32:
        if (!_6NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 64:
        if (!_7NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      case 128:
        if (!_8NotAllowed) {
          if (logic_selection->Status != logic_selection->Command) {
            logic_selection->Status = logic_selection->Command;
          }
        }
        logic_selection->Command = command;
        break;
      default:
        // Handle invalid command if necessary
        break;
    }
  }
}

// Funzione per ottenere il valore dell'ennesimo bit di un intero
int get_bit_value(uint8_t number, int bit_position) {
  return (number >> bit_position) & 1;
}

// Funzione generica per gestire la logic selection
void sclib_logic_generic(LogicSelection *logic_selection, uint8_t *force, uint8_t *NotAllowed) {
  //initialization
  if (logic_selection->Status == 0){
    int status = 1;
    logic_selection->Status = status;
  }
  //management
  int forced = 0;
  for (int i = 0; i < 8; i++) {
    int force_bit = get_bit_value(*force, i);
    if (force_bit) {
      if (logic_selection->Status != 1 << i) {
        int val = 1 << i;
        logic_selection->Status = val;
      }
      forced = 1;
      break;
    }
  }
  if (!forced) {
    for (int i = 0; i < 8; i++) {
      int NotAllowed_bit = get_bit_value(*NotAllowed, i);
      if (1 & (logic_selection->Command >> i)) {
        if (!NotAllowed_bit) {
          logic_selection->Status = logic_selection->Command;
          int command = 0;
          logic_selection->Command = command;
          break;
        }
      }
    }
  }
}



// Funzione generica per gestire un Set
void sclib_Set(Set *set, int force, float forceValue, int NotAllowed) {
  float _forceValue = forceValue;

  //Min and Max check
  if (set->Limit.Min >= set->Limit.Max) {
    return;
  }

  //Initialization
  if (set->Set.Value > set->Limit.Max){
    set->Set.Value = set->Limit.Max;
    set->Set.InputValue = set->Limit.Max;
  }
  if (set->Set.Value < set->Limit.Min) {
    set->Set.Value = set->Limit.Min;
    set->Set.InputValue = set->Limit.Min;
  } 

  //Forced value
  if (force) {
    if (forceValue != set->Set.Value) {
      if (forceValue <= set->Limit.Max && forceValue >= set->Limit.Min) {
        set->Set.Value = _forceValue;
        set->Set.InputValue = _forceValue;
      }
    }
  } else {
    if (set->Set.InputValue != set->Set.Value) {
      // Check if the value inputing is allowed
      if (NotAllowed){
        set->Set.InputValue = set->Set.Value;
      } else {
        // If allowed, check if the value is in the range
        if (set->Set.InputValue <= set->Limit.Max && set->Set.InputValue >= set->Limit.Min) {
          // If is in range, update the value
          set->Set.Value = set->Set.InputValue;
        } else {
          // If is not in range, update the input value with the current value
          set->Set.InputValue = set->Set.Value;
        }
      }
    }
  }
}


// Funzione generica per gestire un SetAct
void sclib_SetAct(SetAct *setact, int force, float forceValue, int NotAllowed) {
  float _forceValue = forceValue;

  //Min and Max check
  if (setact->Limit.Min >= setact->Limit.Max) {
    return;
  }

  //Initialization
  if (setact->Set.Value > setact->Limit.Max){
    setact->Set.Value = setact->Limit.Max;
    setact->Set.InputValue = setact->Limit.Max;
  }
  if (setact->Set.Value < setact->Limit.Min) {
    setact->Set.Value = setact->Limit.Min;
    setact->Set.InputValue = setact->Limit.Min;
  } 

  //Forced value
  if (force) {
    if (forceValue != setact->Set.Value) {
      if (forceValue <= setact->Limit.Max && forceValue >= setact->Limit.Min) {
        setact->Set.Value = _forceValue;
        setact->Set.InputValue = _forceValue;
      }
    }
  } else {
    if (setact->Set.InputValue != setact->Set.Value) {
      // Check if the value inputing is allowed
      if (NotAllowed){
        setact->Set.InputValue = setact->Set.Value;
      } else {
        // If allowed, check if the value is in the range
        if (setact->Set.InputValue <= setact->Limit.Max && setact->Set.InputValue >= setact->Limit.Min) {
          // If is in range, update the value
          setact->Set.Value = setact->Set.InputValue;
        } else {
          // If is not in range, update the input value with the current value
          setact->Set.InputValue = setact->Set.Value;
        }
      }
    }
  }
}

// Funzione generica per scrivere un valore in Act
void sclib_writeAct(Act *act, float value){  
  int scale = (int)pow(10, act->Decimals);
  act->Act.HMIValue = (float)((int)(value * scale + 0.5)) / scale; // Usa floor per evitare imprecisioni
  act->Act.Value = value;

  // Trova l'indice di act->Value in PLC_pointer
  for (size_t i = 0; i < HMI_array_length; i++) {
    if (PLC_pointer[i] == (void *)&act->Act.Value) {
      // Scrivi il valore di percentage nel corrispondente HMI_pointer
      *(float *)HMI_pointer[i] = value;
      break;
    }
  }
};

// Funzione generica per scrivere un valore in SetAct
void sclib_writeSetAct(SetAct *setact, float value){  
  int scale = (int)pow(10, setact->Decimals);
  setact->Act.HMIValue = (float)((int)(value * scale + 0.5)) / scale; // Usa floor per evitare imprecisioni
  setact->Act.Value = value;

  // Per evitare che il valore venga rilevato come differente dalla funzione che si occupa di pubblicare il refresh, il value viene copiato identico sulla sua controparte HMI.
  // Questo perché non voglio che venga trasmesso al broker a causa dell'elevato rate di refresh e della scarsa rilevanza su HMI
  // Trova l'indice di setact->Value in PLC_pointer
  for (size_t i = 0; i < HMI_array_length; i++) {
    if (PLC_pointer[i] == (void *)&setact->Act.Value) {
      // Scrivi il valore di percentage nel corrispondente HMI_pointer
      *(float *)HMI_pointer[i] = value;
      break;
    }
  }
};