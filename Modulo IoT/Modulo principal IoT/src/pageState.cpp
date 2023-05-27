#include <Arduino.h>
#include <OledDisplay.h>
#include <PageState.h>
#include <ble.h>
#include <config.h>

// Declaración de la variable global externa
extern PageState *currentPageState;

void checkHumidity();

// Implementación de MainPageState
MainPageState::MainPageState() {}

void MainPageState::handleInput(int input) {
  if (input == BTN_ENTER || input == BTN_ESC) {
    currentPageState = &mainMenuState;
  }
}

void MainPageState::display() {}
void MainPageState::loopPageState() { checkHumidity(); }

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
      currentPageState = &moistureMenuState;
      break;
    case 1:
      currentPageState = &irrigationMenuState;
      break;
    case 2:
      // Navegar al submenú 3
      currentPageState = &subMenu3State;
      break;
    }
  } else if (input == BTN_ENTER) {
    currentPageState = &mainPageState;
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
      currentPageState = &subMenu1State;
      break;
    case 1:
      // Navegar al submenú 2
      currentPageState = &subMenu2State;
    case 2:
      // Navegar al submenú 3
      currentPageState = &subMenu3State;
    default:
      break;
    }
  } else if (input == BTN_ESC) {
    currentPageState = &mainMenuState;
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
      currentPageState = &subMenu1State;
      break;
    case 1:
      // Navegar al submenú 2
      currentPageState = &subMenu2State;
    case 2:
      // Navegar al submenú 3
      currentPageState = &subMenu3State;
    default:
      break;
    }
  } else if (input == BTN_ESC) {
    currentPageState = &mainMenuState;
    menuIndex = 0;
  }
}

void IrrigationMenuState::display() {
  displayMenu(name, menuOtionsStr, menuSize, menuIndex);
}

// Implementación de SubMenuState
SubMenuState::SubMenuState(const char *name) : name(name) {}
// void SubMenuState::loopPageState() { int i = 0; }

void SubMenuState::handleInput(int input) {
  if (input == 0) {
    // Volver al menú principal
    currentPageState = &mainMenuState;
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

void checkHumidity() {
  static long lastTime = 0;
  long now = millis();
  if (abs(now - lastTime) > READ_HUM_DELAY) {
    lastTime = now; // Stores the last time that this "if" was executed.

    displayMessage("Conectando con el", "sensor de humedad", "");
    delay(1000);
    std::string value = getBleData();

    String line1 = String("Umbral: ");
    line1.concat("");
    line1.concat("%");

    String line2 = String("Humedad: ");
    line2.concat(value.c_str());
    line2.concat("%");

    displayMessage("Lectura de humedad", line1, line2);
  }
}