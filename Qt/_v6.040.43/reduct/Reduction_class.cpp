



#include "Reduction_class.h"
#include "../exp_error/exp_error.h"

#include <math.h>
#ifdef USE_REDUCT_TEST
    extern TEST_RDUC_DATA *rdt_tst;
#endif
//--------------------------------------------------------------------------
//      Функция фычисляет среднее значение на уровне АЦП и отклонения каждого пикселя на
//на уровне от этого среднего значения
// аргументы:
//          - WORD *pWtr - указатель на первый пиксель уровня в буфере
//          - WORD level - номер уровня
// возвращаемое значение:
//          - среднее значение на уровне
// коментарии:
//          - функцмя генерирует исключения:
//      - количество АЦП равно нолю
//      - количество пикселей на уровне равно нолю
//      - номер уровня больше заданного максимального
//              среднее значение на уровне АЦП и отклонения каждого пикселя на этом
//      уровне от этого среднего значения записывается в структуру типа RDUC_DATA
//---------------------------------------------------------------------------
DWORD
REDUCTION::SetLevel(WORD *pWtr,  WORD level)
{
int i,j;

WORD t;
RDUC_DATA *rdt;
RDUC_DATA * rdt_prev;
WORD *wtr = (pWtr + ((size_level_in_byte/2)) * level);
    memset((void*)pMidl, 0L, sizeof(double) * num_adc);
WORD num_pixel_on_level_of_own_adc;

     num_pixel_on_level_of_own_adc  = num_pix_in_level / num_adc;
double midle = 0.0;
    if(num_adc == 0)
    {
        throw EXP_ERROR("SetLevel", "num_adc == 0");
    }

    if(num_pix_in_level == 0)
    {
        throw EXP_ERROR("SetLevel", "num_pix_in_level == 0");
    }

    if(level > MAX_NUM_LEVELS)
    {
        throw EXP_ERROR("SetLevel", "level > MAX_NUM_LEVELS");
    }


WORD  midl_of_level_on_level_of_own_adc;
    midl_of_level_on_level_of_own_adc = (num_pixel_on_level_of_own_adc/2);
    rdt = Rduc_data(level);
    if(level > 0)
        rdt_prev = Rduc_data(level - 1);
    /*
if(level == 31)
    level = level;
*/
    for( i = 0; i < num_adc; i++)
    {
        double db64 = 0.0;
                for( j = 0; j <  num_pixel_on_level_of_own_adc; j++)
                {
                        if(j < l_front_reduct)
                        {
                                db64 +=  (double)(t = wtr[midl_of_level_on_level_of_own_adc * num_adc + i]);
                        }
                        else
                                if(j >=  r_front_reduct)
                                {
                                        db64 +=   (double)(t = wtr[midl_of_level_on_level_of_own_adc * num_adc + i]);
                                }
                                else
                                        db64 += (double)(t = wtr[j * num_adc + i ]);
#ifdef  USE_REDUCT_TEST
                        rdt->data_bufer[i * num_pixel_on_level_of_own_adc + j] = t;
                        rdt->n_p_in_level = num_pixel_on_level_of_own_adc;
#endif
                 }

                pMidl[i] = (db64  / (double)num_pixel_on_level_of_own_adc);
                midle += pMidl[i];
     }//for(int i = 0;



     midle = (midle ) / ((double)num_adc );

     rdt->midl_loading_in_level = midle;
#ifdef  USE_REDUCT_TEST
     memcpy(&rdt->pMidl[0],&pMidl[0],sizeof(double) * num_adc) ;
#endif

     for( i = 0; i < num_adc; i++)
     {
            rdt->deviation[i] = ( midle)/pMidl[i];


         /*
      double dt1;
      double pmd1;
                       if(level > 0)
                       {
                            dt1 = rdt_prev->deviation[i];
                            pmd1 = rdt_prev->midl_loading_in_level;
                       }
                       else
                       {
                            dt1 = 0.0;
                            pmd1 = 65536.0;
                       }


       double dt2 = rdt->deviation[i];
       double pmd2 = rdt->midl_loading_in_level;
       double dt = (dt1 - dt2)/(pmd1 - pmd2);
       */


            double dt1;
            double pmd1;
                             if(level > 0)
                             {
                                  dt1 = rdt_prev->deviation[i];
                                  pmd1 = rdt_prev->midl_loading_in_level;
                                  double dt2 = rdt->deviation[i];
                                  double pmd2 = rdt->midl_loading_in_level;
                                  double dt = (dt1 - dt2)/(pmd1 - pmd2);
                                  rdt->delta_deviation[i] =  dt;
                             }
                             else
                             {
                                  rdt->delta_deviation[i] =  0;
                             }



     }
     return (DWORD)midle;
}
//--------------------------------------------------------------------------
//      Функция в масиве структур класса RDUC_DATA ищет
// структуру i, типа RDUC_DATA, средний уровень midl_loading_in_level, которой
// больше пикселя в w, но меньше среднего уровня следующей структуры
// аргументы:
//          - WORD w - пиксель буфере
// возвращаемое значение:
//          - указатель на найденую структуру типа RDUC_DATA*
// коментарии:
//          - если пиксель находится midl_l и 65535, то возвращается указатель на
//      структуру rduc_data[0]
//          - если пиксель находится midl_r и 0, то то возвращается указатель на
//      структуру rduc_data[num_level - 1]
//          - в случае если уровень пикселя меньше чем в rduc_data[i].midl_loading_in_level
//      и больше чем rduc_data[i + 1].midl_loading_in_level, то возвращается
//      та структура из этих двух, соедний уровень в которой  midl_loading_in_level
//      ближе к пикселю w.
//          функция генерирует исключение, если структура не найдена
//---------------------------------------------------------------------------
RDUC_DATA*
REDUCTION::FindR(WORD w, WORD *level)
{
        if(w >= midl_l)
        {
            *level = 0;
            return &rduc_data[0];
        }
        else
        if(w <= midl_r)
        {
            *level = num_level - 1;
            return  &rduc_data[num_level - 1];
        }
        else
            for(int i = 0; i < num_level; i++)
            {
                if((w <= wl[i]) && (w > wh[i]))
                {
                    *level = i + 1;
                    return  &rduc_data[i + 1];
                }
            }
         throw EXP_ERROR("FindR", "return NULL");
}

//--------------------------------------------------------------------------
//      Функция осуществляет предварительную запись средних загрузок в два
//  массива wl и wh и вычисляет половину разницы между уровнями -для ускорения вычислений
//  аргументы - нет
// возвращаемое значение:
//          -нет
// коментарии:
//          - нет
//--------------------------------------------------------------------------
void REDUCTION::SetWlWh()
{
    for(int i = 0; i < num_level; i++)
    {
        wl[i] = (WORD)rduc_data[i].midl_loading_in_level;
        wh[i] = (WORD)rduc_data[i + 1].midl_loading_in_level;
        midl_delta[i] = (wl[i] - wh[i])/2 + wh[i];
    }
}

//--------------------------------------------------------------------------
//      Функция в масиве структур класса RDUC_DATA ищет
// структуру i, типа RDUC_DATA, средний уровень midl_loading_in_level, которой
// больше пикселя в w, но меньше среднего уровня следующей структуры
// аргументы:
//          - WORD w - пиксель буфере
// возвращаемое значение:
//          - указатель на найденую структуру типа RDUC_DATA*
// коментарии:
//          - если пиксель находится midl_l и 65535, то возвращается указатель на
//      структуру rduc_data[0]
//          - если пиксель находится midl_r и 0, то то возвращается указатель на
//      структуру rduc_data[num_level - 1]
//          - в случае если уровень пикселя меньше чем в rduc_data[i].midl_loading_in_level
//      и больше чем rduc_data[i + 1].midl_loading_in_level, то возвращается
//      та структура из этих двух, соедний уровень в которой  midl_loading_in_level
//      ближе к пикселю w.
//          функция генерирует исключение, если структура не найдена
//---------------------------------------------------------------------------

/*
RDUC_DATA*
REDUCTION::FindR(WORD w, WORD *level)
{
        if(w >= midl_l)
        {
            *level = 0;
            return &rduc_data[0];
        }
        else
        if(w <= midl_r)        
        {
            *level = num_level - 1;
            return  &rduc_data[num_level - 1];
        }
        else
            for(int i = 0; i < num_level; i++)
            {
                if((w <= wl[i]) && (w > wh[i]))
                {
                    if(w > midl_delta[i])
                    {
                        *level = i;
                        return &rduc_data[i];
                    }
                    else
                    {
                        *level = i + 1;
                        return &rduc_data[i + 1];
                    }
                }
            }
         throw EXP_ERROR("FindR", "return NULL");
}

//--------------------------------------------------------------------------
//      Функция осуществляет предварительную запись средних загрузок в два
//  массива wl и wh и вычисляет половину разницы между уровнями -для ускорения вычислений
//  аргументы - нет
// возвращаемое значение:
//          -нет
// коментарии:
//          - нет
//--------------------------------------------------------------------------
void REDUCTION::SetWlWh()
{
    for(int i = 0; i < num_level; i++)
    {
        wl[i] = (WORD)rduc_data[i].midl_loading_in_level;
        wh[i] = (WORD)rduc_data[i + 1].midl_loading_in_level;
        midl_delta[i] = (wl[i] - wh[i])/2 + wh[i];
    }
}

*/


//--------------------------------------------------------------------------
//      Функция вызывается при инициализации dll
// аргументы:
//          - WORD pix_of_string - количество пикселей в строке
//          - WORD lf - отступ от левого фронта ступеньки
//          - WORD rg - отступ от правого фронта ступеньки
//          - BYTE binn - бининг датчика
// возвращаемое значение:
//          - нет
// коментарии:
//      - функцмя генерирует исключения:
//      - если в датчике с 576 пикселями в строке уровни бининга не равны 1,2
//      - если в датчике с 1152 пикселями в строке уровни бининга не равны 1,2,3,4
//      - количество пикселей в строке не равны 576,1152, 2304, 2048
//---------------------------------------------------------------------------
void REDUCTION::Init(WORD pix_of_string, WORD lf, WORD rg, BYTE binn )
 {
        // midle = 0.0;
        binning = binn;
        num_pix_in_string = pix_of_string;

        if(pix_of_string == 576)
        {
            switch(binning)
            {
                 case 1: GetNumLevels(32); num_string_of_reduction_data = 16; num_adc = 9; num_pix_in_string = (576); break;
                 case 2: GetNumLevels(32); num_string_of_reduction_data = 16; num_adc = 9; num_pix_in_string = (576);break;

                 default:
                  string e = "binning error - ";
                         e + "it should be:";
                         e += ": 1,2";
                         throw EXP_ERROR("Init", e.c_str());
            }
        }
        else
        if(pix_of_string == 2304)
        {
                     GetNumLevels(32); num_string_of_reduction_data = 4; num_adc = 9; num_pix_in_string = (2304);
        }
        else
        if(pix_of_string == 2048)
        {
                     GetNumLevels(32); num_string_of_reduction_data = 4; num_adc = 8; num_pix_in_string = (2048);
        }
        else
        if(pix_of_string == 1152)
        {
                switch(binning)
                {
                      //  case 1: GetNumLevels(24); num_string_of_reduction_data = 4; num_adc = 9; num_pix_in_string = (1152); break;
                        case 1: GetNumLevels(32); num_string_of_reduction_data = 4; num_adc = 9; num_pix_in_string = (1152); break;
                        case 2: GetNumLevels(32); num_string_of_reduction_data = 8; num_adc = 9; num_pix_in_string = (1152);break;
                        case 3: GetNumLevels(36); num_string_of_reduction_data = 12;num_adc = 9; num_pix_in_string = (1152);break;
                        case 4: GetNumLevels(32); num_string_of_reduction_data = 16;num_adc = 9; num_pix_in_string = (1152);break;
                        default:
                    string e = "binning error - ";
                            e + "it should be:";
                            e += ": 1,2,3,4";
                            throw EXP_ERROR("Init", e.c_str());
                }
         }
         else
           throw EXP_ERROR("Init", "pix_of_string != (576 || 1152|| 2048 || 2304)");


        for(int i = 0; i < MAX_NUM_LEVELS; i++)
        {
                if(rduc_data[i].deviation != NULL)
                {
                   delete[] rduc_data[i].deviation;                   
                }
                if(rduc_data[i].delta_deviation != NULL)
                {
                   delete[] rduc_data[i].delta_deviation;
                }

                rduc_data[i].delta_deviation = NULL;
                rduc_data[i].deviation = NULL;
        }


        for(int i = 0; i < num_level; i++)
        {
                rduc_data[i].deviation = new double[num_adc];
                rduc_data[i].delta_deviation = new double[num_adc];
                for(int j = 0; j <  num_adc; j++)
                {
                        rduc_data[i].deviation[j] = 0.0;
                        rduc_data[i].delta_deviation[j] = 0.0;
                }
                rduc_data[i].midl_loading_in_level = 0;
        }

		midl_l = (WORD)rduc_data[0].midl_loading_in_level;
        midl_r = (WORD)rduc_data[num_level - 1].midl_loading_in_level;

        num_pix_in_level = ((WORD)((num_pix_in_string * num_string_of_reduction_data)/ num_level));
        if(pix_of_string == 576)
            size_level_in_byte = binning == 3 ? 396 * 2 :  (WORD)(num_pix_in_level *  sizeof(WORD));
            else
            if(pix_of_string == 2304)
                size_level_in_byte = (WORD)(num_pix_in_level *  sizeof(WORD));
            else
            if(pix_of_string == 2048)
                size_level_in_byte = (WORD)(num_pix_in_level *  sizeof(WORD));
                else
                    if(pix_of_string == 1152)
                        size_level_in_byte = binning == 3 ? 396 * 2 :  (WORD)(num_pix_in_level *  sizeof(WORD));
                        else
                        {
                        string e = "pix on string error - ";
                                    e + "it should be:";
                                    e += ": 2048,1152,576";
                                    throw EXP_ERROR("Init", e.c_str());
                        }

        if(pMidl != NULL)
                delete[] pMidl;
        pMidl = new double[num_adc];
        numWordReduction =  WORD(num_pix_in_string * num_string_of_reduction_data);
        l_front_reduct =  lf;
        r_front_reduct =  (num_pix_in_level / num_adc) - rg;
}
 


//--------------------------------------------------------------------------
//      Функция вычисляет среднее значение в буфере для каждого АЦП
// аргументы:
//          - WORD указатель на буфер для поиска среднего значения пикселя
//          - DWORD dNum - длина буфера в пикселях(WORD)
//          - DWORD *sum - указатель на выходной буфер
// возвращаемое значение:
//          - нет
// коментарии:
//          - функцмя генерирует исключения:
//      - количество пикселей в буфере равно 0;
//      - num_pix_in_string равно 0;
//      - num_adc равнo 0;
//---------------------------------------------------------------------------
void REDUCTION::GetMidleForBuferFromADC(WORD *pWtr, DWORD dNum, DWORD *sum)
{
__int64 sum64[9];
WORD *wtr;
DWORD num;
int i = 0;
int j = 0;

        memset(sum, 0L, num_adc * sizeof(DWORD));
        memset(sum64, 0L, num_adc * sizeof(__int64));
        if(dNum == 0)
            throw EXP_ERROR("GetMidleForBuferFromADC", "num pix in bufer == 0");
        if(num_pix_in_string == 0)
            throw EXP_ERROR("GetMidleForBuferFromADC", "num_pix_in_string == 0");
        if(num_adc == 0)
            throw EXP_ERROR("GetMidleForBuferFromADC", "num_adc == 0");

        wtr = pWtr; num = dNum;
int num_str = num/num_pix_in_string;
        WORD x = 0L;
        while(x < num_str )
        {
                for(i = 0; i < num_adc; i++)
                {
                    int n;
                        if(num_pix_in_string == 2304)
                            n = 256;
                        else
                            if(num_pix_in_string == 2048)
                                n = 256;
                                else
                                    if(num_pix_in_string == 1152)
                                        n = 128;
                                    else
                                        n = 64;

                        for(j = 0; j < n; j++)
                        {
                        WORD t = *(wtr + j * num_adc + i + x * num_pix_in_string);
                        sum64[i] += t;
                        }
                }
                x++;
          }
          num /= num_adc;
          for(i = 0; i < num_adc; i++)
          {
			    sum[i] = ((DWORD)sum64[i]) / num;
          }
}

//-------------------------------------------------------------------
/*
double REDUCTION::CalculationDiviation(REDUCTION_TOO_DATA *to_rdt, WORD w, WORD m)
{

double dv;

    if(m >= num_adc)
    {
      throw EXP_ERROR((char*)"CalculationDiviation",(char*)"Error Num_adc" );
    }

    double mdvi = abs(to_rdt->hi->midl_loading_in_level - to_rdt->sm->midl_loading_in_level);
    double d_mdvi = abs(w - to_rdt->sm->midl_loading_in_level);
    double kf;
               kf = abs(to_rdt->hi->deviation[m] - to_rdt->sm->deviation[m]);
               kf = kf * d_mdvi;
               kf = kf/mdvi;
           if(to_rdt->hi->deviation[m] > to_rdt->sm->deviation[m])
               dv = (to_rdt->sm->deviation[m] + kf);
           else
               if(to_rdt->hi->deviation[m] < to_rdt->sm->deviation[m])
                            dv = (to_rdt->hi->deviation[m] + kf);
                  else
                   dv = to_rdt->sm->deviation[m];
return dv;
}
*/


