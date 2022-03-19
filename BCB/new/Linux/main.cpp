// ======================================================================
//  main.cpp
// ======================================================================
//                   This file is a part of the book 
//             "Qt 5.3 Professional programming with C++"
// ======================================================================
//  Copyright (c) 2014 by Max Schlee
//
//  Email : Max.Schlee@neonway.com
//  Blog  : http://www.maxschlee.com
//
//  Social Networks
//  ---------------
//  FaceBook : http://www.facebook.com/mschlee
//  Twitter  : http://twitter.com/Max_Schlee
//  2Look.me : http://2look.me/NW100003
//  Xing     : http://www.xing.com/profile/Max_Schlee
//  vk.com   : https://vk.com/max.schlee
// ======================================================================

#include <QtWidgets>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dlfcn.h>



#include <../../SRC/BCB/dll_src.h>
#include <../../SRC/BCB/reg.h>


// ----------------------------------------------------------------------
char bufer[50000];
char *ptr_e;
int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QLabel       lbl_good("good");
    QLabel       lbl_error("error");

    QLibrary     lib_dll_udp("./s_dll_udp");
    QString e;

    typedef void (*TINIT)(HWND Owner,wBegin b,wWork w ,wEnd e);
    TINIT Init = (TINIT)lib_dll_udp.resolve("Init");    
    if(!Init)
    {
         e = lib_dll_udp.errorString();
         lbl_error.show();
    }
    else
    {
    Init(0,0,0,0);
    typedef bool (*AFTEINITONCREATE)();
        AFTEINITONCREATE AfteInitOnCreate = (AFTEINITONCREATE)lib_dll_udp.resolve("AfteInitOnCreate");
        if(!AfteInitOnCreate)
        {
             e = lib_dll_udp.errorString();
             lbl_error.show();
        }
        else
        lbl_good.show();
    }
    int ret = app.exec();
    return ret;
}



// ----------------------------------------------------------------------

/*
const char *RegKey = "Software\\N_PRIBOR_P_KAMERA_2048_DLL_SRK\\ParamList";
const char *   reg_str_resurs_param_name[] =      {
                                                    "TIMER_PERIOD","GANE","PIXELS_PER_STRING","NORM_DATA",
                                                    "SAVE_IN_BUFFER_IN_FILE_OR_READ_IT","5MC_PER_STRING",
                                                    "IZ_LOG_DEBUG", "IZ_REDUCT", "IZ_SVAZ_UDP",
                                                    "MTR_GANE", "IN_FILE_OF_BUFFER","IF_ERROR_ION_COMPENSATION",
                                                    "IF_DEL_IGLS","IF_OLD_1024","FLAG_16_14", "LEFT_REDUCT_FRONT",
                                                    "RIGHT_REDUCT_FRONT", "IF_ROTATION", "WITH_NOISE_STATISTICS",
                                                    "WITH_MATRICES", "TIME_REPIT", "REVERS_MATRIX", "IZ_DAC", "IZ_BINNING","IZ_HARD_FILTER",
                                                    "DIV_16_9_4","DIV_4_3_2","IZ_RANGE","REDUKTION_TO_RANGE"
                                                  };

TRegistry *reg;
REGDATA *regdata;
char bufer[50000];
int main(int argc, char** argv)
{
    DWORD len = 23040;
    DWORD num = 0;

    QApplication app(argc, argv);
    QLabel       lbl_good("good");
    QLabel       lbl_error("error");

    QLibrary     lib_reg("./p_reg_lib");
    QLibrary     lib_adapter("./p_dll_adapter");

    QString e;

  //  Init(0);

//reg
    typedef TRegistry* (*TINITREG) (const char*, char**,int);
    typedef REGDATA*   (*TREADPARAMTOREG) ();
    typedef void  (*DESTROY)();
//Adapter
    typedef int  (*ADAPTER_INIT)(bool iz_connect, double t_repit, WORD pix, unsigned int debug);
    typedef  void     (*ADAPTER_SEND)(char *data);

    typedef  DWORD  (*ADAPTER_RESIVE_BUFFER)(BYTE **ptr, DWORD len);
    typedef  void   (*ADAPTER_DELETE)();







//reg
    TINITREG InitReg = (TINITREG)(lib_reg.resolve("InitReg"));
    TREADPARAMTOREG ReadParamTooReg = (TREADPARAMTOREG)(lib_reg.resolve("ReadParamTooReg"));
    DESTROY DestroyReg = lib_reg.resolve("DestroyReg");
    if(!InitReg)
    {

         e = lib_reg.errorString();
    }

    if (InitReg) {
            reg = InitReg((char*)RegKey,(char**)reg_str_resurs_param_name , 25 + 4);
            regdata = ReadParamTooReg();
    }

//adapter
   ADAPTER_INIT adapter_init =  (ADAPTER_INIT)(lib_adapter.resolve("adapter_Init"));
   ADAPTER_SEND adapter_Send =    (ADAPTER_SEND)lib_adapter.resolve("adapter_Send");
   ADAPTER_RESIVE_BUFFER adapter_ResiveBuffer = (ADAPTER_RESIVE_BUFFER)lib_adapter.resolve("adapter_ResiveBuffer");
   ADAPTER_DELETE adapter_Delete = (ADAPTER_DELETE)lib_adapter.resolve("adapter_Delete");
   if(!adapter_init)
   {
        e = lib_adapter.errorString();
   }
   else
   {

      if(!adapter_init(1, 1000, 1152, 1) )
      {
          memset(bufer, 0, 50000);
          adapter_Send((char*)"GET 23040");
          BYTE *ptr = (BYTE*)&bufer[0];
                len = 23040;
                num = 0;
          do{
             num = adapter_ResiveBuffer(&ptr, len);
             len -= num;
          }
          while(len > 0);
      }
      else
      lbl_error.show();
   }

    lbl_good.show();

    int ret = app.exec();

    adapter_Delete();
    if(DestroyReg)
            DestroyReg();

    return ret;
}
*/
