


#ifdef unix
    #include <stdlib.h>
#endif



#include <wchar.h>
#include <string.h>
//#include <tiffio.h>
#include "tiffio.h"


#include "../exp_error/exp_error.h"
#include "tiff_dll.h"


TIFF *tiff = (TIFF*)NULL;
char *image = (char*)NULL;
uint32 width;
uint32 height;
uint32* raster;
static bool flagMainApp;


#ifdef unix
    #include <dlfcn.h>
    #include <dlfcn.h>

    typedef void* (* TIFFMALLOC)(tmsize_t);
#else
    typedef void* (* TIFFMALLOC)(tsize_t);
#endif
    void *tiff_lib;
    typedef TIFF* (*TIFFOPEN)(const char*, const char*);
    typedef int   (*TIFFSETFIELD)(TIFF*,uint32, ...);
    typedef uint32 (*TIFFDEFAULTSTRIPSIZE)(TIFF* tif, uint32 request);
    typedef int (*TIFFWRITESCANLINE)(TIFF* tif, void* buf, uint32 row, uint16 sample);
    typedef void (*TIFFCLOSE)(TIFF*);
    typedef void (*TIFFFREE)(void*);
    typedef int  (*TIFFGETFILED)(TIFF*, ttag_t, ...);
    typedef int (*TIFFREADRGBAIMAGE)(TIFF*, uint32, uint32, uint32*, int);


    TIFFOPEN g_TIFFOpen;
    TIFFSETFIELD g_TIFFSetField;
    TIFFMALLOC   g_TIFFmalloc;
    TIFFDEFAULTSTRIPSIZE g_TIFFDefaultStripSize;
    TIFFWRITESCANLINE g_TIFFWriteScanline;
    TIFFCLOSE g_TIFFClose;
    TIFFFREE g_TIFFfree;
    TIFFGETFILED g_TIFFGetField;
    TIFFREADRGBAIMAGE g_TIFFReadRGBAImage;

    DWORD EXP_ERROR::error;
    string EXP_ERROR::text_out;

//--------------------------------------------------------------------------
//Вызывается при инициализации dll
// аргументы:
//          - нет
// возвращаемое значение:
//          - 0 - при отсутствии ошибок
//          - 1 при наличии ошибок
// коментарии:
//          - функция - обертка для обработок except генерируемых int tiff_Load();
//---------------------------------------------------------------------------
bool tiff_Init()
{
EXP_ERROR::ErrorRes();
        try
        {
  return    !(tiff_Load() == 0);
        }
        catch(EXP_ERROR &e)
        {
            e.ErrorSetText("p_tiff_dll");
            return 1;
        }
return 0;
}

//--------------------------------------------------------------------------
//Вызывается при инициализации dll
// аргументы:
//          - нет
// возвращаемое значение: - 0
// коментарии:
//          - функция - генермрует execept если не иожет загрузить библиотеку
//  или функцию из этой библиотеки
//---------------------------------------------------------------------------
int tiff_Load()
{
flagMainApp = 0;
#ifdef unix
static char *ptr_e;
        tiff_lib = dlopen("libtiff.so",RTLD_LAZY);
        if(!tiff_lib)
        {            
            if(ptr_e != NULL)
                throw EXP_ERROR("tiff_Load",ptr_e);
        }

        g_TIFFOpen  = (TIFFOPEN)dlsym(tiff_lib,"TIFFOpen");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFSetField = (TIFFSETFIELD)dlsym(tiff_lib,"TIFFSetField");
        if((ptr_e = dlerror()) != NULL)
        {
             throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFmalloc = (TIFFMALLOC)dlsym(tiff_lib,"_TIFFmalloc");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFDefaultStripSize = (TIFFDEFAULTSTRIPSIZE)dlsym(tiff_lib,"TIFFDefaultStripSize");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFWriteScanline = (TIFFWRITESCANLINE)dlsym(tiff_lib,"TIFFWriteScanline");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFClose = (TIFFCLOSE)dlsym(tiff_lib,"TIFFClose");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFfree = (TIFFFREE)dlsym(tiff_lib,"_TIFFfree");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFGetField = ( TIFFGETFILED)dlsym(tiff_lib,"TIFFGetField");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);
        }

        g_TIFFReadRGBAImage = (TIFFREADRGBAIMAGE)dlsym(tiff_lib,"TIFFReadRGBAImage");
        if((ptr_e = dlerror()) != NULL)
        {
            throw EXP_ERROR("tiff_Load", ptr_e);;
        }

#else
DWORD e;


/*
DWORD len = 5000;
LPTSTR bf[5000];
DWORD er = GetDllDirectory(  len, (LPWSTR)bf );
if(er == 0L)
{
    er = GetLastError();
    er = 0;
}

C:\Program Files (x86)\GnuWin32
C:\Program Files (x86)\GnuWin32

MessageBox(NULL, (LPCWSTR)bf,TEXT("TEST"), MB_OK);


   //     HMODULE tiff_lib = LoadLibrary(TEXT("C:\\Windows\\System32\\libtiff3.dll"));
   //     HMODULE tiff_lib = LoadLibraryEx(TEXT("C:\\Windows\\System32\\libtiff3.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
   //     HMODULE tiff_lib = LoadLibraryEx(TEXT("C:\\Program Files (x86)\\GnuWin32\\bin\\libtiff3.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
//wchar_t* n = wcsrchr(w_buf, L'\\');
//wcscpy(n, TEXT("\\TIFF_TST\\libtiff3.dll"));






wchar_t w_buf[MAX_PATH];
            wmemset(w_buf,L'0',MAX_PATH);
            if(patch_ini == NULL)
            {
                    if(!GetModuleFileName(NULL, w_buf, MAX_PATH))
                            throw EXP_ERROR("p_dll_Init","GetModuleFileName - Can' not resolve m_name");
                    wchar_t* n = wcsrchr(w_buf, L'\\');
                    wcscpy(n, TEXT("\\p_dll_udp_init.ini"));
            }
            else
            {
                    if(AnsiToUnicode(patch_ini, (wchar_t*)w_buf))
                         throw EXP_ERROR("p_dll_Init","can not exec AnsiToUnicode");
                    wcscat(w_buf,TEXT("\\p_dll_udp_init.ini"));
D:\MAP\S_KAMERA\build-group_s_test_app-Desktop_Qt_5_7_1_MSVC2013_32bit-Debug\TIFF_TST\


    //    HMODULE tiff_lib = LoadLibraryEx(TEXT("C:\\Program Files\\GnuWin32\\bin\\libtiff3.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

*/

 //       HMODULE tiff_lib = LoadLibraryEx(TEXT("D:\\MAP\\S_KAMERA\\build-group_s_test_app-Desktop_Qt_5_7_1_MSVC2013_32bit-Debug\\TIFF_TST\\libtiff3.dll"), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

wchar_t w_buf[MAX_PATH];
        if(!GetModuleFileName(NULL, w_buf, MAX_PATH))
                throw EXP_ERROR("tiff","GetModuleFileName - Can' not resolve m_name");
wchar_t* wptr = wcsrchr(w_buf, L'\\');
         wcscpy(wptr, TEXT("\\TIFF_TST\\libtiff3.dll"));

HMODULE tiff_lib = LoadLibraryEx(w_buf, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        if(!tiff_lib)
        {
           e = GetLastError();
           throw EXP_ERROR("libtiff3.dll",(char*)"LoadLibrary",(DWORD)e);
        }



        g_TIFFOpen = (TIFFOPEN)GetProcAddress(tiff_lib,"TIFFOpen");
        if(!g_TIFFOpen)
        {
            e = GetLastError();
            throw EXP_ERROR("tiff_Load","can not find _TIFFOpen",e);
        }

        g_TIFFSetField = (TIFFSETFIELD)GetProcAddress(tiff_lib,"TIFFSetField");
        if(!g_TIFFSetField)
        {
            e = GetLastError();
            throw EXP_ERROR("tiff_Load","can not find TIFFFSetFiled",e);
        }

        g_TIFFmalloc = (TIFFMALLOC)GetProcAddress(tiff_lib,"_TIFFmalloc");
        if(!g_TIFFmalloc)
        {
            e = GetLastError();
            throw EXP_ERROR("tiff_Load","can not find TIFFmalloc",e);
        }

        g_TIFFDefaultStripSize = (TIFFDEFAULTSTRIPSIZE)GetProcAddress(tiff_lib,"TIFFDefaultStripSize");
        if(!g_TIFFDefaultStripSize)
        {
            e = GetLastError();
            throw EXP_ERROR("tiff_Load","can not find TIFFDefaultStrioSize",e);
        }

        g_TIFFWriteScanline = (TIFFWRITESCANLINE)GetProcAddress(tiff_lib,"TIFFWriteScanline");
        if(!g_TIFFWriteScanline)
        {
            e = GetLastError();
            throw EXP_ERROR("tiff_Load","can not find TIFFWriteScanLine",e);
        }

        g_TIFFClose = (TIFFCLOSE)GetProcAddress(tiff_lib,"TIFFClose");
        if(!g_TIFFClose)
        {
            e = GetLastError();
            throw EXP_ERROR("tiff_Load","can not find TIFFClose",e);
        }

        g_TIFFfree = (TIFFFREE)GetProcAddress(tiff_lib,"_TIFFfree");
        if(!g_TIFFfree)
        {
            e = GetLastError();
            throw EXP_ERROR("tiff_Load","can not find TIFFfree",e);
        }

         g_TIFFGetField = ( TIFFGETFILED)GetProcAddress(tiff_lib,"TIFFGetField");
         if(!g_TIFFfree)
         {
             e = GetLastError();
             throw EXP_ERROR("tiff_Load","can not find TIFFGetField",e);
         }

         g_TIFFReadRGBAImage = (TIFFREADRGBAIMAGE)GetProcAddress(tiff_lib,"TIFFReadRGBAImage");
         if(!g_TIFFReadRGBAImage)
         {
             e = GetLastError();
             throw EXP_ERROR("tiff_Load","can not find TIFFReadRGBAImage",e);
         }

#endif
         return 0;
}

//--------------------------------------------------------------------------
//Вызывается при записи в tiff - файл
// аргументы:
//          - const char *tiff_name- имя tiff-файла
//          - WORD width- ширина картинки в пикселях
//          - unsigned long height- высота картинки в пикселях
//          - char *dt- данные для записи
// возвращаемое значение:
//          - 0 - при отсутствии ошибок
//          - 1 при наличии ошибок
// коментарии:
//          - функция - обертка для обработок except генерируемых функцией
//     bool tiff_Write16data(const char *tiff_name, WORD width, unsigned long height, char *dt);
//---------------------------------------------------------------------------
bool tiff_Write16(const char *tiff_name, WORD width, unsigned long height, char *dt)
{
            try
            {
            tiff_Write16data(tiff_name, width, height, dt);
            }
            catch(EXP_ERROR &e)
            {
                 e.ErrorSetText("p_tiff_dll");
            return 1;
            }
    return 0;
}

//--------------------------------------------------------------------------
//Вызывается при записи в tiff - файл
// аргументы:
//          - const char *tiff_name- имя tiff-файла
//          - WORD width- ширина картинки в пикселях
//          - unsigned long height- высота картинки в пикселях
//          - char *dt- данные для записи
// возвращаемое значение: - 0
// коментарии:
//          - функция генерирует except если:
//     - не может выделить память под буфер изображения
//     - не может открыть файл
//     - не может выделить память под буфкр строки
//---------------------------------------------------------------------------
bool tiff_Write16data(const char *tiff_name, WORD width, unsigned long height, char *dt)
{

char *data = new char[width * height * 2 +  5];
    if (!data)
    {
        throw EXP_ERROR("tiff_Write16data","can not malloc buf data");
    }


    memset(data, 0, width * height * 2 + 5);
    memcpy(data,dt, width * height * 2);

    if(flagMainApp == 1)
        tiff_TransformationToRange16(width, height, (WORD *)data);

    TIFFOPEN TIFFOpen = g_TIFFOpen;
    TIFFSETFIELD TIFFSetField = g_TIFFSetField;
    TIFFMALLOC _TIFFmalloc = g_TIFFmalloc;
    TIFFFREE   _TIFFfree = g_TIFFfree;

    TIFFWRITESCANLINE TIFFWriteScanline = g_TIFFWriteScanline;
    TIFFCLOSE TIFFClose =     g_TIFFClose;

    TIFF* tiff = TIFFOpen(tiff_name, "w");
    if(!tiff)
    {
        delete[] data;
        string s = "can not open file: ";
        s += tiff_name;
        throw EXP_ERROR("tiff_Write16data", s.c_str());
    }

 //FILLORDER_LSB2MSB
//    TIFFSetField(tiff, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
//TIFFSetField(tiff, TIFFTAG_FILLORDER, FILLORDER_LSB2MSB);
    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    tsize_t linebytes = (tsize_t)width * 2;
    uint16 *buf = NULL;
    buf =(uint16 *)_TIFFmalloc(linebytes);
    if (!buf)
    {
        delete[] data;
        TIFFClose(tiff);
        throw EXP_ERROR("tiff_Write16data","can not malloc buf in TIFFmalloc");
    }
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, height);

    for (uint32 row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++) {
            buf[col] = (uint16)((WORD*)data)[ row * width +  col];
        }
        if (TIFFWriteScanline(tiff, buf, row, 0) < 0)
        break;
    }

    if (buf)
    {
        _TIFFfree(buf);
    }
    TIFFClose(tiff);

delete[] data;
    return 0;
}

//--------------------------------------------------------------------------
//Вызывается при чтении из  tiff - файла
// аргументы:
//          - const char *tiff_name- имя tiff-файла
// возвращаемое значение:
//          - 0 - при отсутствии ошибок
//          - 1 при наличии ошибок
// коментарии:
//          - функция - обертка для обработок except генерируемых функцией
//     bool tiff_ReadData(char *tiff_name);
//---------------------------------------------------------------------------
bool tiff_Read(char *tiff_name)
{
            try
            {
            tiff_ReadData(tiff_name);
            }
            catch(EXP_ERROR &e)
            {
              e.ErrorSetText("p_tiff_dll");
          //  e.ErrorShow("p_tiff_dll", "tiff_Init");
            return 1;
            }
return 0;
}

//--------------------------------------------------------------------------
//Вызывается при чтении tiff - файлa
// аргументы:
//          - const char *tiff_name- имя tiff-файла
// возвращаемое значение: - 0
// коментарии:
//          - функция генерирует except если:
//     - не может открыть файл
//     - не может выделить память под буфер изображения
//     - не может прочитать файл
//---------------------------------------------------------------------------
bool tiff_ReadData(char *tiff_name)
{
        TIFFGETFILED TIFFGetField = g_TIFFGetField;
        TIFFOPEN TIFFOpen = g_TIFFOpen;
        TIFFMALLOC  _TIFFmalloc = g_TIFFmalloc;
        TIFFREADRGBAIMAGE TIFFReadRGBAImage = g_TIFFReadRGBAImage;
        tiff = TIFFOpen(tiff_name, "r");
        if(tiff != NULL)
        {
            tiff_Close();
            throw EXP_ERROR("tiff_ReadData","read non close file");
        }

        TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
        uint32 n_pixels = width * height;
        raster =  (uint32*)_TIFFmalloc(n_pixels *sizeof(uint32));
        if(raster == NULL)
        {
            throw EXP_ERROR("tiff_ReadData", "can not malloc to raster in TIFFmalloc");
        }

        if(!TIFFReadRGBAImage(tiff, width, height, (long unsigned int*)raster, 0))
        {
            tiff_Close();
            throw EXP_ERROR("tiff_ReadData","read non close file");
        }
        if(tiff != NULL)
                tiff_Close();
return 0;
}


//--------------------------------------------------------------------------
//Вызывается при закрытии tiff - файлa после записи/чтения
// аргументы:
//          - нет
// возвращаемое значение: - нет
// коментарии: - нет
//---------------------------------------------------------------------------
void tiff_Close()
{
        TIFFFREE  _TIFFfree = g_TIFFfree;
        TIFFCLOSE TIFFClose = g_TIFFClose;
        if(image != NULL)
        {
            delete[] image;
            image = 0;
        }

        if(raster != NULL)
        {
            _TIFFfree(raster);
            raster = NULL;
        }

        if(tiff != NULL)
        {
        TIFFClose(tiff);
        tiff = NULL;
        }
}


//--------------------------------------------------------------------------
//Вызывается при записи tiff - файлa
// аргументы:
//          - unsigned long width - ширина изображения в пикселях
//          - unsigned long height -  высота изображения в строках
//          - WORD *data - буфер изображения
// возвращаемое значение: - нет
// коментарии: - функция приводит диапазон значений содержащихся в буфере 0 - 65535
//        к диапазону отображаемых значений 0-255
//---------------------------------------------------------------------------
void tiff_TransformationToRange16(unsigned long width, unsigned long height, WORD *data)
{
DWORD  len = width * height;
MINMAX nx = tiff_FindMinMax(data +   15 * width, len - 30 * width);
double kf = 65536.0/(double)(nx.w_max - nx.w_min);
WORD w;
  for(DWORD i = 0; i < len; i++)
  {
      w = data[i];
      w = w - nx.w_min;
      w = WORD(w * kf);
      data[i] = w;
  }
}

//--------------------------------------------------------------------------
//Вызывается при записи tiff - файлa
// аргументы:
//          - WORD *buf - буфер данных
//          - DWORD len - длина буфера в пикселях
// возвращаемое значение: - структура MINMAX
// коментарии: - функция ищет минимальное и максимальное значение в буфере
//---------------------------------------------------------------------------
MINMAX  tiff_FindMinMax(WORD *buf, DWORD len)
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
//          DWORD tiff_GetError возвращает ошибки, фиксируемые исключительными ситуациями
// аргументы:
//         нет
// возвращаемое значение:
//         1 - наличие ошибки
//         0 - ошибок не было
// коментарии:
//         нет
//---------------------------------------------------------------------------
DWORD tiff_GetError(char **text)
{
    DWORD ret;
    if((ret = EXP_ERROR::ErrorGet()))
        *text = (char*)EXP_ERROR::GetTextError()->c_str();
    else
        *text = NULL;
    return ret;
}
//--------------------------------------------------------------------------------------------
//      функция устанавливает флаг MAIN_APP
//      аргументы: - нет
//  возвращаемое значение:
//                  - нет
//  коментарии: - нет
//------------------------------------------------------------------------------------------------
void tiff_SetFlagMainAPP(bool m)
{
    flagMainApp = m;
}

#ifndef unix
DWORD  tiff_UnicodeToAnsi(TCHAR *pszW, LPSTR *ppszA)
{
    ULONG cbAnsi, cCharacters;
    DWORD dwError;


    if (pszW == NULL)
    {
        throw EXP_ERROR("UnicodeToAnsi","Wrong argument pszW == NULL");
    }

    cCharacters = wcslen((const wchar_t*)pszW)+1;

    cbAnsi = cCharacters*2;


    *ppszA = (LPSTR) new ULONG[cbAnsi];
    if (NULL == *ppszA)
    {
      throw EXP_ERROR("UnicodeToAnsi","Can not allocate memory");
    }

    if (0 == WideCharToMultiByte(CP_ACP, 0, (LPWCH)pszW, cCharacters, *ppszA,
                  cbAnsi, NULL, NULL))
    {
        dwError = GetLastError();
        delete[]  ppszA;
        *ppszA = NULL;
        return HRESULT_FROM_WIN32(dwError);
    }
    return 0;
}
#endif
