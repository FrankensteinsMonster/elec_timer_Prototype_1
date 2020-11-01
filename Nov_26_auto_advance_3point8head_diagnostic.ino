/* i have no idea how this works
 *  I gave up in frustration & just starting putting code anywhere, it works?  Umm ok.
 *  
 *  There should be a second scale to test out for diagnostics, it's hidden 
 *  by the star slash symbos just like this text is, appers 'greyed out' in my uploader software 
 *  (arduino IDE i think it's called?)
 *  Should be the same for you?
 */
 // constants won't change. Used here to set a pin number:
const int coil1 = 3;
const int coil2 = 5;
const int coil3 = 7;
const int coil4 = 9;

const int sensorPin1 = A4;
const int sensorPin2 = A1;
const int sensorPin3 = A2;
const int sensorPin4 = A3;

int sensorValue = 0;
int starterDelay = 10000;
int sensorThreshold = 200;
int sensorMax = 0; 

// Variables will change:
volatile int coilState = LOW;
volatile int coilPin = LOW;

volatile unsigned long startingDelayTimer = 0;
volatile int startingDelay = false;
volatile int sparkTriggerDelay = 0;               //added this to try & see if i can delay spark at low RPM.
volatile unsigned long timestamp = 0;
volatile unsigned long rpmCurrMicros= 170000;
volatile unsigned long rpmOldMicros = 0;
volatile int sensorTriggerState = false;         // current state of the button
volatile int lastsensorTriggerState = true;     // previous state  of the button/sensor in this case.

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store

const long dwell = 10;                // but this is how long the "points" are open, open longer reduces duty cycle of coils.
const long coilCharge = 2;           // interval at which to charge coil (milliseconds)

void setup() {
  // put your setup code here, to run once:
  pinMode(coil1, OUTPUT);
  pinMode(coil2, OUTPUT);
  pinMode(coil3, OUTPUT);
  pinMode(coil4, OUTPUT);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  Serial.begin(115200);
}

void activateCoilsIfLow(uint8_t inPin, uint8_t outPin)
{
   if (analogRead(inPin) <= sensorThreshold)
   {
    sensorTriggerState = true;
    if (lastsensorTriggerState = false)
    {
      lastsensorTriggerState = true;
    }
    else if (lastsensorTriggerState = sensorTriggerState)   //this isn't working with one equal sign.  I'm so lost.
     {
      delayMicroseconds(sparkTriggerDelay);
      coilState = HIGH;
      coilPin = HIGH;
      digitalWrite(outPin, HIGH);
    
      timestamp = millis();
                                          
      while (coilState == HIGH && coilPin == HIGH && (timestamp + coilCharge) > millis());  //Busy wait
   
      digitalWrite(outPin, LOW);
      coilPin = LOW;
      timestamp = millis();

      while (coilState == HIGH && coilPin == LOW && (timestamp + dwell) > millis());
  
      coilState = LOW;
      coilPin = LOW;
    }
  }
  if (analogRead(inPin) >= sensorThreshold)
  {
    sensorTriggerState = false;
    if (sensorTriggerState != lastsensorTriggerState)
    {
      rpmOldMicros = rpmCurrMicros;
      rpmCurrMicros = micros();
            
      if (millis() >= (startingDelayTimer + 10000))
      {
        if ((rpmCurrMicros - rpmOldMicros <=  169491))         //over approx' 177RPM
        {
           startingDelay = true;                               //no longer hold timing advance-retard at full advance, hope you moved the timing lever by now.
        }
        if ((rpmCurrMicros - rpmOldMicros >= 497512))
        {
          startingDelay = false;                               //chances are you have to re-start the engine now. it'll take 10 seconds or more at over 200RPM to act normal again.
          startingDelayTimer = millis();
        }
      }
      if (startingDelay == true)
      {
        rpmConsultRatioTable();
        lastsensorTriggerState = false;
      }
      else
      {
       Serial.print ( " Starter delay period not ended " );
       lastsensorTriggerState = false;
      }
    }
  }
  lastsensorTriggerState = false;           //Why do i need to make that 'false' so many times to avoid it doing weird things?
}


void loop() {
  activateCoilsIfLow(sensorPin1, coil1);
  activateCoilsIfLow(sensorPin3, coil3);
  activateCoilsIfLow(sensorPin2, coil4);
  activateCoilsIfLow(sensorPin4, coil2);
}


void rpmConsultRatioTable()
{
  if (rpmCurrMicros - rpmOldMicros >= 497512) //sub250rpm
    {
      sparkTriggerDelay =0;
      startingDelay = false;
      startingDelayTimer = millis();
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 255024) && (rpmCurrMicros - rpmOldMicros >= 169491)) //125rpm
    {
      sparkTriggerDelay =81000;                     //was 57, then 59 and i don't know what overflow is but i get an error for it here.
      Serial.print ( " RPM 125" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 169491) && (rpmCurrMicros - rpmOldMicros >= 127512)) //177rpm
    {
      sparkTriggerDelay =61000;
      Serial.print ( " RPM 177" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 127512) && (rpmCurrMicros - rpmOldMicros >= 85008)) //250rpm
    {
      sparkTriggerDelay =41000;                    //was 27  //at 250 RPM and 45 degree sensor advance, this would be 30ms to TDC, subtract 2ms for coil charging and subtract more m.s. for advance 
      Serial.print ( " RPM 250" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 85008) && (rpmCurrMicros - rpmOldMicros >= 61000)) //375rpm
    {
      sparkTriggerDelay =31000;                 //was 16
      Serial.print ( " RPM 375 " );
      }
  else if ((rpmCurrMicros - rpmOldMicros <= 61000) && (rpmCurrMicros - rpmOldMicros >= 48000)) //500rpm
    {
      sparkTriggerDelay =23000;                 //was 11
      Serial.print ( " RPM 500 " );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 48000) && (rpmCurrMicros - rpmOldMicros >= 41000))  //625rpm
    {
      sparkTriggerDelay =7000;
      Serial.print ( " RPM 625 " );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 41000) && (rpmCurrMicros - rpmOldMicros >= 30000)) //750rpm
    {
      sparkTriggerDelay =1670;
      Serial.print ( " RPM 750" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 30000) && (rpmCurrMicros - rpmOldMicros >= 25000)) //1000rpm
    {
      sparkTriggerDelay =1670;
      Serial.print ( " RPM 1000 " );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 25000) && (rpmCurrMicros - rpmOldMicros >= 23076)) //1200rpm
    {
      sparkTriggerDelay =125;
      Serial.print ( "RPM 1200 " );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 23076) && (rpmCurrMicros - rpmOldMicros >= 22222)) //just under 1300rpm
    {
      sparkTriggerDelay =20;
      Serial.print ( " RPM 1300 " );
    }
  else if (rpmCurrMicros - rpmOldMicros <= 22222)//just over 1300rpm
    {
      sparkTriggerDelay =0;
      Serial.print ( "RPM over 1300" );
    }
}

/*
void rpmConsultRatioTable()  //Very Retarded, for diagnostic purposes
{
  if (rpmCurrMicros - rpmOldMicros >= 497512) //sub250rpm
    {
      sparkTriggerDelay =0;
      startingDelay = false;
      startingDelayTimer = millis();
      Serial.print ( "RPM Zero" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 255024) && (rpmCurrMicros - rpmOldMicros >= 169491)) //125rpm
    {
      sparkTriggerDelay =162000;                     //was 57, then 59
      Serial.print ( "RPM 125" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 169491) && (rpmCurrMicros - rpmOldMicros >= 127512)) //177rpm
    {
      sparkTriggerDelay =78000;
      Serial.print ( "RPM 177" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 127512) && (rpmCurrMicros - rpmOldMicros >= 85008)) //250rpm
    {
      sparkTriggerDelay =64000;                    //was 27  //at 250 RPM and 45 degree sensor advance, this would be 30ms to TDC, subtract 2ms for coil charging and subtract more m.s. for advance 
      Serial.print ( "RPM 250" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 85008) && (rpmCurrMicros - rpmOldMicros >= 61000)) //375rpm
    {
      sparkTriggerDelay =32000;                 //was 16
      Serial.print ( "RPM 375" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 61000) && (rpmCurrMicros - rpmOldMicros >= 48000)) //500rpm
    {
      sparkTriggerDelay =26000;                 //was 11
      Serial.print ( "RPM 500" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 48000) && (rpmCurrMicros - rpmOldMicros >= 41000))  //625rpm
    {
      sparkTriggerDelay =14000;
      Serial.print ( "RPM 652" );
    }
  else if ((rpmCurrMicros - rpmOldMicros <= 41000) && (rpmCurrMicros - rpmOldMicros >= 31500)) //750rpm
    {
      sparkTriggerDelay =2670;
      Serial.print ( "RPM 750" );
    }
  else if (rpmCurrMicros - rpmOldMicros <= 31500) //just under 1000rpm
    {
      sparkTriggerDelay =0;
      Serial.print ( "RPM 1000" );
    }
}*/
