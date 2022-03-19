//---------------------------------------------------------------------------


#pragma hdrstop
#include "tiff_dll.h"
#include "s_dll_tif_buffer.h"
//static RGB *rgb_bufer = NULL;
RGB  rgb_bufer [1152 * 1152 * sizeof(RGB)];
WORD w_bmi;
WORD h_bmi;
extern REGDATA *regdata;
//---------------------------------------------------------------------------
int     d_dll_tif_bufer_Init(WORD h, WORD w)
{

    w_bmi = w;
    h_bmi = h;
  //   rgb_bufer = new RGB[w_bmi * h_bmi * sizeof(RGB)];
  //   if(rgb_bufer == NULL)
  //   return 1;
     memset(rgb_bufer, 0, w_bmi * h_bmi * sizeof(RGB));

    return 0;
}

//----------------------------------------------------------------------------
void     d_dll_tif_bufer_Close()
{
       /*
    if(rgb_bufer  != NULL)
    delete[] rgb_bufer;
    */

}
//------------------------------------------------------------------------------------
BYTE* s_dll_tif_bufer_Set(BYTE *buffer, long len, WORD iz_binning)
{

WORD ofs = 8;

w_bmi = (1152 / iz_binning);
WORD w_f;
WORD h_f;

_wmemset(rgb_bufer, 0, 1152 * 1152 * sizeof(RGB));

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




WORD x,y,m, w;
WORD d;


//WORD *wtr = new WORD[len];
//_wmemcpy(wtr, buffer, len);

WORD *wtr = new WORD[h_bmi * w_bmi * sizeof(WORD)];
_wmemcpy(wtr, buffer, h_bmi * w_bmi * sizeof(WORD));

MINMAX minmax = s_dll_tif_bufer_FindMinMax(wtr + Iz_pxs_per_string * ofs * 5, h_bmi * w_bmi - Iz_pxs_per_string * ofs* 5);

double delta = (double)(minmax.w_max - minmax.w_min);
double kf = (delta + 1)/256.0;
if(kf  == 0)
        kf += 1.0;
for(y = 0; y < (h_bmi ); y++)
{
       for(x = 0; x < w_bmi; x++)
       {
                         w = wtr[w_bmi  * y  + x];

                         w = ((double)(w - minmax.w_min)/kf);

                         rgb_bufer[y * w_bmi  + x ].r = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].g = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].b = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].l = 0;


       }

}


tiff_Write("C:\\Users\\Public\\s_dll_udp.tif", w_f, h_f, (char*)rgb_bufer);

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
#pragma package(smart_init)
