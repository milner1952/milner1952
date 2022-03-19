//---------------------------------------------------------------------------


#pragma hdrstop
#include <mem.h>
#include "tiffio.h"
#include "tiff_dll.h"

TIFF *tiff = NULL;
char *image = NULL;
uint32 width;
uint32 height;
uint32* raster;
uint32 error;
//---------------------------------------------------------------------------

#pragma package(smart_init)

//----------------------------------------------------------------------------
void tiff_Close();

//-----------------------------------------------------------------------------------
void tiff_Write(char *tiff_name, WORD w, WORD h, char *data)
{
int sampleperpixel = 4;
int bit_per_samples = 8;
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
        
        TIFFSetField (tiff, TIFFTAG_IMAGEWIDTH,width = w);  // ширина
        TIFFSetField(tiff, TIFFTAG_IMAGELENGTH,height = h);    // высота
        TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);   // количество каналов на пиксель
        TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, bit_per_samples);    // ширина одного канала пикселя
        TIFFSetField(tiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPRIGHT);    // ориентация байт в слове    /*ORIENTATION_TOPLEFT*/

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
