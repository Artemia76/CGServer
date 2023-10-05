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
// *                         DEFINITION DES MEMBRES                            *
// *                                                                           *
// *****************************************************************************

#include "Cpasspriv.h"
#include <math.h>


//------------------------------------------------------------------------------

CPassPriv* CPassPriv::PtCPassPriv = 0;

//------------------------------------------------------------------------------

CPassPriv* CPassPriv::Create()
{
	if (!PtCPassPriv) PtCPassPriv = new CPassPriv;
	return PtCPassPriv;
}

//------------------------------------------------------------------------------

void CPassPriv::Kill()
{
	if (PtCPassPriv)
	{
		delete PtCPassPriv;
		PtCPassPriv=0;
	}
}

//------------------------------------------------------------------------------

CPassPriv::CPassPriv()
{
	Serial=0xB4A51FBA;
	Masque[0]= 0x000000FF;
	Masque[1]= 0x0000FF00;
	Masque[2]= 0x00FF0000;
	Masque[3]= 0xFF000000;
	Decalage[0]= 0;
	Decalage[1]= 8;
	Decalage[2]= 16;
	Decalage[3]= 24;
}

//------------------------------------------------------------------------------

CPassPriv::~CPassPriv()
{
}

//------------------------------------------------------------------------------
// Cryptage

wxString CPassPriv::Code(const wxString& MotDePasse)
{
	char buff[3];
	size_t Long=MotDePasse.Len();
	wxString Resultat;
	int j=0;
	unsigned int temp;
	for (size_t i=0; i<Long; i++)
	{
		temp=MotDePasse[i];
		temp=temp^((Serial&Masque[j])>>Decalage[j]);
		sprintf(buff,"%02X",temp);
		Resultat << (wxChar) buff[0];
		Resultat << (wxChar) buff[1];
		j++;
		if (j>3) j=0;
	}
	return Resultat;
}

//------------------------------------------------------------------------------
// Décryptage

wxString CPassPriv::Decode(const wxString& MotDePasse)
{
	char buff[3];
	int j=0;
	unsigned int temp;
	wxString Resultat;
	size_t Long=MotDePasse.Len();
	for (size_t i=0; i<(Long/2); i++)
	{
		buff[0]=MotDePasse[i*2];
		buff[1]=MotDePasse[(i*2)+1];
		buff[2]='\0';
		temp=HexToDec(buff);
		Resultat << (wxChar)(temp^((Serial&Masque[j])>>Decalage[j]));
		j++;
		if (j>3) j=0;
	}
	return Resultat;
}
