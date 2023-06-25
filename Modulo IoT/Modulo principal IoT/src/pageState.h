//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Gestión del menú
//    Para la gestión de las páginas se ha implementado el patrón de
//    programación State: todas las clases heredan de la clase base PageState
//////////////////////////////////////////////

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
class SetTimePageState;
class EnableHumidityCheckPageState;

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
extern EnableHumidityCheckPageState enableHumidityCheckPageState;
extern EnableHumidityCheckPageState enableHumidityCheckPageState;

extern IrrigationCalibMenuState irrigationOffsetCalibMenuState;
extern IrrigationCalibMenuState irrigationCalibMenuState;
extern IrrigationCalibMenuState runIrrigationMenuState;

extern int humidityThreshold;
extern Preferences preferences;
extern short offsetMenuOption;
extern short calibMenuOption;
extern short runMenuOption;

extern SetTimePageState setTimePageState;

// Clase abstracta que define el estado base
class PageState {
public:
  virtual void handleInput(int input) = 0;
  virtual void display() = 0;
  virtual void loopPageState() {}
  virtual void pageInitialState(short data) {}

protected:
  String name;
  int menuIndex = 0;
  byte menuSize = 1;
  String menuOtionsStr[5];
};

// Implementación de un estado concreto: MainPageState
class MainPageState : public PageState {
public:
  MainPageState();
  void handleInput(int input) override;
  void display() override;
  void loopPageState() override;
};

// Implementación de un estado concreto: MainMenuState
class MainMenuState : public PageState {
public:
  MainMenuState();
  void handleInput(int input) override;
  void display() override;
};

// Implementación de un estado concreto: MoistureMenuState
class MoistureMenuState : public PageState {
public:
  MoistureMenuState();
  void handleInput(int input) override;
  void display() override;
};

// Implementación de un estado concreto: IrrigationMenuState
class IrrigationMenuState : public PageState {
public:
  IrrigationMenuState();
  void handleInput(int input) override;
  void display() override;
  // void loopPageState() override;
};

// Implementación de un estado concreto: ThresholdPageState
class ThresholdPageState : public PageState {
public:
  ThresholdPageState();
  void handleInput(int input) override;
  void display() override;
};

// Implementación de un estado concreto: CalibrationPageState
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

// Implementación de SelectOutputPageState: SelectOutputPageState
class SelectOutputPageState : public PageState {
public:
  SelectOutputPageState();
  void handleInput(int input);
  void display();
};

// Implementación de un estado concreto: IrrigationCalibMenuState
class IrrigationCalibMenuState : public PageState {
public:
  IrrigationCalibMenuState(short *ptrMenuSelection);
  void handleInput(int input) override;
  void display() override;
  void pageInitialState(short data);

private:
  short *ptrMenuSelection;
  short irrigationSubmenuIndex = 0;
};

// Implementación de un estado concreto: SetTimePageState
class SetTimePageState : public PageState {
public:
  SetTimePageState();
  void handleInput(int input);
  void display();
  void pageInitialState(short data);

private:
  short tempTime = 0;
  short irrigationSubmenuIndex = 0;
};

// Implementación de un estado concreto: EnableHumidityCheckPageState
class EnableHumidityCheckPageState : public PageState {
public:
  EnableHumidityCheckPageState();
  void handleInput(int input);
  void display();
};
#endif