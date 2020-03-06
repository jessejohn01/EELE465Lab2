#include <msp430.h> 
#include <limits.h>

/**
 * main.c
 *
 *Made by Jesse Arstein
 *Made  Lab 2 Slave Code.
 */
char data_in = 0;
int position = 0;
char pattern[8];
void setLed(int led){ //Sets an Led based on input.
    switch (led){
    case 0:
        P1OUT |= BIT1; //LED 0
        break;
    case 1:
        P1OUT |= BIT6; //LED 1
        break;
    case 2:
        P1OUT |= BIT7; //LED 2
        break;
    case 3:
        P2OUT |= BIT7; //LED 3
        break;
    case 4:
        P1OUT |= BIT4; //LED 4
        break;
    case 5:
        P1OUT |= BIT5; //LED 5
        break;
    case 6:
        P2OUT |= BIT6; //LED 6
        break;
    case 7:
        P2OUT |= BIT0; //LED 7
        break;
    default: //Do nothing.

    }
}

void clearLed(int led){ //Clears an LED based on input.
    switch (led){
    case 0:
        P1OUT &= ~BIT1; //LED 0
        break;
    case 1:
        P1OUT &= ~BIT6; //LED 1
        break;
    case 2:
        P1OUT &= ~BIT7; //LED 2
        break;
    case 3:
        P2OUT &= ~BIT7; //LED 3
        break;
    case 4:
        P1OUT &= ~BIT4; //LED 4
        break;
    case 5:
        P1OUT &= ~BIT5; //LED 5
        break;
    case 6:
        P2OUT &= ~BIT6; //LED 6
        break;
    case 7:
        P2OUT &= ~BIT0; //LED 7
        break;
    default: //Do nothing.

    }
}
void setAllLed(){ //Set all LEDs to High.
    int i = 0;
    for(i=0;i<8;i++){
        setLed(i);
    }
}

void clearAllLed(){ //Set all LEDs to low.
    int i = 0;
    for(i=0;i<8;i++){
        clearLed(i);
    }
}

void readPattern(){
    int i = 0;
    for(i = 0; i<8; i++){
        if(pattern[i] == '0'){
            clearLed(i);
        }
        if(pattern[i] == '1'){
            setLed(i);
        }
    }

}

void initGPIO(void){
    PM5CTL0 &= ~LOCKLPM5; //unlock GPIO
    P1DIR |= BIT1; //Sets rest of pins as outputs.
    P2DIR |= BIT7;
    P2DIR |= BIT6;
    P1DIR |= BIT4;
    P1DIR |= BIT5;
    P2DIR |= BIT0;
    P1DIR |= BIT6;
    P1DIR |= BIT7;

    setAllLed();
}

void initI2C(){
    UCB0CTLW0 |= UCSWRST; // SW Reset High

    P1SEL1 &= ~BIT3; //Set port 1.3 to SCL. SELECT 01
    P1SEL0 |= BIT3; //Set port 1.3 to SCL.

    P1SEL1 &= ~BIT2; //Set port 1.2 to SDA. SELECT 01
    P1SEL0 |= BIT2; //Set port 1.2 to SDA.

    UCB0I2COA0 = 0x60; //Set own address to 60.
    UCB0I2COA0 |= UCOAEN;
    UCB0CTLW0 |= UCMODE_3; //Set to I2C Mode.
    UCB0CTLW0 &= ~UCMST; //Set to slave mode.
    UCB0CTLW0 &= ~UCTR; //Set to Rx Mode.

    UCB0CTLW0 &= ~UCSWRST; // SW Reset LOW
    UCB0IE |= UCRXIE0; //Enable RX interrupt.
    __enable_interrupt();
}



int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initGPIO();
    initI2C();


    while(1){
        readPattern();

    }


}

#pragma vector=USCI_B0_VECTOR
__interrupt void receiveISR(void)
{
    data_in = UCB0RXBUF;
    pattern[position] = data_in;
    position++;
    if(position == 8){
        position = 0;
    }


}
