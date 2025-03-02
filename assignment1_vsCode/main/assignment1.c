#include "assignment1.h"


// Setting debug mode for slower pulses
#define DEBUG_MODE 1

#if DEBUG_MODE
#define FACTOR 1000 
#else 
#define FACTOR 1
#endif

// Timing and pulse parameters
const uint32_t a = 300 * FACTOR;  // Duration of the first pulse on-time (TON(1)) in a DATA waveform cycle
const uint32_t b = 1200 * FACTOR; // Duration of each pulse off-time in a DATA waveform cycle
const uint8_t c = 10;  // DONT MULTIPLY BY FACTOR - Number of pulses in a DATA waveform cycle.
const uint32_t d = 5500 * FACTOR; // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse
const uint32_t synT = 50 * FACTOR; // The SYNC pulse on-time duration - 50us at production timing
const uint32_t increase = 50 * FACTOR; 

// Variables to track debounce timing
volatile uint64_t lastButtonTime1 = 0;
volatile uint64_t lastButtonTime2 = 0;
const uint64_t debounceDelay = 250000;  

// Variables to track pulse modes 
volatile bool on = false;
volatile bool reverseMode = false;
uint8_t n; // Number of waves

// Interrupt for OUTPUT_ENABLE (toggles 'on' variable)
void IRAM_ATTR ISR_ON() {
    uint64_t currentTime = esp_timer_get_time();  // Records time of when interrupt was activated 

    if (currentTime - lastButtonTime1 > debounceDelay) {   // Checks for debounce
        on = !on;     // 'on' variable determines if LED is on or not
        lastButtonTime1 = currentTime;   // updates last time button was pressed
    }
}

// Interrupt for OUTPUT_SELECT (toggles 'reverse mode')
void IRAM_ATTR ISR_SELECT() {
    uint64_t currentTime = esp_timer_get_time();  // Records time of when interrupt was activated 
    
    if (currentTime - lastButtonTime2 > debounceDelay) {   // checks for debounce
        reverseMode = !reverseMode;   // 'reverse' variable determines what mode the wave is in
        lastButtonTime2 = currentTime;   // Updates last time button was pressed
    }
}

// Calculate the on time of a pulse 
void PulseWidth() {
    int32_t onTime;   
    
    onTime = a + ((n - 1) * increase);    //Equation to calcuate pulse on time 
    ets_delay_us(onTime);
}

// Updates after each pulse 
int updateN() {
    if (!reverseMode) { 
        if (n < 10) {
            n++;    // increment n if pulse is in normal mode
        }
    } else {
        if (n > 0) {
            n--;    // decrement n if pulse is in reverse mode
        }
    }
    return n;
}


// Implements the SYN pulse 
void synPulse() {
    gpio_set_level(SYN_RED, 1);
    ets_delay_us(synT);    // On time of sync pulse 
    gpio_set_level(SYN_RED, 0);
}

// Implements the DATA pulse
void dataPulseOn() {
    gpio_set_level(DATA_GREEN, on);
    PulseWidth();   // Calculates 'on' time of pulse and causes delay equal to that time
    gpio_set_level(DATA_GREEN, 0);
}

// Put idle and data off timings into functions for easy reuses/changes 
void idleTime() {
    ets_delay_us(d);
}

void dataPulseOff() {
    ets_delay_us(b);
}

// Main program setup 
void setup_gpio() {
    
    // Initializes inputs and outputs
    gpio_set_direction(SYN_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(DATA_GREEN, GPIO_MODE_OUTPUT);
    gpio_set_direction(OUTPUT_ENABLE, GPIO_MODE_INPUT);
    gpio_set_direction(OUTPUT_SELECT, GPIO_MODE_INPUT);

    gpio_set_pull_mode(OUTPUT_ENABLE, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(OUTPUT_SELECT, GPIO_PULLUP_ONLY);

    // Initializes required interrupts
    gpio_install_isr_service(0);
    gpio_isr_handler_add(OUTPUT_SELECT, ISR_SELECT, NULL);
    gpio_isr_handler_add(OUTPUT_ENABLE, ISR_ON, NULL);
}

// Main program loop
void main_loop() {
    synPulse();   //Perform SYNC pulse

    if (reverseMode) {    // Runs if mode = reverse
        n = c;    // Calculate number of pulses for reverse mode operation
        idleTime(); // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse (at the start of the pulse for reverse mode)

        for (int i = 0; i < c; i++) {
            if (!reverseMode) break;  // If mode has changed, exit loop immediately
            dataPulseOff(); // Off time between pulses
            dataPulseOn();  // Perform DATA pulse 
            n = updateN();  // Update value of n after pulse
        }
    } else {  // Runs if mode = normal
        n = 1;    // Reset n for normal mode operation

        for (int i = 0; i < c; i++) {
            if (reverseMode) break;  // If mode has changed, exit loop immediately
            dataPulseOn(); // Perform DATA pulse
            dataPulseOff(); // Off time between pulses
            n = updateN();  // Update value of n after pulse
        }  
        idleTime(); // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse.
    }
}

// Main program application
void app_main() {
    setup_gpio();

    while (1) {
        main_loop(); //Call main loop
    }
}
