#include <stdio.h>
#include "..\libtiff\tiff.h"
#include "..\libtiff\tiffio.h"
//#include "..\imageSrc\MyImage.h"
//#include "..\imageSrc\ImageFrameWndManager.h"


int main(void)
{
	TIFF *tif;

	tif = TIFFOpen("../TestFiles/Output/exam2001.tif", "rb");

	if (tif == NULL)
	{
		printf("File Not Found!!\n");
		return 0;
	}

	int width, height;
	uint32 count = 0;
	void *icc_profile = NULL;

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	TIFFGetField(tif, TIFFTAG_ICCPROFILE, &count, &icc_profile);
	
	printf("width : %d\n", width);
	printf("height : %d\n", height);
	printf("count : %d\n", count);
	if(icc_profile == NULL)
		printf("icc_profile : NULL\n");
	else
		printf("icc_profile : NOTNULL\n");

	TIFFClose(tif);

	return 0;
}
