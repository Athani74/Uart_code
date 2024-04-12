/*
 * File:   uart.c
 * Author: Sachin Athani
 *
 * Created on 10 April, 2024, 4:29 PM
 */



#include <xc.h>

#define _XTAL_FREQ 8000000 // Assuming an 8MHz crystal oscillator

#define BAUD_RATE 2400

void UART_Init() {
    TRISC6 = 0; // TX Pin setting as output
    TRISC7 = 1; // RX Pin setting as input

    SPBRG = ((_XTAL_FREQ / 16) / BAUD_RATE) - 1; // Calculate baud rate value
    BRGH = 0; // Set for low speed baud rate

    SYNC = 0; //  this bit is for Asynchronous mode
    SPEN = 1; // it enables serial port pins
    CREN = 1; // this bit enable contineous reception
    TXEN = 1; // transmit enable bit
}

void UART_Write(char data) {
    while (!TXIF); // Wait for the TXIF bit to set
    TXREG = data; // Write data to the TXREG register
}

char UART_Read() {
    while (!RCIF); // Wait for the RCIF bit to set
    return RCREG; // Return received data
}

void EEPROM_Write(unsigned int addr, unsigned char data) {
    EEADR = addr; // Loading address to EEPROM address register
    EEDATA = data; // Loading data to EEPROM data register
    CFGS = 0; // Access EEPROM memory
    EEPGD = 0; // Accessing  EEPROM data memory
    WREN = 1; // Enable EEPROM write
    GIE = 0; // Disable interrupts globally
   // EECON2 = 0x55; // Write the first unlock sequence
    //EECON2 = 0xAA; // Write the second unlock sequence
    WR = 1; // Start EEPROM write
    WREN = 0; // Disable EEPROM write
    GIE = 1; // Enable interrupts globally
}

void main(void) {
    UART_Init();

    unsigned int address = 0;
    char data;

    while (1) {
        data = UART_Read();
        EEPROM_Write(address++, data);
    }
}