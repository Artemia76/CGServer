// *****************************************************************************
// *                                                                           *
// *                       CLASSE DE GESTION DU CIEL	                       *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 27/01/2007                                                           *
// *   CopyRight (c)Neophile 2007                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CIEL_H
#define CIEL_H

#ifndef  WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/wxprec.h>
#include <wx/fileconf.h>

#include "CColour.h"

class CCiel
{
	private :
	wxFileConfig*	pConfig;
	void			CalcCoul (CColour& C1, CColour& C2, CColour& Re, int hd, int hf , int h);
	public :
					CCiel (int id);
					~CCiel ();
	int				Top;
	int				hdl,mdl;
	int				hfl,mfl;
	int				hdc,mdc;
	int				hfc,mfc;
	wxString		Soleil;
	wxString		MSoleil;
	int				TSoleil;
	wxString		Lune;
	wxString		MLune;
	int				TLune;
	CColour		NN,NS,NO,NE,NZ,ND,NA,NL;
	CColour		AN,AS,AO,AE,AZ,AD,AA,AL;
	CColour		JN,JS,JO,JE,JZ,JD,JA,JL;
	CColour		CN,CS,CO,CE,CZ,CD,CA,CL;
	bool			Enabled;
	void 			Sauve ();
	void 			MAJ();
};

#endif
