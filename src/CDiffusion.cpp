// *****************************************************************************
// *                                                                           *
// *                            CLASS DIFFUSION                                *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 03/11/2007                                                           *
// *   CopyRight (c)Neophile 2005-2007                                         *
// *                                                                           *
// *   Modifi� le 07/09/2007                                                   *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                    DEFINITION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#include "CDiffusion.h"
#include <algorithm>

// Classe de diffusion

CDiffusion::CDiffusion ()
{

}

CDiffusion::~CDiffusion ()
{

}

void CDiffusion::Post (const wxString& pMessage, const void* pInstance, const wxString& pName, const wxString& pClient, int pType)
{
    wxLogMessage(wxString::Format(_T("Diffusion Subscriber num: %d"),m_Subscribers.size()));
    for (auto Subscriber : m_Subscribers)
    {
        if (Subscriber != nullptr)
        {
            wxLogMessage(_("Diffusion Post : ") + pMessage);
            Subscriber->OnReceiveMessage(pMessage,pInstance,pName,pClient,pType);
        }
    }
}

void CDiffusion::Subscribe (CDiffusionEventHandler* pCallBack)
{
    for (auto Subscriber : m_Subscribers)
    {
        if (Subscriber == pCallBack) return;
    }
    m_Subscribers.push_back(pCallBack);
}

void CDiffusion::UnSubscribe (CDiffusionEventHandler* pCallBack)
{
    Subscribers::iterator it;
    it = std::find(m_Subscribers.begin(), m_Subscribers.end(),pCallBack);
    if (it!=m_Subscribers.end())
    {
        m_Subscribers.erase(it);
    }
}