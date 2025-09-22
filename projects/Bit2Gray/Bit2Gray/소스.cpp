#pragma comment(lib, "tiff.lib")

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"


typedef struct BitData {
	unsigned char bit7 : 1;
	unsigned char bit6 : 1;
	unsigned char bit5 : 1;
	unsigned char bit4 : 1;
	unsigned char bit3 : 1;
	unsigned char bit2 : 1;
	unsigned char bit1 : 1;
	unsigned char bit0 : 1;
}BitData;

// 입력 받은 파일 경로는 argv[1]로 넘어온다.
int main(int argc, char** argv)
{
	int width;
	int height;

	float xres;
	float yres;
	
	int size_bit_data;
	int size_gray_data;

	int grayindex = 0;

	TIFF* infile;
	TIFF* outfile;

	infile = TIFFOpen("input.tif", "rb");
	outfile = TIFFOpen("output.tif", "wb");

	TIFFGetField(infile, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(infile, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(infile, TIFFTAG_XRESOLUTION, &xres);
	TIFFGetField(infile, TIFFTAG_YRESOLUTION, &yres);

	size_bit_data = (width + 7) / 8;
	size_gray_data = size_bit_data * 8;

	BitData* bitdata = (BitData*)calloc(size_bit_data, sizeof(BitData));
	BYTE* graydata = (BYTE*)calloc(size_gray_data, sizeof(BYTE));


	TIFFSetField(outfile, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(outfile, TIFFTAG_IMAGELENGTH, height);
	//TIFFSetField(output, TIFFTAG_ICCPROFILE, icc_profile);
	TIFFSetField(outfile, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(outfile, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(outfile, TIFFTAG_ROWSPERSTRIP, height);
	TIFFSetField(outfile, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(outfile, TIFFTAG_XRESOLUTION, xres);
	TIFFSetField(outfile, TIFFTAG_YRESOLUTION, yres);
	TIFFSetField(outfile, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(outfile, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(outfile, TIFFTAG_PHOTOMETRIC, 0);
	TIFFSetField(outfile, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
	TIFFSetField(outfile, TIFFTAG_COMPRESSION, COMPRESSION_NONE);


	for (int i = 0; i < height; i++)
	{
		grayindex = 0;
		memset(graydata, 0, size_gray_data);
		memset(bitdata, 0, size_bit_data);
		TIFFReadScanline(infile, bitdata, i, 0);

		for (int j = 0; j < size_bit_data; j++)
		{
			graydata[grayindex++] = bitdata[j].bit0 * 255;
			graydata[grayindex++] = bitdata[j].bit1 * 255;
			graydata[grayindex++] = bitdata[j].bit2 * 255;
			graydata[grayindex++] = bitdata[j].bit3 * 255;
			graydata[grayindex++] = bitdata[j].bit4 * 255;
			graydata[grayindex++] = bitdata[j].bit5 * 255;
			graydata[grayindex++] = bitdata[j].bit6 * 255;
			graydata[grayindex++] = bitdata[j].bit7 * 255;
		}

		TIFFWriteScanline(outfile, graydata, i, 0);
	}

	TIFFClose(infile);
	TIFFClose(outfile);

	free(bitdata);
	free(graydata);
	return 0;
}