/*******************************************************************************
 *
 * Copyright 2001 - 2002 (c) by ZeusSoft, Ing. Buero Bauer
 *                              www.zeussoft.de
 *                              Tel: 0700 zeussoft
 *
 * Author:      C.Sperr
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
 * $History: xmanag.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:08
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung 7.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XMANAG_H
#define _INC_EXPERTN_XMANAG_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct stSowInfo *PMSOWINFO;
typedef struct stSowInfo
{
	long	lSowDate;
	int		iDay;
	float	fPlantDens;
	float	fSowDepth;
	float	fTKW;
	float	fRowWidth;
	PMSOWINFO   pNext;
	PMSOWINFO   pBack;
} STSOWINFO;

typedef struct stLitter *PMLITTER;
typedef struct stLitter
{
	long	lHarvestDate;
	int   iDay;
	float	fTopAmount;
	float	fTopC;
	float	fTopCN;
	float	fRootAmount;
	float	fRootC;
	float	fRootCN;
	float	fTotalAmount;
	float	fTotalC;
	float	fTotalCN;
	float	fDecRate;
	int   iCarryOff;
	PMLITTER  pNext;
	PMLITTER  pBack;
} STLITTER;
		
typedef struct stIrrigation *PMIRRIGATION;
typedef struct stIrrigation 
{
	long	lDate;
	int	    iDay;	
	char	acName[51];   /// 10.10.97 CS
	char	acCode[30];		
	float	fBegin;
	float	fEnd;
	float	fDayFrac;
	float	fAmount;
	float	fRate;
	float   fNO3N;
	float   fNH4N;
	float   fUreaN;
	float	fCorgManure;
	float	fNorgManure;
	float   fDOC;
	float   fDON;
	float	fSystem;           
	float	fEfficiency;
	PMIRRIGATION   pNext;
	PMIRRIGATION   pBack;
} STIRRIGATION;

typedef struct stnfertilizer   * PNFERTILIZER;
typedef struct stnfertilizer
{            
	int     iID;
	long		lFertDate;
	int     iDay;	
	char		acName[30];
	char		acCode[30];	
	float		fDryMatter;
	float		forgMatter;	
	float		fCorgManure;
	float		fNorgManure;
	float		fOrgManureCN;
	float		fTotalN;
	float		fNH4N;
	float		fNO3N;
	float		fUreaN;
	float		afInfParam[3];
	PNFERTILIZER pNext;
	PNFERTILIZER pBack;
} STNFERTILIZER;    
	   
typedef struct stTillage    * PTILLAGE;
typedef struct stTillage   
{
	long		lDate;
	int			iDay;	   
	char		acName[30];	
	char		acCode[30];		
	int			iEquipment;
	float		fDepth;
	float		fEffMix;
	float		fEffLoose;
	float		fEffIncorp;
	PTILLAGE   pNext;
	PTILLAGE   pBack;
} STTILLAGE;

typedef struct stStartvalue   *PSTARTVALUE;
typedef struct stStartvalue
{
	long	lDate;
	int		iDay;
	float	afThickness[MAX_DEPTH_PER_MEASURE];
	float	afNO3N[MAX_DEPTH_PER_MEASURE];
	float	afNH4N[MAX_DEPTH_PER_MEASURE];
	float	afH2O[MAX_DEPTH_PER_MEASURE];
	float	afTemp[MAX_DEPTH_PER_MEASURE];
	float	afRootDens[MAX_DEPTH_PER_MEASURE];
} STSTARTVALUE;

typedef struct stmanagement   *PMANAGEMENT;
typedef struct stmanagement
{
	PMSOWINFO		  pSowInfo;
	PMLITTER     	pLitter;
	PMIRRIGATION	pIrrigation;
	PNFERTILIZER	pNFertilizer;
	PTILLAGE		  pTillage;
	STSTARTVALUE	Startvalue;
}	STMANAGEMENT;

#endif // #ifndef _INC_EXPERTN_XMANAG_H

/*******************************************************************************
** EOF */