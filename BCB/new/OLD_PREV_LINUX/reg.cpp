//---------------------------------------------------------------------------
//


#pragma hdrstop

#ifndef __BORLANDC__
	#define _CRT_SECURE_NO_WARNINGS
	#ifndef _CRT_SECURE_NO_DEPRECATE
		# define _CRT_SECURE_NO_DEPRECATE (1)
	#endif 
	#pragma warning(disable : 4996)
	#pragma warning(disable: 4800)
#endif

#include "reg.h"
#include <string>
using namespace std;
TRegistry *reg;

const char *RegKey ;
char **   reg_str_param_name;
static int num_param;
REGDATA d;


//---------------------------------------------------------------------------
TRegistry* InitReg(const char *Key, char **Str_param_name, int num)
{
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
	reg = new TRegistry();
#endif
  reg->RootKey = HKEY_CURRENT_USER;
  RegKey =  Key;    //"Software\\N_PRIBOR_P_KAMERA_2048_DLL_SRK\\ParamList";

  reg_str_param_name = Str_param_name; // {"TIMER_PERIOD","GANE","PIXELS_PER_STRING","NORM_DATA",.....
  num_param = num;
  memset(&d, 0, sizeof(REGDATA));
 if(reg == NULL)
	return NULL;
 else
	return reg;
}
//------------------------------------------------------
void DestroyReg()
{
 delete reg;
}


//---------------------------------------------------------------------------
REGDATA* ReadParamTooReg()
{

const AnsiString  ch(RegKey/* "Software\\N_PRIBOR_P_KAMERA_2048_DLL_SRK\\ParamList"; */);

   if(!reg->OpenKey(ch,false))
    {
       reg->OpenKey(ch,true);
       for(int i = 0;i < num_param; i++)
       {
          reg->WriteString(reg_str_param_name[i],"");
          switch(i)
          {

           case 0:
           reg->WriteString(reg_str_param_name[i],"10");
           break;

           case 3:
           {
            string s;
             for(int j  = 0;j < 2048; j++)
             {
              s += "1,0 ";
             }
             reg->WriteString(reg_str_param_name[i],s.c_str());
           }
           break;

           case 2:
           reg->WriteString(reg_str_param_name[i],"1152");
           break;
           case 4:
            reg->WriteString(reg_str_param_name[i],"0");
           break;

           case 5:
           reg->WriteString(reg_str_param_name[i],"0000");
           break;

           case 6:
            reg->WriteString(reg_str_param_name[i],"0");
            break;

           case 7:
            reg->WriteString(reg_str_param_name[i],"1");
            break;
           case 8:
            reg->WriteString(reg_str_param_name[i],"1");
           break;

           case 15:
            reg->WriteString(reg_str_param_name[i],"5");
           break;


           case 16:
            reg->WriteString(reg_str_param_name[i],"9");
           break;

           case 19:
            reg->WriteString(reg_str_param_name[i],"9");
           break;



           case 20:
            reg->WriteString(reg_str_param_name[i],"5,0");
           break;

           case 23:
            reg->WriteString(reg_str_param_name[i],"1");
           break;

           default :
           reg->WriteString(reg_str_param_name[i],"0");
           break;
          }
       }
    }

//-----------------------
        AnsiString as = reg->ReadString(reg_str_param_name[0]);
		#ifdef __BORLANDC__
			try
			{
                        int t = as.ToInt();
		        d.timer_period = t;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert TIMER PERIOD");
			}
		#else
				d.timer_period = atoi(as.c_str());
		#endif

//------------------------
         as  = reg->ReadString(reg_str_param_name[1]);
		#ifdef __BORLANDC__
			try
			{
				d.gane = as.ToInt();
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert GANE");
			}
		#else
				d.gane = atoi(as.c_str());
		#endif

//-------------------------
//int tp;
         as  = reg->ReadString(reg_str_param_name[2]);
		#ifdef __BORLANDC__
			try
			{
				d.pixsels_per_string =  as.ToInt();
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert PIXELS_PER_STR");
			}
			#else
				d.pixsels_per_string =  atoi(as.c_str());
			#endif
//--------------------------------------------------------------------

         as  = reg->ReadString(reg_str_param_name[3]);

//----------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[4]);
		#ifdef __BORLANDC__
			try
			{
				d.save_in_bufer_in_file_or_read_it = as.ToInt();
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - SAVE_IN_BUFFER_IN_FILE_OR_READ_IT");
			}
			#else
				d.save_in_bufer_in_file_or_read_it  = atoi(as.c_str());
			#endif

//----------------------------------------------------------------------
			as = reg->ReadString(reg_str_param_name[5]);
		#ifdef __BORLANDC__
			try
			{
				d._5ms_per_string = (as.ToInt() == 0 ? 0 : 1);
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert 5MC_PER_STRING");
			}
			#else
				d._5ms_per_string  = atoi(as.c_str());
			#endif

        //----------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[6]);
		#ifdef __BORLANDC__
			try
			{
                                int t = as.ToInt();
                                if((t == 2) || (t == 3))
				        d.iz_log_debug = 2;
                                if(t == 3)
                                d.iz_icmp_test = 1;
                                else
                                d.iz_icmp_test = 0;
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert IZ_LOG_DEBUG");
			}
			#else
				d.iz_log_debug  = atoi(as.c_str());
			#endif
        //----------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[7]);
		#ifdef __BORLANDC__
			try
			{
				d.iz_reduction = as.ToInt() == 0 ? false : true;
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert IZ_REDUCTION");
			}
			#else
			d.iz_reduction  = atoi(as.c_str()) == 0 ? 0 : 1;
			#endif
        //----------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[8]);
		#ifdef __BORLANDC__
			try
			{
				d.iz_svaz_udp = as.ToInt() == 0 ? false : true;
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert IZ_SVAZ_UDP");
			}
			#else
				d.iz_svaz_udp  = atoi(as.c_str())== 0 ? 0 : 1;
			#endif
		//----------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[9]);
		#ifdef __BORLANDC__
			try
			{
				d.mtr_gane = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert MTR_GANE");
			}
			#else
				d.mtr_gane  = atoi(as.c_str());
			#endif
		//--------------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[10]);
		#ifdef __BORLANDC__
			try
			{
				d.in_file_off_byffer = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert IN_FILE_OFF_BUFFER");
			}
			#else
				d.in_file_off_byffer  = atoi(as.c_str())== 0 ? 0 : 1;
			#endif
	
		//-----------------------------------------------------------------------
		as = reg->ReadString(reg_str_param_name[15]);
		#ifdef __BORLANDC__
			try
			{
				d.l_front_reduct = as.ToInt() ;
			}
				catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert LEFT_REDUCT_FRONT");
			}
		#else
			d.l_front_reduct = atoi(as.c_str());
		#endif
		//-------------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[16]);
		#ifdef __BORLANDC__
			try
			{
				d.r_front_reduct = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
				OutputDebugString("Error - Konvert RIGHT_REDUCT_FRONT");
			}
			#else
				d.r_front_reduct  = atoi(as.c_str());
			#endif

		//------------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[17]);
		#ifdef __BORLANDC__
			try
			{
				d.if_rotation = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert IF_ROTATION");
			}
			#else
				d.if_rotation  = atoi(as.c_str())== 0 ? 0 : 1;
			#endif
		//----------------------------------------------------------------------
        as = reg->ReadString(reg_str_param_name[18]);
		#ifdef __BORLANDC__
			try
			{
				d.with_noise_statistics = as.ToInt() ;
			}			
                        catch(EConvertError &e)
                        {
                        OutputDebugString("Error - Konvert WITH_NOISE_STATISTICS");
                        }
		        #else
				 d.with_noise_statistics  = atoi(as.c_str())== 0 ? 0 : 1;
	        	#endif

		//-----------------------------------------------------------------------
				as = reg->ReadString(reg_str_param_name[19]);
		#ifdef __BORLANDC__
			try
			{
				d.with_matrices = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert WITH_MATRICES");
			}
		#else
				d.with_matrices  = atoi(as.c_str())== 0 ? 0 : 1;
		#endif



				as = reg->ReadString(reg_str_param_name[20]);
		#ifdef __BORLANDC__
			try
			{
				d.timer_repit = as.ToDouble() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert TIME_REPIT");
			}
		#else
		d.timer_repit = atof(as.c_str());
		#endif


                 as = reg->ReadString(reg_str_param_name[21]);
		#ifdef __BORLANDC__
			try
			{
				d.if_revers_matrix = as.ToDouble() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert REVERS_MATRIX");
			}
		#else
				d.if_revers_matrix  = atof(as.c_str());
		#endif


                as = reg->ReadString(reg_str_param_name[22]);
		#ifdef __BORLANDC__
			try
			{
				d.iz_dac = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert IZ_DAC");
			}
		#else
				d.iz_dac  = atoi(as.c_str());
		#endif


                        as = reg->ReadString(reg_str_param_name[23]);
		#ifdef __BORLANDC__
			try
			{
				d.iz_binning = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert IZ_BINNING");
			}
		#else
				d.iz_binning  = atoi(as.c_str());
		#endif


                                as = reg->ReadString(reg_str_param_name[24]);
		#ifdef __BORLANDC__
			try
			{
				d.iz_h_filter = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert IZ_FILTER");
			}
		#else
				d.iz_h_filter  = atoi(as.c_str());
		#endif


//-----------------------------------------------------------------------------------
                        as = reg->ReadString(reg_str_param_name[25]);
		#ifdef __BORLANDC__
			try
			{
				d.div_16_9_4 = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert IZ_DIV_4");
			}
		#else
				d.div_16_9_4  = atoi(as.c_str());
		#endif

                        as = reg->ReadString(reg_str_param_name[26]);
		#ifdef __BORLANDC__
			try
			{
				d.div_4_3_2 = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert IZ_DIV_2");
			}
		#else
				d.div_4_3_2  = atoi(as.c_str());
		#endif

                        as = reg->ReadString(reg_str_param_name[27]);
		#ifdef __BORLANDC__
			try
			{
				d.iz_range = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert IZ_RANGE");
			}
		#else
				d.iz_range  = atoi(as.c_str());
		#endif

                        as = reg->ReadString(reg_str_param_name[28]);
		#ifdef __BORLANDC__
			try
			{
				d.range = as.ToInt() ;
			}
			catch(EConvertError &e)
			{
			OutputDebugString("Error - Konvert RANGE");
			}
		#else
				d.range  = atoi(as.c_str());
		#endif




   reg->CloseKey();



   if(d.timer_period < 7)
       {
        d.timer_period = 7;
#ifdef __BORLANDC__
        AnsiString as = d.timer_period;
        WriteParamTooReg(reg_str_param_name[0], as.c_str());
#else
		 char b[20];
		 memset(b, 0, 20);
		 _itoa(d.timer_period, b, 10);
		 AnsiString as(b);
		 WriteParamTooReg(reg_str_param_name[0], (char*)as.c_str());
#endif
       }


   return &d;
}

//---------------------------------------------------------------------------
void WriteParamTooReg(const char* Param, char *Value)
{
	reg->OpenKey(RegKey,true);
	reg->DeleteValue(Param);
	reg->WriteString(Param,Value);
	reg->CloseKey();
}

#ifdef __BORLANDC__
#pragma package(smart_init)
#endif

