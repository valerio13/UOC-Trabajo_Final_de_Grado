//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Gestión del display
//////////////////////////////////////////////

#ifndef OLEDDISPLAY_H_
#define OLEDDISPLAY_H_

#include <Arduino.h>

void setupDisplay();

void displayInitialMessage(String message);

void displayMessage(String line1, String line2, String line3);

void displayErrorMessage(String line1, String line2, String line3);

void displayMenu(String title, String *pdata, byte length, byte selectedOption);

void displaySubMenuStr(String subMenuTitle, String description, String value);

void displaySubMenuStr(String subMenuTitle, String description, String value,
                       String leftOption, String rightOtion);
#endif
