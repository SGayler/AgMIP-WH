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
 * $History: xlocatn.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:08
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 04.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XLOCATN_H
#define _INC_EXPERTN_XLOCATN_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

#define MAX_FILE_STRINGLEN     30

typedef struct Farm  * PFARM;
typedef struct Farm  
	{
	long	lFarmNumber;
	int 	iFieldNumbers;
	float	fLatitude;
	float	fLongitude;
	float	fAltitude;
	char	acFamilyName[MAX_FILE_STRINGLEN];
	char	acName		[MAX_FILE_STRINGLEN];
	char	acStreet	[MAX_FILE_STRINGLEN];
	long	lZipCode;
	char	acTown		[MAX_FILE_STRINGLEN];
	long	lAreaCode;
	long	lPhoneNumber;
	PFARM       pNext;
	PFARM       pBack;
	}
	FARM;

typedef struct Fieldplot  *  PFIELDPLOT;
typedef struct Fieldplot 
	{
	char	acWeatherFileName	[MAX_FILE_STRINGLEN];
	char	acWeatherStation	[MAX_FILE_STRINGLEN];
	int	    iNumber;
	char	acName				[MAX_FILE_STRINGLEN];
	char	acPrevCrop			[MAX_FILE_STRINGLEN];
	char	acCatchCrop			[MAX_FILE_STRINGLEN];
	char	acMainCrop			[MAX_FILE_STRINGLEN];
	float	fSize;
	float	fSlope;
	char	acExposition		[MAX_FILE_STRINGLEN];
	float	fTopSoilDepth;
	float	fSubSoilDepth;
	float	fAveGroundWaterTable;
	float	fDrainDepth;
	float	fDrainDistance;
	char	acConfigFileName	[MAX_FILE_STRINGLEN];
	char	acSimFileName		[MAX_FILE_STRINGLEN];
	char	acModFileName		[MAX_FILE_STRINGLEN];
	int		iRegionNumber;
	}	
	FIELDPLOT;
	

/**********************************************************************************************  
*					 SammelVariable   Location                               *
***********************************************************************************************/

typedef struct Location    * PLOCATION;
typedef struct Location
	{
	PFARM         pFarm;
	PFIELDPLOT    pFieldplot;
	}
	LOCATION;

#endif // #ifndef _INC_EXPERTN_XLOCATN_H

/*******************************************************************************
** EOF */