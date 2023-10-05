// *****************************************************************************
// *                                                                           *
// *                           CODEC MOT DE PASSES                             *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 22/04/2005                                                           *
// *   CopyRight (c)Neophile 2005                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

using namespace std;

#ifndef CPASSPRIV_H
#define CPASSPRIV_H

#ifndef  WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/wxprec.h>

#include "COutils.h"

class CPassPriv : public COutils
{
	private:
						CPassPriv();
						~CPassPriv();
static	CPassPriv*		PtCPassPriv;
		unsigned int	Serial;
		unsigned int	Masque[4];
		int				Decalage[4];

	public:
static	CPassPriv*		Create();
static	void			Kill();
		wxString		Code(const wxString& MotDePasse);
		wxString		Decode(const wxString& MotDePasse);
};

#endif
