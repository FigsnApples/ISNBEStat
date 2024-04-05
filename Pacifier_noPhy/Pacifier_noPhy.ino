#include <LMP91000.h>
#include <Wire.h>
//#include <SparkFun_Bio_Sensor_Hub_Library.h> keep for biometric hub sensor
#include <Adafruit_TMP117.h>
#include <Adafruit_Sensor.h>
#include <MAX30105.h>
#include <heartRate.h>

Adafruit_TMP117  tmp117;
MAX30105 particleSensor;
MAX30105 particleSensor1;

LMP91000 pStat = LMP91000();

int16_t opVolt =3300; //milliVolts if working with a 3.3V device
uint8_t resolution = 10; //10-bits

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;
    

  void setup()
{
  Serial.begin(115200);
  Wire.begin();


  pStat.standby();
  delay(4000); //warm-up time for the gas sensor

  pStat.standby();
  delay(1000); //warm-up time for the gas sensor
    
    // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))
  {
    //debug.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  if (particleSensor1.begin(Wire1) == false)
  {
    //debug.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  particleSensor.setup(); //Configure sensor. Use 6.4mA for LED drive
  particleSensor1.setup();
}

 
  //runAmp(4, 0, 0,300, 10000, 0, 0, 60, 6); 
//  (gain, pre step voltage, quiettime, v1, t1, v2, t2, samples, range)


void loop()
{
  
  while (true){
    unsigned long startTime2 = millis();
    unsigned long elapsedTime = 0;
    unsigned long duration =30000;
  
   while ((millis() - startTime2) < duration) 
   {
  // elapsedTime = millis() - startTime2;
  // long irValue = particleSensor.getIR();

  // if (checkForBeat(irValue) == true)
  // {
  //   //We sensed a beat!
  //   long delta = millis() - lastBeat;
  //   lastBeat = millis();

  //   beatsPerMinute = 60 / (delta / 1000.0);

  //   if (beatsPerMinute < 255 && beatsPerMinute > 20)
  //   {
  //     rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
  //     rateSpot %= RATE_SIZE; //Wrap variable

  //     //Take average of readings
  //     beatAvg = 0;
  //     for (byte x = 0 ; x < RATE_SIZE ; x++)
  //       beatAvg += rates[x];
  //     beatAvg /= RATE_SIZE;
  //   }
  // }

  // // Serial.print("IR=");
  // // Serial.print(irValue);
  // Serial.print("BPM=");
  // Serial.print(beatsPerMinute);
  // Serial.print(", Avg BPM=");
  // Serial.print(beatAvg);
  // Serial.print(",");

  // if (irValue < 50000)
  //   Serial.print(" No finger?");

  // Serial.println();
      
    // Add a delay to control the loop rate (optional)
    //delay(1000); // Add a 1-second delay between iterations
    Serial.print(particleSensor.getIR());
    Serial.print(",");
    Serial.println(particleSensor1.getIR());
 }
   runAmp(4, 0, 0,300, 60000, 0, 0, 120, 6); 

}
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
    Serial.println("Beginning LMP Loop");
     // create an empty event to be filled
    while(millis() - startTime < timeArray[i])
    {
      tmp117.begin();
      sensors_event_t temp;
      tmp117.getEvent(&temp);//fill the empty event object with the current measurements
      //Serial.print((int16_t)(opVolt*TIA_BIAS[abs(voltageArray[i])]*(voltageArray[i]/abs(voltageArray[i]))));
      //Serial.print(",");
      Serial.print(millis()/1000);
      Serial.print(",");
      Serial.print(analogRead(A0)*(3300/1023));
      Serial.print(",");
      Serial.print(pow(10,range)*pStat.getCurrent(pStat.getOutput(A0), opVolt/1000.0, resolution));
      Serial.print(",");
     //Serial.print("HR (bpm)");
      // Serial.print(",");
      //Serial.print(particleSensor.getIR());
      //Serial.print(", "); 
      // Serial.print("Oxygen (%)");
      // Serial.print(",");
      // Serial.print(",");
      // Serial.print("Temp"); 
      Serial.print(temp.temperature);
      Serial.println("C");
      // Serial.println("");
      delay(fs);
      
    }
  }

  //End at 0V
  pStat.setBias(6);
  Serial.println("Ended Amperometric Scan but keeping bias, moving to Loop");
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