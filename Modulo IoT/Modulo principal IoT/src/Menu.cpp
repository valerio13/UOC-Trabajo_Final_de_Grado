#include "menu.h"
#include "OledDisplay.h"
#include "config.h"
#include "Arduino.h"

// Declaración de la variable global externa
extern MenuState *currentMenuState;

// Implementación de MainMenuState
MainMenuState::MainMenuState() {
  name = "MENU PRINCIPAL";
  menuSize = 3;
  String options[] = {"Modulo humedad", "Modulo riego", "Servicio IoT"};
  for (int i = 0; i < menuSize; i++) {
    menuOtionsStr[i] = options[i]; // Asigna los valores a la matriz
  }
}

// Implementación de MainMenuState
void MainMenuState::handleInput(int input) {
  if (input == BTN_DOWN) {
    menuIndex++;
    if (menuIndex >= menuSize)
      menuIndex = 0;
  } else if (input == BTN_UP) {
    menuIndex--;
    if (menuIndex < 0)
      menuIndex = menuSize - 1;
  } else if (input == BTN_ENTER) {
    switch (menuIndex) {
    case 0:
      currentMenuState = &moistureMenuState;
      break;
    case 1:
      currentMenuState = &irrigationMenuState;
      break;
    case 2:
      // Navegar al submenú 3
      currentMenuState = &subMenu3State;
      break;
    }
  }
}

void MainMenuState::display() {
  displayMenu(name, menuOtionsStr, menuSize, menuIndex);
}

// Implementación de MoistureMenuState
MoistureMenuState::MoistureMenuState() {
  name = "MENU HUMEDAD";
  menuSize = 3;
  String options[] = {"Configurar umbral", "Calibrar sequedad",
                      "Calibrar humedad max"};
  for (int i = 0; i < menuSize; i++) {
    menuOtionsStr[i] = options[i]; // Asigna los valores a la matriz
  }
}

// Implementación de MoistureMenuState
void MoistureMenuState::handleInput(int input) {
  if (input == BTN_DOWN) {
    menuIndex++;
    if (menuIndex >= menuSize)
      menuIndex = 0;
  } else if (input == BTN_UP) {
    menuIndex--;
    if (menuIndex < 0)
      menuIndex = menuSize - 1;
  } else if (input == BTN_ENTER) {
    switch (menuIndex) {
    case 0:
      // Navegar al submenú 1
      currentMenuState = &subMenu1State;
      break;
    case 1:
      // Navegar al submenú 2
      currentMenuState = &subMenu2State;
    case 2:
      // Navegar al submenú 3
      currentMenuState = &subMenu3State;
    default:
      break;
    }
  } else if (input == BTN_ESC) {
    currentMenuState = &mainMenuState;
    menuIndex = 0;
  }
}

void MoistureMenuState::display() {
  displayMenu(name, menuOtionsStr, menuSize, menuIndex);
}

// Implementación de IrrigationMenuState
IrrigationMenuState::IrrigationMenuState() {
  name = "MENU RIEGO";
  menuSize = 2;
  String options[] = {"Calibrar", "Regar"};
  for (int i = 0; i < menuSize; i++) {
    menuOtionsStr[i] = options[i]; // Asigna los valores a la matriz
  }
}

void IrrigationMenuState::handleInput(int input) {
  if (input == BTN_DOWN) {
    menuIndex++;
    if (menuIndex >= menuSize)
      menuIndex = 0;
  } else if (input == BTN_UP) {
    menuIndex--;
    if (menuIndex < 0)
      menuIndex = menuSize - 1;
  } else if (input == BTN_ENTER) {
    switch (menuIndex) {
    case 0:
      // Navegar al submenú 1
      currentMenuState = &subMenu1State;
      break;
    case 1:
      // Navegar al submenú 2
      currentMenuState = &subMenu2State;
    case 2:
      // Navegar al submenú 3
      currentMenuState = &subMenu3State;
    default:
      break;
    }
  } else if (input == BTN_ESC) {
    currentMenuState = &mainMenuState;
    menuIndex = 0;
  }
}

void IrrigationMenuState::display() {
  displayMenu(name, menuOtionsStr, menuSize, menuIndex);
}

// Implementación de SubMenuState
SubMenuState::SubMenuState(const char *name) : name(name) {}

void SubMenuState::handleInput(int input) {
  if (input == 0) {
    // Volver al menú principal
    currentMenuState = &mainMenuState;
  } else {
    // Realizar acciones específicas del submenú
    Serial.print("Acción en ");
    Serial.print(name);
    Serial.print(": ");
    Serial.println(input);
  }
}

void SubMenuState::display() {
  Serial.print("=== ");
  Serial.print(name);
  Serial.println(" ===");
  Serial.println("0. Volver al menú principal");
  Serial.println("1. Acción 1");
  Serial.println("2. Acción 2");
  Serial.println("3. Acción 3");
}
