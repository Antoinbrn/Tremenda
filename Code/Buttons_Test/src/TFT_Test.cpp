#include <Arduino.h>
#include <TFT_eSPI.h>
/*
TFT_eSPI tft =TFT_eSPI();

void setup() {
    Serial.begin(115200);

    //Initialize the display
    tft.init();

    //Set rotation (0 to 3 for portrait/landscape)
    tft.setRotation(1);

    //Fill sccreen with a black background
    tft.fillScreen(TFT_BLACK);

    //Draw a red rectangle
    // tft.fill(x, y, width, height, color);
    tft.fillRect(50, 50, 140, 100, TFT_RED);

    //setup text properties
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
}

void loop() {
    //display a continously updating timer
    tft.setCursor(60,90);
    tft.print("Time: ");
    tft.print(millis() / 1000);
    tft.print("s");
    delay(1000);
}*/