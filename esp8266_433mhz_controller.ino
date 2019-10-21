/**
 * ESP8266 driven 433Mhz transmitter to control relays
 * Incuding DHT22 temp/humidity sensor
 * and possibility to control relays by clock
 * 
 * Toni Alho 2019
 */

// Confiq file for keeping WLAN credentials out from code and public repositories
#include "config.h"

// Include libraries ESP requires
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Include libraries for using Network Time Protocol
#include <NTPClient.h>
#include <WiFiUdp.h>

// Include the library required for sending 433Mhz messages
#include <RCSwitch.h>

// Include the DHT library and define DHT type
#include "DHT.h"
#define DHTTYPE DHT22

// Wifi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Define variables related to DHT
byte DHTPin = D7;
double temp;
double hum;

// UTC offset in seconds for Finland's timezone UTC +3 (incl daylight saving time)
const int utcOffset = 10800;

// BlinkWithoutDelay variables
const long updateInterval = 60000; // in milliseconds
unsigned long prevMillis = 0;

// Boolean token to tell if the lights have been switched on
bool lightsOn = 0;

// Create required objects
ESP8266WebServer server(80);
DHT dht(DHTPin, DHTTYPE);
RCSwitch transmitter = RCSwitch();
WiFiUDP ntpUDP;

// Create the NTPClient object with given parameters
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", utcOffset);

// Function prototypes
void handleRoot();
void handleRelay1On();
void handleRelay1Off();
void handleRelay2On();
void handleRelay2Off();
void handleRelay3On();
void handleRelay3Off();
void handleNotFound();


void setup() {
  // Start serial
  Serial.begin(115200);
  Serial.println("");

  // DHT setup
  pinMode(DHTPin, INPUT);
  dht.begin();
  Serial.println("DHT set up");

  // RCSwitch setup
  transmitter.enableTransmit(D8);
  transmitter.setProtocol(4);
  transmitter.setRepeatTransmit(10);
  Serial.println("433Mhz transmitter set up");
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");  
  Serial.println("Connected");
  Serial.print("IP address is: ");
  Serial.println(WiFi.localIP());

  // Callers for handlers
  server.on("/", handleRoot);
  server.on("/Relay1On", HTTP_POST, handleRelay1On);
  server.on("/Relay1Off", HTTP_POST, handleRelay1Off);
  server.on("/Relay2On", HTTP_POST, handleRelay2On);
  server.on("/Relay2Off", HTTP_POST, handleRelay2Off);
  server.on("/Relay3On", HTTP_POST, handleRelay3On);
  server.on("/Relay3Off", HTTP_POST, handleRelay3Off);
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Start the mDNS responder
  if (!MDNS.begin("ESP")) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

  // Start the NTP receiver
  timeClient.begin();
  Serial.println("NTP receiver started");
  
  // First read on DHT values
  temp = dht.readTemperature();
  hum = dht.readHumidity();   
  Serial.println("Read DHT");
}


void loop() {
  // Listen to client requests
  server.handleClient();

  // To keep mDNS working
  MDNS.update();

  // Millis counter (BlinkWithoutDelay)
  unsigned long currentMillis = millis();

  // Executed when defined update interval time has passed (once per minute)
  if(currentMillis - prevMillis > updateInterval){
    prevMillis = currentMillis;
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());

    // Temperature & humidity update
    temp = dht.readTemperature();
    hum = dht.readHumidity();   
    Serial.println("Read DHT");

    // Lights timing
    if(lightsOn == 0 && timeClient.getHours() == 9 && timeClient.getMinutes() == 15){
      handleRelay1On();
      handleRelay2On();
      lightsOn = 1;
    }

    // Reset the lightsOn soon after switching lights on
    if(timeClient.getHours() == 9 && timeClient.getMinutes() == 17){
      lightsOn = 0;
    }
  }
}


// =============================
// ==========HANDLERS===========
// =============================
void handleRoot(){
  // Compile and send the root
  String mes = "<head><meta name='apple-mobile-web-app-capable' content='yes' /><meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' /><meta charset=""UTF-8""><meta name=""viewport"" content=""width=device-width, initial-scale=1""></head><body><center><h2>ESP8266 Relay Controller</h2><br><h3><center>Lämpötila: </center></h3>";
  mes += temp;
  mes += "<h3>Kosteus: </h3>";
  mes += hum;
  mes += "<br><br><form action=\"/Relay1On\" method=\"POST\"><input type=\"submit\" value=\"Valonauha päälle\"></form><form action=\"/Relay1Off\" method=\"POST\"><input type=\"submit\" value=\"Valonauha pois\"></form><br><form action=\"/Relay2On\" method=\"POST\"><input type=\"submit\" value=\"Suolalamppu päälle\"></form><form action=\"/Relay2Off\" method=\"POST\"><input type=\"submit\" value=\"Suolalamppu pois\"></form><br><form action=\"/Relay3On\" method=\"POST\"><input type=\"submit\" value=\"Työpöytä päälle\"></form><form action=\"/Relay3Off\" method=\"POST\"><input type=\"submit\" value=\"Työpöytä pois\"></form></center></body>";
  server.send(200, "text/html", mes);
}

void handleRelay1On() {
  transmitter.send(13378884, 24);
  Serial.println("Light 1 ON"); 
  server.sendHeader("Location","/");
  server.send(303);
}

void handleRelay1Off() {
  transmitter.send(13165588, 24);
  Serial.println("Light 1 OFF"); 
  server.sendHeader("Location","/");
  server.send(303);
}

void handleRelay2On() {
  transmitter.send(13476284, 24);
  Serial.println("Light 2 ON"); 
  server.sendHeader("Location","/");
  server.send(303);
}

void handleRelay2Off() {
  transmitter.send(12861356, 24);
  Serial.println("Light 2 OFF"); 
  server.sendHeader("Location","/");
  server.send(303);
}

void handleRelay3On() {
  transmitter.send(12582946, 24);
  Serial.println("Light 3 ON"); 
  server.sendHeader("Location","/");
  server.send(303);
}

void handleRelay3Off() {
  transmitter.send(13228594, 24);
  Serial.println("Light 3 OFF"); 
  server.sendHeader("Location","/");
  server.send(303);
}

void handleNotFound(){
  // Send 404 when no handler found for request
  server.send(404, "text/plain", "404 not found");
}
