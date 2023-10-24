#ifndef modes_h
#define modes_h

#include "globals.h"

#include "screenLayout.h"

void exampleFunctionInterfaceFunction(ModeSettingSimple * screenSetting, float * modeSettingFloat) {

  // do some math to convert int8 to proper value of float

}
void interfaceStaightCopy(ModeSettingSimple modeSettings, float modeSettingFloat[NUMOFSETTINGS], CRGB * pallet, uint8_t numOfSettings) {
  for (int i = 0; i < numOfSettings; i++) {
    modeSettingFloat[i] = modeSettings.value[i];
  }
}

void interFaceSolid(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  pallet[0] = indexToColorPlus2(modeSettings.value[0], 32);
}
void solid(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {

  for (int i = 0; i < numOfLeds; i++) {
    inLeds[i] = pallet[0];
  }

}

void interfacePride(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  modeSettingFloat[0] = (float) modeSettings.value[0] / 50;
}
void pride(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  float millisAdjust = modeSettings[0]; //edit
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16; //gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis() * millisAdjust; //edit
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t) b16 * (uint32_t) b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t) bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV(hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend(leds[pixelnumber], newcolor, 64);
  }
}

void interfaceGradient(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  pallet[0] = indexToColorPlus2(modeSettings.value[0], 32);
  pallet[1] = indexToColorPlus2(modeSettings.value[1], 32);
}
void gradient(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  //static uint16_t currentTime;
  //static uint16_t previousTime;

  uint32_t ratio = 16777215 / numOfLeds; //0xff ff ff <<16 //bigger ratio value to prevent rounding errors

  for (uint16_t i; i < numOfLeds; i++) {
    uint32_t blender = i * ratio;
    blender = blender >> 16; //ratio back down
    inLeds[i] = blend(pallet[0], pallet[1], blender);
  }

}

void interfaceGradientWrap(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  pallet[0] = indexToColorPlus2(modeSettings.value[0], 32);
  pallet[1] = indexToColorPlus2(modeSettings.value[1], 32);
}
void gradientWrap(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  //static uint16_t currentTime;
  //static uint16_t previousTime;

  uint32_t ratio = 16777215 / numOfLeds; //0xff ff ff //bigger ratio value to prevent rounding errors

  for (uint16_t i; i < numOfLeds; i++) {
    uint32_t blender = i * ratio;
    blender = blender >> 15; //NOT 16
    if (blender > 255) {
      inLeds[i] = blend(pallet[0], pallet[1], (blender)); //+elapsedTime256));

    } else {
      inLeds[i] = blend(pallet[1], pallet[0], (blender)); //+elapsedTime256));

    }
  }

}

void interfacMovingGradientWrap2Color(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  pallet[0] = indexToColorPlus2(modeSettings.value[0], 32);
  pallet[1] = indexToColorPlus2(modeSettings.value[1], 32);
  int16_t milliSecondsMax = 10100; //100,00
  if (modeSettings.value[2] > 0) {
    milliSecondsMax = milliSecondsMax - modeSettings.value[2] * 100; //flip modes settings to rpm for function
  } else if (modeSettings.value[2] < 0) {
    milliSecondsMax *= -1;
    milliSecondsMax = milliSecondsMax - modeSettings.value[2] * 100;
  } else {
    milliSecondsMax = 0;
  }
  modeSettingFloat[0] = milliSecondsMax; // duration

  modeSettingFloat[1] = (float) modeSettings.value[3] / 5; //stresh
  if (modeSettingFloat[1] == 0) {
    modeSettingFloat[1] = .05;
  }

}
void movingGradientWrap2color(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  uint32_t duration;
  bool isNegativeMotion = false;
  if (modeSettings[0] < 0) {
    isNegativeMotion = true;
    duration = -modeSettings[0]; ///modeSettings[1];///modeSettings[1] ; //ms per revolution
  } else {
    duration = modeSettings[0]; ///modeSettings[1]; 
  }
  static uint16_t currentTime;
  static uint16_t previousTime;
  currentTime = millis();

  uint32_t elapsedTime = uint16_t(currentTime - previousTime);

  if (elapsedTime >= duration) {
    elapsedTime = elapsedTime - duration; //if skipped overZero
    previousTime = currentTime; //star duration over
  }
  elapsedTime = elapsedTime << 9; //8+1 to double the rotaion time so it goes around correctly
  //elapsedTime*=2;//done above with 9 instead of 8// 2 domains

  uint32_t elapsedTime512 = elapsedTime / duration;
  if (duration == 0) {
    elapsedTime512 = 0; //no rotation
  }

  uint32_t bitsBigger24 = 0xffffff; //this is used to make the ratio a bigger value to reduce rounding errors
  bitsBigger24 *= modeSettings[1]; //Strech or shrink float 
  uint32_t ratio = bitsBigger24 / numOfLeds; //0xff ff ff //bigger ratio value to prevent rounding errors

  uint16_t flipperRange = 0;
  int16_t flipper = 1;
  if (isNegativeMotion) {
    flipperRange = numOfLeds - 1;
    flipper = -1;
  }

  uint16_t x;
  for (int16_t i; i < numOfLeds; i++) {
    x = flipperRange + (i * flipper);

    uint32_t blender = i * ratio;
    //blender*=2;//2 domains done below with 15 instead of 16
    blender = blender >> 15;
    blender = blender + elapsedTime512;
    //printf("blender = %d  :: elapsedTime256 = %d  \n", blender , elapsedTime256 );
    blender %= 512; //we have 2 gradients so we need 256*2 ranges
    if (blender > 255) {
      inLeds[x] = blend(pallet[0], pallet[1], (blender)); //+elapsedTime256));
      // inLeds[i] = blend(pallet[0],pallet[1],(blender+elapsedTime256));

    } else {
      inLeds[x] = blend(pallet[1], pallet[0], (blender)); //+elapsedTime256));
      //   inLeds[i] = blend(pallet[1],pallet[0],(blender+elapsedTime256));

    }
  }

}

void interfacMovingGradientWrapWithBlack2Color(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  pallet[0] = indexToColorPlus2(modeSettings.value[0], 32);
  pallet[1] = indexToColorPlus2(modeSettings.value[1], 32);
  int16_t milliSecondsMax = 10100; //100,00
  if (modeSettings.value[2] > 0) {
    milliSecondsMax = milliSecondsMax - modeSettings.value[2] * 100; //flip modes settings to rpm for function
  } else if (modeSettings.value[2] < 0) {
    milliSecondsMax *= -1;
    milliSecondsMax = milliSecondsMax - modeSettings.value[2] * 100;
  } else {
    milliSecondsMax = 0;
  }
  modeSettingFloat[0] = milliSecondsMax; // duration

  modeSettingFloat[1] = (float) modeSettings.value[3] / 5; //stresh
  if (modeSettingFloat[1] == 0) {
    modeSettingFloat[1] = .05;
  }

}
void movingGradientWrapWithBlack2Color(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  uint32_t duration;
  bool isNegativeMotion = false;
  if (modeSettings[0] < 0) {
    isNegativeMotion = true;
    duration = -modeSettings[0]; ///modeSettings[1];///modeSettings[1] ; //ms per revolution
  } else {
    duration = modeSettings[0]; ///modeSettings[1]; 
  }
  static uint16_t currentTime;
  static uint16_t previousTime;
  currentTime = millis();

  uint32_t elapsedTime = uint16_t(currentTime - previousTime);

  if (elapsedTime >= duration) {
    elapsedTime = elapsedTime - duration; //if skipped overZero
    previousTime = currentTime; //star duration over
  }

  // elapsedTime = elapsedTime<<16; //may need long for elapsed
  elapsedTime = elapsedTime << 10; //8+2 to quaduple the rotaion time so it goes around correctly
  //elapsedTime*=4; done above with 10 instead of 8// 4 domains

  uint16_t elapsedTime1024 = elapsedTime / duration;

  if (duration == 0) {
    elapsedTime1024 = 0; //no rotation
  }

  uint32_t bitsBigger24 = 0xffffff; //this is used to make the ratio a bigger value to reduce rounding errors
  bitsBigger24 *= modeSettings[1]; //Strech or shrink float 
  uint32_t ratio = bitsBigger24 / numOfLeds; //0xff ff ff //bigger ratio value to prevent rounding errors

  uint16_t flipperRange = 0;
  int16_t flipper = 1;
  if (isNegativeMotion) {
    flipperRange = numOfLeds - 1;
    flipper = -1;
  }

  uint16_t x;
  for (int16_t i; i < numOfLeds; i++) {
    x = flipperRange + (i * flipper);

    uint32_t blender = i * ratio;
    //blender*=4;// 4 domains done below 14 instead of 16
    blender = blender >> 14; //NOT 16 to cancel out bitshift <<10

    blender = blender + elapsedTime1024;
    //printf("blender = %d  :: elapsedTime256 = %d  \n", blender , elapsedTime256 );
    blender %= 1024; //we have 4 gradients so we need 256*4 ranges
    CRGB black = CRGB(0, 0, 0);
    if (blender > 767) {
      inLeds[x] = blend(black, pallet[0], (blender));

    } else if (blender > 511) {
      inLeds[x] = blend(pallet[1], black, (blender));

    } else if (blender > 255) {
      inLeds[x] = blend(black, pallet[1], (blender));

    } else {
      inLeds[x] = blend(pallet[0], black, (blender));

    }
  }
}

void interfacMovingGradientWrapWithBlack3Color(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  pallet[0] = indexToColorPlus2(modeSettings.value[0], 32);
  pallet[1] = indexToColorPlus2(modeSettings.value[1], 32);
  pallet[2] = indexToColorPlus2(modeSettings.value[2], 32);

  int16_t milliSecondsMax = 10100; //100,00
  if (modeSettings.value[3] > 0) {
    milliSecondsMax = milliSecondsMax - modeSettings.value[3] * 100; //flip modes settings to rpm for function
  } else if (modeSettings.value[3] < 0) {
    milliSecondsMax *= -1;
    milliSecondsMax = milliSecondsMax - modeSettings.value[3] * 100;
  } else {
    milliSecondsMax = 0;
  }
  modeSettingFloat[0] = milliSecondsMax; // duration

  modeSettingFloat[1] = (float) modeSettings.value[4] / 5; //stresh
  if (modeSettingFloat[1] == 0) {
    modeSettingFloat[1] = .05;
  }
}
void movingGradientWrapWithBlack3Color(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  uint32_t duration;
  bool isNegativeMotion = false;
  if (modeSettings[0] < 0) {
    isNegativeMotion = true;
    duration = -modeSettings[0]; ///modeSettings[1];///modeSettings[1] ; //ms per revolution
  } else {
    duration = modeSettings[0]; ///modeSettings[1]; 
  }
  static uint16_t currentTime;
  static uint16_t previousTime;
  currentTime = millis();

  uint32_t elapsedTime = uint16_t(currentTime - previousTime);

  if (elapsedTime >= duration) {
    elapsedTime = elapsedTime - duration; //if skipped overZero
    previousTime = currentTime; //star duration over
  }

  // elapsedTime = elapsedTime<<16; //may need long for elapsed
  elapsedTime = elapsedTime << 8; //*256 to get elsped time into fract8
  elapsedTime *= 6; // //6 blend domains

  uint16_t elapsedTime1536 = elapsedTime / duration;

  if (duration == 0) {
    elapsedTime1536 = 0; //no rotation
  }

  uint32_t bitsBigger24 = 0xffffff; //this is used to make the ratio a bigger value to reduce rounding errors
  bitsBigger24 *= modeSettings[1]; //Strech or shrink  
  uint32_t ratio = bitsBigger24 / numOfLeds; //0xff ff ff //bigger ratio value to prevent rounding errors

  uint16_t flipperRange = 0;
  int16_t flipper = 1;
  if (isNegativeMotion) {
    flipperRange = numOfLeds - 1;
    flipper = -1;
  }

  uint16_t x;
  for (int16_t i; i < numOfLeds; i++) {
    x = flipperRange + (i * flipper);

    uint64_t blender = i * ratio;
    blender *= 6; //6 blend domains
    blender = blender >> 16; // 16 to cancel out bitsbigger24 with 8 bits remaining

    blender = blender + elapsedTime1536;
    //printf("blender = %d  :: elapsedTime256 = %d  \n", blender , elapsedTime256 );
    blender %= 1536; //we have 6 gradients so we need 256*6 domains
    CRGB black = CRGB(0, 0, 0);
    if (blender > 1279) {
      inLeds[x] = blend(black, pallet[0], (blender));

    } else if (blender > 1023) {
      inLeds[x] = blend(pallet[2], black, (blender));

    } else if (blender > 767) {
      inLeds[x] = blend(black, pallet[2], (blender));

    } else if (blender > 511) {
      inLeds[x] = blend(pallet[1], black, (blender));

    } else if (blender > 255) {
      inLeds[x] = blend(black, pallet[1], (blender));

    } else {
      inLeds[x] = blend(pallet[0], black, (blender));

    }
  }
}

void interfacTwinkle(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {
  pallet[0] = indexToColorPlus2(modeSettings.value[0], 32);

  modeSettingFloat[0] = modeSettings.value[1] * 2.5; //fade amount
  modeSettingFloat[1] = modeSettings.value[2] * 5.2; //twinkle amount
  modeSettingFloat[2] = modeSettings.value[3] * 2.5; //delay

}
void twinkle1color(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  //  SaturationA=255; 
  uint8_t fadeAmount = modeSettings[0];
  uint16_t chanceOfGlitter = modeSettings[1]; //0-255 if chance 256-511 number of loops range?
  fadeToBlackBy(inLeds, numOfLeds, fadeAmount);
  uint16_t loops = 1;
  if (modeSettings[1] > 255) {
    loops = modeSettings[1] - 255;
  }

  delay(modeSettings[2]);

  for (int16_t i = 0; i < loops; i++) {

    if (random8() < chanceOfGlitter) {
      inLeds[random16(NUM_LEDS)] += pallet[0];
    }
    if (random8() > chanceOfGlitter) {
      inLeds[random16(NUM_LEDS)] += pallet[0];
    }
  }
}

void interfacRainbowWrap(ModeSettingSimple modeSettings, float * modeSettingFloat, CRGB * pallet, uint8_t numOfSettings) {

  int16_t milliSecondsMax = 10100; //100,00
  if (modeSettings.value[0] > 0) {
    milliSecondsMax = milliSecondsMax - modeSettings.value[0] * 100; //flip modes settings to rpm for function
  } else if (modeSettings.value[0] < 0) {
    milliSecondsMax *= -1;
    milliSecondsMax = milliSecondsMax - modeSettings.value[0] * 100;
  } else {
    milliSecondsMax = 0;
  }
  modeSettingFloat[0] = milliSecondsMax; // duration

  modeSettingFloat[1] = (float) modeSettings.value[1] / 5; //stresh
  if (modeSettingFloat[1] == 0) {
    modeSettingFloat[1] = .05;
  }

}
void rainbowWrap(CRGB * pallet, float * modeSettings, CRGB * inLeds, uint16_t numOfLeds) {
  uint32_t duration;
  bool isNegativeMotion = false;
  if (modeSettings[0] < 0) {
    isNegativeMotion = true;
    duration = -modeSettings[0]; ///modeSettings[1];///modeSettings[1] ; //ms per revolution
  } else {
    duration = modeSettings[0]; ///modeSettings[1]; 
  }
  static uint16_t currentTime;
  static uint16_t previousTime;
  currentTime = millis();

  uint32_t elapsedTime = uint16_t(currentTime - previousTime);

  if (elapsedTime >= duration) {
    elapsedTime = elapsedTime - duration; //if skipped overZero
    previousTime = currentTime; //star duration over
  }
  elapsedTime = elapsedTime << 8; //8+1 to double the rotaion time so it goes around correctly
  //elapsedTime*=2;//done above with 9 instead of 8// 2 domains

  uint32_t elapsedTime256 = elapsedTime / duration;
  if (duration == 0) {
    elapsedTime256 = 0; //no rotation
  }

  uint32_t bitsBigger24 = 0xffffff; //this is used to make the ratio a bigger value to reduce rounding errors
  bitsBigger24 *= modeSettings[1]; //Strech or shrink float 
  uint32_t ratio = bitsBigger24 / numOfLeds; //0xff ff ff //bigger ratio value to prevent rounding errors

  uint16_t flipperRange = 0;
  int16_t flipper = 1;
  if (isNegativeMotion) {
    flipperRange = numOfLeds - 1;
    flipper = -1;
  }

  uint16_t x;
  for (int16_t i; i < numOfLeds; i++) {
    x = flipperRange + (i * flipper);

    uint32_t blender = i * ratio;
    //blender*=2;//2 domains done below with 15 instead of 16
    blender = blender >> 16;
    blender = blender + elapsedTime256;
    //printf("blender = %d  :: elapsedTime256 = %d  \n", blender , elapsedTime256 );
    blender %= 256; //we have 2 gradients so we need 256*2 ranges
    // inLeds[i] = blend(pallet[0],pallet[1],(blender+elapsedTime256));

    inLeds[x] = CHSV(blender, 255, 255); //+elapsedTime256));
    //   inLeds[i] = blend(pallet[1],pallet[0],(blender+elapsedTime256));

  }

}

#endif