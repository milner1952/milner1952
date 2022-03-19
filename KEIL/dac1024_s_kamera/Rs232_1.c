

#include <stdlib.h>
#include "Rs232_1.h"
#include "main.h"
#include "string.h"
#include "int1.h"
#include "type_app.h"

#define SZ_BUFER_RX 30

void rs_232_Timer1_Init();
void rs232_1_InteruptRut(char d);

char bufer_rx[SZ_BUFER_RX];
int dac_data;
unsigned char count_rx;
unsigned  char binning;
unsigned char type_test;

#ifdef TEST_SCREEN__TYPE_APP_H
	word im_n;
	word str_n;
	int  amp_noise;
	word amp_sign;
	word msk;
	word num_pix;
#endif

extern  byte  flag_test_screen ;

extern word c_run;
extern unsigned int delta_dac; 
extern  unsigned   char	  flag_576_1152;

//-------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//   ИНТЕРФЕЙС RS232 Для  F017
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

void rs232_1_isr (void) interrupt 4 using 1  
{

	if(RI == 1)
	{	  
	  char d = SBUF;
	  rs232_1_InteruptRut(d);
	  RI = 0;
	}
	TI = 0;
}

 //------------------------------------------------------------------------------------
void rs_232_1_Init()
{

#ifdef TEST_SCREEN__TYPE_APP_H	
	im_n = 0;							
	str_n = 0;										
	amp_noise = 0;
#endif

	memset(bufer_rx,0, SZ_BUFER_RX);
	binning = 1;
        type_test = 0;
	count_rx = 0;
	rs_232_Timer1_Init();
	PCON |= 0x80; //SMOD = 1;
	SCON = 0;
	SCON |= 0x40; // Режим 1 8 бит
	SCON |= 0x20; // Стоп-бит == 1;
	REN = 1;
	PS = 1; // приоритет высокий
	TI = 0;
	RI = 0;
	ES = 1; // Разрешение прерывания
		{
		unsigned int p;
	 		dac_data = 0;
			p = DELTA_DAC2;
	 		dac_data = dac_data * p ;
	 		dac_data = dac_data + MAX_OFFS2;
			dac_data = 4096 - dac_data;
		}
}
//-----------------------------------------------------------
void rs_232_Timer1_Init()
{
	TMOD = 0;

	CKCON |= 0x10;  //TIM == 1
	TMOD |= 0x20;  // 8-разрядный таймер/счетчик с автоперезагрузкой
	TH1 = 0xf4;
	TF1 = 0;
	TR1 = 1; //Включили таймера 1

}

 //-----------------------------------------------------------
void rs232_1_InteruptRut(char d)
{
int i = 0;
long j = 0L;
 	if(d == 0x2b)
 	{
  		count_rx = 0;
		memset(bufer_rx, 0, SZ_BUFER_RX);
 	}
 	else
 	{
  		if((d != 'A')  && (d != 'B') && (d != 'C') && (d != 'D') && (d != 'E') && (d != 'R'))
  			bufer_rx[count_rx++] = d; 
		else
	 		if(d == 'A')
			{
			unsigned int p;
	 			dac_data = atoi(bufer_rx);
				p = DELTA_DAC2;
	 			dac_data = dac_data * p;
	 			dac_data = dac_data + MAX_OFFS2;
				dac_data = 4096 - dac_data;
				d = 0;
			}
            else
				if(d == 'B')
				{
			 	type_test  = atoi(bufer_rx);
				}
            	else
            		if(d == 'C')
					{
			 		binning  = atoi(bufer_rx);
					        switch (binning)
                                                {
					                case 1: delta_dac = DELTA_DAC_S1; break;
                                                        case 2: delta_dac = DELTA_DAC_S2; break;
                                                        case 3: delta_dac = DELTA_DAC_S3; break;
                                                        default: delta_dac = DELTA_DAC_S4; break;
                                                }
					}
					else
					if(d == 'D')
					{
					 flag_576_1152 = 0;	//1152
					}
					else
					if(d == 'E') 
					{
					 flag_576_1152 = 1;	//576
					}
					else
	                   if(d == 'R')
						{

								/*
 							DAC1L	 = 0;
 							DAC1H	 = 0;
							j = 1000;
								while(j > 0L)
								{
								j--;
								}
								
								  */
							do
							{

 							DAC1L	 = LOBYTE(i);
 							DAC1H	 = HIBYTE(i);

							j = 100;
								while(j > 0L)
								{
								j--;
								}
								i++;

							}while(i < 0x0de2) ;

						}
 	}
}