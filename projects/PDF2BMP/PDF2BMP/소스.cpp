//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/PDFDraw.h>
#include <Filters/MappedFile.h>
#include <Filters/FilterWriter.h>
#include <cmath>
//#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <SDF/ObjSet.h>
#include <Windows.h>

using namespace std;
using namespace pdftron;
using namespace PDF;
using namespace Filters;

// 시간을 계산하기 위한 변수
double total_Time_GPU, total_Time_CPU, tmp_time;
LARGE_INTEGER beginClock, endClock, clockFreq;
LARGE_INTEGER tot_beginClock, tot_endClock, tot_clockFreq;

//---------------------------------------------------------------------------------------
// The following sample illustrates how to convert PDF documents to various raster image 
// formats (such as PNG, JPEG, BMP, TIFF, etc), as well as how to convert a PDF page to 
// GDI+ Bitmap for further manipulation and/or display in WinForms applications.
//---------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	try
	{
		// The first step in every application using PDFNet is to initialize the 
		// library and set the path to common PDF resources. The library is usually 
		// initialized only once, but calling Initialize() multiple times is also fine.
		PDFNet::Initialize();

		// Optional: Set ICC color profiles to fine tune color conversion 
		// for PDF 'device' color spaces...

		// PDFNet::SetResourcesPath("../../../resources");
		// PDFNet::SetColorManagement(PDFNet::e_lcms);
		// PDFNet::SetDefaultDeviceCMYKProfile("PDFNetC64/Resources/USWebCoatedSWOP.icc");
		// PDFNet::SetDefaultDeviceRGBProfile("PDFNetC64/Resources/AdobeRGB1998.icc"); // will search in PDFNet resource folder.

		// ----------------------------------------------------
		// Optional: Set predefined font mappings to override default font 
		// substitution for documents with missing fonts...

		// PDFNet::AddFontSubst("StoneSans-Semibold", "C:/WINDOWS/Fonts/comic.ttf");
		// PDFNet::AddFontSubst("StoneSans", "comic.ttf");  // search for 'comic.ttf' in PDFNet resource folder.
		// PDFNet::AddFontSubst(PDFNet::e_Identity, "C:/WINDOWS/Fonts/arialuni.ttf");
		// PDFNet::AddFontSubst(PDFNet::e_Japan1, "C:/Program Files/Adobe/Acrobat 7.0/Resource/CIDFont/KozMinProVI-Regular.otf");
		// PDFNet::AddFontSubst(PDFNet::e_Japan2, "c:/myfonts/KozMinProVI-Regular.otf");
		// PDFNet::AddFontSubst(PDFNet::e_Korea1, "AdobeMyungjoStd-Medium.otf");
		// PDFNet::AddFontSubst(PDFNet::e_CNS1, "AdobeSongStd-Light.otf");
		// PDFNet::AddFontSubst(PDFNet::e_GB1, "AdobeMingStd-Light.otf");

		// Relative path to the folder containing test files.
		//string input_path = "../TestFiles/";
		//string output_path = "../TestFiles/Output/";
		char input_path[100] = "../TestFiles//exam01.pdf";
		char output_path[100];

		PDFDraw draw;  // PDFDraw class is used to rasterize PDF pages.
		Page page;
		int index_page1;
		int index_page2;
		int dpi;

		QueryPerformanceFrequency(&tot_clockFreq);	// 시간을 측정하기위한 준비

		//--------------------------------------------------------------------------------
		// Example 1) Convert the first page to PNG and TIFF at 92 DPI. 
		// A three step tutorial to convert PDF page to an image.
		try
		{
			// A) Open the PDF document.
			PDFDoc doc(input_path);
			printf("PDF파일 페이지 수 : %d\n", doc.GetPageCount());
			//printf("%s\n", doc.GetDocInfo());
			printf("변환시킬 PDF파일 페이지 (A ~ B): ");
			scanf("%d %d", &index_page1, &index_page2);
			printf("해상도 : ");
			scanf("%d", &dpi);
			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();

			// B) The output resolution is set to 92 DPI.
			draw.SetDPI(dpi);
			total_Time_CPU = 0;
			QueryPerformanceCounter(&tot_beginClock); // 시간측정 시작

			for (int i = index_page1; i <= index_page2; i++)
			{

				// C) Rasterize the first page in the document and save the result as PNG.
				//draw.Export(doc.GetPageIterator().Current(), (output_path + "example.png").c_str());
				//cout << "Example 1: example.png" << endl;

				//draw.Export(doc.GetPage(i), (output_path + "example"+ pagenum + ".png").c_str());
				//cout << "Example 1: example" << i << ".png" << endl;


				// Export the same page as TIFF
				//draw.Export(doc.GetPageIterator().Current(), (output_path + "example.tif").c_str(), "TIFF");
				//cout << "Example 1: example.tif" << endl;

				sprintf(output_path, "../TestFiles/Output/exam%03d_max.tif", i);
				draw.Export(doc.GetPage(i), output_path, "TIFF");
				printf("Example 1: example%d.tif\n", i);

			}

			QueryPerformanceCounter(&tot_endClock);		// 시간측정 종료

			total_Time_CPU = (double)(tot_endClock.QuadPart - tot_beginClock.QuadPart) / tot_clockFreq.QuadPart;
			printf("Total processing Time : %f ms\n", total_Time_CPU * 1000);
		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}

		/*
		//--------------------------------------------------------------------------------
		// Example 2) Convert the all pages in a given document to JPEG at 72 DPI.
		cout << "Example 2:" << endl;
		SDF::ObjSet hint_set; //  A collection of rendering 'hits'.
		try
		{
			PDFDoc doc((input_path + "example.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();

			draw.SetDPI(72); // Set the output resolution is to 72 DPI.

			// Use optional encoder parameter to specify JPEG quality.
			SDF::Obj encoder_param = hint_set.CreateDict();
			encoder_param.PutNumber("Quality", 80);

			// Traverse all pages in the document.
			for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next()) {
				ostringstream sstm;
				sstm << output_path << "example" << itr.Current().GetIndex() << ".jpg";
				string path = sstm.str();
				cout << "example" << itr.Current().GetIndex() << ".jpg" << endl;

				draw.Export(itr.Current(), path.c_str(), "JPEG", encoder_param);
			}

			cout << "Done." << endl;
		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}
		*/

		/*
		// Examples 3-6
		try
		{
			// Common code for remaining samples.
			PDFDoc tiger_doc((input_path + "example.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			tiger_doc.InitSecurityHandler();
			Page page = tiger_doc.GetPage(1);

			//--------------------------------------------------------------------------------
			// Example 3) Convert the first page to raw bitmap. Also, rotate the
			// page 90 degrees and save the result as RAW.
			draw.SetDPI(100); // Set the output resolution is to 100 DPI.
			draw.SetRotate(Page::e_90);  // Rotate all pages 90 degrees clockwise.

			int width = 0, height = 0, stride = 0;
			double dpi = 0.0;
			const UChar* buf = draw.GetBitmap(page, width, height, stride, dpi, PDFDraw::e_rgb);


			// Save the raw RGB data to disk.
			ofstream outfile((output_path + "example.raw").c_str(), ofstream::binary);
			outfile.write((char*)buf, height * stride);
			outfile.close();


			cout << "Example 3: example.raw" << endl;
			draw.SetRotate(Page::e_0);  // Disable image rotation for remaining samples.

			//--------------------------------------------------------------------------------
			// Example 4) Convert PDF page to a fixed image size. Also illustrates some
			// other features in PDFDraw class such as rotation, image stretching, exporting
			// to grayscale, or monochrome.

			// Initialize render 'gray_hint' parameter, that is used to control the
			// rendering process. In this case we tell the rasterizer to export the image as
			// 1 Bit Per Component (BPC) image.
			SDF::Obj mono_hint = hint_set.CreateDict();
			mono_hint.PutNumber("BPC", 1);

			// SetImageSize can be used instead of SetDPI() to adjust page  scaling
			// dynamically so that given image fits into a buffer of given dimensions.
			draw.SetImageSize(1000, 1000);		// Set the output image to be 1000 wide and 1000 pixels tall
			draw.Export(page, (output_path + "example.png").c_str(), "PNG", mono_hint);
			cout << "Example 4: example.png" << endl;

			draw.SetImageSize(200, 400);	    // Set the output image to be 200 wide and 300 pixels tall
			draw.SetRotate(Page::e_180);  // Rotate all pages 90 degrees clockwise.

			// 'gray_hint' tells the rasterizer to export the image as grayscale.
			SDF::Obj gray_hint = hint_set.CreateDict();
			gray_hint.PutName("ColorSpace", "Gray");

			draw.Export(page, (output_path + "example.png").c_str(), "PNG", gray_hint);
			cout << "Example 4: example.png" << endl;

			draw.SetImageSize(400, 200, false);  // The third parameter sets 'preserve-aspect-ratio' to false.
			draw.SetRotate(Page::e_0);    // Disable image rotation.
			draw.Export(page, (output_path + "example.jpg").c_str(), "JPEG");
			cout << "Example 4: example.jpg" << endl;

			//--------------------------------------------------------------------------------
			// Example 5) Zoom into a specific region of the page and rasterize the
			// area at 200 DPI and as a thumbnail (i.e. a 50x50 pixel image).
			Rect zoom_rect(216, 522, 330, 600);
			page.SetCropBox(zoom_rect);	// Set the page crop box.

			// Select the crop region to be used for drawing.
			draw.SetPageBox(Page::e_crop);
			draw.SetDPI(900);  // Set the output image resolution to 900 DPI.
			draw.Export(page, (output_path + "example.png").c_str(), "PNG");
			cout << "Example 5: example.png" << endl;

			// -------------------------------------------------------------------------------
			// Example 6)
			draw.SetImageSize(50, 50);	   // Set the thumbnail to be 50x50 pixel image.
			draw.Export(page, (output_path + "example.png").c_str(), "PNG");
			cout << "Example 6: example.png" << endl;

		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}
		*/

		/*
		//--------------------------------------------------------------------------------
		// Example 7) Convert the first PDF page to CMYK TIFF at 92 DPI.
		// A three step tutorial to convert PDF page to an image
		try
		{
			pdftron::SDF::Obj cmyk_hint = hint_set.CreateDict();
			cmyk_hint.PutName("ColorSpace", "CMYK");
			// A) Open the PDF document.
			PDFDoc doc((input_path + "example.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();

			// B) The output resolution is set to 92 DPI.
			draw.SetDPI(92);

			// C) Rasterize the first page in the document and save the result as TIFF.
			Page pg = doc.GetPage(1);
			draw.Export(pg, output_path + "example.tif", "TIFF", cmyk_hint);
			cout << "Example 7: example.tif" << endl;
		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...)
		{
			cout << "Unknown Exception" << endl;
		}
		*/

		/*
		//--------------------------------------------------------------------------------
		// Example 8) PDFRasterizer can be used for more complex rendering tasks, such as
		// strip by strip or tiled document rendering. In particular, it is useful for
		// cases where you cannot simply modify the page crop box (interactive viewing,
		// parallel rendering).  This example shows how you can rasterize the south-west
		// quadrant of a page.
		try
		{
			// A) Open the PDF document.
			PDFDoc doc((input_path + "example.pdf").c_str());
			// Initialize the security handler, in case the PDF is encrypted.
			doc.InitSecurityHandler();

			// B) Get the page matrix
			Page pg = doc.GetPage(1);
			Page::Box box = Page::e_crop;
			Common::Matrix2D mtx = pg.GetDefaultMatrix(true, box);
			// We want to render a quadrant, so use half of width and height
			const double pg_w = pg.GetPageWidth(box) / 2;
			const double pg_h = pg.GetPageHeight(box) / 2;

			// C) Scale matrix from PDF space to buffer space
			const double dpi = 96.0;
			const double scale = dpi / 72.0; // PDF space is 72 dpi
			const int buf_w = static_cast<int>(floor(scale * pg_w));
			const int buf_h = static_cast<int>(floor(scale * pg_h));
			const int bytes_per_pixel = 4; // BGRA buffer
			const int buf_size = buf_w * buf_h * bytes_per_pixel;
			mtx.Translate(0, -pg_h); // translate by '-pg_h' since we want south-west quadrant
			mtx = Common::Matrix2D(scale, 0, 0, scale, 0, 0) * mtx;

			// D) Rasterize page into memory buffer, according to our parameters
			std::vector<unsigned char> buf;
			PDFRasterizer rast;
			buf = rast.Rasterize(pg, buf_w, buf_h, buf_w * bytes_per_pixel, bytes_per_pixel, true, mtx);

			// buf now contains raw BGRA bitmap.
			cout << "Example 8: Successfully rasterized into memory buffer." << endl;
		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...) {
			cout << "Unknown Exception" << endl;
		}
		*/

		/*
		//--------------------------------------------------------------------------------
		// Example 9) Export raster content to PNG using different image smoothing settings.
		try
		{
			PDFDoc text_doc((input_path + "example.pdf").c_str());
			text_doc.InitSecurityHandler();

			draw.SetImageSmoothing(false, false);
			string filename = "example.png";
			draw.Export(text_doc.GetPageIterator().Current(), (output_path + filename).c_str());
			cout << "Example 9 a): " << filename << ". Done." << endl;

			filename = "example.png";
			draw.SetImageSmoothing(true, false //default quality bilinear resampling//);
			draw.Export(text_doc.GetPageIterator().Current(), (output_path + filename).c_str());
			cout << "Example 9 b): " << filename << ". Done." << endl;

			filename = "example.png";
			draw.SetImageSmoothing(true, true //high quality area resampling//);
			draw.Export(text_doc.GetPageIterator().Current(), (output_path + filename).c_str());
			cout << "Example 9 c): " << filename << ". Done." << endl;
		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...) {
			cout << "Unknown Exception" << endl;
		}
		*/
		/*
		//--------------------------------------------------------------------------------
		// Example 10) Export separations directly, without conversion to an output colorspace
		try
		{
			PDFDoc separation_doc((input_path + "example.pdf").c_str());
			separation_doc.InitSecurityHandler();
			pdftron::SDF::Obj separation_hint = hint_set.CreateDict();
			separation_hint.PutName("ColorSpace", "Separation");
			draw.SetDPI(96);
			draw.SetImageSmoothing(true, true);
			draw.SetOverprint(PDFRasterizer::e_op_on);

			string filename = "example.png";
			draw.Export(separation_doc.GetPageIterator().Current(), (output_path + filename).c_str(), "PNG");
			cout << "Example 10 a): " << filename << ". Done." << endl;

			filename = "example";
			draw.Export(separation_doc.GetPageIterator().Current(), (output_path + filename).c_str(), "PNG", separation_hint);
			cout << "Example 10 b): " << filename << "_[ink].png. Done." << endl;

			filename = "example.tif";
			draw.Export(separation_doc.GetPageIterator().Current(), (output_path + filename).c_str(), "TIFF", separation_hint);
			cout << "Example 10 c): " << filename << ". Done." << endl;
		}
		catch (Common::Exception& e)
		{
			cout << e << endl;
		}
		catch (...) {
			cout << "Unknown Exception" << endl;
		}
		PDFNet::Terminate();
		*/
	}
	catch (Common::Exception& e)
	{
		cout << e << endl;
	}
	catch (...) {
		cout << "Unknown Exception" << endl;
	}
	printf("Finish!!\n");
	return 0;
}
