#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(192,168,4,1);
DNSServer dnsServer;
ESP8266WebServer server(80);

const char* apSSID = "NodeMCU-Setup";
const char* apPassword = "12345678";
const String pagePassword = "1234";

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

  server.send(200, "text/html", html);
}

void handleConnect() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  server.send(200, "text/html", "<h3>Connecting... Check Serial Monitor.</h3>");
  WiFi.begin(ssid.c_str(), pass.c_str());
  Serial.print("Connecting to ");
  Serial.println(ssid);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

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

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(apSSID, apPassword);
  Serial.println("📡 AP Started");
  Serial.print("🔗 Go to http://");
  Serial.println(apIP);

  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.on("/connect", handleConnect);
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });

  server.begin();
  Serial.println("🌍 Web server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
