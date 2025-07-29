#include <SPI.h>
#include <LoRa.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// LCD setup (16x2 I2C)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// GPS setup
SoftwareSerial gpsSerial(4, 3); // RX, TX

// Pins
const int metalSensorPin = 7;
const int trigPin = 8;
const int echoPin = 9;
const int buzzerPin = 10;
const int ledPin = 11;

// Motor pins 
const int in1 = 5;
const int in2 = 6;
const int in3 = A0;
const int in4 = A1;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  lcd.begin();
  lcd.backlight();

  LoRa.begin(433E6);  // Change frequency as per region

  pinMode(metalSensorPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
}

void loop() {
  long duration, distance;

  // Ultrasonic distance
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Read metal detection
  int metalDetected = digitalRead(metalSensorPin);

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm");

  if (metalDetected == HIGH) {
    lcd.setCursor(0, 1);
    lcd.print("Mine Detected!");
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);

    // Get GPS data (simplified)
    String gpsData = "";
    while (gpsSerial.available()) {
      gpsData += char(gpsSerial.read());
    }

    // Send via LoRa
    LoRa.beginPacket();
    LoRa.print("Mine Detected at: ");
    LoRa.print(distance);
    LoRa.print("cm | GPS: ");
    LoRa.print(gpsData);
    LoRa.endPacket();

    delay(3000);
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
    lcd.clear();
  }

  delay(500);
}
