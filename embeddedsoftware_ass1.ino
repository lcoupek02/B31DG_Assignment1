//a : C - 3 * 100us = 300us
//b : O - 12 * 100us = 1200us
//c : U - 6 + 4 = 10
//d : P - 11 * 500us = 5500us
//ON time of 1st pusle = a
//ON time of subsquent pulses  = ON(n) = a + ((n-1) x 50us), where 2 ≤ n
//TSYNON = 50us (PRODUCTION TIMING)
//DEBUG TIMING = X1000

//E - (5%4) + 1 = Option 2 , alt behaviour 



#define SYN_RED 14
#define DATA_GREEN 13
#define OUTPUT_ENABLE 32
#define OUTPUT_SELECT 34 

#define a 300 //microseconds
#define b 1200
#define c 10
#define d 5500

uint8_t n = 0;
bool firstPulse = true ;

void PulseT(uint16_t x, uint8_t n, bool &firstPulse){
  unsigned long onTime;
  if(firstPulse) {
    delayMicroseconds(x);
    firstPulse = false;
    Serial.print("First pulse time");
    Serial.println(x);
    }
  else {  
    onTime = x + ((n-1) * 50); 
    Serial.println("reach");
    Serial.println(onTime);
  
    delayMicroseconds(onTime);
    }
}


void setup() {

  Serial.begin(115200);

  // put your setup code here, to run once:
  pinMode(SYN_RED, OUTPUT);
  pinMode(DATA_GREEN, OUTPUT);
  pinMode(OUTPUT_ENABLE, INPUT_PULLUP);
  pinMode(OUTPUT_SELECT, INPUT_PULLUP);

  //digitalWrite(OUTPUT_ENABLE, INPUT_PULLUP);
  //digitalWrite(OUTPUT_SELECT, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
 

  PulseT(a , n, firstPulse);
  n++;
  delay(1000);
  
}



