// *****************************************************************************
// *                                                                           *
// *                        CLASS CLIENT IRC Gateway                           *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 28/10/2023                                                           *
// *   CopyRight (c)Neophile 2023                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DEFINITION DE LA CLASSE PRINCIPALE                      *
// *                                                                           *
// *****************************************************************************

#include "CIRCGateway.h"
#include "global.h"
#include <wx/tokenzr.h>

wxBEGIN_EVENT_TABLE(CIRCGateway, wxEvtHandler)
    EVT_SOCKET (IRC_SOCKET_ID, CIRCGateway::OnIRCEvent)
    EVT_TIMER  (IRC_RECO,   CIRCGateway::OnIRCRecoEvent)
    EVT_TIMER (IRC_HB, CIRCGateway::OnIRCHeartBeat)
wxEND_EVENT_TABLE()

//------------------------------------------------------------------------------
// Constructeur

CIRCGateway::CIRCGateway (CDiffusion& pDiffusion) : Diffusion(pDiffusion)
{
// Configuration du Socket TCP
    SockIRC = new wxSocketClient ();
    SockIRC->SetEventHandler(*this, IRC_SOCKET_ID);
    SockIRC->SetNotify(
        wxSOCKET_CONNECTION_FLAG |
        wxSOCKET_INPUT_FLAG |
        wxSOCKET_LOST_FLAG
    );
    SockIRC->SetTimeout (5);
    SockIRC->Notify(true);
    IRCRecoTimer = new wxTimer(this, IRC_RECO);
    IRCHeartBeat = new wxTimer(this, IRC_HB);
    PassPriv = CPassPriv::Create();

    On_IRC=false;
    ConEC=false;
    DemCon=false;
    FirstCon=false;
    IRCCompt=0;
    IRCCGEna=false;
    IRCRecoCnt=0;
    ModeReco=false;
    TamponTab.Empty();
}

//------------------------------------------------------------------------------
// Destructeur

CIRCGateway::~CIRCGateway ()
{
}

void CIRCGateway::Init (bool pFlag)
{
    if (pFlag)
    {
        Charge();
        if (IRCConAuto)
        {
            if (IRCRecoEna)
            {
                IRCRecoCnt=0;
                IRCRetente= IRCRecoDelay;
                ModeReco = true;
            }
            Connect();
        }
        IRCHeartBeat->Start(Base_Temps, false);
    }
    else
    {
        if (On_IRC)
        {
            IRCRecoTimer->Stop();
            IRCHeartBeat->Stop();
            SockIRC->Close();
        }
        Sauve();
    }
}

//------------------------------------------------------------------------------
// Charge les Paramètres du bot

void CIRCGateway::Charge ()
{
    wxString s,S;
    pConfig = wxConfigBase::Get();
    S.Printf(_T("/IRCGateway"));
    pConfig->Read( S + _T("/IRCHost"), &IRCHost, _T("localhost"));
    pConfig->Read( S + _T("/IRCPassWord"), &s, _T(""));
    IRCPassWord=PassPriv->Decode(s);
    pConfig->Read( S + _T("/IRCPort") ,&IRCPort, 5525);
    pConfig->Read( S + _T("/IRCLogin") ,&IRCLogin, _T("ChatGlobal"));
    pConfig->Read( S + _T("/IRCAutoConnect") ,&IRCConAuto, false);
    pConfig->Read( S + _T("/IRCDelai"), &IRCRecoDelay, 15);
    pConfig->Read( S + _T("/IRCEssais"), &IRCRecoRetry, 3);
    pConfig->Read( S + _T("/IRCCGEna"), &IRCCGEna, false);

    if (IRCRecoRetry != 0) IRCRecoEna=true;
    else IRCRecoEna=false;
}

//------------------------------------------------------------------------------
// Sauvegarde les paramètres du bot

void CIRCGateway::Sauve ()
{
    wxString S;
    S.Printf(_T("/IRCGateway"));
    pConfig->Write( S + _T("/IRCHost") , IRCHost);
    pConfig->Write( S + _T("/IRCPassWord") , PassPriv->Code(IRCPassWord));
    pConfig->Write( S + _T("/IRCPort") , IRCPort);
    pConfig->Write( S + _T("/IRCLogin") , IRCLogin);
    pConfig->Write( S + _T("/IRCAutoConnect") , IRCConAuto);
    pConfig->Write( S + _T("/IRCDelai") , IRCRecoDelay);
    pConfig->Write( S + _T("/IRCEssais") , IRCRecoRetry);
    pConfig->Write( S + _T("/IRCCGEna") , IRCCGEna);
    pConfig->Flush(true);
}

//------------------------------------------------------------------------------
// Le socket client reçois un evenement

void CIRCGateway::OnIRCEvent (wxSocketEvent& event)
{
    char buf1[1000];
    wxString Message=_T("");
    wxString Tampon,Name,Client,Reponse;
    wxStringTokenizer tokenizer;
    switch (event.GetSocketEvent())
    {
        case wxSOCKET_INPUT      :
            SockIRC->Read(buf1, sizeof(buf1));
            Message << wxString(buf1,wxConvISO8859_1);
            Message=Message.Truncate(SockIRC->LastCount());
            tokenizer.SetString(Message, "\n");
            while (tokenizer.HasMoreTokens())
            {
                TamponTab.Add(tokenizer.GetNextToken());
                // process token here
            }
            break;
        case wxSOCKET_OUTPUT :
            break;
        case wxSOCKET_CONNECTION :
            wxLogMessage (_("Connected on the remote host"));
            On_IRC = true;
            ConEC = false;
            FirstCon = false;
            ModeReco = false;
            IRCCompt = 0;
            if (IRCCGEna)
            {
                IRCCGEna = false;
            }
            if (IRCRecoTimer->IsRunning()) IRCRecoTimer->Stop();
            EnvoiMess(_T("HELLO"));
            EnvoiMess(_T("NICK ")+ IRCLogin);
            EnvoiMess(_T("USER ") + IRCLogin + _T(" 8 * :ChatGlobal IRC Client\n"));
            break;
        case wxSOCKET_LOST :
            Message=_("IRC Server Connection lost.Reason : ");
            switch (SockIRC->LastError())
            {
                case wxSOCKET_NOERROR :
                    Message.Append (_("No Error."));
                    break;
                case wxSOCKET_INVOP :
                    Message.Append (_("Invalid operation."));
                    break;
                case wxSOCKET_IOERR :
                    Message.Append (_("Input/Output error."));
                    break;
                case wxSOCKET_INVADDR :
                    Message.Append (_("Invalid address."));
                    break;
                case wxSOCKET_INVSOCK :
                    Message.Append (_("Invalid socket."));
                    break;
                case wxSOCKET_NOHOST :
                    Message.Append (_("No corresponding host."));
                    break;
                case wxSOCKET_INVPORT :
                    Message.Append (_("Invalid port."));
                    break;
                case wxSOCKET_WOULDBLOCK :
                    Message.Append (_("Timeout."));
                    break;
                case wxSOCKET_TIMEDOUT :
                    Message.Append (_("TimeOut Expired."));
                    break;
                case wxSOCKET_MEMERR :
                    Message.Append (_("Memory exhausted."));
                    break;
                default :
                    Message.Append (_("Unknown."));
            }
            wxLogMessage(Message);
            On_IRC=false;
            ConEC=false;
            FirstCon=false;
            TamponTab.Empty();
            IRCHeartBeat->Stop();
            SockIRC->Close();
            if (!FirstCon)
            {
                if (IRCRecoEna)
                {
                    if (!ModeReco)
                    {
                        IRCRecoCnt=0;
                        IRCRetente=IRCRecoDelay;
                        ModeReco=true;
                    }
                    Tentative();
                }
            }
            break;
        default :
            break;
    }
}

//------------------------------------------------------------------------------

bool CIRCGateway::IsOnIRC ()
{
    return On_IRC;
}

//------------------------------------------------------------------------------
// Methode de connection

void CIRCGateway::Connect()
{
    if ((!ConEC) && (!DemCon) && (!On_IRC))
    {
        DemCon=true;
        FirstCon=true;
    }
}

//----------------------------------------------SockXlg--------------------------------
// Methode de Déconnection

void CIRCGateway::Deconnect()
{
    if ((!ConEC) && (!DemCon) && On_IRC) IRCConnect(false);
}

//------------------------------------------------------------------------------
// Connection du socket client

void CIRCGateway::IRCConnect (bool choix)
{
    wxIPV4address addr;
    if (choix && (!On_IRC) && (!ConEC))
    {
        ConEC=true;
        DemCon=false;
        addr.Hostname(IRCHost);
        addr.Service(IRCPort);
        SockIRC->Connect(addr, false);
        wxLogMessage(_("IRC Gateway : Connecting to IRC server..."));
    }
    else
    {
        if (On_IRC)
        {
            wxLogMessage(_("IRC Gateway : Disconnected from IRC server"));
            On_IRC=false;
        }
        ConEC=false;
        FirstCon=false;
        TamponTab.Empty();
        IRCHeartBeat->Stop();
        SockIRC->Close();
    }
}

//------------------------------------------------------------------------------
// Méthode de reconnection automatique au serveur Xelagot

void CIRCGateway::OnIRCRecoEvent (wxTimerEvent& WXUNUSED(event))
{
    IRCConnect (true);
}

//------------------------------------------------------------------------------
// Méthode de reconnection automatique au serveur Xelagot

void CIRCGateway::OnIRCHeartBeat(wxTimerEvent& WXUNUSED(event))
{
    Update();
}


//------------------------------------------------------------------------------
// Tentatives de nouvelle connection

void CIRCGateway::Tentative ()
{
    wxString Tampon;
    if ((IRCRecoCnt<IRCRecoRetry) || (IRCRecoRetry < 0))
    {
        Tampon.Printf(_("A reconnection will be tryed in %d s."), IRCRetente);
        wxLogMessage(_T("IRC Gateway : ") + Tampon);
        IRCRecoTimer->Start(IRCRetente * 1000,wxTIMER_ONE_SHOT);
        if (IRCRecoRetry > (-1)) IRCRecoCnt++;
        if (IRCRetente < 900) IRCRetente = IRCRetente * 2;
    }
    else
    {
        wxLogMessage(_("IRC Gateway : End of attempts"));
        ModeReco=false;
    }
}

//------------------------------------------------------------------------------
// Envoie de Messages sur le reseau Xelagot

void CIRCGateway::EnvoiMess(wxString Message, int Type, wxString Name)
{
    wxString Reponse;
    if (On_IRC)
    {
        Reponse = Message + _T("\n");
        wxLogMessage(_("IRC Gateway Send Message : ") + Reponse);
        SockIRC->Write(Reponse, Reponse.Len());
    }
}

//------------------------------------------------------------------------------
// Traitement de la file d'attente

void CIRCGateway::Update()
{
    int len;
    long num;
    wxString Message=_T("");
    wxString Tampon,Name,Client,Reponse;
    while (!TamponTab.IsEmpty())
    {
        Message =TamponTab[0];
        TamponTab.RemoveAt (0);
        wxLogMessage(_("IRC Gateway Receive Message: ") + Message);
        if (Message.StartsWith(_T("PING ")))
        {
            Message.StartsWith(_T("PING "), &Tampon);
            EnvoiMess(_T("PONG ") + Tampon);
        }
        if (Message.StartsWith(_T(":")+ IRCLogin))
        {
            EnvoiMess(_T("JOIN #lobby"));
        }
    }
    if (DemCon && (!On_IRC) && (!ConEC)) IRCConnect(true);
    if (IRCRecoTimer->IsRunning() && (!ModeReco)) IRCRecoTimer->Stop();
}
