//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop


#include "packet32.h"
#include "reg_app.h"
#include "uAppInstal.h"

#include <Iphlpapi.h>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"



TAppInstal *AppInstal;
//---------------------------------------------------------------------------
__fastcall TAppInstal::TAppInstal(TComponent* Owner)
        : TForm(Owner)
{

}

//---------------------------------------------------------------------------
void __fastcall TAppInstal::FormCreate(TObject *Sender)
{
#define SZ_IP_ADAPTER_INFO 5000
     int i;
     char       *temp,*temp1;
     IP_ADAPTER_INFO *ad_info;
     DWORD len = SZ_IP_ADAPTER_INFO;
     i = 0;
     BYTE ptr[SZ_IP_ADAPTER_INFO ];

     memset( AdapterName,0,8192 * sizeof( WCHAR ));
     dwVersion = GetVersion();
     dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));

     if (!(dwVersion >= 0x80000000 && dwWindowsMajorVersion >= 4))
     {
        DWORD e;
        if((e = GetAdaptersInfo((IP_ADAPTER_INFO* )ptr, &len)) != ERROR_SUCCESS)
          {
                 ErrorAdapterInfo(e);
                 return;
          }

         AdapterLength = sizeof(AdapterName);
         if(PacketGetAdapterNames((char*)AdapterName, &AdapterLength) == FALSE)
		 {
                 AnsiString as("В системе нет сетевых адаптеров!");
                 MessageBox(Handle, as.c_str(), "Error AppInstal", MB_OK);
                 SendMessage(Handle,WM_CLOSE,0,NULL);
                 return ;
                 }

         temp=(char*)AdapterName;
         temp1=(char*)AdapterName;
         while ((*temp!='\0')||(*(temp - 1)!='\0')/* || (*(temp - 2)!='\0')*/)
         {
				if (*((char*)temp)=='\0')
				{
					memcpy(AdapterList[i],
                                              (char*)temp1,(temp - temp1)*2);
                                        AnsiString as((char*)AdapterList[i]);
                                         ad_info = (IP_ADAPTER_INFO*)ptr;
                                        do
                                        {
                                                if(ptr != NULL)
                                                {
                                                int i;
                                                        for(i = 2; i < as.Length();i++)
                                                        {
                                                                if(as[i + 1] == '{')
                                                                break;
                                                        }
                                                        char *ctr = (char*)(as.c_str() + i);

                                                        if(0 == strcmp((char*)(ad_info->AdapterName), ctr))
                                                        {

                                                                as += ",       ";
                                                                as += (char*)(ad_info->IpAddressList.IpAddress.String);
                                                                
                                                                break;
                                                        }
                                                        ad_info = (ad_info)->Next;

                                                }
                                        }while(ad_info);

                                        LBSelect->Items->Add(as);
                                        temp1=temp+1;
                                        i++;
				}
         temp++;

         }
     }
}                                                    
//---------------------------------------------------------------------------
void __fastcall TAppInstal::BtOKClick(TObject *Sender)
{
     if(LBSelect->ItemIndex == -1){
                  return;
     }
      memset(byfffer,0,100);
      memcpy(byfffer,
            (char*)AdapterList[LBSelect->ItemIndex],
            strlen((char*)AdapterList[LBSelect->ItemIndex]));

            REGDATA_APP_INIT r;
            r.WriteParamTooReg(byfffer);
}

//-----------------------------------------------
int  __fastcall TAppInstal::ErrorAdapterInfo(int e)
{
        switch(e)
        {
                case ERROR_BUFFER_OVERFLOW:
                {
                        MessageBox(Handle, "ERROR_BUFFER_OVERFLOW", "ERROR: GetAdaptersInfo",  MB_OK);
                        SendMessage(Handle,WM_CLOSE,0,NULL);
                        break;
                }
                case ERROR_INVALID_PARAMETER:
                {
                        MessageBox(Handle,  "ERROR_INVALID_PARAMETER", "ERROR: GetAdaptersInfo", MB_OK);
                        SendMessage(Handle,WM_CLOSE,0,NULL);
                        break;
                }
                case ERROR_NO_DATA:
                {
                        MessageBox(Handle,  "ERROR_NO_DATA", "ERROR: GetAdaptersInfo",  MB_OK);
                        SendMessage(Handle,WM_CLOSE,0,NULL);
                        break;
                }
                case ERROR_NOT_SUPPORTED:
                {
                        MessageBox(Handle,  "ERROR_NOT_SUPPORTED", "ERROR: GetAdaptersInfo", MB_OK);
                        SendMessage(Handle,WM_CLOSE,0,NULL);
                        break;
                }
                default:
                {
                        MessageBox(Handle,  "ERROR_UN_KNOW", "ERROR: GetAdaptersInfo",MB_OK);
                        SendMessage(Handle,WM_CLOSE,0,NULL);
                        break;
                }
        }
        return 1;
}

