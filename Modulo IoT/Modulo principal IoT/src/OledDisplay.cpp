//////////////////////////////////////////////
///   Author: Valerio Colantonio
///   Gestión del display
//////////////////////////////////////////////

#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

Adafruit_SH1106 oled(OLED_SDA, OLED_SCL);

// Constantes de coordinadas del display
static uint16_t initialXPos = 0;
static uint16_t Yoffset = 13;
static uint16_t firstLineYoffset = 16;

// Método que configura la pantalla al inicio de Arduino.
void setupDisplay() {
  Wire.begin(); // Inicializar bus I2C
  oled.begin(SH1106_SWITCHCAPVCC, OLED_I2C_ADDRESS);
  oled.clearDisplay();
}

// Método que muestra el mensaje inicial.
void displayInitialMessage(String message) {
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0, 0);
  oled.setTextSize(1);
  oled.print(message);
  oled.display();
}

// Método que muestra mensajes genéricos.
// Recibe tres strings y las imprime en tres líneas del display distintas
void displayMessage(String line1, String line2, String line3) {
  oled.clearDisplay();
  oled.setTextColor(WHITE);

  oled.setCursor(initialXPos, 0);
  oled.setTextSize(1);
  oled.print(line1);

  oled.setCursor(initialXPos, firstLineYoffset);

  oled.setTextSize(1);
  oled.print(line2);

  oled.setCursor(initialXPos, firstLineYoffset + Yoffset * 2);

  oled.setTextSize(1);
  oled.print(line3);

  oled.display();
}

// Método que muestra los mensajes de error.
void displayErrorMessage(String line1, String line2, String line3) {
  oled.clearDisplay();
  oled.setTextColor(WHITE);

  oled.setCursor(initialXPos, 0);
  oled.setTextSize(1);
  oled.print(line1);

  oled.setCursor(initialXPos, 16);
  oled.setTextSize(1);
  oled.print(line2);

  oled.setCursor(initialXPos, 32);
  oled.setTextSize(1);
  oled.print(line3);

  // Las siguientes líneas son para dibujar una cara triste como emoji.
  int x = 108;
  int y = 45;
  oled.fillCircle(x, y, 12,
                  WHITE); // Dibuja el círculo de la cara triste. Es blanco.
  oled.drawCircle(x, y + 11, 8,
                  BLACK); // Dibuja el circulo de la cara triste. Es blanco.
  oled.drawCircle(x, y + 12, 8, BLACK);
  oled.fillCircle(x - 5, y - 3, 2, BLACK); // Dibuja el ojo izquierdo.
  oled.fillCircle(x + 5, y - 3, 2, BLACK); // Dibuja el ojo derecho.
  oled.drawCircle(x, y, 10, WHITE);

  oled.display();
}

// Método que muestra el menú principal.
void displayMenu(String title, String *pdata, byte length,
                 byte selectedOption) {
  oled.clearDisplay();

  for (int i = 0; i < length;
       i++) { // Bucle para mostrar las opciones del menú.
    if (selectedOption == i) {
      oled.fillRoundRect(0, firstLineYoffset - 1 + Yoffset * i, oled.width(), 9,
                         4, WHITE);
      oled.setTextColor(BLACK);
    } else {
      oled.setTextColor(WHITE);
    }

    oled.setCursor(initialXPos + 5, firstLineYoffset + Yoffset * i);
    oled.setTextSize(1);
    oled.print(pdata[i]);
  }

  oled.fillRect(0, 0, oled.width(), 12, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(2, 2);
  oled.setTextSize(1);
  oled.print(title);

  oled.display();
}

// Método que muestra la página del submenú.
void displaySubMenuStr(String subMenuTitle, String description, String value,
                       String leftOption, String rightOtion) {
  oled.clearDisplay();

  oled.fillRect(0, 0, oled.width(), 12, WHITE);
  oled.setTextColor(BLACK);
  oled.setCursor(2, 2);
  oled.setTextSize(1);
  oled.print(subMenuTitle);

  oled.setTextColor(WHITE);
  oled.setCursor(2, 16);
  oled.setTextSize(1);
  oled.print(description);

  oled.setCursor(2, 32);
  oled.setTextSize(1);
  oled.print(value);

  // Visualiza la barra inferior de color blanco con las dos opciones
  oled.setTextColor(BLACK);
  oled.fillRect(0, 55, oled.width(), 12, WHITE);
  oled.setCursor(5, 56);
  oled.setTextSize(1);
  oled.print(leftOption);

  oled.setCursor(90, 56);
  oled.setTextSize(1);
  oled.print(rightOtion);

  oled.display();
}

// Visualiza la barra inferior con las opciones de guardar y salir.
void displaySubMenuStr(String subMenuTitle, String description, String value) {
  displaySubMenuStr(subMenuTitle, description, value, "GUARDAR", "SALIR");
}