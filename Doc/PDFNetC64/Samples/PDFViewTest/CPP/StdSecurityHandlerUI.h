
#ifndef   H_UISecurityHandler
#define   H_UISecurityHandler


#include "EditSecurityHandler.h"
#include "AuthorizeDlg.h"


class SecurityHandlerUI : public SDF::SecurityHandler
{
public:

	SecurityHandlerUI (int key_len, int enc_code, void* custom_data)
		: SDF::SecurityHandler("Standard", key_len, enc_code), m_custom_data(custom_data)
	{
	}

	SecurityHandlerUI (const SecurityHandlerUI& s) 
		: SDF::SecurityHandler(s), m_custom_data(0)
	{
	}

	virtual ~SecurityHandlerUI () {
	}

	void* m_custom_data;

	virtual bool GetAuthorizationData (Permission p) 
	{
		CString msg; 

		switch (p) 
		{
		case SecurityHandler::e_owner:
			msg = "Owner (master) password is required to modify\ndocument security.";
			break;
		case SecurityHandler::e_doc_open:
		default:
			msg = "Document is protected.";
		}

		CAuthorize dlg(_T("Password"), msg);

		if (dlg.DoModal() == IDOK)
		{
#ifdef _UNICODE
			InitPassword(UString((LPCWSTR) dlg.m_password).ConvertToAscii().c_str());
#else
			InitPassword((LPCSTR) dlg.m_password);
#endif
			return true;
		}
		
		return false;
	}

	virtual bool EditSecurityData(SDF::SDFDoc& doc) 
	{
		std::string userpass = GetUserPassword();
		std::string masterpass = GetMasterPassword();

		CEditSecurityHandler dlg(userpass, masterpass);
		
		dlg.m_rev_num = GetRevisionNumber();

		if (IsUserPasswordRequired())  
		{
			dlg.m_user_pass_req = true;
			
			// Put a dummy password
			dlg.m_user_pass = "üüüüüüüüüü";
		}

		if (IsMasterPasswordRequired()) 
		{
			dlg.m_owner_pass_req = true;

			// Put a dummy password
			dlg.m_owner_pass = "üüüüüüüüüü";
		}

		// Permissions
		dlg.m_print          = Authorize(SDF::SecurityHandler::e_print);
		dlg.m_doc_modify     = Authorize(SDF::SecurityHandler::e_doc_modify);
		dlg.m_extract_content= Authorize(SDF::SecurityHandler::e_extract_content);
		dlg.m_mod_annot      = Authorize(SDF::SecurityHandler::e_mod_annot);
		dlg.m_fill_forms     = Authorize(SDF::SecurityHandler::e_fill_forms);
		dlg.m_access_support = Authorize(SDF::SecurityHandler::e_access_support);
		dlg.m_assemble_doc   = Authorize(SDF::SecurityHandler::e_assemble_doc);
		dlg.m_high_rez_print = Authorize(SDF::SecurityHandler::e_print_high);

		if (dlg.DoModal() == IDOK)
		{
			ChangeRevisionNumber (dlg.m_rev_num); 
			
			// Get user & owner passwords
			int len = dlg.m_user_pass.GetLength();

			if (dlg.m_user_pass_req == TRUE && len > 0 && dlg.m_user_pass != _T("üüüüüüüüüü"))
			{
#ifdef _UNICODE
				userpass = UString((LPCWSTR) dlg.m_user_pass).ConvertToAscii();
#else
				userpass = dlg.m_user_pass;
#endif
				ChangeUserPassword(userpass.c_str());
			}
			else if (len == 0) 
			{
				userpass.resize(0);
				ChangeUserPassword(userpass.c_str());
			}			

			len = dlg.m_owner_pass.GetLength();
			if (dlg.m_owner_pass_req == TRUE && len > 0  && dlg.m_owner_pass != _T("üüüüüüüüüü"))
			{
#ifdef _UNICODE
				masterpass =  UString((LPCWSTR) dlg.m_owner_pass).ConvertToAscii();
#else
				masterpass = dlg.m_owner_pass;
#endif
				ChangeMasterPassword(masterpass.c_str());
			}
			else if (len == 0) 
			{
				masterpass.resize(0); 
				ChangeMasterPassword(masterpass.c_str());
			}

			// Set Permissions
			SetPermission (e_print, (dlg.m_print == TRUE));
			SetPermission (e_doc_modify, (dlg.m_doc_modify == TRUE));
			SetPermission (e_extract_content, (dlg.m_extract_content == TRUE));
			SetPermission (e_mod_annot, (dlg.m_mod_annot == TRUE));
			SetPermission (e_fill_forms , (dlg.m_fill_forms == TRUE));
			SetPermission (e_access_support, (dlg.m_access_support == TRUE));
			SetPermission (e_assemble_doc, (dlg.m_assemble_doc == TRUE));
			SetPermission (e_print_high, (dlg.m_high_rez_print == TRUE));
			return true;
		}

		return false;
	}

	virtual void AuthorizeFailed() 
	{
		AfxGetApp()->m_pMainWnd->MessageBox(_T("Password is incorrect. Make sure the Caps Lock key is not on."), _T("Authorization Problem"), MB_ICONINFORMATION | MB_OK);
	}

	SecurityHandlerUI (const SecurityHandlerUI& s, TRN_SecurityHandler base) 
		: SecurityHandler(base, true, s.m_derived_procs)
	{
	}

	virtual SDF::SecurityHandler* Clone(TRN_SecurityHandler base) const 
	{
		return new SecurityHandlerUI(*this, base);
	}

	// SecurityHandlerUI factory method
	static TRN_SecurityHandler Create(const char* name, int key_len, int enc_code, void* custom_data)
	{		
		SecurityHandlerUI* ret = new SecurityHandlerUI(key_len, enc_code, custom_data);
		// Explicitly specify which methods are overloaded.
		ret->SetDerived(
			has_CloneProc |   // Clone - must be implemented in every derived class.
			has_AuthFailedProc |
			has_GetAuthDataProc |
			has_EditSecurDataProc);
		return (TRN_SecurityHandler) ret->mp_handler;
	}
};



#endif