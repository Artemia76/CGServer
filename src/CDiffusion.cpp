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
#include <wx/arrimpl.cpp>

CPost::CPost (wxString& Mess, void* Inst, wxString& Name, wxString& Clien, int Typ)
{
    Message=Mess;
    Instance=Inst;
    Nom=Name;
    Client=Clien;
    Type=Typ;
}

// Classe de diffusion

CDiffusion::CDiffusion ()
{
    Client=0;
    Compteur=0;
}

CDiffusion::~CDiffusion ()
{
}

void CDiffusion::Post (wxString& Message, void* Instance, wxString& Name, wxString& Clien, int Typ)
{
    Stack.push_back (CPost(Message, Instance, Name, Clien, Typ));
}

bool CDiffusion::Get (wxString& Message, void* Instance, wxString& Name, wxString& Clien, int& Typ)
{
    Message="";
    if (Client!=Instance)
    {
        Client=Instance;
        Compteur=0;
    }
    if (Compteur >= Stack.size()) return false;
    while ((Stack[Compteur].Instance==Client) && (Compteur < Stack.size()))
    {
        Compteur ++;
        if (Compteur >= Stack.size()) return false;
    }
    Message = Stack[Compteur].Message;
    Name=Stack[Compteur].Nom;
    Clien=Stack[Compteur].Client;
    Typ=Stack[Compteur].Type;
    Compteur++;
    return true;
}

void CDiffusion::Clear ()
{
    Stack.clear ();
    Client=0;
    Compteur=0;
}
