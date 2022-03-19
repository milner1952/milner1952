//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "dll_src.h"

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdFTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include "reg.h"
 #include "dll_src.h"

#include <new>
#include <iostream>
#include "reg.h"
#include <vector>
using namespace std;


class  TMIdFTP : public TIdFTP
{
wBegin Beg;
wWork  Work;
wEnd End;
 public:
 TMIdFTP(TComponent *Own,bool i,wBegin b = NULL,wWork w = NULL,wEnd e = NULL)
    :TIdFTP(Own)
    {
     Beg = b;
     Work = w;
     End = e;
     OnWorkBegin =   TMWorkBegin;
     OnWork =   TMWork;
     OnWorkEnd = TMWorkEnd;
    }
void __fastcall TMWorkBegin(TObject *Sender,TWorkMode wm,const int sz);
void __fastcall TMWork(TObject *Sender,TWorkMode wm,const int sz);
void __fastcall TMWorkEnd(TObject *Sender,TWorkMode wm);
};


typedef enum{ACCT_RUN_MODE_0 = 0,  // Снимок
        ACCT_TEST_MODE_2  = 2,     // Тестирование без АЦП
        ACCT_TEST_MODE_MN = 10,    // 0 АЦП
        ACCT_TEST_MODE_MX = 11,    //  0x0fff АЦП
        ACCT_TEST_MODE_M  = 12}ACCT_MODE; //Пила


REGDATA regdata;
extern TRegistry *reg;
extern char *   reg_str_resurs_param[];
extern const char *RegKey;

HANDLE hTr;
HANDLE hCurr;
TForm *Form;
TLabel *Lb;

static void dllMesError(char *str, int data, int code);
static void SendMode(ACCT_MODE mode);
 void PerenosStr(WORD **wt);
 void  TimeCorection(BYTE *buf,unsigned long len,WORD len_t);
void ZboiIgl(BYTE **ptr);

DWORD WINAPI StartRutEnd(void *v);
static TIdFTP *IdFTP;
static TMemoryStream *pms;
static HANDLE hRutEnd;
static HANDLE hRutGet;
static HANDLE hRutGetGo;
static BYTE *pBufer;

//-----------------------------------------------------------------------------
void  Init(TComponent* Owner,wBegin b,wWork w,wEnd e)
{
hTr = NULL;
try
{
IdFTP = new TMIdFTP(Owner,1,b,w,e);
}
catch(bad_alloc)
 {
   OutputDebugString("Error - new IdFTP in Init()");
 }
catch(...)
 {
   OutputDebugString("GlbError - new IdFTP in Init()");
 }

pms = NULL;
try
   {
   pms = new TMemoryStream();
   }

catch(bad_alloc)
 {
   OutputDebugString("Error - new pms in Init()");
 }
catch(...)
 {
   OutputDebugString("GlbError - new pms in Init()");
 }

  hRutEnd = CreateEvent(NULL,false,true,NULL);
  hRutGet = CreateEvent(NULL,false,true,NULL);
  hRutGetGo = CreateEvent(NULL,false,true,NULL);
  InitReg();
  pBufer = NULL;
}


//----------------------------------------------------------------------------
bool AfteInitOnCreate()
{
  IdFTP->Host = "n_pribor.ru";
  IdFTP->Password = "microchip";
  IdFTP->User = "ftp";
  pms->Position = 0;


 if(!Connect())
 {
  return false;
 }
  regdata = ReadParamTooReg();
  hCurr = GetCurrentThread();
  SetThreadPriority(hCurr,THREAD_PRIORITY_TIME_CRITICAL);
  return true;
}

//---------------------------------------------------------------------------
bool Connect()
{
        try
        {
        IdFTP->Connect(true);
        }
        catch(...)
        {
         OutputDebugString("Connect - Error");
         return false;
        }
        return true;
}

//-------------------------------------------------------------------------
void Disconnect()
{
   IdFTP->Disconnect();
}

//---------------------------------------------------------------------------
void Dll_Close()
{
WaitForSingleObject(hRutEnd,INFINITE);
   if(IdFTP-> Connected() == true)
        IdFTP->Disconnect();
   if(pms != NULL)
   {
            delete pms;
            pms = NULL;
   }

  regdata.norm_string.erase(regdata.norm_string.begin(),
                               regdata.norm_string.end());
   DestroyReg();
}


//---------------------------------------------------------------------------
int Get(unsigned long len)
{
HANDLE h[2];
h[0] = hRutGet;
h[1] = hRutEnd;

WaitForMultipleObjects( 2, h, true, INFINITE);

if(hTr != NULL)
{
TerminateThread(hTr,0);
        CloseHandle(hTr);
}

OutputDebugString("p0");
 if(len <= 0L)
 {
  throw(1);
 }

OutputDebugString("p1");
if(IdFTP->Connected() == false)
 {
    Connect();
 }
OutputDebugString("p2");

 //ACCT
 AnsiString as("ACCT ");  as += len;
 try
 {
 IdFTP->SendCmd(as.c_str());
 }
 catch(EIdConnClosedGracefully &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  IdFTP->SendCmd(as.c_str());
  OutputDebugString("Get - gracef1");
 }
 OutputDebugString("p3");


//Get
 pms->Position = 0;
 try
 {
    IdFTP->Get("MAIN_FILE.bin",pms);
 }
 catch(EIdConnClosedGracefully &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  OutputDebugString("Get - gracef2");
 }
 catch(EIdProtocolReplyError &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  OutputDebugString("Get - ReplyE1");
 }

 OutputDebugString("p4");
 //Bufer
 BYTE *buf = NULL;
 try
 {
  buf = new char[pms->Size + 2];
 }
 catch(bad_alloc)
 {
   OutputDebugString("Error - new buf in Get");
 }
 catch(...)
 {

   OutputDebugString("GlbError - new buf in Get");
 }

 OutputDebugString("p5");
 memset(buf,0,pms->Size + 2);
 pms->Position = 0;
 if(pms->Size > 0)
    pms->ReadBuffer(buf,pms->Size);
 if(buf != NULL)
 delete[] buf;
 OutputDebugString("p6");

//Thread
 DWORD dwId;
 try
 {
 ResetEvent(hRutGetGo);
 hTr = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartRutEnd,(void*)NULL,CREATE_SUSPENDED,&dwId);
 SetThreadPriority(hTr,THREAD_PRIORITY_LOWEST);
 ResumeThread(hTr);
 }
 catch(...)
 {
  OutputDebugString("Get-CreateThread");
 }

//Выход
 OutputDebugString("p7");
 SetEvent(hRutGet);
 SetEvent(hRutGetGo);

 return pms->Size;
}

//-----------------------------------------------------------------------------
int GetBufer(BYTE **ptr, unsigned long len, bool norm)
{
 int num;

num = GetBuferData(ptr, len,
#ifdef  _METKI_TIME_
(len/2048)*4,
#endif // _METKI_TIME_
norm);

return num;
}
//------------------------------------------------------------------------------
void DeleteBufer()
{
if(pBufer != NULL)
 {
  delete[]  pBufer;
  pBufer = NULL;
 }
}

//-----------------------------------------------------------------------------
int GetBuferData(BYTE **ptr, unsigned long len,
#ifdef  _METKI_TIME_
WORD len_t ,
#endif // _METKI_TIME_
bool norm)
{
HANDLE h[2];
h[0] = hRutGet;
h[1] = hRutEnd;

pBufer = *ptr;

WaitForMultipleObjects( 2, h, true, INFINITE);

if(hTr != NULL)
{
TerminateThread(hTr,0);
        CloseHandle(hTr);
}

OutputDebugString("p0");
 if(len <= 0L)
 {
  throw(1);
 }

OutputDebugString("p1");
if(IdFTP->Connected() == false)
 {
    Connect();
 }
OutputDebugString("p2");

 //ACCT
 AnsiString as("ACCT ");  as += len;
 as += " ";

#ifdef  _METKI_TIME_
 as += len_t;
#endif

 try
 {
 IdFTP->SendCmd(as.c_str());
 }
 catch(EIdConnClosedGracefully &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  IdFTP->SendCmd(as.c_str());
  OutputDebugString("Get - gracef1");
 }
 OutputDebugString("p3");


//Get
 pms->Position = 0;
 try
 {
    IdFTP->Get("MAIN_FILE.bin",pms);
 }
 catch(EIdConnClosedGracefully &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  OutputDebugString("Get - gracef2");
 }
 catch(EIdProtocolReplyError &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  OutputDebugString("Get - ReplyE1");
 }

 OutputDebugString("p4");
 //Bufer
 BYTE *buf = NULL;
 try
 {
  buf = new char[pms->Size + 20];
 }
 catch(bad_alloc)
 {
   OutputDebugString("Error - new buf in GetBufer");
 }
 catch(...)
 {
   OutputDebugString("GlbError - new buf in GetBufer");
 }

 OutputDebugString("p5");
 memset(buf,0,pms->Size + 20);
 pms->Position = 0;
 pms->ReadBuffer(buf,pms->Size );

#ifdef _METKI_TIME_
 TimeCorection(buf,pms->Size,len_t);
 pms->Size -= len_t;
#endif

 *ptr = (BYTE*)(buf);////////////Продол отл

 WORD *wtr = (WORD*)buf;
 PerenosStr(&wtr);

 OutputDebugString("p6");

//Thread
 DWORD dwId;
 try
 {
 ResetEvent(hRutGetGo);
 hTr = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)StartRutEnd,(void*)NULL,CREATE_SUSPENDED,&dwId);
 SetThreadPriority(hTr,THREAD_PRIORITY_LOWEST);
 ResumeThread(hTr);
 }
 catch(...)
 {
  OutputDebugString("Get-CreateThread");
 }
  ////////////////НОРМИРОВКА
  ZboiIgl((BYTE**)ptr);
 if(norm == true)
 {

 WORD *pwr = NULL;
 vector<double> :: iterator i;
 try
 {
  pwr = new WORD[pms->Size/2 + 40];
 }
 catch(bad_alloc)
 {
   OutputDebugString("Error - new pwr in GetBufer");
 }
 catch(...)
 {
   OutputDebugString("GlbError - new pwr in GetBufer");
 }

  memset((void*)pwr, 0, pms->Size + 40);
  memcpy((void*)pwr, (void*)*ptr, pms->Size);
  delete[] buf;

  i = regdata.norm_string.begin();
  for(int j = 0,nrm = 0;j < (pms->Size/2);)
  {
     double d;
     d = *i;
     WORD t;

     if(pwr[j] == 0)
     pwr[j] = 1;


     if(pwr[j] >= 0x3fff)
     pwr[j] = 0x3fff;

     t =  (pwr[j] * (d));
     pwr[j]  = t;


            i++; j++;  nrm++ ;
            if(i == (regdata.norm_string.end()))   {
                        i = regdata.norm_string.begin();
                        nrm = 0;
                        }

  }

    *ptr = (BYTE*)(pwr);
 }

//Выход

 OutputDebugString("p7");
 SetEvent(hRutGet);
 SetEvent(hRutGetGo);

 return pms->Size ;
}



//------------------------------------------------------------------------
DWORD WINAPI StartRutEnd(void *v)
{
bool f;
WaitForSingleObject(hRutGetGo,INFINITE);
SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);
 short res[2] = {211,212};
 short res_o = 0;
do
{
 f = false;
 OutputDebugString("p8");
 while(res_o != (short)212)
       {
     try
        {
        res_o = IdFTP->SendCmd("STAT", res,2);
        }
        catch(EIdConnClosedGracefully &e)
        {
        IdFTP->Disconnect();
        IdFTP->Connect();
        OutputDebugString("Error - EIdConnClosedGracefully - in StartRutEnd");
        f = true;
        }
        catch(EIdSocketError &e)
        {
        IdFTP->Disconnect();
        IdFTP->Connect();
        OutputDebugString("Error - EIdSocketError - in StartRutEnd");
        f = true;
        }
        catch(...)
        {
        OutputDebugString("Error - RutEnd");
        f = true;
        }
      }
 OutputDebugString("p9");
}
 while(f == true);
 Sleep(200);

 SetEvent(hRutEnd);
 //Sleep(2000);   ??????????????????????????????????
 return 0;
}
//----------------------------------------------------------------------
void __fastcall TMIdFTP::TMWorkBegin(TObject *Sender,TWorkMode wm,const int sz)
{
  if(Beg != NULL)
   Beg(sz);
}
void __fastcall TMIdFTP::TMWork(TObject *Sender,TWorkMode wm,const int sz)
{
   if(Work != NULL)
   Work(sz);
}
void __fastcall TMIdFTP::TMWorkEnd(TObject *Sender,TWorkMode wm)
{
   if(End != NULL)
   End();
}

//--------------------------------------------------------------------
void dllMesError(char *str, int data, int code)
{
   AnsiString as;
       as =  *str;
       as += ": ";
       as += data;
       as += " код:";
       as += code;
    MessageDlg(as,mtError,TMsgDlgButtons() << mbOK,0);
}


//-------------------------------------------------------
/*
Задаем:
1. Вывод значение Offs    - void PutOffset(int offset)
2. Вывод значения Gn      -  void PutGane(int gane)
3. Два режима тестирования и работы:    - void SendMode(ACCT_MODE mode)

typedef enum{ACCT_RUN_MODE_0 = 0,  // Снимок
        ACCT_TEST_MODE_2  = 2,     // Тестирование без АЦП
        ACCT_TEST_MODE_MN = 10,    // 0 АЦП
        ACCT_TEST_MODE_MX = 11,    //  0x0fff АЦП
        ACCT_TEST_MODE_M  = 12}ACCT_MODE; //Пила

        - mode 2 - режим тестирования без участия АЦП       - void SetTestMode_2()
        - mode 1 - режим тестирования с участием АЦП
                    с тремя подрежимами:
                      - установка Gn в 0  (MN)              - void SetTestMode_MN()
                      - установка Gn в 0x0FFF (MX)          - void SetTestMode_MX()
                      - генерация пилы ЦАП Gn (M)           - void SetTestMode_M()
        - mode 0 - работа                                   - void SetModeRun()
*/
//------------------------------------------------
void PutOffset(int offset)
{
HANDLE h[2];
h[0] = hRutGet;
h[1] = hRutEnd;
WaitForMultipleObjects( 2, h, true, INFINITE);

OutputDebugString("p2");
 if(IdFTP->Connected() == false)
 {
    Connect();
 }
 //ACCT
 AnsiString as("ACCT ");
 as += "O ";
 as += (offset & 0xfff);

 try
 {
 IdFTP->SendCmd(as.c_str());
 }
 catch(EIdConnClosedGracefully &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  IdFTP->SendCmd(as.c_str());
  OutputDebugString("Get - gracef1");
 }
 AnsiString s(offset & 0x0fff);
 WriteParamTooReg(reg_str_resurs_param[0], s.c_str());
 regdata.offs = offset & 0x0fff;
 OutputDebugString("p3");
  SetEvent(hRutGet);
  SetEvent(hRutEnd);
 return;
 }

//------------------------------------------
 void PutGane(int gane)
{
HANDLE h[2];
h[0] = hRutGet;
h[1] = hRutEnd;
WaitForMultipleObjects( 2, h, true, INFINITE);
OutputDebugString("p2");
 if(IdFTP->Connected() == false)
 {
    Connect();
 }
 //ACCT
 AnsiString as("ACCT ");
 as += "G ";
 as += (gane & 0xfff);

 try
 {
 IdFTP->SendCmd(as.c_str());
 }
 catch(EIdConnClosedGracefully &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  IdFTP->SendCmd(as.c_str());
  OutputDebugString("Get - gracef1");
 }
 AnsiString s(gane & 0x0fff);
 WriteParamTooReg(reg_str_resurs_param[1], s.c_str());
 regdata.gane = gane & 0x0fff;
 OutputDebugString("p3");
 SetEvent(hRutGet);
 SetEvent(hRutEnd);
 return;
 }
              /*
typedef enum{ACCT_RUN_MODE_0 = 0,  // Снимок
        ACCT_TEST_MODE_2  = 2,     // Тестирование без АЦП
        ACCT_TEST_MODE_MN = 10,    // 0 АЦП
        ACCT_TEST_MODE_MX = 11,    //  0x0fff АЦП
        ACCT_TEST_MODE_M  = 12}ACCT_MODE; //Пила
        */
//------------------------------------------
void SendMode(ACCT_MODE mode)
{
OutputDebugString("p2");
 if(IdFTP->Connected() == false)
 {
    Connect();
 }
 //ACCT
 AnsiString as("ACCT ");
 as += "M ";
 as += (mode);

 try
 {
 IdFTP->SendCmd(as.c_str());
 }
 catch(EIdConnClosedGracefully &e)
 {
  IdFTP->Disconnect();
  IdFTP->Connect();
  IdFTP->SendCmd(as.c_str());
  OutputDebugString("Get - gracef1");
 }
 OutputDebugString("p3");
 return;
 }

//---------------------------------------
void SetModeRun()
{
 SendMode(ACCT_RUN_MODE_0);
}

//---------------------------------------
void SetTestMode_2()
{
 SendMode(ACCT_TEST_MODE_2);
}

//---------------------------------------
void SetTestMode_MN()
{
 SendMode(ACCT_TEST_MODE_MN);
}

//---------------------------------------
void SetTestMode_MX()
{
 SendMode(ACCT_TEST_MODE_MX);
}

//---------------------------------------
void SetTestMode_M()
{
 SendMode(ACCT_TEST_MODE_M);
}

//------------------------------------------
int GetOffset()
{
 return regdata.offs;
}

//------------------------------------------
int GetGane()
{
return regdata.gane;
}

//-------------------------------------------------
void GetNormalization()
{
BYTE *ptr = NULL;
long ret = 0;

WORD len = regdata.pixsels_per_string;
WORD num = regdata.num_row_norm;
do
{
to_get:
if(ptr != NULL)
delete[] ptr;
 try
 {
 ret  = GetBuferData((BYTE**)&ptr  ,len * num * 2
 #ifdef _METKI_TIME_
 ,((len * num * 2)/2048)*4
 #endif
 );
 }
 catch(EIdSocketError &e)
 {
   Disconnect();
   Connect();
   OutputDebugString("Normalization");
   goto to_get;
 }
 catch(...)
 {
  OutputDebugString("Error App->GetNoise: Не известная ошибка");
 }
}
while(ret != (len * num * 2));
WORD *pwr = (WORD*)(ptr);
typedef vector<double> V_SUM;
V_SUM v_sum;
double d_sum = 0.0;

int i,j;
double db1;

for( i = 0; i < len; i++)
{
double sum = 0.0;

        for( j = 0; j < num; j++)
        {
        double t;


                if(pwr[i + j * len] >= 0x3fff)
                        pwr[i + j * len] = 0x3fff;
                t = pwr[i + j * len];
                sum += t;
        }
//сохраняем усредненные суммы по столбцам
v_sum.push_back( db1 = sum / num);

       //Прибавляем к общей сумме
d_sum += sum;

}
//Усредняем общую сумму
d_sum = d_sum / ((len * num));

//Открываем реестр
const AnsiString  ch(RegKey);
  try
  {
   reg->OpenKey(ch,false);
  }
  catch(EConvertError &e)
  {
  OutputDebugString("Не могу открыть раздел реестра \
                        Software\\N_PRIBOR_P_KAMERA\\ParamList");
  }
V_SUM :: iterator i_sum;
AnsiString as;
regdata.norm_string.erase(regdata.norm_string.begin(),regdata.norm_string.end());
for(i_sum = v_sum.begin();i_sum != v_sum.end();)
{
double t;
               if(*i_sum == 0.0)
                 t = d_sum/(1.0);
               else
                 t = d_sum/(*i_sum);

                regdata.norm_string.push_back(t);
                as += t;
                as += " ";
i_sum++;
}
reg->DeleteValue(reg_str_resurs_param[3]);
reg->WriteString(reg_str_resurs_param[3],as.c_str());
reg->CloseKey();
delete[] ptr;
}

#include <algorithm>


//---------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

bool MedianFiltrVert(WORD *sc,  unsigned long len ,WORD aprt)
{
if((aprt % 2) == 0)
    return false;
    WORD *src = sc ;
int med = aprt/2;
vector<WORD> srt1, srt2,srt3,srt4;

     for( int x = 0; x < 256; x++)
     {
     int y1, y2,y3,y4, j, w;
     y1 = x*4; y2 = x*4 + 1; y3 = x*4 + 2;   y4 = x*4 + 3;
        for( j = 0; j < (1024 - aprt); y1 += 1024, y2 += 1024, y3 += 1024, y4 += 1024, j++)
        {
               for( w = 0; w < aprt;w++)
               {
               srt1.push_back(src[y1 + 1024*w]);
               srt2.push_back(src[y2 + 1024*w]);
               srt3.push_back(src[y3 + 1024*w]);
               srt4.push_back(src[y4 + 1024*w]);
               }

               sort(srt1.end() - (aprt ) , srt1.end());
               sort(srt2.end() - (aprt ) , srt2.end());
               sort(srt3.end() - (aprt ) , srt3.end());
               sort(srt4.end() - (aprt ) , srt4.end());

               src[y1] = srt1[srt1.size() - (med + 1)];
               src[y2] = srt2[srt2.size() - (med + 1)];
               src[y3] = srt3[srt3.size() - (med + 1)];
               src[y4] = srt4[srt4.size() - (med + 1)];

               srt1.erase(srt1.begin(),srt1.end());
               srt2.erase(srt2.begin(),srt2.end());
               srt3.erase(srt3.begin(),srt3.end());
               srt4.erase(srt4.begin(),srt4.end());
        }

     }
   return true;
}


          /*
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
        
bool MedianFiltrHorz(WORD *sc,  unsigned long len ,WORD aprt)
{
if((aprt % 2) == 0)
    return false;
    WORD *src = sc ;
int med = aprt/2;
vector<WORD> srt1, srt2,srt3,srt4;
BYTE *end = (BYTE*)src +  len ;
for(unsigned long j = 0; src < (WORD*)end   ; j++)
{
   for(WORD w = 0; w < (aprt * 4);)
   {
     srt1.push_back(src[w++]);
     srt2.push_back(src[w++]);
     srt3.push_back(src[w++]);
     srt4.push_back(src[w++]);
   }
   sort(srt1.end() - (aprt ) , srt1.end());
   sort(srt2.end() - (aprt ),  srt2.end() );
   sort(srt3.end() - (aprt ),  srt3.end() );
   sort(srt4.end() - (aprt ) , srt4.end());

   *src++ = srt1[srt1.size() - (med + 1)];
   *src++ = srt2[srt2.size() - (med + 1)];
   *src++ = srt3[srt3.size() - (med + 1)];
   *src++ = srt4[srt4.size() - (med + 1)];

   srt1.erase(srt1.begin(),srt1.end());
   srt2.erase(srt2.begin(),srt2.end());
   srt3.erase(srt3.begin(),srt3.end());
   srt4.erase(srt4.begin(),srt4.end());

}
return true;
}
*/




//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
   /*
bool MedianFiltrHorz(WORD *sc,  unsigned long len ,WORD aprt)
{
if((aprt % 2) == 0)
    return false;
int med = aprt/2;
vector<WORD> srt;
BYTE *end = (BYTE*)sc + len  ;
WORD w;
WORD *src;
WORD t;

int i;
for(i = 0; i < 4; i++)
{
src = sc + i;
for( ; src < (WORD*)end ; )
{
   for(WORD w = 0; w < (aprt * 4);w += 4)
   {
        srt.push_back(t = src[w]);
   }
   sort(srt.end() - (aprt ) , srt.end());
  *src = srt[srt.size() - (med + 1)];
   srt.erase(srt.begin(),srt.end());

   src += 4;
}
}
return true;
}
     */
//#define SIZE_MEM_I_T 1000000 + 10
//WORD mem_i_t[SIZE_MEM_I_T];
//--------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
bool MedianFiltrHorz(WORD *sc,  unsigned long len ,WORD aprt)
{
if((aprt % 2) == 0)
    return false;
int med = aprt/2;
vector<WORD> srt;
BYTE *end = (BYTE*)sc + len  ;
WORD w;
WORD *src;

WORD t;
int i;
for(i = 0; i < 4; i++)
{
src = sc + i;
for( ; src < (WORD*)end ; )
{
   for(WORD w = 0; w < (aprt * 4);w += 4)
   {
        if(((WORD*)(src + w)) <= (WORD*)end)
                srt.push_back(t = src[w]);
        else
        {
                long tt  = src + w - (WORD*)end;
                srt.push_back(t = ((WORD*)(src + 1024))[tt]);
        }
   }
   sort(srt.end() - (aprt ), srt.end());
   *src = srt[srt.size() - (med + 1)];
   src += 4;
   srt.erase(srt.begin(),srt.end());
}
}
return true;
}




 //-----------------------------------------------------------
 //
 //----------------------------------------------------------
 void PerenosStr(WORD **wt)
 {
 WORD *wtr;
 wtr = *wt;
#define SZ_PERENOS (128 * 4 * sizeof(WORD))


 for(int j = 0; (j * 256 * 4) < (pms->Size/2);j++)
 {
 WORD temp[128 * 4];
   memcpy(temp,(void*)&wtr[j * 256 * 4], SZ_PERENOS);
   memcpy((void*)&wtr[j * 256 * 4],(void*)&wtr[j * 256 * 4 + 128 * 4],SZ_PERENOS);
   memcpy((void*)&wtr[j * 256 * 4 + 128 * 4],(void*)temp,SZ_PERENOS);

   WORD *beg = &wtr[j * 256 * 4];
   WORD *end = &wtr[(j * 256 * 4 + 128 * 4) - 4];
   for(int i = 0 ;i < 64 ; i++, end = end - 8)
   {
    WORD temp = *beg;
        *beg  = *end;
        *end = temp;
        beg++; end++;

        temp = *beg;
        *beg  = *end;
        *end = temp;
         beg++; end++;

        temp = *beg;
        *beg  = *end;
        *end = temp;
         beg++; end++;

        temp = *beg;
        *beg  = *end;
        *end = temp;
         beg++; end++;
   }

   beg = &wtr[j * 256 * 4 + 128 * 4];
   end = &wtr[(j * 256 * 4 + 256 * 4) - 4];
   for(int i = 0 ;i < 64 ; i++,end = end - 8)
   {
 WORD temp = *beg;
        *beg  = *end;
        *end = temp;
        beg++; end++;

        temp = *beg;
        *beg  = *end;
        *end = temp;
         beg++; end++;

        temp = *beg;
        *beg  = *end;
        *end = temp;
         beg++; end++;

        temp = *beg;
        *beg  = *end;
        *end = temp;
         beg++; end++;
   }

 }

WORD *beg;

 beg = *wt;
long  sz =    pms->Size/8;
 for(int i = 0,j = 0; i < sz ; i++,j++)
 {
   WORD t = beg[3];
   beg[3] = beg[2];
   beg[2] = beg[1];
   beg[1] = beg[0];
   beg[0] = t;
beg += 4;
 }

}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void  TimeCorection(BYTE *buf,unsigned long len,WORD len_t)
{
unsigned long *ltr,*tltr;
double sum = 0.0;
__int64 sum_t = (__int64)0;
double kf[1024];

int i;
int num;
 num =  len_t/sizeof(unsigned long);
 tltr = ltr = (unsigned long*)&buf[len - len_t];
      for(i = 0; i < num; i++)
      {
       __int64 tt = (__int64)(*ltr++);
       if(i > 1)
                sum_t += tt;
      }

      sum = (double)(sum_t/((__int64)(i - 2)));
      memset((void*)&kf[0],0.0,sizeof(double) * 1024);
      for(i = 0, ltr = tltr; i < num; i++)
      {
        double tt =  (*ltr++);
        tt = tt/sum;
        if( i > 1)
              kf[i] = tt;
      }
      WORD* wtr = (WORD*)buf;
          
      int j;
      for(i = 0; i < num ;i++)
      {
       for(j = 0; j < 1024;j++)
       {
       #define M -2
         if((i > 1)   &&  (kf[i + M] > 0))
         {
           double   d = ((double)wtr[j + i * 1024]);
           d = d/kf[i + M];
          wtr[j + i * 1024] = (WORD)(d);
         }
       }
      }
}
void Zbr(WORD *beg, BYTE indx);
//--------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------
void ZboiIgl(BYTE **ptr)
{
WORD *beg;
 beg = (WORD*)*ptr;
long  sz =    pms->Size/32;
int j = 0;
int i = 0;
 for(j = 0,i = 0; i < sz ; i++, beg += 16)
 {
   if(j == (63))
   {
      Zbr(beg,0);
      Zbr(beg,1);
      Zbr(beg,2);
      Zbr(beg,2);
     j = 0;
     continue;
   }
   j++;
 }

 }


 //-----------------------------------------------------
 //
 //------------------------------------------------
void Zbr(WORD *beg, BYTE index)
{
   if(beg[-4 + index] >  beg[ index + 16 + 1 - 1024])

   {
   WORD *tb = &beg[index -4];
   WORD *te = &beg[index + 16 + 1 - 1024];
    WORD w = (*tb - *te)/5;

    beg[index + 0] =  *te + w;
    beg[index + 4] =  *te + 2 * w;
    beg[index + 8] =  *te + 3 * w;
    beg[index + 12] = *te + 4 * w;
   }
   else
   {
   WORD *tb = &beg[ index + 16 + 1 - 1024];
   WORD *te = &beg[index - 4 ];
    WORD w = (*tb - *te)/5;

    beg[index + 12] =  *te + w;
    beg[index + 8] =   *te + 2 * w;
    beg[index + 4] =   *te + 3 * w;
    beg[index + 0] =   *te + 4 * w;
   }

}



#pragma package(smart_init)









