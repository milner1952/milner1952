//---------------------------------------------------------------------------



#ifndef __BORLANDC__
    #define _CRT_SECURE_NO_WARNINGS

    #ifndef _CRT_SECURE_NO_DEPRECATE
        #define _CRT_SECURE_NO_DEPRECATE (1)
    #endif
    #ifndef unix
        #pragma warning(disable : 4996)
        #pragma warning(disable: 4800)
    #else
        #include <string.h>
    #endif

#else
#pragma hdrstop
#endif



#include <string>
#include "reg.h"

    #include "svaz.h"

#include "test_dll.h"
#define DATA_SCR  300

static DWORD gl_num;
char Vers[30];
extern REGDATA *regdata;
static WORD* End;
static WORD* Beg;
static WORD *Data;
static WORD mr[9][16];  //4

static WORD Rm[9];

MAIN_APP flagMainApp;
//#define TEST_REDUCT_DLL

#ifdef TEST_REDUCT_DLL
#define ERR_KF (1,0000001)
#endif
int Binning;
//-------------------------------------------------------------------------
SVAZ::_SVAZ()
{

}

//-------------------------------------------------------------------------
void SVAZ::Init(MAIN_APP ma, int binning)
{
 Binning = binning;
 flagMainApp = ma;
 memset(Vers,0, sizeof(Vers));
 gl_num = 0L;
 End = NULL;
 if(Iz_pxs_per_string == 2048)
        {

                memset(sscr, 0 , (4096 + 6) * 4096* sizeof(WORD));
                scr =  sscr;
                SetStage8ScrString();
                Set8_ION_2048_Data();

                Data = (WORD*)(scr  + 2048 * 6);
                for(int i = 0; i < 4096; i++)
                        SetSinus8ScrString(i);

                End = End;
                InitMem(1);


        }
        else
        {
         if(Iz_pxs_per_string == 2304)
                {

                        memset(sscr, 0 , (4096) * (4608 + 6)* sizeof(WORD));
                        scr =  sscr;
                        SetStage9ScrString();

                        Set9_ION_2304_Data();

                        Data = (WORD*)(scr  + 2304 * 6);

                        for(int i = 0; i < 4608; i++)
                                SetSinus9ScrString(i);
                         InitMem(1);

                }
                else
                if(Iz_pxs_per_string == 1152)
                {

                        memset(sscr, 0 , (1152 * 2) * (1152 * 2 + 6)* sizeof(WORD));
                        scr =  sscr;

                        switch (binning)
                        {
                         default:
                         case 1:
                        SetStage9_binn1_1152_ScrString();
                        Data = (WORD*)(scr  + 1152  * 6);
                        Set9_ION_1152_Data();    break;
                        case 3:
                        SetStage9_binn3_1152_ScrString();
                        Data = (WORD*)(scr  + 1152  * 13); break;

                         case 4:
                        SetStage9_binn4_1152_ScrString();
                        Data = (WORD*)(scr  + 1152  * 17); break;

                        }

                        for(int i = 0; i < 1152 * 2; i++)
                                SetSinus9_1152_ScrString(Data,i);
                         InitMem(1);

                }
                else
                {
                        scr =  sscr;
                        memset(scr, 0 , (1024 + 6) * 2024 * sizeof(WORD));
                        Set4ScrString(scr);

                        End -= ( 8 * 16 );
                        Beg = End;
                        End = Set4_ION(End, Iz_pxs_per_string);
                        End -=1;
                        for(int i = 0; i < 4096; i++)
                        {
                        Beg = End;
                        End = SetSin4ScrString(End);
                }

                }
     }           
}

//-------------------------------------------------------------------------
DWORD SVAZ::ReceiveBuffer(BYTE **ptr, DWORD len, int /*timeout*/)
{
        if(Iz_pxs_per_string == 2304)
        {
          memset(dst, 0 , 2304 * (2048 + 6));
          memcpy((void*)dst,(void*)(scr  +1 ),len);
        }
        else
        {
                if(Iz_pxs_per_string == 2048)
                {
                 memset(dst, 0 , 2048 * (2048 + 6));
                 memcpy((void*)dst,(void*)(scr - 1),len);
                }
                else

                if(Iz_pxs_per_string == 1152)
                {
                 memset(dst, 0 , 1152 * (1152 + 6));

                 memcpy((void*)dst,(void*)(scr
#ifdef MEDIAN1
                                      // + 39
#endif
#ifdef MEDIAN2
                                        + 1
#endif
                                          ),len);
#ifdef TEST_REDUCT_DLL
                 WORD *wtr = (WORD*)(dst

        #ifdef MEDIAN1
                                       + 39
        #endif
        #ifdef MEDIAN2
                                        + 8     // 64
        #endif
                                                );
                 DWORD i;
                 WORD m;

double n[9] = {1.0085, 1.003, 1.0035, 1.0055, 1.0075, 1.0050, 1.0040, 1.0020,1.0010};
                 for(i = 0, m = 0; i < len/2 ; i++, m++)
                 {
                       if(m > 8)
                                m = 0;

                  double t =    n[m];

                                wtr[i] = WORD((double)wtr[i] * t);

                 }
                 i = i;

#endif
                }
                else
                {
                memset(dst, 0 , 2048 * (1024));
                memcpy((void*)dst,(void*)((BYTE*)(scr) + 1),len);
                }
        }
        *ptr = ((BYTE*)dst);
  return len;
}

//-------------------------------------------------------------------------
char* SVAZ::Send(char *ptr)
{
if(Iz_pxs_per_string == 1024)
        {
                {
                        strcpy(Vers, "1024 4.2.1 No connection");
                }
        }
        else
        {
        if(Iz_pxs_per_string == 2048)
        {
                if(!strncmp("P_KAMERA_CMD_SET_DAC",ptr, strlen("P_KAMERA_CMD_SET_DAC")))
                {
                string s(ptr);
                string:: size_type b =  s.find(" ",0);
                        if(b != string::npos)
                                b++;
                string:: size_type e =  s.find(" ",b);
                        if(e != string::npos)
                                e++;
                string km(s,b,e - b);
                int k = atoi(km.c_str());
                        InitMem(k);
                }
                else
                {
                        if(flagMainApp != APP_NEYSTROEV)
                        {
                                if(flagMainApp != APP_2048_MAIN)
                                        strcpy(Vers, "2048 4.2.1 No connection");
                                else
                                        strcpy(Vers, "2048 5.2.1 No connection");
                        }
                        else
                                strcpy(Vers, "2048 5.2.1 No connection");
                }
        }
        else
                if(Iz_pxs_per_string == 2304)
        {
                if(!strncmp("P_KAMERA_CMD_SET_DAC",ptr, strlen("P_KAMERA_CMD_SET_DAC")))
                {
                string s(ptr);
                string:: size_type b =  s.find(" ",0);
                        if(b != string::npos)
                                b++;
                string:: size_type e =  s.find(" ",b);
                        if(e != string::npos)
                                e++;
                string km(s,b,e - b);
                int k = atoi(km.c_str());
                        InitMem(k);
                }
                else
                {
                                strcpy(Vers, "2304 9.0.0 No connection");
                }
        }
       }
 return Vers;
}
//-------------------------------------------------------------------
void SVAZ::SendArpPacket(){
}

#define msc 0x0003ff
#define msc_t 0x100
#define msc_e 0x00ff


#define _32 32 * 15
//----------------------------------------------------------------

void SVAZ::SetStage8ScrString()
{
WORD *sc = (WORD*)(scr + 2048);
WORD r[8];

srand(0);

for(int j = 0; j < 4; j ++)
{//1
for(int m = 0; m < 8; m++)
{
   r[m] = mr[m][j] = (WORD)(msc & rand());
}

sc = sc;

for(int s = 0; s < 8 ; s ++)
{ //2
for(int x = 0; x < 256; x++)
{ //3
for(int m = 0; m < 8; m++)
   {//4
   if((r[m] & msc_t) == 0)
   {
            sc[m + x * 8 + s * 256 + j * 2048] =  65536 -
            (WORD)(_32* (s + 1) * 4 + (j * _32 * 8) * 4  + (r[m] & msc_e) );
   }
   else
   {
            sc[m + x * 8 + s * 256 + j * 2048] =   65536 -
            (WORD)(_32* (s + 1) * 4 +  (j * _32 * 8) * 4  - (r[m] & msc_e) );
   }
} //4
} //3
} //2
} //1
}



//----------------------------------------------------------------------------------
void SVAZ::SetSinus8ScrString(int i)
{
WORD d;
        WORD *sc = (WORD*)(scr  + 2048 * 6 + i * 2048);
        d = 0L;

        for(int m = 0; m < 8; m++)
        {
                for(int p = 0; p < 256; p++)
                {
                  sc[m + p * 8 ] = (WORD)(d++);

                }

         }
}

//----------------------------------------------------------------------------------
WORD* SVAZ::SetSin4ScrString(WORD *end)
{
DWORD d;
int m, p;
WORD *sc = end;
       d = 0L;
        for( m = 0; m < 4; m++)
        {
                for( p = 0; p < 256; p++, d++)
                {
                 sc[m  + p * 4 + 1] = (WORD)(d);
                }
        }

return &sc[m  + p * 4 - 4];
}


//---------------------------------------------------------------------------------
void SVAZ::Set4ScrString(WORD *psc)
{
int i;
int j;
int z;
int m;
WORD *sc = psc;
int q0,q1,q2,q3;
        srand(0);

        for(m = 0; m < 4; m++)
        switch(m)
        {
                case 0:       q0 = 0x003f & rand();   break;
                case 1:       q1 = 0x003f & rand();   break;
                case 2:       q2 = 0x003f & rand();   break;
                case 3:       q3 = 0x003f & rand();   break;
                default: break;
        }

        for(j = 0, z = 16; j < 32; j++, z ++)
                for(m = i = 0; i < 128; i ++,m++)
                {
                int q;
                        if(m > 3)
                                m = 0;
                        switch(m)
                        {
                                case 0:       q = q0;   break;
                                case 1:       q = q1;   break;
                                case 2:       q = q2;   break;
                                case 3:       q = q3;   break;
                                default:       break;
                        }

                        if((q & 0x20) == 0)
                                sc[(z * 128) + i + 1 ] = ((j + 1) * 32 * 16) + q;
                        else
                                sc[(z * 128) + i + 1 ] = ((j + 1) * 32 * 16) - q;
                }
        End = (WORD*)&sc[(z * 128) + i + 1 ];
        gl_num  =  (z * 128) + i + 1;
}
//---------------------------------------------------------------------
//
//---------------------------------------------------------------------
WORD* SVAZ::Set4_ION(WORD * end, WORD pix_in_string)
{
WORD *ptr = end;
WORD w[4];
int i, j;
        for( i = 0; i < 4; i++)
                w[i] = 0x003f & rand();
        for( i = 0, j = 0; i < pix_in_string; i++, j++)
        {
                if(j == 4)
                        j = 0;
                *ptr = (0x1fff);

                {
                if((w[j] & 0x0020) == 0)
                        *ptr += w[j];
                 else
                        *ptr -= w[j];
        }
        ptr++;
}
return ptr;
}
//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
WORD* SVAZ:: Set8_ION_2048_Data()
{
 WORD *sc = (WORD*)(scr  + 2048 * 5);
 WORD d[8];
 srand(1);
        for(int i = 0; i < 8; i++ )
        {
                d[i] = 0x0001f & (rand() * rand());
        }
                for(int m = 0; m < 8; m++)
                        for(int p = 0; p < 256; p++)
                        {
                                if((d[m] & 0x0008) == 0x0008)
                                        sc[m + p * 8 ] = (WORD)(0x1fff + d[m]);
                                else
                                        sc[m + p * 8 ] = (WORD)(0x1fff - d[m]);
                        }
 return NULL;
}

//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
WORD* SVAZ:: Set9_ION_2304_Data()
{
 WORD *sc = (WORD*)(scr  + 2304 * 5);
 WORD d[9];
 srand(1);
        for(int i = 0; i < 9; i++ )
        {
                d[i] = 0x0001f & (rand() * rand());
        }
                for(int m = 0; m < 9; m++)
                        for(int p = 0; p < 256; p++)
                        {
                                if((d[m] & 0x0008) == 0x0008)
                                        sc[m + p * 9 ] = (WORD)(0xdfff + d[m]);
                                else
                                        sc[m + p * 9 ] = (WORD)(0xdfff - d[m]);
                        }
 return NULL;
}

//------------------------------------------------------------------------------------
void InitMem(int k)
{

 int dl = 65536/11;
 int d = dl * (k
 //+ 1
 );


 if(Iz_pxs_per_string == 2048)
 {

        for(int y = 0; y < 6; y ++)
        {
        WORD *sc = (WORD*)(Data + y * 2048);

                for(int m = 0; m < 8; m++)
                {
                        for(int i = 0; i < 256; i++)
                        {
                        *(sc + i * 8 + m) = d;
                        }
                }
        }
 }
 else
  if(Iz_pxs_per_string == 1152)
 {
 int Y = 6;
 if(Binning == 4)
        Y = 2;

        for(int y = 0; y < Y; y ++)
        {
        WORD *sc = (WORD*)(Data + y * 1152);

                for(int m = 0; m < 9; m++)
                {
                        for(int i = 0; i < 128; i++)
                        {
#ifndef TEST_REDUCT_DLL
                        *(sc + i * 9 + m) =  d;
#else
                        *(sc + i * 9 + m) =  d;
#endif
                        }
                }
        }
 }
 else
 {

        for(int y = 0; y < 6; y ++)
        {
        WORD *sc = (WORD*)(Data + y * 2304);

                for(int m = 0; m < 9; m++)
                {
                        for(int i = 0; i < 256; i++)
                        {
                        *(sc + i * 9 + m) = d;
                        }
                }
        }
 }

}

//------------------------------------------------------------------------
void SVAZ:: SetTestNoiseData(WORD *wtr, MAIN_APP flag_main_app , bool iz_svaz_udp,
                                int pixsels_per_string, bool /*with_matrices*/, DWORD pix_num)
{
static bool init = 0;
int num = 0;
        if((pixsels_per_string != 2048) && (pixsels_per_string != 2304))
                return;

        #ifndef NO_XRY   //
                if(iz_svaz_udp == 1)
                return;
        #endif

        switch(flag_main_app)
                {
                case   APP_N_PRIBOR:
                #ifndef NO_XRY   //
                        if(iz_svaz_udp == 1)
                        return;
                #endif
                        num = 100; 
                        break;

                case   APP_1024_MAIN:
                case   APP_2048_MAIN:
                case   APP_2304_MAIN:
                        num = 4;
                        break;

                case   APP_NEYSTROEV:
                #ifndef NO_XRY
                        if(iz_svaz_udp == 1)
                        return;
                #endif
                        if(pix_num < 150000)
                                num = pix_num / pixsels_per_string;

                        else
                                return;
                break;

                default: test_SetStringTooOutDebug3("ERROR in svaz.SetTestNoiseData",
                                                        (unsigned char*)"ERROR of type_APP");break;
                }
static WORD r0;
static WORD r[9];
static WORD u = 0;
WORD wt[9];
WORD n = 0;
        n = pixsels_per_string == 2304 ? 9 : pixsels_per_string == 2048 ? 8 : 4;

        if(init == 0)
        {
                r0 = (0x03f & rand());
                for(int i = 0; i < n; i++)
                        r[i] =  ((0x03f & rand()) + r0);
        }
        srand(u);
		int i;
        for( i = 0; i < n; i++)
                wt[i] =  ((0x01f & rand()));

        for(i = 0; i <  pixsels_per_string * num; i += n)
                {
                for(int j = 0; j < n; j++)
                        wtr[i + j] = (r[j] + (0x001f & rand()) + wt[j]);
                }
        init = 1;
        u++;
}

             /*
#define msc 0x0007f
#define msc_t 0x020
#define msc_e 0x001f
               */
#define _32_ 32 * 14 //14
//----------------------------------------------------------------
 WORD db = 0;
void SVAZ::SetStage9ScrString()
{
WORD *sc = (WORD*)(scr + 2304);
WORD r[9];

srand(0);

for(int j = 0; j < 4; j ++)
{//1
for(int m = 0; m < 9; m++)
{
   r[m] = mr[m][j] = (WORD)(msc & rand());
}

sc = sc;

for(int s = 0; s < 8 ; s ++)    //9
{ //2
for(int x = 0; x < 256; x++)
{ //3
for(int m = 0; m < 9; m++)
   {//4

   if((r[m] & msc_t) == 0)
   {
            sc[m + x * 9 + s * 288 + j * 2304] =  65536 -
            (WORD)(_32_* (s + 1) * 4 + (j * _32_ * 9) * 4  + (r[m] & msc_e)
            );
   }
   else
   {
            sc[m + x * 9 + s * 288 + j * 2304] =   65536 -
            (WORD)(_32_* (s + 1) * 4 +  (j * _32_ * 9) * 4  - (r[m] & msc_e)
             );
   }

} //4
} //3
} //2
} //1
}

//----------------------------------------------------------------------------------
void SVAZ::SetSinus9ScrString(int i)
{
WORD d;
        WORD *sc = (WORD*)(scr  + 2304 * 6 + i * 2304);
        d = 0;
        sc += 1;

         for(int m = 0; m < 9; m++)
        {
                for(int p = 0; p < 256; p++)
                {
                 sc[m + p * 9 ] = d;
                 d++;
                }

         }
}

//-------------------------------------------------------
void SVAZ::SetStage9_binn1_1152_ScrString()
{

db = 0;
WORD *sc = (WORD*)(scr + 1152);
WORD r[9];

srand(0);

for(int j = 0; j < 4; j ++)
{//1
for(int m = 0; m < 9; m++)
{
   Rm[m] = r[m] = mr[m][j] = (WORD)(msc & rand());
}


sc = sc;

for(int s = 0; s < 8 ; s ++)    //8
{ //2
for(int x = 0; x < 128; x++)
{ //3
for(int m = 0; m < 9; m++)
   {//4
#ifndef TEST_REDUCT_DLL
   if((r[m] & msc_t) == 0)
   {
            sc[m + x * 9 + s * 144 + j * 1152] =  65536 -
            (WORD)(_32_* (s + 1) * 4 + (j * _32_ * 9) * 4  + (r[m] & msc_e)
            );
   }
   else
   {
            sc[m + x * 9 + s * 144 + j * 1152] =   65536 -
            (WORD)(_32_* (s + 1) * 4 +  (j * _32_ * 9) * 4  - (r[m] & msc_e)
             );
   }
#else
/*
              sc[m + x * 9 + s * 144 + j * 1152] =  (65536 -
            (WORD)(_32_* (s + 1) * 4 + (j * _32_ * 9) * 4)  * ( m *ERR_KF));
            */

              sc[m + x * 9 + s * 144 + j * 1152] =  65536 -
            (WORD)(_32_* (s + 1) * 4 + (j * _32_ * 9) * 4);

#endif
} //4
} //3
} //2
} //1
}

//----------------------------------------------------------------
void SVAZ::SetStage9_binn3_1152_ScrString()
{
/*
#ifdef msc_e
#undef msc_e

#define msc_e 0x000ff
#endif
  */
#define _DLT3_ ((2500/3)*2)
db = 0;
WORD *sc = (WORD*)(scr + 1152 + 1);
WORD r[9];
srand(0);
for(int j = 0; j < 12; j ++)
{//1
for(int m = 0; m < 9; m++)
{
   Rm[m] = r[m] = mr[m][j] = (WORD)(msc & rand());
}
for(int s = 0; s < 3 ; s ++)
{ //2
for(int x = 0; x < 128; x++)
{ //3
        for(int m = 0; m < 9; m++)
        {//4
           if((r[m] & msc_t) == 0)
           {
            sc[m + x * 9 + s * 384  + j * 1152] =
            65536 - ((WORD)(_DLT3_*(s + 1) + _DLT3_*j *3)) - (r[m] & msc_e);
           }
           else
           {
            sc[m + x * 9 + s * 384  + j * 1152] =
            65536 - ((WORD)(_DLT3_*(s + 1) + _DLT3_*j *3)) + (r[m] & msc_e);
           }
        } //4
} //3
} //2
} //1
}

//----------------------------------------------------------------
void SVAZ::SetStage9_binn4_1152_ScrString()
{
#define _DLT_ 2000
db = 0;
WORD *sc = (WORD*)(scr + 1152 + 1);
WORD r[9];
srand(0);
for(int j = 0; j < 16; j ++)
{//1
for(int m = 0; m < 9; m++)
{
   Rm[m] = r[m] = mr[m][j] = (WORD)(msc & rand());
}
for(int s = 0; s < 2 ; s ++)
{ //2
for(int x = 0; x < 128; x++)
{ //3
        for(int m = 0; m < 9; m++) 
        {//4
           if((r[m] & msc_t) == 0)
           {
            sc[m + x * 9 + s * 576  + j * 1152] =
            65536 - ((WORD)(_DLT_*(s + 1) + _DLT_*j *2)) - (r[m] & msc_e);
           }
           else
           {
            sc[m + x * 9 + s * 576  + j * 1152] =
            65536 - ((WORD)(_DLT_*(s + 1) + _DLT_*j *2)) + (r[m] & msc_e);
           }
        } //4
} //3
} //2
} //1
}


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
WORD* SVAZ:: Set9_ION_1152_Data()
{
 WORD *sc = (WORD*)(scr  + 1152 * 5);
 WORD d[9];
 srand(1);
        for(int i = 0; i < 9; i++ )
        {
                d[i] = 0x0001f & (rand() * rand());
        }
                for(int m = 0; m < 9; m++)
                        for(int p = 0; p < 128; p++)
                        {
                                if((d[m] & 0x0008) == 0x0008)
                                        sc[m + p * 9 ] = (WORD)(0xdfff + d[m]);
                                else
                                        sc[m + p * 9 ] = (WORD)(0xdfff - d[m]);
                        }
 return NULL;
}

//----------------------------------------------------------------------------------
void SVAZ::SetSinus9_1152_ScrString(WORD* begin, int i)
{
WORD d;
        WORD *sc = begin + i * 1152;
        d = 0;
        sc += 1;

         for(int m = 0; m < 9; m++)
        {
                for(int p = 0; p < 128; p++)
                {
                 sc[m + p * 9 ] = d;
                 d++;
                }

         }
}
#ifdef __BORLANDC__
#pragma package(smart_init)
#endif




