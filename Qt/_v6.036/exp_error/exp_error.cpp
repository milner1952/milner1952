#include "exp_error.h"
 void EXP_ERROR::ErrorSetText(const char* lib)
 {
   text_out = lib;
   text_out  += "\r\n";
   text_out += info;
 }
/*
//------------------------------------------------------------------------------
#ifdef unix
void EXP_ERROR::ErrorShow(const char* lib, const char* mes)
{
    char e[1000];
        memset(e, 0, 1000);
        if(dw_valid == true)
        {
        char *ptr_e;
                if((ptr_e = dlerror()) != NULL)
                {
                    strcpy(e, ptr_e);
                }
        }
        strcat(e, lib);
        strcat(e,", ");

        if(mes != NULL)
        {
            strcat(e, mes);        
            strcat(e,", ");
        }
        strcat(e , info.c_str());


     char buf[10];
        if(ret_valid == true)
        {
             strcat(e, ": ");
             memset(buf, 0, 10);
        fprintf(stderr,"%s %d \r\n ",e,ret);
        }
        else
        fprintf(stderr,"%s ",e);
}

#else
void EXP_ERROR::ErrorShow(const char* lpszLib, const char* mes)
{

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

            if(dw_valid == true)
            {
                FormatMessage(
                                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                                FORMAT_MESSAGE_FROM_SYSTEM |
                                FORMAT_MESSAGE_IGNORE_INSERTS,
                                NULL,
                                dw,
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                (LPTSTR) &lpMsgBuf,
                                0, NULL );
            }

wchar_t wInfo[1000];

            if(-1 == mbstowcs(wInfo, info.c_str(), 1000))
             {
                    wcscpy(wInfo,TEXT("Error konvect name"));
             }

wchar_t wLib[100];
            if(lpszLib != NULL)
            {
                if(-1 == mbstowcs(wLib, lpszLib, 100))
                {
                 wcscpy(wLib,TEXT("Error konvect lib"));
                }
            }

wchar_t wbuf[10];
            if(ret_valid == true)
            {
                wmemset(wbuf,0,10);
                swprintf(wbuf,5,L"%ld",ret);
            }



wchar_t wMes[1000];
            if(mes != NULL)
            {
                if(-1 == mbstowcs(wMes, mes, 1000))
                {
                    wcscpy(wMes,TEXT("Error konvect mes"));
                }
            }

           lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                    (dw_valid == true ? lstrlen((LPCTSTR)lpMsgBuf) : 0) +
                                    lstrlen( (LPCTSTR)wLib) +
                                    lstrlen( (LPCTSTR)wInfo) * sizeof(TCHAR) +
                                     (ret_valid == 1 ?  lstrlen((LPCTSTR)wbuf) *   sizeof(TCHAR) : 0) +
                                     (mes  !=   NULL ?  lstrlen((LPCTSTR)wMes) *   sizeof(TCHAR) : 0) +
                                     40 * sizeof(TCHAR));
           if(dw_valid == true)
                wcscpy((wchar_t*)lpDisplayBuf,(wchar_t*)lpMsgBuf);

           if(lpszLib != NULL)
           {
                wcscat((wchar_t*)lpDisplayBuf, (wchar_t*)wLib);
                wcscat((wchar_t*)lpDisplayBuf, TEXT(": "));
           }

           if(mes != NULL)
                wcscat((wchar_t*)lpDisplayBuf, (wchar_t*)wMes);

           wcscat((wchar_t*)lpDisplayBuf,TEXT(": "));

           if(mes != NULL)
                wcscat((wchar_t*)lpDisplayBuf, (wchar_t*)wInfo);

           if(ret_valid == true)
           {
                wcscat((wchar_t*)lpDisplayBuf, TEXT(": "));
                wcscat((wchar_t*)lpDisplayBuf, (wchar_t*)wbuf);
           }

wchar_t wMess[1000];
           if(-1 == mbstowcs(wMess, mes , 1000))
           {
                 wcscpy(wLib,TEXT("Error konvect info"));
           }

           MessageBox(NULL, (LPCWSTR)lpDisplayBuf, (LPCWSTR)wMess, MB_OK);
if(dw_valid == true)
           LocalFree(lpMsgBuf);
           LocalFree(lpDisplayBuf);
}

#endif
*/

