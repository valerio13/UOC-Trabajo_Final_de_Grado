//////////////////////////////////////////////
///   Autor: Valerio Colantonio
//////////////////////////////////////////////

#include "OledDisplay.h"
#include "PageState.h"
#include "bleHumidity.h"
#include "bleIrrigation.h"
#include "config.h"
#include "mqttService.h"
#include <Arduino.h>
#include <Preferences.h>

#define DELAY 500
#define PRESS_BUTTON_DELAY                                                     \
  500 // Define el delay usado en la lectura de los botones

// Definición de funciones
void MonitoringButtons();
void checkHumidity();

// Variables globales, principalmente las páginas del menú
MainPageState mainPageState;
MainMenuState mainMenuState;
MoistureMenuState moistureMenuState;
IrrigationMenuState irrigationMenuState;
ThresholdPageState thresholdPageState;
CalibrationPageState calibrationHumPageState("Calib. max. humedad",
                                             MAX_CALIB_HUM_CHAR_UUID);
CalibrationPageState calibrationDryPageState("Calib. max. sequedad",
                                             MAX_CALIB_DRYNESS_HUM_CHAR_UUID);
SelectOutputPageState selectOutputPageState;
EnableHumidityCheckPageState enableHumidityCheckPageState;

short offsetMenuOption = 0;
short calibMenuOption = 1;
short runMenuOption = 2;

IrrigationCalibMenuState irrigationOffsetCalibMenuState(&offsetMenuOption);
IrrigationCalibMenuState irrigationCalibMenuState(&calibMenuOption);
IrrigationCalibMenuState runIrrigationMenuState(&runMenuOption);

SetTimePageState setTimePageState;

PageState *currentPageState = &mainPageState;

// Declaración de la variable global externa que contiene la página que se está
// visualizando. Para la gestión de las páginas se ha implementado el patrón de
// programación State.
extern PageState *currentPageState;

Preferences preferences;
int humidityThreshold = 50;
int humidityValue = 0;
bool humidityCheckEnabled;

// Función de Setup
void setup() {
  Serial.begin(921600);
  Serial.println("Módulo IoT");

  preferences.begin("my-app", false); // inicializar preferences
  humidityThreshold = preferences.getInt(HUMIDITY_THRESHOLD_STR, 50);
  humidityCheckEnabled = preferences.getBool(CHECK_HUMIDITY_ENABLED, false);

  Serial.print("humidityThreshold:");
  Serial.println(humidityThreshold);

  setupDisplay();
  displayInitialMessage("Iniciando...");

  // Setup de los botones de navegación
  pinMode(BTN_UP, INPUT);
  pinMode(BTN_DOWN, INPUT);
  pinMode(BTN_ENTER, INPUT);
  pinMode(BTN_ESC, INPUT);

  setupBleHumidity();
  setupBleIrrigation();
  setupMqtt();

  Serial.println("fin setup");
}

// Función de loop
void loop() {
  MonitoringButtons();
  // Lógica de delay para no usar la función delay() que bloquea la ejecución
  static long lastTime = 0;
  long now = millis();
  if (abs(now - lastTime) > DELAY) {

    lastTime = now; // Almacena la última vez que se ejecutó este "if".
    currentPageState->loopPageState();
  }

  loopMqtt(); // Ciclo de ejecución del MQTT
}

// Método que monitorea los botones y decide qué hacer en base al
// botón presionado.
void MonitoringButtons() {
  static long lastTime = 0;
  long now = millis();

  int btnDownStateBefore = LOW;
  int btnUpStateBefore = LOW;
  int btnEnterStateBefore = LOW;
  int btnEscStateBefore = LOW;

  if (abs(now - lastTime) > PRESS_BUTTON_DELAY) {
    lastTime = now;

    int btnDownStateNow = digitalRead(BTN_DOWN);
    int btnEnterStateNow = digitalRead(BTN_ENTER);
    int btnEscStateNow = digitalRead(BTN_ESC);
    int btnUpStateNow = digitalRead(BTN_UP);

    int buttonPressed = BTN_NONE;

    if (btnDownStateBefore == LOW && btnDownStateNow == HIGH) {
      buttonPressed = BTN_DOWN;
    } else if (btnUpStateBefore == LOW && btnUpStateNow == HIGH) {
      buttonPressed = BTN_UP;
    } else if (btnEnterStateBefore == LOW && btnEnterStateNow == HIGH) {
      buttonPressed = BTN_ENTER;
    } else if (btnEscStateBefore == LOW && btnEscStateNow == HIGH) {
      buttonPressed = BTN_ESC;
    }

    // Actualizar el estado anterior de los botones
    btnDownStateBefore = btnDownStateNow;
    btnUpStateBefore = btnUpStateNow;
    btnEnterStateBefore = btnEnterStateNow;
    btnEscStateBefore = btnEscStateNow;

    if (buttonPressed != BTN_NONE) {
      // La clase memorizada en currentPageState gestiona el botón presionado
      currentPageState->handleInput(buttonPressed);
      // y lo que se visualiza en la pantalla
      currentPageState->display();
    }
  }
}