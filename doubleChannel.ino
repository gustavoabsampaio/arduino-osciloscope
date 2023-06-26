const int max_storage = 100;
int valores0[max_storage];
int valores1[max_storage];
int idx = 0;
bool analogPin = 0;

volatile int readFlag;
volatile int analogVal = 0;

//  Initialization
void setup(){
  Serial.begin(9600);
  //  clear ADLAR in ADMUX (0x7C) to right-adjust the result
  //  ADCL will contain lower 8 bits, ADCH upper 2 (in last two bits)
  ADMUX &= B11011111;
 
  //  Set REFS1..0 in ADMUX (0x7C) to change reference voltage to the
  //  proper source (01)
  ADMUX |= B01000000;
 
  //  Clear MUX4..0 in ADMUX (0x7C) in preparation for setting the analog
  //  input
  ADMUX &= B11100000;
  //  Clear MUX5 in ADSCRB
  ADCSRB &= B11110111;
  
  //  Set MUX4..0 in ADMUX
  //  The final number may include MUX5 in ADSCRB
  //  Table 26-4 on https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf
  ADMUX |= B00000011;
  //  Set MUX5 in ADSCRB
  ADCSRB |= B1000;
 
  //  Set ADEN in ADCSRA (0x7A) to enable the ADC.
  //  Note, this instruction takes 12 ADC clocks to execute
  ADCSRA |= B10000000;
 
  //  Set ADATE in ADCSRA (0x7A) to enable auto-triggering.
  ADCSRA |= B00100000;
 
  //  Clear ADTS2..0 in ADCSRB (0x7B) to set trigger mode to free running.
  //  This means that as soon as an ADC has finished, the next will be
  //  immediately started.
  ADCSRB &= B11111000;
 
  //  Set the Prescaler to 128 (16000KHz128 = 125KHz)
  //  Above 200KHz 10-bit results are not reliable.
  ADCSRA |= B00000111;
 
  //  Set ADIE in ADCSRA (0x7A) to enable the ADC interrupt.
  //  Without this, the internal interrupt will not trigger.
  ADCSRA |= B00001000;
 
  //  Enable global interrupts
  //  AVR macro included in avrinterrupts.h, which the Arduino IDE
  //  supplies by default.
  sei();
 
  //  Kick off the first ADC
  // readFlag = 0;
  //  Set ADSC in ADCSRA (0x7A) to start the ADC conversion
  ADCSRA |= B01000000;
}


 // Processor loop
void loop(){
  //  Check to see if the value has been updated
  if (readFlag == 1){
    if (idx == max_storage - 1) {
      noInterrupts();
      serialPrint();
    }
    readFlag = 0;
  }
 
  //  Whatever else you would normally have running in loop().
 
}

void serialPrint() {
  for(int i = 0; i < max_storage; i++) {
    Serial.print("Variable_1:");
    Serial.print(valores0[i]);
    Serial.print(",");
    Serial.print("Variable_2:");
    Serial.println(valores1[i]);
  }
  interrupts();
}
//  Interrupt service routine for the ADC completion
ISR(ADC_vect){

  //  Done reading
  readFlag = 1;
 
  //  Must read low first
  analogVal = ADCL | (ADCH << 8);
  if (analogPin) {
    valores1[idx] = analogVal;
    ADMUX &= B11111110;
    idx++;
  }
  else {
    valores0[idx] = analogVal;
    ADMUX |= B1;
  }
  analogPin = !analogPin;
}
