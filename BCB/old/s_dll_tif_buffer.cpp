//---------------------------------------------------------------------------

#ifdef __BORLANDS__
    #pragma hdrstop
#endif
#ifdef unix
    #include <string.h>
#endif

#include "tiff_dll.h"
#include "s_dll_tif_buffer.h"

RGB  rgb_bufer [1152 * 1152 * sizeof(RGB)];
WORD w_bmi;
WORD h_bmi;
extern REGDATA *regdata;

#ifdef unix
    static char *ptr_e;
    void * tiff_lib;
    TIFF_WRITE g_tiff_Write;
    //TIFF_INIT g_tiff_Init;
#endif




//---------------------------------------------------------------------------
int     d_dll_tif_bufer_Init(WORD h, WORD w)
{

    w_bmi = w;
    h_bmi = h;
     memset(rgb_bufer, 0, w_bmi * h_bmi * sizeof(RGB));
#ifdef unix

     //---------reg_lib-------------------------------------
         tiff_lib = dlopen("./libp_tiff_dll.so",RTLD_LAZY);
         if(!tiff_lib)
         {
          char e[1000];
          strcpy(e, dlerror());
          fprintf(stderr,"%s",e);
          return -1;
         }


         g_tiff_Write = (TIFF_WRITE)dlsym(tiff_lib,"tiff_Write");
         if((ptr_e = dlerror()) != NULL)
         {
             char e[1000];
             strcpy(e, ptr_e);
             fprintf(stderr,"%s",e);
          return -2;
         }

         TIFF_INIT tiff_Init = (TIFF_INIT)dlsym(tiff_lib,"tiff_Init");
         if((ptr_e = dlerror()) != NULL)
         {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
          return -2;
         }
         tiff_Init();
#endif
    return 0;
}

//----------------------------------------------------------------------------
void     d_dll_tif_bufer_Close()
{


}
//------------------------------------------------------------------------------------
BYTE* s_dll_tif_bufer_Set(BYTE *buffer, long /*len*/, WORD iz_binning)
{
#ifdef unix
    TIFF_WRITE  tiff_Write = g_tiff_Write;
#endif

WORD ofs = 8;

w_bmi = (Iz_pxs_per_string / iz_binning);
WORD w_f;
WORD h_f;

#ifdef __BORLANDC__
_wmemset(rgb_bufer, 0, 1152 * 1152 * sizeof(RGB));
#else
wmemset((wchar_t*)rgb_bufer, 0, 1152 * 1152 * sizeof(RGB));
#endif

if(iz_binning == 1)
{

h_bmi = 605;
     w_f = w_bmi;
     h_f = h_bmi;
}

if(iz_binning == 2)
{

h_bmi = 1152/2 ;
        w_f =  w_bmi;
        h_f =  h_bmi;
}

if(iz_binning == 3)
{

h_bmi = 1152/3;

    w_bmi -= 9;
    w_f = w_bmi + 9;
    h_f = h_bmi -9;
}

if(iz_binning == 4)
{
h_bmi = 1152/4 ;
w_f =  w_bmi;
h_f =  h_bmi;
}

WORD x, y;
WORD w;

WORD *wtr = new WORD[h_bmi * w_bmi * sizeof(WORD)];

#ifdef __BORLANDC__
	_wmemcpy(wtr, buffer, h_bmi * w_bmi * sizeof(WORD));
#else
    #ifndef unix
        wmemcpy((wchar_t*)wtr, (const wchar_t*)buffer, h_bmi * w_bmi * sizeof(WORD));
    #else
        memcpy((wchar_t*)wtr, (const wchar_t*)buffer, h_bmi * w_bmi * sizeof(WORD));
    #endif
#endif

MINMAX minmax = s_dll_tif_bufer_FindMinMax(wtr + Iz_pxs_per_string * ofs * 8, h_bmi * w_bmi - Iz_pxs_per_string * ofs* 8);

double delta = (double)(minmax.w_max - minmax.w_min);
double kf = (delta + 1)/256.0;
if(kf  == 0)
        kf += 1.0;
for(y = 0; y < (h_bmi ); y++)
{
       for(x = 0; x < w_bmi; x++)
       {
                         w = wtr[w_bmi  * y  + x];

                         w = (WORD)(((double)(w - minmax.w_min))/kf);

                         rgb_bufer[y * w_bmi  + x ].r = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].g = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].b = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].l = 0;


       }

}

#ifdef unix
tiff_Write((const char*)"./s_dll_udp.tif", w_f, h_f, (char*)rgb_bufer);
#else
tiff_Write((const char*)"C:\\Users\\Public\\s_dll_udp.tif", w_f, h_f, (char*)rgb_bufer);
#endif

delete[] wtr;

return (BYTE*)rgb_bufer;

}

//-----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
MINMAX  s_dll_tif_bufer_FindMinMax(WORD *buf, DWORD len)
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
#ifdef __BORLANDC__
#pragma package(smart_init)
#endif
