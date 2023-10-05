// *****************************************************************************
// *                                                                           *
// *                              CLASS DE BOT                                 *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 15/05/2005                                                           *
// *   CopyRight (c)Neophile 2005                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CBOT_H
#define CBOT_H

#include <wx/wxprec.h>

#ifndef  WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/config.h>

#include <aw.h>

#ifndef COMMON_DLL
	#ifdef DLL_EXPORTS
		#define COMMON_DLL WXEXPORT
	#else
		#define COMMON_DLL WXIMPORT
	#endif
#endif

#include "Cpasspriv.h"
#include "CUser.h"

enum
{
	CG_RECO=wxID_HIGHEST
};

class COMMON_DLL CBot : public wxEvtHandler
{
    public:

    friend class CCtrlAw;

        bool                Global;			// Mode Global
        bool	            CGConAuto;
        bool	            CGRecoEna;
        int                 CGRecoDelay;	// Temps de Reconnection sur AW
        int                 CGRecoRetry;	// Nombres d'essais sur AW
        int                 Citoyen;		// Numéro du citoyen
        int                 Port;			// Port de l'univers
        int                 CGRecoCnt;			// Compteur de tentative de reconnections AW
        int                 CGRetente;
        bool                ModeReco;

        wxString            Monde;			// Nom du Monde
        wxString	        Nom;			// Nom du Bot
        wxString	        PassWord;		// Mot de Passe Citoyen
        wxString	        Univers;		// Adresse de l'univers

// Constructeur et Destructeur
                            CBot                ();
                            ~CBot               ();

        void                Connection	        (bool);	// Connection/Deconnection de l'univers
        void                Enter		        ();	// Entrer/Sortir du monde
        void                Connect		        ();
        void                Deconnect	        ();
virtual	void                Update		        ();
		void                Tentative	        ();

        bool                IsOnUniverse        ();
        bool                IsOnWorld	        ();
        int                 GetID		        ();
virtual void                Sauve		        ();
virtual void                Charge		        ();
        bool                SetInstance	        ();
        void*               GetInstance         ();
        void			    SetVisible			(bool Flag);
        bool				GetUser				(int Sess, CUser& User);
		const VUsers&		GetUserVec			() const { return Users;}
		int                 GetUserSession	    (const wxString& Name);
		int					GetUserSession		(int Cit);
		int			        GetUserX		    (int Sess);
		int 			    GetUserY		    (int Sess);
		int			        GetUserZ		    (int Sess);
        int					GetUserYaw			(int Sess);
		wxArrayString       GetUserListe	    ();
		wxString		    GetUserIP		    (int Sess);
		wxString		    GetUserIP		    (const wxString& Name);
		wxString		    GetUserName		    (int Sess);
		int                 GetUserCitizen      (int Sess);
		int                 GetUserCitizen	    (const wxString& Name);
		int                 GetUserPrivilege	(int Sess);
		int                 GetUserPrivilege	(const wxString& Name);
        int					GetUserID			(int Sess);
		int					GetUserID			(const wxString& Name);
        int                 GetUserAvatar       (int Sess);
        int                 GetUserAvatar       (const wxString& Name);
        bool				IsUserEminent		(int Sess);
		bool				IsUserPublicSpeaker (int Sess);
		bool				IsUserModerator		(int Sess);
		bool				IsUserOwner			(int Sess);
		void				ConsoleMessage		(const wxString& Message, int Sess=0, int R = 0, int V = 0,
												int B=0, bool Bold=false, bool Ita=false);
// AW Method proxy for async callback and event safe
		int					GetAttribute        (AW_ATTRIBUTE att);
		void				GetAttribute        (AW_ATTRIBUTE att, wxString& str);
		int					SetAttribute        (AW_ATTRIBUTE att, int value);
		int					SetAttribute        (AW_ATTRIBUTE att, const wxString& str);
#if AW_BUILD>77
		int					CreateHud ();
#endif
		int					LoadObject ();
		int					GetPendingCB ();

static	wxString	        GetRCString			(int); // Traduction littérale du reason code

    private:
        int			        Pending_CB;			// Number of callback who are pending
        bool				On_Exit;			// Demande de quitter
        bool                PerteUniv;
        bool                PerteMonde;
        bool                DemCon;
        bool                ConEC;
        bool                EntEC;
        bool                On_Universe;		// Etat de la connection univers
        bool                On_World;			// Etat de la connection au monde
		bool                DemDeco;
        bool                Visible;			// Etat de la visibilité
        CPassPriv*          PassPriv;
        void*               Instance;

static	int				    NBot;				// Index statique du nombre de bot

	protected:
        wxString            PSList;
		wxString            ModoList;
		wxString            EminentList;
		wxString            OwnerList;
		wxTimer*            CGRecoTimer;
        wxConfigBase*       pConfig;
        VUsers              Users;
        int                 ID;					// Identification du bot

        void                Login_CB	(int rc);
		void                Enter_CB	(int rc);
#if AW_BUILD>77
		void				HUD_CB				(int rc);
#endif
		void				Address_CB			(int rc);
		void				Object_CB			(int rc);

		void                OnCGRecoEvent (wxTimerEvent  & event);
		int                 AddUser (CUser&);
		int                 DelUser (int Sess);
		int                 DelUser (const wxString& Name);
		int                 SetUserName (int Sess, const wxString& Name);
		void                SetUserCoord (int Sess,int X=0, int Y=0, int Z=0, int YAW=0);
		int                 SetUserIP (int Sess=0,int IP=0);
		wxDECLARE_EVENT_TABLE();
};

#endif

