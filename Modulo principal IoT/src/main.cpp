#include <menu.h>
#include <menuIO/serialIn.h>
#include <menuIO/serialOut.h>

#include <Adafruit_GFX.h>     // libreria para pantallas graficas
#include <Adafruit_SSD1306.h> // libreria para controlador SSD1306
#include <Wire.h>             // libreria para bus I2C

#include <menuIO/adafruitGfxOut.h>

using namespace Menu;

// customizing a prompt look!
// by extending the prompt class
class altPrompt : public prompt {
public:
  altPrompt(constMEM promptShadow &p) : prompt(p) {}
  Used printTo(navRoot &root, bool sel, menuOut &out, idx_t idx, idx_t len,
               idx_t) override {
    return out.printRaw(F("special prompt!"), len);
    ;
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

// using the customized menu class
// note that first parameter is the class name
/*altMENU(confirmExit, subMenu, "Exit?", doNothing, noEvent, wrapStyle,
        (Menu::_menuData | Menu::_canNav), OP("Yes", systemExit, enterEvent),
        EXIT("Cancel"));

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle,
     OP("Op1", doNothing, noEvent), SUBMENU(subMenu));
*/
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

TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent,
       noStyle, //,doExit,enterEvent,noStyle,
       VALUE("On", HIGH, doNothing, noEvent),
       VALUE("Off", LOW, doNothing, noEvent));

MENU(subMenu, "Sub-Menu", doNothing, noEvent, noStyle,
     altOP(altPrompt, "", doNothing, noEvent), OP("Op", doNothing, noEvent),
     EXIT("<Back"));

result myLedOff() {
  ledCtrl = LOW;
  return proceed;
}

char *constMEM hexDigit MEMMODE = "0123456789ABCDEF";
char *constMEM hexNr[] MEMMODE = {"0", "x", hexDigit, hexDigit};
char buf1[] = "0x11";

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle,
     OP("Op1", doNothing, noEvent), OP("Op2", doNothing, noEvent),
     // ,FIELD(test,"Test","%",0,100,10,1,doNothing,noEvent,wrapStyle)
     SUBMENU(subMenu), SUBMENU(setLed), OP("LED On", myLedOn, enterEvent),
     OP("LED Off", myLedOff, enterEvent), SUBMENU(selMenu), SUBMENU(chooseMenu),
     //,OP("Alert test",doAlert,enterEvent),
     EDIT("Hex", buf1, hexNr, doNothing, noEvent, noStyle), EXIT("<Back"));

#define MAX_DEPTH 2

#define SCREEN_WIDTH 128 // reemplaza ocurrencia de SCREEN_WIDTH por 128
#define SCREEN_HEIGHT 64 // reemplaza ocurrencia de SCREEN_HEIGHT por 64

#define OLED_RESET 4 // necesario por la libreria pero no usado
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire,
                      OLED_RESET); // crea objeto

const colorDef<uint16_t> colors[] = {
    {{BLACK, WHITE}, {BLACK, WHITE, WHITE}}, // bgColor
    {{WHITE, BLACK}, {WHITE, BLACK, BLACK}}, // fgColor
    {{WHITE, BLACK}, {WHITE, BLACK, BLACK}}, // valColor
    {{WHITE, BLACK}, {WHITE, BLACK, BLACK}}, // unitColor
    {{WHITE, BLACK}, {BLACK, BLACK, BLACK}}, // cursorColor
    {{BLACK, WHITE}, {WHITE, BLACK, BLACK}}, // titleColor
};

#define gfxWidth 128
#define gfxHeight 64
#define fontX 12
#define fontY 16

// MENU_OUTPUTS(out, MAX_DEPTH, SERIAL_OUT(Serial),
//              ADAGFX_OUT(oled, colors, fontX, fontY, {0, 0, SCREEN_WIDTH /
//              fontX, SCREEN_HEIGHT / fontY - 18}));

MENU_OUTPUTS(out, MAX_DEPTH,
             ADAGFX_OUT(oled, colors, fontX, fontY,
                        {0, 0, gfxWidth / fontX, gfxHeight / fontY}),
             SERIAL_OUT(Serial));

serialIn serial(Serial);
NAVROOT(nav, mainMenu, MAX_DEPTH, serial, out);

bool running = true; // lock menu if false

result systemExit() {
  Serial.println();
  Serial.println("Terminating...");
  // do some termiination stuff here
  running = false; // prevents the menu from running again!
  Serial.println("done.");
  nav.idleOn(); // suspend the menu system
  return quit;
}

void setup() {
  Serial.begin(921600);
  while (!Serial)
    ;
  Serial.println("menu 4.x custom sub-menu prompt example");
  Serial.flush();

  Wire.begin(); // inicializa bus I2C
  oled.begin(SSD1306_SWITCHCAPVCC,
             0x3C);    // inicializa pantalla con direccion 0x3C
  oled.clearDisplay(); // limpia pantalla
}

void loop() {
  // if (running)
  //   nav.poll();
  // delay(100); // simulate a delay when other tasks are done

  nav.doInput();
  if (nav.changed(0)) { // only draw if changed
    nav.doOutput();
    oled.display();
  }

  delay(100); // simulate a delay when other tasks are done
}