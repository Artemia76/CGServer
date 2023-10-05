// ****************************************************************************
// *                                                                          *
// *             CSingleTon : Classe de base pour un SingleTon                *
// *                                                                          *
// *                      Neophile (c) 2007 : 31/03/2007                      *
// *                                                                          *
// ****************************************************************************
// *                                                                          *
// * Rev A:Neophile:31/03/2007;Description :                                  *
// * Création classe de base support pour la réalisation des singleton        *
// * Attention , ne marche pas bien avec les classe dérivé de classes wx      *
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
    // Détruit l'instance unique de la classe
    //----------------------------------------------------------------
    static	void	Kill()
    {
        delete Inst;
        Inst = NULL;
    }

protected :

    //----------------------------------------------------------------
    // Constructeur par défaut
    //----------------------------------------------------------------
    CSingleTon() {}

    //----------------------------------------------------------------
    // Destructeur
    //----------------------------------------------------------------
    ~CSingleTon() {}

private :

    //----------------------------------------------------------------
    // Données membres
    //----------------------------------------------------------------
    static	T*	Inst; // Instance de la classe

    //----------------------------------------------------------------
    // Copie interdite
    //----------------------------------------------------------------
    CSingleTon(CSingleTon&);
    void operator =(CSingleTon&);
};

//----------------------------------------------------------------
// Déclaration de notre variable statique
//----------------------------------------------------------------
template <class T> T* CSingleTon<T>::Inst = NULL;

#endif
