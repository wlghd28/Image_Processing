#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"

typedef struct BIT {
	unsigned char bit7 : 1;
	unsigned char bit6 : 1;
	unsigned char bit5 : 1;
	unsigned char bit4 : 1;
	unsigned char bit3 : 1;
	unsigned char bit2 : 1;
	unsigned char bit1 : 1;
	unsigned char bit0 : 1;
}BIT;

int main(int argc, char** argv)
{
	char* cFilePath = argv[1];				// �о���� ���� (1��Ʈ)
	char* cPixel = argv[2];					// �о���� ��� ��� ���� �ȼ�
	char cFileTitle[2048] = { 0, };			// �о���� ���� ��ο� Ȯ���ڸ� �и��� ���ڿ�
	int iPixel = atoi(cPixel);				// ��� ��� ���� �ȼ� ������ȯ
	int iWidth = 0;
	int iHeight = 0;
	int iBitsPerPixel = 0;
	int iSamplesPerPixel = 0;
	float fXres = 0;
	float fYres = 0;
	int iTotal_line = 0;					// ���� �ȼ��� �������� �ִ� ���� ��ġ
	int iFilePath_Length = strlen(cFilePath);		//  �о���� ���� ����� ����
	const char* cFileExt = ".tif";


	// �޸� �Ҵ� ����
	BIT* bit_data = NULL;

	// �о���� ���ϸ� Ȯ���� �и�
	for (int i = strlen(cFilePath); i >= 0; i--)
	{
		if (cFilePath[i] == '.')
			break;
		iFilePath_Length--;

	}
	strncpy(cFileTitle, argv[1], iFilePath_Length);

	TIFF* fp_r = TIFFOpen(cFilePath, "r4");

	// ��� ���� ��´�.
	TIFFGetField(fp_r, TIFFTAG_IMAGEWIDTH, &iWidth);
	TIFFGetField(fp_r, TIFFTAG_IMAGELENGTH, &iHeight);
	TIFFGetField(fp_r, TIFFTAG_XRESOLUTION, &fXres);
	TIFFGetField(fp_r, TIFFTAG_YRESOLUTION, &fYres);

	printf("%d %d %lf %lf\n", iWidth, iHeight, fXres, fYres);

	// �޸� �Ҵ�
	int iBpl = (iWidth + 7) / 8;				// 1����Ʈ / 8 = 1��Ʈ
	bit_data = (BIT*)calloc(iBpl, sizeof(BIT));


	TIFF* fp_w = NULL;

	// ��� ���� ������ ������ ���ϸ� ����
	char cFilePath_Write[2048] = { 0, };
	int iImageNum = 0;
	char cImageNum[5] = { 0, };

	for (iTotal_line = 0; iTotal_line < iHeight; iTotal_line += iPixel)
	{
		if (iTotal_line + iPixel >= iHeight)
			break;

		fp_w = NULL;
		iImageNum = iTotal_line / iPixel;
		memset(cImageNum, 0, 5);
		sprintf(cImageNum, "_%02d", iImageNum);

		memset(cFilePath_Write, 0, 2048);
		sprintf(cFilePath_Write, "%s%s%s", cFileTitle, cImageNum, cFileExt);
		fp_w = TIFFOpen(cFilePath_Write, "w4");

		// ��� ���� ����
		TIFFSetField(fp_w, TIFFTAG_IMAGEWIDTH, iWidth);
		TIFFSetField(fp_w, TIFFTAG_IMAGELENGTH, iPixel);
		TIFFSetField(fp_w, TIFFTAG_XRESOLUTION, fXres);
		TIFFSetField(fp_w, TIFFTAG_YRESOLUTION, fYres);
		TIFFSetField(fp_w, TIFFTAG_SAMPLESPERPIXEL, 1);
		TIFFSetField(fp_w, TIFFTAG_BITSPERSAMPLE, 1);
		TIFFSetField(fp_w, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
		TIFFSetField(fp_w, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
		TIFFSetField(fp_w, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(fp_w, TIFFTAG_PHOTOMETRIC, 0);
		TIFFSetField(fp_w, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
		TIFFSetField(fp_w, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

		// �̹��� �����͸� �߶� ���� ���� ���Ϸ� �����.
		for (int h = 0; h < iPixel; h++)
		{
			memset(bit_data, 0, iBpl);	// �ݺ� �� ������ ���� �޸� ����

			TIFFReadScanline(fp_r, bit_data, h + iTotal_line, 0);

			// �����͸� ���� �� ���δ����� Write
			TIFFWriteScanline(fp_w, bit_data, h, 0);
		}

		TIFFClose(fp_w);

	}

	// ��� ���̷� �ڸ� �� ������ ������ ���� �̹��� ����
	fp_w = NULL;
	iImageNum = iTotal_line / iPixel;
	memset(cImageNum, 0, 5);
	sprintf(cImageNum, "_%02d", iImageNum);

	memset(cFilePath_Write, 0, 2048);
	sprintf(cFilePath_Write, "%s%s%s", cFileTitle, cImageNum, cFileExt);
	fp_w = TIFFOpen(cFilePath_Write, "w4");

	// ��� ���� ����
	TIFFSetField(fp_w, TIFFTAG_IMAGEWIDTH, iWidth);
	TIFFSetField(fp_w, TIFFTAG_IMAGELENGTH, iHeight - iTotal_line);
	TIFFSetField(fp_w, TIFFTAG_XRESOLUTION, fXres);
	TIFFSetField(fp_w, TIFFTAG_YRESOLUTION, fYres);
	TIFFSetField(fp_w, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(fp_w, TIFFTAG_BITSPERSAMPLE, 1);
	TIFFSetField(fp_w, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(fp_w, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(fp_w, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(fp_w, TIFFTAG_PHOTOMETRIC, 0);
	TIFFSetField(fp_w, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
	TIFFSetField(fp_w, TIFFTAG_COMPRESSION, COMPRESSION_NONE);

	for (int h = 0; h < iHeight - iTotal_line; h++)
	{
		memset(bit_data, 0, iBpl);	// �ݺ� �� ������ ���� �޸� ����

		TIFFReadScanline(fp_r, bit_data, h + iTotal_line, 0);

		// �����͸� ���� �� ���δ����� Write
		TIFFWriteScanline(fp_w, bit_data, h, 0);
	}

	TIFFClose(fp_w);


	TIFFClose(fp_r);
	free(bit_data);


	return 0;
}