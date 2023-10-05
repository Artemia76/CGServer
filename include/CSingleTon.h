// ****************************************************************************
// *                                                                          *
// *             CSingleTon : Classe de base pour un SingleTon                *
// *                                                                          *
// *                      Neophile (c) 2007 : 31/03/2007                      *
// *                                                                          *
// ****************************************************************************
// *                                                                          *
// * Rev A:Neophile:31/03/2007;Description :                                  *
// * Cr�ation classe de base support pour la r�alisation des singleton        *
// * Attention , ne marche pas bien avec les classe d�riv� de classes wx      *
// *                                                                          *
// ****************************************************************************

#ifndef CSINGLETON_H
#define CSINGLETON_H


#include <wx/wxprec.h>
#ifndef  WX_PRECOMP
#include <wx/wx.h>
#endif

template <class T> class CSingleTon
{
public :

    //----------------------------------------------------------------
    // Renvoie l'instance unique de la classe
    //----------------------------------------------------------------
    static	T*	Create()
    {
        if (!Inst)
            Inst = new T;

        return Inst;
    }

    //----------------------------------------------------------------
    // D�truit l'instance unique de la classe
    //----------------------------------------------------------------
    static	void	Kill()
    {
        delete Inst;
        Inst = NULL;
    }

protected :

    //----------------------------------------------------------------
    // Constructeur par d�faut
    //----------------------------------------------------------------
    CSingleTon() {}

    //----------------------------------------------------------------
    // Destructeur
    //----------------------------------------------------------------
    ~CSingleTon() {}

private :

    //----------------------------------------------------------------
    // Donn�es membres
    //----------------------------------------------------------------
    static	T*	Inst; // Instance de la classe

    //----------------------------------------------------------------
    // Copie interdite
    //----------------------------------------------------------------
    CSingleTon(CSingleTon&);
    void operator =(CSingleTon&);
};

//----------------------------------------------------------------
// D�claration de notre variable statique
//----------------------------------------------------------------
template <class T> T* CSingleTon<T>::Inst = NULL;

#endif
