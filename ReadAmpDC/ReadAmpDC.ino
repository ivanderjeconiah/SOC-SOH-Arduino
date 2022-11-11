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
float currentMean ;                         /* to calculate the average value from all samples, in analog values*/ 
float RMSCurrentMean ;                      /* square roof of currentMean, in analog values */   
float FinalRMSCurrent ; 
float testData,testDataB;
int a,b;
float FinalValue;

void setup() {
  Serial.begin(9600);
  testDataB=testData;
}

void loop() {
 testDataB=testData;   
}
