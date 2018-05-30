#include <LiquidCrystal.h>

const int rs = 12, en = 13, d0 = A0, d1 = A1, d2 = A2, d3 = A3;
LiquidCrystal lcd(rs, en, d0, d1, d2, d3);
const int analogPin = A4;
float analogValue;
float analogValue2;
float input_voltage;
float input_voltage2;

int ledPins[] = {
2, 3, 4, 5, 6, 7, 8, 9, 10, 11 
};       // an array of pin numbers to which LEDs are attached
int pinCount = 10;           // the number of pins (i.e. the length of the array)

void setup()
{
   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps 
   pinMode(A0,OUTPUT);
   pinMode(A1,OUTPUT);
   pinMode(A2,OUTPUT);
   pinMode(A3,OUTPUT);
   pinMode(A4,INPUT);
   pinMode(A5,INPUT);
   lcd.begin(16, 2);       //// set up the LCD's number of columns and rows:
   lcd.print("Voltage Level1");
   lcd.setCursor(0, 1);
   lcd.print("Voltage1=");
   lcd.print(input_voltage);
   Serial.println(input_voltage);
   delay(4000);
   lcd.clear();
   lcd.print("Voltage Level2");
   lcd.setCursor(0,1);
   lcd.print("Voltage2= ");
   lcd.print(input_voltage2);
   Serial.println(input_voltage2);
   delay(4000);
   lcd.clear();
   

   

}
void LED_function(int stage) // led glowing low and full battery
{
     for (int j=2; j<=11; j++)
   {
    digitalWrite(j,LOW);
   }
   for (int i=1, l=2; i<=stage; i++,l++)
   {
    digitalWrite(l,HIGH);
    //delay(30);
   }
   
}
void loop()
{
//  Conversion formula for voltage
   analogValue = analogRead (A4);
   analogValue2 = analogRead(A5);
   Serial.println(analogValue);
   Serial.println(analogValue2);
   delay (1000); 
   input_voltage = (analogValue * 5.0) / 1024.0;
   input_voltage2 = (analogValue2 * 5.0) / 1024.0;
   lcd.begin(16, 2);       //// set up the LCD's number of columns and rows:
   lcd.print("Voltage Level1");
   lcd.setCursor(0, 1);
   lcd.print("Voltage1=");
   lcd.print(input_voltage);
   Serial.println(input_voltage);
   delay(4000);
   lcd.clear();
   lcd.print("Voltage Level2");
   lcd.setCursor(0,1);
   lcd.print("Voltage2= ");
   lcd.print(input_voltage2);
   Serial.println(input_voltage2);
   delay(4000);
   lcd.clear();
}
//led light up for low and high volt
if (input_voltage < 0.50 && input_voltage >= 0.00 )
{
digitalWrite(2, HIGH);
delay (30);
digitalWrite(2, LOW);
delay (30);
}
else if (input_voltage < 1.00 && input_voltage >= 0.50)
{
LED_function(2);
}
else if (input_voltage < 1.50 && input_voltage >= 1.00)
{
LED_function(3);
}
else if (input_voltage < 2.00 && input_voltage >= 1.50)
{
LED_function(4);
}
else if (input_voltage < 2.50 && input_voltage >= 2.00)
{
LED_function(5);
}
else if (input_voltage < 3.00 && input_voltage >= 2.50)
{
LED_function(6);
}
else if (input_voltage < 3.50 && input_voltage >= 3.00)
{
LED_function(7);
}
else if (input_voltage < 4.00 && input_voltage >= 3.50)
{
LED_function(8);
}
else if (input_voltage < 4.50 && input_voltage >= 4.00)
{
LED_function(9);
}
else if (input_voltage < 5.00 && input_voltage >= 4.50)
{
LED_function(10);
}


if (input_voltage2 < 0.50 && input_voltage2 >= 0.00 )
{
digitalWrite(2, HIGH);
delay (30);
digitalWrite(2, LOW);
delay (30);
}
else if (input_voltage2 < 1.00 && input_voltage2 >= 0.50)
{
LED_function(2);
}
else if (input_voltage2 < 1.50 && input_voltage2 >= 1.00)
{
LED_function(3);
}
else if (input_voltage2 < 2.00 && input_voltage2 >= 1.50)
{
LED_function(4);
}
else if (input_voltage2 < 2.50 && input_voltage2 >= 2.00)
{
LED_function(5);
}
else if (input_voltage2 < 3.00 && input_voltage2 >= 2.50)
{
LED_function(6);
}
else if (input_voltage2 < 3.50 && input_voltage2 >= 3.00)
{
LED_function(7);
}
else if (input_voltage2 < 4.00 && input_voltage2 >= 3.50)
{
LED_function(8);
}
else if (input_voltage2 < 4.50 && input_voltage2 >= 4.00)
{
LED_function(9);
}
else if (input_voltage2 < 5.00 && input_voltage2 >= 4.50)
{
LED_function(10);
}



