
#ifndef __BORLANDC__
#define _CRT_SECURE_NO_WARNINGS
#ifndef _CRT_SECURE_NO_DEPRECATE
# define _CRT_SECURE_NO_DEPRECATE (1)
#endif 
#pragma warning(disable : 4996)
#pragma warning(disable: 4800)
#endif

#pragma hdrstop
#ifdef __BORLANDC__
	#include <vcl.h>
	#include "app_test.h"
#endif

#include "stdafx.h"

#include "reg_app2.h"
#include <new>


using namespace std;
#define NUM_PARAM 13

#ifdef __BORLANDC__
extern TForm1 *Form1;
#endif

static const char *RegKey = "Software\\N_PRIBOR_P_KAMERA_APP_TEST_2048\\ParamList";
static char *   reg_str_resurs_param[NUM_PARAM] = {"POZITION_H",  "POZITION_L",  "ZOOM_X",  "ZOOM_Y",
                                                   "SCRULBARBAZA_X",   "SCRULBARBAZA_Y", "N_POS_NOISE",
                                                   "ZOOM_IMAGE", "IZ_HEX", "STR_GRID_TOP_ROW","SCRULBARBAZA_Y_IM",
                                                   "TIMER_ZDR","COM_NAME"
                                                   };

//---------------------------------------------------------------------------
//                              (&baza,&pozH,&dlm);
REGDATA_APP2::REGDATA_APP2(const REGDATA_APP2 &r)
{
memset(com_name, 0, sizeof(com_name));
pozH = r.pozH;
pozL = r.pozL;
zoom_x = r.zoom_x;
zoom_y = r.zoom_y;
imScrBX = r.imScrBX;
imScrBY = r.imScrBY;
zd_poz = r.zd_poz;
zoom_im = r.zoom_im;
sgtr = r.sgtr;
izhex = r.izhex;
scrbaza_y_im = r.scrbaza_y_im;
timer_zdr = r.timer_zdr;
}
//-----------------------------------------------------------------
//
void REGDATA_APP2:: copy(const REGDATA_APP2 *r)
{
memset(com_name, 0, sizeof(com_name));
pozH = r->pozH;
pozL = r->pozL;
zoom_x = r->zoom_x;
zoom_y = r->zoom_y;
imScrBX = r->imScrBX;
imScrBY = r->imScrBY;
zd_poz = r->zd_poz;
zoom_im = r->zoom_im;
sgtr = r->sgtr;
izhex = r->izhex;
scrbaza_y_im = r->scrbaza_y_im;
timer_zdr = r->timer_zdr;
}

//---------------------------------------------------------------------------
//                              (&baza,&pozH,&dlm);

REGDATA_APP2::REGDATA_APP2( )
{
 try
 {
   reg = new  TRegistry;
 }
 catch(bad_alloc)
 {
   OutputDebugString("Error - new reg in REGDATA_APP2()");
 }
 catch(...)
 {
   OutputDebugString("GlbError - new reg in REGDATA_APP2()");
 }

 reg->RootKey = HKEY_CURRENT_USER;
 ReadParamTooReg();
}


//--------------------------------------------------------------
bool REGDATA_APP2::ReadParamTooReg( )
{
const AnsiString  ch(RegKey);
   if(!reg->OpenKey(ch,false))
    {
       reg->OpenKey(ch,true);
       for(int i = 0;i < NUM_PARAM;i++)
       {
          reg->WriteString(reg_str_resurs_param[i],"");
          switch(i)
          {
           case 0:
           reg->WriteString(reg_str_resurs_param[i],"0");
           break;
           case 1:
           reg->WriteString(reg_str_resurs_param[i],"0");
           break;
           case 2:
            reg->WriteString(reg_str_resurs_param[i],"2");
           break;

           case 3:
            reg->WriteString(reg_str_resurs_param[i],"1");
           break;

           case 4:
            reg->WriteString(reg_str_resurs_param[i],"0");
           break;

           case 5:
            reg->WriteString(reg_str_resurs_param[i],"0");
           break;

           case 6:
            reg->WriteString(reg_str_resurs_param[i],"0");
           break;
           case 11:
            reg->WriteString(reg_str_resurs_param[i],"0");
           break;
           case 12:
            reg->WriteString(reg_str_resurs_param[i],"COM1");
           break;
           default :
           reg->WriteString(reg_str_resurs_param[i],"1");
           break;
          }
       }
    }

//-----------------------
AnsiString as = reg->ReadString(reg_str_resurs_param[0]);
#ifdef __BORLANDC__
         try
         {
			pozH = (WORD)as.ToInt();
         }
		 catch(EConvertError &e)
         {
			OutputDebugString("Error - Konvert POZH");
         return false;
         }
#else
		    pozH  = atoi(as.c_str());
#endif
//---------------------------------
		 as = reg->ReadString(reg_str_resurs_param[1]);
#ifdef __BORLANDC__
        try
        {
        pozL = (WORD)as.ToInt();
        }
        catch(EConvertError &e)
        {
        OutputDebugString("Error - Konvert POZL");
        return false;
        }
#else
		    pozL  = atoi(as.c_str());
#endif
//------------------------
		 as  = reg->ReadString(reg_str_resurs_param[2]);
#ifdef __BORLANDC__
         try
         {
         zoom_x = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert ZOOM_X");
         return false;
         }
#else
		 zoom_x  = atoi(as.c_str());
#endif

//------------------------
		 as  = reg->ReadString(reg_str_resurs_param[3]);
#ifdef __BORLANDC__
         try
         {
         zoom_y = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert ZOOM_Y");
         return false;
         }
#else
		 zoom_y  = atoi(as.c_str());
#endif

//-------------------------
		 as  = reg->ReadString(reg_str_resurs_param[4]);
#ifdef __BORLANDC__
         try
         {
         imScrBX = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert IMSCRULBAR");
         return false;
         }
#else
		 imScrBX  = atoi(as.c_str());
#endif

//-------------------------
		 as  = reg->ReadString(reg_str_resurs_param[5]);
#ifdef __BORLANDC__
         try
         {
         imScrBY = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert IMSCRULBAR");
         return false;
         }
#else
		 imScrBY  = atoi(as.c_str());
#endif

//-------------------------
		 as  = reg->ReadString(reg_str_resurs_param[6]);
#ifdef __BORLANDC__
         try
         {
         zd_poz = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert ZDR_NOISE");
         return false;
         }
#else
		  zd_poz  = atoi(as.c_str());
#endif

 //-------------------------
		 as  = reg->ReadString(reg_str_resurs_param[7]);
#ifdef __BORLANDC__
         try
         {
         zoom_im = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert ZOOM_IMAGE");
         return false;
         }
#else
		 zoom_im  = atoi(as.c_str());
#endif

 //-------------------------
		 as  = reg->ReadString(reg_str_resurs_param[8]);
#ifdef __BORLANDC__
         try
         {
         izhex= (bool)(as.ToInt()== 0? 0 : 1);
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - iz_hex");
         return false;
         }
#else
		 izhex  = (atoi(as.c_str()) == 0 ? 0 : 1);
#endif

  //-------------------------
		 as  = reg->ReadString(reg_str_resurs_param[9]);
#ifdef __BORLANDC__
         try
         {
         sgtr = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert SGTR");
         return false;
         }
#else
		 sgtr  = atoi(as.c_str());
#endif


//--------------------------------------
                 as  = reg->ReadString(reg_str_resurs_param[10]);
#ifdef __BORLANDC__
         try
         {
         scrbaza_y_im = (WORD)as.ToInt();
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert SCRBAZA_Y_IM");
         return false;
         }
#else
		 sgtr  = atoi(as.c_str());
#endif
//-----------------------------------------------------------------------------
                 as  = reg->ReadString(reg_str_resurs_param[11]);
#ifdef __BORLANDC__
         try
         {
          timer_zdr = (WORD)as.ToInt();
     //     Form1->Tmr_zdr->Interval  = timer_zdr * 1000;
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert TIMER ZDR");
         return false;
         }
#else
		 timer_zdr  = atoi(as.c_str());
#endif



//-----------------------------------------------------------------------------
                 as  = reg->ReadString(reg_str_resurs_param[12]);
#ifdef __BORLANDC__
         try
         {
          strcpy(com_name, as.c_str());
         }
         catch(EConvertError &e)
         {
         OutputDebugString("Error - Konvert COM_NAME");
         return false;
         }
#else
		 strcpy(com_name, as.c_str());
#endif
		 reg->CloseKey();


   return true;
}


      //SCRULBARBAZA_Y_IM
//------------------------------------------------------------
bool REGDATA_APP2::WriteParamTooReg(WORD Param, WORD Value)
{
if(Param > NUM_PARAM )
return false;
#ifdef __BORLANDC__
	AnsiString as = Value;
#else
	AnsiString as;
	char buf[20];
		memset(buf, 0, 20);
		itoa(Value,buf,10);
		as = buf;
#endif

 reg->OpenKey(RegKey,true);
 reg->WriteString(reg_str_resurs_param[Param],"");
 reg->WriteString(reg_str_resurs_param[Param],as);
 reg->CloseKey();
 return true;
}

//------------------------------------------------------------
bool REGDATA_APP2::WriteParamTooRegFromBOOL(WORD Param, bool Value)
{
if(Param > NUM_PARAM )
return false;
AnsiString as = Value == 0 ? "0" : "1";

 reg->OpenKey(RegKey,true);
 reg->WriteString(reg_str_resurs_param[Param],"");
 reg->WriteString(reg_str_resurs_param[Param],as);
 reg->CloseKey();
 return true;
}
//------------------------------------------------------------
bool REGDATA_APP2::WriteAllParamTooReg()
{
WORD *wtr = (WORD*)((BYTE*)this + sizeof(TRegistry*));
        for(int i = 0; i <   NUM_PARAM - 1; i++)
        {
                WriteParamTooReg(i, wtr[i]);
        }

 WriteParamTooRegFromBOOL(9, this->izhex );
 return true;
}



//------------------------------------------------------------
bool REGDATA_APP2::WriteParamTooRegFromString(WORD Param, AnsiString as)
{
if(Param > NUM_PARAM )
return false;


 reg->OpenKey(RegKey,true);
 reg->WriteString(reg_str_resurs_param[Param],"");
 reg->WriteString(reg_str_resurs_param[Param],as);
 reg->CloseKey();
 return true;
}

