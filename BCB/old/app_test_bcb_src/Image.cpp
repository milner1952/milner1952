//---------------------------------------------------------------------------


#pragma hdrstop

#include <vcl.h>
#include <vector>

#include "dll_src.h"
#include "app_test.h"


#include <new>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "test_dll.h"
#include "tiff_dll.h"


extern TForm1 *Form1;

//---------------------------------------------------------------------------
void __fastcall TForm1::Image_Init()
{

  TImage->Parent = Form1;
  TImage->Visible = true;

  r_bmi.left = TImage->Left;
  r_bmi.top = TImage->Top;


  w_bmi = TImage->Width;
  h_bmi = TImage->Height;
  TImage->Visible = true;

  rgb_bufer = new RGB[w_bmi * h_bmi * sizeof(RGB)];
    memset(rgb_bufer, 0, w_bmi * h_bmi * sizeof(RGB));
    memset(&bmi, 0, sizeof(bmi));
    RGB *rgb;
    DWORD i;



    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       =  w_bmi;
    bmi.bmiHeader.biHeight      =  h_bmi;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biClrUsed      = 0;
    bmi.bmiHeader.biClrImportant = 0;
    bmi.bmiHeader.biSizeImage  = 0;//w_bmi * h_bmi * 4;
    i_wtr = NULL;
    i_wtr = new WORD[w_bmi * h_bmi];

}
//--------------------------------------------------------------------
void __fastcall TForm1::Image_Reset()
{

    RGB *rgb;

    rgb = rgb_bufer;
    for(int i = 0; i < w_bmi * h_bmi; i++)
    {
      RGB t = {0xff,0xff,0xff,00};
              *rgb = t;
              rgb++;
    }
    SendMessage(Form1->Handle,WM_PAINT,0,0);
}

//-------------------------------------------------------
void __fastcall TForm1::Image_ResetBk()
{
    bmi.bmiHeader.biWidth       =  w_bmi = 1152;
    bmi.bmiHeader.biHeight      =  h_bmi = 608;

    RGB *rgb;
    rgb = rgb_bufer;

    RGB t = {0xff,0xff,0xff,00};

    for(int i = 0; i < 608 * 1152; i++)
    {
              *rgb = t;
              rgb++;
    }
    SendMessage(Form1->Handle,WM_PAINT,0,0);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Image_Get()
{

TBImage->Enabled = false;
if(r->pixsels_per_string == 2048)
        len = (2048  * 2048 * 2);
else
if(r->pixsels_per_string == 2304)
        len = (2304  * 2304  * 2);
   else
   if(r->pixsels_per_string == 1024)
        len = (1024 * 1024  * 2);
   else
        if(r->pixsels_per_string == 1152)
        {

          if(r->iz_binning != 0)
            len = (1152 * 1152  * 2) / (r->iz_binning * r->iz_binning);
          else
            len = (1152 * 1152  * 2);
        }
        else
        if(r->pixsels_per_string == 576)
        {

          if(r->iz_binning != 0)
            len = (576 * 576  * 2) / (r->iz_binning * r->iz_binning);
          else
            len = (576 * 576  * 2);
        }

if(i_wtr != NULL)
        memset(i_wtr, 0, sizeof(*i_wtr));

DWORD num = 0;

do
{
to_get:
if(btr != NULL)
  DeleteBufer();
  num  = GetBufer((BYTE**)&btr ,len, false);
}
while(num != len );

TBImage->Enabled = true;
}
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
BYTE*  __fastcall TForm1::Image_SetBufer_576(BYTE *buffer, long len)
{

WORD ofs = 8;
w_bmi = 576;
w_bmi = (w_bmi / (WORD)r->iz_binning);
WORD w_f;
WORD h_f;

memset(rgb_bufer, 0, 576 * 576 * sizeof(RGB));

if(r->iz_binning == 1)
{

h_bmi = 576;
    bmi.bmiHeader.biWidth       = w_f = w_bmi;
    bmi.bmiHeader.biHeight      = h_f = h_bmi;
}

if(r->iz_binning == 2)
{

h_bmi = 576/2 ;
    bmi.bmiHeader.biWidth       = w_f =  w_bmi;
    bmi.bmiHeader.biHeight      = h_f =  h_bmi;
}

if(r->iz_binning == 3)
{

h_bmi = 576/3;

    bmi.bmiHeader.biWidth       =  w_bmi;
    bmi.bmiHeader.biHeight      =  h_bmi - 9;
    w_bmi -= 9;

    w_f = w_bmi + 9;
    h_f = h_bmi -9;
}

if(i_wtr != NULL)
        delete[] i_wtr;

i_wtr = new WORD[w_bmi* h_bmi];

TImage->Visible = false;
WORD x,y,m, w;
WORD d;
WORD *wtr = (WORD*)(buffer);
MINMAX minmax;
if(r->pixsels_per_string == 1152)
        minmax = Image_FindMinMax(wtr + r->pixsels_per_string * ofs * 5, h_bmi * w_bmi - r->pixsels_per_string * ofs* 5);
else
if(r->pixsels_per_string == 576)
        minmax = Image_FindMinMax(wtr , h_bmi * w_bmi - r->pixsels_per_string  );



double delta = (double)(minmax.w_max - minmax.w_min);
double kf = (delta + 1)/256.0;
if(kf  == 0)
        kf += 1.0;
for(y = 0; y < (h_bmi ); y++)
{
       for(x = 0; x < w_bmi; x++)
       {
                         w = wtr[w_bmi  * (y + ScrollBar_Y_Img->Position) + x];

                         if(w == 0)
                         w = w;

                         d = w;
                         w = ((double)(w - minmax.w_min)/kf);
                         rgb_bufer[y * w_bmi  + x ].r = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].g = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].b = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].l = 0;
                         i_wtr[y * w_bmi + x] = d;
       }

}
PostMessage(Form1->Handle,WM_PAINT,0,0);
                         Form1->Invalidate();

tiff_Write("C:\\Users\\Public\\s_app_test.tif", w_f, h_f, (char*)rgb_bufer);

return (BYTE*)rgb_bufer;
}

//------------------------------------------------------------------------------------
BYTE*  __fastcall TForm1::Image_SetBufer_1152(BYTE *buffer, long len)
{

WORD ofs = 8;
w_bmi = 1152;
w_bmi = (w_bmi / (WORD)r->iz_binning);
WORD w_f;
WORD h_f;

memset(rgb_bufer, 0, 1152 * 605 * sizeof(RGB));

if(r->iz_binning == 1)
{

h_bmi = 605;
    bmi.bmiHeader.biWidth       = w_f = w_bmi;
    bmi.bmiHeader.biHeight      = h_f = h_bmi;
}

if(r->iz_binning == 2)
{

h_bmi = 1152/2 ;
    bmi.bmiHeader.biWidth       = w_f =  w_bmi;
    bmi.bmiHeader.biHeight      = h_f =  h_bmi;
}

if(r->iz_binning == 3)
{

h_bmi = 1152/3;

    bmi.bmiHeader.biWidth       =  w_bmi;
    bmi.bmiHeader.biHeight      =  h_bmi - 9;
    w_bmi -= 9;

    w_f = w_bmi + 9;
    h_f = h_bmi -9;
}

if(r->iz_binning == 4)
{

h_bmi = 1152/4 ;

    bmi.bmiHeader.biWidth      = w_f =  w_bmi;
    bmi.bmiHeader.biHeight     = h_f =  h_bmi;
}

if(i_wtr != NULL)
delete[] i_wtr;

i_wtr = new WORD[w_bmi* h_bmi];

TImage->Visible = false;
WORD x,y,m, w;
WORD d;
WORD *wtr = (WORD*)(buffer);
MINMAX minmax = Image_FindMinMax(wtr + r->pixsels_per_string * ofs * 5, h_bmi * w_bmi - r->pixsels_per_string * ofs* 5);
double delta = (double)(minmax.w_max - minmax.w_min);
double kf = (delta + 1)/256.0;
if(kf  == 0)
        kf += 1.0;
for(y = 0; y < (h_bmi ); y++)
{
       for(x = 0; x < w_bmi; x++)
       {
                         w = wtr[w_bmi  * (y + ScrollBar_Y_Img->Position) + x];
                         
                         if(w == 0)
                         w = w;

                         d = w;
                         w = ((double)(w - minmax.w_min)/kf);
                         rgb_bufer[y * w_bmi  + x ].r = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].g = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].b = LOBYTE(w);
                         rgb_bufer[y * w_bmi  + x ].l = 0;
                         i_wtr[y * w_bmi + x] = d;
       }

}
PostMessage(Form1->Handle,WM_PAINT,0,0);
                         Form1->Invalidate();

tiff_Write("C:\\Users\\Public\\s_app_test.tif", w_f, h_f, (char*)rgb_bufer);

return (BYTE*)rgb_bufer;
}

//------------------------------------------------------------------------------------
BYTE*  __fastcall TForm1::Image_SetBufer_1024(BYTE *buf, long len)
{

 TImage->Visible = false;

WORD x,y,m,x_rgb,w;
WORD d;
WORD *wtr = (WORD*)(buf +  r->pixsels_per_string * 30);
MINMAX minmax = Image_FindMinMax(wtr, h_bmi * w_bmi);
double delta = (double)(minmax.w_max - minmax.w_min);

double kf = (delta + 1)/256.0;
if(kf  == 0)
        kf += 1.0;
for(y = 0; y < h_bmi; y++)
{
x_rgb = 0;
        for(m = 0; m < 8; m++)
        {
                for(x = 0; x < 128; x++)
                {

                         w = wtr[w_bmi  * (y + ScrollBar_Y_Img->Position) + x * 8 + m];
                         d = w;
                         w = ((double)(w - minmax.w_min)/kf);
                         rgb_bufer[y * w_bmi + x_rgb].r = LOBYTE(w);
                         rgb_bufer[y * w_bmi + x_rgb].g = LOBYTE(w);
                         rgb_bufer[y * w_bmi + x_rgb].b = LOBYTE(w);
                         rgb_bufer[y * w_bmi + x_rgb].l = 0;
                         i_wtr[y * w_bmi + x_rgb] = d;
                        x_rgb++;
                }
        }
}
PostMessage(Form1->Handle,WM_PAINT,0,0);
return (BYTE*)rgb_bufer;
}


//------------------------------------------------------------------------------------
BYTE*  __fastcall TForm1::Image_SetBufer_2048(BYTE *buf, long len)
{
TImage->Visible = false;
WORD x,y,m,x_rgb,w;
WORD *wtr = (WORD*)(buf +  r->pixsels_per_string * 30);
WORD x_end = 256;
WORD d;
WORD w_t[2048];
MINMAX minmax = Image_FindMinMax(wtr, h_bmi * w_bmi);
double delta = (double)(minmax.w_max - minmax.w_min);
DWORD s;
double kf = (delta + 1)/256.0;
if(kf  == 0)
        kf += 1.0;
BYTE t[2048];

for(y = 0; y < h_bmi; y++)
{
x_rgb = 0;
                if(UpDZoom_X->Position == 2)
                {
                     for(m = 0; m < 8; m++)
                     {
                        for(x = 0; x < x_end; x++)
                        {
                                w = GetWord2048FromZoom2(wtr + w_bmi * 2 * (y + ScrollBar_Y_Img->Position), m, x++);
                                d = w;
                                w = ((double)(w - minmax.w_min)/kf);
                        BYTE  b = LOBYTE(w);
                            s = y * w_bmi + x_rgb;
                                rgb_bufer[s].r = b;
                                rgb_bufer[s].g = b;
                                rgb_bufer[s].b = b;
                                rgb_bufer[s].l = 0;
                                i_wtr[s] = d;
                                x_rgb++;
                        }
                     }
                }
                else
                {
                     memset(t, 0, 2048);
                     for(m = 0; m < 8; m++)
                     {
                                for(x = 0; x < x_end; x++)
                                {
                                w = wtr[w_bmi * 2 * (y + ScrollBar_Y_Img->Position) + x * 8 + m];
                                d = w;
                                w = ((double)(w - minmax.w_min)/kf);
                                t[x_rgb] = LOBYTE(w);
                                w_t[x_rgb] = d;
                                x_rgb++;
                                }
                     }
                     x_rgb = 0;
                     for(int i = ScrollBar_X->Position; i < w_bmi + ScrollBar_X->Position; i++)
                     {
                            s = y * w_bmi + x_rgb;
                                rgb_bufer[s].r = t[i];
                                rgb_bufer[s].g = t[i];
                                rgb_bufer[s].b = t[i];
                                rgb_bufer[s].l = 0;
                                i_wtr[s] = w_t[i];
                                x_rgb++;
                     }
               }
}
PostMessage(Form1->Handle,WM_PAINT,0,0);
return (BYTE*)rgb_bufer;
}

//------------------------------------------------------------------------------------
BYTE*  __fastcall TForm1::Image_SetBufer_2304(BYTE *buf, long len)
{
 TImage->Visible = false;
WORD x,y,m,x_rgb,w;
WORD *wtr = (WORD*)(buf +  r->pixsels_per_string * 30);
WORD x_end = 256;
WORD d;
MINMAX minmax = Image_FindMinMax(wtr, h_bmi * w_bmi);
double delta = (double)(minmax.w_max - minmax.w_min);

double kf = (delta + 1)/256.0;
if(kf  == 0)
        kf += 1.0;
BYTE t[2304];
WORD w_t[2304];

for(y = 0; y < h_bmi; y++)
{
x_rgb = 0;
                if(UpDZoom_X->Position == 2)
                {
                     for(m = 0; m < 9; m++)
                     {
                        for(x = 0; x < x_end; x++)
                        {
                               w = GetWord2304FromZoom2(wtr + w_bmi * 2 * (y + ScrollBar_Y_Img->Position), m, x++);
                               d = w;
                               w = ((double)(w - minmax.w_min)/kf);
                        BYTE  b = LOBYTE(w);
                        DWORD s = y * w_bmi + x_rgb;
                                rgb_bufer[s].r = b;
                                rgb_bufer[s].g = b;
                                rgb_bufer[s].b = b;
                                rgb_bufer[s].l = 0;
                                i_wtr[s] = d;
                                x_rgb++;
                        }
                     }
                }
                else
                {
                     memset(t, 0, 2304);
                     for(m = 0; m < 9; m++)
                     {
                                for(x = 0; x < x_end; x++)
                                {
                                w = wtr[w_bmi * 2 * (y + ScrollBar_Y_Img->Position) + x * 9 + m];
                                d = w;
                                w = ((double)(w - minmax.w_min)/kf);
                                t[x_rgb] = LOBYTE(w);
                                w_t[x_rgb] = d;
                                x_rgb++;
                                }
                     }
                     x_rgb = 0;
                     for(int i = ScrollBar_X->Position; i < w_bmi + ScrollBar_X->Position; i++)
                     {
                     DWORD s = y * w_bmi + x_rgb;
                                rgb_bufer[s].r = t[i];
                                rgb_bufer[s].g = t[i];
                                rgb_bufer[s].b = t[i];
                                rgb_bufer[s].l = 0;
                                i_wtr[s] = w_t[i];
                                x_rgb++;
                     }
                }
}
PostMessage(Form1->Handle,WM_PAINT,0,0);
return (BYTE*)rgb_bufer;
}

//-------------------------------------------------------------------------------------
 WORD  __fastcall TForm1::GetWord2048FromZoom2(WORD *wtr, WORD m, WORD x)
 {
    WORD w;
        if(x < 255)
                w = max(wtr[x * 8 + m], wtr[(x + 1)* 8 + m]);
        else
                w =  wtr[x * 8 + m];
  return w;
 }
 //-------------------------------------------------------------------------------------
 WORD  __fastcall TForm1::GetWord2304FromZoom2(WORD *wtr, WORD m, WORD x)
 {
    WORD w;
        if(x < 255)
                w = max(wtr[x * 9 + m], wtr[(x + 1)* 9 + m]);
        else
                w =  wtr[x * 9 + m];
  return w;
 }
//-----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
void __fastcall TForm1::Image_Paint(TObject *Sender)
{

            if(rgb_bufer == NULL)
            return;

            if(TImage->Visible == true)
            return;

                                bg_w_bmi = 1152/2 - w_bmi/2;
                                bg_h_bmi = 608/2  - h_bmi/2;

            HANDLE h   = Form1->Handle;
            HDC hdc = GetDC(h);
                            SetDIBitsToDevice(hdc,
                             bg_w_bmi, bg_h_bmi, //1152/2-w_bmi/2,608/2 - h_bmi/2,
                             w_bmi , h_bmi,
                             0, 0,
                             0, h_bmi ,
                             rgb_bufer,
                             &bmi,
                             DIB_RGB_COLORS);

                ReleaseDC(h,hdc);


                                /*
            HANDLE h   = Form1->Handle;
            HDC hdc = GetDC(h);
                            SetDIBitsToDevice(hdc,
                             r_bmi.left, r_bmi.top,
                             w_bmi, h_bmi,
                             0, 0,
                             0, h_bmi,
                             rgb_bufer,
                             &bmi,
                             DIB_RGB_COLORS);

                ReleaseDC(h,hdc);
                */

                    /*
        PAINTSTRUCT ps;
                 HANDLE h   = Form1->Handle;
                 HDC hdc = BeginPaint(h,&ps);


                            SetDIBitsToDevice(hdc,
                             r_bmi.left, r_bmi.top,
                             w_bmi, h_bmi,
                             0, 0,
                             0, h_bmi ,
                             rgb_bufer,
                             &bmi,
                             DIB_RGB_COLORS);

                EndPaint(hdc,&ps);
                */
                      
}
//-----------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------
MINMAX  __fastcall TForm1::Image_FindMinMax(WORD *buf, DWORD len)
{
  MINMAX nx;
         nx.w_min = 0xffff;
         nx.w_max = 0;

  for(DWORD i = 0; i < len; i++)
  {
   WORD w = buf[i];
         if(w == 0)
           continue;
             // w = w;
         nx.w_min = min(w, nx.w_min);
         nx.w_max = max(w, nx.w_max);
         if( nx.w_max > 57000)
          nx.w_max =  nx.w_max;

  }

return nx;
}



#pragma package(smart_init)

