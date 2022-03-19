
#include "reg_app.h"
#include <new>

#include <string>


using namespace std;
#define NUM_PARAM 1

#include "test_dll.h"
static const char *regKey = "Software\\N_PRIBOR_P_KAMERA_APP_INSTAL\\ParamList";


        AnsiString as_adpt;

//---------------------------------------------------------------------------

REGDATA_APP_INIT::REGDATA_APP_INIT()
{
        try
        {
                reg = new  TRegistry;
        }
        catch(bad_alloc)
        {
        test_dll_OutputDebugString("Error - new reg in REGDATA_APP_INIT()");
        }
        catch(...)
        {
        test_dll_OutputDebugString("GlbError - new reg in REGDATA_APPINIT()");
        }
        memset(adapter,0,sizeof(adapter));
        reg->RootKey = HKEY_CURRENT_USER;
}
//---------------------------------------------------------------------------

REGDATA_APP_INIT::REGDATA_APP_INIT(char *name_adapter)
{
        try
        {
                reg = new  TRegistry;
        }
        catch(bad_alloc)
        {
                test_dll_OutputDebugString("Error - new reg in REGDATA_APP_INIT()");
        }
        catch(...)
        {
                test_dll_OutputDebugString("GlbError - new reg in REGDATA_APPINIT()");
        }
        memset(adapter,0,sizeof(adapter));
        reg->RootKey = HKEY_CURRENT_USER;
        WriteParamTooReg(name_adapter);
}
//------------------------------------------------------------
bool REGDATA_APP_INIT::WriteParamTooReg(char *Value)
{
        AnsiString as = Value;
        memcpy(adapter,Value,strlen(Value));
        reg->OpenKey(regKey,true);
        reg->WriteString("ADAPTER" ,"");
        reg->WriteString("ADAPTER", as);
        reg->CloseKey();
        return true;
}
//#include "test_dll.h"
//---------------------------------------------------------------
const char * RegKey(){return regKey;}
//----------------------------------------------------------------
char* REGDATA_APP_INIT::ReadParamFromReg()
{
        if(reg->OpenKey(regKey,false))
        {
                as_adpt  = reg->ReadString("ADAPTER");
                reg->CloseKey();
                return (char*)as_adpt.c_str();
        }
        else
                return NULL;
}




