//---------------------------------------------------------------------------


#pragma hdrstop

#include <stdio.h>
#include <conio.h>
#include <time.h>


#include "packet32.h"


#include <ntddndis.h>


#include <string>


#include "icmp.h"
#include "adapter.h"
#include "test_dll.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------


extern BYTE mac_adres_lokal[];
extern BYTE ip_adres_local[];
extern BYTE ip_adres_remote[];
extern BYTE mac_adres_remote[];

extern HANDLE hRxEvIcmpGo;
extern HANDLE hRxEvIcmpRdy;
//extern bool flag_run_info_test;
static int error = 0;


//-------------------------------------------------------------
void icmp_Init(TForm *form,WORD debug)
{

        if(debug != 0)
        {
        srand(0); form->Visible = true;
        }
}

//------------------------------------------------
BYTE* icmp_SetICMPReq(BYTE *packetbuff)
{
BYTE *end;

BYTE proto[2] = {0x08,0x00};
        end = adapter_Set_EthernetHiader(packetbuff,mac_adres_remote, proto);
        end = adapter_SetIp_Hiader(end, strlen("ERROR") + sizeof(ICMP_HDR) , 0x01);
        end = icmp_Set_ICMPPacketHiader(end, 8,0);
return end;
}
//-------------------------------------------------
BYTE* icmp_Set_ICMPPacketHiader(BYTE *packetbuff, WORD type, WORD code)
{


WORD ident = rand();
WORD seguen = rand();

        packetbuff[0] = type;
        packetbuff[1] = code;
        packetbuff[2] = 0; //CH
        packetbuff[3] = 0; //CH

        packetbuff[4] = HIBYTE(ident);
        packetbuff[5] = LOBYTE(ident);
        packetbuff[6] = HIBYTE(seguen);
        packetbuff[7] = LOBYTE(seguen);

char *ptr= (char*)&packetbuff[8];
        strcpy(ptr,"ERROR");

      *((WORD*)&packetbuff[2]) = adapter_CalcIPChecksum(packetbuff, strlen("ERROR") + sizeof(ICMP_HDR));

return (BYTE*)(packetbuff + strlen("ERROR") + sizeof(ICMP_HDR) );
}
//-----------------------------------------------------------------
void icmp_SendIcmpPacket()
{
char packetbuff[500];
 //       flag_run_info_test = false;
        memset((unsigned char*)packetbuff, 0,500);
BYTE *end;
        end = icmp_SetICMPReq((unsigned char*)packetbuff);
WORD len = end - ((BYTE*)packetbuff);
        SetEvent(hRxEvIcmpGo);
        adapter_SendPacket((unsigned char*)packetbuff, len);
        if(WAIT_FAILED == WaitForSingleObject( hRxEvIcmpRdy,  INFINITE))
        {
        DWORD e = GetLastError();
                test_SetStringTooOutDebug2("ERROR: In adapter_Send - Icmp", e, "WaitForSingleObject");
        }
        if(!ResetEvent(hRxEvIcmpRdy))
        {
        DWORD e = GetLastError();
                test_SetStringTooOutDebug2("ERROR: In adapter_Send - Icmp", e, "ResetEvent");
        }
}