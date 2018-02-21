// Test code for Adafruit GPS modules using MTK3329/MTK3339 driver
//
// This code shows how to listen to the GPS module in an interrupt
// which allows the program to have more 'freedom' - just parse
// when a new NMEA sentence is available! Then access data when
// desired.
/**************************************************************************/
/*
    GPS_Logging.ino V0.0.2
    Test code for Adafruit GPS modules using MTK3329/MTK3339 driver

    This code shows how to listen to the GPS module in an interrupt
    which allows the program to have more 'freedom' - just parse
    when a new NMEA sentence is available! Then access data when
    desired.

    Christian Patalas
    Daksh Patel
    Talha Abdulaziz
    Walter Tchouku

    Feb 21 2018

    https://github.com/uwinrockets

*/
/**************************************************************************/


#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>

//Global sensor objects
File myFile;
int pinCS = 10; // Pin 10 on Arduino Uno
SoftwareSerial mySerial(8, 7);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences. 
#define GPSECHO  false

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

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
//  Serial.println("Adafruit GPS library basic test!");
    pinMode(pinCS, OUTPUT);
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

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
    Serial.println(F("SD card is ready to use."));
  } else
  {
    Serial.println(F("SD card initialization failed"));
    return;
  }
  
  // Create/Open file 
  myFile = SD.open("GPS2.csv", FILE_WRITE); //**************************************************************************************************************************************
  
  // if the file opened okay, write to it:
  if (myFile) {
    //Serial.println(F("Writing to file..."));
    // Write to file
   myFile.println("...");
    myFile.close(); // close the file
    Serial.println(F("Done."));
  }
  // if the file didn't open, print an error:
  else {
    Serial.println(F("error opening test.txt"));
  }

//  // Reading the file
//  myFile = SD.open("GPS.txt");
//  if (myFile) {
//    Serial.println(F("Read:"));
//    // Reading the whole file
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//   }
//    myFile.close();
//  }
//  else {
//    Serial.println(F("error opening test.txt"));
//  }
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
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences! 
    // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
    //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false
  
    if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
      return;  // we can fail to parse a sentence in which case we should just wait for another
  }

  // if millis() or timer wraps around, we'll just reset it
  if (timer > millis())  timer = millis();

  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) { 
    timer = millis(); // reset the timer

     myFile = SD.open("GPS2.csv", FILE_WRITE); //**************************************************************************************************************************************
  if (myFile)
  {
//    Serial.print("\nTime: ");
//myFile.print("\nTime: ");
//    Serial.print(GPS.hour, DEC); Serial.print(':');
myFile.print(GPS.hour, DEC); myFile.print(':');
//    Serial.print(GPS.minute, DEC); Serial.print(':');
myFile.print(GPS.minute, DEC); myFile.print(':');
//    Serial.print(GPS.seconds, DEC); Serial.print('.');
myFile.print(GPS.seconds, DEC); myFile.print('.');
//    Serial.print(GPS.milliseconds);
myFile.print(GPS.milliseconds);
//    Serial.print(" ||");
myFile.print(",");
//    Serial.print(" Date: ");
//myFile.print("Date: ");
//    Serial.print(GPS.day, DEC); Serial.print('/');
myFile.print(GPS.day, DEC); myFile.print('/');
//    Serial.print(GPS.month, DEC); Serial.print("/20");
myFile.print(GPS.month, DEC); myFile.print("/20");
//    Serial.print(GPS.year, DEC);
myFile.print(GPS.year, DEC);
//    Serial.print(" ||");
myFile.print(",");
//    Serial.print(" Fix: "); Serial.print((int)GPS.fix);
//myFile.print("Fix: "); 
myFile.print((int)GPS.fix);
//    Serial.print(", Quality: "); Serial.print((int)GPS.fixquality); 
//myFile.print(", Quality: "); 
myFile.print(",");
myFile.print((int)GPS.fixquality); 

////      Serial.print("Location: ");
myFile.print(",");
//myFile.print("Location: ");
////      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
myFile.print(GPS.latitude, 4); myFile.print(GPS.lat);
////      Serial.print(", "); 
myFile.print(","); 
////      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
myFile.print(GPS.longitude, 4); myFile.print(GPS.lon);
////      Serial.print(" ||");
myFile.print(",");
////      Serial.print(" Location: ");
//myFile.print(" Location: ");
////      Serial.print(GPS.latitudeDegrees, 4);
myFile.print(GPS.latitudeDegrees, 4);
////      Serial.print(", "); 
myFile.print(","); 
////      Serial.print(GPS.longitudeDegrees, 4);
myFile.print(GPS.longitudeDegrees, 4);
////      Serial.print(" ||");
myFile.print(",");
////      Serial.print(" Speed (knots): "); Serial.print(GPS.speed);
//myFile.print(" Speed (knots): "); 
myFile.print(GPS.speed);
////      Serial.print(" ||");
myFile.print(",");
////      Serial.print(" Angle: "); Serial.print(GPS.angle);
//myFile.print(" Angle: "); 
myFile.print(GPS.angle);
////      Serial.print(" ||");
myFile.print(",");
////      Serial.print(" Altitude: "); Serial.print(GPS.altitude);
//myFile.print(" Altitude: ");
myFile.print(GPS.altitude);
////      Serial.print(" ||");
myFile.print(",");
////      Serial.print(" Satellites: "); Serial.print((int)GPS.satellites);
//myFile.print(" Satellites: "); 
myFile.println((int)GPS.satellites);
    myFile.close();
  }
  }
}
