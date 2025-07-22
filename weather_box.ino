#include <Wire.h>                      // For I2C communication
#include <Adafruit_SSD1306.h>         // OLED display library
#include <Adafruit_GFX.h>             // Graphics support for OLED
#include <SFE_BMP180.h>               // BMP180 pressure/temperature sensor library
#include <DHT.h>                      // DHT11 humidity sensor library

// OLED display resolution
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED reset pin (not used with NodeMCU)
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// BMP180 sensor object
SFE_BMP180 bmp;

// DHT11 sensor setup
#define DHTPIN D5       // GPIO pin connected to DHT11 data
#define DHTTYPE DHT11   // Type of DHT sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize I2C with custom SDA (D2) and SCL (D1) pins for NodeMCU
  Wire.begin(D2, D1);

  Serial.begin(9600);
  while (!Serial);  // Wait for serial monitor (optional)

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // 0x3C is I2C address
    Serial.println("OLED not found");
    while (1);  // Stop if OLED not found
  }
  display.clearDisplay();  // Clear screen initially

  // Initialize DHT11 sensor
  dht.begin();

  // Initialize BMP180 sensor
  if (!bmp.begin()) {
    Serial.println("BMP180 failed");
  } else {
    Serial.println("BMP180 connected");
  }
}

void loop() {
  char status;
  double bmpTemp = 0, bmpPressure = 0;

  // Clear OLED before writing new data
  display.clearDisplay();
  display.setTextSize(1);            // Set text size
  display.setTextColor(WHITE);       // Set text color to white
  display.setCursor(0, 0);           // Start from top-left corner

  // === Read BMP180 Temperature ===
  status = bmp.startTemperature();
  if (status) {
    delay(status);                   // Wait for temperature reading
    status = bmp.getTemperature(bmpTemp);
  }

  // === Read BMP180 Pressure ===
  status = bmp.startPressure(3);     // 3 = high resolution
  if (status) {
    delay(status);                   // Wait for pressure reading
    status = bmp.getPressure(bmpPressure, bmpTemp);
  }

  // === Read DHT11 Humidity ===
  float dhtHumidity = dht.readHumidity();

  // === Display Temperature from BMP180 ===
  display.print("Temp     : ");
  display.print(bmpTemp, 2);         // 2 decimal places
  display.println(" C");

  // === Display Humidity from DHT11 ===
  display.print("Humidity : ");
  if (!isnan(dhtHumidity)) {
    display.print(dhtHumidity, 1);   // 1 decimal place
    display.println(" %");
  } else {
    display.println("N/A");         // Display error if sensor failed
  }

  // === Display Pressure from BMP180 ===
  display.print("Pressure : ");
  display.print(bmpPressure / 100, 2); // Convert Pa to hPa
  display.println(" hPa");

  // === Rain Prediction Logic Based on Pressure ===
  float pressure_hPa = bmpPressure / 100.0;
  int rainChancePercent;

  if (pressure_hPa > 1020.0) {
    rainChancePercent = 5;
  } else if (pressure_hPa > 1015.0) {
    rainChancePercent = 15;
  } else if (pressure_hPa > 1010.0) {
    rainChancePercent = 35;
  } else if (pressure_hPa > 1005.0) {
    rainChancePercent = 60;
  } else if (pressure_hPa > 1000.0) {
    rainChancePercent = 80;
  } else {
    rainChancePercent = 90;
  }

  // === Display Rain Chance Percentage ===
  display.print("Rain     : ");
  display.print(rainChancePercent);
  display.println(" %");

  // === Show Everything on OLED ===
  display.display();

  delay(3000);  // Wait 3 seconds before next update
}
