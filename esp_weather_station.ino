#include <ESP8266WiFi.h>
#include "DHT.h"

//*********DHT Settings********
#define DHT_PIN 12              //Set GPIO12 as DHT11 data pin
#define DHTTYPE DHT11           //Set type of DHT sensor

DHT dht(DHT_PIN, DHTTYPE, 15);
float hum;
float temp;


//******WiFi settings**********
const char* ssid     = "SSID";          //Enter here WiFi SSID
const char* password = "password";      //Enter here WiFi Password
WiFiClient client;    

//****GadgetKeeper settings*****
/*  To obtain the Thing Id, see instructions http://docs.gadgetkeeper.com/display/docs/Thing+Id */
String thing_ID = "Thing_Id";

/*  To obtain the Thing Id, see instructions http://docs.gadgetkeeper.com/display/docs/Create%2C+Read%2C+and+Write+Property */
String propertyID_temp = "Temperature_Property_Id";
String propertyID_hum = "Humidity_Property_Id";

/*  To obtain the API Key see instructions http://docs.gadgetkeeper.com/display/docs/API+Keys */
String api_key = "API_Key";

const char* host = "api.gadgetkeeper.com";

String temp_url = "/v1/things/" + thing_ID + "/properties/" + propertyID_temp + "/value.json";

String hum_url = "/v1/things/" + thing_ID + "/properties/" + propertyID_hum + "/value.json";

void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);

  
  // We start by connecting to a WiFi network
  Serial.println();
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

  
}

void loop() {
  
  delay(4000);
  
  if(getDht() == true){

    //first we connect to GadgetKeeper Cloud Platform
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
      Serial.println("connection failed");
      return;
    }

    sendHum();
    delay(500);
    sendTemp();


    //**********************************
    //here we get feedback from API Call
    while(client.available()){
      String line = client.readStringUntil('\r');
      Serial.print(line);
    }
  }
  
}

//###########Send Humidity Data####################
void sendHum(){
    client.print("PUT " + hum_url + " HTTP/1.1\r\n");
    client.print("Host: " + String(host) + "\r\n");
    client.print("X-ApiKey: " + String(api_key) + "\r\n");
    client.print("Content-Type: application/json; charset=UTF-8\r\n");
    client.print("Accept: */*\r\n");
    client.print("Cache-Control: no-cache\r\n");
    client.print("Content-Length: 5\r\n");
    client.print("\n");
    client.print(String(hum));
    client.print("\r\n");
    delay(100);
}


//###########Send Temperature Data##################
void sendTemp(){
    client.print("PUT " + temp_url + " HTTP/1.1\r\n");
    client.print("Host: " + String(host) + "\r\n");
    client.print("X-ApiKey: " + String(api_key) + "\r\n");
    client.print("Content-Type: application/json; charset=UTF-8\r\n");
    client.print("Accept: */*\r\n");
    client.print("Cache-Control: no-cache\r\n");
    client.print("Content-Length: 5\r\n");
    client.print("\n");
    client.print(String(temp));
    client.print("\r\n");
    delay(100);
}

//****This functios in for getting temp and hum data from DHT11
bool getDht(){
  hum = dht.readHumidity();
  temp = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
  if (isnan(hum) || isnan(temp) ) {
    Serial.println("Failed to read from DHT sensor!");
    return false;
  }

  //Print temperature and humidity value to Serial console
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" *C ");

  return true;
}

