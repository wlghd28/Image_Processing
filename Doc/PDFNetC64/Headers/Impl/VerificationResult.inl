inline VerificationResult::~VerificationResult()
{
	Destroy();
}

inline VerificationResult::VerificationResult(const VerificationResult& other)
	: m_impl(other.m_impl)
	, m_owner(false)
{
	if(other.m_owner) {
		other.m_owner = false;
		m_owner = true;
	}
}

inline void VerificationResult::Destroy()
{
	if(m_owner)
	{
		DREX(m_impl, TRN_VerificationResultDestroy(m_impl));
		m_owner = false;
	}
}

inline VerificationResult& pdftron::PDF::VerificationResult::operator= (const VerificationResult& other)
{
	Destroy();

	m_impl = other.m_impl;

	if(other.m_owner) {
		other.m_owner = false;
		m_owner = true;
	}

	return *this;
}

#ifdef SWIG
inline VerificationResult::VerificationResult()
	: m_impl(0)
	, m_owner(false)
{}
#endif

inline VerificationResult::VerificationResult(TRN_VerificationResult impl)
	: m_impl(impl)
	, m_owner(true)
{}

inline bool VerificationResult::GetVerificationStatus() const
{
	TRN_Bool result = 0;
	REX(TRN_VerificationResultGetVerificationStatus((TRN_VerificationResult)m_impl, &result));
	return result != 0;
}

inline VerificationResult::DocumentStatus VerificationResult::GetDocumentStatus() const
{
	DocumentStatus result;
	REX(TRN_VerificationResultGetDocumentStatus((TRN_VerificationResult)m_impl, (enum TRN_VerificationResult_DocumentStatus*) &result));
	return result;
}

inline VerificationResult::DigestStatus VerificationResult::GetDigestStatus() const
{
	DigestStatus result;
	REX(TRN_VerificationResultGetDigestStatus((TRN_VerificationResult)m_impl, (enum TRN_VerificationResult_DigestStatus*) &result));
	return result;
}

inline VerificationResult::TrustStatus VerificationResult::GetTrustStatus() const
{
	TrustStatus result;
	REX(TRN_VerificationResultGetTrustStatus((TRN_VerificationResult)m_impl, (enum TRN_VerificationResult_TrustStatus*) &result));
	return result;
}

inline VerificationResult::ModificationPermissionsStatus VerificationResult::GetPermissionsStatus() const
{
	ModificationPermissionsStatus result;
	REX(TRN_VerificationResultGetPermissionsStatus((TRN_VerificationResult)m_impl, (enum TRN_VerificationResult_ModificationPermissionsStatus*) &result));
	return result;
}


inline bool pdftron::PDF::VerificationResult::HasTrustVerificationResult() const
{
	TRN_Bool result = 0;
	REX(TRN_VerificationResultHasTrustVerificationResult((TRN_VerificationResult)m_impl, &result));
	return result != 0;
}

inline TrustVerificationResult VerificationResult::GetTrustVerificationResult() const
{
	TRN_TrustVerificationResult result;
	REX(TRN_VerificationResultGetTrustVerificationResult((TRN_VerificationResult)m_impl, &result));
	return TrustVerificationResult(result);
}

inline std::vector<pdftron::PDF::DisallowedChange> pdftron::PDF::VerificationResult::GetDisallowedChanges() const
{
	// Broadly similar to PDFDraw::GetSeparationBitmaps and DigitalSignatureField::GetLockedFields implementations
	TRN_Vector changes_vec = NULL;
	REX(TRN_VerificationResultGetDisallowedChanges(m_impl, (TRN_Vector*)&changes_vec));

	TRN_UInt32 vec_size;
	TRN_VectorGetSize(changes_vec, &vec_size);

	std::vector<DisallowedChange> result;
	for (TRN_UInt32 i = 0; i < vec_size; i++)
	{
		TRN_DisallowedChange current_disallowed_change_ptr = NULL;
		TRN_VectorGetAt(changes_vec, i, (void**)&current_disallowed_change_ptr);
		result.push_back(DisallowedChange(current_disallowed_change_ptr));
	}

	TRN_VectorDestroyKeepContents(changes_vec);
	return result;
}

inline DigestAlgorithm::Type VerificationResult::GetSignersDigestAlgorithm() const
{
	DigestAlgorithm::Type result;
	REX(TRN_VerificationResultGetSignersDigestAlgorithm((TRN_VerificationResult)m_impl, (TRN_DigestAlgorithm_Type*) &result));
	return result;
}

