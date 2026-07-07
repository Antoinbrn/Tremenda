#include <Arduino.h>
#include <Wire.h>

//The libraries under is for the OLED module
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//The libraries under is for the Micro SD card
#include <FS.h>
#include <SD.h>
#include <SPI.h>

//defines the custom pins for the SD module
#define SCK  4
#define MISO  6
#define MOSI  5
#define CS  7

//Pins for the OLED screen
#define SDA 20
#define SCL 21

#define SCREEN_WIDTH 128 // define the screen width in pixels
#define SCREEN_HEIGHT 64 // define the screen height in pixels
/*
extern Adafruit_SSD1306 display;

SPIClass spi(0);

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){ //Defines the function to list directories/ folders
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Listing directory: %s\n", dirname);
  display.display();

  File root = fs.open(dirname);
  if(!root){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Failed to open directory");
    display.display();
    return;
  }
  if(!root.isDirectory()){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Not a directory");
    display.display();
    return;
  }

  File file = root.openNextFile();
  while(file){
    if(file.isDirectory()){
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("  DIR : ");
        display.display();
        Serial.println(file.name());
        if(levels){
            listDir(fs, file.name(), levels -1);
        }
    } else {
        display.clearDisplay();
        display.setCursor(0,0);
        display.print("  FILE: ");
        display.display();

        display.clearDisplay();
        display.setCursor(0,0);
        display.print(file.name());
        display.display();

        display.clearDisplay();
        display.setCursor(0,0);
        display.println("  SIZE: ");
        display.display();

        display.clearDisplay();
        display.setCursor(0,0);
        display.print(file.size());
        display.display();
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path){//Creates directories
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Creating Dir: %s\n", path);
    display.display();

  if(fs.mkdir(path)){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Dir created");
    display.display();
  } else {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("mkdir failed");
    display.display();
  }
}

void removeDir(fs::FS &fs, const char * path){// Deletes Directories
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Removing Dir: %s\n", path);
    display.display();
  
  if(fs.rmdir(path)){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Dir removed");
    display.display();
  } else {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("rmdir failed");
    display.display();
  }
}

void readFile(fs::FS &fs, const char * path){//Reads files
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Reading file: %s\n", path);
    display.display();

    File file = fs.open(path);
    if(!file){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Failed to open file for reading");
        display.display();
        return;
    }

    Serial.print("Read from file: ");
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Read from file: ");
    display.display();

    while(file.available()){
        Serial.write(file.read());
        display.clearDisplay();
        display.setCursor(0,0);
        display.write(file.read());
        display.display();
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){//Writes files
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Writing file: %s\n", path);
  display.display();

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Failed to open file for writing");
    display.display();
    return;
  }
  if(file.print(message)){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("File written");
    display.display();
  } else {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Write failed");
    display.display();
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){//Ads texts inside files without deleting everything inside
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Appending to file: %s\n", path);
    display.display();

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Failed to open file for appending");
    display.display();
    return;
  }
  if(file.print(message)){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Message appended");
    display.display();
  } else {
    Serial.println("Append failed");
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Append failed");
    display.display();
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){//Rename files
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Renaming file %s to %s\n", path1, path2);
    display.display();
  if (fs.rename(path1, path2)) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("File renamed");
    display.display();
  } else {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Rename failed");
    display.display();
  }
}

void deleteFile(fs::FS &fs, const char * path){//Deletes files
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Deleting file: %s\n", path);
    display.display();
  if(fs.remove(path)){
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("File deleted");
    display.display();
  } else {
    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Delete failed");
    display.display();
  }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
        size_t toRead = len;
        if(toRead > 512){
            toRead = 512;
        }
        file.read(buf, toRead);
        len -= toRead;
        }
        end = millis() - start;
        display.clearDisplay();
        display.setCursor(0,0);
        display.printf("%u bytes read for %u ms\n", flen, end);
        display.display();
        file.close();
    } else {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Failed to open file for reading");
        display.display();
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Failed to open file for writing");
        display.display();
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    display.display();
    file.close();
}

void setup(){
    Serial.begin(115200);
    delay(100);
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
    
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

    spi.begin(SCK, MISO, MOSI, CS); //Defines the custom Pins

    if (!SD.begin(7,spi)) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Card Mount Failed");
        display.display();
        return;
    }

    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("No SD card attached");
        display.display();
        return;
    }

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("SD Card Type: ");
    display.display();
    if(cardType == CARD_MMC){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("MMC");
        display.display();
    } else if(cardType == CARD_SD){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("SDSC");
        display.display();
    } else if(cardType == CARD_SDHC){
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("SDHC");
        display.display();
    } else {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("UNKNOWN");
        display.display();
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);

    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("SD Card Size: %lluMB\n", cardSize);
    display.display();

    listDir(SD, "/", 0);
    delay(1000);
    createDir(SD, "/mydir");
    delay(1000);
    listDir(SD, "/", 0);
    delay(1000);
    removeDir(SD, "/mydir");
    delay(1000);
    listDir(SD, "/", 2);
    delay(1000);
    writeFile(SD, "/hello.txt", "Hello ");
    delay(1000);
    appendFile(SD, "/hello.txt", "World!\n");
    delay(1000);
    readFile(SD, "/hello.txt");
    delay(1000);
    deleteFile(SD, "/foo.txt");
    delay(1000);
    renameFile(SD, "/hello.txt", "/foo.txt");
    delay(1000);
    readFile(SD, "/foo.txt");
    delay(1000);
    testFileIO(SD, "/test.txt");
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    display.display();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    display.display();
    delay(1000);
}

void loop(){

}*/