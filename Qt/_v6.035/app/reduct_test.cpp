#include "reduct_test.h"
#include "tld_16.h"

#ifdef USE_REDUCT_TEST
#include <QtWidgets>
REDUCTION *rduc;
static char buferOfNonReucData[SIZE_BUFER];
static TEST_RDUC_DATA *rdt_tsts;
static WORD Y;
static WORD X;

static WORD num_matrix;
static WORD pix_in_matrix;
static REG_APP_DATA *reg_app_data;
static TLD_16 *sensor;
static P_DLL_SETNPOSSTRINGNOISE g_p_dll_SetNposstringnoise;
static char buf_no_reduct_data[10000];
static char buf_reduct_data[10000];
static char rdt_tst_buf[10000];
static char dtl_buf0[10000];
static char dtl_buf1[10000];
static char dtl_buf2[10000];
static bool init;
//------------------------------------------------------
void reduct_test_Init(TLD_16 *sen, QLibrary *lib_dll_udp, REG_APP_DATA *rg_app_data)
{
        rdt_tsts = NULL;
        init = 0;
        sensor = sen;
QString *e = 0;
        Y = 0;
        reg_app_data = rg_app_data;
P_DLL_GETRDUCPTR p_dll_GetRducPtr = (P_DLL_GETRDUCPTR)lib_dll_udp->resolve("p_dll_GetRducPtr");
        if(!p_dll_GetRducPtr)
        {
             *e = lib_dll_udp->errorString();
             sensor->SetMessError("reduct_test_Init",(char*)e->toStdString().c_str());
        }

        g_p_dll_SetNposstringnoise = (P_DLL_SETNPOSSTRINGNOISE)lib_dll_udp->resolve("p_dll_SetNposstringnoise");
        if(!g_p_dll_SetNposstringnoise)
        {
            *e = lib_dll_udp->errorString();
            sensor->SetMessError("reduct_test_Init",(char*)e->toStdString().c_str());
        }
        if((sensor->getPixelPerString() == 1152) || (sensor->getPixelPerString() == 576) ||
           (sensor->getPixelPerString() == 2304))
            num_matrix = 9;
        else
            num_matrix = 8;
            pix_in_matrix = sensor->getPixelPerString()/num_matrix;
    memset(buferOfNonReucData, 0, SIZE_BUFER);
    rdt_tsts = new TEST_RDUC_DATA[sensor->getPixelPerString()];
    rduc = p_dll_GetRducPtr((WORD*)buferOfNonReucData, rdt_tsts);
}

//-----------------------------------------------------------
void reduc_test_Delete()
{
    if(rdt_tsts != NULL)
        delete[] rdt_tsts;
}
//-----------------------------------------------------------
char* reduct_test_GetBuferOfNonReducData()
{
return buferOfNonReucData;
}
//----------------------------------------------------
void reduct_test_SetMouseData(WORD x,WORD y)
{
    Y = y;
    X = x;

}

//---------------------------------------------------

//-------------------------------------------------
char* reduct_test_GetTextData(char **b_reduc_data, char **b_n_reduc_data, char **b_rdt_tst,
                              char **dl_b0, char **dl_b1, char **dl_b2)
{
    WORD zoom_x = reg_app_data->zoom_x;

        memset(buf_reduct_data,0, 10000);
        memset(buf_no_reduct_data,0, 10000);
        memset(rdt_tst_buf,0, 10000);
        memset(dtl_buf0,0, 10000);
        memset(dtl_buf1,0, 10000);
        memset(dtl_buf2,0, 10000);
    WORD t_x;
         t_x = zoom_x == 2 ? X * 2 : X;

    WORD x = (t_x / pix_in_matrix);
    WORD ofs = t_x % pix_in_matrix;

    WORD x_buf = reg_app_data->n_pos_string_noise * sensor->getPixelPerString() + x + num_matrix * ofs;

    WORD *wtr = (((WORD*)sensor->Bufer()));
        sprintf(buf_reduct_data,"x_buf=%d x = %d Y reduct= %d", x_buf, X, wtr[x_buf]);
        *b_reduc_data = &buf_reduct_data[0];

    WORD *n_wtr = (((WORD*)buferOfNonReucData));
        sprintf(buf_no_reduct_data,"Y no reduct= %d", n_wtr[x_buf]);
        *b_n_reduc_data = &buf_no_reduct_data[0];

int p = x + num_matrix * ofs -1;
        p = p < 0 ? 0 : p;

    TEST_RDUC_DATA rdt_tst = rdt_tsts[p];
    double dv;
    DWORD midl;

        if(init == 1)
        {

            dv = rdt_tst.rduc->deviation[x];
            midl = rdt_tst.rduc->midl_loading_in_level;

            sprintf(rdt_tst_buf,"level=%d  no reduct data=%d\
                        diviation=%.15f midle loading in level=%ld",
                        rdt_tst.level,rdt_tst.no_reduct_data, dv, midl);
            *b_rdt_tst   = &rdt_tst_buf[0];

            if(rdt_tst.level > 0)
                reduct_testDrawLevelData(rdt_tst.level - 1,(char*)&dtl_buf0[0]);

            reduct_testDrawLevelData(rdt_tst.level,(char*)&dtl_buf1[0]);

            if(rdt_tst.level < rduc->GetNumLevels() - 1)
                reduct_testDrawLevelData(rdt_tst.level + 1,(char*)&dtl_buf2[0]);

            *dl_b0 = (char*)&dtl_buf0;
            *dl_b1 = (char*)&dtl_buf1;
            *dl_b2 = (char*)&dtl_buf2;

        }

    return NULL;
}
//----------------------------------------------------------------
void reduct_testDrawLevelData(WORD level,char *buf)
{

    WORD num_adc = rduc->Num_adc();
    RDUC_DATA* rdt = rduc->Rduc_data(level);
    WORD midl = rdt->midl_loading_in_level;


    if(num_adc > 8)
             sprintf(buf,"Level:%d  midle:%d   %.15f  %.15f  %.15f  %.15f  %.15f  %.15f  %.15f  %.15f  %.15f ",level, midl,
                      rdt->deviation[0],
                      rdt->deviation[1],
                      rdt->deviation[2],
                      rdt->deviation[3],
                      rdt->deviation[4],
                      rdt->deviation[5],
                      rdt->deviation[6],
                      rdt->deviation[7],
                      rdt->deviation[8]);
      else
             sprintf(buf,"Level:%d  midle:%d %.15f %.15f %.15f %.15f %.15f %.15f %.15f %.15f ",level, midl,
                      rdt->deviation[0],
                      rdt->deviation[1],
                      rdt->deviation[2],
                      rdt->deviation[3],
                      rdt->deviation[4],
                      rdt->deviation[5],
                      rdt->deviation[6],
                      rdt->deviation[7]);


}


//--------------------------------------------------
void  reduct_testDrawVertMouseLine(QPainter &ptr, WORD max_y)
{
    ptr.setPen(QPen(Qt::green));
    QPainter::CompositionMode cm = ptr.compositionMode();
    ptr.setCompositionMode(QPainter::CompositionMode_Xor);

    QPoint beg(X,reg_app_data->scrullbaza);
    QPoint end(X,reg_app_data->scrullbaza + max_y);

    ptr.drawLine(beg, end);
    ptr.setCompositionMode(cm);
}
//------------------------------------------------------
bool reduc_test_SetNposstringnoise(WORD nps)
{


    init = g_p_dll_SetNposstringnoise(nps);
    return init;

}

#endif//USE_REDUCT_TEST
