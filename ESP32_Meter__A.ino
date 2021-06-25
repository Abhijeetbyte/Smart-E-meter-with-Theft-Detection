// Meter A (disturbution side meter )

#define BLYNK_PRINT Serial

//Dependiences

#include "ZMPT101B.h"
#include "ACS712.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//Global variable

char auth[] = "XUX_m5m1mOAlfD0jLelsjXmYPpitBDBf";//Token for Meter-A
char ssid[] = "MASTER";
char pass[] = "20042006";

// We have ZMPT101B sensor connected
ZMPT101B voltageSensor(35); //GPIO 35

// We have 5 amps version sensor connected
ACS712 currentSensor(ACS712_05B, 34);

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
  Blynk.begin(auth, ssid, pass);

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
  Amps = Amps - 0.70;
  float Watt = Volts * Amps ;

  //----------------------------------------------------Data setup-----------------------------------------------------------------

  Serial.println(String(" A ") + Amps + (" Watts ") + Watt + (" Voltage ") + Volts  );
  delay(100);

  Blynk.virtualWrite(V0, Amps );// display amps temporary
  //Blynk.virtualWrite(V1, Watt );// display watts  temporary

  bridge1.virtualWrite(V1, Watt);  // send Watt to Device B (Consumer meter for match )web interface
  delay(100);

  Blynk.run();
}
