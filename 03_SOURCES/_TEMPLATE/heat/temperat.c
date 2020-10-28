/*******************************************************************************
 *
 * Copyright 2001 - 2002 (c) by ZeusSoft, Ing. Buero Bauer
 *                              www.zeussoft.de
 *                              Tel: 0700 zeussoft
 *
 * Author:  Christian Bauer
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Sammlung der Modellansaetze zur Berechnung des Waermetransportes im Boden.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 5 $
 *
 * $History: temperat.c $
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:03
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/heat
 * Fehler durch nicht initialisierte Variablen behoben.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:40
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/heat
 * Using _MALLOC macro to detect memory leaks.
 * 
 *   Zusammenfuehren der Funktionen erfolgte am 12.03.97
 *
*******************************************************************************/

#include <crtdbg.h>
#include "xinclexp.h"
#include "xlanguag.h"     
#include "xtempdef.h"
#include "xh2o_def.h"  //wegen Temp_SHAW()

#undef FREEZING_SETS_MINIMAL_TIMESTEP

#undef LOG_FREEZING

/* from time1.c */
extern int NewDay(PTIME);
extern int NewTenthDay(PTIME);
extern int SimStart(PTIME);      

/* from util_fct.c */
extern int   WINAPI Test_Range(float x, float x1, float x2, LPSTR c);
extern int   WINAPI Set_Minimal_Time_Step(PTIME);
extern float abspower(float,float);
extern int   WINAPI Set_At_Least_Epsilon(float *x, LPSTR c);

/* from temp_bc.c */
extern float WINAPI Get_Daisy_LowerBC(EXP_POINTER);
extern float WINAPI Get_Horton_UpperBC(EXP_POINTER);
/* from util_fct.c */
extern int   WINAPI Message(long, LPSTR);

/* function declarations: */
int WINAPI Get_Cond_Cap_Heat(EXP_POINTER);
int WINAPI Heat_Balance(EXP_POINTER);
int WINAPI Solve_LGS_T(EXP_POINTER, int Frost_Rad_Flag);

int WINAPI SlopeIce(EXP_POINTER, int iter);
int WINAPI IceContent(EXP_POINTER);
int WINAPI Solve_LGS_CS(EXP_POINTER);
int WINAPI Solve_explicit(EXP_POINTER);
int Freezing(EXP_POINTER);

/* export functions: */
int WINAPI BodenTempTi(EXP_POINTER);
int WINAPI Temp_D(EXP_POINTER, int Frost_Rad_Flag); 
int WINAPI WaermeFlussWilliams(EXP_POINTER);
int WINAPI ConstTemp(EXP_POINTER);
int WINAPI Temp_SHAW(EXP_POINTER, int Frost_Rad_Flag);

/*
static float *pSlope;
static float *pkSat;
static float *pContSat;
static float *pSlopeOld;
static float *pSlopeOldOld;
static float *pIce;
static float *pTemp;
static int i,m;
*/
static float FAR pSlope[MAXSCHICHT], pSlopeOld[MAXSCHICHT], pSlopeOldOld[MAXSCHICHT];
static float FAR pIce[MAXSCHICHT], pTemp[MAXSCHICHT];
static float FAR pContSat[MAXSCHICHT], pkSat[MAXSCHICHT];

extern float fSnowOutflow,fSnowDensity,fSnowHeight,fSnowStorage;

float  fSoluteConc = (float)0.1;     // Stoffkonzentration mol/kg
float  fR          = (float)8.314;   // Gaskonstante J/(mol K)
float  fg          = (float)9.81;    // Gravity N/kg

float fRedTAmpYear;

/*********************************************************************************************/
/* Name             : BodenTempTi()                                                          */       
/* Inhalt			: Berechnung  Bodentemperaturen nach Tillotson                           */  
/*                                                                                           */
/* Autor            : C.Sperr                                                                */
/* Datum            : 28.01.93                                                               */
/*                                                                                           */
/*                   as, 27.11.96   Header: Einf. verän.Var.                                 */
/*                                                                                           */
/*********************************************************************************************/
/*   veränd. Var.:			pHL->fSoilTemp                                                   */
/*							pSL->afPartVolFrac[3]                                            */
/*							pSH->afPartHeatCond[ ]                                           */
/*********************************************************************************************/
int WINAPI BodenTempTi(EXP_POINTER)
{
  int idummy1,idummy2;

  float  GeomLuftPart[3];
  float  af4[4];
  float  cmTomm = (float)1.0;

  float  f1,f2,f3,f4,f5,f6,f7,f8,f9,f10;
  float*  pDeltaZincm;
  float*  paf1;
  float*  paf2;
  float*  paf3;
  float*  pAA;
  float*  pBB;
  float*  pA;
  float*  pB;
  float*  pC;
  float*  pD;

  double*  pF;
  double*  pG;

  static   float  TZalt = (float)0.0;
  static   int    Tcount= 0;
  static   float  T01   = (float)0.100000;
  
  PSLAYER           pSL;
  PWLAYER           pWL;
  PHLAYER           pHL;
 
                 
  pSo->pSHeat->afPartHeatCond[0] = (float)8.8;     /*8.8  */
  pSo->pSHeat->afPartHeatCond[1] = (float)2.9;     /*2.9  */
  pSo->pSHeat->afPartHeatCond[2] = (float)0.25;    /*0.25  */
  pSo->pSHeat->afPartHeatCond[3] = (float)0.6;     /*0.6    */
  pSo->pSHeat->afPartHeatCond[4] = (float)0.025;   /*0.025 */

 
 /*---------Berechnung soll mindestens in 10tel Tagesabständen erfolgen-------------- */
 if(NewTenthDay(pTi))
 {
    /* ------------- Speicher für Schichtvariable reservieren */

    paf1        = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(paf1,0x0,(pSo->iLayers * sizeof(float)));
    paf2        = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(paf2,0x0,(pSo->iLayers * sizeof(float)));
    paf3        = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(paf3,0x0,(pSo->iLayers * sizeof(float)));
    pDeltaZincm = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(pDeltaZincm ,0x0,(pSo->iLayers * sizeof(float)));
    pAA         = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(pAA,0x0,(pSo->iLayers * sizeof(float)));
    pBB         = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(pBB,0x0,(pSo->iLayers * sizeof(float)));
    pA          = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(pA,0x0,(pSo->iLayers * sizeof(float)));
    pB          = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(pB,0x0,(pSo->iLayers * sizeof(float)));
    pC          = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(pC,0x0,(pSo->iLayers * sizeof(float)));
    pD          = (float *) _MALLOC(pSo->iLayers * sizeof(float));
    memset(pD,0x0,(pSo->iLayers * sizeof(float)));
    pF          = (double *) _MALLOC(pSo->iLayers * sizeof(double));
    memset(pF,0x0,(pSo->iLayers * sizeof(double)));
    pG          = (double *) _MALLOC(pSo->iLayers * sizeof(double));
    memset(pG,0x0,(pSo->iLayers * sizeof(double)));
                                                                       
    /*-------------- Speicher für Schichtvariable reserviert */


 
  /*------------   Umrechnung der Schichtdicken von  [mm] zu [cm]---------------------------- */
  // pDeltaZincm[0] = pSo->pSLayer->pNext->fThickness / cmTomm;        dm, Angabe ist schon in mm
  pDeltaZincm[0] = pSo->pSLayer->pNext->fThickness;
  
  for (idummy1=1,pSL = pSo->pSLayer->pNext;
     ((idummy1<pSo->iLayers-1) && (pSL->pNext!= NULL));
       idummy1++,pSL = pSL->pNext)
      {
       // pDeltaZincm[idummy1] = pSL->fThickness / cmTomm;             dm, Angabe ist schon in mm
       pDeltaZincm[idummy1] = pSL->fThickness;
      }
      
  // pDeltaZincm[pSo->iLayers-1] = pSL->pBack->fThickness / cmTomm;    dm, Angabe ist schon in mm
  pDeltaZincm[pSo->iLayers-1] = pSL->pBack->fThickness;
 
  f10 = pSo->pSHeat->afPartHeatCond[3];


  for (idummy1 = 0;idummy1 <pSo->iLayers;idummy1++)
    {
    paf1[idummy1] = (float)0.0;
    paf2[idummy1] = (float)0.0;
    }


  /*------------   Berechnungen für alle Schichten des Bodenprofils--------------------- */
  for(idummy1 = 1,                  /* ab 2. Schicht */
      pWL = pWa->pWLayer->pNext,
      pSL = pSo->pSLayer->pNext;
     ((idummy1 <pSo->iLayers)&&     /* bis zur untersten */
      (pWL != NULL)&&
      (pSL != NULL));
      idummy1++,                    /* pro Schicht */
      pWL  = pWL->pNext,
      pSL  = pSL->pNext)
  {
   pSo->pSHeat->afPartHeatCond[3] = f10;

   paf1[idummy1] = (pWL->fContAct + pWL->fContOld + pWL->pBack->fContAct + pWL->pBack->fContOld)
                   / (float)4.0;

   if      (idummy1 == 1)
           paf1[idummy1] = (pWL->fContAct + pWL->fContOld)/(float)2.0;

   else if (idummy1 == (pSo->iLayers-1))
           paf1[idummy1] =  paf1[idummy1-1];


   paf2[idummy1] = ((float)0.2 * pSL->fBulkDens)  +  paf1[idummy1];



   if (pWL->fContAct >= (float).20)
      f1 = pSo->pSHeat->afPartHeatCond[4];
   else
      f1 = pSo->pSHeat->afPartHeatCond[3];


   /*    K - Werte zur Berechnung der therm. Leitfähigkeit */
   f2 = (float)0.0;
   f3 = (float)0.0;
   f4 = (float)0.0;
   
   for (idummy2=0;idummy2 <3;idummy2++)
   {
    f2 += (float)1./((float)1.+((pSo->pSHeat->afPartHeatCond[0]/f1)-(float)1.)* pSo->pSHeat->afPartGeom[idummy2][0]);
    f3 += (float)1./((float)1.+((pSo->pSHeat->afPartHeatCond[1]/f1)-(float)1.)* pSo->pSHeat->afPartGeom[idummy2][2]);
    f4 += (float)1./((float)1.+((pSo->pSHeat->afPartHeatCond[2]/f1)-(float)1.)* pSo->pSHeat->afPartGeom[idummy2][1]);
   }
   
   af4[0]=f2/(float)3.0;
   af4[1]=f3/(float)3.0;
   af4[2]=f4/(float)3.0;

   /*------Berechnung Geometrie Luft - Partikel    --------------------------------- */

   if (pWL->fContAct >= (float)0.20)
      GeomLuftPart[0] = (pWL->fContAct - (float)0.0845)/(float)1.10;
   else
      GeomLuftPart[0] = (pWL->fContAct - (float)0.0333)/(float)2.22;

   GeomLuftPart[1] = GeomLuftPart[0];
   GeomLuftPart[2] = (float)1.0 -  GeomLuftPart[0] - GeomLuftPart[1];

   /*-----Thermische  Leitfähigkeit der Gasphase  [cal/cm/s/Celsius] */

   if (pWL->fContAct >= (float)0.20)
       f5 = (float)0.0001760;
   else
       f5 = pWL->fContAct / (float)1136.0;


   pSo->pSHeat->afPartHeatCond[3] += f5;

   f6 = (float)0.0; 
   
   for (idummy2 =0;idummy2 < 3;idummy2++)
     {
      f6 += (float)1./((float)1.+((pSo->pSHeat->afPartHeatCond[3] / f1 -(float)1.) 
                                                        * GeomLuftPart[idummy2]));
     }
   
   af4[3]=f6/(float)3.0;

  /*----------------------------------------------------------------------- */

  f7 =(float)0.0;
  f8 =(float)0.0;


  for (idummy2=0;idummy2 < 4;idummy2++)
  {
   if (idummy2==3) pSL->afPartVolFrac[3]= (float)1.0 - pWL->fContAct
                                                     - pSL->afPartVolFrac[0]
                                                     - pSL->afPartVolFrac[1]
                                                     - pSL->afPartVolFrac[2];

   f7 += (af4[idummy2] * pSL->afPartVolFrac[idummy2] * pSo->pSHeat->afPartHeatCond[idummy2]);
   f8 += (af4[idummy2] * pSL->afPartVolFrac[idummy2]);

  }


  if (pWL->fContAct >= (float) 0.20)
     f9 = paf1[idummy1];
  else                               
     f9 = (float)1.0  - paf1[idummy1] - pSL->afPartVolFrac[0]
                                      - pSL->afPartVolFrac[1]
                                      - pSL->afPartVolFrac[2];


  f7  += (f1 * f9);
  f8  +=  f9;

  /*------- thermische Leitfähigkeit: dt/2    dZ/2 --------------------------------- */

   paf3[idummy1] = (f7/f8) * (float)3600.0 * (float)24.0;

   
   if (pWL->fContAct <= (float)0.20)
      paf3[idummy1] *= (float)1.65;

   if ((!lstrcmp(pSL->acSoilID,"Str\0"))||
				(!lstrcmp(pSL->acSoilID,"Hum\0")))
   {
    paf3[idummy1] = ((float)0.2 * pWL->fContAct + (float)0.015) 
		           //((float)1.22 * pWL->fContAct/pSL->fPorosity + (float)0.14) 
		           * (float)3600.0 * (float)24.0;
   }
  
  } /* Ende for (alle Schichten) */



 pSo->pSHeat->afPartHeatCond[3] = f10;

 /*-------------- Temperaturbedingung Bodenoberfläche  -------------------------------- */
 pHe->pHLayer->fSoilTemp = pCl->pWeather->fTempAve;
 //-(float)max((double)0,(double)0.25*(double)pCl->pWeather->fTempAve);
 /*
   + ((pCl->pWeather->fTempMax - pCl->pWeather->fTempMin)/(float)2)
      * (float)sin((float)2.0 * (float)PI * (pTi->pSimTime->fTimeDay - (float)0.25 ));
 */

  /* --------------- Koeffizienten  für die tridiagonale Matrix ------------------------ */

 #ifdef DEBUG
   fwriteDbg_line(3);
   fwriteDbg_string(" -----  coefficients ------!");
   fwriteDbg_line(1);
   fwriteDbg_string("Layer");
   fwriteDbg_space(6);
   fwriteDbg_string("pA[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pB[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pC[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pD[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pAA[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pBB[]");
   fwriteDbg_line(1);
   fwriteDbg_string("paf2[]");
   fwriteDbg_space(6);
   fwriteDbg_string("paf3[]");
   fwriteDbg_space(6);
   fwriteDbg_string("BodenTemp");
   fwriteDbg_line(1);
 #endif 

  for (idummy1 = 1, pHL = pHe->pHLayer->pNext;                          /*Schicht 2 bis vorletzte */
     ((idummy1 <= (pSo->iLayers - 2)) && (pHL->pNext != NULL));
       pHL = pHL->pNext, idummy1++)
  {
   pAA[idummy1] = ((float)0.1 * paf3[idummy1])
                 /((float)2.0 * paf2[idummy1] *  pDeltaZincm[idummy1] * pDeltaZincm[idummy1]);
   pBB[idummy1] = ((float)0.1 * paf3[idummy1 + 1])
                 /((float)2.0 * paf2[idummy1 + 1] *  pDeltaZincm[idummy1] * pDeltaZincm[idummy1]);

   pA[idummy1]  =  (float)-1.0 * pAA[idummy1];
   pB[idummy1]  =                pAA[idummy1] + pBB[idummy1] + (float)1.0;
   pC[idummy1]  =  (float)-1.0 * pBB[idummy1];
   pD[idummy1]  =  pAA[idummy1] * pHL->pBack->fSoilTemp;
   pD[idummy1]  += (((float)1.0 - pAA[idummy1] - pBB[idummy1])* pHL->fSoilTemp);
   pD[idummy1]  += (pBB[idummy1] * pHL->pNext->fSoilTemp);

  #ifdef DEBUG   
   fwriteDbg_int(&idummy1);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pA[idummy1]),7);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pB[idummy1]),7);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pC[idummy1]),7);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pD[idummy1]),7);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pAA[idummy1]),7);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pBB[idummy1]),7);
   fwriteDbg_line(1);
   fwriteDbg_float(&(pDeltaZincm[idummy1]),1);
   fwriteDbg_space(6);
   fwriteDbg_float(&(paf2[idummy1]),7);
   fwriteDbg_space(6);
   fwriteDbg_float(&(paf3[idummy1]),7);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pHL->fSoilTemp),1);
   fwriteDbg_line(1);
  #endif   

  }               
  

/* ----------------------------------------------------------------------------------- */
/*                   Löser der tridiagonalen Matrix                                    */
/* ----------------------------------------------------------------------------------- */

  pF[1] = (double)pC[1]/(double)pB[1];
  pG[1] = ((double)pD[1] - (double)pA[1] * (double)pHe->pHLayer->fSoilTemp) / (double)pB[1];
    

 /* --- vorwärts einsetzen --- */

 #ifdef DEBUG
   fwriteDbg_line(3);
   fwriteDbg_string(" -----  forward  solution ------!");
   fwriteDbg_line(1);
   fwriteDbg_string("Layer");
   fwriteDbg_space(6);
   fwriteDbg_string("pF[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pG[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pA[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pB[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pC[]");
   fwriteDbg_space(6);
   fwriteDbg_string("pD[]");
   fwriteDbg_space(6);
   fwriteDbg_line(1);
 #endif

   
 for (idummy1=2; idummy1 <= (pSo->iLayers - 2);  idummy1++)
  {

  pF[idummy1] =  (double)pC[idummy1]
              / ((double)pB[idummy1] - pF[idummy1 - 1] * (double)pA[idummy1]);

  pG[idummy1] = ((double)pD[idummy1] - (double)pA[idummy1] * pG[idummy1-1])
              / ((double)pB[idummy1] - pF[idummy1 - 1] * (double)pA[idummy1]);


  #ifdef DEBUG
   fwriteDbg_int(&idummy1);
   fwriteDbg_space(6);
   fwriteDbg_double(&(pF[idummy1]),5);
   fwriteDbg_space(6);
   fwriteDbg_double(&(pG[idummy1]),5);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pA[idummy1]),5);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pB[idummy1]),5);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pC[idummy1]),5);
   fwriteDbg_space(6);
   fwriteDbg_float(&(pD[idummy1]),5);
   fwriteDbg_line(1);
  #endif            
 
  } // for
  
  
  /* --- Temperaturbedingung  unterer Rand Bodenprofil --- */
  /* untere Randbedingung */
  /* Fall 1 : Konstante Temperatur */
  /* Fall 2 : Keine Wärmebewegung */
  
     
  for(pHL  = pHe->pHLayer;     /* setzen auf letzte Schicht */
      pHL->pNext != NULL;
      pHL  = pHL->pNext);

  pHL->fSoilTemp = (float)-1.0 * pHL->fSoilTemp + pHL->pBack->fSoilTemp
                 + (float)pG[pSo->iLayers - 2];

  pHL->fSoilTemp = pHL->fSoilTemp / ((float)1.0 + (float)pF[pSo->iLayers - 2]);
 
  /* --- rückwärts einsetzen --- */
  pHL = pHL->pBack;       

  for(idummy1 =(pSo->iLayers - 2);
    ((pHL->pBack != NULL) && (idummy1 > 0));
      pHL = pHL->pBack, idummy1--)    /* rückwärts bis zweite Schicht */
   {
    pHL->fSoilTemp = (float)pG[idummy1] -  (float)pF[idummy1] * pHL->pNext->fSoilTemp;
	if (idummy1 == (int)1) pHL->fSoilTemp = pCl->pWeather->fTempAve;//pHe->pHLayer->fSoilTemp;
	//min(pHL->fSoilTemp,pHe->pHLayer->fSoilTemp);//pCl->pWeather->fTempAve;
   }
   /*--- Ende Löser DGL --- */
    
  /* Speicher für Schichtvariable freigeben */

  free(paf1);
  free(paf2);
  free(paf3);
  free(pDeltaZincm);
  free(pAA);
  free(pBB);
  free(pA);
  free(pB);
  free(pC);
  free(pD);
  free(pF);
  free(pG);

  return 0;
 }  /* if 10tel Tag */

else  return 1;
} /* end  all */

/*********************************************************************************************/
/* Name             : Get_Cond_Cap_Heat(exp_p)                                               */       
/* Inhalt			: Interne Funktion zur Berechnung von Waermekapazitaeten, Waerme         */  
/*                    Waermeleitfaehigkeiten (nach De Vries mit Hilfe von Geometrie-Faktoren)*/
/* Autor            : C.Sperr                                                                */
/* Datum            : 10.10.95                                                               */
/*                                                                                           */
/*   as, 27.11.96 	Header: Variablenveränderungen erfaßt.                                   */                                        
/*                                                                                           */
/*********************************************************************************************/
/*   veränd. Var.:			pHL->fCapacity                                                   */
/*							pHL->fCapacityOld                                                */
/*							pHL->fHeat                                                       */
/*							pHL->fHeatLatent                                                 */
/*							pHL->fConduct                                                    */
/*							pHL->fConductOld                                                 */
/*********************************************************************************************/
int WINAPI Get_Cond_Cap_Heat(EXP_POINTER)
{
 DECLARE_TEMP_POINTER
 DECLARE_COMMON_VAR
  
 int i, k;
 int iContinMed;   /* Nummer des kontinuierlichen Mediums */
 float f1,f2;
 float fform[6]; 

 float DeltaZ      = pSo->fDeltaZ;

 float fPartWaermeLeitf5 = (float)2.2;     /* Eis   [W /m /K]  */
 float fWaterContMed     = (float)0.2;     /*! ch 17.8.95 Test  */
                                           /* Wasser immer kont. Medium */
                                           /* LeachM: 0.2 */
                                           /* Daisy berechnet dies mit WKP und fC */
 TEMP_ZERO_LAYER

 /* ******************************************************************  */
 /* Achtung!     DeltaZ besitzt in dieser Funktion die Einheit Meter!!! */
 /* ******************************************************************  */
     
 DeltaZ /= (float)1000;


 /*  Waerme-Kapazitaet berechnen */
 for (TEMP_SOIL_LAYERS)
 {
 
  pHL->fCapacityOld  = pHL->fCapacity;
  
  pHL->fCapacity  = DENSITY_WATER   * SPECCAP_WATER   * pWL->fContAct;  
  pHL->fCapacity += DENSITY_ICE     * SPECCAP_ICE     * pWL->fIce; 
  pHL->fCapacity += DENSITY_MINERAL * SPECCAP_MINERAL * pSL->afPartVolFrac[0];  /* Mineralien */

   /* lpXH->fCapacity *= (float)1000; */

   /* [kg/l * J/kg /K ] = [J/l /K ] = [ kJ /m^3 /K]  */
  
  ///*
  if ((!lstrcmp(pSL->acSoilID,"Str\0"))||
				(!lstrcmp(pSL->acSoilID,"Hum\0")))
   {
	//pHL->fCapacity = DENSITY_HUMUS * SPECCAP_HUMUS;
    pHL->fCapacity  = DENSITY_WATER   * SPECCAP_WATER   * pWL->fContAct;  
	pHL->fCapacity += DENSITY_HUMUS * SPECCAP_HUMUS * ((float)1-pSL->fPorosity);
   }
  //*/

 /*lpXH->fHeat = lpXH->fCapacity * (lpT->BodenTemp + (float)273);    = [ kJ /m^3] */
  pHL->fHeat = pHL->fCapacity * pHL->fSoilTemp;                  /* = [ kJ /m^3] */

  pHL->fHeatLatent = pWL->fIce * LAT_HEAT_MELT        /* [ 1 * kJ/kg * kg/l / 1000] */
                               * DENSITY_ICE        /* = [ kJ / m^3] */
                               * (float) 1000;
                                                 
  
  //if (NewDay(pTi))
  if (NewTenthDay(pTi))
  {

  iContinMed = (pWL->fContAct > fWaterContMed)? 3 : 4; /* Wasser : Luft */
                
  pHL->fConductOld = pHL->fConduct; /* Speichern des alten Wertes */
    
  /* ****************************************************************** */
  /*   Berechnung der Waermeleitfähigkeiten                             */
  /*   nach De Vries mithilfe von Geometrie-Faktoren                    */
  /* ****************************************************************** */
    
  for (i = 0 ; i<=5; i++) /* 5 Fraktionen */
    {
    for (k = 0, fform[i] = (float)0; k<3; k++) /* drei Richtungen */
      {
       f1 = (i<5) ? 
            pSo->pSHeat->afPartHeatCond[i] / pSo->pSHeat->afPartHeatCond[iContinMed]
          : fPartWaermeLeitf5 / pSo->pSHeat->afPartHeatCond[iContinMed]; /* Eis */
    
       f1 -= (float) 1;
       f1 *= pSo->pSHeat->afPartGeom[k][1]; /* Aufteilung:  0.125, 0.125, 0.75  */
       f1 += (float) 1;
    
       fform[i] += (float)1 / f1;
      } /* next k  */
    } /* next i */
    
  f1 = fform[1] * ((float)1- pSL->fPorosity) * pSo->pSHeat->afPartHeatCond[1]; /* Minerale */
  f2 = fform[1] * ((float)1- pSL->fPorosity);
    
  /* fehlt: org. Substanz */
    
  f1 += fform[3] * pWL->fContAct * pSo->pSHeat->afPartHeatCond[3]; /* Wasser */
  f2 += fform[3] * pWL->fContAct;
    
  f1 += fform[4] * ( pSL->fPorosity - pWL->fContAct - pWL->fIce) * pSo->pSHeat->afPartHeatCond[4]; /* Luft */
  f2 += fform[4] * ( pSL->fPorosity - pWL->fContAct - pWL->fIce);
    
  f1 += fform[5] * pWL->fIce * fPartWaermeLeitf5; /* Eis */
  f2 += fform[5] * pWL->fIce;
    
  Set_At_Least_Epsilon(&f2, "f2\n in Get_Cond_Cap_Heat()");
        
  pHL->fConduct =  (float)86.4 * f1 / f2;   /* J/s/K/m/86.4 = [kJ/d/K/m] */     
                                         
  ///*
  if ((!lstrcmp(pSL->acSoilID,"Str\0"))||
				(!lstrcmp(pSL->acSoilID,"Hum\0")))
   {
    pHL->fConduct =(
				    (pSL->fPorosity - pWL->fContAct) * (float)0.025 //Luft 0.025
		           +((float)1-pSL->fPorosity) * //(float)0.025 //Streu wie Luft
				    ((float)0.2 * pWL->fContAct + (float)0.015) //Streu & Wassergehalt 
				   )
		           * (float)3600.0 * (float)24.0/(float)1000; //[J/s/K/m]->[kJ/d/K/m]
   }
  //*/
  
  }  /* Neue Konduktivität nur bei NewTenthDay  */
    
  } /* Schichtweise */
           
return 1;
} /* end Get_Cond */

/*********************************************************************************************/
/* Name             : Heat_Balance(exp_p)                                                    */       
/* Inhalt			: Interne Funktion zur Berechnung Berechnung der Waerme-Bilanzgroessen.  */  
/*                                                                                           */
/* Autor            : C.Sperr                                                                */
/* Datum            : 23.10.95                                                               */
/*                                                                                           */
/*       15.11.95  Eisgehalt im Profil berechnen.                                            */
/*   as, 27.11.96  Header: Variablenänderung erfaßt.                                         */
/*                                                                                           */
/*********************************************************************************************/
/*   veränd. Var.:			pHB->fBalance                                                    */
/*							pHB->fInput                                                      */
/*							pHB->fOutput                                                     */
/*							pHB->fProfil                                                     */
/*							pHB->fProfilStart                                                */
/*********************************************************************************************/
int WINAPI Heat_Balance(EXP_POINTER)
{
  DECLARE_TEMP_POINTER
  
  DECLARE_COMMON_VAR
  
  float DeltaZ      = pSo->fDeltaZ;

  TEMP_ZERO_LAYER
  

  /* ******************************************************************  */
  /* Achtung!     DeltaZ besitzt in dieser Funktion die Einheit Meter!!! */
  /* ******************************************************************  */
  
  DeltaZ /= (float) 1000;


  /* ****************************************************************** */
  /*        Input                                                       */
  /* ****************************************************************** */

  /* Daisy 5-22 */
  /* diffusiver Fluß */
  pHB->fInput  -= (pHL->pNext->fConduct  + pHL->pNext->fConductOld)
                * (pHL->pNext->fSoilTemp + pHL->pNext->fSoilTempOld
                - (float)2* pHL->fSoilTemp) / ((float)4* DeltaZ)* DeltaT;
 

  /* konvektiver Fluß */
  /* lpXK->fUpperInputCum += lpk->aktlPtrTempParam->Durchschn
                           * lpXK->fSpecCap[0]           
                           * lpXK->fDensity[0]
                           * lpw->lPtrH2OFluss->FDichte
                           * DeltaT
                          / (float)1000;
   */
                                 /*   [K * (J/kg /K) * (kg/l) *(mm/d) * d * 1000] */
                                 /* = [  J/(0.001 m^3) * (0.001 m) * 1000] = [kJ/m^2] */
 
  /* ****************************************************************** */
  /*        Output */
  /* ****************************************************************** */

  /* Setze Temperatur-Zeiger auf letzte Schicht: */
  for (TEMP_SOIL_LAYERS);
                   
  /* diffusiver Fluß */
  pHB->fOutput  -= (pHL->pBack->fConduct + pHL->pBack->fConductOld)
                 * ((float)2* pHL->fSoilTemp
                 - pHL->pBack->fSoilTemp  - pHL->pBack->fSoilTempOld) 
                 / ((float)4* DeltaZ) * DeltaT;
 

  /* konvektiver Fluß */
  pHB->fOutput  -= pCl->pWeather->fTempAve
                 * SPECCAP_WATER
                 * DENSITY_WATER
                 * pWL->pBack->fFluxDens  /* Fluss aus letzter Soil Schicht  */
                 * DeltaT
                 / (float)1000;   /* = [kJ/m^2] */
                 
  /* ****************************************************************** */
  /*        Aktueller Waermegehalt im Profil                            */
  /* ****************************************************************** */
                                          
  pHB->fProfil = (float) 0;     

  for (SOIL_LAYERS1(pHL, pHe->pHLayer->pNext))
    {
    pHB->fProfil += (pHL->fHeatLatent + pHL->fHeat) * DeltaZ;    /*  [ kJ/m^2] */
    }

                    
  /* ****************************************************************** */
  /* Startwert für Waermebilanz                                         */
  /* ****************************************************************** */
  if (SimStart(pTi)) 
      pHB->fProfilStart = pHB->fProfil;  /* [kJ/m^2] */

                                          
  /* ****************************************************************** */
  /*        Bilanzierung                                                */
  /* ****************************************************************** */

  pHB->fBalance = pHB->fProfil - pHB->fProfilStart 
                - pHB->fInput  + pHB->fOutput;      /* = [kJ/m^2] */


  return 1;
}        
             
/*********************************************************************************************/
/* Name             : Solve_LGS(exp_p)                                                       */       
/* Inhalt			: Interne Funktion zur Berechnung des linearen Gleichungssystems.        */  
/*                    Ergebnis: Neue Temperaturen, evt. Quelle-/Senke für Wassermodul        */
/*                    durch Tauen und Frieren                                                */
/* Autor            : C.Sperr                                                                */
/* Datum            : 19.10.95                                                               */
/*                                                                                           */
/*   ch, 06.02.96 Frost wird durch Energieansatz berechnet.                                 */
/*   as, 27.11.96		Header: Var.veränderungen einführen.                                */
/*                                                                                          */
/********************************************************************************************/
/*		veränd. Var.:		pHe->fFreezingDepth                                             */
/*							pWL->WaterFreezeR                                               */
/*							pWL->WaterFreezeDay                                             */
/*							pWL->fIce                                                       */
/*							pWL->fIceOld                                                    */
/*							pHL->fSoilTemp                                                  */
/*							pHL->fSoilTempOld                                               */
/*							pSo->fAlbedo                                                    */
/*							pSo->fSoilAlbedo                                                */
/*							pSW->fContSat                                                   */
/********************************************************************************************/
int WINAPI Solve_LGS_T(EXP_POINTER, int Frost_Rad_Flag)
{
  DECLARE_TEMP_POINTER
  DECLARE_COMMON_VAR
  
  float *pA,*pB,*pC,*pD;   /* Hilfsgroessen */
  float fA,fB,fC,fD,fQ;
  float fE,fR,fS;
  double *pF, *pG;   
  float fFrozenDepth;
  int iLayerAnz =  pSo->iLayers;
  float DeltaZ      = pSo->fDeltaZ;
  
  const int LOWER_BOUNDARY = 2; /* 0: Konst. Temp,  */
                                /* 1: Kein Fluß, */
                                /* 2: Berechnete Temp. (Daisy) */
   
  TEMP_ZERO_LAYER

  /* ******************************************************************* */
  /* Achtung!     DeltaZ besitzt in dieser Funktion die Einheit Meter!!! */
  /* ******************************************************************* */
  
  DeltaZ /= (float) 1000;
     

  if ((Frost_Rad_Flag==2)||(Frost_Rad_Flag == 3)) /* berücksichtige Strahlungsterm */
   {
    // Albedo
    if (pWL->pNext->fContAct >= (float)0.25)
         {pSo->fSoilAlbedo = (float) 0.1;}
    else if (pWL->pNext->fContAct <= (float)0.1)
         {pSo->fSoilAlbedo = (float) 0.25;}
    else
         {pSo->fSoilAlbedo = (float) 0.35 - pWL->pNext->fContAct;}
    
    // Gesamt-Albedo
    // if (lpb->gesAlbedo == (float)0.0)
      {
       pSo->fAlbedo = pSo->fSoilAlbedo;
      }

    fE = (float)0.9 + (float)0.18 * pWL->pNext->fContAct;
    fS = (float) 5.67e-8;
    fR = fS * abspower(pCl->pWeather->fTempAve + (float)273.169, (float)4.0)
            * ((float) 0.605 + (float) 0.048 
            * abspower((float)1370.0 * pCl->pWeather->fHumidity / (float)100.0, (float)0.5));
   }

 
  /* Speicher für Schichtvariable reservieren */

  pA        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pA,0x0,(iLayerAnz * sizeof(float)));
  pB        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pB,0x0,(iLayerAnz * sizeof(float)));
  pC        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pC,0x0,(iLayerAnz * sizeof(float)));
  pD        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pD,0x0,(iLayerAnz * sizeof(float)));
        
  pF        = (double *) _MALLOC(iLayerAnz * sizeof(double));
    memset(pF,0x0,(iLayerAnz * sizeof(double)));
  pG        = (double *) _MALLOC(iLayerAnz * sizeof(double));
    memset(pG,0x0,(iLayerAnz * sizeof(double)));
                                   

  for (TEMP_SOIL_LAYERS)
   {
    /* ******************************************************* */
    /* Numerische Groessen in den Einheiten J, m, d, K         */
    /* ******************************************************* */

    /* Daisy (5-47) (ohne Frost) */
    fA = (pHL->fCapacity + pHL->fCapacityOld) / (float)2; /* [kJ /m^3 /K] */
    fB = (float) 0;                                       /* [kJ /m^3 /d] */
    fC = (pHL->fConduct + pHL->fConductOld)/(float)2;     /* [ kJ/d/K/m] */

    /* Daisy (5-50) */
    fD = (pHL->pNext->fConduct - pHL->pBack->fConduct
       +  pHL->pNext->fConductOld - pHL->pBack->fConductOld)
       / ((float)4*DeltaZ);                               /* [ kJ/K/m^2 /d] */

    fQ =  SPECCAP_WATER           
       * (pWL->fFluxDens    + pWL->pBack->fFluxDens
       +  pWL->fFluxDensOld + pWL->pBack->fFluxDensOld) 
       *  DENSITY_WATER
       / (float)4000;        /* [ J/kg /K * mm /d * kg/l * 1000] = [ kJ /K /m^2 /d]   */
                            
    /* 2.2.96 mit Komponente zum vorherigen Zeitschritt. */

    if (iLayer==1) /* Obere Randbedingung fuer Koeffizienten. */
      {
       /* Daisy (5-57 bis 5-60) */
       pA[1] = (float) 0;                                    /* [ kJ /K /m^3 /d] */

       pB[1]  = fA / DeltaT
                + fC / (DeltaZ * DeltaZ);
  //              + fC/ ((float)2 * DeltaZ * DeltaZ);            /* = [kJ /m^3 /K /d]   */

       pC[1] =  fQ / ((float) 4 * DeltaZ);              
         
       pC[1] -= fC/ ((float) 2 * DeltaZ * DeltaZ);           /* = [kJ /m^3 /K /d]   */

       pD[1] = fA * pHL->fSoilTemp / DeltaT;

       pD[1] +=  fC
              * (pHL->pNext->fSoilTemp - (float)2 * pHL->fSoilTemp 
              +  pHe->pHLayer->fSoilTempOld + pHL->pBack->fSoilTemp) 
              / ((float)2 * DeltaZ * DeltaZ);

      /* ch orig:  + fTempAktuell) / DeltaZ; */

      pD[1] -=  fQ    
      /*ch orig unterscheidet zw. TempBoden und TempInfiltwasser) */
      /* ch 18.6.96
         es wird nicht mehr * (lpT->BodenTemp) 
         benutzt, da Infiltration stets um 0:00 Uhr stattfindet und
         Temp. um diese Uhrzeit deutlich geringer als Tagesmittelwert.*/
            
	         * (pCl->pWeather->fTempAve) / ( (float) 4 * DeltaZ);        /* [kJ /m^3 /d] */

      if ((Frost_Rad_Flag==2)||(Frost_Rad_Flag == 3))
         {
//         pD[1] -= ((float)1.0 - lpb->gesAlbedo) 
//                * lpk->aktlPtrTempParam->SolRad/(float)800.0 * lpk->aktlPtrTempParam->Tmax;
/*
           pD[1] -= ((float)1.0 - lpb->gesAlbedo)
                  * abspower((lpk->aktlPtrTempParam->SolRad*(float)0.03),(float)0.5)
                  * lpk->aktlPtrTempParam->Durchschn;
*/
           pD[1] += (((float)1.0 - pSo->fAlbedo)
                  * pCl->pWeather->fSolRad * (float) 11.905 + fR 
                  - fE * fS * abspower((pHL->fSoilTemp + (float)273.16), (float)4.0))
                  * (float) 84.0;
         } /* Radiation */

      } /*iLayer==1 */

    else
      {
         pA[iLayer] = (fD - fQ )/((float)4 * DeltaZ)
                      -  fC/ ( (float) 2 * DeltaZ * DeltaZ);        /* [ J /K /m^3 /d] */

         pB[iLayer] = fA / DeltaT  + fC/ ( DeltaZ * DeltaZ);

         pC[iLayer] = (fQ - fD) /((float)4 * DeltaZ)
                      -  fC/ ( (float) 2 * DeltaZ * DeltaZ);

         pD[iLayer] = fA * pHL->fSoilTemp / DeltaT
                      + fC * (pHL->pNext->fSoilTemp - (float)2 * pHL->fSoilTemp + pHL->pBack->fSoilTemp)
                      / ((float) 2 * DeltaZ * DeltaZ);
             
         pD[iLayer] += (fD - fQ )
                       * (pHL->pNext->fSoilTemp - pHL->pBack->fSoilTemp)
                       / ( (float) 4 * DeltaZ);
              
         pD[iLayer] -= fB;

      } /* iLayer>1 */
   } /* Schichtweise      */


  /* *********************************************** */
  /*      Löser des Gleichungssystems H T = D        */
  /*          mit H = tridiag(A,B,C)                 */
  /* *********************************************** */

  pF[1] =  (double)pC[1] / (double)pB[1];
  pG[1] = ((double)pD[1] - (double)pA[1]* (double)pHe->pHLayer->fSoilTemp) / (double)pB[1];


  /* rekursive Definition von F und G: */
  for (iLayer=2; iLayer< iLayerAnz -1; iLayer++)
    {
     pF[iLayer] =  (double)pC[iLayer]
                  / ((double)pB[iLayer] - (double)pF[iLayer-1] * (double)pA[iLayer]);
     pG[iLayer] = ((double)pD[iLayer] - (double)pA[iLayer]   * (double)pG[iLayer-1])
                  / ((double)pB[iLayer] - (double)pF[iLayer-1] * (double)pA[iLayer]);
    }

  pHL->fSoilTempOld = pHL->fSoilTemp;


  /* **************************************************************** */
  /*      Untere Randbedingung:                                       */
  /* **************************************************************** */
  switch(LOWER_BOUNDARY)
  {         
   case 0:  /*  Temp =konst = Temp(t=0) */
      break;

   case 1:  /* Keine Wärmebewegung */
   
      /* lpT Zeiger befindet sich auf letzter Schicht */   
      pHL->fSoilTemp = pHL->pBack->fSoilTemp - pHL->fSoilTemp
                     + (float)pG[iLayer-1];

      pHL->fSoilTemp /= ((float)1.0 + (float)pF[iLayer-1]);   
      break;

   case 2: 
      /* Dirichletbedingung mit sinusförmiger Temperatur  */
      /*          (Periode: 1Jahr)     */
        
      if (NewDay(pTi))
         pHL->fSoilTemp =  Get_Daisy_LowerBC(exp_p);

  } /* switch */


  /* **************************** rückwärts errechnen: Tk => T(k-1) */
  /* Zeiger lpT, lpXH ist auf letzter Schicht, iLayer = 10 */

  for (iLayer--,     pHL = pHL->pBack;      /* Setze Zeiger auf letzte Bodenschicht: */
      (iLayer>0) && (pHL->pBack != NULL); 
       iLayer--,     pHL = pHL->pBack)
       {
        /* Speichern der alten Werte: */
        pHL->fSoilTempOld = pHL->fSoilTemp;
        pHL->fSoilTemp    = (float)pG[iLayer] - (float)pF[iLayer] * pHL->pNext->fSoilTemp;

       } 


  if ((Frost_Rad_Flag==1)||(Frost_Rad_Flag == 3))
    {
     /* **************************************************************************** */
     /* Errechnen der Schmelz- und Friervorgänge,                                    */
     /* Korrektur der Temperatur:                                                    */
     /* **************************************************************************** */
     for (TEMP_SOIL_LAYERS)
       {
        pWL->fWaterFreezeR = (float) 0; /* weder Frieren noch Tauen. */
   
        /* Frieren: */
        if ((pHL->fSoilTemp  < (float)0) && (pWL->fContAct > THETA_MIN + (float)0.01))
          { 

// #ifdef FREEZING_SETS_MINIMAL_TIMESTEP
            /* Frost bedarf genügend kleinen Zeitschritts! *-/
            if (pTi->pSimTime->iTimeFlag1 == 0)    
               {
                 Set_Minimal_Time_Step(pTi);
               }
            else                   
               if (pTi->pSimTime->iTimeFlag2 > 20) 
                  {
                     pTi->pSimTime->iTimeFlag2 = 20;     /-* entspricht ca. DeltaT = 0.001 *-/
                  } 
#endif*/
     
            /* fFreeze ist die Geschwindigkeit, mit der der vol.  */
            /* Wassergehalt frieren muss, um die Temperatur auf  */
            /* 0 Grad zu erhöhen. */
            pWL->fWaterFreezeR = (float)-0.001 * pHL->fSoilTemp * pHL->fCapacity   /* [1000 K * kJ/m^3/K] */
                               / (DENSITY_WATER * LAT_HEAT_MELT * DeltaT);     /* [kJ/kg * kg/l *d] */
                                         /* = [1000 l/m^3d/] = [1/d] */
            #ifdef LOG_FREEZING       
            if (pWL->fIceOld == (float)0)
               {
                  #ifdef LOGFILE                            
                  
                     wsprintf(lpOut,"*        Temp_dai: Layer %d starts freezing.", iLayer); 
                     Message(0,lpOut);
                  #endif
               }
            #endif              
                     
            if ((pWL->fWaterFreezeR * DeltaT <= pWL->fContAct - (THETA_MIN + EPSILON))&&
                (pWL->fWaterFreezeR * DeltaT <= (float)10 * (pSW->fContSat - pWL->fContAct)))
                {    
                   /* Kein Wassermangel und kein Volumenmangel.  */
                   /* => Frieren uneingeschränkt möglich */
                   pHL->fSoilTemp = (float) 0; 
                } 
            else
                { /* Einschraenkung */
                   float FrostNeeded = pWL->fWaterFreezeR;
		  
                   if (pWL->fWaterFreezeR * DeltaT > (float)10 * (pSW->fContSat - pWL->fContAct))
                      {
                        pWL->fWaterFreezeR = max((float)0,(float)10 * (pSW->fContSat - pWL->fContAct) / DeltaT);         
        
                        /*
                        #ifdef LOGFILE                            
                        wsprintf(lpOut,"!        Temp_dai, SolveLGS(): Freezing (%d mm) exceeds free pore space (%d mm) in Layer %d.", \
                        iLayer, (int)(lpXI->fWaterFreeze * DeltaT), (int)(lpGG->ThetaSat - lpGH->aktuell)); 
                        Message(0,lpOut);
                        #endif
                        */
                      }
           
                   if (pWL->fWaterFreezeR * DeltaT > pWL->fContAct - (THETA_MIN + EPSILON))
		               /* Wassermangel */
                      {
                        #ifdef LOGFILE                            
                         if (pWL->fWaterFreezeR * DeltaT - pWL->fContAct - (THETA_MIN + EPSILON) > (float)5 / DeltaZ)
                           {
                             Message(0, ERROR_FREEZING_EXCEEDS_H2O);
                            }
                        #endif

                         pWL->fWaterFreezeR = max((float)0,(pWL->fContAct - (THETA_MIN + EPSILON)) / DeltaT);
                      }
        
                   pHL->fSoilTemp *= (float)1 - pWL->fWaterFreezeR / FrostNeeded;

                }   /* Frieren nur eingeschraenkt moeglich. */
        
          } /* Frieren */

// #ifdef FREEZING_SETS_MINIMAL_TIMESTEP
       else 
            /* Tauen: */
            if ((pHL->fSoilTemp  > (float)0)&&(pWL->fIce > (float)0))
               {
                 /* Tauen bedarf genügend kleines Zeitschritts! *-/
                 if (pTi->pSimTime->iTimeFlag1 == 0)    
                    {
                      Set_Minimal_Time_Step(pTi);
                    }
                 else                   
                    if (pTi->pSimTime->iTimeFlag2 > 20) 
                       {
                          pTi->pSimTime->iTimeFlag2 = 20;     /-* entspricht ca. DeltaT = 0.001 *-/
                       } */

                 #ifdef LOG_FREEZING       
                    #ifdef LOGFILE                            
                     if (pWL->fIce >  pWL->fIceOld + EPSILON)
                       {
                          wsprintf(lpOut,"*        Temp_dai, SolveLGS(): Layer %d starts thawing.", iLayer); 
                          Message(0,lpOut);
                       }
                    #endif
                 #endif    

                 /* fFreeze ist der vol. Wassergehalt, der schmelzen muss, um Temperatur auf O Grad */
                 /* zu erniedrigen. */
                 pWL->fWaterFreezeR = (float)-0.001 * pHL->fSoilTemp * pHL->fCapacity  /*   [1000 K * kJ/m^3/K] */
                                    / (DENSITY_WATER * LAT_HEAT_MELT * DeltaT);    /*   [kJ/kg * kg/l * d] */
                                                                                       /* = [1000 l/m^3/d] = [1/d] */

                                
                 if (pWL->fWaterFreezeR * DeltaT + pWL->fIce  / (float)1.1 >= (float)0)
                    { 
                      /* Tauen uneingeschränkt möglich => Temp erhöht sich nicht! */
                      pHL->fSoilTemp = (float) 0;
                    }                                                    
                 else
                    {                
                 #ifdef LOG_FREEZING       
                      #ifdef LOGFILE                            
                       if (pWL->fWaterFreezeR >= (float)0)
                         {
                           wsprintf(lpOut,"*        Temp_dai, SolveLGS(): Layer %d thawed.", iLayer); 
                           Message(0,lpOut);
                         }
                      #endif
                 #endif
          
                      pHL->fSoilTemp    *= (float)1 + pWL->fIce / (pWL->fWaterFreezeR  * DeltaT  * (float)1.1);
                      pWL->fWaterFreezeR = (float)-1 * pWL->fIce / (DeltaT * (float)1.1);
                    }

               } /* Tauen  */
   
/* Veränderung des Eisgehaltes im Wasserteil:
   pWL->fIceOld = pWL->fIce;

   //-* fIce ist vol. Eisgehalt *-//
    pWL->fIce += (float)1.1 * pWL->fWaterFreezeR * DeltaT;     
	pWL->fContAct -= (float)1.1 * pWL->fWaterFreezeR * DeltaT;     
   
   if (pWL->fIce < (float)0) 
      {
        #ifdef LOGFILE                            
         if (pWL->fIce + EPSILON < (float)0)
           {
    			 		 strcpy((LPSTR)lpOut,"fIce: ");
						 strcat((LPSTR)lpOut,RANGE_ERROR_TXT);
			      		 Message(0, lpOut);
           }
        #endif
 
        pWL->fIce = (float)0;
      }

   //pSW->fContSat = max(pSL->fPorosity - pWL->fIce, THETA_MIN);
   
  if (pSW->fContSat < THETA_MIN) 
     {
         #ifdef LOGFILE                            
    			 		 strcpy((LPSTR)lpOut,"fContSat: ");
						 strcat((LPSTR)lpOut,RANGE_ERROR_TXT);

           if (pSW->fContSat < THETA_MIN - (float)0.01) 
			      		 Message(0, lpOut);
           #endif
         pSW->fContSat = THETA_MIN;
     }
   
*/
   
   //pWL->fWaterFreezeDay += (float)1.1 * pWL->fWaterFreezeR * DeltaT;
   
   } /* For Layer */
 
} /* Frost-Korrektur */
 /* Ende des Lösers des Gleichungssystems H T = D */

  fFrozenDepth         = 
  pHe->fFreezingDepth  = (float)0;
  	
  for(SOIL_LAYERS1(pHL, pHe->pHLayer->pNext))
     {
       if (pHL->fSoilTemp <= (float)0)
          {
             pHe->fFreezingDepth = iLayer * DeltaZ;
	    
	         if (pHL->fSoilTemp + EPSILON < (float)0)
	             fFrozenDepth = iLayer * DeltaZ;
          }
     }	


 /* Speicher für Schichtvariable freigeben */
  free(pA);
  free(pB);
  free(pC);
  free(pD);
  free(pF);
  free(pG);

  return 1;
} /* end Solve_LGS */



/**********************************************************************************************/
/* Name     :   Temp_D()                                                                      */
/*                                                                                            */
/* Inhalt   :   Berechnung der Bodentemperaturen und des Frostes                              */                             
/*                                                                                            */
/* Modul         : TEMP_DAI.c                                                                *      
 * Inhalt        : Berechnung der Bodentemperaturen                                          *
 *                 und des Frostes                                                           *
 *                                                                                           *
 * Methode       : E.Hansen (DAISY)                                                          *
 *                                                                                           *
 * Author        : CH                                                                        *
 *                 Annahme: Schichtdicke konstant.                                           *
 * Date          : 16.8.95, 10.10.95, 23.9.96                                                *
 *                 16.11.95 ThetaSat wird durch Frost herabgesetzt                           *
 *                 06.02.96 temp_daisy: Frost-Senkenterme nach DAISY                         *
 *                          temp_d:     Frost durch Energieansatz                            *
 *				   18.06.96 Infiltrationswasser mit mittl. Tages-Temp.                       *
 *                 02.10.96 dm, Implementierung der neuen Variablenstruktur                  *
 *********************************************************************************************/
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:			pHL->fSoilTemp                                                    */
/*							pHL->fSoilTempOld                                                 */
/*							pSH->afPartHeatCond[]                                             */
/*							pWL->fIce                                                         */
/*							pWL->fWaterFreezeDay                                              */
/**********************************************************************************************/
int WINAPI Temp_D(EXP_POINTER, int Frost_Rad_Flag)
{
  /* ****************************************************************** */
  /* !!!     DeltaZ besitzt in dieser Funktion die Einheit Meter !!!    */
  /* ****************************************************************** */

  DECLARE_TEMP_POINTER

  DECLARE_COMMON_VAR

  float DayTime     = pTi->pSimTime->fTimeDay;
  float DeltaZ      = pSo->fDeltaZ/(float)1000;

  float fRedTAmpDay = (float)1.0;

  TEMP_ZERO_LAYER

  
  
  /* ****************************************************************** */
  /*      Zurücksetzen der am Tag kumulierten Grössen:                  */
  /* ****************************************************************** */
  if (NewDay(pTi))
    {
     for (SOIL_LAYERS0(pWL,pWa->pWLayer))
            pWL->fWaterFreezeDay = (float) 0;
    }
  
                   
  /* ****************************************************************** */
  /* Obere Randbedingung:                                               */
  /* ****************************************************************** */

  /* Aktuelle Lufttemperatur mit sinusförmigem Tagesgang  */
  /* Falls DeltaT >= 0.2 dann  Tagesdurchschnittstemperatur */

   pHL->fSoilTempOld = pHL->fSoilTemp;
   
   pHL->fSoilTemp =	pCl->pWeather->fTempAve;
  ///*   
   pHL->fSoilTemp = (DeltaT < (float)0.2)?
                    pCl->pWeather->fTempAve
				  + fRedTAmpDay * 
				    ((pCl->pWeather->fTempMax - pCl->pWeather->fTempMin)/(float)2)
                  * (float)sin((float)2.0 * (float)PI 
                  * (DayTime - (float)0.25 ))
      
                  : pCl->pWeather->fTempAve;
				    
  //*/   
  
  /* 
   pHL->fSoilTemp = //(DeltaT < (float)0.2)?
                    pCl->pWeather->fTempAve
                    //-(float)max((double)0,(double)0.25*(double)pCl->pWeather->fTempAve);
	                -(pHL->pNext->fSoilTemp-pHL->pNext->pNext->fSoilTemp)/DeltaZ
	                //-(pHL->fSoilTempOld-pHL->pNext->fSoilTemp)/DeltaZ
					//*(float)0.65 //LAI=16
					//*(float)0.49 //LAI=12
					//*(float)0.408	//LAI=10
					*(float)0.327	//LAI=8
					//*(float)0.245	//LAI=6
					*pHL->pNext->pNext->fConduct/(float)86.4;
                    //*(float)1000;//*(float)86.4;
  /*
				  + ((pCl->pWeather->fTempMax - pCl->pWeather->fTempMin) / (float)2)
                  * (float)sin((float)2.0 * (float)PI 
                  * (DayTime - (float)0.25 ))
      
                  : pCl->pWeather->fTempAve
	                -(pHL->pNext->fSoilTemp-pHL->pNext->pNext->fSoilTemp)/DeltaZ
					*(float)0.327	//LAI=8
					*pHL->pNext->pNext->fConduct/(float)86.4;
  */
 
   //pHL->fSoilTemp = Get_Horton_UpperBC(exp_p);

   //pHL->fSoilTemp = pCl->pWeather->fTempAve 
   //                -fRedTAmpYear*(pCl->pWeather->fTempAve-pCl->pAverage->fYearTemp);
   
   if ((pWa->pWBalance->fSnowStorage>(float)1)
	   &&(pWa->pWBalance->fSnowWaterContent >= (float)0))
     pHL->fSoilTemp = (float) 0;
   
  /* ****************************************************************** */
      Get_Cond_Cap_Heat(exp_p);
  /* ****************************************************************** */


  /* ****************************************************************** */
      Solve_LGS_T(exp_p, Frost_Rad_Flag);
  /* ****************************************************************** */

  /* ****************************************************************** */
	  Freezing(exp_p);      
 /* ****************************************************************** */


  return 1;
} 
/* end of procedure Temp_D() */

/*******************************************************************************************
 *
 *  Name     :  Freezing()
 *
 *	Function :  changes ThetaSat
 *				calculates water sink and source term due to freezing or thawing
 *              assuming no plant water uptake 
 *
 *              ch 8.1.97
 *
 ******************************************************************************************
 */
int Freezing(EXP_POINTER)
{ 
  // DECLARE_H2O_POINTER
  PSLAYER     pSL;
  PSWATER     pSW;
  PWLAYER     pWL;
  DECLARE_COMMON_VAR  

  
  for (H2O_SOIL_LAYERS)   
   {
   pWL->fIceOld = pWL->fIce;

   /* fIce ist vol. Eisgehalt */
    //pWL->fIce += (float)1.1 * pWL->fWaterFreezeR * pTi->pTimeStep->fOld;     
    pWL->fIce += pWL->fWaterFreezeR * pTi->pTimeStep->fOld;     
   
   if (!Test_Range(pWL->fIce, (float)0, (float)1, "Ice content"))
      {
        pWL->fIce = (float)0;
      }

	if (!Test_Range(pWL->fIce,  (float)0, (pSL->fPorosity - THETA_MIN),"Ice content"))
      {
        pWL->fIce = pSL->fPorosity - THETA_MIN;
      }
	
   //pSW->fContSat = pSL->fPorosity - pWL->fIce;
   

	if ((pWL->fIce > pWL->fIceOld + EPSILON)||(pWL->fIce < pWL->fIceOld - EPSILON))
	{
		pSW->fContInflec = (float)2* pSW->fCampB * pSW->fContSat / ((float)1 + (float)2* pSW->fCampB);
		
		/* overwrites all other water sinkterms */
		// pWL->fContOld    = WATER_CONTENT(pWL->fMatPotAct);
		   
		/* To calculate correct waterbalance the actual soil water content is recalculated  
		considering the change in ice content */
		//pWL->fContAct   -= (pWL->fIce - pWL->fIceOld) / (float)1.1;
		pWL->fContAct   -= (pWL->fIce - pWL->fIceOld);

   if (!Test_Range(pWL->fContAct, THETA_MIN, (float)1, "Water content"))
      {
        pWL->fContAct = THETA_MIN;
      }

   if (!Test_Range(pWL->fContAct, THETA_MIN, pSW->fContSat, "Water content"))
      {
        pWL->fContAct = pSW->fContSat;
      }
	}	 
   } /* for */
     
return 1;
}

/*********************************************************************************/
/*                                                                               */
/*  Modul     WaermeFlussWilliams                                                */
/*            Methode Williams                                                   */
/*  Name    : WaermeFlussWilliams                                                */
/*  Autoren : Th.Schaaf                                                          */
/*                                                                               */
/*  Datum   : 12.08.96                                                           */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCl->fCumTempL5Days                                               */
/*             pCl->afTempLast5Days[5]                                           */
/*             pHe->pHLayer->fSoilTemp                                           */
/*                                                                               */
/*********************************************************************************/
int WINAPI WaermeFlussWilliams(EXP_POINTER)
{                       
	float w1,w2,w3,w3mit,w4,w5,w6,w7,w8,w9,pwptotal,akttotal,feuchteffekt,eindringtiefe,
			heissertag,teiltagamJahr,tempheute,befeuchttiefe,porositaet,jahrtemp,difftemp,
			gesamttiefe,tempNeu;
	int		idummy1,i1;
 	PHLAYER     akt0;
 	PWLAYER	 	akt1;
 	PSLAYER	 	akt2;    
 	PSWATER		akt4;

if (NewDay(pTi))
{
	w3			= (float)0.0;
	pwptotal	= (float)0.0;                               
	akttotal	= (float)0.0;                               
	gesamttiefe = (float)0.0;                               

     /*Wann ist heissester Tag im Jahr */
    heissertag = (float)200.0;
    teiltagamJahr = (pTi->pSimTime->iJulianDay - heissertag) * (float)0.0174;
    
 	pCl->fCumTempL5Days = pCl->fCumTempL5Days - pCl->afTempLast5Days[4];
	

     /* Aufruecken der 5-Tagestemperaturen */
    for(idummy1=0;idummy1<4; idummy1++)
    {
     pCl->afTempLast5Days[4-idummy1] = pCl->afTempLast5Days[3-idummy1];
    }

     /* Berechnung der heute wirksamen Temperatur */

	w1 = (pCl->pWeather->fTempMax - pCl->pWeather->fTempAve);
	w2 = abspower((pCl->pWeather->fSolRad*(float)0.03),(float)0.5);

	tempheute = ((float)1.0 - pSo->fSoilAlbedo) 
						* (pCl->pWeather->fTempAve + (w1 * w2)) 
						+ (pSo->fSoilAlbedo * pCl->afTempLast5Days[0]);
	pCl->afTempLast5Days[0] = tempheute;

	pCl->fCumTempL5Days = pCl->fCumTempL5Days + pCl->afTempLast5Days[0];

     /* Schichtweise Berechnung */
     
     for (akt0 = pHe->pHLayer->pNext,
          akt1 = pWa->pWLayer->pNext,
          akt2 = pSo->pSLayer->pNext,
          akt4 = pSo->pSWater->pNext;
                	          ((akt0->pNext != NULL)&&
                               (akt1->pNext != NULL)&&
                               (akt2->pNext != NULL)&&
                               (akt4->pNext != NULL));
          akt0 = akt0->pNext,
          akt1 = akt1->pNext,
          akt2 = akt2->pNext,
          akt4 = akt4->pNext)
	    {
		w3 += akt2->fBulkDens;                      
		pwptotal += akt4->fContPWP * (akt2->fThickness/10);
		akttotal += akt1->fContAct * (akt2->fThickness/10);
		}  /* Ende Berechnung Profilgroessen */										

     /* Berechnung mittlere Werte */ 
		w3mit = w3 / pSo->iLayers;
		porositaet = (float)0.356 - ((float)0.114 * w3mit);

     /* Berechnung des Befeuchtungsfaktor */ 
        w4 = w3mit / (w3mit + ((float)686.0 * (float)exp((float)-5.63*(double)w3mit)));     
        befeuchttiefe = (float)1000.0 + ((float)2500.0 * w4);
		w5 = (float)log((float)500.0 / befeuchttiefe);
		
     /* Berechnung des pflanzenverfuegbares Wasser im Profil */ 
		w6 = akttotal - pwptotal;
		if (w6 < (float)0.0)
     	 { w6 = (float)0.01;
     	 }

     /* Berechnung des Feuchtefakors zur Berechnung der Bodentemperatur */
		feuchteffekt = (w6 / (porositaet * ((pSo->fDepth / (float)10.0) * 10)));
		w7  =(float)exp(w5 * (abspower((((float)1.0 - feuchteffekt)/((float)1.0 + feuchteffekt)), (float)2.0)));
		eindringtiefe = (w7 * befeuchttiefe);
                        
     /* Jahres- und Monateinfluss */
		jahrtemp = pCl->pAverage->fYearTemp 
					+ (pCl->pAverage->fMonthTempAmp	* (float)cos(teiltagamJahr) / (float)2.0);
		difftemp = (pCl->fCumTempL5Days / (float)5.0 - jahrtemp);


     /* Berechnung der aktuellen Bodentemperatur des Simulationstages */
     for (akt0 = pHe->pHLayer->pNext,
          akt2 = pSo->pSLayer->pNext,i1=1;
                              ((akt0->pNext != NULL)&&
                               (akt2->pNext != NULL));
          akt0 = akt0->pNext,
          akt2 = akt2->pNext,i1++)
	    {                                            
		w8 = gesamttiefe * 10 + ((akt2->fThickness / (float)10.0) * (pSo->iLayers - (int)2));
        gesamttiefe += (akt2->fThickness / (float)10.0);
		w9 = ((-w8) / eindringtiefe);
	
		tempNeu = pCl->pAverage->fYearTemp + (((pCl->pAverage->fMonthTempAmp * (float)0.5)
		            * (float)cos(teiltagamJahr + w9) + difftemp) * ((float)exp(w9)));
		akt0->fSoilTemp = tempNeu;		
		}
}  /* Ende NewDay() */

 return 1;
}      /*=====   Ende Waermefluss im Bodenprofil nach Williams  =================*/

/*********************************************************************************/
/*                                                                               */
/*  Modul     Waerme konstant                                                    */
/*  Autoren : Stefan Achatz                                                      */
/*                                                                               */
/*  Datum   : 4.11.98                                                            */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pHe->pHLayer->fSoilTemp                                           */
/*                                                                               */
/*********************************************************************************/



int WINAPI  ConstTemp(EXP_POINTER)
{

  DECLARE_TEMP_POINTER
  DECLARE_COMMON_VAR

  for(TEMP_ALL_LAYERS)
  {
	  pHL->fSoilTemp = (float)15.0;  //Konstant 15 Grad Celsius
  }

  return 1;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/**********************************************************************************************/
/* Name     :   Temp_SHAW()                                                                      */
/*                                                                                            */
/* Inhalt   :   Berechnung der Bodentemperaturen und des Frostes                              */                             
/*                                                                                            */
/* Modul         : BoFrost.c                                                                 *      
 * Inhalt        : Berechnung der Bodentemperaturen                                          *
 *                 und des Frostes                                                           *
 *                                                                                           *
 * Methode       : SHAW-Modell, ohne Energieterm für Phasenübergang flüssig-Gas und ohne     *
                   Dampftransport                                                            *
 *                                                                                           *
 * Author        : C. Sambale                                                                *
 *                 Annahme: Schichtdicke konstant.                                           *
 * Date          : 28.03.2000                                                                *
 *********************************************************************************************/
/*                                                                                            */
/**********************************************************************************************/
/*  veränd. Var.:			pHL->fSoilTemp                                                    */
/*							pHL->fSoilTempOld                                                 */
/*							pSH->afPartHeatCond[]                                             */
/*							pWL->fIce                                                         */
/*							pWL->fWaterFreezeDay                                              */
/*							pWL->fContAct                                              */
/**********************************************************************************************/
int WINAPI Temp_SHAW(EXP_POINTER, int Frost_Rad_Flag)
{
  /* ****************************************************************** */
  /* !!!     DeltaZ besitzt in dieser Funktion die Einheit Meter !!!    */
  /* ****************************************************************** */

  DECLARE_TEMP_POINTER

  DECLARE_COMMON_VAR
  
  int i,m;
  int iLayerAnz     = pSo->iLayers;
  float DayTime     = pTi->pSimTime->fTimeDay;
  float fdiff,fCondSnow;
  float fError      = (float)0.01; // maximaler Fehler beim Eisgehalt (Summe der Beträge der Schichten) 
  int iIce;			// 1 wenn irgendwo Temp unter 0, oder Eis, sonst 0!
  float  DeltaZ     = pSo->fDeltaZ;
  float  fSoluteConc = (float)0.001;       // Stoffkonzentration mol/kg
  float  fR			 = (float)8.314;   //Gaskonstante J/(mol K)
  float  fg			 = (float)9.81;    // Gravity N/kg
  float RAD,vLatitude,vDay,SINLD,COSLD,fDaylengthAst,fSunRise,fSunSet,fTempMaxDelay;
  float	vDec,NightTime,fTempMinNext,fhelp,fOsmoticPot,freezTemp;
  static float fTempSunSet;
  float fRedTAmp = (float)1.0;
/*
  static float FAR pSlope[MAXSCHICHT], pSlopeOld[MAXSCHICHT], pSlopeOld[MAXSCHICHT];
  static float FAR pIce[MAXSCHICHT], pTemp[MAXSCHICHT];
  static float FAR pContSat[MAXSCHICHT], pkSat[MAXSCHICHT];
*/
  PWBALANCE   pWB;
  pWB = pWa->pWBalance;

  TEMP_ZERO_LAYER
  m		= 40;  // Anzahl der Iterationsschleifen für die Eismengenbestimmung

  /*
  pSlope        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pSlope,0x0,(iLayerAnz * sizeof(float)));
  pSlopeOld        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pSlopeOld,0x0,(iLayerAnz * sizeof(float)));
  pSlopeOldOld        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pSlopeOldOld,0x0,(iLayerAnz * sizeof(float)));
  pIce        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pIce,0x0,(iLayerAnz * sizeof(float)));
  pTemp        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pTemp,0x0,(iLayerAnz * sizeof(float)));
  
  if(SimStart(pTi))
  {
	  pContSat  = (float *) _MALLOC(iLayerAnz * sizeof(float));
	  pkSat  = (float *) _MALLOC(iLayerAnz * sizeof(float));
  }
  */
  
  /* ****************************************************************** */
  /*      Zurücksetzen der am Tag kumulierten Grössen:                  */
  /* ****************************************************************** */
  /* now in balance.c //ep310113

  if (NewDay(pTi))
    {
     for (SOIL_LAYERS0(pWL,pWa->pWLayer))
            pWL->fWaterFreezeDay = (float) 0;
    }
  */ 
                   
  /* ****************************************************************** */
  /* Obere Randbedingung:                                               */
  /* ****************************************************************** */

  /* Aktuelle Lufttemperatur mit sinusförmigem Tagesgang  */
  /* Falls DeltaT >= 0.2 dann  Tagesdurchschnittstemperatur */

   pHL->fSoilTempOld = pHL->fSoilTemp;
     
 /* pHL->fSoilTemp = (DeltaT < (float)0.2)?		//einfacher Tagesgang
                    pCl->pWeather->fTempAve
                  + ((pCl->pWeather->fTempMax - pCl->pWeather->fTempMin) / (float)2)
                  * (float)sin((float)2.0 * (float)PI 
                  * (DayTime - (float)0.25 ))
      
                  : pCl->pWeather->fTempAve;    
*/
   //Tagesgang bei dem nach Sonnenuntergang ein exp Abfall auf die
   //Min-Temperatur des folgenden Tages erfolgt


    RAD 	  = (float)PI/(float)180.0;
	vLatitude = pLo->pFarm->fLatitude;
	vDay 	  = (float)pTi->pSimTime->iJulianDay;

	//Declination of the sun as function of iJulianDaynumber (iJulianDay)
	vDec = -(float)asin( (float)sin(23.45*RAD)*(float)cos(2.0*(float)PI*(vDay+10.0)/365.0));

	//Intermediate variables
	SINLD = (float)sin(RAD*vLatitude)*(float)sin(vDec);
	COSLD = (float)cos(RAD*vLatitude)*(float)cos(vDec);
	
	//Astronomical daylength (hr) 
	fDaylengthAst  =(float)(12.0*(1.0+2.0*(float)asin(SINLD/COSLD)/PI))/(float)24;         

	fSunRise = ((float)1 - fDaylengthAst)/(float)2; //Tagesbeginn , -ende
	fSunSet  = (float)1 - fSunRise;

	fTempMaxDelay = (float)1/(float)24;   // Verzögerung des Tempmaximums gegenüber der Mitte des Tages

	if(SimStart(pTi)) fTempSunSet =  pCl->pWeather->fTempMin;  //für die erste Nacht ist die Sonnenuntergangstemperatur nicht bekannt!


	if(DayTime>=fSunRise && DayTime<=fSunSet) // während des Tages
	{
	  pHL->fSoilTemp = pCl->pWeather->fTempMin + fRedTAmp *(pCl->pWeather->fTempMax - pCl->pWeather->fTempMin) * 
		  (float)sin((float)PI*(DayTime-fSunRise)/(fDaylengthAst+(float)2*fTempMaxDelay));
	  
	  fTempSunSet = pHL->fSoilTemp;  //letzte berechnete Tagestemperatur=Sonnenuntergangstemperatur
	}
	else  // in der Nacht
	{
      if((pTi->pSimTime->fTimeAct +(float)1 < pTi->pSimTime->iSimDuration)
		 &&(pCl->pWeather->pNext != NULL))
	  {
		  fTempMinNext = pCl->pWeather->pNext->fTempMin;
	  }
	  else
	  {
	  	  fTempMinNext = pCl->pWeather->fTempMin; // für den letzten Simtag
	  }
      if(DayTime>fSunSet)
	  {
		  NightTime  =  DayTime - fSunSet;
		  pHL->fSoilTemp = (fTempSunSet+(float)273.16) * (float)exp(- NightTime * 
				log ((fTempSunSet+(float)273) / (fTempMinNext+(float)273)) / (1-fDaylengthAst)) - (float)273.16;
	  }
	  if(DayTime<fSunRise) 
	  {
		  NightTime =  (float)1 - fSunSet + DayTime;
		  pHL->fSoilTemp = (fTempSunSet+(float)273.16) * (float)exp(- NightTime * 
				log ((fTempSunSet+(float)273) / (fTempMinNext+(float)273)) / (1-fDaylengthAst)) - (float)273.16;

	  }
	}   


   // Einfluß des Schnees auf die Oberflächentemperatur! (erster primitiver Versuch)
 if(fSnowStorage>EPSILON) 
 {
	if(fSnowOutflow>EPSILON) //So lange Schnee schmilzt wird die 
							 //Bodenoberflächentemperatur 0 °C betragen
	{
		pHL->fSoilTemp = (float)0;
	}
	else
	{
	// aus SHAW Modell
	fCondSnow = (float)0.021 + (float)2.51 * (fSnowDensity/(float)100/DENSITY_WATER)*(fSnowDensity/(float)100/DENSITY_WATER);
	
	//Bodenoberflächentemperatur ist das gewichtete Mittel aus der Lufttemperatur und der
	//Temperatur der obersten Bodenschicht (Gewichtung mit Leitfähigkeit und Dicke)
    
	// Analog zum SOIL-Modell
    pHL->fSoilTemp = (fCondSnow / fSnowHeight * pHL->fSoilTemp
					 + (float)2 * pHL->pNext->fConduct / DeltaZ * pHL->pNext->fSoilTemp)
					 / (fCondSnow / fSnowHeight  + (float)2 * pHL->pNext->fConduct / DeltaZ );
	}
	
 }

  
// Anfangswerte merken, für Überprüfung nach der Iteration

  for(TEMP_SOIL_LAYERS)
  {
	if(SimStart(pTi))
	{
		pkSat[iLayer] = pSo->pSWater->fCondSat;
		pContSat[iLayer] = pSo->pSWater->fContSat;
		pSo->pSWater=pSo->pSWater->pNext;
		
		/** Eisgehalt bei SimStart **/
		if(pHL->fSoilTemp<(float)0)
		{
		  fOsmoticPot = -(float)1000 * fSoluteConc * fR * ((pHL->fSoilTemp+pTemp[iLayer])/(float)2 + (float)273.16 )/ fg ;

		  freezTemp = -(float)273.16*fg*fOsmoticPot/(float)1000/(fg*fOsmoticPot/(float)1000-LAT_HEAT_MELT*(float)1000);

	      if(pHL->fSoilTemp<freezTemp)

			pWL->fMatPotAct = LAT_HEAT_MELT * (float)1000000 * (pHL->fSoilTemp) / //Potential zur Temperatur SoilTemp-Epsilon
			(fg * (pHL->fSoilTemp  + (float)273.16));

			pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct + fOsmoticPot);
			pWL->fIce     = pWL->fContOld - pWL->fContAct;
 		}

	}

	 pWL->fIceOld   =  pWL->fIce;
	 pIce[iLayer]   =  pWL->fIce;
	 pTemp[iLayer]  =  pHL->fSoilTemp;
	 pHL->fSoilTempOld  =  pHL->fSoilTemp;
  }
  for(;pSo->pSWater->pBack!=NULL;pSo->pSWater=pSo->pSWater->pBack);

  //Fehler überprüfen
  iIce = 1;
  fdiff=fError + (float)1;

  for( i = 0;( fdiff>fError ) && ( i <= m ) && iIce>0 ; i++)  //Iterationsschleife, für Eisgehalt
  {
  
  if(Frost_Rad_Flag==1)
  { 
      /* ****************************************************************** */
      SlopeIce(exp_p,i); // Neigung der EisgehaltsTemperaturbeziehung, geht als 
	   //abschnittsweise lineare Funktion in die Wärmeleitungs -DGL ein.
      /* ****************************************************************** */
  }

  /* ****************************************************************** */
      Get_Cond_Cap_Heat(exp_p);
  /* ****************************************************************** */


  /* ****************************************************************** */
      Solve_LGS_CS(exp_p);
  /* ****************************************************************** */
//Bodenwärme rechnen  

 /*Berechnung des Eisgehaltes sofern notwendig*/
  iIce=0;

  if(Frost_Rad_Flag==1)
  { 
    for(TEMP_SOIL_LAYERS)
     {
	   if(pHL->fSoilTemp<(float)0)   
	   {
	 	  iIce = 1;
	   }
   	   if(pWL->fIce>(float)0)   
	   {
	 	  iIce = 2;
	   }

     }
  }
 //*****************************************************************************************
   if(iIce>(float)0 )
	{
/*	 #ifdef FREEZING_SETS_MINIMAL_TIMESTEP
*/	      if(pTi->pTimeStep->fAct>=(float)0.001)
		  {

				  // Frost bedarf genügend kleinen Zeitschritts!
            if (pTi->pSimTime->iTimeFlag1 == 0)    
               {
                 Set_Minimal_Time_Step(pTi);
               }
            else                   
               if (pTi->pSimTime->iTimeFlag2 > 10) 
                  {
                   pTi->pSimTime->iTimeFlag2 = 10;     // entspricht ca. DeltaT = 0.001
                  } 
		  }
  /*   #endif
*/	 

	  IceContent(exp_p);

	  fdiff=(float)0;

    for(TEMP_SOIL_LAYERS)
     {
		fdiff = fdiff + (float)fabs(pWL->fIce - pIce[iLayer]);  // überprüfen ob die Änderung mit weiteren Iterationen noch relevant sind
     }
   }
//****************************************************************************************

   if(iIce>0 && fdiff>fError && i<m) for(TEMP_SOIL_LAYERS)  //Anfangswerte wiedereinstellen
    {
		 pIce[iLayer]	=  pWL->fIce; // Icecontent merken, zur Überprüfung weiterer Änderungen
		 pTemp[iLayer]  =  pHL->fSoilTemp; // für Berechnung des Slopes merken 
		 pHL->fSoilTemp =  pHL->fSoilTempOld;
    }

  }  //Ende der Eisiterationsschleife


 pHe->fFreezingDepth = (float)0;
 
 for(TEMP_SOIL_LAYERS)
 {
	  pSo->pSWater=pSo->pSWater->pNext;
	  pWL->fContOld  = pWL->fContAct;
	  pWL->fContAct  = pWL->fContAct - pWL->fIce + pWL->fIceOld;//pCont[iLayer]

	  // gesättigte Leitfähigkeit wird bei Frost reduziert
	  
	  if(pWL->fIce>(float)0)   // maximale Leitfähigkeit bei Bodenfrost reduziert!!
	  {
			pSo->pSWater->fCondSat = pkSat[iLayer];
			fhelp            = MATRIX_POTENTIAL(pWL->fContMax-pWL->fIce); 
			pSo->pSWater->fCondSat = CONDUCTIVITY(fhelp);
  	   }

	  // gesättigte Leitfähigkeit wird bei Frost reduziert


//    Eventuell müssen die maximalen Wassergehalte noch korrigiert werden, um eine zu starke Infiltration zu verhindern! 																
/*	  pSW->fContSat  = pSW->fContSat - pWL->fIce + pWL->fIceOld;
	  pWL->fContMax  = pWL->fContMax - pWL->fIce + pWL->fIceOld; 
	  pSL->fPorosity = pSL->fPorosity - pWL->fIce + pWL->fIceOld;
*/

	  // Freezing Rate
      pWL->fWaterFreezeR =  (pWL->fIce - pWL->fIceOld)/DeltaT;
      
	  // now in balance.c //ep310113
      //pWL->fWaterFreezeDay += pWL->fWaterFreezeR * DeltaT;

      if (pWL->fIce > (float)0.)
       {

		  pHe->fFreezingDepth = iLayer * DeltaZ;

	   }
		
 }
  for(;pSo->pSWater->pBack!=NULL;pSo->pSWater=pSo->pSWater->pBack);


  return 1;
} 
/* end of procedure Temp_SHAW() */


/******************************************************************************************/
/* Name             : SlopeIce(exp_p,i)                                                    */       
/* Inhalt			: interne Funktion zur Berechnung der Steigung der Wassergehalt-       */  
/*                    Temperatur Beziehung im Boden bei gegebener Bodentemperatur,                   */
/*                    Flerchinger, G.N. & Saxton, K.E. 1989:Simultaneous Heat and Water    */
/*                    Model of a Freezing Snow-Residue-Soil System I. Theory and           */
/*                    Development. TRANSACTION of the ASAE, Vol. 32, No. 2, pp. 565-571.    */
/* Autor            : C. Sambale                                                           */
/* Datum            : 28.03.2000                                                           */
/*                                                                                         */
/*                                                                                         */
/******************************************************************************************/

int WINAPI SlopeIce(EXP_POINTER,int iter)
{
  DECLARE_TEMP_POINTER
  DECLARE_COMMON_VAR

  int    iLayerAnz   =  pSo->iLayers;
  float  eps		 = (float)0.0001;
  float  fOsmoticPot,freezTemp;
  float  DeltaZ      = pSo->fDeltaZ;
  float  x1,x2,pS1,pS2;
  float  maxSlope    = -(float)50;


  for (TEMP_SOIL_LAYERS)
  {
     pSlopeOldOld[iLayer] = pSlopeOld[iLayer];
     pSlopeOld[iLayer]	= pSlope[iLayer];

	  
	  fOsmoticPot = -(float)1000 * fSoluteConc * fR * ((pHL->fSoilTemp+pTemp[iLayer])/(float)2 + (float)273.16 )/ fg ;

	  freezTemp = -(float)273.16*fg*fOsmoticPot/(float)1000/(fg*fOsmoticPot/(float)1000-LAT_HEAT_MELT*(float)1000);

	  if((pHL->fSoilTemp+pTemp[iLayer])/(float)2 < freezTemp )
	  {
        x1 = LAT_HEAT_MELT * (float)1000000 * ((pHL->fSoilTemp+pTemp[iLayer])/(float)2 - eps) / //Potential zur Temperatur SoilTemp-Epsilon
			(fg * (pHL->fSoilTemp - eps + (float)273.16));
		x2 = LAT_HEAT_MELT * (float)1000000 * ((pHL->fSoilTemp+pTemp[iLayer])/(float)2 + eps)/ 
			(fg * (pHL->fSoilTemp + eps + (float)273.16));
		
		pS1 = (-WATER_CONTENT(x2-fOsmoticPot) + WATER_CONTENT(x1-fOsmoticPot))/((float)2*eps);

		pS1 = max(pS1,maxSlope);

	  }
	  else
	  {
		pS1 = (float)0;
        if( pWL->fIce>(float)0)  pS1 = maxSlope; 
	  }


	  if(pTemp[iLayer] < freezTemp)
	  {
        x1 = LAT_HEAT_MELT * (float)1000000 * ((pHL->fSoilTemp+pTemp[iLayer])/(float)2 - eps) / 
			(fg * (pTemp[iLayer] - eps + (float)273.16));
		x2 = LAT_HEAT_MELT * (float)1000000 * ((pHL->fSoilTemp+pTemp[iLayer])/(float)2 + eps) / 
			(fg * (pTemp[iLayer] + eps + (float)273.16));
		
		pS2 = (-WATER_CONTENT(x2-fOsmoticPot) + WATER_CONTENT(x1-fOsmoticPot))/((float)2*eps);

		pS2 = max(pS2,maxSlope);
	 }
	  else
	  {
		pS2 = (float)0;
	    if( pWL->fIce>(float)0)  pS2 = maxSlope; 
	  }

//falls Frostgrenze überschritten wird, wird ein gewichtetes Mittel genutzt, um den Slope vernünftig zu bestimmen
   if((freezTemp<pTemp[iLayer] && freezTemp>pHL->fSoilTemp && pTemp[iLayer]!=pHL->fSoilTemp)   ||
	   (freezTemp>pTemp[iLayer] && freezTemp<pHL->fSoilTemp && pTemp[iLayer]!=pHL->fSoilTemp) )
   {
	   pSlope[iLayer] = pS2 * (float)fabs(pHL->fSoilTemp - freezTemp)/(float)fabs(pTemp[iLayer]-pHL->fSoilTemp) +
						pS1 * (float)fabs(pTemp[iLayer] - freezTemp)/(float)fabs(pTemp[iLayer]-pHL->fSoilTemp);
   }
   else
   {
		pSlope[iLayer] = (pS1 + pS2)/(float)2;

   }
   pSlope[iLayer] = min((float)0,pSlope[iLayer]);
   
   if((fabs(pSlope[iLayer] - pSlopeOld[iLayer])+fabs(pSlope[iLayer] - pSlopeOldOld[iLayer]) > EPSILON + fabs(pSlopeOld[iLayer] -pSlopeOldOld[iLayer]))
	  && iter>=2)
   {
	   pSlope[iLayer] = (pSlopeOld[iLayer] + pSlopeOldOld[iLayer])/(float)2;
   }

 
//	if(pSlope[iLayer]<0) pSlope[iLayer] = -(float)1;
  }

return 1;
} 


/******************************************************************************************/
/* Name             : IceContent(exp_p)                                                    */       
/* Inhalt			: interne Funktion zur Berechnung des Eisgehaltes im Boden bei         */  
/*                    gegebener Bodentemperatur, entsprechend Formel 12 in                 */
/*                    Flerchinger, G.N. & Saxton, K.E. 1989:Simultaneous Heat and Water    */
/*                    Model of a Freezing Snow-Residue-Soil System I. Theory and           */
/*                    Development. TRANSACTION of the ASAE, Vol. 32, No. 2, pp. 565-571.    */
/* Autor            : C. Sambale                                                           */
/* Datum            : 16.02.2000                                                           */
/*                                                                                         */
/*                                                                                         */
/********************************************************************************************/
/*		veränd. Var.:							                                           */
/*							pWL->fIce                                                      */
/******************************************************************************************/

int WINAPI IceContent(EXP_POINTER)
{
  DECLARE_TEMP_POINTER
  DECLARE_COMMON_VAR

  

  int    iLayerAnz   =  pSo->iLayers;
  float  fOsmoticPot,freezTemp;
  float  DeltaZ      = pSo->fDeltaZ;
  float  ice1, gesPot;



  PWBALANCE   pWB;
  pWB = pWa->pWBalance;
  
  TEMP_ZERO_LAYER

 
  for (TEMP_SOIL_LAYERS)
   {
      fOsmoticPot = -(float)1000 * fSoluteConc * fR * (pHL->fSoilTemp + (float)273.16 )/ fg ;

	 // gesPot = fOsmoticPot + MATRIX_POTENTIAL(pWL->fContAct);
	  gesPot = fOsmoticPot + MATRIX_POTENTIAL(pWL->fContAct-pWL->fIce);
	  
	  freezTemp = -(float)273.16*fg*gesPot/(float)1000/(fg*gesPot/(float)1000-LAT_HEAT_MELT*(float)1000);

	  ice1 = pSlope[iLayer] * (pHL->fSoilTemp - min(pHL->fSoilTempOld,freezTemp));
  
	  if(pHL->fSoilTemp<freezTemp && pSlope[iLayer]>=-EPSILON)pHL->fSoilTemp=freezTemp;

//	  ice1 = min(ice1,(float)0.01*pTi->pTimeStep->fAct);
	  pWL->fIce = ice1 + pWL->fIceOld;
      pWL->fIce = max((float)0,pWL->fIce);
  }

return 1;
}// Ende IceContent



/*********************************************************************************************/
/* Name             : Solve_LGS_CS(exp_p)                                                     */       
/* Überarbeitet bezüglich Frost C. Sambale                                                   */  
/* Inhalt			: Interne Funktion zur Berechnung des linearen Gleichungssystems.        */  
/*                    Ergebnis: Neue Temperaturen, evt. Quelle-/Senke für Wassermodul        */
/*                    durch Tauen und Frieren                                                */
/* Autor            : C.Sambale                                                              */
/* Datum            : 28.02.2000                                                            */
/* In Anlehnung an TempDaisy mit einem zusätzlichen Bodenfrostmodul analog zum SHAW-Modell  */
/*                                                                                          */
/********************************************************************************************/
/*		veränd. Var.:		pHe->fFreezingDepth                                             */
/*							pWL->WaterFreezeR                                               */
/*							pWL->WaterFreezeDay                                             */
/*							pWL->fIce                                                       */
/*							pWL->fIceOld                                                    */
/*							pHL->fSoilTemp                                                  */
/*							pHL->fSoilTempOld                                               */
/*							pSo->fAlbedo                                                    */
/*							pSo->fSoilAlbedo                                                */
/*							pSW->fContSat                                                   */
/********************************************************************************************/
int WINAPI Solve_LGS_CS(EXP_POINTER)
{
  DECLARE_TEMP_POINTER
  DECLARE_COMMON_VAR
  
  float *pA,*pB,*pC,*pD;   /* Hilfsgroessen */
  float fA,fB,fC,fD,fQ;
  float fE,fS,fR;
  double *pF, *pG;   
  float fTempSurface;
  int iLayerAnz =  pSo->iLayers;
  float DeltaZ      = pSo->fDeltaZ;

  const int LOWER_BOUNDARY = 2; /* 0: Konst. Temp,  */
                                /* 1: Kein Fluß, */
                                /* 2: Berechnete Temp. (Daisy) */
  
  //pIce        = (float *) _MALLOC(iLayerAnz * sizeof(float));

  TEMP_ZERO_LAYER
  fTempSurface = pHL->fSoilTemp; //pCl->pWeather->fTempAve;//  erste einfachste obere Randbedingung

  /* ******************************************************************* */
  /* Achtung!     DeltaZ besitzt in dieser Funktion die Einheit Meter!!! */
  /* ******************************************************************* */
  
  DeltaZ /= (float) 1000;
 
  /* Speicher für Schichtvariable reservieren */

  pA        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pA,0x0,(iLayerAnz * sizeof(float)));
  pB        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pB,0x0,(iLayerAnz * sizeof(float)));
  pC        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pC,0x0,(iLayerAnz * sizeof(float)));
  pD        = (float *) _MALLOC(iLayerAnz * sizeof(float));
    memset(pD,0x0,(iLayerAnz * sizeof(float)));
  pF        = (double *) _MALLOC(iLayerAnz * sizeof(double));
    memset(pF,0x0,(iLayerAnz * sizeof(double)));
  pG        = (double *) _MALLOC(iLayerAnz * sizeof(double));
    memset(pG,0x0,(iLayerAnz * sizeof(double)));
                                   

    fE = (float)0.9 + (float)0.18 * pWL->pNext->fContAct;
    fS = (float) 5.67e-8;
    fR = fS * abspower(pCl->pWeather->fTempAve + (float)273.169, (float)4.0)
            * ((float) 0.605 + (float) 0.048 
            * abspower((float)1370.0 * pCl->pWeather->fHumidity / (float)100.0, (float)0.5));



  for (TEMP_SOIL_LAYERS)
   {
    /* ******************************************************* */
    /* Numerische Groessen in den Einheiten J, m, d, K         */
    /* ******************************************************* */

    /* Daisy (5-47) (ohne Frost) */
    fA = (pHL->fCapacity + pHL->fCapacityOld) / (float)2/DeltaT; /* [kJ /m^3 /K] */
    fB = (float) 0;                                       /* [kJ /m^3 /d] */
    fC = (pHL->fConduct + pHL->fConductOld)/(float)2/((float)2*DeltaZ*DeltaZ);/* [ kJ/d/K/m] */

    /* Daisy (5-50) */
    fD = (pHL->pNext->fConduct - pHL->pBack->fConduct
       +  pHL->pNext->fConductOld - pHL->pBack->fConductOld)* (float)0
       / ((float)4*DeltaZ)/((float)4*DeltaZ);                               /* [ kJ/K/m^2 /d] */

    fQ =  SPECCAP_WATER   * (float)0        
       * (pWL->fFluxDens    + pWL->pBack->fFluxDens
       +  pWL->fFluxDensOld + pWL->pBack->fFluxDensOld) 
       *  DENSITY_WATER
       / (float)4000/((float)4*DeltaZ);        /* [ J/kg /K * mm /d * kg/l * 1000] = [ kJ /K /m^2 /d]   */
    

    if (iLayer==1) /* Obere Randbedingung fuer Koeffizienten. */
      {
       pA[1] = (float) 0;                                    /* [ kJ /K /m^3 /d] */

       pB[1]  = fA
                + (float)2*fC
				 - pSlope[1] * LAT_HEAT_MELT * (float)1000 * DENSITY_ICE / DeltaT;
  //              + fC/ ((float)2 * DeltaZ * DeltaZ);            /* = [kJ /m^3 /K /d]   */

       pC[1] =  fQ-fC;              
         
       // pC[1] -= fC/ ((float) 2 * DeltaZ * DeltaZ);           /* = [kJ /m^3 /K /d]   */

       pD[1] = fA * pHL->fSoilTemp;

       pD[1] +=  fC
              * (pHL->pNext->fSoilTemp - (float)2 * pHL->fSoilTemp 
               + pHL->pBack->fSoilTemp + fTempSurface);

       pD[1] -=   pHL->fSoilTempOld*pSlope[1] * LAT_HEAT_MELT *(float)1000 * DENSITY_ICE / DeltaT;        /* [kJ /m^3 /d] */
 
       pD[1] += (((float)1.0 - pSo->fAlbedo)
                  * pCl->pWeather->fSolRad * (float) 11.905 + fR 
                  - fE * fS * abspower((pHL->fSoilTemp + (float)273.16), (float)4.0))
                  * (float) 84.0;

      } /*iLayer==1 */

    else
      {
         pA[iLayer] = -fQ + fD - fC;        /* [ J /K /m^3 /d] */

         pB[iLayer] = fA  + (float)2*fC - pSlope[iLayer] * LAT_HEAT_MELT * (float)1000 * DENSITY_ICE / DeltaT;

         pC[iLayer] = -fD + fQ -  fC;

         pD[iLayer] = fA * pHL->fSoilTemp
                      + fC * (pHL->pNext->fSoilTemp - (float)2 * pHL->fSoilTemp + pHL->pBack->fSoilTemp);
             
         pD[iLayer] += (fD - fQ )
                       * (pHL->pNext->fSoilTemp - pHL->pBack->fSoilTemp);
              
         pD[iLayer] -=  pHL->fSoilTempOld * pSlope[iLayer] * LAT_HEAT_MELT *(float)1000 * DENSITY_ICE / DeltaT;

      } /* iLayer>1 */
 } /* Schichtweise      */


  /* *********************************************** */
  /*      Löser des Gleichungssystems H T = D        */
  /*          mit H = tridiag(A,B,C)                 */
  /* *********************************************** */
    pF[1] = (double)pB[1];
	pG[1] = (double)pD[1];

  /* rekursive Definition von F und G: */
  for (iLayer=2; iLayer< iLayerAnz -1; iLayer++)
    {
       pF[iLayer] = (double)pB[iLayer] - (double)pA[iLayer] * (double)pC[iLayer-1] / 
		            (double)pF[iLayer-1];
	  
       pG[iLayer] = (double)pD[iLayer] - (double)pA[iLayer] * (double)pG[iLayer-1] /
		            (double)pF[iLayer-1];
  }

  pHL->fSoilTempOld = pHL->fSoilTemp;


  // **************************************************************** /
  //      Untere Randbedingung:                                       /
  // **************************************************************** /
  if (NewTenthDay(pTi))
	pHL->fSoilTemp =  Get_Daisy_LowerBC(exp_p);

  //* **************************** rückwärts errechnen: Tk => T(k-1) /
  //* Zeiger lpT, lpXH ist auf letzter Schicht, iLayer = 10 /

  for (iLayer--,     pHL = pHL->pBack;      //* Setze Zeiger auf letzte Bodenschicht: /
      (iLayer>0) && (pHL->pBack != NULL); 
       iLayer--,     pHL = pHL->pBack)
       {
        //* Speichern der alten Werte: /
        pHL->fSoilTempOld = pHL->fSoilTemp;
        //pHL->fSoilTemp    = (float)pG[iLayer] - (float)pF[iLayer] * pHL->pNext->fSoilTemp;
        pHL->fSoilTemp  = ((float)pG[iLayer] - 
			               (float)pC[iLayer] * pHL->pNext->fSoilTemp)/(float)pF[iLayer];
	   } 

  free(pA);
  free(pB);
  free(pC);
  free(pD);
  free(pF);
  free(pG);

  return 1;
} /* end Solve_LGS_CS */

/*******************************************************************************
** EOF */