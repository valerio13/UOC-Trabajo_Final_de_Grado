//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Gestión del menú
//////////////////////////////////////////////

#include "PageState.h"
#include "OledDisplay.h"
#include "bleHumidity.h"
#include "bleIrrigation.h"
#include "config.h"
#include "globals.h"
#include <Arduino.h>
#include <Preferences.h>

// Declaración de la variable global externa
extern PageState *currentPageState;

extern bool humidityCheckEnabled;
extern int humidityValue;

// Definición de las funciones que serán implementadas abajo
void checkHumidity();
void displayHumidityData();
String getIrrigationCalibMenuStateTitle(short irrigationSubmenuIndex);
short getIrrigationCalibTime(short irrigationSubmenuIndex);
void setBackIrrigationCalibMenuState(short irrigationSubmenuIndex);
void saveIrrigationCalibTime(short irrigationSubmenuIndex);

// Variables internas de este file
int tempThreshold;
String calibState;
bool calibStarted = false;
int irrigationOutput = 0;
short activeIrrigationTime = 0;
RunState runState = IDLE;

// Implementación de MainPageState
MainPageState::MainPageState() {}

// Gestión de las entradas
void MainPageState::handleInput(int input) {
  if (input == BTN_ENTER || input == BTN_ESC) {
    currentPageState = &mainMenuState;
  }
}

// Gestión de la visualización de los datos
void MainPageState::display() { displayHumidityData(); }

void MainPageState::loopPageState() { checkHumidity(); }

// Implementación de MainMenuState
MainMenuState::MainMenuState() {
  name = "MENU PRINCIPAL";
  Serial.println(name);
  menuSize = 4;
  String options[] = {"Modulo humedad", "Modulo riego", "Servicio IoT",
                      "Habilitar"};
  for (int i = 0; i < menuSize; i++) {
    menuOtionsStr[i] = options[i]; // Asigna los valores a la matriz
  }
}

// Gestión de las entradas
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
    case 3:
      currentPageState = &enableHumidityCheckPageState;
      break;
    }
  } else if (input == BTN_ESC) {
    currentPageState = &mainPageState;
    menuIndex = 0;
  }
}

// Gestión de la visualización de los datos
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

// Gestión de las entradas
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
      currentPageState = &calibrationHumPageState;
      break;
    case 2:
      // Navegar a la página de Calibrar humedad max
      currentPageState = &calibrationDryPageState;
    default:
      break;
    }
  } else if (input == BTN_ESC) {
    currentPageState = &mainMenuState;
    menuIndex = 0;
  }
}

// Gestión de la visualización de los datos
void MoistureMenuState::display() {
  displayMenu(name, menuOtionsStr, menuSize, menuIndex);
}

// Implementación de IrrigationMenuState
IrrigationMenuState::IrrigationMenuState() {
  name = "MENU RIEGO";
  menuSize = 4;
  String options[] = {"Selec. salida", "Calib. offset", "Calib. riego",
                      "Regar"};
  for (int i = 0; i < menuSize; i++) {
    menuOtionsStr[i] = options[i]; // Asigna los valores a la matriz
  }
}

// Gestión de las entradas
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
      // Navegar al menú de selección de salida
      currentPageState = &selectOutputPageState;
      break;
    case 1:
      // Navegar al menú calibrar offset
      currentPageState = &irrigationOffsetCalibMenuState;
      currentPageState->pageInitialState(
          getIrrigationCalibTime(offsetMenuOption));
      break;
    case 2:
      // Navegar al menú calibrar riego
      currentPageState = &irrigationCalibMenuState;
      currentPageState->pageInitialState(
          getIrrigationCalibTime(calibMenuOption));
      break;
    case 3:
      // Navegar al menú regar
      currentPageState = &runIrrigationMenuState;
      currentPageState->pageInitialState(getIrrigationCalibTime(runMenuOption));
    default:
      break;
    }
  } else if (input == BTN_ESC) {
    currentPageState = &mainMenuState;
    menuIndex = 0;
  }
}

// Gestión de la visualización de los datos
void IrrigationMenuState::display() {
  displayMenu(name + " - " + irrigationOutput, menuOtionsStr, menuSize,
              menuIndex);
}

// Implementación de ThresholdPageState
ThresholdPageState::ThresholdPageState() { name = "Configuracion umbral"; }

bool thrHandleInputFirstTime = true;

// Gestión de las entradas
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
    displayMessage(name, "Guardado", "");
    delay(3000);
    display();
  } else if (input == BTN_ESC) {
    currentPageState = &moistureMenuState;
    thrHandleInputFirstTime = true;
  }
}

// Gestión de la visualización de los datos
void ThresholdPageState::display() {
  String description = "Nuevo umbral:";
  if (thrHandleInputFirstTime)
    tempThreshold = humidityThreshold;
  thrHandleInputFirstTime = false;

  displaySubMenuStr(name, description, String(tempThreshold));
}

// Implementación de CalibrationPageState
CalibrationPageState::CalibrationPageState(const char *pagName,
                                           const char *characteristic)
    : name(pagName), characteristic(characteristic) {}

// Gestión de las entradas
void CalibrationPageState::handleInput(int input) {
  if (input == BTN_ENTER && calibStarted == false) {
    if (setHumidityCalibData(String(characteristic))) {
      calibStarted = true;
      Serial.println("calibStarted = true");
      displaySubMenuStr(name, "Iniciada!", "", "", "SALIR");
    } else {
      calibStarted = false;
      Serial.println("calibStarted = false");
    }
  } else if (input == BTN_ESC) {
    calibStarted = false;
    currentPageState = &moistureMenuState;
  }
}

// Gestión de la visualización de los datos
void CalibrationPageState::display() {
  String description;
  if (!calibStarted) {
    description = "Iniciar!";
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

  displaySubMenuStr(name, description, "", "EMPEZAR", "SALIR");
}

void CalibrationPageState::loopPageState() {
  if (calibStarted) {
    calibState = getHumidityCalibData(String(characteristic));
  }
  display();
}

// Implementación de SelectOutputPageState
SelectOutputPageState::SelectOutputPageState() { name = "Configurar salida"; }

// Gestión de las entradas
void SelectOutputPageState::handleInput(int input) {
  if (input == BTN_UP) {
    irrigationOutput++;
    if (irrigationOutput >= 4)
      irrigationOutput = 3;
    display();
  } else if (input == BTN_DOWN) {
    irrigationOutput--;
    if (irrigationOutput < 0)
      irrigationOutput = 0;
    display();
  } else if (input == BTN_ENTER) {
    currentPageState = &irrigationMenuState;
  } else if (input == BTN_ESC) {
    irrigationOutput = 0;
    currentPageState = &irrigationMenuState;
  }
}

// Gestión de la visualización de los datos
void SelectOutputPageState::display() {
  String description = "Seleccionar salida:";
  displaySubMenuStr(name, description, String(irrigationOutput));
}

// Implementación de IrrigationCalibMenuState
IrrigationCalibMenuState::IrrigationCalibMenuState(short *ptrMenuSelection)
    : ptrMenuSelection(ptrMenuSelection) {
  menuSize = 3;
  irrigationSubmenuIndex = *ptrMenuSelection;

  String options[] = {String(activeIrrigationTime) + " segundos", "Probar",
                      "Guardar"};
  for (int i = 0; i < menuSize; i++) {
    menuOtionsStr[i] = options[i]; // Asigna los valores a la matriz
  }
}

void IrrigationCalibMenuState::pageInitialState(short data) {
  Serial.print("IrrigationCalibMenuState::pageInitialState: ");
  Serial.println(irrigationSubmenuIndex);
  activeIrrigationTime = data;

  String options[] = {String(activeIrrigationTime) + " segundos", "Probar",
                      "Guardar"};
  for (int i = 0; i < menuSize; i++) {
    menuOtionsStr[i] = options[i]; // Asigna los valores a la matriz
  }
}

// Gestión de las entradas
void IrrigationCalibMenuState::handleInput(int input) {
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
      // Navegar al menú de selección de salida
      currentPageState = &setTimePageState;
      currentPageState->pageInitialState(irrigationSubmenuIndex);
      break;
    case 1:
      // Navegar al submenú 2
      setIrrigationData(irrigationOutput, activeIrrigationTime);
      break;
    case 2:
      // Navegar al submenú 3
      saveIrrigationCalibTime(irrigationSubmenuIndex);
      break;
    default:
      break;
    }
  } else if (input == BTN_ESC) {
    currentPageState = &irrigationMenuState;
    menuIndex = 0;
  }
}

// Gestión de la visualización de los datos
void IrrigationCalibMenuState::display() {
  displayMenu(getIrrigationCalibMenuStateTitle(irrigationSubmenuIndex) +
                  String(" - Sal.") + String(irrigationOutput),
              menuOtionsStr, menuSize, menuIndex);
}

// Implementación de SetTimePageState
SetTimePageState::SetTimePageState() { name = "Configurar tiempo"; }

void SetTimePageState::pageInitialState(short data) {
  irrigationSubmenuIndex = data;
  tempTime = activeIrrigationTime;
}

// Gestión de las entradas
void SetTimePageState::handleInput(int input) {
  if (input == BTN_UP) {
    tempTime++;
    if (tempTime >= 60)
      tempTime = 60;
    display();
  } else if (input == BTN_DOWN) {
    tempTime--;
    if (tempTime < 0)
      tempTime = 0;
    display();
  } else if (input == BTN_ENTER) {
    setBackIrrigationCalibMenuState(irrigationSubmenuIndex);
    currentPageState->pageInitialState(tempTime);
  } else if (input == BTN_ESC) {
    currentPageState = &irrigationMenuState;
  }
}

// Gestión de la visualización de los datos
void SetTimePageState::display() {
  String description = "Seleccionar tiempo:";
  displaySubMenuStr(getIrrigationCalibMenuStateTitle(irrigationSubmenuIndex) +
                        String(" - Sal.") + String(irrigationOutput),
                    description, String(tempTime) + String(" seg."));
}

// Métodos específicos
void checkHumidity() {
  static long lastTime = 0;

  long now = millis();
  if (abs(now - lastTime) > READ_HUM_DELAY) {
    lastTime = now; // Stores the last time that this "if" was executed.

    if (!humidityCheckEnabled) {
      displayMessage("Control de humedad ", "DESHABILITADO", "");
    } else {
      displayMessage("Conectando con el", "sensor de humedad", "");

      delay(1000);
      humidityValue = getBleHumidityData();
      displayHumidityData();
    }
  }
}

void displayHumidityData() {
  String line1 = String("Umbral: ");
  line1.concat(humidityThreshold);
  line1.concat("%");

  String line2 = String("Humedad: ");
  line2.concat(humidityValue);
  line2.concat("%");

  // Controla si el valor de humedad está por encima del umbral
  // En caso de OK: se muestra la lectura de humedad y el dato de umbral
  if (humidityValue > humidityThreshold) {
    displayMessage("Lectura de humedad", line1, line2);
  } else {
    // En caso de NOK: se muestra el error y se activa el riego
    displayErrorMessage("Agua para la planta!", line1, line2);
    short irrigationTime = getIrrigationCalibTime(offsetMenuOption);
    irrigationTime += getIrrigationCalibTime(calibMenuOption);
    Serial.print("irrigationTime: ");
    Serial.println(irrigationTime);
    delay(3000);
    setIrrigationData(irrigationOutput, activeIrrigationTime);
  }
}

String getIrrigationCalibMenuStateTitle(short irrigationSubmenuIndex) {
  switch (irrigationSubmenuIndex) {
  case 0:
    return IRRIGATION_OFFSET_MENU_TITLE;
    break;
  case 1:
    return IRRIGATION_CALIB_MENU_TITLE;
    break;
  case 2:
    return IRRIGATION_RUN_MENU_TITLE;
    break;
  default:
    return "";
    break;
  }
}

// Gestión de la opción de navegar hacia atrás
void setBackIrrigationCalibMenuState(short irrigationSubmenuIndex) {
  switch (irrigationSubmenuIndex) {
  case 0:
    // Navegar al menú calibrar offset
    currentPageState = &irrigationOffsetCalibMenuState;
    break;
  case 1:
    // Navegar al menú calibrar riego
    currentPageState = &irrigationCalibMenuState;
    break;
  case 2:
    // Navegar al menú regar
    currentPageState = &runIrrigationMenuState;
    break;
  }
}

// Recupera el dato del tiempo de riego guardado para ese determinado output
short getIrrigationCalibTime(short irrigationSubmenuIndex) {
  String key;

  switch (irrigationSubmenuIndex) {
  case 0:
    key = String(IRRIGATION_OFFSET_TIME_PREF) + "-" + String(irrigationOutput);
    return preferences.getShort(key.c_str(), 10);
    break;
  case 1:
    key = String(IRRIGATION_RUN_TIME_PREF) + "-" + String(irrigationOutput);
    return preferences.getShort(key.c_str(), 10);
    break;
  case 2:
    return 5;
    break;
  default:
    return 0;
    break;
  }
}

// Guarda el dato del tiempo de riego para ese determinado output
void saveIrrigationCalibTime(short irrigationSubmenuIndex) {
  String key;

  switch (irrigationSubmenuIndex) {
  case 0:
    key = String(IRRIGATION_OFFSET_TIME_PREF) + "-" + String(irrigationOutput);
    preferences.putShort(key.c_str(), activeIrrigationTime);
    break;
  case 1:
    key = String(IRRIGATION_RUN_TIME_PREF) + "-" + String(irrigationOutput);
    preferences.putShort(key.c_str(), activeIrrigationTime);
    break;
  }

  displayMessage("", "Guardado", "");
  delay(3000);
}

// Implementación de EnableHumidityCheckPageState
EnableHumidityCheckPageState::EnableHumidityCheckPageState() {
  name = "Control humedad";
}

// Gestión de las entradas
void EnableHumidityCheckPageState::handleInput(int input) {
  if (input == BTN_UP || input == BTN_DOWN) {
    humidityCheckEnabled = !humidityCheckEnabled;
    display();
  } else if (input == BTN_ENTER) {
    preferences.putBool(CHECK_HUMIDITY_ENABLED, humidityCheckEnabled);
    displayMessage(name, "Guardado", "");
    delay(3000);
    display();
  } else if (input == BTN_ESC) {
    humidityCheckEnabled = preferences.getBool(CHECK_HUMIDITY_ENABLED, false);
    currentPageState = &mainMenuState;
  }
}

// Gestión de la visualización de los datos
void EnableHumidityCheckPageState::display() {
  String description;
  if (humidityCheckEnabled)
    description = "HABILITADO";
  else
    description = "DESHABILITADO";

  displaySubMenuStr(name, description, "");
}