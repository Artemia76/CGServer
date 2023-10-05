// *****************************************************************************
// *                                                                           *
// *                            CLASS CMAINAPP                                 *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 18/12/2011                                                           *
// *   CopyRight (c)Neophile 2005-2011                                         *
// *                                                                           *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CMAINAPP_H
#define CMAINAPP_H

#include "CServiceApp.h"
//#include <wx/intl.h>
#include <wx/fileconf.h>

#include "CLog.h"
#include "Ctrlaw.h"

//#define NB_MAX_LNG 3

// language data
//static const wxLanguage langIds[] =
//{
//    wxLANGUAGE_ENGLISH,
//    wxLANGUAGE_FRENCH,
//    wxLANGUAGE_SPANISH
//};

// note that it makes no sense to translate these strings, they are
// shown before we set the locale anyhow
//const wxString langNames[] =
//{
//    _T("English"),
//    _T("French"),
//    _T("Spanish")
//};

//wxCOMPILE_TIME_ASSERT( WXSIZEOF(langNames) == WXSIZEOF(langIds),
//                       LangArraysMismatch );

class CMainApp : public CServiceApp
{
	private:
		wxFileConfig*		pConfig;
		CCtrlAw*			CtrlAw;
        int					NbBot;
	protected:
		virtual bool		Init();
        virtual void		Exiting();
		//wxLocale			m_locale;
};

#endif
