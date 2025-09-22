//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <PDF/PDFNet.h>
#include <PDF/PDFDC.h>
#include <PDF/PDFDCEX.h>

//-----------------------------------------------------------------------------------
// This sample shows how to create and use the PDFDCEX which allows 
// printer-like multipage translations with GDI resources used across pages.
// PDFDCEX allows you to create page after page of GDI to PDF conversion using existing 
// printing functions.
//-----------------------------------------------------------------------------------

using namespace pdftron;
using namespace PDF;

// Relative path to the folder containing test files.
std::string inputPath =  "../../TestFiles/";
std::string outputPath = "../../TestFiles/Output/";

// Define a font
static const LOGFONTW ArialFont =
{ 
	12,							// height (points)
	0,							// width (use default)
	0,							// rotation of baseline for text strings
	0,							// rotation of each character
	FW_NORMAL,					// weight
	FALSE,						// italic
	FALSE,						// underline
	FALSE,						// strike out
	ANSI_CHARSET,				// character set
	OUT_DEFAULT_PRECIS,			// output precision
	CLIP_DEFAULT_PRECIS,		// clip precision
	DEFAULT_QUALITY,			// quality
	VARIABLE_PITCH | FF_SWISS,	// pitch and family
	L"Arial"					// font name
};


int main()
{
	int ret = 0;
#if defined(_WIN32) && !defined(WINCE)

	PDFNet::Initialize();

	try  
	{	
		////////////////////////////////////////////////////////////////////////////////
		// Add two pages to a new PDFDoc.
		// Fonts and resources are shared across pages.  No need to work with each PDFPage.
		// We work in page coordinates which are page dimensions (inches) times
		// DPI. Origin is at upper left corner of the page, positive down and to right.

		// Start with a PDFDoc to put the translation into, and a PDFDCEX to translate GDI to PDF
		PDFDoc pdfdoc2;
		PDFDCEX pdfDcEx;
		HDC hDC2;
		HFONT hArialFont2;
		HGDIOBJ hOldFont2;
		HGDIOBJ hPen2, hOldPen2;

		// Begin the translation from GDI to PDF -- provide the PDFDoc to append to.
		// Get back a GDI Device Context, we've added "::" the start of all of the
		// GDI calls to emphasize that we only use the PDFDCEX at the beginning and end.
		PDF::Point paperDimensions(8.5, 11);
		hDC2 = pdfDcEx.Begin( pdfdoc2, paperDimensions );

		UInt32 dpi = pdfDcEx.GetDPI(); 
		PDF::Point paperSize( paperDimensions.x * dpi, paperDimensions.y * dpi);

		::StartPage(hDC2);

		// We like to think of font height in "points" -- convert this to page pixels
		LOGFONTW tmpFont = ArialFont;
		tmpFont.lfHeight *= dpi / 72;
		hArialFont2 = ::CreateFontIndirectW( &tmpFont );
		hOldFont2 = ::SelectObject(hDC2, hArialFont2 );

		hPen2 = ::CreatePen(PS_SOLID, 1, RGB(0,0,0));
		hOldPen2 = ::SelectObject(hDC2, hPen2);
		::SetBkMode(hDC2, TRANSPARENT);
		::Ellipse(hDC2, (int)(paperSize.x/2 - dpi), (int)(paperSize.y/2 - dpi), (int)(paperSize.x/2 + dpi), (int)(paperSize.y/2 + dpi));
		::SetTextAlign(hDC2, TA_CENTER | TA_BOTTOM);
		::TextOutW(hDC2, (int)(paperSize.x/2), (int)(paperSize.y/2 + tmpFont.lfHeight / 2), L"Hello World", 11);
		::SelectObject(hDC2, hOldPen2);
		::DeleteObject(hPen2);
		::EndPage(hDC2);

		////////////////////////////////////////////////////////////////////////////
		// Page two
		// Reuse existing font and DC.
		::StartPage(hDC2);
		::Ellipse(hDC2, -20, -20, 20, 20);
		::SetTextAlign(hDC2, TA_CENTER | TA_BOTTOM);
		::TextOutW(hDC2, (int)(paperSize.x/2), (int)(paperSize.y/2 + ArialFont.lfHeight / 2), L"Page 2", 6);
		::SelectObject(hDC2, hOldFont2);
		::DeleteObject(hArialFont2);
		::EndPage(hDC2);

		// Complete the translation
		pdfDcEx.End();

		// Save the PDF document
		pdfdoc2.Save(outputPath + "PDFDCEXTest.pdf", SDF::SDFDoc::e_remove_unused, NULL);
		std::cout << "Saved PDFDCEXTest.pdf\nDone.\n";
	}
	catch(Common::Exception& e)
	{
		std::cout << e << std::endl;
		ret = 1;
	}
	catch(...)
	{
		std::cout << "Unknown Exception" << std::endl;
		ret = 1;
	}

	PDFNet::Terminate();
#endif // defined(_WIN32) && !defined(WINCE)
	return ret;
}
