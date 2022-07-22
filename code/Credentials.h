#ifndef Credentials_h
#define Credentials_h

char ssid[] = "********";        // your network SSID (name)
char pass[] = "********";        // your network password

const char* host = "********";  // webpage to send the data
const char* SecCode = "********";   // security key

const char* directory = "/Temp/addP1.php";  // website page for data posting

char user[]         = "********";        // MySQL user login username
char password[]     = "********";          // MySQL user login password

// string to test data import in website url
String TESTurl = "/Temp/addP1.php?Counter=111111.812&CounterDeliv=338.795&CounterDelivToday=2.71&CounterToday=9.04&idx=6&Hoogtarief=751659&Laagtarief=1005153&Teruglaag=282054&Terughoog=56741&Huidigverbruik=799&Huidiglevering=0&DateTime=2020-03-16%2019:57:54&Name=PowerZZ3";

// Domoticz sensor ID's
int IDXIDgas = 34;     // Gas
int IDXIDpower = 24;   // Elec
int IDXIDpowerL1 = 25; // Voltage line 1
int IDXIDpowerL3 = 27; // Voltage line 3
int IDXIDpowerU1 = 28; // usage on line 1
int IDXIDpowerU3 = 30; // usage on line 3  // NEW TO ADD
int IDXIDpowerD1 = 31; // delivery on line 1  // NEW TO ADD
int IDXIDpowerD3 = 33; // delivery on line 3

// blink times and pin Led ID's
ezOutput ledconnect(12);  // D6 blue create ezOutput object that attach to pin 7;
ezOutput ledsend(13);  // D7 green create ezOutput object that attach to pin 8;
ezOutput ledreboot(15);  // D8 red create ezOutput object that attach to pin 9;

// variables
float Hoogtarief, Laagtarief, Teruglaag, Terughoog;
String DateTime, Name;

int IDXID = IDXIDgas;  // starting with gas data
int nowstatus = 0;     // starting with gas data

int cnt = 5; // wifi connection attempts and then reboot esp

float L1, L3;  // voltage lines
int PU1, PU3;  // usage lines
int PD1 , PD3; // delivery lines
int intHuidigverbruik = 0;  // needed for converse string into integer
int intHuidiglevering = 0; // needed for converse string into integer

String url;  // data url string to post

#endif    //Credentials_h
