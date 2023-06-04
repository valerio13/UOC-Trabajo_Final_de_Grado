#ifndef PAGESTATE_H
#define PAGESTATE_H

#include <Arduino.h>
#include <Preferences.h>

class PageState;
class MainPageState;
class MainMenuState;
class SubMenuState;
class MoistureMenuState;
class IrrigationMenuState;
class ThresholdPageState;
class CalibrationPageState;
class SelectOutputPageState;
class IrrigationCalibMenuState;

// Declaración de la variable global externa
extern PageState *currentPageState;
extern MainPageState mainPageState;
extern MainMenuState mainMenuState;
extern MoistureMenuState moistureMenuState;
extern IrrigationMenuState irrigationMenuState;
extern ThresholdPageState thresholdPageState;
extern CalibrationPageState calibrationHumPageState;
extern CalibrationPageState calibrationDryPageState;
extern SelectOutputPageState selectOutputPageState;

extern IrrigationCalibMenuState irrigationOffsetCalibMenuState;
extern IrrigationCalibMenuState irrigationCalibMenuState;
extern IrrigationCalibMenuState runIrrigationMenuState;

extern int humidityThreshold;
extern Preferences preferences;

// Clase abstracta que define el estado base
class PageState {
public:
  virtual void handleInput(int input) = 0;
  virtual void display() = 0;
  virtual void loopPageState() {}

protected:
  String name;
  int menuIndex = 0;
  byte menuSize = 1;
  String menuOtionsStr[5];
};

// Implementación de un estado concreto
class MainPageState : public PageState {
public:
  MainPageState();
  void handleInput(int input) override;
  void display() override;
  void loopPageState() override;
};

// Implementación de un estado concreto
class MainMenuState : public PageState {
public:
  MainMenuState();
  void handleInput(int input) override;
  void display() override;
  // void loopPageState() override;
};

// Implementación de un estado concreto
class MoistureMenuState : public PageState {
public:
  MoistureMenuState();
  void handleInput(int input) override;
  void display() override;
  // void loopPageState() override;
};

// Implementación de un estado concreto
class IrrigationMenuState : public PageState {
public:
  IrrigationMenuState();
  void handleInput(int input) override;
  void display() override;
  // void loopPageState() override;
};

// Implementación de un estado concreto
class ThresholdPageState : public PageState {
public:
  ThresholdPageState();
  void handleInput(int input) override;
  void display() override;
};

// Implementación de un estado concreto
class CalibrationPageState : public PageState {
public:
  CalibrationPageState(const char *pagName, const char *characteristic);
  void handleInput(int input) override;
  void display() override;
  void loopPageState() override;

private:
  const char *name;
  const char *characteristic;
};

// Implementación de SelectOutputPageState
class SelectOutputPageState : public PageState {
public:
  SelectOutputPageState();
  void handleInput(int input);
  void display();
};

// Implementación de un estado concreto
class IrrigationCalibMenuState : public PageState {
public:
  IrrigationCalibMenuState(short *ptrMenuSelection, short *ptrSeconds);
  void handleInput(int input) override;
  void display() override;

private:
  short *ptrActiveSeconds;
  short *ptrMenuSelection;
};

// Implementación de SelectOutputPageState
class SetTimePageState : public PageState {
public:
  SetTimePageState();
  void handleInput(int input);
  void display();
};
#endif