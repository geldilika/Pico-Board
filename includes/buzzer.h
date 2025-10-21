#include <stdio.h>
#include <math.h> // Required for the ceil function
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#ifndef BUZZER_PIN
#define BUZZER_PIN 17 // Example GPIO pin for the buzzer
#endif

// Function prototypes
void buzzer_init();
void buzzer_disable();
void buzzer_enable(unsigned int frequency);
unsigned int pwm_set_freq_duty(unsigned int slice, unsigned int channel, unsigned int frequency, double duty);

int secondmain() {
    stdio_init_all();  // Initialize standard I/O

    // Initialize the buzzer
    buzzer_init();
    printf("Buzzer initialized\n");

    // Enable the buzzer with a test frequency (e.g., 1000 Hz)
    buzzer_enable(1000);
    printf("Buzzer enabled with frequency 1000 Hz\n");

    // Let the buzzer buzz for 3 seconds
    sleep_ms(3000);

    // Disable the buzzer
    buzzer_disable();
    printf("Buzzer disabled\n");

    return 0;
}

void buzzer_init() {
    gpio_init(BUZZER_PIN);  // Initialize the GPIO pin
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
}

void buzzer_disable() {
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER_PIN), false);  // Disable the PWM slice
    printf("PWM disabled\n");
}

void buzzer_enable(unsigned int frequency) {
    pwm_set_freq_duty(
        pwm_gpio_to_slice_num(BUZZER_PIN),
        pwm_gpio_to_channel(BUZZER_PIN),
        frequency,
        0.1  // Duty cycle set to 10%
    );
    pwm_set_enabled(pwm_gpio_to_slice_num(BUZZER_PIN), true);  // Enable the PWM slice
    printf("PWM enabled\n");
}

unsigned int pwm_set_freq_duty(
    unsigned int slice,
    unsigned int channel,
    unsigned int frequency,
    double duty
) {
    unsigned int clock = 125000000;  // Raspberry Pi Pico clock speed
    unsigned int divider16 = ceil(clock / (frequency * 4096.0));

    if (divider16 < 16) divider16 = 16;

    unsigned int wrap = (clock * 16 / divider16 / frequency) - 1;

    pwm_set_clkdiv_int_frac(slice, divider16 / 16, divider16 & 0xF);
    pwm_set_wrap(slice, wrap);
    pwm_set_chan_level(slice, channel, wrap * duty);

    printf("Set frequency: %d, wrap: %d\n", frequency, wrap); // Debug print

    return wrap;
}
