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
// *                   DECLARATION DE LA CLASSE PRINCIPALE                     *
// *                                                                           *
// *****************************************************************************

#ifndef CCOLOUR_H
#define CCOLOUR_H

#ifndef  WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/wxprec.h>

#include "COutils.h"

class CColour : public COutils
{
    public:
                        CColour
                        (
                            int Red=0,
                            int Green=0,
                            int Blue=0
                        );
                        CColour (const wxString& Color);
                        CColour (const CColour& Color);
                        ~CColour ();

        void            Set
                        (
                            int Red=0,
                            int Green=0,
                            int Blue=0
                        );
        void            Set (const wxString& Color);
        wxString        GetHTML ();
        CColour&        operator=  (const CColour& Color);
        int            Red ();
        int            Green ();
        int            Blue ();

    private:

        unsigned char   R,G,B;

};

#endif
