


//_v2048 8.0.0 � v2034 9.0.0_v01 ������ �������� � ����� 8 � 9 - ��������� 16 ��������� ������� � ������ 16 ��������� �������
//_v2048 8.0.0 ? v2034 9.0.0_v02 ��������� ��������� ������������ ������ �� ������� ����������

#include "dac.h"
#include "main.h"
#include "int0.h"
#include "int1.h"
#include "rs232_1.h"
#include "pmc.h"
#include "timer2.h"

#include <stdlib.h>
#include <string.h>


static void main_Oscillator_init();
static void main_Matrix_init();
static void main_Init_Proc();
static void main_Init_All();

static void main_Decoder();
			
//sbit    A0   =  P0 ^ 4;
//sbit    A1   =  P0 ^ 5;
sbit    RUN   =  P0 ^ 7;
bit     _RUN;
bit     _RUN_RUN;
sbit TestBit = P0 ^ 6;

extern unsigned int tData;
extern word count_string;
extern word w_num_sikle;

extern  word  c_run ;

void main()
{ 

main_Init_All();




//TestBit = 0;
EA = 1;
_RUN_RUN = _RUN = RUN;  
	while(1)
	{
			   
      _RUN = RUN;
	  
 		if((_RUN == 0) & (_RUN_RUN == 1))
 		{
			c_run++;
			// �������� �����
			IE1 = 0;
			count_string = 0;
        	EX1 = 1;   // ��������� ���������� 1 - ���������� �� TRIG/32 			
			EX0 = 1;   // ��������� ���������� �� RST_MTR
			
 		}
		else
		{
		  if(_RUN == 1)
		  {		
		  unsigned int t;
		  		// �������� �����
				IE1 = 0;			 
				CR = 0;// ��������� PMC � �������� ���
				TR2 = 0; // ��������� Timer2
        		EX1 = 0;   // ��������� ���������� 1 - ���������� �� TRIG/32 			
				EX0 = 0;   // ��������� ���������� �� RST_MTR
				count_string  = 0;
				w_num_sikle = (4096L -  (word)/*(MAX_OFFS2/2.0)*/_MAX_PILA);				
							   
							 
    			tData = MAX_OFFS ;
  				DAC0L = tData;
				t =  (tData >> 8);
  				DAC0H = (t & 0x0f);				
		  }
								
		}
       _RUN_RUN = _RUN;				 
	   	   			   

	}

}


//--------------------------------------------------
//
//-------------------------------------------------

static void main_Init_All()
{
main_Init_Proc();
pmc_Init();
timer2_Init();
dac_Init(); 
int0_init();
int1_init();
rs_232_1_Init();
}

//--------------------------------------------------
//
//-------------------------------------------------
static void main_Init_Proc()
{
IE = 0;
TCON = 0;
TMOD = 0;
T2CON = 0;
WDTCN = 0xde;
WDTCN = 0xad;
XBR0 = 0;  
XBR1 = 0;
XBR2 = 0;    
 
main_Oscillator_init();
main_Matrix_init();
}


//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
static void main_Oscillator_init()
{
int i = 0;
OSCXCN    = 0x67;
OSCICN    = 0x08;
   for (i = 0; i < 3000; i++)
	  ; //Wait 1msstruct if () 
	  
	  i = 0;
   while ((OSCXCN & 0x80) == 0)
	   i += OSCXCN;
	  //���������� ������ ��� ������������ ������������
	  //� �������� OSCICN ������ MSCLKE = 1;   ADC0CN
}

//------------------------------------------------
//
//------------------------------------------------
static void main_Matrix_init()
{

XBR0 = 0x04; //UART

XBR1 = 0x14;// INT0, INT1

//WEAKPUD = 0; XBARE = 1;  	������������ �������� ��������; ������� ��������;
XBR2 = 0x40; //  

PRT0CF = 0xFF;
PRT0CF &= 0xBF;
}

