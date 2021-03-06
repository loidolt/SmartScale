/*
* created by Rui Santos, http://randomnerdtutorials.com
* ALTERED BY: Raphango
* Temperature Sensor Displayed on 4 Digit 7 segment common CATHODE
* 2013
*/
const int digitPins[4] = {8,7,6,5}; //4 common CATHODE pins of the display (inverted the pins order)
const int clockPin = 2;    //74HC595 Pin 11
const int latchPin = 3;    //74HC595 Pin 12
const int dataPin = 4;     //74HC595 Pin 14
const int tempPin = 11;     //temperature sensor pin
const byte digit[10] =      //seven segment digits in bits
{
  B00111111, //0
  B00000110, //1
  B01011011, //2
  B01001111, //3
  B01100110, //4
  B01101101, //5
  B01111101, //6
  B00000111, //7
  B01111111, //8
  B01101111  //9
};
int digitBuffer[4] = {0};
int digitScan = 0;
float tempC; //Deleted kelvin and farenheit

void setup(){               
  for(int i=0;i<4;i++)
  {
    pinMode(digitPins[i],OUTPUT);
  }
  pinMode(tempPin, INPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT); 
  pinMode(tempPin, INPUT);
  //Serial.begin(9600);
}

//writes the temperature on display
void updateDisp(){
  
  for(byte j=0; j<4; j++) {digitalWrite(digitPins[j], HIGH);} // Turns the display off. Changed to HIGH
  digitalWrite(latchPin, LOW); 
  shiftOut(dataPin, clockPin, MSBFIRST, B00000000);
  digitalWrite(latchPin, HIGH);

  delayMicroseconds(2);

  digitalWrite(digitPins[digitScan], LOW); //Changed to LOW for turning the leds on.

  digitalWrite(latchPin, LOW); 
  if(digitScan==2)
    shiftOut(dataPin, clockPin, MSBFIRST, (digit[digitBuffer[digitScan]] | B10000000)); //print the decimal point on the 3rd digit
  else
    shiftOut(dataPin, clockPin, MSBFIRST, digit[digitBuffer[digitScan]]);

  digitalWrite(latchPin, HIGH);

  digitScan++;
  if(digitScan>3) digitScan=0;
}


void loop(){
  
  digitBuffer[3] = 1;
  digitBuffer[2] = 2;
  digitBuffer[1] = 3;
  digitBuffer[0] = 4;
  updateDisp();
 
}

