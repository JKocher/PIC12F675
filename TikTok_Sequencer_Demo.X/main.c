/*
 * File:   main.c
 * Author: Jason Kocher
 *
 * Created on October 22, 2025, 8:32 AM
 */

// CONFIG
#pragma config FOSC = INTRCIO   // Internal RC oscillator, GPIO on GP4/GP5
#pragma config WDTE = OFF       // Watchdog Timer Enable (OFF)
#pragma config PWRTE = OFF      // Power-up Timer Enable (OFF)
#pragma config MCLRE = OFF      // GP3/MCLR pin function select (MCLR disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (OFF)
#pragma config CP = OFF         // Code Protection (OFF)
#pragma config CPD = OFF        // Data Code Protection (OFF)

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 4000000  // 4 MHz internal oscillator

// Simple loop delay using constant __delay_ms(1)
void variable_delay(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        __delay_ms(1);
    }
}

// Function to read analog from AN0
uint16_t readADC(void) {
    ADCON0bits.CHS = 0;     // Select AN0
    __delay_us(10);         // Acquisition delay
    ADCON0bits.GO_nDONE = 1;
    while (ADCON0bits.GO_nDONE);  // Wait for conversion
    return ((ADRESH << 8) + ADRESL); // Return 10-bit result
}

void main(void) {
    
    // I/O config
    TRISIO = 0b00001001;    // GP0 (AN0) and GP3 are inputs; GP1, GP4, GP5 are outputs
    ANSEL = 0b00000001;     // AN0 enabled, rest digital
    ADCON0 = 0b10000001;    // ADC ON, AN0 selected, Fosc/32
    CMCON = 0b00000111;   //Must set CM2:CM0 for 111 (Lowest power) and allow digital output
    VRCON = 0b00000000;

    // Init outputs low
    GPIO = 0x00;

    while(1) {
        uint16_t adc_val = readADC();
        uint16_t delay_ms = 1 + ((uint32_t)adc_val * 999) / 1023;

        // Flash GP0
        GPIObits.GP1 = 0; GPIObits.GP2 = 1; GPIObits.GP4 = 1; GPIObits.GP5 = 1;
        variable_delay(delay_ms);

        // Flash GP1
        GPIObits.GP1 = 1; GPIObits.GP2 = 0; GPIObits.GP4 = 1; GPIObits.GP5 = 1;
        variable_delay(delay_ms);

        // Flash GP4
        GPIObits.GP1 = 1; GPIObits.GP2 = 1; GPIObits.GP4 = 0; GPIObits.GP5 = 1;
        variable_delay(delay_ms);

        // Flash GP5
        GPIObits.GP1 = 1; GPIObits.GP2 = 1; GPIObits.GP4 = 1; GPIObits.GP5 = 0;
        variable_delay(delay_ms);
    }
    
    
    return;
}
