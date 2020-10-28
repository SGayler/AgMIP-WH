/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:  Christian Haberbosch
 *
 *------------------------------------------------------------------------------
 *
 * Description: 
 *
 *   Sammlung der in verschiedenen Projekten benötigten Definitionen.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: xdefines.h $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:32
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * MAX_SIM_DAYS defined twice (removed).
 * 
 *   14.9.95, 18.9.95, 5.10.95, 4.11.95, 15.4.96
 *
*******************************************************************************/
 
// ANMERKUNG CH. BAUER:
// Es gibt zu viele "define" Header Files (define.h, xdefine.h, defines.h, xdefines.h)
// mit sich ueberschneidenden und doppelten Definitionen. Wird Zeit, dass hier mal
// aufgeraeumt wird, um exception faults zu reduzieren.

#ifndef _INC_EXPERTN_XDEFINES_H
#define _INC_EXPERTN_XDEFINES_H

#include "defines.h"


// **********************************************************************************
//    Programmbeeinflussende defines  (Schalter)
// **********************************************************************************

//#undef debug
//#define  Messages 0

// **********************************************************************************
//    Konstanten
// **********************************************************************************

#define  EPSILON         (float)1e-9
#define  GradToRad       (float)0.0174533
#define  MAXSCHICHT       300//100
#define  PI               3.141593

#define LOGFILE                                           
     



// **********************************************************************************
//    LongPointer und Variablen-Abkürzungen
// **********************************************************************************

#define EXP_POINTER PCHEMISTRY pCh,PCLIMATE pCl,PGRAPHIC pGr,PHEAT pHe,PLOCATION pLo,PMANAGEMENT pMa,PPLANT pPl,PSPROFILE pSo,PTIME pTi,PWATER pWa 
#define exp_p pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa


#define DECLARE_COMMON_VAR \
        int OK            = 1;\
        int iLayer        = 0;\
        char  acdummy[80];\
        LPSTR lpOut       = acdummy;\
        float DeltaT      = pTi->pTimeStep->fAct;\
        float SimTime     = pTi->pSimTime->fTimeAct;


#define SOIL_LAYERS0(x,y) (x=y, iLayer = 0);(x->pNext!=NULL)&&(iLayer < pSo->iLayers-1);(x = x->pNext, iLayer++)
#define SOIL_LAYERS1(x,y) (x=y, iLayer = 1);(x->pNext!=NULL)&&(iLayer < pSo->iLayers-1);(x = x->pNext, iLayer++)

#define SOIL2_LAYERS0(v,w,x,y) (v=w, x=y, iLayer = 0);(x->pNext!=NULL)&&(v->pNext!=NULL)&&(iLayer < pSo->iLayers-1);\
							(v = v->pNext, x = x->pNext, iLayer++)
#define SOIL2_LAYERS1(v,w,x,y) (v=w, x=y, iLayer = 1);(x->pNext!=NULL)&&(v->pNext!=NULL)&&(iLayer < pSo->iLayers-1);\
							(v = v->pNext, x = x->pNext, iLayer++)



// #define Top3Layers(x,y) x->pNext->y + x->pNext->pNext->y + x->pNext->pNext->pNext->y 
// #define Top6Layers(x,y) x->pNext->y + x->pNext->pNext->y + x->pNext->pNext->pNext->y + x->pNext->pNext->pNext->pNext->y + x->pNext->pNext->pNext->pNext->pNext->y + x->pNext->pNext->pNext->pNext->pNext->pNext->y


#endif // #ifndef _INC_EXPERTN_XDEFINES_H

/**********************************************************************************
** EOF */