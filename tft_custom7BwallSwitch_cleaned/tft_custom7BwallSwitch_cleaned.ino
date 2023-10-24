#include "FS.h" //required for tft library

#include <SPI.h>
 //#include <TFT_eSPI.h>  // Hardware-specific library //moved to tft_doriansSimpleTFTLayout //note I use an edited version that adds rounded corners to buttons
#include <iostream>
 //#include "math.h"
#include "TFT_SETUP.h"
 //#include "TFT_dorianStuff.h" //depreciated changed to the one below
#include "TFT_doriansSimpleTFTLayout.h"
//#include "basicTimer.h"
#include <array>
 //#include "globals.h" //moved to modes.h
#include "modes.h"
 //#include "functioners.h" moved to modes.h

template < size_t N >
  void hasTouchScreenBeenTouched(std::array < DButton, N > & keyZ, TouchInfo & touchInfoZ) { //had a problem for a couple hours were the keys wern't being pressed because of a naming misMatch here

    // / Check if any key coordinate boxes contain the touch coordinates
    for (uint8_t b = 0; b < touchInfoZ.numOfKeys; b++) {
      if (touchInfoZ.pressed && keyZ[b].contains(touchInfoZ.t_x, touchInfoZ.t_y)) {
        keyZ[b].press(true); // tell the button it is pressed
      } else {
        keyZ[b].press(false); // tell the button it is NOT pressed
      }
    }

  }

TaskHandle_t Task0;
TaskHandle_t Task1;

SemaphoreHandle_t xBinarySemaphore;

//------------------------------------------------------------------------------------------

void setup() {
  // Use serial port

  Serial.begin(115200);

  pinMode(powerPin, OUTPUT);

  initializeModesData();

  xTaskCreatePinnedToCore(
    Task0code, /* Task function. */
    "Task0", /* name of task. */
    100000, /* Stack size of task */
    NULL, /* parameter of the task */
    1, /* priority of the task */ &
    Task0, /* Task handle to keep track of created task */
    0); /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1", /* name of task. */
    10000, /* Stack size of task */
    NULL, /* parameter of the task */
    1, /* priority of the task */ &
    Task1, /* Task handle to keep track of created task */
    1); /* pin task to core 1 */
  delay(500);

}

void Task0code(void * pvParameters) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());
  gTft.init();
  gTft.setRotation(0);

  touch_calibrate(gTft, false);

  drawCircleBackground(gTft);

  for (;;) {

    touchInfo.pressed = gTft.getTouch( & touchInfo.t_x, & touchInfo.t_y); //intersting you can def(*x){}; def(&x); to get the same as def(&x){} def(x);
    hasTouchScreenBeenTouched(key, touchInfo);

    if (touchInfo.pressed || touchInfo.wasPressed || sharedData.redrawScreen) {
      runButtonsUpdateScreen4(sharedData, modeSettings, gTft, key); // args  tft, keys

    }

    touchInfo.wasPressed = touchInfo.pressed;
  }
}

void Task1code(void * pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());
  delay(500);

  //these are the real led strips. used in my configuration. There is a "virtual" led strip that copies to the "real" led strips.
  CRGB ledsLeft[60 * 2 * 5];
  CRGB ledsRight[60 * 2 * 5];
  FastLED.addLeds < LED_TYPE, 26, COLOR_ORDER > (ledsLeft, 60 * 2 * 5);
  FastLED.addLeds < LED_TYPE, 14, COLOR_ORDER > (ledsRight, 60 * 2 * 5); //.setCorrection(TypicalLEDStrip);

  //initialize interfaces
  void( * interfaces[NUMBEROFMODES])(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) = {
    interFaceSolid, //0
    interfacePride, //1
    interfacMovingGradientWrap2Color, //2
    interfacMovingGradientWrapWithBlack2Color, //3
    interfacMovingGradientWrapWithBlack3Color, //4
    interfacTwinkle, //5
    interfacRainbowWrap

  };
  //initialize modes
  void( * modes[NUMBEROFMODES])(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) = {
    solid, //0
    pride, //1
    movingGradientWrap2color, //2
    movingGradientWrapWithBlack2Color, //3
    movingGradientWrapWithBlack3Color, //4
    twinkle1color, //5
    rainbowWrap

  };
  for (;;) {
    //modeSettingsFloats
    interfaces[sharedData.currentMode](modeSettings[sharedData.currentMode], modeSettingsFloats, pallet, NUMOFSETTINGS);
    //run selected animation.
    modes[sharedData.currentMode](pallet, modeSettingsFloats, leds, NUM_LEDS);

    //brightness code
    uint16_t maxBrightness = 255;
    uint8_t flippedBrightness = 4 - sharedData.brightnessLevel0to3;
    uint8_t oneHundred = (sharedData.brightnessLevel0to3 + 1) * 25;
    maxBrightness = (maxBrightness * oneHundred) / 100; // //20,40,60,80,100%

    digitalWrite(powerPin, sharedData.powerOnOff);
    if (!sharedData.powerOnOff) {
      maxBrightness = 0;
    }
    //end brightness code

    //reset array of leds// sometimes leds don't stay black
    for (uint16_t i = 0; i < 56; i++) {
      ledsRight[i] = CRGB::Black;
      ledsLeft[i] = CRGB::Black;
    }

    /*my strip layout   
    
                                  loop around ceiling// 2 strips are used to improve refesh rate.
       first 56 leds are not used|-----------|         
                 out->-----------|           |end 
     controller  out->-----------|           |end
                                 |-----------|

    */

    //code to copy from virtual leds to real leds. This is specific to my setup.
    int ceilingLedsToLight = (60 * 2 * 5) - 56;
    uint16_t x = (60 * 2 * 5);
    for (uint16_t i = 0; i < ceilingLedsToLight; i++) {
      x--;
      ledsLeft[i + 56] = leds[i];
      ledsRight[x] = leds[i + ceilingLedsToLight];

    }

    FastLED.show(maxBrightness);
  }
}

void loop() {
  vTaskDelete(NULL);
}