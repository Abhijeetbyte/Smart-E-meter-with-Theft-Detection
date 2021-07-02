// Meter B (Consumer  side meter )

#define BLYNK_PRINT Serial

//Dependiences

#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>



EnergyMonitor emon;
// calibrate() method calibrates zero point of sensor,
#define vCalibration 160
#define currCalibration 1


//Global variable

char auth[] = "MFP3HhJC8Yn8UEJV3N2pe_c9etEzFISx"; //Token for Meter-B
char ssid[] = "MASTER";
char pass[] = "20042006";

unsigned int Meter_A = 0; // create variable for Watts values received from Meter-A

int Relay = 13; //GPIO 13
int theft =  0; // Varible for Theft Aleart on Virtual Pin
int PD ; //Power difference

WidgetBridge bridge1(V10);
// Assign virtual pin 10 of this device for Bridge Widget
//to communicate with other Devices (Meter B) and the channel is named "bridge1"


void setup()
{
  // Debug console
  Serial.begin(9600);
   Blynk.begin(auth, ssid, pass, IPAddress(192,168,2,184));// Put you Local machine IPAddress if using Local Machine as server

  pinMode(Relay, OUTPUT);

  // ZMPT101B sensor connected to GPIO 35
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  // Acs712 sensor connected to GPIO 34
  emon.current(34, currCalibration); // Current: input pin, calibration.


}

void loop()
{

  //-------------------------------------------------Data Setup---------------------------------------

  emon.calcVI(20, 2000);
 // Serial.println(String("Current: ") + Amps + (" Volts: ") + emon.Vrms + (" Meter-B-Watts: ") + emon.apparentPower + ( "  Meter-A-Watts: ") + Meter_A + (" PD:") + PD) ;
 float Amps = emon.Irms;
  Blynk.virtualWrite(V0, emon.Vrms);
  Blynk.virtualWrite(V1, Amps);
  Blynk.virtualWrite(V2, emon.apparentPower);
  delay(100);

  Blynk.run();

  //---------------------------------------------------------Power Cutoff--------------------------------------------------

  PD = Meter_A - emon.apparentPower ;// Power difference in Meter A and Meter B
  if ((PD >= 10) && (PD >= -10)) {
    theft = 1 ;
    Blynk.virtualWrite(V3, theft);
    digitalWrite(Relay, LOW); // Reverse triger Relay (Low - on) &( High-off)
    delay(300);

  } else {
    digitalWrite(Relay, HIGH); //
    theft = 0 ;
    Blynk.virtualWrite(V3, theft);
    delay(300);
  }
}


// Power Feedback (Theft detection)

BLYNK_WRITE(V1) // V1 is th eVirtual Power Pin of Meter-A
{
  Meter_A = param.asInt();// Fetching Watts Data form Meter-A in integers

  delay(100);
}
