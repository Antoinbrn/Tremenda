/*#include <Arduino.h>

//Includes the library for the OLED screen
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Includes the libraries for the MicroSD
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <vector>

#define SDA 20
#define SCL 21

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

//custom MicroSD pins
#define SCK  4
#define MISO  6
#define MOSI  5
#define CS  7

#define row_Height 12 //Defines the height of the rows
#define max_Row 5 //defines the number of rows possible

//Define the buttons pin and their names
#define Left 0
#define Right 1
#define Up 2
#define Down 3
#define Vol_Up 10
#define Vol_Down  9
#define Ok_Pin 8

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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

//Process if file name exceed screen width
String nameShortener(String name, int maxWidth) {
    int charWidth = 6; // Standar font is 6 pixel
    int maxChars = maxWidth / charWidth;

    if (name.length() >= maxChars) {
        return name.substring(0, maxChars - 3 ) + "..."; //Shortens the name if the file name exceeds or is equal to the length of the screen.
    }
    else {
        return name;
    }
}

//Creates the parameters to read Directories inside the MicroSD
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

//Show the UI to the OLED
void updateDisplay() {
    display.clearDisplay();

    //Header Showing current path
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(nameShortener(currentPath, SCREEN_WIDTH));
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
        display.print(nameShortener(displayName, SCREEN_WIDTH - 4));
    }

    display.display();
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
}*/