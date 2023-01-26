/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32f0xx.h"
#include "lcd.h"
#include "support.h"
#include "midi.h"
#include "midiplay.h"


void initialize_midi(void);
int oob; //out of bounds indicator

void TIM6_DAC_IRQHandler(void)
{
    TIM6->SR &= ~1;
    int sample = 0;
    for(int x=0; x < sizeof voice / sizeof voice[0]; x++) {
        if (voice[x].in_use) {
            voice[x].offset += voice[x].step;
            if (voice[x].offset >= N<<16)
                voice[x].offset -= N<<16;
            sample += (wavetable[voice[x].offset>>16] * voice[x].volume) >> 4;
        }
    }
    sample = (sample >> 10) + 2048;
    if (sample > 4095)
        sample = 4095;
    else if (sample < 0)
        sample = 0;
    DAC->DHR12R1 = sample;

}

void init_dac(void)
{
    // TODO: you fill this in.
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= 3<<(2*4);

    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR &= ~DAC_CR_EN1;
    DAC->CR |= (1<<2);
    DAC->CR &= ~(0x7<<3);

    DAC->CR |= DAC_CR_EN1;
}

void init_tim6(void)
{
    // TODO: you fill this in.
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 0;
    TIM6->ARR = 48000000 / (RATE) - 1;
    TIM6->CR2 |= (1<<5);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);
    NVIC_SetPriority(TIM6_DAC_IRQn, 28);
    //NVIC_SetPriority(TIM6_DAC_IRQn,0);
    TIM6->DIER |= TIM_DIER_UIE;
//    TIM6->CR1 |= 1;

}

// Find the voice current playing a note, and turn it off.
void note_off(int time, int chan, int key, int velo)
{
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use && voice[n].note == key) {
            voice[n].in_use = 0; // disable it first...
            voice[n].chan = 0;   // ...then clear its values
            voice[n].note = key;
            voice[n].step = step[key];
            return;
        }
    }
}

// Find an unused voice, and use it to play a note.
void note_on(int time, int chan, int key, int velo)
{
    if (velo == 0) {
        note_off(time, chan, key, velo);
        return;
    }
    int n;
    for(n=0; n<sizeof voice / sizeof voice[0]; n++) {
        if (voice[n].in_use == 0) {
            voice[n].note = key;
            voice[n].step = step[key];
            voice[n].offset = 0;
            voice[n].volume = velo;
            voice[n].chan = chan;
            voice[n].in_use = 1;
            return;
        }
    }
}

void set_tempo(int time, int value, const MIDI_Header *hdr)
{
    // This assumes that the TIM2 prescaler divides by 48.
    // It sets the timer to produce an interrupt every N
    // microseconds, where N is the new tempo (value) divided by
    // the number of divisions per beat specified in the MIDI file header.
    TIM2->ARR = value/hdr->divisions - 1;
}

void TIM2_IRQHandler(void)
{
    // TODO: Remember to acknowledge the interrupt right here!
    TIM2->SR &= ~1;
    midi_play();
}

// Configure timer 2 so that it invokes the Update interrupt
// every n microseconds.  To do so, set the prescaler to divide
// by 48.  Then the CNT will count microseconds up to the ARR value.
// Basically ARR = n-1
// Set the ARPE bit in the CR1 so that the timer waits until the next
// update before changing the effective ARR value.
// Call NVIC_SetPriority() to set a low priority for Timer 2 interrupt.
// See the lab 6 text to understand how to do so.
void init_tim2(int n) {
    // TODO: you fill this in.
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 47;
    TIM2->ARR = (n - 1);
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 29);
//    TIM2->CR1 |= 0x1;
    TIM2->CR1 |= 0x80;
}

//===========================================================================
// Configure GPIOC
//===========================================================================
void enable_ports(void) {
    // Only enable port C for the keypad
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC->MODER &= ~0xffff;
    GPIOC->MODER |= 0x55 << (4*2);
    GPIOC->OTYPER &= ~0xff;
    GPIOC->OTYPER |= 0xf0;
    GPIOC->PUPDR &= ~0xff;
    GPIOC->PUPDR |= 0x55;



}

//<><><><><><><><><><><><>FUCK TIMERS <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
//NOTE: The TIM14_CR1_CEN is not enabled until the game starts
void enable_timer(void){
    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

    float clk = 48000000;
    float psc = 48;
    float freq = 100000; //Hz

    TIM14->ARR =  (uint32_t) (clk / (psc * freq)) - 1 ;
    TIM14->PSC = (uint32_t) psc - 1;

    TIM14->DIER |= 1;
    NVIC->ISER[0] |= 1<<TIM14_IRQn;
    NVIC_SetPriority(TIM14_IRQn, 31);


    //DO NOT SET THE CEN UNTILL THE GAME STARTS




}
int oob; //out of bounds indicator
extern const uint8_t midifile1[];
extern const uint8_t midifile2[];

void startTimer(){
	TIM6->CR1 |= TIM_CR1_CEN;
	TIM2->CR1 |= TIM_CR1_CEN;
    TIM14->CR1 |= TIM_CR1_CEN;
}
//int rubbleIndex = 0; // This is the creation index
void TIM14_IRQHandler(void){

    TIM14->SR &= ~1;

    move_ball();
    oob = check_OOB();
    move_obstacles();


}

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void init_lcd_spi(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;  //Enable the RCC clock to GPIO Port B
        GPIOB->MODER |= GPIO_MODER_MODER8_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER14_0 ; //Set PB8, PB11, and PB14 to be outputs

        GPIOB->ODR = (GPIO_ODR_8 |GPIO_ODR_11| GPIO_ODR_14) ;//Set the ODR value for PB8, PB11, and PB14 to be 1 (logic high)
        GPIOB->MODER &= ~(GPIO_MODER_MODER3 | GPIO_MODER_MODER5 );  //clear the MODER for 3 and 5
        GPIOB->MODER |= (GPIO_MODER_MODER3_1 | GPIO_MODER_MODER5_1);  //Configure PB3 to be alternate function 0
                                                                    //Configure PB5 to be alternate function 0

        //And to set up the SPI1 peripheral:
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

        SPI1->CR1 &= ~SPI_CR1_SPE;
        SPI1->CR1 &= ~(SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2) ; //BR as high as possible by clearing it
        SPI1->CR1 |= SPI_CR1_MSTR;

        SPI1->CR2 &= ~(SPI_CR2_DS_3);
        SPI1->CR2 = (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2);
        SPI1->CR1 |= SPI_CR1_SSI | SPI_CR1_SSM ;
        SPI1->CR1 |= SPI_CR1_SPE;


}

void setup_buttons(void)
{
//         //Enable Ports A and B.
//       RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN);
//
//
//       // Set PA0 and PB2 for input and enable pull-down resistors.
//       GPIOA->MODER &= ~(GPIO_MODER_MODER0_1 | GPIO_MODER_MODER0_0); //clear for input
//       GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;
//
//       GPIOB->MODER &=  ~(GPIO_MODER_MODER0_1 | GPIO_MODER_MODER0_0); //clear for input
//       GPIOB->PUPDR |= GPIO_PUPDR_PUPDR0_1;

    // Only enable port C for the keypad
     RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
     GPIOC->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
     GPIOC->MODER |= (GPIO_MODER_MODER4_0| GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
     GPIOC->OTYPER |= (GPIO_OTYPER_OT_4 | GPIO_OTYPER_OT_5 | GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7);

     GPIOC->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3 );  //input mode (LEAVE ZERO)

     GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPDR0| GPIO_MODER_MODER1| GPIO_MODER_MODER2| GPIO_MODER_MODER3 );
     GPIOC->PUPDR |= (GPIO_PUPDR_PUPDR0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 );

}

void initialize_midi(void)
{
	init_wavetable_hybrid2();
	init_dac();
	init_tim6();
	init_tim2(10417);
}

void basic_drawing(void);
void move_ball(void);

int main(void)
{
    enable_timer();
    setup_buttons();
    LCD_Setup(); // this will call init_lcd_spi()
    //basic_drawing();
    initialize_midi();
    MIDI_Player *mp = midi_init(midifile1);
    LCD_DrawPicture(0,0,&background);
       ship1.sx = 120;
       ship1.sy = 20;
       dodges = 0;
       generate_obstacles();
       startTimer();

    for(;;){
//        oob = check_OOB();
//        move_obstacles();
        asm("wfi");
        if (mp->nexttick == MAXTICKS) {
            mp = midi_init(midifile1);
        }
    }

}

void stop_song(void)
{
	TIM2->CR1 &= ~TIM_CR1_CEN;
	for (int i = 0; i < VOICES; i++) {
		voice[i].in_use = 0;
		voice[i].note = 0;
		voice[i].chan = 0;
		voice[i].volume = 0;
		voice[i].step = 0;
		voice[i].offset = 0;
	}
	volatile MIDI_Player *mp = midi_init(midifile2);
	TIM2->CR1 |= TIM_CR1_CEN;
}

