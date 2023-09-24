#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int motorPin = 3;
const int speakerPin = 4;
const int reedSwitchPin = 5;
const int resetButtonPin = 6;
const int pirSensorPin = 2;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Address for a 16x2 I2C LCD

bool lidOpen = false;
unsigned long lastFeedTime = 0;
unsigned int bowlsOfFoodLeft = 10;

bool resetButtonPressed = false;
unsigned long lastResetButtonPressTime = 0;
const unsigned long buttonPressTimeout = 5000;

void setup() {
  // Initialize components
  pinMode(motorPin, OUTPUT);
  pinMode(speakerPin, OUTPUT);
  pinMode(reedSwitchPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(pirSensorPin, INPUT);

  // Initialize the LCD
  lcd.begin(16, 2);  // Assuming a 16x2 LCD
}

void loop() {
  // Check if the reset button is pressed
  if (digitalRead(resetButtonPin) == LOW) {
    if (!resetButtonPressed) {
      resetButtonPressed = true;
      lastResetButtonPressTime = millis();
      lcd.clear();
      lcd.print("Press again to");
      lcd.setCursor(0, 1);
      lcd.print("reset counter");
    } else {
      if (millis() - lastResetButtonPressTime <= buttonPressTimeout) {
        bowlsOfFoodLeft = 10;
        lcd.clear();
        lcd.print("Counter reset");
        delay(2000);
      } else {
        resetButtonPressed = false;
        lcd.clear();
      }
    }
  } else {
    resetButtonPressed = false;
    lcd.clear();
  }

  checkLidStatus();
  if (isFeedingTime() && !lidOpen) {
    dispenseFood();
    playSound();
    updateLastFeedTime();
    bowlsOfFoodLeft--;
  }
  updateLCD();
}

void checkLidStatus() {
  if (digitalRead(reedSwitchPin) == HIGH) {
    lidOpen = true;
    lcd.clear();
    lcd.print("Lid Open");
  } else {
    lidOpen = false;
  }
}

bool isFeedingTime() {
  unsigned long currentTime = millis();
  return (currentTime - lastFeedTime >= 6UL * 3600UL * 1000UL);
}

void dispenseFood() {
  digitalWrite(motorPin, HIGH);
  delay(1000);
  digitalWrite(motorPin, LOW);
}

void playSound() {
  tone(speakerPin, 1000, 1000);
}

void updateLCD() {
  lcd.clear();
  lcd.print("Bowls left: ");
  lcd.print(bowlsOfFoodLeft);
  lcd.setCursor(0, 1);
  if (!lidOpen) {
    lcd.print("Next feeding:");
    lcd.setCursor(0, 1);
    unsigned long timeToNextFeeding = (lastFeedTime + 6UL * 3600UL * 1000UL) - millis();
    int hours = timeToNextFeeding / 3600000;
    int minutes = (timeToNextFeeding % 3600000) / 60000;
    lcd.print(hours);
    lcd.print("h ");
    lcd.print(minutes);
    lcd.print("m");
  }
}

void updateLastFeedTime() {
  lastFeedTime = millis();
}
