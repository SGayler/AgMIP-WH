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
 * $History: xsoil.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:09
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 19.1.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XSOIL_H
#define _INC_EXPERTN_XSOIL_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct sLayer    *PSLAYER;
typedef struct sLayer  
	{
	char	acSoilID[6];
	float	fThickness;
	float	fClay;
	float	fSand;
	float	fSilt;
	float	fCHumus;
	float	fHumus;
	float	fNHumus;
	float	fHumusCN;
	float	afPartVolFrac[4];
	float	fpH;
	float	fBulkDens;
	float	fBulkDensStart;	
	float	fPorosity;
	float	fCompress;
	float	fRockFrac;
	float   fImpedLiqA;
	float   fImpedLiqB;
	float   fImpedGasA;
	float   fImpedGasB;
	PSLAYER   pNext;
	PSLAYER   pBack;
	}
	SLAYER;

typedef struct sHeat    *PSHEAT;
typedef struct sHeat  
	{
	float	afPartDens[3];
	float	afPartGeom[3][3];
	float	afPartHeatCond[6];
	float	fCapAir;
	float	fCapWater;
	float	fCapIce;
	float	fCapOrg;
	float	fCapAnOrg;
	float	fCondAir;
	float	fCondWater;
	float	fCondIce;
	float	fCondOrg;
	float	fCondAnOrg;
	PSHEAT   pNext;
	PSHEAT 	 pBack;
	}
	SHEAT;

typedef struct swater    *PSWATER;
typedef struct swater
	{
	float	fContSat;
	float	fContPWP;
	float	fContFK;
	float	fContSatCER;
	float   fContRes;
	float   fContMobSat;
	float   fContImmSat;
	float   fMobImmEC;
	float	fCampA;
	float	fCampB;
	float	fVanGenA;     
	float	fVanGenA2;
	float	fVanGenM;
	float	fVanGenM2;
	float	fVanGenN;
	float	fVanGenN2;
	float   fMualemP;
	float   fMualemQ;
	float   fMualemR;
	float	fMinPot;
	float	fCondSat;
	float	fContInflec;
	float	fPotInflec;
	float   fBiModWeight1;
	float   fBiModWeight2;
	PSWATER   pNext;
	PSWATER   pBack;
	}
	SWATER;

/**********************************************************************************************
*		SammelVariable Boden										     *
**********************************************************************************************/

typedef struct sProfile   *PSPROFILE;
typedef struct sProfile
  	{
	int	    iLayers;
	float	fDepth;
	float	fDeltaZ;
	float	fPotPWP;
	float	fSoilAlbedo;
	float	fAlbedo;
	float   fSoilCover;
	float	fCurvNumNorm;
	float	fCurvNumDry;
	float	fCurvNumWet;
	float	fCurvNumAct;
	PSLAYER	pSLayer;
	PSHEAT	pSHeat;
	PSWATER	pSWater;
	}
	SPROFILE;

#endif // #ifndef _INC_EXPERTN_XSOIL_H

/*******************************************************************************
** EOF */