//---------------------------------------------------------------------------
#include <ctime>
#include <string>

#include <stdio.h> 
#include <errno.h>


#include "dll_src.h"
#include "svaz.h"

#include "file.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>


#include <string>
using namespace std;

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../exp_error/exp_error.h"

#ifdef unix
#include <unistd.h>
#endif

#include "../../SRC/BCB/RES_UNIX/res_unix.h"
#ifndef unix
#include <Windows.h>
#include <windowsx.h>
    #pragma warning( disable : 4138)
#endif
#include "../test/test_dll.h"
#include "../reg/reg.h"
#include "../reduct/Reduction_class.h"
#include "../reduct/Reduction.h"
#include "../adapter/adapter.h"

#ifndef unix
    DWORD  UnicodeToAnsi(TCHAR *pszW, LPSTR *ppszA);
#endif

extern  TEST_DLL_SHOW_S_S g_test_dll_Show_s_s;
extern DWORD dw_noise_bufer[9];
V_NOISE_DATA v_statistic_data;
extern TEST_DLL_SHOW_S_DW   g_test_dll_Show_s_dw;
extern REGDATA *regdata;
//---------------------------------------------------------------------------------------------
void file_SaveIn_statistic_log(LPCTSTR name, DWORD *dw_in, DWORD *dw_out, bool flag_ret)
{
                            if(flag_ret == 0)
                            return;
#ifndef unix
HANDLE h_file;

                             h_file =   FileCreate(name, GENERIC_WRITE );
                             if(h_file == NULL)
                             {
                                 char chName[MAX_PATH];
                                     if(UnicodeToAnsi((TCHAR*)&name, (LPSTR*)&chName))
                                         throw EXP_ERROR("can not copen file and convert name", (const char*)chName);
                                     else
                                         throw EXP_ERROR("can not copen file", (const char*)chName);
                                 return;
                              }
#else
int h_file;
                             h_file = open(name, O_WRONLY | O_CREAT, 0777);
                             if(h_file < 0)
                             {
                                close((int)h_file);                               
                                throw EXP_ERROR("can not copen file", (const char*)name);
                                return ;
                             }
#endif

string string_file;
char st[100];
const time_t t  =           time(NULL);
                            memset(st,0,sizeof(st));
                            strcpy(st,ctime(&t));
                            string_file += "\r\n";
                            string_file +=  st;

int len         =           string_file.size();

                            if((*(string_file.c_str() + len -1)== 0x0d)  ||
                                 (*(string_file.c_str() + len -1)== 0x0a))
                                 string_file.erase(len);

                            string_file += "\r\n";
                            p_dll_src_SetContainerOfNoise(&v_statistic_data, dw_in, dw_out, flag_ret);

I_NOISE_DATA b = v_statistic_data.begin();
I_NOISE_DATA e = (v_statistic_data.end() - 1);

int delta_in[9];
int delta_out[9];

int num = 9;

           if(regdata->pixsels_per_string == 2048)
               num = 8;

        for(int i = 0; i < num; i++)
        {
                delta_in[i] =   (*e).d_in[i] - (*b).d_in[i];
                delta_out[i] =  (*e).d_out[i] - (*b).d_out[i];
        }

string  at;
                SAVESTATISTIC("IN:",dw_in, delta_in);
                SAVESTATISTIC("OUT:",dw_out, delta_out);

                string_file += "\r\n\r\n";

                if(FileSeek((HANDLE)h_file, 0, 2))
                    return;
                FileWrite((HANDLE)h_file, string_file.c_str(),  string_file.size());
                //в люблм сл. надо закрывать
                FileClose((HANDLE)h_file);

}


//-----------------------------------------------------------------------
string file_Save_statistic(DWORD *dw_noise)//1
{
string noise_str;
char b[1000];
                memset(b, 0, 1000);
           if(regdata->pixsels_per_string == 2048)
                sprintf(b,"%05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t",
                                 dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                 dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7]);
           else
                sprintf(b,"%05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7], dw_noise[8]);
                noise_str = b;
                noise_str += "\t";
return noise_str;
}




//-----------------------------------------------------------------------
string file_Save_statistic(int *dw_noise)//2
{
string noise_str;
char b[1000];
                memset(b, 0, 1000);
           if(regdata->pixsels_per_string == 2048)
                sprintf(b, "%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                 dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                 dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7]);
               else
                sprintf(b, "%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7], dw_noise[8]);
                noise_str = b;
return noise_str;
}


//-----------------------------------------------------------------------
string file_Save_statistic(char *str)//3
{
string string_file(str);
return string_file;
}

//------------------------------------------------------------------------
#ifndef unix
HANDLE   FileCreate(LPCTSTR FileName, int )
{
    HANDLE hFile;
    DWORD e;

    hFile = CreateFile(FileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        if((e = GetLastError()) == ERROR_FILE_NOT_FOUND)
        {
            hFile = CreateFile(FileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
                                     CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
            if(hFile == INVALID_HANDLE_VALUE)
            {
            char *chName[MAX_PATH];
                if(UnicodeToAnsi((TCHAR*)&FileName, chName))
                    throw EXP_ERROR("can not create file and convert name", (const char*)chName);
                else
                    throw EXP_ERROR("can not create file", (const char*)chName);
                return NULL;
            }
        }
        else
        {
            char *chName[MAX_PATH];
                if(UnicodeToAnsi((TCHAR*)&FileName, chName))
                    throw EXP_ERROR("No open file: and convert name", (const char*)chName);
                else
                    throw EXP_ERROR("No open file:", (const char*)chName);
            return NULL;
        }
    }
    return hFile;
}
#else
HANDLE  FileCreate(const char* FileName, int Mode)
{
    int hFile;

            hFile =  open(FileName, Mode | O_CREAT, 0777);
            if (hFile < 0)
            {
               // if(g_test_dll_Show_s_s((char*)"\r\nNo open file: ",(char*)FileName,0))
               throw EXP_ERROR("No open file:", (const char*)FileName);
                    return 0;
            }
    return (HANDLE)hFile;
}
#endif

//---------------------------------------------------------------------------
#ifndef unix
int   FileSeek(HANDLE Handle, int Offset, int Origin)
{
    DWORD e;
            if(((DWORD)-1) == SetFilePointer(Handle, (long)Offset, NULL, Origin))
            {
                e = GetLastError();
                throw EXP_ERROR("FileSeek","can not file seek to origin" ,e);
                return 1;

            }
        return 0;
}
#else
int   FileSeek(HANDLE Handle, int Offset, int Origin)
{

            if(lseek((int)Handle, Offset, Origin) < 0)
            {
                throw EXP_ERROR("FileSeek","can not file seek to origin", (WORD)errno);
                return 1;

            }

        return 0;
}
#endif

#ifndef unix
DWORD  FileWrite(HANDLE Handle, const void *Buffer, DWORD Count)
    {
    DWORD num;
    DWORD e;
        if(!WriteFile( Handle,(LPCVOID)Buffer, (DWORD) Count, (LPDWORD)&num, NULL))
        {
                e = GetLastError();
                throw EXP_ERROR("WriteFile", "can not write file", e);
                return 0;
        }

        return num;
}
#else
DWORD  FileWrite(HANDLE Handle, const void *Buffer, DWORD Count)
    {
    DWORD num;

        num =  write( (int)Handle,Buffer,  Count);
        if(num != Count)
        {
                throw EXP_ERROR("WriteFile", "can not write file",(WORD)errno);
                return 0;
        }

        return num;
}
#endif

#ifndef unix
void FileClose(HANDLE Handle)
{
    DWORD e;
        if(!CloseHandle(Handle))
        {
                e = GetLastError();
                throw EXP_ERROR("FileClose","can not close file", e);
        }
}
#else
void  FileClose(HANDLE Handle)
{

        if(-1 == close((int)Handle))
        {

           throw EXP_ERROR("FileClose", "can not close file" ,(WORD)errno);
        }
}
#endif


