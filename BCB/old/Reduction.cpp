//---------------------------------------------------------------------------


#ifndef unix
#ifndef __BORLANDC__
    #define _CRT_SECURE_NO_WARNINGS

    #ifndef _CRT_SECURE_NO_DEPRECATE
        # define _CRT_SECURE_NO_DEPRECATE (1)
    #endif

    #pragma warning(disable : 4996)
    #pragma warning(disable: 4800)
#endif



#include <windows.h>
#include "stdafx.h"
#endif


#include "Reduction.h"
#include "Reduction_class.h"
REDUCTION rduc;

#ifdef _TEST
         WORD test2[256][9];
    #ifndef unix
        extern __int64 glob;
    #else
        extern /*long long*/int glob;
    #endif
#endif

 #include <math.h>
#ifndef __BORLANDC__
#include <stdio.h>
    #ifndef unix
//---------------------------------------------------------------------------
        BOOL APIENTRY DllMain(	HANDLE hModule,
                                DWORD  ul_reason_for_call,
                                LPVOID lpReserved
                                )
        {
            return TRUE;
        }
    #endif
#else
//---------------------------------------------------------------------------
	#include <vcl.h>
	#include <Math.hpp>
	#pragma argsused
	int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
	{
			return 1;
	}
#endif

DWORD sum_in[9];
DWORD sum_out[9];

//---------------------------------------------------------------------------
//new
//---------------------------------------------------------------------------
bool rduc_SetData(WORD *pWtr, DWORD num)
{

WORD sz = rduc.Size_level_in_byte();
WORD n_lv = rduc.GetNumLevels();
        if((((DWORD)(n_lv * sz)) > (num * 2)) || (num == 0L))
        {
                rduc.Error((char*)"The length of the transmitted buffer is small", (char*)"ReductionSetData");
                return false;
        }
        if(pWtr == NULL)
        {
                rduc.Error((char*)"Base buffer transferred", (char*)"ReductionSetData");
                return false;
        }
#ifdef _TEST
if(rduc.Num_pix_in_string() == 2048)
        memset((void*)test2, 0, 256 * 8 *  sizeof(WORD));
else
        if(rduc.Num_pix_in_string() == 2304)
                memset((void*)test2, 0, 256 * 9 *  sizeof(WORD));
        else
        if(rduc.Num_pix_in_string() == 1024)
                memset((void*)test2, 0, 128 * 8 *  sizeof(WORD));
                else
                if((rduc.Num_pix_in_string() == 1152) ||
                   (rduc.Num_pix_in_string() == (1152/2)) ||
                   (rduc.Num_pix_in_string() == (1152/3)) ||
                   (rduc.Num_pix_in_string() == (1152/4)))
                        memset((void*)test2, 0, 128 * 9 *  sizeof(WORD));
        glob = 0;
#endif

        for(int i = 0; i < n_lv;i++)
        {
#ifdef _TEST
                if(rduc.Num_pix_in_string() == 2048)
                        memset((void*)test2, 0, 256 * 8 * 2* sizeof(WORD));
                else
                        if(rduc.Num_pix_in_string() == 2304)
                                memset((void*)test2, 0, 256 * 9 * sizeof(WORD));
                               else
                                if(rduc.Num_pix_in_string() == 1024)
                                        memset((void*)test2, 0, 128 * 8 * sizeof(WORD));
                                        else
                                                if((rduc.Num_pix_in_string() == 1152) ||
                                                   (rduc.Num_pix_in_string() == (1152/2)) ||
                                                   (rduc.Num_pix_in_string() == (1152/3)) ||
                                                   (rduc.Num_pix_in_string() == (1152/4)))
                                                                memset((void*)test2, 0, 128 * 9 * sizeof(WORD));
#endif
if(i == 1)
i = i;
                rduc.SetLevel(pWtr, i);
        }
        rduc.Set_midl_l_r();
        rduc.SetLianer();
return true;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
bool rduc_GetData(WORD *pWtr, DWORD num)
{
#ifdef _TEST
        glob = 0L;
#endif
WORD sz = rduc.Size_level_in_byte();

WORD n_lv = rduc.GetNumLevels();
        if((((DWORD)(n_lv * sz)) > (num * 2)) || (num == 0L))
        {
                rduc.Error((char*)"The length of the transmitted buffer is small", (char*)"ReductionGet");
        return false;
        }
        if(pWtr == NULL)
        {
                rduc.Error((char*)"Base buffer transferred", (char*)"ReductionGet");
        return false;
        }
//WORD maska = rduc.Maska();
DWORD n_rduc = rduc.NumWordReduction();
WORD *wtr = (WORD*)(pWtr);
WORD w;
        //Pointer of FindL or FindR
typedef RDUC_DATA* (REDUCTION::*FIND)(WORD w,WORD *l);
FIND find;

        if(rduc.Midl_l() == rduc.Midl_r())
        {
        AnsiString as = "ReductionGet";
                as += "rduc.Midl_l() == rduc.Midl_r() = ";
#ifndef __BORLANDC__
                M_SPRINTF(as, rduc.Midl_r())
#else
                                as += rduc.Midl_r();
#endif
                rduc.Error((char*)"Initialization error", (char*)as.c_str());
        return false;
        }
        if(rduc.Midl_l() < rduc.Midl_r())
                find =  (FIND)&REDUCTION::FindL;//L
        else
                find =  (FIND)&REDUCTION::FindR;  //R
RDUC_DATA *rdt;
WORD level;

                int p = 0;

                //DWORD j;
                for(DWORD i = 0, j = 0; i < num ; i++, p++)
                {

                        w = wtr[i];

                       if(i == (576 * 9))
                         i = i;

                       if(w == 62294)
                       w = w;


                        rdt = (rduc.*find)(w,(WORD*)&level);

                double lianer = rduc.Lianer(level);
                        if(rdt == NULL)
                        {
                                AnsiString mes = "Index in the buffer = ";


#ifndef __BORLANDC__
                                M_SPRINTF(mes, (i + n_rduc))
#else
								mes += (i + n_rduc);
#endif
								mes += "Value in the buffer = ";


#ifndef __BORLANDC__
                                M_SPRINTF(mes, w)
#else
								mes += w;
#endif

                                rduc.Error((char*)"ReductionGet",(char*)"Reduction structure not found" ,(char*)mes.c_str());
                        return false;
                        }
                  double r_d;
                        r_d = rdt-> deviation[j];
                        wtr[i] = (unsigned short int)ceil(((double)wtr[i]) * r_d * lianer);


                        if(j < 8)
                           j++;
                        else
                           j = 0;

                }
return true;
}

//----------------------------------------------------------------------
AnsiString* rduc_GetMesTest()
{
 return rduc.Mes();
}

//--------------------------------------------------------------------
void rduc_Init(WORD w, WORD lf, WORD rg, BYTE bn)
{
        rduc.Init(w, lf, rg, bn);
}
//-----------------------------------------------------------------------
void rduc_ClearMesText()
{
 #ifdef _TEST
        rduc.ClearMesText();
 #endif
}

//----------------------------------------------------------------------------

AnsiString rduc_SetTestReductDataToFile()
{
return rduc.SetReductDataToFile();
}

//------------------------------------------------------------------------
void rduc_GetMideleForBuferInFromADC(WORD *pWtr, DWORD num)
{
rduc.GetMidleForBuferFromADC(pWtr + rduc.NumWordReduction() + 2048,num - rduc.NumWordReduction() - 2048, sum_in);
}

//------------------------------------------------------------------------
void rduc_GetMideleForBuferOutFromADC(WORD *pWtr, DWORD num)
{
rduc.GetMidleForBuferFromADC(pWtr + rduc.NumWordReduction() + 2048,num - rduc.NumWordReduction() - 2048, sum_out);
}

//------------------------------------------------------------------------
DWORD* rduc_GetPtrForMidleBuferIn()
{
return sum_in;
}
//------------------------------------------------------------------------
DWORD* rduc_GetPtrForMidleBuferOut()
{
 return sum_out;
}
//----------------------------------------------

