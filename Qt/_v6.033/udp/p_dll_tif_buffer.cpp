#include "dll_src.h"
#include <string.h>
#include "../exp_error/exp_error.h"
#include "../tiff/tiff_dll.h"
#include "p_dll_tif_buffer.h"
#include <time.h>

extern REGDATA *regdata;


TIFF_WRITE16 g_tiff_Write16;
TIFF_GETERROR g_tiff_GetError;
#ifndef unix
    DWORD  AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW);
#endif
//--------------------------------------------------------------------------
//Вызывается при инициализации dll
// аргументы:
//          - нет
// возвращаемое значение:
//                      - результат инициализации библиотеки tiff
//      функцией tiff_Init();
// коментарии:
//          - функция - генермрует execept если не иожет загрузить библиотеку
//  или функцию из этой библиотеки
//          - функция транслирует except генерируемых tiff_Init();
//---------------------------------------------------------------------------
int      p_dll_tif_bufer_Init(/*string*/const char *dir)
{
EXP_ERROR::ErrorRes();

#ifdef unix
char* ptr_e;
string p = dir;
            p += "/libp_tiff_dll.so";
//---------reg_lib-------------------------------------
 void*   tiff_lib = dlopen(p.c_str(),RTLD_LAZY);
         if(!tiff_lib)
         {
             throw EXP_ERROR("p_dll_tif_bufer_Init","Can not load libp_tiff_dll");
          }

         g_tiff_GetError = (TIFF_GETERROR)dlsym(tiff_lib,"tiff_GetError");
         if( !g_tiff_GetError)
         {
             ptr_e = dlerror();
             throw EXP_ERROR(  (char*)"tiff_GetError", ptr_e);
         }

         g_tiff_Write16 = (TIFF_WRITE16)dlsym(tiff_lib,"tiff_Write16");
         if(!g_tiff_Write16)
         {
             ptr_e = dlerror();
             throw EXP_ERROR(  (char*)"tiff_Write16",ptr_e);
         }

         TIFF_INIT tiff_Init = (TIFF_INIT)dlsym(tiff_lib,"tiff_Init");
         if( !tiff_Init)
         {
             ptr_e = dlerror();
             throw EXP_ERROR( (char*)"tiff_Init", ptr_e);
         }

#else //WINDOWS

HMODULE tiff_lib;
        if(dir == NULL)
               tiff_lib = LoadLibrary(TEXT("p_tiff_dll"));
        else
        {
        string p = dir;
                p += "\\p_tiff_dll";
        wchar_t w_buf[MAX_PATH];
                wmemset(w_buf,L'0',MAX_PATH);
                AnsiToUnicode(p.c_str(), (wchar_t* )&w_buf);
                tiff_lib = LoadLibrary(w_buf);
        }



//HMODULE tiff_lib = LoadLibrary(TEXT("p_tiff_dll"));
    if(!tiff_lib)
    {
             throw EXP_ERROR("p_dll_tif_bufer_Init","Can not load p_tiff_dll");
    }

    g_tiff_GetError = (TIFF_GETERROR)GetProcAddress(tiff_lib,"tiff_GetError");
    if(!g_tiff_GetError)
    {
             throw EXP_ERROR("p_dll_tif_bufer_Init",  (char*)"can not find tiff_GetError");
    }

    g_tiff_Write16 = (TIFF_WRITE16)GetProcAddress(tiff_lib,"tiff_Write16");
    if(!g_tiff_Write16)
    {
             throw EXP_ERROR("p_dll_tif_bufer_Init",  (char*)"can not find tiff_Write16");
    }

    TIFF_INIT tiff_Init = (TIFF_INIT)GetProcAddress(tiff_lib,"tiff_Init");
    if(!tiff_Init)
    {
             throw EXP_ERROR("p_dll_tif_bufer_Init",  (char*)"can not find tiff_Init");
    }

#endif
    return    tiff_Init();
}

//--------------------------------------------------------------------------
//Вызывается при закрытии dll
// аргументы:
//          - нет
// возвращаемое значение:
//                      - нет
// коментарии:
//        функция не реализована, т.к. закрытие tiff-файла происходит сразу после
//   записи. Специально закрывать dll - нет необходимости
//---------------------------------------------------------------------------
void     p_dll_tif_bufer_Close()
{


}

//--------------------------------------------------------------------------
//  Функция ищет минимальное и максимальное значение в буфере данных
// аргументы:
//          - WORD *buf -указатель на буфер
//          - DWORD len - длина буфера
// возвращаемое значение:
//                      - структура MINMAX
// коментарии:
//        - нет
//---------------------------------------------------------------------------
MINMAX  p_dll_tif_bufer_FindMinMax(WORD *buf, DWORD len)
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
         if( nx.w_max > 30000)
          nx.w_max =  nx.w_max;

  }

return nx;
}

//--------------------------------------------------------------------------
//  Функция записывает буфер в tiff файл.
// аргументы:
//          - WORD *buffer -указатель на буфер
//          - long len - длина буфера
//          - WORD iz_binning - размер бининга
// возвращаемое значение:
//           -буфер с записаных данных
// коментарии:
//        - в Linux функция записывает файл в текущую директорию
//      в Windows путь для записии файла  "C:\\Users\\Public
//          Имя файла в обоих случаях составляется из идентификатора p_dll_udp, к
//      которому добавляется год,месяц,день недели,чвс,минута,секунда,милисекунда.
//---------------------------------------------------------------------------
BYTE* p_dll_tif_bufer_Set16(BYTE *buffer, long len, WORD iz_binning)
{

TIFF_WRITE16  tiff_Write16 = g_tiff_Write16;
WORD w_f =   Iz_pxs_per_string/iz_binning;
DWORD h_f = len/w_f;
h_f /= 2;

#ifndef unix
    SYSTEMTIME st;
    GetLocalTime(&st);
    string n_f = "C:\\Users\\Public\\p_dll_udp_";

    char b[10];

    memset(b, 0, 10);
    sprintf(b, "%d", st.wYear);
    n_f += b;
    n_f += " ";

    memset(b, 0, 10);
    sprintf(b, "%d", st.wMonth);
    n_f += b;
    n_f += " ";

    memset(b, 0, 10);
    sprintf(b, "%d", st.wDay);
    n_f += b;
    n_f += "  ";

    memset(b, 0, 10);
    sprintf(b, "%d", st.wHour);
    n_f += b;
    n_f += " ";

    memset(b, 0, 10);
    sprintf(b, "%d", st.wMinute);
    n_f += b;
    n_f += " ";

    memset(b, 0, 10);
    sprintf(b, "%d", st.wSecond);
    n_f += b;
    n_f += " ";

    memset(b, 0, 10);
    sprintf(b, "%d", st.wMilliseconds);
    n_f += b;
    n_f += ".tiff";

#else
    string n_f = "./p_dll_udp_";
    time_t ft       =   time(NULL);
    struct tm *ltm          = localtime(&ft);
    char *asc_time          = asctime(ltm);
    n_f += asc_time;

    n_f.erase(n_f.end() - 1);
    n_f += ".tif";
#endif

tiff_Write16((const char*)n_f.c_str(), w_f, h_f, (char*)buffer);
return (BYTE*)buffer;
}


