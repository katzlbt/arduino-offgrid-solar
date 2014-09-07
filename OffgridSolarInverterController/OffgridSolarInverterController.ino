#include <JeeLib.h>
#include <Ports.h>
//#include <RF12.h>
//#include <RF12sio.h>

// HARDWARE: 3V JeeNode for low power consumption, low power sleep state
// is arduino compatible by removing JeeLib.h, Ports.h for Sleepy::loseSomeTime
// http://jeelabs.net/projects/hardware/wiki/JeeNode

// define pins of JeeNode Ports
const int P1D = 4;
const int P1A = 14;

const int P2D = 5;
const int P2A = 15;

const int P3D = 6;
const int P3A = 16;

const int P4D = 7;
const int P4A = 17;

const int ledPin = P3D;
  // just for debugging and fun
  
const int inverterPulsePin = P1D;
  // 1 x 500ms PULSE to toggle inverter on and off, 
  // requires inverterOnSensorPin to switch to a defined state
  
const int inverterOnSensorPin = P1A;
  // high if HQ inverter is on 
  
const int lowBatterySensorPin = P4D; 
  // TODO: UNUSED! Charge-Regulator output turned off 12V -> 0V

const int manual220vRequestPin = P2D;  
  // PULLUP, LOW = pushed, TASTER, 
  // switches power on for 5 minutes when pushed

//const int manual12vRequestPin = P;
  // PULLUP, LOW = pushed, TASTER
  // TODO request 12v switched LIGHT power via a MOSFET output
//const int manual12vPowerOutputPin = P;  // UNUSED: to acitvate MOSFET for 12V LIGHT supply

EMPTY_INTERRUPT(WDT_vect); // for Sleepy::loseSomeTime(10000);

#define ACTIVATE_SERIAL 0

#ifndef ACTIVATE_SERIAL
#define Serial if(0) Serial
#endif

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);

  pinMode(ledPin,OUTPUT);
  pinMode(inverterPulsePin,OUTPUT);
  
  pinMode(inverterOnSensorPin,INPUT);
  pinMode(manual220vRequestPin,INPUT_PULLUP);
  pinMode(lowBatterySensorPin,INPUT);
  
  digitalWrite(ledPin,HIGH);
  delay(500);
  digitalWrite(ledPin,LOW);
  delay(200);
  digitalWrite(ledPin,HIGH);
  delay(200);
  digitalWrite(ledPin,LOW);
  delay(200);
  digitalWrite(ledPin,HIGH);
  delay(500);
  digitalWrite(ledPin,LOW);
  delay(1000);
  
  powerOff();
}

boolean power230V = false;
unsigned int minutesRunning = 0; // since arduino start, will wrap

long manualPowerRequestMsec = 0; // set to n minutes and power will be on for that long regardless of cycle

void readStatus()
{  
    power230V = LOW == digitalRead(inverterOnSensorPin); // negated! HIGH/+12V if OFF
    minutesRunning = millis()/1000/60;
    
    if(power230V)
    {
        //digitalWrite(ledPin,HIGH);
        Serial.print("STATUS: 230V ON");
    }
    else
    {
        Serial.print("STATUS: OFF");
        //digitalWrite(ledPin,LOW);
    }
  
    Serial.print(" MANUAL: ");
    Serial.print(manualPowerRequestMsec/60);
    Serial.print(" MIN: ");
    Serial.println(minutesRunning);
}

void powerOn()
{
    digitalWrite(ledPin,HIGH);
    
    if(power230V)
       return;
       
    Serial.println("POWER ON");
    digitalWrite(inverterPulsePin,HIGH);
    delay(500);
    digitalWrite(inverterPulsePin,LOW);
}

void powerOff()
{
    digitalWrite(ledPin,LOW);
    
    if(!power230V)
        return;
    
    Serial.println("POWER OFF!");
    digitalWrite(inverterPulsePin,HIGH);
    delay(500);
    digitalWrite(inverterPulsePin,LOW);
}

// Interval for one cycle and how many minutes power on in the cycle
const int minutesInterval = 60;
const int minutesOn = 6;
// 60/10 or 30/5 power = 100W 4h/24h max. 0.4 kWh max.
// 60/5 power = 100W 2h/24h max. 0.2 kWh max.
// 30/3 

const int mainLoopDelayMsec = 1000;

void sleepDelay()
{
    #ifdef ACTIVATE_SERIAL
       Serial.println("... zzZZzz ...");
       delay(mainLoopDelayMsec); // USE for serial monitor
    #else
       Sleepy::loseSomeTime(mainLoopDelayMsec);  // sleep 60s, needs EMPTY_INTERRUPT(WDT_vect);
    #endif  
}

boolean buttonDebouncer = true;

void loop() // once per minute
{
    readStatus();
    
    /*
    // BATTERY PROTECTION from charge regulator sensor output
    if(LOW == digitalRead(lowBatterySensorPin))
    {
       powerOff();
       return;  // >>>> EXIT until battery is OK again
    }
    */
    
    if(buttonDebouncer &&
      manualPowerRequestMsec < 10*60000 && // max 10 mins
      LOW == digitalRead(manual220vRequestPin))  // INTERNAL PULLUP PIN
    {
       manualPowerRequestMsec += 5*60000; // 5 min
       Serial.println("manualPowerRequestMinutes +5");
       buttonDebouncer = false;
    }
    
    if(manualPowerRequestMsec > 0)
    {
       powerOn();
       manualPowerRequestMsec -= mainLoopDelayMsec;
       sleepDelay();
       buttonDebouncer = true;
       return;  // >>>> EXIT until manual mode ends!
    }
    
    // standard procedure switch on in intervals   
    if(minutesRunning % minutesInterval < minutesOn) // run 10 mins per 1h
       powerOn();
    else
       powerOff();
    
    readStatus();
    sleepDelay();
    buttonDebouncer = true;
}
