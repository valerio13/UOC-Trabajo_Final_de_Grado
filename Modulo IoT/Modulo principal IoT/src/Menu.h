//////////////////////////////////////////////
///   Author: Valerio Colantonio
//////////////////////////////////////////////

// This file defines the methods and variables that are developed in the
// menu.cpp. Without this definitions the .ino file can not use it.
#ifndef MENU_H_ // Preprocessor instruction, that it will be executed before
                // compiling the application if the menu.h is not defined.
#define MENU_H_ // It is defined just once.

#include <Arduino.h>

void resetMenu(); // Method that reset the menu options.

enum MenuOptions // Enum that defines the menu options
{
  HUMIDITY_MAIN_MENU,
  IRRIGATION_MAIN_MENU,
  IOT_MAIN_MENU,
  EXIT_MAIN_MENU,
  CALIB_THRESHOLD_HUM,
  CALIB_MAX_HUMIDITY,
  CALIB_DRYNESS_HUMIDITY,
  EXIT_HUMIDITY,
  CALIB_IRRIGATION,
  EXECUTE_IRRIGATION,
  EXIT_IRRIGATION
};

extern MenuOptions
    menuOption; // Variable that stores the selected menu option. Is mark as
                // extern so it can be used in the other files.

String *getMenuOptionsStr(); // Method that returns the menu options array.

byte getMenuLength(); // Method that gets the menu length.

void nextOption(); // Method that select the next menu option.

byte getSelectedOption(); // Method that returns the index of the present
                          // selected option.

MenuOptions
getCurrentMenuOption(); // Method that returns the the present selected option.

byte getMenuLevel();

void displayRightOption();

void displayLeftOption();

#endif // End of define instruction.