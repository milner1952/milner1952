#include "tld_16.h"
#include "qtld_16widget.h"



//---------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------
TLD_16::TLD_16(QLibrary *lib_dll_udp, QString *e ,QWidget *pr) : /*QScrollArea(pr)//*/QWidget(pr)
{
    parent = pr;
    memset(bufer, 0, 2048 * 2048 * 16);
    typedef void (*TINIT)(HWND Owner,wBegin b,wWork w ,wEnd e);
    typedef bool (*AFTEINITONCREATE)();
    typedef char* (*GETVERSIONDLL)();
    dll_src_SetMessError = (DLL_SRC_SETMESSERROR)lib_dll_udp->resolve("dll_src_SetMessError");
    if(!dll_src_SetMessError)
    {
        exit(1);
    }

TINIT Init = (TINIT)lib_dll_udp->resolve("Init");
    if(!Init)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());

    }

AFTEINITONCREATE AfteInitOnCreate = (AFTEINITONCREATE)lib_dll_udp->resolve("AfteInitOnCreate");
    if(!AfteInitOnCreate)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }

    Dll_Close = (DLL_CLOSE)lib_dll_udp->resolve("Dll_Close");
    if(!Dll_Close)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }

    GetBufer = (GETBUFER)lib_dll_udp->resolve("GetBufer");
    if(!GetBufer)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }

GETVERSIONDLL GetVersionDll = (GETVERSIONDLL)lib_dll_udp->resolve("GetVersionDll");
    if(!GetVersionDll)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }


    getPixelPerString = (GETPIXSELPERSTRING)lib_dll_udp->resolve("GetPixelPerString");
    if(!getPixelPerString)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }

    SetDac = (SETDAC)lib_dll_udp->resolve("SetDac");
    if(!SetDac)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }

    SetModeRun = (SETMODERUN)lib_dll_udp->resolve("SetModeRun");
    if(!SetModeRun)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }


    s_dll_src_PutGaneMtr = (S_DLL_SRC_PUTGANEMTR)lib_dll_udp->resolve("s_dll_src_PutGaneMtr");
    if(!s_dll_src_PutGaneMtr)
    {
         *e = lib_dll_udp->errorString();
         dll_src_SetMessError("main_Init",e->toStdString().c_str());
    }


    Init(0,0,0,0);
    AfteInitOnCreate();
    const char *Version = (const char*)GetVersionDll();
    QString qs(Version);

    parent->setWindowTitle(qs);
    parent->resize(getPixelPerString() + 5,getPixelPerString() + 5);

    psigMapper = new QSignalMapper(this);
}

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void TLD_16::slotNoise()
{
        ((TLD_16Widget*)w_paint)->Noise_img(false);
DWORD len = getPixelPerString() * 25 * 2;
        GetData(len, true);
}

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void TLD_16::slotImages()
{
         ((TLD_16Widget*)w_paint)->Noise_img(true);
DWORD len = getPixelPerString() * getPixelPerString() * 2;
        GetData(len, false);
}

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
void TLD_16::GetData(DWORD len, bool b)
{

DWORD num;
int i = 10;
    do{
    num = GetBufer(&ptr, len, b);
    i--;
        if(i == 0)
        {
            string s ="ERROR: REQ = ";
            s += len;
            string r = "RET = ";
            r += num;
            dll_src_SetMessError(s.c_str(),r.c_str());
            return;
        }
    }while(num != len);
    memcpy(bufer,ptr,num);
w_paint->repaint();

}

//-----------------------------------------------------------------------
void TLD_16::slotDisplay()
{


}

//-----------------------------------------------------------------------
void TLD_16::slotTimer()
{
     ((TLD_16Widget*)w_paint)->Noise_img(true);
    ((TLD_16Widget*)w_paint)->run_Timer();

}

//------------------------------------------------------------------------
void TLD_16::slotRbtClicked(int i)
{
  SetDac(i + 1);
}

//------------------------------------------------------------------------
void TLD_16::slotMatrix(bool i)
{
   SetModeRun(MODE(i));
}


//--------------------------------------------------------------------
void TLD_16::slotStringNomber(int i)
{
    ((TLD_16Widget*)w_paint)->StringNomber((WORD)i);
    w_paint->repaint();
}

//---------------------------------------------------------------------
void TLD_16::slotGane(int i)
{
    ((TLD_16Widget*)w_paint)->Gane((WORD)i);
    s_dll_src_PutGaneMtr((BYTE)i);
    w_paint->repaint();
}
//--------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------
void TLD_16::slotRuberLine0(int i)
{
    ((TLD_16Widget*)w_paint)->RubberLine0((WORD)i);
    char b[10];
    memset(b,0,10);
    sprintf(b,"%d",i);
    ((TLD_16Widget*)w_paint)->lb_psbr0->setText(b);
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
w_paint->repaint();
}
//---------------------------------------------------------------------
void TLD_16::slotScale(int i)
{
    ((TLD_16Widget*)w_paint)->Scale((WORD)i);
    ((TLD_16Widget*)w_paint)->sa->verticalScrollBar()->setRange (1, (65536/i - GetPixelPerString()));
    ((TLD_16Widget*)w_paint)->sa->verticalScrollBar()->setSingleStep(i);

    ((TLD_16Widget*)w_paint)->psbr0->setSingleStep(i);
    ((TLD_16Widget*)w_paint)->psbr1->setSingleStep(i);
    w_paint->repaint();
}
//-------------------------------------------------------------------------------
void TLD_16::slotVertScrullBar(int i)
{
    ((TLD_16Widget*)w_paint)->Base(i);
WORD sc =  ((TLD_16Widget*)w_paint)->Scale();
    char b[10];
    memset(b,0,10);
    sprintf(b,"%d",((i -1) * sc));
    ((TLD_16Widget*)w_paint)->lb_Base->setText(b);
    w_paint->repaint();
}
//------------------------------------------------------------------------------
void TLD_16::slotImagesPressed()
{
//((TLD_16Widget*)w_paint)->pcmdImages->setEnabled(false);

}
