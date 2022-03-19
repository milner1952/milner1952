
// 0xf8e388d5 ID = ffff 14.11.2015 - версия для камеры  1024,2048,2034 (V2)

// 0xf99a27d2 ID = ffff 21.11.2015 - старая версия камеры 2048 (V1), считано из hex файла


//	v2.0.01: в ф.p_kamera_PutData(..., void p_kamera_PutBufer(.. изменена реализация switch(VersData & 0x7fff)...	
//заблокированно переключение времени накопления, введены разные версии для прошивок АЛЬТЕРЫ и PIC 


#include "TCPIP Stack/TCPIP.h"
#include "TCPIPConfig.h"
#include "TCPIP Stack/StackTsk.h"
#if defined(STACK_USE_UDP)
	#include "TCPIP Stack/UDP.h"
#endif  
#include "GenericTypeDefs.h"

#include <string.h>
#include <stdlib.h>
#include "TCPIP Stack/p_kamera_udp.h"
#include "TCPIP Stack/Rs232_1.h"




#define P_KAMER_PORT_RX 2048
#define P_KAMER_PORT_TX 2049

#define NUM_SOCKET_P_KAMERA 8
#define MAX_STRING_RESIV 20

#define MAX_NUM_ARGUMENTS 5
#define NUM_P_KAMERA_KOMMAND 6


//const char VersString[] = " PIC: v2.0.01"; //в ф.p_kamera_PutData(..., void p_kamera_PutBufer(.. изменена реализация switch(VersData & 0x7fff)...	
                                           //заблокированно переключение времени накопления, введены разные версии для прошивок АЛЬТЕРЫ и PIC  
										   // ф. p_kamera_Icmp_error_proc()	 - заглужка

const char VersString[] = " PIC: v2.0.02"; //Добавлена поддержка камеры 1024 в отдельных версиях камер.
											// Разблокированно переключение времени накопления для 1024 камеры
											// изменено использование сигнала VERS_DT_IO
											// Прекращена поддержка первой версии v9.0.0 прошивки АЛТ и PIC для камеры 2304 pix
											// 
typedef  enum _P_KAMERA_COMMAND
{
 P_KAMERA_CMD_GET,
 P_KAMERA_CMD_GANE_MTR,
 P_KAMERA_CMD_5MC_2_MC_FLIP_FLOP,
 P_KAMERA_CMD_MODE,
 P_KAMERA_CMD_GET_VERSION,
 P_KAMERA_CMD_SET_DAC,
 INVALID_P_KAMERA_COMMAND = 0xff 
}P_KAMERA_COMMAND; 

ROM char *P_KameraCommandString[] = 
{
 "GET","GANE_MTR","MC_FLIP_FLOP","MC_MODE","MC_GET_VERSION","P_KAMERA_CMD_SET_DAC"
};
static ROM char  *chAppType[] = {"APP_NEYSTROEV", "APP_1024_MAIN", "APP_2048_MAIN", "APP_N_PRIBOR" ,"APP_END_NUMBERING"};
typedef enum{MP_BEGIN, MP_RUN1, MP_RUN2 }MODE_PUT_P_KAMERA;


P_KAMERA_COMMAND p_kamera_command;
BYTE temp_data[_MEM_ * 2];
UDP_SOCKET p_kamera_socket[NUM_SOCKET_P_KAMERA];
NODE_INFO remote_node;
BYTE rs_bufer[MAX_STRING_RESIV];
BYTE *arguments[MAX_NUM_ARGUMENTS];
BYTE adjustment;
static MODE_PUT_P_KAMERA mode;
extern UDP_SOCKET_INFO  UDPSocketInfo[];
extern BYTE temp_data[];
static UDP_SOCKET actual_socket;
char buferr[30];
long l_num;
BYTE iff_init = 0;
MAIN_APP main_app;
DWORD dw_num_oll;
char VersDataASC[21];
BYTE VersDataHex;

void p_kamera_ParseString(BYTE *ptr, WORD len);
P_KAMERA_COMMAND p_kamera_ParseCommand(BYTE ** arg);
void p_kamera_PutBufer(UDP_SOCKET s, DWORD len, BYTE *bufer);
void p_kamera_PutData(UDP_SOCKET s, DWORD len, BYTE *bufer);
WORD UDPPutMemory(BYTE *cData, WORD wDataLen);
void ENC_SetBufer(BYTE *bufer, WORD len);
void p_kamera_PutVersion(UDP_SOCKET s, char *bufer, WORD len);
MAIN_APP p_kamera_ParseArgument(BYTE ** arg);
WORD  Encx24j600_GetVersion();
extern WORD  t_global;
#ifdef _RS232_DEBUG_
 	extern BYTE flag_test_UDP_RS232 ;
 	void p_kamera_StringSinxro(long num);
 	extern char buferr[];
 	extern  int indexx;
#else
	void p_kamera_StringSinxro();
#endif


#ifndef  MIT_AIN_BLOKS
	DWORD 	dw_test_data;
	WORD 	wLewel;
	WORD    r_d[8];
	int global_index;
#endif

//-------------------------------------------------
//
//--------------------------------------------------
void p_kamera_InitAltera144()
{
int i;
 _5MC_2_MC_FLIP_FLOP_TRIS = 0;
 _5MC_2_MC_FLIP_FLOP_IO = 0;
 VERS_DT_TRIS	= 0;
 VERS_DT_IO = 0;	
    P_S_TRIS = 0;		
    P_S_IO	= 0;
	TEST_ENC_TRIS	= 0;
	TEST_ENC_IO	= 1;	


	TEST_ENC_MEM_TRIS = 0;			
	TEST_ENC_MEM_IO	= 0;			

	RES_HARD_IO = 0;		
	RES_HARD_TRIS = 0;
	CS_READ_MTR_TRIS = 0;
	CS_READ_MTR_IO   = 0;


	IZ_8_9_TRIS	= 1;	
			

	for(i = 0 ; i < WAITE_RES_HARD_ALTERA_144 ; i++) 
	{
		RES_HARD_IO = 1;
	}
    
	RES_HARD_IO = 0;
    _5MC_2_MC_FLIP_FLOP_IO = 0;
}

//-------------------------------------------------
//
//--------------------------------------------------
void p_kamera_Init()
{
 iff_init = 0;
 IP_ADDR  ia;
 memset((void*)&remote_node.MACAddr,0xff,6);
 p_kamera_command = INVALID_P_KAMERA_COMMAND;
 ia.v[0] = 0xa;
 ia.v[1] = 0x5;
 ia.v[2] = 0xa;
 ia.v[3] = 0xa;
 memcpy((void*)&remote_node.IPAddr,(void*)&ia.v[0],4);

 p_kamera_socket[0] =  UDPOpen(P_KAMER_PORT_RX, &remote_node, 0);
 p_kamera_socket[1] =  UDPOpen(P_KAMER_PORT_TX, &remote_node, 0);
 mode = MP_BEGIN;

TEST_GO_TRIS  = 0;       
TEST_GO_IO    = 0;     




MTR_A0_TRIS   = 0;
MTR_A1_TRIS   = 0;

ADRES_READ; 

#ifdef INT_RESET_IO
INT_RESET_TRIS = 1;  //вход
#endif

RUN_TRIS =  0;
RUN_IO = 1;



_5MC_2_MC_FLIP_FLOP_TRIS = 0;
_5MC_2_MC_FLIP_FLOP_IO = 0;

PIC_SINXRO_TRIS = 0;
PIC_SINXRO_IO = 0;



 GANE_0_TRIS =0;			
 GANE_0_IO	= 0;		


 GANE_1_TRIS = 0;			
 GANE_1_IO	= 0;		

 GANE_2_TRIS	= 0;		
 GANE_2_IO	= 0;	

#ifdef  INT_RESET_IO
INT_RESET_TRIS = 1;
#endif
Rs232_1_Init();

RST_ADC_TRIS  = 0;
RST_ADC_IO = 1;
/*
P_S_TRIS = 0;		
P_S_IO	= 0;	
*/
adjustment = 0;

	{
	int i;
		for(i = 0; i < WAITE_RESET_ADC; i ++)
			RST_ADC_IO = 0;
		RST_ADC_IO = 1;

		for( i = 0; i < _MEM_ *2; i++)
		{
			temp_data[i] = i;
		}

	}

#ifndef  MIT_AIN_BLOKS
dw_test_data = 0L;
wLewel = 1;
global_index = 0;

	{
	int i;
		srand(0);
		for(i = 0; i < 8 ; i++)
			r_d[i] = (rand() & 0x00ff);
	}
#endif
main_app =   APP_N_PRIBOR;
RES_HARD_IO = 0;
CS_READ_MTR_IO   = 0;


VERS_DT_IO = 1;
	{
    int i = 0;
    memset(VersDataASC,0,20);
    P_S_IO	= 0;
	P_S_IO	= 1;
        Encx24j600_GetVersion();
	P_S_IO	= 0;
   		for(i = 0; i < 21; i++)
    	{
		WORD t;
     		P_S_IO	= 1;          
			t  = (Encx24j600_GetVersion()& 0x7fff);
            if(i == 0)
            	VersDataHex = (t >> 8) & 0x7f;
   			VersDataASC[i] =(t & 0x00ff);

            if((VersDataASC[i] & 0xFF) == 0)
            {
				P_S_IO	= 0; break;
            }

   			P_S_IO	= 0;
		}
 	  
 VERS_DT_IO = 0;
_5MC_2_MC_FLIP_FLOP_IO = 0;
}

RES_HARD_IO = 0;




}

//-------------------------------------------------
//
//--------------------------------------------------
void p_kamera_udp_Task()
{

BYTE i; 
unsigned int  j;
WORD len;
static long arg_1;



if(mode == MP_BEGIN)
{
  for(i = 0; i < NUM_SOCKET_P_KAMERA; i++)
   {
    UDP_SOCKET s = p_kamera_socket[i];

         if((len = UDPIsGetReady(s)) > 0)
         {
           activeUDPSocket = s;
           memset(&rs_bufer[0], 0, MAX_STRING_RESIV);
           memset((BYTE*)&arguments[0],0, sizeof(BYTE*) * MAX_NUM_ARGUMENTS);

	        for(j = 0;  j < len; j++)
    		       UDPGet(&rs_bufer[j]);
        	p_kamera_command = INVALID_P_KAMERA_COMMAND;
        	p_kamera_ParseString(&rs_bufer[0], len);        
        	p_kamera_command = p_kamera_ParseCommand(arguments); 

        	if(p_kamera_command == P_KAMERA_CMD_GET)
          	{  		
                actual_socket = s;
          		dw_num_oll = arg_1 = atol((char*)arguments[1]);
                adjustment = 0;                 
                ADRES_ADJISMENT;
				#ifndef  MIT_AIN_BLOKS
					dw_test_data = 0L;
					srand(global_index++);
				#endif

               #ifdef _RS232_DEBUG_	
					if(flag_test_UDP_RS232 != 0)				
                	{
                	int num = strlen((char*)arguments[1]);
                		memset(buferr,0,30);
                		memcpy(buferr, "GET ", strlen("GET "));
                	char *ptr = &buferr[4];
                		memcpy((void*)ptr,(void*)arguments[1], num);
	 					Rs232_1_PutString(buferr, num + 4);
                } 
               #endif//#ifdef _RS232_DEBUG_
        		p_kamera_PutData(s,arg_1, temp_data);
               #ifdef _RS232_DEBUG_	
					if(flag_test_UDP_RS232 != 0)				
                	{
               			Rs232_1_PutInt1(l_num ,indexx++);
   				   		Rs232_1_PutString("\r\n", 2);
                	} 
               #endif//#ifdef _RS232_DEBUG_

           	  return;
          	}


            if(p_kamera_command == P_KAMERA_CMD_SET_DAC)
            {          
             char *buf;
			 actual_socket = s;
             buf = (char*)arguments[1]; 
				{
			#ifndef  MIT_AIN_BLOKS			
				char e_buf[8];
            		memcpy(e_buf, (char*)(buf), 8);
            		if(e_buf[1] == 0x20)
               			e_buf[1]   = 0;
            		e_buf[2] = 0;
					wLewel = atoi((char*)(buf));
			#endif
             		Rs232_1_PutString_too_Silabs((char*)buf, strlen((char*)buf));
				}
            }

            if(p_kamera_command == P_KAMERA_CMD_GANE_MTR)
            {
                actual_socket = s;
          		arg_1 = atol((char*)arguments[2]); 

                if( (arg_1 & 0x01L) == 1)
                     GANE_0_IO = 0x01;
                else 
                     GANE_0_IO = 0x0;

                if( (arg_1 & 0x02) == 0x02)
                     GANE_1_IO = 0x01;
                else 
                     GANE_1_IO = 0x0;

                if( (arg_1 & 0x04) == 0x04)
                     GANE_2_IO = 0x01;
                else 
                     GANE_2_IO = 0x0;

			}


             if(p_kamera_command == P_KAMERA_CMD_GET_VERSION)
             {
                actual_socket = s; 

             //   if(VERS_DT_IO	== 0)
				{		
					switch(VersDataHex)                  	
					{
           				case 0x01:	memcpy(buferr ,  "2048 8.0.0",  strlen( "2048 8.0.0")); //8 матр
									break;
						case 0x02:  memcpy(buferr ,  "1024 8.0.0",  strlen( "1024 8.0.0")); //8 матр
									break;
           				case 0x03: case 0x04:case 0x05:
                                    memcpy(buferr, VersDataASC,strlen(VersDataASC));
                                    memcpy(	(void*)&buferr[strlen(VersDataASC)], VersString, strlen(VersString));  
									break;
 						default:    memcpy(buferr ,  "xxxx x.0.0",  strlen( "xxxx x.0.0")); //8 матр
							//		memcpy(buferr ,  "2304 9.0.0",  strlen( "2304 9.0.0")); //9 матр
									break;						     
					}
				}
/*
                else
				{
                	memcpy(buferr ,  "2304 9.0.0",  strlen( "2304 9.0.0")); //9 матр
				}
*/
                p_kamera_PutVersion(s,buferr, strlen( buferr));
             }



            if(p_kamera_command == P_KAMERA_CMD_5MC_2_MC_FLIP_FLOP) 
             {
				actual_socket = s;
          		arg_1 = atol((char*)arguments[1]);
				_5MC_2_MC_FLIP_FLOP_IO = arg_1 == 0 ? 0:1;
              	#ifdef _RS232_DEBUG_
               	if(flag_test_UDP_RS232 != 0)
				{
				int num;
                	if(arg_1 == 1)
                	{
                     	num = strlen("2,5mC");
                     	memcpy((void*)buferr,(void*)"2,5mC", num);
                	} 
                	else
                	{
                    	num = strlen("1,25mC");	
                     	memcpy((void*)buferr,(void*)"1,25mC", num);					 
                	}
                	memcpy(buferr + num, " на строку ", strlen(" на строку")); 
					Rs232_1_PutString("\r\n", 2);
	 				Rs232_1_PutString(buferr, num + strlen(" на строку"));
				}
               #endif//#ifdef _RS232_DEBUG_
             } 

            if(p_kamera_command ==  P_KAMERA_CMD_MODE) 
             {
				actual_socket = s;
                if(*((char*)arguments[1] ) == 'R') //Сьемка
				{
					main_app = p_kamera_ParseArgument(arguments);
                    TEST_GO_IO = 0; 
              		#ifdef _RS232_DEBUG_
                	if(flag_test_UDP_RS232 != 0)    
					{
                		memset(buferr,0,30);
               			memcpy(buferr, "MODE RUN", strlen("MODE RUN"));                             
						Rs232_1_PutString("\r\n", 2);
	 					Rs232_1_PutString(buferr, strlen("MODE RUN"));
					}
               		#endif// #ifdef _RS232_DEBUG_   
				}
                else
                if(*((char*)arguments[1] ) == 'T') //ТЕСТИРОВАНИЕ МАТ. ПЛАТЫ с ВЫДАЧКЙ TRIG и END из АЛЬТЕРЫ
				{
					main_app = p_kamera_ParseArgument(arguments);
                    TEST_GO_IO = 1; 
               	  	#ifdef _RS232_DEBUG_
				 	if(flag_test_UDP_RS232 != 0) 
                 	{
                		memset(buferr,0,30);
                		memcpy(buferr, "АВТОНОМНЫЙ ТЕСТ", strlen("АВТОНОМНЫЙ ТЕСТ"));                             
						Rs232_1_PutString("\r\n", 2);
	 					Rs232_1_PutString(buferr, strlen("АВТОНОМНЫЙ ТЕСТ"));                
                 	}
				 	#endif
				}               
             } 
          }
   }
}
else
   p_kamera_PutData(actual_socket,arg_1, temp_data);

}

//----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void p_kamera_ParseString(BYTE *ptr, WORD len)
{
WORD i;
BYTE j;
BYTE t = 0;
arguments[0] = ptr;
 	for(i = 0, j = 1; i < len; i++)
 	{
    	t = ptr[i];
      	if(ptr[i] == ' ')
		{		 
      		arguments[j++] = (BYTE*)(&ptr[i] + 1);         
		}
 	}
	*(&ptr[i]) = 0;
}

//----------------------------------------------------------------------
//
//-------------------------------------------------------------------------
P_KAMERA_COMMAND p_kamera_ParseCommand(BYTE ** arg)
{
int i;
  	for(i = 0; i < NUM_P_KAMERA_KOMMAND;i++)
  	{
   		if ( !memcmppgm2ram((void*)arg[0], (ROM void*)P_KameraCommandString[i], strlenpgm(P_KameraCommandString[i])) )
       		return  ((P_KAMERA_COMMAND)i);
  	}
	return (INVALID_P_KAMERA_COMMAND); 
}

//#define _MEM _MEM_
#define _MIT_ERROR_ION
void Sleep(long t);
void Adjustment();
//--------------------------------------------------------------------
//
//----------------------------------------------------------------------
void p_kamera_PutData(UDP_SOCKET s, DWORD len , BYTE *bufer)
{
long num   = len;
static WORD num_mem;
DWORD size;
l_num = 0L; 
RUN_IO = 0;
//t_global = 0;
/*
if(VERS_DT_IO  == 1) 
  num_mem = _MEM_ + 512;
else
*/
{
	switch(VersDataHex)                   	
					{
						case 0x02: case 0x05: num_mem = _MEM_/2;
									break;	
						case 0x04:   num_mem = _MEM_ + 512; break;
						default: 	num_mem = _MEM_;
									break;
						     
					}
}
	do
	{

	#ifdef INT_RESET_IO
		// Ждем начала чтения строки
		#ifdef _RS232_DEBUG_
			p_kamera_StringSinxro(num);
		#else
			p_kamera_StringSinxro();
		#endif
	#endif
   
	if(num >= num_mem)
		size = num_mem;
	else
		size =  num; 
	p_kamera_PutBufer(s, size, bufer);
	num  -=size;

	}while(num > 0);
	RUN_IO = 1;
}


//----------------------------------------------------------------------
void Adjustment()
{
	#ifdef _MIT_ERROR_ION 
		adjustment++;
		if(adjustment >  (5))
		{
     		ADRES_READ;
        	adjustment = 6;
		}

		if(adjustment  == (5))
		{                 
	    	ADRES_ION;
		}	
	#else  //_MIT_ERROR_ION                                              
		adjustment++;
		if(adjustment > 4)
		{
 			adjustment = 0;
 			ADRES_READ;
		} 
	#endif
}


WORD free_len; 

//-----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

void p_kamera_PutBufer(UDP_SOCKET s, DWORD len, BYTE *bufer)
{
long num;
WORD num_mem;
BYTE *ptr = bufer;
num = len;
/*
if(VERS_DT_IO  == 1) 
  num_mem = _MEM_ + 512;
else
*/
{
	switch(VersDataHex)                   	
					{
						case 0x02: case 0x05: num_mem = _MEM_/2;
									break;
                       	case 0x04:   num_mem = _MEM_ + 512; break; 	
						default: 	num_mem = _MEM_;
									break;
						     
					}
}
	do
	{
		free_len = 0;
		while(!(free_len = UDPIsPutReady(s)));

		if(free_len > num_mem)
			free_len = num_mem;
		if(free_len >= num)
		{
		WORD t;
        	t = UDPPutMemory(ptr, (WORD)num);
        	num -= t;
		}
		else
		{	     
        	free_len = UDPPutMemory(ptr, free_len);
        	num -= (free_len);        
		}
 		UDPFlush();
	}while(num > 0);
}

//--------------------------------------------------------------------
//
//----------------------------------------------------------------------
#ifdef _RS232_DEBUG_				 	
void p_kamera_StringSinxro(long num)
#else
void p_kamera_StringSinxro()
#endif
{

#ifdef  MIT_AIN_BLOKS
	PIC_SINXRO_IO = 0;
	do
	{
		do
		{
			Sleep(1);
		}while(INT_RESET_IO == 0); 
		Sleep(5);
	}while(INT_RESET_IO == 0); 

    Adjustment();
	PIC_SINXRO_IO = 1;
	do
	{
    	do
		{
			Sleep(10);
		}while(INT_RESET_IO == 1); 
		Sleep(20);
	}while(INT_RESET_IO == 1); 


     PIC_SINXRO_IO = 0;
#else
	Sleep(1);
#endif
}

//--------------------------------------------------------------------
//
//----------------------------------------------------------------------
void Sleep(long t)
{
	while(t > 0)t--; 
}


//----------------------------------------------------------------
void p_kamera_PutVersion(UDP_SOCKET s, char *bufer, WORD len)
{
WORD free_len = 0;
DWORD z = 1000000;

	while(z > 0)z--; 

	do
	{
  		free_len = UDPIsPutReady(s);
	}while( len> free_len);

	UDPPutArray((BYTE*)bufer,len);
    UDPFlush();
	iff_init = 0xff;
}

#ifndef  MIT_AIN_BLOKS
//--------------------------------------------------------------
WORD p_kamera_GetWordFromTestData(DWORD num)
{
WORD ret;
static WORD string_index;
	if(dw_test_data == 0)
		string_index = 0;

  	if(string_index < 1)
		ret = 0;
	
	if((string_index >= 1 ) && (string_index < 5))	
		ret = p_kamera_Stage_data(dw_test_data - 2048L);

	if(string_index == 5)
		ret = p_kamera_Ion_data();

	if(string_index > 5)
		ret = p_kamera_Lewel_data(dw_test_data - 6 * 2048, wLewel);

	string_index  = dw_test_data/2048L;
	dw_test_data++;

	return ret;
}

#define M_INIT_STAGE (IZ16_14_IO == 0 ?  (64536 - 1024) : (16384 - 256))
#define M_SET_STAGE (IZ16_14_IO == 0 ? (490 * 4) : 510)
//------------------------------------------------------------
WORD p_kamera_Stage_data(DWORD data_index)
{
static WORD ret = 0;
div_t d;
WORD r;	
	if(data_index == 1)
	{		
		ret =  M_INIT_STAGE;
		d.rem = 0;
		d.quot = 0;
	}
	if(d.rem == 0)
	    ret -= M_SET_STAGE;
	d = div(data_index, 8 * 32);
	r = r_d[data_index & 0x07];
	if((r & 0x01) == 0)
		return	((ret + r) + (rand() & 0x01f));
	else
		return	((ret - r) - (rand() & 0x01f));
}

//------------------------------------------------------------
WORD p_kamera_Ion_data()
{
WORD t = (rand() & 0x01f);
WORD ret = IZ16_14_IO == 0 ? 65535 : 16384;

		if((t & 0x01) == 0)
			return (WORD)(((double)ret) / 2.3 + (double)t);
		else
			return (WORD)(((double)ret) / 2.3 - (double)t);

}
//------------------------------------------------------------
WORD p_kamera_Lewel_data(DWORD data_index, WORD level)
{
WORD t = (rand() & 0x01f);
WORD ret = IZ16_14_IO == 0 ? 65535 : 16384;
		if(p_kamera_1_Bolt_Level(data_index, &ret))
			return ret;
		ret = IZ16_14_IO == 0 ? 65535 : 16384;

		if(level == 0)
			return (WORD)(200.0   +   (double)t);

		if((t & 0x01) == 0)
			return (WORD)((((double)ret)/ 10.0) *  level +  (level < 9 ? (double)t : 0));
		else			
			return (WORD)((((double)ret)/ 10.0) *  level -  (level > 0 ? (double)t : 0));
}
#endif

//----------------------------------------------------------------------
//
//-------------------------------------------------------------------------
MAIN_APP p_kamera_ParseArgument(BYTE ** arg)
{
int i;
//int n_a = NUM_APP;
  for(i = 0; i < NUM_APP; i++)
  {
   if ( !memcmppgm2ram((void*)arg[2], (ROM void*)chAppType[i], strlenpgm(chAppType[i])) )
       return  ((MAIN_APP)i);
  }
return (APP_END_NUMBERING); 
}

//----------------------------------------------------------------------
//
//-------------------------------------------------------------------------
WORD	p_kamera_1_Bolt_Level(DWORD data_index, WORD *ret)
{
WORD t = (rand() & 0x07f);
  
		switch(main_app)
		{
			case APP_NEYSTROEV:  
								if(dw_num_oll > 1500000) 
									return 0;
								break; 
			case APP_1024_MAIN:					
			case APP_2048_MAIN:								
								if(data_index > 2048 * 4) 
									return 0;
								break;								
			case APP_N_PRIBOR:
								if(data_index > 2048 * 100) 
									return 0;
								break;								
			default:  break;
		}
								 	
		*ret = (WORD)(200.0   +   (double)t);												
		return 1;
	
return 0;
}

//--------------------------------------------------------
//
//--------------------------------------------------------
void  p_kamera_Icmp_error_proc()
{
/*
int i = 1000;
 _5MC_2_MC_FLIP_FLOP_IO = 0;
  	while(i > 0)i--;
  _5MC_2_MC_FLIP_FLOP_IO = 1;
*/
}
