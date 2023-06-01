#include <Arduino.h>
#include <OledDisplay.h>
#include <PageState.h>
#include <ble.h>
#include <config.h>
#include <globals.h>

// Declaración de la variable global externa
extern PageState *currentPageState;

void checkHumidity();
void displayHumidityData();

int tempThreshold;
String calibState;
bool calibStarted = false;

// Implementación de MainPageState
MainPageState::MainPageState() { Serial.println("MainPageState"); }

void MainPageState::handleInput(int input) {
  if (input == BTN_ENTER || input == BTN_ESC) {
    currentPageState = &mainMenuState;
  }
}

void MainPageState::display() { displayHumidityData(); }

void MainPageState::loopPageState() { checkHumidity(); }

// Implementación de MainMenuState
MainMenuState::MainMenuState() {
  name = "MENU PRINCIPAL";
  Serial.println(name);
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
      // currentPageState = &subMenuState;
      break;
    }
  } else if (input == BTN_ESC) {
    currentPageState = &mainPageState;
    menuIndex = 0;
  }
}

void MainMenuState::display() {
  displayMenu(name, menuOtionsStr, menuSize, menuIndex);
}

// Implementación de MoistureMenuState
MoistureMenuState::MoistureMenuState() {
  name = "MENU HUMEDAD";
  Serial.println("name");
  menuSize = 3;
  String options[] = {"Configurar umbral", "Calibrar humedad max",
                      "Calibrar sequedad"};
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
      // Navegar a la página de configuración del umbral
      currentPageState = &thresholdPageState;
      currentPageState->display();
      break;
    case 1:
      // Navegar a la página de Calibrar sequedad
      Serial.println("Selecctionado: calibrationHumPageState");
      currentPageState = &calibrationHumPageState;
      break;
    case 2:
      // Navegar a la página de Calibrar humedad max
      Serial.println("Selecctionado: calibrationDryPageState");
      currentPageState = &calibrationDryPageState;
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
  Serial.println("name");
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
      // currentPageState = &subMenu1State;
      break;
    case 1:
      // Navegar al submenú 2
      // currentPageState = &subMenu2State;
    case 2:
      // Navegar al submenú 3
      // currentPageState = &subMenu3State;
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

// Implementación de IrrigationMenuState
ThresholdPageState::ThresholdPageState() { name = "Configuracion umbral"; }

bool thrHandleInputFirstTime = true;

void ThresholdPageState::handleInput(int input) {
  if (thrHandleInputFirstTime)
    tempThreshold = humidityThreshold;
  thrHandleInputFirstTime = false;

  if (input == BTN_UP) {
    tempThreshold++;
    if (tempThreshold >= 100)
      tempThreshold = 100;
    display();
  } else if (input == BTN_DOWN) {
    tempThreshold--;
    if (tempThreshold < 0)
      tempThreshold = 0;
    display();
  } else if (input == BTN_ENTER) {
    preferences.putInt(HUMIDITY_THRESHOLD_STR, tempThreshold);
    humidityThreshold =
        preferences.getInt(HUMIDITY_THRESHOLD_STR, tempThreshold);
  } else if (input == BTN_ESC) {
    currentPageState = &moistureMenuState;
    thrHandleInputFirstTime = true;
  }
}

void ThresholdPageState::display() {
  String description = "Nuevo umbral:";
  if (thrHandleInputFirstTime)
    tempThreshold = humidityThreshold;
  thrHandleInputFirstTime = false;

  displaySubMenuStr(name, description, String(tempThreshold));
}

// Implementación de IrrigationMenuState
CalibrationPageState::CalibrationPageState(const char *pagName,
                                           const char *characteristic)
    : name(pagName), characteristic(characteristic) {}

void CalibrationPageState::handleInput(int input) {
  if (input == BTN_ENTER && calibStarted == false) {
    if (setCalibData(String(characteristic))) {
      Serial.println("calibStarted = true");
      calibStarted = true;
    } else
      calibStarted = false;
    Serial.println("calibStarted = false");
  } else if (input == BTN_ESC) {
    currentPageState = &moistureMenuState;
    calibStarted = false;
  }
}

void CalibrationPageState::display() {
  String description;
  if (!calibStarted) {
    description = "Iniciar...";
  }
  if (calibState == START_CALIB) {
    description = "Iniciando...";
  } else if (calibState == CALIB_ON) {
    description = "En curso.";
  } else if (calibState == CALIB_OK) {
    description = "Finalizada correctamente.";
  } else if (calibState == CALIB_NOK) {
    description = "Error";
  }

  displaySubMenuStr(name, description, "");
}

void CalibrationPageState::loopPageState() {
  if (calibStarted) {
    calibState = getCalibData(String(characteristic));
  }
  display();
}

// Métodos específicos
void checkHumidity() {
  static long lastTime = 0;
  long now = millis();
  if (abs(now - lastTime) > READ_HUM_DELAY) {
    lastTime = now; // Stores the last time that this "if" was executed.

    displayMessage("Conectando con el", "sensor de humedad", "");
    delay(1000);
    humidityValue = getBleData();
    displayHumidityData();
  }
}

void displayHumidityData() {
  String line1 = String("Umbral: ");
  line1.concat(humidityThreshold);
  line1.concat("%");

  String line2 = String("Humedad: ");
  line2.concat(humidityValue);
  line2.concat("%");

  if (humidityValue > humidityThreshold) {
    displayMessage("Lectura de humedad", line1, line2);
  } else {
    displayErrorMessage("La planta necesita agua!", line1, line2);
  }
}