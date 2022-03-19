//---------------------------------------------------------------------------




#ifdef __BORLANDC__
		#pragma hdrstop
		#include <mem.h>
#endif

#ifdef _MSC_BUILD
	#define _CRT_SECURE_NO_WARNINGS
	#ifndef _CRT_SECURE_NO_DEPRECATE
	# define _CRT_SECURE_NO_DEPRECATE (1)
	#include <wchar.h>
#endif
# pragma warning(disable : 4996)
#endif

#include <string.h>

#include <tiffio.h>
#include "tiff_dll.h"


TIFF *tiff = (TIFF*)NULL;
char *image = (char*)NULL;
uint32 width;
uint32 height;
uint32* raster;
uint32 error;
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
	#pragma package(smart_init)
#endif

#ifdef unix
    #include <dlfcn.h>
    #include <dlfcn.h>
    static char *ptr_e;
    void *tiff_lib;
    typedef TIFF* (*TIFFOPEN)(const char*, const char*);
    typedef int (*TIFFSETFIELD)(TIFF*,uint32, ...);
    typedef void* (* TIFFMALLOC)(tmsize_t);
    typedef uint32 (*TIFFDEFAULTSTRIPSIZE)(TIFF* tif, uint32 request);
    typedef int (*TIFFWRITESCANLINE)(TIFF* tif, void* buf, uint32 row, uint16 sample);
    typedef void (*TIFFCLOSE)(TIFF*);
    typedef void (*TIFFFREE)(void*);




    TIFFOPEN g_TIFFOpen;
    TIFFSETFIELD g_TIFFSetField;
    TIFFMALLOC   g_TIFFmalloc;
    TIFFDEFAULTSTRIPSIZE g_TIFFDefaultStripSize;
    TIFFWRITESCANLINE g_TIFFWriteScanline;
    TIFFCLOSE g_TIFFClose;
    TIFFFREE g_TIFFfree;

//----------------------------------------------------------------------
    void tiff_Init()
    {
        tiff_lib = dlopen("libtiff.so",RTLD_LAZY);
        if(!tiff_lib)
        {
        char e[1000];
            strcpy(e, dlerror());
            fprintf(stderr,"%s",e);
        return;
        }

        g_TIFFOpen  = (TIFFOPEN)dlsym(tiff_lib,"TIFFOpen");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }

        g_TIFFSetField = (TIFFSETFIELD)dlsym(tiff_lib,"TIFFSetField");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }

        g_TIFFmalloc = (TIFFMALLOC)dlsym(tiff_lib,"_TIFFmalloc");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }

        g_TIFFDefaultStripSize = (TIFFDEFAULTSTRIPSIZE)dlsym(tiff_lib,"TIFFDefaultStripSize");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }

        g_TIFFWriteScanline = (TIFFWRITESCANLINE)dlsym(tiff_lib,"TIFFWriteScanline");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }

        g_TIFFClose = (TIFFCLOSE)dlsym(tiff_lib,"TIFFClose");
        if((ptr_e = dlerror()) != NULL)
        {
            char e[1000];
            strcpy(e, ptr_e);
            fprintf(stderr,"%s",e);
         return;
        }

        g_TIFFfree = (TIFFFREE)dlsym(tiff_lib,"_TIFFfree");
        if((ptr_e = dlerror()) != NULL)
        {
             char e[1000];
             strcpy(e, ptr_e);
             fprintf(stderr,"%s",e);
          return;
        }

    }

#endif


//-----------------------------------------------------------------------------------
void tiff_Write(const char *tiff_name, WORD w, WORD h, char *data)
{
int sampleperpixel = 4;
int bit_per_samples = 8;
#ifdef unix
    TIFFOPEN TIFFOpen = g_TIFFOpen;
    TIFFSETFIELD TIFFSetField = g_TIFFSetField;
    TIFFMALLOC _TIFFmalloc = g_TIFFmalloc;
    TIFFFREE   _TIFFfree = g_TIFFfree;
    TIFFDEFAULTSTRIPSIZE TIFFDefaultStripSize = g_TIFFDefaultStripSize;
    TIFFWRITESCANLINE TIFFWriteScanline = g_TIFFWriteScanline;
  //  TIFFCLOSE TIFFClose =     g_TIFFClose;
#endif
        if(tiff != NULL)
                tiff_Close();
        tiff = TIFFOpen(tiff_name, "w");
        if(tiff == NULL)
        {
                error |= 1;
        }
        image=new char [w * h * sampleperpixel ];


        if(image == NULL)
        {
                error |= 2;
        }

        memcpy(image, data,  w * h * sampleperpixel/*w * 10 *sampleperpixel*/ );
        
        TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH,width = w);  //
        TIFFSetField(tiff, TIFFTAG_IMAGELENGTH,height = h);    //
        TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   //
        TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bit_per_samples);    //
        TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPRIGHT);    //    /*ORIENTATION_TOPLEFT*/

        TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

        tsize_t linebytes = sampleperpixel * width ;
        unsigned char *buf = NULL;
        buf =(unsigned char *)_TIFFmalloc(linebytes);

        TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiff, width * sampleperpixel ));

        for (uint32 row = 0; row < (uint32)h; row++)
        {
                memcpy(buf, &image[(h-row-1)*linebytes], linebytes);
                if (TIFFWriteScanline(tiff, buf, row, 0) < 0)
                break;
        }
        if(tiff != NULL)
                tiff_Close();
        if(buf != NULL)
        {
            _TIFFfree(buf); buf = NULL;
        }
}
//------------------------------------------------------------------------------
bool tiff_Write16(const char *tiff_name, WORD width, unsigned long height, char *data){

#ifdef unix
    TIFFOPEN TIFFOpen = g_TIFFOpen;
    TIFFSETFIELD TIFFSetField = g_TIFFSetField;
    TIFFMALLOC _TIFFmalloc = g_TIFFmalloc;
    TIFFFREE   _TIFFfree = g_TIFFfree;
    //TIFFDEFAULTSTRIPSIZE TIFFDefaultStripSize = g_TIFFDefaultStripSize;
    TIFFWRITESCANLINE TIFFWriteScanline = g_TIFFWriteScanline;
    TIFFCLOSE TIFFClose =     g_TIFFClose;
#endif


    TIFF* tiff = TIFFOpen(tiff_name, "w");
    if(!tiff)
        return false;

    TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    tsize_t linebytes = (tsize_t)width * 2;
    uint16 *buf = NULL;
    buf =(uint16 *)_TIFFmalloc(linebytes);
    TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, height);

    for (uint32 row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++) {
            buf[col] = (uint16)((WORD*)data)[ row * width +  col];
        }
        if (TIFFWriteScanline(tiff, buf, row, 0) < 0)
        break;
    }
    TIFFClose(tiff);
    if (buf)
        _TIFFfree(buf);
    return true;
}
//---------------------------------------------------------------------------
void tiff_Read(char *tiff_name)
{
        if(tiff != NULL)
            tiff_Close();
        tiff = TIFFOpen(tiff_name, "r");
        TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
        uint32 n_pixels = width * height;
        raster =  (uint32*)_TIFFmalloc(n_pixels *sizeof(uint32));
        if(!TIFFReadRGBAImage(tiff, width, height, (unsigned int*)raster, 0))
        {
                error |= 4;
        }
        if(tiff != NULL)
                tiff_Close();        
}


//----------------------------------------------------------------------
void tiff_Close()
{
#ifdef unix
        TIFFFREE  _TIFFfree = g_TIFFfree;
        TIFFCLOSE TIFFClose = g_TIFFClose;
#endif

 if(image != NULL)
 {
        delete[] image;
        image = 0;
 }

 if(raster != NULL)
        {
        _TIFFfree(raster);
        raster = NULL;
        }

 if(tiff != NULL)
        {
       TIFFClose(tiff);
       tiff = NULL;
       }
}
//--------------------------------------------------------------------------
uint32 tiff_Error()
{
 return error;
}
