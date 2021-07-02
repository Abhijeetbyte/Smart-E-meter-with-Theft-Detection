// Meter A (disturbution side meter )

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

char auth[] = "XUX_m5m1mOAlfD0jLelsjXmYPpitBDBf";//Token for Meter-A
char ssid[] = "MASTER";
char pass[] = "20042006";



//Bridge Widget Blynk Server
WidgetBridge bridge1(V10);


BLYNK_CONNECTED()
{
  bridge1.setAuthToken("MFP3HhJC8Yn8UEJV3N2pe_c9etEzFISx");  //(Meter B token )allow permission to write the values to Meter B
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  WiFi.begin();
  Blynk.begin(auth, ssid, pass, IPAddress(192,168,2,184));// Put you Local machine IPAddress if using Local Machine as server

  // ZMPT101B sensor connected to GPIO 35
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  // Acs712 sensor connected to GPIO 34
  emon.current(34, currCalibration); // Current: input pin, calibration.

}

void loop()
{

  //-------------------------------------------------Data Setup---------------------------------------

  emon.calcVI(20, 2000);

 // Serial.println(String(" A ") + emon.Irms  + (" Voltage ") + emon.Vrms + (" Watts ") + emon.apparentPower );
  delay(100);

  Blynk.virtualWrite(V0, emon.Irms );// display amps temporary

  bridge1.virtualWrite(V1, emon.apparentPower);  // send Watt to Device B (Consumer meter for match )web interface
  delay(100);

  Blynk.run();
}
