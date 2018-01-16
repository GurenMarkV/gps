#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

// Singleton instance of the radio driver
RH_RF95 driver;
// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, SERVER_ADDRESS);

struct dataStruct{
  uint8_t hour, minute, seconds, year, month, day;
  uint16_t milliseconds;
  uint8_t fixquality, satellites;
  boolean fix;
  float latitudeDegrees, longitudeDegrees;
  float geoidheight, altitude;
  float speed, angle, magvariation, HDOP;
  unsigned long counter;
   
}SensorReadings;

void setup()
{
  Serial.begin(9600);
  if (!manager.init())
    Serial.println("init failed");

  SensorReadings.hour = 0;
  SensorReadings.minute = 0;
  SensorReadings.seconds = 0;
  SensorReadings.milliseconds = 0;
  SensorReadings.year = 0;
  SensorReadings.month = 0;
  SensorReadings.day = 0;
  SensorReadings.fixquality = 0;
  SensorReadings.satellites = 0;
  SensorReadings.geoidheight = 0;
  SensorReadings.altitude = 0;
  SensorReadings.speed = 0;
  SensorReadings.angle = 0;
  SensorReadings.magvariation = 0;
  SensorReadings.HDOP = 0;
  SensorReadings.counter = 0;
}

void loop()
{
ReciveFromSensors();
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
    if (manager.recvfromAck(buf, &len, &from))
    {
      int i;
      memcpy(&SensorReadings, buf, sizeof(SensorReadings));
      Serial.println("--------------------------------------------");
      Serial.print("Got message from unit: ");
      Serial.println(from, DEC);
      Serial.print("Transmission number: ");
      Serial.println(SensorReadings.counter);
      Serial.println("");
       
      Serial.print("\nTime: ");
      Serial.print(SensorReadings.hour); Serial.print(':');
      Serial.print(SensorReadings.minute); Serial.print(':');
      Serial.print(SensorReadings.seconds); Serial.print('.');
      Serial.println(SensorReadings.milliseconds);
      Serial.print("Date: ");
      Serial.print(SensorReadings.day); Serial.print('/');
      Serial.print(SensorReadings.month); Serial.print("/20");
      Serial.println(SensorReadings.year);
      Serial.print("Fix: "); Serial.print((int)SensorReadings.fix);
      Serial.print(" quality: "); Serial.println((int)SensorReadings.fixquality); 
      Serial.print("Location: ");
      Serial.print(SensorReadings.latitudeDegrees, 4);
      Serial.print(", "); 
      Serial.println(SensorReadings.longitudeDegrees, 4);
      
      Serial.print("Speed (knots): "); Serial.println(SensorReadings.speed);
      Serial.print("Angle: "); Serial.println(SensorReadings.angle);
      Serial.print("Altitude: "); Serial.println(SensorReadings.altitude);
      Serial.print("Satellites: "); Serial.println((int)SensorReadings.satellites);
      delay(2000);
      Serial.println("--------------------------------------------");
    }
  }
}
