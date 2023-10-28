// *****************************************************************************
// *                                                                           *
// *                              CLASS DE BOT                                 *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 16/05/2005                                                           *
// *   CopyRight (c)Neophile 2005-2006                                         *
// *   Modifié le 11/11/2006                                                   *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                          DEFINITION DES MEMBRES                           *
// *                                                                           *
// *****************************************************************************

#include "CBot.h"

#include <Aw.h>

// Event du Bot
wxBEGIN_EVENT_TABLE(CBot, wxEvtHandler)
	EVT_TIMER  (CG_RECO,   CBot::OnCGRecoEvent)
wxEND_EVENT_TABLE()

//------------------------------------------------------------------------------

int CBot::NBot = 0;

//------------------------------------------------------------------------------
// Contructeur

CBot::CBot ()
{
	pConfig = wxConfigBase::Get();
	ID=NBot;
// Déclaration des instances
	CGRecoTimer = new wxTimer(this, CG_RECO);
	PassPriv = CPassPriv::Create();

// Initialisation des variables internes

	NBot++;
	Global=true;
	On_Universe=false;
	On_World=false;
	Visible=false;
	CGRecoCnt=0;
	PerteUniv=false;
	PerteMonde=false;
	Instance=0;
	DemCon=false;
	ConEC=false;
	EntEC=false;
	DemDeco=false;
	ModeReco=false;
	Visible=false;
	Pending_CB = 0;
}

//------------------------------------------------------------------------------
// Destructeur

CBot::~CBot ()
{
	if (On_World || On_Universe) Connection(false);
}

//------------------------------------------------------------------------------
// Methode de connection

void CBot::Connect()
{
	if ((!ConEC) && (!EntEC) && (!DemCon) && (!DemDeco) && (!On_Universe) && (!On_World)) DemCon=true;
}

//------------------------------------------------------------------------------
// Methode de Déconnection

void CBot::Deconnect()
{
	if ((!ConEC) && (!EntEC) && (!DemCon) && (!DemDeco) && ((On_Universe) || (On_World))) DemDeco=true;
}

//------------------------------------------------------------------------------
// Methode d'entrée/sortie d'univers

void CBot::Connection(bool flag)
{
// Variables Internes
	wxString Message, s;
	int rc;
	if (flag)
	{
        if (Instance)
		{
			wxLogMessage (_("Instance is already initialized."));
			return;
		}
#if AW_BUILD>77
		if ((rc=aw_create(Univers, Port, &Instance))!=0)
#else
        if ((rc=aw_create(Univers.mb_str(), Port, &Instance))!=0)
#endif
		{
			wxLogMessage (_("Unable to create instance, Reason : ") + GetRCString(rc));
			DemCon=false;
            Instance = 0;
			if (ModeReco) Tentative();
		}
		else
		{
			wxLogMessage (_("Instance Initialized."));
			aw_int_set (AW_LOGIN_OWNER, Citoyen);
#if AW_BUILD>77
            aw_string_set (AW_LOGIN_PRIVILEGE_PASSWORD,PassWord);
            aw_string_set (AW_LOGIN_APPLICATION, _T("Chat Global"));
            aw_string_set (AW_LOGIN_NAME,Nom);
#else
			aw_string_set (AW_LOGIN_PRIVILEGE_PASSWORD,PassWord.mb_str());
			aw_string_set (AW_LOGIN_APPLICATION, "Chat Global");
			aw_string_set (AW_LOGIN_NAME,Nom.mb_str());
#endif
			aw_login();
			++Pending_CB;
			ConEC=true;
			CGRecoTimer->Start(15000,wxTIMER_ONE_SHOT);
		}
	}
	else
	{
        if (!Instance)
		{
			wxLogMessage (_("Instance is already empty."));
			return;
		}
		Users.clear();
	    aw_bool_set(AW_WORLD_DISABLE_CHAT,false);
	    aw_world_attributes_change ();
		aw_destroy();
		Instance=0;
		wxLogMessage (_("Disconnected from Universe "));
		On_Universe=false;
		On_World=false;
		DemDeco=false;
	}
}

//------------------------------------------------------------------------------
// Retour Callback d'une demande de connection

void CBot::Login_CB(int rc)
{
	--Pending_CB;
	ConEC=false;
	if (CGRecoTimer->IsRunning()) CGRecoTimer->Stop();
	if (rc)
	{
		DemCon=false;
		wxLogMessage (_("Unable to join the universe, Reason : ") + GetRCString(rc));
		aw_destroy();
		Instance = 0;
		if (ModeReco) Tentative();
	}
	else
	{
		wxLogMessage (_("Connected on Universe"));
		On_Universe=true;
	}
}

//------------------------------------------------------------------------------
// Retour Callback d'une demande d'entrée sur un monde

void CBot::Enter_CB(int rc)
{
	--Pending_CB;
	EntEC=false;
	DemCon=false;
	if (rc)
	{
		DemCon=false;
		wxLogMessage (_("Unable to connect on world ") + Monde + _(",Reason : ") + GetRCString (rc));
		aw_destroy();
		Instance=0;
		On_Universe=false;
		On_World=false;
		if (ModeReco) Tentative();
	}
	else
	{
		wxLogMessage (_("Connected on world ")+Monde);
		On_World=true;
		ModeReco=false;
		if (Visible)
		{
			aw_state_change();
		}
		OwnerList.Printf(_T("[%d]"),Citoyen);
	}
}
//------------------------------------------------------------------------------
// Methode d'entrée ou de sortie d'un monde

void CBot::Enter()
{
	aw_bool_set (AW_ENTER_GLOBAL, Global);
	aw_enter(
	#if AW_BUILD > 77
		Monde);
	#else
		Monde.mb_str());
	#endif
	++Pending_CB;
	EntEC=true;
}

//------------------------------------------------------------------------------
// Charge les Paramètres du bot

void CBot::Charge ()
{
	wxString s,S,Tampon;
	bool Log;
    S = wxString::Format(_T("/Bot%02i"), ID);
	pConfig->Read( S + _T("/Univers"), &Univers, _T("auth.activeworlds.com"));
	pConfig->Read( S + _T("/Monde"), &Monde, _T(""));
	pConfig->Read( S + _T("/Citoyen"), &Citoyen, 0);
    pConfig->Read( S + _T("/Nom"), &Nom, _T("Bot"));
    pConfig->Read( S + _T("/Port"), &Port,5670);
	pConfig->Read( S + _T("/AutoConnect"), &CGConAuto, false);
	pConfig->Read( S + _T("/Delai"), &CGRecoDelay, 15);
	pConfig->Read( S + _T("/Essais"), &CGRecoRetry, 3);
	pConfig->Read( S + _T("/Visible"),&Visible, false);
	pConfig->Read( S + _T("/PassPriv"), &s, _T(""));
	if (s.StartsWith (_T("$"),&Tampon))
	{
		PassWord=Tampon;
		Sauve();
	}
	else
	{
        PassWord = PassPriv->Decode(s);
    }
	if (CGRecoRetry != 0) CGRecoEna=true;
	else CGRecoEna=false;
}

//------------------------------------------------------------------------------
// Sauvegarde les param�tres du bot

void CBot::Sauve ()
{
	wxString S;
	S = wxString::Format(_T("/Bot%02i"), ID);
	pConfig->Write( S + _T("/Univers") ,Univers);
	pConfig->Write( S + _T("/Monde") ,Monde);
	pConfig->Write( S + _T("/Citoyen") ,Citoyen);
	pConfig->Write( S + _T("/PassPriv") ,PassPriv->Code(PassWord));
	pConfig->Write( S + _T("/Nom") ,Nom);
	pConfig->Write( S + _T("/Port") ,Port);
	pConfig->Write( S + _T("/AutoConnect") , CGConAuto);
	pConfig->Write( S + _T("/Delai")  , CGRecoDelay);
	pConfig->Write( S + _T("/Essais") , CGRecoRetry);
    pConfig->Write( S + _T("/Visible"), Visible);
	pConfig->Flush(true);
}

//------------------------------------------------------------------------------
// Méthode de reconnection automatique du chat global

void CBot::OnCGRecoEvent (wxTimerEvent& WXUNUSED(event))
{
	Connect();
}

//------------------------------------------------------------------------------
// Tentatives de nouvelle connection

void CBot::Tentative ()
{
	wxString Tampon;
	if ((CGRecoCnt<CGRecoRetry) || (CGRecoRetry < 0))
	{
        wxLogMessage(wxString::Format(_("A reconnection will be tryed in %i sec."), CGRetente));
		CGRecoTimer->Start(CGRetente * 1000,wxTIMER_ONE_SHOT);
		if (CGRecoRetry > (-1)) CGRecoCnt++;
		if (CGRetente < 900) CGRetente = CGRetente * 2;
	}
	else
	{
        wxLogMessage("End of attempts");
		ModeReco=false;
	}
}

//------------------------------------------------------------------------------
// Retourne l'état connection sur un monde

bool CBot::IsOnWorld()
{
	return On_World;
}

//------------------------------------------------------------------------------
// Retourne l'état connecté sur l'univers

bool CBot::IsOnUniverse()
{
	return On_Universe;
}

//------------------------------------------------------------------------------
// Mises à jours périodiques

void CBot::Update ()
{
	if (PerteUniv || PerteMonde)
	{
		if (PerteUniv) wxLogMessage(_("Connection lost with universe ") + Univers + _T("."));
		else wxLogMessage(_("Connection lost with the world ") + Monde + _T("."));
		PerteUniv=false;
		PerteMonde=false;
		On_Universe=false;
		On_World=false;
		Users.clear();
        PSList=_T("");
		ModoList=_T("");
		EminentList=_T("");
		aw_destroy();
		Instance=0;
		if (CGRecoEna)
		{
			CGRecoCnt=0;
			CGRetente=CGRecoDelay;
			Tentative();
			ModeReco=true;
		}
	}
	if (DemCon && (!On_Universe) && (!ConEC))
	{
        Connection(true);
    }
	if (DemCon && On_Universe && (!On_World) && (!EntEC))
	{
        Enter();
    }
	if (DemDeco) Connection(false);
    if (CGRecoTimer->IsRunning() && (!ConEC) && (!ModeReco)) CGRecoTimer->Stop();
}

//------------------------------------------------------------------------------
// Make Bot visible on world

void CBot::SetVisible (bool Flag)
{
	SetInstance();
	if (Flag && (!Visible))
	{
		Visible=true;
		if (On_World)
		{
			aw_state_change();
		}
	}
	else
	{
		Visible=false;
		if (On_World)
		{
			Connection(false);
			Connect ();
		}
	}
}

//------------------------------------------------------------------------------
// Retourne l'ID

int CBot::GetID()
{
	return ID;
}

//------------------------------------------------------------------------------
// On Règle l'instance sur notre bot actuel

bool CBot::SetInstance ()
{
	if (!Instance) return false;
	aw_instance_set(Instance);
	return true;
}

//------------------------------------------------------------------------------
// On retourne l'instance du bot

void* CBot::GetInstance ()
{
	return Instance;
}

//------------------------------------------------------------------------------
// On retourne un utilisateur;

bool CBot::GetUser (int Sess, CUser& User)
{
	for (VUsers::iterator it= Users.begin(); it != Users.end(); ++it)
	{
		if (it->Session==Sess)
		{
			User=(*it);
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------

int	CBot::GetUserX (int Sess)
{
	int ID=GetUserID(Sess);
	if (ID > -1) return Users[ID].X;
	else return 0;
}

//------------------------------------------------------------------------------

int	CBot::GetUserY (int Sess)
{
	int ID=GetUserID(Sess);
	if (ID > -1) return Users[ID].Y;
	else return 0;
}

//------------------------------------------------------------------------------

int	CBot::GetUserZ (int Sess)
{

	int ID=GetUserID(Sess);
	if (ID > -1) return Users[ID].Z;
	else return 0;
}

//------------------------------------------------------------------------------

int CBot::GetUserID(int Sess)
{
	for(VUsers::iterator i = Users.begin(); i != Users.end(); ++i)
	{
		if (i->Session==Sess) return i - Users.begin();
	}
	return -1;
}

//------------------------------------------------------------------------------

int CBot::GetUserID(const wxString& Name)
{
	for(VUsers::iterator i = Users.begin(); i != Users.end(); ++i)
	{
		if (i->Name==Name) return i - Users.begin();
	}
	return -1;
}

//------------------------------------------------------------------------------

wxArrayString CBot::GetUserListe ()
{
	wxArrayString Resultat;
	for(VUsers::iterator i = Users.begin(); i != Users.end(); ++i)
	{
		Resultat.Add(i->Name);
	}
	return Resultat;
}

//------------------------------------------------------------------------------

int CBot::GetUserSession (const wxString& Name)
{
	int ID=GetUserID(Name);
	if (ID<0) return 0;
	return Users[ID].Session;
}

//------------------------------------------------------------------------------

int CBot::GetUserSession (int Cit)
{
	for(VUsers::iterator i = Users.begin(); i != Users.end(); ++i)
	{
		if (i->Citizen==Cit)
		{
			return i->Session;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------

wxString CBot::GetUserIP (int Sess)
{
	int ID=GetUserID(Sess);
	if (ID<0) return _T("0.0.0.0");
	return Users[ID].IP;
}

//------------------------------------------------------------------------------

wxString CBot::GetUserIP (const wxString& Nom)
{
	int ID=GetUserID(Nom);
	if (ID<0) return _T("0.0.0.0");
	return Users[ID].IP;
}

//------------------------------------------------------------------------------

wxString CBot::GetUserName (int Sess)
{
	int ID=GetUserID(Sess);
	if (ID<0) return _T("");
	return Users[ID].Name;
}

//------------------------------------------------------------------------------

int CBot::GetUserCitizen (int Sess)
{
	int ID=GetUserID(Sess);
	if (ID<0) return -1;
	return Users[ID].Citizen;
}

//------------------------------------------------------------------------------

int CBot::GetUserCitizen (const wxString& Name)
{
	int ID=GetUserID(Nom);
	if (ID<0) return -1;
	return Users[ID].Citizen;
}

//------------------------------------------------------------------------------

bool CBot::IsUserEminent (const CUser& User)
{
	wxString Cito,Priv;
	Cito << _T('[') << User.Citizen << _T(']');
	Priv << _T('[') << User.Privilege << _T(']');
	if (EminentList.Contains(Cito) || (EminentList.Contains(Priv))) return true;
	return false;
}

//------------------------------------------------------------------------------

bool CBot::IsUserPublicSpeaker (const CUser& User)
{
	wxString Cito,Priv;
	Cito << _T('[') << User.Citizen  << _T(']');
	Priv << _T('[') << User.Privilege << _T(']');
	if (PSList.Contains(Cito) || (PSList.Contains(Priv))) return true;
	return false;
}

//------------------------------------------------------------------------------

bool CBot::IsUserModerator (const CUser& User)
{
	wxString Cito,Priv;
	Cito << _T('[') << User.Citizen << _T(']');
	Priv << _T('[') << User.Privilege << _T(']');
	if (ModoList.Contains(Cito) || (ModoList.Contains(Priv))) return true;
	return false;
}

//------------------------------------------------------------------------------

bool CBot::IsUserOwner (const CUser& User)
{
	wxString Cito,Priv;
	Cito << _T('[') << User.Citizen << _T(']');
	Priv << _T('[') << User.Privilege << _T(']');
	if (OwnerList.Contains(Cito) || (OwnerList.Contains(Priv))) return true;
	return false;
}

//------------------------------------------------------------------------------

int CBot::AddUser (CUser& User)
{
	if (User.Name==_T("")) return -1;
	Users.push_back (User);
    if (!aw_address (User.Session)) ++Pending_CB;
	return 0;
}

//------------------------------------------------------------------------------

int CBot::SetUserName (int Sess, const wxString& Name)
{
	int ID=GetUserID(Sess);
	if (ID<0) return -1;
	Users[ID].Name=Name;
	return 0;
}

//------------------------------------------------------------------------------

void CBot::SetUserCoord (int Sess,int X, int Y, int Z, int YAW)
{
	int ID=GetUserID(Sess);
	if (ID > -1)
	{
		Users[ID].X=X;
		Users[ID].Y=Y;
		Users[ID].Z=Z;
        Users[ID].Yaw=YAW;
	}
}

//------------------------------------------------------------------------------

int	CBot::SetUserIP (int Sess,int IP)
{
	int ID=GetUserID(Sess);
	wxString Tampon=_T(""),Temp=_T("");
	if (ID<0) return ID;
	Tampon.Printf (_T("%d."), (IP & 0x000000FF));
	Temp.Append(Tampon);
	Tampon.Printf (_T("%d."), ((IP & 0x0000FF00)>>8));
	Temp.Append(Tampon);
	Tampon.Printf (_T("%d."), ((IP & 0x00FF0000)>>16));
	Temp.Append(Tampon);
	Tampon.Printf (_T("%d"), ((IP & 0xFF000000)>>24));
	Temp.Append(Tampon);
	Users[ID].IP=Temp;
	return 0;
}

//------------------------------------------------------------------------------

int CBot::DelUser (int Sess)
{
	for(VUsers::iterator i = Users.begin(); i != Users.end(); ++i)
	{
		if (i->Session == Sess)
		{
			Users.erase (i);
			return 0;
		}
	}
	return -1;
}

//------------------------------------------------------------------------------

int CBot::DelUser (const wxString& Name)
{
	for(VUsers::iterator i = Users.begin(); i != Users.end(); ++i)
	{
		if (i->Name == Name)
		{
			Users.erase (i);
			return 0;
		}
	}
	return -1;
}


//------------------------------------------------------------------------------

int CBot::GetUserPrivilege (int Sess)
{
	int ID=GetUserID(Sess);
	if (ID<0) return -1;
	return Users[ID].Privilege;
}

//------------------------------------------------------------------------------

int CBot::GetUserPrivilege (const wxString& Name)
{
	int ID=GetUserID(Name);
	if (ID<0) return -1;
	return Users[ID].Privilege;
}

//------------------------------------------------------------------------------

int CBot::GetUserAvatar (int Sess)
{
    int ID=GetUserID(Sess);
	if (ID<0) return -1;
	return Users[ID].Avatar;
}

//------------------------------------------------------------------------------

int CBot::GetUserAvatar (const wxString& Name)
{
    int ID=GetUserID(Name);
	if (ID<0) return -1;
	return Users[ID].Avatar;
}
//------------------------------------------------------------------------------
// Methode de Traduction du codes d'erreurs

wxString CBot::GetRCString (int rc)
{
	wxString rcs;
	switch (rc)
	{
		case 0 :
			rcs=_T("");
			break;
		case 1 :
			rcs=_("Citizen Ship Expired");
			break;
		case 2 :
			rcs=_("Land Limit Exceeded");
			break;
		case 3 :
			rcs=_("No Such Citizen");
			break;
		case 4 :
			rcs=_("Message Length Bad");
			break;
		case 5 :
			rcs=_("License Password Contains Space");
			break;
		case 6 :
			rcs=_("License Password Too Long");
			break;
		case 7 :
    	    rcs=_("License Password Too Short");
    	    break;
		case 8 :
			rcs=_("License Range Too Large");
			break;
		case 9 :
			rcs=_("License Range Too Small");
			break;
		case 10 :
			rcs=_("License Users Too Large");
			break;
		case 11 :
			rcs=_("License Users Too Small");
			break;
		case 12 :
			rcs=_("License Contains Invalid Char");
			break;
		case 13 :
			rcs=_("Invalid Password");
			break;
		case 14 :
			rcs=_("Unable To Mail Back Number");
			break;
		case 15 :
			rcs=_("License World Too Short");
			break;
		case 16 :
			rcs=_("License World Too Long");
			break;
		case 17 :
			rcs=_("Server Out Of Memory");
			break;
		case 20 :
			rcs=_("Invalid World");
			break;
		case 21 :
			rcs=_("Server Outdated");
			break;
		case 22 :
			rcs=_("World Already Started");
			break;
		case 27 :
			rcs=_("No Such World");
			break;
		case 31 :
			rcs=_("Not Logged In");
			break;
		case 32 :
			rcs=_("Unauthorized");
			break;
		case 33 :
			rcs=_("World Already Exists");
			break;
		case 34 :
			rcs=_("No Such License");
			break;
		case 39 :
			rcs=_("Identity Already In Use");
			break;
		case 40 :
			rcs=_("Unable To Report Location");
			break;
		case 41 :
			rcs=_("Invalid Email");
			break;
		case 42 :
			rcs=_("No Such Acting Citizen");
			break;
		case 43 :
			rcs=_("Acting Password Invalid");
			break;
		case 45 :
			rcs=_("Universe Full");
			break;
		case 46 :
			rcs=_("Billing Timeout");
			break;
		case 47 :
			rcs=_("Billing Recv Failed");
			break;
		case 48 :
			rcs=_("Billing Response Invalid");
			break;
		case 55 :
			rcs=_("Billing Rejected");
			break;
		case 56 :
			rcs=_("Billing_Blocked");
			break;
		case 57 :
			rcs=_("Too Many Worlds");
			break;
		case 58 :
			rcs=_("Must Upgrade");
			break;
		case 59 :
			rcs=_("Bot Limit Exceeded");
			break;
		case 61 :
			rcs=_("World_Expired");
			break;
		case 62 :
			rcs=_("Citizen Does Not Expire");
			break;
		case 64 :
			rcs=_("License Sarts With Number");
			break;
		case 66 :
			rcs=_("No Such Ejection");
			break;
		case 67 :
			rcs=_("No Such Session");
			break;
		case 69 :
			rcs=_("Ejection Expired");
			break;
		case 70 :
			rcs=_("Acting Citizen Expired");
			break;
		case 71 :
			rcs=_("Already_Started");
			break;
		case 72 :
			rcs=_("World Running");
			break;
		case 73 :
			rcs=_("World Not Set");
			break;
		case 74 :
			rcs=_("No Such Cell");
			break;
		case 75 :
			rcs=_("No Registry");
			break;
		case 76 :
			rcs=_("Can't Open Registry");
			break;
		case 77 :
			rcs=_("Citizen_Disabled");
			break;
		case 78 :
			rcs=_("World Disabled");
			break;
		case 79 :
			rcs=_("Beta Required");
			break;
		case 80 :
			rcs=_("Acting Citizen Disabled");
			break;
		case 81 :
			rcs=_("Invalid Uuser Count");
			break;
		case 91 :
			rcs=_("Private World");
			break;
		case 92 :
			rcs=_("No Tourists");
			break;
		case 100 :
			rcs=_("EMail Contains Invalid Char");
			break;
		case 101 :
			rcs=_("EMail Ends With Blank");
			break;
		case 102 :
			rcs=_("EMail Missing Dot");
			break;
		case 103 :
			rcs=_("EMail Missing At");
			break;
		case 104 :
			rcs=_("EMail Starts With Blank");
			break;
		case 105 :
			rcs=_("EMail Too Long");
			break;
		case 106 :
			rcs=_("EMail Too Short");
			break;
		case 107 :
			rcs=_("Name Already Used");
			break;
		case 108 :
			rcs=_("Name Contains Nonalphanumeric Char");
			break;
		case 109 :
			rcs=_("Name Contains Invalid Blank");
			break;
		case 110 :
			rcs=_("Name Doesn't Exist");
			break;
		case 111 :
			rcs=_("Name Ends With Blank");
			break;
		case 112 :
			rcs=_("Name Too Long");
			break;
      	case 113 :
			rcs=_("Name Too Short");
			break;
		case 114 :
			rcs=_("Name Unused");
			break;
		case 115 :
			rcs=_("Password Too Long");
			break;
		case 116 :
			rcs=_("Password Too Short");
			break;
		case 117 :
			rcs=_("Password Wrong");
			break;
		case 119 :
			rcs=_("Unable To Delete Name");
			break;
		case 120 :
			rcs=_("Unable To Get Citizen");
			break;
		case 121 :
			rcs=_("Unable To Insert Citizen");
			break;
		case 122 :
			rcs=_("Unable To Insert Name");
			break;
		case 123 :
			rcs=_("Unable To Put Citizen Count");
			break;
		case 124 :
			rcs=_("Unable To Delete Citizen");
			break;
		case 126 :
			rcs=_("Number Already Used");
			break;
		case 127 :
			rcs=_("Number Out Of Range");
			break;
		case 128 :
			rcs=_("Privilege Password Is Too Short");
			break;
		case 129 :
			rcs=_("Privilege Password Is Too Long");
			break;
		case 203 :
			rcs=_("Not Change Owner");
			break;
		case 204 :
			rcs=_("Can't Find Old Element");
			break;
		case 210 :
			rcs=_("Unable To Change Attribute");
			break;
		case 211 :
			rcs=_("Can't Change Owner");
			break;
		case 212 :
			rcs=_("Imposter");
			break;
		case 213 :
			rcs=_("Invalid Request");
			break;
		case 216 :
			rcs=_("Can't Build Here");
			break;
		case 300 :
			rcs=_("Encroaches");
			break;
		case 301 :
			rcs=_("No Such Object");
			break;
		case 302 :
			rcs=_("Not Delete Owner");
			break;
		case 303 :
			rcs=_("Too Many Bytes");
			break;
		case 305 :
			rcs=_("Unable To Store");
			break;
		case 306 :
			rcs=_("Unregistered Object");
			break;
		case 308 :
			rcs=_("Element Already Exists");
			break;
		case 309 :
			rcs=_("Restricted Command");
			break;
		case 310 :
			rcs=_("No Build Rights");
			break;
		case 311 :
			rcs=_("Out Of Bounds");
			break;
		case 313 :
			rcs=_("Restricted Object");
			break;
		case 314 :
			rcs=_("Restricted Area");
			break;
		case 400 :
			rcs=_("Out Of Memory");
			break;
		case 401 :
			rcs=_("Not Yet");
			break;
		case 402 :
			rcs=_("Timeout");
			break;
		case 403 :
			rcs=_("Null Pointer");
			break;
		case 404 :
			rcs=_("Unable To Contact Universe");
			break;
		case 405 :
			rcs=_("Unable To Contact World");
			break;
		case 406 :
			rcs=_("Invalid World Name");
			break;
		case 415 :
			rcs=_("Send Failed");
			break;
		case 416 :
			rcs=_("Receive Failed");
			break;
		case 421 :
			rcs=_("Stream Empty");
			break;
		case 422 :
			rcs=_("Stream Message Too Long");
			break;
		case 423 :
			rcs=_("World Name Too Long");
			break;
		case 426 :
			rcs=_("Message Too Long");
			break;
		case 427 :
			rcs=_("Too Many Resets");
			break;
		case 428 :
			rcs=_("Unable To Create Socket");
			break;
		case 429 :
			rcs=_("Unable To Connect");
			break;
		case 430 :
			rcs=_("Unable To Set NonBlocking");
			break;
		case 434 :
			rcs=_("Can't Open Stream");
			break;
		case 435 :
			rcs=_("Can't Write Stream");
			break;
		case 436 :
			rcs=_("Can't Close Stream");
			break;
		case 439 :
			rcs=_("No Connection");
			break;
		case 442 :
			rcs=_("Unable To Initialize Network");
			break;
		case 443 :
			rcs=_("Incorrect Message Length");
			break;
		case 444 :
			rcs=_("Not Initialized");
			break;
		case 445 :
			rcs=_("No Instance");
			break;
		case 446 :
			rcs=_("Out Buffer Full");
			break;
		case 447 :
			rcs=_("Invalid Callback");
			break;
		case 448 :
			rcs=_("Invalid Attribute");
			break;
		case 449 :
			rcs=_("Type Mismatch");
			break;
		case 450 :
			rcs=_("String Too Long");
			break;
		case 451 :
			rcs=_("Read Only");
			break;
		case 452 :
			rcs=_("Unable To Register Resolve");
			break;
		case 453 :
			rcs=_("Invalid Instance");
			break;
		case 454 :
			rcs=_("Version Mismatch");
			break;
		case 461 :
			rcs=_("In Buffer Full");
			break;
		case 463 :
			rcs=_("Protocol Error");
			break;
		case 464 :
			rcs=_("Query In Progress");
			break;
		case 465 :
			rcs=_("World Full");
			break;
		case 466 :
			rcs=_("Ejected");
			break;
		case 467 :
			rcs=_("Not Welcome");
			break;
		case 468 :
			rcs=_("Unable To Bind");
			break;
		case 469 :
			rcs=_("Unable To Listen");
			break;
		case 470 :
			rcs=_("Unable To Accept");
			break;
		case 471 :
			rcs=_("Connection Lost");
			break;
		case 473 :
			rcs=_("No Stream");
			break;
		case 474 :
			rcs=_("Not Available");
			break;
		case 487 :
			rcs=_("Old Universe");
			break;
		case 488 :
			rcs=_("Old World");
			break;
		case 489 :
			rcs=_("World Not Running");
			break;
		case 505 :
			rcs=_("Invalid Argument");
			break;
		default:
			rcs=wxString::Format(_("Unknow Raison Code %d"),rc);
			break;
   }
   return rcs;
}

//------------------------------------------------------------------------------
// Return Number of callbacks are pending

int CBot::GetPendingCB ()
{
	return Pending_CB;
}

//------------------------------------------------------------------------------
// Return attribute int value

int CBot::GetAttribute (AW_ATTRIBUTE att)
{
	if (SetInstance ())
	{
		return aw_int (att);
	}
	return 0;
}

//------------------------------------------------------------------------------
// Return attribute string value

void CBot::GetAttribute (AW_ATTRIBUTE att, wxString& str)
{
	if (SetInstance ())
	{
#if AW_BUILD>77
		str = aw_string (att);
#else
		str = wxString::From8BitData (aw_string (att));
#endif
	}
}

//------------------------------------------------------------------------------
// Set int attribute value

int	CBot::SetAttribute (AW_ATTRIBUTE att, int value)
{
	if (SetInstance ())
	{
		return aw_int_set(att, value);
	}
	return 0;
}

//------------------------------------------------------------------------------
// Set stgring attribute value

int	CBot::SetAttribute (AW_ATTRIBUTE att, const wxString& str)
{
	if (SetInstance ())
	{
#if AW_BUILD>77
		return aw_string_set(att,str);
#else
		return aw_string_set(att,str.mb_str());
#endif
	}
	return 0;
}
#if AW_BUILD>77
//------------------------------------------------------------------------------
// Create HUD

int CBot::CreateHud ()
{
	int rc = 0;
	if (Instance)
	{
		SetInstance ();
		if (!(rc=aw_hud_create ())) ++Pending_CB;
	}
	return rc;
}
#endif
//------------------------------------------------------------------------------
// Load Object

int CBot::LoadObject ()
{
	int rc = 0;
	if (Instance)
	{
		++Pending_CB;
		if ((rc=aw_object_load ()) == 0 ) ++Pending_CB;
	}
	return rc;
}


#if AW_BUILD>77
//------------------------------------------------------------------------------
// HUD Callback

void CBot::HUD_CB(int rc)
{
	--Pending_CB;
}
#endif

//------------------------------------------------------------------------------
// Address Callback

void CBot::Address_CB (int rc)
{
	--Pending_CB;
	if (rc) wxLogMessage (wxString::Format(_T("Bot %i : Unable to solve IP, Reason:"),GetID()) + GetRCString(rc));
	else SetUserIP (aw_int(AW_AVATAR_SESSION) ,aw_int(AW_AVATAR_ADDRESS));
}

//------------------------------------------------------------------------------
// Address Callback

void CBot::Object_CB (int WXUNUSED(rc))
{
	--Pending_CB;
}

//------------------------------------------------------------------------------
// Send a console message

void CBot::ConsoleMessage(const wxString& Message, int Sess, int R, int V, int B, bool Bold, bool Ita)
{
	if(SetInstance())
	{
		aw_int_set(AW_CONSOLE_RED, R);
		aw_int_set(AW_CONSOLE_GREEN, V);
		aw_int_set(AW_CONSOLE_BLUE, B);
		aw_bool_set(AW_CONSOLE_BOLD, Bold);
		aw_bool_set(AW_CONSOLE_ITALICS, Ita);
#if AW_BUILD>77
		aw_string_set(AW_CONSOLE_MESSAGE, _T("CBot Console Message : ") + Message);
#else
		aw_string_set(AW_CONSOLE_MESSAGE, wxString(Message).mb_str());
#endif
		aw_console_message(Sess);
	}
}
