//Fully Integrated System with PhyPhox functionality, SpO2, HR, Temperature, and 3 Lead Potentiostat


#include <LMP91000.h>
#include <Wire.h>
#include <phyphoxBle.h>
#include <SparkFun_Bio_Sensor_Hub_Library.h>
#include <Adafruit_TMP117.h>
#include <Adafruit_Sensor.h>

Adafruit_TMP117  tmp117;
LMP91000 pStat = LMP91000();

int16_t opVolt =5000; //milliVolts if working with a 3.3V device
uint8_t resolution = 10; //10-bits

  void setup()
{
  Serial.begin(115200);
  Wire.begin();

    //PhyPhox Setup
  PhyphoxBLE::start("XiaoSense"); //Name of BT Device
  PhyphoxBleExperiment XiaoSense;  // Name of Experiment
  XiaoSense.setTitle("XiaoSense");

   //View
  PhyphoxBleExperiment::View firstView; //Create a "view (like a tab/window)"
  firstView.setLabel("View 1");
  //PhyphoxBleExperiment::View secondView;
  //secondView.setLabel("Temperature");  

  //Graph
  PhyphoxBleExperiment::Graph firstGraph; //Create graph for amperometric data
  firstGraph.setLabel("Amperometric Data");
  firstGraph.setUnitX("s");
  firstGraph.setUnitY("uA");
  firstGraph.setLabelX("Time");
  firstGraph.setLabelY("Current");
  firstGraph.setYPrecision(1);
  firstGraph.setColor("2E728E");
  firstGraph.setChannel(0, 1); //assign channel 
//Graph 2
  //Separator
  PhyphoxBleExperiment::Separator mySeparator;      //Creates a line to separate elements.
  mySeparator.setHeight(1);                       //Sets height of the separator.
  mySeparator.setColor("404040");                   //Sets color of the separator. Uses a 6 digit hexadecimal value in "quotation marks".

  //Value
  PhyphoxBleExperiment::Value myValue;         //Creates a value-box.
  myValue.setLabel("Heart Rate");                  //Sets the label 
  myValue.setPrecision(2);                     //The amount of digits shown after the decimal point.
  myValue.setUnit("(bpm)");                        //The physical unit associated with the displayed value.
  myValue.setColor("FF0000");                  //Sets font color. Uses a 6 digit hexadecimal value in "quotation marks".
  myValue.setChannel(2);
  myValue.setXMLAttribute("size=\"2\"");

  PhyphoxBleExperiment::Separator mySeparator1;      //Creates a line to separate elements.
  mySeparator.setHeight(1);                       //Sets height of the separator.
  mySeparator.setColor("404040");                   //Sets color of the separator. Uses a 6 digit hexadecimal value in "quotation marks".

  PhyphoxBleExperiment::Value myValuee;         //Creates a value-box.
  myValuee.setLabel("Oxygen");                  //Sets the label
  myValuee.setPrecision(2);                     //The amount of digits shown after the decimal point.
  myValuee.setUnit("(%)");                        //The physical unit associated with the displayed value.
  myValuee.setColor("FFC0CB");                  //Sets font color. Uses a 6 digit hexadecimal value in "quotation marks".
  myValuee.setChannel(3);
  myValuee.setXMLAttribute("size=\"2\"");
  /* Assign Channels, so which data is plotted on x or y axis
     first parameter represents x-axis, second y-axis
     Channel 0 means a timestamp is created after the BLE package arrives in phyphox
     Channel 1 to N corresponding to the N-parameter which is written in server.write()
  */
  PhyphoxBleExperiment::Separator mySeparator2;      //Creates a line to separate elements.
  mySeparator.setHeight(1);                       //Sets height of the separator.
  mySeparator.setColor("404040"); 

  PhyphoxBleExperiment::Value tempy;         //Creates a value-box.
  tempy.setLabel("Temperature");                  //Sets the label
  tempy.setPrecision(2);                     //The amount of digits shown after the decimal point.
  tempy.setUnit("C");                        //The physical unit associated with the displayed value.
  tempy.setColor("FFA500");                  //Sets font color. Uses a 6 digit hexadecimal value in "quotation marks".
  tempy.setChannel(4);
  tempy.setXMLAttribute("size=\"2\"");

  firstView.addElement(firstGraph);
  firstView.addElement(myValue);             //attach graph to view
  firstView.addElement(myValuee);
  firstView.addElement(tempy);
  XiaoSense.addView(firstView);                //Attach view to experiment
 // XiaoSense.addView(secondView);
  PhyphoxBLE::addExperiment(XiaoSense); 

  pStat.standby();
  delay(4000); //warm-up time for the gas sensor
 
  runAmp(4, 0, 0,300, 60000, 0, 0, 60, 6); 
//  (gain, pre step voltage, quiettime, v1, t1, v2, t2, samples, range)
}


void loop()
{
  

}

//range = 12 is picoamperes
//range = 9 is nanoamperes
//range = 6 is microamperes
//range = 3 is milliamperes
void runAmp(uint8_t user_gain, int16_t pre_stepV, uint32_t quietTime, int16_t v1,
            uint32_t t1, int16_t v2, uint32_t t2, uint16_t samples, uint8_t range)
{
  pStat.disableFET();
  pStat.setGain(user_gain); //sets the transimpedance amplier gain. inputs are [0:7]
  pStat.setRLoad(1); //sets internal load RLoad selection, inputs are [0:3]
  pStat.setIntRefSource(); //sets the reference source of the LMP91000 to an external reference provided at the VRef Pin
  pStat.setIntZ(1); //sets interal zero - particulary the TIA selection - inputs can be[0:3]
  pStat.setThreeLead();  //not sure here, but this seems very important - the .cpp file outlines this lines 461-471 
  pStat.setBias(0);
  
  digitalWrite(LEDG,HIGH);

//Reset pin, MFIO pin
int resPin = 8;
int mfioPin = 7;

// Takes address, reset pin, and MFIO pin.
SparkFun_Bio_Sensor_Hub bioHub(resPin, mfioPin); 

bioData body;

int result = bioHub.begin();
int error = bioHub.configBpm(MODE_ONE);

  //Print column headers
  String current = "";
  if(range == 12) current = "Current(pA)";
  else if(range == 9) current = "Current(nA)";
  else if(range == 6) current = "Current(uA)";
  else if(range == 3) current = "Current(mA)";
  else current = "SOME ERROR";
  
  Serial.println("Voltage(mV),Time(s),Vout(mV)" + current);

  int16_t voltageArray[3] = {pre_stepV, v1, v2};
  uint32_t timeArray[3] = {quietTime, t1, t2};

  //i = 0 is pre-step voltage
  //i = 1 is first step potential
  //i = 2 is second step potential
  for(uint8_t i = 0; i < 3; i++)
  {
    //For pre-step voltage
    uint32_t fs = timeArray[i]/samples;
    voltageArray[i] = determineLMP91000Bias(voltageArray[i]);
  
    if(voltageArray[i] < 0) pStat.setNegBias();
    else pStat.setPosBias();
  
    unsigned long startTime = millis();
    pStat.setBias(abs(voltageArray[i]));
    while(millis() - startTime < timeArray[i])
    {
      body = bioHub.readBpm();
      tmp117.begin();
      sensors_event_t temp;
      tmp117.getEvent(&temp);
      //Serial.print((int16_t)(opVolt*TIA_BIAS[abs(voltageArray[i])]*(voltageArray[i]/abs(voltageArray[i]))));
      //Serial.print(",");
      Serial.print(millis()/1000);
      Serial.print(",");
      Serial.print(analogRead(A0)*(3300/1023));
      Serial.print(",");
      Serial.print(pow(10,range)*pStat.getCurrent(pStat.getOutput(A0), opVolt/1000.0, resolution));
      Serial.print(",");
      Serial.print("HR (bpm)");
      Serial.print(",");
      Serial.print(body.heartRate);
      Serial.print(", "); 
      Serial.print("Oxygen (%)");
      Serial.print(",");
      Serial.print(body.oxygen);
      Serial.print("Temp"); 
      Serial.print(",");
      Serial.print(temp.temperature);
      Serial.println("C");
      delay(fs);
      float current = pow(10,range)*pStat.getCurrent(pStat.getOutput(A0), opVolt/1000.0, resolution);
      float HR = (body.heartRate);
      float Otwo = (body.oxygen);
      PhyphoxBLE::write(current,HR,Otwo,temp.temperature);
      delay(fs);
      
    }
  }

  //End at 0V
  pStat.setBias(0);
}


signed char determineLMP91000Bias(int16_t voltage)
{
  signed char polarity = 0;
  if(voltage < 0) polarity = -1;
  else polarity = 1;
  
  int16_t v1 = 0;
  int16_t v2 = 0;
  
  voltage = abs(voltage);

  if(voltage == 0) return 0;
  
  for(int i = 0; i < NUM_TIA_BIAS-1; i++)
  {
    v1 = opVolt*TIA_BIAS[i];
    v2 = opVolt*TIA_BIAS[i+1];

    if(voltage == v1) return polarity*i;
    else if(voltage > v1 && voltage < v2)
    {
      if(abs(voltage-v1) < abs(voltage-v2)) return polarity*i;
      else return polarity*(i+1);
    }
  }
  return 0;
}
