//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Projecte d'interacció tangible
//////////////////////////////////////////////

// This file defines the methods and variables that are developed in the
// OledDisplay.cpp. Without this definitions the .ino file can not use it.
#ifndef OLEDDISPLAY_H_ // Preprocessor instruction, that it will be executed
                       // before compiling the application if the OledDisplay.h
                       // is not defined.
#define OLEDDISPLAY_H_ // It is defined just once.

#include "Menu.h" //Include the Menu files to use the MenuOptions.
#include <Arduino.h>

void setupDisplay(); // Method that setup the display at Arduino start.

void displayInitialMessage(
    String message); // Method that displays the initial message.

void displayMessage(String line1, String line2,
                    String line3); // Method that displays generic messages.

void displayErrorMessage(
    String line1, String line2,
    String line3); // Method that displays the error messages.

void displayMenu(String title, String *pdata, byte length,
                 byte selectedOption); // Method that displays the main menu.

// void displaySubMenu(MenuOptions menuOption, int humValeu);    //Method that
// displays the submenu page after the selection.

#endif // End of define instruction.
