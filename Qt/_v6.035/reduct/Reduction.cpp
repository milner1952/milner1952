//--------------------------------------------------------------------------
//---------------------------------------------------------------------------
#include "Reduction.h"
#include "Reduction_class.h"

REDUCTION rduc;

#include <math.h>
#include <stdio.h>
#include "../exp_error/exp_error.h"
DWORD sum_in[9];
DWORD sum_out[9];
WORD *buferOfNonReucData;

#ifdef USE_REDUCT_TEST
    TEST_RDUC_DATA *rdt_tst;
#endif

bool init = 0;

DWORD EXP_ERROR::error;
string EXP_ERROR::text_out;
//--------------------------------------------------------------------------
//Вызывается при вычислении средних значений на уровнях
// аргументы:
//              WORD *pWtr - указатель на буфер, в котором вычисляются
//          среднии значения уровней загрузки пикселей
// возвращаемое значение:
//          - 0 - при отсутствии ошибок
//          - 1 - при наличии ошибок
// коментарии:
//              - функция - обертка для обработок except. генерируемых функцией
//      bool rduc_Set(WORD *pWtr);
//---------------------------------------------------------------------------
bool rduc_SetData(WORD *pWtr)
{
    try
    {
    return rduc_Set(pWtr);
    }
    catch(EXP_ERROR &e)
    {
        e.ErrorSetText("p_dll_reduct");
        //e.ErrorShow("p_dll_reduct", "rduc_SetData");
        return 1;
    }
 return 0;
}

//--------------------------------------------------------------------------
//Вызывается при вычислении средних значений на уровнях
// аргументы:
//              WORD *pWtr - указатель на буфер, в котором вычисляются
//          среднии значения уровней загрузки пикселей
// возвращаемое значение: 0
// коментарии:
//              - функция для вычисления каждого уровня
//      вызывает функцию класса RDUC_DATA:SetLevel(pWtr, i); после этого функцией
//      RDUC_DATA: Set_midl_l_r(); заполняются переменные RDUC_DATA:midl_l, RDUC_DATA:midl_r
//      значениями старшего и младшего уровней загрузки пикселей
//              - функция выбрасывает исключение, если аргумент pWtr == NULL
//              - функция транслирует исключения генерируемые функцией RDUC_DATA::SetLevel(WORD*, WORD)
//---------------------------------------------------------------------------
bool rduc_Set(WORD *pWtr)
{
WORD n_lv = rduc.GetNumLevels();
#ifdef USE_REDUCT_TEST
      memset(rdt_tst,0,sizeof(RDUC_DATA));
#endif

        if(pWtr == NULL)
        {
            throw EXP_ERROR("rduc_Set", "Base buffer transferred == NULL");
        }

        for(WORD i = 0; i < n_lv;i++)
        {
                rduc.SetLevel(pWtr, i);
        }

        rduc.Set_midl_l_r();
return 0;
}

//--------------------------------------------------------------------------
//Вызывается при коррекции различных уровней загрузки АЦП
// аргументы:
//              WORD *pWtr - указатель на буфер, в котором производится коррекция
//              DWORD num  - длина буфера в словах
// возвращаемое значение:
//          - 0 - при отсутствии ошибок
//          - 1 - при наличии ошибок
// коментарии:
//              - функция - обертка для обработок except. генерируемых функцией
//      bool rduc_Get(WORD *pWtr);
//---------------------------------------------------------------------------
int rduc_GetData(WORD *pWtr, DWORD num)
{
    try
    {
    return rduc_Get(pWtr,num);
    }
    catch(EXP_ERROR &e)
    {
        e.ErrorSetText("p_dll_reduct");
        return 1;
    }
return 0;
}

//--------------------------------------------------------------------------
//Вызывается при коррекции различных уровней загрузки АЦП
// аргументы:
//              WORD *pWtr - указатель на буфер, в котором производится коррекция
//              DWORD num  - длина буфера в словах
// возвращаемое значение: - 0
// коментарии:
//               - функция выбрасывает исключение, если:
//      - аргумент num меньше минимально допустимого или равен нолю
//      - аргумент pWtr == NULL
//      - переменные RDUC_DATA:midl_l и RDUC_DATA:midl_r равны
//               - функция транслирует исключения функции FindR
//               - функция для каждого пикселя, переданном для коррекции, вызывает
//      функцию rduc.FindR(WORD w,WORD *level), которая ищет структуру RDUC_DATA
//      далее этот пиксель корректируется коеффициентом rdt->deviation этой структуры
//---------------------------------------------------------------------------
int rduc_Get(WORD *pWtr, DWORD num)
{
WORD sz = rduc.Size_level_in_byte();
WORD n_lv = rduc.GetNumLevels();

        if((((DWORD)(n_lv * sz)) > (num * 2)) || (num == 0L))
        {
            throw EXP_ERROR((char*)"rduc_Get", (char*)"The length of the transmitted buffer is small");

        }
        if(pWtr == NULL)
        {
                throw EXP_ERROR((char*)"rduc_Get", (char*)"Base buffer transferred");
        }

            rduc.SetWlWh();
WORD *wtr = (WORD*)(pWtr);
WORD n_adc = rduc.Num_adc();

        if(rduc.Midl_l() == rduc.Midl_r())
        {
        string as = "ReductionGet";
                as += "rduc.Midl_l() == rduc.Midl_r() = ";
                M_SPRINTF(as, rduc.Midl_r())
                throw EXP_ERROR("rduc_Get",  (char*)as.c_str() );
        }

         RDUC_DATA *rdt;

         DWORD j;
         WORD level;
                for(DWORD i = 0; i < (num - 1); i++)
                {
                        j = i % n_adc;
                        rdt = rduc.FindR(wtr[i], &level);

#ifdef USE_REDUCT_TEST
                        WORD n = rduc.N_pos_noise();
                        WORD e = rduc.N_pos_noise_end();
                        if((i > n) && (i  <= e))
                        {
                           init = 1;
                           rdt_tst[i -  n - 1].rduc = rdt;
                           rdt_tst[i -  n - 1].level = level;
                           rdt_tst[i -  n - 1].no_reduct_data = wtr[i];
                        }
#endif
                       wtr[i] = ((WORD)ceil((((double)wtr[i]) * rdt->deviation[j] )));
                }
return 0;
}

//--------------------------------------------------------------------------
//Вызывается при инициализации dll
// аргументы:
//          - WORD pix_off_string - количество пикселей в строке
//          - WORD lf - отступ от левого фронта ступеньки
//          - WORD rg - отступ от правого фронта ступеньки
//          - BYTE bn - бининг датчика
// возвращаемое значение:  0
// коментарии:
//              - функция - обертка для обработок except. генерируемых функцией
//      bool REDUCTION::Init(WORD pix_of_str, WORD lf, WORD rg, BYTE bn)
//---------------------------------------------------------------------------
bool rduc_Init(WORD pix_off_str, WORD lf, WORD rg, BYTE bn)
{
EXP_ERROR::ErrorRes();
        try
        {
            rduc.Init(pix_off_str, lf, rg, bn);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_dll_reduct");
            //e.ErrorShow("p_dll_reduct", "rduc_Init");
        }
return 0;
}

//--------------------------------------------------------------------------
//      Функция вычисляет среднее значение во входном буфере для каждого АЦП
// аргументы:
//          - WORD указатель на буфер для поиска среднего значения пикселя
//          - DWORD dNum - длина буфера в пикселях(WORD)
// возвращаемое значение:
//          - нет
// коментарии:
//         - функция - обертка для обработок except. генерируемых функцией
//      void REDUCTION::GetMidleForBuferFromADC((WORD *pWtr, DWORD num, DWORD *sum)
//---------------------------------------------------------------------------
void rduc_GetMideleForBuferInFromADC(WORD *pWtr, DWORD num)
{
rduc.GetMidleForBuferFromADC(pWtr + rduc.NumWordReduction() ,num - rduc.NumWordReduction(), sum_in);
}

//--------------------------------------------------------------------------
//      Функция вычисляет среднее значение в выходном буфере для каждого АЦП
// аргументы:
//          - WORD указатель на буфер для поиска среднего значения пикселя
//          - DWORD dNum - длина буфера в пикселях(WORD)
// возвращаемое значение:
//          - нет
// коментарии:
//         - функция - обертка для обработок except. генерируемых функцией
//      void REDUCTION::GetMidleForBuferFromADC((WORD *pWtr, DWORD num, DWORD *sum)
//---------------------------------------------------------------------------
void rduc_GetMideleForBuferOutFromADC(WORD *pWtr, DWORD num)
{
rduc.GetMidleForBuferFromADC(pWtr + rduc.NumWordReduction() ,num - rduc.NumWordReduction() , sum_out);
}

//--------------------------------------------------------------------------
//      Функция возвращает указатель на буфер средних уровней загрузки входного буфера, всех АЦП
// аргументы: нет
// возвращаемое значение:
//          - указатель на буфер средних уровней загрузки входного буфера
// коментарии:
//         - нет
//---------------------------------------------------------------------------
DWORD* rduc_GetPtrForMidleBuferIn()
{
return sum_in;
}
//--------------------------------------------------------------------------
//      Функция возвращает указатель на буфер средних уровней загрузки выходного буфера, всех АЦП
// аргументы: нет
// возвращаемое значение:
//          - указатель на буфер средних уровней загрузки выходного буфера
// коментарии:
//         - нет
//---------------------------------------------------------------------------
DWORD* rduc_GetPtrForMidleBuferOut()
{
 return sum_out;
}

//--------------------------------------------------------------------------
//  DWORD rduc_GetError() - Возвращает наличие ошибок
// аргументы:
//          -нет
// возвращаемое значение:
//          - 0 при отсутствии ошибок
//          - EXP_ERROR::ErrorGet() - при наличии ошибок
// коментарии:
//          - функционал функции может быть расширен в будующем
//---------------------------------------------------------------------------
DWORD rduc_GetError(char **text)
{
    DWORD ret;
    if((ret = EXP_ERROR::ErrorGet()))
        *text = (char*)EXP_ERROR::GetTextError()->c_str();
    else
        *text = NULL;
    return ret;
}

#ifdef USE_REDUCT_TEST
//--------------------------------------------------------
 REDUCTION* rduc_GetClassPtr(WORD *ptr, TEST_RDUC_DATA *rdt)
 {
     buferOfNonReucData = (WORD*)ptr;
     rdt_tst = rdt;
 return &rduc;
 }
 //--------------------------------------------------------
void rduc_SetBuferOfNonReducData(WORD *wtr, DWORD num)
{ 
   memcpy(buferOfNonReucData, wtr, num * 2);
}
//--------------------------------------------------------
bool rduc_SetNposstringnoise(WORD string_noise)
{
   rduc.N_pos_noise((string_noise - 1) * rduc.Num_pix_in_string());
   rduc.N_pos_noise_end(rduc.N_pos_noise() + rduc.Num_pix_in_string());
   return init;
}
#endif

