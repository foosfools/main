uthors:  FoosFools Team, University of Utah
*                -  Abdullah Aldobaie
*                -  Tramy Nguyen
*                -  Michael Quigley
*                -  Nathan Patterson
*  Date:  Spring 2014
*
*  About:  Slave SPI and motor controller
*
*/

#include "msp430g2553.h"

#define SIZE 7

//***** MOTOR 1 SECTION ***** 
#define M1_SLEEP_PIN 8
#define M1_STEP_PIN 9
#define M1_DIR_PIN 10

//***** MOTOR 2 SECTION ***** 
#define M2_SLEEP_PIN 11
#define M2_STEP_PIN 12
#define M2_DIR_PIN 13


int motor1State = LOW;            // motorState used to set the motor
int motor2State = LOW;            // motorState used to set the motor


boolean runMotor1 = false; // Rotate Goalie
boolean runMotor2 = false;  // Move Goalie


boolean toggleMotor1 = true;
boolean toggleMotor2 = true;

//int motorSelect = 0;
int motorDirection = 0;
int motor1Steps = 0;
int motor2Steps = 0;

unsigned int motor1Pos = 90; // Goalie stands straight initialy
unsigned int motor2Pos = 0;//1080;


unsigned char RX_BUFF, LED1, LED2, STOP;
//"hello world"
char message[SIZE];
volatile unsigned int count;
char rx;

volatile uint32_t blinkDelay;
volatile uint32_t blinkDelayOff;
volatile uint32_t blinkDelayCounter;

volatile int interruptCounter;

void setup()
{
    //MOTOR 1 SETUP
    pinMode(M1_SLEEP_PIN, OUTPUT);
    digitalWrite(M1_SLEEP_PIN, 0);
    
    pinMode(M1_STEP_PIN, OUTPUT); 
    pinMode(M1_DIR_PIN, OUTPUT);
    digitalWrite(M1_STEP_PIN, 0);  
    digitalWrite(M1_DIR_PIN, 0);
  
    //MOTOR 2 SETUP  
    pinMode(M2_SLEEP_PIN, OUTPUT);
    digitalWrite(M2_SLEEP_PIN, 0);
    
    pinMode(M2_STEP_PIN, OUTPUT);  
    pinMode(M2_DIR_PIN, OUTPUT);
    digitalWrite(M2_STEP_PIN, 0);  
    
    digitalWrite(M2_DIR_PIN, 0);
    
    
   for(int i = 0; i < SIZE; i++)
   {
     message[i] = '\0';
   }
   
  rx = 'r';
  count = 0;
  WDTCTL = WDTPW + WDTHOLD; // Halt Watch Dog
  P1REN |= BIT3;
  P1IE |= BIT3; // enable button interrupt
  P1IFG &= ~BIT3; // clear interrupt flag
  P1DIR |= BIT0 + BIT6; // Set pins for LED1 and LED2 to outputs
  P1OUT &= ~BIT0; // Set LEDs off
  P1OUT &= ~BIT6;
  P1SEL = BIT1 + BIT2 + BIT4; // Secondary Peripheral Module function selected
  P1SEL2 = BIT1 + BIT2 + BIT4;
  UCA0CTL1 = UCSWRST; // Reset USCI
  UCA0CTL0 |= UCCKPL + UCMSB + UCSYNC; // Setting up SPI Slave
  UCA0CTL1 &= ~UCSWRST; // Start USCI
  IE2 |= UCA0RXIE; // Enable RX interrupt
  

    //TA0CTL = TASSEL_1 + MC_1;
  _BIS_SR(GIE);

  blinkDelay = 0x0008ffe;
  blinkDelayOff = blinkDelay + 0x0008ffe;
  blinkDelayCounter = 0;
  interruptCounter = SIZE;
  
//  rotateMotor1(180);
//  rotateMotor1(0);
//  rotateMotor1(90);
//  rotateMotor1(100);
  rotateMotor2(100);
  rotateMotor2(0);
 // rotateMotor2(0);
 // rotateMotor2(100);
   //
  // TA0CTL = TASSEL_1 + MC_1;
  //SMCLK, UP mode, Divide by 4, interrupt enable
    TACTL = TASSEL_2 + MC_1 + ID_2; // SMCLK, cont. mode
    TACCTL0 = ~(CAP | CCIE);
}


















void loop()
{  
   if( blinkDelayCounter++ == blinkDelay)
   {
     // P1OUT ^= BIT0;
     // P1OUT ^= BIT6;
   }
   else if(blinkDelayCounter++ == blinkDelayOff)
   {
   //   P1OUT ^= BIT0;
      blinkDelayCounter = 0;
   }

  
  
  // if rotate goalie
  if(runMotor1)
  {
     runMotor1 = false;
     //P1OUT ^= BIT6;

     motor1Steps = (((unsigned int)message[3] & 0x0ff) << 8) + (((unsigned int)message[4]&0x0ff));
     
     rotateMotor1(motor1Steps);
    // P1OUT ^= BIT6;
  }
  
  
  
  // if move goalie
  if(runMotor2)
  {
     runMotor2 = false;
    // P1OUT ^= BIT6;

     motor2Steps = (((unsigned int)(message[3] & 0x0ff)) << 8) + (((unsigned int)(message[4]&0x0ff)));
     
     rotateMotor2(motor2Steps);
    // P1OUT ^= BIT6;
  }
}













/*
*  SPI interrupt
*/
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR (void)
{
    toggleMotor1 = false;
  toggleMotor2 = false;
    TACCTL0 &= ~CCIE;
  rx = UCA0RXBUF;
  
  P1OUT ^= BIT6;
  // if count is 0, then wait for '$' to begin the counter
  // this's to make sure we have the message from the beginning
  if(count == 0 && rx != '$')
  {
    return;
  }
  
  message[count] = rx;//UCA0RXBUF;
  
  
  if(count == 6 )
  {
    count = 0;
  }
  else if(count == 5 && rx == 'R')
  {
    if(message[1] == '1')
    {
      runMotor1 = true;
    }
    else
    {
      runMotor2 = true;
    }
      
    count = 0;
  }
  else
  {
    count++;
  }
}











/*
*
*
*/
int equals(volatile char * a, char * b, int size)
{
  int result = 0;
  int i = 0;
  if(a[0] == '\0')
    return 1;
  
  for(i = 0; i < size; i++)
  {
   if(a[i] != b[i])
      return 1;
  }
  return 0;
}














void rotateMotor1WithTimers(int velocity)
{ 
    IE2 &= ~UCA0RXIE; // Enable RX interrupt
  digitalWrite(M1_SLEEP_PIN, 1); // motor sleep: 0 - sleep & 1 - awake
  //find direction
  int direc = velocity < 0 ? 0 : 1;
  digitalWrite(M1_DIR_PIN, direc); // motor direction: 0 - ccw & 1 - cw

  //set timer proportional to velocity
  P1OUT |= BIT0;
  TAR = 0;
  TACCR0 = abs(velocity) << 12;
  TACCTL0 |= CCIE;
    toggleMotor1 = true;
  while(toggleMotor1)
  {
      P1OUT |= BIT0;
      TACCTL0 |= CCIE;
    //toggle motor state
    motor1State = motor1State ? LOW : HIGH;
    digitalWrite(M1_STEP_PIN, motor1State);
    delayMicroseconds(5000);
  }
  P1OUT &= ~BIT0;
  digitalWrite(M1_SLEEP_PIN, 0); // motor sleep: 0 - sleep & 1 - awake
  TACCTL0 &= ~CCIE;
      IE2 |= UCA0RXIE; // Enable RX interrupt
}





void rotateMotor2WithTimers(int velocity)
{ 
      IE2 &= ~UCA0RXIE; // Enable RX interrupt
  digitalWrite(M2_SLEEP_PIN, 1); // motor sleep: 0 - sleep & 1 - awake
  //find direction
  int direc = velocity < 0 ? 0 : 1;
  digitalWrite(M2_DIR_PIN, direc); // motor direction: 0 - ccw & 1 - cw

  
    //set timer proportional to velocity

    TAR = 0;
    TACCTL0 |= CCIE;
  TACCR0 = abs(velocity) << 12;
  toggleMotor2 = true;
  while(toggleMotor2)
  {
      P1OUT |= BIT0;
      TACCTL0 |= CCIE;
    //toggle motor state
    motor2State = motor2State ? LOW : HIGH;
    digitalWrite(M2_STEP_PIN, motor2State);
    delayMicroseconds(5000);
  }
  P1OUT &= ~BIT0;
  digitalWrite(M2_SLEEP_PIN, 0); // motor sleep: 0 - sleep & 1 - awake
  TACCTL0 &= ~CCIE;
      IE2 |= UCA0RXIE; // Enable RX interrupt
}












/**
*  Rotate Goalie
*
* param-1: int - number of steps (200 steps/rotation
* param-2: int - 0/1 - ccw/cw
*/
void rotateMotor1(int newPos)
{
  digitalWrite(M1_SLEEP_PIN, 1); // motor sleep: 0 - sleep & 1 - awake

  if(newPos < 0)
    newPos = 0;
  if(newPos > 180)
    newPos = 500;
    
  while(motor1Pos != newPos)
  {
      if(runMotor1)
      {
         digitalWrite(M1_SLEEP_PIN, 0); // put motor in sleep mode
         return; 
      }
      
      
      if (motor1State == LOW)
      {
          if(newPos < motor1Pos) // reverse
          {
            digitalWrite(M1_DIR_PIN, 1); // motor direction: 0 - ccw & 1 - cw
            motor1Pos--;
          }
          else // move forward
          {
            digitalWrite(M1_DIR_PIN, 0); // motor direction: 0 - ccw & 1 - cw
            motor1Pos++;
          }
      
          motor1State = HIGH;
          digitalWrite(M1_STEP_PIN, motor1State);
          delayMicroseconds(5000); // The minimum time to keep the pin up or down in each step is different.
      }
      else
      {
          motor1State = LOW;
          digitalWrite(M1_STEP_PIN, motor1State);
          delayMicroseconds(5000);
      }
  }

  digitalWrite(M1_SLEEP_PIN, 0); // put motor in sleep mode
}




/**
*  Move Goalie
*
* param-1: int - number of steps (200 steps/rotation
* param-2: int - 0/1 - ccw/cw
*/
void rotateMotor2(unsigned int newPos)
{
  digitalWrite(M2_SLEEP_PIN, 1); // motor sleep: 0 - sleep & 1 - awake

  if(newPos < 0)
    newPos = 0;
  if(newPos > 500)
    newPos = 500;
  
  while(motor2Pos != newPos)
  {
      if(runMotor2)
      {
         digitalWrite(M2_SLEEP_PIN, 0); // put motor in sleep mode
         return; 
      }
      
      
      if (motor2State == LOW)
      {
          if(newPos < motor2Pos) // reverse
          {
            digitalWrite(M2_DIR_PIN, 1); // motor direction: 0 - ccw & 1 - cw
            motor2Pos--;
          }
          else // move forward
          {
            digitalWrite(M2_DIR_PIN, 0); // motor direction: 0 - ccw & 1 - cw
            motor2Pos++;
          }
          
          motor2State = HIGH;
          digitalWrite(M2_STEP_PIN, motor2State);
          delayMicroseconds(5000); // The minimum time to keep the pin up or down in each step is different.
      }
      else
      {
          motor2State = LOW;
          digitalWrite(M2_STEP_PIN, motor2State);
          delayMicroseconds(5000);
      }
  }

  digitalWrite(M2_SLEEP_PIN, 0); // put motor in sleep mode
}





#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  TACTL &= ~TAIFG;
  toggleMotor1 = false;
  toggleMotor2 = false;
}



