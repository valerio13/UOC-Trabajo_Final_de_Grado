//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Projecte d'interacció tangible
//////////////////////////////////////////////

#include <Arduino.h>
#include <OledDisplay.h> //Included the file to drive the Oled display
#include <PageState.h>   //Included the file to manage the menu
#include <ble.h>
#include <config.h>

#define DELAY 500 // Define the delay time used in the loop method
#define PRESS_BUTTON_DELAY                                                     \
  500 // Define the delay time used in the read buttons loop

void MonitoringButtons();
void checkHumidity();

// Variables globales
MainPageState mainPageState;
MainMenuState mainMenuState;
MoistureMenuState moistureMenuState;
IrrigationMenuState irrigationMenuState;
SubMenuState subMenu1State("Submenú 1");
SubMenuState subMenu2State("Submenú 2");
SubMenuState subMenu3State("Submenú 3");

PageState *currentPageState = &mainPageState;

// Declaración de la variable global externa
extern PageState *currentPageState;

Preferences preferences;
int humidityThreshold = 50;
int humidityValue = 0;

// The Setup method
void setup() {
  Serial.begin(921600); // Setting the serial speed
  Serial.println("Módulo IoT");

  preferences.begin("my-app", false); // inicializar preferences
  humidityThreshold = preferences.getInt("humidityThreshold", 50);
  Serial.print("humidityThreshold:");
  Serial.println(humidityThreshold);

  setupDisplay();                        // Call the setup display method
  displayInitialMessage("Iniciando..."); // Call the display initial
                                         // message method
  pinMode(BTN_UP, INPUT);                // Setup input for left button
  pinMode(BTN_DOWN, INPUT);              // Setup input for right button
  pinMode(BTN_ENTER, INPUT);             // Setup input for right button
  pinMode(BTN_ESC, INPUT);               // Setup input for right button

  setupBle();

  Serial.println("fin setup");
}

// The loop method
void loop() {
  MonitoringButtons();      // Call the monitoring buttons method
  static long lastTime = 0; // Variable that stores the last time in miliseconds
                            // that the app enters in the "if" statement below.
  long now = millis();
  if (abs(now - lastTime) > DELAY) {

    lastTime = now; // Stores the last time that this "if" was executed.
    currentPageState->loopPageState();
  }
}

// Method that monitoring the buttons and desides what to do in base of the
// button pressed.
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
      currentPageState->handleInput(buttonPressed);
      currentPageState->display();
    }
  }
}