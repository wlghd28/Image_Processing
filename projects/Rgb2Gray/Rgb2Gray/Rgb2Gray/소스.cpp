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

FILE *fp_rgb;
FILE *fp_c;
FILE *fp_m;
FILE *fp_y;
FILE *fp_k;

RGBTRIPLE *pix_rgb;
BYTE *pix_c;
BYTE *pix_m;
BYTE *pix_y;
BYTE *pix_k;
BITMAPFILEHEADER bfh_rgb;
BITMAPINFOHEADER bih_rgb;
BITMAPFILEHEADER bfh_gray;
BITMAPINFOHEADER bih_gray;
RGBQUAD rgb[256] = {0};

long pix_size;
long width;
long height;
long bpl_gray;
long bpl_rgb;
BYTE trash[3];	// ������ ��
BYTE pad_gray;	// gray ���� �е� ��
BYTE pad_rgb;	// rgb ���� �е� ��

void GrayHeader();		// bmp ���� ��� ����
void Fread();
void Fwrite(char *str);
int main(void)
{
	QueryPerformanceFrequency(&tot_clockFreq);	// �ð��� �����ϱ����� �غ�
	QueryPerformanceCounter(&tot_beginClock); // GPU �ð����� ����

	double c, m, y, k;
	double max;

	Fread();

	pix_rgb = (RGBTRIPLE *)calloc(width, sizeof(RGBTRIPLE));
	pix_c = (BYTE *)calloc(width, sizeof(BYTE));
	pix_m = (BYTE *)calloc(width, sizeof(BYTE));
	pix_y = (BYTE *)calloc(width, sizeof(BYTE));
	pix_k = (BYTE *)calloc(width, sizeof(BYTE));

	GrayHeader();

	FILE *fp_c = fopen("images\\grayout_c.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_c);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_c);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_c);

	FILE *fp_m = fopen("images\\grayout_m.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_m);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_m);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_m);

	FILE *fp_y = fopen("images\\grayout_y.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_y);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_y);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_y);

	FILE *fp_k = fopen("images\\grayout_k.bmp", "wb");
	fwrite(&bfh_gray, sizeof(bfh_gray), 1, fp_k);
	fwrite(&bih_gray, sizeof(bih_gray), 1, fp_k);
	fwrite(rgb, sizeof(RGBQUAD), 256, fp_k);

	for (int i = 0; i < height; i++)
	{
		fread(pix_rgb, sizeof(RGBTRIPLE), width, fp_rgb);
		fread(&trash, sizeof(BYTE), pad_rgb, fp_rgb);
		for (int j = 0; j < width; j++)
		{
			max = pix_rgb[j].rgbtRed;
			if (max < pix_rgb[j].rgbtGreen)
				max = pix_rgb[j].rgbtGreen;
			if (max < pix_rgb[j].rgbtBlue)
				max = pix_rgb[j].rgbtBlue;
			//printf("%lf %lf %lf\n", r, g, b);
			//printf("%lf\n", max);
			c = (double)(max - pix_rgb[j].rgbtRed) / max;
			m = (double)(max - pix_rgb[j].rgbtGreen) / max;
			y = (double)(max - pix_rgb[j].rgbtBlue) / max;
			k = 255 - max;

			pix_c[j] = 255 - (BYTE)(c * 255);
			pix_m[j] = 255 -(BYTE)(m * 255);
			pix_y[j] = 255 -(BYTE)(y * 255);
			pix_k[j] = 255 -(BYTE)k;
			//printf("%d %d %d %d\n", pix_c[j], pix_m[j], pix_y[j], pix_k[j]);
		}
		fwrite(pix_c, sizeof(BYTE), width, fp_c);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_c);
		fwrite(pix_m, sizeof(BYTE), width, fp_m);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_m);
		fwrite(pix_y, sizeof(BYTE), width, fp_y);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_y);
		fwrite(pix_k, sizeof(BYTE), width, fp_k);
		fwrite(&trash, sizeof(BYTE), pad_gray, fp_k);
	}
	//Fwrite(str);

	fclose(fp_c);
	fclose(fp_m);
	fclose(fp_y);
	fclose(fp_k);

	free(pix_rgb);
	free(pix_c);
	free(pix_m);
	free(pix_y);
	free(pix_k);

	QueryPerformanceCounter(&tot_endClock); // GPU �ð����� ����
	total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
	printf("����ð� : %.1lf(Sec)\n", total_Time_CPU);
	return 0;
}
void Fread()
{
	fp_rgb = fopen("images\\input.bmp", "rb");
	if (fp_rgb == NULL)
	{
		printf("File not found!\n");
		exit(0);
	}
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
void Fwrite(char *str)
{

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
		rgb[i].rgbBlue = i;
		rgb[i].rgbGreen = i;
		rgb[i].rgbRed = i;
		rgb[i].rgbReserved = 0;
	}
}