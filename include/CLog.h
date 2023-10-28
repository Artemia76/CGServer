#ifndef CLOG_H
#define CLOG_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/config.h>
#include <wx/filename.h>
#include <wx/textfile.h>
#include <wx/timer.h>
#include <wx/log.h>

#include <iostream>
#include <vector>

/*
 * Class CLog - used as a handle for a log file.
 * @path: path to the log file.
 * @handle: log file handle.
 * @max_size: maximum file size before making a backup of the log file and truncating it.
 * @level: lowest log level required for log entries.
 * @log_to_file: if set then log entries will be saved to file.
 * @log_to_console: if set then log entries will be printed on the console.
 * @error: is set when a file operation fails.
 * @flush_timer: used to keep track of when the next flush should occur.
 * @list: strings together the log structs in a linked list.
 */

typedef enum
{
	LOG_MAX_LINE,
	LOG_FLUSH_INTERVAL,
	LOG_LEVEL,
	LOG_FILE,
	LOG_CONSOLE
} LogOption;

class CLogManager;

class CLog : public wxEvtHandler, public wxLog
{
	friend class CLogManager;
public:
	bool		GetOption(LogOption pOption, int& pValue);
	bool		SetOption(LogOption pOption, int pValue);
	bool		Append(const wxString& pText, int pLevel=1);
	void		Flush();

protected:
				CLog (const wxString& pName, bool pLogToFile, bool pLogToConsole);
				~CLog ();
	void		OnTimer(wxTimerEvent& event);
virtual void	DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info);
virtual void 	DoLogTextAtLevel (wxLogLevel level, const wxString &msg);
virtual void 	DoLogText (const wxString &msg);

	wxDECLARE_EVENT_TABLE();
private:
	wxConfigBase* pConfig;
	wxString	Name;
	wxFileName	FileName;
	wxTextFile* File;
	int			MaxLine;
	int			Level;
	int			LogToFile;
	int			LogToConsole;
	int			FlushInterval;
	bool		Error;
	wxTimer*	Timer;

	bool		MoveToOld ();
	void		WriteConf ();
};

typedef std::vector<CLog*> VLog;

class CLogManager
{
public:
			CLogManager();
			~CLogManager ();
	VLog	Logs; // Tableau d'objet de log;

	CLog*	Add (const wxString& pName, bool pLogToFile = true, bool pLogToConsole=true);
	bool	Remove (CLog* pLog);
	bool	Remove (const wxString& pName);
	CLog*	Get (const wxString& pName);
	void	RemoveAll ();
	void	FlushAll();
};
#endif
