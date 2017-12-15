// rf95_base server
// Recieves the data from the Payload
// LoRa Simple Client for Arduino :
// Support Devices: LoRa Shield + Arduino
#include <GPSport.h>
#include <NMEAGPS.h>
//NMEAGPS gps;
NeoGPS::Location_t base( -253448688L, 1310324914L ); // Ayers Rock, AU

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF95 rf95;

// Class to manage message delivery and receipt
RHReliableDatagram manager(rf95, SERVER_ADDRESS);

float frequency = 915.0; // Change the frequency here.

struct dataStruct{
  float dist;
  float rssi;
  unsigned long counter;
}SensorReadings;

 // RF communication, Dont put this on the stack:
 byte buf[sizeof(SensorReadings)] = {0};

void setup() 
{

DEBUG_PORT.begin(115200);
while (!DEBUG_PORT) ; // Wait for DEBUG_PORT port to be available
DEBUG_PORT.println("LoRa Simple Server");
if (!manager.init())
DEBUG_PORT.println("LoRa init failed");
// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
rf95.setFrequency(frequency);

rf95.setTxPower(13, false);

rf95.setCADTimeout(10000);
DEBUG_PORT.println("Waiting for radio to setup");
delay(1000);
DEBUG_PORT.println("Setup completed");
delay(1000);
//DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );
//gpsPort.begin(9600);

SensorReadings.dist = 0;
//SensorReadings.rssi = 0;
SensorReadings.counter = 0;
}

//uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
//uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  ReciveFromSensors();
//  if (manager.available())
//  {
//    // Wait for a message addressed to us from the client
//    uint8_t len = sizeof(buf);
//    uint8_t from;
//    if (manager.recvfromAck(buf, &len, &from))
//    {
//      DEBUG_PORT.print("got request from : 0x");
//      DEBUG_PORT.print(from, HEX);
//      DEBUG_PORT.print(": ");
//      DEBUG_PORT.println((char*)buf);
//      DEBUG_PORT.print("RSSI: ");
//      DEBUG_PORT.println(rf95.lastRssi(), DEC);
//
//      // Send a reply back to the originator client
//      if (!manager.sendtoWait(data, sizeof(data), from))
//        DEBUG_PORT.println("sendtoWait failed - this is base");
//    }
//  } //  if manager
}


void ReciveFromSensors()
{
  // Dont put this on the stack:
  uint8_t buf[sizeof(SensorReadings)];
  uint8_t from;
  uint8_t len = sizeof(buf);
 
  if (manager.available())
  {   
    // Wait for a message addressed to us from the client
    if (manager.recvfrom(buf, &len, &from))
    {
      int i;
      memcpy(&SensorReadings, buf, sizeof(SensorReadings));
      DEBUG_PORT.println("--------------------------------------------");
      DEBUG_PORT.print("Got message from unit: ");
      DEBUG_PORT.println(from, DEC);
      DEBUG_PORT.print("Transmission number: ");
      DEBUG_PORT.println(SensorReadings.counter);
      DEBUG_PORT.println("");
       
      DEBUG_PORT.print("Distance: ");
      DEBUG_PORT.println(SensorReadings.dist);

      DEBUG_PORT.print("RSSI: ");
      DEBUG_PORT.println(SensorReadings.rssi);

      DEBUG_PORT.println("--------------------------------------------");
    }
  }
}
