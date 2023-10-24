#include "dorian_math.h"

#include <FastLED.h>


//nummber of hues should be a multiple of 2
CRGB indexToColorPlus2(uint8_t index, uint8_t numOfHues) { //plus 2 being white and black // ie numOfHues = 16; white= 17 black = 18 so range(0,18)
  CHSV hsvColor;
  uint8_t invertedNumOfHues = 256 / numOfHues;
  uint8_t hue = invertedNumOfHues * index;
  CRGB rgbColor = CHSV(hue, 255, 255);

  if (index == numOfHues) {
    rgbColor = CHSV(hue, 0, 255);
  } else if (index == numOfHues + 1) {
    rgbColor = CHSV(hue, 0, 0);
  }
  return rgbColor;
}
void drawCircleBackground(TFT_eSPI & tft);

template < size_t N >
  void runButtonsUpdateScreen4(SharedData & sharedD, ModeSettingSimple * modeSettings, TFT_eSPI & tft, std::array < DButton, N > & keys) { // args  tft, keys
    /*
    buttons list
    0 setting <
    1 setting >
    2 powerOnOff
    3 brightnessAdjust
    8-11 value -- - + ++
    12 mode <
    13 mode >
    */

    bool updateColorAndText = false;
    static uint8_t valueToChange; //0-4 colors 5-6 settings

    tftElementDefiner def(tft); //def.setScreen(gTft, 240,320);
    tft.setFreeFont(LABEL1_FONT); //global

    tftRatioPositioner keyPositioner;
    tftStyler bigKeyStyler; // selected value

    //if we are redrawing the screen draw the background first
    //2-5unused
    //setting select buttons
    for (int b = 0; b < 2; b++) { ///if we press any of the selector buttons left room to go up to 8
      if (keys[b].justReleased()) {

        //   sharedD.redrawScreen = true;
      }
    }

    //mode buttons
    for (int b = 12; b < 14; b++) { ///if we press any of the selector buttons left room to go up to 8
      if (keys[b].justReleased()) {
        valueToChange = 0; // set setting to adust to the first one
        // valueToChange = b;
        sharedD.redrawScreen = true;
      }
    }

    if (sharedD.redrawScreen) {
      updateColorAndText = true;
      drawCircleBackground(tft);

    }

    uint8_t numRow = 5;
    uint8_t numCol = 4;
    uint8_t i = 0;

    //value changing BUTTONS | - - | - | + | ++ |///////////////////////////////////////////////////////////////
    tft.setFreeFont(LABEL1_FONT);

    bigKeyStyler.shape(6, -2, -14);
    bigKeyStyler.colors(tft.color565(200, 200, 200), tft.color565(0, 0, 0), tft.color565(0, 0, 0)); //fill,border,text
    keys[8].setLabel("- -");
    keys[9].setLabel("-");
    keys[10].setLabel("+");
    keys[11].setLabel("++");

    int tempIncrimentAmount = modeSettings[sharedD.currentMode].type[valueToChange]; //
    if (tempIncrimentAmount < 0) {
      tempIncrimentAmount *= -1;
      tempIncrimentAmount *= 2;
    }

    int incrimentsArray[4] = {
      -.1 * tempIncrimentAmount,
      -1,
      1,
      .1 * tempIncrimentAmount
    };
    if (incrimentsArray[3] < 1) {
      incrimentsArray[0] = -1;
      incrimentsArray[3] = 1;
    }

    for (int b = 8; b < 12; b++) {

      keyPositioner.position(numCol, b - 8, numRow, 4); //colums 0 through 3        
      def.createButton(keys[b], bigKeyStyler, keyPositioner);

      if (keys[b].justPressedDraw(true)) {
        updateColorAndText = true;

        if (modeSettings[sharedD.currentMode].type[valueToChange] == 32) {

          modeSettings[sharedD.currentMode].value[valueToChange] = ranger(modeSettings[sharedD.currentMode].value[valueToChange], 0, 34, incrimentsArray[b - 8], RangerType_MODULO);

        } else if (modeSettings[sharedD.currentMode].type[valueToChange] >= 1 || modeSettings[sharedD.currentMode].type[valueToChange] <= -1) {

          //nifty way to set the range of usable values below by using the type as teh range
          int8_t min = 0;
          int8_t max = modeSettings[sharedD.currentMode].type[valueToChange];
          if (modeSettings[sharedD.currentMode].type[valueToChange] <= -1) {
            min = modeSettings[sharedD.currentMode].type[valueToChange];
            max = modeSettings[sharedD.currentMode].type[valueToChange] * -1;
          }
          //sharedD.ModeSettingSimple[sharedD.currentMode].value[valueToChange] = ranger() ;
          modeSettings[sharedD.currentMode].value[valueToChange] = ranger(modeSettings[sharedD.currentMode].value[valueToChange], min, max, incrimentsArray[b - 8], RangerType_NOWRAP);

        } else if (modeSettings[sharedD.currentMode].type[valueToChange] == 0) {
          //do nothing? yes
        }
        //draw and text change should be here

      }
      if (keys[b].justReleased()) {
        keys[b].drawButton();
      }

      if (sharedD.redrawScreen) {
        keys[b].drawButton();
      }

    } //for
    //END value changing BUTTONS | - - | - | + | ++ |///////////////////////////////////////////////////////////////

    ///  power button /////////////////////////////////

    bigKeyStyler.shape(14, -14, -14); //for power and brightness

    if (keys[2].justPressed()) {
      sharedD.powerOnOff = !sharedD.powerOnOff;
    }

    if (sharedD.redrawScreen || keys[2].justReleased()) {
      if (sharedD.powerOnOff) {
        bigKeyStyler.colors(tft.color565(0, 200, 0), tft.color565(255, 255, 255), tft.color565(255, 255, 255)); //fill,border,text
        keys[2].setLabel("on");
      } else {
        bigKeyStyler.colors(tft.color565(200, 0, 0), tft.color565(0, 0, 0), tft.color565(0, 0, 0)); //fill,border,text
        keys[2].setLabel("off");
      }
      keyPositioner.position(numCol, 2, numRow, 0);
      def.createButton(keys[2], bigKeyStyler, keyPositioner);
      keys[2].drawButton();
    }

    // END POWER BUTTTON  ///////////////////////////////

    ///  BRIGHTNESS button /////////////////////////////////
    if (keys[3].justPressed()) {
      sharedD.brightnessLevel0to3++;
      sharedD.brightnessLevel0to3 %= 4;
    }

    if (sharedD.redrawScreen || keys[3].justReleased()) {
      char textToPrint[4];
      if (sharedD.brightnessLevel0to3 + 1 == 4) {
        sprintf(textToPrint, "%d%", ((sharedD.brightnessLevel0to3 + 1) * 100) / 4); //no percentage at 100
      } else {
        sprintf(textToPrint, "%d%%", ((sharedD.brightnessLevel0to3 + 1) * 100) / 4);
      }
      bigKeyStyler.colors(tft.color565(32, 0, 32), tft.color565(255, 255, 64), tft.color565(255, 255, 64)); //fill,border,text

      keys[3].setLabel(textToPrint);

      keyPositioner.position(numCol, 1, numRow, 0);
      def.createButton(keys[3], bigKeyStyler, keyPositioner);
      keys[3].drawButton();
    }

    // END BRIGHTNESS BUTTTON  ///////////////////////////////

    //SETTING SELCTOR BUTTONS///////////////////////////////////////////////////////////////

    keys[0].setLabel("<");
    keys[1].setLabel(">");

    for (int b = 0; b < 2; b++) {
      bigKeyStyler.colors(tft.color565(200, 200, 200), tft.color565(0, 0, 0), tft.color565(0, 0, 0)); //fill,border,text
      bigKeyStyler.shape(6, -2, -14);

      keyPositioner.position(numCol, 0, numRow, 2);
      def.createButton(keys[0], bigKeyStyler, keyPositioner);
      keyPositioner.position(numCol, 3, numRow, 2);
      def.createButton(keys[1], bigKeyStyler, keyPositioner);

      if (keys[b].justPressedDraw(true)) {
        updateColorAndText = true;

      }
      if (keys[b].justReleased()) {
        updateColorAndText = true;
        keys[b].drawButton();
        if (b == 0) {
          do { //cycle through if type is equal to 0
            valueToChange = valueToChange + NUMOFSETTINGS - 1;
            valueToChange %= NUMOFSETTINGS;
          } while (modeSettings[sharedD.currentMode].type[valueToChange] == 0);

        } else if (b == 1) {
          do {
            valueToChange++;
            valueToChange %= NUMOFSETTINGS;
          } while (modeSettings[sharedD.currentMode].type[valueToChange] == 0);
        }

      }

      if (sharedD.redrawScreen) {
        keys[b].drawButton();
      }

    } //for
    //END SETTING SELECTORS///////////////////////////////////////////////////////////////

    ///mode select buttons < >   //////////////////////////

    //keyStyler.colors(tft.color565(200,200,200),tft.color565(0, 0, 0) ,  tft.color565(0, 0, 0));  //fill,border,text
    bigKeyStyler.colors(tft.color565(200, 200, 200), tft.color565(0, 0, 0), tft.color565(0, 0, 0)); //fill,border,text
    bigKeyStyler.shape(6, -4, -4);
    keys[12].setLabel("<");
    keys[13].setLabel(">");

    for (int b = 12; b < 14; b++) {

      tft.setFreeFont(LABEL1_FONT);

      keyPositioner.position(numCol, (b - 12) * 3, numRow, 0); //colums 0 and 3        
      def.createButton(keys[b], bigKeyStyler, keyPositioner);

      if (keys[b].justPressedDraw(true)) {
        updateColorAndText = true;
        int upOrDownButton;
        if (b == 12) {
          
          sharedD.currentMode += NUMBEROFMODES - 1;
        } else if (b == 13) {
          sharedD.currentMode++;
        }

        sharedD.currentMode %= NUMBEROFMODES;
        //sharedD.currentMode = ranger((int16_t) sharedD.currentMode, 0, NUMBEROFMODES, upOrDownButton, RangerType_MODULO);


      }
      if (keys[b].justReleased()) {
        //Serial.println("DRAW --++");
        keys[b].drawButton();
      }

      if (sharedD.redrawScreen) {
        keys[b].drawButton();
      }

    } //for
    //end mode select buttons < > ////////////////////////////////////

    //update text and color///////////////////////////////////////////////////////////////////////////////////////
    // ALL TEXT BOXES BELOW
    if (updateColorAndText) {
      // Serial.println("update IN NUMBER OF MODES ");
      // Serial.println(NUMBEROFMODES);

      char textToPrint[40];
      char ending[10];
      tftRatioPositioner textBoxPositioner;
      tftStyler textBoxStyler;
      // tft.setFreeFont(LABEL1_FONT); //global

      if (modeSettings[sharedD.currentMode].type[valueToChange] == 32) {
        sharedD.rgb[valueToChange] = indexToColorPlus2(modeSettings[sharedD.currentMode].value[valueToChange], 32);
        printf(" value %d \n ", modeSettings[sharedD.currentMode].value[valueToChange]);
        if (modeSettings[sharedD.currentMode].value[valueToChange] == 33) { //black
          textBoxStyler.colors(tft.color565(sharedD.rgb[valueToChange].r, sharedD.rgb[valueToChange].g, sharedD.rgb[valueToChange].b),
            tft.color565(255, 255, 255), tft.color565(255, 255, 255));
        } else {
          textBoxStyler.colors(tft.color565(sharedD.rgb[valueToChange].r, sharedD.rgb[valueToChange].g, sharedD.rgb[valueToChange].b),
            tft.color565(0, 0, 0), tft.color565(0, 0, 0));
        }
      } else {
        textBoxStyler.colors(tft.color565(0, 0, 0), tft.color565(255, 255, 255), tft.color565(255, 255, 255));
      }
      textBoxStyler.shape(6, -10, -10);
      //      Serial.println("TEXT 1");

      //the value/number box
      //todo if 32 pallet. change background of create rectangle
      textBoxPositioner.position(numCol, 1, numRow, 3, 2, 1);
      // printf("current mode %d : value to change %d \n", sharedD.currentMode,valueToChange);
      sprintf(textToPrint, "%d", modeSettings[sharedD.currentMode].value[valueToChange]);
      //  strcpy(ending ," ind");
      def.createRectangle(0, textBoxStyler, textBoxPositioner);
      def.createTextMC(LABEL1_FONT, textToPrint, textBoxStyler, textBoxPositioner);
      //     Serial.println("TEXT 2");

      //mode text
      textBoxStyler.colors(tft.color565(0, 0, 0), tft.color565(255, 255, 255), tft.color565(255, 255, 255));

      memset(textToPrint, 0, 20);
      textBoxPositioner.position(1, 0, numRow, 1); //FULL WIDTH
      //sprintf goto _d(dictionary )and get striongs
      sprintf(textToPrint, "%s%s%s%s", _d[modeSettings[sharedD.currentMode].modeName[0]], _d[modeSettings[sharedD.currentMode].modeName[1]], _d[modeSettings[sharedD.currentMode].modeName[2]], _d[modeSettings[sharedD.currentMode].modeName[3]]);

      // this is where the timer stuff for the setting button

      def.createRectangle(0, textBoxStyler, textBoxPositioner);
      def.createTextMC(LABEL1_FONT, textToPrint, textBoxStyler, textBoxPositioner);

      //setting text
      memset(textToPrint, 0, 20);
      textBoxPositioner.position(numCol, 1, numRow, 2, 2, 1); //setting name
      //sprintf goto _d(dictionary )and get striongs
      sprintf(textToPrint, "%i. %s", valueToChange + 1, _d[modeSettings[sharedD.currentMode].typeName[valueToChange]]);

      // this is where the timer stuff for the setting button
      def.createRectangle(0, textBoxStyler, textBoxPositioner);
      def.createTextMC(LABEL2_FONT, textToPrint, textBoxStyler, textBoxPositioner);

      Serial.println("update END ");

    }

    //closing variables resect
    updateColorAndText = false;
    sharedD.redrawScreen = false;
    delay(10); // UI debouncing
  }

void drawCircleBackground(TFT_eSPI & tft) {

  tftElementDefiner def(tft); //def.setScreen(gTft, 240,320);

  tft.setFreeFont(LABEL1_FONT);

  tftRatioPositioner positioner;
  tftStyler styler;

  //draw background///////////////////////////////////////////////////////////////////////////////
  tft.fillScreen(tft.color565(0, 0, 48));
  styler.colors(tft.color565(0, 0, 0), tft.color565(64, 64, 0), TFT_BLACK);
  styler.shape(3, -4, -4);

  uint8_t rows = 10;
  uint8_t cols = 10;

  for (int x = 0; x < rows; x++) {
    for (int y = 0; y < cols; y++) {
      int radius = -2;
      for (int i = 0; i < 3; i++) { //border thickness
        radius++;
        styler.shape(3, radius, radius);

        def.createElipse(1, styler, rows, x, cols, y);
      }
    }
  }

  //end background///////////////////////////////////////////////////////////////////////////////
}