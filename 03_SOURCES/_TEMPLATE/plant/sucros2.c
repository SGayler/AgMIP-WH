/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author: 
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: sucros2.c $
 * 
 * *****************  Version 4  *****************
 * Updated in $ExpertN.3.0/ModLib/plant
 * Unsinnige Defines entfernt (WINAPI _loadds wird überdies bereits verwendet).
 * 
*******************************************************************************/

#include <windows.h>
#include <math.h>
#include  "xinclexp.h"

//extern int COMVR_TO_CERES(EXP_POINTER);
extern double WINAPI _loadds LIMIT(double v1, double v2, double x);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Export Funtions developed based on SUCROS2
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PhasicDevelopment_SUCROS			    (EXP_POINTER);
int WINAPI _loadds Photosynthesis_SUCROS(EXP_POINTER);
int WINAPI _loadds BiomassGrowth_SUCROS				    (EXP_POINTER);
int WINAPI _loadds CanopyFormation_SUCROS				(EXP_POINTER);
int WINAPI _loadds RootSystemFormation_SUCROS			(EXP_POINTER);
float WINAPI _loadds PotentialTranspiration_SUCROS		(EXP_POINTER);
int WINAPI _loadds ActualTranspiration_SUCROS			(EXP_POINTER);
int WINAPI _loadds PotentialNitrogenUptake_WAVE		    (EXP_POINTER);
int WINAPI _loadds ActualNitrogenUptake_WAVE			(EXP_POINTER);

extern int   Get_Diffusion_Const(EXP_POINTER);//from transp.c
int PlantHeightGrowth_SUCROS(EXP_POINTER);

extern float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp);
extern int WINAPI _loadds NitrogenConcentrationLimits_SPASS(EXP_POINTER);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//float PotentialEvapTranspiration_Penman		(EXP_POINTER);
float LeafCO2Assimilation_SUCROS(float AMX, float fDayTemp, float fStageVR, PRESPONSE, RESPONSE[21]);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Internal Funtions 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double 	DailyCanopyGrossPhotosynthesis_SUCROS(EXP_POINTER);

double 	CanopyGrossPhotosynthesis_SUCROS(double AMAX, double EFF, double vLAI, double PARDIR, double PARDIF, LPSTR CropCode);
int 	Astronomy_SUCROS(int iJulianDay, double vLatitude);


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Variable definition
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#define RAD	PI/180.0

double SC,DS0, SINB, SINLD, COSLD, DAYL, DSINB, DSINBE;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Model Coefficients 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double SCV	=	0.20; 	//Scattering coefficient of leaves for visible radiation (PAR)
//double AMX	=	40.0;	//Assimilation rate at light saturation (kg CO2/ ha leaf/h )
//double EFF 	= 	0.45;	//Initial light use efficiency (kg CO2/J/ha/h m2 s )
//double KDIF = 	0.60;	//Extinction coefficient for diffuse light 
//double k	= 	0.50;	//The average extincction coefficient for visible and near infrared radiation

float fPotTraDay, fActTraDay;

float fBaseTemperature1;
float fCriticalTemperatureSum1;
float fBaseTemperature2;
float fCriticalTemperatureSum2;

//SG 20110810: CO2 als Input von *.xnm (für AgMIP)
extern float fAtmCO2;


RESPONSE	PmaxAge[21];

 iStageOld=0;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Phasic Development
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PhasicDevelopment_SUCROS(EXP_POINTER)
	{
	PDEVELOP 	pDev;       
	float		fTemp;
	float		VR[10];
	int i;
	//double      DVR[]={0.0, 0.1425, 0.39, 0.656, 0.91, 1.00, 1.15, 1.50, 1.95, 2.0};
	double      DVR[]={0.0, 0.4, 0.55, 0.656, 0.91, 1.00, 1.15, 1.50, 1.95, 2.0};

    for (i=0;i<10;i++) VR[i]=(float)DVR[i];
	
	pDev = pPl->pDevelop;
	//-----------------------------------------------------------------------------
	// Call CERES development model before emergence
	//-----------------------------------------------------------------------------
    iStageOld=pDev->iStageCERES;

	/*

	  if ((pDev->iStageCERES>6)||(pDev->iStageCERES==0))
		{
		DevelopStage1	(exp_p);
		//CERES_TO_COMVR	(exp_p);
		
		if (iStageOld!=pDev->iStageCERES)
			{   
			iOld = pPl->pDevelop->iStageCERES-1;
	    
	    	if (iOld>=1)
				pPl->pDevelop->fSumDTT -= pPl->pGenParam->asStageParam[iOld].fThermalTime;
			}    
		}
*/	
	//-----------------------------------------------------------------------------
	// SUCROS DEVELOPMENT MODEL AFTER EMERGENCE
	//-----------------------------------------------------------------------------
   	if (pDev->fStageSUCROS<0)
	{

		if(pTi->pSimTime->fTimeAct >= (float)(pPl->pModelParam->iEmergenceDay))
			pDev->fStageSUCROS= (float)0.0;

		pDev->iDayAftEmerg = 0;
	}
	else
   	{
      if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH\0"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA\0"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ\0")))
      { //Weizen, Gerste oder Mais 
      
//		if (pDev->fStageSUCROS<=(float)1.0)
//			pDev->fDevR = fTemp*(float)(0.027/30.0);
//		else
//			pDev->fDevR = fTemp*(float)(0.031/30.0);


		if (pDev->fStageSUCROS<=(float)1.0)
		{
			fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)
				-fBaseTemperature1);

			pDev->fDevR = fTemp/fCriticalTemperatureSum1;
		}
		else
		{
			fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)
				-fBaseTemperature2);

			pDev->fDevR = fTemp/fCriticalTemperatureSum2;
		}
		
	  }

	  if (!lstrcmp(pPl->pGenotype->acCropCode,"PT\0"))        
      { //Kartoffel
	   fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin));
       
	   if (fTemp<=(float)13) fTemp -=(float)2.0;
	   else fTemp = (float)29.0 - fTemp;

	   //SG 2005-07-21: fDevR an WH/BA/MZ angepasst!
	   //pDev->fDevR = (float)min((float)11.0,(float)max(fTemp,(float)0.0));
	   //pDev->fDevR /= (float)30.0;

	   	if (pDev->fStageSUCROS<=(float)1.0)
		{
			fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)
				-fBaseTemperature1);

			pDev->fDevR = fTemp/fCriticalTemperatureSum1;
		}
		else
		{
			fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)
				-fBaseTemperature2);

			pDev->fDevR = fTemp/fCriticalTemperatureSum2;
		}

	  }

	  if (!lstrcmp(pPl->pGenotype->acCropCode,"SB\0"))        
      { //Zuckerruebe
       
	   //SG 2005/11/15: angepasst an PT
	   //fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin));

	   //pDev->fDevR = (float)min((float)19.0,(float)max(fTemp-(float)2.0,(float)0.0));
	   ////pDev->fDevR = fTemp*(float)(0.031/30.0);
	   //pDev->fDevR /= (float)1000; 


	   	if (pDev->fStageSUCROS<=(float)1.0)
		{
			fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)
				-fBaseTemperature1);

			pDev->fDevR = (float)min((float)19.0,fTemp/fCriticalTemperatureSum1);
		}
		else
		{
			fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)
				-fBaseTemperature2);

			pDev->fDevR = (float)min((float)19.0,fTemp/fCriticalTemperatureSum2);
		}

	  }

      if (!lstrcmp(pPl->pGenotype->acCropCode,"SF\0"))        
      { //Sonnenblumen
	   fTemp = (float)max(0.0,0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin));
	   pDev->fDevR = fTemp*(float)(0.031/30.0);
      }

	  
	  pDev->fStageSUCROS += pDev->fDevR;

		//wegen Ausgabe:
	    //pDev->fDevStage = pDev->fStageSUCROS*(float)10;
	    //COMVR_TO_CERES(exp_p);
	
	//--------------------------------------------------------------------
	//From fStageSUCROS to pDev->fStageWang
   	//--------------------------------------------------------------------
	if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<=VR[1]))
		pDev->fDevStage=(float)(10.0*(1.0+(pDev->fStageSUCROS-VR[0])/(VR[1]-VR[0])));
	if ((pDev->fStageSUCROS>VR[1])&&(pDev->fStageSUCROS<=VR[2]))
		pDev->fDevStage=(float)(10.0*(2.0+(pDev->fStageSUCROS-VR[1])/(VR[2]-VR[1])));
	if ((pDev->fStageSUCROS>VR[2])&&(pDev->fStageSUCROS<=VR[3]))
		pDev->fDevStage=(float)(10.0*(3.0+(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2])));
	if ((pDev->fStageSUCROS>VR[3])&&(pDev->fStageSUCROS<=VR[4]))
		pDev->fDevStage=(float)(10.0*(4.0+(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3])));
	if ((pDev->fStageSUCROS>VR[4])&&(pDev->fStageSUCROS<=VR[5]))
		pDev->fDevStage=(float)(10.0*(5.0+(pDev->fStageSUCROS-VR[4])/(VR[5]-VR[4])));
	if ((pDev->fStageSUCROS>VR[5])&&(pDev->fStageSUCROS<=VR[6]))
		pDev->fDevStage=(float)(10.0*(6.0+(pDev->fStageSUCROS-VR[5])/(VR[6]-VR[5])));

	if ((pDev->fStageSUCROS>VR[6])&&(pDev->fStageSUCROS<=VR[7]))
		pDev->fDevStage=(float)(10.0*(7.0+(pDev->fStageSUCROS-VR[6])/(VR[7]-VR[6])));
	if ((pDev->fStageSUCROS>VR[7])&&(pDev->fStageSUCROS<=VR[8]))
		pDev->fDevStage=(float)(10.0*(8.0+(pDev->fStageSUCROS-VR[7])/(VR[8]-VR[7])));
	if ((pDev->fStageSUCROS>VR[8])&&(pDev->fStageSUCROS<=VR[9]))
		pDev->fDevStage=(float)(10.0*(9.0+0.2*(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8])));
	if (pDev->fStageSUCROS>VR[9])
		pDev->fDevStage=(float)92.0;

	
	}

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Daily Canopy Gross Photosynthesis
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds Photosynthesis_SUCROS(EXP_POINTER)
	{
	int 	iJulianDay;
	double 	vLatitude;
	double  Pg;
	float   f1,fNitRed;

	PPLTNITROGEN	pPltN = pPl->pPltNitrogen;

	//SG20141103
	float fCO2;

	iJulianDay			= (int)pTi->pSimTime->iJulianDay;
	vLatitude			= (double)pLo->pFarm->fLatitude;
	
	Astronomy_SUCROS(iJulianDay,vLatitude);
	
	Pg=DailyCanopyGrossPhotosynthesis_SUCROS(exp_p);
	

	//	SG 25.3.2003: fStressFacPhoto eingeführt wegen Initialisierung des Wasserstress
    // pPl->pPltCarbon->fGrossPhotosynR=
	//  (float)(Pg*30.0/44.0)*pPl->pPltWater->fActTranspdt/pPl->pPltWater->fPotTranspdt;

	f1=(pPltN->fLeafActConc-pPltN->fLeafMinConc)/(pPltN->fLeafOptConc-pPltN->fLeafMinConc);
    fNitRed  = (float)min((float)1,(float)max((float)0,f1));

	//pPl->pPltCarbon->fGrossPhotosynR=(float)(Pg*30.0/44.0)*pPl->pPltWater->fStressFacPhoto;
	pPl->pPltCarbon->fGrossPhotosynR=(float)(Pg*30.0/44.0)*(float)min(pPl->pPltWater->fStressFacPhoto,fNitRed);
    
    return 1;
    }
    

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Astronomy_SUCROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int Astronomy_SUCROS(int iJulianDay, double vLatitude)
	{
	double DEC,AOB;
	int    nHiWord;

	//Check on input range of parameters
	if (vLatitude>67.0)		nHiWord=0;	//Latitude too high
	if (vLatitude<-67.0) 	nHiWord=0;	//Latitude too low

	//Declination of the sun as function of iJulianDaynumber (iJulianDay)
	DEC = -asin( sin(23.45*RAD)*cos(2.0*PI*(iJulianDay+10.0)/365.0));
	
	//SINLD, COSLD and AOB are intermediate variables
	SINLD = sin(RAD*vLatitude)*sin(DEC);
	COSLD = cos(RAD*vLatitude)*cos(DEC);
	AOB   = SINLD/COSLD;
	
	//iJulianDaylength (DAYL) 
	DAYL   = 12.0*(1.0+2.0*asin(AOB)/PI);
    //SINB = SINLD + COSLD*cos(2.0*PI(fSolarTime-12.0)/24.0), fSolarTime [h]
	DSINB  = 3600.0*(DAYL*SINLD+24.0*COSLD*sqrt(1.0-AOB*AOB)/PI);//daily integral over SINB
	DSINBE = 3600.0*(DAYL*(SINLD+0.4*(SINLD*SINLD+COSLD*COSLD*0.5))
			+12.0*COSLD*(2.0+3.0*0.4*SINLD)*sqrt(1.0-AOB*AOB)/PI);//daily integral 
	//for the calculation of actual global radiation from daily global radiation (input)

	//Solar constant (SC) and daily extraterrestrial radiation (DS0) 
	SC  = 1370.0*(1.0+0.033*cos(2.0*PI*iJulianDay/365.0));
	DS0 = SC*DSINB;
    
    return 1;
	}
	
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Daily Canopy gross photosynthesis 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double DailyCanopyGrossPhotosynthesis_SUCROS(EXP_POINTER)
	{
	int		i,j;
	double 	DTGA,FGROS,HOUR,PAR,ATMTR,FRDIF,PARDIF,PARDIR;
	double 	vLAI,vGlobalRadiation;
	float 	AMAX,fDayTemp,fStageVR;
	int 	nGauss=3;
    double  xGauss[]={0.1127, 0.5000, 0.8873};
	double 	wGauss[]={0.2778, 0.4444, 0.2778};

	double AMX;				//=	40.0; Assimilation rate at light saturation (kg CO2/ ha leaf/h )
	double EFF, EFF0; 			//= 0.45; Initial light use efficiency (kg CO2/J/ha/h m2 s )

	float fCO2;
	char pType[5];	//SG20140914: Unterscheidung C3/C4



	vLAI				= (double)	pPl->pCanopy->fLAI;
	vGlobalRadiation	= (double)	pCl->pWeather->fSolRad*1E6; 	//[J/(m2*d)]
    fDayTemp       		= pCl->pWeather->fTempMax-(float)0.25*
    					 (pCl->pWeather->fTempMax-pCl->pWeather->fTempMin);
    fStageVR			= pPl->pDevelop->fStageSUCROS;


	AMX		=	(double)pPl->pGenotype->fMaxGrossPs;
	EFF 	= 	(double)pPl->pGenotype->fLightUseEff;

	lstrcpy(pType,"C3\0");

	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI"))
		||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG")))

		lstrcpy(pType,"C4\0");

		
	fCO2= (fAtmCO2 > 0? fAtmCO2:CO2);

	//SG 20110810:
	//CO2-dependency of light use efficency for AgMIP-project
	if(!lstrcmp((LPSTR)pType,(LPSTR)"C3"))
	{
		EFF = (0.825 * fCO2)/(280.0+fCO2); //EFF(280 ppm) = 0.41, EFF(360 ppm) = 0.46, EFF(720 ppm) = 0.59
	}

	//SG20121112: CO2-response of C4-crops
	if(!lstrcmp((LPSTR)pType,(LPSTR)"C4"))
	{
		EFF0 = EFF;
		EFF= (1.5 * EFF0 * fCO2)/(165.0+fCO2); //EFF(330 ppm) = EFF0, EFF(380 ppm) = 1.046*EFF0, EFF(660 ppm) = 1.2*EFF0 //For C4 crops
	}

    
	//Assimilation set to zero and three different times of the Day (HOUR)
    DTGA = 0;
      
	for (i=1;i<=nGauss;i++)
		{
		j=i-1;
		//At the specified HOUR, radiation is computed and used to compute assimilation
		HOUR = 12.0+DAYL*0.5*xGauss[j];
		 
		//Sine of solar elevation
		SINB  = max(0.0, SINLD+COSLD*cos(2.0*PI*(HOUR+12.0)/24.0));
		
		//Diffuse light fraction (FRDIF) from atmospheric transmission (ATMTR)
		PAR   = 0.5*vGlobalRadiation*SINB*(1.0+0.4*SINB)/DSINBE;
		ATMTR = PAR/(0.5*SC*SINB);
		
		if (ATMTR<=0.22) 
			FRDIF = 1.0;
		else
			{
			if ((ATMTR>0.22)&&(ATMTR<=0.35)) 
				FRDIF = 1.0-6.4*(ATMTR-0.22)*(ATMTR-0.22); 
			else
				FRDIF = 1.47-1.66*ATMTR;
			}
			
		FRDIF = max (FRDIF, 0.15+0.85*(1.0-exp(-0.1/SINB)));
		
		
		//Diffuse PAR (PARDIF) and direct PAR (PARDIR)
		PARDIF = min(PAR, SINB*FRDIF*ATMTR*0.5*SC);
		PARDIR = PAR-PARDIF;
		
		AMAX  = LeafCO2Assimilation_SUCROS((float)AMX, fDayTemp, fStageVR, pPl->pGenotype->PmaxTemp, PmaxAge);
		
		FGROS = CanopyGrossPhotosynthesis_SUCROS(AMAX, EFF, vLAI,PARDIR,PARDIF, pPl->pGenotype->acCropCode);

		
		//Integration of assimilation rate to a daily total (DTGA) 
		DTGA = DTGA+FGROS*wGauss[j];
    	}
    	
	DTGA = DTGA * DAYL;
	
	return DTGA;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Leaf CO2 assimilation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float LeafCO2Assimilation_SUCROS(float AMX, float fDayTemp, float fStageVR, PRESPONSE PmaxTemp, RESPONSE PmaxAge[21])
	{
	float fAgeEff,fTempEff,fActPs;
		
	//Effect due to aging
	if(PmaxAge[0].fInput == (float)0.0)
	{
		if (fStageVR<= (float)1.0)
			fAgeEff = (float)1.0;
		else
			fAgeEff = (float)1.0-(fStageVR-(float)1.0)*(float)0.5;
	}
	else
		fAgeEff = AFGENERATOR(fStageVR,PmaxAge);
	
	//Effect of day time temperature
	if(PmaxTemp[0].fInput == (float)0.0)
	{
		if (fDayTemp<=(float)0.0)
			fTempEff = (float)0.0;
		if ((fDayTemp>(float)0.0)&&(fDayTemp<=(float)10.0))
			fTempEff = fDayTemp*(float)0.1;
		if ((fDayTemp>(float)10.0)&&(fDayTemp<=(float)25.0))
			fTempEff = (float)1.0;
		if ((fDayTemp>(float)25.0)&&(fDayTemp<=(float)35.0))
			fTempEff = (float)1.0-(fDayTemp-(float)25.0)/(float)10.0;
		if (fDayTemp>(float)35.0)
			fTempEff =(float)0.0;
	}
	else
		fTempEff = AFGENERATOR(fDayTemp,PmaxTemp);
	
	fActPs = AMX*fAgeEff*fTempEff;
	
	return fActPs;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Canopy gross photosynthesis 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double CanopyGrossPhotosynthesis_SUCROS(double AMAX, double EFF, double vLAI, double PARDIR, double PARDIF, LPSTR CropCode)
	{
	int		i,j;
	double 	SQV,REFH,REFS,CLUSTF,KDIRBL,KDIRT,FGROS,LAIC,VISDF,VIST,VISD,VISSHD,FGRSH;
	double 	FGRSUN,VISSUN,VISPP,FGRS,FSLLA,FGL;
	int 	nGauss=3;
    double  xGauss[]={0.1127, 0.5000, 0.8873};
	double 	wGauss[]={0.2778, 0.4444, 0.2778};

	//SG 20110810: Atmospheric CO2-concentration from Input (*.xnm) for AgMIP project
	//float fCO2, EFF0;
		   

	double SCV	=	0.20; 	//Scattering coefficient of leaves for visible radiation (PAR)
	double KDIF = 	0.60;	//Extinction coefficient for diffuse light 

	if (!lstrcmp((LPSTR)CropCode,(LPSTR)"PT"))	KDIF=1.0;
    if (!lstrcmp((LPSTR)CropCode,(LPSTR)"SB"))	KDIF=0.69;
    if (!lstrcmp((LPSTR)CropCode,(LPSTR)"MZ"))	KDIF=0.65;



	//Reflection of horizontal and spherical leaf angle distribution
	SQV  = sqrt(1.0-SCV);
	REFH = (1.0-SQV)/(1.0+SQV);
	REFS = REFH*2.0/(1.0+2.0*SINB);

	//Extinction coefficient for direct radiation and total direct flux
	CLUSTF = KDIF / (0.8*SQV); 
	KDIRBL = (0.5/SINB) * CLUSTF;
	KDIRT  = KDIRBL * SQV;

	////SG 20110810:
	////CO2-dependency of light use efficency for AgMIP-project
	//fCO2= (fAtmCO2 > 0? fAtmCO2:CO2);
	//EFF = (0.825 * fCO2)/(280.0+fCO2); //EFF(280 ppm) = 0.41, EFF(360 ppm) = 0.46, EFF(720 ppm) = 0.59

	////SG20121112: CO2-response of C4-crops
	//if(!lstrcmp((LPSTR)CropCode,(LPSTR)"MZ"))
	//{
	//	EFF0 = EFF;
	//	EFF= (1.5 * EFF0 * fCO2)/(165.0+fCO2); //EFF(330 ppm) = EFF0, EFF(380 ppm) = 1.046*EFF0, EFF(660 ppm) = 1.2*EFF0 //For C4 crops
	//}

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
		 if (AMAX>0.0) 
		 	FGRSH = AMAX * (1.0-exp(-VISSHD*EFF/AMAX));
		 else
		    FGRSH = 0.0;
		    
		//Direct flux absorbed by leaves perpendicular on direct beam and
		//assimilation of sunlit leaf area
         VISPP  = (1.0-SCV) * PARDIR / SINB;

         FGRSUN = 0.0;
         for (j=0;j<nGauss;j++)
         	{
         	VISSUN = VISSHD + VISPP * xGauss[j];
            
            if (AMAX>0.0) 
            	FGRS = AMAX * (1.0-exp(-VISSUN*EFF/AMAX));
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
	
	return FGROS;
	}

float fStemTranslocationRate;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Respiration
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds CropMaintenance_SUCROS(EXP_POINTER)
	{       
	float MainLv,MainSt,MainRt,MainSo,Maints,TempRef,TempEff,fTemp,MnDvs,fTotLvWeight;
	double 	Q10;

	PGENOTYPE	pGen = pPl->pGenotype;

	
	Q10   	= 2.0;
	TempRef	=(float)25.0;
	
	//MainLv	=(float)0.03;
	//MainSt	=(float)0.015;
	//MainRt	=(float)0.015;
	//MainSo	=(float)0.01;

	MainLv	= pGen->fMaintLeaf;
	MainSt	= pGen->fMaintStem;
	MainRt	= pGen->fMaintRoot;
	MainSo	= pGen->fMaintStorage;

	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//Maintenance Respiration
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//Temperature effect
	fTemp =(float)0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin);
	TempEff=(float)pow(Q10,(double)(fTemp-TempRef)/10.0);
	
	//Maintenance of different organs
	Maints  = 	MainLv*pPl->pBiomass->fLeafWeight
			+	MainSt*pPl->pBiomass->fStemWeight
			+	MainRt*pPl->pBiomass->fRootWeight
			+	MainSo*pPl->pBiomass->fGrainWeight;

	//Maintenance change due to aging
//	fTotLvWeight=pPl->pBiomass->fLeafWeight+pPl->pBiomass->fDeadLeafWeight;
	fTotLvWeight=pPl->pBiomass->fLeafWeight+pPl->pBiomass->fDeadLeafWeight;

	if (fTotLvWeight==(float)0.0)
		MnDvs=(float)0.0;
	else
		MnDvs = pPl->pBiomass->fLeafWeight/fTotLvWeight;
		
	//Maintenance respiration 	
	pPl->pPltCarbon->fMaintRespR = (float)min(Maints*TempEff*MnDvs,pPl->pPltCarbon->fGrossPhotosynR);      
	
	
	return 1;
	}
	

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	BiomassGrowth_SUCROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds BiomassPartition_SUCROS(EXP_POINTER);

int WINAPI _loadds BiomassGrowth_SUCROS(EXP_POINTER)
	{
	float ASRQ,ASRQLV,ASRQRT,ASRQST,ASRQSO,CFST,FRTRL,GTW;
	//float CFLV,CFRT,CFSO;
	PGENOTYPE	pGen = pPl->pGenotype;
	PBIOMASS	pBiom= pPl->pBiomass;

	ASRQLV  = pGen->fGrwEffLeaf; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	ASRQRT  = pGen->fGrwEffRoot; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	ASRQST  = pGen->fGrwEffStem; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	ASRQSO  = pGen->fGrwEffStorage; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	
	//ASRQLV  =(float)1.463; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	//ASRQRT  =(float)1.444; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	//ASRQST  =(float)1.513; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	//ASRQSO  =(float)1.415; 	//Assimilate reqirement for conversion of carbohydrates to dry matter
	
	//CFLV	=(float)0.459;	//C-content of leaves
	//CFRT	=(float)0.467;	//C-content of root
	  CFST	=(float)0.494;	//C-content of stem
	//CFSO	=(float)0.471;	//C-content of Storage organs
	
	FRTRL 	=(float)0.20;	//Translocation fraction of stem weight

	
	CropMaintenance_SUCROS(exp_p);
	BiomassPartition_SUCROS(exp_p);

	//Assimilate reqirement for dry matter production   kg CH2O/kg DM
	ASRQ =  ASRQLV*pPl->pBiomass->fPartFracLeaf
	       +ASRQST*pPl->pBiomass->fPartFracStem
	       +ASRQSO*pPl->pBiomass->fPartFracFruit
	       +ASRQRT*pPl->pBiomass->fPartFracRoot;

	//Translocation of stem dry matter
	if (pPl->pDevelop->fStageSUCROS<(float)1.0)
		fStemTranslocationRate = (float)0.0;
	else
		fStemTranslocationRate = pPl->pBiomass->fStemWeight
								 *pPl->pDevelop->fDevR*FRTRL;	       
	
	if (ASRQ==(float)0.0)
		GTW=(float)0.0;
	else
		GTW = (pPl->pPltCarbon->fGrossPhotosynR-pPl->pPltCarbon->fMaintRespR
		 	+(float)0.974*fStemTranslocationRate*CFST*(float)(30.0/12.0))/ASRQ;

	if(GTW<=(float)0.0)
		 GTW=(float)0.0;
					
/*	pPl->pPltCarbon->fGrowthRespR= pPl->pPltCarbon->fGrossPhotosynR
									-pPl->pPltCarbon->fMaintRespR
									-GTW;
	
	if (pPl->pPltCarbon->fGrowthRespR<(float)0.0)
		pPl->pPltCarbon->fGrowthRespR=(float)0.0;
		
	
	pPl->pPltCarbon->fDarkRespR = pPl->pPltCarbon->fMaintRespR
									  +pPl->pPltCarbon->fGrowthRespR;  	
	pPl->pPltCarbon->fNetPhotosyn = pPl->pPltCarbon->fGrossPhotosynR
									  -pPl->pPltCarbon->fDarkRespR;

	pBiom->fBiomGrowR	= pPl->pPltCarbon->fGrossPhotosynR
						 -pPl->pPltCarbon->fMaintRespR
						 -pPl->pPltCarbon->fGrowthRespR;
	
	GTW = pBiom->fBiomGrowR; */

    pBiom->fBiomGrowR = GTW;

	pBiom->fLeafGrowR = GTW*pBiom->fPartFracLeaf;
	pBiom->fStemGrowR = GTW*pBiom->fPartFracStem-fStemTranslocationRate;
	pBiom->fGrainGrowR= GTW*pBiom->fPartFracFruit;
	pBiom->fRootGrowR = GTW*pBiom->fPartFracRoot; 
	                          
	
	pPl->pBiomass->fDeadLeafWeight += pPl->pBiomass->fLeafDeathRate;
	
	pBiom->fLeafWeight 	+= pBiom->fLeafGrowR-pPl->pBiomass->fLeafDeathRate; 
 	pBiom->fStemWeight  += pBiom->fStemGrowR;
	pBiom->fGrainWeight += pBiom->fGrainGrowR;
	if (pBiom->fGrainWeight<(float)1.0e-3) pBiom->fGrainWeight=(float)0;
	pBiom->fRootWeight  += pBiom->fRootGrowR;

	pPl->pBiomass->fTotLeafWeight	+=pBiom->fLeafGrowR;
	pPl->pBiomass->fTotRootWeight	+=pBiom->fRootGrowR;
	pPl->pBiomass->fTotStemWeight	+=pBiom->fStemGrowR;

	pBiom->fBiomassAbvGround  = 	 pBiom->fLeafWeight  
									+pBiom->fStemWeight  
									+pBiom->fGrainWeight;

	pBiom->fStovWeight = pBiom->fLeafWeight+pBiom->fStemWeight;

	pBiom->fFruitWeight = pBiom->fGrainWeight;

	pBiom->fTotalBiomass		= pBiom->fBiomassAbvGround
 						  		 +pBiom->fRootWeight;
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	BiomassPartition_SUCROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


int WINAPI _loadds BiomassPartition_SUCROS(EXP_POINTER)
	{
	float  x;
	float  fWsPartition;
	float  fTopFraction;

	PRESPONSE 	pResp1,pResp2,pResp3;
	PBIOMASS 	pBiom = pPl->pBiomass;
	
	x = pPl->pDevelop->fStageSUCROS;

	fWsPartition=pPl->pPltWater->fStressFacPartition;

		
	//Get the pointer of the partitioning data 
	pResp1	= pPl->pGenotype->PartRoot;
	pResp2	= pPl->pGenotype->PartLeaf;
	pResp3	= pPl->pGenotype->PartStem;

	if ((pResp1->fInput==(float)0.0)||(pResp2->fInput==(float)0.0)||(pResp3->fInput==(float)0.0))
		{
		//Top and Root Fraction
			if (x<0.0)                                      
				fTopFraction=(float)0.0;
			if ((x>=0.0)&&(x<1.2))
				fTopFraction=(float)(0.1851*x*x*x-0.8192*x*x+1.1744*x+0.4465);
			if (x>=1.2)
    			fTopFraction=(float)1.0;


			fTopFraction=(fTopFraction*fWsPartition)
					/((float)1.0+(fWsPartition-(float)1.0)
					 *fTopFraction);
												
			pBiom->fPartFracRoot = (float)1.0-fTopFraction;

			//Leaf, stem, grain fraction
			if (x<0.0)                                
				pBiom->fPartFracLeaf=(float)0.0;
			if ((x>=0.0)&&(x<0.95))
				pBiom->fPartFracLeaf=(float)(2.5718*x*x*x-4.4569*x*x+1.2504*x+0.6194);
			if (x>=0.95)
    			pBiom->fPartFracLeaf=(float)0.0;

			if (x<0.0)
				pBiom->fPartFracStem=(float)0.0;
			if ((x>=0.0)&&(x<0.95))
				pBiom->fPartFracStem=(float)1.0-pBiom->fPartFracLeaf;
			if (x>=0.95)
    			pBiom->fPartFracStem=(float)0.0;

   			pBiom->fPartFracLeaf *=fTopFraction;
   			pBiom->fPartFracStem *=fTopFraction;
			
			pBiom->fPartFracFruit=	fTopFraction
									  - pBiom->fPartFracLeaf
									  - pBiom->fPartFracStem;
		}
		
	else
		{
		//Root and Top Fraction
		pBiom->fPartFracRoot = AFGENERATOR(x, pPl->pGenotype->PartRoot);
		//Einfluß Wassermangel:
		pBiom->fPartFracRoot = pBiom->fPartFracRoot/
			(fWsPartition+((float)1-fWsPartition)*pBiom->fPartFracRoot);
		
		fTopFraction  	   = (float)1.0 - pBiom->fPartFracRoot;         
	
		//Leaf, stem, grain fraction
		pBiom->fPartFracLeaf = fTopFraction*AFGENERATOR(x, pPl->pGenotype->PartLeaf);
	    pBiom->fPartFracStem = fTopFraction*AFGENERATOR(x, pPl->pGenotype->PartStem);
	
	    pBiom->fPartFracGrain= (float)max(0.0,fTopFraction-pBiom->fPartFracLeaf-pBiom->fPartFracStem);
		pBiom->fPartFracFruit = pBiom->fPartFracGrain;
		}
	
	return 1;
	}
	
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	CanopyFormation_SUCROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float fLeafAgeDeathRate,fEarAreaGrowR,fEAI;

int LeafAreaGrowthRate(EXP_POINTER);
int EarAreaGrowthRate(EXP_POINTER);

int WINAPI _loadds CanopyFormation_SUCROS(EXP_POINTER)
	{
    double x,VR,LAICR;   
    float  DLAI;  
    float  fLeafShadeDeathRate,fRelLeafDeathRate;	

   	PCANOPY	pCan=pPl->pCanopy;
   	
	LAICR 	= 4.0; //Critical LAI over which leaf dies due to shading
	x  		= 0.5*(double)(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin);
    VR 		= (double)pPl->pDevelop->fStageSUCROS;
    
	//Leaf death rate due to developmental age
	if (VR<1.0)
		fLeafAgeDeathRate = (float)(0.0);
	else
	{
		//SG 20110815: für Pareto-Analyse
//		fLeafAgeDeathRate = (float)0.25*(-2.0E-6*x*x*x+0.0002*x*x-0.0017*x+0.03);
		fLeafAgeDeathRate = (float)(-2.0E-6*x*x*x+0.0002*x*x-0.0017*x+0.03);
	}

	//Leaf death rate due to shading
	if (pCan->fLAI<(float)LAICR)
		fLeafShadeDeathRate=(float)0.0;
	if ((pCan->fLAI>=(float)LAICR)&&(pCan->fLAI<=(float)(2.0*LAICR)))
		fLeafShadeDeathRate=(float)(0.03*((double)pCan->fLAI-LAICR)/LAICR);
	if (pCan->fLAI>(float)(2.0*LAICR))
		fLeafShadeDeathRate=(float)0.03;

	//Relative leaf death rate
 	fRelLeafDeathRate = max(fLeafAgeDeathRate, fLeafShadeDeathRate);
	//Dead leaf area index
	DLAI = pCan->fLAI*fRelLeafDeathRate;
		                                     
	LeafAreaGrowthRate(exp_p);

	if(!lstrcmp(pPl->pGenotype->acCropCode,"WH\0"))
	{//Wheat

	  EarAreaGrowthRate (exp_p);
    
 	  pCan->fLAI 		-= (float)0.5*fEAI;
    //  pCan->fTotalLAI -= (float)0.5*fEAI;

      fEAI += fEarAreaGrowR;
      pCan->fLAI += (float)0.5*fEAI+pCan->fLAGrowR-DLAI;
	  // pCan->fTotalLAI+= (float)0.5*pCan->fEAI+pCan->fLAGrowR;
	}
	else
	{
      fEAI = (float)0.0;
 	  pCan->fLAI += pCan->fLAGrowR-DLAI;
	}

	//Leaf death rate
	if (pCan->fLAI==(float)0.0)
		pPl->pBiomass->fLeafDeathRate = (float)0.0;
	else
		pPl->pBiomass->fLeafDeathRate = pPl->pBiomass->fLeafWeight*DLAI/pCan->fLAI;

	//SG 2011 08 09: Berechnung der Grain number für AgMIP (wie SPASS)
	//=========================================================================================
	//If flowering occurs, calculate the grain number and days after anthesis	
	//=========================================================================================
	if (pPl->pDevelop->fStageSUCROS<(float)1.0)
		pPl->pDevelop->iDayAftAnthesis=(float)0.0;
	else
		pPl->pDevelop->iDayAftAnthesis++;
		
	if (pPl->pDevelop->iDayAftAnthesis==1)
	{   //Grains per plant
		pCan->fGrainNum	 = pPl->pBiomass->fStemWeight/((float)10.0*pMa->pSowInfo->fPlantDens)*pPl->pGenotype->fStorageOrganNumCoeff;  	

		if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		{   //Grains per plant
			pCan->fGrainNum	 = pPl->pGenotype->fStorageOrganNumCoeff;  			
		}           

    }           

	return 1;
	}

int PlantHeightGrowth_SUCROS(EXP_POINTER)
	{
		float fSSL, fSSLmax;
		float fAS, fBS, fHS, fHB;

		if(pPl->pDevelop->fStageSUCROS<(float)0.0)
			return 0;


		if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SW"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"TC")))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.5;  //
			fHS = (float)8.0;
			fHB = (float)300.0;
		}
		else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"S")))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.8;  //
			fHS = (float)8.0;
			fHB = (float)300.0;
		}
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.8;  //
			fHS = (float)8.0;
			fHB = (float)300.0;
		}
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.5;  //
			fHS = (float)8.0;
			fHB = (float)298;
		}
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.8;  //
			fHS = (float)8.0;
			fHB = (float)300.0;
		}
		else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RY"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SR")))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.8;  //
			fHS = (float)8.0;
			fHB = (float)300.0;
		}
      else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.8;  //
			fHS = (float)7.0; //(aus INTERCOM)
			fHB = (float)67.0; //(aus INTERCOM)
		}
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SY"))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.8;  //
			fHS = (float)8.0;
			fHB = (float)300.0;
		}
		else
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.8;  //
			fHS = (float)8.0;
			fHB = (float)300.0;
		}

		if(pPl->pBiomass->fStemWeight > (float)0.0)
			fSSL = pPl->pCanopy->fPlantHeight/(pPl->pBiomass->fStemWeight/(float)10.0*pMa->pSowInfo->fPlantDens); //cm/g
		else
			fSSL = (float)0.0;

		fSSLmax = (float)exp(-fAS*pPl->pCanopy->fPlantHeight + fBS);

		if(fSSLmax>fSSL)
		{
			pPl->pCanopy->fPlantHeight += pPl->pDevelop->fDevR*pPl->pGenotype->fPlantHeight*fHS*fHB
					*(float)exp(-fHS*pPl->pDevelop->fStageSUCROS)/(float)pow((1.0+fHB*exp(-fHS*pPl->pDevelop->fStageSUCROS)),2)
					*pPl->pPltWater->fStressFacPhoto;
		}

	return 1;
	}

	
int LeafAreaGrowthRate(EXP_POINTER)
	{
 	float  SLA,LA0;
 	double RGRL,vTemp,vTbase;
 	static int Emerg=0;
 	
 	//SLA =(float)0.0022; 	//Specific leaf area (ha leaf/kg leaf weight)      
	
	//WH,BA
	LA0 =(float)5.7E-5;		//Initial leaf area (m2/plant)
    RGRL	=0.007;			//0.009 Relative growth rate of leaf area (1/(C.d))
    vTbase 	=0.0;

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
	{
		LA0=(float)4.0E-3; //Initial leaf area (m2/plant)
		RGRL	=0.012;	   //Relative growth rate of leaf area (1/(C.d))
	}
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))
	{
		LA0=(float)3.0E-3; //Initial leaf area (m2/plant)
		RGRL	=0.016;	   //Relative growth rate of leaf area (1/(C.d))
	}
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
	{
 
		LA0=(float)2.0E-3; //Initial leaf area (m2/plant)
		//RGRL	=0.0294;	   //Relative growth rate of leaf area (1/(C.d))
		//SG20140910: für AgMIP
		RGRL	=0.012;	   //Relative growth rate of leaf area (1/(C.d))
		vTbase 	=0.0;
	}

	//SG20170322: for all crops
	if(pPl->pGenotype->fMaxLfAppRate > (float) 0.001)
	{
		LA0  = pPl->pGenotype->fMaxLfInitRate/(float)10000.;
		RGRL = pPl->pGenotype->fMaxLfAppRate;
	}

	SLA = (float)1.0/pPl->pGenotype->fSpecLfWeight;


	vTemp =0.5*(double)(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin);
    vTemp = max(0.0, vTemp-vTbase);
    
	//Growth during maturation stage
	//SG 20110815 für Pareto
//	pPl->pCanopy->fLAGrowR=(float)0.5*SLA*pPl->pBiomass->fLeafGrowR;
	pPl->pCanopy->fLAGrowR=SLA*pPl->pBiomass->fLeafGrowR;

	//Growth during juvenile stage
	if ((pPl->pDevelop->fStageSUCROS<(float)0.3)&&(pPl->pCanopy->fLAI<(float)0.75))
	{
	    //pPl->pCanopy->fLAGrowR= pPl->pCanopy->fLAI
	    //							*(float)(exp(RGRL*(vTemp-vTbase))-1.0);
    ////SG20140913: für AgMIP ohne vTbase
	    pPl->pCanopy->fLAGrowR= pPl->pCanopy->fLAI
	    							*(float)(exp(RGRL*vTemp)-1.0);
	}
	
	//Growth at day of emergence
	if (pPl->pDevelop->fStageSUCROS<=(float)0.0)
		Emerg = 0;
	if (pPl->pDevelop->fStageSUCROS>(float)0.0)
		Emerg += 1;
	if (Emerg>100)
		Emerg=2;
	if (Emerg==1)
		pPl->pCanopy->fLAGrowR=pMa->pSowInfo->fPlantDens*LA0;
	
	//Growth before the day of emergence
	if (pPl->pDevelop->fStageSUCROS<(float)0.0)
	    pPl->pCanopy->fLAGrowR= (float)0.0;
	    
	return 1;
	}
	
int EarAreaGrowthRate(EXP_POINTER)
	{
	float EAR;
	EAR = (float)6.3E-5; 	//Ear area ratio
	
	if (pPl->pDevelop->fStageSUCROS<(float)0.8)
	    fEarAreaGrowR= (float)0.0;

	if ((pPl->pDevelop->fStageSUCROS>=(float)0.8)&&(fEAI==(float)0.0))
	    fEarAreaGrowR= EAR*pPl->pBiomass->fBiomassAbvGround;
	else
	    fEarAreaGrowR= (float)0.0;

	if (pPl->pDevelop->fStageSUCROS>=(float)1.3)
	    fEarAreaGrowR= -fLeafAgeDeathRate*fEAI;

	return 1;
	}	    								   
	
		
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Potential Evapotranspiration (modified Penman equation according to WANG, 1992)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*
float fPotEvapTransp,fRadPET,fDryPET;
float PotentialEvapTranspiration_Penman(EXP_POINTER)
	{
	int    iJulianDay;
	double vGlobalRadiation,vLongRad,vNetRad,vLatitude,vAltitude,vWind,vLAI;
	double vTempAve,vTempMin,vTempMax,vRH,vWaterCont1,vSatWaterCont1,cp;
	double vAlbedo,vAlbedoSoil,vLH,vP0,vP,vClear,vSlope,vTempAmp,vPET;
    double vBoltzm,vPsych,A,B,b,vWF,vSatVP,vActVP,vAT,vDryPET,vRadPET;

	iJulianDay			= (int)		pTi->pSimTime->iJulianDay;
	vLatitude			= (double)	pLo->pFarm->fLatitude;
	vAltitude			= (double) 	pLo->pFarm->fAltitude;

	vGlobalRadiation	= (double)	pCl->pWeather->fSolRad*1E6;
	vTempMax			= (double)	pCl->pWeather->fTempMax;
	vTempMin			= (double)	pCl->pWeather->fTempMin;
	vTempAve			= (vTempMax+vTempMin)/2.0;
	vRH					= (double)	pCl->pWeather->fHumidity; 
	vWind				= (double)	pCl->pWeather->fWindSpeed;
	
	if (vWind==-99.00)
		vWind=0.0;
		
	vLAI				= (double)	pPl->pCanopy->fLAI;
	vWaterCont1			= (double)	pWa->pWLayer->pNext->fContAct;
	vSatWaterCont1		= (double)	pSo->pSWater->pNext->fContSat;
	
	if (vSatWaterCont1==0.0)
		{
		vWaterCont1		=0.5;
		vSatWaterCont1	=1.0;
		}
		
	
	vBoltzm = 5.668E-8; //Stefan-Boltznan constant   (J /m2/d/K4)
	cp		= 1012.0; 	//specific heat of the air (J/kg/C)
	A		= 0.18;		//Constant in the Ångström formula 
	B		= 0.55; 	//Constant in the Ångström formula
	
	//Indicative values for empirical constants in the Ångström formula in relation
	//to latitude and climate used by the FAO (Grère & Popov, 1979)
	//-----------------------------------------------------------------------------
	//										 A		 	 B	
	//	Cold and temperate zones			0.18		0.55
	//	Dry tropical zones                  0.25		0.45
	//	Humid tropical zones				0.29		0.42
	//-----------------------------------------------------------------------------

	vLH	= (2.501-0.0024*vTempAve)*1.0E+6;	//2.4*1.0E+6; Latent heat of evaporation of water (J/kg H2O)

	vP0 = 101.3; 										// Pressure at sea level (kPa)
	vP  = vP0*exp(-0.034*vAltitude/(vTempAve+273.0));   // Pressure present (kPa)

	vPsych	= cp*vP/(0.622*vLH);		//0.67 Psychrometric instrument constant (kPa/C)	 
	
	Astronomy_SUCROS(iJulianDay,vLatitude);
    
    //Soil and Crop/soil albedo
    vAlbedoSoil	= 0.25*(1.0-0.5*vWaterCont1/vSatWaterCont1);
    vAlbedo 	= vAlbedoSoil*exp(-0.5*vLAI)+0.25*(1.0-exp(-0.5*vLAI));
     
	//Penman's original clearness factor
	vClear = (vGlobalRadiation/DS0-A)/B;	//vClear = n/N percent sunshine hours
	if (vClear<0.0) vClear=0.0;
	if (vClear>1.0) vClear=1.0;

	//Saturated and actural vapour pressure of the air (kPa)
	vSatVP = 0.1*6.11*exp(17.4*vTempAve/(vTempAve+239.0));
	vSlope = 4158.6*vSatVP/((vTempAve+239.0)*(vTempAve+239.0));
	vActVP = vSatVP*vRH/100.0;
	
	//Outgoing long wave and net radiation 
	vLongRad=vBoltzm*pow((vTempAve+273.0),4)*(0.56-0.079*sqrt(10*vActVP))*(0.1+0.9*vClear)*86400.0;
	vNetRad = (1.0-vAlbedo)*vGlobalRadiation-vLongRad;

	//Determine the wind modification coefficient b	and wind function vWF	
	vTempAmp = vTempMax-vTempMin;
	
    b=0.54+0.35*LIMIT(0.0, 1.0, (vTempAmp-12.0)/4.0);
    
	vWF = 6.4*1E+5/vLH*(1.0+b*vWind);
	
	//Dry power term
	vAT = vWF*(vSatVP-vActVP);
	
	//Potential evapotranpiration
	//vPET =(vW*vNetRad + vAT) / (vW+1.0);    		
	vRadPET = (1.0/vLH)*(vSlope/(vSlope+vPsych))*vNetRad;
	vDryPET = vAT/(vSlope+vPsych);
	
	vPET    = vRadPET + vDryPET;
	
	fRadPET = (float)vRadPET;
	fDryPET = (float)vDryPET;

	fPotEvapTransp = (float) vPET;
	
	return (float)vPET;
	}

  */

//################################################################################################
//	Soil Water Balance Part
//################################################################################################

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Definitions
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 1. Saturated Water Content: is equal to soil porosity.
// 2. Field Capacity: is the volumetric water content of the soil after wetting and initial
//			(1-3days) redistribution (Veihmeyer & Hendrickson, 1931)
//			Field capacity is usually defined as the volumetric water content at a soil 
//			moisture suction of 100 mbar or pF 2.0
// 3. Wilting Point: As the soil dries out, it becomes increasingly diffficult for plants 
//			to extract water. At high soil water suctions ( the actual value depending on 
//			environmental conditions), plants may wilt during the day and recover at night 
//			when evaporative demand is low. Above a certain value of moisture suction, 
//			plants do not recover at night and wilt permanently. The soil moisture suction 
//			then usually has a value of about 16,000 mbar or pF 4.2; the value varies among 
//			plant species. The volumetric wate content at this suction value is called the 
//			permanent wilting point of the soil. Its value depends strongly on soil type.
// 4. Air Dry Water Content: the soil water content when air dry is one third or less of 
//			that at wilting point. The soil moisture suction of an air ddry soil is
//			assumed to be 10000000 mbar or pF 7.0 (van Keulen, 1975).
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Canopy Water Interception
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds CanopyWaterInterception_SUCROS(EXP_POINTER)
	{
	float fInterceptionCapacity;	//The interception capacity per layer of leaves (mm/d)
	float fInterception;
	
	fInterceptionCapacity = (float)0.25;
	
	fInterception = fInterceptionCapacity*pPl->pCanopy->fLAI;
	
	if (fInterception>pCl->pWeather->fRainAmount)
		fInterception=pCl->pWeather->fRainAmount;
		
	pPl->pPltWater->fInterceptDay=fInterception;
	
	return 1;
	}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Root depth 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds RootSystemFormation_SUCROS(EXP_POINTER)
	{
	int 	i;
	PSLAYER pSL	=pSo->pSLayer->pNext;
	PSWATER	pSWL=pSo->pSWater->pNext;
	PWLAYER	pSLW=pWa->pWLayer->pNext;
    PLAYERROOT pLR=pPl->pRoot->pLayerRoot;
    float 	fRootElongationRate,fMaxRootDepthSoil,fMaxRootDepthCrop,fMaxSoilDepth;
	float	fSoilDepth,fMaxRootDepth,fElongationRate;
    float   fSpecRootWeight,fRootWeight30,fRootWeight60,fRootWeight90,fRootWeight;
    float   fDTT,fDeltaTemp,fTempCR,fTempCor;
	float	fNewRootLength, fTotalRootLength, TRLDF;
	
	//Constant Coefficient
	fRootElongationRate = (float)12.0;		//mm/d for spring wheat (van Keulen & Seligman, 1987)

	fMaxRootDepthSoil	= (float)1200.0; 	//mm depends on soil property
	fMaxRootDepthCrop   = (float)1200.0; 	//mm depends on crop property

	//=========================================================================================
	//      Werte aus WAVE 2.0 Reference Manual 5-25 Tabelle 5.17 
	//=========================================================================================
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
	{
		fSpecRootWeight=(float)10500;
        fRootWeight30=(float)0.65;
        fRootWeight60=(float)0.25;
        fRootWeight90=(float)0.08;
		//fRootElongationRate = (float)18.0;
  //      fMaxRootDepthCrop=(float)1800;//950;
		
		//SG20140909
		fRootElongationRate = pPl->pGenotype->fMaxRootExtRate*(float)10.0; //mm
        fMaxRootDepthCrop=pPl->pGenotype->fMaxRootDepth*(float)10.0;//mm;
	}
	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI")) 	
		||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")))
	{
		fSpecRootWeight=(float)10100;
		//SG20140909
		//fSpecRootWeight=pPl->pGenotype->fRootLengthRatio;
        fRootWeight30=(float)0.65; //0.65
        fRootWeight60=(float)0.25; //0.25
        fRootWeight90=(float)0.08; //0.08
		//fRootElongationRate = (float)60.0;
       //fMaxRootDepthCrop=(float)1500;//105;
		
		//SG20140909
		fRootElongationRate = pPl->pGenotype->fMaxRootExtRate*(float)10.0; //mm
        fMaxRootDepthCrop=pPl->pGenotype->fMaxRootDepth*(float)10.0;//mm;
	}
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
	{	
		fSpecRootWeight=(float)16000;
        fRootWeight30=(float)0.64;
        fRootWeight60=(float)0.29;
        fRootWeight90=(float)0.05;
		//fRootElongationRate = (float)30.0;
  //      fMaxRootDepthCrop=(float)1500;//750;
		
		//SG20140909
		fRootElongationRate = pPl->pGenotype->fMaxRootExtRate*(float)10.0; //mm
		fMaxRootDepthCrop=pPl->pGenotype->fMaxRootDepth*(float)10.0;//mm;
	}
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
	{
		fSpecRootWeight=(float)23400;
        fRootWeight30=(float)0.65;
        fRootWeight60=(float)0.29;
        fRootWeight90=(float)0.04;
		//fRootElongationRate = (float)14.0;
  //      fMaxRootDepthCrop=(float)900;//650;
	
		//SG20140909
		fRootElongationRate = pPl->pGenotype->fMaxRootExtRate*(float)10.0; //mm
        fMaxRootDepthCrop=pPl->pGenotype->fMaxRootDepth*(float)10.0;//mm;
	}
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))
	{
		fSpecRootWeight=(float)34600;
        fRootWeight30=(float)0.81;
        fRootWeight60=(float)0.19;
        fRootWeight90=(float)0.03;
		//fRootElongationRate = (float)20.0;
  //      fMaxRootDepthCrop=(float)1200;//660;
		
		//SG20140909
		fRootElongationRate = pPl->pGenotype->fMaxRootExtRate*(float)10.0; //mm
        fMaxRootDepthCrop=pPl->pGenotype->fMaxRootDepth*(float)10.0;//mm;
	}

	//SG 20110815: für Pareto-Analyse (und AgMIP)
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
	{
		fSpecRootWeight=pPl->pGenotype->fRootLengthRatio;
        fRootWeight30=(float)0.65;
        fRootWeight60=(float)0.25;
        fRootWeight90=(float)0.08;
		fRootElongationRate = pPl->pGenotype->fMaxRootExtRate*(float)10.0; //mm
        fMaxRootDepthCrop=pPl->pGenotype->fMaxRootDepth*(float)10.0;//mm;
	}



	//Maximum soil depth (mm)
	fMaxSoilDepth = (float)0.0;
	for (i=1;i<=pSo->iLayers-2-1;i++)
		{	
	    fMaxSoilDepth += (float)pSL->fThickness;
	    
	    pSL =pSL->pNext; 
	    }
	    
	//Maximum Root Depth (mm)
	fMaxRootDepth = fMaxRootDepthCrop;
	
	if (fMaxRootDepth>fMaxRootDepthSoil)
		fMaxRootDepth=fMaxRootDepthSoil;

	if (fMaxRootDepth>fMaxSoilDepth)
		fMaxRootDepth=fMaxSoilDepth;
		
	pPl->pModelParam->fMaxRootDepth = fMaxRootDepth;

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))
	{//Elongation Rate of Roots (mm/day) 
	fElongationRate = fRootElongationRate;
	}
	else
	{
    ///*
    //fRootLengthNew = pPl->pBiomass->fTotRootWeight*fSpecRootWeight*(float)1000;
	fDeltaTemp=(pCl->pWeather->fTempMax - pCl->pWeather->fTempMin)/(float)2;
	fTempCR=(pCl->pWeather->fTempMax + pCl->pWeather->fTempMin)/(float)2;
    fDTT=fTempCR;
    if (pCl->pWeather->fTempMax < (float)0)
    {    
        fDTT=(float)0;
    }
    else
    {
        if (pCl->pWeather->fTempMin < (float)0)
        {    
            fTempCor=(float)max((float)0,(float)0.3166*fDeltaTemp-(float)0.4043*fabs(fTempCR));
            fDTT=fTempCR+fTempCor;
        }
    }

    //SG 20110815: für Pareto-Analyse
	fElongationRate=(float)min(fDTT*(float)0.22,fRootElongationRate/(float)10.0)*(float)10;//mm !!!
	//fElongationRate=(float)min(fDTT*(float)0.22,(float)1.8)*(float)10;//mm !!!
    //*/
	}

    if (pPl->pDevelop->fStageSUCROS==(float)0.0)
		fElongationRate = (float)0.0;
    
	if ((pPl->pDevelop->fStageSUCROS>(float)1.0)||(pPl->pRoot->fDepth>fMaxRootDepth))
		fElongationRate = (float)0.0;
   

				
	//Water Limit for Elongation
	if(pPl->pBiomass->fTotRootWeight>(float)0.0)
		{
			pSL	=pSo->pSLayer->pNext;
			pSWL=pSo->pSWater->pNext;
			pSLW=pWa->pWLayer->pNext; 
			pLR=pPl->pRoot->pLayerRoot;

			fTotalRootLength = fSpecRootWeight*pPl->pBiomass->fTotRootWeight;
			fNewRootLength = fSpecRootWeight*pPl->pBiomass->fRootGrowR;
			TRLDF = (float)0.0;

			fSoilDepth = (float)0.0;
			for (i=1;i<=pSo->iLayers-2;i++)
				{	
				fSoilDepth += pSL->fThickness;

				if(fSoilDepth<=(float)300) fRootWeight=fRootWeight30;
				if((fSoilDepth>(float)300)&&(fSoilDepth<=(float)600)) fRootWeight=fRootWeight60;
				if((fSoilDepth>(float)600)&&(fSoilDepth<=(float)900)) fRootWeight=fRootWeight90;
				if((fSoilDepth>(float)900)&&(fSoilDepth<=(float)1200)) fRootWeight=(float)0.02;//0.01;
				if(fSoilDepth>(float)1200) fRootWeight=(float)0.01;//0;

				pLR->fLengthDensFac = fRootWeight*fNewRootLength/((float)1e4*pSL->fThickness);
				
				//pLR->fLengthDens = fSpecRootWeight*fRootWeight*pPl->pBiomass->fTotRootWeight
				//				/((float)1e4*pSL->fThickness);
				//[cm/g kg/ha mm^-1] = [cm/kg 10^3 kg/ha 10 cm^-1] = [cm cm^-1 m^-2] =
				//[cm cm^-3 10^-4]

				//TRLDF += pLR->fLengthDens*((float)1e4*pSL->fThickness)/(fTotalRootLength+(float)1e-9);
				TRLDF += fRootWeight;
		           
				//pLR->fLengthDens=min((float)5.0, max((float)0.0,pLR->fLengthDens));

				//pLR->fLengthDens += pLR->fLengthDensFac*fRootLengthNew/((float)1e8*pSL->fThickness);

				if (fSoilDepth>=pPl->pRoot->fDepth)
					break;
				
				if (i<pSo->iLayers-2)				    
					{
	    			pSL=pSL->pNext; 
	    			pSLW=pSLW->pNext;
	    			pSWL=pSWL->pNext;
					pLR=pLR->pNext;
	    			} 
				}
	    
				fSoilDepth = (float)0.0;
				pLR=pPl->pRoot->pLayerRoot;
				for (i=1;i<=pSo->iLayers-2;i++)
				{	
					fSoilDepth += pSL->fThickness;

					pLR->fLengthDens += pLR->fLengthDensFac/TRLDF;
					pLR=pLR->pNext;

					if (fSoilDepth>=pPl->pRoot->fDepth)
					break;
				}
				
		} //end fTotRootWeight>0.0


    /*
    //pSL =pSL->pBack;
    //pLR=pPl->pRoot->pLayerRoot->pBack;
    pLR->fLengthDens *= ((float)1-(fSoilDepth-pPl->pRoot->fDepth)/pSL->fThickness);
    */
    
	if (pSLW->fContAct<pSWL->fContPWP)
		fElongationRate = (float)0.0;
    
    pPl->pRoot->fDepthGrowR = fElongationRate;
	 
	//Root Depth 
	pPl->pRoot->fDepth += fElongationRate;

	//SG20140909
	//pPl->pRoot->fDepth = min(pPl->pRoot->fDepth,pPl->pGenotype->fMaxRootDepth*10);	

    return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Potential Plant Transpiration 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds PotentialTranspiration_SUCROS(EXP_POINTER)
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


/*int WINAPI _loadds PotentialTranspiration_SUCROS_Alt(EXP_POINTER)
	{          
	PPLTWATER 	pPltW;
	
	pPltW =pPl->pPltWater;

	PotentialEvapTranspiration_Penman(exp_p);

	 
	pPltW->fPotTranspdt = fRadPET*(float)(1.0-exp(-k*(double)pPl->pCanopy->fLAI))
						+ fDryPET*min(pPl->pCanopy->fLAI, (float)2.0)
						- (float)0.5*pPltW->fInterceptDay;
						
	if (pPltW->fPotTranspdt>fPotEvapTransp)
		pPltW->fPotTranspdt=fPotEvapTransp;
		
	return 1;
	}
	*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Actual Plant Transpiration 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

float fTranspLoss[MAXSCHICHT],fUptakeAvailability[MAXSCHICHT],fWaterAmount[MAXSCHICHT];
float RootActivityCoefficient_SUCROS(float fRelWaterCont);
float SoilWaterAvailabilityEffect_SUCROS(EXP_POINTER);

int WINAPI _loadds ActualTranspiration_SUCROS0(EXP_POINTER)
	{          
	int 	i;
	float 	fRelWaterCont,fCumEffectRtLens,fActTransp,fCumDepth,fAvePotUnitUptake;
    float	fRootActCoeff[MAXSCHICHT],fRtLayThick[MAXSCHICHT];
	PSLAYER pSL	=pSo->pSLayer->pNext;
	PSWATER	pSWL=pSo->pSWater->pNext;
	PWLAYER	pSLW=pWa->pWLayer->pNext; 

	//Root activity coefficient and rooted layer depth	
	fCumDepth 			= (float)0.0;	
	fCumEffectRtLens 	= (float)0.0;	

	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	for (i=1; i<=pSo->iLayers-2; i++)
		{         
		fRelWaterCont= (pSLW->fContAct-pSWL->fContPWP)
					  /(pSWL->fContFK-pSWL->fContPWP);

        fRootActCoeff[i]=RootActivityCoefficient_SUCROS(fRelWaterCont);
        
		if (pPl->pRoot->fDepth<=fCumDepth)
			fRtLayThick[i]=(float)0.0;
		else
			fRtLayThick[i]=pSL->fThickness;
			
		fCumEffectRtLens += fRtLayThick[i] * fRootActCoeff[i];
		 
        fCumDepth += pSL->fThickness;
		
		pSL =pSL ->pNext;
		pSLW=pSLW->pNext;
		pSWL=pSWL->pNext;
		}
				        
	//Average potential rate of water uptake per mm effective rooted depth (mm-1)
    fAvePotUnitUptake = fPotTraDay/(fCumEffectRtLens+(float)1.0E-10);
    
	//Soil Water Availability Effect for root water uptake
	SoilWaterAvailabilityEffect_SUCROS(exp_p);

	//Root water uptake in each soil layer
	fActTransp = (float)0.0;
	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	for (i=1; i<=pSo->iLayers-2; i++)
		{
		fTranspLoss[i] = fAvePotUnitUptake*fUptakeAvailability[i]
							*fRtLayThick[i]*fRootActCoeff[i];
	
		fWaterAmount[i]  -= fTranspLoss[i];
		pSLW->fContAct -= fTranspLoss[i]/pSL->fThickness;
		
		fActTransp += fTranspLoss[i];
		
		pSL =pSL ->pNext;
		pSLW=pSLW->pNext;
		pSWL=pSWL->pNext;
		}

	//Plant actual transpiration
	pPl->pPltWater->fActTranspdt = fActTransp;

	return 1;
	}


int WINAPI _loadds ActualTranspiration_SUCROS(EXP_POINTER)
	{          
	int 	i;
	float 	fRelWaterCont,fCumEffectRtLens,fActTransp,fCumDepth,fAvePotUnitUptake,fContAct;
    float	fRootActCoeff[MAXSCHICHT],fRtLayThick[MAXSCHICHT];
	PSLAYER pSL	=pSo->pSLayer->pNext;
	PSWATER	pSWL=pSo->pSWater->pNext;
	PWLAYER	pSLW=pWa->pWLayer->pNext; 
    PLAYERROOT pLR=pPl->pRoot->pLayerRoot;

	//Root activity coefficient and rooted layer depth	
	fCumDepth 			= (float)0.0;	
	fCumEffectRtLens 	= (float)0.0;	

	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext;
	for (i=1; i<=pSo->iLayers-2; i++)
		{         
		fWaterAmount[i] = pSLW->fContAct*pSL->fThickness;
		
		fRelWaterCont= (pSLW->fContAct-pSWL->fContPWP)
					  /(pSWL->fContFK-pSWL->fContPWP);

        fRootActCoeff[i]=RootActivityCoefficient_SUCROS(fRelWaterCont);
        
		if (pPl->pRoot->fDepth<=fCumDepth)
			fRtLayThick[i]=(float)0.0;
		else
			fRtLayThick[i]=pSL->fThickness;
			
		fCumEffectRtLens += fRtLayThick[i] * fRootActCoeff[i];
		 
        fCumDepth += pSL->fThickness;
		
		pSL =pSL ->pNext;
		pSLW=pSLW->pNext;
		pSWL=pSWL->pNext;
		}
				        
	//Average potential rate of water uptake per mm effective rooted depth (mm-1)
    fAvePotUnitUptake = fPotTraDay/(fCumEffectRtLens+(float)1.0E-10);
    
	//Soil Water Availability Effect for root water uptake
	SoilWaterAvailabilityEffect_SUCROS(exp_p);

	//Root water uptake in each soil layer
	fActTransp = (float)0.0;
	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
    pLR=pPl->pRoot->pLayerRoot;
	for (i=1; i<=pSo->iLayers-2; i++)
		{
		fTranspLoss[i] = fAvePotUnitUptake*fUptakeAvailability[i]
							*fRtLayThick[i]*fRootActCoeff[i];
	
		////SG20140914: Abfangen negativer Wassergehalte
		if (fTranspLoss[i]/pSL->fThickness >= pSLW->fContAct-pSWL->fContPWP)
			fTranspLoss[i] = (pSLW->fContAct-pSWL->fContPWP)*pSL->fThickness;
	
		fWaterAmount[i]  -= fTranspLoss[i];
		//pSLW->fContAct -= fTranspLoss[i]/pSL->fThickness;

		fContAct  = pSLW->fContAct;
		fContAct -= fTranspLoss[i]/pSL->fThickness; //mm/mm

	    pLR->fActWatUpt=fTranspLoss[i];
        pLR->fActLayWatUpt=fTranspLoss[i];

		/*
		  if (fContAct<pSWL->fContPWP)
			{
			pLR->fActWatUpt	   *= fContAct/pSWL->fContPWP;
			pLR->fActLayWatUpt *= fContAct/pSWL->fContPWP;
			}
        */
		
		//fActTransp += fTranspLoss[i];
		fActTransp += pLR->fActLayWatUpt;
		
		pSL =pSL ->pNext;
		pSLW=pSLW->pNext;
		pSWL=pSWL->pNext;
        pLR=pLR->pNext;
		}

	//Plant actual transpiration
	//pPl->pPltWater->fActTranspdt = fActTransp;
	  fActTraDay = fActTransp;

	  pPl->pRoot->fUptakeR = pPl->pPltWater->fActUptakedt = fActTransp;    

	
	// SG 25.03.2003
	//----------------------------------------------------------------------------------
	if (fPotTraDay<=(float)0.001)
   		{
    	pPl->pPltWater->fStressFacPhoto 	= (float)1.0;
 		return 0;
		}
	
	pPl->pPltWater->fStressFacPhoto = fActTraDay/fPotTraDay;
	pPl->pPltWater->fStressFacPartition = min((float)1.0,(float)0.5+pPl->pPltWater->fStressFacPhoto);

	//----------------------------------------------------------------------------------
	

	return 1;
	}
	
//=============================================================================================
//	Root activity coefficient according to van Keulen & Seligman, 1987)
//=============================================================================================
float RootActivityCoefficient_SUCROS(float fRelWaterCont)
	{
	double vCoeff,xx;
	//------------------------------------------------------------------------------------
	//			fRelWaterCont			Root Activity Coeff.
	//------------------------------------------------------------------------------------
	//				-0.10						0.0	
	//				-0.05						0.0
	//				 0.0						0.15
	//				 0.15						0.6
	//				 0.30						0.8
	//				 0.50						1.0
	//				 1.10						1.0
	//------------------------------------------------------------------------------------
	//The following regression with the above data is R2=0.9933

	xx = (double)fRelWaterCont;
	
	//SG 20121026:
	// something is wrong with the regression:
	// e.g. >48% reduction of water uptake at field capacity!!!
	// --> far to much water stress

	//vCoeff	=	 0.0010*pow(xx,5)
	//			+0.0188*pow(xx,4)
	//			+0.1031*pow(xx,3)
	//			+0.1271*pow(xx,2)
	//			+0.1125*xx
	//			+0.1571;

	//SG 20121026:
	// simple linear approach

	if (xx < -0.05)
		vCoeff = 0.0;
	else if (xx <= 0.5)
		vCoeff = (xx+0.05)/0.55*xx;
	else
		vCoeff = 1.0;

		
	//SG 20121026:
	// interpolation of table from Sucros97 documentation (see comment above)
	//if (xx < -0.05)
	//	vCoeff = 0.0;
	//else if (xx <= 0.0)
	//	vCoeff = 0.15*(xx+0.05)/0.05;
	//else if (xx <= 0.15)
	//	vCoeff = (0.6*xx-0.15*(xx-0.15))/0.15;
	//else if (xx <= 0.30)
	//	vCoeff = (0.8*(xx-0.15)-0.6*(xx-0.3))/0.15;
	//else if (xx <= 0.5)
	//	vCoeff = ((xx-0.3)-0.8*(xx-0.5))/0.2;
	//else
	//	vCoeff = 1.0;


	return (float)vCoeff;
	}
					 	
//=============================================================================================
//	Soil Water Availability Effect for root water uptake
//=============================================================================================
float SoilWaterAvailabilityEffect_SUCROS(EXP_POINTER)
	{          
	int		i;
	float	fP,fCharPotTransp,fWaterloggCont[MAXSCHICHT], fCritWaterCont;
	PSLAYER pSL	=pSo->pSLayer->pNext;
	PSWATER	pSWL=pSo->pSWater->pNext;
	PWLAYER	pSLW=pWa->pWLayer->pNext; 
	
	//Table 2.2 Characteristic potential transpiration rates for five crop groups according
	//Driessen (1986). (Source: Doorenbos et al., 1978)
	//------------------------------------------------------------------------------------
	//	Crop Group		Ts (mm/d)			Crops (example)
	//------------------------------------------------------------------------------------
	//		1				1.8				Leaf vegetables
	//		2				3.0				Clover, carrot
	//		3				4.5				Pea, potato
	//		4				6.0				Groundnut
	//		5				9.0				Most grains, soybean
	//------------------------------------------------------------------------------------

	fCharPotTransp = (float)8.0;

	//§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§	
	//	This part is assumed by WANG, Enli, 23.07.1995
	//§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§	
	//Sensitivity Coefficient for waterlogging (fWaterloggCont (%))
	for (i=1; i<=pSo->iLayers-2; i++)
		{
		fWaterloggCont[i]= pSWL->fContFK + 
						(float)(3.0/4.0)*(pSWL->fContSat-pSWL->fContFK);
		pSWL=pSWL->pNext;
		}
	//§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§§	

	//Soil water depletion fraction
	if (fPotTraDay==(float)0.0)
		fPotTraDay=(float)0.001;
		
	fP = fCharPotTransp/(fCharPotTransp+fPotTraDay);


	//Soil water availability for water uptake
	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	for (i=1; i<=pSo->iLayers-2; i++)
		{
		fCritWaterCont=pSWL->fContPWP+((float)1.0-fP)*(pSWL->fContFK-pSWL->fContPWP);
	
		if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
			fCritWaterCont=pSWL->fContPWP+(float)0.1*(pSWL->fContFK-pSWL->fContPWP);
			//SG20140914: früheres Einsetzen des Wasserstress (bei sehr niedrigem PWP sonst Abstürze!)
			//fCritWaterCont=pSWL->fContPWP+(float)0.2*(pSWL->fContFK-pSWL->fContPWP);

		
		if (pSLW->fContAct>fWaterloggCont[i])
			fUptakeAvailability[i] = (pSWL->fContSat-pSLW->fContAct)
										/(pSWL->fContSat-fWaterloggCont[i]);
		else
			{
			if (pSLW->fContAct>fCritWaterCont)
				fUptakeAvailability[i] = (float)1.0;
			else
				fUptakeAvailability[i] = (pSLW->fContAct-pSWL->fContPWP)
											/(fCritWaterCont-pSWL->fContPWP);
			}

		if (fUptakeAvailability[i]<(float)0.0)
			fUptakeAvailability[i]=(float)0.0;
	
		if (fUptakeAvailability[i]>(float)1.0)
			fUptakeAvailability[i]=(float)1.0;

		pSL =pSL ->pNext;
		pSLW=pSLW->pNext;
		pSWL=pSWL->pNext;
		}
		
	return fP;
	}
	

/*
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Potential Soil Evaporation 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PotentialEvaporation_SUCROS(EXP_POINTER)
	{          
	lpSoil->lpWater->fPotEvap =
			fPotEvapTransp*(float)exp(-k*pPl->pCanopy->fTotalLAI);
	
	return 1;
	}
	

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Actual Soil Evaporation 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds ActualEvaporation_SUCROS(EXP_POINTER)
	{          
	LPSOILLAYER			pSL;
	LPSOILLAYERWATER	pSLW;
	
	int 				i;
	float				fAirDryWaterCont[MAXSCHICHT],fPartitionParam[MAXSCHICHT];
	float				fTotEvapWater,fDepth,fTotPartitionParam,fLayThick;
	double				vEES,vDepth;
	
	static int 			iDaysSinceRain=1;

	//The air dry water content in each layer
	pSL = lpSoil->lpType->lpLayer;
	pSLW= lpSoil->lpWater->lpLayerWater;
	for (i=1; i<=pSo->iLayers-2; i++)
		{
		fWaterAmount[i] = pSLW->fContAct*(pSL->fThickness;
        fAirDryWaterCont[i]=(float)(1.0/3.0)*pSWL->fContPWP;

		pSL =pSL ->pNext;
		pSLW=pSLW->pNext;
		}
	
	//The number of days since the last rainfall
	if (pCl->pWeather->fRainAmount>=(float)0.5)
		iDaysSinceRain=1;
	else
		iDaysSinceRain += 1;

	//Actual Soil Evaporation
	pSL = lpSoil->lpType->lpLayer;
	pSLW= lpSoil->lpWater->lpLayerWater;

	if (iDaysSinceRain==1)                               
		fTotEvapWater =   fWaterAmount[i]
						- fAirDryWaterCont[1]*pSL->fThickness*(float)10.0;
	else
		fTotEvapWater = (float)0.6*lpSoil->lpWater->fPotEvap
					   *(float)(sqrt((double)iDaysSinceRain)-sqrt((double)iDaysSinceRain-1.0))
					   +lpSoil->lpWater->fPrecipInfilt;
											  							

	lpSoil->lpWater->fActEvap=min(lpSoil->lpWater->fPotEvap, fTotEvapWater);

	//=======================================================================================
	//Extraction of soil water from soil layers
	//=======================================================================================
	vEES = 0.002;// soil-specific extinction coefficient (1/mm) (van Keulen, 1975)

	//Partition parameter in each soil layer	
	vDepth = 0.0;
	fTotPartitionParam = (float)0.0;
	for (i=1; i<=pSo->iLayers-2; i++)
		{
		fDepth  = (pSL->fThickness;
		vDepth +=0.25*(double)pSL->fThickness*10.0;
		
    	fPartitionParam[i] = max((float)0.1,fWaterAmount[i]-fAirDryWaterCont[i]*fDepth)
    	 				    *(float)exp(-vEES*(0.25*vDepth));
		
		vDepth +=(double)pSL->fThickness*10.0-0.25*(double)pSL->fThickness*10.0;
		
		fTotPartitionParam += fPartitionParam[i];
		
		pSL =pSL ->pNext;
		pSLW=pSLW->pNext;
		}

	//Extraction of soil water from soil layers
	pSL = lpSoil->lpType->lpLayer;
	pSLW= lpSoil->lpWater->lpLayerWater;
	for (i=1; i<=pSo->iLayers-2; i++)
		{
    	pSLW->fEvapLoss = lpSoil->lpWater->fActEvap*fPartitionParam[i]/fTotPartitionParam;

		fLayThick = (pSL->fThickness;	//From cm to mm

		fWaterAmount[i]  -= pSLW->fEvapLoss;
		pSLW->fContAct -= pSLW->fEvapLoss/fLayThick;
		

   		pSL =pSL ->pNext;
   		pSLW=pSLW->pNext;
		}

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantWaterStress_SUCROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantWaterStress_SUCROS(EXP_POINTER)
	{  
	pPl->pPltWater->fSWDF1		= pPl->pPltWater->fActTransp/(pPl->pPltWater->fPotTranspdt+(float)1.0E-10);

	pPl->pPltWater->fWsPartition = min((float)1.0,(float)0.5+pPl->pPltWater->fSWDF1);
	
	return 1;
	}
	
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantSenescense_SUCROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantSenescense_SUCROS(EXP_POINTER)
	{   

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	BiomassAccumulation_NWHEAT
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds BiomassAccumulation_SUCROS(EXP_POINTER)
	{    
	return 1;    
	}
*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PotentialNitrogenUptake_WAVE
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PotentialNitrogenUptake_WAVE(EXP_POINTER)
	{
    float f1,fndef,fred,fGrainMaxConc;
	PBIOMASS		pBiom = pPl->pBiomass;
    PPLTNITROGEN	pPltN = pPl->pPltNitrogen;

    //SG 20122410: lower fGrainMaxConc for maize and potato
	if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"SB")) fGrainMaxConc=(float)0.01;
	else if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"MZ")) fGrainMaxConc=(float)0.015;
	else if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"PT")) fGrainMaxConc=(float)0.015;
	else fGrainMaxConc=(float)0.021; //WH: 0.025
    //fGrainMaxConc=(float)0.025;
	
	NitrogenConcentrationLimits_SPASS(exp_p);
    //pPltN->fLeafMaxConc = (float)0.1;
    //pPltN->fLeafOptConc = (float) 0.5 * pPltN->fLeafMaxConc;
    //pPltN->fLeafMinConc = (float) 0.005;
    
    f1=(pPltN->fLeafActConc-pPltN->fLeafMinConc)/(pPltN->fLeafOptConc-pPltN->fLeafMinConc);
    fred  = min((float)1,max((float)0,f1));
    fndef = (float)1 - (float)sqrt((float)1-fred*fred);
    
	pPltN->fLeafDemand = max((float)0,pBiom->fLeafWeight*pPltN->fLeafMaxConc-pPltN->fLeafCont);
    pPltN->fStemDemand = max((float)0,pBiom->fStemWeight*pPltN->fStemMaxConc-pPltN->fStemCont);
    pPltN->fRootDemand = max((float)0,pBiom->fRootWeight*pPltN->fRootMaxConc-pPltN->fRootCont);
	
	pPltN->fShootDemand= pPltN->fLeafDemand+pPltN->fStemDemand;
    //pPltN->fGrainDemand= min(max((float)0,pBiom->fGrainWeight*fGrainMaxConc-pPltN->fGrainCont)
    //                         *fndef,pPltN->fShootDemand);
    pPltN->fGrainDemand= max((float)0,pBiom->fGrainWeight*fGrainMaxConc-pPltN->fGrainCont)*fndef;

    pPltN->fTotalDemand= pPltN->fLeafDemand+pPltN->fStemDemand+pPltN->fRootDemand;  
    //pPltN->fTotalDemand= pPltN->fLeafDemand+pPltN->fStemDemand+pPltN->fRootDemand+pPltN->fGrainDemand;
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	ActualNitrogenUptake_WAVE
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds ActualNitrogenUptake_WAVE(EXP_POINTER)
	{
    int L;
    float fnh4, fno3, fnconvup, fnh4up, fno3up, fnpotdiffup, fndiffup;
    float fnh4r=(float)1.8; 
	float fno3r=(float)6.4;//1.8,6.4;
    float f1,fRDO,fMeanRootRadius,fThetaMid,fLiqDiffCoeff;
    float fnh4diffup[MAXSCHICHT],fno3diffup[MAXSCHICHT];

    PPLTNITROGEN	pPltN = pPl->pPltNitrogen;
	PLAYERROOT		pLR	  = pPl->pRoot->pLayerRoot;
    PSLAYER 		pSL	  = pSo->pSLayer->pNext;
	PWLAYER	   		pSLW  = pWa->pWLayer->pNext; 
	PCLAYER     	pSLN  = pCh->pCLayer->pNext;

    fRDO=(float)0.1;
    fMeanRootRadius=(float)0.22;

    pPltN->fTotalDemand *= (float)100;

    if (pPltN->fTotalDemand<=(float)0)
    {
     for (L=1;L<=pSo->iLayers-2;L++)
	    {
 		 pLR->fActLayNO3NUpt = (float)0;
		 pLR->fActLayNH4NUpt = (float)0;

         pSL =pSL ->pNext;
    	 pLR =pLR ->pNext;
        }
     
 	 pPltN->fActNO3NUpt = (float)0;
	 pPltN->fActNH4NUpt = (float)0;
     pPltN->fActNUptR = (float)0;
    }
    else
    {
     fnconvup=(float)0;
 	 pPltN->fActNO3NUpt = (float)0;
	 pPltN->fActNH4NUpt = (float)0;

     pPltN = pPl->pPltNitrogen;
     pLR   = pPl->pRoot->pLayerRoot;
     pSL   = pSo->pSLayer->pNext;
     pSLW  = pWa->pWLayer->pNext; 
     pSLN  = pCh->pCLayer->pNext;
     for (L=1;L<=pSo->iLayers-2;L++)
	 {
         fnh4 = max((float)0,pSLN->fNH4NSoilConc - fnh4r);
		 fno3 = max((float)0,pSLN->fNO3NSoilConc - fno3r);
         //fnh4up = pLR->fActWatUpt*fnh4*pSLW->fContAct/(float)100;
         //fno3up = pLR->fActWatUpt*fno3*pSLW->fContAct/(float)100;
         fnh4up = pLR->fActWatUpt*fnh4/(float)100;
         fno3up = pLR->fActWatUpt*fno3/(float)100;
         //[l/m^2] [mg/l] = [mg/m^2]
         
         pLR->fActLayNH4NUpt = fnh4up;
         pLR->fActLayNO3NUpt = fno3up; 
	     pPltN->fActNH4NUpt += fnh4up;
     	 pPltN->fActNO3NUpt += fno3up;

         pSL =pSL->pNext;
         pSLW=pSLW->pNext;
         pSLN=pSLN->pNext;
    	 pLR =pLR->pNext;
      }//for
     
     fnconvup=(pPltN->fActNH4NUpt+pPltN->fActNO3NUpt)*(float)100;

     if (fnconvup >= pPltN->fTotalDemand)
     {
       pPltN = pPl->pPltNitrogen;
       pLR   = pPl->pRoot->pLayerRoot;
       pSL   = pSo->pSLayer->pNext;
       pSLW  = pWa->pWLayer->pNext; 
       pSLN  = pCh->pCLayer->pNext;
       for (L=1;L<=pSo->iLayers-2;L++)
       {
		 pLR->fActLayNH4NUpt *= pPltN->fTotalDemand/fnconvup;
         pLR->fActLayNO3NUpt *= pPltN->fTotalDemand/fnconvup; 
  
         pSL =pSL->pNext;
         pSLW=pSLW->pNext;
         pSLN=pSLN->pNext;
    	 pLR =pLR->pNext;
       }//for
       pPltN->fActNH4NUpt *= pPltN->fTotalDemand/fnconvup;
       pPltN->fActNO3NUpt *= pPltN->fTotalDemand/fnconvup;
     }
     else
     {
      fnpotdiffup = pPltN->fTotalDemand - fnconvup;
      fndiffup = (float)0;
 
      Get_Diffusion_Const(exp_p);
 
      pPltN = pPl->pPltNitrogen;
      pLR   = pPl->pRoot->pLayerRoot;
      pSL   = pSo->pSLayer->pNext;
      pSLW  = pWa->pWLayer->pNext; 
      pSLN  = pCh->pCLayer->pNext;
      for (L=1;L<=pSo->iLayers-2;L++)
      {
         fThetaMid = (pSLW->pNext->fContAct+pSLW->pNext->fContOld+pSLW->fContAct+ pSLW->fContOld)/(float)4;
		 fLiqDiffCoeff=(pCh->pCParam->fMolDiffCoef * pSL->fImpedLiqA 
		 	            * (float)exp((double)(pSL->fImpedLiqB * pSLW->fContAct)))/(float)100;
		 	            //* (float)exp((double)(pSL->fImpedLiqB * fThetaMid)))/(float)100;
		 fLiqDiffCoeff=pCh->pCParam->fMolDiffCoef*pSLW->fContAct/(float)100;
		
		 fnh4 = max((float)0,pSLN->fNH4NSoilConc - fnh4r);
		 fno3 = max((float)0,pSLN->fNO3NSoilConc - fno3r);
         f1=pLR->fLengthDens*(float)1e3*fMeanRootRadius*(float)2*(float)PI/((float)1e5*fRDO);
         fnh4up = f1*fLiqDiffCoeff*fnh4*pSLW->fContAct*pSL->fThickness/(float)100;
         fno3up = f1*fLiqDiffCoeff*fno3*pSLW->fContAct*pSL->fThickness/(float)100;
         //[cm/l] [mm] [mm^2/day] [mg/l] = [cm mm^3 l^-2 day^-1 mg] = cm/l 10^-5 day^-1 mg] =
         //[dm^-2 10^-7 day^-1 mg] = 10^-5 [mg/(m^2 day)]

         fnh4diffup[L]=fnh4up;
         fno3diffup[L]=fno3up;
		 pLR->fActLayNH4NUpt += fnh4up;
         pLR->fActLayNO3NUpt += fno3up; 
	     pPltN->fActNH4NUpt += fnh4up;
     	 pPltN->fActNO3NUpt += fno3up;

         fndiffup +=(fnh4up+fno3up)*(float)100;

         pSL =pSL->pNext;
         pSLW=pSLW->pNext;
         pSLN=pSLN->pNext;
    	 pLR =pLR->pNext;
      }//for

      if (fndiffup > fnpotdiffup)
      {
       pPltN = pPl->pPltNitrogen;
       pLR   = pPl->pRoot->pLayerRoot;
       pSL   = pSo->pSLayer->pNext;
       pSLW  = pWa->pWLayer->pNext; 
       pSLN  = pCh->pCLayer->pNext;
       for (L=1;L<=pSo->iLayers-2;L++)
       {
		 pLR->fActLayNH4NUpt += fnh4diffup[L]*(fnpotdiffup/fndiffup-(float)1);
         pLR->fActLayNO3NUpt += fno3diffup[L]*(fnpotdiffup/fndiffup-(float)1); 
	     pPltN->fActNH4NUpt += fnh4diffup[L]*(fnpotdiffup/fndiffup-(float)1);
     	 pPltN->fActNO3NUpt += fno3diffup[L]*(fnpotdiffup/fndiffup-(float)1);

         pSL =pSL->pNext;
         pSLW=pSLW->pNext;
         pSLN=pSLN->pNext;
    	 pLR =pLR->pNext;
       }//for
      }//if fndiffup > fnpotdiffup
     }//if fnconvup < pPltN->fTotalDemand 
    }//if pPltN->fTotalDemand > 0
    /*
    pLR   = pPl->pRoot->pLayerRoot;
    pSLN  = pCh->pCLayer->pNext;
    for (L=1;L<=pSo->iLayers-2;L++)
    {
	  pLR->fActLayNH4NUpt= (float) max(0,min(pLR->fActLayNH4NUpt,pSLN->fNH4N));
	  pLR->fActLayNO3NUpt= (float) max(0,min(pLR->fActLayNO3NUpt,pSLN->fNO3N));
	  //pSLN->fNH4N -= pLR->fActLayNH4NUpt;
	  //pSLN->fNO3N -= pLR->fActLayNO3NUpt;

      pSLN=pSLN->pNext;
      pLR =pLR->pNext;
    }//for
    */
    pPltN->fActNUpt= pPltN->fActNH4NUpt + pPltN->fActNO3NUpt;
	
	//===========================================================================================
	//Nitrogen content of dead leaves (to litter surface pool at harvest!)
	//===========================================================================================
	pPltN->fDeadLeafNw += pPltN->fLeafActConc*pPl->pBiomass->fLeafDeathRate;

	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//Nitrogen Amount of different organs
	//XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	pPltN->fLeafNinc=pPltN->fActNUpt*pPltN->fLeafDemand*(float)100/(pPltN->fTotalDemand+(float)1e-9)
                     -pPltN->fGrainDemand*pPltN->fLeafCont/(pPltN->fLeafCont+pPltN->fStemCont+(float)1e-9);
	pPltN->fStemNinc=pPltN->fActNUpt*pPltN->fStemDemand*(float)100/(pPltN->fTotalDemand+(float)1e-9)
                     -pPltN->fGrainDemand*pPltN->fStemCont/(pPltN->fLeafCont+pPltN->fStemCont+(float)1e-9);
	pPltN->fRootNinc=pPltN->fActNUpt*pPltN->fRootDemand*(float)100/(pPltN->fTotalDemand+(float)1e-9);
	//pPltN->fGrainNinc=pPltN->fActNUpt*pPltN->fGrainDemand*(float)100/(pPltN->fTotalDemand+(float)1e-9);
    //pPltN->fGrainNinc=pPltN->fActNUpt*pPltN->fGrainDemand;
    pPltN->fGrainNinc=pPltN->fGrainDemand;
    pPltN->fLeafCont  += pPltN->fLeafNinc;
    //-pPltN->fLeafNtransRate-pPl->pBiomass->fLeafDeathRate*pPltN->fLeafMinConc;
	pPltN->fStemCont  += pPltN->fStemNinc;//-pPltN->fStemNtransRate;
	pPltN->fStovCont = pPltN->fLeafCont + pPltN->fStemCont;
	pPltN->fRootCont  += pPltN->fRootNinc;
    //-pPltN->fRootNtransRate-pPl->pBiomass->fRootDeathRate*pPltN->fRootMinConc;
	pPltN->fGrainCont += pPltN->fGrainNinc;

	//===========================================================================================
	//Nitrogen content of different organs
	//===========================================================================================
	pPltN->fLeafActConc = pPltN->fLeafCont/(pPl->pBiomass->fLeafWeight+(float)1.0E-9);
	pPltN->fStemActConc = pPltN->fStemCont/(pPl->pBiomass->fStemWeight+(float)1.0E-9);
	pPltN->fVegActConc  = (pPltN->fLeafCont + pPltN->fStemCont)/(pPl->pBiomass->fLeafWeight
		                                                         +pPl->pBiomass->fStemWeight+(float)1.0E-9);
	pPltN->fTopsActConc = pPltN->fVegActConc;//for graphics
	pPltN->fGrainConc   = pPltN->fGrainCont/max((float)1.0E-9,pPl->pBiomass->fGrainWeight);
	pPltN->fRootActConc = pPl->pPltNitrogen->fRootCont/(pPl->pBiomass->fRootWeight+(float)1.0E-9);

    pPltN->fActNUptR=pPltN->fActNUpt;//per day, i.e. dt=1 !

    return 1;    
	}
	
/*******************************************************************************
** EOF */