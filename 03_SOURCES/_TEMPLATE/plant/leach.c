/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:  cs/age, ch/gsf, ep/gsf, sa/gsf
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Leaf/Root Growth and Water-/N-Uptake following LEACHN.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: leach.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:41
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/plant
 * Interface of "DateToDays" changed.
 * 
 *  03.03.94 (PflGrw01.c)
 *  24.04.95
 *  27.8.95 (Termine von lpp!)
 *  17.07.97, 19.02.98, 20.05.99, 02.08.00
 *
*******************************************************************************/

#include "xinclexp.h"
#include "xlanguag.h"
#include "expbasic.h"


// #ifndef GROWING_TIME

// this define is used by the interpolating plant model
#define   GROWING_TIME ((pPM != NULL) && (pPM->pNext != NULL)\
                     && ((int)(SimTime) >= pPM->iDay) && ((int)(SimTime) <= pPM->pNext->iDay)\
                     &&  (pPM->pNext->fDevStage > pPM->fDevStage))
// #endif


#define  TASSELING_REDUCTION 0

//#define MIN_ROOT_POT (float)-150000  
//SG 20111111: -300000 wie in Expert-N Dokumentation
#define MIN_ROOT_POT (float)-300000    //StefanAchatz      
#define TOLERANCE (float)0.0001      

#undef  PLA_WAT_DEBUG


float afConstRootDens[MAXSCHICHT];
int   iIsConstRootDens;
int   iCropCover;

/* from util_fct.c */
extern BOOL NewDay(PTIME);
extern float abspower(float x, float y);
extern int   SimStart(PTIME);
extern int   WINAPI Test_Range(float x, float x1, float x2, LPSTR c);
extern int   WINAPI Message(long, LPSTR);
extern float fPotTraDay;

// Funktionsdeklaration
int WINAPI InitiateLeachModel(EXP_POINTER);

int WINAPI GrowMaizeLeach(EXP_POINTER);
int WINAPI LeafMaizeLeach(EXP_POINTER);
int WINAPI RootMaizeLeach(EXP_POINTER);

float WINAPI LAI2CropCover(float fLAI);

int WINAPI  PotentialTranspiration(EXP_POINTER);
int WINAPI  WaterUptakeNimahHanks(EXP_POINTER);
int WINAPI  WaterUptakeFeddes(EXP_POINTER);
int WINAPI  WaterUptakeVanGenuchten(EXP_POINTER);

int WINAPI  NUptake(EXP_POINTER);


/**********************************************************************************************/
/*                                                                                            */
/*  Name     : InitiateLeachModel()                                                           */
/*                                                                                            */
/*                                                                                            */
/*  Author   : ch/gsf                                                                         */
/*  Date     : 14.07.97                                                                       */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pLR->fLengthDens                                                      */
/*						pPC->fLAI                                                             */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI InitiateLeachModel(EXP_POINTER)
{
        char  acdummy[80];
        LPSTR lpOut       = acdummy;
 
  PPLTMEASURE    pPM;

  PMODELPARAM  pMP = pPl->pModelParam; 

  float   fMinimalRootDepth = (float)50;
  // Dieser Minimalwert ermöglicht Aufnahme in anderen Modulen.
  
  static int iAlreadyInitialised = 0;
 
    if (pPl == NULL) 
   	{
   	return 1;
   	}

   if (pPl->pPMeasure == NULL) 
   	{
   	return 1;
   	}
	
  if (!iAlreadyInitialised)
  for (pPM = pPl->pPMeasure; pPM != NULL; pPM = pPM->pNext)
     {   

     if (pPM->fAboveBiomass < EPSILON)
     {        
     
     pPM->fAboveBiomass = (pPM->fStemWeight + pPM->fLeafWeight  + pPM->fGrainWeight > EPSILON)?
     						pPM->fStemWeight + pPM->fLeafWeight  + pPM->fGrainWeight
     						:
     						(float)0;     
     }  
     
   iAlreadyInitialised = 1;
     
     } 
   
  for (pPM = pPl->pPMeasure;
  		(pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= pTi->pSimTime->fTimeAct);
  		pPM = pPM->pNext);

       Message(1, COMMENT_GROWING_START); 

       pPl->pDevelop->fDevStage = pPM->pNext->fDevStage;  
       pPl->pModelParam->iEmergenceDay = pPM->iDay;

	  if (pPM->fRootDepth < fMinimalRootDepth)
	  	{                      
	  	pPM->fRootDepth = fMinimalRootDepth;
	  	}                        


  while ((pPM->pNext != NULL) && (pPM->pNext->fDevStage > pPM->fDevStage))
     {
     pPM = pPM->pNext;
     } 
       pPl->pModelParam->iHarvestDay = pPM->iDay;
  	   
  	   pPl->pRoot->fMaxDepth = pPM->fRootDepth;
       
		Message(0, "potential plant values:");
		itoa((int)(pPM->fRootDepth), lpOut, 10);
		strcat((LPSTR)lpOut," mm max. root depth");
		Message(0, lpOut);

		itoa((int)(pPM->fTotalN), lpOut, 10);
		strcat((LPSTR)lpOut," kg N demand");
		Message(0, lpOut);

		itoa((int)(pPM->fLAI), lpOut, 10);
		strcat((LPSTR)lpOut," LAI");
		Message(0, lpOut);

return 1;
}       

/**********************************************************************************************/
/*                                                                                            */
/*  Name     : GrowMaizeLeach()                                                               */
/*                                                                                            */
/*                                                                                            */
/*  Autor    : gsf/ch     26.4.95                                                             */
/*             nach  Ch. Sperr                                                                */
/*  Datum    : 13.07.97                                                                       */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pLR->fLengthDens                                                      */
/*						pPC->fLAI                                                             */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI GrowMaizeLeach(EXP_POINTER)
{
LeafMaizeLeach(exp_p);
RootMaizeLeach(exp_p);

return 1;
}

/**********************************************************************************************/
/*                                                                                            */
/*  Name     : RootMaizeLeach()                                                               */
/*                                                                                            */
/*                                                                                            */
/*  Autor    : gsf/ch     26.4.95                                                             */
/*             nach  Ch. Sperr                                                                */
/*  Datum    : 13.07.97                                                                       */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pLR->fLengthDens                                                      */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI RootMaizeLeach(EXP_POINTER) //CH
{
 DECLARE_COMMON_VAR
 
 // Änderung in der neuen Variablenstruktur: Zugriff auf die Hauptfrucht
 // pPl-> wird hochgzählt zu pPl = pPl->pNext->pNext->;
 PPLTMEASURE    pPM = pPl->pPMeasure;
 PCANOPY        pPC = pPl->pCanopy;
 PLAYERROOT     pLR = pPl->pRoot->pLayerRoot;

  float    fTimeConst;             // TIM normierte Zeit seit Pflanzung                                  // t=MaxWurzelTiefe+1  => TIM = 113
  float    fTimeBreak;             // Zeitpunkt des Umschaltens der Wurzelverteilung  
  float    HalbWert;               // [mm] Tiefe, in der WL-Dichte 1/2 des Max.Werts
  float    maxWuLae;               // [m/m] Rmax max. Wurzellängendichte an Boden-Oberfläche 
                                   // !! LeachM: [m Wurzel/mm Tiefe] ExpertN: [cm / cm^3] !!
  float    fPlantDensity;          // Konstant gewählt  [1/m^2]                               
  float    Tiefe;                  // [mm]
  float    fFracGrowS;             // fraction of growing season
  float    fTotalTime;             // Gesamtdauer des Wachstums, bzw. zwischen zwei Messungen
  float    f1;                                 

  if(iIsConstRootDens)
  {
//	  if(SimStart(pTi))
		  for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
			  pLR->fLengthDens = afConstRootDens[iLayer];
  }
  else
  {

  fPlantDensity = (float)100;                                                             

 
  if (NewDay(pTi))
  {
  /*
  // Pflanzenmesswert eventuell in Systemsteuerug übernehmen  
  while ((pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime))
     {
     pPM = pPM->pNext;
     } 
  */
  
  
  if(GROWING_TIME)
  {
   
  pPl->pDevelop->iDayAftEmerg       = (int)SimTime  - pPl->pModelParam->iEmergenceDay;
  
  /* here MaxAge is the time between two measurements */
  fTotalTime    = max((float)(pPM->pNext->iDay  - pPM->iDay + (float)1), (float)1);
  
  /* fFracGrowS is between 0 and 1 */
  fFracGrowS = (SimTime  - pPM->iDay)/fTotalTime;
 
if ((pPl->pRoot->fDepth < pPM->pNext->fRootDepth - EPSILON)||(pPl->pRoot->fDepth > pPM->pNext->fRootDepth + EPSILON))
{   
  for (SOIL_LAYERS0(pLR,pPl->pRoot->pLayerRoot))
       pLR->fLengthDens    = (float) 0.0;

  /* here MaxAge is the time between Harvest and Emergence */
  fTotalTime    = max(pPl->pModelParam->iHarvestDay - pPl->pModelParam->iEmergenceDay, (float)1);

  fTimeConst  =  (float)113.0 * (float)pPl->pDevelop->iDayAftEmerg / fTotalTime; 
  
  fTimeBreak  = fTotalTime *(float)24/(float)113;

  // *********************************************************************
  //                Berechne Dynamik der max. Wurzeltiefe wie Entwicklung des Leaf Area Index

    f1 = (float)6.0 - ((float)12.0 * fFracGrowS);

    /* Lasse als Anfangswert auch Werte > 0 zu.*/    
    pPl->pRoot->fDepth = pPM->fRootDepth + (pPM->pNext->fRootDepth - pPM->fRootDepth) / ((float)1.0 + (float)exp(f1));



  maxWuLae = max((float)100, - (float)0.05253 * fTimeConst * fTimeConst      // [1] [mm Wurzel an der Bodenoberfläche /mm Tiefe ]
                              + (float)24.6667 * fTimeConst
                              - (float)120.0);
 
 /* 
 Dynamik nach Mais-Modell Leach:

  if (fTimeConst <= fTimeBreak)     // "stetig" fuer fTimeBreak ~ 6 (MaxWT ~ 15)
     {
      f1 = max((float)0, (float)10  / (float)1.7 * // [mm]
                          ((float)0.06 * fTimeConst * fTimeConst
                         - (float)0.1  * fTimeConst));      

      pPl->pRoot->fDepth += f1 * (pPM->pNext->fRootDepth - pPM->fRootDepth);
      
     }
  
     else
     {
      f1 = max((float)0;  (float)10  / (float)1.7 * // [mm]
                          ((float)-0.0112 * fTimeConst * fTimeConst
                         + (float)2.9     * fTimeConst
                         - (float)15.0);
	  
	  pPl->pRoot->fDepth += f1* (pPM->pNext->fRootDepth - pPM->fRootDepth);
	  }                            
 */

     HalbWert  = (float)((double)pPl->pRoot->fDepth
                     * (((double)-.0001854 * (double)fTimeConst * (double)fTimeConst)
                      + ((double).022 * (double)fTimeConst)
                       - (double).102));

     HalbWert  = (float)max(HalbWert,((float)0.4* pPl->pRoot->fDepth));
     HalbWert  = (float)min(HalbWert,(pPl->pRoot->fDepth));


     f1 = (float)((double)PI * (double)HalbWert /((double)2.0 * (double)pPl->pRoot->fDepth));
     f1 = (float)((double)2.0 * cos((double)f1));
     f1 = (float)(log((double)f1)/(HalbWert * HalbWert)); //[1/mm^2]


     for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
     {        
     Tiefe = (float)((float)iLayer- (float)0.5) * pSo->fDeltaZ;  // [mm]
  
     if (Tiefe - (float)0.5*pSo->fDeltaZ < pPl->pRoot->fDepth)
      {
        pLR->fLengthDens = (float)exp((double)(-1.0 * f1 * Tiefe * Tiefe));    //[1]
        pLR->fLengthDens *= (float)cos((double)((PI * Tiefe)/(2.0 * pPl->pRoot->fDepth))); //[1]
        pLR->fLengthDens *= maxWuLae;   //!!  [1]
        pLR->fLengthDens *= (float).0001  * fPlantDensity;
                                     // 0.0001 *[1] *[1/m^2] = [1/cm^2]
      }
   
     else
       iLayer = pSo->iLayers; // Abbruch von for()
   
     } // for  .....
  }  //   Zeit   kleiner   Wurzelreifezeit  ( t < MaxWurzelTermin)
  } // Growing Time && TIME_DURING_PLANT_MEASUREMENTS
  else
  {
  if(pPl->pDevelop->fDevStage)
  {
  pPl->pDevelop->fDevStage=(float)0;
    Message(1, COMMENT_GROWING_END); 
  }

  if(pPl->pRoot->pLayerRoot->pNext->fLengthDens > EPSILON)
   for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
     {        
        pLR->fLengthDens = (float)0;
     }   
  }          


  } //NewDay

  
}
 return OK;
} //  ende   RootMaizeLeach    


/**********************************************************************************************/
/*                                                                                            */
/*  Name     : RootMaizeLeach()                                                               */
/*                                                                                            */
/*                                                                                            */
/*  Autor    : ep/gsf 1.8.00   nach  ch/gsf 13.7.97 & 26.4.95 und cs/ibs                      */
/*                                                                                            */
/*  Datum    : 01.08.00                                                                       */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pLR->fLengthDens                                                      */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI RootMaizeLeachEP(EXP_POINTER) //EP
{
 DECLARE_COMMON_VAR
 
 // Änderung in der neuen Variablenstruktur: Zugriff auf die Hauptfrucht
 // pPl-> wird hochgzählt zu pPl = pPl->pNext->pNext->;
 PPLTMEASURE    pPM = pPl->pPMeasure;
 PCANOPY        pPC = pPl->pCanopy;
 PLAYERROOT     pLR = pPl->pRoot->pLayerRoot;

  float    fTimeConst;             // normalized time since planting  
  float    fTimeBreak;             // time point of switching root depth calculation  
  float    fHalfDepth;             // [mm] depth at which root length density = 1/2 of fMaxRLD
  float    fMaxRLD;                // [m/m] max. root length density at soil surface 
                                   // !! LeachM: [m Wurzel/mm Tiefe] ExpertN: [cm / cm^3] !!
  float    fPlantDensity;          // constant value chosen  [1/m^2]                               
  float    fDepth;                 // [mm]
  float    fFracGrowS;             // fraction of growing season
  float    fMaxAge;                // total duration of growth
  float    f1;                                 
  int      iMaxRootDay;	           //day number of root maturity (i.e. max. root length density)
  int	   iSowDay;				   //day number of sowing resp. planting

  if(iIsConstRootDens)
  {
//	  if(SimStart(pTi))
		  for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
			  pLR->fLengthDens = afConstRootDens[iLayer];
  }
  else
  {

  if(NewDay(pTi))
  {
   if(((int)SimTime>=pPl->pModelParam->iEmergenceDay)&&
	 ((int)SimTime<=pPl->pModelParam->iHarvestDay))
  {
  fPlantDensity = (float)100;                                                             
  pPl->pDevelop->fDevStage=(float)1;
  pPl->pDevelop->iDayAftEmerg = (int)SimTime  - pPl->pModelParam->iEmergenceDay;
  iMaxRootDay=(int)DateToDays(pTi->pSimTime->lStartDate,pPl->pModelParam->lMaxRootBiomassDate);
  iSowDay=(int)DateToDays(pTi->pSimTime->lStartDate,pMa->pSowInfo->lSowDate);
  
  /* here MaxAge is the time between Harvest and Emergence */
  fMaxAge = max(pPl->pModelParam->iHarvestDay - pPl->pModelParam->iEmergenceDay, (float)1);
  
  /* fFracGrowS is between 0 and 1 */
  fFracGrowS = (SimTime  - pPl->pModelParam->iEmergenceDay)/fMaxAge;
 
//if ((pPl->pRoot->fDepth < pPM->pNext->fRootDepth - EPSILON)||(pPl->pRoot->fDepth > pPM->pNext->fRootDepth + EPSILON))
{   
  for (SOIL_LAYERS0(pLR,pPl->pRoot->pLayerRoot))
       pLR->fLengthDens    = (float) 0.0;


  //fTimeConst  =  (float)113.0 * (float)pPl->pDevelop->iDayAftEmerg / fMaxAge; 
  //fTimeBreak  = fMaxAge * (float)24/(float)113;

  fTimeConst  =  (float)113.0 * (float)(min(iMaxRootDay,(int)SimTime)-iSowDay)
	             /(float)(iMaxRootDay-iSowDay+(int)1);
  fTimeBreak  = (float)24/(float)113*(float)(iMaxRootDay-iSowDay+(int)1);

    /* Assume planting depth 50 mm */    
  pPl->pRoot->fDepth = (float)50;

  fMaxRLD = max((float)100, - (float)0.05253 * fTimeConst * fTimeConst // [1] [mm Wurzel an der Bodenoberfläche /mm Tiefe ]
                              + (float)24.6667 * fTimeConst
                              - (float)120.0);
 
 
  //corn root growth following LEACHN (Davidson et al. 1978 in EPA Publication 600/3-78-029)

  if (fTimeConst <= fTimeBreak)     // "stetig" fuer fTimeBreak ~ 6 (MaxWT ~ 15)
     {
      f1 = max((float)0, (float)10  / (float)1.7 * // [mm]
                          ((float)0.06 * fTimeConst * fTimeConst
                         - (float)0.1  * fTimeConst));      

      pPl->pRoot->fDepth += f1;
      
     }
  
     else
     {
      f1 = max((float)0,  (float)10  / (float)1.7 * // [mm]
                          ((float)-0.0112 * fTimeConst * fTimeConst
                         + (float)2.9     * fTimeConst
                         - (float)15.0));
	  
	  pPl->pRoot->fDepth += f1;
	  }                            


     fHalfDepth  = (float)((double)pPl->pRoot->fDepth
                   * (((double)-.0001854 * (double)fTimeConst * (double)fTimeConst)
                    + ((double).022 * (double)fTimeConst)
                    - (double).102));
				    
     fHalfDepth  = (float)max(fHalfDepth,((float)0.4 * pPl->pRoot->fDepth));
     fHalfDepth  = (float)min(fHalfDepth,(pPl->pRoot->fDepth));


     f1 = (float)((double)PI * (double)fHalfDepth /((double)2.0 * (double)pPl->pRoot->fDepth));
     f1 = (float)((double)2.0 * cos((double)f1));
     f1 = (float)(log((double)f1)/(fHalfDepth * fHalfDepth)); //[1/mm^2]


     for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
     {        
     fDepth = (float)((float)iLayer- (float)0.5) * pSo->fDeltaZ;  // [mm]
  
     if (fDepth - (float)0.5 *pSo->fDeltaZ < pPl->pRoot->fDepth)
      {
        pLR->fLengthDens = (float)exp((double)(-1.0 * f1 * fDepth * fDepth));    //[1]
        pLR->fLengthDens *= (float)cos((double)((PI * fDepth)/(2.0 * pPl->pRoot->fDepth))); //[1]
        pLR->fLengthDens *= fMaxRLD;   //!!  [1]
        pLR->fLengthDens *= (float).0001  * fPlantDensity;
                                     // 0.0001 *[1] *[1/m^2] = [1/cm^2]
      }
   
     else
       iLayer = pSo->iLayers; // Abbruch von for()
   
     } // end for  .....
   } // end if (t < iMaxRootDay)
  } // end if plant is growing  
  else
  {
  if(pPl->pDevelop->fDevStage)
  {
  pPl->pDevelop->fDevStage=(float)0;
    Message(1, COMMENT_GROWING_END); 
  }

  if(pPl->pRoot->pLayerRoot->fLengthDens > EPSILON)
   for (SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
     {        
        pLR->fLengthDens = (float)0;
     }   
  }// else          
  
}// new day

} //root density not constant
 return OK;
} //  end   RootMaizeLeach()  



/**********************************************************************************************/
/*                                                                                            */
/*  Name     : LeafMaizeLeach()                                                               */
/*                                                                                            */
/*                                                                                            */
/*  Autor    : gsf/ch     26.4.95                                                             */
/*             nach  Ch. Sperr                                                                */
/*  Datum    : 13.07.97                                                                       */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pLR->fLengthDens                                                      */
/*						pPC->fLAI                                                             */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI LeafMaizeLeach(EXP_POINTER) //CH
{
 DECLARE_COMMON_VAR
 
 // Änderung in der neuen Variablenstruktur: Zugriff auf die Hauptfrucht
 // pPl-> wird hochgzählt zu pPl = pPl->pNext->pNext->;
 PPLTMEASURE    pPM = pPl->pPMeasure;
 PCANOPY        pPC = pPl->pCanopy;

  float    fFracGrowS;             // fraction of growing season
  float    fTotalTime;             // Gesamtdauer des Wachstums
  float    f1;                                 
  


  if(iIsConstRootDens)
  {
    /*
	if((pPM->pNext != NULL) && (pPC->fLAI == (float)0))
	   {
		fTotalTime = max((float)(pPM->pNext->iDay  - pPM->iDay + (float)1), (float)1);
		pPC->fLAI  = pPM->fLAI+(pPM->pNext->fLAI-pPM->fLAI)*
			                                      ((int)SimTime-pPM->iDay)/fTotalTime;
	   }


	if((pPM->pNext != NULL) && (pPC->fCropCoverFrac == (float)0))
	   {
		fTotalTime          = max((float)(pPM->pNext->iDay  - pPM->iDay + (float)1), (float)1);
		pPC->fCropCoverFrac = pPM->fCropCoverFrac+(pPM->pNext->fCropCoverFrac-pPM->fCropCoverFrac)*
			                                      ((int)SimTime-pPM->iDay)/fTotalTime;
		//pPC->fCropCoverFrac = min((float)1,pPM->fCropCoverFrac);
	   }
    */
    ///*
    if (NewDay(pTi)) //now next pPM assigned in time.c //ep 250907
    {
    //while ((pPM->pNext->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime)) //ep 250907
       {
        //pPM = pPM->pNext; //ep 250907
	    //pPC->fLAI = pPM->fLAI;
		fTotalTime    = max((float)(pPM->pNext->iDay  - pPM->iDay + (float)1), (float)1);
		pPC->fLAI  = pPM->fLAI+(pPM->pNext->fLAI-pPM->fLAI)*
			                                      ((int)SimTime-pPM->iDay)/fTotalTime;
		if (iCropCover == (int)0)
		pPC->fCropCoverFrac = pPM->fCropCoverFrac+(pPM->pNext->fCropCoverFrac-pPM->fCropCoverFrac)*
			                                      ((int)SimTime-pPM->iDay)/fTotalTime;
		//pPC->fCropCoverFrac = min((float)1,pPM->fCropCoverFrac);
       }
	}
	//*/

  // if(iIsConstRootDens)
  }
  else
  {

  if (NewDay(pTi))
  {
  /* //now next pPM assigned in time.c //ep 250907
  while ((pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime))
     {
     pPM = pPM->pNext;
     } 
  */

  if(GROWING_TIME)
  {
	  if(!pPl->pDevelop->fDevStage)
      {
		  InitiateLeachModel(exp_p);
      }
	  
      
	  pPl->pDevelop->iDayAftEmerg  = (int)SimTime  - pPM->iDay;
	  
	  fTotalTime    = max((float)(pPM->pNext->iDay  - pPM->iDay + (float)1), (float)1);
	  
	  fFracGrowS = pPl->pDevelop->iDayAftEmerg/fTotalTime;
	  
	  
	  // *********************************************************************
	  //                        Leaf Area Index
	  
	  f1 = (float)6.0 - ((float)12.0 * fFracGrowS);
	  
	  /* Lasse als Anfangswert auch Werte > 0 zu.*/    
	  pPC->fLAI = pPM->fLAI  + (pPM->pNext->fLAI - pPM->fLAI) / ((float)1.0 + (float)exp(f1));
	  pPC->fCropCoverFrac=pPM->fCropCoverFrac+(pPM->pNext->fCropCoverFrac-pPM->fCropCoverFrac)
		                  / ((float)1.0 + (float)exp(f1));
	  //pPC->fCropCoverFrac = min((float)1,pPM->fCropCoverFrac);
	  
  } // Growing Time
  else
  {
  if(pPl->pDevelop->fDevStage)
  {
  pPl->pDevelop->fDevStage=(float)0;
  Message(1, COMMENT_GROWING_END); 
  }          
  
  pPC->fLAI           = (float) 0.0;                
  pPC->fCropCoverFrac = (float) 0.0;
  }  //else
	

  } //NewDay

  }
 return OK;
} //  ende   wachstumL    

/**********************************************************************************************/
/*                                                                                            */
/*  Name     : LeafMaizeLeach()                                                               */
/*                                                                                            */
/*                                                                                            */
/*  Autor    : ep/gsf 31.7.00 nach  ch/gsf  26.4.95 & 13.7.97 und cs/ibs                      */
/*                                                                                            */
/*  Datum    : 31.07.00                                                                       */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pPC->fLAI                                                             */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI LeafMaizeLeachEP(EXP_POINTER) //EP
{
 DECLARE_COMMON_VAR
 
 // Änderung in der neuen Variablenstruktur: Zugriff auf die Hauptfrucht
 // pPl-> wird hochgzählt zu pPl = pPl->pNext->pNext->;
 PPLTMEASURE    pPM = pPl->pPMeasure;
 PCANOPY        pPC = pPl->pCanopy;

  float    fFracGrowS;             // fraction of growing season
  float    fMaxAge;                // total duration of growth
  float    f1;                                 
  


  if(iIsConstRootDens)
  {
	  if(pPC->fLAI == (float)0)
		  pPC->fLAI = (float) 10.0;                
//	  if(SimStart(pTi))
  }
  
  else
  {

  if(NewDay(pTi))
  {   
   if(((int)SimTime>=pPl->pModelParam->iEmergenceDay)&&
    ((int)SimTime<=pPl->pModelParam->iHarvestDay))
   {
     
	  
//FGS
    fMaxAge    = max(pPl->pModelParam->iHarvestDay - pPl->pModelParam->iEmergenceDay, (float)1);
    fFracGrowS = (SimTime - pPl->pModelParam->iEmergenceDay)/fMaxAge;

	  // *********************************************************************
//LAI	                          Leaf Area Index
	  
	  f1 = (float)6.0 - ((float)12.0 * fFracGrowS);
	  
      while (pPM->pNext != NULL)
      {
	   if (pPM->fLAI > pPM->pNext->fLAI)
       {                                     
        pPM->pNext->fLAI = pPM->fLAI;
       }

      pPM = pPM->pNext;
      } 

	  /* Lasse als Anfangswert auch Werte > 0 zu.*/    
	  pPC->fLAI = pPM->fLAI / ((float)1.0 + (float)exp(f1));
	  //pPC->fLAI = (float)10 / ((float)1.0 + (float)exp(f1));
  
  }	// if plant is growing
  else
  {
  if(pPl->pDevelop->fDevStage)
  {
  pPl->pDevelop->fDevStage=(float)0;
  Message(1, COMMENT_GROWING_END); 
  }          
  
  pPC->fLAI           = (float) 0.0;                
  
  } // else
  } // new day
	  
  }
 return OK;
} //  end   LeafMaizeLeach    

/**********************************************************************************************/
/*                                                                                            */
/*  Name     : LAI2CropCover()                                                                */
/*                                                                                            */
/*                                                                                            */
/*  Autor    : gsf/ch     17.7.97                                                             */
/*                                                                                            */
/*  Datum    : 17.07.97                                                                       */
/*                                                                                            */
/*  Purpose   : Sehr simpler Ansatz zur Umrechnung LAI -> CropCoverFrac                       */
/*              Da die Pflanzenmodelle LAI modellieren muss CropCoverFrac aus diesem          */
/*              geschätzt werden.                                                             */
/**********************************************************************************************/
float WINAPI LAI2CropCover(float fLAI)
{
float fCropCoverFrac;

  fCropCoverFrac = min((float)1, fLAI / (float)3);

return fCropCoverFrac;
}





/*******************************************************************/
/*  Name     : WaterUptakeNimahHanks()                             */
/*                                                                 */
/*  Eingabe  : Wurzelverteilung, Pot. Transpiration im Zeitschritt */
/*  Ausgabe  : Akt. Transpiration                                  */
/*             Veraendeung des Wassergehalts durch                 */
/*             Wurzelwasseraufnahme ---                            */
/*  Funktion : Berechnung Wasseraufnahme der Pflanze               */
/*             Veränderung des Bodenwassergehalts                  */
/*             Methode Leachn                                      */
/*  Autor    : Ch. Haberbosch                                      */
/*  Datum    : 7.1.97                                              */
/*******************************************************************/
int WINAPI WaterUptakeNimahHanks(EXP_POINTER) 
{
	DECLARE_COMMON_VAR
		
	signed short int  iterat;
	
	/* ch, fRootPot is static for better performance */
	static float  fRootPot = (float)0;			              	/*   [mm]   */
	float         relWuVert = (float)0;			              	/*   [1]    */
	float         fEffectPot[MAXSCHICHT],fPotMax,fPotMin;       /*   [mm]   */
	float         fRootDensTotal = (float)0;        			/*   [mm]   */
	float         fUptake = (float)0;				        	/*   [mm]   */
	float         fUptakeLay = (float)0;                        /*   [mm]   */
	float		  fd = (float)0;
	
	PWLAYER       pWL;
	PLAYERROOT    pLR;
	PROOT         pRO;   
	
#ifdef PLA_WAT_DEBUG
	if (SimStart(pTi)) OpenDebugF();
#endif
	
	if (pPl == NULL) return -1;
	
	pRO = pPl->pRoot;
   	
    /* calculate only if Transpiration occurs */
	if ((pPl->pDevelop->fDevStage > EPSILON || iIsConstRootDens) && (pPl->pPltWater->fPotTranspdt > EPSILON))      
	{
		
		for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
		{
			/* comment in Leach:    lpPot->osmoticPot *= ((float)(-1.0 * 360.0));  */
			
			/* effective water potential = matric + osmotic - root resistance trerm */
			fEffectPot[iLayer] = (float)min((float)0.0,pWL->fMatPotOld)
				+ pWL->fOsmPot
				- (float)1.05 			/*  in Leach: * root flow resistance term */
				* pSo->fDeltaZ
				* (float)iLayer;
			
			fRootDensTotal += pLR->fLengthDens;
		}  /* for */
		
		
		fPotMax = (float)0.0;
		
		fPotMin = MIN_ROOT_POT;
		
		fRootPot = (float)max((fPotMin + fPotMax) / (float)2.0,(float)2.0 * fRootPot);
		
		if (fRootDensTotal > EPSILON)
		{
			/*   Iteration    1.. 40  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
			for (iterat=0; iterat< 50; iterat++)
//			for (iterat=0; iterat< 250; iterat++)
			{                         
				fUptake = (float)0.0;
				
				for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
				{          
					relWuVert = pLR->fLengthDens / fRootDensTotal;
					
					/* For Uptake there must exist roots,
					root potential must be lower than effective water potential and
					effective water potential must be higher than welting point. */
					if ((relWuVert>EPSILON) && (fRootPot < fEffectPot[iLayer])
						&& (fEffectPot[iLayer] > MIN_ROOT_POT))
					{         
						fUptake += (pWL->fHydrCond * relWuVert * DeltaT *(fEffectPot[iLayer] - fRootPot)
							/ ((float)10.0));     /* [ mm/d * 1 * d * mm / mm = mm ] */
					}/* if  */
					
					if(fUptake > pPl->pPltWater->fPotTranspdt + TOLERANCE) 
					{
						break;
					}
				} /* for */
				
				/* if Uptake too high increase root potential */
				if (fUptake > pPl->pPltWater->fPotTranspdt + TOLERANCE)
				{
					fPotMin  = fRootPot;
					fRootPot = (fPotMin + fPotMax) / (float)2.0;
					
				}  /* if */
				else 
					/* if Uptake too low decrease root potential */
					if (fUptake < pPl->pPltWater->fPotTranspdt - TOLERANCE)              
				{                                            
					fPotMax  = fRootPot;
					fRootPot = ((fPotMin + fPotMax)/(float)2.0);
					
					/* if fRootPot near to minimal potential 
					ch, because of the iteration procedure the potential cannot reach minimal value.
					if (fRootPot < MIN_ROOT_POT * (float)0.95) 
					{
					break;
				}       */
					
				}  /* else if */                         
				else
					/* Uptake is ok */
				{
					break;
				}                 
			}  /*  Iteration   */
			
			fUptake = (float)0;  
			
			for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
			{          
				relWuVert = pLR->fLengthDens / fRootDensTotal;
				
				/* For Uptake there must exist roots,
				root potential must be lower than effective water potential and
				effective water potential must be higher than welting point. */
				if ((relWuVert>EPSILON) && (fRootPot < fEffectPot[iLayer])
					&& (fEffectPot[iLayer] > MIN_ROOT_POT))
				{         
					fUptakeLay = (pWL->fHydrCond * relWuVert * DeltaT *(fEffectPot[iLayer] - fRootPot)
						/ ((float)10.0));     /* [ mm/d * 1 * d * mm / mm = mm ] */
					
					
					// wateruptake in each layer is realized by decreasing
					// the actual watercontent in this layer:
					fd = fUptakeLay / pSo->fDeltaZ;
					pWL->fContAct -= fd;  /* [1] */
					
					pLR->fPotLayWatUpt          = fUptakeLay / DeltaT;   /* [mm/day] */
					
					fUptake += fUptakeLay;
				}/* if  */
				
				
			} /* for */
			
/*	
			//Uptake wird jetzt hier berechnet!
			pPl->pRoot->fUptakeR = (float)0;
			
			for (SOIL2_LAYERS1(pWL,pWa->pWLayer->pNext,pLR,pPl->pRoot->pLayerRoot))
			{
				// this is the sink term in Richards Equation: 
				pLR->fActLayWatUpt   = (pWL->fContOld- pWL->fContAct) /  pTi->pTimeStep->fAct; // [1/day] 
				
				// It is assumed that H2O uptake only takes place, when soil is not freezing or thawing 
				if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON))
				{
					pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * pSo->fDeltaZ;  // [mm/day]
				}
			} // for 

*/	
			if (fUptake > pPl->pPltWater->fPotTranspdt + (float)0.1)
			{
				Message(0,ERROR_H2O_UPTAKE_TOO_HIGH); 
				
			}    
			/*
			else
			if (fUptake < pPl->pPltWater->fPotTranspdt - (float)0.1)
			{
			// this is no error! 
			Message(0,ERROR_H2O_UPTAKE_TOO_LOW); 
			
			  }    
			*/	
			
		} /* Wasserentzug möglich */
		else
		{
			Message(1,ERROR_NO_ROOTS); 
			
		}
		
	}   /*  if Transpiration */

return OK;
} /*  ende   WaterUptake    */





/*******************************************************************/
/*  Name     : WaterUptakeFeddes()                                 */
/*                                                                 */
/*  Eingabe  : Wurzelverteilung, Pot. Transpiration im Zeitschritt */
/*  Ausgabe  : Akt. Transpiration                                  */
/*             Veraendeung des Wassergehalts durch                 */
/*             Wurzelwasseraufnahme ---                            */
/*  Funktion : Berechnung Wasseraufnahme der Pflanze               */
/*             Veränderung des Bodenwassergehalts                  */
/*  Autor    : St.Achatz                                           */
/*  Datum    : 19.02.98                                            */
/*******************************************************************/


//--------------- Constants and Subroutines for WaterUptakeFeddes ---------------


float afRSPar[8] = {		(float)9999.0, 

							(float)-120000.0,		//Par1 --- h3
							(float)-8000.0,			//Par2 \__ h2
							(float)-2000.0,			//Par3 /
							(float)-250.0,			//Par4 --- h1
							(float)-100.0,			//Par5 --- h0
							(float)0.1,				//Par6
							(float)0.5}	;			//Par7

						
float FAlfa(float rRoot,float h)
{
	float res;
	float p0,p1,p2,p3;
	
	p0 = afRSPar[5];
	p1 = afRSPar[4];
	p3 = afRSPar[1];
	
	if      (rRoot < afRSPar[6]) p2 = afRSPar[2];
	else if (rRoot > afRSPar[7]) p2 = afRSPar[3];
	else                     p2 = (afRSPar[2]*(afRSPar[7] - rRoot) + 
								afRSPar[3]*(rRoot - afRSPar[6]))  /  (afRSPar[7] - afRSPar[6]);
	res = (float)0.0;
	
	if      (h >  p3  &&  h < p2) res = (h-p3)/(p2-p3);
	else if	(h >= p2  &&  h < p1) res = (float)1.0;
	else if (h >= p1  &&  h < p0) res = (h-p0)/(p1-p0);
	
	return res;
}


/*****************************/
/* FEDDES  UPTAKE  ROUTINE   */
/*****************************/

int WINAPI WaterUptakeFeddes(EXP_POINTER)
{
	DECLARE_COMMON_VAR

	int				L;
	float			rRoot;
	float			dxM;
	float			Alfa;
	float			fEffectPot[MAXSCHICHT];
	float			fRootDensTotal;
	float			Sink;
	
	PWLAYER       pWL;
	PLAYERROOT    pLR;
	
	
	if (pPl == NULL) return -1;
	if ((pPl->pDevelop->fDevStage < EPSILON && !iIsConstRootDens) || 
		(pPl->pPltWater->fPotTranspdt < EPSILON)) return -1;
	
	fRootDensTotal = (float)0.0;
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot))  
	{
		fEffectPot[iLayer] = (float)min((float)0.0,pWL->fMatPotOld);
		fRootDensTotal += pLR->fLengthDens;
	}  /* for */
	
	if (fRootDensTotal <= (float)0.0) return -1;

	rRoot = pPl->pPltWater->fPotTranspdt;
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot))  
	{
		if (pLR->fLengthDens > (float)0.0) 
		{
			dxM  = pSo->fDeltaZ;

			//SG20150930: Für Irenbe Witte - Feddes-alpha kann optional = 1 (const.) gesetzt werden
			if (afRSPar[0] < 0)
				Alfa = (float)1.0;
			else
				Alfa = FAlfa(rRoot,fEffectPot[iLayer]);	

		Sink = Alfa * pLR->fLengthDens / fRootDensTotal * rRoot/dxM;
			
			pWL->fContAct       -= Sink;
			pLR->fPotLayWatUpt   = Sink * dxM / DeltaT; 
		} //if
		else pLR->fPotLayWatUpt = (float)0.0;
	} //for

	// hp 221002: fPotUptakedt wird in CERES zur Berechnung der Stressfaktoren
    //            benoetigt und wird danach gleich Null gesetzt (einmal pro Tag!).

	L 	  = 1;
	pLR	=pPl->pRoot->pLayerRoot; 
    pPl->pPltWater->fPotUptakedt = (float)0.0;

    while (((pLR->fLengthDens!=(float)0.0)||(pLR->pNext->fLengthDens !=(float)0.0))
		  &&(L<pSo->iLayers-2))
		{
			pPl->pPltWater->fPotUptakedt += pLR->fPotLayWatUpt*DeltaT;
			L 	 ++;
			pLR =pLR ->pNext;
		}


/*	
	pPl->pRoot->fUptakeR = (float)0;
	
	for (SOIL2_LAYERS1(pWL,pWa->pWLayer->pNext,pLR,pPl->pRoot->pLayerRoot))
    {
		// this is the sink term in Richards Equation: 
		pLR->fActLayWatUpt   = (pWL->fContOld- pWL->fContAct) /  pTi->pTimeStep->fAct; //[1/day]
		
		// It is assumed that H2O uptake only takes place, when soil is not freezing or thawig
		if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON))
		{
			pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * pSo->fDeltaZ;  // [mm/day] 
		}
	} // for 
*/
	return OK;
}

/*******************************************************************/
/*  Name     : WaterUptakeVanGenuchten()                           */
/*                                                                 */
/*  Eingabe  : Wurzelverteilung, Pot. Transpiration im Zeitschritt */
/*  Ausgabe  : Akt. Transpiration                                  */
/*             Veraendeung des Wassergehalts durch                 */
/*             Wurzelwasseraufnahme ---                            */
/*  Funktion : Berechnung Wasseraufnahme der Pflanze               */
/*             Veränderung des Bodenwassergehalts                  */
/*  Autor    : St.Achatz                                           */
/*  Datum    : 19.02.98                                            */
/*******************************************************************/

//--------------- Constants and Subroutines for WaterUptakeVanGnuchten ---------------

float fh50Global  = (float)10000.0;
float fp1Global   = (float)1.0;

float FWStrs(float h, float h50, float p1)
{
	return (float)(1.0/(1.0 + abspower( min(h,(float)-1.0e-20)/h50 , p1 )));
}


/***********************************/
/* VanGenuchten   UPTAKE  ROUTINE   */
/***********************************/

int WINAPI WaterUptakeVanGenuchten(EXP_POINTER)
{
	DECLARE_COMMON_VAR
		
	float			rRoot;
	float			dxM;
	float			Alfa;
	float			fEffectPot[MAXSCHICHT];
	float			fRootDensTotal;
	float			Sink;
	
	PWLAYER       pWL;
	PLAYERROOT    pLR;
	
	
	if (pPl == NULL) return -1;
	if ((pPl->pDevelop->fDevStage < EPSILON  && !iIsConstRootDens) || 
		(pPl->pPltWater->fPotTranspdt < EPSILON)) return -1;
	
	fRootDensTotal = (float)0.0;
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot))  
	{
		fEffectPot[iLayer] = (float)min((float)0.0,pWL->fMatPotOld);
		fRootDensTotal += pLR->fLengthDens;
	}  /* for */
	
	if (fRootDensTotal <= (float)0.0) return -1;

	rRoot = pPl->pPltWater->fPotTranspdt;
	for (SOIL2_LAYERS1(pWL, pWa->pWLayer->pNext, pLR, pPl->pRoot->pLayerRoot))  
	{
		if (pLR->fLengthDens > (float)0.0) 
		{
			dxM  = pSo->fDeltaZ;
			Alfa = FWStrs(fEffectPot[iLayer], fh50Global, fp1Global);		  
			Sink = Alfa * pLR->fLengthDens / fRootDensTotal * rRoot/dxM;

			pWL->fContAct       -= Sink;
			pLR->fPotLayWatUpt   = Sink * dxM / DeltaT; 
		} //if
		else pLR->fPotLayWatUpt = (float)0.0;
	} //for

/*	
	pPl->pRoot->fUptakeR = (float)0;
	
	for (SOIL2_LAYERS1(pWL,pWa->pWLayer->pNext,pLR,pPl->pRoot->pLayerRoot))
    {
		// this is the sink term in Richards Equation: 
		pLR->fActLayWatUpt   = (pWL->fContOld- pWL->fContAct) /  pTi->pTimeStep->fAct; // [1/day] 
		
		// It is assumed that H2O uptake only takes place, when soil is not freezing or thawig 
		if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON))
		{
			pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * pSo->fDeltaZ;  // [mm/day] 
		}
	} // for 
*/
	return OK;
}


/**************************************************************/
/*  Name     : PotentialTranspiration()                       */
/*                                                            */
/*  Ein      : Zeiger auf Sammelvariable Evaporation,Zeit     */
/*             steuerung, Pflanze                             */
/*                                                            */
/*  Funktion : Belegung der Systemvariablen zur Transpiration */
/*             mit neu errechneten potentiellen               */
/*             Simulationswerten                              */
/*                                                            */
/*  Autor    : ep/gsf, ch/gsf, ep/gsf                         */
/*                                                            */
/*  Datum    : 19.08.92, 12.03.97, 02.08.00                   */
/*                                                            */
/**************************************************************/
/*  veränd. Var.:		pPW->fPotTranspdt                     */
/*						pPW->fPotTranspDay                    */
/**************************************************************/

int WINAPI  PotentialTranspiration(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 PPLTWATER   pPW;
 PEVAP       pEV;
 PCANOPY     pPC;
 extern float fRelIntercept;//,fCanopyClosure,fSCEff;
 float fLAIFactor;
             
  if (pPl == NULL) return -1;     
  
  pPW = pPl->pPltWater;         
  pPC = pPl->pCanopy;         
  pEV = pWa->pEvap;

 pPW->fPotTranspdt = (float)0;
 
 if ((pPC != NULL)&&(pWa->fPotETdt > EPSILON))
    {
	 /*
	 if (pPC->fCropCoverFrac > EPSILON)
	 	{
		 pPW->fPotTranspdt =  pWa->fPotETdt*pPC->fCropCoverFrac;// * min((float)1.0,pPC->fCropCoverFrac);
		 fPotTraDay =  pWa->fPotETDay*pPC->fCropCoverFrac;// * min((float)1.0,pPC->fCropCoverFrac);
		 
		 // transpiration is increased by the evaporation deficit
		  if(pWa->pEvap->fActR < pWa->pEvap->fPotR)
		 {	 
          pPW->fPotTranspdt = max((float)0,pWa->fPotETdt - pWa->pEvap->fActR*DeltaT);
		  //pPW->fPotTranspdt=min(pPW->fPotTranspdt*(float)1.1,pWa->fPotETdt-pWa->pEvap->fActR*DeltaT);
		  //fPotTraDay = min(fPotTraDay*(float)1.1,pWa->fPotETDay-pWa->pEvap->fActR*DeltaT);
		 }
		 
	 	} // crop cover > 0
	 
	 else if (pPC->fLAI > EPSILON)
		{
		 pPW->fPotTranspdt =  pWa->fPotETdt * min((float)1.0,pPC->fLAI/(float)3);
		 fPotTraDay =  pWa->fPotETDay * min((float)1.0,pPC->fLAI/(float)3);
		 
		 // transpiration is increased by the evaporation deficit
		 if(pWa->pEvap->fActR < pWa->pEvap->fPotR)
		 {	 
		  pPW->fPotTranspdt=min(pPW->fPotTranspdt*(float)1.1,pWa->fPotETdt-pWa->pEvap->fActR*DeltaT);
		  //fPotTraDay = min(fPotTraDay*(float)1.1,pWa->fPotETDay-pWa->pEvap->fActR*DeltaT);
		 }
		 
		}// LAI > 0
     */
    
    if ((pPC->fCropCoverFrac > EPSILON)||(pPC->fLAI > EPSILON))
	{
    //potentielle Transpiration ohne Interzeptionsverdunstung	
	pPW->fPotTranspdt =  max((float)0,pWa->fPotETdt - pWa->pEvap->fPotR*DeltaT);
	fPotTraDay =  max((float)0,pWa->fPotETDay - pEV->fPotDay);

    ///* 
	// transpiration is increased by the evaporation deficit
    if((pWa->pEvap->fActR < pWa->pEvap->fPotR)&&(fPotTraDay >(float)0))
    {
     pPW->fPotTranspdt = max((float)0,pWa->fPotETdt - pWa->pEvap->fActR*DeltaT);
	}
    
    //potentielle Transpiration eingeschränkt bei niedrigem LAI <3 :
     fLAIFactor = LAI2CropCover(pPl->pCanopy->fLAI);
     pPW->fPotTranspdt*=fLAIFactor;
     fPotTraDay*=fLAIFactor;
    
	//potentielle Transpiration mit Interzeptionsverdunstung
    /*
    fPotTraDay *= ((float)1 - fRelIntercept);  
    pPW->fPotTranspdt *= ((float)1 - fRelIntercept);
	//*/
	}//pPC->fLAI>EPSILON
    }//pPC!=Null und fPotTranspdt > 0
    

 return 1;
}  /* Ende potentielle Transpirations - Berechnung    */
    

/**********************************************************************************************/
/*                                                                                            */
/*  Name     : NUptake()                                                                      */
/*             N-Aufnahme wird taeglich nur einmal berechnet!                                 */
/*             => dt = 1 day                                                                  */
/*             N-Aufnahme ist unabhängig vom Transpirationsstrom,                             */
/*             sie ist durch max. vorhandenes N-min beschränkt,                               */
/*             sie verteilt sich proportional zur Wurzelverteilung                            */
/*                                                                                            */
/*  Autor    : ch/gsf                                                                         */
/*  Datum    : 26.8.95                                                                        */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pCL->fNH4N                                                            */
/*						pCL->fNO3N                                                            */
/* 						pLR->FLengthDens                                                      */
/*						pPM->pNext->fTotalN                                                   */
/*						pPN->fActNH4NUpt                                                      */
/*						pPN->fActNO3NUpt                                                      */
/*						pPN->fActNUpt                                                         */
/*						pPN->fActNUptR                                                        */
/*						pPN->fCumActNUpt                                                      */
/*						pPN->fCumPotNUpt                                                      */
/**********************************************************************************************/
int WINAPI NUptake(EXP_POINTER)	//CH
{
  float    fFracGrowS;         // fraction of growing season
  float    fFracTotNUP;        // fraction of total N Uptake
  float    fDeficitN;          // Deficit of N
  float    fNH4Aufnahme;
  float    fNO3Aufnahme;              
  float    fMaxAge;            // Gesamtdauer des Wachstums
  float    f1;                                 
  float    fDOCAufnahme;

  float    DeltaZ = pSo->fDeltaZ;
  float    fNmin = (float) 0.0;//0.75;//0.25;//1;  // N-Gehalt,den Pflanzen nicht entziehen [kg/ha].
  
  DECLARE_COMMON_VAR
  
  PPLTMEASURE    pPM;
  PPLTNITROGEN   pPN;
  PLAYERROOT     pLR;
  PCLAYER        pCL;

  if (pPl == NULL) return -1;         
  
  pPM = pPl->pPMeasure;                  
  pPN = pPl->pPltNitrogen;

  fNmin *= DeltaZ/(float)100;

  if (pPl->pDevelop->fDevStage > EPSILON)
  {
    // pPl->pDevelop->iDayAftEmerg must be set!                 

  while ((pPM->pNext != NULL) && ((pPM->pNext->iDay - EPSILON) <= SimTime))
     {
     pPM = pPM->pNext;
     } 
    
   if (pPM->pNext == NULL) 
   	{
   	return -1;
   	}
   
    fMaxAge    = max((float)(pPM->pNext->iDay  - pPM->iDay), (float)1);
    fFracGrowS = (SimTime - pPM->iDay)/fMaxAge;
   
    if (fFracGrowS <= (float) 0.3)
        fFracTotNUP = (float) 5.555 * fFracGrowS * fFracGrowS;    
   
    else 
        fFracTotNUP = (float)-1.0 * fFracGrowS * fFracGrowS           // fgs==0.3 => fgn = 0.5
                                     + (float)2 * fFracGrowS;
                                              
	if (!Test_Range(pPM->pNext->fTotalN, pPM->fTotalN, (float)1000, "Measurement fTotalN"))
	{
      if (pPM->fTotalN > pPM->pNext->fTotalN)
      {                                     
      pPM->pNext->fTotalN = pPM->fTotalN;
      }
      else
      {
      strcpy(lpOut,ERROR_IN_FUNCTION_TXT);
      strcat(lpOut,"NUptake()");
      Message(3,lpOut);
      return -1;
      }
      }                                      
                                              
                                              
    pPN->fCumPotNUpt = pPM->fTotalN + fFracTotNUP *  (pPM->pNext->fTotalN - pPM->fTotalN);
    //lpx->lPtrPlant->MaxKumNUptake;
   
    fDeficitN = pPN->fCumPotNUpt - pPN->fCumActNUpt;                                 

    if (fDeficitN >EPSILON) 
      {
      if (pPl->pRoot->pLayerRoot->fLengthDens < (float)0.001)                
		   {
		     pPN->fActNUpt = pPN->fActNUptR = (float)0;
		
		     Message(1,ERROR_NO_ROOTS); 
		
		     return -1;
		   }
         
      f1 = (float)0;
                 
      for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
        {
        f1 += (pCL->fNH4N + pCL->fNO3N) * pLR->fLengthDens;
        }
                         
      pPN->fActNO3NUpt = pPN->fActNH4NUpt = (float) 0;

                          
      for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
        {
         fNH4Aufnahme = min(fDeficitN * pCL->fNH4N * pLR->fLengthDens / f1, pCL->fNH4N - fNmin);     
         fNH4Aufnahme = max((float) 0, fNH4Aufnahme);     
         fNO3Aufnahme = min(fDeficitN * pCL->fNO3N * pLR->fLengthDens / f1, pCL->fNO3N - fNmin);
         fNO3Aufnahme = max((float) 0, fNO3Aufnahme);             
 
		 fDOCAufnahme = pCL->fDOCSoilConc * pLR->fActLayWatUpt;
		 fDOCAufnahme = max((float) 0, fDOCAufnahme);             
             
         pPN->fActNH4NUpt += fNH4Aufnahme;
         pCL->fNH4N       -= fNH4Aufnahme;

         pPN->fActNO3NUpt += fNO3Aufnahme;
         pCL->fNO3N       -= fNO3Aufnahme;

    	 pCL->fDOC        -= fDOCAufnahme/(float)100;

        } // for

      pPN->fActNUpt = pPN->fActNO3NUpt + pPN->fActNH4NUpt;
      
      pPN->fActNUptR = (pPN->fActNO3NUpt + pPN->fActNH4NUpt)/DeltaT;

      } // Deficit > EPSILON
    
    else
     {
     pPN->fActNUpt =   pPN->fActNUptR = (float)0;
     }

  }   // DevStage > 0
  else
  {
    pPN->fCumPotNUpt = pPN->fCumActNUpt = pPN->fActNUpt = pPN->fActNUptR = pPN->fActNO3NUpt = pPN->fActNH4NUpt = (float)0;
   }

  return 1;
  } /*  ende   NUptake */


/**********************************************************************************************/
/*                                                                                            */
/*  Name     : NUptake()                                                                      */
/*																							  */
/*             N-Aufnahme nach original LEACHN Version 3.1                                    */
/*                                                                                            */
/*  Autor    : ep / gsf 31.7.00  nach ch/gsf 26.8.95                                          */
/*  Datum    : 31.7.00                                                                        */
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:		pCL->fNH4N                                                            */
/*						pCL->fNO3N                                                            */
/*						pPN->fActNH4NUpt                                                      */
/*						pPN->fActNO3NUpt                                                      */
/*						pPN->fActNUpt                                                         */
/*						pPN->fActNUptR                                                        */
/*						pPN->fCumActNUpt                                                      */
/*						pPN->fCumPotNUpt                                                      */
/**********************************************************************************************/
int WINAPI NUptakeEP(EXP_POINTER) //EP
{
  //OrigLeachN
  float    fFracGrowS;         // fraction of growing season
  float    fFracTotNUP;        // fraction of total N Uptake
  float    fDeficitN;          // Deficit of N
  float    fNH4Aufnahme;
  float    fNO3Aufnahme;
  float    fNmin = (float) 1;  //0.5;//0.75;//0.25;//1;
                               // N amount which cannot be taken up by plants [kg/ha].
  float    fMaxAge;            // duration of growth period
  float    f1;                                 
  
  DECLARE_COMMON_VAR
  
  PPLTMEASURE    pPM;
  PPLTWATER      pPW;
  PPLTNITROGEN   pPN;
  PLAYERROOT     pLR;
  PCLAYER        pCL;

  if (pPl == NULL) return -1;         
  
  pPM = pPl->pPMeasure;                  
  pPW = pPl->pPltWater;         
  pPN = pPl->pPltNitrogen;

  if (pPl->pDevelop->fDevStage > EPSILON)
  {
//FGS
    fMaxAge    = max(pPl->pModelParam->iHarvestDay - pPl->pModelParam->iEmergenceDay, (float)1);
    fFracGrowS = (SimTime - pPl->pModelParam->iEmergenceDay)/fMaxAge; //FGS
   
//FTNU
    if (fFracGrowS <= (float) 0.3) 
        fFracTotNUP = (float) 8.876 * abspower(fFracGrowS,(float)3.87);    
    else 
        fFracTotNUP = (float)-0.66 * fFracGrowS +
		              (float)3.485 * fFracGrowS * fFracGrowS -  
                      (float)0.93 * fFracGrowS * fFracGrowS * fFracGrowS -
					  (float)0.899 * fFracGrowS * fFracGrowS * fFracGrowS * fFracGrowS;
                                              
//POTUPT(CROPNO) annual	N uptake
   while (pPM->pNext != NULL)
   {
   /*
	if (!Test_Range(pPM->pNext->fTotalN, pPM->fTotalN, (float)1000, "Measurement fTotalN"))
	{
      if (pPM->fTotalN > pPM->pNext->fTotalN)
      {                                     
      pPM->pNext->fTotalN = pPM->fTotalN;
      }
      else
      {
      strcpy(lpOut,ERROR_IN_FUNCTION_TXT);
      strcat(lpOut,"NUptake()");
      Message(3,lpOut);
      return -1;
      }
    }                                      
   */
   pPM = pPM->pNext;
   } 

   pPN->fCumPotNUpt = pPM->fTotalN * fFracTotNUP;
   
//DELFTN   
   //fDeficitN = pPN->fCumPotNUpt - pPN->fCumActNUpt;                                 
   fDeficitN = max((float)0,pPN->fCumPotNUpt-pPN->fCumActNUpt);
   if(fFracGrowS > (float)0.7) 
	   fDeficitN = fDeficitN *((float)1-(fFracGrowS-(float)0.7)/(float)0.3);
   
   if (fDeficitN >EPSILON) 
      {
      if (pPl->pRoot->pLayerRoot->fLengthDens < (float)0.001)                
		   {
		     pPN->fActNUpt = pPN->fActNUptR = (float)0;
		
		     //Message(1,ERROR_NO_ROOTS); 
		
		     return -1;
		   }
         
//DELN	  
   if(fPotTraDay>(float)0) 
   {  
	fDeficitN *= pPW->fPotTranspdt/fPotTraDay;
   }
   else
   {
	fDeficitN =(float)0;
   }
      
   
//TOTAL
   f1 = (float)0;
                      
   for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
     {
      f1 += (pCL->fNH4N + pCL->fNO3N) * pLR->fActLayWatUpt * DeltaT;
     }
                         
      pPN->fActNO3NUpt = pPN->fActNH4NUpt = (float) 0;

//USE(M,I)                          
   for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pLR, pPl->pRoot->pLayerRoot))
     {
      fNH4Aufnahme = min(fDeficitN * pCL->fNH4N * pLR->fActLayWatUpt / f1, pCL->fNH4N - fNmin);     
      fNH4Aufnahme = max((float) 0, fNH4Aufnahme);     
      fNO3Aufnahme = min(fDeficitN * pCL->fNO3N * pLR->fActLayWatUpt / f1, pCL->fNO3N - fNmin);
      fNO3Aufnahme = max((float) 0, fNO3Aufnahme);             

      pPN->fActNH4NUpt += fNH4Aufnahme;
      pCL->fNH4N       -= fNH4Aufnahme;

      pPN->fActNO3NUpt += fNO3Aufnahme;
      pCL->fNO3N       -= fNO3Aufnahme;

	 } // for

      pPN->fActNUpt = pPN->fActNO3NUpt + pPN->fActNH4NUpt;
      
      pPN->fActNUptR = (pPN->fActNO3NUpt + pPN->fActNH4NUpt)/DeltaT;

      } // Deficit > EPSILON
    
    else
    {
     pPN->fActNUpt =   pPN->fActNUptR = (float)0;
    }

  }   // DevStage > 0
  else
  {
   pPN->fCumPotNUpt = 
   pPN->fCumActNUpt = 
   pPN->fActNUpt    = 
   pPN->fActNUptR   = 
   pPN->fActNO3NUpt = 
   pPN->fActNH4NUpt = (float)0;
  }

  return 1;
  } /*  ende   NUptake */

/*******************************************************************************
** EOF */