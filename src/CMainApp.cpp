#include "CMainApp.h"
#include <wx/stdpaths.h>

// On implemente la classe d'application principale (démarrage du prog)

IMPLEMENT_APP_NO_MAIN(CMainApp);

CMainApp::CMainApp() : CtrlAw(Diffusion), IRCGateway(Diffusion)
{

}

bool CMainApp::Init()
{
	int rc;
	//long lng=-1;
	wxString ConfFile = conf_path_name + wxFILE_SEP_PATH + AppName + _T(".ini");
	pConfig = new wxFileConfig
		(
			AppName,
			wxEmptyString,
			ConfFile
		);
	wxConfigBase::Set(pConfig);
	// Définition de la langue utilisée
	//pConfig->Read (_T("/misc/language"),&lng,-1);
	//if ((lng < 0) || (lng >= NB_MAX_LNG))
    //{
    //    lng = 1;
    //}
	//if (( lng >= 0 ) || (lng < NB_MAX_LNG))
    //{
        // don't use wxLOCALE_LOAD_DEFAULT flag so that Init() doesn't return
        // false just because it failed to load wxstd catalog
    //    if ( !m_locale.Init(langIds[lng]) )
    //    {
    //        wxLogError(_T("This language is not supported by the system."));
    //        return false;
    //    }
    //    pConfig->Write (_T("/misc/language"),lng);
    //    pConfig->Flush ();
    //}
    //else return false;

    // normally this wouldn't be necessary as the catalog files would be found
    // in the default locations, but when the program is not installed the
    // catalogs are in the build directory where we wouldn't find them by
    // default
    //wxLocale::AddCatalogLookupPathPrefix(_T("./lng/"));

    // Initialize the catalogs we'll be using
	//m_locale.AddCatalog(AppName);

    // this catalog is installed in standard location on Linux systems and
	// shows that you may make use of the standard message catalogs as well
	//
	// if it's not installed on your system, it is just silently ignored
#ifdef __linux__
	{
		wxLogNull noLog;
		//m_locale.AddCatalog(_T("fileutils"));
	}
#endif
	pConfig->SetPath(_T("/General"));
	pConfig->Read(_T("NbBot"),&NbBot, 1);
	bool LogFlag;
	pConfig->Read(_T("Log"),&LogFlag, true);
	pConfig->Write(_T("NbBot"),NbBot);
	pConfig->Write(_T("Log"),LogFlag);
	pConfig->Flush ();
	CLog* Log = LogManager.Add(_T("LogGen"), LogFlag, !IsService());
	if(Log == nullptr)
	{
		return false;
	}
	wxLog::SetActiveTarget(Log);
	wxLogMessage(AppName + _(" Started"));
	if ((rc=CtrlAw.Init(true, NbBot)))
	{
		wxLogMessage(_("Fail to start Aw Controller. Exiting..."));
		return false;
	}
    IRCGateway.Init(true);
	return true;
}

// On quitte proprement le service

void CMainApp::Exiting()
{
	wxLogMessage(_("Exiting ") + AppName + _("... Bye Bye!"));
    IRCGateway.Init(false);
	CtrlAw.Init(false);
	LogManager.FlushAll();
	wxLog::SetActiveTarget(NULL);
	LogManager.RemoveAll();
}
