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

// 입력 받은 파일 경로는 argv[1]로 넘어온다.
int main(int argc, char** argv)
{
	//printf("%s\n", argv[1]);

	// Color Mode (7컬러)
	int colormode = 7;

	// 각 컬러마다 1bit 데이터에 대한 컬러 값 0~255를 설정한다. >> 가변성을 가지고 있음
	// 차례대로 (C, M, Y, K, LC, LM, LK)
	int colordata[7] = { 255, 255, 255, 255, 224, 192, 169 };

	// 각 컬러마다 cmyk 라인단위 메모리 참조 인덱스 + 수치
	// 차례대로 (C, M, Y, K, LC, LM, LK)
	int colorindex[7] = { 0, 1, 2, 3, 0, 1, 3 };

	// 입력받은 파일 경로에서 컬러 태그 + 확장자를 분리시킨 파일 경로명
	char filepath[1024] = { '\0', };
	
	// 읽어들일 1bit 파일명
	// 차례대로 (C, M, Y, K, LC, LM, LK)
	char bit_filepath[7][1024] = { {'\0', },  {'\0', }, {'\0', }, {'\0', }, {'\0', }, {'\0', }, {'\0', } };

	// 생성할 CMYK.tif 파일명
	char cmyk_filepath[1024] = { '\0', };

	// 입력받은 파일 경로의 문자열 길이
	int filepathlength = strlen(argv[1]);

	//printf("%d\n", filepathlength);

	// 컬러 태그 + 확장자
	const char* lp_color_ext[7] = { "_C.tif", "_M.tif", "_Y.tif", "_K.tif", "_LC.tif", "_LM.tif", "_LK.tif" };

	// 읽어들일 데이터에 관한 메모리
	BitData* bit_data = NULL;
	unsigned char* cmyk_data = NULL;

	// TIFF 파일 헤더 정보에 관한 변수
	int width, height;
	float xres, yres;

	// 각 할당할 메모리 사이즈
	int size_bit_data;
	int size_cmyk_data;

	// 입력 받은 파일 경로에서 컬러 태그 + 확장자 부분 분리
	while (1)
	{
		if (argv[1][filepathlength] == '_')
			break;
		filepathlength--;
	}
	strncpy(filepath, argv[1], filepathlength);
	//printf("%d\n", filepathlength);
	//printf("%s\n", filepath);

	// 읽어들일 1bit 파일명 작업
	for (int i = 0; i < colormode; i++)
	{
		strcpy(bit_filepath[i], filepath);		// 분리시킨 파일경로를 복사
		strcat(bit_filepath[i], lp_color_ext[i]);	// 복사한 후에 컬러 태그 + 확장자를 이어 붙인다.
		//printf("%s\n", bit_filepath[i]);
	}

	// 생성할 CMYK.tif 파일명 작업
	strcpy(cmyk_filepath, filepath);		// 분리시킨 파일경로를 복사
	strcat(cmyk_filepath, "_Merge.tif");	// 복사한 후에 태그 + 확장자를 이어 붙인다.
	//printf("%s\n", cmyk_filepath);

	////////////////////// 1bit 파일 데이터를 Merge 시킨 CMYK.tif 파일 생성 //////////////////////
	// TIFF 파일 포인터
	TIFF* bit[7];
	TIFF* cmyk;

	// 1bit 파일 Open하고 헤더 정보 얻는다.
	for (int i = 0; i < colormode; i++)
	{
		bit[i] = TIFFOpen(bit_filepath[i], "rb");
	}
	TIFFGetField(bit[0], TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(bit[0], TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(bit[0], TIFFTAG_XRESOLUTION, &xres);
	TIFFGetField(bit[0], TIFFTAG_YRESOLUTION, &yres);


	// 데이터 메모리 할당
	size_bit_data = (width + 7) / 8;
	size_cmyk_data = size_bit_data * 8 * 4; // Byte * 8
	bit_data = (BitData*)calloc(size_bit_data, sizeof(BitData));
	cmyk_data = (unsigned char*)calloc(size_cmyk_data, sizeof(unsigned char));


	// 생성할 cmyk.tif 파일 Open하고 tif 파일 헤더 세팅
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


	// 데이터 Merge 작업
	int cmyk_index = 0;		// cmyk 라인단위 메모리 참조 인덱스
	for (int i = 0; i < height; i++)
	{
		memset(cmyk_data, 0, size_cmyk_data);	// 반복 할 때마다 라인 메모리 리셋

		// 각 컬러별로 라인단위로 데이터 읽은 후 CMYK 라인 데이터에 넣어준다.
		for (int j = 0; j < colormode; j++)
		{
			cmyk_index = 0;	// 반복 할 때마다 cmyk 라인단위 메모리 참조 인덱스 리셋

			memset(bit_data, 0, size_bit_data);	// 반복 할 때마다 라인 메모리 리셋
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

		// 데이터를 넣은 후 라인단위로 Write
		TIFFWriteScanline(cmyk, cmyk_data, i, 0);
	}



	// TIFF 파일 Close
	for (int i = 0; i < colormode; i++)
	{
		TIFFClose(bit[i]);
	}
	TIFFClose(cmyk);

	// 메모리 해제
	free(bit_data);
	free(cmyk_data);

	return 0;
}