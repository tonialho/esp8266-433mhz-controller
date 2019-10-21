**ESP8266 home automation controller**

My aim is to build some automation and "smartness" to my apartment using ESP8266 and various sensors and other components. The system should be accessible with any device connected to my home WLAN, and later on via Internet if purposeful. 

Files:
- esp8266_433mhz_controller.ino the source code
- index.html a separate html file for easier editing of the UI code
- readme.md this file

Features so far:
- HTML based UI
 - Minimum viable design 
 - Accessible with any device in same WLAN
- 433Mhz transmitter 
 - Able to control factory-made remote AC relays (in this case [CoTech 36-6010](https://www.clasohlson.com/fi/Kaukokytkin-3-kpl-Cotech-Smart-Home/p/Pr366010000))
 _- Expandable to control other common 433Mhz receivers_
- DHT22 sensor
 - Read temperature and humidity values, present in UI
- NTP clock
 - Clock control for relays (hard coded)



TODO:
- Info about apartment's status 
 - Status of "at home/not home" switch, which works as a main switch for fixed lights and kitchen power sockets
 - Outside door usage log
- Prettier UI
 - Requires practical way to write and use CSS and JavaScript on ESP8266 (instead of writing it into the .ino file
 - Graphical presentation of temperature and humidity
- Programmable clock control via UI
 - Possibility to choose a relay and the time it switches
- Additional heater control
 - Student apartments are not known for excessive nor controllable heating, an extra relay controlled radiator would allow me to adjust room temperature a bit via UI or automation (for instance, one extra degree celcius in mornings)
 

In vision:
- Fridge cam
- Voice control 
- Fixed touch screen intregration with personal assistant features