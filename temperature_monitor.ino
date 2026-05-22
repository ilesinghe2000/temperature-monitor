/*
 * ============================================
 *   Temperature & Humidity Monitor
 *   Arduino Uno + DHT11 + LCD 16x2 (I2C)
 * ============================================
 *
 * Components:
 *   - Arduino Uno
 *   - DHT11 sensor        → Digital pin 2
 *   - LCD 16x2 (I2C)      → SDA=A4, SCL=A5
 *   - 10kΩ resistor       → between DHT11 VCC and DATA pin
 *
 * Libraries needed (install via Arduino IDE > Library Manager):
 *   - "DHT sensor library" by Adafruit
 *   - "Adafruit Unified Sensor" by Adafruit
 *   - "LiquidCrystal I2C" by Frank de Brabander
 *
 * Author: Your Name
 * Date:   2025
 */

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Pin & sensor config ───────────────────────────────────────────────────────
#define DHTPIN  2        // DHT11 data pin
#define DHTTYPE DHT11    // Sensor model

// ── Object initialization ─────────────────────────────────────────────────────
DHT dht(DHTPIN, DHTTYPE);

// LCD address is usually 0x27 or 0x3F — try 0x27 first
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ── Custom degree symbol for LCD ──────────────────────────────────────────────
byte degreeSymbol[8] = {
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  Serial.println("=== Temperature & Humidity Monitor ===");

  // Start DHT sensor
  dht.begin();

  // Start LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degreeSymbol);  // register custom degree symbol at slot 0

  // Splash screen
  lcd.setCursor(0, 0);
  lcd.print("  Temp Monitor  ");
  lcd.setCursor(0, 1);
  lcd.print("  Starting...   ");
  delay(2000);
  lcd.clear();
}

// ── Main loop ─────────────────────────────────────────────────────────────────
void loop() {
  // DHT11 is slow — wait at least 2 seconds between readings
  delay(2000);

  float humidity    = dht.readHumidity();
  float tempC       = dht.readTemperature();       // Celsius
  float tempF       = dht.readTemperature(true);   // Fahrenheit

  // Check if reading failed
  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("ERROR: Failed to read from DHT11 sensor!");

    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    lcd.setCursor(0, 1);
    lcd.print("Check wiring... ");
    return;
  }

  // ── Print to Serial Monitor ───────────────────────────────────────────────
  Serial.print("Temperature: ");
  Serial.print(tempC, 1);
  Serial.print(" C  /  ");
  Serial.print(tempF, 1);
  Serial.println(" F");

  Serial.print("Humidity:    ");
  Serial.print(humidity, 1);
  Serial.println(" %");
  Serial.println("-------------------------------");

  // ── Display on LCD ────────────────────────────────────────────────────────
  // Row 0: Temperature
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempC, 1);
  lcd.write(byte(0));   // degree symbol
  lcd.print("C  ");

  // Row 1: Humidity
  lcd.setCursor(0, 1);
  lcd.print("Humi: ");
  lcd.print(humidity, 1);
  lcd.print("%    ");

  // ── Heat index (feels-like temperature) ──────────────────────────────────
  // Only meaningful above 27°C and 40% humidity
  if (tempC >= 27.0 && humidity >= 40.0) {
    float heatIndex = dht.computeHeatIndex(tempC, humidity, false);
    Serial.print("Heat Index:  ");
    Serial.print(heatIndex, 1);
    Serial.println(" C");
  }
}
