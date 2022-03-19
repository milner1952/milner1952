
#include <string>
#include "svaz.h"

#include "../global.h"
#include "../reg/reg.h"
#include "../test/test_dll.h"

#ifndef unix
    #pragma warning( disable : 4138)
#else
#include <stdio.h>
#include <string.h>
#endif
extern SVAZ svaz;
extern REGDATA *regdata;
//-------------------------------------------------------------------------
SVAZ::SVAZ()
{

}

//-------------------------------------------------------------------------
void SVAZ::Init( int )
{
    pixel_per_string = regdata->pixsels_per_string;
    binning = 1;
  if(pixel_per_string == 2048)
  {
      num_adc = 8;
      num_levels_of_redut = 32;
      num_strs_of_reduc = 4;
      memset(sscr, 0,SIZE_TX_WITHOUT_COMUNICATION);
      memset(dst , 0,SIZE_TX_WITHOUT_COMUNICATION);
      srand(0);
      for(int i = 0; i < 9; i++)
          noise[i] = (WORD)(MASKA_OFF_NOISE & rand());
     ptr_on_level =  SetAllStepOnAllString2048((sscr + pixel_per_string), 65536 - MIN_STEP_DATA_2100);
#ifdef USE_HANDICAP_IN_SVAZ
      SetHandicap(1899, 57000);
#endif


#ifdef USE_PILA_LO_TO_HI
WORD *wtr =  ptr_on_level;
      for(int i = 0; i < 2048; i++)
      {
      wtr = Set_Pila_LoHi(wtr);
      }
#endif

#ifdef USE_PILA_HI_TO_LO
WORD *wtr =  ptr_on_level;
      for(int i = 0; i < 2048; i++)
      {
      wtr = Set_Pila_HiLo(wtr);
      }
#endif

  }
  else
      if(pixel_per_string == 1152)
      {
          num_adc = 9;
          num_levels_of_redut = 32;

          switch(binning)
          {
          case 1: num_strs_of_reduc = 4;break;
          case 2: num_strs_of_reduc = 8; break;
          case 3: num_strs_of_reduc = 12; num_levels_of_redut = 36; break;
          default: num_strs_of_reduc = 16; break;
          }

          memset(sscr, 0,SIZE_TX_WITHOUT_COMUNICATION);
          memset(dst , 0,SIZE_TX_WITHOUT_COMUNICATION);
          srand(0);
          for(int i = 0; i < 9; i++)
              noise[i] = (WORD)(MASKA_OFF_NOISE & rand());
         ptr_on_level =  SetAllStepOnAllString1152((sscr + pixel_per_string), 65536 - MIN_STEP_DATA_2100);
    #ifdef USE_HANDICAP_IN_SVAZ
          SetHandicap(1899, 57000);
    #endif
      }
}

//-----------------------------------------------------------------
void SVAZ::SetHandicap(WORD index_handicap, WORD amp_handicap)
{
    sscr[pixel_per_string + index_handicap -8 *2] = amp_handicap;
    sscr[pixel_per_string + index_handicap -8 *1] = amp_handicap;
    sscr[pixel_per_string + index_handicap +8] = amp_handicap;
    sscr[pixel_per_string + index_handicap +8 *1] = amp_handicap;
    sscr[pixel_per_string + index_handicap +8 *2] = amp_handicap;
    sscr[pixel_per_string + index_handicap +8 *3] = amp_handicap;
    sscr[pixel_per_string + index_handicap +8 *4] = amp_handicap;
}

//-------------------------------------------------------------------
WORD* SVAZ::SetAllStepOnAllString2048(WORD *ptr, WORD a)
{
WORD *ret = ptr;
WORD amp = a;
int i;

            for(i = 0; i < num_strs_of_reduc; i++)
            {
                ret =   SetAllStepOnWanString(ret, amp);
                amp -= AMP_WAN_STEP_2000 * 8;
            }
return ret;
}

//-------------------------------------------------------------------
WORD* SVAZ::SetAllStepOnAllString1152(WORD *ptr, WORD a)
{
WORD *ret = ptr;
WORD amp = a;
int i;
            for(i = 0; i < num_strs_of_reduc ; i++)
            {
                ret =   SetAllStepOnWanString(ret, amp);
                amp -= AMP_WAN_STEP_2000 * num_adc ;//8
            }
return ret;
}

//-----------------------------------------------------------------
WORD* SVAZ::SetAllStepOnWanString(WORD *ptr, WORD a)
{
 WORD *ret = ptr;
 WORD amp = a;
 int i;
            for(i = 0; i < num_adc ; i++)
            {
                ret = SetWanStepOnAllAdc(ret,  amp);
                amp -= (AMP_WAN_STEP_2000);
            }
 return ret;
}


//---------------------------------------------
WORD* SVAZ::SetWanStepOnAllAdc(WORD *ptr, WORD amp)
{
 WORD *ret = ptr;
 int i;
  WORD num_pix_on_own_level = (pixel_per_string / (num_levels_of_redut / num_strs_of_reduc))/ num_adc;
            for(i = 0; i < num_pix_on_own_level; i++ )
            {
                ret =  SetWanStepOnWanPixel(ret, amp);
            }
 return ret;
}

//---------------------------------------------
WORD* SVAZ::SetWanStepOnWanPixel(WORD* ptr, WORD amp)
{
WORD *ret;
            int i;
            for(i = 0; i < num_adc; i++)
            {
                ptr[i] = amp;

#ifdef  USE_NOISE_OF_SVAZ
                                WORD m = i % num_adc;

                double  t = ((double)noise[m]/65536.0);
                        t = t * ((double)ptr[i]);
                        ptr[i] = ((WORD)t + ptr[i]);
#endif
            }
    return ret = &ptr[i];
}
//-------------------------------------------------------------------
void SVAZ::SendArpPacket()
{
}


DWORD SVAZ:: ReceiveBuffer(BYTE ** ptr, DWORD len )
{
    memset(dst , 0,SIZE_TX_WITHOUT_COMUNICATION);
    memcpy(dst, sscr, len);
    *ptr = (BYTE*)dst;
    return len;
}
//----------------------------------------------------------------------
DWORD SVAZ:: ReceiveBuffer(BYTE ** ptr)
{
    if(regdata->pixsels_per_string == 2048)
        *ptr = (BYTE*)"2048 Without communication";
    else
    if(regdata->pixsels_per_string == 1152)
        *ptr = (BYTE*)"1152 Without communication";
    else
    if(regdata->pixsels_per_string == 2304)
        *ptr = (BYTE*)"2304 Without communication";
    else
    if(regdata->pixsels_per_string == 576)
        *ptr = (BYTE*)"576 Without communication";

    return strlen((const char*)*ptr);
}
//-------------------------------------------------------------------------
char* SVAZ::Send(char */*ptr*/)
{

 return NULL;
}

//---------------------------------------------------------------------
//------------------------------------------------------------------------
void SVAZ:: SetNoiseData(WORD */*wtr*/,  bool /*iz_svaz_udp*/,
                                int /*pixsels_per_string*/, bool , DWORD /*pix_num*/)
{

}
//-----------------------------------------------------------------------
WORD* SVAZ::InitLevelMem()
{
WORD k = k_on_level * 6000;
WORD* ptr = ptr_on_level;
DWORD i = pixel_per_string * pixel_per_string;
WORD j = 0;
    do
    {
        *ptr = k;
#ifdef  USE_NOISE_OF_SVAZ
     WORD m = j % num_adc;

     double  t = ((double)noise[m]/65536.0);
     WORD temp = *ptr;
        t = (t * (double)temp);
        t += temp;
        *ptr = ((WORD)t);

       //*ptr += (WORD)(noise[m]/65536);
#endif
        ptr++; i--; j++;
    }while(i > 0) ;
    return ptr;
}

//-----------------------------------------------------------------------
void SVAZ::KeyOnLevel(WORD k)
{
    k_on_level = k;
    memset(sscr, 0,SIZE_TX_WITHOUT_COMUNICATION);
    memset(dst , 0,SIZE_TX_WITHOUT_COMUNICATION);

    for(int i = 0; i < 9; i++)
        noise[i] = (WORD)(MASKA_OFF_NOISE & rand());

if(pixel_per_string == 2048)
{
    binning = 1;
    num_adc = 8;
    num_levels_of_redut = 32;
    if(binning == 3)
        num_levels_of_redut = 36;
    ptr_on_level =  SetAllStepOnAllString2048((sscr + pixel_per_string), 65536 - MIN_STEP_DATA_2100);
#ifdef USE_HANDICAP_IN_SVAZ
    SetHandicap(1899, 57000);
#endif
}
else
if(pixel_per_string == 1152)
{
    num_adc = 9;
    num_levels_of_redut = 32;

    switch(binning)
    {
    case 1: num_strs_of_reduc = 4;break;
    case 2: num_strs_of_reduc = 8; break;
    case 3: num_strs_of_reduc = 12; num_levels_of_redut = 36; break;
    default: num_strs_of_reduc = 16; break;
    }

   ptr_on_level =  SetAllStepOnAllString1152((sscr + pixel_per_string), 65536 - MIN_STEP_DATA_2100);
#ifdef USE_HANDICAP_IN_SVAZ
    SetHandicap(1899, 57000);
#endif
}
#ifdef USE_LAVEL_IN_SVAZ
    InitLevelMem();
#endif
}


//-----------------------------------------------------------------
bool  SVAZ::SendIcmp()
{
#ifndef unix
    ::Sleep(300);
#else
    sleep(1);
#endif
    return true;
}
//------------------------------------------------------------------
WORD*   SVAZ::Set_Pila_LoHi(WORD *wtr)
{
WORD *wwtr  = wtr;
WORD pixel_in_matrix = pixel_per_string/num_adc;

        for(WORD i = 0; i < pixel_in_matrix; i++)
        {
            for(WORD j = 0; j < num_adc; j++)
            {
                *wwtr = j * pixel_in_matrix + i;
                wwtr++;
            }
        }

return wwtr;
}

//------------------------------------------------------------------
WORD*   SVAZ::Set_Pila_HiLo(WORD *wtr)
{
WORD *wwtr  = wtr;
WORD pixel_in_matrix = pixel_per_string/num_adc;

for(WORD i = 0; i < pixel_in_matrix; i++)
{
    for(WORD j = 0; j < num_adc; j++)
    {
        *wwtr = 2048 - j * pixel_in_matrix + i;
        wwtr++;
    }
}

return wwtr;
}








