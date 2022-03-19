//---------------------------------------------------------------------------
//

#ifndef unix
#pragma hdrstop
#ifndef __BORLANDC__
    #define _CRT_SECURE_NO_WARNINGS
    #ifndef _CRT_SECURE_NO_DEPRECATE
        # define _CRT_SECURE_NO_DEPRECATE (1)
    #endif
    #pragma warning(disable : 4996)
    #pragma warning(disable: 4800)
#endif
#endif

#ifdef unix
    #include <QtWidgets>
    #include <fcntl.h>
    #include <unistd.h>

#endif//UNIX

#include "reg.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




using namespace std;
static TRegistry *reg;

static char *RegKey ;

char **   reg_str_param_name;
static int num_param;


REGSTRING *regstring;
const char **defstring;


//---------------------------------------------------------------------------
TRegistry* InitReg(const char *Key,const char **Str_param_name, int num, const char **df, const char* f_name)
{
    defstring = df;
#ifdef __BORLANDC__
	try
	{
		reg = new  TRegistry(KEY_ALL_ACCESS);
	}
	catch(bad_alloc)
	{
		OutputDebugString("Error - new reg in InitReg()");
	}
	catch(...)
	{
		OutputDebugString("GlbError - new reg in InitReg()");
	}
	
#else
    reg = new TRegistry((char*)f_name);
#endif
  reg->RootKey = HKEY_CURRENT_USER;
  RegKey =  (char*)Key;

  reg_str_param_name = (char**)Str_param_name;
  num_param = num;

  regstring = NULL;
  regstring = new REGSTRING[num];
 if(reg == NULL)
	return NULL;
 else
	return reg;
}
//------------------------------------------------------
void DestroyReg()
{
 if(regstring != NULL)

     delete[] regstring;
 delete reg;
}


//---------------------------------------------------------------------------
REGSTRING* ReadParamTooReg()
{
int i;

const AnsiString  ch(RegKey);
if(!reg->OpenKey(ch,false))
    {
        reg->OpenKey(ch,true);
        for(i = 0;i < num_param; i++)
        {

        reg->WriteString(reg_str_param_name[i],defstring[i]);
        }

    }

    for(i = 0;i < num_param; i++)
    {
    AnsiString as = reg->ReadString(reg_str_param_name[i]);
        regstring[i] = as;
    }
     reg->CloseKey();
return regstring;
}

//---------------------------------------------------------------------------
void WriteParamTooReg(const char* Param, char *Value)
{
    reg->OpenKey(RegKey,true);
    reg->DeleteValue(Param);
    reg->WriteString(Param,Value);
    reg->CloseKey();
}

//-------------------------------------------------------------------------------------------
#ifndef __BORLANDC__

TRegistry::TRegistry(char *f_name){
#ifdef unix
    buffer = NULL;
    hFile = (HKEY)open(f_name, O_RDWR|O_CREAT|O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
            if(hFile < (HKEY)0)
            {
            fprintf(stderr,"Cannot open file s_dll_udp_init_ex\n");
            exit(1);
            }
#endif
}
#endif
//------------------------------------------------------------------------------------------------------------
#ifdef unix
TRegistry::~TRegistry(){
    if(hFile != 0L)
            close((int)hFile);
    if(buffer != NULL)
            delete[] buffer;

}

//-------------------------------------------------------------------------------------------------------------
 long  TRegistry::RegCreateKeyEx(HKEY /*RootKey*/, LPCTSTR lpSubKey, DWORD /*Reserved*/, LPTSTR /*lpClass*/, DWORD /*dwOpt*/,
                                 REGSAM smDes,     LPSECURITY_ATTRIBUTES /*lpSca*/, PHKEY /*phkResult*/, LPDWORD /*lpdwDisp*/)
 {
            off_t sz_file;
                    lseek((int)hFile, 0,SEEK_SET);
                    if((sz_file = lseek((int)hFile, 0,SEEK_END)) <= 0)
                    {
                            if(smDes == 0)
                                return 1L;
                    }
            ssize_t  wr_z;
            string s;

                    if(sz_file <= 0)
                    {
                            s = "[";
                            s +=  lpSubKey ;
                            s+= "]";
                            s+= "\n";

                            for(int i = 0;i  < num_param; i++)
                            {
                                     s +=  reg_str_param_name[i];
                                     s += "=\n";
                            }

                       wr_z = write(hFile, (const void*)s.c_str(), s.size());
                            if(wr_z != (ssize_t)s.size())
                            {
                               fprintf(stderr, "Cannot write file\n");
                                       exit(5);
                            }
                    }

            return ERROR_SUCCESS;
        }




 //-----------------------------------------------------------------------------------------------------------------------------------------------------
 long  TRegistry::RegSetValueEx(HKEY /*RootKey*/, LPCTSTR lpValueName, DWORD /*Reserved*/, DWORD /*dwType*/,
                                const BYTE *lpData, DWORD cbData)
 {
            if(buffer != NULL)
                    delete[] buffer;
    off_t sz_file;
            lseek((int)hFile, 0,SEEK_SET);
            if((sz_file = lseek((int)hFile, 0,SEEK_END)) <= 0)
            {
                        return 10L;
            }
            if(sz_file  > 0)
            {
                    buffer = new char[sz_file + 1];
                    memset(buffer, 0, sz_file + 1);

            lseek((int)hFile, 0,SEEK_SET);
            ssize_t rd_z = read(hFile,buffer,sz_file);
                    if(rd_z != (ssize_t)sz_file)
                    {
                        fprintf(stderr, "Cannot read fille\n");
                         exit(11L);
                    }
            }

    string  s( buffer);

    string  sub_s = lpValueName;

    size_t  beg = s.find(sub_s);

            if(beg == string::npos)
            {
            string s("Cannot find value ");
                    s += sub_s;
                    fprintf(stderr,"%s",(char*)s.c_str());
                    exit(8L);
            }

            beg =  s.find("=", beg);
            if(beg == string::npos)
            {
             string s("Cannot find param ='");
                s += sub_s;
                    fprintf(stderr,"%s",(char*)s.c_str());
                     exit(1L);
            }
            beg++;

    DWORD i,j;
            for(i = 0, j = 0; i < cbData; i++)
            {
                char c = (((char*)lpData)[i]);
                    s.insert((s.begin() + beg + j), c);
                j++;
            }

            off_t ls_z = lseek(hFile,0,SEEK_SET);
            if(ls_z < 0)
            {
                  fprintf(stderr, "Cannot lseek file\n");
                          exit(9);
            }

     ssize_t wr_z = write(hFile, (const void*)s.c_str(), s.size());
     return wr_z != (ssize_t)s.size();
 }
//-----------------------------------------------------------------------------------------------------------------------------
long  TRegistry:: RegQueryValueEx(HKEY /*hKey*/, LPCTSTR lpValueName, LPDWORD /*lpReserved*/,LPDWORD /*lpType*/,LPBYTE lpData, LPDWORD lpcbData)
 {

        if(buffer != NULL)
            delete[] buffer;
    off_t sz_file;
            lseek((int)hFile, 0,SEEK_SET);
            if((sz_file = lseek((int)hFile, 0,SEEK_END)) <= 0)
            {
                return 10L;
            }
            if(sz_file  > 0)
            {
                    buffer = new char[sz_file + 1];
                    memset(buffer, 0, sz_file + 1);

                    lseek((int)hFile, 0,SEEK_SET);
    ssize_t rd_z = read(hFile,buffer,sz_file);
                    if(rd_z != (ssize_t)sz_file)
                    {
                        fprintf(stderr, "Cannot read fille\n");
                        exit(13);
                    }
            }
    //char *  ptr = (char*) buffer;
    string  s(/*ptr*/buffer);
    string  out;
    string  sub_s = lpValueName;

    size_t  beg = s.find(sub_s);
            beg =  s.find("=", beg);
            beg++;
    size_t  end = s.find('\n',beg);
            if(beg < end)
             {
               string o(s,beg, end - beg);
               *lpcbData = o.size();
               out = o;
             }
            else
             {
                   return 14L;
             }

             memcpy(lpData, out.c_str(), out.size());
             lpData[out.size()] = 0;
     return ERROR_SUCCESS;
 }


//---------------------------------------------------------------------------------
long MessageBox(HWND /*hnd*/, LPCTSTR lpMes1,  LPCTSTR lpMes2, DWORD wOk)
{
    string s = lpMes1;
    s += lpMes2;
    s += "\n";

    fprintf(stderr,"%s",s.c_str());
 return wOk;
}
#endif


#ifdef __BORLANDC__
#pragma package(smart_init)
#endif

