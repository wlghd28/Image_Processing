#ifdef SWIG
inline TrustVerificationResult::TrustVerificationResult()
	: m_impl(NULL), m_owner(false)
{
}
#endif

inline TrustVerificationResult::~TrustVerificationResult()
{
	Destroy();
}

inline TrustVerificationResult::TrustVerificationResult(const TrustVerificationResult& other)
	: m_impl(other.m_impl)
	, m_owner(false)
{
	if(other.m_owner) {
		other.m_owner = false;
		m_owner = true;
	}
}

inline void TrustVerificationResult::Destroy()
{
	if(m_owner)
	{
		DREX(m_impl, TRN_TrustVerificationResultDestroy(m_impl));
		m_owner = false;
	}
}

inline TrustVerificationResult& pdftron::PDF::TrustVerificationResult::operator= (const TrustVerificationResult& other)
{
	Destroy();

	m_impl = other.m_impl;

	if(other.m_owner) {
		other.m_owner = false;
		m_owner = true;
	}

	return *this;
}

inline TrustVerificationResult::TrustVerificationResult(TRN_TrustVerificationResult impl)
	: m_impl(impl)
	, m_owner(true)
{}
inline bool TrustVerificationResult::WasSuccessful() const
{
	TRN_Bool result = 0;
	REX(TRN_TrustVerificationResultWasSuccessful((TRN_TrustVerificationResult)m_impl, &result));
	return result != 0;
}

inline UString TrustVerificationResult::GetResultString() const
{
	UString result;
	REX(TRN_TrustVerificationResultGetResultString((TRN_TrustVerificationResult)m_impl, (TRN_UString*)&result.mp_impl));
	return result;
}

inline Int64 TrustVerificationResult::GetTimeOfTrustVerification() const
{
	TRN_Int64 result;
	REX(TRN_TrustVerificationResultGetTimeOfTrustVerification((TRN_TrustVerificationResult)m_impl, &result));
	return result;
}

inline VerificationOptions::TimeMode TrustVerificationResult::GetTimeOfTrustVerificationEnum() const
{
	VerificationOptions::TimeMode result;
	REX(TRN_TrustVerificationResultGetTimeOfTrustVerificationEnum((TRN_TrustVerificationResult)m_impl, (enum TRN_VerificationOptions_TimeMode*) &result));
	return result;
}
