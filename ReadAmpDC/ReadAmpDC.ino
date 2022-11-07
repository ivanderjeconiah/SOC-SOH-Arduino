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
float testData;

void setup() {
  Serial.begin(9600);
}

void loop() {
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
  testData=((RMSCurrentMean /1023) *supplyVoltage);
  //testData=0.17347358*testData+0.08501957;
  // if(FinalRMSCurrent <= (625/mVperAmpValue/100))                                                    /* if the current detected is less than or up to 1%, set current value to 0A*/
  // { 
  //   FinalRMSCurrent =0; 
  // }
  
  Serial.print(" The Current RMS value is: ");
  Serial.print(FinalRMSCurrent,3);
  Serial.println(" A ");

  Serial.print("RAW Voltage : ");
  Serial.print(testData,3);
  Serial.println(" V");
  currentSampleSum =0;                                                                              /* to reset accumulate sample values for the next cycle */
  currentSampleCount=0;     
}
