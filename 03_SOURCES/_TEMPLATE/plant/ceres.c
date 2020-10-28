/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:  Enli Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   This file contains the source code for all plant process functions developed
 *   based on CERES2.0
 *   The full description of the CERES model can be found in Ritchie et al (1986)
 *   and Jones et al(1986)
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 5 $
 *
 * $History: ceres.c $
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 15:41
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/plant
 * Typecasts zur Vermeidung von Warnings.
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:13
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/plant
 * Unsinnige Defines entfernt (DLL wird �berdies bereits verwendet).
 * 
 *   Update: 26.02.1998
 *
*******************************************************************************/

#include <windows.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include  "xinclexp.h"


int WINAPI _loadds PhasicDevelopment_CERES(EXP_POINTER);
int WINAPI _loadds RootSystemFormation_CERES(EXP_POINTER);
int WINAPI _loadds DailyCanopyGrossPhotosynthesis_CERES(EXP_POINTER);

float WINAPI _loadds PotentialTranspiration_CERES(EXP_POINTER);                                     
int WINAPI _loadds ActualTranspiration_CERES(EXP_POINTER);
float fPotTraDay, fActTraDay; //wie ep!

int WINAPI _loadds PlantWaterStress_CERES(EXP_POINTER);
int WINAPI _loadds CropMaintenance_CERES(EXP_POINTER);
int WINAPI _loadds BiomassPartition_CERES(EXP_POINTER);
int WINAPI _loadds BiomassGrowth_CERES(EXP_POINTER);
int WINAPI _loadds CanopyFormation_CERES(EXP_POINTER);
int WINAPI _loadds PlantSenescense_CERES(EXP_POINTER);
int WINAPI _loadds NitrogenConcentrationLimits_CERES(EXP_POINTER);
int WINAPI _loadds PlantNitrogenDemand_CERES(EXP_POINTER);
int WINAPI _loadds NitrogenUptake_CERES(EXP_POINTER);
int WINAPI _loadds PlantNitrogenStress_CERES(EXP_POINTER);		
int WINAPI _loadds RootExtension_CERES(EXP_POINTER);


//extern int WINAPI _loadds DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex);
extern int WINAPI _loadds StringGetFromTo(LPSTR lpStr1,LPSTR lpStr2,int nBeg,int nEnd);                                                              

int WINAPI _loadds DevelopmentBeforeEmergence(EXP_POINTER);
int WINAPI _loadds DevelopmentAfterEmergence (EXP_POINTER);
int   Germination			(EXP_POINTER);
float Emergence	 			(EXP_POINTER);
float PhotoperiodEffect		(EXP_POINTER);
float Vernalization_CERES	(EXP_POINTER);

float WINAPI _loadds DailyThermTime(EXP_POINTER);
int WINAPI _loadds CERES_TO_COMVR(EXP_POINTER);

int PlantHeightGrowth_CERES(EXP_POINTER); //cl 060406

float PlantTemperature(EXP_POINTER);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//       	internal Functions
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int DailyCanopyPhotosynthesis_WH_BA	(EXP_POINTER);
int DailyCanopyPhotosynthesis_MZ	(EXP_POINTER);
int DailyCanopyPhotosynthesis_SF	(EXP_POINTER);
int BiomassPartition_WH_BA 			(EXP_POINTER);
int BiomassPartition_MZ 			(EXP_POINTER);
int BiomassPartition_SF 			(EXP_POINTER);
int PlantLeafNumber_WH_BA  			(EXP_POINTER);
int PlantLeafNumber_MZ  			(EXP_POINTER);
int PlantLeafNumber_SF  			(EXP_POINTER);
int PlantLeafArea_WH_BA	  			(EXP_POINTER);
int LeafSenescence_WH_BA			(EXP_POINTER);
int LeafSenescence_MZ			    (EXP_POINTER);
int LeafSenescence_SF				(EXP_POINTER);
int Tillering_WH_BA		  			(EXP_POINTER);
int TillerDeath_WH_BA	  			(EXP_POINTER);
int ColdHardening_WH_BA				(EXP_POINTER);

extern float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp);

float WINAPI _loadds IncLA_SF(float LeafExpandNum, float LeafTipNum, float TempMax, float TempMin);
int WINAPI _loadds NitrogenGrain_SF(EXP_POINTER);
int WINAPI _loadds Initialize_MZ_SF(EXP_POINTER);



//Plant Part
int		iStageOld;
int		iTLNO;
float	fPHINT, fXDevStage, fP5, fP9, fO1, fNFAC;
float	fDaylengthYesterday=(float)12;

//Maize:
float fEarWeight, fEarGrowR, fEarFraction;
float fTasselLeafNum, fMaxPlantLA;
int  iSlowFillDay;

//Sunflower:
float fStemWeightMin, fStemWeightMax;
float fHeadWeight, fHeadGrowR, fPotHeadWeight, fHeadWeightMin, fHeadWeightMax;
float fPericarpWeight, fPericarpGrowR, fPotPericarpGrowR;
float fEmbryoWeight, fEmbryoGrowR;
float fIGrainWeight, fOil, fOilPercent;
float fNHeadActConc, fNHeadMinConc, fNHeadOptConc, fNHeadDemand, fNHeadCont;
float fNPericarpActConc, fNPericarpOptConc, fNPericarpDemand, fNPericarpCont;
float fNEmbryoActConc, fNEmbryoOptConc, fNEmbryoDemand, fNEmbryoCont;
float fNGreenLeafActConc, fNGreenLeafCont, fNSenesLeafCont;
//float fTotWeight2, fNTopsCritConc;
float fSumPS, fRI1, fRM, fRFR, fGrowFactor, ppp;
float fCumSenesLA, fSpecLeafArea, fAPLA;
float fSenesLAN2, fSLOPEPE, fXPEPE, fYRAT;
static int iDurP, iSenesTime, iJPEPE;
BOOL bSenesCode;

//SG 20110810: CO2 als Input (f�r AgMIP)
extern float fAtmCO2;


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  PHASIC DEVELOPMENT FUNCTION
//------------------------------------------------------------------------------------------------------------
extern float WINAPI DaylengthAndPhotoperiod_SPASS(float fLatitude, int nJulianDay, int nID);
int WINAPI _loadds PhasicDevelopment_CERES(EXP_POINTER)
	{
	PGENOTYPE		pGen		= pPl->pGenotype;
	PBIOMASS		pBiom		= pPl->pBiomass;
	PDEVELOP		pDev		= pPl->pDevelop;
	PCANOPY			pCan		= pPl->pCanopy;
	PPLTNITROGEN	pPltN		= pPl->pPltNitrogen;
	PSTAGEPARAM		pStageParam = pPl->pGenotype->pStageParam;
	PMODELPARAM		pPltMP		= pPl->pModelParam;

	int		iStage, iCurrentStage;
	float 	fDaylength,CHGDL;   

	//f�r Sonnenblume:
	float PSKER, fPS;

	if((pTi->pSimTime->lTimeDate == pMa->pSowInfo->lSowDate)&&
			((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))||
			(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))))
		Initialize_MZ_SF(exp_p);


	if (pPl->pDevelop->iStageCERES==0) 
		pPl->pDevelop->iStageCERES=8;

    iStageOld=iCurrentStage =pPl->pDevelop->iStageCERES;

    //==========================================================================================
    //Phasic Development
	//==========================================================================================
	if (iCurrentStage>6) 
		DevelopmentBeforeEmergence(exp_p);
	else			     
		DevelopmentAfterEmergence(exp_p);

    iCurrentStage =pPl->pDevelop->iStageCERES;

    //==========================================================================================
    //CERES Stage Thermal Time Initialization
	//==========================================================================================
	
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
	{
		fDaylength=DaylengthAndPhotoperiod_SPASS(pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay,1);
		CHGDL=fDaylength-fDaylengthYesterday;
		fDaylengthYesterday = fDaylength;
	}

	if (iStageOld!=iCurrentStage)
		{   
		iStage = iCurrentStage-1;

		if(iStage ==0) //Wechsel von 9 auf 1
		{
			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			{ 
				//SG20110909: sortenabh�ngiges PHINT f�r AgMIP 
				if(pPl->pGenotype->fPhyllochronInterval <= 10)
				{
					fPHINT=(float)95;//(float)84.0;//(float)95.0;//INRA (float)103; //Kraichgau: 122
					if(!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Oasis"))
						fPHINT = (float)104;
					if(!lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,(LPSTR)"Cubus"))//Kraichgau
						fPHINT = (float)122;
				}
				else //(�ber gtp!)
					fPHINT = pPl->pGenotype->fPhyllochronInterval;
			}
            else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))   
				fPHINT=(float)77.5-(float)232.6*CHGDL;//INRA (float)95.0 
			else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
				fPHINT=pPl->pGenotype->pStageParam->afThermalTime[0];
			else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
				fPHINT=pPl->pGenotype->pStageParam->afThermalTime[0];
			else
				fPHINT=pPl->pGenotype->pStageParam->afThermalTime[0];

			//Thermal time of stages 2 + 3

			//ben�tigt f�r WH, BA:
			pPl->pGenotype->pStageParam->afThermalTime[1] = (float)3.0 * fPHINT;
			//INRA-Ceres
			//pPl->pGenotype->pStageParam->afThermalTime[1] = (float)4.0 * fPHINT;
			//ben�tigt f�r WH, BA, SF:
			pPl->pGenotype->pStageParam->afThermalTime[2] = (float)2.0 * fPHINT;

			fP5 = pPl->pGenotype->pStageParam->afThermalTime[3] + 
					pPl->pGenotype->pStageParam->afThermalTime[4] +
					pPl->pGenotype->pStageParam->afThermalTime[5];
		}

    
    	if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")))
		{
    		if (iStage==0) //SG 2005 11 03: Anpassung an INRA-CERES
			{
				pDev->fSumDTT = pDev->fSumDTT - fP9;
				pDev->fDTT = pDev->fSumDTT;
				pDev->fCumTDU = (float)0.0;
			}
			
			if ((iStage>=1)&&(iStageOld<=5))
    		{
    			if (iStage==1) 
					pPl->pDevelop->fSumDTT = (float)0.0;
				else
					pPl->pDevelop->fSumDTT -= pStageParam->afThermalTime[iStage-1];
    			
				if (iStage==4) //SG 2005 11 03: aus BiomassGrowth_CERES hierher verlegt
				{	
					float fStemWeight;
					
					//====================================================================================
					//	Grain number and weight
					//====================================================================================
					pPl->pPltNitrogen->fVegActConc	=pPl->pPltNitrogen->fTopsActConc;
					pPl->pPltNitrogen->fVegMinConc	=pPl->pPltNitrogen->fTopsMinConc;

					pPl->pCanopy->fPlantGreenLA=pPl->pCanopy->fPlantLA
	    												-pPl->pCanopy->fPlantSenesLA;

  					fStemWeight =pBiom->fStemWeight/((float)10.0*pMa->pSowInfo->fPlantDens);
				  	
    				pCan->fGrainNum	= fStemWeight*pPl->pGenotype->fStorageOrganNumCoeff;
    				pCan->fGrainNumSq	= pCan->fGrainNum*pMa->pSowInfo->fPlantDens;

    				if (pCan->fGrainNumSq<(float)100.0)
    					{
    					pCan->fGrainNumSq	=(float)1.0;
    					pCan->fEarNum		=(float)0.0;
    					}

			//      pBiom->fGrainWeight=(float)0.0035*pCan->fGrainNum;
					pBiom->fGrainWeight=(float)0.0035*pCan->fGrainNum*(float)10.0*pMa->pSowInfo->fPlantDens; //SG 2005 11 03
				}
			} // end iStage >= 1 && iStageOld <= 5
		} // end WH, BA
		} // end iStageOld != iCurrentStage


	//##############################################################################

	//Initialisierung von Variablen zu Beginn der einzelnen Entwicklungsstadien:

	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"MZ"))
	{		
		switch(iCurrentStage)
		{

		case 1:
			if(iStageOld==9) // Initialisierung zu Beginn von Stadium 1
			{
				pDev->fSumDTT = pDev->fSumDTT - fP9;

				// Plant leaf area [m^2/ha]
				pCan->fPlantLA=pCan->fPlantGreenLA	=(float)(1.0*pMa->pSowInfo->fPlantDens);
				pCan->fPlantSenesLA = (float)0.0;
				pCan->fLAI=(float)1E-4*pCan->fPlantLA;

				//leaf number (= LN)
				pCan->fLeafTipNum = (float)1.0;
				pPl->pCanopy->fExpandLeafNum = (float)1.0;
				pPltMP->fMaxLeafNum = (float)30.0;

				pBiom->fLeafWeight		= 
				pBiom->fGreenLeafWeight	= (float)0.2;//*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fSenesLeafWeight = (float)0.0;
				pBiom->fRootWeight		= (float)0.2;//*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fStemWeight		= (float)0.2;//*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fGrainWeight		= 
				fEarWeight				= (float)0.0; 
				pBiom->fSeedReserv		= (float)0.2;//*(float)10.0*pMa->pSowInfo->fPlantDens;
				
				// Plant leaf area [m^2/ha]
				pCan->fPlantLA=pCan->fPlantGreenLA	= pBiom->fLeafWeight/pGen->fSpecLfWeight*(float)1.e4;//(float)(1.0*pMa->pSowInfo->fPlantDens);
				pCan->fPlantSenesLA = (float)0.0;
				pCan->fLAI=(float)1E-4*pCan->fPlantLA;
				//**********************************************************************************

				
				pBiom->fStovWeight		=(float)0.4;//*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fTotalBiomass	=pBiom->fStovWeight + pBiom->fRootWeight;		


				 

				pPltN->fRootActConc	=(float)0.022;
				pPltN->fTopsActConc	=(float)0.044;
				

				fNFAC = (float)1.0;

				pPltN->fRootCont =pPltN->fRootActConc*pBiom->fRootWeight;
				pPltN->fStovCont =pPltN->fTopsActConc*pBiom->fStovWeight;

			}
			break;

		case 2:
			
			break;

		case 3:
			if(iStageOld == 2)
			{
				//P3 calculated from the total number of leaves that will eventually
				//appear (iTLNO) and the thermal time required for the emergence of 
				//each leaf... (Jones & Kiniry 1986, S.74)
				pPltMP->fMaxLeafNum = (float)((int)(pPl->pDevelop->fSumDTT/(float)21.0) + 6.0);
				pStageParam->afThermalTime[2] = (float)((pPltMP->fMaxLeafNum-2.0)*38.9 
															+ 96.0 - pPl->pDevelop->fSumDTT);
				fTasselLeafNum = pCan->fExpandLeafNum;
				
				pDev->fSumDTT	= (float)0.0;
			}

			break;

		case 4:
			if(iStageOld==3)
			{
				fEarWeight = (float)0.167*pBiom->fStemWeight;
				pBiom->fStemWeight -= fEarWeight;
				pPltMP->fMinStemWeight = (float)0.85 * pBiom->fStemWeight;

				fMaxPlantLA = pCan->fPlantLA;

				pDev->fSumDTT	-= pGen->pStageParam->afThermalTime[2];
			
				fSumPS = (float)0.0;
				iDurP = 0;
			}

			break;

		case 5:
			if(iStageOld==4)
			{
				pPltMP->fMinLeafWeight = (float)0.85 * pBiom->fLeafWeight;
				pPltMP->fMaxStemWeight = pBiom->fStemWeight;
				pDev->fSumDTT = pDev->fSumDTT - pStageParam->afThermalTime[3];
				iSlowFillDay = 0;

				PSKER = fSumPS/(float)iDurP*(float)(1000.0*3.4/3.5); //[kg/ha/day]
				PSKER *= (float)0.1/pMa->pSowInfo->fPlantDens; //[g/plant]

				//grains per plant (CERES-Maize, pp.72)
				pCan->fGrainNum =(float)max(51.0,pGen->fStorageOrganNumCoeff*(float)((PSKER-195.0)/(1213.2+PSKER-195.0)));
				pCan->fGrainNum = (float)min(pCan->fGrainNum,pGen->fStorageOrganNumCoeff);
				
				//ears per m^2 
				pCan->fEarNumSq = pMa->pSowInfo->fPlantDens;

				if(pCan->fGrainNum < pGen->fStorageOrganNumCoeff*(float)0.15)
					pCan->fEarNumSq = (float)max(0.0,pMa->pSowInfo->fPlantDens * 
						(float)pow((pCan->fGrainNum-50.0)/(pGen->fStorageOrganNumCoeff-50.0),0.33));

				pCan->fGrainNumSq = pCan->fGrainNum * pCan->fEarNumSq;

				pPl->pPltNitrogen->fVegActConc	=pPl->pPltNitrogen->fTopsActConc;
				pPl->pPltNitrogen->fVegMinConc	=pPl->pPltNitrogen->fTopsMinConc;
			}
			break;

		case 6:
			break;

		case 9:
	
			if(iStageOld == 8)
			{
				pDev->fSumDTT = pDev->fCumDTT = (float)0.0;
				pPltN->fNStressPhoto = pPltN->fNStressLeaf = (float)1.0;
			}

			break;
		}

	}

	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SF"))
	{		
		PBIOMASS		pBiom = pPl->pBiomass;
		PDEVELOP		pDev  = pPl->pDevelop;
		PCANOPY			pCan  = pPl->pCanopy;
		PPLTNITROGEN	pPltN = pPl->pPltNitrogen;
		PSTAGEPARAM		pStageParam = pPl->pGenotype->pStageParam;
		float ALF1;

		switch(iCurrentStage)
		{

		case 1:
			if(iStageOld==9) // Initialisierung zu Beginn von Stadium 1
			{
				//OILCROP-SUN, pp.63

				pDev->fSumDTT = pDev->fSumDTT - fP9;

				// Plant leaf area [m^2/ha]
				pCan->fPlantLA=pCan->fPlantGreenLA	=(float)(300.0*pMa->pSowInfo->fPlantDens * 1e-2);
				pCan->fPlantSenesLA = (float)0.0;
				pCan->fLAI=(float)1E-4*pCan->fPlantLA;

				//leaf number (= LN)
				pCan->fLeafTipNum = (float)1.0;
				pPl->pCanopy->fExpandLeafNum = (float)0.0;
				//initial estimate of final leaf number
				//(TLNO = 2+(P1+30+30*P2)/14)
				iTLNO = (int)((float)(2+(pStageParam->afThermalTime[0] +30 + 30*pPl->pGenotype->fDaylengthCoeff)/14));

				pBiom->fLeafWeight = pBiom->fGreenLeafWeight	=
									 (float)0.03*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fRootWeight	=(float)0.02*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fStemWeight	=(float)0.01*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fStovWeight	=(float)0.04*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fTotalBiomass=pBiom->fStovWeight;		
				
				pBiom->fSenesLeafWeight = (float)0.0;
				pBiom->fGrainWeight	=(float)0; 

				pPltN->fRootActConc	=(float)0.0466;
				pPltN->fLeafActConc	=(float)0.062;
				pPltN->fStemActConc	=(float)0.0444;
				pPltN->fTopsActConc	=(float)0.0576;
				fNHeadActConc       =(float)0.042;

				fNFAC = (float)1.0;

				pPltN->fRootCont =pPltN->fRootActConc*pBiom->fRootWeight;
				pPltN->fLeafCont = fNGreenLeafCont = 
								  pPltN->fLeafActConc*pBiom->fLeafWeight;
				pPltN->fStemCont =pPltN->fStemActConc*pBiom->fStemWeight;
				pPltN->fStovCont =pPltN->fTopsActConc*pBiom->fStovWeight;

			}
			break;

		case 2:
			
			break;

		case 3:
			if(iStageOld == 2)
			{
				iTLNO			= (int)((pDev->fCumDTT-fP9)/(float)14.0)+2;
				fSumPS			= (float)0.0;
				iDurP			= 0;
				pDev->fSumDTT	= (float)0.0;
				bSenesCode		= FALSE;
				iSenesTime		= 0;
				fCumSenesLA		= (float)0.0;
			}



			break;

		case 4:

			if(iStageOld==3)
			{

				fStemWeightMin = (float)0.8 * pBiom->fStemWeight;
				fStemWeightMax = (float)1.8 * pBiom->fStemWeight;
				pDev->fSumDTT = pDev->fSumDTT - pStageParam->afThermalTime[2];


				PSKER = fSumPS/(float)iDurP; //[kg/ha/day]
				fRM = (pBiom->fBiomassAbvGround+pBiom->fRootWeight)*(float)(0.008*0.729);//[kg/ha/day]
				fRI1 = (float)1.0+fRM/PSKER;


				//calculate number of pericarps per plant
				if(PSKER > (float)(5.0 * pMa->pSowInfo->fPlantDens * 10.0))
					ppp = (float)(430.0 + pPl->pGenotype->fStorageOrganNumCoeff*
							PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/14.5);
				else
					ppp = (float)(PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/5.0*
						(430.0 + pPl->pGenotype->fStorageOrganNumCoeff*5.0/14.5));

				if(ppp>pPl->pGenotype->fStorageOrganNumCoeff)
					ppp = pPl->pGenotype->fStorageOrganNumCoeff;


				//calculate factor for grain Growth
				fGrowFactor = (float)min(1.0,0.6+0.4*min(1.0,fHeadWeight/fPotHeadWeight));
      
				//initialize pericarp weight [kg/ha]
				fPericarpWeight = ppp*(float)0.002*pMa->pSowInfo->fPlantDens*(float)10.0;
				fHeadWeight = fHeadWeight - fPericarpWeight;

				//calculate potential pericarp growth rate [kg/ha/day]
				ALF1 = (float)((0.22*pPl->pGenotype->fRelStorageFillRate/24.*(fP5-170.)-2.*0.78)/270./0.78);
				fPotPericarpGrowR = (float)0.024*ALF1*ppp*pMa->pSowInfo->fPlantDens*(float)10.0;

				//Fix minimum N amount in leaves at 0.9% of the current amount at first anthes�s
				//fNLeafMinCont = (float)0.009 * pBiom->fLeafWeight;

				//Calculate nitrogen concentration in green leaves
				fNGreenLeafActConc = fNGreenLeafCont/pBiom->fGreenLeafWeight;
				//determine nitrogen concentration in senesced leaves
				fYRAT = (float)((0.009 - 0.0875*fNGreenLeafActConc)/0.9125);

				//Determine the (plant leaf area)/(available N in leaves) [m^2/kg] ratio. 
				//This variable is used to calculate senescence rate driven by N demand
				fSLOPEPE = pCan->fLAI/(fNGreenLeafCont-fYRAT*pBiom->fGreenLeafWeight)*(float)1e4;
				fSenesLAN2 = (float)0.0; 
				//Determine the amount of N in leaves [kg(N)/ha] that can be rapidly exported.
				//This is assumed to equal 50% of labile N.
				fXPEPE   = (float)0.50 * (fNGreenLeafCont - fYRAT*pBiom->fGreenLeafWeight);

				//maximal green leaf area
				fAPLA = pCan->fLAI * (float)10000.0;


				fSumPS = (float)0.0;
				iDurP = 0;

			}

			break;

		case 5:
			if(iStageOld==4)
			{
				//Initialize:
				//Grain number per plant
				pCan->fGrainNum = (float)0.0;
				//maximum head weight
				fHeadWeightMax = (float)2.2*fHeadWeight;
				//minimum head weight
				fHeadWeightMin = fHeadWeight;
				//individual grain weight
				fIGrainWeight = (float)0.0;
			}

			if((pDev->fSumDTT >= (float)230.0)&&(pCan->fGrainNum==(float)0.0))
			{
				fPS = fRM/(fRI1-(float)1.0);
				fRM = (pBiom->fBiomassAbvGround+pBiom->fRootWeight)*(float)(0.008*0.729);
				
				fRI1 = (float)1.0+fRM/fPS;
				
				//calculate average rate of dry matter accumulation in 
				//the period after anthesis
				PSKER = fSumPS/(float)iDurP; //[kg/ha]

				//calculate number of filled grains per plant
				if(PSKER > (float)(5.0 * pMa->pSowInfo->fPlantDens * 10.0))
					pCan->fGrainNum = (float)(430.0 + pPl->pGenotype->fStorageOrganNumCoeff*
							PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/14.5);
				else
					pCan->fGrainNum = (float)(PSKER/(pMa->pSowInfo->fPlantDens * 10.0)/5.0*
						(430.0 + pPl->pGenotype->fStorageOrganNumCoeff*5.0/14.5));

				if(pCan->fGrainNum>pPl->pGenotype->fStorageOrganNumCoeff)
					pCan->fGrainNum = pPl->pGenotype->fStorageOrganNumCoeff;

				if(pCan->fGrainNum > ppp)
					pCan->fGrainNum=ppp;
               
			}
			break;

		case 6:
			break;

		case 9:
			break;
		}

	}


	CERES_TO_COMVR(exp_p);

	return 1;
	}

			
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds DevelopmentBeforeEmergence(EXP_POINTER)
	{
	//=======================================================================
	//Variable Declaration and Initiation
	//=======================================================================
	int			iGerm,iReturn;
	float 		fDayThermTime,fEmerg;   
	PDEVELOP 	pDev=pPl->pDevelop;

	//=======================================================================
	//Germination and Emergence
	//=======================================================================
	switch (pDev->iStageCERES)
	    {
		case 8:	//Germination Part
			iGerm=Germination(exp_p);
            pDev->fStageXCERES=(float)0.0;
      
			switch (iGerm)
            	{
            	case 0:
            		iReturn=0;	//Germination doesn't occur
	   				break;
    			case -1:
    				iReturn=-1; //Germination fails.
			    	break;
			    case 1:   		//Germination occurs
					pDev->iStageCERES=9;
				    pDev->fStageXCERES=(float)0.5;
					iReturn=1;
					break;
				}
			break;

    	case 9:	//Emergence Part 
          	pDev->fDTT=fDayThermTime=DailyThermTime(exp_p);
            //SG 2005 11 03: Anpassung an INRA-CERES
			pDev->fSumDTT  += pDev->fDTT;

			if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")))
			{
				if (pDev->fDTT<(float)0.0)	 pDev->fDTT=(float)0.0;
				pDev->fSumDTT  += pDev->fDTT;
				pDev->fCumDTT  += pDev->fDTT;
			}
          	
    		fEmerg=Emergence(exp_p);

            pDev->fStageXCERES = (float)0.5 + fEmerg/(float)2.0;

    		if (fEmerg<(float)0.0) iReturn=-2;

    		if ((fEmerg>=(float)0.0)&&(fEmerg<(float)1.0))  //Emergence doesn't occur
    			iReturn=0;

    		if (fEmerg>=(float)1.0)//Emergence occurs
    		{
    			iReturn=2;
				pDev->iStageCERES=1;
			}

    			

			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")
				||(!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"BA")))
            	{
				//pDev->fStageXCERES=fEmerg;
    			Vernalization_CERES(exp_p);
				//ColdHardness();
				}

    		break;
    	}

	
    return iReturn;
    }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Development after Emergence
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds DevelopmentAfterEmergence(EXP_POINTER)
	{
	//=========================================================================================
	//Variable Declaration and Initiation
	//=========================================================================================
	int 	iDevStage0,iDevStage;
	float 	DTT,TDU,DF,VF,fDevValue,PHINT0;
	BOOL	bDaylengthSensitive,bVernalization;
	int i=1; 
	PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;

	PDEVELOP pDev=pPl->pDevelop;

	iDevStage0=iDevStage=pDev->iStageCERES;

	if (iDevStage0==0)
		{
		pDev->iStageCERES=8;
		return 8;
		}

	//===========================================================================================
	//		Calculation of Daily Thermal Time (DTT) and SumDTT, CumDTT
	//===========================================================================================
	DTT=DailyThermTime(exp_p);

    if (DTT<(float)0.0)	 DTT=(float)0.0;

	pDev->fSumDTT  += DTT;
    pDev->fCumDTT  += DTT;
	pDev->fDTT		= DTT;

	//=========================================================================================
	//		Check whether the crop in this stage is Daylength sensitive or vernalizes
	//=========================================================================================
	bDaylengthSensitive	=FALSE;
	bVernalization		=FALSE;
	
	//Maize, Sunflower
	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))||
			(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF")))
	{
		if (iDevStage==2)
			bDaylengthSensitive=TRUE;
	}

	//Wheat, Barley
	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
    	{
    		if (iDevStage==1)
				bDaylengthSensitive=TRUE;
			if ((iDevStage==9)||(iDevStage==1)) //Wenn iDevStage = 9 wird diesee Funktion doch gar nicht aufgerufen!
				bVernalization=TRUE;
    	}

	//===========================================================================================
	//		Calculate the Daylength effect factor (DF)
	//===========================================================================================
    if (bDaylengthSensitive==TRUE)		//This stage is photoperiod sensitive.
		DF=PhotoperiodEffect(exp_p);
    else 								//This stage is photoperiod unsensitive.
    	DF=(float)1.0;

	//===========================================================================================
	//		Calculate the vernalization effect factor (VF)
	//===========================================================================================
    if ((bVernalization==TRUE)&&(pPl->pGenotype->fOptVernDays!=0))	//When vernalization exists
//    if (bVernalization==TRUE)	//When vernalization exists
    	VF=Vernalization_CERES(exp_p);
	else                       //When no vernalization
		VF=(float)1.0;

	//===========================================================================================
	//		Calculate the Thermal Development Unit (TDU)
	//===========================================================================================
    TDU=DTT*min(DF,VF);

	if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"BA")) 
		{
		if (pCl->pWeather->fSolRad<=(float)10.0)
			pPl->pModelParam->fBarleyGrwParam=
							(float)(1.0-pow((10.0-(double)pCl->pWeather->fSolRad),2.0)
							/pMa->pSowInfo->fPlantDens);
		else
			pPl->pModelParam->fBarleyGrwParam=(float)1.0;

		TDU=TDU*pPl->pModelParam->fBarleyGrwParam;
		}
		
    pDev->fCumTDU  	+=TDU;
    pDev->fDevR	     =TDU; //fDevR wird in CERES nicht weiter gebraucht. 
						   //Dient nur der Schnittstelle zwischen den verschiedenen Pflanzenmodellen
	//===========================================================================================
	//		Calculate Daily Develop Stage Values
	//===========================================================================================
   	if ((bDaylengthSensitive==FALSE)&&(bVernalization==FALSE))
	{
		fDevValue=pDev->fDTT/pStageParam->afThermalTime[iDevStage-1];

		if((int)(pDev->fStageXCERES+fDevValue)>pDev->iStageCERES)
		{
			fDevValue = (float)(pDev->iStageCERES+1) - pDev->fStageXCERES +
				(pDev->fStageXCERES+fDevValue - (float)(pDev->iStageCERES+1))*
				pStageParam->afThermalTime[iDevStage-1]/pStageParam->afThermalTime[iDevStage];
		}
	}
	
	else
		{
		if ((!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"BA")))
    		{
			int i=1; PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;
		//	while (i<iDevStage)  pStageParam=pStageParam->pNext;

			if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))
				PHINT0=(float)95.0;
    		else
				PHINT0=(float)75.0; //Barley
			
    		fDevValue=TDU/pStageParam->afThermalTime[iDevStage-1];
            fDevValue=fDevValue*PHINT0/fPHINT;
            }

			//Maize, Sunflower
    		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF")))  //for Maize
				{
				fDevValue		= DF;
				//pDev->fCumTDU	+=DF;
    			pDev->fDevR		= DF;

				if(pDev->fStageXCERES+fDevValue > (float)(pDev->iStageCERES+1))
					fDevValue = (float)(pDev->iStageCERES+1) - pDev->fStageXCERES;
 	  			} 
		}

    if (fDevValue<(float)0.0) 	fDevValue=(float)0.0;

	//===========================================================================================
	//		Calculate Develop stage values
	//===========================================================================================
  	pDev->fStageXCERES=pDev->fStageXCERES+fDevValue;
	iDevStage=(int)min(pDev->fStageXCERES,7.0);
	pDev->iStageCERES=iDevStage;


	// Berechnung des aktuellen Entwicklungszustands fXDevStage, 
	// der bei Mais und Sonnenblume f�r die Absch�tzung der
	// NitrogenConcentrationLimits ben�tigt wird:

	//Maize, Sunflower
 	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF")))  
   		{
   		switch (iDevStage)
   			{
   			case 1:
   				fXDevStage = pDev->fStageXCERES-(float)1.0;	// 0 - 1
   				break;

   			case 2:
				if(fXDevStage<(float)1.0)
					fXDevStage = (float)(1.0 + 0.5*(fXDevStage+fDevValue-1.0));
				else
   					fXDevStage += fDevValue*((float)0.5);		// 1 - 1.5
   				break;

   			case 3:
				if(fXDevStage<(float)1.5)
					fXDevStage = (float)(1.5 + 3.0*(fXDevStage+0.5*fDevValue-1.5));
				else
   					fXDevStage += fDevValue*((float)3.0);		// 1.5 - 4.5
   				break;

   			case 4:
   			case 5:
			case 6:												// 4.5 - 10
				if(fXDevStage<(float)4.5)
					fXDevStage = (float)(4.5 + 5.5*pDev->fDTT/fP5);
				else
   					fXDevStage += pDev->fDTT/fP5*((float)5.5);
				break;

			case 7:
				fXDevStage = (float)10.0;
				break;
   			}
   		}

	if ((!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"BA")))
		{
		//	pDev->fStageXCERES += fDevValue; //SG/14/06/99: wurde doch schon dazuaddiert!

	    if (iDevStage==6)
	    	{
	    	pDev->fStageXCERES=pDev->fStageXCERES+(float)0.35;
	        pDev->iStageCERES=(int)pDev->fStageXCERES;
	        }
    	}
	//=====================================================================================
	//	Stage transfer from Wang's fDevStage to BBA Code nach Sch�tte und Meier (1981)
	//=====================================================================================
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		{
	  	//When daily thermal time accumulation is less than 2.0 in the stage
	  	//before the maturing date, then maturity occurs.
	    if ((iDevStage==6) && (DTT<(float)2.0))  //for Maize
	    	{
	    	pDev->fStageXCERES=pDev->fStageXCERES+(float)1.0;
	        pDev->iStageCERES=pDev->iStageCERES+1;
	        }
        }

	return iDevStage;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds DailyThermTime(EXP_POINTER)
	{
	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	float 	fTempMinCrop,fTempMaxCrop,fTempAveCrop,fTempDiff,fTempCorr,fTempFac,fTemp;
	float 	fDailyThermTime; 	//Return Value -Daily Thermal Time (degree.day).
	double 	J;
	int 	I=1;
	float fTempBaseDev, fTempOptDev, fTempMaxDev;

	LPSTR lpCropType	=pPl->pGenotype->acCropCode;   
	int   iDevStage		=pPl->pDevelop->iStageCERES; 
	float fTempMax		=pCl->pWeather->fTempMax;
	float fTempMin		=pCl->pWeather->fTempMin;
	float fSnow			=pCl->pWeather->fSnow;
	PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;

	//SG/10/06/99: 
	//	Hier gibt es Probleme. pPl->pGenotype->pStageParam ist nicht gesetzt,
	//	deshalb kommt es zum Absturz.
	//	Stattdessen fTempBase = 0�C (vgl. CERES-WHEAT, S. 10)
	/////////////////////////////////////////////////////////////////////////
	//while (I<iDevStage) pStageParam=pStageParam->pNext;
	fTempBaseDev =pStageParam->afTempBase[iDevStage-1];
	fTempOptDev = pStageParam->afTempOpt[iDevStage-1];
	fTempMaxDev = pStageParam->afTempMax[iDevStage-1];

	//===========================================================================================
	//For Wheat and Barly, the Daily Thermal Time is calculated as follows:
	//===========================================================================================
	if ((!lstrcmp(lpCropType,(LPSTR)"WH"))||(!lstrcmp(lpCropType,(LPSTR)"BA")))
		{
        //The Plant Crown Temperature is assumed to be the air temperature.

        fTempMinCrop=fTempMin;		//fTempMinCrop -Minimum temperature of plant crown (C)
        fTempMaxCrop=fTempMax;      //fTempMaxCrop -Maximum temperature of plant crown (C)

		    if (fSnow<(float)0.0)
		    	fSnow=(float)0.0;

        //Snow raises the Plant Crown Temperature higher than air temperature.
        //if (fSnow>(float)0.0)
	    //{
			if (fSnow>(float)15.0)	//fSnow -Precipitation in the form of Snow (mm)
        		fSnow=(float)15.0;
			if (fTempMin<(float)0.0)
        		//fTempMinCrop -Minimum temperature of plant crown (C)
        		fTempMinCrop=(float)2.0+fTempMin*((float)0.4+
        		   ((float)0.0018)*(fSnow-(float)15.0)*(fSnow-(float)15.0));
			if (fTempMax<(float)0.0)
        		//fTempMaxCrop -Maximum temperature of plant crown (C)
        		fTempMaxCrop=(float)2.0+fTempMax*((float)0.4+
        		   ((float)0.0018)*(fSnow-(float)15.0)*(fSnow-(float)15.0));
	    //}

        //fTempAveCrop -Average temperature of plant crown (C)
        fTempAveCrop=(fTempMaxCrop+fTempMinCrop)/((float)2.0);

	    pPl->pPltClimate->fCanopyTemp=fTempAveCrop;


		//Calculating Daily Thermal Time as follows
        fTempDiff=fTempMaxCrop-fTempMinCrop; 	//fTempDiff -Temperature Amplitude of Crop (C)

        if (fTempDiff==(float)0.0)
        	fTempDiff=(float)1.0;

        if (fTempMaxCrop<fTempBaseDev)		//fTempMinCrop < fTempMaxCrop < fTempBase
         	fDailyThermTime=(float)0.0;

        if (fTempMaxCrop>=fTempBaseDev)
        	{
			if (fTempMaxCrop<=fTempOptDev)
				{
				if (fTempMinCrop>fTempBaseDev)		//fTempBaseDev < fTempMinCrop < fTempMaxCrop < 26.0
					fDailyThermTime=fTempAveCrop-fTempBaseDev;

				if (fTempMinCrop<=fTempBaseDev)	//fTempMinCrop < fTempBaseDev < fTempMaxCrop < 26.0
					{
					fTempCorr=(fTempMaxCrop-fTempBaseDev)/fTempDiff;//fTempCorr -Temperature Correction
					fDailyThermTime=(fTempMaxCrop-fTempBaseDev)/((float)2.0)*fTempCorr;
	                }
                }

            if ((fTempOptDev<fTempMaxCrop)&&(fTempMaxCrop<fTempMaxDev))
            	{
            	if (fTempMinCrop<=fTempOptDev) //fTempMinCrop < 26.0 < fTempMaxCrop < 34.0
					{
					fTempCorr=(fTempMaxCrop-fTempOptDev)/fTempDiff;
					fDailyThermTime=(float)13.0*((float)1.0+fTempCorr)
									+fTempMinCrop/((float)2.0)*((float)1.0-fTempCorr);
	                }
            	if (fTempMinCrop>fTempOptDev)	//26.0 < fTempMinCrop < fTempMaxCrop < 34.0
            		fDailyThermTime=fTempOptDev;
            	}

            if (fTempMaxCrop>=fTempMaxDev)
            	{
            	if (fTempMinCrop>=fTempOptDev) //26.0 < fTempMinCrop < 34.0 < fTempMaxCrop
					{
					fTempCorr=(fTempMaxCrop-fTempMaxDev)/fTempDiff;
					fDailyThermTime=((float)60.0-fTempMaxCrop)*fTempCorr
									+(fTempOptDev)*((float)1.0-fTempCorr);
					}
	            else //fTempMinCrop < 26.0		//26.0 < fTempMinCrop < 34.0 < fTempMaxCrop < 34.0 < fTempMaxCrop
					{
					fTempCorr=(fTempMaxCrop-fTempMaxDev)/fTempDiff;
					fDailyThermTime=((float)60.0-fTempMaxCrop)*fTempCorr
									+(fTempOptDev)*((float)1.0-fTempCorr);

					fTempCorr=(fTempOptDev-fTempMinCrop)/fTempDiff;
					fDailyThermTime=fDailyThermTime*((float)1.0-fTempCorr)
									+(fTempMinCrop+fTempOptDev)/((float)2.0)*fTempCorr;
		}	}	}	}
	else
	//===========================================================================================
	//For the Crops other than Wheat and Barly, the Daily Thermal Time is calculated as follows:
	//===========================================================================================
		{
		if ((fTempMin>fTempBaseDev)&&(fTempMax<fTempOptDev))
			fDailyThermTime=(fTempMax+fTempMin)/((float)2.0)-fTempBaseDev;
		else
		{
			if (fTempMax<fTempBaseDev)
				fDailyThermTime=(float)0.0;
			else
			{
 				if ((fTempMin<=fTempBaseDev)||(fTempMax>=fTempOptDev))
				{
					fDailyThermTime=(float)0.0;
					for (I=1;I<=8;I++)
					{
						J=(double)I;
						fTempFac=(float)(0.931+0.114*J-0.0703*J*J+0.0053*J*J*J);
						fTemp=fTempMin+fTempFac*(fTempMax-fTempMin);
						if ((fTemp>fTempBaseDev)&&(fTemp<=fTempOptDev))
							fDailyThermTime=fDailyThermTime+(fTemp-fTempBaseDev)/((float)8.0);
						if ((fTemp>fTempOptDev)&&(fTemp<fTempMaxDev))
						{
							if(!lstrcmp(lpCropType,(LPSTR)"SF"))
							{
								fDailyThermTime=fDailyThermTime+(fTempOptDev-fTempBaseDev)*
									(float)((1.0-0.007*pow(fTemp-fTempOptDev,2))/8.0);
							}
							else //z.B. Maize
								fDailyThermTime=fDailyThermTime+(fTempOptDev-fTempBaseDev)*
									((float)1.0-(fTemp-fTempOptDev)/((float)10.0))/((float)8.0);
						}
		}}}}}

	return fDailyThermTime;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

float Emergence(EXP_POINTER)
  {
	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
  	static float fEmergValue; 	   //Thermal time reqired from germination to emergence (Degree.days)

	PDEVELOP pDev=pPl->pDevelop;

	if(pDev->fStageXCERES==(float)0.5)
	{
		fP9 = (float)0.0;
		fEmergValue = (float)0.0;
	}
		

	//=========================================================================================
	//Calculate the SumDTT needed for Emergence for different Crops
	//=========================================================================================
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG"))
		fP9=(float)20.0+((float)6.0)*pMa->pSowInfo->fSowDepth;

	else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI")||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")))
	//	fP9=((float)6.0)*pMa->pSowInfo->fSowDepth;//+(float)15.0;
		fP9=(float)15.0+((float)6.0)*pMa->pSowInfo->fSowDepth; //CERES-MAIZE, page 74

	else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		fP9=(float)40.0+((float)10.2)*pMa->pSowInfo->fSowDepth;

	else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
		fP9=(float)50.0+((float)10.4)*pMa->pSowInfo->fSowDepth;
	
	else if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
		fP9=(float)66.0+((float)11.9)*pMa->pSowInfo->fSowDepth;

	else
		fP9 = (float)200.0;

	//=========================================================================================
	//Calculate Emergence Information
	//=========================================================================================
	if (fP9>(float)150.0)
		fEmergValue=(float)-1.0;
	else
		fEmergValue += pDev->fDTT/fP9;

	//-----------------------------------------------------------------------------
    if(pPl->pModelParam->iEmergenceDay>0) //Wenn "Date of Emergence" in Datenbank oder xnd angegeben, dann erfolgt Feldaufgang an diesem Tag
	{
		//SG 20130516------------------------------------------------------------------
		// Fixen der Emergence nach
		// SUCROS DEVELOPMENT MODEL 
		if(pTi->pSimTime->fTimeAct >= (float)(pPl->pModelParam->iEmergenceDay+1))
		{
			fEmergValue = (float)max(1.05,fEmergValue);
			pDev->fSumDTT = fP9;
		}
		else
				fEmergValue = (float)min(0.9,fEmergValue);
	}
	    
    return fEmergValue;
  }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int Germination(EXP_POINTER)
  	{
	//===========================================
	//Variable Declaration
	//===========================================
  	float 		fCumDepth,fSWSD,fSWSD1,fSWSD2;
  	int			L,NLAYER,iGermination;
  	static int	iDayAftSow;
    float fSowDepth=pMa->pSowInfo->fSowDepth*(float)10.0;//[cm] in [mm]

   	PSLAYER 	pSL	=pSo->pSLayer;
	PSWATER		pSWL=pSo->pSWater;
	PWLAYER	   	pSLW=pWa->pWLayer; 

	if(pPl->pDevelop->fStageXCERES==(float)0.0)
	{
		iDayAftSow = 0;
	}

    NLAYER=pSo->iLayers-2;
	//==============================================
	//Get the Soil Water Pointer in Seed Depth
	//==============================================
	fCumDepth=(float)0.0;
	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	
	for (L=1;L<=NLAYER;L++)
		{
		fCumDepth += pSL->fThickness;
		if (fCumDepth>=fSowDepth)break;
		pSL = pSL->pNext;;
		pSWL = pSWL->pNext;
		pSLW = pSLW->pNext;
		}

	//============================================================
	//Calculating iGermination Possibility Based on Soil Moisture
	//============================================================
	if (pSLW->fContAct>pSWL->fContPWP)
	//Soil water is sufficient to support Germination
		{
		iGermination=1;	//Germination occurs on this day.
		iDayAftSow=1;
		}
	else
	//If pSLW->fContAct<=pSWL->fContPWP,
	// soil water may be a problem
		{
		fSWSD1=(pSLW->fContAct-pSWL->fContPWP)*((float)0.65);
		pSLW = pSLW->pNext;
		pSWL = pSWL->pNext;
		fSWSD2=(pSLW->fContAct-pSWL->fContPWP)*((float)0.35);
		fSWSD=fSWSD1+fSWSD2;

		if (fSWSD>=(float)0.02)	//Soil water is sufficient to support germination
			iGermination=1;		//Germination occurs on this day.
		else                    //Soil water is not sufficient.
			{
			iGermination=0;		//Germination still not occur.

			iDayAftSow++;

			if (iDayAftSow>=40)
			{
			//if ((iDayAftSow<90) && ( (CROP=='WH')||(CROP=='BA') ) )
			if ((iDayAftSow<90) &&
			   ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))))
				iGermination=0; 	//Germination still not occurs
			else
				iGermination=-1;	//Germination fails
		}	}	}

	//=========================================================================================
	//Calculating iGermination Possibility Based on Soil Temperature
	//=========================================================================================
//	pCl->pWeather->fTempAve=(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)/((float)2.0);
//
//	if (pCl->pWeather->fTempAve<=(float)0.0)
//		iGermination=0;

	//SG 20180725------------------------------------------------------------------
	// F�r AgMIP_LI: Fixen der Germination sp�testens am
	// Tag vor dem Feldaufgang (falls in DB gegeben) 
	//-----------------------------------------------------------------------------
    if(pPl->pModelParam->iEmergenceDay>0) //Wenn "Date of Emergence" in Datenbank oder xnd angegeben, dann erfolgt Feldaufgang an diesem Tag
	{
		if (iGermination <= 0)
		{
   			if(pTi->pSimTime->fTimeAct >= (float)(pPl->pModelParam->iEmergenceDay-1))
				iGermination=1;		//Germination occurs on this day.
		}
	}


	return iGermination;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

float PhotoperiodEffect(EXP_POINTER)
  	{
	//=========================================================================================
	//Variable Declaration
	//=========================================================================================
	float 		DF;

	LPSTR lpCropType	=pPl->pGenotype->acCropCode;
	float fPhotoConstant=pPl->pGenotype->fDaylengthCoeff; 
	float fOptDaylength =pPl->pGenotype->fOptDaylen;
	float fDaylength;
	
	fDaylength=DaylengthAndPhotoperiod_SPASS(pLo->pFarm->fLatitude, (int)pTi->pSimTime->iJulianDay,1);
	//=========================================================================================
	//Photoperiod Coefficient
	//=========================================================================================
	if ((!lstrcmp(lpCropType,(LPSTR)"WH"))||(!lstrcmp(lpCropType,(LPSTR)"BA")))
		{
		//if (fDaylength>(float)20.0)	fDaylength=(float)20.0;
		//DF=(float)1.0-fPhotoConstant*((float)20.0-fDaylength)*((float)20.0-fDaylength);
		if (fDaylength>fOptDaylength)	fDaylength=fOptDaylength; //SG 20180626
		DF=(float)1.0-fPhotoConstant*(fOptDaylength-fDaylength)*(fOptDaylength-fDaylength);
		}
    else
        {
		//if (fDaylength<fOptDaylength)
		//	fDaylength=fOptDaylength;

		if (!lstrcmp(lpCropType,(LPSTR)"SG"))
			{
		   	DF=(float)(1.0/92.0);
		   	if (fDaylength>fOptDaylength)
		   		DF=(float)1.0/((float)102.0+fPhotoConstant*(fDaylength-fOptDaylength));
		   	}

		if (!lstrcmp(lpCropType,(LPSTR)"MI"))
			{
			DF=(float)(1.0/68.0);
			if (fDaylength>fOptDaylength)
				DF=(float)1.0/((float)68.0+fPhotoConstant*(fDaylength-fOptDaylength));
			}

		if (!lstrcmp(lpCropType,(LPSTR)"MZ"))
			{
			//fOptDaylength=(float)12.5;
			if (fDaylength<=fOptDaylength)
				fDaylength=fOptDaylength;
			DF=(float)1.0/((float)4.0+fPhotoConstant*(fDaylength-fOptDaylength));
			}

		if (!lstrcmp(lpCropType,(LPSTR)"SF"))
			{
			//fOptDaylength=(float)15.0;
			if (fDaylength>=fOptDaylength)
				fDaylength=fOptDaylength;
			DF=(float)1.0/((float)3.0+fPhotoConstant*(fOptDaylength-fDaylength));
			}
        }

	if (DF>(float)1.0)
		DF=(float)1.0;

	if (DF<(float)0.0)
		DF=(float)0.0;

	return DF;
  }
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

float Vernalization_CERES(EXP_POINTER)
	{
	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	float 	fTempAveCrop;
	double 	VF,VD,VD1,VD2,CumVD;

	PDEVELOP pDev=pPl->pDevelop;

    CumVD=(double)pDev->fCumVernUnit; //Cumulative vernalization till today.

	//=========================================================================================
	//Plant Temperature Calculation
	//=========================================================================================
	//fTempAveCrop is the average plant crown temperature.
    fTempAveCrop=pPl->pPltClimate->fCanopyTemp;

	//=========================================================================================
	//Daily Vernalization Calculation
	//=========================================================================================
    if (pCl->pWeather->fTempMax<=(float)0.0) 		//if fTempMax lower than 0.0C, no vernalization occurs
    	VD=0.0;                 	//VD is the vernalization for a day.
    else                        	//if fTempMax>0.0, vernalization may or may not occur
     	{
		if (pCl->pWeather->fTempMin>(float)15.0)	//if fTempMin>15.0 C, no vernalization occurs.
			VD=0.0;
		else                        //if fTempMin<15.0 C, vernalization may occur.
			{
			if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
				{                   //Vernalization for wheat crops
				VD1=1.4-0.0778*(double)fTempAveCrop;
				VD2=0.5+13.44*
							(double)(fTempAveCrop/((pCl->pWeather->fTempMax-pCl->pWeather->fTempMin+(float)3.0)*
							(pCl->pWeather->fTempMax-pCl->pWeather->fTempMin+(float)3.0)));
				}
			else                    //Vernalization for barly crops ?
				{
				VD1=1.0-0.285*((double)fTempAveCrop-11.0);
				VD2=1.0-0.014*((double)fTempAveCrop-9.0)*((double)fTempAveCrop-9.0);
				}

			VD=max(0.0,min(1.0,min(VD1,VD2)));  //Daily vernalization value.
			}
		}

	//=========================================================================================
	//Cumulative Vernalization Days Calculation
	//=========================================================================================
	CumVD=CumVD+VD;	//Cumulative vernalization value (cumulative vernalization days).

	//=========================================================================================
	//Anti-Vernalization
	//=========================================================================================
    if ((CumVD<10.0)&&(pCl->pWeather->fTempMax>(float)30.0))
    	CumVD=CumVD-0.5*((double)pCl->pWeather->fTempMax-30.0);

    if (CumVD<0.0)
    	CumVD=0.0;

	//=========================================================================================
	//Vernalization Factor
	//=========================================================================================
	//SG/14/06/99:
	//wegen pPl->pGenotype->iVernCoeff = 0 VF immer gleich 1.
	//VF=1.0-(double)pPl->pGenotype->iVernCoeff*(50.0-CumVD);//VF=1.-P1V*(50.-CUMVD) vernalization factor.
	//if(pPl->pGenotype->iVernCoeff > 0)
    VF=min((float)1.0,1.0-(float)(1.0/pPl->pGenotype->iVernCoeff)*(50.0-CumVD));//vernalization factor.
	if (VF<=0.0) 	VF=0.0;//  IF (VF.LE.0.) VF=0.
	if (VF>1.0) 	VF=1.0;//  IF (VF.GT.1.) VF=1.0

    pDev->fVernEff	=(float)VF;
    pDev->fVernUnit	=(float)VD;
    pDev->fCumVernUnit	=(float)CumVD;

	return (float)VF;
   	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	DailyCanopyGrossPhotosynthesis_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	int WINAPI _loadds DailyCanopyGrossPhotosynthesis_CERES(EXP_POINTER)
	{
   	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH")))
		DailyCanopyPhotosynthesis_WH_BA(exp_p);
   	
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"MZ"))
		DailyCanopyPhotosynthesis_MZ(exp_p);

   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SF"))
		DailyCanopyPhotosynthesis_SF(exp_p);

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	CropMaintenance_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds CropMaintenance_CERES(EXP_POINTER)
	{

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	BiomassGrowth_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float fPotShootGrowRate,fPotRootGrowRate,fPotBiomGrowRate,fActBiomGrowRate;
int WINAPI _loadds BiomassGrowth_CERES(EXP_POINTER)
	{       
	static int	iDuration4;
	static int	iDuration5;
	PBIOMASS 	pBiom=pPl->pBiomass;
	PDEVELOP	pDev =pPl->pDevelop;
	PCANOPY		pCan =pPl->pCanopy;
							 
	//wheat, barley:
	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH")))
    {
	//====================================================================================
	//	Organ Death
	//====================================================================================
	pPl->pBiomass->fLeafDeathRate = pBiom->fLeafWeight*((float)0.000267)*pDev->fDTT
					  		 *((float)1.0-pCan->fPlantSenesLA/pCan->fPlantLA);
    pPl->pBiomass->fLeafDeathRate = max((float)0.0,pPl->pBiomass->fLeafDeathRate);
	pPl->pBiomass->fStemDeathRate = pBiom->fStemWeight*((float)0.000267)*pDev->fDTT;
	pPl->pBiomass->fRootDeathRate = ((float)0.4)*pBiom->fRootGrowR
							 +((float)0.005)*pBiom->fRootWeight;
	  					   
	//SG 20120131 - zunehmendes Wurzelabsterben nach einsetzen der Seneszenz
	//pPl->pBiomass->fRootDeathRate = ((float)0.4)*pBiom->fRootGrowR
	//						 +(float)max(0.005,min(0.05,(0.025 - 0.05*pPl->pGenotype->fBeginSenesDvs+0.045*pPl->pDevelop->fStageSUCROS)/(2.0-pPl->pGenotype->fBeginSenesDvs)))*pBiom->fRootWeight;

	  					   
	pPl->pBiomass->fDeadLeafWeight += pPl->pBiomass->fLeafDeathRate;
	pPl->pBiomass->fDeadStemWeight += pPl->pBiomass->fStemDeathRate;
	pPl->pBiomass->fDeadRootWeight += pPl->pBiomass->fRootDeathRate;
		
	//====================================================================================
	//	Organ Dry weight
	//====================================================================================
	pBiom->fLeafWeight += pBiom->fLeafGrowR - pPl->pBiomass->fLeafDeathRate;
	pBiom->fStemWeight += pBiom->fStemGrowR - pPl->pBiomass->fStemDeathRate;
	pBiom->fRootWeight += pBiom->fRootGrowR - pPl->pBiomass->fRootDeathRate;

	pBiom->fGrainWeight+= pBiom->fGrainGrowR;
//	pBiom->fEarWeight	 =pBiom->fGrainWeight;

	//====================================================================================
	//	Leaf Senescence and Biomass Regulation in Stage 1
	//====================================================================================
	if ((pCan->fPlantLA-pCan->fPlantSenesLA)<=(float)0.035*pCan->fTillerNum)
		{
		if ((pDev->iStageCERES<=1)&&(pBiom->fSeedReserv==(float)0.0))
			{
			pBiom->fSeedReserv=(float)0.05*(pBiom->fLeafWeight+pBiom->fRootWeight);

			if(pBiom->fSeedReserv>(float)0.05)
				pBiom->fSeedReserv=(float)0.05;

			pBiom->fLeafWeight=pBiom->fLeafWeight-(float)0.5*pBiom->fSeedReserv;
			pBiom->fRootWeight=pBiom->fRootWeight-(float)0.5*pBiom->fSeedReserv;
	        }
	    }

	//====================================================================================
	//Total weight
	//====================================================================================
	//pBiom->fStoverWeight= pBiom->fLeafWeight+pBiom->fStemWeight+pBiom->fSeedReserv;

	pPl->pBiomass->fTotLeafWeight	=pBiom->fLeafWeight;
	pPl->pBiomass->fTotRootWeight	=pBiom->fRootWeight;
	pPl->pBiomass->fTotStemWeight	=pBiom->fStemWeight;

	pBiom->fBiomassAbvGround	= pBiom->fLeafWeight
						  		 +pBiom->fStemWeight
						  		 +pBiom->fGrainWeight
						  		 +pBiom->fSeedReserv;

	pBiom->fTotalBiomass		= pBiom->fBiomassAbvGround
 						  		 +pBiom->fRootWeight;

	//======================================================================================
	//				Potential Shoot and Root Growth
	//======================================================================================
   /* fPotShootGrowRate = fPotBiomGrowRate
    				    *((float)1.0-pBiom->fRootGrowR/pBiom->fBiomGrowR);
    fPotRootGrowRate  = fPotBiomGrowRate
    					*pBiom->fRootGrowR/pBiom->fBiomGrowR;*/

	// Im Original-CERES steht folgende Berechnung:

	fPotRootGrowRate = fPotBiomGrowRate*((float)60.0-pDev->fStageXCERES*(float)8.0)/(float)100.0;
	fPotShootGrowRate = fPotBiomGrowRate - fPotRootGrowRate;

	//====================================================================================
	//	Minimum stem weight
	//====================================================================================
	if (pDev->iStageCERES==3)
		{
		iDuration4=0;
		iDuration5=0;
		}

	if (pDev->iStageCERES==4)
		iDuration4++;
	if (iDuration4==1)	
		pPl->pModelParam->fMinStemWeight= pBiom->fStemWeight;

	if (pDev->iStageCERES==5)
		iDuration5++;
	}//end wheat, barley

	//MAIZE
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"MZ"))
	{

		pBiom->fRootWeight  += (float)0.5*pBiom->fRootGrowR - 
									(float)0.005*pBiom->fRootWeight;
		pBiom->fStemWeight  += pBiom->fStemGrowR;
		pBiom->fLeafWeight  += pBiom->fLeafGrowR;
		fEarWeight			+= fEarGrowR;
		pBiom->fGrainWeight += pBiom->fGrainGrowR;

		pBiom->fStovWeight  = pBiom->fLeafWeight + pBiom->fStemWeight + fEarWeight;

		pBiom->fBiomassAbvGround = pBiom->fLeafWeight + pBiom->fStemWeight + 
												fEarWeight + pBiom->fGrainWeight;
		pBiom->fTotalBiomass = pBiom->fBiomassAbvGround + pBiom->fRootWeight;
	}


	//SUNFLOWER
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SF"))
	{
		//after anthesis 0.5% of root weight is lost daily as a result of root senescence
		if((pPl->pDevelop->iStageCERES>=4)&&(pPl->pDevelop->iStageCERES<=6))
			pBiom->fRootWeight  += pBiom->fRootGrowR - (float)0.005*pBiom->fRootWeight;
		else
			pBiom->fRootWeight  += pBiom->fRootGrowR;

		pBiom->fLeafWeight  += pBiom->fLeafGrowR;
		pBiom->fStemWeight  += pBiom->fStemGrowR;
		fHeadWeight			+= fHeadGrowR;
		fPericarpWeight		+= fPericarpGrowR;
		fEmbryoWeight		+= fEmbryoGrowR;
		pBiom->fGrainWeight = fEmbryoWeight + fPericarpWeight;

		pBiom->fStovWeight  = pBiom->fLeafWeight + pBiom->fStemWeight + fHeadWeight;

		pBiom->fBiomassAbvGround = pBiom->fLeafWeight + pBiom->fStemWeight + fHeadWeight + 
						fPericarpWeight + pBiom->fGrainWeight;

		if(pDev->iStageCERES <= 2)
			pBiom->fGreenLeafWeight = pBiom->fLeafWeight;

		//calculate Oil percent in the grain
		if(pDev->iStageCERES == 5)
			fOilPercent = (float)100*fOil/pBiom->fGrainWeight;
					

	}//end Sunflower

                
	pBiom->fFruitWeight = pBiom->fGrainWeight;

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	BiomassPartition_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int WINAPI _loadds BiomassPartition_CERES(EXP_POINTER)
	{
   	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH")))
		BiomassPartition_WH_BA(exp_p);

   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"MZ"))
		BiomassPartition_MZ(exp_p);

   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SF"))
		BiomassPartition_SF(exp_p);

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	CanopyFormation_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int WINAPI _loadds CanopyFormation_CERES(EXP_POINTER)
	{    
	PCANOPY	pCan=pPl->pCanopy;
	
   	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH")))
		{
		PlantLeafNumber_WH_BA	(exp_p);
		PlantLeafArea_WH_BA	 	(exp_p);
		LeafSenescence_WH_BA	(exp_p);
		Tillering_WH_BA			(exp_p);
		TillerDeath_WH_BA		(exp_p);
//		ColdHardening_WH_BA		(exp_p);

		//====================================================================================
		//	LAI
		//====================================================================================
		pCan->fLAI=pCan->fPlantGreenLA*(float)0.0001;
		pCan->fLAI=max((float)0.0001,pCan->fLAI);

	    }

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"MZ"))
    	{	
		PlantLeafNumber_MZ	(exp_p);
		LeafSenescence_MZ	(exp_p);

		pCan->fLAI=pCan->fPlantGreenLA*(float)0.0001;
		pCan->fLAI=max((float)0.0001,pCan->fLAI);
		}

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"SF"))
    	{	
		PlantLeafNumber_SF	(exp_p);
		LeafSenescence_SF	(exp_p);

		//Plant Leaf Area (m2/ha)
		pCan->fPlantLA += pPl->pCanopy->fLAGrowR;
		pCan->fPlantGreenLA = pPl->pCanopy->fPlantLA - pPl->pCanopy->fPlantSenesLA;
		pCan->fLAI=pCan->fPlantGreenLA*(float)0.0001;
		pCan->fLAI=max((float)0.0001,pCan->fLAI);

		}

	    
	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantHeightGrowth_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	int PlantHeightGrowth_CERES(EXP_POINTER)
	{
		float fSSL, fSSLmax;
		float fAS, fBS, fHS, fHB;

       
		if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SW"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"TC")))
		{
			fAS = (float)0.0004; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)6.5;  //
			fHS = (float)8;
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
			fAS = (float)0.005; // cm^-1 Chenopodium album (aus INTERCOM)
			fBS = (float)5.0;  //
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

		
		if((pPl->pBiomass->fLeafWeight+pPl->pBiomass->fStemWeight)  > (float)0.0)
            fSSL = pPl->pCanopy->fPlantHeight/((pPl->pBiomass->fLeafWeight+pPl->pBiomass->fStemWeight)/((float)10.0*pMa->pSowInfo->fPlantDens)); //cm/g
		else
			fSSL = (float)0.0;

		fSSLmax = (float)exp(-fAS*pPl->pCanopy->fPlantHeight + fBS);

		if(fSSLmax>fSSL)
		{
			pPl->pCanopy->fPlantHeight += pPl->pDevelop->fDTT*pPl->pGenotype->fPlantHeight*fHS*(float)0.001*fHB
				*(float)exp(-fHS*(float)0.001*pPl->pDevelop->fCumDTT)/(float)pow((1.0+fHB*exp(-fHS*(float)0.001*pPl->pDevelop->fCumDTT)),2)
				*pPl->pPltWater->fStressFacPhoto;
		}

		return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	RootSystemFormation_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
extern int WINAPI _loadds RootExtension_SPASS(EXP_POINTER);
extern int WINAPI _loadds RootLengthDensity_SPASS(EXP_POINTER);
int WINAPI _loadds RootSystemFormation_CERES(EXP_POINTER)
	{
    if (pPl->pDevelop->iStageCERES==9)
    	{
		if(!lstrcmp(pPl->pGenotype->acCropCode,"WH")
			||(!lstrcmp(pPl->pGenotype->acCropCode,"BA")))
	        pPl->pRoot->fDepth +=(float)0.1*pPl->pDevelop->fDTT;
		else
	        pPl->pRoot->fDepth +=(float)0.15*pPl->pDevelop->fDTT;
        }
	
	if (pPl->pBiomass->fRootGrowR<=(float)0.0)
		return 0;

	RootExtension_CERES(exp_p);
	//RootExtension_SPASS(exp_p);
	RootLengthDensity_SPASS(exp_p);
    
	return 1;
	}

///*	                                
int WINAPI _loadds RootExtension_CERES(EXP_POINTER)
	{
	int 				i,L;
	float 				fMaxRootDepthSoil,fMaxRootDepthCrop,DEPMAX,fThickness;
	float 				fDepth,fRelWc,fMoistRed,fNitroRed;
	
	PSLAYER 			pSL		=pSo->pSLayer->pNext;
	PSWATER				pSWL	=pSo->pSWater->pNext;
	PHLAYER				pSLH	=pHe->pHLayer->pNext;
	PWLAYER	   			pSLW	=pWa->pWLayer->pNext; 
	PCLAYER             pSLN    =pCh->pCLayer->pNext;
	PROOT				pRT		=pPl->pRoot;  
	PBIOMASS			pBm		=pPl->pBiomass;    

	//Constant Coefficient
	fMaxRootDepthSoil	= (float)200.0; 	//cm depends on soil property
	//fMaxRootDepthCrop   = (float)150.0; 	//cm depends on crop property
	//SG/20/04/99: 
	//	besser 
	fMaxRootDepthCrop = pPl->pGenotype->fMaxRootDepth;
	//	dann wird der Wert aus What.gtp verwendet!!!
	//	(entsprechend f�r fMaxRootDepthSoil?)

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
	pSLN    =pCh->pCLayer->pNext;
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
			//	habe ich etwas �bersehen?
			//	Aber: in PLAGEN wird immer der aktuelle Wassergehalt der tiefsten
			//	durchwurzelten Schicht verwendet. Deshalb mu� hier pSLW auf pSLW->pNext 
			//	gesetzt werden!!!
			//////////////////////////////////////////////////////////////////////////

			pSLW = pSLW->pNext;
			pSWL = pSWL->pNext;

			pSL = pSL ->pNext;
			pSLH= pSLH->pNext;
			pSLN= pSLN->pNext;
		    }   
		}// for

//	if ((lpBm->fRootGrowRate>(float)0.0)&&(pRT->fDepth<fMaxRootDepthSoil))
	if ((pRT->fDepth<fMaxRootDepthSoil))
		{    
		//Soil water deficit factor for root growth (pLR->fWatStressRoot)
        fRelWc =(pSLW->fContAct-pSWL->fContPWP)/(pSWL->fContFK-pSWL->fContPWP);
        
		if (fRelWc<(float)0.25)
			fMoistRed =max((float)0.0,(float)4.0*fRelWc);
		else
			fMoistRed = (float)1.0;

		//Soil nitrogen deficit factor for root growth
		fNitroRed = (float)((double)1.0-((double)1.17*exp((double)-0.15*
			         (double)(pSLN->fNO3N/pSLN->fNO3Nmgkg+pSLN->fNH4N/pSLN->fNH4Nmgkg))));

		if ((fNitroRed<(float)0.01)&&((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH"))))
			    fNitroRed=(float)0.01;
		else
			if (fNitroRed<(float)0.1) 
			    fNitroRed = (float)0.1;

		//SG/09/04/99//////////////////////////////////////////////////////////////////
		//
		//	In PLAGEN wird, wenn sich die Wurzeln ausschlie�lich in der obersten
		//	Schicht befinden, fMoistRed immer = 1.0 gesetzt!
		///////////////////////////////////////////////////////////////////////////////

		// if(i==1)
		//	fMoistRed = (float)1.0;			//SG/05/05/99: Stre� auch bei i=1!
		
		//pRT->fDepthGrowR = max((float)0.0, pPl->pDevelop->fDTT * (float)0.22 
		//	               * min(pPl->pPltWater->fStressFacPhoto*(float)2.0,fMoistRed));//orig. CERES
	    
		//SG 20111024: bei AgMIP aktiv
		//pRT->fDepthGrowR=max((float)0.0, pPl->pDevelop->fDTT * (float)0.08 
		//	               * min(pPl->pPltWater->fStressFacPhoto*(float)2.0,fMoistRed));//INRA CERES

		//SG 20111024: bei Pareto aktiv
		pRT->fDepthGrowR = max((float)0.0, pPl->pGenotype->fMaxRootExtRate * fNitroRed * fMoistRed);//ep von Enli ???

			               //* fNitroRed * min(pPl->pPltWater->fStressFacPhoto*(float)2.0,fMoistRed));
        //ep 170304 pPl->pPltWater->fStressFacPhoto entspricht pPl->pPltWater->fSWDF1            
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
//*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantSenescense_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantSenescense_CERES(EXP_POINTER)
	{   

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	BiomassAccumulation_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds BiomassAccumulation_CERES(EXP_POINTER)
	{    
	return 1;    
	}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds CERES_TO_COMVR(EXP_POINTER)
	{
    //==========================================================================================
    //Phasic Development
	//==========================================================================================
	int 		i,iStage;
	PDEVELOP 	pDev; 
	float		fValue, VR[10];
//	double		DVR[]={0.0,	0.1425,	0.45,	0.656,	0.91,	1.00,	1.15,	1.50,	1.99,	2.0};
	double		DVR[]={0.0,	0.15,	0.35,	0.55,	0.70,	1.00,	1.25,	1.60,	1.90,	2.0};
	
    for (i=0;i<10;i++)
    	VR[i]=(float)DVR[i];

	iStage 	= pPl->pDevelop->iStageCERES;
	pDev	= pPl->pDevelop;
	
	switch(iStage)
		{
		case 8:	//Sowing to germination  
		case 9:	//Germination to emergence
			pDev->fStageWang= pDev->fStageXCERES;
			pDev->fStageSUCROS	= pDev->fStageXCERES - (float)1.0;
			break;
	
		case 1:	//Emergence to terminal spikelet                                  
			fValue =(VR[1]-VR[0])/(VR[2]-VR[0]);
			 
			if (pDev->fStageXCERES<=(float)1.0+fValue)
				{
				pDev->fStageWang	=(float)1.0 + (pDev->fStageXCERES-(float)1.0)/fValue;
				pDev->fStageSUCROS		=(float)0.0 + (pDev->fStageWang-(float)1.0)*VR[1];
				}
			else
				{
				pDev->fStageWang	= (float)2.0+ (pDev->fStageXCERES-((float)1.0+fValue))
														/((float)1.0-fValue);
				pDev->fStageSUCROS		= VR[1]+(pDev->fStageWang-(float)2.0)*(VR[2]-VR[1]);
				}
			
			break;

		case 2:	//Terminal spikelet to end of leaf growth
			fValue = (VR[3]-VR[2])/(VR[4]-VR[2]);
			
			if (pDev->fStageXCERES<=(float)2.0+fValue)
				{
				pDev->fStageWang = (float)3.0 + (pDev->fStageXCERES-(float)2.0)/fValue;
				pDev->fStageSUCROS	 = VR[2]+(pDev->fStageWang-(float)3.0)*(VR[3]-VR[2]);
			}
			else
			{
				pDev->fStageWang	= (float)4.0+ (pDev->fStageXCERES-((float)2.0+fValue))
														/((float)1.0-fValue);
				pDev->fStageSUCROS		= VR[3]+(pDev->fStageWang-(float)4.0)*(VR[4]-VR[3]);
			}
			
			break;

		case 3:	//End of leaf growth to Heading
			
			fValue = VR[5]-VR[4];
			
			pDev->fStageWang = (float)5.0 + (pDev->fStageXCERES-(float)3.0);
			pDev->fStageSUCROS	 = VR[4]+(pDev->fStageWang-(float)5.0)*fValue;
			
			break;
			
		case 4:	//Heading to Milk development
			fValue =VR[6]-VR[5];
			 
				pDev->fStageWang	=(float)6.0 + (pDev->fStageXCERES-(float)4.0);
				pDev->fStageSUCROS		=VR[5] + (pDev->fStageWang-(float)6.0)*fValue;
			break;


		case 5:	//Milk development(begin of grain filling) to End of grain filling (ripening)
			fValue =(VR[7]-VR[6])/(VR[8]-VR[6]);
			 
				if(pDev->fStageXCERES<=(float)5.0+fValue)
				{
					pDev->fStageWang	=(float)7.0 + ((pDev->fStageXCERES-(float)5.0))/fValue;
					pDev->fStageSUCROS		=VR[6] + (pDev->fStageWang-(float)7.0)*(VR[7]-VR[6]);
				}
				else
				{
					pDev->fStageWang	=(float)8.0 + (pDev->fStageXCERES-((float)5.0+fValue))/((float)1.0-fValue);
					pDev->fStageSUCROS		=VR[7] + (pDev->fStageWang-(float)8.0)*(VR[8]-VR[7]);
				}

			break;

		case 6:
		case 7:
			fValue =VR[9]-VR[8];
			 
				
				pDev->fStageWang = (float)9.0 + (pDev->fStageXCERES-(float)6.0);
				pDev->fStageSUCROS	 = VR[8]+(pDev->fStageWang-(float)9.0)*fValue;
							 		
			break;


		
 /*	case 2:	//Terminal spikelet to end of leaf growth
			fValue = VR[3]-VR[2];
			
			pDev->fStageWang = (float)3.0 + (pDev->fStageXCERES-(float)2.0);
			pDev->fStageSUCROS	 = VR[2]+(pDev->fStageWang-(float)3.0)*fValue;
			
			break;

		case 3:	//End of leaf growth to Heading
			
			fValue = VR[4]-VR[3];
			
			pDev->fStageWang = (float)4.0 + (pDev->fStageXCERES-(float)3.0);
			pDev->fStageSUCROS	 = VR[3]+(pDev->fStageWang-(float)4.0)*fValue;
			
			break;
			
		case 4:	//Heading to Milk development
			fValue =(VR[5]-VR[4])/(VR[6]-VR[4]);
			 
			if (pDev->fStageXCERES<=(float)4.0+fValue)
				{
				pDev->fStageWang	=(float)5.0 + (pDev->fStageXCERES-(float)4.0)/fValue;
				pDev->fStageSUCROS		=VR[4] + (pDev->fStageWang-(float)5.0)
												 *(VR[5]-VR[4]);
				}
			else
				{
				pDev->fStageWang	= (float)6.0+(pDev->fStageXCERES-((float)4.0+fValue))
													/((float)1.0-fValue);
				pDev->fStageSUCROS		= VR[5]+(pDev->fStageWang-(float)6.0)*(VR[6]-VR[5]);
				}
			break;


		case 5:	//Milk development(begin of grain filling) to End of grain filling (ripening)
			fValue =(VR[7]-VR[6])/(VR[8]-VR[6]);
			 
			if (pDev->fStageXCERES<=(float)5.0+fValue)
				{
				pDev->fStageWang	=(float)7.0 + (pDev->fStageXCERES-(float)5.0)/fValue;
				pDev->fStageSUCROS		=VR[6] + (pDev->fStageWang-(float)7.0)
												 *(VR[7]-VR[6]);
				}
			else
				{
				pDev->fStageWang	= (float)8.0+(pDev->fStageXCERES-((float)5.0+fValue))
													/((float)1.0-fValue);
				pDev->fStageSUCROS		= VR[7]+(pDev->fStageWang-(float)8.0)*(VR[8]-VR[7]);
				}
			
			break;

		case 6:
		case 7:
			
			fValue = VR[9]-VR[8];
			
			pDev->fStageWang = (float)9.0 + (pDev->fStageXCERES-(float)6.0);
			pDev->fStageSUCROS	 = VR[8]+(pDev->fStageWang-(float)9.0)*fValue;
			
			break;*/
		}
		
	
	if (pDev->fStageWang<=(float)9.0)
		pDev->fDevStage=pDev->fStageWang;  
	else
		pDev->fDevStage=(float)9.0+(float)0.2*(pDev->fStageWang-(float)9.0);  


	pDev->fDevStage *= (float)10.0;	//SG/14/06/99: wie in SPASS!

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		{
		if (pDev->fStageXCERES<=(float)1.0)
			pDev->fDevStage=pDev->fStageXCERES*(float)10.0;

		if ((pDev->fStageXCERES>(float)1.0)&&(pDev->fStageXCERES<=(float)2.0))
			pDev->fDevStage=(float)10.0+(pDev->fStageXCERES-(float)1.0)*(float)10.0;

		if ((pDev->fStageXCERES>(float)2.0)&&(pDev->fStageXCERES<=(float)3.0))
			pDev->fDevStage=(float)20.0+(pDev->fStageXCERES-(float)2.0)*(float)16.0;

		if ((pDev->fStageXCERES>(float)3.0)&&(pDev->fStageXCERES<=(float)4.0))
			pDev->fDevStage=(float)36.0+(pDev->fStageXCERES-(float)3.0)*(float)24.0;

		if ((pDev->fStageXCERES>(float)4.0)&&(pDev->fStageXCERES<=(float)5.0))
			pDev->fDevStage=(float)60.0+(pDev->fStageXCERES-(float)4.0)*(float)20.0;

		if ((pDev->fStageXCERES>(float)5.0)&&(pDev->fStageXCERES<=(float)6.0))
			pDev->fDevStage=(float)80.0+(pDev->fStageXCERES-(float)5.0)*(float)5.0;

		if ((pDev->fStageXCERES>(float)6.0)&&(pDev->fStageXCERES<=(float)7.0))
			pDev->fDevStage=(float)85.0+(pDev->fStageXCERES-(float)6.0)*(float)4.0;
		
		if (pDev->fStageXCERES>(float)7.0)
			pDev->fDevStage=(float)89.0;
        }

if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
		{
		if (pDev->fStageXCERES<=(float)1.0)
			pDev->fDevStage=pDev->fStageXCERES*(float)10.0;

		if ((pDev->fStageXCERES>(float)1.0)&&(pDev->fStageXCERES<=(float)2.0))
			pDev->fDevStage=(float)10.0+(pDev->fStageXCERES-(float)1.0)*(float)10.0;

		if ((pDev->fStageXCERES>(float)2.0)&&(pDev->fStageXCERES<=(float)3.0))
			pDev->fDevStage=(float)20.0+(pDev->fStageXCERES-(float)2.0)*(float)10.0;

		if ((pDev->fStageXCERES>(float)3.0)&&(pDev->fStageXCERES<=(float)4.0))
			pDev->fDevStage=(float)30.0+(pDev->fStageXCERES-(float)3.0)*(float)6.0;

		if ((pDev->fStageXCERES>(float)4.0)&&(pDev->fStageXCERES<=(float)5.0))
			pDev->fDevStage=(float)50.0+(pDev->fStageXCERES-(float)4.0)*(float)30.0;

		if ((pDev->fStageXCERES>(float)5.0)&&(pDev->fStageXCERES<=(float)6.0))
			pDev->fDevStage=(float)80.0+(pDev->fStageXCERES-(float)5.0)*(float)5.0;

		if ((pDev->fStageXCERES>(float)6.0)&&(pDev->fStageXCERES<=(float)7.0))
			pDev->fDevStage=(float)85.0+(pDev->fStageXCERES-(float)6.0)*(float)4.0;
		
		if (pDev->fStageXCERES>(float)7.0)
			pDev->fDevStage=(float)89.0;
        }



	return 1;
	}     



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//		Biomass Growth Rate
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int DailyCanopyPhotosynthesis_WH_BA(EXP_POINTER)
	{
	double 			PAR,IPAR,TempDay,PRFT,LUEw;
	//SG 20110810:
	float fCO2;
    PCANOPY			pCan	=pPl->pCanopy;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;
    PWEATHER		pWth	=pCl->pWeather;

	if (pPl->pDevelop->fStageSUCROS<(float)0.0) return 0;

	//==========================================================================================
	//	Photosynthetic Active Radiation & Potential Biomass Growth Rate(fPotBiomGrowRate)
	//==========================================================================================
   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")) //for Wheat
 		{

		//==========================================================================================
		//Light Use Efficiency (LUE) / Radiation Use Efficiency (RUE)
		//==========================================================================================

//		LUEw = 6.0; //von CERES: LUEw=9.0; LUEw=7.5; //
		LUEw = 7.5; //von CERES: LUEw=9.0; LUEw=7.5; /Kraichgau 2009: 7.5
	//	LUEw = 9.0; //von CERES: LUEw=9.0; LUEw=7.5;

		
		//SG 20110810:
		//CO2-dependency of light use efficency for AgMIP-project
		fCO2= (fAtmCO2 > 0? fAtmCO2:CO2);
		LUEw = (12.5 * fCO2)/(280.0+fCO2); //LUEw(280 ppm) = 6.25, LUEw(360 ppm) = 7.03, LUEw(720 ppm) = 9.0

	
		//Photosynthetic Active Radiation(PAR MJ/m2 day)
		PAR	=0.5*(double)pWth->fSolRad;
		//Intercepted Photosynthetic Active Radiation IPAR
		IPAR=PAR*(1.0-exp(-0.85*(double)pCan->fLAI));
		//Potential Biomass Growth Rate (g/plant.day)
		fPotBiomGrowRate=(float)(LUEw*pow(IPAR,0.6))*(float)10.0;  //Kg/ha
		
	
		//INRA-Ceres:
		//PAR	=0.48*(double)pWth->fSolRad;
		//fPotBiomGrowRate=(float)(3.3*PAR*(1.0-exp(-0.65*(double)pCan->fLAI))*(float)10.0);
		}
   	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) //for Barley
		{
		PAR	=0.47*(double)pWth->fSolRad;
		IPAR=PAR*(1.0-exp(-0.85*(double)pCan->fLAI));
		fPotBiomGrowRate=(float)(3.5*IPAR)*(float)10.0;			 //Kg/ha
		}
   	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//==========================================================================================
	//	Actual Biomass Growth Rate based on Photosynthetic Reduction Factors
	//==========================================================================================

    //Adjusts photosynthesis rate for variable CO2
	//PCO2  is the CO2 effect coefficeint.
	//	if (ISWCO2==1) 			//ISWCO2 is a switch for CO2 consideration.
	//		fPotBiomGrowRate=fPotBiomGrowRate*PCO2;

	//Photosynthetic reduction due to high or low temperature
	//========================================================================
	//Day Time Temperature (TempDay)
	TempDay=0.25*(double)pWth->fTempMin+0.75*(double)pWth->fTempMax;

	//Temperature reduction factor (PRFT)
	PRFT=1.0-0.0025*(TempDay-18.0)*(TempDay-18.0);

	if (pWth->fTempMin<-(float)3.0) PRFT=0.0;

	PRFT=max((float)0.0, PRFT);
    //==========================================================================
    //Actual Biomass Growth Rate
    //==========================================================================
	pBiom->fBiomGrowR=fPotBiomGrowRate
							*(float)PRFT*min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto);
	
	pBiom->fBiomGrowR =max(pBiom->fBiomGrowR, (float)0.0001);

	return 1;
	}


int DailyCanopyPhotosynthesis_MZ(EXP_POINTER)
	{
	double 			PAR,IPAR,TempDay,PRFT,LUEw;
    PCANOPY			pCan	=pPl->pCanopy;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;
    PWEATHER		pWth	=pCl->pWeather;

	//SG 20140909:
	float fCO2, fCO2Effect;

	if (pPl->pDevelop->fStageSUCROS<(float)0.0) return 0;

	//==========================================================================================
	//Light Use Efficiency (LUE) / Radiation Use Efficiency (RUE)
	//==========================================================================================
if (pPl->pDevelop->fStageSUCROS<(float)0.0) return 0;

	//==========================================================================================
	//Light Use Efficiency (LUE) / Radiation Use Efficiency (RUE)
	//==========================================================================================

	LUEw=5.0; //g/MJ (= 5.0 CERES 2.0); (= 4.33 CERES 3.0);

	//*******************************************************
	//SG 20140909:
	//CO2-dependency of light use efficency for AgMIP-project
	// nach DSSAT Crop Modelling Course Summer 2007
	fCO2= (fAtmCO2 > 0? fAtmCO2:CO2);

	if (fCO2 <= (float)0.0)
	{
		fCO2Effect = 1.0;
	}
	else if(fCO2 < (float)280)
	{
		fCO2Effect = 0.8 * fCO2 / 280;
	}
	else if (fCO2 <= (float)330)
	{
		fCO2Effect = (fCO2-280)/(330-280)*0.2+ 0.8;
	}
	else if (fCO2 <= (float)450)
	{
		fCO2Effect = (fCO2-330)/(450-330)*0.2+ 1.0;
	}
	else
	{
		fCO2Effect = (fCO2-450)/(1000-450)*0.3+ 1.2;;
	}

	if (fCO2Effect > (float)1.5) 
		fCO2Effect = (float)1.5;

	LUEw = LUEw * fCO2Effect;

	//*******************************************************

	//==========================================================================================
	//	Photosynthetic Active Radiation & Potential Biomass Growth Rate(fPotBiomGrowRate)
	//==========================================================================================
	    //Photosynthetic Active Radiation(PAR MJ/m2 day)
		PAR	=0.5*(double)pWth->fSolRad;
		//Intercepted Photosynthetic Active Radiation IPAR
		IPAR=PAR*(1.0-exp(-0.65*(double)pCan->fLAI));
		//Potential Biomass Growth Rate (g/plant.day)
		fPotBiomGrowRate=(float)(LUEw*IPAR)*(float)10.0;  //Kg/ha

   	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


	//Photosnythetic reduction due to high or low temperature
	//========================================================================
	//Day Time Temperature (TempDay)
	TempDay=0.25*(double)pWth->fTempMin+0.75*(double)pWth->fTempMax;

	//Temperature reduction factor (PRFT)
	PRFT=1.0-0.0025*(TempDay-26.0)*(TempDay-26.0);

	PRFT=max((float)0.0, PRFT);

    //==========================================================================
    //Actual Biomass Growth Rate
    //==========================================================================
	pBiom->fBiomGrowR=fPotBiomGrowRate
							*(float)(min(PRFT,min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto)));

	pBiom->fBiomGrowR =max(pBiom->fBiomGrowR, (float)0.0001);

	return 1;
	}


int DailyCanopyPhotosynthesis_SF(EXP_POINTER)
	{
	double 	PAR,IPAR,TempDay,PRFT;
	float	fQN, fQD, fK1, fK2, fC1, fC2;
    PCANOPY			pCan	=pPl->pCanopy;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;
    PWEATHER		pWth	=pCl->pWeather;

	if (pPl->pDevelop->fStageSUCROS<(float)0.0) return 0;

	//==========================================================================================
	//	Photosynthetic Active Radiation & Potential Biomass Growth Rate(fPotBiomGrowRate)
	//==========================================================================================

	//Photosynthetic Active Radiation(PAR MJ/m2 day)
	PAR	=0.5*(double)pWth->fSolRad;

	//Extinction coefficient for PAR (fK2):	
	fQN = (float)(1.0-exp(-0.86*pCan->fLAI));
	fQD = (float)(2*fQN/(1.0+fQN));
	if(pCan->fLAI>(float)0.001)
		fK2 = (float)(-log(1.0-fQD)/pCan->fLAI);
	else
		fK2 = (float)3.0;

	fK2 = (float)min(3.0,fK2);
	if(pPl->pDevelop->iStageCERES>4) fK2=(float)1.0;

	//Intercepted Photosynthetic Active Radiation IPAR
	IPAR=PAR*(1.0-exp((double)(-fK2*pCan->fLAI)));


	//Calculate RUE and potential biomass assimilation rate:
	//fK1 = radiation use efficiency [g(biomass)/MJ(PAR)]
	fK1 = (float)(1.4+1.8*(1.0-exp(-0.5*pCan->fLAI)));
	fK1 = (float)(min(3.0,fK1));


	if(pPl->pDevelop->iStageCERES>=4)
	{
		fC1 = (float)0.8;
		if(pCan->fGrainNum==(float)0.0)
			fC2 = (float)max(0.6,0.8-pPl->pDevelop->fSumDTT/140.0 * 0.2);
		else
			fC2 =(float)max(0.4,0.6-(pPl->pDevelop->fSumDTT-230)/140.0 * 0.2);
	}

	if(pPl->pDevelop->iStageCERES<4)
		fPotBiomGrowRate=(float)(fK1*IPAR)*(float)10.0;  //kg/ha
	else
		fPotBiomGrowRate = (float)max(0.0, IPAR*fRI1*fC2/fC1*fK1*10.0 - fRM);

  	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	//==========================================================================================
	//	Actual Biomass Growth Rate based on Photosynthetic Reduction Factors
	//==========================================================================================

	//Photosynthetic reduction due to high or low temperature
	//========================================================================
	//Day Time Temperature (TempDay)
	TempDay=0.25*(double)pWth->fTempMin+0.75*(double)pWth->fTempMax;


	if((TempDay<(float)4.0)||(TempDay>(float)45.0))
		PRFT = (float)0.0;
	else if(TempDay<(float)17.0)
		PRFT = (TempDay-(float)4.0)/(float)13.0;
	else if(TempDay <= (float)31.0)
		PRFT = (float)1.0;
	else
		PRFT = ((float)45.0-TempDay)/(float)14.0;

	PRFT=max((float)0.0, PRFT);


    //==========================================================================
    //Actual Biomass Growth Rate
    //==========================================================================
	pBiom->fBiomGrowR=fPotBiomGrowRate
							*(float)min(min(PRFT,pPltW->fStressFacPhoto),pPltN->fNStressPhoto);

	pBiom->fBiomGrowR =max(pBiom->fBiomGrowR, (float)0.0001);

	
	//calculate a factor based on the red/far red - ratio to reduce 
	//potential leaf area growth:
	fRFR = (float)(exp(-0.5*fK2*pCan->fLAI));
	if(fRFR>(float)0.5)
		fRFR = (float)1.0;
	else
		fRFR = (float)2.0*fRFR;


	//calculate the number of leaves that have appeared (LN):


	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//		Biomass Partitioning Function of Wheat and Barley
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float fTopFraction;
int BiomassPartition_WH_BA(EXP_POINTER)
	{
	int i;
	float 			AWR,PTF,RGFILL;//EGFT,CLG,PLAGMS;
    PDEVELOP 		pDev	=pPl->pDevelop;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PCANOPY			pCan	=pPl->pCanopy;
    PMODELPARAM		pPltMP	=pPl->pModelParam;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;  
    PWEATHER		pWth	=pCl->pWeather;
	PSTAGEPARAM		pStageParam=pPl->pGenotype->pStageParam;

	if (pPl->pDevelop->fStageSUCROS<(float)0.0)	return 0;

	//For Barley special factor
	if (pWth->fSolRad<=(float)10.0)
		pPltMP->fBarleyGrwParam=(float)(1.0-pow(((double)pWth->fSolRad-10.0),2.0)
									*(double)pMa->pSowInfo->fPlantDens*2.5E-5);
	else
		pPltMP->fBarleyGrwParam=(float)1.0;



	switch (pDev->iStageCERES)
		{
		case 1: 	
			//=================================================================================
			//Stage 1. Emergence-Terminal Spikelet: Leaf, Root Growth
			//=================================================================================
			if (pDev->fDTT<=(float)0.0)	PTF=(float)0.5;

			//Assimilation area to weight ratio (AWR m2/kg)
	    	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))//for Wheat
/* Enli*/ 	//  AWR=(float)80.0-(float)0.075*pDev->fCumTDU;
			//  AWR=(float)150.0-(float)0.075*pDev->fCumTDU;
			// INRA-CERES 				
			    AWR=(float)240.0-(float)0.075*pDev->fCumTDU;

	    	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) 		//for Barley
/* Enli		AWR=((float)80.0-((float)0.075)*pDev->fCumTDU)*((float)1.1);*/
			//	AWR=((float)150.0-((float)0.075)*pDev->fCumTDU)*((float)1.1);
			// INRA-CERES 				
			   AWR=(float)240.0-(float)0.075*pDev->fCumTDU;


	       AWR = AWR * (float)0.1;                        
	       
/*	SG 2005 11 03: auskommentiert, weil bereits am Vortag in PlantLeafArea_WH_BA berechnet
			//==================================================================================
			//	Effect of Temperature on Leaf Extension Growth (EGFT)
			//==================================================================================
			EGFT=(float)1.0;
					
			if (pDev->iStageCERES<=2)
				{
				pWth->fTempAve=(pWth->fTempMax+pWth->fTempMin)/((float)2.0);
					
				EGFT=(float)1.2-(float)0.0042*(pWth->fTempAve-(float)17.0)
											 *(pWth->fTempAve-(float)17.0);
					
				EGFT = max((float)0.0, min((float)1.0,EGFT));
				}
					
	        //Plant leaf area growth rate on the main stem (PLAGMS: cm2/plant.day)
			CLG	  =(float)7.5;
			PLAGMS=CLG*(float)sqrt(pDev->fCumPhyll)*pCan->fLeafAppearR
					  					*min(pPltW->fStressFacLeaf,min(EGFT,pPltN->fNStressLeaf));

            //Plant leaf area growth rate (cm2/plant.day)
			pCan->fLAGrowR=PLAGMS*((float)0.3+(float)0.7*pCan->fTillerNum);

			//Area growth rate(cm2/plant.day) are calculated based on 900 tillers
			//when tiller number more than 900
			if (pCan->fTillerNumSq>(float)900.0)
				pCan->fLAGrowR=PLAGMS*((float)900.0)/pMa->pSowInfo->fPlantDens;
            
            //Leaf Area Growth Rate (m2/ha.day)
            pCan->fLAGrowR *= pMa->pSowInfo->fPlantDens;
            
 */           
			//Leaf and root biomass growth rate  (kg/ha.day)
			pBiom->fLeafGrowR=pCan->fLAGrowR/AWR;
			pBiom->fRootGrowR=pBiom->fBiomGrowR-pBiom->fLeafGrowR;

			//Regulation of  Plant Leaf and Root Growth Rate based on Seed Reserv
			if (pBiom->fRootGrowR<=(float)0.35*pBiom->fBiomGrowR)
				{
				pBiom->fRootGrowR=(float)0.35*pBiom->fBiomGrowR;

				if (pBiom->fSeedReserv>(float)0.0)
					pBiom->fSeedReserv += pBiom->fBiomGrowR
										  -pBiom->fLeafGrowR
										  -pBiom->fRootGrowR;

				if (pBiom->fSeedReserv<(float)0.0)	pBiom->fSeedReserv=(float)0.0;

				if (pBiom->fSeedReserv==(float)0.0)
					{
					pBiom->fLeafGrowR	=pBiom->fBiomGrowR*((float)0.65);
					pCan->fLAGrowR=pBiom->fLeafGrowR*AWR;
					}
				}

   			pBiom->fStemGrowR	=(float)0.0;
// 			pBiom->fEarGrowRate	=(float)0.0;
   			pBiom->fGrainGrowR	=(float)0.0;
			//=================================================================================
			//Plant Top Fraction (%)
			//=================================================================================
			PTF = pBiom->fLeafGrowR/(pBiom->fBiomGrowR+pBiom->fSeedReserv
										+(float)1.0E-9);

			break;

		case 2:	
			//=================================================================================
			//Stage 2. Terminal Spikelet-End of Leaf Growth: Leaf, Root & Stem Growth 
			//=================================================================================
		   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")) 		//for Wheat
				PTF=(float)0.70+((float)0.1)*min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

		   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) 				//for Barley
				PTF=(float)0.70+((float)0.1)
						*min(pPltW->fStressFacPhoto,min(pPltN->fNStressLeaf,pPltMP->fBarleyGrwParam));

			//Root growth rate (g/plant.day)
			pBiom->fRootGrowR=((float)1.0-PTF)*pBiom->fBiomGrowR;

			//Stem growth rate (g/plant.day)
	//		pBiom->fStemGrowR=((float)0.15+(float)0.12*pDev->fSumDTT
	//									//	  /pPl->pGenotype->fPhyllochronInterval)
	//										  /fPHINT)
	//										  *pBiom->fBiomGrowR*PTF; 
			//INRA-Ceres:
			pBiom->fStemGrowR=((float)0.15+(float)0.15*pDev->fSumDTT/fPHINT)*pBiom->fBiomGrowR*PTF;

			//Leaf growth rate (g/plant.day)
			pBiom->fLeafGrowR=pBiom->fBiomGrowR-pBiom->fRootGrowR
													    -pBiom->fStemGrowR;
//			pBiom->fEarGrowRate	=(float)0.0;
   			pBiom->fGrainGrowR	=(float)0.0;

			break;

		case 3:
			//=================================================================================
			//Stage 3: End of Leaf Growth - End of Pre-Anthesis Ear Growth: Root Stem Growth
			//=================================================================================
		   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")) //for Wheat
				PTF=(float)0.75+((float)0.1)*min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

		   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) 		//for Barley
				PTF=(float)0.75+((float)0.1)
						*min(pPltW->fStressFacPhoto,min(pPltN->fNStressLeaf,pPltMP->fBarleyGrwParam));

			//Biomass Growth Rate
			pBiom->fRootGrowR	=((float)1.0-PTF)*pBiom->fBiomGrowR;
			pBiom->fStemGrowR	=pBiom->fBiomGrowR*PTF;

   			pBiom->fLeafGrowR	=(float)0.0;
//   			pBiom->fEarGrowRate	=(float)0.0;
   			pBiom->fGrainGrowR	=(float)0.0;

            break;

		case 4:
			//=================================================================================
			//Stage 4: End of Pre-Anthesis Ear Growth - Begin of Grain Filling: Root Stem Growth
			//=================================================================================
		   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")) //for Wheat
				PTF=(float)0.80+((float)0.1)*min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

		   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) 		//for Barley
				PTF=(float)0.80+((float)0.1)
						*min(pPltW->fStressFacPhoto,min(pPltN->fNStressLeaf,pPltMP->fBarleyGrwParam));

			//Biomass Growth Rate
			pBiom->fRootGrowR	=pBiom->fBiomGrowR*((float)1.0-PTF);
			pBiom->fStemGrowR	=pBiom->fBiomGrowR*PTF;

   			pBiom->fLeafGrowR	=(float)0.0;
//   			pBiom->fEarGrowRate	=(float)0.0;
   			pBiom->fGrainGrowR	=(float)0.0;

			break;

		case 5:
			//=================================================================================
			//Grain Filling Stage: Grain Growth
			//=================================================================================
            PTF=pPltMP->fMinStemWeight/(pBiom->fStemWeight+(float)1.0E-9)
            	*(float)0.35+(float)0.65;

			if (PTF>(float)1.0)	PTF=(float)1.0;

			i=1;
			//while (i<5) pStageParam=pStageParam->pNext;

			if (pBiom->fBiomGrowR==(float)0.0)
					pDev->fSumDTT=pStageParam->afThermalTime[4];

			//Biomass Growth Rate
			pBiom->fBiomGrowR=pBiom->fBiomGrowR
									*((float)1.0-((float)1.2-
									  (float)0.8*pPltMP->fMinStemWeight/pBiom->fStemWeight)
					   			 	*(pDev->fSumDTT+(float)100.0)/
				   					(pStageParam->afThermalTime[4]+(float)100.0));

			pBiom->fRootGrowR=pBiom->fBiomGrowR*((float)1.0-PTF);

			//Relative Grain Filling Rate (RGFILL)
			if (pWth->fTempAve<=(float)10.0)
				RGFILL=((float)0.065)*pWth->fTempAve;

			if (pWth->fTempAve>(float)10.0)
				RGFILL=  (float)0.65
						+((float)0.0787-(float)0.00328*(pWth->fTempMax-pWth->fTempMin))
						*(float)pow(((double)pWth->fTempAve-10.0),0.8);

			if (RGFILL>(float)1.0)
				RGFILL=(float)1.0;

            //Grain Growth Rate (g/plant.day)
			//SG/14/06/99:
			//pBiom->fGrainGrowR=RGFILL*pCan->fGrainNum*pPl->pGenotype->fGrainFillCoeff*(float)0.001;
			pBiom->fGrainGrowR=RGFILL*pCan->fGrainNum*pPl->pGenotype->fRelStorageFillRate*(float)0.001;

            //Grain Growth Rate (kg/ha.day)
			pBiom->fGrainGrowR *= (float)10.0*pMa->pSowInfo->fPlantDens; 

            //Stem Growth Rate
			pBiom->fStemGrowR=pBiom->fBiomGrowR*PTF-pBiom->fGrainGrowR;

/*
       		if (pBiom->fStemGrowR<(float)0.0)
       			{
				if (pBiom->fStemWeight+pBiom->fStemGrowR<pPltMP->fMinStemWeight)
					{
	    			pBiom->fStemGrowR	=pPltMP->fMinStemWeight-pBiom->fStemWeight;
					pBiom->fGrainGrowR	= pBiom->fBiomGrowR*PTF-pBiom->fStemGrowR;
			   		}

				}
*/

			//INRA-Ceres
       		
				if (pBiom->fStemWeight<pPltMP->fMinStemWeight)
					{
	    			pBiom->fStemGrowR	=(float)0.0;
					if(pBiom->fBiomGrowR*PTF<pBiom->fGrainGrowR)
						pBiom->fGrainGrowR	= pBiom->fBiomGrowR*PTF;
			   		}

				



   			pBiom->fLeafGrowR	=(float)0.0;

			break;
		}

	//====================================================================================
	//	Organ Fraction
	//====================================================================================
	pBiom->fPartFracLeaf	= pBiom->fLeafGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracRoot	= pBiom->fRootGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracStem	= pBiom->fStemGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracFruit	= pBiom->fGrainGrowR/pBiom->fBiomGrowR;
//	pBiom->fEarFraction	    = pBiom->fEarGrowRate/pBiom->fBiomGrowR;
    pBiom->fPartFracGrain   = pBiom->fPartFracFruit;
	fTopFraction 	= (float)1.0-pBiom->fPartFracRoot;
	return 1;
	}

int BiomassPartition_MZ(EXP_POINTER)
	{
	int I;
	float fTempFac, fTemp;
	float fNewLeafWeight, fGRF, fExcess;

	PGENOTYPE		pGen	=pPl->pGenotype;
    PDEVELOP 		pDev	=pPl->pDevelop;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PCANOPY			pCan	=pPl->pCanopy;
    PMODELPARAM		pPltMP	=pPl->pModelParam;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;  
    PWEATHER		pWth	=pCl->pWeather;
	PSTAGEPARAM		pStageParam=pPl->pGenotype->pStageParam;


	if (pPl->pDevelop->fStageSUCROS<(float)0.0)	return 0;

	pCan->fLAGrowR = (float)0.0;

	if(pDev->iStageCERES<=3)
	{
		if(pCan->fExpandLeafNum < (float)4.0)
		{
			//[cm^2/plant]
			pCan->fLAGrowR = (float)3.0*pCan->fExpandLeafNum*pCan->fLeafAppearR;
			pCan->fLAGrowR *= (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		}
		else if ((pCan->fExpandLeafNum >= (float)4.0)&&(pCan->fExpandLeafNum <= (float)12.0))
		{
			pCan->fLAGrowR = (float)(3.5*pow(pCan->fExpandLeafNum,2)*pCan->fLeafAppearR);
			pCan->fLAGrowR *= (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		}
		else if ((pCan->fExpandLeafNum >= (float)12.0)&&(pCan->fExpandLeafNum < pPltMP->fMaxLeafNum-(float)3.0))
		{
			pCan->fLAGrowR = (float)(3.5*170.0*pCan->fLeafAppearR);
			pCan->fLAGrowR *= (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		}
		else 
		{
			pCan->fLAGrowR = (float)(3.5*170.0*pCan->fLeafAppearR/sqrt(pCan->fExpandLeafNum+ 5.0 -pPltMP->fMaxLeafNum));
			pCan->fLAGrowR *= (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
		}
	}

	//[m^2/ha]
	pCan->fPlantLA += pPl->pCanopy->fLAGrowR * pMa->pSowInfo->fPlantDens;

	switch (pDev->iStageCERES)
		{
		case 1: //only leaves and roots growth

			//[g/plant]
			fNewLeafWeight = (float)pow((pCan->fPlantLA/pMa->pSowInfo->fPlantDens)/267.0,1.25);//CERES-Maize, p.78
			//[kg/ha]
			fNewLeafWeight *= (float)10.0*pMa->pSowInfo->fPlantDens;

			pBiom->fLeafGrowR = (float)max(0.0,fNewLeafWeight - pBiom->fLeafWeight);
			pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR;

			//regulation based on seed reserv
			if(pBiom->fRootGrowR <= (float)0.25*pBiom->fBiomGrowR)
			{
				pBiom->fRootGrowR = (float)0.25*pBiom->fBiomGrowR;
				pBiom->fSeedReserv += pBiom->fBiomGrowR - pBiom->fLeafGrowR - pBiom->fRootGrowR;

				if(pBiom->fSeedReserv < (float)0.0)
				{
					pBiom->fSeedReserv = (float)0.0;
					pBiom->fLeafGrowR = (float)0.75*pBiom->fBiomGrowR;
					
					//[cm^2/plant]
					pCan->fPlantLA = (float)(267.0*pow((pBiom->fLeafWeight+pBiom->fLeafGrowR)
										/(10.0*pMa->pSowInfo->fPlantDens),0.8));
					//[m^2/ha]
					pCan->fPlantLA *= pMa->pSowInfo->fPlantDens;
				}
			}


   			pBiom->fStemGrowR	=(float)0.0;
			fEarGrowR			=(float)0.0;
			pBiom->fGrainGrowR	=(float)0.0;

			break;

		case 2:	//only leaves and root Growth

			//[g/plant]
			fNewLeafWeight = (float)pow((pCan->fPlantLA/pMa->pSowInfo->fPlantDens)/267.0,1.25);//CERES-Maize, p.78
			//[kg/ha]
			fNewLeafWeight *= (float)10.0*pMa->pSowInfo->fPlantDens;
			pBiom->fLeafGrowR = (float)max(0.0,fNewLeafWeight - pBiom->fLeafWeight);

			if(pBiom->fLeafGrowR >= (float)0.75*pBiom->fBiomGrowR)
			{
				pBiom->fLeafGrowR = (float)0.75*pBiom->fBiomGrowR;

				//[cm^2/plant]
				pCan->fPlantLA = (float)(267.0*pow((pBiom->fLeafWeight+pBiom->fLeafGrowR)
									/(10.0*pMa->pSowInfo->fPlantDens),0.8));
				//[m^2/ha]
				pCan->fPlantLA *= pMa->pSowInfo->fPlantDens;
			}

			fTasselLeafNum = pCan->fExpandLeafNum;

			pBiom->fRootGrowR	= pBiom->fBiomGrowR - pBiom->fLeafGrowR;
			pBiom->fStemGrowR	= (float)0.0;			
			fEarGrowR			= (float)0.0;
   			pBiom->fGrainGrowR	= (float)0.0;

			break;

		case 3://Leaf, Root and Stem Growth

			//[g/plant]
			pBiom->fLeafGrowR = (float)0.00116 * pCan->fLAGrowR *
				(float)pow(pCan->fPlantLA/pMa->pSowInfo->fPlantDens,0.25);
			//[kg/ha]
			pBiom->fLeafGrowR *= (float)10.0*pMa->pSowInfo->fPlantDens;

			if(pCan->fExpandLeafNum < pPltMP->fMaxLeafNum - (float)3.0)
			{
				pBiom->fStemGrowR = pBiom->fLeafGrowR * (float)0.0182 *
					(float)pow(pCan->fExpandLeafNum-fTasselLeafNum,2.0);
			}
			else
			{
				//[kg/ha]
				pBiom->fStemGrowR = (float)(3.1*3.5)*pCan->fLeafAppearR*(float)10.0*pMa->pSowInfo->fPlantDens;
				pBiom->fStemGrowR *= (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
			}

			pBiom->fRootGrowR = pBiom->fBiomGrowR-pBiom->fLeafGrowR-pBiom->fStemGrowR;

			//Regulation
			if(pBiom->fRootGrowR <= (float)0.10*pBiom->fBiomGrowR)
			{
				if((pBiom->fLeafGrowR>(float)0.0)||(pBiom->fStemGrowR>(float)0.0))
				{
					fGRF = (float)0.92*pBiom->fBiomGrowR/(pBiom->fLeafGrowR+pBiom->fStemGrowR);
					pBiom->fRootGrowR = (float)0.10*pBiom->fBiomGrowR;
				}
				else
					fGRF = (float)0.0;

				pBiom->fLeafGrowR *= fGRF;
				pBiom->fStemGrowR *= fGRF;
			}


			fEarGrowR			=(float)0.0;
   			pBiom->fGrainGrowR	=(float)0.0;

            break;

		case 4: //since now ear growth, leaf growth ceases
			
			//[g/plant]
			fEarGrowR = (float)(0.22*pDev->fDTT * min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf));
			//[kg/ha]
			fEarGrowR *= (float)10.0*pMa->pSowInfo->fPlantDens;

			pBiom->fStemGrowR = (float)0.4*fEarGrowR;
			pBiom->fRootGrowR = pBiom->fBiomGrowR-fEarGrowR-pBiom->fStemGrowR;

			//regulation
			if(pBiom->fRootGrowR <= (float)0.08*pBiom->fBiomGrowR)
			{
				if((fEarGrowR>(float)0.0)||(pBiom->fStemGrowR>(float)0.0))
				{
					fGRF = (float)0.92*pBiom->fBiomGrowR/(fEarGrowR+pBiom->fStemGrowR);
					pBiom->fRootGrowR = (float)0.08*pBiom->fBiomGrowR;
				}
				else
					fGRF = (float)0.0;

				fEarGrowR		  *= fGRF;
				pBiom->fStemGrowR *= fGRF;
			}


   			pBiom->fLeafGrowR	=(float)0.0;
   			pBiom->fGrainGrowR	=(float)0.0;

			fSumPS += pBiom->fBiomGrowR;
			iDurP++;


			break;

		case 5: //grain filling


			pBiom->fRelGrainFillR = (float)0.0;

		for (I=1;I<=8;I++)
		{
			fTempFac=(float)(0.931+0.114*I-0.0703*I*I+0.0053*I*I*I);
			fTemp=pWth->fTempMin+fTempFac*(pWth->fTempMax-pWth->fTempMin);

			if(fTemp > (float)6.0)
				pBiom->fRelGrainFillR += (float)((1.0-0.0025*pow(fTemp-26.0,2.0))/8.0);
		}

		// [g/plant/d]
		pBiom->fGrainGrowR = pBiom->fRelGrainFillR * pCan->fGrainNum *
			pGen->fRelStorageFillRate * (float)(0.001*(0.45+0.55*pPltW->fStressFacPhoto));
		// [kg/ha]
		pBiom->fGrainGrowR *= pMa->pSowInfo->fPlantDens * (float)10.0;

		fExcess = pBiom->fBiomGrowR - pBiom->fGrainGrowR;

		//regulation, based on stem and leaves reserve
		if(fExcess >= (float)0.0)
		{
			pBiom->fStemGrowR = (float)0.5*fExcess;
			pBiom->fRootGrowR = (float)0.5*fExcess;
			pBiom->fLeafGrowR = (float)0.0;
		}
		else
		{				
			pBiom->fStemGrowR = fExcess;
			pBiom->fRootGrowR = (float)0.0;
			
			if(pBiom->fStemWeight - fExcess >= pPltMP->fMinStemWeight*(float)1.07)
				pBiom->fLeafGrowR = (float)0.0;
			else
			{
				if(pBiom->fLeafWeight > pPltMP->fMinLeafWeight)
				{
					pBiom->fStemGrowR = fExcess + pBiom->fLeafWeight * (float)0.005;
					pBiom->fLeafGrowR = -pBiom->fLeafWeight * (float)0.005;
				}
				
				if(pBiom->fStemWeight+pBiom->fStemGrowR < pPltMP->fMinStemWeight)
				{
					pBiom->fStemGrowR = pPltMP->fMinStemWeight - pBiom->fStemWeight;
					pBiom->fGrainGrowR = pBiom->fBiomGrowR;
				}
			}
		}

		if(pBiom->fStemWeight+pBiom->fStemGrowR > pPltMP->fMaxStemWeight)
			pBiom->fStemGrowR = (float)max(0.0,pPltMP->fMaxStemWeight - pBiom->fStemWeight);

	//	fEarGrowR = pBiom->fGrainGrowR;
		fEarGrowR = (float)0.0;


		//additional condition for end of grain filling:
		//when grain filling is very slow for two consecutive days,
		//physiological maturity occurs
		if(pBiom->fRelGrainFillR<=(float)0.1)
		{
			iSlowFillDay++;
			if(iSlowFillDay!=1)
				pDev->fSumDTT = (float)1000.0;
			else
				iSlowFillDay = 0;
		}


		break;
		}

	//====================================================================================
	//	Organ Fraction
	//====================================================================================
	pBiom->fPartFracLeaf	= pBiom->fLeafGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracRoot	= pBiom->fRootGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracStem	= pBiom->fStemGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracFruit	= pBiom->fGrainGrowR/pBiom->fBiomGrowR;
    pBiom->fPartFracGrain   = pBiom->fPartFracFruit;
	fEarFraction			= fEarGrowR/pBiom->fBiomGrowR;
	fTopFraction 			= (float)1.0-pBiom->fPartFracRoot;

	return 1;
	}

int BiomassPartition_SF(EXP_POINTER)
	{
	int I;
	float PLAG, fSLAX, fSLAMax, fSLAMin, LeafWeight, fMinGrowLeaf, fracCarb;
	float fTempRedFac, fTemp, fTempFac, fELOFT, fPEPE, fExcess;
	float fCPool, fCPool1, fCPool2, fPool1, fPool2, fCP, fCP2, fPoolFactor;
	float fCDemandTot, fCDemandGrain;
	

    PDEVELOP 		pDev	=pPl->pDevelop;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PCANOPY			pCan	=pPl->pCanopy;
    PMODELPARAM		pPltMP	=pPl->pModelParam;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;  
    PWEATHER		pWth	=pCl->pWeather;
	PSTAGEPARAM		pStageParam=pPl->pGenotype->pStageParam;

	if (pPl->pDevelop->fStageSUCROS<(float)0.0)	return 0;

	LeafWeight = pBiom->fLeafWeight/pMa->pSowInfo->fPlantDens * (float)0.1; // [g/plant]
	fSLAX = (float)(1.0/sqrt(2.77E-5 - 2.007E-7 * LeafWeight));// [cm^2/g]
	fSLAMax = (float)1.4 * fSLAX; 
	fSLAMin = (float)1.0 * fSLAX;
	
	//calcualte a temperature reduction factor for head, stem and pericarp growth:
	fTempRedFac = (float)0.0;
	for (I=1;I<=8;I++)
	{
		fTempFac=(float)(0.931+0.114*I-0.0703*I*I+0.0053*I*I*I);
		fTemp=pWth->fTempMin+fTempFac*(pWth->fTempMax-pWth->fTempMin);

		if((fTemp<(float)4.0)||(fTemp>(float)45.0))
			fELOFT = (float)0.0;
		else if(fTemp<(float)17.0)
			fELOFT = (float)((fTemp-4.0)/13.0);
		else if(fTemp<(float)31.0)
			fELOFT = (float)1.0;
		else
			fELOFT = (float)((45.0 - fTemp)/14.0);

		fTempRedFac = fTempRedFac+ fELOFT/(float)8.;
	}


	switch (pDev->iStageCERES)
		{
	case 1:
	case 2: //biomass partitioning rules do not change between stage 1 and stage 2
			if(pCan->fLeafTipNum < (float)1.0)
				PLAG = (float)2.0; //cm^2/plant/day
			else
				PLAG = IncLA_SF(pCan->fExpandLeafNum, pCan->fLeafTipNum, pWth->fTempMax, pWth->fTempMin);
						
			PLAG = fRFR * PLAG;

			if(pPltW->fStressFacLeaf<(float)1.0)
				fSLAMax = fSLAMin;

			pBiom->fMaxLeafGrowR = PLAG / fSLAMin * pMa->pSowInfo->fPlantDens * (float)10.0; // [kg/(ha*day)]
			fMinGrowLeaf			 = PLAG / fSLAMax * pMa->pSowInfo->fPlantDens * (float)10.0;

			//fraction of assimilates available for leaf growth
			fracCarb = (float)0.57*pBiom->fBiomGrowR;

			//assign a maximum of 57% of accumulated drymatter to leaf growth
			if(pBiom->fMaxLeafGrowR <= fracCarb)
			{
				pBiom->fLeafGrowR = pBiom->fMaxLeafGrowR;
				pCan->fLAGrowR = pBiom->fMaxLeafGrowR * fSLAMin*(float)0.1; //[m^2/ha]
			}
			else if(fMinGrowLeaf > fracCarb)
			{
				pBiom->fLeafGrowR = fracCarb;
				pCan->fLAGrowR = pBiom->fLeafGrowR * fSLAMax*(float)0.1; //[m^2/ha]
			}
			else
			{
				pBiom->fLeafGrowR = fracCarb;
				pCan->fLAGrowR = PLAG * pMa->pSowInfo->fPlantDens; //[m^2/ha]
			}

			pBiom->fLeafGrowR = 
				pBiom->fLeafGrowR*(float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);

			pBiom->fStemGrowR = (float)0.245 * pBiom->fBiomGrowR;
			pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR- pBiom->fStemGrowR;

			pCan->fLAGrowR =
				pCan->fLAGrowR*(float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);

			break;

	
		case 3:
			
			PLAG = IncLA_SF(pCan->fExpandLeafNum, pCan->fLeafTipNum, pWth->fTempMax, pWth->fTempMin);
						
			PLAG = fRFR * PLAG;//cm^2/plant/day

			if(pPltW->fStressFacLeaf<(float)1.0)
				fSLAMax = fSLAMin;

			pBiom->fMaxLeafGrowR = PLAG / fSLAMin * pMa->pSowInfo->fPlantDens * (float)10.0; // [kg/(ha*day)]
			fMinGrowLeaf = PLAG / fSLAMax * pMa->pSowInfo->fPlantDens * (float)10.0;

			if(pDev->fSumDTT < (float)150.0)
			{
				//fraction of assimilates available for leaf growth
				fracCarb = (float)0.57*pBiom->fBiomGrowR;

				//assign a maximum of 57% of accumulated drymatter to leaf growth
				if(pBiom->fMaxLeafGrowR <= fracCarb)
				{
					pBiom->fLeafGrowR = pBiom->fMaxLeafGrowR;
					pCan->fLAGrowR = pBiom->fMaxLeafGrowR * fSLAMin*(float)0.1; //[m^2/ha]
				}
				else if(fMinGrowLeaf > fracCarb)
				{
					pBiom->fLeafGrowR = fracCarb;
					pCan->fLAGrowR = pBiom->fLeafGrowR * fSLAMax*(float)0.1; //[m^2/ha]
				}
				else
				{
					pBiom->fLeafGrowR = fracCarb;
					pCan->fLAGrowR = PLAG * pMa->pSowInfo->fPlantDens; //[m^2/ha]
				}

				pBiom->fLeafGrowR = 
					pBiom->fLeafGrowR*(float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);

				pBiom->fStemGrowR = (float)0.245 * pBiom->fBiomGrowR;
				pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR- pBiom->fStemGrowR;

				pCan->fLAGrowR =
					pCan->fLAGrowR*(float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
			}		

			else 
			{
				//assign a maximum of 60.5% of accumulated drymatter to stem growth
				pBiom->fMaxStemGrowR = (float)0.605 * pBiom->fBiomGrowR; //[kg/ha]

				if((pDev->fSumDTT > pStageParam->afThermalTime[2]-(float)180.0)
											&&(fHeadWeight == (float)0.0))
				{
					fHeadWeight = (float)0.05*pBiom->fStemWeight;
					fPotHeadWeight = (float)(22.1*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]
					pBiom->fStemWeight -= fHeadWeight;
					iJPEPE = 1;
				}

				
				if(fHeadWeight > (float)0.0)
					fHeadGrowR = (float)(1.71*fTempRedFac*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]
				else
				{
					fHeadGrowR = (float)0.0;
					pBiom->fMaxStemGrowR = fTempRedFac * pBiom->fMaxStemGrowR;
				}    

				//steht nicht in Oilcrop-Sun!
				if(fHeadGrowR > pBiom->fBiomGrowR)
					fHeadGrowR = pBiom->fBiomGrowR;


				//fraction of assimilates available for leaf growth
				fracCarb = (float)0.90*pBiom->fBiomGrowR-pBiom->fMaxStemGrowR-fHeadGrowR;
				//limit the amount of assimilates for leaf growth to less than 29.5% 
				//of assimilation rate
				if(fracCarb > (float)0.295*pBiom->fBiomGrowR)
						fracCarb = (float)0.295 * pBiom->fBiomGrowR;

				if(fracCarb <= (float)0.0)
				{
					fracCarb = (float)0.0;
					pBiom->fMaxStemGrowR = (float)max(0.0,0.90*pBiom->fBiomGrowR-fHeadGrowR);
					pBiom->fLeafGrowR = (float)0.0;
					pCan->fLAGrowR  = (float)0.0;
				}
				else if(pBiom->fMaxLeafGrowR <= fracCarb)
				{
					pBiom->fLeafGrowR = pBiom->fMaxLeafGrowR;
					pCan->fLAGrowR =  pBiom->fMaxLeafGrowR * fSLAMin*(float)0.1; //[m^2/ha]
				}
				else if(fMinGrowLeaf > fracCarb)
				{
					pBiom->fLeafGrowR = fracCarb;
					pCan->fLAGrowR =  pBiom->fLeafGrowR * fSLAMax*(float)0.1; //[m^2/ha]
				}
				else
					pCan->fLAGrowR =  PLAG * pMa->pSowInfo->fPlantDens; // [m^2/ha]


				pCan->fLAGrowR = pCan->fLAGrowR*(float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
				pBiom->fLeafGrowR = pBiom->fLeafGrowR*(float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
				fHeadGrowR = fHeadGrowR * (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
				pBiom->fStemGrowR = pBiom->fMaxStemGrowR*(float)min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto);
				pBiom->fRootGrowR = pBiom->fBiomGrowR - pBiom->fLeafGrowR - pBiom->fStemGrowR - fHeadGrowR;

			

			}


			//if thermal time from now to first anthesis is less than 130 �C-days,
			//then sum dry matter accumulation rate to fSumPS and count time
			if(pDev->fSumDTT > pStageParam->afThermalTime[2] - (float)130.0)
			{
				fSumPS += pBiom->fBiomGrowR;
				iDurP++;
			}

            break;

		case 4:

			//calculate potential pericarp and head growth as affected by temperature
			fPericarpGrowR = fPotPericarpGrowR * fTempRedFac;
			fHeadGrowR = (float)(1.71*fTempRedFac*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]

			if(fPericarpGrowR > pBiom->fBiomGrowR)
			{
				fPericarpGrowR = pBiom->fBiomGrowR;
				fHeadGrowR = (float)0.0;
			}
			else if (fPericarpGrowR + fHeadGrowR > pBiom->fBiomGrowR)
				fHeadGrowR = pBiom->fBiomGrowR - fPericarpGrowR;

			fExcess = (float)max(0.0,pBiom->fBiomGrowR - fPericarpGrowR - fHeadGrowR);


			if(pBiom->fStemWeight < fStemWeightMax)
			{
				pBiom->fStemGrowR = fExcess;
				pBiom->fRootGrowR = (float)0.0;
			}
			else
			{			
				pBiom->fStemGrowR = (float)0.0;
				pBiom->fRootGrowR = fExcess;
			}

			pBiom->fLeafGrowR = (float)0.0;
			fHeadGrowR = fHeadGrowR * (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
			fPericarpGrowR = fPericarpGrowR * (float)min(pPltW->fStressFacLeaf,pPltN->fNStressLeaf);
			pBiom->fStemGrowR = pBiom->fStemGrowR*(float)min(pPltW->fStressFacPhoto,pPltN->fNStressPhoto);
			pBiom->fRootGrowR = pBiom->fBiomGrowR-fHeadGrowR-fPericarpGrowR-pBiom->fStemGrowR;	

			//calculate amount of nitrogen demanded by grain growth
			NitrogenGrain_SF(exp_p);

			
			fSumPS += pBiom->fBiomGrowR;
			iDurP++;

			break;

		case 5:
			//calculate potential pericarp and head growth as affected by temperature
			if(pDev->fSumDTT < pStageParam->afThermalTime[3]+(float)180.0)
				fPericarpGrowR = fPotPericarpGrowR*fTempRedFac*(float)(0.70+0.30*pPltW->fStressFacPhoto);
			else
				fPericarpGrowR = (float)0.0;

			if(fHeadWeight < fHeadWeightMax)
				fHeadGrowR = (float)(1.71*fTempRedFac*pMa->pSowInfo->fPlantDens*10.0); //[kg/ha]
			else
				fHeadGrowR = (float)0.0;

			//calculate embryo growth rate [mg/embryo/day] as affected by temperature, 
			//a genetic constant, water deficit and the growth factor based on head weight
			fPEPE = fGrowFactor * fTempRedFac * pPl->pGenotype->fRelStorageFillRate *(float)(0.70+0.30*pPltW->fStressFacPhoto);

			//calculate embryo growth rate [kg/ha/day]
			if(pCan->fGrainNum>(float)0.0)
				fEmbryoGrowR = pCan->fGrainNum * fPEPE * pMa->pSowInfo->fPlantDens * (float)0.01;
			else
				fEmbryoGrowR = ppp * fPEPE * pMa->pSowInfo->fPlantDens * (float)0.01;


			//calculated carbohydrate demand for total growth and grain growth
			fCDemandTot   = fEmbryoGrowR + fPericarpGrowR + fHeadGrowR;
			fCDemandGrain = fEmbryoGrowR + fPericarpGrowR;

			if(pBiom->fBiomGrowR > fCDemandTot)
			{
				//If ->fBiomGrowR exceeds the biomass required for both grain and head 
				//growth then the excess biomass is allocated to stem or roots
				if(pBiom->fStemWeight < fStemWeightMax)
				{
					if(fStemWeightMax-pBiom->fStemWeight > pBiom->fBiomGrowR-fCDemandTot)
						pBiom->fStemGrowR = pBiom->fBiomGrowR-fCDemandTot;
					else
					{
						pBiom->fStemGrowR = fStemWeightMax - pBiom->fStemWeight;
						pBiom->fRootGrowR = pBiom->fBiomGrowR - fCDemandTot - pBiom->fStemGrowR;
					}
				}
				else
				{
					pBiom->fStemGrowR = (float)0.0;
					pBiom->fRootGrowR = pBiom->fBiomGrowR - fCDemandTot;
				}
			}
			else
			{
				pBiom->fStemGrowR = (float)0.0;
				pBiom->fRootGrowR = (float)0.0;

				if(pBiom->fBiomGrowR > fCDemandGrain)
				{
					//If fBiomGrowR exceeds the bimass required for grain but is insufficient
					//for both grain and head growth, then grain (pericarp+embryo) grows
					//potentially and head gets the excess biomass
					fHeadGrowR = pBiom->fBiomGrowR - fCDemandGrain;
				}
				else
				{
					fHeadGrowR = (float)0.0;

					//calculate translocatable biomass Pool [kg/ha/day]
					fCPool = (pBiom->fStemWeight - fStemWeightMin) + 
						(fHeadWeight - fHeadWeightMin);
					
					fCP = (pBiom->fStemWeight - fStemWeightMin)/(fStemWeightMax - fStemWeightMin);
					if(fCP > (float)0.3)
						fCP2 = (float)1.0;
					else
						fCP2 = (float)0.0;

					fCPool1 = (float)(0.79 * fCP2 * pMa->pSowInfo->fPlantDens * 10.0);


					fCP = (fHeadWeight - fHeadWeightMin)/(fHeadWeightMax - fHeadWeightMin);
					if(fCP > (float)0.3)
						fCP2 = (float)1.0;
					else
						fCP2 = (float)0.0;

					fCPool2 = (float)(0.42 * fCP2 * pMa->pSowInfo->fPlantDens * 10.0);

					fCPool = fCPool1 + fCPool2;



					if(fCPool > (fCDemandGrain - pBiom->fBiomGrowR)/(float)0.44)
					{
						//If fCPOOl is enough to satisfy the demand for biomass for use in growth, 
						//calculate export reuirements from head and stem and resulting organ 
						//biomass changes assuming that 100 units of biomass in fCPool are 
						//equivalent to 44 units of biomass in the grain.

						fPool1 = fCPool1/fCPool;
						fPool2 = fCPool2/fCPool;

						pBiom->fStemWeight -= (fEmbryoGrowR + fPericarpGrowR - 
										pBiom->fBiomGrowR)/(float)0.44 * fPool1;
						fHeadWeight -= (fEmbryoGrowR + fPericarpGrowR - 
										pBiom->fBiomGrowR)/(float)0.44 * fPool2;
					}
					else
					{
						// If fCPool is not enough to satisfy the biomass requirement for growth, 
						//calculate the effects of exporting all available labile biomass on 
						//organ dry weight and grain growth.
						if(fCDemandGrain > (float)(0.1*pMa->pSowInfo->fPlantDens * 10.0))
							fPoolFactor = (fCPool*(float)0.44 + pBiom->fBiomGrowR)/fCDemandGrain;
						else
							fPoolFactor = (float)0.0;

						fEmbryoGrowR *= fPoolFactor;
						fPericarpGrowR *= fPoolFactor;
						
						pBiom->fStemWeight -= fCPool1;
						fHeadWeight -= fCPool2;

					} //end if (fCPool > fCDemandGrain - pBiom->fBiomGrowR)
				} //end if (pBiom->fBiomGrowR > fCDemandGrain)
			} //end if (BiomGrowR > fCDemandTot)

		//pBiom->fRootGrowR = pBiom->fBiomGrowR - fEmbryoGrowR - 
						//fPericarpGrowR - fHeadGrowR - pBiom->fStemGrowR;

		
		NitrogenGrain_SF(exp_p);
  

		fSumPS += pBiom->fBiomGrowR;
		iDurP++;

		break;
		}

	//====================================================================================
	//	Organ Fraction
	//====================================================================================
	pBiom->fPartFracLeaf	= pBiom->fLeafGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracRoot	= pBiom->fRootGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracStem	= pBiom->fStemGrowR/pBiom->fBiomGrowR;
	pBiom->fPartFracFruit	= pBiom->fGrainGrowR/pBiom->fBiomGrowR;
//	pBiom->fEarFraction	= pBiom->fEarGrowRate/pBiom->fBiomGrowR;
    pBiom->fPartFracGrain   = pBiom->fPartFracFruit;
	fTopFraction 	= (float)1.0-pBiom->fPartFracRoot;

	return 1;
	}



float WINAPI _loadds IncLA_SF(float LeafExpandNum, float LeafTipNum, float TempMax, float TempMin)
{
	float fMaxLA, fLER, fTLNOi, fYLNOi;
	float TempAve, PLAG = (float)0.0, EGFT = (float)0.0;
	float fTTE = (float)350.0;
	int i;

		//==================================================================================
		//	Effect of Temperature on Leaf Extension Growth (EGFT)
		//==================================================================================
			
		TempAve=(TempMax+TempMin)/((float)2.0);
					
		if((TempAve<(float)4.0)||(TempAve>(float)40.0))
			EGFT=(float)0.0;
		else if(TempAve<(float)24.0)
			EGFT = (TempAve - (float)4.0)/(float)20.0;
		else
			EGFT = ((float)40.0 - TempAve)/(float)16.0;
				
		EGFT = max((float)0.0, min((float)1.0,EGFT));

		//calculate leaf position where maximum individual leaf area occurs:
		fTLNOi = (float)(int)(1.54 + 0.61*iTLNO);

		//calculate maximum leaf area corresponding tp position fTLNOi:
		fYLNOi = (float)(150 + (fTLNOi - 6.) *74); //cm^2?


		for(i=(int)LeafExpandNum+1; i<=(int)LeafTipNum;i++)
		{
			if(i<=6)
				fMaxLA = (float)(i*25);
			else if (i<=(int)fTLNOi)
				fMaxLA = (float)(150 + (i-6)*74);
			else
				fMaxLA = fYLNOi - (float)(177*(i-(int)fTLNOi));

			if(fMaxLA < (float)0.0)
				fMaxLA = (float)1.0;
				
				
			fLER = fMaxLA * EGFT/(float)18.0;

			PLAG += fLER;
		}
		return PLAG;
}


int WINAPI _loadds NitrogenGrain_SF(EXP_POINTER)
{
	float fPR, fPO, fOilInc;
	float fNSink, fNSink1, fNSink2, fFracSink1, fFracSink2;
	float fNStovActConc, fNOut, fNOutLeaf;
	float fNPool, fNPool1, fNPool2;
	float fNPool1Leaf, fNPool1Stem, fNPool1Head;
	float fNPool2Leaf, fNPool2Root;
	float fNSupplyDemandR;
	static float fOilFrac;

	PBIOMASS		pBiom = pPl->pBiomass;
	PPLTNITROGEN	pPltN = pPl->pPltNitrogen;

	//optimal N-concentration of pericarp and embryo
	fNEmbryoOptConc = (float)(0.0225+0.0200*fNFAC);
	fNPericarpOptConc = (float)(0.0050+0.0100*fNFAC);

	if((fEmbryoGrowR > (float)0.0)&&(pPl->pDevelop->fSumDTT>(float)230.0))
	{
		fPR = (float)100.0*fEmbryoGrowR/(pPl->pCanopy->fGrainNum * 
			pPl->pGenotype->fRelStorageFillRate * pMa->pSowInfo->fPlantDens);

		if(fPR <= (float)0.7)
			fPO = fO1;
		else
			fPO = (float)exp(-1.4*(fPR-0.8))*fO1;

		//calculate oil accumulation [kg/ha/day]
		fOilInc =  fPO/(float)100.0 * fEmbryoGrowR * (fP5 - (float)170.0)/(fP5 - (float)230.0);
		fOil = fOil + fOilInc;
	}
	else
	{
		fOil = (float)0.0;
		fOilFrac = (float)0.0;
	}


	//calculate nitrogen demand generated by grain N concentration for
	//new growth [kg(N)/ha]
	if(fEmbryoWeight > (float)0.0)
		fNSink1 = fEmbryoGrowR * fNEmbryoOptConc;
	else
		fNSink1 = (float)0.0;

	if(fPericarpWeight > (float)0.0)
		fNSink2 = fPericarpGrowR * fNPericarpOptConc;
	else
		fNSink2 = (float)0.0;

	fNSink = fNSink1 + fNSink2;

	if(fNSink != (float)0.0)
	{
		fFracSink1 = fNSink1/fNSink;
		fFracSink2 = fNSink2/fNSink;

		if(pPltN->fRootActConc < pPltN->fRootMinConc)
			pPltN->fRootActConc = pPltN->fRootMinConc;


		//actual N concentration
		fNStovActConc = pPltN->fStovCont/pBiom->fStovWeight;

		//calculate leaf contribution to the two N pools
		fNPool1Leaf = (float)max(0.0,pBiom->fGreenLeafWeight * 
							(pPltN->fLeafActConc - pPltN->fLeafMinConc));

		if(fNPool1Leaf > fXPEPE)
		{
			fNPool1Leaf -= fXPEPE;
			fNPool2Leaf  = fXPEPE;
		}
		else
		{
			fNPool2Leaf = fNPool1Leaf;
			fNPool1Leaf = (float)0.0;
		}

		//calculate stem contribution to NPool1
		fNPool1Stem = (float)max(0.0,pBiom->fStemWeight * 
							(pPltN->fStemActConc - pPltN->fStemMinConc));

		//calculate head contribution to NPool1
		fNPool1Head = (float)max(0.0,fHeadWeight *
							(fNHeadActConc - fNHeadMinConc));

		//calculate the size of the first labile N pool in above ground parts
		//[kg(N)/ha]
		fNPool1 = fNPool1Leaf+fNPool1Stem+fNPool1Head;

		//calculate root contribution to NPool2
		fNPool2Root = (float)max(0.0,pBiom->fRootWeight * 
							(pPltN->fRootActConc - pPltN->fRootMinConc));
				
		//calculate the size of the second labile N pool [kg(N)/ha]
		fNPool2 = fNPool2Leaf+fNPool2Root;

		fNPool = fNPool1 + fNPool2;


		//calculate (nitrogen supply)/(nitrogen demand) ratio
		fNSupplyDemandR = fNPool/fNSink;

		if(fNSupplyDemandR < (float)1.0)
			fNSink *= fNSupplyDemandR;

		//calculate limiting N concentration for green leaves that have lost all
		//available nitrogen
		fYRAT = (float)((0.009-0.0875*fNGreenLeafActConc)/0.9125);

		//reduce stem, head and stover N contents
		if(fNSink > fNPool1)
		{
			pPltN->fStemCont -= fNPool1Stem;
			fNHeadCont		 -= fNPool1Head;
			pPltN->fStovCont -= fNPool1;

			//the amount of N that will be extracted from each organ contributing to NPool2
			//will be proportional to its labile N content
			fNOut	= fNSink - fNPool1;
			//root:
			pPltN->fRootCont -= fNOut*fNPool2Root/fNPool2;
			pPltN->fRootActConc = pPltN->fRootCont/pBiom->fRootWeight;
			//leaves:
			fNOutLeaf = fNOut*fNPool2Leaf/fNPool2 + fNPool1Leaf;
			pPltN->fLeafCont -= fNOutLeaf;

			fNGreenLeafCont -= fNOutLeaf-fNOutLeaf*fYRAT/(fNGreenLeafCont/pBiom->fGreenLeafWeight-fYRAT);
			fNSenesLeafCont  = pPltN->fLeafCont - fNGreenLeafCont;
		}
		else
		{
			pPltN->fLeafCont -= fNPool1Leaf/fNPool1*fNSink;
			fNGreenLeafCont  -= fNPool1Leaf/fNPool1*fNSink*((float)1.0+fYRAT
							/(fNGreenLeafCont/pBiom->fGreenLeafWeight-fYRAT));
			fNSenesLeafCont  = pPltN->fLeafCont - fNGreenLeafCont;

			pPltN->fStemCont -= fNPool1Stem/fNPool1*fNSink;
			fNHeadCont		 -= fNPool1Head/fNPool1*fNSink;

			pPltN->fStovCont -= fNSink;		
			fNStovActConc = pPltN->fStovCont/pBiom->fStovWeight;
		}

		//actualize pericarp, embryo and grain N contens
		fNPericarpCont += fNSink * fFracSink2;
		fNEmbryoCont   += fNSink * fFracSink1;
	}

	pPltN->fGrainCont = fNPericarpCont + fNEmbryoCont;

	return 1;
}



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//		Cold Hardening for Wheat
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*
int ColdHardening_WH_BA(EXP_POINTER)
	{
	int  		i;
    float 		HTI,CK,TEMKIL,fTempCropAve;
    PDEVELOP 		pDev	=pPl->pDevelop;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PCANOPY		pCan	=pPl->pCanopy;   
    PWEATHER		pWth	=pCl->pWeather;
	PMODELPARAM	pPltMP	=pPl->pModelParam;

	//==============================================================================================
	//		Hardness Index
	//==============================================================================================
	if ((pWth->fTempMin>pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase-(float)3.0)&&
		(pPltMP->fHardnessIndex==(float)0.0))
		return 1;

	HTI=(float)1.0;

    fTempCropAve=PlantTemperature(lpCtr,lpClm,lpPlt,lpSoil,lpMan);

	if (pPltMP->fHardnessIndex>=HTI)
		{
		if (fTempCropAve<pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase-(float)1.0)
			if (pWth->fTempMin>(float)(-6.0)) return 1;

		if (fTempCropAve<=pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase+(float)8.0)
			{
			pPltMP->fHardnessIndex +=  (float)0.1
							  -(fTempCropAve-(pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase+(float)3.5))
							  *(fTempCropAve-(pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase+(float)3.5))
							  /((float)506.0);

			if (pPltMP->fHardnessIndex<HTI) goto JUMP;
			}
		}

   	if (fTempCropAve<=pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase+(float)0.0)
		{
  		pPltMP->fHardnessIndex=pPltMP->fHardnessIndex+(float)0.083;
  		if (pPltMP->fHardnessIndex>(float)2.0*HTI)
  			pPltMP->fHardnessIndex=(float)2.0*HTI;
  		}

JUMP: if (pWth->fTempMax>=pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase+(float)10.0)
		{
		pPltMP->fHardnessIndex += (float)0.2-(float)0.02*pWth->fTempMax;
		if (pPltMP->fHardnessIndex>HTI)
			pPltMP->fHardnessIndex += (float)0.2-(float)0.02*pWth->fTempMax;
		if (pPltMP->fHardnessIndex<(float)0.0)
			pPltMP->fHardnessIndex=(float)0.0;
		}

	if (pWth->fTempMin>(float)(-6.0)) return 1;

	//==============================================================================================
	//      Calculating Leaf Senescence
	//==============================================================================================
    //Factor determing the green leaf area reduction due to low temperature
	CK=((float)0.020*pPltMP->fHardnessIndex-(float)0.10)
			*(pWth->fTempMin*(float)0.85+pWth->fTempMax*(float)0.15
										 +(float)10.0+(float)0.25*pWth->fSnow);

	if (CK<(float)0.0) 	CK=(float)0.0;
	if (CK>(float)0.96) CK=(float)0.96;

	pCan->fPlantSenesLA += CK*(pCan->fPlantLA-pCan->fPlantSenesLA);

	for (i=1;i<=(int)pCan->fExpandLeafNum;i++)
		pPltMP->PLSC[i] *= ((float)1.0-CK);

	if (pCan->fPlantLA-pCan->fPlantSenesLA<=(float)0.5*pCan->fTillerNum)
		{
		pCan->fPlantSenesLA=pCan->fPlantLA-pCan->fTillerNum*(float)0.5;

		if (pBiom->fSeedReserv<=(float)0.0)
			{
			pBiom->fSeedReserv=(float)0.05*(pBiom->fLeafWeight+pBiom->fRootWeight);
			
			if (pBiom->fSeedReserv>(float)0.05*(float)10.0*pMa->pSowInfo->fPlantDens)
				pBiom->fSeedReserv=(float)0.05*(float)10.0*pMa->pSowInfo->fPlantDens;

			pBiom->fLeafWeight=pBiom->fLeafWeight-pBiom->fSeedReserv*(float)0.5;
			pBiom->fRootWeight=pBiom->fRootWeight-pBiom->fSeedReserv*(float)0.5;
			}
		}

	//==============================================================================================
	//      Calculating Tiller and Plant Death
	//==============================================================================================
	//Loss of tillers due to cold temperature
	TEMKIL=pPl->pGenotype->asStageParam[pDev->iStageCERES].fTempBase-(float)6.0-(float)6.0*pPltMP->fHardnessIndex;

	if (TEMKIL<fTempCropAve)	return 1;

	if (pCan->fTillerNum>=(float)1.0)
		pCan->fTillerNum *= ((float)0.9-(float)0.02*(fTempCropAve-TEMKIL)*(fTempCropAve-TEMKIL));

	if (pCan->fTillerNum<(float)1.0)
		{
		pCan->fPLANTS=pCan->fPLANTS*((float)0.95
										-(float)0.02*(fTempCropAve-TEMKIL)*(fTempCropAve-TEMKIL));
		pCan->fTillerNum=(float)1.0;

		if (pCan->fPLANTS<(float)5.0)	//95% plants killed by cold
			{
			pPltMP->bColdKill	=TRUE;
		    pCan->fPLANTS		=(float)0.0;
      		pBiom->fGrainWeight=(float)0.0;
      		pCan->fGrainNum	=(float)1.0;
      		pDev->iStageCERES	=5;
      		}
      	}

	return 1;
	}
*/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Leaf Number
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int PlantLeafNumber_WH_BA(EXP_POINTER)
	{
    float 		fMaxLeafNum;

    PDEVELOP 	pDev	=pPl->pDevelop;
    PCANOPY	pCan	=pPl->pCanopy;

	fMaxLeafNum=(float)25.0;  //von CERES

	//Enli?: fMaxLeafNum	=(float)12.0;

//	pDev->fDTT=max((float)0.0,pDev->fDTT);

	if (pDev->iStageCERES<=2)
		{
//		pCan->fLeafAppearR=pDev->fDTT/pPl->pGenotype->fPhyllochronInterval;
		pCan->fLeafAppearR=pDev->fDTT/fPHINT;
		pDev->fCumPhyll += pCan->fLeafAppearR;

		if (pCan->fExpandLeafNum>=fMaxLeafNum) //Crop Reached Maximum Leaf Number on that Day
			pCan->fExpandLeafNum=fMaxLeafNum;
		else
			pCan->fExpandLeafNum=pDev->fCumPhyll+(float)2.0;
		}

    return (int)pCan->fExpandLeafNum;
    }

int PlantLeafNumber_MZ(EXP_POINTER)
	{
    float 	fPhyllFac;

    PDEVELOP 	pDev	=pPl->pDevelop;
    PCANOPY	pCan	=pPl->pCanopy;

	if ((pDev->iStageCERES<=3)&&(pCan->fExpandLeafNum<=pPl->pModelParam->fMaxLeafNum))
		{
		if(pDev->fCumPhyll<(float)5.0)
			fPhyllFac = (float)0.66+(float)0.068*pDev->fCumPhyll;
		else
			fPhyllFac = (float)1.0;

		//SG20140910: Phyllochron-Intervall aus Maize.gtp
		//pCan->fLeafAppearR=pDev->fDTT/((float)38.9*fPhyllFac);
		pCan->fLeafAppearR=pDev->fDTT/(pPl->pGenotype->fPhyllochronInterval*fPhyllFac);
		
		}
	//else
	//	pCan->fLeafAppearR=(float)0.0;

	pDev->fCumPhyll += pCan->fLeafAppearR;
	//pCan->fExpandLeafNum = pCan->fLeafTipNum = pDev->fCumPhyll;//+(float)1.0;
	//SG20140910: ein Blatt mehr f�r AgMIP-Maize#2
	pCan->fExpandLeafNum = pCan->fLeafTipNum = pDev->fCumPhyll+(float)1.0;

    return 1;
    }

int PlantLeafNumber_SF(EXP_POINTER)
	{
    PDEVELOP 	pDev = pPl->pDevelop;
    PCANOPY		pCan = pPl->pCanopy;
	float fMaxLeafNum, fXHY;

	float fTTE = (float)350;
	float fPhy1 = (float)39.0;
	float fPhy2 = (float)24.0;


	fMaxLeafNum = (float)iTLNO;

	//calculate total number of leaves appeared:
	if (pCan->fLeafTipNum<=(float)6)
		fPHINT = fPhy1;
	else
		fPHINT = fPhy2;

	pCan->fLeafAppearR=pDev->fDTT/fPHINT;
	pCan->fLeafTipNum += pCan->fLeafAppearR;

	if(pCan->fLeafTipNum>=fMaxLeafNum)
		pCan->fLeafTipNum = fMaxLeafNum;


	//calculate number of expanded leaves:
	if(pCan->fExpandLeafNum<=(float)6.0)
		fXHY = fPhy1;
	else
		fXHY = fPhy2;

	pCan->fLeafAppearR=pDev->fDTT/fXHY;
	
	
	if(pCan->fExpandLeafNum<=fMaxLeafNum)
	{
		if((pDev->fCumDTT-fP9)<=(fTTE+fPhy1))
			pCan->fExpandLeafNum=(float)0.0;
		else
			pCan->fExpandLeafNum+=pCan->fLeafAppearR;
	}
	else
		pCan->fExpandLeafNum = fMaxLeafNum;


    return 1;
    }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Leaf Area
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int PlantLeafArea_WH_BA(EXP_POINTER)
	{
	float 	CLG,PLAGMS,EGFT,fLeafGrowRate;

	PDEVELOP 		pDev	=pPl->pDevelop;
	PBIOMASS 		pBiom	=pPl->pBiomass;
	PCANOPY 		pCan	=pPl->pCanopy;
	PMODELPARAM pPltMP	=pPl->pModelParam;
    PPLTWATER 		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen; 
    PWEATHER		pWth	=pCl->pWeather;

	//==================================================================================
	//	Effect of Temperature on Leaf Extension Growth (EGFT)
	//==================================================================================
	EGFT=(float)1.0;

	if (pDev->iStageCERES<=2)
		{
		pWth->fTempAve=(pWth->fTempMax+pWth->fTempMin)/((float)2.0);

		EGFT=(float)1.2-(float)0.0042*(pWth->fTempAve-(float)17.0)
									 *(pWth->fTempAve-(float)17.0);

		EGFT = max((float)0.0, min((float)1.0,EGFT));
		}

	//==================================================================================
	//	Leaf Area Growth in different stages
	//==================================================================================
	switch (pDev->iStageCERES)
		{
		case 1:
	        //Plant leaf area growth rate on the main stem (PLAGMS: cm2/plant.day)
			CLG	  =(float)7.5;
			//SG 20111109: f�r Kraichgau - gebremstes Blattwachstum
			CLG	  =(float)6.5;  //6.0
			PLAGMS=CLG*(float)sqrt(pDev->fCumPhyll)*pCan->fLeafAppearR
					  					*min(pPltW->fStressFacLeaf,min(EGFT,pPltN->fNStressLeaf));

            //Plant leaf area growth rate (cm2/plant.day)
			pCan->fLAGrowR=PLAGMS*((float)0.3+(float)0.7*pCan->fTillerNum);

			//Area growth rate(cm2/plant.day) are calculated based on 900 tillers
			//when tiller number more than 900
			if (pCan->fTillerNumSq>(float)900.0)
				pCan->fLAGrowR=PLAGMS*((float)900.0)/pMa->pSowInfo->fPlantDens;

			break;

		case 2:
	    	//Plant Leaf area growth rate (cm2/plant.day)
			fLeafGrowRate = pBiom->fLeafGrowR/((float)10.0*pMa->pSowInfo->fPlantDens);
			
		   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")) 	//for Wheat
				pCan->fLAGrowR=fLeafGrowRate*((float)115.0);

	    	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) 			//for Barley
				pCan->fLAGrowR=fLeafGrowRate*((float)127.0);


			break;

		default:
			pCan->fLAGrowR=(float)0.0;
	  		break;

        }

            
    //Leaf Area Growth Rate (m2/ha.day)
    pCan->fLAGrowR *= pMa->pSowInfo->fPlantDens;

	//Plant Leaf Area (m2/ha)
	pCan->fPlantLA += pCan->fLAGrowR;

 	return 1;
    }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Leaf Senecence
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int LeafSenescence_WH_BA(EXP_POINTER)
	{
	int 	I;
	float 	WaterFact,NitrogFact;
	static float PLSC[30];

	PDEVELOP		pDev	=pPl->pDevelop;
	PBIOMASS		pBiom	=pPl->pBiomass;
	PCANOPY			pCan	=pPl->pCanopy;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;

    if ((int)pCan->fExpandLeafNum==0)
    	{
    	for (I=0;I<=29;I++)
    		PLSC[I]=(float)0.0;
    	}

	//====================================================================================
	//	Normal Leaf Senescence
	//====================================================================================
	switch (pDev->iStageCERES)
		{
		case 1:
			if (pDev->fCumPhyll<=(float)4.0)
				pCan->fLALossR=(float)0.0;
            else
				pCan->fLALossR=(PLSC[(int)pCan->fExpandLeafNum-4]
  										      -PLSC[(int)pCan->fExpandLeafNum-5])
  										      *pCan->fLeafAppearR;

			if ((pCan->fPlantSenesLA/pCan->fPlantLA>(float)0.4)&&(pCan->fLAI<(float)6.0))
				pCan->fLALossR=(float)0.0;


          	break;

   		case 2:
			if (pDev->fCumPhyll>(float)4.0)
				pCan->fLALossR=(PLSC[(int)pCan->fExpandLeafNum-4]
											  -PLSC[(int)pCan->fExpandLeafNum-5])
											  *pCan->fLeafAppearR;
			if ((pCan->fPlantSenesLA/pCan->fPlantLA>(float)0.4)&&(pCan->fLAI<(float)6.0))
				pCan->fLALossR=(float)0.0;

   			break;

   		case 3:
			pCan->fLALossR=((float)0.0003)*pDev->fDTT
  											        *pCan->fPlantGreenLA;
   			break;

   		case 4:
 			pCan->fLALossR=((float)0.0006)*pDev->fDTT
													*pCan->fPlantGreenLA;
 	        break;

 	    case 5:
		
			{
			int i=1; PSTAGEPARAM pStageParam=pPl->pGenotype->pStageParam;
		//	while (i<5)  pStageParam=pStageParam->pNext;

			//pCan->fLALossR=pCan->fPlantGreenLA*((float)2.0)
			//						*pDev->fSumDTT*pDev->fDTT/
			//						(pStageParam->afThermalTime[4]*
			//						 pStageParam->afThermalTime[4]);

			///////////////////////////////////////////////////////////////////////
			//SG 20111109: f�r Kraichgau - gebremste Blattseneszenz
			pCan->fLALossR=pCan->fPlantGreenLA*((float)1.0)
									*pDev->fSumDTT*pDev->fDTT/
									(pStageParam->afThermalTime[4]*
									 pStageParam->afThermalTime[4]);

			//pCan->fLALossR=(float)max(pCan->fLALossR,((float)0.0006)*pDev->fDTT
			//										*pCan->fPlantGreenLA);
			// End SG 20111109
			//////////////////////////////////////////////////////////////////////
			}

			break;
  		}


	//====================================================================================
	//	Cumulative Leaf Area
	//====================================================================================
	PLSC[(int)pCan->fExpandLeafNum]=pCan->fPlantLA;

	//====================================================================================
	//	Leaf Senescence due to Water and Nitrogen Stress
	//====================================================================================
	if (pPltW->fStressFacPhoto<=(float)0.8)
		{
    	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))  //for Wheat
			WaterFact=(float)2.0-pPltW->fStressFacPhoto/((float)0.8);

        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) //for Barley
			WaterFact=(float)1.5-pPltW->fStressFacPhoto/((float)1.6);
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		NitrogFact=(float)2.0-pPltN->fNStressFac/((float)0.8);

        pCan->fLALossR *= min(WaterFact,NitrogFact);
		}
                            

	//Dead Leaf Area (m2/ha.day)	
	pCan->fPlantSenesLA += pCan->fLALossR; 
	
	pCan->fPlantGreenLA = pCan->fPlantLA-pCan->fPlantSenesLA;

	return 1;
	}

int LeafSenescence_MZ(EXP_POINTER)
	{
	static float fNormLeafSenes;
	float fSenesLAOld;
	float fLeafSenesWaterFact,fLeafSenesLightFact,fLeafSenesTempFact,fTempAve;

	PDEVELOP		pDev	=pPl->pDevelop;
	PCANOPY			pCan	=pPl->pCanopy;
    PPLTWATER		pPltW	=pPl->pPltWater;

	fSenesLAOld = pCan->fPlantSenesLA;

	if(pDev->iDayAftEmerg == 1)
		fNormLeafSenes = (float)0.0;
	//====================================================================================
	//	Normal Leaf Senescence
	//====================================================================================
	switch (pDev->iStageCERES)
		{
		case 1:
   		case 2:
			fNormLeafSenes = pDev->fSumDTT*pCan->fPlantLA/(float)10000.0;
   			break;

   		case 3:
			fNormLeafSenes += pCan->fPlantLA/(float)1000.0;
   			break;

   		case 4:
			//nach CERES-Maize pp.81:
			fNormLeafSenes = pCan->fPlantLA*(float)(0.05 + 0.05*pDev->fSumDTT/170.0);
			//altes Expert-N:
			//fNormLeafSenes = pCan->fPlantLA*(float)(0.025*(1.0+4.0*pDev->fSumDTT/fP5));
 	        break;

 	    case 5:
			//nach CERES-Maize pp.82:
			fNormLeafSenes =pCan->fPlantLA*(float)(0.1+0.8*pow((pDev->fSumDTT+170.0)/fP5,3.0));
			//altes Expert-N:
			//fNormLeafSenes =pCan->fPlantLA*(float)(0.05+0.8*pow(pDev->fSumDTT/fP5,4.0));
			break;
  		}



	//====================================================================================
	//	Leaf Senescence due to Drought Stress, Competition for Light and low temperature
	//====================================================================================
	
		//water:
		fLeafSenesWaterFact=(float)0.95+(float)0.05*pPltW->fStressFacPhoto;

        //Light:
		if(pCan->fLAI > (float)4.0)
			fLeafSenesLightFact = (float)1.0 - (float)0.008*(pCan->fLAI-(float)4.0);
		else
			fLeafSenesLightFact = (float)1.0;

		//Temperature:
		fLeafSenesTempFact = (float)1.0;
		fTempAve = (pCl->pWeather->fTempMin+pCl->pWeather->fTempMax)/(float)2.0;
		if(fTempAve < (float)6.0)
			fLeafSenesTempFact = (float)max(0.0,fTempAve/6.0);
		if(fTempAve <= (float)-5.0)
			fLeafSenesTempFact = (float)0.0;

		pCan->fLeafSenesR = pCan->fPlantGreenLA*(float)(1.0-min(fLeafSenesWaterFact,
								min(fLeafSenesLightFact,fLeafSenesTempFact)));

		pCan->fPlantSenesLA += pCan->fLeafSenesR; 

		if(pCan->fPlantSenesLA < fNormLeafSenes)
			pCan->fPlantSenesLA = fNormLeafSenes;

		if(pCan->fPlantSenesLA >= pCan->fPlantLA)
			pCan->fPlantSenesLA = pCan->fPlantLA;

		pCan->fLALossR = pCan->fPlantSenesLA - fSenesLAOld;
	
		pCan->fPlantGreenLA = pCan->fPlantLA-pCan->fPlantSenesLA;
//		pPl->pBiomass->fLeafWeight -= pCan->fLALossR*pPl->pGenotype->fSpecLfWeight;

	return 1;
	}

int LeafSenescence_SF(EXP_POINTER)
	{
	int i;
	float wwww, fXXX, fYYY, fZZZ;
	float fDSenesLAN2, fDSenesLANW;
	float fNAvailableGreenLeaf, fLAEq;
	static float fSGRO[12];
	
	PCANOPY		pCan	= pPl->pCanopy;
	PPLTWATER	pPltW	= pPl->pPltWater;
	PBIOMASS	pBiom	= pPl->pBiomass;
	PPLTNITROGEN pPltN	= pPl->pPltNitrogen;


	if(pPl->pDevelop->iStageCERES ==3)
	{
		if(pCan->fLAI <= (float)1.2)
			iSenesTime = 0;

		// When LAI>1.2, commence counting time from now to the start 
		// of leaf senescence driven by shade
		if((pCan->fLAI > (float)1.2)&&(bSenesCode==FALSE))
		{
			iSenesTime++;
			fSGRO[iSenesTime-1] = pCan->fLAGrowR/pMa->pSowInfo->fPlantDens; //[cm^2/plant]
		}

		if(iSenesTime  == 13)
			bSenesCode = TRUE;

		//When iSenesTime exceeds 13 days then calculate the rate of shade 
		//driven leaf senescence. fSGRO[1] is plant area growth rate 13 days ago. 
		if(bSenesCode)
		{
			wwww = (float)(-.0182+.00004147*fSGRO[0]*pMa->pSowInfo->fPlantDens); //[m^2/m^2]

			for(i=0;i<11;i++)
				fSGRO[i] = fSGRO[i+1];

			fSGRO[11] = pCan->fLAGrowR/pMa->pSowInfo->fPlantDens; //[cm^2/plant];

			pCan->fLeafSenesR = wwww * (float)1e4;	//[m^2/ha]
		}
		else
			pCan->fLeafSenesR = (float)0.0;


		//After the last 5 leaves have begun to expand and if water 
		//stress is affecting expansion, then leaf senescence caused 
		//by water stress is calculated as 3& per day of green plant 
		//leaf area.
		if(((int)pCan->fExpandLeafNum>iTLNO-5)&&(pPltW->fStressFacLeaf<(float)0.8))
			pCan->fLeafSenesR = (float)max(pCan->fLeafSenesR,0.03*pCan->fPlantGreenLA);

		pCan->fPlantSenesLA = pCan->fPlantSenesLA + pCan->fLeafSenesR;

		//cumulative senesced leaf area
		fCumSenesLA += pCan->fLeafSenesR;

		//////////////////////////////////////////////////////////////////////
		//Adjust nitrogen contents and weights of green and dead leaves		//
		//																	//
			//specific leaf area [m^2/kg]
			fSpecLeafArea = pCan->fPlantGreenLA/pBiom->fGreenLeafWeight;
			//calculate nitrogen concentration of green leaves
			fNGreenLeafActConc = fNGreenLeafCont/pBiom->fGreenLeafWeight;
			//calculate residual nitrogen concentration
			fYRAT = (float)((0.009 - 0.0875*fNGreenLeafActConc)/0.9125);

			//calculate the amount of nitrogen retranslocated from
			//senesced leaves to other organs [kg(N)/ha]
			pPltN->fLeafNtransRate = pCan->fLeafSenesR/fSpecLeafArea * 
												(fNGreenLeafActConc - fYRAT);

			//adjust the weight of senesced leaves
			pBiom->fSenesLeafWeight += pCan->fLeafSenesR/fSpecLeafArea - 
										pPltN->fLeafNtransRate * (float)6.25;

			//calculate residual nitrogen in senesced leaves
			fNSenesLeafCont += pCan->fLeafSenesR/fSpecLeafArea*fYRAT;

			//adjust the weight of green leaves
			pBiom->fGreenLeafWeight += pBiom->fLeafGrowR  
										+ pPltN->fLeafNtransRate * (float)6.25  
										- pCan->fLeafSenesR/fSpecLeafArea;

			//Calculate the amount of nitrogen in green leaves
			fNGreenLeafCont -= pCan->fLeafSenesR/fSpecLeafArea*fYRAT;

			//calculate maximum permissible amount of nitrogen in green leaves
			 fXXX = pBiom->fGreenLeafWeight * pPltN->fLeafOptConc;

			 //if the calculated amount of N in green leaves is greater than the 
			 //maximum (optimum), then allocate the excess N to the stem and adjust 
			 //organ weights
			 if(fNGreenLeafCont > fXXX)
			 {
				 fYYY = fNGreenLeafCont - fXXX;
				 fNGreenLeafCont = fXXX;

				 pBiom->fGreenLeafWeight -= fYYY*(float)6.25;
				 pBiom->fLeafWeight      -= fYYY*(float)6.25;
				 pBiom->fStemWeight		 += fYYY*(float)6.25;

				 pPltN->fStemCont		+= fYYY;
				 pPltN->fLeafCont		-= fYYY;
			 }
	}

	if((pPl->pDevelop->iStageCERES == 4)||(pPl->pDevelop->iStageCERES == 5))
	{
		//calculate senescence due to N demand/////////////////////////////////////////

		if(pCan->fPlantGreenLA > (float)0.1)
		{
		//calculate nitrogen concentration of green leaves
		fNGreenLeafActConc = fNGreenLeafCont/pBiom->fGreenLeafWeight;
		//calculate residual nitrogen concentration
		fYRAT = (float)((0.009 - 0.0875*fNGreenLeafActConc)/0.9125);
		//calculate amount of nitrogen available in green leaves
		fNAvailableGreenLeaf = fNGreenLeafCont-fYRAT*pBiom->fGreenLeafWeight;
		//calculate leaf area equivalent of the amount of N in green leaves [m^2/ha]
		fLAEq = fSLOPEPE * fNAvailableGreenLeaf;
		//calculated senesced leaf area [m^2/ha]
		fZZZ = fAPLA - fLAEq;

		//calculate increment in senesced leaf area attributable to N retranslocation
		if(fZZZ > fSenesLAN2)
			fDSenesLAN2 = fZZZ - fSenesLAN2;
		else
			fDSenesLAN2 = (float)0.0;

		//calculate increment in senesced leaf area attributable to
		//water stress
		if(pPltW->fStressFacLeaf < (float)0.8)
			fDSenesLANW = (float)0.06*pCan->fPlantGreenLA;
		else
			fDSenesLANW = (float)0.0;

		if(fDSenesLANW > fDSenesLAN2)
			fDSenesLAN2 = fDSenesLANW;

		//actualize senesced plant leaf area
		fSenesLAN2 += fDSenesLAN2;
		//specific leaf area [m^2/kg]
		fSpecLeafArea = pCan->fPlantGreenLA/pBiom->fGreenLeafWeight;

		fCumSenesLA += fCumSenesLA + fSenesLAN2;

		//reduce leaf weight according to the amount of N exported
		pBiom->fLeafWeight -= fDSenesLAN2/fSpecLeafArea * 
			(fNGreenLeafActConc - fYRAT)*(float)6.25;
		pBiom->fStemWeight += fDSenesLAN2/fSpecLeafArea * 
			(fNGreenLeafActConc - fYRAT)*(float)6.25;

		pBiom->fGreenLeafWeight -= fDSenesLAN2/fSpecLeafArea;
		pBiom->fSenesLeafWeight += fDSenesLAN2/fSpecLeafArea *
			(float)((1.0-6.25*(fNGreenLeafActConc - fYRAT)));

		pCan->fPlantGreenLA -= fDSenesLAN2;
		pCan->fPlantSenesLA += fDSenesLAN2;

		}
	}


	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Tillering
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int Tillering_WH_BA(EXP_POINTER)
	{

	float 	TC1,TC2,WaterFact,NitrogFact,RTSW,fMaxTillerNumSq,fStemWeight;
    PDEVELOP 		pDev	=pPl->pDevelop;
    PBIOMASS		pBiom	=pPl->pBiomass;
    PCANOPY		pCan	=pPl->pCanopy;
	PMODELPARAM	pPltMP	=pPl->pModelParam;
    PPLTWATER		pPltW	=pPl->pPltWater;
    PPLTNITROGEN	pPltN	=pPl->pPltNitrogen;

	float G3; //Genetischer Parameter fuer SingleTillerStemWeight-Berechnung

//	G3 = (float)2.0; //Spring and Winter Barley (2 rows)
//	G3 = (float)4.0; //Spring and WinterBarley (6 rows)
//	G3 = (float)2.0; //Spring and Winter Wheat (Europe West)

	G3 = (float)2.3;

//	Umrechnung auf "physiologische" Einheiten (alter Code)
//	G3 = (float)(1.0 + (G3-1.0)*0.5);
//	G3 = (float)(-0.005 + G3*0.2);

//  SG 24.11.04: aus source code CERES von INRA (readpara.for)
	G3 = (float)(-0.005 + G3*0.35);


    //Maximum Tiller number per square meter
 	//fMaxTillerNumSq=(float)3000.0; von CERES
	fMaxTillerNumSq=(float)2000.0;

	//Stem weight (g/plant)
	fStemWeight = pBiom->fStemWeight /((float)10.0*pMa->pSowInfo->fPlantDens);
	
   	switch (pDev->iStageCERES)
		{
		case 1:
			//Previous day's tiller number (pCan->fTillerNumOld: Tillers/plant)
			pCan->fTillerNumOld=pCan->fTillerNum;

			//Today's Tiller Number pCan->fTillerNum (Tillers/plant)
			if (pDev->fCumPhyll>=(float)2.5)
				{
				TC1=((float)(-2.5))+pDev->fCumPhyll;

				if (pCan->fTillerNumSq>=fMaxTillerNumSq)
					TC2=(float)0.0;
				else
					TC2=(float)(2.50E-7*pow((fMaxTillerNumSq-pCan->fTillerNumSq),3.0));

				WaterFact =(float)1.4*pPltW->fStressFacLeaf-(float)0.4;
				NitrogFact=(float)1.4*pPltN->fNStressTiller-(float)0.4;

			   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")) //for Wheat
					pCan->fTillerNum += pCan->fLeafAppearR
					 					 *min(WaterFact,NitrogFact)
										 *min(TC1,TC2);

			   	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
			   	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) //for Barley
					pCan->fTillerNum += pCan->fLeafAppearR
										 *min(WaterFact,min(NitrogFact,pPltMP->fBarleyGrwParam))
										 *min(TC1,TC2);
			   	//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
				}

			if (pCan->fTillerNum<(float)1.0)
				pCan->fTillerNum=(float)1.0;

           	break;

		case 2:
			//Tiller Number yesterday (Tillers/plant)
			pCan->fTillerNumOld=pCan->fTillerNum;

     		//Single Tiller Weight (g/Tiller)
/*			pPltMP->fSingleTillerStemWeight += pPl->pGenotype->fStorageOrganNumCoeff
			                                 	*(float)0.0889*pDev->fDTT
					   							//*pDev->fSumDTT/(pPl->pGenotype->fPhyllochronInterval
					   							//*pPl->pGenotype->fPhyllochronInterval)
					   							*pDev->fSumDTT/(fPHINT*fPHINT)
					   							*min(pPltN->fNStressLeaf,pPltW->fStressFacPhoto);
*/
			//SG/19/11/99:
			// In CERES wird hier der genetische Parameter G3 verwendet. fStorageOrganNumCoeff ist aber G2!
			// Da f�r G3 keine Expert-N-Variable vorhanden ist, wird G3 oben gesetzt und umgerechnet (vgl.
			// PlantSimInit in alter CERES-Version

			pPltMP->fSingleTillerStemWeight += G3
			                                 	*(float)0.0889*pDev->fDTT
					   							//*pDev->fSumDTT/(pPl->pGenotype->fPhyllochronInterval
					   							//*pPl->pGenotype->fPhyllochronInterval)
					   							*pDev->fSumDTT/(fPHINT*fPHINT)
					   							*min(pPltN->fNStressLeaf,pPltW->fStressFacPhoto);

            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	    	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) //for Barley  ?????????????
				pPltMP->fSingleTillerStemWeight=G3
												*(float)(1.18*10E-6)
												*pDev->fSumDTT*pDev->fSumDTT;
            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			RTSW=fStemWeight/pPltMP->fSingleTillerStemWeight/pCan->fTillerNum;

            //Tiller Number (Tillers/plant)
			pCan->fTillerNum += pCan->fTillerNum*pDev->fDTT*(float)0.005
													*(RTSW-(float)1.0);

			if (pCan->fTillerNum<(float)1.0)
				pCan->fTillerNum=(float)1.0;

			break;

		case 3:

			
			// Wegen G3 siehe case 2:
			pPltMP->fSingleTillerStemWeight += G3
											   *pDev->fDTT*((float)0.25)
										//	   /pPl->pGenotype->fPhyllochronInterval
											   /fPHINT
											   *min(pPltW->fStressFacPhoto,pPltN->fNStressLeaf);

            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	    	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"BA")) //for Barley
				pPltMP->fSingleTillerStemWeight=G3
												*((float)0.6+(float)0.00266*pDev->fSumDTT);
            //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			RTSW=fStemWeight/pPltMP->fSingleTillerStemWeight/pCan->fTillerNum;

			pCan->fTillerNum += pCan->fTillerNum*pDev->fDTT*(float)0.005
																		   *(RTSW-(float)1.0);
			if (pCan->fTillerNum<(float)1.0)
				pCan->fTillerNum=(float)1.0;

			break;

		}

	//Tiller Number per square meter (Tillers/m2)
    pCan->fTillerNumSq=(pCan->fTillerNum+(float)1.0)*pMa->pSowInfo->fPlantDens;
//SG:
//      pCan->fTillerNumSq=pCan->fTillerNum*pMa->pSowInfo->fPlantDens;


	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//        Plant Tiller Death
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int TillerDeath_WH_BA(EXP_POINTER)
	{
	float 		DTN;
    PDEVELOP 	pDev	=pPl->pDevelop;
    PBIOMASS	pBiom	=pPl->pBiomass;
    PCANOPY	pCan	=pPl->pCanopy;

	switch (pDev->iStageCERES)
		{
		case 1:
			//Today's Senescenced Tiller Numbers per square meter (Tillers/m2)
			DTN=(pCan->fTillerNumOld-pCan->fTillerNum)
							*pMa->pSowInfo->fPlantDens;

			//Total senesecenced tiller number till today (Tillers)
			if (DTN>(float)0.0)
				pCan->fSenesTillerNumSq=pCan->fSenesTillerNumSq+DTN;

			break;

		case 2:

			if ((pCan->fTillerNumOld-pCan->fTillerNum)>(float)0.0)
				DTN=(pCan->fTillerNumOld-pCan->fTillerNum)
											*pMa->pSowInfo->fPlantDens;
            else
            	DTN=(float)0.0;

			pCan->fSenesTillerNumSq=pCan->fSenesTillerNumSq+DTN;

			break;

        }

   	return 1;
   	}



/************************************************************************************************
				Function PlantTemperature ()
************************************************************************************************/
float PlantTemperature(EXP_POINTER)
	{
	//=========================================================================================
	//Variable Declaration and Intitiation
	//=========================================================================================
	float fTempMinCrop,fTempMaxCrop,fTempAveCrop;
	LPSTR lpCropType	=pPl->pGenotype->acCropCode;
	float fTempMax		=pCl->pWeather->fTempMax;
	float fTempMin		=pCl->pWeather->fTempMin;
	float fSnow			=pCl->pWeather->fSnow;
	
    if (fSnow==(float)-99.00)
    	fSnow=(float)0.0;
    	
	//=========================================================================================
	//When no snow, the Plant Crown Temperature is assumed to be the air temperature.
	//=========================================================================================
	fTempMinCrop=fTempMin;		//fTempMinCrop -Minimum temperature of plant crown (C)
	fTempMaxCrop=fTempMax;      //fTempMaxCrop -Maximum temperature of plant crown (C)

	//=========================================================================================
	//Snow raises the Plant Crown Temperature higher than air temperature.
	//=========================================================================================
	if (fSnow>(float)15.0)	//fSnow -Precipitation in the form of Snow (mm)
		fSnow=(float)15.0;

	if (fTempMin<(float)0.0)
		fTempMinCrop=(float)2.0+fTempMin*
					 ((float)0.4+((float)0.0018)*(fSnow-(float)15.0)*(fSnow-(float)15.0));

	if (fTempMax<(float)0.0)
		fTempMaxCrop=(float)2.0+fTempMax*
					 ((float)0.4+((float)0.0018)*(fSnow-(float)15.0)*(fSnow-(float)15.0));

	//fTempAveCrop -Average temperature of plant crown (C)
	fTempAveCrop=(fTempMaxCrop+fTempMinCrop)/((float)2.0);
    
    pPl->pPltClimate->fCanopyTemp=fTempAveCrop;
    
    return fTempAveCrop;
    }
/************************************************************************************************
				End of Function PlantTemperature ()
************************************************************************************************/


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	CanopyWaterInterception_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds CanopyWaterInterception_CERES(EXP_POINTER)
	{

	return 1;
	}


/*

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//PotentialEvapotranspiration_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds PotentialEvapotranspiration_CERES(EXP_POINTER)
	{
	//	Variable Definition and Initiation
	float fTempDay,fAlbedo,fEEQ,fPotEvapTransp;
	
    PWEATHER 	pWth	=pCl->pWeather;
	PPLTWATER	pPltW	=pPl->pPltWater;
	PDEVELOP 	pDev	=pPl->pDevelop;
	PCANOPY 	pCan	=pPl->pCanopy;
	
	//=========================================================================================
	//	Day Temperature calculated based on max and min daily temperature
	//=========================================================================================
	fTempDay=(float)0.60*pWth->fTempMax+(float)0.40*pWth->fTempMin;

	//=========================================================================================
	//	Field Albedo calculation
	//=========================================================================================
	if (pDev->iStageCERES<=6)
		{
	 	if (pDev->iStageCERES>=5)
	 		fAlbedo=(float)(0.23+((double)pCan->fLAI-4.0)
	 							*((double)pCan->fLAI-4.0)/160.0);
	 	else
	 		fAlbedo =(float)(0.23-(0.23-(double)pSo->fSoilAlbedo)
	 				*exp(-0.75*((double)pCan->fLAI)));
		}
	else
		{
		if (pWth->fSnow>(float)0.5)
			fAlbedo=(float)0.6;
      	else
      		fAlbedo = pSo->fSoilAlbedo;
      	}

	//=========================================================================================
	//	Equlibrium evaporation Rate
	//=========================================================================================
	fEEQ = pWth->fSolRad*(float)(4.88E-3 - 4.37E-3*(double)fAlbedo)
								  *(fTempDay+(float)29.0);

	//=========================================================================================
	//	Potential Evapotranspiration Rate
	//=========================================================================================
	if ((pWth->fTempMax>(float)24.0)&&
			((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"WH"))
			||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))))
		fPotEvapTransp = fEEQ * ((pWth->fTempMax - (float)24.0) *((float)0.05)+(float)1.1);
	else
		{
		if ((pWth->fTempMax>=(float)5.0)&&(pWth->fTempMax<=(float)35.0))
			fPotEvapTransp = fEEQ*((float)1.1);
		else
			{
			if (pWth->fTempMax>(float)35.0)
		 		fPotEvapTransp=fEEQ*((pWth->fTempMax-(float)35.0)*((float)0.05)+(float)1.1);
			else
				fPotEvapTransp=fEEQ*((float)(0.01*exp(0.18*((double)pWth->fTempMax+20.0))));
			}
		}

	return fPotEvapTransp;
	}

*/


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PotentialTranspiration_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/* from LEACH.c */
extern float WINAPI LAI2CropCover(float fLAI);

float TranspirationRatio(EXP_POINTER);

float WINAPI _loadds PotentialTranspiration_CERES(EXP_POINTER)
	{
	//float fPotPlantTransp,fTrsRatio;

     float fPotTr;
     float fCropCov=(float)0;
	 
	 if (pPl != NULL) fCropCov=LAI2CropCover(pPl->pCanopy->fLAI);
	/* daily potential transpiration */
	//fPotTr = pWa->fPotETDay - pWa->pEvap->fPotDay;
    fPotTr = fCropCov*pWa->fPotETDay;
	//pPl->pPltWater->fPotTranspDay = fPotTr;
	fPotTraDay = fPotTr;
	
	/* time step potential transpiration */
	//pPl->pPltWater->fPotTranspdt = pWa->fPotETdt - pWa->pEvap->fPotR * DeltaT;
	
	return fPotTr;
	}

    /*
	fTrsRatio=(float)1.0;//TranspirationRatio(lpCtr,lpClm,lpPlt,lpSoil,lpMan);


	if (pPl->pCanopy->fLAI<=(float)3.0)
		fPotPlantTransp = pWa->fPotETDay*
						(float)(1.0-exp(-pPl->pCanopy->fLAI))*fTrsRatio;
	else
		fPotPlantTransp = pWa->fPotETDay * fTrsRatio;

	if (pWa->pEvap->fActDay > (float)0)
	{
	if (fPotPlantTransp+pWa->pEvap->fActDay>pWa->fPotETDay*fTrsRatio)
		fPotPlantTransp=pWa->fPotETDay*fTrsRatio-pWa->pEvap->fActDay;
	}
	else
	{
	if (fPotPlantTransp+pWa->pEvap->fPotDay>pWa->fPotETDay*fTrsRatio)
		fPotPlantTransp=pWa->fPotETDay*fTrsRatio-pWa->pEvap->fPotDay;
	}
	
	if (fPotPlantTransp<(float)0.01)
		fPotPlantTransp=(float)0.01;
    


    //pPl->pPltWater->fPotTranspDay=fPotPlantTransp;

    
	return fPotPlantTransp;
	}
	*/



//=============================================================================================
//		Plant Transpiration  Ratio
//=============================================================================================
float TranspirationRatio(EXP_POINTER)
	{
	double UAVG,RB,fLAIMAX,RLF,RLFC,CHIGHT,RATIO,TRATIO;
	double RL,RLC,EN,Z1,X,D,Z0,E1,E2,E3,E4,E5,E6;
	double RS1,RS2,RC2,RC1,RA,DAIR,CP,P,ZEPSILON;
	double VPDF1,VPDF2,DELTA,LHV,GAMMA,XNUM,XDEN,vCO2;

	vCO2=350.0; //ppm

     UAVG =2.0;

     //LEAF LEVEL BOUNDARY LEVEL RESISTANCE = RB
     RB =10.0;

	 //Maximum ((double)fLAI)
     fLAIMAX =3.5;

	//=======================================================================
	//	Calculation of Leaf Stomatal Resistance (RLF at CO2=330, 
	//											 RLFC at CO2 higher level)
	//=======================================================================
	if (  (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
     	{
        RLF    =(9.72+0.0757*330.0+10.0);
        RLFC   =(9.72+0.0757*(CO2)+10.0);
        CHIGHT =1.0;
        }
	else
		{
		// EQ 7 FROM ALLEN(1986) FOR CORN.
		RLF	=(1.0/(0.0328-5.49E-5*330.0+2.96E-8*330.0*330.0))+RB;
		
		RLFC=(1.0/(0.0328-5.49E-5*CO2+2.96E-8*vCO2*vCO2))+RB;
				
		CHIGHT=1.5;
		}
	//=======================================================================
	//			Calculation of TRATIO
	//=======================================================================
	if (pPl->pCanopy->fLAI<(float)0.01)
		TRATIO=1.0;
	else
		{
		RATIO = ((double)pPl->pCanopy->fLAI)/fLAIMAX;

      	if (RATIO>1.0) 	RATIO=1.0;
      	
		//===================================================================
		//	Calculation of Canopy Resistance (RL at CO2=330, RLC at CO2 higher level)
		//===================================================================
         RL = RLF  / ((double)pPl->pCanopy->fLAI);
         RLC= RLFC / ((double)pPl->pCanopy->fLAI);

		//===================================================================
		//	Calculation of Boundary Layer Resistance
		//===================================================================
		EN =3.0;
		Z1 =0.01;
		X  =2.0;
		D  =0.7 *pow(CHIGHT,0.979);
		Z0 =0.13*pow(CHIGHT,0.997);

		E1 = exp(EN*(1.0-(D+Z0)/CHIGHT));
		E2 = exp(EN*(CHIGHT-Z1));
		E2 = exp(EN);
		E3 = CHIGHT/(EN*(CHIGHT-D));
		E4 = log((X-D)/Z0);
		E5 = 0.4*0.4*UAVG;
		RS2= E4*E3*(E2-E1)/E5;
		E6 = log((X-D)/(CHIGHT-D));
		RC2 = E4*(E6+E3*(E1-1))/E5;
		RS1 = log(X/Z1)*log((D+Z0)/Z1)/E5;
		RC1 = (log(X/Z1)*log(X/Z1))/E5;
		RC1 = RC1-RS1;

      	RA= RC1+(RC2-RC1)*RATIO;

		//==================================================================
		//	Calculation of DELTA and GAMMA
		//===================================================================

      	DAIR =1.1400E3;		// gm/m3
      	CP   =1.00488;		// J/gm-C
      	P    =1013.0;		// mb
      	ZEPSILON =0.622;

		VPDF1=2.7489*1.0E8*exp(-4278.6/(pCl->pWeather->fTempAve+242.79));
		VPDF2=2.7489*1.0E8*exp(-4278.6/((pCl->pWeather->fTempAve+1.0)+242.79));

		DELTA = VPDF2-VPDF1;
		LHV   = 2500.9 - 2.345*pCl->pWeather->fTempAve;
		GAMMA = P*CP/(LHV*ZEPSILON);

		XNUM = DELTA + GAMMA*(1.0 + RL/RA);
		XDEN = DELTA + GAMMA*(1.0 + RLC/RA);
		TRATIO = XNUM / XDEN;
		}

	return (float)TRATIO;
	}

	

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	ActualTranspiration_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds ActualTranspiration_CERES(EXP_POINTER)
	{
	int   L;
	float TRWU,WUF,TLL,RWUMX,fExtWater,fTotSoilWater,fThickness,fContAct;
	float DUL, fRAT, fPAW;

	PSLAYER 		pSL		=pSo->pSLayer;
	PSWATER			pSWL	=pSo->pSWater;
	PWLAYER	   		pSLW	=pWa->pWLayer; 
	PLAYERROOT		pLR		=pPl->pRoot->pLayerRoot;
	PROOT			pRT		=pPl->pRoot;  
    PPLTWATER		pPltW	=pPl->pPltWater;

	pLR	=pPl->pRoot->pLayerRoot;

	// RWUMX =pPl->pGenotype->fMaxWuptRate;//RWUMX =(float)0.03;

	//SG 20120126: 
	// Abnahme der maximalen Wasseraufnahmerate RWUMX (eigentl. des Gesamtwiderstands 
	// des Xylems inkl. Stomatawiderstand!) nach Beginn der Seneszenz
    RWUMX =pPl->pGenotype->fMaxWuptRate * (float)min(1.0,max(0.0,((float)2.1 - pPl->pDevelop->fStageSUCROS)/((float)2.1 - pPl->pGenotype->fBeginSenesDvs)));


    for (L=1;L<=pSo->iLayers-2;L++)
    	{
		pLR->fActWatUpt	   	=(float)0.0;
		pLR->fActLayWatUpt 	=(float)0.0;
		pLR->fPotWatUpt		=(float)0.0;
		pLR->fPotLayWatUpt	=(float)0.0;

      	pLR =pLR ->pNext;
		}

//   	if (pPltW->fPotTranspDay<=(float)0.001)
//   	{
//      pPltW->fActTranspDay	= (float)0.0;
//    	pPltW->fStressFacPhoto 	= (float)1.0;
//		pPltW->fStressFacLeaf 	= (float)1.0;
// 		return 0;
//		}

		if (fPotTraDay<=(float)0.001)
   		{
        fActTraDay	            = (float)0.0;
    	pPltW->fStressFacPhoto 	= (float)1.0;
		pPltW->fStressFacLeaf 	= (float)1.0;
 		return 0;
		}

	//=======================================================================================
	//			Root Water Uptake
	//=======================================================================================
	L 	  = 1;
	TRWU  =(float)0.0;
	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	pLR	=pPl->pRoot->pLayerRoot;

	//ep 130899: (... || LR->pNext...))
	while (((pLR->fLengthDens!=(float)0.0)||(pLR->pNext->fLengthDens !=(float)0.0))
		  &&(L<pSo->iLayers-2))
		{
		fThickness = (float)0.1*pSL->fThickness; //mm to cm
		
        //Root water uptake per unit root length
        fExtWater = max((float)0.0,pSLW->fContAct-pSWL->fContPWP);
        
        if (pSLW->fContAct<=pSWL->fContPWP)
        	pLR->fPotWatUpt=(float)0.0;
        else
        	{
        	//pLR->fPotWatUpt=(float)(2.67*1E-3*exp(min(62.0*(double)fExtWater,10.0))
            //								/(6.68-log((double)pLR->fLengthDens)));
            
			//INRA-Ceres:
			pLR->fPotWatUpt=(float)(2.67*1E-3*exp(min(35.0*(double)fExtWater,10.0))
            								/(6.68-log((double)pLR->fLengthDens)));

            pLR->fPotWatUpt = min(RWUMX,pLR->fPotWatUpt);
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
	
//	if (pPltW->fPotTranspDay<=TRWU)
//    	WUF = pPltW->fPotTranspDay/TRWU;

	if (fPotTraDay<=TRWU)
    	WUF = fPotTraDay/TRWU;
    else
        WUF = (float)1.0;

    TLL=(float)0.0;
    DUL=(float)0.0;

	fTotSoilWater=(float)0.0;
    fActTraDay=(float)0.0;
 	pPl->pPltWater->fActUptakedt=(float)0.0;

	pSL	=pSo->pSLayer->pNext;
	pSWL=pSo->pSWater->pNext;
	pSLW=pWa->pWLayer->pNext; 
	pLR	=pPl->pRoot->pLayerRoot;
    for (L=1;L<=pSo->iLayers-2;L++)
    	{
		pLR->fActWatUpt	 	=  pLR->fPotWatUpt *WUF;
		pLR->fActLayWatUpt 	=  pLR->fPotLayWatUpt*WUF;

		fContAct  = pSLW->fContAct;
		fContAct -= pLR->fActLayWatUpt/pSL->fThickness; //mm/mm

		if (fContAct<pSWL->fContPWP)
			{
			pLR->fActWatUpt	   *= fContAct/pSWL->fContPWP;
			pLR->fActLayWatUpt *= fContAct/pSWL->fContPWP;
//			pSLW->fContAct=pSWL->fContPWP;
			}

		//fTotSoilWater += fContAct*fThickness*(float)10.0;
		fTotSoilWater 	+= fContAct*pSL->fThickness;

      	pPl->pPltWater->fActUptakedt += pLR->fActLayWatUpt;
		//TLL   += pSWL->fContPWP*fThickness*(float)10.0;
		//DUL   += pSWL->fContFK*fThickness*(float)10.0;
		TLL   += pSWL->fContPWP*pSL->fThickness;
		DUL   += pSWL->fContFK*pSL->fThickness;

      	pSL =pSL ->pNext;
      	pSWL=pSWL->pNext;
      	pSLW=pSLW->pNext;
      	pLR =pLR ->pNext;
		}

   
//    if (pPltW->fPotTranspDay>=TRWU)
//        pPltW->fActTranspDay=TRWU;
//    else
//        pPltW->fActTranspDay=pPltW->fPotTranspDay;
//        
//	pPl->pRoot->fUptakeR = pPltW->fActTranspDay;    
	

	if (fPotTraDay>=TRWU)
        fActTraDay=TRWU;
    else
        fActTraDay=fPotTraDay;
        
	//pPl->pRoot->fUptakeR = fActTraDay;    
	pPl->pRoot->fUptakeR = pPl->pPltWater->fActUptakedt;    
	

	//=======================================================================================
	//	Soil Water Deficit Factors
	//=======================================================================================

//    if (pPl->pPltWater->fPotUptakedt/pPl->pPltWater->fPotTranspdt<(float)1.5)
//    	  pPl->pPltWater->fStressFacLeaf = ((float)0.67)*pPl->pPltWater->fPotUptakedt/pPl->pPltWater->fPotTranspdt;
    if (pPl->pPltWater->fPotUptakedt/fPotTraDay<(float)1.5)
    	pPl->pPltWater->fStressFacLeaf = ((float)0.67)*pPl->pPltWater->fPotUptakedt/fPotTraDay;
    else
        pPl->pPltWater->fStressFacLeaf = (float)1.0;
		
	//INRA-Ceres:
	if (pPl->pDevelop->iStageCERES>=4)
	{
		if (pPl->pPltWater->fPotUptakedt/fPotTraDay+(float)0.25<(float)1.5)
    		pPl->pPltWater->fStressFacLeaf = (float)0.67*(pPl->pPltWater->fPotUptakedt/fPotTraDay+(float)0.25);
		else
			pPl->pPltWater->fStressFacLeaf = (float)1.0;
	}


//    if (pPl->pPltWater->fPotTranspdt>=pPl->pPltWater->fPotUptakedt)
//        pPl->pPltWater->fStressFacPhoto	=pPl->pPltWater->fPotUptakedt/pPl->pPltWater->fPotTranspdt;
    if (fPotTraDay>=pPl->pPltWater->fPotUptakedt)
        pPl->pPltWater->fStressFacPhoto	= pPl->pPltWater->fPotUptakedt/fPotTraDay;
	    //pPl->pPltWater->fStressFacPhoto = (float)pow(pPl->pPltWater->fPotUptakedt/fPotTraDay,0.5);
    else
        pPl->pPltWater->fStressFacPhoto	=(float)1.0;

	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
	{
		fPAW = (float)max(0.0,(fTotSoilWater - TLL)/(DUL - TLL));
		
		if(pWa->fPotETDay > (float)0.0)
			fRAT = (float)10.0*fPAW/pWa->fPotETDay;
		else
			fRAT = (float)1.0;

		fRAT = (float)min(1.0,fRAT);

		if((pPl->pPltWater->fStressFacPhoto < (float)1.0)&&
				(fRAT > (float)0.5*pPl->pPltWater->fStressFacPhoto))
			pPl->pPltWater->fStressFacLeaf = (float)0.5*pPl->pPltWater->fStressFacPhoto;
		else
			pPl->pPltWater->fStressFacLeaf = fRAT;
	}


    
//  pPl->pPltWater->fShortage=pPl->pPltWater->fPotTranspdt-pPl->pPltWater->fActTranspdt;	
	pPl->pPltWater->fShortage=fPotTraDay-fActTraDay;	
	
//	pPl->pPltWater->fCumDemand   += pPl->pPltWater->fPotTranspdt;
	pPl->pPltWater->fCumShortage += pPl->pPltWater->fShortage;
	
	
//	if (pPl->pDevelop->iStageCERES<2)
//		{
//		pPl->pPltWater->fStressFacPhoto		=(float)1.0;
//		pPl->pPltWater->fStressFacLeaf		=(float)1.0;
//		pPl->pPltWater->fStressFacTiller	=(float)1.0;
//		pPl->pPltNitrogen->fNStressPhoto	=(float)1.0;
//		pPl->pPltNitrogen->fNStressLeaf		=(float)1.0;
//		pPl->pPltNitrogen->fNStressTiller	=(float)1.0;
//
//		}

	return 1;
	}
	

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantWaterStress_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantWaterStress_CERES(EXP_POINTER)
	{

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	CritialNitrogenConcentration_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds NitrogenConcentrationLimits_CERES(EXP_POINTER)
	{
	double ZS2,ZS3,ZS4;
	float  P1V, fStage;

    
	PBIOMASS		pBiom = pPl->pBiomass;
    PDEVELOP 		pDev  = pPl->pDevelop;
    PPLTNITROGEN	pPltN = pPl->pPltNitrogen;
    
//	P1V=(float)pPl->pGenotype->iVernCoeff;
	P1V=(float)1.0/pPl->pGenotype->iVernCoeff;
	fStage = (float)0.1 * pDev->fDevStage;

	//============================================================================
	//Tops Nitrogen Concentration of the Plant - Stage dependant
	//============================================================================
	ZS2=(double)(fStage*fStage);
	ZS3=(double)(fStage*fStage*fStage);
	ZS4=(double)(fStage*fStage*fStage*fStage);

	
	//Wheat, Barley:
	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
	{
	if (P1V>=(float)0.03)
		pPltN->fTopsOptConc=(float)(-5.0112400-6.350677*(double)fStage
		 							+14.95784*sqrt((double)fStage)
									+0.2238197*ZS2);
	else
		pPltN->fTopsOptConc=(float)( 7.4531813-1.7907829*(double)fStage
									+0.6092849*sqrt((double)fStage)
									+0.0933967*ZS2);
//	if (pDev->fDevStage>(float)6.0)
//		pPltN->fTopsOptConc -=(pDev->fDevStage-(float)6.0)*(float)0.140;

	pPltN->fTopsOptConc	=pPltN->fTopsOptConc*(float)0.01;
	//pPltN->fTopsMinConc	=(float)((2.97-0.455*(double)pDev->fStageXCERES)*0.01);

	// SPASS:	
	pPltN->fTopsMinConc	= (float)0.005;

	if (pDev->iStageCERES>=5)
		{
		pPltN->fVegActConc	=pPltN->fTopsActConc;
		pPltN->fVegMinConc	=pPltN->fTopsMinConc;
        }

	//============================================================================
	//Roots Nitrogen Concentration of the Plant - Stage dependant
	//============================================================================
	pPltN->fRootOptConc	=(float)((2.10-0.14*sqrt((double)fStage))*0.01);
	pPltN->fRootMinConc	=((float)0.75)*pPltN->fRootOptConc;
	}//end wheat, barley

	//Maize:
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
	{
	pPl->pPltNitrogen->fTopsOptConc=(float)(exp(1.52-0.160*fXDevStage)/100.0);

	if (pPl->pDevelop->fStageXCERES<(float)4.0)
		pPl->pPltNitrogen->fTopsMinConc=(float)((1.25-0.20*fXDevStage)/100.0);
	else
		pPl->pPltNitrogen->fTopsMinConc=(float)0.0045;


	pPl->pPltNitrogen->fRootOptConc=(float)(1.06/100.0);


	fNFAC=(float)1.0-(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsActConc)
	               /(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsMinConc);

	if (fNFAC>(float)1.0) fNFAC=(float)1.0;

	if ((pPl->pDevelop->iStageCERES==3)||(pPl->pDevelop->iStageCERES==4))
		fNFAC=(float)(1.0-1.80*exp(-3.5*(double)fNFAC));

	if (fNFAC<(float)0.001)  fNFAC=(float)0.001;


	pPl->pPltNitrogen->fNStressPhoto=(float)1.0;
	pPl->pPltNitrogen->fNStressLeaf=(float)1.0;
	pPl->pPltNitrogen->fNStressTiller=(float)1.0;

	pPl->pPltNitrogen->fNStressLeaf=fNFAC;

	if (pPl->pPltNitrogen->fNStressFac<(float)0.8) 
		pPl->pPltNitrogen->fNStressTiller=(float)0.2+fNFAC;

    pPl->pPltNitrogen->fNStressPhoto=fNFAC*(float)1.2+(float)0.2;

	if (fNFAC>(float)0.5)
		pPl->pPltNitrogen->fNStressPhoto=(float)0.4*fNFAC+(float)0.6;

	/*c  If(Nfac.lt.0.7)ndef1=nfac*(nfac+0.2) */

//	if (pPl->pPltNitrogen->fNStressPhoto>(float)1.0) pPl->pPltNitrogen->fNStressPhoto=(float)1.0;
	if (pPl->pPltNitrogen->fNStressLeaf>(float)1.0) pPl->pPltNitrogen->fNStressLeaf=(float)1.0;
	if (pPl->pPltNitrogen->fNStressTiller>(float)1.0) pPl->pPltNitrogen->fNStressTiller=(float)1.0;
}//end maize

	//Sunflower:
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
	{
		//optimal nitrogen concentrations
		if (pPl->pGenotype->OptLvNc[0].fInput==(float)0.0)
			pPltN->fLeafOptConc = (float)(0.0294*exp(-0.326*fXDevStage) + 0.0326);
		else
			pPltN->fLeafOptConc = AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptLvNc);
		
		if (pPl->pGenotype->OptStNc[0].fInput==(float)0.0)
			pPltN->fStemOptConc = (float)(0.0329*exp(-0.516*fXDevStage) + 0.0125);
		else
			pPltN->fStemOptConc = AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptStNc);
		
		if (pPl->pGenotype->OptRtNc[0].fInput==(float)0.0)
			pPltN->fRootOptConc = (float)(0.0361*exp(-0.521*fXDevStage) + 0.0105);
		else
			pPltN->fRootOptConc = AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->OptRtNc);
		
		//f�r Head kein Eintrag in Sunflower.gtp
		fNHeadOptConc = (float)(0.2137*exp(-0.6*fXDevStage) + 0.016);
		
		
		//minimal nitrogen concentrations
		if (pPl->pGenotype->MinLvNc[0].fInput==(float)0.0)
			pPltN->fLeafMinConc = pPltN->fLeafOptConc - (float)0.02;
		else
			pPltN->fLeafMinConc = AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinLvNc);
		
		if (pPl->pGenotype->MinStNc[0].fInput==(float)0.0)
			pPltN->fStemMinConc = pPltN->fStemOptConc - (float)0.0112;
		else
			pPltN->fStemMinConc = AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinStNc);
		
		if (pPl->pGenotype->MinRtNc[0].fInput==(float)0.0)
			pPltN->fRootMinConc = pPltN->fRootOptConc - (float)0.0062;
		else
			pPltN->fRootMinConc = AFGENERATOR(pDev->fStageSUCROS, pPl->pGenotype->MinRtNc);
		
		//f�r Head kein Eintrag in Sunflower.gtp
			fNHeadMinConc = fNHeadOptConc - (float)0.00895;

		pPltN->fTopsOptConc = (pPltN->fLeafOptConc*pBiom->fGreenLeafWeight + 
				pPltN->fStemOptConc*pBiom->fStemWeight + fNHeadOptConc*fHeadWeight)
				/(pBiom->fStovWeight - pBiom->fSenesLeafWeight);

		pPltN->fTopsMinConc = (pPltN->fLeafMinConc*pBiom->fGreenLeafWeight + 
				pPltN->fStemMinConc*pBiom->fStemWeight + fNHeadMinConc*fHeadWeight)
				/(pBiom->fStovWeight - pBiom->fSenesLeafWeight);

		}//end sunflower


	pPltN->fTopsOptConc	=max((float)0.0,pPltN->fTopsOptConc);
	pPltN->fTopsMinConc	=max((float)0.0,pPltN->fTopsMinConc);
	pPltN->fLeafOptConc	=max((float)0.0,pPltN->fLeafOptConc);
	pPltN->fLeafMinConc	=max((float)0.0,pPltN->fLeafMinConc);
	pPltN->fStemOptConc	=max((float)0.0,pPltN->fStemOptConc);
	pPltN->fStemMinConc	=max((float)0.0,pPltN->fStemMinConc);
	pPltN->fRootOptConc	=max((float)0.0,pPltN->fRootOptConc);
	pPltN->fRootMinConc	=max((float)0.0,pPltN->fRootMinConc);
	fNHeadOptConc		=max((float)0.0,fNHeadOptConc);
	fNHeadMinConc		=max((float)0.0,fNHeadMinConc);

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantNitrogenDemand_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
extern float fPotShootGrowRate,fPotRootGrowRate;

int WINAPI _loadds PlantNitrogenDemand_CERES(EXP_POINTER)
	{
	float			fNewGrwNdem; //fTRNLOS;
	PBIOMASS		pBiom=pPl->pBiomass;
	PPLTNITROGEN	pPltN=pPl->pPltNitrogen;

	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")))
	{

		if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
		{
			//===========================================
			//		Potential Shoot and Root Growth
			//===========================================
			fPotShootGrowRate = fPotBiomGrowRate
						    *((float)1.0-pBiom->fRootGrowR/pBiom->fBiomGrowR);
			fPotRootGrowRate  = fPotBiomGrowRate
    						*pBiom->fRootGrowR/pBiom->fBiomGrowR;
		}


		//Nitrogen demand of potential new growth of tops fNewGrwNdem(g N/plant)
		fNewGrwNdem=pBiom->fMaxShootGrowR*pPltN->fTopsOptConc;

		if (pPl->pDevelop->fStageXCERES<=(float)1.2)
			fNewGrwNdem=(float)0.0;

		pBiom->fStovWeight=pBiom->fLeafWeight+pBiom->fStemWeight+fEarWeight;

		//Plant tops demand for nitrogen: fTNDEM(kg N/ha)
		pPltN->fShootDemand	=(float)max(0.0,pBiom->fStovWeight*
					(pPltN->fTopsOptConc-pPltN->fTopsActConc)+fNewGrwNdem);
	
		//Plant root demand for nitrogen: fRNDEM(kg N/ha)
		pPltN->fRootDemand	=(float)max(0.0,pBiom->fRootWeight*(pPltN->fRootOptConc-
				pPltN->fRootActConc)+fPotRootGrowRate*pPltN->fRootOptConc);

		//Crop nitrogen demand per ha: fANDEM(Kg N/ha)
		pPltN->fTotalDemand=(float)max(0.0,pPltN->fShootDemand+pPltN->fRootDemand);
	}

	//Sunflower:
	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
	{
		//optimal N-concentration of pericarp and embryo
		fNEmbryoOptConc = (float)(0.0225+0.0200*fNFAC);
		fNPericarpOptConc = (float)(0.0050+0.0100*fNFAC);

		//reset N loss by roots due to root exsudation
		//fTRNLOS = (float)0.0;

		if(iJPEPE == 1 )
		{
			fNHeadCont = (float)0.042* fHeadWeight;
			pPltN->fStemCont -= fNHeadCont;
			iJPEPE = 0;
		}

		// Actual N concentrations in plant parts before N uptake
		pPltN->fRootActConc=pPltN->fRootCont/pBiom->fRootWeight;
		pPltN->fStemActConc=pPltN->fStemCont/pBiom->fStemWeight;
		pPltN->fLeafActConc=fNGreenLeafCont/pBiom->fGreenLeafWeight;
		
		if(fHeadWeight > (float)0.0)
			fNHeadActConc=fNHeadCont/fHeadWeight;

		pPltN->fTopsActConc=(pPltN->fStemCont+fNGreenLeafCont+fNHeadCont)/
			(pBiom->fStovWeight-pBiom->fSenesLeafWeight);

	//	fTotWeight2 = pBiom->fStemWeight + pBiom->fGreenLeafWeight + fHeadWeight
	//		+ fPericarpWeight + fEmbryoWeight;

	//	fNTopsCritConc = (pPltN->fLeafOptConc * pBiom->fGreenLeafWeight +
	//		pPltN->fStemOptConc * pBiom->fStemWeight + fNHeadOptConc * fHeadWeight +
	//		(float)0.015 * fPericarpWeight + (float)0.0425 * fEmbryoWeight)/fTotWeight2;


		pPltN->fRootDemand	=pBiom->fRootWeight*(pPltN->fRootOptConc-pPltN->fRootActConc);
		  				//+pBiom->fRootGrowR*pPltN->fRootOptConc;

		pPltN->fStemDemand	=pBiom->fStemWeight*(pPltN->fStemOptConc-pPltN->fStemActConc);
		  				//+pBiom->fStemGrowR*pPltN->fStemOptConc;

		pPltN->fLeafDemand	=pBiom->fGreenLeafWeight*(pPltN->fLeafOptConc-pPltN->fLeafActConc);
		  				//+pBiom->fLeafGrowR*pPltN->fLeafOptConc;

	
		if((fHeadWeight > (float)0.0) || (fHeadGrowR > (float)0.0))
			fNHeadDemand = fHeadWeight*(fNHeadOptConc-fNHeadActConc); //+ fHeadGrowR*fNHeadOptConc;
		
		if(fPericarpWeight > (float)0.0)
			fNPericarpDemand = fPericarpGrowR * fNPericarpOptConc;
		else
			fNPericarpDemand = (float)0.0;

		if(fEmbryoWeight > (float)0.0)
			fNEmbryoDemand   = fEmbryoGrowR * fNEmbryoOptConc;
		else
			fNEmbryoDemand = (float)0.0;

		pPltN->fTotalDemand = pPltN->fRootDemand+pPltN->fStemDemand+pPltN->fLeafDemand+
			fNHeadDemand + fNPericarpDemand + fNEmbryoDemand; //[kg(N)/ha]

		if(pPltN->fTotalDemand==(float)0.0)
			pPltN->fTotalDemand = (float)(0.001*pMa->pSowInfo->fPlantDens*10.0);
	
	}


	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	NitrogenUptake_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds NitrogenUptake_CERES(EXP_POINTER)
	{
    int		L,L1;
    float 	fTotRtLenDens,fANDEM;
	float 	fFNO3,fFNH4,fSMDFR,fRFAC,fNUF,fTRNU;
    float 	fUNO3,fUNH4,SNO3min,SNH4min,RNO3U[MAXSCHICHT],RNH4U[MAXSCHICHT];

   	PSLAYER pSL	=pSo->pSLayer->pNext;
	PSWATER	pSWL=pSo->pSWater->pNext;
	PWLAYER	pSLW=pWa->pWLayer->pNext; 
	PCLAYER pSLN=pCh->pCLayer->pNext;

	PLAYERROOT	 pLR  =pPl->pRoot->pLayerRoot;
	PBIOMASS	 pBiom=pPl->pBiomass;
	PPLTNITROGEN pPltN=pPl->pPltNitrogen;
    
    static int Uptake=0; 
    
	//===========================================================================================
	//Exam whether uptake occurs
	//===========================================================================================
    if ((pBiom->fRootWeight==(float)0.0))
    	return 0;

	if (pBiom->fRootGrowR>(float)0.0)  	
		Uptake=1;
	if (Uptake==0)						
		return 0;
 	
	//======================================================================================
	//Layer Nitrogen Initiation and Transformation
	//======================================================================================
	fTotRtLenDens=(float)0.0;
    pLR=pPl->pRoot->pLayerRoot;

	for (L=1;L<=pSo->iLayers-2;L++)
		{
		RNO3U[L]=(float)0.0;
		RNH4U[L]=(float)0.0;

		fTotRtLenDens += pLR->fLengthDens;

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
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG")))
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

 		if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
			{
			fFNO3=(float)(1.0-exp(-0.030*((double)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
			fFNH4=(float)(1.0-exp(-0.030*(((double)(pSLN->fNH4N/pSLN->fNH4Nmgkg))-0.5)));
		  	if (fFNO3<(float)0.04) fFNO3=(float)0.0;
		  	if (fFNH4<(float)0.04) fFNH4=(float)0.0;
			}

		if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		    ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
			{
		  	fFNO3=(float)(1.0-exp(-0.0275*((float)(pSLN->fNO3N/pSLN->fNO3Nmgkg))));
		  	fFNH4=(float)(1.0-exp(-0.0250*((float)(pSLN->fNH4N/pSLN->fNH4Nmgkg)-0.5)));
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

		
		//fRFAC: Effect of water stress and root length density on potential nitrogen uptake
		//[cm cm-3][0.1 mm][100] = [cm-1][100]=[m-1]
		fRFAC=pLR->fLengthDens*fSMDFR*fSMDFR*(float)0.1*pSL->fThickness*((float)100.0);

		//Potential nitrogen uptake from layer L: RNO3[L],RNH4U[L](kg N/ha)
		//[m-1][-][(kg/ha)/(m/m2) d-1]=[m-1][kg ha-1 m d-1]= [kg ha-1 d-1]
		RNO3U[L]=fRFAC*fFNO3*pPl->pGenotype->fMaxNuptRate;
		RNH4U[L]=fRFAC*fFNH4*pPl->pGenotype->fMaxNuptRate;

		//??? what is the dimension of input for the max. uptake rate?
		//uptake: [kg cm-3]=[kg 10-4 m-2 cm-1]=[kg ha-1 cm-1]
		//[cm-1][-][kg cm-1 d-1]=[kg cm-2 d-1]=[kg 10-4 m-2 d-1]=[kg ha-1 d-1]
		//[100 cm-1][-][kg cm-1 d-1]=[kg 100 cm-2 d-1]= kg 100 10-4 m-2 d-1] = 100 [kg ha-1 d-1]
		//[100 mm-1][-][kg mm-1 d-1]=[kg 100 mm-2 d-1]=[kg 100 10-6 m-2 d-1] = [kg ha-1 d-1] 

		//Total Potential Root Nitrogen Uptake: fTRNU(kg N/ha)
		fTRNU=fTRNU+max((float)0.0,RNO3U[L]+RNH4U[L]);
                                    
		pSL =pSL ->pNext;
		pSWL=pSWL->pNext;
		pSLW=pSLW->pNext;
		pSLN=pSLN->pNext;
		pLR =pLR ->pNext;
		}

	//======================================================================================
	//Nitrogen uptake factor relating supply and plant demand: fNUF (dimensionless)
	//======================================================================================
	fANDEM=pPltN->fTotalDemand;
	
	if (fANDEM<=(float)0.0)
        fNUF=(float)0.0;
	else
		{
      	if (fANDEM>fTRNU) fANDEM=fTRNU;
    
		if (fTRNU==(float)0.0) 	return 0;
      	fNUF=fANDEM/fTRNU;
      	}            

	//======================================================================================
	//Actual Nitrogen Uptake  and Nitrogen Loss form Died Roots
	//======================================================================================
	fTRNU=(float)0.0; 	//fTRNU is now the actual total root nitrogen uptake (kg N/ha)

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
		//SNO3min=(float)1.25*pSLN->fNO3Nmgkg;
		//SNH4min=(float)2.5*pSLN->fNH4Nmgkg;

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

	//SG20151127: f�r Ausgabe in *.rfp
	pPltN->fActNUpt = pPltN->fActNUptR;

    pPltN->fCumTotalDemand	+=  pPltN->fTotalDemand;

	return 1;
	}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	PlantNitrogenStress_CERES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PlantNitrogenStress_CERES(EXP_POINTER)
	{ 
	float			fPoolAvailability,RGNFIL, factor, fTempFac, fWatFac, fGrainIncNConc;
	PWEATHER		pWth =pCl->pWeather;
	PBIOMASS		pBiom=pPl->pBiomass;
	PPLTNITROGEN 	pPltN=pPl->pPltNitrogen;
	


	//Wheat, Barley:
	if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
		  ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")))
	{
	//======================================================================================
	//	Change in Plant Nitrogen Content due to nitrogen exudation
	//======================================================================================
	//Daily Change in plant nitrogen content due to exudation
	if (pPltN->fTotalDemand==(float)0.0)
		{
		pPltN->fStovContChange	=(float)0.0;
		pPltN->fRootContChange	=(float)0.0;
		}
	else
		{
		pPltN->fStovContChange	=pPltN->fShootDemand/pPltN->fTotalDemand
		                      *pPltN->fActNUptR;
		pPltN->fRootContChange	=pPltN->fRootDemand/pPltN->fTotalDemand
								*pPltN->fActNUptR;
		}

	//Stover and root nitrogen content:(g N/plant)
	pPltN->fStovCont 	+= pPltN->fStovContChange;
	pPltN->fRootCont	    += pPltN->fRootContChange;


	//Actual nitrogen concentration for tops and root
	pBiom->fStovWeight=pBiom->fLeafWeight+pBiom->fStemWeight+fEarWeight;

	pPltN->fTopsActConc=pPltN->fStovCont/pBiom->fStovWeight;
	pPltN->fVegActConc =pPltN->fStovCont/pBiom->fStovWeight;
	pPltN->fRootActConc=pPltN->fRootCont/(pBiom->fRootWeight-(float)0.01*pBiom->fRootWeight);

	pPltN->fRootActConc	=max(pPltN->fRootMinConc,pPltN->fRootActConc);
   	pPltN->fVegActConc	=max(pPltN->fVegActConc, pPltN->fVegMinConc);
   	pPltN->fTopsActConc	=max(pPltN->fTopsActConc,pPltN->fTopsMinConc);

	//======================================================================================
	//	Nitrogen Available in the Stover and Root
	//======================================================================================
	//Potential Nitrogen Available in the Stover and root
	pPltN->fStovNPool =pBiom->fStovWeight*(pPltN->fVegActConc-pPltN->fVegMinConc);
	pPltN->fRootNPool	=pBiom->fRootWeight*(pPltN->fRootActConc-pPltN->fRootMinConc);

	pPltN->fStovNPool=max((float)0.0,pPltN->fStovNPool);
	pPltN->fRootNPool	=max((float)0.0,pPltN->fRootNPool);

	//Availability Factor
	pPltN->fNStressFac=(float)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
			               /(pPltN->fTopsOptConc-pPltN->fTopsMinConc);

	pPltN->fNStressFac=min((float)1.0,max((float)0.0,pPltN->fNStressFac));

	fPoolAvailability=(float)0.15+((float)0.20)*pPltN->fNStressFac;

	//Actual Available Nitrogen in the Stover and root and total
	pPltN->fStovNLabile	=fPoolAvailability*pPltN->fStovNPool;
	pPltN->fRootNLabile		=fPoolAvailability*pPltN->fRootNPool;
	pPltN->fTotalNLabile	=pPltN->fStovNLabile+pPltN->fRootNLabile;

	//============================================================================
	//Grain nitrogen demand
	//============================================================================
	RGNFIL=   (float)4.829666-((float)3.2488)*pPl->pDevelop->fDTT
			+((float)0.2503)*(pWth->fTempMax-pWth->fTempMin)
		    +((float)4.3067)*pWth->fTempAve;

	if (pWth->fTempAve<=(float)10.0) RGNFIL=((float)0.49)*pWth->fTempAve;

	pPltN->fGrainDemand =RGNFIL*pPl->pCanopy->fGrainNum*((float)1.0E-6); 			//g/plant
	pPltN->fGrainDemand =pPltN->fGrainDemand*(float)10.0*pMa->pSowInfo->fPlantDens;//kg/ha
	pPltN->fGrainDemand =min(pPltN->fTotalNLabile,pPltN->fGrainDemand);

	//======================================================================================
	//Nitrogen translocation to grain
	//======================================================================================
	if (pPltN->fGrainDemand!=(float)0.0)
		{
		if (pPltN->fGrainDemand>pPltN->fStovNLabile)
			{
			pPltN->fStovCont	-=  pPltN->fStovNLabile;
			pPltN->fRootCont 	-= (pPltN->fGrainDemand-pPltN->fStovNLabile);
			pPltN->fRootActConc		=  pPltN->fRootCont/pBiom->fRootWeight;
		   	pPltN->fVegActConc		= pPltN->fStovCont/pBiom->fStovWeight;
			pPltN->fTopsActConc		= pPltN->fVegActConc;
			pPltN->fStovNLabile=(float)0.0;
			}
		else
			{
		   	pPltN->fStovCont	  -= pPltN->fGrainDemand;
		   	pPltN->fVegActConc		= pPltN->fStovCont/pBiom->fStovWeight;
			pPltN->fTopsActConc		= pPltN->fVegActConc;
			}
		}

	//======================================================================================
	//	Grain Nitrogen Content
	//======================================================================================
	pPltN->fGrainCont += pPltN->fGrainDemand;
	pPltN->fGrainConc  = pPltN->fGrainCont/(pBiom->fGrainWeight+(float)1.0E-9);

	//============================================================================
	// 		Nitrogen Factors affecting Growth
	//============================================================================
	pPltN->fNStressFac=(float)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
			               /(pPltN->fTopsOptConc-pPltN->fTopsMinConc);

	if (pPltN->fNStressFac<(float)0.02)					pPltN->fNStressFac=(float)0.02;
	if (pPl->pDevelop->fStageXCERES<=(float)1.10) 	pPltN->fNStressFac=(float)1.0;

	pPltN->fNStressGrain=pPltN->fNStressFac*pPltN->fNStressFac;

	if(pPltN->fNStressGrain>(float)1.5)
		pPltN->fNStressGrain=(float)1.5;

	if (pPltN->fTopsActConc>=pPltN->fTopsOptConc)
		pPltN->fNStressFac=(float)1.0;

	pPltN->fNStressTiller=pPltN->fNStressFac*pPltN->fNStressFac;
	pPltN->fNStressPhoto=(float)0.10+(float)2.0*pPltN->fNStressFac;
	pPltN->fNStressLeaf=pPltN->fNStressFac;

	if (pPltN->fNStressLeaf>(float)1.0)		pPltN->fNStressLeaf=(float)1.0;
	if (pPltN->fNStressPhoto>(float)1.0)		pPltN->fNStressPhoto=(float)1.0;
	if (pPltN->fNStressTiller>(float)1.0)		pPltN->fNStressTiller=(float)1.0;

//	auskommentiert am 18.11.99. Gerste-Simulation funktioniert dann viel besser
	/*
	if (pPl->pDevelop->iStageCERES<2)
		{
		//pPltN->fNStressPhoto=(float)1.0;
		pPltN->fNStressLeaf=(float)1.0;
		pPltN->fNStressTiller=(float)1.0;
		}
    */

	}//end wheat, barley

	//Maize:
	if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
	{
	//======================================================================================
	//	Change in Plant Nitrogen Content due to nitrogen exudation
	//======================================================================================
	//Daily Change in plant nitrogen content due to exudation
	if (pPltN->fTotalDemand==(float)0.0)
		{
		pPltN->fStovContChange	=(float)0.0;
		pPltN->fRootContChange	=(float)0.0;
		}
	else
		{
		pPltN->fStovContChange	=pPltN->fShootDemand/pPltN->fTotalDemand
		                      *pPltN->fActNUptR;
		pPltN->fRootContChange	=pPltN->fRootDemand/pPltN->fTotalDemand
								*pPltN->fActNUptR;
		}

	//Stover and root nitrogen content:(g N/plant)
	pPltN->fStovCont 	+= pPltN->fStovContChange;
	pPltN->fRootCont	+= pPltN->fRootContChange;


	//Actual nitrogen concentration for tops and root
	pBiom->fStovWeight=pBiom->fLeafWeight+pBiom->fStemWeight+fEarWeight;

	pPltN->fTopsActConc=pPltN->fStovCont/pBiom->fStovWeight;
	pPltN->fVegActConc =pPltN->fStovCont/pBiom->fStovWeight;
	pPltN->fRootActConc=pPltN->fRootCont/(pBiom->fRootWeight-(float)0.01*pBiom->fRootWeight);

	pPltN->fRootActConc	=max(pPltN->fRootMinConc,pPltN->fRootActConc);
   	pPltN->fVegActConc	=max(pPltN->fVegActConc, pPltN->fVegMinConc);
   	pPltN->fTopsActConc	=max(pPltN->fTopsActConc,pPltN->fTopsMinConc);

	
	//============================================================================
	//Grain nitrogen demand
	//============================================================================
	
	fTempFac = (float)0.69 + (float)0.0125*(pWth->fTempMax-pWth->fTempMin)/(float)2.0;
	fWatFac  = (float)1.125 - (float)0.125 * pPl->pPltWater->fStressFacLeaf;

	//Grain nitrogen concentration on the day
	//CERES-Maize pp.84
	fGrainIncNConc = (float)((0.007+0.01*pPltN->fNStressLeaf)*max(fTempFac,fWatFac));
	//altesExpert-N:
	//fGrainIncNConc = (float)((0.004+0.013*pPltN->fNStressLeaf)*max(fTempFac,fWatFac));

	//Total nitrogen Demand of the growing grain
	pPltN->fGrainDemand = pBiom->fGrainGrowR*fGrainIncNConc;

	pPltN->fRootOptConc = (float)((2.5-0.25*fXDevStage)/100.0);

	//======================================================================================
	//Nitrogen translocation to grain
	//======================================================================================
	if (pPltN->fGrainDemand!=(float)0.0)
	{
		pPltN->fRootMinConc = (float)0.75*pPltN->fRootOptConc;
		if(pPltN->fRootActConc < pPltN->fRootMinConc)
			pPltN->fRootActConc = pPltN->fRootMinConc;

		pPltN->fVegActConc = pPltN->fStovCont/pBiom->fStovWeight;
		if(pPltN->fVegActConc < pPltN->fVegMinConc)
			pPltN->fVegActConc = pPltN->fVegMinConc;

		//======================================================================================
		//	Nitrogen Available in the Stover and Root
		//======================================================================================
		//Potential Nitrogen Available in the Stover and root
		pPltN->fStovNPool =pBiom->fStovWeight*(pPltN->fVegActConc-pPltN->fVegMinConc);
		pPltN->fRootNPool	=pBiom->fRootWeight*(pPltN->fRootActConc-pPltN->fRootMinConc);
	
		pPltN->fStovNPool=max((float)0.0,pPltN->fStovNPool);
		pPltN->fRootNPool	=max((float)0.0,pPltN->fRootNPool);
	
		//Availability Factor
		pPltN->fNStressFac=(float)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
							/(pPltN->fTopsOptConc-pPltN->fTopsMinConc);
	
		pPltN->fNStressFac=min((float)1.0,max((float)0.0,pPltN->fNStressFac));
	
		fPoolAvailability=(float)0.15+((float)0.25)*pPltN->fNStressFac;
	
		//Actual Available Nitrogen in the Stover and root and total
		pPltN->fStovNLabile	=fPoolAvailability*pPltN->fStovNPool;
		pPltN->fRootNLabile		=fPoolAvailability*pPltN->fRootNPool;
		pPltN->fTotalNLabile	=pPltN->fStovNLabile+pPltN->fRootNLabile;


		//////////////////////////////////////////////////////
		//Grain nitrogen demand regulation					//
		//////////////////////////////////////////////////////
		if(pPltN->fStovNPool < pPltN->fGrainDemand)
			pPltN->fGrainDemand = pPltN->fStovNPool;

		if(pPltN->fGrainDemand > pPltN->fStovNLabile)
		{
			pPltN->fStovCont -= pPltN->fStovNLabile;
			pPltN->fRootCont -= (pPltN->fGrainDemand - pPltN->fStovNLabile);

			pPltN->fRootActConc = pPltN->fRootCont/pBiom->fRootWeight;
		}
		else
		{
			pPltN->fStovCont -= pPltN->fGrainDemand;
			pPltN->fTopsActConc=pPltN->fStovCont/pBiom->fStovWeight;
			pPltN->fVegActConc =pPltN->fTopsActConc;
		}
		
	}

	//======================================================================================
	//	Grain Nitrogne Content
	//======================================================================================
	pPltN->fGrainCont += pPltN->fGrainDemand;
	pPltN->fGrainConc  = pPltN->fGrainCont/(pBiom->fGrainWeight+(float)1.0E-9);





	//============================================================================
	// 		Nitrogen Factors affecting Growth
	//============================================================================
	pPltN->fNStressFac=(float)1.0-(pPltN->fTopsOptConc-pPltN->fTopsActConc)
			               /(pPltN->fTopsOptConc-pPltN->fTopsMinConc);

	if (pPltN->fNStressFac>(float)1.0)
		pPltN->fNStressFac=(float)1.0;

	pPltN->fNStressPhoto	=(float)1.0;
	pPltN->fNStressLeaf		=(float)1.0;
	pPltN->fNStressGrain	=(float)1.0;

	if ((pPl->pDevelop->iStageCERES==3)||(pPl->pDevelop->iStageCERES==4))
		pPltN->fNStressFac = (float)(1.0-1.8*exp(-3.5*pPltN->fNStressFac));

	if (pPltN->fNStressFac<(float)0.001)
		pPltN->fNStressFac=(float)0.001;

	if (pPltN->fNStressFac<(float)0.8)
		pPltN->fNStressGrain = (float)0.2 + pPltN->fNStressFac;

	pPltN->fNStressPhoto = pPltN->fNStressFac*(float)1.2 + (float)0.2;
	if(pPltN->fNStressFac > (float)0.5)
		pPltN->fNStressPhoto = pPltN->fNStressFac*(float)0.4 + (float)0.6;

	pPltN->fNStressLeaf=pPltN->fNStressFac;

	if (pPltN->fNStressLeaf		>(float)1.0)		pPltN->fNStressLeaf		=(float)1.0;
	if (pPltN->fNStressPhoto	>(float)1.0)		pPltN->fNStressPhoto	=(float)1.0;
	if (pPltN->fNStressGrain	>(float)1.0)		pPltN->fNStressGrain	=(float)1.0;

}//end maize


//Sunflower:
	if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
	{
		
		if(pPltN->fTotalDemand > pPltN->fActNUptR)
		{
			//if N demand exceeds potential N uptake, then  reduce plant N demand to 
			//potential N uptake and calculate a supply/demand faftor
			factor = pPltN->fActNUptR/pPltN->fTotalDemand;

			pPltN->fRootDemand	= pPltN->fRootDemand*factor;
			pPltN->fStemDemand	= pPltN->fStemDemand*factor;
			pPltN->fLeafDemand	= pPltN->fLeafDemand*factor;
			fNHeadDemand		= fNHeadDemand*factor;
			fNPericarpDemand	= fNPericarpDemand*factor;
			fNEmbryoDemand		= fNEmbryoDemand*factor;
		}

		pPltN->fRootCont += pPltN->fRootDemand;
		pPltN->fStemCont += pPltN->fStemDemand;
		pPltN->fLeafCont += pPltN->fLeafDemand;
		fNGreenLeafCont  += pPltN->fLeafDemand;
		fNSenesLeafCont   = pPltN->fLeafCont - fNGreenLeafCont;
		fNHeadCont       += fNHeadDemand;
		pPltN->fStovCont = pPltN->fLeafCont + pPltN->fStemCont + fNHeadCont;

		// Actual N concentrations in plant parts after N uptake
		pPltN->fRootActConc=pPltN->fRootCont/((float)0.99*pBiom->fRootWeight);
		pPltN->fStemActConc=pPltN->fStemCont/pBiom->fStemWeight;
		pPltN->fLeafActConc=fNGreenLeafCont/pBiom->fGreenLeafWeight;
		
		if(fHeadWeight > (float)0.0)
			fNHeadActConc=fNHeadCont/fHeadWeight;

		pPltN->fTopsActConc=(pPltN->fStemCont+fNGreenLeafCont+fNHeadCont)/
			(pBiom->fStovWeight-pBiom->fSenesLeafWeight);

		



		fNFAC=(float)1.0-(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsActConc)
	               /(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsMinConc);

		pPltN->fNStressFac=(float)1.0-(pPl->pPltNitrogen->fLeafOptConc-pPl->pPltNitrogen->fLeafActConc)
	               /(pPl->pPltNitrogen->fLeafOptConc-pPl->pPltNitrogen->fLeafMinConc);

		if(fNFAC>(float)1.0)
			fNFAC = (float)1.0;

		if(fNFAC<(float)0.001)
			fNFAC = (float)0.001;

		if(pPltN->fNStressFac >= (float)0.5)
			pPltN->fNStressPhoto = pPltN->fNStressFac * (float)0.4 + (float)0.6;
		else if(pPltN->fNStressFac>(float)0.0)
			pPltN->fNStressPhoto = pPltN->fNStressFac * (float)1.2 + (float)0.2;
		else
			pPltN->fNStressPhoto = (float)0.2;

		//nitrogen deficit factor affecting expansion
		pPltN->fNStressLeaf = fNFAC;
		
		//nitrogen deficit factor affecting grain filling
		if(fNFAC<=(float)0.8)
			pPltN->fNStressGrain = (float)0.2 + fNFAC;
		else
			pPltN->fNStressGrain = (float)0.0;
	} //end sunflower

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//		Millet Nitrogen Factor Function
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int NitrogenFactors_MI(EXP_POINTER)
	{
	float fNFAC;

	pPl->pPltNitrogen->fTopsOptConc=(float)(8.276-1.485*(double)pPl->pDevelop->fStageXCERES
											 +0.0711*(double)(pPl->pDevelop->fStageXCERES
											                 *pPl->pDevelop->fStageXCERES));

    if (pPl->pPltNitrogen->fTopsOptConc>(float)6.0)
    	pPl->pPltNitrogen->fTopsOptConc=(float)6.0;

	if (pPl->pDevelop->fStageXCERES<(float)5.0)
		pPl->pPltNitrogen->fTopsMinConc=(float)(2.0-0.22*(double)pPl->pDevelop->fStageXCERES);
	else
		pPl->pPltNitrogen->fTopsMinConc=(float)(1.2-0.05*(double)pPl->pDevelop->fStageXCERES);

	if (pPl->pDevelop->fStageXCERES>=(float)8.0)
		pPl->pPltNitrogen->fTopsMinConc=pPl->pPltNitrogen->fTopsOptConc*(float)0.9;

	pPl->pPltNitrogen->fTopsMinConc		=pPl->pPltNitrogen->fTopsMinConc   *(float)0.01;
	pPl->pPltNitrogen->fTopsOptConc	=pPl->pPltNitrogen->fTopsOptConc*(float)0.01;


	pPl->pPltNitrogen->fRootOptConc=(float)((2.5-0.125*((double)pPl->pDevelop->fStageXCERES))/100.0);

    if (pPl->pDevelop->fStageXCERES<(float)0.2)
    	{
      	pPl->pPltNitrogen->fTopsActConc=pPl->pPltNitrogen->fTopsOptConc;
      	pPl->pPltNitrogen->fRootActConc=pPl->pPltNitrogen->fRootOptConc;
      	}

	//pPl->pPltNitrogen->fTopsMinConc=pPl->pPltNitrogen->fTopsMinConc*0.01

	if (pPl->pDevelop->iStageCERES>=5)
		{
		pPl->pPltNitrogen->fTopsActConc =pPl->pPltNitrogen->fVegActConc;
		pPl->pPltNitrogen->fVegMinConc=pPl->pPltNitrogen->fTopsMinConc;
        }

	fNFAC=(float)1.0-(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsActConc)
	               /(pPl->pPltNitrogen->fTopsOptConc-pPl->pPltNitrogen->fTopsMinConc);


	if (fNFAC>(float)1.0) fNFAC=(float)1.0;
	if ((pPl->pDevelop->iStageCERES==3)||(pPl->pDevelop->iStageCERES==4))
		fNFAC=(float)(1.0-2.0*exp(-3.5*(double)fNFAC));
	if (fNFAC>(float)0.95) fNFAC=(float)1.0;
	if (fNFAC<(float)0.0)  fNFAC=(float)0.0;

	pPl->pPltNitrogen->fNStressPhoto=(float)1.0;
	pPl->pPltNitrogen->fNStressLeaf=(float)1.0;
	pPl->pPltNitrogen->fNStressTiller=(float)1.0;

	pPl->pPltNitrogen->fNStressLeaf=fNFAC;

	if (pPl->pPltNitrogen->fNStressLeaf>(float)1.0) pPl->pPltNitrogen->fNStressLeaf=(float)1.0;
	if (fNFAC<(float)0.3) 	     pPl->pPltNitrogen->fNStressTiller=(float)2.33*fNFAC+(float)0.3;
	if (fNFAC<(float)0.5) 	     pPl->pPltNitrogen->fNStressPhoto=(float)1.20*fNFAC-(float)0.2;

	/*c  If(Nfac.lt.0.7)ndef1=nfac*(nfac+0.2) */

	if (pPl->pPltNitrogen->fNStressPhoto>(float)1.0) pPl->pPltNitrogen->fNStressPhoto=(float)1.0;
	if (pPl->pPltNitrogen->fNStressLeaf>(float)1.0) pPl->pPltNitrogen->fNStressLeaf=(float)1.0;
	if (pPl->pPltNitrogen->fNStressTiller>(float)1.0) pPl->pPltNitrogen->fNStressTiller=(float)1.0;
	if (pPl->pPltNitrogen->fNStressPhoto<(float)0.4) pPl->pPltNitrogen->fNStressPhoto=(float)0.4;

//	pPl->pPltNitrogen->fCNSD1=pPl->pPltNitrogen->fCNSD1+(float)1.0-fNFAC;
//	pPl->pPltNitrogen->fCNSD2=pPl->pPltNitrogen->fCNSD2+(float)1.0-pPl->pPltNitrogen->fNStressLeaf;

	return 1;
	}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 	This function calculates the effects of nitrogen stress on the nitrogen concentrations
//	and growth of the various plant parts.
//	Hight temperature, low soil water and high N increase the grain nitrogen concentration
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int GrainNitrogConc_MA(EXP_POINTER)
	{
	float		fGrainIncNitrogConc, fPoolAvailability;
	double		dTempFactor, dWaterFactor;
    PPLTNITROGEN	pPltN = pPl->pPltNitrogen;
    
	//==========================================================================================
	//	Grain Nitrogen Concentration for a Day  (0.010-0.018)
	//==========================================================================================
	//Effect of mean temperature
	dTempFactor=0.69+0.0125*((double)(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin)/2.0);

	//Effect of Drought Stress:
  	dWaterFactor=1.125-0.125*((double)pPl->pPltWater->fStressFacLeaf);

  	//Grain Nitrogen Concentration on the Day:
    fGrainIncNitrogConc=(float)((0.004+0.013*(double)pPltN->fNStressFac)
    					*max(dWaterFactor,dTempFactor));

    //Total Nitrogen Demand of the Growing Grain:
    pPltN->fGrainDemand=pPl->pBiomass->fGrainGrowR*fGrainIncNitrogConc;

	pPltN->fRootOptConc=(float)((2.5-0.25*((double)pPl->pDevelop->fStageXCERES))/100.0);

	if (pPltN->fGrainDemand!=(float)0.0)
		{
		//======================================================================================
		//	Root and Stover Nitrogen Concentration Regulation
		//======================================================================================
		pPltN->fRootMinConc=((float)0.75)*pPltN->fRootOptConc;

		if (pPltN->fRootActConc<pPltN->fRootMinConc)
			pPltN->fRootActConc=pPltN->fRootMinConc;

		pPltN->fVegActConc=pPltN->fStovCont/pPl->pBiomass->fStovWeight;

		if (pPltN->fVegActConc<pPltN->fVegMinConc)
			pPltN->fVegActConc=pPltN->fVegMinConc;

		//======================================================================================
		//	Nitrogen Available in the Stover and Root
		//======================================================================================
		//Potential Nitrogen Available in the Stover
		pPltN->fStovNPool=pPl->pBiomass->fStovWeight*
									(pPltN->fVegActConc-pPltN->fVegMinConc);

		//Potential Nitrogen Available in the Root
		pPltN->fRootNPool=pPl->pBiomass->fRootWeight*
										(pPltN->fRootActConc-pPltN->fRootMinConc);

		if (pPltN->fStovNPool<(float)0.0)
			pPltN->fStovNPool=(float)0.0;

		if (pPltN->fRootNPool<(float)0.0)
			pPltN->fRootNPool=(float)0.0;

		//Availability Factor
		fPoolAvailability=(float)0.15+((float)0.25)*pPltN->fNStressFac;

		//Actual Available Nitrogen in the Stover
		pPltN->fStovNLabile=fPoolAvailability*pPltN->fStovNPool;

		//Actual Available Nitrogen in the Root
		pPltN->fRootNLabile=fPoolAvailability*pPltN->fRootNPool;

		//Total Available Nitrogen in Stover and Root
		pPltN->fTotalNLabile=pPltN->fStovNLabile+pPltN->fRootNLabile;


		//======================================================================================
		//	Grain Nitrogen Demand Regulation
		//======================================================================================
		if (pPltN->fTotalNLabile<pPltN->fGrainDemand)
			{
			pPltN->fGrainDemand=pPltN->fTotalNLabile;
			//pPl->pBiomass->fGrainGrowR=pPl->pBiomass->fGrainGrowR*pPltN->fTotalNLabile/pPltNd->fGrainDemand;
			}

		//======================================================================================
		//	Root and Stover Nitrogen Concentration Regulation
		//======================================================================================
		if (pPltN->fGrainDemand>pPltN->fStovNLabile)
			{
			pPltN->fStovCont=pPltN->fStovCont-pPltN->fStovNLabile;
			pPltN->fRootCont -= (pPltN->fGrainDemand-pPltN->fStovNLabile);
			pPltN->fRootActConc=pPltN->fRootCont/pPl->pBiomass->fRootWeight;
			}
		else
			{
		   	pPltN->fStovCont -= pPltN->fGrainDemand;
		   	pPltN->fVegActConc=pPltN->fStovCont/pPl->pBiomass->fStovWeight;
			pPltN->fTopsActConc=pPltN->fVegActConc;
			}
		}

		//======================================================================================
		//	Grain Nitrogen Content
		//======================================================================================
		pPltN->fGrainCont += pPltN->fGrainDemand;


	return 1;
	}

	int WINAPI _loadds Initialize_MZ_SF(EXP_POINTER)
	{

		//Initialisierung der globalen Variablen, die nicht im EXP_POINTER stehen
		fPHINT=fXDevStage=fP5=fP9=fNFAC= (float)0.0;
		fStemWeightMin=fStemWeightMax= (float)0.0;
 		fHeadWeight=fHeadGrowR=fPotHeadWeight=fHeadWeightMin=fHeadWeightMax= (float)0.0;
 		fPericarpWeight=fPericarpGrowR=fPotPericarpGrowR= (float)0.0;
 		fEmbryoWeight=fEmbryoGrowR= (float)0.0;
 		fIGrainWeight=fOil=fOilPercent= (float)0.0;
 		fNHeadActConc=fNHeadMinConc=fNHeadOptConc=fNHeadDemand=fNHeadCont= (float)0.0;
 		fNPericarpActConc=fNPericarpOptConc=fNPericarpDemand=fNPericarpCont= (float)0.0;
 		fNEmbryoActConc=fNEmbryoOptConc=fNEmbryoDemand=fNEmbryoCont= (float)0.0;
 		fNGreenLeafActConc=fNGreenLeafCont=fNSenesLeafCont= (float)0.0;
 		fSumPS=fRI1=fRM=fRFR=fGrowFactor=ppp= (float)0.0;
 		fCumSenesLA=fSpecLeafArea=fAPLA= (float)0.0;
 		fSLOPEPE=fXPEPE=fYRAT= (float)0.0;
		fEarWeight=fEarGrowR=fEarFraction=(float)0.0;
		fTasselLeafNum=fMaxPlantLA=(float)0.0;
 		iDurP=iSenesTime=iJPEPE=iStageOld=iTLNO=iSlowFillDay=0;
 		bSenesCode = 0;

		return 1;
	}

/*******************************************************************************
** EOF */