//---------------------------------------------------------------------------
#include "reg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../exp_error/exp_error.h"
#include <ctype.h>

#ifdef unix
#include <errno.h>
#include <unistd.h>
#else
#include <Windows.h>
#include <Winuser.h>
#endif
DWORD EXP_ERROR::error;
string EXP_ERROR::text_out;
//--------------------------------------------------------------------------
//Инициализация класса Tregistry
// аргументы:
//          *Key указатель на строку -ключа раздела(по аналогии с подразделом реестра в Windows)
//          ** Str_param_namе - указатель, на указатели строк параметров данного подраздела
//      num - количество параметров в подразделе
//          **df - указатель, на указатели строк начальных значений параметров, записываемых
//      при создании файла *.ini
//          f_name - указатель на строку с путем и именем файла *.ini
// возвращаемое значение:
//      указатель на стуктуру TRegistry или ноль при ошибке
// коментарии:
//      функция - обертка для обработок except.
//---------------------------------------------------------------------------
TRegistry* InitReg(const char *Key, const char ** Str_param_name, int num, const char **df, LPCTSTR f_name)
{
TRegistry*    reg;
EXP_ERROR::ErrorRes();
try
{
    reg = new TRegistry(Key,Str_param_name, num, df, f_name);
}
catch(EXP_ERROR &e)
{
   e.ErrorSetText("p_reg_dll");
   return 0;
}
    return reg;
}


//--------------------------------------------------------------------------
//Конструктор класса Tregistry
// аргументы:
//          *Key указатель на строку -ключа раздела(по аналогии с подразделом реестра в Windows)
//          ** Str_param_namе - указатель, на указатели строк параметров данного подраздела
//      num - количество параметров в подразделе
//          **df - указатель, на указатели строк начальных значений параметров, записываемых
//      при создании файла *.ini
//          f_name - указатель на строку с путем и именем файла *.ini
// возвращаемое значение:
//      нет
// коментарии:
//          функция - выбрасывает исключение:
//              - при не возможности открыть или создать файл
//              - при не возможности выделить память под строку regstring
//-------------------------------------------------------------------------------------------
TRegistry::TRegistry(const char *Key,const char **Str_param_name, WORD num, const char **df, LPCTSTR f_name)
{
#ifdef unix
    hFile = (int)0;
#else
    hFile = NULL;
#endif
    num_param = 0;
    buffer = NULL;
    reg_str_param_name = NULL;
    regkey = NULL;
    regstring = NULL;
    defstring = NULL;
    num_param = num;
    reg_str_param_name = (const char**)Str_param_name;
    regkey =  (char*)Key;
    regstring = new REGSTRING[num];
    if(regstring == NULL)
        throw EXP_ERROR("TRegistry","can not allocate memory regstring");
    defstring = df;
#ifndef unix
    DWORD e;
    LPCTSTR fn;
    fn = (LPCTSTR)f_name;
    hFile = CreateFile(fn, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE)
    {
        if((e = GetLastError()) == ERROR_FILE_NOT_FOUND)        
                hFile = CreateFile(fn, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL,
                                     CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);        
        if((e != ERROR_FILE_NOT_FOUND) || (hFile == INVALID_HANDLE_VALUE))
                throw EXP_ERROR("TRegistry","Cannot open file s_dll_udp_init.ini");
    }
#else
    hFile = open(f_name, O_RDWR|O_CREAT|O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
            if(hFile < 0)
                throw EXP_ERROR("TRegistry","Cannot open file s_dll_udp_init.ini");

#endif
}
//--------------------------------------------------------------------------
//Удаление класса Tregistry
// аргументы:
//      *reg - указатель на удаляемый класс
// возвращаемое значение:
//      нет
// коментарии:
//      нет
//-------------------------------------------------------------------------------------------
void DestroyReg(TRegistry* reg)
{
 delete reg;
}
//--------------------------------------------------------------------------
//Чтение параметров из файла *.ini в строку класса TRegistry: regstring, типа REGSTRING
// аргументы:
//          *reg указатель на класс TRegistry, строку которого  regstring, надо заполнить
//      параметрами, читаемыми из файла *.ini.
// возвращаемое значение:
//          указатель на строку REGSTRING или ноль при ошибке
// коментарии:
//          функция - обертка для обработок except.
//---------------------------------------------------------------------------
REGSTRING* ReadParamTooReg(TRegistry *reg)
{
    try
    {
    return ReadParam(reg);
    }
    catch(EXP_ERROR &e)
    {
           e.ErrorSetText("p_reg_dll");
      return NULL;
    }
    return NULL;
}

//--------------------------------------------------------------------------
//Чтение параметров из файла *.ini в строку класса TRegistry: regstring, типа REGSTRING
// аргументы:
//          *reg указатель на класс TRegistry, строку которого  regstring, надо заполнить
//      параметрами, читаемыми из файла *.ini.
// возвращаемое значение:
//          указатель на строку REGSTRING или ноль при ошибке
// коментарии:
//          функция - при первом вызове созлает раздел имя которого возвращается вызовом Regkey()
//      и записывает в файл значения указанные в строке defstring типа const char ** класса TRegistry
//          функция транслирует исключения выбрасываемые функциями:
//      OpenKey, WriteString, ReadString
//---------------------------------------------------------------------------
REGSTRING* ReadParam(TRegistry *reg)
{
int i;
int ret;
 string  ch(reg->Regkey());
 ret = reg->OpenKey(ch,false);
    if(!ret)
    {
        reg->OpenKey(ch,true);
        for(i = 0;i < reg->Num_param(); i++)
        {                       
            reg->WriteString(reg->Reg_str_param_name(i),reg->Defstring(i));
        }

    }

    for(i = 0;i < reg->Num_param(); i++)
    {
    string as;
        if(i == 8)
           i = i;
         reg->ReadString(reg->Reg_str_param_name(i), &as);
         reg->Regstring(as.c_str(), i);
    }
     reg->CloseKey();
return reg->Regstring();
}

//--------------------------------------------------------------------------
//          TRegistry::OpenKey имеряет длину ф. *.ini и в зависимомти
//      от аргумента smDes либо выходит и возвращает 0, либо создает раздел Кеу с именами параметров
//      если длина файла равна нолю.
// аргументы:
//         string Key - ключ раздела
//         bool smDes - запрещает/разрешает зозданое раздела Кеу с параметрами, при его отсутствии в файле
//       *.ini.
// возвращаемое значение:
//         0 -  при отсутствии в файле *.ini раздела Key
//         1 - если раздел нейден.
// коментарии:
//          функция - при вызове с smDes == 0 и нолевой длиной файла выходит с возвращаемым значенгием: 0
//     пр вызове c smDes == 1 - записывает в файл ключ раздела параметров и имена параметров,
//     указанные в строке const char **reg_str_param_name, класса TRegistry.
//          функция транслирует исключения выбрасываемые функциями:
//      OpenKey, WriteString, ReadString
//      В переспективи функция должна создавать раздел и в не нулевом файле
//---------------------------------------------------------------------------
 int TRegistry::OpenKey(string Key, bool smDes)
 {
#ifndef unix
    DWORD sz_file;
    DWORD er;
                if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
                {
                   er =  GetLastError();
                   throw EXP_ERROR("OpenKey","Cannot seek file", er);
                }


                if((sz_file = SetFilePointer(hFile, 0,NULL,FILE_END)) == INVALID_SET_FILE_POINTER)
                {
                    er =  GetLastError();
                    throw EXP_ERROR("OpenKey","Cannot seek file", er);
                }

                if(sz_file == 0)
                {
                     if(smDes == 0)
                         return 0;
                }

     size_t  wr_z;
#else
     off_t sz_file;
                lseek((int)hFile, 0,SEEK_SET);
                if((sz_file = lseek((int)hFile, 0,SEEK_END)) <= 0)
                {
                     if(smDes == 0)
                         return 0;
                }
     ssize_t  wr_z;
#endif
     string s;

             if(sz_file <= 0)
             {
                     s = "[";
                     s +=  Key ;
                     s+= "]";
                     s+= "\r\n";


                     for(int i = 0;i  < num_param; i++)
                     {
                              s +=  reg_str_param_name[i];
                              s += "=\r\n";
                     }
#ifndef unix
                    if(!WriteFile(hFile,s.c_str(),(DWORD)s.size(), (LPDWORD)&wr_z, NULL))
                    {
                        er = GetLastError();
                        throw EXP_ERROR("OpenKey","Cannot write file", er);
                    }
#else
                    wr_z = write(hFile, (const void*)s.c_str(), s.size());
                    if(wr_z != (ssize_t)s.size())
                    {
                        throw EXP_ERROR("OpenKey","Cannot write file", (DWORD)errno);
                    }
#endif
             }
     return 1;
 }

 //--------------------------------------------------------------------------
 //     TRegistry::СloseKey- Ничего не делает
 // аргументы:
 //          нет
 // возвращаемое значение:
 //          нет
 // коментарии:
 // Функция . введена на переспективу
 //---------------------------------------------------------------------------
 void TRegistry::CloseKey()
 {


 }

 //--------------------------------------------------------------------------
 //          TRegistry::WriteString ищет в файле *.ini строку параметра Param и записывает в нее
 //     значение sData
 // аргументы:
 //         const char * Param - параметр в файле *.ini, значение которого надо изменить
 //         const char* sData - значение на которое надо изменить параметр Param в файле *.ini.
 // возвращаемое значение:
 //         0 -  при отсутствии ошибок- если длина записываемого значения параметра совпала с
 //     количеством записанных в файл байт
 //         1 -  при наличии ошибок
 // коментарии:
 //          функция - записывает значение параметра
 //          функция выбрасывает исключения, если она не может:
 //    - установить указатель файла в конец или начало файла;
 //    - при не возможности выделить буфер для чтения и последующей записи в файл;
 //    - если ошибка произошла при чтении файла;
 //    - если в буфере не находится изменяемый параметр-Param;
 //    - если после Param не находится разделитель - "=";
 //    - если она не может записать буфер в файл.
 //---------------------------------------------------------------------------
 int TRegistry::WriteString(const char * Param, const char* sData)
 {
     DWORD size_data = (DWORD)strlen(sData);
     if(buffer != NULL)
             delete[] buffer;

#ifndef unix
long int sz_file;
DWORD er;

    if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        er =  GetLastError();
        throw EXP_ERROR("WriteString","Cannot seek file", er);
    }

    if((sz_file = SetFilePointer(hFile, 0, NULL, FILE_END)) == INVALID_SET_FILE_POINTER)
    {
        er =  GetLastError();
        throw EXP_ERROR("WriteString","Cannot seek file", er);
    }

#else
off_t sz_file;
     lseek((int)hFile, 0,SEEK_SET);
     if((sz_file = lseek((int)hFile, 0,SEEK_END)) <= 0)
     {
        throw EXP_ERROR("WriteString","Key not open key for write", (DWORD)errno);
     }
#endif

     if(sz_file  > 0)
     {
                buffer = new char[sz_file + 1];
                if(buffer == NULL)
                {
#ifdef unix
                    throw EXP_ERROR("WriteString","Cannot can not allocate memory", (DWORD)errno);
#else
                    er = GetLastError();
                    throw EXP_ERROR("WriteString","Cannot can not allocate memory", er);
#endif

                }
                memset(buffer, 0, sz_file + 1);
#ifndef unix
     DWORD rd_z;
                SetFilePointer(hFile, 0, NULL,FILE_BEGIN);
                if(!ReadFile(hFile,buffer,sz_file, &rd_z, NULL))
                {
                     er = GetLastError();
                     throw EXP_ERROR("WriteString","Cannot read file", er);
                }
#else
                lseek((int)hFile, 0,SEEK_SET);
     ssize_t rd_z = read(hFile,buffer,sz_file);
                if(rd_z != (ssize_t)sz_file)
                {
                     throw EXP_ERROR("WriteString","Cannot read file",(DWORD)errno);
                }

#endif
    }
string  s( buffer);
string  sub_s = Param;
        sub_s += "=";
size_t  beg = s.find(sub_s);

     if(beg == string::npos)
     {
     string  s("Cannot find value ");
             s += sub_s;
             s.erase(s.end() - 1);
             throw EXP_ERROR("WriteString", (char*)s.c_str());
     }

     beg =  s.find("=", beg);
     if(beg == string::npos)
     {
      string s("Cannot find param ='");
             s += sub_s;
             s.erase(s.end() - 1);
             throw EXP_ERROR( "WriteString",(char*)s.c_str());
     }
     beg++;

DWORD i,j;
     for(i = 0, j = 0; i < size_data; i++)
     {
         char c = (((char*)sData)[i]);
             s.insert((s.begin() + beg + j), c);
         j++;
     }
#ifndef unix
 DWORD wr_z;
        if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            er =  GetLastError();
            throw EXP_ERROR("WriteString","Cannot seek file",er);
        }
        if(!WriteFile(hFile,s.c_str(),(DWORD)s.size(), (LPDWORD)&wr_z, NULL))
        {
            er = GetLastError();
            throw EXP_ERROR("WriteString","Cannot write file", er);
        }
        return wr_z != (size_t)s.size();
#else
  off_t ls_z = lseek(hFile,0,SEEK_SET);
        if(ls_z < 0)
        {
           throw EXP_ERROR("WriteString","Cannot write file",(DWORD)errno);
        }
ssize_t wr_z = write(hFile, (const void*)s.c_str(), s.size());
        return wr_z != (ssize_t)s.size();
#endif
 }

 //--------------------------------------------------------------------------
 //          TRegistry::ReadString ищет в файле *.ini строку параметра Param и записывает его
 //     значение в строку выходного параметра string *out.
 // аргументы:
 //         const char * Param - параметр в файле *.ini, значение которого надо прочитать
 //         string *out - строка в которой возвращается значение параметра Param.
 // возвращаемое значение:
 //         функция возвращает ноль при выходе 0
 // коментарии:
 //          функция - читает значение параметра
 //          функция выбрасывает исключения, если она не может:
 //    - установить указатель файла в конец или начало файла;
 //    - при не возможности выделить буфер для чтения;
 //    - если ошибка произошла при чтении файла;
 //    - если в буфере не находится искомый параметр-Param;
 //    - если после Param не находится разделитель - "=";
 //    - если разделитель "=" не находится между Param и символом "\n"(нарушена структура файла)
 //    - если в значении параметра встречаются не цифры.
 //---------------------------------------------------------------------------
int TRegistry::ReadString(const char *prm, string *out)
 {
    if(buffer != NULL)
        delete[] buffer;
#ifndef unix
    DWORD er;
long int sz_file;
        if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        {
            er =  GetLastError();
            throw EXP_ERROR("ReadString","Can not open key for read", er);
        }
        if((sz_file = SetFilePointer(hFile, 0, NULL,FILE_END)) == INVALID_SET_FILE_POINTER)
        {            
            er =  GetLastError();
            throw EXP_ERROR("ReadString","Can not open key for read", er);
        }
#else
off_t sz_file;
        lseek((int)hFile, 0,SEEK_SET);
        if((sz_file = lseek((int)hFile, 0,SEEK_END)) <= 0)//
        {
            throw EXP_ERROR("ReadString","Can not open key for read",(DWORD)errno);
        }
#endif
        if(sz_file  > 0)
        {
                buffer = new char[sz_file + 1];
                if(buffer == NULL)
                {
#ifdef unix
                    throw EXP_ERROR("ReadString","Cannot can not allocate memory", (DWORD)errno);
#else
                    er = GetLastError();
                    throw EXP_ERROR("ReadString","Cannot can not allocate memory", er);
#endif
                }
                memset(buffer, 0, sz_file + 1);
#ifndef unix
        DWORD rd_z;
                if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
                {
                    er =  GetLastError();
                    throw EXP_ERROR("ReadString","Can not set file pointer", er);
                }
                if(!ReadFile(hFile,buffer,sz_file, &rd_z, NULL))
                {
                     er = GetLastError();
                     throw EXP_ERROR("ReadString","Can not read file", er);
                }
#else
                lseek((int)hFile, 0,SEEK_SET);
ssize_t rd_z = read(hFile,buffer,sz_file);
                if(rd_z != (ssize_t)sz_file)
                {
                    throw EXP_ERROR("ReadString","Can not read file", (DWORD)errno);

                }
#endif
        }

string  s(buffer);
string  sub_s = prm;

sub_s += "=";

size_t  beg = s.find(sub_s);
        if(string:: npos == (beg =  s.find("=", beg)))
        {
            string t("Can not find param: ");
            t += sub_s;
            t.erase(t.end() -1);
            throw EXP_ERROR("ReadString",t.c_str());
        }
        beg++;
size_t  end = s.find('\n',beg);

        if(beg < end)
         {
                string o(s,beg, end - beg);
                string t(s,beg, end -  beg);

                if(!(end - 1 - beg))
                {
                    string t("Can not read value of param: ");
                    t += sub_s;
                    t.erase(t.end() -1);
                    throw EXP_ERROR("ReadString",t.c_str());
                }
                for(WORD i = 0; i < (end - 1 - beg); i++)
                        if((isdigit(t.c_str()[i])) )
                            continue;
                        else
                        {
                            string t("Can not read value of param: ");
                            t += sub_s;
                            t.erase(t.end() -1);
                            throw EXP_ERROR("ReadString",t.c_str());
                         }
                *out = o;
         }
        else
         {
            string t("Can not read value of param: ");
            t += sub_s;
            t.erase(t.end() -1);
            throw EXP_ERROR("ReadString",t.c_str());
         }
return 0;
 }
//--------------------------------------------------------------------------
//         WriteParamTooReg ищет в файле *.ini созданного ранее классом TRegistry строку параметра Param и записывает
//     его новое.
// аргументы:
//         TRegistry *reg - указатель на класс
//         const char * Param - параметр в файле *.ini, значение которого надо записать
//         char *Value - указатель на строку значения параметра Param
// возвращаемое значение:
//         0 - при отсутствии ошибок
//         1,2,3 при их наличии
// коментарии:
//          функция - открывает раздел записанный в строке key->regkey
//                  - удаляет значение параметра Param в файле
//                  - записывает новое знаяение Value в парпаметр Param в файле *.ini
//          функция обрабатывает исключения при выбросе ошибок ф.OpenKey,DeleteValue ,WriteString
//---------------------------------------------------------------------------
int WriteParamTooReg(TRegistry *reg, const char* Param, char *Value)
{
    string  ch(reg->Regkey());
    try
    {
    reg->OpenKey(ch,true);
    }
    catch(EXP_ERROR &e)
    {
        e.ErrorSetText("p_reg_dll - WriteParamTooReg: OpenKey");
        reg->CloseKey();
        return 1;
    }


    try
    {
     reg->DeleteValue(Param);
    }
    catch(EXP_ERROR &e)
    {
        e.ErrorSetText("p_reg_dll - WriteParamTooReg: DeleteValue");
        reg->CloseKey();
        return 2;
    }

    try
    {
    reg->WriteString(Param,Value);
    }
    catch(EXP_ERROR &e)
    {
        e.ErrorSetText("p_reg_dll - WriteParamTooReg: WriteString");
        reg->CloseKey();
        return 3;
    }
    reg->CloseKey();
  return 0;
}

//--------------------------------------------------------------------------
//          TRegistry::DeleteValue ищет в файле *.ini строку параметра lpParam и
//      удаляет его значение
// аргументы:
//         const char * Param - параметр в файле *.ini, значение которого надо удалить
// возвращаемое значение:
//         функция возвращает ноль при выходе 0

// коментарии:
//          функция - удаляет значение параметра
//          функция выбрасывает исключения, если она не может:
//    - установить указатель файла в конец или начало файла;
//    - при не возможности выделить буфер для чтения;
//    - если ошибка произошла при чтении файла;
//    - если в буфере не находится искомый параметр-Param;
//    - если после Param не находится разделитель - "=";
//    - если разделитель "=" не находится между Param и символом "\n"(нарушена структура файла)
//    - при ошибке записи измененого буфера в файл
//---------------------------------------------------------------------------
int TRegistry::DeleteValue(const char* lpParam)
{
            if(buffer != NULL)
                    delete[] buffer;
#ifndef unix
long int    sz_file;
DWORD er;
            if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
            {
                er =  GetLastError();
                throw EXP_ERROR("DeleteValue","Can not open key for read", er);
            }

            if((sz_file = SetFilePointer(hFile, 0, NULL,FILE_END)) == INVALID_SET_FILE_POINTER)
            {
                er =  GetLastError();
                throw EXP_ERROR("DeleteValue","Can not open key for read", er);
            }
#else
    off_t sz_file;
            lseek((int)hFile, 0,SEEK_SET);
            if((sz_file = lseek((int)hFile, 0,SEEK_END)) <= 0)
            {
                throw EXP_ERROR("DeleteValue","Can not open key for delete", (DWORD)errno);
            }
#endif
            if(sz_file  > 0)
            {
                buffer = new char[sz_file + 1];
                if(buffer == NULL)
                {
#ifdef unix
                    throw EXP_ERROR("DeleteValue","Cannot can not allocate memory", (DWORD)errno);
#else
                    er = GetLastError();
                    throw EXP_ERROR("DeleteValue","Cannot can not allocate memory", er);
#endif
                }
                memset(buffer, 0, sz_file + 1);
#ifndef unix
DWORD rd_z;
                if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
                {
                    er =  GetLastError();
                 throw EXP_ERROR("DeleteValue","Can not open key for read", er);
                }
                if(!ReadFile(hFile,buffer,sz_file, &rd_z, NULL))
                {
                    er = GetLastError();
                    throw EXP_ERROR("DeleteValue","Can not read file", er);
                }
#else
                lseek((int)hFile, 0,SEEK_SET);
                ssize_t rd_z = read(hFile,buffer,sz_file);
                    if(rd_z != (ssize_t)sz_file)                    
                        throw EXP_ERROR("DeleteValue","Can not read file", (DWORD)errno);

#endif
            }

    string  s( buffer);
    string  sub_s = lpParam;
    sub_s += "=";
    size_t  beg = s.find(sub_s);
            if(beg == string::npos)
            {
             string s("Can not find param ");
                s += sub_s;
                s.erase(s.end() -1);
                throw EXP_ERROR("DeleteValue", s.c_str());
            }

            beg =  s.find("=", beg);
            if(beg == string::npos)
            {
             string s("Cannot find param = ");
                s += sub_s;
                s.erase(s.end() -1);
                throw EXP_ERROR("DeleteValue", s.c_str(), (WORD)5);
            }

    size_t end = s.find("\r", beg);
            if(beg == string::npos)
            {
            string s("Cannot find param '\r' ");
                s += sub_s;
                 s.erase(s.end() -1);
                 throw EXP_ERROR("DeleteValue", s.c_str(), (WORD)6);
            }
            beg++;
            if(end > beg)
                s.erase(beg, end - beg);
#ifndef  unix
            if(SetFilePointer(hFile, 0, NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
            {
                    er =  GetLastError();
                    throw EXP_ERROR("DeleteValue","Can not open key for delete", er);
            }

#else
            lseek((int)hFile, 0,SEEK_SET);
#endif

DWORD num = (off_t)s.length() > sz_file ? (DWORD)s.length() : (DWORD)sz_file;
#ifndef unix
DWORD wr_z;
            if(!WriteFile(hFile,s.c_str(), num, (LPDWORD)&wr_z, NULL))
            {
                er = GetLastError();
                throw EXP_ERROR("DeleteValue","Can not write file", er);
            }
#else
    size_t wr_z = write(hFile, (const void*)s.c_str() , num);
#endif
            if(wr_z != (size_t)num)
            {
                throw EXP_ERROR("DeleteValue","Can not write file");
            }

        return 0;
}

//--------------------------------------------------------------------------
//          DWORD reg_GetError возвращает ошибки, фиксируемые исключительными ситуациями
// аргументы:
//         нет
// возвращаемое значение:
//         1 - наличие ошибки
//         0 - ошибок не было
// коментарии:
//         нет
//---------------------------------------------------------------------------
DWORD reg_GetError(char **text)
{
    DWORD ret;
    if((ret = EXP_ERROR::ErrorGet()))
        *text = (char*)EXP_ERROR::GetTextError()->c_str();
    else
        *text = NULL;
    return ret;
}
