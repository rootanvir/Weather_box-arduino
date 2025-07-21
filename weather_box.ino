#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SFE_BMP180.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SFE_BMP180 bmp;
#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Wire.begin(D2, D1);  // SDA, SCL for NodeMCU
  Serial.begin(9600);
  while (!Serial);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found");
    while (1);
  }
  display.clearDisplay();

  dht.begin();

  if (!bmp.begin()) {
    Serial.println("BMP180 failed");
  } else {
    Serial.println("BMP180 connected");
  }
}

void loop() {
  char status;
  double bmpTemp, bmpPressure;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  // Read BMP180 Temperature
  status = bmp.startTemperature();
  if (status) {
    delay(status);
    status = bmp.getTemperature(bmpTemp);
  }

  // Read BMP180 Pressure
  status = bmp.startPressure(3);
  if (status) {
    delay(status);
    status = bmp.getPressure(bmpPressure, bmpTemp);
  }

  // Read DHT11 Humidity
  float dhtHumidity = dht.readHumidity();

  // Print Temperature from BMP180
  display.print("Temp     : ");
  display.print(bmpTemp, 2);
  display.println(" C");

  // Print Humidity from DHT11
  display.print("Humidity : ");
  if (!isnan(dhtHumidity)) {
    display.print(dhtHumidity, 1);
    display.println(" %");
  } else {
    display.println("N/A");
  }

  // Print Pressure from BMP180
  display.print("Pressure : ");
  display.print(bmpPressure / 100, 2); // Pa to hPa
  display.println(" hPa");

  // Calculate Rain Chance % based on pressure and print
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

  display.print("Rain     : ");
  display.print(rainChancePercent);
  display.println(" %");

  display.display();
  delay(3000);
}
