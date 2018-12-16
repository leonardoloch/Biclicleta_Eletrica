/*
Piloto Automatico para Bicicletas
Autor : Leonardo Loch da Silva
Data : 30/11/2016
Universidade Federal de Santa Catarina 
*/

#include <Software.h>
#include <lcd.c>
#include <stdlib.h>
#include <stdio.h>
#define pause PIN_B4
#define enter PIN_B2
#define reduzir PIN_B0
#define aumentar PIN_B5
#define diminuir PIN_B1
#define led_manual PIN_C4
#define led_automatico PIN_C3
#define roda PIN_C2
#define led_freio PIN_C1
#define led_acelerando PIN_C0






//tratamento do timer
#int_timer0
void trata_timer() {     
   SET_TIMER0(6);
   cont++;  
   if(cont>=10){       
   if(flag_timer==1) {
      velocidade_atual++;
      }  
   cont=0;      
   }  
}

//tratamento da interrupção externa
#int_ext  
void trata_ext() {   
   flag_ext=1;  
   while(Input(reduzir)==0){
      restart_wdt();
      delay_ms(200);
      if(velocidade_atual>0 ){
         velocidade_atual--;  
      }
      if(velocidade_atual==0 && input(pause)==0  ){
      flag_deslig=1;
      break;
      }
      lcd_putc(c);
      printf(LCD_PUTC,"Vel.=%d km/h",velocidade_atual);
      velocidade_pwm=(velocidade_atual*255)/20;
      set_pwm1_duty (velocidade_pwm); 
      lcd_gotoxy(1,2);
      adc = read_adc();
      valor=(adc*100)/255;
      printf(LCD_PUTC,"Bateria:%d %%",valor);
      output_high(led_freio);
     
   }  
   output_low(led_freio);
   flag_ext=0;
}


void main()
{
   lcd_init();//inicializacao do lcd
   
   setup_ADC_ports (AN0);            //configura porta AN0 com entrada analogica
   setup_adc(ADC_CLOCK_INTERNAL);    //usar o clock interno do AD(mais comum)

   port_b_pullups(true); //ativando o resistores de pullps nas portas b
   
   //Ativando interrupção externa
   enable_interrupts(global | int_ext);
   ext_int_edge( H_TO_L );
   
   //configurando timer
   setup_timer_0 (RTCC_INTERNAL | RTCC_DIV_8 );
   SET_TIMER0(6);
   ENABLE_INTERRUPTS (GLOBAL | INT_TIMER0);
      
    // configura CCP1 para modo PWM
   setup_ccp1 (ccp_pwm);  
   set_pwm1_duty ( 0 );  // configura o ciclo ativo em 0 (desligado)
   setup_timer_2(T2_DIV_BY_1,255,1);  
     
   flag_timer=cont=flag_ext=flag_deslig=0;
   int op=0;
   velocidade=0;
   velocidade_atual=0;
   output_low(roda);
   setup_wdt(WDT_2304MS);
   while(TRUE)
   
   {  
      flag_deslig=0;
      restart_wdt();
      c='\f';
      lcd_putc(c);
      output_low(led_automatico);
      output_low(led_manual);
      set_pwm1_duty (0);
      velocidade=0;
      velocidade_atual=0;
      do{
		restart_wdt();
		lcd_putc(c);
		printf(LCD_PUTC,"Escolha funcao");
		lcd_gotoxy(1,2);
		printf(LCD_PUTC,"ENTER-AUTOMATICO");
		delay_ms(500);
		if (!Input(enter)){
			delay_ms(400);
			op=1;
			break;
		}
		if(Input(pause)==0){
			delay_ms(400);
			op=2;
			break;
		}
		lcd_putc(c);
		printf(LCD_PUTC,"Escolha funcao");
		lcd_gotoxy(1,2);
		printf(LCD_PUTC,"PAUSE-MANUAL");
		delay_ms(500);
		if (Input(enter)==0){
		    delay_ms(400);
			op=1;
		break;
		}
		if(!Input(pause)){
			delay_ms(400);
			op=2;
			break;
		}
      
      
      
      } while(Input(enter) && Input(pause));
      switch (op) {
      case 1:
		restart_wdt();
		output_high(led_automatico);
		lcd_putc(c);
		printf(LCD_PUTC,"Sel. a vel.");
		lcd_gotoxy(1,2);
		printf(LCD_PUTC,"Vel.=%d km/h",velocidade);
		define_velocidade();
		automatico();
      break;
      case 2 :
		lcd_putc(c);
      
		printf(LCD_PUTC,"Modo Hibernar"); 
		output_high(led_manual);
      do{
		restart_wdt();
      
      } while(Input(enter)==1);
		lcd_putc(c); 
      break;
      
      
      
      }
   }
}
       
       
  




void define_velocidade(){//seleciona a velocidade 

while(Input(pause)==1){ 
      restart_wdt();
      lcd_putc(c);
      printf(LCD_PUTC,"Selecione uma vel .");
      lcd_gotoxy(1,2);
      printf(LCD_PUTC,"Vel.=%d km/h",velocidade);
      delay_ms(200);
      if(Input(aumentar)==0){      
         delay_ms(200);
         lcd_putc(c); 
      if(velocidade<20){
         velocidade++;
      }
      }
      
      if(Input(diminuir)==0){
      delay_ms(200);
      lcd_putc(c); 
      if(velocidade>0){
      velocidade--;
      }
      }
      

      

}
}


void automatico(){//estabiliza
      lcd_putc(c); 
      restart_wdt();
      while(velocidade_atual<=velocidade){//vai voltar a velocidade desejada
            restart_wdt();
            flag_timer=1;
            lcd_putc(c);
            printf(LCD_PUTC,"Vel.=%d km/h",velocidade_atual);
            velocidade_pwm=(velocidade_atual*255)/20;
            set_pwm1_duty (velocidade_pwm); 
            lcd_gotoxy(1,2);
            adc = read_adc();
            valor=(adc*100)/255;
            printf(LCD_PUTC,"Bateria:%d %%",valor);
            delay_ms(200);
            output_high(led_acelerando);
       }
       output_low(led_acelerando);
       flag_timer=0;
       flag_ext=0;    
       while(flag_deslig==0){
         restart_wdt();
         flag_ext=2;
         delay_ms(400);
         lcd_putc(c);
         printf(LCD_PUTC,"Vel.=%d km/h",velocidade_atual);      
         lcd_gotoxy(1,2);
         adc = read_adc();
         valor=(adc*100)/255;
         printf(LCD_PUTC,"Bateria:%d %%",valor);
         
         while(flag_ext==1){ //fica preso pela interrupção externa     

         
         }
         while(velocidade_atual<=velocidade && flag_deslig==0){//vai voltar a velocidade desejada
            restart_wdt();
            flag_timer=1;
            output_high(led_acelerando);
            delay_ms(200); 
            lcd_putc(c);
            printf(LCD_PUTC,"Vel.=%d km/h",velocidade_atual);
            velocidade_pwm=(velocidade_atual*255)/20;
            set_pwm1_duty (velocidade_pwm); 
            lcd_gotoxy(1,2);
            adc = read_adc();
            valor=(adc*100)/255;
            printf(LCD_PUTC,"Bateria:%d %%",valor);
            }
        output_low(led_acelerando);        
        flag_timer=0;          
      }
   }


