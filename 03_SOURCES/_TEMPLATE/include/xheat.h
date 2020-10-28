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
 * $History: xheat.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:07
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 04.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XHEAT_H
#define _INC_EXPERTN_XHEAT_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct hlayer   *PHLAYER;
typedef struct hlayer
	{
	float	 fSoilTemp;
	float    fSoilTempOld;
	float    fHeat;
	float	 fHeatflow;
	float	 fHeatSens;
	float	 fHeatLatent;
	float	 fMeltPoint;
	float    fConduct;
	float    fConductOld;
	float    fCapacity;
	float    fCapacityOld;
	float	 fSoilTempAve;
	PHLAYER	pNext;
	PHLAYER	pBack;
	}
	HLAYER;


typedef struct hbalance   *PHBALANCE;
typedef struct hbalance
	{
	float	 fInput;
	float	 fOutput;
	float	 fProfil;
	float	 fProfilStart;
	float	 fBalance;
	}
	HBALANCE;


typedef struct hmeasure   *PHMEASURE;
typedef struct hmeasure
	{
	long	 lDate;
	int		 iDay;	
	float	 afDepth[MAX_DEPTH_PER_MEASURE];
	float	 afTemp[MAX_DEPTH_PER_MEASURE];
	PHMEASURE	pNext;
	PHMEASURE	pBack;
	}
	HMEASURE;

/****************************************************************************************
*                      SammelVariable  HEAT						*
*****************************************************************************************/

typedef struct heat   * PHEAT;
typedef struct heat
	{ 
	float   fFreezingDepth;
	PHLAYER		pHLayer;
	PHBALANCE	pHBalance;
	PHMEASURE	pHMeasure;
	}
	HEAT;

#endif // #ifndef _INC_EXPERTN_XHEAT_H

/*******************************************************************************
** EOF */