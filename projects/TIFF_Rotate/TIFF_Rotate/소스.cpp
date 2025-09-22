#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>

#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"

#define COLORNUM 7

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

char* file_name[COLORNUM];
unsigned int WINAPI ThreadFunction(LPVOID color_num);

int main(int argc, char** argv)
{
	int iArgc = argc - 1;
	for (int i = 0; i < iArgc; i++)
	{
		file_name[i] = argv[i + 1];
		//printf("%s\n", file_name[i]);
		//printf("%s\n", argv[i + 1]);
	}
	//system("pause");

	// 스레드 핸들
	HANDLE hThread[COLORNUM] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	DWORD dwThreadID[COLORNUM] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

	// 스레드 함수 포인터 배열
	unsigned int(_stdcall * ThreadFunction_ptr[COLORNUM])(LPVOID);

	for (int i = 0; i < iArgc; i++)
	{
		ThreadFunction_ptr[i] = ThreadFunction;
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, (_beginthreadex_proc_type)ThreadFunction_ptr[i], (LPVOID)i, 0, (unsigned*)&dwThreadID[i]);
		if (hThread[i] == 0)
		{
			printf("_beginthreadex Error %d\n", i);
			system("pause");
			exit(0);
		}
	}

	for (int i = 0; i < iArgc; i++)
	{
		// main 쓰레드가 종료하면 워커쓰레드도 종료하므로 무한 대기하게 한다
		WaitForSingleObject(hThread[i], INFINITE);
	}
	return 0;
}


unsigned int WINAPI ThreadFunction(LPVOID color_num)
{
	// 파일 명 작업 
	// 왼쪽으로 90도 회전 시킨 이미지 파일명은 원본 파일명_R.tif로 한다.
	char* cRotate_ext = ".tif";
	char* cRotate_initial = "_R";

	// 확장자를 제외한 파일명의 길이 구한다.
	int iLength_original_fn = strlen(file_name[(int)color_num]);

	// 회전시킨 이미지의 파일명
	char* cRotate_fn = (char*)calloc(iLength_original_fn + 5, sizeof(char));

	// .tif 를 뺀 파일 명의 길이를 구해준다.
	while (file_name[(int)color_num][iLength_original_fn] != '.')
	{
		iLength_original_fn--;
	}

	// .tif 를 뺀 파일 명의 길이 만큼 회전시킨 이미지 파일명에 복사해준다.
	strncpy(cRotate_fn, file_name[(int)color_num], iLength_original_fn);

	// 회전 이니셜을 붙여준다.
	strcat(cRotate_fn, cRotate_initial);

	// 확장자를 붙여준다.
	strcat(cRotate_fn, cRotate_ext);

	// 파일명 Test
	//printf("%s\n", cRotate_fn);

	// Read TIFF 파일 Open
	TIFF* fp_r = TIFFOpen(file_name[(int)color_num], "r4");
	if (fp_r == NULL)
	{
		printf("Reading File failed!!\n");
		return 0;
	}

	// 헤더 정보 변수 선언
	int iWidth = 0;
	int iHeight = 0;
	int iBitsperSample = 0;
	int iSamplesperPixel = 0;
	float fXres = 0;
	float fYres = 0;
	int iMin_Color = 0;
	int iRow_Per_Stript = 0;

	// 읽어들일 데이터에 관한 메모리
	BIT* bit_data_origin_line = NULL;			
	BIT* bit_data_rotate_tile = NULL;
	//BIT* bit_data_rotate_line = NULL;

	// 헤더 정보 GET
	TIFFGetField(fp_r, TIFFTAG_IMAGEWIDTH, &iWidth);
	TIFFGetField(fp_r, TIFFTAG_IMAGELENGTH, &iHeight);
	TIFFGetField(fp_r, TIFFTAG_XRESOLUTION, &fXres);
	TIFFGetField(fp_r, TIFFTAG_YRESOLUTION, &fYres);
	TIFFGetField(fp_r, TIFFTAG_SAMPLESPERPIXEL, &iSamplesperPixel);
	TIFFGetField(fp_r, TIFFTAG_PHOTOMETRIC, &iMin_Color);
	TIFFGetField(fp_r, TIFFTAG_BITSPERSAMPLE, &iBitsperSample);
	TIFFGetField(fp_r, TIFFTAG_ROWSPERSTRIP, &iRow_Per_Stript);


	// Bit Per Line 계산
	int iSize_bit_data_origin = (iWidth + 7) / 8;
	int iSize_bit_data_rotate = (iHeight + 7) / 8;

	// 메모리 할당
	bit_data_origin_line = (BIT*)calloc(iSize_bit_data_origin, sizeof(BIT));
	//bit_data_rotate_line = (BIT*)calloc(iSize_bit_data_rotate, sizeof(BIT));
	bit_data_rotate_tile = (BIT*)calloc(iSize_bit_data_rotate * iSize_bit_data_origin * 8, sizeof(BIT));

	if (bit_data_rotate_tile == NULL)
	{
		printf("Mem Alloc Failed!!\n");
		exit(0);
	}

	// Rotate Write TIFF 파일 Open
	TIFF* fp_w = TIFFOpen(cRotate_fn, "w4");
	if (fp_w == NULL)
	{
		printf("Creating File failed!!\n");
		return 0;
	}

	// 헤더 정보 SET
	TIFFSetField(fp_w, TIFFTAG_ROWSPERSTRIP, iWidth);
	TIFFSetField(fp_w, TIFFTAG_IMAGEWIDTH, iHeight);
	TIFFSetField(fp_w, TIFFTAG_IMAGELENGTH, iWidth);
	TIFFSetField(fp_w, TIFFTAG_XRESOLUTION, fXres);
	TIFFSetField(fp_w, TIFFTAG_YRESOLUTION, fYres);
	TIFFSetField(fp_w, TIFFTAG_SAMPLESPERPIXEL, iSamplesperPixel);
	TIFFSetField(fp_w, TIFFTAG_BITSPERSAMPLE, iBitsperSample);
	TIFFSetField(fp_w, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(fp_w, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(fp_w, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(fp_w, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
	TIFFSetField(fp_w, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(fp_w, TIFFTAG_PHOTOMETRIC, iMin_Color);

	//printf("%d %d %d %d\n", iWidth, iHeight, iSamplesperPixel, iBitsperSample);

	// 이미지 회전 시작
	// 원본 이미지를 회전시켜서 데이터에 담는다.
	int iIndex_origin_height = 0;
	int iIndex_rotate_width = 0;

	for (int i = 0; i < iSize_bit_data_rotate; i++)
	{
		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			//printf("%d\n", j);
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit0; iIndex_rotate_width--; 	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit1; iIndex_rotate_width--; 	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit2; iIndex_rotate_width--; 	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit3; iIndex_rotate_width--; 	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit0 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}

		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;

		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			//printf("%d\n", j);
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit0; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit1; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit2; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit3; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit1 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}
		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;


		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			//printf("%d\n", j);
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit0; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit1; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit2; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit3; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit2 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}
		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;


		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit0; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit1; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit2; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit3; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit3 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}
		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;


		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit0; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit1; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit2; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit3; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit4 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}
		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;



		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit0; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit1; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit2; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit3; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit5 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}
		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;


		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit0; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit1; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit2; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit3; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit6 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}
		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;


		memset(bit_data_origin_line, 0, iSize_bit_data_origin);
		iIndex_rotate_width = iSize_bit_data_origin * 8 - 1;
		TIFFReadScanline(fp_r, bit_data_origin_line, iIndex_origin_height, 0);

		for (int j = 0; j < iSize_bit_data_origin; j++)
		{
			//printf("%d\n", j);
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit0; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit1; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit2; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit3; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit4; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit5; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit6; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;
			bit_data_rotate_tile[iIndex_rotate_width * iSize_bit_data_rotate + i].bit7 = bit_data_origin_line[j].bit7; iIndex_rotate_width--;	//if (iIndex_rotate_width < 0) break;

		}
		iIndex_origin_height++; if (iIndex_origin_height >= iHeight) break;
		//printf("%d\n", i);
	}

	// TIFF File Write	
	/*
	for (int i = 0; i < iWidth; i++)
	{
		memset(bit_data_rotate_line, 0, iSize_bit_data_rotate);
		for (int j = 0; j < iSize_bit_data_rotate; j++)
		{
			bit_data_rotate_line[j] = bit_data_rotate_tile[i * iSize_bit_data_rotate + j];
			//printf("%d\n", bit_data_rotate_line[j]);
		}
		TIFFWriteScanline(fp_w, bit_data_rotate_line, i, 0);
	}
	*/
	TIFFWriteRawStrip(fp_w, (tstrip_t)0, (tdata_t)bit_data_rotate_tile, (tsize_t)(iSize_bit_data_rotate* iSize_bit_data_origin * 8));

	// 파일 Close
	TIFFClose(fp_r);
	TIFFClose(fp_w);


	// 메모리 해제
	free(cRotate_fn);
	free(bit_data_origin_line);
	free(bit_data_rotate_tile);
	//free(bit_data_rotate_line);

	return 0;
}