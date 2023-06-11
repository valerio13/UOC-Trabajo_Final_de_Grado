#ifndef GLOBALS_H
#define GLOBALS_H

#include <Preferences.h>

enum RunState {
  IDLE,
  HUM_CONNECTING,
  HUM_READING,
  HUM_END_READING,
  IRRIG_CONNECTING,
  IRRIG_WRITING,
  IRRIG_END_WRITING
};

#endif