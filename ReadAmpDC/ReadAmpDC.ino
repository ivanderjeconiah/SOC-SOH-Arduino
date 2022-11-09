#include "Filter.h"

ExponentialFilter<float> FilteredGain(30, 0);
ExponentialFilter<float> FilteredZero(30, 0);
int currentAnalogInputPin = A1;             // Which pin to measure Current Value (A0 is reserved for LCD Display Shield Button function)
int calibrationPin = A2;                    // Which pin to calibrate offset middle value
float manualOffset = 0.00;                  // Key in value to manually offset the initial value
float mVperAmpValue = 6.25;                 // If using "Hall-Effect" Current Transformer, key in value using this formula: mVperAmp = maximum voltage range (in milli volt) / current rating of CT
                                                    // For example, a 20A Hall-Effect Current Transformer rated at 20A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 20A = 31.25mV/A 
                                                    // For example, a 50A Hall-Effect Current Transformer rated at 50A, 2.5V +/- 0.625V, mVperAmp will be 625 mV / 50A = 12.5 mV/A
float supplyVoltage = 5000;                 // Analog input pin maximum supply voltage, Arduino Uno or Mega is 5000mV while Arduino Nano or Node MCU is 3300mV
float offsetSampleRead = 0;                 /* to read the value of a sample for offset purpose later */
float currentSampleRead  = 0;               /* to read the value of a sample including currentOffset1 value*/
float currentLastSample  = 0;               /* to count time for each sample. Technically 1 milli second 1 sample is taken */
float currentSampleSum   = 0;               /* accumulation of sample readings */
float currentSampleCount = 0;               /* to count number of sample. */
int currentMean ;                         /* to calculate the average value from all samples, in analog values*/ 
float RMSCurrentMean ;                      /* square roof of currentMean, in analog values */   
float FinalRMSCurrent ; 
float testData,testDataB;
int a,b;

void setup() {
  Serial.begin(9600);
}

void loop() {
  testDataB=testData;
  while(currentSampleCount<=5000){
    a=analogRead(currentAnalogInputPin);
    FilteredGain.Filter(a);
    a=FilteredGain.Current();
    currentSampleRead = a-511;                  /* read the sample value including offset value*/
    currentSampleSum = currentSampleSum + currentSampleRead ;                                      /* accumulate total analog values for each sample readings*/
    currentSampleCount = currentSampleCount + 1;                                                       /* to count and move on to the next following count */  
    delay(0.1);  
  }
  currentMean = (currentSampleSum/currentSampleCount);                                                /* average accumulated analog values*/                                                              /* square root of the average value*/
  FinalRMSCurrent = ((((currentMean /1023.0) *supplyVoltage)) /mVperAmpValue)- manualOffset;         /* calculate the final RMS current*/
  testData=(((currentMean /1023) *supplyVoltage));
  
  Serial.print("The Current RMS value is: ");
  Serial.print(FinalRMSCurrent,3);
  Serial.println(" A ");


  if(abs(testData-testDataB)>15){
    Serial.print("RAW Voltage : ");
    Serial.print(testData,2);
    Serial.println(" mV");
  }
  else {
    Serial.print("RAW Voltage : ");
    Serial.print(testDataB,2);
    Serial.println(" mV UNCHANGEABLE");
  }
  
  Serial.println("CurrentMean in  ADC : " + String(currentMean));
  currentSampleSum =0;                                                                              /* to reset accumulate sample values for the next cycle */
  currentSampleCount=0;     
}
