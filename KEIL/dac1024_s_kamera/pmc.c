#include "pmc.h"
#include "main.h"

extern word w_timer2_init; 

//-----------------------
//
//----------------------------
void pmc_Init()
{
//Прерывания от Таймера/счетчика PMC запрещены, тактирпование SYSCLK/4 
 PCA0MD = 0x02;
 PCA0L = 0;
 PCA0H = 0;
 // Таймер/счетчик выключили
 CR = 0;
}





