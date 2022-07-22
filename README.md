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
