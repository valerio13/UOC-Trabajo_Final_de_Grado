//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Projecte d'interacció tangible
//////////////////////////////////////////////

#include "Menu.h"    //Include the menu.h to be able to use the enum MenuOptions
#include <Arduino.h> //Includes the Arduino's standard library

MenuOptions menuOption = HUMIDITY_MENU; // Define the selected menu option
                                        // when the app is started.

const byte menuSize = 3; // Variable that define the menu size.
static String menuOtionsStr[menuSize] = {
    "Calib. Max. Hum.", "Calib. Max. Sequedad",
    "Salir"}; // Define an array string with the menu option that is displayed
              // when menu is selected.

byte selectedOption = 0; // Variable that stores the selected menu option index.

// Method that reset the menu options.
void resetMenu() {
  selectedOption = 0; // Reset the selected menu option index.
  menuOption =
      (MenuOptions)selectedOption; // Stores the new menu option with index 0.
}

// Method that returns the menu options array.
String *getMenuOptionsStr() {
  return &menuOtionsStr[0]; // It returns the address of the first element. It
                            // is the way to pass the array using a method.
}

// Method that gets the menu length.
byte getMenuLength() {
  return menuSize; // It returns the size of the menu array.
}

// Method that select the next menu option.
void nextOption() {
  selectedOption++; // Increase the selected option index.
  if (selectedOption >=
      menuSize)         // If the index oversize the menu length it is reset.
    selectedOption = 0; // Reset the selected option index.

  menuOption = (MenuOptions)selectedOption; // Stores the selected menu option.
}

// Method that returns the index of the present selected option.
byte getSelectedOption() {
  return selectedOption; // Returns the stored present selected option index as
                         // byte.
}

// Method that returns the the present selected option.
MenuOptions getCurrentMenuOption() {
  return menuOption; // Returns the stored present selected option.
}
