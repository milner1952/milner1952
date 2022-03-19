//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "app_median.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
#include <algorithm>
#include <string>
#include <vector.h>

#define _NUM_TERM 3
#define X_TABL 3
#define Y_TABL 8
using namespace std;
const char *strInit[X_TABL] =   {"A>B ",
                                 "B>C ",
                                 "A>C "};
ARG temp_arg[X_TABL];


COMPARISIJN main_tabl[Y_TABL][X_TABL];
const WORD tabl_bits[] =  {0x0001,0x0002,0x0004,0x0008,
                           0x0010,0x0020,0x0040,0x0080,
                           0x0100,0x0200,0x0400,0x0800,
                           0x1000,0x2000,0x4000,0x8000};
/*
                          {0x8000,0x4000,0x2000,0x1000,
                          0x0800,0x0400,0x0200,0x0100,
                          0x0080,0x0040,0x0020,0x0010,
                          0x0008,0x0004,0x0002,0x0001};
  */
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
  memset(&temp_arg, 0, sizeof(temp_arg));
  char *ch_t1 = new char[(X_TABL * 2)];
  char *ch_t2 = new char[X_TABL];
  memset(ch_t1,0,(X_TABL * 2));
  memset(ch_t2,0,(X_TABL));
  int j = 0;
  for(int i = 0; i < X_TABL; j++)
  {
   ch_t1[i++] =  strInit[j][0];
   ch_t1[i++] =  strInit[j][2];
  }
   j = 0;
  string s1(ch_t1);
  for(string::size_type i = 0; i < s1.length(); i++)
  {
      string::size_type ret1,ret2;
                ret1 = 0;
                ret1 = s1.find(ch_t1[i],ret1);
                string s2(ch_t2);
                ret2 = string::npos;
                ret2 = s2.find(ch_t1[i],0);
                if(ret2 == string::npos)
                {
                       ch_t2[j]= ch_t1[ret1];
                       temp_arg[j].ch_arg = ch_t2[j]; j++;
                }
  }

        srand(1);
  memset(&main_tabl,0 ,sizeof(COMPARISIJN) * X_TABL * Y_TABL);
  delete[] ch_t1;
  delete[] ch_t2;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    CreateMedianTable(3, 8);
}
//---------------------------------------------------------------------------
// ф. CreateMedianTable
// аргументы:
//           x_size_tbl - размер генерируемой таблицы по Х
//           y_size_tbl - размер генерируемой таблицы по Y
// возвращаемое значение - нет
// ф.
// 1. анализирует биты текущего индекса таблицы j:
//            в соответствии с номером бита индекса j
//               выбирает из таблицы  strInit буквы аргументов и код текущего равенства.
// 2.генератором случайных чисел генерируются два числа w1 и w2 и если текущее равенство выполняется
//              то эти числа в качестве аргументоа структуры   COMPARISIJN заносятся в таблицу main_tabl по номеру бита
//              индекса- столбец таблицы и значению индекса - строка таблицы.
// 3 далее осуществляется проверка на соблюдение всех равенств для данной строки таблицы,
//              начиная с нолевого индекса по Х и по текущий индекс, если равенства соблюдаются,
//              то асуществляется переход к следующему биту индекса Y, если нет, то генерация начинаетс сначала-с нолевого бита
//
//------------------------------------------------------------------------------
void __fastcall TForm1::CreateMedianTable(BYTE x_size_tbl, WORD y_size_tbl)
{
      for(int i = 0; i < 8; i++)
      {
                memset(&temp_arg, 0, sizeof(temp_arg));
                for(int i = 0; i < X_TABL; i++)
                {
                        temp_arg[i].w_arg = 0;
                }

                for(int j = 0; j < x_size_tbl; j++)
                {

                  char a1 =   *(char*)(strInit[j]);
                  char a2 =   *((char*)(strInit[j]) + 2);
                  
                  char op =   *((char*)(strInit[j]) + 1);
                  if(op == '>')
                  {
                        if((tabl_bits[j] & i) == 0)
                                op = '<';
                  }
                  else
                  if(op == '<')
                  {
                        if((tabl_bits[j] & i) == 0)
                                op = '>';
                  }

                        main_tabl[i][j].arg1.ch_arg = a1;
                        main_tabl[i][j].op = op;
                        main_tabl[i][j].arg2.ch_arg = a2;
                        WORD w1,w2;
                        if(op == '>')
                        {
                         int a,b;
                                do{
                                        for( a = 0; a <  X_TABL; a++)
                                        {
                                                if(temp_arg[a].ch_arg == a1)
                                                {
                                                        if(temp_arg[a].w_arg == 0)
                                                        {
                                                                w1 = rand(); break;
                                                        }
                                                        else
                                                        {
                                                                w1 = temp_arg[a].w_arg; break
                                                        }
                                                }
                                         }

                                         for( b = 0; b <  X_TABL)
                                         {
                                                if(temp_arg[b].ch_arg == a2)
                                                {
                                                        if(temp_arg[b].w_arg == 0)
                                                        {
                                                                w2 = rand(); break;
                                                        }
                                                        else
                                                        {
                                                                w2 = temp_arg[b].w_arg; break;
                                                        }
                                                }
                                         }
                                 }while(w1 < w2);
                                  temp_arg[a].w_arg = w1;
                                 temp_arg[b].w_arg = w1;
                        }
                     else
                     {
                        do{
                                w1 = rand();
                                w2 = rand();
                        }while(w1 > w2);
                     }



                     /*
                     for(int i = 0; i <  X_TABL)
                        {
                        if(temp_arg[i].ch_arg == a1)
                                if(temp_arg[i].w_arg == 0)
                                {
                                temp_arg[i].w_arg = w1;
                                }

                        if(temp_arg[i].ch_arg == a2)
                                if(temp_arg[i].w_arg == 0)
                                {
                                temp_arg[i].w_arg = w2;
                                }
                        }
                          */
                  main_tabl[i][j].arg1.w_arg = w1;
                  main_tabl[i][j].arg2.w_arg = w2;



                  /*
                  for(int x = 0; x < j; x++)
                  {
                      char op = main_tabl[i][x].op;
                      if(op == '>')
                      {
                                if(main_tabl[i][x].arg1.w_arg > main_tabl[i][x].arg2.w_arg)
                                        continue;
                                else
                                {
                                   memset(&main_tabl[i][x],0,sizeof(COMPARISIJN));
                                   j = 0; break;
                                }
                      }
                      else
                      if(op == '<')
                      {
                                if(main_tabl[i][x].arg1.w_arg < main_tabl[i][x].arg2.w_arg)
                                        continue;
                                else
                                {
                                   memset(&main_tabl[i][x],0,sizeof(COMPARISIJN));
                                   j = 0; break;
                                }
                      }
                  }// for(int x = 0; x < j; x++)
                  */
                }
      }
}
