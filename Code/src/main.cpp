#include <Arduino.h>

#define RGB_LED 48

void setup(){

}

void loop(){
  //Red
  neopixelWrite(RGB_LED, 255, 0, 0);
  delay(1000);

  //Green
  neopixelWrite(RGB_LED, 0, 255, 0);
  delay(1000);

  //Blue
  neopixelWrite(RGB_LED, 0, 0, 255);
  delay(1000);

  //Off
  neopixelWrite(RGB_LED, 0, 0, 0);
  delay(1000);
}