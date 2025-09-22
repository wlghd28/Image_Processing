//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#ifndef PDFTRON_H_CPPPDFVerificationOptions
#define PDFTRON_H_CPPPDFVerificationOptions
#include <C/PDF/TRN_VerificationOptions.h>

#include <Common/BasicTypes.h>
#include <Common/UString.h>
#include <PDF/PDFDoc.h>

namespace pdftron { namespace PDF { 


/**
 * The class VerificationOptions.
 * Options pertaining to digital signature verification.
 */
class VerificationOptions
{
public:
	//enums:
	enum SecurityLevel {
		e_compatibility_and_archiving = 0,
		e_maximum = 1
	};
	enum TimeMode {
		e_signing = 0,
		e_timestamp = 1,
		e_current = 2
	};
	VerificationOptions(const VerificationOptions& other);
	VerificationOptions(TRN_VerificationOptions impl);
	VerificationOptions& operator= (const VerificationOptions& other);
	~VerificationOptions();
	
	void Destroy();

	//methods:
	
	/**
	 * Constructs a set of options for digital signature verification.
	 * 
	 * @param level -- the general security level to use. Sets other security settings internally.
	 */
	VerificationOptions(SecurityLevel level);

	/**
	 * Adds a certificate to the store of trusted certificates inside this options object.
	 *
	 * @param in_binary_DER_certificate -- a buffer consisting of the data of a public key certificate encoded in binary DER format
	 *
	*/
	void AddTrustedCertificate(const std::vector<unsigned char> in_binary_DER_certificate);
#ifndef SWIG
	void AddTrustedCertificate(const unsigned char* in_binary_DER_certificate, size_t in_buf_size);
#endif

	/**
	 * Removes a certificate from the store of trusted certificates inside this options object.
	 *
	 * @param in_binary_DER_certificate -- a buffer consisting of the data of a public key certificate encoded in binary DER format
	 *
	*/
	void RemoveTrustedCertificate(const std::vector<unsigned char> in_binary_DER_certificate);
#ifndef SWIG
	void RemoveTrustedCertificate(const unsigned char* in_binary_DER_certificate, size_t in_buf_size);
#endif

	/**
	 * Sets a flag that can turn on or off the verification of the permissibility of any modifications made to the document after the signing of the digital signature being verified, in terms of the document and field permissions specified by the digital signature being verified.
	 * 
	 * @param in_on_or_off -- A boolean.
	 */
	void EnableModificationVerification(bool in_on_or_off);
	
	/**
	 * Sets a flag that can turn on or off the verification of the digest (cryptographic hash) component of a digital signature.
	 * 
	 * @param in_on_or_off -- A boolean.
	 */
	void EnableDigestVerification(bool in_on_or_off);
	
	/**
	 * Sets a flag that can turn on or off the verification of the trust status of a digital signature.
	 * 
	 * @param in_on_or_off -- A boolean.
	 */
	void EnableTrustVerification(bool in_on_or_off);

#ifndef SWIGHIDDEN
	TRN_VerificationOptions m_impl;
#endif

private:

#ifndef SWIGHIDDEN
	mutable bool m_owner; 
#endif
};

#include <Impl/VerificationOptions.inl>
} //end pdftron
} //end PDF


#endif //PDFTRON_H_CPPPDFVerificationOptions
