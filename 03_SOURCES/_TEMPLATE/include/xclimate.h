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
 * $History: xclimate.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:07
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung 01.12.96
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XCLIMATE_H
#define _INC_EXPERTN_XCLIMATE_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

typedef struct weather   * PWEATHER;
typedef struct weather  
	{
	long	 lDate;
	float	 fNumDay;
	float	 fSolRad;
	float	 fSunDuration;
	float	 fPAR;
	float	 fTempAve;
	float	 fTempMax;
	float	 fTempMin;
	float	 fHumidity;
	float	 fPanEvap;
	float	 fWindSpeed;
	float	 fDewPoint;
	float	 fSatDef;
	float	 fSoilTemp5;
	float	 fSoilTemp10;
	float	 fSoilTemp20;
	float	 fRainBegin;
	float	 fRainEnd;
	float	 fDayFrac;
	float	 fRainRate;
	float	 fRainAmount;
	float	 fSnow;
	float    fCanopyDrain;
	float    fWaterTable;
	float	 fUreaNRain;
	float	 fNH4NRain;
	float	 fNO3NRain;
	float	 fChemRain;

	PWEATHER       pNext;
	PWEATHER       pBack;
	}
	WEATHER;

typedef struct average   *  PAVERAGE;
typedef struct average 
	{
	float	 afMonthTemp[12];
	float	 fYearTemp;
	float	 fMonthTempAmp;
	float	 fAveYearTemp;
	float	 fYearSunDuration;
	float	 fYearSolRad;
	float	 fYearPAR;
	float	 fYearRain;
	float	 fYearSnow;
	float	 fYearHumidity;
	float	 fYearPanEvap;
	float	 fYearSatDef;
	}	
	AVERAGE;

/**********************************************************************************************
*			SammelVariable   Climate                                              *                       					     *
***********************************************************************************************/

typedef struct climate    * PCLIMATE;
typedef struct climate
	{
	float	 fCumRain;
	float	 afTempLast5Days[5];
	float	 fCumTempL5Days;
	float	 fTempMeasHeight;
	float	 fWindMeasHeight;

	PWEATHER	pWeather;
	PAVERAGE	pAverage;
	}
	CLIMATE;

#endif // #ifndef _INC_EXPERTN_XCLIMATE_H

/*******************************************************************************
** EOF */