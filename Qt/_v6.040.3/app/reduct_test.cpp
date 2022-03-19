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
static char buf_midl[10000];
static char data_bufer_of_midl[10000];
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
                              char **dl_b0, char **dl_b1, char **dl_b2, char **b_pMidls, char **dt_Midl)
{
    WORD zoom_x = reg_app_data->zoom_x;

        memset(buf_reduct_data,0, 10000);
        memset(buf_no_reduct_data,0, 10000);
        memset(rdt_tst_buf,0, 10000);
        memset(dtl_buf0,0, 10000);
        memset(dtl_buf1,0, 10000);
        memset(dtl_buf2,0, 10000);
        memset(buf_midl,0, 10000);
        memset(data_bufer_of_midl,0, 10000);

    WORD t_x;
         t_x = zoom_x == 2 ? X * 2 : X;

    WORD n_mtr = (t_x / pix_in_matrix);//номер матрицы под маркером
    WORD ofs_mtr = t_x % pix_in_matrix;//nomer pix

    WORD index_buf = reg_app_data->n_pos_string_noise * sensor->getPixelPerString() + n_mtr + num_matrix * ofs_mtr;

    WORD *r_wtr = (((WORD*)sensor->Bufer()));

        sprintf(buf_reduct_data,"index in buf=%d x = %d Y reduct= %d", index_buf, X, r_wtr[index_buf]);
        *b_reduc_data = &buf_reduct_data[0];

    WORD *n_wtr = (((WORD*)buferOfNonReucData));
        sprintf(buf_no_reduct_data,"Y no reduct= %d", n_wtr[index_buf ]);
        *b_n_reduc_data = &buf_no_reduct_data[0];

int p = n_mtr + num_matrix * ofs_mtr -1;
        p = p < 0 ? 0 : p;
    TEST_RDUC_DATA *ptr_tst = &rdt_tsts[0] ;
    TEST_RDUC_DATA rdt_tst = *(ptr_tst + p);//rdt_tsts[p ];
    double dv;
    double midl;
    double *pMidle;
    WORD *dtMidl;
        if(init == 1)
        {

            dv = rdt_tst.rduc->deviation[n_mtr];
            midl = rdt_tst.rduc->midl_loading_in_level;
            pMidle = &rdt_tst.rduc->pMidl[0];
            dtMidl = &rdt_tst.rduc->data_bufer[0];
WORD  num_pixel_on_level_of_own_adc = rdt_tst.rduc->n_p_in_level;

WORD t_level = rdt_tst.level;
            if(t_level == 1)
                t_level = 0;

            sprintf(rdt_tst_buf,"level=%d  no reduct data=%d\
    diviation=%.18f midle loading in level=%.18f",   //midle loading in level=%ld
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

            reduct_testDrawMidelData(pMidle,(char*)&buf_midl);
            *b_pMidls = (char*)&buf_midl;

            rduct_testDrawMidlDataBufer(dtMidl, (char*)&data_bufer_of_midl, n_mtr,
                                        num_pixel_on_level_of_own_adc);
            *dt_Midl = (char*)&data_bufer_of_midl;
        }

    return NULL;
}
//--------------------------------------------------------------------------------------
void rduct_testDrawMidlDataBufer(WORD* dt_in, char* dt_out, WORD level, WORD num)
{
   char t[20];

   WORD *wtr = &dt_in[level * num];
   strcpy(dt_out,"pix: ");
   for(int i = 0; i < num; i++)
   {
       memset(t, 0 ,20);
       sprintf(t,"%d", wtr[i]);
       strcat(dt_out, t);
       if(i < (num - 1))
            strcat(dt_out, "+");

   }

}
//-----------------------------------------------------------------------------------------------------------------
void reduct_testDrawMidelData(double *pMidle, char *buf) //pMidl
{
    WORD num_adc = rduc->Num_adc();
    if(num_adc > 8)
               sprintf(buf,"midels=%.18f %.18f %.18f %.18f %.18f %.18f %.18f %.18f %.18f ",
                        pMidle[0],pMidle[1],pMidle[2],pMidle[3],pMidle[4],pMidle[5],pMidle[6],pMidle[7],pMidle[8]);
      else
                sprintf(buf,"midels=%.18f %.18f %.18f %.18f %.18f %.18f %.18f %.18f ",
                        pMidle[0],pMidle[1],pMidle[2],pMidle[3],pMidle[4],pMidle[5],pMidle[6],pMidle[7]);
}

//----------------------------------------------------------------
void reduct_testDrawLevelData(WORD level,char *buf)
{

    WORD num_adc = rduc->Num_adc();
    RDUC_DATA* rdt = rduc->Rduc_data(level);
    if(rdt == NULL)
        return;
    double midl = rdt->midl_loading_in_level;


    if(num_adc > 8)
               sprintf(buf,"Level=%d midle:%.18f %.18f  %.18f  %.18f %.18f  %.18f  %.18f  %.18f  %.18f  %.18f ",level, midl,
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
               sprintf(buf,"Level=%d  midle:%.18f %.18f %.18f %.18f %.18f %.18f %.18f %.18f %.18f ",level, midl,
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
