// *****************************************************************************
// *                                                                           *
// *                         CONTROLEUR AW BODY                                *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 22/04/2005                                                           *
// *   CopyRight (c)Neophile 2005                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                        DEFINITION DES MEMBRES                             *
// *                                                                           *
// *****************************************************************************

#include "Ctrlaw.h"

#include <wx/datetime.h>
#include <wx/tokenzr.h>

#include <Aw.h>

#include "global.h"

wxBEGIN_EVENT_TABLE(CCtrlAw, wxEvtHandler)
	EVT_TIMER (HEARTBEAT, CCtrlAw::On_HeartBeat)
wxEND_EVENT_TABLE()

CCtrlAw* CCtrlAw::PtCCtrlAw = nullptr;

//------------------------------------------------------------------------------
// Constructeur

CCtrlAw::CCtrlAw (CDiffusion &pDiffusion) : Diffusion(pDiffusion)
{
	AwInit=false;
    ServerMode = true;
    pConfig=wxConfigBase::Get();
	AHeure=0;
	Heart = new wxTimer (this, HEARTBEAT);
	ShdEject=false;
    PtCCtrlAw = this;
}

//------------------------------------------------------------------------------
// Destructeur

CCtrlAw::~CCtrlAw()
{
}

void CCtrlAw::SetMode (CTRLAW_MODE Mode)
{
	if (Mode==CTRLAW_SERVER) ServerMode = true;
	else ServerMode = false;
}

//------------------------------------------------------------------------------
// Initialisation SDK, Instance AW, Table des Events

int CCtrlAw::Init (bool flag, size_t NbBot)
{
	int rc;
    if (NbBot<1) NbBot=1;
	if ((flag) && (!AwInit))
	{
		if ((rc=aw_init (AW_BUILD))!=0)
		{
            wxLogMessage(_("Unable to init the SDK, Reason :")+ CBot::GetRCString(rc));
			return rc;
		}
		// Installe les events AW
		aw_event_set (AW_EVENT_CHAT, CCtrlAw::On_Chat);
		aw_event_set (AW_EVENT_AVATAR_ADD, CCtrlAw::On_Avatar_Add);
		aw_event_set (AW_EVENT_AVATAR_CHANGE, CCtrlAw::On_Avatar_Change);
		aw_event_set (AW_EVENT_AVATAR_DELETE, CCtrlAw::On_Avatar_Delete);
		aw_event_set (AW_EVENT_WORLD_DISCONNECT, CCtrlAw::On_World_Disconnect);
		aw_event_set (AW_EVENT_UNIVERSE_DISCONNECT, CCtrlAw::On_Universe_Disconnect);
		aw_event_set (AW_EVENT_WORLD_ATTRIBUTES, CCtrlAw::On_World_Attributes);

		// Installe les callbacks AW
		aw_callback_set (AW_CALLBACK_LOGIN, CCtrlAw::On_Login);
		aw_callback_set (AW_CALLBACK_ENTER, CCtrlAw::On_Enter);
		aw_callback_set (AW_CALLBACK_ADDRESS, CCtrlAw::On_Address);
        aw_callback_set (AW_CALLBACK_CITIZEN_ATTRIBUTES, CCtrlAw::On_Citizen_Attributes);

		AwInit=true;
		for (size_t i=0 ; i<NbBot ; i++)
		{
			Bots.push_back(new CBotCG(Diffusion));
			Bots.back()->Charge();
			if (Bots.back()->CGConAuto)
			{
				Bots.back()->Global=true;
				if (Bots.back()->CGRecoEna)
				{
					Bots.back()->CGRecoCnt=0;
					Bots.back()->CGRetente=Bots.back()->CGRecoDelay;
					Bots.back()->ModeReco=true;
				}
				Bots.back()->Connect();
			}
		}
		Heart->Start(Base_Temps);
		return 0;
	}
	else if((!flag) && (AwInit))
	{
		Heart->Stop();
		for (auto Bot : Bots)
		{
			if ( Bot->SetInstance())
			{
				Bot->Connection(false);
			}
            Bot->Sauve();
			delete (Bot);
		}
		Bots.clear();
        AwInit=false;
		aw_term();
		return 0;
	}
	return 1000;
}

//------------------------------------------------------------------------------
// Methode Statique Privées des evenements AW
// Mettez ici votre code à traiter


void CCtrlAw::On_Admin_Worlds_Delete(void)
{
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Admin_Worlds_Info(void)
{
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Avatar_Add(void)
{
	wxString Name,Message=_T("");
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	CUser User;
	User.Session=aw_int(AW_AVATAR_SESSION);
	User.Name=
	#if AW_BUILD > 77
		aw_string(AW_AVATAR_NAME);
	#else
		wxString(aw_string(AW_AVATAR_NAME), wxConvISO8859_1);
	#endif
	User.Citizen=aw_int(AW_AVATAR_CITIZEN);
	User.Privilege=aw_int(AW_AVATAR_PRIVILEGE);
	User.X=aw_int(AW_AVATAR_X);
	User.Y=aw_int(AW_AVATAR_Y);
	User.Z=aw_int(AW_AVATAR_Z);
	User.Yaw=aw_int(AW_AVATAR_YAW);
	Robot->AddUser(User);
	Robot->Kling=true;
	Robot->NKling=User.Name;
	int id,modo;
	id=Robot->DBase->Identifie(User.Name, User.Citizen);
	if (Robot->IsUserModerator(User) || Robot->IsUserEminent(User))
	{
		Robot->TellList (User.Session);
	}
	Robot->DBase->AddFreq(id);
	Robot->Mess_Bot(User.Name, 1);
	Robot->Mess_Bot(User.Name, 7,_T(""), User.Session);
	aw_address (User.Session);
	if (Robot->CGON)
	{
		PtCCtrlAw->Diffusion.Post(Message, aw_instance(), User.Name, Robot->Nom, 5);
	}
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Avatar_Change(void)
{
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	CUser User;
	User.Session=aw_int(AW_AVATAR_SESSION);
	User.Name=
	#if AW_BUILD > 77
		aw_string(AW_AVATAR_NAME);
	#else
		wxString(aw_string(AW_AVATAR_NAME), wxConvISO8859_1);
	#endif
	if (Robot->GetUserName(User.Session)!=User.Name)
	{
		Robot->SetUserName(User.Session,User.Name);
	}
	Robot->SetUserCoord
					(
						User.Session,
						aw_int(AW_AVATAR_X),
						aw_int(AW_AVATAR_Y),
						aw_int(AW_AVATAR_Z),
                        aw_int(AW_AVATAR_YAW)
					);
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Avatar_Delete(void)
{
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	wxString Message=_T("");
	CUser User;
	User.Session=aw_int(AW_AVATAR_SESSION);
	User.Name=
	#if AW_BUILD > 77
		aw_string(AW_AVATAR_NAME);
	#else
		wxString(aw_string(AW_AVATAR_NAME), wxConvISO8859_1);
	#endif
	if (Robot->DelUser(User.Session)<0)
	{
        Message << _("Avatar_Delete_Error: Name=") + User.Name + _(", Session=") + wxString::Format(_T("%i"),User.Session);
		wxLogMessage(Message);
	}
	Robot->Kloug=true;
	Robot->NKloug=User.Name;
	Robot->Mess_Bot(User.Name, 2);
	if (Robot->CGON)
	{
		PtCCtrlAw->Diffusion.Post(Message,aw_instance(), User.Name, Robot->Nom, 6);
	}
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Avatar_Click(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Cell_Begin(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Cell_End(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Cell_Object(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Chat(void)
{
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	int Session=aw_int(AW_CHAT_SESSION);
	wxString Message =
	#if AW_BUILD > 77
		aw_string(AW_CHAT_MESSAGE);
	#else
		wxString(aw_string(AW_CHAT_MESSAGE), wxConvISO8859_1);
	#endif
	int Type=aw_int(AW_CHAT_TYPE);
    wxString Nom = Robot->GetUserName(Session);
	int Citoyen = Robot->GetUserCitizen(Session);
	int Privilege = Robot->GetUserPrivilege(Session);
	wxString Original=Robot->Analyse(Nom, Session, Citoyen, Message, Type, false, _T(""), Privilege);
	Message=Original;
	if (Robot->CGON)
	{
		if (Robot->ExtBride)
		{
			int Distance=Robot->Distance(Session, 0, true);
			if (Distance < Robot->ExtDistance) PtCCtrlAw->Diffusion.Post (Message, aw_instance(), Nom, Robot->Nom, 0);
		}
		else PtCCtrlAw->Diffusion.Post (Message, aw_instance(), Nom, Robot->Nom, 0);
	}
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Console_Message(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Object_Add(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Object_Click(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Object_Delete(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Object_select(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Teleport(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Universe_Attributes(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Url(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_World_Attributes(void)
{
    CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	Robot->PSList=_T("");
	Robot->ModoList=_T("");
	Robot->EminentList=_T("");
	wxStringTokenizer Droits(
	#if AW_BUILD > 77
		aw_string(AW_WORLD_PUBLIC_SPEAKER_RIGHT)
	#else
		wxString(aw_string(AW_WORLD_PUBLIC_SPEAKER_RIGHT), wxConvISO8859_1)
	#endif
    ,_T(", "));

	while ( Droits.HasMoreTokens() )
	{
		Robot->PSList   << _T("[")
					    << Droits.GetNextToken()
						<< _T("]");
	}
	Droits.SetString (
	#if AW_BUILD > 77
		aw_string(AW_WORLD_EJECT_RIGHT)
	#else
		wxString(aw_string(AW_WORLD_EJECT_RIGHT), wxConvISO8859_1)
	#endif
	,_T(", "));

	while ( Droits.HasMoreTokens() )
	{
		Robot->ModoList << _T("[")
								<< Droits.GetNextToken()
								<< _T("]");
	}
	Droits.SetString (
	#if AW_BUILD > 77
		aw_string(AW_WORLD_EMINENT_DOMAIN_RIGHT)
	#else
		wxString(aw_string(AW_WORLD_EMINENT_DOMAIN_RIGHT), wxConvISO8859_1)
	#endif
	,_T(", "));
	while ( Droits.HasMoreTokens() )
	{
		Robot->EminentList << _T("[")
								<< Droits.GetNextToken()
								<< _T("]");
	}
	if ((!aw_bool(AW_WORLD_DISABLE_CHAT)) && (!Robot->BlockCG))
	{
		aw_bool_set(AW_WORLD_DISABLE_CHAT,true);
		aw_world_attributes_change ();
	}
}

//------------------------------------------------------------------------------

void CCtrlAw::On_World_Disconnect(void)
{
    CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	Robot->PerteMonde=true;
	Robot->Kling=false;
	Robot->Kloug=false;
	Robot->PSList=_T("");
	Robot->ModoList=_T("");
	Robot->EminentList=_T("");
}

//------------------------------------------------------------------------------

void CCtrlAw::On_World_Info(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Terrain_Begin(void)
{

}

void CCtrlAw::On_Terrain_Changed(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Terrain_Data(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Terrain_End(void)
{

}

//------------------------------------------------------------------------------

void CCtrlAw::On_Universe_Disconnect(void)
{
    CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	Robot->PerteUniv=true;
	Robot->Kling=false;
	Robot->Kloug=false;
}

//------------------------------------------------------------------------------
// Gestion des callbacks

void CCtrlAw::On_Login (int rc)
{
    CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	Robot->Login_CB(rc);
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Enter (int rc)
{
    CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	Robot->Enter_CB(rc);
	if (!rc) 	Robot->OwnerList.Printf(_T("[%d]"),Robot->Citoyen);
}


//------------------------------------------------------------------------------

void CCtrlAw::On_Address (int rc)
{
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	Robot->ConMess (0,_T("On_Address"),51,153,51,true);
	wxString Message;
	if (rc)
    {
        Message << _("Unable to find IP address, Reason: ") << rc;
        return;
    }
	int Session=aw_int(AW_AVATAR_SESSION);
	int IP=aw_int(AW_AVATAR_ADDRESS);
	Robot->SetUserIP (Session ,IP);
	Message=Robot->GetUserName(Session) + _(" enter with IP : ") + Robot->GetUserIP(Session);

	for (CUser User : Robot->Users)
	{
		int id=Robot->DBase->Identifie (User.Name, User.Citizen);
		if (id >= 0)
		{
			if (Robot->IsUserModerator(User) || Robot->IsUserPublicSpeaker(User))
			{
				Robot->ConMess (Session,Message,51,153,51,true);
			}
		}
	}
	wxLogMessage (Message);
}

//------------------------------------------------------------------------------

void CCtrlAw::On_Citizen_Attributes (int rc)
{
	CBotCG* Robot=PtCCtrlAw->GetBotInst(aw_instance());
	wxString Resultat;
	Resultat	<< _("Result of Citizen Search:")
				<< _("\nNumber : ") << aw_int(AW_CITIZEN_NUMBER)
				<< _("\nName : ") << aw_string (AW_CITIZEN_NAME);
	Robot->ConMess (Robot->SessionReq, Resultat,45,145,244, true);
	Robot->SessionReq=0;
}

//------------------------------------------------------------------------------

CBotCG* CCtrlAw::GetBot (unsigned int num)
{
	if ( (num >= Bots.size()) || (!AwInit) ) return nullptr;
	return Bots[num];
}

//------------------------------------------------------------------------------

CBotCG* CCtrlAw::GetBotInst(void* Instance)
{
    for (auto Bot : Bots)
	{
		if (Bot->GetInstance()==Instance)
		{
			return (Bot);
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------

void CCtrlAw::On_HeartBeat (wxTimerEvent& WXUNUSED(event))
{
	int i,Type;
	bool njour=false;
	wxString Message, Nom, Client;
	Horloge = wxDateTime::Now();
	if ((AHeure != Horloge.GetHour()) && (AHeure==23)) njour=true;
	AHeure=Horloge.GetHour();
	aw_wait(0);
    for (auto Bot : Bots)
	{
		Bot->SetInstance();
		Bot->Update();
		if (ShdEject) Bot->Eject ();
		if (Bot->IsOnWorld()) Bot->Ciel->MAJ ();
		if (njour) wxLogMessage(_("A new day happen"));
	}
	if (ShdEject) ShdEject=false;
	if (CBotCG::Ejecte)
	{
		ShdEject=true;
		CBotCG::Ejecte=false;
	}
}
