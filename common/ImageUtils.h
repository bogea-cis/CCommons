/*
 * ImageUtils.h
 *
 *  Created on: 23 de mai de 2017
 *      Author: david
 *      TODO: ADICIONAR SUPORTE A OUTROS FORMATOS e CORES (SUPORTA GRAYSCALE e B&W 1Bit em PNG E BMP)
 */

#ifndef COMMON_IMAGEUTILS_H_
#define COMMON_IMAGEUTILS_H_


#include <iostream>
namespace Utils {

class Image{

public:
    /** Open PNG FILE TODO: ADD suport for others formats and update bitPercolor and size*/
	Image(std::string* name);
	Image(int bitsPerColor,int width, int height, unsigned char* imgBuff);
	Image(int bitsPerColor,int width, int height, std::string* name,unsigned char* imgBuff);
	virtual ~Image();


	int getBitsPerColor() const {
		return bitsPerColor;
	}

	void setBitsPerColor(int bitsPerColor) {
		this->bitsPerColor = bitsPerColor;
	}

	int getHeight() const {
		return height;
	}

	void setHeight(int height) {
		this->height = height;
	}

	unsigned char* getImgBuff() const {
		return imgBuff;
	}

	void setImgBuff(unsigned char* imgBuff) {
		this->imgBuff = imgBuff;
	}

	const std::string* getName()  {
		return name;
	}

	void setName(std::string* name) {
		this->name = name;
	}

	int getWidth() const {
		return width;
	}

	void setWidth(int width) {
		this->width = width;
	}

	int getImgBuffLen() const {
		return imgBuffLen;
	}

	void convertFor1Bit();
	int saveBitMap();
	int saveBitMap(std::string &path);

private:

	void readPNG8B(std::string* path);

	int bitsPerColor;
	int height;
	int width;
	std::string* name;
	unsigned char * imgBuff;
	int imgBuffLen;
};




} /* namespace Utils */

#endif /* COMMON_IMAGEUTILS_H_ */
