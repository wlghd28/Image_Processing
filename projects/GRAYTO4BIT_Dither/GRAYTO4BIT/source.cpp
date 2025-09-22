#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"

#include "mask.h"

typedef struct BIT4
{
	unsigned char bit1 : 4;
	unsigned char bit0 : 4;
}BIT4;



int main(void)
{

	int int_width = 0;
	int int_height = 0;
	float float_xres = 0;
	float float_yres = 0;
	int int_row_per_stript = 0;
	int int_bits_per_sample = 0;
	int int_samples_per_pixel = 0;

	int int_4bit_data_size = 0;

	BYTE* byte_pix_gray = NULL;
	BIT4* byte_pix_4bit = NULL;

	TIFF* fp_r = TIFFOpen("titan.tif", "r4");

	TIFFGetField(fp_r, TIFFTAG_IMAGEWIDTH, &int_width);
	TIFFGetField(fp_r, TIFFTAG_IMAGELENGTH, &int_height);
	TIFFGetField(fp_r, TIFFTAG_XRESOLUTION, &float_xres);
	TIFFGetField(fp_r, TIFFTAG_YRESOLUTION, &float_yres);
	TIFFGetField(fp_r, TIFFTAG_ROWSPERSTRIP, &int_row_per_stript);
	TIFFGetField(fp_r, TIFFTAG_BITSPERSAMPLE, &int_bits_per_sample);
	TIFFGetField(fp_r, TIFFTAG_SAMPLESPERPIXEL, &int_samples_per_pixel);

	int_4bit_data_size = (int_width + 1) / 2;

	byte_pix_gray = (BYTE*)calloc(int_4bit_data_size * 2, sizeof(BYTE));
	byte_pix_4bit = (BIT4*)calloc(int_4bit_data_size, sizeof(BIT4));

	TIFF* fp_w = TIFFOpen("titan_4bit.tif", "w4");

	TIFFSetField(fp_w, TIFFTAG_IMAGEWIDTH, int_width);
	TIFFSetField(fp_w, TIFFTAG_IMAGELENGTH, int_height);
	TIFFSetField(fp_w, TIFFTAG_XRESOLUTION, float_xres);
	TIFFSetField(fp_w, TIFFTAG_YRESOLUTION, float_yres);
	TIFFSetField(fp_w, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(fp_w, TIFFTAG_BITSPERSAMPLE, 4);
	TIFFSetField(fp_w, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	TIFFSetField(fp_w, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(fp_w, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(fp_w, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
	TIFFSetField(fp_w, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	TIFFSetField(fp_w, TIFFTAG_ROWSPERSTRIP, int_row_per_stript);
	TIFFSetField(fp_w, TIFFTAG_PHOTOMETRIC, 0);

	int int_4bit_pix = 0;

	// threshold값 조건들을 찾아가야함...
	int int_bit_threshold1_pix = 254;
	int int_bit_threshold2_pix = 127;
	int int_bit_threshold3_pix = 0;

	int int_bit_threshold1_sub = 88;
	int int_bit_threshold2_sub = 108;
	int int_bit_threshold3_sub = 128;

	int int_index = 0;
	int int_sub = 0;		// gray - mask
	for (int h = 0; h < int_height; h++)
	{
		memset(byte_pix_4bit, 0, int_4bit_data_size);

		TIFFReadScanline(fp_r, byte_pix_gray, h, 0);
		int_index = 0;

		for (int w = 0; w < int_4bit_data_size; w++)
		{
			
			///////////// bit 0 //////////////
			if ((int)byte_pix_gray[int_index] == 0)
				byte_pix_4bit[w].bit0 = 0;
			else if ((int)byte_pix_gray[int_index] == 255)
				byte_pix_4bit[w].bit0 = 15;
			else
			{
				// mask pix >> 4bit pix 변환
				if ((int)byte_mask[h % int_mask_row][int_index % int_mask_col] > int_bit_threshold1_pix)
					int_4bit_pix = 15;
				else if ((int)byte_mask[h % int_mask_row][int_index % int_mask_col] > int_bit_threshold2_pix)
					int_4bit_pix = 10;
				else if ((int)byte_mask[h % int_mask_row][int_index % int_mask_col] > int_bit_threshold3_pix)
					int_4bit_pix = 5;
				else
					int_4bit_pix = 0;

				// Gray 픽셀 값 > 마스크 값
				if ((int)byte_pix_gray[int_index] > (int)byte_mask[h % int_mask_row][int_index % int_mask_col])
				{
					int_sub = (int)byte_pix_gray[int_index] - (int)byte_mask[h % int_mask_row][int_index % int_mask_col];
					if (int_sub < int_bit_threshold1_sub)
						byte_pix_4bit[w].bit0 = int_4bit_pix;
					else if (int_sub < int_bit_threshold2_sub)
						byte_pix_4bit[w].bit0 = int_4bit_pix + 5 > 0 ? 15 : int_4bit_pix + 5;
					else if (int_sub < int_bit_threshold3_sub)
						byte_pix_4bit[w].bit0 = int_4bit_pix + 10 > 0 ? 10 : int_4bit_pix + 10;
					else
						byte_pix_4bit[w].bit0 = 15;
				}
				// Gray 픽셀 값 <= 마스크 값
				else
				{

					int_sub = (int)byte_mask[h % int_mask_row][int_index % int_mask_col] - (int)byte_pix_gray[int_index];
					if (int_sub < int_bit_threshold1_sub)
						byte_pix_4bit[w].bit0 = int_4bit_pix;
					else if (int_sub < int_bit_threshold2_sub)
						byte_pix_4bit[w].bit0 = int_4bit_pix - 5 < 0 ? 0 : int_4bit_pix - 5;
					else if (int_sub < int_bit_threshold3_sub)
						byte_pix_4bit[w].bit0 = int_4bit_pix - 10 < 0 ? 0 : int_4bit_pix - 10;
					else
						byte_pix_4bit[w].bit0 = 0;
				}
			}
			int_index++;

			///////////// bit 1 //////////////
			if ((int)byte_pix_gray[int_index] == 0)
				byte_pix_4bit[w].bit1 = 0;
			else if ((int)byte_pix_gray[int_index] == 255)
				byte_pix_4bit[w].bit1 = 15;
			else
			{
				// mask pix >> 4bit pix 변환
				if ((int)byte_mask[h % int_mask_row][int_index % int_mask_col] > int_bit_threshold1_pix)
					int_4bit_pix = 15;
				else if ((int)byte_mask[h % int_mask_row][int_index % int_mask_col] > int_bit_threshold2_pix)
					int_4bit_pix = 10;
				else if ((int)byte_mask[h % int_mask_row][int_index % int_mask_col] > int_bit_threshold3_pix)
					int_4bit_pix = 5;
				else
					int_4bit_pix = 0;


				// Gray 픽셀 값 > 마스크 값
				if ((int)byte_pix_gray[int_index] > (int)byte_mask[h % int_mask_row][int_index % int_mask_col])
				{
					int_sub = (int)byte_pix_gray[int_index] - (int)byte_mask[h % int_mask_row][int_index % int_mask_col];
					if (int_sub < int_bit_threshold1_sub)
						byte_pix_4bit[w].bit1 = int_4bit_pix;
					else if (int_sub < int_bit_threshold2_sub)
						byte_pix_4bit[w].bit1 = int_4bit_pix + 5 > 0 ? 15 : int_4bit_pix + 5;
					else if (int_sub < int_bit_threshold3_sub)
						byte_pix_4bit[w].bit1 = int_4bit_pix + 10 > 0 ? 10 : int_4bit_pix + 10;
					else
						byte_pix_4bit[w].bit1 = 15;
				}
				// Gray 픽셀 값 <= 마스크 값.
				else
				{
					int_sub = (int)byte_mask[h % int_mask_row][int_index % int_mask_col] - (int)byte_pix_gray[int_index];
					if (int_sub < int_bit_threshold1_sub)
						byte_pix_4bit[w].bit1 = int_4bit_pix;
					else if (int_sub < int_bit_threshold2_sub)
						byte_pix_4bit[w].bit1 = int_4bit_pix - 5 < 0 ? 0 : int_4bit_pix - 5;
					else if (int_sub < int_bit_threshold3_sub)
						byte_pix_4bit[w].bit1 = int_4bit_pix - 10 < 0 ? 0 : int_4bit_pix - 10;
					else
						byte_pix_4bit[w].bit1 = 0;
				}
			}
			int_index++;
		}

		TIFFWriteScanline(fp_w, byte_pix_4bit, h, 0);

	}






	TIFFClose(fp_r);
	TIFFClose(fp_w);

	free(byte_pix_gray);
	free(byte_pix_4bit);

	return 0;
}