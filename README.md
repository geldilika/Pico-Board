# Pico Morse + 7-Seg + RGB + Buzzer

Raspberry Pi Pico app that:
- Reads Morse from a push-button (dot/dash by press length)
- Shows the decoded letter on a 7-segment display
- Gives feedback with RGB LED and buzzer
- Plays a short “melody” of four decoded letters (per-letter tones)

## Requirements
- Raspberry Pi Pico + Pico SDK
- One push-button (left) on GPIO16, one push-button (right) on GPIO22
- Passive buzzer on GPIO17 (PWM)
- Common-anode 7-segment (pins below)
- RGB LED on GPIO13 (R), GPIO12 (G), GPIO11 (B) (PWM)
Pins (GPIO)

Buttons

    LEFT / input button: GPIO16 (BUTTON_PIN)
    RIGHT / exit button: GPIO22 (RIGHT_BUTTON_PIN)

Buzzer (PWM)

    GPIO17 (BUZZER_PIN)

RGB LED (PWM)

    R: GPIO13  •  G: GPIO12  •  B: GPIO11

7-Segment (common-anode)

    A: GPIO7  •  B: GPIO6  •  C: GPIO20
    D: GPIO19 •  E: GPIO18 •  F: GPIO8
    G: GPIO9


How it works

    Dot / dash by press duration on LEFT button:
    Dot: < 250 ms
    Dash: ≥ 250 ms
    Error: > 700 ms → shows “8” on 7-seg, red LED, buzz
    End of letter: 400 ms pause after release
    Accepts up to 4 letters per “word”; then:
    Prints decoded message over USB serial
    Plays tones per letter (A–Z mapped to frequencies)
    Prompts: press LEFT to continue, RIGHT to stop all and exit
    Feedback
    Green flash + short buzz on valid input
    Red flash + buzz on errors (overflow or invalid Morse)
    Displays
    7-seg shows A–Z (custom segment map) and “8” for errors

File map (main ones)

    main.c (your file): button logic, Morse decode, RGB/buzzer/7-seg control
    includes/seven_segment.h: pin map + segment patterns + seven_segment_*
    includes/buzzer.h and buzzer.c: PWM buzzer helpers
    includes/potentiometer.h and potentiometer.c: ADC helpers (GPIO26)

Tweaks

    Timing: DASH_THRESHOLD, ERROR_THRESHOLD, inter-letter gap (400 ms)
    Brightness: BRIGHTNESS (0–100)
    RGB, segment, buzzer pins: change defines at the top of the files
    Extend 7-seg glyphs in values[] if you want more symbols

Run

    Open a serial monitor (USB CDC) to see logs
    Enter Morse with LEFT button; after 4 letters follow on-screen prompt
    RIGHT button exits (turns off LED/7-seg, disables buzzer)
