/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:   Christoph Sperr
 *           B. Kloecking
 *           Christian Haberbosch
 *           Andreas Sonnenbichler
 *
 *------------------------------------------------------------------------------
 *
 * Description: Output routines
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: xdataout.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:14
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Using _MALLOC macro to detect memory leaks.
 * Unused method "XLogout" removed.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 15.11.01   Time: 20:53
 * Updated in $/ExpertN/ModLib/ModLib/control
 * Methode "Logout" des gleichnamigen Moduls aus dem System-Verzeichnis
 * von Expertn in das ModLib Modul kopiert. XDataOut als redundantes Modul
 * im System-Verzeichniss entfernt.
 * 
*******************************************************************************/

#include <crtdbg.h>
#include "xinclexp.h"
#include "xn_def.h"
#include "xh2o_def.h"
            
// from Time1.c
extern int EndDay(PTIME);

// from FileOut.c
extern HANDLE hFileLog;

extern int  fout_floatNorm(HANDLE,float,int,int);
extern int  fout_line(HANDLE, int);
extern int  fout_space(HANDLE, int);
extern int  fout_long(HANDLE, long far *);
extern int  foutDate_long(HANDLE, long far *);
//extern int  fout_double(HANDLE,double far *,int);
extern int  fout_int(HANDLE,int far *,int);
extern int  fout_string(HANDLE,LPSTR);
extern int FloatToCharacter(LPSTR,float,int);
//extern BOOL Dbg_DoubleToText(double,LPSTR,int);

int far pascal WaterTransformer(EXP_POINTER, float *Layer);
int far pascal NTransformer(EXP_POINTER, float *Layer);
int far pascal HeatTransformer(EXP_POINTER, float *Layer);
                          
// local: prototypes
int far pascal out_water(HANDLE, EXP_POINTER);
int far pascal out_heat(HANDLE, EXP_POINTER);
int far pascal out_nitrogen(HANDLE, EXP_POINTER);
int far pascal out_carbon(HANDLE, EXP_POINTER);
int far pascal out_plant(HANDLE, EXP_POINTER);
int far pascal out_balance(HANDLE, EXP_POINTER);
int far pascal out_DSSAT_Sum(HANDLE, EXP_POINTER);
int far pascal out_DSSAT_Growth(HANDLE, EXP_POINTER);
int far pascal out_DSSAT_Carbon(HANDLE, EXP_POINTER);
int far pascal out_DSSAT_Water(HANDLE, EXP_POINTER);
int far pascal out_DSSAT_Nitrogen(HANDLE, EXP_POINTER);

int far pascal out_StartValue(HANDLE, EXP_POINTER);


// Hilfsvariablen
char    acDummy[500];
float   *Layer;

extern int iGECROS;


/****************************************************************************************
  Ausgabefunktionen für Expert-N, Datenmodule

  Fkt.: Ausgabe der Daten für Water, Heat, ...
  Bearb: as, 05.02.97
 ****************************************************************************************/
int far pascal out_water(HANDLE hFile, EXP_POINTER)
{
  DECLARE_H2O_POINTER  
  DECLARE_COMMON_VAR
  PWEATHER pCW = pCl->pWeather;
  PEVAP    pWE = pWa->pEvap;
  
  pSL; pSW;      // nur zur Unterdrückung von 'not referenced' warnings ....
  
  pWB = pWa->pWBalance;  
  pWL = pWa->pWLayer;
  
  Layer=_MALLOC(sizeof(float)*4); 
  WaterTransformer(exp_p, Layer);    // Transformation x Simschichten -> 4 Ausgabeschichten
    
  //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);
  foutDate_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCl->fCumRain, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWa->fCumInfiltration, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWa->fRunOffCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWa->fPotETCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fPotCumEvap, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fActCumEvap, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fPotCumTransp, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fActCumTransp, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fCumInterception, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWa->fCumLeaching, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fProfil, 2, 3); fout_string(hFile, "\t");
  // jeweils in 30cm Abschnitten berechnen.
  if (Layer != NULL)
     {
     fout_floatNorm(hFile, Layer[0]*100, 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[1]*100, 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[2]*100, 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[3]*100, 2, 3); fout_string(hFile, "\t");
     }
  else fout_string(hFile, "NO MEM\tNO MEM\tNO MEM\tNO MEM\t"); 
  fout_floatNorm(hFile, pWa->fPondWater, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCW->fRainAmount, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWa->fInfiltDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWa->fRunOffDay, 2, 3); fout_string(hFile, "\t");    
  fout_floatNorm(hFile, pWB->fPotETDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fPotEVDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fActEVDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fPotTranspDay, 2, 3);  fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWB->fActTranspDay, 2, 3);  fout_string(hFile, "\t");
  if (pPl == NULL) fout_floatNorm(hFile, (float)0, 2, 3);  
  else fout_floatNorm(hFile, pPl->pPltWater->fInterceptDay, 2, 3);  
  fout_string(hFile, "\t");
  fout_floatNorm(hFile, pWa->fLeaching, 2, 3);
  
  fout_line(hFile, 1);     
  
  if (Layer!=NULL) free(Layer);
  return 0;
}                             


int far pascal out_heat(HANDLE hFile, EXP_POINTER)
{
  PWEATHER pCW = pCl->pWeather;
  PHLAYER  pHL = pHe->pHLayer;   
  
  Layer=_MALLOC(sizeof(float)*6); 
    
  HeatTransformer(exp_p, Layer);
  pHL=pHe->pHLayer->pNext;  //Reinitialisiere auf erste Schicht
        
  //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);
  foutDate_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCW->fTempAve, 2, 3); fout_string(hFile, "\t");
  //in 5 verschiedenen Schichten ...
  if (Layer!=NULL)
     {
     fout_floatNorm(hFile, Layer[0], 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[1], 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[2], 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[3], 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[4], 2, 3);
     }
  else fout_string(hFile, "NO MEM\tNO MEM\tNO MEM\tNO MEM\tNO MEM\t");
  
  fout_line(hFile, 1);
      
  if (Layer!=NULL) free(Layer);                            

  return 0;
}



int far pascal out_nitrogen(HANDLE hFile, EXP_POINTER)
{
  float sumNLitterMinerC, sumNHumusMinerC;
    
  PCBALANCE pCB;
  DECLARE_COMMON_VAR 
  DECLARE_N_POINTER
            
  pNF; pPA;       // nur zur Unterdrückung der 'not referenced' warnings          
            
  Layer=_MALLOC(sizeof(float)*4); 
  NTransformer(exp_p, Layer);
  
    sumNLitterMinerC = sumNHumusMinerC = (float)0;   
    for (N_SOIL_LAYERS)
      { 
      sumNLitterMinerC+=pCL->fNLitterMinerCum;
      sumNHumusMinerC +=pCL->fNHumusMinerCum;
      }
  
  
  pCP = pCh->pCProfile;
  pCB = pCh->pCBalance;
    
  //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);                            
  foutDate_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNO3LeachCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNH4LeachCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, sumNLitterMinerC, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, sumNHumusMinerC, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNMinerCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNH4NitrCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNO3DenitCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNImmobCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dN2OEmisCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNOEmisCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dN2EmisCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dNH3VolatCum, 2, 3); fout_string(hFile, "\t");
 
  if (Layer != NULL)
     {
     fout_floatNorm(hFile, Layer[0], 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[1], 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[2], 2, 3); fout_string(hFile, "\t");
     fout_floatNorm(hFile, Layer[3], 2, 3); fout_string(hFile, "\t");
     }
  else fout_string(hFile, "NO MEM\tNO MEM\tNO MEM\tNO MEM\t");
  
  fout_floatNorm(hFile, pCB->fNO3NProfile, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCB->fNH4NProfile, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNO3LeachDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNH4LeachDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNLitterMinerDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNHumusMinerDay, 2, 3); fout_string(hFile, "\t");   
  fout_floatNorm(hFile, pCP->fNMinerDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNH4NitrDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNO3DenitDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNImmobDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fN2OEmisDay, 2, 3);fout_string(hFile, "\t"); 
  fout_floatNorm(hFile, pCP->fNOEmisDay, 2, 3);fout_string(hFile, "\t"); 
  fout_floatNorm(hFile, pCP->fN2EmisDay, 2, 3);fout_string(hFile, "\t"); 
  fout_floatNorm(hFile, pCP->fNH3VolatDay, 2, 3);fout_string(hFile, "\t"); 
  //fout_floatNorm(hFile, pCP->fNetNMinerCum30, 2, 3); 
  
  fout_line(hFile, 1);  
    
  if (Layer != NULL) free(Layer);
  return 0;
}


int far pascal out_carbon(HANDLE hFile, EXP_POINTER)
{
    
  PCPROFILE pCP;
  DECLARE_COMMON_VAR 
  
  pCP = pCh->pCProfile;
    
  //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);                            
  foutDate_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dCO2EmisCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dCO2SurfEmisCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dCH4ImisCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCO2EmisDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCO2SurfEmisDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCH4ImisDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCLitter, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCManure, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCHumus, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCLitterSurf, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCManureSurf, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCHumusSurf, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCStandCropRes, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fCSoilMicBiom, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNLitter, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNManure, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNHumus, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNLitterSurf, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNManureSurf, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNHumusSurf, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNStandCropRes, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fNSoilMicBiom, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fDON, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pCP->fDOC, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dDONLeachCum, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, (float)pCP->dDOCLeachCum, 2, 3); fout_string(hFile, "\t");
  
  fout_line(hFile, 1);  
    
  return 0;
}

int far pascal out_plant(HANDLE hFile, EXP_POINTER)
{
  PDEVELOP     pPD;    
  PCANOPY      pPC; 
  PMODELPARAM  pPM;  
  PROOT        pPR; 
  PBIOMASS     pPB; 
  PPLTNITROGEN pPN; 
  PPLTWATER    pPW; 
  
  if (pPl != NULL)
  {
  pPD = pPl->pDevelop;   
  pPC = pPl->pCanopy;
  pPM = pPl->pModelParam; 
  pPR = pPl->pRoot;
  pPB = pPl->pBiomass;
  pPN = pPl->pPltNitrogen;
  pPW = pPl->pPltWater;

  //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);
  foutDate_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPD->fDevStage, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPC->fLAI, 2, 3); fout_string(hFile, "\t");                
  //SG 20110809: Ausgabe der Grain/m2 für AgMIP anstelle von Tillers/m2
  fout_floatNorm(hFile, pPC->fGrainNum*pMa->pSowInfo->fPlantDens, 2, 3); fout_string(hFile, "\t");                     
  //fout_floatNorm(hFile, pPC->fTillerNum, 2, 3); fout_string(hFile, "\t");                     
  //fout_floatNorm(hFile, pPR->fDepth/(float)10, 2, 3); fout_string(hFile, "\t");   
  fout_floatNorm(hFile, pPR->fDepth, 2, 3); fout_string(hFile, "\t");   //Ceres, SPASS
  fout_floatNorm(hFile, pPB->fRootWeight, 2, 3); fout_string(hFile, "\t");   
  //SG 20180410: GECROS-SB - 'stem' is 'storage organ' --> output of stem weight
  if(((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"OR")))&&(iGECROS > 0)) //Unerscheidung GECROS-SB <--> andere Crop Modelle
  {
	  fout_floatNorm(hFile, pPB->fLeafWeight, 2, 3); fout_string(hFile, "\t");  //sugar beet, oilradish
	  fout_floatNorm(hFile, pPB->fStemWeight, 2, 3); fout_string(hFile, "\t");
  }
  else
  {
	  fout_floatNorm(hFile, pPB->fStovWeight, 2, 3); fout_string(hFile, "\t");  //other crops
  fout_floatNorm(hFile, pPB->fGrainWeight, 2, 3); fout_string(hFile, "\t");
  }
  fout_floatNorm(hFile, pPB->fTotalBiomass, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPN->fRootCont, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPN->fStovCont, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPN->fGrainCont, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPN->fCumActNUpt, 2, 3); fout_string(hFile, "\t"); 
  fout_floatNorm(hFile, pPN->fRootActConc*(float)100, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPN->fVegActConc*(float)100, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPN->fGrainConc*(float)100, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPN->fActNUpt, 2, 3); fout_string(hFile, "\t");  
  fout_floatNorm(hFile, pPW->fPotTranspDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPW->fActTranspDay, 2, 3); fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPW->fActTranspDay + pWa->pWBalance->fActEVDay, 2, 3); fout_string(hFile, "\t");//SG20160609
  fout_floatNorm(hFile, pPW->fCumUptake, 2, 3);
  //SG 20160726: für AgMIP_ET
  fout_string(hFile, "\t");
  fout_floatNorm(hFile, pPC->fExpandLeafNum, 2, 3); fout_string(hFile, "\t");  
  fout_floatNorm(hFile, pPB->fLeafWeight, 2, 3); fout_string(hFile, "\t");  
  fout_floatNorm(hFile, pPB->fStemWeight, 2, 3);  

  
  fout_line(hFile, 1);  
  }
  
  return 0;
}


int far pascal out_balance(HANDLE hFile, EXP_POINTER)
{
 PWBALANCE pWB = pWa->pWBalance; 
 PCBALANCE pCB = pCh->pCBalance;
 
 //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);
 foutDate_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
 fout_floatNorm(hFile, pWB->fBalance, 2, 3); fout_string(hFile, "\t");
 fout_floatNorm(hFile, pWB->fInput, 2, 3); fout_string(hFile, "\t");
 fout_floatNorm(hFile, pWB->fOutput, 2, 3); fout_string(hFile, "\t");                 
 fout_floatNorm(hFile, (pWB->fProfil + pWB->fProfileIce), 2, 3); fout_string(hFile, "\t");
 fout_floatNorm(hFile, (float)pCB->dNBalance, 2, 3); fout_string(hFile, "\t");
 fout_floatNorm(hFile, (float)pCB->dNInputCum, 2, 3); fout_string(hFile, "\t");
 fout_floatNorm(hFile, (float)pCB->dNOutputCum, 2, 3); fout_string(hFile, "\t");
 fout_floatNorm(hFile, (float)pCB->dNProfile, 2, 3); fout_string(hFile, "\t");
 
 fout_line(hFile, 1);
 
 return 0;
}


/****************************************************************************************
  Ausgabefunktionen für DSSAT, Datenmodule

  Fkt.: ???
  Bearb: B. Klöcking, 16.08.94
 ****************************************************************************************/

int far pascal out_DSSAT_Sum(HANDLE hFileDSSATSum, EXP_POINTER)
{
  return 0;
}


int far pascal out_DSSAT_Growth(HANDLE hFileDSSATGrowth, EXP_POINTER)
{
  fout_line(hFileDSSATGrowth,1);

  return 0;
}

int far pascal out_DSSAT_Carbon(HANDLE hFileDSSATCarbon, EXP_POINTER)
{
  return 0;
}


int far pascal out_DSSAT_Water(HANDLE hFileDSSATWater, EXP_POINTER)
{
  fout_string(hFileDSSATWater," New Release Expected !");  
  fout_line(hFileDSSATWater,1);

  return 0;
}


int far pascal out_DSSAT_Nitrogen(HANDLE hFileDSSATNitrogen, EXP_POINTER)
{
  fout_string(hFileDSSATNitrogen," New Release Expected !");  
  fout_line(hFileDSSATNitrogen,1);

  return 0;
}





/*****************************************************************************************
 *
 * TRANSFORMER-FUNKTIONEN
 *
 * Die Transformerfunktionen wandeln die Werte der n Simultionsschichten aus Expert-N   
 * in m Ausgabeschichten für die Log-Dateien, d.h. transformieren sie.
 *
 * WATER:               
 * Wandlung in 4 Ausgabeschichten: 0-30, 30-60, 60-90. 90-120cm.
 * Bei der Wassergehaltsausgabe [in Vol%] wird die Transformation so durchgeführt,
 * daß der jeweilige Anteil in echten Vol-% erhalten bleibt. Die Überschreitung von
 * Ausgabeschichtgrenzen durch die Sim.-schichten wird korrekt gehandhabt (d.h. an-
 * teilig berechnet).
 *
 * NITROGEN:
 * Wandlung in 4 Ausgabeschichten: 0-30, 30-60, 60-90. 90-120cm.
 * Hier wird die N-Menge für die einzelnen Ausgabeschichten aufsummiert. Über-
 * schreitungen von Ausgabeschichtgrenzen werden jeweils anteilsmäßig korrekt
 * addiert.
 *
 * HEAT:
 * Zuordnung zu Temperaturen in 5, 10, 20, 50, 100cm Tiefe.
 * Es wird jeweils die Temperatur angegeben, die in der Simulationsschicht herrscht, 
 * die die Ausgabetiefe beinhaltet. Bei 'Grenzgängern' wird die Sim.-schicht verwandt,
 * die an der Ausgabetiefe _endet_.
 *
 * Hinweis:
 * Für die Transformationen wird ein Array (Dim. 1) der jeweils benötigten Länge,
 * idR. 4 (bei HEAT: 5) eingesetzt. Der benötigte Speicher wird mittels _MALLOC allokiert
 * und am Ende mit FREE wieder freigegeben. Bezeichnung: *LAYER.
 * 
 * as, 03.03.97
 *****************************************************************************************/
 


// local: prototypes
int far pascal WaterTransformer(EXP_POINTER, float *Layer);
int far pascal NTransformer(EXP_POINTER, float *Layer);
int far pascal HeatTransformer(EXP_POINTER, float *Layer);


/**********************************************************************************/
/*                                                                                */
/*  module  :   WaterTransformer.c                                                */
/*  purpose :   Aufteilung der Simulationsschichten in Schichten von              */
/*              0-30cm, 30-60cm, 60-90cm und 90-120cm                             */
/*  date  :     as, 13.12.96                                                      */
/*                                                                                */
/**********************************************************************************/

int far pascal WaterTransformer(EXP_POINTER, float *Layer)
/* Layer: Zeiger auf die auszugebenden 4 Schichten */
{
 PWLAYER   pWL = pWa->pWLayer->pNext;     // Zeiger auf erste Schicht 
 PSLAYER   pSL = pSo->pSLayer->pNext;

 float z1 = (float)0;		/* Startmarke Simschicht [mm] */
 float z2 = (float)0;      /* Endmarke Simschicht [mm] */
 int s;             /* aktuelle AUSGABE-Schicht */
 
 if (Layer == NULL) return 0;
 
 for (s=0;s<=3;s++)          /* Ausgabeschichten werden auf Null gesetzt */
     Layer[s] = (float)0;
     
 s = 1;                      /* s jetzt 1, daher while-wend bis s<=4 !!! */
 
 while (s<=4)
 { 
    if (pWL->pNext == NULL || pSL->pNext == NULL) 
      {                      /* Abbruchbedingung, wenn keine Simschichten mehr */
       //Layer[s-1] *= 300 / (z2 - (s-1)*300);    // Interpoltion bis Schichtende
       if (fabs((double)z2-(double)s*300) > EPSILON ) Layer[s-1] = (float)-0.99;     //oder -99
       for (s++;s<=4;s++) Layer[s-1]=(float)-0.99;
       return 1;    
      }
                                
    z1 = z2;                 // alte Endmarke ist jetzt neue Anfangsmarke der Sim.schichten
    z2 += pSL->fThickness;   /* erhöhe Endmarke Simschicht um Dicke */
    
    if (z2 <= s * 300)       /* Simschicht ganz in (s-1)-ter Ausgabeschicht */
      {
      Layer[s-1] += pWL->fContAct * (z2-z1)/300;      /* WG (%) * Dicke/Gesamtdicke */
      }
    else if ((z2 > s*300) && (z2 <= (s+1)*300))
      {                      /* Simschicht überschreitet eine Ausgabeschichtgrenze */
      Layer[s-1] += pWL->fContAct * (300*s-z1)/300;   /* Anteil aktuelle Schicht */
      s++; if (s>4) return 1;                         /* Erhöhe aktuelle Schicht +1, wenn >4 exit */
      Layer[s-1] += pWL->fContAct * (z2-300*(s-1))/300; /* Anteil nächste (jetzt aktuelle!) Schicht */
      }
    else if ((z2 > (s+1)*300) && (z2 <= (s+2)*300))
      {                      /* Simschicht überschreitet zwei Ausgabeschichtgrenzen */              
      Layer[s-1] += pWL->fContAct * (300*s-z1)/300;   
      s++; if (s>4) return 1;
      Layer[s-1] += pWL->fContAct;                     /* WG (%) Ausgabeschicht entspricht */
      s++; if (s>4) return 1;                         /* WG Simschicht, da ganz enthalten */                         
      Layer[s-1] += pWL->fContAct * (z2-300*(s-1))/300;
      }
    else if ((z2 > (s+2)*300)) 
      {                      /* Simschicht überschreitet mehr als zwei Ausgabeschichtgrenzen */
      Layer[s-1] += pWL->fContAct * (300*s-z1)/300;
      s++; if (s>4) return 1;
      Layer[s-1] += pWL->fContAct;
      s++; if (s>4) return 1;                                                   
      Layer[s-1] += pWL->fContAct;
      s++; if (s>4) return 1;                                                   
      if (z2 <= s*300)  // da s jetzt um 3 erhöht, d.h. mind. 4: noch innerhalb äußerster Grenze bei 1200mm
         Layer[s-1] += pWL->fContAct * (z2-300*(s-1))/300; 
      else
         {              // wenn außerhalb letzter Ausgabeschicht volle Übernahme und exit
         Layer[s-1] += pWL->fContAct;
         return 1;
         }
      }
      
  
    pWL = pWL->pNext;  // Zeiger auf nächste Schicht
    pSL = pSL->pNext;
 }                                                    
   
return 1;
}


/**********************************************************************************/
/*                                                                                */
/*  module  :   N-Transformer.c                                                   */
/*  purpose :   Aufteilung der Simulationsschichten in Schichten von              */
/*              0-30cm, 30-60cm, 60-90cm und 90-120cm                             */
/*  date  :     as, 13.12.96                                                      */
/*                                                                                */
/**********************************************************************************/

int far pascal NTransformer(EXP_POINTER, float *Layer)
/* Erklärungen analog zu WaterTransformer, s. also dort !!! */
{
 PCLAYER   pCL = pCh->pCLayer->pNext;
 PSLAYER   pSL = pSo->pSLayer->pNext;

 float z1 = (float)0;		/* Startwert Simschicht */
 float z2 = (float)0;      /* Endwert Simschicht */
 int s;
 
 if (Layer == NULL) return 0;
 
 for (s=0;s<=3;s++)          /* Ausgabeschichten werden auf Null gesetzt */
     Layer[s] = (float)0;
     
 s = 1;                      /* s jetzt 1, daher while-wend bis s<=4 !!! */
 
 while (s<=4)
 { 
    if (pCL->pNext == NULL || pSL->pNext == NULL) 
       {
       //Layer[s-1] *= 300 / (z2 - (s-1)*300);    // Interpolation fehlende Werte
       if (fabs((double)z2-(double)s*300) > EPSILON ) Layer[s-1] = (float)-99;    // ... oder -99 ?
       for (s++;s<=4;s++) Layer[s-1]=(float)-99;
       return 1;              /* Abbruchbedingung, wenn weniger Sim-Schichten als                             Ausgabeschichten */
       }
       
    z1 = z2;
    z2 += pSL->fThickness;   
    
    if (z2 <= s * 300)       /* Simschicht ganz in (s-1)-ter Ausgabeschicht */
      {
      Layer[s-1] += pCL->fNO3N;
      }
    else if ((z2 > s*300) && (z2 <= (s+1)*300))
      {                      /* Simschicht überschreitet eine Ausgabeschichtgrenze */
      Layer[s-1]+=(300*s-z1)/(pSL->fThickness)*pCL->fNO3N;
      s++; if (s>4) return 1;/* wird anteilsmäßig aufgeteilt */                                  
      Layer[s-1]+=(z2-300*(s-1))/(pSL->fThickness)*pCL->fNO3N;
      }
    else if ((z2 > (s+1)*300) && (z2 <= (s+2)*300))
      {                      /* Simschicht überschreitet zwei Ausgabeschichtgrenzen */              
      Layer[s-1]+=(300*s-z1)/(pSL->fThickness)*pCL->fNO3N;
      s++; if (s>4) return 1;     
      Layer[s-1]+=pCL->fNO3N;
      s++; if (s>4) return 1;                                                   
      Layer[s-1]+=(z2-300*(s-1))/(pSL->fThickness)*pCL->fNO3N;
      }
    else if (z2 > (s+2)*300)
      {                      /* Simschicht überschreitet mehr als 2 Ausgabeschichtgrenzen */
      Layer[s-1]+=(300*s-z1)/(pSL->fThickness)*pCL->fNO3N;
      s++; if (s>4) return 1;
      Layer[s-1]+=pCL->fNO3N;
      s++; if (s>4) return 1;
      Layer[s-1]+=pCL->fNO3N;
      s++; if (s>4) return 1;                                                   
      if (z2 <= s*300)  // da s jetzt um 3 erhöht, d.h. mind. 4 !!!
        Layer[s-1]+=(z2-300*(s-1))/(pSL->fThickness)*pCL->fNO3N;
      else       
        {
        Layer[s-1]+=pCL->fNO3N;
        return 1;
        }
      }
      
  
    pCL = pCL->pNext;
    pSL = pSL->pNext;
 }                                                    
   
return 1;
}



/**********************************************************************************/
/*                                                                                */
/*  module  :   HeatTransformer.c                                                 */
/*  purpose :   Aufteilung der Simulationsschichten in Tiefen zu                  */
/*              5, 10, 20, 50, 100cm                                              */
/*  date  :     as, 03.03.97                                                      */
/*                                                                                */
/**********************************************************************************/

int far pascal HeatTransformer(EXP_POINTER, float *Layer)
/* Layer: Zeiger auf die auszugebenden 5 Schichten */
{              
 PHLAYER   pHL = pHe->pHLayer->pNext;
 PSLAYER   pSL = pSo->pSLayer->pNext;              
 
 BOOL bWriteAverage = TRUE;
   
 float z1 = (float)0;		/* Startwert Simschicht */
 float z2 = (float)0;      /* Endwert Simschicht */
 int s=0;        
 
 /*#ifdef DEBUG    // wenn Debugmodus, dannwird kein MW berechnet, daher Testwertausgabe
   bWriteAverage = FALSE;  // as, 110697
 #endif*/
 
 if (Layer == NULL) return 0;
 
 for (;s<=4;s++)          /* Ausgabeschichten werden initialisiert */
     Layer[s] = (float)-99;
     

 for (; (pHL->pNext!=NULL && pSL->pNext!=NULL); (pHL=pHL->pNext, pSL=pSL->pNext))
 /* gekürzte Fassung von N_SOIL_LAYERS */
 {              
    z1 = z2;					// alter Endwert = neuer Startwert
    z2 += pSL->fThickness;      // neuer Endwert erhöht um Sim.-Schichtdicke
           
    // wenn gewünschte Tiefe in SimSchicht oder dort ihr Ende, dann Zuordnung       
    if (z1<50   && z2>=50)   Layer[0]= (bWriteAverage ? pHL->fSoilTempAve : pHL->fSoilTemp);
    if (z1<100  && z2>=100)  Layer[1]= (bWriteAverage ? pHL->fSoilTempAve : pHL->fSoilTemp);
    if (z1<200  && z2>=200)  Layer[2]= (bWriteAverage ? pHL->fSoilTempAve : pHL->fSoilTemp);
    if (z1<500  && z2>=500)  Layer[3]= (bWriteAverage ? pHL->fSoilTempAve : pHL->fSoilTemp);
    if (z1<1000 && z2>=1000) Layer[4]= (bWriteAverage ? pHL->fSoilTempAve : pHL->fSoilTemp);
      /* wenn WriteAverage, also nur eine Ausgabe je Tag, dann Durchschnitt */
 }                                                    
   
return 1;
}

/*******************************************************************************************
**
**  Funktion :  ch 21.3.97
**
********************************************************************************************/
int WINAPI _loadds Logout(float SimTime, LPSTR lpOut)
{ 
	if (hFileLog)  
  {
    fout_floatNorm(hFileLog, SimTime, 2, 3); 
    fout_string(hFileLog, (LPSTR)" ");
    fout_string(hFileLog, lpOut);
    fout_line(hFileLog, 1);     
  }  
  else
  {
    /* something goes wrong! */
  }
	 
  return 1;
}               

/******************************************************************************
** EOF */

