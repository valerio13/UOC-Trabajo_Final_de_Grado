// Ejemplo:
// https://github.com/neu-rah/ArduinoMenu/blob/master/examples/SSD1306Ascii_Button_Navigation/Button_Navigation/Button_Navigation.ino
// https://www.iotsharing.com/2017/05/how-to-use-arduino-esp32-to-display-oled.html

#include <Adafruit_GFX.h> // libreria para pantallas graficas
#include <Wire.h>         // libreria para bus I2C
#include <menu.h>
#include <menuIO/keyIn.h>
// #include <menuIO/serialIn.h>
// #include <menuIO/serialOut.h>
#include <Adafruit_SH1106.h>
#include <SPI.h>
#include <ble.h>

#include <menuIO/adafruitGfxOut.h>

using namespace Menu;
#include "config.h"

#define OLED_SDA 21
#define OLED_SCL 22

Adafruit_SH1106 oled(OLED_SDA, OLED_SCL);

// customizing a prompt look!
// by extending the prompt class
class altPrompt : public prompt {
public:
  altPrompt(constMEM promptShadow &p) : prompt(p) {}
  Used printTo(navRoot &root, bool sel, menuOut &out, idx_t idx, idx_t len,
               idx_t) override {
    return out.printRaw(F("special prompt!"), len);
  }
};

// customizing a menu prompt look
class confirmExit : public menu {
public:
  confirmExit(constMEM menuNodeShadow &shadow) : menu(shadow) {}
  Used printTo(navRoot &root, bool sel, menuOut &out, idx_t idx, idx_t len,
               idx_t p) override {
    return idx < 0 ? // idx will be -1 when printing a menu title or a valid
                     // index when printing as option
               menu::printTo(root, sel, out, idx, len, p)
                   : // when printing title
               out.printRaw((constText *)F("Exit"),
                            len); // when printing as regular option
  }
};

// this function is defined below because we need to refer
// to the navigation system (suspending the menu)
result systemExit();

int ledCtrl = LOW;
result myLedOn() {
  ledCtrl = HIGH;
  return proceed;
}

int selTest = 0;
SELECT(selTest, selMenu, "Select", doNothing, noEvent, noStyle,
       VALUE("Zero", 0, doNothing, noEvent),
       VALUE("One", 1, doNothing, noEvent),
       VALUE("Two", 2, doNothing, noEvent));

int chooseTest = -1;
CHOOSE(chooseTest, chooseMenu, "Choose", doNothing, noEvent, noStyle,
       VALUE("First", 1, doNothing, noEvent),
       VALUE("Second", 2, doNothing, noEvent),
       VALUE("Third", 3, doNothing, noEvent),
       VALUE("Last", -1, doNothing, noEvent));

// TOGGLE(ledCtrl, irrigationMenu, "Led: ", doNothing, noEvent,
//        noStyle, //,doExit,enterEvent,noStyle,
//        VALUE("On", HIGH, doNothing, noEvent),
//        VALUE("Off", LOW, doNothing, noEvent));

int sensorIndex = 0;

MENU(irrigationMenu, "Menu riego", doNothing, noEvent, noStyle,
     FIELD(sensorIndex, "Sensor ", "", 0, 10, 1, 1, doNothing, noEvent,
           wrapStyle),
     //  altOP(altPrompt, "", doNothing,
     //        noEvent),               // creo ejecuta la función de altPrompt
     //  OP("Op", doNothing, noEvent), // no hace nada
     EXIT("<Atras")); // vuelve atras

MENU(calibMinMenu, "Calibrar humedad minima", doNothing, noEvent, noStyle,
     altOP(altPrompt, "", doNothing,
           noEvent), // creo ejecuta la función de altPrompt
     //  SUBMENU(moistureMenu), OP("Op", doNothing, noEvent), // no hace nada
     EXIT("<Atras"));

MENU(calibMaxMenu, "Calibrar humedad maxima", doNothing, noEvent, noStyle,
     altOP(altPrompt, "", doNothing,
           noEvent), // creo ejecuta la función de altPrompt
     //  SUBMENU(moistureMenu), OP("Op", doNothing, noEvent), // no hace nada
     EXIT("<Atras"));

MENU(moistureMenu, "Menu humedad", doNothing, noEvent, noStyle,
     FIELD(sensorIndex, "Sensor ", "", 0, 10, 1, 1, doNothing, noEvent,
           wrapStyle),
     //  altOP(altPrompt, "", doNothing,
     //        noEvent), // creo ejecuta la función de altPrompt
     SUBMENU(calibMinMenu), SUBMENU(calibMaxMenu),
     //  OP("Op", doNothing, noEvent), // no hace nada
     EXIT("<Atras")); // vuelve atras

MENU(iotMenu, "Menu IoT", doNothing, noEvent, noStyle,
     altOP(altPrompt, "", doNothing,
           noEvent),               // creo ejecuta la función de altPrompt
     OP("Op", doNothing, noEvent), // no hace nada
     EXIT("<Atras"));              // vuelve atras

result myLedOff() {
  ledCtrl = LOW;
  return proceed;
}

int test = 55;

MENU(mainMenu, "Menu principal", doNothing, noEvent, wrapStyle,
     //  OP("Op1", doNothing, noEvent), // no hace nada
     //  OP("Op2", doNothing, noEvent), // no hace nada
     //  FIELD(test, "Test", "%", 0, 100, 10, 1, doNothing, noEvent, wrapStyle),
     // //pone un valor a test
     SUBMENU(moistureMenu),   // entra en el submenu "subMenu"
     SUBMENU(irrigationMenu), // cambia el valor de "ledCtrl"
     SUBMENU(iotMenu),        // cambia el valor de "ledCtrl"
     //  OP("LED On", myLedOn, enterEvent),   // ejecuta la función "ledCtrl"
     //  OP("LED Off", myLedOff, enterEvent), // ejecuta la función "myLedOff"
     //  SUBMENU(selMenu),                    // va cambiando los valores de
     //  selMenu SUBMENU(chooseMenu),                 // hace una selección
     EXIT("<Back"));

#define MAX_DEPTH 2

#define SCREEN_WIDTH 128 // reemplaza ocurrencia de SCREEN_WIDTH por 128
#define SCREEN_HEIGHT 64 // reemplaza ocurrencia de SCREEN_HEIGHT por 64

#define OLED_RESET 4 // necesario por la libreria pero no usado

const colorDef<uint16_t> colors[] = {
    {{BLACK, WHITE}, {BLACK, WHITE, WHITE}}, // bgColor
    {{WHITE, BLACK}, {WHITE, BLACK, BLACK}}, // fgColor
    {{WHITE, BLACK}, {WHITE, BLACK, BLACK}}, // valColor
    {{WHITE, BLACK}, {WHITE, BLACK, BLACK}}, // unitColor
    {{WHITE, BLACK}, {BLACK, BLACK, BLACK}}, // cursorColor
    {{BLACK, WHITE}, {WHITE, BLACK, BLACK}}, // titleColor
};

#define fontX 12
#define fontY 16

MENU_OUTPUTS(out, MAX_DEPTH,
             ADAGFX_OUT(oled, colors, fontX, fontY,
                        {0, 0, SCREEN_WIDTH / (fontX / 2),
                         SCREEN_HEIGHT / fontY}),
             //  SERIAL_OUT(Serial));
             NONE);

// build a map of keys to menu commands
keyMap joystickBtn_map[] = {
    {BTN_SEL, defaultNavCodes[enterCmd].ch},
    {BTN_UP, defaultNavCodes[upCmd].ch},
    {BTN_DOWN, defaultNavCodes[downCmd].ch},
    {BTN_ESC, defaultNavCodes[escCmd].ch},
};
keyIn<TOTAL_NAV_BUTTONS> joystickBtns(joystickBtn_map); // the input driver

NAVROOT(nav, mainMenu, MAX_DEPTH, joystickBtns, out);

bool running = true; // lock menu if false

result systemExit() {
  //   Serial.println();
  //   Serial.println("Terminating...");
  // do some termiination stuff here
  running = false; // prevents the menu from running again!
                   //   Serial.println("done.");
  nav.idleOn();    // suspend the menu system
  return quit;
}

void setup() {
  Serial.begin(921600);
  //   while (!Serial)
  //     ;
  Serial.println("menu 4.x custom sub-menu prompt example");
  //   Serial.flush();

  Wire.begin(); // inicializa bus I2C
  oled.begin(SH1106_SWITCHCAPVCC,
             0x3C);    // inicializa pantalla con direccion 0x3C
  oled.clearDisplay(); // limpia pantalla

  setupBle();
}

void loop() {
  nav.doInput();
  if (nav.changed(0)) { // only draw if changed
    nav.doOutput();
    oled.display();
  }

  loopBle();

  delay(100); // simulate a delay when other tasks are done
}