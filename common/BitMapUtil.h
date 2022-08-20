/*
       __   _____   __________  _______
      /L_| /  _ /\ /\_________\/\______\
     /L_/ / /\__\/ \/___   ___//  _____/
    /L_/ / /_/_        /L_/   \\  \___
   /L_/ /_ _ _/\      /L_/    _\\___  \
   L_/  \_____\/      L_/     \\______/

 =========================================================================================
 Name        : BitMapUtil.h
 Author      : David Vladimir S.Azevedo david.azevedo@grupoicts.com.br,davidvlad@gmail.com
 Version     : 0.1
 Description : Data types and struct to generating a BMP file
 =========================================================================================
 DATA        |  Description
 ------------+----------------------------------------------------------------------------
 20/03/2017  | Project Start
 =========================================================================================

    1 ) Write the BITMAPFILEHEADER to file
    2 ) Write the BITMAPINFOHEADER to file
    3 ) Write the BMP_PALETTE N x the number of colors in the palette.
	4 ) Write the image bytes padding in rows multiples of 32 bits.
 */


#ifndef BITMAPUTIL_H_
#define BITMAPUTIL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define BITMAP_FILE_OPEN_ERROR -1;
#define BITMAP_MALLOC_ERROR    -2;

#define BITMAP_HEADER {'B','M'}

#pragma pack(push, 1)  // DO NOT REMOVE, used to set memory byte alignment to 1 byte


typedef struct tagBITMAPFILEHEADER
{
    char  			bfType1;      //specifies the file type
    char 			bfType2;      //specifies the size in bytes of the bitmap file
    unsigned int    bfSize;  	  //specifies the size in bytes of the bitmap file
    char            bfReserved1;  //reserved; must be 0
    char            bfReserved2;  //reserved; must be 0
    char            bfReserved3;  //reserved; must be 0
    char            bfReserved4;  //reserved; must be 0
    unsigned int    bOffset;      //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER
{
	unsigned int     biSize;  //specifies the number of bytes required by the struct
	         int     biWidth;  //specifies width in pixels
	         int     biHeight;  //species height in pixels
    unsigned short   biPlanes; //specifies the number of color planes, must be 1
    unsigned short   biBitCount; //specifies the number of bit per pixel
    unsigned int  	 biCompression;//spcifies the type of compression
    unsigned int     biSizeImage;  //size of image in bytes
             int	 biXPelsPerMeter;  //number of pixels per meter in x axis
             int	 biYPelsPerMeter;  //number of pixels per meter in y axis
    unsigned int     biClrUsed;  //number of colors used by th ebitmap
    unsigned int     biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

typedef struct {
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char TERM;
} BMP_PALETTE;


#pragma pack(pop)

int saveBitMapFile(const char* filePath,int width,int height, int bitsPerColor,const unsigned char * imgBuff);
int convert8BitsTo1BitBW(const unsigned char * img,int* width, int height,unsigned char ** out);
int binarizeRawGrayScale(const unsigned char * img,int width, int height,unsigned char ** out);
unsigned char saturated_add(int16_t val1, int16_t val2);



#ifdef __cplusplus
}
#endif

#endif /* BITMAPUTIL_H_ */
