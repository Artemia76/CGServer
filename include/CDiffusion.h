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

class CPost
{
    public:
                            CPost (wxString& Mess, void* Inst, wxString& Name, wxString& Clien, int Typ=0);
        wxString			Message;
        void*				Instance;
        wxString			Nom,Client;
        int					Type;
};

typedef std::vector<CPost>  VPosts;

class CDiffusion
{
    private:
        VPosts              Stack;//Pile de récèption des messages
        void*				Client;
        size_t				Compteur;
    public:
                            CDiffusion ();
                            ~CDiffusion ();
        void				Post (wxString& Message, void* Instance, wxString& Name, wxString& Clien, int Typ=0); // Poste un message
        bool				Get (wxString& Message, void* Instance, wxString& Name, wxString& Clien, int& Typ); // Récupere les message de diffusion (renvoi true si dernier message)
        void				Clear ();
};

#endif
