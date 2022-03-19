
#include "int1.h"
#include "main.h"
#include "dac.h"
#include "pmc.h"
#include "timer2.h"
unsigned int delta_dac;
unsigned  int tData;	 	
sbit TestBit = P0 ^ 6;
extern word  count_string;
extern int dac_data;
extern bit old_run;
extern unsigned  char binning;
//-------------------------------------------------------------------
//
// int1
//
//-------------------------------------------------------------------				 
void int1 (void) interrupt 2 using 1  
{
	    TestBit = 1;
	int1_isr();
	    TestBit = 0;
	IE1 = 0;
	  
}

//--------------------------------------------------
 void int1_init()
 {
 //Прерывание чувствительно к срезу
 	IT1 = 1;
 //Приоритет прерывания-высший
 	PX1 = 1;
 //Разрешение прерывания INT1 
 	EX1 = 1;

 	PRT0CF = 0x40;
    DAC0L = (MAX_OFFS);
    DAC0H = ((MAX_OFFS >> 8));
	tData = 0;   
	TestBit = 1;
    delta_dac = DELTA_DAC_S1;
 }

 //---------------------------------------------------------
// Прерывание по TRIG/32
//----------------------------------------------------------	 	   				                                                                                                                                                                
 void  int1_isr()
 {	 
   		 			 		  		   
	if((count_string <= 4 * binning) && ((count_string >= 1))) //Per steps
	{	
    	unsigned int t;       
     	tData =  tData + delta_dac;//DELTA_DAC;
     	tData = (tData & 0x0fff);       
     	DAC0L =  (tData);
	 	t = (tData >> 8);
     	DAC0H = (t & 0x0f);		
	} 	 	 	 
}			

//--------------------------------------------------
void int1_InitDacPerLine()
{
   
word t = dac_data;
	DAC0L =  (t);
	t = (t >> 8);
	t &= 0x000f;
    DAC0H = t;	
}

			  /*
void int1_InitDacPerSteps()
{	
		if((count_string <= 4) && ((count_string >= 1))) //Per steps
	{	
    	unsigned int t;       
     	tData =  tData + DELTA_DAC;
     	tData = (tData & 0x0fff);       
     	DAC0L =  (tData);
	 	t = (tData >> 8);
     	DAC0H = (t & 0x0f);		
	}
}	
*/