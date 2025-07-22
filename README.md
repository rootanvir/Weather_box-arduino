# Requirement
- ## Hardware 
  - Nodemcu ESP8266
  - DHT11
  - BMP180
  - I2C OLED Display
- ## Software and Library
  - ### Arduino ide
  - ### library for BMP180
     - Go to this github and [downlaod](https://github.com/sparkfun/BMP180_Breakout_Arduino_Library/tree/master) as zip
     - open your arduino ide (I am using version 2.3.2)
     - goto sketch > include library > Add .zip library
  - ### library for Ndemcu esp8266
    - [go to](https://www.instructables.com/Steps-to-Setup-Arduino-IDE-for-NODEMCU-ESP8266-WiF/) this websites and follow the instruction    
  - ### Library for DHT11
    - [Download](https://github.com/adafruit/DHT-sensor-library) from GitHub
    - Extract the ZIP file
    - Move the folder to your Arduino libraries directory:

        - Windows: Documents\Arduino\libraries\

        - Linux/Mac: ~/Arduino/libraries/
  - ### I2C OLED Display
    - Go to Sketch > Include Library > Manage Libraries…
    - Search for : Adafruit SSD1306
    - install it
# Wiring: NodeMCU + BMP180 + DHT11 + OLED SSD1306

---

##  BMP180 (Barometric Pressure Sensor)

| **Pin on BMP180** | **Connects to NodeMCU** | **Note**             |
|-------------------|--------------------------|----------------------|
| VCC               | VIN                      | 3.3V compatible      |
| GND               | GND                      | Ground               |
| SDA               | D2                       | I2C Data             |
| SCL               | D1                       | I2C Clock            |

---

##  DHT11 (Temperature & Humidity Sensor)

| **Pin on DHT11**  | **Connects to NodeMCU** | **Note**                        |
|-------------------|--------------------------|----------------------------------|
| VCC               | VIN                      | Use 5V (VIN)                    |
| GND               | GND                      | Ground                          |
| DATA              | D5                       | Digital data pin (GPIO14)       |

---

##  OLED SSD1306 (I2C Display)

| **Pin on OLED**   | **Connects to NodeMCU** | **Note**                              |
|-------------------|--------------------------|----------------------------------------|
| VCC               | VIN                      | 3.3V           |
| GND               | GND                      | Ground                                 |
| SDA               | D2                       | I2C Data (shared with BMP180)          |
| SCL               | D1                       | I2C Clock (shared with BMP180)         |
<img width="1842" height="1646" alt="Image" src="https://github.com/user-attachments/assets/142312f4-0df8-4f82-9bde-be3b72472fba" />

---

# Final 
![Image](https://github.com/user-attachments/assets/8198d71a-88df-44e2-8664-7f1dfc45b673)
