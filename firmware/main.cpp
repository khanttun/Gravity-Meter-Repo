#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

// --------------------------------------------------------------------------------
// CONFIGURATION - UPDATE THESE
// --------------------------------------------------------------------------------
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* ws_host  = "192.168.1.100"; // REPLACE WITH YOUR PC'S LOCAL IP ADDRESS
const int   ws_port  = 3000;

// --------------------------------------------------------------------------------
// PINS & GLOBALS
// --------------------------------------------------------------------------------
const int PIN_PUMP = 2; // Built-in LED or Pump Pin
const int PIN_MOISTURE = 34; // Analog pin for moisture sensor (or simulate)

WebSocketsClient webSocket;
unsigned long lastMoistureSend = 0;
int currentMoisture = 100; // Simulated 0-100

// --------------------------------------------------------------------------------
// WEBSOCKET EVENTS
// --------------------------------------------------------------------------------
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED: {
            Serial.printf("[WSc] Connected to url: %s\n", payload);
            }
            break;
        case WStype_TEXT: {
            Serial.printf("[WSc] get text: %s\n", payload);

            // Parse JSON
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, payload);

            const char* msgType = doc["type"];
            
            // Handle 'pump' or 'water' command
            // In our server logic we actually just update moisture, but let's handle "water"
            // The server currently sends "moisture" broadcasts. 
            // If the server forwards a "water" command specifically:
            if (String(msgType) == "water" || String(msgType) == "pump") {
                Serial.println("Activating Pump!");
                digitalWrite(PIN_PUMP, HIGH);
                delay(2000); // Pump on for 2 seconds
                digitalWrite(PIN_PUMP, LOW);
                
                // Reset moisture after watering
                currentMoisture = 100;
            }
            
            // If we receive a moisture update from the server (e.g. from the web button)
            if (String(msgType) == "moisture") {
                 int val = doc["value"];
                 currentMoisture = val;
                 Serial.printf("Moisture updated from server: %d\n", val);
            }

            }
            break;
    }
}

// --------------------------------------------------------------------------------
// SETUP
// --------------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    pinMode(PIN_PUMP, OUTPUT);
    digitalWrite(PIN_PUMP, LOW);

    // Connect WiFi
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Connect WebSocket
    webSocket.begin(ws_host, ws_port, "/");
    webSocket.onEvent(webSocketEvent);
    // Try to reconnect every 5s if failed
    webSocket.setReconnectInterval(5000);
}

// --------------------------------------------------------------------------------
// LOOP
// --------------------------------------------------------------------------------
void loop() {
    webSocket.loop();

    // Simulate drying out (approx every 1 sec -> -1%)
    // In real life, you'd read analogRead(PIN_MOISTURE) and map it.
    static unsigned long lastDry = 0;
    if (millis() - lastDry > 1000) {
        lastDry = millis();
        if (currentMoisture > 0) currentMoisture--;
    }

    // Send update to server every 2 seconds
    if (millis() - lastMoistureSend > 2000) {
        lastMoistureSend = millis();
        
        // JSON payload
        String json = "{\"type\":\"moisture_update\", \"value\":" + String(currentMoisture) + "}";
        webSocket.sendTXT(json);
        Serial.print("Sent: "); Serial.println(json);
    }
}
