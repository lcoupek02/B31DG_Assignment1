#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"

// Define input and output pins
#define SYN_RED GPIO_NUM_26
#define DATA_GREEN GPIO_NUM_25
#define OUTPUT_ENABLE GPIO_NUM_12
#define OUTPUT_SELECT GPIO_NUM_14

// Setting debug mode for slower pulses
#define DEBUG_MODE 1

#if DEBUG_MODE
#define FACTOR 1000 // Convert microseconds to milliseconds for visibility
#else 
#define FACTOR 1
#endif

// Timing and pulse parameters
const uint32_t a = 300 * FACTOR;  // Duration of the first pulse on-time (TON(1)) in a DATA waveform cycle
const uint32_t b = 1200 * FACTOR; // Duration of each pulse off-time in a DATA waveform cycle
const uint8_t c = 10;  // DONT MULTIPLY BY FACTOR - Number of pulses in a DATA waveform cycle.
const uint32_t d = 5500 * FACTOR; // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse
const uint32_t synT = 50 * FACTOR; // The SYNC pulse on-time duration - 50us at production timing
const uint32_t increase = 50 * FACTOR; // IDK

// Variables to track debounce timing
volatile unsigned long last_button_time1 = 0;
volatile unsigned long last_button_time2 = 0; 
const unsigned long debounceDelay = 250000;

// Variables to track pulse modes 
volatile bool on = false;
volatile bool reverseMode = false;
bool firstPulse = true;
uint8_t n; // Number of

// Interrupt for OUTPUT_ENABLE (toggles 'on' variable)
static void IRAM_ATTR ISR_ON(void *arg) {
    unsigned long current_time = esp_timer_get_time();  // Records time of when interrupt was activated
    if (current_time - last_button_time1 > debounceDelay) {   // checks for debounce
        on = !on;     // on variable determines if LED is on or not
        last_button_time1 = current_time;   // updates last time button was pressed
    }
}

// Interrupt for OUTPUT_SELECT (toggles 'reverse mode')
static void IRAM_ATTR ISR_SELECT(void *arg) {
    unsigned long current_time = esp_timer_get_time();  // Records time of when interrupt was activated
    if (current_time - last_button_time2 > debounceDelay) {   // checks for debounce
        reverseMode = !reverseMode;   // reverse variable determines what mode the wave is in
        last_button_time2 = current_time;   // updates last time button was pressed
    }
}

// Calculate on time of a pulse 
void PulseT(uint32_t x, uint8_t n) {
    int32_t onTime = x + ((n-1) * increase);
    ets_delay_us(onTime);
}

// Updates after each pulse 
int updateN(int n) {
    if (!reverseMode) {
        if (n < 10) {
            n++;    // increment n if pulse is in normal mode
        }
        return n;
    }
    else {
        if (n > 0) {
            n--;    // decrement n if pulse is in reverse mode
        }
        return n;
    }
}

// Implements the SYN pulse 
void synPulse() {
    gpio_set_level(SYN_RED, 1);
    ets_delay_us(synT);    // On time of sync pulse 
    gpio_set_level(SYN_RED, 0);
}

// Implements the DATA pulse
void dataPulse() {
    gpio_set_level(DATA_GREEN, on);
    PulseT(a, n);   // Calculates 'on' time of pulse and causes delay equal to that time
    gpio_set_level(DATA_GREEN, 0);
}

void setup_gpio() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pin_bit_mask = (1ULL << OUTPUT_ENABLE) | (1ULL << OUTPUT_SELECT)
    };
    gpio_config(&io_conf);

    gpio_set_direction(SYN_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(DATA_GREEN, GPIO_MODE_OUTPUT);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(OUTPUT_ENABLE, ISR_ON, NULL);
    gpio_isr_handler_add(OUTPUT_SELECT, ISR_SELECT, NULL);
}

void app_main() {
    setup_gpio();

    while (1) {
        if (reverseMode) {    // Runs if mode = reverse
            ESP_LOGI("MAIN", "Generating Reverse Pulse Sequence...");
            n = c ;    // Calculate number of pulses for reverse mode operation
            ets_delay_us(d); // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse (at the start of the pulse for reverse mode)

            for (int i = 0; i < c; i++) {
                if (!reverseMode) break;  // If mode has changed, exit loop immediately
                
                ets_delay_us(b); // Off time between pulses
                dataPulse();  // Perform DATA pulse 
                
                if (reverseMode) {    // To stop n updating if normal mode has been activated but loop hasn't been broken
                    n = updateN(n);
                }
            }
            if (reverseMode) {    // Only run if still in correct mode and end of pulse sequence has been reached
                synPulse();  // Perform SYN pulse
            }
        }
        else {  // Runs if mode = normal
            ESP_LOGI("MAIN", "Generating Normal Pulse Sequence...");
            n = 1;    // Reset n for normal mode operation
            synPulse();   // Perform SYN pulse

            for (int i = 0; i < c; i++) {
                if (reverseMode) break;  // If mode has changed, exit loop immediately

                dataPulse(); // Perform DATA pulse
                ets_delay_us(b); // Off time between pulses

                if (!reverseMode) {   // To stop n updating if reverse mode has been activated but loop hasn't been broken
                    n = updateN(n);
                }
            }
            ets_delay_us(d); // Idle time between the end of the final pulse off-time and the start of the next SYNC pulse.
        }
    }
}
