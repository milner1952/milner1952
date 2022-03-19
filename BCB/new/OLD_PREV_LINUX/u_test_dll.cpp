//---------------------------------------------------------------------------

#ifndef __BORLANDC__
	#define _CRT_SECURE_NO_WARNINGS
	#ifndef _CRT_SECURE_NO_DEPRECATE
		# define _CRT_SECURE_NO_DEPRECATE (1)
	#endif 
    # pragma warning(disable : 4996)
#endif

#include "stdafx.h"
#include <windows.h>
#include <ctime>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "test_dll.h"



COORD c;
HANDLE hnd;
WORD lkl = 0;
static unsigned int iz_log_debug;
char m_name[1024];


const BYTE b_ASC2[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
HANDLE hVisbl;


#ifndef __BORLANDC__
	#include <stdlib.h>
    #include <string>
	using namespace std;

	typedef string AnsiString;
//---------------------------------------------------------------------------
	BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
	{
		return TRUE;
	}
#else
	#include <vcl.h>
	#pragma hdrstop
	#pragma argsused
//---------------------------------------------------------------------------
	int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
	{
			return 1;
	}
#endif

//-------------------------------------------------------------------
int ByteAsASC(byte *dst,byte *src,int n_src)
{
byte bl,bh,b;
int i=0;
int j=0;
        for(;i < n_src;i++)
        {
                b = src[i];
                bl= b_ASC2[0x0f&b];
                        b = b>>4;
                bh = b_ASC2[0x0f&b];
                dst[j++] = bh;
                dst[j++] = bl;
        }
return j;
}


//---------------------------------------------------------------
void test_dll_Init(char *m_n, unsigned int iz_debug)
{
        iz_log_debug = iz_debug;

        if(iz_log_debug == 0)
        return;

        hVisbl  = CreateEvent( NULL, true, false, NULL);
        if(hVisbl == NULL)
        return;
        SetEvent(hVisbl);
        memset(m_name ,0 ,1024);
        strcpy(m_name, m_n);

        if(iz_log_debug == 2)
        {
                AllocConsole();
                hnd =GetStdHandle(STD_ERROR_HANDLE	);
                if(hnd == INVALID_HANDLE_VALUE)
                        hnd = hnd;
                SetConsoleActiveScreenBuffer(hnd);
        }
        test_dll_OutputDebugString(m_name);
}
//-------------------------------------
void test_dll_OutputDebugString(char *ptr, bool iz_time )
{
char *ctr;



                tm *tim;
                time_t t = 0L;
                time(&t);
                tim = localtime(&t);
                ctr = asctime(tim);


        if(iz_log_debug == 1)   // Отладка под Борландом
        {
                if(iz_time == 0)
						#ifndef __BORLANDC__
							OutputDebugString((LPCWSTR)ptr);
						#else
							OutputDebugString(ptr);
						#endif
                else                    // Отладка под Борландом с указанием времени
                {
                char b[1000];
                        memset(b,0,1000);
                        strcpy(b,ptr);
                        strcpy((char*)(b + strlen(ptr)),"  ");
                        strcpy((char*)(b + strlen(b)),ctr);
						#ifndef __BORLANDC__
							OutputDebugString((LPCWSTR)ptr);
						#else
							OutputDebugString(ptr);
						#endif
                }
        }

        if(iz_log_debug == 2)   // Отладка с консолью
        {
        DWORD num;
                c.X = 1;
                SetConsoleCursorPosition(hnd,c);
                WriteConsole(hnd,ptr,(DWORD)strlen(ptr),&num,NULL);
                if(iz_time == 1)    // Отладка с консолью с указанием времени
                {
                        WriteConsole(hnd,"   ",3,&num,NULL);
						WriteConsole(hnd, ctr, (DWORD)strlen(ctr), &num, NULL);
                }
                c.X = 1;
                c.Y++;
        }
}

//-------------------------------------------------------------------------
void test_dll_IncrementYPositrion()
{
        c.X = 1;
        ++c.Y;
        SetConsoleCursorPosition(hnd,c);
}

//-----------------------------------
void test_dll_IzClrConsol()
{
        if(iz_log_debug != 2)
        return;
        lkl++;
        if(lkl > 9)
        {
        COORD cr; cr.X = 0;cr.Y = 0;
                lkl = 0;
                system("cls");
                c.Y = 0; c.X = 0;
        }
}

//---------------------------------------------------------------------------
 //1     //(char* , WORD)
bool test_SetStringTooOutDebug1(char* st , DWORD w)
{
//Sleep(T_SLP);
if(iz_log_debug == 0)
return 1;
        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);
        AnsiString ain(st);


#ifdef __BORLANDC__
        ain += w;
#else
		char buf[10];
			memset(buf, 0, 10);
			itoa(w, buf, 10);
			ain += buf;
#endif
        test_dll_OutputDebugString((char*)ain.c_str(),1);
        SetEvent(hVisbl);
return 0;
}

//---------------------------------------------------------------------------
//2    //(char* , DWORD, char*)
bool test_SetStringTooOutDebug2(char* st, DWORD dw, char* st2)
{

if(iz_log_debug == 0)
return 1;
        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);

        AnsiString ain(st);
#ifdef __BORLANDC__
		ain += dw;
#else
		{
			char b[100]; memset(b, 0, 100);
			sprintf(b, "%s", dw);
			ain += b;
		}
#endif
        ain += st2;
        test_dll_OutputDebugString((char*)ain.c_str());
        SetEvent(hVisbl);
return 0;
}

//---------------------------------------------------------------------------
//3       //(char* , unsigned char*)
bool test_SetStringTooOutDebug3(char* st, unsigned char* bt)
{
//Sleep(T_SLP);
if(iz_log_debug == 0)
return 1;
        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);

        AnsiString ain(st);
char t[20];
        memset(t,0,20);
        sprintf(t,"%X",(int)bt);
        ain += t;
        test_dll_OutputDebugString((char*)ain.c_str());
        SetEvent(hVisbl);
return 0;
}



//---------------------------------------------------------------------------
//4     //(char* , char**)
bool test_SetStringTooOutDebug4(char* st, unsigned char** st2)
{
//Sleep(T_SLP);
if(iz_log_debug == 0)
return 1;
        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);

        AnsiString  ain(st);
char t[20];
        memset(t,0,20);
        sprintf(t,"%X",(int)*st2);
        ain += t;
        test_dll_OutputDebugString((char*)ain.c_str());
        SetEvent(hVisbl);
return 0;
}

//---------------------------------------------------------------------------
 //5     //(char* , WORD)
bool test_SetStringTooOutDebug5(char* st , double d)
{
if(iz_log_debug == 0)
return 1;
        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);

        AnsiString ain(st);
#ifdef __BORLANDC__
        ain += d;
#else

		{
			char b[100]; memset(b, 0, 100);
			sprintf(b, "%s", d);
			ain += b;
		}
#endif

        test_dll_OutputDebugString((char*)ain.c_str(),1);
SetEvent(hVisbl);
return 0;
}

//---------------------------------------------------------------------------
//6       //(char* , char*)
bool test_SetStringTooOutDebug6(char* st,  char* bt)
{
if(iz_log_debug == 0)
return 1;

        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);
        AnsiString ain(st);
        ain += bt;
        test_dll_OutputDebugString((char*)ain.c_str());
        SetEvent(hVisbl);

return 0;
}
//-------------------------------------------------------------------------
// 7
bool test_SetStringTooOutDebug7(char* st, DWORD dw, char* st2, DWORD dw2)
{
if(iz_log_debug == 0)
return 1;
        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);

        AnsiString ain(st);
        AnsiString adw = (const char*)dw;
        ain += adw;
        ain += "  ";
        ain += st2;
        AnsiString adw2 = (const char*)dw2;
        ain += adw2;

        test_dll_OutputDebugString((char*)ain.c_str());
        SetEvent(hVisbl);
return 0;

}
//-----------------------------------------------------------------------------
//8
bool test_SetStringTooOutDebug8(char* sd, BYTE *d, char* ss, BYTE *s,
                                WORD num_d_s, char *sm, BYTE *m, WORD num_m)
{
if(iz_log_debug == 0)
return 1;
        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);
char bd[30];
char bs[30];
char bm[30];
        memset(bd,0,30);
        memset(bs,0,30);
        memset(bm,0,30);

        ByteAsASC((unsigned char*)bd, d, num_d_s);
        ByteAsASC((unsigned char*)bs, s, num_d_s);
        ByteAsASC((unsigned char*)bm, m, num_m);

        AnsiString ain(sd);
        ain += bd; ain += " ";
        ain += ss;
        ain += bs; ain += " ";
        ain += sm;
        ain += bm;
        test_dll_OutputDebugString((char*)ain.c_str());
        SetEvent(hVisbl);

return 0;
}

//---------------------------------------------------------------------------
//9       //(char* , unsigned char*. num)
bool test_SetStringTooOutDebug9(char* st, unsigned short int* bt, WORD num)
{
if(iz_log_debug == 0)
return 1;

        WaitForSingleObject(  hVisbl,  INFINITE);
        ResetEvent(hVisbl);
AnsiString ain(st);
AnsiString a_t;
int i;
char t[10];
        for(i = 0; i < (num - 1); i++)
        {
                memset(t,0,10);
                sprintf(t,"%x",bt[i]);
                a_t += t;
                a_t += ",";
        }

        memset(t,0,10);
        sprintf(t,"%x",bt[i]);
        a_t += t;
        ain += a_t;

        test_dll_OutputDebugString((char*)ain.c_str());
        SetEvent(hVisbl);
return 0;
}




