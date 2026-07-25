/*#include <Arduino.h>
#include <Wire.h>

//Libraries for TFT
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library for ST7789 (240x320)

#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <vector>

// Define the exact pins connected to your ESP32-C3
#define TFT_CS     2
#define TFT_RST    0
#define TFT_DC     1
#define TFT_MOSI   5  // SDA / DIN Pin
#define TFT_SCLK   4  // SCL / CLK Pin
#define TFT_MISO   6  // MISO / SDO Pin

//Color definition
#define COLOR_BACKGROUND ILI9341_BLACK
#define COLOR_TEXT ILI9341_WHITE
#define COLOR_HIGHLIGHT ILI9341_NAVY
#define COLOR_TEXT_HI ILI9341_YELLOW
#define COLOR_BORDER ILI9341_DARKGREY

//defines the custom pins for the SD module
#define SCK 4
#define MISO 6
#define MOSI 5
#define CS 7

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

//Define the buttons pin and their names
#define Left 20
#define Right 21
#define Up 21
#define Down 3
#define Vol_Up 10
#define Vol_Down  9
#define Ok_Pin 8

//Defines file explorer parameters

#define row_Height 24 //Defines the height of the rows
#define max_Row 11 //defines the number of rows possible
#define MAX_TXT_ROWS 15 //Text viewer rows

// Use the standard Hardware SPI constructor (MISO is handled automatically by SPI.begin)
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

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
    int16_t x1, y1;
    uint16_t w, h;

    // Check if the full name already fites within the maxWidth
    tft.getTextBounds(name, 0, 0, &x1, &y1, &w, &h);
    if (w <= maxWidth) {
        return name;
    }

    // If it doesn't fit, start cutting characters back until it fits with "..."
    String truncated = name;
    while (truncated.length() > 0) {
        truncated = truncated.substring(0, truncated.length() - 1);
        String testString = truncated + "...";

        // Measure the new string's exact pixel width
        tft.getTextBounds(testString, 0, 0, &x1, &y1, &w, &h);
        if (w <= maxWidth) {
            return testString;
        }
    }

    return "..."; // Fallback if the space is incredibly tiny
}

// Render the File Explorer
void updateDisplay() {
    tft.fillScreen(COLOR_BACKGROUND);

    //Top Directory Header Bar
    tft.setTextSize(2);
    tft.setTextColor(COLOR_TEXT_HI);
    tft.setCursor(8, 8);
    tft.print(truncateName(currentPath, tft.width() - 16));
    tft.drawFastHLine(0, 32, tft.width(), COLOR_BORDER);

    if (currentFiles.empty()) {
        tft.setCursor(10, 50);
        tft.setTextColor(COLOR_TEXT);
        tft.print("[Empty Directory]");
        return;
    }

    //Scroll calculation
    if (selectedIndex < scrollIndex) {
        scrollIndex = selectedIndex;
    } else if (selectedIndex >= scrollIndex + max_Row) {
        scrollIndex = selectedIndex - max_Row + 1;
    }

    //Render list elements
    for (int i = 0; i < max_Row; i++) {
        int fileIdx = scrollIndex + i;
        if (fileIdx >= (int)currentFiles.size()) break;

        int yPos = 40 + (i * row_Height);
        FileItem item = currentFiles[fileIdx];

        if (fileIdx == selectedIndex) {
            //High resolution selection block
            tft.fillRect(4, yPos - 2, tft.width() - 8, row_Height, COLOR_HIGHLIGHT);
            tft.setTextColor(COLOR_TEXT_HI);
        } else {
            tft.setTextColor(COLOR_TEXT);
        }

        tft.setCursor(12, yPos + 2);
        String displayName = (item.isDir ? "/" : "") + item.name;
        tft.print(truncateName(displayName, tft.width() - 24));
    }
}

void readDirectory(String path) {
    currentFiles.clear();
    scrollIndex = 0;
    selectedIndex = 0;

    File dir = SD.open(path);
    if (!dir || !dir.isDirectory()) {
        tft.println("Failed to open directory");
        return;
    }

    File file = dir.openNextFile();
    while (file) {
        FileItem item;
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

//Show temporary volume bar animation overlay
void showVolumeAnimation() {
    tft.fillScreen(COLOR_BACKGROUND);

    tft.setTextSize(2);
    tft.setTextColor(COLOR_TEXT);
    tft.setCursor(60, 80);
    tft.print("VOLUME: ");
    tft.print(globalVolume);
    tft.print("%");

    int barWidth = 180;
    int barHeight = 24;
    int barX = (tft.width() - barWidth) / 2; //Centered horizontally
    int barY = 130;

    tft.drawRect(barX, barY, barWidth, barHeight, COLOR_TEXT);
    int fillWidth = map(globalVolume, 0, 100, 0, barWidth - 4);
    if (fillWidth > 0) {
        tft.fillRect(barX + 2, barY + 2, fillWidth, barHeight - 4, ILI9341_GREEN);
    }

    delay(1000);
}

//Text File Viewer Sub_system
void readAndDisplayTxtFile(String filePath) {
    std::vector<String> fileLines;

    File file = SD.open(filePath, FILE_READ);
    if (!file) {
        tft.println("Failed to open text file");
        return;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        fileLines.push_back(line);
    }
    file.close();

    if (fileLines.empty()) {
        fileLines.push_back("[Empty File]");
    }

    int txtScrollIdx = 0;
    bool insideViewer = true;

    while (insideViewer) {
        tft.fillScreen(COLOR_BACKGROUND);
        tft.setTextSize(2); //Dynamic text size 2 for confort
        tft.setTextColor(COLOR_TEXT);

        for (int i = 0; i < MAX_TXT_ROWS; i++) {
            int lineIdx = txtScrollIdx + i;
            if (lineIdx >= (int)fileLines.size()) break;

            tft.setCursor(8, i * 20 + 10); //20px line vertical padding
            tft.print(truncateName(fileLines[lineIdx], tft.width() - 16));
        }

        //Wait for button interactions
        unsigned long startTime = millis();
        bool actionTaken = false;

        while(millis() - startTime < 300 && !actionTaken) {
            if (isButtonPressed(Down)) {
                if (txtScrollIdx + MAX_TXT_ROWS < (int)fileLines.size()) {
                    txtScrollIdx++;
                }
                actionTaken = true;
            }
            if (isButtonPressed(Up)) {
                if (txtScrollIdx > 0) {
                    txtScrollIdx--;
                }
                actionTaken = true;
            }
            if (isButtonPressed(Left)) {
                insideViewer = false;
                actionTaken = true;
            }
            if (isButtonPressed(Vol_Up)) {
                globalVolume = min(globalVolume + VOL_STEP, 100);
                showVolumeAnimation();
                actionTaken = true;
            }
            if (isButtonPressed(Vol_Down)) {
                globalVolume = max(globalVolume - VOL_STEP, 0);
                showVolumeAnimation();
                actionTaken = true;
            }
            delay(10);
        }
    }
}

void setup() {
    Serial.begin(115200);

    //Creates the button ass HIGH
    pinMode(Up, INPUT_PULLUP);
    pinMode(Down, INPUT_PULLUP);
    pinMode(Left, INPUT_PULLUP);
    pinMode(Right, INPUT_PULLUP);
    pinMode(Vol_Up, INPUT_PULLUP);
    pinMode(Vol_Down, INPUT_PULLUP);
    pinMode(Ok_Pin, INPUT_PULLUP);

    //Initialize TFT
    SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
    tft.begin();
    tft.setRotation(0); //0 is portrait (240x320), 1 is Landscape (320x240)
    tft.fillScreen(COLOR_BACKGROUND);

    Serial.print("Initializing SD card...");
    if (!SD.begin(CS)) {
        Serial.println("initialization failed!");
        tft.setCursor(10, 50);
        tft.setTextColor(ILI9341_RED);
        tft.setTextSize(2);
        tft.print("SD Card Fail!");
        for (;;);
    }
    Serial.println("initialization done");

    readDirectory(currentPath);
    updateDisplay();
}

void loop() {
    bool stateChanged = false;

    //Volume events
    if (isButtonPressed(Vol_Up)) {
        globalVolume = min(globalVolume + VOL_STEP, 100);
        showVolumeAnimation();
        stateChanged = true;
    }
    if (isButtonPressed(Vol_Down)) {
        globalVolume = max(globalVolume - VOL_STEP, 0);
        showVolumeAnimation();
        stateChanged = true;
    }

    //Nav config
    if (isButtonPressed(Up)) {
        if (selectedIndex > 0) {
            selectedIndex--;
            stateChanged = true;
        }
    }
    if (isButtonPressed(Down)) {
        if (selectedIndex < (int)currentFiles.size() - 1) {
            selectedIndex++;
            stateChanged = true;
        }
    }

    if (isButtonPressed(Ok_Pin) || isButtonPressed(Right)) {
        if (!currentFiles.empty()) {
            FileItem selected = currentFiles[selectedIndex];
            String fullPath = (currentPath == "/") ? ("/" + selected.name) : (currentPath + "/" + selected.name);

            if (selected.isDir) {
                currentPath = fullPath;
                readDirectory(currentPath);
                stateChanged = true;
            } else {
                String lowerName = selected.name;
                lowerName.toLowerCase();
                if (lowerName.endsWith(".txt")) {
                    readAndDisplayTxtFile(fullPath);
                    stateChanged = true;
                }
            }
        }
    }

    if (isButtonPressed(Left)) {
        if (currentPath != "/") {
            int lastSlash = currentPath.lastIndexOf('/');
            currentPath = (lastSlash == 0) ? "/" : currentPath.substring(0, lastSlash);
            readDirectory(currentPath);
            stateChanged = true;
        }
    }

    if (stateChanged) {
        updateDisplay();
    }
}*/