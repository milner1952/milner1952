//sudo chmod -v u+s qtcreator_process_stub 0755 -> 4755
//-rwxr-xr-x  1 milner milner     9124 Oct 15  2015 qtcreator_process_stub
//milner@milnerPC /media/milner/1ТБ/ARXIV/MOI/Keil/S_KAMERA/build-group_s_test_app-Desktop_Qt_5_5_1_GCC_32bit-Release $ sudo ./_test_app
//https://habrahabr.ru/post/78094/-Создание deb пакетов
//media/milner/700GB_ARXIV/MAP/S_KAMERA/build-group_s_test_app-Desktop_Qt_5_5_1_GCC_32bit-Release $
//export LD_LIBRARY_PATH=/media/milner/700GB_ARXIV/MAP/S_KAMERA/build-group_s_test_app-Desktop_Qt_5_5_1_GCC_32bit-Release/:$LD_LIBRARY_PATH
//usr/lib/i386-linux-gnu/libQt5Core - QTDIR
// sudo strace ./_test_app
// /etc/ld.so.conf ldconfig
//nc, ncat или netcat
//echo GET 8396800 | nc -u 10.10.5.15 2048 > nen_cc.txt

//1. Загрузка процессора: В отдельной консоли  выдать:
//while true; do let A++; done Для остановки ctrl-С

//2. Загрузка диска. Для этого из-под root в отдельной консоли надо выдать:
//badblocks -v .диск Для остановки ctrl-С

//3. Загрузка сети. В отдельной консоли из-под root выдать:
//ping -f ip-Вашего-контроллера Для остановки ctrl-С


/*
 Проверены пути файлов:
 *.ini - "C:\\Users\\Public\\" или от прог. ФМЦ, CPK
 *.tif - всегда "C:\\Users\\Public\\p_dll_udp_"
 statistic.log всегда "C:\\Users\\Public\\statistic_log.log"
 *.txt -при LOG_DEBUG = 1 всегда "C:\\Users\\Public\\*.txt"
 */

//
// tar -cvf test_app.tar *

//dirname

#define __CRT__NO_INLINE

#include <string.h>
#ifdef unix
    #include <dlfcn.h>
    #include <dlfcn.h>
    #include <unistd.h>
    #include <fcntl.h>

#endif
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <string>
#include <iostream>
#include <sys/types.h>

#ifdef unix
    #include <unistd.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#else
    #include <Winsock2.h>
    #include <Windows.h>
    #include <WinUser.h>

    #include <ws2tcpip.h>
    #include <stdio.h>
    #include <stdlib.h>
    #pragma comment(lib, "Ws2_32.lib")
#endif
#include "../exp_error/exp_error.h"
#include "adapter.h"


using namespace std;
#ifdef unix
    int sock_src;
    int sock_icmp;
 //   struct icmphdr icmp_hdr;
#else
    WSADATA  WSData;
    SOCKET sock_src;
    SOCKET sock_icmp;
 //   ICMP_HDR icmp_hdr;
struct sockaddr_in    addr_in_loc;
#endif


int sequence;
struct sockaddr_in addr_in_icmp;
struct sockaddr_in addr_in_rem;

U_ADRES_TEMP adr_t;
TEST_DLL_SHOW test_dll_Show;
const char* ptr_e;
double time_repit;
static WORD pixel_per_string;
static WORD iz_icmp_debug;
static DWORD error_ch;


#define SIZE_BUFFER_OFF_RX_DATA   ((2048 * 2048 * 4))
BYTE  buffer_rx[SIZE_BUFFER_OFF_RX_DATA];
DWORD EXP_ERROR::error;
string EXP_ERROR::text_out;

 //void MessegeOfBufer(WORD *ptr, DWORD len);

//--------------------------------------------------------------------------
//Вызывается при инициализации dll
// аргументы:
//              bool iz_connect - при iz_connect == 0, не производится инициализация сокетов,
//     связь по Ethernet - отсутствует
//              double t_repit - время повторения запросов-не реализовано
//              WORD pix - количество пикселей в строке
//              unsigned int debug - отладка с ICMP - не реализовано
// возвращаемое значение:
//          - 0 - при отсутствии ошибок
//          - 1 - при наличии ошибок
// коментарии:
//              - функция - обертка для обработок except. генерируемых функцией
//    int adapter_InitSock(bool iz_connect, double t_repit, WORD pix, unsigned int debug);
//---------------------------------------------------------------------------
 int adapter_Init(string *dir, bool iz_connect, double t_repit, WORD pix, unsigned int debug)
 {
     EXP_ERROR::ErrorRes();
     try
     {
     return adapter_InitSock(dir, iz_connect,  t_repit,  pix,  debug) ;
     }
     catch(EXP_ERROR &e)
     {
            e.ErrorSetText("p_dll_adapter");
       //  e.ErrorShow("p_dll_adapter", "adapter_Init");
         return 1;
     }
     return 0;
 }

 //--------------------------------------------------------------------------
 //Вызывается при инициализации dll
 // аргументы:
 //              bool iz_connect - при iz_connect == 0, не производится инициализация сокетов,
 //     связь по Ethernet - отсутствует
 //              double t_repit - время повторения запросов-не реализовано
 //              WORD pix - количество пикселей в строке
 //              unsigned int debug - отладка с ICMP - не реализовано
 // возвращаемое значение: - 0
 // коментарии:
 //              - функция - генерирует исключения:
 //      -если не находит p_test_dll;
 //      -если не находит функции:  test_dll_Show_s_dw_s,
 //      - если не может выполнить инициализацию WINSOCK2 в Windows функцией WSAStartup(MAKEWORD(VMJ,VMN);
 //      - если не может выполнить инициализацию сокетов
 //      - если не может получить опции сокетов
 //      - если не может установить опции сокетов
 //              - функция транслирует исключения генерируемык функцией void  adapter_InitIcmp()
 //---------------------------------------------------------------------------
 int adapter_InitSock(string *dir, bool iz_connect, double t_repit, WORD pix, unsigned int debug)
 {
       error_ch = 0L;
       //string p = *dir;
  #ifdef unix    
                string p = *dir;
                p += "/libp_test_dll.so";
    void *test_lib = dlopen(p.c_str(), RTLD_LAZY);
  #else
    HMODULE test_lib;
                //p +=  "\\p_test_dll";
                if(dir == NULL)
                        test_lib = LoadLibrary(TEXT("p_test_dll"));
                else
                {
                    string p = *dir;
                                p += "\\p_test_dll";
                    wchar_t w_buf[MAX_PATH];
                                wmemset(w_buf,L'0',MAX_PATH);
                        AnsiToUnicode(p.c_str(), (wchar_t* )&w_buf);
                        test_lib = LoadLibrary(w_buf);
                }
  #endif
        if(!test_lib)
        {
 #ifdef unix
         char e[1000];
                 strcpy(e, dlerror());
                 throw EXP_ERROR("adapter_InitSock",e);
 #else
                 DWORD er = GetLastError();
                 throw EXP_ERROR("adapter_InitSock","Cannot find p_test_dll", er);
 #endif

        }

  #ifdef unix
        test_dll_Show = (TEST_DLL_SHOW)dlsym(test_lib, "test_dll_Show_s_dw_s");
        if((ptr_e = dlerror()) != NULL)
        {
         char e[1000];
                strcpy(e, ptr_e);
                throw EXP_ERROR("adapter_InitSock",e);
        }
  #else
        test_dll_Show = (TEST_DLL_SHOW)GetProcAddress(test_lib, "test_dll_Show_s_dw_s");
        if(!test_dll_Show)
        {
            DWORD er = GetLastError();
            throw EXP_ERROR("adapter_InitSock","Cannot find test_dll_Show_s_dw_s", er);
        }

  #endif
        time_repit = t_repit;
        pixel_per_string = pix;
        iz_icmp_debug = debug;

        if(iz_connect == 0)
             return 0;

         adr_t.b[3] = 10;
         adr_t.b[2] = 10;
         adr_t.b[1] = 5;
         adr_t.b[0] = 15;
#ifdef unix
        sock_src =  socket(AF_INET, SOCK_DGRAM, 0);
        if(sock_src < 0)
#else
const int VMJ = 2;
const int VMN = 2;
DWORD e;
         if(WSAStartup(MAKEWORD(VMJ,VMN), &WSData))
         {
            e =  WSAGetLastError();
             throw EXP_ERROR("adapter_InitSock", "WSAStartup", e);
         }

        sock_src =  socket(PF_INET, SOCK_DGRAM, 0);
        if(sock_src == INVALID_SOCKET)
#endif //#ifdef unix
        {
            throw EXP_ERROR("adapter_InitSock", "sock_src == INVALID_SOCKET");
        }

int sock_buf_size;
int len_sock_buf_size = sizeof(sock_buf_size);
int ret;
        ret = getsockopt(sock_src, SOL_SOCKET, SO_RCVBUF,(char *)&sock_buf_size, (socklen_t*)&len_sock_buf_size );
#ifdef unix
        if(ret == -1)
        {
            throw EXP_ERROR("adapter_InitSock","Error: getsockopt", (DWORD)errno);
        }
#else
        if(ret == SOCKET_ERROR)
        {
            DWORD e = WSAGetLastError();
            throw EXP_ERROR("adapter_InitSock","Error: getsockopt", (DWORD)e);
        }
#endif
        if(sock_buf_size < SOCK_BUF_SIZE)
        {
                sock_buf_size = SOCK_BUF_SIZE;
                ret = setsockopt(sock_src, SOL_SOCKET, SO_RCVBUF,(char *)&sock_buf_size, sizeof(sock_buf_size) );                                               
#ifdef unix
                if(ret == -1)
                {
                    throw EXP_ERROR("adapter_InitSock","Error: setsockopt", (DWORD)errno);
                }
#else
                if(ret == SOCKET_ERROR)

                {   DWORD e = WSAGetLastError();
                    throw EXP_ERROR("adapter_InitSock","Error: setsockopt",(DWORD)e);
                }
#endif
        }

        memset(&addr_in_rem, 0, sizeof(addr_in_rem));
        addr_in_rem.sin_family = AF_INET;
        addr_in_rem.sin_port = htons(PORT_REMOTE);
        addr_in_rem.sin_addr.s_addr = htonl( adr_t.l);

        adapter_InitIcmp();
 return 0;
 }

 //--------------------------------------------------------------------------
 //Вызывается для передачи пакетов UDP на датчик
 // аргументы:
 //              char *data - строка передaваемых данных (с "0" на конце).
 // возвращаемое значение:
 //          - 0 - при отсутствии ошибок
 //          - 1 - при наличии ошибок
 // коментарии:
 //              - функция - обертка для обработок except. генерируемых функцией
 //    void adapter_SendSock(char *data)
 //---------------------------------------------------------------------------
bool adapter_Send(char *data)
 {
    try{
     adapter_SendSock(data);
     }
     catch(EXP_ERROR &e)
     {
        e.ErrorSetText("p_dll_adapter");
        // e.ErrorShow("p_dll_adapter", "adapter_SendSock");
        return 1;
     }
 return 0;
 }

//--------------------------------------------------------------------------
//Вызывается для передачи пакетов UDP на датчик
// аргументы:
//              char *data - строка передaваемых данных (с "0" на конце).
// возвращаемое значение:
//              -нет
// коментарии:
//              - функция - генерирует исключение, если при передачи произошла ошибка
//---------------------------------------------------------------------------
 void adapter_SendSock(char *data)
 {
         int num = strlen(data);
         if(-1 == sendto(sock_src, data, num, 0, (sockaddr*)&addr_in_rem, sizeof(addr_in_rem)))
#ifdef unix
         {
                  throw EXP_ERROR("adapter_SendSock","Error: sendto", (DWORD)errno);
         }
#else
         {
                DWORD e = WSAGetLastError();
                throw EXP_ERROR("adapter_SendSock","Error: sendto", e);
         }
#endif
 }

//------------------------------------------------------------------------
//  http://www.binarytides.com/udp-socket-programming-in-winsock/
// Проверена и работает в Linux
// http://serenity-networks.com/how-to-change-the-tcpip-mtu-on-windows-server-2016/
// команда TOP-вывод инф по использованию ресурсов в Linux
//---------------------------------------------------------------
 //--------------------------------------------------------------------------
 //Вызывается для приема пакетов UDP из датчика
 // аргументы:
 //              BYTE **data - указатель, на указатель буфера приема
 //              DWORD len - длина ожидаемых данных
 // возвращаемое значение:
 //          - при отсутствии ошибок - количество принятых байт
 //          - 0 - при наличии ошибок
 // коментарии:
 //              - функция - обертка для обработок except. генерируемых функцией
 //    DWORD  adapter_ResiveBufferSock(BYTE **ptr, DWORD len);
 //---------------------------------------------------------------------------
DWORD    adapter_ResiveBuffer(BYTE **ptr, DWORD len)
{
    try
    {
        return adapter_ResiveBufferSock(ptr,len);
    }
    catch(EXP_ERROR &e)
    {
      e.ErrorSetText("p_dll_adapter");
      //  e.ErrorShow("p_dll_adapter", "adapter_ResiveBufferSock");
       return 0;
    }

}
//--------------------------------------------------------------------------
//Вызывается для приема пакетов UDP из датчика
// аргументы:
//              BYTE **data - указатель, на указатель буфера приема
//              DWORD len - длина ожидаемых данных
// возвращаемое значение:
//          - при отсутствии ошибок - количество принятых байт
// коментарии:
//              -  в Linux ф. пытается увеличить приоритет своего потока,
//  но обычно это не удается, т.к главное приложение запускается не от SUDO
//              - функция - генерирует исключение, если при приеме произошла ошибка
//              - если ф. работает с датчиком 2048 пикселей и длина принимаемых
//       данных больше одной строки, то все принятые строки, проверяются на
//       наличие ошибок контрольной суммы
//---------------------------------------------------------------------------
DWORD         adapter_ResiveBufferSock(BYTE **ptr, DWORD len)
{
int  ret = 0;
int  er = 0;
BYTE *end = &buffer_rx[0] + len;
BYTE *t_end = &buffer_rx[SIZE_BUFFER_OFF_RX_DATA ];
        memset(buffer_rx,0,sizeof(buffer_rx));
BYTE *t_ptr = &buffer_rx[0];

#ifdef unix
    #ifdef MIT_NICE
    int new_p;
    errno = 0;
       if((new_p =  nice(-20) < 0) && (errno != 0))
       {
           test_dll_Show((char*)" adapter_ResivBuffer", errno, (char*)"cannot set nice",0);
       }
    #endif
#endif
      do{
           ret = recv(sock_src, (char*)t_ptr , t_end - t_ptr, 0);
#ifdef unix
           if(ret <= 0)
           {
                er = errno;
#else
           if((SOCKET_ERROR == ret) || (ret == 0))
           {
                er = WSAGetLastError();
#endif
                throw EXP_ERROR("adapter_ResiveBufferSock","recv",(DWORD)er);
           }
           t_ptr += ret;
           if((len < 2048) && (ret > 0))
             break;
        }while(t_ptr < end);

#ifdef unix
    #ifdef MIT_NICE
        if(errno == 0)
        {
        errno = 0;
            if((new_p =  nice(+20) < 0) && (errno != 0))
            {
            test_dll_Show((char*)" adapter_ResivBuffer", errno, (char*)"ERROR cannot set nice",0);
            }
        }
    #endif
#endif
        if((len > 2048) && (pixel_per_string == 2048))
                adapter_CheckBuferSum((WORD*)&buffer_rx[0], len/2);

        *ptr   = &buffer_rx[0];
    return (t_ptr - ((BYTE*)&buffer_rx[0]));
 }

//--------------------------------------------------------------------------
//      Вызывается при приема пакетов UDP из датчика 2048 пикселей, для подсчета
//контрольной суммы
// аргументы:
//              WORD *ptr - указатель на буфер данных
//              DWORD len - длина буфера данных в пикселях
// возвращаемое значение:
//          - количество строк с ошибками в буфере
// коментарии:
//       фунуция подсчитывает контрольную сумму строки, переданную датчиком и
// сравнивает ее с принятой суммой, передаваеиой в конце строки
// после этого переносит данные этой строки на одно слово к началу буфера, убирая
// контрольную суммы из буфера, т. к. она не должна переноситься в Аpp
//---------------------------------------------------------------------------
int     adapter_CheckBuferSum(WORD* ptr, DWORD ln)
{
        if(pixel_per_string != 2048)
                return 0;
WORD *ch_ptr = (WORD*)(ptr + pixel_per_string + 2);
WORD ch_ret;
long len = ln;
int i = 1;
       do
       {
            ch_ret = adapter_CheckStringSum(ch_ptr + 1, 2048 );
            if(ch_ret != ch_ptr[2049])
            {
                    error_ch++;
            char a[200];
                    memset(a,0,200);
            char b2048[10];
            char r[10];
                    strcpy(a,"ERROR:checksum ch_ptr[2048] = ");
                    sprintf(b2048,"%X",ch_ptr[2048]);
                    sprintf(r,"%X",ch_ret);
                    strcat(a,b2048);
                    strcat(a,", ret= ");
                    strcat(a,r);
                    test_dll_Show((char*)" adapter_ResivBuffer", ch_ret,  (char*)a,0);
            }
            memmove((void*)(ch_ptr - 2 * i ),(void*)(ch_ptr ), (pixel_per_string ) * sizeof(WORD) );
            ch_ptr += (2048 + 2); len -= 2050; i++;
        }while(len > 0);



return error_ch;
}

//--------------------------------------------------------------------------
//      Вызывается при приема пакетов UDP из датчика 2048 пикселей, для подсчета
//контрольной суммы строки
// аргументы:
//              WORD *ptr - указатель на буфер данных
//              DWORD len - длина буфера данных в пикселях
// возвращаемое значение:
//          - WORD - контрольная сумма строки
// коментарии: - подсчет ведется по стандартному алгоритму 16 бит контрольной суммы
//---------------------------------------------------------------------------
WORD adapter_CheckStringSum(WORD *wtr, int len)
{
 WORD ret;
 int i,j;
union
{
unsigned long H_L;
struct
{
 unsigned short int r_L;
 unsigned short int r_H;
}s;
}u;

u.s.r_L = wtr[0];
u.s.r_H = u.s.r_L;
       for(i = 1; i < (len - 1); i++)
       {
        u.s.r_L = wtr[i];
               for(j = 0; j < 16; j++)
               {
                   if((u.s.r_H & 0x8000) == 0x8000)
                       u.s.r_H ^= DELIMER;
                       u.H_L = u.H_L << 1;
               }
       }

       ret = u.s.r_H;
 return ret;
}
//--------------------------------------------------------------------------
//      Вызывается при закрытии приложения и выгрузки dll
// аргументы: - нет
// возвращаемое значение: - нет
// коментарии:
//       закрывает открытые сокеты, в Windows - закрывает Winsock2
//---------------------------------------------------------------------------
 void          adapter_Delete()
 {

#ifndef unix
                closesocket(sock_src);
                closesocket(sock_icmp);
                WSACleanup();
#else
                close(sock_src);
                close(sock_icmp);
#endif
 }


 //--------------------------------------------------------------------------
 //      Возвращает количество ошибок контрольной суммы
 // аргументы: - нет
 // возвращаемое значение:
 //       WORD - количество ошибок контрольной суммы
 // коментарии:
 //       Количество ошибок сбрасывается только инициализацией датчика
 //---------------------------------------------------------------------------
DWORD adapter_GetChError()
{
    return  error_ch;
}

//--------------------------------------------------------------------------
//     Устанавливает для dll параметр - количество пикселей в строке
// аргументы: -
//      WORD - pix  - количество пикселей в строке
// возвращаемое значение:
//       - нет
// коментарии:
//       - нет
//---------------------------------------------------------------------------
void adapter_SetPixelPerString(WORD pix)
{
    pixel_per_string = pix;
}


//--------------------------------------------------------------------------
//          Инициализирует дополнительный UDP сокет в не блокирующем состоянии,
//  для передачи датчику дополнительных запросов для определения его состояния:
//  включен/выключен
// аргументы: -
//      нет
// возвращаемое значение:
//       - нет
// коментарии:
//       - функция генерирует исключения при:
//      -не возможности создать сокет;
//      -не возможности задать его не блокирующее состояние
//---------------------------------------------------------------------------
void  adapter_InitIcmp()
{
    sequence = 0;

    sock_icmp = socket(AF_INET, SOCK_DGRAM, 0);

#ifdef  unix
    if (sock_icmp < 0) {
       throw EXP_ERROR("adapter_InitIcmp","Cannot open icmp socket", (DWORD)errno);
    }
int flags;
    flags = fcntl(sock_icmp, F_GETFL, NULL);
    if(flags < 0)
    {
        throw EXP_ERROR("adapter_InitIcmp","Cannot get non-bloking mode", (DWORD)errno);
    }

    flags |= O_NONBLOCK;

    if(fcntl(sock_icmp, F_SETFL, flags) < 0)
    {
        throw EXP_ERROR("adapter_InitIcmp","Cannot set non-bloking mode", (DWORD)errno);
    }

#else
    if(sock_icmp == INVALID_SOCKET)
    {
       int e = WSAGetLastError();
        throw EXP_ERROR("adapter_InitIcmp","adapter_InitSockICMP-socket",
                        (DWORD)e);
    }
bool arg = 1;
    if (SOCKET_ERROR == ioctlsocket(sock_icmp, FIONBIO, (unsigned long*)&arg ))
    {
    int e = WSAGetLastError ();
    throw EXP_ERROR("adapter_InitIcmp","adapter_InitSockICMP-ioctlsocket", (DWORD)e);
    }

#endif


    memset(&addr_in_icmp, 0, sizeof(addr_in_icmp));
    addr_in_icmp.sin_family = AF_INET;
    addr_in_icmp.sin_port = htons(PORT_REMOTE);
    addr_in_icmp.sin_addr.s_addr = htonl( adr_t.l);

}

//--------------------------------------------------------------------------
//      Вызывается для передачи дополнительных пакетов UDP на датчик, через дополнительный
//  сокет
// аргументы:
//              - нет
// возвращаемое значение:
//          - 0 - датчик выключен
//          - 1 - датчик включен
// коментарии:
//              - функция - обертка для обработок except. генерируемых функцией
//   bool adapter_SendIcmpPacket()
//---------------------------------------------------------------------------
bool adapter_SendIcmp()
{
    try
    {
    return adapter_SendIcmpPacket();
    }
    catch(EXP_ERROR &e)
    {
       e.ErrorSetText("p_dll_adapter");
        return 1;
    }
return 0;
}

//--------------------------------------------------------------------------
//Вызывается для передачи и приема пакетов UDP на датчик через дополнительный сокет
// аргументы:
//              нет
// возвращаемое значение:
//          - 0 - датчик выключен
//          - 1 - датчик включен
// коментарии:
//              - функция пять раз передает запрос MS_GET_VERSION, в Windows каждые 0,3 сек
//       Linux - каждую секунду. Если в течении этого времени, от датчика ответ не пришел
//       считаем датчик выключеным.
//              - функция - транслирует except. генерируеме функцией
//   void adapter_SendIcmpSock(char *data)
//---------------------------------------------------------------------------
bool adapter_SendIcmpPacket()
{
    DWORD len = 0;
    static char *ptr;
    int n_sk = 0;

            string data("MC_GET_VERSION" );

                    do{
                    adapter_SendIcmpSock((char*)data.c_str());
#ifdef unix
                    sleep(1.0);
#else
                    ::Sleep(300);
#endif
                    len +=   (WORD)adapter_ResiveBufferIcmpSock((BYTE**)&ptr, 30);
                    n_sk++;
                    if(n_sk > 5)
                    break;
                    }
                    while(len < 30);

    return (len >= 30);
}


//--------------------------------------------------------------------------
//Вызывается для передачи пакетов UDP на датчик через дополнительный сокет
// аргументы:
//              char *data - строка передaваемых данных (с "0" на конце).
// возвращаемое значение:
//              -нет
// коментарии:
//              - функция - генерирует исключение, если при передачи произошла ошибка
//---------------------------------------------------------------------------
void adapter_SendIcmpSock(char *data)
{
        int num = strlen(data);
        if(-1 == sendto(sock_icmp, data, num, 0, (sockaddr*)&addr_in_icmp, sizeof(addr_in_icmp)))
#ifdef unix
        {
                 throw EXP_ERROR("adapter_SendIcmpSock","Error: sendto_icmp", (DWORD)errno);
        }
#else
        {
               DWORD e = WSAGetLastError();
               throw EXP_ERROR("adapter_SendIcmpSock", "Error: sendto_icmp", e);
        }
#endif
}
//0x591e4f8 ""
//--------------------------------------------------------------------------
//Вызывается для приема пакетов UDP из датчика через дополнительный сокет
// аргументы:
//              BYTE **data - указатель, на указатель буфера приема
//              DWORD len - длина ожидаемых данных
// возвращаемое значение:
//          - DWORD - при отсутствии ошибок - количество принятых байт
//          - 0 - при наличии ошибок
// коментарии: нет
//---------------------------------------------------------------------------
DWORD         adapter_ResiveBufferIcmpSock(BYTE **ptr, DWORD /*len*/)
{
int  ret = 0;
BYTE buffer_icmp[100];

BYTE *t_end = &buffer_icmp[100];
        memset(buffer_icmp,0,sizeof(buffer_icmp));
BYTE *t_ptr = &buffer_icmp[0];

           ret = recv(sock_icmp, (char*)t_ptr , t_end - t_ptr, 0);
#ifdef unix
           if(ret <= 0)
           {
               return 0;
#else
           if((SOCKET_ERROR == ret) || (ret == 0))
           {
               return 0;
#endif
           }
        *ptr   = &buffer_rx[0];
    return (ret);
 }
//--------------------------------------------------------------------------
//          DWORD adapter_GetError возвращает ошибки, фиксируемые исключительными ситуациями
// аргументы:
//         нет
// возвращаемое значение:
//         1 - наличие ошибки
//         0 - ошибок не было
// коментарии:
//         нет
//---------------------------------------------------------------------------
DWORD adapter_GetError(char **text)
{
    DWORD ret;
    if((ret = EXP_ERROR::ErrorGet()))
        *text = (char*)EXP_ERROR::GetTextError()->c_str();
    else
        *text = NULL;
    return ret;
}
#ifndef unix
//--------------------------------------------------------------------------------------------
//      Функция принимает указатель на буфер строки с ANSIE - символами и возвращает
//  буфер строки с UNICODE-символами
//      аргументы: - LPCSTR - pszA указатель на буфер с ANSI - символами
//                 - wchar_t* - ppszW указатель на выходной буфер, который был выделен заранее.
//  возвращаемое значение:
//              DWORD  0 - при нормальном завершении
//                     1 - при если указатель на входной буфер равен NULL
//              DWORD dwError - если произошла ошибка при преобразовании функции
//  коментарии: - функция генерирует исключения если:
//          -указатель на входной буфкр равен нолю
//          -если не может выделить файл, под выходной буфер
//------------------------------------------------------------------------------------------------
DWORD  AnsiToUnicode(LPCSTR pszA, wchar_t* ppszW)
{
    ULONG cCharacters;
    DWORD dwError;
    if (NULL == pszA)
    {
        *ppszW = NULL;
        return 1;
    }
    cCharacters =  strlen(pszA)+1;
    if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,
                  (LPWCH)ppszW, cCharacters))
    {
        dwError = GetLastError();
        *ppszW = NULL;
        return dwError;
    }
    return 0;
}

#endif

/*
//-----------------------------------------------
 void MessegeOfBufer(WORD *ptr, DWORD len)
 {

  char buf[2048];
        memset(buf, 0, 2048);
        for(DWORD i = 0; i < len; i++)
        {
            char b[20];
                memset(b, 0, 20);
                sprintf(b,"%d",ptr[i]);
                strcat(buf,b);
                strcat(buf,";");
                if(((i + 1)%8 == 0) && (i != 0))
                    strcat(buf,"\r\n");
        }
wchar_t w_buf[2048];
      wmemset(w_buf, 0, 2048 );
      AnsiToUnicode(buf,w_buf);
      MessageBox(NULL,w_buf,TEXT("TEST"), MB_OK);
 }
*/
