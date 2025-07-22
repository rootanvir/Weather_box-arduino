#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "Dot...Internet";
const char* password = "76757473&";

// OpenWeatherMap API info
const char* apiKey = "ed72eda3136dc18b0446b16b64063c1e";
const char* city = "Dhaka";
const char* countryCode = "BD";

ESP8266WebServer server(80); // Web server on port 80
String weatherHTML = "Loading..."; // HTML content to serve

// Convert Unix timestamp to 12-hour time string with AM/PM, adjusted by timezone offset (seconds)
String unixToTimeString(long unixTime, int timezoneOffset) {
  unixTime += timezoneOffset; // Adjust for timezone

  int hours24 = (unixTime % 86400L) / 3600;
  int minutes = (unixTime % 3600) / 60;
  int seconds = unixTime % 60;

  // Convert to 12-hour format
  int hours12 = hours24 % 12;
  if (hours12 == 0) hours12 = 12;

  // AM or PM
  const char* ampm = (hours24 >= 12) ? "PM" : "AM";

  char buffer[12];
  snprintf(buffer, sizeof(buffer), "%02d:%02d:%02d %s", hours12, minutes, seconds, ampm);
  return String(buffer);
}

// Fetch weather data and update weatherHTML
void fetchWeather() {
  WiFiClient client;
  HTTPClient http;

  // Build API URL
  String url = "http://api.openweathermap.org/data/2.5/weather?q=" + String(city) + "," + countryCode + "&appid=" + apiKey + "&units=metric";

  http.begin(client, url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
      // Extract main data
      float temp = doc["main"]["temp"];
      float feels = doc["main"]["feels_like"];
      int humidity = doc["main"]["humidity"];
      int pressure = doc["main"]["pressure"];
      int temp_min = doc["main"]["temp_min"];
      int temp_max = doc["main"]["temp_max"];

      // Weather condition and icon
      const char* condition = doc["weather"][0]["main"];
      const char* desc = doc["weather"][0]["description"];
      const char* icon = doc["weather"][0]["icon"];

      // Clouds percentage
      int clouds = doc["clouds"]["all"];

      // Wind info
      float wind_speed = doc["wind"]["speed"];
      int wind_deg = doc["wind"]["deg"];
      float wind_gust = doc["wind"]["gust"] | 0.0;

      // Visibility
      int visibility = doc["visibility"];

      // Coordinates
      float lon = doc["coord"]["lon"];
      float lat = doc["coord"]["lat"];

      // Sys info
      long sunrise = doc["sys"]["sunrise"];
      long sunset = doc["sys"]["sunset"];
      const char* country = doc["sys"]["country"];
      int timezone = doc["timezone"];
      long dt = doc["dt"];

      // Build HTML content
      weatherHTML = "<h2>Weather in " + String(city) + ", " + String(country) + " (" + String(lat, 6) + "," + String(lon, 6) + ")</h2>";
      weatherHTML += "<ul>";
      weatherHTML += "<li>Temperature: " + String(temp) + " &deg;C (min: " + String(temp_min) + ", max: " + String(temp_max) + ")</li>";
      weatherHTML += "<li>Feels Like: " + String(feels) + " &deg;C</li>";
      weatherHTML += "<li>Humidity: " + String(humidity) + " %</li>";
      weatherHTML += "<li>Pressure: " + String(pressure) + " hPa</li>";
      weatherHTML += "<li>Condition: " + String(condition) + " (" + String(desc) + ") <img src='http://openweathermap.org/img/wn/" + String(icon) + "@2x.png' alt='icon'></li>";
      weatherHTML += "<li>Clouds: " + String(clouds) + " %</li>";
      weatherHTML += "<li>Visibility: " + String(visibility) + " meters</li>";
      weatherHTML += "<li>Wind Speed: " + String(wind_speed) + " m/s</li>";
      weatherHTML += "<li>Wind Direction: " + String(wind_deg) + " &deg;</li>";
      weatherHTML += "<li>Wind Gust: " + String(wind_gust) + " m/s</li>";
      weatherHTML += "<li>Sunrise: " + unixToTimeString(sunrise, timezone) + "</li>";
      weatherHTML += "<li>Sunset: " + unixToTimeString(sunset, timezone) + "</li>";
      weatherHTML += "<li>Timezone offset from UTC: " + String(timezone) + " seconds</li>";
      weatherHTML += "<li>Data timestamp: " + unixToTimeString(dt, timezone) + "</li>";
      weatherHTML += "</ul>";
    } else {
      weatherHTML = "<p>JSON parse failed!</p>";
    }
  } else {
    weatherHTML = "<p>HTTP GET failed: " + String(httpCode) + "</p>";
  }

  http.end();
}

// Serve the root page with weather info
void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Weather Info</title></head><body>";
  html += weatherHTML;
  html += "<p>Refreshes every 10 seconds</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  fetchWeather();

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

unsigned long lastUpdate = 0;

void loop() {
  server.handleClient();

  if (millis() - lastUpdate > 10000) {  // Refresh every 10 seconds
    fetchWeather();
    lastUpdate = millis();
  }
}
