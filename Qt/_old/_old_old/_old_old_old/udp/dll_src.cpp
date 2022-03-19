

#ifndef unix
#include <direct.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include <ctype.h>
#ifndef unix
    #include <Objbase.h>
#endif

#include <string>
#include <string.h>


using namespace std;


#include <time.h>
#include <vector>
#include <algorithm>
#include <functional>

#include "svaz.h"
#include "dll_src.h"
#include "regdata.h"
#include "p_dll_tif_buffer.h"
#include "../exp_error/exp_error.h"
#include "../app/reduct_test.h"

 void MessegeOfBufer(WORD *ptr, DWORD len, WORD *beg);
#ifndef unix
    #include <Windows.h>
    #include <WinUser.h>
DWORD  UnicodeToAnsi(TCHAR *pszW, LPSTR *ppszA);
DWORD  AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW);
#else
    #include <stdio.h>
    #include <unistd.h>
    #include <stdio.h>
    #include <dlfcn.h>
#endif
static char *ptr_e;
const char str_dll_version[] = "DLL_VERSION: ";
const char str_version_of_sensor[] = ", VERSION OF SENSOR: ";

char *ptch_ini;
char *ptch_work;

//reg
WRITEPARAMTOOREG  g_WriteParamTooReg;
DESTROYREG g_DestroyReg;
REG_GETERROR g_reg_GetError;
//test
#ifndef unix
    HMODULE  test_lib;
#else
    void* test_lib;
#endif

extern TIFF_GETERROR g_tiff_GetError;
extern TIFF_SET_FLAGMAINAPP g_tiff_setFlagMainAPP;

TEST_DLL_SHOW_S_DW  g_test_dll_Show_s_dw;
TEST_DLL_SHOW_S_DW_S  g_test_dll_Show_s_dw_s;
TEST_DLL_SHOW_S_US  g_test_dll_Show_s_us;
TEST_DLL_SHOW_S_D g_test_dll_Show_s_d;
TEST_DLL_SHOW_S_BT g_test_dll_Show_s_bt;
TEST_DLL_SHOW_S_DW_S_DW g_test_dll_Show_s_dw_s_dw;
TEST_DLL_SHOW_S_S  g_test_dll_Show_s_s;
TEST_DLL_CLOSE g_test_dll_Close;
TEST_GETERROR g_test_GetError;

//adapter
ADAPTER_SEND g_adapter_Send;
ADAPTER_RESIVEBUFFER g_adapter_ResiveBuffer;
ADAPTER_GETCHERROR g_adapter_GetChError;
ADAPTER_DELETE g_adapter_Delete;
ADAPTER_SETPIXELPERSTRING g_adapter_SetPixelPerString;
ADAPTER_SEND_ICMP g_adapter_SendIcmp;
ADAPTER_GETERROR g_adapter_GetError;


//reduct
RDUC_INIT g_rduc_Init;
RDUC_GETPTRFORMIDLRBUFERIN  g_rduc_GetPtrForMidleBuferIn;
RDUC_GETPTRFORMIDLRBUFEROUT  g_rduc_GetPtrForMidleBuferOut;
RDUC_GETMIDELEFORBUFERINFROMADC g_rduc_GetMideleForBuferInFromADC;
RDUC_GETMIDELEFORBUFEROUTFROMADC g_rduc_GetMideleForBuferOutFromADC;
RDUC_SETDATA g_rduc_SetData;
RDUC_GETDATA g_rduc_GetData;
RDUC_GETERROR g_rduc_GetError;



#ifdef USE_REDUCT_TEST
    RDUC_GETCLASSPTR g_rduc_GetClassPtr;
    RDUC_SETBUFEROFNONREDUCDATA g_rduc_SetBuferOfNonReducData;        
    RDUC_SETNPOSSTRINGNOISE g_rduc_SetNposstringnoise;
#endif

const char *   reg_str_resurs_param_name[] =      { "TIMER_PERIOD"     ,"PIXELS_PER_STRING",
                                                    "SAVE_IN_BUFFER_IN_FILE_OR_READ_IT","5MC_PER_STRING", "IZ_LOG_DEBUG", "IZ_REDUCT",
                                                    "IZ_SVAZ_UDP","MTR_GANE", "IN_FILE_OF_BUFFER",
                                                     "LEFT_REDUCT_FRONT",
                                                    "RIGHT_REDUCT_FRONT", "IF_ROTATION", "WITH_NOISE_STATISTICS", "WITH_MATRICES",
                                                    "TIME_REPIT", "REVERS_MATRIX", "IZ_DAC", "IZ_BINNING",
                                                    "IZ_HARD_FILTER","DIV_16_9_4","DIV_4_3_2","IZ_RANGE",
                                                    "REDUKTION_TO_RANGE","FILE_TIF", "IZ_ICMP_TEST", "TIME_PER_STRING","IZ_SOFT_FILTER"
                                                    };

const char *   defstr[] = {"10","1152",
                           "0","0","0","0",
                           "1","1","0",
                           "5",
                           "5","0","0","1",
                           "1000","0","1","1",
                           "1","0","0","0",
                           "65535","0","0","0","5"};
REGSTRING *regstring;
TRegistry *reg;
const char *RegKey = "Software\\N_PRIBOR_P_KAMERA_2048_DLL_SRK\\ParamList";
REGDATA *regdata;
char Version[100];
char bufer[100];
SVAZ svaz;
DWORD global;


bool flagMainApp;
const char *time_tx[]={"0","1"};
DWORD EXP_ERROR::error;
string EXP_ERROR::text_out;

string s_ini;
string s_work;


//--------------------------------------------------------------------------
//Инициализация dll
// аргументы:
//      const char* dir_work - путь к рабочей директории - где находятся dll
//      const char* patch_ini - путь к файлу .ini (в Windows, для Linux путь не используется)
// возвращаемое значение:
//      - нет
// коментарии:
//      функция - обертка для обработок except функции void p_dll_Init(const char* patch_ini );
//    После вызова функции, ф ункция DWORD p_dll_GetError(char**) покажет наличие ошибок
//---------------------------------------------------------------------------
void Init(const char *dir_work, const char* patch_ini)
{
    EXP_ERROR::ErrorRes();
    try{
    p_dll_Init(dir_work, patch_ini);
    }
    catch(EXP_ERROR &e)
    {
         e.ErrorSetText("p_dll_udp");
    }
}

//--------------------------------------------------------------------------------------------
//Инициализация dll
//      аргументы:
//      const char* dir_work - путь к рабочей директории
//      const char* patch_ini - путь к файлу .ini (в Windows, для Linux путь не используется)
// возвращаемое значение:
//         0 -  при отсутствии ошибок
//         1 -  при наличии ошибок
// коментарии:
//      функция - выбрасывает исключение:
//              - при не возможности открыть библиотеку
//              - при не возможности найти фунцию в библиотеке
//              - при невозможности найти имя модуля
//              - при невозможности преобразоват ANSI в "широкие" символы
//      функция транслирует исключения генерируемые функциями:
//          - REGDATA*  p_dll_src_FaseReg(REGSTRING *r_str);
//------------------------------------------------------------------------------------------------
#ifdef unix
bool p_dll_Init(const char* dir_work, const char* patch_ini )
#else
bool p_dll_Init(const char* dir_work, const char* patch_ini )
#endif
{
 flagMainApp = 0;
 P_INITREG InitReg;
 READPARAMTOOREG ReadParamTooReg;

 global = 0;
 test_lib = NULL;


 ptch_ini = (char*)patch_ini;
 ptch_work = (char*)dir_work;

 s_ini = "INI: ";
if(ptch_ini != NULL)
    s_ini += ptch_ini;
else
    s_ini += "NULL";

 s_ini += "   WORK";

if(ptch_work != NULL)
 s_work = ptch_work;
else
 s_work = "NULL";



#ifdef unix //LINUX
 string p = dir_work;
 p += "/libp_reg_lib.so";
                                //#ifdef unix //LINUX
    //---------reg_lib-------------------------------------
void*  reg_lib;
            reg_lib = dlopen(p.c_str(),RTLD_LAZY );
            if(!reg_lib)
            {
                if((ptr_e = dlerror()) != NULL)
                    throw EXP_ERROR("p_dll_Init",ptr_e);
            }

            g_reg_GetError = (REG_GETERROR)dlsym(reg_lib,"reg_GetError");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

            InitReg = (P_INITREG)dlsym(reg_lib,"InitReg");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            ReadParamTooReg = (READPARAMTOOREG)dlsym(reg_lib,"ReadParamTooReg");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_DestroyReg = (DESTROYREG)dlsym(reg_lib,"DestroyReg");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);



            p = dir_work;
            p += "/libp_test_dll.so";

    //--------------test_lib--------------------------------------------------------
void*       test_lib = dlopen(/*"libp_test_dll.so"*/p.c_str(),RTLD_LAZY);
            if(!test_lib)
            {
            if((ptr_e = dlerror()) != NULL)
                    throw EXP_ERROR("p_dll_Init",ptr_e);

            }

            g_test_GetError = (TEST_GETERROR )dlsym(test_lib,"test_GetError");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init", ptr_e);

            TEST_DLL_INIT test_dll_Init = (TEST_DLL_INIT)dlsym(test_lib, "test_dll_Init");
            if((ptr_e = dlerror()) != NULL)
            {
                throw EXP_ERROR("p_dll_Init",ptr_e);
            }

            g_test_dll_Show_s_dw = (TEST_DLL_SHOW_S_DW)dlsym(test_lib, "test_dll_Show_s_dw");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_test_dll_Show_s_dw_s = (TEST_DLL_SHOW_S_DW_S)dlsym(test_lib, "test_dll_Show_s_dw_s");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_test_dll_Show_s_us = (TEST_DLL_SHOW_S_US)dlsym(test_lib, "test_dll_Show_s_us");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

            g_test_dll_Show_s_d = (TEST_DLL_SHOW_S_D)dlsym(test_lib, "test_dll_Show_s_d");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_test_dll_Show_s_bt = (TEST_DLL_SHOW_S_BT)dlsym(test_lib, "test_dll_Show_s_bt");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_test_dll_Show_s_dw_s_dw = (TEST_DLL_SHOW_S_DW_S_DW)dlsym(test_lib, "test_dll_Show_s_dw_s_dw");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_test_dll_Show_s_s = (TEST_DLL_SHOW_S_S)dlsym(test_lib, "test_dll_Show_s_s");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_test_dll_Close = (TEST_DLL_CLOSE)dlsym(test_lib,"test_dll_Close");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            p = dir_work;
            p += "/libp_dll_adapter.so";


        //--------------adapter_lib--------------------------------------------------------
void* adapter_lib = dlopen(/*"libp_dll_adapter.so"*/p.c_str(),RTLD_LAZY);
            if(!adapter_lib)
            {
                if((ptr_e = dlerror()) !=  NULL)
                    throw EXP_ERROR("p_dll_Init",ptr_e);
            }

            g_adapter_GetError = (ADAPTER_GETERROR)dlsym(adapter_lib,"adapter_GetError");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

            ADAPTER_INIT adapter_Init = (ADAPTER_INIT)dlsym(adapter_lib, "adapter_Init");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_adapter_Send = (ADAPTER_SEND)dlsym(adapter_lib, "adapter_Send");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_adapter_ResiveBuffer = (ADAPTER_RESIVEBUFFER)dlsym(adapter_lib, "adapter_ResiveBuffer");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_adapter_GetChError = (ADAPTER_GETCHERROR)dlsym(adapter_lib, "adapter_GetChError");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_adapter_Delete = (ADAPTER_DELETE)dlsym(adapter_lib, "adapter_Delete");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

            g_adapter_SetPixelPerString = (ADAPTER_SETPIXELPERSTRING)dlsym(adapter_lib,"adapter_SetPixelPerString");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

            g_adapter_SendIcmp = (ADAPTER_SEND_ICMP)dlsym(adapter_lib,"adapter_SendIcmp");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);



//----------------------------------------------------------
            p =dir_work;
            p += "/libp_dll_reduct.so";
            //rduc_lib
void*  rduc_lib = dlopen(/*"libp_dll_reduct.so"*/p.c_str(),RTLD_LAZY);
            if(!rduc_lib)
            {
                if((rduc_lib = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

            }

            g_rduc_GetError = (RDUC_GETERROR)dlsym(rduc_lib,"rduc_GetError");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

            g_rduc_Init = (RDUC_INIT)dlsym(rduc_lib,"rduc_Init");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);



            g_rduc_GetPtrForMidleBuferIn = (RDUC_GETPTRFORMIDLRBUFERIN)dlsym(rduc_lib,"rduc_GetPtrForMidleBuferIn");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_rduc_GetPtrForMidleBuferOut = (RDUC_GETPTRFORMIDLRBUFEROUT)dlsym(rduc_lib,"rduc_GetPtrForMidleBuferOut");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_rduc_SetData = (RDUC_SETDATA)dlsym(rduc_lib,"rduc_SetData");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);;


            g_rduc_GetData = (RDUC_GETDATA)dlsym(rduc_lib,"rduc_GetData");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);



            g_rduc_GetMideleForBuferInFromADC = (RDUC_GETMIDELEFORBUFERINFROMADC)
                                                dlsym(rduc_lib,"rduc_GetMideleForBuferInFromADC");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);


            g_rduc_GetMideleForBuferOutFromADC = (RDUC_GETMIDELEFORBUFEROUTFROMADC)
                                                dlsym(rduc_lib,"rduc_GetMideleForBuferOutFromADC");
            if((ptr_e = dlerror()) != NULL)
                throw EXP_ERROR("p_dll_Init",ptr_e);

#ifdef USE_REDUCT_TEST
            g_rduc_GetClassPtr = (RDUC_GETCLASSPTR)
                                                dlsym(rduc_lib,"rduc_GetClassPtr");
             if((ptr_e = dlerror()) != NULL)
                  throw EXP_ERROR("p_dll_Init", ptr_e);


            g_rduc_SetBuferOfNonReducData = (RDUC_SETBUFEROFNONREDUCDATA)
                                                dlsym(rduc_lib,"rduc_SetBuferOfNonReducData");
             if((ptr_e = dlerror()) != NULL)
                  throw EXP_ERROR("p_dll_Init", ptr_e);


            g_rduc_SetNposstringnoise = (RDUC_SETNPOSSTRINGNOISE)
                                                dlsym(rduc_lib,"rduc_SetNposstringnoise");
            if((ptr_e = dlerror()) != NULL)
                  throw EXP_ERROR("p_dll_Init", ptr_e);
#endif


#else //WINDOWS
 DWORD e;
            //reg
HMODULE  reg_lib = LoadLibrary(TEXT("p_reg_lib"));
            if(!reg_lib)
            {
               e = GetLastError();
               throw EXP_ERROR("p_dll_Init","can not find reg_lib", e);
            }

            g_reg_GetError = (REG_GETERROR)GetProcAddress(reg_lib,"reg_GetError");
            if(!g_reg_GetError)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find reg_GetError", e);
            }

            InitReg = (P_INITREG)GetProcAddress(reg_lib,"InitReg");
            if(!InitReg)
            {
               e = GetLastError();
               throw EXP_ERROR("p_dll_Init","can not find InitReg", e);
            }

            ReadParamTooReg = (READPARAMTOOREG)GetProcAddress(reg_lib,"ReadParamTooReg");
            if(!ReadParamTooReg)
            {
               e = GetLastError();
               throw EXP_ERROR("p_dll_Init","can not find ReadParamTooReg", e);
            }


            g_WriteParamTooReg = (WRITEPARAMTOOREG)GetProcAddress(reg_lib,"WriteParamTooReg");
            if(!g_WriteParamTooReg)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find WriteParamTooReg", e);
            }


            g_DestroyReg = (DESTROYREG)GetProcAddress(reg_lib,"DestroyReg");
            if(!g_DestroyReg)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find DestroyReg", e);
            }





//--------------test_lib--------------------------------------------------------
 /*HMODULE*/  test_lib = LoadLibrary(TEXT("p_test_dll"));
            if(!test_lib)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_lib", e);
            }

            g_test_GetError = (TEST_GETERROR)GetProcAddress(test_lib,"test_GetError");
            if(!g_test_GetError)
            {
               e = GetLastError();
                throw EXP_ERROR("p_dll_Init","can not find test_GetError", e);
            }


 TEST_DLL_INIT test_dll_Init = (TEST_DLL_INIT)GetProcAddress(test_lib, "test_dll_Init");
            if(!test_dll_Init)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dll_Init", e);
            }

            g_test_dll_Show_s_dw = (TEST_DLL_SHOW_S_DW)GetProcAddress(test_lib, "test_dll_Show_s_dw");
            if(!g_test_dll_Show_s_dw)
            {
                e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dllShow_s_dw", e);
            }


            g_test_dll_Show_s_dw_s = (TEST_DLL_SHOW_S_DW_S)GetProcAddress(test_lib,"test_dll_Show_s_dw_s");
            if(!g_test_dll_Show_s_dw_s)
            {
                e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dllShow_s_dw_s", e);
            }

            g_test_dll_Show_s_us = (TEST_DLL_SHOW_S_US)GetProcAddress(test_lib, "test_dll_Show_s_us");
            if(!g_test_dll_Show_s_us)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dllShow_s_us", e);
            }


            g_test_dll_Show_s_d = (TEST_DLL_SHOW_S_D)GetProcAddress(test_lib, "test_dll_Show_s_d");
            if(!g_test_dll_Show_s_d)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dllShow_s_d", e);
            }

            g_test_dll_Show_s_bt = (TEST_DLL_SHOW_S_BT)GetProcAddress(test_lib,"test_dll_Show_s_bt");
            if(!g_test_dll_Show_s_bt)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dllShow_s_bt", e);
            }


            g_test_dll_Show_s_dw_s_dw = (TEST_DLL_SHOW_S_DW_S_DW)GetProcAddress(test_lib,"test_dll_Show_s_dw_s_dw");
            if(!g_test_dll_Show_s_dw_s_dw)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dllShow_s_dw_s_dw", e);
            }


            g_test_dll_Show_s_s = (TEST_DLL_SHOW_S_S)GetProcAddress(test_lib,"test_dll_Show_s_s");
            if(!g_test_dll_Show_s_s)
            {
               e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_dllShow_s_s", e);
            }


            g_test_dll_Close = (TEST_DLL_CLOSE)GetProcAddress(test_lib,"test_dll_Close");
            if(!g_test_dll_Close)
            {
               e = GetLastError();
                throw EXP_ERROR("p_dll_Init","can not find test_dll_Close", e);
            }




//--------------adapter_lib--------------------------------------------------------
 HMODULE    adapter_lib = LoadLibrary(TEXT("p_dll_adapter"));
            if(!adapter_lib)
            {
                     e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find adapter_lib", e);
            }

            g_adapter_GetError = (ADAPTER_GETERROR)GetProcAddress(adapter_lib,"adapter_GetError");
            if(!g_adapter_GetError)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","g_adapter_GetError", e);
            }

 ADAPTER_INIT adapter_Init = (ADAPTER_INIT)GetProcAddress(adapter_lib, "adapter_Init");
            if(!adapter_Init)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find adapter_Init", e);
            }



            g_adapter_Send = (ADAPTER_SEND)GetProcAddress(adapter_lib, "adapter_Send");
            if(!g_adapter_Send)
            {
                 e = GetLastError();
               throw EXP_ERROR("p_dll_Init","can not find adapter_Send", e);
            }

            g_adapter_ResiveBuffer = (ADAPTER_RESIVEBUFFER)GetProcAddress(adapter_lib, "adapter_ResiveBuffer");
            if(!g_adapter_ResiveBuffer)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find adapter_ResiveBuffer", e);
            }

            g_adapter_Delete = (ADAPTER_DELETE)GetProcAddress(adapter_lib, "adapter_Delete");
            if(!g_adapter_Delete)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find adapter_Delete", e);
            }


            g_adapter_GetChError = (ADAPTER_GETCHERROR)GetProcAddress(adapter_lib, "adapter_GetChError");
            if(!g_adapter_GetChError)
            {
                 e = GetLastError();
                throw EXP_ERROR("p_dll_Init","can not find adapter_GetChError", e);
            }


            g_adapter_SetPixelPerString = (ADAPTER_SETPIXELPERSTRING)GetProcAddress(adapter_lib,"adapter_SetPixelPerString");
            if(!g_adapter_SetPixelPerString)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find adapter_SetPixelPerString", e);
            }


            g_adapter_SendIcmp = (ADAPTER_SEND_ICMP)GetProcAddress(adapter_lib,"adapter_SendIcmp");
            if(!g_adapter_SendIcmp)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","adapter_SendIcmp", e);
            }

//--------------------------------
 //rduc_lib
 HMODULE rduc_lib = LoadLibrary(TEXT("p_dll_reduct.dll"));
            if(!rduc_lib)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_lib", e);
            }

            g_rduc_GetError = (RDUC_GETERROR)GetProcAddress(rduc_lib,"rduc_GetError");
            if(!g_rduc_GetError)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find test_GetError", e);
            }


            g_rduc_Init = (RDUC_INIT)GetProcAddress(rduc_lib,"rduc_Init");
            if(!g_rduc_Init)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_Init", e);
            }

            g_rduc_GetPtrForMidleBuferIn = (RDUC_GETPTRFORMIDLRBUFERIN)GetProcAddress(rduc_lib,"rduc_GetPtrForMidleBuferIn");
            if(!g_rduc_GetPtrForMidleBuferIn)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_GetPtrForMidleBuferIn", e);
            }

            g_rduc_GetPtrForMidleBuferOut = (RDUC_GETPTRFORMIDLRBUFEROUT)GetProcAddress(rduc_lib,"rduc_GetPtrForMidleBuferOut");
            if(!g_rduc_GetPtrForMidleBuferOut)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_GetPtrForMidleBuferOut", e);
            }

            g_rduc_SetData = (RDUC_SETDATA)GetProcAddress(rduc_lib,"rduc_SetData");
            if(!g_rduc_SetData)
            {
                 e = GetLastError();
                throw EXP_ERROR("p_dll_Init","can not find rduc_SetData", e);
            }

            g_rduc_GetData = (RDUC_GETDATA)GetProcAddress(rduc_lib,"rduc_GetData");
            if(!g_rduc_GetData)
            {
                 e = GetLastError();
                 throw EXP_ERROR("p_dll_Init","can not find rduc_GetData", e);
            }

            g_rduc_GetMideleForBuferInFromADC = (RDUC_GETMIDELEFORBUFERINFROMADC)
                                                GetProcAddress(rduc_lib,"rduc_GetMideleForBuferInFromADC");
            if(!g_rduc_GetMideleForBuferInFromADC)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_GetMideleForBuferInFromADC", e);
            }

            g_rduc_GetMideleForBuferOutFromADC = (RDUC_GETMIDELEFORBUFEROUTFROMADC)
                                                GetProcAddress(rduc_lib,"rduc_GetMideleForBuferOutFromADC");
            if(!g_rduc_GetMideleForBuferOutFromADC)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_GetMideleForBuferOutFromADC", e);
            }



#ifdef USE_REDUCT_TEST
            g_rduc_GetClassPtr = (RDUC_GETCLASSPTR)
                                                GetProcAddress(rduc_lib,"rduc_GetClassPtr");
            if(!g_rduc_GetClassPtr)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_GetClassPtr", e);
            }


            g_rduc_SetBuferOfNonReducData = (RDUC_SETBUFEROFNONREDUCDATA)
                                                GetProcAddress(rduc_lib,"rduc_SetBuferOfNonReducData");
            if(!g_rduc_SetBuferOfNonReducData)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_SetBuferOfNonReucData", e);
            }

            g_rduc_SetNposstringnoise = (RDUC_SETNPOSSTRINGNOISE)
                                                GetProcAddress(rduc_lib,"rduc_SetNposstringnoise");
            if(!g_rduc_SetNposstringnoise)
            {
                 e = GetLastError();
                  throw EXP_ERROR("p_dll_Init","can not find rduc_SetNposstring_noise", e);
            }


#endif //#ifdef USE_REDUCT_TEST
#endif

#ifdef unix
            reg = InitReg(RegKey, reg_str_resurs_param_name, 27, defstr, "./s_dll_udp_init");
            if(reg == NULL)
                      return 1;
#else
wchar_t w_buf[MAX_PATH];
            wmemset(w_buf,L'0',MAX_PATH);
            if(patch_ini == NULL)
            {
                    if(!GetModuleFileName(NULL, w_buf, MAX_PATH))
                            throw EXP_ERROR("p_dll_Init","GetModuleFileName - Can' not resolve m_name");
                    wchar_t* n = wcsrchr(w_buf, L'\\');
                    wcscpy(n, TEXT("\\p_dll_udp_init.ini"));
            }
            else
            {
                    if(AnsiToUnicode(patch_ini, (wchar_t*)w_buf))
                         throw EXP_ERROR("p_dll_Init","can not exec AnsiToUnicode");
                    wcscat(w_buf,TEXT("\\p_dll_udp_init.ini"));
            }

            reg = InitReg(RegKey, reg_str_resurs_param_name, 27, defstr, w_buf);
            if(reg == NULL)
                return 1;
#endif//Windows

            regstring = ReadParamTooReg(reg);
            if(regstring ==  NULL)
                return 1;

            regdata = p_dll_src_FaseReg(regstring);
            if(regdata == NULL)
                return 1;
#ifndef   unix
            if(test_dll_Init((char*)"C:\\Users\\Public\\", If_log_debug))
                    return 1;

#else   //unix
char m_name[1024];
            memset(m_name,0,1024);
char *ptr = (char*)getenv("HOME");
int num;
            if(strlen(ptr) > 1023)
                        num = 1023;
            else
                        num = strlen(ptr);
            memcpy(m_name, ptr,num);
            if(test_dll_Init(m_name, If_log_debug))
                    return 1;
#endif  //unix
/*
wchar_t w_out[2048];
            AnsiToUnicode(LPCSTR(dir_work), w_out);

            MessageBox(NULL,w_out,TEXT("TEST"), MB_OK);
*/
            if(p_dll_tif_bufer_Init(dir_work))
                 return 1;
            if(dir_work == NULL)
            {
            if(adapter_Init(NULL, If_svaz_udp, regdata->timer_repit, Iz_pxs_per_string, Iz_icmp_test ))
                return 1;
            }
            else
            {
            string p = dir_work;
            if(adapter_Init(&p, If_svaz_udp, regdata->timer_repit, Iz_pxs_per_string, Iz_icmp_test ))
                    return 1;
            }
TEST_DLL_SHOW_S_S  test_dll_Show_s_s = g_test_dll_Show_s_s;
            test_dll_Show_s_s ((char*)s_ini.c_str(), (char*)s_work.c_str(), 0);
return 0;
}

//--------------------------------------------------------------------------------------------
//Инициализация структуры REGDATA  параметрами инициализации, прочитаными из файла *ini
//      аргументы:
//      REGSTRING *r_str - строка текстовых значений параметров инициализации *.ini файла, которая возвращается
//   функцией -REGSTRING* ReadParamTooReg(TRegistry *reg);
// возвращаемое значение:
//              при отсутствии ошибок- указатель на структуру REGDATA*
//              - NULL- при наличии ошибок
// коментарии:
//      функция - выбрасывает исключение:
//                      - при не возможности выделить память под структуру REGDATA
//------------------------------------------------------------------------------------------------
REGDATA*  p_dll_src_FaseReg(REGSTRING *r_str)
{
 WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
 REGDATA *d = new REGDATA;

 if (d == NULL)
 {
     throw EXP_ERROR("p_dll_src_FaseReg","Can not allocate memory");
 }
                        memset(d,0,sizeof(REGDATA));
                        d->timer_period = atoi(r_str[0].c_str());
                        d->pixsels_per_string =  atoi(r_str[1].c_str());
                        d->save_in_bufer_in_file_or_read_it  = atoi(r_str[2].c_str());
                        d->_5ms_per_string  = atoi(r_str[3].c_str());
                        d->iz_log_debug  = atoi(r_str[4].c_str());
                        d->iz_reduction  = atoi(r_str[5].c_str()) == 0 ? 0 : 1;
                        d->iz_svaz_udp  = atoi(r_str[6].c_str())== 0 ? 0 : 1;
                        d->mtr_gane  = atoi(r_str[7].c_str());
                        d->in_file_off_byffer  = atoi(r_str[8].c_str())== 0 ? 0 : 1;
                        d->l_front_reduct = atoi(r_str[9].c_str());
                        d->r_front_reduct  = atoi(r_str[10].c_str());
                        d->if_rotation  = atoi(r_str[11].c_str())== 0 ? 0 : 1;
                        d->with_noise_statistics  = atoi(r_str[12].c_str())== 0 ? 0 : 1;
                        d->with_matrices  = atoi(r_str[13].c_str())== 0 ? 0 : 1;
                        d->timer_repit = atof(r_str[14].c_str());
                        d->if_revers_matrix  = atoi(r_str[15].c_str()) == 0? 0 : 1;
                        d->iz_dac  = atoi(r_str[16].c_str());
                        d->iz_binning  = atoi(r_str[17].c_str());
                        d->iz_h_filter  = atoi(r_str[18].c_str()) == 0? 0 : 1;
                        d->div_4_3_2  = atoi(r_str[20].c_str()) == 0? 0 : 1;
                        d->iz_range  = atoi(r_str[21].c_str()) == 0? 0 : 1;
                        d->range  = atoi(r_str[22].c_str());
                        d->iz_tif  = atoi(r_str[23].c_str()) == 0? 0 : 1;
                        d->iz_icmp_test = atoi(r_str[24].c_str()) == 0? 0 : 1;
                        d->iz_time_per_string  = atoi(r_str[25].c_str());
                        d->iz_s_filter  = atoi(r_str[26].c_str());
                    if( d->timer_period < 7)
                    {
                        d->timer_period = 7;
                    char b[20];
                        memset(b, 0, 20);
                        sprintf(b,"%d",d->timer_period);
                    string as(b);
                        if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_TIMER_PERIOD], (char*)as.c_str()))
                        {
                            return NULL;
                        }
                    }

    return d;
}

//--------------------------------------------------------------------------------------------
//Вызывается после инициализации и установления связи с датчиком
//      аргументы: - нет
// возвращаемое значение:
//              -0- при отсутствии ошибок
//              -1- при наличии ошибок
// коментарии: - После вызова функции, вызов функции DWORD p_dll_GetError(char**) - покажет наличие ошибок
//------------------------------------------------------------------------------------------------
bool  AfteInitOnCreate()
{
   //  MessageBox(NULL,NULL,TEXT("IN AftOnCr"), MB_OK);
 WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
 RDUC_INIT rduc_Init = g_rduc_Init;
 ADAPTER_SETPIXELPERSTRING adapter_SetPixelPerString = g_adapter_SetPixelPerString;

 if(If_svaz_udp == 0)
 {
               svaz.Init(Iz_binning);
               if(p_dll_GetErrorDWord())
                   return 1;
 }

 char    buf_p[10];
       memset((void*)bufer,0,sizeof(bufer));
       memset((void*)buf_p,0,sizeof(buf_p));


 DWORD len;


 char *ptr;
       if(!(ptr = p_dll_src_Get_Version_From_Datchic((DWORD*)&len)))
           return 1;
       strcpy(bufer, ptr);

int i;
       for(i = 0; i < (int)len; i++)
       {
               if(bufer[i] == 0x20)
               {
                       memcpy(buf_p,bufer,i);
                       buf_p[i] = 0x00;
                       Iz_pxs_per_string = atoi(buf_p);
                       memset(Version, 0, sizeof(Version));
                       if(Iz_pxs_per_string  == 576)
                               strcpy(Version, (char*)bufer);
                       else
                       {
                              strcpy(Version, (char*)&bufer[i+1]);
                              if(Iz_pxs_per_string  == 2048)
                              {
                                    regdata->iz_binning = 1;
                                    if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_IZ_BINNING],(char*)"1"))
                                        return 1;
                              }
                       }
               string s;
                       s = buf_p;
                       if(WriteParamTooReg(reg ,reg_str_resurs_param_name[EN_PIXELS_PER_STRING], (char*)s.c_str()))
                           return 1;
                       adapter_SetPixelPerString(Iz_pxs_per_string);
                       break;
               }
       }
        if(rduc_Init(Iz_pxs_per_string, Iz_l_front_reduct, Iz_r_front_reduct, Iz_binning))
            if(p_dll_GetErrorDWord())
                return 1;


        p_dll_SetModeRun((MODE)(If_with_matrices == 0));
        if(p_dll_GetErrorDWord())
            return 1;

        p_dll_PutGaneMtr(Iz_mtr_gane);
        if(p_dll_GetErrorDWord())
            return 1;
   //  MessageBox(NULL,NULL,TEXT("OUT AftOnCr"), MB_OK);
        p_dll_PutTimePerString((TIMEPERSTRING) Iz_time_per_string);
return 0;
}

//--------------------------------------------------------------------------------------------
//Возвращает приложению текстовую строку о версии dll и прошивок датчика
//      аргументы: - нет
// возвращаемое значение:
//              -указатель на текстовую строку
// коментарии: - нет
//------------------------------------------------------------------------------------------------
char* p_dll_GetVersionDll()
{
static char t[1024];
string s = str_dll_version;
        s +=  Version_of_dll;
        s +=  str_version_of_sensor;
        s += bufer;
        memset(t , 0, 1024);

int num;
        num = s.length() > 1024 ? 1024 : s.length();
        memcpy(t, s.c_str(),num);

        for(WORD i = 0; i < num; i++)
        {
            if(!isalnum(t[i]) && (t[i] != ',') && (t[i] != ':') && (t[i] != '.') && (t[i] != 0))
                t[i] = ' ';
        }


return (char*)t;
}

//--------------------------------------------------------------------------------------------
//Возвращает dll текстовую строку прошивок датчика
//      аргументы: - указатель на двойное слово куда записывается длина принятой строки
// возвращаемое значение:
//              -при успешной работе возвращается указатель на текстовую строку прошивок датчика
//              -при ошибке - NULL
// коментарии: - нет
//------------------------------------------------------------------------------------------------
char*  p_dll_src_Get_Version_From_Datchic(DWORD *num)
{
DWORD len;
static char *ptr;

ADAPTER_SEND adapter_Send = g_adapter_Send;
ADAPTER_RESIVEBUFFER adapter_ResiveBuffer = g_adapter_ResiveBuffer;

        string data("MC_GET_VERSION" );
        if(If_svaz_udp == 1)
        {
                if(adapter_Send((char*)data.c_str()))
                    return NULL;
                if(!(len =   (WORD)adapter_ResiveBuffer((BYTE**)&ptr, 30)))
                    return NULL;
        }
        else
        {
                if((ptr =  (char*)svaz.Send((char*)data.c_str())))
                    return NULL;
                if(!(len =  svaz.ReceiveBuffer((BYTE**)&ptr)))
                    return NULL;
        }
         *num = len;
        return ptr;
}


//--------------------------------------------------------------------------------------------
//Устанавливает на датчике режим работы RUN - TEST
//      аргументы: - MODE mode - режим работы
//  возвращаемое значение: - нет
//  коментарии: - после вызова этой функции вызов DWORD p_dll_GetError(), определяет
//  наличие или отсутствие ошибок
//------------------------------------------------------------------------------------------------
void  p_dll_SetModeRun(MODE mode)
{
WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
TEST_DLL_SHOW_S_DW test_dll_Show_s_dw = g_test_dll_Show_s_dw;
ADAPTER_SEND adapter_Send = g_adapter_Send;

string data("MC_MODE ");
if(mode == RUN)
        {
                If_with_matrices = 1;
                if(test_dll_Show_s_dw("mode: Run with matrices ",0,0))
                    return;
                data += ("R ");
        }
        else
        if(mode == TEST)
        {
                If_with_matrices = 0;
                if(test_dll_Show_s_dw("mode: Run without matrix ", 1, 0))
                    return;
                data += ("T ");
        }
string s =  If_with_matrices == 0 ? "0":"1";
        if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_WITH_MATRICES], (char*)s.c_str()))
         return;

        if(If_svaz_udp == 1)
        {
                if(adapter_Send((char*)data.c_str()))
                    return;
        }
        else
        {
                if(svaz.Send((char*)data.c_str()))
                return;
        }
}


//--------------------------------------------------------------------------------------------
//Устанавливает на датчике коеффициет усиления матриц
//      аргументы: - BYTE gane - режим работы
//  возвращаемое значение: - нет
//  коментарии: - после вызова этой функции вызов DWORD p_dll_GetError(), определяет
//  наличие или отсутствие ошибок
//------------------------------------------------------------------------------------------------
void p_dll_PutGaneMtr(BYTE gane)
{
WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
ADAPTER_SEND adapter_Send = g_adapter_Send;
string data("GANE_MTR ");
        data += "G ";
char b[3];
        memset(b,0,3);
        sprintf(b, "%d",gane);
        data += b;
        if(If_svaz_udp == 1)
        {
                if(adapter_Send((char*)data.c_str()))
                    return;
        }
        else
        {
                if(!svaz.Send((char*)data.c_str()))
                    return;
        }
        Iz_mtr_gane = gane;
string s = b;
//::Sleep(2000);
        if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_MTR_GANE], (char*)s.c_str()))
            return;
}
//--------------------------------------------------------------------------------------------
//      Функция реализует медианный фильтр для строк, используемых при сведении усилителей датчика
//      аргументы: -
//            WORD *scc - буфер для обработки фильтром
//            unsigned long len  - длина обрабатываемого буфера в пикселях
//            WORD aprt - аппертура фильтра
//  возвращаемое значение:
//              - 0 при отсутствии ошибок
//              - 1 при наличии ошибок
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
bool p_dll_src_MedianFiltrHorz1(WORD *scc,  unsigned long len ,WORD aprt)
{
WORD *sc = scc ;
        if((aprt % 2) == 0)
            return 1;
int med = aprt/2;
vector<WORD> v_sort;

WORD *end = (WORD*)sc + len/2;
WORD m = Iz_pxs_per_string == 2048 ? 8 : 9;
WORD *wtr;
int i;
        for(i = 0; i < m; i++)
        {
                wtr = sc + i;

                for( ; wtr < end ; )
                {
                 WORD t;
                 int j;
                        for(j = 0; j < aprt; j++)
                        {
                            t = *(wtr + j*m);
                            v_sort.push_back(t);
                        }
                        sort(v_sort.begin(), v_sort.end());
                        *(WORD*)(wtr + med * m ) = v_sort[med];
                        v_sort.erase(v_sort.begin(),v_sort.end());
                        wtr += m;
                 }
        }
return 0;
}

//--------------------------------------------------------------------------------------------
//Функция меняет местами пиксели каждой матрицы
//      аргументы: -
//            WORD *wtr - буфер для обмена
//            DWORD len  - длина буфера в пикселях
//  возвращаемое значение:
//              -указатель на буфер
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
WORD* s_dll_src_RotationHor(WORD *wtr, DWORD len)
{
WORD *wWtr = wtr;
WORD num_adc;
int ex, x;
num_adc = 9;
WORD num_str = (WORD)(len/Iz_pxs_per_string);
WORD *beg,*end;
        for(WORD y = 0; y < num_str; y++)
        {
         int n,  en;
                for(n = 0, en = num_adc - 1; n  < num_adc; n++, en--)
                {
                        for(x = 0, ex = 64; x < 32; x++, ex--)
                        {
                                beg = wWtr + y * Iz_pxs_per_string + x * num_adc    + n;
                                end = wWtr + y * Iz_pxs_per_string + ex * num_adc   + en;
                         WORD t = *beg;
                                *beg = *end;
                                *end =  t;
                        }
                }
        }
return wWtr;
}

//--------------------------------------------------------------------------------------------
//Функция изменяет пиксели в буфере в соответствии с бинингом
//      аргументы: -
//           WORD **buffer - указатель на указатель буфера
//           unsigned long len - длина буфера
//           BYTE binning - бининг датчика
//  возвращаемое значение:
//              -нет
//  коментарии:
//           функция переставляет пиксели в буфере с "матричного" в "линейное" и
//      складывает их в соответствии с бинингом
//           функция генерирует исключения, если
//      -она не может выделить память для буфера при сложении пиксели в соответствии с бинингом
//      - если бининг не действителен для данного типа датчика
//      - последующий вызов p_dll_GetError()- установит наличие ошибок
//------------------------------------------------------------------------------------------------
void p_dll_src_SetBinningBuffer(WORD **buffer, unsigned long len, BYTE binning)
{
 WORD *wtr;
 DWORD dwnum;
                 switch(binning)
                 {
                        case 1:
                     if(Iz_pxs_per_string == 2304)
                     {
                         wtr = p_dll_src_SetBinning_1_Bufer2304(*buffer ,len, &dwnum);
                         if(wtr == NULL)
                         {
                              throw EXP_ERROR("p_dll_src_SetBinningBuffer ", "Error p_dll_src_SetBinning_1_Bufer2304");
                              return;
                         }
                              break;

                     }
                     else
                                if(Iz_pxs_per_string == 2048)
                                {
                                    wtr = p_dll_src_SetBinning_1_Bufer2048(*buffer ,len, &dwnum);
                                    if(wtr == NULL)
                                    {
                                         throw EXP_ERROR("p_dll_src_SetBinningBuffer ", " p_dll_src_SetBinning_1_Bufer2048");
                                         return;
                                    }
                                    break;
                                }
                                else
                                    if(Iz_pxs_per_string == 576)
                                    {
                                        wtr = p_dll_src_SetBinning_1_Bufer576(*buffer ,len, &dwnum);
                                        if(wtr == NULL)
                                        {
                                             throw EXP_ERROR("p_dll_src_SetBinningBuffer ", " p_dll_src_SetBinning_1_Bufer576");
                                             return;
                                        }
                                        break;
                                    }
                                    else
                                    {
                                          if(Iz_pxs_per_string == 1152)
                                          {
                                    //        MessageBox(NULL,NULL,TEXT("In f. set binning"), MB_OK);
                                            wtr = p_dll_src_SetBinning_1_Bufer1152(*buffer ,len, &dwnum);
                                           }break;
                                    }


                        case 2:
                                 if(Iz_pxs_per_string == 576)
                                 {
                                   wtr = p_dll_src_SetBinning_2_Bufer576(*buffer ,len, &dwnum);
                                   if(wtr == NULL)
                                   {
                                        throw EXP_ERROR("p_dll_src_SetBinningBuffer ", " p_dll_src_SetBinning_2_Bufer576");
                                        return;
                                   }
                                   break;
                                 }
                                 if(Iz_pxs_per_string == 1152)
                                 {
                                  wtr = p_dll_src_SetBinning_2_Bufer1152(*buffer, len, &dwnum);
                                  if(wtr == NULL)
                                  {
                                       throw EXP_ERROR("p_dll_src_SetBinningBuffer ", "p_dll_src_SetBinning_2_Bufer1152");
                                       return;
                                  }
                                 }
                                  break;


                        default:
                                throw EXP_ERROR("p_dll_src_SetBinningBuffer ", "Error argument binning");
                                return;
                 }

WORD  *wctr = (WORD*)*buffer;
              memcpy( wctr, (void*)wtr, dwnum * sizeof(WORD) );

 delete[] wtr;
}

//--------------------------------------------------------------------------------------------
//      функция переставляет пиксели в буфере с "матричного" в "линейное" и т.к. бининг равен при ее
//вызове единицк, сложение пикселей не производится
//      аргументы: -
//           WORD **buffer - указатель на указатель буфера
//           unsigned long len - длина буфера
//           DWORD *num - указатель на новую длину буфера
//  возвращаемое значение:
//              -WORD * указатель на новый буфер изображения
//              - при ошибке возвращается - NULL
//  коментарии:
//           функция переставляет пиксели в буфере с "матричного" в "линейное" и
//      записывает их в новый буфер
//------------------------------------------------------------------------------------------------
WORD* p_dll_src_SetBinning_1_Bufer2304(WORD *buffer,unsigned long len, DWORD *num)
{
WORD *wtr = new WORD[len + 2];
if(wtr == NULL)
    return NULL;
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t = 0;
WORD len_str = (2304);
WORD num_y =   (WORD)(len / len_str );

                for(y = 0; y < num_y; y++)
                {
                          for(m = 0; m < 9; m++)
                          {
                                        for(x = 0; x < 256; x++)
                                        {
                                         t = *(buffer + len_str * y + x * 9 + m);
                                          *(wtr + y * 2304 + m * 256 + x) = t;
                                        }
                          }
                }

                *num = len;
return wtr;
}

//--------------------------------------------------------------------------------------------
//      функция переставляет пиксели в буфере с "матричного" в "линейное" и т.к. бининг равен при ее
//вызове единицк, сложение пикселей не производится
//      аргументы: -
//           WORD **buffer - указатель на указатель буфера
//           unsigned long len - длина буфера
//           DWORD *num - указатель на новую длину буфера
//  возвращаемое значение:
//              -WORD * указатель на новый буфер изображения
//              - при ошибке возвращается - NULL
//  коментарии:
//           функция переставляет пиксели в буфере с "матричного" в "линейное" и
//      записывает их в новый буфер
//------------------------------------------------------------------------------------------------
WORD* p_dll_src_SetBinning_1_Bufer2048(WORD *buffer,unsigned long len, DWORD *num)
{
WORD *wtr = new WORD[len + 2];
if(wtr == NULL)
    return NULL;
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t = 0;
WORD len_str = (2048);
WORD num_y =   (WORD)(len / len_str );

                for(y = 0; y < num_y; y++)
                {
                          for(m = 0; m < 8; m++)
                          {
                                        for(x = 0; x < 256; x++)
                                        {
                                         t = *(buffer + len_str * y + x * 8 + m);
                                          *(wtr + y * 2048 + m * 256 + x) = t;
                                        }
                          }
                }

                *num = len;
return wtr;
}
//--------------------------------------------------------------------------------------------
//      функция переставляет пиксели в буфере с "матричного" в "линейное" и т.к. бининг равен при ее
//вызове единицк, сложение пикселей не производится
//      аргументы: -
//           WORD **buffer - указатель на указатель буфера
//           unsigned long len - длина буфера
//           DWORD *num - указатель на новую длину буфера
//  возвращаемое значение:
//              -WORD * указатель на новый буфер изображения
//              - при ошибке возвращается - NULL
//  коментарии:
//           функция переставляет пиксели в буфере с "матричного" в "линейное" и
//      записывает их в новый буфер
//------------------------------------------------------------------------------------------------
WORD* p_dll_src_SetBinning_1_Bufer576(WORD *buffer,unsigned long len, DWORD *num)
{
WORD *wtr = new WORD[len + 2];
if(wtr == NULL)
    return NULL;
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t = 0;
WORD num_y =   (WORD)(len / 576 );


 MINMAX  mn;
 DWORD delta;
 double kf = 0.0;
 memset(&mn,0,sizeof(MINMAX));
                if(Iz_range == 1)
                {
                        mn = p_dll_tif_bufer_FindMinMax(buffer  + Iz_pxs_per_string * 30, len);
                        delta = mn.w_max - mn.w_min;
                        WORD w = W_range;
                        kf = ((double)delta) / ((double)w) ;
                }
                for(y = 0; y < num_y; y++)
                {
                          for(m = 0; m < 9; m++)
                          {
                                        for(x = 0; x < 64; x++)
                                        {
                                         t = *(buffer + 576 * y + x * 9 + m);
                                                if(Iz_range == 1)
                                                {
                                                        t = t - mn.w_min;
                                                        t = (WORD)((double)t/kf);
                                                }
                                          *(wtr + y * 576 + m * 64 + x) = t;
                                        }
                          }
                }
                *num = len;
return wtr;
}
//--------------------------------------------------------------------------------------------
//      функция переставляет пиксели в буфере с "матричного" в "линейное" и т.к. бининг равен при ее
//вызове единицк, сложение пикселей не производится
//      аргументы: -
//           WORD **buffer - указатель на указатель буфера
//           unsigned long len - длина буфера
//           DWORD *num - указатель на новую длину буфера
//  возвращаемое значение:
//              -WORD * указатель на новый буфер изображения
//              - при ошибке возвращается - NULL
//  коментарии:
//           функция переставляет пиксели в буфере с "матричного" в "линейное" и
//      записывает их в новый буфер
//------------------------------------------------------------------------------------------------
WORD* p_dll_src_SetBinning_1_Bufer1152(WORD *buffer,unsigned long len, DWORD *num)
{
    WORD *wtr = new WORD[len + 2];
    if(wtr == NULL)
        return NULL;
    memset((void*)wtr, 0, len * sizeof(WORD) + 2);
    WORD x = 0;
    WORD y = 0;
    BYTE m = 0;
    WORD t = 0;
    WORD len_str = (1152);
    WORD num_y =   (WORD)(len / len_str );

                    for(y = 0; y < num_y; y++)
                    {
                              for(m = 0; m < 9; m++)
                              {
                                            for(x = 0; x < 128; x++)
                                            {
                                             t = *(buffer + len_str * y + x * 9 + m);
                                              *(wtr + y * 1152 + m * 128 + x) = t;
                                            }
                              }
                    }

                    *num = len;
    return wtr;
}
//--------------------------------------------------------------------------------------------
//      функция переставляет пиксели в буфере с "матричного" в "линейное" и т.к. бининг равен при ее
//вызове двум, производится сложение двух сосодних пикселей
//      аргументы: -
//           WORD **buffer - указатель на указатель буфера
//           unsigned long len - длина буфера
//           DWORD *num - указатель на новую длину буфера
//  возвращаемое значение:
//              -WORD * указатель на новый буфер изображения
//              - при ошибке возвращается - NULL
//  коментарии:
//           функция переставляет пиксели в буфере с "матричного" в "линейное", складывает соседние и
//      записывает их в новый буфер
//------------------------------------------------------------------------------------------------
WORD* p_dll_src_SetBinning_2_Bufer1152(WORD *buffer,unsigned long len, DWORD *num)
{
WORD num_y =   (WORD)(len / 1152 );
WORD *wtr = new WORD[len  + 2];
if(wtr == NULL)
    return NULL;
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t1 = 0;
WORD t2 = 0;
WORD z = 0;
DWORD dw_t;

MINMAX  mn;
DWORD delta;
double kf;
                for(y = 0; y < num_y; y++)
                {
                 z = 0;
                          for(m = 0; m < 9; m++)
                          {
                             bool f = 0;
                                        for(x = 0; x < 128; x++)
                                        {
                                                if(f == 0)
                                                {
                                                        t1 = *(buffer + 1152 * y + x * 9 + m);
                                                }
                                                else
                                                {
                                                        t2 = *(buffer + 1152 * y + x * 9 + m);
                                                        dw_t = t1 + t2;
                                                        if(Iz_div_16_9_4 == 1)
                                                        {
                                                         dw_t = dw_t/4;
                                                        }
                                                        else
                                                        if(Iz_div_4_3_2 == 1)
                                                        {
                                                         dw_t = dw_t/2;
                                                        }

                                                        *(wtr + y * 1152/2 + z++) = (WORD)dw_t;
                                                 }
                                                 f = (f ^ 1);
                                        }
                          }
                }
                if(Iz_range == 1)
                {
                        mn = p_dll_tif_bufer_FindMinMax(wtr  + Iz_pxs_per_string * 30, len - Iz_pxs_per_string * 30);
                        delta = mn.w_max - mn.w_min;
                        WORD w = W_range;
                        kf = ((double)delta) / ((double)w);
                        for(DWORD i = 0; i < len; i++)
                        {
                         WORD w = wtr[i];
                                        w = w - mn.w_min;
                                        w = (WORD)((double)w/kf);
                                        wtr[i] = w;
                        }

                }
                *num = len;
return wtr;
}

//--------------------------------------------------------------------------------------------
//      функция переставляет пиксели в буфере с "матричного" в "линейное" и т.к. бининг равен при ее
//вызове двум, производится сложение двух сосодних пикселей
//      аргументы: -
//           WORD **buffer - указатель на указатель буфера
//           unsigned long len - длина буфера
//           DWORD *num - указатель на новую длину буфера
//  возвращаемое значение:
//              -WORD * указатель на новый буфер изображения
//              - при ошибке возвращается - NULL
//  коментарии:
//           функция переставляет пиксели в буфере с "матричного" в "линейное", складывает соседние и
//      записывает их в новый буфер
//------------------------------------------------------------------------------------------------
WORD* p_dll_src_SetBinning_2_Bufer576(WORD *buffer,unsigned long len, DWORD *num)
{

WORD num_y =   (WORD)(len / 576 );
WORD *wtr = new WORD[len  + 2];
if(wtr == NULL)
    return NULL;
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t1 = 0;
WORD t2 = 0;
WORD z = 0;
DWORD dw_t;

 MINMAX  mn;
 DWORD delta;
 double kf;



                for(y = 0; y < num_y; y++)
                {
                 z = 0;
                          for(m = 0; m < 9; m++)
                          {
                             bool f = 0;
                                        for(x = 0; x < 64; x++)
                                        {
                                                if(f == 0)
                                                {
                                                        t1 = *(buffer + 576 * y + x * 9 + m);
                                                }
                                                else
                                                {

                                                        t2 = *(buffer + 576 * y + x * 9 + m);
                                                        dw_t = t1 + t2;
                                                        if(Iz_div_16_9_4 == 1)
                                                        {
                                                         dw_t = dw_t/4;
                                                        }
                                                        else
                                                        if(Iz_div_4_3_2 == 1)
                                                        {
                                                         dw_t = dw_t/2;
                                                        }

                                                        *(wtr + y * 576/2 + z++) = (WORD)dw_t;
                                                 }
                                                 f = (f ^ 1);
                                        }
                          }
                }

                if(Iz_range == 1)
                {
                        mn = p_dll_tif_bufer_FindMinMax(wtr  + Iz_pxs_per_string * 30, len - Iz_pxs_per_string * 30);
                        delta = mn.w_max - mn.w_min;
                        WORD w = W_range;
                        kf = ((double)delta) / ((double)w) ;

                        for(DWORD i = 0; i < len; i++)
                        {
                         WORD w = wtr[i];
                                        w = w - mn.w_min;
                                        w = (WORD)((double)w/kf);
                                        wtr[i] = w;
                        }

                }
                *num = len/2;
return wtr;
}

//--------------------------------------------------------------------------------------------
//      функция вызавается для получении кадра изображения из датчика
//      аргументы: -
//              BYTE **pt  Указатель на указатель буфера изображения
//              unsigned long ln - длина требуемого кадра в байтах
//              bool disable_set_binning - 0/1 выполнить бинтнг/не выполнять бининг
//  возвращаемое значение:
//              unsigned long - количество принятых байтов, если ошибок не было
//              - 0 - при наличии лшибок, последующий вызов p_dll_GetError(char**) может идентифицировать ошибку
//  коментарии:
//              -нет
//------------------------------------------------------------------------------------------------
unsigned long   DLL_EI GetBufer(BYTE **pt, unsigned long ln, bool disable_set_binning)
{
TEST_DLL_SHOW_S_DW test_dll_Show_s_dw =  g_test_dll_Show_s_dw;
TEST_DLL_SHOW_S_S  test_dll_Show_s_s = g_test_dll_Show_s_s;
TEST_DLL_SHOW_S_BT test_dll_Show_s_bt = g_test_dll_Show_s_bt;
//#ifdef unix
TEST_DLL_SHOW_S_D test_dll_Show_s_d =  g_test_dll_Show_s_d;
//#endif
ADAPTER_SEND adapter_Send = g_adapter_Send;
ADAPTER_RESIVEBUFFER adapter_ResiveBuffer =  g_adapter_ResiveBuffer;

RDUC_GETPTRFORMIDLRBUFERIN rduc_GetPtrForMidleBuferIn = g_rduc_GetPtrForMidleBuferIn;
RDUC_GETPTRFORMIDLRBUFEROUT rduc_GetPtrForMidleBuferOut = g_rduc_GetPtrForMidleBuferOut;
RDUC_GETMIDELEFORBUFERINFROMADC  rduc_GetMideleForBuferInFromADC = g_rduc_GetMideleForBuferInFromADC;
RDUC_GETMIDELEFORBUFEROUTFROMADC rduc_GetMideleForBuferOutFromADC =  g_rduc_GetMideleForBuferOutFromADC;

RDUC_SETDATA rduc_SetData =  g_rduc_SetData;
RDUC_GETDATA rduc_GetData =  g_rduc_GetData;

DWORD num = 0;
#ifndef unix
    DWORD tim;
#endif
DWORD len;
DWORD error = 0;
BYTE *ptr;
DWORD n_str = 0;

        len =  ln;
        if((Iz_pxs_per_string != 2048) && (Iz_pxs_per_string != 2304))
             if(Iz_pxs_per_string == 1152)
             {
                len += Iz_pxs_per_string * 2 * (4 + 2);
             }
            else
                  len += Iz_pxs_per_string * 2 * (16 + 2);
        else
        {
            len += Iz_pxs_per_string * 2 * (4 + 2);
            if(Iz_pxs_per_string == 2048)
            {   //плюс WORD для контрольной суммы
                n_str = len/Iz_pxs_per_string;
                len += n_str * 2;
            }
        }
        if(If_log_debug != 0)
        {
         global++;
         if(test_dll_Show_s_dw("\r\n                 N = ",global,1))
             return 0;
         if(test_dll_Show_s_dw("Req ln: " , ln, 0))
             return 0;
        }

        //p_dll_PutGaneMtr(Iz_mtr_gane);
     //   if(p_dll_GetErrorDWord())
      //      return 0;

        string get = "GET ";
char buf[10];
        memset(buf, 0, 10);
        sprintf(buf,"%ld",len);
        get += buf;

#ifndef unix
clock_t t1, t2;

                t1 = clock();

#else
struct timespec mt1, mt2;
                clock_gettime (CLOCK_REALTIME, &mt1);
#endif
                do
                {
                        if(If_svaz_udp == 1)
                        {
                                if(adapter_Send((char*)get.c_str()))
                                    return 0;
                                if(!(num =    adapter_ResiveBuffer((BYTE**)&ptr, len)))
                                    return 0;
                        }
                        else
                        {
                                if(svaz.Send((char*)get.c_str()))
                                    return 0;
                                if(!(num =     svaz.ReceiveBuffer((BYTE**)&ptr , len)))
                                    return 0;
#ifndef unix
                                DWORD t_num = num/(Iz_pxs_per_string *2);
                                tim =   (DWORD)((double)t_num * 4.0)  * Iz_binning;
                                ptr -= 2;
                                Sleep(tim);
#else
                                sleep(1);
#endif
                        }
                        if(num != len)
                        error++;
                }
                while(num != len);
                if(If_log_debug != 0)
                {
                double tt;
#ifndef unix
                    t2 = clock();
                    tt = (t2 - t1) / CLOCKS_PER_SEC/*CLK_TCK*/;
                    if(test_dll_Show_s_d("Time per image,sek" , tt, 0))
                        return 0;
                    if(num != 0)
                    {
                    DWORD strs = (num / (Iz_pxs_per_string * 2));
                          tt = tt / (((double)strs));
                            if(test_dll_Show_s_d("Time per string,sek" , tt, 0))
                                return 0;
                    }
#else
                    clock_gettime (CLOCK_REALTIME, &mt2);
                    tt = 1000000000.0 *(double)(mt2.tv_sec - mt1.tv_sec) + (double)(mt2.tv_nsec - mt1.tv_nsec);
                    if(test_dll_Show_s_d("Time per image,sek" , (tt/1000000000.0), 0))
                        return 0;
                    if(num != 0)
                    {
                    DWORD strs = (num / (Iz_pxs_per_string * 2));
                          tt = tt / (((double)strs));
                            if(test_dll_Show_s_d("Time per string,sek" , tt/1000000.0, 0))
                                return 0;
                    }
                    if(test_dll_Show_s_dw("Errors: " , error, 0))
                        return 0;
#endif
                }
WORD *wtr;
WORD ofs;
   if(If_rotation == 0)
   {
       if(Iz_pxs_per_string == 2304)
        wtr = (WORD*)ptr + (9);
        else
           if(Iz_pxs_per_string == 1152)
            wtr = (WORD*)(ptr +  4);//2
       else
            wtr = (WORD*)(ptr + 2);
   }
   else
   {
        wtr = (WORD*)(ptr);
   }


        ofs = Iz_pxs_per_string;
        if(If_with_noise_statistics  == 1)
        {
                svaz.SetNoiseData(wtr + ofs,  If_svaz_udp,
                                        Iz_pxs_per_string, If_with_matrices, len/2);
                if(p_dll_GetChError())
                    return 0;
                if(test_dll_Show_s_s((char*)"WITH NOISE STATISTIC" , (char*)"ON", 0))
                    return 0;
        }
        else
        {
                if(test_dll_Show_s_s((char*)"WITH NOISE STATISTIC" , (char*)"OF", 0))
                    return 0;
        }

        if(If_with_noise_statistics  == 1)
        {
                rduc_GetMideleForBuferInFromADC(wtr + ofs, (num/2 - ofs));
        }
#ifdef MEDIAN1
WORD window = regdata->iz_s_filter;
               if(window > 1)
               {
                WORD num_med = 8;
                        if(Iz_pxs_per_string == 2048 || Iz_pxs_per_string == 1152 ||  Iz_pxs_per_string == 2304)
                            num_med = 8;
                        else
                            if(Iz_pxs_per_string == 576 )
                                num_med = 34;
                        if(p_dll_src_MedianFiltrHorz1(wtr + Iz_pxs_per_string  - window/2 ,
                                                      Iz_pxs_per_string * num_med
                                                      + window , window))
                        return 0;
               }
#endif
        if(Iz_reduct == 1)
        {
#ifdef USE_REDUCT_TEST
                g_rduc_SetBuferOfNonReducData(wtr - (Iz_pxs_per_string == 1152 ? 1 : 0) , num/2 );
#endif//#ifdef USE_REDUCT_TEST
                if(rduc_SetData((WORD*)(wtr  +  ofs - (Iz_pxs_per_string == 1152 ? 1 : 0))))
                    return 0;
                if(rduc_GetData((WORD*)(wtr  +  ofs  - (Iz_pxs_per_string == 1152 ? 1 : 0)) , num/2))
                    return 0;
                if(test_dll_Show_s_s((char*)"Reduction" , (char*)"ON", 0))
                    return 0;
        }
        else
                if(test_dll_Show_s_s((char*)"Reduction" , (char*)"OFF", 0))
                    return 0;
        if(Iz_pxs_per_string != 2048)
        {
            if(Iz_pxs_per_string == 2304)
            {
                len -= Iz_pxs_per_string * 2 * (2 + 4);
                len -= n_str * 2;
            }
            else
                if(Iz_pxs_per_string == 576)
                    len -= Iz_pxs_per_string * 2 * (2 + 16);
                else
                    len -= Iz_pxs_per_string * 2 * (2 + 4);
        }
                else
                {
                    len -= Iz_pxs_per_string * 2 * (2 + 4);
                    len -= n_str * 2;
                }

        if(test_dll_Show_s_dw((char*)"Resiv(Bytes)", num, 0))
            return 0;
        if (If_with_noise_statistics == 1)
                rduc_GetMideleForBuferOutFromADC(wtr + ofs,
                                                    (num/2 - ofs ));
        if(If_with_noise_statistics == 1)
        {
        #ifndef unix
                file_SaveIn_statistic_log(TEXT("C:\\Users\\Public\\statistic_log.log"),  rduc_GetPtrForMidleBuferIn(),
                                               rduc_GetPtrForMidleBuferOut(),
                                                flagMainApp
                                          );
        #else
                file_SaveIn_statistic_log("./statistic_log.log", rduc_GetPtrForMidleBuferIn(),
                                           rduc_GetPtrForMidleBuferOut(),
                                          flagMainApp
                                          );
        #endif
        }
        if(If_rotation == 1)
        {
                s_dll_src_RotationHor((WORD*)(wtr + (16 + 2) * Iz_pxs_per_string), (len - (16 + 2) * Iz_pxs_per_string ));                
                if(test_dll_Show_s_s((char*)"if_rotation", (char*)"ON",0))
                    return 0;
        }
        else
                if(test_dll_Show_s_s((char*)"if_rotation", (char*)"OFF",0))
                    return 0;
       if(test_dll_Show_s_dw("REDUKT L_FRONT = ", Iz_l_front_reduct,0))
           return 0;
       if(test_dll_Show_s_dw("REDUKT R_FRONT = ", Iz_r_front_reduct,0))
           return 0;
       if(test_dll_Show_s_bt("GANE = ", (char)Iz_mtr_gane ,0))
           return 0;

       if(Iz_pxs_per_string == 1152)
        wtr = (wtr - 1 );


       if(If_revers_matrix == 1)
       {
             p_dll_src_MatrixRevers((WORD*)wtr , len/2 );
       }



       if(disable_set_binning == 0)  //У Ильи = 1 для ФMC
       {
            WORD *pW = (WORD*)(wtr + (16 + 2) * Iz_pxs_per_string);
            try
            {

                p_dll_src_SetBinningBuffer(&pW , len / 2 , Iz_binning);
            }
            catch(EXP_ERROR &e)
            {
               e.ErrorSetText("p_dll_udp");
               return 0;
            }
                if(test_dll_Show_s_bt("Binning: " , Iz_binning, 0))
                    return 0;
                if(test_dll_Show_s_bt("HFilter: " , Iz_h_filer, 0))
                    return 0;

                *pt = (BYTE*)(wtr + (16 + 2) * Iz_pxs_per_string);
       }
       else
       {

         if(flagMainApp == 0 )
         {
           if(Iz_pxs_per_string == 2304)
           {
               *pt = (BYTE*)(wtr + (4 + 2) * Iz_pxs_per_string);
           }
           else
               if(Iz_pxs_per_string == 2048)
               {
                   *pt = (BYTE*)(wtr + (4 + 2) * Iz_pxs_per_string);
               }
                 else
                    if(Iz_pxs_per_string == 1152)
                    {
                         *pt = (BYTE*)(wtr + (4 + 2) * Iz_pxs_per_string);
                    }
                    else
                        if(Iz_pxs_per_string == 576)
                        {
                          *pt = (BYTE*)(wtr + (16 + 2) * Iz_pxs_per_string);
                        }
          }


       *pt = (BYTE*)wtr;
       }

      if(Iz_tif == 1)
      {
       BYTE *btr = *pt;
              if(disable_set_binning == 0)
                    p_dll_tif_bufer_Set16(btr, len, Iz_binning);
      }


 return len;
}

//--------------------------------------------------------------------------------------------
//      функция вызавается при закрытии APP
//      аргументы:
//              -нет
//  возвращаемое значение:
//              -нет
//  коментарии:
//              -функция закрывает все dll, сокеты, освобождает всю память
//------------------------------------------------------------------------------------------------
 void   p_dll_Close()
 {
DESTROYREG  DestroyReg = g_DestroyReg;
ADAPTER_DELETE adapter_Delete = g_adapter_Delete;
TEST_DLL_CLOSE test_dll_Close = g_test_dll_Close;

        DeleteBufer();
        DestroyReg(reg);
        adapter_Delete();
        p_dll_tif_bufer_Close();
        test_dll_Close();
    delete regdata;
 }


//--------------------------------------------------------------------------------------------
//      функция вызавается при закрытии APP
//      аргументы:
//              -нет
//  возвращаемое значение:
//              -нет
//  коментарии:
//              -функция информирует о закрытии dll
//              -последующий вызов p_dll_GetError()- установит наличие ошибок
//------------------------------------------------------------------------------------------------
void DeleteBufer()
{
TEST_DLL_SHOW_S_DW test_dll_Show_s_dw = g_test_dll_Show_s_dw;
        if(If_log_debug != 0)
                  test_dll_Show_s_dw("\nDeleteBufer - in ",0,1);
}

//------------------------------------------------------------
//      функция возвращает количество пикселей в строке датчика
//      аргументы:
//              -нет
//  возвращаемое значение:
//              -DWORD - количество пикселей в строке датчика
//  коментарии:
//              -нет
//------------------------------------------------------------
WORD p_dll_GetPixelPerString(){return (WORD)Iz_pxs_per_string;}

//--------------------------------------------------------------------------------------------
//Функция возвращает коеффициент усиления матриц, записанный в REGDATA
//      аргументы: - нет
//  возвращаемое значение:
//              BYTE - код коеффициента усиления
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
BYTE p_dll_GetMtrGane(){return (WORD)Iz_mtr_gane;}

//--------------------------------------------------------------------------------------------
//      Функция устанавливает уровень сигнала DAC датчика, передавая
//   этот уровень на датчик, записывая его в структуру REGDATA и файл *.ini
//      аргументы: - WORD i - уровень сигнала DAC датчика
//  возвращаемое значение:
//              BYTE - нет
//  коментарии: - последующий вызов p_dll_GetError(char**)- установит наличие ошибок
//------------------------------------------------------------------------------------------------
void p_dll_SetDac(WORD i)
{
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
    TEST_DLL_SHOW_S_DW test_dll_Show_s_dw = g_test_dll_Show_s_dw;
    ADAPTER_SEND adapter_Send = g_adapter_Send;

    string    data("P_KAMERA_CMD_SET_DAC");
    if(test_dll_Show_s_dw("SET_DAC: ", i,0))
        return;
    data += " ";
char b[20];
    memset(b,0,20);
    sprintf(b,"%d",i);
        data += b;
        data += " ";
        data += "A";
        if(If_svaz_udp == 1)
        {
                if(adapter_Send((char*)data.c_str()))
                    return;
        }
        else
        {
                if(svaz.Send((char*)data.c_str()))
                    return;
#ifdef USE_LAVEL_IN_SVAZ
                svaz.KeyOnLevel(i);
#endif

        }
Iz_dac = i;
    memset(b,0,20);
    sprintf(b, "%d", i);
    string dac(b);
    WriteParamTooReg(reg, reg_str_resurs_param_name[EN_IZ_DAC], (char*)dac.c_str());//все равно выходим
}
//--------------------------------------------------------------------------------------------
//Функция записывает в структуру REGDATA и в файл ini состояние бита reduct
//      аргументы: - bool reg
//  возвращаемое значение:
//              - нет
//  коментарии: - последующий вызов p_dll_GetError()- установит наличие ошибок
//------------------------------------------------------------------------------------------------
void   p_dll_Reduct_set(bool reduct)
{

    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
        Iz_reduct = reduct;
        string s =  reduct == 0 ? "0":"1";
        if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_IZ_REDUCT], (char*)s.c_str()))
            return;
}

//--------------------------------------------------------------------------------------------
//Функция возвращает указатель на структуру REGDATA*
//      аргументы: нет
//  возвращаемое значение:
//              - REGDATA* -указатель на структуру с параметрами инициализации
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
REGDATA* p_dll_GetParamRegData()
{
        return regdata;
}

//--------------------------------------------------------------------------------------------
//      Функция устанавливает бининг датчика, передавая
//   этот уровень на датчик, записывая его в структуру REGDATA и файл *.ini
//      аргументы: - WORD w - бининг датчика
//  возвращаемое значение:
//               - нет
//  коментарии: - при установке нового бининга производится переинициализация rduc dll, вызовом
//      - функции bool rduc_Init(WORD pix_off_str, WORD lf, WORD rg, BYTE bn);
//      - последующий вызов p_dll_GetError()- установит наличие ошибок
//------------------------------------------------------------------------------------------------
void     p_dll_SendBinning(WORD w)
{

WRITEPARAMTOOREG    WriteParamTooReg =      g_WriteParamTooReg;
TEST_DLL_SHOW_S_DW  test_dll_Show_s_dw =    g_test_dll_Show_s_dw;
TEST_DLL_SHOW_S_S   test_dll_Show_s_s =     g_test_dll_Show_s_s;
RDUC_INIT rduc_Init = g_rduc_Init;
ADAPTER_SEND adapter_Send = g_adapter_Send;

        if(test_dll_Show_s_dw("\r\n  In dll_src_SendBinning: " , w,0))
            return;
        if(If_svaz_udp == 0)
        {
                svaz.Init( Iz_binning);
        }
        if((w < 1) || (w > 2))
        {
                 test_dll_Show_s_s((char*)"ERROR  in s_dll_src_SetBinning",
                                               (char*)"ERROR of Range",0);
                 return;
        }

        if(rduc_Init(Iz_pxs_per_string , Iz_l_front_reduct, Iz_r_front_reduct, (BYTE)w))
            return;

        string data("MC_SET_BINNING " );

char b[20];
        memset(b,0,20);
        sprintf(b,"%X",w);
        data += b;
        test_dll_Show_s_dw("Send Binning:",w,0);
        if(If_svaz_udp == 1)
        {
                if(adapter_Send((char*)data.c_str()))
                    return;
        }
        else
        {
                if(svaz.Send((char*)data.c_str()))
                    return;
        }

string as;

        memset(b,0,20);
        sprintf(b,"%X",w);
        as = b;
        regdata->iz_binning = (BYTE)w;
        if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_IZ_BINNING], (char*)as.c_str()))
            return;
}

//--------------------------------------------------------------------------------------------
//      Функция возвращает биннинг датчика.
//      аргументы: - нет
//  возвращаемое значение:
//               - биннинг
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
WORD     p_dll_GetBinning()
{
   return (WORD)regdata->iz_binning;
}


//--------------------------------------------------------------------------------------------
//      Функция устанавливает состояние "h-фильтра" датчика.
//   Состояние фильтра передается на датчикк, записывается в структуру REGDATA и файл *.ini
//      аргументы: - char w - состояние фильтра
//  возвращаемое значение:
//               - нет
//  коментарии: - последующий вызов p_dll_GetError()- установит наличие ошибок
//------------------------------------------------------------------------------------------------
void     p_dll_SetFilter(char w)
{
 WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
 ADAPTER_SEND adapter_Send = g_adapter_Send;
 TEST_DLL_SHOW_S_BT test_dll_Show_s_bt =  g_test_dll_Show_s_bt;
 string data("MC_SET_FILTER " );

 char b[20];
     memset(b,0,20);
     sprintf(b,"%X",w);
     data += b;
        test_dll_Show_s_bt("Set HFiltr:",w == 0x30 ? 0 : 1, 0);
        if(If_svaz_udp == 1)
        {
                if(adapter_Send((char*)data.c_str()))
                    return;
        }
        else
        {
                if(svaz.Send((char*)data.c_str()))
                    return;
        }
string as;
        memset(b,0,20);
        sprintf(b,"%X",w);
        as = b;
 //REGDATA *rg = regdata;

        Iz_h_filer = (bool)((w == 0x30) || (w == 0) ? false : true);
        if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_IZ_HARD_FILTER], (char*)as.c_str()))
            return;
}
//--------------------------------------------------------------------------------------------
//      Функция принимает указатель на буфер строки с UNICODE - символами и возвращает
//  буфер строки с АNSI-символами
//      аргументы: - TCHAR *pszW - указатель на буфер с UNICODE - символами
//  возвращаемое значение:
//              DWORD  0 - при нормальном завершении
//                     HRESULT_FROM_WIN32 - при ошибке
//  коментарии:
//      - функция генерирует исключения если:
//          -указатель на входной буфкр равен нолю
//          -если не может выделить память, под выходной буфер
//      - функция выделяет память под буфер, который после вызова и использования
//  должен быть удален
//------------------------------------------------------------------------------------------------
#ifndef unix
DWORD  UnicodeToAnsi(TCHAR *pszW, LPSTR *ppszA)
{
    ULONG cbAnsi, cCharacters;
    DWORD dwError;


    if (pszW == NULL)
    {
        throw EXP_ERROR("UnicodeToAnsi","Wrong argument pszW == NULL");
    }

    cCharacters = wcslen((const wchar_t*)pszW)+1;

    cbAnsi = cCharacters*2;


    *ppszA = (LPSTR) new ULONG[cbAnsi];
    if (NULL == *ppszA)
    {
      throw EXP_ERROR("UnicodeToAnsi","Can not allocate memory");
    }

    if (0 == WideCharToMultiByte(CP_ACP, 0, (LPWCH)pszW, cCharacters, *ppszA,
                  cbAnsi, NULL, NULL))
    {
        dwError = GetLastError();
        delete[]  ppszA;
        *ppszA = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }
    return 0;
}


//--------------------------------------------------------------------------------------------
//      Функция принимает указатель на буфер строки с ANSIE - символами и возвращает
//  буфер строки с UNICODE-символами
//      аргументы: - LPCSTR - pszA указатель на буфер с ANSI - символами
//                 - wchar_t* - ppszW указатель на выходной буфер, который был выделен заранее.
//  возвращаемое значение:
//              DWORD  0 - при нормальном завершении
//                     1 - при если указатель на входной буфер равен NULL
//              DWORD dwError - если произошла ошибка при преобразовании функции
//  коментарии: - функция генерирует исключения если:
//          -указатель на входной буфкр равен нолю
//          -если не может выделить файл, под выходной буфер
//------------------------------------------------------------------------------------------------
DWORD  AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW)
{
    ULONG cCharacters;
    DWORD dwError;
    if (NULL == pszA)
    {
        *ppszW = NULL;
        return 1;
    }
    cCharacters =  strlen(pszA)+1;
    if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,
                  (LPWCH)ppszW, cCharacters))
    {
        dwError = GetLastError();
        *ppszW = NULL;
        return dwError;
    }
    return 0;
}
#endif
//--------------------------------------------------------------------------------------------
//      функция устанавливает флаг MAIN_APP
//      аргументы: - нет
//  возвращаемое значение:
//                  - нет
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
void setFlagMainAPP(bool m, bool z)
{

TIFF_SET_FLAGMAINAPP tiff_setFlagMainAPP = g_tiff_setFlagMainAPP;

    flagMainApp = m;
    tiff_setFlagMainAPP(z);   
}

//--------------------------------------------------------------------------------------------
//      Функция заполняет структуру NOISE_DATA информацией о шуме на входе dll и выходе dll
// используется при семках темнового тока для собирания статистической информации о шуме при тех.
// прогоне датчика в цеху
//      аргументы:
//              - V_NOISE_DATA *v_data- контейнер(vector) структур NOISE_DATA
//              - DWORD *dw_in - шум на входе dll
//              - DWORD *dw_out - шум на выходе dll
//              - bool flag_ret - трансляция в функцию флага MAIN_APP
//  возвращаемое значение:
//              - нет
//  коментарии:
//            -  Функция заполняет структуру NOISE_DATA информацией о шуме на входе dll и выходе dll
//      затем эта структура сохраняется в контейтере V_NOISE_DATA *v_data
//------------------------------------------------------------------------------------------------
void  p_dll_src_SetContainerOfNoise(V_NOISE_DATA *v_data, DWORD *dw_in, DWORD *dw_out, bool flag_ret)
{
 NOISE_DATA nd;

        if(flag_ret == 0)
        {
                return;
        }

        memcpy(nd.d_in, dw_in, sizeof(DWORD) * 9);
        memcpy(nd.d_out, dw_out, sizeof(DWORD) * 9);
        v_data->push_back(nd);
}

//--------------------------------------------------------------------------------------------
//Функция меняет местами  матрицы
//      аргументы: -
//            WORD *wtr - буфер для обмена
//            DWORD len  - длина буфера в пикселях
//  возвращаемое значение:
//              -указатель на буфер
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
void p_dll_src_MatrixRevers(WORD *w, DWORD len)
{
int num_str = len / Iz_pxs_per_string;
WORD num_matrix = Iz_pxs_per_string == 2048 ? 8 : 9;
WORD pix_in_matrix = Iz_pxs_per_string == 2048 ? 256 : Iz_pxs_per_string == 1152 ? 128 : Iz_pxs_per_string == 2304 ? 256 : 64;
int i, j;
WORD *wtr = w;
int pixlen =  Iz_pxs_per_string;

for( j = 0; j < num_str; j ++)
{
        wtr = w + j * pixlen - 1;
   WORD b[9];
   WORD e[9];
        for(  i = 0; i < pix_in_matrix/2 ; i++)
        {
            memcpy(b, wtr + i * num_matrix, num_matrix * sizeof(WORD) );
            memcpy(e, wtr + pix_in_matrix * num_matrix - num_matrix *( i + 1), num_matrix * sizeof(WORD) );
            memcpy(wtr + i  * num_matrix, e, num_matrix * sizeof(WORD) );
            memcpy(wtr + pix_in_matrix * num_matrix - num_matrix *( i + 1) + num_matrix, b, num_matrix * sizeof(WORD) );
        }
    }
}
//--------------------------------------------------------------------------
//      Возвращает количество ошибок контрольной суммы произошедших с момента
// последнего включения  датчика. Функция используется только для датчика с 2048 пикселей
// аргументы: - нет
// возвращаемое значение:
//       WORD - количество ошибок контрольной суммы
//            0 - ошибок не было
// коментарии:
//       Количество ошибок сбрасывается только инициализацией датчика
//---------------------------------------------------------------------------
DWORD p_dll_GetChError()
{
ADAPTER_GETCHERROR adapter_GetChError = g_adapter_GetChError;
        return  adapter_GetChError();
}

//--------------------------------------------------------------------------
//      Возвращает количество ошибок всех видов(кроме контрольной суммы), всех dll
// аргументы: - нет
// возвращаемое значение:
//       DWORD - количество ошибок
//            0 - ошибок не было
// коментарии:
//       Количество ошибок сбрасывается только инициализацией датчика
//---------------------------------------------------------------------------
DWORD p_dll_GetError(char** t_out)
{

DWORD e = 0;
ADAPTER_GETERROR adapter_GetError = g_adapter_GetError;
REG_GETERROR reg_GetError = g_reg_GetError;
TEST_GETERROR test_GetError = g_test_GetError;
RDUC_GETERROR rduc_GetError = g_rduc_GetError;
TIFF_GETERROR tiff_GetError = g_tiff_GetError;

static char chE[300];
        memset(chE,0,300);

char *t;

        if(EXP_ERROR::ErrorGet())
        {
            t = (char*)EXP_ERROR::GetTextError()->c_str();
            strcat(chE, t);
            e |= UDP_ERROR_DLL;
             *t_out = &chE[0];
            return e;
        }

        if(adapter_GetError(&t))
        {
            strcat(chE,t);
            e |= ADAPTER_ERROR_DLL;
            *t_out = &chE[0];
            return e;
        }

        if(rduc_GetError(&t))
        {
            strcat(chE,t);
            e |= RDUC_ERROR_DLL;
            *t_out = &chE[0];
            return e;
        }


        if(reg_GetError(&t))
        {
            strcpy(chE,t);
            e |= REG_ERROR_DLL;
            *t_out = &chE[0];
            return e;
        }

        if(test_GetError(&t))
        {
            strcat(chE,t);
            e |= TEST_ERROR_DLL;
            *t_out = &chE[0];
            return e;
        }

        if(tiff_GetError(&t))
        {
            strcat(chE,t);
            e |= TIFF_ERROR_DLL;
            *t_out = &chE[0];
            return e;
        }

return e;
}

//--------------------------------------------------------------------------
//      Возвращает количество ошибок всех видов(кроме контрольной суммы), всех dll
// аргументы: - нет
// возвращаемое значение:
//       DWORD - количество ошибок
//            0 - ошибок не было
// коментарии:
//       Количество ошибок сбрасывается только инициализацией датчика
//---------------------------------------------------------------------------
DWORD p_dll_GetErrorDWord()
{
DWORD e = 0;
ADAPTER_GETERROR adapter_GetError = g_adapter_GetError;
REG_GETERROR reg_GetError = g_reg_GetError;
TEST_GETERROR test_GetError = g_test_GetError;
RDUC_GETERROR rduc_GetError = g_rduc_GetError;
TIFF_GETERROR tiff_GetError = g_tiff_GetError;

char *t;

    e |= reg_GetError(&t) == 0 ? 0 : REG_ERROR_DLL;
    e |= test_GetError(&t) == 0 ? 0 : TEST_ERROR_DLL;
    e |= rduc_GetError(&t) == 0 ? 0 : RDUC_ERROR_DLL;
    e |= adapter_GetError(&t) == 0 ? 0 : ADAPTER_ERROR_DLL;
    e |= tiff_GetError(&t) == 0 ? 0 : TIFF_ERROR_DLL;
    e |= EXP_ERROR::ErrorGet() == 0 ? 0 : UDP_ERROR_DLL;

return e;
}

//--------------------------------------------------------------------------------------------
//      Функция устанавливает время накопления датчика.
//   Время накопления передается на датчикк, записывается в структуру REGDATA и файл *.ini
//      аргументы: - TIMEPERSTRING t - время накопления датчика
//  возвращаемое значение:
//               - нет
//  коментарии:  - нет
//------------------------------------------------------------------------------------------------
void    p_dll_PutTimePerString(TIMEPERSTRING t)
{
ADAPTER_SEND adapter_Send = g_adapter_Send;
WRITEPARAMTOOREG  WriteParamTooReg = g_WriteParamTooReg;
string s = "MC_TIME_PER_STRING ";
            s += time_tx[t];
if(If_svaz_udp  == 1)
{
            if(adapter_Send((char*)s.c_str()))
                return;
}
else
{
            if(svaz.Send((char*)s.c_str()))
                return;
}
            regdata->iz_time_per_string = (BOOL)t;
            if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_TIME_PER_STRING], (char*)(t == _19MC ? "0" : "1")))
                return;
}
//--------------------------------------------------------------------------------------------
//      Функция возвращаетт время накопления датчика записанное в структуре regdata.
//      аргументы: - нет
//  возвращаемое значение:
//               - TIMEPERSTRING -время накопления
//  коментарии:  - нет
//------------------------------------------------------------------------------------------------
TIMEPERSTRING p_dll_GetTimePerString()
{
    return (TIMEPERSTRING)regdata->iz_time_per_string;
}



//--------------------------------------------------------------------------------------------
//      Функция устанавливает состояние "s-фильтра" dll.
//   Состояние фильтра записывается в структуру REGDATA и файл *.ini
//      аргументы: - int v - апертура фильтра 0 - 31.
//  возвращаемое значение:
//               - нет
//  коментарии: - последующий вызов p_dll_GetError()- установит наличие ошибок
//              -  если апертура равна 0, то фильтр выключен.
//------------------------------------------------------------------------------------------------
void p_dll_SetSFilter(int v)
{
WRITEPARAMTOOREG  WriteParamTooReg = g_WriteParamTooReg;
    if(v > _MAX_S_FILTER)
        return;
char b[10];
    memset(b,0,10);
    sprintf(b,"%d",v);
    if(WriteParamTooReg(reg, reg_str_resurs_param_name[EN_SOFT_FILTER], (char*)b))
        return;
    regdata->iz_s_filter = v;
}
//--------------------------------------------------------------------------------------------
//      Функция устанавливает состояние "s-фильтра" dll.
//   Состояние посылает UDP пакет, в качестве запроса о состоянии: включения/выключения датчика
//      аргументы: - int v - апертура фильтра 0 - 31.
//  возвращаемое значение:
//               - нет
//  коментарии: - последующий вызов p_dll_GetError()- установит наличие ошибок
//------------------------------------------------------------------------------------------------
bool p_dll_SendIcmp()
{
ADAPTER_SEND_ICMP adapter_SendIcmp = (ADAPTER_SEND_ICMP)g_adapter_SendIcmp;
      if(If_svaz_udp == 1)
          return        adapter_SendIcmp();
      else
          return        svaz.SendIcmp();
}
//-----------------------------------------------------------------------
//
//------------------------------------------------------------------------
#ifdef USE_REDUCT_TEST
REDUCTION* p_dll_GetRducPtr(WORD *ptr, TEST_RDUC_DATA *rdt_tst)
{
       return g_rduc_GetClassPtr(ptr, rdt_tst);
}
//----------------------------------------------------------------------
//
//--------------------------------------------------------------------
bool p_dll_SetNposstringnoise(WORD string_noise)
{
  return g_rduc_SetNposstringnoise(string_noise);
}

#endif

#ifndef unix
//-----------------------------------------------
 void MessegeOfBufer(WORD *ptr, DWORD len, WORD *beg)
 {

  char buf[2048];
        memset(buf, 0, 2048);

        char b[20];
            memset(b, 0, 20);
            sprintf(b,"%d",(&ptr[0] - 1) - beg);
            strcat(buf,b);
            strcat(buf," = ");


        for(DWORD i = 0; i < len; i++)
        {
            char b[20];
                memset(b, 0, 20);
                sprintf(b,"%04d",ptr[i]);
                strcat(buf,b);
                strcat(buf,";");
                if(((i + 1)%8 == 0) && (i != 0))
                {
                    strcat(buf,"\r\n");
                    if(i < (len - 8))
                    {
                    char b[20];
                        memset(b, 0, 20);
                        sprintf(b,"%d",&ptr[i] - beg);
                        strcat(buf,b);
                        strcat(buf," = ");
                    }
                }
        }
wchar_t w_buf[2048];
      wmemset(w_buf, 0, 2048 );
      AnsiToUnicode(buf,w_buf);
      MessageBox(NULL,w_buf,TEXT("TEST"), MB_OK);
 }
#endif
//---------------------------------------------------------------------
 char* p_dll_Patch_ini()
 {
     return (char*)ptch_ini;
 }
//----------------------------------------------------------------------
char* p_dll_Patch_work()
{
    return (char*)ptch_work;
}
