// Meter B (Consumer  side meter )

#define BLYNK_PRINT Serial

//Dependiences
#include "ZMPT101B.h"
#include "ACS712.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// We have ZMPT101B sensor connected
ZMPT101B voltageSensor(35); //GPIO 35

// We have 5 amps version sensor connected
ACS712 currentSensor(ACS712_05B, 34);



//Global variable

char auth[] = "MFP3HhJC8Yn8UEJV3N2pe_c9etEzFISx"; //Token for Meter-B
char ssid[] = "MASTER";
char pass[] = "20042006";

float Meter_A = 0; // create variable for Watts values received from Meter-A

int Relay = 13; //GPIO 13
int theft =  0; // Varible for Theft Aleart on Virtual Pin
uint8_t PD = 0; //Power difference

WidgetBridge bridge1(V10);
// Assign virtual pin 10 of this device for Bridge Widget
//to communicate with other Devices (Meter B) and the channel is named "bridge1"


void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(Relay, OUTPUT);
  // calibrate() method calibrates zero point of sensor,
  // It is not necessary, but may positively affect the accuracy
  // Ensure that no current flows through the sensor at this moment
  // If you are not sure that the current through the sensor will not leak during calibration - comment out this method
  Serial.println("Calibrating... Ensure that no current flows through the sensor at this moment");
  delay(100);
  voltageSensor.calibrate();
  currentSensor.calibrate();
  Serial.println("Done!");
}

void loop()
{

  //-------------------------------------------------Measuring part---------------------------------------
  // To measure voltage/current we need to know the frequency of voltage/current
  // By default 50Hz is used, but you can specify desired frequency
  // as first argument to getVoltageAC and getCurrentAC() method, if necessary

  float Volts = voltageSensor.getVoltageAC();
  float Amps = currentSensor.getCurrentAC();

  // To calculate the power we need voltage multiplied by current
  // VOLTAGE and  CURRENT Offset's for correction
  Volts = 100 + Volts;
  Amps = Amps - 0.94;
  float watt = Volts * Amps ;


  //----------------------------------------------------Data setup-----------------------------------------------------------------
  delay(100);
  Serial.println(String("Current: ") + Amps + (" Volts: ") + Volts + (" Meter-B-Watts: ") + watt + ( "  Meter-A-Watts: ") + Meter_A + (" PD:") + PD) ;
  delay(1500);
  Blynk.virtualWrite(V0, Volts);
  Blynk.virtualWrite(V1, Amps);
  Blynk.virtualWrite(V2, watt);


  Blynk.run();

  //---------------------------------------------------------Power Cutoff--------------------------------------------------

  PD = watt - Meter_A ;// Power difference in Meter A and Meter B
  if (PD > 200) {
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
  Meter_A = param.asFloat();

  delay(100);
}
