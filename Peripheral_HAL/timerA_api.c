#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdint.h>
#include "common.h"

#define masterCount ( ( (uint32_t)TIMER_A1->R << 16) | TIMER_A0->R)

static uint32_t queue[32]; //Queue of events to fire
static uint_fast32_t index;
#define index6b (index & 0x1F)

static void (*fireOutput[32]) (void); //Array of outputs to fire

//Config for upper 16 bits, Timer A1


//Output square wave from lower 16 to drive upper 16 bits



void init_sparkTimer(void) {
    //Config for lower 16 bits, Timer A0
    //Total division = SMCLK / 12 = 1 MHz frequency
    TIMER_A0->EX0 = bit_0 | bit_2; //Divide by 6
    TIMER_A0->CTL = bit_9 | bit_6 | bit_5; //SMCLK, divide by 2, continuous mode

    TIMER_A0->CCR[0] = UINT16_MAX; //Store value, capture/compare disabled
    TIMER_A0->CCR[4] = (UINT16_MAX / 2); //Output 50% duty cycle square wave
    TIMER_A0->CCTL[4] = bit_6 | bit_5; //Set/reset mode, interrupt disabled

    TIMER_A1->EX0 = 0; //Divide by 1
    TIMER_A1->CTL = bit_5; //Continuous mode

}

void reloadTimer(void) {
    TIMER_A0->CCR[1] = queue[index6b];
    index++;
}


//Timer A0 all other interrupts
void TA0_N_IRQHandler() {
    const uint16_t reg = TIMER_A0->IV;
      switch (reg) {
      case 0x02: //Compare 1
        //  if(masterCount >= n) {
              (*fireOutput[index6b])(); //Turn on or off the output
              reloadTimer();
       //   }
          break;
      default:
          while (1) {
             //Catch debugger
          }

    /*  Not enabled
      case 0x04: //Compare 2
          break;
      case 0x06: //Compare 3
          break;
      case 0x08: //Compare 4
          break;                  */

     /*  Not available on this hardware
      case 0x0A: //Compare 5
          break;
      case 0x0C: //Compare 6
          break;                    */

     /* case 0x0E: //Overflow, not enabled
          break;                    */
      }
}







//Timer A0 Compare 0
void TA0_0_IRQHandler() {

}


//Timer A1 Compare 0
void TA1_0_IRQHandler() {

}


//Timer A1 all other interrupts
void TA1_N_IRQHandler() {

}




