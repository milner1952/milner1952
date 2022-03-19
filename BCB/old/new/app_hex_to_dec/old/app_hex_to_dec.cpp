//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop



#pragma hdrstop

#ifndef __BORLANDC__
	#define _CRT_SECURE_NO_WARNINGS
	#ifndef _CRT_SECURE_NO_DEPRECATE
	#define _CRT_SECURE_NO_DEPRECATE (1)
	#endif
# pragma warning(disable : 4996)
#endif


//#include <strstream>
//#include <iostream>
#include <stdio>
#include <conio.h>
#include <time.h>





#include <string>
#include <string.h>
#include <ctime>
#include <vector.h>
#include <iomanip>

#define M_DEBUG

using namespace std;

int  hex_to_dec_GetLengthFile(const char *name);
DWORD  hex_to_dec_File_Read(const char *name, const char *btr, DWORD num);
string *hex_to_dec_Converting(string *s, size_t num);
string hex_to_dec_ConvertingString(string *s,size_t begin, size_t *end);
bool hex_to_dec_File_Write(char *name, char* btr, size_t num);
//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char* argv[])
{
char name[1000];
char *btr;
DWORD num;
        memset(name,0,1000);
        memcpy(name,argv[1], strlen(argv[1]));
        num = hex_to_dec_GetLengthFile(name);
        if(!num)
                return 1;
        btr = new char[num + 2];
        memset(btr, 0 ,num + 2);
        if(hex_to_dec_File_Read(name, btr, num))
                return 2;
string s(btr);
        string *out = hex_to_dec_Converting((string*)&s,num);
        if(hex_to_dec_File_Write((char*)&name, (char*)out->c_str(), out->length()  ))
                return 2;
delete out;
delete[] btr;
        return 0;
}

//---------------------------------------------------------------
DWORD  hex_to_dec_File_Read(const char *name, const char *btr, DWORD num)
{
	FILE *f;
	DWORD ret;
	if ((f = fopen(name, "rb")) == NULL)
	{
		return 1;
	}
        fseek(f, 0, SEEK_SET);
	fread((void*)btr, sizeof(char) ,num, f);
        if( ferror( f ) )
        {
                return 2;
        }
	if (fclose(f) == EOF)
	{
		return 3;
	}
	return 0;
}


//--------------------------------------------------------------
int   hex_to_dec_GetLengthFile(const char *name)
{
 FILE *f;
 if((f = fopen(name, "rb")) == NULL)
 {

        return 0;
 }
        fseek(f, 0, SEEK_SET);
        fseek(f, 0, SEEK_END);
DWORD num = ftell(f);
        fseek(f, 0, SEEK_SET);


if(fclose(f) == EOF)
{
        return 0;
}

return num;
}

//-------------------------------------------------------------------
string *hex_to_dec_Converting(string *s, size_t num)
{
size_t begin = 0;
size_t end = 0;
string *out;

#ifdef M_DEBUG
char *te;
char *tb;
char *to;
#endif

        out = new string;
        out->clear();
        if((end = s->find("PATTERN",begin)) == string::npos)
                        return NULL;
        if((end = s->find('=',end + 1)) == string::npos)
                        return NULL;
        begin = end + 1;
        if((end = s->find('\n',begin)) == string::npos)
                        return NULL;

        begin = end + 1;
        if((end = s->find('\n',begin)) == string::npos)
                        return NULL;                        

string t;
        for(; end < num-5;  begin = (end + 1))
        {

#ifdef M_DEBUG
        tb = ((char*)s->c_str() + begin);
        te = ((char*)s->c_str() + end);
#endif
                     t =  hex_to_dec_ConvertingString(s, begin, &end);
                        *out += t;

#ifdef M_DEBUG
        to = ((char*)out->c_str());
        tb = ((char*)s->c_str() + begin);
        te = ((char*)s->c_str() + end);

#endif
                
        }
 return out;
}

//-----------------------------------------------------------------------------------
string hex_to_dec_ConvertingString(string *s,size_t begin, size_t *end)
{
size_t b = 0;
size_t e = 0;

char buf_h[10];
char buf_d[10];
char *tb;
#ifdef M_DEBUG
char *te;
char *to;
#endif
        b = begin;
        e = s->find('=',b);
#ifdef M_DEBUG
        tb = ((char*)s->c_str() + b);
        te = ((char*)s->c_str() + e);
#endif
       string out(*s,begin, e - b);
        b = s->find('=',b);
#ifdef M_DEBUG
        to = (char*)out.c_str();
        tb = ((char*)s->c_str() + b);
        te = ((char*)s->c_str() + e);
#endif

        while(1)
        {
           memset(buf_h, 0, 10);
           memset(buf_d, 0, 10);
#ifdef M_DEBUG
        tb = ((char*)s->c_str() + b);
        te = ((char*)s->c_str() + e);
#endif

bool f;
                while(1)
                {

                        tb = (char*)(s->c_str() + b);
                        f =   (isalpha(*tb) || isdigit(*tb));
                        if(f)
                        {
                                break;
                        }
                        b++;
                }



                e = s->find(' ',b);

#ifdef M_DEBUG
           tb = ((char*)s->c_str() + b);
           te = ((char*)s->c_str() + e);
#endif


           memcpy((void*)buf_h, (void*)(s->c_str() + b), e - b);

char *ptr;
   DWORD d  = strtoul(buf_h, &ptr, 16);
           sprintf(buf_d,"%06d",d);
           out += "  ";
           out += buf_d;
           b = e;
                while(1)
                {

                        te = (char*)(s->c_str() + e);
                        if(*te == '\n')
                        {
                         *end = e;
                          out += "\r\n";
                          return out;
                        }
                        f =   (isalpha(*te) || isdigit(*te));
                        if(f)
                        {
                                break;
                        }
                        e++;
                }


#ifdef M_DEBUG
           tb = ((char*)s->c_str() + b);
           te = ((char*)s->c_str() + e);
#endif

        }


}

//---------------------------------------------------------------------------------------
bool hex_to_dec_File_Write(char *name, char* btr, size_t num)
{
  FILE *f;
string s(name);
WORD n = s.rfind("\\",strlen(name));
s.insert(n + 1,"c_");
         if((f = fopen(s.c_str(), "wb")) == NULL)
         {
                return 1;
         }

        fwrite((const void*)btr, num, 1, f);

        if(fclose(f) == EOF)
        {
                return 2;
        }
        return 0;
}
