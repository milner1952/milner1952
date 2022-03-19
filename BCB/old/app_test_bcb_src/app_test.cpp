//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <windowsx.h>
#include "app_test.h"
#include "dll_src.h"
#include "reg_app2.h"


#include <stdio.h>
#include <math.h>


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
REGDATA_APP2 *reg2;


#define MAX_Y_14 16384
#define MAX_Y_16 65536
//app_test - c запоминанием в файле   и 16 разрядов АЦП
//---------------------------------------------------------------------------
//   Конструктор
//-------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
 TImage->Width = 1024;
 TImage->Height = 768;
 TImage->Top = 2;
 bg_w_bmi = 0;
 bg_h_bmi = 0;
  kf_scr = 1.0;
 Init(Owner, NULL,NULL,NULL);
 r = GetParamRegData();
 Timer->Interval = (r->timer_period * 1000);
 i_wtr = NULL;
}
//--------------------------------------------------------------------------
//      FormCreate
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{

 bool izhex;

 cy =        GetSystemMetrics(SM_CYCURSOR);
 btr = NULL;
 b_rezine = false;

 reg2 = new REGDATA_APP2();

  if(r->pixsels_per_string == 2304)
  {
        SetFlagIzMainAPP(APP_2304_MAIN);
  }
 else
 if(r->pixsels_per_string == 2048)
        SetFlagIzMainAPP(APP_2048_MAIN);
        if(r->pixsels_per_string == 1024)
                SetFlagIzMainAPP(APP_1024_MAIN);
                if(r->pixsels_per_string == 1152)
                {
                        SetFlagIzMainAPP(APP_1152_MAIN);
                        TImage->Width = 1152;
                }
                else
                        if(r->pixsels_per_string == 576)
                        {
                                SetFlagIzMainAPP(APP_576_MAIN);
                                TImage->Width = 1152;
                        }


AfteInitOnCreate();

  if(r->pixsels_per_string == 2304)
  {
        SetFlagIzMainAPP(APP_2304_MAIN);
         TImage->Width += 128;
  }
 else
 if(r->pixsels_per_string == 2048)
        SetFlagIzMainAPP(APP_2048_MAIN);
        if(r->pixsels_per_string == 1024)
                SetFlagIzMainAPP(APP_1024_MAIN);
                if(r->pixsels_per_string == 1152)
                {
                        SetFlagIzMainAPP(APP_1152_MAIN);
                        TImage->Width = 1152;
                }
                else
                        if(r->pixsels_per_string == 576)
                        {
                                SetFlagIzMainAPP(APP_576_MAIN);
                                TImage->Width = 1152;
                        }

StrGridAsText->Font->Size = 6;
Form1->Width = TImage->Left + TImage->Width + ScrollBar_Y->Width  + 50;//+ 300;//9;




 StrGridAsText->Left = 2;//TImage->Left;
 StrGridAsText->Top = TImage->Top;
 if(r->pixsels_per_string == 576 )
 {
 StrGridAsText->Width =  TImage->Left + 1152 + ScrollBar_Y->Width  + 50;
 Form1->Width =  TImage->Left + 1152 + ScrollBar_Y->Width  + 50;
 }
 else
 StrGridAsText->Width = Form1->Width;

 StrGridAsText->Visible = false;
 if((r->pixsels_per_string == 2048) || (r->pixsels_per_string == 1024))
 {
        for(int i = 0; i < 40; i += 5 )
                StrGridAsText->ColWidths[i] = 7;
        StrGridAsText->ColWidths[0] = 40;
 }
 else
        if((r->pixsels_per_string == 2304) || (r->pixsels_per_string == 1152) || (r->pixsels_per_string == 576))
        {
                StrGridAsText->ColCount = 45;
                for(int i = 0; i < 45; i++ )
                        StrGridAsText->ColWidths[i] = (StrGridAsText->Font->Size ) *5 + 1;

                for(int i = 0; i < 45; i += 5 )
                        StrGridAsText->ColWidths[i] = 1;//7
                StrGridAsText->ColWidths[0] = 30;
        }

 flag_noise_image = 0;

 dw_len = 0L;

 TImage->Canvas->Brush->Color = (TColor)RGB(255,255,255);
 TImage->Canvas->FillRect(Rect(0,0, TImage->Width ,TImage->Height ));

{
        TRBarPozH->Max = MAX_Y_16;
        TRBarPozL->Max = MAX_Y_16;
}




ScrollBar_X->Left = TImage->Left;
ScrollBar_X->Width = TImage->Width;



ScrollBar_Y->Left = TImage->Left +  TImage->Width + 1;
ScrollBar_Y->Height =  TImage->Height;


FormCopyFromReg2();
ChBMode->Checked = (((TCheckBoxState)r->with_matrices) ^ 1);
ChBModeClick(Sender);
UpMtrG->Position = GetMtrGane();
InitDac(Sender, r->iz_dac);


char* Version = GetVersionDll();

AnsiString as = "ТЛД-";
as += Version;
Form1->Caption =as;

kf_scr = FormMove();
noise_Visible((WORD*)(btr ));
Image_Init();


if(reg2->timer_zdr > 0)
        comm_Init();

Form1->AutoSize = true;    //769
        Timer->Enabled = 0;
ChRts->Checked = RTS_PW;
EscapeCommFunction(hComm,  POWER_EN);



s_dll_src_SendBinning(r->iz_binning);



switch(r->iz_binning)
{
        case 1: Binn1->Checked = true; break;
        case 2: Binn2->Checked = true; break;
        case 3: Binn3->Checked = true; break;
        case 4: Binn4->Checked = true; break;
        default: MessageBox(NULL, "Биннинг может быть 1,2,3,4","Error" ,MB_OK); break;
}
TFilter->Checked = r->iz_h_filter == 1 ? true : false;
TDiv_16_9_4->Checked = r->div_16_9_4;
TDiv_4_3_2->Checked = r->div_4_3_2;
TRange->Checked = r->iz_range;
//MessageBox(NULL,"Error","Create1", MB_OK);
}

//-----------------------------------------------------------------
void __fastcall TForm1::FormCopyFromReg2()
{
UpDZoom_X->Position = reg2->zoom_x;       ////////////////////3
UpDZoom_Y->Position = reg2->zoom_y;       ////////////////////4

UpDZoom_Y->Max = 21 *  4;
UpDZoom_Y->Increment =  4;

 TRBarPozL->SmallChange =     UpDZoom_Y->Position;
 TRBarPozH->SmallChange =     UpDZoom_Y->Position;
 ScrollBar_Y->SmallChange =   (Forms::TScrollBarInc)(UpDZoom_Y->Position * 4);
if(r->pixsels_per_string == 2304)
 ScrollBar_X->SmallChange =   (Forms::TScrollBarInc)(UpDZoom_X->Position * 9);
else
        if(r->pixsels_per_string == 2048)
                 ScrollBar_X->SmallChange =   (Forms::TScrollBarInc)(UpDZoom_X->Position * 8);       //8

TRBarPozH->Position =  reg2->pozH;                ////////////1
TRBarPozL->Position =  reg2->pozL;                ////////////2

old_position_H = (WORD)(reg2->pozH / UpDZoom_Y->Position);
old_position_L = (WORD)(reg2->pozL / UpDZoom_Y->Position);

{
        ScrollBar_Y->Max  = MAX_Y_16 / UpDZoom_Y->Position - MAX_Y_16 / (21 * 4)  +  11;   //??????????????
}
ScrollBar_Y->Max = 65536;//Form1->Height;

if(r->pixsels_per_string == 2304)
{
ScrollBar_X->Max = 2304 / UpDZoom_X->Position - 2304 / 2;
}
else
if(r->pixsels_per_string == 2048)
ScrollBar_X->Max = 2048 / UpDZoom_X->Position - 2048 / 2;
else
if(r->pixsels_per_string == 1024)
       ScrollBar_X->Visible = false;

ScrollBar_X->Position = reg2->imScrBX; ///////////////5


ScrollBar_Y->Position = old_baza = reg2->imScrBY;      //////////////////6
//ScrollBar_Y_Img->Position = reg2->scrbaza_y_im;
LabPozScY->Caption = ScrollBar_Y->Position * UpDZoom_Y->Position;
                      
TUpDStringNomber->Position = reg2->zd_poz;    ////////////////////7
ChHex_Decm->State = (TCheckBoxState)reg2->izhex;   ///////////////10
ChReduct->State =  (TCheckBoxState)Reduct_get();

LbPOZ_H->Caption =  TRBarPozH->Position;
LbPOZ_L->Caption =  TRBarPozL->Position;
bool t =     TimePerString();
ChTimeString->State = (TCheckBoxState)t;
//ChTimeStringClick((TObject* )ChTimeString);


StrGridAsText->TopRow = reg2->sgtr;  /////////////////9


UpDZoom_YClick(this,(TUDBtnType)0);

flag_init = 1;
}



//---------------------------------------------------------------------------
//Прокрутка по горизонтале - окончание
//---------------------------------------------------------------------------
void __fastcall TForm1::ScrollBar_XScroll(TObject */*Sender*/,
      TScrollCode ScrollCode, int &/*ScrollPos*/)
{
if(ScrollCode == scEndScroll)
        {
        reg2->WriteParamTooReg(4, (WORD)ScrollBar_X->Position);
        reg2->imScrBX = ScrollBar_X->Position;
        }
}

//-------------------------------------------------------------------------
//Прокрутка по горизонтале
//---------------------------------------------------------------------------
void __fastcall TForm1::ScrollBar_XChange(TObject *Sender)
{
 if(btr != NULL)
 {
  if(flag_noise_image == 0)
        {
          if(r->pixsels_per_string == 2304)
                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 2304 * 2));
          else

                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4096));
        }
  else
        {
         TBVisionClick(Sender);
        }
  }
}

//Прокрутка снимка   по вертикале
//---------------------------------------------------------------------------

void __fastcall TForm1::ScrollBar_Y_ImgScroll(TObject *Sender,
      TScrollCode ScrollCode, int &ScrollPos)
{
          TBVisionClick(Sender);
          reg2->scrbaza_y_im = ScrollBar_Y_Img->Position;
          reg2->WriteParamTooReg(10, (WORD)reg2->scrbaza_y_im);
}



//--------------------------------------------------------------------------
//ZOOM_X
//---------------------------------------------------------------------------
void __fastcall TForm1::UpDZoom_XClick(TObject *Sender, TUDBtnType /*Button*/)
{
if(r->pixsels_per_string == 2048)
         ScrollBar_X->Max = 2048 / UpDZoom_X->Position - 2048 / 2;
else
         ScrollBar_X->Max = 2304 / UpDZoom_X->Position - 2304 / 2;

reg2->zoom_x = UpDZoom_X->Position;
reg2->WriteParamTooReg(2, UpDZoom_X->Position);
ScrollBar_X->SmallChange =   (Forms::TScrollBarInc)(UpDZoom_X->Position * 8);
noise_PrintRezine(0,H_TR_BAR);

if(flag_noise_image == 0)
{
        if(btr != NULL)
                        if(r->pixsels_per_string == 2048)
                                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4096));
                        else
                                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4608));
}
else
TBVisionClick(Sender);
}

//-------------------------------------------------------------------------
//Прокрутка по вертикале   - окончание
//---------------------------------------------------------------------------
void __fastcall TForm1::ScrollBar_YScroll(TObject */*Sender*/,
      TScrollCode ScrollCode, int &ScrollPos)
{
LabPozScY->Caption = ScrollPos * UpDZoom_Y->Position ;
reg2->imScrBY = ScrollBar_Y->Position;
if(ScrollCode == scEndScroll)
        reg2->WriteParamTooReg(5, (WORD)ScrollPos);
}

//--------------------------------------------------------------------------
//Прокрутка по вертикале
//----------------------------------------------------------------------------
void __fastcall TForm1::ScrollBar_YChange(TObject *Sender)
{


        if(flag_noise_image == 0)
        {
                LabPozScY->Caption = ScrollBar_Y->Position * UpDZoom_Y->Position ;
                reg2->imScrBY = ScrollBar_Y->Position;
                reg2->zoom_y = UpDZoom_Y->Position;
                ScrollBar_Y->SmallChange =   (Forms::TScrollBarInc)(UpDZoom_Y->Position * 4);
                noise_PrintRezine(0);
                if(btr != NULL)
                {
                        if(r->pixsels_per_string == 2048)
                                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4096));
                        else
                           if(r->pixsels_per_string == 2304)
                                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4608));
                                else
                                if(r->pixsels_per_string == 1024)
                                       noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 1024));
                }
        }
        else
        {
                TBVisionClick(Sender);
        }

}
// ZOOM_Y
//---------------------------------------------------------------------------
void __fastcall TForm1::UpDZoom_YClick(TObject */*Sender*/, TUDBtnType /*Button*/)
{
{
         ScrollBar_Y->Max = MAX_Y_16 / UpDZoom_Y->Position - MAX_Y_16/(21 * 4);
}


 reg2->zoom_y = UpDZoom_Y->Position;
 reg2->imScrBY = ScrollBar_Y->Position;

 TRBarPozL->SmallChange =     UpDZoom_Y->Position;
 TRBarPozH->SmallChange =     UpDZoom_Y->Position;
 ScrollBar_Y->SmallChange =   (Forms::TScrollBarInc)(UpDZoom_Y->Position * 4);

 reg2->WriteParamTooReg(5, (WORD)ScrollBar_Y->Position);
 reg2->WriteParamTooReg(3, (WORD)UpDZoom_Y->Position);
 noise_PrintRezine(0,OLL_TR_BAR);
 noise_PrintRezine(1,OLL_TR_BAR);

 if(btr != NULL)
 {
       if(r->pixsels_per_string == 2048)
                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4096));
       else
                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4608));
 }
 LbPOZ_H->Caption =  TRBarPozH->Position;
 reg2->pozH = TRBarPozH->Position;
 LbPOZ_L->Caption =  TRBarPozL->Position ;
 reg2->pozL = TRBarPozL->Position;
}

//TRBar_H
//---------------------------------------------------------------------------
void __fastcall TForm1::TRBarPozHScroll(TObject */*Sender*/,
      TScrollCode ScrollCode, int &/*ScrollPos*/)
{
        if(ScrollCode == scEndScroll)
        {
        reg2->WriteParamTooReg(0, (WORD)TRBarPozH->Position);
        reg2->pozH  =  TRBarPozH->Position;

       AnsiString as = in_volt_h = (((4.5 - 1.0) /( (double)MAX_Y_16)  ) * TRBarPozH->Position) + 1.0;

        as += " B";
        LBVoltH->Caption = as;
        }
        LDeltaN->Caption = abs(TRBarPozH->Position - TRBarPozL->Position);

        AnsiString as = fabs(in_volt_h - in_volt_l);
        as += " B";
        LDeltaB->Caption = as;
}

//TRBar_H
//---------------------------------------------------------------------------
void __fastcall TForm1::TRBarPozHChange(TObject *Sender)
{
if(flag_noise_image == 0)
{
        noise_PrintRezine(0,H_TR_BAR);
        noise_PrintRezine(1,H_TR_BAR);
}
else
{
        TBVisionClick(Sender);
}
LbPOZ_H->Caption =  TRBarPozH->Position;
reg2->pozH  =  TRBarPozH->Position;

AnsiString as = in_volt_h = (((4.5 - 1.0) /((double)MAX_Y_16)  ) * TRBarPozH->Position) + 1.0;
as += " B";
LBVoltH->Caption = as;
LDeltaN->Caption = abs(TRBarPozH->Position - TRBarPozL->Position);
        as = fabs(in_volt_h - in_volt_l);
        as += " B";
        LDeltaB->Caption = as;
}

//----------------------------------------------------------------------------
//TRBar_L
//---------------------------------------------------------------------------
void __fastcall TForm1::TRBarPozLScroll(TObject */*Sender*/,
      TScrollCode ScrollCode, int &/*ScrollPos*/)
{
if(ScrollCode == scEndScroll)
        reg2->WriteParamTooReg(1, (WORD)TRBarPozL->Position);
        reg2->pozL  =  TRBarPozL->Position;

        AnsiString as = in_volt_l = (((4.5 - 1.0) /((double)MAX_Y_16)) * TRBarPozL->Position) + 1.0;
        as += " B";
        LBVoltL->Caption = as;
        LDeltaN->Caption = abs(TRBarPozH->Position - TRBarPozL->Position);

        as = fabs(in_volt_h - in_volt_l);
        as += " B";
        LDeltaB->Caption = as;

}

//-----------------------------------------------------------------------------
//TRBar_L
//------------------------------------------------------------------------------
void __fastcall TForm1::TRBarPozLChange(TObject *Sender)
{
if(flag_noise_image == 0)
{
noise_PrintRezine(0,L_TR_BAR);
noise_PrintRezine(1,L_TR_BAR);
}
else
{
 TBVisionClick(Sender);
}

AnsiString as = in_volt_l = (((4.5 - 1.0) /((double)MAX_Y_16) ) * TRBarPozL->Position) + 1.0;
as += " B";
LBVoltL->Caption = as;
LbPOZ_L->Caption =  TRBarPozL->Position;
reg2->pozL  =  TRBarPozL->Position;
LDeltaN->Caption = abs(TRBarPozH->Position - TRBarPozL->Position);
as = fabs(in_volt_h - in_volt_l);
as += " B";
LDeltaB->Caption = as;
}


//--------------------------------------------------------------------------
// noise_Get, noise_Visible     ШУМ       GET 20 строк по 2048 pix
//---------------------------------------------------------------------------
void __fastcall TForm1::TBtNoiseClick(TObject *Sender)
{

ScrollBar_Y_Img->Visible = false;

r = GetParamRegData();
dw_len = 20 * (r->pixsels_per_string * 2);

InitDac(Sender, r->iz_dac);
noise_Get(&btr, dw_len, false);
 if(btr != NULL)
 {
 noise_AsText((WORD*)btr , dw_len/2, ChHex_Decm->State);
       if(r->pixsels_per_string == 2048)
                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4096));
       else
            if(r->pixsels_per_string == 2304)
                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 4608));
                else
            if(r->pixsels_per_string == 1024)
                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * 2048));
                            if(r->pixsels_per_string == 1152)
                                noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * (1152 *2)));
                            else
                                if(r->pixsels_per_string == 576)
                                        noise_Visible((WORD*)(btr + (TUpDStringNomber->Position - 1) * (576 * 2)));
 }
DWORD e;
DWORD p = s_dll_src_GetOfErrorConnect(&e);
Label16->Caption = e;
Label12->Caption = (HIWORD(p));
Label14->Caption = (LOWORD(p));
flag_noise_image = 0;
}
//-------------------------------------------------------------------------
//Задержка
//---------------------------------------------------------------------------
void __fastcall TForm1::TUpDStringNomberClick(TObject *Sender,
      TUDBtnType /*Button*/)
{
reg2->zd_poz = TUpDStringNomber->Position;
reg2->WriteParamTooReg(6, TUpDStringNomber->Position);
TBtNoiseClick(Sender);
}

//-----------------------------------------------------------------------------
// DllClose
//---------------------------------------------------------------------------
//Сведение
//-----------------------------------------------------------------------------
void __fastcall TForm1::ChReductClick(TObject */*Sender*/)
{
         if((unsigned char)ChReduct->State == 1)
         {
         ChReduct->Caption = "Сведение";
         Reduct_set(0x01);
         }
         else
         {
         ChReduct->Caption = "Без сведения";
         Reduct_set(0x00);
         }
}
//-----------------------------------------------------------------------------
//      Вoton Тест сведения -нажали
//-----------------------------------------------------------------------------
void __fastcall TForm1::TBTestClick(TObject */*Sender*/)
{
  TMTest->Visible = (TMTest->Visible ^ 1);
 if(TMTest->Visible == 1)
 {
 TMTest->Clear();
 TMTest->Text = *GetMesTest();
 }
}
//---------------------------------------------------------------------------
// Снимок
//---------------------------------------------------------------------------
void __fastcall TForm1::TBImageMouseDown(TObject */*Sender*/,
      TMouseButton /*Button*/, TShiftState /*Shift*/, int /*X*/, int /*Y*/)
{

flag_noise_image = 1;
//Put5McPerString(ChTimeString->State, 1);
Image_Get();
DWORD e;
DWORD p = s_dll_src_GetOfErrorConnect(&e);
Label16->Caption = e;
Label12->Caption = (HIWORD(p));
Label14->Caption = (LOWORD(p));

}

//-----------------------------------------------------------------
//   Отображение снимка
//---------------------------------------------------------------------------
void __fastcall TForm1::TBVisionClick(TObject */*Sender*/)
{

if(btr == NULL)
return;

if(len == 0L)
return;

if(flag_noise_image == 0)
return;

       if(r->pixsels_per_string == 1024)
       {
                        ScrollBar_Y_Img->Visible = true;        
                        noise_AsText((WORD*)btr , 1024 * 20, ChHex_Decm->State);
                        Image_SetBufer_1024(btr, len);
       }
       else
                if(r->pixsels_per_string == 2048)
                {
                        ScrollBar_Y_Img->Visible = true;
                        noise_AsText((WORD*)btr , 2048 * 20, ChHex_Decm->State);
                        Image_SetBufer_2048(btr, len);
                }
                else
                if(r->pixsels_per_string == 2304)
                {
                        ScrollBar_Y_Img->Visible = true;
                        noise_AsText((WORD*)btr , 2304 * 20, ChHex_Decm->State);
                        Image_SetBufer_2304(btr, len);
                }
                else
                if(r->pixsels_per_string == 1152)
                {
                        ScrollBar_Y_Img->Visible = true;
                        noise_AsText((WORD*)btr , 1152 * 20, ChHex_Decm->State);
                        Image_SetBufer_1152(btr, len);
                }
                else
                if(r->pixsels_per_string == 576)
                {

     //                   ScrollBar_Y_Img->Visible = true;
     //                   ScrollBar_Y->Visible = false;
                        
                        noise_AsText((WORD*)btr , 576 * 20, ChHex_Decm->State);
                        Image_SetBufer_576(btr, len);
                }

}

//---------------------------------------------------
// Снимок по таймеру
//---------------------------------------------------------------------------
void __fastcall TForm1::TimerTimer(TObject *Sender)
{
        if(reg2->timer_zdr != 0)
        {
        DWORD er;
                ChRts->Checked = RTS_N_PW;
                EscapeCommFunction(hComm,  POWER_DS);
                ::Sleep(reg2->timer_zdr);
        }
        flag_noise_image = 1;

        Image_ResetBk();
        Image_Get();
        if(reg2->timer_zdr != 0)
        {
                ::Sleep(reg2->timer_zdr);
                EscapeCommFunction(hComm, POWER_EN );
                ChRts->Checked = RTS_PW;
        }

DWORD e;
DWORD p = s_dll_src_GetOfErrorConnect(&e);
        Label16->Caption = e;
        Label12->Caption = (HIWORD(p));
        Label14->Caption = (LOWORD(p));
        PostMessage(Form1->Handle,WM_COMMAND,0,(long)TBVision->Handle);
}

//-------------------------------------------------------------------
//      Вoton Timer -нажали
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{

Timer->Enabled ^= 1;
}

//-------------------------------------------------------------------
//      Вoton StrGrid -нажали
//---------------------------------------------------------------------------
void __fastcall TForm1::TBAsTextClick(TObject *Sender)
{
 StrGridAsText->Visible ^= 1;
}
//-------------------------------------------------------------------
//      Hex/Dec
//---------------------------------------------------------------------------
void __fastcall TForm1::ChHex_DecmClick(TObject *Sender)
{
  if(ChHex_Decm->State == 1)
  ChHex_Decm->Caption = "As Hex";
  else
  ChHex_Decm->Caption = "As Dec";

  reg2->izhex = ChHex_Decm->State;


   noise_AsText((WORD*)btr , dw_len/2, ChHex_Decm->State);
   reg2->WriteParamTooReg(8, (bool)ChHex_Decm->State);
}
//---------------------------------------------------------------------------
//  Кеу_Up над StringGrid
//-----------------------------------------------------------------------
void __fastcall TForm1::StrGridAsTextKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
reg2->WriteParamTooReg(9, (WORD)StrGridAsText->TopRow);
reg2->sgtr = StrGridAsText->TopRow;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ChBModeClick(TObject *Sender)
{

SetModeRun((MODE)(ChBMode->Checked == true ? TEST : RUN));
if(ChBMode->Checked == true)
 ChBMode->Caption = "Без Матриц";
else
 ChBMode->Caption = "C матрицами";

}
//---------------------------------------------------------------------------

void __fastcall TForm1::UpMtrGClick(TObject *Sender, TUDBtnType Button)
{
BYTE p = UpMtrG->Position;
s_dll_src_PutGaneMtr(p);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
        if(i_wtr != NULL)
                delete[] i_wtr;
        if(reg2 != NULL)
        {
                if(reg2->timer_zdr > 0)
                       comm_Close();
                delete reg2;
                reg2 = NULL;
                Dll_Close();
        }

        if(rgb_bufer != NULL)
        delete[] rgb_bufer;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

void __fastcall TForm1::noise_SaveInFile(TObject *Sender)
{

      noise_SaveInFile((WORD*)btr);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::LoadFromFile1Click(TObject *Sender)
{
       noise_LoadFromFile((WORD*)btr);

}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_0Click(TObject *Sender)
{
SetDac(0);        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_1Click(TObject *Sender)
{
 SetDac(1);        
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_2Click(TObject *Sender)
{
SetDac(2);         
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_3Click(TObject *Sender)
{
SetDac(3);         
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_4Click(TObject *Sender)
{
SetDac(4);         
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_5Click(TObject *Sender)
{
SetDac(5);         
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_6Click(TObject *Sender)
{
SetDac(6);         
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_7Click(TObject *Sender)
{
   SetDac(7);      
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_8Click(TObject *Sender)
{
SetDac(8);         
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_9Click(TObject *Sender)
{
SetDac(9);         
}
//---------------------------------------------------------------------------

void __fastcall TForm1::RB_10Click(TObject *Sender)
{
SetDac(10);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::InitDac(TObject *Sender, int value)
{
        switch(value)
        {
        case 0: RB_0->Checked = 1; RB_0Click(RB_0); break;
        case 1: RB_1->Checked = 1; RB_1Click(RB_1); break;
        case 2: RB_2->Checked = 1; RB_2Click(RB_2); break;
        case 3: RB_3->Checked = 1; RB_3Click(RB_3); break;
        case 4: RB_4->Checked = 1; RB_4Click(RB_4); break;
        case 5: RB_5->Checked = 1; RB_5Click(RB_5); break;
        case 6: RB_6->Checked = 1; RB_6Click(RB_6); break;
        case 7: RB_7_->Checked = 1; RB_7Click(RB_7_); break;
        case 8: RB_8->Checked = 1; RB_8Click(RB_8); break;
        case 9: RB_9->Checked = 1; RB_9Click(RB_9); break;
        default: RB_10->Checked = 1; RB_10Click(RB_10); break;
        }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


void __fastcall TForm1::N1Click(TObject *Sender)
{
SendMessage(Form1->Handle,WM_CLOSE,0,0);
}
//---------------------------------------------------------------------------



void __fastcall TForm1::TBIcmp_TestKeyUp(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
 s_dll_src_SendIcmpPacet();
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TBIcmp_TestMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  s_dll_src_SendIcmpPacet();
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormPaint(TObject *Sender)
{
  Image_Paint(Sender);
}
//---------------------------------------------------------------------------
//-------------------------------------------------------------------
void __fastcall TForm1::LoadFromBinFileIn1Click(TObject *Sender)
{
        flag_noise_image = 1;
        s_dll_src_SetFlagLoadFromFile();
        len  = GetBufer((BYTE**)&btr ,len, false);
        TBImage->Enabled = true;
}




void __fastcall TForm1::LoadFromBinFileOut1Click(TObject *Sender)
{
        flag_noise_image = 1;
        s_dll_src_SetFlagLoadFromFileOut();
        len  = GetBufer((BYTE**)&btr ,len, false);
        TBImage->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::FormMouseMove(TObject *Sender, TShiftState Shift,
      int X, int Y)
{
 if( (X < (w_bmi + bg_w_bmi)) &&   (Y < (h_bmi + bg_h_bmi)) &&
     (X > bg_w_bmi) &&   (Y > bg_h_bmi) && (i_wtr != NULL))

 {

        Label7->Top =  Y > h_bmi/2 ?  Y - Label7->Height : Y - Label7->Height + cy + 2;
        Label7->Left = X > w_bmi/2 ?  X - Label7->Width : X;

         Label7->Caption = i_wtr[X - bg_w_bmi - 1 + (Y - bg_h_bmi) * w_bmi];
 }
}
//---------------------------------------------------------------------------

double __fastcall TForm1::FormMove()         
{
double kf;
WORD dt_h1;
WORD dt_h3;
   WORD screen_y = GetSystemMetrics( SM_CYSCREEN);
   dt_h3 = Label3->Top - (Label1->Top + Label1->Height);
   if(screen_y > 1370)
     kf = 1.0;
   else
   {
   kf = screen_y/(1370.0 );
   if(screen_y < 720)
        kf = kf / 1.1;
   }
   WORD h;
        TImage->Height =  TImage->Height * kf;

        ScrollBar_X->Top = TImage->Top + TImage->Height + 1;

        ScrollBar_Y->Top = TImage->Top;
        ScrollBar_Y->Height =  TImage->Height;

 //Ряд1

        h =  TImage->Height  + ScrollBar_X->Height + 10;
        UpDZoom_X->Top = h;
        TEdZoomX->Top = h;
        TUpDStringNomber->Top = h;
        TEdStringNomber->Top = h;
        Label5->Top = h;   //"Номер отображаемлй строки"
        UpDZoom_Y->Top = h;
        TEdZoomY->Top = h;
        TBtNoise->Top = h;
        TBTest->Top = h;
        TBImage->Top = h;
        TBVision->Top = h;
        TBAsText->Top = h;
        Button1->Top  =  h;//Как текст
        CMBCom->Top = h;

        BinningRGrp->Top = h;



//Ряд2
        h =  TEdZoomY->Top + TEdZoomY->Height + 3;
        Label1->Top = h;    //"ZOOM X"
        Label2->Top =  h;    //"ZOOM Y"
        ChTimeString->Top = h;
        ChReduct->Top = h;
        ChBMode->Top = h;
        UpMtrG->Top = h;
        EMtrG->Top = h;
        Label6->Top = h; //"Коеффициент усиления матриц"
        ChHex_Decm->Top = h;
        ChRts->Top = h;


 //Ряд3
        h = (Label1->Top + Label1->Height + dt_h3 * kf);
        Label3->Top = h;
        Label11->Top = h;
        Label12->Top = h;
        Label15->Top = h;
        Label16->Top = h;
        Label13->Top = h;
        Label14->Top = h;
        Label8->Top = h;
        LabPozScY->Top = h;
        TBIcmp_Test->Top  = h - TBIcmp_Test->Height/2;

//Ряд 4
        h =  Label3->Top + Label3->Height + 4;
        LbPOZ_H->Top = h;
        TRBarPozH->Top = h;
        LBVoltH->Top = h;
        TDiv_16_9_4->Top  = h;


//Ряд5
        h = TRBarPozH->Top + TRBarPozH->Height;
        Label4->Top = h;
//Ряд6
        h = Label4->Top + Label4->Height;
        LbPOZ_L->Top = h;
        TRBarPozL->Top  = h = Label4->Top + Label4->Height/2;;
        LBVoltL->Top = h;
        TDiv_4_3_2->Top  = h;
//Ряд7
        h = TRBarPozL->Top + TRBarPozL->Height + TRBarPozL->Height/5;
        RGBLevel->Top = h;
        TRange->Top = h;

        WORD fh = RGBLevel->Font->Height/2;
        h = h + (RGBLevel->Height / 2 - RB_0->Height/2 - fh);
        RB_0->Top =  h;    RB_2->Top =  h; RB_4->Top =  h; RB_6->Top =  h;
        RB_8->Top =  h;    RB_10->Top =  h;
        Label9->Top = h;//   Label10->Top = h;
//Ряд 8

        RB_1->Top = h;   RB_3->Top = h; RB_5->Top = h; RB_7_->Top = h;
        RB_9->Top = h; Label7->Top = h;
        LDeltaN->Top = h; LDeltaB->Top = h;
                           Label10->Top = h;
        TMTest->Left = TImage->Left;
        TMTest->Top = TImage->Top;
        TMTest->Height = TImage->Height;
        TMTest->Width = TImage->Width;
        TMTest->Visible = false;

        StrGridAsText->Height = TImage->Height;


                       ScrollBar_Y_Img->Left = ScrollBar_Y->Left;
                       ScrollBar_Y_Img->Height = ScrollBar_Y->Height;
                       ScrollBar_Y_Img->Top = ScrollBar_Y->Top;
                       ScrollBar_Y_Img->Width = ScrollBar_Y->Width;

                       ScrollBar_Y_Img->Visible = false;
                       ScrollBar_Y->Visible = true;

                       ScrollBar_Y_Img->SmallChange =   (Forms::TScrollBarInc)(32 * 4);
                       ScrollBar_Y_Img->Max = MAX_Y_16 / 32 - MAX_Y_16/(21 * 4);
                       ScrollBar_Y_Img->Position = reg2->scrbaza_y_im;


                        CMBCom->Enabled = false;
                        ChRts->Enabled = false;

                        BinningRGrp->Width = (30 + max(Binn1->Width,max(Binn2->Width,max(Binn3->Width,Binn4->Width))));

                        TFilter->Left = BinningRGrp->Left;
                        TFilter->Top = BinningRGrp->Top + BinningRGrp->Height + 2;
                   /*
if(r->pixsels_per_string == 576)
{
        TImage->Left = 1152/2 - 608/2;
}
        ScrollBar_Y->Left = TImage->Left + TImage->Width;
        ScrollBar_Y_Img->Left = Form1->Width - ScrollBar_Y->Width - 1;  //ScrollBar_Y->Left;
ScrollBar_Y_Img->Visible = false;
Form1->VertScrollBar->Visible = false;
                     */
 return kf;
 }


void __fastcall TForm1::Binn1Click(TObject *Sender)
{
r->iz_binning = 1;
s_dll_src_SendBinning(1);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Binn2Click(TObject *Sender)
{
r->iz_binning = 2;
s_dll_src_SendBinning(2);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Binn3Click(TObject *Sender)
{
r->iz_binning = 3;
s_dll_src_SendBinning(3);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Binn4Click(TObject *Sender)
{
r->iz_binning = 4;
s_dll_src_SendBinning(4);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::TFilterClick(TObject *Sender)
{
s_dll_src_SetFilter(TFilter->State == cbUnchecked ? '0':'1');
}

//---------------------------------------------------------------------------

void __fastcall TForm1::TRangeClick(TObject *Sender)
{
 dll_src_SetRange(((TCheckBox*)Sender)->Checked);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::TDiv_16_9_4Click(TObject *Sender)
{
 dll_src_SetDiv_16_9_4(((TCheckBox*)Sender)->Checked);
 if(( TDiv_16_9_4->Checked == 1) && ( TDiv_4_3_2->Checked == 1))
        TDiv_4_3_2->Checked = 0;

}
//---------------------------------------------------------------------------

void __fastcall TForm1::TDiv_4_3_2Click(TObject *Sender)
{
  dll_src_SetDiv_4_3_2(((TCheckBox*)Sender)->Checked);
 if(( TDiv_16_9_4->Checked == 1) && ( TDiv_4_3_2->Checked == 1))
  TDiv_16_9_4->Checked = 0;
}
//---------------------------------------------------------------------------

