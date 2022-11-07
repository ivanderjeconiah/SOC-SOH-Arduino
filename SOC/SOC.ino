//#include<EEPROM.h>

float Qtot=180;
float Qmax=180; //3AH in AMin
float soc,soc_batt,soh;
float ampb,amp;
float volt;
float suhu;
float bantubacaeeprom;


int currentAnalogInputPin = A1;             // Which pin to measure Current Value (A0 is reserved for LCD Display Shield Button function)
int calibrationPin = A2;                    // Which pin to calibrate offset middle value
float manualOffset = 0.00;                  // Key in value to manually offset the initial value
float mVperAmpValue = 12.5;                 // If using "Hall-Effect" Current Transformer, key in value using this formula: mVperAmp = maximum voltage range (in milli volt) / current rating of CT
                                                    // For example, a 20A Hall-Effect Current Transformer rated at 20A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 20A = 31.25mV/A 
                                                    // For example, a 50A Hall-Effect Current Transformer rated at 50A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 50A = 12.5 mV/A
float supplyVoltage = 5000;                 // Analog input pin maximum supply voltage, Arduino Uno or Mega is 5000mV while Arduino Nano or Node MCU is 3300mV
float offsetSampleRead = 0;                 /* to read the value of a sample for offset purpose later */
float currentSampleRead  = 0;               /* to read the value of a sample including currentOffset1 value*/
float currentLastSample  = 0;               /* to count time for each sample. Technically 1 milli second 1 sample is taken */
float currentSampleSum   = 0;               /* accumulation of sample readings */
float currentSampleCount = 0;               /* to count number of sample. */
float currentMean ;                         /* to calculate the average value from all samples, in analog values*/ 
float RMSCurrentMean ;                      /* square roof of currentMean, in analog values */   
float FinalRMSCurrent ; 

float readVolt(){
  int value = 0;
  float voltage = 0.0;
  float inV = 0.0;
  float R1 = 30000.0;
  float R2 = 7500.0;
  value = analogRead(A0);
  voltage = value * (5.0/1023.0);
  inV = voltage / (R2/(R2+R1));
  return inV;  
}

float readSuhu(){
  const int sensortemp = A2;
  int tempc;
  float vout;
  float adc; 
  adc =analogRead(sensortemp);
  vout= adc /1023.0 *5 ;
  tempc = vout * 100;
  return tempc;
 
}

float readAmp(){
  while(currentSampleCount<=4000){
    currentSampleRead = analogRead(currentAnalogInputPin)-analogRead(calibrationPin);                  /* read the sample value including offset value*/
    currentSampleSum = currentSampleSum + sq(currentSampleRead) ;                                      /* accumulate total analog values for each sample readings*/
    currentSampleCount = currentSampleCount + 1;                                                       /* to count and move on to the next following count */  
    currentLastSample = micros();
    delay(0.2);  
  }
  currentMean = currentSampleSum/currentSampleCount;                                                /* average accumulated analog values*/
  RMSCurrentMean = sqrt(currentMean);                                                               /* square root of the average value*/
  FinalRMSCurrent = (((RMSCurrentMean /1023) *supplyVoltage) /mVperAmpValue)- manualOffset;         /* calculate the final RMS current*/
  if(FinalRMSCurrent <= (625/mVperAmpValue/100))                                                    /* if the current detected is less than or up to 1%, set current value to 0A*/
  { 
    FinalRMSCurrent =0; 
  }
  Serial.print(" The Current RMS value is: ");
  Serial.print(FinalRMSCurrent,3);
  Serial.println(" A ");
  currentSampleSum =0;                                                                              /* to reset accumulate sample values for the next cycle */
  currentSampleCount=0;     
  return FinalRMSCurrent;
}

void SOC(){
  amp=readAmp();
  Serial.println("Amp= " +String(amp,3));
  soc=soc+(1*((amp+ampb)/2)); 
  soc_batt=(soc*100.0)/Qtot;
  if((amp==ampb) && (amp<=0.0)){
    Qtot=soc;
   // EEPROM.put(0,Qtot);
  }
  ampb=amp;
}

void SOH(){
  soh=(Qtot*100.0)/Qmax;
}

void OCV(){
  int teg=readVolt();
  if(teg>12.7){
    soc=100;
  }
  else if(teg>12.5){
    soc=50*teg-535;
  }
  else if(teg>12.32){
    soc=110.6557377*teg-1293.60655738;
  }
  else if(teg>12.06){
    soc=76.77165354*teg-876.1023622;  
  }
  else if(teg>11.58){
    soc=62.85827239*teg-708.14192528;
  }
  else if(teg>11.31){
    soc=37.03703704*teg-408.88888889; 
  }
  else if(teg>=10.5){
    soc=12.34567901*teg-129.62962963;
  }
  else{
    soc=0;
  }
}

void setup(){
  Serial.begin(9600);
  ampb=amp;
  Serial.println ("CLEARDATA");
  Serial.println ("LABEL, Volt, SOC ,battSOC , SOH");
  OCV();
 // EEPROM.get(0,bantubacaeeprom);
 // if(bantubacaeeprom!=0){
 //   Qtot=bantubacaeeprom;
 // }
}

void loop(){
  volt=readVolt();
  suhu = readSuhu();
  SOC();
  SOH();
  Serial.println ( (String) "DATA," + volt + "," + soc + "," + soc_batt + "," + soh);

  Serial.println("Volt= " +String(volt,3));
  Serial.println("SOC= " +String(soc,3));
  Serial.println("Batt SOC= " +String(soc_batt,3));
  Serial.println("SOH= " +String(soh,3));
    Serial.println("suhu= " +String(suhu,3));
  Serial.println("================================");
  delay(1000);

}
