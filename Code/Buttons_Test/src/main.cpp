#include <Arduino.h>
#include <Wire.h>

//The libraries under is for the OLED module
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//libraries for MicroSD
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <vector>

#define SCREEN_WIDTH 128 // define the screen width in pixels
#define SCREEN_HEIGHT 64 // define the screen height in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//Pins for the OLED screen
#define SDA 20
#define SCL 21

//Define the buttons pin and their names
#define Left 0
#define Right 1
#define Up 2
#define Down 3
#define Vol_Up 10
#define Vol_Down  9
#define Ok_Pin 8

//defines the custom pins for the SD module
#define SCK  4
#define MISO  6
#define MOSI  5
#define CS  7

#define row_Height 12 //Defines the height of the rows
#define max_Row 5 //defines the number of rows possible

//volume variables
int globalVolume = 0; // Percentage 0 - 100
const int VOL_STEP = 5; // How much volume changes per click

SPIClass spi(0);

uint8_t cardType = SD.cardType();

struct FileItem {
  String name;
  bool isDir;
};

//Sets the necesary parameters for the MicroSd
std::vector<FileItem> currentFiles;
String currentPath = "/";
int selectedIndex = 0;
int scrollIndex = 0;

//Debounce helper
bool isButtonPressed(int pin) {
    if (digitalRead(pin) == LOW) { // Checks if the button is pressed
        delay(50);// Simple debounce
        if (digitalRead(pin) == LOW ){
            while (digitalRead(pin) == LOW); // Wait for release
            return true;
        }
    }
    return false;
}

//Truncate file name if it exceeds screen width
String truncateName(String name, int maxWidth) {
  int charWidth = 6; // Standard font is 6 pixel
    int maxChars = maxWidth / charWidth;

    if (name.length() >= maxChars) {
        return name.substring(0, maxChars - 3 ) + "..."; //Shortens the name if the file name exceeds or is equal to the length of the screen.
    }
    else {
        return name;
    }
}

//Render the File Explorer UI to the OLED
void updateDisplay() {
    display.clearDisplay();

    //Header Showing current path
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(truncateName(currentPath, SCREEN_WIDTH));
    display.drawFastHLine(0,9, SCREEN_WIDTH, WHITE);

    if (currentFiles.empty()){
        display.setCursor(0,20);
        display.print("[Empty Directory]");
        display.display();
        return;
    }

    // adjust the scroll window
    if (selectedIndex < scrollIndex) {
        scrollIndex = selectedIndex;
    }
    else if (selectedIndex >= scrollIndex + max_Row){  
        scrollIndex = selectedIndex - max_Row + 1;
    }

    //Show files row
    for (int i = 0; i < max_Row; i++) {
        int fileIdx = scrollIndex +i;
        if (fileIdx >= currentFiles.size()) break;

        int yPos = 12 + (i * row_Height);
        FileItem item = currentFiles[fileIdx];

        //Draw rectangle
        if (fileIdx == selectedIndex) {
            display.fillRect(0, yPos - 1, SCREEN_WIDTH, row_Height, WHITE);
            display.setTextColor(BLACK); //Invert text color
        }
        else {
            display.setTextColor(WHITE);
        }

        display.setCursor(2, yPos + 1);
        
        // Prefix Folders with a '/' slash
        String displayName = (item.isDir ? "/" : "") + item.name;
        display.print(truncateName(displayName, SCREEN_WIDTH - 4));
    }

    display.display();
}

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

void readDirectory(String path) {
    currentFiles.clear();
    selectedIndex = 0;
    scrollIndex = 0;

    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) {
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Failed to open directory");
        return;
    }
    
    File file = dir.openNextFile();
    while (file) {
        FileItem item;

        //strip the leading path if necessary, grab just the name
        String name = String(file.name());
        if (name.lastIndexOf('/') != -1) {
            name = name.substring(name.lastIndexOf('/') + 1);
        }

        item.name = name;
        item.isDir = file.isDirectory();
        currentFiles.push_back(item);
        file = dir.openNextFile();
    }
    dir.close();
}

void showVolumeAnimation() {
  display.clearDisplay();

  //Draw Volume Title Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(36, 12);
  display.print("Volume: ");
  display.print(globalVolume);
  display.print("%");

  //Draw Outer Boundary Rectangle for the Bar
  int barWidth = 100;
  int barHeight = 14;
  int barX = (SCREEN_WIDTH - barWidth) / 2; //Center horizontally
  int barY = 30;

  display.drawRect(barX, barY, barWidth, barHeight, WHITE);

  //Calculate filled rectangle width mapping 0-100% leaving 2 pixel border
  int fillWidth = map(globalVolume, 0, 100, 0, barWidth - 4);

  if (fillWidth > 0) {
    display.fillRect(barX + 2, barY + 2, fillWidth, barHeight - 4, WHITE);
  }
  display.display();

  //Keep volume bar visible briefly before returning to previous menu state
  delay(250);

  display.clearDisplay();
  //Automatically redraw the exact screen state we were in before the volume interrupt
  updateDisplay();
}

void setup() {
  Serial.begin(115200); // Starts the Baud rate
  delay(100);

  uint8_t cardType = SD.cardType();

  //Defines the Buttons as inputs
  pinMode(Left, INPUT_PULLUP);
  pinMode(Up, INPUT_PULLUP);
  pinMode(Right, INPUT_PULLUP);
  pinMode(Down, INPUT_PULLUP);
  pinMode(Vol_Up, INPUT_PULLUP);
  pinMode(Vol_Down, INPUT_PULLUP);
  pinMode(Ok_Pin, INPUT_PULLUP);

  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);

  Wire.setPins(SDA, SCL); //Define the SDA, SCL Pins

  spi.begin(SCK, MISO, MOSI, CS); //Defines the custom Pins
  
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
  delay(500); // Wait for a second

  if (!SD.begin(7,spi)) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("Card Mount Failed");
      display.display();
      return;
  }

  //Load Root Directory
  readDirectory(currentPath);
  updateDisplay();
}

void loop() {
  bool stateChanged = false;

  //Volume interruption detection
  if (isButtonPressed(Vol_Up)) {
    globalVolume += VOL_STEP; // Add 5 from the existing volume
    if (globalVolume > 100) globalVolume = 100; // Cap at max
    showVolumeAnimation(); // Displays animation and restores the screen automatically
  }

  if (isButtonPressed(Vol_Down)) {
    globalVolume -= VOL_STEP; // Remouves 5 from the existing volume
    if (globalVolume < 0) globalVolume = 0; // Cap at min
    showVolumeAnimation();
  }

    //Navigate Up
    if (isButtonPressed(Up)) {
        if (selectedIndex > 0) {
            selectedIndex--;
            stateChanged = true;
        }
    }

    //Navigate Down
    if (isButtonPressed(Down)) {
        if (selectedIndex < (int) currentFiles.size() - 1) {
            selectedIndex++;
            stateChanged = true;
        }
    }

    //Enter Folder/ select File (OK or right)

    if (isButtonPressed(Ok_Pin) || isButtonPressed(Right)) {
        if (!currentFiles.empty()) {
            FileItem selected = currentFiles[selectedIndex];
            if (selected.isDir) {
                //Update Path string cleanly
                if (currentPath == "/") {
                    currentPath += selected.name;
                }
                else {
                    currentPath += "/" + selected.name;
                }
                readDirectory(currentPath);
                stateChanged = true;
            }
            else {
                //it's a file, update this line to make the music play
                display.clearDisplay();
                display.setCursor(0,0);
                display.print("Selected File: ");
                display.println(currentPath + "/" + selected.name);
            }
        }
    }

    // Back Track Folder (Left)
    if (isButtonPressed(Left)) {
        if (currentPath != "/") {
            int lastSlash = currentPath.lastIndexOf('/');
            if (lastSlash == 0) {
                currentPath = "/";
            }
            else {
                currentPath = currentPath.substring(0, lastSlash);
            }
            readDirectory(currentPath);
            stateChanged = true;
        }
    }

    //Refresh UI if user interacted
    if (stateChanged) {
        updateDisplay();
    }
}