// esp32_clock

#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library
TFT_eSprite sprite  = TFT_eSprite(&tft); // 文字盤
TFT_eSprite needle1   = TFT_eSprite(&tft); // Sprite秒針
TFT_eSprite needle2   = TFT_eSprite(&tft); // Sprite分針
TFT_eSprite needle3   = TFT_eSprite(&tft); // Sprite時針

// For ESP32 Dev board (only tested with GC9A01 display)
// The hardware SPI can be mapped to any pins

//#define TFT_MOSI 23 // In some display driver board, it might be written as "SDA" and so on.
//#define TFT_SCLK 18
//#define TFT_CS   22  // Chip select control pin
//#define TFT_DC   16  // Data Command control pin
//#define TFT_RST  5  // Reset pin (could connect to Arduino RESET pin)
//#define TFT_BL   22  // LED back-light


#include <image.h>

#define MAX_IMAGE_WIDTH 240 // Sets rendering line buffer lengths, adjust for your images

// Include the jpeg decoder library
#include <TJpg_Decoder.h>

#include "RTClib.h"
//RTCモジュールのインスタンス
RTC_DS1307 rtc;

//RTCとesp-wroom-32のピン配置です。

//RTC GND　→　esp-wroom-32 GND

//RTC VCC　→　esp-wroom-32 3.3VCC

//RTC SCL　→　esp-wroom-32 GPIO 22

//RTC SDA　→　esp-wroom-32 GPIO 21



// 現在の時刻を取得
int hours = 11 ;
int minutes = 7 ;
int seconds = 0 ; 

double minuteAngle;
double hourAngle;
double secondAngle ;

int radius = 120;
int centerX = 120;
int centerY = 120;

int16_t xpos = (240-70)/2;
int16_t ypos = (240-70)/4;



#define DEG_TO_RAD 0.017453292519943295769236907684886

// =======================================================================================
// This function will be called during decoding of the jpeg file
// =======================================================================================
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  sprite.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}


void setup() {
 
  Serial.begin(115200);
  //#ifndef ESP8266
  //while (!Serial); // wait for serial port to connect. Needed for native USB
  //endif


  tft.begin();       // Initialise the display
 

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }


  // The byte order can be swapped (set true for TFT_eSPI)
  TJpgDec.setSwapBytes(true);

  // The jpeg decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);


  

  
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  //rtc.adjust(DateTime(2024, 12, 14, 1, 34, 0));

  


// スプライトの作成、文字盤
  sprite.setColorDepth(8);
  sprite.createSprite(240,240);
// スプライトの作成、秒針
  needle1.setColorDepth(8);
  needle1.createSprite(20,100);
  needle1.fillSprite(TFT_TRANSPARENT);
  needle1.fillTriangle(4,4,16,4,10,106,TFT_BLACK);
  needle1.fillTriangle(8,4,12,4,10,100,TFT_RED);  
  needle1.fillCircle(10,10,8,TFT_BLACK);
  needle1.setPivot(10,10);
// スプライトの作成、分針　　　　　　　　　　　
  needle2.setColorDepth(8);
  needle2.createSprite(20,100);
  needle2.fillSprite(TFT_TRANSPARENT);
  needle2.fillTriangle(4,4,16,4,10,106,TFT_BLACK);
  needle2.fillTriangle(7,4,13,4,10,100,TFT_BLUE);
  //needle2.fillCircle(10,10,8,TFT_BLACK);
  needle2.setPivot(10,10);
// スプライトの作成、時針
  needle3.setColorDepth(8);
  needle3.createSprite(20,100);
  needle3.fillSprite(TFT_TRANSPARENT);
  needle3.fillTriangle(4,4,16,4,10,96,TFT_BLACK);
  needle3.fillTriangle(6,4,14,4,10,90,TFT_YELLOW);
  //needle3.fillCircle(10,10,8,TFT_BLACK);
  needle3.setPivot(10,10);



} 





void loop(void) {




  sprite.fillCircle(120,120,120,TFT_BLUE);
  sprite.fillCircle(120, 120, 115, TFT_WHITE);
  
 
  // Draw the dial----------------------------------------
  TJpgDec.drawJpg(0, 0, dial, sizeof(dial));
  TJpgDec.drawJpg(xpos,ypos, yochan, sizeof(yochan));

  sprite.setCursor(80, 115);
  sprite.setTextColor(TFT_BLACK);  sprite.setTextSize(2.5);
  sprite.println("YOCHAN");

  sprite.setCursor(95, 135);
  sprite.setTextColor(TFT_BLACK);  sprite.setTextSize(2.5);
  sprite.println("-TV");

  sprite.setCursor(65, 150);
  sprite.setTextColor(TFT_BLACK);  sprite.setTextSize(2);
  sprite.println("electronic");

  sprite.setCursor(95, 165);
  sprite.setTextColor(TFT_BLACK);  tft.setTextSize(2);
  sprite.println("work");


   
    // 時針の描画

DateTime now = rtc.now();

//Serial.println(String("DateTime::TIMESTAMP_FULL:\t")+now.timestamp(DateTime::TIMESTAMP_FULL));
//Serial.println("\n");

//double minuteAngle = minutes *6 + 180; //180度進めとく
minuteAngle = now.minute() *6 + 180 ; //180度進めとく 

//Serial.println(minuteAngle);
//Serial.println("\n");

//double hourAngle  = hours * 30 + minutes *30/60 + 180; //180度進めとく
hourAngle  = (now.hour()%12) * 30 + now.minute() *30/60 + 180 ;    //180度進めとく

//Serial.println(hourAngle);
//Serial.println("\n");

//double secondAngle = +180;  //180度進めとく
secondAngle =  now.second()*6 + 180 ;  //180度進めとく

//Serial.println(secondAngle);
//Serial.println("\n");


  seconds = seconds + 1;

  //secondAngle = (seconds)*6;  // 秒の針をリアルタイムで動かす時に使う
  
  //minuteAngle = minuteAngle  + (seconds)*(6/60);      // 分の針をリアルタイムで動かす時に使う
  
  //hourAngle =  hourAngle + (seconds)*(6/60/12) ;       //時の針をリアルタイムで動かす時に使う

  
  //int hourHandLength = radius * 0.6;
  //int hourHandendX = centerX + hourHandLength * sin(hourAngle* DEG_TO_RAD);
  //int hourHandendY = centerY - hourHandLength * cos(hourAngle* DEG_TO_RAD);
  //sprite.drawLine(centerX, centerY, hourHandendX, hourHandendY, TFT_YELLOW);

  // スプライトを画面表示する
  needle3.pushRotated(&sprite,(hourAngle),TFT_TRANSPARENT);  

  // 分針の描画
  //int minuteHandLength ;
  //minuteHandLength = radius * 0.7;
  //int minuteHandendX = centerX + minuteHandLength * sin(minuteAngle* DEG_TO_RAD);
  //int minuteHandendY = centerY - minuteHandLength * cos(minuteAngle* DEG_TO_RAD);
  //sprite.drawLine(centerX, centerY,  minuteHandendX, minuteHandendY, TFT_BLUE);

  // スプライトを画面表示する
  needle2.pushRotated(&sprite,(minuteAngle),TFT_TRANSPARENT);

  
  // 秒針の描画
  //int secondHandLength ;
  //secondHandLength = radius * 0.85;
  //int secondHandendX = centerX + secondHandLength * sin(secondAngle* DEG_TO_RAD);
  //int secondHandendY = centerY - secondHandLength * cos(secondAngle* DEG_TO_RAD);
  //sprite.drawLine(centerX, centerY, secondHandendX, secondHandendY, TFT_RED);

  // スプライトを画面表示する
  needle1.pushRotated(&sprite,(secondAngle),TFT_TRANSPARENT);


   
  /*
  int memoriLength ; 
  for (int i=0;i<12;i++) {
    memoriLength = radius * 0.85 ;
    int memoristartX = centerX + memoriLength * sin((i * 30)* DEG_TO_RAD);
    int memoristartY = centerY - memoriLength * cos((i * 30)* DEG_TO_RAD);
    memoriLength = radius * 1.0;
    int memoriendX = centerX + memoriLength * sin((i * 30)* DEG_TO_RAD);
    int memoriendY = centerY - memoriLength * cos((i * 30)* DEG_TO_RAD);
    sprite.drawLine(memoristartX, memoristartY, memoriendX, memoriendY, TFT_BLACK);
    //tft.drawLine(memoristartX, memoristartY, memoriendX, memoriendY, TFT_GREEN);
  }
  */

  // スプライトを画面表示する
  sprite.pushSprite(0,0);  
  delay(1000);

}




