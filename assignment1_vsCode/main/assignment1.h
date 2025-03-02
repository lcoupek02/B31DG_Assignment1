#ifndef ASSIGNMENT1_H
#define ASSIGNMENT1_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"

// Define input and output pins 
#define SYN_RED 26
#define DATA_GREEN 25
#define OUTPUT_ENABLE 12
#define OUTPUT_SELECT 14



// Timing and pulse parameters
extern const uint32_t a;
extern const uint32_t b;
extern const uint8_t c;
extern const uint32_t d;
extern const uint32_t synT;
extern const uint32_t increase;

// Debounce timing
extern volatile uint64_t lastButtonTime1;
extern volatile uint64_t lastButtonTime2;
extern const uint64_t debounceDelay;

// Pulse mode tracking
extern volatile bool on;
extern volatile bool reverseMode;
extern uint8_t n; 

// Function Prototypes
void PulseWidth();
int updateN();

void synPulse();
void dataPulseOn();
void idleTime();
void dataPulseOff();

void setup_gpio();
void main_loop();
void app_main();

#endif 
