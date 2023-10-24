/*
code for screen text timeout.
class textDisplay
  int startTime;
  int displayDuration
  bool isDisplaySettingName; it could be good if every so often the text re displayed.

modetext
settingText
settingButton..setStart,isDisplaySettingName

if(isDisplaySettingName){
 textDisplay.update(millis());
 tft.textToDisplay(_d.[mode.setting[i].name]);
}else if(){
 display mode name
}

*/

/*
latest idea for strip functions
current screenMode settings{
    int8_t value[6]; char type[6];

}

//screen settings are converted to stripFunc values
int settingstoPalOffset=0; //when do the settings start relative to pallets
for(i:values){ //first pal

settingstoPalOffset=i;

if(type != pal)break; or continue;
if(type == pal){ // pal is pretty straight forward 
CRGB color[i] = convertFrompal(value);


}
}
for(i:settings){


 if(  type == unused ){ break}

   setting[i] = value[i + settingstoPalOffset];
}

//then those values are adjusted to be in range of strip function if nessicary
//this is done with custom functions for each
void interface_stripFunc1_3_2(float *settings){
setting[0]  = settings[0]/=100.0; //lets say this func take value in setting from 0-1.0
settings[1] = settings[1]*2.56 -1; convert to byte;
}


void stripFunc1_3_2(CRGB *colors, float *settings, CRGB *leds, uint16_t numLeds){
      arbitrary number of colors 
      arbitrary number of settings


}



*/







/* 

updating text box with matching ptr

#include <stdio.h>
#include <iostream>
using namespace  std;

int datas = 2;
int otherData =5;

class withPtr{
    public:
  int *ptr;  
};
int main()
{
    
    
    withPtr testerA;
    withPtr testerB;
    withPtr testerC;
    
    testerA.ptr = &datas;
        testerB.ptr = &otherData;

    testerC.ptr = &datas;
    
    cout << testerA.ptr << endl;
    cout << testerB.ptr << endl;
    cout << testerC.ptr << endl;

if(testerA.ptr == testerC.ptr){
    printf("matching address");}

    return 0;
}
*/