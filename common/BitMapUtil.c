/*
 * BitMapUtil.c
 *
 *  Created on: 24 de mai de 2017
 *      Author: david
 */


#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<BitMapUtil.h>



int saveBitMapFile(const char* filePath,int width,int height, int bitsPerColor,const unsigned char * imagem){
	  int ret = 0;
	 printf("iniciando save BMP\n");
	 // char * x = (char *)malloc(1024);

	  BITMAPFILEHEADER* bmpH = (BITMAPFILEHEADER*) 	malloc(sizeof(BITMAPFILEHEADER));
	  BITMAPINFOHEADER* bmpI = (BITMAPINFOHEADER*)	malloc(sizeof(BITMAPINFOHEADER));
	  BMP_PALETTE* 		bmpP = (BMP_PALETTE     *)	malloc(sizeof(BMP_PALETTE));
	  memset(bmpH,0,sizeof(BITMAPFILEHEADER));
	  memset(bmpI,0,sizeof(BITMAPINFOHEADER));
	  memset(bmpP,0,sizeof(BMP_PALETTE));

	  //Setting info header
	  bmpI->biSize     = sizeof(BITMAPINFOHEADER);
	  bmpI->biBitCount = bitsPerColor;
	  bmpI->biPlanes   = 1;
	  bmpI->biWidth    = width;
	  bmpI->biHeight   = 0 - height ; //the value is negative  to avoid upside-down image display
	  //Setting header
	  bmpH->bfType1     = 'B';
	  bmpH->bfType2     = 'M';
	  int imgSize 		= ((bitsPerColor * (width * height)) / 8);
	  //int imgSize =  (int)((float)bitsPerColor * ((float)width * (float)height)) % 8 != 0 ? (((float)bitsPerColor *  ((float)width * (float)height)) /(float)8) + 1 : (((float)bitsPerColor *  ((float)width * (float)height)) /(float)8);
	  bmpH->bOffset     = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+  (4*256);
	  bmpH->bfSize      = bmpH->bOffset  + imgSize;  //+ sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+(4*256);   //8 * OKI_IMG_HEIGHT * OKI_IMG_WIDTH * IMG_COLOR_DEPTH;
	  printf("alocou e vai abrir arquivo BMP\n");

	  FILE* tmpHandle =  fopen(filePath,"wb+");

	  if(tmpHandle>0){
		if(bmpH && bmpI && bmpP)
		printf("File created [%s] FD[%p]\n",filePath,tmpHandle);
		fwrite(bmpH,(size_t)sizeof(BITMAPFILEHEADER),1,tmpHandle);
		fwrite(bmpI,(size_t)sizeof(BITMAPINFOHEADER),1,tmpHandle);
		printf("gravou header do BMP\n");
		// Creating BMP palette 256 gray scale.
		for(int i = 0; i< 256;i++){
			if(bmpI->biBitCount==1 && i > 0){
				bmpP->B = 255;
				bmpP->G = 255;
				bmpP->R = 255;
			}else{
				bmpP->B = i;
				bmpP->G = i;
				bmpP->R = i;
			}
			fwrite(bmpP,(size_t)sizeof(BMP_PALETTE),1,tmpHandle);
		}
		int addBytes = (4 * ((width / 4)+ 1)) - width;
		unsigned char padd[addBytes];
		for (int indPix = 0; indPix < imgSize; indPix+=width) {
			fwrite((imagem + indPix),(size_t) width,1,tmpHandle);
			if(width % 4!=0){
				fwrite(&padd,(size_t) addBytes,1,tmpHandle);
			}
		}

		fflush(tmpHandle);
		fclose(tmpHandle);
	  }else{
		printf("file open Error [%s] FD[%p] \n",filePath,tmpHandle);
		ret = BITMAP_FILE_OPEN_ERROR;
	  }

	   free(bmpH);
	   free(bmpI);
	   free(bmpP);
	   return ret;
}

//int convert8BitsTo1BitBW(unsigned char * img,int width, int height,unsigned char ** out){
//	int nWidth = width;
//	while(nWidth % 8 !=0){
//		  nWidth++;
//	}
//
//	int size =  ((nWidth * height) /8)+1;//(width * height) % 8 != 0 ? ((width * height) /8)+1 : ((width * height) /8);
//	printf("width[%d] nWidth[%d] size[%d]\n",width,nWidth,size);
//	unsigned char* imgOut =(unsigned char* ) malloc(size);
//
//	int indOut =0;
//	int indIn  =0;
//	int curWidth  =0;
//	unsigned char packagePix =0;
//
//	while (indIn < (width * height))
//	{
//
//		for(int b=0; b < 8; b++){
//			packagePix =  (packagePix << 1) + (img[indIn++] > 1 ? 1  : 0);
//			if(curWidth++ >= width){
//				curWidth=0;
//				break;
//			}
//		}
//
//		imgOut[indOut++] = packagePix;
//		packagePix =0;
//	}
//	*out = imgOut;
//	return size;
//}
//

int convert8BitsTo1BitBW(const unsigned char * img,int* width, int height,unsigned char** out){
  unsigned int 	nWidth = *width;
  while(nWidth % 8 !=0){
	nWidth++;
  }
  size_t size =  2*((nWidth * height) /8);//(width * height) % 8 != 0 ? ((width * height) /8)+1 : ((width * height) /8);
  printf("width[%d] nWidth[%d] size[%d]\n",*width,nWidth,size);

  unsigned char* imgOut =(unsigned char* ) malloc(size);
  memset(imgOut,0,size);

  int indOut =0;
  int indIn  =0;
  int indW   =0;

  unsigned char packagePix =0;

  while (indIn < ((*width) * height))
  {
	for(int b=0; b < 8; b++){
		if(indW < *width || *width == nWidth){
			indW++;
			packagePix =  (packagePix << 1) + (img[indIn++] > 127 ? 0  : 1);
		}else{
			indW = 0;
			packagePix =  (packagePix << (7 - (b-1)));
			break;
		}

	}
	imgOut[indOut++] = packagePix;
	packagePix =0;
  }
  *out   = imgOut;
  *width = nWidth;
  return size;
}


int binarizeRawGrayScale(const unsigned char * img, int width, int height,unsigned char ** dithImg){
	  //unsigned char* dithImg =(unsigned char* ) malloc(((width * height) /8));
	  int size = ((width * height));
	  //unsigned char* dithImg = (unsigned char* ) malloc(size);
	  *dithImg = (unsigned char* ) malloc(size+2);
	  memcpy(*dithImg,img,size);
	  int ind =0,x=0,y=0;
	  int16_t err=0;
	  int16_t a=0,b=0,c=0,d=0,curr=0;

	  while(ind < size)
	  {

	      curr =(*dithImg)[ind];
	      if(curr > 127)
	      {
	        err = curr - 255;
	        (*dithImg)[ind] = 255;
	      }
	      else
	      {
	        err = curr - 0;
	        (*dithImg)[ind] = 0;
	      }

	      a = (err * 7) / 16;
	      b = (err * 1) / 16;
	      c = (err * 5) / 16;
	      d = (err * 3) / 16;


//	      //if((i != (height-1)) && (j != 0) && (j != (width - 1)))
//	      if((ind+width +1) < size && (y != (height-1)) && (x > 0) && (x < (width - 1)) )
//	      //if( (ind+width +1) < size && ind % width !=0)
//	      {
	        /*
	        dithImg.at<uint8_t>(i+0,j+1) = saturated_add(dithImg.at<uint8_t>(i+0,j+1),a);
	        dithImg.at<uint8_t>(i+1,j+1) = saturated_add(dithImg.at<uint8_t>(i+1,j+1),b);
	        dithImg.at<uint8_t>(i+1,j+0) = saturated_add(dithImg.at<uint8_t>(i+1,j+0),c);
	        dithImg.at<uint8_t>(i+1,j-1) = saturated_add(dithImg.at<uint8_t>(i+1,j-1),d);
	        */
	    	  //printf("Ind normal[%d]\n",ind);
	    	 if(ind + 1         < size) (*dithImg)[ind + 1] 			= saturated_add ((*dithImg)[ind+1] 		,a);
	    	 if(ind + width + 1 < size) (*dithImg)[ind + width + 1] 	= saturated_add ((*dithImg)[ind+width+1]	,b);
	    	 if(ind + width     < size) (*dithImg)[ind+width] 		= saturated_add	((*dithImg)[ind+width] 	,c);
	    	 if(ind+width   - 1 < size) (*dithImg)[ind+width-1] 		= saturated_add ((*dithImg)[ind+width-1]	,d);

	      //}

	      if(x<= width){
	   			  x++;
	   		  }else{
	   			  x=0;
	   			  y++;
	   		  }

	      ind++;
	  }
	  //*out = dithImg;
	  return size;
}


unsigned char saturated_add(int16_t val1, int16_t val2){
	    int16_t val1_int = val1;
	    int16_t val2_int = val2;
	    int16_t tmp = val1_int + val2_int;

	    if(tmp > 255)
	    {
	      return 255;
	    }
	    else if(tmp < 0)
	    {
	      return 0;
	    }

	    return (unsigned char) tmp;

}

