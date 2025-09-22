//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/Print.h> // new Print API
#include <iostream>

using namespace std;
using namespace pdftron;
using namespace PDF;


/** 
 * The following sample is a simplest C/C++ program used to illustrate how to print 
 * PDF document using currently selected default printer. In this sample, PDF::Print class 
 * is used to send data to the printer. 
 * 
 * Following this function is the more complex way of using PDFDraw directly.
 *
 * The first example uses the new PDF::Print::StartPrintJob function to send a rasterization 
 * of the document with optimal compression to the printer.  If the OS is Windows 7, then the
 * XPS print path will be used to preserve vector quality.
 *  
 * The second example uses PDFDraw send unoptimized rasterized data via the print path. 
 *  
 * If you would like to rasterize page at high resolutions (e.g. more than 600 DPI), you 
 * should use PDFRasterizer or PDFNet vector output instead of PDFDraw. 
 */
int main()
{
	PDFNet::Initialize();
	try
	{
		// Relative path to the folder containing test files.
		string input_path =  "../../TestFiles/";
		PDFDoc doc((input_path +  "tiger.pdf").c_str());
		doc.InitSecurityHandler();

		// Set our PrinterMode options
		PrinterMode printerMode;
		printerMode.SetCollation(true);
		printerMode.SetCopyCount(1);
		printerMode.SetDPI(600); // regardless of ordering, an explicit DPI setting overrides the OutputQuality setting
		printerMode.SetDuplexing(PrinterMode::e_Duplex_Auto);
		
		// If the XPS print path is being used, then the printer spooler file will
		// ignore the grayscale option and be in full color
		printerMode.SetOutputColor(PrinterMode::e_OutputColor_Grayscale);
		printerMode.SetOutputQuality(PrinterMode::e_OutputQuality_Medium);
		// printerMode.SetNUp(2,1);
		// printerMode.SetScaleType(PrinterMode::e_ScaleType_FitToOutputPage);

		// Print the PDF document to the default printer, using "tiger.pdf" as the document
		// name, send the file to the printer not to an output file, print all pages, set the printerMode
		// and don't provide a cancel flag.
		Print::StartPrintJob(doc, UString(""), doc.GetFileName(), UString(""), NULL, &printerMode, NULL );
	}
	catch(Common::Exception& e)
	{
		std::cout << e << std::endl;
	}
	catch(...)
	{
		std::cout << "Unknown Exception" << std::endl;
	}

	PDFNet::Terminate (); // Done with PDFNet related stuff --------------------

	return 0;
}




//////////////////////////////////////////////////////////////////////////
// The second example uses PDFDraw send unoptimized rasterized data via the print path. 
//////////////////////////////////////////////////////////////////////////
#include <PDF/PDFDraw.h> // only needed for more complex example
#if defined(_WIN32) && !defined(__WINRT__)
#include <windows.h>
#include <windef.h>
#include <wingdi.h>

// Return HDC for the default printer
HDC GetDefaultPrinterDC(void);

int PrintUsingPDFDraw ()
{	
	PRINTDLG pd;
	
	pd.lStructSize = sizeof(pd);
	pd.hDevMode = NULL;
	pd.hDevNames = NULL;
	pd.Flags = PD_USEDEVMODECOPIESANDCOLLATE | PD_RETURNDC | PD_HIDEPRINTTOFILE | PD_NOSELECTION | PD_NOPAGENUMS;
	pd.nCopies = 1;
	pd.nFromPage = 0xFFFF;
	pd.nToPage = 0xFFFF;
	pd.nMinPage = 1;
	pd.nMaxPage = 0xFFFF;
	pd.hDC = GetDefaultPrinterDC();  

	DOCINFOA docinfo;
	memset(&docinfo, 0, sizeof(DOCINFO));
	docinfo.cbSize = sizeof(docinfo);
	docinfo.lpszDocName = "My Test";
	docinfo.fwType = 0;
	docinfo.lpszDatatype = (LPSTR) 0;
	docinfo.lpszOutput = (LPSTR)0;
	
	int nError = StartDocA(pd.hDC, &docinfo);
	if(nError == SP_ERROR) {
		MessageBoxA(NULL, "Error", "Error, Printing", MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	// Start with PDFNew related stuff -----------------------
	PDFNet::Initialize();
	try
	{
		// Relative path to the folder containing test files.
		string input_path =  "../../TestFiles/";
		PDFDoc doc((input_path +  "tiger.pdf").c_str());
		doc.InitSecurityHandler();

		PDFDraw pdfdraw;	

		pdfdraw.SetRasterizerType(PDFRasterizer::e_BuiltIn);

		//  Note: If you would like to rasterize page at high resolutions (e.g. more 
		// than 600 DPI), you should use PDFRasterizer. 
		pdfdraw.SetDPI(200); // Set DPI (Dots Per Inch).
		pdfdraw.SetPrintMode(true);

		for (PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next()) 
		{
			nError = StartPage(pd.hDC);
			if(nError == SP_ERROR) {
				MessageBoxA(NULL, "Error", "Error, Printing", MB_OK | MB_ICONEXCLAMATION);
				AbortDoc(pd.hDC);
				return 1;
			} 

			// Obtain the size of printer page (in pixels).
			PDF::Rect r;
			bool use_physical_page = false;
			if (use_physical_page){
				// Use the physical page for printing. Note: the physical page is almost always 
				// greater than the printable area of the page, and never smaller. 
				r.x1 = -GetDeviceCaps(pd.hDC, PHYSICALOFFSETX);
				r.y1 = -GetDeviceCaps(pd.hDC, PHYSICALOFFSETY);
				r.x2 = GetDeviceCaps(pd.hDC, PHYSICALWIDTH) - GetDeviceCaps(pd.hDC, PHYSICALOFFSETX);
				r.y2 = GetDeviceCaps(pd.hDC, PHYSICALHEIGHT) - GetDeviceCaps(pd.hDC, PHYSICALOFFSETY);
			}
			else { // use the printable area of the page for printing.
				r.x1 = r.y1 = 0;
				r.x2 = GetDeviceCaps(pd.hDC, PHYSICALWIDTH) - GetDeviceCaps(pd.hDC, PHYSICALOFFSETX) * 2;
				r.y2 = GetDeviceCaps(pd.hDC, PHYSICALHEIGHT) - GetDeviceCaps(pd.hDC, PHYSICALOFFSETY) * 2;
			}

			// Convert page rectangle dimensions to points. One PDF point is 1/72 of an inch.
			// LOGPIXELSX/Y returns number of pixels per logical inch along the screen width/height.
			double conv_x2pts = 72.0/GetDeviceCaps(pd.hDC, LOGPIXELSX);
			double conv_y2pts = 72.0/GetDeviceCaps(pd.hDC, LOGPIXELSY);
			r.x1 *= conv_x2pts; r.y1 *= conv_y2pts;
			r.x2 *= conv_x2pts; r.y2 *= conv_y2pts;

			pdfdraw.DrawInRect(itr.Current(), pd.hDC, r);  // Print the page
			EndPage(pd.hDC);
		}
	}
	catch(Common::Exception& e)
	{
		std::cout << e << std::endl;
	}
	catch(...)
	{
		std::cout << "Unknown Exception" << std::endl;
	}

	PDFNet::Terminate (); // Done with PDFNet related stuff --------------------
	
	EndDoc(pd.hDC);
	DeleteDC(pd.hDC);
	
	return 0;
}

HDC GetDefaultPrinterDC(void) 
{ 
   char szPrinter[80]; 
   char *szDevice, *szDriver, *szOutput; 

   GetProfileStringA("WINDOWS", "DEVICE", ",,,", szPrinter, 80); 

   szDevice = strtok(szPrinter, ","); 
   szDriver = strtok(NULL, ","); 
   szOutput = strtok(NULL, ","); 

   if ( !szDevice || !szDriver || !szOutput ) 
      return 0; 
   else 
      return CreateDCA( szDriver, szDevice, szOutput, NULL ); 

	return 0;
}
#endif //_WIN32

