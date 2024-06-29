#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "<WIFI>";
const char* password = "<PASSWORD>";

// Create an instance of the server
WebServer server(80);

// Define the pin where the MQ-5 sensor is connected
const int gasSensorPin = 35;
const int gasThreshold = 130; // Set your desired threshold value in ppm

// Function to convert analog reading to gas concentration
float analogToPPM(int analogValue) {
  // Placeholder for conversion logic based on MQ-5 datasheet
  // This function needs to be updated with the actual conversion formula
  return (float)analogValue / 4095.0 * 1000.0; // Example: scaling to a 0-1000 range
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(9600);

  // Connect to Wi-Fi
  connectToWiFi();

  // Initialize the gas sensor pin as an input
  pinMode(gasSensorPin, INPUT);

  // Define route and handler for root and reading
  server.on("/", handleRoot);
  server.on("/reading", handleReading);
  server.on("/api", handleAPI);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    connectToWiFi();
  }

  server.handleClient();

  // Read gas sensor value
  int sensorValue = analogRead(gasSensorPin);
  float gasConcentration = analogToPPM(sensorValue);
}

void handleRoot() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">";
  html += "<title>Gas Sensor Readings</title>";
  html += "<style>body { font-family: Arial, sans-serif; text-align: center; }";
  html += "table { margin: auto; border-collapse: collapse; }";
  html += "td, th { border: 1px solid #ddd; padding: 8px; }";
  html += "th { padding-top: 12px; padding-bottom: 12px; text-align: center; background-color: #4CAF50; color: white; }</style>";
  html += "<script>function fetchReading() { fetch('/reading').then(response => response.text()).then(data => { document.getElementById('sensorReading').innerText = data; setTimeout(fetchReading, 1000); }); } window.onload = fetchReading;</script>";
  html += "</head><body><h1>Gas Sensor Readings</h1>";
  html += "<table><tr><th>Reading (ppm)</th></tr>";
  html += "<tr><td id='sensorReading'>Loading...</td></tr></table>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleReading() {
  int sensorValue = analogRead(gasSensorPin);
  float gasConcentration = analogToPPM(sensorValue);
  server.send(200, "text/plain", String(gasConcentration));
}

void handleAPI() {
  int sensorValue = analogRead(gasSensorPin);
  float gasConcentration = analogToPPM(sensorValue);
  String json = "{\"value\":" + String(gasConcentration) + "}";
  server.send(200, "application/json", json);
}