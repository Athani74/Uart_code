/*
 * File:   pc.c
 * Author: Sachin Athani
 *
 * Created on 10 April, 2024, 4:29 PM
 */


#include <xc.h>
#include <stdio.h>

#define _XTAL_FREQ 8000000
#define BAUD_RATE 2400

void UART_init() {
    TRISC6 = 0;
    TRISC7 = 1; 

    SPBRG = ((_XTAL_FREQ / 16) / BAUD_RATE) - 1; // Calculating baud rate value
    BRGH = 0; // Setting the low speed baud rate

    TX9 = 0;
    SYNC = 0;
    CREN = 1;
    SPEN = 1;
    RX9 = 0;
}

void UART_Write(char data) {
    while (!TXIF); // Wait for the TXIF bit to set
    TXREG = data; // Write data to the TXREG register
}

char UART_Read() {
    while (!RCIF); // Wait for the RCIF bit to set
    return RCREG; // Return received data
}
// this function is for printing the data transfered/ received in bits per second.
void print_speed(unsigned long bytes_transferred, unsigned long time_taken) {
    double speed = (double)(bytes_transferred * 8) / time_taken;  
    printf("Transmission Speed: %.2f bits/second\n", speed);
}

void Timer0_init() {
    T0CS = 0; 
    PSA = 0; 
    PS2 = 1; 
    PS1 = 1;
    PS0 = 1;
}

unsigned long Timer0_read() {
    return (TMR0H << 8) | TMR0L; //  combining both registers to get 16 bit timer0 value
}

void Timer0_start() {
    TMR0 = 0; // Clear Timer0
    T0IF = 0; // Clear timer0 overflow flag
    T0IE = 0; // Disable timer0 interrupt
    TMR0ON = 1; //  this bit Start timer0
}

void Timer0_stop() {
    TMR0ON = 0; // this bit will stop the timer0
}

int main() {
    UART_init(); 
    Timer0_init(); 

    FILE *fp = fopen("received_data.txt", "wb"); // Open file for write operation
    if (fp == NULL) {
        printf("Error creating/opening file\n"); //printing the error message if file not opened 
        return 1;
    }

    unsigned long bytes_transferred = 0;
    unsigned long start_time = 0, end_time = 0;

    Timer0_start(); // Start Timer0
    while (1) {
        char data = UART_read(); //this function read data from UART
        if (data == '\0') // if this condition fails it will end the transmission
            break;
        fwrite(&data, sizeof(char), 1, fp); // Writing data to file
        bytes_transferred++; // Increment the bytes transferred
    }
    Timer0_stop(); // it will stop the timer0 module
    end_time = Timer0_read(); // Get Timer0 value

    fclose(fp); // Close file
    print_speed(bytes_transferred, end_time); // Print transmission speed

    return 0;
}
