#pragma once

#include <GL/glut.h>


class BMPLoader
{
private:
	BITMAPFILEHEADER header{};
	BITMAPINFOHEADER info{};
public:
	unsigned char* m_Data;
	int m_Width;
	int m_Height;
	int m_NRChannels;
public:
	BMPLoader(std::string& filename);
	~BMPLoader();
};
