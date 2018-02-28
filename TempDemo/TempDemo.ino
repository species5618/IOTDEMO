#include <OneWire.h> 
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 
//MQTT
#include <PubSubClient.h>
// WIFI stuff
#include <ESP8266WiFi.h>


const char* ssid     = "MQTTDEMO";
const char* password = "Demo2018";
const char* configMQTT_Host ="mybroker.com";  //change to correct host
const int configMQTT_Port = 1883;  // change to correct port
const char* MQTT_ClientID="switchDemo";
const char* MQTT_User="MQTTUser"; //add user name here
const char* MQTT_Pass="MQTTPassword"; //add password here

float TempC ;
float last;
const String outTopic = "/control/dial/cmnd/pwm1";

//setup andconnect MQTT to WIFI
WiFiClient espClient;
PubSubClient client(espClient);

void MQTTreconnect() {
  // Loop until we're reconnected
  while (!client.connected() ) {
    Serial.println("Attempting MQTT connection...");
      client.connect(MQTT_ClientID, MQTT_User, MQTT_Pass);
      delay(1000);
  }
}

void setup() {
  // put your setup code here, to run once:
 // start serial port 
 Serial.begin(9600); 
 Serial.println("Dallas Temperature IC Control Library Demo"); 
 // Start up the library 
  sensors.begin(); 
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  //disconnect required here
  //improves reconnect reliability
  WiFi.disconnect(); 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
//wait for connection
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    delay(1000);
    Serial.print("*");
    WiFi.begin(ssid,password);
    WiFi.setAutoReconnect(true);
    WiFi.reconnect();
    }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

//set MQTT Server
client.setServer(configMQTT_Host, configMQTT_Port);
MQTTreconnect();
}

void loop() {
  MQTTreconnect();
  // put your main code here, to run repeatedly:
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 Serial.print(" Requesting temperatures..."); 
 sensors.requestTemperatures(); // Send the command to get temperature readings 
 Serial.println("DONE"); 
/********************************************************************/
// Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the fxirst IC on the wire 
 TempC = sensors.getTempCByIndex(0);
 Serial.print("Temperature is: "); 
 Serial.print(TempC);
// map(TempC, 0,70,0,1023 ) converts a number bwteen 0-70 to 0-1023
 String message = String(map(TempC, 10,35,0,1023),DEC);
 Serial.print("  Message is: "); 
 Serial.print(message);
//send to MQTT, note  convert topic and message to C string note Object String
if(TempC != last)
{
  // only publish i temp has changed
  last = TempC;
 client.publish(outTopic.c_str(), message.c_str());
  }
 // wait for 2 seconds
 delay(2000); 
}

