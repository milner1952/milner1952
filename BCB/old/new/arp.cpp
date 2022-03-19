//---------------------------------------------------------------------------


#pragma hdrstop

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "packet32.h"
#include "ntddndis.h"
#include <string>

#include "test_dll.h"
#include "adapter.h"
#include "arp.h"

//---------------------------------------------------------------------------
BYTE* arp_SetARPReq(BYTE *packetbuff);
BYTE* arp_Set_PacketHiader(BYTE *packetbuff, WORD kod_operat);

extern BYTE mac_adres_lokal[];
extern BYTE ip_adres_local[];
extern BYTE ip_adres_remote[];
extern BYTE mac_adres_remote[];

extern HANDLE hRxEvArpGo;
extern HANDLE hRxEvArpRdy;

static int error = 0;
//------------------------------------------------
BYTE* arp_SetARPReq(BYTE *packetbuff)
{
BYTE *end;
BYTE dest[6]={0xff,0xff,0xff,0xff,0xff,0xff};
BYTE proto[2] = {0x08,0x06};
        end = adapter_Set_EthernetHiader(packetbuff,dest, proto);
        end = arp_Set_PacketHiader(end, REQ);
return end;
}

//--------------------------------------------
BYTE* arp_Set_PacketHiader(BYTE *packetbuff, WORD kod_operat)
{
BYTE *end;
        packetbuff[0] = 0x00;    // hard type h
        packetbuff[1] = 0x01;    // hard type l
        packetbuff[2] = 0x08;    // prot type l
        packetbuff[3] = 0x00;    // prot type h
        packetbuff[4] = 0x06;     // hard size
        packetbuff[5] = 0x04;     // prot size
        packetbuff[6] = HIBYTE(kod_operat);
        packetbuff[7] = LOBYTE(kod_operat);


 	packetbuff[8] = mac_adres_lokal[0];         // hard adres src
	packetbuff[9] = mac_adres_lokal[1];
	packetbuff[10] = mac_adres_lokal[2];
	packetbuff[11] = mac_adres_lokal[3];
	packetbuff[12] = mac_adres_lokal[4];
	packetbuff[13] = mac_adres_lokal[5];

        packetbuff[14] =  ip_adres_local[0];
        packetbuff[15] =  ip_adres_local[1];
        packetbuff[16] =  ip_adres_local[2];
        packetbuff[17] =  ip_adres_local[3];
        if(kod_operat == REQ)
        {
                packetbuff[18] = 0;
                packetbuff[19] = 0;
                packetbuff[20] = 0;
                packetbuff[21] = 0;
                packetbuff[22] = 0;
                packetbuff[23] = 0;
        }
        else
        {
                packetbuff[18] = mac_adres_remote[0];
                packetbuff[19] = mac_adres_remote[1];
                packetbuff[20] = mac_adres_remote[2];
                packetbuff[21] = mac_adres_remote[3];
                packetbuff[22] = mac_adres_remote[4];
                packetbuff[23] = mac_adres_remote[5];
        }
        packetbuff[24] = ip_adres_remote[0];
        packetbuff[25] = ip_adres_remote[1];
        packetbuff[26] = ip_adres_remote[2];
        packetbuff[27] = ip_adres_remote[3];
        end = &packetbuff[28];
    return end;
}

//---------------------------------------------------------------------------

bool arp_SendArpPacket()
{
char packetbuff[5000];
        memset((unsigned char*)packetbuff, 0,5000);
BYTE *end;
        end = arp_SetARPReq((unsigned char*)packetbuff);
WORD len = (WORD)(end - ((BYTE*)packetbuff));
        adapter_SendPacket((unsigned char*)packetbuff, len);


        SetEvent(hRxEvArpGo);
        DWORD ret;
        ret = WaitForSingleObject( hRxEvArpRdy, 1000 /*INFINITE*/);
        if(WAIT_TIMEOUT == ret)
        {
           test_SetStringTooOutDebug2("WAIT_TIMEOUT: In adapter_Send - Arp ", 1, " Waite ARP Answer");
           return 1;
        }
        if(WAIT_FAILED == ret)
        {
        DWORD e = GetLastError();
                test_SetStringTooOutDebug2("ERROR: In adapter_Send - Arp", e, "WaitForSingleObject");
                return 1;
        }
        if(!ResetEvent(hRxEvArpRdy))
        {
        DWORD e = GetLastError();
                test_SetStringTooOutDebug2("ERROR: In adapter_Send - Arp", e, "ResetEvent");
                return 1;
        }
        return 0;
}
#ifdef __BORLANDC__
#pragma package(smart_init)
#endif
