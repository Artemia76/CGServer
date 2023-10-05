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
// *   Modifié le 07/09/2007                                                   *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                    DEFINITION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#include "CDiffusion.h"
#include <wx/arrimpl.cpp>

WX_DEFINE_OBJARRAY(APost);

CPost::CPost (wxString& Mess, void* Inst, wxString& Name, wxString& Clien, int Typ)
{
	Message=Mess;
	Instance=Inst;
	Nom=Name;
	Client=Clien;
	Type=Typ;
}

// Classe de diffusion

CDiffusion* CDiffusion::PtCDiffusion=0;

CDiffusion::CDiffusion ()
{
	Client=0;
	Compteur=0;
}

CDiffusion::~CDiffusion ()
{
}

CDiffusion* CDiffusion::Create ()
{
	if (!CDiffusion::PtCDiffusion) CDiffusion::PtCDiffusion = new CDiffusion ();
	return CDiffusion::PtCDiffusion;
}

void CDiffusion::Kill ()
{
	if (CDiffusion::PtCDiffusion) delete CDiffusion::PtCDiffusion;
	CDiffusion::PtCDiffusion=0;
}

void CDiffusion::Post (wxString& Message, void* Instance, wxString& Name, wxString& Clien, int Typ)
{
	Stack.Add (CPost(Message, Instance, Name, Clien, Typ));
}

bool CDiffusion::Get (wxString& Message, void* Instance, wxString& Name, wxString& Clien, int& Typ)
{
	Message="";
	if (Client!=Instance)
	{
		Client=Instance;
		Compteur=0;
	}
	if (Compteur >= Stack.Count()) return false;
	while ((Stack[Compteur].Instance==Client) && (Compteur < Stack.Count()))
	{
		Compteur ++;
		if (Compteur >= Stack.Count()) return false;
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
	Stack.Clear ();
	Client=0;
	Compteur=0;
}
