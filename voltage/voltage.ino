
const int analogPin = A4;
float analogValue;
float analogValue2;
float input_voltage;
float input_voltage2;


void setup()
{
   Serial.begin(9600);     //  opens serial port, sets data rate to 9600 bps 

   pinMode(A4,INPUT);
   pinMode(A5,INPUT);

   Serial.println(input_voltage);

   Serial.println(input_voltage2);


}

void loop()
{
//  Conversion formula for voltage
   analogValue = analogRead (A4);
   analogValue2 = analogRead(A5);
   Serial.println(analogValue);
   Serial.println(analogValue2);

   input_voltage = (analogValue * 5.0) / 1024.0;
   input_voltage2 = (analogValue2 * 5.0) / 1024.0;

   Serial.println(input_voltage);
   Serial.println(input_voltage2);
}
      



