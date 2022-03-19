#include "qtld_16widget.h"

//------------------------------------------------------------------------
TLD_16Widget::TLD_16Widget(QWidget *par, TLD_16 *sen, QScrollArea *s): QWidget(par)
{
 sa = s;
 parent = par;
 sensor = sen;
 base = 0;
 scale = 1;
 bufer = (WORD*)sen->Bufer();
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
    pcmdNoise = new QPushButton ( "NOISE" );
    pcmdImages = new QPushButton ( "IMAGES" );
    pcmdDisplay= new QPushButton ( "DISPLAY" );
    pcmdTimer= new QPushButton ( "TIMER" );
    pchkMatrix = new QCheckBox("Witch matrix");


   // pcmdImages->setDisabled(true);

    lb_Base = new QLabel("0");
    lb_Base->setFixedHeight(lb_Base->fontMetrics().height());
    lb_Base->setAlignment(Qt::AlignCenter);

    pssbScale_Y = new QSpinBox();
    pssbScale_Y->setRange (1, 65536/sensor->GetPixelPerString());

    pssbStringNamber = new QSpinBox();
    pssbStringNamber->setRange (1, 25);

    pssbGane = new QSpinBox();
    pssbStringNamber->setRange (1, 16);


    lb_Sc = new QLabel("  Scale Y");
    lb_Sc->setFixedHeight(lb_Sc->fontMetrics().height());
    lb_Sn = new QLabel("String Nomber");
    lb_Sn->setFixedHeight(lb_Sn->fontMetrics().height());
    lb_G =  new QLabel(" Matrix Gane");
    lb_G->setFixedHeight(lb_G->fontMetrics().height());    
    lb_StringBase = new QLabel("Base");
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
    pGr->addWidget(pcmdDisplay,0,2);
    pGr->addWidget(pcmdTimer,0,3);
    pGr->addWidget(lb_Base,0,4);


    pGr->addWidget(pchkMatrix,1,3);

    pGr->addWidget(lb_Sc,2,0);
    pGr->addWidget(lb_Sn,2,1);
    pGr->addWidget(lb_G ,2,2);



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


    pGr->addWidget(lb_psbr0);
    pGr->addWidget(psbr0, 5, 1, 4, 4);

    pGr->addWidget(lb_psbr1);
    pGr->addWidget(psbr1, 6, 1, 4, 4);
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
    QObject::connect (pcmdDisplay, SIGNAL(clicked()), sensor,SLOT(slotDisplay()));
    QObject::connect (pchkMatrix, SIGNAL(clicked(bool)), sensor,SLOT(slotMatrix(bool)));
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


    QObject::connect (sa->verticalScrollBar(), SIGNAL(valueChanged(int)), sensor,SLOT(slotVertScrullBar(int)));

    QObject::connect (    pcmdImages, SIGNAL(pressed()), sensor, SLOT(slotImagesPressed()));

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
void TLD_16Widget::drawImg(QPainter &ptr)
{
WORD n = sensor->getPixelPerString();
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

//---------------------------------------------------------------
void TLD_16Widget::init_AfteShow()
{
    sa->setFixedWidth(sensor->GetPixelPerString() + sa->verticalScrollBar()->width());
    parent->move(100, 100);
    sa->verticalScrollBar()->setRange (1, (65535/scale - sensor->GetPixelPerString()));
    sa->verticalScrollBar()->setSingleStep(1);
    startTimer(5000);
}
