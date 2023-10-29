// *****************************************************************************
// *                                                                           *
// *                           CLASS DE BOT DERIVEE                            *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 16/05/2005                                                           *
// *   Modifié e 06/06/2008 : Ajout d'une citoyenneté modératricglobale     *
// *   CopyRight (c)Neophile 2005                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                          DEFINITION DES MEMBRES                           *
// *                                                                           *
// *****************************************************************************

#include "CBotCG.h"

#include <Aw.h>
#include <math.h>
#include <arpa/inet.h>
#include "global.h"


bool CBotCG::Ejecte=false;
unsigned int CBotCG::IP=0;
wxString CBotCG::Ejector=_T("");
wxString CBotCG::EjectNom=_T("");
unsigned int CBotCG::EjectCit=0;
unsigned int CBotCG::EjectTime=24;

//------------------------------------------------------------------------------
// Contructeur

CBotCG::CBotCG (CDiffusion& pDiffusion) : CBot() , Diffusion(pDiffusion)
{
    DBase = CDBase::Create ();
    DBColor = CDBColor::Create();
    Ciel = new CCiel (CBot::ID);
    // Définition des variables internes
    Diffusion.Subscribe(this);
    CGON=false;
    Kling=false;
    Kloug=false;
    EnaStyle=false;
    BlockCG=false;
    SessionReq=0;
}

CBotCG::~CBotCG ()
{
    Diffusion.UnSubscribe(this);
    Sauve ();
    delete Ciel;
}

//------------------------------------------------------------------------------
// Charge les Paramètres du bot

void CBotCG::Charge ()
{
    int R,V,B;
    wxString S;
    CBot::Charge();
    S.Printf(_T("/Bot%02d"),ID);
    pConfig->Read( S + _T("/EnaCoulPerso"), &EnaCoulPerso, true);
    pConfig->Read( S + _T("/EnaStyle"), &EnaStyle, false);
    pConfig->Read( S + _T("/ChatBride"), &ChatBride, false);
    pConfig->Read( S + _T("/ChatDistance"), &ChatDistance, 250);
    pConfig->Read( S + _T("/ExtBride"), &ExtBride, false);
    pConfig->Read( S + _T("/ExtDistance"), &ExtDistance, 250);
    pConfig->Read( S + _T("/ExtCoordX"), &ExtCoordX, 0);
    pConfig->Read( S + _T("/ExtCoordZ"), &ExtCoordZ, 0);
    pConfig->Read( S + _T("/CGON"), &CGON, false);
// Chargement des paramètres personnalisés
    pConfig->Read( S + _T("/EnaMessAc"), &EnaMessAc, true);
    pConfig->Read( S + _T("/TxtMessAc"), &TxtMessAc, _("Welcome on the ChatGlobal %n."));
    pConfig->Read( S + _T("/ClrMessAc/R"), &R, 255);
    pConfig->Read( S + _T("/ClrMessAc/V"), &V, 100);
    pConfig->Read( S + _T("/ClrMessAc/B"), &B, 100);
    ClrMessAc.Set( R , V , B );
    pConfig->Read( S + _T("/EnaAnnArr"), &EnaAnnArr, true);
    pConfig->Read( S + _T("/TxtAnnArr"), &TxtAnnArr, _("%n incoming."));
    pConfig->Read( S + _T("/ClrAnnArr/R"), &R, 255);
    pConfig->Read( S + _T("/ClrAnnArr/V"), &V, 100);
    pConfig->Read( S + _T("/ClrAnnArr/B"), &B, 100);
    ClrAnnArr.Set( R , V , B );
    pConfig->Read( S + _T("/EnaAnnDep"), &EnaAnnDep, true);
    pConfig->Read( S + _T("/TxtAnnDep"), &TxtAnnDep, _("%n outgoing"));
    pConfig->Read( S + _T("/ClrAnnDep/R"), &R, 255);
    pConfig->Read( S + _T("/ClrAnnDep/V"), &V, 100);
    pConfig->Read( S + _T("/ClrAnnDep/B"), &B, 100);
    ClrAnnDep.Set( R , V , B );
    pConfig->Read( S + _T("/EnaAnnKli"), &EnaAnnKli, true);
    pConfig->Read( S + _T("/TxtAnnKli"), &TxtAnnKli, _("Great %n, good Kling on %c !!!, you have now %p points!"));
    pConfig->Read( S + _T("/ClrAnnKli/R"), &R, 255);
    pConfig->Read( S + _T("/ClrAnnKli/V"), &V, 100);
    pConfig->Read( S + _T("/ClrAnnKli/B"), &B, 100);
    ClrAnnKli.Set( R , V , B );
    pConfig->Read( S + _T("/EnaAnnKlo"), &EnaAnnKlo, true);
    pConfig->Read( S + _T("/TxtAnnKlo"), &TxtAnnKlo, _("Great %n, good Kloug on %c !!!, you have now %p points!"));
    pConfig->Read( S + _T("/ClrAnnKlo/R"), &R, 255);
    pConfig->Read( S + _T("/ClrAnnKlo/V"), &V, 100);
    pConfig->Read( S + _T("/ClrAnnKlo/B"), &B, 100);
    ClrAnnKlo.Set( R , V , B );
    Sauve ();
}

//------------------------------------------------------------------------------
// Sauvegarde les paramètres du bot

void CBotCG::Sauve ()
{
    wxString S;
    CBot::Sauve();
    S.Printf(_T("/Bot%02d"),ID);
    pConfig->Write( S + _T("/EnaCoulPerso"), EnaCoulPerso);
    pConfig->Write( S + _T("/EnaStyle"), EnaStyle);
    pConfig->Write( S + _T("/ChatBride"), ChatBride);
    pConfig->Write( S + _T("/ChatDistance"), ChatDistance);
    pConfig->Write( S + _T("/ExtBride"), ExtBride);
    pConfig->Write( S + _T("/ExtDistance"), ExtDistance);
    pConfig->Write( S + _T("/ExtCoordX"), ExtCoordX);
    pConfig->Write( S + _T("/ExtCoordZ"), ExtCoordZ);
    pConfig->Write( S + _T("/CGON"), CGON);
// Sauvegarde des paramètres personnalisés
    pConfig->Write( S + _T("/EnaMessAc"), EnaMessAc);
    pConfig->Write( S + _T("/TxtMessAc"), TxtMessAc);
    pConfig->Write( S + _T("/ClrMessAc/R"), ClrMessAc.Red());
    pConfig->Write( S + _T("/ClrMessAc/V"), ClrMessAc.Green());
    pConfig->Write( S + _T("/ClrMessAc/B"), ClrMessAc.Blue());
    pConfig->Write( S + _T("/EnaAnnArr"), EnaAnnArr);
    pConfig->Write( S + _T("/TxtAnnArr"), TxtAnnArr);
    pConfig->Write( S + _T("/ClrAnnArr/R"), ClrAnnArr.Red());
    pConfig->Write( S + _T("/ClrAnnArr/V"), ClrAnnArr.Green());
    pConfig->Write( S + _T("/ClrAnnArr/B"), ClrAnnArr.Blue());
    pConfig->Write( S + _T("/EnaAnnDep"), EnaAnnDep);
    pConfig->Write( S + _T("/TxtAnnDep"), TxtAnnDep);
    pConfig->Write( S + _T("/ClrAnnDep/R"), ClrAnnDep.Red());
    pConfig->Write( S + _T("/ClrAnnDep/V"), ClrAnnDep.Green());
    pConfig->Write( S + _T("/ClrAnnDep/B"), ClrAnnDep.Blue());
    pConfig->Write( S + _T("/EnaAnnKli"), EnaAnnKli);
    pConfig->Write( S + _T("/TxtAnnKli"), TxtAnnKli);
    pConfig->Write( S + _T("/ClrAnnKli/R"), ClrAnnKli.Red());
    pConfig->Write( S + _T("/ClrAnnKli/V"), ClrAnnKli.Green());
    pConfig->Write( S + _T("/ClrAnnKli/B"), ClrAnnKli.Blue());
    pConfig->Write( S + _T("/EnaAnnKlo"), EnaAnnKlo);
    pConfig->Write( S + _T("/TxtAnnKlo"), TxtAnnKlo);
    pConfig->Write( S + _T("/ClrAnnKlo/R"), ClrAnnKlo.Red());
    pConfig->Write( S + _T("/ClrAnnKlo/V"), ClrAnnKlo.Green());
    pConfig->Write( S + _T("/ClrAnnKlo/B"), ClrAnnKlo.Blue());
    pConfig->Flush(true);
}

//------------------------------------------------------------------------------

wxString CBotCG::Analyse(wxString Name, int Session, int Citoyen, wxString Message, int Type, bool Mess_Ext, wxString Client, int Privilege)
{
    CColour Couleur(0,0,0);
    wxString Reponse, Original, UCase, Titre, Quali, Tampon;
    wxArrayString Liste;
    int Dest=0, id, R=255, V=50, B=50, pos=-1, rc, Dist;
    bool Bold=false,Ita=false,Ti=false,Qu=false,Me=false,PSpeaker=false,MessBot=false,Eminent=false;
    bool cmd_ok=true, Modo=false, Dieu=false;
    bool SessGlob1,SessGlob2;
    wxString Cito,Priv;
    long valeure=0;
    if (!Mess_Ext)
    {
        Dest=Session;
        Cito << Citoyen;
        Cito = _T('[') + Cito + _T(']');
        Priv << Privilege;
        Priv = _T('[') + Priv + _T(']');
        if (Type==AW_CHAT_BROADCAST) PSpeaker=true;
        if (ModoList.Contains(Priv) || ModoList.Contains(Cito)) Modo=true;
        if (EminentList.Contains(Priv) || EminentList.Contains(Cito)) Eminent=true;
    }
    id=DBase->Identifie (Name, Citoyen);
    if ((DBase->GetMuteMode (id)) && (!Modo) && (!Eminent)) return _T("");
    // Recherche anti retour a la ligne
    while ((rc=Message.Find(_T('\n')))!=wxNOT_FOUND)
    {
        Message.Remove(rc,1);
    }
    while ((rc=Message.Find(_T('\r')))!=wxNOT_FOUND)
    {
        Message.Remove(rc,1);
    }
    Original=Message;
    UCase=Message.Upper();
    while(1)
    {
// Zone publique
        if (UCase==_T("VERSION"))
        {
            Reponse=CGVersion + _(" by ") + CGCopyright;
            MessBot=true;
            break;
        }
        if ((UCase==_T("KLING"))&&Kling&&(Name!=NKling))
        {
            if (!EnaAnnKli) break;
            Kling=false;
            DBase->AddKling(id);
            Reponse=TxtAnnKli;
            Tampon.Printf (_T("%d"),DBase->GetNbKling(id));
            Reponse.Replace(_T("%n"),Name, true);
            Reponse.Replace(_T("%p"),Tampon, true);
            Reponse.Replace (_T("%c"), NKling, true);
            R=ClrAnnKli.Red();
            V=ClrAnnKli.Green();
            B=ClrAnnKli.Blue();
            Dest=0;
            MessBot=true;
            break;
        }
        if ((UCase==_T("KLOUG"))&&Kloug&&(Name!=NKloug))
        {
            if (!EnaAnnKlo) break;
            Kloug=false;
            DBase->AddKloug(id);
            Reponse=TxtAnnKlo;
            Tampon.Printf (_T("%d"),DBase->GetNbKloug(id));
            Reponse.Replace(_T("%n"),Name, true);
            Reponse.Replace(_T("%p"),Tampon, true);
            Reponse.Replace (_T("%c"), NKloug, true);
            R=ClrAnnKlo.Red();
            V=ClrAnnKlo.Green();

            break;
        }
        if ((UCase==_T("/AIDE") || UCase== _T("/HELP")) && (!Mess_Ext))
        {
            MessBot=true;
            cmd_ok=true;
            R=45;V=145;B=244;
            ConMess(Dest,_("ChatGLobal Help: (to use commands, place them in first followed by a space of the sentence)"),251,150,38,true);
            ConMess(Dest,_("/me\t:Speak without tabulation"),R,V,B,true);
            ConMess(Dest,_("/bold\t:Speak bold"),R,V,B,true);
            ConMess(Dest,_("/ita\t:Speak italic"),R,V,B,true);
            ConMess(Dest,_("//\t:Cancel all the commands on sentence"),R,V,B,true);
            ConMess(Dest,_("/color Rx Gy Bz\t:Use the RGB colour"),R,V,B,true);
            ConMess(Dest,_("/chat pink\t:Use the \"pink\" named colour"),R,V,B,true);
            ConMess(Dest,_("/rejoindre \"nick\"\t:Join a person on the world"),R,V,B,true);
            ConMess(Dest,_("/global on\t:Set you tchat mode in global"),R,V,B,true);
            ConMess(Dest,_("/global off\t:Set your tchat mode in local"),R,V,B,true);
            ConMess(Dest,_("/join on\t:Enable people to join you"),R,V,B,true);
            ConMess(Dest,_("/join off\t:Disable people to join you"),R,V,B,true);
            ConMess(Dest,_("/info\t:Give you some information about your modes"),R,V,B,true);
            ConMess(Dest,_("/avat x\t:Change your avatar with the number x (0 - 255)\n\tIf you are tourist , you will be alone to see your avatar"),R,V,B,true);
            if (PSpeaker)
            {
                ConMess(Dest,_("For Public Speaker:"),51,153,51,true);
                ConMess(Dest,_("/boldon\t:Enable the permanent bold mode"),R,V,B,true);
                ConMess(Dest,_("/boldoff\t:Disable the permanent bold mode"),R,V,B,true);
                ConMess(Dest,_("/itaon\t:Enable the permanent italic mode"),R,V,B,true);
                ConMess(Dest,_("/itaoff\t:Disable the permanent italic mode"),R,V,B,true);
                ConMess(Dest,_("/titre\t:Set the nobility title (before the nick)"),R,V,B,true);
                ConMess(Dest,_("/tion\t:Enable the nobility title"),R,V,B,true);
                ConMess(Dest,_("/tioff\t:Disable the nobility title"),R,V,B,true);
                ConMess(Dest,_("/quali\t:Set the nobility quality (After the nick)"),R,V,B,true);
                ConMess(Dest,_("/quon\t:Enable the nobility quality"),R,V,B,true);
                ConMess(Dest,_("/quoff\t:Disable the nobility quality"),R,V,B,true);
            }
            if (Modo)
            {
                ConMess(Dest,_("For Moderator:"),51,153,51,true);
                ConMess(Dest,_("/muteon nom\t:Mute the user nom"),R,V,B,true);
                ConMess(Dest,_("/muteoff nom\t:Unmute the user nom"),R,V,B,true);
                ConMess(Dest,_("/listmute\t:List muted users"),R,V,B,true);
                ConMess(Dest,_("/list\t:Give number of user on the world"),R,V,B,true);
                ConMess(Dest,_("/ejectip x.x.x.x=y\t:Eject ip x.x.x.x for y hours, if =y is not specified, the ban time will be for 24 hours by default"),R,V,B,true);
                ConMess(Dest,_("/modon\t:Enable moderator mode"),R,V,B,true);
                ConMess(Dest,_("/modoff\t:Disable moderator mode"),R,V,B,true);
            }
            if (Eminent)
            {
                ConMess(Dest,_("/addcolor name Rx Gy By\t:Set a new named colour"),R,V,B,true);
                ConMess(Dest,_("/delcolor nom\t:Erase a named colour"),R,V,B,true);
                ConMess(Dest,_("/cgon\t:Conect the T'Chat streaming on xelag server"),R,V,B,true);
                ConMess(Dest,_("/cgoff\t:Disconnect the T'Chat streaming from xelagot server"),R,V,B,true);
                ConMess(Dest,_("/stat\t:Give the DB statistics"),R,V,B,true);
            }
            Reponse = _("");
            break;
        }
        if (UCase==_("/SCORE") && (!Mess_Ext))
        {
            DBase->SetQuali(id,false);
            Reponse=_("Kling results:\t1 - ");
            id=DBase->GetRankKling(1);
            Reponse.Append(DBase->GetName(id));
            Tampon.Printf(_(" with %d points.\n\t2 - "),DBase->GetNbKling(id));
            Reponse.Append(Tampon);
            id=DBase->GetRankKling(2);
            Reponse.Append(DBase->GetName(id));
            Tampon.Printf(_(" with %d points.\n\t3 - "),DBase->GetNbKling(id));
            Reponse.Append(Tampon);
            id=DBase->GetRankKling(3);
            Reponse.Append(DBase->GetName(id));
            Tampon.Printf(_(" with %d points.\nKloug results:\n\t1 - "),DBase->GetNbKling(id));
            Reponse.Append(Tampon);
            id=DBase->GetRankKloug(1);
            Reponse.Append(DBase->GetName(id));
            Tampon.Printf(_(" with %d points.\n\t2 - "),DBase->GetNbKloug(id));
            Reponse.Append(Tampon);
            id=DBase->GetRankKloug(2);
            Reponse.Append(DBase->GetName(id));
            Tampon.Printf(_(" with %d points.\n\t3 - "),DBase->GetNbKloug(id));
            Reponse.Append(Tampon);
            id=DBase->GetRankKloug(3);
            Reponse.Append(DBase->GetName(id));
            Tampon.Printf(_(" with %d points."),DBase->GetNbKloug(id));
            Reponse.Append(Tampon);
            MessBot=true;
            cmd_ok=true;
            R=51;V=153;B=51;
            break;
        }
        if (UCase==_T("/MONSCORE") && (!Mess_Ext))
        {
            Reponse.Printf(_("You have %d points for Kling.\nYou have %d points for Kloug."),
                DBase->GetNbKling(id),
                DBase->GetNbKloug(id));
            MessBot=true;
            cmd_ok=true;
            R=51;V=153;B=51;
            break;
        }
        if ((UCase.StartsWith(_T("/COLOR "), &Tampon)) && (!Mess_Ext))
        {
            if (EnaCoulPerso || PSpeaker)
            {
                Couleur=ExtColor (Tampon);
                DBase->SetColor(id, Couleur);
                R=Couleur.Red();
                V=Couleur.Green();
                B=Couleur.Blue();
                MessBot=true;
                cmd_ok=true;
                Reponse=_("You are writing on this color now.");
                break;
            }
        }
        if ((UCase.StartsWith(_("/CHAT "), &Tampon)) && (!Mess_Ext))
        {
            MessBot=true;
            cmd_ok=true;
            rc=DBColor->Identifie(Tampon);
            if (rc==DBC_NOT_EXIST)
            {
                Reponse=_("Unknown color... Try another syntax.");
                break;
            }
            Couleur=DBColor->GetColor(rc);
            DBase->SetColor(id, Couleur);
            R=Couleur.Red();
            V=Couleur.Green();
            B=Couleur.Blue();
            Message.Printf (_T(" R%d"), R);
            Original=_T("/color")+ Message;
            Message.Printf (_T(" V%d"), V);
            Original.Append (Message);
            Message.Printf (_T(" B%d"), B);
            Original.Append (Message);
            Reponse=_("You are writing on this color : ") + Tampon;
            break;
        }
        if ((UCase.StartsWith(_("/REJOINDRE "), &Tampon))&& (!Mess_Ext))
        {
            Tampon=Message.Remove (0, 11);
            MessBot=true;
            cmd_ok=true;
            if (Tampon.IsEmpty())
            {
                Reponse=_("Please check the name");
                break;
            }
            if ((rc=GetUserSession (Tampon))<1)
            {
                Reponse=_("The user ") + Tampon + _(" not exist.");
                break;
            }
            if ((!DBase->GetReachable(DBase->Search(Tampon))) && (!Modo))
            {
                Reponse=_("The user ") + Tampon + _(" don't want to be joined.");
                break;
            }
            aw_int_set (AW_TELEPORT_X, GetUserX(rc));
            aw_int_set (AW_TELEPORT_Y, GetUserY(rc));
            aw_int_set (AW_TELEPORT_Z, GetUserZ(rc));
            aw_int_set (AW_TELEPORT_YAW, 0);
            aw_string_set (AW_TELEPORT_WORLD,
            // New unicode support for AW 5.0
            #if AW_BUILD > 77
                _T(""));
            #else
                "");
            #endif
            aw_bool_set (AW_TELEPORT_WARP, 0);
            aw_teleport (Session);
            Reponse = _("You has been joined by ") + Name;
            ConMess(rc,Reponse,R,V,B,true);
            Reponse = _T("Zoom....");
            break;
        }
        if ((UCase==_("/LISTCOLOR")) && (!Mess_Ext))
        {
            MessBot=true;
            cmd_ok=true;
            CColour Colo;
            wxString NomColo;
            int NBCol = DBColor->GetNbColors ();
            ConMess(Dest,_("List of customs colours: "),251,150,38,true);
            for (int i=0 ; i < NBCol; i++)
            {
                NomColo = _T("/chat ") + DBColor->GetName (i);
                Colo = DBColor->GetColor (i);
                ConMess(Dest,NomColo,Colo.Red(),Colo.Green(),Colo.Blue(),true);
            }
            Reponse = _("End of list");
            break;
        }
        if ((UCase==_T("/GLOBAL ON"))&& (!Mess_Ext))
        {
            if (ChatBride)
            {
                Reponse=_("The Global function is disable");
                break;
            }
            DBase->SetGlobMode(id,true);
            Reponse=_("You are in global mode");
            MessBot=true;
            cmd_ok=true;
            break;
        }
        if ((UCase==_T("/GLOBAL OFF")) && (!Mess_Ext))
        {
            if (ChatBride)
            {
                Reponse=_("The global function is enable");
                break;
            }
            DBase->SetGlobMode(id,false);
            Reponse=_("You are in local mode");
            MessBot=true;
            cmd_ok=true;
            break;
        }
        if ((UCase==_T("/JOIN ON")) && (!Mess_Ext))
        {
            DBase->SetReachable(id,true);
            Reponse=_("You are joignable");
            MessBot=true;
            cmd_ok=true;
            break;
        }
        if ((UCase==_T("/JOIN OFF")) && (!Mess_Ext))
        {
            DBase->SetReachable(id,false);
            Reponse=_("You aren't joignable");
            MessBot=true;
            cmd_ok=true;
            break;
        }
        if ((UCase==_T("/INFO")) && (!Mess_Ext))
        {
            if (DBase->GetGlobMode(id))
                Reponse=_("You are speaking in global mode\n");
            else
                Reponse=_("You are speaking in local mode\n");
            if (DBase->GetReachable(id))
                Reponse.Append(_("You are joignable"));
            else
                Reponse.Append(_("You aren't joignable"));
            if (PSpeaker) Reponse.Append(_("\nYou are Public Speaker"));
            if (Modo) Reponse.Append(_("\nYou are keeper"));
            if (Eminent) Reponse.Append(_("\nYou are Eminent"));
            if (!Citoyen) Reponse.Append(_("\nYou are tourist"));
            else
            {
                Reponse.Append(_("\nYour citizen number : "));
                Reponse << Citoyen;
            }
            MessBot=true;
            cmd_ok=true;
            break;
        }
        if ((UCase.StartsWith(_("/AVAT "), &Tampon))&& (!Mess_Ext))
        {
            Tampon.ToLong(&valeure);
            rc=(int)valeure;
            if (rc<0) rc=0;
            if (rc>255) rc=255;
            aw_int_set (AW_AVATAR_TYPE, rc);
            aw_avatar_set (Session);
            Reponse << _("You choose the avatar number ") << rc;
            MessBot=true;
            cmd_ok=true;
            break;
        }
//Zone PSpeaker
        if (PSpeaker)
        {
if (UCase==_T("/BOLDON"))
            {
                DBase->SetGras(id,true);
                Reponse=_("You activated Permanent Bold.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/BOLDOFF"))
            {
                DBase->SetGras(id,false);
                Reponse=_("You desactivated Permanent Bold.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/ITAON"))
            {
                DBase->SetItalique(id,true);
                Reponse=_("You activated Permanent Italic.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/ITAOFF"))
            {
                DBase->SetItalique(id,false);
                Reponse=_("You desactivated Permanent Italic.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/TION"))
            {
                DBase->SetTitre(id,true);
                Reponse=_("You activated your nobility title.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/TIOFF"))
            {
                DBase->SetTitre(id,false);
                Reponse=_("You desactivated your nobility title.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/QUON"))
            {
                DBase->SetQuali(id,true);
                Reponse=_("You activated your nobility quality.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/QUOFF"))
            {
                DBase->SetQuali(id,false);
                Reponse=_("You desactivated your nobility quality.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase.StartsWith(_T("/TITRE ")))
            {
                Tampon=Message.Remove (0, 7);
                Tampon.Append(_T(" "));
                DBase->SetTxtTitre(id,Tampon);
                Reponse=_("You setted your nobility title.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase.StartsWith(_T("/QUALI "),&Tampon))
            {
                Tampon=Message.Remove (0, 7);
                Tampon= _T(" ") + Tampon;
                DBase->SetTxtQuali(id,Tampon);
                Reponse=_("You setted your nobility quality.");
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/STOP"))
            {
                Reponse=_("The global tchat mode is stoped.");
                BlockCG=true;
                aw_bool_set(AW_WORLD_DISABLE_CHAT,false);
                aw_world_attributes_change ();
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase==_T("/START"))
            {
                Reponse=_("The global tchat mode is run.");
                BlockCG=false;
                aw_bool_set(AW_WORLD_DISABLE_CHAT,true);
                aw_world_attributes_change ();
                MessBot=true;
                cmd_ok=true;
                break;
            }
        }
// Zone Eminent
        if (Eminent)
        {
            if (UCase.StartsWith(_T("/CITBYNUM "), &Tampon))
            {
                if (SessionReq != 0)
                {
                    Reponse = _("Please wait a moment, another work in progress");
                    MessBot=true;
                    cmd_ok=true;
                    break;
                }
                Reponse = _("Request ...");
                SessionReq = Session;
                long lReqCit;
                Tampon.ToLong(&lReqCit);
                int ReqCit = (int) lReqCit;
                aw_citizen_attributes_by_number (ReqCit);
                cmd_ok=true;
                MessBot=true;
                break;
            }
            if (UCase.StartsWith(_T("/RESET")))
            {
                DBase->RstKlingKloug();
                Reponse=_("All Kling Kloug results cleaned");
                Tampon=_("Result Erased by : ") + Name;
                wxLogMessage(Tampon);
                MessBot=true;
                cmd_ok=true;
                break;
            }
            if (UCase.StartsWith(_T("/ADDCOLOR "),&Tampon))
            {
                MessBot=true;
                cmd_ok=true;
                pos=Tampon.First(_T(' '));
                Message=Tampon.Mid (pos);
                Tampon=Tampon.Left(pos);
                Couleur=ExtColor (Message);
                rc=DBColor->Add(Tampon,Couleur);
                if (rc==DBC_ALREADY_EXIST)
                {
                    Reponse=_("This color name already exist.");
                    break;
                }
                if (rc==DBC_LIST_FULL)
                {
                    Reponse=_("The color database is full, please purge some unused colors before.");
                    break;
                }
                Reponse=_("The color has been indeed added.");
                break;
            }
            if (UCase.StartsWith(_T("/DELCOLOR "),&Tampon))
            {
                MessBot=true;
                cmd_ok=true;
                if ((rc=DBColor->Identifie(Tampon))==DBC_NOT_EXIST)
                {
                    Reponse=_("This Color doesn't exist");
                    break;
                }
                if ((DBColor->Remove(rc))==DBC_OK)
                    Reponse=_("This color has been indeed removed.");
                else
                    Reponse=_("Error in database.");
                break;
            }
            if (UCase.StartsWith(_("/CGON")))
            {
                CGON=true;
                Sauve ();
                return Original;
            }
            if (UCase.StartsWith(_("/CGOFF")))
            {
                CGON=false;
                Sauve ();
                return Original;
            }
            if (UCase==_("/STAT"))
            {
                MessBot=true;
                cmd_ok=true;
                SStats stati=DBase->Satistic();
                Reponse.Printf(_("Stat of DB:\n- %d users.\n- %d%% of Citizen.\n- %d%% of tourists.\n- Frequentation average index: %d\n- Kling Average: %d\n- Kloug Average: %d\n"),
                    stati.NbUtil,
                    stati.PCCitoyen,
                    stati.PCTouriste,
                    stati.MFreq,
                    stati.MKling,
                    stati.MKloug);
                break;
            }
            if (UCase.StartsWith(_T("/DIEU ")))
            {
                wxString Vide;
                Message=Message.Remove (0, 6);
                cmd_ok=true;
                Dieu=true;
                Dest=0;
                Diffusion.Post (Vide,GetInstance(),Message,Vide,8);
                break;
            }
        }
//Zone Modération
        if (Modo)
        {
            if (UCase==_T("/LIST"))
            {
                MessBot=true;
                cmd_ok=true;
                TellList (Dest);
                break;
            }
            if (UCase.StartsWith(_T("/MUTEON ")))
            {
                MessBot=true;
                cmd_ok=true;
                Tampon=UCase.Remove (0, 8);
                rc=DBase->Search (Tampon);
                if (rc>(-1))
                {
                    DBase->SetMuteMode (rc, true);
                    Tampon=Message.Remove (0, 8);
                    Reponse= Tampon + _(" has been muted.");
                }
                else Reponse=_("Unknown user");
                break;
            }
            if (UCase.StartsWith(_T("/MUTEOFF ")))
            {
                MessBot=true;
                cmd_ok=true;
                Tampon=UCase.Remove (0, 9);
                Reponse=_("End of list");
                wxArrayString Liste = GetUserListe();
                if (Liste.GetCount () ==0 ) Reponse=_("nobody in the world");
                else
                {
                    ConMess(Dest,_("Users list :"),R,V,B,true);
                    for (unsigned int i=0; i<Liste.GetCount (); i++)
                    {
                        ConMess(Dest,Liste[i],R,V,B,true);
                    }
                }
                rc=DBase->Search (Tampon);
                if (rc>(-1))
                {
                    DBase->SetMuteMode (rc, false);
                    Tampon=Message.Remove (0, 8);
                    Reponse= Tampon + _(" has been unmuted.");
                }
                else Reponse=_("Unknown user");
                break;
            }
            if (UCase==_T("/LISTMUTE"))
            {
                MessBot=true;
                cmd_ok=true;
                wxArrayString Lis=DBase->GetMuteList ();
                if (!Lis.GetCount()) Reponse=_("Empty list.");
                else
                {
                    Reponse = _("List of muted users:\n");
                    for (unsigned int i=0; i<Lis.GetCount(); i++)
                    {
                        Reponse.Append(Lis[i]+_T("\n"));
                    }
                }
                break;
            }
            if (UCase.StartsWith(_T("/EJECTIP ")))
            {
                MessBot=true;
                cmd_ok=true;
                unsigned int Temps=24;
                wxString Tampon2;
                Tampon=UCase.Remove (0, 9);
                if (UCase.Contains(_T("=")))
                {
                    rc=Tampon.Find (_T('='));
                    Tampon2=Tampon.AfterFirst (_T('='));
                    Tampon2.ToLong (&valeure);
                    if (valeure>0) Temps=valeure;
                    Tampon=Tampon.Remove (rc, (Tampon.Length()-rc));
                }
                if (Tampon.Length()<7)
                {
                    Reponse=_("Not a valid IP Address");
                    break;
                }
                unsigned int AdIP=inet_addr (Tampon);
                if (Tampon.Contains(_T("TEST")))
                {
                    Reponse= _("Test : IP ");
                    Reponse << Tampon << _(" has been ban during ") << Tampon2 << _(" hour(s)");
                }
                else if (AdIP >0)
                {
                    CBotCG::Ejecte=true;
                    CBotCG::IP=AdIP;
                    CBotCG::Ejector=_("Ban by ")+Name;
                    CBotCG::EjectNom="";
                    CBotCG::EjectCit=0;
                    CBotCG::EjectTime=Temps;
                    Reponse= "IP ";
                    Reponse << Tampon << _(" has been ban during ") << Temps << _(" hour(s)");
                }
                else Reponse= _("Not a valid IP Address");
                break;
            }
            if (UCase==_T("/MODON"))
            {
                MessBot=true;
                cmd_ok=true;
                DBase->SetModo( id, true);
                Reponse = _("You are on moderation mode");
                break;
            }
            if (UCase=="/MODOFF")
            {
                MessBot=true;
                cmd_ok=true;
                DBase->SetModo( id, false);
                Reponse = _("You are not on moderation mode");
                break;
            }
        }
        if (UCase.Contains("/ME/") || UCase.Contains("/ME ")) Me=true;
        if (UCase.Contains("/BOLD/") || UCase.Contains("/BOLD "))
        {
            if (PSpeaker) Bold=true;
            else if (EnaStyle) Bold=true;
        }
        if (UCase.Contains(_T("/ITA/")) || UCase.Contains(_T("/ITA ")))
        {
            if (PSpeaker) Ita=true;
            else if (EnaStyle) Ita=true;
        }
        if (Me || Bold || Ita)
        {
            pos=Message.First(_T(" "));
            Message=Message.Mid(pos+1);
            Dest=0;
            cmd_ok=true;
            break;
        }
        if (!cmd_ok)
        {
            Reponse==_("Bad Command");
            MessBot=true;
            break;
        }
        Dest=0;
        break;
    }
    if (MessBot)
    {
        if (Mess_Ext) return Original;
        Bold=true; Ita=false;
    }
    else
    {
        if (PSpeaker)
        {
            Bold=Bold || DBase->GetGras(id);
            Ita=Ita || DBase->GetItalique(id);
        }
        Ti=DBase->GetTitre(id);
        Qu=DBase->GetQuali(id);
        Titre=DBase->GetTxtTitre(id);
        Quali=DBase->GetTxtQuali(id);
        Couleur=DBase->GetColor(id);
        R=Couleur.Red();
        V=Couleur.Green();
        B=Couleur.Blue();
        Reponse=_T("");
        if (!Dieu)
        {
            if ((PSpeaker)&&(Ti)) Reponse=Titre;
            Reponse.Append(Name);
            if ((PSpeaker)&&(Qu)) Reponse.Append(Quali);
            if (Mess_Ext)
            {
                Reponse.Append(_(" on "));
                Reponse.Append(Client);
            }
            if (!Me) Reponse.Append(_T(":\t"));
            else Reponse.Append(_T(" "));
        }
        Reponse.Append(Message);
        if (Dieu)
        {
            Bold=true;
            Ita=false;
        }
        if (!Mess_Ext) wxLogMessage(Name + _T(":") + Original);
    }
    if (SetInstance ())
    {
        if (Reponse.Length() > 255) Reponse= Reponse.Truncate (255);
        if ((!Dest) && (!Mess_Ext) && (!BlockCG))
        {
            SessGlob1=DBase->GetGlobMode(id);
            for (auto User : Users)
            {
                Dist = Distance (User.Session,Session, false);
                SessGlob2=DBase->GetGlobMode(DBase->Search(GetUserName(User.Session)));
                if (ChatBride || (!SessGlob1) || (!SessGlob2))
                {
                    if (Dist < ChatDistance) ConMess(User.Session,Reponse,R,V,B,Bold,Ita);
                }
                else ConMess(User.Session,Reponse,R,V,B,Bold,Ita);
            }
        }
        else if ((!Dest) && Mess_Ext && (!BlockCG))
        {
            for (auto User : Users)
            {
                SessGlob1=DBase->GetGlobMode(DBase->Search(GetUserName(User.Session)));
                Dist = Distance (User.Session,0, true);
                if (ExtBride || (!SessGlob1))
                {
                    if	(Dist < ExtDistance) if (Dist < ChatDistance) ConMess(User.Session,Reponse,R,V,B,Bold,Ita);
                }
                else ConMess(User.Session,Reponse,R,V,B,Bold,Ita);
            }
        }
        else if (Dest && (!Mess_Ext)) ConMess(Dest,Reponse,R,V,B,Bold,Ita);
    }
    return Original;
}

//------------------------------------------------------------------------------

int CBotCG::Mess_Bot (wxString Name, int code, wxString Client, int Session)
{
    int R=255,V=100,B=100,rc=0, Dist=0;
    wxString Message=Name;
    switch (code)
    {
        case 1	: //Message d'anonnce d'arrivée
            if (!EnaAnnArr) return 0;
            Message=TxtAnnArr;
            Message.Replace (_T("%n"),Name, true);
            R=ClrAnnArr.Red();
            V=ClrAnnArr.Green();
            B=ClrAnnArr.Blue();
            break;
        case 2	: //Message d'annonce de départ
            if (!EnaAnnDep) return 0;
            Message=TxtAnnDep;
            Message.Replace (_T("%n"),Name, true);
            R=ClrAnnDep.Red();
            V=ClrAnnDep.Green();
            B=ClrAnnDep.Blue();
            break;
        case 3	:
            Message.Append (_(" connected on ChatGlobal Network")); // Annonce de connection sur le serveur global
            break;
        case 4	:
            Message.Append (_(" disconnected from ChatGlobal Network")); // Annonce de déconnexion sur le serveur global
            break;
        case 5	: //Message d'annonce d'arrivée exterieure
            if (!EnaAnnArr) return 0;
            Message.Append (_(" on "));
            Message.Append (Client);
            Message.Append (_(" incoming."));
            break;
        case 6	: //Message d'annonce de sortie Exterieur
            if (!EnaAnnDep) return 0;
            Message.Append (_(" on "));
            Message.Append (Client);
            Message.Append (_(" outgoing."));
            break;
        case 7	: // Message D'accueil
            if (!EnaMessAc) return 0;
            Message=TxtMessAc;
            Message.Replace (_T("%n"),Name, true);
            R=ClrMessAc.Red();
            V=ClrMessAc.Green();
            B=ClrMessAc.Blue();
            break;
        case 8 : // Message dieu
            R=30;
            V=120;
            B=50;
            Message=Name;
            break;
        default	:
            return 0;
    }
    if (code==8)
    {
        ConMess(0,Message,R,V,B,true,false);
        return 0;
    }
    if (SetInstance ())
    {
        if (ExtBride && (code > 2) && (code < 7))
        {
            for (auto User : Users)
            {
                Dist = Distance (User.Session,0, true);
                if	(Dist < ExtDistance)
                {
                    ConMess(Session,Message,R,V,B,true,false);
                }
            }
        }
        else ConMess(Session,Message,R,V,B,true,false);
        wxLogMessage(Message);
    }
    return 0;
}

//------------------------------------------------------------------------------

CColour CBotCG::ExtColor(const wxString &Colour)
{
    int R=0,V=0,B=0;
    CColour Color;
    int pos;
    long num;
    wxString Tampon;
    wxString Copie=Colour.Upper();
    if ((pos=Copie.Find(_T("R")))> -1 )
    {
        Tampon=Colour.Mid(pos+1,3);
        Tampon.ToLong(&num);
        R=(int)num;
    }
    if (((pos=Copie.Find(_T("V")))> -1 ) || ((pos=Copie.Find(_T("G")))> -1 ))
    {
        Tampon=Colour.Mid(pos+1,3);
        Tampon.ToLong(&num);
        V=(int)num;
    }
    if ((pos=Copie.Find(_T("B")))> -1 )
    {
        Tampon=Colour.Mid(pos+1,3);
        Tampon.ToLong(&num);
        B=(int)num;
    }
    Color.Set(R,V,B);
    return Color;
}

//------------------------------------------------------------------------------

int CBotCG::Distance(int Session1, int Session2, bool Ext)
{
    int X1=GetUserX(Session1);
    int Y1=GetUserY(Session1);
    int Z1=GetUserZ(Session1);
    int X2,Y2,Z2;
    if (Ext)
    {
        X2=ExtCoordX;
        Y2=0;
        Z2=ExtCoordZ;
    }
    else
    {
        X2=GetUserX(Session2);
        Y2=GetUserY(Session2);
        Z2=GetUserZ(Session2);
    }
    double distance=sqrt(	pow ((double)( X1-X2 ), 2 )+
                            pow ((double)( Z1-Z2 ), 2 )+
                            pow ((double)( Y1-Y2 ), 2 )
                        )/100;
    return (int) distance;
}

//------------------------------------------------------------------------------
// Console Message

void CBotCG::ConMess (int Session, wxString Message, int R, int V, int B, bool Bold, bool Ital)
{
    if (Message.Length () > 0)
    {
        aw_string_set(AW_CONSOLE_MESSAGE,
        #if AW_BUILD > 77
            Message);
        #else
            Message.mb_str());
        #endif
        aw_int_set(AW_CONSOLE_RED, R);
        aw_int_set(AW_CONSOLE_GREEN, V);
        aw_int_set(AW_CONSOLE_BLUE, B);
        aw_bool_set(AW_CONSOLE_BOLD, Bold);
        aw_bool_set(AW_CONSOLE_ITALICS, Ital);
        aw_console_message (Session);
    }
}

void CBotCG::Eject ()
{
    if (CBotCG::IP!=0)
    {
        aw_int_set (AW_EJECTION_TYPE, AW_EJECT_BY_ADDRESS);
        aw_int_set (AW_EJECTION_ADDRESS, CBotCG::IP);
        aw_int_set (AW_EJECTION_EXPIRATION_TIME, time (NULL) + CBotCG::EjectTime * 60 * 60 );
        aw_string_set (AW_EJECTION_COMMENT,
    #if AW_BUILD > 77
        CBotCG::Ejector);
    #else
        CBotCG::Ejector.mb_str());
    #endif
        if (aw_world_ejection_add ()) wxLogMessage(_("Unable process ejection by IP method"));
        else wxLogMessage (_("User ejected by IP method"));
        if (CBotCG::EjectCit>0)
        {
            aw_int_set (AW_EJECTION_TYPE, AW_EJECT_BY_CITIZEN);
            aw_int_set (AW_EJECTION_ADDRESS, CBotCG::EjectCit);
            aw_int_set (AW_EJECTION_EXPIRATION_TIME, time (NULL) + 24 * 60 * 60);
            aw_string_set (AW_EJECTION_COMMENT,
        #if AW_BUILD > 77
            CBotCG::Ejector);
        #else
            CBotCG::Ejector.mb_str());
        #endif
            if (aw_world_ejection_add ()) wxLogMessage (_("Unable process ejection by Citizen method"));
            else wxLogMessage (_("User ejected by Citizen method"));
        }
    }
}

void CBotCG::TellList (int Session)
{
    if (Users.size() ==0 ) ConMess(Session,_("Nobody on the world"),51,153,51,true);
    else
    {
        ConMess(Session,_("Users list :"),51,153,51,true);
        for (auto User : Users)
        {
            ConMess(Session,User.Name,51,153,51,true);
        }
        ConMess(Session,_("End of list"),51,153,51,true);
    }
}

void CBotCG::OnReceiveMessage(const wxString& pMessage,
                    const void* pInstance,
                    const wxString& pName,
                    const wxString& pClient,
                    int pType)
{
    if (IsOnWorld() && CGON && (pInstance!=GetInstance()))
    {
        if (!pType)
            Analyse (pName, 0,0,pMessage,0,true,pClient,0);
        else
            Mess_Bot (pName, pType, pClient);
    }
}