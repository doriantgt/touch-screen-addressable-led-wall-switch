
/*
wanting to make a sunrise mode that turns on the led strip at a selected time and has some settings to control how long the led strip takes to brighten.

#include <FastLED.h>

uint32_t timeToSecs(float hour, uint8_t minute=0, uint8_t sec =0){
    uint32_t seconds = hour * 60 * 60;
    seconds += minute * 60;
    seconds += sec;
    return seconds;

}
CRGB sunRiseColor(uint8_t currentTimeRatio8){

  byte hue1 = 160;
  byte hue2 = 255;

  byte hue3 = lerp8by8(hue1,hue2,currentTimeRatio8);
  byte saturation = sin8((currentTimeRatio8/2)-192);
  saturation -= 255; //flip over;

 // for (int i = 0; i < 600; i++) {  printf("%i ,  %i \n" , i, sin8(i));delay(100); }

  CRGB rgb = CHSV(hue3, saturation, currentTimeRatio8);   
  printf(" %i , %i , %i \n" , rgb.r, rgb.g ,rgb.b);
  return  rgb;

}


#define DATA_PIN    19
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    132
CRGB leds[NUM_LEDS];

//#define BRIGHTNESS          96
//#define FRAMES_PER_SECOND  120



#include <WiFi.h>
#include "time.h"

const char* ssid       = "arthole";
const char* password   = "helpfulbox";



const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -3600*7;
const int   daylightOffset_sec = 3600;



#include <FastLED.h>

class SunriseAlarm {

  private: 
    float _sunrise8ratio;

  public:
    uint32_t startTime_sec;
    uint32_t riseTime_sec;
    uint32_t holdTime_sec;
    bool isAlarmActive =0;


    
  void setSunrise(uint32_t start_sec, uint32_t riseLength_sec, uint32_t hold_sec){
    startTime_sec = start_sec; 
    riseTime_sec  = riseLength_sec;
    holdTime_sec  = hold_sec;

    _sunrise8ratio = (float)256/(float)riseTime_sec;
   // printf("riseTime_sec = %i\n" , riseTime_sec);
   // printf("_sunrise8ratio = %f\n" , _sunrise8ratio);
  }

  uint8_t getSunriseRatio8(uint32_t currentTime_sec){
  // printf(" sunriseRatio8 currentTime = %i \n" ,currentTime_sec );

    //if time is not above start time check if alarm wraps around 2400hrs 
     if(currentTime_sec < startTime_sec){ currentTime_sec += 3600 * 24;}
         //       printf("check wrap around time %i  :: %i\n" , currentTime_sec , startTime_sec + riseTime_sec);

      if(currentTime_sec > startTime_sec && currentTime_sec <= startTime_sec + riseTime_sec ){
          //inside riseTime
        //  printf("rising time %f \n", (currentTime_sec-startTime_sec)*_sunrise8ratio );
           isAlarmActive = true;
          return (currentTime_sec-startTime_sec)*_sunrise8ratio;

      }else if(currentTime_sec > startTime_sec + riseTime_sec && currentTime_sec < startTime_sec + riseTime_sec +holdTime_sec){
        //inside hold time
      //    printf("hold time \n" );
          isAlarmActive = true;
         return 255;
      }else{
          isAlarmActive = false;
      return 0;}


  }


};



void printLocalTime(){
  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println();
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      Serial.println(timeinfo.tm_sec);



}
void getTimeFromWeb(){
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

}

    SunriseAlarm sunriseAlarm;
     tm timeinfo;

   // TimeMillis timeMillis;
   // struct tm timeinfo;

*/