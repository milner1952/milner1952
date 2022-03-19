//---------------------------------------------------------------------------

//dll_src.cpp -
/********************************************

********************************************/

#ifndef __BORLANDC__
    #define _CRT_SECURE_NO_WARNINGS
    #ifndef _CRT_SECURE_NO_DEPRECATE
        # define _CRT_SECURE_NO_DEPRECATE (1)
    #endif

    #ifdef _MSC_BUILD
        #pragma warning(disable : 4996)
        #pragma warning(disable: 4800)
    #endif
#endif

#include <new>
#include <iostream>

#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>

#ifdef _MSC_BUILD
	#include "res_bcb.h"
#endif

#ifdef __BORLANDC__
	#include "res_bcb.h"
    #pragma hdrstop
    #include <vcl.h>
#endif



using namespace std;



const char Version_of_dll[] = "3.0.0";

#include <ctime>
#include "dll_src.h"
#include "test_dll.h"
#include "reg.h"
#include "svaz.h"
#include "Reduction.h"
#include "adapter.h"
#include "file.h"
#include "dll_needle.h"
#include "Reduction_class.h"
#include "s_dll_tif_buffer.h"

#ifdef unix
    #include "../../SRC/BCB/RES_UNIX/res_unix.h"
    char *ptr_e;
//reg_dll
    void *reg_lib;
    WRITEPARAMTOOREG  g_WriteParamTooReg;

//test_dll
    void *test_lib;

//adapter_lib
    void *adapter_lib;
#endif

#define VISION(i) {\
AnsiString as; \
as += (DWORD)*pt;                  \
as += "     ";                      \
as += (BYTE)(*(*pt + i +0));as += "/ ";\
as += str_adres;\
as += (int)(*pt + i +0);\
as+=" ";                 \
as += (BYTE)(*(*pt + i + 1));as += "/ ";\
as += str_adres;                         \
as += (int)(*pt+ i +1);                  \
as+=" ";                                  \
as += (BYTE)(*(*pt + i + 2));as += "/ ";    \
as += str_adres;                             \
as += (int)(*pt + i + 2);                    \
as+=" ";                                      \
as += (BYTE)(*(*pt + i + 3));as += "/ ";        \
as += str_adres;                                 \
as += (int)(*pt + i + 3);                        \
as+=" ";                                          \
as += (BYTE)(*(*pt + i + 4));as += "/ ";        \
as += str_adres;                                 \
as += (int)(*pt + i + 4);                        \
as+=" ";                                          \
as += (BYTE)(*(*pt + i + 5));as += "/ ";        \
as += str_adres;                                 \
as += (int)(*pt + i + 5);                        \
as+=" ";                                          \
as += (BYTE)(*(*pt + i + 6));as += "/ ";        \
as += str_adres;                                 \
as += (int)(*pt + i + 6);                        \
as+=" ";                                          \
as += (BYTE)(*(*pt + i + 7));as += "/ ";        \
as += str_adres;                                 \
as += (int)(*pt + i + 7);                        \
as+=" ";                                          \
MessageBox(NULL,(const char*)as.c_str(), str_mesenge,MB_OK);}\



//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#define INIT_CLR_TEST	{\
                test_dll_IzClrConsol(); \
                test_SetStringTooOutDebug1("                              Number N = ", \
                global++); \
                AnsiString as   = Iz_pxs_per_string; \
                as +=" ";    \
                as += "LIBCAP";  \
                AnsiString bs = "Version of sensor: "; \
                bs += Version; \
                bs += " VERSION DLL: "; \
                bs += Version_of_dll;\
                bs += " ";  \
                test_SetStringTooOutDebug6((char*)bs.c_str(), (char*)as.c_str()); \
                test_SetStringTooOutDebug2("GetBufer - in len = ", len, " Bytes"); \
                }
#else
#define INIT_CLR_TEST	{\
                test_dll_IzClrConsol(); \
                test_SetStringTooOutDebug1("                              Number N = ", \
                global++); \
                AnsiString as   = Iz_pxs_per_string == 0 ? "0" : "1"; \
                as +=" ";    \
                as += "LIBCAP";  \
                AnsiString bs = "Version of sensor: "; \
                bs += Version; \
                bs += " VERSION DLL: "; \
                bs += Version_of_dll;\
                bs += " ";  \
                test_SetStringTooOutDebug6((char*)bs.c_str(), (char*)as.c_str()); \
                test_SetStringTooOutDebug2("GetBufer - in len = ", len, " Bytes"); \
                }
#endif


#define AZ_14_BIT_DIAPAZON\
        {                      \
        WORD *wtr = (WORD*)*pt;\
                for(DWORD i = 0L ; i < len/2; i++)\
                        {                          \
                           wtr[i] = wtr[i]/4;       \
                        }                            \
        }                                             \
//----------------------------------------------------------------
#define SET_14_BIT_DIAPAZON                             \
                if(wtr != NULL)                         \
                {                                       \
                        for(DWORD i = 0; i < len/2; i++) \
                        {       \
                                wtr[i] &= 0x7fff;\
                                if((wtr[i] & 0x04000) != 0)\
                                {                           \
                                        if((wtr[i] & 0x2000) == 0)\
                                                wtr[i] = 0x03fff;  \
                                        else                        \
                                }                                     \
                        }                                              \
                }                                                       \

DWORD dw_ion[9];
double db_ion[9];
DWORD dw_ion_afte[9];
double db_ion_afte[9];
DWORD dw_ion_bfr[9];
double db_ion_bfr[9];

DWORD dw_noise_bufer[9];
double db_noise_bufer[9];
SVAZ svaz;

WORD data_in[256][4];



static WORD global;
static bool flag_load_from_bin_file;
static bool flag_load_from_bin_file_out;
HINSTANCE m_hinst;
REGDATA *regdata;

BYTE *ptr;
int error;
DWORD e_scl = 0;
extern MAIN_APP flagMainApp;
extern V_NOISE_DATA v_statistic_data;
V_NOISE_DATA v_noise_data;

char Version[100];
char bufer[100];
TRegistry *reg;                                            //save_in_bufer_in_file_or_read_i

const char *RegKey = "Software\\N_PRIBOR_P_KAMERA_2048_DLL_SRK\\ParamList";
const char *   reg_str_resurs_param_name[] =      {"TIMER_PERIOD","GANE","PIXELS_PER_STRING","NORM_DATA",
                                            "SAVE_IN_BUFFER_IN_FILE_OR_READ_IT","5MC_PER_STRING",
                                            "IZ_LOG_DEBUG", "IZ_REDUCT", "IZ_SVAZ_UDP",
                                            "MTR_GANE", "IN_FILE_OF_BUFFER","IF_ERROR_ION_COMPENSATION",
                                            "IF_DEL_IGLS","IF_OLD_1024","FLAG_16_14", "LEFT_REDUCT_FRONT",
                                            "RIGHT_REDUCT_FRONT", "IF_ROTATION", "WITH_NOISE_STATISTICS",
                                             "WITH_MATRICES", "TIME_REPIT", "REVERS_MATRIX", "IZ_DAC", "IZ_BINNING","IZ_HARD_FILTER",
                                             "DIV_16_9_4","DIV_4_3_2","IZ_RANGE","REDUKTION_TO_RANGE"
                                             };



//---------------------------------------------------------
void
#ifdef __BORLANDC__
	Init(TComponent* Owner,wBegin b,wWork w,wEnd e)
#else
    Init(HWND /*Owner*/,wBegin /*b*/,wWork /*w*/ ,wEnd /*e*/)
#endif
{

#ifdef unix
    P_INITREG InitReg;
    READPARAMTOOREG ReadParamTooReg;

//---------lib reg-------------------------------------
    reg_lib = dlopen("./libp_reg_lib.so",RTLD_LAZY);
    if(!reg_lib)
    {
     char e[1000];
     strcpy(e, dlerror());
     fprintf(stderr,"%s",e);
     return;
    }

    InitReg = (P_INITREG)dlsym(reg_lib,"InitReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }

    ReadParamTooReg = (READPARAMTOOREG)dlsym(reg_lib,"ReadParamTooReg");

    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }

    g_WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
//--------------test_lib--------------------------------------------------------
    test_lib = dlopen("./libs_test_dll.so",RTLD_LAZY);
    if(!test_lib)
    {
     char e[1000];
     strcpy(e, dlerror());
     fprintf(stderr,"%s",e);
     return;
    }
    TEST_DLL_INIT test_dll_Init = (TEST_DLL_INIT)dlsym(test_lib, "test_dll_Init");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }


    //--------------adapter_lib--------------------------------------------------------
        adapter_lib = dlopen("./libp_dll_adapter.so",RTLD_LAZY);
        if(!adapter_lib)
        {
         char e[1000];
         strcpy(e, dlerror());
         fprintf(stderr,"%s",e);
         return;
        }

        ADAPTER_IINIT adapter_Init = (ADAPTER_IINIT)dlsym(adapter_lib, "adapter_Init");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }
        ADAPTER_PUT5MCPERSTRING adapter_Put5McPerString = (ADAPTER_PUT5MCPERSTRING)dlsym(adapter_lib, "adapter_Put5McPerString");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }

#endif
        flagMainApp = APP_N_PRIBOR;
        flag_load_from_bin_file = 0;
        flag_load_from_bin_file_out = 0;
#ifndef unix
        error = 0;
#endif
        global = 0;
        reg = InitReg(RegKey, reg_str_resurs_param_name, 25 + 4 );

        regdata = ReadParamTooReg();

char m_name[1024];
        memset(m_name,0,1024);
#ifdef __BORLANDC__
        GetModuleFileName(m_hinst, m_name, sizeof(m_name));
#else
        #ifndef unix
            GetModuleFileName(m_hinst, (LPWSTR)m_name, sizeof(m_name));
        #else
        m_name[0] = 0;
        char *ptr = (char*)getenv("HOME");
        int num;

        if(strlen(ptr) > 1023)
            num = 1023;
        else
            num = strlen(ptr);

        memcpy(m_name, ptr,num);
        #endif
#endif
        test_dll_Init(m_name, If_log_debug);

#ifdef __BORLANDC__
		m_hinst = GetModuleHandle( m_name  );
#else
    #ifndef unix
		m_hinst = GetModuleHandle((LPWSTR)m_name);
    #else
        m_hinst = 0;
    #endif
#endif

#ifdef DLL_TIF_BUFER
        d_dll_tif_bufer_Init(1152, 1152);
#endif


        if(adapter_Init(If_svaz_udp, regdata->timer_repit, Iz_pxs_per_string, Iz_icmp_test ))
                return;
adapter_Put5McPerString(regdata->_5ms_per_string);
}



 char t[100];
//-----------------------------------------------------------------
char* GetVersionDll()
{

 string s = str_vers_dll;
 s +=  Version_of_dll;
 s +=  str_vers_prog;
 s += bufer;
 memset(t , 0, 100);
 memcpy(t, s.c_str(),s.length());
 return (char*)t;
}


//-----------------------------------------------------------------
bool  AfteInitOnCreate()
 {
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return 0;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif

        if(If_svaz_udp == 0)
                svaz.Init(flagMainApp, Iz_binning);
                
char    buf_p[10];
        memset((void*)bufer,0,sizeof(bufer));
        memset((void*)buf_p,0,sizeof(buf_p));


WORD len;
#ifndef unix
::Sleep(500);
#else

#endif
        if(If_svaz_udp == 1)
        {
         bool ret;
              do
              {
                        ret = arp_SendArpPacket();
              }while(ret);
         }
        else
                svaz.SendArpPacket();

char *ptr;
        ptr = s_dll_src_Get_Version_From_Datchic((WORD*)&len);
        strcpy(bufer, ptr);

int i;
        for(i = 0; i < len; i++)
        {
                if(bufer[i] == 0x20)
                {
                        memcpy(buf_p,bufer,i);
                        buf_p[i] = 0x00;
                        Iz_pxs_per_string = atoi(buf_p);
                        memset(Version, 0, sizeof(Version));
                        if((Iz_pxs_per_string  == 1152) || (Iz_pxs_per_string  == 576))
                                strcpy(Version, (char*)bufer);
                        else
                                strcpy(Version, (char*)&bufer[i+1]);


                AnsiString s;
                        s = buf_p;
                        WriteParamTooReg(reg_str_resurs_param_name[2], (char*)s.c_str());
                        if((Iz_binning > 2) & (Iz_pxs_per_string == 576))
                        {
                                regdata->iz_binning = 2;
#ifdef __BORLANDC__
                                AnsiString as = regdata->iz_binning;
#else
		                char b[20];
		                memset(b, 0, 20);
    #ifndef unix
		                _itoa(regdata->timer_period, b, 10);
    #else
                         sprintf(b,"%d",regdata->timer_period);
    #endif
		                AnsiString as(b);
#endif
		                WriteParamTooReg(reg_str_resurs_param_name[23], (char*)as.c_str());
                        }

                        break;
                }
        }
 rduc_Init(Iz_pxs_per_string, Iz_l_front_reduct, Iz_r_front_reduct, Iz_binning);
#ifdef DLL_NIDLE
 dll_needle_Init(m_hinst ,&v_statistic_data, &v_noise_data ,flagMainApp );
#endif

SetModeRun((MODE)(If_with_matrices == 0));
 return true;
 }

//----------------------------------------------------------------
void Put5McPerString(unsigned char d, bool iz_wr)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif

        if(Iz_pxs_per_string == 2048)
        {
                test_dll_IncrementYPositrion();
                d == 0?  test_SetStringTooOutDebug6("Put5McPerString__ - in", "= 1,25 mS per string"):
                test_SetStringTooOutDebug6("Put5McPerString__ - in", "= 2,5 mS per string");
        }
        else
        {
                test_dll_IncrementYPositrion();
                d == 0?  test_SetStringTooOutDebug6("Put5McPerString__ - in", "= 2,5 mS per string"):
                test_SetStringTooOutDebug6("Put5McPerString__ - in", "= 5 mS mS per string");
        }

        AnsiString data("MC_FLIP_FLOP ");
        data += (d & 0xff);
        adapter_Put5McPerString(d);
        if(If_svaz_udp == true)
                adapter_Send((char*)data.c_str());
        else
                svaz.Send((char*)data.c_str());

        if(iz_wr != 0)
        {
#ifdef __BORLANDC__
                AnsiString s(d & 0xff);
#else
		char b[10];
				memset(b, 0, 10);
        #ifndef  unix
				itoa(d & 0xff, b, 10);
        #else
                 sprintf(b,"%d",d & 0xff);
        #endif
				AnsiString s(b);
#endif
                WriteParamTooReg(reg_str_resurs_param_name[5], (char*)s.c_str());
        }

        If_5ms_per_string = BYTE(d == 0x00 ? 0x00: 0x01);
        test_SetStringTooOutDebug6("Put5McPerString - out", "  ");
        return;
 }
 void ClearMesText();
//-----------------------------------------------------------------
unsigned long   DLL_EI GetBufer(BYTE **pt, unsigned long ln, bool disable_set_binning)
{
unsigned long len;
//unsigned long t_ln = ln;
         ClearMesText();

        if(s_dll_src_FlagMainApp() == 0)
        {

                if(ln > 300000)
                        ::Sleep(200);
        }
        else
                ::Sleep(400);
DWORD num = 0;
DWORD tim_d;
        len = ln;


        len = len * Iz_binning;

        if(Iz_pxs_per_string == 1152)
        {
        len += Iz_pxs_per_string * 2 * (2 + (4 * Iz_binning));
        }
        else
        if(Iz_pxs_per_string == 576)
        {
        len += Iz_pxs_per_string * 2 * (2 + 16);
        }



        len += Iz_pxs_per_string * 2 * 6;

#ifdef _MIT_ERROR_ION
        len += Iz_pxs_per_string * 2;
#endif

        if(If_log_debug != 0)
        {
                INIT_CLR_TEST;
         test_SetStringTooOutDebug1("Req ln: " , ln);
        }        
        else
                global++;

        s_dll_src_PutGaneMtr(Iz_mtr_gane);
        AnsiString get = "GET ";
#ifdef __BORLANDC__
        get += len;
#else
char buf[10];
	memset(buf, 0, 10);
    #ifndef unix
        ltoa(len, buf, 10);
    #else
        sprintf(buf,"%ld",len);
    #endif
	get += buf;
#endif

clock_t t1, t2;

        t1 = clock();
        if((flag_load_from_bin_file == 0)   && (flag_load_from_bin_file_out == 0))
        {
                do
                {
                        if(If_svaz_udp == 1)
                        {
                                adapter_SetFlagRs();
                                adapter_Send((char*)get.c_str());

                                num =    adapter_ResiveBuffer((BYTE**)&ptr, len);

                        }
                        else
                        {
                                svaz.Send((char*)get.c_str());
                                num =     svaz.ReceiveBuffer((BYTE**)&ptr , len, 1000);

                                if(If_5ms_per_string == 0)
                                        tim_d =   (DWORD)((double)num * (2.0/4096.0));
                                else
                                        tim_d =   (DWORD)((double)num * (1.0/4096.0));

                                Sleep(tim_d);
                        }
                        if(num != len)
                        e_scl++;
                }
                while(num != len);

                if((Iz_save_in_bufer_in_file_or_read_it == 1)  || (Iz_save_in_bufer_in_file_or_read_it == 3))
                {
                file_Write("C:\\Users\\Public\\buffer_log_in.log",(const WORD *)ptr, num);
                }
        }
        else
        {
                   if(flag_load_from_bin_file == 1)
                   {
                        file_Read("C:\\Users\\Public\\buffer_log_in.log", (BYTE**)&ptr, &len);
                        num = len;
                        flag_load_from_bin_file = 0;
                   }

        }

        if(e_scl > 0)
                        error++;
        if(If_log_debug != 0)
        {
                t2 = clock();
            #ifndef unix
               double t = (t2 - t1) / CLK_TCK;
            #else
               double t = (t2 - t1) / CLOCKS_PER_SEC;
            #endif

                test_SetStringTooOutDebug5("Time per image: " , t);
                if(num != 0)
                {
                        t = t / (num / (Iz_pxs_per_string * 2));
                        test_SetStringTooOutDebug5("Time per string: " , t);
                }
                else
                {
                        test_SetStringTooOutDebug6("Time per string: " , "NO RX_DATA");
                }
                test_SetStringTooOutDebug1("Errors: " , error);
        }

WORD *wtr;
        if(Iz_pxs_per_string != 2304)
        {
                wtr = (WORD*)ptr;

                 if(Iz_pxs_per_string == 1024)
                 {
                     wtr -= 1;

                 }                   
                if(Iz_pxs_per_string == 1152)
                {
                *pt = (BYTE*)(wtr +1);   //+0
                }
                else
                if(Iz_pxs_per_string == 576)
                {
                *pt = (BYTE*)(wtr + 1);   //+1
                 wtr -= 1; //???????
                }
                else
                *pt = (BYTE*)(wtr + 1);
        }
        else
        {
           wtr = ((WORD*)ptr  + 9);
          *pt = (BYTE*)wtr ;
        }
WORD        ofs = 0;
        if(flag_load_from_bin_file_out == 0)
        {
                ofs = Iz_pxs_per_string * 6;

        if(Iz_pxs_per_string != 2304)
                ofs *=   Iz_pxs_per_string == 1024 ? 2 : 1;
        }

        if((If_with_noise_statistics  == 1) || (If_in_file_off_byffer == true))
        {
                svaz.SetTestNoiseData(wtr + ofs, flagMainApp , If_svaz_udp,
                                        Iz_pxs_per_string, If_with_matrices, len/2);
                test_SetStringTooOutDebug6("WITH ERROR NOISE: " , "ON");
        }


        if(flag_load_from_bin_file_out == 0)
        {

                s_dll_src_GetNoiseDataInNoiseBufer(wtr + ofs, dw_noise_bufer, db_noise_bufer,
                                        Iz_pxs_per_string, flagMainApp, len/2);

                s_dll_src_SetContainerOfNoise(&v_noise_data, rduc_GetPtrForMidleBuferIn(),
                                                                rduc_GetPtrForMidleBuferOut(),
                                                                dw_ion, dw_ion_bfr, dw_ion_afte,
                                                                flagMainApp, len/2);
        }
        if((Iz_pxs_per_string == 2048) || ((Iz_pxs_per_string == 2304)))
        {


              if((If_in_file_off_byffer == true) || (If_with_noise_statistics  == true))
              {
                        WORD *e_ptr = wtr + (Iz_pxs_per_string * 5);
                        WORD *s_ptr = wtr + (Iz_pxs_per_string * 5);
                        s_dll_src_GetIonError2048_2304(s_ptr , e_ptr , Iz_pxs_per_string,
                                                       len, dw_ion_bfr, db_ion_bfr, false);
              }
        }
                ofs = Iz_pxs_per_string;
        ofs += Iz_pxs_per_string == 1152 ? 1 : ofs += Iz_pxs_per_string == 576 ? 1 : 0;


        if((If_in_file_off_byffer == true) || (If_with_noise_statistics  == 1))
                rduc_GetMideleForBuferInFromADC(wtr + ofs, (num - ofs * 4)/2);

           #ifdef MEDIAN1
           if(Iz_pxs_per_string == 1152)
                MedianFiltrHorz1(wtr + Iz_pxs_per_string - WINDOW/2 ,  Iz_pxs_per_string * 8 + WINDOW/2 ,WINDOW);
           else
                MedianFiltrHorz1(wtr + Iz_pxs_per_string - WINDOW/2 ,  Iz_pxs_per_string * 16 + WINDOW/2 ,WINDOW);
           #endif

        if(Iz_reduct == 1)
        {
            if(flag_load_from_bin_file_out == 0)
            {
                rduc_SetData((WORD*)(wtr + ofs ) , (num - ofs *4)/2);
                rduc_GetData((WORD*)(wtr + ofs  ), (num - ofs *4)/2);
                rduc_SetData((WORD*)(wtr + ofs  ) , (num - ofs *4)/2);
                test_SetStringTooOutDebug6("Reduction: " , "ON");
            }
        }
        else
                test_SetStringTooOutDebug6("Reduction: " , "OFF");

        if(Iz_pxs_per_string == 1152)
        {
        len -= Iz_pxs_per_string * 2 * (2 + (4 * Iz_binning));
        }
        else
        if(Iz_pxs_per_string == 576)
        {
        len -= Iz_pxs_per_string * 2 * (2 + 16);
        }
        len -=  Iz_pxs_per_string * 2 * 6;
#ifdef _MIT_ERROR_ION
        len -=  Iz_pxs_per_string * 2;
#endif
        if(If_log_debug != 0)
                test_SetStringTooOutDebug2("GetBufer - out num = ", num, " Bytes");
BYTE *f_ptr;
          //APP_NEYSTROEV, APP_N_PRIBOR ,
        f_ptr = (BYTE*)wtr;
        if((s_dll_src_FlagMainApp() == APP_NEYSTROEV) || (s_dll_src_FlagMainApp() == APP_N_PRIBOR))
        {
                        *pt = *pt + 6 * Iz_pxs_per_string * 2;
                        *pt +=   Iz_pxs_per_string * 2;
        }

        if(((Iz_pxs_per_string == 2048) || (Iz_pxs_per_string == 2304)) && ((If_in_file_off_byffer == true)||
                (If_with_noise_statistics  == 1))  &&
                ((Iz_reduct == 1) ))
        {
                WORD *e_ptr = wtr + (Iz_pxs_per_string * 5);
                WORD *s_ptr = wtr + (Iz_pxs_per_string * 5);
                        s_dll_src_GetIonError2048_2304(s_ptr , e_ptr , Iz_pxs_per_string,
                                                len, dw_ion_afte, db_ion_afte, false);
        }

        if((If_in_file_off_byffer == true) || (If_with_noise_statistics == 1))
                rduc_GetMideleForBuferOutFromADC(wtr + ofs, (num - ofs*4)/2);

        if(If_in_file_off_byffer == true)
                file_SaveIn_bufer_log("C:\\Users\\Public\\buffer_log.log", f_ptr, len, regdata);

        if(If_with_noise_statistics == 1)
        {
                file_SaveIn_statistic_log("C:\\Users\\Public\\statistic_log.log", dw_noise_bufer, rduc_GetPtrForMidleBuferIn(),
                                               rduc_GetPtrForMidleBuferOut(), db_noise_bufer , Iz_pxs_per_string, flagMainApp,
                                               dw_ion, dw_ion_bfr, dw_ion_afte, regdata, len/2);
        }
        test_SetStringTooOutDebug6("in_file_off_byffer: " , (char*)(If_in_file_off_byffer == 0 ?"OFF":"ON"));

        if(flag_load_from_bin_file_out == 1)
        {
                file_Read("C:\\Users\\Public\\buffer_log_out.log", (BYTE**)&ptr, &len);
                num = len;
                flag_load_from_bin_file_out = 0;
                  if(Iz_pxs_per_string == 2304)
                  {
                        wtr = (((WORD*)ptr) + 9);
                  }
                  else
                        if(Iz_pxs_per_string == 1024)
                                wtr = (((WORD*)ptr) - 1);
                        else
                                wtr = (((WORD*)ptr) + 1);
                *pt = (BYTE*)(wtr);
        }
        else
        if((Iz_save_in_bufer_in_file_or_read_it == 2) || (Iz_save_in_bufer_in_file_or_read_it == 3))
        {
                file_Write("C:\\Users\\Public\\buffer_log_out.log",(const WORD *)ptr, num);
        }

   #ifdef MEDIAN2
         *pt -= 2;//(2 + 2 + 2);
   #endif
        if(If_rotation == 1)
        {
                s_dll_src_RotationHor((WORD*)*(pt), len/2);
                test_SetStringTooOutDebug6("if_rotation: ", "ON");
        }
        else
                test_SetStringTooOutDebug6("if_rotation: ", "OFF");

                

       test_SetStringTooOutDebug1("REDUKT L_FRONT = ", Iz_l_front_reduct);
       test_SetStringTooOutDebug1("REDUKT R_FRONT = ", Iz_r_front_reduct);

       if(Iz_pxs_per_string == 2048)
       {
                If_5ms_per_string == 0?  test_SetStringTooOutDebug6("Put5McPerString", " 1,25 mS per string"):
                test_SetStringTooOutDebug6("Put5McPerString:", " 2,5 mS per string");
       }
       else
       {
                If_5ms_per_string == 0?  test_SetStringTooOutDebug6("Put5McPerString: ", "2,5 mS per string"):
                test_SetStringTooOutDebug6("Put5McPerString:", " 5 mS mS per string");
       }
       test_SetStringTooOutDebug1("GANE = ", Iz_mtr_gane);

       if(If_revers_matrix == 1)
               s_dll_src_MatrixRevers(wtr, len, regdata);
       {



       if(disable_set_binning == 0)
       {
       WORD offs;
                if(Iz_pxs_per_string == 1152)
                {
                offs =(s_dll_src_FlagMainApp() == 0 ? 0 :  ( (4 * Iz_binning + 2) * Iz_pxs_per_string * 2));

                }
                else
                {
                offs =(s_dll_src_FlagMainApp() == 0 ? 0 :  ( (16 + 2) * Iz_pxs_per_string * 2));
                
                }

       WORD *wtr = (WORD*)(*pt  + offs  );


                SetBinningBuffer(&wtr, len / 2 , Iz_binning);
                test_SetStringTooOutDebug1("Binning: " , Iz_binning);
                test_SetStringTooOutDebug1("Len in BYTES: " , len);

                test_SetStringTooOutDebug1("HFilter: " , Iz_h_filer/* == 0 ? "0" : "1"*/);

                *pt = (BYTE*)(wtr );
       #ifdef VISION_BUFER
                VISION(0);
       #endif
       }
            #ifdef MEDIAN2
                MedianFiltrHorz2(wtr,  len ,WINDOW);// 9
           #endif
       len = len / Iz_binning;

           #ifdef DLL_TIF_BUFER
           BYTE *btr = *pt;
                s_dll_tif_bufer_Set(btr, len, Iz_binning);
           #endif
           
       }
 return len;
}

//-----------------------------------------------------------------
void s_dll_src_PutGaneMtr(BYTE g)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
     WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif
BYTE gane = BYTE(0x07 & g);
        AnsiString data("GANE_MTR ");
        data += "G ";
        data += gane;

        if(If_svaz_udp == 1)
                adapter_Send((char*)data.c_str());
        else
                svaz.Send((char*)data.c_str());

        Iz_mtr_gane = gane;
#ifdef __BORLANDC__
        AnsiString s = gane;
#else
		AnsiString s = gane == 0 ? "0" : "1";
#endif
        WriteParamTooReg(reg_str_resurs_param_name[9], (char*)s.c_str());
}

//---------------------------------------------------------

MAIN_APP s_dll_src_FlagMainApp(MAIN_APP b)
{
         if(b < APP_END_NUMBERING)    //2
                flagMainApp = (MAIN_APP)b;
         return  flagMainApp;
}

//-----------------------------------------------------
bool  TimePerString(BYTE c)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return 0;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif

 if(c != 2)
 {
#ifdef __BORLANDC__
        AnsiString s(c & 0xff);
#else
char b[10];
		memset(b, 0, 10);
        #ifndef unix
                itoa( c & 0xff, b, 10);
        #else
                sprintf(b,"%d", c & 0xff);
        #endif
		AnsiString s(b); 
#endif
        WriteParamTooReg(reg_str_resurs_param_name[5], (char*)s.c_str());
        If_5ms_per_string = (BYTE)(c == 0 ? 0: 1);
 }
 return  If_5ms_per_string ==  0 ? false : true;
}

//-----------------------------------------------------------------
 bool   Connect(){return true;}

//-----------------------------------------------------------------
 void   Disconnect() {}

//-----------------------------------------------------------------
 void   Dll_Close()
 {


#ifdef  unix
DESTROYREG DestroyReg;
     DestroyReg = (DESTROYREG)dlsym(reg_lib,"DestroyReg");
     if((ptr_e = dlerror()) != NULL)
     {
         char e[1000];
         strcpy(e, ptr_e);
         fprintf(stderr,"%s",e);
      return;
     }
#endif
        DeleteBufer();
        DestroyReg();
        adapter_Delete();
#ifdef DLL_NIDLE
        dll_needle_Close();
#endif
#ifdef DLL_TIF_BUFER
       // d_dll_tif_bufer_Close();
#endif
 }

//-----------------------------------------------------------------
void DeleteBufer()
{
        if(If_log_debug != 0)
                  test_SetStringTooOutDebug1("DeleteBufer - in ",0);


        if(ptr != NULL)
        {
                if(If_log_debug != 0)
                       test_SetStringTooOutDebug1("DeleteBufer - delete ",1);

        }

        if(If_log_debug != 0)
                       test_SetStringTooOutDebug1("DeleteBufer - out ",2);
}

//-----------------------------------------------------------------
void  SetModeRun(MODE mode)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif
AnsiString data("MC_MODE ");
char *ptr = (char*)chAppType[(int)flagMainApp];
        if(mode == RUN)
        {
                If_with_matrices = 1;
                test_SetStringTooOutDebug1("mode: Run with matrices ",0);
                data += ("R ");
                data +=  ptr;
        }
        else
        if(mode == TEST)
        {
                If_with_matrices = 0;
                test_SetStringTooOutDebug1("mode: Run without matrix matrices ",1);
                data += ("T ");
                data += ptr;

        }

        AnsiString s =  If_with_matrices == 0 ? "0":"1";
        WriteParamTooReg(reg_str_resurs_param_name[19], (char*)s.c_str());

        if(If_svaz_udp == 1)
                adapter_Send((char*)data.c_str());
        else
        {
                svaz.Send((char*)data.c_str());
        }
}

//---------------------------------------------------------------
void SetDac(WORD i)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif

	AnsiString    data("P_KAMERA_CMD_SET_DAC");
	test_SetStringTooOutDebug1("SET_DAC: ", i);
	data += " ";

#ifndef __BORLANDC__
	
	M_SPRINTF((data), i)	
#else
		data += i;
#endif


		data += " ";
		data += "A";
        if(If_svaz_udp == 1)
                adapter_Send((char*)data.c_str());
        else
        {
                svaz.Send((char*)data.c_str());
                InitMem(i);

        }
Iz_dac = i;

#ifdef __BORLANDC__
	AnsiString dac(i);
	WriteParamTooReg(reg_str_resurs_param_name[22], dac.c_str());
#else
{
	char b[100];
	sprintf(b, "%d", i);
	AnsiString dac(b);
	WriteParamTooReg(reg_str_resurs_param_name[22], (char*)dac.c_str());
}
#endif




}

//-----------------------------------------------------------------
void SetFlagIzMainAPP(MAIN_APP m)
{
        s_dll_src_FlagMainApp(m);
}

//------------------------------------------------------------
WORD GetPixelPerString(){return (WORD)Iz_pxs_per_string;}

//------------------------------------------------------------
BYTE GetMtrGane(){return (WORD)Iz_mtr_gane;}

//-----------------------------------
void   Reduct_set(bool reduct)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");

    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif
        Iz_reduct = reduct;
        AnsiString s =  reduct == 0 ? "0":"1";
        WriteParamTooReg(reg_str_resurs_param_name[7], (char*)s.c_str());
}

//-----------------------------------
bool   Reduct_get()
{
        return Iz_reduct;
}

//------------------------------------
AnsiString* GetMesTest()
{
        return rduc_GetMesTest();
}

//--------------------------------------
void ClearMesText()
{
        rduc_ClearMesText();
}

//---------------------------------------
char* s_dll_src_Get_Version_From_Datchic(WORD *num)
{
WORD len;
static char *ptr;
        AnsiString data("MC_GET_VERSION" );
        if(If_svaz_udp == 1)
        {
                adapter_Send((char*)data.c_str());
#ifndef __BORLANDC__
                len =   (WORD)adapter_ResiveBuffer((BYTE**)&ptr, 10);
#else
				len =   adapter_ResiveBuffer((BYTE**)&ptr, 10);
#endif
        }
        else
        {
                ptr =  (char*)svaz.Send((char*)data.c_str());
                len = strlen(ptr);
        }
         *num = len;
        return ptr;
}

//------------------------------------
//
//----------------------------------
int GetParam(WORD *w)
{
WORD t  = Iz_pxs_per_string;

        flagMainApp = APP_NEYSTROEV;
        *w = t;
 return 0;
}



//-----------------------------------------------------------------------------------
void s_dll_src_GetIonError2048_2304(WORD * sr, WORD *ion, WORD pixel_per_string, DWORD len, DWORD *dw_ion, double *db_ion, bool iz_write)
{
DWORD dw_sm_koeff;
WORD *src = (sr);
WORD *t_ion  = ion;
WORD t;
WORD num = pixel_per_string == 2048 ? 8 : pixel_per_string == 2304 ? 9 : 4;
memset(dw_ion, 0, num * sizeof(DWORD));
dw_sm_koeff = 0L;
int i;
        for(i = 0; i < num; i++)
                db_ion[i] = 0.0;

        for(i = 0; i < num; i++)
        {
                for(int j = 0; j < (256 - _2048_ION_OFFS)  ; j++)
                {
                        t = *((t_ion + _2048_ION_OFFS) + num * j + i);
                        dw_ion[i] += t;
                }
                dw_sm_koeff += dw_ion[i];
        }

        for(i = 0; i < num; i++)
                db_ion[i] = ((double)dw_sm_koeff)/((double)(dw_ion[i] * num));


        if(iz_write == true)
        {
        DWORD i;
        int e = 0;
                for( i = 0; i < len; i++)
                {
                        e = i%num;
                        src[i] = (WORD)(((double)src[i]) * db_ion[e]);
                }
        }

return;
}

//-----------------------------------------------------
void WriteTooReg(WORD Param_index, char *Value)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif

        switch(Param_index)
        {
                case 11: Iz_ion_compencat = 0;
                default: break;
        }
        
 WriteParamTooReg(reg_str_resurs_param_name[Param_index], Value);
}

//-----------------------------------------------------------------
REGDATA* GetParamRegData()
{
        return regdata;
}

//---------------------------------------------------------------------------
WORD* s_dll_src_RotationHor(WORD *wtr, DWORD dLen)
{

WORD *wWtr = wtr;

WORD len_str = Iz_pxs_per_string;
WORD num_adc;
int ex, x;
if(len_str == 2304)
{
        num_adc = 9;  ex = 255; x = 128;
}
        else
        if(len_str == 2048)
        num_adc = 8;    ex = 255; x = 128;
                if(len_str == 1024)
                {
                num_adc = 8;     ex = 128; x = 64;
                }
                else
                if(len_str == 1152)
                {
                num_adc = 9;  ex = 128; x = 64;
                }

WORD num_str = (WORD)(dLen/len_str);
WORD *beg,*end;
        for(WORD y = 0; y < num_str; y++)
        {
         int n,  en;
                for(n = 0, en = num_adc - 1; n  < num_adc; n++, en--)
                {
                        for(x = 0, ex = 128/*255*/; x < 64/*128*/; x++, ex--)
                        {
                                beg = wWtr + y * len_str + x * num_adc    + n;
                                end = wWtr + y * len_str + ex * num_adc   + en;
                         WORD t = *beg;
                                *beg = *end;
                                *end =  t;
                        }
                }
        }


return wWtr;
}

//-----------------------------------------------------------------------------------------------
void s_dll_src_GetNoiseDataInNoiseBufer(WORD *wtr, DWORD *dw_noise_bufer, double * /*db_noise_bufer*/ ,
                                   WORD pixsels_per_string, MAIN_APP flag_main_app, DWORD pix_num)
{
WORD num = 0;

        switch(flag_main_app)
                {
                case   APP_N_PRIBOR: num = 100; break;
                case   APP_1024_MAIN:
                case   APP_2304_MAIN:
                case   APP_1152_MAIN:
                case   APP_576_MAIN:
                case   APP_2048_MAIN: num = 4; break;
                case   APP_NEYSTROEV:
                                        if(pix_num < 150000)
                                                num = (WORD)(pix_num/pixsels_per_string);
                                        else
                                        return;
                break;
                default: test_SetStringTooOutDebug3("ERROR in file_GetStatisticInNoiseBufer",
                                                        (unsigned char*)"ERROR of type_APP");break;
                }
WORD n = 0;;
        n =  pixsels_per_string == 2304 ? 9 :
             pixsels_per_string == 2048 ? 8 :
             pixsels_per_string == 1152 ? 9 :
             pixsels_per_string == 576 ? 9  :
             8;
        memset(dw_noise_bufer, 0, sizeof(DWORD) * 9);
        for(int i = 0; i <  pixsels_per_string * num; i +=  n)
                {
                for(int j = 0; j < n; j++)
                       dw_noise_bufer[j] += wtr[i + j];
                }

        for(int j = 0; j < n; j++)
                dw_noise_bufer[j] /= 256 * num;
}

//-----------------------------------------------------------------------------------
void  s_dll_src_SetContainerOfNoise(V_NOISE_DATA *v_data, DWORD *dw_in, DWORD *dw_out, DWORD *dw_ion,
        DWORD *dw_ion_bfr, DWORD *dw_ion_afte, MAIN_APP flag_main_app, DWORD pix_num)
{
 NOISE_DATA nd;


        if((flag_main_app == APP_NEYSTROEV) &&   (pix_num < 150000))
                return;
WORD n = 0;
        n =  Iz_pxs_per_string == 2304 ? 9 : Iz_pxs_per_string == 2048 ? 8 : 4;

        memcpy(nd.d_in, dw_in, sizeof(DWORD) * n);
        memcpy(nd.d_ns, dw_noise_bufer, sizeof(DWORD) * n);

        for(int i = 0; i < n; i++)
        {
                dw_ion[i] /= (256 - _2048_ION_OFFS);
                dw_ion_bfr[i] /= (256 - _2048_ION_OFFS);
                dw_ion_afte[i] /= (256 - _2048_ION_OFFS);
        }

        memcpy(nd.d_ion, dw_ion, sizeof(DWORD) * n);
        memcpy(nd.d_ion_bfr, dw_ion_bfr, sizeof(DWORD) * n);
        memcpy(nd.d_ion_afte, dw_ion_afte, sizeof(DWORD) * n);

        memcpy(nd.d_out, dw_out, sizeof(DWORD) * n);
        v_data->push_back(nd);
}

//-----------------------------------------------------------------------
HINSTANCE SetHinstance(HINSTANCE h)
{
m_hinst = h;
return m_hinst;
}


//------------------------------------------------------------------------
void s_dll_src_MatrixRevers(WORD *w, DWORD len, REGDATA *regdata)
{

int num_str = len / Iz_pxs_per_string;
int pixlen =  Iz_pxs_per_string;
WORD *wtr = w;
int i, j;
WORD n = 8;
WORD num = 256;
if(Iz_pxs_per_string == 2304)
{
    n = 9; num = 256;
}
else
        if(Iz_pxs_per_string == 2048)
        {
                n = 8; num = 256;
        }
                else
                if(Iz_pxs_per_string == 1024)
                {
                 n = 8; num = 128;
                }
                 else
                        if(Iz_pxs_per_string == 1152)
                        {
                        n = 9; num = 128;
                        }
                        else
                        {
                        test_SetStringTooOutDebug3("ERROR in MatrixRevers",
                                                  (unsigned char*)"ERROR of Iz_pxs_per_string");
                                                  return;
                        }

        for( j = 0; j < num_str; j ++)
        {
               wtr = w + j * pixlen;
               for(  i = 0; i < num/2; i++)
                {
                WORD b[9];
                WORD e[9];
                        memcpy(b, wtr + i * n, n * sizeof(WORD));
                        memcpy(e, wtr + num * n - n *( i + 1), n * sizeof(WORD));
                        memcpy(wtr + i * n, e, n * sizeof(WORD));
                        memcpy(&wtr[num * n - n *( i + 1)], b, n * sizeof(WORD));

                }
        }


}

//-----------------------------------------------------------------------
void s_dll_src_SendIcmpPacet()
{
#ifdef __BORLANDC__
	icmp_SendIcmpPacket();
#endif
}

//---------------------------------------------------------------------
DWORD s_dll_src_GetOfErrorConnect(DWORD *er)
{
*er = e_scl;
e_scl = 0;
typedef union _S
{
 DWORD dw;
 struct
 {
  WORD gl;
  WORD er;
 }st;
}S;
S s;
s.st.er = error;
s.st.gl = global;
return s.dw;
}

//-----------------------------------------------------------------
BYTE* s_dll_src_SetFlagLoadFromFile()
{
  flag_load_from_bin_file ^= 1;
  return ptr;
}

//-----------------------------------------------------------------
BYTE*   s_dll_src_SetFlagLoadFromFileOut()
{
  flag_load_from_bin_file_out ^= 1;
  return ptr;
}
//-------------------------------------------------------------
void     s_dll_src_SendBinning(WORD w)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif
//WORD len;
        test_SetStringTooOutDebug1("\r\n  In dll_src_SendBinning: " , w);
        if(If_svaz_udp == 0)
                svaz.Init(flagMainApp, Iz_binning);




        if((w < 1) || (w > 4))
        {
                 test_SetStringTooOutDebug3("ERROR  in s_dll_src_SetBinning",
                                                  (unsigned char*)"ERROR of Range");
                 return;

        }

        rduc_Init(Iz_pxs_per_string , Iz_l_front_reduct, Iz_r_front_reduct, (BYTE)w);

        AnsiString data("MC_SET_BINNING " );
#ifdef __BORLANDC__
        data += w;
#else
		char b[20];

        #ifndef unix
                itoa(w, b, 10);
        #else
                sprintf(b,"%X",w);
        #endif
		data += b;
#endif

        test_SetStringTooOutDebug1("Send Binning:",w);
        if(If_svaz_udp == 1)
        {
                adapter_Send((char*)data.c_str());
        }
        else
        {
                svaz.Send((char*)data.c_str());
        }
#ifdef __BORLANDC__
		AnsiString as = w;
#else
		AnsiString as;
		{
			char b[20];

            #ifndef unix
                itoa(w, b, 10);
            #else
                sprintf(b,"%X",w);
            #endif

			as = b;
		}
#endif
        regdata->iz_binning = (BYTE)w;
        WriteParamTooReg( (char*)"IZ_BINNING", (char*)as.c_str());
}

#ifdef MEDIAN1
//--------------------------------------------------------------------
//
//--------------------------------------------------------------------

bool MedianFiltrHorz1(WORD *scc,  unsigned long len ,WORD aprt)
{

WORD *sc = scc ;
if((aprt % 2) == 0)
    return false;
int med = aprt/2;
vector<WORD*> v_sort;
BYTE *end = (BYTE*)sc + len  ;
WORD *src;


int i;
for(i = 0; i < 9; i++)
{
src = sc + i;
WORD *wtr = src;
        for( ; wtr < (WORD*)end ; )
        {
                {

                        if(((WORD*)(wtr )) <= (WORD*)end)
                                v_sort.push_back(wtr);

                         sort(v_sort.begin(), v_sort.end());

                         WORD *w_wtr;


                         WORD t = ((WORD)v_sort.size()) > (med + 1) ?  (med + 1) : 0;

                         w_wtr = v_sort[((WORD)v_sort.size()) - t - 1];




                         if(w_wtr != NULL)
                                *src = *w_wtr;

                         if(aprt < (WORD)v_sort.size())
                                v_sort.erase(v_sort.begin());


                }
         wtr += 9;
        }

}

return true;
}
#endif

#ifdef MEDIAN2
//--------------------------------------------------------------------
//  GOOD
//--------------------------------------------------------------------

bool MedianFiltrHorz2(WORD *sc,  unsigned long len ,WORD aprt)
{
if((aprt % 2) == 0)
    return false;
    WORD *src = sc ;
int med = aprt/2;
vector<WORD> srt1;
BYTE *end = (BYTE*)src +  len ;
for(unsigned long j = 0; src < (WORD*)end   ; j++)
{
   for(WORD w = 0; srt1.size() < (size_t)aprt;)
   {
     srt1.push_back(src[w++]);

   }
   sort(srt1.end() - (aprt ) , srt1.end());
   *src++ = srt1[srt1.size() - (med + 1)];
   srt1.erase(srt1.begin(),srt1.end());
}
return true;
}
#endif


#ifdef __BORLANDC__
#pragma package(smart_init)
#endif

//-------------------------------------------------------------
//
//--------------------------------------------------------------
void SetBinningBuffer(WORD **buffer, unsigned long len, BYTE binning)
{
 WORD *wtr;
 DWORD dwnum;


if(Iz_pxs_per_string == 1152)
{
         switch(binning)
        {
                case 1:  wtr = SetBinning_1_Bufer1152(*buffer ,len, &dwnum); break;
                case 2:  wtr = SetBinning_2_Bufer1152(*buffer ,len, &dwnum); break;
                case 3:  wtr = SetBinning_3_Bufer1152(*buffer,len,  &dwnum); break;
                case 4:  wtr = SetBinning_4_Bufer1152(*buffer,len,  &dwnum); break;
                default:
                        test_SetStringTooOutDebug3("ERROR  in SetBinningBuffer 1152",
                                                  (unsigned char*)"ERROR of Range");
                 return;
        }
}
else
        if(Iz_pxs_per_string == 576)
        {
                 switch(binning)
                 {
                        case 1:  wtr = SetBinning_1_Bufer576(*buffer ,len, &dwnum); break;
                        case 2:  wtr = SetBinning_2_Bufer576(*buffer ,len, &dwnum); break;
                        case 3:  wtr = SetBinning_3_Bufer576(*buffer,len,  &dwnum); break;
                        default:
                                test_SetStringTooOutDebug3("ERROR  in SetBinningBuffer 576",
                                                  (unsigned char*)"ERROR of Range");
                 return;
                 }
        }
#ifdef __BORLANDC__
	wmemcpy(*buffer,wtr,dwnum);
#else
wchar_t  *wctr = (wchar_t*)*buffer;
	wmemcpy( wctr, (const wchar_t*)wtr, dwnum );
#endif
 delete[] wtr;
}

//-------------------------------------------------------------
// len == 1327104
// num_y == 1152
//--------------------------------------------------------------
WORD* SetBinning_1_Bufer1152(WORD *buffer,unsigned long len, DWORD *num)
{
WORD *wtr = new WORD[len + 2];
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t = 0;
WORD num_y =   (WORD)(len / 1152 );


 MINMAX  mn;
 DWORD delta;
 double kf;
                if(Iz_range == 1)
                {
                        mn = s_dll_tif_bufer_FindMinMax(buffer  + Iz_pxs_per_string * 30, len);
                        delta = mn.w_max - mn.w_min;
                        WORD w = W_range;
                        kf = ((double)delta) / ((double)w) ;
                }
                for(y = 0; y < num_y; y++)
                {
                          for(m = 0; m < 9; m++)
                          {
                                        for(x = 0; x < 128; x++)
                                        {
                                         t = *(buffer + 1152 * y + x * 9 + m);
                                                if(Iz_range == 1)
                                                {
                                                        t = t - mn.w_min;
                                                        t = (WORD)((double)t/kf);
                                                }
                                          *(wtr + y * 1152 + m * 128 + x) = t;
                                        }
                          }
                }
                *num = len;
return wtr;
}


//-------------------------------------------------------------
// len == 663552
// num_y == 576
//--------------------------------------------------------------
WORD* SetBinning_2_Bufer1152(WORD *buffer,unsigned long len, DWORD *num)
{

WORD num_y =   (WORD)(len / 1152 );
WORD *wtr = new WORD[len  + 2];
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
                 z = 0;     //  bool f = 0;
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
                        mn = s_dll_tif_bufer_FindMinMax(wtr  + Iz_pxs_per_string * 30, len - Iz_pxs_per_string * 30);
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
                *num = len;
return wtr;
}


//-------------------------------------------------------------
// len ==
// num_y ==
//--------------------------------------------------------------
WORD* SetBinning_3_Bufer1152(WORD *buffer,unsigned long len, DWORD *num)
{
WORD num_y =   (WORD)(len / 1152 );
WORD *wtr = new WORD[len  + 2];
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t[3] = {0,0,0};

WORD z = 0;
BYTE cn3;
DWORD dw_t;

 MINMAX  mn;
 DWORD delta;
 double kf;
                for(y = 0; y < num_y; y++)
                {
                 z = 0;   cn3 = 0;
                          for(m = 0; m < 9; m++)
                          {
                        //     cn3 = 0;
                                        for(x = 0; x < 128 ; x++)
                                        {
                                                        t[cn3] = *(buffer + 1152 * y + x * 9 + m);
                                                        if(cn3 == 2)
                                                        {
                                                          dw_t =   (t[0] + t[1] + t[2]);
                                                          if(Iz_div_16_9_4 == 1)
                                                          {
                                                                dw_t = dw_t/9;
                                                          }
                                                          else
                                                          if(Iz_div_4_3_2 == 1)
                                                          {
                                                                dw_t = dw_t/3;
                                                          }
                                                          *(wtr + y * 1152/3 + z) = (WORD)dw_t;

                                                        }

                                                        if(cn3 == 2)
                                                        {
                                                                cn3 = 0; z++;
                                                        }
                                                        else
                                                                cn3++;

                                        }
                          }
                }
                if(Iz_range == 1)
                {
                        mn = s_dll_tif_bufer_FindMinMax(wtr  + Iz_pxs_per_string * 30, len - Iz_pxs_per_string * 30);
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
                *num = len;
return wtr;
}
//-------------------------------------------------------------
// len == 331776
// num_y == 288
//--------------------------------------------------------------
WORD* SetBinning_4_Bufer1152(WORD *buffer,unsigned long len, DWORD *num)
{
WORD num_y =   (WORD)(len / 1152 );
WORD *wtr = new WORD[len  + 2];
memset(wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t[4] = {0,0,0,0};

WORD z = 0;
BYTE cn4;
//WORD dw_t;

 MINMAX  mn;
 DWORD delta;
 double kf;

                for(y = 0; y < num_y; y++)
                {
                 z = 0;
                          for(m = 0; m < 9; m++)
                          {
                             cn4 = 0;
                                        for(x = 0; x < 128; x++)
                                        {
                                                        t[cn4] = *(buffer + 1152 * y + x * 9 + m);
                                                        if(cn4 == 3)
                                                        {

                                                          WORD dw_t = (t[0] + t[1] + t[2] + t[3]);
                                                          if(Iz_div_16_9_4 == 1)
                                                          {
                                                                dw_t = dw_t/16;
                                                          }
                                                          else
                                                          if(Iz_div_4_3_2 == 1)
                                                          {
                                                                dw_t = dw_t/4;
                                                          }
                                                          *(wtr + y * 1152/4 + z++) = (WORD)dw_t;

                                                        }
                                                        if(cn4 == 3)
                                                                cn4 = 0;
                                                        else
                                                                cn4++;
                                        }
                          }
                }
                if(Iz_range == 1)
                {
                        mn = s_dll_tif_bufer_FindMinMax(wtr  + Iz_pxs_per_string * 30, len - Iz_pxs_per_string * 30);
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
                *num = len;
return wtr;
}


//-------------------------------------------------------------
void     s_dll_src_SetFilter(char w)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
   WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif
        AnsiString data("MC_SET_FILTER " );

        data += w;

        test_SetStringTooOutDebug1("Set HFiltr:",w == 0x30 ? 0 : 1);
        if(If_svaz_udp == 1)
        {
                adapter_Send((char*)data.c_str());
        }
        else
        {
                svaz.Send((char*)data.c_str());
        }
#ifdef __BORLANDC__
        AnsiString as = w;
#else
		AnsiString as;
		char b[20];

                #ifndef unix
                    itoa(w, b, 10);
                #else
                    sprintf(b,"%X",w);
                #endif

				as = b;
#endif
        Iz_h_filer = w == 0x30 ? 0 : 1;
        WriteParamTooReg( "IZ_HARD_FILTER", (char*)as.c_str());
}


//-------------------------------------------------------------
char     s_dll_src_GetFilter(char /*w*/)
{
char ret;
        ret = Iz_h_filer == 0 ? '0' : '1';
return ret;
}

//-----------------------------------------------------------------
 void dll_src_SetDiv_16_9_4(bool b)
 {
#ifdef    unix
     /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif
       Iz_div_16_9_4 = b;
       AnsiString as = b == 0 ? "0" : "1";
       WriteParamTooReg( "DIV_16_9_4", (char*)as.c_str());
 }

//---------------------------------------------------------------------------
void dll_src_SetDiv_4_3_2(bool b)
{
#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif
       Iz_div_4_3_2 = b;
       AnsiString as = b == 0 ? "0" : "1";
       WriteParamTooReg( "DIV_4_3_2", (char*)as.c_str());
}

//---------------------------------------------------------------------------
void dll_src_SetRange(bool b)
{

#ifdef    unix
    /*
    WRITEPARAMTOOREG WriteParamTooReg = (WRITEPARAMTOOREG)dlsym(reg_lib,"WriteParamTooReg");
    if((ptr_e = dlerror()) != NULL)
    {
        char e[1000];
        strcpy(e, ptr_e);
        fprintf(stderr,"%s",e);
     return;
    }
    */
    WRITEPARAMTOOREG WriteParamTooReg = g_WriteParamTooReg;
#endif


       Iz_range = b;
       AnsiString as = b == 0 ? "0" : "1";;
       WriteParamTooReg( "IZ_RANGE", (char*)as.c_str());
}


WORD* SetBinning_1_Bufer576(WORD *buffer,unsigned long len, DWORD *num)
{
WORD *wtr = new WORD[len + 2];
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t = 0;
WORD num_y =   (WORD)(len / 576 );


 MINMAX  mn;
 DWORD delta;
 double kf;
                if(Iz_range == 1)
                {
                        mn = s_dll_tif_bufer_FindMinMax(buffer  + Iz_pxs_per_string * 30, len);
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


//-------------------------------------------------------------
// len == 663552
// num_y == 576
//--------------------------------------------------------------
WORD* SetBinning_2_Bufer576(WORD *buffer,unsigned long len, DWORD *num)
{

WORD num_y =   (WORD)(len / 576 );
WORD *wtr = new WORD[len  + 2];
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
                        mn = s_dll_tif_bufer_FindMinMax(wtr  + Iz_pxs_per_string * 30, len - Iz_pxs_per_string * 30);
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
                *num = len;
return wtr;
}


//-------------------------------------------------------------
// len ==
// num_y ==
//--------------------------------------------------------------
WORD* SetBinning_3_Bufer576(WORD *buffer,unsigned long len, DWORD *num)
{
WORD num_y =   (WORD)(len / 1152);
WORD *wtr = new WORD[len  + 2];
memset((void*)wtr, 0, len * sizeof(WORD) + 2);
WORD x = 0;
WORD y = 0;
BYTE m = 0;
WORD t[3] = {0,0,0};

WORD z = 0;
BYTE cn3;
DWORD dw_t;

 MINMAX  mn;
 DWORD delta;
 double kf;
                for(y = 0; y < num_y; y++)
                {
                 z = 0;   cn3 = 0;
                          for(m = 0; m < 9; m++)
                          {
                                        for(x = 0; x < 128 ; x++)
                                        {
                                                        t[cn3] = *(buffer + 1152 * y + x * 9 + m);
                                                        if(cn3 == 2)
                                                        {
                                                          dw_t =   (t[0] + t[1] + t[2]);
                                                          if(Iz_div_16_9_4 == 1)
                                                          {
                                                                dw_t = dw_t/9;
                                                          }
                                                          else
                                                          if(Iz_div_4_3_2 == 1)
                                                          {
                                                                dw_t = dw_t/3;
                                                          }
                                                          *(wtr + y * 1152/3 + z) = (WORD)dw_t;

                                                        }

                                                        if(cn3 == 2)
                                                        {
                                                                cn3 = 0; z++;
                                                        }
                                                        else
                                                                cn3++;

                                        }
                          }
                }
                if(Iz_range == 1)
                {                    
                        mn = s_dll_tif_bufer_FindMinMax(wtr  + Iz_pxs_per_string * 30, len - Iz_pxs_per_string * 30);
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
                *num = len;
return wtr;
}

//--------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------
#ifdef unix
void Sleep(long l)
{
    while(l > 0)
        l--;
}

#endif
