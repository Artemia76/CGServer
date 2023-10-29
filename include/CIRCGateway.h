// *****************************************************************************
// *                                                                           *
// *                        CLASS CLIENT IRC GATEWAY                           *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 28/10/2023                                                           *
// *   CopyRight (c)Neophile 2023                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#pragma once

#ifndef  WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/wxprec.h>
#include <wx/socket.h>
#include <wx/config.h>
#include "Cpasspriv.h"
#include "CDiffusion.h"

enum
{
	IRC_SOCKET_ID=wxID_HIGHEST,
	IRC_RECO,
	IRC_HB
};

class CIRCGateway : public wxEvtHandler, CDiffusionEventHandler
{
	private:
		wxSocketClient*	SockIRC;
		wxTimer*		IRCRecoTimer;
		wxTimer*		IRCHeartBeat;
		CPassPriv*		PassPriv;
        CDiffusion&     Diffusion;

		bool			ConEC;
		bool			DemCon;
		bool			On_IRC;
		bool			FirstCon;

		void			Tentative		();
		wxArrayString	TamponTab;

	public:

		wxConfigBase	*pConfig;

		bool			IRCConAuto;
		bool			IRCRecoEna;
		bool			IRCCGEna;
		bool			ModeReco;

		int				IRCCompt;
		int				IRCPort;
		int				IRCRecoDelay;
		int				IRCRecoRetry;
		int				IRCRecoCnt;
		int				IRCRetente;

		wxString		IRCHost;
		wxString		IRCLogin;
		wxString		IRCPassWord;
        wxString        IRCChannel;

						CIRCGateway	(CDiffusion& pDiffusion);
						~CIRCGateway ();
        void            Init (bool pFlag);
		void			Connect ();
		void			Deconnect ();
		bool			IsOnIRC	();
		void			Charge ();
		void			Sauve ();
		void			EnvoiMess
						(
							wxString Message,
							int Type=0,
							wxString Name=_T("")
						);
		void			Update ();

	protected:
		void			IRCConnect		(bool);// Connection/Deconnection au serveur Xelag
		void			OnIRCRecoEvent	(wxTimerEvent  & event);
		void			OnIRCHeartBeat   (wxTimerEvent& event);
		void			OnIRCEvent	(wxSocketEvent & event);
        void            OnReceiveMessage (
                            const wxString& pMessage,
                            const void* pInstance,
                            const wxString& pName,
                            const wxString& pClient,
                            int pType);
		wxDECLARE_EVENT_TABLE();
};

