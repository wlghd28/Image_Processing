//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// This sample demonstrates the basic usage of the high-level digital signatures API in PDFNet.
//
// The following steps reflect typical intended usage of the digital signatures API:
//
//	0.	Start with a PDF with or without form fields in it that one would like to lock (or, one can add a field, see (1)).
//	
//	1.	EITHER: 
//		(a) Call doc.createDigitalSignatureField, optionally providing a name. You receive a DigitalSignatureField.
//		-OR-
//		(b) If you didn't just create the digital signature field that you want to sign/certify, find the existing one within the 
//		document by using PDFDoc.DigitalSignatureFieldIterator or by using PDFDoc.getField to get it by its fully qualified name.
//	
//	2.	Create a signature widget annotation, and pass the DigitalSignatureField that you just created or found. 
//		If you want it to be visible, provide a Rect argument with a non-zero width or height, and don't set the
//		NoView and Hidden flags. [Optionally, add an appearance to the annotation when you wish to sign/certify.]
//		
//	[3. (OPTIONAL) Add digital signature restrictions to the document using the field modification permissions (SetFieldPermissions) 
//		or document modification permissions functions (SetDocumentPermissions) of DigitalSignatureField. These features disallow 
//		certain types of changes to be made to the document without invalidating the cryptographic digital signature's hash once it
//		is signed.]
//		
//	4. 	Call either CertifyOnNextSave or SignOnNextSave. There are three overloads for each one (six total):
//		a.	Taking a PKCS #12 keyfile path and its password
//		b.	Taking a buffer containing a PKCS #12 private keyfile and its password
//		c.	Taking a unique identifier of a signature handler registered with the PDFDoc. This overload is to be used
//			in the following fashion: 
//			i)		Extend and implement a new SignatureHandler. The SignatureHandler will be used to add or 
//					validate/check a digital signature.
//			ii)		Create an instance of the implemented SignatureHandler and register it with PDFDoc with 
//					pdfdoc.addSignatureHandler(). The method returns a SignatureHandlerId.
//			iii)	Call SignOnNextSaveWithCustomHandler/CertifyOnNextSaveWithCustomHandler with the SignatureHandlerId.
//		NOTE: It is only possible to sign/certify one signature per call to the Save function.
//	
//	5.	Call pdfdoc.save(). This will also create the digital signature dictionary and write a cryptographic hash to it.
//		IMPORTANT: If there are already signed/certified digital signature(s) in the document, you must save incrementally
//		so as to not invalidate the other signature's('s) cryptographic hashes. 
//
// Additional processing can be done before document is signed. For example, UseSignatureHandler() returns an instance
// of SDF dictionary which represents the signature dictionary (or the /V entry of the form field). This can be used to
// add additional information to the signature dictionary (e.g. Name, Reason, Location, etc.).
//
// Although the steps above describes extending the SignatureHandler class, this sample demonstrates the use of
// StdSignatureHandler (a built-in SignatureHandler in PDFNet) to sign a PDF file.
//----------------------------------------------------------------------------------------------------------------------

import java.util.Locale;
import com.pdftron.common.PDFNetException;
import com.pdftron.pdf.annots.TextWidget;
import com.pdftron.pdf.annots.SignatureWidget;
import com.pdftron.pdf.FieldIterator;
import com.pdftron.pdf.DigitalSignatureFieldIterator;
import com.pdftron.pdf.Field;
import com.pdftron.pdf.Image;
import com.pdftron.pdf.PDFDoc;
import com.pdftron.pdf.PDFNet;
import com.pdftron.pdf.Page;
import com.pdftron.pdf.Rect;
import com.pdftron.pdf.Date;
import com.pdftron.pdf.DigitalSignatureField;
import com.pdftron.pdf.VerificationOptions;
import com.pdftron.pdf.VerificationResult;
import com.pdftron.pdf.TrustVerificationResult;
import com.pdftron.pdf.DigestAlgorithm;
import com.pdftron.pdf.DisallowedChange;
import com.pdftron.sdf.Obj;
import com.pdftron.sdf.SignatureHandler;
import com.pdftron.sdf.SDFDoc;
import com.pdftron.filters.FilterReader;
import com.pdftron.filters.MappedFile;

public class DigitalSignaturesTest 
{
	// EXPERIMENTAL. Digital signature verification is undergoing active development, but currently does not support a number of features. If we are missing a feature that is important to you, or if you have files that do not act as expected, please contact us using one of the following forms: https://www.pdftron.com/form/trial-support/ or https://www.pdftron.com/form/request/
	static boolean VerifyAllAndPrint(String in_docpath, String in_public_key_file_path) throws PDFNetException
	{
		PDFDoc doc = new PDFDoc(in_docpath);
		System.out.println("==========");
		VerificationOptions opts = new VerificationOptions(VerificationOptions.SecurityLevel.e_compatibility_and_archiving);

		// Trust the public certificate we use for signing.
		MappedFile trusted_cert_file = new MappedFile(in_public_key_file_path);
		long file_sz = trusted_cert_file.fileSize();
		FilterReader file_reader = new FilterReader(trusted_cert_file);
		byte[] trusted_cert_buf = new byte[(int) file_sz];
		file_reader.read(trusted_cert_buf);
		opts.addTrustedCertificate(trusted_cert_buf);

		// Iterate over the signatures and verify all of them.
		DigitalSignatureFieldIterator digsig_fitr = doc.getDigitalSignatureFieldIterator();
		boolean verification_status = true;
		for (; digsig_fitr.hasNext(); )
		{
			DigitalSignatureField curr = digsig_fitr.next();
			VerificationResult result = curr.verify(opts);
			if (result.getVerificationStatus())
			{
				System.out.print("Signature verified, ");
			}
			else
			{
				System.out.print("Signature verification failed, ");
				verification_status = false;
			}
			System.out.println(String.format(Locale.US, "objnum: %d", curr.getSDFObj().getObjNum()));

			switch (result.getSignersDigestAlgorithm())
			{
				case e_sha1:
					System.out.println("Digest algorithm: SHA-1");
					break;
				case e_sha256:
					System.out.println("Digest algorithm: SHA-256");
					break;
				case e_sha384:
					System.out.println("Digest algorithm: SHA-384");
					break;
				case e_sha512:
					System.out.println("Digest algorithm: SHA-512");
					break;
				case e_ripemd160:
					System.out.println("Digest algorithm: RIPEMD-160");
					break;
				case e_unknown_digest_algorithm:
					System.out.println("Digest algorithm: unknown");
					break;
				default:
					System.err.println("unrecognized digest algorithm");
					assert(false);
			}

			System.out.println("Detailed verification result: ");
			switch (result.getDocumentStatus())
			{
				case e_no_error:
					System.out.println("\tNo general error to report.");
					break;
				case e_corrupt_file:
					System.out.println("\tSignatureHandler reported file corruption.");
					break;
				case e_unsigned:
					System.out.println("\tThe signature has not yet been cryptographically signed.");
					break;
				case e_bad_byteranges:
					System.out.println("\tSignatureHandler reports corruption in the ByteRanges in the digital signature.");
					break;
				case e_corrupt_cryptographic_contents:
					System.out.println("\tSignatureHandler reports corruption in the Contents of the digital signature.");
					break;
				default:
					System.err.println("unrecognized document status");
					assert(false);
			}
			switch (result.getDigestStatus())
			{
				case e_digest_invalid:
					System.out.println("\tThe digest is incorrect.");
					break;
				case e_digest_verified:
					System.out.println("\tThe digest is correct.");
					break;
				case e_digest_verification_disabled:
					System.out.println("\tDigest verification has been disabled.");
					break;
				case e_weak_digest_algorithm_but_digest_verifiable:
					System.out.println("\tThe digest is correct, but the digest algorithm is weak and not secure.");
					break;
				case e_no_digest_status:
					System.out.println("\tNo digest status to report.");
					break;
				case e_unsupported_encoding:
					System.out.println("\tNo installed SignatureHandler was able to recognize the signature's encoding.");
					break;
				default:
					System.err.println("unrecognized digest status");
					assert(false);
			}
			switch (result.getTrustStatus())
			{
				case e_trust_verified:
					System.out.println("\tEstablished trust in signer successfully.");
					break;
				case e_untrusted:
					System.out.println("\tTrust could not be established.");
					break;
				case e_trust_verification_disabled:
					System.out.println("\tTrust verification has been disabled.");
					break;
				case e_no_trust_status:
					System.out.println("\tNo trust status to report.");
					break;
				default:
					System.err.println("unrecognized trust status");
					assert(false);
			}
			switch (result.getPermissionsStatus())
			{
				case e_invalidated_by_disallowed_changes:
					System.out.println("\tThe document has changes that are disallowed by the signature's permissions settings.");
					break;
				case e_has_allowed_changes:
					System.out.println("\tThe document has changes that are allowed by the signature's permissions settings.");
					break;
				case e_unmodified:
					System.out.println("\tThe document has not been modified since it was signed.");
					break;
				case e_permissions_verification_disabled:
					System.out.println("\tPermissions verification has been disabled.");
					break;
				case e_no_permissions_status:
					System.out.println("\tNo permissions status to report.");
					break;
				default:
					System.err.println("unrecognized modification permissions status");
					assert(false);
			}

			DisallowedChange[] changes = result.getDisallowedChanges();
			for (DisallowedChange it2 : changes)
			{
				System.out.println(String.format(Locale.US, "\tDisallowed change: %s, objnum: %d", it2.getTypeAsString(), it2.getObjNum()));
			}

			// Get and print all the detailed trust-related results, if they are available.
			if (result.hasTrustVerificationResult())
			{
				TrustVerificationResult trust_verification_result = result.getTrustVerificationResult();
				System.out.println(trust_verification_result.wasSuccessful() ? "Trust verified." : "Trust not verifiable.");
				System.out.println(String.format(Locale.US, "Trust verification result string: %s", trust_verification_result.getResultString()));

			   long time_of_verification = trust_verification_result.getTimeOfTrustVerification();
				switch (trust_verification_result.getTimeOfTrustVerificationEnum())
				{
					case e_current:
						System.out.println(String.format(Locale.US, "Trust verification attempted with respect to current time (as epoch time): %d", time_of_verification));
						break;
					case e_signing:
						System.out.println(String.format(Locale.US, "Trust verification attempted with respect to signing time (as epoch time): %d", time_of_verification));
						break;
					case e_timestamp:
						System.out.println(String.format(Locale.US, "Trust verification attempted with respect to secure embedded timestamp (as epoch time): %d", time_of_verification));
						break;
					default:
						System.err.println("unrecognized time enum value");
						assert(false);
				}
			}
			else
			{
				System.out.println("No detailed trust verification result available.");
			}

			System.out.println("==========");
		}

		return verification_status;
	}
	
	public static void certifyPDF(String in_docpath,
		String in_cert_field_name,
		String in_private_key_file_path,
		String in_keyfile_password,
		String in_appearance_image_path,
		String in_outpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Certifying PDF document");

		// Open an existing PDF
		PDFDoc doc = new PDFDoc(in_docpath);

		if (doc.hasSignatures())
		{
			System.out.println("PDFDoc has signatures");
		}
		else
		{
			System.out.println("PDFDoc has no signatures");
		}

		Page page1 = doc.getPage(1);

		// Create a text field that we can lock using the field permissions feature.
		TextWidget annot1 = TextWidget.create(doc, new Rect(50, 550, 350, 600), "asdf_test_field");
		page1.annotPushBack(annot1);

		/* Create a new signature form field in the PDFDoc. The name argument is optional;
		leaving it empty causes it to be auto-generated. However, you may need the name for later.
		Acrobat doesn't show digsigfield in side panel if it's without a widget. Using a
		Rect with 0 width and 0 height, or setting the NoPrint/Invisible flags makes it invisible. */
		DigitalSignatureField certification_sig_field = doc.createDigitalSignatureField(in_cert_field_name);
		SignatureWidget widgetAnnot = SignatureWidget.create(doc, new Rect(0, 100, 200, 150), certification_sig_field);
		page1.annotPushBack(widgetAnnot);

		// (OPTIONAL) Add an appearance to the signature field.
		Image img = Image.create(doc, in_appearance_image_path);
		widgetAnnot.createSignatureAppearance(img);

		// Prepare the document locking permission level. It will be applied upon document certification.
		System.out.println("Adding document permissions.");
		certification_sig_field.setDocumentPermissions(DigitalSignatureField.DocumentPermissions.e_annotating_formfilling_signing_allowed);
		
		// Prepare to lock the text field that we created earlier.
		System.out.println("Adding field permissions.");
		String[] fields_to_lock = {"asdf_test_field"};
		certification_sig_field.setFieldPermissions(DigitalSignatureField.FieldPermissions.e_include, fields_to_lock);

		certification_sig_field.certifyOnNextSave(in_private_key_file_path, in_keyfile_password);

		// (OPTIONAL) Add more information to the signature dictionary.
		certification_sig_field.setLocation("Vancouver, BC");
		certification_sig_field.setReason("Document certification.");
		certification_sig_field.setContactInfo("www.pdftron.com");

		// Save the PDFDoc. Once the method below is called, PDFNet will also sign the document using the information provided.
		doc.save(in_outpath, SDFDoc.SaveMode.NO_FLAGS, null);

		System.out.println("================================================================================");
	}

	public static void signPDF(String in_docpath,
		String in_approval_field_name,
		String in_private_key_file_path,
		String in_keyfile_password,
		String in_appearance_img_path,
		String in_outpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Signing PDF document");

		// Open an existing PDF
		PDFDoc doc = new PDFDoc(in_docpath);

		// Retrieve the unsigned approval signature field.
		Field found_approval_field = doc.getField(in_approval_field_name);
		DigitalSignatureField found_approval_signature_digsig_field = new DigitalSignatureField(found_approval_field);
		
		// (OPTIONAL) Add an appearance to the signature field.
		Image img = Image.create(doc, in_appearance_img_path);
		SignatureWidget found_approval_signature_widget = new SignatureWidget(found_approval_field.getSDFObj());
		found_approval_signature_widget.createSignatureAppearance(img);

		// Prepare the signature and signature handler for signing.
		found_approval_signature_digsig_field.signOnNextSave(in_private_key_file_path, in_keyfile_password);

		// The actual approval signing will be done during the following incremental save operation.
		doc.save(in_outpath, SDFDoc.SaveMode.INCREMENTAL, null);

		System.out.println("================================================================================");
	}

	public static void clearSignature(String in_docpath,
		String in_digsig_field_name,
		String in_outpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Clearing certification signature");

		PDFDoc doc = new PDFDoc(in_docpath);

		DigitalSignatureField digsig = new DigitalSignatureField(doc.getField(in_digsig_field_name));
		
		System.out.println("Clearing signature: " + in_digsig_field_name);
		digsig.clearSignature();

		if (!digsig.hasCryptographicSignature())
		{
			System.out.println("Cryptographic signature cleared properly.");
		}

		// Save incrementally so as to not invalidate other signatures' hashes from previous saves.
		doc.save(in_outpath, SDFDoc.SaveMode.INCREMENTAL, null);

		System.out.println("================================================================================");
	}

	 public static void printSignaturesInfo(String in_docpath) throws PDFNetException
	{
		System.out.println("================================================================================");
		System.out.println("Reading and printing digital signature information");

		PDFDoc doc = new PDFDoc(in_docpath);
		if (!doc.hasSignatures())
		{
			System.out.println("Doc has no signatures.");
			System.out.println("================================================================================");
			return;
		}
		else
		{
			System.out.println("Doc has signatures.");
		}

		
		for (FieldIterator fitr = doc.getFieldIterator(); fitr.hasNext(); )
		{
			Field current = fitr.next();
			if (current.isLockedByDigitalSignature())
			{
				System.out.println("==========\nField locked by a digital signature");
			}
			else
			{
				System.out.println("==========\nField not locked by a digital signature");
			}

			System.out.println("Field name: " + current.getName());
			System.out.println("==========");
		}

		System.out.println("====================\nNow iterating over digital signatures only.\n====================");

		DigitalSignatureFieldIterator digsig_fitr = doc.getDigitalSignatureFieldIterator();
		for (; digsig_fitr.hasNext(); )
		{
			DigitalSignatureField current = digsig_fitr.next();
			System.out.println("==========");
			System.out.println("Field name of digital signature: " + new Field(current.getSDFObj()).getName());

			DigitalSignatureField digsigfield = current;
			if (!digsigfield.hasCryptographicSignature())
			{
				System.out.println("Either digital signature field lacks a digital signature dictionary, " +
					"or digital signature dictionary lacks a cryptographic hash entry. " +
					"Digital signature field is not presently considered signed.\n" +
					"==========");
				continue;
			}

			int cert_count = digsigfield.getCertCount();
			System.out.println("Cert count: " + cert_count);
			for (int i = 0; i < cert_count; ++i)
			{
				byte[] cert = digsigfield.getCert(i);
				System.out.println("Cert #" + i + " size: " + cert.length);
			}

			DigitalSignatureField.SubFilterType subfilter = digsigfield.getSubFilter();

			System.out.println("Subfilter type: " + subfilter.ordinal());

			if (subfilter != DigitalSignatureField.SubFilterType.e_ETSI_RFC3161)
			{
				System.out.println("Signature's signer: " + digsigfield.getSignatureName());

				Date signing_time = digsigfield.getSigningTime();
				if (signing_time.isValid())
				{
					System.out.println("Signing day: " + signing_time.getDay());
				}

				System.out.println("Location: " + digsigfield.getLocation());
				System.out.println("Reason: " + digsigfield.getReason());
				System.out.println("Contact info: " + digsigfield.getContactInfo());
			}
			else
			{
				System.out.println("SubFilter == e_ETSI_RFC3161 (DocTimeStamp; no signing info)");
			}

			if (digsigfield.hasVisibleAppearance())
			{
				System.out.println("Visible");
			}
			else
			{
				System.out.println("Not visible");
			}
			
			DigitalSignatureField.DocumentPermissions digsig_doc_perms = digsigfield.getDocumentPermissions();
			String[] locked_fields = digsigfield.getLockedFields();
			for (String it : locked_fields)
			{
				System.out.println("This digital signature locks a field named: " + it);
			}

			switch (digsig_doc_perms)
			{
			case e_no_changes_allowed:
				System.out.println("No changes to the document can be made without invalidating this digital signature.");
				break;
			case e_formfilling_signing_allowed:
				System.out.println("Page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature.");
				break;
			case e_annotating_formfilling_signing_allowed:
				System.out.println("Annotating, page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature.");
				break;
			case e_unrestricted:
				System.out.println("Document not restricted by this digital signature.");
				break;
			default:
				System.err.println("Unrecognized digital signature document permission level.");
				assert(false);
			}
			System.out.println("==========");
		}

		System.out.println("================================================================================");
	}

	public static void main(String[] args) 
	{
		// Initialize PDFNet
		PDFNet.initialize();

		boolean result = true;
		String input_path = "../../TestFiles/";
		String output_path = "../../TestFiles/Output/";

		//////////////////// TEST 0: 
		/* Create an approval signature field that we can sign after certifying.
		(Must be done before calling CertifyOnNextSave/SignOnNextSave/WithCustomHandler.) */
		try
		{
			PDFDoc doc = new PDFDoc(input_path + "tiger.pdf");
			DigitalSignatureField approval_signature_field = doc.createDigitalSignatureField("PDFTronApprovalSig");
			SignatureWidget widgetAnnotApproval = SignatureWidget.create(doc, new Rect(300, 300, 500, 200), approval_signature_field);
			Page page1 = doc.getPage(1);
			page1.annotPushBack(widgetAnnotApproval);
			doc.save(output_path + "tiger_withApprovalField_output.pdf", SDFDoc.SaveMode.REMOVE_UNUSED, null);
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}

		//////////////////// TEST 1: certify a PDF.
		try
		{
			certifyPDF(input_path + "tiger_withApprovalField.pdf",
				"PDFTronCertificationSig",
				input_path + "pdftron.pfx",
				"password",
				input_path + "pdftron.bmp",
				output_path + "tiger_withApprovalField_certified_output.pdf");
			printSignaturesInfo(output_path + "tiger_withApprovalField_certified_output.pdf");
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}

		//////////////////// TEST 2: sign a PDF with a certification and an unsigned signature field in it.
		try
		{
			signPDF(input_path + "tiger_withApprovalField_certified.pdf",
				"PDFTronApprovalSig",
				input_path + "pdftron.pfx",
				"password",
				input_path + "signature.jpg",
				output_path + "tiger_withApprovalField_certified_approved_output.pdf");
			printSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_output.pdf");
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}

		//////////////////// TEST 3: Clear a certification from a document that is certified and has an approval signature.
		try
		{
			clearSignature(input_path + "tiger_withApprovalField_certified_approved.pdf",
				"PDFTronCertificationSig",
				output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
			printSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}
		
		//////////////////// TEST 4: Verify a document's digital signatures.
		try
		{
			// EXPERIMENTAL. Digital signature verification is undergoing active development, but currently does not support a number of features. If we are missing a feature that is important to you, or if you have files that do not act as expected, please contact us using one of the following forms: https://www.pdftron.com/form/trial-support/ or https://www.pdftron.com/form/request/
			result &= VerifyAllAndPrint(input_path + "tiger_withApprovalField_certified_approved.pdf", input_path + "pdftron.cer");
		}
		catch (Exception e)
		{
			System.err.println(e.getMessage());
			e.printStackTrace(System.err);
			result = false;
		}

		//////////////////// End of tests. ////////////////////

		if (result)
		{
			System.out.println("Tests successful.\n==========");
		}
		else
		{
			System.out.println("Tests FAILED!!!\n==========");
		}
	}
}
