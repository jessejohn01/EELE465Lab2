#include <msp430.h> 
#include <string.h>
#include <stdio.h>
/**
 * main.c
 */
int data_out[8];
int position = 0;
int packet[8];
void initializeGPIO(void){
    PM5CTL0 &= ~LOCKLPM5; //unlock GPIO

    P6DIR &= ~BIT1;//Clear R1-R4 bits.
    P6DIR &= ~BIT2;
    P6DIR &= ~BIT3;
    P6DIR &= ~BIT4;
    P3DIR |= BIT7; //Set c1-c4 to out.
    P3DIR |= BIT0;
    P3DIR |= BIT2;
    P2DIR |= BIT4;


    P3OUT |= BIT7;//Set C1-C4 bits to high.
    P3OUT |= BIT0;
    P3OUT |= BIT2;
    P2OUT |= BIT4;

}
void initI2C(){
    UCB0CTLW0 |= UCSWRST; // SW Reset High

    UCB0CTLW0 |= UCSSEL_3; //Choose SMCLK or 1MHz
    UCB0BRW = 10; // Make clock 100Khz

    UCB0CTLW0 |= UCMODE_3; //Set to I2C Mode.
    UCB0CTLW0 |= UCMST; //Set to master mode.
    UCB0CTLW0 |= UCTR; //Set to Tx Mode.
    UCB0I2CSA = 0x0060; //Set slave address to 60.
    UCB0CTLW1 |= UCASTP_2;
    UCB0TBCNT = 0x01;//sizeof(packet);

    P1SEL1 &= ~BIT3; //Set port 1.3 to SCL. SELECT 01
    P1SEL0 |= BIT3; //Set port 1.3 to SCL.

    P1SEL1 &= ~BIT2; //Set port 1.2 to SDA. SELECT 01
    P1SEL0 |= BIT2; //Set port 1.2 to SDA.

    UCB0CTLW0 &= ~UCSWRST; // SW Reset LOW
    UCB0IE |= UCTXIE0; //Enable TX interrupt.
    __enable_interrupt();
}

void startI2C(){
    UCB0CTLW0 |= UCTXSTT;
}
void stopI2C(){
    UCB0CTLW0 |= UCTXSTP;
}
void delay(){ //Delay one second on a 1MHZ clock.
    __delay_cycles(1000000);

}

void setLed(int inLed){
    //Transmit one Led over I2C.
}

void clearLed(int inLed){
    //Transmit one LED over I2C.
}

void setAllLed(){
    int i = 0;
    for(i=0; i<8; i++){
        packet[i] = 1;
    }
    startI2C();
}

void clearAllLed(){
    int i = 0;
    for(i=0; i<8; i++){
        packet[i] = 0;
    }
    startI2C();
    delay();
    stopI2C();
}



void aPattern(){
    setLed(0); //LED 0
    clearLed(1); //LED 1
    setLed(2); //LED 2
    clearLed(3); //LED 3
    setLed(4); //LED 4
    clearLed(5); //LED 5
    setLed(6); //LED 6
    clearLed(7); //LED 7
}

void bPattern(){

    setAllLed();
    clearLed(0);
    delay();
    int i =0;
    for(i=1;i<8;i++){
        setLed(i-1); //LED 0
        clearLed(i); //LED 1
        delay();
    }
    /*
    setLed(0); //LED 0
    clearLed(1); //LED 1
    delay();
    setLed(1) //LED 1
    clearLed(2) //LED 2
    delay();
    setLed(2); //LED 2
    clearLed(3); //LED 3
    delay();
    setLed(3); //LED 3
    clearLed(4); //LED 4
    delay();
    setLed(4);
    clearLed(5);
    delay();
    setLed(5);
    clearLed(6);
    delay();
    setLed(6);
    clearLed(7);
    delay();
*/
}

void cPattern(){
    clearAllLed();

        setLed(3);
        setLed(4);
        delay();
        clearLed(3);
        clearLed(4);
        delay();
        setLed(2);
        setLed(5);
        delay();
        clearLed(2);
        clearLed(5);
        delay();
        setLed(1);
        setLed(6);
        delay();
        clearLed(1);
        clearLed(6);
        delay();
        setLed(0);
        setLed(7);
        delay();
        clearLed(0);
        clearLed(7);
        delay();
        setLed(1);
        setLed(6);
        delay();
        clearLed(1);
        clearLed(6);
        delay();
        setLed(2);
        setLed(5);
        delay();
        clearLed(2);
        clearLed(5);
        delay();


}

void dPattern(){
    clearAllLed();
    int i = 0;

        for(i = 2; i<6; i++){ //Step 1.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 3; i<7; i++){ //Step 2.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 4; i<8; i++){ //Step 3.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 5; i<8; i++){ //Step 4.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 6; i<8; i++){ //Step 5.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 7; i<8; i++){ //Step 6.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 6; i<8; i++){ //Step 7.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 5; i<8; i++){ //Step 8.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 4; i<8; i++){ //Step 9.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();
        for(i = 3; i<7; i++){ //Step 10.
            setLed(i);
        }
        delay();
        clearAllLed();
        delay();


}

int* readCol(int inCol[]){

    int bit3 = (P3IN & BIT2);
    int bit2 = (P3IN & BIT0);
    int bit1 = (P2IN & BIT4);
    int bit0 = (P3IN & BIT7);

    if(bit0 > 0){
        inCol[0] = 1;
    }else{
        inCol[0] = 0;
    }
    if(bit1 > 0){
        inCol[1] = 1;
    }else{
        inCol[1] = 0;
    }
    if(bit2 > 0){
        inCol[2] = 1;
    }else{
        inCol[2] = 0;
    }
    if(bit3 > 0){
        inCol[3] = 1;
    }else{
        inCol[3] = 0;
    }
    return inCol;
}

int* readRow(int inRow[]){
    int bit3 = (P6IN & BIT4);
    int bit2 = (P6IN & BIT3);
    int bit1 = (P6IN & BIT2);
    int bit0 = (P6IN & BIT1);

    if(bit0 > 0){
        inRow[0] = 1;
    }else{
        inRow[0] = 0;
    }
    if(bit1 > 0){
        inRow[1] = 1;
    }else{
        inRow[1] = 0;
    }
    if(bit2 > 0){
        inRow[2] = 1;
    }else{
        inRow[2] = 0;
    }
    if(bit3 > 0){
        inRow[3] = 1;
    }else{
        inRow[3] = 0;
    }
    return inRow;
}

char whichKey(int row, int col){
    if(row == -1 || col == -1){
        return 'X'; //We will return an error if nothing is found.
    }
    char keypad[4][4] ={
                        {'1','2','3','A'},
                        {'4','5','6','B'},
                        {'7','8','9','C'},
                        {'*','0','#','D'}
    };

    return keypad[row][col];

}

void changeCOut(){
    P3DIR |= BIT7;//Set C1-C4 bits to out.
    P3DIR |= BIT0;
    P3DIR |= BIT2;
    P2DIR |= BIT4;

    P3OUT &= ~BIT7;//Sets C1-C4 bits to low initially.
    P3OUT &= ~BIT0;
    P3OUT &= ~BIT2;
    P2OUT &= ~BIT4;
}

void pullCHigh(){
    P3OUT |= BIT7;//Clear C1-C4 bits.
    P3OUT |= BIT0;
    P3OUT |= BIT2;
    P2OUT |= BIT4;
}


char checkKeypad(int rowTriggered){ //checks for which key is pressed.
    int tempArray[4];
    int row = -1;
    int col = -1;
    changeCOut();
    __delay_cycles(1000000);
    //Begin pulling col high.
    P3OUT |= BIT7;
    __delay_cycles(1000000);
    memcpy(tempArray,readRow(tempArray),sizeof tempArray);//Read row to see if we 0 out.


        if(tempArray[rowTriggered] == 1){
            row = rowTriggered;
            col = 0;
            goto foundKey;
        }
        P3OUT &= ~BIT7;//Clear C1-C4 bits.
        P3OUT &= ~BIT0;
        P3OUT &= ~BIT2;
        P2OUT &= ~BIT4;
    P2OUT |= BIT4;
    __delay_cycles(1000000);
    memcpy(tempArray,readRow(tempArray),sizeof tempArray);//Read row to see if we 0 out.

        if(tempArray[rowTriggered] == 1){
            row = rowTriggered;
            col = 1;
            goto foundKey;
        }
        P3OUT &= ~BIT7;//Clear C1-C4 bits.
        P3OUT &= ~BIT0;
        P3OUT &= ~BIT2;
        P2OUT &= ~BIT4;
    P3OUT |= BIT0;
    __delay_cycles(1000000);
    memcpy(tempArray,readRow(tempArray),sizeof tempArray);//Read row to see if we 0 out.

    if(tempArray[rowTriggered] == 1){
        row = rowTriggered;
        col = 2;
        goto foundKey;
    }
    P3OUT &= ~BIT7;//Clear C1-C4 bits.
    P3OUT &= ~BIT0;
    P3OUT &= ~BIT2;
    P2OUT &= ~BIT4;
    P3OUT |= BIT2;
    __delay_cycles(1000000);
    memcpy(tempArray,readRow(tempArray),sizeof tempArray);//Read row to see if we 0 out.

        if(tempArray[rowTriggered] == 1){
            row = rowTriggered;
            col = 3;
            goto foundKey;
        }

foundKey:
    pullCHigh();
    return whichKey(row,col);

}


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initializeGPIO();
    initI2C();
    int keypadCArray[4]; //Create an array of 4 for the Col.
    int keypadRArray[4]; //Create an array of 4 for the Rows
    char output = -1;
    while(1){
        __delay_cycles(1000000);
        //keypadCArray = readCol(keypadCArray);
        memcpy(keypadCArray, readCol(keypadCArray), sizeof keypadCArray);//Read the Col ands store.
        memcpy(keypadRArray, readRow(keypadRArray), sizeof keypadRArray); //Read the Rows and store
        clearAllLed();

        int i = 0;
        for(i=0;i<4;i++){
            if(keypadRArray[i]==1){
                output = checkKeypad(i);
                printf("%c", output);
            }
        }

        if(output == 'A'){
          aPattern();
        }
        if(output == 'B'){
          bPattern();
        }
        if(output == 'C'){
          cPattern();
        }
        if(output == 'D'){
          dPattern();
        }

    }

    return 0;
}

#pragma vector=EUSCI_B0_VECTOR
__interrupt void sendISR(void)
{
    UCB0TXBUF = 0x001;
    /*if(position == (sizeof(packet) - 1 )){
        UCB0TXBUF = packet[position];
        position = 0;
    }else{
        UCB0TXBUF = packet[position];
        position++;
    }
    */
}
