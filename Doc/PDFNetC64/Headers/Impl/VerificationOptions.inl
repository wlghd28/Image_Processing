inline VerificationOptions::~VerificationOptions()
{
	Destroy();
}

inline VerificationOptions::VerificationOptions(const VerificationOptions& other)
	: m_impl(other.m_impl)
	, m_owner(false)
{
	if(other.m_owner) {
		other.m_owner = false;
		m_owner = true;
	}
}

inline void VerificationOptions::Destroy()
{
	if(m_owner)
	{
		DREX(m_impl, TRN_VerificationOptionsDestroy(m_impl));
		m_owner = false;
	}
}

inline VerificationOptions& pdftron::PDF::VerificationOptions::operator= (const VerificationOptions& other)
{
	Destroy();

	m_impl = other.m_impl;

	if(other.m_owner) {
		other.m_owner = false;
		m_owner = true;
	}

	return *this;
}

inline VerificationOptions::VerificationOptions(TRN_VerificationOptions impl)
	: m_impl(impl)
	, m_owner(true)
{}

inline  VerificationOptions::VerificationOptions(SecurityLevel in_level)
	:m_impl(NULL), m_owner(true)
{
	REX(TRN_VerificationOptionsCreate((enum TRN_VerificationOptions_SecurityLevel) in_level, (TRN_VerificationOptions*)&m_impl));
}

inline void VerificationOptions::AddTrustedCertificate(const std::vector<unsigned char> in_binary_DER_certificate)
{
	REX(TRN_VerificationOptionsAddTrustedCertificate(m_impl, &in_binary_DER_certificate.front(), in_binary_DER_certificate.size()));
}

inline void VerificationOptions::RemoveTrustedCertificate(const std::vector<unsigned char> in_binary_DER_certificate)
{
	REX(TRN_VerificationOptionsRemoveTrustedCertificate(m_impl, &in_binary_DER_certificate.front(), in_binary_DER_certificate.size()));
}

#ifndef SWIG
inline void VerificationOptions::AddTrustedCertificate(const unsigned char* in_binary_DER_certificate, size_t in_buf_size)
{
	REX(TRN_VerificationOptionsAddTrustedCertificate(m_impl, in_binary_DER_certificate, in_buf_size));
}

inline void VerificationOptions::RemoveTrustedCertificate(const unsigned char* in_binary_DER_certificate, size_t in_buf_size)
{
	REX(TRN_VerificationOptionsRemoveTrustedCertificate(m_impl, in_binary_DER_certificate, in_buf_size));
}
#endif

inline void VerificationOptions::EnableModificationVerification(bool in_on_or_off)
{
	REX(TRN_VerificationOptionsEnableModificationVerification(m_impl, in_on_or_off));
}

inline void VerificationOptions::EnableDigestVerification(bool in_on_or_off)
{
	REX(TRN_VerificationOptionsEnableDigestVerification(m_impl, in_on_or_off));
}

inline void VerificationOptions::EnableTrustVerification(bool in_on_or_off)
{
	REX(TRN_VerificationOptionsEnableTrustVerification(m_impl, in_on_or_off));
}
