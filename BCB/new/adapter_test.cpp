//---------------------------------------------------------------------------
#ifdef __BORLANDC__
#include <vcl.h>
#pragma hdrstop

#include <ctime>
#include "adapter_test.h"
#include "adapter.h"
#include <stdio.h>
//#include <bios.h>




//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm2 *Form2;
bool flag_run_info_test;
bool p_flag;
DWORD full_len;

extern clock_t _1_mks;

#pragma warn -8070

__int64 Ticks();
__int64 Ticks()
{
//asm 586
//asm RDTSC
}

#pragma warn +8070




//---------------------------------------------------------------------------
__fastcall TForm2::TForm2(TComponent* Owner)
        : TForm(Owner)
{
time_old = Ticks();
flag_run_info_test = true; p_flag = true;
number = 0;
}

//---------------------------------------------------------------------------
void TForm2::adapter_testSet(IP_HDR *h_ip, UDP_HDR *h_udp)
{
  INFO i;
  __int64 tim;

   /*
  if(flag_run_info_test == false)
  {
        return;
  }
  */

  if(p_flag == false)
  return;

  tim = Ticks();
  memset((void*)&i, 0, sizeof(INFO));
  number++;
  i.delta_time = (tim - time_old)/ _1_mks; //100000;

  i.ip_identifier = h_ip->identif;  i.ip_data_len = h_ip->len; i.data_len = h_udp->len ; i.number = number;
  full_len +=    (adapter_Swaps(h_udp->len) - sizeof(UDP_HDR));

  info_mem.push_back(i);
  DWORD t =  Memo1->Text.Length();

  if(t > 2)
     Memo1->Text = "     ";
  time_old = Ticks();

}
//---------------------------------------------
void TForm2::adapter_testReset()
{
 if((info_mem.size() > 0)  &&  (flag_run_info_test == true))
 {
        info_mem.erase(info_mem.begin(),info_mem.end());
        full_len = 0L;
 }
 if(flag_run_info_test == false)
 {
   p_flag = false;
 }
  number = 0;
}
//----------------------------------------------------------

void __fastcall TForm2::TBVisMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
 Memo1->Clear();
 TV_INFO_ITERATOR it;

    AnsiString s;
  for(it = info_mem.begin(); it != info_mem.end();)
  {
     INFO *i;
           i = it++;

    char b[200];
    memset(b,200,0);
    sprintf(b,"%x",adapter_Swaps(i->ip_identifier));
    s +="Id = ";
    s += b;




    memset(b,200,0);
    sprintf(b,"%x",adapter_Swaps(i->ip_data_len));
    s += ";  Ip_data len =";
    s += b;

    s += ";   Data len =";
    s += adapter_Swaps(i->data_len ) - sizeof(UDP_HDR);


    s += ";   dlt_time(мкс) = ";
    memset(b,0,200);
    sprintf(b, "%10d", i->delta_time);

    s += b;
    s += "      N =";

    s += i->number;


    s += "\r\n";

  }
    s += "\r\n";
    s += "Full len = ";
    s +=  full_len;



   Memo1->Text = s;
}
//---------------------------------------------------------------------------
// Кнопка RUN, востанавливает записи после ошибки
void __fastcall TForm2::Button1Click(TObject *Sender)
{
     flag_run_info_test = true; p_flag == true;
}
//---------------------------------------------------------------------------
void  TForm2::Flag_run_info_test_Reset()
{
flag_run_info_test = false;
}
#endif