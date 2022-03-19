#include "dac.h"

void dac_VrefOutInit();

void dac_Init()
{
int i = 0;
long j = 0L;
REF0CN = 0x03;


 DAC0L	 = 0xff;//TEST
 DAC0H	 = 0x7f;

  DAC0CN = 0x80;	//80

		 /*
 DAC1L	 = 0xe2;// 0;// 0x10; //REF2
 DAC1H	 = 0x0d;//0x0d;//0x0f;

  DAC1CN = 0x80;
*/
  		   

  DAC1CN = 0x80;

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

//dac_VrefOutInit();
}  

//--------------------------------                                                                           
void dac_VrefOutInit()
{
 REF0CN |= 0x03; //REF1
 
}










