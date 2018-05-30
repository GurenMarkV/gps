
/**************************************************************************/
/*
    GPS_Logging.ino V0.0.7
    Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
    With Voltage measuring

    This code shows how to listen to the GPS module in an interrupt
    which allows the program to have more 'freedom' - just parse
    when a new NMEA sentence is available! Then access data when
    desired.

    Christian Patalas
    Daksh Patel
    Talha Abdulaziz
    Walter Tchoukou 

    May 30 2018

    https://github.com/uwinrockets

*/
/**************************************************************************/


#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <NeoTee.h>
#include "SdFat.h"

//Global sensor objects
SdFat SD;
File myFile;
int pinCS = 10; // Pin 10 on Arduino Uno

//Specify the SoftwareSerial Ports on GPS
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

//Print out cleanly
Print *outputs[] = { &Serial, &myFile };  // <--  list all the output destinations here
NeoTee tee( outputs, sizeof(outputs)/sizeof(outputs[0]) );


// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy


float analogValue1;
float analogValue2;
float analogValue3;
float input_voltage1;
float input_voltage2;
float input_voltage3;

/**************************************************************************/
/*
    Output Sensor Values to File on SD Card
*/
/**************************************************************************/
// Generic pattern for most
template<typename T> void outputToSD( T arg )
{
  tee.print( arg );
  tee.print( ',' );
}
// void outputToSD(float num) {
//   tee.print(num);
//   tee.print(",");
// }




/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/

void setup()  
{
    
  // connect at 115200 so we can read the GPS fast enough and echo without dropping chars
  // also spit it out
  Serial.begin(115200);

    pinMode(pinCS, OUTPUT);
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_5HZ);   // 1 Hz update rate

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  // the nice thing about this code is you can have a timer0 interrupt go off
  // every 1 millisecond, and read data from the GPS for you. that makes the
  // loop code a heck of a lot easier!
  useInterrupt(true);

  delay(1000);
  // Ask for firmware version
  mySerial.println(PMTK_Q_RELEASE);

    // SD Card Initialization
  if (SD.begin())
  {
    tee.println(F("SD card is ready to use."));
  } else
  {
    tee.println(F("SD card initialization failed"));
    return;
  }
  
  // Create/Open file 
  myFile = SD.open("GPSLog.csv", FILE_WRITE); //**************************************************************************************************************************************
  
  // if the file opened okay, write to it:
  // if (myFile) {
  //   //Serial.println(F("Writing to file..."));
  //   // Write to file
  //  myFile.println("...");
  //   myFile.close(); // close the file
  //   Serial.println(F("Done."));
  // }
  // // if the file didn't open, print an error:
  // else {
  //   Serial.println(F("error opening test.txt"));
  // }

  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
}


// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
uint32_t timer = millis();
void loop()                     // run over and over again
{
  analogValue1 = analogRead (A3);
  analogValue2 = analogRead (A4);
  analogValue3 = analogRead (A5);

  // in case you are not using the interrupt above, you'll
  // need to 'hand query' the GPS, not suggested :(
  if (! usingInterrupt) {
    // read data from the GPS in the 'main loop'
    char c = GPS.read();
    // if you want to debug, this is a good time to do it!
    if (GPSECHO)
      if (c) Serial.print(c);
  }
  
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {

    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 0.5 seconds or so, print out the current stats
  if (millis() - timer > 500) { 
    timer = millis(); // reset the timer

  //myFile = SD.open("GPS2.csv", FILE_WRITE); //**************************************************************************************************************************************
  if (myFile)
  {
    //Time
    tee.print(GPS.hour, DEC); tee.print(':');
    tee.print(GPS.minute, DEC); tee.print(':');
    tee.print(GPS.seconds, DEC); tee.print('.');
    tee.print(GPS.milliseconds);
    tee.print(",");

    //Date
    tee.print(GPS.day, DEC); tee.print('/');
    tee.print(GPS.month, DEC); tee.print("/20");
    tee.print(GPS.year, DEC);
    tee.print(",");
    outputToSD((int)GPS.fix);
    outputToSD((int)GPS.fixquality); 
    
    //Location
    tee.print(GPS.latitude, 4); 
    tee.print(",");
    tee.print(GPS.longitude, 4); 
    tee.print(",");
    //More GPS Lat and Long
    tee.print(GPS.latitudeDegrees, 4);   
    tee.print(",");
    tee.print(GPS.longitudeDegrees, 4); 
    tee.print(",");
    
    //Speed
    outputToSD(GPS.speed);
  
    //Angle
    outputToSD(GPS.angle);
    
    //Altitude
    outputToSD(GPS.altitude);

    //Satellites
    outputToSD((int)GPS.satellites);

    //  Conversion formula for voltage
    outputToSD(analogValue1);
    outputToSD(analogValue2);
    outputToSD(analogValue3);

    input_voltage1 = (analogValue1 * 5.0) / 1024.0;
    input_voltage2 = (analogValue2 * 5.0) / 1024.0;
    input_voltage3 = (analogValue3 * 5.0) / 1024.0;

    outputToSD(input_voltage1);
    outputToSD(input_voltage2);
    outputToSD(input_voltage3);

    tee.println();
    myFile.flush();
    }
  }
}

