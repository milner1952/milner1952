

#include "GenericTypeDefs.h"

#include "..\..\..\S_KAMERA\SRC\Microchip\Rs232_1.h"

#include <stdlib.h>
#include <stdio.h>

char buf;
WORD indexx; 

//------------------------------------------------------
void __ISR(24, ipl5) InterruptHandler (void)
{
	IFS0bits.U1RXIF = 0;
	buf = U1RXREG;
	if(buf != 0x1b)
		{
          //Прием вставить здесь
 		} 
}

//------------------------------------------------------
void Rs232_1_Init()
{

   U1MODE=0;                     //// Mode Register, Manual DS61168D page 180

   U1MODEbits.SIDL=0;
   U1MODEbits.IREN=0;
   U1MODEbits.RTSMD=0;

   U1MODEbits.UEN=0b00;           //// Just TX/RX, No CTS/RTS

   U1MODEbits.WAKE=1;
   U1MODEbits.LPBACK=0;
   U1MODEbits.ABAUD=0;
   U1MODEbits.RXINV=0;
   U1MODEbits.BRGH=1;
   U1MODEbits.PDSEL=0b00;
   U1MODEbits.STSEL=0;

   U1STA=0;
   U1STAbits.ADM_EN=0;
   U1STAbits.UTXINV=0;
   U1STAbits.URXEN=1;
   U1STAbits.UTXBRK=0;
   U1STAbits.UTXEN=1;
   U1STAbits.ADDEN=0;
   U1STAbits.OERR=0;     
                                 //// Status register, Manual DS61168D page 183

   U1BRG=172;                   //// 172 for 115200 (80 MIPS)

   IFS0bits.U1RXIF=0;
   IPC6bits.U1IP=5;
   IPC6bits.U1IS=3;
   IEC0bits.U1RXIE=1;

   U1MODEbits.ON=1;


}


//----------------------------------------------------------
void Rs232_1_PutChar(char ch)
{
  while(U1STAbits.UTXBF == 1);
  U1TXREG = ch;
}


//----------------------------------------------------------
void Rs232_1_PutString_too_Silabs(char *p,int num)
{
char *ptr;
int len = 0;
ptr = p;
   Rs232_1_PutChar(0x2b);
   while(len < num)
	{
   	Rs232_1_PutChar(*(ptr + len++));
	}
    Rs232_1_PutChar(0x0d);
    Rs232_1_PutChar(0x0a);
}



//----------------------------------------------------
void Rs232_1_PutString(char *p,int num)
{
char *ptr;
int len = 0;
ptr = p;
   Rs232_1_PutChar(0x1b);
   while(len < num)
	{
   	Rs232_1_PutChar(*(ptr + len++));
	}
    Rs232_1_PutChar(0x0d);
    Rs232_1_PutChar(0x0a);
}

//------------------------------------------------

char Rs232_1_GetChar()
{
char t = 0;;
  while(IFS0bits.U1RXIF == 0);
  t = U1RXREG;
  IFS0bits.U1RXIF = 0;
return t;
}






