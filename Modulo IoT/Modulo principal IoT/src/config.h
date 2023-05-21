/*config.h*/

/* List of Supported Fonts
 *
  Arial14,
  Arial_bold_14,
  Callibri11,
  Callibri11_bold,
  Callibri11_italic,
  Callibri15,
  Corsiva_12,
  fixed_bold10x15,
  font5x7,    //Do not use in LARGE_FONT, can use as default font
  font8x8,
  Iain5x7,    //Do not use in LARGE_FONT, can use as default font
  lcd5x7,     //Do not use in LARGE_FONT, can use as default font
  Stang5x7,   //Do not use in LARGE_FONT, can use as default font
  System5x7,  //Do not use in LARGE_FONT, can use as default font
  TimesNewRoman16,
  TimesNewRoman16_bold,
  TimesNewRoman16_italic,
  utf8font10x16,
  Verdana12,
  Verdana12_bold,
  Verdana12_italic,
  X11fixed7x14,
  X11fixed7x14B,
  ZevvPeep8x16
 *
 */

#define OLED_I2C_ADDRESS 0x3C // Defined OLED I2C Address
#define OLED_SDA 21
#define OLED_SCL 22

#define MAX_DEPTH 2

#define SCREEN_WIDTH 128 // reemplaza ocurrencia de SCREEN_WIDTH por 128
#define SCREEN_HEIGHT 64 // reemplaza ocurrencia de SCREEN_HEIGHT por 64

/*
 * Define your font from the list.
 * Default font: lcd5x7
 * Comment out the following for using the default font.
 */
#define LARGE_FONT Verdana12

// Navigate buttons
#define BTN_RIGHT 17 // Up Button
#define BTN_LEFT 16  // Down Button

// Comment the following to disable internal pullup for Navigate buttons
// #define NAV_BUTTONS_INPUT_PULLUP

#define TOTAL_NAV_BUTTONS 4 // Total Navigation Button used

/*Demonstrate PWM with LED on D11*/
#define LED_PIN 11 // Defined LED Pin to D11

#define MAX_DEPTH 2

#define fontX 12
#define fontY 16