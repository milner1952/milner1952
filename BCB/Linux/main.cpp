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



#ifndef USE_REG_EX
    #include <../../SRC/BCB/reg.h>
#else
    #include <../../SRC/BCB/REG_EX/reg.h>
    #include <../../SRC/BCB/REG_EX/regdata.h>
    #include <../../SRC/BCB/REG_EX/reg_app_data.h>
#endif

#include "tld_16.h"
#include "qtld_16widget.h"

QWidget* main_Init(QWidget*, QScrollArea**);

// ----------------------------------------------------------------------

TLD_16 *sensor;

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
int main(int argc, char** argv)
{
QScrollArea *sa;

    QApplication app(argc, argv);
    QWidget wnd(0,Qt::Window);
QWidget    *w_paint =  main_Init(&wnd,&sa);
    wnd.show();
    ((TLD_16Widget*)w_paint)->init_AfteShow();
    int ret = app.exec();
    return ret;
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
QWidget *main_Init(QWidget *parent, QScrollArea **sa)
{
QLibrary     lib_dll_udp("./s_dll_udp");
QString e;
            sensor = new TLD_16((QLibrary*)&lib_dll_udp, (QString*)&e, parent);
            *sa = new QScrollArea;
QWidget    *w_paint   = new TLD_16Widget(parent, sensor, *sa);
            (*sa)->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            (*sa)->setFixedHeight(sensor->GetPixelPerString());

            sensor->set_Widget(w_paint);
QPalette pall;
            pall.setColor(w_paint->backgroundRole () , Qt::white );
            w_paint->setPalette(pall);
            w_paint->resize(sensor->GetPixelPerString(),65536);
            w_paint->setAutoFillBackground(true);

            ((TLD_16Widget*)w_paint)->init_Widgets(*sa);
            ((TLD_16Widget*)w_paint)->connect();
            ((TLD_16Widget*)w_paint)->InitRegApp((QString*)&e);
return w_paint;
}


