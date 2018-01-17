#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);
#define GPSECHO  false
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

//RH communication
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
// Singleton instance of the radio driver
RH_RF95 driver;
// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

struct dataStruct{
  uint8_t hour, minute, seconds, year, month, day;
  uint16_t milliseconds;
  uint8_t fixquality, satellites;
  boolean fix;
  float latitudeDegrees, longitudeDegrees;
  float geoidheight, altitude;
  float speed, angle, magvariation, HDOP;
  float latitude, longitude;
  int32_t latitude_fixed, longitude_fixed;
  char lat, lon;
  unsigned long counter;
   
}SensorReadings;

 // RF communication, Dont put this on the stack:
  byte buf[sizeof(SensorReadings)] = {0};

void setup()
{
  //Serial.begin(9600);
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
  delay(1000);
  mySerial.println(PMTK_Q_RELEASE);
  //RF communication
  if (!manager.init())
    Serial.println("init failed");

  SensorReadings.hour = 0;
  SensorReadings.minute = 0;
  SensorReadings.seconds = 0;
  SensorReadings.milliseconds = 0;
  SensorReadings.year = 0;
  SensorReadings.month = 0;
  SensorReadings.day = 0;
  SensorReadings.fix = 0;
  SensorReadings.fixquality = 0;
  SensorReadings.satellites = 0;
  SensorReadings.geoidheight = 0;
  SensorReadings.altitude = 0;
  SensorReadings.speed = 0;
  SensorReadings.angle = 0;
  SensorReadings.magvariation = 0;
  SensorReadings.HDOP = 0;
  SensorReadings.latitude = 0;
  SensorReadings.longitude = 0;
  SensorReadings.latitude_fixed = 0;
  SensorReadings.longitude_fixed = 0;
  SensorReadings.lat = 0;
  SensorReadings.lon = 0;
  SensorReadings.counter = 0;
}

SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();

#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

void loop()
{
  Serial.println("------------------------------------");
  Serial.println("Sending to RX");
  float tempValue;
  int soilValue;
 
  date(); //Read date and time
  delay(1000);

  SendValues(); //Send sensor values
  delay(2000);
  Serial.println("------------------------------------");
}

uint32_t timer = millis();
//Get temperatures from Dallas sensor
void date()
{
 if (! usingInterrupt) {
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }

  if (GPS.newNMEAreceived()) {
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer
    
    Serial.print(F("\nTime: "));
    Serial.print(GPS.hour, DEC); Serial.print(':');
    Serial.print(GPS.minute, DEC); Serial.print(':');
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    Serial.println(GPS.milliseconds);
    Serial.print(F("Date: "));
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print(F("/20"));
    Serial.println(GPS.year, DEC);
    Serial.print(F("Fix: ")); Serial.print((int)GPS.fix);
    Serial.print(F(" quality: ")); Serial.println((int)GPS.fixquality); 
    if (GPS.fix) {
      Serial.print(F("Location: "));
      Serial.print(GPS.latitudeDegrees, 4);
      Serial.print(", "); 
      Serial.println(GPS.longitudeDegrees, 4);
      
      Serial.print(F("Speed (knots): ")); Serial.println(GPS.speed);
      Serial.print(F("Angle: ")); Serial.println(GPS.angle);
      Serial.print(F("Altitude: ")); Serial.println(GPS.altitude);
      Serial.print(F("Satellites: ")); Serial.println((int)GPS.satellites);
    }
  }

  SensorReadings.hour = GPS.hour;
  SensorReadings.minute = GPS.minute;
  SensorReadings.seconds = GPS.seconds;
  SensorReadings.milliseconds = GPS.milliseconds;
  SensorReadings.day = GPS.day;
  SensorReadings.month = GPS.month;
  SensorReadings.year = GPS.year;
  SensorReadings.fix = GPS.fix;
  SensorReadings.fixquality = GPS.fixquality;
  SensorReadings.satellites = GPS.satellites;
  SensorReadings.geoidheight = GPS.geoidheight;
  SensorReadings.altitude = GPS.altitude;
  SensorReadings.speed = GPS.speed;
  SensorReadings.angle = GPS.angle;
  SensorReadings.magvariation = GPS.magvariation;
  SensorReadings.HDOP = GPS.HDOP;
  SensorReadings.latitude = GPS.latitude;
  SensorReadings.longitude = GPS.longitude;
  SensorReadings.latitude_fixed = GPS.latitude_fixed;
  SensorReadings.longitude_fixed = GPS.longitude_fixed;
  SensorReadings.lat = GPS.Lat;
  SensorReadings.lon = GPS.lon;
}

//RF communication
void SendValues()
{
    //uint8_t data[] = {SensorReadings};
    // Dont put this on the stack:
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

  // Send a message to manager_server
  if (manager.sendtoWait((uint8_t*)&SensorReadings, sizeof(SensorReadings), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    delay(1000);
  }
  else{
    Serial.println("sendtoWait failed");
  }

  delay(1000);
  SensorReadings.counter = SensorReadings.counter + 1;
}

