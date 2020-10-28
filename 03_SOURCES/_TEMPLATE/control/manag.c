/*********************************************************************************/
/**                                                                             **/
/**  Module     : Manage.c                                                      **/
/**                                                                             **/
/**  Description: Integration der Modelle zur Beruecksichtigung von Bewirt-     **/
/**               schaftungsmassnamhen zur Bodenbearbeitung, mineralischer und  **/
/**               organischer Duengung sowie der Beregnung.                     **/
/**                                                                             **/
/**               ts/tum  12.08.96 (CERES)                                      **/
/**               ab/gsf  18.12.01 (DNDC)                                       **/
/**               cb/zs   25.12.01 Speicherueberschreiber nach Korrektur des    **/
/**                                SimZeitraums in der Methode "DateToDays"     **/
/**                                (siehe PATCH CH.BAUER)                       **/
/*********************************************************************************/

#include  "xinclexp.h"
//#include "PLANTMOD.H"
#include "xlanguag.h"

#define SET_IF_99(x,y) 		if (x == (float)-99)   x = (float)y;


extern float abspower(float,float);

/* aus output.ccp */
extern int   WINAPI Message(long, LPSTR);
/* from time1.c*/
extern int SimStart(PTIME);      
/* from util_fct.c */
extern int	NewDay(PTIME);

/*********************************************************************************/
/**                    Externe Functionen                                       **/
/*********************************************************************************/

/* from soil.c */
extern int WINAPI Bioturbation(EXP_POINTER);                                                    
extern int WINAPI NaturRueck(EXP_POINTER);                                                    

/* from miner.c */
extern int WINAPI MinerOrgDuengHoff(EXP_POINTER);
extern int WINAPI NitrOrgNH4Hoff(EXP_POINTER);

extern int PlantVariableInitiation(EXP_POINTER);
extern int SPASS_Genotype_Read(HINSTANCE);
//extern BOOL bMaturity,bPlantGrowth;

/*********************************************************************************/
/**                 Interne Functionen                                          **/
/*********************************************************************************/
							
signed short int WINAPI TSSoilLoosing(EXP_POINTER);                                                    
signed short int WINAPI TSSoilMixing(EXP_POINTER);                                                    
signed short int WINAPI TSFertilizer(EXP_POINTER);                                                    
signed short int WINAPI LAFertilizer(EXP_POINTER);                                                    
signed short int WINAPI TSFertilizerLoss(EXP_POINTER);                                                    
signed short int WINAPI TSIrrigation(EXP_POINTER);                                                    
signed short int WINAPI TSIrrigationLoss(EXP_POINTER);                                                    
signed short int WINAPI TSSowing(EXP_POINTER);                                                    

signed short int WINAPI TSComparePreCropDate(EXP_POINTER);                                                    
signed short int WINAPI TSCompareStartValueDate(EXP_POINTER);                                                    
signed short int WINAPI TSCorrectMeasureValue(EXP_POINTER);                                                    

//dabei intern verwendete

signed short int DNDCTillage(EXP_POINTER);
signed short int NCSOILTillage(EXP_POINTER);
signed short int EinarbeitRueckstaendeDNDC(EXP_POINTER);
signed short int DurchMischDNDC(EXP_POINTER);
signed short int EinarbeitRueckstaendeCERES(EXP_POINTER);
signed short int DurchMischCERES(EXP_POINTER);

signed short int InitBBGeraet(EXP_POINTER);
signed short int LagerungNeu(EXP_POINTER);
signed short int EinarbeitRueckstaende(EXP_POINTER);
signed short int DurchMisch(EXP_POINTER);
signed short int InitOrgDuenger(EXP_POINTER);
signed short int InfiltrationOrgDuenger(EXP_POINTER);
signed short int InfiltrationOrgDuengerRegen(EXP_POINTER);
signed short int InfiltrationOrgDuengerBeregnung(EXP_POINTER);

static	float   aforigPWP[MAXSCHICHT];
static	float   aforigFK[MAXSCHICHT];
static	float   aforigSatCER[MAXSCHICHT];		

BOOL 	bCorrectMeasure;

/*********************************************************************************/
/*  Name     : SoilLoosing                                                       */
/*                                                                               */
/*  Funktion : Durch Bodenbearbeitungsmassnahmen werden durch die Lockerung      */
/*             zahlreiche Kenngroessen waehrend der Simulation veraendert. Die   */
/*             integrieretn Modellansaetze sind dem Programm EPIC entnommen      */
/*             und wurden modifiziert.                                           */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI TSSoilLoosing(EXP_POINTER)
{
  if (pMa->pTillage != NULL)
  {
   	if (pTi->pSimTime->fTimeAct >= (float)pMa->pTillage->iDay)
	{
	InitBBGeraet(exp_p); 
	LagerungNeu(exp_p); 

	}/* Ende if (datum == pMa->pTillage->lDate) Bodenbearbeitungstermin */
  }
	NaturRueck(exp_p); 	

  return 1;

}      /*=====   Ende der Integration Lockerung nach EPIC  =================*/

/*********************************************************************************/
/*  Name     : SoilMixing                                                        */
/*                                                                               */
/*  Funktion : Durch Bodenbearbeitungsmassnahmen findet eine Einarbeitungng von  */
/*             Ernterueckstaenden und eine Durchmischung der im Boden            */
/*             befindlichen Pools statt. Hierzu wurden Modellansaetze aus dem    */
/*             Modell EPIC integriert und modifiziert.                           */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSSoilMixing(EXP_POINTER)
{
        char  acdummy[80];
        LPSTR lpOut       = acdummy;

        char  acdummy2[80];
        LPSTR lpOut2       = acdummy2;
 

  if (pMa->pTillage != NULL)
	if ((float)pTi->pSimTime->fTimeAct >= (float)pMa->pTillage->iDay)
	{       
	
	InitBBGeraet(exp_p); 
 	
 	EinarbeitRueckstaende(exp_p);  
    DurchMisch(exp_p); 		

      strcpy(lpOut,COMMENT_TILLAGE_TXT); 
      itoa((int)(pMa->pTillage->fDepth), lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mm depth."); 
      
      Message(1, lpOut);
    
	}/* Ende if (datum == pMa->pTillage->lDate) Bodenbearbeitungstermin */

	Bioturbation(exp_p); 		

  return 1;

}      /*=====   Ende der Integration Lockerung nach EPIC  =================*/

/*********************************************************************************/
/*  Name     : DNDCSoilMixing                                                    */
/*                                                                               */
/*  Funktion : Durch Bodenbearbeitungsmassnahmen findet eine Einarbeitungng von  */
/*             Ernterueckstaenden und eine Durchmischung der im Boden            */
/*             befindlichen Pools statt. Hierzu wurden Modellansaetze aus dem    */
/*             Modell EPIC integriert und modifiziert.                           */
/*                                                                               */
/*  Autor    : A. Berkenkamp                                                     */
/*  Datum	 : 18.12.01                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI DNDCSoilMixing(EXP_POINTER)
{
        char  acdummy[80];
        LPSTR lpOut       = acdummy;

        char  acdummy2[80];
        LPSTR lpOut2       = acdummy2;
 

  if (pMa->pTillage != NULL)
	if ((float)pTi->pSimTime->fTimeAct >= (float)pMa->pTillage->iDay)
	{       
	
	InitBBGeraet(exp_p); 
 	
 	EinarbeitRueckstaendeDNDC(exp_p);  
    DurchMischDNDC(exp_p); 		

      strcpy(lpOut,COMMENT_TILLAGE_TXT); 
      itoa((int)(pMa->pTillage->fDepth), lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mm depth."); 
      
      Message(1, lpOut);
    
	}/* Ende if (datum == pMa->pTillage->lDate) Bodenbearbeitungstermin */

	Bioturbation(exp_p); 		

	return 1;

}      /*=====   Ende der Integration Lockerung nach EPIC  =================*/

/*********************************************************************************/
/*  Name     : CERESSoilMixing                                                        */
/*                                                                               */
/*  Funktion : Durch Bodenbearbeitungsmassnahmen findet eine Einarbeitungng von  */
/*             Ernterueckstaenden und eine Durchmischung der im Boden            */
/*             befindlichen Pools statt. Hierzu wurden Modellansaetze aus dem    */
/*             Modell EPIC integriert und modifiziert.                           */
/*                                                                               */
/*  Autor    : A. Berkenkamp                                                     */
/*  Datum	 : 18.12.01                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI CERESSoilMixing(EXP_POINTER)
{
        char  acdummy[80];
        LPSTR lpOut       = acdummy;

        char  acdummy2[80];
        LPSTR lpOut2       = acdummy2;
 

  if (pMa->pTillage != NULL)
	if ((float)pTi->pSimTime->fTimeAct >= (float)pMa->pTillage->iDay)
	{       
	
	InitBBGeraet(exp_p); 
 	
 	EinarbeitRueckstaendeCERES(exp_p);  
    DurchMischCERES(exp_p); 		

      strcpy(lpOut,COMMENT_TILLAGE_TXT); 
      itoa((int)(pMa->pTillage->fDepth), lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mm depth."); 
      
      Message(1, lpOut);
    
	}/* Ende if (datum == pMa->pTillage->lDate) Bodenbearbeitungstermin */

	Bioturbation(exp_p); 		

	return 1;

}      /*=====   Ende der Integration Lockerung nach EPIC  =================*/


/*********************************************************************************/
/*  Name     : TSFertilizer                                                      */
/*                                                                               */
/*  Funktion : Die N-Zufuhr durch mineralische oder organische Duengemittel wird */
/*             nach Modellansaetzen von Godwin, die von Engel modifiziert wurden,*/
/*             integriert.                                                       */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*				pCh->pCLayer->fNO3N                                              */
/*				pCh->pCLayer->fNH4N                                              */
/*				pCh->pCLayer->fUreaN                                             */
/*				pCh->pCLayer->fNManure                                           */
/*				pCh->pCLayer->fCManure                                           */
/*				pCh->pCLayer->fManureCN                                          */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSFertilizer(EXP_POINTER)
{                                                                   
DECLARE_COMMON_VAR
        
char  acdummy2[80];
float fumFacC;

LPSTR lpOut2       = acdummy2;

  if (pMa->pNFertilizer != NULL)
  {
	if (pTi->pSimTime->fTimeAct >= (float)pMa->pNFertilizer->iDay)
	{

	fumFacC = (float)0.4; // 40 % der org. Substanz im Duenger ist Kohlenstoff

	/*Faktor fumFacC wird in Abhaengugkeit von Duenger bestimmt. Bis
	  der Duenger identifiziert werden kann standardmaessig auf 0.4 gesetzt.  */
		pMa->pNFertilizer->fCorgManure = pMa->pNFertilizer->fDryMatter * fumFacC;	
		
		if (pMa->pNFertilizer->fNorgManure > EPSILON) 
		{
		pMa->pNFertilizer->fOrgManureCN = pMa->pNFertilizer->fCorgManure / pMa->pNFertilizer->fNorgManure; 
		}
		else
		pMa->pNFertilizer->fOrgManureCN = (float)0.1;

	/*Uebertragen der Duengergroessen auf die Oberflaechenschicht.  */

		pCh->pCProfile->fNO3NSurf     += pMa->pNFertilizer->fNO3N;

		if (!lstrcmp(pMa->pNFertilizer->acCode,"RE\0"))  //organische Duengung
		{
		pCh->pCProfile->fNH4ManureSurf+= pMa->pNFertilizer->fNH4N;
		}
		else
		{
		pCh->pCProfile->fNH4NSurf     += pMa->pNFertilizer->fNH4N;
		}
		
		//pCh->pCProfile->fUreaNSurf    += pMa->pNFertilizer->fUreaN;
		//pCh->pCProfile->fNManureSurf  += pMa->pNFertilizer->fNorgManure;
		//pCh->pCProfile->fCManureSurf  += pMa->pNFertilizer->fCorgManure;
		//pCh->pCProfile->fCNManureSurf += pMa->pNFertilizer->fOrgManureCN;

		//SG 20161009: For DAISY model - Partitioning of fNorgManure and fCorgManure
		// Scott Demyan: "current partition of 100% to AOM2 causes too little stabilization"
		// New partitioning:
		// AOM1 - 72% 
		// AOM2 - 18%
		// SOM2 - 10% 
		pCh->pCProfile->fUreaNSurf    += pMa->pNFertilizer->fUreaN;
		pCh->pCProfile->fNManureSurf  += (float)0.18*pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCManureSurf  += (float)0.18*pMa->pNFertilizer->fCorgManure;
		pCh->pCProfile->fCNManureSurf += pMa->pNFertilizer->fOrgManureCN;
		pCh->pCProfile->fNLitterSurf  += (float)0.72*pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCLitterSurf  += (float)0.72*pMa->pNFertilizer->fCorgManure;
		pCh->pCProfile->fCNLitterSurf += pMa->pNFertilizer->fOrgManureCN;
		pCh->pCProfile->fNHumusSurf  += (float)0.1*pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCHumusSurf  += (float)0.1*pMa->pNFertilizer->fCorgManure;
		pCh->pCProfile->fCNHumusSurf += pMa->pNFertilizer->fOrgManureCN;


	InitOrgDuenger(exp_p);
	InfiltrationOrgDuenger(exp_p);

     #ifdef LOGFILE

      strcpy(lpOut,COMMENT_FERTILIZER_TXT); 
      
      if (pMa->pNFertilizer->fNO3N > 0)
      {
      itoa((int)pMa->pNFertilizer->fNO3N, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha NO3-N, "); 
      }                  
      
      if (pMa->pNFertilizer->fNH4N > 0)
      {
      itoa((int)pMa->pNFertilizer->fNH4N, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha NH4-N, "); 
      }                  
      
      if (pMa->pNFertilizer->fUreaN > 0)
      {
      itoa((int)pMa->pNFertilizer->fUreaN, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha Urea-N, "); 
      }                  
      
      if (pMa->pNFertilizer->fNorgManure > 0)
      {
      itoa((int)pMa->pNFertilizer->fNorgManure, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha org. N; "); 
      }                  


      Message(1, lpOut);

     #endif  	    

	}/* Ende if */
  }
	/*  */

	InfiltrationOrgDuengerRegen(exp_p);
	InfiltrationOrgDuengerBeregnung(exp_p);
	//MinerOrgDuengHoff(exp_p);
	//NitrOrgNH4Hoff(exp_p);

  return 1;
}      /*=====   Ende der Integration Duengung  =================*/


/*********************************************************************************/
/*  Name     : LAFertilizer                                                      */
/*                                                                               */
/*  Funktion : Die N-Zufuhr durch mineralische oder organische Duengemittel wird */
/*             nach Modellansaetzen von Godwin, die von Engel modifiziert wurden,*/
/*             integriert.                                                       */
/*                                                                               */
/*  Autor    : ch/gsf                                                            */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*				pCh->pCLayer->fNO3N                                              */
/*				pCh->pCLayer->fNH4N                                              */
/*				pCh->pCLayer->fUreaN                                             */
/*				pCh->pCLayer->fNManure                                           */
/*				pCh->pCLayer->fCManure                                           */
/*				pCh->pCLayer->fManureCN                                          */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI LAFertilizer(EXP_POINTER)
{                                                                   
DECLARE_COMMON_VAR
        
        
        char  acdummy2[80];
        char  acdummy3[80];
		float fumFacC;                  

		int   iDummy;
		float fLitterPart, fHumusPart;                  
		        
        LPSTR lpOut2       = acdummy2;
        LPSTR lpOut3       = acdummy3;

  if (pMa->pNFertilizer != NULL) 
	if (pTi->pSimTime->fTimeAct >= (float)pMa->pNFertilizer->iDay)
	{
                         
                         
	fumFacC = (float)0.4; // 40 % der org. Substanz im Duenger ist Kohlenstoff

	/*Faktor fumFacC wird in Abhaengugkeit von Duenger bestimmt. Bis
	  der Duenger identifiziert werden kann standardmaessig auf 0.4 gesetzt.  */
		pMa->pNFertilizer->fCorgManure = pMa->pNFertilizer->fDryMatter * fumFacC;	
		

		iDummy = 3 + strcspn(pMa->pNFertilizer->acName, "LA:");
		
		strcpy(lpOut3,pMa->pNFertilizer->acName);
		
		if (iDummy < (int)strlen(lpOut3))
		{
		lpOut3 += iDummy;
		
		fLitterPart = (float)atof(lpOut3)/(float)100;
        }
        else
   			fLitterPart = (float)0;

		fLitterPart = min((float)1, fLitterPart);
		fLitterPart = max((float)0, fLitterPart);
		

		iDummy = 3 + strcspn(lpOut3, "HA:");
		
		if (iDummy < (int)strlen(lpOut3))
		{
		lpOut3 += iDummy;
		
		fHumusPart = (float)atof(lpOut3)/(float)100;
		lpOut3 -= iDummy;
        }
        else
   			fHumusPart = (float)0;


		fHumusPart = min((float)1-fLitterPart, fHumusPart);
		fHumusPart = max((float)0, fHumusPart);
		
		
		
		/* 25.06.01 auch unnnoetig?
		if (pMa->pNFertilizer->fNorgManure > EPSILON) 
		{
		pMa->pNFertilizer->fOrgManureCN = pMa->pNFertilizer->fCorgManure / pMa->pNFertilizer->fNorgManure; 
		}
		else
		pMa->pNFertilizer->fOrgManureCN = (float)0.1;
		*/

	/*Uebertragen der Duengergroessen auf die Oberflaechenschicht.  */

		pCh->pCProfile->fNO3NSurf     += pMa->pNFertilizer->fNO3N;
		pCh->pCProfile->fNH4NSurf     += pMa->pNFertilizer->fNH4N;
		pCh->pCProfile->fUreaNSurf    += pMa->pNFertilizer->fUreaN;

		pCh->pCProfile->fNManureSurf  += ((float)1- fLitterPart- fHumusPart)* pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCManureSurf  += ((float)1- fLitterPart- fHumusPart)* pMa->pNFertilizer->fCorgManure;

		pCh->pCProfile->fNLitterSurf  += fLitterPart * pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCLitterSurf  += fLitterPart * pMa->pNFertilizer->fCorgManure;

		pCh->pCProfile->fNHumusSurf   += fHumusPart * pMa->pNFertilizer->fNorgManure;
		pCh->pCProfile->fCHumusSurf   += fHumusPart * pMa->pNFertilizer->fCorgManure;


		/* 19.8.97 unnötig: 
		if (pCh->pCProfile->fNManureSurf > EPSILON)
			{
			pCh->pCProfile->fCNManureSurf = pCh->pCProfile->fCManureSurf/ pCh->pCProfile->fNManureSurf;
			}
			else
				pCh->pCProfile->fCNManureSurf = (float)0.1;
		
		if (pCh->pCProfile->fNLitterSurf > EPSILON)
			{
			pCh->pCProfile->fCNLitterSurf = pCh->pCProfile->fCLitterSurf/ pCh->pCProfile->fNLitterSurf;
			}
			else
				pCh->pCProfile->fCNLitterSurf = (float)0.1;
		*/


     #ifdef LOGFILE

      strcpy(lpOut,COMMENT_FERTILIZER_TXT); 
      
      if (pMa->pNFertilizer->fNO3N > 0)
      {
      itoa((int)pMa->pNFertilizer->fNO3N, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha NO3-N, "); 
      }                  
      
      if (pMa->pNFertilizer->fNH4N > 0)
      {
      itoa((int)pMa->pNFertilizer->fNH4N, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha NH4-N, "); 
      }                  
      
      if (pMa->pNFertilizer->fUreaN > 0)
      {
      itoa((int)pMa->pNFertilizer->fUreaN, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha Urea-N, "); 
      }                  
      
      if ((pMa->pNFertilizer->fNorgManure > 0)&&((float)1 > fLitterPart + fHumusPart))
      {                                        
      iDummy = (int)(((float)1- fLitterPart -fHumusPart)*pMa->pNFertilizer->fNorgManure);
      itoa(iDummy, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha Manure-N, "); 
      }                  
      
      if ((pMa->pNFertilizer->fNorgManure > 0)&&((float)0 < fLitterPart))
      {
      iDummy = (int)(fLitterPart * pMa->pNFertilizer->fNorgManure);
      itoa(iDummy, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha Litter-N, "); 
      }                  
      
      if ((pMa->pNFertilizer->fNorgManure > 0)&&((float)0 < fHumusPart))
      {
      iDummy = (int)(fHumusPart * pMa->pNFertilizer->fNorgManure);
      itoa(iDummy, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," kg/ha Humus-N; "); 
      }                  
      
      Message(1, lpOut);

     #endif  	    

	 /*	//ep 100899 wird in balance.c erledigt?
	  if (pMa->pNFertilizer->pNext != NULL)
	  	{                         
	  	pMa->pNFertilizer = pMa->pNFertilizer->pNext;
	  	}
	  	else
	  	{
		pMa->pNFertilizer->iDay = 11111;
	  	}
	 */
    
	}/* Ende if */
	
	//InfiltrationOrgDuengerRegen(exp_p);
	//InfiltrationOrgDuengerBeregnung(exp_p);

  return 1;
}      /*=====   Ende der Integration Duengung  =================*/

/*********************************************************************************/
/*  Name     : TSFertilizerLoss                                                  */
/*                                                                               */
/*  Funktion : Nach der organischen Duengung entstehen gasfoermige Verluste,     */
/*             die durch eine Vielzahl von Prozessen beeinflusst werden. In      */
/*             dieser Funktion werden Ansaetze von Hoffmann&Ritchie integriert.  */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 05.02.98                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*				pCh->pCLayer->fNO3N                                              */
/*				pCh->pCLayer->fNH4N                                              */
/*				pCh->pCLayer->fUreaN                                             */
/*				pCh->pCLayer->fNManure                                           */
/*				pCh->pCLayer->fCManure                                           */
/*				pCh->pCLayer->fManureCN                                          */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSFertilizerLoss(EXP_POINTER)
{
 return 1;
}      /*=====   Ende der Integration Verluste Duengung  =================*/


/*********************************************************************************/
/*  Name     : TSIrrigation                                                      */
/*                                                                               */
/*  Funktion : Die Zufuhr von Wasser durch Beregnungsmassnahmen werden mit       */
/*             mit dieser Funktion beruecksichtigt.                              */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*				pCl->pWeather->fRainAmount                                       */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSIrrigation(EXP_POINTER)
{
 DECLARE_COMMON_VAR
      char  acdummy2[80];
      char  acdummy3[80];
		        
      LPSTR lpOut2 = acdummy2;
      LPSTR lpOut3 = acdummy3;

	  if (pMa->pIrrigation != NULL)
      {
	   if (pTi->pSimTime->fTimeAct >= (float)pMa->pIrrigation->iDay)
	   {
		pWa->pWBalance->fReservoir   += pMa->pIrrigation->fAmount;

		pCh->pCProfile->fNO3NSurf    += pMa->pIrrigation->fNO3N;
		pCh->pCProfile->fNH4NSurf    += pMa->pIrrigation->fNH4N;
		//pCh->pCProfile->fUreaNSurf += pMa->pIrrigation->fUreaN;
		pCh->pCProfile->fNManureSurf += pMa->pIrrigation->fNorgManure;
		pCh->pCProfile->fCManureSurf += pMa->pIrrigation->fCorgManure;
		pCh->pCProfile->fDONSurf     += pMa->pIrrigation->fDON;
		pCh->pCProfile->fDOCSurf     += pMa->pIrrigation->fDOC;

      #ifdef LOGFILE

      strcpy(lpOut,COMMENT_IRRIGATION_TXT); 
      
      if (pMa->pIrrigation->fAmount > 0)
      {
      itoa((int)pMa->pIrrigation->fAmount, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mm H2O, "); 
      }                  
      
      if (pMa->pIrrigation->fRate > 0)
      {
      itoa((int)pMa->pIrrigation->fRate, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mm/h H2O, "); 
      }                  

	  if (pMa->pIrrigation->fNO3N > 0)
      {
      itoa((int)pMa->pIrrigation->fNO3N, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mg/l NO3-N, "); 
      }                  
      
      if (pMa->pIrrigation->fNH4N > 0)
      {
      itoa((int)pMa->pIrrigation->fNH4N, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mg/l NH4-N, "); 
      }                  
      
      if (pMa->pIrrigation->fNorgManure > 0)
      {
      itoa((int)pMa->pIrrigation->fNorgManure, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mg/l Norg-N, "); 
      }                  
      
      if (pMa->pIrrigation->fCorgManure > 0)                                        
      {
      itoa((int)pMa->pIrrigation->fCorgManure, lpOut2, 10);
      strcat(lpOut,lpOut2); 
      strcat(lpOut," mg/l Corg-C, "); 
      }                  
      
      Message(1, lpOut);

     #endif  	    

	    /* Nach erfolgter Beregnung wird der Zeiger 
				auf die naechste Struktur gestellt. */
		pMa->pIrrigation = pMa->pIrrigation->pNext;		

	   }/* Ende if (datum == pMa->pIrrigation->lDate) Beregnungstermin */
      }
  return 1;
}      /*=====   Ende der Integration Beregnung  =================*/

/*********************************************************************************/
/*  Name     : TSIrrigationLoss                                                  */
/*                                                                               */
/*  Funktion :                                                                   */
/*                                                                               */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 :                                                                   */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*		                                                                         */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSIrrigationLoss(EXP_POINTER)
{


  return 1;
}      /*=====   Ende der Integration Beregnung  =================*/


/*********************************************************************************/
/*  Name     : InitBBGeraet                                                      */
/*                                                                               */
/*  Funktion : Die eingesetzten Bodenbearbeitungsgeraete werden bezueglich       */
/*             ihrer Lockerungs-, Misch- und Einarbeitungswirkung bewertet.      */
/*             Die bewerteten Parameter wurden im Modell EPIC definiert. Zur     */
/*             Integration wurden Veraenderungen nach Schaaf vorgenommen.        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*             pMa->pTillage->fEffMix                                            */
/*             pMa->pTillage->fEffLoose                                          */
/*             pMa->pTillage->fEffIncorp                                         */
/*                                                                               */
/*********************************************************************************/
signed short int InitBBGeraet(EXP_POINTER)
{

	PTILLAGE	pTI = pMa->pTillage;

	if (!lstrcmp(pTI->acCode,"TI047\0"))        
	{ //Schaelpflug
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.75;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI048\0"))        	
	{ //Scharpflug
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.75;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI005\0"))        	
	{ //Volldrehpflug
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.95;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI049\0"))        	
	{ //Scheibenpflug
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.75;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI007\0"))        	
	{ //Scheibenpflug
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.75;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI030\0"))        	
	{ //Schichtengubber
		pMa->pTillage->fEffMix = (float)0.6;
		pMa->pTillage->fEffLoose = (float)0.55;
		pMa->pTillage->fEffIncorp = (float)0.55;
	}
	if (!lstrcmp(pTI->acCode,"TI010\0"))        	
	{ //Schaelgrubber
		pMa->pTillage->fEffMix = (float)0.6;
		pMa->pTillage->fEffLoose = (float)0.55;
		pMa->pTillage->fEffIncorp = (float)0.35;
	}
	if (!lstrcmp(pTI->acCode,"TI012\0"))        	
	{ //Federzahnegge
		pMa->pTillage->fEffMix = (float)0.2;
		pMa->pTillage->fEffLoose = (float)0.05;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI037\0"))        	
	{ //Feingrubber
		pMa->pTillage->fEffMix = (float)0.2;
		pMa->pTillage->fEffLoose = (float)0.05;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI032\0"))        	
	{ //Spatenrollegge
		pMa->pTillage->fEffMix = (float)0.2;
		pMa->pTillage->fEffLoose = (float)0.05;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI042\0"))        	
	{ //Ackerfraese
		pMa->pTillage->fEffMix = (float)0.8;
		pMa->pTillage->fEffLoose = (float)0.75;
		pMa->pTillage->fEffIncorp = (float)0.64;
	}
	if (!lstrcmp(pTI->acCode,"TI036\0"))        	
	{ //Striegelegge
		pMa->pTillage->fEffMix = (float)0.2;
		pMa->pTillage->fEffLoose = (float)0.0;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI033\0"))        	
	{ //Loeffelegge
		pMa->pTillage->fEffMix = (float)0.5;
		pMa->pTillage->fEffLoose = (float)0.0;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI035\0"))        	
	{ //Ruettelegge
		pMa->pTillage->fEffMix = (float)0.7;
		pMa->pTillage->fEffLoose = (float)0.0;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI034\0"))        	
	{ //Kreiselegge
		pMa->pTillage->fEffMix = (float)0.7;
		pMa->pTillage->fEffLoose = (float)0.05;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI013\0"))        	
	{ //Saategge
		pMa->pTillage->fEffMix = (float)0.2;
		pMa->pTillage->fEffLoose = (float)0.0;
		pMa->pTillage->fEffIncorp = (float)0.1;
	}
	if (!lstrcmp(pTI->acCode,"TI031\0"))        	
	{ //Scheibenegge
		pMa->pTillage->fEffMix = (float)0.7;
		pMa->pTillage->fEffLoose = (float)0.5;
		pMa->pTillage->fEffIncorp = (float)0.5;
	}
	if (!lstrcmp(pTI->acCode,"TI043\0"))        	
	{ //Zinkenroter
		pMa->pTillage->fEffMix = (float)0.8;
		pMa->pTillage->fEffLoose = (float)0.64;
		pMa->pTillage->fEffIncorp = (float)0.64;
	}
	if (!lstrcmp(pTI->acCode,"TI015\0"))        	
	{ //Rotortiller
		pMa->pTillage->fEffMix = (float)0.8;
		pMa->pTillage->fEffLoose = (float)0.64;
		pMa->pTillage->fEffIncorp = (float)0.64;
	}
	if (!lstrcmp(pTI->acCode,"TI024\0"))        	
	{ //Pflug mit Untergrundlockerer
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.95;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI025\0"))        	
	{ // Volldrehpflug mit Packer
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.55;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI004\0"))        	
	{ // Beetpflug
		pMa->pTillage->fEffMix = (float)0.4;
		pMa->pTillage->fEffLoose = (float)0.65;
		pMa->pTillage->fEffIncorp = (float)0.75;
	}
	if (!lstrcmp(pTI->acCode,"TI027\0"))        	
	{ // Spatenmaschine
		pMa->pTillage->fEffMix = (float)0.3;
		pMa->pTillage->fEffLoose = (float)0.75;
		pMa->pTillage->fEffIncorp = (float)0.5;
	}
	if (!lstrcmp(pTI->acCode,"TI029\0"))
	{ // Schwergrubber
		pMa->pTillage->fEffMix = (float)0.65;
		pMa->pTillage->fEffLoose = (float)0.65;
		pMa->pTillage->fEffIncorp = (float)0.650;
	}
	if (!lstrcmp(pTI->acCode,"TI038\0"))        	
	{ // Schleppegge
		pMa->pTillage->fEffMix = (float)0.0;
		pMa->pTillage->fEffLoose = (float)-0.05;
		pMa->pTillage->fEffIncorp = (float)0.0;
	}
	if (!lstrcmp(pTI->acCode,"TI039\0"))        	
	{ // Cambridge-Walze
		pMa->pTillage->fEffMix = (float)0.0;
		pMa->pTillage->fEffLoose = (float)-0.5;
		pMa->pTillage->fEffIncorp = (float)0.0;
	}
	if (!lstrcmp(pTI->acCode,"TI040\0"))        	
	{ // Crosskill-Walze
		pMa->pTillage->fEffMix = (float)0.0;
		pMa->pTillage->fEffLoose = (float)-0.5;
		pMa->pTillage->fEffIncorp = (float)0.0;
	}
	if (!lstrcmp(pTI->acCode,"TI041\0"))        	
	{ // Ringel-Walze
		pMa->pTillage->fEffMix = (float)0.0;
		pMa->pTillage->fEffLoose = (float)-0.5;
		pMa->pTillage->fEffIncorp = (float)0.0;
	}
	if (!lstrcmp(pTI->acCode,"TI044\0"))        	
	{ // Dammfraese
		pMa->pTillage->fEffMix = (float)0.55;
		pMa->pTillage->fEffLoose = (float)0.65;
		pMa->pTillage->fEffIncorp = (float)0.15;
	}
	if (!lstrcmp(pTI->acCode,"TI045\0"))        	
	{ // Tiefenmeissel
		pMa->pTillage->fEffMix = (float)0.0;
		pMa->pTillage->fEffLoose = (float)-0.65;
		pMa->pTillage->fEffIncorp = (float)0.0;
	}
	if (!lstrcmp(pTI->acCode,"TI046\0"))        	
	{ // Mulchgeraet
		pMa->pTillage->fEffMix = (float)0.15;
		pMa->pTillage->fEffLoose = (float)0.0;
		pMa->pTillage->fEffIncorp = (float)0.25;
	}

      /* Wenn fuer das gewaehlte Geraet keine Zuordnung stattgefunden 
           hat wird es hier mit Standarwerten belegt. */
		if ((pMa->pTillage->fEffMix ==(float)0.0) &&
		   (pMa->pTillage->fEffLoose ==(float)0.0) &&
		   (pMa->pTillage->fEffIncorp ==(float)0.0))
		   {
			pMa->pTillage->fEffMix = (float)0.1;
			pMa->pTillage->fEffLoose = (float)0.1;
			pMa->pTillage->fEffIncorp = (float)0.1;
		   }
		
  return 1;
}      /*===== Ende der Initialisierung der Bodenbearbeitungsgeraete =============*/


/*********************************************************************************/
/*  Name     : LagerungNeu                                                       */
/*                                                                               */
/*  Funktion : Die Lagerungsdichte wird nach jeder Bodenbearbeitung neu be-      */
/*             rechnet. Die integrierten Funktionen sind dem Modell EPIC         */
/*             entnommen und wurden nach Schaaf geaendert.                        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int LagerungNeu(EXP_POINTER)
{
 	PSLAYER pSL;

	int		i1,anzahl;
	float   afanteilschicht[MAXSCHICHT];
	float   lagdichtevor[MAXSCHICHT];
	float   lagdichteEnt[MAXSCHICHT];
    float	bearbTiefe,aktTiefe,obereTiefe,anteil,lagletzteSchicht,diff,
    		lagvor,lagvoll,lockUnten,lockOben,differ;

		/* Berechnung des anteils eine Bodenschicht 
				an dem bearbeiteten Bodenvolumen. */
		bearbTiefe = pMa->pTillage->fDepth;
		aktTiefe =(float)0.0;

	    for (pSL = pSo->pSLayer->pNext,anzahl=0;
	                 (aktTiefe < bearbTiefe);
	         pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
	
			if (aktTiefe > bearbTiefe)
			{
			  anteil = (bearbTiefe - obereTiefe) / bearbTiefe;
			}
			else
			{
			  anteil = (pSL->fThickness / bearbTiefe);
			}
			afanteilschicht[anzahl] = anteil;
	    } 	/* Ende der Schleife zur Berechnung der Schichtanteile */


 	/* Berechnung der neuen Lagerungsdichte fuer die bearbeiteten 
 	   Simulationsschichten. Nur teilweise bearbeitete Bodenschichten
 	   werden gewichtet gelockert. */

 		/* Lokales Array wird initialisiert */
	    for (pSL = pSo->pSLayer->pNext,i1=0;
	                 (pSL->pNext->pNext != NULL);
	         pSL = pSL->pNext,i1++)
		{
			lagdichtevor[i1] = pSL->fBulkDens;                   
			lagletzteSchicht = pSL->fBulkDens;                   
		}/* Ende lokale Initialisierung */


	 	/* Berechnung der entgueltigen Lagerungsdichte derzeit lokal, 
	 		da kein globale Variable vorhanden. */
	    for (pSL = pSo->pSLayer->pNext,i1=0;
	                 (pSL->pNext->pNext != NULL);
	         pSL = pSL->pNext,i1++)
		{
			diff = ((lagletzteSchicht - pSL->fBulkDensStart) / (float ) 10.0);
			if (diff < (float)0.0) 
			{ diff = (float)0.0;}
			lagdichteEnt[i1] = diff;
		}/* Ende Initialisierung entgueltigen Lagerungsdichte*/

 		/* Veraenderung der bearbeiteten Bodenschichten. */
 	   
	    for (pSL = pSo->pSLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,i1++)
		{
 			/* Nur erste Bodenschicht wurde bearbeitet */
			if (i1 == (int)1)
            {
			  pSL->fBulkDens = (lagdichtevor[i1] - ((lagdichtevor[i1] 
			  				- (((float)2.0 * (pSL->fBulkDens + lagdichteEnt[i1])) / (float)3.0))
			  									 * pMa->pTillage->fEffLoose));

            }  /* Ende nur erste Schicht bearbeitet */
			else  /* Mehrere Bodenschicht wurden bearbeitet */
			{
			  if (i1 == anzahl) // Berechnung teilweise bearbeiteter Sim.schicht
			  {
			    /* Veraenderungen, die bei vollstaendiger Bearbeitung entstanden 
			       waeren werden nur anteilsmaessig realisiert */

				lagvor = lagdichtevor[i1];
				lagvoll =  (lagdichtevor[i1] - ((lagdichtevor[i1] 
			  				- (((float)2.0 * (pSL->fBulkDens + lagdichteEnt[i1])) / (float)3.0))
			  									 * pMa->pTillage->fEffLoose));
							  									 
				lockUnten = (pSL->fThickness * i1) - pMa->pTillage->fDepth;
				lockOben = pSL->fThickness - lockUnten;
				differ = ((lagvor - lagvoll)/ pSL->fThickness) * lockOben;
				pSL->fBulkDens = lagvor - differ;			
					
			  }/* Ende teilweise bearbeitete Sim.schicht */
			  else /* Berechnung Sim.schicht, die komplett bearbeitet wurden */
			  {
			    pSL->fBulkDens = (lagdichtevor[i1] - ((lagdichtevor[i1] 
			  	        			- (((float)2.0 * (pSL->fBulkDens + lagdichteEnt[i1])) / (float)3.0))
			  									 * pMa->pTillage->fEffLoose));
			  }/* Ende komplett bearbeiteter Sim.schicht */
			
			}  /* Ende mehrere Schichten bearbeitet */
			
		}   /* Ende Schleife der bearbeiteten Schichten bis Anzahl */


  return 1;
}      /*===== Ende der Neuberechnung der Lagerungsdichte =============*/
							


/*********************************************************************************/
/*  Name     : EinarbeitRueckstaende                                             */
/*                                                                               */
/*  Funktion : Bei der Durchfuehrung einer Bodenbearbeitung werden die an der    */
/*             Oberflaeche aufliegenden Materialien in den bearbeiteten Boden    */
/*             eingearbeitet. Diese Einarbeitung wird durch diese Funktion       */
/*             sichergestellt. Die Modellansaetze sind dem Modell EPIC entnommen */
/*             und wurden modifiziert integriert.                                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int EinarbeitRueckstaende(EXP_POINTER)
{
 PCPROFILE pCP = pCh->pCProfile;
// orig.:  PCLAYER   pCL = pCh->pCLayer;

 PCLAYER   pCL = pCh->pCLayer->pNext;
 PSLAYER   pSL = pSo->pSLayer->pNext;
 
 float	restMenge,tiefe,effektEinarbeitung; 
	
	restMenge = (float)0.0;	
	
	effektEinarbeitung = pMa->pTillage->fEffIncorp;					
	tiefe = pMa->pTillage->fDepth;
	
	if (pCP->fNO3NSurf >(float)0.0)
	{
		restMenge = pCP->fNO3NSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNO3N    += (pCP->fNO3NSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fNO3NSurf - restMenge);
		pCP->fNO3NSurf = restMenge;
	}

	if (pCP->fNH4NSurf >(float)0.0)
	{
		restMenge = pCP->fNH4NSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNH4N    += (pCP->fNH4NSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fNH4NSurf - restMenge);
		pCP->fNH4NSurf = restMenge;
	}

	if (pCP->fUreaNSurf >(float)0.0)
	{
		restMenge = pCP->fUreaNSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fUreaN    += (pCP->fUreaNSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fUreaNSurf - restMenge);
		pCP->fUreaNSurf = restMenge;
	}
	
	if (pCP->fNStandCropRes >(float)0.0)
	{
		restMenge = pCP->fNStandCropRes * (float)exp((float)-0.0569* tiefe
				      * (effektEinarbeitung * effektEinarbeitung));
		pCP->fNLitterSurf += (pCP->fNStandCropRes - restMenge);
		pCP->fNStandCropRes = restMenge;
	}

	if (pCP->fCStandCropRes >(float)0.0)
	{
		restMenge = pCP->fCStandCropRes * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCP->fCLitterSurf += (pCP->fCStandCropRes - restMenge);
		pCP->fCStandCropRes = restMenge;
	}


	if (pCP->fNManureSurf >(float)0.0)
	{
		restMenge = pCP->fNManureSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNManure    += (pCP->fNManureSurf - restMenge);
		pCP->fNManureSurf = restMenge;
	}

	if (pCP->fCManureSurf >(float)0.0)
	{
		restMenge = pCP->fCManureSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fCManure    += (pCP->fCManureSurf - restMenge);
		pCP->fCManureSurf = restMenge;
	}

	if (pCP->fNLitterSurf >(float)0.0)
	{
		restMenge = pCP->fNLitterSurf * (float)exp((float)-0.0569 * tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNLitter    += (pCP->fNLitterSurf - restMenge);
		pCP->fNLitterSurf = restMenge;						 
	}

	if (pCP->fCLitterSurf >(float)0.0)
	{
		restMenge = pCP->fCLitterSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fCLitter    += (pCP->fCLitterSurf - restMenge);
		pCP->fCLitterSurf = restMenge;
	}

	if (pCP->fNHumusSurf >(float)0.0)
	{
		restMenge = pCP->fNHumusSurf * (float)exp((float)-0.0569 * tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fNHumus    += (pCP->fNHumusSurf - restMenge);
		//SG 20161009: For DAISY model - 100% of fNorgManure that is partitioned to "fNHumusSurf" are SOM2 (see TSFertilizer, manage.c line 334-349)
		pCL->fNHumusFast   += (pCP->fNHumusSurf - restMenge);
		pCP->fNHumusSurf = restMenge;		
	}

	if (pCP->fCHumusSurf >(float)0.0)
	{
		restMenge = pCP->fCHumusSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fCHumus    += (pCP->fCHumusSurf - restMenge);
		pCP->fCHumusSurf = restMenge;

		//SG 20161009: For DAISY model - 100% of fCorgManure that is partitioned to "fCHumusSurf" are SOM2 (see TSFertilizer, manage.c line 334-349)
		pCL->fCHumusFast   += (pCP->fCHumusSurf - restMenge);

        pSL->fHumusCN    =pSL->fCHumus/pSL->fNHumus;
	}

  return 1;
}      /*===== Ende der Einarbeitung Ernterueckstaende =============*/


/*********************************************************************************/
/*  Name     : DurchMisch                                                        */
/*                                                                               */
/*  Funktion :                                                                   */
/*                                                                               */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int DurchMisch(EXP_POINTER)
{
						
 	PSLAYER pSL=pSo->pSLayer;
 	PCLAYER	pCL=pCh->pCLayer;

	int		i1,anzahl;
	float   afanteilschicht[MAXSCHICHT];
    float	bearbTiefe,aktTiefe,obereTiefe,anteilschicht,anteil,no3ges,
    		nh4ges,ureages,humCges,humNges,humges,litterCges,litterNges,manureNges,
    		manureCges,mischeffekt;

	//SG20161009 - Mixing of DAISY pools			
	float   FOMFastCges, FOMSlowCges, MicBiomDenitCges, MicBiomFastCges, MicBiomSlowCges, HumusFastCges, HumusSlowCges;
	float   FOMFastNges, FOMSlowNges, MicBiomDenitNges, MicBiomFastNges, MicBiomSlowNges, HumusFastNges, HumusSlowNges;


		mischeffekt = pMa->pTillage->fEffMix;
		
		no3ges     = (float)0.0;
		nh4ges     = (float)0.0;
		ureages    = (float)0.0;
		humges     = (float)0.0;
		humCges    = (float)0.0;
		humNges    = (float)0.0;
		litterCges = (float)0.0;
		litterNges = (float)0.0;
		manureNges = (float)0.0;
		manureCges = (float)0.0;
		//SG20161009 - Mixing of DAISY pools			
		FOMFastCges = (float)0.0;
		FOMFastNges = (float)0.0;
		FOMSlowCges = (float)0.0;
		FOMSlowNges = (float)0.0;
		MicBiomDenitCges = (float)0.0;
		MicBiomDenitNges = (float)0.0;
		MicBiomFastCges = (float)0.0;
		MicBiomFastNges = (float)0.0;
		MicBiomSlowCges = (float)0.0;
		MicBiomSlowNges = (float)0.0;
		HumusFastCges = (float)0.0;
		HumusFastNges = (float)0.0;
		HumusSlowCges = (float)0.0;
		HumusSlowNges = (float)0.0;

		/* Berechnung des anteils eine Bodenschicht 
				an dem bearbeiteten Bodenvolumen. */
		bearbTiefe = pMa->pTillage->fDepth;
		aktTiefe =(float)0.0;

	    for (anzahl=0,pSL = pSo->pSLayer->pNext;
	         ((pSL!=NULL)&&(aktTiefe < bearbTiefe));
	         pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
		
			if (aktTiefe > bearbTiefe)
			{
			  anteilschicht = (bearbTiefe - obereTiefe) / bearbTiefe;
			}
			else
			{
			  anteilschicht = (pSL->fThickness / bearbTiefe);
			}
			afanteilschicht[anzahl] = anteilschicht;
	    } 	/* Ende der Schleife zur Berechnung der Schichtanteile */


		/* Summierung des bis zur Einarbeitungstiefe vorhandenen Nitrat, Ammonium,
		    Menge und Stickstoff an frischer org. Substanz sowie Menge und Stickstoff
		    an Humus. Die letzte Schicht wird eventuell nur anteilsmaessig 
		    beruecksichtigt*/

		aktTiefe =(float)0.0;

	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
			if (aktTiefe < bearbTiefe)
			{ anteil = (float)1.0;}
			else
			{ anteil = (bearbTiefe - obereTiefe) / pSL->fThickness;
			}

			no3ges     += pCL->fNO3N * anteil;
			nh4ges     += pCL->fNH4N * anteil;
			ureages    += pCL->fUreaN * anteil;
			humges	   += pSL->fHumus * anteil;
			humCges    += pSL->fCHumus * anteil;
			humNges    += pSL->fNHumus * anteil;
			litterCges += pCL->fCLitter * anteil;
			litterNges += pCL->fNLitter * anteil;
			manureNges += pCL->fNManure * anteil;
			manureCges += pCL->fCManure * anteil;

			//SG20161009 - Mixing of DAISY pools			
			FOMFastCges += pCL->fCFOMFast * anteil;
			FOMFastNges += pCL->fNFOMFast * anteil;
			FOMSlowCges += pCL->fCFOMSlow * anteil;
			FOMSlowNges += pCL->fNFOMSlow * anteil;
			MicBiomDenitCges += pCL->fCMicBiomDenit * anteil;
			MicBiomDenitNges += pCL->fNMicBiomDenit * anteil;
			MicBiomFastCges += pCL->fCMicBiomFast * anteil;
			MicBiomFastNges += pCL->fNMicBiomFast * anteil;
			MicBiomSlowCges += pCL->fCMicBiomSlow * anteil;
			MicBiomSlowNges += pCL->fNMicBiomSlow * anteil;
			HumusFastCges += pCL->fCHumusFast * anteil;
			HumusFastNges += pCL->fNHumusFast * anteil;
			HumusSlowCges += pCL->fCHumusSlow * anteil;
			HumusSlowNges += pCL->fNHumusSlow * anteil;


        } 	/* Ende der Schleife zum Summieren */


		/* Gleichmaessige Verteilung der Gesamtmengen auf die betroffenen
		   Bodenschichten. Dabei wird der Mischeffekt des Bodenbearbeitungs-
		   geraetes beruecksichtigt. Die Verteilungsfunktion ist aus dem
		   Modell EPIC entnommen.  */


	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
			anteilschicht = afanteilschicht[i1-1];
		   if (i1 != anzahl)
		   {
			pCL->fNO3N    = (((float)1.0 - mischeffekt) * pCL->fNO3N) 
											+ (anteilschicht * mischeffekt * no3ges);
			pCL->fNH4N    = (((float)1.0 - mischeffekt) * pCL->fNH4N) 
											+ (anteilschicht * mischeffekt * nh4ges);
			pCL->fUreaN   = (((float)1.0 - mischeffekt) * pCL->fUreaN) 
											+ (anteilschicht * mischeffekt * ureages);
			pCL->fCLitter = (((float)1.0 - mischeffekt) * pCL->fCLitter) 
											+ (anteilschicht * mischeffekt * litterCges);
			pCL->fNLitter = (((float)1.0 - mischeffekt) * pCL->fNLitter) 
											+ (anteilschicht * mischeffekt * litterNges);
			pCL->fNManure = (((float)1.0 - mischeffekt) * pCL->fNManure) 
											+ (anteilschicht * mischeffekt * manureNges);
			pCL->fCManure = (((float)1.0 - mischeffekt) * pCL->fCManure) 
											+ (anteilschicht * mischeffekt * manureCges);
			pSL->fHumus   = (((float)1.0 - mischeffekt) * pSL->fHumus) 
											+ (anteilschicht * mischeffekt * humges);
			pSL->fCHumus  = (((float)1.0 - mischeffekt) * pSL->fCHumus) 
											+ (anteilschicht * mischeffekt * humCges);
			pSL->fNHumus  = (((float)1.0 - mischeffekt) * pSL->fNHumus) 
											+ (anteilschicht * mischeffekt * humNges);
            if(pSL->fNHumus) pSL->fHumusCN  = pSL->fCHumus/pSL->fNHumus;

			
			//SG20161009 - Mixing of DAISY pools			
			pCL->fCFOMFast = (((float)1.0 - mischeffekt) * pCL->fCFOMFast) 
											+ (anteilschicht * mischeffekt * FOMFastCges);
			pCL->fNFOMFast = (((float)1.0 - mischeffekt) * pCL->fNFOMFast) 
											+ (anteilschicht * mischeffekt * FOMFastNges);
			pCL->fCFOMSlow = (((float)1.0 - mischeffekt) * pCL->fCFOMSlow) 
											+ (anteilschicht * mischeffekt * FOMSlowCges);
			pCL->fNFOMSlow = (((float)1.0 - mischeffekt) * pCL->fNFOMSlow) 
											+ (anteilschicht * mischeffekt * FOMSlowNges);
			pCL->fCMicBiomDenit = (((float)1.0 - mischeffekt) * pCL->fCMicBiomDenit) 
											+ (anteilschicht * mischeffekt * MicBiomDenitCges);
			pCL->fNMicBiomDenit = (((float)1.0 - mischeffekt) * pCL->fNMicBiomDenit) 
											+ (anteilschicht * mischeffekt * MicBiomDenitNges);
			pCL->fCMicBiomFast = (((float)1.0 - mischeffekt) * pCL->fCMicBiomFast) 
											+ (anteilschicht * mischeffekt * MicBiomFastCges);
			pCL->fNMicBiomFast = (((float)1.0 - mischeffekt) * pCL->fNMicBiomFast) 
											+ (anteilschicht * mischeffekt * MicBiomFastNges);
			pCL->fCMicBiomSlow = (((float)1.0 - mischeffekt) * pCL->fCMicBiomSlow) 
											+ (anteilschicht * mischeffekt * MicBiomSlowCges);
			pCL->fNMicBiomSlow = (((float)1.0 - mischeffekt) * pCL->fNMicBiomSlow) 
											+ (anteilschicht * mischeffekt * MicBiomSlowNges);
			pCL->fCHumusFast = (((float)1.0 - mischeffekt) * pCL->fCHumusFast) 
											+ (anteilschicht * mischeffekt * HumusFastCges);
			pCL->fNHumusFast = (((float)1.0 - mischeffekt) * pCL->fNHumusFast) 
											+ (anteilschicht * mischeffekt * HumusFastNges);
			pCL->fCHumusSlow = (((float)1.0 - mischeffekt) * pCL->fCHumusSlow) 
											+ (anteilschicht * mischeffekt * HumusSlowCges);
			pCL->fNHumusSlow = (((float)1.0 - mischeffekt) * pCL->fNHumusSlow) 
											+ (anteilschicht * mischeffekt * HumusSlowNges);
				   } /* Ende der Schleife wenn akt. Schicht komplett bearbeitet */
		   else
		   {
			pCL->fNO3N    = (pCL->fNO3N * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNO3N * anteil ))
                                            +  (anteilschicht * mischeffekt * no3ges));
			pCL->fNH4N    = (pCL->fNH4N * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * (pCL->fNH4N * anteil ))
                                            +  (anteilschicht * mischeffekt * nh4ges));
			pCL->fUreaN   = (pCL->fUreaN * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fUreaN * anteil ))
                                            +  (anteilschicht * mischeffekt * ureages));
			pCL->fCLitter = (pCL->fCLitter * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterCges));
			pCL->fNLitter = (pCL->fNLitter * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterNges));
			pCL->fNManure = (pCL->fNManure * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureNges));
			pCL->fCManure = (pCL->fCManure * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureCges));
			
			//SG20161009 - Mixing of DAISY pools			
			pCL->fCFOMFast = (pCL->fCFOMFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMFast * anteil ))
                                            +  (anteilschicht * mischeffekt * FOMFastCges));
			pCL->fNFOMFast = (pCL->fNFOMFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNFOMFast * anteil ))
                                            +  (anteilschicht * mischeffekt * FOMFastNges));
			pCL->fCFOMSlow = (pCL->fCFOMSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * FOMSlowCges));
			pCL->fNFOMSlow = (pCL->fNFOMSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNFOMSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * FOMSlowNges));
			pCL->fCMicBiomDenit = (pCL->fCMicBiomDenit * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCMicBiomDenit * anteil ))
                                            +  (anteilschicht * mischeffekt * MicBiomDenitCges));
			pCL->fNMicBiomDenit = (pCL->fNMicBiomDenit * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNMicBiomDenit * anteil ))
                                            +  (anteilschicht * mischeffekt * MicBiomDenitNges));
			pCL->fCMicBiomFast = (pCL->fCMicBiomFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCMicBiomFast * anteil ))
                                            +  (anteilschicht * mischeffekt * MicBiomFastCges));
			pCL->fNMicBiomFast = (pCL->fNMicBiomFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNMicBiomFast * anteil ))
                                            +  (anteilschicht * mischeffekt * MicBiomFastNges));
			pCL->fCMicBiomSlow = (pCL->fCMicBiomSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCMicBiomSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * MicBiomSlowCges));
			pCL->fNMicBiomSlow = (pCL->fNMicBiomSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNMicBiomSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * MicBiomSlowNges));
			pCL->fCHumusFast = (pCL->fCHumusFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCHumusFast * anteil ))
                                            +  (anteilschicht * mischeffekt * HumusFastCges));
			pCL->fNHumusFast = (pCL->fNHumusFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNHumusFast * anteil ))
                                            +  (anteilschicht * mischeffekt * HumusFastNges));
			pCL->fCHumusSlow = (pCL->fCHumusSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCHumusSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * HumusSlowCges));
			pCL->fNHumusSlow = (pCL->fNHumusSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNHumusSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * HumusSlowNges));

			pSL->fHumus   = (pSL->fHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humges));
			pSL->fNHumus  = (pSL->fNHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fNHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humNges));
			pSL->fCHumus  = (pSL->fCHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fCHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humCges));
            if(pSL->fNHumus) pSL->fHumusCN  = pSL->fCHumus/pSL->fNHumus;

		   } /* Ende der Schleife wenn akt. Schicht teilweise bearbeitet */

        } 	/* Ende der Schleife zum Verteilen auf die Bodenschichten */

  return 1;
}      /*===== Ende der Berechnung der Durchmischung=============*/

/***********************************************************************************/
/*  Name       : EinarbeitRueckstaendeDNDC                                             */
/*                                                                                 */
/*  Funktion   : Bei der Durchfuehrung einer Bodenbearbeitung werden die an der    */
/*               Oberflaeche aufliegenden Materialien in den bearbeiteten Boden    */
/*               eingearbeitet. Diese Einarbeitung wird durch diese Funktion       */
/*               sichergestellt. Die Modellansaetze sind dem Modell EPIC entnommen */
/*               und wurden modifiziert integriert.                                */
/*  Autor      : Th. Schaaf                                                        */
/*  Datum	   : 12.08.96                                                          */
/*                                                                                 */
/*  Erweiterung: Berücksichtigung der C und N Pools mit festem CN Verhältnis       */  
/*  Autor      : A. Berkenkamp                                                     */
/*  Datum	   : 30.10.01                                                          */
/*  Change     : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                                 */
/*               pCL->fNO3N                 pCP->fNO3NSurf                         */
/*               pCL->fNH4N                 pCP->fNH4NSurf                         */
/*               pCL->fUreaN                pCP->fUreaNSurf                        */
/*               pCP->fCStandCropRes        pCP->fNStandCropRes                    */
/*               pCP->fCLitterSurf          pCP->fNLitterSurf                      */
/*               pCP->fCManureSurf          pCP->fNManureSurf                      */
/*               pCP->fCHumusSurf           pCP->fNHumusSurf                       */
/*               pCL->fCLitter              pCL->fNLitter                          */
/*               pCL->fCManure              pCL->fNManure                          */
/*               pCL->fCHumus               pCL->fNHumus                           */
/*               pCL->fCFOMVeryFast         pCL->fNFOMVeryFast                     */
/*               pCL->fCFOMFast             pCL->fNFOMFast                         */
/*               pCL->fCFOMSlow             pCL->fNFOMSlow                         */
/*               pCh->pCBalance->fNInputCum pCL->fHumusCN                          */
/*                                                                                 */
/***********************************************************************************/
signed short int EinarbeitRueckstaendeDNDC(EXP_POINTER)
{
 PCPROFILE pCP = pCh->pCProfile;
 // orig: PCLAYER   pCL = pCh->pCLayer;

 PCLAYER   pCL = pCh->pCLayer->pNext;
 PSLAYER   pSL = pSo->pSLayer->pNext;
 
 float NManureAmount = (float)0.0;
 float CManureAmount = (float)0.0;
 float NLitterAmount = (float)0.0;
 float CLitterAmount = (float)0.0;

 float fCFOMVF,fCFOMF,fCFOMS;
 float AddCN,fCFast,fNFast;

 float restMenge = (float)0.0;
 float tiefe = pMa->pTillage->fDepth;
 float effektEinarbeitung = pMa->pTillage->fEffIncorp; 
 
 const float fCN_AOM1   = (float) 200.0;
 const float fCN_AOM2   = (float)  50.0;	
 const float fCN_AOM3   = (float)   5.0;

	if (pCP->fNO3NSurf >(float)0.0)
    {
		restMenge = pCP->fNO3NSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNO3N    += (pCP->fNO3NSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fNO3NSurf - restMenge);
		pCP->fNO3NSurf = restMenge;
	}

	if (pCP->fNH4NSurf >(float)0.0)
	{
		restMenge = pCP->fNH4NSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNH4N    += (pCP->fNH4NSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fNH4NSurf - restMenge);
		pCP->fNH4NSurf = restMenge;
	}

	if (pCP->fUreaNSurf >(float)0.0)
	{
		restMenge = pCP->fUreaNSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fUreaN    += (pCP->fUreaNSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fUreaNSurf - restMenge);
		pCP->fUreaNSurf = restMenge;
	}
	
	if (pCP->fNStandCropRes >(float)0.0)
	{
		restMenge = pCP->fNStandCropRes * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCP->fNLitterSurf += (pCP->fNStandCropRes - restMenge);
    	pCP->fNStandCropRes       = restMenge;
	}

	if (pCP->fCStandCropRes >(float)0.0)
	{
		restMenge = pCP->fCStandCropRes  * (float)exp((float)-0.0569* tiefe
			      * (effektEinarbeitung * effektEinarbeitung));
		pCP->fCLitterSurf += (pCP->fCStandCropRes  - restMenge);
		pCP->fCStandCropRes      = restMenge;
	}

	if (pCP->fNManureSurf >(float)0.0)
	{
		restMenge = pCP->fNManureSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		NManureAmount     = pCP->fNManureSurf - restMenge;
		pCL->fNManure    += NManureAmount;
		pCP->fNManureSurf = restMenge;
	}

	if (pCP->fCManureSurf >(float)0.0)
	{
		restMenge = pCP->fCManureSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		CManureAmount     = pCP->fCManureSurf - restMenge;
		pCL->fCManure    += CManureAmount;
		pCP->fCManureSurf = restMenge;
	}

	if (pCP->fNLitterSurf >(float)0.0)
	{
		restMenge = pCP->fNLitterSurf * (float)exp((float)-0.0569 * tiefe
						  * (effektEinarbeitung * effektEinarbeitung));
		NLitterAmount     = pCP->fNLitterSurf - restMenge;
		pCL->fNLitter    += NLitterAmount;
		pCP->fNLitterSurf = restMenge;						 
	}

	if (pCP->fCLitterSurf >(float)0.0)
	{
	    restMenge = pCP->fCLitterSurf * (float)exp((float)-0.0569* tiefe
						  * (effektEinarbeitung * effektEinarbeitung));
		CLitterAmount     = pCP->fCLitterSurf - restMenge;
		pCL->fCLitter    += CLitterAmount;
		pCP->fCLitterSurf = restMenge;
	
	 }
	 

    if (pCP->fCHumusSurf >(float)0.0)
	{
		restMenge = pCP->fCHumusSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fCHumus    += (pCP->fCHumusSurf - restMenge);
		pCP->fCHumusSurf = restMenge;
	}
	
	if (pCP->fNHumusSurf >(float)0.0)
	{
		restMenge = pCP->fNHumusSurf * (float)exp((float)-0.0569 * tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fNHumus    += (pCP->fNHumusSurf - restMenge);
		pCP->fNHumusSurf = restMenge;
		pSL->fHumusCN = pSL->fCHumus/pSL->fNHumus;
	}

	
   	fCFast = CManureAmount + CLitterAmount;
	fNFast = NManureAmount + NLitterAmount;
	
	if(fNFast > EPSILON)
       AddCN = fCFast/fNFast;
    
    else 
  	   AddCN = (float)0.0;
   
  	if(AddCN >= fCN_AOM3 && AddCN < fCN_AOM2)
  	 {
 	  fCFOMVF = (fCFast * (1/AddCN - 1/fCN_AOM3)) / (1/fCN_AOM2 - 1/fCN_AOM3);
      fCFOMF  = fCFast - fCFOMVF;
 	  fCFOMS  = (float)0.0;	
 	 }

    if(AddCN >= fCN_AOM2 && AddCN < fCN_AOM1)
 	 {
      fCFOMF  = (fCFast * (1/AddCN - 1/fCN_AOM2)) / (1/fCN_AOM1 - 1/fCN_AOM2);
      fCFOMS  = fCFast - fCFOMF;
      fCFOMVF = (float)0.0;	
     }
  
    if ((AddCN>=(float)0) && (AddCN<=(float)5))
     {
	  fCFOMF  = (float)0.0;
      fCFOMS  = (float)0.0;
      fCFOMVF = (float)0.0;
     }

    if (AddCN > fCN_AOM1)
     {
      Message(3,RANGE_ERROR_TXT);
     }

	 pCL->fCFOMVeryFast += fCFOMVF;
     pCL->fCFOMFast += fCFOMF;
	 pCL->fCFOMSlow += fCFOMS;

  return 1;
}      /*===== Ende der Einarbeitung Ernterueckstaende =============*/


/*********************************************************************************/
/*  Name       : DurchMischDNDC                                                      */
/*                                                                               */
/*  Funktion   : Homogene Verteilung innerhalb der Bodenbearbeitungstiefe        */
/*               Der Modellansatz ist dem Modell EPIC entnommen                  */
/*                                                                               */
/*  Autor      : Th. Schaaf                                                      */
/*  Datum	   : 12.08.96                                                        */
/*                                                                               */
/*  Veränderung: Berücksichtigung der C und N Pools mit festem CN Verhältnis     */
/*                                                                               */
/*  Autor      : A. Berkenkamp                                                   */
/*  Datum	   : 30.10.01                                                        */
/*                                                                               */
/*  Change     : Dokumentation der veraenderten globalen Variablen               */
/*                                                                               */
/*               pCL->fNO3N          pCL->fNH4N                                  */
/*               pCL->fUreaN         pSL->fHumus                                 */
/*               pSL->fCHumus        pSL->fNHumus                                */
/*               pCL->fCLitter       pCL->fNLitter                               */
/*               pCL->fCManure       pCL->fNManure                               */
/*               pCL->fCFOMVeryFast  pCL->fCMicBiomFast                          */
/*               pCL->fCFOMFast      pCL->fCMicBiomSlow                          */
/*               pCL->fCFOMSlow      pSL->fHumusCN                               */
/*                                                                               */
/*********************************************************************************/
signed short int DurchMischDNDC(EXP_POINTER)
{
						
 	PSLAYER pSL;
 	PCLAYER	pCL;

	int		i1,anzahl;
	float   afanteilschicht[MAXSCHICHT];
    float	bearbTiefe,aktTiefe,obereTiefe,anteilschicht,anteil,no3ges,
    		nh4ges,ureages,humCges,humNges,humges,litterCges,litterNges,manureNges,
    		manureCges,mischeffekt;
    float CFOMVFges,CFOMFges,CFOMSges,CMicFges,CMicSges;

		mischeffekt = pMa->pTillage->fEffMix;
		
		no3ges     = (float)0.0;
		nh4ges     = (float)0.0;
		ureages    = (float)0.0;
		humges     = (float)0.0;
		humCges    = (float)0.0;
		humNges    = (float)0.0;
		litterCges = (float)0.0;
		litterNges = (float)0.0;
		manureNges = (float)0.0;
		manureCges = (float)0.0;
	
		CFOMVFges  = (float)0.0;
		CFOMFges   = (float)0.0;
		CFOMSges   = (float)0.0;
		CMicFges   = (float)0.0;
		CMicSges   = (float)0.0;
	
	   /* Berechnung des anteils eine Bodenschicht 
				an dem bearbeiteten Bodenvolumen. */
		bearbTiefe = pMa->pTillage->fDepth;
		aktTiefe =(float)0.0;

	    for (pSL = pSo->pSLayer->pNext,anzahl=0;
	                 (aktTiefe < bearbTiefe);
	         pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
		
			if (aktTiefe > bearbTiefe)
			{
			  anteilschicht = (bearbTiefe - obereTiefe) / bearbTiefe;
			}
			else
			{
			  anteilschicht = (pSL->fThickness / bearbTiefe);
			}
			afanteilschicht[anzahl] = anteilschicht;
	    } 	/* Ende der Schleife zur Berechnung der Schichtanteile */


		/* Summierung des bis zur Einarbeitungstiefe vorhandenen Nitrat, Ammonium,
		    Menge und Stickstoff an frischer org. Substanz sowie Menge und Stickstoff
		    an Humus. Die letzte Schicht wird eventuell nur anteilsmaessig 
		    beruecksichtigt*/

		aktTiefe =(float)0.0;

	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
			if (aktTiefe < bearbTiefe)
			{ anteil = (float)1.0;}
			else
			{ anteil = (bearbTiefe - obereTiefe) / pSL->fThickness;
			}

			no3ges     += pCL->fNO3N * anteil;
			nh4ges     += pCL->fNH4N * anteil;
			ureages    += pCL->fUreaN * anteil;
			humges	   += pSL->fHumus * anteil;
			humCges    += pSL->fCHumus * anteil;
			humNges    += pSL->fNHumus * anteil;
			litterCges += pCL->fCLitter * anteil;
			litterNges += pCL->fNLitter * anteil;
			manureNges += pCL->fNManure * anteil;
			manureCges += pCL->fCManure * anteil;
			CFOMVFges  += pCL->fCFOMVeryFast * anteil;  
            CFOMFges   += pCL->fCFOMFast * anteil;  
            CFOMSges   += pCL->fCFOMSlow * anteil;
            CMicFges   += pCL->fCMicBiomFast * anteil;
            CMicSges   += pCL->fCMicBiomSlow * anteil;
		} 	/* Ende der Schleife zum Summieren */


		/* Gleichmaessige Verteilung der Gesamtmengen auf die betroffenen
		   Bodenschichten. Dabei wird der Mischeffekt des Bodenbearbeitungs-
		   geraetes beruecksichtigt. Die Verteilungsfunktion ist aus dem
		   Modell EPIC entnommen.  */


	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
			anteilschicht = afanteilschicht[i1-1];
		   if (i1 != anzahl)
		   {
			pCL->fNO3N    = (((float)1.0 - mischeffekt) * pCL->fNO3N) 
											+ (anteilschicht * mischeffekt * no3ges);
			pCL->fNH4N    = (((float)1.0 - mischeffekt) * pCL->fNH4N) 
											+ (anteilschicht * mischeffekt * nh4ges);
			pCL->fUreaN   = (((float)1.0 - mischeffekt) * pCL->fUreaN) 
											+ (anteilschicht * mischeffekt * ureages);
			pCL->fCLitter = (((float)1.0 - mischeffekt) * pCL->fCLitter) 
											+ (anteilschicht * mischeffekt * litterCges);
			pCL->fNLitter = (((float)1.0 - mischeffekt) * pCL->fNLitter) 
											+ (anteilschicht * mischeffekt * litterNges);
			pCL->fNManure = (((float)1.0 - mischeffekt) * pCL->fNManure) 
											+ (anteilschicht * mischeffekt * manureNges);
			pCL->fCManure = (((float)1.0 - mischeffekt) * pCL->fCManure) 
											+ (anteilschicht * mischeffekt * manureCges);
			pSL->fHumus   = (((float)1.0 - mischeffekt) * pSL->fHumus) 
											+ (anteilschicht * mischeffekt * humges);
			pSL->fCHumus  = (((float)1.0 - mischeffekt) * pSL->fCHumus) 
											+ (anteilschicht * mischeffekt * humCges);
			pSL->fNHumus  = (((float)1.0 - mischeffekt) * pSL->fNHumus) 
											+ (anteilschicht * mischeffekt * humNges);		
			pCL->fCFOMVeryFast = (((float)1.0 - mischeffekt) * pCL->fCFOMVeryFast) 
											+ (anteilschicht * mischeffekt * CFOMVFges);	
            pCL->fCFOMFast  = (((float)1.0 - mischeffekt) * pCL->fCFOMFast) 
											+ (anteilschicht * mischeffekt * CFOMFges);	
            pCL->fCFOMSlow  = (((float)1.0 - mischeffekt) * pCL->fCFOMSlow) 
											+ (anteilschicht * mischeffekt * CFOMSges);	
   			pCL->fCMicBiomFast = (((float)1.0 - mischeffekt) * pCL->fCMicBiomFast) 
											+ (anteilschicht * mischeffekt * CMicFges);	

			pCL->fCMicBiomSlow = (((float)1.0 - mischeffekt) * pCL->fCMicBiomSlow) 
											+ (anteilschicht * mischeffekt * CMicSges);	
			
			/* Neuberechnung des Humus CN Verhältnisses */			
			if(pSL->fNHumus)
    		  pSL->fHumusCN = pSL->fCHumus/pSL->fNHumus;

		   } /* Ende der Schleife wenn akt. Schicht komplett bearbeitet */
		   else
		   {
			pCL->fNO3N    = (pCL->fNO3N * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNO3N * anteil ))
                                            +  (anteilschicht * mischeffekt * no3ges));
			pCL->fNH4N    = (pCL->fNH4N * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * (pCL->fNH4N * anteil ))
                                            +  (anteilschicht * mischeffekt * nh4ges));
			pCL->fUreaN   = (pCL->fUreaN * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fUreaN * anteil ))
                                            +  (anteilschicht * mischeffekt * ureages));
			pCL->fCLitter = (pCL->fCLitter * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterCges));
			pCL->fNLitter = (pCL->fNLitter * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterNges));
			pCL->fNManure = (pCL->fNManure * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureNges));
			pCL->fCManure = (pCL->fCManure * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureCges));
			pSL->fHumus   = (pSL->fHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humges));
			pSL->fNHumus  = (pSL->fNHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fNHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humNges));
			pSL->fCHumus  = (pSL->fCHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fCHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humCges));
            pCL->fCFOMVeryFast  = (pCL->fCFOMVeryFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMVeryFast * anteil ))
                                            +  (anteilschicht * mischeffekt * CFOMVFges));
			pCL->fCFOMFast  = (pCL->fCFOMFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMFast * anteil ))
                                            +  (anteilschicht * mischeffekt * CFOMFges));
			pCL->fCFOMSlow  = (pCL->fCFOMSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * CFOMSges));
		    pCL->fCMicBiomFast  = (pCL->fCMicBiomFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCMicBiomFast * anteil ))
											+  (anteilschicht * mischeffekt * CMicFges));
			pCL->fCMicBiomSlow  = (pCL->fCMicBiomSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCMicBiomSlow * anteil ))
											+  (anteilschicht * mischeffekt * CMicSges));
			
			/* Neuberechnung des Humus CN Verhältnisses */			
			if(pSL->fNHumus)
    		  pSL->fHumusCN = pSL->fCHumus/pSL->fNHumus;
		   } /* Ende der Schleife wenn akt. Schicht teilweise bearbeitet */

        } 	/* Ende der Schleife zum Verteilen auf die Bodenschichten */

  return 1;
}      /*===== Ende der Berechnung der Durchmischung=============*/

/***********************************************************************************/
/*  Name       : EinarbeitRueckstaende                                             */
/*                                                                                 */
/*  Funktion   : Bei der Durchfuehrung einer Bodenbearbeitung werden die an der    */
/*               Oberflaeche aufliegenden Materialien in den bearbeiteten Boden    */
/*               eingearbeitet. Diese Einarbeitung wird durch diese Funktion       */
/*               sichergestellt. Die Modellansaetze sind dem Modell EPIC entnommen */
/*               und wurden modifiziert integriert.                                */
/*  Autor      : Th. Schaaf                                                        */
/*  Datum	   : 12.08.96                                                          */
/*                                                                                 */
/*  Erweiterung: Berücksichtigung der C und N Pools mit festem CN Verhältnis       */  
/*  Autor      : A. Berkenkamp                                                     */
/*  Datum	   : 30.10.01                                                          */
/*  Change     : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                                 */
/*               pCL->fNO3N                 pCP->fNO3NSurf                         */
/*               pCL->fNH4N                 pCP->fNH4NSurf                         */
/*               pCL->fUreaN                pCP->fUreaNSurf                        */
/*               pCP->fCStandCropRes        pCP->fNStandCropRes                    */
/*               pCP->fCLitterSurf          pCP->fNLitterSurf                      */
/*               pCP->fCManureSurf          pCP->fNManureSurf                      */
/*               pCP->fCHumusSurf           pCP->fNHumusSurf                       */
/*               pCL->fCLitter              pCL->fNLitter                          */
/*               pCL->fCManure              pCL->fNManure                          */
/*               pCL->fCHumus               pCL->fNHumus                           */
/*               pCL->fCFOMVeryFast         pCL->fNFOMVeryFast                     */
/*               pCL->fCFOMFast             pCL->fNFOMFast                         */
/*               pCL->fCFOMSlow             pCL->fNFOMSlow                         */
/*               pCh->pCBalance->fNInputCum pCL->fHumusCN                          */
/*                                                                                 */
/***********************************************************************************/
signed short int EinarbeitRueckstaendeCERES(EXP_POINTER)
{
 PCPROFILE pCP = pCh->pCProfile;
 // orig: PCLAYER   pCL = pCh->pCLayer;

 PCLAYER   pCL = pCh->pCLayer->pNext;
 PSLAYER   pSL = pSo->pSLayer->pNext;
 
 float NManureAmount = (float)0.0;
 float CManureAmount = (float)0.0;
 float NLitterAmount = (float)0.0;
 float CLitterAmount = (float)0.0;
 float fCFast,fNFast;

 float restMenge = (float)0.0;
 float tiefe = pMa->pTillage->fDepth;
 float effektEinarbeitung = pMa->pTillage->fEffIncorp; 
 

	if (pCP->fNO3NSurf >(float)0.0)
    {
		restMenge = pCP->fNO3NSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNO3N    += (pCP->fNO3NSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fNO3NSurf - restMenge);
		pCP->fNO3NSurf = restMenge;
	}

	if (pCP->fNH4NSurf >(float)0.0)
	{
		restMenge = pCP->fNH4NSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fNH4N    += (pCP->fNH4NSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fNH4NSurf - restMenge);
		pCP->fNH4NSurf = restMenge;
	}

	if (pCP->fUreaNSurf >(float)0.0)
	{
		restMenge = pCP->fUreaNSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCL->fUreaN    += (pCP->fUreaNSurf - restMenge);
		pCh->pCBalance->dNInputCum += (double)(pCP->fUreaNSurf - restMenge);
		pCP->fUreaNSurf = restMenge;
	}
	
	if (pCP->fNStandCropRes >(float)0.0)
	{
		restMenge = pCP->fNStandCropRes * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pCP->fNLitterSurf += (pCP->fNStandCropRes - restMenge);
    	pCP->fNStandCropRes       = restMenge;
	}

	if (pCP->fCStandCropRes >(float)0.0)
	{
		restMenge = pCP->fCStandCropRes  * (float)exp((float)-0.0569* tiefe
			      * (effektEinarbeitung * effektEinarbeitung));
		pCP->fCLitterSurf += (pCP->fCStandCropRes  - restMenge);
		pCP->fCStandCropRes      = restMenge;
	}

	if (pCP->fNManureSurf >(float)0.0)
	{
		restMenge = pCP->fNManureSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		NManureAmount     = pCP->fNManureSurf - restMenge;
		pCL->fNManure    += NManureAmount;
		pCP->fNManureSurf = restMenge;
	}

	if (pCP->fCManureSurf >(float)0.0)
	{
		restMenge = pCP->fCManureSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		CManureAmount     = pCP->fCManureSurf - restMenge;
		pCL->fCManure    += CManureAmount;
		pCP->fCManureSurf = restMenge;
	}

	if (pCP->fNLitterSurf >(float)0.0)
	{
		restMenge = pCP->fNLitterSurf * (float)exp((float)-0.0569 * tiefe
						  * (effektEinarbeitung * effektEinarbeitung));
		NLitterAmount     = pCP->fNLitterSurf - restMenge;
		pCL->fNLitter    += NLitterAmount;
		pCP->fNLitterSurf = restMenge;						 
	}

	if (pCP->fCLitterSurf >(float)0.0)
	{
	    restMenge = pCP->fCLitterSurf * (float)exp((float)-0.0569* tiefe
						  * (effektEinarbeitung * effektEinarbeitung));
		CLitterAmount     = pCP->fCLitterSurf - restMenge;
		pCL->fCLitter    += CLitterAmount;
		pCP->fCLitterSurf = restMenge;
	
	 }
	 

    if (pCP->fCHumusSurf >(float)0.0)
	{
		restMenge = pCP->fCHumusSurf * (float)exp((float)-0.0569* tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fCHumus    += (pCP->fCHumusSurf - restMenge);
		pCP->fCHumusSurf = restMenge;
	}
	
	if (pCP->fNHumusSurf >(float)0.0)
	{
		restMenge = pCP->fNHumusSurf * (float)exp((float)-0.0569 * tiefe
						 * (effektEinarbeitung * effektEinarbeitung));
		pSL->fNHumus    += (pCP->fNHumusSurf - restMenge);
		pCP->fNHumusSurf = restMenge;
		pSL->fHumusCN = pSL->fCHumus/pSL->fNHumus;
	}

	/* Veränderung in den Pools */
   	fCFast = CManureAmount + CLitterAmount;
	fNFast = NManureAmount + NLitterAmount;

	/* 10 % Lignin */ 
	pCL->fCFOMSlow     += (float)0.1 * fCFast;
	pCL->fNFOMSlow     += (float)0.1 * fNFast;

	/* 70 % Cellulose */ 
	pCL->fCFOMFast     += (float)0.7 * fCFast;
	pCL->fNFOMFast     += (float)0.7 * fNFast;

	/* 20 % Kohlenhydrate */
	pCL->fCFOMVeryFast += (float)0.2 * fCFast;
    pCL->fNFOMVeryFast += (float)0.2 * fNFast;

  return 1;
}      /*===== Ende der Einarbeitung Ernterueckstaende =============*/


/*********************************************************************************/
/*  Name       : DurchMisch                                                      */
/*                                                                               */
/*  Funktion   : Homogene Verteilung innerhalb der Bodenbearbeitungstiefe        */
/*               Der Modellansatz ist dem Modell EPIC entnommen                  */
/*                                                                               */
/*  Autor      : Th. Schaaf                                                      */
/*  Datum	   : 12.08.96                                                        */
/*                                                                               */
/*  Veränderung: Berücksichtigung der C und N Pools mit festem CN Verhältnis     */
/*                                                                               */
/*  Autor      : A. Berkenkamp                                                   */
/*  Datum	   : 30.10.01                                                        */
/*                                                                               */
/*  Change     : Dokumentation der veraenderten globalen Variablen               */
/*                                                                               */
/*               pCL->fNO3N          pCL->fNH4N                                  */
/*               pCL->fUreaN         pSL->fHumus                                 */
/*               pSL->fCHumus        pSL->fNHumus                                */
/*               pCL->fCLitter       pCL->fNLitter                               */
/*               pCL->fCManure       pCL->fNManure                               */
/*               pCL->fCFOMVeryFast  pCL->fCMicBiomFast                          */
/*               pCL->fCFOMFast      pCL->fCMicBiomSlow                          */
/*               pCL->fCFOMSlow      pSL->fHumusCN                               */
/*                                                                               */
/*********************************************************************************/
signed short int DurchMischCERES(EXP_POINTER)
{
						
 	PSLAYER pSL;
 	PCLAYER	pCL;

	int		i1,anzahl;
	float   afanteilschicht[MAXSCHICHT];
    float	bearbTiefe,aktTiefe,obereTiefe,anteilschicht,anteil,no3ges,
    		nh4ges,ureages,humCges,humNges,humges,litterCges,litterNges,manureNges,
    		manureCges,mischeffekt;
    float   CFOMVFges,CFOMFges,CFOMSges;

		mischeffekt = pMa->pTillage->fEffMix;
		
		no3ges     = (float)0.0;
		nh4ges     = (float)0.0;
		ureages    = (float)0.0;
		humges     = (float)0.0;
		humCges    = (float)0.0;
		humNges    = (float)0.0;
		litterCges = (float)0.0;
		litterNges = (float)0.0;
		manureNges = (float)0.0;
		manureCges = (float)0.0;
	
		CFOMVFges  = (float)0.0;
		CFOMFges   = (float)0.0;
		CFOMSges   = (float)0.0;
	
	   /* Berechnung des anteils eine Bodenschicht 
				an dem bearbeiteten Bodenvolumen. */
		bearbTiefe = pMa->pTillage->fDepth;
		aktTiefe =(float)0.0;

	    for (pSL = pSo->pSLayer->pNext,anzahl=0;
	                 (aktTiefe < bearbTiefe);
	         pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
		
			if (aktTiefe > bearbTiefe)
			{
			  anteilschicht = (bearbTiefe - obereTiefe) / bearbTiefe;
			}
			else
			{
			  anteilschicht = (pSL->fThickness / bearbTiefe);
			}
			afanteilschicht[anzahl] = anteilschicht;
	    } 	/* Ende der Schleife zur Berechnung der Schichtanteile */


		/* Summierung des bis zur Einarbeitungstiefe vorhandenen Nitrat, Ammonium,
		    Menge und Stickstoff an frischer org. Substanz sowie Menge und Stickstoff
		    an Humus. Die letzte Schicht wird eventuell nur anteilsmaessig 
		    beruecksichtigt*/

		aktTiefe =(float)0.0;

	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
			if (aktTiefe < bearbTiefe)
			{ anteil = (float)1.0;}
			else
			{ anteil = (bearbTiefe - obereTiefe) / pSL->fThickness;
			}

			no3ges     += pCL->fNO3N * anteil;
			nh4ges     += pCL->fNH4N * anteil;
			ureages    += pCL->fUreaN * anteil;
			humges	   += pSL->fHumus * anteil;
			humCges    += pSL->fCHumus * anteil;
			humNges    += pSL->fNHumus * anteil;
			litterCges += pCL->fCLitter * anteil;
			litterNges += pCL->fNLitter * anteil;
			manureNges += pCL->fNManure * anteil;
			manureCges += pCL->fCManure * anteil;
			CFOMVFges  += pCL->fCFOMVeryFast * anteil;  
            CFOMFges   += pCL->fCFOMFast * anteil;  
            CFOMSges   += pCL->fCFOMSlow * anteil;
		} 	/* Ende der Schleife zum Summieren */


		/* Gleichmaessige Verteilung der Gesamtmengen auf die betroffenen
		   Bodenschichten. Dabei wird der Mischeffekt des Bodenbearbeitungs-
		   geraetes beruecksichtigt. Die Verteilungsfunktion ist aus dem
		   Modell EPIC entnommen.  */


	    for (pSL = pSo->pSLayer->pNext,
	    	 pCL = pCh->pCLayer->pNext,i1=1;
	                 (i1 <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i1++)
		{
			anteilschicht = afanteilschicht[i1-1];
		   if (i1 != anzahl)
		   {
			pCL->fNO3N    = (((float)1.0 - mischeffekt) * pCL->fNO3N) 
											+ (anteilschicht * mischeffekt * no3ges);
			pCL->fNH4N    = (((float)1.0 - mischeffekt) * pCL->fNH4N) 
											+ (anteilschicht * mischeffekt * nh4ges);
			pCL->fUreaN   = (((float)1.0 - mischeffekt) * pCL->fUreaN) 
											+ (anteilschicht * mischeffekt * ureages);
			pCL->fCLitter = (((float)1.0 - mischeffekt) * pCL->fCLitter) 
											+ (anteilschicht * mischeffekt * litterCges);
			pCL->fNLitter = (((float)1.0 - mischeffekt) * pCL->fNLitter) 
											+ (anteilschicht * mischeffekt * litterNges);
			pCL->fNManure = (((float)1.0 - mischeffekt) * pCL->fNManure) 
											+ (anteilschicht * mischeffekt * manureNges);
			pCL->fCManure = (((float)1.0 - mischeffekt) * pCL->fCManure) 
											+ (anteilschicht * mischeffekt * manureCges);
			pSL->fHumus   = (((float)1.0 - mischeffekt) * pSL->fHumus) 
											+ (anteilschicht * mischeffekt * humges);
			pSL->fCHumus  = (((float)1.0 - mischeffekt) * pSL->fCHumus) 
											+ (anteilschicht * mischeffekt * humCges);
			pSL->fNHumus  = (((float)1.0 - mischeffekt) * pSL->fNHumus) 
											+ (anteilschicht * mischeffekt * humNges);		
			pCL->fCFOMVeryFast = (((float)1.0 - mischeffekt) * pCL->fCFOMVeryFast) 
											+ (anteilschicht * mischeffekt * CFOMVFges);	
            pCL->fCFOMFast  = (((float)1.0 - mischeffekt) * pCL->fCFOMFast) 
											+ (anteilschicht * mischeffekt * CFOMFges);	
            pCL->fCFOMSlow  = (((float)1.0 - mischeffekt) * pCL->fCFOMSlow) 
											+ (anteilschicht * mischeffekt * CFOMSges);	
			
			/* Neuberechnung des Humus CN Verhältnisses */			
			if(pSL->fNHumus)
    		  pSL->fHumusCN = pSL->fCHumus/pSL->fNHumus;

		   } /* Ende der Schleife wenn akt. Schicht komplett bearbeitet */
		   else
		   {
			pCL->fNO3N    = (pCL->fNO3N * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNO3N * anteil ))
                                            +  (anteilschicht * mischeffekt * no3ges));
			pCL->fNH4N    = (pCL->fNH4N * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * (pCL->fNH4N * anteil ))
                                            +  (anteilschicht * mischeffekt * nh4ges));
			pCL->fUreaN   = (pCL->fUreaN * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fUreaN * anteil ))
                                            +  (anteilschicht * mischeffekt * ureages));
			pCL->fCLitter = (pCL->fCLitter * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterCges));
			pCL->fNLitter = (pCL->fNLitter * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNLitter * anteil ))
                                            +  (anteilschicht * mischeffekt * litterNges));
			pCL->fNManure = (pCL->fNManure * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fNManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureNges));
			pCL->fCManure = (pCL->fCManure * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCManure * anteil ))
                                            +  (anteilschicht * mischeffekt * manureCges));
			pSL->fHumus   = (pSL->fHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humges));
			pSL->fNHumus  = (pSL->fNHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fNHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humNges));
			pSL->fCHumus  = (pSL->fCHumus * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pSL->fCHumus * anteil ))
                                            +  (anteilschicht * mischeffekt * humCges));
            pCL->fCFOMVeryFast  = (pCL->fCFOMVeryFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMVeryFast * anteil ))
                                            +  (anteilschicht * mischeffekt * CFOMVFges));
			pCL->fCFOMFast  = (pCL->fCFOMFast * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMFast * anteil ))
                                            +  (anteilschicht * mischeffekt * CFOMFges));
			pCL->fCFOMSlow  = (pCL->fCFOMSlow * ((float)1.0 - anteil ))
                                 +  (( (1 - mischeffekt) * ( pCL->fCFOMSlow * anteil ))
                                            +  (anteilschicht * mischeffekt * CFOMSges));
			
			/* Neuberechnung des Humus CN Verhältnisses */			
			if(pSL->fNHumus)
    		  pSL->fHumusCN = pSL->fCHumus/pSL->fNHumus;
		   } /* Ende der Schleife wenn akt. Schicht teilweise bearbeitet */

        } 	/* Ende der Schleife zum Verteilen auf die Bodenschichten */

  return 1;
}      /*===== Ende der Berechnung der Durchmischung=============*/


/*************************************************************************************/
/* Procedur    :   DNDC-Tillage                                                      */
/* Beschreibung:   Bodenbearbeitung nach dem Modell DNDC                             */
/*                 Grundlage Li et al. 1992                                          */
/*                                                                                   */
/*				   GSF/ab  Axel Berkenkamp         25.10.01                             */
/*                                                                                   */
/*************************************************************************************/
/*	veränd. Var.    pCL->fCHumusSlow  pCL->fNHumusSlow								 */
/*					pCL->fCHumusFast  pCL->fNHumusFast								 */
/*                                                                                   */
/*************************************************************************************/
signed short int DNDCTillage(EXP_POINTER)
{
 PSLAYER pSL;
 PCLAYER pCL;

 int	i,anzahl;
 static int FunktionDurchlaufen = 0;
 int Regentag = 0;
 static float tillfac = (float)0.16;
 float  bearbTiefe,aktTiefe,obereTiefe,TeilC,TeilN,anteil;

 if(NewDay(pTi))
 {
 if(FunktionDurchlaufen >= 1 && FunktionDurchlaufen < 4)
  {   
   if (pCl->pWeather->fRainAmount > (float) 0.0)
	 {
 	  Regentag++;
	  tillfac -= (float)0.04;
     }
  }

 if ((float)pTi->pSimTime->fTimeAct >= (float)pMa->pTillage->iDay
    || Regentag > 0)
 {
   bearbTiefe = pMa->pTillage->fDepth;
   aktTiefe   = (float)0.0;

   /* Berechnung Anzahl an betroffenden Simulationsschichten und
      des Anteils einer eventuell nur teilweise betroffenen Schicht*/
   
   for (pSL = pSo->pSLayer->pNext,anzahl=0;
       (aktTiefe < bearbTiefe);
        pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
		}
       anteil = (bearbTiefe - obereTiefe) / pSL->fThickness;

    	
   for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext,i=1;
                (i <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i++)
		{
		  if (i != anzahl)
		   {
			TeilC = tillfac * pCL->fCHumusSlow;
		    TeilN = tillfac * pCL->fNHumusSlow;
			pCL->fCHumusSlow -= TeilC;
		    pCL->fCHumusFast += TeilC;
			pCL->fNHumusSlow -= TeilN;
			pCL->fNHumusFast += TeilN;
		   }

		  else
		   {
		    TeilC = tillfac * anteil * pCL->fCHumusSlow;
			TeilN = tillfac * anteil * pCL->fNHumusSlow;
		    pCL->fCHumusSlow -= TeilC;
			pCL->fCHumusFast += TeilC;
		    pCL->fNHumusSlow -= TeilN;
			pCL->fNHumusFast += TeilN;
		   }
		}
	 FunktionDurchlaufen++;
	 if(tillfac == (float)0.04)
	  {
	   FunktionDurchlaufen = 0;
	   tillfac = (float)0.16;
	  }
  }
 }
return 0;
}/* Ende Berechnung Tillage bei DNDC */


/*************************************************************************************/
/* Procedur    :   NCSOIL-Tillage                                                      */
/* Beschreibung:   Bodenbearbeitung nach dem Modell NCSOIL                            */
/*                 Grundlage Molina et al. 1983                                          */
/*                                                                                   */
/*				   GSF/ab  Axel Berkenkamp         09.11.01                             */
/*                                                                                   */
/*************************************************************************************/
/*	veränd. Var.    pCL->fCHumusSlow  pCL->fNHumusSlow								 */
/*					pCL->fCHumusFast  pCL->fNHumusFast								 */
/*                                                                                   */
/*************************************************************************************/
signed short int NCSOILTillage(EXP_POINTER)
{
 PSLAYER pSL;
 PCLAYER pCL;

 int	i,anzahl;
 float tillfac = (float)0.16;
 float  bearbTiefe,aktTiefe,obereTiefe,TeilC,TeilN,anteil;

 if(NewDay(pTi))
  {

  if ((float)pTi->pSimTime->fTimeAct >= (float)pMa->pTillage->iDay)
   {
    bearbTiefe = pMa->pTillage->fDepth;
    aktTiefe   = (float)0.0;

    /* Berechnung Anzahl an betroffenden Simulationsschichten und
      des Anteils einer eventuell nur teilweise betroffenen Schicht*/
   
    for (pSL = pSo->pSLayer->pNext,anzahl=0;
        (aktTiefe < bearbTiefe);
        pSL = pSL->pNext,anzahl++)
		{
	        obereTiefe = aktTiefe;
			aktTiefe += pSL->fThickness;
		}
       anteil = (bearbTiefe - obereTiefe) / pSL->fThickness;

    	
    for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext,i=1;
                (i <= anzahl);
	         pSL = pSL->pNext,
	         pCL = pCL->pNext,i++)
		{
		  if (i != anzahl)
		   {
			TeilC = tillfac * pCL->fCHumusSlow;
		    TeilN = tillfac * pCL->fNHumusSlow;
			pCL->fCHumusSlow -= TeilC;
		    pCL->fCHumusFast += TeilC;
			pCL->fNHumusSlow -= TeilN;
			pCL->fNHumusFast += TeilN;
		   }

		  else
		   {
		    TeilC = tillfac * anteil * pCL->fCHumusSlow;
			TeilN = tillfac * anteil * pCL->fNHumusSlow;
		    pCL->fCHumusSlow -= TeilC;
			pCL->fCHumusFast += TeilC;
		    pCL->fNHumusSlow -= TeilN;
			pCL->fNHumusFast += TeilN;
		   }
		}
   }
  }
return 0;
}/* Ende Berechnung Tillage bei NCSOIL */


/*********************************************************************************/
/*  Name     : TSSowing                                                          */
/*                                                                               */
/*  Funktion : Setzen notwendiger Variablen bei Erreichen eines Pflanztermins    */
/*  Autor    : ch                                                                */
/*  Datum	 : 17.1.97                                                           */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSSowing(EXP_POINTER)
{

  if (pPl != NULL)
  {
	 if ((pMa->pSowInfo != NULL)&&
	     (pTi->pSimTime->fTimeAct == (float)pMa->pSowInfo->iDay)) 
		{
			if (pMa->pSowInfo->fPlantDens <= (float)0)(Message(1,"error: zero seed density"));	
			if (pMa->pSowInfo->fRowWidth  <= (float)0)(Message(1,"error: zero row width"));	
			if (pMa->pSowInfo->fSowDepth  <= (float)0)(Message(1,"error: zero sow depth"));	

	        SPASS_Genotype_Read(hModuleInstance);
			PlantVariableInitiation(exp_p);
				
			/* Schalter fuer Wachstumsmodell initialisieren  */
			//bMaturity	 		= FALSE;
			//bPlantGrowth 		= TRUE;
            pPl->pDevelop->bMaturity = FALSE;
            pPl->pDevelop->bPlantGrowth = TRUE;

		}
  } 
  return 1;
}      /*===== Ende der Berechnung der Pflanzung =============*/


/*********************************************************************************/
/*  Name     : TSComparePreCropDate                                              */
/*                                                                               */
/*  Funktion : Vergleicht das Stardatum der Simulation mit dem Termin fuer       */
/*             Ernte der Vorfrucht. Liegt dieser Termine nach dem Beginn der     */
/*             Simulation werden die entsprechenden Pools aktualisiert und die   */
/*             Bilanzgroessen auf null gesetzt.                                  */
/*  Autor    :                                                                   */
/*  Datum	 :                                                                   */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSComparePreCropDate(EXP_POINTER)
{
    float          actDepth,RootProp,amount,factor,RootSum;
    
	PSLAYER		  pSL;
	PCLAYER		  pCL;

    RootProp = (float)0.0;
    RootSum  = (float)0.0;
    actDepth = (float)0.0;

	if (pTi->pSimTime->fTimeAct == (float)pMa->pLitter->iDay) 
	{
 	/* Wenn Angaben zu den Ernterueckstaenden in der Datenbank fehlen kommt es
	   in dieser Funktion zum Programmabsturz aufgrund Division by zero. Daher
	   findet eine letzte Ueberpruefung statt und es werden Werte ungleich Null
	   initialisiert. */     

		if (pMa->pLitter->fTopCN <= (float)0.0)    
		   pMa->pLitter->fTopCN = (float)0.1;    
		if (pMa->pLitter->fRootCN <= (float)0.0)    
		   pMa->pLitter->fRootCN = (float)0.1;    



	/* Gesamtmenge der ober- und unterirdischen Ernterueckstaende. */ 
		if (pMa->pLitter->fTotalAmount == (float)-99)
			pMa->pLitter->fTotalAmount = pMa->pLitter->fRootAmount;
		else
			pMa->pLitter->fTotalAmount += pMa->pLitter->fRootAmount;

		if(pMa->pLitter->iCarryOff==0)
			pMa->pLitter->fTotalAmount += pMa->pLitter->fTopAmount;


	/* Zu Beginn der Simulation werden die oberirdischen Ernterueckstaende auf 
	   eine Oberflaechenschicht initialisiert. Dabei bestehen 40% der Rueck-
	   staende aus Kohlenstoff und werden daher auf die Pools geschrieben */ 
	
	if(pMa->pLitter->iCarryOff==0)
     pMa->pLitter->fTopC = pMa->pLitter->fTopAmount  * (float)0.4;
		
	else
     pMa->pLitter->fTopC = (float)0.0;

	SET_IF_99(pCh->pCProfile->fCLitterSurf,pMa->pLitter->fTopC);
	SET_IF_99(pCh->pCProfile->fNLitterSurf,pMa->pLitter->fTopC / pMa->pLitter->fTopCN);

	SET_IF_99(pCh->pCProfile->fCManureSurf,(float)0);
	SET_IF_99(pCh->pCProfile->fNManureSurf,(float)0);
		
	SET_IF_99(pCh->pCProfile->fCHumusSurf,(float)0);
	SET_IF_99(pCh->pCProfile->fNHumusSurf,(float)0);

	/* Die Wurzelmasse, wird auf die Bodenschichten verteilt. Zuerst wird die Gesamtsumme ermittelt,
	   der Schichtanteil berechnet und dann die schichtmaessige Verteilung vorgenommen.
	   Zur Vereinfachung Variable pCL->fCFreeFOM benutzt um Anteile zu uebergeben */
	
	pMa->pLitter->fRootC = pMa->pLitter->fRootAmount * (float)0.40;

	for (pSL = pSo->pSLayer->pNext,
         pCL = pCh->pCLayer->pNext;
                      ((pSL->pNext != NULL)&&
                       (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext)
		{
		  actDepth = actDepth + (pSL->fThickness / 10);
		  RootProp = (float)exp((float)-3.0 * actDepth / (pSo->fDepth / (float)10.0));
		  RootSum += RootProp;
		  pCL->fCFreeFOM = RootProp;
		}

    for (pCL = pCh->pCLayer->pNext;
	     pCL->pNext != NULL;
		 pCL = pCL->pNext)
		{
		  factor = pCL->fCFreeFOM / RootSum;
          amount = pMa->pLitter->fRootC * factor;

          if(amount>(float)0)
		  {  
		   if (pCL->fCLitter == (float)-99) pCL->fCLitter = amount;
		   if (pCL->fNLitter == (float)-99) pCL->fNLitter = amount/pMa->pLitter->fRootCN;
		  }

		  else
		  {
		   if (pCL->fCLitter == (float)-99) pCL->fCLitter = (float) 0.0;
		   if (pCL->fNLitter == (float)-99) pCL->fNLitter = (float) 0.1;
		  }

          if(pCL->fNLitter>EPSILON)  
		   pCL->fLitterCN = pCL->fCLitter / pCL->fNLitter;          
	      
		  else
           pCL->fLitterCN =(float)0.1;
		}
	}
  
  return 1;
}     /*===== Ende der Ueberpreufung der Startwerte =============*/

/*********************************************************************************/
/*  Name     : TSCompareStartValueDate                                           */
/*                                                                               */
/*  Funktion : Vergleicht das Startdatum der Simulation mit dem Termin fuer      */
/*             die Startwerte. Liegen diese Termine nach dem Beginn der          */
/*             Simulation werden die entsprechenden Pools aktualisiert und die   */
/*             Bilanzgroessen auf null gesetzt.                                  */
/*  Autor    :                                                                   */
/*  Datum	 :                                                                   */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSCompareStartValueDate(EXP_POINTER)
{

PSTARTVALUE  pMS = &pMa->Startvalue;
int          i1, maxi1;
float        f1;

i1 = (int)0;
f1 = (float)0.0;
maxi1 = MAX_DEPTH_PER_MEASURE;
maxi1 -= (int)1;
 
if (pTi->pSimTime->fTimeAct == (float)pMS->iDay) 
{

	/* Berechnung der Nitrat- und Ammoniummenge, die auf jede 
	   Simulationsschicht verteilt wird. Dies muss zu Beginn einer 
	   jeden Materialschicht durchgefuehrt werden. */			

    #if 0

    float  part1,part2,rest,mengeNO3,mengeNH4,restNO3,restNH4;

    PSLAYER     pSL;
    PCLAYER     pCL;
    PCPROFILE   pCP = pCh->pCProfile;
    /*
    PATCH CH.BAUER:
    ---------------

    Der folgende Code verursacht Speicherueberschreiber wenn
    DateToDays korrigiert wird und die korrekte Anzahl von
    Tagen zurueckgibt (1 Tag fuer den Zeitraum vom 1.Jan bis 1.Jan).
    i1 wird groesser als MAX_DEPTH_PER_MEASURE. Allem Anschein nach
    koennen die verketteten Listen pCL und PSL mehr Elemente haben
    als MAX_DEPTH_PER_MEASURE. Ich habe den Code komplett auskommentiert.
    Der Code wird sowiese NIE durchlaufen, wenn DateToDays einen Tag zuwenig
    zurueckgibt.
    */

		while (i1 <= maxi1)
		{
         f1 += pMS->afNO3N[i1] + pMS->afNH4N[i1];
			i1++;
		}

		i1 = (int)0;

		pCh->pCBalance->fNBalCorrect = pCP->fN2OEmisCum + pCP->dNOEmisCum + pCP->dN2EmisCum + pCP->fNH3VolatCum  //= fNUpperOutputCum
								      + pCP->fCumActNUpt + pCP->fNImmobCum - pCP->fNMinerCum  //= fNMiddleOutputCum
									  + pCP->fNTotalLeachCum;
	
		pCh->pCBalance->fNO3NProfile   = (float)0.0;
		pCh->pCBalance->fNH4NProfile   = (float)0.0;
		pCh->pCBalance->fNProfileStart = (float)0.0;

    for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext)
	{
	/* Berechnung der Menge, die auf jede Simulationsschicht
	   verteilt wird. Dies muss zu Beginn einer jeden Materialschicht
	   durchgefuehrt werden. */			
	
		part1 = pMS->afThickness[i1] / pSL->fThickness; 
		part2 = part1 + (float)0.5;
		
		rest = part1 - part2;
		restNO3 = pMS->afNO3N[i1] * rest;
		restNH4 = pMS->afNH4N[i1] * rest;

		mengeNO3 = pMS->afNO3N[i1] / part2;
		mengeNH4 = pMS->afNH4N[i1] / part2;

	/* Menge pro aequidistante Schicht ohne Beruecksichtigung eines
	   Restes aus der vorhergehenden Materialschicht */			
	/* i2 beschreibt die Anzahl der Simulationsschichten, die jeder 
	   Materialschicht in der Datenbank zugeordnet wurde. Nach dem 
	   Beschreiben einer Simulationsschicht wird dieser Zaehler verrringert. */
		while (part2 >= (int)1)  
		{
			pCL->fNO3N = mengeNO3;                      
			pCL->fNH4N = mengeNH4;                      

			pCh->pCBalance->fNO3NProfile += pCL->fNO3N;
			pCh->pCBalance->fNH4NProfile += pCL->fNH4N;
			pCh->pCBalance->fNProfileStart += pCL->fNO3N + pCL->fNH4N;
		
			
			if ((pCL->pNext != NULL)&&(pSL->pNext != NULL))
			{	pCL = pCL->pNext;
				pSL = pSL->pNext;
			}
			part2--;
		}
		pCL = pCL->pBack;
		pSL = pSL->pBack;
	
		i1++;
		pMS->afNO3N[i1] += restNO3;
		pMS->afNH4N[i1] += restNH4;

	}
                  
       if ((f1 < pCh->pCBalance->fNProfileStart - 1)||(f1 > pCh->pCBalance->fNProfileStart + 1))
       {
    	Message(1,START_BALANCE_ERROR_TXT); 
    	return -1;
       }
	/* Nachdem die Mengen der Variablen auf die Simulationsschichten verteilt worden sind
	   muessen sowohl der erste als auch die letzte virtuelle Simulationsschicht beschrieben
	   werden. Dies geschieht durch kopieren der zweiten bzw. vorletzten Simulationsschicht.*/

			/* Die letzte virtuelle Simulationsschicht wird mit den Werten
			   der vorletzten Simulationsschicht beschrieben */

				pCL->fNO3N = pCL->pBack->fNO3N;
				pCL->fNH4N = pCL->pBack->fNH4N;

			/* Die erste virtuelle Simulationsschicht wird mit den Werten
			   der zweiten Simulationsschicht beschrieben */

				pCh->pCLayer->fNO3N = pCh->pCLayer->pNext->fNO3N;
				pCh->pCLayer->fNH4N = pCh->pCLayer->pNext->fNH4N;

    #endif
}
	return 1;
}      /*===== Ende der Ueberpreufung der Starwerte =============*/


/*********************************************************************************/
/*  Name     : InitOrgDuenger                                                    */
/*                                                                               */
/*  Funktion : Fuer die organischen Duengemittel werden Parameter benoetigt      */
/*             um deren Infiltration in den Boden zu berechnen. Diese werden     */
/*             hier auf die entsprechenden Variablen geschrieben.                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*             pFE->afInfParam[0]                                                */
/*             pFE->afInfParam[1]                                                */
/*             pFE->afInfParam[2]                                                */
/*                                                                               */
/*********************************************************************************/
signed short int InitOrgDuenger(EXP_POINTER)
{

 PNFERTILIZER	pFE = pMa->pNFertilizer;

//pFE->acCode korrekt voruebergehend acName
	if (!lstrcmp(pFE->acCode,"RE005\0"))        
	{ //Rindergülle 5% TS
		pFE->afInfParam[0] = (float)0.23;
		pFE->afInfParam[1] = (float)-0.25;
		pFE->afInfParam[2] = (float)0.50;
	}
	if (!lstrcmp(pFE->acCode,"RE006\0"))        	
	{ //Rindergülle 10% TS
		pFE->afInfParam[0] = (float)0.43;
		pFE->afInfParam[1] = (float)0.46;
		pFE->afInfParam[2] = (float)0.185;
	}
	if (!lstrcmp(pFE->acCode,"RE007\0"))        	
	{ //Rindergülle 15% TS
		pFE->afInfParam[0] = (float)0.53;
		pFE->afInfParam[1] = (float)0.56;
		pFE->afInfParam[2] = (float)0.185;
	}
	if (!lstrcmp(pFE->acCode,"RE008\0"))        	
	{ //Scheineguelle 3.5% TS
		pFE->afInfParam[0] = (float)0.33;
		pFE->afInfParam[1] = (float)-0.15;
		pFE->afInfParam[2] = (float)0.38;
	}
	if (!lstrcmp(pFE->acCode,"RE009\0"))        	
	{ //Scheineguelle 7% TS
		pFE->afInfParam[0] = (float)0.43;
		pFE->afInfParam[1] = (float)0.22;
		pFE->afInfParam[2] = (float)0.135;
	}
	if (!lstrcmp(pFE->acCode,"RE010\0"))        	
	{ //Scheineguelle 10.5% TS
		pFE->afInfParam[0] = (float)0.50;
		pFE->afInfParam[1] = (float)0.35;
		pFE->afInfParam[2] = (float)0.155;
	}
	if (!lstrcmp(pFE->acCode,"RE011\0"))        	
	{ //Scheineguelle 14% TS
		pFE->afInfParam[0] = (float)0.6;
		pFE->afInfParam[1] = (float)0.55;
		pFE->afInfParam[2] = (float)0.185;
	}
	if (!lstrcmp(pFE->acCode,"RE012\0"))        	
	{ //Gefluegelguelle 7% TS
		pFE->afInfParam[0] = (float)0.15;
		pFE->afInfParam[1] = (float)-0.15;
		pFE->afInfParam[2] = (float)0.38;
	}
	if (!lstrcmp(pFE->acCode,"RE013\0"))        	
	{ //Gefluegelguelle 14% TS
		pFE->afInfParam[0] = (float)0.35;
		pFE->afInfParam[1] = (float)0.35;
		pFE->afInfParam[2] = (float)0.125;
	}
	if (!lstrcmp(pFE->acCode,"RE014\0"))        	
	{ //Gefluegelguelle 21% TS
		pFE->afInfParam[0] = (float)0.55;
		pFE->afInfParam[1] = (float)0.55;
		pFE->afInfParam[2] = (float)0.185;
	}
	if (!lstrcmp(pFE->acCode,"RE015\0"))        	
	{ //Rindermist
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
	if (!lstrcmp(pFE->acCode,"RE016\0"))        	
	{ //Schweinemist
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
	if (!lstrcmp(pFE->acCode,"RE017\0"))        	
	{ //Haehnchenmist
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
	if (!lstrcmp(pFE->acCode,"RE018\0"))        	
	{ //Putenmist
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
	if (!lstrcmp(pFE->acCode,"RE019\0"))        	
	{ //Pferdemist
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
	if (!lstrcmp(pFE->acCode,"RE020\0"))        	
	{ //Schafmst
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
	if (!lstrcmp(pFE->acCode,"RE021\0"))        	
	{ //Huehnerfrischkot
		pFE->afInfParam[0] = (float)0.1;
		pFE->afInfParam[1] = (float)-0.1;
		pFE->afInfParam[2] = (float)0.3;
	}
	if (!lstrcmp(pFE->acCode,"RE022\0"))        	
	{ //Huehnertrockenkot
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
	if (!lstrcmp(pFE->acCode,"RE023\0"))        	
	{ //Rinderjauche
		pFE->afInfParam[0] = (float)0.85;
		pFE->afInfParam[1] = (float)0.65;
		pFE->afInfParam[2] = (float)0.10;
	}
	if (!lstrcmp(pFE->acCode,"RE024\0"))        	
	{ //Schweinjauche
		pFE->afInfParam[0] = (float)0.23;
		pFE->afInfParam[1] = (float)-0.25;
		pFE->afInfParam[2] = (float)0.50;
	}
	if (!lstrcmp(pFE->acCode,"RE025\0"))        	
	{ // Klaerschlamm 5% TS
		pFE->afInfParam[0] = (float)0.15;
		pFE->afInfParam[1] = (float)-0.15;
		pFE->afInfParam[2] = (float)0.38;
	}
	if (!lstrcmp(pFE->acCode,"RE026\0"))        	
	{ // Klaerschlamm 35% TS
		pFE->afInfParam[0] = (float)0.55;
		pFE->afInfParam[1] = (float)0.65;
		pFE->afInfParam[2] = (float)0.175;
	}
	if (!lstrcmp(pFE->acCode,"RE027\0"))        	
	{ // Gruengutkompost
		pFE->afInfParam[0] = (float)0.0;
		pFE->afInfParam[1] = (float)0.0;
		pFE->afInfParam[2] = (float)0.0;
	}
  /* Wenn fuer das gewaehlte Geraet keine Zuordnung stattgefunden 
   hat wird es hier mit Standarwerten belegt. */
	if ((pFE->afInfParam[0] == (float)0.0) &&
	   (pFE->afInfParam[1]  == (float)0.0) &&
	   (pFE->afInfParam[2]  == (float)0.0))
	   {
		pFE->afInfParam[0] = (float)0.1;
		pFE->afInfParam[1] = (float)0.1;
		pFE->afInfParam[2] = (float)0.1;
	   }
		
  return 1;
}      /*===== Ende der Initialisierung der Bodenbearbeitungsgeraete =============*/
/*********************************************************************************/
/*  Name     : InfiltrationOrgDuenger                                                  */
/*                                                                               */
/*  Funktion : Am Ausbringungstag versickern die org. Duenger im Boden. Diese    */
/*             Infiltration in den Boden wird hier berechnet.                    */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 02.02.98                                                   */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int InfiltrationOrgDuenger(EXP_POINTER)
{

PNFERTILIZER	pFE = pMa->pNFertilizer;

float	infNH4,inforgN,inforgC,infFeuchte;
double  feuchte;

infNH4 = (float)0.0;
inforgN = (float)0.0;
inforgC = (float)0.0;
infFeuchte = (float)0.0;

/* Feuchtebedingungen fuer die Infiltration.*/	
infFeuchte = pWa->pWLayer->pNext->fContAct / pSo->pSWater->pNext->fContFK;

/* Infiltration findet nut statt, wenn der Boden nicht gefroren ist.*/	
if (  pHe->pHLayer->pNext->fSoilTemp >(float)0.0)
{
	/* Eine Infiltration wird fuer die Fraktionen Ammonium, 
	   org. N und org. Substanz berechnet. */	

	if (!lstrcmp(pMa->pNFertilizer->acCode,"RE\0"))  //organische Duengung
	{
		infNH4 = pFE->afInfParam[0] * pCh->pCProfile->fNH4ManureSurf * (float)
			 (sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens)) *
			 (pFE->afInfParam[1] + (pFE->afInfParam[2] * (float)
			 (log((double)pCh->pCProfile->fNH4ManureSurf))));
	}

	if ((!lstrcmp(pFE->acCode,"RE005\0"))||
		(!lstrcmp(pFE->acCode,"RE006\0"))||
		(!lstrcmp(pFE->acCode,"RE007\0")))
	{
		inforgN = (float)0.06 * pCh->pCProfile->fNManureSurf * (float)
				(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
		inforgC = (float)0.06 * pCh->pCProfile->fCManureSurf *	(float)
				(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
	}

	if ((!lstrcmp(pFE->acCode,"RE008\0"))||
		(!lstrcmp(pFE->acCode,"RE009\0"))||
		(!lstrcmp(pFE->acCode,"RE010\0"))||
		(!lstrcmp(pFE->acCode,"RE011\0")))
	{
		inforgN = (float)0.15 * pCh->pCProfile->fNManureSurf * (float)
				(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
		inforgC = (float)0.15 * pCh->pCProfile->fCManureSurf * (float)
				(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));

	}

	if ((!lstrcmp(pFE->acCode,"RE012\0"))||
		(!lstrcmp(pFE->acCode,"RE013\0"))||
		(!lstrcmp(pFE->acCode,"RE014\0"))||
		(!lstrcmp(pFE->acCode,"RE025\0"))||
		(!lstrcmp(pFE->acCode,"RE026\0")))
	{
		inforgN = (float)0.2 * pCh->pCProfile->fNManureSurf * (float)
				(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
		inforgC = (float)0.2 * pCh->pCProfile->fCManureSurf * (float)
				(sqrt((double)1.45 / (double)pSo->pSLayer->pNext->fBulkDens));
	
	}

	if ((!lstrcmp(pFE->acCode,"RE015\0"))||
		(!lstrcmp(pFE->acCode,"RE016\0"))||
		(!lstrcmp(pFE->acCode,"RE017\0"))||
		(!lstrcmp(pFE->acCode,"RE018\0"))||
		(!lstrcmp(pFE->acCode,"RE019\0"))||
		(!lstrcmp(pFE->acCode,"RE020\0"))||
		(!lstrcmp(pFE->acCode,"RE021\0"))||
		(!lstrcmp(pFE->acCode,"RE022\0"))||
		(!lstrcmp(pFE->acCode,"RE027\0")))
	{
		inforgN = (float)0.0;
		inforgC = (float)0.0;
	}

	/* Beruecksichtigung der Feuchtebedingungen */	
	
	if ((infFeuchte >(float)1.2)&&
		(infFeuchte <(float)0.4))
	{
		infNH4 = infNH4 * (float)0.9;	
	}
	else
	{	
		feuchte = infFeuchte - (float)0.8;
		infNH4 = infNH4 * ((float)1.1 - ((float)1.25 
					* (float)pow((double)feuchte,(double)2)));
	}


	/* Bilanzierung */	

	pCh->pCProfile->fNH4ManureSurf      -= infNH4;
	pCh->pCLayer->pNext->fNH4Manure     += infNH4;

	pCh->pCProfile->fNManureSurf        -= inforgN;
	pCh->pCLayer->pNext->fNManure		+= inforgN;

	pCh->pCProfile->fCManureSurf        -= inforgC;
	pCh->pCLayer->pNext->fCManure	    += inforgC;

	
	if (pCh->pCLayer->pNext->fNManure > (float)0.0)
	{
		pCh->pCLayer->pNext->fManureCN = pCh->pCLayer->pNext->fCManure /
								pCh->pCLayer->pNext->fNManure;
	
		pCh->pCProfile->fCNManureSurf = pCh->pCProfile->fCManureSurf /
										pCh->pCProfile->fNManureSurf;
	}
  }

  return 1;
}      /*===== Ende der Berechnung der Infiltration =============*/

/*********************************************************************************/
/*  Name     : InfiltrationOrgDuengerRegen                                       */
/*                                                                               */
/*  Funktion : Durch den Niederschlag wird der an der Oberflaeche aufliegnde     */
/*             organische Duenger in den Boden eingewaschen. Dies wird hier      */
/*             berechnet.                                                        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 02.02.98                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int InfiltrationOrgDuengerRegen(EXP_POINTER)
{

PNFERTILIZER	pFE = pMa->pNFertilizer;

float	sicknh4,sickorgN,sickorgC,wirkNied;


//if (pCl->pWeather->fRainAmount > (float)5.0)
//if (pWa->pWBalance->fReservoir > (float)5.0)
//if ((pCl->pWeather->fRainAmount > (float)5.0)||(pWa->pWBalance->fReservoir > (float)5.0))
if ((NewDay(pTi))&&((pCl->pWeather->fRainAmount > (float)5.0)||(pWa->pWBalance->fReservoir > (float)5.0)))//ep 090806
{
	wirkNied = ((float)1.0/(float)(sqrt((double)pCl->pWeather->fRainAmount + (double)0.75)))
						- ((float)0.02 * (pCl->pWeather->fRainAmount - (float)0.25));
	//wirkNied = ((float)1.0/(float)(sqrt((double)pWa->pWBalance->fReservoir + (double)0.75)))
	//					- ((float)0.02 * (pWa->pWBalance->fReservoir - (float)0.25));
	if (wirkNied > (float)0.0)
		wirkNied = (float)0.0;

  if (pFE != NULL)
  {
	if ((!lstrcmp(pFE->acCode,"RE015\0"))||
		(!lstrcmp(pFE->acCode,"RE016\0"))||
		(!lstrcmp(pFE->acCode,"RE017\0"))||
		(!lstrcmp(pFE->acCode,"RE018\0"))||
		(!lstrcmp(pFE->acCode,"RE019\0"))||
		(!lstrcmp(pFE->acCode,"RE020\0"))||
		(!lstrcmp(pFE->acCode,"RE021\0"))||
		(!lstrcmp(pFE->acCode,"RE022\0"))||
		(!lstrcmp(pFE->acCode,"RE027\0")))
	{
		if (pCh->pCProfile->fNH4ManureSurf > (float)0.0)
		{
			sicknh4 = pCh->pCProfile->fNH4ManureSurf * 
							((float)1.0 - wirkNied);
			pCh->pCProfile->fNH4ManureSurf		-= sicknh4;
			pCh->pCLayer->pNext->fNH4Manure     += sicknh4;
		}
			
		if (pCh->pCProfile->fNManureSurf > (float)0.0)
		{
			sickorgN = pCh->pCProfile->fNManureSurf * (float)0.004 *
							((float)1.0 - wirkNied);
			pCh->pCProfile->fNManureSurf		-= sickorgN;
			pCh->pCLayer->pNext->fNManure		+= sickorgN;
		}
		
		if (pCh->pCProfile->fCManureSurf > (float)0.0)
		{
			sickorgC = pCh->pCProfile->fCManureSurf * (float)0.004 *
							((float)1.0 - wirkNied);
			pCh->pCProfile->fCManureSurf		-= sickorgC;
			pCh->pCLayer->pNext->fCManure	    += sickorgC;
		}
	}
	else
	{
		if (pCh->pCProfile->fNH4ManureSurf > (float)0.0)
		{
			sicknh4 = pCh->pCProfile->fNH4ManureSurf * 
							((float)1.0 - wirkNied);
			pCh->pCProfile->fNH4ManureSurf		-= sicknh4;
		//	pCh->pCLayer->pNext->fNH4Manure     += sicknh4;
		}
			
		if (pCh->pCProfile->fNManureSurf > (float)0.0)
		{
			sickorgN = pCh->pCProfile->fNManureSurf * (float)0.02 *
							((float)1.0 - wirkNied);
			pCh->pCProfile->fNManureSurf		-= sickorgN;
			pCh->pCLayer->pNext->fNManure		+= sickorgN;
		}
		
		if (pCh->pCProfile->fCManureSurf > (float)0.0)
		{
			sickorgC = pCh->pCProfile->fCManureSurf * (float)0.02 *
							((float)1.0 - wirkNied);
			pCh->pCProfile->fCManureSurf		-= sickorgC;
			pCh->pCLayer->pNext->fCManure	    += sickorgC;
		}
	}
  }
 }
  return 1;
}      /*===== Ende der Berechnung der Infiltration durch Regen =============*/

/*********************************************************************************/
/*  Name     : InfiltrationOrgDuengerRegen                                       */
/*                                                                               */
/*  Funktion : Durch den Niederschlag wird der an der Oberflaeche aufliegnde     */
/*             organische Duenger in den Boden eingewaschen. Dies wird hier      */
/*             berechnet.                                                        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 02.02.98                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int InfiltrationOrgDuengerBeregnung(EXP_POINTER)
{

PMIRRIGATION	pIR = pMa->pIrrigation;

float	sicknh4,sickorgN,sickorgC,wirkNied;;


if ((pWa->pWBalance->fReservoir > (float)5.0)&&(pIR != NULL))
{
	wirkNied = ((float)1.0/(float)(sqrt((double)pIR->fAmount + (double)0.75)))
						- ((float)0.02 * (pIR->fAmount - (float)0.25));
	if (wirkNied > (float)0.0)
		wirkNied = (float)0.0;

	{
		if (pCh->pCProfile->fNH4ManureSurf > (float)0.0)
		{
			sicknh4 = pCh->pCProfile->fNH4ManureSurf * 
							((float)1.0 - wirkNied);
			pCh->pCProfile->fNH4ManureSurf		-= sicknh4;
			pCh->pCLayer->pNext->fNH4Manure     += sicknh4;
		}
			
		if (pCh->pCProfile->fNManureSurf > (float)0.0)
		{
			sickorgN = pCh->pCProfile->fNManureSurf * (float)0.02 *
							((float)1.0 - wirkNied);
			pCh->pCProfile->fNManureSurf		-= sickorgN;
			pCh->pCLayer->pNext->fNManure		+= sickorgN;
		}
		
		if (pCh->pCProfile->fCManureSurf > (float)0.0)
		{
			sickorgC = pCh->pCProfile->fCManureSurf * (float)0.02 *
							((float)1.0 - wirkNied);
			pCh->pCProfile->fCManureSurf		-= sickorgC;
			pCh->pCLayer->pNext->fCManure	    += sickorgC;
		}
	}
  }
 
  return 1;
}      /*===== Ende der Berechnung der Infiltration durch Beregnung =============*/


/*********************************************************************************/
/*  Name     : TSCorrectMeasureValue                                             */
/*                                                                               */
/*  Funktion : Vergleicht den ersten Messwert mit den Ergebnissen der Simulation.*/
/*             Der simulationswert wird auf den Messwert korregiert.             */
/*                                                                               */
/*  Autor    : Thomas Schaaf                                                     */
/*  Datum	 : 26.02.98                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI TSCorrectMeasureValue(EXP_POINTER)
{

int		i1,part2,maxi1;
float	part1,f1,rest,mengeNO3,mengeNH4,restNO3,restNH4,dicke;

PNMEASURE		pNM= pCh->pNMeasure;
PSLAYER			pSL;
PCLAYER			pCL;
PCPROFILE		pCP = pCh->pCProfile;

dicke = (float)0;
i1 = (int)0;
f1 = (float)0;
maxi1 = MAX_DEPTH_PER_MEASURE; //maximale Messwertschichtzahl ist 5;
maxi1 -= (int)1;

if (bCorrectMeasure == FALSE) //ep: if (bCorrectMeasure == TRUE) (Version vom 30.10.2012)
{
 if (pNM != NULL)
 {
  if (pTi->pSimTime->fTimeAct == (float)pNM->iDay) 
  {

	/* Berechnung der Nitrat- und Ammoniummenge, die auf jede 
	   Simulationsschicht verteilt wird. Dies muss zu Beginn einer 
	   jeden Materialschicht durchgefuehrt werden. */			

		while (i1 <= maxi1)
		{   f1 += pNM->afNO3N[i1] + pNM->afNH4N[i1];
			i1++;
		}

		pCh->pCBalance->dNBalCorrect = pCP->dN2OEmisCum + pCP->dNOEmisCum + pCP->dN2EmisCum + pCP->dNH3VolatCum  
								+ pCP->dNUptakeCum + pCP->dNImmobCum - pCP->dNMinerCum
								+ pCP->dNTotalLeachCum;
		
		pCh->pCBalance->fNO3NProfile   = (float)0.0;
		pCh->pCBalance->fNH4NProfile   = (float)0.0;
		pCh->pCBalance->fNProfileStart = (float)0.0;

		i1 = (int)0;
		dicke = pNM->afDepth[i1];


    for (pSL = pSo->pSLayer->pNext,
    	 pCL = pCh->pCLayer->pNext;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL)&& i1<=maxi1);
         pSL = pSL->pNext,
         pCL = pCL->pNext)
	{
	/* Berechnung der Menge, die auf jede Simulationsschicht
	   verteilt wird. Dies muss zu Beginn einer jeden Materialschicht
	   durchgefuehrt werden. */			
	
		part1 = dicke / pSL->fThickness; 
		part2 = (int)(part1 + (float)0.5);
		
		rest = part1 - part2;
		restNO3 = pNM->afNO3N[i1] * rest;
		restNH4 = pNM->afNH4N[i1] * rest;

		mengeNO3 = pNM->afNO3N[i1] / part2;
		mengeNH4 = pNM->afNH4N[i1] / part2;

	/* Menge pro aequidistante Schicht ohne Beruecksichtigung eines
	   Restes aus der vorhergehenden Materialschicht */			
	/* i2 beschreibt die Anzahl der Simulationsschichten, die jeder 
	   Materialschicht in der Datenbank zugeordnet wurde. Nach dem 
	   Beschreiben einer Simulationsschicht wird dieser Zaehler verrringert. */
		while (part2 >= (int)1)  
		{
			pCL->fNO3N = mengeNO3;                      
			pCL->fNH4N = mengeNH4;                      

			pCh->pCBalance->fNO3NProfile += pCL->fNO3N;
			pCh->pCBalance->fNH4NProfile += pCL->fNH4N;
			pCh->pCBalance->fNProfileStart += pCL->fNO3N + pCL->fNH4N;
		
			
			if ((pCL->pNext != NULL)&&(pSL->pNext != NULL))
			{	pCL = pCL->pNext;
				pSL = pSL->pNext;
			}
			part2--;
		}
		pCL = pCL->pBack;
		pSL = pSL->pBack;
	
		i1++;
		dicke = pNM->afDepth[i1] - pNM->afDepth[i1-1];;
		pNM->afNO3N[i1] += restNO3;
		pNM->afNH4N[i1] += restNH4;

	}
                  
   if ((f1 < pCh->pCBalance->fNProfileStart - 1)||(f1 > pCh->pCBalance->fNProfileStart + 1))
   {
    Message(1,CORRECT_MEASURE_VALUE_TXT); 
    return -1;
   }
	/* Nachdem die Mengen der Variablen auf die Simulationsschichten verteilt worden sind
	   muessen sowohl der erste als auch die letzte virtuelle Simulationsschicht beschrieben
	   werden. Dies geschieht durch kopieren der zweiten bzw. vorletzten Simulationsschicht.*/

	/* Die letzte virtuelle Simulationsschicht wird mit den Werten
	   der vorletzten Simulationsschicht beschrieben */

		pCL->fNO3N = pCL->pBack->fNO3N;
		pCL->fNH4N = pCL->pBack->fNH4N;

	/* Die erste virtuelle Simulationsschicht wird mit den Werten
	   der zweiten Simulationsschicht beschrieben */

		pCh->pCLayer->fNO3N = pCh->pCLayer->pNext->fNO3N;
		pCh->pCLayer->fNH4N = pCh->pCLayer->pNext->fNH4N;


	bCorrectMeasure = TRUE;
  }
 }
}
	return 1;
}      /*===== Ende der Ueberpreufung der Starwerte =============*/


/*******************************************************************************
** EOF */