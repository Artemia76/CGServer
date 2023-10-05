// *****************************************************************************
// *                                                                           *
// *                               CLASS USER                                  *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 25/03/2006                                                           *
// *   CopyRight (c)Neophile 2005-2006                                         *
// *                                                                           *
// *   Modifié le 07/09/2006                                                   *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                         DEFINITION DES MEMBRES                            *
// *                                                                           *
// *****************************************************************************

#include "CUser.h"

CUser::CUser
		(
			wxString pName,
			int pSess,
			int pCit,
			int pPriv,
			wxString pIP
		)
{
	Name=pName;
	Session=pSess;
	Citizen=pCit;
	Privilege=pPriv;
	IP=pIP;
	Gras=false;
	Italique=false;
	Titre=false;
	Quali=false;
	Marked=false;
	Freq=0;
	Kling=0;
	Kloug=0;
	TexteTi="";
	TexteQu="";
	X=0;
	Y=0;
	Z=0;
	Yaw=0;
	Mute=false;
    Avatar=0;
    Modo=false;
}

