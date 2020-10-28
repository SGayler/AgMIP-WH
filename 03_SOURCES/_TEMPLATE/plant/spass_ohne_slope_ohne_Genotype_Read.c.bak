/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:  Enli Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   This file contains the source code for all plant process functions developed
 *   based on the SPASS model. The full description of the SPASS model can be
 *   found in Enli Wang (1997): 'Development of a generic process-oriented model
 *   for simulation of plant growth'.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 6 $
 *
 * $History: spass.c $
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 24.01.02   Time: 11:39
 * Updated in $/Projekte/ExpertN/ModLib/plant
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:18
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/plant
 * Fehler durch nicht initialisierte Variablen behoben.
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:42
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/plant
 * Unused method removed.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/plant
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 *  Update:   26.02.1998	
 *  Modified: 04.05.1999 by Sebastian Gayler
 *
*******************************************************************************/

#include <windows.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//Module defined include files
#include  "xinclexp.h"

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  LIBRARY EXPORT - MAIN PLANT PROCESS FUNCTIONS
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds PhasicDevelopment_SPASS	(EXP_POINTER);
int WINAPI _loadds Photosynthesis_SPASS	(EXP_POINTER);
int WINAPI _loadds BiomassGrowth_SPASS		(EXP_POINTER);
int WINAPI _loadds CanopyFormation_SPASS	(EXP_POINTER);
int WINAPI _loadds RootSystemFormation_SPASS(EXP_POINTER);
int WINAPI _loadds OrganSenescence_SPASS	(EXP_POINTER);
int WINAPI _loadds WaterCapture_SPASS		(EXP_POINTER);
int WINAPI _loadds NitrogenCapture_SPASS	(EXP_POINTER);
int WINAPI _loadds CropMaintenance_SPASS	(EXP_POINTER);

int WINAPI _loadds COMVR_TO_ZSTAGE_AND_CERES_WHEAT(EXP_POINTER);
int WINAPI _loadds COMVR_TO_ZSTAGE_AND_CERES_POTATO(EXP_POINTER);
int WINAPI _loadds COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(EXP_POINTER);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  LIBRARY EXPORT - SUB PROCESS FUNCTIONS
//------------------------------------------------------------------------------------------------------------
//Biological processes
float WINAPI _loadds RelativeTemperatureResponse_SPASS	(float fTemp, float fMinTemp, float fOptTemp, float fMaxTemp);
float WINAPI _loadds ThermalDevelopment_SPASS	(float fTempMin,float fTempMax,PRESPONSE pResp,float fMinTemp,float fOptTemp,float fMaxTemp);
float WINAPI _loadds DailyVernalization_SPASS	(float fTempMin,float fTempMax,PRESPONSE pResp,float fMinTemp,float fOptTemp,float fMaxTemp);
float WINAPI _loadds VernalizationEffect_SPASS	(float fCumVern, float fBasVern, float fCrtVern);
float WINAPI _loadds Photoperiodism_SPASS		(float fDaylen,PRESPONSE pResp, float fOptDaylen,float fSensF,LPSTR CropCode);
double		DVR[11];

//Climate/weather processes
float WINAPI DaylengthAndPhotoperiod_SPASS(float fLatitude, int nJulianDay, int nID);
//float WINAPI _loadds Daylength_SPASS			(float fLatitude, int iJulianDay);
float WINAPI _loadds HourlyTemperature_SPASS	(int iOclock, float fTempMax, float fTempMin);


//Assimilation,dissimilation and growth
float WINAPI _loadds LeafMaxGrossPhotosynthesis_SPASS(float fPgmmax,float fTemp,float fCO2,float fCO2Cmp,float fCO2R,
										   PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, PPLANT pPl);

float WINAPI _loadds LeafLightUseEfficiency_SPASS(float fMaxLUE, float fTemp, float fCO2, LPSTR pType);
float WINAPI _loadds CanopyGrossPhotosynthesis_SPASS(float fPgMax, float fLUE, float fLAI, 
										  float fLatitude,int nDay,float fHour, float fPARdir,float fPARdif);

float WINAPI _loadds DailyCanopyGrossPhotosythesis_SPASS(LPSTR pType,float fPgmmax,float fMaxLUE,float fCO2Cmp,float fCO2R,
											  PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, float fLAI,
											  float fLatitude,int nDay,
											  float fRad,float fTmpMax,float fTmpMin,float fCO2, PPLANT pPl);

int WINAPI _loadds AssimilatePartition_SPASS(EXP_POINTER);
int WINAPI _loadds OrganBiomassGrowth_SPASS(EXP_POINTER);
int WINAPI _loadds ReserveTranslocation_SPASS(EXP_POINTER);

//Root growth										  
int WINAPI _loadds RootExtension_SPASS(EXP_POINTER);
int WINAPI _loadds RootLengthDensity_SPASS(EXP_POINTER);         

//Water Uptake
float WINAPI _loadds CanopyWaterInterception_SPASS(float fLAI,float fRainfall);
float WINAPI _loadds PotentialTranspiration_SPASS(EXP_POINTER);
float WINAPI _loadds PotentialTranspiration_SPASSAlt(EXP_POINTER);
int   WINAPI _loadds ActualTranspiration_SPASS(EXP_POINTER);
int   WINAPI _loadds PlantWaterStress_SPASS(EXP_POINTER);
float fPotTraDay, fActTraDay;

//Nitrogen uptake
int WINAPI _loadds PlantNitrogenStress_SPASS			(EXP_POINTER);
int WINAPI _loadds NitrogenUptake_SPASS				(EXP_POINTER);
int WINAPI _loadds PlantNitrogenTranslocation_SPASS	(EXP_POINTER);
int WINAPI _loadds PlantNitrogenDemand_SPASS			(EXP_POINTER);
int WINAPI _loadds NitrogenConcentrationLimits_SPASS	(EXP_POINTER);

//zusätzliche Variablen für CERES-Maize und CERES-Sunflower
extern  int		iStageOld,iTLNO,iSlowFillDay;

extern float	fPHINT, fXDevStage, fP5, fP9, fO1, fNFAC;
//Maize:
extern float fEarWeight, fEarGrowR, fEarFraction;
extern float fTasselLeafNum, fMaxPlantLA;

//Sunflower:
extern float fStemWeightMin, fStemWeightMax;
extern float fHeadWeight, fHeadGrowR, fPotHeadWeight, fHeadWeightMin, fHeadWeightMax;
extern float fPericarpWeight, fPericarpGrowR, fPotPericarpGrowR;
extern float fEmbryoWeight, fEmbryoGrowR;
extern float fIGrainWeight, fOil, fOilPercent;
extern float fNHeadActConc, fNHeadMinConc, fNHeadOptConc, fNHeadDemand, fNHeadCont;
extern float fNPericarpActConc, fNPericarpOptConc, fNPericarpDemand, fNPericarpCont;
extern float fNEmbryoActConc, fNEmbryoOptConc, fNEmbryoDemand, fNEmbryoCont;
extern float fNGreenLeafActConc, fNGreenLeafCont, fNSenesLeafCont;
extern float fSumPS, fRI1, fRM, fRFR, fGrowFactor, ppp;
extern float fCumSenesLA, fSpecLeafArea, fAPLA;
extern float fSenesLAN2, fSLOPEPE, fXPEPE, fYRAT;

//zusätzliche Variablen für SUCROS:
extern float fBaseTemperature1;
extern float fCriticalTemperatureSum1;
extern float fBaseTemperature2;
extern float fCriticalTemperatureSum2;
extern RESPONSE	PmaxAge[21];

//SG 20110810: CO2 als Input von *.xnm (für AgMIP)
extern float fAtmCO2;


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  TOOL FUNCTIONS
//------------------------------------------------------------------------------------------------------------
extern float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp);
extern double WINAPI _loadds LIMIT(double v1, double v2, double x);

extern int WINAPI _loadds ReadSymbol(HANDLE,LPSTR);
//SG 20111019
extern float WINAPI _loadds ReadFloatOrNewLine(HANDLE);

extern int WINAPI _loadds ReadDate(HANDLE,LPSTR);

extern int WINAPI _loadds ReadInt(HANDLE);
extern long WINAPI _loadds ReadLong(HANDLE);
extern float WINAPI _loadds ReadFloat(HANDLE);
extern double WINAPI _loadds ReadDouble(HANDLE);

//extern int WINAPI _loadds DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex);
//extern int WINAPI _loadds LongToDate(long dValue,LPSTR lpDate,BOOL bFullYearNumber);

/*********************************************************************************************/
//		
/*********************************************************************************************/
BOOL WINAPI ReadDispDlgProc(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam);

int PlantVariableInitiation(EXP_POINTER);
int SPASS_Genotype_Read(HANDLE hInstance);

/* util_fct.c */
//extern int   WINAPI Message(long, LPSTR);
extern PPLANT  	WINAPI  GetPlantPoi(void);        


//#define SET_IF_99(x,y) 		if (x == (float)-99)   x = (float)y;


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  PHASIC DEVELOPMENT FUNCTION
//------------------------------------------------------------------------------------------------------------
	int WINAPI _loadds PhasicDevelopment_SPASS(EXP_POINTER)
		{
		float 		fTempMax,fTempMin,fMinTemp,fOptTemp,fMaxTemp; 
		float		fBasVernDays;
		float		fDaylength,fOptDaylength,fSensitivity;
		PRESPONSE 	pResp;
		PWEATHER	pWth=pCl->pWeather;
		PDEVELOP	pDev=pPl->pDevelop;

		int iGerm;
        
		int Germination(EXP_POINTER); //aus Ceres.c

		float fSeedDepth,fSprGrwRate; //für PLAGEN-Variante before emergence!

		
		//if (SimStart(pTi)) iGerm=(int)0;
		//SG/04/11/99:
		//In SPASS bisher keine Abfrage, ob Keimung stattfindet.
		//Da dies in der Dokumentation aber behauptet wird, wird nun an dieser 
		//Stelle die Germination-Funktion aus CERES aufgerufen, die mit der 
		//Dokumentation übereinstimmt.
		
		if(pDev->fStageSUCROS<(float)-0.5)
		{
			iGerm=Germination(exp_p);
			if(iGerm <= 0)
			{
				pPl->pDevelop->iDayAftSow++;	
				return	-1;
			}

			if(iGerm == 1)
				pDev->fStageSUCROS = (float)-0.5;
		}
	
		
		fTempMax = pWth->fTempMax;	    
		fTempMin = pWth->fTempMin;	   
			
		//=====================================================================================================
		//Thermal Effect of the Temperature
		//The cardinal temperatures of development rate
		if (pPl->pDevelop->fStageSUCROS<(float)1.0)
			{
		    fMinTemp= pPl->pGenotype->fTempMinDevVeg;
		    fOptTemp= pPl->pGenotype->fTempOptDevVeg;
		    fMaxTemp= pPl->pGenotype->fTempMaxDevVeg;
			pResp   = pPl->pGenotype->DevRateVegTemp;
		    }
		else
			{
		    fMinTemp= pPl->pGenotype->fTempMinDevRep;
		    fOptTemp= pPl->pGenotype->fTempOptDevRep;
		    fMaxTemp= pPl->pGenotype->fTempMaxDevRep;
			pResp   = pPl->pGenotype->DevRateRepTemp;
		    }
		
		pPl->pDevelop->fThermEffect = ThermalDevelopment_SPASS(fTempMax,fTempMin,pResp,fMinTemp,fOptTemp,fMaxTemp);
		
		//=====================================================================================================
		//Vernalization Process
	    fMinTemp= pPl->pGenotype->fTempMinVern;	
	    fOptTemp= pPl->pGenotype->fTempOptVern;
	    fMaxTemp= pPl->pGenotype->fTempMaxVern; 
	    pResp   = pPl->pGenotype->VernRateTemp;

		/* In Plagen für Weizen pPl->pGenotype->fOptVernDays = 45 */

		fBasVernDays		= (float)0.173913*pPl->pGenotype->fOptVernDays;   
		pDev->fVernUnit		= DailyVernalization_SPASS(fTempMin,fTempMax,pResp,fMinTemp,fOptTemp,fMaxTemp);
		pDev->fCumVernUnit += pDev->fVernUnit; 
		
		if (pDev->fStageWang<=2.0)	//Before floral initiation
			pDev->fVernEff 	= VernalizationEffect_SPASS(pDev->fCumVernUnit, fBasVernDays, pPl->pGenotype->fOptVernDays);
		
		//=====================================================================================================
		//Photoperiodism
	    fOptDaylength	= pPl->pGenotype->fOptDaylen;	
	    fSensitivity	= pPl->pGenotype->fDaylenSensitivity;
	    pResp   		= pPl->pGenotype->DevRateVegPhtP;   
		                                                          
		fDaylength = DaylengthAndPhotoperiod_SPASS(pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay,2);
			                                                          
		if (pDev->fStageSUCROS<(float)1.0)	//Before flowering
			pDev->fDaylengthEff = Photoperiodism_SPASS(fDaylength,pResp, fOptDaylength,fSensitivity,pPl->pGenotype->acCropCode);
		else
			pDev->fDaylengthEff = (float)1.0;      
		
		//=====================================================================================================
		//Development stage
		if (pDev->fStageSUCROS<(float)1.0)
			pDev->fDevR = pPl->pDevelop->fThermEffect*pDev->fVernEff*pDev->fDaylengthEff/pPl->pGenotype->fDevelopConstant1;
		else
			pDev->fDevR = pPl->pDevelop->fThermEffect/pPl->pGenotype->fDevelopConstant2;			
		
		//=====================================================================================================
		//Before Emergence:

		/*SG/30/03/99////////////////////////////////////////////////////////////////////////////
		//
		//Phänologische Entwicklung before Emergence wie in PLAGEN:								*/

		if (pDev->fStageSUCROS<(float)0.0)
			{

			/* Saat-Tiefe und maximales Längenwachstum:
				fSeedDepth [cm](=pMa->pSowInfo->fSowDepth)
				fSprGrwRate [cm/d]
			*/
			//sinnvoller wäre Eingabe über PLANT.gtp
	
				
	/*  // Berechnung der Dauer von "germination" bis "emergence" aus Saattiefe	
		// und Temperatur 
			//fSeedDepth = (float)3.0;
			fSeedDepth = pMa->pSowInfo->fSowDepth;
			fSprGrwRate= (float)1.5;

        //SG/04/11/99: wegen Germination aus CERES:
		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
			fSprGrwRate= (float)1.5;
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"S"))	
			fSprGrwRate= (float)1.5;


*/
		//	altes SPASS:
		//  pDev->fDevR = ((float)0.5*(fTempMax+fTempMin)-(float)0.0)/(float)100.0;
			
		//  nach Diss E.Wang und Wang&Engel (1998):
		//fSumDTT needed for Emergence = (float)125.0; 
		pDev->fDevR = (float)0.5*(float)max(0.0,((float)0.5*(fTempMax+fTempMin)-(float)0.0)/(float)125.0);


		//fSumDTT needed for Emergence for different Crops aus Saattiefe nach CERES:
		if(pMa->pSowInfo->fSowDepth>(float)0.0)
			fSeedDepth = pMa->pSowInfo->fSowDepth;
		else
		{
			fSeedDepth = (float)3.0;
			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
				fSeedDepth = (float)6.0;
		}
		
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			pDev->fDevR = (float)max(0.0,(float)0.5*((float)0.5*(fTempMax+fTempMin)-(float)2.0)/((float)40.0+((float)10.2)*fSeedDepth));
		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))||
			(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"S")))
			pDev->fDevR = (float)max(0.0,(float)0.5*((float)0.5*(fTempMax+fTempMin)-(float)2.0)/((float)50.0+((float)10.4)*fSeedDepth));


		//SG/19/05/99: Potato
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))	
			{
			fSprGrwRate= (float)0.4;//Agria=0.4; Christa=0.405
			if (!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Agria"))	
				fSprGrwRate= (float)0.4;
			if (!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Christa"))	
				fSprGrwRate= (float)0.45;

			pDev->fDevR = fSprGrwRate*pPl->pDevelop->fThermEffect/fSeedDepth;
			}

		//SG/24/05/05: Maize
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
			pDev->fDevR = (float)max(0.0,(float)0.5*((float)0.5*(fTempMax+fTempMin)-(float)10.0)/((float)50.0+((float)6.0)*fSeedDepth)); // /((float)15.0+((float)6.0)*fSeedDepth))

		if (pDev->fStageSUCROS+pDev->fDevR>(float)0.0)
					pDev->fDevR = -pDev->fStageSUCROS;

		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
			pDev->fDevStage = (pDev->fStageSUCROS+pDev->fDevR + (float)1.0) * (float)10.0;

		}
		
        /*SG/////////////////////////////////////////////////////////////////////////////////////
        /                                                                                       /
        / iDayAftEmerg wird auch in "DevelopmentCheckAndPostHarvestManagement" in PLANT.C bzw.  /
        / PLANT_SG.C hochgezählt! (und somit doppelt!!)                                         /
        /                                                                                       /		
        / else                                                                                  /
        /     pPl->pDevelop->iDayAftEmerg++;                                                    /
        /                                                                                       /
        /                                                                                       /
        / Da aber iDayAftSow nie hochgezählt wird, nun jetzt hier:                              /
        ///////////////////////////////////////////////////////////////////////////////////////*/

        pPl->pDevelop->iDayAftSow++;	
		

        //SG/19/05/99: after emergence

        if (pDev->fStageSUCROS>=(float)0.0)
        {

        //SG/17/05/99:////////////////////////////////////////////
        //
        //  Unterscheidung zwischen verschiedenen Pflanzenarten.
        //  DEFAULT und POTATO aus PLAGEN!
        //////////////////////////////////////////////////////////


		if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"S")))
			COMVR_TO_ZSTAGE_AND_CERES_WHEAT(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);

		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
			COMVR_TO_ZSTAGE_AND_CERES_POTATO(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);

		else
			COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);

		}
        //SG/09/06/99:
        // Änderung des internen Entwicklungsstadiums hinter die 
        // Umwandlung in EC-Stadium verlegt
		pDev->fStageSUCROS	+= pDev->fDevR;


// SG 20130516-----------------------------------------------------------------
	// To fix the date of Emergence following SUCROS DEVELOPMENT MODEL
	// provide "Date of Emergence" in in database or xnd 
        // ----------------------------------------------------------------------------
        if(pPl->pModelParam->iEmergenceDay>0) //Wenn "Date of Emergence" in Datenbank oder xnd angegeben, dann erfolgt Feldaufgang an diesem Tag (=SUCROS)
	{
   	    if(pTi->pSimTime->fTimeAct < (float)(pPl->pModelParam->iEmergenceDay))
			pDev->fStageSUCROS = -(float)0.3;

	    if(pTi->pSimTime->fTimeAct >= (float)(pPl->pModelParam->iEmergenceDay)&&(pDev->fStageSUCROS < (float)0.0))
			pDev->fStageSUCROS = (float)0.0;
        }

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds ThermalDevelopment_SPASS(float fTempMin,float fTempMax,PRESPONSE pResp,
//											   float fMinTemp,float fOptTemp,float  fMaxTemp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function uses daily maximum and minimum temperature to calculate the thermal effect on 
//			development rate. The calculated effect is a daily average value and ranges from 0.0 to 1.0.  
//			The data under pResp are used at highest priority. If pResp=NULL or the first element of pResp 
//			contains zero data, then this function calculates the effect using the cardinal values
//			(fMinTemp,fOptTemp,fMaxTemp) and a predefined function RelativeTemperatureResponse_SPASS.
//Inputs:	1. fTempMin - Daily minimum temperature of the air (C) 	
//			2. fTempMax	- Daily maxmimum temperature of the air (C)
//			3. pResp	- Pointer to a RESPONSE structure containing temperature response data for development
//			4. fMinTemp	- Minimum temperature for development (C)	
//			5. fOptTemp	- Optimum temperature for development (C)	
//			6. fMaxTemp	- Maximum temperature for development (C)	
//Outputs:	1. Return 	- Thermal effect on development rate (0-1)
//Functions Called: 
//			HourlyTemperature_SPASS
//			RelativeTemperatureResponse_SPASS 
//			AFGENERATOR
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds ThermalDevelopment_SPASS(float fTempMin,float fTempMax,PRESPONSE pResp,
								   float fMinTemp,float fOptTemp,float  fMaxTemp)
	{
	int 	 iLoop;
    float 	 fTemp,fTempFunc,fTermEffect;  

	//Calculating the daily thermal effect
	fTermEffect=(float)0.0;
	for (iLoop=1; iLoop<=8; iLoop++)
		{       
		fTemp = HourlyTemperature_SPASS(iLoop,fTempMin,fTempMax);

		if ((pResp==NULL)||(pResp->fInput==(float)0.0))
			fTempFunc = RelativeTemperatureResponse_SPASS(fTemp,fMinTemp,fOptTemp,fMaxTemp);
		else
			fTempFunc = AFGENERATOR(fTemp, pResp);

        fTermEffect += fTempFunc; 
		}

	fTermEffect = fTermEffect/(float)8.0;

	return fTermEffect;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds VernalizationEffect_SPASS(float fBasVern, float fCrtVern, float fCumVern)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function calculates the current vernalization effect on development rate (0-1)
//Inputs:	1. fCumVern - Cumulative vernalization days till the current time (PVDs)
//			2. fBasVern	- Minimum vernalization days after that development progresses (PVDs)
//			3. fCrtVern	- Critical vernalization days with which vernalization fully fulfilled (PVDs)	
//Outputs:	1. Return 	- Vernalization effect on development rate (0-1)
//Functions Called: 
//			None
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds VernalizationEffect_SPASS(float fCumVern, float fBasVern, float fCrtVern)
	{
    float fVernEffect;

	if (fCrtVern==(float)0.0)
		fVernEffect = (float)1.0;
	else
		fVernEffect = (fCumVern-fBasVern)/(fCrtVern-fBasVern);
	
	fVernEffect =min((float)1.0,max((float)0.0,fVernEffect));
	
	return fVernEffect;
	}
	
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds DailyVernalization_SPASS(float fTempMin, float fTempMax, PRESPONSE pResp,
//		  							  	       float fMinTemp, float fOptTemp, float  fMaxTemp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function uses daily maximum and minimum temperature to calculate daily vernalization unit. 
//			The unit ranges from 0.0 (min.or max. temperature) to 1.0 (optimal temperature). 
//			The data under pResp are used at highest priority. If pResp=NULL or the first element of pResp 
//			contains zero data, then this function calculates the effect using the cardinal values
//			(fMinTemp,fOptTemp,fMaxTemp) and a predefined function RelativeTemperatureResponse_SPASS.
//Inputs:	1. fTempMin - Daily minimum temperature of the air (C) 	
//			2. fTempMax	- Daily maxmimum temperature of the air (C)
///			3. pResp	- Pointer to a RESPONSE structure containing temperature response data for vernalization
//			4. fMinTemp	- Minimum temperature for vernalization (C)	
//			5. fOptTemp	- Optimum temperature for vernalization (C)	
//			6. fMaxTemp	- Maximum temperature for vernalization (C)	
//Outputs:	1. Return 	- Daily vernalization unit (PVDs)
//Functions Called: 
//			HourlyTemperature_SPASS
//			RelativeTemperatureResponse_SPASS
//			AFGENERATOR
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds DailyVernalization_SPASS(float fTempMin, float fTempMax, PRESPONSE pResp,
							  	   float fMinTemp, float fOptTemp, float  fMaxTemp)
	{
	int 		iLoop;
    float 		fTemp,fTempFunc,fVernUnit;

	//Daily vernalization unit
	fVernUnit=(float)0.0;
	for (iLoop=1; iLoop<=8; iLoop++)
		{       
		//3 Hour temperature
		fTemp = HourlyTemperature_SPASS(iLoop,fTempMin,fTempMax); 
		
		//Vernalization unit
		if ((pResp==NULL)||(pResp->fInput==(float)0.0))
			fTempFunc = RelativeTemperatureResponse_SPASS(fTemp,fMinTemp,fOptTemp,fMaxTemp);
		else
			fTempFunc = AFGENERATOR(fTemp, pResp);

        fVernUnit 	+= fTempFunc; 
		}
	
	fVernUnit = fVernUnit/(float)8.0;
	
	return fVernUnit;
	}      

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds Photoperiodism_SPASS(float fDaylen,PRESPONSE pResp, float fOptDaylen,float fSensF,LPSTR CropCode)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function uses current daylength to calculate the effect of photoperiod on development rate
//			The data under pResp are used at highest priority. If pResp=NULL or the first element of pResp 
//			contains zero data, this function calculates the effect using the optimal daylength (fOptDaylen)
//			and a daylength sensitivity factor (fSensF) with an exponental equation
//Inputs:	1. fDaylen		- Current daylength (hours)
///			2. pResp		- Pointer to a RESPONSE structure containing photoperiod response data 
//			3. fOptDaylen	- Optimal photoperiod (hours)
//			4. fSensF		- photoperiod sensitivity factor (-)	
//Outputs:	1. Return 		- Effect of photoperiod on development (0-1)
//Functions Called: 
//			AFGENERATOR
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds Photoperiodism_SPASS(float fDaylen,PRESPONSE pResp, float fOptDaylen,float fSensF, LPSTR CropCode)
	{
    float 	fDaylenEffect;
    double 	vDaylen, vMinDaylen,vOptDaylen, vSens;
    
    vDaylen    = (double)fDaylen;
	vOptDaylen = (double)fOptDaylen;
	vSens	   = (double)fSensF;  

	//Output results
	fDaylenEffect=(float)1.0;      
	
	if ((pResp==NULL)||(pResp->fInput==(float)0.0))  
		{
		if (vSens<0.000001)
		  	fDaylenEffect=(float)1.0;
		else
			{			
			vMinDaylen = vOptDaylen-4.0/vSens;
			if(!lstrcmp((LPSTR)CropCode,(LPSTR)"PT"))
				vMinDaylen = max(0.0,min(24.0,vOptDaylen+4.0/vSens));
	  		fDaylenEffect=(float)(1.0-exp(-4.0*(vDaylen-vMinDaylen)/(vOptDaylen-vMinDaylen)));
			if(!lstrcmp((LPSTR)CropCode,(LPSTR)"PT"))
	  			fDaylenEffect=(float)(1.0-exp(+4.0*(vDaylen-vMinDaylen)/fabs(vOptDaylen-vMinDaylen)));

		}   }
	else
		fDaylenEffect= AFGENERATOR((float)vDaylen, pResp);

  	fDaylenEffect=max((float)0.0,fDaylenEffect);
  	   
	return fDaylenEffect;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  CHANGE VR STAGE TO ZADOCKS STAGE AND CERES STAGES
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds COMVR_TO_ZSTAGE_AND_CERES_WHEAT(EXP_POINTER)
	{                
	int			i;
	float		fMainStemTiller; //SG/19/04/99: wg.anpassen an PLAGEN
	static float node;
	float VR[11];

	PDEVELOP 	pDev=pPl->pDevelop; 
	PCANOPY		pCan=pPl->pCanopy;

	double		VRD[]={0.0,	0.1425,	0.4,	0.5,	0.69,	0.91,	1.00,	1.15,	1.50,	1.95,	2.0};
//	double		DVR[]={0.0,	0.1425,	0.39,	0.656,	0.91,	1.00,	1.15,	1.50,	1.95,	2.0};
//	double		DVR[]={0.0,	0.15,	0.35,	0.69,	0.80,	1.00,	1.20,	1.80,	1.99,	2.0};
	
    if(DVR[0]<0.0)
		for (i=0;i<=10;i++)  DVR[i]=VRD[i];

	for (i=0;i<=10;i++)  VR[i]=(float)DVR[i];

	if (pDev->iStageCERES==0)
		{
		pDev->iStageCERES=8;
		pDev->fStageXCERES=(float)0.0;
		
		/*if (pDev->fStageSUCROS==(float)0.0)		//SG/22/05/99: versteh ich nicht!
			pDev->fStageSUCROS=(float)-1.0;*/		// funktioniert ohne diese Zeilen viel besser!!
		}

   	//--------------------------------------------------------------------
	//Fromm fStageSUCROS to pDev->fStageWang
   	//--------------------------------------------------------------------
	if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<=VR[1]))
		pDev->fStageWang=(float)1.0+(pDev->fStageSUCROS-VR[0])/(VR[1]-VR[0]);
	if ((pDev->fStageSUCROS>VR[1])&&(pDev->fStageSUCROS<=VR[2]))
		pDev->fStageWang=(float)2.0+(pDev->fStageSUCROS-VR[1])/(VR[2]-VR[1]);
	if ((pDev->fStageSUCROS>VR[2])&&(pDev->fStageSUCROS<=VR[3]))
		pDev->fStageWang=(float)3.0+(float)0.2*(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2]);
	if ((pDev->fStageSUCROS>VR[3])&&(pDev->fStageSUCROS<=VR[4]))
		pDev->fStageWang=(float)3.2+(float)0.8*(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3]);
	if ((pDev->fStageSUCROS>VR[4])&&(pDev->fStageSUCROS<=VR[5]))
		pDev->fStageWang=(float)4.0+(pDev->fStageSUCROS-VR[4])/(VR[5]-VR[4]);
	if ((pDev->fStageSUCROS>VR[5])&&(pDev->fStageSUCROS<=VR[6]))
		pDev->fStageWang=(float)5.0+(pDev->fStageSUCROS-VR[5])/(VR[6]-VR[5]);

	if ((pDev->fStageSUCROS>VR[6])&&(pDev->fStageSUCROS<=VR[7]))
		pDev->fStageWang=(float)6.0+(pDev->fStageSUCROS-VR[6])/(VR[7]-VR[6]);
	if ((pDev->fStageSUCROS>VR[7])&&(pDev->fStageSUCROS<=VR[8]))
		pDev->fStageWang=(float)7.0+(pDev->fStageSUCROS-VR[7])/(VR[8]-VR[7]);
	if ((pDev->fStageSUCROS>VR[8])&&(pDev->fStageSUCROS<=VR[9]))
		pDev->fStageWang=(float)8.0+(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8]);
	if ((pDev->fStageSUCROS>VR[9])&&(pDev->fStageSUCROS<=VR[10]))
		pDev->fStageWang=(float)9.0+(pDev->fStageSUCROS-VR[9])/(VR[10]-VR[9]);
			
   	//--------------------------------------------------------------------
	//Fromm fStageSUCROS to fXStage
   	//--------------------------------------------------------------------
	//Sowing to germination to emergence	
	if (pDev->fStageSUCROS<VR[0]) 
		pDev->fStageXCERES=pDev->fStageSUCROS+(float)1.0;
    
    //Emergence to terminal spikelet
	if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<VR[2]))
	    pDev->fStageXCERES=(float)1.0+(pDev->fStageSUCROS-VR[0])/(VR[2]-VR[0]);
	    
    //Terminal spikelet to end of leaf growth
	if ((pDev->fStageSUCROS>=VR[2])&&(pDev->fStageSUCROS<VR[3]))
	    pDev->fStageXCERES=(float)2.0+(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2]);
	    
    //End of leaf growth to Heading
	if ((pDev->fStageSUCROS>=VR[3])&&(pDev->fStageSUCROS<VR[4]))
	    pDev->fStageXCERES=(float)3.0+(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3]);
	    
    //Heading to Milk development
	if ((pDev->fStageSUCROS>=VR[4])&&(pDev->fStageSUCROS<VR[6]))
	    pDev->fStageXCERES=(float)4.0+(pDev->fStageSUCROS-VR[4])/(VR[6]-VR[4]);
	    
    //Milk development(begin of grain filling) to End of grain filling (ripening)
	if ((pDev->fStageSUCROS>=VR[6])&&(pDev->fStageSUCROS<VR[8]))
	    pDev->fStageXCERES=(float)5.0+(pDev->fStageSUCROS-VR[6])/(VR[8]-VR[6]);
	    
	if ((pDev->fStageSUCROS>=VR[8])&&(pDev->fStageSUCROS<VR[9]))
	    pDev->fStageXCERES=(float)6.0+(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8]);

   	//--------------------------------------------------------------------
	//Fromm fStageSUCROS,fStageXCERES to iStageCERES
   	//--------------------------------------------------------------------
	if (pDev->fStageSUCROS<(float)-0.5)
		pDev->iStageCERES=8;
	if ((pDev->fStageSUCROS>=(float)-0.5)&&(pDev->fStageSUCROS<VR[0]))
		pDev->iStageCERES=9;
	if (pDev->fStageSUCROS>=VR[0])
		pDev->iStageCERES=(int)pDev->fStageXCERES;
		
   	//--------------------------------------------------------------------
	//Fromm pDev->fStageWang to fZStage
   	//--------------------------------------------------------------------


	fMainStemTiller= max((float)0.0, pCan->fExpandLeafNum-(float)2.5+(float)1.0);
	
	if ((pDev->fStageWang>=(float)1.0)&&(pDev->fStageWang<(float)3.0))
		{
		if (pCan->fTillerNum<=(float)1.0)
	//	if (fMainStemTiller<=(float)1.0)
			pDev->fDevStage=(float)1.0+pCan->fExpandLeafNum/(float)10.0;
		else
			//pDev->fDevStage=(float)2.0+fMainStemTiller/(float)10.0;
			//SG/09/06/99.
			//	besser mit pCan->fMainStemTillers, dann EC 21 besser erreicht!
			pDev->fDevStage=(float)2.0+pCan->fTillerNum/(float)10.0;
        
        node = (float)0.0;
        }
        
		
	if ((pDev->fStageWang>=(float)3.0)&&(pDev->fStageWang<(float)4.0))
		{    
		node += (float)0.2844*pPl->pDevelop->fThermEffect*pDev->fDaylengthEff;
		
		pDev->fDevStage=(float)3.0+node/(float)10.0;

		if (pDev->fStageSUCROS>=0.656)
			pDev->fDevStage=(float)3.9;
			
		}
        
	if ((pDev->fStageWang>=(float)4.0)&&(pDev->fStageWang<=(float)9.0))
		pDev->fDevStage=pDev->fStageWang;  
	
	if (pDev->fStageWang>(float)9.0)
		pDev->fDevStage=(float)9.0+(float)0.2*(pDev->fStageWang-(float)9.0);  


	pDev->fDevStage *=(float)10.0;	


	//SG/25/05/99: Wahlweise Berechnung nach PLAGEN:
	//	Wird pPl->pGenotype->fMaxLfAppRate = 0 gesetzt (Zeile 2000040 in Plant.gtp)
	//	erfolgt EC-Berechnung ohne Leaves, Tillers und nodes.
	if(pPl->pGenotype->fMaxLfAppRate==(float)0.0) 
	pDev->fDevStage = min((float)92.0,pDev->fStageWang * (float)10.0);
	
	return 1;
	}


int WINAPI _loadds COMVR_TO_ZSTAGE_AND_CERES_POTATO(EXP_POINTER)
	{                
		//Based on Spitters, van Keulen & van Kraalingen, 19xx, SUCROS 87
	//Maturity class (MATR) 2.5 for very late cv's and 9.5 for very early cv's
	float fMaxTuberizRate,fTuberizRate;
	float x;

	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
		pPl->pDevelop->fStageXCERES = (float)0.0;   

	if (pPl->pDevelop->fStageXCERES<(float)1.0) //Before tuberization
		{
		//fMaxTuberization ranges from 10 to 26 
		fMaxTuberizRate = (float)1.0/((float)0.5*pPl->pGenotype->fDevelopConstant1);//Agria=0.7 Christa=0.5
		if (!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Agria"))	
			fMaxTuberizRate = (float)(1.0/(0.5*pPl->pGenotype->fDevelopConstant1));
		if (!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Christa"))	
			fMaxTuberizRate = (float)(1.0/(0.4*pPl->pGenotype->fDevelopConstant1));

		fTuberizRate    = fMaxTuberizRate*pPl->pDevelop->fThermEffect*pPl->pDevelop->fDaylengthEff;
			
		//If fDevStage=1.0, tuberization occurs
		pPl->pDevelop->fStageXCERES += fTuberizRate;
		}
	else
		pPl->pDevelop->fStageXCERES += pPl->pDevelop->fThermEffect/(float)40.0;

    		
	if (pPl->pDevelop->fStageSUCROS<(float)1.0)
		pPl->pDevelop->fDevR=pPl->pDevelop->fThermEffect/pPl->pGenotype->fDevelopConstant1; 
	else
		pPl->pDevelop->fDevR=pPl->pDevelop->fThermEffect/pPl->pGenotype->fDevelopConstant2; 

//	pPl->pDevelop->fDevStage = (float)5*(pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR)*(float)10;

	x=pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR;


	if (x<(float)1.0)
		pPl->pDevelop->fDevStage = (float)(5.*x + 1.)*(float)10;
	else
		pPl->pDevelop->fDevStage = (float)min(100.,(2+4*x)*10);

	return 1;
	}


int WINAPI _loadds COMVR_TO_ZSTAGE_AND_CERES_DEFAULT(EXP_POINTER)
	{                
	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
		pPl->pDevelop->fStageWang = pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR;   
	else
		pPl->pDevelop->fStageWang = (float)5*(pPl->pDevelop->fStageSUCROS+pPl->pDevelop->fDevR);   

	pPl->pDevelop->fDevStage = (float)10.0*pPl->pDevelop->fStageWang;   
	//pPl->pDevelop->fDevStage = pPl->pDevelop->fStageWang;   

	return 1;
	}
	                           

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds RelativeTemperatureResponse_SPASS(float fTemp, 
//														float fTempMin, float fTempOpt, float fTempMax)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function calculates the effect of temperature on the rate of certain plant process with
//			a temperature optimum (fOptTemp)
//Inputs:	1. fTemp	- Current temperature (C)
//			2. fMinTemp	- Minimum temperature for the process (C) 	
//			2. fOptTemp	- Optimum temperature for the process (C) 	
//			2. fMaxTemp	- Maximum temperature for the process (C) 	
//Outputs:	The calculated temperature effect (0-1)  0-fMinTemp,fMaxTemp; 1-fOptTemp
//Functions Called:
//			None 
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds RelativeTemperatureResponse_SPASS(float fTemp, float fMinTemp, float fOptTemp, float fMaxTemp)
	{
	double vTemp,vTmin,vTopt,vTmax,p, vRelEff;
    
    vTemp = (double)fTemp;
    vTmin = (double)fMinTemp;
    vTopt = (double)fOptTemp;
    vTmax = (double)fMaxTemp;
    
	if ((fTemp<=fMinTemp)||(fTemp>=fMaxTemp))
		vRelEff=0.0;
	else
		{
	    p =log(2)/log((vTmax-vTmin)/(vTopt-vTmin));
		vRelEff = (2*pow(vTemp-vTmin,p)*pow(vTopt-vTmin,p)-pow(vTemp-vTmin,2*p))/pow(vTopt-vTmin,2*p);   
		}
			
	return (float)vRelEff;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds HourlyTemperature_SPASS(int iOclock, float fTempMax, float fTempMin)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function uses daily maximum & minimum temperature to calculate the hourly temperature
//			(temperature daily course)  
//Inputs:	1. iOclock	- Current time in hours
//			2. fTempMax	- Daily maxmimum temperature of the air (C) 	
//			3. fTempMin	- Daily minimum temperature of the air (C)
//Outputs:	The calculated temperature at time iOclock (C)
//Functions Called: 
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds HourlyTemperature_SPASS(int iOclock, float fTempMax, float fTempMin)
	{    
    float	fTemp;
    double  fr,r;

	r 	  = (double)iOclock;
	fr	  = 0.5*(1.0+cos((2*r-1)*PI/8));
	fTemp = fTempMin + (float)fr*(fTempMax-fTempMin);
	
	return fTemp;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  BIOMASS GROWTH FUNCTION
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds Photosynthesis_SPASS(EXP_POINTER)
	{

	PRESPONSE	pResp;
	CARDTEMP	CardTmp;
	ORGANNC		OrgNc;
	float 		fPgmmax, fMaxLUE,fCO2Cmp,fCO2R;

    PWEATHER	pWth=pCl->pWeather; 
	PCANOPY 	pCan=pPl->pCanopy;
    PPLTCARBON	pCbn=pPl->pPltCarbon;

	char pType[5];	//SG/17/05/99: wg. Unterscheidung der Pflanzenarten

	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
		return 0;


	//SG/18/05/99: Unterscheidung der Pflanzenarten

	lstrcpy(pType,"C3\0");

	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI"))
		||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG")))

		lstrcpy(pType,"C4\0");




    
	//OrgNc.fMaxOptNc = pPl->pPltNitrogen->fLeafOptConc;
	//if(pPl->pPltNitrogen->fLeafOptConc == (float)0.0)	
	OrgNc.fMaxOptNc = (float) 0.067;

    OrgNc.fActNc	= pPl->pPltNitrogen->fLeafActConc;
    OrgNc.fMinNc 	= pPl->pPltNitrogen->fLeafMinConc;
		//SG 2005/12/14:
    OrgNc.fOptNc	= pPl->pPltNitrogen->fLeafOptConc;
	OrgNc.fMaxOptNc = pPl->pGenotype->OptLvNc[1].fOutput;


	//=======================================================================================================
	//Photosynthesis
	//=======================================================================================================
	CardTmp.fMinTemp = pPl->pGenotype->fTempMinPs;
	CardTmp.fOptTemp = pPl->pGenotype->fTempOptPs;
	CardTmp.fMaxTemp = pPl->pGenotype->fTempMaxPs;
	fPgmmax = pPl->pGenotype->fMaxGrossPs;
	fMaxLUE = pPl->pGenotype->fLightUseEff;
	fCO2Cmp = pPl->pGenotype->fCO2CmpPoint;
	fCO2R   = pPl->pGenotype->fCiCaRatio;
	pResp	= pPl->pGenotype->PmaxTemp;

	pCbn->fGrossPhotosynR = DailyCanopyGrossPhotosythesis_SPASS(pType, fPgmmax,fMaxLUE,
										fCO2Cmp,fCO2R,pResp,&CardTmp, &OrgNc,
										pCan->fLAI,pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay,
										pWth->fSolRad,pWth->fTempMax,pWth->fTempMin,(float)CO2,pPl);

    	return 1;
    }


int WINAPI _loadds BiomassGrowth_SPASS(EXP_POINTER)
	{ 
	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
		return 0;

	AssimilatePartition_SPASS	(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	OrganBiomassGrowth_SPASS	(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	
	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	CropMaintenance_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds CropMaintenance_SPASS(EXP_POINTER)
	{
	float MainLv,MainSt,MainRt,MainSo,Maints,MaintMetab,TempRef,TempEf,fTemp,MnDvs,fTotLvWeight;
	float fActLv,fActSt,fActRt;//,fActSo;
	double 	Q10;
	
	PWEATHER		pWth =pCl->pWeather;
	PBIOMASS		pBiom=pPl->pBiomass;   
	PDEVELOP		pDev =pPl->pDevelop;
	PPLTCARBON 		pPltC=pPl->pPltCarbon;

	
	MainLv	=pPl->pGenotype->fMaintLeaf;		//(float)0.03;		Werte stehen in PLANT.gtp!
	MainSt	=pPl->pGenotype->fMaintStem;		//(float)0.015;		Im Moment stehen dort die
	MainRt	=pPl->pGenotype->fMaintRoot;		//(float)0.015;		Weizen-Werte aus PLAGEN.
	MainSo	=pPl->pGenotype->fMaintStorage;	//(float)0.01;			(0.16, 0.1, 0.15, 0.1) 
	
	//===============================================================================================
	//Temperature effect
	Q10   	= 2.0;
	TempRef	=(float)25.0;
	fTemp =(float)0.5*(pWth->fTempMax+pWth->fTempMin);
	TempEf=(float)pow(Q10,(double)(fTemp-TempRef)/10.0);
	//===============================================================================================
	
	//Nitrogen influence:
	//Activity coefficient accounting for the influence of nitrogen content on maintenance resp 


	fActLv = pPl->pPltNitrogen->fLeafRelNc;
	fActSt = pPl->pPltNitrogen->fStemRelNc;
	fActRt = pPl->pPltNitrogen->fRootRelNc;
	//===============================================================================================
	//Maintenance change due to aging
	fTotLvWeight=pBiom->fLeafWeight+pBiom->fSenesLeafWeight;

	if (fTotLvWeight==(float)0.0)
		MnDvs=(float)0.0;
	else
		MnDvs = pBiom->fLeafWeight/fTotLvWeight;   //SUCROS2
	//===============================================================================================
	//Maintenance of different organs. It is assumed that daytime maintenance respiration of 
	//leaves is reduced by 50% due to excess energy (0.75)
	pPltC->fMainRespRLeaf = 	MainLv*pBiom->fLeafWeight *TempEf*fActLv*(float)0.75 ;
	pPltC->fMainRespRStem = 	MainSt*pBiom->fStemWeight *TempEf*fActSt*MnDvs;
	pPltC->fMainRespRRoot =    MainRt*pBiom->fRootWeight *TempEf*fActRt;
	pPltC->fMainRespRGrain= 	MainSo*pBiom->fGrainWeight*TempEf*MnDvs; 
	 
	Maints  = 	MainLv*pBiom->fLeafWeight *TempEf*fActLv*(float)0.75 
			+	MainSt*pBiom->fStemWeight *TempEf*fActSt*MnDvs
			+	MainRt*pBiom->fRootWeight *TempEf*fActRt
			+	MainSo*pBiom->fGrainWeight*TempEf*MnDvs;

	//The metabolic components is 20% of gross ps and reduced 50% during the day
	MaintMetab=	pPltC->fGrossPhotosynR*(float)(0.5*0.2);
    Maints	= Maints+MaintMetab;

	//Maintenance respiration 	
	pPltC->fMaintRespR = Maints;      

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	ReserveTranslocation_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds ReserveTranslocation_SPASS(EXP_POINTER)
	{                 
	static float fTotalReservWeihgt=(float)0.0;
	float 		CFLV,CFRT,CFST,CFSO;
	PBIOMASS	pBiom=pPl->pBiomass;
	PDEVELOP	pDev =pPl->pDevelop;
	
	CFLV	=(float)0.459;	//C-content of leaves
	CFRT	=(float)0.467;	//C-content of root
	CFST	=(float)0.494;	//C-content of stem
	CFSO	=(float)0.471;	//C-content of Storage organs
	
	pPl->pBiomass->fStemReserveGrowRate=pBiom->fStemGrowR*pPl->pGenotype->fReserveRatio*CFST/(float)0.444;
	pPl->pBiomass->fStemReserveWeight +=pPl->pBiomass->fStemReserveGrowRate;
	

	//SG/06/05/99:
	//
	//Bedingung bei der if-Abfrage erweitert, damit fTotalReservWeihgt bei jeder
	//neuen Pflanze wieder auf 0 gesetzt wird!
	if (pBiom->fPartFracStem>(float)0.0||(pBiom->fPartFracStem==(float)0.0
									&&pDev->fStageSUCROS==(float)0.0))
		{
		pPl->pBiomass->fReserveTransRate = (float)0.0;
		fTotalReservWeihgt		= pPl->pBiomass->fStemReserveWeight;
		}
	else     
		{
		pPl->pBiomass->fReserveTransRate   = fTotalReservWeihgt*pDev->fDevR;
		pPl->pBiomass->fStemReserveWeight -= pPl->pBiomass->fReserveTransRate;
		}
		 
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	OrganBiomassGrowth_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds OrganBiomassGrowth_SPASS(EXP_POINTER)
	{
	float PrtcLv,PrtcSt,PrtcRt,PrtcSo,EffPrtn,EffCH2O,EffGrn,ASRQLV,ASRQST,ASRQRT,ASRQSO,fAvailableAssim;
	float fGrwRespLv,fGrwRespSt,fGrwRespRt,fGrwRespSo,fGrwRespRes,CO2Lv,CO2St,CO2Rt,CO2So;
	float fTempMin,fTempMax,fTempOpt,fTemp,fTempFunc,fGrainCarboDemand,fCarboSupply;

	float fSeedReserv;// Wasserfaktor;
	
	PWEATHER		pWth =pCl->pWeather;
	PBIOMASS		pBiom=pPl->pBiomass; 
	PCANOPY			pCan =pPl->pCanopy; 
	PDEVELOP		pDev =pPl->pDevelop;
	PPLTCARBON 		pPltC=pPl->pPltCarbon;
	PPLTWATER 		pPltW=pPl->pPltWater;
	PPLTNITROGEN 	pPltN=pPl->pPltNitrogen;
    
	//---------------------------------------------------------------------------------
	//Effect of Nitrogen Content
	//Protein content of different organs
	EffPrtn =(float)0.440; 	//Efficiency coefficient for conversion of photosythate to protein
	EffCH2O =(float)0.825; 	//Efficiency coefficient for conversion of photosythate to structural carbohydrate
	EffGrn  =(float)0.690; 	//Efficiency coefficient for conversion into grain biomass

	PrtcLv = (float)6.25*pPl->pPltNitrogen->fLeafActConc;
	PrtcSt = (float)6.25*pPl->pPltNitrogen->fStemActConc;
	PrtcRt = (float)6.25*pPltN->fRootActConc;
	PrtcSo = (float)6.25*pPltN->fGrainConc;

	//Assimilate requirement for conversion of carbohydrates to dry matter	
	ASRQLV = PrtcLv*EffPrtn +((float)1.0-PrtcLv)*EffCH2O;
	ASRQST = PrtcSt*EffPrtn +((float)1.0-PrtcSt)*EffCH2O;
	ASRQRT = PrtcRt*EffPrtn +((float)1.0-PrtcRt)*EffCH2O;
	ASRQSO = PrtcSo*EffGrn  +((float)1.0-PrtcSo)*EffCH2O;

	ReserveTranslocation_SPASS(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);


	//SG/07/04/99/////////////////////////////////////////////////////////////////////////
	//
	// Wasserfaktor für ersten Tag mit fStageSUCROS>0.
	// (Aktuelle Transpiration wird vor fStageSUCROS berechnet und
	//  ist deshalb am ersten Tag == 0).
	//////////////////////////////////////////////////////////////////////////////////////

	
	/* if (fPotTraDay == (float)0.0)
				Wasserfaktor = (float)1.0;
			else
				Wasserfaktor = fActTraDay/fPotTraDay;
	*/

	//SG/20/05/99: Sonderfall Kartoffel

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
		{
			fSeedReserv = (float)1.5*(float)10.0*pMa->pSowInfo->fPlantDens;
			
			if (pBiom->fSeedReserv>fSeedReserv)
				pBiom->fSeedReserv -= fSeedReserv;
			else
				fSeedReserv = (float)0.0;
		}
	else
			fSeedReserv = (float)0.0;
				



	pPltC->fGrossPhotosynR *= pPltW->fStressFacPhoto; //SG/26/05/99: pPltW->fStressFacPhoto statt Wasserfaktor

	//SG/20/05/99: fSeedReserv wg. Potato
	fAvailableAssim=(float)(30.0/44.0)*max((float)0.0, pPltC->fGrossPhotosynR + fSeedReserv-pPltC->fMaintRespR)
									+pPl->pBiomass->fReserveTransRate*(float)(1.1111*0.947); 

if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))||
	(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))||
	(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")))
	{
	//=========================================================================================
	//Grain demand
	//=========================================================================================
	if (pDev->fStageSUCROS>=(float)1.0)
	{
	fTempMin = pPl->pGenotype->fTempMinSoFill;
	fTempOpt = pPl->pGenotype->fTempOptSoFill;
	fTempMax = pPl->pGenotype->fTempMaxSoFill;  
	
	fTemp = (float)0.5*(pWth->fTempMin+pWth->fTempMax);

	if (pPl->pGenotype->SoFillRateTemp[0].fInput==(float)0.0)
		fTempFunc = RelativeTemperatureResponse_SPASS(fTemp,fTempMin,fTempOpt,fTempMax);
	else
		fTempFunc = AFGENERATOR(fTemp, pPl->pGenotype->SoFillRateTemp);
		
	fGrainCarboDemand = pPl->pGenotype->fRelStorageFillRate*fTempFunc*pCan->fGrainNum
						*(float)1E+4*pMa->pSowInfo->fPlantDens*(float)1E-6/ASRQSO;
	  
	fCarboSupply = min(fAvailableAssim,fGrainCarboDemand);
	
	if (fCarboSupply<fAvailableAssim)
		{ 
		pPl->pBiomass->fReserveTransRate  -= (fAvailableAssim-fCarboSupply)/((float)(1.1111*0.947));
		pPl->pBiomass->fStemReserveWeight += (fAvailableAssim-fCarboSupply)/((float)(1.1111*0.947));

		if (pPl->pBiomass->fStemReserveWeight/pBiom->fStemWeight>=pPl->pGenotype->fReserveRatio)
			{
			pPl->pBiomass->fLeafReserveWeight=pPl->pBiomass->fStemReserveWeight-pBiom->fStemWeight*pPl->pGenotype->fReserveRatio;
			}
		}
		 	
	fAvailableAssim=fCarboSupply;
	}               
	}
	
	//==========================================================================================
	//Biomass Growth Rate of Different Organs	

	if (fAvailableAssim==(float)0.0)
		{
		pBiom->fLeafGrowR 		= (float)0.0;
		pBiom->fRootGrowR 		= (float)0.0; 
		pBiom->fStemGrowR 		= (float)0.0; 
		pBiom->fGrainGrowR		= (float)0.0;
		}
	else
		{			
/*
		ASRQLV = (float)1.0/lpGen->fGrwEffLeaf;
		ASRQST = (float)1.0/lpGen->fGrwEffStem;
		ASRQRT = (float)1.0/lpGen->fGrwEffRoot;
		ASRQSO = (float)1.0/lpGen->fGrwEffStorage;
*/
		//---------------------------------------------------------------------------------
		//Growth rate, leaves, stems, roots and stem reserves
		pBiom->fLeafGrowR = fAvailableAssim*pBiom->fPartFracLeaf*ASRQLV;
		pBiom->fRootGrowR = fAvailableAssim*pBiom->fPartFracRoot*ASRQRT; 
		pBiom->fStemGrowR = fAvailableAssim*pBiom->fPartFracStem*ASRQST;								
		pBiom->fGrainGrowR= fAvailableAssim*pBiom->fPartFracGrain*ASRQSO;
		}

	//==========================================================================================
	//Growth Respiration  and total respiration
	//CO2 producing factor
	CO2Lv 	=pPl->pGenotype->fCO2EffLeaf;		//(float)0.461;
	CO2St 	=pPl->pGenotype->fCO2EffStem;		//(float)0.408;
	CO2Rt 	=pPl->pGenotype->fCO2EffRoot;		//(float)0.406;
	CO2So 	=pPl->pGenotype->fCO2EffStorage;		//(float)0.347;
                                                          
	//Growth respiration
	fGrwRespLv = max((float)0.0,pBiom->fLeafGrowR *CO2Lv);
	fGrwRespSt = max((float)0.0,pBiom->fStemGrowR *CO2St);
	fGrwRespRt = max((float)0.0,pBiom->fRootGrowR *CO2Rt);
	fGrwRespSo = max((float)0.0,pBiom->fGrainGrowR*CO2So);
	
	fGrwRespRes= (float)max(0.0,(double)pPl->pBiomass->fReserveTransRate*1.111*0.053*44.0/30.0);
	
	//Total respiration and net carbon gain
	pPltC->fGrowthRespR = fGrwRespLv+fGrwRespSt+fGrwRespRt+fGrwRespSo+fGrwRespRes;
	pPltC->fTotRespR    = pPltC->fGrowthRespR + pPltC->fMaintRespR;
	pPltC->fNetPhotosynR= max((float)0.0, pPltC->fGrossPhotosynR-pPltC->fTotRespR);
	

	//Dry weight of different organs	                          
	pBiom->fLeafWeight 		+= pBiom->fLeafGrowR; 
	pBiom->fStemWeight  	+= pBiom->fStemGrowR;
	pBiom->fRootWeight  	+= pBiom->fRootGrowR;
	pBiom->fGrainWeight 	+= pBiom->fGrainGrowR;

	pPl->pBiomass->fStemReserveWeight =  max((float)0.0,pPl->pBiomass->fStemReserveWeight);

	pPl->pBiomass->fTotLeafWeight 	= pBiom->fLeafWeight; 
	pPl->pBiomass->fTotRootWeight  	= pBiom->fRootWeight;
	pPl->pBiomass->fTotStemWeight  	= pBiom->fStemWeight;

	pBiom->fBiomassAbvGround = pPl->pBiomass->fTotLeafWeight+pPl->pBiomass->fTotStemWeight+pBiom->fGrainWeight;

	pBiom->fBiomGrowR=pBiom->fLeafGrowR+pBiom->fRootGrowR+pBiom->fStemGrowR+pBiom->fGrainGrowR;

	//SG/16/04/99////////////////////////////////////////////////////////////
	//
	//	Biomass "above ground" wird in der Grafik nicht ausgegeben!
	//	Stattdessen Wert für "StovWeight", der aber nie aktualisiert wird.
	//	Deshalb nun die folgende Zeile:
	/////////////////////////////////////////////////////////////////////////

	pBiom->fStovWeight = pPl->pBiomass->fTotLeafWeight+pPl->pBiomass->fTotStemWeight;

	pBiom->fFruitWeight = pBiom->fGrainWeight;

	pBiom->fTotalBiomass		= pBiom->fBiomassAbvGround
 						  		 +pBiom->fRootWeight;
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	AssimilatePartition_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds DefaultAssimilatePartitioning_SPASS(EXP_POINTER);
int WINAPI _loadds AssimilatePartition_SPASS(EXP_POINTER)
	{
	static float xtuber;
	float 		x,fTopFraction; 
	PRESPONSE 	pResp1,pResp2,pResp3;
	PBIOMASS 	pBm =pPl->pBiomass;

	x=pPl->pDevelop->fStageSUCROS;

	//SG/03/11/99, Kartoffel:
	//Da der Zeitpunkt der Knollenbildung unabhängig von den restlichen
	//phänologischen Entwicklungsstadien berechnet wird, erfolgt die
	//Assimilatverteilung zunächst nach XCERES und nach dem Beginn des
	//Knollenwachstums nach SUCROS.
	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
	{
			if(pPl->pDevelop->fStageXCERES<=(float)1.0)
			{
				x=pPl->pDevelop->fStageXCERES/(float)2.0;
				xtuber = pPl->pDevelop->fStageSUCROS;
			}
			else if(pPl->pDevelop->fStageSUCROS<=(float)1.0)
			
				x = (float)(0.5+(xtuber-0.5)*(pPl->pDevelop->fStageSUCROS-xtuber)/(1.0-xtuber))
						*pPl->pDevelop->fStageSUCROS/xtuber;
			else
				x = pPl->pDevelop->fStageSUCROS;
	}

/*	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
			x=pPl->pDevelop->fStageXCERES;
*/


	//Get the pointer of the partitioning data 
	pResp1	= pPl->pGenotype->PartRoot;
	pResp2	= pPl->pGenotype->PartLeaf;
	pResp3	= pPl->pGenotype->PartStem;

	if ((pResp1->fInput==(float)0.0)||(pResp2->fInput==(float)0.0)||(pResp3->fInput==(float)0.0))
		{
		DefaultAssimilatePartitioning_SPASS(exp_p);
		}
		
	else
		{
		//Root and Top Fraction
		pBm->fPartFracRoot = AFGENERATOR(x, pPl->pGenotype->PartRoot)/pPl->pPltWater->fStressFacPartition;
		fTopFraction  	   = (float)1.0 - pBm->fPartFracRoot;         
	
		//Leaf, stem, grain fraction
		pBm->fPartFracLeaf = fTopFraction*AFGENERATOR(x, pPl->pGenotype->PartLeaf);
	    pBm->fPartFracStem = fTopFraction*AFGENERATOR(x, pPl->pGenotype->PartStem);
	
	    pBm->fPartFracGrain= fTopFraction-pBm->fPartFracLeaf-pBm->fPartFracStem;
		}
		
	pBm->fPartFracFruit	= pBm->fPartFracGrain;

	return 1;
	}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float WINAPI _loadds DailyCanopyGrossPhotosythesis_SPASS(LPSTR pType,float fPgmmax,float fMaxLUE,float fCO2Cmp,float fCO2R,
//											  PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, float fLAI,
//											  float fLatitude,int nDay,
//											  float fRad,float fTmpMax,float fTmpMin,float fCO2,PPLANT pPl)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the daily canopy photosynthesis rate under optimal water condition
//Inputs:	1. pType		- Pointer to a string containing the type of the plant (C3 or C4)
//			2. fPgmmax		- Maximum gross leaf photosynthesis rate at optimal temperature, N level, both 
//							  CO2 and light saturation (kgCO2/ha.hr)
//			3. fMaxLUE		- Maximum leaf light use efficiency at low light, low temperature and low CO2
//							  ((kgCO2/ha leaf.hr)/(W/m2))
//			4. fCO2Cmp		- Compensation point of CO2 for photosynthesis (vppm)
//			5. fCO2R		- Ratio of CO2 concentration in stomatal cavity to that in the air (-)
///			6. pResp		- Pointer to a RESPONSE structure containing temperature response data for photosynthesis 
//			7. pCardTemp	- pointer to a CARDTEMP structure containing the cardinal temperature for photosynthesis
//			8. pLfNc		- Pointer to a ORGANNC structure containing leaf nitrogen concentration
//			9. fLAI			- Effective leaf area index (-)
//		   10. fLatitude	- Location latitude (Degree)
//		   11. nDay			- Julain day (-)
//		   12. fRad			- Daily global radiation (MJ/d)
//		   13. fTmpMax		- Daily maximum air temperature(C)
//		   14. fTmpMin		- Daily minimum air temperature(C)
//		   15. fCO2			- Current CO2 concentration in the air (vppm)
//Outputs:	1. Return   	- Calculated daily gross photosynthesis rate of unit leaf area (kgCO2/ha.day)
//Functions Called:
//			LeafMaxGrossPhotosynthesis_SPASS
//			LeafLightUseEfficiency_SPASS
//			CanopyGrossPhotosynthesis_SPASS 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the temperature response function for photosynthesis. If not, the cardinal temperatures
//			at pCardTemp will be used to construct the temperature response function. If pCardTemp equals NULL,
//			a minimum, optimum and maximum temperature of 0, 22 and 35C will be assumed respectively.
//			If pLfNc equals NULL, no nitrogen stress will be considered.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds DailyCanopyGrossPhotosythesis_SPASS(LPSTR pType,float fPgmmax,float fMaxLUE,float fCO2Cmp,float fCO2R,
											  PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, float fLAI,
											  float fLatitude,int nDay,
											  float fRad,float fTmpMax,float fTmpMin,float fCO2, PPLANT pPl)
	{
	int		i;
	double 	vAveGrossPs,vGrossPs,vDailyGrossPs,PAR,PARDIF,PARDIR,vAveGrossPsMax;
	double 	vLAI,vGlobalRadiation,vSinHeight,vDayl,vAtmTrans,vDifFr;
	double  vLatitude,vDec,vSin,vCos,vRsc,vSolarConst,vDailySin,vDailySinE,vHour,vRadExt;
	float	fLUE,fPgMax,fTemp;
	int    	nGauss=5;
    double 	xGauss[]={0.0469101, 0.2307534, 0.5000000, 0.7692465, 0.9530899};
	double 	wGauss[]={0.1184635, 0.2393144, 0.2844444, 0.2393144, 0.1184635};

	double RAD	= PI/180.0;

	double PARCum = 0.0;

	vLAI				= (double)fLAI;
	vGlobalRadiation	= (double)fRad*1E6; 	//J/m2.d
	vLatitude 			= (double)fLatitude;

	//===========================================================================================
	//Daylength, Solar constant and daily extraterrestrial radiation
	//===========================================================================================
	//Declination of the sun as function of Daynumber (vDay)
	vDec = -asin( sin(23.45*RAD)*cos(2.0*PI*((double)nDay+10.0)/365.0));
	
	//vSin, vCos and vRsc are intermediate variables
	vSin = sin(RAD*vLatitude)*sin(vDec);
	vCos = cos(RAD*vLatitude)*cos(vDec);
	vRsc = vSin/vCos;

	//Astronomical daylength (hr)
	vDayl=12.0*(1.0+2.0*asin(vSin/vCos)/PI);         
 
	//integral of Sine of solar height vDailySin and integral of vDailySin with 
	//correction for lower atmospheric transmission at low solar elevations (vDailySinE)
	vDailySin  = 3600.0*(vDayl*vSin+24.0*vCos*sqrt(1.0-vRsc*vRsc)/PI);
	vDailySinE = 3600.0*(vDayl*(vSin+0.4*(vSin*vSin+vCos*vCos*0.5))
				+12.0*vCos*(2.0+3.0*0.4*vSin)*sqrt(1.0-vRsc*vRsc)/PI);

	//Solar constant(vSolarConst) and daily extraterrestrial (vRadExt) 
	vSolarConst  = 1370.0*(1.0+0.033*cos(2.0*PI*(double)nDay/365.0));	//J/m2.d
	vRadExt		 = vSolarConst*vDailySin*1E-6;					//MJ/m2.d
    
	//===========================================================================================
	//Daily photosynthesis
	//===========================================================================================
	//Assimilation set to zero and three different times of the Day (vHour)
    vAveGrossPs 	= 0;
    vAveGrossPsMax 	= 0;     
    
   	//Daytime temperature
    fTemp = (float)0.71*fTmpMax+(float)0.29*fTmpMin;
      
	for (i=0;i<nGauss;i++)
		{
		//At the specified vHour, radiation is computed and used to compute assimilation
		vHour = 12.0+vDayl*0.5*xGauss[i];
		//Sine of solar elevation
		vSinHeight  = max(0.0, vSin + vCos*cos(2.0*PI*(vHour+12.0)/24.0));
		//Diffuse light fraction (vDifFr) from atmospheric transmission (vAtmTrans)
 		PAR   		= 0.5*vGlobalRadiation*vSinHeight*(1.0+0.4*vSinHeight)/vDailySinE;
		vAtmTrans  	= PAR/(0.5*vSolarConst*vSinHeight);
		
		PARCum += PAR * wGauss[i];
		if (vAtmTrans<=0.22) 
			vDifFr = 1.0;
		else
			{
			if ((vAtmTrans>0.22)&&(vAtmTrans<=0.35)) 
				vDifFr = 1.0-6.4*(vAtmTrans-0.22)*(vAtmTrans-0.22); 
			else
				vDifFr = 1.47-1.66*vAtmTrans;
			}
			
		vDifFr = max (vDifFr, 0.15+0.85*(1.0-exp(-0.1/vSinHeight)));
		
		//Diffuse PAR (PARDIF) and direct PAR (PARDIR)
		PARDIF = min(PAR, vSinHeight*vDifFr*vAtmTrans*0.5*vSolarConst);
		PARDIR = PAR-PARDIF;

		//SG 20110810:
		//CO2-conc. from input file (*.xnm) for AgMIP-project
		fCO2= (fAtmCO2 > 0? fAtmCO2:fCO2);
		
		//Light response parameters
		fPgMax  = LeafMaxGrossPhotosynthesis_SPASS(fPgmmax,fTemp,fCO2,fCO2Cmp,fCO2R,pResp,pCardTmp,pLfNc,pPl);
		fLUE	= LeafLightUseEfficiency_SPASS(fMaxLUE,fTemp,fCO2,pType);

		//Canopy gross photosynthesis
		vGrossPs= CanopyGrossPhotosynthesis_SPASS(fPgMax,fLUE,fLAI,fLatitude,nDay,(float)vHour,
			                                      (float)PARDIR,(float)PARDIF);

		//Integration of assimilation rate to a daily total (vAveGrossPs) 
		vAveGrossPs += vGrossPs*wGauss[i];
    	}

	vDailyGrossPs= vAveGrossPs * vDayl;
	
    return (float)vDailyGrossPs;
    }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float WINAPI _loadds LeafMaxGrossPhotosynthesis_SPASS(float fPgmmax,float fTemp,float fCO2,float fCO2Cmp,float fCO2R,
// 											   PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, PPLANT pPl)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the maximum gross photosynthesis of unit leaf area at current temperature
//			current CO2 concentration, current leaf N level and light saturation
//Inputs:	1. fPgmmax		- Maximum gross leaf photosynthesis rate at optimal temperature, N level, both 
//							  CO2 and light saturation (kgCO2/ha.hr)
//			2. fTemp		- Current air temperature (C) 	
//			3. fCO2			- Current CO2 concentration in the air (vppm)
//			4. fCO2Cmp		- Compensation point of CO2 for photosynthesis (vppm)
//			5. fCO2R		- Ratio of CO2 concentration in stomatal cavity to that in the air (-)
///			6. pResp		- Pointer to a RESPONSE structure containing temperature response data for photosynthesis 
//			7. pCardTemp	- pointer to a CARDTEMP structure containing the cardinal temperature for photosynthesis
//			8. pLfNc		- Pointer to a ORGANNC structure containing leaf nitrogen concentration
//Outputs:	1. Return   	- Calculated maximum gross photosynthesis rate of unit leaf area (kgCO2/ha.hr)
//Functions Called:
//			RelativeTemperatureResponse_SPASS
//			AFGENERATOR 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the temperature response function for photosynthesis. If not, the cardinal temperatures
//			at pCardTemp will be used to construct the temperature response function. If pCardTemp equals NULL,
//			a minimum, optimum and maximum temperature of 0, 22 and 35C will be assumed respectively.  
//			If pLfNc equals NULL, no nitrogen stress will be considered.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds LeafMaxGrossPhotosynthesis_SPASS(float fPgmmax,float fTemp,float fCO2,float fCO2Cmp,float fCO2R,
										   PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, PPLANT pPl)
	{     
    float	fMinTemp,fOptTemp,fMaxTemp,fTempFunc,fCO2I,fCO2I340,fCO2Func,fLfNcFunc,fPmaxGross;
	float	fNcFact; //SG/17/05/99: wg. Unterscheidung Weizen/Kartoffel
    
	//------------------------------------------------------------------------    	
	//Temperature response and Effect of daytime temperature
	if (pCardTmp==NULL)
		{
		fMinTemp = (float)0.0;
		fOptTemp = (float)22.0;
		fMaxTemp = (float)35.0;
		}
	else
    	{
		fMinTemp = pCardTmp->fMinTemp;
		fOptTemp = pCardTmp->fOptTemp;
		fMaxTemp = pCardTmp->fMaxTemp;
		}		

	if ((pResp==NULL)||(pResp->fInput==(float)0.0))
		fTempFunc = RelativeTemperatureResponse_SPASS(fTemp,fMinTemp,fOptTemp,fMaxTemp);
	else
		fTempFunc = AFGENERATOR(fTemp, pResp);

	//------------------------------------------------------------------------    	
	//Effect of CO2 concentration of the air
	fCO2I 	= fCO2*fCO2R;
	fCO2I340= fCO2R*(float)340.0;

	fCO2Func= min((float)2.3,(fCO2I-fCO2Cmp)/(fCO2I340-fCO2Cmp)); //For C3 crops
	//SG20121112: CO2-response of C4-crops (not used in AgMIP-MAIZE!!!)
	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		fCO2Func= min((float)1.3,(fCO2I-fCO2Cmp)/(fCO2I340-fCO2Cmp)); //For C4 crops
	//	fCO2Func= min((float)1.1,(fCO2I-fCO2Cmp)/(fCO2I340-fCO2Cmp)); //For C4 crops


	//------------------------------------------------------------------------    	
    //Effect of leaf nitrogen content
	if (pLfNc==NULL)
		fLfNcFunc = (float)1.0;
	else

		//SG/19/04/99////////////////////////////////////////////////////////////
		//
		//	Anpassung an PLAGEN: Optimale N-Konz. statt maximaler; deshalb auch Faktor 0.9
		//	weggelassen!
		//fLfNcFunc = (pLfNc->fActNc-pLfNc->fMinNc)/((float)0.9*pLfNc->fMaxOptNc-pLfNc->fMinNc);
		fLfNcFunc = (float) min((float)1.0,max((float)0.0,
		             (pLfNc->fActNc-pLfNc->fMinNc)/((float)0.9*pLfNc->fMaxOptNc-pLfNc->fMinNc)));
		////////////////////////////////////////////////////////////////////////

		//fLfNcFunc = (pLfNc->fActNc-pLfNc->fMinNc)/(pLfNc->fMaxOptNc-pLfNc->fMinNc);

	//SG 20051214: 
	//laut Dokumentation (Diss E.Wang) und Ur-SPASS:
	//fLfNcFunc = (float)pow((pLfNc->fActNc-pLfNc->fMinNc)/(pLfNc->fOptNc-pLfNc->fMinNc),2);
	//fLfNcFunc = (float)min(1.0,max(0.0,fLfNcFunc));


	//SG19990517:
	//
	//	Sonderbehandlung für Kartoffel (aus PLAGEN)!

	fNcFact = (float)0.0;
	
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
		fNcFact = (float)pow((double)fLfNcFunc,1.0/3.0);

	fLfNcFunc = min((float)1.0, max(fNcFact,fLfNcFunc));	
	//fLfNcFunc = min((float)1.0, max((float)0.0,fLfNcFunc));


	//SG/08/04/99///////////////////////////////////////////////////////////////////
	//
	// In PLAGEN fLfNcFunc immer 1.0, solange fStageSUCROS < 0.0015!!!
	//
	// Ausserdem Fehler in PLAGEN, der beweirkt, dass fStageSUCROS an dieser Stelle
	// immer gleich 0 ist. Deshalb:
	////////////////////////////////////////////////////////////////////////////////
	// fLfNcFunc = (float)1.0;	//SG/16/04/99: auskommentiert, um zu sehen, wie SPASS ohne diesen Fehler rechnet!

	//------------------------------------------------------------------------    	
    //Maximum leaf gross photosynthesis
	fPmaxGross= max((float)1.0, fPgmmax*(fCO2Func*fTempFunc*fLfNcFunc));
	 	                     
	return fPmaxGross;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float WINAPI _loadds LeafLightUseEfficiency_SPASS(float fMaxLUE, float fTemp, float fCO2, LPSTR pType)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the light use efficiency of the leaf at low light level and 
//			current CO2 concentration and temperature
//Inputs:	1. fMaxLUE		- Maximum leaf light use efficiency at low light, low temperature and low CO2
//							  ((kgCO2/ha leaf.hr)/(W/m2))
//			2. fTemp		- Current air temperature (C) 	
//			3. fCO2			- Current CO2 concentration in the air (vppm)
//			4. pType		- Pointer to a string containing the type of the plant (C3 or C4)
//Outputs:	1. Return   	- Calculated light use efficiency at low light ((kgCO2/ha leaf.hr)/(W/m2))
//Functions Called:
//			None
//Comments:	For C3 plants the light use efficiency will decreased if temperature becomes or CO2 concentration 
//			becomes low due to the increased photorespiration. So that the effect of photorespiration is 
//			simulated in this function
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds LeafLightUseEfficiency_SPASS(float fMaxLUE, float fTemp, float fCO2, LPSTR pType)
	{     
	float fCO2PhotoCmp0,fCO2PhotoCmp,fEffPAR;
	 
	//Check whether a C3 or C4 crop
	if ((0==lstrcmp(pType,"C3\0"))||(0==lstrcmp(pType,"c3\0")))   //C3 plants
		fCO2PhotoCmp0=(float)38.0; //vppm
	else
		fCO2PhotoCmp0=(float)0.0;
		
	//Efect of Temperature
	fCO2PhotoCmp = fCO2PhotoCmp0*(float)pow(2.0,((double)fTemp-20.0)/10.0);

	//Efect of CO2 concentration
	fEffPAR	= fMaxLUE*(fCO2-fCO2PhotoCmp)/(fCO2+2*fCO2PhotoCmp);
	
	return fEffPAR;
	}		

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float WINAPI _loadds CanopyGrossPhotosynthesis_SPASS(float fPgMax, float fLUE, float fLAI, 
//										  float fLatitude,int nDay,float fHour, float fPARdir,float fPARdif)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the canopy gross photosynthesis rate for a given crop with fPgMax,fLUE,fLAI
//			at latitude (fLatitude) on day (julian day fDay) at fHour
//Inputs:	1. fPgMax	- Maximum leaf gross photosynthesis rate at light saturation (kgCO2/ha.hr)
//			2. fLUE		- Light use efficiency of the leaf at current conditions ((kgCO2/ha leaf.hr)/(W/m2)) 	
//			3. fLAI		- Effective LAI (-)
//			4. fLatitude- Location latitude (Degree)
//			5. nDay		- Julain day (-)
//			6. fHour	- Current time (Hour)
//			7. fPARdir	- Direct component of incident photosynthetic active radiation (W/m2)
//			7. fPARdif	- Diffuse component of incident photosynthetic active radiation (W/m2)
//Outputs:	1. Return   - Calculated canopy photosynthesis rate (kgCO2/ha.hr)
//Functions Called:
//			None
//Comments:	The input variable fPgMax and fLUE should be calculated using the following functions:
//			fPgMax = LeafMaxGrossPhotosynthesis_SPASS(...);
//			fLUE   = LeafLightUseEfficiency_SPASS(...)
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

float WINAPI _loadds CanopyGrossPhotosynthesis_SPASS(float fPgMax, float fLUE, float fLAI, 
										  float fLatitude,int nDay,float fHour, float fPARdir,float fPARdif)
	{
	int		i,j;
	double  PMAX,EFF,vLAI,PARDIR,PARDIF,SINB;
	double 	SQV,REFH,REFS,CLUSTF,KDIRBL,KDIRT,FGROS,LAIC,VISDF,VIST,VISD,VISSHD,FGRSH;
	double 	FGRSUN,VISSUN,VISPP,FGRS,FSLLA,FGL,LAT,DAY,HOUR,DEC,vSin,vCos;
	int    nGauss=5;
    double xGauss[]={0.0469101, 0.2307534, 0.5000000, 0.7692465, 0.9530899};
	double wGauss[]={0.1184635, 0.2393144, 0.2844444, 0.2393144, 0.1184635};
		   
	double RAD	= PI/180.0;

	double SCV	=	0.20; 	//Scattering coefficient of leaves for visible radiation (PAR)
	double k	= 	0.50;	//The average extinction coefficient for visible and near infrared radiation
	double KDIF = 	0.60;	//Extinction coefficient for diffuse light 


  	//if WheatAndBarley  KDIF =0.6;
  	//if Potato			 KDIF =1.0;
  	//if SugarBeet		 KDIF =0.69;   WAVE, p5-12
    
    PPLANT pPl=GetPlantPoi();

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))	KDIF=1.0;
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))	KDIF=0.69;
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))	KDIF=0.65;
    
							   
	PMAX   = (double)fPgMax;
	EFF    = (double)fLUE;
	vLAI   = (double)fLAI;
	PARDIR = (double)fPARdir;
	PARDIF = (double)fPARdif;
	LAT	   = (double)fLatitude;
	HOUR   = (double)fHour;
    DAY    = (double)nDay;

	//===========================================================================================
	//Sine of the solar height
	//===========================================================================================
	//Declination of the sun as function of Daynumber (vDay)
	DEC = -asin( sin(23.45*RAD)*cos(2.0*PI*(DAY+10.0)/365.0));
	
	//vSin, vCos and vRsc are intermediate variables
	vSin = sin(RAD*LAT)*sin(DEC);
	vCos = cos(RAD*LAT)*cos(DEC);

	SINB  = max(0.0, vSin + vCos*cos(2.0*PI*(HOUR+12.0)/24.0));

	//===========================================================================================
	//Reflection of horizontal and spherical leaf angle distribution
	SQV  = sqrt(1.0-SCV);
	REFH = (1.0-SQV)/(1.0+SQV);
	REFS = REFH*2.0/(1.0+2.0*SINB);

	//Extinction coefficient for direct radiation and total direct flux
	CLUSTF = KDIF / (0.8*SQV); 
	KDIRBL = (0.5/SINB) * CLUSTF;
	KDIRT  = KDIRBL * SQV;

	//===========================================================================================
	//Selection of depth of canopy, canopy assimilation is set to zero
    FGROS = 0;
	for (i=0;i<nGauss;i++)
		{
		LAIC = vLAI * xGauss[i];
		
		//Absorbed fluxes per unit leaf area: diffuse flux, total direct
		//flux, direct component of direct flux.
		VISDF = (1.0-REFH)*PARDIF*KDIF  *exp (-KDIF  *LAIC);
		VIST  = (1.0-REFS)*PARDIR*KDIRT *exp (-KDIRT *LAIC);
		VISD  = (1.0-SCV) *PARDIR*KDIRBL*exp (-KDIRBL*LAIC);
		
		//Absorbed flux (J/M2 leaf/s) for shaded leaves and assimilation of shaded leaves
		 VISSHD = VISDF + VIST - VISD;
		 if (PMAX>0.0) 
		 	FGRSH = PMAX * (1.0-exp(-VISSHD*EFF/PMAX));
		 else
		    FGRSH = 0.0;
		    
		//Direct flux absorbed by leaves perpendicular on direct beam and
		//assimilation of sunlit leaf area
         VISPP  = (1.0-SCV) * PARDIR / SINB;

         FGRSUN = 0.0;
         for (j=0;j<nGauss;j++)
         	{
         	VISSUN = VISSHD + VISPP * xGauss[j];
            
            if (PMAX>0.0) 
            	FGRS = PMAX * (1.0-exp(-VISSUN*EFF/PMAX));
            else
               	FGRS = 0.0;
               	
            FGRSUN = FGRSUN + FGRS * wGauss[j];
            }

		//Fraction sunlit leaf area (FSLLA) and local assimilation rate (FGL)
         FSLLA = CLUSTF * exp(-KDIRBL*LAIC);
         FGL   = FSLLA  * FGRSUN + (1.0-FSLLA) * FGRSH;

		//Integration of local assimilation rate to canopy assimilation (FGROS)
         FGROS = FGROS + FGL * wGauss[i];
         }
              
	FGROS = FGROS * vLAI;
	
	return (float)FGROS;
	}



int WINAPI _loadds DefaultAssimilatePartitioning_SPASS(EXP_POINTER)
	{
	double 		x,vFractionRoot,vFractionLeaf,vFractionStem,vFractionStorage,vFractionSupport;
	float		fTopFraction,fPartFracSupp; 
	PBIOMASS 	pBm =pPl->pBiomass;
	
	x=(double)pPl->pDevelop->fStageSUCROS;
	
	
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))	//Source: SPASS (Wang,1997, p109)
		{
		vFractionRoot =0.3022*pow(x,4)-1.2418*pow(x,3)+1.8016*x*x-1.3653*x+0.4999;
		vFractionRoot =(x<=1.0) ? (vFractionRoot) : (0.0);
		
		vFractionLeaf =-5.382*pow(x,4)+12.189*pow(x,3)-8.0485*x*x+0.2266*x+1.0058;
		vFractionStem =5.382*pow(x,4)-12.189*pow(x,3)+8.0485*x*x-0.2266*x-0.0058;
		vFractionStem =(x<1.0) ? (min(1.0,vFractionStem)):((1.25-x)/(1.25-1.0));

//		vFractionSupport =0.0;

		vFractionSupport =(x<=1.1) ? (max(0.0,1.0-vFractionLeaf-vFractionStem)):(0.0);
		}

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))	//Source: Penning de Vries et al., 1989, p89
		{
		/*
		stimmt so nicht!!
		vFractionRoot =2E-6*pow(x,4)-2E-4*pow(x,3)+5E-3*x*x-0.0778*x+0.5729;
		vFractionLeaf =-5.382*pow(x,4)+12.189*pow(x,3)-8.0485*x*x+0.2266*x+1.0058;
		vFractionStem =5.382*pow(x,4)-12.189*pow(x,3)+8.0485*x*x-0.2266*x-0.0058;
		*/
		
		//ebenfalls nach Penning de Vries et al., 1989, p89
		vFractionRoot =4.851063149*pow(x,4) -8.443020487*pow(x,3) +3.49673349*x*x -0.554776147*x +0.65;
		vFractionRoot =(x<=1.0) ? (vFractionRoot) : (0.0);

		vFractionLeaf = 4.368259694*pow(x,4) -7.999268252*pow(x,3) +3.786273302*x*x -0.975264604*x +0.82;
		vFractionLeaf =(x<=1.0) ? (vFractionLeaf) : (0.0);

		vFractionStem = -4.366483841*pow(x,4) +7.996527499*pow(x,3) -3.785398215*x*x +0.975354092*x +0.18;

		vFractionStem =(x<1.0) ? (min(1.0,vFractionStem)):((1.25-x)/(1.25-1.0));
//		vFractionStem =(x<0.95) ? (min(1.0,vFractionStem)):((1.21-x)/(1.21-0.95));

		vFractionSupport =(x<=1.1) ? (max(0.0,1.0-vFractionLeaf-vFractionStem)):(0.0);
		}
			
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))	//Source: Spitters et al., 1986, p179
		{
		//x=(double)pPl->pDevelop->fStageWang;                                        
		//x=(double)pPl->pDevelop->fStageXCERES;                                        
			
		vFractionRoot 	 = 1.0 - LIMIT(0.5, 1.0, 0.5+0.5*(x-1.0));
		vFractionLeaf 	 = LIMIT(0.0, 0.6, 0.6-(x-1.0));
		vFractionStorage = LIMIT(0.0, 1.00, x-1.0);
		
		//PLAGEN:
		/*vFractionRoot 	 = 1.0 - LIMIT(0.5, 1.0, 0.5+0.5*(x-1.0));
		vFractionLeaf 	 = LIMIT(0.0, 0.6, 0.6-(x-1.0));
		vFractionStorage = LIMIT(0.0, 1.00, x-1.0);*/

		//Spitters et al.:
		/*vFractionRoot 	 = 1.0 - LIMIT(0.8, 1.0, 0.8+0.2*(x-1.0));
		vFractionLeaf 	 = LIMIT(0.0, 0.75, 0.75-(x-1.0));
		vFractionStorage = LIMIT(0.0, 1.00, x-1.0);*/


		vFractionStem 	 = 1.0-vFractionLeaf-vFractionStorage;

		vFractionSupport =0.0;
		}
	
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))	//Grain Maize - Source: Penning de Vries et al., 1989, p89-90
		{
		vFractionRoot =0.5*(1.37-x)/(1.37-0.0);
		vFractionLeaf =3.4937*pow(x,5)-15.738*pow(x,4)+25.665*pow(x,3)-17.777*x*x+3.986*x+0.49;
		vFractionLeaf =LIMIT(0.0, 0.7, vFractionLeaf);
		vFractionStem =5.8302*pow(x,5)-18.436*pow(x,4)+17.878*pow(x,3)-5.1831*x*x+0.0413*x+0.51;
		vFractionStem =LIMIT(0.0, 0.7,vFractionStem);

		vFractionSupport =(x<=1.1) ? (max(0.0,1.0-vFractionLeaf-vFractionStem)):(0.0);
		}



	//Root fraction
	pBm->fPartFracRoot = (float)LIMIT(0.0, 1.0, vFractionRoot/pPl->pPltWater->fStressFacPartition);
	fTopFraction = (float)1.0-pBm->fPartFracRoot;         

	//Leaf, stem, grain fraction
	fPartFracSupp = fTopFraction*(float)LIMIT(0.0, 1.0, vFractionSupport);

	pBm->fPartFracLeaf = fTopFraction*(float)LIMIT(0.0, 1.0, vFractionLeaf);
	pBm->fPartFracStem = fTopFraction*(float)LIMIT(0.0, 1.0, vFractionStem+fPartFracSupp);
	
    pBm->fPartFracGrain= fTopFraction-pBm->fPartFracLeaf-pBm->fPartFracStem;
	

	return 1;
	}

int WINAPI _loadds MainStemLeafing_SPASS(float fMaxLfInitRate,float fMaxLfEmergRate,float fTemp, float fDaylenEff, 
						      float fStageVR, PRESPONSE pResp,PCARDTEMP pCardTemp, POUTPUT pOut);
float WINAPI _loadds LeafAreaGrowthRate_SPASS(float fLfGrwRate,float  fStageVR,float fAveSpcLfWt, 
								   PRESPONSE pResp,PTRANSP pTransp, PORGANNC pLfNc); 
float WINAPI _loadds CerealTillering_SPASS(float fLfEmergRate, float fExpLfNum, float fTillNum, float fTillDem, 
								float fStemWght,    float fPltDens,  float fStageVR);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  CANOPY FORMATION FUNCTION
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds CanopyFormation_SPASS(EXP_POINTER)
	{
	OUTPUT 		Out;
	CARDTEMP	CardTmp;    
	TRANSP		Transp;
	ORGANNC		OrgNc;
	PRESPONSE 	pResp;
	float 		fMaxLfInitRate, fMaxLfEmergRate,fDaylenEff,fStageVR,fTemp,fStemWeight;
	PWEATHER	pWth=pCl->pWeather;
	PDEVELOP	pDev=pPl->pDevelop;
	PCANOPY		pCan=pPl->pCanopy;
	PBIOMASS	pBm =pPl->pBiomass;
	PMSOWINFO	pSI =pMa->pSowInfo;
	
	//Leaf Number initialization
    if (pPl->pDevelop->iDayAftEmerg==1)
    	{ 
		pPl->pCanopy->fLeafPrimodiaNum = (float)0.0;
		pCan->fExpandLeafNum  = (float)0.0;
        }

	//----------------------------------------------------------------------------------------------------
	//Main stem leaf numbers: input variables
	fMaxLfInitRate 	= pPl->pGenotype->fMaxLfInitRate;
	fMaxLfEmergRate	= pPl->pGenotype->fMaxLfAppRate;
	fTemp 			=(pWth->fTempMax+pWth->fTempMin)/(float)2.0;
	fDaylenEff 		= pDev->fDaylengthEff;
	fStageVR		= pDev->fStageSUCROS;
    pResp   		= pPl->pGenotype->LfAppRateTemp;
    CardTmp.fMinTemp= pPl->pGenotype->fTempMinLfApp;
    CardTmp.fOptTemp= pPl->pGenotype->fTempOptLfApp;
    CardTmp.fMaxTemp= pPl->pGenotype->fTempMaxLfApp;
    
	//Leaf emergence rate
	MainStemLeafing_SPASS(fMaxLfInitRate,fMaxLfEmergRate,fTemp, fDaylenEff,fStageVR,pResp,&CardTmp,&Out);

	//Leaf numbers
	pPl->pCanopy->fLeafPrimodiaNum   += Out.fValue1;
	pCan->fLeafAppearR  = Out.fValue2;
	pCan->fLeafTipNum  += pCan->fLeafAppearR;
	pCan->fExpandLeafNum= pCan->fLeafTipNum;	

	//----------------------------------------------------------------------------------------------------
	//Main stem Tiller numbers
	pPl->pCanopy->fMainStemTillers = pCan->fExpandLeafNum-(float)2.5; //SG/25/05/99: wird nie verwendet!
	
	//Tiller number per m2
	pCan->fTillerNumSq = CerealTillering_SPASS(pCan->fLeafAppearR, pCan->fExpandLeafNum, pCan->fTillerNum,
										   pPl->pGenotype->fTillerDemand, pBm->fStemWeight,pSI->fPlantDens,fStageVR);
	//Tiller number per plant
    pCan->fTillerNum   = pCan->fTillerNumSq/pSI->fPlantDens;
	
	//----------------------------------------------------------------------------------------------------
	//Growth of Leaf area index
	/*
    Transp.fPotTransp = pPl->pPltWater->fPotTranspDay;
    Transp.fActTransp = pPl->pPltWater->fActTranspDay;
    */
	Transp.fPotTransp = fPotTraDay;
    Transp.fActTransp = fActTraDay;

//	OrgNc.fOptNc = pPl->pPltNitrogen->fLeafOptConc;
	OrgNc.fOptNc = pPl->pPltNitrogen->fLeafMaxConc;
	OrgNc.fMinNc = pPl->pPltNitrogen->fLeafMinConc;
	OrgNc.fActNc = pPl->pPltNitrogen->fLeafActConc;
	    

	//SG/13/04/99/////////////////////////////////////////////////////////////////////
	//
	//	Im ursprünglichen SPASS werden keine Transpirations- und Stickstoffwerte
	//	übergeben!!
	//	Deshalb sind Wasser- und N-Stress in der Funktion "LeafAreaGrowthRate"
	//	immer = 0.
	//
	//	pCan->fLAI += LeafAreaGrowthRate_SPASS(pBm->fLeafGrowR, fStageVR,pPl->pGenotype->fSpecLfWeight,pPl->pGenotype->SpecLfWght,
	//									   NULL, NULL);
	//
	//	Anpassung an PLAGEN:
	/////////////////////////////////////////////////////////////////////////////////////
 
	if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))
	{
 	//SG 20110629: für Anpassung an Kraichgau-Daten
	pCan->fLAI += (float)0.8*LeafAreaGrowthRate_SPASS(pBm->fLeafGrowR, fStageVR,pPl->pGenotype->fSpecLfWeight,pPl->pGenotype->SpecLfWght,
										   &Transp, &OrgNc);
	}
	else //MZ, BA, PT
	{
			pCan->fLAI += LeafAreaGrowthRate_SPASS(pBm->fLeafGrowR, fStageVR,pPl->pGenotype->fSpecLfWeight,pPl->pGenotype->SpecLfWght,
										   &Transp, &OrgNc);
	}

    
	//=========================================================================================
	//If flowering occurs, calculate the grain number, LAIanthesis and days after anthesis	
	//=========================================================================================
	if (pDev->fStageSUCROS<(float)1.0)
		pPl->pDevelop->iDayAftAnthesis=(float)0.0;
	else
		pPl->pDevelop->iDayAftAnthesis++;
		
	if (pPl->pDevelop->iDayAftAnthesis==1)
		{
//		lpCan->fGrainNum=(float)3500.0E4+(float)1.4E4
//						 *(lpBiom->fLeafWeight+lpBiom->fStemWeight+lpBiom->fStemReserveWeight);
		

  		fStemWeight =pBm->fStemWeight/((float)10.0*pMa->pSowInfo->fPlantDens);	//g per plant
  		
    	pCan->fGrainNum	 = fStemWeight*pPl->pGenotype->fStorageOrganNumCoeff;  			//Grains per plant
			
		if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		{   //Grains per plant
			pCan->fGrainNum	 = pPl->pGenotype->fStorageOrganNumCoeff;  			
		}           
		
    	pCan->fGrainNumSq= pCan->fGrainNum*pMa->pSowInfo->fPlantDens; //Grains per sq meter 	


//		fLAIanthesis =lpCan->fTotalLAI;
		}           
		
	return 1;
	}
                                                     


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds CerealTillering_SPASS(float fLfEmergRate, float fExpLfNum, float fTillNum, float fTillDem, 
//											float fStemWght,    float fPltDens,  float fStageVR)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates tiller number per square meter of cereal crops 
//Inputs:	1. fLfEmergRate	- Leaf emergence rate (lvs/d) calculated by MainStemLeafing()
//			2. fExpLfNum	- Expended leaf number on main stem (leaves) 	
//			3. fTillNum		- Current tiller number per square meter (Tillers/m2) 	
//			4. fTillDem		- Assimilate demand per single tiller (g)
//			5. fStemWght	- Current dry weight of stems (kg/ha)
//			6. fPltDens		- Plant density (plants/m2)
//			7. fStageVR		- Current VR development stage value (-) 	
//Outputs:	1. Return		- Calculated tiller number per square meter (Tillers/m2)
//Functions Called:
//			None 
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds CerealTillering_SPASS(float fLfEmergRate, float fExpLfNum, float fTillNum, float fTillDem, 
								float fStemWght,    float fPltDens,  float fStageVR)
	{
    float fTotTillerDemand, fTillerNumber,fTillerNumSq;

	if (fStageVR>=(float)0.70) return fTillNum*fPltDens;

	if (fExpLfNum < (float)2.5) return fTillNum*fPltDens;
		
	//Tiller number per plant
	if (fStageVR<=(float)0.39)	//Jointing  			
	    //fTillerNumber = fTillNum+ max((float)0.0, fLfEmergRate*(fExpLfNum-(float)2.5));
		//SG/09/06/99/ Alternativvorschlag: (fMaxLfEmergRate in Plant.gtp halbiert)
		fTillerNumber = fTillNum+fLfEmergRate;
	else
	    fTillerNumber = fTillNum;
    
	//Total assimilate demand for all tillers per hacter
    fTotTillerDemand =fPltDens*fTillerNumber*fTillDem*fStageVR*fStageVR*fStageVR*(float)(1E-3*1E+4);

	//Tiller number per square meter
	//SG/25/05/99: Modifikation: fTillerNumSq kann nur wachsen
   	fTillerNumSq = max(fPltDens*fTillNum, fPltDens*max((float)1.0, fTillerNumber*min((float)1.0,fStemWght/fTotTillerDemand)));
	
	return fTillerNumSq;
	}
	    
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: int WINAPI _loadds MainStemLeafing_SPASS(float fMaxLfInitRate,float fMaxLfEmergRate,float fTemp, float fDaylenEff, 
//		  						          float fStageVR, PRESPONSE pResp,PCARDTEMP pCardTemp, POUTPUT pOut)
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates the leaf primodia initiation rate and leaf emergence rate on 
//			the main stem of cereal crops
//Inputs:	1. fMaxLfInitRate	- Maximum leaf initiation rate at optimal temperature (lvs/d)
//			2. fMaxLfEmergRate	- Maximum leaf emergence rate at optimal temperature (Lvs/d) 	
//			3. fTemp			- Current daily average temperature (C) 	
//			4. fDaylenEff		- Daylength effect on development rate (-)
//			5. fStageVR			- Current VR development stage value (-) 	
///			6. pResp			- Pointer to a RESPONSE structure containing temperature response data 
//			7. pCardTemp		- pointer to a CARDTEMP structure containing the cardinal temperature for leafing
//			8. pOut				- Pointer to a OUTPUT structure holding the output values
//Outputs:	1. pOut->fValue1	- Calculated initiation rate of leaf primodia (Primodia/d)
//			2. pOut->fValue2	- Calculated emergence rate of leaf on main stem (Leaves/d)
//Functions Called:
//			None 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the temperature response function for leafing. If not, the cardinal temperatures
//			at pCardTemp will be used to construct the temperature response function. If pCardTemp equals NULL,
//			a minimum, optimum and maximum temperature of 0, 22 and 35C will be assumed respectively.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds MainStemLeafing_SPASS(float fMaxLfInitRate,float fMaxLfEmergRate,float fTemp, float fDaylenEff, 
						      float fStageVR, PRESPONSE pResp,PCARDTEMP pCardTemp, POUTPUT pOut)
	{ 
	float fMinTemp,fOptTemp,fMaxTemp,fTempFunc;  
	float fLfPrimInitRate,fLfEmergRate,fDevFI,fDevFlg;
	      
	
	fDevFI=(float)0.15; fDevFlg = (float)0.65;
	
	//The cardinal temperature for leaf initiation and emergence 
	if (pCardTemp==NULL)
		{
		fMinTemp=(float)0.0;
		fOptTemp=(float)22.0;
		fMaxTemp=(float)35.0;
		}
	else
		{
		fMinTemp=pCardTemp->fMinTemp;
		fOptTemp=pCardTemp->fOptTemp;
		fMaxTemp=pCardTemp->fMaxTemp;
		} 
	
	//The temperature function for leaf initiation and emergence 
	if ((pResp==NULL)||(pResp->fInput==(float)0.0))
		fTempFunc = RelativeTemperatureResponse_SPASS(fTemp,fMinTemp,fOptTemp,fMaxTemp);
	else
		fTempFunc = AFGENERATOR(fTemp, pResp);
		
	//Leaf initiation
	if ((fStageVR>=(float)0.0)&&(fStageVR<=fDevFI))  
		fLfPrimInitRate = fMaxLfInitRate*fTempFunc*fDaylenEff;//fThermEffect*fDaylenEffect;//Leaf appearing rate (leaves /day)
	else
		fLfPrimInitRate = (float)0.0;
		
	//Leaf Appearance
	fLfEmergRate=fMaxLfEmergRate*(float)fTempFunc*fDaylenEff;

	//Flag leaf 
	if (fStageVR>=fDevFlg) fLfEmergRate = (float)0.0; 
	
	pOut->fValue1 = fLfPrimInitRate;
	pOut->fValue2 = fLfEmergRate;
	
	return 1;
	}
	
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	float WINAPI _loadds LeafAreaGrowthRate_SPASS(float fLfGrwRate,float  fStageVR,float fAveSpcLfWt, 
//									   PRESPONSE pResp,PTRANSP pTransp, PORGANNC pLfNc) 
//Author:	Enli Wang
//Date:		10.11.1996
//Purpose:	This function calculates daily leaf area growth rate based on the leaf biomass growth rate and a 
//			development stage-dependent specific leaf weight 
//			the main stem of cereal crops
//Inputs:	1. fLfGrwRate		- Leaf biomass growth rate (kg/ha.d)
//			2. fStageVR			- Current VR development stage value (-) 	
//			3. fAveSpcLfWt		- Average specific leaf weight of the whole growing period (kg/ha leaf) 	
///			4. pResp			- Pointer to a RESPONSE structure containing stage dependent coefficients for
//								  correcting fAveSpcLfWt 
//			5. pTransp			- pointer to a TRANSP structure containing daily actual and potential transpiration
//			6. pLfNc			- Pointer to a ORGANNC structure containing leaf nitrogen concentration data
//Outputs:	1. Return 			- The calculated growth rate of leaf area index (1/d) 
//Functions Called:
//			AFGENERATOR 
//Comments:	This function checks at first the data contained under pResp. If these data are valid, they will be
//			used to construct the development stage response function for specific leaf weight. If not, the 
//			average specific leaf weight will be used through all the stages. If pTransp or pLfNc equals NULL,
//			the water stress or nitrogen stress will not be considered, i.e, leaf grows at optimal conditions.
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds LeafAreaGrowthRate_SPASS(float fLfGrwRate,float  fStageVR,float fAveSpcLfWt, 
								   PRESPONSE pResp,PTRANSP pTransp, PORGANNC pLfNc) 
	{
	float fLfAreaGrwRate,fSpcLfArea,fWaterIndex,fWaterStress,fLfNcIndex,fLfNStress;
	
	//Specific Leaf area
	if ((pResp==NULL)||(pResp->fInput==(float)0.0))  
		fSpcLfArea = (float)1.0/fAveSpcLfWt;
	else
		fSpcLfArea = (float)1.0/(fAveSpcLfWt*AFGENERATOR(fStageVR, pResp));  

	//Water Stress Factor
	if ((pTransp==NULL)||(pTransp->fPotTransp==(float)0.0))
		fWaterStress = (float)1.0;
	else
		{	
		fWaterIndex = pTransp->fActTransp/pTransp->fPotTransp;
		fWaterStress= (float)pow((double)fWaterIndex, 2);
	    }

	//SG/13/04/99//////////////////////////////////////////////////////////////////
	//
	//	Folgende Zeile aus PLAGEN:
	///////////////////////////////////////////////////////////////////////////////
	// if(fStageVR<=(float)0.1) 
	//	fWaterStress = (float)1.0;
	    
	//Nitrogen Stress Factor


	//SG/13/04/99/////////////////////////////////////////////////////////////
	//
	//	Angleichung an PLAGEN:
	//
	//if ((pLfNc==NULL)||(pLfNc->fOptNc==(float)0.0))
	//////////////////////////////////////////////////////////////////////////
	if((pLfNc==NULL)||(pLfNc->fActNc<=(float)0.0))
		fLfNStress = (float)1.0;
	else
		{	
		fLfNcIndex  = (pLfNc->fActNc-pLfNc->fMinNc)/(pLfNc->fOptNc-pLfNc->fMinNc+(float)1e-9);
		fLfNStress = (float)min(1.0,sqrt((double)fLfNcIndex));
	    }

	//Leaf area index growth rate	
    fLfAreaGrwRate = fLfGrwRate*fSpcLfArea*fWaterStress*fLfNStress;

	return fLfAreaGrwRate;
	}
		
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  ROOT SYSTEM FORMATION FUNCTION
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds RootSystemFormation_SPASS(EXP_POINTER)
	{
	
	RootExtension_SPASS		(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	RootLengthDensity_SPASS	(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);

	return 1;
	}

	                                
int WINAPI _loadds RootExtension_SPASS(EXP_POINTER)
	{
	int 				i,L;
	float 				fMaxRootDepthSoil,fMaxRootDepthCrop,DEPMAX,fThickness;
	float 				fDepth,fRelWc,fMoistRed;
	float				fTempMin,fTempMax,fTempOpt,fTempFunc;	

	//SG 20140320
	//float fBDx, fBD0, fAlphaBD, fSoilStrength;


	PSLAYER 			pSL		=pSo->pSLayer->pNext;
	PSWATER				pSWL	=pSo->pSWater->pNext;
	PHLAYER				pSLH	=pHe->pHLayer->pNext;
	PWLAYER	   			pSLW	=pWa->pWLayer->pNext; 
	PROOT				pRT		=pPl->pRoot;  
	PBIOMASS			pBm		=pPl->pBiomass;    

	//Constant Coefficient
	fMaxRootDepthSoil	= (float)200.0; 	//cm depends on soil property
	//fMaxRootDepthCrop   = (float)150.0; 	//cm depends on crop property
	//SG/20/04/99: 
	//	besser 
	fMaxRootDepthCrop = pPl->pGenotype->fMaxRootDepth;
	//	dann wird der Wert aus What.gtp verwendet!!!
	//	(entsprechend für fMaxRootDepthSoil?)

    DEPMAX=(float)0.0;
	pSL	  =pSo->pSLayer->pNext;
	for (L=1;L<=pSo->iLayers-2;L++)
		{
		DEPMAX += (float)0.1*pSL->fThickness;	//cm
		pSL=pSL->pNext;
		}
  
  	pRT->fMaxDepth=min(DEPMAX,min(fMaxRootDepthSoil,fMaxRootDepthCrop));

	//=========================================================================================
	//Root Depth Growth Rate
	//=========================================================================================
    fDepth 	= (float)0.0;
	pSL		=pSo->pSLayer->pNext;
	pSLH	=pHe->pHLayer->pNext;
	pSLW	=pWa->pWLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	for (i=1;i<=pSo->iLayers-2;i++)
		{ 
		fThickness = (float)0.1*pSL->fThickness;
		
		if ((pRT->fDepth>fDepth)&&(pRT->fDepth<=fDepth+fThickness))
			break;
		fDepth += fThickness; 
		
		if (i<pSo->iLayers-2)
			{

			//SG/09/04/99////////////////////////////////////////////////////////////
			//
			//	pSL bzw. pSL->pNext wird doch gar nicht gebraucht. Oder
			//	habe ich etwas übersehen?
			//	Aber: in PLAGEN wird immer der aktuelle Wassergehalt der tiefsten
			//	durchwurzelten Schicht verwendet. Deshalb muß hier pSLW auf pSLW->pNext 
			//	gesetzt werden!!!
			//////////////////////////////////////////////////////////////////////////

			pSLW = pSLW->pNext;
			pSWL = pSWL->pNext;

			pSL = pSL ->pNext;
			pSLH= pSLH->pNext;
		    }   
		}// for

//	if ((lpBm->fRootGrowRate>(float)0.0)&&(pRT->fDepth<fMaxRootDepthSoil))
	if ((pRT->fDepth<fMaxRootDepthSoil))
		{    
		//Temperature Effect

		fTempMin = pPl->pGenotype->fRootExtTempMin;
		fTempOpt = pPl->pGenotype->fRootExtTempOpt; 
		fTempMax = pPl->pGenotype->fRootExtTempMax;
		

		if (pPl->pGenotype->RootExtRateTemp[0].fInput==(float)0.0)
			fTempFunc=(float)RelativeTemperatureResponse_SPASS(pSLH->fSoilTemp,fTempMin,fTempOpt,fTempMax);
		else
			fTempFunc=AFGENERATOR(pSLH->fSoilTemp, pPl->pGenotype->RootExtRateTemp);
		
		//-------------------------------------------------------------------------------
		//Soil water deficit factor for root growth (pLR->fWatStressRoot)
        fRelWc =(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
        
		if (fRelWc<(float)0.25)
			fMoistRed =max((float)0.0,(float)4.0*fRelWc);
		else
			fMoistRed = (float)1.0;

		//////////////////////////////////////////////////////////////////////////////////////
		//SG 20140320: Impact of soil hardness on root penetration (Thesis Hong):
		//fBDx = (float)1.3 + 0.004*pSL->fSand;
		//fBD0 = (float)1.7 + 0.005*pSL->fSand;

		//fAlphaBD = (float)max(0.0, min(1.0,(fBD0-pSL->fBulkDens)/(fBD0-fBDx)));

		//fSoilStrength = fAlphaBD * (float)(sin(PI/2.0*fMoistRed)*pow(sin(PI/2.0*fMoistRed),0.5*pSL->fSand));
		//////////////////////////////////////////////////////////////////////////////////////




		//SG/09/04/99//////////////////////////////////////////////////////////////////
		//
		//	In PLAGEN wird, wenn sich die Wurzeln ausschließlich in der obersten
		//	Schicht befinden, fMoistRed immer = 1.0 gesetzt!
		///////////////////////////////////////////////////////////////////////////////

		// if(i==1)
		//	fMoistRed = (float)1.0;			//SG/05/05/99: Streß auch bei i=1!
		
		pRT->fDepthGrowR = max((float)0.0, pPl->pGenotype->fMaxRootExtRate * fTempFunc * fMoistRed);
		//SG 20140320: with soil hardness
		//pRT->fDepthGrowR = max((float)0.0, pPl->pGenotype->fMaxRootExtRate * fTempFunc * fMoistRed * fSoilStrength);
		}
	else
		pRT->fDepthGrowR = (float)0.0;		
		
	//=========================================================================================
	//Root Depth 
	//=========================================================================================
	
	
	//SG/12/04/99///////////////////////////////////////////////////////////////////////////////
	//
	//	In PLAGEN wird die Wurzeltiefe erst nach der Berechnung der Wurzelverteilung
	//	aktualisiert!!
	////////////////////////////////////////////////////////////////////////////////////////////

	
	pRT->fDepth += pRT->fDepthGrowR;

	pRT->fDepth = min(pRT->fDepth,pRT->fMaxDepth);	

		
	
	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	RootGrowth_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float fSWDF3;
int WINAPI _loadds RootLengthDensity_SPASS(EXP_POINTER)
	{
    int 	L,L1,NLAYR;
    float   fCumDepth,DEPMAX,TRLDF,RNLF,RLVF,fRelExtWater,fNewRootLength,fNewRootGrowFactor,fThickness;

	PSLAYER 			pSL		=pSo->pSLayer;
	PSWATER				pSWL	=pSo->pSWater;
	PHLAYER				pSLH	=pHe->pHLayer;
	PWLAYER	   			pSLW	=pWa->pWLayer; 
	PCLAYER     		pSLN	=pCh->pCLayer;
	PROOT				pRT		=pPl->pRoot;  
	PLAYERROOT			pLR		=pPl->pRoot->pLayerRoot;
	PBIOMASS			pBm		=pPl->pBiomass;    


	if (pBm->fRootGrowR<=(float)0.0)

	//SG/12/04/99///////////////////////////////////////////////////////////////////////////////
	//
	//	In PLAGEN wird die Wurzeltiefe erst nach der Berechnung der Wurzelverteilung
	//	aktualisiert!!
	//	Die folgenden Zeilen wurden deshalb vom Ende der Funktion RootExtension_SPASS
	//	hierher verlegt.
	////////////////////////////////////////////////////////////////////////////////////////////

	{
	//	 pRT->fDepth += pRT->fDepthGrowR;

	//	 pRT->fDepth = min(pRT->fDepth,pRT->fMaxDepth);
	
		return 0;
	}


    NLAYR=pSo->iLayers-2;

    DEPMAX=(float)0.0;
	pSL	  =pSo->pSLayer->pNext;
	for (L=1;L<=NLAYR;L++)
		{
		DEPMAX += (float)0.1*pSL->fThickness;	//cm
		pSL=pSL->pNext;
		}
  
	//pRT->fMaxDepth=DEPMAX;	//SG/20/04/99: wird doch gar nicht gebraucht, oder?
								// Liefert nur falschen Wert am Ende bei Wurzellänge
  	
	//=========================================================================================
	//		New Root Length Calculation (fNewRootLength, RLNEW)


    //SG 20111024: Werte aus spez. Wurzellänge (*.gtp, Zeile 2000080)
    fNewRootLength=pBm->fRootGrowR*pPl->pGenotype->fRootLengthRatio*(float)1E-5;
	
	if(pPl->pGenotype->fRootLengthRatio == (float)0.0)
	{
		fNewRootLength=pBm->fRootGrowR*((float)1.00)*(float)1E-1;

		//      Werte aus E.Wang, Tabelle 3.5 [cm/cm^2]
		//=========================================================================================
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG"))
			fNewRootLength=pBm->fRootGrowR*((float)1.00)*(float)1E-1; 
		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")))
			fNewRootLength=pBm->fRootGrowR*((float)0.80)*(float)1E-1;
		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
			fNewRootLength=pBm->fRootGrowR*((float)1.05)*(float)1E-1;
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
			fNewRootLength=pBm->fRootGrowR*((float)1.70)*(float)1E-1; 
	}

	//=========================================================================================
	//		Factor Calculation for Root Growth
	//=========================================================================================
	fCumDepth	=(float)0.0;
	fSWDF3 		=(float)0.0;
	L = 0;   
	
	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;
 	while ((fCumDepth<=pRT->fDepth)&&(L<NLAYR))
		{
		L++;
		
		fThickness =(float)0.1*pSL->fThickness; //cm

		pSLN->fNO3Nmgkg=(float)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(float)0.01*pSL->fBulkDens*pSL->fThickness;

		fCumDepth +=(float)0.1*pSL->fThickness; //cm
		//-------------------------------------------------------------------------------
		//Soil water deficit factor for root growth (pLR->fWatStressRoot)
        fRelExtWater=(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
		if (fRelExtWater<(float)0.25)
			pLR->fWatStressRoot =max((float)0.0,(float)4.0*fRelExtWater);
		else
			pLR->fWatStressRoot = (float)1.0;

		//SG/12/04/99:	folgende Zeile aus PLAGEN:
		// if(L==1) pLR->fWatStressRoot = (float)1.0;	Streß auch bei L=1!

                                          
        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		//-------------------------------------------------------------------------------
        //Mineral nitrogen availability effect factor on root growth (pLR->fNStressRoot)
		if (TRUE)
			{
			pLR->fNStressRoot =(float)(1.0-(1.17*exp(-0.15*(double)(pSLN->fNO3N/pSLN->fNO3Nmgkg
																   +pSLN->fNH4N/pSLN->fNH4Nmgkg))));

			if ((pLR->fNStressRoot<(float)0.01)&&
 				  ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))))
			   pLR->fNStressRoot=(float)0.01;
			else
				pLR->fNStressRoot=max((float)0.1,pLR->fNStressRoot);


			if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))&&(pLR->fNStressRoot<(float)0.1))
				pLR->fNStressRoot = (float)0.1;
			}
        else
        	pLR->fNStressRoot=(float)1.0;

		//SG/12/04/99:	folgende Zeile aus PLAGEN:
		//if(L==1) pLR->fNStressRoot = (float)1.0;	Streß auch bei L=1!


        //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

         //Root length density factor to calculate new root growth distribution 
         //fNewRootGrowFactor =(float)exp(-4.0*(double)((fCumDepth-(float)0.5*fThickness)/DEPMAX));
		//für Jörg (13.12.99):
		 fNewRootGrowFactor =(float)exp(-2.0*(double)((fCumDepth-(float)0.5*fThickness)/DEPMAX));
         pLR->fLengthDensFac = fNewRootGrowFactor*fThickness*min(pLR->fWatStressRoot,pLR->fNStressRoot);
  		if ((fCumDepth<pRT->fDepth)&&(L<NLAYR))
		 	fSWDF3 += (pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP)*fThickness;

		if (L<NLAYR-1)
			{
			pSL =pSL ->pNext;
			pSWL=pSWL->pNext;
			pSLW=pSLW->pNext;
			pSLN=pSLN->pNext;
			pLR =pLR ->pNext;
			}
		}  
		L1=L;
	//=========================================================================================
	//	Deepest root layer
	//=========================================================================================
	if (L<=NLAYR)
		{
	    pSL=pSL->pBack;
		pLR=pLR->pBack;
		}  	

	//auf Wunsch von Jörg Lippert geändert am 13.12.99
//	pLR->fLengthDensFac=pLR->fLengthDensFac*((float)1.0-(fCumDepth-pRT->fDepth)/((float)0.1*pSL->fThickness));

	pLR->fLengthDensFac=pLR->fLengthDensFac*(float)1.0;
	if(fCumDepth>pRT->fDepth)
		pLR->fLengthDensFac=(float)0.0;


	//=========================================================================================
	//		Total Root Length Density Factor
	//=========================================================================================
	TRLDF =(float)0.0;
	pLR	  =pPl->pRoot->pLayerRoot;
	for (L=1;L<=L1;L++)
		{
		TRLDF += pLR->fLengthDensFac;
        pLR    = pLR->pNext;
        }
	//=========================================================================================
	//		Root Length Density Calculation
	//=========================================================================================
	//if (TRLDF>=fNewRootLength*((float)0.00001))
        if ((TRLDF>0)&&(TRLDF>=fNewRootLength*((float)0.00001))) //SG 20160726: Abfangen von TRLDF = 0; tritt auf, wenn Saattiefe < 1.Schichtdicke
		RNLF = fNewRootLength/TRLDF; 
	else
		return 0;

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;
	fCumDepth=(float)0.0;
	for (L=1;L<=L1;L++)
		{//Fow Wheat and Barley
		fThickness =(float)0.1*pSL->fThickness; //cm

		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))|(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
			{
        	fCumDepth +=fThickness;
          
            pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-((float)0.01)*pLR->fLengthDens;
            if (fCumDepth<(float)115.0)
                pLR->fLengthDens=max((float)0.0,pLR->fLengthDens);
	      	else
	      		{
	           	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		         	RLVF=(float)0.377-((float)0.0015)*fCumDepth;
	    		else
	           		RLVF=((float)0.00004)*(fCumDepth-(float)220.0)*(fCumDepth-(float)220.0);
	            pLR->fLengthDens=min(RLVF,pLR->fLengthDens);
	            }
	      	}
		
		//SG/17/05/99: Potato (ausPLAGEN)
		else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
       		{
				pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-(float)0.005*pLR->fLengthDens; 
				pLR->fLengthDens  = min((float)8.0, max((float)0.0,pLR->fLengthDens));
			}

		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
			{
				if(pPl->pDevelop->iStageCERES < 4)
					pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness;
				else
					pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness-(float)0.005*pLR->fLengthDens;

				pLR->fLengthDens = (float)min(10.0,max(0.0,pLR->fLengthDens));
			}
		else //For Other crops
           	{
            pLR->fLengthDens += pLR->fLengthDensFac*RNLF/fThickness
            						    -((float)0.005)*pLR->fLengthDens;
            pLR->fLengthDens=min((float)5.0, max((float)0.0,pLR->fLengthDens));
            }

		pSL =pSL->pNext;
		pLR =pLR->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext; 
		}



	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  ORGAN SENESCENCE FUNCTION
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds OrganSenescence_SPASS(EXP_POINTER)
	{
	float fRelDeathRateLv,fRelDeathRateRt,RDR,fFlvNc,fFrtNc,fTemp;
 	float LAICR,DvsCR;
 	
	PWEATHER		pWth = pCl->pWeather;
	PBIOMASS		pBiom= pPl->pBiomass;
	PDEVELOP		pDev = pPl->pDevelop;
	PCANOPY     	pCan = pPl->pCanopy;   
	PPLTNITROGEN 	pPltN= pPl->pPltNitrogen;
	
	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
		return 0;

   	LAICR = pPl->pGenotype->fBeginShadeLAI;	//4.0; //Critical LAI over which leaf dies due to shading
    DvsCR = pPl->pGenotype->fBeginSenesDvs; //1.0
    
	//=========================================================================================
	// Effect of nitrogne content
	fFlvNc = (float)1.0/max((float)0.5,pPl->pPltNitrogen->fLeafRelNc);  //0.5 sg:doch wieder 0.5 statt 0.3
	fFrtNc = (float)1.0/max((float)0.5,pPl->pPltNitrogen->fRootRelNc);  //0.5

	
	//SG/18/05/99: Potato aus PLAGEN!
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
	  {
	   fFlvNc = (float)1.0/max((float)0.1,pPl->pPltNitrogen->fLeafRelNc);  //0.5
	   fFrtNc = (float)1.0/max((float)0.1,pPl->pPltNitrogen->fRootRelNc);  //0.5
	  }


	//lpPlt->pPltNitrogen->fNDEF2 = max((float)0.3,pPl->pPltNitrogen->fLeafRelNc);
	//=========================================================================================
	//Temperature Effect  
	fTemp = (float)0.5*(pWth->fTempMax+pWth->fTempMin);
	RDR = AFGENERATOR(fTemp,pPl->pGenotype->LvDeathRateTemp);

	//=========================================================================================
	//Leaf death rate due to developmental age 
	if (pDev->fStageSUCROS<DvsCR)
		pPl->pCanopy->fLeafAgeDeathRate = (float)(0.0);
	else
		{
		 //pPl->pCanopy->fLeafAgeDeathRate = RDR*(float)pow((double)pDev->fStageSUCROS, 1.5);
			
		 //SG 20110629: Für Anpassung an Kraichgau-Daten wie in Dokumentation (Diss Enli Wang)
		 pPl->pCanopy->fLeafAgeDeathRate = (float)0.5*RDR;

		//SG 20111118: alte Version für AgMip aktiviert
		//pPl->pCanopy->fLeafAgeDeathRate = RDR*(float)pow((double)pDev->fStageSUCROS, 1.5);

		//SG20140909: für AgMIP-Maize Phase II
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		{
			pPl->pCanopy->fLeafAgeDeathRate = (float)1.0*RDR*(float)(pow(((double)pDev->fStageSUCROS-DvsCR)/((float)2.-DvsCR), 2));
		}

	//SG/18/05/99: Potato aus PLAGEN!
	    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
	      {
	       pPl->pCanopy->fLeafAgeDeathRate = RDR*(float)(6.0*pow((double)pDev->fStageSUCROS-1, 3.0)); 
	       if (!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Agria"))//Agria=^3 Christa=^1.5	
		     pPl->pCanopy->fLeafAgeDeathRate = RDR*(float)(6.0*pow(((double)pDev->fStageSUCROS-DvsCR)/((float)2.-DvsCR), 3.0)); 
	       if (!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Christa"))	
		     pPl->pCanopy->fLeafAgeDeathRate = RDR*(float)(6.0*pow(((double)pDev->fStageSUCROS-DvsCR)/((float)2.-DvsCR), 1.5)); 
  	      }
			
		}

	//=========================================================================================
	//Leaf death rate due to shading   
	pPl->pCanopy->fLeafShadeDeathRate=(float)LIMIT((double)0.0, RDR, RDR*(pCan->fLAI-LAICR)/LAICR);

	//=========================================================================================
 	//Relative leaf death rate due to nitrogen deficiency
    fRelDeathRateLv = fFlvNc*max(pPl->pCanopy->fLeafAgeDeathRate, pPl->pCanopy->fLeafShadeDeathRate);
 	pPl->pCanopy->fRelLeafDeathRate =fRelDeathRateLv; 
	
//	if (pDev->fZStage>=(float)8.5)
//	 	pCan->fRelLeafDeathRate = (float)2.0*fRelDeathRateLv; 
	//=========================================================================================
	//Dead leaf area index and biomass weight
	pPl->pCanopy->fLeafAreaLossRate = pCan->fLAI*pPl->pCanopy->fRelLeafDeathRate;
	pPl->pBiomass->fLeafDeathRate   = fRelDeathRateLv*pBiom->fLeafWeight;

	pCan->fLAI -= pPl->pCanopy->fLeafAreaLossRate;

	//===========================================================================================
	//Death rate of Root
	//===========================================================================================
	//Temperature Effect
	RDR = AFGENERATOR(fTemp,pPl->pGenotype->RtDeathRateTemp);

	//if (pDev->fStageSUCROS<(float)1) 
	if (pDev->fStageSUCROS<DvsCR) 
		fRelDeathRateRt= (float)0.0;
	else
		fRelDeathRateRt = RDR;

	//===========================================================================================
 	//Relative Root death rate due to nitrogen deficiency
    fRelDeathRateRt      = fFrtNc*fRelDeathRateRt;
	pPl->pBiomass->fRootDeathRate = fRelDeathRateRt*pBiom->fRootWeight;

	pPl->pBiomass->fDeadLeafWeight += pPl->pBiomass->fLeafDeathRate;
	pPl->pBiomass->fDeadRootWeight += pPl->pBiomass->fRootDeathRate;

	//Dry weight of different organs	                          
	pBiom->fLeafWeight 		-= pPl->pBiomass->fLeafDeathRate; 
	pBiom->fStemWeight  	-= pPl->pBiomass->fReserveTransRate;
	pBiom->fRootWeight  	-= pPl->pBiomass->fRootDeathRate;

	pPl->pBiomass->fTotLeafWeight 	= pBiom->fLeafWeight+pPl->pBiomass->fDeadLeafWeight; 
	pPl->pBiomass->fTotRootWeight  	= pBiom->fRootWeight+pPl->pBiomass->fDeadRootWeight;
	pPl->pBiomass->fTotStemWeight  	= pBiom->fStemWeight+pPl->pBiomass->fDeadStemWeight;

	pBiom->fBiomassAbvGround = pPl->pBiomass->fTotLeafWeight+pPl->pBiomass->fTotStemWeight+pBiom->fGrainWeight;
	pBiom->fTotalBiomass     = pBiom->fBiomassAbvGround + pBiom->fRootWeight;

	return 1;
	}
                                        

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  int WINAPI _loadds WaterCapture_SPASS(EXP_POINTER)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds WaterCapture_SPASS(EXP_POINTER)
	{
	PWEATHER	pWth =pCl->pWeather;
    PCANOPY		pCan =pPl->pCanopy;
    PPLTWATER	pPltW=pPl->pPltWater;
    
	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
		return 0;
	
	pPltW->fInterceptDay		= CanopyWaterInterception_SPASS(pCan->fLAI,pWth->fRainAmount);
	//pPltW->fPotTranspDay	= PotentialTranspiration_SPASS(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	fPotTraDay	= PotentialTranspiration_SPASS(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);

	ActualTranspiration_SPASS	(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	PlantWaterStress_SPASS		(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float WINAPI _loadds CanopyWaterInterception_SPASS(float fLAI,float fRainfall)
//Author:	Enli Wang
//Date:		15.11.1996
//Purpose:	This function uses daily rainfall to calculate the daily water interception by the canopy
//Inputs:	1. fLAI 	- Active leaf area of the canopy (-) 	
//			2. fRainfall- Daily amount of rainfall (mm)
//Outputs:	1. Return 	- Daily water interception (mm)
//Functions Called: 
//			None
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds CanopyWaterInterception_SPASS(float fLAI,float fRainfall)
	{
	float fInterceptionCapacity;	//The interception capacity per layer of leaves (mm/d)
	float fInterception;
	
	fInterceptionCapacity = (float)0.25;
	
	fInterception = fInterceptionCapacity*fLAI;
	
	if (fInterception>fRainfall)
		fInterception=fRainfall;
		
	return fInterception;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  POTENTIAL TRANSPIRATON FUNCTION
//------------------------------------------------------------------------------------------------------------
float WINAPI _loadds PotentialTranspiration_SPASS(EXP_POINTER)
	{
  	 float DeltaT = pTi->pTimeStep->fAct;
     float fPotTr;

	/* daily potential transpiration */
	fPotTr = pWa->fPotETDay - pWa->pEvap->fPotDay;
	//pPl->pPltWater->fPotTranspDay = fPotTr;
	fPotTraDay = fPotTr;
	
	/* time step potential transpiration */
	//pPl->pPltWater->fPotTranspdt = pWa->fPotETdt - pWa->pEvap->fPotR * DeltaT;
	
	return fPotTr;
	}


///*
float WINAPI _loadds ExtraterrestrialRadiation_SPASS(float fLatitude, int nJulianDay);

float WINAPI _loadds PotentialTranspiration_SPASSAlt(EXP_POINTER)
	{
	double vGlobalRadiation,vLongRadIn,vLongRadOut,vRadMax,vNetRad,vDry;
	double vPotTr,vPotTrRad,vPotTrDry;
    double vSatVP,vSatVPave,vActVP,vSlope,vWind,vWindDay,vWindR,vTempDay,vTempAve,vRH,vDaylen,vCa,vCi;
    double vLeafWidth,vPltHght,vLAI,vPSgross,vPn,vMaxPs,vMaintLv;
    double Rbv,Rcv,Rlv,Rlvm,Rlvx,Rch,Rbh,vZr,vZo,vZd;
    double vBoltzm,vPsych,vLH,vVHair,vAlbedo,vPsych1;

	PWEATHER	pWth=pCl->pWeather;   
	PDEVELOP	pDev=pPl->pDevelop; 
	PPLTCARBON	pPlC=pPl->pPltCarbon;

	CARDTEMP CardTmp;	//SG/15/04/99: für Anpassung an PLAGEN;
	ORGANNC OrgNc;
	float	fCO2Cmp,fCO2R,fPgmmax,fMaxLUE;

	    
	vGlobalRadiation	= (double)	pWth->fSolRad*1E6;
	vTempDay			= (double)  ((float)0.71*pWth->fTempMax+(float)0.29*pWth->fTempMin);
	vTempAve			= (double)  ((float)0.5*(pWth->fTempMax+pWth->fTempMin));
	vRH					= (double)	pWth->fHumidity;
	vWind				= (double)	pWth->fWindSpeed;

	vDaylen				= (double)  DaylengthAndPhotoperiod_SPASS(pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay,1);
	vRadMax 			= (double)  ExtraterrestrialRadiation_SPASS(pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay);
	vLAI				= (double)	pPl->pCanopy->fLAI;

	if (vWind==-99.00)
		vWind=2.0;
	if (vRH==-99.00)
		vRH=65.0;
	//______________________________________________________________________________
	//Constants
	vBoltzm = 4.8972E-3; 	//Stefan-Boltznan constant   (J /m2/d/K4)
	vPsych	= 0.67;		//Psychrometric instrument constant (mbar/C)	 
	vLH		= 2.4E6;	//Latent heat of evaporation of water (J/kg H2O)
	vVHair	= 1200;		//Volumetric heat capacity of the air (J/m2/K)
    vAlbedo = 0.25;     //Leaf albedo
	//______________________________________________________________________________
	//Boundary Layer Resistance                                 
	vWindDay = 1.333*max(0.2,vWind);
	vLeafWidth = 0.01; //m
	
	Rbv = 0.5*172*sqrt(vLeafWidth/(vWindDay*0.6)); 
	
	//______________________________________________________________________________
	//Canopy resistance
	vPltHght = (double)pPl->pGenotype->fPlantHeight*min(1.0, (double)pDev->fStageSUCROS)/100.0;
	vWindR   = max(0.2, 1.333*vWind);
	
	vZd = max(0.10, 0.63*vPltHght);
	vZo = max(0.05, 0.10*vPltHght);
	vZr = max(2.00, 1.0+vPltHght);
	
	Rcv = 0.74*max(1.0,vLAI)*pow(log((vZr-vZd)/vZo),2)/(0.16*vWindR);

	//______________________________________________________________________________
	//Leaf resistance
	vCa 	= (double)CO2;
	vCi 	= vCa*(double)pPl->pGenotype->fCiCaRatio;


	//SG/15/04/99//////////////////////////////////////////////////////
	//	Fehler!
	//	vMaxPs ist nicht die maximale Photosyntheserate eines Blatts,
	//	sondern die tatsächliche! (siehe Doktorarbeit, Gl.3.2.122)
	//
	//	vMaxPs 	= (double)pPl->pGenotype->fMaxGrossPs;

	fPgmmax = pPl->pGenotype->fMaxGrossPs;
	fMaxLUE = pPl->pGenotype->fLightUseEff;

	CardTmp.fMinTemp = pPl->pGenotype->fTempMinPs;
	CardTmp.fOptTemp = pPl->pGenotype->fTempOptPs;
	CardTmp.fMaxTemp = pPl->pGenotype->fTempMaxPs;

	OrgNc.fMinNc = pPl->pPltNitrogen->fLeafMinConc;
	OrgNc.fActNc = pPl->pPltNitrogen->fLeafActConc;
	OrgNc.fMaxOptNc = pPl->pPltNitrogen->fLeafOptConc;

	fCO2Cmp = pPl->pGenotype->fCO2CmpPoint;
	fCO2R = pPl->pGenotype->fCiCaRatio;
	
	vMaxPs = LeafMaxGrossPhotosynthesis_SPASS(fPgmmax,(float)vTempDay,
		(float)vCa,fCO2Cmp,fCO2R,pPl->pGenotype->PmaxTemp,&CardTmp,&OrgNc,pPl);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//SG/15/04/99//////////////////////////////////////////////////////////////
	//
	//	An dieser Stelle wird in PLAGEN die Brutto-Photosynthese neu berechnet.
	//	
	//	vPSgross= (double)pPlC->fGrossPhotosynR;

	vPSgross = DailyCanopyGrossPhotosythesis_SPASS("C3",fPgmmax,fMaxLUE,fCO2Cmp,fCO2R,
		pPl->pGenotype->PmaxTemp,&CardTmp,&OrgNc,(float)vLAI,pLo->pFarm->fLatitude, 
		(int)pTi->pSimTime->iJulianDay,pWth->fSolRad,pWth->fTempMax,pWth->fTempMin,(float)vCa,pPl);
	
	Rlvm = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(0.9*(vMaxPs+1E-9))/1.6-10.0;
	Rlvx = 2000.0; //s/m


		vMaintLv= (double)pPlC->fMainRespRLeaf;

	//////////////////////////////////////////////////////////////////////////

	vPn = max(0.0, vPSgross/vDaylen - 0.25*(vMaintLv/0.75)/24.0);
	vPn = vPn/min(2.5, vLAI);
	Rlv = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(vPn+1E-9)/1.6-Rbv-Rcv;
	
	Rlv = (double)LIMIT((float)Rlvm,(float)Rlvx,(float)Rlv);
		
	//______________________________________________________________________________
   //Apparent Psychrometric constant (mbar/C)
	Rbh = Rbv/0.93;
	Rch = Rcv;
	
	vPsych1	= vPsych*(Rlv+Rbv+Rcv)/(Rbh+Rch);		//Psychrometric instrument constant (mbar/C)	 

	//______________________________________________________________________________
	//Saturated and actural vapour pressure of the air (mbar)
	vSatVP = 0.1*6.11*exp(17.4*vTempDay/(vTempDay+239.0));
	vSlope = 4158.6*10*vSatVP/((vTempDay+239.0)*(vTempDay+239.0));

	vSatVPave = 0.1*6.11*exp(17.4*vTempAve/(vTempAve+239.0));
	vActVP = vSatVPave*vRH/100.0;
	
	//______________________________________________________________________________
	//Radiation Term 
	vLongRadIn =vBoltzm*pow((vTempDay+273.0),4)*(0.618+0.00365*sqrt(10*vActVP));
	vLongRadOut=vBoltzm*pow((vTempDay+273.0),4);
 
 	vNetRad = (1.0-vAlbedo)*vGlobalRadiation
		-(vLongRadOut-vLongRadIn)*(vGlobalRadiation/(0.75*vRadMax*1E6))*(vDaylen/24.0);

	vPotTrRad = vNetRad/vLH*(vSlope/(vSlope+vPsych1));
	
	//______________________________________________________________________________
	//Drying Term
	vDry = 10.0*(vSatVP-vActVP)*vVHair/(Rbh+Rch)*(vDaylen/24.0);
	vPotTrDry=vDry/vLH/(vSlope+vPsych1)*(24.0*3600.0);
	
	vPotTr  = vPotTrRad*(1.0-exp(-0.5*vLAI)) + vPotTrDry*min(2.5,vLAI);

	//pPl->pPltWater->fPotTranspDay = (float)vPotTr;
	fPotTraDay = (float)vPotTr;
	 



	//?????????????????????????????????????????????????????????
	//For setting the Expert_n Variable pPl->pPltWater->fPotTranspdt

	//pPl->pPltWater->fPotTranspdt = pPl->pPltWater->fPotTranspDay*DeltaT;
	
	return (float)vPotTr;
	}
//*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Daylength and Photoperiod
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI DaylengthAndPhotoperiod_SPASS(float fLatitude, int nJulianDay, int nID)
	{
	double SINLD=0.0;
	double COSLD=0.0;
	double vLatitude=0.0;
	double vDay=0.0;
	double vDec=0.0;
	double RAD=0.0;
	float  fReturn=(float)0.0;
	float  fDaylengthAst=(float)0.0;
	float  fPhotoperiod=(float)0.0;
	float  fDaylengthPs=(float)0.0;
	
	RAD 	  = PI/180.0;
	vLatitude = (double)fLatitude;
	vDay 	  = (double)nJulianDay;

	//Declination of the sun as function of iJulianDaynumber (iJulianDay)
//	vDec = -asin( sin(23.45*RAD)*cos(2.0*PI*(vDay+10.0)/365.0));
	vDec = sin(23.45*RAD)*cos(2.0*PI*(vDay+10.0)/365.0);
	vDec = - asin(vDec);

	//Intermediate variables
	SINLD = sin(RAD*vLatitude)*sin(vDec);
	COSLD = cos(RAD*vLatitude)*cos(vDec);
	
	//Astronomical daylength (hr) 
	fDaylengthAst  =(float)(12.0*(1.0+2.0*asin(SINLD/COSLD)/PI));         
   
   	//Photoperiodically active daylength (hr)
	fPhotoperiod =(float)(12.0*(1.0+2.0*asin((-sin(-4.0*RAD)+SINLD)/COSLD)/PI));         

   	//Photosythesis active daylength (hr)
	fDaylengthPs =(float)(12.0*(1.0+2.0*asin((-sin(8.0*RAD)+SINLD)/COSLD)/PI));         
   

	fReturn=fDaylengthAst;

	if (nID==1)
		fReturn=fDaylengthAst;
	if (nID==2)
		fReturn=fPhotoperiod;
	if (nID==3)
		fReturn=fDaylengthPs;
	
	return fReturn;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//MaximumRadiation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds ExtraterrestrialRadiation_SPASS(float fLatitude, int nJulianDay)
	{
	//Stapper (1984)
	double vLatitude,vDay,vDec,RAD,AOB,SC,DAYL,DSINB;
	float  fRadMax;
	
	RAD 	  = PI/180.0;
	vLatitude = (double)fLatitude;
	vDay 	  = (double)nJulianDay;
	
	//Declination of the sun as function of iJulianDaynumber (iJulianDay)
	vDec = -asin( sin(23.45*RAD)*cos(2.0*PI*(vDay+10.0)/365.0));
	
	//AOB are intermediate variables
	AOB   = sin(RAD*vLatitude)*sin(vDec)/cos(RAD*vLatitude)*cos(vDec);
	
	//Daylength (DAYL) 
	DAYL   = 12.0*(1.0+2.0*asin(AOB)/PI);

	DSINB  = 3600.0*(DAYL*sin(RAD*vLatitude)*sin(vDec)+24.0*cos(RAD*vLatitude)*cos(vDec)
				*sqrt(1.0-AOB*AOB)/PI);

	//Solar constant (SC) 
	SC  = 1370.0*(1.0+0.033*cos(2.0*PI*vDay/365.0));
	
	fRadMax=(float)(SC*DSINB*1E-6); 	//MJ/m2.d

	return fRadMax;
	}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  ACTUAL TRANSPIRATON FUNCTION
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds ActualTranspiration_SPASS(EXP_POINTER)
	{
	int   			L;
	float 			TRWU,WUF,TLL,RWUMX,fExtWater,fTotSoilWater,fThickness,fContAct;

	PSLAYER 		pSL		=pSo->pSLayer;
	PSWATER			pSWL	=pSo->pSWater;
	PWLAYER	   		pSLW	=pWa->pWLayer; 
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;
	PROOT			pRT		=pPl->pRoot;  
    PPLTWATER		pPltW	=pPl->pPltWater;
	
    
// RWUMX =(float)0.03;
// RWUMX =pPl->pGenotype->fMaxWuptRate;   

	//SG 20120126: 
	// Abnahme der maximalen Wasseraufnahmerate RWUMX (eigentl. des Gesamtwiderstands 
	// des Xylems inkl. Stomatawiderstand!) nach Beginn der Seneszenz
    RWUMX =pPl->pGenotype->fMaxWuptRate * (float)min(1.0,max(0.0,((float)2.1 - pPl->pDevelop->fStageSUCROS)/((float)2.1 - pPl->pGenotype->fBeginSenesDvs)));
       

	pLR	=pPl->pRoot->pLayerRoot;

    for (L=1;L<=pSo->iLayers-2;L++)
    	{
		pLR->fActWatUpt	   	=(float)0.0;
		pLR->fActLayWatUpt 	=(float)0.0;
		pLR->fPotWatUpt		=(float)0.0;
		pLR->fPotLayWatUpt	=(float)0.0;

      	pLR =pLR ->pNext;
		}
/*
   	if (pPltW->fPotTranspDay<=(float)0.001)
   		{
        pPltW->fActTranspDay	=(float)0.0;
    	pPltW->fStressFacPhoto 	= (float)1.0;
		pPltW->fStressFacLeaf 	= (float)1.0;
 		return 0;
		}
*/	
	if (fPotTraDay<=(float)0.001)
   		{
        fActTraDay	=(float)0.0;
    	pPltW->fStressFacPhoto 	= (float)1.0;
		pPltW->fStressFacLeaf 	= (float)1.0;
 		return 0;
		}

	//=======================================================================================
	//			Root Water Uptake
	//=======================================================================================
	L 	  = 1;
	TRWU  =(float)0.0;
	pPl->pPltWater->fCumShortage = (float)0.0;
	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	pLR	=pPl->pRoot->pLayerRoot;
	while ((pLR->fLengthDens!=(float)0.0)&&(L<pSo->iLayers-2))
		{
		fThickness = (float)0.1*pSL->fThickness; //mm to cm
		
        //Root water uptake per unit root length
        fExtWater = max((float)0.0,pSLW->fContAct-pSWL->fContPWP);

        pPl->pPltWater->fCumShortage += fExtWater;

        if (pSLW->fContAct<=pSWL->fContPWP)
        	pLR->fPotWatUpt=(float)0.0;
        else
        	{
        	pLR->fPotWatUpt=(float)(2.67*1E-3*exp(min(62.0*(double)fExtWater,10.0))
            								/(6.68-log((double)pLR->fLengthDens)));
            pLR->fPotWatUpt=min(RWUMX,pLR->fPotWatUpt);
            }

		//Root water uptake per layer
		pLR->fPotLayWatUpt=pLR->fPotWatUpt*fThickness*pLR->fLengthDens*(float)10.0
								*((float)0.18+(float)0.00272*(pLR->fLengthDens-(float)18.0)
			 					   						    *(pLR->fLengthDens-(float)18.0));
		if ((pPl->pDevelop->fCumPhyll<(float)4.0)&&(pPl->pCanopy->fLAI<(float)1.0))
			pLR->fPotLayWatUpt *= ((float)3.0-(float)2.0*pPl->pCanopy->fLAI);

		TRWU += pLR->fPotLayWatUpt;
		L 	 ++;

      	pSL =pSL ->pNext;
      	pSWL=pSWL->pNext;
      	pSLW=pSLW->pNext;
      	pLR =pLR ->pNext;
      	}
   
   	pPl->pPltWater->fPotUptakedt=TRWU;
	//=======================================================================================
	//			Root Water Uptake Modification  and Soil Water Content
	//=======================================================================================
	/*
	if (pPltW->fPotTranspDay<=TRWU)
    	WUF = pPltW->fPotTranspDay/TRWU;
	*/
	if (fPotTraDay<=TRWU)
    	WUF = fPotTraDay/TRWU;
    else
        WUF = (float)1.0;


    TLL=(float)0.0;

	fTotSoilWater=(float)0.0;

 	pPl->pPltWater->fActUptakedt=(float)0.0;

	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	pLR	=pPl->pRoot->pLayerRoot;
    for (L=1;L<=pSo->iLayers-2;L++)
    	{
		pLR->fActWatUpt	 	=  pLR->fPotWatUpt   *WUF;
		pLR->fActLayWatUpt 	=  pLR->fPotLayWatUpt*WUF;

		fContAct  = pSLW->fContAct;
		fContAct -= pLR->fActLayWatUpt/pSL->fThickness; //mm/mm

		if (fContAct<pSWL->fContPWP)
			{
			pLR->fActWatUpt	   *= fContAct/pSWL->fContPWP;
			pLR->fActLayWatUpt *= fContAct/pSWL->fContPWP;
//			pSLW->fContAct=pSWL->fContPWP;
			}

		fTotSoilWater 	+= fContAct*fThickness*(float)10.0;

      	pPl->pPltWater->fActUptakedt += pLR->fActLayWatUpt;
		TLL   +=pSWL->fContPWP;

      	pSL =pSL ->pNext;
      	pSWL=pSWL->pNext;
      	pSLW=pSLW->pNext;
      	pLR =pLR ->pNext;
		}

//    lpSoil->lpWater->fTotExtSoilWater = lpSoil->lpWater->fTotSoilWater-TLL;

/*
   if (pPltW->fPotTranspDay>=TRWU)
        pPltW->fActTranspDay=TRWU;
    else
        pPltW->fActTranspDay=pPltW->fPotTranspDay;
        
	pPl->pRoot->fUptakeR = pPltW->fActTranspDay;    
	
*/
   if (fPotTraDay>=TRWU)
        fActTraDay=TRWU;
    else
        fActTraDay=fPotTraDay;
        
	pPl->pRoot->fUptakeR = pPl->pPltWater->fActUptakedt;    
	//eigentlich: 	pPl->pRoot->fUptakeR = fActTraDay;    
	

	//=======================================================================================
	//	Water Deficit and water demand
	//=======================================================================================
//	pPltW->fWaterShortage     =pPltW->fPotTranspdt-pPltW->fActTranspDay;	
//	pPltW->fCumWaterDemand   += pPltW->fPotTranspDay;
//	pPltW->fCumWaterShortage += pPltW->fWaterShortage;
	
	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantWaterStress_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantWaterStress_SPASS(EXP_POINTER)
	{  
	PPLTWATER 	pPltW = pPl->pPltWater;

//	float  WsFact[] = {3,  0.0, 1.0, 2.0};
//	float  WsDevR[] = {3,  1.0, 1.0, 1.0};
	
	//Stress factor on development rate
//	pPltW->fWsDevRate = AFGEN(pPltW->fSWDF1, WsFact, WsDevR);

	//Direct effect of air humidity on Pmax
	//pPltW->fWsPsRH  = AFGEN(VPDC );

	//=======================================================================================
	//	Plant water stress factor
	//=======================================================================================
	//Stress factor for CO2 assimilation
	//pPltW->fStressFacPhoto=min((float)1.0, pPltW->fActTranspDay/pPltW->fPotTranspDay);
	pPltW->fStressFacPhoto=min((float)1.0, fActTraDay/fPotTraDay);
    
	//Stress factor on expension growth
   	//pPltW->fStressFacLeaf = min((float)1.0, (float)0.67*pPl->pPltWater->fPotUptakedt/pPltW->fPotTranspDay);
   	pPltW->fStressFacLeaf = min((float)1.0, (float)0.67*pPl->pPltWater->fPotUptakedt/fPotTraDay);

	//Stress factor on carbohydrate partitioning
	pPl->pPltWater->fStressFacPartition = min((float)1.0,(float)0.5+pPltW->fStressFacPhoto);

	if (pPl->pDevelop->iStageCERES<2)
		{
		pPltW->fStressFacPhoto		=(float)1.0;
		pPltW->fStressFacLeaf		=(float)1.0;
		pPltW->fStressFacTiller		=(float)1.0;
		pPl->pPltWater->fStressFacPartition			=(float)1.0;
		}

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  NITROGEN DEMAND & UPTAKE FUNCTION
//------------------------------------------------------------------------------------------------------------
int WINAPI _loadds NitrogenCapture_SPASS(EXP_POINTER)
	{

	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
		return 0;

	NitrogenConcentrationLimits_SPASS	(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	PlantNitrogenDemand_SPASS			(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	NitrogenUptake_SPASS				(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	PlantNitrogenStress_SPASS			(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);
	
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	NitrogenConcentrationLimits_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int WINAPI _loadds NitrogenConcentrationLimits_SPASS(EXP_POINTER)
	{            
	PDEVELOP		pDev  = pPl->pDevelop;
    PPLTNITROGEN	pPltN = pPl->pPltNitrogen;
    
	
	pPltN->fLeafMaxConc = AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->OptLvNc);	
	pPltN->fStemMaxConc = AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->OptStNc);	
	pPltN->fRootMaxConc = AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->OptRtNc);	

	pPltN->fLeafMinConc = AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->MinLvNc);	
	pPltN->fStemMinConc = AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->MinStNc);	
	pPltN->fRootMinConc = AFGENERATOR(pDev->fStageSUCROS,pPl->pGenotype->MinRtNc);	


	//SG/05/05/99/zusätzlich optimale Werte:
    pPltN->fLeafOptConc = (float)0.7*pPltN->fLeafMaxConc; //instead of 0.5 from van Keulen
    pPltN->fStemOptConc = (float)0.7*pPltN->fStemMaxConc; //instead of 0.5 from van Keulen
    pPltN->fRootOptConc = (float)0.7*pPltN->fRootMaxConc; //instead of 0.5 from van Keulen


	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantNitrogenDemand_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantNitrogenDemand_SPASS(EXP_POINTER)
	{
	PBIOMASS		pBiom = pPl->pBiomass;
    PPLTNITROGEN	pPltN = pPl->pPltNitrogen;

    
	//SG/13/04/99/////////////////////////////////////////////////////////////////////////////////////////
	//
	//	Achtung!
	//
	//	In SPASS erfolgt die Aktualisierung der Biomassen bereits vor der Berechnung
	//	des Stickstoffbedarfs. Dies hat zur Folge, dass hier der Bedarf der neu gebildeten
	//	Biomasse zweimal mitberücksichtigt ist.
	//	
	//pPl->pPltNitrogen->fLeafDemand =  pBiom->fLeafWeight*(pPl->pPltNitrogen->fLeafMaxConc-pPl->pPltNitrogen->fLeafActConc)
    //					+pBiom->fLeafGrowR*pPl->pPltNitrogen->fLeafMaxConc;
    //pPl->pPltNitrogen->fStemDemand =  pBiom->fStemWeight*(pPl->pPltNitrogen->fStemMaxConc-pPl->pPltNitrogen->fStemActConc)
    //					+pBiom->fStemGrowR*pPl->pPltNitrogen->fStemMaxConc;
    //pPl->pPltNitrogen->fRootDemand =  pBiom->fRootWeight*(pPl->pPltNitrogen->fRootMaxConc-pPltN->fRootActConc)
    //					+pBiom->fRootGrowR*pPl->pPltNitrogen->fRootMaxConc;
	/////////////////////////////////////////////////////////////////////////////////////////////////////

	pPl->pPltNitrogen->fLeafDemand =  (pBiom->fLeafWeight-pBiom->fLeafGrowR)*(pPl->pPltNitrogen->fLeafMaxConc-pPl->pPltNitrogen->fLeafActConc)
    					+pBiom->fLeafGrowR*pPl->pPltNitrogen->fLeafMaxConc;
    pPl->pPltNitrogen->fStemDemand =  (pBiom->fStemWeight-pBiom->fStemGrowR)*(pPl->pPltNitrogen->fStemMaxConc-pPl->pPltNitrogen->fStemActConc)
    					+pBiom->fStemGrowR*pPl->pPltNitrogen->fStemMaxConc;
    pPl->pPltNitrogen->fRootDemand =  (pBiom->fRootWeight-pBiom->fRootGrowR)*(pPl->pPltNitrogen->fRootMaxConc-pPltN->fRootActConc)
    					+pBiom->fRootGrowR*pPl->pPltNitrogen->fRootMaxConc;
	
	pPltN->fRootDemand = pPl->pPltNitrogen->fRootDemand;
	pPltN->fShootDemand= pPl->pPltNitrogen->fLeafDemand+pPl->pPltNitrogen->fStemDemand;

    pPltN->fTotalDemand= pPl->pPltNitrogen->fLeafDemand+pPl->pPltNitrogen->fStemDemand+pPl->pPltNitrogen->fRootDemand;  

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantNitrogenTranslocation_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantNitrogenTranslocation_SPASS(EXP_POINTER)
	{   
	PBIOMASS			pBiom=pPl->pBiomass;
	PDEVELOP			pDev =pPl->pDevelop;
	PCANOPY				pCan =pPl->pCanopy;
	PPLTNITROGEN	 	pPltN=pPl->pPltNitrogen;  

	PWEATHER			pWth =pCl->pWeather;
	PMSOWINFO			pSI  =pMa->pSowInfo;
	
	float				fTransN,fTransNconst, fTempMin,fTempOpt,fTempMax,fTemp,fTempFunc,fGrainNcumRate,fGrainDemand;
	RESPONSE			*pResp;  
	
//	float fNDemandMax;
	
	//Time constant for N translocation
	fTransNconst = (float)20.0;
	
	//SG/17/05/99:Etwas anders bei Kartoffel (aus PLAGEN)
	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
		fTransNconst = (float)30.0;


	if ((pDev->fStageSUCROS>=(float)1.0)
		||((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))&&(pDev->fStageSUCROS>=(float)0.5)))
		{
		//===========================================================================================
		//Amount of translocatable nitrogen
		//===========================================================================================
	    pPl->pPltNitrogen->fLeafTransNw=pBiom->fLeafWeight*(pPl->pPltNitrogen->fLeafActConc-pPl->pPltNitrogen->fLeafMinConc)+pPl->pPltNitrogen->fLeafNinc;
	    pPl->pPltNitrogen->fStemTransNw=pBiom->fStemWeight*(pPl->pPltNitrogen->fStemActConc-pPl->pPltNitrogen->fStemMinConc)+pPl->pPltNitrogen->fStemNinc;
	    pPl->pPltNitrogen->fRootTransNw=pBiom->fRootWeight*(pPltN->fRootActConc-pPl->pPltNitrogen->fRootMinConc)+pPl->pPltNitrogen->fRootNinc;
	
	    pPltN->fRootNLabile=pPl->pPltNitrogen->fRootTransNw;
	    pPltN->fStovNLabile=pPl->pPltNitrogen->fLeafTransNw+pPl->pPltNitrogen->fStemTransNw;

	    pPl->pPltNitrogen->fTotalTransNw=pPltN->fTotalNLabile=pPl->pPltNitrogen->fLeafTransNw+pPl->pPltNitrogen->fStemTransNw+pPl->pPltNitrogen->fRootTransNw;

		//===========================================================================================
		//Storage Nitrogen demand
		//===========================================================================================
		//The cordinal temperature of development rate
	    fTempMin= pPl->pGenotype->fTempMinSoNFill;
	    fTempOpt= pPl->pGenotype->fTempOptSoNFill;
	    fTempMax= pPl->pGenotype->fTempMaxSoNFill;
		pResp   = pPl->pGenotype->SoNFillRateTemp;
		
		//Calculating the daily thermal effect
		fTemp = (float)0.5*(pWth->fTempMax+pWth->fTempMin);
	
		if (pPl->pGenotype->SoNFillRateTemp[0].fInput==(float)0.0)
			fTempFunc = (float)RelativeTemperatureResponse_SPASS(fTemp,fTempMin,fTempOpt,fTempMax);
		else
			fTempFunc = (float)AFGENERATOR(fTemp, pPl->pGenotype->SoNFillRateTemp);

		fGrainNcumRate=(float)pPl->pGenotype->fRelStorageNFillRate*fTempFunc;

        if (pBiom->fGrainGrowR>(float)0.0)
			{
			pPl->pPltNitrogen->fGrainDemand = fGrainNcumRate*pCan->fGrainNum*pSI->fPlantDens*(float)1E-2;
			      fGrainDemand = (pBiom->fGrainWeight-pBiom->fGrainGrowR)*max((float)0.0, (float)0.02-pPltN->fGrainConc)
								 +pBiom->fGrainGrowR*(float)0.02;

			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))			
			//	
			{
				fGrainDemand = (pBiom->fGrainWeight-pBiom->fGrainGrowR)*max((float)0.0, (float)0.015-pPltN->fGrainConc)
								 +pBiom->fGrainGrowR*(float)0.015;
			}

			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))			
			//	
			{
				float fTubMaxConc[]={(float)0.02, (float)0.015, (float)0.01};
				float dev[]={(float)0.5, (float)1.0, (float)2.0};
				float xTm=(float)0;


				if(!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Christa"))
				{
					fTubMaxConc[0]=(float)0.02;
					fTubMaxConc[1]=(float)0.015;
					fTubMaxConc[2]=(float)0.01;
				}
				if(!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Agria"))
				{
					fTubMaxConc[0]=(float)0.03;
					fTubMaxConc[1]=(float)0.025;//0.0225;
					fTubMaxConc[2]=(float)0.025;//0.015;
				}

				if ((pDev->fStageSUCROS>=dev[0])&&(pDev->fStageSUCROS<=dev[1]))
					xTm=fTubMaxConc[0]+(pDev->fStageSUCROS-dev[0])/(dev[1]-dev[0])*(fTubMaxConc[1]-fTubMaxConc[0]);

				if ((pDev->fStageSUCROS>=dev[1])&&(pDev->fStageSUCROS<=dev[2]))
					xTm=fTubMaxConc[1]+(pDev->fStageSUCROS-dev[1])/(dev[2]-dev[1])*(fTubMaxConc[2]-fTubMaxConc[1]);

				if (pDev->fStageSUCROS>=dev[2])
					xTm=fTubMaxConc[2];

				fGrainDemand = (pBiom->fGrainWeight-pBiom->fGrainGrowR)*max((float)0.0, xTm-pPltN->fGrainConc)
								 +pBiom->fGrainGrowR*xTm;
			}

			
	//		fNDemandMax = pBiom->fGrainWeight * ((float)0.02 - pPltN->fGrainConc) + pBiom->fGrainGrowR*(float)0.02;

	//		pPl->pPltNitrogen->fGrainDemand = min(pPl->pPltNitrogen->fTotalTransNw/fTransNconst, min(fNDemandMax,max(pPl->pPltNitrogen->fGrainDemand,fGrainDemand)));
			pPl->pPltNitrogen->fGrainDemand = min(pPl->pPltNitrogen->fTotalTransNw/fTransNconst, max(pPl->pPltNitrogen->fGrainDemand,fGrainDemand));
			 
			}
		else
			pPl->pPltNitrogen->fGrainDemand = (float)0.0;	

		pPltN->fGrainDemand = pPl->pPltNitrogen->fGrainDemand;
		 
		//===========================================================================================
		//Total translocation
		//===========================================================================================
		fTransN = min(pPl->pPltNitrogen->fTotalTransNw, pPl->pPltNitrogen->fGrainDemand);
		
        pPl->pPltNitrogen->fGrainNinc = fTransN;
    
        pPl->pPltNitrogen->fLeafNtransRate=fTransN*pPl->pPltNitrogen->fLeafTransNw/(pPl->pPltNitrogen->fTotalTransNw+(float)1.0E-9);
        pPl->pPltNitrogen->fStemNtransRate=fTransN*pPl->pPltNitrogen->fStemTransNw/(pPl->pPltNitrogen->fTotalTransNw+(float)1.0E-9);
        pPl->pPltNitrogen->fRootNtransRate=fTransN*pPl->pPltNitrogen->fRootTransNw/(pPl->pPltNitrogen->fTotalTransNw+(float)1.0E-9);
		}
	else
		{
        pPl->pPltNitrogen->fGrainNinc     =(float)0.0;
        pPl->pPltNitrogen->fLeafNtransRate=(float)0.0;
        pPl->pPltNitrogen->fStemNtransRate=(float)0.0;
        pPl->pPltNitrogen->fRootNtransRate=(float)0.0;
        }
		
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	NitrogenUptake_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds NitrogenUptake_SPASS(EXP_POINTER)
	{
    int		L,L1;
	//float 	fTotRtLenDens;  
	float	fANDEM,fFNO3,fFNH4,fSMDFR,fRFAC,fNUF,fTRNU,fTRNLOS;
    float 	fUNO3,fUNH4,SNO3min,SNH4min,RNO3U[MAXSCHICHT],RNH4U[MAXSCHICHT];
	float	ammonium = (float)0;

	PSLAYER 		pSL		=pSo->pSLayer;
	PSWATER			pSWL	=pSo->pSWater;
	PWLAYER	   		pSLW	=pWa->pWLayer; 
	PCLAYER     	pSLN	=pCh->pCLayer;
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;

	PROOT			pRT		=pPl->pRoot;  
    PPLTWATER		pPltW	=pPl->pPltWater;
	
	
	PBIOMASS			pBiom=pPl->pBiomass;
	PPLTNITROGEN		pPltN=pPl->pPltNitrogen;
    
	//======================================================================================
	//Layer Nitrogen Initiation and Transformation
	//======================================================================================
	//fTotRtLenDens=(float)0.0;
    pLR=pPl->pRoot->pLayerRoot;

	for (L=1;L<=pSo->iLayers-2;L++)
		{
		RNO3U[L]=(float)0.0;
		RNH4U[L]=(float)0.0;

		//fTotRtLenDens += pLR->fLengthDens;

		pLR->fActLayNO3NUpt=(float)0.0;
		pLR->fActLayNH4NUpt=(float)0.0;
		pLR=pLR->pNext;
		}
    pPltN->fActNUptR =(float)0.0;

	//======================================================================================
	//Potential Nitrogen Uptake
	//======================================================================================
	fTRNU=(float)0.0;

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;

	for (L=1;L<=pSo->iLayers-2;L++)
		{

		pSLN->fNO3Nmgkg=(float)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(float)0.01*pSL->fBulkDens*pSL->fThickness;

	    //Check the whether there are roots in this layer:
	    if (pLR->fLengthDens==(float)0.0)		break;

		//The last layer of root:
		L1=L;

				
		// SG20140604: for further plant species (e.g. catch crops)
		fFNO3=(float)(1.0-exp(-0.030*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
		fFNH4=(float)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));

		//Potential nitrogen availability fFACTOR for NO3 (fFNO3) and NH4 (fFNH4) (0-1):
		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG"))
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT")))
			{
			fFNO3=(float)(1.0-exp(-0.030*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(float)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));

			if (fFNO3<(float)0.04) fFNO3=(float)0.0;
			if (fFNH4<(float)0.04) fFNH4=(float)0.0;
			}

 		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI"))
			{
			fFNO3=(float)(1.0-exp(-0.035*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(float)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));
		  	if (fFNO3<(float)0.03) fFNO3=(float)0.0;
		  	if (fFNH4<(float)0.03) fFNH4=(float)0.0;
			}

		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
			{

		  	fFNO3=(float)(1.0-exp(-0.0275*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
		  	fFNH4=(float)(1.0-exp(-0.0250*((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg)-0.5)));
		  	if (fFNO3<(float)0.03) fFNO3 *= (fFNO3/(float)0.03)*(fFNO3/(float)0.03)*(fFNO3/(float)0.03);//(float)0.0;
		  	if (fFNH4<(float)0.03) fFNH4 *= (fFNH4/(float)0.03)*(fFNH4/(float)0.03)*(fFNH4/(float)0.03);//(float)0.0;
			}

		if (fFNO3>(float)1.0) fFNO3=(float)1.0;
		if (fFNH4>(float)1.0) fFNH4=(float)1.0;

		//Soil moisture deficit fFACTOR (fSMDFR) affecting nitrogen uptake at low soil water:
		if (pSLW->fContAct<pSWL->fContFK)
			fSMDFR=(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
		else
			fSMDFR=(pSWL->fContSat-pSLW->fContAct)/(pSWL->fContSat-pSWL->fContFK);

		fSMDFR = max(min((float)1.0,fSMDFR),(float)0.0);

		//SG/27/05/99: bei ausreichender Wasserversorgung nur linearer Stress!
		//if(pPl->pPltWater->fActTranspDay>(float)1.0)		
		if(fActTraDay>(float)1.0)		
			//fSMDFR=(float)1.0;                            
		{
			fRFAC=pLR->fLengthDens*fSMDFR*(float)0.1*pSL->fThickness*((float)100.0);
			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
			{
				//SG 20141111: für AgMip-MZ wieder zurück
				fRFAC=pLR->fLengthDens*fSMDFR*fSMDFR*(float)0.1*pSL->fThickness*((float)100.0);
			}
		}
		else
		{
			//fRFAC: Effect of water stress and root length density on potential nitrogen uptake
			fRFAC=pLR->fLengthDens*fSMDFR*fSMDFR*(float)0.1*pSL->fThickness*((float)100.0);

			//SG 20111115: für AgMip-WH nur linearer Stress bei Wasserknappheit
			//fRFAC=pLR->fLengthDens*fSMDFR*(float)0.1*pSL->fThickness*((float)100.0);
			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
			{
				//SG 20141111: für AgMip-MZ wieder zurück
				fRFAC=pLR->fLengthDens*fSMDFR*fSMDFR*(float)0.1*pSL->fThickness*((float)100.0);
			}
		}
     
		//SG/10/11/99
		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
		{
			fRFAC=pLR->fLengthDens*fSMDFR*(float)0.1*pSL->fThickness*((float)100.0);
		}

		//Potential nitrogen uptake from layer L: RNO3[L],RNH4U[L](kg N/ha)
		RNO3U[L]=fRFAC*fFNO3*((float)0.006);
		RNH4U[L]=fRFAC*fFNH4*((float)0.006);

		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			{
		 	RNO3U[L]=fRFAC*fFNO3*((float)0.009);
		 	RNH4U[L]=fRFAC*fFNH4*((float)0.009);
			}

		RNO3U[L]=fRFAC*fFNO3*pPl->pGenotype->fMaxNuptRate; 	//SG/13/04/99: Warum der Umstand, 
		RNH4U[L]=fRFAC*fFNH4*pPl->pGenotype->fMaxNuptRate;  //wenn dann doch die Werte aus Plant.gtp 
															//verwendet werden?

		//Total Potential Root Nitrogen Uptake: fTRNU(kg N/ha)
		fTRNU=fTRNU+max((float)0.0,RNO3U[L]+RNH4U[L]);
                                    
		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
		pLR =pLR ->pNext;
		}

	//======================================================================================
	//Nitrogen uptake factor relating supply and plant  demand: fNUF (dimensionless)
	//======================================================================================
	fANDEM=pPltN->fTotalDemand;
	
	if (fANDEM<=(float)0.0)
        fNUF=(float)0.0;
	else
		{
			//SG/17/05/99:Etwas anders bei Kartoffel (aus PLAGEN)
			if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
				fNUF = ((float)0.3*fANDEM>fTRNU)?((float)1.0):((float)0.3*fANDEM/fTRNU);
			else
			{
				if (fANDEM>fTRNU)	fANDEM=fTRNU;
      			fNUF=fANDEM/fTRNU;
			}
		
			if (fTRNU==(float)0.0) 	return 0;
      	}            

	//======================================================================================
	//Actual Nitrogen Uptake  and Nitrogen Loss form Died Roots
	//======================================================================================
	fTRNU=(float)0.0; 	//fTRNU is now the actual total root nitrogen uptake (kg N/ha)
	fTRNLOS=(float)0.0;	//Total loss of nitrogen from root system:fTRNLOS (g N/plant)

	pPltN->fActNO3NUpt =(float)0.0;
	pPltN->fActNH4NUpt =(float)0.0;
    pPltN->fActNUptR    =(float)0.0;

	pSL		=pSo->pSLayer->pNext;
	pSWL	=pSo->pSWater->pNext;
	pSLW	=pWa->pWLayer->pNext; 
	pSLN	=pCh->pCLayer->pNext;
	pLR		=pPl->pRoot->pLayerRoot;

	for (L=1;L<=L1;L++)
		{
		//Possible plant uptake from a layer:fUNO3,fUNH4(Kg N/ha)
		fUNO3=RNO3U[L]*fNUF;
		fUNH4=RNH4U[L]*fNUF;

		pSLN->fNO3Nmgkg=(float)0.01*pSL->fBulkDens*pSL->fThickness;
		pSLN->fNH4Nmgkg=(float)0.01*pSL->fBulkDens*pSL->fThickness;

		//Low limit of extractable nitrogen in layer L: SNO3min,SNH4min(Kg N/ha)
		SNO3min=(float)0.25*pSLN->fNO3Nmgkg;
		SNH4min=(float)0.50*pSLN->fNH4Nmgkg;

		//Actual plant uptake from a layer:fUNO3,fUNH4(Kg N/ha)
		if(fUNO3>pSLN->fNO3N-SNO3min)
			fUNO3=max((float)0.0, pSLN->fNO3N-SNO3min);

		if(fUNH4>(pSLN->fNH4N-SNH4min))
			fUNH4=max((float)0.0, pSLN->fNH4N-SNH4min);

		pPltN->fActNO3NUpt += fUNO3;
		pPltN->fActNH4NUpt += fUNH4;

		pLR->fActLayNO3NUpt= fUNO3;
		pLR->fActLayNH4NUpt= fUNH4;

		//Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
//		pSLN->fNO3N=pSLN->fNO3N-fUNO3;
//		pSLN->fNH4N=pSLN->fNH4N-fUNH4;

		//Total actual root nitrogen uptake(kg N/ha)
		fTRNU=fTRNU+max((float)0.0,fUNO3+fUNH4);

		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
		pLR =pLR ->pNext;
		}

	pPltN->fActNUptR = max((float)0.0,fTRNU);

	//SG20151127: für Ausgabe in *.rfp
	pPltN->fActNUpt = pPltN->fActNUptR;

//	pPltN->fCumActNUpt 	+=	pPltN->fActNUpt;

	//======================================================================================
	//	Change in Plant Nitrogen Content due to nitrogen (Nitrogen partition) 
	//======================================================================================
	if (pPltN->fTotalDemand==(float)0.0)
		{
		pPl->pPltNitrogen->fLeafNinc = (float)0.0;
		pPl->pPltNitrogen->fStemNinc = (float)0.0;
		pPl->pPltNitrogen->fRootNinc = (float)0.0;
		pPl->pPltNitrogen->fShootNinc= (float)0.0;
		}
	else
		{	
		pPl->pPltNitrogen->fLeafNinc = pPltN->fActNUptR*pPl->pPltNitrogen->fLeafDemand/pPltN->fTotalDemand; 
		pPl->pPltNitrogen->fStemNinc = pPltN->fActNUptR*pPl->pPltNitrogen->fStemDemand/pPltN->fTotalDemand;
		pPl->pPltNitrogen->fRootNinc = pPltN->fActNUptR*pPl->pPltNitrogen->fRootDemand/pPltN->fTotalDemand;
		pPl->pPltNitrogen->fShootNinc= pPl->pPltNitrogen->fLeafNinc+pPl->pPltNitrogen->fStemNinc ;
 		}


	PlantNitrogenTranslocation_SPASS(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantNitrogenStress_SPASS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantNitrogenStress_SPASS(EXP_POINTER)
	{
	PBIOMASS			pBiom=pPl->pBiomass;
	PPLTNITROGEN	 	pPltN=pPl->pPltNitrogen;  
	PDEVELOP			pDev =pPl->pDevelop;



	//SG/06/04/99//////////////////////////////////////////////////////////////////////////////////
	//
	// In PLAGEN wird der aktuelle N-Gehalt der einzelnen Organe etwas anders berechnet!
	//
	///////////////////////////////////////////////////////////////////////////////////////////////



	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//Nitrogen Amount of different organs
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	
	pPl->pPltNitrogen->fDeadLeafNw += pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc;
	pPl->pPltNitrogen->fDeadRootNw += pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc;

	//SG/09/04/99/////////////////////////////////////////////////////////////////////
	//
	//	Achtung: im ursprünglichen PLAGEN stehen diese Gleichungen vor der
	//	Aktualisierung der Biomasse. SPASS (auch der alte Code) macht bei der
	//	Konzentrationsberechnung also einen Fehler, da auf die aktualisierte 
	//	Biomasse nocheinmal die aktuelle Zuwachsrate addiert wird.
	//////////////////////////////////////////////////////////////////////////////////
	
		
	if (pBiom->fLeafWeight!=(float)0.0)
	{
	pPl->pPltNitrogen->fLeafCont  += pPl->pPltNitrogen->fLeafNinc-pPl->pPltNitrogen->fLeafNtransRate
					   -pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc;
//	pPltN->fLeafCont  = pBiom->fLeafWeight*pPltN->fLeafActConc+pPltN->fLeafNinc-pPltN->fLeafNtransRate
//					   -pBiom->fLeafDeathRate*pPltN->fLeafMinConc;

	//SG 20130923: untere Grenze für Blattstickstoffgehalt (kg N /ha)
	pPltN->fLeafCont = max(pPltN->fLeafCont,pBiom->fLeafWeight*pPltN->fLeafMinConc);

	// SG20140904: Keine negativen N-Gehalte!!!
	if(pPl->pPltNitrogen->fLeafCont < 1.e-6)
		pPl->pPltNitrogen->fLeafCont = (float)1.e-6;

	pPltN->fLeafActConc = pPltN->fLeafCont/(pBiom->fLeafWeight-pPl->pBiomass->fLeafDeathRate+(float)1.0E-6);
	//SG/14/04/99:	epsilon auf 1e-6 gesetzt, wegen PLAGEN!

	}

	if (pBiom->fStemWeight!=(float)0.0)
	{
	pPl->pPltNitrogen->fStemCont  += pPl->pPltNitrogen->fStemNinc-pPl->pPltNitrogen->fStemNtransRate;
//	pPltN->fStemCont  = pBiom->fStemWeight*pPltN->fStemActConc+pPltN->fStemNinc-pPltN->fStemNtransRate
//					   -pBiom->fStemDeathRate*pPltN->fStemMinConc; //in SPASS keine Death-Rate!
	
	pPltN->fStemActConc = pPltN->fStemCont/(pBiom->fStemWeight-pPl->pBiomass->fStemDeathRate+(float)1.0E-6);

	}

	if (pBiom->fRootWeight!=(float)0.0)
	{
	pPl->pPltNitrogen->fRootCont  += pPl->pPltNitrogen->fRootNinc-pPl->pPltNitrogen->fRootNtransRate
  					   -pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc;
//	pPltN->fRootCont  = pBiom->fRootWeight*pPltN->fRootActConc+pPltN->fRootNinc-pPltN->fRootNtransRate
//					   -pBiom->fRootDeathRate*pPltN->fRootMinConc;
	
	//SG 20130923: untere Grenze für Wurzelstickstoffgehalt (kg N /ha)
	pPltN->fRootCont = max(pPltN->fRootCont,pBiom->fRootWeight*pPltN->fRootMinConc);

	// SG20140904: Keine negativen N-Gehalte!!!
	if(pPl->pPltNitrogen->fRootCont < 1.e-9)
		pPl->pPltNitrogen->fRootCont = (float)1.e-9;

	pPltN->fRootActConc = pPltN->fRootCont/(pBiom->fRootWeight-pPl->pBiomass->fRootDeathRate+(float)1.0E-6);

	}

	if (((pDev->fStageSUCROS>=(float)1.0)&&(pBiom->fGrainWeight!=(float)0.0))
		||((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))&&(pBiom->fGrainWeight>(float)0.0)))
	{
	pPl->pPltNitrogen->fGrainCont += pPl->pPltNitrogen->fGrainNinc;
//	pPltN->fGrainCont  = pBiom->fGrainWeight*pPltN->fGrainConc+pPltN->fGrainNinc; 
	// in PLAGEN -DeathRate*MinConc! DeathRate und MinConc sind in SPASS aber nicht bekannt!

	pPltN->fGrainConc= pPltN->fGrainCont/(pBiom->fGrainWeight+(float)1.0E-6);

	//sg19102000: Zur Vermeidung unrealistisch hoher N-Konzentrationen in den Körnern 
	//während der ersten Tage der Kornfüllung Einführung einer oberen Grenze
	if(pPltN->fGrainConc>=(float)0.04)
		pPltN->fGrainConc = (float)0.04;
	
	}

	
   

	//		alter SPASS-Code
/*
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//Nitrogen Amount of different organs
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPl->pPltNitrogen->fLeafCont  += pPl->pPltNitrogen->fLeafNinc-pPl->pPltNitrogen->fLeafNtransRate
					   -pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc;
	pPl->pPltNitrogen->fStemCont  += pPl->pPltNitrogen->fStemNinc-pPl->pPltNitrogen->fStemNtransRate;
	pPl->pPltNitrogen->fRootCont  += pPl->pPltNitrogen->fRootNinc-pPl->pPltNitrogen->fRootNtransRate
  					   -pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc;
	pPl->pPltNitrogen->fGrainCont += pPl->pPltNitrogen->fGrainNinc;

	//===========================================================================================
	//Nitrogen content of different organs
	//===========================================================================================
	pPl->pPltNitrogen->fLeafActConc = pPl->pPltNitrogen->fLeafCont/(pBiom->fLeafWeight+pBiom->fLeafGrowR-pPl->pBiomass->fLeafDeathRate+(float)1.0E-9);
	pPl->pPltNitrogen->fStemActConc = pPl->pPltNitrogen->fStemCont/(pBiom->fStemWeight+pBiom->fStemGrowR+(float)1.0E-9);
	
	pPltN->fRootActConc = pPl->pPltNitrogen->fRootCont/(pBiom->fRootWeight+pBiom->fRootGrowR-pPl->pBiomass->fRootDeathRate+(float)1.0E-9);

	if ((pDev->fStageSUCROS>=(float)1.0)&&(pBiom->fGrainWeight!=(float)0.0))
		pPltN->fGrainConc= pPl->pPltNitrogen->fGrainCont/(pBiom->fGrainWeight+pBiom->fGrainGrowR+(float)1.0E-9);
    

*/

    pPl->pPltNitrogen->fLeafActConc=(float)LIMIT(pPl->pPltNitrogen->fLeafMinConc, pPl->pPltNitrogen->fLeafMaxConc, pPl->pPltNitrogen->fLeafActConc);
    pPl->pPltNitrogen->fStemActConc=(float)LIMIT(pPl->pPltNitrogen->fStemMinConc, pPl->pPltNitrogen->fStemMaxConc, pPl->pPltNitrogen->fStemActConc);
    
    pPltN->fRootActConc=(float)LIMIT(pPl->pPltNitrogen->fRootMinConc, pPl->pPltNitrogen->fRootMaxConc, pPltN->fRootActConc);

	//pPltN->fTopsActConc=pPltN->fVegActConc= (pPl->pPltNitrogen->fStemCont+pPl->pPltNitrogen->fLeafCont)
	//									  /(pBiom->fStemWeight+pBiom->fStemGrowR-pPl->pBiomass->fLeafDeathRate
	//									   +pBiom->fLeafWeight+pBiom->fLeafGrowR+(float)1.0E-9);
    
	pPltN->fTopsActConc=pPltN->fVegActConc= (pPltN->fLeafActConc*pBiom->fLeafWeight + pPltN->fStemActConc*pBiom->fStemWeight)
												/(pBiom->fLeafWeight+pBiom->fStemWeight);
    
	pPltN->fStovCont = pPltN->fLeafCont+ pPltN->fStemCont;

    if (pPltN->fGrainConc>=(float)1.0)
    	pPltN->fGrainConc=(float)0.07;      

    //SG/06/04/99/////////////////////////////////////////////////////////////////////////////
	//
	// In PLAGEN werden die relativen N-Gehalte der einzelnen Organe etwas anders berechnet.
	// Insbesondere fällt die Unterscheidung zwischen iStageCERES >2 und =<2 weg.
	// Ausserdem wird nicht die maximale sondern die optimale N-Konzentration berücksichtigt.
	//////////////////////////////////////////////////////////////////////////////////////////

	
	if((pPltN!=NULL)&&(pDev->fStageSUCROS>=(float)0.0))
	{
	
	pPltN->fLeafRelNc=(pPltN->fLeafActConc-pPltN->fLeafMinConc)/(pPltN->fLeafMaxConc-pPltN->fLeafMinConc+(float)1.0E-9);
    pPltN->fStemRelNc=(pPltN->fStemActConc-pPltN->fStemMinConc)/(pPltN->fStemMaxConc-pPltN->fStemMinConc+(float)1.0E-9);
    pPltN->fRootRelNc=(pPltN->fRootActConc-pPltN->fRootMinConc)/(pPltN->fRootMaxConc-pPltN->fRootMinConc+(float)1.0E-9);
	/*
	pPltN->fLeafRelNc=(pPltN->fLeafActConc-pPltN->fLeafMinConc)/(pPltN->fLeafOptConc-pPltN->fLeafMinConc+(float)1.0E-9);
    pPltN->fStemRelNc=(pPltN->fStemActConc-pPltN->fStemMinConc)/(pPltN->fStemOptConc-pPltN->fStemMinConc+(float)1.0E-9);
    pPltN->fRootRelNc=(pPltN->fRootActConc-pPltN->fRootMinConc)/(pPltN->fRootOptConc-pPltN->fRootMinConc+(float)1.0E-9);
	*/
	}

	else
	{
		pPltN->fLeafRelNc=(float)1.0;
	    pPltN->fStemRelNc=(float)1.0;
	    pPltN->fRootRelNc=(float)1.0;
	}

	
	
//		alter SPASS-Code

/*	pPl->pPltNitrogen->fLeafRelNc=(pPl->pPltNitrogen->fLeafActConc-pPl->pPltNitrogen->fLeafMinConc)/(pPl->pPltNitrogen->fLeafMaxConc-pPl->pPltNitrogen->fLeafMinConc+(float)1.0E-9);
    pPl->pPltNitrogen->fStemRelNc=(pPl->pPltNitrogen->fStemActConc-pPl->pPltNitrogen->fStemMinConc)/(pPl->pPltNitrogen->fStemMaxConc-pPl->pPltNitrogen->fStemMinConc+(float)1.0E-9);
    pPl->pPltNitrogen->fRootRelNc=(pPltN->fRootActConc-pPl->pPltNitrogen->fRootMinConc)/(pPl->pPltNitrogen->fRootMaxConc-pPl->pPltNitrogen->fRootMinConc+(float)1.0E-9);

 
	if (pPl->pDevelop->iStageCERES <= 2)
		{	  
	    pPl->pPltNitrogen->fLeafRelNc=(float)1.0;
	    pPl->pPltNitrogen->fStemRelNc=(float)1.0;
	    pPl->pPltNitrogen->fRootRelNc=(float)1.0;
        }

*/  
  
        
	return 1;
	}




//////////////////////////////////////////////////////////////////////////////////////////////
// READ & INITALISATION FUNCTIONS (called from manage.c)		
//////////////////////////////////////////////////////////////////////////////////////////////

#define SET_IF_99(x,y) 		if (x == (float)-99)   x = (float)y;


//extern int WINAPI _loadds DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex);
//extern int WINAPI _loadds LongToDate(long dValue,LPSTR lpDate,BOOL bFullYearNumber);
                                                              

//BOOL WINAPI ReadDispDlgProc(HWND hWndDlg, WORD Message, WORD wParam, LONG lParam);

//int PlantVariableInitiation(EXP_POINTER);
//extern int SPASS_Genotype_Read(HANDLE hInstance);

// util_fct.c 
extern int   WINAPI Message(long, LPSTR);

extern float fHeadWeight;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  Initialization of plant variables
//------------------------------------------------------------------------------------------------------------
int PlantVariableInitiation(EXP_POINTER)
	{
	float 			CUMDEP,fThickness;
	int   			L,L1;
	char			pCropCode[40];

 	PGENOTYPE 		pGT		=pPl->pGenotype;
	PDEVELOP		pDev	=pPl->pDevelop;
	PBIOMASS		pGrw	=pPl->pBiomass;
	PMODELPARAM 	pGrwP	=pPl->pModelParam;
	PCANOPY			pCan	=pPl->pCanopy;
	PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;
	PMSOWINFO		pSI		=pMa->pSowInfo;
	PSLAYER 		pSL		=pSo->pSLayer;

	float fSowDepth;

   
	lstrcpy(pCropCode,(LPSTR)pPl->pGenotype->acCropCode);

	if(pSI->fSowDepth>(float)0.0)
		fSowDepth = pSI->fSowDepth;
	else
		fSowDepth = (float)3.0;

	

	//======================================================================================
	//Development Stage
	//======================================================================================
	if (pPl->pDevelop->iStageCERES == (int)-99) pPl->pDevelop->iStageCERES	= (int)0;
	SET_IF_99(pPl->pDevelop->fDevStage,(float)0.0);
	SET_IF_99(pPl->pDevelop->fStageXCERES,(float)0.0);
	SET_IF_99(pPl->pDevelop->fStageWang,(float)0.0);
	SET_IF_99(pPl->pDevelop->fStageSUCROS,(float)0.0);

	
    pDev->iStageCERES= (int)0;
    //pDev->fDevStage=(float)0;
	//pDev->fStageXCERES=(float)0;
	//pDev->fStageWang=(float)0;
	pDev->fStageSUCROS=(float)-1.0;

	
	
   	if (pSI->fPlantDens == (float)0.0) pSI->fPlantDens = (float)390;
    if (pSI->fRowWidth  == (float)0.0) pSI->fRowWidth  = (float)10;
    if (pSI->fSowDepth  == (float)0.0) pSI->fSowDepth  = (float)3;

	pCan->fLAI=pSI->fPlantDens*((float)5.0E-4);

	pPl->pCanopy->fCropCoverFrac    = (float) 0.0;
	pPl->pModelParam->fMaxRootDepth = (float) 0.0;


	pCan->fPlantLA					=(float)0.16	*pSI->fPlantDens; 
	pGrw->fLeafWeight				=(float)0.00136	*(float)10.0*pSI->fPlantDens; 
	pGrw->fSeedReserv				=(float)0.048	*(float)10.0*pSI->fPlantDens; 					
	pGrw->fStemWeight				=(float)0.0		*(float)10.0*pSI->fPlantDens; 					
	pGrw->fStovWeight				= pGrw->fLeafWeight;
	pGrw->fRootWeight				=(float)2.0		*pGrw->fSeedReserv;	
	pGrw->fTotalBiomass				=(float)0.0;		

    //für CERES-Modelle
    iStageOld=iTLNO=iSlowFillDay=(int)0;

    fPHINT= fXDevStage= fP5= fP9= fO1= fNFAC=(float)0.0;
    //Maize:
    fEarWeight= fEarGrowR= fEarFraction=(float)0.0;
    fTasselLeafNum= fMaxPlantLA=(float)0.0;

    //Sunflower:
    fStemWeightMin= fStemWeightMax=(float)0.0;
    fHeadWeight= fHeadGrowR= fPotHeadWeight= fHeadWeightMin= fHeadWeightMax=(float)0.0;
    fPericarpWeight= fPericarpGrowR= fPotPericarpGrowR=(float)0.0;
    fEmbryoWeight= fEmbryoGrowR=(float)0.0;
    fIGrainWeight= fOil= fOilPercent=(float)0.0;
    fNHeadActConc= fNHeadMinConc= fNHeadOptConc= fNHeadDemand= fNHeadCont=(float)0.0;
    fNPericarpActConc= fNPericarpOptConc= fNPericarpDemand= fNPericarpCont=(float)0.0;
    fNEmbryoActConc= fNEmbryoOptConc= fNEmbryoDemand= fNEmbryoCont=(float)0.0;
    fNGreenLeafActConc= fNGreenLeafCont= fNSenesLeafCont=(float)0.0;
    fSumPS= fRI1= fRM= fRFR= fGrowFactor= ppp=(float)0.0;
    fCumSenesLA= fSpecLeafArea= fAPLA=(float)0.0;
    fSenesLAN2= fSLOPEPE= fXPEPE= fYRAT=(float)0.0;

	//======================================================================================
	//Plant nitrogen content   
	//======================================================================================
	pPltN->fRootActConc	=(float)0.02;
	pPltN->fGrainConc	=(float)0.0;
	pPltN->fTopsActConc	=(float)0.050;
	pPltN->fGrainCont	=(float)0.0;

	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPltN->fLeafActConc	=(float)0.05;
	pPltN->fStemActConc	=(float)0.05;

	pPltN->fLeafRelNc	=(float)1.0;
	pPltN->fStemRelNc	=(float)1.0;
	pPltN->fRootRelNc	=(float)1.0;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

    pPltN->fDeadLeafNw = (float)0.0; 
    pPltN->fDeadRootNw = (float)0.0; 

	//======================================================================================
	//Dry matter initialization
	//======================================================================================
	if ((!lstrcmp(pCropCode,(LPSTR)"WH"))
		||((!lstrcmp(pCropCode,(LPSTR)"BA"))||(!lstrcmp(pCropCode,(LPSTR)"S")))
		||((!lstrcmp(pCropCode,(LPSTR)"RY"))))
		{
	   	if (pSI->fPlantDens<(float)80)
	   		pSI->fPlantDens=(float)390;
	
		pDev->fDaylengthEff	=(float)0.01; 	


		//SG 10.11.2004 //////////////////////////////////////////////////////
		//
		//	Tausendkorngewicht (TKG)
		//	WW: 45 g
		//	BA:	45 g
		//	S:	50 g
		//	RY:	35 g
		//	TC:	45 g
		//
		// ca. 50% available carbohydrates

		if (!lstrcmp(pCropCode,(LPSTR)"WH"))
			pGrw->fSeedReserv = (float)(0.045/2.0*10.0)*pSI->fPlantDens; // kg/ha				
		if (!lstrcmp(pCropCode,(LPSTR)"BA"))
			pGrw->fSeedReserv = (float)(0.045/2.0*10.0)*pSI->fPlantDens; // kg/ha 				
		if (!lstrcmp(pCropCode,(LPSTR)"S"))
			pGrw->fSeedReserv = (float)(0.05/2.0*10.0)*pSI->fPlantDens; // kg/ha 				
		if (!lstrcmp(pCropCode,(LPSTR)"RY"))
			pGrw->fSeedReserv = (float)(0.035/2.0*10.0)*pSI->fPlantDens; // kg/ha 				

		//Biomass
		pGrw->fLeafWeight				= (float)0.5*pGrw->fSeedReserv; 
		pCan->fPlantLA					= (float)1E4*pGrw->fLeafWeight/pGT->fSpecLfWeight; 
		pGrw->fStemWeight				= (float)0.0*pGrw->fSeedReserv; 				
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				= (float)0.5*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				= (float)0.0;		
		pDev->fCumPhyll					= (float)0.0;				
		pGrwP->fSingleTillerStemWeight	= (float)0.01;		

		pCan->fLAI=pCan->fPlantLA*(float)1E-4;
		
/*
		// alte Initialisierung:

		//Biomass
		pCan->fPlantLA					=(float)0.04	*pSI->fPlantDens; 
		pGrw->fLeafWeight				=(float)0.00034	*(float)10.0*pSI->fPlantDens; 
		pGrw->fSeedReserv				=(float)0.012	*(float)10.0*pSI->fPlantDens; 					
		pGrw->fStemWeight				=(float)0.0		*(float)10.0*pSI->fPlantDens; 					
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				=(float)0.5		*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				=(float)0.0;		
		pDev->fCumPhyll					=(float)0.0;				
		pGrwP->fSingleTillerStemWeight	=(float)0.01;		

		if (!lstrcmp(pCropCode,(LPSTR)"WH"))
			pCan->fLAI=pSI->fPlantDens*((float)4.0E-6);
		else
			pCan->fLAI=pSI->fPlantDens*((float)4.0E-4);

		pCan->fLAI=pSI->fPlantDens*((float)5.0E-4); //(float)0.010;//0.005;            		

*/
		pCan->fTillerNum=(float)1.0;                   
		pCan->fTillerNumSq=pSI->fPlantDens;				
	   		
		PLSC[1]=(float)0.0;
	
		if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")))
		{
			pDev->fCumPhyll		=(float)0.0;
		}
	
		if ((!lstrcmp(pCropCode,(LPSTR)"WH"))||((!lstrcmp(pCropCode,(LPSTR)"RY"))))
			{
				pPltN->fLeafActConc		=(float)0.044;
				pPltN->fStemActConc		=(float)0.044;

				pPltN->fRootActConc=(float)0.02;//0.022; CERES
				pPltN->fTopsActConc=(float)0.044;
			}

		if ((!lstrcmp(pCropCode,(LPSTR)"BA"))||((!lstrcmp(pCropCode,(LPSTR)"S"))))
			{
				pPltN->fLeafActConc		=(float)0.045;
				pPltN->fStemActConc		=(float)0.045;

				pPltN->fRootActConc=(float)0.02; //0.045; CERES
				pPltN->fTopsActConc=(float)0.045;
			}
		}

  	//==========================================================================
	//POTATO
	//==========================================================================
	if (!lstrcmp(pCropCode,(LPSTR)"PT"))
		{
		//SIMPOTATO
		pCan->fLAI			=pSI->fPlantDens*((float)4.0E-6);

		pGrw->fSeedReserv	=(float)20.0*(float)10.0*pSI->fPlantDens; //20g/plt					
		pGrw->fLeafWeight	=(float)0.1125*(float)10.0*pMa->pSowInfo->fPlantDens; //0.1125g/plt 
		pGrw->fStemWeight	=(float)0.1125*(float)10.0*pMa->pSowInfo->fPlantDens; 
		pGrw->fGrainWeight	=(float)0; 

		pPltN->fLeafActConc		=(float)0.06;
		pPltN->fStemActConc		=(float)0.05;

		pPltN->fRootActConc=(float)0.0215;
		pPltN->fTopsActConc=(float)0.08;
		}   

  	//==========================================================================
	//MAIZE
	//==========================================================================
	if (!lstrcmp(pCropCode,(LPSTR)"MZ"))
		{

		//SG 10.11.2004 //////////////////////////////////////////////////////
		//
		//	Tausendkorngewicht (TKG)
		//	MZ: 300 g
		//
		// ca. 50% available carbohydrates

		/* CERES
		pCan->fPlantLA		=(float)1.0;
		pCan->fLAI			=(float)0.0001*pSI->fPlantDens*pCan->fPlantLA;
		*/

		pDev->fCumPhyll		=(float)0.514;

		//pGrw->fSeedReserv	=(float)0.2;	
		pGrw->fSeedReserv = (float)(0.3/2.0*10.0)*pSI->fPlantDens; // kg/ha				

		pGrw->fLeafWeight				= (float)0.5*pGrw->fSeedReserv; 
		pCan->fPlantLA					= (float)1E4*pGrw->fLeafWeight/pGT->fSpecLfWeight; 
		pCan->fLAI=pCan->fPlantLA*(float)1E-4;
		pGrw->fStemWeight				= (float)0.0*pGrw->fSeedReserv; 				
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				= (float)0.5*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				= pGrw->fSeedReserv;	

        //Biomass
		pGrw->fLeafWeight				= (float)0.5*pGrw->fSeedReserv; 
		pCan->fPlantLA					= (float)1E4*pGrw->fLeafWeight/pGT->fSpecLfWeight; 
		pGrw->fStemWeight				= (float)0.0*pGrw->fSeedReserv; 				
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				= (float)0.5*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				= (float)0.0;		
		pDev->fCumPhyll					= (float)0.0;				
		pGrwP->fSingleTillerStemWeight	= (float)0.01;		

		pCan->fLAI=pCan->fPlantLA*(float)1E-4;		/* alt
		pGrw->fLeafWeight	=(float)0.2;
		pGrw->fStemWeight	=(float)0.2;
		pGrw->fGrainWeight	=(float)0; 
		pGrw->fStovWeight	=(float)0.4;
		pGrw->fRootWeight	=(float)0.2;
		pGrw->fTotalBiomass	=pGrw->fStovWeight;	
		*/
		pPltN->fLeafActConc		=(float)0.044;
		pPltN->fStemActConc		=(float)0.044;
		
		pPltN->fRootActConc=(float)0.022;
		pPltN->fTopsActConc=(float)0.044;

		}   

  	//==========================================================================
	//Sunflower
	//==========================================================================
	if (!lstrcmp(pCropCode,(LPSTR)"SF"))
		{
		pGrw->fSeedReserv	=(float)0.2;
		fHeadWeight = (float)0.0;

		//OILCROP-SUN, pp.5
		pPltN->fRootActConc	=(float)0.0;
		pPltN->fLeafActConc	=(float)0.0;
		pPltN->fStemActConc	=(float)0.0;
		pPltN->fTopsActConc	=(float)0.0;

		pDev->fSumDTT = pDev->fCumDTT = pDev->fDTT = (float)0.0;

		//leaf number (= LN)
		pCan->fLeafTipNum = (float)0.0;	
		pCan->fLAI=(float)0.0;
		
		pPl->pCanopy->fExpandLeafNum = (float)0.0;

		}   

  	//==========================================================================
	//Sugarbeet
	//==========================================================================
	if (!lstrcmp(pCropCode,(LPSTR)"SB"))
		{
	//   	if (pSI->fPlantDens<(float)50)
	//   		pSI->fPlantDens=(float)90;
	   	if (pSI->fPlantDens<(float)5) //SG20160510
	   		pSI->fPlantDens=(float)8;
	
	
		//Biomass
		pCan->fPlantLA					=(float)0.16    *pSI->fPlantDens; 
		pGrw->fLeafWeight				=(float)0.00136	*(float)10.0*pSI->fPlantDens; 
		pGrw->fSeedReserv				=(float)0.05	*(float)10.0*pSI->fPlantDens; 					
		pGrw->fStemWeight				=(float)0.0	*(float)10.0*pSI->fPlantDens; 					
		pGrw->fGrainWeight	            =(float)0; 
		pGrw->fStovWeight				= pGrw->fLeafWeight;
		pGrw->fRootWeight				=(float)0.5		*pGrw->fSeedReserv;	
		pGrw->fTotalBiomass				=(float)0.0;		

		pCan->fLAI			=pSI->fPlantDens*((float)5.0E-4);


		pPltN->fLeafActConc		=(float)0.06;
		pPltN->fStemActConc		=(float)0.0;
		pPltN->fRootActConc=(float)0.0215;
		}

	//======================================================================================
	//Dry matter initialization "pasture"
	//======================================================================================
	if (!lstrcmp(pCropCode,(LPSTR)"IR"))
	{
   		if (pSI->fPlantDens==(float)0.0) pSI->fPlantDens=(float)1000;
		pCan->fLAI=pSI->fPlantDens*((float)5.0E-4);

		pPl->pCanopy->fCropCoverFrac    = (float) 0.0;
		pPl->pModelParam->fMaxRootDepth = (float) 0.0;


		pCan->fPlantLA			= (float)1E4*pCan->fLAI; 
		pGrw->fLeafWeight		= (float)1E-4 * pCan->fPlantLA * pGT->fSpecLfWeight; 
		pGrw->fSeedReserv		= (float)0.048	*(float)10.0*pSI->fPlantDens; 					
		pGrw->fStemWeight		= pGrw->fLeafWeight; 					
		pGrw->fStovWeight		= pGrw->fLeafWeight + pGrw->fStemWeight;
		pGrw->fRootWeight		= pGrw->fStovWeight;	
		pGrw->fTotalBiomass		= pGrw->fRootWeight + pGrw->fLeafWeight + pGrw->fStemWeight;		


		//======================================================================================
		//Plant nitrogen content   (CERES, NWHEAT)
		//======================================================================================
		pPltN->fRootActConc	=(float)0.01;
		pPltN->fGrainConc	=(float)0.0;
		pPltN->fTopsActConc	=(float)0.050;
		pPltN->fGrainCont	=(float)0.0;

		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
		pPltN->fLeafActConc	=(float)0.02;
		pPltN->fStemActConc	=(float)0.02;

		pPltN->fLeafRelNc	=(float)1.0;
		pPltN->fStemRelNc	=(float)1.0;
		pPltN->fRootRelNc	=(float)1.0;
		//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX    
		pDev->fStageSUCROS = (float)0.0;
		lstrcpy(pPl->pGenotype->acCropCode,"WH\0");
	} // end "pasture (IR)"

	//SG 2005/12/14: (wegen neuer N-Stress-Berechnung in Photosynthese)
	NitrogenConcentrationLimits_SPASS(pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa);

   //======================================================================================
	//Root depth and length density
	//======================================================================================
	
	pPl->pRoot->fDepth= fSowDepth;

	pLR	  =pPl->pRoot->pLayerRoot;
	pSL	  =pSo->pSLayer->pNext;
	CUMDEP=(float)0.0;

	for (L=1;L<=pSo->iLayers-2;L++)
		{
		fThickness =(float)0.1*pSL->fThickness; //cm

		CUMDEP=CUMDEP+fThickness;

	  //IF_CROP_IS_UNKNOWN
			pLR->fLengthDens=(float)0.20*pSI->fPlantDens/fThickness;
	
		if ((!lstrcmp(pCropCode,(LPSTR)"WH"))||(!lstrcmp(pCropCode,(LPSTR)"BA")))
			pLR->fLengthDens=(float)0.0024*pSI->fPlantDens/fThickness;

		if (!lstrcmp(pCropCode,(LPSTR)"PT"))
			pLR->fLengthDens=(float)0.2*pSI->fPlantDens/fThickness;

		if (CUMDEP>pPl->pRoot->fDepth)
			break;

		pSL=pSL->pNext;
		pLR=pLR->pNext;
		}

	pLR->fLengthDens = pLR->fLengthDens*
							 ((float)1.0-(CUMDEP-pPl->pRoot->fDepth)/((float)0.1*pSL->fThickness));
	
	L1=L+1;
	pLR=pLR->pNext;
	if (L1<pSo->iLayers)
		{
		for (L=L1;L<=pSo->iLayers;L++)
        	{
        	pLR->fLengthDens=(float)0.0;
        	pLR=pLR->pNext;
        	}
        }                       
      
	
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPltN->fRootCont = pPltN->fRootActConc*pGrw->fRootWeight;
    pPltN->fStovCont = pPltN->fTopsActConc*pGrw->fStovWeight;
	
    pPltN->fLeafCont = pPltN->fLeafActConc*pGrw->fLeafWeight;
    pPltN->fStemCont = pPltN->fStemActConc*pGrw->fStemWeight;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPl->pPltWater->fStressFacPartition	= (float)1;
	pPl->pPltWater->fStressFacPhoto		= (float)1;
	pPl->pPltWater->fStressFacLeaf		= (float)1;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

	//SG 2005 11 03: kein N-Mangel bei Feldaufgang (CERES)
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPl->pPltNitrogen->fNStressPhoto=(float)1.0;
	pPl->pPltNitrogen->fNStressLeaf=(float)1.0;
	pPl->pPltNitrogen->fNStressTiller=(float)1.0;
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
    
	pPl->pCanopy->fPlantHeight = (float)0.0;

	
	return 1;
    }

////////////////////////////////////////////////////////////////////////////////////////////////
//						SPAMC Crop Data Reading
//							
//									Enli Wang   
//									
//	    							22.03.1995
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//		SPASS Genotype Data Read 
///////////////////////////////////////////////////////////////////////////////////////////////
extern float fO1;
int SPASS_Genotype_Read(HANDLE hInstance)
{
	HANDLE  hFile;
  int			i,iValue;
	long 		longCode;
	char 		cChar[20];
	
	PPLANT pPl=GetPlantPoi();

	//===================================================================================
	//The genotype file name
	//===================================================================================
    // char	cGenFileName[]="param\\WHEAT.GTP\0";
    char	cGenFileName[20];

		if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			lstrcpy(cGenFileName,"param\\WHEAT.GTP\0");
		else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"S")))
			lstrcpy(cGenFileName,"param\\BARLEY.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
			lstrcpy(cGenFileName,"param\\POTATO.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
			lstrcpy(cGenFileName,"param\\MAIZE.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
			lstrcpy(cGenFileName,"param\\SUNFLOWER.GTP\0");
		else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RY"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SR")))
			lstrcpy(cGenFileName,"param\\RYE.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"TC"))
			lstrcpy(cGenFileName,"param\\TRITICALE.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RP"))
			lstrcpy(cGenFileName,"param\\RAPE.GTP\0");
        else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))
			lstrcpy(cGenFileName,"param\\SUGARBEET.GTP\0");

		else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WM"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"GC"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"OR"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"AL"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"IR"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BR")))
			lstrcpy(cGenFileName,"param\\ZWIFRU.GTP\0");

		else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BE"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BW"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SP"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"FW"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"AP"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PI"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"KW"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"OA"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"EW"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"AS"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"HO"))
				//||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SY"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"LI"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MA")))
            return 1;

		//else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BE"))
		    //lstrcpy(cGenFileName,"param\\BEECH.GTP\0");
		//else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SP"))
			//lstrcpy(cGenFileName,"param\\SPRUCE.GTP\0");

		else
			lstrcpy(cGenFileName,"param\\PLANT.GTP\0");

    
	//===================================================================================
	//Read Genotype data
	//===================================================================================
  hFile = CreateFile(
    /* lpFileName             */ cGenFileName,
    /* dwDesiredAccess        */ GENERIC_READ,
    /* dwShareMode            */ 0,
    /* lpSecurityAttributes   */ NULL,
    /* dwCreationDistribution */ OPEN_EXISTING,
    /* dwFlagsAndAttributes   */ FILE_ATTRIBUTE_NORMAL,
    /* hTemplateFile          */ NULL );	

	//Display elements
	if ( hFile == INVALID_HANDLE_VALUE )
  {
    if( GetLastError() == ERROR_FILE_NOT_FOUND )
    {
      SetLastError( NO_ERROR );
    }
		Message(1,"Genotype file not found ! File Open Error");
		return 0;
  }                         
       
	Message(1,"Reading GENOTYPE Data ");
    
	longCode=0;

	//=======================================================================
	//	Name and Type
	//=======================================================================
	while (longCode!=2000001) longCode=ReadLong(hFile);
    
    iValue = ReadInt(hFile);
    if (iValue>0)
   	{
		ReadSymbol(hFile,cChar);
		if(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,cChar))
			Message(1,"Plant error ");

		if(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"GL"))
		lstrcpy(pPl->pGenotype->acCropCode,  	cChar);
        /*
		if ((lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"S"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"PT"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SF"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"MZ"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SB"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BW"))&&
			(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SP")))
		lstrcpy(pPl->pGenotype->acCropCode,"WH\0");
        */


		if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"RY"))
			lstrcpy(pPl->pGenotype->acCropCode,"WH\0");

		ReadSymbol(hFile,cChar); // lstrcpy(pPl->pGenotype->acCropName,  	cChar);
		ReadSymbol(hFile,cChar);	lstrcpy(pPl->pGenotype->acEcoType,  	cChar);
		ReadSymbol(hFile,cChar);	lstrcpy(pPl->pGenotype->acDaylenType, 	cChar);
		ReadSymbol(hFile,cChar);	lstrcpy(pPl->pGenotype->acLvAngleType, cChar);  

	    pPl->pGenotype->fPlantHeight=ReadFloat(hFile);
		}

	//=======================================================================
	//	Photosynthesis Response
	//=======================================================================
	//Ps parameters
	while (longCode!=2000010)	longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

    if (iValue>0)
    	{
			//SG 20110907: read different values for different cultivars
			ReadSymbol(hFile,cChar);
		
			pPl->pGenotype->fMaxGrossPs 	= ReadFloat(hFile);
			pPl->pGenotype->fLightUseEff	= ReadFloat(hFile);
			pPl->pGenotype->fMinRm			= ReadFloat(hFile);
			pPl->pGenotype->fCO2CmpPoint	= ReadFloat(hFile);
			pPl->pGenotype->fCiCaRatio		= ReadFloat(hFile);
			pPl->pGenotype->fPsCurveParam	= ReadFloat(hFile);
			pPl->pGenotype->fDaylengthCoeff	= ReadFloat(hFile);
					
		
			for(i=1;i<iValue;i++)
			{
				ReadSymbol(hFile,cChar);

				if(!(lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,cChar)))
				{
					pPl->pGenotype->fMaxGrossPs 	= ReadFloat(hFile);
					pPl->pGenotype->fLightUseEff	= ReadFloat(hFile);
					pPl->pGenotype->fMinRm			= ReadFloat(hFile);
					pPl->pGenotype->fCO2CmpPoint	= ReadFloat(hFile);
					pPl->pGenotype->fCiCaRatio		= ReadFloat(hFile);
					pPl->pGenotype->fPsCurveParam	= ReadFloat(hFile);
					pPl->pGenotype->fDaylengthCoeff	= ReadFloat(hFile);
				}
				else
				{
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
				} //end if Variety
			}	//end for
		}
	//----------------------------------------------------------
	//	Photosynthesis Temperature Response
	while (longCode!=2000011) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

	pPl->pGenotype->fTempMinPs 	= ReadFloat(hFile);
	pPl->pGenotype->fTempOptPs 	= ReadFloat(hFile);
	pPl->pGenotype->fTempMaxPs 	= ReadFloat(hFile);

    if (iValue>0)
    	{   
    	pPl->pGenotype->PmaxTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->PmaxTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PmaxTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PmaxTemp[i].fOutput=	ReadFloat(hFile);
		}

	//----------------------------------------------------------
	//	Photosynthesis: development stage response
	while (longCode!=2000012) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

    PmaxAge[0].fInput =(float)iValue;
    PmaxAge[0].fOutput=(float)iValue;

    if (iValue>0)
    	{   
			for (i=1;i<=iValue;i++)
	    		PmaxAge[i].fInput = ReadFloat(hFile);
			for (i=1;i<=iValue;i++)
	    		PmaxAge[i].fOutput= ReadFloat(hFile);
		}

	//=======================================================================
	//	Maintenance and Growth
	//=======================================================================
	//Maintenance Respiration Coefficients
	while (longCode!=2000020) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{
		pPl->pGenotype->fMaintLeaf 	= ReadFloat(hFile);
		pPl->pGenotype->fMaintStem		= ReadFloat(hFile);
		pPl->pGenotype->fMaintRoot		= ReadFloat(hFile);
		pPl->pGenotype->fMaintStorage	= ReadFloat(hFile);
		}
	//----------------------------------------------------------
	//Growth Respiration CO2 production Factor
	while (longCode!=2000021) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{
		pPl->pGenotype->fCO2EffLeaf 		= ReadFloat(hFile);
		pPl->pGenotype->fCO2EffStem		= ReadFloat(hFile);
		pPl->pGenotype->fCO2EffRoot		= ReadFloat(hFile);
		pPl->pGenotype->fCO2EffStorage		= ReadFloat(hFile);
		}
	//----------------------------------------------------------
	//Growth Efficiency
	while (longCode!=2000022) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{
		pPl->pGenotype->fGrwEffLeaf 		= ReadFloat(hFile);
		pPl->pGenotype->fGrwEffStem		= ReadFloat(hFile);
		pPl->pGenotype->fGrwEffRoot		= ReadFloat(hFile);
		pPl->pGenotype->fGrwEffStorage		= ReadFloat(hFile);
		}

	//=======================================================================
	//	Phenological Development
	//=======================================================================
	//Development Coefficients
	while (longCode!=2000030) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);


    if (iValue>0)
    	{
			ReadSymbol(hFile,cChar);

			pPl->pGenotype->fDevelopConstant1 	= ReadFloat(hFile);
			pPl->pGenotype->fDevelopConstant2	= ReadFloat(hFile);
			pPl->pGenotype->fOptVernDays		= ReadFloat(hFile);
			pPl->pGenotype->fDaylenSensitivity	= ReadFloat(hFile);
			pPl->pGenotype->fOptDaylen			= ReadFloat(hFile);
			pPl->pGenotype->iVernCoeff			= (int)ReadFloat(hFile);
			//SG 20111019			
			pPl->pGenotype->fPhyllochronInterval = ReadFloatOrNewLine(hFile);
		
			for(i=1;i<iValue;i++)
			{
				ReadSymbol(hFile,cChar);

				if(!(lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,cChar)))
				{
					pPl->pGenotype->fDevelopConstant1		= ReadFloat(hFile);
					pPl->pGenotype->fDevelopConstant2		= ReadFloat(hFile);
					pPl->pGenotype->fOptVernDays			= ReadFloat(hFile);
					pPl->pGenotype->fDaylenSensitivity	= ReadFloat(hFile);
					pPl->pGenotype->fOptDaylen			= ReadFloat(hFile);
					pPl->pGenotype->iVernCoeff			= (int)ReadFloat(hFile);
					//SG 20111019			
					pPl->pGenotype->fPhyllochronInterval	= ReadFloatOrNewLine(hFile);
				}
				else
				{
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					//SG 20111019			
					ReadFloatOrNewLine(hFile);
				} //end if Variety
			}	//end for
		}	//end if iValue



	//----------------------------------------------------------
	//Development Rate vs Temperature (Vegetative)
	while (longCode!=2000031) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

	pPl->pGenotype->fTempMinDevVeg 	= ReadFloat(hFile);
	pPl->pGenotype->fTempOptDevVeg 	= ReadFloat(hFile);
	pPl->pGenotype->fTempMaxDevVeg 	= ReadFloat(hFile);

    if (iValue>0)
    	{   
    	pPl->pGenotype->DevRateVegTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->DevRateVegTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->DevRateVegTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->DevRateVegTemp[i].fOutput= ReadFloat(hFile);
		}
	//----------------------------------------------------------
	//Development Rate vs Temperature (Reproductive)
	while (longCode!=2000032) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

	pPl->pGenotype->fTempMinDevRep 	= ReadFloat(hFile);
	pPl->pGenotype->fTempOptDevRep 	= ReadFloat(hFile);
	pPl->pGenotype->fTempMaxDevRep 	= ReadFloat(hFile);

    if (iValue>0)
    	{   
    	pPl->pGenotype->DevRateRepTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->DevRateRepTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->DevRateRepTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->DevRateRepTemp[i].fOutput= ReadFloat(hFile);
		}
	//----------------------------------------------------------
	//Vernalization Rate vs Temperature 
	while (longCode!=2000033) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

	pPl->pGenotype->fTempMinVern 	= ReadFloat(hFile);
	pPl->pGenotype->fTempOptVern 	= ReadFloat(hFile);
	pPl->pGenotype->fTempMaxVern 	= ReadFloat(hFile);

    if (iValue>0)
    	{   
    	pPl->pGenotype->VernRateTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->VernRateTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->VernRateTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->VernRateTemp[i].fOutput= ReadFloat(hFile);
		}
	//----------------------------------------------------------
	//Development Rate vs Photoperiod (Vegetative)

	while (longCode!=2000034) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->DevRateVegPhtP[0].fInput =(float)iValue;
    	pPl->pGenotype->DevRateVegPhtP[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->DevRateVegPhtP[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->DevRateVegPhtP[i].fOutput= ReadFloat(hFile);
		}  
	//----------------------------------------------------------
	
	if((lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))&&(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB")))
	{

	//Development Rate vs EC-Stadium

	while (longCode!=2000035) longCode=ReadLong(hFile);

	iValue = ReadInt(hFile);
    if (iValue>0)
    { 
        for (i=0;i<=10;i++)
	    	DVR[i] = ReadFloat(hFile);
	}
	else
	    	DVR[0] = (float)-1.0;

	//----------------------------------------------------------
	//Stage dependend parameters for the CERES - model

	while (longCode!=2000036) longCode=ReadLong(hFile);

        for (i=0;i<=8;i++)
  	    	pPl->pGenotype->pStageParam->afTempMax[i] = ReadFloat(hFile);
        for (i=0;i<=8;i++)
	    	pPl->pGenotype->pStageParam->afTempOpt[i] = ReadFloat(hFile);
        for (i=0;i<=8;i++)
	    	pPl->pGenotype->pStageParam->afTempBase[i] = ReadFloat(hFile);
        for (i=0;i<=8;i++)
	    	pPl->pGenotype->pStageParam->afThermalTime[i] = ReadFloat(hFile);
        for (i=0;i<=8;i++)
	    	pPl->pGenotype->pStageParam->afMaxDevR[i] = ReadFloat(hFile);
	}	//end (!="PT")&&(!="SB")
    //----------------------------------------------------------
	//Development Coefficients for the SUCROS - model
	while (longCode!=2000037) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

    if (iValue>0)
    	{
			ReadSymbol(hFile,cChar);

			fBaseTemperature1 			= ReadFloat(hFile);
			fCriticalTemperatureSum1	= ReadFloat(hFile);
			fBaseTemperature2 			= ReadFloat(hFile);
			fCriticalTemperatureSum2	= ReadFloat(hFile);

			for(i=1;i<iValue;i++)
			{

				ReadSymbol(hFile,cChar);


				if(!(lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,cChar)))
				{
					fBaseTemperature1 			= ReadFloat(hFile);
					fCriticalTemperatureSum1	= ReadFloat(hFile);
					fBaseTemperature2 			= ReadFloat(hFile);
					fCriticalTemperatureSum2	= ReadFloat(hFile);
				}
				else
				{
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
				} //end if Variety
			}	//end for
		}	//end if iValue

	//=======================================================================
	//	Morphology
	//=======================================================================
	while (longCode!=2000040)	longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

	    if (iValue>0)
    	{
			ReadSymbol(hFile,cChar);
			pPl->pGenotype->fMaxLfInitRate 			= ReadFloat(hFile);
			pPl->pGenotype->fMaxLfAppRate			= ReadFloat(hFile);
			pPl->pGenotype->fTillerDemand			= ReadFloat(hFile);
			pPl->pGenotype->fStorageOrganNumCoeff	= ReadFloat(hFile);
			pPl->pGenotype->fSpecLfWeight			= ReadFloat(hFile);

			for(i=1;i<iValue;i++)
			{

				ReadSymbol(hFile,cChar);

				if(!(lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,cChar)))
				{
					pPl->pGenotype->fMaxLfInitRate 		= ReadFloat(hFile);
					pPl->pGenotype->fMaxLfAppRate			= ReadFloat(hFile);
					pPl->pGenotype->fTillerDemand			= ReadFloat(hFile);
					pPl->pGenotype->fStorageOrganNumCoeff	= ReadFloat(hFile);
					pPl->pGenotype->fSpecLfWeight			= ReadFloat(hFile);
				}
				else
				{
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
				} //end if Variety
			}	//end for
		}	//end if iValue




	//----------------------------------------------------------
	//Specific leaf weight
	while (longCode!=2000041) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->SpecLfWght[0].fInput =(float)iValue;
    	pPl->pGenotype->SpecLfWght[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->SpecLfWght[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->SpecLfWght[i].fOutput= ReadFloat(hFile);
		}  
	//----------------------------------------------------------
	//Leaf appearance rate
	while (longCode!=2000042) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

	pPl->pGenotype->fTempMinLfApp 	= ReadFloat(hFile);
	pPl->pGenotype->fTempOptLfApp 	= ReadFloat(hFile);
	pPl->pGenotype->fTempMaxLfApp 	= ReadFloat(hFile);

    if (iValue>0)
    	{   
    	pPl->pGenotype->LfAppRateTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->LfAppRateTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->LfAppRateTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->LfAppRateTemp[i].fOutput= ReadFloat(hFile);
		}
	//=======================================================================
	//	Carbohydrate Partitioning
	//=======================================================================
	//Root
	while (longCode!=2000050) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->PartRoot[0].fInput =(float)iValue;
    	pPl->pGenotype->PartRoot[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartRoot[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartRoot[i].fOutput= ReadFloat(hFile);
		}  
	//----------------------------------------------------------
	//Leaves
	while (longCode!=2000051)	longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->PartLeaf[0].fInput =(float)iValue;
    	pPl->pGenotype->PartLeaf[0].fOutput=(float)iValue;
    	                                                                      
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartLeaf[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartLeaf[i].fOutput= ReadFloat(hFile);
		}  
	//----------------------------------------------------------
	//Stems
	while (longCode!=2000052) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->PartStem[0].fInput =(float)iValue;
    	pPl->pGenotype->PartStem[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartStem[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartStem[i].fOutput= ReadFloat(hFile);
		}  
	//----------------------------------------------------------
	//Storage organs
	while (longCode!=2000053) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->PartStorage[0].fInput =(float)iValue;
    	pPl->pGenotype->PartStorage[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartStorage[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->PartStorage[i].fOutput= ReadFloat(hFile);
		}  
		
	//=======================================================================
	//	Storage Growth and Reserve Translocation
	//=======================================================================

	if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB")))
	{
	//Parameters
	while (longCode!=1000060) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
     if (iValue>0)
    	{
		pPl->pGenotype->fReserveRatio 			= ReadFloat(hFile); 
		pPl->pGenotype->fRelResvGrwRate 		= ReadFloat(hFile); 
		pPl->pGenotype->fRelStorageFillRate		= ReadFloat(hFile);
		pPl->pGenotype->fRelStorageNFillRate	= ReadFloat(hFile);
		}

//	//----------------------------------------------------------
//	//Lag phase for reserve accumulation
	while (longCode!=1000061) longCode=ReadLong(hFile);
	
    iValue = ReadInt(hFile);     
	if (iValue>0)
		{   
		pPl->pGenotype->ReservGrwLag[0].fInput =(float)iValue;
		pPl->pGenotype->ReservGrwLag[0].fOutput=(float)iValue;
		    	
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->ReservGrwLag[i].fInput = ReadFloat(hFile);
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->ReservGrwLag[i].fOutput= ReadFloat(hFile);
		}  
	
	//----------------------------------------------------------
	//Time constant for reserve translocation
	while (longCode!=1000062) longCode=ReadLong(hFile);
	
  iValue = ReadInt(hFile);     
	if (iValue>0)
		{   
		pPl->pGenotype->ReservTransTc[0].fInput =(float)iValue;
		pPl->pGenotype->ReservTransTc[0].fOutput=(float)iValue;
		    	
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->ReservTransTc[i].fInput = ReadFloat(hFile);
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->ReservTransTc[i].fOutput= ReadFloat(hFile);
		}  

	//----------------------------------------------------------
	//Lag phase for storage filling
	while (longCode!=1000063) longCode=ReadLong(hFile);
	
	iValue = ReadInt(hFile);     
	if (iValue>0)
		{   
		pPl->pGenotype->SoFillLag[0].fInput =(float)iValue;
		pPl->pGenotype->SoFillLag[0].fOutput=(float)iValue;
	    	
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->SoFillLag[i].fInput = ReadFloat(hFile);
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->SoFillLag[i].fOutput= ReadFloat(hFile);
		} 
	
		//----------------------------------------------------------
	//Storage Fill Rate
	while (longCode!=1000064) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);     
    
   	pPl->pGenotype->fTempMinSoFill 	= ReadFloat(hFile);
	pPl->pGenotype->fTempOptSoFill 	= ReadFloat(hFile);
	pPl->pGenotype->fTempMaxSoFill 	= ReadFloat(hFile);

	   if (iValue>0)
    	{   
    	pPl->pGenotype->SoFillRateTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->SoFillRateTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->SoFillRateTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->SoFillRateTemp[i].fOutput= ReadFloat(hFile);
		}  

	//----------------------------------------------------------
	//Storage N Fill Rate
	while (longCode!=1000065) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);     
    
   	pPl->pGenotype->fTempMinSoNFill 	= ReadFloat(hFile);
	pPl->pGenotype->fTempOptSoNFill 	= ReadFloat(hFile);
	pPl->pGenotype->fTempMaxSoNFill 	= ReadFloat(hFile);

	   if (iValue>0)
    	{   
    	pPl->pGenotype->SoNFillRateTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->SoNFillRateTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->SoNFillRateTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->SoNFillRateTemp[i].fOutput= ReadFloat(hFile);
		}  

	}	//end Potato
	else	//other crops
	{
	//Parameters
		while (longCode!=2000060) longCode=ReadLong(hFile);

		iValue = ReadInt(hFile);

		if (iValue>0)
    	{
		
			ReadSymbol(hFile,cChar);
			pPl->pGenotype->fReserveRatio 			= ReadFloat(hFile); 
			pPl->pGenotype->fRelResvGrwRate 		= ReadFloat(hFile); 
			pPl->pGenotype->fRelStorageFillRate		= ReadFloat(hFile);
			pPl->pGenotype->fRelStorageNFillRate	= ReadFloat(hFile);
			if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
				fO1 = ReadFloat(hFile);

			for(i=1;i<iValue;i++)
			{

				ReadSymbol(hFile,cChar);


				if(!(lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,cChar)))
				{
					pPl->pGenotype->fReserveRatio 			= ReadFloat(hFile); 
					pPl->pGenotype->fRelResvGrwRate 		= ReadFloat(hFile); 
					pPl->pGenotype->fRelStorageFillRate	= ReadFloat(hFile);
					pPl->pGenotype->fRelStorageNFillRate	= ReadFloat(hFile);
					if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
						fO1 = ReadFloat(hFile);

				}
				else
				{
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					ReadFloat(hFile);
					if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
						fO1 = ReadFloat(hFile);
				} //end if Variety
			}	//end for
		}	//end if iValue

	//----------------------------------------------------------
	//Storage Fill Rate
		while (longCode!=2000061) longCode=ReadLong(hFile);

		iValue = ReadInt(hFile);     
    
   		pPl->pGenotype->fTempMinSoFill 	= ReadFloat(hFile);
		pPl->pGenotype->fTempOptSoFill 	= ReadFloat(hFile);
		pPl->pGenotype->fTempMaxSoFill 	= ReadFloat(hFile);

		if (iValue>0)
    		{   
    		pPl->pGenotype->SoFillRateTemp[0].fInput =(float)iValue;
    		pPl->pGenotype->SoFillRateTemp[0].fOutput=(float)iValue;
    	
			for (i=1;i<=iValue;i++)
	    		pPl->pGenotype->SoFillRateTemp[i].fInput = ReadFloat(hFile);
			for (i=1;i<=iValue;i++)
	    		pPl->pGenotype->SoFillRateTemp[i].fOutput= ReadFloat(hFile);
			}  

	//----------------------------------------------------------
	//Storage N Fill Rate
		while (longCode!=2000062) longCode=ReadLong(hFile);

		iValue = ReadInt(hFile);     
    
   		pPl->pGenotype->fTempMinSoNFill 	= ReadFloat(hFile);
		pPl->pGenotype->fTempOptSoNFill 	= ReadFloat(hFile);
		pPl->pGenotype->fTempMaxSoNFill 	= ReadFloat(hFile);

		if (iValue>0)
    		{   
    		pPl->pGenotype->SoNFillRateTemp[0].fInput =(float)iValue;
    		pPl->pGenotype->SoNFillRateTemp[0].fOutput=(float)iValue;
    	
			for (i=1;i<=iValue;i++)
	    		pPl->pGenotype->SoNFillRateTemp[i].fInput = ReadFloat(hFile);
			for (i=1;i<=iValue;i++)
	    		pPl->pGenotype->SoNFillRateTemp[i].fOutput= ReadFloat(hFile);
			}  
	}	//end other crops
	//=======================================================================
	//	TISSUE NITROGEN CONTENT
	//=======================================================================
	//Leaf Opt Nc
	while (longCode!=2000070) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->OptLvNc[0].fInput =(float)iValue;
    	pPl->pGenotype->OptLvNc[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->OptLvNc[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->OptLvNc[i].fOutput= ReadFloat(hFile);
		}  
	//----------------------------------------------------------
	//Stem Opt Nc
	while (longCode!=2000071) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->OptStNc[0].fInput =(float)iValue;
    	pPl->pGenotype->OptStNc[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->OptStNc[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->OptStNc[i].fOutput= ReadFloat(hFile);
		}  
	//----------------------------------------------------------
	//Root Opt Nc
	while (longCode!=2000072) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->OptRtNc[0].fInput =(float)iValue;
    	pPl->pGenotype->OptRtNc[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->OptRtNc[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->OptRtNc[i].fOutput= ReadFloat(hFile);
		}  
  	//----------------------------------------------------------
	//Leaf Min Nc
	while (longCode!=2000073) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->MinLvNc[0].fInput =(float)iValue;
    	pPl->pGenotype->MinLvNc[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->MinLvNc[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->MinLvNc[i].fOutput= ReadFloat(hFile);
		}  
  	//----------------------------------------------------------
	//Stem Min Nc
	while (longCode!=2000074) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->MinStNc[0].fInput =(float)iValue;
    	pPl->pGenotype->MinStNc[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->MinStNc[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->MinStNc[i].fOutput= ReadFloat(hFile);
		}  
   	//----------------------------------------------------------
	//Root Min Nc
	while (longCode!=2000075) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->MinRtNc[0].fInput =(float)iValue;
    	pPl->pGenotype->MinRtNc[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->MinRtNc[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->MinRtNc[i].fOutput= ReadFloat(hFile);
		}  

	//=======================================================================
	//	Root Growth and Activity
	//=======================================================================
	//Parameters
	while (longCode!=2000080) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
     if (iValue>0)
    	{
		pPl->pGenotype->fMaxRootExtRate	    = ReadFloat(hFile);
		pPl->pGenotype->fRootLengthRatio	= ReadFloat(hFile);
		pPl->pGenotype->fMaxWuptRate 		= ReadFloat(hFile);
		pPl->pGenotype->fMaxNuptRate		= ReadFloat(hFile);
		pPl->pGenotype->fMaxRootDepth 		= ReadFloat(hFile);
		}
	//----------------------------------------------------------
	//Root Extension Rate vs Temperature
	while (longCode!=2000081) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);

   	pPl->pGenotype->fRootExtTempMin = ReadFloat(hFile);
	pPl->pGenotype->fRootExtTempOpt = ReadFloat(hFile);
	pPl->pGenotype->fRootExtTempMax = ReadFloat(hFile);

    if (iValue>0)
    	{   
    	pPl->pGenotype->RootExtRateTemp[0].fInput =(float)iValue;
    	pPl->pGenotype->RootExtRateTemp[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->RootExtRateTemp[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->RootExtRateTemp[i].fOutput= ReadFloat(hFile);
		}  

	//----------------------------------------------------------
	//Root Extension Rate vs Soil moisture
	while (longCode!=2000082) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
    if (iValue>0)
    	{   
    	pPl->pGenotype->RootExtRateWc[0].fInput =(float)iValue;
    	pPl->pGenotype->RootExtRateWc[0].fOutput=(float)iValue;
    	
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->RootExtRateWc[i].fInput = ReadFloat(hFile);
        for (i=1;i<=iValue;i++)
	    	pPl->pGenotype->RootExtRateWc[i].fOutput= ReadFloat(hFile);
		}  

	//=======================================================================
	//	Senesence
	//=======================================================================
	//Parameters
	while (longCode!=2000090) longCode=ReadLong(hFile);

    iValue = ReadInt(hFile);
     if (iValue>0)
    	{
		pPl->pGenotype->fBeginSenesDvs 		= ReadFloat(hFile); 
		pPl->pGenotype->fBeginShadeLAI 		= ReadFloat(hFile); 
		}

	//----------------------------------------------------------
	//Relative death rate temperature leaves
	while (longCode!=2000091) longCode=ReadLong(hFile);
	
    iValue = ReadInt(hFile);     
	if (iValue>0)
		{   
		pPl->pGenotype->LvDeathRateTemp[0].fInput =(float)iValue;
		pPl->pGenotype->LvDeathRateTemp[0].fOutput=(float)iValue;
		    	
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->LvDeathRateTemp[i].fInput = ReadFloat(hFile);
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->LvDeathRateTemp[i].fOutput= ReadFloat(hFile);
		}  

	//----------------------------------------------------------
	//Relative death rate temperature leaves
	while (longCode!=2000092) longCode=ReadLong(hFile);
	
    iValue = ReadInt(hFile);     
	if (iValue>0)
		{   
		pPl->pGenotype->RtDeathRateTemp[0].fInput =(float)iValue;
		pPl->pGenotype->RtDeathRateTemp[0].fOutput=(float)iValue;
		    	
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->RtDeathRateTemp[i].fInput = ReadFloat(hFile);
		for (i=1;i<=iValue;i++)
			pPl->pGenotype->RtDeathRateTemp[i].fOutput= ReadFloat(hFile);
		}  

	CloseHandle( hFile );

	return 1;
}

/*******************************************************************************
** EOF */