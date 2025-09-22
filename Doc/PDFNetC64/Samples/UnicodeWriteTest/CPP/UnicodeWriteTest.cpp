//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/ColorSpace.h>
#include <PDF/Rect.h>

#include <fstream>
#include <iostream>

using namespace std;

using namespace pdftron;
using namespace SDF;
using namespace PDF;

/**
 * This example illustrates how to create Unicode text and how to embed composite fonts.
 * 
 * Note: This demo assumes that 'arialuni.ttf' is present in '/Samples/TestFiles' 
 * directory. Arial Unicode MS is about 24MB in size and it comes together with Windows and 
 * MS Office.
 * 
 * For more information about Arial Unicode MS, please consult the following Microsoft Knowledge 
 * Base Article: WD2002: General Information About the Arial Unicode MS Font
 *    http://support.microsoft.com/support/kb/articles/q287/2/47.asp
 * 
 * For more information consult: 
 *    http://office.microsoft.com/search/results.aspx?Scope=DC&Query=font&CTT=6&Origin=EC010331121033
 *    http://www.microsoft.com/downloads/details.aspx?FamilyID=1F0303AE-F055-41DA-A086-A65F22CB5593
 * 
 * In case you don't have access to Arial Unicode MS you can use cyberbit.ttf 
 * (http://ftp.netscape.com/pub/communicator/extras/fonts/windows/) instead.
 */
int main(int argc, char *argv[])
{
	int ret = 0;
	PDFNet::Initialize();

	// Relative path to the folder containing test files.
	string input_path =  "../../TestFiles/";
	string output_path = "../../TestFiles/Output/";

	try  
	{	 
		PDFDoc doc;

		ElementBuilder eb;		
		ElementWriter writer;	

		// Start a new page ------------------------------------
		Page page = doc.PageCreate(Rect(0, 0, 612, 794));

		writer.Begin(page);	// begin writing to this page

        string font_program = input_path + "ARIALUNI.TTF";
        // RAII block for ifstream
        {
            std::ifstream ifs(font_program.c_str(), ios_base::in);
            if (!ifs) {
#if defined(_WIN32)
                font_program = string("C:/Windows/Fonts/ARIALUNI.TTF");
                cout << "Note: Using ARIALUNI.TTF from C:/Windows/Fonts directory." << endl;
#else
                cerr << "Error: Cannot find ARIALUNI.TTF." << endl;
	            PDFNet::Terminate();
                return 1;
#endif
            }
        }

		Font fnt = Font::CreateCIDTrueTypeFont(doc, font_program.c_str(), true, true);
		
		Element element = eb.CreateTextBegin(fnt, 1);
		element.SetTextMatrix(10, 0, 0, 10, 50, 600);
		element.GetGState().SetLeading(2);		 // Set the spacing between lines
		writer.WriteElement(element);

		// Hello World!
		Unicode hello[] = { 'H','e','l','l','o',' ','W','o','r','l','d','!'};
		writer.WriteElement(eb.CreateUnicodeTextRun(hello, sizeof(hello)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

        // Latin
		Unicode latin[] = {   
			'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 0x45, 0x0046, 0x00C0, 
			0x00C1, 0x00C2, 0x0143, 0x0144, 0x0145, 0x0152, '1', '2' // etc.
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(latin, sizeof(latin)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

        // Greek
		Unicode greek[] = {   
			0x039E, 0x039F, 0x03A0, 0x03A1,0x03A3, 0x03A6, 0x03A8, 0x03A9  // etc.
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(greek, sizeof(greek)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

        // Cyrillic
		Unicode cyrillic[] = {   
			0x0409, 0x040A, 0x040B, 0x040C, 0x040E, 0x040F, 0x0410, 0x0411,
		    0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419 // etc.
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(cyrillic, sizeof(cyrillic)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

        // Hebrew
		Unicode hebrew[] = {
			0x05D0, 0x05D1, 0x05D3, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7, 0x05D8, 
			0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF, 0x05E0, 0x05E1 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(hebrew, sizeof(hebrew)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

        // Arabic
		Unicode arabic[] = {
			0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A, 0x062B, 0x062C, 
			0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(arabic, sizeof(arabic)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Thai 
		Unicode thai[] = {
			0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07, 0x0E08, 0x0E09, 
			0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F, 0x0E10, 0x0E11, 0x0E12 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(thai, sizeof(thai)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Hiragana - Japanese 
		Unicode hiragana[] = {
			0x3041, 0x3042, 0x3043, 0x3044, 0x3045, 0x3046, 0x3047, 0x3048, 0x3049, 
			0x304A, 0x304B, 0x304C, 0x304D, 0x304E, 0x304F, 0x3051, 0x3051, 0x3052 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(hiragana, sizeof(hiragana)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// CJK Unified Ideographs
		Unicode cjk_uni[] = {
			0x5841, 0x5842, 0x5843, 0x5844, 0x5845, 0x5846, 0x5847, 0x5848, 0x5849, 
			0x584A, 0x584B, 0x584C, 0x584D, 0x584E, 0x584F, 0x5850, 0x5851, 0x5852 // etc. 
		};
		writer.WriteElement(eb.CreateUnicodeTextRun(cjk_uni, sizeof(cjk_uni)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Simplified Chinese
		Unicode chinese_simplified[] = {
			0x4e16, 0x754c, 0x60a8, 0x597d
		};

		writer.WriteElement(eb.CreateUnicodeTextRun(chinese_simplified, sizeof(chinese_simplified)/sizeof(Unicode)));
		writer.WriteElement(eb.CreateTextNewLine());

		// Finish the block of text
		writer.WriteElement(eb.CreateTextEnd());		

		writer.End();  // save changes to the current page
		doc.PagePushBack(page);

		doc.Save((output_path + "unicodewrite.pdf").c_str(), SDFDoc::e_remove_unused | SDFDoc::e_hex_strings , NULL);
		cout << "Done. Result saved in unicodewrite.pdf..." << endl;
	}
	catch(Common::Exception& e)
	{
		cout << e << endl;
		ret = 1;
	}
	catch(...)
	{
		cout << "Unknown Exception" << endl;
		ret = 1;
	}
	
	PDFNet::Terminate();
	return ret;
}
