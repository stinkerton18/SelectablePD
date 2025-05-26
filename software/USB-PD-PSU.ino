/**************************************************************************
Code for the USB-PD to 5v/9v/12v/20v power adapter
Credit to Adafruit for both their HUSB238 breakout board and 0.91" OLED screen
HUSB238 - https://www.adafruit.com/product/5807
OLED Screen - https://www.adafruit.com/product/4440
 **************************************************************************/
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_HUSB238.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // Per the datasheet 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the USB PD adapter:
Adafruit_HUSB238 usbpd;

// These values are used to check if a button was pressed
int Button5v = 0;
int Button9v = 0;
int Button12v = 0;
int Button20v = 0;
// This is used simply to keep a timeout for the screen and blank it to extend the life of the OLEDs
int Timeout = 0;
// Defining the buttons to pins on an Uno/Nano ATmega328P. Adjust to match your board/wiring
const int ButtonPin5v = 5;
const int ButtonPin9v = 4;
const int ButtonPin12v = 3;
const int ButtonPin20v = 2;
// Define the LED/indicators for the currently enabled voltage. Adjust to match your board/wiring
const int LED5v = 9;
const int LED9v = 8;
const int LED12v = 7;
const int LED20v = 6;

void setup()
{
  pinMode(ButtonPin5v, INPUT);    //  5v button
  pinMode(ButtonPin9v, INPUT);    //  9v button
  pinMode(ButtonPin12v, INPUT);    // 12v button
  pinMode(ButtonPin20v, INPUT);    // 20v button
  pinMode(LED5v, OUTPUT);  //  5v LED/indicator
  pinMode(LED9v, OUTPUT);  //  9v LED/indicator
  pinMode(LED12v, OUTPUT);  // 12v LED/indicator
  pinMode(LED20v, OUTPUT);   // 20v LED/indicator

  // Start serial console
  Serial.begin(115200);
  while (!Serial) delay(10);
  Serial.println("Adafruit HUSB238 based selectable PD power supply starting");

  // Initialize the HUSB238
  if (usbpd.begin(HUSB238_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("HUSB238 USB-PD initialized successfully.");
  } else {
    Serial.println("Couldn't find HUSB238 USB-PD, check your wiring?");
    for(;;); // Don't proceed, loop forever
  }

  //Initialize the OLED display
  if(display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 OLED Screen initialized successfully.");
  } else {
    Serial.println("Couldn't find SSD1306 OLED Screen, check your wiring?");
    for(;;); // Don't proceed, loop forever
  }

  // On start, we set the voltage to 5v and set the appropriate indicator
  updateScreen('S', "5V");
  usbpd.selectPD(PD_SRC_5V);
  usbpd.requestPD();
  digitalWrite(LED5v, HIGH);
  Serial.println("Initialization complete");
}

void loop()
{
  // Read the state of the 5v pushbutton
  Button5v = digitalRead(ButtonPin5v);
  // Read the state of the 9v pushbutton  
  Button9v = digitalRead(ButtonPin9v);
  // Read the state of the 12v pushbutton
  Button12v = digitalRead(ButtonPin12v);
  // Read the state of the 20v pushbutton
  Button20v = digitalRead(ButtonPin20v);
  
  // Check which button is pressed
  if (Button5v == HIGH) {
    // Turn on the 5V LED and the others off
    digitalWrite(LED5v, HIGH);
    digitalWrite(LED9v, LOW);
    digitalWrite(LED12v, LOW);
    digitalWrite(LED20v, LOW);
    Serial.println("5V LED lit, others turned off");
    // Set the USB-PD board to 5V
    usbpd.selectPD(PD_SRC_5V);
    usbpd.requestPD();
    Serial.println("USB-PD Requested 5V");
    // Check if it was successful
    HUSB238_VoltageSetting selectedPD = usbpd.getPDSrcVoltage();
    Serial.println("USB-PD received " + selectedPD);
    if (selectedPD == PD_5V) {
      updateScreen('S', "5V");
      Timeout=0;
    } else {
      updateScreen('F',"0V");
    }
  }
  if (Button9v == HIGH) {
    // turn LED on
    digitalWrite(LED9v, HIGH);
    digitalWrite(LED5v, LOW);
    digitalWrite(LED12v, LOW);
    digitalWrite(LED20v, LOW);
    // Set the USB-PD board to 9V
    usbpd.selectPD(PD_SRC_9V);
    usbpd.requestPD();
    Serial.println("USB-PD Requested 9V");
    // Check if it was successful
    HUSB238_VoltageSetting selectedPD = usbpd.getPDSrcVoltage();
    Serial.println("USB-PD received " + selectedPD);
    if (selectedPD == PD_9V) {
      updateScreen('S', "9V");
      Timeout=0;
    } else {
      updateScreen('F',"0V");
    }
  }
  if (Button12v == HIGH) {
    // turn LED on
    digitalWrite(LED12v, HIGH);
    digitalWrite(LED5v, LOW);
    digitalWrite(LED9v, LOW);
    digitalWrite(LED20v, LOW);
    // Set the USB-PD board to 12V
    usbpd.selectPD(PD_SRC_12V);
    usbpd.requestPD();
    Serial.println("USB-PD Requested 12V");
    // Check if it was successful
    HUSB238_VoltageSetting selectedPD = usbpd.getPDSrcVoltage();
    Serial.println("USB-PD received " + selectedPD);
    if (selectedPD == PD_12V) {
      updateScreen('S', "12V");
      Timeout=0;
    } else {
      updateScreen('F',"0V");
    }
  }
  if (Button20v == HIGH) {
    // turn LED on
    digitalWrite(LED20v, HIGH);
    digitalWrite(LED5v, LOW);
    digitalWrite(LED9v, LOW);
    digitalWrite(LED12v, LOW);
    // Set the USB-PD board to 20V
    usbpd.selectPD(PD_SRC_20V);
    usbpd.requestPD();
    Serial.println("USB-PD Requested 20V");
    // Check if it was successful
    HUSB238_VoltageSetting selectedPD = usbpd.getPDSrcVoltage();
    Serial.println("USB-PD received " + selectedPD);
    if (selectedPD == PD_20V) {
      updateScreen('S', "20V");
      Timeout=0;
    } else {
      updateScreen('F',"0V");
    }
  }
  
  if (Timeout <= 1000) {
    Timeout++;
  } else {
    // We clear the display to help extend the life of the OLED screen
    display.clearDisplay();
  }
}

/*************************************************************** 
This function is for updating the OLED screen with a success (S)
and what voltage to display or if a failure (F) occurred.
***************************************************************/
void updateScreen(char result, String voltage) {
  switch(result) {
  case 'S':
    //Display the new voltage
    Serial.println("Updating display with new voltage");
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Voltage:  " + voltage);
    display.display();
    break;
  case 'F':
    //Display an error message
    Serial.println("Updating display with error message");
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Unsupported");
    display.setCursor(0, 1);
    display.println("Voltage");
    display.display();
  }
}