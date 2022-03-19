#include "int0.h"
#include "int1.h"
#include "main.h"
#include "pmc.h"
#include "timer2.h"
#include "type_app.h"



sbit    RUN   =  P0 ^ 7;
sbit TestBit = P0 ^ 6;
#ifdef TEST_SCREEN__TYPE_APP_H
	extern word im_n;							
	extern word str_n;										
	extern int  amp_noise;
#endif




extern word w_num_sikle;
word   count_string;
word c_run;
extern unsigned  char type_test;
extern unsigned  char binning;

byte  flag_test_screen ;
//-------------------------------------------------------------------
// ���������� �� RES_MTR  
//-------------------------------------------------------------------				 
void int0 (void) interrupt 0 using 1  
{
 	int0_isr(); 
 	IE0 = 0;
}

//--------------------------------------------------
 void int0_init()
 {
 
		   c_run = 0;
		   flag_test_screen = 0;
 //���������� ������������� � �����
 	IT0 = 1;
 //��������� ����������-������
 	PX0 = 1;
 //���������� ���������� INT0 
 	EX0 = 1;	  ///1
 	count_string = 0;
 }

//---------------------------------------------------------- 
//  ���������� �� RES_MTR
//----------------------------------------------------------
 void  int0_isr()
 {
 //	static int c_8;
//	static int c_256;	
  
	w_num_sikle = (4096L -  (word)_MAX_PILA);	


		if((count_string > (4 * binning)) && (count_string < (11 * binning))) 
		{
			EX1 = 0;   // ��������� ����������  ���������� �� TRIG/32
 			int1_InitDacPerLine();
		}
			
	if(type_test == 1)
	{			  	
		if(count_string >= (11 * binning))
		{
		    PCA0H = 0;
			PCA0L = 0;
		   	CR = 1;// �������� PMC � �������� ���
			TR2 = 1; // �������� Timer2
		}
	}
		/*
	else
	if(type_test == 2)
	{



		if(count_string >= 11)
		{

			word t = count_string >> 7;
		    DAC0L = LOBYTE(t + 3000);
			DAC0H = 0x0f & (HIBYTE(t + 3000));
		}
	}
	   	  */
	count_string++;
 }
 