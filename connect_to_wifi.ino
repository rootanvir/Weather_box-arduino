#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

// DNS port and IP configuration for Access Point
const byte DNS_PORT = 53;
IPAddress apIP(192,168,4,1);  // Static IP for the AP

// DNS and Web Server instances
DNSServer dnsServer;
ESP8266WebServer server(80);

// AP credentials
const char* apSSID = "NodeMCU-Setup";
const char* apPassword = "12345678";

// Password prompt for webpage access
const String pagePassword = "1234";

// HTML handler for root path "/"
void handleRoot() {
  String html = R"=====(<!DOCTYPE html>
<html>
<head>
  <title>WiFi Setup</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    input { padding: 10px; font-size: 16px; width: 80%; max-width: 300px; margin: 5px; }
    button { padding: 10px 20px; font-size: 16px; }
  </style>
</head>
<body>
  <div id="main" style="display:none;">
    <h2>WiFi Configuration</h2>
    <form action="/connect" method="POST">
      <input name="ssid" placeholder="WiFi SSID"><br>
      <input name="pass" type="password" placeholder="WiFi Password"><br>
      <input type="submit" value="Connect">
    </form>
  </div>

  <script>
    var input = prompt("Enter password to access:");
    if (input === "1234") {
      document.getElementById("main").style.display = "block";
    } else {
      document.body.innerHTML = "<h3 style='color:red'>Access Denied</h3>";
    }
  </script>
</body>
</html>)=====";

  server.send(200, "text/html", html);  // Send the HTML page
}

// Handles the "/connect" route, receives SSID and password
void handleConnect() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  server.send(200, "text/html", "<h3>Connecting... Check Serial Monitor.</h3>");

  // Begin WiFi connection with received credentials
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Wait for connection with retry limit
  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  // Print connection result
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Failed to connect.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Set device as Access Point and configure static IP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(apSSID, apPassword);
  Serial.println("📡 AP Started");
  Serial.print("🔗 Go to http://");
  Serial.println(apIP);

  // Start DNS server to redirect all domains to AP IP (captive portal)
  dnsServer.start(DNS_PORT, "*", apIP);

  // Define web server routes
  server.on("/", handleRoot);            // Serve main page
  server.on("/connect", handleConnect);  // Handle WiFi form POST

  // Redirect any unknown route to root (for captive portal behavior)
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.begin();
  Serial.println("🌍 Web server started");
}

void loop() {
  // Handle DNS and HTTP client requests
  dnsServer.processNextRequest();
  server.handleClient();
}
