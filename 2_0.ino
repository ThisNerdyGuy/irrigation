// Millis() uses unsigned longs which is a 32 bit number.
// Translates to around 4 billion millseconds (4294967296 if you're feeling pedantic) or 49 days and 17 hours


// ** Switch **
const int buttonPin      = 11;
boolean ledState         = HIGH;
boolean buttonState      = LOW;
unsigned long buttonTime = 0;
const int debouncer      = 50;


// ** LEDs **
const int ledPin          = 9;
const int ledPin2         = 0;
unsigned long blinkMillis = 0;
const int blinkOFFtime    = 1800;
const int blinkONtime     = 200;
boolean ledStatus         = LOW;


// ** Valve **
const int valvePinON         = 1;
const int valvePinOFF        = 2;
const int valveLenght        = 30000; // 30s
unsigned long valveStartTime = 0;
boolean valveStatus          = LOW;
const long valveCutOff       = 600000; // 10m
const int valvePulse         = 50;


// ** Moisture Sensor **
const int moisturePin       = A5;
unsigned int moistureValue  = 0;   // 1024: dry - 0: wet
const int moistureMin       = 800;
unsigned long lastIrrigated = 0;


// ** Luminosity Sensor **
const int photoresistorPin    = A2;
const int luminosityThreshold = 250;
unsigned int darknessValue    = 0;  //1024: pitch black - 0: whitest white


void setup()
{
  pinMode(buttonPin,        INPUT);
  pinMode(moisturePin,      INPUT);
  pinMode(photoresistorPin, INPUT);

  pinMode(ledPin,      OUTPUT);
  pinMode(valvePinON,  OUTPUT);
  pinMode(valvePinOFF, OUTPUT);

  //Serial.begin(9600); //Serial.println("Program Started");

  valveOff();
}


// ********************************************************************************************************************

void loop()
{
  // **** SENSORS ****
  buttonState   = digitalRead(buttonPin);       //Serial.println(buttonPin);
  moistureValue = analogRead(moisturePin);      //Serial.println(moistureValue);
  darknessValue = analogRead(photoresistorPin); //Serial.println(photoresistorPin);


 
  // **** BUTTON ****
  if (buttonState == HIGH)
  {

    if (buttonTime == 0)
      buttonTime = millis();


    // Clicked with debounced on
    if ((millis() - buttonTime) > debouncer)
    {
      if (valveStatus == LOW)
      {
        LEDOn();
        valveOn();
        valveStatus = HIGH;
        valveStartTime = millis();
      }
    }
  }
  else
  {
    buttonTime = 0; // reset button debouncer
    
    
    // minimun on-time for valve
    if (valveStatus == HIGH)
    {
      if ((millis() - valveStartTime) > valveLenght)
      {
        valveStartTime = 0;
        valveStatus = LOW;
        valveOff();
        LEDOff();
        lastIrrigated = millis();
      }
    }
    else
    {

      
      // **** LUMINOSITY ****
      // only if minimun brightness is reached
      if (darknessValue < luminosityThreshold)
      {

        
        // **** MOISTURE ****
        // only if minimun time since last irrigation has passed
        if ( (millis() - lastIrrigated) > valveCutOff)
        {
          if (moistureValue > moistureMin)
          {
            Irrigate();
          }
        }
      
      

        // **** LED ****
        // led on during day
        if ( ((millis() - blinkMillis) > blinkOFFtime) && (ledStatus == LOW) )
        {
          blinkMillis = millis();
          ledStatus = HIGH;
          LEDOn();
        }
        // led off
        if ( ((millis() - blinkMillis) > blinkONtime) && (ledStatus == HIGH) )
        {
          blinkMillis = millis();
          ledStatus = LOW;
          LEDOff();
        }
      }
      else
      {
     
        // **** LED ****
        // led on during night
        if ( ((millis() - blinkMillis) > blinkONtime) && (ledStatus == LOW) )
        {
          blinkMillis = millis();
          ledStatus = HIGH;
          LEDOn();
        }
        // led off
        if ( ((millis() - blinkMillis) > blinkOFFtime) && (ledStatus == HIGH) )
        {
          blinkMillis = millis();
          ledStatus = LOW;
          LEDOff();
        }
      }
    }
  }
}





void Irrigate()
{
  LEDOn();
  valveOn();
  delay(valveLenght);
  valveOff();
  LEDOff();
  lastIrrigated = millis();
}


void LEDOn()
{
  digitalWrite(ledPin, HIGH);
}

void LEDOff()
{
  digitalWrite(ledPin, LOW);
}


void valveOn()
{
  digitalWrite(valvePinON, HIGH); //Serial.println("valve-ON HIGH");
  delay(valvePulse);
  digitalWrite(valvePinON, LOW);  //Serial.println("valve-ON LOW");
}

void valveOff()
{
  digitalWrite(valvePinOFF, HIGH); //Serial.println("valve-OFF HIGH");
  delay(valvePulse);
  digitalWrite(valvePinOFF, LOW);  //Serial.println("valve-OFF LOW");
}
