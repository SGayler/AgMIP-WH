/*******************************************************************************
 *
 *   Module      :   MINER.C
 *
 *   Description:    Sammlung der Modellansaetze zur Berechnung der Mineralisation 
 *                   von organischer Substanz (Pflanzenrueckstaende, Faeces, Humus) 
 *                   im Boden 
 *
 ********************************************************************************
 * 
 *   ch/gsf 12.03.97:  Zusammenfuehren der Funktionen 
 *------------------------------------------------------------------------------
 *   cb/zs  29.12.01:  Typecasts zur Vermeidung von Warnings
 *                     Umformatierungen f�r leichteres Debuggen
 *------------------------------------------------------------------------------
 *   ep/gsf 08.02.02:  Zusammenfuehren von cslib- und modlib-Funktionen
 *
 ********************************************************************************/

#include <crtdbg.h>
#include "xinclexp.h"     
#include "xn_def.h"     
#include "xh2o_def.h"
#include "xlanguag.h"
#include "century.h"

/*********************************/
/*  external procedures          */
/*********************************/

/* aus util_fct.c */
extern float abspower(float, float);
extern int   NewDay(PTIME);
extern int   SimStart(PTIME);
extern float WINAPI Polygon4(float, float, float,float,float ,float ,float ,float ,float);
extern float WINAPI RelAnteil(float, float);
extern int   WINAPI Test_Range(float x, float x1, float x2, LPSTR c);
/* aus output.cpp */
extern int 	 WINAPI   DebTime(float, int);
extern int   WINAPI Message(long, LPSTR);
/* aus manage.c */
extern short int DNDCTillage(EXP_POINTER);
extern short int NCSOILTillage(EXP_POINTER);

/*********************************/
/*  external variables          */
/*********************************/
extern int   iTempCorr;
extern float fParCAOM1Q10, fParCAOM2Q10, fParCBOM1Q10, fParCBOM2Q10, fParCSOM1Q10, fParCSOM2Q10;
extern float fParDBOM1Q10, fParDBOM2Q10, fParMBOM1Q10, fParMBOM2Q10;

extern float fParBOM1, fParSOM1, fParSOM2, fParEff;

extern float fEff_AOM1, fEff_AOM2, fEff_BOM1, fEff_BOM2, fEff_SOM1, fEff_SOM2;   /* org.:  0.6 */




/*********************************/
/*  Exportprozeduren             */
/*********************************/              
int WINAPI MinerLEACHN(EXP_POINTER);
//int WINAPI MinerCERES(EXP_POINTER);
int WINAPI MinerNCSOIL(EXP_POINTER);
int WINAPI MinerDAISY(EXP_POINTER);
int WINAPI MinerDNDC(EXP_POINTER);
int WINAPI MinerNITS(EXP_POINTER);
int WINAPI MinerOMI(EXP_POINTER);
int WINAPI MinerXNCERES(EXP_POINTER);
int WINAPI MinerDAYCENT(EXP_POINTER);

int WINAPI HydroLEACHN(EXP_POINTER);
int WINAPI HydroNSIM(EXP_POINTER);

int SoilNitrogen_GECROS(EXP_POINTER);

signed short int WINAPI MineralisationCERES(EXP_POINTER);
signed short int WINAPI MinerOrgDuengHoff(EXP_POINTER);
signed short int WINAPI NitrOrgNH4Hoff(EXP_POINTER);
signed short int WINAPI ImmobilisationCERES(EXP_POINTER);
signed short int ReduktionsFunktionen(EXP_POINTER);
signed short int NAusErnteRueck(EXP_POINTER);
signed short int NAusHumus(EXP_POINTER);

/**********************************/
/*  Interne Funktionen            */
/**********************************/
signed short int SurfaceMiner(EXP_POINTER);
signed short int WINAPI StandingPoolDecrease(EXP_POINTER);              
signed short int WINAPI BiomassCoverRelation(EXP_POINTER);
float WINAPI CN_Red(float);

int   iSoilCover;

/********************************************************************************************/
/*                                                                                          */
/* description :   Mineralisierung bzw. Immobilisierung                                     */
/*                 Methode  LeachM V3.0 (Hutson & Wagenet 1992)                             */
/*                 und Methode DNDC (Li, Frolking & Frolking, 1992)                         */
/*------------------------------------------------------------------------------------------*/
/*                                                                                          */
/*                 ep/gsf  18.5.94                                                          */
/*                 ch/gsf  13.4.95                                                          */
/*                         13.9.95  Einf�hrung der Plot- Funktionen                         */
/*                         18.9.95 Aufspaltung in miner.c, nitrif.c                         */
/*                         11.3.96 Frost-Einfluss                                           */
/*                 dm/gsf  01.8.96 Implementieren der neuen Variablenstruktur               */
/********************************************************************************************/
/********************************************************************************************/
/* Procedur    :   HydroLEACHN()                                                            */
/* Beschreibung:   Harnstoff Hydrolyse                                                      */
/*                 Methode LEACHM 3.0 (Hutson & Wagenet, 1992)                              */
/*------------------------------------------------------------------------------------------*/
/*                                                                                          */
/*                 cs/age  16.02.93                            version    0.1               */
/*                 ep/gsf  28.02.94                                                         */
/*                                                                                          */
/********************************************************************************************/
/* ver�nd. Var.    pCL->fUreaN                                                              */
/*                 pCL->fNH4N                                                               */
/*                                                                                          */
/********************************************************************************************/
int WINAPI HydroLEACHN(EXP_POINTER)
{

  float Q10,rm,Btem,Scorr,Tcorr,Wcorr,wmin,wlow,whigh;
  float f1;

  PSLAYER    pSL;
  PSWATER    pSW;
  PWLAYER    pWL;
  PHLAYER    pHL;
  PCLAYER    pCL;

  /* Initialisieren der Rektionsraten ?=> expinit.expfile, Eingabedaten */

  Q10   = (float)3.0;
  rm    = (float)1.0;
  Btem  = (float)20.0;
  Scorr = (float)0.6;

  /* Berechnung erfolgt von Schicht 2 bis vorletzte  */

  for (pSW = pSo->pSWater->pNext, pWL = pWa->pWLayer->pNext,
       pHL = pHe->pHLayer->pNext, pCL = pCh->pCLayer->pNext,
       pSL = pSo->pSLayer->pNext;
      ((pSW->pNext != NULL) && (pWL->pNext != NULL) &&
       (pHL->pNext != NULL) && (pCL->pNext != NULL) &&
       (pSL->pNext != NULL));
       pSW = pSW->pNext, pWL = pWL->pNext,
       pHL = pHL->pNext, pCL = pCL ->pNext,
       pSL = pSL->pNext)
       {
       
       if (pCL->fUreaN>EPSILON)
       {
        Tcorr = abspower(Q10,((pHL->fSoilTemp - Btem)/(float)10.0));

        f1    = (float)-153300.0;
        wmin  = WATER_CONTENT(f1);
        f1    = (float)-30660.0;
        wlow  = WATER_CONTENT(f1);
        whigh = max(wmin,(pSL->fPorosity - (float)0.08));
//      wlow  = min(whigh,(wmin+(float)0.1));

        if (pWL->fContAct > whigh)
           {
            f1    = (pSL->fPorosity - pWL->fContAct) / (pSL->fPorosity - whigh);
            Wcorr = Scorr +((float)1.0 - Scorr) * abspower(f1,rm);
           }
        else 
           {
            if (pWL->fContAct >= wlow)
               {
                Wcorr=(float)1.0;
               }
           }

        if (pWL->fContAct < wlow) 
           {
            f1 =(max(pWL->fContAct,wmin) - wmin)/(wlow - wmin);
            Wcorr = abspower(f1,rm);
           }
           
        /* Hydrolyse - Rate  */
        pCL->fUreaHydroR = pCL->fUreaHydroMaxR * Tcorr * Wcorr * pCL->fUreaN;

        /* Hydrolyse   */
        pCL->fUreaN     -= pCL->fUreaHydroR  * pTi->pTimeStep->fAct;
        pCL->fNH4N      += pCL->fUreaHydroR  * pTi->pTimeStep->fAct;
  
   }
  }
  return 0;
}
    
/*********************************************************************************************/
/* Procedur    :   MinerLEACHN()                                                             */
/* Beschreibung:   Mineralisierung                                                           */
/*                 Methode nach LEACHM 3.0 (Hutson & Wagenet, 1992)                          */
/*-------------------------------------------------------------------------------------------*/
/*                                                                                           */
/*                 ep/gsf  14.03.94                                                          */
/*                 ch/gsf  19.04.95 (Neue Struktur"lpx->ntrans" f�r Steuervariable Biomasse) */
/*                         13.06.97 MinerLEACHN: Immobilisierung im Litter-Pool              */
/*                 ab/gsf  12.11.00 ausgeduennt                                              */
/*********************************************************************************************/
/* ver�nd. Var.		pCL->fCLitter                  pCL->fNH4N                                */
/*					pCL->fCManure                  pCL->fNO3N                                */
/*					pCL->fCO2C                     pCL->fNLitter                             */
/*					pCL->fCSolC                    pCL->fNManure                             */
/*					pCL->fHumusMinerR              pSL->fCHumus                              */
/*					pCL->fLitterMinerR             pSL->fNHumus                              */
/*                  pCL->fManureMinerR                                                       */
/*                  pCL->fCO2ProdR                 TEST_RANGE: pCL->fCManure                 */
/*					                                                                         */
/*********************************************************************************************/
int WINAPI MinerLEACHN(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER

  /*
  int   OK = 1;
  int   iLayer = 0;
  char  acdummy[80];
  LPSTR lpOut   = acdummy;
  
  float DeltaT  = pTi->pTimeStep->fAct;
  float SimTime = pTi->pSimTime->fTimeAct;
  */
  
  /*Variablen f�r die C-Fl�sse*/
  float wmin,wlow,whigh,wmax;
  float f1,f3,f4;
  float fHumusMineralR,fLitterToHumusR,fLitterToCO2R,fLitterToMBiomassR;
  float fManureToHumusR,fManureToCO2R,fManureToMBiomassR;
  float fNCHumus,fNCLitter,fNCManure,fLitterToNH4R,fManureToNH4R,fLitterImmR,fManureImmR;

  float fLitterToHumusCC,fLitterIntCC,fLitterToCO2CC;
  float fManureToHumusCC,fManureIntCC,fManureToCO2CC,fHumusToCO2CC, fHumusToLitterCC;
  float fLitterImmC,fLitImmReduc,fNH4ToLitterC;
  float fManureImmC,fManImmReduc,fNH4ToManureC;
  float fNO3ToLitterC,fNO3ToManureC;
  float fLitterToHumusC,fManureToHumusC,fHumusToLitterC;
  
  //float fBiomassToHumusR,fBiomassToCO2R,fBiomassToLitterR  
  //float fCBiomass, fNBiomass
  
  struct ncorr corr={(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1}; 
  
  /*Variablen f�r die Massenbilanz*/
  float fCTotal,fNTotal;
  float fCDiff,fNDiff;
  static float fCDiffSum;
  static float fNDiffSum;
  float fFD = (float)0.02;   /* org.: 0.02 /-* Quotient aus Denitrifikanten und mikrobieller Biomasse  [1]  *-/ */
/*
  PSLAYER      pSL = NULL;
  PCLAYER      pCL = NULL;
  PCPROFILE    pCP = NULL;
  PCPARAM      pPA = NULL;
  PSWATER      pSW = NULL;
  PWLAYER      pWL = NULL;
  PHLAYER      pHL = NULL;
  PNFERTILIZER pNF = NULL;

  _ASSERTE( pSo != NULL );
  _ASSERTE( pCh != NULL );
  _ASSERTE( pWa != NULL );
  _ASSERTE( pHe != NULL );
  _ASSERTE( pMa != NULL );

  pSL = pSo->pSLayer;
  pCL = pCh->pCLayer;
  pCP = pCh->pCProfile;
  pPA = pCh->pCParam;
  pSW = pSo->pSWater;
  pWL = pWa->pWLayer;
  pHL = pHe->pHLayer;
  pNF = pMa->pNFertilizer;

  _ASSERTE( pSL != NULL );
  _ASSERTE( pCL != NULL );
  _ASSERTE( pCP != NULL );
  _ASSERTE( pPA != NULL );
  _ASSERTE( pSW != NULL );
  _ASSERTE( pWL != NULL );
  _ASSERTE( pHL != NULL );
  //_ASSERTE( pNF != NULL );
*/
  
  N_ZERO_LAYER 

  SurfaceMiner(exp_p);

/* 
  pSL = pSL->pNext;
  pCL = pCL->pNext;
  pSW = pSW->pNext;
  pWL = pWL->pNext;
  pPA = pPA->pNext;
  pHL = pHL->pNext;
*/

  /* Schichtweise Berechnung */
  for(N_SOIL_LAYERS) //( iLayer = 1; iLayer < pSo->iLayers - 1; iLayer++ )
  {
/*
    _ASSERTE( pSL != NULL );
    _ASSERTE( pCL != NULL );
    _ASSERTE( pSW != NULL );
    _ASSERTE( pWL != NULL );
    _ASSERTE( pPA != NULL );
    _ASSERTE( pHL != NULL );
*/
    #ifdef DEB_TIME
      if (SimTime > START_TIME_DEB)
        DebTime(SimTime, iSchicht + 1000);
    #endif      
     
    /* Plausibilit�tstest: */
    #ifdef  TEST_RANGE 
        AT_LEAST_EPSILON(pCL->fNLitter,"fNLitter\n in Miner.c");
        AT_LEAST_EPSILON(pCL->fNManure,"fNManure\n in Miner.c");
        AT_LEAST_EPSILON(pSL->fCHumus,  "fCHumus\n in Miner.c");
        AT_LEAST_EPSILON(pSL->fNHumus, "fNHumus\n in Miner.c");
    #endif
    
	/* 1. Temperatur-Reduktionsfunktion*/
    /* Q10-Funktion */
	corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
    
    /* 2. Feuchte-Reduktionsfunktion*/
	/* Ansatz LeachN, ver�ndert, Herkunft der Grenzwerte unbekannt 
    f1= (float)-153300.0;      //pF = 4.18 = PWP
    wmin = WATER_CONTENT(f1);
    f1= (float)-30660.0;       //pF = 3.49
    wlow = WATER_CONTENT(f1);
    whigh = max(wmin,(pSL->fPorosity - pPA->fMinerThetaMin));
    
    // Original-Grenzwerte, beruhen auf Werten des volumetrischen Wassergehalts 
    wmin  = pSW->fContPWP; //= PWP-Wert
    wlow  = wmin + (float)0.10;
	wmax  = pSL->fPorosity;
    whigh = wmax - (float)0.08;
    
    corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							pSL->fPorosity, pPA->fMinerSatActiv);
    */
    
	/* Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (float)-31622770;    //pF = 6.5
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160;        //pF = 2.5
    wlow = WATER_CONTENT(f1);
    f1= (float)-316;         //pF = 1.5
    whigh = WATER_CONTENT(f1);
    f1= (float)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							wmax, pPA->fMinerSatActiv);
    

    /* Kohlenstoff-Mineralisierungs-Konstanten (1/t) */
    fHumusMineralR    = pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
    fLitterToHumusR   = pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * pPA->fMinerHumFac;
    fLitterToCO2R     = pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * ((float)1.0 - pPA->fMinerEffFac);
    fLitterToMBiomassR= pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * ((float)1.0 - pPA->fMinerHumFac);
    fManureToHumusR   = pCL->fManureMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * pPA->fMinerHumFac;
    fManureToCO2R     = pCL->fManureMinerMaxR * corr.Temp * corr.Feucht * ((float)1.0 - pPA->fMinerEffFac);
    fManureToMBiomassR= pCL->fManureMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * ((float)1.0 - pPA->fMinerHumFac);
  //fBiomassToHumusR  = pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * ((float)1.0 - pPA->fMinerEffFac);
  //fBiomassToCO2R    = pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * ((float)1.0-pPA->fMinerHumFac);
  //fBiomassToBiomassR= pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht * pPA->fMinerEffFac * pPA->fMinerHumFac;

    /* ch 21.8.95 Csol nach Li et al. (DNDC) ist ein Indikator f�r Mineralisierungsaktivit�t (kein Pool)
	ab 23.11.00 dort aber als Indikator f�r die Mineralisierung des soil organic carbon verstanden
	und eigentlich als DOC f�r die Denitrifikation angesetzt!*/
    pCL->fCsolC  = pCL->fCLitter * fLitterToMBiomassR;
    pCL->fCsolC += pCL->fCManure * fManureToMBiomassR;
//  pCL->fCsolC += pWL->fWaterFreezeR * lpXT->fIcefRedCsol;
//  pCL->fCsolC += pSL->fCHumus * fHumusMineralR * lpXT->fHumusToCsol;
            

    /* CN-Werte f�r Litter und Manure   */
    fNCLitter     = (pCL->fCLitter > EPSILON)?
    	pCL->fNLitter/pCL->fCLitter
    	:(float)0.1; 
            
    fNCManure     = (pCL->fNManure > EPSILON)?
    	pCL->fNManure / pCL->fCManure
    	:(float)0.1; 

    fNCHumus      = (pSL->fNHumus > EPSILON)?
    	pSL->fNHumus / pSL->fCHumus
    	:(float)-99; 
    
	/*Ist das n�tig oben -99 zu setzen und diese Abfrage durchzuf�hren?*/
    Test_Range(fNCHumus,(float)0.001,(float)1,"CN-Humus");

	/*Entscheidung Immobilisierung/Mineralisierung und 
	  Berechnung der jeweiligen Konstanten */
    f3     = fNCLitter - pPA->fMinerEffFac / pCL->fMicBiomCN;
    fLitterToNH4R = (f3 > 0)? f3 : 0;
    fLitterImmR   = (f3 < 0)? (float)-1 * f3 : 0;

    f4     = fNCManure - pPA->fMinerEffFac / pCL->fMicBiomCN;
    fManureToNH4R = (f4 > 0)? f4 : 0;
    fManureImmR   = (f4 < 0)? (float)-1 * f4 : 0;
            

    /********************************************************************************/
    /*                  Veraenderung des C-Pools pro Zeitschritt                    */
    /********************************************************************************/

    fCTotal = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C;

    /* 1. Veraenderung des C-Litter Pools pro Zeitschritt */

    fLitterToHumusCC  = pCL->fCLitter * fLitterToHumusR;
    fLitterToCO2CC    = pCL->fCLitter * fLitterToCO2R;
    fLitterIntCC      = pCL->fCLitter * fLitterToMBiomassR;
//  fHumusToLitterCC  = pCL->fCLitter * fHumusShiftR;
    fHumusToLitterCC  = (float)0;
    
    pCL->fCLitter -=(fLitterToHumusCC + fLitterToCO2CC - fHumusToLitterCC)* DeltaT;
         
    /* 2. Veraenderung des C-Manure Pools pro Zeitschritt */

    fManureToHumusCC  = pCL->fCManure * fManureToHumusR;
    fManureIntCC      = pCL->fCManure * fManureToMBiomassR;
    fManureToCO2CC    = pCL->fCManure * fManureToCO2R;

    pCL->fCManure  -= (fManureToHumusCC + fManureToCO2CC)* DeltaT;

    /* 3. Veraenderung des C-Humus Pools */

    fHumusToCO2CC    = pSL->fCHumus * fHumusMineralR;

    pSL->fCHumus  += (fLitterToHumusCC +fManureToHumusCC -fHumusToCO2CC - fHumusToLitterCC)* DeltaT;

    /* 4. BioMasse Zuwachs */               
    //pCL->fCMicBiomDenit += lpXT->MinerBio * fFD * (fLitterIntCC + fManureIntCC) * DeltaT;

    /* 5. CO2-C Zuwachs */
    pCL->fCO2ProdR = fLitterToCO2CC +  fManureToCO2CC + fHumusToCO2CC;
    
    pCL->fCO2C += pCL->fCO2ProdR * DeltaT;


    /********************************************************************************/
    /*      Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt          */
    /********************************************************************************/

    fNTotal = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N;

    /* 1. Immobilisierung im Litter-Pool */
    fLitterImmC = fLitterImmR * (fLitterIntCC + fLitterToHumusCC + fLitterToCO2CC);

    /*ch, 17.7.97 max. 8% des NMin k�nnen am Tag immobilisiert werden: */
    if ((fLitterImmC < EPSILON)||((float)0.08*(pCL->fNO3N + pCL->fNH4N)>= fLitterImmC))
        fLitImmReduc =  (float)1.0;
    else
       {
        fLitImmReduc =  (float)0.015*(pCL->fNO3N + pCL->fNH4N)/ fLitterImmC;
       } 

    if (pCL->fNO3N + pCL->fNH4N > EPSILON)
       {              
        fNH4ToLitterC = fLitterImmC * fLitImmReduc * RelAnteil(pCL->fNH4N,pCL->fNO3N);
        fNO3ToLitterC = fLitterImmC * fLitImmReduc * RelAnteil(pCL->fNO3N,pCL->fNH4N);
       }
        
    else
        fLitImmReduc = fNH4ToLitterC  = fNO3ToLitterC  = (float)0;

	pCL->fNLitterImmobR = fNH4ToLitterC + fNO3ToLitterC; /*=fLitterImmC*/
	
    /* 2. Immobilisierung im Manure-Pool */
    fManureImmC = fManureImmR * (fManureIntCC + fManureToHumusCC + fManureToCO2CC);
        
         /*ch, 13.5.97, 13.1.97 max. 8% des NMin k�nnen am Tag immobilisiert werden: */
            if ((fManureImmC < EPSILON)||((float)0.08*(pCL->fNO3N + pCL->fNH4N)>= fManureImmC))
            fManImmReduc =  (float)1.0;
            else
            {
            fManImmReduc =  (float)0.08*(pCL->fNO3N + pCL->fNH4N)/ fManureImmC;
            } 


    if (pCL->fNO3N + pCL->fNH4N > EPSILON)
       {              
         fNH4ToManureC = fManureImmC * fManImmReduc * RelAnteil(pCL->fNH4N,pCL->fNO3N);
         fNO3ToManureC = fManureImmC * fManImmReduc * RelAnteil(pCL->fNO3N,pCL->fNH4N);
        }
    else
         fNH4ToManureC = fNO3ToManureC = fManImmReduc = (float)0;

	pCL->fNManureImmobR = fNH4ToManureC + fNO3ToManureC; 
    
    pCL->fNImmobR = pCL->fNManureImmobR + pCL->fNLitterImmobR;
    
    /* 3. Ammonium-Mineralisierung aus Humus, Litter und Manure */
    pCL->fHumusMinerR  = pSL->fNHumus  * fHumusMineralR;
    pCL->fLitterMinerR = fLitterToNH4R * (fLitterToHumusCC + fLitterToCO2CC + fLitterIntCC);
    pCL->fManureMinerR = fManureToNH4R * (fManureToHumusCC + fManureToCO2CC + fManureIntCC);
    pCL->fMinerR = pCL->fHumusMinerR + pCL->fLitterMinerR + pCL->fManureMinerR; 

    /* N-Bilanz */
    pCL->fNH4N += (pCL->fHumusMinerR  + pCL->fLitterMinerR  + pCL->fManureMinerR
                -  fNH4ToLitterC - fNH4ToManureC) * DeltaT;
    pCL->fNO3N -= (fNO3ToLitterC + fNO3ToManureC) * DeltaT;

    /* 4. Litter-N Abbau */
    fHumusToLitterC = fHumusToLitterCC * fNCHumus; //sinnlos weil immer 0
            
    fLitterToHumusC = min((float)0.5 * pCL->fNLitter,
                     (fLitterToHumusCC * fLitImmReduc / pCL->fMicBiomCN));

    pCL->fNLitter  += (pCL->fNLitterImmobR  - pCL->fLitterMinerR
                    + fHumusToLitterC - fLitterToHumusC) * DeltaT;

    /* 5. Manure-N Abbau */
    fManureToHumusC = min((float)0.5 * pCL->fNManure,
                           fManureToHumusCC * fManImmReduc / pCL->fMicBiomCN);

    pCL->fNManure += (pCL->fNManureImmobR - fManureToHumusC - pCL->fManureMinerR) * DeltaT;

    /* 6. Humus-N Abbau */
    pSL->fNHumus += (fLitterToHumusC + fManureToHumusC - fHumusToLitterC
    				 - pCL->fHumusMinerR)* DeltaT;

    fCDiff = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C-fCTotal;
    fNDiff = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N-fNTotal;
    fCDiffSum    += fCDiff;
    fNDiffSum    += fNDiff;

    /*
    // Naechste Schicht
    pSL = pSL->pNext;
    pCL = pCL->pNext;
    pSW = pSW->pNext;
    pWL = pWL->pNext;
    pPA = pPA->pNext;
    pHL = pHL->pNext;
    */
  }    /* Schichtweise */

  return 0;
}                    
 

/********************************************************************************************/
/* Procedur    :   MinerDNDC()                                                              */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Mineralisierung                                                          */
/*                 Methode DNDC (Li, Frolking & Frolking, 1992)                             */
/*                              (source-code  Vers. 5.2  20.03.96)                          */
/*------------------------------------------------------------------------------------------*/
/*                                                                                          */
/*                 ep/gsf  15.1.97                                                          */
/*                 ab/gsf  12.01.01 korrigiert                                              */
/*                 ab/gsf  11.06.01 �bergang zu den Pools Litter, Manure und Humus          */
/*                 ab/gsf  14.12.01 letzte �berarbeitung                                    */
/*                                                                                          */
/********************************************************************************************/
/* ver�nd. Var.		pCL->fCFOMFast              pCL->fNFOMFast                              */
/*					pCL->fCFOMSlow              pCL->fNFOMSlow                              */
/*					pCL->fCFOMVeryFast          pCL->fNFOMVeryFast                          */
/*					pCL->fCMicBiomFast          pCL->fNMicBiomFast                          */
/*					pCL->fCMicBiomSlow          pCL->fNMicBiomSlow                          */
/*					pCL->fCHumusFast            pCL->fNHumusFast                            */
/*					pCL->fCHumusSlow            pCL->fNHumusSlow                            */
/*			    	pCL->fCHumusStable          pCL->fNHumusStable                          */
/*					pCL->fCLitter               pCL->fNLitter                               */
/*					pCL->fCManure               pCL->fNManure                               */
/*					pSL->fCHumus                pSL->fNHumus                                */
/*                  pCL->fCSoilMicBiom          pCL->fNSoilMicBiom                          */ 
/*					pCL->fCO2C                  pCL->fNO3N                                  */
/*					pCL->fProdCO2R              pCL->fNH4N                                  */
/*                  pCL->fLitterMinerR          pCL->fNImmobR                               */
/*                  pCL->fHumusMinerR           pCL->fMinerR                                */
/*					pCL->fFOMSlowDecMaxR        pCL->fCsolC                                 */
/*   				pCL->fFOMFastDecMaxR        pCL->fNLitterImmobR                         */
/*   				pCL->fFOMVeryFastDecMaxR    corr.Lehm                                   */
/*   				pCL->fHumusSlowMaxDecMaxR   corr.Feucht                                 */
/*   				pCL->fHumusFastMaxDecMaxR   corr.Rate                                   */
/*   				pCL->fMicBiomSlowDecMaxR    corr.Temp                                   */
/*   				pCL->fMicBiomFastDecMaxR                                                */
/********************************************************************************************/
int WINAPI MinerDNDC(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr  corr = {(float)1}; 
  
  /******************************************************************/
  /*           Deklaration lokaler Variablen                        */
  /******************************************************************/
  
  /* 1. Variablen f�r die Reduktionsfunktionen*/
  float f1,wmin,wlow,whigh,wmax;

  /* 2. Variablen f�r die C-Fl�sse */
  float fCAOM1DecayR,fCAOM2DecayR,fCAOM3DecayR,fCBOM1DecayR,fCBOM2DecayR,fCSOM1DecayR,fCSOM2DecayR;
  float fCAOMToBOMR,fCAOMToBOM1R,fCAOMToBOM2R,fCAOMToCO2R;
  float fCBOMToBOMR,fCBOMToBOM1R,fCBOMToBOM2R,fCBOMToSOM1R,fCBOMToCO2R;
  float fCSOMToBOMR,fCSOMToBOM1R,fCSOMToBOM2R,fCSOMToCO2R,fCSOMToSOM3R;

  /* 3. Variablen f�r die C/N Verh�ltnisse */
  const float fCN_AOM1   = (float) 200.0;   /* fr�her:  20 */
  const float fCN_AOM2   = (float)  50.0;   /* fr�her:  20 */
  const float fCN_AOM3   = (float)   5.0;   /* fr�her:2.35 */
  const float fCN_BOM1   = (float)   8.0;   /* orig.:   8 */
  const float fCN_BOM2   = (float)   8.0;   /* orig.:   8 */
  const float fCN_SOM    = (float)   8.0;   /* orig.:   8 */

  /* 4. Variablen f�r die Aufteilung der Stofffl�sse  */
  const float fEffAOMToBOM = (float) 0.6; /* orig.: 0.6*/
  const float fEffAOMToCO2 = (float) 0.4; /* orig.: 0.4*/
  
  const float fEffBOMToBOM = (float) 0.6; /* orig.: 0.6*/
  const float fEffBOMToSOM = (float) 0.2; /* orig.: 0.2*/
  const float fEffBOMToCO2 = (float) 0.2; /* orig.: 0.2*/

  const float fEffSOMToBOM = (float) 0.2; /* orig.: 0.2*/
  const float fEffSOMToSOM = (float) 0.4; /* orig.: 0.4*/
  const float fEffSOMToCO2 = (float) 0.4; /* orig.: 0.4*/

  const float fDisBOM      = (float) 0.9;  /* orig.: 0.9  */
  const float fDisSOM      = (float) 0.16; /* orig.: 0.16  */

  /* 5. Variablen f�r die Abbau-Begrenzung bei mangelndem Nmin */
  float fNminAOM1,fNminAOM2,fNminAOM3;

  /* 6. Variablen f�r die Berechnung der N-Mineralisation/N-Immobilisierung*/
  float h1,h2,h3,h4,h5,h6,h7,h8,h9,h10,h11;
  float fNToNH4R,fNImmR;

  /* 7. Variablen f�r die Massenbilanz*/
  float fCTotal0,fNTotal0;      //f�r die DNDC-Pools
  float fCTotal1,fNTotal1;
  float fCDiff,fNDiff;                       
  float fCTotal0_B,fNTotal0_B;  //f�r die ExpertN-Pools (Litter,Manure,Humus)
  float fCTotal1_B,fNTotal1_B;
  float fCDiff_B,fNDiff_B;                    

  /* 8. Variablen f�r die Anteilsberechnung */
  float fCFastOld, fCSlowOld;
  float AnteilCFOMSlow,AnteilCFOMFast,AnteilCFOMVeryFast;
  float AnteilCMicBiomSlow,AnteilCMicBiomFast;
  float AnteilCHumusFast,AnteilCHumusSlow,AnteilCHumusStable;
  float fCSlow,fCFast,fCMic;
  float fNSlow,fNFast,fNMic;
  float CAnteilLitter,CAnteilManure,NAnteilLitter,NAnteilManure;

  /* Keine Ahnung bzw. Output-Funktion*/
  static float fCDiffSum = (float)0.0;
  static float fNDiffSum = (float)0.0;
  /* Variablendeklaration Ende */

  
  N_ZERO_LAYER            //Setzen der Pointer auf die nullte Schicht 
  
  SurfaceMiner(exp_p);  //Aufruf der Oberfl�chenmineralisierung
 
  DNDCTillage(exp_p);     //Aufruf der Tillagemassnahmen

  
  for (N_SOIL_LAYERS)     //schichtweise Berechnung
    {
	/***********************************************************************/
	/* Berechnung der Mengen in den spezifischen DNDC-Pools aus den Pools  */
	/* Litter, Manure und Humus anhand der relativen Verh�ltnisse zum Ende */
	/* des vorherigen Zeitsschritts.                                       */
	/***********************************************************************/
	
if (!SimStart(pTi))
	
 {
/* 1. Fast cycling Pools */
	fCFastOld = pCL->fCFOMSlow + pCL->fCFOMFast +  pCL->fCFOMVeryFast
	            + pCL->fCMicBiomSlow + pCL->fCMicBiomFast;
  
    if(fCFastOld)
	 {
	  AnteilCFOMSlow     = pCL->fCFOMSlow     / fCFastOld;
      AnteilCFOMFast     = pCL->fCFOMFast     / fCFastOld;
      AnteilCFOMVeryFast = pCL->fCFOMVeryFast / fCFastOld;
	  AnteilCMicBiomSlow = pCL->fCMicBiomSlow / fCFastOld; 
      AnteilCMicBiomFast = pCL->fCMicBiomFast / fCFastOld;
     
	  pCL->fCFOMSlow     = AnteilCFOMSlow     * (pCL->fCLitter + pCL->fCManure);
      pCL->fCFOMFast     = AnteilCFOMFast     * (pCL->fCLitter + pCL->fCManure);
      pCL->fCFOMVeryFast = AnteilCFOMVeryFast * (pCL->fCLitter + pCL->fCManure);
	  pCL->fCMicBiomSlow = AnteilCMicBiomSlow * (pCL->fCLitter + pCL->fCManure);
      pCL->fCMicBiomFast = AnteilCMicBiomFast * (pCL->fCLitter + pCL->fCManure);
  	 }

    else
     {
      pCL->fCFOMSlow     = (float)0.0;
      pCL->fCFOMFast     = (float)0.0;
      pCL->fCFOMVeryFast = (float)0.0;
	  pCL->fCMicBiomSlow = (float)0.0;
      pCL->fCMicBiomFast = (float)0.0;
	 }
	
	/* 2. Slow Cycling Pools */
	fCSlowOld = pCL->fCHumusSlow + pCL->fCHumusFast + pCL->fCHumusStable;

	if(fCSlowOld)
	 {
	  AnteilCHumusSlow   = pCL->fCHumusSlow   / fCSlowOld;
	  AnteilCHumusFast   = pCL->fCHumusFast   / fCSlowOld;
      AnteilCHumusStable = pCL->fCHumusStable / fCSlowOld;
	  
	  pCL->fCHumusSlow   = AnteilCHumusSlow   * pSL->fCHumus;
      pCL->fCHumusFast   = AnteilCHumusFast   * pSL->fCHumus;
	  pCL->fCHumusStable = AnteilCHumusStable * pSL->fCHumus;
	 }

	else
	 {
      pCL->fCHumusSlow   = (float)0.0;
      pCL->fCHumusFast   = (float)0.0;
      pCL->fCHumusStable = (float)0.0;
	 } /* Anteilsberechnung Ende */
	
 }

    /******************************************************************/
    /*                          Reduktionsfunktionen                  */
    /******************************************************************/
  
	/* 1. Temperatur-Reduktionsfunktion, Q10 Funktion*/
    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
    
	
	/* 2. Feuchte-Reduktionsfunktion, Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (float)-31622770;    //pF = 6.5
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160;        //pF = 2.5
    wlow = WATER_CONTENT(f1);
    f1= (float)-316;         //pF = 1.5
    whigh = WATER_CONTENT(f1);
    f1= (float)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							wmax, pPA->fMinerSatActiv);

    /* 3. Tongehalt-Reduktionsfunktion, Ansatz DAISY*/
    if (pSL->fClay <= (float)25.0)
	   corr.Lehm = (float)1.0 - (float)0.02 * pSL->fClay;

     else
	   corr.Lehm = (float)1.0 - (float)0.02 * (float)25.0;
	/* Reduktionsfunktion Ende */


   /* Original Reduktionsfunktionen 
	1. Temperatur: Ansatz Polygonzug, Bezugswert: Tagesmittelwert der jeweiligen Simulationsschicht
	corr.Temp = Polygon4(pHL->fSoilTemp,
		        (float)0, (float)0,(float)30,(float)1.8,(float)40,(float)1.8,(float)50,(float)1.4);
				
    2. Feuchte: Ansatz Polygonzug, Bezugswert: Wassergef�llte Porenvolumen 
          Optimalwert WFPS = 60% 
    corr.Feucht = Polygon4(((pWL->fContAct + pWL->fIce) / pSL->fPorosity), 
                  (float)0.1,(float)0,(float)0.6,(float)1,(float)0.8,(float)0.5,(float)1,(float)0.4);
    
    3. Textur: Ansatz DNDC, Bezugswert: Tongehalt, 0.63 - 1.50 mit 1.0 f�r Lehm 
	corr.Lehm = (float)log10(0.14/pSL->fClay * 100.0) /(float)2.3026 + (float)1.0;
   */
	 
    /* 4. Tiefenabh�ngiger und Tongehaltsabh�ngiger Korrekturfaktor */
	corr.Rate = (float)0.015 - ((iLayer - (float)1.0) * (float)0.00135 * pSL->fClay/(float)100);
    
	if (corr.Rate < 0)
	    corr.Rate = (float)0.0;
	/* Reduktionsfunktion Ende */


	/******************************************************************/
    /*    C  Mineralisierungsraten (dC/dt )                           */
    /******************************************************************/

	/* 1. lokal ver�nderbare spez. Abbaukonstanten zur Sensitivit�tsanalyse */
	pCL->fFOMSlowDecMaxR      = (float)0.02  * corr.Rate;
    pCL->fFOMFastDecMaxR      = (float)0.074 * corr.Rate;
    pCL->fFOMVeryFastDecMaxR  = (float)0.25  * corr.Rate;
	pCL->fHumusSlowMaxDecMaxR = (float)0.006 * corr.Rate;
    pCL->fHumusFastMaxDecMaxR = (float)0.16  * corr.Rate;
    pCL->fMicBiomSlowDecMaxR  = (float)0.04  * corr.Rate;
    pCL->fMicBiomFastDecMaxR  = (float)0.33  * corr.Rate;



    /* 2. Kohlenstoff-Mineralisierungs-Raten (dC/dt) */
    fCAOM1DecayR = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR      * corr.Temp * corr.Feucht;
    fCAOM2DecayR = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR      * corr.Temp * corr.Feucht;
    fCAOM3DecayR = pCL->fCFOMVeryFast * pCL->fFOMVeryFastDecMaxR  * corr.Temp * corr.Feucht;
    fCBOM1DecayR = pCL->fCMicBiomSlow * pCL->fMicBiomSlowDecMaxR  * corr.Temp * corr.Feucht;
    fCBOM2DecayR = pCL->fCMicBiomFast * pCL->fMicBiomFastDecMaxR  * corr.Temp * corr.Feucht;
    fCSOM1DecayR = pCL->fCHumusSlow   * pCL->fHumusSlowMaxDecMaxR * corr.Temp * corr.Feucht * corr.Lehm;
    fCSOM2DecayR = pCL->fCHumusFast   * pCL->fHumusFastMaxDecMaxR * corr.Temp * corr.Feucht * corr.Lehm;

    /* 3. Reduktion des AOM-Abbaus bei mangelndem mineralischem N */
	fNminAOM1 = min((float)1.0,(float)0.2+(float)7.2/(fCAOM1DecayR/(fCAOM1DecayR/fCN_AOM1 + (pCL->fNO3N + pCL->fNH4N))));
	fNminAOM2 = min((float)1.0,(float)0.2+(float)7.2/(fCAOM2DecayR/(fCAOM2DecayR/fCN_AOM2 + (pCL->fNO3N + pCL->fNH4N))));
	fNminAOM3 = min((float)1.0,(float)0.2+(float)7.2/(fCAOM3DecayR/(fCAOM3DecayR/fCN_AOM3 + (pCL->fNO3N + pCL->fNH4N))));

	fCAOM1DecayR  = fCAOM1DecayR * fNminAOM1;
    fCAOM2DecayR  = fCAOM2DecayR * fNminAOM2;
    fCAOM3DecayR  = fCAOM3DecayR * fNminAOM3;
    /* Mineralisation Ende */


   /*************************************************************************/
   /*  Berechnung der Ammonium Bilanz bei jedem relvanten Pool�bergang      */
   /*  Summe positiv = Nettomineralisierung                                 */
   /*  Summe negativ = Nettoimmobilisierung                                 */
   /*************************************************************************/	
   
	/*AOM -> BOM1*/
    h1 = ((float)1.0 -  fDisBOM) * fCAOM1DecayR * ((float)1.0 / fCN_AOM1 - fEffAOMToBOM / fCN_BOM1);
    h2 = ((float)1.0 -  fDisBOM) * fCAOM2DecayR * ((float)1.0 / fCN_AOM2 - fEffAOMToBOM / fCN_BOM1);
    h3 = ((float)1.0 -  fDisBOM) * fCAOM3DecayR * ((float)1.0 / fCN_AOM3 - fEffAOMToBOM / fCN_BOM1);

    /*AOM -> BOM2*/
    h4 = fDisBOM * fCAOM1DecayR * ((float)1.0 / fCN_AOM1 - fEffAOMToBOM / fCN_BOM2);
    h5 = fDisBOM * fCAOM2DecayR * ((float)1.0 / fCN_AOM2 - fEffAOMToBOM / fCN_BOM2);
    h6 = fDisBOM * fCAOM3DecayR * ((float)1.0 / fCN_AOM3 - fEffAOMToBOM / fCN_BOM2);

	/*BOM1 -> BOM ohne SOM1*/
    h7 = fCBOM1DecayR * ((float)1.0 / fCN_BOM1 - fEffBOMToBOM * ((float)1.0 -  fDisBOM) / fCN_BOM1 - 
	    	fEffBOMToBOM * fDisBOM / fCN_BOM2 - fEffBOMToSOM / pSL->fHumusCN);
    
	/*BOM2 -> BOM ohne SOM1*/
	h8 = fCBOM2DecayR * ((float)1.0 / fCN_BOM2 - fEffBOMToBOM * ((float)1.0 -  fDisBOM) / fCN_BOM1 - 
	    	fEffBOMToBOM * fDisBOM / fCN_BOM2 - fEffBOMToSOM / pSL->fHumusCN);
    
    /*SOM1 -> BOM ohne SOM3*/
    h9 = fCSOM1DecayR * ((float)1.0 / pSL->fHumusCN - fEffSOMToBOM * ((float)1.0 -  fDisBOM) / fCN_BOM1 - 
	    	fEffSOMToBOM * fDisBOM / fCN_BOM2 - fEffSOMToSOM / pSL->fHumusCN);
	
	/*SOM2 -> BOM ohne SOM3*/
    h10 = fCSOM2DecayR * ((float)1.0 / pSL->fHumusCN - fEffSOMToBOM * ((float)1.0 -  fDisBOM) / fCN_BOM1 - 
	    	fEffSOMToBOM * fDisBOM / fCN_BOM2 - fEffSOMToSOM / pSL->fHumusCN);
  

	h11 = h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8 + h9 + h10;

	fNToNH4R = (h11 > 0) ? h11 : 0;
    fNImmR   = (h11 < 0) ? (float)-1 * h11 : 0;
    /* Entscheidung Immobilisierung/Mineralisierung Ende*/


    /******************************************************************/
    /* Veraenderung des C- und N-Pools pro Zeitschritt                */ 
    /* Da CN-Verh�ltnis in den Pools konstant ist, ist eine separate  */
    /* Modellierung des N redundant.                                  */
    /******************************************************************/

    /* 1. Gesamt C vor dem Zeitschritt */
    fCTotal0 = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCFOMVeryFast + 
               pCL->fCMicBiomSlow + pCL->fCMicBiomFast + pCL->fCHumusSlow + 
			   pCL->fCHumusFast + pCL->fCHumusStable + pCL->fCO2C;

    fCTotal0_B = pCL->fCLitter + pCL->fCManure + pSL->fCHumus + pCL->fCO2C;

   	/* 2. Gesamt N vor dem Zeitschritt */
	pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_AOM1;
    pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_AOM2; 
    pCL->fNFOMVeryFast = pCL->fCFOMVeryFast / fCN_AOM3;
	pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_BOM1;
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_BOM2;
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / pSL->fHumusCN;
    pCL->fNHumusFast   = pCL->fCHumusFast   / pSL->fHumusCN;
    pCL->fNHumusStable = pCL->fCHumusStable / pSL->fHumusCN;
	
	fNTotal0 = pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNFOMVeryFast +
	           pCL->fNMicBiomSlow + pCL->fNMicBiomFast + pCL->fNHumusSlow +
			   pCL->fNHumusFast + pCL->fNHumusStable + pCL->fNH4N + pCL->fNO3N;

	fNTotal0_B = pCL->fNLitter + pCL->fNManure + pSL->fNHumus + pCL->fNH4N + pCL->fNO3N;

	/* 3. Abbau der Residue Pools pro Zeitschritt */
    fCAOMToBOMR  = (fCAOM1DecayR + fCAOM2DecayR + fCAOM3DecayR) * fEffAOMToBOM;
    fCAOMToBOM1R =                       ((float)1.0 - fDisBOM) * fCAOMToBOMR;
    fCAOMToBOM2R =                                     fDisBOM  * fCAOMToBOMR;
    fCAOMToCO2R  = (fCAOM1DecayR + fCAOM2DecayR + fCAOM3DecayR) * fEffAOMToCO2;

	/* 4. Abbau der Biomasse Pools */
	fCBOMToBOMR  = (fCBOM1DecayR + fCBOM2DecayR) * fEffBOMToBOM;
	fCBOMToBOM1R =        ((float)1.0 - fDisBOM) * fCBOMToBOMR;
	fCBOMToBOM2R =                      fDisBOM  * fCBOMToBOMR;
	fCBOMToSOM1R = (fCBOM1DecayR + fCBOM2DecayR) * fEffBOMToSOM; 
    fCBOMToCO2R  = (fCBOM1DecayR + fCBOM2DecayR) * fEffBOMToCO2;

	/* 5. Abbau der Humads Pools*/
	fCSOMToBOMR  = (fCSOM1DecayR + fCSOM2DecayR) * fEffSOMToBOM;
	fCSOMToBOM1R =        ((float)1.0 - fDisBOM) * fCSOMToBOMR;
	fCSOMToBOM2R =                      fDisBOM  * fCSOMToBOMR;
	fCSOMToCO2R  = (fCSOM1DecayR + fCSOM2DecayR) * fEffSOMToCO2;
    fCSOMToSOM3R = (fCSOM1DecayR + fCSOM2DecayR) * fEffSOMToSOM;

    /* 6. L�slicher organischer Kohlenstoff (DOC),ist kein Pool
	      der bilanziert wird, sondern nur ein Indikator f�r die
		  Verf�gbarkeit von org. C und wird im Denitrifikationsmodell
	      nach Li verwendet, besteht aus den Mengen C die in die Biomasse eingebaut werden */
    pCL->fCsolC = fCAOMToBOMR + fCBOMToBOMR + fCSOMToBOMR;

	/* 7. Veraenderung der Residue-Pools pro Zeitschritt */
    pCL->fCFOMSlow     -= fCAOM1DecayR * DeltaT;
    pCL->fCFOMFast     -= fCAOM2DecayR * DeltaT;             
    pCL->fCFOMVeryFast -= fCAOM3DecayR * DeltaT;             

    /* 8. Veraenderung der Biomass-Pools pro Zeitschritt */
    pCL->fCMicBiomSlow += (fCAOMToBOM1R - fCBOM1DecayR + fCSOMToBOM1R + fCBOMToBOM1R) * DeltaT;
    pCL->fCMicBiomFast += (fCAOMToBOM2R - fCBOM2DecayR + fCSOMToBOM2R + fCBOMToBOM2R) * DeltaT;
    
    /* 9. Veraenderung der Humads-Pools pro Zeitschritt */ 
	pCL->fCHumusSlow   += (fCBOMToSOM1R - fCSOM1DecayR) * DeltaT;
    pCL->fCHumusFast   -= fCSOM2DecayR * DeltaT;
    pCL->fCHumusStable += fCSOMToSOM3R * DeltaT;

    /* 10. Veraenderung des CO2-Pools pro Zeitschritt */
    pCL->fCO2ProdR = fCAOMToCO2R + fCBOMToCO2R + fCSOMToCO2R;
    pCL->fCO2C += pCL->fCO2ProdR * DeltaT;

    /* 11. Ammonium-Netto-Mineralisierung bzw. Immobilisierung */
    if (fNToNH4R > 0) //Mineralisierung
      pCL->fNH4N += fNToNH4R * DeltaT;  
      
    else             //Immobilisierung
	 {
	  pCL->fNH4N -= fNImmR * RelAnteil(pCL->fNH4N,pCL->fNO3N) * DeltaT;  
      pCL->fNO3N -= fNImmR * RelAnteil(pCL->fNO3N,pCL->fNH4N) * DeltaT;  	
	 }

	/* 12.Ver�nderung in den N-Pools */
    pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_AOM1;
	pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_AOM2; 
	pCL->fNFOMVeryFast = pCL->fCFOMVeryFast / fCN_AOM3; 
    pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_BOM1;  
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_BOM2;   
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / pSL->fHumusCN;   
    pCL->fNHumusFast   = pCL->fCHumusFast   / pSL->fHumusCN;  
    pCL->fNHumusStable = pCL->fCHumusStable / pSL->fHumusCN;  
    /* Ver�nderung C- und N-Pools Ende */  
   
	
   /* Belegen der globalen Variablen*/
	pCL->fMinerR = fNToNH4R;
    pCL->fHumusMinerR = fCSOMToCO2R/pSL->fHumusCN;
	pCL->fLitterMinerR = fNToNH4R-pCL->fHumusMinerR;
	pCL->fNImmobR = fNImmR;
    pCL->fNLitterImmobR = fNImmR;


    /********************************************************************************/
    /* �bertragen der Ver�nderungen auf die Pools Litter, Manure und Humus          */
    /********************************************************************************/
   
	/* 1. Kohlenstoff */ 
	fCFast = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCFOMVeryFast;
    fCMic  = pCL->fCMicBiomSlow + pCL->fCMicBiomFast; 
    fCSlow = pCL->fCHumusSlow +  pCL->fCHumusFast + pCL->fCHumusStable;
   
    if(pCL->fCLitter + pCL->fCManure)
	 {
	  CAnteilLitter  = pCL->fCLitter / (pCL->fCLitter + pCL->fCManure);
      CAnteilManure  = pCL->fCManure / (pCL->fCLitter + pCL->fCManure);
	 }
	
	else
	 {
	  CAnteilLitter = (float)1.0;
      CAnteilManure = (float)0.0;
	 }
	
	pCL->fCLitter = CAnteilLitter * fCFast + fCMic;
    pCL->fCManure = CAnteilManure * fCFast;
    pSL->fCHumus  = fCSlow;
    pCL->fCSoilMicBiom = fCMic;
    
	/* 2. Stickstoff */
    fNFast = pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNFOMVeryFast;
    fNMic  = pCL->fNMicBiomSlow + pCL->fNMicBiomFast; 
    fNSlow = pCL->fNHumusSlow +  pCL->fNHumusFast + pCL->fNHumusStable;
    
	if(pCL->fNLitter + pCL->fNManure)
	 {
	  NAnteilLitter  = pCL->fNLitter / (pCL->fNLitter + pCL->fNManure);
      NAnteilManure  = pCL->fNManure / (pCL->fNLitter + pCL->fNManure);
	 }

	else
	 {
	  NAnteilLitter = (float)1.0;
      NAnteilManure = (float)0.0;
	 }

    pCL->fNLitter = NAnteilLitter * fNFast + fNMic;
    pCL->fNManure = NAnteilManure * fNFast;
    pSL->fNHumus  = fNSlow;
    pCL->fNSoilMicBiom = fNMic;
	/* �bertragen auf ExpertN-Pools Ende */


    /********************************************************************************/
    /* Massenbilanz, die Summe der Ver�nderungen muss ungef�hr Null ergeben          */
    /********************************************************************************/

	/* 1. DNDC-Pools */
    fCTotal1 = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCFOMVeryFast + 
	           pCL->fCMicBiomSlow + pCL->fCMicBiomFast + pCL->fCHumusSlow + 
		       pCL->fCHumusFast + pCL->fCHumusStable + pCL->fCO2C;
    fCDiff   = fCTotal1 - fCTotal0;

    fNTotal1 = pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNFOMVeryFast +
	           pCL->fNMicBiomSlow + pCL->fNMicBiomFast + pCL->fNHumusSlow + 
			   pCL->fNHumusFast + pCL->fNHumusStable + pCL->fNH4N + pCL->fNO3N;
	fNDiff   = fNTotal1 - fNTotal0;
   	fCDiffSum += fCDiff;
	fNDiffSum += fNDiff;

	/* 2. ExpertN Pools */
    fCTotal1_B = pCL->fCLitter + pCL->fCManure + pSL->fCHumus + pCL->fCO2C;
    fCDiff_B   = fCTotal1_B - fCTotal0_B;
    fNTotal1_B = pCL->fNLitter + pCL->fNManure + pSL->fNHumus + pCL->fNH4N + pCL->fNO3N;
    fNDiff_B   = fNTotal1_B - fNTotal0_B;
	/* Massenbilanz Ende */


  }   /* Schichtweise */
  return 0;
}
                   
/********************************************************************************************/
/* Procedur    :   MinerDaisy()                                                             */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Mineralisierung                                                          */
/*                 Methode Daisy (Hansen, Jensen,Nielsen, Svendsen, 1990)                   */
/*                                                                                          */
/*              GSF/ch                  20.04.95                                            */
/*                Nicht ausgetestet dm, 13.1.97												*/
/*                                                           								*/
/*                 /ab  20.03.01 �berarbeitet                                               */
/*                 /ab  19.12.01 �berarbeitet                                               */
/********************************************************************************************/
/* ver�nd. Var.		pCL->fCFOMFast               pCL->fNFOMFast                             */
/*					pCL->fCFOMSlow               pCL->fNFOMSlow                             */
/*					pCL->fCMicBiomDenit          pCL->fNMicBiomDenit                        */
/*					pCL->fCMicBiomFast           pCL->fNMicBiomFast                         */
/*					pCL->fCMicBiomSlow           pCL->fNMicBiomSlow                         */
/*                  pCL->fCHumusFast             pCL->fNHumusFast                           */
/*                  pCL->fCHumusSlow             pCL->fNHumusSlow                           */
/*					pCL->fCLitter				 pCL->fNLitter                              */
/*					pCL->fCManure				 pCL->fNManure                              */
/*					pSL->fCHumus				 pSL->fNHumus                               */
/*                  pCL->fCSoilMicBiom			 pCL->fNSoilMicBiom                         */
/*					pCL->fCO2C                   pCL->fNH4N                                 */
/*                  pCL->fCsolC                  pCL->fNO3N                                 */
/*                  pCL->fCMicBiomDenit          pCL->fMicBiomSlowDecMaxR                   */
/*		            pCL->fCO2ProdR               pCL->fMicBiomFastDecMaxR                   */
/*					pCL->fNImmobR                pCL->fMinerR                               */
/*				    pCL->fNLitterImmobR          pCL->fLitterMinerR                         */
/*                  pCL->fNManureImmobR          pCL->fManureMinerR                         */
/*                  corr.Lehm                    pCL->fHumusMinerR                          */
/*					corr.Temp                    corr.Feucht                                */
/********************************************************************************************/
int WINAPI MinerDAISY(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr corr  = {(float)1}; 
  
  /******************************************************************/
  /*           Deklaration lokaler Variablen                        */
  /******************************************************************/

  /* 1. Variablen f�r die C-Fl�sse */
  float fCAOM1DecayR,fCAOM2DecayR,fCBOM1DecayR,fCBOM2DecayR,fCSOM1DecayR,fCSOM2DecayR;
  float fCAOM1ToBOM1R,fCAOM1ToBOM2R,fCAOM1ToCO2R;
  float fCAOM2ToBOM2R,fCAOM2ToCO2R;
  float fCBOM1ToBOM2R,fCBOM1ToSOM2R,fCBOM1ToCO2R;
  float fBOM2ToSOM2C,fBOM2InternC,fBOM2ToCO2C;
  float fSOM1ToBOM1C,fSOM1ToCO2C;
  float fSOM2ToSOM1C,fSOM2ToBOM1C,fSOM2ToCO2C;
  float fBOM1_DeathC,fBOM2_DeathC;
  float fBOM1_MainC,fBOM2_MainC;
 
  /* 2. Variablen f�r die C/N Verh�ltnisse */
  float fCN_FOMSlow     = (float)100.0;   /*! org.:  100*/
  float fCN_FOMFast     = (float) 20.0;   /*! org.:  unterschiedlich*/
  float fCN_MicBiomSlow = (float) 10.0;   /*! org.:  10 */
  float fCN_MicBiomFast = (float) 8.0;    /*! org.:  6  */
  float fCN_MicBiomDenit =(float) 8.0;   /* SG 20160802 */
  float fCN_HumusSlow   = (float) 12.0;   /*! org.:  12 */
  float fCN_HumusFast   = (float)  8.0;   /*! org.:  10 */

  /* 3. Variablen f�r die Aufteilung der Stofffl�sse  */
  float fBOM1      = (float)0.5;    /* org.:  0.5 */
  float fSOM2      = (float)0.4;    /* org.:  0.4 */
  float fSOM1      = (float)0.1;    /* org.:  0.1 */
  float fEff       = (float)0.55;   /* org.:  0.6 */


 
  /* 4. Sterbe- und Erhaltungsraten der Biomasse*/
  float BOM1death        = (float)0.001; /* org.:  0.01 */
  float BOM1maintenance  = (float)0.01;  /* org.:  0.1 */
  float BOM2death        = (float)0.01;  /* org.:  0.1 */
  float BOM2maintenance  = (float)0.01;  /* org.:  0.1 */
  
  /* 5. Variablen f�r die Abbau-Begrenzung bei mangelndem Nmin */
  float fNH4ImmMaxR = (float)0.5;
  float fNO3ImmMaxR = (float)0.5;
  float fRed,fNH4ImmN,fNO3ImmN;  

  /* 6. Variablen f�r die Berechnung der N-Mineralisation/N-Immobilisierung*/
  float help1,help2,help3,help4,help5,help6,help7,help8;
  float fToNH4R,fImmR,fNH4NR,fNO3NR;

  /* 7. Variablen f�r die Massenbilanz*/
  float fCTotal0,fNTotal0;                     /* Summe: AOM + BOM + SOM zu Beginn des Zeitschritts */
  float fCTotal1,fNTotal1;		       		   /* Summe: AOM + BOM + SOM zum Ende des Zeitschritts */
  float fCDiff,fNDiff;                         /* Differenz zwischen momentaner und anfaenglicher Summe*/
  static float fCDiffSum  = (float)0.0;
  static float fNDiffSum  = (float)0.0;

  /* 8. Variablen f�r die Reduktionsfunktionen*/
  const float T0= (float)0;              /* Parameter f�r Temperatur-Korrektur */
  const float T1= (float)20;

  float fTempCorr_CAOM1, fTempCorr_CAOM2;
  float fTempCorr_CBOM1, fTempCorr_CBOM2;
  float fTempCorr_CSOM1, fTempCorr_CSOM2;
  float fTempCorr_DBOM1, fTempCorr_DBOM2;
  float fTempCorr_MBOM1, fTempCorr_MBOM2;

  float f1;                              /* Parameter f�r Feuchte-Korrektur */
  float wmin,wlow,whigh,wmax;
  
  const float C0= (float)25.0;           /* Parameter f�r Ton-Korrektur */
  const float C1= (float)0.02;
  
  /*UNKLAR!!!                                 Quotient aus mikrobieller Biomasse und totalem org. C [1] */
  const float fFD = (float)0.02; /* orig.: 0.02   Quotient aus Denitrifikanten und mikrobieller Biomasse  [1]  */

  /* 9 Variablen f�r die Anteilsberechnung */
  float afAnt[4], fC_Old, fN_Old;

  
  if (fParBOM1 != 0)
     fBOM1 = fParBOM1;

  
  if (fParSOM1 != 0)
    fSOM1 = fParSOM1;
  
  if (fParSOM2 != 0)
     fSOM2 = fParSOM2;
  
  if (fParEff != 0)
     fEff = fParEff;
  
 

  N_ZERO_LAYER            //Setzen der Pointer auf die nullte Schicht 
  
  SurfaceMiner(exp_p);  //Aufruf der Oberfl�chenmineralisierung

  for (N_SOIL_LAYERS)     //schichtweise Berechnung
  {
  /* m�gliche Standardinitialisierung 
  if (SimStart(pTi))
  {
   pCL->fCFOMSlow     = (float)0.588 * pCL->fCLitter;
   pCL->fCFOMFast     = (float)0.392 * pCL->fCLitter;
   pCL->fCMicBiomSlow = (float)0.01  * pCL->fCLitter;
   pCL->fCMicBiomFast = (float)0.01  * pCL->fCLitter;
  }*/ /* Ende der Startinitialisierung */


  /***********************************************************************/
  /* �bertragen der Mengen in die spezifischen DAISY-Pools               */
  /* Litter         = AOM1 = FOMSlow                                     */
  /* Manure         = AOM2 = FOMFast                                     */
  /* Humus (Anteil) = BOM1 = MicBiomSlow                                 */
  /* Humus (Anteil) = BOM2 = MicBiomFast                                 */
  /* Humus (Anteil) = SOM1 = HumusSlow                                   */
  /* Humus (Anteil) = SOM2 = HumusFast                                   */
  /*	                                                                 */
  /* Berechnung der Anteile aus dem Humus anhand der relativen           */
  /* Verh�ltnisse zum Ende des vorherigen Zeitsschritts.                 */
  /***********************************************************************/
 
  pCL->fCFOMSlow = pCL->fCLitter;
  pCL->fCFOMFast = pCL->fCManure;

  pCL->fNFOMSlow = pCL->fNLitter;
  pCL->fNFOMFast = pCL->fNManure; 

  if(!SimStart(pTi))
  {
   fC_Old = pCL->fCMicBiomSlow + pCL->fCMicBiomFast +
               pCL->fCHumusSlow + pCL->fCHumusFast;

   if(fC_Old)
   {
    afAnt[0]  = pCL->fCMicBiomSlow / fC_Old;
    afAnt[1]  = pCL->fCMicBiomFast / fC_Old;
    afAnt[2]  = pCL->fCHumusSlow   / fC_Old;
    afAnt[3]  = pCL->fCHumusFast   / fC_Old;
	 	 
    pCL->fCMicBiomSlow = afAnt[0] * pSL->fCHumus;
    pCL->fCMicBiomFast = afAnt[1] * pSL->fCHumus;
    pCL->fCHumusSlow   = afAnt[2] * pSL->fCHumus;
    pCL->fCHumusFast   = afAnt[3] * pSL->fCHumus;
   }
	 
   fN_Old = pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
                pCL->fNHumusSlow + pCL->fNHumusFast;

   if(fN_Old)
   {
    afAnt[0]  = pCL->fNMicBiomSlow / fN_Old;
    afAnt[1]  = pCL->fNMicBiomFast / fN_Old;
    afAnt[2]  = pCL->fNHumusSlow   / fN_Old;
    afAnt[3]  = pCL->fNHumusFast   / fN_Old;
 	 	 
    pCL->fNMicBiomSlow = afAnt[0] * pSL->fNHumus;
    pCL->fNMicBiomFast = afAnt[1] * pSL->fNHumus;
    pCL->fNHumusSlow   = afAnt[2] * pSL->fNHumus;
    pCL->fNHumusFast   = afAnt[3] * pSL->fNHumus;
   }
  }
 
  /******************************************************************/
  /*                          Reduktionsfunktionen                  */
  /******************************************************************/
   	
	/* 1. Temperatur-Reduktionsfunktion, Q10 Funktion*/
    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
		
	/* Pool specific temperature functions begin below (Scott Demyan, 2015.07.06)*/	
	if (iTempCorr == 1)
	{
      fTempCorr_CAOM1 = abspower(fParCAOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
	  fTempCorr_CAOM2 = abspower(fParCAOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
      fTempCorr_CBOM1 = abspower(fParCBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
	  fTempCorr_CBOM2 = abspower(fParCBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
      fTempCorr_CSOM1 = abspower(fParCSOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/ 
	  fTempCorr_CSOM2 = abspower(fParCSOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
      fTempCorr_DBOM1 = abspower(fParDBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
	  fTempCorr_DBOM2 = abspower(fParDBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
      fTempCorr_MBOM1 = abspower(fParMBOM1Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
	  fTempCorr_MBOM2 = abspower(fParMBOM2Q10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0)); /* exponential*/
	}
	/* Pool specific temperature functions end*/   

	
	/* 2. Feuchte-Reduktionsfunktion, Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (float)-31622770;    //pF = 6.5
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160;        //pF = 2.5
    wlow = WATER_CONTENT(f1);
    f1= (float)-316;         //pF = 1.5
    whigh = WATER_CONTENT(f1);
    f1= (float)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							wmax, pPA->fMinerSatActiv);

   /* Original Reduktionsfunktionen 
    1. Temperatur-Reduktionsfunktion
	if (pHL->fSoilTemp >= T1 ) 
       corr.Temp = (float) exp((double)0.47 - (double)0.027 * (double)pHL->fSoilTemp
               + (double)0.00193 * (double)pHL->fSoilTemp * (double)pHL->fSoilTemp);
            
    else if (pHL->fSoilTemp >= T0) 
       corr.Temp = (float)0.1 * pHL->fSoilTemp;
                   
    else 
       corr.Temp = (float)0.0;                        
 */

  /* 3. Tongehalt-Reduktonsfunktion*/
    if (pSL->fClay <= C0)
	   corr.Lehm = (float)1.0 - C1 * pSL->fClay;

     else
	   corr.Lehm = (float)1.0 - C1 * C0;
	 /* Reduktionsfunktion Ende */


	/******************************************************************/
    /*    C  Mineralisierungsraten (dC/dt )                           */
    /******************************************************************/
	
	/* 1. CN Werte berechnen */
	fCN_FOMSlow     = (pCL->fNFOMSlow && pCL->fCFOMSlow) ?
    	pCL->fCFOMSlow/pCL->fNFOMSlow
    	:fCN_FOMSlow; 

	fCN_FOMFast     = (pCL->fNFOMFast && pCL->fCFOMFast) ?
    	pCL->fCFOMFast/pCL->fNFOMFast
    	:fCN_FOMFast; 

//  Wenn der CN Wert der mikrobiellen Biomasse variabel sein soll 
 	fCN_MicBiomSlow     = (pCL->fNMicBiomSlow && pCL->fCMicBiomSlow) ?
    	pCL->fCMicBiomSlow/pCL->fNMicBiomSlow
    	:fCN_MicBiomSlow; 
  	
 	fCN_MicBiomFast     = (pCL->fNMicBiomFast && pCL->fCMicBiomFast) ?
    	pCL->fCMicBiomFast/pCL->fNMicBiomFast
    	:fCN_MicBiomFast; 

//SG 20160802
 	fCN_MicBiomDenit     = (pCL->fNMicBiomDenit && pCL->fCMicBiomDenit) ?
    	pCL->fCMicBiomDenit/pCL->fNMicBiomDenit
    	:fCN_MicBiomDenit; 


	fCN_HumusSlow     = (pCL->fNHumusSlow && pCL->fCHumusSlow) ?
    	pCL->fCHumusSlow/pCL->fNHumusSlow
    	:fCN_HumusSlow; 
	
	fCN_HumusFast     = (pCL->fNHumusFast && pCL->fCHumusFast) ?
    	pCL->fCHumusFast/pCL->fNHumusFast
    	:fCN_HumusFast; 
	

	/* Original-Abbaurraten, lokal ver�nderbar
    pCL->fFOMSlowDecMaxR      = (float)0.05;
    pCL->fFOMFastDecMaxR      = (float)0.005;
    pCL->fHumusSlowMaxDecMaxR = (float)0.00014;
    pCL->fHumusFastMaxDecMaxR = (float)0.0000027;
    pCL->fMicBiomSlowDecMaxR  = BOM1death + BOM1maintenance;
    pCL->fMicBiomFastDecMaxR  = BOM2death + BOM2maintenance;*/
    
    /* 1. Kohlenstoff-Mineralisierungs-Raten (dC/dt)*/
    fCAOM1DecayR    = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR       * corr.Temp * corr.Feucht;
    fCAOM2DecayR    = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR       * corr.Temp * corr.Feucht;
    fCBOM1DecayR    = pCL->fCMicBiomSlow * pCL->fMicBiomSlowDecMaxR   * corr.Temp * corr.Feucht * corr.Lehm;
    fCBOM2DecayR    = pCL->fCMicBiomFast * pCL->fMicBiomFastDecMaxR   * corr.Temp * corr.Feucht;
    fCSOM1DecayR    = pCL->fCHumusSlow   * pCL->fHumusSlowMaxDecMaxR  * corr.Temp * corr.Feucht * corr.Lehm;
    fCSOM2DecayR    = pCL->fCHumusFast   * pCL->fHumusFastMaxDecMaxR  * corr.Temp * corr.Feucht * corr.Lehm;

    fBOM1_DeathC    = pCL->fCMicBiomSlow * BOM1death                  * corr.Temp * corr.Feucht * corr.Lehm;
	fBOM2_DeathC    = pCL->fCMicBiomFast * BOM2death                  * corr.Temp * corr.Feucht;
    fBOM1_MainC     = pCL->fCMicBiomSlow * BOM1maintenance            * corr.Temp * corr.Feucht * corr.Lehm;
    fBOM2_MainC     = pCL->fCMicBiomFast * BOM2maintenance            * corr.Temp * corr.Feucht;

	if (iTempCorr == 1)
	{
    fCAOM1DecayR    = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR       * fTempCorr_CAOM1 * corr.Feucht;
    fCAOM2DecayR    = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR       * fTempCorr_CAOM2 * corr.Feucht;
    fCBOM1DecayR    = pCL->fCMicBiomSlow * pCL->fMicBiomSlowDecMaxR   * fTempCorr_CBOM1 * corr.Feucht * corr.Lehm;
    fCBOM2DecayR    = pCL->fCMicBiomFast * pCL->fMicBiomFastDecMaxR   * fTempCorr_CBOM2 * corr.Feucht;
    fCSOM1DecayR    = pCL->fCHumusSlow   * pCL->fHumusSlowMaxDecMaxR  * fTempCorr_CSOM1 * corr.Feucht * corr.Lehm;
    fCSOM2DecayR    = pCL->fCHumusFast   * pCL->fHumusFastMaxDecMaxR  * fTempCorr_CSOM2 * corr.Feucht * corr.Lehm;

    fBOM1_DeathC    = pCL->fCMicBiomSlow * BOM1death                  * fTempCorr_DBOM1 * corr.Feucht * corr.Lehm;
	fBOM2_DeathC    = pCL->fCMicBiomFast * BOM2death                  * fTempCorr_DBOM1 * corr.Feucht;
    fBOM1_MainC     = pCL->fCMicBiomSlow * BOM1maintenance            * fTempCorr_MBOM1 * corr.Feucht * corr.Lehm;
    fBOM2_MainC     = pCL->fCMicBiomFast * BOM2maintenance            * fTempCorr_MBOM2 * corr.Feucht;
	}


   /*************************************************************************/
   /*  Berechnung der Ammonium Bilanz bei jedem relvanten Pool�bergang      */
   /*  Summe positiv = Nettomineralisierung                                 */
   /*  Summe negativ = Nettoimmobilisierung                                 */
   /*************************************************************************/	
    
    /*AOM1->BOM1 old
    help1 = fBOM1 * fCAOM1DecayR * ((float)1.0 / fCN_FOMSlow - fEff / fCN_MicBiomSlow);
	*/
   
   /*AOM1->BOM1*/
    help1 = fBOM1 * fCAOM1DecayR * ((float)1.0 / fCN_FOMSlow - fEff_AOM1 / fCN_MicBiomSlow);

	/*AOM1->BOM2 old
    help2 = ((float)1.0 - fBOM1) * fCAOM1DecayR * ((float)1.0 / fCN_FOMSlow - fEff / fCN_MicBiomFast);
	*/
   
   /*AOM1->BOM2*/
    help2 = ((float)1.0 - fBOM1) * fCAOM1DecayR * ((float)1.0 / fCN_FOMSlow - fEff_AOM1 / fCN_MicBiomFast);

   /*AOM2->BOM2 old
    help3 = fCAOM2DecayR * ((float)1.0 / fCN_FOMFast - fEff / fCN_MicBiomFast);
	*/
	
   /*AOM2->BOM2*/
    help3 = fCAOM2DecayR * ((float)1.0 / fCN_FOMFast - fEff_AOM2 / fCN_MicBiomFast);

   /*BOM1->BOM2 old
    help4 = fCBOM1DecayR / fCN_MicBiomSlow -  (fSOM2 / fCN_HumusFast + ((float) 1.0 - fSOM2) * fEff / fCN_MicBiomFast) * fBOM1_DeathC;
	*/
	
   /*BOM1->BOM2*/
    help4 = fCBOM1DecayR / fCN_MicBiomSlow -  (fSOM2 / fCN_HumusFast + ((float) 1.0 - fSOM2) * fEff_BOM1 / fCN_MicBiomFast) * fBOM1_DeathC;

	/*SOM1->BOM1 old
    help5 = fCSOM1DecayR * ((float)1.0 / fCN_HumusSlow - fEff / fCN_MicBiomSlow);
	*/
	
	/*SOM1->BOM1*/
    help5 = fCSOM1DecayR * ((float)1.0 / fCN_HumusSlow - fEff_SOM1 / fCN_MicBiomSlow);

   /*BOM2->BOM2 old
    help6 = fCBOM2DecayR / fCN_MicBiomFast -  (fSOM2 / fCN_HumusFast + ((float) 1.0 - fSOM2) * fEff / fCN_MicBiomFast) * fBOM2_DeathC;
	*/
	
   /*BOM2->BOM2*/
    help6 = fCBOM2DecayR / fCN_MicBiomFast -  (fSOM2 / fCN_HumusFast + ((float) 1.0 - fSOM2) * fEff_BOM2 / fCN_MicBiomFast) * fBOM2_DeathC;
	
	/*SOM2->BOM1 old
    help7 = fCSOM2DecayR * ((float)1.0 / fCN_HumusFast - fSOM1 / fCN_HumusSlow - fEff / fCN_MicBiomSlow + fEff * fSOM1 / fCN_MicBiomSlow);
	*/

	/*SOM2->BOM1*/
    help7 = fCSOM2DecayR * ((float)1.0 / fCN_HumusFast - fSOM1 / fCN_HumusSlow - fEff_SOM2 / fCN_MicBiomSlow + fEff_SOM2 * fSOM1 / fCN_MicBiomSlow);
  


   /*Summe �ber alle relevanten Fl�sse*/
	help8 = help1 + help2 + help3 + help4 + help5 + help6 + help7;

	/*Entscheidung ob Mineralisierung oder Immobilisierung*/
	fToNH4R = (help8 > 0) ? help8 : 0;
    fImmR   = (help8 < 0) ? (float)-1 * help8 : 0;


   /*************************************************************/
   /*     Reduktion des AOM-Abbaus bei Nmin-Mangel!             */
   /*************************************************************/

	if(fImmR)
	{
     /*Maximale Immobilisierungsraten*/
   	 fNH4ImmN = fNH4ImmMaxR * pCL->fNH4N;
     fNO3ImmN = fNO3ImmMaxR * pCL->fNO3N;
	 
     /*Berechnung des Reduktionsfaktors*/  	
     fRed = (float)1.0;                      // Reduktionsfaktor =1 => keine Reduktion 

	 if (fImmR  > (fNH4ImmN + fNO3ImmN))
          fRed = (fNH4ImmN + fNO3ImmN) / fImmR;
	    
     help1 = fRed * help1;
     help2 = fRed * help2;

	 //Added by Hong on 20180124: it was missed before, supposed to be an implementation bug!!!
     help3 = fRed * help3;
	 //End of Hong

	 fCAOM1DecayR  = fRed * fCAOM1DecayR;
	 fCAOM2DecayR  = fRed * fCAOM2DecayR;

	 help8 = help1 + help2 + help3 + help4 + help5 + help6 + help7;
     fImmR = help8 * (float)-1;
	}
	
	/*�bertragung auf globale Variablen*/
 	pCL->fLitterMinerR  = fCAOM1DecayR/fCN_FOMSlow;
	pCL->fManureMinerR  = fCAOM2DecayR/fCN_FOMFast;
    pCL->fHumusMinerR   = fCSOM1DecayR/fCN_HumusSlow + 
	                      fCSOM2DecayR/fCN_HumusFast;
  
	pCL->fNLitterImmobR = ((help1 + help2) < 0) ? (float)-1 * (help1 + help2) : 0;
    pCL->fNManureImmobR = (help3 < 0) ? (float)-1 * help3 : 0;

	pCL->fNImmobR = fImmR;
    pCL->fMinerR = fToNH4R;


   /********************************************************************************/
   /*    Veraenderung der C-Pools pro Zeitschritt                                  */ 
   /*    Da CN-Verh�ltnis in den Pools konstant ist, ist eine separate             */
   /*    Modellierung des N redundant.                                             */
   /********************************************************************************/
    
	/*Gesamt C-Mengen vor dem Zeitschritt*/
	fCTotal0 = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCMicBiomSlow
              + pCL->fCMicBiomFast + pCL->fCHumusSlow + pCL->fCHumusFast
              + pCL->fCO2C;

    pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_FOMSlow;
    pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_FOMFast; 
    pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_MicBiomSlow;
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_MicBiomFast;
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / fCN_HumusSlow;
    pCL->fNHumusFast   = pCL->fCHumusFast   / fCN_HumusFast;



	fNTotal0 = pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNMicBiomSlow +
			   pCL->fNMicBiomFast + pCL->fNHumusSlow + pCL->fNHumusFast +
               pCL->fNH4N + pCL->fNO3N;

	/* Die Ver�nderung in den Pools pro Zeitschritt (direkt, ohne Berechnung der Fl�sse) */
    pCL->fCFOMSlow     -= fCAOM1DecayR * DeltaT;
    
	pCL->fCFOMFast     -= fCAOM2DecayR * DeltaT; 
    
	/* original 
	pCL->fCMicBiomSlow += (fEff_SOM1 * (fCSOM1DecayR + ((float)1.0-fSOM1) * fCSOM2DecayR

	                       + fBOM1 * fCAOM1DecayR) - fCBOM1DecayR) * DeltaT;
	*/
	
	pCL->fCMicBiomSlow += (fEff_SOM1 * fCSOM1DecayR + ((float)1.0-fSOM1) * fEff_SOM2 * fCSOM2DecayR
	                       + fBOM1 * fCAOM1DecayR * fEff_AOM1 - fCBOM1DecayR) * DeltaT;
						   
						   
    pCL->fCHumusSlow   += (fSOM1*fCSOM2DecayR - fCSOM1DecayR) * DeltaT;

    /* original
	pCL->fCMicBiomFast += (fEff * (((float)1.0-fSOM2) * (fBOM1_DeathC + fBOM2_DeathC) 
                          + ((float)1.0-fBOM1) * fCAOM1DecayR + fCAOM2DecayR) - fCBOM2DecayR) * DeltaT; 

    
	*/
	pCL->fCMicBiomFast += ((((float)1.0-fSOM2) * (fEff_BOM1*fBOM1_DeathC + fEff_BOM2*fBOM2_DeathC) 
                          + ((float)1.0-fBOM1) * fCAOM1DecayR * fEff_AOM1 + fCAOM2DecayR * fEff_AOM2) - fCBOM2DecayR) * DeltaT; 

	
	pCL->fCHumusFast   += (fSOM2 * (fBOM1_DeathC + fBOM2_DeathC) - fCSOM2DecayR) * DeltaT;

	
	// original
    //pCL->fCO2ProdR     = (((float)1.0 - fEff) * (fCAOM1DecayR + fCAOM2DecayR + fCSOM1DecayR + fCSOM2DecayR) + fCBOM1DecayR + fCBOM2DecayR
	   //                   - (fSOM1 - fEff * fSOM1) * fCSOM2DecayR - (fEff + fSOM2 - fEff * fSOM2) * (fBOM1_DeathC + fBOM2_DeathC));
    
	pCL->fCO2ProdR     = ((float)1.0 - fEff_AOM1) * fCAOM1DecayR + ((float)1.0 - fEff_AOM2) * fCAOM2DecayR + ((float)1.0 - fEff_SOM1) * fCSOM1DecayR + ((float)1.0 - fEff_SOM2) *fCSOM2DecayR + fCBOM1DecayR + fCBOM2DecayR
	                      - (fSOM1 - fEff_SOM2 * fSOM1) * fCSOM2DecayR - (fEff_BOM1 + fSOM2 - fEff_BOM1 * fSOM2) * fBOM1_DeathC - (fEff_BOM2 + fSOM2 - fEff_BOM2 * fSOM2) * fBOM2_DeathC;
    
	pCL->fCO2C += pCL->fCO2ProdR * DeltaT;

   /*Die Raten pro Zeitschritt, eigentlich �berfl�ssig aber f�r Punkt 4 und 5 n�tig */
   /*Eventuell mit einem Fehler, da bei diesen Fl�ssen momentan keine Sterberate be-*/
   /*r�cksichtigt wird.                                                             */
	
	/* old
    fCAOM1ToBOM1R    = fEff * fBOM1 * fCAOM1DecayR;
    fCAOM1ToBOM2R    = fEff * ((float)1.0 - fBOM1) * fCAOM1DecayR;
    fCAOM1ToCO2R     = ((float)1.0 - fEff) * fCAOM1DecayR;
	*/
	
	fCAOM1ToBOM1R    = fEff_AOM1 * fBOM1 * fCAOM1DecayR;
    fCAOM1ToBOM2R    = fEff_AOM1 * ((float)1.0 - fBOM1) * fCAOM1DecayR;
    fCAOM1ToCO2R     = ((float)1.0 - fEff_AOM1) * fCAOM1DecayR;

	/* old
	fCAOM2ToBOM2R    = fEff * fCAOM2DecayR;
    fCAOM2ToCO2R     = ((float)1.0 - fEff) * fCAOM2DecayR;
	*/
	
	fCAOM2ToBOM2R    = fEff_AOM2 * fCAOM2DecayR;
    fCAOM2ToCO2R     = ((float)1.0 - fEff_AOM2) * fCAOM2DecayR;
		
	/* old
	fCBOM1ToBOM2R    = fEff *((float)1.0 - fSOM2) * fCBOM1DecayR;
    fCBOM1ToSOM2R    = fSOM2 * fCBOM1DecayR;
    fCBOM1ToCO2R     = ((float)1.0 - fEff) * ((float)1.0 - fSOM2) * fCBOM1DecayR;
	*/
	
	fCBOM1ToBOM2R    = fEff_BOM1 *((float)1.0 - fSOM2) * fCBOM1DecayR;
    fCBOM1ToSOM2R    = fSOM2 * fCBOM1DecayR;
    fCBOM1ToCO2R     = ((float)1.0 - fEff_BOM1) * ((float)1.0 - fSOM2) * fCBOM1DecayR;

    /* old
	fBOM2InternC    = fEff * ((float)1.0 - fSOM2)* fCBOM2DecayR;  
    fBOM2ToSOM2C    = fSOM2 * fCBOM2DecayR;
    fBOM2ToCO2C     = ((float)1.0 - fEff) * ((float)1.0 - fSOM2) * fCBOM2DecayR;
	*/
	
	fBOM2InternC    = fEff_BOM2 * ((float)1.0 - fSOM2)* fCBOM2DecayR;  
    fBOM2ToSOM2C    = fSOM2 * fCBOM2DecayR;
    fBOM2ToCO2C     = ((float)1.0 - fEff_BOM2) * ((float)1.0 - fSOM2) * fCBOM2DecayR;

    /* old
	fSOM1ToBOM1C    = fEff * fCSOM1DecayR;
    fSOM1ToCO2C     = ((float)1.0 - fEff) * fCSOM1DecayR;
	*/
	
	fSOM1ToBOM1C    = fEff_SOM1 * fCSOM1DecayR;
    fSOM1ToCO2C     = ((float)1.0 - fEff_SOM1) * fCSOM1DecayR;

	/* old
	fSOM2ToBOM1C    = fEff * ((float)1.0 - fSOM1) * fCSOM2DecayR;
    fSOM2ToSOM1C    = fSOM1 * fCSOM2DecayR;
    fSOM2ToCO2C     = ((float)1.0 - fEff) * ((float)1.0 - fSOM1) * fCSOM2DecayR;
	*/
	
	fSOM2ToBOM1C    = fEff_SOM2 * ((float)1.0 - fSOM1) * fCSOM2DecayR;
    fSOM2ToSOM1C    = fSOM1 * fCSOM2DecayR;
    fSOM2ToCO2C     = ((float)1.0 - fEff_SOM2) * ((float)1.0 - fSOM1) * fCSOM2DecayR;


    /* UNKLAR!!!!*/
    /* 4. Veraenderung des BOMD-Pools pro Zeitschritt              */
    /*    Annahme: W�hrend Dekompostierung wachsen Denitrifikanten */
    /*             wie die restliche labile mikrobielle Biomasse   */
      pCL->fCMicBiomDenit += fFD * (fCAOM1ToBOM1R + fCAOM1ToBOM2R + fCAOM2ToBOM2R + fSOM1ToBOM1C + fSOM2ToBOM1C)
                         * DeltaT;

    /* UNKLAR!!!!*/
	/* 5. Festlegung des Csol-Pools  */
    pCL->fCsolC = (fCBOM1DecayR + fCBOM2DecayR) * fSOM2 + fSOM1ToBOM1C + fSOM2ToBOM1C ;
                        
    pCh->pCLayer->fCsolC += pCL->fCsolC;
     

    /********************************************************************************/
    /*        Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt        */
    /********************************************************************************/

    /* 1. Ammonium-Netto-Mineralisierung/-Immobilisierung */
	
    if (fToNH4R > 0) //Mineralisierung
     
      pCL->fNH4N += fToNH4R * DeltaT;  
      
    else             //Immobilisierung
     { 
	  fNH4NR = fImmR * RelAnteil(pCL->fNH4N,pCL->fNO3N);  
      fNO3NR = fImmR * RelAnteil(pCL->fNO3N,pCL->fNH4N);  	
	 
	  pCL->fNH4N -= fNH4NR * DeltaT; 
	  pCL->fNO3N -= fNO3NR * DeltaT;
	 }

    /* 2. Werte in den N-Pools */
    pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_FOMSlow;
    pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_FOMFast; 
    pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_MicBiomSlow;
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_MicBiomFast;
    pCL->fNMicBiomDenit= pCL->fCMicBiomDenit / fCN_MicBiomDenit;
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / fCN_HumusSlow;
    pCL->fNHumusFast   = pCL->fCHumusFast   / fCN_HumusFast;


    /********************************************************************************/
    /* �bertragen der Ver�nderungen auf die Pools Litter, Manure,Humus u.           */
    /********************************************************************************/

    pCL->fCLitter = pCL->fCFOMSlow; 
    pCL->fNLitter = pCL->fNFOMSlow; 
 
    pCL->fCManure = pCL->fCFOMFast; 
    pCL->fNManure = pCL->fNFOMFast; 

    pSL->fCHumus = pCL->fCMicBiomSlow + pCL->fCMicBiomFast
                   + pCL->fCHumusSlow + pCL->fCHumusFast;
    pSL->fNHumus = pCL->fNMicBiomSlow + pCL->fNMicBiomFast
                   + pCL->fNHumusSlow + pCL->fNHumusFast;

	pCL->fCSoilMicBiom = pCL->fCMicBiomSlow + pCL->fCMicBiomFast;
	pCL->fNSoilMicBiom = pCL->fNMicBiomSlow + pCL->fNMicBiomFast;
    /*�bertragen auf ExpertN-Pools Ende */



  /*Massenbilanz*/ 
  fCTotal1 = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCMicBiomSlow
              + pCL->fCMicBiomFast + pCL->fCHumusSlow + pCL->fCHumusFast
              + pCL->fCO2C;

  fNTotal1 = pCL->fNFOMSlow + pCL->fNFOMFast +   
			 pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
			 pCL->fNHumusSlow + pCL->fNHumusFast + 
             pCL->fNH4N + pCL->fNO3N;

    fCDiff = fCTotal1 - fCTotal0;    
    fCDiffSum += fCDiff;

    fNDiff = fNTotal1 - fNTotal0;
    fNDiffSum += fNDiff;
  }
 return 0;
}

/*********************************************************************************************/
/* Procedur    :   MinerOMI                                                                  */
/* Beschreibung:   Mineralisierung/Immobilisierung                                           */
/*                 Methode Das Beste aus SOILN/DAISY/NITS/CERES/DNDC/NCSOIL                  */
/*                                                                                           */
/*              GSF/ab  Axel Berkenkamp                  19.06.01                            */
/*                                                                                           */
/*                                                                                           */
/*********************************************************************************************/
/* ver�nd. Var.		pCL->fCLitter                  pCL->fNLitter                             */
/*					pCL->fCManure                  pCL->fNManure                             */
/*					pSL->fCHumus                   pSL->fNHumus                              */
/*					pCL->fCO2C                     pCL->fNH4N                                */
/*					pCL->fCO2ProdR                 pCL->fNO3N                                */
/*                  pCL->fHumusMinerR              pCL->fImmobR                              */
/*					pCL->fLitterMinerR             struct.Temp                               */
/*                  pCL->fManureMinerR             struct.Feucht                             */
/*					                                                                         */
/*********************************************************************************************/
int WINAPI MinerOMI(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr corr={(float)1};

  /******************************************************************/
  /*           Deklaration lokaler Variablen                        */
  /******************************************************************/

  /* 1. Variablen f�r die Reduktionsfunktionen*/
  float f1,wmin,wlow,whigh;
  
  /* 2. Variablen f�r die C-Fl�sse*/
  float fCHumusDecayR,fCLitterDecayR,fCManureDecayR;
  float fCLitterToHumusR,fCLitterToLitterR,fCLitterToCO2R;
  float fCManureToHumusR,fCManureToLitterR,fCManureToCO2R;
  float fCHumusToCO2R,fCHumusToLitterR;

  /* 3. Variablen f�r die N-Fl�sse*/
  float fNHumusDecayR,fNLitterDecayR,fNManureDecayR;
  float fNImmR,fNH4ToLitterR,fNO3ToLitterR,fNHumusToLitterR;
  float fNH4ImmR,fNO3ImmR;
  float fNLitterToHumusR,fNManureToHumusR;
  float fNLitterToNH4R,fNManureToNH4R,fNHumusToNH4R;
  
  /* 4. Variablen f�r die Minereralisation- Immobilisationsentscheidung */
  float fLitterImmR,fManureImmR;
  float fLitterToNH4R,fManureToNH4R;
  float h1,h2, fLitAnt;

  /* 5. Variablen f�r die Immobiliserungsbegrenzung */
  float fNH4ImmMaxR = (float)0.1;
  float fNO3ImmMaxR = (float)0.1;
  float fRed;

  /* 6. Variablen f�r die Aufteilung der Stofffl�sse*/  
  float fEff,fHum;
  
  /* 7. Variablen f�r die Massenbilanz*/
  float fCTotal0,fNTotal0;
  float fCTotal1,fNTotal1;
  float fCDiff,fNDiff;
  static float fCDiffSum   = (float)0.0;
  static float fNDiffSum   = (float)0.0;
  /* Variablendeklaration Ende  */


  N_ZERO_LAYER             //Setzen der Pointer auf die nullte Schicht
   
  SurfaceMiner(exp_p);   //Aufruf der Oberfl�chenmineralisierung 


  for (N_SOIL_LAYERS)      //Schichtweise Berechnung
    {
	/******************************************************************/
    /*                          Reduktionsfunktionen                  */
    /******************************************************************/
  
    /* 1. Temperatur: Ansatz Q10-Funktion, Bezugswert: Tagesmittelwert der jeweiligen Simulationsschicht*/ 
    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));

    /* 2. Feuchte: Ansatz Polygonzug, Bezugswert pF-Wert, Grenzwerten aus dem DAISY-Modell*/
	f1= (float)-31622770.0;    //pF = 6.5
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160.0;        //pF = 2.5
    wlow = WATER_CONTENT(f1);
    f1= (float)-310.6;         //pF = 1.5
    whigh = WATER_CONTENT(f1);
    
	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							pSL->fPorosity, pPA->fMinerSatActiv);
    /* Reduktionsfunktion Ende */

	
	/******************************************************************/
    /*    C und N Mineralisierungsraten (dC/dt  und dN/dt)            */
    /******************************************************************/

    fCHumusDecayR   = pSL->fCHumus  * pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
    fCLitterDecayR  = pCL->fCLitter * pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht;
    fCManureDecayR  = pCL->fCManure * pCL->fManureMinerMaxR * corr.Temp * corr.Feucht;

    fNHumusDecayR   = pSL->fNHumus  * pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
    fNLitterDecayR  = pCL->fNLitter * pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht;
    fNManureDecayR  = pCL->fNManure * pCL->fManureMinerMaxR * corr.Temp * corr.Feucht;	
	/* Mineralisationsraten Ende */

            
    /******************************************************************/
    /* Entscheidung Immobilisierung/Mineralisierung beim Litter u.    */
    /* Manure Abbau. Der Wert h�ngt ab vom CN Wert der abgebauten     */
    /* Substanz, dem CN Wert der aufgebauten Substanz (Mikrobielle    */ 
    /* Biomasse und Humus) sowie dem Effektivit�tsfaktor mit der die  */ 
    /* Mikros Kohlenstoff verwerten k�nnen.                           */
    /* Hilfsvariable positiv = Nettomineralisierung                   */
    /* Hilfsvariable negativ = Nettoimmobilisierung                   */                            
    /******************************************************************/

	/* 1. Lokal ver�nderbare Effektivit�tsfaktoren */
	fEff    = pPA->fMinerEffFac;
    fHum    = pPA->fMinerHumFac;

    /* 2. CN-Werte f�r Litter und Manure */
    pCL->fLitterCN   = (pCL->fNLitter > EPSILON)?
    	pCL->fCLitter/pCL->fNLitter
    	:(float)0.1; 
            
    pCL->fManureCN   = (pCL->fNManure > EPSILON)?
    	pCL->fCManure / pCL->fNManure
    	:(float)0.1; 

    /* 3. Mineralisierungs bzw. Immobilisierungsfaktoren*/
    if (pCL->fLitterCN > (float)0.1)
	{
	 h1            = (float)1 / pCL->fLitterCN - fEff / pCL->fMicBiomCN;
     fLitterToNH4R = (h1 > 0)? h1 : 0;
     fLitterImmR   = (h1 < 0)? (float)-1 * h1 : 0;
	}

	else
    {
     fLitterToNH4R = (float)0;
     fLitterImmR   = (float)0;
    }

	if (pCL->fManureCN > (float)0.1)
    {
     h2     = (float)1 / pCL->fManureCN - fEff / pCL->fMicBiomCN;
     fManureToNH4R = (h2 > 0)? h2 : 0;
     fManureImmR   = (h2 < 0)? (float)-1 * h2 : 0;
    }
  
    else
    {
     fManureToNH4R = (float)0;
     fManureImmR   = (float)0;
    }
    /* Entscheidung Immobilisierung/Mineralisierung Ende*/


    /******************************************************************/
    /* Beschr�nkung des C-Abbaus von Litter und Manure, wenn die      */
    /* Immobilisierung mehr N-Bedarf entwickelt als mineralischer N   */
    /* zur Verf�gung steht.                                           */
    /******************************************************************/

    /* 1. Gesamt N Bedarf */
    fNImmR = fLitterImmR * fCLitterDecayR + fManureImmR * fCManureDecayR;

    /* 2. Maximale Immobilisierungsraten (10% des vorhandenen Nmin)*/
    fNH4ImmR = fNH4ImmMaxR * pCL->fNH4N;
    fNO3ImmR = fNO3ImmMaxR * pCL->fNO3N;
 
    /* 3. Berechnung des Reduktionsfaktors*/  	
    fRed = (float)1.0;           /* Reduktionsfaktor = 1 => keine Reduktion */

    if (fNImmR  > (fNH4ImmR + fNO3ImmR))
     fRed = (fNH4ImmR + fNO3ImmR) / fNImmR;

    if (fLitterImmR)
    {   
     fCLitterDecayR = fRed * fCLitterDecayR;
     fNLitterDecayR = fRed * fNLitterDecayR;
    }

    if (fManureImmR)
    {
     fCManureDecayR = fRed * fCManureDecayR;
     fNManureDecayR = fRed * fNManureDecayR;
    }

    fNImmR = fRed * fNImmR;
    /* Berechnung Reduktion des Abbaus Ende */
        
    fLitAnt = ((fLitterImmR > (float)0.0)&&(fManureImmR > (float)0.0))?
              fLitterImmR / (fLitterImmR + fManureImmR)
              : (float)0;


    /********************************************************************************/
    /*                  Veraenderung des C-Pools pro Zeitschritt                    */
    /********************************************************************************/
 
	/* 1. Gesamt C vor dem Zeitschritt */
	fCTotal0 = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C;

    /* 2. Abbau des C-Litter Pools pro Zeitschritt */
    fCLitterToHumusR  = fCLitterDecayR * fEff * fHum;
    fCLitterToCO2R    = fCLitterDecayR * ((float)1 - fEff);
    fCLitterToLitterR = fCLitterDecayR * fEff * ((float)1 - fHum);
    
    /* 3. Abbau des C-Manure Pools pro Zeitschritt */
    fCManureToHumusR  = fCManureDecayR * fEff * fHum;
    fCManureToCO2R    = fCManureDecayR * ((float)1 - fEff);
    fCManureToLitterR = fCManureDecayR * fEff * ((float)1 - fHum);

    /* 4. Abbau des C-Humus Pools pro Zeitschritt */
    fCHumusToCO2R     = fCHumusDecayR * ((float)1 - fEff);
    fCHumusToLitterR  = fCHumusDecayR * fEff;

	/* 5. Zunahme im CO2 Pool */
    pCL->fCO2ProdR = fCLitterToCO2R +  fCManureToCO2R + fCHumusToCO2R;
 
    /* 6. Veraenderung in den C-Pools */
    pCL->fCLitter -= (fCLitterToHumusR + fCLitterToCO2R - fCManureToLitterR - fCHumusToLitterR) * DeltaT;
    pCL->fCManure -= fCManureDecayR * DeltaT;
	pSL->fCHumus  += (fCLitterToHumusR + fCManureToHumusR - fCHumusToCO2R - fCHumusToLitterR)* DeltaT;
    pCL->fCO2C    += pCL->fCO2ProdR * DeltaT;
    /* Ver�nderungen im C-Pool Ende */


    /********************************************************************************/
    /*      Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt          */
    /********************************************************************************/

	/* 1. Gesamt N vor dem Zeitschritt */
    fNTotal0 = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N;

	/* 2. Immobilisierung in den Litter-Pool */
    fNH4ToLitterR = fNImmR * RelAnteil(pCL->fNH4N,pCL->fNO3N);
    fNO3ToLitterR = fNImmR * RelAnteil(pCL->fNO3N,pCL->fNH4N);
    fNHumusToLitterR = fNHumusDecayR * ((float)1 - fEff);
    //Variante: fNHumusToLitterR = fCHumusToLitterR / pCL->fMicBiomCN;
    
	/* 3. Humifizierung in den Humus-Pool */
    fNLitterToHumusR = fNLitterDecayR * fEff * fHum;
    fNManureToHumusR = fNManureDecayR * fEff * fHum;
	//Variante: fNLitterToHumusR = fCLitterToHumusR / pCL->fMicBiomCN;
    //Variante: fNManureToHumusR = fCManureToHumusR / pCL->fMicBiomCN;


    /* 4. Ammonium-Mineralisierung aus Humus, Litter und Manure */
	fNHumusToNH4R  = fNHumusDecayR * fEff; 
    fNLitterToNH4R = fLitterToNH4R * fCLitterDecayR;
    fNManureToNH4R = fManureToNH4R * fCManureDecayR;
    //Variante : fNHumusToNH4R = fNHumusDecayR - fNHumusToLiitterR;

	/* 5. Ver�nderungen in den N Pools */
    pCL->fNH4N += (fNHumusToNH4R  + fNLitterToNH4R  + fNManureToNH4R -  fNH4ToLitterR) * DeltaT;
    pCL->fNO3N -= fNO3ToLitterR * DeltaT;
    pCL->fNLitter += (fNImmR + fNHumusToLitterR - fNLitterToNH4R - fNLitterToHumusR) * DeltaT;
    pCL->fNManure -= (fNManureToHumusR + fNManureToNH4R) * DeltaT;
    pSL->fNHumus += (fNLitterToHumusR + fNManureToHumusR - fNHumusToNH4R - fNHumusToLitterR) * DeltaT;
    /* Ver�nderungen im N-Pool Ende */

    /* 6 �bertragen auf globale Variablen */
	pCL->fNLitterImmobR = fNImmR * fLitAnt;
   	pCL->fNManureImmobR = fNImmR * ((float)1.0 - fLitAnt);
    pCL->fNImmobR = pCL->fNLitterImmobR + pCL->fNManureImmobR;
    pCL->fHumusMinerR = fNHumusToNH4R;
    pCL->fLitterMinerR = fNLitterToNH4R;
    pCL->fManureMinerR = fNManureToNH4R;
    pCL->fMinerR = pCL->fHumusMinerR + pCL->fLitterMinerR + pCL->fManureMinerR;

    /********************************************************************************/
    /* Massenbilanz, die Summe der Ver�nderungen muss ungef�hr Null ergeben         */
    /********************************************************************************/
 
    fCTotal1 = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C;
    fCDiff   = fCTotal1 - fCTotal0;    

    fNTotal1 = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N;
    fNDiff   = fNTotal1 - fNTotal0;    

    fCDiffSum    += fCDiff;
    fNDiffSum    += fNDiff;
    /* Massenbilanz Ende */

   }    /* Schichtweise */

  return 0;
}                    

/*********************************************************************************************/
/* Procedur    :   MinerBerk                                                                 */
/* Beschreibung:   Mineralisierung/Immobilisierung                                           */
/*                 Methode: Das Beste aus SOILN/LEACHN/DAISY/NITS/CERES/DNDC/NCSOIL          */
/*                                                                                           */
/*              GSF/ab  Axel Berkenkamp                  19.06.01                            */
/*                                                                                           */
/*                                                                                           */
/*********************************************************************************************/
/* ver�nd. Var.		pCL->fCLitter                  pCL->fNLitter                             */
/*					pCL->fCManure                  pCL->fNManure                             */
/*					pSL->fCHumus                   pSL->fNHumus                              */
/*					pCL->fCMicBiomSlow             pSL->fNMicBiomSlow                        */
/*					pCL->fCSoilMicBiom             pSL->fNSoilMicBiom                        */
/*					pCL->fCO2C                     pCL->fNH4N                                */
/*					pCL->fCO2ProdR                 pCL->fNO3N                                */
/*                  pCL->fHumusMinerR              pCL->fImmobR                              */
/*					pCL->fLitterMinerR             pCL->fMinerR                              */
/*                  pCL->fManureMinerR             corr.Feucht                               */
/*					pCL->fMicBiomCN                corr.Temp                                 */
/*                  pCL->fLitterImmobR             pCL->fManureImmobR                        */
/*********************************************************************************************/
int WINAPI ABMinerBERK(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr corr={(float)1};

  /******************************************************************/
  /*           Deklaration lokaler Variablen                        */
  /******************************************************************/

  /* 1. Variablen f�r die Reduktionsfunktionen*/
  float f1,wmin,wlow,whigh,wmax;
  
  /* 2. Variablen f�r die C-Fl�sse*/
  float fCHumusDecayR,fCLitterDecayR,fCManureDecayR,fCBiomDecayR;
  float fCLitterToHumusR,fCLitterToBiomR,fCLitterToCO2R;
  float fCManureToHumusR,fCManureToBiomR,fCManureToCO2R;
  float fCHumusToCO2R,fCHumusToBiomR;
  float fCBiomToHumusR,fCBiomToCO2R,fCBiomToBiomR;

  /* 3. Variablen f�r die N-Fl�sse*/
  float fNHumusDecayR,fNLitterDecayR,fNManureDecayR,fNBiomDecayR;
  float fNImmR,fNH4ToBiomR,fNO3ToBiomR,fNBiomToHumusR;
  float fNH4ImmR,fNO3ImmR;
  float fNLitterToBiomR, fNManureToBiomR,fNBiomToBiomR,fNHumusToBiomR;
  float fNLitterToHumusR,fNManureToHumusR;
  float fNLitterToNH4R,fNManureToNH4R,fNHumusToNH4R,fNBiomToNH4R;

  /* 4. Variablen f�r die Minereralisation- Immobilisationsentscheidung */
  float fLitterImmR,fManureImmR;
  float fLitterToNH4R,fManureToNH4R;
  float h1,h2,fLitAnt;

  /* 5. Variablen f�r die Immobiliserungsbegrenzung */
  float fNH4ImmMaxR = (float)0.1;
  float fNO3ImmMaxR = (float)0.1;
  float fRed;

  /* 6. Variablen f�r die Aufteilung der Stofffl�sse*/  
  float fEff,fHum;

  /* 7. Variablen f�r die Massenbilanz*/
  float fCTotal0,fNTotal0;
  float fCTotal1,fNTotal1;
  float fCDiff,fNDiff;
  static float fCDiffSum   = (float)0.0;
  static float fNDiffSum   = (float)0.0;
  /* Variablendeklaration Ende  */
float fLitterMinerR,fManureMinerR;
float help1;

  N_ZERO_LAYER             //Setzen der Pointer auf die nullte Schicht
   
  SurfaceMiner(exp_p);   //Aufruf der Oberfl�chenmineralisierung 


  for (N_SOIL_LAYERS)      //Schichtweise Berechnung
    {
/*     if(SimStart(pTi))	
	  {
	   pCL->fCMicBiomSlow = (float)0.01 * pCL->fCLitter;
	   pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / pCL->fMicBiomCN;

	   pCL->fCLitter -= pCL->fCMicBiomSlow;
	   pCL->fNLitter -= pCL->fNMicBiomSlow;
	  }
*/
	/******************************************************************/
    /*                          Reduktionsfunktionen                  */
    /******************************************************************/
  
	/* 1. Temperatur-Reduktionsfunktion, Q10 Funktion*/
    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
    
	/* 2. Feuchte-Reduktionsfunktion, Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (float)-31622770;    //pF = 6.5 Orig.: -10000000.0 pF = 6.0 
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160;        //pF = 2.5 Orig.: -3000.0;    pF = 2.48
    wlow = WATER_CONTENT(f1);
    f1= (float)-316;         //pF = 1.5 Orig.: -600.0,     pF = 1.78
    whigh = WATER_CONTENT(f1);
    f1= (float)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							wmax, pPA->fMinerSatActiv);
    /* Reduktionsfunktion Ende */

	fLitterMinerR = pCL->fLitterMinerMaxR * corr.Feucht * corr.Temp;
    fLitterMinerR = fLitterMinerR * pCL->fCMicBiomSlow/((float)20.0 + pCL->fCMicBiomSlow);
    fManureMinerR = pCL->fManureMinerMaxR * corr.Feucht * corr.Temp;
    fManureMinerR = fManureMinerR * pCL->fCMicBiomSlow/((float)20.0 + pCL->fCMicBiomSlow);

	//logData2(fLitterMinerR,fManureMinerR);
	
	
	/******************************************************************/
    /*    C und N Mineralisierungsraten (dC/dt  und dN/dt)            */
    /******************************************************************/

    fCHumusDecayR   = pSL->fCHumus  * pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
    fCLitterDecayR  = pCL->fCLitter * fLitterMinerR;
    fCManureDecayR  = pCL->fCManure * fManureMinerR;
    fCBiomDecayR    = pCL->fCMicBiomSlow * fLitterMinerR;

    fNHumusDecayR   = pSL->fNHumus  * pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
    fNLitterDecayR  = pCL->fNLitter * fLitterMinerR;
    fNManureDecayR  = pCL->fNManure * fManureMinerR;	
    fNBiomDecayR    = pCL->fNMicBiomSlow * fLitterMinerR;
	/* Mineralisationsraten Ende */

            
    /******************************************************************/
    /* Entscheidung Immobilisierung/Mineralisierung beim Litter u.    */
    /* Manure Abbau. Der Wert h�ngt ab vom CN Wert der abgebauten     */
    /* Substanz, dem CN Wert der aufgebauten Substanz (Mikrobielle    */ 
    /* Biomasse und Humus) sowie dem Effektivit�tsfaktor mit der die  */ 
    /* Mikros Kohlenstoff verwerten k�nnen.                           */
    /* Hilfsvariable positiv = Nettomineralisierung                   */
    /* Hilfsvariable negativ = Nettoimmobilisierung                   */                            
    /******************************************************************/

	/* 1. Lokal ver�nderbare Effektivit�tsfaktoren */
	fEff    = pPA->fMinerEffFac;
    fHum    = pPA->fMinerHumFac;

    /* 2. CN-Werte f�r Litter und Manure */
    pCL->fLitterCN   = (pCL->fNLitter > EPSILON)?
    	pCL->fCLitter/pCL->fNLitter
    	:(float)0.1; 
            
    pCL->fManureCN   = (pCL->fNManure > EPSILON)?
    	pCL->fCManure / pCL->fNManure
    	:(float)0.1; 

	help1 = pCL->fMicBiomCN;
    pCL->fMicBiomCN   = (pCL->fNMicBiomSlow > EPSILON)?
    	pCL->fCMicBiomSlow / pCL->fNMicBiomSlow
    	:(float)help1; 




    /* 3. Mineralisierungs bzw. Immobilisierungsfaktoren*/
    if (pCL->fLitterCN > (float)0.1)
	{
	 h1            = (float)1 / pCL->fLitterCN - fEff / pCL->fMicBiomCN;
     fLitterToNH4R = (h1 > 0)? h1 : 0;
     fLitterImmR   = (h1 < 0)? (float)-1 * h1 : 0;
	}

	else
    {
     fLitterToNH4R = (float)0;
     fLitterImmR   = (float)0;
    }

	if (pCL->fManureCN > (float)0.1)
    {
     h2     = (float)1 / pCL->fManureCN - fEff / pCL->fMicBiomCN;
     fManureToNH4R = (h2 > 0)? h2 : 0;
     fManureImmR   = (h2 < 0)? (float)-1 * h2 : 0;
    }
  
    else
    {
     fManureToNH4R = (float)0;
     fManureImmR   = (float)0;
    }
    /* Entscheidung Immobilisierung/Mineralisierung Ende*/


    /******************************************************************/
    /* Beschr�nkung des C-Abbaus von Litter und Manure, wenn die      */
    /* Immobilisierung mehr N-Bedarf entwickelt als mineralischer N   */
    /* zur Verf�gung steht.                                           */
    /******************************************************************/

    /* 1. Gesamt N Bedarf */
    fNImmR = fLitterImmR * fCLitterDecayR + fManureImmR * fCManureDecayR;

    /* 2. Maximale Immobilisierungsraten (10% des vorhandenen Nmin)*/
    fNH4ImmR = fNH4ImmMaxR * pCL->fNH4N;
    fNO3ImmR = fNO3ImmMaxR * pCL->fNO3N;
 
    /* 3. Berechnung des Reduktionsfaktors*/  	
    fRed = (float)1.0;           /* Reduktionsfaktor = 1 => keine Reduktion */

    if (fNImmR  > (fNH4ImmR + fNO3ImmR))
     fRed = (fNH4ImmR + fNO3ImmR) / fNImmR;

    if (fLitterImmR)
    {   
     fCLitterDecayR = fRed * fCLitterDecayR;
     fNLitterDecayR = fRed * fNLitterDecayR;
    }

    if (fManureImmR)
    {
     fCManureDecayR = fRed * fCManureDecayR;
     fNManureDecayR = fRed * fNManureDecayR;
    }

    fNImmR = fRed * fNImmR;
    /* Berechnung Reduktion des Abbaus Ende */

    fLitAnt = ((fLitterImmR > (float)0.0)||(fManureImmR > (float)0.0))?
              fLitterImmR / (fLitterImmR + fManureImmR)
              : (float)0;


    /********************************************************************************/
    /*                  Veraenderung des C-Pools pro Zeitschritt                    */
    /********************************************************************************/
 
	/* 1. Gesamt C vor dem Zeitschritt */
	fCTotal0 = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C + pCL->fCMicBiomSlow;

    /* 2. Abbau des C-Litter Pools pro Zeitschritt */
    fCLitterToHumusR  = fCLitterDecayR * fEff * fHum;
    fCLitterToCO2R    = fCLitterDecayR * ((float)1 - fEff);
    fCLitterToBiomR   = fCLitterDecayR * fEff * ((float)1 - fHum);
    
    /* 3. Abbau des C-Manure Pools pro Zeitschritt */
    fCManureToHumusR  = fCManureDecayR * fEff * fHum;
    fCManureToCO2R    = fCManureDecayR * ((float)1 - fEff);
    fCManureToBiomR   = fCManureDecayR * fEff * ((float)1 - fHum);

    /* 4. Abbau des C-Humus Pools pro Zeitschritt */
    fCHumusToCO2R     = fCHumusDecayR * ((float)1 - fEff);
    fCHumusToBiomR    = fCHumusDecayR * fEff;

    /* 5. Abbau des C-MicBiom Pools pro Zeitschritt */
    fCBiomToHumusR  = fCBiomDecayR * fEff * fHum;
    fCBiomToCO2R    = fCBiomDecayR * ((float)1 - fEff);
    fCBiomToBiomR   = fCBiomDecayR * fEff * ((float)1 - fHum);

	/* 6. Zunahme im CO2 Pool */
    pCL->fCO2ProdR = fCLitterToCO2R +  fCManureToCO2R + fCHumusToCO2R + fCBiomToCO2R;
 
    /* 7. Veraenderung in den C-Pools */
    pCL->fCLitter -= fCLitterDecayR * DeltaT;
    pCL->fCManure -= fCManureDecayR * DeltaT;
	pSL->fCHumus  += (fCLitterToHumusR + fCManureToHumusR + fCBiomToHumusR - fCHumusToCO2R - fCHumusToBiomR)* DeltaT;
    pCL->fCMicBiomSlow += (fCLitterToBiomR + fCManureToBiomR + fCHumusToBiomR + fCBiomToBiomR - fCBiomDecayR) * DeltaT;
	pCL->fCO2C    += pCL->fCO2ProdR * DeltaT;
    
	pCL->fCSoilMicBiom = pCL->fCMicBiomSlow;
	/* Ver�nderungen im C-Pool Ende */


    /********************************************************************************/
    /*      Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt          */
    /********************************************************************************/

	/* 1. Gesamt N vor dem Zeitschritt */
    fNTotal0 = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N + pCL->fNMicBiomSlow;
			
	/* 2. Immobilisierung in den Biomasse-Pool */
    fNH4ToBiomR = fNImmR * RelAnteil(pCL->fNH4N,pCL->fNO3N);
    fNO3ToBiomR = fNImmR * RelAnteil(pCL->fNO3N,pCL->fNH4N);
    fNManureToBiomR = fCManureDecayR * fEff * ((float)1 - fHum)/ pCL->fMicBiomCN;
    fNBiomToBiomR   = fCBiomDecayR * fEff * ((float)1 - fHum) / pCL->fMicBiomCN;
    fNHumusToBiomR  = fCHumusDecayR * fEff / pCL->fMicBiomCN;
    fNLitterToBiomR = fCLitterDecayR * fEff * ((float)1 - fHum) / pCL->fMicBiomCN;
    
	/* 3. Humifizierung */
    fNLitterToHumusR  = fCLitterDecayR * fEff * fHum / pCL->fMicBiomCN;
    fNManureToHumusR  = fCManureDecayR * fEff * fHum/ pCL->fMicBiomCN;
    fNBiomToHumusR  = fCBiomDecayR * fEff * fHum / pCL->fMicBiomCN;

    /* 4. Ammonium-Mineralisierung aus Humus, Litter und Manure */
	fNHumusToNH4R  = fNHumusDecayR * ((float)1 - fEff); 
    fNBiomToNH4R   = fNBiomDecayR  * ((float)1 - fEff);
	fNLitterToNH4R = fLitterToNH4R * fCLitterDecayR;
    fNManureToNH4R = fManureToNH4R * fCManureDecayR;

	/* 5. Ver�nderungen in den N Pools */
    pCL->fNH4N += (fNHumusToNH4R  + fNLitterToNH4R  + fNManureToNH4R + fNBiomToNH4R - fNH4ToBiomR) * DeltaT;
    pCL->fNO3N -= fNO3ToBiomR * DeltaT;
    pCL->fNLitter -= fNLitterDecayR * DeltaT;
    pCL->fNManure -= fNManureDecayR * DeltaT;
    pCL->fNMicBiomSlow += (fNLitterToBiomR + fNManureToBiomR + fNHumusToBiomR - fNBiomToNH4R - fNBiomToHumusR) * DeltaT;
	pSL->fNHumus += (fNLitterToHumusR + fNManureToHumusR + fNBiomToHumusR - fNHumusToNH4R - fNHumusToBiomR) * DeltaT;
    pCL->fNSoilMicBiom = pCL->fNMicBiomSlow;
	/* Ver�nderungen im N-Pool Ende */

    /* 6 �bertragen auf globale Variablen */
	pCL->fNLitterImmobR = fNImmR * fLitAnt;
   	pCL->fNManureImmobR = fNImmR * ((float)1.0 - fLitAnt);
	pCL->fNImmobR = pCL->fNLitterImmobR + pCL->fNManureImmobR;
    pCL->fHumusMinerR = fNHumusToNH4R;
    pCL->fLitterMinerR = fNLitterToNH4R + fNBiomToNH4R;
    pCL->fManureMinerR = fNManureToNH4R;
    pCL->fMinerR = pCL->fHumusMinerR + pCL->fLitterMinerR + pCL->fManureMinerR;
    /********************************************************************************/
    /* Massenbilanz, die Summe der Ver�nderungen muss ungef�hr Null ergeben         */
    /********************************************************************************/
 
    fCTotal1 = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C + pCL->fCMicBiomSlow;
    fCDiff   = fCTotal1 - fCTotal0;    

    fNTotal1 = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N + pCL->fNMicBiomSlow;
    fNDiff   = fNTotal1 - fNTotal0;    

    fCDiffSum    += fCDiff;
    fNDiffSum    += fNDiff;
//    logData2(fCDiffSum,fNDiffSum);
	/* Massenbilanz Ende */

   }    /* Schichtweise */

  return 0;
}                    

/*********************************************************************************************/
/* Procedur    :   NITS (Nitrate Integrated Transformation component for SHETRAN             */
/* Beschreibung:   Mineralisation                                                            */
/*                 Methode Grundlage SOILN/WAVE/LEACHN mit Ver�nderungenung                  */
/*                 Quelle: Birkinshaw & Ewen (2000) J. of Hydrology 230: 1-17                */
/*                                                                                           */
/*              GSF/ab                  18.04.01                                             */
/*                                                                                           */
/*********************************************************************************************/
/* ver�nd. Var.		pCL->fCLitter                  pCL->fNLitter                             */
/*					pCL->fCManure                  pCL->fNManure                             */
/*					pCL->fCHumus                   pSL->fNHumus                              */
/*					pCL->fCO2C                     pCL->fNH4N                                */
/*					pCL->fCO2ProdR                 pCL->fNO3N                                */
/*                  pCL->fHumusMinerR              pCL->fImmobR                              */
/*					pCL->fLitterMinerR             corr.Temp                                 */
/*                  pCL->fManureMinerR             corr.Feucht                               */
/*					pCL->fMinerR                   pCL->fNLitterImmobR                       */
/*********************************************************************************************/
int WINAPI MinerNITS(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr corr={(float)1}; 
 
  /******************************************************************/
  /*           Deklaration lokaler Variablen                        */
  /******************************************************************/

  /* 1.Variablen f�r die Reduktionsfunktionen*/
  float wmin,wlow,whigh,wmax,f1;
 
  /* 2. Variablen f�r die C-Fl�sse */
  float fCHumusDecayR,fCLitterDecayR,fCManureDecayR;
  float fCLitterToHumusR,fCLitterToBiomR,fCLitterToCO2R;
  float fCManureToBiomR,fCManureToCO2R;
  float fCHumusToBiomR,fCHumusToCO2R;  

  /* 3. Variablen f�r die N-Fl�sse*/
  float fNHumusDecayR,fNLitterDecayR,fNManureDecayR;

  /* 4. Variablen f�r die Aufteilung der Stofffl�sse*/  
  float fEff,fHum;
  
  /* 5. Variablen f�r die Berechnung der N-Mineralisation/N-Immobilisierung*/
  float gamma;
  float fHumusCN,fMicBiomCN;
  float fNMinR,fNImmR,fRed;
  float fNH4ImmMaxK = (float)0.15;
  float fNO3ImmMaxK = (float)0.15;
  float fNH4ImmMaxR,fNO3ImmMaxR;
  float fNH4ImmR,fNO3ImmR;

  /*Variablen f�r die Massenbilanz*/
  float fCTotal0,fNTotal0;
  float fCTotal1,fNTotal1;
  float fCDiff,fNDiff;

  /*Parameter f�r die �berpr�fung*/
  static float fCDiffSum   = (float)0.0;
  static float fNDiffSum   = (float)0.0;


  N_ZERO_LAYER            //Setzen der Pointer auf die nullte Schicht 
   
  SurfaceMiner(exp_p);  //Aufruf der Oberfl�chenmineralisierung

  for (N_SOIL_LAYERS)    //schichtweise Berechnung
   {
	/******************************************************************/
    /*                          Reduktionsfunktionen                  */
    /******************************************************************/
 
    /* 1. Temperatur-Reduktionsfunktion, Q10 Funktion*/
    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
    
	/* 2. Feuchte-Reduktionsfunktion, Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (float)-31622770;    //pF = 6.5 Orig.: -10000000.0 pF = 6.0 
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160;        //pF = 2.5 Orig.: -3000.0;    pF = 2.48
    wlow = WATER_CONTENT(f1);
    f1= (float)-316;         //pF = 1.5 Orig.: -600.0,     pF = 1.78
    whigh = WATER_CONTENT(f1);
    f1= (float)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							wmax, pPA->fMinerSatActiv);
    /* Reduktionsfunktion Ende */


	/******************************************************************/
    /*    C und N Mineralisierungsraten (dC/dt  und dN/dt)            */
    /******************************************************************/
	
	/* Lokal ver�nderbare Effektivit�tsfaktoren */
	fEff       = pPA->fMinerEffFac; // Original = 0.5
    fHum       = pPA->fMinerHumFac; // Original = 0.2
    fHumusCN   = pSL->fHumusCN;     // Original = 12.0
    fMicBiomCN = pCL->fMicBiomCN;   // Original = 8.0
	
	/* Der N Humus-Pool wird  nicht mit einer Differentialgleichung
	   gerechnet, sondern es wird angenommen, dass der Humus CN Wert
	   konstant bleibt. Je nach gew�hltem CN Wert muss der N Humus Gehalt
	   hier neu berechent werden. */
	pSL->fNHumus = pSL->fCHumus / fHumusCN;

    fCHumusDecayR   = pSL->fCHumus  * pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
    fCLitterDecayR  = pCL->fCLitter * pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht;
    fCManureDecayR  = pCL->fCManure * pCL->fManureMinerMaxR * corr.Temp * corr.Feucht;
    fNHumusDecayR   = pSL->fNHumus  * pCL->fHumusMinerMaxR  * corr.Temp * corr.Feucht;
	fNLitterDecayR  = pCL->fNLitter * pCL->fLitterMinerMaxR * corr.Temp * corr.Feucht;
	fNManureDecayR  = pCL->fNManure * pCL->fManureMinerMaxR * corr.Temp * corr.Feucht;
    /* Mineralisationsraten Ende */

            
    /*************************************************************************/
    /*  Berechnung der Ammonium Bilanz beim Litter u. Manure Abbau           */
    /*  Hilfsvariable positiv = Nettomineralisierung                         */
    /*  Hilfsvariable negativ = Nettoimmobilisierung                         */
    /*************************************************************************/	

    /* Netto-Stickstoff-Fluss */ 
	gamma = fNLitterDecayR + fNManureDecayR + fNHumusDecayR -
            fCLitterDecayR * ((float)1.0 - fEff) * fHum / fHumusCN -      
	        fCLitterDecayR * fEff / fMicBiomCN -
            fCHumusDecayR  * fEff / fMicBiomCN -
        	fCManureDecayR * fEff / fMicBiomCN;


    fNMinR  = (gamma > 0)? gamma : 0;
    fNImmR  = (gamma < 0)? (float)-1 * gamma : 0;
    /* Entscheidung Immobilisierung/Mineralisierung Ende*/


    /******************************************************************/
    /* Beschr�nkung des C-Abbaus von Litter und Manure, wenn die      */
    /* Immobilisierung mehr N-Bedarf entwickelt als mineralisches N   */
    /* zur Verf�gung steht.                                           */
    /******************************************************************/

	/* 1. Maximale Immobilisierungsraten*/
   	fNH4ImmMaxR = fNH4ImmMaxK * pCL->fNH4N;
    fNO3ImmMaxR = fNO3ImmMaxK * pCL->fNO3N;

    /* 2. Berechnung des Reduktionsfaktors*/  	
    fRed = (float)1.0;                      /* Reduktionsfaktor =1 => keine Reduktion */


	if (fNImmR  > (fNH4ImmMaxR + fNO3ImmMaxR))
          fRed = (fNH4ImmMaxR + fNO3ImmMaxR) / fNImmR;

    /* 3. Reduktion der Abbauraten */
    fCLitterDecayR = fRed * fCLitterDecayR;
    fCManureDecayR = fRed * fCManureDecayR;
	fCHumusDecayR  = fRed * fCHumusDecayR;
    fNLitterDecayR = fRed * fNLitterDecayR;
    fNManureDecayR = fRed * fNManureDecayR;
	fNHumusDecayR  = fRed * fNHumusDecayR;
	fNImmR         = fRed * fNImmR;


   /* 4. Ammonium wird bevorzugt eingebaut*/
	fNH4ImmR = min(fNImmR,fNH4ImmMaxR);         
	fNO3ImmR = min(fNImmR-fNH4ImmR,fNO3ImmMaxR);




	/********************************************************************************/
    /*                  Veraenderung des C-Pools pro Zeitschritt                    */
    /********************************************************************************/
	
	/* 1. Gesamt C vor dem Zeitschritt */
    fCTotal0 = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C;
    
	/* 2. Abbau des Litter Pools pro Zeitschritt */
    fCLitterToHumusR = fCLitterDecayR * ((float)1.0 - fEff) * fHum;
    fCLitterToCO2R   = fCLitterDecayR * ((float)1.0 - fEff) * ((float)1.0 - fHum);
    fCLitterToBiomR  = fCLitterDecayR * fEff;
      
    /* 3. Abbau des Manure Pools pro Zeitschritt */
    fCManureToBiomR  = fCManureDecayR * fEff;
    fCManureToCO2R   = fCManureDecayR * ((float)1.0 - fEff);

    /* 3. Abbau des Humus Pools pro Zeitschritt */
    fCHumusToCO2R    = fCHumusDecayR * ((float)1.0 - fEff);
    fCHumusToBiomR   = fCHumusDecayR * fEff;


	/* 4. Veraenderung der C-Pools pro Zeitschritt */
	pCL->fCLitter += (fCLitterToBiomR + fCManureToBiomR + fCHumusToBiomR - fCLitterDecayR)* DeltaT;
    pCL->fCManure -= fCManureDecayR * DeltaT;
    pSL->fCHumus  += (fCLitterToHumusR - fCHumusDecayR) * DeltaT;

    /* 5. CO2-C Zuwachs */
    pCL->fCO2ProdR = fCLitterToCO2R + fCManureToCO2R + fCHumusToCO2R;
    pCL->fCO2C += pCL->fCO2ProdR * DeltaT;
    /* Veraenderung C-Pools Ende */


    /********************************************************************************/
    /*      Veraenderung der N-Pools pro Zeitschritt                                */
    /********************************************************************************/

    /* 1. Gesamt N vor dem Zeitschritt */
	fNTotal0 = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N;
   
    /* 2. Ver�nderung im Ammonium und Nitrat Pool */
    if (fNMinR > 0) //Mineralisierung
     
      pCL->fNH4N += fNMinR * DeltaT;  
	

    else             //Immobilisierung
     { 
      pCL->fNH4N -= fNH4ImmR * DeltaT;  
      pCL->fNO3N -= fNO3ImmR * DeltaT;  	
	 }

    /* 3. Ver�nderung im Litter Pool pro Zeitschritt */
    pCL->fNLitter -= (fNLitterDecayR -
	                 fEff * fCLitterDecayR / fMicBiomCN -
					 fEff * fCHumusDecayR  / fMicBiomCN -
					 fEff * fCManureDecayR / fMicBiomCN) * DeltaT;

    /* 4. Ver�nderung im Manure Pool pro Zeitschritt */
    pCL->fNManure -= fNManureDecayR * DeltaT;

    /* 5. Ver�nderung im Humus Pool pro Zeitschritt */
	pSL->fNHumus = pSL->fCHumus / fHumusCN;       
    /* Ver�nderungen in den N-Pools Ende */



    /********************************************************************************/
    /* �bertragen auf globale Variablen, wichtig f�r die Bilanz                     */
	/* und den grafischen Output                                                    */
    /********************************************************************************/
	    
	pCL->fMinerR = fNMinR;
	pCL->fHumusMinerR = fCHumusToCO2R / pSL->fHumusCN; 
	pCL->fLitterMinerR = pCL->fMinerR - pCL->fHumusMinerR;
	pCL->fManureMinerR = (float)0.0;

	pCL->fNImmobR = fNImmR;
	pCL->fNLitterImmobR = fNImmR;


    /********************************************************************************/
    /* Massenbilanz, die Summe der Ver�nderungen muss ungef�hr Null ergeben          */
    /********************************************************************************/

    fCTotal1 = pSL->fCHumus + pCL->fCLitter + pCL->fCManure + pCL->fCO2C;
    fCDiff   = fCTotal1 - fCTotal0;    

    fNTotal1 = pSL->fNHumus + pCL->fNLitter + pCL->fNManure + pCL->fNH4N + pCL->fNO3N;
    fNDiff   = fNTotal1 - fNTotal0;    
    /*Massenbilanz Ende */

	/*F�r die �berpr�fung */
    fCDiffSum    += fCDiff;
    fNDiffSum    += fNDiff;
//logData2(pTi->pSimTime->fTimeAct,fNDiffSum);

   }    /* Schichtweise */

  return 0;
}                    



/********************************************************************************************/
/* Procedur    :   XNCERES                                                                  */
/* Beschreibung:   Mineralisation/Immobilisation                                            */
/*                 Methode CERES                                                            */
/*                 Quelle: Engel et al.(1983) Simulationsmodelle zur N-Dynamik              */
/*                         source-code von T. Schaaf                                        */
/*                         Hanks & Ritchie (1991) Modeling Plants and Soil Systems          */
/*                                                                                          */
/*                 GSF/ab                  03.05.01                                         */
/*                                                                                          */
/********************************************************************************************/
/* ver�nd. Var.		pCL->fCFOMFast              pCL->fNFOMFast                              */
/*					pCL->fCFOMSlow              pCL->fNFOMSlow                              */
/*					pCL->fCFOMVeryFast          pCL->fNFOMVeryFast                          */
/*					pCL->fCLitter               pCL->fNLitter                               */
/*					pCL->fCManure               pCL->fNManure                               */
/*					pSL->fCHumus                pSL->fNHumus                                */
/*					pCL->fCO2C                  pCL->fNO3N                                  */
/*					pCL->fProdCO2R              pCL->fNH4N                                  */
/*                  pCL->fLitterMinerR          pCL->fNImmobR                               */
/*                  pCL->fHumusMinerR           pCL->fManureMinerR                          */
/*                  pCL->fMinerR                pCL->fCNCoeff                               */
/*					pCL->fFOMSlowDecMaxR        corr.Feucht                                 */
/*   				pCL->fFOMFastDecMaxR        corr.Temp                                   */
/*   				pCL->fFOMVeryFastDecMaxR                                                */ 
/********************************************************************************************/
int WINAPI MinerXNCERES(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr  corr = {(float)1}; 
  
  /******************************************************************/
  /*           Deklaration lokaler Variablen                        */
  /******************************************************************/

  /* 1. Reduktionsparameter */
  float wmin,wlow,whigh,wmax,f1,cn1,fRed,fNToNH4R;

  /* 2. Variablen f�r die C-Fl�sse */
  float fCAOM1DecayR,fCAOM2DecayR,fCAOM3DecayR,fCHumusDecayR;  
  float fRCAC1,fRCAC2,fRCAC3;  
  float fCAOMToCO2R,fCAOMToHumR,fCHumToCO2R;
  float	fCFreeR1,fCFreeR2,fCFreeR3,fCFreeTotalR;

  /* 3. Variablen f�r die N-Fl�sse */
  float fNAOM1DecayR,fNAOM2DecayR,fNAOM3DecayR,fNHumusDecayR;
  float fRNAC1,fRNAC2,fRNAC3;
  float fNAOMToNH4R, fNAOMToHumR, fNHumToNH4R;
  float fNMinR, fNImmR;
  float fNH4ImmR,fNO3ImmR;
  float fNH4ImmMaxR = (float)0.1;
  float fNO3ImmMaxR = (float)0.1;
  float defizit,help,fNNetto1,fNNetto2,fNNetto3;
  float fLigninMinerR,fCelluloseMinerR,fKohlenhydrateMinerR;
  float fLigninImmobR,fCelluloseImmobR,fKohlenhydrateImmobR;

  /* 5. Variablen f�r die Massenbilanz */
  float fCTotal0,fNTotal0;                     
  float fCTotal1,fNTotal1;		       		   
  float fCDiff,fNDiff;      

  /* 6. Variablen f�r die Anteilsberechnung bzw. Initialisierung*/
  float fCFastOld,fCFast,fNFastOld,fNFast;
  float AnteilCFOMSlow,AnteilCFOMFast,AnteilCFOMVeryFast;
  float AnteilNFOMSlow,AnteilNFOMFast,AnteilNFOMVeryFast;
  float CAnteilLitter,CAnteilManure,NAnteilLitter,NAnteilManure;
  /*Variablendeklaration Ende*/

  N_ZERO_LAYER           // Setzen der Pointer auf die nullte Schicht
   
  SurfaceMiner(exp_p); // Aufruf der Oberfl�chenmineralisierung

  for (N_SOIL_LAYERS)    // Schichtweise Berechnung 
  {
   /* Anfangsinitialisierung */ 
   if (SimStart(pTi))
    {
     pCL->fCFOMSlow     = (float)0.1 * pCL->fCLitter; // 10 % Lignin
	 pCL->fCFOMFast     = (float)0.7 * pCL->fCLitter; // 70 % Cellulose
	 pCL->fCFOMVeryFast = (float)0.2 * pCL->fCLitter; // 20 % Kohlenhydrate
	
     pCL->fNFOMSlow     = (float)0.1 * pCL->fNLitter; // 10 % Lignin
	 pCL->fNFOMFast     = (float)0.7 * pCL->fNLitter; // 70 % Cellulose
	 pCL->fNFOMVeryFast = (float)0.2 * pCL->fNLitter; // 20 % Kohlenhydrate
	}

   	/* 2. Kohlenstoff */
   fCFastOld = pCL->fCFOMSlow + pCL->fCFOMFast +  pCL->fCFOMVeryFast;
  
    if(fCFastOld)
	 {
	  AnteilCFOMSlow     = pCL->fCFOMSlow     / fCFastOld;
      AnteilCFOMFast     = pCL->fCFOMFast     / fCFastOld;
      AnteilCFOMVeryFast = pCL->fCFOMVeryFast / fCFastOld;
     
      pCL->fCFOMSlow     = AnteilCFOMSlow     * (pCL->fCLitter + pCL->fCManure);
      pCL->fCFOMFast     = AnteilCFOMFast     * (pCL->fCLitter + pCL->fCManure);
      pCL->fCFOMVeryFast = AnteilCFOMVeryFast * (pCL->fCLitter + pCL->fCManure);
  	 }

    else
     {
      pCL->fCFOMSlow     = (float)0.0;
      pCL->fCFOMFast     = (float)0.0;
      pCL->fCFOMVeryFast = (float)0.0;
	 }

	/* 3. Stickstoff */
	fNFastOld = pCL->fNFOMSlow + pCL->fNFOMFast +  pCL->fNFOMVeryFast;

    if(fNFastOld > 0.11)
	 {
	  AnteilNFOMSlow     = pCL->fNFOMSlow     / fNFastOld;
      AnteilNFOMFast     = pCL->fNFOMFast     / fNFastOld;
      AnteilNFOMVeryFast = pCL->fNFOMVeryFast / fNFastOld;
     
      pCL->fNFOMSlow     = AnteilNFOMSlow     * (pCL->fNLitter + pCL->fNManure);
      pCL->fNFOMFast     = AnteilNFOMFast     * (pCL->fNLitter + pCL->fNManure);
      pCL->fNFOMVeryFast = AnteilNFOMVeryFast * (pCL->fNLitter + pCL->fNManure);
  	 }

    else
     {
      pCL->fNFOMSlow     = (float)0.0;
      pCL->fNFOMFast     = (float)0.0;
      pCL->fNFOMVeryFast = (float)0.0;
	 }


    /******************************************************************/
    /*                          Reduktionsfunktionen                  */
    /******************************************************************/
    
	/* 1. Temperatur-Reduktionsfunktion, Q10 Funktion*/
    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
    
	/* 2. Feuchte-Reduktionsfunktion, Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (float)-31622770;    //pF = 6.5 Orig.: -10000000.0 pF = 6.0 
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160;        //pF = 2.5 Orig.: -3000.0;    pF = 2.48
    wlow = WATER_CONTENT(f1);
    f1= (float)-316;         //pF = 1.5 Orig.: -600.0,     pF = 1.78
    whigh = WATER_CONTENT(f1);
    f1= (float)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							wmax, pPA->fMinerSatActiv);


	/* 3. C/N Verh�ltnisse, Ansatz DNDC */
    cn1 = pCL->fCLitter / (pCL->fNLitter + pCL->fNH4N + pCL->fNO3N);
    pCL->fCNCoeff = min(1,(float)0.2 + (float)7.2 / cn1);
    
   /* Original Funktionen nach CERES
    1. Temperatur: Ansatz linear, Bezugswert: Tagesmittelwert der jeweiligen Simulationsschicht
    if (pHL->fSoilTemp < 0)
	  corr.Temp = (float)0.0;

    else if (pHL->fSoilTemp > 30.0)
	  corr.Temp = (float)1.0;

      else
	    corr.Temp = pHL->fSoilTemp / (float)30.0;

	/* 2. Feuchte: Ansatz Polygonzug, Bezugswerte PWP, FK und S�ttigungspunkt
    wmin  = (float)0.0;
    wlow  = (float)0.5 * pSW->fContPWP;
    whigh = pSW->fContFK;
	wmax  = pSW->fContSatCER;

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)0 , whigh, (float)1, 
							wmax, (float)0.5);
    

	/* 3. C/N Verh�ltnisse 
    cn1 = pCL->fCLitter / (pCL->fNLitter + pCL->fNH4N + pCL->fNO3N);
	cn2 = (float)exp((double)( ((float)-0.693 * (cn1 - (float)25.0)) / (float)25.0));
		
	if (cn2 > (float)1.0)
    	cn2 = (float)1.0;

    pCL->fCNCoeff = cn2;*/
    /* Reduktionsfunktion Ende */
	

	/******************************************************************/
    /*    C und N Mineralisierungsraten (dC/dt  und dN/dt)            */
    /******************************************************************/
		
	/* 1. lokal ver�nderbare spez. Abbaukonstanten */
    pCL->fFOMVeryFastDecMaxR  = pCL->afFOMDecR[0];
    pCL->fFOMFastDecMaxR      = pCL->afFOMDecR[1];
    pCL->fFOMSlowDecMaxR      = pCL->afFOMDecR[2];
  
    /* 2. Kohlenstoff-Mineralisierungs-Raten (dC/dt) */
    fCAOM1DecayR  = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR     * corr.Temp * corr.Feucht * pCL->fCNCoeff;
    fCAOM2DecayR  = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR     * corr.Temp * corr.Feucht * pCL->fCNCoeff;
    fCAOM3DecayR  = pCL->fCFOMVeryFast * pCL->fFOMVeryFastDecMaxR * corr.Temp * corr.Feucht * pCL->fCNCoeff;
    fCHumusDecayR = pSL->fCHumus       * pCL->fHumusMinerMaxR     * corr.Temp * corr.Feucht; 

    /* 3. Stickstoff-Mineralisierungs-Raten (dN/dt) */
    fNAOM1DecayR  = pCL->fNFOMSlow     * pCL->fFOMSlowDecMaxR      * corr.Temp * corr.Feucht * pCL->fCNCoeff;
    fNAOM2DecayR  = pCL->fNFOMFast     * pCL->fFOMFastDecMaxR      * corr.Temp * corr.Feucht * pCL->fCNCoeff;
    fNAOM3DecayR  = pCL->fNFOMVeryFast * pCL->fFOMVeryFastDecMaxR  * corr.Temp * corr.Feucht * pCL->fCNCoeff;
    fNHumusDecayR = pSL->fNHumus       * pCL->fHumusMinerMaxR      * corr.Temp * corr.Feucht; 
	/* Mineralisationsraten Ende */


    /******************************************************************/
    /* Entscheidung Immobilisierung/Mineralisierung                   */
    /******************************************************************/

    /* 1. Bruttoassimilation */
	fRCAC1  = (float) 0.4  * fCAOM1DecayR;
	fRNAC1  = (float) 0.06 * fCAOM1DecayR; 

	fRCAC2  = (float) 0.4  * fCAOM2DecayR;
	fRNAC2  = (float) 0.06 * fCAOM2DecayR; 
	
	fRCAC3  = (float) 0.4  * fCAOM3DecayR;
	fRNAC3  = (float) 0.06 * fCAOM3DecayR; 

	/* 2. Nettofreisetzung bzw. -assimilation */
	fCFreeR1 = fCAOM1DecayR - fRCAC1;
    fNNetto1 = fNAOM1DecayR - fRNAC1;

    help = fNNetto1;
    fLigninMinerR = (help > 0) ? help : 0;
    fLigninImmobR = (help < 0) ? (float)-1 * help : 0;

	fCFreeR2 = fCAOM2DecayR - fRCAC2;
    fNNetto2 = fNAOM2DecayR - fRNAC2;

    help = fNNetto2;
    fCelluloseMinerR = (help > 0) ? help : 0;
    fCelluloseImmobR = (help < 0) ? (float)-1 * help : 0;

	fCFreeR3 = fCAOM3DecayR - fRCAC3;
    fNNetto3 = fNAOM3DecayR - fRNAC3;

    help = fNNetto3;
    fKohlenhydrateMinerR = (help > 0) ? help : 0;
    fKohlenhydrateImmobR = (help < 0) ? (float)-1 * help : 0;
	

    /******************************************************************/
    /* Beschr�nkung des C-Abbaus, wenn die Immobilisierung mehr       */
	/* N-Bedarf entwickelt als mineralischer N zur Verf�gung steht.   */
    /******************************************************************/

	/* 1. Gesamt N Bedarf */
	fNImmR = fLigninImmobR + fCelluloseImmobR + fKohlenhydrateImmobR;
	
	/* 2. Maximale Immobilisierungsraten (10% des vorhandenen Nmin)*/
    fNH4ImmR = fNH4ImmMaxR * pCL->fNH4N;
    fNO3ImmR = fNO3ImmMaxR * pCL->fNO3N;
 
    /* 3. Berechnung des Reduktionsfaktors*/  	
    fRed = (float)1.0;           /* Reduktionsfaktor = 1 => keine Reduktion */

    if (fNImmR  > (fNH4ImmR + fNO3ImmR))
     fRed = (fNH4ImmR + fNO3ImmR) / fNImmR;

	/* 4. Abfrage bei welcher Fraktion es zur Immobilisierung kommt */
    if (fLigninImmobR)
    {   
     fCFreeR1 = fRed * fCFreeR1;
     fLigninImmobR = fRed * fLigninImmobR;
    }

    if (fCelluloseImmobR)
    {   
     fCFreeR2         = fRed * fCFreeR2;
     fCelluloseImmobR = fRed * fCelluloseImmobR;
    }

    if (fKohlenhydrateImmobR)
    {   
     fCFreeR3         = fRed * fCFreeR3;
     fKohlenhydrateImmobR = fRed * fKohlenhydrateImmobR;
    }

	fNImmR = fRed * fNImmR;
    /* Berechnung Reduktion des Abbaus Ende */


    /********************************************************************************/
    /*                  Veraenderung des C-Pools pro Zeitschritt                    */
    /********************************************************************************/
 
	/* 1. Gesamt C vor dem Zeitschritt */
	fCTotal0 = pSL->fCHumus + pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCFOMVeryFast + pCL->fCO2C;
 	fNTotal0 = pSL->fNHumus + pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNFOMVeryFast + pCL->fNH4N + pCL->fNO3N;

    /* 2. Ver�nderung der Lignin-Fraktion pro Zeitschritt */
	pCL->fCFOMSlow -= fCFreeR1 * DeltaT;
	pCL->fNFOMSlow -= fLigninMinerR * DeltaT;
	pCL->fNFOMSlow += fLigninImmobR * DeltaT;

    /* 3. Ver�nderung der Cellulose-Fraktion pro Zeitschritt */
	pCL->fCFOMFast -= fCFreeR2 * DeltaT;
	pCL->fNFOMFast -= fCelluloseMinerR * DeltaT;
	pCL->fNFOMFast += fCelluloseImmobR * DeltaT;

    /* 4. Ver�nderung der Kohlenhydrate-Fraktion pro Zeitschritt */
	pCL->fCFOMVeryFast -= fCFreeR3 * DeltaT;
	pCL->fNFOMVeryFast -= fKohlenhydrateMinerR * DeltaT;
	pCL->fNFOMVeryFast += fKohlenhydrateImmobR * DeltaT;

	/* 5. Summe �ber die drei FOM-Fraktionen */
    fCFreeTotalR = fCFreeR1 + fCFreeR2 + fCFreeR3;
    fNMinR       = fLigninMinerR + fCelluloseMinerR + fKohlenhydrateMinerR;
	
	/* 6. FOS Abbau und Verteilung*/
	fNAOMToNH4R  = (float) 0.8 * fNMinR;
	fNAOMToHumR  = (float) 0.2 * fNMinR;
	fCAOMToHumR  = fNAOMToHumR / (float) 0.1;
	fCAOMToCO2R  = fCFreeTotalR - fCAOMToHumR;

    /* 7. Humus Verteilung*/	
    fCHumToCO2R =  fCHumusDecayR;
    fNHumToNH4R =  fNHumusDecayR;

    /* 8. Veraenderung im Humus-Pool*/
    pSL->fCHumus += (fCAOMToHumR - fCHumusDecayR) * DeltaT;
    pSL->fNHumus += (fNAOMToHumR - fNHumusDecayR) * DeltaT;

    /* 9. Veraenderung im CO2-Pool */
    pCL->fCO2ProdR   = fCAOMToCO2R + fCHumToCO2R;
    pCL->fCO2C   += pCL->fCO2ProdR * DeltaT;

	/* 10. Berechnung der Gesamtmineralisation bzw. Immobilisation
	       NETTOWERTE! (FOS + Humus) und �bertragen auf globale Variablen */
	if(fNMinR > fNImmR)
     {
      fNAOMToNH4R -= fNImmR;
	  fNToNH4R =  fNAOMToNH4R + fNHumToNH4R;
	  pCL->fLitterMinerR = fNAOMToNH4R;
      pCL->fHumusMinerR  = fNHumToNH4R;
	  pCL->fMinerR       = pCL->fLitterMinerR + pCL->fHumusMinerR;
	  pCL->fNImmobR      = (float)0.0;
     }
    else
	 {  
	  fNImmR -= fNMinR;
	
     if (fNImmR > fNHumToNH4R)
      {
	   fNImmR -= fNHumToNH4R;
       pCL->fNImmobR = fNImmR;
       pCL->fNLitterImmobR = fNImmR;
	   fNToNH4R           = (float)0.0;
	   pCL->fLitterMinerR = (float)0.0;
       pCL->fHumusMinerR  = (float)0.0;
       pCL->fMinerR       = pCL->fHumusMinerR + pCL->fLitterMinerR;
	  }
   
	 else
	  {
	   fNToNH4R = fNHumToNH4R - fNImmR;
       pCL->fHumusMinerR  = fNToNH4R;
       pCL->fLitterMinerR = (float)0.0;
       pCL->fMinerR       = pCL->fHumusMinerR + pCL->fLitterMinerR;
       pCL->fNImmobR      = (float)0.0;
	  } 
	 }
    /* 11. Ammonium und Nitrat*/
    if (fNToNH4R > 0)   //Mineralisierung
     pCL->fNH4N   += (fNToNH4R) * DeltaT;
 
    else                //Immobilisierung, erst NH4, dann NO3
	 {
	  pCL->fNH4N   -= (fNImmR) * DeltaT;
	
	  if (pCL->fNH4N < (float)0.1)
       {
	    defizit = (float)0.1 -  pCL->fNH4N;
	    pCL->fNO3N -= defizit;
	    if (pCL->fNO3N < (float)0.1)
	     {
	      pCL->fNO3N = (float)0.1;		 
	     }               
	    pCL->fNH4N = (float)0.1;
	   }
	 }


    /********************************************************************************/
    /* �bertragen der Ver�nderungen auf die Pools Litter und Manure                 */
    /********************************************************************************/
   
	/* 1. Kohlenstoff */ 
	fCFast = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCFOMVeryFast;
   
    if(pCL->fCLitter + pCL->fCManure)
	 {
	  CAnteilLitter  = pCL->fCLitter / (pCL->fCLitter + pCL->fCManure);
      CAnteilManure  = pCL->fCManure / (pCL->fCLitter + pCL->fCManure);
	 }
	
	else
	 {
	  CAnteilLitter = (float)1.0;
      CAnteilManure = (float)0.0;
	 }
	
	pCL->fCLitter = CAnteilLitter * fCFast;
    pCL->fCManure = CAnteilManure * fCFast;

    /* 2. Stickstoff */
    fNFast = pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNFOMVeryFast;
    
	if(pCL->fNLitter + pCL->fNManure)
	 {
	  NAnteilLitter  = pCL->fNLitter / (pCL->fNLitter + pCL->fNManure);
      NAnteilManure  = pCL->fNManure / (pCL->fNLitter + pCL->fNManure);
	 }

	else
	 {
	  NAnteilLitter = (float)1.0;
      NAnteilManure = (float)0.0;
	 }

    pCL->fNLitter = NAnteilLitter * fNFast;
    pCL->fNManure = NAnteilManure * fNFast;
    /* �bertragen auf ExpertN-Pools Ende */


    /********************************************************************************/
    /* Massenbilanz, die Summe der Ver�nderungen muss ungef�r Null ergeben          */
    /********************************************************************************/

    fCTotal1 = pCL->fCFOMSlow + pCL->fCFOMFast + pCL->fCFOMVeryFast + 
               pSL->fCHumus + pCL->fCO2C;

    fNTotal1 = pCL->fNFOMSlow + pCL->fNFOMFast + pCL->fNFOMVeryFast + 
               pSL->fNHumus + pCL->fNH4N + pCL->fNO3N;

    fCDiff = fCTotal1 - fCTotal0;
    fNDiff = fNTotal1 - fNTotal0;

	/* Massenbilanz Ende */

  }/*Ende Schichtweise Berechnung*/
  return 0;
}

/********************************************************************************************/
/* Procedur    :   MinerNCSOIL()                                                            */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Mineralisierung                                                          */
/*                 Methode NCSOIL (Molina et al 1983)                                       */
/*                                                                                          */
/*              GSF/ch                  1995                                                */
/*                  ab                  14.05.01 �berarbeitet                                                      */
/*                                                                                          */
/********************************************************************************************/
/* ver�nd. Var.		pCL->fCFOMFast        pCL->fNFOMFast                                    */
/*                  pCL->fCFOMSlow        pCL->fNOMSlow                                     */
/*					pCL->fCHumusFast      pCL->fNHumusFast                                  */
/*                  pCL->fCHumusSlow      pCL->fNHumusSlow                                  */
/*					pCL->fCHumusStable    pCL->fNHumusStable                                */
/*                  pCL->fCMicBiomFast    pCL->fNMicBiomFast                                */
/*                  pCL->fCMicBiomSlow    pCL->fNMicBiomSlow                                */
/*                  pCL->fCSoilMicBiom    pCL->fNSoilMicBiom                                */ 
/*					pCL->fLitterMinerR    pCL->fNO3N                                        */
/*                  pCL->fHumusMinerR     pCL->fNH4N                                        */
/*					pCL->fNImmobR         pCL->fCO2C                                        */
/*					pCL->fCsolC           pCL->fCO2ProdR                                    */
/*                  pCL->fMinerR	      pCL->fNLitterImmobR                               */
/********************************************************************************************/
int WINAPI MinerNCSOIL(EXP_POINTER)
{
  #define ANZ 2
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr  corr = {(float)1}; 

  /*Hilfsvariablen*/
  int  i1;
  float fCNrate,fRed,h1;
  float f1,wmin,wlow,whigh,wmax;

  /*Variablen f�r die N-Verf�gbarkeit*/
  float fNH4ImmR,fNO3ImmR;
  float fNH4ImmMaxK = (float)0.1; 
  float fNO3ImmMaxK = (float)0.1;
  float fAvailN;

  /*Variablen f�r die Residue-Schleife*/
  float afRes_DecayC[ANZ];
  float afCN_Res[ANZ];
  float fCwithNmin,fCwithNmin_Sum;
  float fNBedarf[ANZ];
  float fNBedarf_Total;
  float fCO2ProdR;


  /*Variablen f�r die Verteilung der Stofffl�sse */
  float fNImmobR,fNMinerR;
  float fNetMinR,fNetImmR;
  float fNH4NR,fNO3NR;


  /*Variablen f�r die C-Fl�sse*/
  float fPool0Fast_DecayC,fPool0Fast_DecayN; 
  float fPool0Slow_DecayC,fPool0Slow_DecayN;  
  float fPool1Fast_DecayC,fPool1Fast_DecayN;
  float fPool1Slow_DecayC,fPool1Slow_DecayN;
  float fPool2Fast_DecayC,fPool2Fast_DecayN;
  float fPool2Slow_DecayC,fPool2Slow_DecayN;
  float fPool3_DecayC,fPool3_DecayN;

  /*Variablen f�r die Stofffl�sse*/     /*Originalbezeichung*/
  float fNrelease;                      /*SNMIN 1. Schritt  */
  float fPool0toPool1;                  /*RTIMB8*/ 
  float fPool0toPool2;                  /*RTIMB9*/ 
  float fPool1toPool1;                  /*RTIMB1*/
  float fPool1toPool2;                  /*RTIMB2*/
  float fPool2toPool1;                  /*RTIMB3*/ 
  float fPool2toPool3;                  /*RTIMB4*/ 
  float fPool3toPool1;                  /*RTIMB5*/
  float fPool0toCO2C,fPool1toCO2C,fPool2toCO2C,fPool3toCO2C;
  float fNToPool1;                      /*RENIMB*/
  float fNToPool2,fNToPool3,fNToNH4;
  float fNfree;                         /*SNMIN 2. Schritt*/
  float fCToBiomass;

  /* Aufteilung der Stofffl�sse  */
  const float EFHUM  = (float)0.2;    /* org.: �ber Input-File              */
  const float EFFAC  = (float)0.5;    /* org.: �ber Input-File              */
  const float ABOM1  = (float)0.56;   /* Aufteilung resistant/labile */
         
  /* C/N Verh�ltnisse */
  float fCN_AOM  = (float)8.0;  /* org.: �ber Input-File */
  float fCN_BOM  = (float)8.0;  /* org.: �ber Input-File */
  float fCN_SOM  = (float)8.0;  /* org.: �ber Input-File */

  float fCN_FOMSlow,fCN_FOMFast; 
  float fCN_MicBiomSlow,fCN_MicBiomFast;
  float fCN_HumusSlow,fCN_HumusFast,fCN_HumusStable;


  /*Variablen f�r die Massenbilanz*/
  float fCTotal0,fNTotal0;                     /* Summe: AOM + BOM + SOM zu Beginn des Zeitschritts */
  float fCTotal1,fNTotal1;		       		   /* Summe: AOM + BOM + SOM zum Ende des Zeitschritts */
  float fCDiff,fNDiff;                         /* Differenz zwischen momentaner und anfaenglicher Summe*/
  static float fCDiffSum = (float)0.0;
  static float fNDiffSum = (float)0.0;
  
  float afAnt[7], fC_Old, fN_Old;

  SurfaceMiner(exp_p);    //Aufruf der Oberfl�chenmineralisierung
  NCSOILTillage(exp_p);     //Aufruf der Tillagemassnahme


  N_ZERO_LAYER 

  for (N_SOIL_LAYERS) 
    {    
	if(!SimStart(pTi))
	 {
 	  fC_Old = pCL->fCFOMSlow + pCL->fCFOMFast + 
	           pCL->fCMicBiomSlow + pCL->fCMicBiomFast +
               pCL->fCHumusSlow + pCL->fCHumusFast +
			   pCL->fCHumusStable;


      if(fC_Old)
	  {
	   afAnt[0]  = pCL->fCFOMSlow     / fC_Old; 
	   afAnt[1]  = pCL->fCFOMFast     / fC_Old;
	   afAnt[2]  = pCL->fCMicBiomSlow / fC_Old;
	   afAnt[3]  = pCL->fCMicBiomFast / fC_Old;
	   afAnt[4]  = pCL->fCHumusSlow   / fC_Old;
	   afAnt[5]  = pCL->fCHumusFast   / fC_Old;
	   afAnt[6]  = pCL->fCHumusStable / fC_Old;
	 	 
	   pCL->fCFOMSlow     = afAnt[0] * pSL->fCHumus;
       pCL->fCFOMFast     = afAnt[1] * pSL->fCHumus;
       pCL->fCMicBiomSlow = afAnt[2] * pSL->fCHumus;
       pCL->fCMicBiomFast = afAnt[3] * pSL->fCHumus;
       pCL->fCHumusSlow   = afAnt[4] * pSL->fCHumus;
       pCL->fCHumusFast   = afAnt[5] * pSL->fCHumus;
       pCL->fCHumusStable = afAnt[6] * pSL->fCHumus;
	  }
	 
	  fN_Old = pCL->fNFOMSlow + pCL->fNFOMFast + 
	           pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
               pCL->fNHumusSlow + pCL->fNHumusFast +
			   pCL->fNHumusStable;

	  if(fN_Old)
	  {
	   afAnt[0]  = pCL->fNFOMSlow     / fN_Old; 
	   afAnt[1]  = pCL->fNFOMFast     / fN_Old;
	   afAnt[2]  = pCL->fNMicBiomSlow / fN_Old;
	   afAnt[3]  = pCL->fNMicBiomFast / fN_Old;
	   afAnt[4]  = pCL->fNHumusSlow   / fN_Old;
	   afAnt[5]  = pCL->fNHumusFast   / fN_Old;
	   afAnt[6]  = pCL->fNHumusStable / fN_Old;
	 	 
	   pCL->fNFOMSlow     = afAnt[0] * pSL->fNHumus;
       pCL->fNFOMFast     = afAnt[1] * pSL->fNHumus;
       pCL->fNMicBiomSlow = afAnt[2] * pSL->fNHumus;
       pCL->fNMicBiomFast = afAnt[3] * pSL->fNHumus;
       pCL->fNHumusSlow   = afAnt[4] * pSL->fNHumus;
       pCL->fNHumusFast   = afAnt[5] * pSL->fNHumus;
       pCL->fNHumusStable = afAnt[6] * pSL->fNHumus;
	  }
	 }
     
    /******************************************************************/
    /*                          Reduktionsfunktionen                  */
    /******************************************************************/
	/* 1. Temperatur-Reduktionsfunktion, Q10 Funktion*/
    corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
    
	/* 2. Feuchte-Reduktionsfunktion, Ansatz DAISY, Bezugswert Wasserspannung */
	f1= (float)-31622770;    //pF = 6.5 
    wmin = WATER_CONTENT(f1);
    f1= (float)-3160;        //pF = 2.5 
    wlow = WATER_CONTENT(f1);
    f1= (float)-316;         //pF = 1.5 
    whigh = WATER_CONTENT(f1);
    f1= (float)0;
    wmax = WATER_CONTENT(f1);

	corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							wmax, pPA->fMinerSatActiv);
    

	/******************************************************************/
    /*    C und N Stofffl�sse der Bodenpools                                        */
    /******************************************************************/
	
	/* 1. CN Werte berechnen */

/* Wenn der CN Wert der mikrobiellen Biomasse variabel sein soll 
	fCN_FOMSlow     = (pCL->fNFOMSlow && pCL->fCFOMSlow) ?
    	pCL->fCFOMSlow/pCL->fNFOMSlow
    	:fCN_AOM; 

	fCN_FOMFast     = (pCL->fNFOMFast && pCL->fCFOMFast) ?
    	pCL->fCFOMFast/pCL->fNFOMFast
    	:fCN_AOM; 
   */
 	
	
	fCN_FOMSlow = fCN_AOM;
	fCN_FOMFast = fCN_AOM;
	
	
	fCN_MicBiomSlow     = (pCL->fNMicBiomSlow && pCL->fCMicBiomSlow) ?
    	pCL->fCMicBiomSlow/pCL->fNMicBiomSlow
    	:fCN_BOM; 
  	
 	fCN_MicBiomFast     = (pCL->fNMicBiomFast && pCL->fCMicBiomFast) ?
    	pCL->fCMicBiomFast/pCL->fNMicBiomFast
    	:fCN_BOM; 
	
	fCN_HumusSlow     = (pCL->fNHumusSlow && pCL->fCHumusSlow) ?
    	pCL->fCHumusSlow/pCL->fNHumusSlow
    	:fCN_SOM; 
	
	fCN_HumusFast     = (pCL->fNHumusFast && pCL->fCHumusFast) ?
    	pCL->fCHumusFast/pCL->fNHumusFast
    	:fCN_SOM; 
	
	fCN_HumusStable   = (pCL->fNHumusStable && pCL->fCHumusStable) ?
    	pCL->fCHumusStable/pCL->fNHumusStable
    	:fCN_SOM; 
   

    fPool0Slow_DecayC  = pCL->fCFOMSlow     * pCL->fFOMSlowDecMaxR      * corr.Temp * corr.Feucht;
    fPool0Fast_DecayC  = pCL->fCFOMFast     * pCL->fFOMFastDecMaxR      * corr.Temp * corr.Feucht;
	fPool1Fast_DecayC  = pCL->fCMicBiomFast * pCL->fMicBiomSlowDecMaxR  * corr.Temp * corr.Feucht;
	fPool1Slow_DecayC  = pCL->fCMicBiomSlow * pCL->fMicBiomFastDecMaxR  * corr.Temp * corr.Feucht;
	fPool2Slow_DecayC  = pCL->fCHumusSlow   * pCL->fHumusSlowMaxDecMaxR * corr.Temp * corr.Feucht;
	fPool2Fast_DecayC  = pCL->fCHumusFast   * pCL->fHumusFastMaxDecMaxR * corr.Temp * corr.Feucht;
	fPool3_DecayC      = pCL->fCHumusStable * pCL->fHumusMinerMaxR      * corr.Temp * corr.Feucht;

    fPool0Fast_DecayN  = fPool0Fast_DecayC  / fCN_FOMFast;
    fPool0Slow_DecayN  = fPool0Slow_DecayC  / fCN_FOMSlow;
	fPool1Fast_DecayN  = fPool1Fast_DecayC  / fCN_MicBiomFast;
	fPool1Slow_DecayN  = fPool1Slow_DecayC  / fCN_MicBiomSlow;
	fPool2Fast_DecayN  = fPool2Fast_DecayC  / fCN_HumusFast;
	fPool2Slow_DecayN  = fPool2Slow_DecayC  / fCN_HumusSlow;
	fPool3_DecayN      = fPool3_DecayC      / fCN_HumusStable;  
    
	/* Insgesamt freigesetzte Mengen N*/
	fNrelease = fPool0Fast_DecayN + fPool0Slow_DecayN +
	            fPool1Fast_DecayN + fPool1Slow_DecayN +                   
	            fPool2Fast_DecayN + fPool2Slow_DecayN + fPool3_DecayN;
  
	/* Aufteilung der Stofffl�sse */									   
	fPool0toPool1 = EFFAC * (fPool0Fast_DecayC + fPool0Slow_DecayC); 
	fPool0toPool2 = EFHUM * (fPool0Fast_DecayC + fPool0Slow_DecayC); 
	fPool0toCO2C  = ((float)1.0 - EFFAC - EFHUM) * (fPool0Fast_DecayC + fPool0Slow_DecayC);
	
	fPool1toPool1 = EFFAC * (fPool1Fast_DecayC + fPool1Slow_DecayC); 
	fPool1toPool2 = EFHUM * (fPool1Fast_DecayC + fPool1Slow_DecayC);
	fPool1toCO2C  = ((float)1.0 - EFFAC - EFHUM) * (fPool1Fast_DecayC + fPool1Slow_DecayC);

	fPool2toPool1 = EFFAC * (fPool2Fast_DecayC + fPool2Slow_DecayC); 
	fPool2toPool3 = EFHUM * (fPool2Fast_DecayC + fPool2Slow_DecayC);
	fPool2toCO2C  = ((float)1.0 - EFFAC - EFHUM) * (fPool2Fast_DecayC + fPool2Slow_DecayC);

	fPool3toPool1 = EFFAC * fPool3_DecayC;
	fPool3toCO2C  = ((float)1.0 - EFFAC) * fPool3_DecayC;
	
	/* N-Mengen, die direkt in Pool 2 und Pool 3 �bertragen werden*/
    fNToPool2  = (fPool0toPool2 + fPool1toPool2) / (ABOM1 * fCN_HumusFast + ((float)1.0 -ABOM1) * fCN_HumusSlow);
	fNToPool3  = fPool2toPool3 / fCN_HumusStable;
	
	/* N-Mengen die in Pool 1 immobilisiert werden */
    fNToPool1   = (fPool0toPool1 + fPool1toPool1 + fPool2toPool1 + fPool3toPool1) / (ABOM1 * fCN_MicBiomFast + ((float)1.0 -ABOM1) * fCN_MicBiomSlow); 
	
	/* Brutto mineralisierter Stickstoff */ 
	fNfree = fNrelease - (fNToPool2 + fNToPool3); /*SNMIN 2. Schritt*/
	
	/* Netto mineralisierter Stickstoff */
	fNToNH4 = fNfree - fNToPool1;
	
	/* Abbruch wenn es zur Nettoimmobilisierung kommt*/
	if (fNToNH4 < (float)0.0)
	Message(3,ERROR_NOT_POSITIVE_TXT);

    /* Kohlendioxid Freisetzung*/
	pCL->fCO2ProdR = fPool0toCO2C + fPool1toCO2C + fPool2toCO2C + fPool3toCO2C;
	

    /****************************************************************************/
    /*      Aufsummierung der C und N Mengen f�r die Zeitschrittbilanz          */ 
    /****************************************************************************/

	fCTotal0 = pCL->fCFOMSlow + pCL->fCFOMFast +
			   pCL->fCMicBiomSlow + pCL->fCMicBiomFast + 
			   pCL->fCHumusSlow + pCL->fCHumusFast +
			   pCL->fCHumusStable + pCL->fCO2C + pCL->fCLitter + pCL->fCManure;

   	pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_FOMFast;
    pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_FOMSlow; 
	pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_MicBiomFast;
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_MicBiomSlow;
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / fCN_HumusFast;
    pCL->fNHumusFast   = pCL->fCHumusFast   / fCN_HumusSlow;
    pCL->fNHumusStable = pCL->fCHumusStable / fCN_HumusStable;
	
	fNTotal0 = pCL->fNFOMSlow + pCL->fNFOMFast + 
			   pCL->fNMicBiomSlow + pCL->fNMicBiomFast +
			   pCL->fNHumusSlow +  pCL->fNHumusFast +
			   pCL->fNHumusStable + pCL->fNH4N + pCL->fNO3N + pCL->fNLitter + pCL->fNManure;
   
	
    /* Ver�nderung im Ammonium-Pool */
    pCL->fHumusMinerR = fNToNH4;
    pCL->fNH4N += pCL->fHumusMinerR * DeltaT;  

 /****************************************************************************/
 /*      Abbau der Residues, als Schleife programmiert um eine eventuelle    */ 
 /*      Erweiterung um weitere Residue-Pools zu erleichtern                 */
 /****************************************************************************/

	/* Null setzen von Sammelvariablen */
     fCwithNmin_Sum = (float) 0.0;
	 fNImmobR       = (float) 0.0;
     fNMinerR       = (float) 0.0;
     fNBedarf_Total = (float) 0.0;

    /* �bertragen der relevanten Werte in die Arrays */
    pCL->afCOrgFOMFrac[0] = pCL->fCLitter;
    pCL->afNOrgFOMFrac[0] = pCL->fNLitter;
    pCL->afCOrgFOMFrac[1] = pCL->fCManure;
    pCL->afNOrgFOMFrac[1] = pCL->fNManure;
  
    if (pCL->afNOrgFOMFrac[0])
      afCN_Res[0] = pCL->afCOrgFOMFrac[0]/pCL->afNOrgFOMFrac[0];
  
    if (pCL->afNOrgFOMFrac[1])
      afCN_Res[1] = pCL->afCOrgFOMFrac[1]/pCL->afNOrgFOMFrac[1];

    pCL->afFOMDecR[0] = pCL->fLitterMinerMaxR;
    pCL->afFOMDecR[1] = pCL->fManureMinerMaxR;
  
    /* Zur Verf�gung stehendes mineralisches N */
    fNH4ImmR = fNH4ImmMaxK * pCL->fNH4N;
    fNO3ImmR = fNO3ImmMaxK * pCL->fNO3N;
    fAvailN  = fNO3ImmR + fNH4ImmR;

    /* Berechnung des Gesamt N Bedarfs */
    for (i1 = 0; i1 < ANZ; i1++)
     {
      // Mengen C die abgebaut werden
      afRes_DecayC[i1] = pCL->afCOrgFOMFrac[i1] * pCL->afFOMDecR[i1] * corr.Temp * corr.Feucht;
      
	  // CN-Reduktionsfaktor, Ansatz DNDC
      fCNrate = afRes_DecayC[i1]/(fAvailN + afRes_DecayC[i1] / afCN_Res[i1]);
      fRed = min(1,(float)0.2 + (float)7.2 / fCNrate);
	    
	  //Ansatz CERES  fRed = min(1,(float)exp((double)( ((float)-0.693 * (fCNrate - (float)25.0)) / (float)25.0)));
	  //Ansatz NCSOIL fRed = CN_Red(fCNrate);*/
        
  	  //reduzierter Abbau
  	  afRes_DecayC[i1] = fRed * afRes_DecayC[i1];
        
	  //Bedarf an N
	  fNBedarf[i1] = EFFAC/fCN_AOM * afRes_DecayC[i1];
	  fNBedarf_Total += fNBedarf[i1];
     }  
  
    for (i1 = 0; i1 < ANZ; i1++)
  	 {
      /* Wenn nicht gen�gend mineralischer N zur Verf�gung steht, 
         wird die mikrobielle Effektivit�t ver�ndert und es entsteht mehr 
		 CO2 und weniger mikrobielle Biomasse */
	  if(fAvailN < fNBedarf_Total)
       {
	    //C-Mengen die mit mineralischem N eingebaut werden
	    fCwithNmin = fNBedarf[i1] * fAvailN/fNBedarf_Total * (ABOM1 * fCN_FOMFast + ((float)1.0 -ABOM1) * fCN_FOMSlow);
		fCwithNmin_Sum += fCwithNmin;
		//entstehendes CO2
		fCO2ProdR = afRes_DecayC[i1] - fCwithNmin;
	   }
     
	  else
       {
	    fCwithNmin = fNBedarf[i1] * (ABOM1 * fCN_FOMFast + ((float)1.0 -ABOM1) * fCN_FOMSlow);
        fCwithNmin_Sum += fCwithNmin;
		fCO2ProdR = ((float)1.0 - EFFAC) * afRes_DecayC[i1];
	   }
      
	  //CO2 Produktion
      pCL->fCO2ProdR += fCO2ProdR;

	  //Brutto-Immobilisierung in den Biomasse-Pool
	  fNImmobR += fCwithNmin / (ABOM1 * fCN_FOMFast + ((float)1.0 -ABOM1) * fCN_FOMSlow);
	
	  //Brutto-Mineralisierung
	  if(!afCN_Res[i1]) 
	   afCN_Res[i1] = (float)0.1;
	  fNMinerR +=  afRes_DecayC[i1] / afCN_Res[i1];

	  //Ver�nderungen in den Residue-Pools
      pCL->afCOrgFOMFrac[i1] -= afRes_DecayC[i1] * DeltaT;
      if(!afCN_Res[i1])
	   afCN_Res[i1] = (float)0.1;
	  pCL->afNOrgFOMFrac[i1] = pCL->afCOrgFOMFrac[i1] / afCN_Res[i1];
	} // Residue-Schleife zu Ende
	  
    //Mineral-N Bilanz
	h1 = fNMinerR - fNImmobR;
    fNetMinR = (h1 > 0)? h1 : 0;
    fNetImmR   = (h1 < 0)? (float)-1 * h1 : 0;

	/*
	fNH4R = fNMinerR - (fNImmobR * RelAnteil(pCL->fNH4N,pCL->fNO3N));
    fNO3R = fNImmobR * RelAnteil(pCL->fNO3N,pCL->fNH4N);  	
   */
    if (fNetMinR > 0) //Mineralisierung
     
      pCL->fNH4N += fNetMinR * DeltaT;  
      
    else             //Immobilisierung
     { 
	  fNH4NR = fNetImmR * RelAnteil(pCL->fNH4N,pCL->fNO3N);  
      fNO3NR = fNetImmR * RelAnteil(pCL->fNO3N,pCL->fNH4N);  	
	 
	  pCL->fNH4N -= fNH4NR * DeltaT; 
	  pCL->fNO3N -= fNO3NR * DeltaT;
	 }

    /*Zur �bertragung auf globale Variable, Berechnung der Nettoraten */

    pCL->fNLitterImmobR = fNetImmR;
	pCL->fLitterMinerR = fNetMinR;

	pCL->fNImmobR = fNetImmR;
    pCL->fMinerR = pCL->fLitterMinerR + pCL->fHumusMinerR;

    //Berechnen der neuen Anteile
    pCL->fCLitter = pCL->afCOrgFOMFrac[0]; 
    pCL->fNLitter = pCL->afNOrgFOMFrac[0]; 
 
    pCL->fCManure = pCL->afCOrgFOMFrac[1]; 
    pCL->fNManure = pCL->afNOrgFOMFrac[1]; 

    /********************* Residue-Abbau zu Ende **********************/


    /******************************************************************/ 
    /* Veraenderung des C- und N-Pools(au�er Ammonium und Nitrat)     */
    /* pro Zeitschritt,da CN-Verh�ltnis in den Pools konstant ist,    */
    /* ist eine separate Modellierung des N redundant.                */
    /******************************************************************/

    /* 1. Veraenderung der zymogenen Biomasse Pools (Pool0)*/
	pCL->fCFOMFast += (ABOM1 * fCwithNmin_Sum - fPool0Fast_DecayC) * DeltaT;
    pCL->fCFOMSlow += ((((float)1.0 - ABOM1) * fCwithNmin_Sum) - fPool0Slow_DecayC) * DeltaT;

	/* 2. Veraenderung der autochtonen Biomasse Pools (PoolI) */
    fCToBiomass = fPool0toPool1 + fPool1toPool1 + fPool2toPool1 + fPool3toPool1;
    pCL->fCMicBiomFast += (ABOM1 * fCToBiomass - fPool1Fast_DecayC) * DeltaT;
    pCL->fCMicBiomSlow += ((((float)1 - ABOM1) * fCToBiomass) - fPool1Slow_DecayC) * DeltaT;
            
    /* 3. Veraenderung der Humads Pools (PoolII) */
    pCL->fCHumusSlow   += ((fPool1toPool2 + fPool0toPool2) - fPool2Slow_DecayC) * DeltaT;
    pCL->fCHumusFast   -= fPool2Fast_DecayC * DeltaT;
      
    /* 4. Veraenderung des stabilen Humus Pools (PoolIII) */
	pCL->fCHumusStable += (fPool2toPool3 - fPool3_DecayC) * DeltaT;

    /* 5. Veraenderung des CO2-Pools pro Zeitschritt */
    pCL->fCO2C    += pCL->fCO2ProdR * DeltaT;
   
	/* 6. Ver�nderung in den N-Pools */
    pCL->fNFOMSlow     = pCL->fCFOMSlow     / fCN_FOMFast;
	pCL->fNFOMFast     = pCL->fCFOMFast     / fCN_FOMSlow; 
    pCL->fNMicBiomSlow = pCL->fCMicBiomSlow / fCN_MicBiomFast; 
    pCL->fNMicBiomFast = pCL->fCMicBiomFast / fCN_MicBiomSlow;  
    pCL->fNHumusSlow   = pCL->fCHumusSlow   / fCN_HumusFast;  
    pCL->fNHumusFast   = pCL->fCHumusFast   / fCN_HumusSlow; 
    pCL->fNHumusStable = pCL->fCHumusStable / fCN_HumusStable;

    /* �bertagen der Biomasse auf Ouptut-Pool*/
    pCL->fCSoilMicBiom = pCL->fCFOMSlow + pCL->fCFOMFast +
                         pCL->fCMicBiomSlow + pCL->fCMicBiomFast;
   
    pCL->fNSoilMicBiom = pCL->fNFOMSlow + pCL->fNFOMFast +
	                     pCL->fNMicBiomSlow + pCL->fNMicBiomFast; 

    /**********************************************************************/
    /*                Zeitschritt - Bilanzen bilden                       */
    /**********************************************************************/

    pSL->fCHumus = pCL->fCFOMSlow + pCL->fCFOMFast +
                   pCL->fCMicBiomSlow + pCL->fCMicBiomFast +
			       pCL->fCHumusSlow + pCL->fCHumusFast +
			       pCL->fCHumusStable;
		
    pSL->fNHumus = pCL->fNFOMSlow + pCL->fNFOMFast +
	               pCL->fNMicBiomSlow + pCL->fNMicBiomFast + 
			       pCL->fNHumusSlow + pCL->fNHumusFast +
			       pCL->fNHumusStable;

	fCTotal1 =     pSL->fCHumus + pCL->fCO2C +
                   pCL->fCLitter + pCL->fCManure;


	fNTotal1 =     pSL->fNHumus + pCL->fNH4N + pCL->fNO3N +
                   pCL->fNLitter + pCL->fNManure;    

	fCDiff = fCTotal1 - fCTotal0;    
    fCDiffSum += fCDiff;
	fNDiff = fNTotal1 - fNTotal0;
    fNDiffSum += fNDiff;
    }    /* Schichtweise */   
        
  
  return 0;
}


/********************************************************************************************/
/* Procedur    :   SoilNitrogen_GECROS()                                                    */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Bodenstickstofftransport mit Mineralisierung                             */
/*                 Methode GECROS (Yin and van Laar 2005)                                   */
/*                                                                                          */
/*                 HMGU/ep              2008                                                */
/*                                                                                          */
/********************************************************************************************/
/* ver�nd. Var.		pCL->fCFOMFast        pCL->fNFOMFast                                    */
/********************************************************************************************/
#include  "gecros.h"
extern double WINAPI _loadds NOTNUL(double x);
extern double WINAPI _loadds FCNSW(double x,double y1,double y2,double y3);
extern double WINAPI _loadds INSW(double x,double y1,double y2);
extern double WINAPI _loadds LIMIT(double y1,double y2,double x);


int SoilNitrogen_GECROS(EXP_POINTER)
{
	  PGECROSNITROGEN  pGPltN = pGecrosPlant->pGecrosNitrogen;
	  PGECROSSOIL      pGS    = pGecrosPlant->pGecrosSoil;

      /*** for output to expertn ***/
      PSLAYER      pSL   = pSo->pSLayer->pNext;
      PWLAYER      pSLW  = pWa->pWLayer->pNext; 
      PCLAYER      pSLN  = pCh->pCLayer->pNext;
	  PCPROFILE    pCP   = pCh->pCProfile;

	  PLAYERROOT pLR;
      //if (pPl != NULL) pLR=pPl->pRoot->pLayerRoot;
      //PPLTNITROGEN pPltN = pPl->pPltNitrogen;


      /*** local variables ***/
	  double RD,RRD,RAIN;
	  double WCMAX,RFIR,RRUL,RWUG;
	  double DPM,RDPM,RPM,RRPM,BIO,RBIO,HUM,RHUM,LITC,LITN,DPN,RDPN,RPN,RRPN;
	  double DECDPM,DECRPM,DECBIO,DECHUM,CNDRPM,DECDPN,DECRPN;
	  double DPMR,RPMR,DPMRC,RPMRC,RESCO2;
	  double NAUL,NALL,NNUL,NNLL,RNAUL,RNALL,RNNUL,RNNLL;
	  //double NA,NN,NMINER;
	  double NITRUL,NITRLL,DENIUL,DENILL,FMUL,FMLL,NUPTA,NUPTN,NUPT,NDEM,NFIXR;
   	  double WUL,WLL,FERNA,SFERNA,RSFNA,VOLA,FERNN;
	  double CBH,FT,FM;
	  double FWS,NSUPAS,NSUPNS,NSUPA,NSUPN,NSUP;
	  double LAYNA,LAYNN;
	  double LEAUL,LEALL;
	  double MDN,MDNUL,MDNLL;
	  double MINAUL,MINALL,MINNUL,MINNLL;
	  double TSOIL,RTSOIL,TAVSS,TMAX,TMIN,DAVTMP,NAVTMP,DIFS;
      
	  /*** fixed parameters ***/ 
	  double DRPM, DPMR0, RPMR0, TOC, BHC, FBIOC;
	  double CLAY, WCMIN, WCPWC, TCP, TCT, BIOR, HUMR;
	  /*
	  double DRPM =(double)1.44, DPMR0 =(double)10., RPMR0 =(double)0.3, BIOR =(double)0.66, HUMR =(double)0.02;
	  double TOC   = (double)7193.0, BHC = (double) 3500.0, FBIOC = (double)0.03;
	  double CLAY  = (double)23.4, TCP = (double)1., TCT = (double)4., WCMIN = (double)0.05, WCPWC = (double)25.;
	  */
      double SD1 = (double)25.; 
      double RA = (double)1., RN = (double)1.;
      
	  //zum Testen:
	  //double WCFC  = (double)0.25;//test
	  double NINPA = (double)0., NINPN = (double)0.65;
	  double NSWI  = (double)+1.;//test: -1. sonst +1.
      
	  /*** for initialisation ***/
      extern float fparSD1,fparTOC,fparBHC,fparFBIOC;
	  extern float fparPRFDPT,fparCLAY,fparWCMIN,fparWCPWC,fparTCT,fparTCP;
	  extern float fparBIOR,fparHUMR,fparDRPM,fparDPMR0,fparRPMR0;
      extern float fparTOC,fparBHC,fparFBIOC;

	  //float fRA, fRN, fNAI, fNNI;

	  /*** for output to expertn ***/
	  int L;
	  float f1,f2,frd,fpd,DeltaZ;
	  DeltaZ = pSo->fDeltaZ;
	  if (pPl != NULL) pLR=pPl->pRoot->pLayerRoot;

	  //*** initialisation N contents
	  if (SimStart(pTi))
	  {
	   NAUL = (double)0;
	   NNUL = (double)0;
	   NALL = (double)0;
	   NNLL = (double)0;
       //pCP->dNO3LeachCum = (double)0;// in yinit.c
	   frd=(float)0;
	   fpd = (pSo->iLayers-2)*DeltaZ;

	   if (fparSD1==(float)-99) fparSD1 = (float)SD1;
	   
	   if (pPl!=NULL)
	   {
        pPl->pRoot->fDepth = (float)max(2.,(double)fparSD1);
        frd = min(fpd,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));
	   }
	   else
	   {
        frd = (float)max(2.,(double)fparSD1*10.);
	   }

	   f2  = (float)NOTNUL(fpd-frd)/DeltaZ;

	   for (L=1;L<=pSo->iLayers-2;L++)
       {
        if(L*DeltaZ <= frd) 
	    {
		 NAUL += (double)pSLN->fNH4N/10.;
		 NNUL += (double)pSLN->fNO3N/10.;
	    }
	    else if ((frd < L*DeltaZ)&&((L-1)*DeltaZ <= frd))
        {
         f1 = (L*DeltaZ-frd)/DeltaZ;
		 NAUL += (double)(pSLN->fNH4N*((float)1-f1))/10.;
		 NALL += (double)(pSLN->fNH4N*f1)/10.;
         NNUL += (double)(pSLN->fNO3N*((float)1-f1))/10.;
		 NNLL += (double)(pSLN->fNO3N*f1)/10.;
	    }
	    else
	    {
         NALL += (double)pSLN->fNH4N/10.;             
		 NNLL += (double)pSLN->fNO3N/10.;
	    }//if,else if,else

		pSLN=pSLN->pNext;
	   }//for

	  pGS->fNH4NContUpperLayer = (float)NAUL;
      pGS->fNH4NContLowerLayer = (float)NALL;
	  pGS->fNO3NContUpperLayer = (float)NNUL;
	  pGS->fNO3NContLowerLayer = (float)NNLL;

	  /*
	  DPMI   = ZERO
      RPMI   = TOC - BHC - DPMI
      BIOI   = FBIOC * TOC
      HUMI   = BHC - BIOI

	  DPNI   = 1./ 40.*DPMI
      RPNI   = 1./100.*RPMI
	  */

	  ///*
	  if (fparTOC==(float)-99) fparTOC = (float)7000;
	  TOC = (double)fparTOC;

	  if (fparBHC==(float)-99) fparBHC = (float)3500;
	  BHC = (double)fparBHC;

	  if (fparFBIOC==(float)-99) fparFBIOC = (float)0.03;
	  FBIOC = (double)fparFBIOC;

	  pGS->fDecPlantMatC = (float)0.;
	  pGS->fResPlantMatC = fparTOC - fparBHC - pGS->fDecPlantMatC;
	  pGS->fMicroBiomC   = fparFBIOC * fparTOC;
	  pGS->fHumusC       = fparBHC - fparFBIOC * fparTOC;

	  pGS->fDecPlantMatN = (float)1./(float)40.  * pGS->fDecPlantMatC;
      pGS->fResPlantMatN = (float)1./(float)100. * pGS->fResPlantMatC;
	  //*/

      /*
      NAI    = 2.
      NNI    = 2.
      NAULI  = (1.-EXP(-0.065*RDI))*NAI +     RDI/150. *RA
      NALLI  =     EXP(-0.065*RDI) *NAI + (1.-RDI/150.)*RA
      NNULI  = (1.-EXP(-0.065*RDI))*NNI +     RDI/150. *RN
      NNLLI  =     EXP(-0.065*RDI) *NNI + (1.-RDI/150.)*RN
      */
      /*
      fNAI = pGS->fNH4NContUpperLayer + pGS->fNH4NContLowerLayer;
	  fNNI = pGS->fNO3NContUpperLayer + pGS->fNO3NContLowerLayer;
	  fRA  = (float)RA;
	  fRN  = (float)RN;
	  fNAI = (float)2.;
	  fNNI = (float)2.;
	  pGS->fNH4NContUpperLayer = ((float)1.-(float)exp(-0.065*(double)frd))* fNAI +           frd/fparPRFDPT *fRA;
      pGS->fNH4NContLowerLayer =            (float)exp(-0.065*(double)frd) * fNAI +((float)1.-frd/fparPRFDPT)*fRA;
	  pGS->fNO3NContUpperLayer = ((float)1.-(float)exp(-0.065*(double)frd))* fNNI +           frd/fparPRFDPT *fRN;
	  pGS->fNO3NContLowerLayer =            (float)exp(-0.065*(double)frd) * fNNI +((float)1.-frd/fparPRFDPT)*fRN;
	  */

	  }//SimStart

      /* input from readmod soil parameters I & II: marker 80009, 80010*/
	  if (fparPRFDPT==(float)-99) fparPRFDPT = (float)150.;
	  pGS->fProfileDepth = fparPRFDPT;

      if (fparCLAY==(float)-99) fparCLAY = (float)23.4;
	  CLAY  = (double)fparCLAY;

	  if (fparWCMIN==(float)-99) fparWCMIN = (float)0.05;
	  WCMIN  = (double)fparWCMIN;

      if (fparWCPWC==(float)-99) fparWCPWC = (float)375;
	  WCPWC  = (double)fparWCPWC/NOTNUL((double)fparPRFDPT*10);
      pGS->fPlantWaterCapacity = (float)WCPWC;
      //pGS->fPlantWaterCapacity = (float)WCFC;//test

	  if (fparSD1==(float)-99) fparSD1 = (float)25.0;
	  SD1    = (double)fparSD1;

      if (fparTCT==(float)-99) fparTCT = (float)4;
	  TCT    = (double)fparTCT;

      if (fparTCP==(float)-99) fparTCP = (double)1;
	  TCP    = (double)fparTCP;

      if (fparBIOR==(float)-99) fparBIOR = (float)0.66;
	  BIOR   = (double)fparBIOR;

      if (fparHUMR==(float)-99) fparHUMR = (float)0.02;
	  HUMR   = (double)fparHUMR;

      if (fparDRPM==(float)-99) fparDRPM = (float)1.44;
	  DRPM   = (double)fparDRPM;

	  if (fparDPMR0==(float)-99) fparDPMR0 = (float)10.0;
	  DPMR0  = (double)fparDPMR0;

      if (fparRPMR0==(float)-99) fparRPMR0 = (float)0.3;
	  RPMR0  = (double)fparRPMR0;
	  


	  /*** input from expertn global variables ***/
	  if (pPl!=NULL) RD     = (double)pPl->pRoot->fDepth;
	  if (pPl!=NULL) RRD    = (double)pPl->pRoot->fDepthGrowR;
	  //NUPTN = (double)pPltN->fActNO3NUpt/10.0;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      //NUPTA = (double)pPltN->fActNH4NUpt/10.0;//[kg N ha-1 d-1] --> [g N m-2 d-1]
	  RAIN   = (double)pCl->pWeather->fRainAmount;
	  //RFIR  = (double)pWa->pWBalance->fReservoir;//RAIN + IRRI
	  RFIR   = (double)pWa->fInfiltR;
      RWUG   = (double)pWa->fPercolR;

      /*** input from GECROS global variables ***/
	  RRUL   = (double)pGS->fWaterFlowToLowLayer;
	  DIFS   = (double)pGS->fDiffSoilAirTemp;
	  TSOIL  = (double)pGS->fSoilTemp;//pHe->pHBalance->fProfil ???;
      RTSOIL = (double)pGS->fSoilTempR;

      DPM  = (double)pGS->fDecPlantMatC;
	  RDPM = (double)pGS->fDecPltMCRate;
      RPM  = (double)pGS->fResPlantMatC;
	  RRPM = (double)pGS->fResPltMCRate;
	  BIO  = (double)pGS->fMicroBiomC;
	  HUM  = (double)pGS->fHumusC;
	  RBIO = (double)pGS->fMicBiomCRate;
	  RHUM = (double)pGS->fHumusCRate;
	  LITC = (double)pGS->fLitterC;
	  LITN = (double)pGS->fLitterN;

	  //DECDPM = (double)pGS->fDecPltMCDecR;
	  //DECRPM = (double)pGS->fResPltMCDecR;
	  //DECBIO = (double)pGS->fMicBiomCDecR;
	  //DECHUM = (double)pGS->fHumusCDecR;
	  //DECDPN = (double)pGS->fDecPltMNDecR;
	  //DECRPN = (double)pGS->fResPltMNDecR;

	  DPN    = (double)pGS->fDecPlantMatN;
      RDPN   = (double)pGS->fDecPltMNRate;
	  RPN    = (double)pGS->fResPlantMatN;
	  RRPN   = (double)pGS->fResPltMNRate;
	  CNDRPM = (double)pGS->fCNPlantMat;

	  WUL   = (double)pGS->fWaterContUpperLayer;
	  WLL   = (double)pGS->fWaterContLowerLayer;

      //*** NO3-N and NH4-N content ***
	  //*** upperlayer(UL) and lower layer (LL) ***
	  RNAUL  = (double)pGS->fNH4NContUpLayRate;
      RNALL  = (double)pGS->fNH4NContLwLayRate;
	  RNNUL  = (double)pGS->fNO3NContUpLayRate;
	  RNNLL  = (double)pGS->fNO3NContLwLayRate;
	  NAUL   = (double)0;//pGS->fNH4NContUpperLayer;
      NALL   = (double)0;//pGS->fNH4NContLowerLayer;
	  NNUL   = (double)0;//pGS->fNO3NContUpperLayer;
	  NNLL   = (double)0;//pGS->fNO3NContLowerLayer;

	  fpd = (pSo->iLayers-2)*DeltaZ;        
	  frd = min(fpd,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));

	  pSLN  = pCh->pCLayer->pNext;
	   for (L=1;L<=pSo->iLayers-2;L++)
       {
        if(L*DeltaZ <= frd) 
	    {
		 NAUL += (double)pSLN->fNH4N/10.;
		 NNUL += (double)pSLN->fNO3N/10.;
	    }
	    else if ((frd < L*DeltaZ)&&((L-1)*DeltaZ <= frd))
        {
         f1 = (L*DeltaZ-frd)/DeltaZ;
		 NAUL += (double)(pSLN->fNH4N*((float)1-f1))/10.;
		 NALL += (double)(pSLN->fNH4N*f1)/10.;
         NNUL += (double)(pSLN->fNO3N*((float)1-f1))/10.;
		 NNLL += (double)(pSLN->fNO3N*f1)/10.;
	    }
	    else
	    {
         NALL += (double)pSLN->fNH4N/10.;             
		 NNLL += (double)pSLN->fNO3N/10.;
	    }//if,else if,else

		pSLN=pSLN->pNext;
	   }//for

	  FERNA  = (double)pGS->fNH4NFertilizer;
	  SFERNA = (double)pGS->fNH4NVolatFert;
	  RSFNA  = (double)pGS->fNH4NVolatFertR;
	  VOLA   = (double)pGS->fNH4NVolatilizRate;
	  FERNN  = (double)pGS->fNO3NFertilizer;

	  NDEM  = (double)pGPltN->fNDemand;        	  
	  NFIXR = (double)pGPltN->fNFixationReserve;
    
	  /*** soil temperature ***/
	  TMAX = (double) pCl->pWeather->fTempMax;//[�C]
      TMIN = (double) pCl->pWeather->fTempMin;//[�C]
	  DAVTMP = 0.29*TMIN + 0.71*TMAX;
	  NAVTMP = 0.71*TMIN + 0.29*TMAX;

	  TAVSS  = ((DAVTMP+DIFS)+NAVTMP)/2.;
	  RTSOIL = (TAVSS - TSOIL)/TCT;

	  /*** environmental impact factors ***/
	  CBH    = 1.67*(1.85+1.60*exp(-0.0786*CLAY));
      FT     = 47.9/(1.  +exp(106./(TSOIL+18.3)));
      FM     = LIMIT(0.2, 1.0, 0.2+0.8*(WUL+WLL)/10./(double)pGS->fProfileDepth/(double)pGS->fPlantWaterCapacity);
      //FM     = LIMIT(0.2, 1.0, 0.2+0.8*(WUL+WLL)/10./(WCFC-WCMIN));//test
	  /*** Soil organic carbon ***/
	  CNDRPM = (DPM+RPM)/NOTNUL(DPN+RPN);

      DPMRC  = INSW(NNUL+NAUL+NNLL+NALL-RA-RN, 0., DPMR0);
      RPMRC  = INSW(NNUL+NAUL+NNLL+NALL-RA-RN, 0., RPMR0);

	  DPMR   = INSW(1./NOTNUL(CNDRPM)-1./8.5/(1.+CBH), DPMRC, DPMR0);
      RPMR   = INSW(1./NOTNUL(CNDRPM)-1./8.5/(1.+CBH), RPMRC, RPMR0);

      DECDPM = DPM*(1.-exp(-FT*FM*DPMR/365.))/TCP;
      DECRPM = RPM*(1.-exp(-FT*FM*RPMR/365.))/TCP;
      DECBIO = BIO*(1.-exp(-FT*FM*BIOR/365.))/TCP;
      DECHUM = HUM*(1.-exp(-FT*FM*HUMR/365.))/TCP;

	  RDPM   = LITC*DRPM/(1.+DRPM) - DECDPM;
      RRPM   = LITC*1.  /(1.+DRPM) - DECRPM;
      RBIO   = 0.46/(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM) - DECBIO;
      RHUM   = 0.54/(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM) - DECHUM;
	  RESCO2 = CBH /(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM);


	  /*** Soil organic nitrogen ***/
      DECDPN = DPN*(1.-exp(-FT*FM*DPMR/365.))/TCP;
      DECRPN = RPN*(1.-exp(-FT*FM*RPMR/365.))/TCP;

      RDPN   = LITN/(1.+ 40./DRPM/100.) - DECDPN;
      RRPN   = LITN/(1.+100.*DRPM/40. ) - DECRPN;



	  /*** Soil mineral nitrogen ***/
      MDN    = 1./8.5*(DECBIO+DECHUM)+ DECDPN+DECRPN -
               1./8.5/(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM);
	  MDNUL  = (1.-exp(-0.065*RD))*MDN;
      MDNLL  =     exp(-0.065*RD) *MDN;

      MINAUL = INSW(MDN,-min((NAUL-      RD /(double)pGS->fProfileDepth*RA)/TCP,-MDNUL),MDNUL);
      MINALL = INSW(MDN,-min((NALL-((double)pGS->fProfileDepth-RD)
		                                    /(double)pGS->fProfileDepth*RA)/TCP,-MDNLL),MDNLL);
      MINNUL = INSW(MDN,-min(NNUL/TCP,-MDNUL+MINAUL), 0.);
      MINNLL = INSW(MDN,-min(NNLL/TCP,-MDNLL+MINALL), 0.);

      FMUL   = LIMIT(0.2, 1.0, 0.2+0.8*WUL/10./      RD /(double)pGS->fPlantWaterCapacity);
      FMLL   = LIMIT(0.2, 1.0, 0.2+0.8*WLL/10./((double)pGS->fProfileDepth-RD)/(double)pGS->fPlantWaterCapacity);

	  NITRUL = max(0.,(NAUL+MINAUL*TCP-RD /(double)pGS->fProfileDepth*RA))*(1.-exp(-FT*FMUL*0.6/7.))/TCP;
      NITRLL = max(0.,(NALL+MINALL*TCP-
		               ((double)pGS->fProfileDepth-RD)/(double)pGS->fProfileDepth*RA))*(1.-exp(-FT*FMLL*0.6/7.))/TCP;
      			   
      DENIUL = .0005*max(0.,NNUL+MINNUL*TCP-RD /(double)pGS->fProfileDepth*RN)* RESCO2*(1.-exp(-0.065*RD));
      DENILL = .0005*max(0.,NNLL+MINNLL*TCP-
		                    ((double)pGS->fProfileDepth-RD)/(double)pGS->fProfileDepth*RN)* RESCO2*exp(-0.065*RD);
      
      FWS    = min(1., WUL/NOTNUL(RD*10.*(double)pGS->fPlantWaterCapacity));

      NSUPAS = max (0., NAUL+(MINAUL-NITRUL)*TCP-RD/(double)pGS->fProfileDepth*RA)/TCP;
      NSUPNS = max (0., NNUL+(MINNUL-DENIUL)*TCP-RD/(double)pGS->fProfileDepth*RN)/TCP*FWS;
      NSUPA  = INSW(NSWI, NINPA, NSUPAS);
      NSUPN  = INSW(NSWI, NINPN, NSUPNS);
	  NSUP   = NSUPA + NSUPN;

	  NUPTA  = min(NSUPA, NSUPA/NOTNUL(NSUP)*max(0.,NDEM-NFIXR/TCP));
      NUPTN  = min(NSUPN, NSUPN/NOTNUL(NSUP)*max(0.,NDEM-NFIXR/TCP));
      NUPT   = max(0., NUPTA + NUPTN + min(NDEM, NFIXR/TCP));

	  WCMAX  = (double)pGS->fPlantWaterCapacity + WCMIN;

      LEAUL  = max(0.,(NSUPN-NUPTN)*TCP-RD/(double)pGS->fProfileDepth*RN)
		       *min((RFIR-RRUL)/WCMAX/RD/10.,1.);
      LEALL  = max(0.,NNLL+(MINNLL-DENIUL)*TCP-((double)pGS->fProfileDepth-RD)/(double)pGS->fProfileDepth*RN)
		       *min(RWUG/WCMAX/((double)pGS->fProfileDepth-RD)/10.,1.);

      VOLA   = INSW (RAIN-1., 0.15, 0.) * SFERNA;
      SFERNA += RSFNA;
      RSFNA  = FERNA - SFERNA/3.;

      LAYNA  = RRD/((double)pGS->fProfileDepth-RD)*NALL;
      LAYNN  = RRD/((double)pGS->fProfileDepth-RD)*NNLL;
      LAYNA  = (double)0;
      LAYNN  = (double)0;

	 if (pMa->pNFertilizer != NULL)
     {
	  FERNA  = FCNSW((double)(pMa->pNFertilizer->iDay-(int)pTi->pSimTime->fTimeAct),0.,
		             (double)pMa->pNFertilizer->fNH4N/10.,0.);
	  FERNN  = FCNSW((double)(pMa->pNFertilizer->iDay-(int)pTi->pSimTime->fTimeAct),0.,
		             (double)pMa->pNFertilizer->fNO3N/10.,0.);
	  pCh->pCBalance->dNInputCum += (FERNA + FERNN)*(double)10;
      pCh->pCProfile->fNH4NSurf = (float)0;
      pCh->pCProfile->fNO3NSurf = (float)0;
	 }
	  /*
      FERNA  =FCNSW(FNA1T-DFS,0.,FNA1,0.)+FCNSW(FNA2T-DFS,0.,FNA2,0.)...
             +FCNSW(FNA3T-DFS,0.,FNA3,0.)+FCNSW(FNA4T-DFS,0.,FNA4,0.)...
             +FCNSW(FNA5T-DFS,0.,FNA5,0.)+FCNSW(FNA6T-DFS,0.,FNA6,0.)...
             +FCNSW(FNA7T-DFS,0.,FNA7,0.)+FCNSW(FNA8T-DFS,0.,FNA8,0.)
      FERNN  =FCNSW(FNN1T-DFS,0.,FNN1,0.)+FCNSW(FNN2T-DFS,0.,FNN2,0.)...
             +FCNSW(FNN3T-DFS,0.,FNN3,0.)+FCNSW(FNN4T-DFS,0.,FNN4,0.)...
             +FCNSW(FNN5T-DFS,0.,FNN5,0.)+FCNSW(FNN6T-DFS,0.,FNN6,0.)...
             +FCNSW(FNN7T-DFS,0.,FNN7,0.)+FCNSW(FNN8T-DFS,0.,FNN8,0.)
      */

	  //RNAUL  = FERNA+MINAUL       +LAYNA-INSW(NSWI,0.,NUPTA)-NITRUL-VOLA;
	  RNAUL  = FERNA+MINAUL       +LAYNA      -NITRUL-VOLA;
      RNALL  =       MINALL       -LAYNA      -NITRLL;
      //RNNUL  = FERNN+MINNUL+NITRUL+LAYNN-INSW(NSWI,0.,NUPTN)-DENIUL-LEAUL;
      RNNUL  = FERNN+MINNUL+NITRUL+LAYNN      -DENIUL-LEAUL;
      RNNLL  = LEAUL+MINNLL+NITRLL-LAYNN      -DENILL-LEALL;

      //state variables/pools: evaluation
	  TSOIL  += RTSOIL;

	  NAUL   += RNAUL;
      NALL   += RNALL;
      NNUL   += RNNUL;
      NNLL   += RNNLL;

	  //NA     = NAUL + NALL;
      //NN     = NNUL + NNLL;
      //NMINER = NA   + NN;

	  DPM    += RDPM;
      RPM    += RRPM;
      BIO    += RBIO;
      HUM    += RHUM;

	  DPN    += RDPN;
      RPN    += RRPN;

	  //TNLEA  = INTGRL (ZERO, LEALL)


	  /*** output ***/
	  pGS->fSoilTemp  = (float)TSOIL;//pHe->pHBalance->fProfil ???;
      pGS->fSoilTempR = (float)RTSOIL;

	  pGS->fDecPlantMatC = (float)DPM;
	  pGS->fDecPltMCRate = (float)RDPM;
      pGS->fResPlantMatC = (float)RPM;
	  pGS->fResPltMCRate = (float)RRPM;
	  pGS->fMicroBiomC   = (float)BIO;
	  pGS->fHumusC       = (float)HUM;
	  pGS->fMicBiomCRate = (float)RBIO;
	  pGS->fHumusCRate   = (float)RHUM;

	  pGS->fDecPlantMatN = (float)DPN;
      pGS->fDecPltMNRate = (float)RDPN;
	  pGS->fResPlantMatN = (float)RPN;
	  pGS->fResPltMNRate = (float)RRPN;
	  pGS->fCNPlantMat   = (float)CNDRPM;

      pGS->fNH4NContUpperLayer = (float)NAUL;
      pGS->fNH4NContLowerLayer = (float)NALL;
	  pGS->fNO3NContUpperLayer = (float)NNUL;
	  pGS->fNO3NContLowerLayer = (float)NNLL;
	  pGS->fNH4NContUpLayRate  = (float)RNAUL;
      pGS->fNH4NContLwLayRate  = (float)RNALL;
	  pGS->fNO3NContUpLayRate  = (float)RNNUL;
	  pGS->fNO3NContLwLayRate  = (float)RNNLL;

	  pGPltN->fNPlantSupply    = (float)NSUP;

	  /*** output to expertn ***/
	  fpd =(pSo->iLayers-2)*DeltaZ;

	  if (pPl!=NULL) 
		frd = min(fpd-DeltaZ,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));
	  else
		frd = (float)max(2.,(double)fparSD1)*(float)10;

	  f2  = max((float)1,(fpd-frd)/DeltaZ);

	  pCP->fN2EmisR   = (float)0;
	  pCP->fNH3VolatR = (float)(VOLA*10);
	  pCP->fNO3LeachR = (float)(LEALL*10);
	  pCP->dNO3LeachCum += LEALL*10;

      pSLN  = pCh->pCLayer->pNext;
	  if (pPl!=NULL) pPl->pPltNitrogen->fActNH4NUpt = (float)0;
	  if (pPl!=NULL) pPl->pPltNitrogen->fActNO3NUpt = (float)0;
	  for (L=1;L<=pSo->iLayers-2;L++)
      {
       if(L*DeltaZ <= frd) 
	   {
		pSLN->fNH4N         = (float)NAUL*DeltaZ/frd*(float)10;
		pSLN->fNO3N         = (float)NNUL*DeltaZ/frd*(float)10;
        pSLN->fMinerR       = ((float)MINAUL+(float)MINNUL)*DeltaZ/frd*(float)10;
		pSLN->fNH4NitrR     = (float)NITRUL*DeltaZ/frd*(float)10;
        pSLN->fNO3DenitR    = (float)DENIUL*DeltaZ/frd*(float)10;
		pCP->fN2EmisR      += pSLN->fNO3DenitR/(float)24e-5;//Denitrifikation [kg ha-1] ^= N2 Emission [ug h-1 m-2]
        if (pPl!=NULL) pLR->fActLayNH4NUpt = (float)NUPTA*DeltaZ/frd*(float)10;
        if (pPl!=NULL) pLR->fActLayNO3NUpt = (float)NUPTN*DeltaZ/frd*(float)10;
	   }
	   else if ((frd < L*DeltaZ)&&(L*DeltaZ <= frd + DeltaZ))
       {
        f1 = (L*DeltaZ-frd)/DeltaZ;
        pSLN->fNH4N         = (((float)1-f1)*(float)NAUL*DeltaZ/frd+f1*(float)NALL/f2)*(float)10;             
        pSLN->fNO3N         = (((float)1-f1)*(float)NNUL*DeltaZ/frd+f1*(float)NNLL/f2)*(float)10;
        pSLN->fMinerR       = (((float)1-f1)*((float)MINAUL+(float)MINNUL)*DeltaZ/frd
			                   +f1*((float)MINALL+(float)MINNLL)/f2)*(float)10;
        pSLN->fNH4NitrR     = (((float)1-f1)*(float)NITRUL*DeltaZ/frd+f1*(float)NITRLL/f2)*(float)10;             
        pSLN->fNO3DenitR    = (((float)1-f1)*(float)DENIUL*DeltaZ/frd+f1*(float)DENILL/f2)*(float)10;
		pCP->fN2EmisR      += pSLN->fNO3DenitR/(float)24e-5;//Denitrifikation [kg ha-1] ^= N2 Emission [ug h-1 m-2]
        if (pPl!=NULL) pLR->fActLayNH4NUpt = (((float)1-f1)*(float)NUPTA*DeltaZ/frd)*(float)10;
        if (pPl!=NULL) pLR->fActLayNO3NUpt = (((float)1-f1)*(float)NUPTN*DeltaZ/frd)*(float)10;
	   }
	   else
	   {
        pSLN->fNH4N         = (float)NALL/f2*(float)10;             
		pSLN->fNO3N         = (float)NNLL/f2*(float)10;
        pSLN->fMinerR       = (((float)MINALL+(float)MINNLL)/f2)*(float)10;
		pSLN->fNH4NitrR     = ((float)NITRLL/f2)*(float)10;
        pSLN->fNO3DenitR    = ((float)DENILL/f2)*(float)10;
		pCP->fN2EmisR      += pSLN->fNO3DenitR/(float)24e-5;//Denitrifikation [kg ha-1] ^= N2 Emission [ug h-1 m-2]
		if (pPl!=NULL) pLR->fActLayNH4NUpt = (float)0;
        if (pPl!=NULL) pLR->fActLayNO3NUpt = (float)0;
	   }//if,else if,else

	   pSLN->fCO2ProdR    = (float)RESCO2/fpd;         
	   
	   if (pPl!=NULL) pPl->pPltNitrogen->fActNH4NUpt += pLR->fActLayNH4NUpt;
       if (pPl!=NULL) pPl->pPltNitrogen->fActNO3NUpt += pLR->fActLayNO3NUpt;

       pSL =pSL->pNext;
       pSLW=pSLW->pNext;
       pSLN=pSLN->pNext;
       if((pPl!=NULL)&&(pLR->pNext!=NULL)) pLR =pLR->pNext;
	  }//for

      //pGS->fNH4NContUpperLayer -= pPl->pPltNitrogen->fActNH4NUpt/(float)10; 
      //pGS->fNO3NContUpperLayer -= pPl->pPltNitrogen->fActNO3NUpt/(float)10; 
	  //pPltN->fActNH4NUpt = (float)NUPTA*(float)10;
      //pPltN->fActNO3NUpt = (float)NUPTN*(float)10;

      if (pPl!=NULL) pPl->pPltNitrogen->fActNUpt  = pPl->pPltNitrogen->fActNH4NUpt + pPl->pPltNitrogen->fActNO3NUpt;
      if (pPl!=NULL) pPl->pPltNitrogen->fActNUptR = pPl->pPltNitrogen->fActNUpt;//per day, i.e. dt=1 !


  return 0;
}



/*************************************************************************************/
/* Procedur    :   SurfaceMineralisierung                                            */
/* Beschreibung:   Mineralisierung der Oberfl�chenpools                              */
/*                 Methode Grundlage SOILN / NITS mit Ver�nderungen                  */
/*                                                                                   */
/*              GSF/ab: Axel Berkenkamp         07.06.01                             */
/*                  ab: Ver�nderung des C-Abbaus bei Nmin-Mangel 28.08.01            */
/*                                                                                   */
/*                                                                                   */
/*                                                                                   */
/*************************************************************************************/
/*	ver�nd. Var.	pCP->fCLitterSurf          pCP->fNLitterSurf                     */
/*					pCP->fCManureSurf          pCP->fNManureSurf                     */
/*					pCP->fCHumusSurf           pCP->fNHumusSurf                      */
/*                  pCL->fCO2C                 pCP->fNH4NSurf                        */
/*					pCL->fCO2ProdR(?)          pCP->fNO3NSurf                        */
/*                  pCP->fCNLitterSurf         pCP->fCNManureSurf                    */ 
/*                  pCL->fHumusMinerR(?)       pCL->fHumusImmobR(?)                  */
/*					pCL->fLitterMinerR(?)      pCL->fLitterImmobR(?)                 */
/*                  pCL->fManureMinerR(?)      pCL->fManureImmobR(?)                 */
/*					corr.Temp                  corr.Feucht                           */
/*                                                                                   */
/*************************************************************************************/
signed short int SurfaceMiner(EXP_POINTER)
 {
  DECLARE_COMMON_VAR
  DECLARE_N_POINTER
  struct ncorr corr ={(float)1};
  
  /******************************************************************/
  /*                Variablendeklaration                            */
  /******************************************************************/

  /*Hilfsvariablen*/
  float f1,f2,f3;
  float fRedLit,fRedMan,fEffNew;
  float NoImmLit, NoImmMan;
  float fMinerEffFac,fMicBiomCN,fMinerHumFac;

  /* Konstanten (1/dt) */
  float fNH4ImmMaxK = (float)0.1;
  float fNO3ImmMaxK = (float)0.1;
  float fLitterImmK,fManureImmK;
  float fLitterToNH4K,fManureToNH4K;

  /* Raten f�r die C und N-Fl�sse (dC/dt bzw. dN/dt) */
  float fHumusMinerMaxR;  
  float fLitterMinerMaxR; 
  float fManureMinerMaxR; 
  float fNToLitterR;
  float fNH4ImmR,fNO3ImmR;
  float fCLitterToHumusR,fCLitterToLitterR,fCLitterToCO2R;
  float fCManureToHumusR,fCManureToLitterR,fCManureToCO2R;
  float fCHumusToCO2R;
  
  float fNH4ToLitterR,fNO3ToLitterR;
  float fNLitterToHumusR,fNManureToHumusR,fNManureToLitterR;
  float fNHumusToNH4R,fNLitterToNH4R,fNManureToNH4R;
  
  /*Variablen f�r die Surface-Fl�sse (eigentlich auch Raten) */
  float fCLitterSurfDecay, fCManureSurfDecay, fCHumusSurfDecay;
  float fNLitterSurfDecay, fNManureSurfDecay, fNHumusSurfDecay;

  /*Variablen f�r die Massenbilanz*/
  float fCTotalSurf,fNTotalSurf;
  float fCDiffSurf,fNDiffSurf;
  static float fCDiffSum, fNDiffSum;
  /* Variablendeklaration Ende  */




  N_ZERO_LAYER /* Setzten der Pointer auf die nullte Schicht*/


  /******************************************************************/
  /*                          Reduktionsfunktionen                  */
  /******************************************************************/
  
  /* 1. Temperatur: Ansatz SOILN, Bezugswert: Tagesmittelwert der Luft*/ 
  corr.Temp = abspower(pPA->fMinerQ10,((pCl->pWeather->fTempAve - pPA->fMinerTempB)/(float)10.0));


  /* 2. Feuchte: Ansatz RESMAN, Bezugswert: Das wassergef�llte Porenvolumen
     der obersten Bodenschicht, Optimalwert WFPS = 60% */
  f1 = (pWL->pNext->fContAct + pWL->pNext->fIce) / pSL->pNext->fPorosity;

  if (f1 < (float)0.6)
    corr.Feucht = f1 / (float)0.6;

  else
    corr.Feucht = (float) 0.6 / f1;
  /*Reduktionsfunktion Ende*/


  /******************************************************************/
  /*    C und N -Mineralisierungsraten (dC/dt bzw. dN/dt)           */
  /******************************************************************/

fHumusMinerMaxR  = pCL->pNext->fHumusMinerMaxR;
fLitterMinerMaxR = pCL->pNext->fLitterMinerMaxR;
fManureMinerMaxR = pCL->pNext->fManureMinerMaxR;
fMinerEffFac     = pPA->pNext->fMinerEffFac;
fMicBiomCN       = pCL->pNext->fMicBiomCN;
fMinerHumFac     = pPA->pNext->fMinerHumFac;

  fCHumusSurfDecay  = pCP->fCHumusSurf  * fHumusMinerMaxR  * corr.Temp * corr.Feucht;
  fCLitterSurfDecay = pCP->fCLitterSurf * fLitterMinerMaxR * corr.Temp * corr.Feucht;
  fCManureSurfDecay = pCP->fCManureSurf * fManureMinerMaxR * corr.Temp * corr.Feucht;

  fNHumusSurfDecay  = pCP->fNHumusSurf  * fHumusMinerMaxR  * corr.Temp * corr.Feucht;
  fNLitterSurfDecay = pCP->fNLitterSurf * fLitterMinerMaxR * corr.Temp * corr.Feucht;
  fNManureSurfDecay = pCP->fNManureSurf * fManureMinerMaxR * corr.Temp * corr.Feucht;
  /* C und N -Mineralisierungsraten Ende */

  /******************************************************************/
  /* Entscheidung Immobilisierung/Mineralisierung beim Litter u.    */
  /* Manure Abbau. Der Wert h�ngt ab vom CN Wert der abgebauten     */
  /* Substanz, dem CN Wert der aufgebauten Substanz (Mikrobielle    */ 
  /* Biomasse und Humus) sowie dem Effektivit�tsfaktor mit der die  */ 
  /* Mikros Kohlenstoff verwerten k�nnen.                           */
  /* Hilfsvariable positiv = Nettomineralisierung                   */
  /* Hilfsvariable negativ = Nettoimmobilisierung                   */                            
  /******************************************************************/

  /* CN-Werte */
  pCP->fCNLitterSurf = (pCP->fNLitterSurf > EPSILON)?
    	pCP->fCLitterSurf / pCP->fNLitterSurf
    	:(float)0.1; 
            
  pCP->fCNManureSurf = (pCP->fNManureSurf > EPSILON)?
    	pCP->fCManureSurf / pCP->fNManureSurf
      	:(float)0.1; 

  /*Mineralisierungs bzw. Immobilisierungsfaktor*/
   if (pCP->fCNLitterSurf > (float)0.1)
   {
    f2     = (float)1 / pCP->fCNLitterSurf - fMinerEffFac / fMicBiomCN;
	fLitterToNH4K = (f2 > 0)? f2 : 0;
    fLitterImmK   = (f2 < 0)? (float)-1 * f2 : 0;
   }

  else
   {
    fLitterToNH4K = (float)0;
    fLitterImmK   = (float)0;
   }

  if (pCP->fCNManureSurf > (float)0.1)
   {
    f3     = (float)1 / pCP->fCNManureSurf - fMinerEffFac / fMicBiomCN;
    fManureToNH4K = (f3 > 0)? f3 : 0;
    fManureImmK   = (f3 < 0)? (float)-1 * f3 : 0;
   }
  
  else
   {
    fManureToNH4K = (float)0;
    fManureImmK   = (float)0;
   }
  /* Entscheidung Immobilisierung/Mineralisierung Ende*/


  /*************************************************************************/
  /* Wenn die Immobilisierung mehr N-Bedarf entwickelt als mineralisches N */
  /* zur Verf�gung steht, wird das Wachstum der Mikroorganismen und die    */
  /* Bildung von Humus unterbunden, sowie der C-Abbau reduziert            */
  /*************************************************************************/

  fNToLitterR = fLitterImmK * fCLitterSurfDecay + fManureImmK * fCManureSurfDecay;

  /*Maximale Immobilisierungsraten*/
  fNH4ImmR = fNH4ImmMaxK * pCP->fNH4NSurf;
  fNO3ImmR = fNO3ImmMaxK * pCP->fNO3NSurf;

  if (fNToLitterR  > (fNH4ImmR + fNO3ImmR))
   {  
    if (fLitterImmK)
	 {
	  NoImmLit = (float)0.0;
      fEffNew = fMicBiomCN / pCP->fCNLitterSurf;
      fRedLit = min(fEffNew,fMinerEffFac);
      fLitterToNH4K = (float)0;
      fLitterImmK   = (float)0;
     }
	else
	 {
	  NoImmLit = (float)1.0;
	  fRedLit  = (float)1.0;
	 }

    if (fManureImmK)
	 {
	  NoImmMan = (float)0.0;
      fEffNew = fMicBiomCN / pCP->fCNManureSurf;
      fRedMan = min(fEffNew,fMinerEffFac);
      fManureToNH4K = (float)0;
      fManureImmK   = (float)0;
     }
    
	else
	 {
	  NoImmMan = (float)1.0;
	  fRedMan  = (float)1.0;
	 }

	fNToLitterR = (float)0.0; 
   }
  
  else
   {
    fRedLit = (float)1.0;
	fRedMan = (float)1.0;
	NoImmLit = (float)1.0;
	NoImmMan = (float)1.0;
   }
   
  /*Berechnung des Reduktionsfaktors alte Variante  	
  fRed = (float)1.0;                      // Reduktionsfaktor =1 => keine Reduktion 

    fRed = (fNH4ImmR + fNO3ImmR) / fNToLitterR;

  if (fLitterImmK)
  {   
   fCLitterSurfDecay = fRed * fCLitterSurfDecay;
   fNLitterSurfDecay = fRed * fNLitterSurfDecay;
  }

  if (fManureImmK)
  {
   fCManureSurfDecay = fRed * fCManureSurfDecay;
   fNManureSurfDecay = fRed * fNManureSurfDecay;
  }

  fNToLitterR        = fRed * fNToLitterR;
  Berechnung Reduktion des Abbaus Ende */


  /******************************************************************/
  /* Veraenderung des C-Pools pro Zeitschritt                       */
  /******************************************************************/
    
  /* 1. Gesamt C vor dem Zeitschritt */
  fCTotalSurf = pCP->fCHumusSurf + pCP->fCLitterSurf + pCP->fCManureSurf + pCL->fCO2C;

  /* 2. Abbau des C-Litter Pools pro Zeitschritt */
  
  if(!NoImmLit)
   {
    fCLitterToCO2R    = fCLitterSurfDecay * fRedLit;
    fCLitterToHumusR  = (float)0.0;
    fCLitterToLitterR = (float)0.0;
   }

  else
   {
    fCLitterToHumusR  = fCLitterSurfDecay * fMinerEffFac * fMinerHumFac;
    fCLitterToCO2R    = fCLitterSurfDecay * ((float)1.0 - fMinerEffFac);
    fCLitterToLitterR = fCLitterSurfDecay * fMinerEffFac * ((float)1.0 - fMinerHumFac);
   }
  
  /* 3. Abbau des C-Manure Pools pro Zeitschritt */
  
  if(!NoImmMan)
   {  
    fCManureToCO2R    = fCManureSurfDecay * fRedMan;
    fCManureToHumusR  = (float)0.0;
    fCManureToLitterR = (float)0.0;
   }

  else
   {
    fCManureToHumusR  = fCManureSurfDecay * fMinerEffFac * fMinerHumFac;
    fCManureToCO2R    = fCManureSurfDecay * ((float)1.0 - fMinerEffFac);
    fCManureToLitterR = fCManureSurfDecay * fMinerEffFac * ((float)1.0 - fMinerHumFac);
   }

  /* 4. Abbau des C-Humus Pools */
  fCHumusToCO2R     = fCHumusSurfDecay;
  
  /* 5. Zunahme im CO2 Pool */
  pCL->fCO2ProdR = fCLitterToCO2R +  fCManureToCO2R + fCHumusToCO2R;

  /* 6. Veraenderung in den C-Pools */
  pCP->fCLitterSurf -= (fCLitterToHumusR + fCLitterToCO2R - fCManureToLitterR) * DeltaT;
  pCP->fCManureSurf -= (fCManureToHumusR + fCManureToCO2R + fCManureToLitterR) * DeltaT;
  pCP->fCHumusSurf  += (fCLitterToHumusR + fCManureToHumusR - fCHumusToCO2R) * DeltaT;
  pCL->fCO2C        += pCL->fCO2ProdR * DeltaT;
  /* Ver�nderungen im C-Pool Ende */


  /********************************************************************************/
  /*      Veraenderung der N-Pools durch Mineralisierung pro Zeitschritt          */
  /********************************************************************************/
    
  /* 1. Gesamt N vor dem Zeitschritt */
  fNTotalSurf = pCP->fNHumusSurf + pCP->fNLitterSurf + pCP->fNManureSurf + pCP->fNH4NSurf + pCP->fNO3NSurf;

  /* 2. Immobilisierung im Litter-Pool */
  fNH4ToLitterR = fNToLitterR * RelAnteil(pCP->fNH4NSurf,pCP->fNO3NSurf);
  fNO3ToLitterR = fNToLitterR * RelAnteil(pCP->fNO3NSurf,pCP->fNH4NSurf);

  /* 3. Bildung von mikrobieller Biomasse */
  fNManureToLitterR = fCManureToLitterR / fMicBiomCN;


  /* 4. Humifizierung im Humus-Pool */
  if(!NoImmLit)
   fNLitterToHumusR = (float)0.0;
  else
   fNLitterToHumusR = fCLitterToHumusR / fMicBiomCN;

  if(!NoImmMan)
   fNManureToHumusR = (float)0.0;
  else
   fNManureToHumusR = fCManureToHumusR / fMicBiomCN;
   
  /* 5. Ammonium-Mineralisierung aus Humus, Litter und Manure */
  fNHumusToNH4R  = fNHumusSurfDecay;
  fNLitterToNH4R = fLitterToNH4K * fCLitterSurfDecay;
  fNManureToNH4R = fManureToNH4K * fCManureSurfDecay;

  /* 6. Ver�nderungen in den N Pools */
  pCP->fNH4NSurf += (fNHumusToNH4R  + fNLitterToNH4R  + fNManureToNH4R -  fNH4ToLitterR) * DeltaT;
  pCP->fNO3NSurf -= fNO3ToLitterR * DeltaT;
  
  pCP->fNLitterSurf += (fNToLitterR + fNManureToLitterR - fNLitterToNH4R - fNLitterToHumusR) * DeltaT;
  pCP->fNManureSurf -= (fNManureToHumusR + fNManureToLitterR + fNManureToNH4R) * DeltaT;
  pCP->fNHumusSurf += (fNLitterToHumusR + fNManureToHumusR - fNHumusToNH4R) * DeltaT;
  /* Ver�nderungen im N-Pool Ende */

  /* �bertragung auf globale Variablen */
  pCL->fNImmobR = fNToLitterR;
  pCL->fMinerR = fNHumusToNH4R + fNLitterToNH4R + fNManureToNH4R;
  pCL->fHumusMinerR  = fNHumusToNH4R;
  pCL->fLitterMinerR = fNLitterToNH4R;
  pCL->fManureMinerR = fNManureToNH4R;

  /********************************************************************************/
  /* Massenbilanz, die Summe der Ver�nderungen muss ungef�r Null ergeben */
  /********************************************************************************/
  
  fCDiffSurf = pCP->fCHumusSurf + pCP->fCLitterSurf + pCP->fCManureSurf + pCL->fCO2C - fCTotalSurf;
  fNDiffSurf = pCP->fNHumusSurf + pCP->fNLitterSurf + pCP->fNManureSurf + pCP->fNH4NSurf + pCP->fNO3NSurf - fNTotalSurf;
  /*Massenbilanz Ende*/
  
  fCDiffSum += fCDiffSurf;
  fNDiffSum += fNDiffSurf;
  return 1;
 } /*Funktion Ende*/


 /*************************************************************************************/
/* Procedur    :   StandingPoolDecrease                                              */
/* Beschreibung:   Abnahme des Standing-Pools                                        */
/*                 Grundlage RESMAN (Stott et al. 1995)                              */
/*                                                                                   */
/*              GSF/ab  Axel Berkenkamp         18.06.01                             */
/*                                                                                   */
/*************************************************************************************/
/*	ver�nd. Var.  pCh->pCProfile->fCLitterSurf    pCh->pCProfile->fNLitterSurf       */
/*				  pCh->pCProfile->fCStandCropRes  pCh->pCProfile->fNStandCropRes     */
/*                                                                                   */
/*************************************************************************************/
signed short int WINAPI StandingPoolDecrease(EXP_POINTER)
 {
  /*Hilfsvariablen*/
   float fCDecrease,fNDecrease;
	
PCPROFILE pCP = pCh->pCProfile;

  /*  Funktionsaufruf einmal t�glich */
  if (NewDay(pTi))
  {
   if (pCP->fCStandCropRes > (float)0.0 && pCP->fNStandCropRes > (float)0.0)
   {
    fCDecrease = (float)0.01 * pCP->fCStandCropRes;
    fNDecrease = (float)0.01 * pCP->fNStandCropRes;

	pCP->fCStandCropRes -= fCDecrease;
	pCh->pCProfile->fCLitterSurf += fCDecrease;

	pCP->fNStandCropRes -= fNDecrease;
	pCh->pCProfile->fNLitterSurf += fNDecrease;
   }
  }
 
 return 0;
 }

/*************************************************************************************/
/* Procedur    :   BiomassCoverRelation                                              */
/* Beschreibung:   Beziehung zwischen liegender Biomasse und der Bedeckung           */
/*                 Grundlage Steiner et al. 2000 SSSAJ 64:2109-2114                  */
/*                                                                                   */
/*              GSF/ab  Axel Berkenkamp         18.08.01                             */
/*                                                                                   */
/*************************************************************************************/
/*	ver�nd. Var.  pCh->pCProfile->fFlatCover                                         */
/*                                                                                   */
/*************************************************************************************/
signed short int WINAPI BiomassCoverRelation(EXP_POINTER)
 {
  //Hilfsvariablen
  float fCover,fCoverOld,fDryMass;
  float fkay = (float)0.0175;
	

  PCPROFILE pCP = pCh->pCProfile;
  
  //  Funktionsaufruf einmal t�glich 
  if (NewDay(pTi))
  {
  //Alter Wert
     fCoverOld = pSo->fSoilCover;
  
  //Umrechnung C-Gehalt in Biomass und kg/ha in g/m�
	 fDryMass = (pCP->fCLitterSurf + pCP->fCManureSurf + pCP->fCHumusSurf) * (float)2.5 / (float)10;
   
   //Berechnung der Beziehung
	 fCover = ((float)1.0 - (float)exp((double)(-fkay * fDryMass)));
	 //Cover = Cover-CoverOld;
	 pSo->fSoilCover = fCover;
	 
	 if(iSoilCover==(int)1) pSo->fSoilCover=fCoverOld;
  }
 
 return 0;
 }


/***********************************************************************************/
/* Procedur    :   CN_Red()                                                      */
/* Op.-System  :   DOS                                                             */
/* Beschreibung:   berechnet die Reduktionswirkung eines Faktors              */
/*                                                                                 */
/*              GSF: ch                     14.9.95                                */
/***********************************************************************************/
float WINAPI CN_Red(float x)
{
 float y = (float)0;
 
 if (x <= 10.0) y = (float)1.0;
   else if (x < 10.0 && x <= 20) y = ((float)1.0-(float)0.70)*((x-(float)20.0)/((float)10.0-(float)20.0))+(float)0.70;
     else if(x < 20.0 && x <= 50) y = ((float)0.70-(float)0.40)*((x-(float)50.0)/((float)20.0-(float)50.0))+(float)0.40;
       else if (x < 50.0 && x <= 80) y = ((float)0.40-(float)0.25)*((x-(float)80.0)/((float)50.0-(float)80.0))+(float)0.25;
         else y = ((float)0.25-(float)0.20)*((x-(float)120.0)/((float)80.-(float)120.0))+(float)0.20; 
 
 if (y < 0.2) y = (float)0.2;
  
	return y;
 }


/*********************************************************************************/
/*  Name     : MineralisationCERES                                               */
/*                                                                               */
/*  Funktion : Berechnung der Freisetzung von Stickstoff aus der frischen org.   */
/*             Substanz und der Humusfraktion des Bodens nach dem Modell CERES.  */
/*             Die im Modell NSIM integrierten Aenderungen werden                */
/*             beruecksichtigt.                                                  */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI MineralisationCERES(EXP_POINTER)
{
 signed short int  OK=0;							
 
	OK += NAusErnteRueck(exp_p);
	OK += NAusHumus(exp_p);     

  return OK;
}  /*  Ende Berechnung Mineralisation */


/*********************************************************************************/
/*  Name     : Reduktionsfunktionen                                              */
/*                                                                               */
/*  Funktion : Interne Methode zur Berechnung der Reduktionsfaktoren fuer die    */
/*             Bereiche Temperatur, Wasser und Umgebungsfaktor fuer Stickstoff   */
/*             nach dem Modellansatz CERES. Die Faktoren werden i�n jeder        */
/*             externen Funktion integriert, um die Austauschbarkeit zu          */
/*             gewaehrleisten.                                                   */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*             pCh->pCLayer->fNH4NitrCoeff                                       */
/*             pCh->pCLayer->fH2ONitrCoeff                                       */
/*             pCh->pCLayer->fTempCoeff                                          */
/*             pCh->pCLayer->fCNCoeff                                            */
/*             pCh->pCLayer->fNAnorg                                             */
/*             pCh->pCLayer->fpHCoeff                                            */
/*                                                                               */
/*********************************************************************************/
signed short int ReduktionsFunktionen(EXP_POINTER)
{
 signed short int  OK=0;							
 
	int		i1;
	float	minEvap,fakA,fakN,lufttrocken,w1,w2,t1,ntotal,ppmTokg,cn1,cn2;
   
	PWLAYER	    akt0;
	PSWATER		akt1;
	PCLAYER		akt2;
	PHLAYER		akt3;
	PSLAYER		akt4;

   /* Berechnung der Reduktionsfaktoren fuer den Feuchteeinfluss.*/
   
   
		   /* Berechnung minimale Evaporationsverluste.*/
	minEvap = (float)0.9 - ((float)0.00038 * (((pSo->pSLayer->pNext->fThickness)/10) - (float)30.0)
                      				     * (((pSo->pSLayer->pNext->fThickness)/10) - (float)30.0));

    for (akt0 = pWa->pWLayer->pNext,
         akt1 = pSo->pSWater->pNext,
         akt2 = pCh->pCLayer->pNext,
         i1=1;
                                   ((akt0->pNext != NULL)&&
                                    (akt1->pNext != NULL)&&
                                    (akt2->pNext != NULL));
         akt0 = akt0->pNext,
         akt1 = akt1->pNext,
         akt2 = akt2->pNext,
         i1++)
    {        
	/* Berechnung Feuchtefaktor fuer Ammonifizierung.*/
      lufttrocken = (akt1->fContPWP * (float)0.5);
	  if (i1 ==1) 
      	lufttrocken = (akt1->fContPWP * minEvap);

      fakA =  ((akt0->fContAct - lufttrocken) / (akt1->fContFK - lufttrocken));

	  if (akt0->fContAct > akt1->fContFK)
	  {
		w1 = ((akt0->fContAct - akt1->fContFK) / (akt1->fContSatCER - akt1->fContFK));
        fakA = (float)1.0 - w1;
	  }

	  if (fakA < (float)0.0)
	  {
	    fakA = (float)0.0;
	  }

	  akt2->fNH4NitrCoeff = fakA;

	/* Berechnung Feuchtefaktor fuer Nitrifizierung.*/


	  fakN = ((akt0->fContAct - akt1->fContPWP) / (akt1->fContFK - akt1->fContPWP));

	  if (akt0->fContAct > akt1->fContFK)
	  {
		w2 = ((akt0->fContAct - akt1->fContFK) / (akt1->fContSatCER - akt1->fContFK));
        fakN = (float)1.0 - w2;
	  }

	  if (fakN < (float)0.0)
	  {
	    fakN = (float)0.0;
	  }

	  akt2->fH2ONitrCoeff = fakN;

	}	/* Ende Berechnung Wassereinfluss.*/
   
   /* Berechnung der Reduktionsfaktoren fuer den Temperatureinfluss.*/   

    for (akt2 = pCh->pCLayer->pNext,
    	 akt3 = pHe->pHLayer->pNext;
                               ((akt2->pNext != NULL)&&
                                (akt3->pNext != NULL));                   
         akt2 = akt2->pNext,
         akt3 = akt3->pNext)

    {        
		t1 = akt3->fSoilTemp / (float)30.0;
		if (t1 < (float)0.0)
		{
		   t1 = (float)0.0;
        }

	  akt2->fTempCoeff = t1;
     }

   /* Berechnung der Reduktionsfaktoren 
	fuer den Sticktoffumgebungseinfluss.*/                                                            
 
    for (akt2 = pCh->pCLayer->pNext,
         akt4 = pSo->pSLayer->pNext;
                                   ((akt2->pNext != NULL)&&
                                    (akt4->pNext != NULL));
         akt2 = akt2->pNext,
         akt4 = akt4->pNext)
    {        

		ppmTokg = akt4->fBulkDens * (float)0.1                     /* /100 da CERES % erwartet */
					* (akt4->fThickness / 10) * ((float)1.0 - (akt4->fRockFrac / 100));
		ntotal = (akt2->fNH4N + akt2->fNO3N - ((float)2.0 / ppmTokg));
		if (ntotal < (float)0.0)
		{
		ntotal = (float)0.0;
		}
		
        cn1 = ((akt2->fCLitter) / (akt2->fNLitter + ntotal));	
		cn2 = (float)exp((double)( ((float)-0.693 * (cn1 - (float)25.0)) / (float)25.0));
		
		if (cn2 > (float)1.0)
		{
		cn2 = (float)1.0;
		}
        akt2->fCNCoeff = cn2;
        akt2->fNAnorg = ntotal;
     }   /*  Ende Berechnung Stickstoffumgebungsfaktor */


   /* Berechnung der Reduktionsfaktoren 
	fuer den pH-Wert.*/                                                            
 
    for (akt2 = pCh->pCLayer->pNext,
         akt4 = pSo->pSLayer->pNext;
                                   ((akt2->pNext != NULL)&&
                                    (akt4->pNext != NULL));
         akt2 = akt2->pNext,
         akt4 = akt4->pNext)
    {        
		akt2->fpHCoeff = (float)1.0;
		if (akt4->fpH < (float)6.0)
		{
		 akt2->fpHCoeff = ((akt4->fpH +(float)-4.5) / (float)1.5);
		}
		if (akt4->fpH > (float)8.0)
		{
		 akt2->fpHCoeff = (float)9.0 - akt4->fpH;
		}
    }/*  Ende Berechnung Reduktionsfaktor pH-Wert */

  return OK;
}  /*  Ende Berechnung Reduktionsfaktoren */


/*********************************************************************************/
/*  Name     : NAusHumus                                                         */
/*                                                                               */
/*  Funktion : Interne Methode zur Berechnung der N-Freisetzung aus der          */
/*             Humusfraktion des Bodens. Modellansatz nach CERES und Integration */
/*             der Veraenderung nach Engel&Schaaf                                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pSo->pSLayer->fHumus                                              */
/*             pSo->pSLayer->fNHumus                                             */
/*             pSo->pSLayer->fCHumus                                             */
/*             pCh->pCLayer->fNH4N                                               */
/*                                                                               */
/*********************************************************************************/
signed short int NAusHumus(EXP_POINTER)
{
 signed short int  OK=0;							                    
 
     float DeltaT  = pTi->pTimeStep->fAct;
 	 float	zerfall,nhumusverlust; /*,humusverlust,chumusverlust;*/
	
	PSLAYER	akt0;	
    PCLAYER akt1;
    PHLAYER akt2;
    PCPARAM akt3;               


	OK += ReduktionsFunktionen(exp_p);
	

    for (akt0 = pSo->pSLayer->pNext,
         akt1 = pCh->pCLayer->pNext,
         akt2 = pHe->pHLayer->pNext,
         akt3 = pCh->pCParam->pNext;
                                   ((akt0->pNext != NULL)&&
                                    (akt1->pNext != NULL)&&
                                    (akt2->pNext != NULL)&&
                                    (akt3->pNext != NULL));
         akt0 = akt0->pNext,
         akt1 = akt1->pNext,
         akt2 = akt2->pNext,         
         akt3 = akt3->pNext)
    {
	/* Zu Beginn eines jeden Zeitschrittes muessen die Raten
	   initialisiert werden, d.h auf null gesetzt werden. */
		akt1->fHumusMinerR = (float)0;

     /*
     zerfall = akt0->fNHumus * akt1->fNH4NitrCoeff * 
     					akt1->fTempCoeff * akt1->fHumusMinerR;               
	 akt1->fNHumusMinerDay = zerfall; 
     */
     
     akt1->fHumusMinerR = akt0->fNHumus * akt1->fNH4NitrCoeff * 
     					akt1->fTempCoeff * akt1->fHumusMinerMaxR; /* [kg /ha /day] */
     
     zerfall = akt1->fHumusMinerR * DeltaT;		  /* [kg /ha] */
     akt1->fMinerR+=zerfall;

     nhumusverlust = zerfall;
//!!	 humusverlust = (zerfall * (float)10.0) * 
//!!	 					((float)0.2 * (float)25.0 * akt1->fCLitterMinerR);
//	 chumusverlust = 

	 akt0->fNHumus  -= nhumusverlust;
//!!	 akt0->fHumus   -= humusverlust;
//	 akt0->fCHumus = akt0->fNHumus - chumusverlust;

	/*  Der aus demHumus freigesetzte Stickstoff wird zum
		Ammoniumpool addiert. */

	akt1->fNH4N += nhumusverlust;

    }         

  return OK;
}  /*  Ende Berechnung N-Freisetzung aus Humuspool */


/*********************************************************************************/
/*  Name     : NAusErnteRueck                                                    */
/*                                                                               */
/*  Funktion : Interne Methode zur Berechnung der N-Freisetzung aus der frischen */
/*             organischen Substanz des Bodens. Der Pool wird mir den Pflanzen-  */
/*             rueckstaenden nach der Ernte der angebauten Frucht. Die Wurzeln   */
/*			   werden auf die einzelnen Bodenschichten verteilt. Methode nach    */
/*			   CERES und veraenderung durch Schaaf&Engel intergriert             */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCh->pCLayer->afCOrgFOMFrac[]                                     */
/*             pCh->pCLayer->afNOrgFOMFrac[]                                     */
/*             pCh->pCLayer->fCLitter                                            */
/*             pCh->pCLayer->fNLitter                                            */
/*             pCh->pCLayer->fNH4N                                               */
/*             pCh->pCLayer->fCFreeFOM                                           */
/*                                                                               */
/*********************************************************************************/
signed short int NAusErnteRueck(EXP_POINTER)							 
{
 signed short int  OK=0;
 int	i1;
 float	zerfall,cfreimenge,nfreimenge,cfreiOben,nfreiOben,foM,foN,cfrei,nfrei;
 float  fCLitterMinerR;
 float  afPoolAnt[3]={(float)0.2,(float)0.7,(float)0.1}; //Kohlehydrate;Cellulose;Lignin
 float  afzerfallOben[3]={(float)0.02,(float)0.005,(float)0.00095}; //Kohlehydrate;Cellulose;Lignin
 float  DeltaT  = pTi->pTimeStep->fAct;
 

	PCLAYER	akt0;
	PSLAYER	akt1;                                                       
	
	OK += ReduktionsFunktionen(exp_p);
	
	cfreimenge = (float)0.0;
	nfreimenge = (float)0.0;
	cfreiOben = (float)0.0;
	nfreiOben = (float)0.0;                                                  
	cfrei = (float)0.0;                                                  
	nfrei = (float)0.0;                                                  

  /*  Aktualisieren der drei Fraktionen Kohlehydrate, Cellulose und Lignin
  	  der frischen organischen Substanz. Es werden auch die Fraktionen der 
  	  Oberflaechenschicht berechnet. */

    for (akt0 = pCh->pCLayer;
                      (akt0->pNext != NULL);
         akt0 = akt0->pNext)
		{
			for (i1 = 0; i1 < 3; i1++)
			{
			  akt0->afCOrgFOMFrac[i1] = ((akt0->fCLitter * afPoolAnt[i1]) / (float)0.4);
			  akt0->afNOrgFOMFrac[i1] = akt0->fNLitter * afPoolAnt[i1];
			  
			} /* Ende der Fraktionsschleife  */
		} /* Ende der schichtweisen Berechnung */                                                                 
		

  /* Zur Berechnung der Gesamtfreisetzung wird die Freisetzung jedes Pools 
  	 getrennt berechnet.Diese Berechnungen betrifft nicht die Oberflaechen-
  	 schicht (pCh->pCLayer->pNext). Deren Freisetzung wird getrennt berechnet.
  	 Die N-Freisetzung wird nicht aus einem N-Pool ermittelt sondern lediglich
  	 aus dem freigetzten Kohlenstoff unter Einbeziehung des CN-Verhaeltnisses.*/

    for (akt0 = pCh->pCLayer->pNext,
    	 akt1 = pSo->pSLayer->pNext;
                      (akt0->pNext != NULL)&&
                      (akt1->pNext != NULL);
         akt0 = akt0->pNext,
         akt1 = akt1->pNext)
		{
		
		/* Zu Beginn eines jeden Zeitschrittes muessen die Raten
		   initialisiert werden, d.h auf null gesetzt werden. */
		akt0->fNImmobR		 = (float)0;     
		akt0->fLitterMinerR  = (float)0;     
		
			for (i1 = 0; i1 < 3; i1++)
			{
     			foM = akt0->afCOrgFOMFrac[i1];
     			zerfall = akt0->fNH4NitrCoeff * akt0->fTempCoeff
     							 * akt0->fCNCoeff * akt0->afFOMDecR[i1];

		
				if (foM < (float)5.0)
				{ zerfall = (float)0.0;}
				
				akt0->afCOrgFOMFrac[i1] -= zerfall * foM * DeltaT;

				cfreimenge += (zerfall * foM);
				nfreimenge += (zerfall * afPoolAnt[i1] * akt0->afNOrgFOMFrac[i1]);
			} /* Ende der Fraktionsschleife  */

		fCLitterMinerR = cfreimenge * (float)0.4;
        akt0->fLitterMinerR = nfreimenge;
        akt0->fMinerR = nfreimenge;

		akt0->fCLitter -= 	fCLitterMinerR * DeltaT;
		akt0->fNLitter -= 	akt0->fLitterMinerR * DeltaT;

		/*  Der aus der frischen org. Substanz freigesetzte Stickstoff wird 
			zu 20% wieder im Humuspool festgelegt und zu 80 % zum Ammoniumpool
			addiert. */

 		akt1->fNHumus += ((float)0.2 * akt0->fLitterMinerR * DeltaT); //20% werden wieder immobilisiert
		akt0->fNH4N += ((float)0.8 * akt0->fLitterMinerR * DeltaT); //80% gehen in NH4-Pool.

		/*  Fuer eine korrekte N-Bilanz muss der dem Humuspool zugefuehrte Stickstoff
			der Immobilisation zugerechnet werden. */
		
		akt0->fNImmobR += ((float)0.2 * akt0->fLitterMinerR * DeltaT); //20% werden wieder immobilisiert

		/*  Fuer Berechnung naechste Schicht Loeschen */        
        cfreimenge = (float)0.0;
        nfreimenge = (float)0.0; 
        
		} /* Ende der schichtweisen Berechnung  */

	/* Die Freisetzung aus der Oberflaechenschicht (nullte Schicht) weicht
	   von der Freisetzung aus den Bodenschichten ab. Daher wird sie getrennt
	   berechnet. Die Zerfallsrate weichen ebenfalls ab. die freigesetzte 
	   Kohlenstoffmenge wird auf */

		if (pCh->pCProfile->fCLitterSurf > (float)0.0)
		{
			for (i1 = 0; i1 < 3; i1++)
			{
     			foM = pCh->pCLayer->afCOrgFOMFrac[i1];
     			foN = pCh->pCLayer->afNOrgFOMFrac[i1];
     			zerfall = pCh->pCLayer->pNext->fNH4NitrCoeff * pCh->pCLayer->pNext->fTempCoeff
     							 * pCh->pCLayer->pNext->fCNCoeff * afzerfallOben[i1];
     			cfreiOben += (foM * zerfall); 
     			nfreiOben += (foN * zerfall); 				 
			} /* Ende der Fraktionsschleife  */             
			/* Wenn Freisetzung groesser ist als Poolmenge an Oberflaeche  */             
			if ((pCh->pCProfile->fCLitterSurf - cfreiOben * DeltaT) < (float)0.0)
			{
				pCh->pCProfile->fCLitterSurf -= pCh->pCProfile->fCLitterSurf;
				pCh->pCProfile->fNLitterSurf -= pCh->pCProfile->fNLitterSurf;			
		
				fCLitterMinerR += pCh->pCProfile->fCLitterSurf;
		        pCh->pCLayer->pNext->fLitterMinerR += pCh->pCProfile->fNLitterSurf;
		
				/* Der aus der Oberflaechenschicht freigesetzte Stickstoff wird dem
				   Ammoniumpool der ersten Bodenschicht zugeschlagen. Der verbrauchte */
				pCh->pCLayer->pNext->fNH4N += pCh->pCProfile->fNLitterSurf;
			}
			else  /* Wenn Freisetzung kleiner ist als Poolmenge an Oberflaeche  */             
			{
				pCh->pCProfile->fCLitterSurf -= cfreiOben * DeltaT;
				pCh->pCProfile->fNLitterSurf -= nfreiOben * DeltaT;			
				
				fCLitterMinerR += cfreiOben;
		        pCh->pCLayer->pNext->fLitterMinerR += nfreiOben;
		
				/* Der aus der Oberflaechenschicht freigesetzte Stickstoff wird dem
				   Ammoniumpool der ersten Bodenschicht zugeschlagen. Der verbrauchte */
				pCh->pCLayer->pNext->fNH4N += nfreiOben * DeltaT;
			}      
		}

  return OK;
}  /*  Ende Berechnung N-Freisetzung aus der frischen organischen Substanz */



/*********************************************************************************/
/*  Name     : ImmobilisationCERES                                               */
/*                                                                               */
/*  Funktion : Methode zur Berechnung des immobilisierten Stickstoffs            */
/*             Modellansaetze nach CERES.                                        */
/*			   Veraenderung durch Schaaf&Engel intergriert                       */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCh->pCLayer->fNLitter                                            */
/*             pCh->pCLayer->fNH4N                                               */
/*             pCh->pCLayer->fNO3N                                               */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI ImmobilisationCERES(EXP_POINTER)							 
{
 signed short int  OK=0;
 float DeltaT  = pTi->pTimeStep->fAct;

	float	immob,defizit,potimmob;

	PCLAYER	akt0;

	immob	= (float)0.0;
	defizit	= (float)0.0;
	potimmob = (float)0.0;


	OK += ReduktionsFunktionen(exp_p);

    /*  Die Variable fNImmobR muss nicht null sein, da in der Funktion NAusErnteRueck
    	auch N in den Humuspool immobilisiert wird.   */

   for (akt0 = pCh->pCLayer->pNext;
                    (akt0->pNext != NULL);
       akt0 = akt0->pNext)
	{   
			immob = akt0->fLitterMinerR * ((float)0.35 * (akt0->fNLitter / akt0->fCLitter));

			if ((akt0->fNH4N + akt0->fNO3N) < immob)
			{ 
			 immob = (akt0->fNH4N + akt0->fNO3N)/(float)100;
			 if ( (akt0->fNH4N + akt0->fNO3N) < (float)20) immob/=(float)10;//ep 100899 
			}
			else
			{ 
			 immob = immob;
			}

			if (immob <(float)0.0)
			{ immob = (float)0.0;
			}                               
        
		
    /*  Der immobilisierte Stickstoff wird dem NH4-Pool entnommen. Steht hierzu nicht
    	genuegend N zur Verfuegung wird der Nitratpool entleert. */
	akt0->fNH4N -= immob * DeltaT;
		
		if (akt0->fNH4N < (float)0.1)
		{
		 defizit = (float)0.1 -  akt0->fNH4N;//ep 100899 Tippfehler 1.0 in 0.1 korrigiert
		 akt0->fNO3N -= defizit;
		 if (akt0->fNO3N < (float)0.1)
		 {
			akt0->fNO3N = (float)0.1;		 
		 }               
		 akt0->fNH4N = (float)0.1;
		}  //  Ende NH4 ist kleiner 0.1 


    /*  Der immobilisierte Stickstoff wird dem N-Pool der frischen
         org. Substanz zugeordnet. */
	akt0->fNLitter += immob * DeltaT;

	/*  Aktualisieren des immobilisierten Stickstoffs. */
	akt0->fNImmobR += immob * DeltaT;
			
	   }  /*  end for: schichtweise Berechnung der Immobilisation */

 
  return OK;
}  /*  Ende Berechnung der Immobilisation */


/*********************************************************************************/
/*  Name     : HydroNSIM                                                         */
/*                                                                               */
/*  Funktion : Methode zur Berechnung der Hydrolyse                              */
/*             Modellansaetze nach Engel (Modell NSIM).                          */
/*			   Veraenderung durch Schaaf&Engel intergriert                       */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCh->pCLayer->fNLitter                                            */
/*             pCh->pCLayer->fNH4N                                               */
/*             pCh->pCLayer->fNO3N                                               */
/*                                                                               */
/*********************************************************************************/
int WINAPI HydroNSIM(EXP_POINTER)							 
{

 float	wfac,tfac,minfac,minUrea,ureafac;

 PCLAYER	pCL;
 PSWATER	pSW;
 PSLAYER	pSL;
 PHLAYER	pHL;


 ReduktionsFunktionen(exp_p);


for (pCL = pCh->pCLayer->pNext,
 	 pSW = pSo->pSWater->pNext,
	 pSL = pSo->pSLayer->pNext,
	 pHL = pHe->pHLayer->pNext;
                ((pCL->pNext != NULL)&&
				 (pSW->pNext != NULL)&&
				 (pSL->pNext != NULL)&&
				 (pHL->pNext != NULL));
    pCL = pCL->pNext,
	pSW = pSW->pNext,
	pSL = pSL->pNext,
	pHL = pHL->pNext)
	{   
		wfac = pCL->fNH4NitrCoeff + (float)0.2;
		if (wfac > (float)1.0) wfac = (float)1.0;
		tfac = (pHL->fSoilTemp / (float)40.0) + (float)0.2;
		if (tfac < (float)0.0) tfac = (float)0.0;
		minfac = min(wfac,tfac);
		


		ureafac = ((float)-1.12 + ((float)1.31 * pSL->fCHumus) 
					+ ((float)0.203 * pSL->fpH)
					- ((float)1.31 * pSL->fCHumus * pSL->fpH));

		if (ureafac < (float)0.25) ureafac = (float)0.25;
		
		minUrea = ureafac * minfac * pCL->fUreaN;
		
		if (minUrea > pCL->fUreaN) minUrea = pCL->fUreaN;

		pCL->fUreaN -= minUrea;
		pCL->fNH4N += minUrea;
	
	}

  return 1;
}  /*  Ende Berechnung der Immobilisation */


/*********************************************************************************/
/*  Name     : MinerOrgDuengHoff                                                 */
/*                                                                               */
/*  Funktion : Methode zur Berechnung der Mineralisation der aus dem organischen */
/*             Duenger stammenden organischen Stickstoffs. Modell nach Hoffmann& */
/*			   Ritchie mit Anpassungen von Schaaf.                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 16.02.98                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI MinerOrgDuengHoff(EXP_POINTER)							 
{
 signed short int  OK=0;

 float minorgN,minSurforgN,lai,freeOrgSub,freeOrgC;

 PCLAYER	pCL;
 PSWATER	pSW;
 PSLAYER	pSL;
	
 /* Berechnung der Mineralisation des organischen Stickstoffs,
				- der nach Duengung an der Bodenoberflaeche liegt, 
				- der durch Versickerung und/oder Bearbeitung in den 
				  Bodenpool ueberfuehrt wurde.
		  Dies muss getrennt berechnet werden, da andere Bedingungen an 
		  der Oberflaeche und im Boden zugrunde gelegt werden.*/


 ReduktionsFunktionen(exp_p);

 minSurforgN = (float)0.0;
 minorgN = (float)0.0;
 freeOrgSub = (float)0.0;
 freeOrgC = (float)0.0;

 if (pPl != NULL)
 { lai = pPl->pCanopy->fLAI;}
 else
 { lai = (float)0.0;}


/*  Berechnung der Mineralisation aus dem an der Bodenoberflaeche 
    liegenden org. Stickstoffs. Methode wird erst gerechnet wenn erste 
    org. Duengung erfolgt ist. */
if (pCh->pCProfile->fNManureSurf > (float)0.0)
{
	minSurforgN = (float)0.035
					* ((float)1.2 - ((float)exp((double)-0.3 * (double)lai)))
					* pCh->pCProfile->fNManureSurf 
					* pCh->pCLayer->pNext->fNH4NitrCoeff
					* pCh->pCLayer->pNext->fTempCoeff;

/*  Die berechnete mineralisierte org. N-Menge wird zu 35% dem NH4-Pool
    der Oberflaeche, zu 35%  dem NH4-Pool des allgemeinen Ammoniumpools
    der ersten Bodenschicht und zu 30% dem Humuspool der ersten Bodenschicht
	zugeordnet. Neben der mineralisierten org. N-Menge muss auch die organische
	Substanz aus dem organischen Duenger vermindert werden, da bei der 
	Freisetzung des Stickstoffes Kohlenstoff veratmet wird.  */


	pCh->pCProfile->fNH4NSurf += (float)0.35 * minSurforgN;
	pCh->pCLayer->pNext->fNH4N += (float)0.35 * minSurforgN;
	pSo->pSLayer->pNext->fNHumus += (float)0.3 * minSurforgN;

	pCh->pCLayer->pNext->fManureMinerR+=(float)0.35 * minSurforgN;//ep 120999 zur Bilanzierung?

	if (pCh->pCProfile->fDryMatterManureSurf >(float)0.0)
	{
		freeOrgSub = pCh->pCProfile->fDryMatterManureSurf / 
						(pCh->pCProfile->fNManureSurf/minSurforgN);
		
		pCh->pCProfile->fDryMatterManureSurf -= freeOrgSub;
		pSo->pSLayer->pNext->fHumus += freeOrgSub;
	}
}
if (pCh->pCProfile->fCManureSurf >(float)0.0)
{
		freeOrgC = pCh->pCProfile->fCManureSurf / 
						(pCh->pCProfile->fNManureSurf/minSurforgN);
		
		pCh->pCProfile->fCManureSurf -= freeOrgC;
		pSo->pSLayer->pNext->fCHumus += freeOrgC;
}


/*  Der organische Stickstoff an der Bodenoberflaeche
	unterliegt der Versickerung in den Boden oder wird durch
	eine Bodenbearbeitung eingearbeitet. der im Boden be-
	findliche Pool unterliegt folgender Mineralisation. Methode 
	wird erst gerechnet wenn erste org. Duengung erfolgt ist. */

    ///* //sa/gsf folgendes ist in der Bilanzierung nicht erfasst, 
       //wird deshalb weggelassen
   for (pCL = pCh->pCLayer->pNext,
	    pSW = pSo->pSWater->pNext,
		pSL = pSo->pSLayer->pNext;
                    ((pCL->pNext != NULL)&&
					 (pSW->pNext != NULL)&&
					 (pSL->pNext != NULL));
       (pCL = pCL->pNext,
		pSW = pSW->pNext,
		pSL = pSL->pNext))
	{   
		minorgN = (float)0.035 *  pCL->fNManure
					* pCL->fNH4NitrCoeff
					* pCL->fTempCoeff;

		pCL->fNManure -= minorgN;
		pCL->fNH4N	  += minorgN * (float)0.7;
		pSL->fNHumus  += minorgN * (float)0.3;
		
		pCL->fManureMinerR+= minorgN * (float)0.7;//ep 120999 zur Bilanzierung?
		pCL->fNImmobR+= minorgN * (float)0.3;//ep 120999 zur Bilanzierung?
		
		
		if (minorgN > (float)0.0)
		{
			freeOrgC = pCL->fCManure / (pCL->fNManure/minorgN);
			pCL->fCManure -= freeOrgC;
			pSL->fCHumus  += freeOrgC;
		}
	}

  //*/ //sa/gsf
  return OK;
}  /*  Ende Berechnung der Mineralisation org. N aus org. Duenger*/

/*********************************************************************************/
/*  Name     : NitrOrgNH4Hoff                                                    */
/*                                                                               */
/*  Funktion : Methode zur Berechnung der Freisetzung des Ammonium aus dem       */
/*             Pool der organische Duenger in den NH4-Boden-Pool. Modell         */
/*			   nach Hoffmann& Ritchie mit Anpassungen von Schaaf.                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 16.02.98                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*             pCL->fNH4Manure                                                   */
/*             pCL->fNH4N                                                        */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI NitrOrgNH4Hoff(EXP_POINTER)							 
{
 signed short int  OK=0;

 float	minRate;

 PCLAYER	pCL;
 PSWATER	pSW;
 PSLAYER	pSL;


for (pCL = pCh->pCLayer->pNext,
 	 pSW = pSo->pSWater->pNext,
	 pSL = pSo->pSLayer->pNext;
                ((pCL->pNext != NULL)&&
				 (pSW->pNext != NULL)&&
				 (pSL->pNext != NULL));
    pCL = pCL->pNext,
	pSW = pSW->pNext,
	pSL = pSL->pNext)
	{   
		minRate = (float)0.075 
					* pCL->fNH4Manure
					* pCL->fNH4NitrCoeff
					* pCL->fTempCoeff;
		pCL->fNH4Manure -= minRate;
		pCL->fNH4N += minRate;
	}

  return OK;
}  /*  Ende Berechnung der Nitrifikation des Ammoniums aus org. Duenger*/



int iForest;
/********************************************************************************************/
/* Procedur    :   MinerDAYCENT()                                                            */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Mineralisierung                                                          */
/*                 Methode DAYCENT                                   */
/*                                                                                          */
/*                 HMGU/sb & ep              2009                                           */
/*                                                                                          */
/********************************************************************************************/
/* ver�nd. Var.		pCL->f...             pCL->f...                                         */
/********************************************************************************************/

int WINAPI MinerDAYCENT(EXP_POINTER)// returns total daily NNM [g/m2] = mineralisation - immobilisation
{
// used in loops over all layers
PCLAYER		pCL;
PSLAYER		pSL;
PWLAYER		pWL;
PHLAYER		pHL;

int const TimeStepsPerDay=4;
float dtDecompDC=(float)1.0f/365.f/TimeStepsPerDay;	//fraction of year per timestep
static struct fixedparameter fixed;
//static struct parameter par;
float C=0.0, N=0.0, lig=0.0, Navail=0.0, oldlig=0.0, oldsC, sC=0.0, sN=0.0, newlig=0.0, mC=0.0, mN=0.0, ret; //partition litter into struc/metab
float tcflow; //total c flow
float defacSRFC, anerb;
int i,iday;
float *pdecf;
float rneww1[2],								//ratio C/N fine branches to: [0]SOM1 [1]SOM2
	  rneww2[2],								//ratio C/N large wood    to: [0]SOM1 [1]SOM2
	  rneww3[2],								//ratio C/N coarse roots  to: [0]SOM1 [1]SOM2
	  rnewas[2],								//ratio C/N above ground structural  to: [0]SOM1 [1]SOM2
	  rnewbs[2];								//ratio C/N below ground structural  to: [0]SOM1 [1]SOM2
float radds1;
float CtoSOM2, CtoSOM1, co2loss;
float orgNflow=0.0, minNflow=0.0;		
float mineralFlow;// Mineralization associated with respiration
float cfmes1,rcemet,cfsfs2,rceto2,rceto3,rceto1,cfs1s3,cfs1s2,cfs2s1,cfs2s3,cfs3s1;
float microbeC; //C flow out of surface microbe pool 
float efftext;
float frNH4, frNO3;
float dummy;
float MinByWood,MinByStructLitter,MinByMetabLitter,MinBySOM; //Mineralisation by pools [kg/ha]
float ImmByWood,ImmByStructLitter,ImmByMetabLitter,ImmByInput,ImmBySOM; //Immobilisation by pools [kg/ha]
float fDrain =(float)1.0; 
//FILE *outf;

if(SimulationStarted)
{
//set fixed parameters (values from Century file 'ffixed.100' for forests) 
	fixed = SetFixedParameters();
//read site parameter from 'century.cfg' file
	//par = ReadParameter();

//********************************************************************************************
 // Initialize pools
//********************************************************************************************
 // A) Wood
	//wood 1: fine branch //aus xnm file Marke 20000 [kg/ha]
		//pCh->pCProfile->fCBranchLitterSurf = par.wood1C;
		//pCh->pCProfile->fNBranchLitterSurf = par.wood1C / par.cerfor[2][FBRCH];//calciv_dc line 216 
	//wood 2: large wood  //aus xnm file Marke 20000 [kg/ha]
		//pCh->pCProfile->fCStemLitterSurf = par.wood2C;
		//pCh->pCProfile->fNStemLitterSurf = par.wood2C / par.cerfor[2][LWOOD];//calciv_dc line 216 
	//wood 3: coarse roots //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCGrossRootLitter = pCL->fCManure;
			pCL->fNGrossRootLitter = pCL->fNManure;
		}
 // B) Litter
	//Belowground = fine root litter //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCFineRootLitter = pCL->fCLitter;
			pCL->fNFineRootLitter = pCL->fNLitter /(float)3.0; //Adjustment from workbook 2.b.
		}
	//Aboveground = leaves litter	//aus xnm file Marke 20004
	 pCh->pCProfile->fCLeafLitterSurf = pCh->pCProfile->fCLitterSurf;
	 pCh->pCProfile->fNLeafLitterSurf = pCh->pCProfile->fNLitterSurf;

	//SOM soil //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCHumusFast   = pSL->fCHumus * 0.03f;	//percentages from century workbook Ch. II
			pCL->fNHumusFast   = pSL->fNHumus * 0.03f / 0.7f;   
			pCL->fCHumusSlow   = pSL->fCHumus * 0.65f;  
			pCL->fNHumusSlow   = pSL->fNHumus * 0.65f / 1.1615f;//neu berechnet, so dass alles N aufgeteilt ist  
			pCL->fCHumusStable = pSL->fCHumus * 0.32f;  
			pCL->fNHumusStable = pSL->fNHumus * 0.32f / 0.7f;  
		}
	//SOM1 srfc //aus xnm file Marke 20004
		pCh->pCProfile->fCMicLitterSurf = pCh->pCProfile->fCHumusSurf;
		pCh->pCProfile->fNMicLitterSurf = pCh->pCProfile->fNHumusSurf;
}

if (NewDay(pTi))
{

//********************************************************************************************
// 0) Initialize 
//********************************************************************************************

	for(pCL=pCh->pCLayer;pCL->pNext!=NULL;pCL=pCL->pNext)	 
	{	  
		pCL->fMinerR = 0.0;
		pCL->fNImmobR = 0.0;
		pCL->fCO2ProdR = 0.0;
		pCL->fCO2C= 0.0;
		pCL->fHumusMinerR = 0.0;
		pCL->fLitterMinerR = 0.0;
		pCL->fNLitterImmobR = 0.0;
	}
	pCh->pCProfile->fNHumusImmobDay = 0.0;
	MinByWood=MinByStructLitter=MinByMetabLitter=MinBySOM= 0.0; 
	ImmByWood=ImmByStructLitter=ImmByMetabLitter=ImmByInput=ImmBySOM= 0.0; 
	//DailyCorrection=0.0;

//********************************************************************************************
// I) Calculate temperature and water factor for decomposition, do this daily 
//********************************************************************************************
//	a) Surface pools: defacSRFC, anerb 
		decf20cm(&defacSRFC, &anerb, fixed, fDrain, exp_p);
//	b) Soil layers:   pdecf[i]
		pdecf =(float *)malloc((pSo->iLayers-2) * sizeof (float));
		for(i=0,pSL=pSo->pSLayer->pNext,pHL=pHe->pHLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pHL=pHL->pNext, pWL=pWL->pNext, i++)
		{
			pdecf[i]=decfSL(pSL->fSand,(pWL->fContAct/pSL->fPorosity),pHL->fSoilTemp,fixed,exp_p);
		}

//********************************************************************************************


//********************************************************************************************
// II) Input litter (leaves to surface, fine roots to soil)
//********************************************************************************************
// a) surface litter
 		C=pCh->pCProfile->fCLeafLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNLeafLitterSurf * kgphaTOgpm2;
		lig=pCh->pCProfile->fLigFracLeaf;
		Navail=0.0; //no direct adsorbtion in surface litter
		oldlig = pCh->pCProfile->fLignFracStrLitSurf;
		oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
		if(C>0.0f && N>0.0f) 
		{
			ret = PartitionResidue(C,N/C,lig,SRFC,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,fixed); 		
			pCh->pCProfile->fCLeafLitterSurf = 0.0; //all litter is now in struc/metab pools
			pCh->pCProfile->fNLeafLitterSurf = 0.0;
			pCh->pCProfile->fCStrLitterSurf += sC * gpm2TOkgpha;
			pCh->pCProfile->fNStrLitterSurf += sN * gpm2TOkgpha;
			pCh->pCProfile->fLignFracStrLitSurf = newlig;
			pCh->pCProfile->fCMtbLitterSurf += mC * gpm2TOkgpha;
			pCh->pCProfile->fNMtbLitterSurf += mN * gpm2TOkgpha;
		}
	 
// b) soil litter (fine roots)
		for(i=0,pCL=pCh->pCLayer->pNext; pCL->pNext!=NULL;pCL=pCL->pNext,i++)
		{
 			C=pCL->fCFineRootLitter * kgphaTOgpm2;
			N=pCL->fNFineRootLitter * kgphaTOgpm2;								
			lig=pCh->pCProfile->fLigFracFineRoot;
			Navail=(pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //direct adsorbtion from first 20 cm 
			oldlig = pCh->pCProfile->fLignFracStrLitSurf;
			oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
			if(C>0.0f && N>0.0f) 
			{
				ret = PartitionResidue(C,N/C,lig,SOIL,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,fixed); 			
				//direct adsorbtion of mineral N
				if(ret>0.0)
				{
				ret = min (ret, (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
				frNH4 = pCL->fNH4N / (pCL->fNO3N + pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
				frNO3 = pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N); 
				pCL->fNH4N -= frNH4  
							* ret * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
				pCL->fNO3N -= frNO3
							* ret * gpm2TOkgpha;
				pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
				pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
				pCL->fNImmobR += ret * gpm2TOkgpha;
				ImmByInput += ret * gpm2TOkgpha;
				if(pCL->fNO3N <0.0 ) Message(1,"Warning: NO3 @ input negative!");	 
				if(pCL->fNH4N <0.0 ) Message(1,"Warning: NH4N @ input negative!");	 
	
				}
				//all litter is now in struc/metab pools
				pCL->fCFineRootLitter = 0.0; 
				pCL->fNFineRootLitter = 0.0;
				pCL->fCStrcLitter += sC * gpm2TOkgpha;
				pCL->fNStrcLitter += sN * gpm2TOkgpha;
				pCL->fLignFracStrcLit  = newlig;
				pCL->fCMtbLitter += mC * gpm2TOkgpha;
				pCL->fNMtbLitter += mN * gpm2TOkgpha;
			}
		}//loop over soil layers

//************************************************************************************************
// IV)  Calculates the decomposition ratio.
// 	Determine C/N of new material entering 'Box B'. 	
// 	The C/N ratios for structural and wood are computed once per day here;
//************************************************************************************************
if(iForest == 1) //forest
{
// Fine branch
// Ratio for new SOM1 from decomposition of Fine Branches
		C=pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
		rneww1[0]= AboveGroundDecompRatio(C,N,2.0f,fixed);
// Ratio for new SOM2 from decomposition of Fine Branches
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
		radds1=fixed.rad1p[0] + fixed.rad1p[1] * 
			( AboveGroundDecompRatio (C,N,2.5f,fixed)- fixed.pcemic[1]);
		rneww1[1]= rneww1[0]+ radds1;
// Large Wood
// Ratio for new SOM1 from decomposition of Large Wood
		C=pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
	    rneww2[0] =AboveGroundDecompRatio(C,N,2.0f,fixed);
// Ratio for new SOM2 from decomposition of Large Wood
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
	    rneww2[1] = rneww2[0] + fixed.rad1p[0] + fixed.rad1p[1] * 
			( AboveGroundDecompRatio (C,N,2.0f,fixed)- fixed.pcemic[1]);
	    rneww2[1] =  max (rneww2[1],fixed.rad1p[2]);
// Coarse Roots
// Ratio for new SOM1 from decomposition of Coarse Roots
		rneww3[0] = fixed.varat1[0];
// Ratio for new SOM2 from decomposition of Coarse Roots
		rneww3[1] = fixed.varat2[1]; 
}//forest
// Surface Structural
// Ratios for new som1 from decomposition of AG structural
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
		rnewas[0] = AboveGroundDecompRatio(C,N,2.5f,fixed);
// Ratio for new SOM2 from decomposition of AG strutural
		radds1=fixed.rad1p[0] + fixed.rad1p[1] * 
		( AboveGroundDecompRatio (C,N,2.5f,fixed)- fixed.pcemic[1]);
		rnewas[1] =max( fixed.rad1p[2], rnewas[0] + radds1);
// Soil Structural
// Ratios for new som1 from decomposition of BG structural
		rnewbs[0] = fixed.varat1[0];
// Ratio for new SOM2 from decomposition of BG strutural
		rnewbs[1] = fixed.varat2[0];


//   ***************************************************************************************
//		Decomposition Routines (CALL: simsom_dc.cpp line 280), do 4 times a day
//		 IV) DecomposeLitter  
//		  V) DecomposeWood  
//		 VI) DecomposeSOM  
//				Steps:       
//				1. Surface SOM1 decomposes to SOM2 with CO2 loss.    
//				2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss and leaching.         
//				3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss       
//				4. SOM3 decomposes to soil SOM1 with CO2 loss.
//   ***************************************************************************************
 
for(iday=1;iday<=TimeStepsPerDay;iday++)  //4 times a day
{
//************************************************************************************************
// IV) Decompose Litter
//************************************************************************************************

//SURFACE STRUCTURAL Material to SOM1[SRFC] and SOM2
	C		= pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
	lig		= pCh->pCProfile->fLignFracStrLitSurf;
	Navail  = getNtoDepth(20.0f,exp_p);  //top 20cm 
	if (C >= 10e-7) 
	{
	tcflow=min(C , fixed.strmax[SRFC]* defacSRFC * fixed.dec1[SRFC] //C flow from A to B
		       * (float)exp(-fixed.pligst[SRFC]*lig) ) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rnewas[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCStrLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;   
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rnewas[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,  20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow, 20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByStructLitter += minNflow * gpm2TOkgpha; ;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( (float)ret, 20.0f, exp_p);			//B += minflow
		ImmByStructLitter +=  ret * gpm2TOkgpha; ;
		} 

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
	 
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rnewas[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByStructLitter += minNflow * gpm2TOkgpha; ;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByStructLitter +=  ret * gpm2TOkgpha; ;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7

//SOIL STRUCTURAL Material to SOM1[SOIL] and SOM2
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
		pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCStrcLitter * kgphaTOgpm2; 
	N		= pCL->fNStrcLitter * kgphaTOgpm2; 
	lig		= pCL->fLignFracStrcLit;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
	if (C >= 10e-7) 
	{
	tcflow=min(C , fixed.strmax[SOIL]* pdecf[i] *  anerb  * fixed.dec1[SOIL] //C flow from A to B
		       * (float)exp(-fixed.pligst[SOIL]*lig) ) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rnewbs[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCL->fCStrcLitter -= tcflow * gpm2TOkgpha;
		pCL->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rnewbs[1],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		MinByStructLitter += minNflow * gpm2TOkgpha; ;	 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if (minNflow > 1e-7)
		{
		pCL->fCHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4 *  (float)fabs(minNflow) * gpm2TOkgpha; //Mineral -= minflow
		pCL->fNO3N -= frNO3 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		pCL->fNImmobR  +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		ImmByStructLitter +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		}
		}
 
		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SOIL];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
		 
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast += CtoSOM1 * gpm2TOkgpha;;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByStructLitter += minNflow * gpm2TOkgpha;  
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral -= minflow				
		pCL->fNO3N -= frNO3 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByStructLitter +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha; 
 		}
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7
	}//loop over all layer

 
//METABOLIC surface to SOM1
	C = pCh->pCProfile->fCMtbLitterSurf * kgphaTOgpm2;
	N = pCh->pCProfile->fNMtbLitterSurf * kgphaTOgpm2;
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-6)
	{
	// Determine C/E ratios for flows to SOM1
		rcemet = AboveGroundDecompRatio(C,N,2.5f,fixed);

	// If decomposition can occur
	if (CanDecompose(C,N,rcemet,Navail) )
	{
		// C Flow
		// Compute total C flow out of metabolic into SOM1[layer]
		tcflow = C * defacSRFC * fixed.dec2[SRFC] * anerb * dtDecompDC;
		// Make sure metab does not go negative.
		tcflow = min ( tcflow, C );
		co2loss = tcflow * fixed.pmco2[SRFC];

		cfmes1 = tcflow - co2loss;
		pCh->pCProfile->fCMtbLitterSurf -= cfmes1 * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM1(cfmes1,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNMtbLitterSurf -= mineralFlow * gpm2TOkgpha; 
		MinByMetabLitter += mineralFlow * gpm2TOkgpha; 
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(cfmes1,rcemet,C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNMtbLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM1(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNMtbLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByMetabLitter += minNflow * gpm2TOkgpha;
		}
	if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow), 20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM1( ret, 20.0f, exp_p);			//B += minflow
		ImmByMetabLitter += ret * gpm2TOkgpha;
		}
	}// if candecomp
	} //if (pCh->pCProfile->fCMtbLitterSurf >= 10e-6)


//METABOLIC soil to SOM1
	// Determine C/E ratios for flows to SOM1
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	if(getNtoDepth(20.0f,exp_p) <= 0.0f)			rcemet=fixed.varat1[0]; //set ratio to max. allowed
 	else if(getNtoDepth(20.0f,exp_p) <= fixed.varat1[2])	rcemet=fixed.varat1[1]; //set ratio to min. allowed
	else	//  aminrl > 0 and <= varat(2,element)
	rcemet=(1.0f - getNtoDepth(20.0f,exp_p)/fixed.varat1[2])*(fixed.varat1[0]-fixed.varat1[1])+fixed.varat1[1];

	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCMtbLitter * kgphaTOgpm2;
	N		= pCL->fNMtbLitter * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; // 
	if (C >= 10e-7)
	{	
	// If decomposition can occur
	if (CanDecompose(C,N,rcemet,Navail) )
		{
		// C Flow
		// Compute total C flow out of metabolic into SOM1[layer]
		tcflow = C * pdecf[i] * fixed.dec2[SOIL] * anerb * dtDecompDC;
		// Make sure metab does not go negative.
		tcflow = min ( tcflow, C );	    //gross  C flow to SOM1
		if(tcflow > 10e-7)
		{
		co2loss=tcflow * fixed.pmco2[SOIL];
		tcflow -= co2loss;					//net  C flow to SOM1
		 
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast  += tcflow * gpm2TOkgpha; 
		pCL->fCMtbLitter  -= tcflow * gpm2TOkgpha; 
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNMtbLitter -= mineralFlow * gpm2TOkgpha; 
		MinByMetabLitter += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(tcflow,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNMtbLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNMtbLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByMetabLitter += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByMetabLitter +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		}//if(CtoSOM1 > 10e-7)
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)
	} // loop over all layer

//_______________________________________________________________________________________

//		 V) DecomposeWood  
//_______________________________________________________________________________________

if(iForest == 1) //forest
{
// FINE BRANCHES
	C		= pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
	lig		= pCh->pCProfile->fLigFracFineRoot;
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-7) 
	{
	tcflow=C * defacSRFC * pCh->pCProfile->fFineBranchDecR * (float)exp(-fixed.pligst[SRFC]*lig) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww1[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCBranchLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww1[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( ret,20.0f, exp_p);			//B += minflow	
		ImmByWood +=  ret * gpm2TOkgpha;
		} 
	
		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1

		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww1[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 

// LARGE WOOD	
	C		= pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
	lig		= pCh->pCProfile->fLigFracStem;
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-7) 
	{
	tcflow= C * defacSRFC * pCh->pCProfile->fLargeWoodDecR * (float)exp(-fixed.pligst[SRFC]*lig) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww2[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCStemLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		mineralFlow = FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww2[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation 
		{
		pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( ret, 20.0f, exp_p);			//B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		} 

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
	 
		// C flow
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww2[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 

// COARSE ROOTS	
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
		pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCGrossRootLitter * kgphaTOgpm2;
	N		= pCL->fNGrossRootLitter * kgphaTOgpm2;
	dummy=C/N;

	lig		= pCh->pCProfile->fLigFracCoarseRoot;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
	if (C >= 10e-7) 
	{
	tcflow= C * pdecf[i] * pCh->pCProfile->fCoarseRootDecR * (float)exp(-fixed.pligst[SOIL]* lig) * dtDecompDC * anerb;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww3[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		// C flow
		pCL->fCGrossRootLitter -= tcflow * gpm2TOkgpha;
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww3[1],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByWood += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByWood +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SOIL];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
		 
		// C flow
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast  += CtoSOM1 * gpm2TOkgpha;;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww3[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByWood += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByWood +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 
	}//loop over all layer
}//if site is a forest
//_______________________________________________________________________________________
//		 VI) DecomposeSOM (dtDecomp, anerb, defac);
//_______________________________________________________________________________________

//	VI) 1. Surface SOM1 decomposes to SOM2 with CO2 loss.   
	C		= pCh->pCProfile->fCMicLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNMicLitterSurf * kgphaTOgpm2;
	Navail  = getNtoDepth(20.0f,exp_p);
if( C >= 10e-7 ) 
{
//	determine C/N ratios for flows to SOM2
	rceto2=C/N + fixed.rad1p[0] +  fixed.rad1p[1]*(C/N - fixed.pcemic[1]);
	rceto2=max(rceto2,fixed.rad1p[2]);
//  If decomposition can occur, schedule flows associated with /
//  respiration and decomposition
	if (CanDecompose (C, N , rceto2, Navail) )
	{
		// Decompose Box A to SOM2
		// -----------------------
		// Calculate C Flow
		// total C flow out of surface microbes.
		microbeC=C * defacSRFC * fixed.dec3[SRFC] * dtDecompDC;    
		co2loss = microbeC * fixed.p1co2a[SRFC];
		// cfsfs2 is C Flow from SurFace som1 to Som2
		cfsfs2 = microbeC - co2loss;

		//C flow
		pCh->pCProfile->fCMicLitterSurf -= microbeC * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(cfsfs2,20.0f, exp_p);
		
		//Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		mineralFlow = FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNMicLitterSurf -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha;
		//N flow from A to B.
		ScheduleNFlow(cfsfs2,rceto2,C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNMicLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);	 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNMicLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinBySOM += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N		 
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow	
		FlowNintoSOM2( ret, 20.0f, exp_p);			//B += minflow
		ImmBySOM +=  ret * gpm2TOkgpha;
		}
	}//if canDecompose
} // if amount is significant

 

//	VI) 2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss

	// Determine C/E ratios for flows to SOM2 and SOM3  
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	rceto2=rceto3=0.0f;
	if(getNtoDepth(20.0f, exp_p) <=0.0f ){rceto2 = fixed.varat2[0];	rceto3 = fixed.varat3[0];} //set to maximum allowed ??<=??
	else if (getNtoDepth(20.0f, exp_p) > fixed.varat2[2]){
		rceto2 = fixed.varat2[1];rceto3 = fixed.varat3[1];} //set to minimal allowed
	else // >0 and <= varat2_2
	{
		rceto2 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat2[2])*(fixed.varat2[0]-fixed.varat2[1])+fixed.varat2[1];
		rceto3 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat3[2])*(fixed.varat3[0]-fixed.varat3[1])+fixed.varat3[1];
	}
	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusFast * kgphaTOgpm2;
	N		= pCL->fNHumusFast * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; 
	if (C >= 10e-7)
	{	
	// If soil som1 can decompose to som2, it will also go to som3.
	// If it can't go to som2, it can't decompose at all.
	// If decomposition can occur
	if (CanDecompose(C,N,rceto2,Navail) )
		{
		//C Flow
		efftext = fixed.peftxa + fixed.peftxb * pSL->fSand*0.01f;
		microbeC =C * pdecf[i] * fixed.dec3[SOIL] * dtDecompDC * anerb * efftext;//cltfac=1 for no cultivation
		co2loss = microbeC * fixed.p1co2a[SOIL];
		cfs1s3 = microbeC * (fixed.ps1s3[0]+fixed.ps1s3[1]*pSL->fClay*0.01f) *
	    			(fixed.animp * (1.0f - anerb) + 1.0f); // C to SOM3 	
			// SOM2 gets what's left of microbeC.
		cfs1s2 = microbeC - co2loss - cfs1s3;
		//C flow  
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusStable  += cfs1s3 * gpm2TOkgpha;
		pCL->fCHumusSlow  += cfs1s2 * gpm2TOkgpha;
		pCL->fCHumusFast   -= microbeC * gpm2TOkgpha; 		
 		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusFast -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// to SOM3
		// N flow from A to B.
		ScheduleNFlow(cfs1s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
	    pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusStable  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		
		//to SOM2
		//N flow from A to B.
		ScheduleNFlow(cfs1s2,rceto2,C,N,Navail, &orgNflow, &minNflow);
		pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;	//B += minflow
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;	//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;		
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-7)
	} // loop over all layer


//	VI)	3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss   

	// Determine C/E ratios for flows to SOM1 and SOM3  
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	rceto1= 0.0f;rceto3= 0.0f;
	if(getNtoDepth(20.0f, exp_p) <=0.0f ){rceto1 = fixed.varat1[0];rceto3 = fixed.varat3[0];}  //set to maximum allowed  
	else if (getNtoDepth(20.0f, exp_p) > fixed.varat1[2]){ rceto1 = fixed.varat1[1]; rceto3 = fixed.varat3[1];}  //set to minimal allowed
	else // >0 and <= varat1_2
	{	rceto1 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat1[2])*(fixed.varat1[0]-fixed.varat1[1])+fixed.varat1[1];
		rceto3 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat3[2])*(fixed.varat3[0]-fixed.varat3[1])+fixed.varat3[1];
	}

	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusSlow * kgphaTOgpm2;
	N		= pCL->fNHumusSlow * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; 
	if (C >= 10e-7)
	{	
	// If soil som2 can decompose to som1, it will also go to som3.
	// If it can't go to som1, it can't decompose at all.
	// If decomposition can occur
	if (CanDecompose(C,N,rceto1,Navail) )
		{
		// C Flow
		tcflow=C * pdecf[i] * fixed.dec5 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
		co2loss = tcflow * fixed.p2co2;
		cfs2s3 = tcflow * (fixed.ps2s3[0]+fixed.ps2s3[1]*pSL->fClay*0.01f) *
	    (fixed.animp * (1.0f - anerb) + 1.0f);
		cfs2s1 = tcflow - co2loss - cfs2s3;//SOM1 gets what's left of tcflow.
		// C flow  
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusFast    += cfs2s1 * gpm2TOkgpha;
		pCL->fCHumusStable  += cfs2s3 * gpm2TOkgpha;
		pCL->fCHumusSlow    -= tcflow * gpm2TOkgpha; 		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;	
		mineralFlow = min(mineralFlow, pCL->fNHumusSlow * kgphaTOgpm2);
		pCL->fNH4N			+= mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusSlow	-= mineralFlow * gpm2TOkgpha; 
		MinBySOM			+= mineralFlow * gpm2TOkgpha;
		pCL->fMinerR		+= mineralFlow * gpm2TOkgpha ;
		//SB! Fehler in N Bilanz im Logfile, nicht in der Grafischen Ausgabe,
		//nur hier... funktioniert, wenn oberen 4 Zeilen auskommentiert sind.
		//Fehler erscheint sogar t�glich, wenn die unteren beiden Zeilen auskommentiert werden ???

		//pCL->fNH4N	+= 0.1;
		//pCL->fMinerR	+= 0.1;

		if (pCL->fNO3N < 0)		Message(1,"Warning: neg. Mineral NO3N 3a");		
		if (pCL->fNH4N < 0)		Message(1,"Warning: neg. Mineral NH4N 3a");
		//to SOM3
		// N flow from A to B.
		ScheduleNFlow(cfs2s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusStable  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}

		//to SOM1
		// N flow from A to B.
		ScheduleNFlow(cfs2s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if(pTi->pSimTime->iJulianDay==245 && iday==2 && i==15)
		{
			int dummy=666;
		}

		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4   //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}		
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-7)
 
	} // loop over all layer


//	IV)	4. SOM3 decomposes to soil SOM1 with CO2 loss.

	// Determine C/E ratios for flows to SOM1 : already done
	//loop
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusStable * kgphaTOgpm2;
	N		= pCL->fNHumusStable * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //

	if (C >= 10e-7)
	{	
	// If decomposition can occur
	if (CanDecompose(C,N,rceto1,Navail) )
		{
		// Calculate C Flow
		// C flow out of metabolic into SOM1[layer]
		tcflow = C * pdecf[i] * fixed.dec4 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
		co2loss=tcflow * fixed.p3co2;
		cfs3s1 = tcflow - co2loss;
		 
		// C flow
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusFast   += cfs3s1 * gpm2TOkgpha;;
		pCL->fCHumusStable  -= tcflow * gpm2TOkgpha;;	
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow = min (mineralFlow,pCL->fNHumusStable * kgphaTOgpm2 );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusStable -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// N flow from A to B.
		ScheduleNFlow(cfs3s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusStable -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusStable -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  += (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							* (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							* (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM += (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR += (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)

	if (pCL->fNO3N < 0)		Message(1,"Warning: neg. Mineral NO3N ");		
	if (pCL->fNH4N < 0)		Message(1,"Warning: neg. Mineral NH4N ");
 
	} // loop over all layer

} //4 times a day

//*******************************************************************************
//CEAN UP HEAP
free(pdecf);
//*******************************************************************************

	
//*******************************************************************************
//	write ExpertN variables 
//	for result files and Denitrification modul
//*******************************************************************************
//write mineralisation/immobilisation by pools into first real XN layer
//values could be calculated per layer, but thats not implemented yet.
pCh->pCLayer->pNext->fHumusMinerR   = MinBySOM ;
pCh->pCLayer->pNext->fLitterMinerR  = MinByStructLitter + MinByMetabLitter+ MinByWood;	 
// fNHumusImmobR is not summed up in XN, use Day instead!!
pCh->pCProfile->fNHumusImmobDay    = ImmBySOM ;
pCh->pCLayer->pNext->fNLitterImmobR = ImmByInput + ImmByStructLitter
										 + ImmByMetabLitter+ ImmByWood;// - DailyCorrection;
//write humus and FOS f�r Ceres-N Denitrifikation
for(pCL=pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext ; pSL->pNext != NULL ;
												pSL=pSL->pNext,pCL=pCL->pNext) 
{
		pSL->fCHumus = pCL->fCHumusSlow + pCL->fCHumusFast + pCL->fCHumusStable;
		//like Mineralisation_Fagus: just the fine root litter is written to FOM[i], no coarse roots
		pCL->afCOrgFOMFrac[1]= pCL->fCMtbLitter + pCL->fCStrcLitter; 
}

//*******************************************************************************
// write resultfile: "century.out"
//******************************************************************************* 
//open file
//if(outf==NULL) ... exception handling
/*
if (pTi->pSimTime->fTimeAct == 0)
	outf = fopen("result/century.out", "w");	//Create file
else outf = fopen("result/century.out", "a+");	//append  

fprintf(outf, "%f ",pTi->pSimTime->fTimeAct);
//Variables
//Profile
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fNImmobR ); 
	fprintf(outf, "%f ",ImmByInput );
	fprintf(outf, "%f ",ImmBySOM );
	fprintf(outf, "%f ",ImmByStructLitter );
	fprintf(outf, "%f ",ImmByMetabLitter );
	fprintf(outf, "%f ",ImmByWood );
	//8
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fMinerR ); 
	fprintf(outf, "%f ",MinBySOM );
	fprintf(outf, "%f ",MinByStructLitter );
	fprintf(outf, "%f ",MinByMetabLitter );
	fprintf(outf, "%f ",MinByWood );
	//13ff. NNM
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fMinerR - pCh->pCLayer->pNext->fNImmobR );
	fprintf(outf, "%f ",MinBySOM - ImmBySOM);
	fprintf(outf, "%f ",MinByStructLitter -ImmByStructLitter);
	fprintf(outf, "%f ",MinByMetabLitter -ImmByMetabLitter);
	fprintf(outf, "%f ",MinByWood -ImmByWood);
	//18: CumCorrection
	fprintf(outf, "%f ",CumCorrection);
	//new line
	fprintf(outf, "\n"); 
	//close file
	fclose(outf);
*/
}// if new day
return 1;
}
//************ end of function MinerDAYCENT****************


struct fixedparameter SetFixedParameters()
{
struct fixedparameter fixed;
	fixed.aneref[0]=1.5f;
	fixed.aneref[1]=3.0f;
	fixed.aneref[2]=0.3f;
	fixed.pprpts[0]=0.0f;
	fixed.pprpts[1]=1.0f;
	fixed.pprpts[2]=0.8f;
	fixed.varat1[0]=18.0f;
	fixed.varat1[1]=8.0f;
	fixed.varat1[2]=2.0f;
	fixed.varat2[0]=40.0f;
	fixed.varat2[1]=12.0f;
	fixed.varat2[2]=2.0f;
	fixed.varat3[0]=20.0f;
	fixed.varat3[1]=6.0f;
	fixed.varat3[2]=2.0f;
	fixed.animp=5.0f;
	fixed.damr[0]=0.0f;
	fixed.damr[1]=0.02f;
	fixed.damrmn=15.0f;
	fixed.dec1[0]=3.9f;
	fixed.dec1[1]=4.9f;
	fixed.dec2[0]=14.8f;
	fixed.dec2[1]=18.5f;	
	fixed.dec3[0]=6.0f;
	fixed.dec3[1]=7.3f;	
	fixed.p1co2a[0]=0.6f;	
	fixed.p1co2a[1]=0.17f;
	fixed.pligst[0]=3.0f;	
	fixed.pligst[1]=3.0f;
	fixed.spl[0]=0.85f;
	fixed.spl[1]=0.013f;
	fixed.strmax[0]=5000.0f;
	fixed.strmax[1]=5000.0f;
	fixed.pmco2[0]=0.55f;
	fixed.pmco2[1]=0.55f;
	
	fixed.ps1s3[0]=0.003f;
	fixed.ps1s3[1]=0.032f;
	fixed.ps2s3[0]=0.003f;
	fixed.ps2s3[1]=0.009f;
	fixed.dec4=0.0045f;
	fixed.dec5=0.2f;
	fixed.favail=0.9f;
	fixed.frfrac[0]= 0.01f;//      For trees only:frfrac[0] - minimum possible allocation fraction to fine roots
	fixed.frfrac[1]= 0.8f;//                    (tree.100)
//        frfrac[1] - maximum possible allocation fraction to fine roots
	fixed.p2co2=0.55f;
	fixed.p3co2=0.55f;
	fixed.pabres=100.0f;
	fixed.pcemic[0]=16.0f;
	fixed.pcemic[1]=10.0f;
	fixed.pcemic[2]=0.02f;
	fixed.peftxa=0.25f;
	fixed.peftxb=0.75f;
	fixed.rad1p[0]=12.0f;
	fixed.rad1p[1]=3.0f;
	fixed.rad1p[2]=5.0f;
	fixed.rcestr=200.0f;
	fixed.rictrl=0.015f;
	fixed.riint=0.8f;
	fixed.rsplig=0.3f;
	fixed.teff[0]= 15.4f;	//different in daily version, see online manual or Daycent code
	fixed.teff[1]= 11.75f;
	fixed.teff[2]= 29.70f,
	fixed.teff[3]= 0.031f;
return fixed;
}


/********************************************************************
Partition Residue: Input: source pool of residue, C amount, ratio residue: N/C, lignin fraction
Partions the residue into structual and metabolic and schedules flows.

returns direct adsorbed mineral N [g/m2]

NB: no direct adsorption of Surface litter
    arguments: C/N/lig of TMP struc/metab layer
*********************************************************************/

float PartitionResidue(	
					    float C,			// C Content g/m2 of litter 
					    float recres,		//  residue: ratio N/C
					    float frlig,		// // fraction of incoming material = lignin.
					    int layer,			// SRFC or SOIL
					    float Navailable,   //available N, either in first 20 or in XN soil layer
                                            //for direct adsorbtion
                        float *newmetabC,      //return values: new C,N of struc and metab pool,
                        float *newmetabN,      
						float oldstrucC,
                        float *newstrucC,
                        float *newstrucN,
						float oldlig,
						float *newlig,		//               adjusted lignin content of structural pool
						struct fixedparameter fixed)
{
float ratioCtotalN; // C/N ratio after direct adsorption
float fractionN; //  fraction of nitrogen in residue after direct adsorption.
float rlnres;	//Lignin/nitrogen ratio of residue
float frmet;  //fractio of C that goes to metabolic
float caddm, cadds; //total C added to metabolic/structural
float  amtadded;
float fligst;		// fligst is the lig. fraction of incoming structural residue
float N=C*recres; //N content in residue
float directadsorbedN=0.0f; //amount of Mineral N from mineral pool that is adsorbed by the residue
						//return value
float dummy; //debugger
if (C >= 10e-7)
{
	// Direct absorption of mineral element by residue
	// (mineral will be transferred to donor compartment and then
	// partitioned into structural and metabolic using flow routines.)
	// If minerl(SRFC,e) is negative then directAbsorbE = zero.

     if (Navailable < 0.0f) directadsorbedN=0.0f; 
	 else
	 {
		directadsorbedN=fixed.damr[layer]*Navailable*(float)min(1.0, C/fixed.pabres); //Change  it to min?? In Daycent: max makes no sense
	 }
    
	// If C/E ratio is too low, transfer just enough to make
	// C/E of residue = damrmn
	if (N+directadsorbedN <= 0.0f) ratioCtotalN=0.0f;
	else ratioCtotalN=C/(N+directadsorbedN);
	if(ratioCtotalN < fixed.damrmn) 
	{
		directadsorbedN = C/fixed.damrmn - N;
	}
	directadsorbedN= max(directadsorbedN,0.0f);

	//Partition carbon into structural and metabolic fraction of residue
	fractionN= (N+directadsorbedN) / (C*2.5f);
	
	//Carbon added to metabolic
	rlnres=frlig/fractionN;		// Lignin/nitrogen ratio of residue
	frmet= fixed.spl[0] + fixed.spl[1]*rlnres;//fractio of C that goes to metabolic
    // Make sure the fraction of residue which is lignin isn't
    // greater than the fraction which goes to structural. 
	frmet=min(frmet, 1.0f-frlig);
	frmet=max(frmet, 0.2f); // at least 20% goes to metabolic
	//Compute amounts to flow
	caddm=max(0.0f,C*frmet); // C added to metabolic
	cadds= C - caddm; //C added to structural
    // Adjust lignin content of structural.
    // fligst is the fraction of incoming structural residue
    // which is lignin;
    // Changed maximum fraction from .6 to 1.0  -lh 1/93
	fligst=(float)min(frlig*C/cadds, 1.0);
	//ligninFraction = (oldlig + newlig) / (oldC + addC);
	*newlig= (oldlig*oldstrucC + fligst*cadds ) / 
						(oldstrucC + cadds);
	dummy=*newlig;

	// C flow to structural / metabolic [layer]
	*newstrucC = cadds;
	*newmetabC = caddm;
	
	//N flow to struc/metab
	//structural
	// 14.10.2008 SB: insert min() to avoid amtadded>N
	//amtadded=cadds/fixed.rcestr; 
	amtadded=min( (N+directadsorbedN), cadds/fixed.rcestr); 
	*newstrucN =  amtadded;
	//metabolical
	amtadded=N + directadsorbedN - amtadded ;
	*newmetabN =  amtadded;
}// if C is significant

	return directadsorbedN;
}

//sum all available mineral N=Nitrate+Ammonium to depth
// return g/m2
float getNtoDepth (float depth, EXP_POINTER)
{
// Calculate N amount to depth [g/m2]
PSLAYER  pSL=pSo->pSLayer;// used to iterate over the original list
PCLAYER  pCH=pCh->pCLayer;
float N=0.0f; //return value
float d=0.0f;
for(pSL=pSL->pNext,pCH=pCH->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext,pCH=pCH->pNext) // exclude virtual layer at top and bottom
{
	d+= pSL->fThickness*0.1f;
	if (d <= depth)
	{
		N += pCH->fNO3N   
		   + pCH->fNH4N;
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < depth))
	{
		N += pCH->fNO3N   *(pSL->fThickness * 0.1f-(d-depth))/(pSL->fThickness*0.1f)
		   + pCH->fNH4N *(pSL->fThickness * 0.1f-(d-depth))/(pSL->fThickness*0.1f);
	} 
}
N *= kgphaTOgpm2;
return N;
}

float FlowNfromMineralSoil(float Ndemand,		// amount of mineral N wanted (gE/m^2) 
						float depth,			// soil depth to give mineral E (cm)
						float useNitrateFrac,	// max fraction removed from NOx pools range = (0-1),
												// -1 = remove in proportion to nitrate/ammonium available
						EXP_POINTER)	
													
{	
	PSLAYER  pSL=pSo->pSLayer->pNext;// XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;
 
	float d=0.0f,removed=0.0f,totalNremoved=0.0f;
	int i;
	float nitrateFraction, ammoniumFraction;

	//	FlowNFromSoil
	//	Schedule a flow for a mineral element from the top simDepth of soil.
	//	Demand is limited by availability in the total soil.
	//	so no layer is completely depleted.
	//	Return the total amount of flow scheduled.
	if( getNtoDepth(depth, exp_p)< 0.0f) return 0;
	
	Ndemand= min(Ndemand, getNtoDepth (depth, exp_p));
	for(i= 0; pSL != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
	{
	// nitrate and ammonium fractions
	if(pCL->fNO3N <= 0.0 && pCL->fNH4N <= 0.0) 
	{
		totalNremoved = 0.0;
		break;
	}
	else
	{
		if (useNitrateFrac < 0.0f)
		{		  
			nitrateFraction = pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N);
			ammoniumFraction =1.0f - nitrateFraction;
		}
		else   // 0.0 <= useNitrateFrac <= 1.0;
		{
			nitrateFraction = min( useNitrateFrac, 
							 pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N) );
			ammoniumFraction = 1.0f - nitrateFraction;
		}
			
		if(pCL->fNO3N <=0.0f)
		{
			nitrateFraction=0.0f;
			ammoniumFraction=1.0f;
		}
		if(pCL->fNH4N <=0.0f)
		{
			nitrateFraction=1.0f;
			ammoniumFraction=0.0f;
		}			
		//Flow from Soil
		d+= pSL->fThickness*0.1f;
		if (d <= depth)	//full layer
		{
			//nitrate
			if(pCL->fNO3N > 0.0f)
			{
			removed=Ndemand * nitrateFraction * (pSL->fThickness*0.1f)/depth;
			removed=min(removed, (pCL->fNO3N*kgphaTOgpm2)); 
			pCL->fNO3N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
			//ammonium
			if(pCL->fNH4N > 0.0f)
			{
			removed=Ndemand * ammoniumFraction * (pSL->fThickness*0.1f)/depth;
			removed=min(removed, (pCL->fNH4N*kgphaTOgpm2)); 
			pCL->fNH4N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
		} 
		if (d > depth && ((d-pSL->fThickness*0.1f) < depth))  //fractional layer
		{
			//nitrate
			if(pCL->fNO3N > 0.0f)
			{
			removed=Ndemand * nitrateFraction * ((pSL->fThickness*0.1f)-(d-depth))/depth;
			//removed=min(removed, (pCL->fNO3N*kgphaTOgpm2)); 
			pCL->fNO3N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
			//ammonium
			if(pCL->fNH4N > 0.0f)
			{
			removed=Ndemand * ammoniumFraction * ((pSL->fThickness*0.1f)-(d-depth))/depth;
			removed=min(removed, (pCL->fNH4N*kgphaTOgpm2)); 
			pCL->fNH4N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
		}
		if(pCL->fNO3N <0.0) pCL->fNO3N = 0.0;//numerical issue
 		if(pCL->fNH4N <0.0) pCL->fNH4N = 0.0;//numerical issue
	}//there is some mineral N at all
	}//loop over layer
		
	return totalNremoved;
}
float FlowNintoMineralSoil(float Ntoadd,			// amount of mineral N to add (gN/m^2)
							float depth,				// soil depth to receive mineral E (cm)
						float fractionToNitrate,	// fraction of N to go to Nitrate range = (0-1) 
						EXP_POINTER)	
													
{
 
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext; 
 
float d=0.0f,toadd=0.0f,debug;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		//nitrate
		toadd=Ntoadd * fractionToNitrate * (pSL->fThickness*0.1f)/depth;
		pCL->fNO3N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
		//ammonium
		toadd=Ntoadd * (1.0f-fractionToNitrate) * (pSL->fThickness*0.1f)/depth;
		pCL->fNH4N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		//nitrate
		debug=4;
		toadd=Ntoadd * fractionToNitrate * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNO3N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
		//ammonium
		toadd=Ntoadd * (1.0f-fractionToNitrate) * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNH4N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
	}
}

	return Ntoadd;
}

 
 /* Calculates if Immobilization or Mineralization occurs and calculates flows 
   from A to B and from/to the mineral pool.
  
*/
void ScheduleNFlow(				
	float CFlowAtoB, 
	float FlowRatioCtoN,
	float AC,float AN,	//C and N content of Box A (source)
	float availMinN,	//available mineral N for possible Immobilisation
	//return values
	float *orgNflow,
	float *minNflow		//negative value for immobilisation
	)
{
	float const NFlowFromA=CFlowAtoB * AN/AC; //proportional zum C Flu�, abh�ngig vom
 															//C/N Verh�ltniss in A																	//!!i.A.: nicht alles kommt in B an
//Immobilization 
	if(CFlowAtoB/NFlowFromA > FlowRatioCtoN)
	{
		// organic N flow from A to B
		*orgNflow = NFlowFromA;
		//calculate Immobilization
		*minNflow = -(float)min( availMinN, CFlowAtoB/FlowRatioCtoN - NFlowFromA);//neg value for immobilization
		//Schedule flow from mineral pool to pool B (immobilization flow)
	}
//Mineralization
	if(CFlowAtoB/NFlowFromA <= FlowRatioCtoN)
	{
		// N flow from A to B
	    *orgNflow = CFlowAtoB/FlowRatioCtoN;  
		//Schedule flow from pool A to mineral pool
		*minNflow = NFlowFromA - CFlowAtoB/FlowRatioCtoN; //pos. value for mineralization
	}

}

int CanDecompose (
	float const C,		// total C in Box A
	float const N,		//   N in Box A
	float const ratio, //   C/N,  ratios of new material being added to Box B
	float availMinN)	 
{
    int canDo = 1;	// flag for N = true
	// If there is no available mineral E
	if(N>=0){
	if ( availMinN <=10e-5 )
	{
	    // if C/E of Box A > C/E of new material
	    if (C / N > ratio)
	    {
		// Immobilization is necessary therefore
		// Box A cannot decompose to Box B.
		canDo = 0; //false
	    }
    }
	}
	if (N<=0) canDo=0; //false

    return ( canDo );
}
 

void decf20cm(	float * defac, float * anerb, //return values
				struct fixedparameter fixed,
				float drain,
				EXP_POINTER
				 )
{
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	float avgWFPS,a,b,c,d,e1,e2,base1,base2,wfunc,tfunc;
	float DailyPET,xh20,ratioH2OtoPET;
// a)Determine texture index (COARSE,MEDIUM, FINE, VERY FINE); 
	if (getSandtoDepth(20.0f,exp_p) > 70.0 ) textureindex=COARSE;
	if (getSandtoDepth(20.0f,exp_p) <= 70.0 && getSandtoDepth(20.0f,exp_p) >= 30.0  ) textureindex=MEDIUM;
	if (getSandtoDepth(20.0f,exp_p) < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"
 
// b)Determine water factor
	avgWFPS=getWFPStoDepth(20.0f, exp_p); //water filled pore space to 20cm depth									   
	switch(textureindex){
		case COARSE: a=0.55f; b=1.70f; c=-0.007f; d=3.22f; break;
		case MEDIUM: a=0.60f; b=1.27f; c=0.012f; d=2.84f;break;
		case FINE: a=0.60f; b=1.27f; c=0.012f; d=2.84f; break;//Daycent: no values here. ??
		case VERYFINE: a=0.60f; b=1.27f; c=0.012f; d=2.84f; break; //Where can it be set to VERYFINE?
	}
	//exponent bases
	base1=(avgWFPS - b)/(a-b);
	base2=(avgWFPS - c)/(a-c);
	//exponents
	e1=d*(b-a)/(a-c);
	e2=d;
    //water factor
	wfunc= (float)pow(base1,e1)*(float)pow(base2,e2);
	if(wfunc > 1.0f) wfunc=1.0;

// c)Determine temperature factor 
//	 Century uses temperature at 5cm depth
	tfunc=( fixed.teff[1] + (fixed.teff[2]/
		(float)PI)*(float)atan((float)PI*fixed.teff[3]*(pHe->pHLayer->pNext->fSoilTemp - fixed.teff[0])) ) 
		 /( fixed.teff[1] + (fixed.teff[2]/
		 (float)PI)*(float)atan((float)PI*fixed.teff[3]*(30.0f - fixed.teff[0])) ); // normalized to 30�C
	tfunc=min(1.2f,max(0.01f,tfunc));

// d)Compute Decomposition factor
	*defac=max(0.0f, wfunc*tfunc);
	*defac=wfunc*tfunc;

// e)Determine factor of anaerobic conditions 
	//drain: the fraction of excess water lost by drainage.
    //Anaerobic conditions (high soil water content) cause decomposition to decrease. 
	//The soil drainage factor (site parameter DRAIN) allows a soil to have differing degrees of wetness 
	//(e.g., DRAIN=1 for well drained sandy soils and DRAIN=0 for a poorly drained clay soil).
	DailyPET=0.1f* pWa->fPotETDay;
	if(DailyPET<=0.01f) DailyPET=0.01f;
    ratioH2OtoPET = (getH2OtoDepth(-1.0,exp_p) + 0.1f*pCl->pWeather->fRainAmount) //+ pMa->pIrrigation->fAmount causes fault
		                  /DailyPET;  
	*anerb=1.0;
		if(pCl->pWeather->fTempAve > 2.0 && ratioH2OtoPET > fixed.aneref[0]){
			xh20=(ratioH2OtoPET-fixed.aneref[0])*0.1f*pWa->fPotETDay*(1.0f-drain);
			if(xh20>0.0f){
			*anerb=max(fixed.aneref[2],
			( ((1.0f-fixed.aneref[2])/(fixed.aneref[0]-fixed.aneref[1])) * xh20/(0.1f*pWa->fPotETDay) + 1.0f ));
			}
		}
}//decf20cm

// decomposition factor of soil layer
float decfSL(
				float sand, // [%] in Soil layer
				float WFPS, // Water filled pore space = pWL->fContAct / pSo->pSLayer->fPorosity
				float SoilTemp, //pHe->pHLayer->fSoilTemp
				struct fixedparameter fixed,	
				EXP_POINTER
				 )
{
	float defac;  //return value
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	float a,b,c,d,e1,e2,base1,base2,wfunc,tfunc;
 
// a)Determine texture index (COARSE,MEDIUM, FINE, VERY FINE); 
	if (sand > 70.0 ) textureindex=COARSE;
	if (sand <= 70.0 && getSandtoDepth(20.0f,exp_p) >= 30.0  ) textureindex=MEDIUM;
	if (sand < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"
 
// b)Determine water factor										
	switch(textureindex){
		case COARSE:	a=0.55f; b=1.70f; c=-0.007f;d=3.22f; break;
		case MEDIUM:	a=0.60f; b=1.27f; c=0.012f; d=2.84f; break;
		case FINE:		a=0.60f; b=1.27f; c=0.012f; d=2.84f; break; //Daycent: no values here. ??
		case VERYFINE:	a=0.60f; b=1.27f; c=0.012f; d=2.84f; break; //Where can it be set to VERYFINE?
	}
	//exponent bases
	base1=(WFPS - b)/(a-b);
	base2=(WFPS - c)/(a-c);
	//exponents
	e1=d*(b-a)/(a-c);
	e2=d;
    //water factor
	wfunc= (float)pow(base1,e1)*(float)pow(base2,e2);
	if(wfunc > 1.0f) wfunc=1.0;

// c)Determine temperature factor 
//	 Century uses temperature at 5cm depth
	tfunc=( fixed.teff[1] + (fixed.teff[2]/(float)PI)*(float)((float)PI*fixed.teff[3]*(SoilTemp - fixed.teff[0])) ) 
		 /( fixed.teff[1] + (fixed.teff[2]/(float)PI)*(float)atan((float)PI*fixed.teff[3]*(30.0f     - fixed.teff[0])) ); // normalized to 30�C
	tfunc=min(1.2f,max(0.01f,tfunc));

// d)Compute Decomposition factor
	defac=max(0.0f, wfunc*tfunc);
	defac=wfunc*tfunc;
return defac;
}//decf soil layer

//returns percentage of sand content 
float getSandtoDepth(float depth, EXP_POINTER)
{
float sand=0.0f,weightingFactorFullLayer=0.0f; //return value
float d=0.0f;
PSLAYER  pSL=pSo->pSLayer; // used to iterate over the original list
								
for(pSL=pSL->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext)   // exclude virtual layer at top and bottom
{
	d+= pSL->fThickness*0.1f;
	weightingFactorFullLayer=pSL->fThickness * 0.1f /depth;
	if (d <= depth)
	{
		sand += pSL->fSand * weightingFactorFullLayer; 
	}
		if (d > depth && ((d-pSL->fThickness*0.1f) < depth))
	{
		sand +=  pSL->fSand *  
			(pSL->fThickness * 0.1f-(d-depth))/depth ;
	} 
}
return sand;
}

float getWFPStoDepth(float depth, EXP_POINTER)
{
float WFPS=0.0f,weightingFactorFullLayer=0.0f;
float d=0.0f;
PSLAYER  pSL=pSo->pSLayer; // used to iterate over the original list
PWLAYER  pWA=pWa->pWLayer;							
for(pSL=pSL->pNext,pWA=pWA->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext,pWA=pWA->pNext)   // exclude virtual layer at top and bottom
{
	weightingFactorFullLayer=pSL->fThickness * 0.1f /depth;

	d += pSL->fThickness*0.1f; //[cm]
	if (d <= depth)
	{
		WFPS += pWA->fContAct / pSo->pSLayer->fPorosity  * weightingFactorFullLayer; 
										 //	use direct input value: pSo->pSLayer->fPorosity. 
										 //Daycent: porosity=1-bulk density/2.65f
	}
		if (d > depth && ((d-pSL->fThickness*0.1f) < depth)) //Fractional Layer
	{
		WFPS +=  pWA->fContAct / pSo->pSLayer->fPorosity *  
			     (pSL->fThickness * 0.1f-(d-depth))/depth ; 
	} 
} 
// if (WFPS <0.0f || WFPS >1.0f) ExpertN Warnung ausgeben 
return WFPS; //0..1
}

// getH2OtoDepth 
// depth= -1 : return water content of all layers
float getH2OtoDepth (float depth, EXP_POINTER)
{
// Calculate H2O amount to root depth
PWLAYER  pWL=pWa->pWLayer;// used to iterate over the original list
PSLAYER  pSL=pSo->pSLayer;// used to iterate over the original list
float h2o=0.0f; //return value
float d=0.0f;
for( pWL = pWL->pNext, pSL=pSL->pNext;
	pWL->pNext != NULL && pSL->pNext != NULL ;  pWL = pWL->pNext, pSL=pSL->pNext)  // exclude virtual layer at top and bottom	
{
	if (depth <0.0)
	{
		h2o += pWL->fContAct * pSL->fThickness * 0.1f;

	}
	else //depth > 0
	{
		d+= pSL->fThickness*0.1f;
		if (d <= depth) //full layer
		{
			h2o += pWL->fContAct * pSL->fThickness * 0.1f; // H20 [cm]
		}
			if (d > depth && ((d-pSL->fThickness*0.1f) < depth)) //fractional layer
		{
			h2o += pWL->fContAct *  (pSL->fThickness * 0.1f-(d-depth))/depth ; // H20 [cm];
		} 
	}
}
return h2o; //[cm]
}


//	AboveGroundDecompRatio
// 	Calculates the aboveground decomposition ratio.
// 	Determine C/N of new material entering 'Box B'.
// 	
// 	The C/N ratios for structural and wood can be computed once;
// 	they then remain fixed throughout the run.  The ratios for
// 	metabolic and som1 may vary and must be recomputed each time step.
// 	Returns the C/N ratio of new N;  = function of the N content of the
// 	material being decomposed.
float AboveGroundDecompRatio (float totalCinA ,float totalNinA,float biofac,//amount of C and N in box A
																			//biomass factor: 2.0 for wood, 2.5 else
							 struct fixedparameter fixed)
	{
	float ratio;
	float cemicb;
	float const biomass = totalCinA * biofac;	// biomass factor=2.0 for wood
	float const econt =				// E/C ratio in box A
	 biomass <= 1.0e-10f ?	0.0f : totalNinA / biomass;
	//		TDecomposition.cpp line 173ff:
// cemicb = slope of the regression line for C/E of som1
//    where pcemicMax = maximum C/E of new som1
//      pcemicMin = minimum C/E of new som1
//      pcemic3 = minimum E content of decomposing
//                material that gives minimum C/E   //    pcemic is a fixed parameter
	cemicb =( fixed.pcemic[1] - fixed.pcemic[0]) / fixed.pcemic[2];	
	ratio  = ( econt > fixed.pcemic[2] ? fixed.pcemic[1] : fixed.pcemic[0] + econt * cemicb);
	return ratio;
	}

float FlowCintoSOM2(float Ctoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ctoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fCHumusSlow += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ctoadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fCHumusSlow += toadd * gpm2TOkgpha;
	}
}
	return Ctoadd;
}

float FlowNintoSOM2(float Ntoadd,			// amount of organic N to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ntoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fNHumusSlow += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ntoadd *  ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNHumusSlow += toadd * gpm2TOkgpha;
	}
}
	return Ntoadd;
}
float FlowCintoSOM1(float Ctoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ctoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fCHumusFast += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ctoadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fCHumusFast += toadd * gpm2TOkgpha;
	}
}
	return Ctoadd;
}

float FlowNintoSOM1(float Ntoadd,			// amount of organic N to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ntoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fNHumusFast += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ntoadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNHumusFast += toadd * gpm2TOkgpha;
	}
}
	return Ntoadd;
}

float CO2intoSOIL(float CO2toadd,			// amount of organic N to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
{
	d += pSL->fThickness *0.1f; //cm
	if (d <= depth)
	{
		toadd = CO2toadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fCO2C		+= toadd * gpm2TOkgpha; // [kg/ha]
		pCL->fCO2ProdR	+= toadd * gpm2TOkgpha; // [kg/ha]
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=CO2toadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fCO2C		+= toadd * gpm2TOkgpha;// [kg/ha]
		pCL->fCO2ProdR	+= toadd * gpm2TOkgpha; // [kg/ha]
	}
}
	return CO2toadd;
}

//Following functions are for debugging purpose
//it returns the total mineral N in TMP Clayers [kg/ha]
float getN(PCLAYER  pCL)
{ 
float N=0;
for(pCL=pCL->pNext; pCL->pNext != NULL ;pCL=pCL->pNext) 
{
	N += pCL->fNO3N + pCL->fNH4N;
}
return N;
}

float getNH4(PCLAYER  pCL)
{ 
float N=0;
for(pCL=pCL->pNext; pCL->pNext != NULL ;pCL=pCL->pNext) 
{
	N +=  pCL->fNH4N;
}
return N;
}

float getNO3(PCLAYER  pCL)
{ 
float N=0;
for(pCL=pCL->pNext; pCL->pNext != NULL ;pCL=pCL->pNext) 
{
	N += pCL->fNO3N  ;
}
return N;
}

/*******************************************************************************
** EOF */