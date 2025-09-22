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
//		(a) Call doc.CreateDigitalSignatureField, optionally providing a name. You receive a DigitalSignatureField.
//		-OR-
//		(b) If you didn't just create the digital signature field that you want to sign/certify, find the existing one within the 
//		document by using PDFDoc.DigitalSignatureFieldIterator or by using PDFDoc.GetField to get it by its fully qualified name.
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
//					pdfdoc.AddSignatureHandler(). The method returns a SignatureHandlerId.
//			iii)	Call SignOnNextSaveWithCustomHandler/CertifyOnNextSaveWithCustomHandler with the SignatureHandlerId.
//		NOTE: It is only possible to sign/certify one signature per call to the Save function.
//	
//	5.	Call pdfdoc.Save(). This will also create the digital signature dictionary and write a cryptographic hash to it.
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

// To build and run this sample with OpenSSL, please specify OpenSSL include & lib paths to project settings.
//
// In MSVC, this can be done by opening the DigitalSignatureTest project's properties. Go to Configuration Properties ->
// C/C++ -> General -> Additional Include Directories. Add the path to the OpenSSL headers here. Next, go to
// Configuration Properties -> Linker -> General -> Additional Library Directories. Add the path to the OpenSSL libraries
// here. Finally, under Configuration Properties -> Linker -> Input -> Additional Dependencies, add libeay32.lib,
// crypt32.lib, and advapi32.lib in the list.
//
// For GCC, modify the Makefile, add -lcrypto to the $(LIBS) variable. If OpenSSL is installed elsewhere, it may be
// necessary to add the path to the headers in the $(INCLUDE) variable as well as the location of either libcrypto.a or
// libcrypto.so/libcrypto.dylib.
//

#define USE_STD_SIGNATURE_HANDLER 1 // Comment out this line if you intend to use OpenSSLSignatureHandler rather than StdSignatureHandler.

// standard library includes
#include <cstdio>
#include <iostream>
#include <vector>

// PDFNetC includes
#include <Common/Exception.h>
#include <Common/UString.h>
#include <PDF/Page.h>
#include <PDF/Annot.h>
#include <PDF/Annots/TextWidget.h>
#include <PDF/Date.h>
#include <PDF/Element.h>
#include <PDF/ElementBuilder.h>
#include <PDF/ElementWriter.h>
#include <PDF/Field.h>
#include <PDF/Image.h>
#include <PDF/PDFDoc.h>
#include <PDF/PDFNet.h>
#include <SDF/SignatureHandler.h>
#include <PDF/Annots/SignatureWidget.h>
#include <PDF/VerificationResult.h>
#include <PDF/TrustVerificationResult.h>
#include <PDF/DisallowedChange.h>
#include <Filters/MappedFile.h>

#if (!USE_STD_SIGNATURE_HANDLER)
// OpenSSL includes
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pkcs12.h>
#include <openssl/pkcs7.h>
#include <openssl/rsa.h>
#include <openssl/sha.h>
#endif // (!USE_STD_SIGNATURE_HANDLER)

using namespace std;
using namespace pdftron;
using namespace pdftron::SDF;
using namespace pdftron::PDF::Annots;
using namespace pdftron::PDF;

//////////////////// Here follows an example of how to implement a custom signature handler. //////////
#if (!USE_STD_SIGNATURE_HANDLER)
//
// Extend SignatureHandler by using OpenSSL signing utilities.
//
class OpenSSLSignatureHandler : public SignatureHandler
{
public:
	OpenSSLSignatureHandler(const char* in_pfxfile, const char* in_password) : m_pfxfile(in_pfxfile), m_password(in_password)
	{
		FILE* fp = fopen(in_pfxfile, "rb");
		if (fp == NULL)
			throw (Common::Exception("Cannot open private key.", __LINE__, __FILE__, "PKCS7Signature::PKCS7Signature", "Cannot open private key."));

		PKCS12* p12 = d2i_PKCS12_fp(fp, NULL);
		fclose(fp);

		if (p12 == NULL)
			throw (Common::Exception("Cannot parse private key.", __LINE__, __FILE__, "PKCS7Signature::PKCS7Signature", "Cannot parse private key."));

		mp_pkey = NULL;
		mp_x509 = NULL;
		mp_ca = NULL;
		int parseResult = PKCS12_parse(p12, in_password, &mp_pkey, &mp_x509, &mp_ca);
		PKCS12_free(p12);

		if (parseResult == 0)
			throw (Common::Exception("Cannot parse private key.", __LINE__, __FILE__, "PKCS7Signature::PKCS7Signature", "Cannot parse private key."));

		Reset();
	}

	virtual UString GetName() const
	{
		return (UString("Adobe.PPKLite"));
	}

	virtual void AppendData(const std::vector<pdftron::UInt8>& in_data)
	{
		SHA1_Update(&m_sha_ctx, (const void*) &(in_data[0]), in_data.size());
		return;
	}

	virtual bool Reset()
	{
		m_digest.resize(0);
		m_digest.clear();
		SHA1_Init(&m_sha_ctx);
		return (true);
	}

	virtual std::vector<pdftron::UInt8> CreateSignature()
	{
		if (m_digest.size() == 0) {
			m_digest.resize(SHA_DIGEST_LENGTH);
			SHA1_Final(&(m_digest[0]), &m_sha_ctx);
		}

		PKCS7* p7 = PKCS7_new();
		PKCS7_set_type(p7, NID_pkcs7_signed);

		PKCS7_SIGNER_INFO* p7Si = PKCS7_add_signature(p7, mp_x509, mp_pkey, EVP_sha1());
		PKCS7_add_attrib_content_type(p7Si, OBJ_nid2obj(NID_pkcs7_data));
		PKCS7_add0_attrib_signing_time(p7Si, NULL);
		PKCS7_add1_attrib_digest(p7Si, &(m_digest[0]), (int)m_digest.size());
		PKCS7_add_certificate(p7, mp_x509);

		for (int c = 0; c < sk_X509_num(mp_ca); c++) {
			X509* cert = sk_X509_value(mp_ca, c);
			PKCS7_add_certificate(p7, cert);
		}
		PKCS7_set_detached(p7, 1);
		PKCS7_content_new(p7, NID_pkcs7_data);

		PKCS7_SIGNER_INFO_sign(p7Si);

		int p7Len = i2d_PKCS7(p7, NULL);
		std::vector<unsigned char> result(p7Len);
		UInt8* pP7Buf = &(result[0]);
		i2d_PKCS7(p7, &pP7Buf);

		PKCS7_free(p7);

		return (result);
	}

	virtual OpenSSLSignatureHandler* Clone() const
	{
		return (new OpenSSLSignatureHandler(m_pfxfile.c_str(), m_password.c_str()));
	}

	virtual ~OpenSSLSignatureHandler()
	{
		sk_X509_free(mp_ca);
		X509_free(mp_x509);
		EVP_PKEY_free(mp_pkey);
	}

private:
	std::vector<UInt8> m_digest;
	std::string m_pfxfile;
	std::string m_password;

	SHA_CTX m_sha_ctx;
	EVP_PKEY* mp_pkey;      // private key
	X509* mp_x509;          // signing certificate
	STACK_OF(X509)* mp_ca;  // certificate chain up to the CA
}; // class OpenSSLSignatureHandler
#endif // (!USE_STD_SIGNATURE_HANDLER)
////////// End of the OpenSSLSignatureHandler custom handler code. ////////////////////

string input_path =  "../../TestFiles/";
string output_path = "../../TestFiles/Output/";

// EXPERIMENTAL. Digital signature verification is undergoing active development, but currently does not support a number of features. If we are missing a feature that is important to you, or if you have files that do not act as expected, please contact us using one of the following forms: https://www.pdftron.com/form/trial-support/ or https://www.pdftron.com/form/request/
bool VerifyAllAndPrint(const UString& in_docpath, const UString& in_public_key_file_path)
{
	PDFDoc doc(in_docpath);
	cout << "==========" << endl;
	PDF::VerificationOptions opts(PDF::VerificationOptions::e_compatibility_and_archiving);
	
	// Trust the public certificate we use for signing.
	std::vector<UInt8> trusted_cert_buf;
	Filters::MappedFile trusted_cert_file(in_public_key_file_path);
	size_t file_sz = trusted_cert_file.FileSize();
	Filters::FilterReader file_reader(trusted_cert_file);
	trusted_cert_buf.resize(file_sz);
	file_reader.Read(&trusted_cert_buf.front(), file_sz);
	opts.AddTrustedCertificate(&trusted_cert_buf.front(), trusted_cert_buf.size());

	// Iterate over the signatures and verify all of them.
	DigitalSignatureFieldIterator digsig_fitr = doc.GetDigitalSignatureFieldIterator();
	bool verification_status = true;
	for (; digsig_fitr.HasNext(); digsig_fitr.Next())
	{
		DigitalSignatureField curr = digsig_fitr.Current();
		VerificationResult result = curr.Verify(opts);
		if (result.GetVerificationStatus())
		{
			cout << "Signature verified, ";
		}
		else
		{
			cout << "Signature verification failed, ";
			verification_status = false;
		}
		cout << "objnum: " << curr.GetSDFObj().GetObjNum() << endl;
		
		switch (result.GetSignersDigestAlgorithm())
		{
		case DigestAlgorithm::e_SHA1:
			cout << "Digest algorithm: SHA-1" << endl;
			break;
		case DigestAlgorithm::e_SHA256:
			cout << "Digest algorithm: SHA-256" << endl;
			break;
		case DigestAlgorithm::e_SHA384:
			cout << "Digest algorithm: SHA-384" << endl;
			break;
		case DigestAlgorithm::e_SHA512:
			cout << "Digest algorithm: SHA-512" << endl;
			break;
		case DigestAlgorithm::e_RIPEMD160:
			cout << "Digest algorithm: RIPEMD-160" << endl;
			break;
		case DigestAlgorithm::e_unknown_digest_algorithm:
			cout << "Digest algorithm: unknown" << endl;
			break;
		default:
			BASE_ASSERT(false, "unrecognized digest algorithm");
		}

		cout << "Detailed verification result: " << endl;
		switch (result.GetDocumentStatus())
		{
		case VerificationResult::e_no_error:
			cout << "\tNo general error to report." << endl;
			break;
		case VerificationResult::e_corrupt_file:
			cout << "\tSignatureHandler reported file corruption." << endl;
			break;
		case VerificationResult::e_unsigned:
			cout << "\tThe signature has not yet been cryptographically signed." << endl;
			break;
		case VerificationResult::e_bad_byteranges:
			cout << "\tSignatureHandler reports corruption in the ByteRanges in the digital signature." << endl;
			break;
		case VerificationResult::e_corrupt_cryptographic_contents:
			cout << "\tSignatureHandler reports corruption in the Contents of the digital signature." << endl;
			break;
		default:
			BASE_ASSERT(false, "unrecognized document status");
		}
		switch (result.GetDigestStatus())
		{
		case VerificationResult::e_digest_invalid:
			cout << "\tThe digest is incorrect." << endl;
			break;
		case VerificationResult::e_digest_verified:
			cout << "\tThe digest is correct." << endl;
			break;
		case VerificationResult::e_digest_verification_disabled:
			cout << "\tDigest verification has been disabled." << endl;
			break;
		case VerificationResult::e_weak_digest_algorithm_but_digest_verifiable:
			cout << "\tThe digest is correct, but the digest algorithm is weak and not secure." << endl;
			break;
		case VerificationResult::e_no_digest_status:
			cout << "\tNo digest status to report." << endl;
			break;
		case VerificationResult::e_unsupported_encoding:
			cout << "\tNo installed SignatureHandler was able to recognize the signature's encoding." << endl;
			break;
		default:
			BASE_ASSERT(false, "unrecognized digest status");
		}
		switch (result.GetTrustStatus())
		{
		case VerificationResult::e_trust_verified:
			cout << "\tEstablished trust in signer successfully." << endl;
			break;
		case VerificationResult::e_untrusted:
			cout << "\tTrust could not be established." << endl;
			break;
		case VerificationResult::e_trust_verification_disabled:
			cout << "\tTrust verification has been disabled." << endl;
			break;
		case VerificationResult::e_no_trust_status:
			cout << "\tNo trust status to report." << endl;
			break;
		default:
			BASE_ASSERT(false, "unrecognized trust status");
		}
		switch (result.GetPermissionsStatus())
		{
		case VerificationResult::e_invalidated_by_disallowed_changes:
			cout << "\tThe document has changes that are disallowed by the signature's permissions settings." << endl;
			break;
		case VerificationResult::e_has_allowed_changes:
			cout << "\tThe document has changes that are allowed by the signature's permissions settings." << endl;
			break;
		case VerificationResult::e_unmodified:
			cout << "\tThe document has not been modified since it was signed." << endl;
			break;
		case VerificationResult::e_permissions_verification_disabled:
			cout << "\tPermissions verification has been disabled." << endl;
			break;
		case VerificationResult::e_no_permissions_status:
			cout << "\tNo permissions status to report." << endl;
			break;
		default:
			BASE_ASSERT(false, "unrecognized modification permissions status");
		}

		std::vector<DisallowedChange> changes = result.GetDisallowedChanges();
		for (std::vector<DisallowedChange>::iterator it2 = changes.begin(); it2 != changes.end(); ++it2)
		{
			cout << "\tDisallowed change: " << it2->GetTypeAsString().ConvertToUtf8() << ", objnum: " << it2->GetObjNum() << endl;
		}
		
		// Get and print all the detailed trust-related results, if they are available.
		if (result.HasTrustVerificationResult())
		{
			const TrustVerificationResult trust_verification_result = result.GetTrustVerificationResult();
			trust_verification_result.WasSuccessful()? cout << "Trust verified." << endl : cout << "Trust not verifiable." << endl;
			cout << "Trust verification result string: " << trust_verification_result.GetResultString().ConvertToUtf8() << endl;

			Int64 tmp_time_t = trust_verification_result.GetTimeOfTrustVerification();
			switch (trust_verification_result.GetTimeOfTrustVerificationEnum())
			{
			case VerificationOptions::e_current:
				cout << "Trust verification attempted with respect to current time (as epoch time): " << tmp_time_t << endl;
				break;
			case VerificationOptions::e_signing:
				cout << "Trust verification attempted with respect to signing time (as epoch time): " << tmp_time_t << endl;
				break;
			case VerificationOptions::e_timestamp:
				cout << "Trust verification attempted with respect to secure embedded timestamp (as epoch time): " << tmp_time_t << endl;
				break;
			default:
				BASE_ASSERT(false, "unrecognized time enum value");
			}
		}
		else
		{
			cout << "No detailed trust verification result available." << endl;
		}

		cout << "==========" << endl;
	}

	return verification_status;
}

void CertifyPDF(const UString& in_docpath,
	const UString& in_cert_field_name,
	const UString& in_private_key_file_path,
	const UString& in_keyfile_password,
	const UString& in_appearance_image_path,
	const UString& in_outpath)
{
	cout << "================================================================================" << endl;
	cout << "Certifying PDF document" << endl;

	// Open an existing PDF
	PDFDoc doc(in_docpath);

	cout << "PDFDoc has " << (doc.HasSignatures() ? "signatures" : "no signatures") << endl;

	Page page1 = doc.GetPage(1);

	// Create a text field that we can lock using the field permissions feature.
	Annots::TextWidget annot1 = Annots::TextWidget::Create(doc, Rect(50, 550, 350, 600), "asdf_test_field");
	page1.AnnotPushBack(annot1);

	/* Create a new signature form field in the PDFDoc. The name argument is optional;
	leaving it empty causes it to be auto-generated. However, you may need the name for later.
	Acrobat doesn't show digsigfield in side panel if it's without a widget. Using a
	Rect with 0 width and 0 height, or setting the NoPrint/Invisible flags makes it invisible. */
	PDF::DigitalSignatureField certification_sig_field = doc.CreateDigitalSignatureField(in_cert_field_name);
	Annots::SignatureWidget widgetAnnot = Annots::SignatureWidget::Create(doc, Rect(0, 100, 200, 150), certification_sig_field);
	page1.AnnotPushBack(widgetAnnot);

	// (OPTIONAL) Add an appearance to the signature field.
	PDF::Image img = PDF::Image::Create(doc, in_appearance_image_path);
	widgetAnnot.CreateSignatureAppearance(img);

	// Prepare the document locking permission level. It will be applied upon document certification.
	cout << "Adding document permissions." << endl;
	certification_sig_field.SetDocumentPermissions(DigitalSignatureField::e_annotating_formfilling_signing_allowed);
	
	// Prepare to lock the text field that we created earlier.
	cout << "Adding field permissions." << endl;
	vector<UString> fields_to_lock;
	fields_to_lock.push_back("asdf_test_field");
	certification_sig_field.SetFieldPermissions(DigitalSignatureField::e_include, fields_to_lock);

#ifdef USE_STD_SIGNATURE_HANDLER
	certification_sig_field.CertifyOnNextSave(in_private_key_file_path, in_keyfile_password);
#else
	OpenSSLSignatureHandler sigHandler(in_private_key_file_path, in_keyfile_password);
	SignatureHandlerId sigHandlerId = doc.AddSignatureHandler(sigHandler);
	certification_sig_field.CertifyOnNextSaveWithCustomHandler(sigHandlerId);
#endif

	// (OPTIONAL) Add more information to the signature dictionary.
	certification_sig_field.SetLocation("Vancouver, BC");
	certification_sig_field.SetReason("Document certification.");
	certification_sig_field.SetContactInfo("www.pdftron.com");

	// Save the PDFDoc. Once the method below is called, PDFNet will also sign the document using the information provided.
	doc.Save(in_outpath, 0, NULL);

	cout << "================================================================================" << endl;
}

void SignPDF(const UString& in_docpath,
	const UString& in_approval_field_name,
	const UString& in_private_key_file_path,
	const UString& in_keyfile_password,
	const UString& in_appearance_img_path,
	const UString& in_outpath)
{
	cout << "================================================================================" << endl;
	cout << "Signing PDF document" << endl;

	// Open an existing PDF
	PDFDoc doc(in_docpath);

	// Retrieve the unsigned approval signature field.
	Field found_approval_field(doc.GetField(in_approval_field_name));
	PDF::DigitalSignatureField found_approval_signature_digsig_field(found_approval_field);
	
	// (OPTIONAL) Add an appearance to the signature field.
	PDF::Image img = PDF::Image::Create(doc, in_appearance_img_path);
	Annots::SignatureWidget found_approval_signature_widget(found_approval_field.GetSDFObj());
	found_approval_signature_widget.CreateSignatureAppearance(img);

	// Prepare the signature and signature handler for signing.
#ifdef USE_STD_SIGNATURE_HANDLER
	found_approval_signature_digsig_field.SignOnNextSave(in_private_key_file_path, in_keyfile_password);
#else
	OpenSSLSignatureHandler sigHandler(in_private_key_file_path, in_keyfile_password);
	SignatureHandlerId sigHandlerId = doc.AddSignatureHandler(sigHandler);
	found_approval_signature_digsig_field.SignOnNextSaveWithCustomHandler(sigHandlerId);
#endif

	// The actual approval signing will be done during the following incremental save operation.
	doc.Save(in_outpath, SDFDoc::e_incremental, NULL);

	cout << "================================================================================" << endl;
}

void ClearSignature(const UString& in_docpath,
	const UString& in_digsig_field_name,
	const UString& in_outpath)
{
	cout << "================================================================================" << endl;
	cout << "Clearing certification signature" << endl;

	PDFDoc doc(in_docpath);

	DigitalSignatureField digsig(doc.GetField(in_digsig_field_name));
	
	cout << "Clearing signature: " << in_digsig_field_name << endl;
	digsig.ClearSignature();

	if (!digsig.HasCryptographicSignature())
	{
		cout << "Cryptographic signature cleared properly." << endl;
	}

	// Save incrementally so as to not invalidate other signatures' hashes from previous saves.
	doc.Save(in_outpath, SDFDoc::e_incremental, NULL);

	cout << "================================================================================" << endl;
}

void PrintSignaturesInfo(const UString& in_docpath)
{
	cout << "================================================================================" << endl;
	cout << "Reading and printing digital signature information" << endl;

	PDFDoc doc(in_docpath);
	if (!doc.HasSignatures())
	{
		cout << "Doc has no signatures." << endl;
		cout << "================================================================================" << endl;
		return;
	}
	else
	{
		cout << "Doc has signatures." << endl;
	}

	
	for (FieldIterator fitr = doc.GetFieldIterator(); fitr.HasNext(); fitr.Next())
	{
		fitr.Current().IsLockedByDigitalSignature() ? cout << "==========" << endl << "Field locked by a digital signature" << endl :
			cout << "==========" << endl << "Field not locked by a digital signature" << endl;

		cout << "Field name: " << fitr.Current().GetName() << endl;
		cout << "==========" << endl;
	}

	cout << "====================" << endl << "Now iterating over digital signatures only." << endl << "====================" << endl;

	DigitalSignatureFieldIterator digsig_fitr = doc.GetDigitalSignatureFieldIterator();
	for (; digsig_fitr.HasNext(); digsig_fitr.Next())
	{
		cout << "==========" << endl;
		cout << "Field name of digital signature: " << Field(digsig_fitr.Current().GetSDFObj()).GetName() << endl;

		DigitalSignatureField digsigfield(digsig_fitr.Current());
		if (!digsigfield.HasCryptographicSignature())
		{
			cout << "Either digital signature field lacks a digital signature dictionary, "
				"or digital signature dictionary lacks a cryptographic hash entry. "
				"Digital signature field is not presently considered signed." << endl 
				<< "==========" << endl;
			continue;
		}

		UInt32 cert_count = digsigfield.GetCertCount();
		cout << "Cert count: " << cert_count << endl;
		for (UInt32 i = 0; i < cert_count; ++i)
		{
			std::vector<unsigned char> cert = digsigfield.GetCert(i);
			cout << "Cert #" << i << " size: " << cert.size() << endl;
		}

		DigitalSignatureField::SubFilterType subfilter = digsigfield.GetSubFilter();

		cout << "Subfilter type: " << (int)subfilter << endl;

		if (subfilter != DigitalSignatureField::e_ETSI_RFC3161)
		{
			cout << "Signature's signer: " << digsigfield.GetSignatureName() << endl;

			Date signing_time(digsigfield.GetSigningTime());
			if (signing_time.IsValid())
			{
				cout << "Signing day: " << (int)signing_time.day << endl;
			}

			cout << "Location: " << digsigfield.GetLocation() << endl;
			cout << "Reason: " << digsigfield.GetReason() << endl;
			cout << "Contact info: " << digsigfield.GetContactInfo() << endl;
		}
		else
		{
			cout << "SubFilter == e_ETSI_RFC3161 (DocTimeStamp; no signing info)" << endl;
		}

		cout << ((digsigfield.HasVisibleAppearance()) ? "Visible" : "Not visible") << endl;

		DigitalSignatureField::DocumentPermissions digsig_doc_perms = digsigfield.GetDocumentPermissions();
		vector<UString> locked_fields(digsigfield.GetLockedFields());
		for (vector<UString>::iterator it = locked_fields.begin(); it != locked_fields.end(); ++it)
		{
			cout << "This digital signature locks a field named: " << it->ConvertToAscii() << endl;
		}

		switch (digsig_doc_perms)
		{
		case DigitalSignatureField::e_no_changes_allowed:
			cout << "No changes to the document can be made without invalidating this digital signature." << endl;
			break;
		case DigitalSignatureField::e_formfilling_signing_allowed:
			cout << "Page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature." << endl;
			break;
		case DigitalSignatureField::e_annotating_formfilling_signing_allowed:
			cout << "Annotating, page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature." << endl;
			break;
		case DigitalSignatureField::e_unrestricted:
			cout << "Document not restricted by this digital signature." << endl;
			break;
		default:
			BASE_ASSERT(false, "Unrecognized digital signature document permission level.");
		}
		cout << "==========" << endl;
	}

	cout << "================================================================================" << endl;
}

int main(void)
{
	// Initialize PDFNetC
	PDFNet::Initialize();

#if (!USE_STD_SIGNATURE_HANDLER)
	// Initialize OpenSSL library
	CRYPTO_malloc_init();
	ERR_load_crypto_strings();
	OpenSSL_add_all_algorithms();
#endif // (!USE_STD_SIGNATURE_HANDLER)

	int ret = 0;

	//////////////////// TEST 0: 
	/* Create an approval signature field that we can sign after certifying.
	(Must be done before calling CertifyOnNextSave/SignOnNextSave/WithCustomHandler.) */
	try
	{
		PDFDoc doc(input_path + "tiger.pdf");
		PDF::DigitalSignatureField approval_signature_field = doc.CreateDigitalSignatureField("PDFTronApprovalSig");
		Annots::SignatureWidget widgetAnnotApproval = Annots::SignatureWidget::Create(doc, Rect(300, 300, 500, 200), approval_signature_field);
		Page page1 = doc.GetPage(1);
		page1.AnnotPushBack(widgetAnnotApproval);
		doc.Save(output_path + "tiger_withApprovalField_output.pdf", SDFDoc::e_remove_unused, 0);
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 1: certify a PDF.
	try
	{
		CertifyPDF(input_path + "tiger_withApprovalField.pdf",
			"PDFTronCertificationSig",
			input_path + "pdftron.pfx",
			"password",
			input_path + "pdftron.bmp",
			output_path + "tiger_withApprovalField_certified_output.pdf");
		PrintSignaturesInfo(output_path + "tiger_withApprovalField_certified_output.pdf");
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 2: sign a PDF with a certification and an unsigned signature field in it.
	try
	{
		SignPDF(input_path + "tiger_withApprovalField_certified.pdf",
			"PDFTronApprovalSig",
			input_path + "pdftron.pfx",
			"password",
			input_path + "signature.jpg",
			output_path + "tiger_withApprovalField_certified_approved_output.pdf");
		PrintSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_output.pdf");
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 3: Clear a certification from a document that is certified and has an approval signature.
	try
	{
		ClearSignature(input_path + "tiger_withApprovalField_certified_approved.pdf",
			"PDFTronCertificationSig",
			output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
		PrintSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// TEST 4: Verify a document's digital signatures.
	try
	{
		// EXPERIMENTAL. Digital signature verification is undergoing active development, but currently does not support a number of features. If we are missing a feature that is important to you, or if you have files that do not act as expected, please contact us using one of the following forms: https://www.pdftron.com/form/trial-support/ or https://www.pdftron.com/form/request/
		if (!VerifyAllAndPrint(input_path + "tiger_withApprovalField_certified_approved.pdf", input_path + "pdftron.cer"))
		{
			ret = 1;
		}
	}
	catch (Common::Exception& e)
	{
		cerr << e << endl;
		ret = 1;
	}
	catch (exception& e)
	{
		cerr << e.what() << endl;
		ret = 1;
	}
	catch (...)
	{
		cerr << "Unknown exception." << endl;
		ret = 1;
	}

	//////////////////// End of tests. ////////////////////

	if (!ret)
	{
		cout << "Tests successful." << endl  << "==========" << endl;
	}
	else
	{
		cout << "Tests FAILED!!!" << endl << "==========" << endl;
	}

	PDFNet::Terminate();

#if (!USE_STD_SIGNATURE_HANDLER)
	ERR_free_strings();
	EVP_cleanup();
#endif // (!USE_STD_SIGNATURE_HANDLER)

	return ret;
}
