
#include "test_dll.h"




#include <fcntl.h>

#ifdef unix
#include <unistd.h>
#include <errno.h>
#endif


#include <ctime>
#include <string.h>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "../exp_error/exp_error.h"
#ifndef unix
    FILE *hFile;
#else
    int hFile;
#endif



    DWORD EXP_ERROR::error;
    string EXP_ERROR::text_out;
static int iz_log_debug;


//--------------------------------------------------------------------------
//Вызывается при закрытии dll
// аргументы:
//          - нет
// возвращаемое значение:
//          - нет
// коментарии:
//          -функция - если iz_log_debug == 0 - функция возвращается, т. к. dll
//      не записывает отладочную информацию
//          -функция закрывает файл с отладочной информацией
//---------------------------------------------------------------------------
void test_dll_Close()
{
    if(iz_log_debug == 0)
        return;
    else
    if(hFile > 0)
#ifndef unix
        fclose(hFile);
#else
        close(hFile);
#endif
}
//--------------------------------------------------------------------------
//Вызывается при инициализации dll
// аргументы:
//          - char *note - путь к файлу для записи отладочной информации
//          - unsigned int iz_debug - флаг разрешения записии отладочной информации
// возвращаемое значение:
//          - 0 - при отсутствии ошибок
//          - 1 при наличии ошибок
// коментарии:
//          - если iz_log_debug == 0 - функция возвращается, т. к. dll
//      не записывает отладочную информацию
//          - функция - обертка для обработок except.
//---------------------------------------------------------------------------
bool test_dll_Init(char *note, unsigned int iz_debug)
{
    EXP_ERROR::ErrorRes();
    try
    {
    test_dll_InitFn(note, iz_debug);
    }
    catch(EXP_ERROR &e)
    {
           e.ErrorSetText("p_test_dll");
        return 1;
    }
    return 0;
}
//--------------------------------------------------------------------------
//Вызывается при инициализации dll
// аргументы:
//          - char *note - путь к файлу для записи отладочной информации
//          - unsigned int iz_debug - флаг разрешения записии отладочной информации
// возвращаемое значение:
//          - нет
// коментарии:
//          - если iz_log_debug == 0 - функция возвращается, т. к. dll
//      не записывает отладочную информацию
//          - функция открывает файл, имя которого составлено из пукти к файлу, переданому
//          - в аргументе - note, дня недели, месяца, числа месяца, часов, минут, секунд, года
//      открытия файла
//          - функция выбрасывает исключения если:
//      -она не может в парамете note найти конечныесимволы"\\"
//      -она не может открыть файл
//      - переместить указатель файла в конец файла
//      - измерить его длину
//      - записать путь к файлу (аргумент note) в файл
//---------------------------------------------------------------------------
void test_dll_InitFn(char *note, unsigned int iz_debug)
{
        iz_log_debug = iz_debug;

        if(iz_debug == 0)
        return;

char *ctr;
        tm *tim;
        time_t t = 0L;
        time(&t);
        tim = localtime(&t);
        ctr = asctime(tim);
{
string  s;
#ifdef unix
  s = "./";
#endif

        s += ctr;

string::iterator beg;

            for(beg = s.begin(); beg != s.end();)
            {
            char c = *beg;
                    if(c == ':')
                        *beg = ' ';
                    beg++;
            }
         s.erase((s.end() - 1));
         s += ".txt";

#ifndef unix
string sn = note;

size_t e;

            if(string::npos == (e = sn.find_last_of("\\",sn.length())))
                throw EXP_ERROR("test_dll_InitFn", "Can not find_last_of(""\\\\"") ");
            sn.erase(sn.begin() + e, sn.end());
            sn += "\\";
            sn += s;
            hFile = (FILE*)fopen(sn.c_str(), "w+");
            if(hFile == NULL)
#else
            hFile = (int)open(s.c_str(), O_RDWR|O_CREAT, 0777);
            if(hFile < 0)
#endif
            {
             string      e("Cannot open file: ");
                    e += s;
                    throw EXP_ERROR("test_dll_InitFn", e.c_str(), (DWORD)errno);
            }
#ifndef unix
 int er =  fseek(hFile,0,SEEK_SET);
            if(er != 0)
            {
                    throw EXP_ERROR("test_dll_InitFn","Cannot seek file",(WORD)1);
            }

            er = fseek(hFile,0,SEEK_END);
            if(er != 0)
            {
                    throw EXP_ERROR("test_dll_InitFn","Cannot seek file",(WORD)2);
            }
long int ls_z = ftell(hFile);
            if(ls_z < 0)
            {
                    throw EXP_ERROR("test_dll_InitFn","Cannot ftell file",(WORD)3);
            }

#else
            off_t ls_z = lseek(hFile,0,SEEK_SET);
            if(ls_z < 0)
            {
                throw EXP_ERROR("test_dll_InitFn","Cannot lseek file", (DWORD)errno);
            }
#endif
}
string s(note);
     s += "\n";
#ifndef unix
     size_t  wr_z = fwrite((const void*)s.c_str(), 1, s.size(), hFile );
            if(wr_z != (size_t)s.size())
            {
                throw EXP_ERROR("test_dll_InitFn","Cannot write file",(WORD)4);
            }
#else
     ssize_t wr_z = write(hFile, (const void*)s.c_str(), s.size());    
            if(wr_z != (ssize_t)s.size())
            {
                throw EXP_ERROR("test_dll_InitFn","Cannot write file",(DWORD)errno);
            }
#endif
}

//--------------------------------------------------------------------------
//Вызывается при записи в файл отладочной информации
// аргументы:
//          - char *ptr - указатель на записываемую строку
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - нет
// коментарии:
//          - функция записывает в файл строку *ptr и добавляет к ней время записи при iz_time == 1
//      не записывает отладочную информацию
//          - функция выбрасывает исключения если:
//          - выполнить запись в файл
//---------------------------------------------------------------------------
void test_dll_OutputDebugString(char *ptr, bool iz_time )
{

char *ctr;
        string s(ptr);
        s+= " ";
        if(iz_time)
        {
            tm *tim;
            time_t t = 0L;
            time(&t);
            tim = localtime(&t);
            ctr = asctime(tim);
            s+= ctr;
        }
        else
        s+= "\n";

#ifndef   unix
        size_t  wr_z = fwrite((const void*)s.c_str(), 1, s.size(), hFile );
                if(wr_z != (size_t)s.size())
#else
        ssize_t wr_z = write(hFile, (const void*)s.c_str(), s.size());
                if(wr_z != (ssize_t)s.size())
#endif
                {
                    throw EXP_ERROR("test_dll_OutputDebugString","Cannot write file");
                }
}



//1
//--------------------------------------------------------------------------
//Вызывается при записи в файл отладочной информации: строки и двойного слова
// аргументы:
//          - const char* st - указатель на записываемую строку
//          - DWORD w - записываемое двойное слово
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - 0 при успехе
//          - 1 при ошибке
// коментарии:
//          - функция записывает в файл строку *st и добавляет к ней двойное
//      слово w и время записи при iz_time == 1
//          - функция транслирует исключения выбрасываемые test_dll_OutputDebugString
//---------------------------------------------------------------------------
bool test_dll_Show_s_dw(const char* st , DWORD w, bool iz_time)
{

if(iz_log_debug == 0)
return 0;
            string ain(st);
char buf[10];
            memset(buf, 0, 10);
            sprintf((char*)buf,"%lu",w);
            ain += buf;
            try
            {
            test_dll_OutputDebugString((char*)ain.c_str(),iz_time);
            }
            catch(EXP_ERROR &e)
            {


                 e.ErrorSetText("p_test_dll - test_dll_Show_s_dw");
                return 1;
            }
return 0;
}


//2
//--------------------------------------------------------------------------
//Вызывается при записи в файл отладочной информации: строки, двойного слова и дополнительной строки
// аргументы:
//          - const char* st - указатель на записываемую строку
//          - DWORD w - записываемое двойное слово
//          - const char* st2-дополнительная строка
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - 0 при успехе
//          - 1 при ошибке
// коментарии:
//          - функция записывает в файл строку *st, двойное слово w и добавляет к ним строку *st2
//      и время записи при iz_time == 1
//          - функция транслирует исключения выбрасываемые test_dll_OutputDebugString
//---------------------------------------------------------------------------
bool test_dll_Show_s_dw_s(const char* st, DWORD dw, const char* st2, bool iz_time)
{

if(iz_log_debug == 0)
return 0;
        string ain(st);
        char b[100]; memset(b, 0, 100);
        ain += ":";
        sprintf(b, "%ld", dw);
        ain += b;
        ain += ", ";
        ain += st2;
        try
        {
        test_dll_OutputDebugString((char*)ain.c_str(), iz_time);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_test_dll - test_dll_Show_s_dw_s");
            return 1;
        }
return 0;
}


//3       //(char* , unsigned char*)
//--------------------------------------------------------------------------
//  Вызывается при записи в файл отладочной информации: строки, и  масива байт,
//конец которого обозначается нолевым байтом
// аргументы:
//          - const char* st - указатель на записываемую строку
//          - const unsigned char* - масив байт, конец которого обозначается нолевым байтом
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - 0 при успехе
//          - 1 при ошибке
// коментарии:
//          - функция записывает в файл строку *st, масив байт bt и добавляет к ним
//      время записи при iz_time == 1
//          - функция транслирует исключения выбрасываемые test_dll_OutputDebugString
//---------------------------------------------------------------------------
bool test_dll_Show_s_us(const char* st, const unsigned char* bt, bool iz_time)
{

if(iz_log_debug == 0)
return 0;

        string ain(st);
        ain += ":";
char t[20];
        memset(t,0,20);
        sprintf(t,"%X",*bt);
        ain += t;
        try
        {
        test_dll_OutputDebugString((char*)ain.c_str(), iz_time);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_test_dll - test_dll_Show_s_us");
            return 1;
        }
return 0;
}


//5     //(char* , double)
//--------------------------------------------------------------------------
//Вызывается при записи в файл отладочной информации: строки, значение с плавающей точкой
// аргументы:
//          - const char* st - указатель на записываемую строку
//          - double d - значение с плавающей точкой
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - 0 при успехе
//          - 1 при ошибке
// коментарии:
//          - функция записывает в файл строку *st, значение с плавающей точкой d и добавляет к ним
//      время записи при iz_time == 1
//          - функция транслирует исключения выбрасываемые test_dll_OutputDebugString
//---------------------------------------------------------------------------
bool test_dll_Show_s_d(const char* st , double d, bool iz_time)
{
if(iz_log_debug == 0)
return 0;
        string ain(st);
        ain += ":";
   char b[100]; memset(b, 0, 100);
        sprintf(b, "%f", d);
        ain += b;
        try
        {
        test_dll_OutputDebugString((char*)ain.c_str(), iz_time);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_test_dll - test_dll_Show_s_d");
            return 1;
        }
return 0;
}



//6       //(char* , char*)
//--------------------------------------------------------------------------
//Вызывается при записи в файл отладочной информации: строки и одного байта
// аргументы:
//          - const char* st - указатель на записываемую строку
//          - const char bt - байт для записи
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - 0 при успехе
//          - 1 при ошибке
// коментарии:
//          - функция записывает в файл строку *st, значение байта bt и добавляет к ним
//      время записи при iz_time == 1
//          - функция транслирует исключения выбрасываемые test_dll_OutputDebugString
//---------------------------------------------------------------------------
bool test_dll_Show_s_bt(const char* st, const char bt, bool iz_time)
{
if(iz_log_debug == 0)
return 0;

        string ain(st);
char buf[10];
        memset(buf, 0, 10);
        sprintf((char*)buf,"%d",bt);
        ain += buf;
        try
        {
        test_dll_OutputDebugString((char*)ain.c_str(), iz_time);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_test_dll - test_dll_Show_s_bt");
            return 1;
        }

return 0;
}

// 7
//--------------------------------------------------------------------------
//  Вызывается при записи в файл отладочной информации: строки, двойного слова,
//дополнительной строки, дополнительного двойного слова
// аргументы:
//          - const char* st - указатель на записываемую строку
//          - DWORD dw - двойное слово
//          - const char* st2 - указатель на дополнительную строку
//          - DWORD dw2 - дополнительное двойное слово
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - 0 при успехе
//          - 1 при ошибке
// коментарии:
//          - функция записывает в файл строку *st, двойное слово dw, строку *st2,
//      двойное слово dw2 и добавляет время записи при iz_time == 1
//          - функция транслирует исключения выбрасываемые test_dll_OutputDebugString
//---------------------------------------------------------------------------
bool test_dll_Show_s_dw_s_dw(char* st, DWORD dw, char* st2, DWORD dw2, bool iz_time)
{
if(iz_log_debug == 0)
return 0;

        string ain(st); ain += ":";

char buf[10];
        memset(buf, 0, 10);
        sprintf((char*)buf,"%ld",dw);
        ain += buf;

        ain += "  ";
        ain += st2; ain += ":";        
        memset(buf, 0, 10);
        sprintf((char*)buf,"%ld",dw2);
        ain += buf;
        try
        {
        test_dll_OutputDebugString((char*)ain.c_str(), iz_time);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_test_dll - test_dll_Show_s_dw_s_dw");
            return 1;
        }
return 0;
}

//8
//--------------------------------------------------------------------------
//  Вызывается при записи в файл отладочной информации: строки и дополнительной строки
// аргументы:
//          - const char* st - указатель на записываемую строку
//          - DWORD dw - двойное слово
//          - const char* st2 - указатель на дополнительную строку
//          - DWORD dw2 - дополнительное двойное слово
//          - bool iz_time - необходимость в добавлении к параметру времени записи
// возвращаемое значение:
//          - 0 при успехе
//          - 1 при ошибке
// коментарии:
//          - функция записывает в файл строку *st, двойное слово dw, строку *st2,
//      двойное слово dw2 и добавляет время записи при iz_time == 1
//          - функция транслирует исключения выбрасываемые test_dll_OutputDebugString
//---------------------------------------------------------------------------
bool test_dll_Show_s_s(const char* st,const  char* st2, bool iz_time)
{
if(iz_log_debug == 0)
return 0;

        string ain(st); ain += ": ";
        ain += st2;
        try
        {
        test_dll_OutputDebugString((char*)ain.c_str(), iz_time);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_test_dll - test_dll_Show_s_s");
            return 1;
        }
return 0;
}
const char ASC[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
//------------------------------------------------------------------------------
bool test_dll_Hex_Bufer_Too_String(char* out_str, const WORD *in_hex, DWORD num)
{
    DWORD i = 0;
    DWORD j = 0;
    WORD temp;
  //  WORD dem_temp;
    DWORD z;
        z = 0;
  for(; i < num;i++)
  {
      char tl,tml,tmh,th;
      temp = in_hex[i];
    //  dem_temp = temp;

      tl = (temp & 0x000f);
      tml = ((temp & 0x00f0)>>4);

      temp = temp >> 8;

      tmh = (temp & 0x00f);
      th = ((temp & 0x00f0)>>4);
        z++;
      out_str[j++] = ASC[(int)th];
      out_str[j++] = ASC[(int)tmh];

      out_str[j++] = ASC[(int)tml];
      out_str[j++] = ASC[(int)tl];

      out_str[j++] = ' ';


      if(z == 255)
      {
          z = 0;
          out_str[j++] = '\r';
          out_str[j++] = '\n';
      }
  }
  out_str[j] = 00;
  return 0;
}


//--------------------------------------------------------------------------
//DWORD test_GetError()-  Возвращает наличие ошибок
// аргументы:
//          -нет
// возвращаемое значение:
//          - 0 при отсутствии ошибок
//          - EXP_ERROR::ErrorGet() - при наличии ошибок
// коментарии:
//          - функционал функции может быть расширен в будующем
//---------------------------------------------------------------------------
DWORD test_GetError(char **text)
{
    DWORD ret;
    if((ret = EXP_ERROR::ErrorGet()))
        *text = (char*)EXP_ERROR::GetTextError()->c_str();
    else
        *text = NULL;
    return ret;
}
