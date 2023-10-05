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
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CUSER_H
#define CUSER_H

#include <wx/wxprec.h>
#ifndef  WX_PRECOMP
	#include <wx/wx.h>
#endif
#include <vector>

#include "CColour.h"

#ifndef COMMON_DLL
	#ifdef DLL_EXPORTS
		#define COMMON_DLL WXEXPORT
	#else
		#define COMMON_DLL WXIMPORT
	#endif
#endif

class COMMON_DLL CUser
{
	public:
						CUser
						(
							wxString Name=_T(""),
							int Sess=0,
							int Cit=0,
							int Priv=0,
							wxString ip=_T("0.0.0.0")
						);
		wxString 		Name;
		int	            Session;
		int             Citizen;
		int             Privilege;
		wxString		IP;
		bool			Gras;
		bool			Italique;
		bool			Titre;
		bool			Quali;
		bool			Marked;
		bool			Glob_Mode;
		bool			Reachable;
		int				Freq;
		int				Kling;
		int				Kloug;
		wxString		TexteTi;
		wxString		TexteQu;
		CColour			Couleur;
		int				X;
		int				Y;
		int				Z;
		int             Yaw;
        int             Avatar;
		bool            Mute;
        bool            Modo;
};

typedef std::vector<CUser> VUsers;

#endif
