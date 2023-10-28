#include "CServiceApp.h"
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <getopt.h>
wxString AppName;
wxString log_path_name;
wxString conf_path_name;

#ifdef	__linux__
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif

static bool ExitResult = EXIT_SUCCESS;

#ifdef __linux__

char *pid_file_name = NULL;
int pid_fd = -1;
bool Running = false;

void daemonize()
{
    pid_t pid =0;
	int fd;

	/* Fork off the parent process */
	pid = fork();

	/* An error occurred */
    if (pid < 0){
        exit(EXIT_FAILURE);
    }

    /* Success: Let the parent terminate */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

	/* On success: The child process becomes session leader */
	if (setsid() < 0) {
		exit(EXIT_FAILURE);
	}

	/* Ignore signal sent from child to parent process */
	signal(SIGCHLD, SIG_IGN);

	/* Fork off for the second time*/
	pid = fork();

	/* An error occurred */
	if (pid < 0) {
		exit(EXIT_FAILURE);
	}

	/* Success: Let the parent terminate */
	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	/* Set new file permissions */
	umask(0);

	/* Change the working directory to the root directory */
	/* or another appropriated directory */
	chdir("/");

	/* Close all open file descriptors */
	for (fd = sysconf(_SC_OPEN_MAX); fd > 0; fd--) {
		close(fd);
	}

	/* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
	stdin = fopen("/dev/null", "r");
	stdout = fopen("/dev/null", "w+");
	stderr = fopen("/dev/null", "w+");

	/* Try to write PID of daemon to lockfile */
	if (pid_file_name != NULL)
	{
		char str[256];
		pid_fd = open(pid_file_name, O_RDWR|O_CREAT, 0640);
		if (pid_fd < 0) {
			/* Can't open lockfile */
			exit(EXIT_FAILURE);
		}
		if (lockf(pid_fd, F_TLOCK, 0) < 0) {
			/* Can't lock file */
			exit(EXIT_FAILURE);
		}
		/* Get current PID */
		sprintf(str, "%d\n", getpid());
		/* Write PID to lockfile */
		write(pid_fd, str, strlen(str));
	}
}

void handle_signal(int sig)
{
    switch (sig) {
		case SIGTERM: // Exiting ask by system service manager
		case SIGINT: // Normal exiting by CTRL-C
			ExitResult=EXIT_SUCCESS;
			Running=false;
			wxExit();
			break;
    }
	//If we ask exiting
	if (!Running)
	{
		/* Unlock and close lockfile */
		if (pid_fd != -1) {
			lockf(pid_fd, F_ULOCK, 0);
			close(pid_fd);
		}
		/* Try to delete lockfile */
		if (pid_file_name != NULL) {
			unlink(pid_file_name);
		}
		/* Reset signal handling to default behavior */
		signal(SIGINT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
	}
}

/**
 * \brief Print help for this application
 */
void print_help(void)
{
	printf("\n Usage: %s [OPTIONS]\n\n", AppName.mb_str());
	printf("  Options:\n");
	printf("   -h --help                 Print this help\n");
	printf("   -c --conf_path path       Configuration path\n");
	printf("   -l --log_path  path   	 Log path\n");
	printf("   -d --daemon               Daemonize this application\n");
	printf("   -p --pid_file  filename   PID file used by daemonized app\n");
	printf("\n");
}

int main ( int argc, char *argv[] )
{
	bool start_daemonized=false;

	static struct option long_options[] = {
		{"conf_path", required_argument, 0, 'c'},
		{"log_path", required_argument, 0, 'l'},
		{"help", no_argument, 0, 'h'},
		{"daemon", no_argument, 0, 'd'},
		{"pid_file", required_argument, 0, 'p'},
		{NULL, 0, 0, 0}
	};
	wxFileName FileName = wxFileName(argv[0]);
	AppName = FileName.GetName();
	int value, option_index = 0;

	/* Try to process all command line arguments */
	while ((value = getopt_long(argc, argv, "c:l:t:p:dh", long_options, &option_index)) != -1) {
		switch (value) {
			case 'c':
				conf_path_name = strdup(optarg);
				break;
			case 'l':
				log_path_name = strdup(optarg);
				break;
			case 'p':
				pid_file_name = strdup(optarg);
				break;
			case 'd':
				start_daemonized = true;
				break;
			case 'h':
				print_help();
				return EXIT_SUCCESS;
			case '?':
				print_help();
				return EXIT_FAILURE;
			default:
				break;
		}
	}
	/* When daemonizing is requested at command line. */
	if (start_daemonized) {
		/* It is also possible to use glibc function deamon()
		 * at this point, but it is useful to customize your daemon. */
		daemonize();
		Running=true;
	}
	/* Daemon will handle two signals */
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	return wxEntry (argc, argv);
}

#else

HINSTANCE g_hInstance;
HINSTANCE g_hPrevInstance;
wxCmdLineArgType g_lpCmdLine;
int g_nCmdShow;

//Point d'entrée windows en mode non service

extern "C" int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    wxCmdLineArgType lpCmdLine,
    int nCmdShow)
{
	WCHAR szFileName[MAX_PATH];
	GetModuleFileName(hInstance, szFileName, MAX_PATH);
	wxFileName FileName = wxFileName(szFileName);
	AppName = FileName.GetName();
    if(strstr(lpCmdLine, "--svc")==NULL)
	{
        return wxEntry(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }
	else
	{
		static SERVICE_TABLE_ENTRY   dispatchTable[] =
		{
			{szFileName, &svcStart},
			{NULL, NULL}
		};
        g_hInstance = hInstance;
        g_hPrevInstance = hPrevInstance;
        g_lpCmdLine = lpCmdLine;
        g_nCmdShow = nCmdShow;
		if(StartServiceCtrlDispatcher(dispatchTable) == FALSE)
		{
			return GetLastError();
		}
    }
	return 0;
}

bool CServiceApp::isInstalled(void)
{
    bool found;
    openSCM();
    SC_HANDLE svc = OpenService(
        m_globalSCM,
		AppName,
        SERVICE_ALL_ACCESS);
    if(svc)
	{
        found=true;
        CloseServiceHandle(svc);
    }
	else found=false;
    closeSCM();
    return found;
}

bool CServiceApp::openSCM(void)
{
    m_globalSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (NULL == m_globalSCM)
	{
		wxMessageDialog(NULL, wxString::Format("openSCM error %d", GetLastError()), AppName, wxOK).ShowModal();
        return FALSE;
    } else
        return TRUE;
}

void CServiceApp::closeSCM(void)
{
    CloseServiceHandle(m_globalSCM);
}

void CServiceApp::install(void)
{
    openSCM();
    SC_HANDLE schService = CreateService(
        m_globalSCM,
        GetAppName(),        // name of service
		AppName,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, // remove interactive if not using gui
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        wxString::Format("\"%s\" --svc", ::wxStandardPaths::Get().GetExecutablePath()),
        NULL,
        NULL,
        NULL,
        NULL,
        NULL);
    if(schService==NULL)
		wxMessageDialog(NULL, wxString::Format("createSvc error %d", GetLastError()), AppName, wxOK).ShowModal();
    else
		wxMessageDialog(NULL, AppName + _(" installed."), AppName, wxOK).ShowModal();
    closeSCM();
}

void CServiceApp::uninstall(void) {
    openSCM();
    SC_HANDLE svc = OpenService(
        m_globalSCM,
		AppName,
        SERVICE_ALL_ACCESS);
    SERVICE_STATUS status;
    ControlService(svc, SERVICE_CONTROL_STOP, &status);
    if(!DeleteService(svc))
		wxMessageDialog(NULL, wxString::Format("deleteSvc error %d", GetLastError()), AppName, wxOK).ShowModal();
    else
		wxMessageDialog(NULL, AppName + _(" uninstalled."), AppName, wxOK).ShowModal();
    CloseServiceHandle(svc);
    closeSCM();
}

void WINAPI svcStart(DWORD WXUNUSED(argc), LPTSTR* WXUNUSED(argv))
{
	svcStatusHnd = RegisterServiceCtrlHandler(AppName, svcHandler);
	if(!svcStatusHnd)
	{
		return;
	}
	ZeroMemory(&svcStatus, sizeof (svcStatus));
	svcStatus.dwServiceType        = SERVICE_WIN32;
	svcStatus.dwCurrentState       = SERVICE_START_PENDING;
	svcStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP;
	svcStatus.dwWin32ExitCode      = 0;
	svcStatus.dwServiceSpecificExitCode = 0;
	svcStatus.dwCurrentState = SERVICE_RUNNING;
	svcStatus.dwCheckPoint         = 0;
	svcStatus.dwWaitHint           = 0;

	if(SetServiceStatus(svcStatusHnd, &svcStatus))
		wxEntry(g_hInstance, g_hPrevInstance, g_lpCmdLine, g_nCmdShow);
	return;
}

void WINAPI svcHandler (DWORD opcode)
{
	DWORD status;

	switch(opcode)
	{
		case SERVICE_CONTROL_PAUSE:
			svcStatus.dwCurrentState = SERVICE_PAUSED;
			break;

		case SERVICE_CONTROL_CONTINUE:
			svcStatus.dwCurrentState = SERVICE_RUNNING;
			break;
		case SERVICE_CONTROL_STOP:
			svcStatus.dwWin32ExitCode = 0;
			svcStatus.dwCurrentState = SERVICE_STOP_PENDING;
			svcStatus.dwCheckPoint    = 0;
			svcStatus.dwWaitHint = 5;
			if(!SetServiceStatus(svcStatusHnd, &svcStatus))
			{
				status = GetLastError();
				MessageBox(NULL, _T("svcStatusStop error"), AppName, MB_OK);
			}
			// This will signal the worker thread to start shutting down
			wxTheApp->OnExit();
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
	}
	if(!SetServiceStatus(svcStatusHnd, &svcStatus))
	{
		status = GetLastError();
		// error msg
	}
	return;
}

#endif

bool CServiceApp::OnInit()
{
	wxString PathEXE = ::wxStandardPaths::Get().GetExecutablePath();
#ifndef __linux__
	wxSetWorkingDirectory (PathEXE.BeforeLast(_T('\\')));
    wxString usage = _("usage:\n") + AppName + T(" --install or --uninstall to manage windows service. --console to run as console program");
    if(argc!=2)
	{
		wxPrintf(usage);
		return false;
	}
    wxString command = argv[1];
	if ((command==_T("--svc")) || (command==_T("--console")))
	{
		if (command==_T("--svc")) RunAsService=true;
		else RunAsService=false;
        return Init();
	}
	else if(command==_T("--install"))
	{
		if(isInstalled())
		{
			wxMessageDialog(NULL, AppName + _(" is already installed"), AppName, wxOK).ShowModal();
 		}
		else
		{
 			install();
		}
	}
	else if(command==_T("--uninstall"))
	{
		if(!isInstalled())
		{
			wxMessageDialog(NULL, AppName + _(" was not installed."), AppName, wxOK).ShowModal();
		}
		else
		{
			uninstall();
		}
	}
	else
	{
		wxMessageDialog(NULL, usage, AppName, wxOK).ShowModal();
	}
	return false;
#else

    RunAsService = Running;

	return Init();
#endif
}

// On quitte proprement le service

int CServiceApp::OnExit()
{
	Exiting();
	int exit_code = wxAppConsole::OnExit();
	Yield();
#ifndef __linux__
	if(RunAsService)
	{
		svcStatus.dwWin32ExitCode = 0;
		svcStatus.dwCurrentState = SERVICE_STOPPED;
		svcStatus.dwCheckPoint = 0;
		svcStatus.dwWaitHint = 0;
		SetServiceStatus(svcStatusHnd, &svcStatus);
	}
#endif
	return exit_code;
}
