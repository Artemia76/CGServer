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
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CDIFFUSION_H
#define CDIFFUSION_H

#ifndef  WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/wxprec.h>
#include <vector>

class CDiffusion;

class CDiffusionEventHandler
{
    friend CDiffusion;
protected:
virtual void    OnReceiveMessage(
                    const wxString& pMessage,
                    const void* pInstance,
                    const wxString& pName,
                    const wxString& pClient,
                    int pType)=0;

};

typedef std::vector<CDiffusionEventHandler*> Subscribers;

class CDiffusion
{
    private:
        Subscribers          m_Subscribers;

    public:
                            CDiffusion ();//Constructeur 
                            ~CDiffusion ();//Destructeur
        void				Post (
                                const wxString& pMessage,
                                const void* pInstance,
                                const wxString& pName,
                                const wxString& pClient,
                                int pType=0); // Poste un message
        void                Subscribe(CDiffusionEventHandler* pCallBack);
        void                UnSubscribe(CDiffusionEventHandler* pCallBack);
};

#endif
