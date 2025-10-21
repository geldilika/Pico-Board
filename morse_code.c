#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"
#include "includes/buzzer.h" 
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define RIGHT_BUTTON_PIN 22
#define BUTTON_PIN 16  // GPIO pin for button
#define MAX_MORSE_LENGTH 5
// #define PAUSE_THRESHOLD 400  // Milliseconds for end-of-character detection
#define DASH_THRESHOLD 250   // Milliseconds for distinguishing dash from dot
#define ERROR_THRESHOLD 700 // Milliseconds to distinguish an error if held for longer than 700ms
#define BUZZER_PIN 17 ///
#define POT_ADC_CHANNEL 0 ////
//#define DEFAULT_TIME_LIMIT 4000 // Default time limit in milliseconds

//RGB BELOW
#define R 13 
#define G 12 
#define B 11 

#define BRIGHTNESS 50
#define LOOP_SLEEP 10
#define MAX_COLOUR_VALUE 255
#define MAX_PWM_LEVEL 65535

#define UP true
#define DOWN false
//

void setup_rgb()
{
    // Tell the LED pins that the PWM is in charge of its value.
    gpio_set_function(R, GPIO_FUNC_PWM);
    gpio_set_function(G, GPIO_FUNC_PWM);
    gpio_set_function(B, GPIO_FUNC_PWM);

    // Figure out which slice we just connected to the LED pin, this is done for the other colors below
    uint slice_num = pwm_gpio_to_slice_num(R);
    // Get the defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);

    slice_num = pwm_gpio_to_slice_num(G);
    pwm_init(slice_num, &config, true);

    slice_num = pwm_gpio_to_slice_num(B);
    pwm_init(slice_num, &config, true);
}
// Allow a colour value [0,255] and scale it up to uint16_t [0,65535].
// Then scale it by brightness (out of 100) and invert the value (LED low is ON).
void show_rgb(int r, int g, int b)
{
    //printf("Showing RGB: %u %u %u\n", r, g, b);
    pwm_set_gpio_level(R, ~(MAX_PWM_LEVEL * r / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(G, ~(MAX_PWM_LEVEL * g / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
    pwm_set_gpio_level(B, ~(MAX_PWM_LEVEL * b / MAX_COLOUR_VALUE * BRIGHTNESS / 100));
}


char arr[MAX_MORSE_LENGTH] = {0};  // Array to hold Morse code input
int index1 = 0;  // Current index for the Morse code array

char alphabet[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
char decodedMessage[4] = {0};
int Frequencies[] = { 120, 350, 390, 590, 760, 890, 1000, 1200, 1300, 1450,  // A-J
                    1600, 1750, 1800, 1900, 2000, 2150, 2250, 2400, 2500, 2600,    // K-T
                    2800, 3000, 3100, 3250, 3500, 3750         // U-Z
                };



// Function prototypes
int find_morse_code_index(char* arr);

int main() {
    timer_hw->dbgpause = 0;
    stdio_init_all();
    buzzer_init();///

    setup_rgb();

    seven_segment_init();
    seven_segment_off();
    seven_segment_show(26);
    sleep_ms(500);
    seven_segment_off();

    printf("WELCOME\n");

     gpio_get(BUZZER_PIN);
    // buzzer_enable(1000);///
	// sleep_ms(1000);///
	// buzzer_disable();///

    // Initialize button pin
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUTTON_PIN);

    gpio_init(RIGHT_BUTTON_PIN);
    gpio_set_dir(RIGHT_BUTTON_PIN, GPIO_IN);
    gpio_pull_down(RIGHT_BUTTON_PIN);

    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_down(BUZZER_PIN);

    int songCount = 0;

   while (true) {
    bool buttonPressed = false;
    int time = 0;
    
    // Wait for button press
    while ((buttonPressed = gpio_get(BUTTON_PIN)) == false) {
        sleep_ms(1);
    }

    // Start fresh for a new character input
    if (index1 == 0) {
        memset(arr, 0, sizeof(arr));  // Clear the Morse array
    }

    // Measure press duration
    while ((buttonPressed = gpio_get(BUTTON_PIN)) == true) {
        sleep_ms(1);
        time++;
    }

    printf("Button press duration: %d ms\n", time);

    // Handle error condition if the button is held for too long
    if (time > ERROR_THRESHOLD) {
        buzzer_init();///
        printf("Error: Button held too long (%d ms). Displaying 8.\n", time);
        seven_segment_show(26); // Assuming index 26 corresponds to `8` in `values[]`.
        //sleep_ms(1000);  // Display the error (number 8) for 1 second
        buzzer_enable(500);
        show_rgb(255, 0, 0);
        sleep_ms(1000);
        show_rgb(0,0,0);
        seven_segment_off();
        buzzer_disable();
        memset(arr, 0, sizeof(arr));  // Reset Morse input array
        index1 = 0;  // Reset index for next character
        continue;  // Skip the rest of the loop
    }

    // Determine dot or dash
    if (time >= DASH_THRESHOLD) {
        //seven_segment_show(1);  // Dash
        // gpio_get(BUZZER_PIN);
        // buzzer_enable(1000);///
	    //sleep_ms(1000);///
        // buzzer_disable();
        if (index1 < MAX_MORSE_LENGTH - 1) {
            arr[index1++] = '-';
            buzzer_init();
            buzzer_enable(1000);
            sleep_ms(400);
            buzzer_disable();
        }
        else {
            buzzer_init();///
            seven_segment_show(26); // Assuming index 26 corresponds to `8` in `values[]`.
            seven_segment_off();
            printf("Too many inputs ");
            seven_segment_show(26); 
            buzzer_enable(500); // negative feedback
            //sleep_ms(1000);
            show_rgb(255, 0, 0);
            sleep_ms(1000);
            show_rgb(0,0,0);
            buzzer_disable();
            memset(arr, 0, sizeof(arr));
            index1 = 0;
            continue;
        }
    } else if (time > 0 && time < DASH_THRESHOLD) {
        //seven_segment_show(2);  // Dot
        // gpio_get(BUZZER_PIN);
        // buzzer_enable(1000);///
	    // sleep_ms(1000);///
        // buzzer_disable();
        if (index1 < MAX_MORSE_LENGTH - 1) {
            arr[index1++] = '.';
            buzzer_init();
            buzzer_enable(1000);
            sleep_ms(150);
            buzzer_disable();
        }
        else {
            buzzer_init();///
            seven_segment_show(26); // Assuming index 26 corresponds to `8` in `values[]`.
            seven_segment_off();
            printf("Too many inputs");
            seven_segment_show(26); 
            buzzer_enable(500); // negative feedback
            //sleep_ms(1000);
            show_rgb(255, 0, 0);
            sleep_ms(1000);
            show_rgb(0,0,0);
            buzzer_disable();
            memset(arr, 0, sizeof(arr));
            index1 = 0;
            continue;
        }
    }

    seven_segment_off();

    printf("Current Morse array: %s\n", arr);

    // Measure pause after button release
    time = 0;
    while ((buttonPressed = gpio_get(BUTTON_PIN)) == false) {
        sleep_ms(1);
        time++;
        if (time > 400) {  // End of character
            arr[index1] = '\0';

            printf("Final Morse array: %s\n", arr);

            int morse_index = find_morse_code_index(arr);

            if (morse_index >= 0 && morse_index < 10) {
                printf("Decoded Letter: %c\n", alphabet[morse_index]);
                seven_segment_show(morse_index);  // Display digits 0-9
                decodedMessage[songCount] = alphabet[morse_index];
                songCount++;
                //sleep_ms(1000);
                show_rgb(0, 255, 0);
                sleep_ms(1000);
                show_rgb(0,0,0);
            } else if (morse_index >= 10 && morse_index < 36) {
                printf("Decoded Letter: %c\n", alphabet[morse_index]);
                seven_segment_show(morse_index);  // Adjust for letters A-Z
                decodedMessage[songCount] = alphabet[morse_index];
                songCount++;
                //sleep_ms(1000);
                show_rgb(0, 255, 0);
                sleep_ms(1000);
                show_rgb(0,0,0);
            } else {
                printf("Invalid Morse index: %d\n", morse_index);
                buzzer_init();///
                seven_segment_show(26);
                buzzer_enable(500); // negative feedback
                //sleep_ms(1000);
                show_rgb(255, 0, 0);
                sleep_ms(1000);
                show_rgb(0,0,0);
                buzzer_disable();
            }

            // Reset input array and index for the next character
            memset(arr, 0, sizeof(arr));
            index1 = 0;
            break;
        }
    }

    if(songCount == 4){
        printf("Decoded Message: ");
        for(int i = 0; i < 4; i++){
            printf("%c", decodedMessage[i]);
        }
        printf("\n");
 
        for (int i = 0; i < 4; i++) {
            if (decodedMessage[i] >= 'A' && decodedMessage[i] <= 'Z') {
                buzzer_enable(Frequencies[decodedMessage[i] - 'A']);
                sleep_ms(500);
                buzzer_disable();
            }
        }
        // for(int i = 500; i <= 3500; i = i + 500){
        //     buzzer_init();///
        //     buzzer_enable(i);
        //     sleep_ms(500);
        //     buzzer_disable();

        // }
        songCount = 0;

        printf("Press Left button to continue and right button to turn of all components and terminate the program. \n");
        while(true){
            if (gpio_get(BUTTON_PIN)){
                show_rgb(0,255,0);
                sleep_ms(1000);
                show_rgb(0,0,0);
                seven_segment_off();
                break;
            } else if (gpio_get(RIGHT_BUTTON_PIN)){
                show_rgb(255,0,0);
                sleep_ms(1000);
                show_rgb(0,0,0);
                seven_segment_off();
                buzzer_disable;
                return 0;
            }
        }
    }

    // if (time > PAUSE_THRESHOLD && index1 == 0){
    //     buzzer_init();///
    //     buzzer_enable(250);
    //     sleep_ms(500);
    //     buzzer_disable();
    // }
}
    return 0;
    

}

int find_morse_code_index(char* arr) {
    printf("Searching for Morse: '%s'\n", arr);

    for (int i = 0; i < 26; i++) {  // 26 letters 
        if (strcmp(arr, morse_code[i]) == 0) {
            printf("Match found: '%s' -> Index: %d\n", morse_code[i], i);
            return i;
        }
    }

    printf("No match found for Morse: '%s'\n", arr);
    return -1;  // Not found
}