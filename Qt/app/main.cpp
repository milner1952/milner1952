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
#include <QCoreApplication>
#include <QtWidgets>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include <winuser.h>

#ifdef unix
    #include <unistd.h>
    #include <dlfcn.h>
    #include <dlfcn.h>
    #include <errno.h>
#endif

#include <stdio.h>

#include <../../SRC/Qt/udp/dll_src.h>
#include "../../SRC/Qt/reg/reg.h"

#include "reg_app_data.h"
#include "tld_16.h"
#include "qtld_16widget.h"


#include <fcntl.h>


QWidget* main_Init(string *dir_work, string *patch_ini,QWidget*, QScrollArea**, QClipboard* pcb, QString dir_app);

#ifndef unix
DWORD  AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW);
#endif


bool flag_Zoom_tiff_data;
//QString Dir;
// ----------------------------------------------------------------------

TLD_16 *sensor;
//--------------------------------------------------------------------
//При наличии опции командной строки -z, -Z происходит установка флага
//flag_Zoom_tiff_data = 1; что дает возможность просматривать слабоконтрастные
//tiff-файлы программой просмотра фотографий Windows
//--------------------------------------------------------------------
int main(int argc, char** argv)
{

    if(argc > 1)
    {

#ifndef unix
char svers[10];
    if((!strcmp(argv[1], "-v")) || (!strcmp(argv[1], "-V")))
    {
            memset(svers, 0,10);
    long v = QT_VERSION;
            sprintf(svers,"%X",v);
    wchar_t w_buf[2048];
            wmemset(w_buf, 0, 2048 );
            AnsiToUnicode(svers,w_buf);
        ::MessageBox(NULL,(LPCWSTR)w_buf,TEXT("VERS OF QT"),MB_OK);
    }
#endif



    if((!strcmp(argv[1], "-z")) || (!strcmp(argv[1], "-Z")))
    {

      flag_Zoom_tiff_data = 1;
    }
    else
      flag_Zoom_tiff_data = 0;
}
else
    flag_Zoom_tiff_data = 0;

 errno = 0;
    fprintf(stderr,"Main start\n");
QScrollArea *sa;
    QApplication app(argc, argv);

QClipboard* pcb = QApplication::clipboard();

QString dir = app.applicationDirPath();
//QString Dir = app.applicationDirPath();

QDir dr;
dir = dr.toNativeSeparators(dir);
string s_dir = dir.toStdString();
string patch_ini("C:\\Users\\Public");
QWidget wnd(0, Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint
                 |Qt:: CustomizeWindowHint);
QWidget    *w_paint =  main_Init(&s_dir,&patch_ini,&wnd,&sa, (QClipboard*)&pcb, dir);
    //((TLD_16Widget*)w_paint)->setAppWork(dir);
    wnd.show();
    ((TLD_16Widget*)w_paint)->init_AfteShow();
     wnd.setFixedSize(wnd.width()  + 2 *sa->verticalScrollBar()->width() , wnd.height());
    int ret = app.exec();
    fprintf(stderr,"Main stop\n");
    return ret;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
QWidget *main_Init(string * dir_work, string *patch_ini,
                   QWidget *parent, QScrollArea **sa, QClipboard* pcb, QString dir_app)
{
QString e;
string p = *dir_work;

#ifdef unix
    p += "/p_dll_udp.so";
    QLibrary     lib_dll_udp(p.c_str());
             sensor = new TLD_16(dir_work,  patch_ini,(QLibrary*)&lib_dll_udp, (QString*)&e, parent);
#else
   p += "\\p_dll_udp";
   QLibrary      lib_dll_udp(p.c_str());
              sensor = new TLD_16(dir_work, patch_ini,(QLibrary*)&lib_dll_udp, (QString*)&e, parent);
#endif

            *sa = new QScrollArea;
QWidget     *w_paint   = new TLD_16Widget(parent, sensor, *sa, pcb, dir_app);

            if(sensor->GetPixelPerString() == 2304)
            {
                (*sa)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

            WORD w = (*sa)->horizontalScrollBar()->width();
                (*sa)->horizontalScrollBar()->resize(w,16);
                (*sa)->setFixedHeight(sensor->GetPixelPerString()/4 +16);//64
            }
            else
            if(sensor->GetPixelPerString() == 2048)
            {
                (*sa)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

                WORD w = (*sa)->horizontalScrollBar()->width();
                (*sa)->horizontalScrollBar()->resize(w,16);
                (*sa)->setFixedHeight(sensor->GetPixelPerString()/4 + 16);//
            }
            else
                if(sensor->GetPixelPerString() == 576)
                    {
                        (*sa)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                        (*sa)->setFixedHeight(sensor->GetPixelPerString());
                    }
                    else
                    if(sensor->GetPixelPerString() == 1152)
                        {
                            (*sa)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                            (*sa)->setFixedHeight(sensor->GetPixelPerString()/2);//2
                        }

            sensor->set_Widget(w_paint);
QPalette pall;
            pall.setColor(w_paint->backgroundRole () , Qt::white );
            w_paint->setPalette(pall);
            if(sensor->GetPixelPerString() == 2304)
                w_paint->resize(sensor->GetPixelPerString()/2,65536);
            else
            if(sensor->GetPixelPerString() == 2048)
                w_paint->resize(sensor->GetPixelPerString()/2,65536);
            else
                w_paint->resize(sensor->GetPixelPerString(),65536);
            w_paint->setAutoFillBackground(true);

            ((TLD_16Widget*)w_paint)->init_Widgets(*sa);

            ((TLD_16Widget*)w_paint)->InitRegApp(patch_ini,(QString*)&e);

#ifdef USE_REDUCT_TEST
            sensor->initReducTest();
#endif

return w_paint;
}





#ifndef unix
DWORD  AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW)
{
    ULONG cCharacters;
    DWORD dwError;
    if (NULL == pszA)
    {
        *ppszW = NULL;
        return 1;
    }
    cCharacters = (ULONG)strlen(pszA)+1;
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
