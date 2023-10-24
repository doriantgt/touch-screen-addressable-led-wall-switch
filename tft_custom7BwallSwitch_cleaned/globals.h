#include "ledDictionary.h"

#include "sunrise.h"

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT & FreeSansBold12pt7b // Key label font 1
#define LABEL2_FONT & FreeSansBold9pt7b // Key label font 2
//const GFXfont * LABEL1_FONT = &FreeSansBold12pt7b;  // Key label font 1
//const GFXfont * LABEL2_FONT = &FreeSansBold24pt7b;  // Key label font 2
const GFXfont * xxxxx = LABEL1_FONT;

bool colorHasChanged = false;

#define NUMOFKEYS 20

using namespace std;

class TouchInfo {
  public: uint16_t t_x;
  uint16_t t_y;
  uint8_t pressed; //touchinfo returns a press value
  uint8_t wasPressed;

  uint16_t numOfKeys;
  //pointer to function
  TouchInfo(uint8_t numKeys) {
    numOfKeys = numKeys;
  }
};
TouchInfo touchInfo(NUMOFKEYS);

#include "FastLED.h"

#define DATA_PIN 25
//#define CLK_PIN   4
#define LED_TYPE WS2812
#define COLOR_ORDER RGB
#define NUM_LEDS (60 * 4 * 5) - (56 * 2) //(60*4*5)-56  // 60*5  desk
uint8_t powerPin = 32;

CRGB leds[NUM_LEDS];

#define NUMOFSETTINGS 6
class ModeSettingSimple {
  //all settings really only need a range from -100 to 100;
  //the esp32 has plenty of memory to hold 30 modes in memeory
  //having any setting from -100 to 100 is fine. 
  public: uint8_t modeName[4]; // _i::entrys
  int8_t value[NUMOFSETTINGS];
  int8_t type[NUMOFSETTINGS]; //32 = 32 colorPal :  40 pal32 plus 8pastel :  64 pal64. all other ranges are just the number. ie -1 = -1 to 1; 10 = 0-10; 1 just being on and off;
  uint8_t typeName[NUMOFSETTINGS]; //_i entry

  //_i::entry
  void setModeName(uint8_t zero, uint8_t one = 0, uint8_t two = 0, uint8_t three = 0) {
    modeName[0] = zero;
    modeName[1] = one;
    modeName[2] = two;
    modeName[3] = three;
  }
  void setDefaultValue(int8_t zero = 0, int8_t one = 0, int8_t two = 0, int8_t three = 0, int8_t four = 0, int8_t five = 0) {
    value[0] = zero;
    value[1] = one;
    value[2] = two;
    value[3] = three;
    value[4] = four;
    value[5] = five;
  }
  void setModeSetting(uint8_t settingNum, int8_t defaultValue, int8_t typE, uint8_t typeNamE) {
    value[settingNum] = defaultValue;
    type[settingNum] = typE;
    typeName[settingNum] = typeNamE;
  }

};

#define NUMBEROFMODES 7

ModeSettingSimple modeSettings[NUMBEROFMODES];
float modeSettingsFloats[NUMOFSETTINGS];
CRGB pallet[4];

void initializeModesData() {

  uint8_t X = 0;
  modeSettings[X].setModeName(_i::Solid, _i::Color, 0);
  modeSettings[X].setModeSetting(0, 0, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(1, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(2, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(3, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(4, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(5, 0, _type::NOSETTING, _i::NOSETTING);

  X = 1;
  modeSettings[X].setModeName(_i::Pride, 0, 0);
  modeSettings[X].setModeSetting(0, 50, _type::zeroTo100, _i::Speed);
  modeSettings[X].setModeSetting(1, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(2, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(3, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(4, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(5, 0, _type::NOSETTING, _i::NOSETTING);

  X = 2;
  modeSettings[X].setModeName(0, _i::Gradient, 0);
  modeSettings[X].setModeSetting(0, 12, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(1, 25, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(2, -30, _type::neg100to100, _i::Speed);
  modeSettings[X].setModeSetting(3, 15, _type::zeroTo100, _i::Strech);
  modeSettings[X].setModeSetting(4, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(5, 0, _type::NOSETTING, _i::NOSETTING);

  X = 3;
  modeSettings[X].setModeName(_i::Grad, _i::w, _i::Blk);
  modeSettings[X].setModeSetting(0, 20, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(1, 25, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(2, -30, _type::neg100to100, _i::Speed);
  modeSettings[X].setModeSetting(3, 15, _type::zeroTo100, _i::Strech);
  modeSettings[X].setModeSetting(4, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(5, 0, _type::NOSETTING, _i::NOSETTING);

  X = 4;
  modeSettings[X].setModeName(_i::Grad, _i::w, _i::Blk, _i::n3);
  modeSettings[X].setModeSetting(0, 20, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(1, 24, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(2, 28, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(3, -30, _type::neg100to100, _i::Speed);
  modeSettings[X].setModeSetting(4, 15, _type::zeroTo100, _i::Strech);
  modeSettings[X].setModeSetting(5, 0, _type::NOSETTING, _i::NOSETTING);

  X = 5;
  modeSettings[X].setModeName(_i::Twinkle);
  modeSettings[X].setModeSetting(0, 12, _type::pal32, _i::Color);
  modeSettings[X].setModeSetting(1, 25, _type::zeroTo100, _i::Fade_Amnt);
  modeSettings[X].setModeSetting(2, 30, _type::zeroTo100, _i::Twk_Amnt);
  modeSettings[X].setModeSetting(3, 0, _type::zeroTo100, _i::Delay);
  modeSettings[X].setModeSetting(4, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(5, 0, _type::NOSETTING, _i::NOSETTING);

  X = 6;
  modeSettings[X].setModeName(_i::Rainbow);
  modeSettings[X].setModeSetting(0, 12, _type::neg100to100, _i::Speed);
  modeSettings[X].setModeSetting(1, 25, _type::zeroTo100, _i::Strech);
  modeSettings[X].setModeSetting(2, 30, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(3, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(4, 0, _type::NOSETTING, _i::NOSETTING);
  modeSettings[X].setModeSetting(5, 0, _type::NOSETTING, _i::NOSETTING);
  /*
         X=7;
        modeSettings[X].setModeName(_i::fire);
        modeSettings[X].setModeSetting(0,1, _type::pal32,    _i::Color);
        modeSettings[X].setModeSetting(1,3,  _type::pal32,    _i::Color);
        modeSettings[X].setModeSetting(2,31, _type::pal32,    _i::Color);
        modeSettings[X].setModeSetting(3,28,   _type::pal32,    _i::Color);
        modeSettings[X].setModeSetting(4,0 , _type::zeroTo100, _i::cooling);
        modeSettings[X].setModeSetting(5,0 , _type::zeroTo100, _i::heat);

        */
  /*
     X=99;
     modeSettings[X].setModeName(_i::Rainbow,0,0,0);
     modeSettings[X].setModeSetting(0,0 , _type::NOSETTING, _i::NOSETTING);
     modeSettings[X].setModeSetting(1,20, _type::NOSETTING, _i::NOSETTING);
     modeSettings[X].setModeSetting(2,0 , _type::NOSETTING, _i::NOSETTING);
     modeSettings[X].setModeSetting(3,32, _type::NOSETTING, _i::NOSETTING);
     modeSettings[X].setModeSetting(4,0 , _type::NOSETTING, _i::NOSETTING);
     modeSettings[X].setModeSetting(5,0 , _type::NOSETTING, _i::NOSETTING);
     */

}

struct SharedData { //program specific structure

  uint8_t currentMode = 2;
  uint16_t currentTime;
  uint16_t previousTime;

  CRGB temp; // four colors plus temp
  CRGB rgb[6]; // = {CRGB(255,0,0),CRGB(0,255,0),CRGB(0,0,255),CRGB(255,255,255)}; // four colors plus temp
  CHSV hsv[6]; // = {CRGB(255,0,0),CRGB(0,255,0),CRGB(0,0,255),CRGB(255,255,255)}; // four colors plus temp

  uint8_t currentScreen;
  uint8_t screenHistory[4];
  uint8_t lastScreen; //for updating screen
  bool redrawScreen = true; //
  bool powerOnOff = false;
  uint8_t brightnessLevel0to3 = 0; //25,50,75,100%
};
SharedData sharedData;

char x[9] = {
  char(0xf1)
}; //dafuq is this for?
array < DButton, NUMOFKEYS > key {
  {
    DButton(x), DButton("ff"), DButton("/|\ \|/")
  }
};

////////move to library
//#define ILI9341_DRIVER   in user setup. also pins are defined here
TFT_eSPI gTft = TFT_eSPI(); // Invoke custom library normally TFT_ESPI takes 2 arguments but you change sreen size in user setup