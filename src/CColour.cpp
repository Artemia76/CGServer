// *****************************************************************************
// *                                                                           *
// *                             CLASS CCOLOUR                                 *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *   Ecrit par Gianni Peschiutta {Neophile}                                  *
// *   Le 02/11/2007                                                           *
// *   CopyRight (c)Neophile 2007                                              *
// *                                                                           *
// *****************************************************************************
// *                                                                           *
// *                    DEFINITION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#include "CColour.h"

CColour::CColour (int Red, int Green, int Blue)
{
    Set (Red, Green, Blue);
}

CColour::CColour (const wxString& Color)
{
    Set (Color);
}

CColour::~CColour ()
{
}

void CColour::Set (int Red, int Green, int Blue)
{
    R=(unsigned char)Red;
    G=(unsigned char)Green;
    B=(unsigned char)Blue;
}

void CColour::Set (const wxString& Color)
{
    wxString Temp;
    if ((Color[0]=='#') && (Color.Length()==7))
    {
        Temp=Color.Mid (1,2);
        R=HexToDec (Temp.c_str ());
        Temp=Color.Mid (3,2);
        G=HexToDec (Temp.c_str ());
        Temp=Color.Mid (5,2);
        B=HexToDec (Temp.c_str ());
    }
}

wxString CColour::GetHTML ()
{
    wxString Temp;
    Temp.Printf ("#%02X%02X%02X", R,G,B);
    return Temp;
}

int CColour::Red ()
{
    return R;
}

int CColour::Green ()
{
    return G;
}

int CColour::Blue ()
{
    return B;
}

CColour& CColour::operator= (const CColour& Color)
{
    R=Color.R;
    G=Color.G;
    B=Color.B;
    return *this;
}

CColour::CColour (const CColour& Color)
{
    R=Color.R;
    G=Color.G;
    B=Color.B;
}