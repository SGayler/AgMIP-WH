/*******************************************************************************
 *
 * Copyright  (c) 
 *
 * Author:  cs/ibs, mb/tum, ch/gsf, ts/tum, ep/gsf  95-99
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Initialisation routines before simulation starts.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 6 $
 *
 * $History: yinit.c $
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 18.01.02   Time: 14:49
 * Updated in $/Projekte/ExpertN/ModLib/control
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:02
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/control
 * Zur Stabilisierung des Codes Methoden Prototypen Definition in Header
 * Files verschoben statt über extern Declarationen festgelegt.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:26
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Access to function arrays through methods.
 * 
 *   15.11.95
 *
*******************************************************************************/

#include <windows.h>
#include "defines.h"
#include "confmem.h"
#include "xinclexp.h"
#include "xlanguag.h"     
#include "xh2o_def.h"
#include "xmemory.h"


#define SET_IF_99(x,y) 		if (x == (float)-99)   x = (float)y;

// =======================================================================================
/*------< MATH_FCT.C >------*/
extern float abspower(float x, float y);


// from util_fct.c
extern int   WINAPI Message(long, LPSTR);

//extern BOOL bMaturity,bPlantGrowth;

// from balance.c 
extern int WINAPI Set_Day_Zero(EXP_POINTER);

// from manag.c
extern BOOL bCorrectMeasure;

// from Uptake1.c
extern float afRSPar[];
extern float fh50Global;
extern float fp1Global;

/* from h2o_fct.c */
/* PedoTransferFunctions */
extern double PedoTransferCampbell(double,double,double,double,double,double, int);
extern double PedoTransferVereecken(double,double,double,double,double,double, int);
extern double PedoTransferRawlsBrakensiek(double,double,double,double,double,double, int);
extern double PedoTransferScheinost(double,double,double,double,double,double, int);
extern double PedoTransferScheinostBimodal(double,double,double,double,double,double, int);

// --------- export   procedures
int WINAPI initData(EXP_POINTER); 
int WINAPI init99Data(EXP_POINTER); 

int initTime(EXP_POINTER);
int initSoil(EXP_POINTER);
int initWater(EXP_POINTER);
int initClimate(EXP_POINTER);
int initPlant(EXP_POINTER);
int initEvapoTranpiration(EXP_POINTER);
int initChemistry(EXP_POINTER);
int initCumValue(EXP_POINTER);

int initSoilPotential(EXP_POINTER);
//int initHumus(EXP_POINTER);
int initSoilOrganicMatter(EXP_POINTER);

int initHydFunctions(int);


float GPV_FK_PWP(float,float,float,float,char*,int);
int	  SoilCode(float,float,float,char*);
int   BD_Class(float);
int   Clay_Class(float);
int   CapacityParam2(int,float,int);
int   CapacityParam1(int,char*,int);
					 
/* in manag.c */
extern int WINAPI TSComparePreCropDate(EXP_POINTER);

char  lpOut[100],lpOut2[100];


/*********************************************************************************************
 *                                                                                           *
 *  Name :  init99Data                                                                       *
 *  Func.:  Vor dem Einlesen des Modellparameter-Files werden hier alle Variablen, die       *
 *			standardmaessig initialisiert werden muessen mit -99 belegt. Nach dem Einlesen   *
 *			des Modellparameter-Files kann bei der folgenden Initialisierung entschieden     *
 *			werden, ob der Anwender eine Variable selbst initialisiernen moechte, oder ob    *
 *			das System dies fuer ihn erledigen soll.                                         *
 *  Autor:  ts/tum, ch/gsf ep/gsf                                                            *
 *  Date :  97-99                                                                            *
 *********************************************************************************************/
int  WINAPI init99Data(EXP_POINTER) 
{

	PWLAYER		pWL;
	PSWATER		pSW;
	PSLAYER		pSL;
	PCLAYER		pCL;
	PCPARAM		pPA;   
	
	int iLayer;
   
		pSo->pSHeat->afPartGeom[0][0] = (float)-99;
		pSo->pSHeat->afPartGeom[0][1] = (float)-99;
		pSo->pSHeat->afPartGeom[0][2] = (float)-99;
					
		pSo->pSHeat->afPartGeom[1][0] = (float)-99;
		pSo->pSHeat->afPartGeom[1][1] = (float)-99;
		pSo->pSHeat->afPartGeom[1][2] = (float)-99;
					
		pSo->pSHeat->afPartGeom[2][0] = (float)-99;
		pSo->pSHeat->afPartGeom[2][1] = (float)-99;
		pSo->pSHeat->afPartGeom[2][2] = (float)-99;
					
		pSo->pSHeat->afPartDens[0] = (float)-99;
		pSo->pSHeat->afPartDens[1] = (float)-99;
		pSo->pSHeat->afPartDens[2] = (float)-99;              
				
		pSo->pSHeat->afPartHeatCond[0]  = (float)-99;
		pSo->pSHeat->afPartHeatCond[1]  = (float)-99;
		pSo->pSHeat->afPartHeatCond[2]  = (float)-99;
		pSo->pSHeat->afPartHeatCond[3]  = (float)-99;
		pSo->pSHeat->afPartHeatCond[4]  = (float)-99;
		pSo->pSHeat->afPartHeatCond[5]  = (float)-99;

     for(pSL = pSo->pSLayer;
         (pSL != NULL);
         pSL = pSL->pNext)
     {
		pSL->afPartVolFrac[0] = (float)-99;
		pSL->afPartVolFrac[1] = (float)-99;
		pSL->afPartVolFrac[2] = (float)-99;
		pSL->afPartVolFrac[3] = (float)-99;

		pSL->fImpedLiqA  = (float)-99;  
		pSL->fImpedLiqB  = (float)-99;  
		pSL->fImpedGasA  = (float)-99;  
		pSL->fImpedGasB  = (float)-99;  
		
	 }	


		for(pWL = pWa->pWLayer;(pWL != NULL);pWL = pWL->pNext)
	    {
	     if (pWL->fMatPotAct >= (float)0.0) pWL->fMatPotAct  = (float)-99;
	     pWL->fMatPotOld  = pWL->fMatPotAct;
	     pWL->fSoilAirVol = (float)-99;
	    }

		for (pSW = pSo->pSWater;(pSW!= NULL);pSW= pSW->pNext)
		{ 
//		pSW->fCampA       = (float)-99;aus der Datenbank gelesen
//      pSW->fContInflec  = (float)-99;aus der Datenbank gelesen
//		pSW->fCampB       = (float)-99;aus der Datenbank gelesen
//      pSW->fVanGenA     = (float)-99;aus der Datenbank gelesen
//      pSW->fVanGenN     = (float)-99;aus der Datenbank gelesen
        pSW->fVanGenM     = (float)-99;
//      pSW->fContSat     = (float)-99;aus der Datenbank gelesen
        if (pSW->fContRes  < (float)0) pSW->fContRes     = (float)-99;
//      pSW->fPotInflec   = (float)-99;aus der Datenbank gelesen
	    pSW->fContSatCER  = (float)-99;
		}

       pWa->pWBalance->fSnowStorage      = (float)-99; 
	   pWa->pWBalance->fSnowWaterContent = (float)-99;

	   pWa->iTopBC                       = (int)-99;
	   pWa->iBotBC                       = (int)-99;

	   if (pPl != NULL)
	   {
			pPl->pDevelop->fDevStage	= (float)-99;
			pPl->pDevelop->iStageCERES	= (int)-99;
			pPl->pDevelop->fStageXCERES	= (float)-99;
			pPl->pDevelop->fStageWang	= (float)-99;
			pPl->pDevelop->fStageSUCROS	= (float)-99;
		
			pPl->pPltWater->fPanFac        = (float)-99;
			pPl->pPltWater->fPotTranspDay  = (float)-99;
			pPl->pPltWater->fPotTranspdt   = (float)-99;
			pPl->pPltWater->fActTranspDay  = (float)-99;
		}	
		pWa->pEvap->fCumStage1  = (float)-99;
		pWa->pEvap->fCumStage2  = (float)-99;
		pWa->pEvap->fTimeStage2 = (float)-99;
		pWa->pEvap->fActDay  	= (float)-99;
		pWa->pEvap->fActR  		= (float)-99;
		pWa->pEvap->fMax  		= (float)-99;
		pWa->pEvap->fMaxR		= (float)-99;

		for (SOIL_LAYERS1(pCL, pCh->pCLayer->pNext))
		{
		pCL->fHumusMinerMaxR 		= (float)-99;
		pCL->fNH4NitrMaxR			= (float)-99;
		pCL->fNO3DenitMaxR			= (float)-99;
		pCL->fHumusDecR 			= (float)-99;
		pCL->afFOMDecR[0] 			= (float)-99;
		pCL->afFOMDecR[1] 			= (float)-99;
		pCL->afFOMDecR[2] 			= (float)-99;
		pCL->fNFOMVeryFast	  		= (float)-99;
		pCL->fCFOMVeryFast	  		= (float)-99;
		pCL->fNFOMFast	  			= (float)-99;
		pCL->fCFOMFast	  			= (float)-99;
		pCL->fNFOMSlow	  			= (float)-99;
		pCL->fCFOMSlow	  			= (float)-99;
		pCL->fNMicBiomFast  		= (float)-99;
		pCL->fCMicBiomFast  		= (float)-99;
		pCL->fNMicBiomSlow  		= (float)-99;
		pCL->fCMicBiomSlow  		= (float)-99;
		pCL->fNMicBiomDenit 		= (float)-99;
		pCL->fCMicBiomDenit 		= (float)-99;
		pCL->fNHumusFast 	  		= (float)-99;
		pCL->fCHumusFast 	  		= (float)-99;
		pCL->fNHumusSlow 	  		= (float)-99;
		pCL->fCHumusSlow 	  		= (float)-99;
		pCL->fCHumusStable  		= (float)-99;
		pCL->fNHumusStable  		= (float)-99;
		pCL->fLitterMinerMaxR		= (float)-99;
		pCL->fManureMinerMaxR		= (float)-99;
		pCL->fMicBiomCN				= (float)-99;
		pCL->fFOMVeryFastDecMaxR	= (float)-99;
		pCL->fFOMFastDecMaxR		= (float)-99;
		pCL->fFOMSlowDecMaxR		= (float)-99;
		pCL->fMicBiomFastDecMaxR	= (float)-99;
		pCL->fMicBiomSlowDecMaxR	= (float)-99;
		pCL->fHumusFastMaxDecMaxR	= (float)-99;
		pCL->fHumusSlowMaxDecMaxR	= (float)-99;
		pCL->fUreaHydroMaxR			= (float)-99;
		pCL->fNO3DenitMaxR 			= (float)-99;

		pCL->fHumusToDONMaxR        = (float)-99;
		pCL->fLitterToDONMaxR       = (float)-99;
		pCL->fManureToDONMaxR       = (float)-99;
		pCL->fDONToHumusMaxR        = (float)-99;
		pCL->fDONToLitterMaxR       = (float)-99;
		pCL->fDONMinerMaxR          = (float)-99;

		pCL->fCLitter  = (float)-99;	
		pCL->fNLitter  = (float)-99;
		pCL->fLitterCN = (float)-99;
		}

	    for (SOIL_LAYERS0(pPA,pCh->pCParam))
		{
        pPA->fMolDiffCoef    	= (float)-99;
        pPA->fBioMassFastCN	 	= (float)-99;
        pPA->fBioMassSlowCN	 	= (float)-99;
        pPA->fBioMassDenitCN 	= (float)-99;
        pPA->fDispersivity	 	= (float)-99;
     	pPA->fNitrifPot 		= (float)-99;
     	pPA->fMinerThetaMin		= (float)-99;
     	pPA->fMinerEffFac		= (float)-99;
     	pPA->fMinerHumFac		= (float)-99;
     	pPA->fMinerTempB		= (float)-99;
     	pPA->fMinerQ10			= (float)-99;
     	pPA->fMinerSatActiv		= (float)-99;
     	pPA->fMaintCoeff		= (float)-99;
     	pPA->fYieldCoeff		= (float)-99;
     	pPA->fBioFastMaxGrowR	= (float)-99;
     	pPA->fN2ORedMaxR    	= (float)-99;
     	pPA->fDenitThetaMin		= (float)-99;
     	pPA->fDenitKsNO3		= (float)-99;
     	pPA->fNitrifNO3NH4Ratio	= (float)-99;
     	pPA->fDenitMaxGrowR		= (float)-99;
     	pPA->fDenitKsCsol		= (float)-99;
     	pPA->fDenitMaintCoeff	= (float)-99;
     	pPA->fDenitYieldCoeff	= (float)-99;
     	pPA->afKd[0]			= (float)-99;
     	pPA->afKd[1]			= (float)-99;
     	pPA->afKd[2]			= (float)-99;
     	pPA->afKd[3]			= (float)-99;
     	pPA->afKd[4]			= (float)-99;
     	pPA->afKd[5]			= (float)-99;
     	pPA->afKh[0]			= (float)-99;
     	pPA->afKh[1]			= (float)-99;
     	pPA->afKh[2]			= (float)-99;
     	pPA->afKh[3]			= (float)-99;
     	pPA->afKh[4]			= (float)-99;
     	pPA->afKh[5]			= (float)-99;
        }
		
        pCh->pCProfile->fNH3VolatMaxR = (float)-99;
        pCh->pCProfile->fCH4ImisR     = (float)-99;
        pCh->pCProfile->fCH4ImisDay   = (float)-99;
		
        pCh->pCProfile->fCLitterSurf = (float)-99;
		pCh->pCProfile->fNLitterSurf = (float)-99;

		pCh->pCProfile->fCManureSurf = (float)-99;
		pCh->pCProfile->fNManureSurf = (float)-99;
		
		pCh->pCProfile->fCHumusSurf  = (float)-99;
		pCh->pCProfile->fNHumusSurf  = (float)-99;
        
		pCh->pCProfile->fCStandCropRes  = (float)-99;
		pCh->pCProfile->fNStandCropRes  = (float)-99;

		pSo->fSoilCover =(float)-99;

    return 1;
}

/*********************************************************************************************
 *                                                                                        
 *  Name :  initData                                                                 
 *  Func.:  Initializing ExpertN system variables 
 *  Autor:  cs/ibs                                                                  
 *  Date :  03.12.96                                                                 
 ********************************************************************************************/
int WINAPI initData(EXP_POINTER)
{int OK=0;

    OK += initTime(exp_p);
    OK += initSoil(exp_p);
    OK += initWater(exp_p);
    OK += initClimate(exp_p);
    //OK += initPlant(exp_p);
    OK += initEvapoTranpiration(exp_p);
    OK += initChemistry(exp_p);
    OK += initCumValue(exp_p);
    OK +=Set_Day_Zero(exp_p);
    return OK;
}

/*********************************************************************************************
 *  Name     	: initTime()                            
 *                                                   
 *  Function 	: Initialisierung  für zeitabhängige Variable
 *                       
 *  Author		: cs/ibs  03.12.96
 *  Aenderungen : ts/tum  12.03.97, ch/gsf 19.05.97
 **********************************************************************************************/
int initTime(EXP_POINTER)
{ 

   pTi->pSimTime->fTimeOld     = (float)0.0;
   pTi->pSimTime->fTimeAct  = (float)0.0;

   pTi->pTimeStep->fOld    = (float)0.0;
   pTi->pTimeStep->fAct    = (float)0.0;

   /* ch, 19.5.97 */
   pTi->pTimeStep->fOpt    = (float)0.01;

   pTi->pSimTime->fTimeDay     = (float)0.0;
   pTi->pSimTime->fTimeTenDay  = (float)0.0;

	/* einzige unbeschriebene Größe aus DB Einleseroutine  */  
   pTi->pSimTime->lTimeDate = pTi->pSimTime->lStartDate;

   return 1;
   
}   

/********************************************************************************************
*  Name     : initSoil ()                                                                   *
*  Funktion : Initialisierung erforderlicher Variablen aus Eingabedaten und/oder mit        *
*             Standardwerten. Die Abfrage if -99 stellt sicher, dass Daten, die aus dem     *
*             Modellparameter-File individuell eingelesen wurden nicht ueberschrieben werden*
*             werden.                                                                       *
*  Author   : cs/ibs, ch/gsf                                                                *
*  Date     : 3.12.96                                                                       *
*********************************************************************************************/
int initSoil(EXP_POINTER)
{ 

 
	PSLAYER		pSL;
                                                           
	                                                           
	/* Initialisierung der Geometriefaktoren zur Berechnung 
	    der Bodentemperaturen in den Simulationsschichten.   */
		  
		  SET_IF_99(pSo->pSHeat->afPartGeom[0][0],(float)0.125);
		  SET_IF_99(pSo->pSHeat->afPartGeom[0][1],(float)0.125);
		  SET_IF_99(pSo->pSHeat->afPartGeom[0][2],(float)0.5);

		  SET_IF_99(pSo->pSHeat->afPartGeom[1][0],(float)0.125);
		  SET_IF_99(pSo->pSHeat->afPartGeom[1][1],(float)0.125);
		  SET_IF_99(pSo->pSHeat->afPartGeom[1][2],(float)0.5);
			
		  SET_IF_99(pSo->pSHeat->afPartGeom[2][0],(float)0.75);
		  SET_IF_99(pSo->pSHeat->afPartGeom[2][1],(float)0.75);
		  SET_IF_99(pSo->pSHeat->afPartGeom[2][2],(float)0.0);

		  SET_IF_99(pSo->pSHeat->afPartDens[0],(float)2.65);
		  SET_IF_99(pSo->pSHeat->afPartDens[1],(float)2.65);
		  SET_IF_99(pSo->pSHeat->afPartDens[2],(float)1.1);

	/* Spezifische Waermekonduktivitaeten: [W / (K m)] nach: De Vries, 1963 */
		  SET_IF_99(pSo->pSHeat->afPartHeatCond[0],8.8);     /* Quarz */
		  SET_IF_99(pSo->pSHeat->afPartHeatCond[1],2.9);     /* Mineral */
		  SET_IF_99(pSo->pSHeat->afPartHeatCond[2],0.25);    /* org. Substanz */
		  SET_IF_99(pSo->pSHeat->afPartHeatCond[3],0.57);    /* Wasser */
		  SET_IF_99(pSo->pSHeat->afPartHeatCond[4],0.025);   /* Luft */
		  SET_IF_99(pSo->pSHeat->afPartHeatCond[5],2.2);     /* Eis  */

	/* Hier erfolgt die Bestimmung der Volumenanteile fuer die vier 
	   Fraktionen Sand, Schluff, Ton und org. Substanz.  */
     for(pSL = pSo->pSLayer;
         (pSL != NULL);
         pSL = pSL->pNext)
     {
		SET_IF_99(pSL->afPartVolFrac[0],(((float)100.0 - pSL->fClay - pSL->fHumus)/((float)100.0 * pSo->pSHeat->afPartDens[0])));
		SET_IF_99(pSL->afPartVolFrac[1],(((float)pSL->fClay * pSL->fBulkDens)/((float)100.0 * pSo->pSHeat->afPartDens[1])));
		SET_IF_99(pSL->afPartVolFrac[2],(pSL->fHumus/((float)100.0 * pSo->pSHeat->afPartDens[2])));
		SET_IF_99(pSL->afPartVolFrac[3],(float)0.0);
		SET_IF_99(pSL->fImpedLiqA,(float)0.005);
		SET_IF_99(pSL->fImpedLiqB,(float)10.0);
		SET_IF_99(pSL->fImpedGasA,(float)1.0);  // Provisorischer Wert 
		SET_IF_99(pSL->fImpedGasB,(float)1.0);  // Provisorischer Wert 

     }  // for  calculating particle volume fraction

     
	/* Die Gesamttiefe des Profiles wird durch aufaddieren
	   der einzelnen Schichtdicken berechnet.*/     
     
     for(pSL = pSo->pSLayer->pNext,pSo->fDepth = (float)0.0;
         (pSL->pNext != NULL);
         pSL = pSL->pNext)
     {    
	     pSo->fDepth += pSL->fThickness;
     }
	                                                
	    if (pSo->iLayers < 3)
	    {   
	    pSo->iLayers = 3;
	    }
	                                                
		pSo->fDeltaZ = pSo->fDepth / (float)(pSo->iLayers - 2);
     
     
	/* Der Bodenalbedo der ersten Bodenschicht muss initialisiert
	   werden. Dies geschieht in abhaengigkeit von der Bodenart und
	   dem Humusgehalt der ersten Bodenschicht.*/


		if ((pSo->pSLayer->pNext->fHumus) <= (float)2.0)
		{
			if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"L\0"))
			{   
				pSo->fSoilAlbedo = (float)0.13;
			}
			else
			{   
				if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"S\0"))
				{   
					pSo->fSoilAlbedo = (float)0.13;
				}
				else
				{   
					if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"l\0"))
					{   
						pSo->fSoilAlbedo = (float)0.13;
					}
					else
					{   
						if (!lstrcmp(pSo->pSLayer->pNext->acSoilID,"l\0"))
						{   
							pSo->fSoilAlbedo = (float)0.13;
						}
						else
						{   
							pSo->fSoilAlbedo = (float)0.19;
						}	
					}	
				}		
			}
		}
		else
		{
			if ((pSo->pSLayer->pNext->fHumus) <= (float)5.0)		
				pSo->fSoilAlbedo = (float)0.13;		
			if (((pSo->pSLayer->pNext->fHumus) > (float)5.0)&&
				((pSo->pSLayer->pNext->fHumus) <= (float)10.0))
				pSo->fSoilAlbedo = (float)0.11;		
			if ((pSo->pSLayer->pNext->fHumus) > (float)10.0)		
				pSo->fSoilAlbedo = (float)0.08;		
		}	

   SET_IF_99(pSo->fSoilCover,(float)0); 

   return 1;
}  /*   Ende Init Bodendaten      */


/**********************************************************************************
 *  Name     : initEvaTra()                            
 *                                                    
 *  Autor    : cs/ibs, ch/gsf                                                                
 *  Date     : 3.12.96                                                                        
 *---------------------------------------------------------------------------------
 *  Changed   Date    Author         Topic
 *
 *
 **********************************************************************************/
int initEvapoTranpiration(EXP_POINTER)
{  

      if (pPl != NULL)
	   {
	
		SET_IF_99(pPl->pPltWater->fPanFac,(float)0.0);
		SET_IF_99(pPl->pPltWater->fPotTranspDay,(float)0.0);
		SET_IF_99(pPl->pPltWater->fPotTranspdt,(float)0.0);
		SET_IF_99(pPl->pPltWater->fActTranspDay,(float)0.0);
	   }
		SET_IF_99(pWa->pEvap->fActDay,(float)0.0);
		SET_IF_99(pWa->pEvap->fActR,(float)0.0);
		SET_IF_99(pWa->pEvap->fMax,(float)8.0);
		SET_IF_99(pWa->pEvap->fMaxR,(float)0.0);

   return 1;
}  /*  end init evapo - transpiration data      */


/*********************************************************************************
 *  Name    : initPlant ()                            
 *  Author  : cs/ibs, ch/gsf                                
 *  Date    : 3.12.96                                   
 *--------------------------------------------------------------------------------
 *  Changed   Date    Author         Topic
 *
 *
 *********************************************************************************/
int initPlant(EXP_POINTER)
{ 

int	numberPlant = 0;

PPLANT	pPL;

pPL = GetPlantPoi();

/* Schalter fuer Wachstumsmodell initialisieren  */
//bMaturity	 		= FALSE;                           
//bPlantGrowth 		= FALSE;

if (pPL != NULL)
{
 pPl->pDevelop->bMaturity	 		= FALSE;                           
 pPl->pDevelop->bPlantGrowth 		= FALSE;

 numberPlant = (int)1;

  if (pPL->pNext != NULL)
  {
	  numberPlant = (int)2;
	if (pPL->pNext->pNext != NULL)
	{
		numberPlant = (int)3;
	  if (pPL->pNext->pNext->pNext != NULL)
		  numberPlant = (int)4;
	}
  }
 }

if (numberPlant == (int)0)
{
	strcpy(pLo->pFieldplot->acCatchCrop,"keine");
	strcpy(pLo->pFieldplot->acMainCrop,"keine");
}
if (numberPlant == (int)1)
{
	strcpy(pLo->pFieldplot->acCatchCrop,"keine");
	strcpy(pLo->pFieldplot->acMainCrop,pPL->pGenotype->acCropName);
}
if (numberPlant == (int)2)
{
	strcpy(pLo->pFieldplot->acCatchCrop,pPL->pGenotype->acCropName);
	strcpy(pLo->pFieldplot->acMainCrop,pPL->pGenotype->pNext->acCropName);
}
if (numberPlant == (int)3)
{
	strcpy(pLo->pFieldplot->acCatchCrop,pPL->pGenotype->pNext->acCropName);
	strcpy(pLo->pFieldplot->acMainCrop,pPL->pGenotype->pNext->pNext->acCropName);
}
if (numberPlant == (int)4)
{
	strcpy(pLo->pFieldplot->acCatchCrop,pPL->pGenotype->pNext->pNext->acCropName);
	strcpy(pLo->pFieldplot->acMainCrop,pPL->pGenotype->pNext->pNext->pNext->acCropName);
}
	
   return 1;
}  /*   end InitPlant()       */


/**************************************************************************************
 *  Name     : initWater()                             
 *  Author   : cs/ibs, ch/gsf
 *  Date     : 3.12.96                                   
 *-------------------------------------------------------------------------------------
 *  Changed   Date    Author         Topic
 *			  02.97	  ep/gsf		 fPorosity
 *			  03.99	  ep/gsf		 GPV_FK_PWP
 **************************************************************************************/
int initWater(EXP_POINTER)
{
   int    iLayer = 0;
   int    iSoilNr;
   float  f0,f1,f2,f3;
   double dClayF, dSiltF, dSandF, dRockF, dBD, dSP, dCarbonF;
 
   PWLAYER	pWL;
   PSLAYER	pSL;
   PSWATER	pSW;
   

      if (pCl->pWeather == NULL)
      {
      Message(3,ERROR_WEATHER_DATA); 
      return -1;
      }                           
      
      /* set the pool of water that may infiltrate, runoff or stay on the surface. */
	  pWa->pWBalance->fReservoir = pCl->pWeather->fRainAmount;
      SET_IF_99(pWa->pWBalance->fSnowStorage,(float)0); 
	  SET_IF_99(pWa->pWBalance->fSnowWaterContent,(float)0);
                                  
	    pWa->fPondWater     = (float)0.0;                 
	    pWa->fPotInfiltration	= (float)0.0;
                                        
	    pWa->fRunOffR	    = (float)0.0; 
	    pWa->fPercolR	    = (float)0.0;

      if (pWa->iBotBC == (int) -99) pWa->iBotBC = (int)1;// standard: "free drainage" = (int)1;

	/* Initialisieren der hydraulischen Funktionen */
     initHydFunctions(GetSelectedFunction(ID_HYDRAULIC));   // 0 = Hutson & Cass
     									// 1 = VanGenuchten
     									// 2 = VanGenuchten Bimodal type

 
  for (iLayer = 0,
       pSW=pSo->pSWater,
  	   pSL=pSo->pSLayer,
  	   pWL= pWa->pWLayer;
       ((pSW!= NULL)&&
  	    (pSL!= NULL)&&
  	    (pWL!= NULL));
     iLayer++,
 	   pSL = pSL->pNext,
	   pWL = pWL->pNext,
	   pSW = pSW->pNext)
   {
        // ep 230109 
	    if (pSL->fPorosity == (float)-99) f0=(float)-1;
        // ep 150397
        SET_IF_99(pSL->fPorosity,((float)1 - pSL->fBulkDens/(float)2.65));
		SET_IF_99(pWL->fSoilAirVol,(pSL->fPorosity - pWL->fContAct));
		SET_IF_99(pSW->fMinPot,(float)-2);

		// ep 150704
		if ((pSL->fHumus == (float)-99)&&(pSL->fCHumus == (float)-99))	
		   {
			itoa(iLayer,lpOut2,3);
			strcpy(lpOut,(LPSTR)START_DATA_ERROR_TXT);
			strcat(lpOut,(LPSTR)" - In layer ");
			itoa(iLayer,lpOut2,10);
			strcat(lpOut,(LPSTR)lpOut2);
			strcat(lpOut,(LPSTR)" : no organic matter nor organic matter-C content given !");
            Message(2, (LPSTR)lpOut);
            Message(3, START_DATA_ERROR_TXT);
            pSL->fCHumus = (float)0.001;
		   }
		SET_IF_99(pSL->fHumus,pSL->fCHumus*(float)1.72); 
		SET_IF_99(pSL->fCHumus,pSL->fHumus/(float)1.72); 
		SET_IF_99(pSL->fNHumus,pSL->fCHumus/(float)10); 

		// ep 120500
		if ((!lstrcmp(pSL->acSoilID,"-99"))||(!lstrcmp(pSL->acSoilID,"")))
		  iSoilNr=SoilCode(pSL->fClay,pSL->fSilt,pSL->fSand,pSL->acSoilID);	

		// ep 180500 transform fRockfrac from weight% to volume% ?
		//f0=
        // ep 180299 
        f1=GPV_FK_PWP(pSL->fClay,pSL->fBulkDens,pSL->fCHumus,
			          pSL->fRockFrac,pSL->acSoilID,1);
        f2=GPV_FK_PWP(pSL->fClay,pSL->fBulkDens,pSL->fCHumus,
			          pSL->fRockFrac,pSL->acSoilID,2);
        f3=GPV_FK_PWP(pSL->fClay,pSL->fBulkDens,pSL->fCHumus,
			          pSL->fRockFrac,pSL->acSoilID,3);
		///*
        if ((pSW->fContFK==(float)-99)||(pSW->fContPWP==(float)-99))
		{
		 if (f0 < (float)0) pSL->fPorosity=f1/(float)100;
         pSW->fContSat = pSL->fPorosity;
		}
        //*/
		if (pSL->fPorosity <= f2/(float)100.0) f2 = pSL->fPorosity * (float)85.0;  
		SET_IF_99(pSW->fContFK,  f2/(float)100.0);
        SET_IF_99(pSW->fContPWP, f3/(float)100.0);
        

		dClayF= (double)pSL->fClay;
        dSiltF= (double)pSL->fSilt;
        dSandF= (double)pSL->fSand;
        dRockF= (double)pSL->fRockFrac;
        dRockF*=(double)2.65;//[vol.%] to [weight%]
        dRockF*=(double)100;//[1] to [%]
        dBD= (double)pSL->fBulkDens;
        dSP= (double)pSL->fPorosity;
/*        dCarbonF= (double)pSL->fCHumus /((double)pSo->fDeltaZ * dBD *(double)100);   */
   		dCarbonF= (double)pSL->fCHumus;

		/* Da derzeit keine PTF2 aufgerufen wird, wird fContRes nach
		   Initialisierungsliste mit null belegt. */
/*
		SET_IF_99(pSW->fContSat, PTF(1));
		SET_IF_99(pSW->fContRes, PTF(2));
		SET_IF_99(pSW->fCampA,   PTF(3));
		SET_IF_99(pSW->fCampB,   PTF(4));
		SET_IF_99(pSW->fVanGenA, PTF(5));
		SET_IF_99(pSW->fVanGenN, PTF(6));
		SET_IF_99(pSW->fVanGenM, PTF(7));
		SET_IF_99(pSW->fCondSat, PTF(8));
*/
//        pSL->fPorosity = pSW->fContSat;


     
		// saturated volumetric water content [1]
	    if (pSW->fContSat == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
			    break;                  
			case 1: 
		        f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
			    break;                  
			case 2: 
		        f1= (float)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
			    break;                  
			case 3: 
		        f1= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,dBD,dCarbonF,dSP,(int)1);
			    break;                  
			default: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)1);
			    break;                  
			}    
		pSW->fContSat = f1;
		pSW->fContSat = pSL->fPorosity;//in water1.c : pSW->fContSat = pSL->fPorosity - pWL->fIce !!
									   // this leads to a water balance error at simulation start
		}

		// residual volumetric water content [1]
		if (pSW->fContRes == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)2);
			    break;                  
			case 1: 
		        f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)2);
			    break;                  
			case 2: 
		        f1= (float)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)2);
			    break;                  
			case 3: 
		        f1= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,dBD,dCarbonF,dSP,(int)2);
			    break;                  
			default: 
		        f1= (float)PedoTransferCampbell(dClayF, dSiltF, dSandF, dBD, dCarbonF, (double)1,(int)2);
			    break;                  
			}    
		pSW->fContRes = f1;
		}

		// Campbell A parameter or "air entry value" [?]
		if (pSW->fCampA == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
			    break;                  
			case 1: 
		        f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
			    break;                  
			case 2: 
		        f1= (float)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
			    break;                  
			case 3: 
		        f1= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,dBD,dCarbonF,dSP,(int)3);
			    break;                  
			default: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)3);
			    break;                  
			}    
		pSW->fCampA = f1;
		}
		
		//Campbell B parameter [1]
		if (pSW->fCampB == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
			    break;                  
			case 1: 
		        f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
			    break;                  
			case 2: 
		        f1= (float)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
			    break;                  
			case 3: 
		        f1= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,dBD,dCarbonF,dSP,(int)4);
			    break;                  
			default: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)4);
			    break;                  
			}    
		pSW->fCampB = f1;
		}
		
        if ((pSW->fVanGenA != (float)-99) 
			&& (pSW->fVanGenN != (float)-99) && (pSW->fVanGenN != (float)0)
			&&(pSW->fVanGenM == (float)-99)) 
		{
			pSW->fVanGenM = (float)1 - (float)1/pSW->fVanGenN;
		}
		
		// van Genuchten alpha parameter [1/cm]
		if (pSW->fVanGenA == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
			    break;                  
			case 1: 
		        f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
			    break;                  
			case 2: 
		        f1= (float)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
			    break;                  
			case 3: 
		        f1= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,dBD,dCarbonF,dSP,(int)5);
			    break;                  
			default: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)5);
			    break;                  
			}    
		pSW->fVanGenA = f1*(float)0.1;// [1/cm] to [1/mm], da van Genuchten alpha in [1/cm]!
		}
		
		// van Genuchten n parameter [1]
		if (pSW->fVanGenN == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
			    break;                  
			case 1: 
		        f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
			    break;                  
			case 2: 
		        f1= (float)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
			    break;                  
			case 3: 
		        f1= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,dBD,dCarbonF,dSP,(int)6);
			    break;                  
			default: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)6);
			    break;                  
			}    
		pSW->fVanGenN = f1;
		}

		// van Genuchten m parameter [1]
		if (pSW->fVanGenM == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
			    break;                  
			case 1: 
		        f1= (float)1.0;//PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
			    break;                  
			case 2: 
		        f1= (float)PedoTransferRawlsBrakensiek(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
			    break;                  
			case 3: 
		        f1= (float)1.0;//PedoTransferScheinost(dClayF,dSiltF,dSandF,dBD,dCarbonF,dSP,(int)7);
			    break;                  
			default: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)7);
			    break;                  
			}    
		pSW->fVanGenM = f1;
		}
		
		// saturated hydraulic conductivity [mm/day]
		if (pSW->fCondSat == (float)-99)  
		{
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
			case 0: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
			    break;                  
			case 1: 
		        f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                f2= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)9);
                f3= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)10);
				pSW->fVanGenA2 = f2;//Gardner A
                pSW->fVanGenN2 = f3;//Gardner B
			    break;                  
			case 2: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
			    break;
            case 3: /* Scheinost */
		        //f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                //f2= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,(double)(f1*(float)10),dCarbonF,(double)1,(int)9);
                f1= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
                f2= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)9);
                f3= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)10);
				pSW->fVanGenA2 = f2;//Gardner A
                pSW->fVanGenN2 = f3;//Gardner B
		    break;
			default: 
		        f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
			    break;                  
		    }    
		
        //only Campbell estimates sat. cond.
		//f1= (float)PedoTransferCampbell(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)8);
        pSW->fCondSat  = f1;//*(float)10;// [cm/d] to [mm/d]
		//pSW->fVanGenA2 = f2;//Gardner A
        //pSW->fVanGenN2 = f3;//Gardner B
		}
        else
        {
			switch(GetSelectedFunction(ID_PEDOTRANS))
			{
            case 1:
		        f2= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,pSW->fCondSat,dCarbonF,dSP,(int)9);
                f3= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)10);
 		        pSW->fVanGenA2 = f2;//Gardner A
                pSW->fVanGenN2 = f3;//Gardner B
                break;                  
            case 3:
		        f2= (float)PedoTransferScheinost(dClayF,dSiltF,dSandF,pSW->fCondSat,dCarbonF,dSP,(int)9);
                f3= (float)PedoTransferVereecken(dClayF,dSiltF,dSandF,dBD,dCarbonF,(double)1,(int)10);
		        pSW->fVanGenA2 = f2;//Gardner A
                pSW->fVanGenN2 = f3;//Gardner B
			    break;
            }
        }

        /*
        //Bimodal Test//
        pSW->fContSat=pSL->fPorosity=(float)PedoTransferScheinostBimodal(dClayF,dSiltF,dSandF,dBD,dCarbonF,dRockF,(int)1);
        pSW->fContRes=(float)PedoTransferScheinostBimodal(dClayF,dSiltF,dSandF,dBD,dCarbonF,dRockF,(int)2);
        pSW->fVanGenA=(float)1*(float) 0.1;// [1/cm] to [1/mm]
        pSW->fVanGenN=(float)5;
        pSW->fVanGenM=(float)1;
        f1=(float)PedoTransferScheinostBimodal(dClayF,dSiltF,dSandF,dBD,dCarbonF,dRockF,(int)5);
        pSW->fVanGenA2=f1*(float)0.1;// [1/cm] to [1/mm]
        pSW->fVanGenN2=(float)PedoTransferScheinostBimodal(dClayF,dSiltF,dSandF,dBD,dCarbonF,dRockF,(int)6);
        pSW->fVanGenN2+=(float)1;
        pSW->fVanGenM2=(float)1;
        f2=(float)PedoTransferScheinostBimodal(dClayF,dSiltF,dSandF,dBD,dCarbonF,dRockF,(int)4);
        pSW->fBiModWeight1=(float)1 - f2;
        pSW->fBiModWeight2=f2;
        pSW->fCondSat=(float)PedoTransferScheinostBimodal(dClayF,dSiltF,dSandF,dBD,dCarbonF,dRockF,(int)8);//*(float)10;        
        */


        
        //// watercontent at the inflection point  ===============
         // ch if (pSW->fContInflec == (float)-99)
         
         // This variable must be > 0 !
         if (pSW->fContInflec < EPSILON) 
        {
        pSW->fContInflec = (float)2.0 * pSW->fCampB * pSL->fPorosity;
        pSW->fContInflec /= ((float)1.0+(float)2.0*pSW->fCampB);
        }

		
         // This variable must be < 0 !
         if ((pSW->fPotInflec + EPSILON > 0) || (pSW->fPotInflec == (float)-99))
		{
        f1 = (float)2.0 * pSW->fCampB;
        f1 /= ((float)1.0+(float)2.0*pSW->fCampB);
        
        pSW->fPotInflec = pSW->fCampA * abspower(f1,((float)-1.0*(pSW->fCampB)));
		}                            

        if (pSW->fMinPot > pSW->fPotInflec)
        {

		////=========  max water content per layer ===
         pWL->fContMax = (float)1.0 - (pSW->fContInflec /pSL->fPorosity);
         pWL->fContMax *= abspower(pSW->fMinPot,(float)2.0);
         pWL->fContMax *= pSL->fPorosity;
         f1 =    (float)-2.0*pSW->fCampB;                 
         f2 = pSW->fContInflec/pSL->fPorosity;
         f3 = (float)(abspower(pSW->fCampA,(float)2.0));
         f3 *= (abspower(f2,f1));
         pWL->fContMax /= f3;
         pWL->fContMax = pSL->fPorosity - pWL->fContMax;
        } // Ende: if Min. potential  
      	else
        {
         f1 = ((float)-1.0 /pSW->fCampB);
         f2 = (pSW->fMinPot/pSW->fCampA);
         pWL->fContMax	= (abspower(f2,f1)* pSL->fPorosity);
        } //// Ende: else Min. potential  

   }  // Ende for: schichtweise Berechnung

	/* Fuer das Kapazitaetsmodell nach Ritchie wird die Modellvariable
	   fContSatCER benoetigt. Sie berechnet sich aus der Feldkapazitaet
	   und dem Gesamtporenvolumen */

	  for(pSW = pSo->pSWater, 
	      pSL = pSo->pSLayer; ((pSW != NULL)&&
	      					   (pSL != NULL));
	      pSW = pSW->pNext,
	      pSL = pSL->pNext)
	  {
		//SET_IF_99(pSW->fContSatCER,(pSW->fContFK + ((pSL->fPorosity - pSW->fContFK)  * (float)0.5)));
		SET_IF_99(pSW->fContSatCER,pSW->fContFK);//ep 20090331 fuer HydroConsult
	  }  // Ende schichtweise Berechnung von fContSatCER


     /* Der aus den Startwerten eingelesene Wassergehalt zu 
		Beginn der Simulation wird auf die aktuellen Wassergehalte
		initialisiert. Die Fluesse aller Simulationsschichten werden
		mit Null initialisiert.  */ 

	   for (H2O_SOIL_LAYERS)
	   {
	     pWL->fFluxDens = (float)0.0;
	     pWL->fFlux   	= (float)0.0;        
	     
	     if (pWL->fContInit < (float)0.0)
	     	{              
	     	pWL->fContInit = WATER_CONTENT(pWL->fMatPotAct);
	     	}
	     
		 if (pWL->fContInit > pSW->fContSat)
		   {
            pWL->fContInit = WATER_CONTENT(pWL->fMatPotAct);
    
			itoa(iLayer,lpOut2,3);
			strcpy(lpOut,(LPSTR)START_DATA_ERROR_TXT);
			strcat(lpOut,(LPSTR)" - In layer ");
			itoa(iLayer,lpOut2,10);
			strcat(lpOut,(LPSTR)lpOut2);
			strcat(lpOut,(LPSTR)" : initial water content > porosity or saturated w. c. !");
            Message(2, (LPSTR)lpOut);
		   }
	     
	     pWL->fContAct  = pWL->fContInit;
	     pWL->fContOld  = pWL->fContInit;
	   }

		/* Die virtuellen Schichten werden mit den 
			physikalischen Randschichten initialisiert.*/
		pWa->pWLayer->fContAct   = pWa->pWLayer->pNext->fContAct;
		pWa->pWLayer->fContOld   = pWa->pWLayer->pNext->fContOld;
		pWa->pWLayer->fFluxDens  = (float)0.0;
		pWa->pWLayer->fFlux      = (float)0.0;
	
		pWL->fContAct   = pWL->pBack->fContAct;
		pWL->fContOld   = pWL->pBack->fContOld;
		pWL->fFluxDens  = (float)0.0;
		pWL->fFlux      = (float)0.0;
	
	/* Initialisieren der Potentialgroessen des Bodens fuer die 
	   Loeser der Richardsgleichung.  */
     initSoilPotential(exp_p);  // newly calculation of soil water potential

	/* Initialisieren des permanenten Welkepunkts des Bodens */
	 for (H2O_SOIL_LAYERS)
	   {
        SET_IF_99(pSW->fContFK, WATER_CONTENT((float)-3300.0));//     -330 hPa -->   -3300 mm WS
        SET_IF_99(pSW->fContPWP,WATER_CONTENT((float)-155000.0));// -15000 hPa --> -150000 mm WS
	   }

//SG 20110413:
// Field capacity and Wilting point from hydraulic function, even if different values are provided in the database
// or table in *.xnm file is used
// = use values from database or *.xnm only when no hydraulic function is selected

	 //Muss aktiviert sein für Pareto-Analyse:

	 if(GetSelectedFunction(ID_HYDRAULIC) < (int)5)
	 {
	 for (H2O_SOIL_LAYERS)
	   {
        pSW->fContFK = WATER_CONTENT((float)-3300.0);//     -330 hPa -->   -3300 mm WS
        pSW->fContPWP = WATER_CONTENT((float)-150000.0);// -15000 hPa --> -150000 mm WS
	   }
	 }

  return 1;
}  /*   End initWater  */


/********************************************************************************
 *  Name     : initSoilPotential()                            
 *                                                        
 *                                                        
 *  Author  : cs/ibs                               
 *  Date    : 3.12.96                                   
 *-------------------------------------------------------------------------------
 *  Changed   Date    Author         Topic
 *
 *
 ********************************************************************************/
int initSoilPotential(EXP_POINTER)
 { 

   PWLAYER		pWL;
   PSLAYER		pSL;
   PSWATER		pSW;
	
   int iLayer;

   int iL,iNodeWaterTable;
   float fDiff;
   float DeltaZ = pSo->fDeltaZ;
   
   for(H2O_SOIL_LAYERS)
     {
      pWL->fMatPotAct = MATRIX_POTENTIAL(pWL->fContAct);
      pWL->fMatPotOld = pWL->fMatPotAct;
     }
  

  /*
  //for ground water table
  //adjust pressure potentials in the saturated zone if a water table is present:
  if ((pWa->iBotBC == (int)2)||(pWa->iBotBC == (int)5))
   {
	pWa->fGrdWatLvlPot = -(pSo->fDepth - pWa->fGrdWatLevel + pSo->fDeltaZ/(float)2);	
	//pWa->fGrdWatLvlPot = -(pSo->fDepth - pWa->fGrdWatLevel);	
	
      
	iNodeWaterTable = pSo->iLayers-2 - (int)(-pWa->fGrdWatLvlPot/DeltaZ);
    fDiff           = (pWa->fGrdWatLvlPot+(float)(pSo->iLayers-2-iNodeWaterTable)*DeltaZ);
    
	if (iNodeWaterTable < pSo->iLayers-2)
	{
	 pWL=pWa->pWLayer->pNext;
	 for(iL = 1; iL <= iNodeWaterTable-1; iL++) pWL = pWL->pNext;
     for(iL = iNodeWaterTable;((iL < pSo->iLayers-2)&&(pWL->pNext != NULL)); pWL = pWL->pNext,iL++)
	 {
      pWL->fMatPotAct = pWa->fGrdWatLvlPot-(fDiff-DeltaZ*(float)(iL-iNodeWaterTable));
	 }
	
   //first few nodes above the phreatic surface set to avoid large initial potential gradients:
	 pWL=pWa->pWLayer->pNext;
	 for(iL = 1; iL <= iNodeWaterTable-1; iL++) pWL = pWL->pNext;
	 for(iL=iNodeWaterTable-(int)1; iL >= max((int)1,iNodeWaterTable-(int)4); iL--,pWL = pWL->pBack)
	 {
      //pWL->fMatPotAct = (float)max(pWL->fMatPotAct,fDiff+DeltaZ*(float)(iNodeWaterTable-iL));
      //pWL->fMatPotAct = pWa->fGrdWatLvlPot-(fDiff-DeltaZ*(float)(iNodeWaterTable-iL));
      pWL->fMatPotAct = pWa->fGrdWatLvlPot+(fDiff-DeltaZ*(float)(iNodeWaterTable-iL));
	 }

	 for(H2O_SOIL_LAYERS)
     {
		pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
		pWL->fContOld = pWL->fContAct;

		pWL->fMatPotOld = pWL->fMatPotAct;
	 }
	}
   }//if water table
   */

  ///*
  //for ground water table
  //adjust pressure potentials in the saturated zone if a water table is present:
  if ((pWa->iBotBC == (int)2)||(pWa->iBotBC == (int)5)) // an dieser Stelle immer pWa->iBotBC = 1 !!!???
   {
	pWa->fGrdWatLvlPot = pSo->fDepth - pWa->fGrdWatLevel + pSo->fDeltaZ/(float)2;	
	//pWa->fGrdWatLvlPot = -(pSo->fDepth - pWa->fGrdWatLevel + pSo->fDeltaZ/(float)2);	
	     
	iNodeWaterTable = pSo->iLayers-2 - (int)(fabs(pWa->fGrdWatLvlPot)/DeltaZ);
    fDiff           = pWa->fGrdWatLvlPot-(float)(pSo->iLayers-2-iNodeWaterTable)*DeltaZ;
    
	if (iNodeWaterTable < pSo->iLayers-2)
	{
	 pWL=pWa->pWLayer->pNext;
	 for(iL = 1; iL <= iNodeWaterTable-1; iL++) pWL = pWL->pNext;
     for(iL = iNodeWaterTable;((iL < pSo->iLayers-2)&&(pWL->pNext != NULL)); pWL = pWL->pNext,iL++)
	 {
      pWL->fMatPotAct = fDiff+DeltaZ*(float)(iL-iNodeWaterTable);
	  //pWL->fMatPotAct = pWa->fGrdWatLvlPot+fDiff+DeltaZ*(float)(iL-iNodeWaterTable);
	 }
	
   //first few nodes above the phreatic surface set to avoid large initial potential gradients:
	 pWL=pWa->pWLayer->pNext;
	 for(iL = 1; iL <= iNodeWaterTable-1; iL++) pWL = pWL->pNext;
	 for(iL=iNodeWaterTable-(int)1; iL >= max((int)1,iNodeWaterTable-(int)4); iL--,pWL = pWL->pBack)
	 {
      pWL->fMatPotAct = (float)max(pWL->fMatPotAct,fDiff+DeltaZ*(float)(iNodeWaterTable-iL));
      //pWL->fMatPotAct = pWa->fGrdWatLvlPot-(fDiff-DeltaZ*(float)(iNodeWaterTable-iL));
      //pWL->fMatPotAct = pWa->fGrdWatLvlPot+(fDiff-DeltaZ*(float)(iNodeWaterTable-iL));
      //pWL->fMatPotAct = -((float)max((float)2.0,fDiff+DeltaZ*(float)(iNodeWaterTable-iL)));
	 }

	 for(H2O_SOIL_LAYERS)
     {
		pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
		pWL->fContOld = pWL->fContAct;

		pWL->fMatPotOld = pWL->fMatPotAct;
	 }
	}
   }//if water table
    
   //*/

	/* Auf die virtuelle erste Simulationsschicht werden
	   Werte der zweiten Simulationsschicht geschrieben  */

	pWa->pWLayer->fMatPotOld = pWa->pWLayer->pNext->fMatPotOld;
	pWa->pWLayer->fMatPotAct = pWa->pWLayer->pNext->fMatPotAct;


	/* Auf die virtuelle letzte Simulationsschicht werden
	   Werte der vorletzten Simulationsschicht geschrieben  */

	pWL->fMatPotOld = pWL->pBack->fMatPotOld;
	pWL->fMatPotAct = pWL->pBack->fMatPotAct;


  return 1;
}  

/**********************************************************************************
 *  Name     : initClimate()                               
 *
 *  Author  : cs/ibs, ch/gsf                               
 *  Date    : 3.12.96                                   
 *---------------------------------------------------------------------------------
 *  Changed   Date    Author         Changes
 *********************************************************************************/
int initClimate(EXP_POINTER)
{ 
	int  idummy;

	PHLAYER		pHL;
	PWEATHER	pCW;
	
/*    Zu klaeren ob integriert werden soll
		for (pCW = pCl->pWeather;
		(pCW != NULL);
		pCW = pCW->pNext)
		{
		//pCW->lDate = i++;
		pCW->fRainRate =  pCW->fRainAmount;
		}
*/

		/* Derzeit werden die aus der DB gelesenen Daten nicht auf
		   Plausibilitaet geprueft. Dies wird hier nachgeholt.    */
		for (pCW = pCl->pWeather;
		(pCW != NULL);
		pCW = pCW->pNext)
		{
			/* ch, 27.5.97  setze nicht belegte Werte auf -99 */
			if (pCW->fSolRad == (float)-99.99) pCW->fSolRad = (float)-99;
			if (pCW->fSunDuration == (float)-9.99) pCW->fSunDuration = (float)-99;
			if (pCW->fTempMax == (float)-99.9) pCW->fTempMax = (float)-99;
			if (pCW->fTempMin == (float)-99.9) pCW->fTempMin = (float)-99;
			if (pCW->fTempAve == (float)-99.9) pCW->fTempAve = (pCW->fTempMax + pCW->fTempMin)*(float)0.5;
			//if (pCW->fHumidity == (float)99.9) pCW->fHumidity = (float)-99;
			if (pCW->fPanEvap == (float)999.9) pCW->fPanEvap = (float)-99; 
			if ((pCW->fWindSpeed == (float)-9.9)||(pCW->fWindSpeed == (float)99.9)) pCW->fWindSpeed = (float)-99;
			if (pCW->fDewPoint == (float)-9.9) pCW->fDewPoint = (float)-99;
			if (pCW->fSatDef == (float)999.9) pCW->fSatDef = (float)-99;
			if (pCW->fSoilTemp5 == (float)-99.9) pCW->fSoilTemp5 = (float)-99;
			if (pCW->fSoilTemp10 == (float)-99.9) pCW->fSoilTemp10 = (float)-99;
			if (pCW->fSoilTemp20 == (float)-99.9) pCW->fSoilTemp20 = (float)-99;
			if (pCW->fRainAmount == (float)-99.9) pCW->fRainAmount = (float)-99;
			if (pCW->fSnow == (float)9999.90) pCW->fSnow = (float)-99;
		}
                                                                  
	/* Initialisierung der akt. Temperatur der letzten fuenf Tage 
	   und der kumulativen Temperatur der letzten fuenf Tage. Diese
	   werden fuer die Berechnung der aktuellen Bodentemperatur nach
	   Williams benoetigt. */

   pCl->fCumTempL5Days 	= (float)0.0;  
   for (idummy =0;idummy<5;idummy++){
      pCl->afTempLast5Days[idummy] = (pCl->pWeather->fTempMax + pCl->pWeather->fTempMin)
                                   /(float)2.0;
     pCl->fCumTempL5Days +=    pCl->afTempLast5Days[idummy];
   }

	/* Wenn keine langjaehrige Durchschnittemperatur oder Monatsdurchschnittstemeratur
	   eingelesen worden ist, wird diese hier initialisiert und eine Meldung in Log-File
	   geschrieben. */                    

		SET_IF_99(pCl->pAverage->fYearTemp,(float)8.5);
		SET_IF_99(pCl->pAverage->fMonthTempAmp,(float)17.0);


	/* Bodentemperaturen zu Beginn initialisieren*/                    
		for(pHL= pHe->pHLayer->pNext;
		    pHL->pNext != NULL;
		    pHL = pHL->pNext)
		{
		    pHL->fSoilTempOld = pHL->fSoilTemp;
		}  /* Ende beschreiben Bodentemperatur	*/										
						
 return 1;
} /*-----------  init Klimadaten Ende   ----------------------*/

/********************************************************************************
 *  Name     : initChemistry()                               
 *  Author   : cs/ibs                               
 *  Date     : 3.12.96 
 *------------------------------------------------------------------------------
 *  Changed   Date    Author         Topic
 *
 *
 *******************************************************************************/
int initChemistry(EXP_POINTER)
{ 
  float tiefe = (float)0;
  int iLayer;

  PSLAYER			pSL;
  PCLAYER			pCL;
  PCPARAM			pPA;

  
   /* Schalter fuer die Korrektur der Simulationswerte
      auf den ersten Messwert.  */
  
	  bCorrectMeasure = FALSE;

     /* Initialisieren der Pools fuer die fr. org. Substanz 
        und die Humuspools der Simulationsschichten.  */

		for( pSL=pSo->pSLayer,
			 pCL=pCh->pCLayer;((pSL->pNext!=NULL)&&
		     				   (pCL->pNext!=NULL));
		       pSL=pSL->pNext,
		       pCL=pCL->pNext)
		{
		   pCL->fPpmTOkg = pSL->fBulkDens * pSL->fThickness * (float)0.01;  //  factor 10 cm/mm
		}                                    


     /* Initialisieren der Pools fuer die fr. org. Substanz 
        und die Humuspools der Bodenschichten.  */

	 	initSoilOrganicMatter(exp_p);

	/* Werden fuer bestimmte konstante Zerfallsraten oder Potentiale keine
		Werte eingelesen, muessen fuer einen ordnungsgemaessen Ablauf der
		Simulation diese Werte standardmaessig initialisiert werden.  */
		
    for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext;
         ((pSL->pNext != NULL)&&
          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext)
		{
        SET_IF_99(pCL->fN2ONGasConc,(float)2.07e-4);
		SET_IF_99(pCL->fN2ONGasConcOld,(float)2.07e-4);
		SET_IF_99(pCL->fHumusMinerMaxR,(float)1.0e-04);//8.3e-05);
		SET_IF_99(pCL->fNH4NitrMaxR,(float)1.0);//0.1);
		SET_IF_99(pCL->fHumusDecR,(float)7.0e-05);
		SET_IF_99(pCL->afFOMDecR[0],(float)0.2);
		SET_IF_99(pCL->afFOMDecR[1],(float)0.05);			
        SET_IF_99(pCL->afFOMDecR[2],(float)0.0095);			
		//SET_IF_99(pCL->fNFOMFast,(pCL->fNLitter * (float)0.4 * (float)0.98));			
		//SET_IF_99(pCL->fCFOMFast,(pCL->fCLitter * (float)0.4 * (float)0.98));
		//SET_IF_99(pCL->fNFOMSlow,(pCL->fNLitter * (float)0.6 * (float)0.98));
		//SET_IF_99(pCL->fCFOMSlow,(pCL->fCLitter * (float)0.6 * (float)0.98));
		//SET_IF_99(pCL->fNMicBiomFast,(pCL->fCLitter * (float)0.5 * (float)0.02 / (float)8.0));
		//SET_IF_99(pCL->fCMicBiomFast,(pCL->fCLitter * (float)0.5 * (float)0.02));
		//SET_IF_99(pCL->fNMicBiomSlow,(pCL->fCLitter * (float)0.5 * (float)0.02 / (float)8.0));
		//SET_IF_99(pCL->fCMicBiomSlow,(pCL->fCLitter * (float)0.5 * (float)0.02));
		SET_IF_99(pCL->fNMicBiomDenit,(pCL->fCLitter * (float)0.5 * (float)0.02 / (float)8.0));
		SET_IF_99(pCL->fCMicBiomDenit,(pCL->fCLitter * (float)0.5 * (float)0.02));
		//SET_IF_99(pCL->fNHumusFast,(pSL->fNHumus * (float)0.25));
		//SET_IF_99(pCL->fCHumusFast,(pSL->fCHumus * (float)0.25));
		//SET_IF_99(pCL->fNHumusSlow,(pSL->fNHumus * (float)0.75));
		//SET_IF_99(pCL->fCHumusSlow,(pSL->fCHumus * (float)0.75));
		//SET_IF_99(pCL->fCHumusStable,(float)0.0);
		//SET_IF_99(pCL->fNHumusStable,(float)0.0);
		SET_IF_99(pCL->fLitterMinerMaxR,(float)0.02);
		SET_IF_99(pCL->fManureMinerMaxR,(float)0.01);
		SET_IF_99(pCL->fMicBiomCN,(float)8.0);
		SET_IF_99(pCL->fFOMVeryFastDecMaxR,(float)0.25 * (float)0.015);
		SET_IF_99(pCL->fFOMFastDecMaxR,(float)0.074 * (float)0.015);
		SET_IF_99(pCL->fFOMSlowDecMaxR,(float)0.02 * (float)0.015);
		SET_IF_99(pCL->fMicBiomFastDecMaxR,(float)0.33 * (float)0.015);
		SET_IF_99(pCL->fMicBiomSlowDecMaxR,(float)0.04 * (float)0.015);
		SET_IF_99(pCL->fHumusFastMaxDecMaxR,(float)0.16 * (float)0.015);
		SET_IF_99(pCL->fHumusSlowMaxDecMaxR,(float)0.006 * (float)0.015);
		SET_IF_99(pCL->fUreaHydroMaxR,(float)1.36);

        SET_IF_99(pCL->fHumusToDONMaxR,(float)0);
		SET_IF_99(pCL->fLitterToDONMaxR,(float)0);
		SET_IF_99(pCL->fManureToDONMaxR,(float)0);
		SET_IF_99(pCL->fDONToHumusMaxR,(float)0);
		SET_IF_99(pCL->fDONToLitterMaxR,(float)0);
		SET_IF_99(pCL->fDONMinerMaxR,(float)0);
		}

	/* Tiefenabhaengige Initialisierung. Bis 30 cm 
	   Bodentiefe wird mit anderem Wert gerechnet.  */

	for( tiefe = (float)0,
		 pSL=pSo->pSLayer,
		 pCL=pCh->pCLayer;((pSL->pNext!=NULL)&&
	     				   (pCL->pNext!=NULL));
   		 tiefe += pSL->fThickness,
		 pSL=pSL->pNext,
	     pCL=pCL->pNext)
       {
		 if (tiefe <= (float)300)
		 {                                  
		/* als Standard wird hier als max. Denitrifikationsrate 1kg/ha 
		   auf die Schichten von 0-30cm aufgeteilt. */
		  SET_IF_99(pCL->fNO3DenitMaxR,((float)1 * pSL->fThickness /(float)300));
		 
		 //Umrechnung in Konzentration pro Liter pro Tag:*100/theta/DeltaZ;theta=0.25
		 // SET_IF_99(pCL->fNO3DenitMaxR,(float)1/(float)0.75);
		 
		 //maximal denitrification rate in mg per liter soil solution per day
		 // SET_IF_99(pCL->fNO3DenitMaxR,(float)0.5);//1/(3*0.667)
         }
         else
         {
		  SET_IF_99(pCL->fNO3DenitMaxR,(float)0.0);
         }
	   }

	/* Initialisierung von Modellparametern */           
		
	    for (SOIL_LAYERS0(pPA,pCh->pCParam))
		{
		SET_IF_99(pPA->fMolDiffCoef,(float)120.0);
		SET_IF_99(pPA->fBioMassFastCN,(float)20.0);
		SET_IF_99(pPA->fBioMassSlowCN,(float)20.0);
		SET_IF_99(pPA->fBioMassDenitCN,(float)20.0);
		SET_IF_99(pPA->fDispersivity,(float)100.0);
		SET_IF_99(pPA->fNitrifPot,(float)0.1);
		SET_IF_99(pPA->fMinerThetaMin,(float)0.08);
		SET_IF_99(pPA->fMinerEffFac,(float)0.4);
		SET_IF_99(pPA->fMinerHumFac,(float)0.2);
		SET_IF_99(pPA->fMinerTempB,(float)20.0);
		SET_IF_99(pPA->fMinerQ10,(float)2.0);
		SET_IF_99(pPA->fMinerSatActiv,(float)0.6);
		SET_IF_99(pPA->fMaintCoeff,(float)0.1824);
		SET_IF_99(pPA->fYieldCoeff,(float)0.503);
		SET_IF_99(pPA->fBioFastMaxGrowR,(float)3.4);
		SET_IF_99(pPA->fN2ORedMaxR,(float)7);
		SET_IF_99(pPA->fDenitThetaMin,(float)0.6);
		SET_IF_99(pPA->fDenitKsNO3,((float)0.1 * pSo->pSLayer->pNext->fThickness));
		//SET_IF_99(pPA->fDenitKsNO3,(float)0.1);
		//SET_IF_99(pPA->fDenitKsNO3,(float)40.0);//assuming 0.25 avg. vol. wc. 
		SET_IF_99(pPA->fNitrifNO3NH4Ratio,(float)25.0);
		SET_IF_99(pPA->fDenitMaxGrowR,(float)3.4);
		SET_IF_99(pPA->fDenitKsCsol,(float)1.7);
		SET_IF_99(pPA->fDenitMaintCoeff,(float)0.2);
		SET_IF_99(pPA->fDenitYieldCoeff,(float)0.5);
		
		SET_IF_99(pPA->afKd[0],(float)1.0);
		SET_IF_99(pPA->afKd[1],(float)0.92);
		SET_IF_99(pPA->afKd[2],(float)0.0);
		SET_IF_99(pPA->afKd[3],(float)0.0);
		SET_IF_99(pPA->afKd[4],(float)0.0);
		SET_IF_99(pPA->afKd[5],(float)0.0);
		
		SET_IF_99(pPA->afKh[0],(float)0.0);
		SET_IF_99(pPA->afKh[1],(float)0.0);
		SET_IF_99(pPA->afKh[2],(float)0.0);
		SET_IF_99(pPA->afKh[3],(float)0.0);
		SET_IF_99(pPA->afKh[4],(float)0.0);
		SET_IF_99(pPA->afKh[5],(float)0.0);
		}

        SET_IF_99(pCh->pCProfile->fNH3VolatMaxR,(float)0.1);
        SET_IF_99(pCh->pCProfile->fCH4ImisR,(float)0.027);
        SET_IF_99(pCh->pCProfile->fCH4ImisDay,(float)0.014);

 return 1;
} /*-----------  init Chemie   ----------------------*/


/******************************************************************************************
 *  Procedure    : initSoilOrganicMatter()                                                *
 *  Author       : cs/ibs                                                                 *
 *  Date         : 3.12.96                                                                *
 *----------------------------------------------------------------------------------------*
 *  Changed   Date      Author              Topic                                         *
 *            23.02.97  ts/tum															  *
 *			  22.06.01	ab&ep/gsf
 *****************************************************************************************/
int  initSoilOrganicMatter(EXP_POINTER)
{ 

    float f1,actDepth,RootProp,RootSum;
    
	float fCFOMVF, fCFOMF, fCFOMS,fNFOMVF, fNFOMF, fNFOMS;
	float fCMBF, fCMBS, fNMBF, fNMBS;
	float fCHF, fCHS, fCHR, fNHF, fNHS, fNHR;

	float fCN_AOM1,fCN_AOM2,fCN_AOM3,fCN_BOM1,fCN_BOM2;

    float AddCN,fCTotal,fNTotal;
    //float AnteilCFOMSlow,AnteilCFOMFast,AnteilCFOMVeryFast;
    //float AnteilCMicBiomSlow,AnteilCMicBiomFast;
    //float AnteilCHumusFast,AnteilCHumusSlow,AnteilCHumusStable;
    
	//float fCSlow,fNSlow;
    float fCFast,fNFast;
    //float CAnteilLitter,CAnteilManure,NAnteilLitter,NAnteilManure;


	PSLAYER		  pSL;
	PCLAYER		  pCL;

    RootProp = (float)0.0;
    RootSum  = (float)0.0;
    actDepth = (float)0.0;

	fCN_AOM1 = (float)200;
	fCN_AOM2 = (float) 50;
	fCN_AOM3 = (float)  5;

	fCN_BOM1 = (float)  8;
	fCN_BOM2 = (float)  8;



	/* Humusgehalte aus den Bodenbasisdaten zum Gehalt organischer Substanz
	
	In der Datenbank werden der Humusgehalt und der Gehalt an Kohlenstoff
	und Stickstoff im Humus als Gewichts-Prozent gespeichert. Bis zu dieser Funktion
	wurde mit diesen Prozentangaben gerechnet. Fuer die verwendeten
	Modelle ist es notwendig die Humusfraktionen in Mengen zu ueberfuehren. 
	Hierzu muss neben der Lagerungsdichte auch der Flaechenbezug berueck-
	sichtigt werden. (10000 m2; Schichtdicke in mm; Lagerungsdichte in t/m3).
	Als Umrechnungsfaktor bleibt dann 100 erhalten. Weiterhin muss der Steingehalt
	bei der Berechnung des Bodengewichtes beuecksichtigt werden. Der Gehalt
	an organischer Substanz bezieht sich auf den Feinbodenanteil. */

    for (pSL= pSo->pSLayer->pNext;(pSL->pNext!=NULL);pSL=pSL->pNext)
	    {
            /* Masse des Feinbodens pro ha */
            f1 = pSL->fBulkDens * pSL->fThickness * 100 * ((float)1 -  pSL->fRockFrac); 
            
            pSL->fHumus  *= f1; 
            pSL->fCHumus *= f1; 
            pSL->fNHumus *= f1; 
            
	    }


    /* Pflanzenrueckstaende (Litter-Pools) aus der Vorfrucht 
   
    Am Starttag der Simulation wird geprueft, ob die Ernte der
    Vorfrucht bereits erreicht ist. Wenn ja erfolgt in der Funktion
    TSComparePreCropDate die Initialisierung. Falls nein	erfolgt die
    Beruecksichtigung der Pflanzenrueckstaende durch Aufruf von
    TSComparePreCropDate zu spaeteren Termin aus time1.c heraus
	   
    */     

	
	if ((int)pTi->pSimTime->fTimeAct == (int)pMa->pLitter->iDay) 
	{
		TSComparePreCropDate(exp_p);
	}
	else
	{

	/* Wenn Angaben zu den Ernterueckstaenden in der Datenbank fehlen kommt es
	   in dieser Funktion zum Programmabsturz aufgrund Division by zero. Daher
	   findet eine letzte Ueberpruefung statt und es werden Werte ungleich Null
	   initialisiert. */     

		if (pMa->pLitter->fTopCN <= (float)0.0)    
		   pMa->pLitter->fTopCN = (float)0.1;    
		if (pMa->pLitter->fRootCN <= (float)0.0)    
		   pMa->pLitter->fRootCN = (float)0.1;    

		pMa->pLitter->fTotalAmount = (float)0.0;
		pMa->pLitter->fTopC = (float)0.0;
		pMa->pLitter->fRootC = (float)0.0;
		pMa->pLitter->fTotalC = (float)0.0;		
		pMa->pLitter->fTotalCN = (float)0.1;

		SET_IF_99(pCh->pCProfile->fCLitterSurf,(float)0);
		SET_IF_99(pCh->pCProfile->fNLitterSurf,(float)0);

		SET_IF_99(pCh->pCProfile->fCManureSurf,(float)0);
		SET_IF_99(pCh->pCProfile->fNManureSurf,(float)0);
		
		SET_IF_99(pCh->pCProfile->fCHumusSurf,(float)0);
		SET_IF_99(pCh->pCProfile->fNHumusSurf,(float)0);

		SET_IF_99(pCh->pCProfile->fCStandCropRes,(float)0);
		SET_IF_99(pCh->pCProfile->fNStandCropRes,(float)0);

		/* Berechnen schichtmaessiger Anteil. Zur Vereinfachung 
	   	  Variable pCL->fCLitter benutzt um Anteile zu uebergeben. */ 
    for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext)
		{
		  SET_IF_99(pCL->fCLitter,(float)0.0);
		}

		/* Anteilsmaessige Verteilung auf die Bodenschichten. */ 
    for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext)
		{
	   /* wurde auf -99 initialisiert; wenn nein in Modellparameter-
	      File eingelesen und sollte nicht ueberschrieben werden   */ 

		  SET_IF_99(pCL->fCLitter,(float)0.0);
		  SET_IF_99(pCL->fNLitter,(float)0.1);
		  pCL->fLitterCN =(float)0.1;
	  	
        }
	}

	
	
	/***********************************************************************/
	/* Initialisierung der Mengen in den spezifischen DNDC-Pools           */
	/***********************************************************************/

	//schichtweise Berechnung
    for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext)
	{

		 /* Summe der zu verteilenden Mengen */ 
	 fCTotal =  pCL->fCLitter +  pSL->fCHumus;
	 fCFast  = (float)0.15 * fCTotal;

	 fNTotal =  pCL->fNLitter +  pSL->fNHumus;
	 fNFast  = (float)0.15 * fNTotal;

    /* Initialisierung der Residue-Pools Resistant, Labile, VeryLabile*/
	if(fNFast)
       AddCN = fCFast/fNFast;
    else 
  	   AddCN = (float)0.0;
   
  	if(AddCN >= fCN_AOM3 && AddCN < fCN_AOM2)
  	 {
 	  fCFOMVF = (fCFast * (1/AddCN - 1/fCN_AOM3)) / (1/fCN_AOM2 - 1/fCN_AOM3);
      fCFOMF = fCFast - fCFOMVF;
 	  fCFOMS = (float)0.0;	
 	 }
   
 	 if(AddCN >= fCN_AOM2 && AddCN < fCN_AOM1)
 	 {
      fCFOMF = (fCFast * (1/AddCN - 1/fCN_AOM2)) / (1/fCN_AOM1 - 1/fCN_AOM2);
      fCFOMS = fCFast - fCFOMF;
      fCFOMVF = (float)0.0;	
     }
  
     if ((AddCN>=(float)0) && (AddCN<=(float)5))
     {
	  fCFOMF = (float)0.0;
      fCFOMS = (float)0.0;
      fCFOMVF = (float)0.0;
     }

    if (AddCN > fCN_AOM1)
    {
     Message(3,START_DATA_ERROR_TXT);
    }

    fNFOMVF = fCFOMVF / fCN_AOM3;
    fNFOMF  = fCFOMF  / fCN_AOM2;
    fNFOMS  = fCFOMS  / fCN_AOM1;

	
	//initialisieren der globalen Variablen
	SET_IF_99(pCL->fCFOMVeryFast,fCFOMVF);
	SET_IF_99(pCL->fCFOMFast,fCFOMF);
	SET_IF_99(pCL->fCFOMSlow,fCFOMS);

    SET_IF_99(pCL->fNFOMVeryFast,fNFOMVF);			
    SET_IF_99(pCL->fNFOMFast,fNFOMF);			
    SET_IF_99(pCL->fNFOMSlow,fNFOMS);
    
	
	/* Initialisieren der Biomasse Pools */
    fCMBF = (float)0.15 * (float)0.02 * (float)0.9 * fCTotal;
    fCMBS = (float)0.15 * (float)0.02 * (float)0.1 * fCTotal;
    
	fNMBF = 1/fCN_BOM2 * fCMBF;
	fNMBS = 1/fCN_BOM1 * fCMBS;

	SET_IF_99(pCL->fCMicBiomFast,fCMBF);
	SET_IF_99(pCL->fCMicBiomSlow,fCMBS);
    SET_IF_99(pCL->fNMicBiomFast,fNMBF);
	SET_IF_99(pCL->fNMicBiomSlow,fNMBS);
	
	
	/* Restmengen berechnen */
	fCTotal -= (pCL->fCMicBiomSlow + pCL->fCMicBiomFast +
	            pCL->fCFOMFast + pCL->fCFOMSlow + pCL->fCFOMVeryFast);
    fNTotal -= (pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
	            pCL->fNFOMFast + pCL->fNFOMSlow + pCL->fNFOMVeryFast);
	
	/*
	Initialisieren der "Humads" und Humus Pools
	bezogen auf den gesamten org. C (Humus + Litter)
	fCTotal ist der noch nicht auf Pools 
	verteilte Rest des gesamten org. C
	*/ 
	
	if ((fCTotal)&&(fNTotal))
	  {
	   pSL->fHumusCN = fCTotal/fNTotal;
	   fCHS   = (float)0.146 * fCTotal;//0.15 * 0.98 * 0.84 des gesamten org. C
       fCHF   = (float)0.028 * fCTotal;//0.15 * 0.98 * 0.16 des gesamten org C
	   fCHR = (float)0.826 * fCTotal;  //0.7 des gesamten org. C
	   fNHS = fCHS / pSL->fHumusCN;
       fNHF = fCHF / pSL->fHumusCN;
	   fNHR = fCHR / pSL->fHumusCN;
	  }

		
    SET_IF_99(pCL->fCHumusFast,fCHF);
    SET_IF_99(pCL->fCHumusSlow,fCHS);
    SET_IF_99(pCL->fCHumusStable,fCHR);
	SET_IF_99(pCL->fNHumusFast,fNHF);
    SET_IF_99(pCL->fNHumusSlow,fNHS);
    SET_IF_99(pCL->fNHumusStable,fNHR);

   }// end for zur schichtweisen Berechnung


  return 1;
}

/******************************************************************************************
 * Procedur : initHumus()                                                                 *
 *                                                                                        *
 *  Author  : cs/ibs                                                                      *
 *  Date    : 15.11.95                                                                    *
 *----------------------------------------------------------------------------------------*
 *  Changed   Date      Author         Topic                                              *
 *            23.02.97  ts/tum                                                            *
 ******************************************************************************************/
int initHumus(EXP_POINTER)
{   
	float f1;
    
   PSLAYER			pSL=NULL;
   PCLAYER			pCL=NULL;
   

	/* In der Datenbank werden der Humusgehalt und der Gehalt an Kohlenstoff
	   und Stickstoff im Humus als Prozent gespeichert. Bis zu dieser Funktion
	   initHumus wurde mit diesen Prozentangaben gerechnet. Fuer die verwendeten
	   Modelle ist es notwendig die Humusfraktionen in Mengen zu ueberfuehren. 
	   Hierzu muss neben der Lagerungsdichte auch der Flaechenbezug berueck-
	   sichtigt werden. (10000 m2; Schichtdicke in mm; Lagerungsdichte in t/m3).
	   Als Umrechnungsfaktor bleibt dann 100 erhalten. Weiterhin muss der Steingehalt
	   bei der Berechnung des Bodengewichtes beuecksichtigt werden. Der Gehalt
	   an organischer Substanz bezieht sich auf den Feinbodenanteil. */

    for (pSL= pSo->pSLayer->pNext;(pSL->pNext!=NULL);pSL=pSL->pNext)
	    {
            /* Masse des Feinbodens pro ha */
            f1 = pSL->fBulkDens * pSL->fThickness * 100 * ((float)1 -  pSL->fRockFrac); 
            
            pSL->fHumus  *= f1; 
            pSL->fCHumus *= f1; 
            pSL->fNHumus *= f1; 
            
	    }

 return 1;
}
                         
                         
/******************************************************************************************
 * Procedur : initCumValue()
 *
 *  Author  : ts/tum                               
 *  Date    : 17.02.97                                  
 *-----------------------------------------------------------------------------------------
 *  Changed   Date    Author         Topic
 ******************************************************************************************/
int initCumValue(EXP_POINTER)
{   
	int iLayer;

	PWLAYER pWL;
	PCLAYER pCL;

	/* Initialisierung Bilanzvariablen */
 
    pWa->pWBalance->fProfilStart = (float)0.0;
    pWa->pWBalance->fBalance     = (float)0.0;
     
     for (SOIL_LAYERS1(pWL,pWa->pWLayer->pNext)) // Schicht 1 bis n-1
        {
        pWa->pWBalance->fProfilStart += pWL->fContAct * pSo->fDeltaZ;  
        }

     pCh->pCBalance->fNProfileStart = (float)0.0;
     pCh->pCBalance->dNBalance     = (double)0.0;
     
     for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
	    {
	    pCh->pCBalance->fNProfileStart  += pCL->fNO3N + pCL->fNH4N + pCL->fUreaN +  pCL->fN2ON + pCL->fN2N + pCL->fNON;
        }

	/* Initialisierung von kumulativen Variablen */

	/* Water */	
	 pCl->fCumRain 								= (float)0.0;                 
	 pWa->fRunOffCum 							= (float)0.0;                 
	 pWa->fCumLeaching 							= (float)0.0;                 
	 pWa->fCumInfiltration 						= (float)0.0;                 
	 pWa->fActETCum    							= (float)0.0;                 
	 pWa->fPotETCum    							= (float)0.0;                 

	 pWa->pWBalance->fInput 					= (float)0.0;               
	 pWa->pWBalance->fOutput 					= (float)0.0;               

	 pCh->pCBalance->dNBalCorrect				= (double)0.0;               
	 
	 pWa->pWBalance->fPotCumEvap  				= (float)0.0;
	 pWa->pWBalance->fActCumEvap  			    = (float)0.0;
     pWa->pWBalance->fPotCumTransp              = (float)0.0;
     pWa->pWBalance->fActCumTransp 				= (float)0.0;
	 
	 pWa->pWBalance->fCumInterception           = (float)0.0;          
     pWa->fActTranspCum 						= (float)0.0;

	// Nur noch bis Einlesen aus DB	die dritte Struktur !!!!!
	 if (pPl != NULL)
	   {
	    pPl->pPltWater->fCumUptake 				= (float)0.0;                 
		pPl->pPltNitrogen->fCumActNUpt 			= (float)0.0;
       }                                                  
       
	/* Chemistry */	
	 pCh->pCProfile->dNMinerCum 				= (double)0.0;
	 pCh->pCProfile->dNLitterMinerCum			= (double)0.0;
	 pCh->pCProfile->dNManureMinerCum			= (double)0.0;
	 pCh->pCProfile->dNHumusMinerCum			= (double)0.0;

	 pCh->pCProfile->dNO3DenitCum    			= (double)0.0;
	 pCh->pCProfile->dN2ODenitCum			 	= (double)0.0;
	 pCh->pCProfile->dNH4NitrCum		     	= (double)0.0;
	 pCh->pCProfile->dNImmobCum     	 		= (double)0.0;         

	 pCh->pCProfile->dNUptakeCum	 			= (double)0.0;

	 pCh->pCBalance->dNInputCum   	 	 		= (double)0.0;
	 
	 pCh->pCBalance->dNOutputCum   	 	 		= (double)0.0;
	 pCh->pCProfile->dN2OEmisCum   	 	 		= (double)0.0;
	 pCh->pCProfile->dNOEmisCum   	 	 		= (double)0.0;
	 pCh->pCProfile->dN2EmisCum   	 	 		= (double)0.0;
	 pCh->pCProfile->dNTotalLeachCum 			= (double)0.0;
	 pCh->pCProfile->dNH3VolatCum    			= (double)0.0;
	 pCh->pCProfile->dUreaLeachCum 				= (double)0.0;
	 pCh->pCProfile->dNH4LeachCum 				= (double)0.0;
	 pCh->pCProfile->dNO3LeachCum 				= (double)0.0;
	 pCh->pCProfile->dN2ODrainCum   			= (double)0.0;
 
 return 1;
}




/* from h2o_fct.c */
/* Hydraulic Functions */
extern double HC_WCont(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double HC_HCond(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double HC_DWCap(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double HC_MPotl(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);

extern double VG_WCont(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double VG_HCond(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double VG_DWCap(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double VG_MPotl(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);

extern double M2_WCont(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double M2_HCond(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double M2_DWCap(double,double,double,double,double,double,double,double,double,double,
                       double,double,double,double,double,double,double,double,double,double);
extern double M2_MPotl(double,double,double,double,double,double,double,double,double,double,
       		           double,double,double,double,double,double,double,double,double,double);

extern double BC_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double BC_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double BC_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double BC_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

extern double C2_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double C2_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double C2_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double C2_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

extern double BG_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double BG_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double BG_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
extern double BG_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);


PFCT			WCont,
				HCond,
				DWCap,
				MPotl;
				
/********************************************************************************
 *  Name     : initHydFunctions()                            
 *                                                        
 *                                                        
 *  Author  : ch/gsf                 
 *  Date    : 2.10.97                                   
 *-------------------------------------------------------------------------------
 *  Changed   Date    Author         Topic
 *
 *
 ********************************************************************************/
int initHydFunctions(int i)
 { 
switch(i)
{
case 0: 
	Message(1,"Hydraulic Functions: Hutson & Cass");
 	WCont = HC_WCont;
	HCond = HC_HCond;
	DWCap = HC_DWCap;
	MPotl = HC_MPotl;
    break;
    
case 1: 
	Message(1,"Hydraulic Functions: VanGenuchten");
 	WCont = VG_WCont;
	HCond = VG_HCond;
	DWCap = VG_DWCap;
	MPotl = VG_MPotl;
    break;
    
case 2: 
	Message(1,"Hydraulic Functions: Brooks & Corey");
 	WCont = BC_WCont;
	HCond = BC_HCond;
	DWCap = BC_DWCap;
	MPotl = BC_MPotl;
    break;

case 3: 
 	/*
	Message(1,"Hydraulic Functions: Clapp & Hornberger");
 	WCont = CH_WCont;
	HCond = CH_HCond;
	DWCap = CH_DWCap;
	MPotl = CH_MPotl;
    */
    /*
	Message(1,"Hydraulic Functions: Campbell Bimodal Type");
 	WCont = C2_WCont;
	HCond = C2_HCond;
	DWCap = C2_DWCap;
	MPotl = C2_MPotl;
    break;
    */
	Message(1,"Hydraulic Functions: Brutsaert & Gardner");
 	WCont = BG_WCont;
	HCond = BG_HCond;
	DWCap = BG_DWCap;
	MPotl = BG_MPotl;
    break;

case 4: 
	Message(1,"Hydraulic Functions: VanGenuchten Bimodal Type");
 	WCont = M2_WCont;
	HCond = M2_HCond;
	DWCap = M2_DWCap;
	MPotl = M2_MPotl;          // Keine bimodale Potentialfunktion vorhanden 
    break;
    
default: 
	Message(1,"Error! No hydraulic Functions selected!");
	Message(1,"Hydraulic Functions: Hutson & Cass");
 	WCont = HC_WCont;
	HCond = HC_HCond;
	DWCap = HC_DWCap;
	MPotl = HC_MPotl;
}
    
 return 1;
}  

////////////////////
// Name:    SoilCode(float fClay, float fSilt, float fSand, char acSoilCode)
//
// Methode: Weist anhand der Texturdaten die ADV-Kennung
//          des Bodens nach der Bodenkartieranleitung zu
//
// Author:  ep/gsf 150399
/////////////////////////////////////////

int	SoilCode(float fClay, float fSilt, float fSand, char* acSoilCode)
{
 int iSoilNr;

 if ((fClay>=0)&&(fClay<5)&&(fSilt>=0)&&(fSilt<10))
 {  
  lstrcpy(acSoilCode,"S");
  iSoilNr=1;
 }  

 if ((fClay>=0)&&(fClay<5)&&(fSilt>=10)&&(fSilt<25))
 {  
  lstrcpy(acSoilCode,"Su2");
  iSoilNr=2;
 }  

 if ((fClay>=0)&&(fClay<8)&&(fSilt>=25)&&(fSilt<40))
 {  
  lstrcpy(acSoilCode,"Su3");
  iSoilNr=3;
 }  

 if ((fClay>=0)&&(fClay<8)&&(fSilt>=40)&&(fSilt<50))
 {  
  lstrcpy(acSoilCode,"Su4");
  iSoilNr=4;
 }  

 if ((fClay>=8)&&(fClay<15)&&(fSilt>=40)&&(fSilt<50))
 {  
  lstrcpy(acSoilCode,"Slu");
  iSoilNr=5;
 }  

 if ((fClay>=5)&&(fClay<8)&&(fSilt>=5)&&(fSilt<25))
 {  
  lstrcpy(acSoilCode,"Sl2");
  iSoilNr=6;
 }  

 if ((fClay>=8)&&(fClay<12)&&(fSilt>=7)&&(fSilt<40))
 {  
  lstrcpy(acSoilCode,"Sl3");
  iSoilNr=7;
 }  

 if (((fClay>=12)&&(fClay<15)&&(fSilt>=13)&&(fSilt<40))
	 ||((fClay>=15)&&(fClay<17)&&(fSilt>=13)&&(fSilt<35)))
 {  
  lstrcpy(acSoilCode,"Sl4");
  iSoilNr=8;
 }  

 if (((fClay>=5)&&(fClay<8)&&(fSilt>=0)&&(fSilt<5))
	 ||((fClay>=8)&&(fClay<12)&&(fSilt>=0)&&(fSilt<7))
	 ||((fClay>=12)&&(fClay<15)&&(fSilt>=0)&&(fSilt<13)))
 {  
  lstrcpy(acSoilCode,"St2");
  iSoilNr=9;
 }  

 if (((fClay>=15)&&(fClay<17)&&(fSilt>=0)&&(fSilt<13))
	 ||((fClay>=17)&&(fClay<25)&&(fSilt>=0)&&(fSilt<15)))
 {  
  lstrcpy(acSoilCode,"St3");
  iSoilNr=10;
 }  

 if ((fClay>=0)&&(fClay<8)&&(fSilt>=80)&&(fSilt<=100))
 {  
  lstrcpy(acSoilCode,"U");
  iSoilNr=11;
 }  

 if ((fClay>=0)&&(fClay<8)&&(fSilt>=50)&&(fSilt<80))
 {  
  lstrcpy(acSoilCode,"Us");
  iSoilNr=12;
 }  

 if ((fClay>=8)&&(fClay<17)&&(fSilt>=50)&&(fSilt<65))
 {  
  lstrcpy(acSoilCode,"Uls");
  iSoilNr=13;
 }  

 if ((fClay>=8)&&(fClay<12)&&(fSilt>=65)&&(fSilt<92))
 {  
  lstrcpy(acSoilCode,"Ul2");
  iSoilNr=14;
 }  

 if ((fClay>=12)&&(fClay<17)&&(fSilt>=65)&&(fSilt<88))
 {  
  lstrcpy(acSoilCode,"Ul3");
  iSoilNr=15;
 }  

 if ((fClay>=17)&&(fClay<30)&&(fSilt>=70)&&(fSilt<83))
 {  
  lstrcpy(acSoilCode,"Ul4");
  iSoilNr=16;
 }  

 if ((fClay>=15)&&(fClay<25)&&(fSilt>=40)&&(fSilt<50))
 {  
  lstrcpy(acSoilCode,"Ls2");
  iSoilNr=20;
 }  

 if (((fClay>=15)&&(fClay<17)&&(fSilt>=35)&&(fSilt<40))
	 ||((fClay>=17)&&(fClay<25)&&(fSilt>=28)&&(fSilt<40)))
 {  
  lstrcpy(acSoilCode,"Ls3");
  iSoilNr=21;
 }  

 if ((fClay>=17)&&(fClay<25)&&(fSilt>=15)&&(fSilt<28))
 {  
  lstrcpy(acSoilCode,"Ls4");
  iSoilNr=22;
 }  

 if ((fClay>=17)&&(fClay<30)&&(fSilt>=50)&&(fSilt<70))
 {  
  lstrcpy(acSoilCode,"Lu");
  iSoilNr=23;
 }  

 if ((fClay>=25)&&(fClay<35)&&(fSilt>=35)&&(fSilt<50))
 {  
  lstrcpy(acSoilCode,"Lt2");
  iSoilNr=24;
 }  

 if ((fClay>=35)&&(fClay<45)&&(fSilt>=30)&&(fSilt<50))
 {  
  lstrcpy(acSoilCode,"Lt3");
  iSoilNr=25;
 }  

 if ((fClay>=30)&&(fClay<45)&&(fSilt>=50)&&(fSilt<70))
 {  
  lstrcpy(acSoilCode,"Ltu");
  iSoilNr=26;
 }  

 if (((fClay>=25)&&(fClay<35)&&(fSilt>=18)&&(fSilt<35))
	 ||((fClay>=35)&&(fClay<45)&&(fSilt>=18)&&(fSilt<30)))
 {  
  lstrcpy(acSoilCode,"Lts");
  iSoilNr=27;
 }  

 if ((fClay>=50)&&(fClay<65)&&(fSilt>=0)&&(fSilt<18))
 {  
  lstrcpy(acSoilCode,"Ts2");
  iSoilNr=28;
 }  

 if ((fClay>=35)&&(fClay<50)&&(fSilt>=0)&&(fSilt<18))
 {  
  lstrcpy(acSoilCode,"Ts3");
  iSoilNr=29;
 }  

 if ((fClay>=25)&&(fClay<35)&&(fSilt>=0)&&(fSilt<18))
 {  
  lstrcpy(acSoilCode,"Ts4");
  iSoilNr=30;
 }  

 if ((fClay>=45)&&(fClay<65)&&(fSilt>=18)&&(fSilt<55))
 {  
  lstrcpy(acSoilCode,"Tl");
  iSoilNr=34;
 }  

 if ((fClay>=65)&&(fClay<=100)&&(fSilt>=0)&&(fSilt<35))
 {  
  lstrcpy(acSoilCode,"T");
  iSoilNr=35;
 }  

 return iSoilNr;
}

//////////////////////////////////////////////////////
// Name:    GPV_FK_PWP(float fClay, float fBulkDens,
//                     float fHumus, float fStone,
//      			   char acSoilCode, int iRet)
//
// Methode: Berechnet anhand der Texturdaten
//          und der Boden-ADV-Kennung
// 
//          das GesamtPorenVolumen GPV
//          die FeldKapazitaet FK
//          den PermanentenWelkePunkt PWP
//
// Author: ep/gsf 250299
/////////////////////////////////////////////////////

float GPV_FK_PWP(float fClay,float fBulkDens,float fHumus, float fStone,
				 char* acSoilCode,int iRet)
{
 float fRet;
 //float fGPV,fFK,fPWP;
 int iRetValue1,iRetValue2,iFK,iFKK;
 int iBD,iClay;
 
 iBD = BD_Class(fBulkDens);
 iRetValue1=CapacityParam1(iBD,acSoilCode,iRet);

 iClay = Clay_Class(fClay);
 iRetValue2=CapacityParam2(iClay,fHumus,iRet);

 if (iRet < 3) fRet=((float)iRetValue1 + (float)iRetValue2)*((float)1 - fStone);
 else 
 if (iRet==3) 
 {
  iFK=CapacityParam1(iBD,acSoilCode,2);
  iFKK=CapacityParam2(iClay,fHumus,2);

  fRet=((float)iFK+(float)iFKK)*((float)1 - fStone)
	    -((float)iFK-(float)iRetValue1+(float)iRetValue2)*((float)1 - fStone);
 }

 /* 
 if      (iRet==1) fRet=fGPV;
 else if (iRet==2) fRet=fFK;
 else if (iRet==3) fRet=fPWP;
*/
 return fRet; 
}


/**************************************************************
 * int BD_Class(float fVal)                                   *
 *                                                            *
 * Methode: Weist anhand der Lagerungsdichte                  *
 *          die Lagerungsdichteklasse LDi zu				  *
 *															  *
 * Author: mb/tum 93, angepasst ep/gsf 250299				  *
 **************************************************************/

int BD_Class(float fVal)
{

 if(fVal > (float)0   && fVal <= (float)0.6)
	return 1;									/* Ld1		*/
 if(fVal > (float)0.6 && fVal <= (float)1.3)
	return 2;                       			/* Ld2		*/
 if(fVal > (float)1.3 && fVal <= (float)1.6)
	return 3;                       			/* Ld3		*/
 if(fVal > (float)1.6 && fVal <= (float)1.8)
	return 4;                       			/* Ld4		*/
 if(fVal > (float)1.8)
	return 5;                       			/* Ld5		*/

 return 0;

} /* End of BD_Class() */

/**************************************************************
* int Clay_Class(float fVal)                                  *
*						                                      *
* Methode: Weist anhand des Tongehalts 						  *
*          die Tongehaltsklasse zu							  *
*															  *
* Author:  mb/tum 93, angepasst ep/gsf 250299      			  *
*															  *
**************************************************************/

int Clay_Class(float fVal)
{
 fVal=fVal*(float)100;

 if(fVal > (float)0   && fVal < (float)5)
	return 1;									// Tongehalt < 5 %
 if(fVal >= (float)5  && fVal < (float)12)
	return 2;                       			// Tongehalt 5-12%
 if(fVal >= (float)12 && fVal < (float)17)
	return 3;                       			// Tongehalt 12-17%
 if(fVal >= (float)17 && fVal < (float)35)
	return 4;                       			// Tongehalt 17-35%
 if(fVal >= (float)35 && fVal < (float)65)
	return 5;                       			// Tongehalt 35-65%
 if(fVal >= (float)65)
	return 6;                       			// Tongehalt > 65%

 return 0;

} /* End of Clay_Class() */

/************************************************************/
/* int CapacityParam2(int		iTon,      					*/
/*					  float		fOs,						*/
/*					  int		iReturnType)				*/
/*															*/
/* Methode: Berechnet die Zu-/Ab-Schlaege fuer  			*/
/*          das GesamtPorenVolumen GPV						*/
/*          die FeldKapazitaet FK							*/
/*          die nutzbare FeldKapazitaet nFK  				*/
/*															*/
/* Author:  mb/tum 93, angepasst ep/gsf 250299 	   			*/
/************************************************************/

int CapacityParam2(int iTon, float fOs, int iReturnType)
{
 int	iReturnVal;

 fOs=fOs*(float)100;
 iReturnVal = 0;

 switch(iTon)
 {
 	case 1:									// Tongehalt < 5 %
		if(fOs >= 2 && fOs < 4)				// 2% OS
		{
			if(iReturnType == 1)
				iReturnVal = 1;				// Zu-/Abschlag Gpv
			if(iReturnType == 2)
				iReturnVal = 1;             // Zu-/Abschlag Fk
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 4 && fOs < 6)				// 4% OS
		{
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 3;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8)				// 6% OS
		{
			if(iReturnType == 1)
				iReturnVal = 5;
			if(iReturnType == 2)
				iReturnVal = 7;
			if(iReturnType == 3)
				iReturnVal = 7;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10)				// 8% OS
		{
			if(iReturnType == 1)
				iReturnVal = 9;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12)			// 10% OS
		{
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 15;
			if(iReturnType == 3)
				iReturnVal = 12;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14)			// 12% OS
		{
			if(iReturnType == 1)
				iReturnVal = 14;
			if(iReturnType == 2)
				iReturnVal = 19;
			if(iReturnType == 3)
				iReturnVal = 14;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14)						// 14% OS
		{
			if(iReturnType == 1)
				iReturnVal = 17;
			if(iReturnType == 2)
				iReturnVal = 23;
			if(iReturnType == 3)
				iReturnVal = 16;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 2:									// Tongehalt 5-12%
		if(fOs >= 2 && fOs < 4)				// 2% OS
		{
			if(iReturnType == 1)
				iReturnVal = 1;
			if(iReturnType == 2)
				iReturnVal = 1;
			if(iReturnType == 3)
				iReturnVal = 1;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 4 && fOs < 6)				// 4% OS
		{
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 3;
			if(iReturnType == 3)
				iReturnVal = 3;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8)				// 6% OS
		{
			if(iReturnType == 1)
				iReturnVal = 4;
			if(iReturnType == 2)
				iReturnVal = 6;
			if(iReturnType == 3)
				iReturnVal = 5;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10)			// 8% OS
		{
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 10;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12)			// 10% OS
		{
			if(iReturnType == 1)
				iReturnVal = 10;
			if(iReturnType == 2)
				iReturnVal = 13;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14)			// 12% OS
		{
			if(iReturnType == 1)
				iReturnVal = 13;
			if(iReturnType == 2)
				iReturnVal = 17;
			if(iReturnType == 3)
				iReturnVal = 13;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14)						// 14% OS
		{
			if(iReturnType == 1)
				iReturnVal = 16;
			if(iReturnType == 2)
				iReturnVal = 21;
			if(iReturnType == 3)
				iReturnVal = 15;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 3:                                 // Tongehalt 12-17%
		if(fOs >= 4 && fOs < 6)				// 4% OS
		{
			if(iReturnType == 1)
				iReturnVal = 1;
			if(iReturnType == 2)
				iReturnVal = 2;
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8)				// 6% OS
		{
			if(iReturnType == 1)
				iReturnVal = 4;
			if(iReturnType == 2)
				iReturnVal = 6;
			if(iReturnType == 3)
				iReturnVal = 5;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10)			// 8% OS
		{
			if(iReturnType == 1)
				iReturnVal = 7;
			if(iReturnType == 2)
				iReturnVal = 9;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12)			// 10% OS
		{
			if(iReturnType == 1)
				iReturnVal = 9;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14)			// 12% OS
		{
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 15;
			if(iReturnType == 3)
				iReturnVal = 12;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14)						// 14% OS
		{
			if(iReturnType == 1)
				iReturnVal = 15;
			if(iReturnType == 2)
				iReturnVal = 20;
			if(iReturnType == 3)
				iReturnVal = 15;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 4:                                 // Tongehalt 17-35%
		if(fOs >= 4 && fOs < 6)				// 4% OS
		{
			if(iReturnType == 1)
				iReturnVal = 0;
			if(iReturnType == 2)
				iReturnVal = 1;
			if(iReturnType == 3)
				iReturnVal = 1;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 6 && fOs < 8)				// 6% OS
		{
			if(iReturnType == 1)
				iReturnVal = 3;
			if(iReturnType == 2)
				iReturnVal = 4;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10)			// 8% OS
		{
			if(iReturnType == 1)
				iReturnVal = 6;
			if(iReturnType == 2)
				iReturnVal = 7;
			if(iReturnType == 3)
				iReturnVal = 6;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12)			// 10% OS
		{
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 9;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14)			// 12% OS
		{
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
				iReturnVal = 10;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14)						// 14% OS
		{
			if(iReturnType == 1)
				iReturnVal = 14;
			if(iReturnType == 2)
				iReturnVal = 15;
			if(iReturnType == 3)
				iReturnVal = 12;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 5:                                 // Tongehalt 35-65%
		if(fOs >= 6 && fOs < 8)				// 6% OS
		{
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 2;
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10)			// 8% OS
		{
			if(iReturnType == 1)
				iReturnVal = 5;
			if(iReturnType == 2)
				iReturnVal = 4;
			if(iReturnType == 3)
				iReturnVal = 3;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12)			// 10% OS
		{
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 5;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14)			// 12% OS
		{
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 7;
			if(iReturnType == 3)
				iReturnVal = 6;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14)						// 14% OS
		{
			if(iReturnType == 1)
				iReturnVal = 13;
			if(iReturnType == 2)
				iReturnVal = 9;
			if(iReturnType == 3)
				iReturnVal = 8;              // Zu-/Abschlag nFk
			break;
		}
		break;

	case 6:                                 // Tongehalt >65%
		if(fOs >= 6 && fOs < 8)				// 6% OS
		{
			if(iReturnType == 1)
				iReturnVal = 2;
			if(iReturnType == 2)
				iReturnVal = 2;
			if(iReturnType == 3)
				iReturnVal = 2;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 8 && fOs < 10)			// 8% OS
		{
			if(iReturnType == 1)
				iReturnVal = 5;
			if(iReturnType == 2)
				iReturnVal = 3;
			if(iReturnType == 3)
				iReturnVal = 3;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 10 && fOs < 12)			// 10% OS
		{
			if(iReturnType == 1)
				iReturnVal = 8;
			if(iReturnType == 2)
				iReturnVal = 4;
			if(iReturnType == 3)
				iReturnVal = 4;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 12 && fOs < 14)			// 12% OS
		{
			if(iReturnType == 1)
				iReturnVal = 11;
			if(iReturnType == 2)
				iReturnVal = 5;
			if(iReturnType == 3)
				iReturnVal = 5;              // Zu-/Abschlag nFk
			break;
		}
		if(fOs >= 14)						// 14% OS
		{
			if(iReturnType == 1)
				iReturnVal = 13;
			if(iReturnType == 2)
				iReturnVal = 6;
			if(iReturnType == 3)
				iReturnVal = 6;              // Zu-/Abschlag nFk
			break;
		}
		break;
 }

 return iReturnVal;

} /* End of CapacityParam2() */


/****************************************************************
* int CapacityParam1(int iLd, char acSoilCode, int iReturnType) *
*															    *
* Methode: Weist anhand der BodenKennung und Lagerungsdichte	*
*          noch zu korrigierende Werte zu fuer					*
*          														*
*          das GesamtPorenVolumen GPV							*
*          die FeldKapazitaet FK								*
*          den PermanentenWelkePunkt PWP						*
*																*
* Author: mb/tum 93, angepasst ep/gsf 250299				    *
*																*
****************************************************************/

int CapacityParam1(int iLd, char* acSoilCode, int iReturnType)
{
 int	iReturnVal;
 int	iNfk;
 int	iFk;
 int    iSwitch;
// char acSoilCode[6];

 iReturnVal = 0;

 if      (iLd < 3) iSwitch=1;
 else if (iLd = 3) iSwitch=2;
 else if (iLd > 3) iSwitch=3;

 switch(iSwitch)
 {
 	case 1:									// Ld1 & Ld2
		if (!lstrcmp(acSoilCode,"S\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 14;
			if(iReturnType == 3)
			{
				iFk  = 14;
				iNfk = 10;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Su\0"))|| //Su ep121207
			(!lstrcmp(acSoilCode,"Su2\0"))||
			(!lstrcmp(acSoilCode,"Su3\0"))||
			(!lstrcmp(acSoilCode,"Su4\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3)
			{
				iFk  = 31;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
		
		if (!lstrcmp(acSoilCode,"Sl2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3)
			{
				iFk  = 27;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
		
		if (!lstrcmp(acSoilCode,"Slu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 49;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3)
			{
				iFk  = 34;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
	
		if ((!lstrcmp(acSoilCode,"Sl3\0"))||
			(!lstrcmp(acSoilCode,"Sl\0")))//Sl ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 51;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3)
			{
				iFk  = 32;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Sl4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3)
			{
				iFk  = 34;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"St2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 29;
			if(iReturnType == 3)
			{
				iFk  = 29;
				iNfk = 18;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"St3\0"))||
			(!lstrcmp(acSoilCode,"St\0")))//St ep121207
		{
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3)
			{
				iFk  = 32;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"U\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 51;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 28;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Us\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 35;
			if(iReturnType == 3)
			{
				iFk  = 35;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
			
		if (!lstrcmp(acSoilCode,"Ul2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3)
			{
				iFk  = 38;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
			
		if ((!lstrcmp(acSoilCode,"Ul\0"))|| //Ul ep121207
			(!lstrcmp(acSoilCode,"Ul3\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3)
			{
				iFk  = 40;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ul4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3)
			{
				iFk  = 40;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Uls\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ut2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3)
			{
				iFk  = 38;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ut\0"))|| //Ut ep121207
		    (!lstrcmp(acSoilCode,"Ut3\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3)
			{
				iFk  = 40;
				iNfk = 27;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ut4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3)
			{
				iFk  = 40;
				iNfk = 26;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ls2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3)
			{
				iFk  = 38;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ls3\0"))||
            (!lstrcmp(acSoilCode,"Ls\0")))//Ls ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3)
			{
				iFk  = 38;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ls4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 52;
			if(iReturnType == 2)
				iReturnVal = 40;
			if(iReturnType == 3)
			{
				iFk  = 40;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lt2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 56;
			if(iReturnType == 2)
				iReturnVal = 46;
			if(iReturnType == 3)
			{
				iFk  = 46;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Lt3\0"))||
        (!lstrcmp(acSoilCode,"Lt\0")))//Lt ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 56;
			if(iReturnType == 2)
				iReturnVal = 46;
			if(iReturnType == 3)
			{
				iFk  = 46;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ltu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 57;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3)
			{
				iFk  = 47;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lts\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 58;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3)
			{
				iFk  = 47;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		/* Ts2, Ts3, Ts4 nicht in Tab. 43, AG Bodenkunde 1982 enthalten ! */
		/* Werte übernommen von Lts ! */
		if ((!lstrcmp(acSoilCode,"Ts\0"))|| //Ts ep121207
			(!lstrcmp(acSoilCode,"Ts2\0"))||
			(!lstrcmp(acSoilCode,"Ts3\0"))||
			(!lstrcmp(acSoilCode,"Ts4\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 58;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3)
			{
				iFk  = 47;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if (!lstrcmp(acSoilCode,"Tu4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 57;
			if(iReturnType == 2)
				iReturnVal = 47;
			if(iReturnType == 3)
			{
				iFk  = 47;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
		

		if ((!lstrcmp(acSoilCode,"Tu\0"))|| //Tu ep121207
			(!lstrcmp(acSoilCode,"Tu3\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 56;
			if(iReturnType == 2)
				iReturnVal = 46;
			if(iReturnType == 3)
			{
				iFk  = 46;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Tu2\0"))||   
				(!lstrcmp(acSoilCode,"Tl\0")))			
		{
			if(iReturnType == 1)
				iReturnVal = 63;
			if(iReturnType == 2)
				iReturnVal = 55;
			if(iReturnType == 3)
			{
				iFk  = 55;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"T\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 66;
			if(iReturnType == 2)
				iReturnVal = 59;
			if(iReturnType == 3)
			{
				iFk  = 59;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
		break;



 	case 2:                                 // Ld3

		if (!lstrcmp(acSoilCode,"S\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 36;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
			{
				iFk  = 12;
				iNfk = 9;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Su\0"))|| //Su ep121207
            (!lstrcmp(acSoilCode,"Su2\0"))||
			(!lstrcmp(acSoilCode,"Su3\0"))||
			(!lstrcmp(acSoilCode,"Su4\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 24;
			if(iReturnType == 3)
			{
				iFk  = 24;
				iNfk = 18;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Sl2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 22;
			if(iReturnType == 3)
			{
				iFk  = 22;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Slu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 30;
			if(iReturnType == 3)
			{
				iFk  = 30;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Sl3\0"))||
			(!lstrcmp(acSoilCode,"Sl\0")))//Sl ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3)
			{
				iFk  = 27;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Sl4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 28;
			if(iReturnType == 3)
			{
				iFk  = 28;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"St2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 22;
			if(iReturnType == 3)
			{
				iFk  = 22;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"St3\0"))||
			(!lstrcmp(acSoilCode,"St\0")))//St ep121207
		{
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3)
			{
				iFk  = 27;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"U\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3)
			{
				iFk  = 34;
				iNfk = 25;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Us\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3)
			{
				iFk  = 33;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ul2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3)
			{
				iFk  = 36;
				iNfk = 25;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Uls\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3)
			{
				iFk  = 33;
				iNfk = 22;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ul\0"))|| //Ul ep121207
			(!lstrcmp(acSoilCode,"Ul3\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ul4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 45;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ut2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3)
			{
				iFk  = 36;
				iNfk = 25;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ut\0"))|| //Ut ep121207
			(!lstrcmp(acSoilCode,"Ut3\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 44;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 24;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ut4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 45;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ls2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3)
			{
				iFk  = 33;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ls3\0"))||
            (!lstrcmp(acSoilCode,"Ls\0")))//Ls ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3)
			{
				iFk  = 33;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ls4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 43;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3)
			{
				iFk  = 32;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 45;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3)
			{
				iFk  = 36;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lt2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3)
			{
				iFk  = 41;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Lt3\0"))||
            (!lstrcmp(acSoilCode,"Lt\0")))//Lt ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3)
			{
				iFk  = 41;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ltu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 49;
			if(iReturnType == 2)
				iReturnVal = 42;
			if(iReturnType == 3)
			{
				iFk  = 42;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lts\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3)
			{
				iFk  = 41;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		/* Ts2, Ts3, Ts4 nicht in Tab. 43, AG Bodenkunde 1982 enthalten ! */
		/* Werte übernommen von Lts ! */
		if ((!lstrcmp(acSoilCode,"Ts\0"))|| //Ts ep121207
			(!lstrcmp(acSoilCode,"Ts2\0"))||
			(!lstrcmp(acSoilCode,"Ts3\0"))||
			(!lstrcmp(acSoilCode,"Ts4\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3)
			{
				iFk  = 41;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if (!lstrcmp(acSoilCode,"Tu4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 49;
			if(iReturnType == 2)
				iReturnVal = 42;
			if(iReturnType == 3)
			{
				iFk  = 42;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Tu\0"))|| //Tu ep121207
			(!lstrcmp(acSoilCode,"Tu3\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 48;
			if(iReturnType == 2)
				iReturnVal = 41;
			if(iReturnType == 3)
			{
				iFk  = 41;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Tu2\0"))||   
				(!lstrcmp(acSoilCode,"Tl\0")))			
		{
			if(iReturnType == 1)
				iReturnVal = 53;
			if(iReturnType == 2)
				iReturnVal = 49;
			if(iReturnType == 3)
			{
				iFk  = 49;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"T\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 58;
			if(iReturnType == 2)
				iReturnVal = 54;
			if(iReturnType == 3)
			{
				iFk  = 54;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
	 	break;

 
	case 3:                                 // Ld4 & Ld5

		if (!lstrcmp(acSoilCode,"S\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 31;
			if(iReturnType == 2)
				iReturnVal = 12;
			if(iReturnType == 3)
			{
				iFk  = 12;
				iNfk = 9;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if ((!lstrcmp(acSoilCode,"Su\0"))|| //Su ep121207
			(!lstrcmp(acSoilCode,"Su2\0"))||
			(!lstrcmp(acSoilCode,"Su3\0"))||
			(!lstrcmp(acSoilCode,"Su4\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 33;
			if(iReturnType == 2)
				iReturnVal = 24;
			if(iReturnType == 3)
			{
				iFk  = 24;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Sl2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 32;
			if(iReturnType == 2)
				iReturnVal = 21;
			if(iReturnType == 3)
			{
				iFk  = 21;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Slu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3)
			{
				iFk  = 27;
				iNfk = 17;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Sl3\0"))||
			(!lstrcmp(acSoilCode,"Sl\0")))//Sl ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 26;
			if(iReturnType == 3)
			{
				iFk  = 26;
				iNfk = 15;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Sl4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 27;
			if(iReturnType == 3)
			{
				iFk  = 27;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"St2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 33;
			if(iReturnType == 2)
				iReturnVal = 20;
			if(iReturnType == 3)
			{
				iFk  = 20;
				iNfk = 13;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"St3\0"))||
			(!lstrcmp(acSoilCode,"St\0")))//St ep121207
		{
			if(iReturnType == 1)
				iReturnVal = 30;
			if(iReturnType == 2)
				iReturnVal = 22;
			if(iReturnType == 3)
			{
				iFk  = 22;
				iNfk = 13;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"U\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3)
			{
				iFk  = 31;
				iNfk = 23;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Us\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 36;
			if(iReturnType == 2)
				iReturnVal = 29;
			if(iReturnType == 3)
			{
				iFk  = 29;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ul2\0"))||
			(!lstrcmp(acSoilCode,"Ut2\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 32;
			if(iReturnType == 3)
			{
				iFk  = 32;
				iNfk = 21;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Uls\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 35;
			if(iReturnType == 2)
				iReturnVal = 30;
			if(iReturnType == 3)
			{
				iFk  = 30;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ul\0"))|| //Ul ep121207
			(!lstrcmp(acSoilCode,"Ul3\0"))||
			(!lstrcmp(acSoilCode,"Ut\0"))|| //Ut ep121207
			(!lstrcmp(acSoilCode,"Ut3\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 39;
			if(iReturnType == 2)
				iReturnVal = 34;
			if(iReturnType == 3)
			{
				iFk  = 34;
				iNfk = 20;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Ul4\0"))||
			(!lstrcmp(acSoilCode,"Ut4\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 35;
			if(iReturnType == 3)
			{
				iFk  = 35;
				iNfk = 19;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ls2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 36;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3)
			{
				iFk  = 31;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
			
		if ((!lstrcmp(acSoilCode,"Ls3\0"))||
            (!lstrcmp(acSoilCode,"Ls\0")))//Ls ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 31;
			if(iReturnType == 3)
			{
				iFk  = 31;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ls4\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 37;
			if(iReturnType == 2)
				iReturnVal = 30;
			if(iReturnType == 3)
			{
				iFk  = 30;
				iNfk = 14;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 38;
			if(iReturnType == 2)
				iReturnVal = 33;
			if(iReturnType == 3)
			{
				iFk  = 33;
				iNfk = 16;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lt2\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3)
			{
				iFk  = 36;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Lt3\0"))||
            (!lstrcmp(acSoilCode,"Lt\0")))//Lt ep 121207
		{
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3)
			{
				iFk  = 36;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Ltu\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3)
			{
				iFk  = 38;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"Lts\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		/* Ts2, Ts3, Ts4 nicht in Tab. 43, AG Bodenkunde 1982 enthalten ! */
		/* Werte übernommen von Lts ! */
		if ((!lstrcmp(acSoilCode,"Ts\0"))|| //Ts ep121207
			(!lstrcmp(acSoilCode,"Ts2\0"))||
			(!lstrcmp(acSoilCode,"Ts3\0"))||
			(!lstrcmp(acSoilCode,"Ts4\0")))
		{
			if(iReturnType == 1)
				iReturnVal = 41;
			if(iReturnType == 2)
				iReturnVal = 37;
			if(iReturnType == 3)
			{
				iFk  = 37;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}


		if (!lstrcmp(acSoilCode,"Tu4\0"))   
		{
			if(iReturnType == 1)
				iReturnVal = 42;
			if(iReturnType == 2)
				iReturnVal = 38;
			if(iReturnType == 3)
			{
				iFk  = 38;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if ((!lstrcmp(acSoilCode,"Tu\0"))|| //Tu ep121207
			(!lstrcmp(acSoilCode,"Tu3\0")))   
		{
			if(iReturnType == 1)
				iReturnVal = 40;
			if(iReturnType == 2)
				iReturnVal = 36;
			if(iReturnType == 3)
			{
				iFk  = 36;
				iNfk = 12;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
			
		if ((!lstrcmp(acSoilCode,"Tu2\0"))||   
				(!lstrcmp(acSoilCode,"Tl\0")))			
		{
			if(iReturnType == 1)
				iReturnVal = 47;
			if(iReturnType == 2)
				iReturnVal = 45;
			if(iReturnType == 3)
			{
				iFk  = 45;
				iNfk = 11;
				iReturnVal = iFk - iNfk;
			}
			break;
		}

		if (!lstrcmp(acSoilCode,"T\0"))
		{
			if(iReturnType == 1)
				iReturnVal = 50;
			if(iReturnType == 2)
				iReturnVal = 49;
			if(iReturnType == 3)
			{
				iFk  = 49;
				iNfk = 11;
				iReturnVal = iFk - iNfk;
			}
			break;
		}
	 	break;


 }

 return iReturnVal;
} /* End of CapacityParam1() */


/*******************************************************************************
** EOF */