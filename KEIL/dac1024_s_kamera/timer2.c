#include "timer2.h"
#include "main.h"
#include "int1.h"

//-------------------------------------------------------------------
//
// timer2 
//

//-------------------------------------------------------------------
word w_timer2_init;
extern word  count_string;
word w_num_sikle;

void _timer2_isr (void) interrupt 5 using 1
{
 	timer2_Isr();
	TF2 = 0;
}

//-----------------------------------------------
void timer2_Init()
{
   TL2 = LOBYTE(INIT_TIMER_2);
   TH2  = HIBYTE(INIT_TIMER_2);
   RCAP2L = LOBYTE(INIT_TIMER_2);
   RCAP2H = HIBYTE(INIT_TIMER_2);
//Таймер инкрементирует2ся под воздействием 
//внутреннего сигнала 
	T2CON = 0; 		 
	CKCON |= 0x20; // c частотой SYSCLK;
//Таймер 2 в режиме автоперезагрузки
	T2CON &=  0xfe;	 //CP/RL2 = 0;
//Прерывания Таймера 2 разрешены, приоритет младший
   	PT2 = 1;
   	ET2 = 1;
// Выключаем таймер
	TR2 = 0;
}


//----------------------------------------------------------
 void  timer2_Isr()
 {  	    	   			
 word t =0L;					
 word a = 35;
	t =  w_num_sikle -= 20;         
    
		DAC0L =  (t);
		t = (t >> 8);
		t &= 0x000f;
    	DAC0H = t;		
while(a > 0)a--;			        							    	  

 }
