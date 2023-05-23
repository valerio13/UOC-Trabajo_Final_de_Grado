#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

class MenuState;
class MainMenuState;
class SubMenuState;
class MoistureMenuState;
class IrrigationMenuState;

// Declaración de la variable global externa
extern MenuState *currentMenuState;
extern MainMenuState mainMenuState;
extern MoistureMenuState moistureMenuState;
extern IrrigationMenuState irrigationMenuState;

extern SubMenuState subMenu1State;
extern SubMenuState subMenu2State;
extern SubMenuState subMenu3State;

// Clase abstracta que define el estado base
class MenuState {
public:
  virtual void handleInput(int input) = 0;
  virtual void display() = 0;

protected:
  String name;
  int menuIndex = 0;
  byte menuSize = 1;
  String menuOtionsStr[5];
};

// Implementación de un estado concreto
class MainMenuState : public MenuState {
public:
  MainMenuState();
  void handleInput(int input) override;
  void display() override;
};

// Implementación de un estado concreto
class MoistureMenuState : public MenuState {
public:
  MoistureMenuState();
  void handleInput(int input) override;
  void display() override;
};

// Implementación de un estado concreto
class IrrigationMenuState : public MenuState {
public:
  IrrigationMenuState();
  void handleInput(int input) override;
  void display() override;
};

// Implementación de un estado concreto
class SubMenuState : public MenuState {
public:
  SubMenuState(const char *name);
  void handleInput(int input) override;
  void display() override;

private:
  const char *name;
};
#endif // MENU_H