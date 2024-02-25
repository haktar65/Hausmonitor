#include <Console.h>
#include <Bridge.h>
#include <FileIO.h>
#include <Process.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>

char chBuf[200];

// _DateAndTime variable declaration
Process date;
time_t DaTTimeValue = 0;
tmElements_t DaTTimeTM;
String DaTYear;
String DaTMonth;
String DaTDate;

time_t nextDData;
time_t nextMData;
time_t nextYData;

// _SzSerial Variables and Constants
const int SzBaudRate = 9600;
const int SzPinRX = 8;
const int SzPinTX = 9;
SoftwareSerial SzSerial(SzPinRX, SzPinTX);
long long SzZWTP = 0;
long long SzZWTN = 0;
long SzZWTM = 0;
byte SzReadFlags = 0x00;


// _Relays Variables and Constants
const int relayWater = 7;
const int relayGas = 2;
boolean stateRelayWater = true;
boolean stateRelayGas = true;
long valueWater = 0;
long counterWater = 0;
long valueGas = 0;
long counterGas = 0;

// _FileAccess

void setup() 
{
  Bridge.begin();
  Console.println("Starting ...");
  FaInit();
  DaTInit();
  SzInit();
  RelaysInit();

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  Console.println("Started!");
}
