/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:      Sperr Chr.
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Priesack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Simulation time objects
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: xwater.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:09
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 04.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XWATER_H
#define _INC_EXPERTN_XWATER_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct wLayer    *PWLAYER;
typedef struct wLayer
	{
	float	 fContInit;
	float	 fContMax;
	float	 fContAct;
	float	 fContOld;
	float	 fMatPotAct;
	float	 fMatPotOld;
	float	 fContMobAct;
	float	 fContMobOld;
	float	 fContImmAct;
	float	 fContImmOld;
	float	 fOsmPot;
	float	 fFluxDens;
	float    fFluxDensOld;
	float	 fFlux;
	float	 fHydrCond;
	float	 fSoilAirVol;
	float	 fDiffCap;
	float	 fMaxEvapFD;
	float    fIce;
	float    fIceOld;
	float    fWaterFreezeR;
	float    fWaterFreezeDay;
	float	 fFluxDay;
	float	 fFluxCum;
	float  	 fLatFlowR;
	float	 fLatFlowDay;
	PWLAYER	pNext;
	PWLAYER	pBack;
	}
	WLAYER;


typedef struct wMeasure   *PWMEASURE;
typedef struct wMeasure
	{ 
	long	 lDate;     
	int		 iDay;
	float	 afDepth[MAX_DEPTH_PER_MEASURE];
	float	 afContent[MAX_DEPTH_PER_MEASURE];
	float	 afFluxDens[MAX_DEPTH_PER_MEASURE];
	PWMEASURE pNext;
	PWMEASURE pBack;
	}
	WMEASURE;


typedef struct Evap   *PEVAP;
typedef struct Evap
	{ 
	float	 fCumStage1;
	float	 fCumStage2;
	float	 fTimeStage2;
	float    fPotDay;
	float    fPotR;
	float	 fActDay;
	float	 fActR;
	float	 fMax;
	float	 fMaxR;
	float    fCropBasalIni;
	float    fCropBasalMid;
	float    fCropBasalEnd;
    float    fCropTimeIni;
    float    fCropTimeMid;
    float    fCropTimeEnd;
    float    fCropTimeDev;
	}
	EVAP;


typedef struct wBalance   *PWBALANCE;
typedef struct wBalance
	{ 
	float	 fInput;            
	float	 fOutput;           
	float	 fProfil;           
	float    fProfileIce;
	float	 fProfilStart;
	float	 fBalance;
    float    fPotETDay;
    float    fPotEVDay;
    float    fActEVDay;
	float	 fPotCumEvap;
	float	 fActCumEvap;
	float	 fPotTranspDay;
	float	 fActTranspDay;
	float	 fPotCumTransp;
	float	 fActCumTransp;
	float    fCumInterception;
	float    fReservoir;
	float    fSnowStorage;
	float    fSnowWaterContent;
	float	 fLatFlow;
	}
	WBALANCE;


/********************************************************************************************
 *	    SammelVariable   Water                   									        *
 ********************************************************************************************/
typedef   struct xwater * PWATER;
typedef   struct xwater
	{
    int      iTopBC;
    int      iBotBC;
	float	 fRunOffR;
	float	 fLeaching; 
	float	 fCumLeaching;
	float	 fPotInfiltration;
	float    fInfiltR;
	float    fInfiltDay;
	float	 fCumInfiltration;
	float	 fPondWater;
	float	 fPercolR;
	float	 fPotETDay;
	float	 fActETDay;
	float	 fPotETCum;
	float	 fActETCum;
	float	 fPotETdt;
	float	 fActETdt;
	float    fRunOffCum;
	float	 fRunOffDay;
	float	 fActTranspDay;
	float	 fActTranspCum;
	float	 fLatFlowCum;
	float    fGrdWatLevel;
	float    fGrdWatLvlPot;
	PWLAYER	   pWLayer;
	PWMEASURE  pWMeasure;
	PEVAP	   pEvap;
	PWBALANCE  pWBalance;
 	}WATER;


#endif // #ifndef _INC_EXPERTN_XWATER_H

/*******************************************************************************
** EOF */