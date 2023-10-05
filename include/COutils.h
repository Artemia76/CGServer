// *****************************************************************************
// *                                                                           *
// *                              Outils Divers                                *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 30/06/2006                                                           *
// *   CopyRight (c)Neophile 2006                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef COUTILS_H
#define COUTILS_H

#ifndef  WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/wxprec.h>

class COutils
{
    public:
        wxString		CoordToAw	(int x=0, int z=0);
        void			AwToCoord	(int& x, int& y,wxString Coord=_T(""));
		void			AwToCoord	(double& x, double& y,double& a, wxString Coord=_T(""));
		wxString		BinToHex	(unsigned char* dat_ptr=0, size_t Longueur=0);
		size_t			HexToBin	(wxString& Data_Str, unsigned char* dat_ptr=0);
		unsigned short	HexToDec	(const char* Hexa);
};

#endif
