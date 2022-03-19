
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
    #include "stdafx.h"
#endif

#include "Reduction_class.h"
#ifdef __BORLANDC__
	#include <vcl.h>
#endif

#include "test_dll.h"

 #include <math.h>

#ifdef _TEST
extern WORD test2[256][9];
WORD z;
__int64 glob;
#endif
AnsiString mes;


/***************************************************************************

***************************************************************************/
DWORD
REDUCTION::SetLevel(WORD *pWtr,  WORD level)
{
int i,j;
RDUC_DATA *rdt;

WORD *wtr = (pWtr + ((size_level_in_byte/2)) * level);

memset((void*)pMidl, 0L, sizeof(double) * num_adc);

double d = (double)num_pix_in_level / (double)num_adc;

double midle = 0.0;

WORD midl_of_level;
 midl_of_level = (WORD)(d/2);


        for( i = 0; i < num_adc; i++)
        {
        double db64 = 0.0;


                for( j = 0; j <  d; j++)
                {
                        if(j < l_front_reduct)  //9
                        {
                                db64 +=  (double)wtr[midl_of_level * num_adc + i];

                        }
                        else
                                if(j >= r_front_reduct)      //27
                                {
                                        db64 +=  (double)wtr[midl_of_level * num_adc + i];
                                }
                                else
                                        db64 +=  (double)wtr[j * num_adc + i ];
#ifdef _TEST
                glob++;
                test2[j][i] = wtr[j * num_adc + i];
#endif
                }

//By the end of the spiral, the meaning of the pixel right for each adcognition
                pMidl[i] = db64 / (binning == 3 ? ceil(d) : d);
//The dump of the middle of the pixmap is the pix of the adc on the datum of the midle +=  pMidl[i];
                midle += pMidl[i];

                }//for(int i = 0;

//Income from other countries (if compared to the previous one, the use of the pixel-by-wire line adc)
        midle = midle /((double)num_adc );
        rdt = Rduc_data(level);
        rdt->midl_loading_in_level = midle;
        for( i = 0; i < num_adc; i++)
        {
                if(pMidl[i] == 0.0)
                {
                        pMidl[i] += 1.0;
                AnsiString e;
                        e =   "Malfunction ADC";
                        e += i;
                        e +=  e += "; ";
                        for(int j = 0; j < num_adc; j++)
                        {



							#ifdef __BORLANDC__
								e += FormatFloat(0, pMidl[j]);
							#else
                                M_SPRINTF(e, pMidl[j])
							#endif
								e += ", ";
                        }
                Error("SetLevel",(char*)e.c_str());
                }
                rdt->deviation[i] = midle/pMidl[i];
        }
#ifdef _TEST
        mes += SetMessTest(level,rdt, num_adc);
#endif


#ifdef __BORLANDC__
		return midle;
#else
		return (DWORD)midle;
#endif
}

     
/**********************************************************************

********************************************************************/
RDUC_DATA*
REDUCTION::FindL(WORD w,WORD *level)
{
#ifdef _TEST
        z = 0;
#endif

        if(w <= midl_l)
        {
                *level = 0;
        return &rduc_data[0];
        }
        if(w >= midl_r)
        {
         *level = num_level - 1;
        return &rduc_data[num_level - 1];
        }

        for(int i = 0; i < num_level; i++)
        {
                if((w >= rduc_data[i].midl_loading_in_level) &&
                   (w < rduc_data[i + 1].midl_loading_in_level))
                {
                        if(abs(w - rduc_data[i].midl_loading_in_level) <
                           abs(w - rduc_data[i + 1].midl_loading_in_level))
                        {
                                *level = i;
                        return    &rduc_data[i];
                        }
                        else
                        {
                                *level = i + 1;
                        return    &rduc_data[i + 1];
                        }
                }
        }
return NULL;
}

/**********************************************************************

********************************************************************/
RDUC_DATA*
REDUCTION::FindR(WORD w, WORD *level)
{
#ifdef _TEST
        z = 0;
#endif
        if(w >= midl_l)
        {
                *level = 0;
        return &rduc_data[0];
        }
        if(w <= midl_r)
        {
                *level = num_level -1;
                return &rduc_data[num_level - 1];
        }

        for(int i = 0; i < num_level; i++)
        {
                if((w <= rduc_data[i].midl_loading_in_level) &&
                   (w > rduc_data[i + 1].midl_loading_in_level))
                {
                        if(abs(w - rduc_data[i].midl_loading_in_level) <
                           abs(w - rduc_data[i + 1].midl_loading_in_level))
                        {
                                *level = i;
                         return    &rduc_data[i];
                        }
                        else
                        {
                                *level = i + 1;
                        return    &rduc_data[i + 1];
                        }
                }
        }
return NULL;
}
/**********************************************************************

********************************************************************/
void
REDUCTION::Error(const char* s1, const char* s2, const char *s3)
{
AnsiString s = s1;
        if(s3 != NULL)
        {
        s += " ";
        s += s3;
        }

#ifdef __BORLANDC__
        MessageBox(NULL, s2, s.c_str(), MB_OK);
#else
    #ifndef  unix
		MessageBox(NULL, (LPCWSTR)s2, (LPCWSTR)s.c_str(), MB_OK);
    #else
        MessageBox(NULL, (LPCTSTR)s2, (LPCTSTR)s.c_str(), MB_OK);
    #endif
#endif
}

#ifdef _TEST
/***********************************************************************

***************************************************************************/
AnsiString REDUCTION::SetMessTest(WORD level, RDUC_DATA *rdt, WORD num_ad)
{
 AnsiString mes = "\r\n           level = ";
#ifdef __BORLANDC__
        mes += level;
#else
        M_SPRINTF(mes, level)
#endif
        for(int j = 0;j < num_ad; j++)
        {
                mes += "\r\n";
                for(int i = 0; i < num_pix_in_level / num_adc;i++)
                {
		#ifdef __BORLANDC__
                        mes += test2[i][j];
		#else
                        M_SPRINTF(mes, test2[i][j])
		#endif
                        mes += " ";
                        if(i == 15)
                                mes += "\r\n";
                }
                mes += "        midle[";
                mes += j;
                mes += "] = ";
#ifdef __BORLANDC__
                mes += pMidl[j];
#else

                M_SPRINTF(mes, pMidl[j])
#endif
        }

        mes += "\r\n";
        mes += "rdt->midle_loading_in_level = ";

#ifdef __BORLANDC__
		mes += rdt->midl_loading_in_level;
#else
		{
            M_SPRINTF(mes, rdt->midl_loading_in_level)
		}
#endif
        mes += "\r\n";
        for(int i = 0; i < num_ad; i++)
                {
                mes += "rdt->deviation[";
                mes += i;
                mes += "] = ";
#ifdef __BORLANDC__
                mes += rdt->deviation[i];
#else
                M_SPRINTF(mes, rdt->deviation[i])
#endif
                mes += "   ";
                }
        mes += "\r\n";
return mes;
}
#endif


//-------------------------------------------------------------------
//new
//-------------------------------------------------------------------
void REDUCTION::Init(WORD pix_of_string, WORD lf, WORD rg, BYTE binn )
 {


        binning = binn;
        num_pix_in_string = pix_of_string;

        if(pix_of_string == 1152)
        {
                switch(binning)
                {
                        case 1: GetNumLevels(32); num_string_of_reduction_data = 4; num_adc = 9; num_pix_in_string = (1152); break;
                        case 2: GetNumLevels(32); num_string_of_reduction_data = 8; num_adc = 9; num_pix_in_string = (1152);break;
                        case 3: GetNumLevels(36); num_string_of_reduction_data = 12;num_adc = 9; num_pix_in_string = (1152);break;
                        case 4: GetNumLevels(32); num_string_of_reduction_data = 16;num_adc = 9; num_pix_in_string = (1152);break;
                        default:
                                AnsiString e = "binning error - ";
                                e + "it should be:";
                                e += ": 1,2,3,4";
                                Error((char*)e.c_str(), "REDUCTION");
                                break;
                }
          }

          else
                if(pix_of_string == 576)
                {
                        switch(binning)
                        {                            //16                                                                    //576
                                case 1: GetNumLevels(32); num_string_of_reduction_data = 16; num_adc = 9; num_pix_in_string = (576); break;
                                case 2: GetNumLevels(32); num_string_of_reduction_data = 16; num_adc = 9; num_pix_in_string = (576);break;

                                default:
                                AnsiString e = "binning error - ";
                                e + "it should be:";
                                e += ": 1,2";
                                Error((char*)e.c_str(), "REDUCTION");
                                break;
                        }
                }

        if(num_pix_in_string == 2304)
        num_adc = 9;
        else
                if(num_pix_in_string == 2048)
                num_adc = 8;
                        else
                        if(num_pix_in_string == 1024)
                                num_adc = 8;
                                else
                                if((num_pix_in_string == 1152) ||
                                   (num_pix_in_string == (1152/2)) ||
                                   (num_pix_in_string == (1152/3)) ||
                                   (num_pix_in_string == (1152/4)))
                                        num_adc = 9;


        if(num_adc == 4)
                maska = 0x0003;
        else
                if(num_adc == 8)
                        maska = 0x0007;
        else
                 if(num_adc == 9)
                        maska = 0x000f;
                 else
                 {
                        AnsiString e = "Invalid amount of ADC: set - ";
                        e += num_adc;
                        e += "may be: 4,8,9";
                        Error((char*)e.c_str(), "REDUCTION");
                 }



        for(int i = 0; i < MAX_NUM_LEVELS; i++)
        {
                if(rduc_data[i].deviation != NULL)
                   delete[] rduc_data[i].deviation;
                rduc_data[i].deviation = NULL;
        }


        for(int i = 0; i < num_level; i++)
        {
                rduc_data[i].deviation = new double[num_adc];
                for(int j = 0; j <  num_adc; j++)
                        rduc_data[i].deviation[j] = 0.0;
                rduc_data[i].midl_loading_in_level = 0;
        }
#ifdef __BORLANDC__
        midl_l   = rduc_data[0].midl_loading_in_level;
        midl_r   = rduc_data[num_level - 1].midl_loading_in_level;
#else
		midl_l = (WORD)rduc_data[0].midl_loading_in_level;
        midl_r = (WORD)rduc_data[num_level - 1].midl_loading_in_level;
#endif
        num_pix_in_level = ((WORD)((num_pix_in_string * num_string_of_reduction_data)/ num_level));
        size_level_in_byte = binning == 3 ? 396 * 2 :  (WORD)(num_pix_in_level *  sizeof(WORD));
        if(pMidl != NULL)
                delete[] pMidl;
        pMidl = new double[num_adc];
        numWordReduction =  WORD(num_pix_in_string * num_string_of_reduction_data);
        l_front_reduct =  lf;
        r_front_reduct =  (num_pix_in_level / num_adc) - rg;
}
 
//--------------------------------------------------------------------------------
void REDUCTION::SetLianer()
{
WORD midl_l = Midl_l();
WORD midl_r = Midl_r();

        if(midl_l == midl_r)
        {
        AnsiString as = "ReductionGet";
                as += "rduc.Midl_l() == rduc.Midl_r() = ";
#ifdef __BORLANDC__
                as += midl_r;
#else
                M_SPRINTF(as,midl_r)
#endif
                Error("Error init", (char*)as.c_str());
        return;
        }

WORD delta_midl = abs(midl_r - midl_l);
WORD delta_level  = delta_midl / (num_level - 1);
WORD midl = (midl_l < midl_r) ? midl_l : midl_r;
        for(int i = 0, j =  (num_level - 1); i < num_level; i++, j-- )
        {
         int z = (midl_l < midl_r ? i : j);

                RDUC_DATA *rd = Rduc_data(z);
                WORD sm = midl + delta_level * i;
                double ln = sm / rd->midl_loading_in_level;
                Lianer(z, ln);
                Lianer(z, ln);
        }
}


//------------------------------------------------------------------------

//----------------------------------------------------------------------
void REDUCTION::GetMidleForBuferFromADC(WORD *pWtr, DWORD dNum, DWORD *sum)
{
__int64 sum64[9];
WORD *wtr;
DWORD num;
int i = 0;
int j = 0;

memset(sum, 0L, num_adc * sizeof(DWORD));
memset(sum64, 0L, num_adc * sizeof(__int64));

        if((dNum/num_pix_in_string) > 101)
        {
                wtr = pWtr + num_pix_in_string * 100; num = dNum - 200 * num_pix_in_string;
        }
        else
        {
                wtr = pWtr; num = dNum;
        }
int num_str = num/num_pix_in_string;
        WORD x = 0L;
        while(x < num_str )
        {
                for(i = 0; i < num_adc; i++)
                {
                  int n = num_pix_in_string == 1152 ? 128 : 256;
                        for(j = 0; j < n; j++)
                        {
                        WORD t = *(wtr + j * num_adc + i + x * num_pix_in_string);
                        sum64[i] += t;
                        }
                }
                x++;
          }
          num /= num_adc;
          for(i = 0; i < num_adc; i++)
          {
#ifdef __BORLANDC__
                sum[i] = sum64[i]/num;
#else
			    sum[i] = ((DWORD)sum64[i]) / num;
#endif
          }
}

#ifdef __BORLANDC__
//-------------------------------------------------------------------
AnsiString REDUCTION::SetReductDataToFile()
{

AnsiString mes = "\r\n\r\nDeteriorating the values of the faults and the connections from them:";


        for(int i = 0; i < /*NUM_LEVELS*/num_level; i++)
        {
         RDUC_DATA  *rdt = Rduc_data(i);
			mes += "\r\nLevel ";
			mes += i;
			mes += "   Midle:";
			mes += rdt->midl_loading_in_level;
			mes += "     The efficiency of non-linearity: ";
			mes += rdt->lianer; 	 		 		 		 
			mes += "\r\n";
            for(int j = 0; j < num_adc; j++)
            {
						mes += " ADC";
						mes += j;
						mes += ": Deviation:";
						mes += rdt->deviation[j];
						mes += ";    ";
                        if((j & 0x001) == 0x001)
                        {
                                mes += "\r\n";
                        }

             }

        }
		
  return mes;
}
#else
//-------------------------------------------------------------------
AnsiString REDUCTION::SetReductDataToFile()
{
 mes ="\r\n\r\nDeteriorating the values of the faults and the connections from them:";
 DWORD num;

        for(int i = 0; i < num_level/*NUM_LEVELS*/; i++)
        {
         RDUC_DATA  *rdt = Rduc_data(i);

			mes += "\r\nLevel ";

		 char buf[10];
			memset(buf, 0, 10);
#ifdef  unix
            sprintf(buf,"%d",i);
#else
			itoa(i, buf, 10);
#endif
			mes += buf;

			mes += "   Midle:";

        //    buf[10];
			memset(buf, 0, 10);
#ifdef __BORLANDC__
			itoa(rdt->midl_loading_in_level, buf, 10);
#else
    #ifdef unix
            sprintf(buf,"%f",rdt->midl_loading_in_level);
    #else
			itoa((int)rdt->midl_loading_in_level, buf, 10);
    #endif
#endif
			mes += buf;

			mes += "     The efficiency of non-linearity: ";

			memset(buf, 0, 10);
#ifdef __BORLANDC__
			itoa(rdt->lianer, buf, 10);
#else

    #ifdef unix
            sprintf(buf,"%d",(int)rdt->lianer);
    #else
			itoa((int)rdt->lianer, buf, 10);
    #endif
#endif
			mes += buf;
		 		 		 		 
			mes += "\r\n";

            for(int j = 0; j < num_adc; j++)
            {
						mes += " ADC";

				char buf[20];
						memset(buf, 0, 20);


#ifdef unix
                        sprintf(buf,"%d", j);
#else
						itoa(j, buf, 10);
#endif
						mes += buf;

						mes += ": Deviation:";

						memset(buf, 0, 20);
						sprintf(buf,"%f", rdt->deviation[j]);
						mes += buf;

						mes += ";    ";

                        if((j & 0x001) == 0x001)
                        {
                                mes += "\r\n";
                        }


						if(j == 0)
							num = mes.size();

						{
						DWORD 	sz_cap = mes.capacity();
						DWORD	sz_req = mes.size() + num;
								if(sz_req > sz_cap)
										mes.reserve(sz_req * 3);
						}

             }

        }

  return mes;
}


#ifdef unix
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
#endif


