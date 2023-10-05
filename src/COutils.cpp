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
// *                    DEFINITION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************


#include "COutils.h"


//------------------------------------------------------------------------------
// Convertion de coordon�es num�riques en coordonn�es AW

wxString COutils::CoordToAw (int x, int z)
{
	wxString Coord,S;
	if (z>=0) S.Printf ("%dN", (int)floor(z/1000));
	else S.Printf ("%dS", (int)(z/(-1000))+1);
	Coord= S;
	if (x>=0) S.Printf ("%dW", (int)floor(x/1000));
	else S.Printf ("%dE", (int)floor(x/(-1000))+1);
	Coord.Append(S);
	return Coord;
}

//-----------------------------------------------------------------------------
// Convert a String Coord AW (like 0N 0W) to absolute integer x y coord for SDK
// ToDo : Add alt and YAW support

void COutils::AwToCoord (int& x, int& y,wxString Coord)
{
	Coord.UpperCase ();
	long num;
	x=0;
	y=0;
	wxString First=Coord.BeforeFirst(' ');
	wxString Sec=Coord.AfterFirst(' ');
	First.ToLong (&num);
	if (First.Contains(_T("N")))
	{
		y=(int)num;
	}
	else if (First.Contains(_T("S")))
	{
		y=(int)(-num);
	}
	Sec.ToLong (&num);
	if (Sec.Contains(_T("W")))
	{
		x=num;
	}
	else if (Sec.Contains(_T("E")))
	{
		x=(int)(-num);
	}
}

//-----------------------------------------------------------------------------
// Convert a String Coord AW (like 0N 0W 0a) to absolute double x y a coord for SDK
// ToDo : Add YAW support

void COutils::AwToCoord	(double& x, double& y,double& a, wxString Coord)
{
	Coord.UpperCase ();
	int First=0;
	int Sec=0;
	bool LatS=true,LonS=true;
	wxString Lat=_T("0"),Lon=_T("O"),Alt=_T("0");
	if (Coord.Contains (_T("N")))
	{
		First=Coord.Find(_T('N'));
		LatS=true;
	}
	else if (Coord.Contains (_T("S")))
	{
		First=Coord.Find(_T('S'));
		LatS=false;
	}
	if (Coord.Contains (_T("W")))
	{
		Sec=Coord.Find(_T('W'));
		LonS=true;
	}
	else if (Coord.Contains (_T("E")))
	{
		Sec=Coord.Find(_T('E'));
		LonS=false;
	}
	if ((First>0) && (Sec>First))
	{
		Lat=Coord.Mid (0,First);
		Lon=Coord.Mid ((First+1),(Sec-First-1));
		Alt=Coord.Mid ((Sec+1),wxString::npos);
	}
	Lat.ToDouble (&y);
	if (!LatS) y = -y;
	Lon.ToDouble (&x);
	if (!LonS) x = -x;
	Alt.ToDouble (&a);
}

//------------------------------------------------------------------------------
// Convert a String Hexadecimal Number to Integer value (16 bits max)

unsigned short COutils::HexToDec(const char* Hexa)
{
	unsigned short valeur=0,res=0;
	int i=0;
	int Long=strlen (Hexa);
	if ((Long==0)||(Long>4)) return 0;
	for (i=0;i<Long;i++)
	{
		if ((Hexa[i]>='0')&&(Hexa[i]<='9')) valeur=Hexa[i]-48;
		else if ((Hexa[i]>='A')||(Hexa[i]<='F')) valeur=Hexa[i]-55;
		else if ((Hexa[i]>='a')||(Hexa[i]<='f')) valeur=Hexa[i]-87;
		else valeur=0;
		valeur=valeur*(unsigned short)(pow(16,(double)((Long-1)-i)));
		res+=valeur;
	}
	return res;
}

//-----------------------------------------------------------------------------
// Convert a buffer to a string hexadecimal notation

wxString COutils::BinToHex	(unsigned char* dat_ptr, size_t Longueur)
{
	wxString Resultat('0',Longueur*2);
	char Tampon [3];
	for (size_t i=0; i<Longueur; i++)
	{
		sprintf (Tampon,"%02X", dat_ptr[i]);
		Resultat[i*2]=Tampon[0];
		Resultat[(i*2)+1]=Tampon[1];
	}
	return Resultat;
}

//-----------------------------------------------------------------------------
// Convert String in hexadecimal notation to binary buffer and return the length

size_t COutils::HexToBin (wxString& Data_Str, unsigned char* dat_ptr)
{
	char Tampon[3];
	size_t Longueur=Data_Str.Len()/2;
	for (size_t i=0 ; i<Longueur ; i++)
	{
		Tampon[0]=Data_Str[(i*2)];
		Tampon[1]=Data_Str[(i*2)+1];
		Tampon[2]='\0';
		dat_ptr[i]=HexToDec(Tampon);
	}
	return Longueur;
}