//NodeMCU
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire 
#define ONE_WIRE_BUS 4   //D2 pin of nodemcu

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

//define SSID, password, mqtt server
extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}



//define SSID, password, mqtt server
static const char* ssid = "spsvos-student";
static const char* username = "iot";
static const char* password = "Heslo123";

const char* mqtt_server = "connect.hexio.cloud";
const char* mqtt_user = "5l6jGB04"; //https://grafana.hexio.cloud/login - sps.zaci passwd:SPSvos123
const char* mqtt_password = "c647S61AK4";

const char* outTopic = "sensor01";
float temp;
const char* msgtemplate = "[{\"n\":\"teplota\",\"v\":%d}]";

char msgEvent[80] = "[{\"n\":\"teplota\",\"v\":23}]"; // znakový řetězec znaků naměřené hodnoty

WiFiClient espClient;
PubSubClient client(espClient);
/*
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.print("password: ");
  Serial.println(password);  
    // Wait for connection AND IP address from DHCP
  Serial.println();
  Serial.println("Waiting for connection and IP Address from DHCP");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
*/
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  Serial.print("username: ");
  Serial.println(username);
  Serial.print("password: ");
  Serial.println(password);  
  // WPA2 Connection starts here
  // Setting ESP into STATION mode only (no AP mode or dual mode)
    wifi_set_opmode(STATION_MODE);
    struct station_config wifi_config;
    memset(&wifi_config, 0, sizeof(wifi_config));
    strcpy((char*)wifi_config.ssid, ssid);
    wifi_station_set_config(&wifi_config);
    wifi_station_clear_cert_key();
    wifi_station_clear_enterprise_ca_cert();
    wifi_station_set_wpa2_enterprise_auth(1);
    wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
    wifi_station_set_enterprise_username((uint8*)username, strlen(username));
    wifi_station_set_enterprise_password((uint8*)password, strlen(password));
    wifi_station_connect();
  // WPA2 Connection ends here
  // Wait for connection AND IP address from DHCP
  Serial.println();
  Serial.println("Waiting for connection and IP Address from DHCP");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    String clientId = "ESP8266Client";
    clientId += String(random(0xffff), HEX);
    
   if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
 // if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  //initialize Serial Monitor
  Serial.begin(74880);
  while (!Serial);
  Serial.println("\WiFi-Honzuv, Dallas-mqtt sender-Grafana \n");

  // Start up the library
  sensors.begin();
  sensors.setResolution(12); 

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  delay(100);
Serial.println("\n mqtt_server: ");
Serial.print("mqtt_user: ");
  Serial.println(mqtt_user);
  Serial.print("mqtt_password: ");
  Serial.println(mqtt_password);
}

void loop() {

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  temp = sensors.getTempCByIndex(0);
  // After we got the temperatures, we can print them here.
  // We use the function ByIndex, and as an example get the temperature from the first sensor only.
 Serial.print("Temperature for the device 1 (index 0) is: ");
  Serial.println((int)(temp*100));
 
  // zaslání topic
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
    Serial.print("Publish Topic: ");
    Serial.print(outTopic);
    Serial.print(", msgEvent:");
    sprintf(msgEvent, msgtemplate, (int)(temp*100));
    Serial.println(msgEvent);
    client.publish(outTopic, msgEvent);
//    WiFi.disconnect(true); 
    delay (300000);    
}
