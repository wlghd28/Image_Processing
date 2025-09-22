// Generated code. Do not modify!
//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//
using System;
using System.Drawing;
using System.Drawing.Printing;

using pdftron;
using pdftron.PDF;
using pdftron.Common;
using pdftron.Filters;

namespace PDFPrintTestCS
{
	/// <summary>
	/// The following sample illustrates how to print PDF document using currently selected
	/// default printer. 
	/// 
	/// The first example uses the new PDF::Print::StartPrintJob function to send a rasterization 
	/// of the document with optimal compression to the printer.  If the OS is Windows 7, then the
	/// XPS print path will be used to preserve vector quality.
	/// 
	/// The second example uses PDFDraw send unoptimized rasterized data.
	/// 
	/// If you would like to rasterize page at high resolutions (e.g. more than 600 DPI), you 
	/// should use PDFRasterizer or PDFNet vector output instead of PDFDraw. 
	/// </summary>
	class PDFPrint
	{
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static PDFPrint() {}
		
		PDFDoc pdfdoc = null;
		PDFDraw pdfdraw = null;
		PageIterator pageitr = null;

        static void Main(string[] args)
        {
            var driver = new PDFPrint();
            driver.Execute(args);
        }

        void Execute(string[] args)
		{
			PDFNet.Initialize();

			// Optional: Set ICC color profiles to fine tune color conversion 
			// for PDF 'device' color spaces. You can use your own ICC profiles. 
			// Standard Adobe color profiles can be download from Adobes site: 
			// http://www.adobe.com/support/downloads/iccprofiles/iccprofiles_win.html
			//
			// Simply drop all *.icc files in PDFNet resource folder or you specify 
			// the full pathname.
			try
			{
				// PDFNet.SetColorManagement();
				// PDFNet.SetDefaultDeviceCMYKProfile("USWebCoatedSWOP.icc"); // will search in PDFNet resource folder.
				// PDFNet.SetDefaultDeviceRGBProfile("AdobeRGB1998.icc"); 
			}
			catch (Exception)
			{
				Console.WriteLine("The specified color profile was not found.");
			}

			// Optional: Set predefined font mappings to override default font 
			// substitution for documents with missing fonts. For example:
			//---
			// PDFNet.AddFontSubst("StoneSans-Semibold", "C:/WINDOWS/Fonts/comic.ttf");
			// PDFNet.AddFontSubst("StoneSans", "comic.ttf");  // search for 'comic.ttf' in PDFNet resource folder.
			// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Identity, "C:/WINDOWS/Fonts/arialuni.ttf");
			// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Japan1, "C:/Program Files/Adobe/Acrobat 7.0/Resource/CIDFont/KozMinProVI-Regular.otf");
			// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Japan2, "c:/myfonts/KozMinProVI-Regular.otf");
			//
			// If fonts are in PDFNet resource folder, it is not necessary to specify 
			// the full path name. For example,
			//---
			// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_Korea1, "AdobeMyungjoStd-Medium.otf");
			// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_CNS1, "AdobeSongStd-Light.otf");
			// PDFNet.AddFontSubst(PDFNet.CharacterOrdering.e_GB1, "AdobeMingStd-Light.otf");

			string input_path = "../../TestFiles/";	// Relative path to the folder containing test files.
			try
			{
				// Open the PDF document.
				Console.WriteLine("Opening the input file...");
				using (pdfdoc = new PDFDoc(input_path + "tiger.pdf"))
				{
					pdfdoc.InitSecurityHandler();


					//////////////////////////////////////////////////////////////////////////
					// Example 1: use the PDF::Print::StartPrintJob interface
					// This is silent (no progress dialog) and blocks until print job is at spooler
					// The rasterized print job is compressed before sending to printer
					Console.WriteLine("Printing the input file using PDF.Print.StartPrintJob...");

					// Setup printing options:
					PrinterMode printerMode = new PrinterMode();
					printerMode.SetAutoCenter(true);
					printerMode.SetAutoRotate(true);
					printerMode.SetCollation(true);
					printerMode.SetCopyCount(1);
					printerMode.SetDPI(300); // regardless of ordering, an explicit DPI setting overrides the OutputQuality setting
					printerMode.SetDuplexing(PrinterMode.DuplexMode.e_Duplex_Auto);
					printerMode.SetNUp(PrinterMode.NUp.e_NUp_1_1, PrinterMode.NUpPageOrder.e_PageOrder_LeftToRightThenTopToBottom);
					printerMode.SetOrientation(PrinterMode.Orientation.e_Orientation_Portrait);
					printerMode.SetOutputAnnot(PrinterMode.PrintContentTypes.e_PrintContent_DocumentAndAnnotations);

					// If the XPS print path is being used, then the printer spooler file will
					// ignore the grayscale option and be in full color
					printerMode.SetOutputColor(PrinterMode.OutputColor.e_OutputColor_Grayscale);
					printerMode.SetOutputPageBorder(false);
					printerMode.SetOutputQuality(PrinterMode.OutputQuality.e_OutputQuality_Medium);
					printerMode.SetPaperSize(new Rect(0, 0, 612, 792));
					PageSet pagesToPrint = new PageSet(1, pdfdoc.GetPageCount(), PageSet.Filter.e_all);

					// You can get the name of the default printer by using:
					// PrinterSettings ps = new PrinterSettings();
					// String printerName   ps.PrinterName();
					// however Print.StartPrintJob can also determine this for you, just pass an empty printer name	

					// Print the document on the default printer, name the print job the name of the 
					// file, print to the printer not a file, and use printer options:
					Print.StartPrintJob(pdfdoc, "", pdfdoc.GetFileName(), "", pagesToPrint, printerMode, null);


					//////////////////////////////////////////////////////////////////////////
					// Example 2: use the .Net PrintDocument class and PDFDraw rasterizer
					// This will pop up a progress dialog

					// Start printing from the first page
					pageitr = pdfdoc.GetPageIterator();
					pdfdraw = new PDFDraw();
					pdfdraw.SetPrintMode(true);
					pdfdraw.SetRasterizerType(PDFRasterizer.Type.e_BuiltIn);

					// Create a printer
					PrintDocument printer = new PrintDocument();

					// name the document to be printed
					printer.DocumentName = pdfdoc.GetFileName();

					// Set the PrintPage delegate which will be invoked to print each page
					printer.PrintPage += new PrintPageEventHandler(PrintPage);

					Console.WriteLine("Printing the input file using .NET PrintDocument and PDFDraw...");
					printer.Print();	// Start printing

					pdfdraw.Dispose();	// Free allocated resources (generally a good idea when printing many documents).
				}
			}
			catch (PDFNetException e)
			{
				Console.WriteLine(e.Message);
			}

		}

#if NET_1_1
		// In case you need to account for 'hard margin' printer property.
		// .NET Framework 2.x or above offers direct access 'hard margin' property.
		[System.Runtime.InteropServices.DllImport("gdi32.dll")]
		private static extern int GetDeviceCaps(IntPtr hdc, int nIndex); 
		private const int PHYSICALOFFSETX = 112; 
		private const int PHYSICALOFFSETY = 113;
#endif

		// Print event hander
		void PrintPage(object sender, PrintPageEventArgs ev)
		{
			Graphics gr = ev.Graphics;

			Rectangle rectPage = ev.PageBounds;         //print without margins
			//Rectangle rectPage = ev.MarginBounds;     //print using margins

			float dpi = gr.DpiX;

			int example = 2;
			bool use_hard_margins = false;

			// Example 1) Print the Bitmap.
			if (example == 1)
			{
				pdfdraw.SetDPI(dpi);
				System.Drawing.Bitmap bmp = pdfdraw.GetBitmap(pageitr.Current());
				gr.DrawImage(bmp, rectPage, 0, 0, bmp.Width, bmp.Height, GraphicsUnit.Pixel);
			}

			// Example 2) Print via PDFDraw class.
			if (example == 2)
			{
				gr.PageUnit = GraphicsUnit.Inch;
				if (dpi > 300) dpi = 300;

				double left, right, top, bottom;

				if (use_hard_margins) // You could adjust the rectangle to account for hard and soft margins, etc.
				{
#if NET_1_1
					// This code is used to obtain printer hard margins when running on .NET 1.1x or below. 
					IntPtr hdc = new IntPtr();
					hdc = ev.Graphics.GetHdc(); // Get handle to device context.
			 
					double hardMarginX = GetDeviceCaps(hdc, PHYSICALOFFSETX);
					double hardMarginY  = GetDeviceCaps(hdc, PHYSICALOFFSETY);		
					ev.Graphics.ReleaseHdc(hdc); // Release handle to device context.
#else
					// If you are running on .NET Framework 2.x or above, you can directly access 'hard margin' property.
					double hardMarginX = ev.PageSettings.HardMarginX;
					double hardMarginY = ev.PageSettings.HardMarginY;
#endif
					left = (rectPage.Left - hardMarginX) / 100.0;
					right = (rectPage.Right - hardMarginX) / 100.0;
					top = (rectPage.Top - hardMarginY) / 100.0;
					bottom = (rectPage.Bottom - hardMarginY) / 100.0;
				}
				else
				{
					left = rectPage.Left / 100.0;
					right = rectPage.Right / 100.0;
					top = rectPage.Top / 100.0;
					bottom = rectPage.Bottom / 100.0;
				}

				// The above page dimensions are in inches. We need to convert 
				// the page dimensions to PDF units (or points). One point is 
				// 1/72 of an inch.
				pdftron.PDF.Rect rect = new Rect(left * 72, bottom * 72, right * 72, top * 72);

				try
				{
					pdfdraw.SetDPI(dpi);
					pdfdraw.DrawInRect(pageitr.Current(), gr, rect);
				}
				catch (Exception ex)
				{
					Console.WriteLine("Printing Error: " + ex.ToString());
				}
			}

			pageitr.Next();  // Move to the next page, if any
			ev.HasMorePages = pageitr.HasNext();
		}
	}
}

