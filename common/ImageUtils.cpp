/*
 * ImageUtils.cpp
 *
 *  Created on: 23 de mai de 2017
 *      Author: david
 */
#include <stdlib.h>
#include <ImageUtils.h>
#define PNG_DEBUG 3
#include <png.h>
#include "png++/png.hpp"
#include "BitMapUtil.h"

//using namespace png;
namespace Utils {

Image::Image(std::string* path) {
	readPNG8B(path);
	name 	= path;
}

Image::~Image() {
	if(this->imgBuff) free(this->imgBuff);
}

int Image::saveBitMap(){
	return Image::saveBitMap(*name);
}

int Image::saveBitMap(std::string &path/*, int bitsPerColor, int width,int height ,unsigned char* imgBuff*/){
	return saveBitMapFile(path.c_str(),width,height,bitsPerColor,imgBuff);
}

void Image::convertFor1Bit(){
	unsigned char * tmpImgB;
	unsigned char * tmpImg1B;

	binarizeRawGrayScale(imgBuff,width,height,&tmpImgB);

	if(imgBuff != NULL) free(imgBuff);

	int sizeB= convert8BitsTo1BitBW(tmpImgB,&width,height,&tmpImg1B);


	free(tmpImgB);
	imgBuff = tmpImg1B;
	bitsPerColor = 1;
	imgBuffLen = sizeB;
}

 void Image::readPNG8B(std::string* path){

	png::image< png::gray_pixel>*  imgPng = new png::image< png::gray_pixel>(*path);
	 //png::image< png::gray_pixel>*  img = new png::image< png::gray_pixel>(path);
	height = (int) imgPng->get_height();
	width  = (int) imgPng->get_width();
	bitsPerColor = 8;

	printf("height %d width %d \n",height,width);

	imgBuffLen = (width * height)+1;
	imgBuff =(unsigned char *) malloc(imgBuffLen);
	memset(imgBuff,0,imgBuffLen);

	int ind = 0;
	for (size_t y = 0; y < imgPng->get_height(); ++y)
	{
		for (size_t x = 0; x < imgPng->get_width(); ++x)
		{
			imgBuff[ind++] = (unsigned char) imgPng->get_pixel(x,y);
		}
	}

	//saveBitMapFile("/tmp/BBX_TEST_2.bmp", width,height,8,this->imgBuff);
	unsigned char * tmpImg = NULL;
	unsigned char * imgBin = NULL;
	int tmpWidth =width;

	saveBitMapFile("/tmp/BBX_TEST_ORIGINAL.bmp", width,height,8,this->imgBuff);

	binarizeRawGrayScale(imgBuff,width,height,&tmpImg);

	saveBitMapFile("/tmp/BBX_TEST_8BIT_COR.bmp", width,height,8,tmpImg);

	convert8BitsTo1BitBW(tmpImg,&tmpWidth,height,&imgBin);
	printf("buff[%p]tmpImg[%p],imgBin[%p]\n",imgBuff,tmpImg,imgBin);

	saveBitMapFile("/tmp/BBX_TEST_1BIT_COR.bmp", tmpWidth,height,1,imgBin);

	free(tmpImg);
	free(imgBin);
	delete imgPng;
}


} /* namespace Utils */
