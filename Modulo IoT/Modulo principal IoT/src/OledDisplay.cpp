//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Projecte d'interacció tangible
//////////////////////////////////////////////

#include <Adafruit_GFX.h> //Library for graphic displays.
#include <Wire.h>         //I2C bus library.

#include "Menu.h" //Include the Menu file.
#include <Adafruit_SH1106.h>
#include <config.h>

#define ANCHO 128 // Declare the width resolution.
#define ALTO 64   // Declare the heigh resolution.

#define OLED_RESET 4 // Needed by the Adafruit_SSD1306 library but not used.
Adafruit_SH1106 oled(OLED_SDA, OLED_SCL);

static uint16_t initialXPos = 0;       // Initial x position.
static uint16_t Yoffset = 13;          // Y offset, used in the lines display.
static uint16_t firstLineYoffset = 16; // First line Y offset.

// Method that setup the display at Arduino start.
void setupDisplay() {
  Wire.begin(); // Initialize I2C bus
  oled.begin(SH1106_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();
}

// Method that displays the initial message.
void displayInitialMessage(String message) {
  oled.clearDisplay(); // Clean the screen
  oled.setTextColor(
      WHITE); // Set the text color. White is the color of the screen in this
              // area (first two rows are yellow, the others blue).
  oled.setCursor(0, 0); // Locate the cursor at start of coordinates 0,0.
  oled.setTextSize(1);  // Set text size to 1.
  oled.print(message);  // Set the message that will be displayed on the screen.
  oled.display();       // Displays everything previously set on the screen.
}

// Method that displays generic messages.
void displayMessage(String line1, String line2, String line3) {
  oled.clearDisplay();      // Clean the screen.
  oled.setTextColor(WHITE); // Set the text color.

  oled.setCursor(initialXPos, 0); // Locate cursor at given coordinates.
  oled.setTextSize(1);            // Set text size to 2.
  oled.print(
      line1); // Set the text that will be displayed on the given screen line.

  oled.setCursor(
      initialXPos,
      firstLineYoffset); // Locate cursor at given coordinates. It is used the Y
                         // offset to calculate the Y line position.
  oled.setTextSize(1);   // Set text size to 2.
  oled.print(
      line2); // Set the text that will be displayed on the given screen line.

  oled.setCursor(initialXPos,
                 firstLineYoffset +
                     Yoffset *
                         2); // Locate cursor at given coordinates. It is used
                             // the Y offset to calculate the Y line position.
  oled.setTextSize(1);       // Set text size to 2.
  oled.print(
      line3); // Set the text that will be displayed on the given screen line.

  oled.display(); // Displays everything previously set on the screen.
}

// Method that displays the error messages.
void displayErrorMessage(String line1, String line2, String line3) {
  oled.clearDisplay();      // Clean the screen.
  oled.setTextColor(WHITE); // Set the text color.

  oled.setCursor(initialXPos, 0); // Locate cursor at given coordinates.
  oled.setTextSize(1);            // Set text size to 1.
  oled.print(
      line1); // Set the text that will be displayed on the given screen line.

  oled.setCursor(initialXPos, 8); // Locate cursor at given coordinates.
  oled.setTextSize(1);            // Set text size to 1.
  oled.print(
      line2); // Set the text that will be displayed on the given screen line.

  oled.setCursor(initialXPos, 24); // Locate cursor at given coordinates.
  oled.setTextSize(1);             // Set text size to 2.
  oled.print(
      line3); // Set the text that will be displayed on the given screen line.

  // The following lines are to draw a sad face as emoji.
  int x = 108; // Center X of face.
  int y = 45;  // Center Y of face.
  oled.fillCircle(x, y, 12,
                  WHITE); // Draw the circle of the sad face. It is white.
  oled.drawCircle(x, y + 11, 8, BLACK);    // Draw the sad face mouth.
  oled.drawCircle(x, y + 12, 8, BLACK);    // Draw the sad face mouth. It is
                                           // double.
  oled.fillCircle(x - 5, y - 3, 2, BLACK); // Draw the left eye.
  oled.fillCircle(x + 5, y - 3, 2, BLACK); // Draw the right eye.
  oled.drawCircle(x, y, 10,
                  WHITE); // Draw again a empy circle to close the first one.

  oled.display(); // Displays everything previously set on the screen.
}

// Method that displays the main menu.
void displayMenu(String *pdata, byte length, byte selectedOption) {
  oled.clearDisplay(); // Clean the screen.

  for (int i = 0; i < length; i++) { // Loop to display the menu options.
    if (selectedOption == i) {       // If the index is the selected option...
      oled.fillRoundRect(0, firstLineYoffset - 1 + Yoffset * i, oled.width(), 9,
                         4, WHITE); // Is draw a rounded filled white rectangle.
      oled.setTextColor(BLACK);     // ANd the text color is black.
    } else {
      oled.setTextColor(WHITE); // If the index is not the selected option, the
                                // text color is white.
    }

    oled.setCursor(initialXPos + 5,
                   firstLineYoffset +
                       Yoffset * i); // Locate cursor at given coordinates.
    oled.setTextSize(1);             // Set text size to 1.
    oled.print(pdata[i]); // Set the text from the string array that have the
                          // menu options text, that will be displayed on the
                          // given screen line.
  }

  oled.setTextColor(WHITE); // Set the text color.
  oled.setCursor(0, 0);     // Locate cursor at given coordinates.
  oled.setTextSize(1);      // Set text size to 2.
  oled.print("MENU");       // Set the text of the menu title.

  oled.fillRect(0, 55, oled.width(), 12,
                WHITE);     // Draw a white filled rectangle in the bottom side.
                            // Here will be displayed the button function text.
  oled.setTextColor(BLACK); // Set the text color to black.

  oled.setCursor(5, 56); // Locate cursor at given coordinates.
  oled.setTextSize(1);   // Set text size to 1.
  oled.print("SELEC.");  // Set the text for the left button function.

  oled.setCursor(70, 56);  // Locate cursor at given coordinates.
  oled.setTextSize(1);     // Set text size to 1.
  oled.print("SIGUIENTE"); // Set the text for the right button function.

  oled.display(); // Displays everything previously set on the screen.
}

// Method that displays the submenu page.
void displaySubMenuStr(String subMenuTitle, String description,
                       String humValeu) {
  oled.clearDisplay(); // limpia pantalla

  oled.setTextColor(WHITE); // Set the text color to white.
  oled.setCursor(0, 0);     // Locate cursor at given coordinates.
  oled.setTextSize(1);      // Set text size to 2.
  oled.print(subMenuTitle); // Set the page title.

  oled.setCursor(2, 16);   // Locate cursor at given coordinates.
  oled.setTextSize(1);     // Set text size to 2.
  oled.print(description); // Set the page description.

  oled.setCursor(2, 32); // Locate cursor at given coordinates.
  oled.setTextSize(1);   // Set text size to 2.
  oled.print(humValeu);  // Set the humidity value text.

  oled.setTextColor(BLACK); // Set the text color to black.
  oled.fillRect(0, 55, oled.width(), 12,
                WHITE);  // Draw a white filled rectangle in the bottom side.
                         // Here will be displayed the button function text.
  oled.setCursor(5, 56); // Locate cursor at given coordinates.
  oled.setTextSize(1);   // Set text size to 1.
  oled.print("GUARDAR"); // Set the text for the left button function.

  oled.setCursor(90, 56); // Locate cursor at given coordinates.
  oled.setTextSize(1);    // Set text size to 1.
  oled.print("SALIR");    // Set the text for the right button function.

  oled.display(); // Displays everything previously set on the screen.
}

// Method that displays the submenu page after the selection.
void displaySubMenu(MenuOptions menuOption, int humValeu) {
  switch (menuOption) { // Switch with the selected option.
  case HUMIDITY_MENU:   // If the option is humidity...
    displaySubMenuStr(
        "CALIBRAR", "Humedad",
        String(humValeu)); // It will displays the calibrate humidity page.
    break;                 // Command necessary to don't execute the code below.
  case IRRIGATION_MENU:    // If the option is dryness...
    displaySubMenuStr(
        "CALIBRAR", "Sequedad",
        String(humValeu)); // It will displays the calibrate dryness page.
    break;                 // Command necessary to don't execute the code below.
  }
}
