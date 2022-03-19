

#include "qtld_16widget.h"

static const char  *  RegKey_app = "Software\\N_PRIBOR_P_KAMERA_2048_APP_SRK\\ParamList";


static const char *   reg_str_resurs_param_app[] = {"ZOOM_Y",
                                                   "SCRULBARBAZA_Y", "N_POS_STRING_NOISE",
                                                   "IZ_HEX", "STR_GRID_TOP_ROW",
                                                   "TIMER_ZDR","COM_NAME"
                                                   };
static const char * defstr[] = {"1", "0","1", "0", "0", "5000","1"};

    void *reg_lib;
    WRITEPARAMTOOREG  g_WriteParamTooReg;

//------------------------------------------------------------------------
TLD_16Widget::TLD_16Widget(QWidget *par, TLD_16 *sen, QScrollArea *s): QWidget(par)
{
 timer_div2 = 0;
  sa = s;
 parent = par;
 sensor = sen;
  bufer = (WORD*)sen->Bufer();

  // From ini file
 base = 0;
 scale = 1;
 stringNomber = 0;
 rubberline0 = 0;
 rubberline1 = 0;




 run_timer = false;
 noise_img = false;
 Rgb = new QRgb[sensor->GetPixelPerString() * sensor->GetPixelPerString() * sizeof(QRgb)];
}

//------------------------------------------------------------------------
void TLD_16Widget::init_Widgets(QScrollArea *sa)
{
    pcmdNoise = new QPushButton ( "ШУМ" );
    pcmdImages = new QPushButton ( "СНИМОК" );
    pcmdTimer= new QPushButton ( "ТАЙМЕР" );
    pchkMatrix = new QCheckBox("C матрицами");
    pchkReduct = new QCheckBox("Без сведения");
    pchkFilter = new QCheckBox("Без фильтра");

    lb_Base = new QLabel("0");
    lb_Base->setFixedHeight(lb_Base->fontMetrics().height());
    lb_Base->setAlignment(Qt::AlignCenter);

    pssbScale_Y = new QSpinBox();
    pssbScale_Y->setRange (1, 65536/sensor->GetPixelPerString());

    pssbStringNamber = new QSpinBox();
    pssbStringNamber->setRange (1, 25);

    pssbGane = new QSpinBox();
    pssbGane->setRange (1, 16);

    pssbBinning = new QSpinBox();
    pssbBinning->setRange (1, sensor->GetPixelPerString() == 576 ? 2 : 4);
    lb_TextBinning = new  QLabel("Binning");

    lb_Sc = new QLabel(" Маштаб по Y");
    lb_Sc->setFixedHeight(lb_Sc->fontMetrics().height());
    lb_Sn = new QLabel("Номер строки");
    lb_Sn->setFixedHeight(lb_Sn->fontMetrics().height());
    lb_G =  new QLabel("Коефф. уиления матриц");
    lb_G->setFixedHeight(lb_G->fontMetrics().height());
    lb_StringBase = new QLabel("База");
    lb_StringBase->setFixedHeight(lb_StringBase->fontMetrics().height());
    lb_StringBase->setAlignment(Qt::AlignCenter);


    psbr0 = new QScrollBar(Qt::Horizontal);
    lb_psbr0 = new QLabel("0");
    psbr1 = new QScrollBar(Qt::Horizontal);
    lb_psbr1 = new QLabel("0");

    psbr0->setRange(0,65535);
    psbr1->setRange(0,65535);
    psbr0->setPageStep(sensor->GetPixelPerString()/2);
    psbr1->setPageStep(sensor->GetPixelPerString()/2);

QVBoxLayout  *pvL_Ed_Gr = new QVBoxLayout;
QGridLayout  *pGr = new QGridLayout;

    pGr->addWidget(pcmdNoise,0,0);
    pGr->addWidget(pcmdImages,0,1);
    pGr->addWidget(pchkReduct,0,3);
    pGr->addWidget(pcmdTimer,0,2);
    pGr->addWidget(lb_Base,0,4);


    pGr->addWidget(pchkMatrix,1,3);

    pGr->addWidget(lb_Sc,2,0);
    pGr->addWidget(lb_Sn,2,1);
    pGr->addWidget(lb_G ,2,2);

    pGr->addWidget(pchkFilter,2,3);


    pGr->addWidget(pssbScale_Y,1,0);
    pGr->addWidget(pssbStringNamber,1,1);
    pGr->addWidget(pssbGane,1,2);
    pGr->addWidget(lb_StringBase,1,4);

    const char *txt_rbt[] = {"0B","1B","2B","3B","4B",
                            "5B","6B","7B","8B","9B"};

    int i;
    for(i = 0; i < 10; i++)
    {
        QString qtxt(txt_rbt[i]);
                rbt[i] = new QRadioButton(txt_rbt[i]);
        if(i < 5)
                pGr->addWidget(rbt[i],3,i);
        else
                pGr->addWidget(rbt[i],4,i - 5);
    }


    pGr->addWidget( pssbBinning);
    pGr->addWidget(lb_psbr0,5,1,1,1,Qt::AlignRight);
    pGr->addWidget(psbr0, 5, 2, 4, 4, Qt::AlignTop);
    pGr->addWidget(lb_TextBinning, 6, 0, 1, 1, Qt::AlignTop);
    pGr->addWidget(lb_psbr1,6,1, 1,1, Qt::AlignRight);
    pGr->addWidget(psbr1, 6, 2, 4, 4, Qt::AlignTop);

    sa->setWidget(this);
    pvL_Ed_Gr->addWidget(sa);
    pvL_Ed_Gr->addLayout(pGr);

    parent->setLayout(pvL_Ed_Gr);

}

//-----------------------------------------------------------
void TLD_16Widget::connect()
{

    QObject::connect (pcmdNoise, SIGNAL(clicked()), sensor,SLOT(slotNoise()));
    QObject::connect (pcmdImages, SIGNAL(clicked()), sensor,SLOT(slotImages()));

    QObject::connect (pchkMatrix, SIGNAL(clicked(bool)), sensor,SLOT(slotMatrix(bool)));
    QObject::connect (pchkReduct, SIGNAL(clicked(bool)), sensor,SLOT(slotReduct(bool)));

    QObject::connect (pchkFilter, SIGNAL(clicked(bool)), sensor,SLOT(slotFilter(bool)));



    QObject::connect (pcmdTimer, SIGNAL(clicked()), sensor,SLOT(slotTimer()));
    QObject::connect (sensor->psigMapper, SIGNAL(mapped(int)), sensor,SLOT(slotRbtClicked(int)));

int i;
    for( i = 0; i < 10; i++)
    {
     QObject::connect (rbt[i], SIGNAL(clicked()), sensor->psigMapper,SLOT(map()));
     sensor->psigMapper->setMapping(rbt[i], i);
    }
    QObject::connect (psbr0, SIGNAL(sliderMoved(int)), sensor,SLOT(slotRuberLine0(int)));
    QObject::connect (psbr1, SIGNAL(sliderMoved(int)), sensor,SLOT(slotRuberLine1(int)));

    QObject::connect (psbr0, SIGNAL(valueChanged(int)), sensor,SLOT(slotRuberLine0(int)));
    QObject::connect (psbr1, SIGNAL(valueChanged(int)), sensor,SLOT(slotRuberLine1(int)));

    QObject::connect (pssbScale_Y, SIGNAL(valueChanged(int)), sensor, SLOT(slotScale(int)));
    QObject::connect (pssbStringNamber, SIGNAL(valueChanged(int)), sensor,SLOT(slotStringNomber(int)));
    QObject::connect (pssbGane, SIGNAL(valueChanged(int)), sensor, SLOT(slotGane(int)));

    QObject::connect (pssbBinning, SIGNAL(valueChanged(int)), sensor, SLOT(slotBinning(int)));


    QObject::connect (sa->verticalScrollBar(), SIGNAL(valueChanged(int)), sensor,SLOT(slotVertScrullBar(int)));

    QObject::connect (    pcmdImages, SIGNAL(pressed()), sensor, SLOT(slotImagesPressed()));

}
//---------------------------------------------------------------
void TLD_16Widget::init_AfteShow()
{
    sa->setFixedWidth(sensor->GetPixelPerString() + sa->verticalScrollBar()->width());
    parent->move(100, 100);
    sa->verticalScrollBar()->setRange (1, (65535/scale - sensor->GetPixelPerString()));
    sa->verticalScrollBar()->setSingleStep(1);


    regdata = sensor->getParamRegData();

    pchkReduct->setChecked(regdata->iz_reduction);
    pchkReduct->setText(regdata->iz_reduction == 1 ? "Со сведением" : "Без сведения");
    pchkMatrix->setChecked(1 ^ regdata->with_matrices);
    pchkMatrix->setText((1 ^ regdata->with_matrices) == 0 ? "C матрицами" : "Без матриц");


    pchkFilter->setChecked(regdata->iz_h_filter);
    pchkFilter->setText(regdata->iz_h_filter == 1 ? "С фильтром" : "Без фильтра");


    pssbGane->setValue(regdata->mtr_gane);
    pssbBinning->setValue(regdata->iz_binning);
    time_period  = regdata->timer_period;
    startTimer(time_period /2);

    rbt[regdata->iz_dac]->setChecked(true);

}

//----------------------------------------------------------------
MINMAX TLD_16Widget::Image_FindMinMax(WORD *buf, DWORD len)
{
  MINMAX nx;
         nx.w_min = 0xffff;
         nx.w_max = 0;

  for(DWORD i = 0; i < len; i++)
  {
   WORD w = buf[i];
         if(w == 0)
           continue;

         nx.w_min = min(w, nx.w_min);
         nx.w_max = max(w, nx.w_max);
         if( nx.w_max > 57000)
          nx.w_max =  nx.w_max;

  }

return nx;
}

//------------------------------------------------------------------
void TLD_16Widget::rstImg(QPainter &ptr)
{
WORD n = sensor->getPixelPerString()/regdata->iz_binning;
    for(int i = 0; i < (n * n); i++)
    {
        WORD w;

          w = 0xff;
         _RGB rgb;
         rgb.r.r = w;
         rgb.r.g = w;
         rgb.r.b = w;
         rgb.r.l = 0;
         Rgb[i] = rgb.l;

    }
QImage   img((unsigned char*)&Rgb[0], n,n, QImage::Format_RGB32);
    ptr.drawImage(0, 0, img);
}

//------------------------------------------------------------------
void TLD_16Widget::drawImg(QPainter &ptr)
{
WORD n = sensor->getPixelPerString()/regdata->iz_binning;
MINMAX minmax = Image_FindMinMax(bufer + 30 * n, n * n);

double delta = (double)(minmax.w_max - minmax.w_min);
double kf = (delta + 1)/256.0;

if(kf  == 0)
        kf += 1.0;
    for(int i = 0; i < (n * n); i++)
    {
        WORD w;
        w = bufer[i];
        w = ((double)(w - minmax.w_min)/kf);

         _RGB rgb;
         rgb.r.r = w;
         rgb.r.g = w;
         rgb.r.b = w;
         rgb.r.l = 0;
         Rgb[i] = rgb.l;

    }
QImage   img((unsigned char*)&Rgb[0], n,n, QImage::Format_RGB32);
    ptr.drawImage(0, 0, img);
}

//--------------------------------------------------------------
void TLD_16Widget::drawNoise (QPainter &ptr)
{
 WORD n_pix_in_str = sensor->getPixelPerString();
 QPoint *a = new QPoint[n_pix_in_str];
 WORD n_matrix = 9;

 int i, x, m;
 x = 0;
 m = 0;
 for(m = 0; m < n_matrix; m++)
 {

     for(i = 0; i < n_pix_in_str; i += n_matrix)
     {
         long t = bufer[stringNomber * n_pix_in_str + i + m];
         t = t/scale;
         a[x] = QPoint(x,t) ;
         x++;
     }
 }
ptr.drawPolyline(a, n_pix_in_str);
delete[] a;
}
//----------------------------------------------------------------
void TLD_16Widget::drawRubberLine(QPainter &ptr, WORD r)
{
 QPainter::CompositionMode cm = ptr.compositionMode();
ptr.setCompositionMode(QPainter::CompositionMode_Xor);
long rl;

rl = r/scale;
QPoint beg(0,rl);
QPoint end(575,rl);

ptr.drawLine(beg, end);
ptr.setCompositionMode(cm);
}



//-------------------------------------------------------------
void TLD_16Widget::reset_Img()
{
 reset_img = true;
}


//------------------------------------------------------------------
void TLD_16Widget::InitRegApp(QString *e)
{

    P_INITREG InitReg;
    READPARAMTOOREG ReadParamTooReg;
    QLibrary     reg_lib_app("./libp_reg_lib_ex");


    InitReg = (P_INITREG)reg_lib_app.resolve("InitReg");
    if(!InitReg)
    {
         *e = reg_lib_app.errorString();
         sensor->dll_src_SetMessError("main_Init",e->toStdString().c_str());

    }

    ReadParamTooReg = (READPARAMTOOREG)reg_lib_app.resolve("ReadParamTooReg");
    if(!ReadParamTooReg)
    {
         *e = reg_lib_app.errorString();
         sensor->dll_src_SetMessError("main_Init",e->toStdString().c_str());

    }

    g_WriteParamTooReg = (WRITEPARAMTOOREG)reg_lib_app.resolve("WriteParamTooReg");
    if(!g_WriteParamTooReg)
    {
         *e = reg_lib_app.errorString();
         sensor->dll_src_SetMessError("main_Init",e->toStdString().c_str());

    }

    reg = InitReg(RegKey_app, reg_str_resurs_param_app, 7, defstr, "./s_app_test_init_ex");
    regstring = ReadParamTooReg();
    reg_app_data = app_FaseReg(regstring);
}
//-----------------------------------------------------------
//
//-----------------------------------------------------------------------
REG_APP_DATA *TLD_16Widget::app_FaseReg(REGSTRING *r_str)
{


    REG_APP_DATA *d = new REG_APP_DATA;
                        d->zoom_y = atoi(r_str[0].c_str());
                        d->scrullbaza = atoi(r_str[1].c_str());
                        d->n_pos_string_noise = atoi(r_str[1].c_str());

                        d->iz_hex = atoi(r_str[1].c_str());
                        d->str_grid_top_row = atoi(r_str[1].c_str());
                        d->timer_zdr = atol(r_str[1].c_str());
                        d->com_name = atoi(r_str[1].c_str());
return d;
}
