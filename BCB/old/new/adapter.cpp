//--------------------------------------------------------------------------


#ifndef unix
	#ifdef __BORLANDC__
		#pragma hdrstop
	#endif

   // #ifndef __BORLANDC__
	#ifdef _MSC_BUILD
        #define _CRT_SECURE_NO_WARNINGS
        #ifndef _CRT_SECURE_NO_DEPRECATE
            # define _CRT_SECURE_NO_DEPRECATE (1)
        #endif
        # pragma warning(disable : 4996)
    #endif

//#define TEST_ICMP

#include <stdio.h>
#include <conio.h>
#include <time.h>


#include "packet32.h"


#include <ntddndis.h>


#include <string>

#include "reg_app.h"
#include "adapter.h"
#include "icmp.h"
#include "test_dll.h"

#include <iphlpapi.h>


#include "arp.h"
#include "adapter_test.h"

#ifndef __BORLANDC__
	#include <WINIOCTL.H>
#endif


//---------------------------------------------------------------------------

#define SIZE_BUFFER_OFF_ADAPTER (2048000 * 8)

#define SIZE_OFF_ARP_PACET 28
#define UDP_PROTOKOL 0x11
#define ICMP_PROTOKOL 0x01
using namespace std;

#define SIZE_BUFFER_OFF_ADAPTER_NAME 100
static char lpAdapterName[SIZE_BUFFER_OFF_ADAPTER_NAME];

#define SIZE_BUFFER_OFF_PACKET     ((4096 + 34 + 40) * 2)
BYTE  buffer[SIZE_BUFFER_OFF_PACKET];
#define SIZE_BUFFER_OFF_RX_DATA   ((2048 * 2048 * 8))
BYTE  buffer_rx[SIZE_BUFFER_OFF_RX_DATA];
BYTE  *ptr_rx;

LPADAPTER  lpAdapter = 0;
LPPACKET   lpPacket_tx;
LPPACKET   lpPacket_rx;
BYTE       ip_adres_remote[4];
BYTE ip_adres_local[4];
BYTE mac_adres_remote[6];
BYTE mac_adres_lokal[6];
WORD portRemote;
WORD portLocal;
WORD identifikator;
DWORD rx_len;
DWORD t_rx_len;
WORD pixel_per_string;

DWORD data_len = 0;

DWORD idThRead;
HANDLE  hThRead;

HANDLE hRxEvArpGo;
HANDLE hRxEvUdpGo;
HANDLE hRxEvIcmpGo;

HANDLE hRxEvArpRdy;
HANDLE hRxEvUdpRdy;
HANDLE hRxEvIcmpRdy;


HANDLE hSendUdpPacket;



static WORD error = 0;
bool iz_svaz = 0;
bool flag_rs = 0;
BYTE bt_Put5McPerString;
double time_repit;

int  adapter_GetMacAdres(BYTE *mac, char* lpAdapterName);
void adapter_StartReadRut();
void ReadRut();
long adapter_ReadPacket( DWORD len, DWORD ret);
DWORD adapter_AnalysisPackets(LPPACKET lpPacket, DWORD ret);
BYTE* adapter_SetData(BYTE *packetbuff,BYTE *data ,WORD len);


void  adapter_CopyUdpData(BYTE *udp_data, WORD udp_data_len);
void  adapter_SendUdp_Packet(BYTE *data);
BYTE* adapter_SetUdp_Packet(BYTE *packetbuff, BYTE *data, WORD len);
BYTE *adapter_SetUdpHiader(BYTE *packetbuff, WORD len);


WORD adapter_CalcIPChecksum(BYTE* buffer, WORD count);
WORD adapter_CalcIPBufferChecksum(BYTE *ptr, WORD len);
static void adapter_SwapIPHeader(IP_HDR* h);

BOOL  adapter_PacketReset(LPADAPTER  AdapterObject    );

#ifdef __BORLANDC__
__int64 Ticks();
clock_t _1_mks;
#endif

clock_t t;
#ifdef __BORLANDC__
#pragma package(smart_init)
extern TForm2 *Form2;
#endif

static WORD iz_icmp_debug;
//---------------------------------------------------------------------------
int adapter_Init(bool iz_sv, double t_repit, WORD pix, unsigned int debug)
{

        if(!SetPriorityClass( GetCurrentProcess()  ,    HIGH_PRIORITY_CLASS   ))
        {
                test_SetStringTooOutDebug6("ERROR: In adapter_Init",  "SetPriorityClass");
         return - 14;
        }



       iz_icmp_debug = debug;
#ifdef __BORLANDC__
#ifdef TEST_ICMP
       if( iz_icmp_debug != 0)
                Form2 =   new TForm2(NULL);
       else
#endif       
                Form2 = NULL;
#endif
        pixel_per_string = pix;
        time_repit = t_repit;
        flag_rs = 0;

        lpPacket_rx = NULL;
        iz_svaz = iz_sv;
        identifikator = 0;
        memset(lpAdapterName, 0 , SIZE_BUFFER_OFF_ADAPTER_NAME);
        REGDATA_APP_INIT app_reg_init;

char *ptr = app_reg_init.ReadParamFromReg();


        if(ptr == NULL)
        {
                 test_SetStringTooOutDebug6("ERROR: In adapter_Init",  "No adapters in system");
                 return - 1;
        }



        strcpy(  lpAdapterName, ptr);

        test_SetStringTooOutDebug6("ADAPTER: ",  lpAdapterName);
#ifdef __BORLANDC__
        lpAdapter =   PacketOpenAdapter(lpAdapterName);
#else
		lpAdapter = PacketOpenAdapter((LPTSTR)lpAdapterName);
#endif

        if (!lpAdapter || (lpAdapter->hFile == INVALID_HANDLE_VALUE))
        {
                DWORD    dwErrorCode;
                dwErrorCode=GetLastError();
                AnsiString as = lpAdapterName;
                AnsiString bs = "ERROR: ";
#ifdef __BORLANDC__
				bs += dwErrorCode;
#else
				{
					char b[100]; memset(b, 0, 100);
					sprintf(b, "%s", dwErrorCode);
					bs += b;
				}
#endif
                bs += " PacketOpenAdapter, ";
                test_SetStringTooOutDebug6((char*)bs.c_str() , (char*)as.c_str());
        return -2;
        }


                memset(mac_adres_lokal,0 , 6);
                memset(mac_adres_remote, 0 , 6);
                 string s(lpAdapterName); int i = (int)s.find('{');
        if(adapter_GetMacAdres(mac_adres_lokal, (char*)&lpAdapterName[i]))
        {
        return -3;
        }
        if(PacketSetHwFilter(lpAdapter,/*NDIS_PACKET_TYPE_PROMISCUOUS*/NDIS_PACKET_TYPE_DIRECTED)==FALSE)
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init ",  "In PacketSetHwFilter");
                return -4;

        }


        if(PacketSetReadTimeout(lpAdapter,-1)==FALSE)
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init",  "PacketSetReadTimeout");
                return -6;
        }
        if((lpPacket_tx = PacketAllocatePacket())==NULL)
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init",  "PacketAllocatePacket");
                return -7;
        }

        ip_adres_remote[0] = 10; ip_adres_remote[1] = 10; ip_adres_remote[2] = 5; ip_adres_remote[3] = 15;
        ip_adres_local[0] = 10; ip_adres_local[1] = 10; ip_adres_local[2] = 5; ip_adres_local[3] = 1;
        portRemote = 2048; portLocal = 20000;

        hRxEvArpGo = CreateEvent( NULL, true, false, NULL);
        if(hRxEvArpGo == NULL)    //1
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init", "CreateEvent hRxEvArpGo");
                return - 9;
        }

        hRxEvUdpGo = CreateEvent( NULL, true, false, NULL);
        if(hRxEvUdpGo == NULL)     //2
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init", "CreateEvent hRxEvUdpGo");
                return - 9;
        }

        hRxEvArpRdy  = CreateEvent( NULL, true, false, NULL);
        if(hRxEvArpRdy == NULL)     //5
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init", "CreateEvent hRxEvArpRdy");
                return -12;
        }

        hRxEvUdpRdy  = CreateEvent( NULL, true, false, NULL);
        if(hRxEvUdpRdy == NULL)     //6
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init", "CreateEvent hRxEvUdpRdy");
                return -12;
        }


        hSendUdpPacket  = CreateEvent( NULL, true, true, NULL);
        if(hSendUdpPacket == NULL) //7
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init", "CreateEvent hSendUdpPacket");
                return -13;
        }


        hRxEvIcmpGo  = CreateEvent( NULL, true, false, NULL);
        if( hRxEvIcmpGo == NULL)     //6
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init", "CreateEvent  hRxEvIcmpGo");
                return -12;
        }


        hRxEvIcmpRdy  = CreateEvent( NULL, true, true, NULL);
        if(hRxEvIcmpRdy == NULL) //7
        {
                test_SetStringTooOutDebug6("ERROR:  In adapter_Init", "CreateEvent hRxEvIcmpRdyt");
                return -13;
        }
#ifdef __BORLANDC__
#ifdef  TEST_ICMP
        icmp_Init(Form2, iz_icmp_debug);
#endif
#endif
        if(iz_svaz == 1)
        adapter_StartReadRut();

  return 0;
}
//---------------------------------------------------------
void adapter_Delete()
{
        TerminateThread(hThRead,0);
        if( lpPacket_rx != NULL)
                PacketFreePacket(lpPacket_rx);
        PacketFreePacket(lpPacket_tx);
        PacketCloseAdapter(lpAdapter);
        CloseHandle(hRxEvArpGo);
        CloseHandle(hRxEvUdpGo);
        CloseHandle(hRxEvArpRdy);
        CloseHandle(hRxEvUdpRdy);
        CloseHandle(hSendUdpPacket);
#ifdef __BORLANDC__
        if(Form2 != NULL)
                delete Form2;
#endif
}

//----------------------------------------
int adapter_GetMacAdres(BYTE *mac , char *name)
{
#define SZ_IP_ADAPTER_INFO 5000
IP_ADAPTER_INFO* pAdapterInfo;
BYTE ptr[SZ_IP_ADAPTER_INFO ];
memset(ptr,0,SZ_IP_ADAPTER_INFO );
DWORD len = SZ_IP_ADAPTER_INFO;
int e;
        if((e =  GetAdaptersInfo((IP_ADAPTER_INFO* )ptr, &len)) != ERROR_SUCCESS)
        {
                switch(e)
                {
                        case ERROR_BUFFER_OVERFLOW:
                        {
                                test_SetStringTooOutDebug6("ERROR: adapter_GetMacAdres",  "ERROR_BUFFER_OVERFLOW");
                        break;
                        }
                        case ERROR_INVALID_PARAMETER:
                        {
                                test_SetStringTooOutDebug6("ERROR: adapter_GetMacAdres",  "ERROR_INVALID_PARAMETER");
                        break;
                        }
                        case ERROR_NO_DATA:
                        {
                                test_SetStringTooOutDebug6("ERROR: adapter_GetMacAdres",  "ERROR_NO_DATA");
                        break;
                        }
                        case ERROR_NOT_SUPPORTED:
                        {
                                test_SetStringTooOutDebug6("ERROR: adapter_GetMacAdres",  "ERROR_NOT_SUPPORTED");
                        break;
                        }
                        default:
                        {
                                test_SetStringTooOutDebug6("ERROR: adapter_GetMacAdres",  "ERROR_UN_KNOW");
                        break;
                        }
                }
        return - 3;
        }
        else
        {
                pAdapterInfo = (IP_ADAPTER_INFO*)ptr;
                do
                {
                        if(0 == strcmp((char*)pAdapterInfo->AdapterName, name))
                        break;
                        else
                        {
                                pAdapterInfo = pAdapterInfo->Next;
                        }
                }while(pAdapterInfo != NULL);
        }

        if(pAdapterInfo == NULL)
        {
                test_SetStringTooOutDebug6("ERROR: adapter_GetMacAdres",  "I CAN NOT FIND MAC ADRESS FRIM ADAPTER");
        return - 8;
        }
        else
                memcpy(mac, pAdapterInfo->Address, 6);
return 0;
}

//-------------------------------------------------
BYTE* adapter_Set_EthernetHiader(BYTE *packetbuff, BYTE *dest, BYTE *proto)
{
 	packetbuff[0] = dest[0];
	packetbuff[1] = dest[1];
	packetbuff[2] = dest[2];
	packetbuff[3] = dest[3];
	packetbuff[4] = dest[4];
	packetbuff[5] = dest[5];

	packetbuff[6] = mac_adres_lokal[0];
	packetbuff[7] = mac_adres_lokal[1];
	packetbuff[8] = mac_adres_lokal[2];
	packetbuff[9] = mac_adres_lokal[3];
	packetbuff[10] = mac_adres_lokal[4];
	packetbuff[11] = mac_adres_lokal[5];

        packetbuff[12] = proto[0];
        packetbuff[13] = proto[1];
return packetbuff + sizeof(ETHER_HDR);
}


//---------------------------------------------------

void adapter_SendPacket(BYTE *packetbuff, WORD len)
{
double	cpu_time;
        PacketInitPacket(lpPacket_tx,packetbuff,len);

        if(PacketSetNumWrites(lpAdapter,1)==FALSE)
        {
                error = 1;
                test_SetStringTooOutDebug1("ERROR: In adapter_SendPacket", error);
        return;
	}
	cpu_time = clock ();

        if(PacketSendPacket(lpAdapter,lpPacket_tx,TRUE)==FALSE)
        {
                error = 2;
                test_SetStringTooOutDebug1("ERROR: In adapter_SendPacket", error);
		return ;
        }
	cpu_time = (clock() - cpu_time)/CLK_TCK;
}

//---------------------------------------------------------------------
void adapter_StartReadRut()
{
        hThRead = CreateThread(NULL,  0,  (LPTHREAD_START_ROUTINE)ReadRut,  NULL,
                       CREATE_SUSPENDED,  &idThRead);
        if(hThRead == NULL)
        {
#ifdef __BORLANDC__
        error = GetLastError();
#else
		error = (WORD)GetLastError();
#endif
        test_SetStringTooOutDebug1("ERROR: In adapter_StartReadRut of ReadRut", error);
        return;
        }
        if(((DWORD)-1) == ResumeThread(hThRead))
        {
                test_SetStringTooOutDebug6("ERROR: In adapter_StartReadRut of ReadRut", "In ResumeThread");
        return;
        }
}

//-----------------------------------------------------------------------------
void ReadRut()
{                                     //2048000 * 16      32
        if(PacketSetBuff(lpAdapter,SIZE_BUFFER_OFF_ADAPTER)==FALSE)
        {
                test_SetStringTooOutDebug6("ERROR: In ReadRut()", "PacketSetBuff");
                return;
        }

        if(PacketSetReadTimeout(lpAdapter,-1)==FALSE)
        {
                test_SetStringTooOutDebug6("ERROR: In ReadRut()", "PacketSetReadTimeout");
                return;
        }

        if((lpPacket_rx = PacketAllocatePacket())==NULL)
        {
                test_SetStringTooOutDebug6("ERROR: In ReadRut()", "PacketAllocatePacket");
                return;
        }

        PacketInitPacket(lpPacket_rx,(char*)buffer, 4096  );

                        ResetEvent(hRxEvUdpGo);
                        flag_rs = false;
                        ResetEvent(hRxEvUdpRdy);

 HANDLE hnd[3] = {hRxEvArpGo,hRxEvUdpGo,hRxEvIcmpGo};
        for(;;)
        {
        DWORD ret;
        DWORD e;
#ifdef __BORLANDC__
        __int64 t1 = Ticks();
                ::Sleep(1);
        __int64 t2 = Ticks();
         _1_mks = (clock_t)(t2 - t1)/1000;
#endif

                ret = WaitForMultipleObjects(  sizeof(hnd)/sizeof(HANDLE),  hnd,  false,flag_rs == true ?INFINITE: 500 );
                if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST
                                                                                       /* THREAD_PRIORITY_HIGHEST  THREAD_PRIORITY_ABOVE_NORMAL*/))
                {
                        test_SetStringTooOutDebug2("ERROR: SetThreadPriority(: ",  GetLastError(), " SetThreadPriority ");
                }
                if(WAIT_FAILED == ret)
                {
                    e = GetLastError();
                        test_SetStringTooOutDebug2("ERROR: ReadRut: ", e, " WaitForMultipleObjects ");
                }

                if(ret == WAIT_OBJECT_0)
                {
                        ResetEvent(hRxEvArpGo);
                        Sleep(1);
                        if(-1 == (adapter_ReadPacket(SIZE_OFF_ARP_PACET, ret)))
                        return;
                        ResetEvent(hRxEvArpGo);
                        SetEvent(hRxEvArpRdy);
                }

                if(ret == WAIT_OBJECT_0  + 1)
                {
                        ResetEvent(hRxEvUdpGo);
                        if(-1 == ( adapter_ReadPacket(rx_len, ret)))
                        return;
                        flag_rs = false;
                        SetEvent(hRxEvUdpRdy);
                }


                if(ret == WAIT_OBJECT_0  + 2)
                {
                        ResetEvent(hRxEvIcmpGo);
                        if(-1 == ( adapter_ReadPacket((DWORD)strlen("ERROR"), ret)))
                        return;
                        flag_rs = false;
                    //    SetEvent(hRxEvIcmpRdy);
                }



                if(ret == WAIT_TIMEOUT)
                {
                        if(flag_rs == true)
                        continue;
                        ptr_rx = buffer_rx;
                        if(PacketReceivePacket(lpAdapter,lpPacket_rx,TRUE)==FALSE)
                                   test_SetStringTooOutDebug6("Error: In ReadRut()", "Error 1");
                }

        }

}


//-----------------------------------------------------------------------------------
void adapter_CopyUdpData(BYTE *udp_data, WORD udp_data_len)
{
        memcpy( (void*)ptr_rx, (const void*)udp_data, udp_data_len);
        ptr_rx += udp_data_len ;
}
//--------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------
void adapter_SendUdp_Packet(BYTE *data, WORD len_dt)
{
        data_len = 0;
char packetbuff[5000];
        memset(packetbuff, 0,5000);
BYTE *end;
        end = adapter_SetUdp_Packet((unsigned char*)packetbuff, data, len_dt);
WORD len = (WORD)(end - (BYTE*)packetbuff);
        adapter_SendPacket((unsigned char*)packetbuff, len);
}

//----------------------------------------------------------------------------------
BYTE* adapter_SetUdp_Packet(BYTE *packetbuff, BYTE *data, WORD len)
{
BYTE *end;
BYTE proto[2] = {0x08,0x00};
        end = adapter_Set_EthernetHiader(packetbuff,mac_adres_remote, proto);
        end = adapter_SetIp_Hiader(end, len + sizeof(UDP_HDR) , 0x11);
        end = adapter_SetUdpHiader(end, len + sizeof(UDP_HDR));
        end = adapter_SetData(end, data, len);
return end;
}
//-------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------
BYTE* adapter_SetIp_Hiader(BYTE *packetbuff,  WORD len, BYTE proto)
{
        WORD num = len +  sizeof(IP_HDR); // + sizeof(UDP_HDR);
        packetbuff[0] = 0x45;
        packetbuff[1] = 0x0;
        packetbuff[2] = LOBYTE(num) ;
        packetbuff[3] = HIBYTE(num) ;
        packetbuff[4] = LOBYTE(identifikator);

        packetbuff[5] = HIBYTE(identifikator);
        packetbuff[6] = 00;  // flags_end_offset h
        packetbuff[7] = 00;  // flags_end_offset l
        packetbuff[8] = 255; // ttl
        packetbuff[9] = proto;

        packetbuff[10] = 00;  //cymma HDR
        packetbuff[11] = 00;  //cymma HDR

        packetbuff[12] = ip_adres_local[0];     // 10
        packetbuff[13] = ip_adres_local[1];     //10
        packetbuff[14] = ip_adres_local[2];     //5
        packetbuff[15] = ip_adres_local[3];     //1

        packetbuff[16] = ip_adres_remote[0];    //10
        packetbuff[17] = ip_adres_remote[1];    //10
        packetbuff[18] = ip_adres_remote[2];    //5
        packetbuff[19] = ip_adres_remote[3];    //15
        identifikator++;

        adapter_SwapIPHeader((LPIP_HDR)packetbuff);
        *((WORD*)&packetbuff[10]) = (adapter_CalcIPChecksum(packetbuff, sizeof(IP_HDR)));
 return (BYTE*)(packetbuff + sizeof(IP_HDR));
}
//-------------------------------------------------------------------
//  
//-------------------------------------------------------------------
BYTE *adapter_SetUdpHiader(BYTE *packetbuff, WORD len)
{
        packetbuff[0] = HIBYTE(portLocal);
        packetbuff[1] = LOBYTE(portLocal);
        packetbuff[2] = HIBYTE(portRemote);
        packetbuff[3] = LOBYTE(portRemote);
        packetbuff[4] = HIBYTE(len);

        packetbuff[5] = LOBYTE(len);
        packetbuff[6] = 00;
        packetbuff[7] = 00;


PSEUDO_HEADER    pseudoHeader;
WORD  checksum;

        pseudoHeader.SourceAddress[0]   = ip_adres_local[0];
        pseudoHeader.SourceAddress[1]   = ip_adres_local[1];
        pseudoHeader.SourceAddress[2]   = ip_adres_local[2];
        pseudoHeader.SourceAddress[3]   = ip_adres_local[3];

        pseudoHeader.DestAddress[0]   = ip_adres_remote[0];
        pseudoHeader.DestAddress[1]   = ip_adres_remote[1];
        pseudoHeader.DestAddress[2]   = ip_adres_remote[2];
        pseudoHeader.DestAddress[3]   = ip_adres_remote[3];

        pseudoHeader.Zero               = 0x0;
        pseudoHeader.Protocol           = UDP_PROTOKOL;
        pseudoHeader.Length             = len;
        pseudoHeader.Length = adapter_Swaps(pseudoHeader.Length);
        checksum = ~adapter_CalcIPChecksum((BYTE*)&pseudoHeader,  sizeof(pseudoHeader));
        packetbuff[6] = LOBYTE(checksum );
        packetbuff[7] = HIBYTE(checksum );

        checksum = adapter_CalcIPBufferChecksum((BYTE*)&packetbuff[0],  len);

        packetbuff[6] = LOBYTE(checksum );
        packetbuff[7] = HIBYTE(checksum );

 return  (BYTE*)(packetbuff + sizeof(UDP_HDR));
}
//------------------------------------------------------------------
//  
//------------------------------------------------------------------
BYTE* adapter_SetData(BYTE *packetbuff,BYTE *data ,WORD len)
{
        memcpy(packetbuff, data, len);
 return (BYTE*)(packetbuff + len);
}
//-----------------------------------------------------------------
//
//------------------------------------------------------------------
void adapter_Send(char *data)
{
        WaitForSingleObject(  hSendUdpPacket,  INFINITE);
        ResetEvent(hSendUdpPacket);
        WORD len = (WORD)strlen(data);
        adapter_SendUdp_Packet((BYTE*)data, len);
        SetEvent(hSendUdpPacket);
}

//-----------------------------------------------------------------
WORD adapter_CalcIPChecksum(BYTE* buffer, WORD count)
{
WORD i;
WORD *val;
        union
        {
         DWORD Val;
                struct
                {
                WORD_VAL LSB;
                WORD_VAL MSB;
                } words;
        } tempSum, sum;

        sum.Val = 0;

        i = count >> 1;
        val = (WORD *)buffer;

        while( i-- )
                sum.Val += *val++;

        if ( count & 1 )
                sum.Val += *(BYTE *)val;

        tempSum.Val = sum.Val;

        while( (i = tempSum.words.MSB.Val) != 0u )
        {
                sum.words.MSB.Val = 0;
                sum.Val = (DWORD)sum.words.LSB.Val + (DWORD)i;
                tempSum.Val = sum.Val;
        }
        return (~sum.words.LSB.Val);
}

//------------------------------------------------------------
static void adapter_SwapIPHeader(IP_HDR* h)
{
        h->len      = adapter_Swaps(h->len);
        h->identif   = adapter_Swaps(h->identif);
        h->ch_sum_off_hd   = adapter_Swaps(h->ch_sum_off_hd);
}

//------------------------------------------------------------
WORD adapter_Swaps(WORD v)
{
WORD_VAL t;
BYTE b;
        t.Val   = v;
        b       = t.v[1];
        t.v[1]  = t.v[0];
        t.v[0]  = b;

        return t.Val;
}
//---------------------------------------------------------
DWORD adapter_ResiveBuffer(BYTE **ptr, DWORD len)
{
        test_SetStringTooOutDebug2("In: adapter_ResiveBuffer - Udp: ", 0, "WaitForSingleObject ");
#ifdef __BORLANDC__
#ifdef TEST_ICMP
        if(iz_icmp_debug != 0)
        {
                Form2->adapter_testReset();
        }
#endif
#endif
        rx_len = len;
        t_rx_len = 0;
        data_len = 0;
        ptr_rx = buffer_rx;
        ResetEvent(hRxEvUdpRdy);
        memset((void*)ptr_rx, 0, sizeof(buffer_rx));

        SetEvent(hRxEvUdpGo);

        if(WAIT_FAILED == WaitForSingleObject( hRxEvUdpRdy,  INFINITE))
        {
        DWORD e = GetLastError();
                test_SetStringTooOutDebug2("ERROR: adapter_ResiveBuffer - Udp: ", e, "WaitForSingleObject ");
        }

        flag_rs = false;
        (*ptr) =  buffer_rx;

        test_SetStringTooOutDebug2("OUT: adapter_ResiveBuffer - Udp: ", 0, "WaitForSingleObject ");
 return t_rx_len;
}
//---------------------------------------------------------------------------------------
long adapter_ReadPacket( DWORD len, DWORD ret)
{
clock_t  e;
clock_t  t1;
DWORD num;
num = 0;
        e = (len / (pixel_per_string * 2));
        e++;
#ifdef __BORLANDC__
        e = e * time_repit;
#else
        e = e * (clock_t)time_repit;
#endif
        e = (e  / bt_Put5McPerString);
        e += 1000;
        t1 = clock();
        do
        {
                 if(PacketReceivePacket(lpAdapter,lpPacket_rx,TRUE)==FALSE)
                 {
                        test_SetStringTooOutDebug6("Error: In adapter_ReadPacket", "Error 1");
                        return (-1);
                 }
                 num += adapter_AnalysisPackets(lpPacket_rx,  ret);

                 if(ret != WAIT_OBJECT_0)
                 {
                        if((((clock() - t1)/ CLK_TCK) * (clock_t)1000.0) > e)
                        {
#ifdef __BORLANDC__
#ifdef  TEST_ICMP
                                if(iz_icmp_debug != 0)
                                Form2->Flag_run_info_test_Reset();
                             //           icmp_SendIcmpPacket();
#endif
#endif
#ifdef __BORLANDC__
                          AnsiString a_num;
                                        a_num = "Error 2, resiv: num =";
                                        a_num += num;
                                        a_num += ", reqvest: len =";
                                        a_num += len;
#else
						AnsiString a_num("Error 2, resiv: num =");
						char b[20];
								memset(b, 0, 20);
								itoa(num, b, 10);
								a_num += b;
								a_num += ", reqvest: len =";
								memset(b, 0, 20);
								itoa(len, b, 10);
								a_num += b;

#endif
                                        test_SetStringTooOutDebug6("Error: In adapter_ReadPacket", (char*)a_num.c_str());
                          return 0;
                        }
                 }
        }while(num < len);
return 0L;
}

//------------------------------------------------------------------------------------------
DWORD adapter_AnalysisPackets(LPPACKET lpPacket, DWORD ret)
{
DWORD dwBytesReceived = lpPacket->ulBytesReceived;
char* buf = (char*)lpPacket->Buffer;
char* pChar;
DWORD off = 0L;
WORD  cap_len;
struct bpf_hdr *hdr;
while(off < dwBytesReceived)
{
        hdr = (struct bpf_hdr*)(buf + off);
        cap_len = hdr->bh_caplen;
        off += hdr->bh_hdrlen;
        pChar = (char*)(buf + off);
        off = Packet_WORDALIGN(off + cap_len);

        LPETHER_HDR ether_hdr = (LPETHER_HDR)pChar;
        if((WORD)memcmp(ether_hdr->dest, mac_adres_lokal,6))
        {
        continue;
        }
        else
        if(ether_hdr->type == 0x0608)      
                {
                data_len = 0;
                LPARP_HDR arp_hdrr = (LPARP_HDR)(((BYTE*)ether_hdr) + sizeof(ETHER_HDR));
                        if(memcpy(arp_hdrr->dst_ip_adres, ip_adres_local,4))
                        {
                                memcpy(mac_adres_remote, arp_hdrr->src_mac_adres, 6);
                                data_len += sizeof(ARP_HDR);
                                if(ret == WAIT_OBJECT_0)
                                        return data_len;
                                else
                                        continue;
                        }
                }
                else
                if(ether_hdr->type == 0x0008)    
                {
                   if((ret != WAIT_OBJECT_0  + 2) && (ret != WAIT_OBJECT_0  + 1))
                        continue;
                   LPIP_HDR ip_hdr =   (LPIP_HDR)(((BYTE*)ether_hdr) + sizeof(ETHER_HDR));
                      
                        if(ip_hdr->protokol != UDP_PROTOKOL)
                        {
                                if(ip_hdr->protokol != ICMP_PROTOKOL)   
                                {
                                continue;
                                }
                                else       //ICMP
                                {
                                        if((WORD)memcmp(ip_hdr->dst_ip_adres, ip_adres_local,4))
                                        {
                                                continue;
                                        }
                                        else
                                        {
                                        LPICMP_HDR icmp_hdr = (LPICMP_HDR)((BYTE*)ip_hdr + sizeof(IP_HDR));

                                         test_SetStringTooOutDebug2("RESIV ICMP PACKET", 0, " 00");
                                        char *icmp_data = (char*)icmp_hdr + sizeof(ICMP_HDR);
                                         SetEvent(hRxEvIcmpRdy);
                                         WORD r = (WORD)strlen(icmp_data);
                                         return r;
                                        }
                                }
                        }
                        else                //No IP adres
                                 if((WORD)memcmp(ip_hdr->dst_ip_adres, ip_adres_local,4))
                                 {
                                        continue;
                                 }
                                   else        // No IP -summ
                                   {
                                        if(adapter_CalcIPBufferChecksum((BYTE*)ip_hdr, sizeof(IP_HDR)))
                                        {
                                        continue;
                                        }
                                        else
                                        {        // No UDP - port
                                        BYTE ip_hdr_len = (ip_hdr->ver_end_lenhd << 2);
                                        LPUDP_HDR udp_hdr =  (LPUDP_HDR)((BYTE*)ip_hdr + ip_hdr_len);
                                                if(adapter_Swaps(udp_hdr->port_dst) !=  portLocal)
                                                {
                                                continue;
                                                }
                                                else
                                                {      // No data
                                                BYTE *udp_data;
                                                WORD t = sizeof(UDP_HDR);
                                                        udp_data = (BYTE*)((BYTE*)udp_hdr + t);
                                                WORD udp_data_len = adapter_Swaps(udp_hdr->len);
                                                       udp_data_len -= sizeof(UDP_HDR);
                                                       t =  hdr->bh_datalen;

                                                       adapter_CopyUdpData(udp_data,udp_data_len);
                                                       t_rx_len  += udp_data_len;

#ifdef __BORLANDC__
#ifdef  TEST_ICMP
                                                       if(iz_icmp_debug != 0)
                                                                Form2->adapter_testSet(ip_hdr, udp_hdr);
#endif
#endif

                                                       if(t_rx_len >= rx_len)
                                                        {
                                                                data_len  = t_rx_len;
                                                                SetEvent(hRxEvUdpRdy);
                                                                return data_len;
                                                        }

                                                }
                                         }
                                  }

                }
}

return 0;
}


//--------------------------------------------------
//
//----------------------------------------------------
WORD adapter_CalcIPBufferChecksum(BYTE *ptr, WORD len)
{
BOOL lbMSB;
WORD_VAL checkSum;
BYTE Checkbyte;
    lbMSB = TRUE;
    checkSum.Val = 0;
    int i = 0;
    while( len-- )
    {
        Checkbyte = ptr[i++];
        if ( !lbMSB )
        {
            if ( (checkSum.v[0] = Checkbyte+checkSum.v[0]) < Checkbyte)
            {
                if ( ++checkSum.v[1] == 0 )
                    checkSum.v[0]++;
            }
        }
        else
        {
            if ( (checkSum.v[1] = Checkbyte+checkSum.v[1]) < Checkbyte)
            {
                if ( ++checkSum.v[0] == 0 )
                    checkSum.v[1]++;
            }
        }
        lbMSB = !lbMSB;
    }
    checkSum.v[1] = ~checkSum.v[1];
    checkSum.v[0] = ~checkSum.v[0];
    return checkSum.Val;
}

//------------------------------------------------------------------
BOOL  adapter_PacketReset(LPADAPTER  AdapterObject    )
{
DWORD       BytesReturned;
OVERLAPPED oL;
        memset(&oL, 0, sizeof(OVERLAPPED));
return (BOOL)DeviceIoControl(AdapterObject->hFile,(DWORD)IOCTL_PROTOCOL_RESET,
                             NULL, 0, NULL, 0, &BytesReturned, &oL);
}

//-----------------------------------------------------------------
void adapter_Put5McPerString(unsigned char d)
{
      bt_Put5McPerString = d == 0 ? 2 : 1;

}

//----------------------------------------------------------------
void adapter_SetFlagRs()
{
 flag_rs = true;
}

//-----------------------------------------------------------------
BYTE* adapter_GetBufferAdres()
{

 return (BYTE*)buffer_rx;
}

#else
    //ifdef unix
//-----------------------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------------------

#include <QtWidgets>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "adapter.h"


using namespace std;

int sock_src;
struct sockaddr_in addr_in_rem;


U_ADRES_TEMP adr_t;
TEST_SETSTRINGTOOOUTDEBUG2 test_SetStringTooOutDebug2;

double time_repit;
static WORD pixel_per_string;
static WORD iz_icmp_debug;
static BYTE *t_ptr;

//------------------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------------------
 int adapter_Init(bool iz_connect, double t_repit, WORD pix, unsigned int debug)
 {


     QLibrary     lib_tst("./s_test_dll");
     QString e;

        test_SetStringTooOutDebug2 = (TEST_SETSTRINGTOOOUTDEBUG2)(lib_tst.resolve("test_SetStringTooOutDebug2"));
        if(!test_SetStringTooOutDebug2)
        {

            e = lib_tst.errorString();
            string s = e.toStdString();
            test_SetStringTooOutDebug2((char*)"adapter_Init", 0, (char*)s.c_str());
            return 1;
        }


        time_repit = t_repit;
        pixel_per_string = pix;
        iz_icmp_debug = debug;

        if(iz_connect == 0)
             return 0;

         adr_t.b[3] = 10;
         adr_t.b[2] = 10;
         adr_t.b[1] = 5;
         adr_t.b[0] = 15;



        sock_src =  socket(AF_INET, SOCK_DGRAM, 0);
        if(sock_src < 0)
        {
            test_SetStringTooOutDebug2((char*)"adapter_Init", 0, (char*)"ERROR: sock_src ");
            return 2;
        }

        addr_in_rem.sin_family = AF_INET;
        addr_in_rem.sin_port = htons(PORT_REMOTE);
        addr_in_rem.sin_addr.s_addr = htonl( adr_t.l);

 return 0;
 }

 //-----------------------------------------------------------------
 BYTE* adapter_GetBufferAdres()
 {

  return t_ptr;
 }

 //-----------------------------------------------------------------------
 void          adapter_Send(char *data)
 {
         int num = strlen(data);
         if(-1 == sendto(sock_src, data, num, 0, (sockaddr*)&addr_in_rem, sizeof(addr_in_rem)))
           {
            test_SetStringTooOutDebug2((char*)" adapter_Send", 0, (char*)"ERROR:send");
           }
 }

 //------------------------------------------------------------------------
 bool          arp_SendArpPacket()
 {

    return true;
 }
 //------------------------------------------------------------------------
 DWORD         adapter_ResiveBuffer(BYTE **ptr, DWORD len)
 {

     DWORD num = 0;
     t_ptr = *ptr;
     do{
         DWORD ret;
           t_ptr += num;
           ret = recv(sock_src, *ptr, len - num, 0);
           num += ret;
     }while(num < len);


 return num;
 }
 //-----------------------------------------------------------------------------
 void          adapter_Delete()
 {
        close(sock_src);
 }

 //-----------------------------------------------------------------------------
 void          adapter_Put5McPerString(unsigned char /*d*/)
 {

 }

 //-----------------------------------------------------------------------------
 void          icmp_SendIcmpPacket()
 {

 }

 //-----------------------------------------------------------------------------
 void          adapter_SetFlagRs()
 {

 }

#endif // end ifdef unix
