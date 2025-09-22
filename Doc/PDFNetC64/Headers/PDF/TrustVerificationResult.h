//---------------------------------------------------------------------------------------
// Copyright (c) 2001-2020 by PDFTron Systems Inc. All Rights Reserved.
// Consult legal.txt regarding legal and license information.
//---------------------------------------------------------------------------------------

#ifndef PDFTRON_H_CPPPDFTrustVerificationResult
#define PDFTRON_H_CPPPDFTrustVerificationResult
#include <C/PDF/TRN_TrustVerificationResult.h>

#include <Common/BasicTypes.h>
#include <Common/UString.h>
#include <PDF/VerificationOptions.h>
#include <PDF/PDFDoc.h>

namespace pdftron { namespace PDF { 


/**
 * The class TrustVerificationResult.
 * The detailed result of a trust verification step of a verification
 * operation performed on a digital signature.
 */
class TrustVerificationResult
{
public:
	TrustVerificationResult(const TrustVerificationResult& other);
	TrustVerificationResult(TRN_TrustVerificationResult impl);
	TrustVerificationResult& operator= (const TrustVerificationResult& other);
	~TrustVerificationResult();
	
	void Destroy();

	//methods:
	
	/**
	 * Retrieves the trust verification status.
	 * 
	 * @return A boolean representing whether or not the trust verification operation was successful. Whether trust-related warnings are treated as errors or not depend on the VerificationOptions used for the verification operation.
	 */
	bool WasSuccessful() const;
	
	/**
	 * Retrieves a string representation of the details of the trust verification status.
	 * 
	 * @return A string.
	 */
	UString GetResultString() const;
	
	/**
	 * Retrieves the reference-time used for trust verification as an epoch time.
	 * 
	 * @return An integral value representing an epoch time.
	 */
	Int64 GetTimeOfTrustVerification() const;
	
	/**
	 * Retrieves the type of reference-time used for trust verification.
	 * 
	 * @return An enumerated value representing the type of reference-time used for trust verification.
	 */
	VerificationOptions::TimeMode GetTimeOfTrustVerificationEnum() const;

#ifdef SWIG
	TrustVerificationResult();
#endif

#ifndef SWIGHIDDEN
	TRN_TrustVerificationResult m_impl;
#endif

private:

#ifndef SWIGHIDDEN
	mutable bool m_owner; 
#endif
};

#include <Impl/TrustVerificationResult.inl>
} //end pdftron
} //end PDF


#endif //PDFTRON_H_CPPPDFTrustVerificationResult
