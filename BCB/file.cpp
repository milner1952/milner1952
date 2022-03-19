//---------------------------------------------------------------------------


#ifndef __BORLANDC__
    #define _CRT_SECURE_NO_WARNINGS
    #ifndef _CRT_SECURE_NO_DEPRECATE
        # define _CRT_SECURE_NO_DEPRECATE (1)
#endif
#ifndef unix
    #pragma warning(disable : 4996)
    #pragma warning(disable: 4800)
#endif
#endif

#ifdef _MSC_BUILD
#include "res_bcb.h"
#endif

#ifdef __BORLANDC__
#include <vcl.h>
//#endif
#pragma hdrstop
#endif

#include <ctime>
#include <string>
    #ifndef unix
        #include <windows.h>
        #include <Winbase.h>
    #endif
#include <stdio.h> 
#include <errno.h>


#include "dll_src.h"
#include "test_dll.h"
#include "reg.h"
#include "svaz.h"
#include "Reduction_class.h"
#include "Reduction.h"
#include "adapter.h"

#include "file.h"

#ifdef __BORLANDC__
#include "res_bcb.h"
#endif

#ifdef unix
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include "../../SRC/BCB/RES_UNIX/res_unix.h"
#endif


static BYTE* AsAsc(BYTE *dst, BYTE *src, WORD num);
BYTE ASC2[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

BYTE bufer_noise[4096 * 25];

extern DWORD dw_ion[8];
extern double db_ion[8];
extern DWORD dw_ion_afte[8];
extern double db_ion_afte[8];
extern DWORD dw_ion_bfr[8];
extern double db_ion_bfr[8];

extern DWORD dw_noise_bufer[8];
extern double db_noise_bufer[8];

V_NOISE_DATA v_statistic_data;
//---------------------------------------------------------------------------

BYTE* AsAsc(BYTE *dst,BYTE *src, WORD num)
{
    WORD it, ot;
    for(it = 0, ot = 0; it < num; it++, ot+=2)
    {
        BYTE t =   src[it];
        BYTE tl   =  ASC2[t & 0x0f];
        t = t >> 4;
        BYTE th   =  ASC2[t & 0x0f];
        dst[ot + 1] = tl;
        dst[ot ] = th;
    }
    return &dst[ot];
}

//---------------------------------------------------------------------------
DWORD file_SaveInFile(const char * name, BYTE *p_data, WORD len_p_data ,WORD *buf_in, DWORD num_pix)
{
#ifdef __BORLANDC__
	int h_file =   FileCreate(name, fmCreate );
		FileSeek(h_file, 0, 0);
	DWORD ret = FileWrite(h_file, (const void*)p_data,  len_p_data);
		ret += FileWrite(h_file, (const void*)buf_in,  num_pix * sizeof(WORD));
		FileClose(h_file);
#else
    #ifndef unix
    HANDLE  h_file =   FileCreate(name, GENERIC_WRITE );
		FileSeek(h_file, 0, 0);
	DWORD ret = FileWrite(h_file, (const void*)p_data,  len_p_data);
          ret += FileWrite(h_file, (const void*)buf_in,  num_pix * sizeof(WORD));
		FileClose(h_file);
    #else

    int     h_file = open(name, O_WRONLY | O_CREAT, 0777);
            if(h_file < 0)
            {
                close(h_file);
                 return 0;
            }
    off_t len =  lseek((h_file),0,SEEK_SET);
            if(len < 0)
            {
                close(h_file);
                 return 0;
            }
    DWORD ret =  write(h_file,(const void*)p_data,  len_p_data);
          ret += write(h_file, (const void*)buf_in,  num_pix * sizeof(WORD));
            if(ret != (len_p_data + num_pix * sizeof(WORD) ) )
            {
                close(h_file);
                 return 0;
            }

            close(h_file);

    #endif
#endif
	return ret;
}

//-----------------------------------------------------------------------------
DWORD file_LoadFromFile(char* name, BYTE **btr)
{
#ifdef __BORLANDC__
	int h_file =   FileOpen(name, fmOpenRead);
		if(h_file == -1)
			return 0L;
		FileSeek(h_file, 0, 0);
	DWORD len = FileSeek(h_file,0,2);
		FileSeek(h_file, 0, 0);
		FileRead(h_file,bufer_noise, len);
		*btr = (BYTE*)&bufer_noise;
		FileClose(h_file);
#else
    #ifndef unix
        HANDLE  h_file =   FileCreate(name, GENERIC_READ );
            if(h_file == INVALID_HANDLE_VALUE)
                return 0L;
            FileSeek(h_file, 0, 0);
        DWORD len = FileSeek(h_file,0,2);
            FileSeek(h_file, 0, 0);
            FileRead(h_file,bufer_noise, len);
            *btr = (BYTE*)&bufer_noise;
            FileClose(h_file);
    #else
    int  h_file = open(name, O_RDONLY|O_CREAT, 0777);
            if(h_file < 0)
                    return 0;
            off_t len = lseek(h_file,0,SEEK_SET);
                    if(len < 0)
                        return 0;
                    len = lseek(h_file,0,SEEK_END);
                    if(len < 0)
                        return 0;
                    lseek(h_file,0,SEEK_SET);
                    if(len < 0)
                        return 0;
            size_t  num = read(h_file, bufer_noise, len);
                    if(num != (size_t)len)
                        return (0);
                    *btr = (BYTE*)&bufer_noise;
                    close(h_file);
    #endif
#endif
	return len;
}

#define NUM_PIX_IN_STRING_256_X_9 256 * 9
#define NUM_PIX_IN_STRING_256_X_8 256 * 8
#define NUM_PIX_IN_STRING_256_X_4 256 * 4
//---------------------------------------------------------------------------
DWORD file_SaveIn_bufer_log(const char * name, BYTE *buf_in, DWORD num, REGDATA *regdata)
{
AnsiString a_name(name);
#ifdef __BORLANDC__
	int h_file = FileCreate(a_name, fmCreate );
#else
    #ifndef unix
    HANDLE h_file = FileCreate(name, GENERIC_WRITE );
    #else
int     h_file = open(name, O_WRONLY | O_CREAT, 0777);
        if(h_file < 0)
        {
            close(h_file);
             return 0;
        }
    #endif
#endif
	 

AnsiString string_file;
AnsiString out_string;
AnsiString in_string;

BYTE *src_in;
src_in =  buf_in;

char dst_out[5];
DWORD num_string = num / (regdata->pixsels_per_string * sizeof(WORD));

        for(DWORD j = 0; j < num_string;j++)
        {
        AnsiString string_bufer;
#ifdef __BORLANDC__
        AnsiString  n = (j + 1);
#else
		char cn[10];
        #ifndef unix
                itoa(j + 1, cn, 10);
        #else
                sprintf(cn,"%ld", (j + 1));
        #endif
        AnsiString  n(cn);
#endif

                string_bufer = str_string_n;
                string_bufer += n.c_str();
                string_bufer += "\r\n ";
                if(regdata->pixsels_per_string == 2304)
                {
                        for(int i = 0; i < (int)(NUM_PIX_IN_STRING_256_X_9 * sizeof(WORD)) ; )
                        {
                                memset(dst_out, 0, 5);

                                AsAsc((BYTE*)&dst_out[0] , (BYTE*)src_in, 1);
                                src_in += 1;
                                i += 1;

                                AsAsc((BYTE*)&dst_out[2] , (BYTE*)src_in, 1);
                                src_in += 1;
                                i += 1;

                                string_bufer += dst_out;

                                string_bufer += " ";


                                if((i %  (2 * 9)) == 0)
                                        string_bufer += "\r\n";

                                if((i %  2) == 0)
                                        string_bufer += " ";

                        }
#ifndef __BORLANDC__
						DWORD sz_cap = string_file.capacity();
						DWORD sz_req = string_file.size() + string_bufer.size();
						if(sz_req > sz_cap)
							string_file.reserve(sz_cap * 3);
						sz_cap = string_file.capacity();
#endif
                        string_file +=  string_bufer;
                        string_file += "\r\n";
                }
                else
                if(regdata->pixsels_per_string == 2048)
                {
                        for(int i = 0; i < (int)(NUM_PIX_IN_STRING_256_X_8 * sizeof(WORD)) ; )
                        {
                                memset(dst_out, 0, 5);

                                AsAsc((BYTE*)&dst_out[0] , (BYTE*)src_in, 1);
                                src_in += 1;
                                i += 1;

                                AsAsc((BYTE*)&dst_out[2] , (BYTE*)src_in, 1);
                                src_in += 1;
                                i += 1;

                                string_bufer += dst_out;

                                string_bufer += " ";


                                if((i %  (2 * 8)) == 0)
                                        string_bufer += "\r\n";

                                if((i %  2) == 0)
                                        string_bufer += " ";

                        }
#ifndef __BORLANDC__
						DWORD sz_cap = string_file.capacity();
						DWORD sz_req = string_file.size() + string_bufer.size();
						if(sz_req > sz_cap)
							string_file.reserve(sz_cap * 3);
						sz_cap = string_file.capacity();
#endif
                        string_file +=  string_bufer;
                        string_file += "\r\n";
                }
                else
                if(regdata->pixsels_per_string == 1024)
                {

                        for(int i = 0; i < (int)(NUM_PIX_IN_STRING_256_X_4 * sizeof(WORD)); )
                        {
                                memset(dst_out, 0, 5);

                                AsAsc((BYTE*)&dst_out[0] , (BYTE*)src_in, 1);
                                src_in += 1;
                                i += 1;


                                AsAsc((BYTE*)&dst_out[2] , (BYTE*)src_in, 1);
                                src_in += 1;
                                i += 1;

                                string_bufer += dst_out;

                                string_bufer += " ";


                                if((i %  (2 * 4)) == 0)
                                        string_bufer +=    "\r\n ";

                        }
                        string_file +=  string_bufer;
                        string_file += "\r\n";
                }
        }


                if(Iz_reduct == 1)
#ifndef __BORLANDC__

				{
				DWORD sz_cap,sz_req;
						AnsiString s  =    rduc_SetTestReductDataToFile();
						sz_cap = string_file.capacity();
						sz_req = string_file.size() + s.size();
						if(sz_req > sz_cap)
							string_file.reserve(sz_req * 3);
						sz_cap = string_file.capacity();
						string_file += s;
				}			
#else
                string_file += rduc_SetTestReductDataToFile();
#endif
		
				in_string = file_PrintMidleForInBuferFromADC(regdata);


				string_file +=  in_string;
                AnsiString at;
                        if(!Iz_reduct)
                                at = str_ion_in;
                        if(If_reduct_end_not_ion_compensat)
                                at = str_ion_before;
                        if(If_reduct_end_ion_compensat == 1)
                                at  = str_ion_before_reduct_and_ion_compensation;

                string_file += file_Print_Ion_bufer(dw_ion_bfr, db_ion_bfr, regdata, (char*)at.c_str());


#ifdef _MIT_ERROR_ION
                        if(If_ion_compencat == 1)
                        {
                                if(Iz_reduct)
                                        at = str_ion_after_reduct_and_ion_compensation;
                                else
                                        at = str_before_ion_compensation;
                                string_file += file_Print_Ion_bufer(dw_ion, db_ion, regdata, (char*)at.c_str());
                        }
#endif
                        if(If_reduct_or_ion_compensat == 1)
                        {
                                if(If_ion_compencat_end_not_reduct)
                                        at  = str_after_ion_compensation;
                                if(If_reduct_end_not_ion_compensat)
                                        at  = str_after_reduct;
                                if(If_reduct_end_ion_compensat == 1)
                                        at  = str_ion_after_reduct_and_ion_compensation;
                                string_file += file_Print_Ion_bufer(dw_ion_afte, db_ion_afte, regdata, (char*)at.c_str());
                        }

				out_string = file_PrintMidleForOutBuferFromADC(regdata);

                string_file += out_string;
                string_file += "\r\n";

              char st[100];
              const time_t t = time(NULL);
                memset(st,0,sizeof(st));
                strcpy(st,ctime(&t));
                string_file += "\r\n";
                string_file +=  st;
#ifdef __BORLANDC__
                *((char*)string_file.c_str() + string_file.Length() - 1)  = ' ';
                *((char*)string_file.c_str() + string_file.Length() )  = '\0';
				FileWrite(h_file, string_file.c_str(),  string_file.Length());
				FileClose(h_file);
		return string_file.Length();
#else
                *((char*)string_file.c_str() + string_file.size() - 1)  = ' ';
                *((char*)string_file.c_str() + string_file.size() )  = '\0';
        #ifndef unix
				FileWrite(h_file, string_file.c_str(),  string_file.size());
				FileClose(h_file);
        #else
                {
               size_t num =  write(h_file,string_file.c_str(), string_file.size());
                    if(num != string_file.size())
                    {
                        close(h_file);
                        return 0;
                    }
                }
        #endif
		return string_file.size();
#endif
}

//-------------------------------------------------------------------------------
AnsiString file_Print_Ion_bufer(DWORD *dw_bufer, double *db_bufer, REGDATA *regdata, char *text)
{
DWORD iz_max = 0;
DWORD iz_min = 0xffffffff;
DWORD iz_midle = 0L;

double db_iz_max = 0.0;
double db_iz_min = 65536.0;
double db_iz_midle = 0.0;

AnsiString mes("\r\n\r\n");
mes += text; mes += "\r\n";
int num_adc = regdata->pixsels_per_string == 2304 ? 9 : regdata->pixsels_per_string == 2048 ? 8 : 4;
        for(int i = 0; i < num_adc; i++)
        {
                mes += "ION"; mes += i;
                mes += str_midle;
#ifndef __BORLANDC__
                M_SPRINTF(mes, (dw_bufer[i] / (256 - _2048_ION_OFFS)))
#else
				mes += (dw_bufer[i]/(256 - _2048_ION_OFFS));
#endif
                mes += ";  ";
                mes += str_coefficient;

#ifndef __BORLANDC__
                M_SPRINTF(mes, db_bufer[i])
#else
				mes += db_bufer[i];
#endif

				mes += ";     ";
                if((i & 0x01)== 0x01)
                        mes += "\r\n";
                iz_max = max(iz_max,(dw_bufer[i]/(256 - _2048_ION_OFFS)));
                iz_min = min(iz_min,(dw_bufer[i]/(256 - _2048_ION_OFFS)));
                iz_midle  += dw_bufer[i]/(256 - _2048_ION_OFFS);

                db_iz_max = max(db_iz_max,(db_bufer[i]));
                db_iz_min = min(db_iz_min,(db_bufer[i]));
                db_iz_midle += db_bufer[i];

        }
        iz_midle /= num_adc;
        db_iz_midle /= num_adc;
        mes += "        MAX = ";

#ifndef __BORLANDC__
        M_SPRINTF(mes, iz_max)
#else
		mes += iz_max;
#endif

		mes += "; MIN = ";
#ifndef __BORLANDC__
		M_SPRINTF(mes, iz_min)
#else
		mes += iz_min;
#endif
		mes += "\r\n";
                mes += str_delta;
#ifndef __BORLANDC__
		M_SPRINTF(mes, (iz_max - iz_min))
#else
		mes += (iz_max - iz_min);
#endif		
		mes += str_midle_from_ion;

#ifndef __BORLANDC__
		M_SPRINTF(mes, iz_midle)
#else		
		mes += iz_midle;
#endif

		mes += str_delta_midle_from_ion;
#ifndef __BORLANDC__
		M_SPRINTF(mes, (((double)(iz_max - iz_min) / (iz_midle))* 100.0))
#else		
		mes += ((double)(iz_max - iz_min)/(iz_midle))* 100.0;
#endif
        
		mes += ";";
        mes += "\r\n";
        mes += str_coefficient_max;
#ifndef __BORLANDC__
		M_SPRINTF(mes, db_iz_max)
#else		
		mes += db_iz_max;
#endif
        
		mes += str_coefficient_min;

#ifndef __BORLANDC__
		M_SPRINTF(mes, db_iz_min)
#else
		mes += db_iz_min;
#endif

        mes += str_coefficient_midle;
		
#ifndef __BORLANDC__
		M_SPRINTF(mes, db_iz_midle)
#else
		mes += db_iz_midle;
#endif

        mes += str_delta_coefficient;

#ifndef __BORLANDC__
		M_SPRINTF(mes, (db_iz_max - db_iz_min))
#else		
		mes += (db_iz_max - db_iz_min);
#endif
        mes += str_diviation_coefficient;

#ifndef __BORLANDC__
        M_SPRINTF(mes, (long int)((db_iz_max - db_iz_min) / db_iz_midle))
#else		
		mes += (db_iz_max - db_iz_min)/db_iz_midle;
#endif

        mes += "\r\n";
return mes;
}


//---------------------------------------------------------------------------------------------
void file_SaveIn_statistic_log(const char *name, DWORD *dw_noise_bufer, DWORD *dw_in, DWORD *dw_out,
                              double * /*db_noise_bufer*/ , WORD /*pixsels_per_string*/, MAIN_APP flag_main_app,
                              DWORD *dw_ion, DWORD *dw_ion_bfr, DWORD *dw_ion_afte, REGDATA *regdata, DWORD pix_num)

{

AnsiString a_name(name);

#ifdef __BORLANDC__
	int h_file;
#else
#ifndef unix
	HANDLE h_file;
#else
    int h_file;
#endif
#endif



        if((flag_main_app == APP_NEYSTROEV) &&   (pix_num < 150000))
            return;
#ifdef __BORLANDC__
			if(!FileExists(a_name))
                h_file =   FileCreate(a_name, fmCreate );
			else
                h_file =   FileOpen(a_name, fmOpenWrite );
#else

        #ifndef unix
				h_file =   FileCreate(name, GENERIC_WRITE );
        #else
                h_file = open(name, O_WRONLY | O_CREAT, 0777);
                if(h_file < 0)
                {
                    close((int)h_file);
                     return ;
                }
        #endif
#endif


AnsiString string_file;
char st[100];
        const time_t t = time(NULL);
        memset(st,0,sizeof(st));
        strcpy(st,ctime(&t));
        string_file += "\r\n";
        string_file +=  st;

#ifdef __BORLANDC__
	int len =  string_file.Length();
#else
	int len =  string_file.size();
#endif
        if((*(string_file.c_str() + len -1)== 0x0d)  ||
           (*(string_file.c_str() + len -1)== 0x0a))
#ifdef __BORLANDC__
            string_file.Delete(len, 1);
#else
			string_file.erase(len);
#endif
            string_file += "\r\n";

        s_dll_src_SetContainerOfNoise(&v_statistic_data, dw_in, dw_out, dw_ion, dw_ion_bfr, dw_ion_afte,
                                        flag_main_app, pix_num);

I_NOISE_DATA b = v_statistic_data.begin();
I_NOISE_DATA e = (v_statistic_data.end() - 1);

int delta_in[9];
int delta_ns[9];
int delta_out[9];

int delta_ion[9];
int delta_ion_bfr[9];
int delta_ion_afte[9];


int num = Iz_pxs_per_string == 2304 ? 9 : Iz_pxs_per_string == 2048 ? 8 : 4;


        for(int i = 0; i < num; i++)
        {
                delta_in[i] = (*e).d_in[i] - (*b).d_in[i];
                delta_ns[i] = (*e).d_ns[i] - (*b).d_ns[i];

                delta_ion[i] = (*e).d_ion[i] - (*b).d_ion[i];
                delta_ion_bfr[i] = (*e).d_ion_bfr[i] - (*b).d_ion_bfr[i];
                delta_ion_afte[i] = (*e).d_ion_afte[i] - (*b).d_ion_afte[i];

                delta_out[i] = (*e).d_out[i] - (*b).d_out[i];
        }
AnsiString at;
        SAVESTATISTIC("IN:",dw_in, delta_in,num);

        if(!Iz_reduct)
                at = str_ion_in;
        if(If_reduct_end_not_ion_compensat)
                at = str_ion_before;
        if(If_reduct_end_ion_compensat == 1)
                at  = str_ion_before_reduct_and_ion_compensation;
        SAVESTATISTIC((char*)at.c_str(),dw_ion_bfr, delta_ion_bfr, num);


#ifdef _MIT_ERROR_ION
        if(If_ion_compencat == 1)
        {
                if(Iz_reduct)
                        at = str_ion_after_reduct_and_ion_compensation;
                else
                        at = str_before_ion_compensation;
        SAVESTATISTIC((char*)at.c_str(),dw_ion, delta_ion, num);
        }
#endif


        if(If_reduct_or_ion_compensat == 1)
        {
                if(If_ion_compencat_end_not_reduct)
                        at  = str_after_ion_compensation;
                if(If_reduct_end_not_ion_compensat)
                        at  = str_after_reduct;
                if(If_reduct_end_ion_compensat == 1)
                        at  = str_after_ion_compensation_and_reduct;
                SAVESTATISTIC((char*)at.c_str(),dw_ion_afte, delta_ion_afte, num);
        }
        SAVESTATISTIC("NOISE:",dw_noise_bufer, delta_ns, num);
        SAVESTATISTIC("OUT:",dw_out, delta_out, num);

        string_file += "\r\n\r\n";



#ifdef __BORLANDC__
        FileSeek(h_file, 0, 2);
        FileWrite(h_file, string_file.c_str(),  string_file.Length());
        FileClose(h_file);

#else
        FileSeek((HANDLE)h_file, 0, 2);
        FileWrite((HANDLE)h_file, string_file.c_str(),  string_file.size());
        FileClose((HANDLE)h_file);
#endif


}

//-----------------------------------------------------------------------
AnsiString file_Save_statistic(DWORD *dw_noise, int num)//1
{
AnsiString noise_str;
#ifdef __BORLANDC__
if(num == 8)
{
                noise_str.printf("%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7]);
}
else
if(num == 9)
{
                noise_str.printf("%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7],dw_noise[8]);

}
#else
char b[1000];
if(num == 8)
{
				memset(b, 0, 1000);
                sprintf(b,"%05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7]);
                /*
				sprintf(b,"%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7]);
                                  */
				noise_str = b;
}
else
if(num == 9)
{
				memset(b, 0, 1000);
                sprintf(b,"%05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t %05ld\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7], dw_noise[8]);
/*
				sprintf(b,"%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7], dw_noise[8]);
                */

				noise_str = b;
}
#endif
                noise_str += "\t";

return noise_str;
}


//-----------------------------------------------------------------------
AnsiString file_Save_statistic(int *dw_noise, int num)//2
{
AnsiString noise_str;
#ifdef __BORLANDC__
if(num == 8)
{
                noise_str.printf("%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7]);
}
else
if(num == 9)
{
                noise_str.printf("%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7], dw_noise[8]);
}
#else
char b[1000];
if(num == 8)
{
				memset(b, 0, 1000);
				sprintf(b, "%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7]);
				noise_str = b;
}
else
if(num == 9)
{
				memset(b, 0, 1000);
				sprintf(b, "%05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t %05d\t",
                                  dw_noise[0], dw_noise[1],dw_noise[2],dw_noise[3],
                                  dw_noise[4],dw_noise[5],dw_noise[6],dw_noise[7], dw_noise[8]);
				noise_str = b;
}

#endif
return noise_str;
}


//-----------------------------------------------------------------------
AnsiString file_Save_statistic(char *str)//3
{
AnsiString string_file(str);
return string_file;
}

#ifndef __BORLANDC__
//------------------------------------------------------------------------
#ifndef unix
HANDLE __fastcall FileCreate(const char* FileName, int Mode)
{
	HANDLE hFile;

    hFile = CreateFile((LPWSTR)FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE)
			{
                test_SetStringTooOutDebug3("\r\nNo open file: ",(unsigned char*)FileName);
			}
	return hFile;

}
#else
HANDLE  FileCreate(const char* FileName, int Mode)
{
    int hFile;

            hFile =  open(FileName, /*O_WRONLY*/Mode | O_CREAT, 0777);
            if (hFile < 0)
            {
                test_SetStringTooOutDebug3("\r\nNo open file: ",(unsigned char*)FileName);
            }
    return (HANDLE)hFile;

}
#endif
//---------------------------------------------------------------------------
#ifndef unix
int  __fastcall  FileSeek(HANDLE Handle, int Offset, int Origin)
{
	DWORD e; 
			if(((DWORD)-1) == SetFilePointer(Handle, (long)Offset, NULL, Origin))
			{
				e = GetLastError();
				test_SetStringTooOutDebug1("FileSeek",e);
                return - 1;
			}

		return 0;
}
#else
int   FileSeek(HANDLE Handle, int Offset, int Origin)
{

            if(lseek((int)Handle, Offset, Origin) < 0)
            {
                test_SetStringTooOutDebug1("FileSeek",0);
                return -1;
            }

        return 0;
}
#endif
//------------------------------------------------------------------
#ifndef unix
DWORD __fastcall FileWrite(HANDLE Handle, const void *Buffer, DWORD Count)
	{
	DWORD num;
	DWORD e;
		if(!WriteFile( Handle,(LPCVOID)Buffer, (DWORD) Count, (LPDWORD)&num, NULL))
		{
				e = GetLastError();
                test_SetStringTooOutDebug1("FileWrite",e);
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
                test_SetStringTooOutDebug1("FileWrite", 0);
                return 0;
        }

        return num;
}
#endif
//----------------------------------------------------------------
#ifndef unix
void __fastcall FileClose(HANDLE Handle)
{
	DWORD e;
		if(!CloseHandle(Handle))
		{
				e = GetLastError();
				test_SetStringTooOutDebug1("FileClose",e);
		}
}
#else
void  FileClose(HANDLE Handle)
{

        if(-1 == close((int)Handle))
        {

                test_SetStringTooOutDebug1("FileClose",-1);
        }
}
#endif
//----------------------------------------------------------------------
#ifndef unix
DWORD __fastcall FileRead(HANDLE Handle, void *Buffer, DWORD Count)
{

	DWORD num;
	DWORD e;
			if(!ReadFile(Handle, Buffer, Count, (LPDWORD)&num, NULL))
			{
				e = GetLastError();
				test_SetStringTooOutDebug1("FileClose",e);
			}
		return num;
}
#else
DWORD  FileRead(HANDLE Handle, void *Buffer, DWORD Count)
{

    DWORD num;

            num = read((int)Handle, Buffer, Count);
            if(num != Count)
            {
                test_SetStringTooOutDebug1("FileClose",0);
                return 0;
            }
        return num;
}
#endif
#endif
//-----------------------------------------------------------------------

AnsiString file_PrintMidleForInBuferFromADC(REGDATA *regdata)
{

DWORD *dw_b = rduc_GetPtrForMidleBuferIn();

return file_PrintMidleForBuferFromADC(dw_b,  "\r\nIN: ", regdata);
}


//----------------------------------------------------------------------------
AnsiString file_PrintMidleForOutBuferFromADC(REGDATA *regdata)
{

DWORD *dw_b = rduc_GetPtrForMidleBuferOut();

return file_PrintMidleForBuferFromADC(dw_b, "\r\nOUT: ", regdata);
}

//-------------------------------------------------------------
AnsiString file_PrintMidleForBuferFromADC(DWORD *wtr,const char *str, REGDATA *regdata)
{
DWORD iz_max = 0;
DWORD iz_min = 0xffffffff;
DWORD iz_midle;
AnsiString mes;
BYTE num_adc = Iz_pxs_per_string == 2304? 9:  Iz_pxs_per_string == 2048 ? 8 : 4;

        mes = str;
        if(!strcmp(str,"\r\nIN: "))
                mes += str_midl_of_bufer_in;
        else
                mes += str_midl_of_bufer_out;
        for(int i = 0; i < num_adc; i++)
        {
                mes += str_adc;
#ifdef __BORLANDC__
                mes += i;
				mes += ": ";
				mes += wtr[i];
                mes += ";    ";
                iz_max = max(iz_max,wtr[i]);
                iz_min = min(iz_min,wtr[i]);
#else
		char buf[10];
				memset(buf, 0, 10);
        #ifndef unix
				itoa(i, buf, 10);
        #else
                sprintf(buf, "%d", i);
        #endif
				mes += buf;

				mes += ": ";
				memset(buf, 0, 10);
        #ifndef unix
				itoa(wtr[i], buf, 10);
        #else
                sprintf(buf, "%ld",wtr[i]);
        #endif
				mes += buf;
                mes += ";    ";
                iz_max = max(iz_max,wtr[i]);
                iz_min = min(iz_min,wtr[i]);

#endif
        }
#ifdef __BORLANDC__
        mes += "\r\nMAX = ";  mes += iz_max;
        mes += "; MIN = ";  mes += iz_min;
        iz_midle = (iz_max + iz_min)/2.0; mes += str_midle;
        mes += iz_midle;
        mes += "\r\n";
        mes += str_delta;

        mes += (iz_max - iz_min);
        mes += str_diviation_from_midle;
        mes += ((double)(iz_max - iz_min)/(iz_midle))* 100.0;
#else
		char buf[10];
        mes += "\r\nMAX = "; 
		memset(buf, 0, 10);                
    #ifndef unix
        ltoa(iz_max, buf, 10);
    #else
        sprintf(buf, "%ld",iz_max);
    #endif
		mes += buf;
        mes += "; MIN = ";  
		memset(buf, 0, 10);

    #ifndef unix
		ltoa(iz_min, buf, 10);
    #else
        sprintf(buf, "%ld",iz_min);
    #endif
		mes += buf;		

#ifdef __BORLANDC__
        iz_midle = (iz_max + iz_min)/2.0;
#else
		iz_midle = (DWORD)((iz_max + iz_min) / 2.0);
#endif

		mes += str_midle;
		memset(buf, 0, 10);
    #ifndef unix
		ltoa(iz_midle, buf, 10);
    #else
        sprintf(buf, "%ld",iz_midle);
    #endif
		mes += buf;


        mes += "\r\n";
        mes += str_delta;
		memset(buf, 0, 10);
    #ifndef unix
		_ltoa(iz_max - iz_min, buf, 10);
    #else
        sprintf(buf,"%ld", iz_max - iz_min);
    #endif
		mes += buf;				
        mes += str_diviation_from_midle;
        memset(buf, 0, 10);
        sprintf(buf, "%f", ((double)(iz_max - iz_min)/(iz_midle))* 100.0);
        mes += buf;
#endif
 return mes;
}
//---------------------------------------------------------------
int  file_Write(const char *name, const WORD *wtr, DWORD num)
{
 FILE *f;

 WORD ret;
 if((f = fopen(name, "wb")) == NULL)
 {
        test_SetStringTooOutDebug1("FileOpen-write", 1);
        return 1;
 }

if((ret = fwrite((const void*)wtr, num, 1, f)) != 1)
{
        test_SetStringTooOutDebug1("file_write", ret);
        return 2;
}
if(fclose(f) == EOF)
{
        test_SetStringTooOutDebug1("FileClose - write", 3);
        return 3;
}
return 0;
}

//--------------------------------------------------------------
int   file_Read(const char *name, BYTE **ptr,  DWORD *len)
{
 FILE *f;

 WORD ret;
 if((f = fopen(name, "rb")) == NULL)
 {
        test_SetStringTooOutDebug1("FileOpen -read", 1);
        return 1;
 }
        fseek(f, 0, SEEK_SET);
        fseek(f, 0, SEEK_END);
DWORD num = ftell(f);
        fseek(f, 0, SEEK_SET);
        *ptr = adapter_GetBufferAdres();
if((ret = fread((void*)*ptr, num, 1, f)) != 1)
{
        test_SetStringTooOutDebug1("file_read", ret);
        return 2;
}


if(fclose(f) == EOF)
{
        test_SetStringTooOutDebug1("FileClose -read", 3);
        return 3;
}

*len = num;
return 0;
}


//--------------------------------------------------------------
int   file_GetLengthFile(const char *name)
{
 FILE *f;


 if((f = fopen(name, "rb")) == NULL)
 {
        test_SetStringTooOutDebug1("file_GetLengthFile -open", 1);
        return 0;
 }
        fseek(f, 0, SEEK_SET);
        fseek(f, 0, SEEK_END);
DWORD num = ftell(f);
        fseek(f, 0, SEEK_SET);


if(fclose(f) == EOF)
{
        test_SetStringTooOutDebug1("file_GetLengthFile close", 3);
        return 0;
}

return num;
}



#ifdef __BORLANDC__
#pragma package(smart_init)
#endif
