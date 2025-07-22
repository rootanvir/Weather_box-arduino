#include <ESP8266WiFi.h>          // WiFi functions for ESP8266
#include <ESP8266WebServer.h>     // Web server library
#include <DNSServer.h>            // DNS server for redirecting all domains

const byte DNS_PORT = 53;                  // DNS port
IPAddress apIP(192, 168, 4, 1);            // Static IP for the AP
DNSServer dns;                             // DNS server instance
ESP8266WebServer server(80);               // Web server running on port 80

// This is the main HTML page served on all routes
void handleRoot() {
  server.send(200, "text/html", "<h1>Hello, welcome to my captive portal!</h1>");
}

void setup() {
  Serial.begin(115200);

  // 1. Start Access Point with SSID and password
  WiFi.softAP("test005", "12345678");  // You can change the SSID and password
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));  // Set static IP
  delay(2000);  // Give time to stabilize

  Serial.println("Access Point started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // 2. Start DNS server to redirect all requests to our ESP IP
  dns.start(DNS_PORT, "*", apIP);

  // 3. Setup captive portal handlers for common OS-specific checks
  server.on("/", handleRoot);                          // Default route
  server.on("/generate_204", handleRoot);              // Android check
  server.on("/fwlink", handleRoot);                    // Windows check
  server.on("/hotspot-detect.html", handleRoot);       // Apple devices
  server.onNotFound(handleRoot);                       // Catch-all redirect

  // 4. Start the web server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  // Keep DNS and web server running
  dns.processNextRequest();
  server.handleClient();
}
