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
void manageRightButtonPressed();
void manageLeftButtonPressed();
void checkHumidity();

// Enum representing the state of the aplication
enum AppState { RUNNING, MENU, SUBMENU };

AppState appState =
    RUNNING; // Variable that stores the states of the aplication

// The Setup method
void setup() {
  Serial.begin(9600); // Setting the serial speed

  setupDisplay();                        // Call the setup display method
  displayInitialMessage("Iniciando..."); // Call the display initial
                                         // message method
  pinMode(BTN_LEFT, INPUT);              // Setup input for left button
  pinMode(BTN_RIGHT, INPUT);             // Setup input for right button

  setupBle();
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
      checkHumidity();
      break;      // Code necessary to don't execute the code below.
    case SUBMENU: // Code executed if the submenu opcion was choose.
      // When a submenu is selected or calibrating humidity or calibrating
      // dryness was selected.

      MenuOptions menuOption =
          getCurrentMenuOption(); // We need to know if the calibrating
                                  // humidity or calibrating dryness was
                                  // selected.
      if (menuOption ==
          HUMIDITY_MENU) // If the calibrating humidity was selected.
        displaySubMenu(menuOption,
                       100); // We display the maximum humidity
                             // value sended from the sensor.
      else if (menuOption ==
               IRRIGATION_MENU) // If the calibrating dryness was selected.
        displaySubMenu(menuOption,
                       11); // We display the maximum dryness
                            // value sended from the sensor.
      break;
    }
  }
}

// Method that monitoring the buttons and desides what to do in base of the
// button pressed.
void MonitoringButtons() {
  static long lastTime = 0; // Variable that stores the last time in miliseconds
                            // that the app enters in the "if" statement below.
  long now = millis();

  int btnRightStateBefore =
      LOW; // Stores the state before of the right button reading.
  int btnRightStateNow =
      LOW; // Stores the present state of the right button reading.
  int btnLeftStateBefore =
      LOW; // Stores the state before of the left button reading.
  int btnLeftStateNow =
      LOW; // Stores the present state of the left button reading.

  if (abs(now - lastTime) >
      PRESS_BUTTON_DELAY) { // This "if" is used instead of the "delay"
                            // function and it has the same functionality but
                            // the "delay" stops the execution of the program.
    lastTime = now;         // Stores the last time that this "if" was executed.

    btnRightStateBefore = btnRightStateNow; // Record the state of the right
                                            // button before the reading.
    btnRightStateNow =
        digitalRead(BTN_RIGHT); // Update the present state of the right button.

    btnLeftStateBefore = btnLeftStateNow; // Record the state of the left
                                          // button before the reading.
    btnLeftStateNow =
        digitalRead(BTN_LEFT); // Update the present state of the left button.

    bool rightButtonPressed =
        btnRightStateBefore == LOW &&
        btnRightStateNow ==
            HIGH; // Check if the right button signal has a rising edge
    bool leftButtonPressed =
        btnLeftStateBefore == LOW &&
        btnLeftStateNow ==
            HIGH; // Check if the left button signal has a rising edge

    MenuOptions menuOption =
        getCurrentMenuOption(); // Get the current menu option

    if (rightButtonPressed &&
        leftButtonPressed) {     // If both buttons are pressed.
      if (appState == RUNNING) { // And the present app state is running.
        appState = MENU;         // The app state changes to menu.
        displayMenu(getMenuOptionsStr(), getMenuLength(),
                    0); // And the main menu is displayed.
      }
    } else if (rightButtonPressed) { // If right button is pressed.
      manageRightButtonPressed();
    } else if (leftButtonPressed) {
      manageLeftButtonPressed();
    }
  }
}

void manageRightButtonPressed() {
  switch (appState) { // And the app state is...
  case RUNNING:       // running than...
    break;            // Command necessary to don't execute the code below.
  case MENU:          // instead if the main menu is displayed...
    nextOption();     // next option is selected.
    displayMenu(getMenuOptionsStr(), getMenuLength(),
                getSelectedOption()); // the main menu is updated to
                                      // highlight the selected option
    break;           // Command necessary to don't execute the code below.
  case SUBMENU:      // instead if a submenu is displayed...
    resetMenu();     // The last menu selection is reset.
    appState = MENU; // And the app returns to the main manu.
    break;           // Command necessary to don't execute the code below.
  }
}

void manageLeftButtonPressed() {
  // If left button is pressed.
  switch (appState) { // And the app state is...
  case RUNNING:       // running than...
    break;            // Command necessary to don't execute the code below.
  case MENU:          // instead if the main menu is displayed...
    if (menuOption == EXIT) { // and the exit option is selected...
      appState = RUNNING;     // the app state returns to running.
      resetMenu(); // and the display returns to the main page showing the
                   // sensor humidity value and thresold.
    } else {       // otherwise...
      appState = SUBMENU; // the app enters in a selected submenu.
    }
    break;      // Command necessary to don't execute the code below.
  case SUBMENU: // If the submenu was selected...
    if (menuOption ==
        HUMIDITY_MENU) { // And the menu option is the humidity calibration
                         // (the calibrate humidity page is displayed)
    } else if (menuOption ==
               IRRIGATION_MENU) { // But if the menu option is the dryness
                                  // calibration (the calibrate dryness page
                                  // is displayed)
    }
    displayMessage("CALIBRAR", "GUARDADO",
                   ""); // In the two cases above the message of calibration
                        // saved is displayed.
    delay(1000);        // The message is displayed for 1 second.
    break;              // Command necessary to don't execute the code below.
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