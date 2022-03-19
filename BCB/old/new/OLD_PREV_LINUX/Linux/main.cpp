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
#include <../p_reg_dll/reg.h>

// ----------------------------------------------------------------------


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
int main(int argc, char** argv)
{
#ifdef    unix
    QApplication app(argc, argv);
    QLabel       lbl1("good");
    QLabel       lbl2("error");
#endif
    QLibrary     lib("./p_reg_lib");
    QString e;

    typedef TRegistry* (*TINITREG) (const char*, char**,int);
    typedef REGDATA*   (*TREADPARAMTOREG) ();
    typedef void  (*DESTROY)();

    TINITREG InitReg = (TINITREG)(lib.resolve("InitReg"));
    TREADPARAMTOREG ReadParamTooReg = (TREADPARAMTOREG)(lib.resolve("ReadParamTooReg"));
    DESTROY DestroyReg = lib.resolve("DestroyReg");
    if(!InitReg)
    {

         e = lib.errorString();
    }

    if (InitReg) {
            reg = InitReg((char*)RegKey,(char**)reg_str_resurs_param_name , 25 + 4);
            regdata = ReadParamTooReg();

            lbl1.show();
    }
    else
    lbl2.show();


    int ret = app.exec();

    if(DestroyReg)
            DestroyReg();

    return ret;
}
