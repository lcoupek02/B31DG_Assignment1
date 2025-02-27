// Define input and output pins
#define SYN_RED 26
#define DATA_GREEN 25
#define OUTPUT_ENABLE 12
#define OUTPUT_SELECT 14

// Setting debug mode for slower pulses
#define DEBUG_MODE 1

#if DEBUG_MODE
#define FACTOR 1000 
#else 
#define FACTOR 1
#endif

// Timing and pulse paramters
const uint32_t a = 300 * FACTOR;  //Duration of the first pulse on-time (TON(1)) in a DATA waveform cycle
const uint32_t b = 1200 * FACTOR; //Duration of each pulse off-time in a DATA waveform cycle
const uint8_t c = 10;  //DONT MULTIPLY BY FACTOR - Number of pulses in a DATA waveform cycle.
const uint32_t d = 5500 * FACTOR; //Idle time between the end of the final pulse off-time and the start of the next SYNC pulse
const uint32_t synT = 50 * FACTOR; //The SYNC pulse on-time duration - 50us at production timing
const uint32_t increase = 50 * FACTOR; 

// Variables to track debounce timing
volatile unsigned long lastButtonTime1 = 0;
volatile unsigned long lastButtonTime2 = 0;
const unsigned long debounceDelay = 250000;  

// Variables to track pulse modes 
volatile bool on = false;
volatile bool reverseMode = false;
uint8_t n; //Number of waves

//Function Prototypes
void PulseWidth();
int updateN();
void IRAM_ATTR ISR_ON();



// Calculate on time of a pulse 
void PulseWidth() {
  int32_t onTime;   
    
  //onTime = x + ((increase)*n)  ;
  onTime = a + ((n-1) * increase);
  delayMicroseconds(onTime);

}

// Updates after each pulse 
int updateN() {
  if(!reverseMode){ 
    if (n < 10) {
        n++;    //increment n if pulse is in normal mode
    }
    return n;
    }
  else{
    if(n > 0){
      n--;    //decrement n if pulse is in reverse mode
    }
    return n;
  }
}

// Interrupt for OUTPUT_ENABLE (toggles 'on' variable)
void IRAM_ATTR ISR_ON() {
  unsigned long currentTime = micros();  // Records time of when interrupt was activated 

  if (currentTime - lastButtonTime1 > debounceDelay) {   // checks for debounce
    on = !on;     //on variable determines if LED is on or not
    lastButtonTime1 = currentTime;   // updates last time button was pressed
  }
}

// Interrupt for OUTPUT_SELECT (toggles 'reverse mode')
void IRAM_ATTR ISR_SELECT() {
  unsigned long currentTime = micros();  // Records time of when interrupt was activated 
    
  if (currentTime - lastButtonTime2 > debounceDelay) {   // checks for debounce
    reverseMode = !reverseMode;   //reverse variable determines what mode the wave is in
    lastButtonTime2 = currentTime;   // updates last time button was pressed
  }
}

// Implements the SYN pulse 
void synPulse(){
  digitalWrite(SYN_RED, HIGH);
  delayMicroseconds(synT);    //On time of sync pulse 
  digitalWrite(SYN_RED, LOW);
}

//  Implements the DATA pulse
void dataPulseOn(){
  digitalWrite(DATA_GREEN, on);
  PulseWidth();   //Calcuates 'on' time of pulse and causes delay equal to that time
  digitalWrite(DATA_GREEN, LOW);
}

// Put into functions for easy reuses/changes 
void idleTime(){
  delayMicroseconds(d);
}

void dataPulseOff(){
  delayMicroseconds(b);
}

// Main program setup 
void setup() {
  Serial.begin(9600);

  // Initliases inputs and outputs
  pinMode(SYN_RED, OUTPUT);
  pinMode(DATA_GREEN, OUTPUT);
  pinMode(OUTPUT_ENABLE, INPUT_PULLUP);
  pinMode(OUTPUT_SELECT, INPUT_PULLUP);

  //  Initialises required interrupts
  attachInterrupt(OUTPUT_SELECT, ISR_SELECT, RISING);
  attachInterrupt(OUTPUT_ENABLE, ISR_ON, RISING);

  interrupts();

}

// Main program loop
void loop() {
   

  if (reverseMode) {    //Runs if mode = reverse
   
    n = c;    //calculate number of pulses for reverse mode operation
    synPulse(); 
    
    idleTime(); // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse (at the start of the pulse for reverse mode)

    for (int i = 0; i < c; i++) {
      if (!reverseMode) break;  // If mode has changed, exit loop immediately
   
      dataPulseOff(); //Off time between pulses
      dataPulseOn();  // Perform DATA pulse 
 
      n = updateN();  //update value of n after pulse
      
    }
  }
  else {  //Runs if mode = normal
        
    n = 1;    //Reset n for normal mode operation
    synPulse();   //Perform SYN pulse

    for (int i = 0; i < c; i++) {
      if (reverseMode) break;  // If mode has changed, exit loop immediately

      dataPulseOn(); //Perform DATA pulse
      dataPulseOff(); //Off time between pulses

      n = updateN();  //update value of n after pulse
      
    }  
    idleTime(); // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse.
  }
 
}
