#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <process.h>
#include "mask_bnm.h"

double total_Time_CPU = 0;
double total_Time_GPU = 0;
LARGE_INTEGER beginClock, endClock, clockFreq;
LARGE_INTEGER tot_beginClock, tot_endClock, tot_clockFreq;

//#define xx(index) unsigned char bit##index : 1;

BITMAPFILEHEADER bfh_rgb;
BITMAPINFOHEADER bih_rgb;
BITMAPFILEHEADER bfh_bit;
BITMAPINFOHEADER bih_bit;
RGBQUAD rgb[2] = { 0 };
//RGBQUAD rgb[256] = { 0 };

long pix_size;
long width;
long height;
long bpl_bit;
long bpl_rgb;
BYTE trash[3];	// 쓰레기 값
BYTE pad_bit;	// bit 파일 패딩 값
BYTE pad_rgb;	// rgb 파일 패딩 값
unsigned int bit8_array[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

void BitHeader();		// bmp 파일 헤더 세팅
void Fread(FILE* fp_rgb);

unsigned int WINAPI ThreadFunction_C(void *arg);
unsigned int WINAPI ThreadFunction_M(void *arg);
unsigned int WINAPI ThreadFunction_Y(void *arg);
unsigned int WINAPI ThreadFunction_K(void *arg);

int main(void)
{
	QueryPerformanceFrequency(&tot_clockFreq);	// 시간을 측정하기위한 준비
	QueryPerformanceCounter(&tot_beginClock); // GPU 시간측정 시작

	HANDLE hThread_c = NULL;
	DWORD dwThreadID_c = NULL;
	HANDLE hThread_m = NULL;
	DWORD dwThreadID_m = NULL;
	HANDLE hThread_y = NULL;
	DWORD dwThreadID_y = NULL;
	HANDLE hThread_k = NULL;
	DWORD dwThreadID_k = NULL;

	hThread_c = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_C, NULL, 0, (unsigned*)&dwThreadID_c);
	if (hThread_c == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("생성된 쓰레드 C의 핸들 : %d\n", (int)hThread_c);
	printf("생성된 쓰레드 C의 ID : %d\n", dwThreadID_c);
	hThread_m = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_M, NULL, 0, (unsigned*)&dwThreadID_m);
	if (hThread_m == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("생성된 쓰레드 M의 핸들 : %d\n", (int)hThread_m);
	printf("생성된 쓰레드 M의 ID : %d\n", dwThreadID_m);
	hThread_y = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_Y, NULL, 0, (unsigned*)&dwThreadID_y);
	if (hThread_y == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("생성된 쓰레드 Y의 핸들 : %d\n", (int)hThread_y);
	printf("생성된 쓰레드 Y의 ID : %d\n", dwThreadID_y);
	hThread_k = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_K, NULL, 0, (unsigned*)&dwThreadID_k);
	if (hThread_k == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("생성된 쓰레드 K의 핸들 : %d\n", (int)hThread_k);
	printf("생성된 쓰레드 K의 ID : %d\n", dwThreadID_k);

	// main 쓰레드가 종료하면 워커쓰레드도 종료하므로 무한 대기하게 한다
	WaitForSingleObject(hThread_c, INFINITE);
	WaitForSingleObject(hThread_m, INFINITE);
	WaitForSingleObject(hThread_y, INFINITE);
	WaitForSingleObject(hThread_k, INFINITE);

	QueryPerformanceCounter(&tot_endClock); // GPU 시간측정 종료
	total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
	printf("실행시간 : %.1lf(Sec)\n", total_Time_CPU);

	return 0;
}
unsigned int WINAPI ThreadFunction_C(void *arg)
{
	FILE * fp_rgbc = fopen("images\\input.bmp", "rb");
	if (fp_rgbc == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread(fp_rgbc);

	double c;
	double max;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_c = (BYTE *)calloc(width, sizeof(BYTE));
	BYTE *pix_bit = (BYTE *)calloc((width + 7) / 8, sizeof(BYTE));

	BitHeader();

	FILE *fp_c = fopen("images\\ripout_c.bmp", "wb");
	fwrite(&bfh_bit, sizeof(bfh_bit), 1, fp_c);
	fwrite(&bih_bit, sizeof(bih_bit), 1, fp_c);
	fwrite(&rgb, sizeof(RGBQUAD), 2, fp_c);
	//fwrite(rgb, sizeof(RGBQUAD), 256, fp_c);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgbc);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgbc);
		memset(pix_bit, 0, (width + 7) / 8);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			c = (double)(max - pix_rgb[j].rgbtRed) / max;
			pix_c[j] = 255 - (BYTE)(c * 255);

			if (pix_c[j] != 0 && pix_c[j] >= mask_bnm[i % ms][j % ms])
			{
				pix_bit[j / 8] += bit8_array[j % 8];
			}
		}
		fwrite(pix_bit, sizeof(BYTE), (width + 7) / 8, fp_c);
		fwrite(&trash, sizeof(BYTE), pad_bit, fp_c);
	}
	fclose(fp_rgbc);
	fclose(fp_c);
	free(pix_c);
	free(pix_rgb);
	free(pix_bit);
	printf("ripout_C finish!! \n");

	return 0;
}
unsigned int WINAPI ThreadFunction_M(void *arg)
{
	FILE * fp_rgbm = fopen("images\\input.bmp", "rb");
	if (fp_rgbm == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread(fp_rgbm);

	double m;
	double max;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_m = (BYTE *)calloc(width, sizeof(BYTE));
	BYTE *pix_bit = (BYTE *)calloc((width + 7) / 8, sizeof(BYTE));

	BitHeader();

	FILE *fp_m = fopen("images\\ripout_m.bmp", "wb");
	fwrite(&bfh_bit, sizeof(bfh_bit), 1, fp_m);
	fwrite(&bih_bit, sizeof(bih_bit), 1, fp_m);
	fwrite(&rgb, sizeof(RGBQUAD), 2, fp_m);
	//fwrite(rgb, sizeof(RGBQUAD), 256, fp_m);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgbm);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgbm);
		memset(pix_bit, 0, (width + 7) / 8);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			m = (double)(max - pix_rgb[j].rgbtGreen) / max;
			pix_m[j] = 255 - (BYTE)(m * 255);

			if (pix_m[j] != 0 && pix_m[j] >= mask_bnm[i % ms][j % ms])
			{
				pix_bit[j / 8] += bit8_array[j % 8];
			}
		}

		fwrite(pix_bit, sizeof(BYTE), (width + 7) / 8, fp_m);
		fwrite(&trash, sizeof(BYTE), pad_bit, fp_m);
	}

	fclose(fp_rgbm);
	fclose(fp_m);
	free(pix_m);
	free(pix_rgb);
	free(pix_bit);
	printf("ripout_M finish!! \n");

	return 0;
}
unsigned int WINAPI ThreadFunction_Y(void *arg)
{
	FILE *fp_rgby = fopen("images\\input.bmp", "rb");
	if (fp_rgby == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread(fp_rgby);
	double y;
	double max;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_y = (BYTE *)calloc(width, sizeof(BYTE));
	BYTE *pix_bit = (BYTE *)calloc((width + 7) / 8, sizeof(BYTE));

	BitHeader();

	FILE *fp_y = fopen("images\\ripout_y.bmp", "wb");
	fwrite(&bfh_bit, sizeof(bfh_bit), 1, fp_y);
	fwrite(&bih_bit, sizeof(bih_bit), 1, fp_y);
	fwrite(&rgb, sizeof(RGBQUAD), 2, fp_y);
	//fwrite(rgb, sizeof(RGBQUAD), 256, fp_y);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgby);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgby);
		memset(pix_bit, 0, (width + 7) / 8);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			y = (double)(max - pix_rgb[j].rgbtBlue) / max;
			pix_y[j] = 255 - (BYTE)(y * 255);

			if (pix_y[j] != 0 && pix_y[j] >= mask_bnm[i % ms][j % ms])
			{
				pix_bit[j / 8] += bit8_array[j % 8];
			}
		}

		fwrite(pix_bit, sizeof(BYTE), (width + 7) / 8, fp_y);
		fwrite(&trash, sizeof(BYTE), pad_bit, fp_y);
	}

	fclose(fp_rgby);
	fclose(fp_y);
	free(pix_y);
	free(pix_rgb);
	free(pix_bit);
	printf("ripout_Y finish!! \n");

	return 0;
}
unsigned int WINAPI ThreadFunction_K(void *arg)
{
	FILE *fp_rgbk = fopen("images\\input.bmp", "rb");
	if (fp_rgbk == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread(fp_rgbk);

	double k;
	double max;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_k = (BYTE *)calloc(width, sizeof(BYTE));
	BYTE *pix_bit = (BYTE *)calloc(((width + 7) / 8), sizeof(BYTE));

	BitHeader();

	FILE *fp_k = fopen("images\\ripout_k.bmp", "wb");
	fwrite(&bfh_bit, sizeof(bfh_bit), 1, fp_k);
	fwrite(&bih_bit, sizeof(bih_bit), 1, fp_k);
	fwrite(&rgb, sizeof(RGBQUAD), 2, fp_k);
	//fwrite(rgb, sizeof(RGBQUAD), 256, fp_k);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgbk);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgbk);
		memset(pix_bit, 0, ((width + 7) / 8));

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			k = 255 - max;
			pix_k[j] = 255 - (BYTE)k;

			if (pix_k[j] != 0 && pix_k[j] >= mask_bnm[i % ms] [j % ms])
			{
				pix_bit[j / 8] += bit8_array[j % 8];
			}
		}
		fwrite(pix_bit, sizeof(BYTE), ((width + 7) / 8), fp_k);
		fwrite(&trash, sizeof(BYTE), pad_bit, fp_k);
	}

	fclose(fp_rgbk);
	fclose(fp_k);
	free(pix_k);
	free(pix_rgb);
	free(pix_bit);
	printf("ripout_K finish!! \n");

	return 0;
}

void Fread(FILE* fp_rgb)
{
	// 파일헤더, 정보헤더 읽어들인다
	fread(&bfh_rgb, sizeof(bfh_rgb), 1, fp_rgb);
	fread(&bih_rgb, sizeof(bih_rgb), 1, fp_rgb);

	width = bih_rgb.biWidth;
	height = bih_rgb.biHeight;
	pix_size = width * height;
	printf("Image size : %d X %d\n", width, height);

	// BPL을 맞춰주기 위해서 픽셀데이터의 메모리를 4의 배수로 조정
	bpl_bit = (((width + 7) / 8) + 3) / 4 * 4;
	bpl_rgb = (width * 3 + 3) / 4 * 4;

	// 패딩 값 계산
	pad_rgb = (BYTE)(bpl_rgb - width * 3);
	pad_bit = (BYTE)(bpl_bit - ((width + 7) / 8));

}
void BitHeader()
{
	// bit 파일 비트맵 파일헤더
	bfh_bit.bfType = 0x4D42;
	bfh_bit.bfSize = bpl_bit * height + sizeof(bfh_bit) + sizeof(bih_bit) + sizeof(rgb);
	bfh_bit.bfReserved1 = 0;
	bfh_bit.bfReserved2 = 0;
	bfh_bit.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + sizeof(RGBQUAD) * 2; // 40 + 16 + 8

	// bit 파일 비트맵 정보헤더
	bih_bit.biSize = 0x28; // 40 byte
	bih_bit.biWidth = width;
	bih_bit.biHeight = height;
	bih_bit.biPlanes = 1;
	bih_bit.biBitCount = 1;	// 8
	bih_bit.biSizeImage = bpl_bit * height;
	bih_bit.biCompression = 0;
	bih_bit.biClrImportant = 0;
	bih_bit.biClrUsed = 2;	// 256
	//bih_bit.biXPelsPerMeter = 0;
	//bih_bit.biYPelsPerMeter = 0;
	bih_bit.biXPelsPerMeter = bih_rgb.biXPelsPerMeter; // 1440 DPI
	bih_bit.biYPelsPerMeter = bih_rgb.biYPelsPerMeter; // 1440 DPI


	// 팔레트작성
	for (int i = 0; i < 2; i++)
	{
		rgb[i].rgbBlue = i * 255;
		rgb[i].rgbGreen = i * 255;
		rgb[i].rgbRed = i * 255;
		rgb[i].rgbReserved = 0;
	}

}