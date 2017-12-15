// rf95_payload client
// Sends data to the Base
// LoRa Simple Server for Arduino :
// Support Devices: LoRa Shield/GPS + Arduino
#include <GPSport.h>
#include <NMEAGPS.h>
NMEAGPS gps;
NeoGPS::Location_t base( -253448688L, 1310324914L ); // Ayers Rock, AU

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF95 rf95;

// Class to manage message delivery and receipt
RHReliableDatagram manager(rf95, CLIENT_ADDRESS);

float frequency = 915.0; // Change the frequency here.

struct dataStruct{
  float dist;
  unsigned long counter;
  float rssiV;
}SensorReadings;

 // RF communication, Dont put this on the stack:
 byte buf[sizeof(SensorReadings)] = {0};

void setup() 
{

  DEBUG_PORT.begin(115200);
  while (!DEBUG_PORT) ; // Wait for DEBUG_PORT port to be available
  DEBUG_PORT.println("LoRa Distance Payload");
  if (!manager.init())
    DEBUG_PORT.println("LoRa init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  rf95.setFrequency(frequency);

  rf95.setTxPower(20, false);
  
  rf95.setCADTimeout(10000);
  DEBUG_PORT.println("Waiting for radio to setup");
  delay(1000);
  DEBUG_PORT.println("Setup completed");
  delay(1000);
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );
  gpsPort.begin(9600);

  SensorReadings.dist = 0;
  SensorReadings.counter = 0;
  SensorReadings.rssiV = 0;
  
} // Setup

//uint8_t data[] = "Hello World Base ! I'm happy if you can read me";
// Dont put this on the stack:
//uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  float dist;
  float rssiV;
  DEBUG_PORT.println("------------------------------------");
  DEBUG_PORT.println("Sending to base"); 
  distance(); //Read temp sensor
  delay(1000);
  rssi();  //Read RSSI value
  delay(1000);
  SendValues(); //Send sensor values
  delay(2000);
  DEBUG_PORT.println("------------------------------------");

 
    
//  // Send a message to manager_server
//  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
//  {
//    // Now wait for a reply from the server
//    uint8_t len = sizeof(buf);
//    uint8_t from;   
//    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
//    {
//      DEBUG_PORT.print("got reply from : 0x");
//      DEBUG_PORT.print(from, HEX);
//      DEBUG_PORT.print(": ");
//      DEBUG_PORT.println((char*)buf);
//      DEBUG_PORT.print("RSSI: ");
//      DEBUG_PORT.println(rf95.lastRssi(), DEC);
//    }
//    else
//    {
//      DEBUG_PORT.println("No reply, is base running?");
//    }
//  }
//  else
//    DEBUG_PORT.println("sendtoWait failed - this is payload");
//  delay(500);
} //  Loop

//Get Distance from GPS sensor
void distance()
{
  gps.available( gpsPort );
  gps_fix fix = gps.read(); // save the latest

  // When we have a location, calculate how far away we are from the base location.

  float range = fix.location.DistanceMiles( base );

  DEBUG_PORT.print( F("Range: ") );
  DEBUG_PORT.print( range );
  DEBUG_PORT.println( F(" Miles") );
  SensorReadings.dist = range;

} //  Distance

void rssi()
{
    DEBUG_PORT.print("RSSI: ");
    DEBUG_PORT.println(rf95.lastRssi(), DEC);
    SensorReadings.dist = rf95.lastRssi();
}

//RF communication
void SendValues()
{ 
  //Load message into data-array
  //byte data[sizeof(sensorValues)];

  byte zize=sizeof(SensorReadings);
  memcpy (buf, &SensorReadings, zize);

 
  DEBUG_PORT.println("Sending to Base");
  int i = 0;
  int repeat = 1; //Times to repeat same message
   
  // Send a message to manager_server
  while (i<repeat)
  {
    if (manager.sendto(buf, zize, SERVER_ADDRESS))
    {
      DEBUG_PORT.println("Message sent");   
    }
    else
    {
      DEBUG_PORT.println("sendto failed");
    }
    delay(100);
    i = i+1;
  }
  SensorReadings.counter = SensorReadings.counter + 1;
}

