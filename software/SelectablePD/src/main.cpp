/**************************************************************************
Code for the USB-PD to 5v/9v/12v/20v power adapter
Credit to Adafruit for both their HUSB238 breakout board and 0.91" OLED screen
HUSB238 - https://www.adafruit.com/product/5807
OLED Screen - https://www.adafruit.com/product/4440
 **************************************************************************/
#include <Arduino.h>
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

// A variable to help deter/avoid button bouncing
bool buttonPressed = false;

/*************************************************************** 
This function is for updating the OLED screen with a success (S)
and what voltage to display or if a failure (F) occurred.
***************************************************************/
void updateScreen(char result, String voltage) {
  // Ensure the display is cleared before updating
  display.clearDisplay();
  switch(result) {
  case 'S':
    // Display the new voltage
    Serial.println("Updating display with new voltage");
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Voltage:  " + voltage);
    display.display();
    break;
  case 'F':
    // Display an error message
    Serial.println("Updating display with error message");
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Unsupported");
    display.display();
    break;
  }
}

/*************************************************************** 
This function is for updating the USB-PD board to the requested voltage
***************************************************************/
void setvoltage (int voltage) {
  switch(voltage) {
    case 5:
      usbpd.selectPD(PD_SRC_5V);
      Serial.println("USB-PD Requested 5V");
      usbpd.requestPD();
      // Add a small delay to allow the PD request to process
      //delayMicroseconds(500);
      break;
    case 9:
      usbpd.selectPD(PD_SRC_9V);
      Serial.println("USB-PD Requested 9V");
      usbpd.requestPD();
      // Add a small delay to allow the PD request to process
      //delayMicroseconds(500);
      break;
    case 12:
      usbpd.selectPD(PD_SRC_12V);
      Serial.println("USB-PD Requested 12V");
      usbpd.requestPD();
      // Add a small delay to allow the PD request to process
      //delayMicroseconds(500);
      break;
    case 20:
      usbpd.selectPD(PD_SRC_20V);
      Serial.println("USB-PD Requested 20V");
      usbpd.requestPD();
      // Add a small delay to allow the PD request to process
      //delayMicroseconds(500);
      break;
    default:
      Serial.println("Unsupported voltage selection");
      break;
  }
}

/***************************************************************
 * This function checks the voltage requested by the PD board
 * and updates the screen accordingly
 ***************************************************************/
void checkvoltage () {
  HUSB238_PDSelection srcVoltage = usbpd.getSelectedPD();
  Serial.println("USB-PD received " + srcVoltage);
  switch (srcVoltage)
  {
    case PD_SRC_5V:
      updateScreen('S', "5V");
      Timeout=0;
      break;
    case PD_SRC_9V:
      updateScreen('S', "9V");
      Timeout=0;
      break;
    case PD_SRC_12V:
      updateScreen('S', "12V");
      Timeout=0;
      break;
    case PD_SRC_20V:
      updateScreen('S', "20V");
      Timeout=0;
      break;
    default:
      updateScreen('F',"0V");
      Serial.println("Voltage request failed");
      Timeout=0;
      break;
  }
}

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
//  Serial.begin(115200);
  Serial.begin(9600);
  while (!Serial) delay(10);
  Serial.println("Adafruit HUSB238 based selectable PD power supply starting");

  // Initialize the HUSB238
  if (usbpd.begin(HUSB238_I2CADDR_DEFAULT, &Wire)) {
    Serial.println("HUSB238 USB-PD initialized successfully.");
  } else {
    Serial.println("Couldn't find HUSB238 USB-PD, check your wiring?");
    for(;;); // Don't proceed, loop forever
  }

  // Initialize the OLED display
  if(display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 OLED Screen initialized successfully.");
  } else {
    Serial.println("Couldn't find SSD1306 OLED Screen, check your wiring?");
    for(;;); // Don't proceed, loop forever
  }

  // On start, we set the voltage to 5v and set the appropriate indicator
  updateScreen('S', "5V");
  setvoltage(5);
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

  if (buttonPressed) {
    // If a button was already pressed, wait until all are released
    if (Button5v == LOW && Button9v == LOW && Button12v == LOW && Button20v == LOW) {
      buttonPressed = false; // Reset the flag when all buttons are released
    } else {
      return; // Exit the loop early to avoid multiple triggers
    }
  } else {
    // If no button was pressed, check if any button is currently pressed
    if (Button5v == HIGH || Button9v == HIGH || Button12v == HIGH || Button20v == HIGH) {
      buttonPressed = true; // Set the flag to indicate a button press
    }
  }
  
  // Check which button is pressed
  if (Button5v == HIGH) {
    // Turn on the 5V LED and the others off
    digitalWrite(LED5v, HIGH);
    digitalWrite(LED9v, LOW);
    digitalWrite(LED12v, LOW);
    digitalWrite(LED20v, LOW);
    Serial.println("5V LED lit, others turned off");
    // Set the USB-PD board to 5V
    setvoltage(5);
    // Check if it was successful
    checkvoltage();
  }
  
  if (Button9v == HIGH) {
    // Turn LED on
    digitalWrite(LED9v, HIGH);
    digitalWrite(LED5v, LOW);
    digitalWrite(LED12v, LOW);
    digitalWrite(LED20v, LOW);
    Serial.println("9V LED lit, others turned off");
    // Set the USB-PD board to 9V
    setvoltage(9);
    // Check if it was successful
    checkvoltage();
  }
  
  if (Button12v == HIGH) {
    // Turn LED on
    digitalWrite(LED12v, HIGH);
    digitalWrite(LED5v, LOW);
    digitalWrite(LED9v, LOW);
    digitalWrite(LED20v, LOW);
    Serial.println("12V LED lit, others turned off");
    // Set the USB-PD board to 12V
    setvoltage(12);
    // Check if it was successful
    checkvoltage();
  }
  
  if (Button20v == HIGH) {
    // Turn LED on
    digitalWrite(LED12v, LOW);
    digitalWrite(LED5v, LOW);
    digitalWrite(LED9v, LOW);
    digitalWrite(LED20v, HIGH);
    Serial.println("20V LED lit, others turned off");
    // Set the USB-PD board to 20V
    setvoltage(20);
    // Check if it was successful
    checkvoltage();
  }
  
  if (Timeout <= 3000) {
    Timeout++;
  } else {
    // We clear the display to help extend the life of the OLED screen
    display.clearDisplay();
  }
}