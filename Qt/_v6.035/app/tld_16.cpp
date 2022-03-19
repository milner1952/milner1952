#include "tld_16.h"
#include "qtld_16widget.h"
#include "reg_app_data.h"
#include "reduct_test.h"
#include <QMessageBox>

extern const char *res_str_no_s_filtr;
extern const char *res_str_s_filtr;


extern     WRITEPARAMTOOREG  g_WriteParamTooReg;
extern const char *   reg_str_resurs_param_app[];

//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------
TLD_16::TLD_16(string *dir_work, string *patch_ini, QLibrary *dll_udp, QString *e ,QWidget *pr) : QWidget(pr)
{
    power = 0;
    lib_dll_udp = dll_udp;
    f_timer_run = 0;
    global = 0;
    parent = pr;
    memset(bufer, 0, 2048 * 2048 * 16);
    typedef void (*TINIT)(const char*, const char*);
    typedef bool (*AFTEINITONCREATE)();
    typedef char* (*GETVERSIONDLL)();



TINIT Init = (TINIT)lib_dll_udp->resolve("Init");
    if(!Init)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());

    }

AFTEINITONCREATE AfteInitOnCreate = (AFTEINITONCREATE)lib_dll_udp->resolve("AfteInitOnCreate");
    if(!AfteInitOnCreate)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

    Dll_Close = (DLL_CLOSE)lib_dll_udp->resolve("p_dll_Close");
    if(!Dll_Close)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

    GetBufer = (GETBUFER)lib_dll_udp->resolve("GetBufer");
    if(!GetBufer)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

GETVERSIONDLL GetVersionDll = (GETVERSIONDLL)lib_dll_udp->resolve("p_dll_GetVersionDll");
    if(!GetVersionDll)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }


    getPixelPerString = (GETPIXSELPERSTRING)lib_dll_udp->resolve("p_dll_GetPixelPerString");
    if(!getPixelPerString)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

    SetDac = (SETDAC)lib_dll_udp->resolve("p_dll_SetDac");
    if(!SetDac)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

    SetModeRun = (SETMODERUN)lib_dll_udp->resolve("p_dll_SetModeRun");
    if(!SetModeRun)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }


    dll_PutGaneMtr = (DLL_PUTGANEMTR)lib_dll_udp->resolve("p_dll_PutGaneMtr");
    if(!dll_PutGaneMtr)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

    Reduct_set = (REDUCT_SET)lib_dll_udp->resolve("p_dll_Reduct_set");
    if(!Reduct_set)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

    GetParamRegData = (GETPARAMREGDATA)lib_dll_udp->resolve("p_dll_GetParamRegData");
    if(!GetParamRegData)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

    dll_SendBinning =  (DLL_SENDBINNING)lib_dll_udp->resolve("p_dll_SendBinning");
    if(!dll_SendBinning)
    {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
    }

     p_dll_src_SetFilter = (P_DLL_SRC_SERFILTER)lib_dll_udp->resolve("p_dll_SetFilter");
     if(!p_dll_src_SetFilter)
     {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
     }


     setFlagMainAPP = (SETFLAGMAINAPP)lib_dll_udp->resolve("setFlagMainAPP");
     if(!setFlagMainAPP)
     {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
     }

     p_dll_GetChError = (P_DLL_GETCHERROR)lib_dll_udp->resolve("p_dll_GetChError");
     if(!p_dll_GetChError)
     {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
     }

     p_dll_PutTimePerString = (P_DLL_PUTTIMEPERSTRING)lib_dll_udp->resolve("p_dll_PutTimePerString");
     if(!p_dll_PutTimePerString)
     {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
     }

     p_dll_SetSFilter = (P_DLL_SET_S_FILTER)lib_dll_udp->resolve("p_dll_SetSFilter");
     if(!p_dll_SetSFilter)
     {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
     }

     p_dll_SendIcmp = (P_DLL_SEND_ICMP)lib_dll_udp->resolve("p_dll_SendIcmp");
     if(!p_dll_SendIcmp)
     {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
     }

     p_dll_GetError = (P_DLL_GETERROR)lib_dll_udp->resolve("p_dll_GetError");
     if(!p_dll_GetError)
     {
         *e = lib_dll_udp->errorString();
         SetMessError("main_Init",(char*)e->toStdString().c_str());
     }

    //Init("C:\\Users\\Public\\");
     if(dir_work == NULL)
        {
                 Init(NULL, 0);
        }
        else
        {
                //Init(dir_work->c_str(), "C:\\Users\\Public\\");
                Init(dir_work->c_str(), patch_ini->c_str());
        }

char *t = NULL;
     if(p_dll_GetError(&t))
     {
         SetMessError(t);
         exit(2);
     }

#ifndef unix
int i = 5;
#else
int i = 1;
#endif
    do
    {
        power = p_dll_SendIcmp();
        if(power == 1)
        break;
        i--;
    }
    while(i > 0);
    if(i == 0)
        exit(1);


    AfteInitOnCreate();
    if(p_dll_GetError(&t))
    {
        SetMessError(t);
        exit(3);
    }



    setFlagMainAPP(true);

    const char *Version = (const char*)GetVersionDll();
QString qs(Version);
    parent->setWindowTitle(qs);
    if(getPixelPerString() == 2304)
        parent->resize(getPixelPerString()/2 + 5,getPixelPerString()/4 + 5);
       else
    if(getPixelPerString() == 2048)
        parent->resize(getPixelPerString()/2 + 5,getPixelPerString()/4 + 5 );
            else
                if(getPixelPerString() == 576)
                    parent->resize(getPixelPerString() + 5,getPixelPerString() + 5);
                    else
                        if(getPixelPerString() == 1152)
                            parent->resize(getPixelPerString() + 5,getPixelPerString()/20 + 5);

    psigMapper = new QSignalMapper(this);


   // SetMessError("aaa","bbb");

}
//---------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------
void TLD_16::initReducTest()
{
#ifdef USE_REDUCT_TEST
    reduct_test_Init(this, lib_dll_udp, ((TLD_16Widget*)w_paint)->Reg_app_data());
#endif
}

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void TLD_16::slotNoise()
{
 ((TLD_16Widget*)w_paint)->lb_Loading->hide();
        ((TLD_16Widget*)w_paint)->Noise_img(false);
DWORD len = getPixelPerString() * 26 * 2;
    ((TLD_16Widget*)w_paint)->setFixedHeight(65536/ ((TLD_16Widget*)w_paint)->Scale());
#ifdef USE_REDUCT_TEST
        reduc_test_SetNposstringnoise(((TLD_16Widget*)w_paint)->Reg_app_data()->n_pos_string_noise );
#endif
        GetData(len, true);
}

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void TLD_16::slotImages()
{
 ((TLD_16Widget*)w_paint)->lb_Loading->show();

        ((TLD_16Widget*)w_paint)->Noise_img(true);
DWORD len = (getPixelPerString() * getPixelPerString()) * 2;

WORD num;
       if(GetPixelPerString() == 2304)
         num = GetPixelPerString()/2;
       else
          if(GetPixelPerString() == 2048)
                 num = GetPixelPerString()/2 + 16;
       else
         num = GetPixelPerString();
        ((TLD_16Widget*)w_paint)->setFixedHeight(2048 - num);
        GetData(len, false);
        ((TLD_16Widget*)w_paint)->prevDrawImg();
         w_paint->repaint();
}

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void TLD_16::GetData(DWORD len, bool b)
{

DWORD num;
int i = 10;
        global++;

    do{
    num = GetBufer(&ptr, len, b);
    i--;
        if(i == 0)
        {
            string s ="ERROR: REQ = ";
            s += len;
            string r = "RET = ";
            r += num;
            SetMessError(s.c_str(),(char*)r.c_str());
            return;
        }
    }while(num != len);

    memcpy(bufer,ptr,num);    


char e[20];
char g[20];
            memset(g,0,20);
            sprintf(g,"%d",global);
            memset(e,0,20);
            char *t;
            sprintf(e,"%ld", (p_dll_GetChError() + p_dll_GetError(&t)));
            strcat(g,": ");
            strcat(g,e);
            ((TLD_16Widget*)w_paint)->lb_Error->setText(g);

w_paint->repaint();
}



//------------------------------------------------------------------------
void TLD_16::slotRbtClicked(int i)
{
    SetDac(i);
}

//------------------------------------------------------------------------
void TLD_16::slotMatrix(bool b)
{
   ((TLD_16Widget*)w_paint)->pchkMatrix->setText(b == 0 ? "C матрицами" : "Без матриц");
   SetModeRun(MODE(b));
}


//--------------------------------------------------------------------
void TLD_16::slotStringNomber(int i)
{
    ((TLD_16Widget*)w_paint)->StringNomber((WORD)i);
    char b[10];
    memset(b,0,10);
    sprintf(b,"%d",i);
    string s(b);
    g_WriteParamTooReg(((TLD_16Widget*)w_paint)->Reg(), reg_str_resurs_param_app[EN_N_POS_STRING_NOISE], (char*)s.c_str());
    w_paint->repaint();
}

//---------------------------------------------------------------------
void TLD_16::slotGane(int i)
{
    dll_PutGaneMtr((BYTE)i);

    w_paint->repaint();

}

//---------------------------------------------------------------------
void TLD_16::slotRuberLine0(int i)
{
    ((TLD_16Widget*)w_paint)->RubberLine0((WORD)i);
char b[10];
    memset(b,0,10);
    sprintf(b,"%d",i);
    ((TLD_16Widget*)w_paint)->lb_psbr0->setText(b);
    string s(b);
    g_WriteParamTooReg(((TLD_16Widget*)w_paint)->Reg(), reg_str_resurs_param_app[EN_RUBBER_LINE0], (char*)s.c_str());
w_paint->repaint();
}
//---------------------------------------------------------------------
void TLD_16::slotRuberLine1(int i)
{
    ((TLD_16Widget*)w_paint)->RubberLine1((WORD)i);
char b[10];
    memset(b,0,10);
    sprintf(b,"%d",i);    
    ((TLD_16Widget*)w_paint)->lb_psbr1->setText(b);
    string s(b);
    g_WriteParamTooReg(((TLD_16Widget*)w_paint)->Reg(), reg_str_resurs_param_app[EN_RUBBER_LINE1], (char*)s.c_str());
w_paint->repaint();
}
//---------------------------------------------------------------------
void TLD_16::slotScaleY(int i)
{

    ((TLD_16Widget*)w_paint)->Scale((WORD)i);
    ((TLD_16Widget*)w_paint)->setFixedHeight(65536/ ((TLD_16Widget*)w_paint)->Scale());
    ((TLD_16Widget*)w_paint)->psbr0->setSingleStep(i);
    ((TLD_16Widget*)w_paint)->psbr1->setSingleStep(i);
    w_paint->repaint();

char b[10];
    memset(b,0,10);
    sprintf(b,"%d", i);
    string s(b);

    g_WriteParamTooReg(((TLD_16Widget*)w_paint)->Reg(), reg_str_resurs_param_app[EN_ZOOM_Y], (char*)s.c_str());
}
//-------------------------------------------------------------------------------
void TLD_16::slotVertScrullBar(int i)
{    
    ((TLD_16Widget*)w_paint)->Base(i);
WORD sc =  ((TLD_16Widget*)w_paint)->Scale();
char b[10];
    memset(b,0,10);
    sprintf(b,"%d",(i * sc));
    ((TLD_16Widget*)w_paint)->lb_Base->setText(b);
    memset(b,0,10);
    sprintf(b,"%d",i);
string s(b);
    g_WriteParamTooReg(((TLD_16Widget*)w_paint)->Reg(), reg_str_resurs_param_app[EN_SCRULBARBAZA], (char*)s.c_str());
     w_paint->repaint();
}

//------------------------------------------------------------------------------
void TLD_16::slotImagesPressed()
{
    ((TLD_16Widget*)w_paint)->prevRstImg();
    w_paint->repaint();
}

//------------------------------------------------------------------------
void TLD_16::slotReduct(bool b)
{
    ((TLD_16Widget*)w_paint)->pchkReduct->setText(b == 1 ? "Со сведением" : "Без сведения");
    reduct_Set(b);
}

//--------------------------------------------------------------------------
void TLD_16::slotBinning(int i)
{
     dll_SendBinning(i);
}

//---------------------------------------------------------------------
void TLD_16::slotFilter(bool b)
{
    p_dll_src_SetFilter(b);
    ((TLD_16Widget*)w_paint)->pchkFilter->setText(b == 1 ? "С фильтром" : "Без фильтра");
}

//-------------------------------------------------------------------------------------
 TLD_16::~TLD_16()
 {

    TRegistry *reg = ((TLD_16Widget*)w_paint)->Reg();
    ((TLD_16Widget*)w_paint)->DestroyReg(reg);
    Dll_Close();
 }
//-----------------------------------------------------------------------------------------
 void TLD_16::slotScaleX(int i)
 {
   ((TLD_16Widget*)w_paint)->ScaleX((WORD)i);
     if((GetPixelPerString() == 2048) || (GetPixelPerString() == 2304))
     {

         if(i == 2)
         {
            ((TLD_16Widget*)w_paint)->sa->horizontalScrollBar()->setRange (1, 2);
            ((TLD_16Widget*)w_paint)->setFixedWidth(GetPixelPerString()/2);
         }
         else
         {
            ((TLD_16Widget*)w_paint)->sa->horizontalScrollBar()->setRange (1, 1024);
            ((TLD_16Widget*)w_paint)->setFixedWidth(GetPixelPerString());
         }
     }

  char b[10];
     memset(b,0,10);
     sprintf(b,"%d", i);
  string s(b);
    g_WriteParamTooReg(((TLD_16Widget*)w_paint)->Reg(), reg_str_resurs_param_app[EN_ZOOM_X], (char*)s.c_str());

    ((TLD_16Widget*)w_paint)->prevDrawImg();

    w_paint->repaint();
 }

 //--------------------------------------------------------------------------------------------
 void TLD_16::slotHorzScrullBar(int i)
 {
 ((TLD_16Widget*)w_paint)->Reg_app_data()->scrullbazaX = i;
     w_paint->repaint();
 }



 //-----------------------------------------------------------------------
 //-----------------------------------------------------------------------
 void TLD_16::slotTimerPressed()
 {
    ((TLD_16Widget*)w_paint)->Noise_img(true);
    if(f_timer_run == 0)
    {
        ((TLD_16Widget*)w_paint)->prevRstImg();
        w_paint->repaint();
     WORD str = regdata->timer_period;
        ((TLD_16Widget*)w_paint)->pTimer->start(str );
    }
    else
        ((TLD_16Widget*)w_paint)->pTimer->stop();
        f_timer_run ^= 1;
 }

 //--------------------------------------------------------------
 //-------------------------------------------------------------
 void TLD_16::slotTimeOut()
 {
     ((TLD_16Widget*)w_paint)->pTimer->stop();

     if(f_timer_run == 1)
     {
#ifndef unix
         ::Sleep(2000);
#else
          sleep(2);
#endif
     }
     ((TLD_16Widget*)w_paint)->prevRstImg();
     w_paint->repaint();
  WORD n = getPixelPerString();
  DWORD len = n * n * 2;
     GetData(len, false);
     ((TLD_16Widget*)w_paint)->prevDrawImg();
     w_paint->repaint();
     if(f_timer_run == 1)
     {

         WORD str = regdata->timer_period;
        ((TLD_16Widget*)w_paint)->pTimer->start(str );
     }
 }

//---------------------------------------------------------------
//-------------------------------------------------------------
void TLD_16::slotTimePerString(bool b)
{
string *s = ((TLD_16Widget*)w_paint)->Res_str_time1152();
((TLD_16Widget*)w_paint)->pchkTimePerString->setText(s[b ^ 1].c_str());
p_dll_PutTimePerString((TIMEPERSTRING)b);
}

//---------------------------------------------------------------
//-------------------------------------------------------------
void TLD_16::slotSFilter(int v)
{
    if((v % 2) == 0)
    {
        ((TLD_16Widget*)w_paint)->pssbSFilter->setValue( v + 1 );
    }
    p_dll_SetSFilter(v);

    if(v > 1)
        ((TLD_16Widget*)w_paint)->lb_TextSFilter->setText(res_str_s_filtr);
    else
        ((TLD_16Widget*)w_paint)->lb_TextSFilter->setText(res_str_no_s_filtr);

}

//---------------------------------------------------------------
//--------------------------------------------------------------
void TLD_16::slotSendIcmp()
{
bool b = p_dll_SendIcmp();
((TLD_16Widget*)w_paint)->lb_Power->setText(b == 0 ? "Выкл":"Вкл");
}

//-----------------------------------------------------------------
//-----------------------------------------------------------------
void TLD_16::SetMessError(const char* mes)
{
    QMessageBox::warning(this,"test_app", mes );
    exit(1);
}


//-----------------------------------------------------------------
//-----------------------------------------------------------------
void TLD_16::SetMessError(const char* lib,const char* mes)
{
    QMessageBox::warning(this,lib, mes );
    exit(1);
}

