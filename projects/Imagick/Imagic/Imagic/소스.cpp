#include <stdio.h>
#include <stdlib.h>
#include "Magick++.h"
#include "MagickCore/resample.h"
#include "MagickCore/resize.h"

using namespace MagickCore;

PixelInfo *pixel;

int main(void)
{
	ResampleFilter* filter;
	SetResampleFilter(filter, RobidouxFilter);

	pixel = (PixelInfo*)calloc(512 * 2, sizeof(PixelInfo));

	ResamplePixelColor(filter, 600.0, 600.0, pixel, NULL);

	free(pixel);
	return 0;
}