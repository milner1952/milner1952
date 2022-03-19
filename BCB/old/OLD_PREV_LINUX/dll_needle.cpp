//---------------------------------------------------------------------------
#include <windows.h>
#include "dll_needle.h"
#include "test_dll.h"
#include "type_app.h"

#pragma hdrstop
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
BOOL CALLBACK dll_needle_EnmPrc(HWND hwnd,LPARAM p);
BOOL CALLBACK dll_needle_EnmChildPrc(HWND hwnd, LPARAM p);
BOOL dll_needle_SetHook(HWND hwnd);
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK HookWndProc(int nCode, WPARAM wParam, LPARAM lParam);
DWORD WINAPI dll_needle_StartEnum(void*);
DWORD WINAPI dll_needle_StartChildEnum(void*);

DWORD error;
WNDENUMPROC lpEtr = (WNDENUMPROC)dll_needle_EnmPrc;

char* ch_res_name[4] = {"ВРК-Программа контроля аппарата", "Полупроводниковая камера","ПРОГРАММА ТЕСТИРОВАНИЯ  ТЛД-6", "  "};
char* ch_menu_name[4] = {"Отнормировать кадр\t", "Отнормировать", "Отнормировать", " "};

AnsiString as_res_name;
AnsiString as_menu_name;


HHOOK hHook_k = NULL;
HHOOK hHook_wnd = NULL;

HINSTANCE hinst_needle_dll = NULL;
HANDLE hThStartEnum;
DWORD idThStartEnum;
bool flag_redy = 0;

MAIN_APP flagMainApp;
V_NOISE_DATA *v_statistic_data;
V_NOISE_DATA *v_noise_data;


//---------------------------------------------------------------------------
#ifndef __BORLANDC__

#include <winuser.h>
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

//---------------------------------------------------------------------------
void dll_needle_Init(HINSTANCE h, V_NOISE_DATA * s, V_NOISE_DATA *n ,MAIN_APP ma)
{
	    hinst_needle_dll = h;
        hHook_k = NULL;
        hHook_wnd = NULL;
        v_statistic_data = s;
        v_noise_data = n;
        flagMainApp = ma;
        as_res_name = ch_res_name[(int)ma];
        as_menu_name = ch_menu_name[(int)ma];
static int i = 0;
        if(i == 1)
                return;
        i = 1;

        flag_redy  = false;
        dll_needle_StartEnum(NULL);
        return;
}

//------------------------------------------------------------------------------
void dll_needle_Init( V_NOISE_DATA * s, V_NOISE_DATA *n ,MAIN_APP ma)
{
        hHook_k = NULL;
        hHook_wnd = NULL;
        v_statistic_data = s;
        v_noise_data = n;
        flagMainApp = ma;
        as_res_name = ch_res_name[(int)ma];
        as_menu_name = ch_menu_name[(int)ma];
static int i = 0;
        if(i == 1)
                return;
        i = 1;

        flag_redy  = false;
        dll_needle_StartEnum(NULL);
        return;
}

//-------------------------------------------------------------------
DWORD WINAPI dll_needle_StartEnum(void*)
{
int i;
i = 0;
        error = 0L;
                if(!EnumWindows(lpEtr,i++))
                {
                        error = GetLastError();
                        test_SetStringTooOutDebug1("ERROR: In dll_needle_StartEnum", error);
                }
        return (DWORD)i;
}


//-----------------------------------------------------------------------------
BOOL CALLBACK dll_needle_EnmPrc(HWND hwnd, LPARAM p)
{
char w_name[1000];
memset(w_name,0,1000);
        GetWindowText(hwnd,w_name,1000);
#ifdef __BORLANDC__
        if(!as_res_name.AnsiCompare(w_name))
#else
        if(!as_res_name.compare(w_name))
#endif
        {
                return dll_needle_SetHook(hwnd);
        }
        return true;
}


//--------------------------------------------------------------------------
BOOL dll_needle_SetHook(HWND hwnd)
{
DWORD dwId = GetCurrentThreadId();
        hHook_k = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, hinst_needle_dll, dwId);
        if(hHook_k == NULL)
        {
                error = GetLastError();
                test_SetStringTooOutDebug1("ERROR: In dll_needle_SetHook_M", error);
                return false;
        }

        hHook_wnd = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)HookWndProc, hinst_needle_dll, dwId);
        if(hHook_wnd == NULL)
        {
                error = GetLastError();
                test_SetStringTooOutDebug1("ERROR: In dll_needle_SetHook_M", error);
                return false;
        }
        return true;
}


BOOL Prc(HWND hwnd, LPARAM w);
#define ALT_DOUN 0x20000000
int Y = 50;
int X = 10;
//---------------------------------------------------------------------
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
       if (nCode < 0)
                return CallNextHookEx(hHook_k, nCode,wParam, lParam);

                if((lParam & ALT_DOUN) == ALT_DOUN)
                {
                        flag_redy = true;
                        test_SetStringTooOutDebug1("OK: HookWndProc: Hook_k ", (DWORD)hHook_k);
                        test_SetStringTooOutDebug1("OK: HookWndProc: Hook_wnd: ", (DWORD)hHook_wnd);

                }

        return CallNextHookEx(hHook_k, nCode, wParam, lParam);
}

//----------------------------------------------------------------------
LRESULT CALLBACK HookWndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
 MSG *msg;
 char w_name[1000];
 AnsiString as;

int wId;
        if (nCode < 0)
                return CallNextHookEx(hHook_wnd, nCode,wParam, lParam);

         if((flag_redy == true))
         {
           msg = (MSG*)lParam;
           if(msg->message == WM_COMMAND)
            {
                if((HIWORD(msg->wParam)  == 0) && (msg->lParam == 0))
                {
								memset(w_name, 0, 1000);
								wId = LOWORD(msg->wParam);

                        HMENU hstr_menu = GetMenu(msg->hwnd);
                        UINT poz = GetMenuItemCount(hstr_menu);

                                if(poz == -1)
                                {
                                        error = GetLastError();
                                        test_SetStringTooOutDebug1("ERROR: HookWndProc: GetMenuItemCount ", error);
                                }

                        HMENU hmnsb =      GetSubMenu(hstr_menu, 0);
                                if(hmnsb == NULL)
                                {
                                        error = -1;
                                        test_SetStringTooOutDebug1("ERROR: HookWndProc: GetSubMenu ", error);
                                }
                                if(!GetMenuString(hmnsb, wId , w_name, 1000, MF_BYCOMMAND))
                                {
                                        error = -1;
                                        test_SetStringTooOutDebug1("ERROR: HookWndProc: GetMenuString ", error);
                                }
#ifdef __BORLANDC__
                                w_name[as_menu_name.Length()] = 0;
                                if(!as_menu_name.AnsiCompare(w_name))
#else
                                w_name[as_menu_name.size()] = 0;
                                if(!as_menu_name.compare(w_name))
#endif
                                {

                                        {
                                                v_statistic_data->erase(v_statistic_data->begin(), v_statistic_data->end() - 1);
                                                test_SetStringTooOutDebug1("OK: v_statistic_dataerase(... ", v_statistic_data->size());
                                        }
                                       error = 0;
                                }
                }
            }

         }
         return CallNextHookEx(hHook_wnd, nCode, wParam, lParam);
}

//----------------------------------------
void dll_needle_Close()
{

error = 0L;
        if(hHook_wnd == NULL)
                return;
        if(hHook_k == NULL)
                return;

        ::Sleep(200);
        if(!UnhookWindowsHookEx( hHook_wnd))
        {
                error = GetLastError();
                test_SetStringTooOutDebug1("ERROR UnhookWindowsHookEx Hook_wnd: ", (DWORD)hHook_wnd);

        }
        ::Sleep(200);
        if(!UnhookWindowsHookEx( hHook_k))
        {
                error = GetLastError();
                test_SetStringTooOutDebug1("ERROR UnhookWindowsHookEx Hook_k: ", (DWORD)hHook_k);

        }
        if(error |= 0L)
        {
        test_SetStringTooOutDebug1("ERROR: dll_needle_Close:", error);
#ifdef __BORLANDC__
        AnsiString e(error);
				MessageBox(NULL, e.c_str(), "ERROR: dll_needle_Close:", MB_OK);
#else
		char buf[10];
				memset(buf, 0, 10);
				ltoa(error,buf,10);
				MessageBox(NULL, buf, "ERROR: dll_needle_Close:", MB_OK);
#endif
				
        return;
        }
        else
				test_SetStringTooOutDebug1("OK: UnHookWndProc:", 0);
                ::Sleep(2000);
}










