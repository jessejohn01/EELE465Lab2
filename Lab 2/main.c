#include <msp430.h> 
#include <string.h>
#include <stdio.h>
/**
 * main.c
 * Made by Jesse Arstein
 * Master Code
 */
int data_out[8];
int position = 0;
int positionSent = 0;
int packet[8];
int addressLED[7] = {1,1,0,0,0,0,0}; // Our slave's 7 bit address.
int queue[100][8];
void insert(int inPacket[]){
    if(position == 100){
        return; //if our queue is full don't insert anything.
    }
    int tempArray[8];
    memcpy(tempArray, inPacket, sizeof tempArray );
    int i = 0;
    for(i = 0; i<8; i++){
        queue[position][i] = tempArray[i];
    }
    position++;

}

void clearQueue(){
    int i = 0;
    int j = 0;

    for(i = 0 ; i<100; i++){
        for(j=0;j<8;j++){
            queue[i][j] = 0;
        }
    }
    position = 0;
    positionSent = 0;

}

void initializeGPIO(void)
{
    PM5CTL0 &= ~LOCKLPM5; //unlock GPIO

    P6DIR &= ~BIT1; //Clear R1-R4 bits.
    P6DIR &= ~BIT2;
    P6DIR &= ~BIT3;
    P6DIR &= ~BIT4;
    P3DIR |= BIT7; //Set c1-c4 to out.
    P3DIR |= BIT0;
    P3DIR |= BIT2;
    P2DIR |= BIT4;

    P3OUT |= BIT7; //Set C1-C4 bits to high.
    P3OUT |= BIT0;
    P3OUT |= BIT2;
    P2OUT |= BIT4;

    TB0CTL |= TBCLR;
    TB0CTL |= TBSSEL__ACLK;
    TB0CTL |= MC__CONTINUOUS;
    TB0CTL |= MC__UP;
    TB0CCR0 = 10000;

    TB0CTL |= TBIE;
    TB0CTL &= ~TBIFG;
    __enable_interrupt();

}
void sdaHigh(){ //We will use this a lot so made functions.
    P1OUT |= BIT2;
}
void sclHigh(){
    P1OUT |= BIT3;
}

void sdaLow(){ //We will use this a lot so made functions.
    P1OUT &= ~BIT2;
}
void sclLow(){
    P1OUT &= ~BIT3;
}
void initI2C()
{
    P1DIR |= BIT3;
    P1DIR |= BIT2;
    sclHigh();
    sdaHigh();
}

void shortestDelay(){
    __delay_cycles(5);
}

void delay()
{ //Delay one second on a 1MHZ clock.
    __delay_cycles(100);

}
void shortDelay(){
    __delay_cycles(20);
}

void longDelay(){
    __delay_cycles(100);
}


void stopI2C() //Stops our i2c communications. Assumes High SCL and * SDA.
{
    shortDelay();
    sclLow();
    sdaLow();
    shortDelay();
    sclHigh();
    __delay_cycles(10);
    sdaHigh();
    __delay_cycles(500);

}

void startI2C(int inAddress[],int inByte[], int rw) //Assumes a state of scl high, sda high, a 7 bit address and one byte of data.
{
    //rw is the read write bit.
    sdaLow(); //Pull sda low while scl is high.
    shortestDelay();
    sclLow(); //Make changes during SCL Low.
    //Send address
    int i = 0;
    for(i = 0; i<7 ; i++ ){ //
        if(inAddress[i] == 1){
            sdaHigh();

        }else{
            sdaLow();

        }
        shortDelay();
        sclHigh(); //out data bit
        shortDelay();
        sclLow();

    }


    if(rw == 1){
        sdaHigh();
    }else{
        sdaLow();
    }

    shortDelay();
    sclHigh(); //Out r/w
    shortDelay();

    sclLow();
    P1DIR &= ~BIT2; //Need to read the ACK. So float the SDA line.
    shortDelay();
    sclHigh(); //poll ACK.

    if((P1IN & BIT2) > 0){
        P1DIR |= BIT2; //Reset dir to output.
        stopI2C();

        return; //If we get a NACK we exit reset.
    }
    P1DIR |= BIT2;
    shortDelay();
    sclLow();

    for(i = 0; i<8; i++){
        if(inByte[i] == 1){

            sdaHigh();
        }else{
            sdaLow();
        }
        shortDelay();
        sclHigh(); //send data bit.
        shortDelay();
        sclLow();
    }

    shortDelay();

    stopI2C();
}

void setLed(int inLed)
{
    packet[inLed] = 1;

}

void clearLed(int inLed)
{
    packet[inLed] = 0;
}

void setAllLed()
{
    int i = 0;
    for (i = 0; i < 8; i++)
    {
        packet[i] = 1;
    }
}

void clearAllLed()
{
    int i = 0;
    for (i = 0; i < 8; i++)
    {
        packet[i] = 0;
    }

}
void aPattern()
{

    int packet[] = {1,0,1,0,1,0,1,0};
    insert(packet);
    longDelay();
}

void bPattern()
{

    setAllLed();
    clearLed(0);
    insert(packet);
    longDelay();
    int i = 0;
    for (i = 1; i < 8; i++)
    {
        setLed(i - 1); //LED 0
        clearLed(i); //LED 1
        insert(packet);
        //longDelay();
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

void cPattern()
{
    clearAllLed();

    setLed(3);
    setLed(4);
    insert(packet);
    longDelay();
    clearLed(3);
    clearLed(4);
    longDelay();
    setLed(2);
    setLed(5);
    insert(packet);
    longDelay();
    clearLed(2);
    clearLed(5);
    longDelay();
    setLed(1);
    setLed(6);
    insert(packet);
    longDelay();
    clearLed(1);
    clearLed(6);
    longDelay();
    setLed(0);
    setLed(7);
    insert(packet);
    longDelay();
    clearLed(0);
    clearLed(7);
    longDelay();
    setLed(1);
    setLed(6);
    insert(packet);
    longDelay();
    clearLed(1);
    clearLed(6);
    longDelay();
    setLed(2);
    setLed(5);
    insert(packet);
    longDelay();
    clearLed(2);
    clearLed(5);
    longDelay();

}

void dPattern()
{
    clearAllLed();
    int i = 0;

    for (i = 2; i < 6; i++)
    { //Step 1.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();
    for (i = 3; i < 7; i++)
    { //Step 2.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();
    for (i = 4; i < 8; i++)
    { //Step 3.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();
    for (i = 5; i < 8; i++)
    { //Step 4.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();
    for (i = 6; i < 8; i++)
    { //Step 5.
        setLed(i);
    }
    longDelay();
    insert(packet);
    clearAllLed();
    longDelay();
    for (i = 7; i < 8; i++)
    { //Step 6.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();

    for (i = 6; i < 8; i++)
    { //Step 7.
        setLed(i);
    }
    longDelay();
    clearAllLed();
    longDelay();
    for (i = 5; i < 8; i++)
    { //Step 8.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();
    for (i = 4; i < 8; i++)
    { //Step 9.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();
    for (i = 3; i < 7; i++)
    { //Step 10.
        setLed(i);
    }
    insert(packet);
    longDelay();
    clearAllLed();
    longDelay();

}

int* readCol(int inCol[])
{

    int bit3 = (P3IN & BIT2);
    int bit2 = (P3IN & BIT0);
    int bit1 = (P2IN & BIT4);
    int bit0 = (P3IN & BIT7);

    if (bit0 > 0)
    {
        inCol[0] = 1;
    }
    else
    {
        inCol[0] = 0;
    }
    if (bit1 > 0)
    {
        inCol[1] = 1;
    }
    else
    {
        inCol[1] = 0;
    }
    if (bit2 > 0)
    {
        inCol[2] = 1;
    }
    else
    {
        inCol[2] = 0;
    }
    if (bit3 > 0)
    {
        inCol[3] = 1;
    }
    else
    {
        inCol[3] = 0;
    }
    return inCol;
}

int* readRow(int inRow[])
{
    int bit3 = (P6IN & BIT4);
    int bit2 = (P6IN & BIT3);
    int bit1 = (P6IN & BIT2);
    int bit0 = (P6IN & BIT1);

    if (bit0 > 0)
    {
        inRow[0] = 1;
    }
    else
    {
        inRow[0] = 0;
    }
    if (bit1 > 0)
    {
        inRow[1] = 1;
    }
    else
    {
        inRow[1] = 0;
    }
    if (bit2 > 0)
    {
        inRow[2] = 1;
    }
    else
    {
        inRow[2] = 0;
    }
    if (bit3 > 0)
    {
        inRow[3] = 1;
    }
    else
    {
        inRow[3] = 0;
    }
    return inRow;
}

char whichKey(int row, int col)
{
    if (row == -1 || col == -1)
    {
        return 'X'; //We will return an error if nothing is found.
    }
    char keypad[4][4] = { { '1', '2', '3', 'A' }, { '4', '5', '6', 'B' },
                          { '7', '8', '9', 'C' }, { '*', '0', '#', 'D' } };
    char out = keypad[row][col];
    return out;

}

void changeCOut()
{
    P3DIR |= BIT7; //Set C1-C4 bits to out.
    P3DIR |= BIT0;
    P3DIR |= BIT2;
    P2DIR |= BIT4;

    P3OUT &= ~BIT7; //Sets C1-C4 bits to low initially.
    P3OUT &= ~BIT0;
    P3OUT &= ~BIT2;
    P2OUT &= ~BIT4;
}

void pullCHigh()
{
    P3OUT |= BIT7; //Clear C1-C4 bits.
    P3OUT |= BIT0;
    P3OUT |= BIT2;
    P2OUT |= BIT4;
}

char checkKeypad(int rowTriggered)

//green 3.7
//yellow 3.0
//orange 3.2
//red 2.4
{ //checks for which key is pressed.
    int tempArray[4];
    int row = -1;
    int col = -1;
    changeCOut();
    delay();
    //Begin pulling col high.
    P3OUT |= BIT7;
    delay();
    memcpy(tempArray, readRow(tempArray), sizeof tempArray); //Read row to see if we 0 out.

    if (tempArray[rowTriggered] == 1)
    {
        row = rowTriggered;
        col = 0;
        goto foundKey;
    }
    P3OUT &= ~BIT7; //Clear C1-C4 bits.
    P3OUT &= ~BIT0;
    P3OUT &= ~BIT2;
    P2OUT &= ~BIT4;
    P3OUT |= BIT2;
    delay();
    memcpy(tempArray, readRow(tempArray), sizeof tempArray); //Read row to see if we 0 out.

    if (tempArray[rowTriggered] == 1)
    {
        row = rowTriggered;
        col = 1;
        goto foundKey;
    }
    P3OUT &= ~BIT7; //Clear C1-C4 bits.
    P3OUT &= ~BIT0;
    P3OUT &= ~BIT2;
    P2OUT &= ~BIT4;
    P3OUT |= BIT0;
    delay();
    memcpy(tempArray, readRow(tempArray), sizeof tempArray); //Read row to see if we 0 out.

    if (tempArray[rowTriggered] == 1)
    {
        row = rowTriggered;
        col = 2;
        goto foundKey;
    }
    P3OUT &= ~BIT7; //Clear C1-C4 bits.
    P3OUT &= ~BIT0;
    P3OUT &= ~BIT2;
    P2OUT &= ~BIT4;
    P2OUT |= BIT4;
    delay();
    memcpy(tempArray, readRow(tempArray), sizeof tempArray); //Read row to see if we 0 out.

    if (tempArray[rowTriggered] == 1)
    {
        row = rowTriggered;
        col = 3;
        goto foundKey;
    }

    foundKey: pullCHigh();
    char out = whichKey(row,col);
    return out;

}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    initializeGPIO();
    initI2C();
    int keypadCArray[4]; //Create an array of 4 for the Col.
    int keypadRArray[4]; //Create an array of 4 for the Rows
    char output = 'X';


    while (1)
    {
        delay();
        //keypadCArray = readCol(keypadCArray);
        memcpy(keypadCArray, readCol(keypadCArray), sizeof keypadCArray); //Read the Col ands store.
        memcpy(keypadRArray, readRow(keypadRArray), sizeof keypadRArray); //Read the Rows and store

        int i = 0;
        for (i = 0; i < 4; i++)
        {
            if (keypadRArray[i] == 1)
            {
                output = checkKeypad(i);
            }
        }

        if (output == 'A')
        {
            while (1)
            {
                aPattern();
                delay();
                //keypadCArray = readCol(keypadCArray);
                memcpy(keypadCArray, readCol(keypadCArray),
                       sizeof keypadCArray); //Read the Col ands store.
                memcpy(keypadRArray, readRow(keypadRArray),
                       sizeof keypadRArray); //Read the Rows and store

                int i = 0;
                for (i = 0; i < 4; i++)
                {
                    if (keypadRArray[i] == 1)
                    {
                        output = checkKeypad(i);
                    }
                }
                if (output != 'A' && (output == 'B' || output == 'C' || output == 'D'))
                {
                    clearQueue();
                    break;
                }
            }
        }
        if (output == 'B')
        {
            while(1){
            bPattern();
            delay();
            //keypadCArray = readCol(keypadCArray);
            memcpy(keypadCArray, readCol(keypadCArray), sizeof keypadCArray); //Read the Col ands store.
            memcpy(keypadRArray, readRow(keypadRArray), sizeof keypadRArray); //Read the Rows and store

            int i = 0;
            for (i = 0; i < 4; i++)
            {
                if (keypadRArray[i] == 1)
                {
                    output = checkKeypad(i);
                }
            }
            if (output != 'B' && (output == 'A' || output == 'C' || output == 'D') )
            {
                clearQueue();
                break;
            }
            }
        }
        if (output == 'C')
        {
            while(1){
            cPattern();
            delay();
            //keypadCArray = readCol(keypadCArray);
            memcpy(keypadCArray, readCol(keypadCArray), sizeof keypadCArray); //Read the Col ands store.
            memcpy(keypadRArray, readRow(keypadRArray), sizeof keypadRArray); //Read the Rows and store

            int i = 0;
            for (i = 0; i < 4; i++)
            {
                if (keypadRArray[i] == 1)
                {
                    output = checkKeypad(i);
                }
            }
            if (output != 'C' && (output == 'A' || output == 'B' || output == 'D'))
            {
                clearQueue();
                break;
            }
        }
        }
        if (output == 'D')
        {
            while(1){
            dPattern();
            delay();
            //keypadCArray = readCol(keypadCArray);
            memcpy(keypadCArray, readCol(keypadCArray), sizeof keypadCArray); //Read the Col ands store.
            memcpy(keypadRArray, readRow(keypadRArray), sizeof keypadRArray); //Read the Rows and store

            int i = 0;
            for (i = 0; i < 4; i++)
            {
                if (keypadRArray[i] == 1)
                {
                    output = checkKeypad(i);
                }
            }
            if (output != 'D' && (output == 'A' || output == 'B' || output == 'C'))
            {
                clearQueue();
                break;
            }
        }
        }

    }

    return 0;
}

#pragma vector=TIMER0_B1_VECTOR
__interrupt void timerEnd(void){
    startI2C(addressLED,queue[positionSent], 0);
    if(positionSent == 100){
        positionSent = 0;
    }
    int temp[] = queue[positionSent];
    positionSent++;
    TB0CTL &= ~TBIFG; //Clear timer flag.
}
