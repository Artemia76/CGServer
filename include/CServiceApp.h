// *****************************************************************************
// *                                                                           *
// *                            CLASS CSERVICEAPP                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 18/12/2013                                                           *
// *   CopyRight (c)Neophile 2005-2014                                         *
// *                                                                           *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CSERVICEAPP_H
#define CSERVICEAPP_H

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
	#include <wx/wx.h>
#endif

#ifdef __linux__
extern char* pid_file_name;
extern int pid_fd;
// Service loop
extern bool Running;
#else
// Windows Service definition
static SERVICE_STATUS svcStatus;
static SERVICE_STATUS_HANDLE svcStatusHnd;
void WINAPI svcStart(DWORD argc, LPTSTR *argv);

static SC_HANDLE m_globalSCM;
static HANDLE svcStopEvent = INVALID_HANDLE_VALUE;
void WINAPI svcHandler(DWORD opcode);
#endif

extern wxString AppName;
extern wxString log_path_name;
extern wxString conf_path_name;

class CServiceApp : public wxAppConsole
{
	private:
		int					m_ExitCode;
		bool				RunAsService;
#ifndef __linux__
        bool				openSCM(void);
        void				closeSCM(void);
	protected:
        bool				isInstalled(void);
        void				install(void);
        void				uninstall(void);

#endif
	protected:
		virtual bool		Init()=0;
        virtual void		Exiting()=0;
	public:
		bool				OnInit();
		int					OnExit();
		bool				IsService(){return RunAsService;}
};

#endif //CSERVICEAPP_H
