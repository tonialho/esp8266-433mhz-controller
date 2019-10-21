/**
 * ESP8266 driven 433Mhz transmitter to control relays
 * Incuding DHT22 temp/humidity sensor
 * and possibility to control lights by clock
 * 
 * Toni Alho 2019
 * With some help of https://tttapa.github.io/ESP8266/Chap10%20-%20Simple%20Web%20Server.html
 */

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
const char* ssid = "Koti527";
const char* password = "SFLVTTGTAE4TK";

// Create server object listening to defined port
ESP8266WebServer server(80);

// Define variables related to DHT
byte DHTPin = D7;
double temp;
double hum;

// Boolean to help controlling time controlled lights switch
bool lightsOn = 0;

// Create DHT object
DHT dht(DHTPin, DHTTYPE);

// Create RCSwitch object
RCSwitch transmitter = RCSwitch();

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

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

  // DHT
  pinMode(DHTPin, INPUT);
  dht.begin();

  // RCSwitch setup
  transmitter.enableTransmit(D8);
  transmitter.setProtocol(4);
  transmitter.setRepeatTransmit(10);

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

  // Save DHT output to according variables
  temp = dht.readTemperature();
  hum = dht.readHumidity();   
  Serial.println("Read DHT");

  // Start the mDNS responder
  if (!MDNS.begin("ESP")) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

  // Start the NTP receiver
  timeClient.begin();
}


void loop() {
  // Listen to client requests
  server.handleClient();

  // To keep mDNS working
  MDNS.update();

  // Update time
  timeClient.update();

  // Lights timing
  if(lightsOn == 0 && timeClient.getHours() == 7 && timeClient.getMinutes() == 15){
    handleRelay1On();
    handleRelay2On();
    lightsOn = 1;
  }

  // Reset the lightsOn
  if(timeClient.getHours() == 7 && timeClient.getMinutes() == 17){
    lightsOn = 0;
  }
}


// =============================
// ==========HANDLERS===========
// =============================
void handleRoot(){
  // Save DHT output to according variables
  temp = dht.readTemperature();
  hum = dht.readHumidity();   
  Serial.println("Read DHT");

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
  server.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
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
