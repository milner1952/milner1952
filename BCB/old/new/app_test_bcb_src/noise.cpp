//---------------------------------------------------------------------------


#pragma hdrstop

#include "app_test.h"
#include "noise.h"
#include "dll_src.h"
#include "test_dll.h"
#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "reg_app2.h"

#include <ctime>
extern TForm1 *Form1;
extern REGDATA_APP2 *reg2;
//---------------------------------------------------------------------------
#pragma package(smart_init)
void __fastcall TForm1::noise_Visible(WORD *wtr)
{

TImage->Canvas->Brush->Color = (TColor)RGB(255,255,255);
TImage->Canvas->Pen->Color  = clBlack;
TImage->Canvas->FillRect(Rect(0,0, TImage->Width ,TImage->Height ));

if(r->pixsels_per_string == 576)
        noise_Print576(wtr);
else
        noise_Print(wtr);

noise_PrintRezine(1);
}


//-------------------------------------------------------------------------

void __fastcall TForm1::noise_Print576(WORD *wtr)
{
if(wtr == NULL)
return;
if(r->pixsels_per_string != 576)
return;
WORD num_matrix = 9;
WORD n_pix_in_matrix = 64;
WORD i, x, w, m, ax;

      for(m = 0; m < num_matrix; m++)
      {
          for( i = 0, x = 0; x < n_pix_in_matrix *  2; i += num_matrix)
          {
                w = (wtr[i + m] / UpDZoom_Y->Position);
                if(w > ScrollBar_Y->Position)

                        w -= (WORD)ScrollBar_Y->Position;
                else
                        w = 0;

                ax =  (WORD)(x + n_pix_in_matrix * 2 * m);
                if(ax == 0)
                TImage->Canvas->MoveTo(ax  ,  w * kf_scr);
                else
                TImage->Canvas->LineTo(ax  ,  w * kf_scr);
                
                TImage->Canvas->LineTo(ax +  1,  w * kf_scr);
                x += 2;
           }
      }



}





//-------------------------------------------------------------------------

void __fastcall TForm1::noise_Print(WORD *wtr)
{
if(wtr == NULL)
return;
bool init = 1;
WORD x,i,m,w, z, X, Xx;
static WORD old[9]={0};

WORD num_matrix =    r->pixsels_per_string == 2304 ? 9 :
                     r->pixsels_per_string == 1152 ? 9 :
                     r->pixsels_per_string == 576 ? 9 : 8;

w = 0xfffe;
X = 0;
z = (WORD)(UpDZoom_X->Position == 1 ? 256 : 128);

     if((r->pixsels_per_string == 1024) || (r->pixsels_per_string == 1152))
     {
        Xx = 128;     UpDZoom_X->Position = 1; z = 128;
     }
     else
     if(r->pixsels_per_string == 576)
     {
        Xx = 64;     UpDZoom_X->Position = 1; z = 64;
     }
     else
     {
        Xx = 256;
     }


      for(m = 0; m < num_matrix; m++)
      { //1
      WORD ax;
      old[m] = w;
       for( i = 0, x = 0; x < Xx ; i += (WORD)num_matrix ,x++)
       { //2
       X = x;
         w = (WORD)(wtr[i + m] / UpDZoom_Y->Position);
         if(UpDZoom_X->Position == 2)
           {//3
             int iw;
             if((iw = noise_ZoomX(w, (WORD)(x + 256 * m) , m)) == -1)
             {
                continue;
             }
             else    {
                w = (WORD)iw;   X /= (WORD)2;
                }
           } //3

         if(w > ScrollBar_Y->Position)
         {//4
                w -= (WORD)ScrollBar_Y->Position;
         }//4
         else
         {//5
         w = 0;
         if(old[m] <= 0)
               continue;
         }//5

        if(init == 0)
        {//6
            if((X + z * m - 1) > ScrollBar_X->Position)
                ax =  (WORD)(X + z * m - ScrollBar_X->Position  - 1);
            else
            {
            old[m] = w;
            continue;
            }
         TImage->Canvas->MoveTo(ax , old[m] * kf_scr);
         TImage->Canvas->LineTo(++ax, (old[m] = w) * kf_scr);
        }//6
        else
         init = 0;
       }  //2

      }
}




//-------------------------------------------------------------------------
int __fastcall TForm1:: noise_ZoomX(WORD w, WORD x, BYTE m)
{
static w_even[9]={0};
if((x & 0x001)== 0)
{
w_even[m] = w;  return -1;
}
else
 return w > w_even[m] ? w : w_even[m];
}

//----------------------------------------------------------------------
void __fastcall TForm1::noise_PrintRezine(BYTE iz_set, TYPE_TR_BAR ttb)
{
WORD y;

WORD num_pix = GetPixelPerString();
if(num_pix == 576)
   num_pix *= 2;
TPenMode mode = TImage->Canvas->Pen->Mode;
TImage->Canvas->Pen->Mode = pmNot;

if(b_rezine == 1)
{
        if((ttb == OLL_TR_BAR) || (ttb == H_TR_BAR))
        {
                y =    (WORD)(old_position_H - old_baza);
                TImage->Canvas->MoveTo(0,y * kf_scr);
                TImage->Canvas->LineTo(num_pix,y * kf_scr);
        }
        if((ttb == OLL_TR_BAR) || (ttb == L_TR_BAR))
        {
                y =    (WORD)(old_position_L - old_baza);
                TImage->Canvas->MoveTo(0,y * kf_scr);
                TImage->Canvas->LineTo(num_pix,y * kf_scr);
        }
}
else
{
        if((ttb == OLL_TR_BAR) || (ttb == H_TR_BAR))
        {
                y = (WORD)(TRBarPozH->Position / UpDZoom_Y->Position - ScrollBar_Y->Position);
                old_position_H = (WORD)(TRBarPozH->Position / UpDZoom_Y->Position);
                TImage->Canvas->MoveTo(0,y * kf_scr);
                TImage->Canvas->LineTo(num_pix,y * kf_scr);


        }

        if((ttb == OLL_TR_BAR) || (ttb == L_TR_BAR))
        {
                y = (WORD)(TRBarPozL->Position / UpDZoom_Y->Position - ScrollBar_Y->Position);
                old_position_L = (WORD)(TRBarPozL->Position / UpDZoom_Y->Position);
                TImage->Canvas->MoveTo(0,y * kf_scr);
                TImage->Canvas->LineTo(num_pix,y * kf_scr);
        }
}

if(iz_set == 2)
        b_rezine = (bool)(b_rezine ^ 1);
else
        b_rezine = (bool)iz_set;
       


old_baza = (WORD)ScrollBar_Y->Position;
TImage->Canvas->Pen->Mode  = mode;
}

//----------------------------------------------------------------------------
DWORD __fastcall TForm1:: noise_Get(BYTE **btr, DWORD dw_len, bool iz_normalization)
{

 DWORD dw_num;
 TImage->Visible = true;

 do
{
 to_get:
 DeleteBufer();
 try
 {
 dw_num  = (DWORD)GetBufer((BYTE**)btr ,dw_len, true);
 }
 catch(EIdSocketError &e)
 {
   test_dll_OutputDebugString("Noise: EIdSocketError", 1);
   goto to_get;
 }
 catch(...)
 {
   test_dll_OutputDebugString("Noise: Íå èçâåñòíàÿ îøèáêà", 1);
   if(*btr != NULL)
   DeleteBufer();
   return 0;
 }
}
while(dw_num != dw_len);

 return dw_num;
}

//------------------------------------------------------------------
void __fastcall TForm1:: noise_AsText(WORD *wtr , DWORD len, bool iz_decimal)
{
int x, y, m, n, in,sn;
DWORD i;
WORD w;
char text[20];

y = 1;
in = 0;
sn = 0;
x = 0;

WORD num_matrix =    r->pixsels_per_string == 2304 ? 9 :
                     r->pixsels_per_string == 1152 ? 9 :
                     r->pixsels_per_string == 576 ? 9 : 8;


           StrGridAsText->Cells[0][y] = "Str:";
           StrGridAsText->Cells[1][y] = "1";
  for(i = 0,  n = 0; i < len  ;i+= num_matrix)
  {
           for(m = 0; m < num_matrix; m++)
           {
            AnsiString a_text;
            memset(text,0,20);
            w = wtr[i + m];

         if(iz_decimal == 1)
            sprintf(text,"%0.4x",w);
         else
            sprintf(text,"%0.4d",w);

            a_text = text;
            x = n + 5 * m + 1; in++;
            StrGridAsText->Cells[x][y + 1] = a_text;
           }
           n++;
           if(n >= 4)
           {
            n = 0;
            memset(text,0,20);
            sprintf(text,"%0.4d",y );
            AnsiString as(text);
            StrGridAsText->Cells[0][y + 1] = as;
            y++;

           if(in >= r->pixsels_per_string)
           {
           in = 0;
           sn++;
           StrGridAsText->Cells[0][y + 1] = "Str:";
           StrGridAsText->Cells[1][y + 1] = sn + 1;
           y++;
           }
           }
  }

}

//6087   2B04 890d 2b04 0100 0100  0100 bb1e 0000 0100 0100 0100
//34565  1067 3465 1067                 7867

//-----------------------------------------------------------------------------
void __fastcall TForm1::noise_SaveInFile(WORD *wtr)
{
const time_t t = time(NULL);

REGDATA *r;
r = GetParamRegData();

char st[100];
memset(st,0,sizeof(st));

strcpy(st,ctime(&t));
SaveDialog1->Filter = "File APM(*.apm)|*.APM";

SaveDialog1->InitialDir = GetCurrentDir();
String s(st);

int      pos = s.LastDelimiter("\n");
if(pos > 0)
        s.Delete(pos ,1);
s += ".apm";

do      {
        pos = s.LastDelimiter(":");
        if(pos > 0)
                   s[pos] = '_';
        }
while(pos > 0);


SaveDialog1->FileName = s;


if( SaveDialog1->Execute())
        {
         FName = SaveDialog1->FileName;
         file_SaveInFile(FName.c_str(),(BYTE*)(reg2 ), sizeof(REGDATA_APP2), wtr, r->pixsels_per_string * 20 );
        }
        else
        test_SetStringTooOutDebug3("In  noise_SaveInFile", "Can not save file");
}

//------------------------------------------------------------------------------
void __fastcall TForm1::noise_LoadFromFile(WORD* wtr)
{
REGDATA *r;
r = GetParamRegData();
dw_len = r->pixsels_per_string *2 * 20;
OpenDialog1->Filter = "File APM(*.apm)|*.APM";
if( OpenDialog1->Execute() )
 {
     FName =  OpenDialog1->FileName;
     DWORD num = file_LoadFromFile(FName.c_str(), &btr);
     if(num !=  0L)
     {
        REGDATA_APP2 *apr;
        apr = (REGDATA_APP2*)btr;
        reg2->copy((const REGDATA_APP2*)apr);
        FormCopyFromReg2();
        reg2->WriteAllParamTooReg();

        if(btr != NULL)
        {
        btr += sizeof(REGDATA_APP2);
        noise_AsText((WORD*)(btr), dw_len/2, ChHex_Decm->State);
        noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * r->pixsels_per_string *2));

        AnsiString as = "ÏÐÎÃÐÀÌÌÀ ÒÅÑÒÈÐÎÂÀÍÈß  ÒËÄ-6:       ";
        as += FName;
        Form1->Caption = as;
        }
        flag_noise_image = 0;
     }

  }
}
