// Generated code. Do not modify!
//
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
//

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

// In order to use .NET Framework's Cryptography library, define "USE_DOTNET_CRYPTO" and then add System.Security to
// references list.

using System;
using System.Collections.Generic;
using System.IO;
#if USE_DOTNET_CRYPTO
using System.Security.Cryptography;
using System.Security.Cryptography.Pkcs;
using System.Security.Cryptography.X509Certificates;
#endif // USE_DOTNET_CRYPTO

using pdftron;
using pdftron.Common;
using pdftron.PDF;
using pdftron.PDF.Annots;
using pdftron.SDF;

namespace DigitalSignaturesTestCS
{
	//////////////////// Here follows an example of how to implement a custom signature handler. //////////
#if USE_DOTNET_CRYPTO
	class DotNetCryptoSignatureHandler : SignatureHandler
	{
		private List<byte> m_data;
		private string m_signingCert;
		private string m_certPassword;

		public DotNetCryptoSignatureHandler(string signingCert, string password)
		{
			m_signingCert = signingCert;
			m_certPassword = password;
			m_data = new List<byte>();
		}

		public override void AppendData(byte[] data)
		{
			m_data.AddRange(data);
		}

		public override bool Reset()
		{
			m_data.Clear();
			return (true);
		}

		public override byte[] CreateSignature()
		{
			try {
				ContentInfo ci = new ContentInfo(m_data.ToArray());
				SignedCms sc = new SignedCms(ci, true);
				X509Certificate2 cert = new X509Certificate2(m_signingCert, m_certPassword);
				CmsSigner cs = new CmsSigner();
				cs.Certificate = cert;
				sc.ComputeSignature(cs);
				byte[] sig = sc.Encode();
				return (sig);
			}
			catch (Exception e) {
				Console.Error.WriteLine(e);
			}
			return (null);
		}

		public override string GetName()
		{
			return ("Adobe.PPKLite");
		}
	}
#endif // USE_DOTNET_CRYPTO
	////////// End of the DotNetCryptoSignatureHandler custom handler code. ////////////////////

	class Class1
	{
		static string input_path = "../../TestFiles/";
		static string output_path = "../../TestFiles/Output/";
        
		// EXPERIMENTAL. Digital signature verification is undergoing active development, but currently does not support a number of features. If we are missing a feature that is important to you, or if you have files that do not act as expected, please contact us using one of the following forms: https://www.pdftron.com/form/trial-support/ or https://www.pdftron.com/form/request/
		static bool VerifyAllAndPrint(string in_docpath, string in_public_key_file_path)
		{
            using (PDFDoc doc = new PDFDoc(in_docpath))
			{
				Console.WriteLine("==========");
				VerificationOptions opts = new VerificationOptions(VerificationOptions.SignatureVerificationSecurityLevel.e_compatibility_and_archiving);

				// Trust the public certificate we use for signing.
				byte[] trusted_cert_buf = File.ReadAllBytes(in_public_key_file_path);
				opts.AddTrustedCertificate(trusted_cert_buf);

				// Iterate over the signatures and verify all of them.
				DigitalSignatureFieldIterator digsig_fitr = doc.GetDigitalSignatureFieldIterator();
				bool verification_status = true;
				for (; digsig_fitr.HasNext(); digsig_fitr.Next())
				{
					DigitalSignatureField curr = digsig_fitr.Current();
					VerificationResult result = curr.Verify(opts);
					if (result.GetVerificationStatus())
					{
						Console.Write("Signature verified, ");
                    }
					else
					{
						Console.Write("Signature verification failed, ");
						verification_status = false;
					}
					Console.WriteLine("objnum: {0}", curr.GetSDFObj().GetObjNum());

                    switch (result.GetSignersDigestAlgorithm())
                    {
                        case DigestAlgorithm.Type.e_sha1:
                            Console.WriteLine("Digest algorithm: SHA-1");
                            break;
                        case DigestAlgorithm.Type.e_sha256:
                            Console.WriteLine("Digest algorithm: SHA-256");
                            break;
                        case DigestAlgorithm.Type.e_sha384:
                            Console.WriteLine("Digest algorithm: SHA-384");
                            break;
                        case DigestAlgorithm.Type.e_sha512:
                            Console.WriteLine("Digest algorithm: SHA-512");
                            break;
                        case DigestAlgorithm.Type.e_ripemd160:
                            Console.WriteLine("Digest algorithm: RIPEMD-160");
                            break;
                        case DigestAlgorithm.Type.e_unknown_digest_algorithm:
                            Console.WriteLine("Digest algorithm: unknown");
                            break;
                        default:
                            throw new Exception("unrecognized digest algorithm");
                    }

					Console.WriteLine("Detailed verification result: ");
					switch (result.GetDocumentStatus())
					{
						case VerificationResult.DocumentStatus.e_no_error:
							Console.WriteLine("\tNo general error to report.");
							break;
						case VerificationResult.DocumentStatus.e_corrupt_file:
							Console.WriteLine("\tSignatureHandler reported file corruption.");
							break;
						case VerificationResult.DocumentStatus.e_unsigned:
							Console.WriteLine("\tThe signature has not yet been cryptographically signed.");
							break;
						case VerificationResult.DocumentStatus.e_bad_byteranges:
							Console.WriteLine("\tSignatureHandler reports corruption in the ByteRanges in the digital signature.");
							break;
						case VerificationResult.DocumentStatus.e_corrupt_cryptographic_contents:
							Console.WriteLine("\tSignatureHandler reports corruption in the Contents of the digital signature.");
							break;
						default:
							throw new Exception("unrecognized document status");
					}
					switch (result.GetDigestStatus())
					{
						case VerificationResult.DigestStatus.e_digest_invalid:
							Console.WriteLine("\tThe digest is incorrect.");
							break;
						case VerificationResult.DigestStatus.e_digest_verified:
							Console.WriteLine("\tThe digest is correct.");
							break;
						case VerificationResult.DigestStatus.e_digest_verification_disabled:
							Console.WriteLine("\tDigest verification has been disabled.");
							break;
						case VerificationResult.DigestStatus.e_weak_digest_algorithm_but_digest_verifiable:
							Console.WriteLine("\tThe digest is correct, but the digest algorithm is weak and not secure.");
							break;
						case VerificationResult.DigestStatus.e_no_digest_status:
							Console.WriteLine("\tNo digest status to report.");
							break;
						case VerificationResult.DigestStatus.e_unsupported_encoding:
							Console.WriteLine("\tNo installed SignatureHandler was able to recognize the signature's encoding.");
							break;
						default:
							throw new Exception("unrecognized digest status");
					}
					switch (result.GetTrustStatus())
					{
						case VerificationResult.TrustStatus.e_trust_verified:
							Console.WriteLine("\tEstablished trust in signer successfully.");
							break;
						case VerificationResult.TrustStatus.e_untrusted:
							Console.WriteLine("\tTrust could not be established.");
							break;
						case VerificationResult.TrustStatus.e_trust_verification_disabled:
							Console.WriteLine("\tTrust verification has been disabled.");
							break;
						case VerificationResult.TrustStatus.e_no_trust_status:
							Console.WriteLine("\tNo trust status to report.");
							break;
						default:
							throw new Exception("unrecognized trust status");
					}
					switch (result.GetPermissionsStatus())
					{
						case VerificationResult.ModificationPermissionsStatus.e_invalidated_by_disallowed_changes:
							Console.WriteLine("\tThe document has changes that are disallowed by the signature's permissions settings.");
							break;
						case VerificationResult.ModificationPermissionsStatus.e_has_allowed_changes:
							Console.WriteLine("\tThe document has changes that are allowed by the signature's permissions settings.");
							break;
						case VerificationResult.ModificationPermissionsStatus.e_unmodified:
							Console.WriteLine("\tThe document has not been modified since it was signed.");
							break;
						case VerificationResult.ModificationPermissionsStatus.e_permissions_verification_disabled:
							Console.WriteLine("\tPermissions verification has been disabled.");
							break;
						case VerificationResult.ModificationPermissionsStatus.e_no_permissions_status:
							Console.WriteLine("\tNo permissions status to report.");
							break;
						default:
							throw new Exception("unrecognized modification permissions status");
					}

					DisallowedChange[] changes = result.GetDisallowedChanges();
					foreach (DisallowedChange it2 in changes)
					{
						Console.WriteLine("\tDisallowed change: {0}, objnum: {1}", it2.GetTypeAsString(), it2.GetObjNum());
					}

					// Get and print all the detailed trust-related results, if they are available.
					if (result.HasTrustVerificationResult())
					{
						TrustVerificationResult trust_verification_result = result.GetTrustVerificationResult();
						Console.WriteLine(trust_verification_result.WasSuccessful() ? "Trust verified." : "Trust not verifiable.");
						Console.WriteLine("Trust verification result string: {0}", trust_verification_result.GetResultString());

					   long time_of_verification = trust_verification_result.GetTimeOfTrustVerification();
						switch (trust_verification_result.GetTimeOfTrustVerificationEnum())
						{
							case VerificationOptions.TimeMode.e_current:
								Console.WriteLine("Trust verification attempted with respect to current time (as epoch time): {0}", time_of_verification);
								break;
							case VerificationOptions.TimeMode.e_signing:
								Console.WriteLine("Trust verification attempted with respect to signing time (as epoch time): {0}", time_of_verification);
								break;
							case VerificationOptions.TimeMode.e_timestamp:
								Console.WriteLine("Trust verification attempted with respect to secure embedded timestamp (as epoch time): {0}", time_of_verification);
								break;
							default:
								throw new Exception("unrecognized time enum value");
						}
					}
					else
					{
						Console.WriteLine("No detailed trust verification result available.");
					}

					Console.WriteLine("==========");
				}

                return verification_status;
            }
		}

		static void CertifyPDF(string in_docpath,
			string in_cert_field_name,
			string in_private_key_file_path,
			string in_keyfile_password,
			string in_appearance_image_path,
			string in_outpath)
		{
			Console.Out.WriteLine("================================================================================");
			Console.Out.WriteLine("Certifying PDF document");

			// Open an existing PDF
			using (PDFDoc doc = new PDFDoc(in_docpath))
			{
				Console.Out.WriteLine("PDFDoc has " + (doc.HasSignatures() ? "signatures" : "no signatures"));

				Page page1 = doc.GetPage(1);

				// Create a text field that we can lock using the field permissions feature.
				TextWidget annot1 = TextWidget.Create(doc, new Rect(50, 550, 350, 600), "asdf_test_field");
				page1.AnnotPushBack(annot1);

				/* Create a new signature form field in the PDFDoc. The name argument is optional;
				leaving it empty causes it to be auto-generated. However, you may need the name for later.
				Acrobat doesn't show digsigfield in side panel if it's without a widget. Using a
				Rect with 0 width and 0 height, or setting the NoPrint/Invisible flags makes it invisible. */
				DigitalSignatureField certification_sig_field = doc.CreateDigitalSignatureField(in_cert_field_name);
				SignatureWidget widgetAnnot = SignatureWidget.Create(doc, new Rect(0, 100, 200, 150), certification_sig_field);
				page1.AnnotPushBack(widgetAnnot);

				// (OPTIONAL) Add an appearance to the signature field.
				Image img = Image.Create(doc, in_appearance_image_path);
				widgetAnnot.CreateSignatureAppearance(img);

				// Prepare the document locking permission level. It will be applied upon document certification.
				Console.Out.WriteLine("Adding document permissions.");
				certification_sig_field.SetDocumentPermissions(DigitalSignatureField.DocumentPermissions.e_annotating_formfilling_signing_allowed);
				
				// Prepare to lock the text field that we created earlier.
				Console.Out.WriteLine("Adding field permissions.");
				string[] fields_to_lock = new string[1];
				fields_to_lock[0] = "asdf_test_field";
				certification_sig_field.SetFieldPermissions(DigitalSignatureField.FieldPermissions.e_include, fields_to_lock);

			#if USE_DOTNET_CRYPTO
				DotNetCryptoSignatureHandler sigHandler = new DotNetCryptoSignatureHandler(in_private_key_file_path, in_keyfile_password);
				SDF.SignatureHandlerId sigHandlerId = doc.AddSignatureHandler(sigHandler);
				found_approval_signature_digsig_field.CertifyOnNextSaveWithCustomHandler(sigHandlerId);
			#else
				certification_sig_field.CertifyOnNextSave(in_private_key_file_path, in_keyfile_password);
			#endif

				// (OPTIONAL) Add more information to the signature dictionary.
				certification_sig_field.SetLocation("Vancouver, BC");
				certification_sig_field.SetReason("Document certification.");
				certification_sig_field.SetContactInfo("www.pdftron.com");

				// Save the PDFDoc. Once the method below is called, PDFNet will also sign the document using the information provided.
				doc.Save(in_outpath, 0);
			}

			Console.Out.WriteLine("================================================================================");
		}

		static void SignPDF(string in_docpath,
			string in_approval_field_name,
			string in_private_key_file_path,
			string in_keyfile_password,
			string in_appearance_img_path,
			string in_outpath)
		{
			Console.Out.WriteLine("================================================================================");
			Console.Out.WriteLine("Signing PDF document");

			// Open an existing PDF
			using (PDFDoc doc = new PDFDoc(in_docpath))
			{
				// Retrieve the unsigned approval signature field.
				Field found_approval_field = doc.GetField(in_approval_field_name);
				DigitalSignatureField found_approval_signature_digsig_field = new DigitalSignatureField(found_approval_field);
				
				// (OPTIONAL) Add an appearance to the signature field.
				Image img = Image.Create(doc, in_appearance_img_path);
				SignatureWidget found_approval_signature_widget = new SignatureWidget(found_approval_field.GetSDFObj());
				found_approval_signature_widget.CreateSignatureAppearance(img);

				// Prepare the signature and signature handler for signing.
			#if USE_DOTNET_CRYPTO
				DotNetCryptoSignatureHandler sigHandler = new DotNetCryptoSignatureHandler(in_private_key_file_path, in_keyfile_password);
				SDF.SignatureHandlerId sigHandlerId = doc.AddSignatureHandler(sigHandler);
				found_approval_signature_digsig_field.SignOnNextSaveWithCustomHandler(sigHandlerId);
			#else
				found_approval_signature_digsig_field.SignOnNextSave(in_private_key_file_path, in_keyfile_password);
			#endif

				// The actual approval signing will be done during the following incremental save operation.
				doc.Save(in_outpath, SDFDoc.SaveOptions.e_incremental);
			}
			Console.Out.WriteLine("================================================================================");
		}

		static void ClearSignature(string in_docpath,
			string in_digsig_field_name,
			string in_outpath)
		{
			Console.Out.WriteLine("================================================================================");
			Console.Out.WriteLine("Clearing certification signature");

			using (PDFDoc doc = new PDFDoc(in_docpath))
			{
				DigitalSignatureField digsig = new DigitalSignatureField(doc.GetField(in_digsig_field_name));
				
				Console.Out.WriteLine("Clearing signature: " + in_digsig_field_name);
				digsig.ClearSignature();

				if (!digsig.HasCryptographicSignature())
				{
					Console.Out.WriteLine("Cryptographic signature cleared properly.");
				}

				// Save incrementally so as to not invalidate other signatures' hashes from previous saves.
				doc.Save(in_outpath, SDFDoc.SaveOptions.e_incremental);
			}

			Console.Out.WriteLine("================================================================================");
		}

		static void PrintSignaturesInfo(string in_docpath)
		{
			Console.Out.WriteLine("================================================================================");
			Console.Out.WriteLine("Reading and printing digital signature information");

			using (PDFDoc doc = new PDFDoc(in_docpath))
			{
				if (!doc.HasSignatures())
				{
					Console.Out.WriteLine("Doc has no signatures.");
					Console.Out.WriteLine("================================================================================");
					return;
				}
				else
				{
					Console.Out.WriteLine("Doc has signatures.");
				}

				
				for (FieldIterator fitr = doc.GetFieldIterator(); fitr.HasNext(); fitr.Next())
				{
					if (fitr.Current().IsLockedByDigitalSignature())
					{
						Console.Out.WriteLine("==========\nField locked by a digital signature");
					}
					else
					{
						Console.Out.WriteLine("==========\nField not locked by a digital signature");
					}

					Console.Out.WriteLine("Field name: " + fitr.Current().GetName());
					Console.Out.WriteLine("==========");
				}

				Console.Out.WriteLine("====================\nNow iterating over digital signatures only.\n====================");

				DigitalSignatureFieldIterator digsig_fitr = doc.GetDigitalSignatureFieldIterator();
				for (; digsig_fitr.HasNext(); digsig_fitr.Next())
				{
					Console.Out.WriteLine("==========");
					Console.Out.WriteLine("Field name of digital signature: " + new Field(digsig_fitr.Current().GetSDFObj()).GetName());

					DigitalSignatureField digsigfield = digsig_fitr.Current();
					if (!digsigfield.HasCryptographicSignature())
					{
						Console.Out.WriteLine("Either digital signature field lacks a digital signature dictionary, " +
							"or digital signature dictionary lacks a cryptographic hash entry. " +
							"Digital signature field is not presently considered signed.\n" +
							"==========");
						continue;
					}

					int cert_count = digsigfield.GetCertCount();
					Console.Out.WriteLine("Cert count: " + cert_count);
					for (int i = 0; i < cert_count; ++i)
					{
						byte[] cert = digsigfield.GetCert(i);
						Console.Out.WriteLine("Cert #" + i + " size: " + cert.Length);
					}

					DigitalSignatureField.SubFilterType subfilter = digsigfield.GetSubFilter();

					Console.Out.WriteLine("Subfilter type: " + (int)subfilter);

					if (subfilter != DigitalSignatureField.SubFilterType.e_ETSI_RFC3161)
					{
						Console.Out.WriteLine("Signature's signer: " + digsigfield.GetSignatureName());

						Date signing_time = digsigfield.GetSigningTime();
						if (signing_time.IsValid())
						{
							Console.Out.WriteLine("Signing day: " + (int)signing_time.day);
						}

						Console.Out.WriteLine("Location: " + digsigfield.GetLocation());
						Console.Out.WriteLine("Reason: " + digsigfield.GetReason());
						Console.Out.WriteLine("Contact info: " + digsigfield.GetContactInfo());
					}
					else
					{
						Console.Out.WriteLine("SubFilter == e_ETSI_RFC3161 (DocTimeStamp; no signing info)\n");
					}

					Console.Out.WriteLine(((digsigfield.HasVisibleAppearance()) ? "Visible" : "Not visible"));

					DigitalSignatureField.DocumentPermissions digsig_doc_perms = digsigfield.GetDocumentPermissions();
					string[] locked_fields = digsigfield.GetLockedFields();
					foreach (string field_name in locked_fields)
					{
						Console.Out.WriteLine("This digital signature locks a field named: " + field_name);
					}

					switch (digsig_doc_perms)
					{
					case DigitalSignatureField.DocumentPermissions.e_no_changes_allowed:
						Console.Out.WriteLine("No changes to the document can be made without invalidating this digital signature.");
						break;
					case DigitalSignatureField.DocumentPermissions.e_formfilling_signing_allowed:
						Console.Out.WriteLine("Page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature.");
						break;
					case DigitalSignatureField.DocumentPermissions.e_annotating_formfilling_signing_allowed:
						Console.Out.WriteLine("Annotating, page template instantiation, form filling, and signing digital signatures are allowed without invalidating this digital signature.");
						break;
					case DigitalSignatureField.DocumentPermissions.e_unrestricted:
						Console.Out.WriteLine("Document not restricted by this digital signature.");
						break;
					default:
						throw new Exception("Unrecognized digital signature document permission level.");
					}
					Console.Out.WriteLine("==========");
				}
			}

			Console.Out.WriteLine("================================================================================");
		}
		
		private static pdftron.PDFNetLoader pdfNetLoader = pdftron.PDFNetLoader.Instance();
		static Class1() {}

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			// Initialize PDFNetC
			PDFNet.Initialize();

			bool result = true;

			//////////////////// TEST 0: 
			/* Create an approval signature field that we can sign after certifying.
			(Must be done before calling CertifyOnNextSave/SignOnNextSave/WithCustomHandler.) */
			try
			{
				using (PDFDoc doc = new PDFDoc(input_path + "tiger.pdf")) 
				{
					DigitalSignatureField approval_signature_field = doc.CreateDigitalSignatureField("PDFTronApprovalSig");
					SignatureWidget widgetAnnotApproval = SignatureWidget.Create(doc, new Rect(300, 300, 500, 200), approval_signature_field);
					Page page1 = doc.GetPage(1);
					page1.AnnotPushBack(widgetAnnotApproval);
					doc.Save(output_path + "tiger_withApprovalField_output.pdf", SDFDoc.SaveOptions.e_remove_unused);
				}				
			}
			catch (Exception e)
			{
				Console.Error.WriteLine(e);
				result = false;
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
			catch (Exception e)
			{
				Console.Error.WriteLine(e);
				result = false;
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
			catch (Exception e)
			{
				Console.Error.WriteLine(e);
				result = false;
			}

			//////////////////// TEST 3: Clear a certification from a document that is certified and has an approval signature.
			try
			{
				ClearSignature(input_path + "tiger_withApprovalField_certified_approved.pdf",
					"PDFTronCertificationSig",
					output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
				PrintSignaturesInfo(output_path + "tiger_withApprovalField_certified_approved_certcleared_output.pdf");
			}
			catch (Exception e)
			{
				Console.Error.WriteLine(e);
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
				Console.Error.WriteLine(e);
				result = false;
			}

			//////////////////// End of tests. ////////////////////

			if (result)
			{
				Console.Out.WriteLine("Tests successful.\n==========");
			}
			else
			{
				Console.Out.WriteLine("Tests FAILED!!!\n==========");
			}
		}
	}
}
