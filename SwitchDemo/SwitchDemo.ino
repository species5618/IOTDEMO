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
int input ;
const String outTopic = "/control/plug/cmnd/power";
int last=0;
#define inputpin D0 // 

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

//set input pin
pinMode(inputpin, INPUT_PULLUP);

}

void loop() {
  MQTTreconnect();    
  // put your main code here, to run repeatedly:
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
  Serial.println("Requesting Reading..."); 
 input  = digitalRead(inputpin);
 Serial.print("Reading is: "); 
 Serial.print(input);
 // try testing value of input and if > a value send message to plug !!!!
 
 String message = "";
 if ( input != last)
 {
     last = input;
     if(input ==0) //test button
     {
      message = "OFF";
     }
     else
     {
      message = "ON";
      }
 Serial.print("  Message is: "); 
 Serial.print(message);
//send to MQTT, note  convert topic and message to C string note Object String
 client.publish(outTopic.c_str(), message.c_str());  
  }

 // wait for 0.1 seconds
 delay(250); 
}
