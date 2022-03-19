

//-----------------S_KAMERA--------------------------//
#include "TCPIP Stack/TCPIP.h"
#include "TCPIPConfig.h"
#include "TCPIP Stack/StackTsk.h"
#if defined(STACK_USE_UDP)
	#include "TCPIP Stack/UDP.h"
//#include "../../../../SRC/Microchip/UDP.h"
#endif  
#include "GenericTypeDefs.h"

#include <string.h>
#include <stdlib.h>
#include "..\..\..\S_KAMERA\SRC\Microchip\s_kamera_udp.h"
#include "..\..\..\S_KAMERA\SRC\Microchip\Rs232_1.h"

#define P_KAMER_PORT_RX 2048
#define P_KAMER_PORT_TX 2049

#define NUM_SOCKET_P_KAMERA 8
#define MAX_STRING_RESIV 20

#define MAX_NUM_ARGUMENTS 5
#define NUM_P_KAMERA_KOMMAND 8

const char VersString[] = " PIC: v3.0.000"; //Прошивка PIC для SRK

typedef  enum _P_KAMERA_COMMAND
{
 P_KAMERA_CMD_GET,
 P_KAMERA_CMD_GANE_MTR,
 P_KAMERA_CMD_MODE,
 P_KAMERA_CMD_GET_VERSION,
 P_KAMERA_CMD_SET_DAC,
 S_KAMERA_CMD_SET_BINNING,
 S_KAMERA_CMD_SET_FILTER,
 S_KAMERA_TIME_PER_STRING, 
 INVALID_P_KAMERA_COMMAND = 0xff 
}P_KAMERA_COMMAND; 

ROM char *P_KameraCommandString[] = 
{
 "GET","GANE_MTR","MC_MODE","MC_GET_VERSION","P_KAMERA_CMD_SET_DAC", "MC_SET_BINNING","MC_SET_FILTER", "MC_TIME_PER_STRING"
};

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


static UDP_SOCKET actual_socket;
char buferr[90];
DWORD dw_num_oll;
char VersDataASC[90];
BYTE VersDataHex;
WORD free_len; 
WORD w_binning;

void s_kamera_ParseString(BYTE *ptr, WORD len);
P_KAMERA_COMMAND s_kamera_ParseCommand(BYTE ** arg);
void s_kamera_PutBufer(UDP_SOCKET s, DWORD len, BYTE *bufer);
void s_kamera_PutData(UDP_SOCKET s, DWORD len, BYTE *bufer);
WORD UDPPutMemory(BYTE *cData, WORD wDataLen);
void s_kamera_PutVersion(UDP_SOCKET s, char *bufer, WORD len);
void s_kamera_StringSinxro();
BYTE s_kamera_InitVersion( char *versdata);
void s_kamera_Sleep(long t);



//-------------------------------------------------
//
//--------------------------------------------------
void s_kamera_InitAltera144()
{
long i;
	MTR_A1_TRIS = 0;      
	MTR_A1_IO  = 1;       
	CS_READ_MTR_TRIS = 0;
	CS_READ_MTR_IO   = 0;

  	RF6_ALT_CS_TRIS  = 0;
  	RF6_ALT_CS_IO  = 0;   


	TEST_ENC_TRIS	= 0;
	TEST_ENC_IO	= 1;	


	TEST_ENC_MEM_TRIS = 0;			
	TEST_ENC_MEM_IO	= 0;			

	RES_HARD_TRIS = 0;
	RES_HARD_IO = 0;		


	for(i = 0 ; i < (WAITE_RES_HARD_ALTERA_144 *20); i++) 
	{
		RES_HARD_IO = 1;
	}
    
	RES_HARD_IO = 0;

}

//-------------------------------------------------
//
//--------------------------------------------------
void s_kamera_Init()
{
int i;
 //iff_init = 0;
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


PIC_SINXRO_TRIS = 0;
PIC_SINXRO_IO = 0;

Rs232_1_Init();

RST_ADC_TRIS  = 0;
RST_ADC_IO = 1;

adjustment = 0;
		for(i = 0; i < WAITE_RESET_ADC; i ++)
			RST_ADC_IO = 0;
		RST_ADC_IO = 1;

		for( i = 0; i < _MEM_ *2; i++)
		{
			temp_data[i] = 0;
		}
CS_READ_MTR_IO   = 0;


VersDataHex = s_kamera_InitVersion(( char*)VersDataASC);
w_binning = 0x01;
}

//-------------------------------------------------
//
//--------------------------------------------------
void s_kamera_udp_Task()
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
        	s_kamera_ParseString(&rs_bufer[0], len);        
        	p_kamera_command = s_kamera_ParseCommand(arguments); 

			//------------------------------------------------------------------------------
        	if(p_kamera_command == P_KAMERA_CMD_GET)
          	{  		
                actual_socket = s;
          		dw_num_oll = arg_1 = atol((char*)arguments[1]);

				if(TEST_GO_IO == 1)
				{
					MTR_A1_IO = 1;
					s_kamera_Sleep(1000);
					MTR_A1_IO = 0;
				}


                adjustment = 0;                 
                ADRES_ADJISMENT;
        		s_kamera_PutData(s,arg_1, temp_data);
           	  return;
          	}

			//------------------------------------------------------------------------------
            if(p_kamera_command == P_KAMERA_CMD_SET_DAC)
            {          
             char *buf;
			 actual_socket = s;
             	buf = (char*)arguments[1]; 			
             	Rs232_1_PutString_too_Silabs((char*)buf, strlen((char*)buf));				
            }
			//-------------------------------------------------------------------------------
            if(p_kamera_command == P_KAMERA_CMD_GANE_MTR)
            {
          //   long i;
				actual_socket = s;
            char b;
			    b  = atol((char*)arguments[2]);
            WORD w;
		//	WORD	w = b == 0x01 ? SET_GANE_1_2040 : SET_GANE_0_2000;



	//		RST_ADC_IO = 0;



            if(VersDataHex == 0x06)
            {
            w = SET_GANE_0_2000 | (b & 0x07);
			}
            else
            if(VersDataHex == 0x07)
            {
            w = SET_GANE_0_2000 | (b & 0x0f);
			}
			else
			{
			w = (b & 0x01) == 0x01 ? SET_GANE_1_2040 : SET_GANE_0_2000;
   			}        

   		    Encx24j600_SendWordKomandTooAltera(w);
			Rs232_1_PutString_too_Silabs((char*)"R", 1);
			
      

	//		RST_ADC_IO = 1;

            }
			//------------------------------------------------------------------------------
             if(p_kamera_command == P_KAMERA_CMD_GET_VERSION)
             {
                actual_socket = s; 
						
					switch(VersDataHex)                  	
					{
           				case 0x01:	memcpy(buferr ,  "2048 8.0.0",  strlen( "2048 8.0.0")); //8 матр
									break;
						case 0x02:  memcpy(buferr ,  "1024 8.0.0",  strlen( "1024 8.0.0")); //8 матр
									break;
           				case 0x03: case 0x04:case 0x05:case 0x06: case 0x07:
                                    memcpy(buferr, VersDataASC,strlen(VersDataASC));
                                    memcpy(	(void*)&buferr[strlen(VersDataASC)], VersString, strlen(VersString));  

									if(VersDataHex == 0x06)	
									{
                					char buf[5];
										memset(buf,0,5);
                						buf[1] = 'D';
                						buf[0] = '0' ;
										Rs232_1_PutString_too_Silabs((char*)buf, strlen((char*)buf));
									}
 				
				
									if(VersDataHex == 0x07)	
									{
                					char buf[5];
										memset(buf,0,5);
                						buf[1] = 'E';
                						buf[0] = '0' ;
										Rs232_1_PutString_too_Silabs((char*)buf, strlen((char*)buf));
									}


									break;
 						default:    memcpy(buferr ,  "xxxx x.0.0",  strlen( "xxxx x.0.0")); //8 матр					
									break;						     
					}
				
                {
                int n = strlen( buferr);  
                s_kamera_PutVersion(s,buferr, n);
                }
             }

			//------------------------------------------------------------------------------
            if(p_kamera_command ==  P_KAMERA_CMD_MODE) 
             {
				actual_socket = s;
                if(*((char*)arguments[1] ) == 'R') //Сьемка
				{
                   //  MTR_A1_IO = 0;
                     TEST_GO_IO = 0;
					 Encx24j600_SendWordKomandTooAltera(SENSORS_MODE_0400);	
				}
                else
                if(*((char*)arguments[1] ) == 'T') //ТЕСТИРОВАНИЕ МАТ. ПЛАТЫ с ВЫДАЧКЙ TRIG и END из АЛЬТЕРЫ
				{
                     TEST_GO_IO = 1; 
                    // MTR_A1_IO = 1;
					 Encx24j600_SendWordKomandTooAltera(MODE_WITHOUT_SENSORS_0404);
				}               
             } 
           //------------------------------------------------------------------------------
			if(p_kamera_command == S_KAMERA_CMD_SET_BINNING)
			{
               //BYTE b;
				actual_socket = s;
                char buf[5];
				memset(buf,0,5);
                buf[1] = 'C';
                buf[0] = *(char*)arguments[1];
				Rs232_1_PutString_too_Silabs((char*)buf, strlen((char*)buf));	


                WORD w = CH_BINNING_0800;
                switch(*(BYTE*)arguments[1])
                {
                    case 0x31: w |= 0x0000; w_binning = 0x01; break;
					case 0x32: w |= 0x0008; w_binning = 0x02; break;
					case 0x33: w |= 0x0010; w_binning = 0x03; break;
					default:   w |= 0x0018; w_binning = 0x04; break;

                }
					 Encx24j600_SendWordKomandTooAltera(w);
			}
           //-------------------------------------------------------------------------------
			if(p_kamera_command == S_KAMERA_CMD_SET_FILTER)
			{
               
				actual_socket = s;
            char b;
			    b = *(char*)arguments[1];

			WORD	w = b == 0x31 ? SET_FILTER_1020 : RESET_FILTER_1000 ;

   			    Encx24j600_SendWordKomandTooAltera(w);
			}

//----------------------------------------------------------------------------
			if(p_kamera_command == S_KAMERA_TIME_PER_STRING)
			{
               
				actual_socket = s;
            char b;
			    b = *(char*)arguments[1];
           		if((VersDataHex == 0x06) || (VersDataHex == 0x06))
           		{
				WORD	w = b == 0x30 ? SET_TIME_5MC : SET_TIME_10MC ;
   			    			Encx24j600_SendWordKomandTooAltera(w);
				}
          }
   }
}
}
else
   s_kamera_PutData(actual_socket,arg_1, temp_data);

}

//----------------------------------------------------------------------
//
//-----------------------------------------------------------------------
void s_kamera_ParseString(BYTE *ptr, WORD len)
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
P_KAMERA_COMMAND s_kamera_ParseCommand(BYTE ** arg)
{
int i;
  	for(i = 0; i < NUM_P_KAMERA_KOMMAND;i++)
  	{
   		if ( !memcmppgm2ram((void*)arg[0], (ROM void*)P_KameraCommandString[i], strlenpgm(P_KameraCommandString[i])) )
       		return  ((P_KAMERA_COMMAND)i);
  	}
	return (INVALID_P_KAMERA_COMMAND); 
}




void Adjustment();
//--------------------------------------------------------------------
//
//----------------------------------------------------------------------
void s_kamera_PutData(UDP_SOCKET s, DWORD len , BYTE *bufer)
{
long num   = len;
static WORD num_mem;
DWORD size;

//RUN_IO = 0;
	switch(VersDataHex)                   	
					{
						case 0x02: case 0x05: num_mem = _MEM_/2; RUN_IO = 0;
									break;	

						case 0x04:   num_mem = _MEM_ + 512; RUN_IO = 0; break;

						case 0x06:    num_mem = _MEM_1152; 
									Encx24j600_SendWordKomandTooAltera(REQ_GET_DATA_0202);
                                    RUN_IO = 0;
									break;

						case 0x07:  num_mem = _MEM_576; 
									Encx24j600_SendWordKomandTooAltera(REQ_GET_DATA_0202);
                                    RUN_IO = 0;
									break;

						default: 	num_mem = _MEM_;
									break;
						     
					}

	do
	{

	#ifdef INT_RESET_IO
		// Ждем начала чтения строки
			s_kamera_StringSinxro();
	#endif
   
	if(num >= num_mem)
		size = num_mem;
	else
		size =  num; 
	s_kamera_PutBufer(s, size, bufer);
	num  -=size;

	}while(num > 0);

    s_kamera_StringSinxroOfEnd();
	RUN_IO = 1; 
	Encx24j600_SendWordKomandTooAltera(RESET_GET_DATA_0200);
}


//-----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------

void s_kamera_PutBufer(UDP_SOCKET s, DWORD len, BYTE *bufer)
{
long num;
WORD num_mem;
BYTE *ptr = bufer;
num = len;

{
	switch(VersDataHex)                   	
					{
						case 0x02: case 0x05: num_mem = _MEM_/2;
									break;
                       	case 0x04:   num_mem = _MEM_ + 512; break; 
						case 0x06:    num_mem = _MEM_1152; break;	
						case 0x07:    num_mem = _MEM_576; break;
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

void s_kamera_StringSinxro()
{
	PIC_SINXRO_IO = 0;
	do
	{
		do
		{
			s_kamera_Sleep(10);
		}while(INT_RESET_IO == 0); 
		s_kamera_Sleep(20);//2
	}while(INT_RESET_IO == 0); 
    Adjustment();

	PIC_SINXRO_IO = 1;
	do
	{
    	do
		{
			s_kamera_Sleep(10);
		}while(INT_RESET_IO == 1); 
		s_kamera_Sleep(20);
	}while(INT_RESET_IO == 1); 
     PIC_SINXRO_IO = 0;
}


//----------------------------------------------------------------------
//
//--------------------------------------------------------------------
void s_kamera_StringSinxroOfEnd()
{

    Adjustment();
	PIC_SINXRO_IO = 0;
	do
	{

		s_kamera_Sleep(200);
	}while(INT_RESET_IO == 0); 
     PIC_SINXRO_IO = 0;
}

//--------------------------------------------------------------------
//
//----------------------------------------------------------------------
void s_kamera_Sleep(long t)
{
	while(t > 0)t--; 
}


//----------------------------------------------------------------
void s_kamera_PutVersion(UDP_SOCKET s, char *bufer, WORD len)
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
}

//----------------------------------------------------------------------
void Adjustment()
{
static WORD madj;
if(VersDataHex == 0x07)
madj = 16;  
else
madj = 4 * w_binning;


		if(adjustment <  madj)
		{
     		ADRES_ADJISMENT;
            adjustment++;
		}
       else
        ADRES_READ;
}


//--------------------------------------------------------
void  s_kamera_Icmp_error_proc()
{

}
//----------------------------------------------------------------
BYTE s_kamera_InitVersion(char *versdata)
{
WORD i,j;
WORD w;
BYTE ret = 0;
BYTE count = 0;
        memset(VersDataASC, 0, sizeof(VersDataASC));
 	    Encx24j600_SendWordKomandTooAltera(REQ_GET_VERSION_0101);
//Прокрутка шины
        Encx24j600_GetWordStateFromAltera();

 		while(((Encx24j600_GetWordStateFromAltera()) & RDY_SEND_VERSION_001) != RDY_SEND_VERSION_001);

      	for(i = 0, j = 0; i < sizeof(VersDataASC); i++)
      	{ 
            
       		w = (Encx24j600_GetWordDataFromAltera() /*& 0x0ff*/);
       		if(i == 1)
         		ret = (BYTE)HIBYTE(w); 
            if(i > 0)    
            {   
                j = i - 1;  
       		    versdata[j] = (char)LOBYTE(w); 
            	if((versdata[j]) == 0)
            	{ 
                	if(count == 3)
                 		break;
            		count++;
            	}
            }
          
      	}
        Encx24j600_SendWordKomandTooAltera(RESET_GET_VERSION_0100);
return ret;
}


