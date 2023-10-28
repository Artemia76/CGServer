#include "CLog.h"
#include "CServiceApp.h"

const int CLOG_TIMER = wxNewId();

wxBEGIN_EVENT_TABLE(CLog, wxEvtHandler)
EVT_TIMER(CLOG_TIMER, CLog::OnTimer)
wxEND_EVENT_TABLE()


/*
 * CLog::CLog - creates a log object to be used for accessing a log file.
 * @pName: name of the log object.
 * @pLogToFile: if set then the log to file.
 * @pLogToConsole: if set then the log to console.
 */

CLog::CLog (const wxString& pName, bool pLogToFile, bool pLogToConsole)
{
	Name=pName;
	wxString Path;
    pConfig =  wxConfigBase::Get();
	//pConfig->SetPath(_T("/") + Name);
	//pConfig->Read(_T("FileName"), &Path,_T("./") + Name + _T(".log") );
    FileName = wxFileName(log_path_name + wxFILE_SEP_PATH + Name + _T(".log") );
	Error=false;
	File=new wxTextFile (FileName.GetFullPath());
	if (File->Exists())
	{
		if (!File->Open ())
		{
			delete File;
			Error=true;
		}
	}
	else if (!File->Create ())
	{
		delete File;
		Error=true;
	}
	File->Clear();
	pConfig->Read(_T("MaxLine"), &MaxLine, 10000);
	pConfig->Read(_T("Level"),&Level, 1);
	pConfig->Read(_T("FlushInterval"),&FlushInterval, 60);

	FlushInterval *= 1000; // Convert to millisec
	Timer = new wxTimer (this,CLOG_TIMER);
	if (FlushInterval > 0)
	{
		Timer->Start(FlushInterval);
	}
	LogToFile = pLogToFile;
	LogToConsole = pLogToConsole;
	WriteConf();
}

/*
 * log_destroy - closes log file and free up memory.
 * @log: log file to perform this operation on.
 */
CLog::~CLog()
{
	WriteConf();
	File->Write ();
	File->Close ();
	delete File;
}

/*
 * CLog::Append - appends a log entry.
 * @pText: string that should be added to the log file.
 * @pLevel: minimum log level required for this entry to get logged.
 */
bool CLog::Append(const wxString& pText, int pLevel)
{

	if (pLevel > Level)
	{
		return false;
	}
	if (LogToConsole)
	{
		std::cout << pText;
		std::cout << std::endl;
	}
	if (Error)
	{
		return true;
	}
	if (LogToFile)
	{
		if (File->GetLineCount() > MaxLine)
		{
			if (MoveToOld())
			{
				Error = true;
				return true;
			}
		}
		File->AddLine (pText);
		if (!Timer->IsRunning())
		{
			File->Write();
		}
	}
	return false;
}

/*
 * CLog::GetOption - gets a logging option.
 * @pOption: option to return the value of.
 * @pValue: pointer to where the value will be stored.
 */
bool CLog::GetOption(LogOption pOption, int& pValue)
{
	switch (pOption)
	{
		case LOG_MAX_LINE:
			pValue = MaxLine;
			return false;
		case LOG_FLUSH_INTERVAL:
			pValue = Timer->GetInterval();
			return false;
		case LOG_LEVEL:
			pValue = Level;
			return false;
		case LOG_FILE:
			pValue = LogToFile;
			return false;
		case LOG_CONSOLE:
			pValue = LogToConsole;
			return false;
	}
	return true;
}

/*
 * log_set_option - sets a logging option.
 * @pOption: option to set the value of.
 * @pValue: new value for the option.
 */
bool CLog::SetOption(LogOption pOption, int pValue)
{
	switch (pOption)
	{
		case LOG_MAX_LINE:
			MaxLine = pValue;
			WriteConf();
			return false;
		case LOG_FLUSH_INTERVAL:
			if (pValue > 0)
			{
				Timer->Start(pValue);
			}
			else
			{
				Timer->Stop();
			}
			WriteConf();
			return false;
		case LOG_LEVEL:
			Level = pValue;
			WriteConf();
			return false;
		case LOG_FILE:
			LogToFile = pValue;
			WriteConf();
			return false;
		case LOG_CONSOLE:
			LogToConsole = pValue;
			WriteConf();
			return false;
	}
	return true;
}

/*
 * CLog::Flush - flushes file buffers for the log file if a flush is scheduled.
 */
void CLog::Flush()
{
	if (Error)
	{
		return;
	}
	File->Write ();
}

/*
 * CLog::OnTimer - Called by timer to flush log.
 */
void CLog::OnTimer(wxTimerEvent& WXUNUSED(event))
{
	Flush();
}

/*
 * CLog::MoveToOld - Appends ".old" to existing log file and opens a new one.
 */
bool CLog::MoveToOld()
{
	wxFileName old=FileName;
	File->Close();
	old.SetExt(_T(".old"));

	if (!wxRenameFile(FileName.GetFullPath(), old.GetFullPath(), true))
	{
		wxLogMessage(_("remove failed"));
		Error=true;
		return true;
	}
	if (!File->Create (FileName.GetFullPath()))
	{
		wxLogMessage(_("open failed"));
		Error=true;
		return true;
	}
	return false;
}

void CLog::DoLogRecord(wxLogLevel WXUNUSED(level), const wxString &msg, const wxLogRecordInfo& WXUNUSED(info))
{
	Append (msg);
}

void CLog::DoLogTextAtLevel (wxLogLevel WXUNUSED(level), const wxString &msg)
{
	Append (msg);
}

void CLog::DoLogText (const wxString &msg)
{
	Append (msg);
}

/*
 * CLog::WriteConf - Write Options to config file.
 */

 void CLog::WriteConf()
 {
	pConfig->SetPath(_T("/") + Name);
 	pConfig->Write(_T("FileName"),FileName.GetFullPath());
 	pConfig->Write(_T("MaxLine"), MaxLine);
	pConfig->Read(_T("Level"),Level);
	pConfig->Read(_T("FlushInterval"),Timer->GetInterval());
	pConfig->Flush();
 }

//*******************************************************************************************


/*
 * log_flush_all - calls log_flush for all the log files.
 */
CLogManager::CLogManager()
{

}

/*
 * log_flush_all - calls log_flush for all the log files.
 */
CLogManager::~CLogManager()
{

}

/*
 * CLogManager::Add - Create a new Log Object and return its pointer.
 */
CLog* CLogManager::Add (const wxString& pName, bool pLogToFile, bool pLogToConsole)
{
	CLog* Log= new CLog(pName,pLogToFile,pLogToConsole );
	if (Log->Error) return NULL;
	Logs.push_back(Log);
	return Log;
}

/*
 * CLogManager::FlushAll - calls log_flush for all the log files.
 */
void CLogManager::FlushAll()
{
	for (auto Log : Logs)
	{
		Log->Flush();
	}
}

/*
 * CLogManager::Get - return Log pointer with name provided.
 * Otherway, it return null pointer
 * @Name: The name of the Log object.
 */
CLog* CLogManager::Get (const wxString& pName)
{
	for (auto Log : Logs)
	{
		if (Log->Name==pName) return Log;
	}
	return NULL;
}

/*
 * CLogManager::Remove - search and destroy Log Object.
 * @pLog: The pointer of the Log object.
 */
bool CLogManager::Remove (CLog* pLog)
{
	for (VLog::iterator it = Logs.begin(); it != Logs.end(); ++it)
	{
		if ((*it)==pLog)
		{
			delete (*it);
			Logs.erase(it);
			return false;
		}
	}
	return true;
}

/*
 * CLogManager::Remove - search and destroy Log Object.
 * @pName: The name of the object.
 */
bool CLogManager::Remove (const wxString& pName)
{
	CLog* Log=Get(pName);
	if (!Log) return true;
	return Remove(Log);
}

/*
 * CLogManager::Remove - search and destroy Log Object.
 * @pName: The name of the object.
 */
void CLogManager::RemoveAll ()
{
	for (auto Log : Logs)
	{
		delete (Log);
	}
	Logs.clear();
}
