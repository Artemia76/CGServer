// *****************************************************************************
// *                                                                           *
// *                         CONTROLEUR AW HEADER                              *
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

#ifndef CTRLAW_H
#define CTRLAW_H

#include <wx/wxprec.h>
#ifndef  WX_PRECOMP
    #include <wx/wx.h>
#endif
#include <wx/vector.h>
#include <wx/config.h>
#include <wx/timer.h>

#ifndef COMMON_DLL
    #ifdef DLL_EXPORTS
        #define COMMON_DLL WXEXPORT
    #else
        #define COMMON_DLL WXIMPORT
    #endif
#endif

#include "CBotCG.h"
#include "CDiffusion.h"
#include "global.h"

enum
{
    HEARTBEAT=wxID_HIGHEST
};

typedef enum
{
    CTRLAW_SERVER = 0,
    CTRLAW_STANDALONE
} CTRLAW_MODE;

typedef std::vector<CBotCG*> VBots;

class COMMON_DLL CCtrlAw : public wxEvtHandler
{
    public:
                            CCtrlAw (CDiffusion& pDiffusion);
                            ~CCtrlAw();
            CBotCG*			GetBot (unsigned int num);
            int				Init (bool flag, size_t NbBot=1); // On/off de la Dll
            CBotCG*			GetBotInst (void* Instance);
            void			SetMode (CTRLAW_MODE Mode=CTRLAW_SERVER);
    private:
static      CCtrlAw*        PtCCtrlAw;
            VBots			Bots;
            wxConfigBase*	pConfig;
            wxTimer*		Heart;
            CDiffusion&     Diffusion;

            bool			AwInit;
            bool			ServerMode;
            wxDateTime		Horloge;
            int				AHeure;
            bool			ShdEject;

// Evenements AW
static		void			On_Admin_Worlds_Delete	();
static		void			On_Admin_Worlds_Info	();
static		void			On_Avatar_Add			();
static		void			On_Avatar_Change		();
static		void			On_Avatar_Delete		();
static		void			On_Avatar_Click			();
static		void			On_Cell_Begin			();
static		void			On_Cell_End				();
static		void			On_Cell_Object			();
static		void			On_Chat					();
static		void			On_Console_Message		();
static		void			On_Object_Add			();
static		void			On_Object_Click			();
static		void			On_Object_Delete		();
static		void			On_Object_select		();
static		void			On_Teleport				();
static		void			On_Universe_Attributes	();
static		void			On_Url					();
static		void			On_World_Attributes		();
static		void			On_World_Disconnect		();
static		void			On_World_Info			();
static		void			On_Terrain_Begin		();
static		void			On_Terrain_Changed		();
static		void			On_Terrain_Data			();
static		void			On_Terrain_End			();
static		void			On_Universe_Disconnect	();

// Callbacks AW

static		void			On_Login				(int rc);
static		void			On_Enter				(int rc);
static		void			On_ObjDelete			(int rc);
static		void			On_Address				(int rc);
static		void			On_Citizen_Attributes	(int rc);
    protected:
            void            On_HeartBeat            (wxTimerEvent & event);
            wxDECLARE_EVENT_TABLE();
};

#endif
