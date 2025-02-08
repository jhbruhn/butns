// Btns 0.6
// by Leo Kuroshita for Hügelton instruments.
// Modified to include MPU-6050 tilt support

#include "MonomeSerialDevice.h"
#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

#define NUM_ROWS 8
#define NUM_COLS 8
#define BRIGHTNESS 127

#define DEFAULT_ROTATION 0
#define DEFAULT_FLIP_HORIZONTAL false
#define DEFAULT_FLIP_VERTICAL false

const uint8_t ROW_PINS[NUM_ROWS] = {0, 1, 2, 3, 4, 5, 6, 7};  // GPIO00-GPIO07
const uint8_t COL_PINS[NUM_COLS] = {8, 9, 10, 11, 12, 13, 14, 15};  // GPIO8-GPIO15
const uint8_t gammaTable[16] = { 0,  2,  3,  6,  11, 18, 25, 32, 41, 59, 70, 80, 92, 103, 115, 127}; 
const uint8_t gammaAdj = 2;

bool isInited = false;
String deviceID = "btns";
String serialNum = "m4216124";

char mfgstr[32] = "monome";
char prodstr[32] = "monome";
char serialstr[32] = "m4216124";

Adafruit_NeoPixel pixels(NUM_ROWS * 8, 28, NEO_GRB + NEO_KHZ800);

MonomeSerialDevice mdp;
Adafruit_MPU6050 mpu;

bool buttonStates[NUM_ROWS][NUM_COLS] = {0};

int gridRotation = DEFAULT_ROTATION;
bool flipHorizontal = DEFAULT_FLIP_HORIZONTAL;
bool flipVertical = DEFAULT_FLIP_VERTICAL;

void mapButtonToLED(int buttonRow, int buttonCol, int &ledRow, int &ledCol) {
    // Handle button to LED mapping based on rotation and flipping
    ledRow = buttonRow;
    ledCol = buttonCol;

    // Apply rotation
    switch (gridRotation) {
        case 0: // No rotation
            break;
        case 1: // 90 degree rotation
            {
                int temp = ledRow;
                ledRow = ledCol;
                ledCol = NUM_COLS - 1 - temp;
            }
            break;
        case 2: // 180 degree rotation
            ledRow = NUM_ROWS - 1 - ledRow;
            ledCol = NUM_COLS - 1 - ledCol;
            break;
        case 3: // 270 degree rotation
            {
                int temp = ledRow;
                ledRow = NUM_ROWS - 1 - ledCol;
                ledCol = temp;
            }
            break;
    }

    // Apply flipping
    if (flipHorizontal) ledCol = NUM_COLS - 1 - ledCol;
    if (flipVertical) ledRow = NUM_ROWS - 1 - ledRow;
}

bool detectGridOrientation() {
    bool buttonPressed = false;
    // Scan to check which button is pressed
    for (int col = 0; col < NUM_COLS; col++) {
        digitalWrite(COL_PINS[col], LOW);
        for (int row = 0; row < NUM_ROWS; row++) {
            if (!digitalRead(ROW_PINS[row])) {
                buttonPressed = true;
                // Set grid rotation based on pressed button
                if (row == 0 && col == 0) {
                    gridRotation = 0;
                } else if (row == 0 && col == NUM_COLS - 1) {
                    gridRotation = 1;
                } else if (row == NUM_ROWS - 1 && col == NUM_COLS - 1) {
                    gridRotation = 2;
                } else if (row == NUM_ROWS - 1 && col == 0) {
                    gridRotation = 3;
                }
                digitalWrite(COL_PINS[col], HIGH);
                return buttonPressed;
            }
        }
        digitalWrite(COL_PINS[col], HIGH);
    }
    // If no button is pressed, set default orientation
    gridRotation = DEFAULT_ROTATION;
    return buttonPressed;
}

void setup() {
    USBDevice.setManufacturerDescriptor(mfgstr);
    USBDevice.setProductDescriptor(prodstr);
    USBDevice.setSerialDescriptor(serialstr);

    pinMode(LED_BUILTIN, OUTPUT);

    for (int i = 0; i < NUM_ROWS; i++) {
        pinMode(ROW_PINS[i], INPUT_PULLUP);
    }
    for (int i = 0; i < NUM_COLS; i++) {
        pinMode(COL_PINS[i], OUTPUT);
        digitalWrite(COL_PINS[i], HIGH);
    }

    detectGridOrientation();

    mdp.isMonome = true;
    mdp.deviceID = deviceID;
    mdp.setupAsGrid(NUM_ROWS, NUM_COLS);

    isInited = true;
    mdp.poll();

    // Send grid size and rotation information
    mdp.sendSysSize();
    mdp.sendSysRotation();
    pixels.begin();

    // Initialize MPU-6050
    /*Wire.setSCL(21);
    Wire.setSDA(20);
    Wire.begin();*/

    /*if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        while (1) {
            delay(10);
        }
    }
    Serial.println("MPU6050 Found!");

    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    // Set tilt sensor 0 as active
    mdp.setTiltActive(0, true);*/
}

void loop() {
    static unsigned long lastCheck = 0;
    static unsigned long lastTiltCheck = 0;
    unsigned long currentMillis = millis();

    mdp.poll();

    if (currentMillis - lastCheck >= 15) {
        lastCheck = currentMillis;
        scanButtonMatrix();
        updateLEDMatrix();
    }

    // Send tilt data every 100ms
    if (currentMillis - lastTiltCheck >= 100) {
        lastTiltCheck = currentMillis;
        //sendTiltData();
    }
}

void scanButtonMatrix() {
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            digitalWrite(COL_PINS[col], LOW);
            bool currentState = !digitalRead(ROW_PINS[row]);
            digitalWrite(COL_PINS[col], HIGH);

            int ledRow, ledCol;
            mapButtonToLED(row, col, ledRow, ledCol);

            if (currentState != buttonStates[row][col]) {
                mdp.sendGridKey(ledCol, ledRow, currentState);
                buttonStates[row][col] = currentState;
            }
        }
    }
}

void updateLEDMatrix() {
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            int ledRow, ledCol;
            mapButtonToLED(row, col, ledRow, ledCol);
            uint8_t intensity = gammaTable[mdp.leds[row * NUM_ROWS + col]] * gammaAdj;
            pixels.setPixelColor(ledRow * NUM_ROWS + ledCol, pixels.Color(intensity / 2, intensity, intensity / 2));
        }
    }
    pixels.show();
}

void sendTiltData() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Scale gyro data to 16-bit integer range
    int16_t x = (int16_t)(g.gyro.x * 1000);
    int16_t y = (int16_t)(g.gyro.y * 1000);
    int16_t z = (int16_t)(g.gyro.z * 1000);

    mdp.sendTiltEvent(0, x, y, z);
}