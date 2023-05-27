#ifndef PAGESTATE_H
#define PAGESTATE_H

#include <Arduino.h>

class PageState;
class MainPageState;
class MainMenuState;
class SubMenuState;
class MoistureMenuState;
class IrrigationMenuState;

// Declaración de la variable global externa
extern PageState *currentPageState;
extern MainPageState mainPageState;
extern MainMenuState mainMenuState;
extern MoistureMenuState moistureMenuState;
extern IrrigationMenuState irrigationMenuState;

extern SubMenuState subMenu1State;
extern SubMenuState subMenu2State;
extern SubMenuState subMenu3State;

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
class SubMenuState : public PageState {
public:
  SubMenuState(const char *name);
  void handleInput(int input) override;
  void display() override;
  // void loopPageState() override;
private:
  const char *name;
};
#endif