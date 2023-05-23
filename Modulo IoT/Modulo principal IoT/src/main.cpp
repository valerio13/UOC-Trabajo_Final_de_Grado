//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Projecte d'interacció tangible
//////////////////////////////////////////////

#include "Menu.h"        //Included the file to manage the menu
#include "OledDisplay.h" //Included the file to drive the Oled display
#include <Arduino.h>
#include <ble.h>
#include <config.h>

#define DELAY 500 // Define the delay time used in the loop method
#define HUM_DELAY 30000
#define PRESS_BUTTON_DELAY                                                     \
  500 // Define the delay time used in the read buttons loop

void MonitoringButtons();
void checkHumidity();

// Variables globales
MainMenuState mainMenuState;
MoistureMenuState moistureMenuState;
IrrigationMenuState irrigationMenuState;
SubMenuState subMenu1State("Submenú 1");
SubMenuState subMenu2State("Submenú 2");
SubMenuState subMenu3State("Submenú 3");

MenuState *currentMenuState = &mainMenuState;

// Declaración de la variable global externa
extern MenuState *currentMenuState;

// Enum representing the state of the aplication
enum AppState { RUNNING, MENU, SUBMENU };

AppState appState =
    RUNNING; // Variable that stores the states of the aplication

// The Setup method
void setup() {
  Serial.begin(921600); // Setting the serial speed
  Serial.println("Módulo IoT");
  
  setupDisplay();                        // Call the setup display method
  displayInitialMessage("Iniciando..."); // Call the display initial
                                         // message method
  pinMode(BTN_UP, INPUT);                // Setup input for left button
  pinMode(BTN_DOWN, INPUT);              // Setup input for right button
  pinMode(BTN_ENTER, INPUT);             // Setup input for right button
  pinMode(BTN_ESC, INPUT);               // Setup input for right button

  // setupBle();
}

// The loop method
void loop() {
  MonitoringButtons();      // Call the monitoring buttons method
  static long lastTime = 0; // Variable that stores the last time in miliseconds
                            // that the app enters in the "if" statement below.
  long now = millis();
  if (abs(now - lastTime) > DELAY) {
    lastTime = now; // Stores the last time that this "if" was executed.

    switch (appState) { // Switch atatement to check the aplication state.
    case RUNNING:       // Code executed if the appState is running.
      // checkHumidity();
      break;      // Code necessary to don't execute the code below.
    case SUBMENU: // Code executed if the submenu opcion was choose.
      // When a submenu is selected or calibrating humidity or calibrating
      // dryness was selected.

      // MenuOptions menuOption =
      //     getCurrentMenuOption(); // We need to know if the calibrating
      //                             // humidity or calibrating dryness was
      //                             // selected.
      // if (menuOption ==
      //     HUMIDITY_MAIN_MENU) // If the calibrating humidity was selected.
      //   displaySubMenu(menuOption,
      //                  100); // We display the maximum humidity
      //                        // value sended from the sensor.
      // else if (menuOption ==
      //          IRRIGATION_MAIN_MENU) // If the calibrating dryness was
      //          selected.
      //   displaySubMenu(menuOption,
      //                  11); // We display the maximum dryness
      //                       // value sended from the sensor.
      break;
    }
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

    bool downButtonPressed = btnDownStateBefore == LOW && btnDownStateNow == HIGH;
    bool upButtonPressed = btnUpStateBefore == LOW && btnUpStateNow == HIGH;
    bool enterButtonPressed = btnEnterStateBefore == LOW && btnEnterStateNow == HIGH;
    bool escButtonPressed = btnEscStateBefore == LOW && btnEscStateNow == HIGH;

    switch (appState)
    {
    case RUNNING:
      if (enterButtonPressed || escButtonPressed) {
        currentMenuState = &mainMenuState;
        currentMenuState->display();
        appState = MENU;
      }
      break;
    case MENU:
      if (downButtonPressed) {
        currentMenuState->handleInput(BTN_DOWN);
        currentMenuState->display();
      } else if (upButtonPressed) {
        currentMenuState->handleInput(BTN_UP);
        currentMenuState->display();
      } else if (enterButtonPressed) {
        currentMenuState->handleInput(BTN_ENTER);
        currentMenuState->display();
      } else if (escButtonPressed) {
        currentMenuState->handleInput(BTN_ESC);
        currentMenuState->display();
      }
      break;
    }

    btnDownStateBefore = btnDownStateNow;
    btnUpStateBefore = btnUpStateNow;
    btnEnterStateBefore = btnEnterStateNow;
    btnEscStateBefore = btnEscStateNow;
  }
}

void checkHumidity() {
  static long lastTime = 0;
  long now = millis();
  if (abs(now - lastTime) > HUM_DELAY) {
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