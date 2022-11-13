#include<EEPROM.h>
#include "Filter.h"
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd (0x27,20,4);

ExponentialFilter<float> FilteredGain(40, 0);
float Qtot=234000;
float Qmax=234000; //65AH in AMin
float soc,soc_batt,soh,sohb;
float ampb,amp;
float volt;
float suhu;
float bantubacaeeprom;

int currentAnalogInputPin = A1;             // Which pin to measure Current Value (A0 is reserved for LCD Display Shield Button function)
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
int a;
float FinalValue;
float inVB,inV;

float readVolt(){
  inVB=inV;
  int value = 0;
  float voltage = 0.0;
  float R1 = 30000.0;
  float R2 = 7500.0;
  value = analogRead(A0);
  voltage = value * (5.0/1023.0);
  inV = voltage / (R2/(R2+R1));
  if(abs(inVB-inV)>0.05){
    return inV;
  }
  else{
    inV=inVB;
    return inVB; 
  }
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
  testData=(((currentMean /1023.0) *supplyVoltage));
  

  if(abs(testData-testDataB)>0.5){
    FinalValue=testData*0.16939131-0.13606419+0.081818182;
    testDataB=testData;
  }
  else {
    FinalValue=testDataB*0.16939131-0.13606419+0.081818182;
  }
  currentSampleSum =0;                                                                              /* to reset accumulate sample values for the next cycle */
  currentSampleCount=0;   
  return FinalValue;  
}


void SOC(){
  amp=readAmp();
  if((amp>=-0.06)&& (amp<=0.06)){
    amp=0;
  }
  Serial.println("Amp= " +String(amp,3));
  soc=soc+(1*((amp+ampb)/2)); 
  soc_batt=(soc*100.0)/Qtot;
  if((amp==ampb) && (amp==0.0)){
    Qtot=soc;
    // EEPROM.put(0,Qtot);
  }
  ampb=amp;

}

void SOH(){
  sohb=soh;
  soh=(Qtot*100.0)/Qmax;
  if(sohb!=soh){  
    EEPROM.put(0,soh);
   }
}

void OCV(){
  float teg=readVolt();
  volt=teg;
  Serial.println("TEGANGAN BRO: "+String(teg));
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

  soc=(soc* Qtot)/100.0;
  Serial.println("HASIL OCV: "+String(soc));
}


void cetak(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(volt,2);
  lcd.print("V");
  if(amp<100){
    lcd.setCursor(12,0);
    lcd.print(amp,3);   
    lcd.print("A");
  }
  else {
    lcd.setCursor(12,0);
    lcd.print(amp,2);
    lcd.print("A");    
  }
  lcd.setCursor(0,1);
  lcd.print("SOC :");
  lcd.print(soc_batt,2);
  lcd.print("%");

  lcd.setCursor(0,2);
  lcd.print("SOH :");
  lcd.print(soh,2);
  lcd.print("%");

  lcd.setCursor(0,3);
  lcd.print("TEMP:");
  lcd.print(suhu,1);
  lcd.print("C");

  if(soh>90){
    lcd.setCursor(13,2);
    lcd.print("SANGAT");
    lcd.setCursor(14,3);
    lcd.print("BAIK");
  }
  else if (soh>20){
    lcd.setCursor(14,2);
    lcd.print("BAIK");
  }
  else{
    lcd.setCursor(13,2);
    lcd.print("buruk");
  }  

  lcd.display();
}

void(* resetFunc) (void) = 0;

void setup(){
  pinMode(2,INPUT_PULLUP);
  Serial.begin(9600);
  ampb=amp;
  Serial.println ("CLEARDATA");
  Serial.println ("LABEL, Volt, SOC ,battSOC , SOH");
  OCV();
  testDataB=testData;
  lcd.init();
  lcd.backlight();
  lcd.clear();
  EEPROM.get(0,bantubacaeeprom);
  if(isnan(bantubacaeeprom)){
    EEPROM.put(0,100.00);
    soh=100;
    Serial.println(String(1)+" "+String("write to eeprom"));
  }
  else{
    if(bantubacaeeprom<100.00){
      soh=bantubacaeeprom;
      Qtot=(bantubacaeeprom*Qmax)/100.0;  
      Serial.println(String(3)+" "+String(bantubacaeeprom,2)+" "+String(Qtot,3));    
    
    }
    else{
      soh=100;
      Serial.println(String(4)+" "+String(bantubacaeeprom,2));
    }
  }

  
}


unsigned long waktuTerakhir=1000;
void loop(){
  if(digitalRead(2)==LOW){
    EEPROM.put(0,100.00);  
    Serial.println("RESET EEPROM TO 100");
    resetFunc();
  }  
  if(abs(millis()-waktuTerakhir)>=1000){
    waktuTerakhir=millis();
    volt=readVolt();
    suhu = readSuhu();
    SOC();
    SOH();
    
    //Serial.println ( (String) "DATA," + volt + "," + soc + "," + soc_batt + "," + soh);

    Serial.println("Volt= " +String(volt,3));
    Serial.println("Arus= " +String(amp,3));
    Serial.println("SOC= " +String(soc,3));
    Serial.println("Batt SOC= " +String(soc_batt,3));
    Serial.println("SOH= " +String(soh,3));
    Serial.println("suhu= " +String(suhu,3));
    Serial.println("================================");

    cetak();
    //waktuTerakhir=millis();
  }
  delay(100);
}
