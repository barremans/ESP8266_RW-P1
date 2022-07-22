#define VERSION F("1.1.0 ")
/*
   EXPLANATION
   This code read json file from domoticz ip and send the data to a web url.
   The web url imports the data into a DB.
   Credentials.h holds all the settings, the main .ino, the logica code.
   DSMR 5.0 P1 has eight active sensor readings (IDX), Gas, Power, Line voltage,Line usage, line delivery.
   Starts with reading the gas data and then other power IDX.
   Ends with the power data!
   Important because these holds all the power data.
   Only gas and full power data will be send to the url!
   EzOutput is used for the led status.
   Flash Green = send to url
   Blinking Blue = connecting to wifi
   Blinking Red = reconnecting wifi, after five attempts the esp reboots.
   WEBSERVICES
   Three services are prsent.
   on a webbrowser you go tto the ip  of the ESP.
   For example 192.168.0.203, that is a control screen
   adding one of the follow:
    /webserial  => shows last url string that was posted
    /update  => OTA firmware update possibility.
*/

/*
  To Do:
** Data import direct in Mysql DB, not by url page.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ezOutput.h> // ezOutput library
#include <ESPAsyncWebServer.h>  // webserial
#include <ESPAsyncTCP.h> // easyOTA
#include <AsyncElegantOTA.h> // easyOTA
#include <WebSerial.h>  // webserial
AsyncWebServer server(80);  // web port
#include "Credentials.h"  // settings
#include <ArduinoJson.h>

void setup() {

  // initialize the digital pin as an output.
  ledconnect.blink(500, 250); // 500 milliseconds ON, 250 milliseconds OFF wifi connection led
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println();

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    ledconnect.loop();  // blinking wifi status led
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  ledconnect.low(); // stop blinking wifi status led

  WebSerial.begin(&server);  // starting webserial service
  WebSerial.msgCallback(callback);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/plain", "Hi! ESP8266 runs P1 - Domotics webservice.");
  });

  AsyncElegantOTA.begin(&server);    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");
  Serial.print('\n');

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  delay(500);
}

void loop() {
  // wait for WiFi connection
  if ((WiFi.status() == WL_CONNECTED)) {
    cnt = 5;  // attempts wifi reconnecting

    WiFiClient client;
    HTTPClient http;

    // Send request
    http.useHTTP10(true);
    String adddata = ("http://192.168.0.136:8855/json.htm?type=devices&rid=");  // url to connect domoticz API
    adddata += IDXID;  // add domoticz IDX of sensor, changes after every loop

    http.begin(client, adddata); // get data from the domoticz API
    int httpCode = http.GET();

    // Allocate the JSON document
    // Use arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument <1782> doc;
    if (httpCode > 0) {
      // Get the request response payload
      String payload = http.getString();
      //Parsing
      deserializeJson(doc, payload);
      JsonObject result_0 = doc["result"][0];  // json object holds the API data

      // parameters
      String Counter = result_0["Counter"]; // Counter of meter (elec)
      String CounterDeliv = result_0["CounterDeliv"]; // Counter delivery of meter (elec)
      String CounterDelivToday = result_0["CounterDelivToday"]; // Counter delivery today of meter (elec)
      String CounterToday = result_0["CounterToday"]; // Counter today of meter (elec)
      String Huidigverbruik = result_0["Usage"]; // actual consumption, string combines integer with string
      String Huidiglevering = result_0["UsageDeliv"]; // actual consumption, string combines integer with string
      const char* idx = result_0["idx"]; // idx sensor (power, gas)
      String result_0_Data = result_0["Data"]; // complete dataset
      String DateTime = result_0["LastUpdate"]; // Last update data
      String Name = result_0["Name"]; // Sensor name, Power of Gas

      // split json string object and converse to integer, without the string Watt
      if ( IDXID == IDXIDpower) {

        int index = Huidigverbruik.indexOf('W');
        int index1 = Huidiglevering.indexOf('W');

        intHuidigverbruik = Huidigverbruik.substring(0, index).toInt();
        intHuidiglevering = Huidiglevering.substring(0, index1).toInt();

        // split json string object and converse to integer, without the string Watt
        int i1 = result_0_Data.indexOf(';');
        int i2 = result_0_Data.indexOf(';', i1 + 1);
        int i3 = result_0_Data.indexOf(';', i2 + 1);
        int i4 = result_0_Data.indexOf(';', i3 + 1);
        // converse to float and store
        Hoogtarief = result_0_Data.substring(0, i1).toFloat() / 1000;
        Laagtarief = result_0_Data.substring(i1 + 1, i2).toFloat() / 1000;
        Teruglaag = result_0_Data.substring(i1 + 1, i2).toFloat() / 1000;
        Terughoog = result_0_Data.substring(i3 + 1, i4).toFloat() / 1000;
      }

      // if sensor is gas, then reset variables
      if ( IDXID == IDXIDgas) {
        ResetV();
      }

      if ( IDXID == IDXIDpowerL1) {
        int index4 = result_0_Data.indexOf('V'); // L1
        L1 = result_0_Data.substring(0, index4).toFloat();
      }

      if ( IDXID == IDXIDpowerL3) {
        int index5 = result_0_Data.indexOf('V'); // L3
        L3 = result_0_Data.substring(0, index5).toFloat();
      }

      if ( IDXID == IDXIDpowerU1) {
        int index2 = result_0_Data.indexOf('W'); // PU1
        PU1 = result_0_Data.substring(0, index2).toInt();
      }

      if ( IDXID == IDXIDpowerU3) {
        int index2 = result_0_Data.indexOf('W'); // PU1
        PU3 = result_0_Data.substring(0, index2).toInt();
      }

      if ( IDXID == IDXIDpowerD1) {
        int index3 = result_0_Data.indexOf('W'); // PD3
        PD1 = result_0_Data.substring(0, index3).toInt();
      }

      if ( IDXID == IDXIDpowerD3) {
        int index3 = result_0_Data.indexOf('W'); // PD3
        PD3 = result_0_Data.substring(0, index3).toInt();
      }

      // end split

      String buff = result_0_Data;

      // check if webpage is reachable
      const int httpPort = 80;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }

      ////////////////  create sending url  //////////////////////////
      url = directory;  // see defination in Credentials.h
      url += "?Counter=";
      url += Counter;
      url += "&CounterDeliv=";
      url += CounterDeliv;
      url += "&CounterDelivToday=";
      url += CounterDelivToday.substring(0, 5);
      url += "&CounterToday=";
      url += CounterToday.substring(0, 5);
      url += "&idx=";
      url += idx;
      url += "&Hoogtarief=";
      url += Hoogtarief;
      url += "&Laagtarief=";
      url += Laagtarief;
      url += "&Teruglaag=";
      url += Teruglaag;
      url += "&Terughoog=";
      url += Terughoog;
      url += "&Huidigverbruik=";
      url += intHuidigverbruik;
      url += "&Huidiglevering=";
      url += intHuidiglevering;
      url += "&L1=";
      url += L1;
      url += "&L3=";
      url += L3;
      url += "&powerLU1=";
      url += PU1;  // need to change in php
      url += "&powerLU3=";
      url += PU3;  // need to change in php
      url += "&powerLD1=";
      url += PD1; // need to change in php
      url += "&powerLD3=";
      url += PD3; // need to change in php
      url += "&Name=";
      url += Name;
      url += "&SecCode=";
      url += SecCode;

      //for sending url testing only!
      // Serial.print("Requesting URL: ");
      // Serial.println(TESTurl);

      // when IDX, domoticz sensor is readed then change IDX for next sensor reading
      if (nowstatus == 8) {
        nowstatus = 0;
        IDXID = IDXIDpowerL1; //  Voltage on line 1
      }

      if (nowstatus == 7) {
        nowstatus = 8;
        IDXID = IDXIDgas; //  Gas data
        // print Power values for visual check, not needed in pratice
        /*
          Serial.print("Data: ");
          Serial.println(result_0_Data);
          Serial.println("-----------------------------");
          Serial.print("Counter: "); // Counter of meter (elec)
          Serial.println(Counter);
          Serial.print("CounterDeliv: "); // Counter delivery of meter (elec)
          Serial.println(CounterDeliv);
          Serial.print("CounterDelivToday: "); // Counter delivery today of meter (elec)
          Serial.println(CounterDelivToday.substring(0, 5));
          Serial.print("CounterToday: "); // Counter today of meter (elec)
          Serial.println(CounterToday.substring(0, 5));
          Serial.print("idx: "); // idx sensor (elec)
          Serial.println(idx);
          Serial.print("Hoogtarief: "); // 1
          Serial.println(Hoogtarief);
          Serial.print("Laagtarief: ");  //2
          Serial.println(Laagtarief);
          Serial.print("Teruglaag: ");  //3
          Serial.println(Teruglaag);
          Serial.print("Terughoog: ");  //4
          Serial.println(Terughoog);
          Serial.print("Huidigverbruik: ");  //5
          Serial.println(intHuidigverbruik);
          Serial.print("Huidiglevering: ");  //6
          Serial.println(intHuidiglevering);
          Serial.print("Voltage L1: ");  //7
          Serial.println(L1);
          Serial.print("Voltage L3: ");  //8
          Serial.println(L3);
          Serial.print("Usage L1: ");  //9
          Serial.println(PU1);
          Serial.print("Usage L3: ");  //10
          Serial.println(PU3);
          Serial.print("Delivery L1: ");  //11
          Serial.println(PD1);
          Serial.print("Delivery L3: ");  //12
          Serial.println(PD3);
          Serial.print("Last Update: "); // last update data
          Serial.println(DateTime);
          Serial.print("Name: ");  // name sensor
          Serial.println(Name);
          Serial.println("-----------------------------");
        */
        delay(2000);
        //
        // Serial.print("Requesting URL: ") // uncomment for check
        ledsend.high();  // blink sending led
        WebSerial.print("Last Update: "); // last update data
        WebSerial.println(DateTime);
        WebSerial.println(url); // This will send the request to the server
        // Serial.println(url); // This will send the request to the server // uncomment for check
        client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n"); // send to url, disabled for testing code
        delay(10);  // for stability
      }

      // IMPORTANT!! Start with GAS ends with POWER!!!!
      if (nowstatus == 6) {
        nowstatus = 7;
        IDXID = IDXIDpower; // Complete power data
      }

      if (nowstatus == 5) {
        nowstatus = 6;
        IDXID = IDXIDpowerD3; // Delivery on line 3
      }

      if (nowstatus == 4) {
        nowstatus = 5;
        IDXID = IDXIDpowerD1; // Delivery on line 1
      }

      if (nowstatus == 3) {
        nowstatus = 4;
        IDXID = IDXIDpowerU3; // Usage on line 3
      }

      if (nowstatus == 2) {
        nowstatus = 3;
        IDXID = IDXIDpowerU1; // Usage on line 1
      }

      if (nowstatus == 1) {
        nowstatus = 2;
        IDXID = IDXIDpowerL3; // Voltage on line 3
      }

      if (nowstatus == 0) {
        nowstatus = 1;
        IDXID = IDXIDpowerL1; // Voltage on line 1
        // print Gas values for visual check not needed in pratice
        /*
          Serial.print("Data: ");
          Serial.println(result_0_Data);
          Serial.println("-----------------------------");
          Serial.print("Counter: "); // Counter of meter (elec)
          Serial.println(Counter);
          Serial.print("CounterDeliv: "); // Counter delivery of meter (elec)
          Serial.println(CounterDeliv);
          Serial.print("CounterDelivToday: "); // Counter delivery today of meter (elec)
          Serial.println(CounterDelivToday.substring(0, 5));
          Serial.print("CounterToday: "); // Counter today of meter (elec)
          Serial.println(CounterToday.substring(0, 5));
          Serial.print("idx: "); // idx sensor (elec)
          Serial.println(idx);
          Serial.print("Hoogtarief: "); // 1
          Serial.println(Hoogtarief);
          Serial.print("Laagtarief: ");  //2
          Serial.println(Laagtarief);
          Serial.print("Teruglaag: ");  //3
          Serial.println(Teruglaag);
          Serial.print("Terughoog: ");  //4
          Serial.println(Terughoog);
          Serial.print("Huidigverbruik: ");  //5
          Serial.println(intHuidigverbruik);
          Serial.print("Huidiglevering: ");  //6
          Serial.println(intHuidiglevering);
          Serial.print("Voltage L1: ");  //7
          Serial.println(L1);
          Serial.print("Voltage L3: ");  //8
          Serial.println(L3);
          Serial.print("Usage L1: ");  //9
          Serial.println(PU1);
          Serial.print("Usage L3: ");  //10
          Serial.println(PU3);
          Serial.print("Delivery L1: ");  //11
          Serial.println(PD1);
          Serial.print("Delivery L3: ");  //12
          Serial.println(PD3);
          Serial.print("Last Update: "); // last update data
          Serial.println(DateTime);
          Serial.print("Name: ");  // name sensor
          Serial.println(Name);
          Serial.println("-----------------------------");
        */
        delay(2000);
        //
        // Serial.print("Requesting URL: "); // uncomment for check
        ledsend.high(); // blink sending led
        // Serial.println(url); // This will send the request to the server // uncomment for check
        WebSerial.print("Last Update: "); // last update data
        WebSerial.println(DateTime);
        WebSerial.println(url); // This will send the request to the server
        client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n"); // send to url, disabled for testing code
        delay(10);  // for stability
      }

      http.end(); // close connecting
      ledsend.low(); // end blink send led
    }
    /////
    delay(6000); // every +- 60 secs sending to url
  }

  // if not connect to wifi, then try to reconnect, 5 attamps the esp reboot
  else {
    Serial.println("failed!!! !");
    cnt--; // minus 1 for each attempt
    ledreboot.high();
    WiFi.reconnect(); // try to reconnect wifi
  }
  if (cnt == 0) {
    ledreboot.high();
    Serial.println("Reset..");
    ESP.restart(); // restart ESP after 5 attempts
  }
  //
}


//reset variables
void ResetV() {
  intHuidigverbruik = 0;
  intHuidiglevering = 0;
  Hoogtarief = 0.00;
  Laagtarief = 0.00;
  Teruglaag = 0.00;
  Terughoog = 0.00;
  L1 = 0.00;
  L3 = 0.00;
  PU1 = 0.00;
  PU3 = 0.00;
  PD1 = 0.00;
  PD3 = 0.00;
}

void callback(unsigned char* data, unsigned int length)
{
  data[length] = '\0';
  Serial.println((char*) data);  // print input of webserial to serial print
}
