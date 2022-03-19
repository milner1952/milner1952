//---------------------------------------------------------------------------



#pragma hdrstop

#include <vcl.h>
#include <windowsx.h>
#include "app_test.h"
#include "reg_app2.h"
#include "comm.h"
extern REGDATA_APP2 *reg2;

//---------------------------------------------------------------------------

#pragma package(smart_init)
void __fastcall TForm1::comm_Init()
{
 DWORD er;
 hComm = NULL;
 iz_rts = 0;
        CMBCom->Text = reg2->com_name;
        comm_Find();
        comm_Create();
        CMBCom->Enabled = true;
        ChRts->Enabled = true;
}

//-------------------------------------------------------------------------
void __fastcall TForm1::comm_Find()
{
String comm_port;


    for(int i = 1; i < 255; i++)
     {
        comm_port = "COM" + IntToStr(i);
        hComm = CreateFile(comm_port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
        if(hComm != INVALID_HANDLE_VALUE)
        {
            CMBCom->Items->Add("COM" + IntToStr(i));
            CloseHandle(hComm);
            hComm = NULL;

        }
     }
        if(hComm != NULL)
                CloseHandle(hComm);
}

//-------------------------------------------------------
void __fastcall TForm1:: comm_Close()
{
 DWORD er;
        com_exit = 1;
        if(hComm != NULL)
        {
                if(0 == SetCommState(hComm,&olddcb))
  	        {
		        er = GetLastError();
                        AnsiString as = "Не могу изменить DCB блок e = ";
                        as + er;
                        MessageBox(NULL, as.c_str(), "comm_Close", MB_OK);
	        }
        }

        if(dcb != NULL)
                delete dcb;
        if(hComm != NULL)
                CloseHandle(hComm);

}

//-----------------------------------------------------------
void __fastcall TForm1::comm_Create()
{
try
 {
  hComm = CreateFile(CMBCom->Text.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
 }
  catch(...)
 {
  MessageBox(NULL, "Не могу открыть COMPORT", "comm_Init", MB_OK);
  return;
 }
 DWORD er;
 dcb = new DCB;
 if(dcb == NULL)
        {
                MessageBox(NULL, "Не могу выделить память под DCB блок", "comm_Init", MB_OK);
		return ;
	}
 if(0 == GetCommState(hComm,dcb))
  	{
		er = GetLastError();
                AnsiString as = "Не могу получить DCB блок e = ";
                as + er;
                MessageBox(NULL, as.c_str(), "comm_Init", MB_OK);
		return ;
	}
        olddcb = *dcb;
        dcb->fRtsControl = RTS_CONTROL_HANDSHAKE;
        if(0 == GetCommState(hComm,dcb))
  	{
		er = GetLastError();
                AnsiString as = "Не могу изменить DCB блок e = ";
                as + er;
                MessageBox(NULL, as.c_str(), "comm_Init", MB_OK);
		return ;
	}
}
//---------------------------------------------------------------------------
// SetThreadPriority
void __fastcall TForm1::CMBComChange(TObject *Sender)
{
        if(hComm != NULL)
                CloseHandle(hComm);
        comm_Create();
}


