//#pragma comment(lib, "tiff.lib")

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

// �Է� ���� ���� ��δ� argv[1]�� �Ѿ�´�.
int main(int argc, char** argv)
{
	//printf("%s\n", argv[1]);

	// Color Mode (7�÷�)
	int colormode = 7;

	// �� �÷����� 1bit �����Ϳ� ���� �÷� �� 0~255�� �����Ѵ�. >> �������� ������ ����
	// ���ʴ�� (C, M, Y, K, LC, LM, LK)
	int colordata[7] = { 255, 255, 255, 255, 224, 192, 169 };

	// �� �÷����� cmyk ���δ��� �޸� ���� �ε��� + ��ġ
	// ���ʴ�� (C, M, Y, K, LC, LM, LK)
	int colorindex[7] = { 0, 1, 2, 3, 0, 1, 3 };

	// �Է¹��� ���� ��ο��� �÷� �±� + Ȯ���ڸ� �и���Ų ���� ��θ�
	char filepath[1024] = { '\0', };
	
	// �о���� 1bit ���ϸ�
	// ���ʴ�� (C, M, Y, K, LC, LM, LK)
	char bit_filepath[7][1024] = { {'\0', },  {'\0', }, {'\0', }, {'\0', }, {'\0', }, {'\0', }, {'\0', } };

	// ������ CMYK.tif ���ϸ�
	char cmyk_filepath[1024] = { '\0', };

	// �Է¹��� ���� ����� ���ڿ� ����
	int filepathlength = strlen(argv[1]);

	//printf("%d\n", filepathlength);

	// �÷� �±� + Ȯ����
	const char* lp_color_ext[7] = { "_C.tif", "_M.tif", "_Y.tif", "_K.tif", "_LC.tif", "_LM.tif", "_LK.tif" };

	// �о���� �����Ϳ� ���� �޸�
	BitData* bit_data = NULL;
	unsigned char* cmyk_data = NULL;

	// TIFF ���� ��� ������ ���� ����
	int width, height;
	float xres, yres;

	// �� �Ҵ��� �޸� ������
	int size_bit_data;
	int size_cmyk_data;

	// �Է� ���� ���� ��ο��� �÷� �±� + Ȯ���� �κ� �и�
	while (1)
	{
		if (argv[1][filepathlength] == '_')
			break;
		filepathlength--;
	}
	strncpy(filepath, argv[1], filepathlength);
	//printf("%d\n", filepathlength);
	//printf("%s\n", filepath);

	// �о���� 1bit ���ϸ� �۾�
	for (int i = 0; i < colormode; i++)
	{
		strcpy(bit_filepath[i], filepath);		// �и���Ų ���ϰ�θ� ����
		strcat(bit_filepath[i], lp_color_ext[i]);	// ������ �Ŀ� �÷� �±� + Ȯ���ڸ� �̾� ���δ�.
		//printf("%s\n", bit_filepath[i]);
	}

	// ������ CMYK.tif ���ϸ� �۾�
	strcpy(cmyk_filepath, filepath);		// �и���Ų ���ϰ�θ� ����
	strcat(cmyk_filepath, "_Merge.tif");	// ������ �Ŀ� �±� + Ȯ���ڸ� �̾� ���δ�.
	//printf("%s\n", cmyk_filepath);

	////////////////////// 1bit ���� �����͸� Merge ��Ų CMYK.tif ���� ���� //////////////////////
	// TIFF ���� ������
	TIFF* bit[7];
	TIFF* cmyk;

	// 1bit ���� Open�ϰ� ��� ���� ��´�.
	for (int i = 0; i < colormode; i++)
	{
		bit[i] = TIFFOpen(bit_filepath[i], "rb");
	}
	TIFFGetField(bit[0], TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(bit[0], TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(bit[0], TIFFTAG_XRESOLUTION, &xres);
	TIFFGetField(bit[0], TIFFTAG_YRESOLUTION, &yres);


	// ������ �޸� �Ҵ�
	size_bit_data = (width + 7) / 8;
	size_cmyk_data = size_bit_data * 8 * 4; // Byte * 8
	bit_data = (BitData*)calloc(size_bit_data, sizeof(BitData));
	cmyk_data = (unsigned char*)calloc(size_cmyk_data, sizeof(unsigned char));


	// ������ cmyk.tif ���� Open�ϰ� tif ���� ��� ����
	cmyk = TIFFOpen(cmyk_filepath, "wb");

	TIFFSetField(cmyk, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(cmyk, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(cmyk, TIFFTAG_XRESOLUTION, xres);
	TIFFSetField(cmyk, TIFFTAG_YRESOLUTION, yres);
	TIFFSetField(cmyk, TIFFTAG_INKSET, INKSET_CMYK);
	TIFFSetField(cmyk, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(cmyk, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(cmyk, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(cmyk, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(cmyk, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(cmyk, TIFFTAG_PHOTOMETRIC, 5);
	TIFFSetField(cmyk, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
	TIFFSetField(cmyk, TIFFTAG_COMPRESSION, COMPRESSION_NONE);


	// ������ Merge �۾�
	int cmyk_index = 0;		// cmyk ���δ��� �޸� ���� �ε���
	for (int i = 0; i < height; i++)
	{
		memset(cmyk_data, 0, size_cmyk_data);	// �ݺ� �� ������ ���� �޸� ����

		// �� �÷����� ���δ����� ������ ���� �� CMYK ���� �����Ϳ� �־��ش�.
		for (int j = 0; j < colormode; j++)
		{
			cmyk_index = 0;	// �ݺ� �� ������ cmyk ���δ��� �޸� ���� �ε��� ����

			memset(bit_data, 0, size_bit_data);	// �ݺ� �� ������ ���� �޸� ����
			TIFFReadScanline(bit[j], bit_data, i, 0);
			
			for (int k = 0; k < size_bit_data; k++)
			{
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit0;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit1;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit2;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit3;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit4;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit5;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit6;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
				if (cmyk_data[cmyk_index * 4 + colorindex[j]] == 0)
					cmyk_data[cmyk_index * 4 + colorindex[j]] = colordata[j] * bit_data[k].bit7;
				cmyk_index++;
				//if (cmyk_index >= width)
					//break;
			}
			
		}

		// �����͸� ���� �� ���δ����� Write
		TIFFWriteScanline(cmyk, cmyk_data, i, 0);
	}



	// TIFF ���� Close
	for (int i = 0; i < colormode; i++)
	{
		TIFFClose(bit[i]);
	}
	TIFFClose(cmyk);

	// �޸� ����
	free(bit_data);
	free(cmyk_data);

	return 0;
}