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
 *   Sammlung der Modellansaetze zur Berechnung des Stickstofftransportes
 *   im Boden.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: transp.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:28
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/chemist
 * Using _MALLOC macro to detect memory leaks.
 *
 *   Zusammenfuehren der Funktionen erfolgte am 12.03.97
 * 
*******************************************************************************/

#include <crtdbg.h>
#include "xinclexp.h"    
#include "xn_def.h" 
#include "xh2o_def.h"     


#define kgPhaTomgPsqm    (float)100.0

/*******************************************
 *  external procedures                    *
 *******************************************/

/* aus water1h.c */
extern BOOL BotInf;
extern BOOL FreeD;

/* aus util_fct.c */
extern float abspower(float, float);
extern int   NewDay(PTIME);
extern int   SimStart(PTIME);
extern float WINAPI Polygon4(float, float, float,float,float ,float ,float ,float ,float);
extern int   WINAPI Test_Range(float x, float x1, float x2, LPSTR c);

/* aus output.ccp */
extern int   WINAPI Message(long, LPSTR);
extern int WINAPI Deb_NBal(EXP_POINTER);

/*******************************************
 *  Exportprozeduren                       *
 *******************************************/
int WINAPI NTransp(EXP_POINTER, int N2O_IN_SOLUTION);
int WINAPI NVolat1(EXP_POINTER);
int WINAPI NVolat2(EXP_POINTER);
int WINAPI NConstDeposition(EXP_POINTER);

signed short int WINAPI NTransportCERES(EXP_POINTER);//CERES
signed short int WINAPI FertilMinInflow(EXP_POINTER);//CERES
signed short int NFlussCERES(EXP_POINTER);//CERES

float Solut_Coeff(float Temp);
int   Get_Diffusion_Const(EXP_POINTER);
int   Solve_LGS(EXP_POINTER);

int WINAPI EmitAllN2O(EXP_POINTER);


#define  N_TRANSP_FIRST_LAYER  iLayer = 1, idummy = 1,\
              pSL = pSo->pSLayer->pNext,\
              pCL = pCh->pCLayer->pNext,\
              pWL = pWa->pWLayer->pNext;
              
                     
#define  N_TRANSP_STOP_LAYER \
             ((pSL->pNext!=NULL)&&\
              (pCL->pNext!=NULL)&&\
              (pWL->pNext!=NULL));
              
#define  N_TRANSP_NEXT_LAYER iLayer++, idummy++,\
              pSL = pSL->pNext,\
		      pCL = pCL->pNext,\
		      pWL = pWL->pNext

#define  N_TRANSP_F_LAYERS  N_TRANSP_FIRST_LAYER N_TRANSP_STOP_LAYER N_TRANSP_NEXT_LAYER 

#define  N_TRANSP_ZERO_LAYER  idummy=0,\
              pSL = pSo->pSLayer,\
              pCL = pCh->pCLayer,\
              pWL = pWa->pWLayer;              

#define  N_TRANSP_Z_LAYERS  N_TRANSP_ZERO_LAYER N_TRANSP_STOP_LAYER N_TRANSP_NEXT_LAYER


static int iMobImm;
iMobImm = (int)0;


/**********************************************************************************************/
/* Procedur    :NConstDeposition()                                                            */
/* Op.-System  :   DOS                                                                        */
/* Beschreibung:   Ammoniak, NO3 Deposition                                                   */
/*                 Constant deposition                                                        */
/*                                                                                            */
/*              GSF/ch 22.8.97	                                                              */
/*                                                                                            */
/**********************************************************************************************/
/* veränd. Var.		pCh->pCLayer->pNext->fNH4N                                                */
/*					pCh->pCLayer->pNext->fNO3N                                                */
/*					                                                                          */
/**********************************************************************************************/
int WINAPI NConstDeposition(EXP_POINTER)
{

  const float     fNH4DepositonR = pCh->pCProfile->fCH4ImisR;//0.054;//Solling //0.027;//Scheyern 0.027kg/d*365d/a=10kg/a
  const float     fNO3DepositonR = pCh->pCProfile->fCH4ImisDay;//0.06;//Solling //0.014;//Scheyern 0.014kg/d*365d/a=5kg/a
  
               
  pCh->pCLayer->pNext->fNH4N += fNH4DepositonR * pTi->pTimeStep->fAct;
  pCh->pCLayer->pNext->fNO3N += fNO3DepositonR * pTi->pTimeStep->fAct;
    
  pCh->pCBalance->dNInputCum += (double)((fNH4DepositonR+fNO3DepositonR)*pTi->pTimeStep->fAct);


  return 1;
}




/**********************************************************************************************/
/* Procedur    :NVolat1()                                                                     */
/* Op.-System  :   DOS                                                                        */
/* Beschreibung:   Ammoniak Verfluechtigung                                                   */
/*                 Methode LEACHM 3.0 (Hutson & Wagenet, 1992)                                */
/*                                                                                            */
/*                  ep/gsf 15.03.94                                                           */
/*					ch/gsf 22.07.96	  Betrachte ROrgNH3 als kumul. Groesse.                   */
/*								      => Verwertbar in Bilanzierung.                          */  
/*								                                                              */
/**********************************************************************************************/
/* veränd. Var.		pCh->pCLayer->pNext->fNH4N                                                */
/*					pCh->pCProfile->fNH3VolatR                                                */
/*					                                                                          */
/**********************************************************************************************/
int WINAPI NVolat1(EXP_POINTER)
{
  float f1;

  //fNH3VolMaxR = (float)0.1;   // orig Leach:0.4;
  							//  0.0 - 0.3  Jemison et al 1994
  
  /************** Volatilisation ***************/
   
    //  Berechnung erfolgt fuer Schicht 1

    f1       = max(EPSILON,
    				pWa->pWLayer->pNext->fContAct
             		+ pSo->pSLayer->pNext->fBulkDens * pCh->pCParam->afKd[1]);

// unnötig:    f2       = pSo->pSLayer->pNext->fThickness * (float)0.01;
    
    pCh->pCProfile->fNH3VolatR  = min(((float)0.5 * pCh->pCLayer->pNext->fNH4N / pTi->pTimeStep->fAct),
                   (pCh->pCProfile->fNH3VolatMaxR * pCh->pCLayer->pNext->fNH4N))
// betrachtet wird der gelöste Anteil des NH4:
             		* pWa->pWLayer->pNext->fContAct /f1; 
               
    pCh->pCLayer->pNext->fNH4N   -= pCh->pCProfile->fNH3VolatR * pTi->pTimeStep->fAct;



  return 1;
}

/*********************************************************************************/
/*  Name     : NVolat2                                                  */
/*                                                                               */
/*  Funktion : Nach der organischen Duengung entstehen gasfoermige Verluste,     */
/*             die durch eine Vielzahl von Prozessen beeinflusst werden. In      */
/*             dieser Funktion werden Ansaetze von Hoffmann&Ritchie integriert.  */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 05.02.98                                                   */
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
int WINAPI NVolat2(EXP_POINTER)
{

float	volNH4Ob,volNH4Bo,tempfac;

PCPROFILE	pCP = pCh->pCProfile;


volNH4Ob = (float)0.0;
volNH4Bo = (float)0.0;

tempfac = pCl->pWeather->fTempMax;
if (tempfac < (float)0.0)
	tempfac = (float)0.1;

	/* Berechnung der Verluste aus dem Oberflaechenpool.*/

	if (pCh->pCProfile->fNH4ManureSurf > (float)0.0)
	{
		volNH4Ob = (float)0.00795 * (float)pow((double)tempfac,(double)1.2) *
					(float)pow((double)pCh->pCProfile->fNH4ManureSurf,(double)1.05) *
					(float)exp((double)0.1 * (double)pPl->pCanopy->fLAI);
	}

	/* Berechnung der Verluste aus dem Pool der ersten 
	   realen Bodenschicht.*/
	
	if (pCh->pCLayer->pNext->fNH4Manure > (float)0.0)
	{
		volNH4Bo = (float)0.00225 * (float)pow((double)tempfac,(double)1.2) *
					(float)pow((double)pCh->pCLayer->pNext->fNH4Manure,(double)1.05);
	}


  if (pMa->pNFertilizer != NULL)
  {
	if ((int)pTi->pSimTime->fTimeAct >= pMa->pNFertilizer->iDay)
	{
		volNH4Ob = volNH4Ob * (float)1.5;
	}	
  }

	/* Die verfuechtigte Ammoniummenge wird vom Oberflaechenpool
       und aus der ersten realen Bodenschicht abgezogen. */
	pCh->pCProfile->fNH4ManureSurf		-= volNH4Ob;
	pCh->pCLayer->pNext->fNH4Manure	-= volNH4Bo;


	pCP->fNH3VolatR += (volNH4Ob + volNH4Bo);

 return 1;
}      /*=====   Ende der Integration Verluste Duengung  =================*/

	
/*********************************************************************************************/
/* Procedur    :   Solut_Coeff()                                                             */
/* Beschreibung:   Approximiere Löslichkeit von N2O in Abhängigkeit von der Temperatur       */
/* Nach        :   Moraghan und Buresh (1977)                                                */
/* aus         :   Dissertation C. Braun (1994)                                              */
/*********************************************************************************************/
float Solut_Coeff(float Temp)
{
float f1;

if (Temp <= (float)0)
   f1 = (float) 0.01;//to avoid division by zero in transp.c //0; /* [cm^3 N2O / cm^3 H2O]  */

else if (Temp <= (float)5)
   f1 = (float) 0.213 * Temp; 

else
   f1 = (float)1.1067 - (float) 0.02 * Temp; 

return f1;
}                                                                  


/**********************************************************************************************/
/* Procedur    :   Get_Diffusion_Const()                                                      */
/* Beschreibung:   Berechne Diffusions-Koeffizienten                                          */
/*                                                                                            */
/**********************************************************************************************/
/* geänderte Var.	pCL->fGasDiffCoef                                                         */
/*					pCL->fLiqDiffCoef                                                         */
/**********************************************************************************************/
int Get_Diffusion_Const(EXP_POINTER)
{
	DECLARE_COMMON_VAR
    
    PWLAYER  pWL;
    PCLAYER  pCL;
    PSLAYER  pSL;

    float f1,f2;   
    float fThetaMid, fGasMid, fGasVol;
    
    float DeltaZ = pSo->fDeltaZ;
    float fGDIF  = (float)11840;    /* estimated N2O  diffusion coeff. in air [cm^2 /d] 
    									calculated with Fuller correlation (Perry, 1984) */
    
   
    for (iLayer=0,
         pWL=pWa->pWLayer,
         pCL=pCh->pCLayer,
         pSL=pSo->pSLayer;
         ((pWL->pNext!=NULL)&&
          (pCL->pNext!=NULL)&&
          (pSL->pNext!=NULL));
          pWL = pWL->pNext,
          pCL = pCL->pNext,
          pSL = pSL->pNext,
          iLayer++)
    {
       /* Gas - Diffusionskoeffizient -------------------------------------------- */

       fThetaMid = (pWL->pNext->fContAct+ pWL->pNext->fContOld
                 +  pWL->fContAct + pWL->fContOld)
                 / (float)4.0;
       
       fGasVol 	 = pSL->fPorosity 
       		 	 - (pWL->fIce + pWL->fIceOld + pWL->fContAct+ pWL->fContOld)
                 / (float)2; 
                 
       if (fGasVol < EPSILON)
       	{          
       	fGasVol = (float)0;
       	}

       fGasMid 	 = (fGasVol
       			 + pSL->pNext->fPorosity 
                 - (pWL->pNext->fIce + pWL->pNext->fIceOld + pWL->pNext->fContAct+ pWL->pNext->fContOld)
       			 / (float)4) 
       			 / (float)2;
       			   
       
       /* Tortuositätsfaktor nach Millington und Quirk : */
       f1 = abspower(fGasVol,(float)3.330) / (pSL->fPorosity * pSL->fPorosity);
     

       pCL->fGasDiffCoef = fGDIF * f1;        /*[cm^2 / d] */

       f1 = fThetaMid * pCh->pCParam->fDispersivity  - (float)0.16 * DeltaZ;
       f1 = fThetaMid * ((float)-0.84+(float)1.264*pCh->pCParam->fDispersivity-(float)0.6458*DeltaZ);
       f2 = pWL->fFluxDens; 
       if (f2 < (float)0.0)  
           f2 *= (float)-1.0;

       f1 = max((float)0.0,f1) * f2 / fThetaMid;
       f2 = (float)exp((double)(pSL->fImpedLiqB * fThetaMid)); /* ch ?? Vorzeichen */

       pCL->fLiqDiffCoef = (pCh->pCParam->fMolDiffCoef * pSL->fImpedLiqA * f2 + f1)/(float)100;
        
       if ((iLayer == 0) && (pWL->fFluxDens == (float)0.0))
          pCL->fLiqDiffCoef = (float)0.0;

       /* Test: ohne Diffusion  !!! *-/
       pCL->fLiqDiffCoef = pCL->fGasDiffCoef = (float)0;  */
  } /* for */
    
  /* Korrektur Schicht 0: */
  pCh->pCLayer->fGasDiffCoef = pCh->pCLayer->pNext->fGasDiffCoef;


  return 1;
} /* Get_Diffusion_Const */
  
  
/**********************************************************************************************/
/* Procedur    :   Get_DON_TransformationRates()                                              */
/* Beschreibung:   Berechne DON-Umsatzraten                                                   */
/*                                                                                            */
/**********************************************************************************************/
/* geänderte Var.	pCL->fHumusToDONR                                                         */
/*					pCL->fLitterToDONR                                                        */
/*					pCL->fManureToDONR                                                        */
/*					pCL->fDONToHumusR                                                         */
/*					pCL->fDONToLitterR                                                        */
/*					pCL->fDONMinerR                                                           */
/**********************************************************************************************/
int Get_DON_TransformationRates(EXP_POINTER)
{
	DECLARE_COMMON_VAR
    
    PWLAYER  pWL;
    PCLAYER  pCL;
    PSLAYER  pSL;
    PCPARAM  pPA;
    PHLAYER  pHL;
    PSWATER  pSW;

    struct ncorr corr={(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1,(float)1}; 

    float DeltaZ = pSo->fDeltaZ;

    /* 1. Variablen für die Reduktionsfunktionen*/
    float f1,wmin,wlow,whigh;
    //float f2,f3;

    pWL = pWa->pWLayer;
    pCL = pCh->pCLayer;
    pSL = pSo->pSLayer;
    pPA = pCh->pCParam;
    pHL = pHe->pHLayer;
    pSW = pSo->pSWater;

    for (iLayer=1,
         pWL=pWa->pWLayer->pNext,
         pCL=pCh->pCLayer->pNext,
         pSL=pSo->pSLayer->pNext,
         pSW = pSo->pSWater->pNext;
         ((pWL->pNext!=NULL)&&
          (pCL->pNext!=NULL)&&
          (pSL->pNext!=NULL)&&
          (pSW->pNext!=NULL));
          pWL = pWL->pNext,
          pCL = pCL->pNext,
          pSL = pSL->pNext,
          pSW = pSW->pNext,
          iLayer++)
    {
    /* 1. Temperatur-Reduktionsfunktion*/
    /* Q10-Funktion */
	corr.Temp = abspower(pPA->fMinerQ10,((pHL->fSoilTemp - pPA->fMinerTempB)/(float)10.0));
    
    /* 2. Feuchte-Reduktionsfunktion*/
	/* Ansatz LeachN, verändert*/ 
    f1= (float)-153300.0;      //pF = 4.18 = PWP
    wmin = WATER_CONTENT(f1);
    f1= (float)-30660.0;       //pF = 3.49
    wlow = WATER_CONTENT(f1);
    whigh = max(wmin,(pSL->fPorosity - pPA->fMinerThetaMin));
    
    /* Original-Grenzwerte, beruhen auf Werten des volumetrischen Wassergehalts 
    wmin  = pSW->fContPWP; //= PWP-Wert
    wlow  = wmin + (float)0.10;
	wmax  = pSL->fPorosity;
    whigh = wmax - (float)0.08; */
    
    corr.Feucht = Polygon4((pWL->fContAct + pWL->fIce), 
                            wmin,(float)0,  wlow, (float)1 , whigh, (float)1, 
							pSL->fPorosity, pPA->fMinerSatActiv);
        
    /* DON-Transformations-Raten (1/t) */
    pCL->fHumusToDONR  = pCL->fHumusToDONMaxR  * corr.Temp * corr.Feucht;// * pSL->fNHumus;
    pCL->fLitterToDONR = pCL->fLitterToDONMaxR  * corr.Temp * corr.Feucht;// * pCL->fNLitter;
    pCL->fManureToDONR = pCL->fManureToDONMaxR  * corr.Temp * corr.Feucht;// * pCL->fNManure;
    pCL->fDONToHumusR  = pCL->fDONToHumusMaxR  * corr.Temp * corr.Feucht;// * pCL->fDON;
    pCL->fDONToLitterR = pCL->fDONToLitterMaxR  * corr.Temp * corr.Feucht;// * pCL->fDON;
    pCL->fDONMinerR    = pCL->fDONMinerMaxR  * corr.Temp * corr.Feucht;

    // in transo.c, da dort auch die DOC-Mineralisierungsrate benoetigt wird
    //pCL->fDOMCN   = (pCL->fDON > EPSILON)? pCL->fDOC/pCL->fDON:(float)0.1; 
    //f3     = (float)1 - pPA->fMinerEffFac * pCL->fDOMCN / pCL->fMicBiomCN;
    //pCL->fDONMinerR    *= f3;

  } /* for */
    
  /* Korrektur Schicht 0: */
  //pCh->pCLayer->fGasDiffCoef = pCh->pCLayer->pNext->fGasDiffCoef;


  return 1;
} /* Get_DON_TransformationRates */

/**********************************************************************************************/
/* Procedur    :   Solve_LGS()                                                                */
/* Beschreibung:   Löser ...                                                                  */
/*                                                                                            */
/**********************************************************************************************/
/* veränd. Var		pCL->fLiqDiffCoef                                                         */
/*					pCL->fGasDiffCoef                                                         */
/*					pCL->fNH4NSoilConc                                                        */
/*					pCL->fNO3NSoilConc                                                        */
/*					pCL->fN2ONGasConc                                                         */
/*					pCL->fUreaNSoilConc                                                       */
/**********************************************************************************************/
int Solve_LGS(EXP_POINTER)
{                    
      DECLARE_COMMON_VAR

      PCLAYER pCL;
      PWLAYER pWL;
      PSLAYER pSL;
      
      double  *pA,*pB,*pC,*pD;
      double  *pAB,*pBB,*pCB,*pDB;
      double  *pF,*pG;
      
      int Stoff, idummy;
        
      float  DeltaZ = pSo->fDeltaZ;
      double f1,f2,f3,f4,fBeta1,fBeta2,fBeta3,fBeta4;
	  float  fAG,fBG;
      double d1;

/* -------------Speicher für Schichtvariable reservieren-------------- */

        pAB        = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pAB,0x0,(pSo->iLayers * sizeof(double)));
        pBB        = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pBB,0x0,(pSo->iLayers * sizeof(double)));
        pCB        = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pCB,0x0,(pSo->iLayers * sizeof(double)));
        pDB        = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pDB,0x0,(pSo->iLayers * sizeof(double)));


        pA          = (double *) _MALLOC(pSo->iLayers  * sizeof(double));
        memset(pA,0x0,(pSo->iLayers * sizeof(double)));
        pB          = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pB,0x0,(pSo->iLayers * sizeof(double)));
        pC          = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pC,0x0,(pSo->iLayers * sizeof(double)));
        pD          = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pD,0x0,(pSo->iLayers * sizeof(double)));


        pF          = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pF,0x0,(pSo->iLayers * sizeof(double)));
        pG          = (double *) _MALLOC(pSo->iLayers * sizeof(double));
        memset(pG,0x0,(pSo->iLayers * sizeof(double)));

/*------------------ Speicher für Schichtvariable reserviert---------------- */


/* Randbedingungen berichtigen  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */

/* Keine Diffusion in geloester Phase aus dem Profil */
pCh->pCLayer->fLiqDiffCoef = (float)0.0; /* Schicht 0 */

for (SOIL_LAYERS0(pCL,pCh->pCLayer));   /* Zeiger auf letzte Schicht */
//pCL->pBack->fLiqDiffCoef = pCL->pBack->fGasDiffCoef = (float)0.0; 
pCL->fLiqDiffCoef = pCL->fGasDiffCoef = (float)0.0; 


for (Stoff = 0; Stoff < 6; Stoff++)   
{

    f1 = (double)2.0 * (double)DeltaZ* (double)DeltaZ /(double)100;   /*15.4.96 ch da DiffCoeff in cm^2 /d */
    f2 = (double)2.0 * (double)DeltaZ;

    for (SOIL2_LAYERS1(pCL, pCh->pCLayer->pNext, pWL, pWa->pWLayer->pNext))
     {
		pAB[iLayer] = (Stoff != 3) ? (double)(pCL->pBack->fLiqDiffCoef / f1) :
            (double)((pCL->pBack->fN2OKh * pCL->pBack->fLiqDiffCoef + pCL->pBack->fGasDiffCoef) / f1);
        
        pBB[iLayer] = (Stoff != 3) ? (double)(pCL->fLiqDiffCoef / f1) :
            (double)((pCL->fN2OKh * pCL->fLiqDiffCoef + pCL->fGasDiffCoef) / f1);
            
	
	    /* ch 24.6.96 konvektiver Transport N2O: */
        pCB[iLayer] = (Stoff != 3)? (double)(pWL->pBack->fFluxDens / f2) :
                                     (double)(pCL->fN2OKh * pWL->pBack->fFluxDens / f2);

        pDB[iLayer] = (Stoff != 3) ? (double)(pWL->fFluxDens / f2) :
                                      (double)(pCL->fN2OKh * pWL->fFluxDens / f2);

     }
    
    //if (FreeD==TRUE)
	{
	 pBB[pSo->iLayers-2] = (double)0;
	}

	
	/* Koeffizienten der tridiagonalen Matrix errechnen  */

    for (N_TRANSP_F_LAYERS)
    {        
        if (pWL->fFluxDens > (float)0.0)
           {
            fBeta2 = (double)1.0;
            fBeta3 = (double)0.0;
           }
        else
           {
            fBeta2 = (double)0.0;
            fBeta3 = (double)1.0;
           }
        if (pWL->pBack->fFluxDens > (float)0.0)
           {
            fBeta1 = (double)1.0;
            fBeta4 = (double)0.0;
           }
        else
           {
            fBeta1 = (double)0.0;
            fBeta4 = (double)1.0;
           }  
 
    /* Koeffizienten berechnen ------------------------------------------- */

        pA[iLayer] = (double)-1.0 * pAB[iLayer] - fBeta1 * pCB[iLayer];

        /*----------------- */

        pB[iLayer] = (Stoff != 3)?
        		       (double)((pWL->fContAct
				 	 + (pSL->fPorosity - pWL->fContAct - pWL->fIce) * pCh->pCParam->afKh[Stoff]
        			 + pSL->fBulkDens * pCh->pCParam->afKd[Stoff])
        			 / DeltaT)
        			 :
        		  	   (double)((pWL->fContAct * pCL->fN2OKh 
        			 + pSL->fPorosity - pWL->fContAct - pWL->fIce)
        			 / DeltaT);

        if((iMobImm==1)&&(Stoff==5)) pB[iLayer] = (double)(pWL->fContMobAct/DeltaT);//pWL->fContAct/DeltaT;//
        //if((iMobImm==1)&&(Stoff==5)) pB[iLayer] = pWL->fContMobOld/DeltaT;//pWL->fContAct/DeltaT;//
        //if((iMobImm==1)&&(Stoff==5)) pB[iLayer] = (float)0.5*(pWL->fContMobAct+pWL->fContMobOld)/DeltaT;//pWL->fContAct/DeltaT;//


        pB[iLayer] += pAB[iLayer] + pBB[iLayer]
        			  + fBeta2 * pDB[iLayer]
        			  - fBeta4 * pCB[iLayer];

        /*----------------------- */

        pC[iLayer]= (double)-1.0 * pBB[iLayer] + fBeta3 * pDB[iLayer];
        /*---------------------- */

        f1 = pAB[iLayer] + fBeta1 * pCB[iLayer];
        
        f2 = (Stoff != 3)?
        	 (double)((pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]
           + (pSL->fPorosity - pWL->fContOld - pWL->fIceOld) * pCh->pCParam->afKh[Stoff])
           / DeltaT)
           :                                            
        	 (double)((pWL->fContOld * pCL->fN2OKh 
           + pSL->fPorosity - pWL->fContOld - pWL->fIceOld)
           / DeltaT);

	    if((iMobImm==1)&&(Stoff==5)) f2 = (double)(pWL->fContMobOld/DeltaT);//pWL->fContOld/DeltaT;//
        	
        f2 -= pAB[iLayer] + fBeta2 * pDB[iLayer];
        f2 += fBeta4 * pCB[iLayer] - pBB[iLayer];

        f3 = pBB[iLayer] - fBeta3 * pDB[iLayer];

        switch(Stoff)
        {
        case 0:
        f4 = (double)((pCL->fUreaNSoilConcOld - pCL->fUreaNSoilConc)
           * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT); 
        
        pD[iLayer] = (double)pCL->pBack->fUreaNSoilConcOld * f1
                     + (double)pCL->fUreaNSoilConcOld        * f2
                     - f4
                     + (double)pCL->pNext->fUreaNSoilConcOld * f3;
        break;
                    
        case 1:
        f4 = (double)(pCL->fNH4NSoilConcOld - pCL->fNH4NSoilConc)
           * (double)((pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT);
            
        pD[iLayer] = (double)pCL->pBack->fNH4NSoilConcOld * f1
                     + (double)pCL->fNH4NSoilConcOld * f2
                     - f4
                     + (double)pCL->pNext->fNH4NSoilConcOld * f3;
        break;
                     
        case 2:
        f4 = (double)((pCL->fNO3NSoilConcOld - pCL->fNO3NSoilConc)
           * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT);
            
        pD[iLayer]= (double)pCL->pBack->fNO3NSoilConcOld * f1
                    + (double)pCL->fNO3NSoilConcOld        * f2
                    - f4
                    + (double)pCL->pNext->fNO3NSoilConcOld * f3;
        break;
                    
	    case 3:
        f4 = (double)((pCL->fN2ONGasConcOld - pCL->fN2ONGasConc)
           * pWL->fContOld * pCL->fN2OKh / DeltaT);
            
        pD[iLayer]= (double)pCL->pBack->fN2ONGasConcOld * f1
                    + (double)pCL->fN2ONGasConcOld        * f2
                    - f4
                    + (double)pCL->pNext->fN2ONGasConcOld * f3;
        break;
        
        case 4:
        f4 = (double)((pCL->fDONSoilConcOld - pCL->fDONSoilConc)
           * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT); 
        
        pD[iLayer] = (double)pCL->pBack->fDONSoilConcOld * f1
                     + (double)pCL->fDONSoilConcOld      * f2
                     - f4
                     + (double)pCL->pNext->fDONSoilConcOld * f3;
        break;
        
        case 5:
        f4 = (double)((pCL->fDOCSoilConcOld - pCL->fDOCSoilConc)
           * (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[Stoff]) / DeltaT); 

		if(iMobImm==1)
		{ 
			pD[iLayer] =  (double)pCL->pBack->fDOCSltConcMobOld * f1
                        + (double)pCL->fDOCSltConcMobOld * f2
                        - f4
						- (double)pCh->pCParam->fMobImmSltEC 
						*(double)(pCL->fDOCSltConcMobOld - pCL->fDOCSltConcImmOld)
						//-(pWL->fContImmAct-pWL->fContImmOld)/DeltaT*pCL->fDOCSltConcMobOld
						//-(float)0.5*(pWL->fContImmAct-pWL->fContImmOld)/DeltaT*pCL->fDOCSltConcMobOld
						+ (double)pCL->pNext->fDOCSltConcMobOld * f3;
		}
		else
		{     
        pD[iLayer] = (double)pCL->pBack->fDOCSoilConcOld * f1
                     + (double)pCL->fDOCSoilConcOld * f2
                     - f4
                     + (double)pCL->pNext->fDOCSoilConcOld * f3;
		}

        break;
        } /*switch */

		        
    }/* Schicht */

    /* Löser der Tridiagonalen Matrix   Obere RandSchicht                        */
    /*  Obere Randbedingung  --------------------------------------------------- */

    pF[1] = pC[1] / pB[1];
    
    switch(Stoff)
    {
    case 0:
	pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fUreaNSoilConcOld) / pB[1];
    break;
    
    case 1:
	pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fNH4NSoilConcOld) / pB[1];
    break;
    
    case 2:
	pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fNO3NSoilConcOld) / pB[1];
    break;
    
    case 3:
	pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fN2ONGasConcOld) / pB[1];
    break;

    case 4:
	pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fDONSoilConcOld) / pB[1];
    break;

    case 5:
	if(iMobImm==1)
	{
	pG[1] = (pD[1]-pA[1]*(double)pCh->pCLayer->fDOCSltConcMobOld) / pB[1];
	}
	else
	{
	pG[1] = (pD[1] - pA[1] * (double)pCh->pCLayer->fDOCSoilConcOld) / pB[1];
	}
	break;
    }  

    /* Löser der Tridiagonalen Matrix   Profil - berechnung  */

    for (iLayer = 2;iLayer < pSo->iLayers-1;iLayer++)
     {
        pF[iLayer] = pC[iLayer]
                     / (pB[iLayer] - pF[iLayer-1] * pA[iLayer]);

        pG[iLayer] = (pD[iLayer] - pG[iLayer-1] * pA[iLayer])
                     / (pB[iLayer] - pF[iLayer-1] * pA[iLayer]);
    }

    /* Untere Randbedingung ---------------------------------------------------- */

	 /* Zeiger auf letzte Listenelemente setzten */
    for (SOIL_LAYERS0(pCL, pCh->pCLayer->pNext));
	     
	if (FreeD==FALSE)
	{
	 //for (SOIL2_LAYERS0(pCL, pCh->pCLayer->pNext, pWL, pWa->pWLayer->pNext));

     pF[pSo->iLayers-2] = pC[pSo->iLayers-2] / (pB[pSo->iLayers-2] + (double)pCL->pBack->fLiqDiffCoef/f1 -
		                   pF[pSo->iLayers-3] * pA[pSo->iLayers-2]);
	 pG[pSo->iLayers-2] = (pD[pSo->iLayers-2]
	                       - pG[pSo->iLayers-3]* pA[pSo->iLayers-2])// * pCh->pCLayer->fNO3NSoilConcOld))
                           /(pB[pSo->iLayers-2]+ (double)pCL->pBack->fLiqDiffCoef/f1 - pF[pSo->iLayers-3] * pA[pSo->iLayers-2]);
	}

	//if (pWa->pWLayer->fFluxDens>(float)0)
	{/* anstelle der mixing cell ep 3.11.94 */
    pCL->fUreaNSoilConc = pCL->pBack->fUreaNSoilConcOld;
    pCL->fNH4NSoilConc  = pCL->pBack->fNH4NSoilConcOld;
    pCL->fNO3NSoilConc  = pCL->pBack->fNO3NSoilConcOld;
    pCL->fN2ONGasConc   = pCL->pBack->fN2ONGasConcOld;
    pCL->fDONSoilConc   = pCL->pBack->fDONSoilConcOld;
    pCL->fDOCSoilConc   = pCL->pBack->fDOCSoilConcOld;
	if(iMobImm==1) pCL->fDOCSltConcMob   = pCL->pBack->fDOCSltConcMobOld;
	}
	//else
	{
	//pCL->fUreaNSoilConc = (float)0;//pG[pSo->iLayers-2];
    //pCL->fNH4NSoilConc  = (float)0;//pG[pSo->iLayers-2];
    //pCL->fNO3NSoilConc  = (float)0;//pG[pSo->iLayers-2];
    //pCL->fN2ONGasConc   = (float)0;//pG[pSo->iLayers-2];
    //pCL->fDONSoilConc   = (float)0;//pG[pSo->iLayers-2];
    //pCL->fDOCSoilConc   = (float)0;//pG[pSo->iLayers-2];
	}


    /* Zeiger auf vorletzte Listenelemente setzten */
    /* Rückwärts einsetzen */

    for (iLayer = pSo->iLayers-2, pCL = pCL->pBack;
         ((pCL->pBack!=NULL) && (iLayer > 0));
         pCL = pCL->pBack, iLayer--)
       { 
        switch(Stoff)
        {
        case 0:
	  	  d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fUreaNSoilConc;
          pCL->fUreaNSoilConc = (float)max(0.0,d1);
        break;
        
        case 1:
		  d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fNH4NSoilConc;
          pCL->fNH4NSoilConc = (float)max(0.0,d1);
        break;
        
        case 2:
		  d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fNO3NSoilConc;
          pCL->fNO3NSoilConc = (float)max(0.0,d1);
        break;
        
        case 3:
		  d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fN2ONGasConc;
          pCL->fN2ONGasConc = (float)max(0.0,d1);
        break;

        case 4:
	  	  d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fDONSoilConc;
          pCL->fDONSoilConc = (float)max(0.0,d1);
        break;

        case 5:
		  if(iMobImm==1)
		  {
		  d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fDOCSltConcMob;
          pCL->fDOCSltConcMob = (float)max(0.0,d1);
		  }
		  else
		  {
		  d1 = pG[iLayer] - pF[iLayer] * (double)pCL->pNext->fDOCSoilConc;
          pCL->fDOCSoilConc = (float)max(0.0,d1);
		  }
		break;
        }
    }

	   if((iMobImm==1)&&(Stoff==5))
	   {
		for (iLayer=1,
            pWL=pWa->pWLayer->pNext,
            pCL=pCh->pCLayer->pNext;
            ((pWL->pNext!=NULL)&&
             (pCL->pNext!=NULL));
            pWL = pWL->pNext,
            pCL = pCL->pNext,
            iLayer++)
			{ 
			 fAG = pWL->fContImmOld/DeltaT - (float)0.5 *  pCh->pCParam->fMobImmSltEC*(float)3;
             fBG = pWL->fContImmAct/DeltaT - (float)0.5 *  pCh->pCParam->fMobImmSltEC;
		     if (fBG==(float)0) fBG += (float)1e-9;
		     pCL->fDOCSltConcImm = fAG/fBG * pCL->fDOCSltConcImmOld 
			                      - (float)0.5*pCh->pCParam->fMobImmSltEC/fBG 
			                         *(pCL->fDOCSltConcMob - pCL->fDOCSltConcMobOld*(float)3);

		     pCL->fDOCSoilConc = (pWL->fContMobAct*pCL->fDOCSltConcMob + pWL->fContImmAct*pCL->fDOCSltConcImm)
		  	                    /pWL->fContAct;
		     //pCL->fDOCSoilConc = (pWL->fContMobOld*pCL->fDOCSltConcMob + pWL->fContImmOld*pCL->fDOCSltConcImm)
		     //                   /pWL->fContOld;
		     //pCL->fDOCSoilConc = pCL->fDOCSltConcMob;
		     //pCL->fDOCSoilConc = pCL->fDOCSltConcImm;
		     //pCL->fDOCSoilConc = pCL->fDOCSltConcMob+pCL->fDOCSltConcImm;

		   }
	   }

} /* for Stoff */


    /*------------------------------ Speicher für Schichtvariable Freigeben */
        free(pAB);
        free(pBB);
        free(pCB);
        free(pDB);

        free(pA);
        free(pB);
        free(pC);
        free(pD);

        free(pF);
        free(pG);

return 1;
} /* Solve_LGS */


/**********************************************************************************************/
/* Procedur    :   NTransp                                                                    */
/*                                                                                            */
/* Inhalt      :   Stickstoffverlagerung nach Hutson & Wagenet (LeachN 1994-Version 3.0)      */
/*                                                                                            */
/* Autoren     :   GSF: S.Storm/E.Priesack                  V1.0                              */
/* Datum       :   24.05.94                                                                   */
/*                                                                                            */
/**********************************************************************************************/
/* veränd. Var.		pCh->pCParam->afKd[0]                 pCh->pCParam->afKh[0]               */
/*					pCh->pCParam->afKd[1]                 pCh->pCParam->afKh[1]               */
/*					pCh->pCParam->afKd[2]                 pCh->pCParam->afKh[2]               */
/*					pCh->pCParam->fDispersivity           pCh->pCParam->fMolDiffCoef          */
/*					pCP->fN2OEmisR                        pCL->fN2OKh                         */
/*					pCL->fUreaN                           pCL->fNH4N                          */
/*					pCL->fNO3N                            pCL->fN2ON                          */
/*					pCL->fUreaNSoilConc                   pCL->fNH4NSoilConc                  */
/*					pCL->fNO3NSoilConc                    pCL->fN2ONGasConc                   */
/*					pCL->fUreaNSoilConcOld                pCL->fNH4NSoilConcOld               */
/*					pCL->fNO3NSoilConcOld                 pCL->fN2ONGasConcOld                */
/*					pSL->fImpedLiqA                       pSL->fImpedLiqB                     */
/*                  pCB->fNProfileStart                   pCB->fNInputCum                     */
/*                  pCP->fNO3NSurf                        pCP->fNH4NSurf                      */
/*                  pCP->fUreaNSurf                                                           */
/*                                                                                            */
/*					Get_Diffusion_Const()		pCL->fGasDiffCoef                             */
/*												pCL->fLiqDiffCoef                             */
/*					Solve_LGS()                 pCL->fLiqDiffCoef                             */
/*												pCL->fGasDiffCoef                             */
/*												pCL->fNH4NSoilConc                            */
/*												pCL->fNO3NSoilConc                            */
/*												pCL->fN2ONGasConc                             */
/*												pCL->fUreaNSoilConc                           */
/*                                                                                            */
/**********************************************************************************************/
int WINAPI NTransp(EXP_POINTER, int N2O_IN_SOLUTION)
{
    DECLARE_COMMON_VAR 

    PCLAYER    pCL;
    PCPROFILE  pCP ;
    PCBALANCE  pCB;	

    PCPARAM    pPA;
    PHLAYER    pHL;
    PSLAYER    pSL;
    PWLAYER    pWL;
   	//PSWATER	   pSW;

	float DeltaZ = pSo->fDeltaZ;
    //float DeltaT = pTi->pTimeStep->fAct;
    float fGasMid; 
	//float fPorosityOld;
    float f0;

    float     fDuengungAktuellNO3  = (float)0.0;
    float     fDuengungAktuellNH4  = (float)0.0;
    float     fDuengungAktuellUrea = (float)0.0;
    float     fDuengungAktuellDON  = (float)0.0;
    float     fDuengungAktuellDOC  = (float)0.0;
      
	pCP = pCh->pCProfile;
	pCB = pCh->pCBalance;
    pPA = pCh->pCParam;

/* ************************* Kh-Wert fuer N2O berechnen. ************ */
for ( SOIL2_LAYERS0(pCL, pCh->pCLayer->pNext, pHL, pHe->pHLayer->pNext))
    {
	pCL->fN2OKh = (N2O_IN_SOLUTION) ? Solut_Coeff(pHL->fSoilTemp) : (float)0.000001;
    }
	
//SG/07/05/99:	pCL != NULL durch pCL->pNext != NULL ersetzt!
for (pCL = pCh->pCLayer;(pCL->pNext != NULL);pCL = pCL->pNext)  
    {                                                    
    pCL->fUreaNSoilConcOld = pCL->fUreaNSoilConc;       
    pCL->fNH4NSoilConcOld  = pCL->fNH4NSoilConc;
    pCL->fNO3NSoilConcOld  = pCL->fNO3NSoilConc;		
    pCL->fN2ONGasConcOld   = pCL->fN2ONGasConc;
    pCL->fDONSoilConcOld = pCL->fDONSoilConc;       
	pCL->fDOCSoilConcOld = pCL->fDOCSoilConc;
	if (iMobImm==1)
	{
	pCL->fDOCSltConcMobOld = pCL->fDOCSltConcMob;       
	pCL->fDOCSltConcImmOld = pCL->fDOCSltConcImm;       
	}
	}//for
///*
if ((iMobImm==1)&&(SimStart(pTi)))//zu Testzwecken
   {
   for (pPA = pCh->pCParam;(pPA->pNext != NULL);pPA = pPA->pNext)
      {
      pPA->fMobImmSltEC=(float)0.05;
      pPA->fMobAdsSiteFrac=(float)1;
	  }

   for (pCL = pCh->pCLayer;(pCL->pNext != NULL);pCL = pCL->pNext)  
	   {
	   pCL->fDOCSltConcMobOld = (float)0;//iLayer;       
	   pCL->fDOCSltConcImmOld = (float)0;//iLayer;       
	   pCL->fDOCSltConcMob    = pCL->fDOCSltConcMobOld;       
	   pCL->fDOCSltConcImm    = pCL->fDOCSltConcImmOld;       
	   }

   }
//*/
   
 /*
  * ch: lpHF->FDichte   > 1 um floating point error zu vermeiden 
  * 	if Abfrage.
  */               
  
  if ((pWa->pWLayer->fFluxDens>(float)1.0)&&   
     ((pCP->fNO3NSurf > EPSILON)||(pCP->fNH4NSurf > EPSILON)||
     (pCP->fUreaNSurf > EPSILON)||(pCP->fDONSurf > EPSILON)||(pCP->fDOCSurf > EPSILON)))
     {

      pCL = pCh->pCLayer;         /*   0. Schicht  (virtuell) */  
      pSL = pSo->pSLayer->pNext; /*   1. Schicht */
      pWL = pWa->pWLayer->pNext;

      pCL->fNO3NSoilConcOld  = pCP->fNO3NSurf  * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
      pCL->fNH4NSoilConcOld  = pCP->fNH4NSurf  * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
      pCL->fUreaNSoilConcOld = pCP->fUreaNSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
      pCL->fDONSoilConcOld   = pCP->fDONSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
      pCL->fDOCSoilConcOld   = pCP->fDOCSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;
      if(iMobImm==1) pCL->fDOCSltConcMobOld = pCP->fDOCSurf * kgPhaTomgPsqm / pWa->pWLayer->fFlux;

      fDuengungAktuellNO3  = pCL->fNO3NSoilConcOld * pWa->pWLayer->fFlux /kgPhaTomgPsqm;
      pCP->fNO3NSurf      -= fDuengungAktuellNO3;
      pCP->fNO3NSurf       = max((float)0.0,pCP->fNO3NSurf);

      fDuengungAktuellNH4  = pCL->fNH4NSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
      pCP->fNH4NSurf      -= fDuengungAktuellNH4;
      pCP->fNH4NSurf       = max((float)0.0,pCP->fNH4NSurf);

      fDuengungAktuellUrea = pCL->fUreaNSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
      pCP->fUreaNSurf     -= fDuengungAktuellUrea;
      pCP->fUreaNSurf      = max((float)0.0,pCP->fUreaNSurf);

      fDuengungAktuellDON = pCL->fDONSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
      pCP->fDONSurf     -= fDuengungAktuellDON;
      pCP->fDONSurf      = max((float)0.0,pCP->fDONSurf);

      fDuengungAktuellDOC = pCL->fDOCSoilConcOld * pWa->pWLayer->fFlux / kgPhaTomgPsqm;
      pCP->fDOCSurf     -= fDuengungAktuellDOC;
      pCP->fDOCSurf      = max((float)0.0,pCP->fDOCSurf);
  
     } //fertilizer application

     

    Get_DON_TransformationRates(exp_p);
  
    for (iLayer = 1,
    	 pCL = pCh->pCLayer->pNext,
         pWL = pWa->pWLayer->pNext,
         pSL = pSo->pSLayer->pNext;
         ((pCL->pNext !=NULL)&&
          (pWL->pNext !=NULL)&&
          (pSL->pNext !=NULL)); //ep 200899 auf ->pNext !=NULL gesetzt, da fuer letzte virtuelle Schicht sinnlos?
         iLayer++,				//			und es tritt Fehler bei TestRange(Porosity - Water) auf!
         pCL = pCL->pNext,
         pWL = pWL->pNext,
         pSL = pSL->pNext)
    {
    pCL->fNH4N+= pCL->fDONMinerR*pCL->fDON*DeltaT;
    //pCL->fNH4N+= max((float)0,pCL->fDONMinerR*pCL->fDON);
        
    pCL->fDOMCN   = (pCL->fDON > EPSILON)? pCL->fDOC/pCL->fDON:(float)0.1; 
    f0 = (float)1 - pPA->fMinerEffFac * pCL->fDOMCN / pCL->fMicBiomCN;

    pCL->fDON += (pCL->fHumusToDONR*pSL->fNHumus + pCL->fLitterToDONR*pCL->fNLitter + pCL->fManureToDONR*pCL->fNManure
                - (pCL->fDONToHumusR + pCL->fDONToLitterR + pCL->fDONMinerR*f0)*pCL->fDON)*DeltaT;
    //pCL->fDOC += (pCL->fHumusToDONR*pSL->fCHumus + pCL->fLitterToDONR*pCL->fCLitter + pCL->fManureToDONR*pCL->fCManure
    //            - (pCL->fDONToHumusR + pCL->fDONToLitterR + pCL->fDONMinerR)*pCL->fDOC)*DeltaT;
    
    pSL->fNHumus  += (pCL->fDONToHumusR*pCL->fDON - pCL->fHumusToDONR*pSL->fNHumus)*DeltaT;
    pSL->fCHumus  += (pCL->fDONToHumusR*pCL->fDOC - pCL->fHumusToDONR*pSL->fCHumus)*DeltaT;
    pCL->fNLitter += (pCL->fDONToLitterR*pCL->fDON - pCL->fLitterToDONR*pCL->fNLitter)*DeltaT;
    pCL->fCLitter += (pCL->fDONToLitterR*pCL->fDOC - pCL->fLitterToDONR*pCL->fCLitter)*DeltaT;
    pCL->fNManure -= pCL->fManureToDONR*pCL->fNManure*DeltaT;
    pCL->fCManure -= pCL->fManureToDONR*pCL->fCManure*DeltaT;

    pCL->fUreaNSoilConc = pCL->fUreaN * (kgPhaTomgPsqm / (DeltaZ*
                         (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[0])));	
                         /*[kg N /ha /100 /mm] = kg N /m^2 10^-4 /100 /m 1000] */
                         /* = g N /m^3 = mg N /l */
    pCL->fNH4NSoilConc  = pCL->fNH4N  * (kgPhaTomgPsqm / (DeltaZ*
                         (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[1])));
    pCL->fNO3NSoilConc  = pCL->fNO3N  * (kgPhaTomgPsqm / (DeltaZ*
                         (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[2])));

    pCL->fDONSoilConc   = pCL->fDON  * (kgPhaTomgPsqm / (DeltaZ*
                         (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[4])));
    pCL->fDOCSoilConc   = pCL->fDOC  * (kgPhaTomgPsqm / (DeltaZ*
                         (pWL->fContOld + pSL->fBulkDens * pCh->pCParam->afKd[5])));

    if (!Test_Range(pSL->fPorosity - pWL->fContOld - pWL->fIceOld, EPSILON, (float)1, "(Porosity - Water)"))
    	{
		pSL->fPorosity = pWL->fContOld + pWL->fIceOld + (float)0.1 * pSL->fPorosity;
		//fPorosityOld = pWL->fContOld + pWL->fIceOld + (float)0.1 * pSL->fPorosity;
		//fPorosityOld = pWL->fContOld + pWL->fIceOld;
		//return -1;
    	}
/*    
        else
	    {
		fPorosityOld = pSL->fPorosity;
	    }
*/

    pCL->fN2ONGasConc   = pCL->fN2ON  * kgPhaTomgPsqm / (DeltaZ*
		//(pWL->fContOld * pCL->fN2OKh + (fPorosityOld - pWL->fContOld - pWL->fIceOld)));
        (pWL->fContOld * pCL->fN2OKh + (pSL->fPorosity - pWL->fContOld - pWL->fIceOld)));
        //(pWL->fContAct * pCL->fN2OKh + (pSL->fPorosity - pWL->fContAct - pWL->fIce)));
              
  //  if (SimStart(pTi))
		pCL->fN2ONGasConcOld = pCL->fN2ONGasConc;   
   }

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *		Bestimme Koeffizienten fuer Diffusion
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
 Get_Diffusion_Const(exp_p);

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *		Loese lin. Gleichungssystem
 * >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
 Solve_LGS(exp_p);


    
 /* Umrechnung der Konzentrationen */

 /* alle  Schichten  */
 for ( pCL = pCh->pCLayer,
       pWL = pWa->pWLayer,
       pSL = pSo->pSLayer;
     ((pCL !=NULL)&&(pWL !=NULL)&&(pSL !=NULL));
       pCL = pCL->pNext, 
       pWL = pWL->pNext, 
       pSL = pSL->pNext)
     {
     pCL->fUreaN = pCL->fUreaNSoilConc * DeltaZ *
                  (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[0])/(float)100.0;
     pCL->fNH4N  = pCL->fNH4NSoilConc  * DeltaZ *
                  (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[1])/(float)100.0;
     pCL->fNO3N  = pCL->fNO3NSoilConc  * DeltaZ *
                  (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[2])/(float)100.0;
     /* ch ? */
     pCL->fN2ON  = pCL->fN2ONGasConc   * DeltaZ *
                  //(pWL->fContAct * pCL->fN2OKh + fPorosityOld - pWL->fContAct - pWL->fIce)
                  (pWL->fContAct * pCL->fN2OKh + pSL->fPorosity - pWL->fContAct - pWL->fIce)
                 / kgPhaTomgPsqm;
     
     pCL->fDON = pCL->fDONSoilConc * DeltaZ *
                  (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[4])/(float)100.0;
     pCL->fDOC = pCL->fDOCSoilConc * DeltaZ *
                  (pWL->fContAct + pSL->fBulkDens * pCh->pCParam->afKd[5])/(float)100.0;
     }

	/*
	for(pSW=pSo->pSWater,
		pSL=pSo->pSLayer;
	  ((pSW!=NULL)&&(pSL!=NULL));
	    pSW=pSW->pNext,
	    pSL=pSL->pNext)
	{
	 pSL->fPorosity=pSW->fContSat;
	}
	*/

	/* ch 30.4.96 */
    /* Obere RB für gelöste Stoffe: q < 0 */
    if (pWa->pWLayer->fFlux < (float)0.0)
       {
       pWL = pWa->pWLayer;          /* Fluss in 1. oder 0. Schicht ? */
       pCL = pCh->pCLayer->pNext;
	
       pCL->fUreaN -= pWL->fFlux * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
                                 / (float)2.0 / kgPhaTomgPsqm;
                                 
       pCL->fNO3N  -= pWL->fFlux * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
                                 / (float)2.0 / kgPhaTomgPsqm;
                                 
       pCL->fNH4N  -= pWL->fFlux * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
                                 / (float)2.0 / kgPhaTomgPsqm;
        
       pCL->fN2ON  -= pWL->fFlux * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc) 
                                 *  pCL->fN2OKh
                                 / (float)2.0 / kgPhaTomgPsqm;

       pCL->fDON -= pWL->fFlux * (pCL->fDONSoilConcOld + pCL->fDONSoilConc)
                                 / (float)2.0 / kgPhaTomgPsqm;
       pCL->fDOC -= pWL->fFlux * (pCL->fDOCSoilConcOld + pCL->fDOCSoilConc)
                                 / (float)2.0 / kgPhaTomgPsqm;
                                 
       } /* q(i=0) < 0 */


/* ******************************************************************************/
/*  Bilanzierung */
/* ******************************************************************************/

	pPA = pCh->pCParam;  
	pCL = pCh->pCLayer;
	pCP = pCh->pCProfile;
	pCB = pCh->pCBalance;
	pWL = pWa->pWLayer;
    pSL = pSo->pSLayer;
    
/* 1. Oberer Rand:	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
	fGasMid = pSL->pNext->fPorosity 
	        - (pWL->pNext->fContAct+ pWL->pNext->fContOld + pWL->pNext->fIce + pWL->pNext->fIceOld)
	        / (float)2; 	 
    
    if (fGasMid > EPSILON)
	   pCP->fN2OEmisR = (pCL->fGasDiffCoef + pCL->pNext->fGasDiffCoef) / (float)2
					  * (pCL->pNext->fN2ONGasConc - pCL->fN2ONGasConc
					  +  pCL->pNext->fN2ONGasConcOld - pCL->fN2ONGasConcOld) / (float)2
					  * (float)1e5 / (float)24 / DeltaZ ;
                     /* [cm^2 /d * mg N /l /mm /1e5 * 24  ]
                     =  [e-4 m^2 /24 /h *e3 mu g *e3 /m^3 *e3 /m e-5 24]
                     =  [ mu g N /m^2 /h] */

	   pCP->fNOEmisR = (float)0;
	   pCP->fN2EmisR = (float)0;
  
  //pCP->fNFertilizerR existiert noch nicht !!!                   
  //pCP->fNFertilizerR = (fDuengungAktuellNO3 + fDuengungAktuellNH4 + fDuengungAktuellUrea) / DeltaT;

   if ((fDuengungAktuellNO3 > EPSILON)|| (fDuengungAktuellNH4 > EPSILON)||
	  (fDuengungAktuellUrea > EPSILON))
	  {
	   pCB->dNInputCum += (double)(fDuengungAktuellNO3 + fDuengungAktuellNH4 + fDuengungAktuellUrea);
                         
      }			


   if (pCP->fN2OEmisR > 0)
	  {
	   // pCP->fN2OEmisCum += pCP->fN2OEmisR  * (float)24e-5 * DeltaT;   dm, ist in cum_bil1.c
	  }
   else
	  {
	   pCB->dNInputCum  -= (double)pCP->fN2OEmisR * (double)24e-5 * (double)DeltaT; 
	  }
                   /* [µg N / qm / h / 24 * 10^5 * d] = [kg N/ha] */

  
      if (fDuengungAktuellDON > EPSILON)
      {
       //pCB->fDONInputCum += fDuengungAktuellDON; 
      } 

   
  /*******************************  Leaching ***************************************/

  for (pCL = pCh->pCLayer, pWL = pWa->pWLayer;
      ((pCL->pNext->pNext!=NULL)&&(pWL->pNext->pNext!=NULL));
      //((pCL->pNext!=NULL)&&(pWL->pNext!=NULL));
       pCL = pCL->pNext, pWL = pWL->pNext); // Zeiger auf vorletzte Schicht setzen

     if ((FreeD==TRUE)&&(pWL->fFlux > (float)0.0)) //pWa->fPercolR*DeltaT //???
     {
        pCP->fUreaLeachDay += pWL->fFlux
                                       * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm;
        pCP->fNH4LeachDay  += pWL->fFlux
                                       * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm;
        pCP->fNO3LeachDay  += pWL->fFlux
                                      * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
                                      / (float)2.0 / kgPhaTomgPsqm;
      // Variable noch nicht vorhanden
      //pCP->fN2ODrainDay  += pWL->fFlux
      //                               * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc) 
      //                               * pCL->fN2OKh
      //                               / (float)2.0 / kgPhaTomgPsqm;
                                       
        pCP->fDONLeachDay += pWL->fFlux
                                       * (pCL->fDONSoilConcOld + pCL->fDONSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm;
        pCP->fDOCLeachDay += pWL->fFlux
                                       * (pCL->fDOCSoilConcOld + pCL->fDOCSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm;

        pCP->dUreaLeachCum += (double)(pWL->fFlux
                                       * (pCL->fUreaNSoilConcOld + pCL->fUreaNSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm);
        pCP->dNH4LeachCum  += (double)(pWL->fFlux
                                       * (pCL->fNH4NSoilConcOld + pCL->fNH4NSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm);
        pCP->dNO3LeachCum  += (double)(pWL->fFlux 
                                      * (pCL->fNO3NSoilConcOld + pCL->fNO3NSoilConc)
                                      / (float)2.0 / kgPhaTomgPsqm);
        pCP->dN2ODrainCum  += (double)(pWL->fFlux
                                       * (pCL->fN2ONGasConcOld + pCL->fN2ONGasConc) 
                                       * pCL->fN2OKh
                                       / (float)2.0 / kgPhaTomgPsqm);
                
        pCP->dDONLeachCum += (double)(pWL->fFlux
                                       * (pCL->fDONSoilConcOld + pCL->fDONSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm);
        pCP->dDOCLeachCum += (double)(pWL->fFlux
                                       * (pCL->fDOCSoilConcOld + pCL->fDOCSoilConc)
                                       / (float)2.0 / kgPhaTomgPsqm);

        }
	 else
	    {
		pCP->fUreaLeachDay += pWa->fPercolR * DeltaT * (pCL->pBack->fUreaNSoilConc+pCL->fUreaNSoilConc)/(float)2 
			                                            / kgPhaTomgPsqm;
        pCP->fNH4LeachDay  += pWa->fPercolR * DeltaT * (pCL->pBack->fNH4NSoilConc+pCL->fNH4NSoilConc)/(float)2 
			                                            / kgPhaTomgPsqm;
        pCP->fNO3LeachDay  += pWa->fPercolR * DeltaT * (pCL->pBack->fNO3NSoilConc+pCL->fNO3NSoilConc)/(float)2 
			                                            / kgPhaTomgPsqm;
		///*
		pCP->dUreaLeachCum += (double)(pWa->fPercolR*DeltaT*(pCL->pBack->fUreaNSoilConc+pCL->fUreaNSoilConc)/(float)2/kgPhaTomgPsqm);
					         // +pCL->pBack->fLiqDiffCoef*DeltaT *(pCL->pBack->fUreaNSoilConc-pCL->fUreaNSoilConc)/DeltaZ); //auskommentiert 20180312 EP+SG
        pCP->dNH4LeachCum  += (double)(pWa->fPercolR*DeltaT*(pCL->pBack->fNH4NSoilConc+pCL->fNH4NSoilConc)/(float)2/kgPhaTomgPsqm);
							//  +pCL->pBack->fLiqDiffCoef*DeltaT *(pCL->pBack->fNH4NSoilConc-pCL->fNH4NSoilConc)/DeltaZ);//auskommentiert 20180312 EP+SG
        pCP->dNO3LeachCum  += (double)(pWa->fPercolR*DeltaT*(pCL->pBack->fNO3NSoilConc+pCL->fNO3NSoilConc)/(float)2/kgPhaTomgPsqm);
							//  +pCL->pBack->fLiqDiffCoef*DeltaT *(pCL->pBack->fNO3NSoilConc-pCL->fNO3NSoilConc)/DeltaZ);		//auskommentiert 20180312 EP+SG					
        //*/
		}

 return 1;
  
} /* NVerlagerung */



/*********************************************************************************/
/*  Name     : NTransportCERES                                                   */
/*                                                                               */
/*  Funktion : Aufgrund der Wasserbewegung zwischen den Simulationsschichten     */
/*             wird der darin geloeste Stickstoff zwischen den Schichten  aus-   */
/*             getauscht. Dabei kann Stickstoff nach oben oder nach unten be-    */
/*             wegt werden. Die Beschreibung erfolgt nach CERES/Godwin mit Ver-  */
/*             aenderungen von Engel/Schaaf                                      */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*             pCh->pCLayer->fNO3N                                               */
/*             pCh->pCProfile->fNTotalLeachDay                                   */
/*             pCh->pCProfile->fNTotalLeachCum                                   */
/*             pCh->pCBalance->fNO3NProfile                                      */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI NTransportCERES(EXP_POINTER)							                          
{
signed short int  OK=0;							

	OK += FertilMinInflow(exp_p);
	OK += NFlussCERES(exp_p);

  return OK;
}  /*  Ende Berechnung des N-Transportes */


/*********************************************************************************/
/*  Name     : NFlussCERES                                                       */
/*                                                                               */
/*  Funktion : Aufgrund der Wasserbewegung zwischen den Simulationsschichten     */
/*             wird der darin geloeste Stickstoff zwischen den Schichten  aus-   */
/*             getauscht. Dabei kann Stickstoff nach oben oder nach unten be-    */
/*             wegt werden. Im Originalmodell werden die Transportprozesse an    */
/*             Wassertrasnport gekoppelt. Zuerst wird der abwaertsgerichtete     */
/*             Fluss realisiert, dann die Evaporation berechnet und anschlie-    */
/*             ssend der aufwaertsgerichtete Fluss berechnet. Da diese Vorgeh-   */
/*             nsweise der Berechnung aus Programmtechnischen Gruenden nicht     */
/*             moeglich ist, werden die Fluesse in Expert-N gegeneinander auf-   */
/*             gerechnet. Abwaertsgerichtete Fluesse sind positiv, aufwaertge-   */
/*             richtete Fluesse negativ. Aus diesen Gruenden exisitiert in       */
/*             Expert-N lediglich eine Funktion zur Berechnung des N-Transportes.*/
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCh->pCLayer->fNO3N                                               */
/*             pCh->pCProfile->fNTotalLeachDay                                   */
/*             pCh->pCProfile->fNTotalLeachCum                                   */
/*             pCh->pCBalance->fNO3NProfile                                      */
/*                                                                               */
/*********************************************************************************/
signed short int NFlussCERES(EXP_POINTER)							                          
{
signed short int  OK=0;							

	int		i1;
	float	nitppm,nabwaerts,naufwaerts,miniNit,profilN,profilN2;
//	float	FluxTemp[15]={(float)0.0};
	float  *FluxTemp;
	PSLAYER		pSL;
	PCLAYER		pCL;
	PWLAYER		pWL;
    
	FluxTemp = (float *)_MALLOC((pSo->iLayers+1)*sizeof(float));
	for(i1=0;i1<=pSo->iLayers;i1++) FluxTemp[i1] = (float)0;
	  
	nabwaerts  = (float)0.0;          
	naufwaerts = (float)0.0;          
	profilN  = (float)0.0;          	
	profilN2 = (float)0.0;          		
    

	for(i1=0, pWL = pWa->pWLayer;
		(pWL->pNext != NULL);
		pWL = pWL->pNext,i1++)
	{              
		FluxTemp[i1] = pWL->fFlux;
	}	

    for (pSL = pSo->pSLayer->pNext,
         pCL = pCh->pCLayer->pNext,
         pWL = pWa->pWLayer->pNext,i1=1;
                                   ((pSL->pNext != NULL)&&
                                    (pCL->pNext != NULL)&&
                                    (pWL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext,
         pWL = pWL->pNext,i1++)
    {        
         profilN += pCL->fNO3N;
	}


    for (pSL = pSo->pSLayer->pNext,
         pCL = pCh->pCLayer->pNext,
         pWL = pWa->pWLayer->pNext,i1=1;
                                   ((pSL->pNext != NULL)&&
                                    (pCL->pNext != NULL)&&
                                    (pWL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext,
         pWL = pWL->pNext,i1++)
    {
	 if (pWL->fFlux > (float)0.0) /* abwaertsgerichteter Transport */
     {
		nabwaerts = (float) 0.0;
		nitppm = (pCL->fNO3N / pCL->fPpmTOkg);
		if (nitppm > (float)0.1) /* abwaertsgerichteter Transport nur wenn N-Konz. gross genug */
		{
          nabwaerts = (pCL->fNO3N * pWL->fFlux 
          					/ (pWL->fContAct * pSL->fThickness + pWL->fFlux));
		  miniNit = ((float)0.1 * pCL->fPpmTOkg);         					
		  if (nabwaerts > (pCL->fNO3N - miniNit))
		  {
		    nabwaerts = (pCL->fNO3N - miniNit);
		  }

		  pCL->fNO3N = pCL->fNO3N - nabwaerts;

		/* Nitrat-N, dass aus der letzten Schicht nach unter verlagert wird,
		   gilt aus ausgewaschen und wird aus dem Bodenprofil entfernt. Andernfalls
		   wird es zumNitrat-N der naechsten Schicht addiert.*/
			if (pCL->pNext->pNext == NULL)
			{
        	 	pCh->pCProfile->fNO3LeachDay  += nabwaerts;
        	 	pCh->pCProfile->dNO3LeachCum  += (double)nabwaerts;
			} /* Ende Auswaschung */
			else
			{
		  		pCL->pNext->fNO3N = (pCL->pNext->fNO3N + nabwaerts);		  			
			}
		} /* Ende if (nitppm > (float)0.1) */		

      } /* Ende abwaertsgerichteter Fluss */		

	 if (pWL->fFlux < (float)0.0) /*aufwaertsgerichteter Fluss */
     {
		naufwaerts = (float) 0.0;
		nitppm = (pCL->fNO3N / pCL->fPpmTOkg);
		if (nitppm > (float)0.1) /* aufwaertsgerichteter Transport nur wenn N-Konz. gross genug */
		{
		/* Aufwaertsgerichteter Fluss ist negativ. 
				Daher wird naufwaerts * (float)-1.0 gerechnet. */
          naufwaerts = (pCL->fNO3N * pWL->fFlux 
          					/ (pWL->fContAct * pSL->fThickness - pWL->fFlux));
          naufwaerts *= (float)-1.0;

		  miniNit = ((float)0.1 * pCL->fPpmTOkg);         					
		  if (naufwaerts > (pCL->fNO3N - miniNit))
		  {
		    naufwaerts = (pCL->fNO3N - miniNit);
		  }

		  pCL->fNO3N = pCL->fNO3N - naufwaerts;
  		  pCL->pBack->fNO3N = (pCL->pBack->fNO3N + naufwaerts);		  			

		} /* Ende if (nitppm > (float)0.1) */		

	 } /* Ende aufwaertsgerichteter Fluss */		


	} /* Ende der schichtweisen Berechnung */

    for (pSL = pSo->pSLayer->pNext,
         pCL = pCh->pCLayer->pNext,
         pWL = pWa->pWLayer->pNext,i1=1;
                                   ((pSL->pNext != NULL)&&
                                    (pCL->pNext != NULL)&&
                                    (pWL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext,
         pWL = pWL->pNext,i1++)
    {        
     profilN2 += pCL->fNO3N;        
	}

	free(FluxTemp);
  
  return OK;
}  /*  Ende Berechnung Stickstoffbewegung nach unten im Profil */


/*********************************************************************************/
/*  Name     : FertilMinInflow                                                     */
/*                                                                               */
/*  Funktion : Die in die Oberflaechenpools eingetragenen Duenger, Ernteruek-    */
/*             staende und abgestorbene Biomasse wird auch bei Unterlassen einer */
/*             Bodenbearbeitungsmassnahme in die oberste Schicht eingetragen.    */
/*             Diese Vorgaenge werden in dieser Funktion berechnet.              */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCh->pCProfile->fNO3NSurf                                         */
/*             pCh->pCProfile->fNH4NSurf                                         */
/*             pCh->pCProfile->fUreaNSurf                                        */
/*             pCh->pCLayer->pNext->fNO3N                                        */
/*             pCh->pCLayer->pNext->fNH4N                                        */
/*             pCh->pCLayer->pNext->fUreaN                                       */
/*             pCh->pCProfile->fCLitterSurf                                      */
/*             pCh->pCProfile->fNLitterSurf                                      */
/*             pCh->pCLayer->pNext->fCLitter                                     */
/*             pCh->pCLayer->pNext->fNLitter                                     */
/*             pCB->fNInputCum                                                   */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI FertilMinInflow(EXP_POINTER)
{

	float	no3,nh4,urea; 

    PCBALANCE  pCB;	
    pCB = pCh->pCBalance;
	
	if (NewDay(pTi)&&(pCl->pWeather->fRainAmount>EPSILON))
	{
	   if ((pCh->pCProfile->fNO3NSurf > EPSILON))
	   	{
			no3 = pCh->pCProfile->fNO3NSurf * (float)0.1 * pCl->pWeather->fRainAmount;
			
			if (no3 > pCh->pCProfile->fNO3NSurf)
				{
				no3 = pCh->pCProfile->fNO3NSurf;
				}
		
			pCh->pCLayer->pNext->fNO3N += no3;
			pCh->pCProfile->fNO3NSurf -=  no3;
		
		    pCB->dNInputCum += (double)no3;
	      }			
		
	   if ((pCh->pCProfile->fNH4NSurf > EPSILON))
	   	{
		nh4 = pCh->pCProfile->fNH4NSurf * (float)0.1 * pCl->pWeather->fRainAmount;

			if (nh4 > pCh->pCProfile->fNH4NSurf)
			{
			nh4 = pCh->pCProfile->fNH4NSurf;
			}

			pCh->pCLayer->pNext->fNH4N += nh4;
			pCh->pCProfile->fNH4NSurf -= nh4;
		    
		    pCB->dNInputCum += (double)nh4;
		   } 
	
	   if ((pCh->pCProfile->fUreaNSurf > EPSILON))
	   	{
		urea = pCh->pCProfile->fUreaNSurf * (float)0.1 * pCl->pWeather->fRainAmount;
		
		if (urea > pCh->pCProfile->fUreaNSurf)
		{
		urea = pCh->pCProfile->fUreaNSurf;
		}
	
		pCh->pCLayer->pNext->fUreaN += urea;

		pCh->pCProfile->fUreaNSurf -= urea;
	    pCB->dNInputCum += (double)urea;
		} 
	}
							
  return 1;
}      /*===== Ende der Initialisierung der FertilMinInflow =============*/



/*********************************************************************************/
/*  Name     : EmitAllN2O                                                        */
/*                                                                               */
/*  Funktion : Emittiert alles im Profil vorhandene N2O sofort.                  */
/*             Mithilfe dieser Funktion kann ein weiterer Abbau N2O -> N2        */
/*             verhindert werden.                                                */
/*  Autor    : Ch. Haberbosch                                                    */
/*  Datum	 : 17.07.97                                                          */
/*                                                                               */
/*  Change   : pCh->pCLayer->pNext->fNLitter                                     */
/*                                                                               */
/*********************************************************************************/
int WINAPI EmitAllN2O(EXP_POINTER)
{
        int   iLayer;
        float fN2O_In_Profile = (float)0;
        float fNO_In_Profile  = (float)0;
        float fN2_In_Profile  = (float)0;
        
        PCLAYER      pCL= pCh->pCLayer;

        pCL->fN2ONGasConc               =
        pCL->fN2ONGasConcOld            = (float)0; 
		/* Athmosphaerische N2O-Konzentration nicht beachten */
        
     
		 for(SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
		 	{            
		 	if (pCL->fN2ON > EPSILON)
		 	{
			 fN2O_In_Profile += pCL->fN2ON; 
			 fNO_In_Profile  += pCL->fNON; 
			 fN2_In_Profile  += pCL->fN2N; 
			}
		
		    pCL->fN2ONGasConc    =
		    pCL->fN2ONGasConcOld =
		    pCL->fN2ON           =

		    pCL->fNON            = 
		    pCL->fN2N            = (float)0;
			}    
		
		pCh->pCProfile->fN2OEmisR = fN2O_In_Profile/((float)24e-5 * pTi->pTimeStep->fAct);
			/*  [ mu g N /m^2 /h] */
		pCh->pCProfile->fNOEmisR  = fNO_In_Profile/((float)24e-5 * pTi->pTimeStep->fAct);
			/*  [ mu g N /m^2 /h] */
		pCh->pCProfile->fN2EmisR  = fN2_In_Profile/((float)24e-5 * pTi->pTimeStep->fAct);
			/*  [ mu g N /m^2 /h] */
			
return 1;        
}      /* EmitAllN2O */

/*******************************************************************************
** EOF */
