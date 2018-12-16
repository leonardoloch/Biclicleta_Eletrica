#include <16F877A.h>
#device ADC=8

#fuses wdt
#FUSES NOBROWNOUT               //No brownout reset
#FUSES NOLVP                    //No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O


int flag_timer, velocidade, flag_ext, flag_deslig;
char c;
int velocidade_atual, valor, velocidade_pwm;
float adc;
void define_velocidade();
void automatico();
unsigned long int  cont;

#use delay(crystal=4MHz)

