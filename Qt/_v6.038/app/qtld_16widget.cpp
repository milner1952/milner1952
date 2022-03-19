

#include "qtld_16widget.h"
#include "reg_app_data.h"
#include "../global.h"
#include <locale.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

const char res_str_noise[] =  {"ШУМ"};
const char res_str_snaphot[] =  {"СНИМОК"};
const char res_str_timer[] =  {"ТАЙМЕР"};
const char res_str_with_matrix[] =  {"С матрицами"};
const char res_str_n_reduct[] =  {"Без сведения"};
const char res_str_n_filtr[] =  {"Без H фильтра"};
const char res_str_m_Y[] =  {"Масштаб по Y"};
const char res_str_nomber_string[] =  {"Номер строки"};
const char res_str_kf_gane_matrix[] =  {"Коеффициент усиления матриц"};
const char res_str_baza[] =  {"База"};
const char res_str_reduct[] =  {"Со сведением"};
const char* res_str_no_matrix =  {"Без матриц"};
const char res_str_filtr[] =  {"С H фильтром"};
const char res_str_m_X[] =  {"Масштаб по X"};
const char *res_str_s_filtr =  {"С S фильтром"};
const char *res_str_no_s_filtr =  {"Без S фильтра"};
const char res_str_icmp[] = {"ICMP"};
const char res_str_Power[] = {"Выкл"};


static const char  *  RegKey_app = "Software\\N_PRIBOR_P_KAMERA_2048_APP_SRK\\ParamList";


const char *   reg_str_resurs_param_app[] = {"ZOOM_Y",
                                                   "SCRULBARBAZA_Y", "N_POS_STRING_NOISE",
                                                   "IZ_HEX", "STR_GRID_TOP_ROW",
                                                   "TIMER_ZDR","COM_NAME","RUBBER_LINE0","RUBBER_LINE1",
                                                    "ZOOM_X"
                                                   };
static const char * defstr[] = {"1", "0","1", "0", "0", "5000","1","1","1","1"};

    void *reg_lib;
    WRITEPARAMTOOREG  g_WriteParamTooReg;


//------------------------------------------------------------------------
TLD_16Widget::TLD_16Widget(QWidget *par, TLD_16 *sen, QScrollArea *s, QClipboard* p): QWidget(par)
{

    pcb = p;
    timer_div2 = 0;
    sa = s;
    parent = par;
    sensor = sen;
    bufer = (WORD*)sen->Bufer();
    noise = 0.0;
    run_timer = false;
    noise_img = false;
    Rgb = new QRgb[sensor->GetPixelPerString() * sensor->GetPixelPerString() * sizeof(QRgb)];
    setMouseTracking ( true);    
    if(sensor->GetPixelPerString() == 1152)
    {
        time1152[0] = _19MC;
        time1152[1] = _38MC;
        res_str_time1152[0] = "Время на строку: 1,9mc";
        res_str_time1152[1] = "Время на строку: 3,8mc";
    }
}
//-----------------------------------------------------------------------
void TLD_16Widget::init_Widgets(QScrollArea *sa)
{
    const char *txt_rbt[] = {"0B","1B","2B","3B","4B",
                            "5B","6B","7B","8B","9B"};
    for(int i = 0; i < 10; i++)
    {
                rbt[i] = new QRadioButton(txt_rbt[i]);
    }

    pcmdNoise = new QPushButton (res_str_noise);
    pcmdImages = new QPushButton(res_str_snaphot);
    pcmdTimer = new QPushButton ((char*)res_str_timer);
    pcmdIcmpPkt = new QPushButton (res_str_icmp);

    pchkMatrix = new QCheckBox((char*)res_str_with_matrix);
    pchkReduct = new QCheckBox((char*)res_str_n_reduct);

    pssbSFilter = new QSpinBox();
    pssbScale_Y = new QSpinBox();
    pssbStringNamber = new QSpinBox();
    pssbGane = new QSpinBox();

    lb_TextSFilter = new QLabel();
    lb_Error = new QLabel("0");
    lb_Base = new QLabel("0");
    lb_Sc = new QLabel((char*)res_str_m_Y);
    lb_Sn = new QLabel((char*)res_str_nomber_string);
    lb_G =  new QLabel((char*)res_str_kf_gane_matrix);
    lb_StringBase = new QLabel((char*)res_str_baza);
    lb_Loading = new QLabel("X=0,Y=0,N=0");
    lb_psbr0 = new QLabel("0");
    lb_psbr1 = new QLabel("0");
    lb_Power = new QLabel(res_str_Power);



    psbr0 = new QScrollBar(Qt::Horizontal);
    psbr1 = new QScrollBar(Qt::Horizontal);
    pTimer =   new  QTimer(this);
    QVBoxLayout  *pvL_Ed_Gr = new QVBoxLayout;
    QGridLayout  *pGr = new QGridLayout;


    if((sensor->GetPixelPerString() != 2048) && (sensor->GetPixelPerString() != 2304))
    {
        pchkFilter = new QCheckBox((char*)res_str_n_filtr);
        if(sensor->GetPixelPerString() == 1152)
        {
            pchkTimePerString = new QCheckBox((char*)res_str_time1152[0].c_str());
        }
        pssbBinning = new QSpinBox();
        lb_TextBinning = new  QLabel("Binning");
        pGr->addWidget(pcmdIcmpPkt,2,5,Qt::AlignCenter);
        pGr->addWidget(lb_Power,2,4,Qt::AlignRight);
    }
    else
    {
        pssbScale_X = new QSpinBox();
        lb_ScX = new QLabel((char*)res_str_m_X);
        pGr->addWidget(pcmdIcmpPkt,2,4,Qt::AlignCenter);
        pGr->addWidget(lb_Power,2,4,Qt::AlignLeft);
    }



    lb_Base->setFixedHeight(lb_Base->fontMetrics().height());
    lb_Base->setAlignment(Qt::AlignCenter);
    pssbStringNamber->setRange (1, 25);
    pssbGane->setRange (1, 16);

    lb_Sc->setFixedHeight(lb_Sc->fontMetrics().height());
    lb_Sn->setFixedHeight(lb_Sn->fontMetrics().height());
    lb_G->setFixedHeight(lb_G->fontMetrics().height());
    lb_StringBase->setFixedHeight(lb_StringBase->fontMetrics().height());

    lb_Loading->hide();
    psbr0->setRange(0,65535);
    psbr1->setRange(0,65535);
    psbr0->setPageStep(sensor->GetPixelPerString()/2);
    psbr1->setPageStep(sensor->GetPixelPerString()/2);

    pGr->addWidget(pcmdNoise,0,0);
    pGr->addWidget(pcmdImages,0,1);
    pGr->addWidget(pchkReduct,0,3);
    pGr->addWidget(pcmdTimer,0,2);
    pGr->addWidget(lb_Base,0,3,Qt::AlignRight);

    pGr->addWidget(lb_Sc,2,0);
    pGr->addWidget(lb_Sn,2,1);
    pGr->addWidget(lb_G ,2,2);

    pGr->addWidget(pssbScale_Y,1,0);
    pGr->addWidget(pssbStringNamber,1,1);
    pGr->addWidget(pssbGane,1,2);

    for(int i = 0; i < 10; i++)
    {
        if(i < 5)
                pGr->addWidget(rbt[i],3,i);
        else
                pGr->addWidget(rbt[i],4,i - 5);
    }

    if((sensor->GetPixelPerString() != 2048) &&   (sensor->GetPixelPerString() != 2304))
    {
        pGr->addWidget(pchkFilter,2,3);
        pGr->addWidget(pchkMatrix,1,3);
        lb_StringBase->setAlignment(Qt::AlignCenter);
        if(sensor->GetPixelPerString() == 1152)
        {
                pssbScale_Y->setRange (1, 65536/(sensor->GetPixelPerString()/8));
                pGr->addWidget(pchkTimePerString,0,4,1,1, Qt::AlignCenter);
                pGr->addWidget(lb_StringBase,1,3, Qt::AlignRight);
                pssbBinning->setRange (1, 2);
         }
         else
            if(sensor->GetPixelPerString() == 576)
            {
                pssbScale_Y->setRange (1, 65536/sensor->GetPixelPerString());
                pGr->addWidget(lb_StringBase,1,4);
                pssbBinning->setRange (1, 4);
            }

         pGr->addWidget(psbr1, 6, 2, 4, 4, Qt::AlignTop);//1111111111111
         pGr->addWidget(psbr0, 5, 2, 4, 4, Qt::AlignTop);

         pGr->addWidget(lb_psbr0,5,1,1,1,Qt::AlignRight);
         pGr->addWidget(lb_psbr1,6,1, 1,1, Qt::AlignRight);
         pGr->addWidget(lb_TextSFilter,3,5,1,1,Qt::AlignLeft );
         pGr->addWidget(lb_Loading,6, 0, 1, 2, Qt::AlignLeft);
         pGr->addWidget(lb_Error,  2, 4, 1, 1, Qt::AlignCenter);
         pGr->addWidget(lb_TextBinning, 5, 1, 1, 1, Qt::AlignLeft);

         pGr->addWidget(pssbBinning,5, 0);
         pGr->addWidget(pssbSFilter,3, 4,1,1, Qt::AlignRight);

    }
    else
    {
        lb_StringBase->setAlignment(Qt::AlignLeft);
        pssbScale_Y->setRange (1, 65536/(sensor->GetPixelPerString()/4));
        pssbScale_X->setRange (1,2);

        pGr->addWidget(psbr1,   6, 0, 6, 4, Qt::AlignTop);  //222222222222222
        pGr->addWidget(psbr0,   5, 0, 6, 4, Qt::AlignTop );
        pGr->addWidget(lb_psbr1,6, 4, 1, 1, Qt::AlignLeft);
        pGr->addWidget(lb_psbr0,5, 4, 1, 1, Qt::AlignLeft);

        pGr->addWidget(lb_ScX,2,3);
        pGr->addWidget(lb_TextSFilter,4,4,1,1,Qt::AlignCenter );
        pGr->addWidget(lb_Loading,3, 4, 1, 1, Qt::AlignRight);
        pGr->addWidget(lb_StringBase,0,4);
        pGr->addWidget(lb_Error,  6, 4, 1, 1, Qt::AlignCenter);

        pGr->addWidget(pchkMatrix,1,4);
        pGr->addWidget(pssbScale_X,1,3);
        pGr->addWidget(pssbSFilter,5, 4,1,1, Qt::AlignCenter);

    }
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
    QObject::connect (pcmdIcmpPkt, SIGNAL(clicked()), sensor,SLOT(slotSendIcmp()));


    QObject::connect (pchkMatrix, SIGNAL(clicked(bool)), sensor,SLOT(slotMatrix(bool)));
    QObject::connect (pchkReduct, SIGNAL(clicked(bool)), sensor,SLOT(slotReduct(bool)));
    if((sensor->GetPixelPerString() != 2048) && (sensor->GetPixelPerString() != 2304))
    {
        QObject::connect (pchkFilter, SIGNAL(clicked(bool)), sensor,SLOT(slotFilter(bool)));
        QObject::connect (pssbBinning, SIGNAL(valueChanged(int)), sensor, SLOT(slotBinning(int)));
        if(sensor->GetPixelPerString() == 1152)
            QObject::connect (pchkTimePerString, SIGNAL(clicked(bool)), sensor,SLOT(slotTimePerString(bool)));
    }
    else
    {
        QObject::connect (pssbScale_X, SIGNAL(valueChanged(int)), sensor, SLOT(slotScaleX(int)));
        QObject::connect (sa->horizontalScrollBar(), SIGNAL(valueChanged(int)), sensor,SLOT(slotHorzScrullBar(int)));

        QObject::connect (sa->horizontalScrollBar(), SIGNAL(sliderMoved(int)), sensor,SLOT(slotHorzScrullBar(int)));
    }
    QObject::connect (pcmdTimer, SIGNAL(clicked()), sensor,SLOT(slotTimerPressed()));
    QObject::connect (sensor->psigMapper, SIGNAL(mapped(int)), sensor,SLOT(slotRbtClicked(int)));

    for(int i = 0; i < 10; i++)
    {
        QObject::connect (rbt[i], SIGNAL(clicked()), sensor->psigMapper,SLOT(map()));
        sensor->psigMapper->setMapping(rbt[i], i);
    }
    QObject::connect (psbr0, SIGNAL(sliderMoved(int)), sensor,SLOT(slotRuberLine0(int)));
    QObject::connect (psbr1, SIGNAL(sliderMoved(int)), sensor,SLOT(slotRuberLine1(int)));
    QObject::connect (psbr0, SIGNAL(valueChanged(int)), sensor,SLOT(slotRuberLine0(int)));
    QObject::connect (psbr1, SIGNAL(valueChanged(int)), sensor,SLOT(slotRuberLine1(int)));
    QObject::connect (pssbScale_Y, SIGNAL(valueChanged(int)), sensor, SLOT(slotScaleY(int)));
    QObject::connect (pssbStringNamber, SIGNAL(valueChanged(int)), sensor,SLOT(slotStringNomber(int)));
    QObject::connect (pssbGane, SIGNAL(valueChanged(int)), sensor, SLOT(slotGane(int)));
    QObject::connect (sa->verticalScrollBar(), SIGNAL(valueChanged(int) ), sensor,SLOT(slotVertScrullBar(int)));
    QObject::connect (    pcmdImages, SIGNAL(pressed()), sensor, SLOT(slotImagesPressed()));
    QObject::connect (    pTimer, SIGNAL(timeout()), sensor, SLOT(slotTimeOut()));
    QObject::connect (pssbSFilter, SIGNAL(valueChanged(int)), sensor, SLOT(slotSFilter(int)));
}
//---------------------------------------------------------------
void TLD_16Widget::init_AfteShow()
{    
    sa->verticalScrollBar()->setSingleStep(1);
WORD x;
WORD ofs_y;
    x = sensor->GetPixelPerString();
    ofs_y = x;
    if((sensor->GetPixelPerString() == 2304) || (sensor->GetPixelPerString() == 2048))
    {
        x /= 2; ofs_y = x/4;
    }
    sa->setFixedWidth(x + sa->verticalScrollBar()->width());
    parent->move(100, 100);
    sa->verticalScrollBar()->setRange (1, 65535/reg_app_data->zoom_y - ofs_y);
    connect();
    regdata = sensor->getParamRegData();
    pchkReduct->setChecked(regdata->iz_reduction);
    pchkReduct->setText(regdata->iz_reduction == 1 ? (char*)res_str_reduct : (char*)res_str_n_reduct);
    pchkMatrix->setChecked(1 ^ regdata->with_matrices);
    pchkMatrix->setText((1 ^ regdata->with_matrices) == 0 ? (char*)res_str_with_matrix : (char*)res_str_no_matrix);
    if((sensor->GetPixelPerString() == 2048) || (sensor->GetPixelPerString() == 2304))
    {
        sa->horizontalScrollBar()->setSingleStep(1);
        pssbScale_X->setValue(reg_app_data->zoom_x);
        sensor->slotScaleX(reg_app_data->zoom_x);
    }
    else
    {
        pchkFilter->setChecked(regdata->iz_h_filter);
        pchkFilter->setText(regdata->iz_h_filter == 1 ? (char*)res_str_filtr : (char*)res_str_n_filtr);
        sensor->slotFilter(regdata->iz_h_filter);
        pssbBinning->setValue(regdata->iz_binning);
        sensor->slotScaleX(1);
    }
    pssbGane->setValue(regdata->mtr_gane);
    srand(time(NULL));
    rbt[regdata->iz_dac]->setChecked(true);
    sensor->slotRbtClicked(regdata->iz_dac);
    pssbScale_Y->setValue(reg_app_data->zoom_y);
    sensor->slotRuberLine0(reg_app_data->rubberline0);
    sensor->slotRuberLine1(reg_app_data->rubberline1);
    psbr0->setValue(reg_app_data->rubberline0);
    psbr1->setValue(reg_app_data->rubberline1);
    sa->verticalScrollBar()->setValue(reg_app_data->scrullbaza );
    pssbStringNamber->setValue(reg_app_data->n_pos_string_noise);
    reg_app_data->scrullbazaX = 0;
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
     if(sensor->GetPixelPerString() == 1152)
     {
      REGDATA *rd =     regdata;
            sensor->slotTimePerString((bool)regdata->iz_time_per_string);
            Qt::CheckState state = rd->iz_time_per_string == 0 ? Qt::Unchecked : Qt:: Checked;
            pchkTimePerString->setCheckState(state);
     }
     pssbSFilter->setValue(regdata->iz_s_filter);
     lb_TextSFilter->setText(regdata->iz_s_filter <= 1 ? res_str_no_s_filtr : res_str_s_filtr);
     pssbSFilter->setRange(1,_MAX_S_FILTER);
     pssbSFilter->setSingleStep(2);

    lb_Power->setText(sensor->Power() == 0 ? "Выкл" : "Вкл");

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
  }

return nx;
}

//------------------------------------------------------------------
void TLD_16Widget::prevRstImg()
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

}

//------------------------------------------------------------------

void TLD_16Widget::prevDrawImg()
{
int i;
WORD n = sensor->getPixelPerString()/regdata->iz_binning;
MINMAX minmax = Image_FindMinMax(((WORD*)bufer) + 40 * n, (n * n - 60 *n));

double delta = (double)(minmax.w_max - minmax.w_min);
double kf = (delta + 1)/256.0;

    if((WORD)kf  == 0)
        kf += 0.01;
    int x = 0;
    for( i = 0; i < (n * n); i++,x++)
    {
    WORD w;
        w = bufer[x += (reg_app_data->zoom_x - 1)];
        w = ((double)(w - minmax.w_min)/kf);

    _RGB rgb;
         rgb.r.r = w;
         rgb.r.g = w;
         rgb.r.b = w;
         rgb.r.l = 0;
         Rgb[i] = rgb.l;
    }

}

//------------------------------------------------------------------------
void TLD_16Widget::drawImg(QPainter &ptr)
{
 WORD n = sensor->getPixelPerString()/regdata->iz_binning;
    QImage img((unsigned char*)&Rgb[0], n, n, QImage::Format_RGB32);
    ptr.drawImage(0, 0, img, 0, 0, 0, 0);
}



//--------------------------------------------------------------
void TLD_16Widget::drawNoise (QPainter &ptr, WORD *bfr)
{

 WORD n_pix_in_str;
bool flag = 0;
if(bfr != bufer)
    ptr.setPen(QPen(Qt::red));
else
    ptr.setPen(QPen(Qt::black));
    n_pix_in_str = sensor->getPixelPerString();
 QPoint *a = new QPoint[sensor->getPixelPerString()];
 WORD n_matrix = sensor->getPixelPerString() == 2048 ? 8 : 9;


 int i, x, m;
 x = 0;
 m = 0;
 for(m = 0; m < n_matrix; m++)
 {
     for(i = 0; i < sensor->getPixelPerString(); i += n_matrix)
     {
         long t =0;
         t = bfr[reg_app_data->n_pos_string_noise * sensor->getPixelPerString() + i + m];
         t = t/reg_app_data->zoom_y;
         if(reg_app_data->zoom_x == 1)
         {
            a[x] = QPoint(x,t) ;
            x++;
         }
         else
         {
             if(flag == 0)
             {
                 a[x] = QPoint(x,t) ;
                 x++;
             }
             flag ^= 1;
         }
     }
 }

#ifdef USE_REDUCT_TEST
    if(bfr != bufer)
    {
#ifdef unix
        ptr.setFont(QFont("Times",9, QFont::Normal));
#else
        ptr.setFont(QFont("Times",7, QFont::Normal));
#endif
    WORD tx = reg_app_data->scrullbazaX + 1;
    char *text;
    char *n_text;
    char *rd_text = NULL;
    char *dl_text0 = NULL;
    char *dl_text1 = NULL;
    char *dl_text2 = NULL;
    char *b_midl = NULL;
    char *dt_md_bufer = NULL;
        reduct_test_GetTextData(&text,&n_text,&rd_text, &dl_text0, &dl_text1, &dl_text2, &b_midl, &dt_md_bufer);
        ptr.setPen(QPen(Qt::black));               
        WORD baze = Base();        
        ptr.drawText(tx,baze + 15, text);
s_pcb.clear();

        s_pcb = text;
        if(rd_text != NULL)
        {
            ptr.drawText(tx, baze + 30, rd_text);
            s_pcb += "\r\n";
            s_pcb += rd_text;
            s_pcb += "\r\n";

        }

#ifdef unix
        ptr.setFont(QFont("Times",9, QFont::Normal));
#else
        ptr.setFont(QFont("Times",6, QFont::Normal));
#endif
        if(dl_text0 != NULL)
        {
            ptr.drawText(tx,baze + 45, dl_text0);
            s_pcb += dl_text0;
            s_pcb += "\r\n";

        }
        if(dl_text1 != NULL)
        {
            ptr.drawText(tx,baze + 60, dl_text1);
            s_pcb += dl_text1;
            s_pcb += "\r\n";
        }
        if(dl_text2 != NULL)
        {
            ptr.drawText(tx,baze + 75, dl_text2);
            s_pcb += dl_text2;
            s_pcb += "\r\n";
        }
#ifdef unix
        ptr.setFont(QFont("Times",9, QFont::Normal));
#else
        ptr.setFont(QFont("Times",7, QFont::Normal));
#endif
        ptr.setPen(QPen(Qt::red));        
        ptr.drawText(tx,baze + 90, n_text);
        s_pcb += n_text;
        s_pcb += "\r\n";

#ifdef unix
        ptr.setFont(QFont("Times",9, QFont::Normal));
#else
        ptr.setFont(QFont("Times",6, QFont::Normal));
#endif
        if(b_midl != NULL)
        {
            ptr.setPen(QPen(Qt::black));
            ptr.drawText(tx,baze + 105, b_midl);
            s_pcb += b_midl;
            s_pcb += "\r\n";

            string::iterator it;
            it = s_pcb.begin();

            do
            {
             char c = *it;
                    if(c == '.')
                    s_pcb.replace(it - s_pcb.begin(), 1, ",");
                    it++;

            } while(it != s_pcb.end());

        }

        if(dt_md_bufer != NULL)
        {
            ptr.drawText(tx, baze + 120, dt_md_bufer);
            s_pcb += "\r\n";
            s_pcb += dt_md_bufer;
        }
        ptr.setPen(QPen(Qt::red));
        setFocusPolicy(Qt::StrongFocus);
    }
#endif//#ifdef USE_REDUCT_TEST
    ptr.drawPolyline(a, n_pix_in_str/(reg_app_data->zoom_x == 1 ? 1 : 2));
#ifdef USE_REDUCT_TEST
        reduct_testDrawVertMouseLine(ptr, sa->height());
#endif//#ifdef USE_REDUCT_TEST
    delete[] a;

}

//----------------------------------------------------------------
void TLD_16Widget::drawRubberLine(QPainter &ptr, WORD r)
{
    ptr.setPen(QPen(Qt::black));
    QPainter::CompositionMode cm = ptr.compositionMode();
    ptr.setCompositionMode(QPainter::CompositionMode_Xor);
long rl;

    rl = r/reg_app_data->zoom_y;

QPoint beg(0,rl);
int l_end;
    if(sensor->getPixelPerString() == 2048)
        l_end = 2047;    
    else
        if(sensor->getPixelPerString() == 1152)
            l_end = 1152;
        else
            if(sensor->getPixelPerString() == 2304)
                l_end = 2304;
        else
        l_end = 575;
QPoint end(l_end,rl);
    ptr.drawLine(beg, end);
    ptr.setCompositionMode(cm);
}

//------------------------------------------------------------------
#ifdef unix
void TLD_16Widget::InitRegApp(string */*patch_ini*/,QString *e)
#else
void TLD_16Widget::InitRegApp(string *patch_ini,QString *e)
#endif
{
 P_INITREG       InitReg;
 READPARAMTOOREG ReadParamTooReg;

#ifdef unix
 QLibrary     reg_lib_app("./libp_reg_lib");
#else
 QLibrary     reg_lib_app("p_reg_lib");
#endif

    InitReg = (P_INITREG)reg_lib_app.resolve("InitReg");
    if(!InitReg)
    {
         *e = reg_lib_app.errorString();
         sensor->SetMessError("main_Init",(char*)e->toStdString().c_str());

    }

    ReadParamTooReg = (READPARAMTOOREG)reg_lib_app.resolve("ReadParamTooReg");
    if(!ReadParamTooReg)
    {
         *e = reg_lib_app.errorString();
         sensor->SetMessError("main_Init",(char*)e->toStdString().c_str());

    }

    g_WriteParamTooReg = (WRITEPARAMTOOREG)reg_lib_app.resolve("WriteParamTooReg");
    if(!g_WriteParamTooReg)
    {
         *e = reg_lib_app.errorString();
         sensor->SetMessError("main_Init",(char*)e->toStdString().c_str());

    }


    DestroyReg = (DESTROYREG)reg_lib_app.resolve("DestroyReg");
    if(!DestroyReg)
    {
         *e = reg_lib_app.errorString();
         sensor->SetMessError("main_Init",(char*)e->toStdString().c_str());

    }
//DWORD  AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW)
#ifdef unix
    reg = InitReg(RegKey_app, reg_str_resurs_param_app, 10, defstr, "./p_app_test_init");
#else  


if(patch_ini == NULL)
{
wchar_t w_buf[MAX_PATH];
                if(!GetModuleFileName(NULL, w_buf, MAX_PATH))
                     sensor->SetMessError("main_Init",(char*)"GetModuleFileName");

wchar_t* n = wcsrchr(w_buf, L'\\');
                wmemset(n,L'0',100);
                wcscpy(n,TEXT("\\p_app_test_init.ini"));
                reg = InitReg(RegKey_app, reg_str_resurs_param_app, 10, defstr, w_buf);                        
}
else
{
wchar_t w_buf[MAX_PATH];
                wmemset(w_buf,L'0',MAX_PATH);

                AnsiToUnicode(patch_ini->c_str(), w_buf);

                wcscat(w_buf,TEXT("\\p_app_test_init.ini"));
                reg = InitReg(RegKey_app, reg_str_resurs_param_app, 10, defstr, w_buf);
}

#endif
    regstring = ReadParamTooReg(reg);
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
                        d->n_pos_string_noise = atoi(r_str[2].c_str());
                        d->iz_hex = atoi(r_str[3].c_str());
                        d->str_grid_top_row = atoi(r_str[4].c_str());
                        d->timer_zdr = atol(r_str[5].c_str());
                        d->com_name = atoi(r_str[6].c_str());
                        d->rubberline0 = atoi(r_str[7].c_str());
                        d->rubberline1 = atoi(r_str[8].c_str());
                        d->zoom_x = atoi(r_str[9].c_str());
return d;
}
//---------------------------------------------------------------
void TLD_16Widget::mouseMoveEvent(QMouseEvent *pe)
{

 char bx[100];
 WORD X = pe->x();
 WORD Y = pe->y();
 WORD load;
    if(noise_img == 1)
    {
      if(reg_app_data->zoom_x  == 2)
        load = bufer[Y  * sensor->getPixelPerString() + X * 2];
      else
        load = bufer[(Y + reg_app_data->scrullbazaX)  * sensor->getPixelPerString() + X];

        memset(bx,0,100);
        strcpy(bx,"X=");
        sprintf(&bx[strlen(bx)],"%d",pe->x());
        strcat(bx,",Y=");
        sprintf(&bx[strlen(bx)],"%d",pe->y());
        strcat(bx,",N=");
        sprintf(&bx[strlen(bx)],"%d",load);

        lb_Loading->setText(bx);
    }
#ifdef USE_REDUCT_TEST
    else
    {

        if (pe->modifiers() & Qt::ShiftModifier)
        {
            reduct_test_SetMouseData(X,Y);
            repaint();
        }
    }
#endif//#ifdef USE_REDUCT_TEST
}
//-----------------------------------------------------------------
void TLD_16Widget::keyPressEvent(QKeyEvent *pe)
{
    switch (pe->key())
    {
    case Qt::Key_C:
    if (pe->modifiers() & Qt::ControlModifier)
        {
        pcb->clear();
        pcb->setText(s_pcb.c_str());
        }
        break;
    default:
        QWidget::keyPressEvent(pe);
    }

}

//----------------------------------------------------------------------
double TLD_16Widget::getNoisePower(MINMAX mn, WORD *wtr, DWORD num)
{
double sum = 0.0;
WORD min = mn.w_min;
    for(DWORD i = 0; i < num; i++)
    {

        WORD w = wtr[i] - min;
        double dw = (double)(w *w);
        sum += dw;
    }

    sum = sqrt(sum);
    noise = sum/(double)num;
    return noise;
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
#ifndef unix
DWORD  TLD_16Widget::AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW)
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
