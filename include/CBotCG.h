// *****************************************************************************
// *                                                                           *
// *                          CLASSE DE BOT DERIVEE                            *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 15/05/2005                                                           *
// *   CopyRight (c)Neophile 2005                                              *
// *   Modifie le 06/06/2008 : Définition d'ue cito modératrice globale       *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CBOTCG_H
#define CBOTCG_H

#ifndef  WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/wxprec.h>

#include "DBase.h"
#include "DBColor.h"
#include "CBot.h"
#include "Ciel.h"

#include "CColour.h"
#include "CDiffusion.h"

using namespace std;

class CBotCG : public CBot, CDiffusionEventHandler
{
    private:
	    CDiffusion&     Diffusion;

    public:

        CDBase*         DBase;
        CDBColor*       DBColor;
        CCiel*          Ciel;

        bool            CGON;
        bool            EnaCoulPerso;
        bool            EnaStyle;
        bool            ChatBride;
        int             ChatDistance;
        bool            ExtBride;
        int             ExtDistance;
        int             ExtCoordX;
        int             ExtCoordZ;
        int				SessionReq;

	    bool            Kling;                 // Possibilité de Klinguer
        bool            Kloug;                 // Possibilité de Klouguer
        bool            BlockCG;		// Bloque le ChatGlobal

        wxString        NKling;           // Nom du dernier entré
		wxString        NKloug;           // Nom du dernier sorti

// Personnalisations des messages
        bool            EnaMessAc;
        wxString        TxtMessAc;
        CColour         ClrMessAc;
        bool            EnaAnnArr;
        wxString        TxtAnnArr;
        CColour         ClrAnnArr;
        bool            EnaAnnDep;
        wxString        TxtAnnDep;
        CColour         ClrAnnDep;
        bool            EnaAnnKli;
        wxString        TxtAnnKli;
        CColour         ClrAnnKli;
        bool            EnaAnnKlo;
        wxString        TxtAnnKlo;
        CColour         ClrAnnKlo;
static  bool            Ejecte;
static  unsigned int    IP;
static  wxString        Ejector;
static  wxString        EjectNom;
static  unsigned int    EjectCit;
static  unsigned int    EjectTime;

// ----------------
// METHODES PUBLICS
// ----------------

// Constructeur et Destructeur
                        CBotCG	(CDiffusion& pDiffusion);
                        ~CBotCG	();

// Entiers

        wxString    Analyse	( wxString Name=_T(""),
                                int Session=0,
                                int Citoyen=0,
                                wxString Message=_T(""),
                                int Type=0,
                                bool Mess_Ext=false,
                                wxString Client=_T(""),
                                int Privilege=0
							);
        void        Sauve ();
        void        Charge ();
        void        ConMess ( int Session=0,
                                wxString Message=_T(""),
                                int R=0,
                                int V=0,
                                int B=0,
                                bool Bold=false,
                                bool Ital=false
                            );
        int         Mess_Bot ( wxString Name=_T(""),
                                int code=0,
                                wxString Client=_T(""),
                                int Session=0
                            );
        CColour     ExtColor (const wxString &Colour);
        int         Distance (int Session1, int Session2, bool Ext=false);
        void        Eject ();
        void        TellList (int Session);

    protected:
        void        OnReceiveMessage(
                        const wxString& pMessage,
                        const void* pInstance,
                        const wxString& pName,
                        const wxString& pClient,
                        int pType);
};

#endif

