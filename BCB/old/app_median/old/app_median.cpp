//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "app_median.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;

#define _NUM_TERM 3
#define X_TABL 3
#define Y_TABL 8
const char *strInit[] = {"A>B ",
                         "B>C ",
                         "A>C "};
COMPARISIJN compare[3];

WORD main_tabl[Y_TABL][X_TABL];
const WORD tabl_bits[] = {0x8000,0x4000,0x2000,0x1000,
                          0x0800,0x0400,0x0200,0x0100,
                          0x0080,0x0040,0x0020,0x0010,
                          0x0008,0x0004,0x0002,0x0001};

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
        : TForm(Owner)
{
  for(int i = 0; i < _NUM_TERM; i++)
        memcpy(compare[i].data, strInit[i], 4);
        srand(1);
  memset(&main_tabl,0 ,sizeof(WORD) * X_TABL * Y_TABL);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FormCreate(TObject *Sender)
{
    CreateMedianTable(3, 8);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::CreateMedianTable(BYTE x_size_tbl, WORD y_size_tbl)
{
      for(int i = 0; i < 8; i++)
      {
                for(int j = 0; j < x_size_tbl; j++)
                {
                      str_Init[tabl_bits[j]]
                      main_tabl
                }
      }
}
