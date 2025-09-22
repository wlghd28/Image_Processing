//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

import com.pdftron.pdf.*;
import com.pdftron.sdf.SDFDoc;

/**
 * This example illustrates how to create Unicode text and how to embed composite fonts.
 * <p>
 * Note: This demo assumes that 'arialuni.ttf' is present in '/Samples/TestFiles'
 * directory. Arial Unicode MS is about 24MB in size and it comes together with Windows and
 * MS Office.
 * <p>
 * For more information about Arial Unicode MS, please consult the following Microsoft Knowledge
 * Base Article: WD2002: General Information About the Arial Unicode MS Font
 * http://support.microsoft.com/support/kb/articles/q287/2/47.asp
 * <p>
 * For more information consult:
 * http://office.microsoft.com/search/results.aspx?Scope=DC&Query=font&CTT=6&Origin=EC010331121033
 * http://www.microsoft.com/downloads/details.aspx?FamilyID=1F0303AE-F055-41DA-A086-A65F22CB5593
 * <p>
 * In case you don't have access to Arial Unicode MS you can use cyberbit.ttf
 * (http://ftp.netscape.com/pub/communicator/extras/fonts/windows/) instead.
 */
public class UnicodeWriteTest {
    public static void main(String[] args) {
        PDFNet.initialize();

        // Relative path to the folder containing test files.
        String input_path = "../../TestFiles/";
        String output_path = "../../TestFiles/Output/";

        try {
            PDFDoc doc = new PDFDoc();

            ElementBuilder eb = new ElementBuilder();
            ElementWriter writer = new ElementWriter();

            // Start a new page ------------------------------------
            Page page = doc.pageCreate(new Rect(0, 0, 612, 794));

            writer.begin(page);    // begin writing to this page

            Font fnt;
            try {
                // Embed and subset the font
                fnt = Font.createCIDTrueTypeFont(doc, input_path + "arialuni.ttf", true, true);
            } catch (Exception e) {
                System.out.println(e);
                System.out.println("Note: 'arialuni.ttf' font file was not found in " + input_path + " directory.\n");
                PDFNet.terminate();
                return;
            }

            Element element = eb.createTextBegin(fnt, 1);
            element.setTextMatrix(10, 0, 0, 10, 50, 600);
            element.getGState().setLeading(2);         // Set the spacing between lines
            writer.writeElement(element);

            // Hello World!
            char hello[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
            writer.writeElement(eb.createUnicodeTextRun(new String(hello)));
            writer.writeElement(eb.createTextNewLine());

            // Latin
            char latin[] = {
                    'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 0x45, 0x0046, 0x00C0,
                    0x00C1, 0x00C2, 0x0143, 0x0144, 0x0145, 0x0152, '1', '2' // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(latin)));
            writer.writeElement(eb.createTextNewLine());

            // Greek
            char greek[] = {
                    0x039E, 0x039F, 0x03A0, 0x03A1, 0x03A3, 0x03A6, 0x03A8, 0x03A9  // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(greek)));
            writer.writeElement(eb.createTextNewLine());

            // Cyrillic
            char cyrilic[] = {
                    0x0409, 0x040A, 0x040B, 0x040C, 0x040E, 0x040F, 0x0410, 0x0411,
                    0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418, 0x0419 // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(cyrilic)));
            writer.writeElement(eb.createTextNewLine());

            // Hebrew
            char hebrew[] = {
                    0x05D0, 0x05D1, 0x05D3, 0x05D3, 0x05D4, 0x05D5, 0x05D6, 0x05D7, 0x05D8,
                    0x05D9, 0x05DA, 0x05DB, 0x05DC, 0x05DD, 0x05DE, 0x05DF, 0x05E0, 0x05E1 // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(hebrew)));
            writer.writeElement(eb.createTextNewLine());

            // Arabic
            char arabic[] = {
                    0x0624, 0x0625, 0x0626, 0x0627, 0x0628, 0x0629, 0x062A, 0x062B, 0x062C,
                    0x062D, 0x062E, 0x062F, 0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635 // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(arabic)));
            writer.writeElement(eb.createTextNewLine());

            // Thai
            char thai[] = {
                    0x0E01, 0x0E02, 0x0E03, 0x0E04, 0x0E05, 0x0E06, 0x0E07, 0x0E08, 0x0E09,
                    0x0E0A, 0x0E0B, 0x0E0C, 0x0E0D, 0x0E0E, 0x0E0F, 0x0E10, 0x0E11, 0x0E12 // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(thai)));
            writer.writeElement(eb.createTextNewLine());

            // Hiragana - Japanese
            char hiragana[] = {
                    0x3041, 0x3042, 0x3043, 0x3044, 0x3045, 0x3046, 0x3047, 0x3048, 0x3049,
                    0x304A, 0x304B, 0x304C, 0x304D, 0x304E, 0x304F, 0x3051, 0x3051, 0x3052 // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(hiragana)));
            writer.writeElement(eb.createTextNewLine());

            // CJK Unified Ideographs
            char cjk_uni[] = {
                    0x5841, 0x5842, 0x5843, 0x5844, 0x5845, 0x5846, 0x5847, 0x5848, 0x5849,
                    0x584A, 0x584B, 0x584C, 0x584D, 0x584E, 0x584F, 0x5850, 0x5851, 0x5852 // etc.
            };
            writer.writeElement(eb.createUnicodeTextRun(new String(cjk_uni)));
            writer.writeElement(eb.createTextNewLine());

            // Finish the block of text
            writer.writeElement(eb.createTextEnd());

            writer.end();  // save changes to the current page
            doc.pagePushBack(page);

            doc.save(output_path + "unicodewrite.pdf", new SDFDoc.SaveMode[]{SDFDoc.SaveMode.REMOVE_UNUSED, SDFDoc.SaveMode.HEX_STRINGS}, null);
            // output PDF doc
            doc.close();
            System.out.println("Done. Result saved in unicodewrite.pdf...");
        } catch (Exception e) {
            e.printStackTrace();
        }

        PDFNet.terminate();
    }

}
