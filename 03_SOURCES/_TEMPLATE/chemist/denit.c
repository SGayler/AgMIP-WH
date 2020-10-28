/*******************************************************************************
 *
 * Copyright (c) by 
 *
 *------------------------------------------------------------------------------
 *
 * Contents: Sammlung der Modellansaetze zur Berechnung der Denitrifizierung
 *           von Stickstoff im Boden.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 2 $
 *
 * $History: denit.c $
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/chemist
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 *   Zusammenfuehren der Funktionen erfolgte am 12.03.97
 *
 ******************************************************************************/

/* Sammlung aller Header-Dateien */
#include "xinclexp.h"     
#include "xn_def.h"     
#include "xlanguag.h"     
#include "century.h"


// #define RAINY_DAY    (pCl->pWeather->fRainAmount > (float)5)
#define RAINY_DAY    (pCl->pWeather->fRainAmount > (float)10)
//#define RAINY_DAY    (pCl->pWeather->fRainAmount > (float)15)

#define NO3_KIN_SUPRALINEAR (iNO3Kin >= 2)
#define NO3_KIN_MM ((iNO3Kin == 1)||(iNO3Kin == 3))

#define  REWETTING_ENHANCES_MAX_DEN ((pPA->fDenitReWet> 0)&&((iRewet == 1)||(iRewet == 3)||(iRewet == 5)||(iRewet == 7)))
//#define  REWETTING_ENHANCES_MAX_DEN  FALSE

#define  REWETTING_RELEASES_NO3     ((pPA->fDenitReWet> 0)&&((iRewet == 2)||(iRewet == 3)||(iRewet == 6)||(iRewet == 7)))

#define  REWETTING_REDUCES_O2       ((pPA->fDenitReWet> 0)&&(iRewet >= 4))

//#define   REWETTING_ONLY_WITH_CROP
#undef   REWETTING_ONLY_WITH_CROP

#define TEST_RATE(x,y) if (y < EPSILON) {y = EPSILON; x = (float)0;}  \
                       else if ((y - x * DeltaT) < EPSILON) x = (y - EPSILON) / DeltaT;


 
/***********************************************/
/*  external procedures                        */
/***********************************************/

/* from util_fct.c */
extern float abspower(float, float);
extern int   NewDay(PTIME);
extern int   SimStart(PTIME);

extern float WINAPI Polygon4(float, float, float,float,float ,float ,float ,float ,float);
extern float WINAPI RelAnteil(float, float);
extern int   WINAPI Set_At_Least_Epsilon(float *x, LPSTR c);

/* from util_fct.c */
extern int WINAPI Message(long, LPSTR);

extern int WINAPI DebTime(float, int);

/*
extern int WINAPI Deb_Corr(struct ncorr corr, int iLayer, EXP_POINTER, int iOutput);
extern int WINAPI Deb_Corr_Daily(struct ncorr corr, int iLayer, EXP_POINTER);
*/ 

BOOL bDenitRed;
 
/***********************************************/
/*  Hilfsprozeduren                            */
/***********************************************/
_inline int   Reduce_C_Via_Denit(float, float , PSLAYER, PCLAYER);
_inline float Get_Rewet(int, EXP_POINTER);


/***********************************************/
/*  Exportprozeduren                           */
/***********************************************/
int WINAPI DenitXN(EXP_POINTER, float, float, int, int);
int WINAPI DenitLi(EXP_POINTER, int, int);
int WINAPI DenitSoend1(EXP_POINTER);
int WINAPI DenitSoend2(EXP_POINTER);
int WINAPI N2OProdIBW(EXP_POINTER);

int WINAPI DenitCERES(EXP_POINTER);
int WINAPI DenitDAYCENT(EXP_POINTER);

/**********************************************************************************************/
/* module      :   denit.c                                                                    */
/* description :   Denitrifikation                                                            */
/*                 Methode  LeachM V3.0    (Hutson & Wagenet 1992)                            */
/*                 Methode DNDC (Li, Frolking & Frolking, 1992)                               */
/*                 Methode Söndgerath1 und 2                                                  */
/*                                                                                            */
/*                 GSF/ep          18.05.94                                                   */
/*                     ch          13.4.95,                                                   */
/*                                 13.9.95 Einführung der Plot... - Funktionen                */
/*                                 18.9.95 Aufspaltung in miner.c, nitrif.c, ...              */
/*                     dm          01.8.96 Implementieren der neuen Variablenstruktur         */
/**********************************************************************************************/

/*********************************************************************************************/
/* Function    :   Get_Rewet()                                                               */
/* Beschreibung:   Liefert Ergebis zwischen 0 und 1 zur Beschreibung einer Wiederbefeuchtung.*/
/*				   Hoehe der Wiederbefeuchtung richtet sich nach kumulierter                 */
/*                 pot. Evapotranspiration seit dem letzten Wiederbefeuchtungsereignis       */
/*                 (NS > 5 mm)                                                               */
/*                                                                                           */
/*              GSF: ch                     6.11.96                                          */
/*********************************************************************************************/
_inline float Get_Rewet(int iLog, EXP_POINTER)
{
  static float fDryDays;      /* Maß für Intensität der Austrocknung */
  static int   iReWetDay;   /* Simulationszeit des aktuellen (oder letzten) Rewet-Ereignisses */
  static float fRewetEvent;   /* Bewertung des aktuellen (oder letzten) rewet-Events (zwischen 0 und 1) */
  static float LastTime;
 
  float fWet;

	const float fRewetDuration 	= (float)7;//5;//3;//4;//7;       /* Dauer des rewet-Effekts */
	const float fRewetUp 		= (float)3;//1;//1;//1;//2;       /* Verzoegerung bis zum max. Effekt */
	const float fRewetDown 		= (float)3;//1;//1;//2;//3;       /* Zeitpunkt der Abnahme des Effekts */

  float SimTime = pTi->pSimTime->fTimeAct;

  
  #ifdef LOGFILE                            
     char  acdummy[80];
	 LPSTR lpOut = acdummy;
  #endif    
	
  if (SimStart(pTi)) 
  {
   iReWetDay = -99;
   fDryDays = (float)0;
   fRewetEvent = (float)0;
   LastTime = (float)0;
  }

  if (NewDay(pTi) && (SimTime > LastTime + EPSILON))   // Die Überprüfung erlaubt es, 
                                                       // diese Funktion in mehreren Modulen 
        {                                              // aufzurufen. 

	     LastTime  = SimTime;
	     
	     if (pCl->pWeather->fPanEvap > (float)0)
	     	{
	     	fDryDays += pCl->pWeather->fPanEvap;
	     	
	        }
	        else
	        {
	     	fDryDays += (float)6; 
	        }

         if  RAINY_DAY 
            {
	          /* Ein neues rewetting kann fruehestens eine Woche nach dem letzten vorkommen: */
	          if ( SimTime >= iReWetDay + 7)//+ 4)//+ 7)
                 {
	              iReWetDay = (int)SimTime;    /* Heute ist Regentag */
                  
		      	  fRewetEvent = min((float)1, fDryDays/(float)100);		
		      	  //fRewetEvent = (float)1;//ep 310100  min((float)1, fDryDays/(float)100);		
				                         // Normiert am Regenereignis 14.7.95 Damm
	        	  //fRewetEvent*=min((float)1,pCl->pWeather->fRainAmount/(float)30);
	              #ifdef LOGFILE                            

	    	        if (fRewetEvent > (float)0.1)
		               {
					if (iLog) 
						{
    			 		 itoa((int)((float)100*fRewetEvent), lpOut, 10);
						 strcat((LPSTR)lpOut,COMMENT_REWET_TXT);
			      		 Message(0, lpOut);
    		             }     
    		           }
	              #endif
                 }
 	        
	          fDryDays = (float)0;
	          
	        }  /* RainyDay */
	        else
	        {
	     	// !!! Neue Idee:
	     	fDryDays -= pCl->pWeather->fRainAmount;
	     	fDryDays = max(fDryDays,(float)0);
	     	}
	        
	    } /* NewDay */
	    	        
 if (SimTime < iReWetDay + fRewetDuration)
    {
     if ( SimTime < iReWetDay + fRewetUp)
       	fWet = fRewetEvent * (SimTime - iReWetDay)/fRewetUp;
     else
        if ( SimTime < iReWetDay + fRewetDown)
       	  fWet = fRewetEvent;
       	else
          fWet = fRewetEvent * ((float)1 - (SimTime - iReWetDay - fRewetDown)/(fRewetDuration - fRewetDown));
     }
 else 
     fWet = (float)0;

return fWet;
}       


         
/*********************************************************************************************/
/* Procedur    :   Reduce_C_Via_Denit()                                                      */
/* Beschreibung:   Reduziert die Pools Litter, Manure und Humus während                      */
/*                 Denitrifikation.                                                          */
/*                                                                                           */
/*              GSF: ch                     18.9.95                                          */
/*                                                                                           */
/*********************************************************************************************/
/* veränd. Var.		pCL->fCLitter                                                            */
/*					pCL->fCManure                                                            */
/*					pSL->fCHumus                                                              */
/*********************************************************************************************/
_inline int Reduce_C_Via_Denit(float fDenitrifCC, float DeltaT, PSLAYER pSL, PCLAYER pCL)
{
  float fLitterDenitrifCC, fManureDenitrifCC, fHumusDenitrifCC;

  /** Litter-C Abbau  */
  Set_At_Least_Epsilon(&pSL->fCHumus, "pSL->fCHumus");
    
  fLitterDenitrifCC = fDenitrifCC * RelAnteil(pCL->fCLitter, pCL->fCManure + pSL->fCHumus);
  pCL->fCLitter    -= fLitterDenitrifCC * DeltaT;

  /** Manure-C Abbau  */
  fManureDenitrifCC = fDenitrifCC * RelAnteil(pCL->fCManure, pCL->fCLitter + pSL->fCHumus);
  pCL->fCManure    -= fManureDenitrifCC * DeltaT;
    
  /** Humus-C Abbau  */
  fHumusDenitrifCC  = fDenitrifCC  * RelAnteil(pSL->fCHumus, pCL->fCManure + pCL->fCLitter);
  pSL->fCHumus      -= fHumusDenitrifCC * DeltaT;
    
return 1;
} 



/**********************************************************************************************/
/*                                                                                            */
/* Prozedur    :   DenitXN()                                                         */
/* Beschreibung:   Denitrifikation                                                            */
/*                 Verallgemeinerung der                                                      */
/*			  	   Methode LEACHM 3.0 (Hutson & Wagenet, 1992)                                */
/*                                                                                            */
/*                  ch                  10.11.96                                              */
/*				 	                                                                          */
/*					Zusaetzliche Modellparameter:                                             */
/*					                                                                          */
/* 					fN2Fraction 	= N2:(N2+N2O) der Denitrifikationsprodukte. [0<x<1]       */
/* 					                                                                          */
/* 					fN2OReduction	= Abbaurate von N2O zu N2.				    [1/day]       */
/*					                                                                          */
/*					iRewet			= REWETTING_ENHANCES_MAX_DEN            0 oder 1          */
/*									+ 2* REWETTING_RELEASES_NO3             0 oder 1          */
/*									+ 4* REWETTING_REDUCES_O2               0 oder 1          */
/*                                                                                            */
/*					iNO3Kin 		= NO3_KIN_SUPRALINEAR                	0 oder 1          */
/*									+ 2* NO3_KIN_MM                 		0 oder 1          */
/*					                                                                          */
/*					Für Ansatz LeachN: (exp_lp, 0, 0, 0, 3)                                   */
/*					                                                                          */
/**********************************************************************************************/
/* veränd. Var.		pCL->fNO3DenitR                                                           */
/*                  pCL->fN2ODenitR                                                           */
/*					pCL->fNO3N                                                                */
/*					pCL->fN2ON                                                                */
/*					pCL->fN2N                                                                 */
/*					                                                                          */
/*					Reduce_C_via_Denit:	pCL->fCLitter                                         */
/*										pCL->fCManure                                         */
/*										pSL->fCHumus                                          */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI DenitXN(EXP_POINTER, float fN2Fraction, float fN2OReduction, int iRewet, int iNO3Kin)
{
  // DECLARE_COMMON_VAR
  int OK            = 1;
  int iLayer        = 0;
  char  acdummy[80];
  LPSTR lpOut       = acdummy;
  float DeltaT      = pTi->pTimeStep->fAct;
  float SimTime     = pTi->pSimTime->fTimeAct;
  // DECLARE_N_POINTER
  PSLAYER      pSL;
  PCLAYER      pCL;
  PCPARAM      pPA;
  PSWATER      pSW;
  PWLAYER      pWL;
  PHLAYER      pHL;
 
  char  acdummy2[80];
  LPSTR lpOut2= acdummy2; 

  struct ncorr  corr = { (float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1}; 
  float fDenitrifCC;    /* C-Abbaurate durch Denitrifikation */
  float fCLimitDenR;    /* Beschränkung der Denitrifikation durch C-Pools  */

  float fcNO3rewet;
  float fMinWFPS;  
  float fRewetFactor;
  float f1,f2;
  
  float DeltaZ = pSo->fDeltaZ;
  
  N_FIRSTLAYER 

   if (SimStart(pTi))
      {
   if (fN2Fraction >EPSILON)   	
   {
      strcpy(lpOut,"denitrification: N2:(N2O+N2)= ");
      itoa((int)(fN2Fraction*100),lpOut2,10); 
      strcat(lpOut,lpOut2);
      strcat(lpOut,"%.");
      
      Message(0, lpOut);
   }
   
   if (fN2OReduction >EPSILON)
   {
      strcpy(lpOut,"denitrification: N2O->N2 max= ");
      
      if (fN2OReduction > 1)
      {
      itoa((int)(fN2OReduction),lpOut2,10); 
      strcat(lpOut,lpOut2);
      strcat(lpOut,"/day.");
      }
      else
      {
      itoa((int)(100*fN2OReduction),lpOut2,10); 
      strcat(lpOut,lpOut2);
      strcat(lpOut,"%/day.");
      }
      
      Message(0, lpOut);
   }
   
if (NO3_KIN_SUPRALINEAR) 
	{
      strcpy(lpOut,"denitrification: LeachN-Kin. MaxR ");
      itoa((int)(pCh->pCLayer->pNext->fNO3DenitMaxR),lpOut2,10); 
      strcat(lpOut,lpOut2);
      strcat(lpOut," 1/d, KsNO3 ");

      itoa((int)pPA->fDenitKsNO3,lpOut2,10); 
      strcat(lpOut,lpOut2);
      strcat(lpOut," mg NO3-N/l.");

      Message(0, lpOut);
    }
    else
	{
      strcpy(lpOut,"denitrification: MM Kin.  KDenMax(1) ");
      itoa((int)(pCh->pCLayer->pNext->fNO3DenitMaxR),lpOut2,10); 
      strcat(lpOut,lpOut2);
      strcat(lpOut," ng NO3-N/ml/d, KsNO3 ");
      //strcat(lpOut," g/ha/d per layer, KsNO3 ");
      
      itoa((int)pPA->fDenitKsNO3,lpOut2,10); 
      strcat(lpOut,lpOut2);
      strcat(lpOut," mg NO3-N/l.");

      Message(0, lpOut);
    }
    
if ((REWETTING_REDUCES_O2)||(REWETTING_RELEASES_NO3)) 
	{
      strcpy(lpOut,"denitrification: Rewet-Factor ");
      itoa((int)pPA->fDenitReWet,lpOut2,10); 
      strcat(lpOut,lpOut2);

      Message(0, lpOut);
    }

if (pPA->fDenitFreeze > EPSILON) 
	{
      strcpy(lpOut,"denitrification: Freeze-Factor ");
      itoa((int)pPA->fDenitFreeze,lpOut2,10); 
      strcat(lpOut,lpOut2);

      Message(0, lpOut);
    }

if (pPA->fDenitThaw > EPSILON) 
	{
      strcpy(lpOut,"denitrification: Thaw-Factor ");
      itoa((int)pPA->fDenitThaw,lpOut2,10); 
      strcat(lpOut,lpOut2);

      Message(0, lpOut);
    }

       
      } //SimStart
   

	
if ((REWETTING_REDUCES_O2)||(REWETTING_RELEASES_NO3))
   fRewetFactor        = Get_Rewet(1, exp_p);
   else
   fRewetFactor        = Get_Rewet(0, exp_p);
       		
   corr.WiederBefeucht = (REWETTING_ENHANCES_MAX_DEN && (fRewetFactor > EPSILON)) ?

	  /* ch 13.12.96      			*/
#ifdef REWETTING_ONLY_WITH_CROP
      			         fRewetFactor * pPA->fDenitReWet * pPl->pCanopy->fCropCoverFrac + (float)1
#endif
#ifndef REWETTING_ONLY_WITH_CROP
      			fRewetFactor*pPA->fDenitReWet  +(float)1
#endif
         		:
      	 		(float)1;
    
    /* 26.8.97 Für Ausgabe wird eine globale Variable benötigt!
    Benutze die 2. Schicht der Variable pPA->fBioSlowMaxGrowR um den Korrekturfaktor abzuspeichern: */
	DENIT_REWET_FACTOR = corr.WiederBefeucht;
	
	/* ch 13.12.96     Test: 	fRewetFactor * pPA->fDenitReWet * fcropcover +(float)1 */
 
   fcNO3rewet          = (REWETTING_RELEASES_NO3 && (fRewetFactor > EPSILON)) ?
	 				     fRewetFactor * (float)1   /* 0.66 */
	 				   : 
	 				     0;

   /* Berechnung erfolgt von Schicht 1 bis vorletzte  */
   for (N_SOIL_LAYERS)
     { 

     /* Verringerung des Schwellenwertes bzgl Wassergehalt */
     if (REWETTING_REDUCES_O2 && (fRewetFactor > EPSILON))
        {
		 /* 15.12.97 der Schwellenwert soll mithilfe des eingelesenen Werts pPA->fDenitReWet 
					gesenkt werden, falls dieser < 1 ist. */
          fMinWFPS = (pPA->fDenitReWet<=(float)1)?
						  pPA->fDenitThetaMin * (1 - pPA->fDenitReWet* fRewetFactor)
						  :
						  pPA->fDenitThetaMin * (1 - (float)0.3333 * fRewetFactor);
        }
     else
          fMinWFPS = pPA->fDenitThetaMin;
      
      
      Set_At_Least_Epsilon(&pPA->fDenitKsNO3, "fDenitKsNO3"); 

      corr.Temp    = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));

      corr.Feucht  = Polygon4(((pWL->fContAct + pWL->fIce) / pSL->fPorosity), fMinWFPS, (float)0, (float)1, (float)1, (float)2, (float)1, (float)3, (float)1);
            
      corr.Feucht *=  corr.Feucht; /*d=2 */
      

	  //f2=pWL->fContAct*DeltaZ/(float)100.0;	// [mm] -> [dm] i.e. [mg l-1] -> [kg ha-1] !
	  //f2=DeltaZ/(float)100.0;	// [mm] -> [dm] i.e. [mg l-1] -> [kg ha-1] !
	  f2=(float)1;//alte Version: bei Umstellung Standardwerte in yinit.c nicht vergessen! 

      if (NO3_KIN_MM)
         corr.NO3  = RelAnteil(pCL->fNO3N, pPA->fDenitKsNO3*f2);
      else
         corr.NO3  = (float)0.5 * pCL->fNO3N/f2 / pPA->fDenitKsNO3;   /* das stimmt fuer NO3 = Ks mit obiger Definition ueberein. */
      
	  /* Frost-Einfluss */
	  //if (pHL->fSoilTemp > (float)-5)
	  {
	  if (pWL->fWaterFreezeR > 0)
	     corr.Frost = (float)1 + pPA->fDenitFreeze * pWL->fWaterFreezeR;
	  else
	     corr.Frost = (float)1 - pPA->fDenitThaw   * pWL->fWaterFreezeR;
	  }
/*	  else
	  {
		 corr.Frost = (float)1;
	  }
*/  
	/* 26.8.97 Für Ausgabe wird eine globale Variable benötigt!
    Benutze die 3. Schicht der Variable pPA->fBioSlowMaxGrowR um den Korrekturfaktor Frost abzuspeichern: */
	
	DENIT_FROST_FACTOR = (NewDay(pTi))?
			(float)1:
			max(DENIT_FROST_FACTOR, corr.Frost);

      /* Bei REWETTING_RELEASES_NO3 und bei Frostverstaerkung tritt NO3-Freigabe auf: */
         
      if (corr.Frost > (float)2 )
         {
          corr.NO3 = max((float)0.66, corr.NO3);
         }

      corr.NO3 = max(fcNO3rewet,corr.NO3); //ep 180100 auskommentiert!???
	  //if(fcNO3rewet > 1) corr.NO3 = (float)1;//max((float)0.5,corr.NO3);
	  //corr.NO3 = min((float)0.5,max(fcNO3rewet,corr.NO3));//ep 270100
      if (pWL->fContOld/pSL->fPorosity >= (float)0.55) corr.WiederBefeucht = (float)1;//ep 141009
                                           
      /* Denitrifikations - Rate  */  
      fCLimitDenR = (float)0.1 * (pCL->fCLitter + pSL->fCHumus + pCL->fCManure)
                  * (float)56 /(float)72 /DeltaT ;
                                
      pCL->fNO3DenitR = pCL->fNO3DenitMaxR*f2
                      * corr.Temp * corr.Feucht * corr.NO3 * corr.Frost * corr.WiederBefeucht;
     
      if (NO3_KIN_SUPRALINEAR)
         {
          pCL->fNO3DenitR *= pCL->fNO3N;
         }
      else
         {
          //  lpXN->fNO3DenitR *= (float)15;  /* Mittlerer NO3-Gehalt in Kartoffelsimulation */
         }
      
      if (pCL->fNO3DenitR > fCLimitDenR)
         {
          pCL->fNO3DenitR = max((float)0,fCLimitDenR);
         }
      
      
      TEST_RATE(pCL->fNO3DenitR,pCL->fNO3N)

      /* C-Abbau durch Denitrifikation */

      fDenitrifCC = pCL->fNO3DenitR * (float)72/(float)56;

      Reduce_C_Via_Denit(fDenitrifCC, DeltaT, pSL, pCL);
                        
      /* Denitrifikation */

      /**** results ***/

      pCL->fNO3N -= pCL->fNO3DenitR * DeltaT;
      pCL->fN2ON += pCL->fNO3DenitR * DeltaT;

      if ((fN2Fraction >= (float)0) && (fN2Fraction <= (float)1))
      {
      pCL->fN2ON   -= fN2Fraction * pCL->fNO3DenitR *  DeltaT; 
      pCL->fNON  += fN2Fraction * pCL->fNO3DenitR * DeltaT;
      }
      
	  /* 
		Es wird ein Abbau von N2O zu N2 erster Ordnung angenommen.
		Abbaurate: fN2OReduction [1/day]
	  */	 
      if (fN2OReduction > EPSILON)
         {                                                                           
	      /* Test!!!
	      : Nehme an, dass N2O->N2 bei niedrigen Temp. geringer ist als NO3->N2O
	      if (pHL->fSoilTemp < pPA->fMinerTempB)
	      corr.Temp    = abspower(pPA->fMinerQ10*(float)4,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
          */
          
          f1 =  pCL->fNO3DenitMaxR*f2
                      * corr.Temp * corr.Feucht * corr.Frost * corr.WiederBefeucht;
          pCL->fN2ODenitR    = pCL->fN2ON * ((float)1 -(float)exp(-fN2OReduction * f1 * DeltaT))/ DeltaT;
          pCL->fN2N         += pCL->fN2ODenitR * DeltaT;
          pCL->fN2ON        -= pCL->fN2ODenitR * DeltaT;
         }
      
      
      /*
    	     Deb_Corr(corr, iLayer, exp_p, 1); */
    	           
     } /* Schichtweise */

return 0;
} /* DenitXN */
    
    
/*********************************************************************************************/
/* Procedur    :   DenitLi()                                                                 */
/* Op.-System  :   DOS                                                                       */
/* Beschreibung:   Denitrifikation                                                           */
/*                 Methode nach DNDC (Li & Frolking & Frolking, 1992)                        */
/*                                                                                           */
/*              GSF/ch               12.1.95                                                 */
/*                                                                                           */
/*********************************************************************************************/
/* veränd. Var.		pCL->fCMicBiomDenit                                                      */
/*					pCL->fNO3N                                                               */
/*					pCL->fNO3->DenitR                                                        */
/*					pCL->fN2ODenitR                                                          */
/*					pCL->fN2N                                                                */
/*					pCL->fN2ON                                                               */
/*                  pCH->pCProfile->fN2OEmisR                                                */
/*					                                                                         */
/*					#ifdef CSOL_IS_ANTFOS:	pCL->fCSolC                                      */
/*                                                                                           */
/*					Reduce_C_via_Denit()	pCL->fCLitter                                    */
/*											pCL->fCManure                                    */
/*											pSL->fCHumus                                     */
/*					ADD_DENIT_SUM			pCh->pCBalance->fN2NProfile                      */
/*											pCh->pCBalance->fN2ONProfile                     */
/*											pCh->pCLayer->fN2oDenitR                         */
/*					TEST_RATE				pCL->fNO3DenitR                                  */
/*											pCL->fN2ODenitR                                  */
/*					                                                                         */
/*********************************************************************************************/

/* Nicht ausgetestet  dm, 13.1.97 */

int WINAPI DenitLi(EXP_POINTER, int iRewet, int Li_N2O_Emission)
{   
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER

static float fBioMin;  /* Masse der Denitrifikanten kann unter diesen Wert nicht absinken. */
static float fBioMax;  /* Masse der Denitrifikanten kann ueber diesen Wert nicht ansteigen. */
 
  struct ncorr  corr = { (float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1}; 
  
        /* NO3 - Werte */ 
   float fYNO3       = (float)0.401;  /* org.: 0.401 [kg Bio C / kg N]  */
                                      /* Erntefaktor der NO3-Denitrifikanten [kg C/ kg N] */
   float fMNO3       = (float)2.16;   /* org.: 2.16  [kg N / kg Bio day]  = 0.09 kg N/kg Bio h    */
                                      /* Erhaltungskoeffizient der NO3-Denitrifikanten [kg N/ kg C day] */
                                      /* Quelle: Van Versefeld et al. 1977 */
        /* N2O - Werte */ 
    /* Starke Erniedrigung von fYN2O, um Abbau N2O -> N2 zu erreichen. */
    /* Bei Änderung ist das Einlesen des Wertes aus Simfile: SOM2, Schicht 1-3 möglich. */
   float  fYN2O      = (float) 0.428; /* lpx->lPtrCPool->Pnext->SOM2;   /-* org.: 0.428 [kg Bio C / kg N]  */ 
   float  fEmisRate0 = (float) 0.019; /* lpx->lPtrCPool->Pnext->Pnext->SOM2;    /-*  orig: 0.019 1/day */ 
   float  fEmisRate1 = (float) 0.08;  /* lpx->lPtrCPool->Pnext->Pnext->Pnext->SOM2;    /-*  orig: 0.08 1/day */ 

   float fMN2O       = (float) 1.896; /* org.: 1.896 [kg N / kg Bio day] = 0.079 kg N / kg Bio h]    */
                                      /* Quelle: Van Versefeld et al. 1977 */
                                      /* Max. Wachstumsrate = fBioDNO3max + fBioDN2Omax */
                                      /* = 1 1/h */

        /* C - Werte */ 
   float fMC         = (float)0.1824; /* org.:0.1824 [kg C / kg Bio day] = 0.0076 kg C / kg Bio h]    */
                                      /* Erhaltungskoeffizient  */
                                      /* Quelle: Van Versefeld et al. 1977 */
   float fYC         = (float)0.503;  /* org.: 0.503 [kg Bio C / kg C]   */
                                      /*  Erntefaktor [kg C / kg C] */
                                      /* Quelle: Van Versefeld et al. 1977 */
                                      /* dh Sterberate (fMC*fYC) ist 0.091 */ 
   
   float Tod, Wuchs;                  /* dm, 31.7.96 globale Variablen lokal eingeführt */                                   

  /*ch Um BOMD in Grenzen zu halten, wurde folgende Groesse eingeführt: */
  const float fBioRange = (float)5;          /* Faktor, um den BOMD max. vom Anfangswert abweichen kann.                                      */
  
  float fDenitrifCC;    
  float fRewetFactor;
  float f1;
  int   fcNO3rewet;
  
  float DeltaZ = pSo->fDeltaZ;

  N_ZERO_LAYER 

  if (SimStart(pTi))
   {               
    /*!!  ch 1.9.95 Provisorium: Einschränkung von BOMD durch BOMD(1) */
    fBioMin  = pCL->pNext->fCMicBiomDenit / fBioRange;         
    fBioMax  = pCL->pNext->fCMicBiomDenit * fBioRange;         
     
     for (SOIL_LAYERS1(pSL,pSo->pSLayer->pNext))
       if ((pSL->fpH < 1)||(pSL->fPorosity < 0.01))
          {                                            
           // testrange ERROR3BOX("Fehler in Modul: DenitLi()","Fehlerhafte Eingabedaten!\n (pH, Porenvolumen)");
           return  -1;
          }                  

   }   /* Simulationsstart */


   /***************** Profilsummen nullsetzen (Schicht 0) **********/
  //ZERO_DENIT_SUM
    

  if(Li_N2O_Emission)                 /* Nur bei Emission nach Li wird ueber Schichten Kumuliert! */
  	    pCh->pCProfile->fN2OEmisR = (float)0;

if (REWETTING_ENHANCES_MAX_DEN) 
   fRewetFactor        = Get_Rewet(1, exp_p);
   else
   fRewetFactor        = Get_Rewet(0, exp_p);
       		
  
  corr.WiederBefeucht = (REWETTING_ENHANCES_MAX_DEN && (fRewetFactor > EPSILON))?
      		   	         fRewetFactor * pPA->fDenitReWet + (float)1
         		      :
      	 		        (float)1;
      	 		        
  fcNO3rewet = (REWETTING_RELEASES_NO3 && (fRewetFactor > EPSILON))? 1 : 0;
  
  /**********  Berechnung erfolgt von Schicht 2 bis vorletzte   */
  for (N_SOIL_LAYERS)
      {
      #ifdef DEB_TIME
      if (SimTime > START_TIME_DEB) DebTime(SimTime, iLayer + 2000);
      #endif      

  /************* Korrektur-Faktoren ***************************************/

  /** Temperatur-Korrektur nach Li */
  corr.Temp = abspower((float)2,(pHL->fSoilTemp - (float)22.5) / (float)10);

  /** Feuchte-Korrektur nach Johnsson */
  corr.Feucht = Polygon4(((pWL->fContAct + pWL->fIce) / pSL->fPorosity), pPA->fDenitThetaMin, (float)0, (float)1, (float)1, (float)2, (float)1, (float)3, (float)1);
               
  corr.Feucht *= corr.Feucht;     /* d=2 */

  /** pH Korrektur:    (nach Li et al)                                        */
  corr.ph  =  Polygon4(pSL->fpH, (float)3.8, (float)0, (float)7, (float)1, (float)8, (float)1, (float)9, (float)1); 
  corr.ph2 =  Polygon4(pSL->fpH, (float)4.4, (float)0, (float)7, (float)1, (float)8, (float)1, (float)9, (float)1); 
            
  /* Frost-Einfluss */
  //corr.FrostG = (float)1 + (float)fabs(lpXT->fIceFactorGrowth * pWL->fWaterFreezeR);

  corr.Frost =  (pWL->fWaterFreezeR > 0)?
		        (float)1 + pPA->fDenitFreeze * pWL->fWaterFreezeR
		     :  
		        (float)1 - pPA->fDenitThaw   * pWL->fWaterFreezeR;
	            
 
  /** Korrektur durch gelösten C */
  #ifdef CSOL_IS_ANTFOS           
     pCL->fCsolC = pCL->fCLitter;       /* Litter als Csol */
  #endif

  f1=pWL->fContAct*DeltaZ/(float)100.0;
  /* Csol nach Li = Mineralisierungsaktivitaet */
  corr.Csol = RelAnteil(pCL->fCsolC,pPA->fDenitKsCsol); /* Michaelis-Menten Kinetik */

  corr.NO3  = RelAnteil(pCL->fNO3N,pPA->fDenitKsNO3*f1);
            
  /** Relative Wachstums - Raten  */
  corr.DNO3rel = pPA->fDenitMaxGrowR * corr.Csol * corr.NO3;
  corr.DN2Orel = (pPA->fDenitMaxGrowR /(float)2.0) * corr.Csol
               * RelAnteil(pCL->fN2ON,pPA->fDenitKsNO3*f1); 
       
  corr.DNrel = corr.Temp * corr.Feucht  * corr.WiederBefeucht
             * (corr.DNO3rel * corr.ph + corr.DN2Orel * corr.ph2 );


  /* ********************** BOMD Dynamik: ********************************* */
  Tod   =  (pCL->fCMicBiomDenit > fBioMin) ?        
           pPA->fMaintCoeff * fYC * (pCL->fCMicBiomDenit - fBioMin) : (float) 0; 
                
  Wuchs =  (pCL->fCMicBiomDenit < fBioMax) ?
           corr.DNrel * ((fBioMax - pCL->fCMicBiomDenit) / fBioMax) * pCL->fCMicBiomDenit 
           : (float) 0;     
                      
            
  pCL->fCMicBiomDenit  += (Wuchs - Tod) * DeltaT;

  Set_At_Least_Epsilon(&pCL->fNO3N, "pCL->fNO3N"); 
                
  /****************************    Denitrifikationsrate ***************** */
      pCL->fNO3DenitR = ( corr.DNO3rel / fYNO3 + fMNO3 * RelAnteil(pCL->fNO3N,pCL->fN2ON) )
                * pCL->fCMicBiomDenit * corr.ph * corr.Feucht * corr.Frost * corr.WiederBefeucht;
        
      pCL->fN2ODenitR  = ( corr.DN2Orel / fYN2O + fMN2O * RelAnteil(pCL->fN2ON,pCL->fNO3N) )
                * pCL->fCMicBiomDenit * corr.ph2 * corr.Feucht * corr.Frost * corr.WiederBefeucht;
                                   
      /* C-Abbau durch Denitrifikation  */
      fDenitrifCC = ( corr.DNrel / fYC + pPA->fMaintCoeff) * pCL->fCMicBiomDenit;
        
      /*!! LEACH Beschränkung durch C Gehalt aufgehoben. */
      Reduce_C_Via_Denit(fDenitrifCC, DeltaT, pSL, pCL);
                        
      /** Denitrifikation */
      TEST_RATE(pCL->fNO3DenitR,pCL->fNO3N)
      TEST_RATE(pCL->fN2ODenitR,pCL->fN2ON)


      /*************************  Veraenderung der NOx-Pools *************** */
      pCL->fNO3N        -= pCL->fNO3DenitR * DeltaT;
        
      pCL->fN2ON   += (pCL->fNO3DenitR - pCL->fN2ODenitR) * DeltaT;
      pCL->fN2N    += pCL->fN2ODenitR * DeltaT;

      /**************************** N2O-Emission Tageswert *****************************/
            /* AD = 1 bei Li */

      if(Li_N2O_Emission)
       {
            pCP->fN2OEmisR    = (fEmisRate0 + fEmisRate1 * ((float)1 - ((pWL->fContAct + pWL->fIce) / pSL->fPorosity)))
                                * pCL->fN2ON;
                              
            pCL->fN2ON       -= pCP->fN2OEmisR * DeltaT;
 
       } /* Li_N2O_Emission */
    

     /*  Profilsummen */
     //ADD_DENIT_SUM

     /*
     #define DEB_OUTPUT  ((NewDay(pTi))||(pGr->iDllGraphicNum > 100))

     if (pGr->iDllGraphicNum  == 2)
        {                                     
          if DEB_OUTPUT
   	         Deb_Corr(corr, iLayer, exp_p, 1);
	      else
		     Deb_Corr(corr, iLayer, exp_p, 0);

        } 
     else
	     if (pGr->iDllGraphicNum  == 102)
	  	     Deb_Corr_Daily(corr, iLayer, exp_p);
     */

  } /* for */
 
  return 1;
}               /* Denit Li */
 
/*********************************************************************************************/
/* Procedur    :DenitSoend1()                                                                */
/* Op.-System  :   DOS                                                                       */
/* Beschreibung:   Denitrifikation  nach D. Söndgerath                                       */
/*                                                                                           */
/*              GSF/ch                  22.11.94                                             */
/*                                                                                           */
/*********************************************************************************************/
/* veränd. Var.		pCL->fNO3DenitR                                                          */
/*					pCL->fNO3N                                                               */
/*					                                                                         */
/*                  Reduce_C_via_Denit()	pCL->fCLitter                                    */
/*											pCL->fCManure                                    */
/*											pSL->fCHumus                                      */
/*                                                                                           */
/*                  TEST_RATE				pCL->fNO3DenitR                                  */
/*                                                                                           */
/*********************************************************************************************/

/* Nicht ausgetestet  dm, 13.1.97 */

int WINAPI DenitSoend1(EXP_POINTER)
{
DECLARE_COMMON_VAR
  
  struct ncorr  corr = { (float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1}; 
  float fTkrit;
  float fFkrit;
  float falpha;
  float fbeta;     
  float fCTotal, fDenitrifCC;
  float f1;

  float DeltaZ = pSo->fDeltaZ;

  DECLARE_N_POINTER
  N_ZERO_LAYER 

  fTkrit = (float)16.01;
  fFkrit = (float)0.4371;
  falpha = (float)4.7;
  fbeta = (float)4.0;

  /* Berechnung erfolgt von Schicht 2 bis vorletzte  */
  for (N_SOIL_LAYERS) 
    {
        #ifdef DEB_TIME
        if (SimTime > START_TIME_DEB) DebTime(SimTime, iLayer + 2000);
        #endif
              
        corr.Temp   = (float)(1 - exp((double)(-abspower((pHL->fSoilTemp/fTkrit),falpha))));
        corr.Feucht = (float)(1 - exp((double)(-abspower(((pWL->fContAct + pWL->fIce)/fFkrit),fbeta))));
            
        /** Denitrifikations - Rate   */
        /* orig.: DenNO3max = 0.5 kg N2O/ha/day = 0.5 * 28 / 44 kg N/ ha/d = 0.318 kg N/ ha/d            */
        /* orig.: KsDenNO3  = 12.73 kg N/ ha             */
        f1=pWL->fContAct*DeltaZ/(float)100.0;    
        corr.NO3 = RelAnteil(pCL->fNO3N * pCL->fNO3N, pPA->fDenitKsNO3*f1 * pPA->fDenitKsNO3*f1);
        pCL->fNO3DenitR = pCL->fNO3DenitMaxR*f1 * corr.NO3 * corr.Temp * corr.Feucht;

        TEST_RATE(pCL->fNO3DenitR,pCL->fNO3N)

        /* C-Abbau durch Denitrifikation */
        fCTotal = pSL->fCHumus + pCL->fCLitter + pCL->fCManure;
            
            
        fDenitrifCC = min((float)0.1 * fCTotal / DeltaT,   /* max. 10% Abbau pro Zeitschritt */
                              pCL->fNO3DenitR * (float)72.0 / (float)56.0);

        Reduce_C_Via_Denit(fDenitrifCC, DeltaT, pSL, pCL);

            /* Denitrifikation **** results ***/
        pCL->fNO3N                   -= pCL->fNO3DenitR * DeltaT;
    } /* Schichtweise  */
    
  return 1;
}   


/*********************************************************************************************/
/* Procedur    :   DenitSoend2()                                                             */
/* Op.-System  :   DOS                                                                       */
/* Beschreibung:   Denitrifikation                                                           */
/*                 Methode 2 von D. Söndgerath                                               */
/*                                                                                           */
/*              GSF/ch                  24.11.94                                             */
/*                                                                                           */
/*********************************************************************************************/
/* veränd. Var.		pCL->fNO3DenitR                                                          */
/*					pCL->fNO3N                                                               */
/*                                                                                           */
/*                  Reduce_C_via_Denit()	pCL->fCLitter                                    */
/*											pCL->fCManure                                    */
/*											pSL->fCHumus                                      */
/*                                                                                           */
/*                  TEST_RATE				pCL->fNO3DenitR                                  */
/*                                                                                           */
/*********************************************************************************************/

/* Nicht ausgetestet  dm, 13.1.97 */

int WINAPI DenitSoend2(EXP_POINTER)
{
  DECLARE_COMMON_VAR

  struct ncorr  corr = { (float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1}; 
  float cTemp2;             /* Temperatur-Korrektur */
  float cFeucht2;           /* Feuchte-Korrektur */
  float fTkrit1,fTkrit2,fTkrit3;
  float fFkrit1,fFkrit2;
  float falpha;
  float fbeta;            
  float fp;     
  float fDenitrifCC;
  float f1;

  float DeltaZ = pSo->fDeltaZ;

  DECLARE_N_POINTER
  N_ZERO_LAYER 

  /* lpXT->DenNO3max    = (float)0.167;            /[kg/ha/d] */
  /* lpXT->KsDenNO3 = (float)12.7;                 / [kg/ha] ^2=161.93;   */ 
  fTkrit1  = (float)2.91;
  fTkrit2  = (float)30.0;
  fTkrit3  = (float)12.09;
   
  fFkrit1  = (float)0.7501;
  fFkrit2  = (float)0.1765;
   
  falpha   = (float)4.7;
  fbeta    = (float)4.0;    
  fp       = (float)0.07;
   


  /* Berechnung erfolgt von Schicht 2 bis vorletzte  */

  for (N_SOIL_LAYERS) 
    {

       corr.Temp = (float)( (1 - exp( (double)(-abspower(pHL->fSoilTemp/fTkrit1,falpha)) ))*exp((double)(-abspower(pHL->fSoilTemp/fTkrit2, fbeta))) );
       cTemp2 = (float)(1 - exp( (double)(-abspower(pHL->fSoilTemp/fTkrit3,falpha)) ));
       
       corr.Feucht = (float)(1 - exp( (double)(-abspower((pWL->fContAct + pWL->fIce)/fFkrit1,fbeta)) ));
       cFeucht2 = (float)( (1 - exp( (double)(-abspower((pWL->fContAct + pWL->fIce)/fFkrit2,fbeta)) ))*exp((double)(-abspower(pWL->fContAct/fFkrit1,fbeta))) );

	   f1=pWL->fContAct*DeltaZ/(float)100.0;
       /* Denitrifikations - Rate  */
       corr.NO3 = RelAnteil(pCL->fNO3N * pCL->fNO3N,pPA->fDenitKsNO3*f1 * pPA->fDenitKsNO3*f1);
       
       pCL->fNO3DenitR = corr.NO3 * pCL->fNO3DenitMaxR*f1 * corr.Temp * corr.Feucht + pCL->fNO3DenitMaxR/f1 * fp * cTemp2 * cFeucht2;
       TEST_RATE(pCL->fNO3DenitR,pCL->fNO3N)

       /* C-Abbau durch Denitrifikation */
       fDenitrifCC = pCL->fNO3DenitR * (float)72.0 / (float)56.0;

       Reduce_C_Via_Denit(fDenitrifCC, DeltaT, pSL, pCL);

       /* Denitrifikation *** results ***/
       pCL->fNO3N                    -= pCL->fNO3DenitR * DeltaT;
 
    } /* Schichtweise */
  
  return 1;
}

/*********************************************************************************/
/*  Name     : DenitCERES                                              */
/*                                                                               */
/*  Funktion : Berechnung der Stickstoffverluste aufgrund denitrifizierender     */
/*             Prozesse. Die Beschreibung erfolgt nach CERES/Godwin mit Ver-     */
/*             aenderungen von Engel/Schaaf                                      */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCh->pCLayer->fNO3N                                               */
/*                                                                               */
/*********************************************************************************/
int WINAPI DenitCERES(EXP_POINTER)							                          
{
	float		nitppm,ppmTokg,fakW,fakT,gesamtC,wasloesC,miniNit;
	float       DeltaT  = pTi->pTimeStep->fAct;
    BOOL Calculate;	

	PSLAYER		akt0;
	PCLAYER		akt1;
	PHLAYER		akt2;
	PWLAYER		akt3;
	PSWATER		akt4;	                
	

    for (akt0 = pSo->pSLayer->pNext,
         akt1 = pCh->pCLayer->pNext,
         akt2 = pHe->pHLayer->pNext,
         akt3 = pWa->pWLayer->pNext,
         akt4 = pSo->pSWater->pNext;
                                   ((akt0->pNext != NULL)&&
                                    (akt1->pNext != NULL)&&
                                    (akt2->pNext != NULL)&&
                                    (akt3->pNext != NULL)&&
                                    (akt4->pNext != NULL));
         akt0 = akt0->pNext,
         akt1 = akt1->pNext,
         akt2 = akt2->pNext,         
         akt3 = akt3->pNext,         
         akt4 = akt4->pNext)
    {        
		akt1->fNO3DenitR = (float)0;
		
		Calculate = TRUE;

		ppmTokg = akt0->fBulkDens * (float)0.1 * (akt0->fThickness / 10);

		nitppm = (akt1->fNO3N / ppmTokg);
		
		if (nitppm < (float)1.0)
		{	Calculate = FALSE;
		}
		
		if (akt2->fSoilTemp < (float)1.0)
		{	Calculate = FALSE;
		}

		if (akt3->fContAct < akt4->fContFK)
		{	Calculate = FALSE;
		}
		
		if (Calculate)
		{
          fakW = (float)0.0;
          //gesamtC = (akt0->fCHumus * (float)0.0031) + (akt1->afCOrgFOMFrac[1] * (float)0.4);// ??? T. Schaaf
          gesamtC = (akt0->fCHumus * (float)0.58) + (akt1->afCOrgFOMFrac[1] * (float)0.4);//ep 120901 nach CERES-Maize
          gesamtC *= (float)0.0031;									   //ep 120901 und modeling plant and soil systems
		  wasloesC = (gesamtC / ppmTokg) + (float)24.5;
		  fakW = (float)1.0 - ((akt4->fContSatCER - akt3->fContAct) / (akt4->fContSatCER - akt4->fContFK));
		  fakT = (float)0.1 * (float)exp((double)(float)0.046 * akt2->fSoilTemp);

		  akt1->fNO3DenitR = (float)0.00006 * wasloesC * fakW * fakT * nitppm 
		  								* (akt0->fThickness / 10) * akt0->fBulkDens;
		  miniNit = (float)1.0 * ppmTokg;
		  
		  if (akt1->fNO3DenitR * DeltaT > (akt1->fNO3N - miniNit))
		  {
			if ((akt1->fNO3N - miniNit) < (float)0.0)		  
			 {akt1->fNO3DenitR  = (float)0.0;
			 } // Ende if (akt1->fNO3DenitR * DeltaT > (akt1->fNO3N - miniNit))                       
			else
			 {akt1->fNO3DenitR  = (akt1->fNO3N - miniNit)/DeltaT;
			 } // Ende else (akt1->fNO3DenitR * DeltaT > (akt1->fNO3N - miniNit))                       
          } /* Ende if (akt1->fNO3DenitR * DeltaT > (akt1->fNO3N - miniNit)) */

 	/* Denitrifizierter Stickstoff wird vom Nitratpool abgezogen 
 	      und dem N2O-Pool zugegeben. */
			akt1->fNO3N -= akt1->fNO3DenitR * DeltaT;
			akt1->fN2ON += akt1->fNO3DenitR * DeltaT;

        } /* Calculate ist True */
        else               
        {
        akt1->fNO3DenitR = (float)0;
        }
        
	} /* Ende der schichtweisen Berechnung */

  return 1;
}  /*  Ende Berechnung Denitrifikation */


/*********************************************************************************************/
/* Procedur    :   N2OProdIBW()                                                              */
/* Op.-System  :   DOS                                                                       */
/* Beschreibung:   N2O Produktion Regression fuer Solling                                    */
/*                                                                                           */
/*                 ep/gsf               11.05.99                                             */
/*                                                                                           */
/*********************************************************************************************/
/* veränd. Var.: pCL->fNO3N		                                                             */
/*               pCL->fN2ON                                                                  */
/*********************************************************************************************/


int WINAPI N2OProdIBW(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 int   iflag;	 
 float fA,fB,fC,fD,fN2OProfileDepth,fSimDepth,fT,fWCA,fWCS;
 float DeltaZ = pSo->fDeltaZ;
 double dWFPS;

 DECLARE_N_POINTER
 N_ZERO_LAYER 

 iflag = (int) 0;
 fT    = (float) 0;
 fWCA  = (float) 0;
 fWCS  = (float) 1;

 //Buche als Standard
 fA = (float)2.7e34;//1.0e34;//1.0e31;//1.0e35;
 fB = (float)175;//173.5;//150;//180;
 fC = (float)61;//62.34;//60;
 
 if ((!lstrcmp(pPl->pGenotype->acCropCode,"FW\0"))||(!lstrcmp(pPl->pGenotype->acCropCode,"SP\0"))) 
   {//Fichte
    fA = (float)1.0e33;
    fB = (float)175;
    fC = (float)43;
   }

 fN2OProfileDepth = (float) 300;
 fSimDepth =(float) 0;
 //pCh->pCProfile->fN2OEmisR = (float)0;
 
 ///*
 // Berechnung erfolgt von Schicht 2 bis vorletzte  
 // Temperatur und Wassergehalt in/unter 5 cm Tiefe	Mineralboden 
  for (N_SOIL_LAYERS) 
  {
   if ((!lstrcmp(pSL->acSoilID,"Str\0"))||(!lstrcmp(pSL->acSoilID,"Hum\0")))
   {
	fSimDepth=(float)0;
   }
   else
   {
    fSimDepth += DeltaZ;
   }
   
   if((fSimDepth>=(float)50)&&(iflag==0))
	{
	 iflag = (int)1;
	 fT   = pHL->fSoilTemp;
	 fWCA = pWL->fContAct;
	 fWCS =	pSW->fContSat;
	}
  }
  N_ZERO_LAYER
  fSimDepth=(float)0;
 //*/
 
 /* Berechnung erfolgt von Schicht 2 bis vorletzte  */
  for (N_SOIL_LAYERS) 
    {
	 /*
	 fT   = pHL->fSoilTemp;
	 fWCA = pWL->fContAct;
	 fWCS = pSW->fContSat;
	 */
	 dWFPS=(double)100 * (double)fWCA/(double)fWCS;
	 //dWFPS=(double)100 * (double)pWL->fContAct/(double)pSW->fContSat;
     fD	= fA * (float)exp(-(double)fB/((double)0.008314 * ((double)273.15 + (double)fT)))
	     * (float)exp(pow(dWFPS,(double)1.16)/(double)fC)
       	 /(float) 1e+5; //mug/m2/d -> kg/ha/d

	 pCL->fNO3DenitR = (fD/fN2OProfileDepth) * DeltaZ;

	 if((pTi->pSimTime->iJulianDay > 152)&&(dWFPS < 40))
	   bDenitRed=TRUE;
	 if(((pTi->pSimTime->iJulianDay > 335)&&(dWFPS > 40))&&(bDenitRed==TRUE))
	   bDenitRed=FALSE;

	 //if(bDenitRed==TRUE) pCL->fNO3DenitR *= (float)0.2;
	 // ep/gsf 07.11.01 eingefuegt zum einlesen mit readmod.c von 20012  
	 if(bDenitRed==TRUE) pCL->fNO3DenitR *= pCh->pCParam->pNext->fN2ORedMaxR;

	 fSimDepth += DeltaZ;
	 if (fSimDepth > fN2OProfileDepth) 
	 {
	  if ((fSimDepth - fN2OProfileDepth) < DeltaZ)
	  {
	      pCL->fNO3DenitR = (fD/fN2OProfileDepth) * (fSimDepth - fN2OProfileDepth);
	  }
	  else
	  {
		  pCL->fNO3DenitR = (float) 0;
	  }
	 }
/*
	 if (iLayer < 9) 
	 {
		 pCL->fNO3DenitR =fD * ((float)3/fN2OProfileDepth) * DeltaZ;
	//	 pCh->pCProfile->fN2OEmisR = fD/((float)24e-5);
	 }
	 else
	 {
		 pCL->fNO3DenitR = (float) 0;
	 }
*/
	 //pCh->pCProfile->fN2OEmisR += pCL->fNO3DenitR / ((float)24e-5);//kg/ha/d -> mug/m2/h

	 // Denitrifikation  results
     pCL->fNO3N -= pCL->fNO3DenitR * DeltaT;
	 pCL->fN2ON += pCL->fNO3DenitR * DeltaT;
    
	} // Schichtweise 
      
  return 1;
} 


extern int iForest;
/*********************************************************************************/
/*  Name     : DenitDAYCENT                                                      */
/*                                                                               */
/*  Funktion : Berechnung der Stickstoffverluste aufgrund denitrifizierender     */
/*             Prozesse.                                                         */
/*                                                                               */
//  Modell   : Parton et al.: Model for NOx and N2O Emissions, 
//			   Journal of Geophysical Research, vol. 106, (2001)
//
//             See also DayCent reference 'NOx Submodel'
//
/*                                                                               */
/*  Autor    : S. Bittner                                                        */
/*  Datum	 : 24.06.09                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*             pCh->pCLayer->fNO3N                                               */
/*                                                                               */
/*********************************************************************************/
int WINAPI DenitDAYCENT(EXP_POINTER)
{
int i,j;
PSLAYER  pSL=pSo->pSLayer->pNext;	// used to iterate over the linked list
PCLAYER	 pCL=pCh->pCLayer->pNext;
PWLAYER  pWL=pWa->pWLayer->pNext;	 
PSWATER  pSW=pSo->pSWater->pNext;
//static struct parameter par;
static float ppt2lastweeks[14];//the daily rain amount of the last 2 weeks [cm] is needed for
								//the calculation of the NOx pulse due to rain on dry soil
float sumppt;//rain amount of the last 2 weeks [cm]
static float NOxPulse[12]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};//NOx pulse factor of the next 12 days 
double co2PPM[1000]; //newCO2 [ppm]
double grams_soil[1000]; //grams of soil in a layer in a 1m x 1m section
float  nitratePPM[1000];//soil nitrate (NO3-) concentration (ppm)
double fluxNTotal[1000];// soil (N2+N2O) denitrif. flux by lyr ( gN/m^2 )
double fluxTotalDenitPPM; // total (N2+N2O) denitrif. flux (ppm N/day)
double gramsSoil;   		// soil mass in layer (g m-2)
double sum=0.0;		//normalization factor for soil exp. distribution
double fluxN2Odenit[1000];	// N2O flux from denitrification (gN/m2/day)
double fluxN2denit[1000];	// N2 flux from denitrification (gN/m2/day)
double fluxNOdenit[1000];	// NO flux from denitrification (gN/m2/day)
float  porosity; 
float  wfps_fc,dD0_fc;	// water filled pore space at field capacity (0-1)
double WFPS_threshold;//  water filled pore space threshold (0-1)
double WFPS;
double co2PPM_correction;
double a;
float  fDno3;		// Nitrate effect on denitrification
double fDco2;		//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
double fDwfps, M, x_inflection;		// wfps effect on denitrification (fDwfps, 0-1?)
double fRno3_co2, k1;//  Nitrate effect on the ratio of N2 to N2O
double fRwfps;		// WFPS effect on the N2/N2O Ratio
double ratioN2N2O;	// N2:N2O Ratio
double fluxN2O ;
double excess = 0.0;	// amount of N flux in excess of what
				//   can actually be removed from soil (gN/m^2)
double totalFlux;
double n2oFrac ; // N2O fraction of flux
double n2Frac  ; // N2 fraction of flux
double minNitrate_final;
double fluxOut;
double kPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
double potentialFluxDenitrifNO;	//   maximum possible denitrification NO flux
								//   based on NO/N2O and kPrecipNO (gN/m^2/day)
double NO_N2O_ratio;//ratio NO:N2O
//constants
	float const ug_per_gram = 1.0E6f;		// micrograms per gram (ppm)
	float const grams_per_ug = 1.0E-6f;		// grams per microgram
	// min. nitrate concentration required in a lyr for trace gas calc. (ppm N)
	double const minNitratePPM = 0.1;
	// min. allowable nitrate per lyr at end of day (ppm N)
	double const minNitratePPM_final = 0.05;
	float const cm2m2 = 10000.0f;	// cm^2 per m^2
	float A[4] =	        // parameters to parton-innis functions
		{ 9.23f, 1.556f, 76.91f, 0.00222f };
    double const tolerance = 1.0E-30;


if (NewDay(pTi))
{
	//  multiplier NOx flux based on recent rain events and snowpack
    // increase of NO due to moisture and rain >= 1.0
	//shift the daily values and add current day's ppt [cm]
	for(j=0;j<13;j++)//shift the daily values and add current day's ppt [cm]
	{
		ppt2lastweeks[j]=ppt2lastweeks[j+1];
	}
	ppt2lastweeks[13] = pCl->pWeather->fRainAmount*0.1f;
	//sum of rain during last 2 weeks:
	sumppt=0.0;
	for(j=0;j<14;j++)//shift the daily values and add current day's ppt [cm]
	{
		sumppt += ppt2lastweeks[j]; 
	}
	//get pulse factor for today
	kPrecipNO = NOxPulse[0];
	for(j=0;j<11;j++)//shift the daily values and add factor 1.0 for the last day
	{
		NOxPulse[j]=NOxPulse[j+1];
	}
	NOxPulse[11]=1.0;

	//calculate future pulses
	//new pulse, if sum is lower than 1cm and no snow
	if (sumppt <= 1.0 && (pCl->pWeather->fSnow==0.0) )
	{
		if(pCl->pWeather->fRainAmount*0.1f <= 0.5)//low rain event
		{
			for(j=0;j<1;j++)//low rain event pulses last 1 day
			{
				NOxPulse[j] = max(NOxPulse[j], 11.19f *  (float)exp(-0.805f * (j+2)) );
			}
		}
		 if(pCl->pWeather->fRainAmount*0.1f > 0.5//medium rain event
			&& pCl->pWeather->fRainAmount*0.1f <= 1.5)
		{
			for(j=0;j<5;j++)//medium rain event pulses last 5 days
			{
				NOxPulse[j] = max(NOxPulse[j], 14.68f * (float)exp(-0.384f * (j+2)) );
			}
		}
		if(pCl->pWeather->fRainAmount*0.1f > 1.5)//high rain event
		{
			for(j=0;j<12;j++)//large rain event pulses last 12 days
			{
				NOxPulse[j] = max(NOxPulse[j], 18.46f * (float)exp(-0.208f * (j+1)) );
			}
		}
	}

	//*********************************************************************
	// Convert newCO2 (g/m2) to co2PPM[] (ppm) and distrbute it
    // through the soil profile  
	//*********************************************************************
	for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext,pCL=pCL->pNext)
	{
		gramsSoil = pSL->fBulkDens  * pSL->fThickness * 0.1   * 1.0e4f;//soil mass in layer (g m-2) 
		if(gramsSoil <= 0.0) Message(1, "Denitrify Error: Devison by 0");
		co2PPM[i] = pCL->fCO2C*kgphaTOgpm2 / gramsSoil * 1.0E6f;	
		if(co2PPM[i]<1e-30) co2PPM[i]=0.0;
	}

	//*********************************************************************
    // grams of soil in a layer in a 1m x 1m section
	//*********************************************************************
	for(pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext)
	{
		grams_soil[i] = pSL->fBulkDens  * pSL->fThickness * 0.1 * cm2m2;
	}

	//*********************************************************************
	// soil nitrate (NO3-) concentration (ppm)
	//*********************************************************************
	for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2; i++,pCL=pCL->pNext)
	{
		nitratePPM[i] = pCL->fNO3N * kgphaTOgpm2 / (float)grams_soil[i] * ug_per_gram;
	}

	//********************************************************************* 
	// denitrification loop
    //  Dentrification occurs over all layers
	//*********************************************************************
    //  Convert nitrate (gN/m2) to nitratePPM (ppm N)
	for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater->pNext,i=0; 
		i<pSo->iLayers-2; 
		i++, pSL= pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext,pSW=pSW->pNext)
    {
		if (nitratePPM[i] < minNitratePPM) continue;

		// normalized diffusivity in aggregate soil
		//   media, at a standard field capacity (0-1)
		//dD0_fc = diffusiv(&stdfieldc, &stdbulkd, &wfps_fc);
		//dD0 calc changed 6/20/00 -mdh
		// water filled pore space at field capacity (0-1)
		porosity = 1.0f - pSL->fBulkDens / (float)2.65;
		wfps_fc = pSW->fContFK / porosity;
		wfps_fc = max ( 0.0f, wfps_fc );
		wfps_fc = min ( wfps_fc, 1.0f );
		dD0_fc = Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,wfps_fc);
		if(dD0_fc < 0 || dD0_fc > 1.0 ) Message(1,"Warning - Denitrification: dD0_fc out of range");

		// water filled pore space threshold (0-1)	
		WFPS_threshold =
			(dD0_fc >= 0.15f) ? 0.80f : (dD0_fc * 250.0f + 43.0f) / 100.0f;
		if(WFPS_threshold < 0 || WFPS_threshold > 1.0 ) 
			Message(1,"Warning - Denitrification: WFPS_threshold out of range");
		WFPS = pWL->fContAct / pSL->fPorosity;

		// CO2 correction factor when WFPS has reached threshold
		if (WFPS <= WFPS_threshold)
		{
			co2PPM_correction = co2PPM[i];
		}
		else
		{
			a = (dD0_fc >= 0.15f) ? 0.004f : (-0.1f * dD0_fc + 0.019f);
			co2PPM_correction = co2PPM[i] * (1.0f + a * (WFPS - WFPS_threshold) * 100.0f);
		}
		if(co2PPM_correction < 0.0f) 
		{
			Message(1,"Warning - Denit.: co2PPM_correction < 0.0");
		}

		// Nitrate effect on denitrification
		// denitrification flux due to soil nitrate (ppm N/day)
		// Changed NO3 effect on denitrification based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		// fDno3 = 1.15 * std::pow(nitratePPM(layer), 0.57);
		fDno3 = max( 0.0f, f_arctangent(nitratePPM[i], A) );

		//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
		//  Changed CO2 effect on denitrification based on
		//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		fDco2 = max(0.0,(0.1 * pow(co2PPM_correction, 1.3) - minNitratePPM) );

		// wfps effect on denitrification (fDwfps, 0-1?)
		// Changed wfps effect on denitrification based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		M =  min(0.113f, dD0_fc) * (-1.25) + 0.145;
		x_inflection = 9.0 - M * co2PPM_correction;
		fDwfps = max ( 0.0, 0.45 +
			(atan(0.6 *  3.14159265358979  * (10.0 * WFPS - x_inflection))) / 3.14159265358979  );

		//  N fluxes (N2 + N2O) for the current layer, ppm N
		fluxTotalDenitPPM = // total (N2+N2O) denitrif. flux (ppm N/day)
			(fDno3 < fDco2) ? fDno3 : fDco2;
		if(fluxTotalDenitPPM < 0.0)Message(1,"Warning - Denit.: fluxTotalDenitPPM < 0.0");
		// Minimum value for potential denitrification in simulation layers.
	    fluxTotalDenitPPM = max(0.066, fluxTotalDenitPPM);
		fluxTotalDenitPPM *= fDwfps;

		//  Nitrate effect on the ratio of N2 to N2O
		//  Maximum N2/N2O ratio soil respiration function
		//  Changed the NO3 and CO2 effect on the N2/N2O ratio based on
		//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		k1 = max (1.5, 38.4 - 350 * dD0_fc);
			if(k1 < 0.0) Message(1,"Warning - Denit.: k1 < 0.0f");
		fRno3_co2 = max (0.16 * k1,
				k1 * exp(-0.8 * nitratePPM[i] / co2PPM[i]) );
			if(fRno3_co2 < 0.0) Message(1,"Warning - Denit.: fRno3_co2 < 0.0");

		// WFPS effect on the N2/N2O Ratio
		// Changed wfps effect on the N2/N2O ratio based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		fRwfps = max (0.1f, 0.015f * WFPS * 100.0f - 0.32f);

		// N2:N2O Ratio
		// ratioN2N2O = (fRno3 < fRco2) ? fRno3 : fRco2;
		// ratioN2N2O *= fRwfps;
		ratioN2N2O = max(0.1, fRno3_co2 * fRwfps);

		// Compute N2 and N2O flux by layer (fluxNTotal(layer))
		// convert ppm N to gN/m^2
		fluxNTotal[i] = fluxTotalDenitPPM * grams_soil[i] * grams_per_ug;
			if(fluxNTotal[i] > 10.0)Message(1,"Warning - Denit.: fluxNTotal > 10.0");
		fluxN2O = fluxNTotal[i] / (ratioN2N2O + 1.0);
		fluxN2Odenit[i] = fluxN2O;
		fluxN2denit[i]	= max(0.0, fluxNTotal[i] - fluxN2O);
			if(fluxN2Odenit[i] < 0.0) Message(1,"Warning - Denit.: fluxN2Odenit < 0.0");
			if(fluxN2denit[i] < 0.0)  Message(1,"Warning - Denit.: fluxN2denit < 0.0");
    } // denitrification loop

	//********************************************************************* 
    //  Reduce nitrate in soil by the amount of N2-N N2O-N that is lost
    //  Do not let nitrate in any layer go below minNitratePPM_final
	//********************************************************************* 
	for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2;pCL=pCL->pNext, i++)
    {
	totalFlux = fluxN2Odenit[i] + fluxN2denit[i];
	if (totalFlux > 1.0E-30)	// have any flux?
	{
		n2oFrac = fluxN2Odenit[i] / totalFlux; // N2O fraction of flux
		n2Frac  = fluxN2denit[i]  / totalFlux; // N2 fraction of flux

	    minNitrate_final = minNitratePPM_final * grams_soil[i] * grams_per_ug;
	    if ( nitratePPM[i] < minNitratePPM )	// No flux from layer?
	    {
			excess += fluxNTotal[i];
	    }
	    else if (pCL->fNO3N*kgphaTOgpm2 - fluxNTotal[i] > minNitrate_final )
	    {
		// remove N in calculated trace gas flux from the layer
			pCL->fNO3N -= (float)fluxNTotal[i] * gpm2TOkgpha;
	    }
	    else
	    {
			// reduce trace gas flux in layer so soil N won't fall below min
			// actual total trace gas flux (gN/m^2)
			fluxOut =
				(nitratePPM[i] - minNitratePPM_final) *
				grams_soil[i] * grams_per_ug;
			excess += fluxNTotal[i] - fluxOut;
			pCL->fNO3N = (float)minNitrate_final * gpm2TOkgpha;
	    }

		fluxN2Odenit[i] -= n2oFrac * excess;
		fluxN2denit[i]  -= n2Frac * excess;
		fluxN2Odenit[i]  = max(0.0,fluxN2Odenit[i]);
		fluxN2denit[i]   = max(0.0,fluxN2denit[i]);
	}
    else //no Flux: totalFlux < 1.0E-30
    {
		fluxN2Odenit[i] = 0.0;
		fluxN2denit[i] = 0.0;
    }

    //********************************************************************* 
    // --- Partition denitratrification fluxes ---
    //********************************************************************* 
	// For denitrification, kPrecipNO is >= 1.0 -mdh 6/22/00
	// potentialFluxDenitrifNO =
	//   maximum possible denitrification NO flux
	//   based on NO/N2O and kPrecipNO (gN/m^2/day)

	//*********************************************************************
	// ratio NO:N2O and NO flux
	//*********************************************************************
	WFPS = pWL->fContAct / pSL->fPorosity;
	NO_N2O_ratio = 15.23 + ( 35.45 *
		atan ( 0.676f * 3.141592f * 
		(10.0f * Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,(float)WFPS) - 1.86f) )
		) / 3.141592f;
		if(NO_N2O_ratio < 0.0)Message(1,"Warning - Denitrification: NO_N2O_ratio < 0"); 
	if (iForest == 0)// true if agricultural system or grassland  				 
	{	
		NO_N2O_ratio *= 0.20;
	}
    potentialFluxDenitrifNO = fluxN2Odenit[i] * NO_N2O_ratio *  min (1.0, kPrecipNO);
	if(potentialFluxDenitrifNO < 0.0) Message(1,"Warning - Denit.: potentialFluxDenitrifNO < 0");

	if (potentialFluxDenitrifNO <= pCL->fNH4N * kgphaTOgpm2)//  Take all N out of NH4
	{
		fluxNOdenit[i] = potentialFluxDenitrifNO;
		pCL->fNH4N -= (float)potentialFluxDenitrifNO * gpm2TOkgpha;
	}
    else// NH4 limits; some N from N2O
    {
		// Convert all ammonium to NO
		fluxNOdenit[i] = pCL->fNH4N * kgphaTOgpm2;
		potentialFluxDenitrifNO -= pCL->fNH4N * kgphaTOgpm2;
		pCL->fNH4N = 0.0;
		// convert some N2O to NO
		if (potentialFluxDenitrifNO <= fluxN2Odenit[i])
		{
			fluxNOdenit[i]  += potentialFluxDenitrifNO;
			fluxN2Odenit[i] -= potentialFluxDenitrifNO;
		}
    }// NH4 limits; some N from N2O

	 // Check for very small values
    if(fluxNOdenit[i] < 0.0)   Message(1,"Warning - Denit.: fluxNOdenit < 0");;
    if (fluxNOdenit[i] < tolerance)
	fluxNOdenit[i] = 0.0;
    if(fluxN2Odenit[i] < 0.0f) Message(1,"Warning - Denit.: fluxN2Odenit < 0");;
    if (fluxN2Odenit[i] < tolerance)
	fluxN2Odenit[i] = 0.0f;
    if(fluxN2denit[i] < 0.0f)  Message(1,"Warning - Denit.: fluxN2denit < 0");;
    if (fluxN2denit[i] < tolerance)
	fluxN2denit[i] = 0.0f;

    //*********************************************************************
    //  Write fluxes to XN variables
    //  fluxN2Odenit[i]	-	[g/m2/day]
    //  fluxNOdenit[i]	-	[g/m2/day]
    //  fluxN2denit[i]	-	[g/m2/day]
    //*********************************************************************
    pCL->fNO3DenitR = (float)(fluxN2Odenit[i] + fluxNOdenit[i] + fluxN2denit[i]) 
							* gpm2TOkgpha; //[kg/ha/day]
	
	pCL->fN2ON += (float)fluxN2Odenit[i]* gpm2TOkgpha;//[kg/ha]
	pCL->fNON  += (float)fluxNOdenit[i] * gpm2TOkgpha;//[kg/ha]
	pCL->fN2N  += (float)fluxN2denit[i] * gpm2TOkgpha;//[kg/ha]

    }// for layer
}//new Day
return 1;
}
//************ end of function DenitDAYCENT****************
