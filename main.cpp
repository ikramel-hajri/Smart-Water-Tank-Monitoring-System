#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin definitions
const int trigPin = 6;    // Ultrasonic sensor trigger pin
const int echoPin = 7;    // Ultrasonic sensor echo pin
const int greenLEDPin = 9; // Green LED pin
const int redLEDPin = 2;   // Red LED pin

// Threshold distance
const int thresholdDistance = 50;

// LCD settings
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  // Set pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);

  // Initialize the LCD
  lcd.init();
  lcd.clear();
  lcd.backlight();
}


void loop() {
  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo time and calculate distance in centimeters
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;

  // Calculate water level
  int tankHeight = 500; 
  int distance1 = tankHeight - distance;
  int percentage = (1 - (static_cast<float>(distance) / tankHeight)) * 100;


  // Display distance and water level on LCD
  
  lcd.clear(); // Clear the LCD screen
  lcd.setCursor(0, 0); // Set cursor to the beginning of the first line
  lcd.print("Distance: ");
  lcd.print(distance1);
  lcd.print("cm");

  lcd.setCursor(0, 1); // Set cursor to the beginning of the second line
  lcd.print("Percentage: ");
  lcd.print(percentage);
  lcd.print("%");

  // Control LEDs based on distance
  if (distance1 > thresholdDistance) {
    digitalWrite(greenLEDPin, HIGH);
    digitalWrite(redLEDPin, LOW);
  } else {
    digitalWrite(greenLEDPin, LOW);
    digitalWrite(redLEDPin, HIGH);
  }

  // Send telemetry via Serial
  Serial.println(distance1);


  // Add a delay to control the rate of distance measurements
  delay(500);
}
