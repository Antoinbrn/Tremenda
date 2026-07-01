#include <Arduino.h>
#include <Wire.h>

//The libraries under is for the OLED module
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // define the screen width in pixels
#define SCREEN_HEIGHT 64 // define the screen height in pixels

#define SDA 7 //Define the SDA pin from the OLED
#define SCL 8 //Define the SCL pin from the OLED

//Define the buttons pin and their names
#define Left 0
#define Right 1
#define Up 2
#define Down 3
#define Vol_Up 4
#define Vol_Down  5
#define Ok_Pin 6

extern Adafruit_SSD1306 display;
/*
void setup(){
  Serial.begin(115200); // Starts the Baud rate
  delay(100);

  //Defines the Buttons as inputs
  pinMode(Left, INPUT_PULLUP);
  pinMode(Up, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Vol_Up, INPUT_PULLUP);
  pinMode(Vol_Down, INPUT_PULLUP);
  pinMode(Ok_Pin, INPUT_PULLUP);

  Wire.setPins(SDA, SCL); //Define the SDA, SCL Pins
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adress 0x3D for the 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  
  display.setTextSize(1); // Set the smallest text size
  display.setTextColor(WHITE); // Set the color of the text
  display.clearDisplay(); // Clears the OLED screen
  display.setCursor(0,0); // Starts the text from the top left
  display.println("Button Test"); // Define the display
  display.display(); // Display the text
  delay(1000); // Wait for a second
}

void loop() {
  int Left_Button = digitalRead(Left); 
  int Right_Button = digitalRead(Right); 
  int Up_Button = digitalRead(Up);
  int Down_Button = digitalRead(Down);
  int Vol_Up_Button = digitalRead(Vol_Up);
  int Vol_Down_Button = digitalRead(Vol_Down);
  int Ok_Button = digitalRead(Ok_Pin);

  if (Left_Button == LOW){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Left");
    display.display();
    delay(10);
  }

  if (Right_Button == LOW) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Right");
    display.display();
    delay(10);
  }
  
  if (Up_Button == LOW) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("UP");
    display.display();
    delay(10);
  }
  
  if (Down_Button == LOW) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Down");
    display.display();
    delay(10);
  }

  if (Vol_Up_Button == LOW) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Vol Up");
    display.display();
    delay(10);
  }

  if (Vol_Down_Button == LOW) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Vol Down");
    display.display();
    delay(10);
  }

  if (Ok_Button == LOW) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Ok");
    display.display();
    delay(10);
  }
  delay(100);
}*/