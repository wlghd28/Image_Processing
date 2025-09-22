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

//FILE *fp_rgb;
//RGBTRIPLE *pix_rgb;
BITMAPFILEHEADER bfh_rgb;
BITMAPINFOHEADER bih_rgb;
BITMAPFILEHEADER bfh_gray;
BITMAPINFOHEADER bih_gray;
RGBQUAD rgb[256] = { 0 };

long pix_size;
long width;
long height;
long bpl_gray;
long bpl_rgb;
BYTE trash[3];	// ������ ��
BYTE pad_gray;	// gray ���� �е� ��
BYTE pad_rgb;	// rgb ���� �е� ��

BYTE boundary = 127;	// light deep ��� ��
BYTE color_max = 255;	// ligth deep �ִ� ��

void GrayHeader();		// bmp ���� ��� ����
void Fread(FILE* fp_rgb);

// 4�� ����
unsigned int WINAPI ThreadFunction_C(void *arg);
unsigned int WINAPI ThreadFunction_M(void *arg);
unsigned int WINAPI ThreadFunction_Y(void *arg);
unsigned int WINAPI ThreadFunction_K(void *arg);
// 7�� ���� �� �߰�
unsigned int WINAPI ThreadFunction_LC(void *arg);
unsigned int WINAPI ThreadFunction_LM(void *arg);
unsigned int WINAPI ThreadFunction_LK(void *arg);

int main(void)
{
	QueryPerformanceFrequency(&tot_clockFreq);	// �ð��� �����ϱ����� �غ�
	QueryPerformanceCounter(&tot_beginClock); // GPU �ð����� ����

	/*
	fp_rgb = fopen("images\\input.bmp", "rb");

	if (fp_rgb == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread();
	*/

	HANDLE hThread_c = NULL;
	DWORD dwThreadID_c = NULL;
	HANDLE hThread_m = NULL;
	DWORD dwThreadID_m = NULL;
	HANDLE hThread_y = NULL;
	DWORD dwThreadID_y = NULL;
	HANDLE hThread_k = NULL;
	DWORD dwThreadID_k = NULL;

	// 7�����ǽ� �߰�
	HANDLE hThread_lc = NULL;
	DWORD dwThreadID_lc = NULL;
	HANDLE hThread_lm = NULL;
	DWORD dwThreadID_lm = NULL;
	HANDLE hThread_lk = NULL;
	DWORD dwThreadID_lk = NULL;

	hThread_c = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_C, NULL, 0, (unsigned*)&dwThreadID_c);
	if (hThread_c == 0) 
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("������ ������ C�� �ڵ� : %d\n", (int)hThread_c);
	printf("������ ������ C�� ID : %d\n", dwThreadID_c);
	hThread_m = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_M, NULL, 0, (unsigned*)&dwThreadID_m);
	if (hThread_m == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("������ ������ M�� �ڵ� : %d\n", (int)hThread_m);
	printf("������ ������ M�� ID : %d\n", dwThreadID_m);
	hThread_y = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_Y, NULL, 0, (unsigned*)&dwThreadID_y);
	if (hThread_y == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("������ ������ Y�� �ڵ� : %d\n", (int)hThread_y);
	printf("������ ������ Y�� ID : %d\n", dwThreadID_y);
	hThread_k = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_K, NULL, 0, (unsigned*)&dwThreadID_k);
	if (hThread_k == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("������ ������ K�� �ڵ� : %d\n", (int)hThread_k);
	printf("������ ������ K�� ID : %d\n", dwThreadID_k);

	// 7�� ���� �� �߰�
	hThread_lc = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_LC, NULL, 0, (unsigned*)&dwThreadID_lc);
	if (hThread_lc == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("������ ������ LC�� �ڵ� : %d\n", (int)hThread_lc);
	printf("������ ������ LC�� ID : %d\n", dwThreadID_lc);

	hThread_lm = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_LM, NULL, 0, (unsigned*)&dwThreadID_lm);
	if (hThread_lm == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("������ ������ LM�� �ڵ� : %d\n", (int)hThread_lm);
	printf("������ ������ LM�� ID : %d\n", dwThreadID_lm);

	hThread_lk = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction_LK, NULL, 0, (unsigned*)&dwThreadID_lk);
	if (hThread_lk == 0)
	{
		printf("_beginthreadex Error\n");
		exit(0);
	}
	printf("������ ������ LK�� �ڵ� : %d\n", (int)hThread_lk);
	printf("������ ������ LK�� ID : %d\n", dwThreadID_lk);
	
	// main �����尡 �����ϸ� ��Ŀ�����嵵 �����ϹǷ� ���� ����ϰ� �Ѵ�
	WaitForSingleObject(hThread_c, INFINITE);
	WaitForSingleObject(hThread_m, INFINITE);
	WaitForSingleObject(hThread_y, INFINITE);
	WaitForSingleObject(hThread_k, INFINITE);

	// 7�� ���ǽ� �߰�

	WaitForSingleObject(hThread_lc, INFINITE);
	WaitForSingleObject(hThread_lm, INFINITE);
	WaitForSingleObject(hThread_lk, INFINITE);
	

	//fclose(fp_rgb);
	//free(pix_rgb);

	QueryPerformanceCounter(&tot_endClock); // GPU �ð����� ����
	total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
	printf("����ð� : %.1lf(Sec)\n", total_Time_CPU);
	
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
	double pix_tmp;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_c = (BYTE *)calloc(width, sizeof(BYTE));
	GrayHeader();

	FILE *fp_c = fopen("images\\grayout_c.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_c);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_c);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_c);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgbc);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgbc);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			c = (double)(max - pix_rgb[j].rgbtRed) / max;
			//pix_c[j] = 255 - (c * 255);
			pix_c[j] = c * 255;
			if (pix_c[j] > boundary)
			{
				pix_tmp = (double)((color_max * pix_c[j] - color_max * boundary) / (color_max - boundary));
				pix_c[j] = pix_tmp;
			}
			else 
			{
				pix_c[j] = 0;
			}
		}
		fwrite(pix_c, sizeof(BYTE), width, fp_c);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_c);
	}
	fclose(fp_rgbc);
	fclose(fp_c);
	free(pix_c);
	free(pix_rgb);
	printf("grayout_C finish!! \n");

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
	double pix_tmp;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_m = (BYTE *)calloc(width, sizeof(BYTE));
	GrayHeader();

	FILE *fp_m = fopen("images\\grayout_m.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_m);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_m);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_m);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgbm);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgbm);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			m = (double)(max - pix_rgb[j].rgbtGreen) / max;
			//pix_m[j] = 255 - (m * 255);
			pix_m[j] = m * 255;
			if (pix_m[j] > boundary)
			{
				pix_tmp = (double)((color_max * pix_m[j] - color_max * boundary) / (color_max - boundary));
				pix_m[j] = pix_tmp;
			}
			else
			{
				pix_m[j] = 0;
			}
		}
		fwrite(pix_m, sizeof(BYTE), width, fp_m);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_m);
	}

	fclose(fp_rgbm);
	fclose(fp_m);
	free(pix_m);
	free(pix_rgb);
	printf("grayout_M finish!! \n");

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

	GrayHeader();

	FILE *fp_y = fopen("images\\grayout_y.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_y);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_y);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_y);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgby);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgby);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			y = (double)(max - pix_rgb[j].rgbtBlue) / max;
			//pix_y[j] = 255 - (y * 255);
			pix_y[j] = y * 255;
		}
		fwrite(pix_y, sizeof(BYTE), width, fp_y);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_y);
	}

	fclose(fp_rgby);
	fclose(fp_y);
	free(pix_y);
	free(pix_rgb);
	printf("grayout_Y finish!! \n");

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
	double pix_tmp;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_k = (BYTE *)calloc(width, sizeof(BYTE));
	GrayHeader();

	FILE *fp_k = fopen("images\\grayout_k.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_k);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_k);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_k);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgbk);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgbk);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			k = 255 - max;
			//pix_k[j] = 255 - k;
			pix_k[j] = k;
			if (pix_k[j] > boundary)
			{
				pix_tmp = (double)((color_max * pix_k[j] - color_max * boundary) / (color_max - boundary));;
				pix_k[j] = pix_tmp;
			}
			else
			{
				pix_k[j] = 0;
			}
		}
		fwrite(pix_k, sizeof(BYTE), width, fp_k);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_k);
	}

	fclose(fp_rgbk);
	fclose(fp_k);
	free(pix_k);
	free(pix_rgb);
	printf("grayout_K finish!! \n");

	return 0;
}
// 7�� ���� �� �߰�

unsigned int WINAPI ThreadFunction_LC(void *arg)
{
	FILE *fp_rgblc = fopen("images\\input.bmp", "rb");
	if (fp_rgblc == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread(fp_rgblc);

	double lc;
	double max;
	double pix_tmp;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_lc = (BYTE *)calloc(width, sizeof(BYTE));
	GrayHeader();

	FILE *fp_lc = fopen("images\\grayout_lc.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_lc);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_lc);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_lc);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgblc);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgblc);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			lc = (double)(max - pix_rgb[j].rgbtRed) / max;
			//pix_lc[j] = 255 - (lc * 255);
			pix_lc[j] = lc * 255;
			if (pix_lc[j] > boundary)
			{
				pix_tmp = (double)(color_max * (pix_lc[j] - color_max) / (boundary - color_max));
				pix_lc[j] = pix_tmp;
			}
			else
			{
				pix_tmp = (double)pix_lc[j] * (color_max / boundary);
				pix_lc[j] = pix_tmp;
			}
		}
		fwrite(pix_lc, sizeof(BYTE), width, fp_lc);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_lc);
	}

	fclose(fp_rgblc);
	fclose(fp_lc);
	free(pix_lc);
	free(pix_rgb);
	printf("grayout_LC finish!! \n");

	return 0;
}

unsigned int WINAPI ThreadFunction_LM(void *arg)
{
	FILE *fp_rgblm = fopen("images\\input.bmp", "rb");
	if (fp_rgblm == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread(fp_rgblm);

	double lm;
	double max;
	double pix_tmp;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_lm = (BYTE *)calloc(width, sizeof(BYTE));
	GrayHeader();

	FILE *fp_lm = fopen("images\\grayout_lm.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_lm);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_lm);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_lm);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgblm);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgblm);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			lm = (double)(max - pix_rgb[j].rgbtGreen) / max;
			//pix_lm[j] = 255 - (lm * 255);
			pix_lm[j] = lm * 255;
			if (pix_lm[j] > boundary)
			{
				pix_tmp = (double)(color_max * (pix_lm[j] - color_max) / (boundary - color_max));
				pix_lm[j] = pix_tmp;
			}
			else
			{
				pix_tmp = (double)pix_lm[j] * (color_max / boundary);
				pix_lm[j] = pix_tmp;
			}

		}
		fwrite(pix_lm, sizeof(BYTE), width, fp_lm);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_lm);
	}

	fclose(fp_rgblm);
	fclose(fp_lm);
	free(pix_lm);
	free(pix_rgb);
	printf("grayout_LM finish!! \n");

	return 0;
}

unsigned int WINAPI ThreadFunction_LK(void *arg)
{
	FILE *fp_rgblk = fopen("images\\input.bmp", "rb");
	if (fp_rgblk == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
	Fread(fp_rgblk);

	double lk;
	double max;
	double pix_tmp;
	RGBTRIPLE *pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	BYTE *pix_lk = (BYTE *)calloc(width, sizeof(BYTE));
	GrayHeader();

	FILE *fp_lk = fopen("images\\grayout_lk.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_lk);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_lk);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_lk);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgblk);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgblk);

		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;

			lk = 255 - max;
			//pix_lk[j] = 255 - lk;
			pix_lk[j] = lk;
			if (pix_lk[j] > boundary)
			{
				pix_tmp = (double)(color_max * (pix_lk[j] - color_max) / (boundary - color_max));
				pix_lk[j] = pix_tmp;
			}
			else
			{
				pix_tmp = (double)pix_lk[j] * (color_max / boundary);
				pix_lk[j] = pix_tmp;
			}
		}
		fwrite(pix_lk, sizeof(BYTE), width, fp_lk);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_lk);
	}

	fclose(fp_rgblk);
	fclose(fp_lk);
	free(pix_lk);
	free(pix_rgb);
	printf("grayout_LK finish!! \n");

	return 0;
}

void Fread(FILE* fp_rgb)
{
	// �������, ������� �о���δ�
	fread(&bfh_rgb, sizeof(bfh_rgb), 1, fp_rgb);
	fread(&bih_rgb, sizeof(bih_rgb), 1, fp_rgb);

	width = bih_rgb.biWidth;
	height = bih_rgb.biHeight;
	pix_size = width * height;
	printf("Image size : %d X %d\n", width, height);

	// BPL�� �����ֱ� ���ؼ� �ȼ��������� �޸𸮸� 4�� ����� ����
	bpl_gray = (width + 3) / 4 * 4;
	bpl_rgb = (width * 3 + 3) / 4 * 4;

	// �е� �� ���
	pad_rgb = (BYTE)(bpl_rgb - width * 3);
	pad_gray = (BYTE)(bpl_gray - width);

}
void GrayHeader()
{
	// gray ���� ��Ʈ�� �������
	bfh_gray.bfType = 0x4D42;
	bfh_gray.bfSize = bpl_gray * height + sizeof(bfh_gray) + sizeof(bih_gray) + sizeof(rgb);
	bfh_gray.bfReserved1 = 0;
	bfh_gray.bfReserved2 = 0;
	bfh_gray.bfOffBits = sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + sizeof(RGBQUAD) * 256; // 56 byte

	// gray ���� ��Ʈ�� �������
	bih_gray.biSize = 0x28; // 40 byte
	bih_gray.biWidth = width;
	bih_gray.biHeight = height;
	bih_gray.biPlanes = 1;
	bih_gray.biBitCount = 8;
	bih_gray.biSizeImage = bpl_gray * height;
	bih_gray.biCompression = BI_RGB;
	bih_gray.biClrImportant = 0;
	bih_gray.biClrUsed = 256;
	bih_gray.biXPelsPerMeter = bih_rgb.biXPelsPerMeter;
	bih_gray.biYPelsPerMeter = bih_rgb.biYPelsPerMeter;

	// �ȷ�Ʈ�ۼ�
	for (int i = 0; i < 256; i++)
	{
		rgb[i].rgbBlue = 255 - i;
		rgb[i].rgbGreen = 255 - i;
		rgb[i].rgbRed = 255 - i;
		rgb[i].rgbReserved = 0;
	}
}