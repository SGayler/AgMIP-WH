/*******************************************************************************
 *
 * Copyright (c) by 
 *
 *------------------------------------------------------------------------------
 *
 * Contents:  This is the new interface for linking the plant process modules
 *            in the ZCROPMOD.DLL to the ExpertN simulation system.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 6 $
 *
 * $History: plant.c $
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 23.01.02   Time: 14:08
 * Updated in $/Projekte/ExpertN/ModLib/plant
 * Anbindung an Datenbank auch unter Win XP möglich. Fitting in Gang
 * gesetzt.
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:17
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/plant
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:42
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/plant
 * Using _MALLOC macro to detect memory leaks.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/plant
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 ******************************************************************************/

#include <memory.h>
#include <crtdbg.h>

//Module defined include files
#include "xinclexp.h"
#include "xlanguag.h"     
#include "ZCropMod.H"
#include "gecros.h"

extern int WINAPI Message(long, LPSTR);        // from util_fct.c
extern int introduce(LPSTR lpName);            // from util_fct.c

extern int SimStart(PTIME);                    // from time1.c
extern int SimEnd(PTIME);                    // from time1.c
extern int NewDay(PTIME);                      // from time1.c 

//extern long DaysBetweenTwoLongDate(long lDate1, long lDate2);  // aus toolslib.c

extern int PlantVariableInitiation(EXP_POINTER);
extern int SPASS_Genotype_Read(HANDLE hInstance);
extern int PlantHeightGrowth_SUCROS(EXP_POINTER);
extern int PlantHeightGrowth_CERES(EXP_POINTER);//cl 060406

extern int iGECROS;
extern int jGECROS;

#define SimulationStarts      (SimStart(pTi))
#define NewDayAndPlantGrowing (NewDay(pTi)&&(pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
#define PlantIsGrowing        ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
#define SimulationEnds        (SimEnd(pTi))

OFSTRUCT   ofStruct;
HANDLE     hFileTest;
//BOOL    bMaturity,bPlantGrowth;
BOOL    bChangeJulianDay,bCalculateJulianDay;

static float FAR afActLayRootWatUpt[MAXSCHICHT];

extern float fPotTraDay,fActTraDay; // ep 210599

//SG 20110810: CO2 als Input von *.xnm (für AgMIP)
extern float fAtmCO2;


int   iStandingResidues; // ep 200701

OFSTRUCT   ofStruct;
//============================================================================================
//             DLL-Funktionen                       
//============================================================================================

int WINAPI  dllEWDevelopment_SPASS(EXP_POINTER);
int WINAPI  dllEWRootSystem_SPASS(EXP_POINTER);
int WINAPI  dllEWPhotosynthesis_SPASS(EXP_POINTER);
int WINAPI  dllEWPotTransp_SPASS(EXP_POINTER);
int WINAPI  dllEPPotTra_SPASSAlt(EXP_POINTER);
int WINAPI  dllEWActTransp_SPASS(EXP_POINTER);
int WINAPI  dllEWBiomassGrowth_SPASS(EXP_POINTER);
int WINAPI  dllEWCanopyForm_SPASS(EXP_POINTER);
int WINAPI  dllEWOrganSenesz_SPASS(EXP_POINTER);
int WINAPI  dllEWNitrogenDemand_SPASS(EXP_POINTER);
int WINAPI  dllEWNitrogenUptake_SPASS(EXP_POINTER);

int WINAPI  dllEWDevelopment_PLAGEN(EXP_POINTER);
int WINAPI  dllEWRootSystem_PLAGEN(EXP_POINTER);
int WINAPI  dllEWPhotosynthesis_PLAGEN(EXP_POINTER);
int WINAPI  dllEWPotTransp_PLAGEN(EXP_POINTER);
int WINAPI  dllEWActTransp_PLAGEN(EXP_POINTER);
int WINAPI  dllEWBiomassGrowth_PLAGEN(EXP_POINTER);
int WINAPI  dllEWCanopyForm_PLAGEN(EXP_POINTER);
int WINAPI  dllEWOrganSenesz_PLAGEN(EXP_POINTER);
int WINAPI  dllEWNitrogenDemand_PLAGEN(EXP_POINTER);
int WINAPI  dllEWNitrogenUptake_PLAGEN(EXP_POINTER);

int WINAPI  dllEWDevelopment_CERES(EXP_POINTER);
int WINAPI  dllEWRootSystem_CERES(EXP_POINTER);
int WINAPI  dllEWPhotosynthesis_CERES(EXP_POINTER);
int WINAPI  dllEWPotTransp_CERES(EXP_POINTER);
int WINAPI  dllEWActTransp_CERES(EXP_POINTER);
int WINAPI  dllEWBiomassGrowth_CERES(EXP_POINTER);
int WINAPI  dllEWCanopyForm_CERES(EXP_POINTER);
int WINAPI  dllEWOrganSenesz_CERES(EXP_POINTER);
int WINAPI  dllEWNitrogenDemand_CERES(EXP_POINTER);
int WINAPI  dllEWNitrogenUptake_CERES(EXP_POINTER);

int WINAPI  dllEWDevelopment_SUCROS(EXP_POINTER);
int WINAPI  dllEWRootSystem_SUCROS(EXP_POINTER);
int WINAPI  dllEWPhotosynthesis_SUCROS(EXP_POINTER);
int WINAPI  dllEWPotTransp_SUCROS(EXP_POINTER);
int WINAPI  dllEWActTransp_SUCROS(EXP_POINTER);
int WINAPI  dllEWBiomassGrowth_SUCROS(EXP_POINTER);
int WINAPI  dllEWCanopyForm_SUCROS(EXP_POINTER);
int WINAPI  dllEWOrganSenesz_SUCROS(EXP_POINTER);
int WINAPI  dllEPNitrogenDemand_WAVE(EXP_POINTER);
int WINAPI  dllEPNitrogenUptake_WAVE(EXP_POINTER);

int WINAPI _loadds dllCHRootSystem_LEACH(EXP_POINTER);
int WINAPI _loadds dllCHCanopyForm_LEACH(EXP_POINTER);
int WINAPI _loadds dllCHPotTransp_LEACH(EXP_POINTER);
int WINAPI _loadds dllCHActTransp_LEACH(EXP_POINTER);
int WINAPI _loadds dllCHNitrogenUptake_LEACH(EXP_POINTER);
int WINAPI _loadds dllCHPotTransp_LEACH(EXP_POINTER);
int WINAPI _loadds dllCHPlantLeach(EXP_POINTER);

//int WINAPI  dllEPMain_GECROS(EXP_POINTER);
int WINAPI  dllEPDevelopment_GECROS(EXP_POINTER);
int WINAPI  dllEPRootSystem_GECROS(EXP_POINTER);
int WINAPI  dllEPPhotosynthesis_GECROS(EXP_POINTER);
//int WINAPI  dllEPPotTransp_GECROS(EXP_POINTER);
int WINAPI  dllEPActTransp_GECROS(EXP_POINTER);
int WINAPI  dllEPBiomassGrowth_GECROS(EXP_POINTER);
int WINAPI  dllEPCanopyForm_GECROS(EXP_POINTER);
int WINAPI  dllEPOrganSenesz_GECROS(EXP_POINTER);
int WINAPI  dllEPNitrogenDemand_GECROS(EXP_POINTER);
int WINAPI  dllEPNitrogenUptake_GECROS(EXP_POINTER);

//============================================================================================
//             Internal functions                      
//============================================================================================
void GetDataFromExpert_N(EXP_POINTER,LPSTR,PZDVS,PZCAN,PZBIOM,PZTRP,PZPLTN,PZROOT,PZSOIL);
void SetDataToExpert_N(EXP_POINTER,LPSTR,PZDVS,PZCAN,PZBIOM,PZTRP,PZPLTN,PZROOT,PZSOIL);
void PlantAccumulationForExpert_N(PPLANT pPl);
void SetAllPlantRateToZero(PPLANT pPl,PSPROFILE pSo);


//=============================================================================================
// DLL-Function:  dllEWDevelopment_PLAGEN
// Description:    Phasic development module for different crops (Wang,1997)
// Updating Date:    12.10.97                   
//=============================================================================================
OFSTRUCT   ofStruct;
  
int DevelopmentCheckAndPostHarvestManagement(EXP_POINTER);

int WINAPI  dllEWDevelopment_PLAGEN(EXP_POINTER)
{
  DECLARE_COMMON_VAR     
  ZDVS  Stg;
  ZGROWTH  zGrw;
  ZCANOPY  zCan;
  ZROOT  zRt;
  ZPLTNC  zPltNc;
  ZPLTN  zPltN;
  ZSOIL  zSoil;
  ZSOWINFO zSowInfo;

  memset(&Stg,  0,sizeof(ZSTAGE));
  memset(&zGrw,  0,sizeof(ZGROWTH));
  memset(&zCan,  0,sizeof(ZCANOPY));
  memset(&zRt,  0,sizeof(ZROOT));
  memset(&zPltNc,  0,sizeof(ZPLTNC));
  memset(&zPltN,  0,sizeof(ZPLTN));
  memset(&zSoil,  0,sizeof(ZSOIL));
  memset(&zSowInfo,0,sizeof(ZSOWINFO));

  if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return 0;

  zSowInfo.fSeedDepth    =pMa->pSowInfo->fSowDepth;      
  zSowInfo.fSeepDensity  =pMa->pSowInfo->fPlantDens;     
  zSowInfo.fRowWidth    =pMa->pSowInfo->fRowWidth;     

  //Plant variable initialization
  if (pTi->pSimTime->fTimeAct == (float)pMa->pSowInfo->iDay) 
    {//Initialization at sowing date
    GetDataFromExpert_N(exp_p,pCrop,&Stg,&zCan,&zGrw,NULL,&zPltN,&zRt,&zSoil);
    Initialization(pCrop,0,&zSowInfo,&Stg,&zGrw,&zCan,&zRt,&zPltNc,&zSoil);
    SetDataToExpert_N(exp_p,pCrop,&Stg,&zCan,&zGrw,NULL,NULL,&zRt,&zSoil);

    pMa->pSowInfo->fSowDepth  =zSowInfo.fSeedDepth;      
    pMa->pSowInfo->fPlantDens  =zSowInfo.fSeepDensity;     
    pMa->pSowInfo->fRowWidth  =zSowInfo.fRowWidth;     
    }
    
  if SimulationStarts
    {
     pPl->pDevelop->bMaturity=FALSE;
     introduce((LPSTR)"dllEWDevelopment_PLAGEN");
    }
        
  if NewDayAndPlantGrowing
    {
    float    fPhotoperiod;   
    //-------------------------------------------------------------------
    //The NEW code with the ZCROPMOD.DLL
    GetDataFromExpert_N(exp_p,pCrop,&Stg,NULL,NULL,NULL,NULL,NULL,NULL); 

    fPhotoperiod=DaylengthAndPhotoperiod(pLo->pFarm->fLatitude,
                         pTi->pSimTime->iJulianDay,
                         GET_PHOTOPERIOD);
    PhenologicalDevelopment(pCrop,
                    pCl->pWeather->fTempMax,
                    pCl->pWeather->fTempMin,
                    fPhotoperiod,
                     &Stg,
                     &zSowInfo,
                     NULL);
  
     SetDataToExpert_N(exp_p,pCrop,&Stg,NULL,NULL,NULL,NULL,NULL,NULL);


    if (pPl->pDevelop->fStageSUCROS>=(float)0.0)
      pPl->pDevelop->iDayAftEmerg++;
      
    if (pPl->pDevelop->fStageSUCROS>=(float)1.0)  
      pPl->pDevelop->bMaturity=pPl->pDevelop->bMaturity;
                          
    if (pPl->pDevelop->fStageSUCROS>=(float)2.0)
      {
      pPl->pDevelop->bMaturity=TRUE;
      SetAllPlantRateToZero(pPl,pSo);
      }
    }    
      

  //Plant variable initialization
  if (pPl->pDevelop->iDayAftEmerg==1) 
    {//Initialization at emergence
    GetDataFromExpert_N(exp_p,pCrop,&Stg,&zCan,&zGrw,NULL,&zPltN,&zRt,&zSoil);

    Initialization(pCrop,1,&zSowInfo,&Stg,&zGrw,&zCan,&zRt,&zPltNc,&zSoil);
  
    zPltN.Leaf.fActNc = zPltNc.fLeaf;
    zPltN.Stem.fActNc = zPltNc.fStem;
    zPltN.Root.fActNc = zPltNc.fRoot;

    SetDataToExpert_N(exp_p,pCrop,&Stg,&zCan,&zGrw,NULL,&zPltN,&zRt,&zSoil);
    }

  DevelopmentCheckAndPostHarvestManagement(exp_p);

    return 1;
  }



//=============================================================================================
// DLL-Function:  dllEWRootSystem_PLAGEN
// Description:    Root system formation module for all crops (Wang,1997)
// Updating Date:    12.09.97                   
//=============================================================================================
int WINAPI  dllEWRootSystem_PLAGEN(EXP_POINTER)
  {
   DECLARE_COMMON_VAR   
   ZROOT Rt; ZSOIL   Soil;
 
  if SimulationStarts  introduce((LPSTR)"dllEWRootSystem_PLAGEN");
  
  if NewDayAndPlantGrowing                               
    { 
    //-------------------------------------------------------------------
    //The NEW code with the ZCROPMOD.DLL
    GetDataFromExpert_N(exp_p,pCrop,NULL,NULL,NULL,NULL,NULL,&Rt,&Soil);

    RootSystemFormation(pCrop,pPl->pBiomass->fRootGrowR,&Rt,&Soil);

    Rt.fDepth       +=Rt.fExtensionRate;   
    pPl->pRoot->fDepth   +=Rt.fExtensionRate;   

    SetDataToExpert_N(exp_p,pCrop,NULL,NULL,NULL,NULL,NULL,&Rt,&Soil);

    //-------------------------------------------------------------------
    //The OLD code without the ZCROPMOD.DLL
    //RootSystemFormation_PLAGEN(exp_p);
    //-------------------------------------------------------------------
      }
  
     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPhotosynthesis_PLAGEN 
// Description:  Daily canopy photosynthesis according to Wang (1997)                                   
// Updating Date:    12.09.97                   
//=============================================================================================
int WINAPI  dllEWPhotosynthesis_PLAGEN(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
   
  if SimulationStarts
    introduce((LPSTR)"dllEWPhotosynthesis_PLAGEN");
  
  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
    if (pPl->pDevelop->fStageSUCROS>(float)0.0)
      {
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
      GetDataFromExpert_N(exp_p,pCrop,NULL,NULL,NULL,NULL,NULL,NULL,NULL);


      pPl->pPltCarbon->fGrossPhotosynR = 
      DailyCanopyGrossPhotosythesis(pCrop,
                      pPl->pCanopy->fLAI, 
                      pLo->pFarm->fLatitude,
                      pTi->pSimTime->iJulianDay,
                           pCl->pWeather->fSolRad,
                           pCl->pWeather->fTempMax,
                           pCl->pWeather->fTempMin,
                           (float)350,
                           pPl->pPltNitrogen->fLeafActConc,
                           NULL);
      //-------------------------------------------------------------------
      //The Old code without the ZCROPMOD.DLL
      //Photosynthesis_PLAGEN(exp_p);
      //-------------------------------------------------------------------
         }
    }                           
                                              
  return 1;
  }




//=============================================================================================
// DLL-Function:  dllEWPotTransp_PLAGEN
// Description:    Daily canopy potential transpiration (Wang,1997)                                   
// Updating Date:    12.09.97                   
//=============================================================================================
int WINAPI  dllEWPotTransp_PLAGEN(EXP_POINTER)
  {
  //extern float CALLBACK _loadds PotentialTranspiration_SPASS(EXP_POINTER); // ep 210599                                    

   DECLARE_COMMON_VAR
 
  if SimulationStarts    introduce((LPSTR)"dllEWPotTransp_PLAGEN");

  if NewDayAndPlantGrowing
    {
    if (pPl->pDevelop->fStageSUCROS>(float)0.0)
      { 
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
      GetDataFromExpert_N(exp_p,pCrop,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

      ///*
      //pPl->pPltWater->fPotTranspdt =
      fPotTraDay=
	  DailyPotentialTranspiration(pCrop,
                    pPl->pCanopy->fLAI,
                    pPl->pDevelop->fStageSUCROS,
                    pLo->pFarm->fLatitude,
                    pLo->pFarm->fAltitude,
                    pTi->pSimTime->iJulianDay,
                         pCl->pWeather->fSolRad,
                         pCl->pWeather->fTempMax,
                         pCl->pWeather->fTempMin,
                         pCl->pWeather->fHumidity,
                         pCl->pWeather->fWindSpeed,
                         (float)350,
                         pPl->pPltNitrogen->fLeafActConc,
                         NULL);
      //*/

      //fPotTraDay=PotentialTranspiration_SPASS(exp_p);

      //-------------------------------------------------------------------
      //The Old code without the ZCROPMOD.DLL
      //PotentialTranspiration_PLAGEN(exp_p);                                     
      //-------------------------------------------------------------------
       }
     }

    if PlantIsGrowing
      {

      float DeltaT;
      DeltaT=pTi->pTimeStep->fAct;

      pPl->pPltWater->fPotTranspdt = fPotTraDay * DeltaT;
      }
  

     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWActTransp_PLAGEN
// Description:    Daily water uptake and water stress factors (CERES and Wang,1997)                                   
// Updating Date:    12.09.97                   
//=============================================================================================
int WINAPI  dllEWActTransp_PLAGEN(EXP_POINTER)
  {
  DECLARE_COMMON_VAR   

  static double vSum;
  //double v,v1,vSin,vSumSin;
  
  ZROOT Rt; ZSOIL Soil;

  if SimulationStarts     introduce((LPSTR)"dllEWActTransp_PLAGEN");

  if NewDayAndPlantGrowing
    {
    int L;
    PLAYERROOT   pLR  =pPl->pRoot->pLayerRoot;

    vSum=0.0;

    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->fStageSUCROS>(float)0.0)
      {
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
      GetDataFromExpert_N(exp_p,pCrop,NULL,NULL,NULL,NULL,NULL,&Rt,&Soil);

      DailyRootWaterUptake(pCrop,pPl->pCanopy->fLAI,fPotTraDay,&Rt,&Soil);
    
      SetDataToExpert_N(exp_p,NULL,NULL,NULL,NULL,NULL,NULL,&Rt,&Soil);

      //-------------------------------------------------------------------
        // If the time step is smaller than one day, the water uptake is 
      // calculated from daily Wateruptake
      // 
      //-------------------------------------------------------------------
    
          for (L=1;L<=pSo->iLayers-2;L++)
            {
          afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
              pLR =pLR ->pNext;
          }

      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
      //ActualTranspiration_PLAGEN(exp_p);
      //PlantWaterStress_PLAGEN(exp_p);
      //-------------------------------------------------------------------
      }                                                       
    }

  if PlantIsGrowing
      {
    //-------------------------------------------------------------------------------------
    //If the time step is smaller than one day, the water uptake is calculated as follows:
    //During the night no water uptake occurs   (0.8-0.3)
    //During the daytime, water uptake follows a sine curve  (0.3-0.8)
///*
        float DeltaZ;
            int L;
        PSLAYER    pSL  =pSo->pSLayer->pNext;
        PWLAYER    pSLW=pWa->pWLayer->pNext; 
        PLAYERROOT   pLR  =pPl->pRoot->pLayerRoot;
        DeltaZ=pSo->fDeltaZ;

          for (L=1;L<=pSo->iLayers-2;L++)
            {
          pSLW->fContAct -= afActLayRootWatUpt[L]
                          /DeltaZ
//                          /pSL->fThickness
                          *pTi->pTimeStep->fAct; //mm/mm
              pSL =pSL ->pNext;
              pSLW=pSLW->pNext;
              pLR =pLR ->pNext;
          }
//*/
/*
    v1=0; 
    for (v=max(0.30001,(double)DeltaT);v<0.8;v=v+DeltaT) 
      v1++;
    v1=max(v1,1.0);
    
    vSumSin=0.0;
    for (v=1; v<=v1; v++)
      vSumSin += sin(3.1415926*(2*v-1)/(2.0*v1));
    
    v=(double)(SimTime-(int)SimTime);
    
    if ((v>0.300001)&&(v<0.8)||(v1==1.0)&&(vSum==0.0))
      {
      vSum++;
      vSin = sin(3.1415926*(2*vSum-1)/(2.0*v1));
      
      if PlantIsGrowing
          {
        int L;
        PSLAYER    pSL  =pSo->pSLayer->pNext;
        PWLAYER    pSLW=pWa->pWLayer->pNext; 
        PLAYERROOT   pLR  =pPl->pRoot->pLayerRoot;
    
          for (L=1;L<=pSo->iLayers-2;L++)
            {
//          pSLW->fContAct -= pLR->fActLayWatUpt*(float)(vSin/vSumSin)/pSL->fThickness; //mm/mm
          pSLW->fContAct -= afActLayRootWatUpt[L]*(float)(vSin/vSumSin)/pSL->fThickness; //mm/mm
              pSL =pSL ->pNext;
              pSLW=pSLW->pNext;
              pLR =pLR ->pNext;
          }
      }    }  
*/    
    }

     return 1;
  }  



//=============================================================================================
// Description:    Maitenance and biomass growth (Wang,1997)                                   
//=============================================================================================
int WINAPI  dllEWBiomassGrowth_PLAGEN(EXP_POINTER)
{
  DECLARE_COMMON_VAR 
  ZDVS   Stg;
  ZBIOM  Bm;
  ZTRP   Trp;
  ZPLTN  PltN; 
  ZCAN   Can; //SG: wegen PlantAccumulationForExpert_N aufgenommen!

  //SG: Faktor für Wasserstress:
  float Wasserfaktor;

  if SimulationStarts
  {
    introduce((LPSTR)"dllEWBiomassGrowth_PLAGEN");
  }

  if NewDayAndPlantGrowing
  {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->fStageSUCROS>(float)0.0)
    {
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
      ZBIOMASS Biom;
      ZPLTNC   PltNc;
      float    fSeedReserv;
            
      GetDataFromExpert_N(exp_p,pCrop,&Stg,&Can,&Bm,&Trp,&PltN,NULL,NULL);
            
      GetOrganBiomass(&Biom,&Bm);
      GetOrganNitrogenConcentration(&PltNc,&PltN);

      if CROP_IS_(POTATO)
      {
        fSeedReserv = (float)1.5*(float)10.0*pMa->pSowInfo->fPlantDens;
        
        if (pPl->pBiomass->fSeedReserv>fSeedReserv)
          pPl->pBiomass->fSeedReserv -= fSeedReserv;
        else
          fSeedReserv = (float)0.0;
      }
      else
        fSeedReserv = (float)0.0;
        
  
    /*SG/01/04/99/////////////////////////////////////////////////////////////////
  
    Um Schwierigkeiten bei der Übergabe der Photosysntheserate am ersten Tag mit 
    pPl->pDevelop->fStageSUCROS >0 zu vermeiden (Transpirationsraten sind dann
    nämlich noch nicht berechnet) wird ein Wasserstressfaktor eingefügt:

    /////////////////////////////////////////////////////////////////////////////*/

    if (Trp.fPotTransp == (float)0.0)
      Wasserfaktor = (float)1.0;
    else
      Wasserfaktor = Trp.fActTransp/Trp.fPotTransp;


    BiomassGrowth(
      pCrop,
      pPl->pDevelop->fStageSUCROS,
      pPl->pDevelop->fStageXCERES,
      pPl->pPltCarbon->fGrossPhotosynR*Wasserfaktor+fSeedReserv,
      (float)0.5*(pCl->pWeather->fTempMax+pCl->pWeather->fTempMin),
      &Biom,
      &Trp,
      &PltNc,
      &Bm,
      NULL);


    //SG/07/04/99///////////////////////////////////////////////////////////////////
    //
    // Aktualisierung der Biomassen nach vorne verlegt (von "dllEWNitrogenUptake_PLAGEN")!
    // (SetDataToExpert_N muss dann natürlich auch vorverlegt werden.
    ////////////////////////////////////////////////////////////////////////////////
    
    SetDataToExpert_N(exp_p,NULL,NULL,&Can,&Bm,NULL,NULL,NULL,NULL);

    PlantAccumulationForExpert_N(pPl);

    //-------------------------------------------------------------------
    //The old code without the ZCROPMOD.DLL
    //CropMaintenance_PLAGEN(exp_p);
    //BiomassGrowth_PLAGEN(exp_p);
    //-------------------------------------------------------------------
    }
  }

  return 1;
}


//=============================================================================================
// DLL-Function:  dllEWCanopyForm_PLAGEN
// Description:    Canopy formation module for cereals (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWCanopyForm_PLAGEN(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  ZDVS   Stg;
  ZTRP  Trp;
  ZPLTN  PltN;
  ZCAN  Can;
   
  if SimulationStarts
       introduce((LPSTR)"dllEWCanopyForm_PLAGEN");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->fStageSUCROS>(float)0.0)
      {
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
            ZPLTNC   PltNc;
            
      GetDataFromExpert_N(exp_p,pCrop,&Stg,&Can,NULL,&Trp,&PltN,NULL,NULL);
            
            GetOrganNitrogenConcentration(&PltNc,&PltN);

      CanopyFormation(pCrop, 
              pPl->pDevelop->fStageSUCROS,
              pPl->pBiomass->fLeafGrowR,
              pPl->pBiomass->fStemGrowR,
                 &Trp,
                 &PltNc,
                 &Can);

      SetDataToExpert_N(exp_p,NULL,NULL,&Can,NULL,NULL,NULL,NULL,NULL);

      //-------------------------------------------------------------------
      //The old code without the ZCROPMOD.DLL
      //if CROP_IS_CEREAL
      //  CanopyFormation_CEREALS(exp_p);
      //if CROP_IS_POTATO
      //  CanopyFormation_POTATO(exp_p);
      //-------------------------------------------------------------------
      }  
    }

  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWOrganSenesz_PLAGEN
// Description:    Organ senescence (Wang,1997)                                   
// Updating Date:    12.11.97                   
//=============================================================================================
int WINAPI  dllEWOrganSenesz_PLAGEN(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  ZDVS   Stg;
  ZTRP  Trp;
  ZPLTN  PltN;
  ZCAN  Can;   
  ZBIOM  Bm;   
  ZROOT  Rt;

  if SimulationStarts introduce((LPSTR)"dllEWOrganSenesz_PLAGEN");

  if NewDayAndPlantGrowing
    {                                        
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->fStageSUCROS>(float)0.0) 
      {
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
            ZPLTNC   PltNc;
            
      GetDataFromExpert_N(exp_p,pCrop,&Stg,&Can,&Bm,&Trp,&PltN,&Rt,NULL); 
            
            GetOrganNitrogenConcentration(&PltNc,&PltN);
         
      Senescence(pCrop,
             pPl->pDevelop->fStageSUCROS,
             &Trp,
             &PltNc,
             &Bm,
             &Can,
             &Rt);


      //Senescence(pCrop,&Stg,&Bm,&Can,&Rt,&Trp,&PltN);
      SetDataToExpert_N(exp_p,pCrop,&Stg,&Can,&Bm,&Trp,&PltN,&Rt,NULL);


      //SG/08/04/99///////////////////////////////////////////////
      //
      // folgende Zeile stammt aus PlantAccumulationForEPERT_N
      // und wurde hierher verlegt, um die Aktualisierung des LAI
      // zum richtigen Zeitpunkt zu gewährleisten.
      ////////////////////////////////////////////////////////////
      pPl->pCanopy->fLAI += pPl->pCanopy->fLAGrowR - pPl->pCanopy->fLeafSenesR;

      //-------------------------------------------------------------------
      //The old code without the ZCROPMOD.DLL
      //OrganSenecence_PLAGEN(exp_p); 
      }
      
    }
    
  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWNitrogenDemand_PLAGEN
// Description:    Plant nitrogen demand module (Wang,1997)                                   
// Updating Date:    12.10.97                   
//=============================================================================================
int WINAPI  dllEWNitrogenDemand_PLAGEN(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  ZBIOM  Bm;   
  ZPLTN  PltN;
   
  if SimulationStarts introduce((LPSTR)"dllEWNitrogenDemand_PLAGEN");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->fStageSUCROS>(float)0.0)
      {      
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
      GetDataFromExpert_N(exp_p,pCrop,NULL,NULL,&Bm,NULL,&PltN,NULL,NULL);
      DailyNitrogenDemand(pCrop, pPl->pDevelop->fStageSUCROS, &Bm, &PltN);
      SetDataToExpert_N(exp_p,pCrop,NULL,NULL,&Bm,NULL,&PltN,NULL,NULL);
      
      //-------------------------------------------------------------------
      //The old code without the ZCROPMOD.DLL
      //NitrogenConcentrationLimits_PLAGEN(exp_p);
      //PlantNitrogenDemand_PLAGEN(exp_p);
      //-------------------------------------------------------------------
      }
    }
  
  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWNitrogenUptake_PLAGEN
// Description:    Plant nitrogen demand module (Wang,1997)                                   
// Updating Date:    12.10.97                   
//=============================================================================================
extern int CALLBACK _loadds PlantNitrogenTranslocation_PLAGEN(EXP_POINTER);

int WINAPI  dllEWNitrogenUptake_PLAGEN(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  ZDVS   Stg;
  ZTRP  Trp;
  ZPLTN  PltN;
  ZCAN  Can;   
  ZBIOM  Bm;   
  ZROOT  Rt;
  ZSOIL   Soil;
  
  if SimulationStarts  introduce((LPSTR)"dllEWNitrogenUptake_PLAGEN");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
    if (pPl->pDevelop->fStageSUCROS>(float)0.0)
      {
      //-------------------------------------------------------------------
      //The NEW code with the ZCROPMOD.DLL
            float fUptake=(float)0;
      GetDataFromExpert_N(exp_p,pCrop,&Stg,&Can,&Bm,&Trp,&PltN,&Rt,&Soil);
      fUptake = DailyNitrogenUptake(pCrop,pPl->pPltNitrogen->fTotalDemand,&Rt,&Soil);
      DailyNitrogenTranslocation(pCrop,pPl->pDevelop->fStageSUCROS, fUptake,&Bm,&PltN);  
      SetDataToExpert_N(exp_p,pCrop,&Stg,&Can,&Bm,&Trp,&PltN,&Rt,&Soil);
  
      //SG/07/04/99///////////////////////////////////////////////////////////////////
      //
      // Aktualisierung der Biomassen nach vorne ("dllEWBiomassGrowth_PLAGEN")verlegt!
      ////////////////////////////////////////////////////////////////////////////////
      
      // PlantAccumulationForExpert_N(pPl);

      //-------------------------------------------------------------------
      //The Old code without the ZCROPMOD.DLL
      //NitrogenUptake_PLAGEN(exp_p);
      //PlantNitrogenStress_PLAGEN(exp_p);    
      //-------------------------------------------------------------------
      }
    }
    
  if PlantIsGrowing
      {
    int L;
    PLAYERROOT  pLR  =pPl->pRoot->pLayerRoot;
    PCLAYER   pSLN=pCh->pCLayer->pNext;
  
    for (L=1;L<=pSo->iLayers-2;L++)
      {
        //Check the whether there are roots in this layer:
        if (pLR->fLengthDens==(float)0.0)    break;

      //Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
      pSLN->fNO3N=max((float)0.0, pSLN->fNO3N-pLR->fActLayNO3NUpt*pTi->pTimeStep->fAct);
      pSLN->fNH4N=max((float)0.0, pSLN->fNH4N-pLR->fActLayNH4NUpt*pTi->pTimeStep->fAct);

      pLR =pLR ->pNext;
      pSLN=pSLN->pNext;
      }
      }
      
  return 1;
  }


//#define EXP_POINTER PCHEMISTRY pCh,PCLIMATE pCl,PGRAPHIC pGr,PHEAT pHe,PLOCATION pLo,PMANAGEMENT pMa,PPLANT pPl,PSPROFILE pSo,PTIME pTi,PWATER pWa 


//=============================================================================================
// Internal funciton:  GetDataFromExpert_N
// Description:      Get the data from ExpertN system variables for the ZCROPMOD.DLL
// Updating Date:      12.10.97                   
//=============================================================================================
void GetDataFromExpert_N(EXP_POINTER,LPSTR pCrop,PZDVS pStage, PZCAN pCan,PZBIOM pBm,
             PZTRP pTransp,PZPLTN pPltN,PZROOT pRt,PZSOIL pSoil)
               
  {  
  int L;
  //--------------------------------------------------------------------------------------
  //Predefined Crop Type
  //--------------------------------------------------------------------------------------
  if (pCrop!=NULL)
    {
    lstrcpy(pCrop,"Unkown");
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH")) lstrcpy(pCrop,WHEAT);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA")) lstrcpy(pCrop,BARLEY);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ")) lstrcpy(pCrop,MAIZE);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI")) lstrcpy(pCrop,MILLET);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG")) lstrcpy(pCrop,SORGHUM);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT")) lstrcpy(pCrop,POTATO);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB")) lstrcpy(pCrop,SUGARBEET);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SY")) lstrcpy(pCrop,SOYBEAN);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RI")) lstrcpy(pCrop,RICE);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"CO")) lstrcpy(pCrop,COTTON);
    if (!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RP")) lstrcpy(pCrop,"RAPE");
    }

  //--------------------------------------------------------------------------------------
  //Development Stage
  //--------------------------------------------------------------------------------------
  if (pStage!=NULL)
    {   
    pStage->fStageVR  = pPl->pDevelop->fStageSUCROS;  //The internal VR stage value (-1.0 - 2.0)
    pStage->fStageEC  = pPl->pDevelop->fDevStage;     //The EC stage value (0-10) as output
    pStage->fStageWE  = pPl->pDevelop->fStageWang;    //The WE stage value (0.0-10.0)
    pStage->fStageAX  = pPl->pDevelop->fStageXCERES;  //The auxiliary stage value
    pStage->fCumPVDs  = pPl->pDevelop->fCumVernUnit;   //Cumulative vernalization days
    pStage->fDevRate  = pPl->pDevelop->fDevR;
    pStage->fThermEff  = pPl->pDevelop->fThermEffect;
    pStage->fPhotopEff  = pPl->pDevelop->fDaylengthEff;
    pStage->fVernEff  = pPl->pDevelop->fVernEff;
    }

  //--------------------------------------------------------------------------------------
  //Canopy
  //--------------------------------------------------------------------------------------
  if (pCan!=NULL)
    {
    pCan->fLAI    =pPl->pCanopy->fLAI;
    pCan->fGrwLAI  =pPl->pCanopy->fLAGrowR;
    pCan->fDieLAI  =pPl->pCanopy->fLeafSenesR;
    }
    
  //--------------------------------------------------------------------------------------
  //Biomass
  //--------------------------------------------------------------------------------------
  if (pBm!=NULL)
    {    
    pBm->fBmLeaf    = pPl->pBiomass->fLeafWeight;
    pBm->fBmStem    = pPl->pBiomass->fStemWeight;
    pBm->fBmRoot    = pPl->pBiomass->fRootWeight;
    pBm->fBmStorage    = pPl->pBiomass->fGrainWeight;
    pBm->fBmReserve    = pPl->pBiomass->fStemReserveWeight;
    pBm->fBmDeadLeaf  = pPl->pBiomass->fDeadLeafWeight;
    pBm->fBmDeadStem  = pPl->pBiomass->fDeadStemWeight;
    pBm->fBmDeadRoot  = pPl->pBiomass->fDeadRootWeight;
    pBm->fGrwLeaf    = pPl->pBiomass->fLeafGrowR;
    pBm->fGrwStem    = pPl->pBiomass->fStemGrowR;
    pBm->fGrwRoot    = pPl->pBiomass->fRootGrowR;
    pBm->fGrwStorage  = pPl->pBiomass->fGrainGrowR;

    pBm->fGrwReserve  = pPl->pBiomass->fStemReserveGrowRate;

    pBm->fDieLeaf    = pPl->pBiomass->fLeafDeathRate;
    pBm->fDieStem    = pPl->pBiomass->fStemDeathRate;
    pBm->fDieRoot    = pPl->pBiomass->fRootDeathRate;
    pBm->fDieStorage  = (float)0.0;
//    pBm->fAssPrtLf    = pPl->pBiomass->fPartFracLeaf;
//    pBm->fAssPrtSt    = pPl->pBiomass->fPartFracStem;
//    pBm->fAssPrtRt    = pPl->pBiomass->fPartFracRoot;
//    pBm->fAssPrtSo    = pPl->pBiomass->fPartFracGrain;
        }
       
  //--------------------------------------------------------------------------------------
  //Transpiration
  //--------------------------------------------------------------------------------------
  if (pTransp!=NULL)
    {
    //pTransp->fPotTransp = pPl->pPltWater->fPotTranspdt;
    pTransp->fPotTransp = fPotTraDay;
    pTransp->fActTransp = pPl->pPltWater->fActTranspdt;
    }    
       
  //--------------------------------------------------------------------------------------
  //Plant nitrogen information
  //--------------------------------------------------------------------------------------
  if (pPltN!=NULL)
    {
    pPltN->Leaf.fMaxNc  = pPl->pPltNitrogen->fLeafMaxConc;
    pPltN->Leaf.fOptNc  = pPl->pPltNitrogen->fLeafMaxConc*(float)0.9;
    pPltN->Leaf.fMinNc  = pPl->pPltNitrogen->fLeafMinConc;
    pPltN->Leaf.fActNc  = pPl->pPltNitrogen->fLeafActConc;
    pPltN->Leaf.fDemand  = pPl->pPltNitrogen->fLeafDemand;

    pPltN->Stem.fMaxNc  = pPl->pPltNitrogen->fStemMaxConc;
    pPltN->Stem.fOptNc  = pPl->pPltNitrogen->fStemMaxConc*(float)0.9;
    pPltN->Stem.fMinNc  = pPl->pPltNitrogen->fStemMinConc;
    pPltN->Stem.fActNc  = pPl->pPltNitrogen->fStemActConc;
    pPltN->Stem.fDemand  = pPl->pPltNitrogen->fStemDemand;

//    pPltN->Storage.fMaxNc  = 
//    pPltN->Storage.fOptNc  = 
//    pPltN->Storage.fMinNc  = 
    pPltN->Storage.fActNc  = pPl->pPltNitrogen->fGrainConc;
    pPltN->Storage.fDemand  = pPl->pPltNitrogen->fGrainDemand;

    pPltN->Root.fMaxNc  = pPl->pPltNitrogen->fRootMaxConc; 
    pPltN->Root.fOptNc  = pPl->pPltNitrogen->fRootMaxConc*(float)0.9;
    pPltN->Root.fMinNc  = pPl->pPltNitrogen->fRootMinConc;
    pPltN->Root.fActNc  = pPl->pPltNitrogen->fRootActConc;
    pPltN->Root.fDemand  = pPl->pPltNitrogen->fRootDemand;

    pPltN->fDemand     = pPl->pPltNitrogen->fTotalDemand;
    }  
      
  //--------------------------------------------------------------------------------------
  //Root Information
  //--------------------------------------------------------------------------------------
  if (pRt!=NULL)
    { 
    PLAYERROOT  pLayRoot;
    
    pRt->fDepth        =pPl->pRoot->fDepth;//*(float)100;   
    
    pRt->fExtensionRate    =pPl->pRoot->fDepthGrowR;
    pRt->fMaxDepth      =pPl->pRoot->fMaxDepth;

    pRt->fWaterUptake    =pPl->pPltWater->fActTranspdt;
    pRt->fNitrogenUptake  =pPl->pPltNitrogen->fActNUptR;

    pRt->fNO3Uptake      =pPl->pPltNitrogen->fActNO3NUpt;
    pRt->fNH4Uptake      =pPl->pPltNitrogen->fActNH4NUpt;

    pLayRoot = pPl->pRoot->pLayerRoot;
    for (L=1;L<=pSo->iLayers-2;L++)
      {
      pRt->fLengthDensity[L]  =pLayRoot->fLengthDens;
      pRt->fGrwLength[L]    =pLayRoot->fLengthDensR;
      pRt->fDieLength[L]    =(float)0.0;

      pRt->fLayerWaterUptake[L]  =pLayRoot->fActLayWatUpt;
      pRt->fLayerNitrogenUptake[L]=pLayRoot->fActLayNUpt;
      pRt->fLayerNO3Uptake[L]    =pLayRoot->fActLayNO3NUpt;
      pRt->fLayerNH4Uptake[L]    =pLayRoot->fActLayNH4NUpt;
      
      pLayRoot = pLayRoot->pNext;
      }


    for (L=pSo->iLayers-2+1;L<MAXSCHICHT;L++)
      {
      pRt->fLengthDensity[L]  =(float)0.0;
      pRt->fGrwLength[L]    =(float)0.0;
      pRt->fDieLength[L]    =(float)0.0;

      pRt->fLayerWaterUptake[L]  =(float)0.0;
      pRt->fLayerNitrogenUptake[L]=(float)0.0;
      pRt->fLayerNO3Uptake[L]    =(float)0.0;
      pRt->fLayerNH4Uptake[L]    =(float)0.0;
      }

    L=0;
      {
      pRt->fLengthDensity[L]  =(float)0.0;
      pRt->fGrwLength[L]    =(float)0.0;
      pRt->fDieLength[L]    =(float)0.0;

      pRt->fLayerWaterUptake[L]  =(float)0.0;
      pRt->fLayerNitrogenUptake[L]=(float)0.0;
      pRt->fLayerNO3Uptake[L]    =(float)0.0;
      pRt->fLayerNH4Uptake[L]    =(float)0.0;
      }
    }
                      
  //--------------------------------------------------------------------------------------
  //Soil Information
  //--------------------------------------------------------------------------------------
  if (pSoil!=NULL)
    {    
    PSLAYER   pSL    =pSo->pSLayer->pNext;
    PSWATER    pSWL  =pSo->pSWater->pNext;
    PWLAYER    pSLW  =pWa->pWLayer->pNext; 
         PCLAYER      pSLN  =pCh->pCLayer->pNext;
        PHLAYER    pSLH  =pHe->pHLayer->pNext;

    for (L=1;L<=pSo->iLayers-2;L++)
      {
      pSoil->sType[L].fThickness    =pSL->fThickness/(float)10.0;
      pSoil->sType[L].fBulkDensity  =pSL->fBulkDens;
      pSoil->sType[L].fpH        =pSL->fpH;

      pSoil->sType[L].fSatWaterContent=pSWL->fContSat;
      pSoil->sType[L].fFieldCapacity  =pSWL->fContFK;
      pSoil->sType[L].fWiltingPoint  =pSWL->fContPWP;

      pSoil->fTemperature[L]      =pSLH->fSoilTemp;
      pSoil->fWaterContent[L]      =pSLW->fContAct;
      pSoil->fNO3N[L]          =pSLN->fNO3N;
      pSoil->fNH4N[L]          =pSLN->fNH4N;
      pSoil->fNitrogenContent[L]    =pSLN->fNO3N+pSLN->fNH4N;

      pSL  = pSL->pNext;
      pSWL = pSWL->pNext;
      pSLW = pSLW->pNext;
      pSLN = pSLN->pNext;
      pSLH = pSLH->pNext;
      }

    for (L=pSo->iLayers-2+1;L<MAXSCHICHT;L++)
      {
      pSoil->sType[L].fThickness    =(float)0.0;
      pSoil->sType[L].fBulkDensity  =(float)0.0;
      pSoil->sType[L].fpH        =(float)0.0;

      pSoil->sType[L].fSatWaterContent=(float)0.0;
      pSoil->sType[L].fFieldCapacity  =(float)0.0;
      pSoil->sType[L].fWiltingPoint  =(float)0.0;

      pSoil->fTemperature[L]      =(float)0.0;
      pSoil->fWaterContent[L]      =(float)0.0;
      pSoil->fNO3N[L]          =(float)0.0;
      pSoil->fNH4N[L]          =(float)0.0;
      pSoil->fNitrogenContent[L]    =(float)0.0;
      }
    
    L=0;      
      {
      pSoil->sType[L].fThickness    =(float)0.0;
      pSoil->sType[L].fBulkDensity  =(float)0.0;
      pSoil->sType[L].fpH        =(float)0.0;

      pSoil->sType[L].fSatWaterContent=(float)0.0;
      pSoil->sType[L].fFieldCapacity  =(float)0.0;
      pSoil->sType[L].fWiltingPoint  =(float)0.0;

      pSoil->fTemperature[L]      =(float)0.0;
      pSoil->fWaterContent[L]      =(float)0.0;
      pSoil->fNO3N[L]          =(float)0.0;
      pSoil->fNH4N[L]          =(float)0.0;
      pSoil->fNitrogenContent[L]    =(float)0.0;
      }
    }
    
   return;
  }


//=============================================================================================
// Internal funciton:  SetDataToExpert_N
// Description:      Set the data to ExpertN system variables for the ZCROPMOD.DLL
// Updating Date:      12.10.97                   
//=============================================================================================
void SetDataToExpert_N(EXP_POINTER,LPSTR pCrop,PZDVS pStage, PZCAN pCan,PZBIOM pBm,
               PZTRP pTransp,PZPLTN pPltN,PZROOT pRt,PZSOIL pSoil)
               
  { 
  int L;
  //--------------------------------------------------------------------------------------
  //Development Stage
  //--------------------------------------------------------------------------------------
  if (pStage!=NULL)
    {   
    pPl->pDevelop->fStageSUCROS    =pStage->fStageVR;  //The internal VR stage value (-1.0 - 2.0)
    pPl->pDevelop->fDevStage    =pStage->fStageEC;   //The EC stage value (0-10) as output
    pPl->pDevelop->fStageWang    =pStage->fStageWE;   //The WE stage value (0.0-10.0)
    pPl->pDevelop->fStageXCERES    =pStage->fStageAX;
    pPl->pDevelop->fCumVernUnit    =pStage->fCumPVDs;   //Cumulative vernalization days
    pPl->pDevelop->fDevR      =pStage->fDevRate;
    pPl->pDevelop->fThermEffect        =pStage->fThermEff;
    pPl->pDevelop->fDaylengthEff  =pStage->fPhotopEff;
    pPl->pDevelop->fVernEff      =pStage->fVernEff;
    }
  //--------------------------------------------------------------------------------------
  //Canopy
  //--------------------------------------------------------------------------------------
  if (pCan!=NULL)
    {
    pPl->pCanopy->fLAI      = pCan->fLAI;
    pPl->pCanopy->fLAGrowR    = pCan->fGrwLAI;
    pPl->pCanopy->fLeafSenesR  = pCan->fDieLAI;
    }

  //--------------------------------------------------------------------------------------
  //Biomass
  //--------------------------------------------------------------------------------------
  if (pBm!=NULL)
    {    
    pPl->pBiomass->fLeafWeight  =pBm->fBmLeaf;
    pPl->pBiomass->fStemWeight  =pBm->fBmStem;
    pPl->pBiomass->fRootWeight  =pBm->fBmRoot;
    pPl->pBiomass->fGrainWeight  =pBm->fBmStorage;
    pPl->pBiomass->fStemReserveWeight  =pBm->fBmReserve;
    pPl->pBiomass->fDeadLeafWeight    =pBm->fBmDeadLeaf;
    pPl->pBiomass->fDeadStemWeight    =pBm->fBmDeadStem;
    pPl->pBiomass->fDeadRootWeight    =pBm->fBmDeadRoot;
    pPl->pBiomass->fLeafGrowR  =pBm->fGrwLeaf;
    pPl->pBiomass->fStemGrowR  =pBm->fGrwStem;
    pPl->pBiomass->fRootGrowR  =pBm->fGrwRoot;
    pPl->pBiomass->fGrainGrowR  =pBm->fGrwStorage;

    pPl->pBiomass->fStemReserveGrowRate  =pBm->fGrwReserve;

    pPl->pBiomass->fLeafDeathRate    =pBm->fDieLeaf;
    pPl->pBiomass->fStemDeathRate    =pBm->fDieStem;
    pPl->pBiomass->fRootDeathRate    =pBm->fDieRoot;

//    pPl->pBiomass->fPartFracLeaf=pBm->fAssPrtLf;
//    pPl->pBiomass->fPartFracStem=pBm->fAssPrtSt;
//    pPl->pBiomass->fPartFracRoot=pBm->fAssPrtRt;
//    pPl->pBiomass->fPartFracGrain=pBm->fAssPrtSo;
//    pBm->fDieStem    = (float)0.0;
//    pBm->fDieStorage  = (float)0.0;
        }
       
  //--------------------------------------------------------------------------------------
  //Transpiration
  //--------------------------------------------------------------------------------------
  if (pTransp!=NULL)
    {
  //  pPl->pPltWater->fPotTranspdt = pTransp->fPotTransp;
  //  pPl->pPltWater->fActTranspdt = pTransp->fActTransp;
    pPl->pPltWater->fPotTranspDay = pTransp->fPotTransp;
    pPl->pPltWater->fActTranspDay = pTransp->fActTransp;
    }    

  //--------------------------------------------------------------------------------------
  //Plant nitrogen information
  //--------------------------------------------------------------------------------------
  if (pPltN!=NULL)
    {
//    pPl->pPltNitrogen->fLeafNC   = pPltN->Leaf.fActNc;
//    pPl->pPltNitrogen->fStenNC  = pPltN->Stem.fActNc;
    
    pPl->pPltNitrogen->fGrainConc   = pPltN->Storage.fActNc;
    pPl->pPltNitrogen->fRootActConc = pPltN->Root.fActNc;


    pPl->pPltNitrogen->fLeafMaxConc = pPltN->Leaf.fMaxNc;
    pPl->pPltNitrogen->fLeafMinConc = pPltN->Leaf.fMinNc;
    pPl->pPltNitrogen->fLeafActConc    = pPltN->Leaf.fActNc;
    pPl->pPltNitrogen->fLeafDemand  = pPltN->Leaf.fDemand;

    pPl->pPltNitrogen->fStemMaxConc = pPltN->Stem.fMaxNc;
    pPl->pPltNitrogen->fStemMinConc = pPltN->Stem.fMinNc;
    pPl->pPltNitrogen->fStemActConc    = pPltN->Stem.fActNc;
    pPl->pPltNitrogen->fStemDemand  = pPltN->Stem.fDemand;

    pPl->pPltNitrogen->fGrainConc = pPltN->Storage.fActNc;
    pPl->pPltNitrogen->fGrainDemand = pPltN->Storage.fDemand;

    pPl->pPltNitrogen->fRootMaxConc = pPltN->Root.fMaxNc; 
    pPl->pPltNitrogen->fRootMinConc = pPltN->Root.fMinNc;
    pPl->pPltNitrogen->fRootActConc = pPltN->Root.fActNc;
    pPl->pPltNitrogen->fRootDemand = pPltN->Root.fDemand;

    pPl->pPltNitrogen->fRootDemand = pPl->pPltNitrogen->fRootDemand;
    pPl->pPltNitrogen->fShootDemand= pPl->pPltNitrogen->fLeafDemand+pPl->pPltNitrogen->fStemDemand;
    pPl->pPltNitrogen->fTotalDemand = pPltN->fDemand;
    }  
  //--------------------------------------------------------------------------------------
  //Root Information
  //--------------------------------------------------------------------------------------
  if (pRt!=NULL)
    { 
    PLAYERROOT  pLayRoot;
    
    pPl->pRoot->fDepth    = pRt->fDepth;///(float)100;
    pPl->pRoot->fDepthGrowR  = pRt->fExtensionRate;

    pPl->pPltWater->fActTranspdt  =pRt->fWaterUptake;    
    pPl->pRoot->fUptakeR       =pRt->fWaterUptake;
    
    pPl->pPltNitrogen->fActNUptR  =pRt->fNitrogenUptake;
    pPl->pPltNitrogen->fActNUpt    =pRt->fNitrogenUptake;
    pPl->pPltNitrogen->fActNO3NUpt  =pRt->fNO3Uptake;
    pPl->pPltNitrogen->fActNH4NUpt  =pRt->fNH4Uptake;
    
    pLayRoot = pPl->pRoot->pLayerRoot;
    for (L=1;L<=pSo->iLayers-2;L++)
      {
      pLayRoot->fLengthDens  =pRt->fLengthDensity[L];
      pLayRoot->fLengthDensR  =pRt->fGrwLength[L];
//      pRt->fDieLength[L]    =(float)0.0;

      pLayRoot->fActLayWatUpt  =pRt->fLayerWaterUptake[L];
      pLayRoot->fActLayNO3NUpt=pRt->fLayerNO3Uptake[L];
      pLayRoot->fActLayNH4NUpt=pRt->fLayerNH4Uptake[L];
      pLayRoot->fActLayNUpt  =pRt->fLayerNitrogenUptake[L];
      
      pLayRoot = pLayRoot->pNext;
      } 
    }

  //--------------------------------------------------------------------------------------
  //Soil Information
  //--------------------------------------------------------------------------------------
  if (pSoil!=NULL)
    {    
    //Do nothing
    }

  return;
  }
  

//=============================================================================================
// Internal funciton:  PlantAccumulationForExpert_N
// Description:      The ZCROPMOD.DLL does not accumulate the following variables 
// Updating Date:      12.10.97                   
//=============================================================================================
void PlantAccumulationForExpert_N(PPLANT pPl)
  {
  PBIOMASS pBiom = pPl->pBiomass;
  float fReserveTransRate = max((float)0.0, -pPl->pBiomass->fStemReserveGrowRate);
      

  //Canopy - LAI  
  
    //SG/08/04/99////////////////////////////////////////////////
  // folgende Zeile wurde nach "dllEWOrganSenesz" verlegt
  //
  // pPl->pCanopy->fLAI += pPl->pCanopy->fLAGrowR - pPl->pCanopy->fLeafSenesR;

  //Dry weight of different organs                            
  pBiom->fLeafWeight     += pBiom->fLeafGrowR - pPl->pBiomass->fLeafDeathRate; 
  pBiom->fStemWeight    += pBiom->fStemGrowR-fReserveTransRate-pPl->pBiomass->fStemDeathRate;
  pBiom->fRootWeight    += pBiom->fRootGrowR - pPl->pBiomass->fRootDeathRate;
  pBiom->fGrainWeight   += pBiom->fGrainGrowR;
  pBiom->fFruitWeight   += pBiom->fGrainGrowR;
        
  pPl->pBiomass->fStemReserveWeight =  max((float)0.0,pPl->pBiomass->fStemReserveWeight);
        
  pBiom->fBiomassAbvGround =pBiom->fLeafWeight+pBiom->fStemWeight+pBiom->fGrainWeight;
  pBiom->fBiomGrowR=pBiom->fLeafGrowR+pBiom->fRootGrowR+pBiom->fStemGrowR+pBiom->fGrainGrowR;
        
  pBiom->fStovWeight   = pBiom->fLeafWeight+pBiom->fStemWeight;
  pBiom->fTotalBiomass = pBiom->fStovWeight + pBiom->fGrainWeight;
  
  pPl->pBiomass->fDeadLeafWeight += pPl->pBiomass->fLeafDeathRate;
  pPl->pBiomass->fDeadRootWeight += pPl->pBiomass->fRootDeathRate;
  pPl->pBiomass->fDeadStemWeight += pPl->pBiomass->fStemDeathRate;;
    
  pPl->pBiomass->fTotLeafWeight   = pPl->pBiomass->fLeafWeight+pPl->pBiomass->fDeadLeafWeight; 
  pPl->pBiomass->fTotRootWeight    = pPl->pBiomass->fRootWeight+pPl->pBiomass->fDeadRootWeight;
  pPl->pBiomass->fTotStemWeight    = pPl->pBiomass->fStemWeight+pPl->pBiomass->fDeadStemWeight;

  return;
  }  


//=============================================================================================
// Internal funciton:  SetAllPlantRateToZero
// Description:      If plant matures all rates are set to zero
// Updating Date:      12.10.97                   
//=============================================================================================
void SetAllPlantRateToZero(PPLANT pPl, PSPROFILE pSo)
  {        
  int L;
  PLAYERROOT  pLayRoot;
  //--------------------------------------------------------------------------------------
  //Development Stage
  //--------------------------------------------------------------------------------------
    pPl->pDevelop->fDevR    =(float)0.0;

  //--------------------------------------------------------------------------------------
  //Canopy
  //--------------------------------------------------------------------------------------
    pPl->pCanopy->fLAGrowR    =(float)0.0;
    pPl->pCanopy->fLeafSenesR  =(float)0.0;

  //--------------------------------------------------------------------------------------
  //Biomass
  //--------------------------------------------------------------------------------------
    pPl->pBiomass->fLeafGrowR  =(float)0.0;
    pPl->pBiomass->fStemGrowR  =(float)0.0;
    pPl->pBiomass->fRootGrowR  =(float)0.0;
    pPl->pBiomass->fGrainGrowR  =(float)0.0;

    pPl->pBiomass->fStemReserveGrowRate  =(float)0.0;

    pPl->pBiomass->fLeafDeathRate    =(float)0.0;
    pPl->pBiomass->fRootDeathRate    =(float)0.0;
       
  //--------------------------------------------------------------------------------------
  //Transpiration
  //--------------------------------------------------------------------------------------
    pPl->pPltWater->fPotTranspdt =(float)0.0;
    pPl->pPltWater->fActTranspdt =(float)0.0;

  //--------------------------------------------------------------------------------------
  //Plant water and nitrogen information
  //--------------------------------------------------------------------------------------
    pPl->pPltWater->fActTranspdt  =(float)0.0;
    pPl->pRoot->fUptakeR       =(float)0.0;
    
    pPl->pPltNitrogen->fActNUptR  =(float)0.0;
    pPl->pPltNitrogen->fActNUpt    =(float)0.0;
    pPl->pPltNitrogen->fActNO3NUpt  =(float)0.0;
    pPl->pPltNitrogen->fActNH4NUpt  =(float)0.0;

  //--------------------------------------------------------------------------------------
  //Root Information
  //--------------------------------------------------------------------------------------
    pPl->pRoot->fDepthGrowR  =(float)0.0;

    pLayRoot = pPl->pRoot->pLayerRoot;
    for (L=1;L<=pSo->iLayers-2;L++)
      {
      pLayRoot->fLengthDensR  =(float)0.0;
//      pRt->fDieLength[L]    =(float)0.0;

      pLayRoot->fActLayWatUpt  =(float)0.0;
      pLayRoot->fActLayNO3NUpt=(float)0.0;
      pLayRoot->fActLayNH4NUpt=(float)0.0;
      pLayRoot->fActLayNUpt  =(float)0.0;
      
      pLayRoot = pLayRoot->pNext;
      } 


  return;
  } 
             
  

int DevelopmentCheckAndPostHarvestManagement(EXP_POINTER)
  {
	/*  	
	if ((pPl != NULL)&&(pMa->pSowInfo != NULL)&&(pTi->pSimTime->fTimeAct == pMa->pSowInfo->iDay)) 
	{	
		//SPASS_Genotype_Read(hModuleInstance);
		PlantVariableInitiation(exp_p);

		pPl->pDevelop->bMaturity = FALSE;
		pPl->pDevelop->bPlantGrowth = TRUE;
	}
    */
    
	if NewDayAndPlantGrowing
    {
    if (pPl->pDevelop->fStageSUCROS>=(float)0.0)
      pPl->pDevelop->iDayAftEmerg++;
      
    if (pPl->pDevelop->fStageSUCROS>=(float)1.0)  
      pPl->pDevelop->bMaturity=pPl->pDevelop->bMaturity;
                          
    if (pPl->pDevelop->fStageSUCROS>=(float)2.0)
      {
      pPl->pDevelop->bMaturity=TRUE;
      SetAllPlantRateToZero(pPl,pSo);
      }
    }    
      

  if (pTi->pSimTime->fTimeAct == (float)(pPl->pModelParam->iHarvestDay-1))
  {  
    float *pMat1Local,factor,amount,actDepth,RootProp,RootSum;
    int    i1,i2;
    
    float fCut,fHeight;
        float fCResidue,fNResidue;
        float fCStandR,fNStandR;
    float fStaFak;
    PSLAYER    pSL  =pSo->pSLayer->pNext;
    PCLAYER pCL;
        PCPROFILE pCP = pCh->pCProfile;

    pPl->pDevelop->bPlantGrowth = FALSE;
    pPl->pDevelop->bMaturity = TRUE;
    i1 = (int)MAXSCHICHT; //aktuelle Bodenschichten
    RootSum = actDepth = (float)0.0;


    pMat1Local        = (float *) _MALLOC(i1 * sizeof(float));
    memset(pMat1Local,0x0,(i1 * sizeof(float)));
    
    // The aboveground biomass at iHarvestDay must be written 
    // to the surfacepools and the rootbiomass to the Litterpools
    /*
    if(pPl->pModelParam->cResidueCarryOff==0)
    {
    pCh->pCProfile->fDryMatterLitterSurf += pPl->pBiomass->fBiomassAbvGround-pPl->pBiomass->fGrainWeight;
    pCh->pCProfile->fNLitterSurf += pCh->pCProfile->fDryMatterLitterSurf * pPl->pPltNitrogen->fVegActConc;//pPl->pPltNitrogen->fCumActNUpt;
    pCh->pCProfile->fCLitterSurf += (pPl->pBiomass->fBiomassAbvGround-pPl->pBiomass->fGrainWeight) * (float)0.4 ; // 40% C in Biomasse
    
    if (pCh->pCProfile->fNLitterSurf > (float)0.0)
    pCh->pCProfile->fCNLitterSurf = pCh->pCProfile->fCLitterSurf /
                          pCh->pCProfile->fNLitterSurf; 
    }
    */

     fCut = (float)5;
     fHeight = (float)20;

        /* Initialisieren des Standing / Verteilungsfaktor */
      if (!lstrcmp(pPl->pGenotype->acCropCode,"WH\0"))        
      { //Weizen und alles andere
     fCut = (float)15;
     fHeight = (float)100;
      }

      if (!lstrcmp(pPl->pGenotype->acCropCode,"BA\0"))        
      { //Gerste
     fCut = (float)15;
     fHeight = (float)100;
       }

      if (!lstrcmp(pPl->pGenotype->acCropCode,"MZ\0"))        
      { //Mais
     fCut = (float)20;
     fHeight = (float)200;
      }

      if (!lstrcmp(pPl->pGenotype->acCropCode,"PT\0"))        
      { //Kartoffel
     fCut = (float)0;
     fHeight = (float)60;
      }

	  if (!lstrcmp(pPl->pGenotype->acCropCode,"BS\0"))        
      { //Zuckerruebe
     fCut = (float)0;
     fHeight = (float)40;
      }

      if (!lstrcmp(pPl->pGenotype->acCropCode,"SF\0"))        
      { //Sonnenblumen
       fCut = (float)150;
     fHeight = (float)200;
      }


        fStaFak = fCut / fHeight; 
 

    // Falls ohne Standing Pool gerechnet werden soll
        if(!iStandingResidues) fStaFak = (float)0;
  


        /* Berechnen stehender und liegender Anteil */
        //fCResidue = (pPl->pBiomass->fBiomassAbvGround - pPl->pBiomass->fGrainWeight) * (float)0.4;
        fCResidue = pPl->pBiomass->fStovWeight * (float)0.4;
        fCStandR  = fStaFak * fCResidue;
        //fNResidue = fCResidue * pPl->pPltNitrogen->fTopsActConc;
        fNResidue = pPl->pBiomass->fStovWeight * pPl->pPltNitrogen->fTopsActConc;
        fNStandR  = fStaFak * fNResidue;

        /* Abfrage ob Zwischenfrucht oder Hauptfrucht im Augenblick (10.07.01)
        nur Ackersenf, Ölrettisch, Winterrübsen, Kleegras und Luzerne als
        Zwischenfrucht definiert */
  
     if   (!lstrcmp(pPl->pGenotype->acCropName,"Ackersenf\0")||
          !lstrcmp(pPl->pGenotype->acCropName,"Kleegras\0")||
          !lstrcmp(pPl->pGenotype->acCropName,"Luzerne\0")||
          !lstrcmp(pPl->pGenotype->acCropName,"Winterrübsen\0")||
          !lstrcmp(pPl->pGenotype->acCropName,"Ölrettich\0"))
     {
       if(pPl->pModelParam->cResidueCarryOff==0)
       {
         if(iStandingResidues)
         {
          //pCP->fCStandCropRes += pPl->pBiomass->fBiomassAbvGround * (float)0.4; 
          //pCP->fNStandCropRes += pPl->pBiomass->fBiomassAbvGround * pPl->pPltNitrogen->fVegActConc;
          pCP->fCStandCropRes += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * (float)0.4; 
          pCP->fNStandCropRes += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * pPl->pPltNitrogen->fVegActConc;
         }
         else
         {
          //pCP->fCLitterSurf += pPl->pBiomass->fBiomassAbvGround * (float)0.4;   
          //pCP->fNLitterSurf += pPl->pBiomass->fBiomassAbvGround * pPl->pPltNitrogen->fVegActConc;
          pCP->fCLitterSurf += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * (float)0.4;   
          pCP->fNLitterSurf += (pPl->pBiomass->fStovWeight+pPl->pBiomass->fGrainWeight) * pPl->pPltNitrogen->fVegActConc;
         } 
       }
    } 
    else if(pPl->pModelParam->cResidueCarryOff==0)
    {
     pCP->fCStandCropRes  += fCStandR; 
     pCP->fCLitterSurf    += (fCResidue - fCStandR)+pPl->pBiomass->fDeadLeafWeight*(float)0.4;
     pCP->fNStandCropRes  += fNStandR; 
     pCP->fNLitterSurf    += (fNResidue - fNStandR)+pPl->pPltNitrogen->fDeadLeafNw;
    }
    else 
    {
     pCP->fCStandCropRes += fCStandR; 
     pCP->fNStandCropRes += fNStandR;
    }

    pMa->pLitter->fRootC = (pPl->pBiomass->fRootWeight+pPl->pBiomass->fDeadRootWeight) * (float)0.4 ; // 40% C in Biomasse


    if (pPl->pPltNitrogen->fRootCont > (float) 0)
    {
     pMa->pLitter->fRootCN = (pPl->pBiomass->fRootWeight+pPl->pBiomass->fDeadRootWeight) * (float)0.4
                                                    /(pPl->pPltNitrogen->fRootCont+pPl->pPltNitrogen->fDeadRootNw);
    }
    else
    {
     pMa->pLitter->fRootCN =(float)0.1;// ep 250399 fRootCN=0.1 overestimates NLitter
    }


     // Berechnen des schichtmaessigen Anteils

  for (pSL = pSo->pSLayer->pNext,
       pCL = pCh->pCLayer->pNext,i2=0;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext,i2++)
    {
      actDepth = actDepth + pSL->fThickness;
      RootProp =  (float)exp((float)-3.0 * actDepth / pSo->fDepth);
      RootSum += RootProp;

      pMat1Local[i2] = RootProp;
    }

    // Anteilsmaessige Verteilung auf die Bodenschichten. 
    for (pSL = pSo->pSLayer->pNext,
       pCL = pCh->pCLayer->pNext,i2=0;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext,i2++)
    {
      factor = pMat1Local[i2] / RootSum;
          amount = pMa->pLitter->fRootC * factor;

      pCL->fCLitter += amount;
      pCL->fNLitter += amount / pMa->pLitter->fRootCN;
      
      if((pCL->fCLitter>EPSILON)&&(pCL->fNLitter>EPSILON))  
        {
        pCL->fLitterCN = pCL->fCLitter / pCL->fNLitter;          
        }             
        else
        pCL->fLitterCN =(float)0.1;

    }

     free(pMat1Local);                                        

     if(pPl->pNext==NULL)
     {
      memset(pPl->pDevelop,0x0,sizeof(STDEVELOP));
      memset(pPl->pBiomass,0x0,sizeof(STBIOMASS));
      memset(pPl->pCanopy,0x0,sizeof(STCANOPY));
      memset(pPl->pPltNitrogen,0x0,sizeof(STPLTNITROGEN));
      memset(pPl->pPltCarbon,0x0,sizeof(STPLTCARBON));
      memset(pPl->pPltWater,0x0,sizeof(STPLTWATER));  
     }


  }


  return 1;
  }







//############################################################################################################
//  THE FOLLOWING FUNCTIONS FROM MODULE - SPASS.C
//############################################################################################################
extern int CALLBACK _loadds PhasicDevelopment_SPASS(EXP_POINTER);
extern int CALLBACK _loadds RootSystemFormation_SPASS(EXP_POINTER);
extern int CALLBACK _loadds Photosynthesis_SPASS(EXP_POINTER);
extern float CALLBACK _loadds PotentialTranspiration_SPASS(EXP_POINTER); // ep 210599                                    
extern float CALLBACK _loadds PotentialTranspiration_SPASSAlt(EXP_POINTER); // ep 210599                                    
extern int CALLBACK _loadds ActualTranspiration_SPASS(EXP_POINTER);
extern int CALLBACK _loadds PlantWaterStress_SPASS(EXP_POINTER);        
extern int CALLBACK _loadds CropMaintenance_SPASS(EXP_POINTER);
extern int CALLBACK _loadds BiomassPartition_SPASS(EXP_POINTER);
extern int CALLBACK _loadds BiomassGrowth_SPASS(EXP_POINTER);
extern int CALLBACK _loadds CanopyFormation_SPASS(EXP_POINTER);
extern int CALLBACK _loadds OrganSenescence_SPASS(EXP_POINTER);
extern int CALLBACK _loadds NitrogenConcentrationLimits_SPASS(EXP_POINTER);
extern int CALLBACK _loadds PlantNitrogenDemand_SPASS(EXP_POINTER);
extern int CALLBACK _loadds NitrogenUptake_SPASS(EXP_POINTER);
extern int CALLBACK _loadds PlantNitrogenStress_SPASS(EXP_POINTER);  

//=============================================================================================
// DLL-Function:  dllEWDevelopment_SPASS
// Description:    Phasic development module for cereal crops (Wang,1997)
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWDevelopment_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
    
  
  if SimulationStarts
         introduce((LPSTR)"dllEWDevelopment_SPASS");
        
  if (pPl==NULL) return 1;

  if NewDayAndPlantGrowing
         PhasicDevelopment_SPASS(exp_p);
  
  DevelopmentCheckAndPostHarvestManagement(exp_p);


    return 1;
  }




//=============================================================================================
// DLL-Function:  dllEWRootSystem_SPASS
// Description:    Root system formation module for all crops (Wang,1997)
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWRootSystem_SPASS(EXP_POINTER)
  {
   DECLARE_COMMON_VAR
 
  if SimulationStarts
    introduce((LPSTR)"dllEWRootSystem_SPASS");
  
  if NewDayAndPlantGrowing
    RootSystemFormation_SPASS(exp_p);

     return 1;
  }



//=============================================================================================
// DLL-Function:  dllEWPhotosynthesis_SPASS 
// Description:  Daily canopy photosynthesis according to Wang (1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWPhotosynthesis_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
   
  if SimulationStarts
    introduce((LPSTR)"dllEWPhotosynthesis_SPASS");
  
  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
      
    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

    if (pPl->pDevelop->fStageSUCROS > (float)0.0 )          */      

		if (pPl->pDevelop->iDayAftEmerg > 0) 
		{
			//SG 20111114:
			//CO2-conc. from input file (*.xnm) for AgMIP-project
			fAtmCO2 = (pCl->pWeather->fSnow > 0? pCl->pWeather->fSnow:fAtmCO2);
	
		    Photosynthesis_SPASS(exp_p);
		}
    }
  
  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPotTransp_SPASS
// Description:    Daily canopy potential transpiration (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
extern float CALLBACK _loadds PotentialTranspiration_SPASS(EXP_POINTER);  // ep 210599

int WINAPI  dllEWPotTransp_SPASS(EXP_POINTER)
  {
   DECLARE_COMMON_VAR
 
  if SimulationStarts
       introduce((LPSTR)"dllEWPotTransp_SPASS");

  if ((int)NewDay(pTi)) fPotTraDay=(float)0;

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 

    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

      if (pPl->pDevelop->fStageSUCROS > (float)0.0 )                 */

    if (pPl->pDevelop->iDayAftEmerg > 0) 
      
      fPotTraDay=PotentialTranspiration_SPASS(exp_p);                                     
     }

    
  if PlantIsGrowing
      {
    //-------------------------------------------------------------------------------------
    //If the time step is smaller than one day, the water uptake is calculated as follows:
    //During the night no water uptake occurs   (0.8-0.3)
    //During the daytime, water uptake follows a sine curve  (0.3-0.8)

      float DeltaT;
      DeltaT=pTi->pTimeStep->fAct;

//      pPl->pPltWater->fPotTranspdt = pWa->fPotETdt - pWa->pEvap->fPotR * DeltaT;
      pPl->pPltWater->fPotTranspdt = fPotTraDay * DeltaT;
      // ep 210599 to avoid "initial value = daily value" in balance.c by using
          // pPl->pPltWater->fPotTranspdt = pPl->pPltWater->fPotTranspDay * DeltaT;
      }

     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPotTransp_SPASS
// Description:    Daily canopy potential transpiration (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
extern float CALLBACK _loadds PotentialTranspiration_SPASSAlt(EXP_POINTER); // ep 210599

int WINAPI  dllEPPotTra_SPASSAlt(EXP_POINTER)
  {
   DECLARE_COMMON_VAR
 
  if SimulationStarts
       introduce((LPSTR)"dllEWPotTransp_SPASS");

  if ((int)NewDay(pTi)) fPotTraDay=(float)0;

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 

    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

      if (pPl->pDevelop->fStageSUCROS > (float)0.0 )                 */

    if (pPl->pDevelop->iDayAftEmerg > 0) 
      
      fPotTraDay=PotentialTranspiration_SPASSAlt(exp_p);                                     
     }

  if PlantIsGrowing
      {
      float DeltaT;
      DeltaT=pTi->pTimeStep->fAct;

      pPl->pPltWater->fPotTranspdt = fPotTraDay * DeltaT;
      // ep 210599 to avoid "initial value = daily value" in balance.c by using
          // pPl->pPltWater->fPotTranspdt = pPl->pPltWater->fPotTranspDay * DeltaT;
      }

     return 1;
  }



 //=============================================================================================
// DLL-Function:  dllEWActTransp_SPASS
// Description:    Daily water uptake and water stress factors (CERES and Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWActTransp_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR   

  if SimulationStarts
    {
    int L;
    introduce((LPSTR)"dllEWActTransp_SPASS");
    for (L=1;L<=pSo->iLayers-2;L++) afActLayRootWatUpt[L]=(float)0;
    }
  
  if ((int)NewDay(pTi)) fActTraDay=(float)0;
  if (((int)NewDay(pTi))&&((pPl!=NULL))) pPl->pRoot->fUptakeR =(float)0.0;
  //ep 05.02.01  not reinitialized after harvest (in capacity water flow module)!

  if NewDayAndPlantGrowing
    {                   
    int L;
    PLAYERROOT   pLR  =pPl->pRoot->pLayerRoot;

    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    
    //SG/19/04/99////////////////////////////////////////////
    //In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    //Entwicklungsstadium!
    //if (pPl->pDevelop->fStageSUCROS > (float)0.0)
    
    if (pPl->pDevelop->iDayAftEmerg > 0)                       
      {
      ActualTranspiration_SPASS(exp_p);
      PlantWaterStress_SPASS(exp_p);
	  }

    //-------------------------------------------------------------------
    // If the time step is smaller than one day, the water uptake is 
    // calculated from daily Wateruptake
    //-------------------------------------------------------------------
    
    for (L=1;L<=pSo->iLayers-2;L++)
      {
       afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
       pLR =pLR ->pNext;
      }

         
    }  //  end if NewDayAndPlantGrowing


    if PlantIsGrowing
    {
    //PTIMESTEP    pTS  =pTi->pTimeStep;
    PSLAYER      pSL  =pSo->pSLayer->pNext;
    PWLAYER      pSLW =pWa->pWLayer->pNext; 
    PLAYERROOT   pLR  =pPl->pRoot->pLayerRoot;

    int L;

    for (L=1;L<=pSo->iLayers-2;L++)
      {
       pSLW->fContAct -= afActLayRootWatUpt[L]
                         /pSo->fDeltaZ
                         *pTi->pTimeStep->fAct; //mm/mm
       pSL =pSL ->pNext;
       pSLW=pSLW->pNext;
       pLR =pLR ->pNext;
      }

    }//end if plant is growing     

    
  return 1;
  }  
  

  
//=============================================================================================
// DLL-Function:  dllEWBiomassGrowth_SPASS
// Description:    Maitenance and biomass growth (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWBiomassGrowth_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  if SimulationStarts
       introduce((LPSTR)"dllEWBiomassGrowth_SPASS");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 

    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

    if (pPl->pDevelop->fStageSUCROS > (float)0.0 )                    */

    if (pPl->pDevelop->iDayAftEmerg > 0)
      {
      CropMaintenance_SPASS(exp_p);
      BiomassGrowth_SPASS(exp_p);
      }
    }

     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWCanopyForm_SPASS
// Description:    Canopy formation module for cereals (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWCanopyForm_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
 
  if SimulationStarts
       introduce((LPSTR)"dllEWCanopyForm_SPASS");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 

    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

     if (pPl->pDevelop->fStageSUCROS > (float)0.0 )                   */

    if (pPl->pDevelop->iDayAftEmerg > 0)
	  {
      CanopyFormation_SPASS(exp_p);
	  PlantHeightGrowth_SUCROS(exp_p);
	  }
    }

  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWOrganSenesz_SPASS
// Description:    Organ senescence (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWOrganSenesz_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
  
  	int 	L,L1,NLAYR;
    float   fCumDepth,TRLDF,fThickness;
	float	fDeadRootC, fDeadRootN;

	PLAYERROOT		pLR	  = pPl->pRoot->pLayerRoot;
	PSLAYER 		pSL	  = pSo->pSLayer;
    PCLAYER			pCL;

 
  if SimulationStarts
       introduce((LPSTR)"dllEWOrganSenesz_SPASS");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 

    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

     if (pPl->pDevelop->fStageSUCROS > (float)0.0 )                   */

        if (pPl->pDevelop->iDayAftEmerg > 0)
	{
	    OrganSenescence_SPASS(exp_p);

		// Dead biomass to soil litter pools (sg 2005/11/14)

		//leaves
		pCh->pCProfile->fCLitterSurf += pPl->pBiomass->fLeafDeathRate * (float)0.4 ; // 40% C in Biomasse;
		pCh->pCProfile->fNLitterSurf += pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc;

		//roots
		if(pPl->pDevelop->fStageSUCROS>=pPl->pGenotype->fBeginSenesDvs)
		{
			NLAYR=pSo->iLayers-2;
			L = 0;
			fCumDepth = (float)0.0;

			while ((fCumDepth<=pPl->pRoot->fDepth)&&(L<NLAYR))
			{
				L++;
				fThickness =(float)0.1*pSL->fThickness; //cm
				fCumDepth +=(float)0.1*pSL->fThickness; //cm
				if (L<NLAYR-1) pSL =pSL ->pNext;
			}
			L1 = L;

			//Total Root Length Density Factor
			TRLDF =(float)0.0;
			pLR	  =pPl->pRoot->pLayerRoot;
			for (L=1;L<=L1;L++)
			{
				TRLDF += pLR->fLengthDensFac;
				pLR    = pLR->pNext;
			}

			fDeadRootC = pPl->pBiomass->fRootDeathRate * (float)0.4 ; // 40% C in Biomasse
			fDeadRootN = pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc;

			pSL	= pSo->pSLayer->pNext;
			pLR	= pPl->pRoot->pLayerRoot;
			pCL = pCh->pCLayer->pNext;
			for (L=1;L<=L1;L++)
			{
				fThickness =(float)0.1*pSL->fThickness; //cm
		        
				if(TRLDF > (float)0.0)
				{
					pCL->fCLitter += fDeadRootC * pLR->fLengthDensFac/TRLDF;
					pCL->fNLitter += fDeadRootN * pLR->fLengthDensFac/TRLDF;
				}

				pSL = pSL->pNext;
				pLR = pLR->pNext;
				pCL = pCL->pNext;
			}
		} //End dead root weight to soil litter pools
	}//End iDayAftEmerg > 0


    }
    
  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWNitrogenDemand_SPASS
// Description:    Plant nitrogen demand module (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWNitrogenDemand_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
   
  if SimulationStarts
    introduce((LPSTR)"dllEWNitrogenDemand_SPASS");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 

        
    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

     if (pPl->pDevelop->fStageSUCROS > (float)0.0 )                   */

    if (pPl->pDevelop->iDayAftEmerg > 0)
      {


      //SG/07/04/99/////////////////////////////////////////////////////////
      //
      // Die Funktion NitrogenConcentrationLimits_SPASS wird in PLAGEN an 
      // anderer Stelle aufgerufen. 
      //////////////////////////////////////////////////////////////////////
      
      NitrogenConcentrationLimits_SPASS(exp_p);

      PlantNitrogenDemand_SPASS(exp_p);
      }
    }
  
  return 1;
  }


 
//=============================================================================================
// DLL-Function:  dllEWNitrogenUptake_SPASS
// Description:    Plant nitrogen demand module (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWNitrogenUptake_SPASS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
 
  if SimulationStarts
    introduce((LPSTR)"dllEWNitrogenUptake_SPASS");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
    
    /*SG/01/04/99////////////////////////////////////////////
    In PLAGEN Abfrage nicht über Feldaufgang sondern über 
    Entwicklungsstadium!

     if (pPl->pDevelop->fStageSUCROS > (float)0.0 )                   */

    if (pPl->pDevelop->iDayAftEmerg > 0)
      {
      NitrogenUptake_SPASS(exp_p);
      PlantNitrogenStress_SPASS(exp_p);
      }
    }
    
  if PlantIsGrowing
      {
    int L;
    PLAYERROOT  pLR  =pPl->pRoot->pLayerRoot;
    PCLAYER   pSLN=pCh->pCLayer->pNext;

    for (L=1;L<=pSo->iLayers-2;L++)
      {
        //Check the whether there are roots in this layer:
        if (pLR->fLengthDens==(float)0.0)    break;

      //Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
      pSLN->fNO3N=pSLN->fNO3N-pLR->fActLayNO3NUpt*pTi->pTimeStep->fAct;
      pSLN->fNH4N=pSLN->fNH4N-pLR->fActLayNH4NUpt*pTi->pTimeStep->fAct;

      pLR =pLR ->pNext;
      pSLN=pSLN->pNext;
      }
      }
      

  return 1;
  }




//############################################################################################################
//  THE FOLLOWING FUNCTIONS FROM MODULE - CERES.C
//############################################################################################################
extern int CALLBACK _loadds PhasicDevelopment_CERES(EXP_POINTER);
extern int CALLBACK _loadds RootSystemFormation_CERES(EXP_POINTER);
extern int CALLBACK _loadds DailyCanopyGrossPhotosynthesis_CERES(EXP_POINTER);
extern float CALLBACK _loadds PotentialTranspiration_CERES(EXP_POINTER);                                     
extern int CALLBACK _loadds ActualTranspiration_CERES(EXP_POINTER);
extern int CALLBACK _loadds PlantWaterStress_CERES(EXP_POINTER);
extern int CALLBACK _loadds CropMaintenance_CERES(EXP_POINTER);
extern int CALLBACK _loadds BiomassPartition_CERES(EXP_POINTER);
extern int CALLBACK _loadds BiomassGrowth_CERES(EXP_POINTER);
extern int CALLBACK _loadds CanopyFormation_CERES(EXP_POINTER);
extern int CALLBACK _loadds PlantSenescense_CERES(EXP_POINTER);
extern int CALLBACK _loadds NitrogenConcentrationLimits_CERES(EXP_POINTER);
extern int CALLBACK _loadds PlantNitrogenDemand_CERES(EXP_POINTER);
extern int CALLBACK _loadds NitrogenUptake_CERES(EXP_POINTER);
extern int CALLBACK _loadds PlantNitrogenStress_CERES(EXP_POINTER);    

//=============================================================================================
// DLL-Function:  dllEWDevelopment_CERES
// Description:    Phasic development module for cereal crops (Wang,1997)
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWDevelopment_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
    
  if SimulationStarts
         introduce((LPSTR)"dllEWDevelopment_CERES");

  if (pPl==NULL) return 1;

        
  if NewDayAndPlantGrowing
    PhasicDevelopment_CERES(exp_p);

  DevelopmentCheckAndPostHarvestManagement(exp_p);
       
    return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWRootSystem_CERES
// Description:    Root system formation module for all crops (Wang,1997)
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWRootSystem_CERES(EXP_POINTER)
  {
   DECLARE_COMMON_VAR
 
  if SimulationStarts
    introduce((LPSTR)"dllEWRootSystem_CERES");
  
  if NewDayAndPlantGrowing
    RootSystemFormation_CERES(exp_p);

     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPhotosynthesis_CERES 
// Description:  Daily canopy photosynthesis according to Wang (1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWPhotosynthesis_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
   
  if SimulationStarts
    introduce((LPSTR)"dllEWPhotosynthesis_CERES");
  
  if NewDayAndPlantGrowing
  {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
    if (pPl->pDevelop->iDayAftEmerg > 0)
	{
		//SG 20111114:
		//CO2-conc. from input file (*.xnm) for AgMIP-project
		fAtmCO2 = (pCl->pWeather->fSnow > 0? pCl->pWeather->fSnow:fAtmCO2);
		DailyCanopyGrossPhotosynthesis_CERES(exp_p);
	}
  }
  
  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPotTransp_CERES
// Description:    Daily canopy potential transpiration (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
extern float CALLBACK _loadds PotentialTranspiration_CERES(EXP_POINTER);

int WINAPI  dllEWPotTransp_CERES(EXP_POINTER)
  {
   DECLARE_COMMON_VAR
 
  if SimulationStarts
       introduce((LPSTR)"dllEWPotTransp_CERES");

  if ((int)NewDay(pTi)) fPotTraDay=(float)0;

  if NewDayAndPlantGrowing
  {
   //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
   if (pPl->pDevelop->iDayAftEmerg > 0)
   fPotTraDay=PotentialTranspiration_CERES(exp_p);                                     
  }  
   
   if PlantIsGrowing
      {
      float DeltaT;
      DeltaT=pTi->pTimeStep->fAct;

      pPl->pPltWater->fPotTranspdt = fPotTraDay * DeltaT;
      // ep 210599 to avoid "initial value = daily value" in balance.c by using
          // pPl->pPltWater->fPotTranspdt = pPl->pPltWater->fPotTranspDay * DeltaT;

      // since pPl->pPltWater->fPotTranspDay is used as cumulative variable which
      // sums up the pPl->pPltWater->fPotTranspdt values and has to be zero at the
      // beginning of a new day
      }

     return 1;
  }


  
//=============================================================================================
// DLL-Function:  dllEWActTransp_CERES
// Description:    Daily water uptake and water stress factors (CERES and Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWActTransp_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR   

  
  if SimulationStarts
    {
    int L;

    introduce((LPSTR)"dllEWActTransp_CERES");

    for (L=1;L<=pSo->iLayers-2;L++)
      afActLayRootWatUpt[L]=(float)0;
    }
  
    

  if ((int)NewDay(pTi)) fActTraDay=(float)0;
  if (((int)NewDay(pTi))&&((pPl!=NULL))) pPl->pRoot->fUptakeR =(float)0.0;
  //ep 05.02.01  not reinitialized after harvest (in capacity water flow module)!


  if NewDayAndPlantGrowing
    {                   
    int L;
    PLAYERROOT   pLR  =pPl->pRoot->pLayerRoot;

    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    
    if (pPl->pDevelop->iDayAftEmerg > 0)
      {
       ActualTranspiration_CERES(exp_p);
       PlantWaterStress_CERES(exp_p);
      }

    //-------------------------------------------------------------------
    // If the time step is smaller than one day, the water uptake is 
    // calculated from daily Wateruptake
    //-------------------------------------------------------------------
    
    for (L=1;L<=pSo->iLayers-2;L++)
      {
      afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
      pLR =pLR ->pNext;
      }

       
    }  //  end if NewDayAndPlantGrowing


    
  if PlantIsGrowing
    {
    PWLAYER    pSLW=pWa->pWLayer->pNext; 

    int L;

    for (L=1;L<=pSo->iLayers-2;L++)
      {
      pSLW->fContAct -= afActLayRootWatUpt[L]
                       /pSo->fDeltaZ
                       *pTi->pTimeStep->fAct; //mm/mm
      pSLW=pSLW->pNext;
      }

    } //end if plant is growing     


  return 1;
  }  

  
//=============================================================================================
// DLL-Function:  dllEWBiomassGrowth_CERES
// Description:    Maitenance and biomass growth (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWBiomassGrowth_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  if SimulationStarts
       introduce((LPSTR)"dllEWBiomassGrowth_CERES");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
      {
      CropMaintenance_CERES(exp_p);
      BiomassPartition_CERES(exp_p);
      BiomassGrowth_CERES(exp_p);
      }
    }

     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWCanopyForm_CERES
// Description:    Canopy formation module for cereals (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWCanopyForm_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
 
  if SimulationStarts
       introduce((LPSTR)"dllEWCanopyForm_CERES");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
	  {
      CanopyFormation_CERES(exp_p);
	  PlantHeightGrowth_CERES(exp_p);
	  }
    }

  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWOrganSenesz_CERES
// Description:    Organ senescence (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWOrganSenesz_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
  
  	int 	L,L1,NLAYR;
    float   fCumDepth,TRLDF,fThickness;
	float	fDeadRootC, fDeadRootN;

	PLAYERROOT		pLR	  = pPl->pRoot->pLayerRoot;
	PSLAYER 		pSL	  = pSo->pSLayer;
    PCLAYER			pCL;

 
  if SimulationStarts
       introduce((LPSTR)"dllEWOrganSenesz_CERES");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
		{
			PlantSenescense_CERES(exp_p);

			// Dead biomass to soil litter pools (sg 2005/11/14)

			//leaves
			pCh->pCProfile->fCLitterSurf += pPl->pBiomass->fLeafDeathRate * (float)0.4 ; // 40% C in Biomasse;
			pCh->pCProfile->fNLitterSurf += pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fTopsMinConc;

			//roots
			if(pPl->pDevelop->fStageSUCROS>=pPl->pGenotype->fBeginSenesDvs)
			{
				NLAYR=pSo->iLayers-2;
				L = 0;
				fCumDepth = (float)0.0;

				while ((fCumDepth<=pPl->pRoot->fDepth)&&(L<NLAYR))
				{
					L++;
					fThickness =(float)0.1*pSL->fThickness; //cm
					fCumDepth +=(float)0.1*pSL->fThickness; //cm
					if (L<NLAYR-1) pSL =pSL ->pNext;
				}
				L1 = L;

				//Total Root Length Density Factor
				TRLDF =(float)0.0;
				pLR	  =pPl->pRoot->pLayerRoot;
				for (L=1;L<=L1;L++)
				{
					TRLDF += pLR->fLengthDensFac;
					pLR    = pLR->pNext;
				}

				fDeadRootC = pPl->pBiomass->fRootDeathRate * (float)0.4 ; // 40% C in Biomasse
				fDeadRootN = pPl->pBiomass->fRootDeathRate*pPl->pPltNitrogen->fRootMinConc;

				pSL	= pSo->pSLayer->pNext;
				pLR	= pPl->pRoot->pLayerRoot;
				pCL = pCh->pCLayer->pNext;
				for (L=1;L<=L1;L++)
				{
					fThickness =(float)0.1*pSL->fThickness; //cm
			        
					if(TRLDF > (float)0.0)
					{
						pCL->fCLitter += fDeadRootC * pLR->fLengthDensFac/TRLDF;
						pCL->fNLitter += fDeadRootN * pLR->fLengthDensFac/TRLDF;
					}

					pSL = pSL->pNext;
					pLR = pLR->pNext;
					pCL = pCL->pNext;
				} // End for(...)
			} //End dead root weight to soil litter pools
		} // End iDayAftEmerg > 0
    }
    
  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWNitrogenDemand_CERES
// Description:    Plant nitrogen demand module (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWNitrogenDemand_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
   
  if SimulationStarts
    introduce((LPSTR)"dllEWNitrogenDemand_CERES");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
      {
      NitrogenConcentrationLimits_CERES(exp_p);
      PlantNitrogenDemand_CERES(exp_p);
      }
    }
  
  return 1;
  }


 
//=============================================================================================
// DLL-Function:  dllEWNitrogenUptake_CERES
// Description:    Plant nitrogen demand module (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWNitrogenUptake_CERES(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
 
  if SimulationStarts
    introduce((LPSTR)"dllEWNitrogenUptake_CERES");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
    if (pPl->pDevelop->iDayAftEmerg > 0)
      {
      NitrogenUptake_CERES(exp_p);
      PlantNitrogenStress_CERES(exp_p);    
      }
    }
    
  if PlantIsGrowing
      {
    int L;
    PLAYERROOT  pLR  =pPl->pRoot->pLayerRoot;
    PCLAYER   pSLN=pCh->pCLayer->pNext;

    for (L=1;L<=pSo->iLayers-2;L++)
      {
        //Check the whether there are roots in this layer:
        if (pLR->fLengthDens==(float)0.0)    break;

      //Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
      pSLN->fNO3N=pSLN->fNO3N-pLR->fActLayNO3NUpt*pTi->pTimeStep->fAct;
      pSLN->fNH4N=pSLN->fNH4N-pLR->fActLayNH4NUpt*pTi->pTimeStep->fAct;

      pLR =pLR ->pNext;
      pSLN=pSLN->pNext;
      }
      }
      

  return 1;
  }

  
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  THE FOLLOWING FUNCTIONS FROM MODULE - SUCROS.C
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
extern int CALLBACK _loadds PhasicDevelopment_SUCROS(EXP_POINTER);
extern int CALLBACK _loadds Photosynthesis_SUCROS(EXP_POINTER);
extern int CALLBACK _loadds BiomassGrowth_SUCROS(EXP_POINTER);
extern int CALLBACK _loadds CanopyFormation_SUCROS(EXP_POINTER);
extern int CALLBACK _loadds RootSystemFormation_SUCROS(EXP_POINTER);
extern float CALLBACK _loadds PotentialTranspiration_SUCROS(EXP_POINTER);
extern int CALLBACK _loadds ActualTranspiration_SUCROS(EXP_POINTER);
extern int CALLBACK _loadds PotentialNitrogenUptake_WAVE(EXP_POINTER);
extern int CALLBACK _loadds ActualNitrogenUptake_WAVE(EXP_POINTER);

//=============================================================================================
// DLL-Function:  dllEWDevelopment_CERES
// Description:    Phasic development module for cereal crops (Wang,1997)
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWDevelopment_SUCROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
    
  if SimulationStarts
         introduce((LPSTR)"dllEWDevelopment_SUCROS");

  if(pPl==NULL) return 1;
        
  if NewDayAndPlantGrowing
    PhasicDevelopment_SUCROS(exp_p);

  DevelopmentCheckAndPostHarvestManagement(exp_p);
       
    return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWRootSystem_SUCROS
// Description:    Root system formation module for all crops (Wang,1997)
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWRootSystem_SUCROS(EXP_POINTER)
  {
   DECLARE_COMMON_VAR
 
  if SimulationStarts
    introduce((LPSTR)"dllEWRootSystem_SUCROS");
  
  if NewDayAndPlantGrowing
    RootSystemFormation_SUCROS(exp_p);

     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPhotosynthesis_SUCROS 
// Description:  Daily canopy photosynthesis according to Wang (1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWPhotosynthesis_SUCROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
   
  if SimulationStarts
    introduce((LPSTR)"dllEWPhotosynthesis_SUCROS");
  
  if NewDayAndPlantGrowing
  {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
    if (pPl->pDevelop->iDayAftEmerg > 0)
	{			
		//SG 20111114:
		//CO2-conc. from input file (*.xnm) for AgMIP-project
		fAtmCO2 = (pCl->pWeather->fSnow > 0? pCl->pWeather->fSnow:fAtmCO2);
		
		Photosynthesis_SUCROS(exp_p);
	}
  }
  
  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPotTransp_SUCROS
// Description:    Daily canopy potential transpiration (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
extern float CALLBACK _loadds PotentialTranspiration_SUCROS(EXP_POINTER);

int WINAPI  dllEWPotTransp_SUCROS(EXP_POINTER)
	{
	 DECLARE_COMMON_VAR
 
	if SimulationStarts
       introduce((LPSTR)"dllEWPotTransp_SUCROS");

	if ((int)NewDay(pTi)) fPotTraDay=(float)0;

	if NewDayAndPlantGrowing
	{
	 //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
	 if (pPl->pDevelop->iDayAftEmerg > 0)
	 fPotTraDay=PotentialTranspiration_SUCROS(exp_p);                                     
	}	
	 
	 if PlantIsGrowing
    	{
		  float DeltaT;
		  DeltaT=pTi->pTimeStep->fAct;

		  pPl->pPltWater->fPotTranspdt = fPotTraDay * DeltaT;
	    }
   	return 1;
	}


  
//=============================================================================================
// DLL-Function:  dllEWActTransp_SUCROS
// Description:    Daily water uptake and water stress factors (CERES and Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWActTransp_SUCROS(EXP_POINTER)
	{
	DECLARE_COMMON_VAR   

	//static double vSum;
	//double v,v1,vSin,vSumSin;
	
	if SimulationStarts
    {
		int L;

       introduce((LPSTR)"dllEWActTransp_SUCROS");

		for (L=1;L<=pSo->iLayers-2;L++)
			afActLayRootWatUpt[L]=(float)0;
	}
	
		

	if ((int)NewDay(pTi)) fActTraDay=(float)0;
	if (((int)NewDay(pTi))&&((pPl!=NULL))) pPl->pRoot->fUptakeR =(float)0.0;


	if NewDayAndPlantGrowing
	{                   
	 int L;
	 PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;

	//	vSum=0.0;
		//Wird nur berechnet wenn der Feldaufgang erreicht wurde 
		if (pPl->pDevelop->iDayAftEmerg > 0)
			{
			ActualTranspiration_SUCROS(exp_p);
//			PlantWaterStress_SUCROS(exp_p);
			}
		//-------------------------------------------------------------------
     // If the time step is smaller than one day, the water uptake is 
	 // calculated from daily Wateruptake
	 //-------------------------------------------------------------------
		
	 for (L=1;L<=pSo->iLayers-2;L++)
	 {
	  afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
	  pLR =pLR ->pNext;
	 }

		   
	}	//	end if NewDayAndPlantGrowing


    
	if PlantIsGrowing
    {

//				PSLAYER		pSL	=pSo->pSLayer->pNext;
				PWLAYER		pSLW=pWa->pWLayer->pNext; 
//				PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;

				int L;
                ///*
			    for (L=1;L<=pSo->iLayers-2;L++)
			    	{
					pSLW->fContAct -= afActLayRootWatUpt[L]
						              /pSo->fDeltaZ
									  *pTi->pTimeStep->fAct; //mm/mm
//			      	pSL =pSL ->pNext;
			      	pSLW=pSLW->pNext;
//			      	pLR =pLR ->pNext;
					}
               //*/

        } //end if plant is growing   	
    
/*

	//-------------------------------------------------------------------------------------
	//If the time step is smaller than one day, the water uptake is calculated as follows:
	//During the night no water uptake occurs   (0.8-0.3)
	//During the daytime, water uptake follows a sine curve  (0.3-0.8)
	v1=0; 
	for (v=max(0.30001,(double)DeltaT);v<0.8;v=v+DeltaT) 
		v1++;
	v1=max(v1,1.0);
	
	vSumSin=0.0;
	for (v=1; v<=v1; v++)
		vSumSin += sin(3.1415926*(2*v-1)/(2.0*v1));
	
	v=(double)(SimTime-(int)SimTime);
	
	if ((v>0.300001)&&(v<0.8)||(v1==1.0)&&(vSum==0.0))
	{
	vSum++;
	vSin = sin(3.1415926*(2*vSum-1)/(2.0*v1));
	
	if PlantIsGrowing
    	{
		int L;
		PSLAYER		pSL	=pSo->pSLayer->pNext;
		PWLAYER		pSLW=pWa->pWLayer->pNext; 
		PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;

	    for (L=1;L<=pSo->iLayers-2;L++)
	    	{
			pSLW->fContAct -= pLR->fActLayWatUpt*(float)(vSin/vSumSin)/pSL->fThickness; //mm/mm
	      	pSL =pSL ->pNext;
	      	pSLW=pSLW->pNext;
	      	pLR =pLR ->pNext;
			}
    	}
	}   
	*/

   	return 1;
	}	

  
//=============================================================================================
// DLL-Function:  dllEWBiomassGrowth_SUCROS
// Description:    Maitenance and biomass growth (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWBiomassGrowth_SUCROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  if SimulationStarts
       introduce((LPSTR)"dllEWBiomassGrowth_SUCROS");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
      BiomassGrowth_SUCROS(exp_p);
    }

     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWCanopyForm_SUCROS
// Description:    Canopy formation module for cereals (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWCanopyForm_SUCROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
 
  if SimulationStarts
       introduce((LPSTR)"dllEWCanopyForm_SUCROS");

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
	  {
	  CanopyFormation_SUCROS(exp_p);
	  PlantHeightGrowth_SUCROS(exp_p);
	  }
    }

  return 1;
  }

//=============================================================================================
// DLL-Function:  dllSGOrganSenesz_SUCROS
// Description:    Canopy formation module for cereals (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEWOrganSenesz_SUCROS(EXP_POINTER)
  {

	if NewDayAndPlantGrowing
    {
		if (pPl->pDevelop->iDayAftEmerg > 0)
		{
			// Dead biomass to soil litter pools (sg 2005/11/14)

			//leaves
			pCh->pCProfile->fCLitterSurf += pPl->pBiomass->fLeafDeathRate * (float)0.4 ; // 40% C in Biomasse;
			pCh->pCProfile->fNLitterSurf += pPl->pBiomass->fLeafDeathRate*pPl->pPltNitrogen->fLeafMinConc;
		}
	}


  return 1;
  }
  
  //=============================================================================================
// DLL-Function:	dllEPNitrogenDemand_WAVE
// Description:		calculation of nitrogen demand following WAVE 2.0 (Priesack, 2003) 			                            
// Updating Date:  	24.04.03                   
//=============================================================================================
int WINAPI dllEPNitrogenDemand_WAVE(EXP_POINTER)
	{
	DECLARE_COMMON_VAR
 
	if SimulationStarts
       introduce((LPSTR)"dllEPNitrogenDemand_WAVE");

	if NewDayAndPlantGrowing
		{
		//Wird nur berechnet wenn der Feldaufgang erreicht wurde 
		if (pPl->pDevelop->iDayAftEmerg > 0)
			PotentialNitrogenUptake_WAVE(exp_p);
		}

	return 1;
	}

//=============================================================================================
// DLL-Function:	dllEPNitrogenUptake_WAVE
// Description:		calculation of actual nitrogen uptake following WAVE 2.0 (Priesack, 2003) 			                            
// Updating Date:  	24.04.03                   
//=============================================================================================
int WINAPI dllEPNitrogenUptake_WAVE(EXP_POINTER)
	{
	DECLARE_COMMON_VAR
 
	if SimulationStarts
       introduce((LPSTR)"dllEPNitrogenUptake_WAVE");

	if NewDayAndPlantGrowing
		{
		//Wird nur berechnet wenn der Feldaufgang erreicht wurde 
		if (pPl->pDevelop->iDayAftEmerg > 0)
			ActualNitrogenUptake_WAVE(exp_p);
		}
    	
    ///*
    if PlantIsGrowing
    	{
		int L;
		PLAYERROOT	pLR	=pPl->pRoot->pLayerRoot;
		PCLAYER 	pSLN=pCh->pCLayer->pNext;

		for (L=1;L<=pSo->iLayers-2;L++)
			{
		    //Check the whether there are roots in this layer:
		    if (pLR->fLengthDens==(float)0.0)		break;

			//Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
			pSLN->fNO3N=pSLN->fNO3N-pLR->fActLayNO3NUpt*pTi->pTimeStep->fAct;
			pSLN->fNH4N=pSLN->fNH4N-pLR->fActLayNH4NUpt*pTi->pTimeStep->fAct;

			pLR =pLR ->pNext;
			pSLN=pSLN->pNext;
			}
    	}
    //*/
	return 1;
	}

//############################################################################################################
//  THE FOLLOWING FUNCTIONS FROM MODULE - LEACHN.C
//############################################################################################################

extern int WINAPI WaterUptakeNimahHanks(EXP_POINTER); 
extern int WINAPI WaterUptakeFeddes(EXP_POINTER);
extern int WINAPI WaterUptakeVanGenuchten(EXP_POINTER);

extern int WINAPI GrowMaizeLeach(EXP_POINTER); 
extern int WINAPI LeafMaizeLeach(EXP_POINTER);
extern int WINAPI RootMaizeLeach(EXP_POINTER);

extern int WINAPI NUptake(EXP_POINTER);
extern int WINAPI PotentialTranspiration(EXP_POINTER);
extern int WINAPI Interception(EXP_POINTER);

/********************************************************************/
/* dll-Funktion:   dllCHCanopyForm_LEACH                            */
/* Beschreibung:   Blattflächenwachstum nach LeachN                 */
/*         ch/gsf                   24.4.95                         */
/********************************************************************/
int WINAPI _loadds dllCHCanopyForm_LEACH(EXP_POINTER)
{
 DECLARE_COMMON_VAR

 PPLTMEASURE pPM;
 
    if (SimStart(pTi))
       {
       introduce((LPSTR)"dllCHCanopyForm_LEACH");
       }
  
    if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return 0;

    if ((pPl != NULL)&&((pPl->pPMeasure == NULL)||(pPl->pPMeasure->pNext == NULL)))
    {
    Message(3, ERROR_NO_PLANT_MEASURE);
    return -1;
    }
    
    LeafMaizeLeach(exp_p);


  //Wenn Tag der Ernte erreicht ist, müssen die Ernterückstände den 
  //Litter-Pools zugeschrieben werden

  pPM = pPl->pPMeasure;                  
/*
  if (NewDay(pTi))
    while ((pPM->pNext!=NULL)&&((pPM->pNext->iDay-EPSILON)<=pTi->pSimTime->fTimeAct+(float)1.0))
      pPM = pPM->pNext;
*/

  if (pTi->pSimTime->fTimeAct == (float)(pPl->pModelParam->iHarvestDay-(int)1))
  {  
    float actDepth,RootProp,RootSum;
    float *pMat1Local;
    int    i1,i2;
    PSLAYER    pSL;
    PCLAYER    pCL; 

    i1 = (int)99; //aktuelle Bodenschichten
    RootSum = actDepth = (float)0.0;

    pMat1Local        = (float *) _MALLOC(i1 * sizeof(float));
    memset(pMat1Local,0x0,(i1 * sizeof(float)));
    
    // The aboveground biomass at iHarvestDay must be written 
    // to the surfacepools and the rootbiomass to the Litterpools
    /*
    if(pPl->pModelParam->cResidueCarryOff==0)
    {
    pCh->pCProfile->fDryMatterLitterSurf += pPM->fAboveBiomass-pPM->fGenBiomass;
    pCh->pCProfile->fNLitterSurf += pPl->pPltNitrogen->fCumActNUpt;
    pCh->pCProfile->fCLitterSurf += (pPM->fAboveBiomass-pPM->fGenBiomass) * (float)0.45 ; // 45% C in Biomasse
    
    if (pCh->pCProfile->fNLitterSurf > (float)0.0)
    pCh->pCProfile->fCNLitterSurf = pCh->pCProfile->fCLitterSurf /
                          pCh->pCProfile->fNLitterSurf; 
    }
    */
    pMa->pLitter->fRootC = pPM->fRootWeight*(float)0.4; //40% C in Biomasse


    if (pPM->fNRoot > (float) 0)
      pMa->pLitter->fRootCN = pPM->fRootWeight*(float)0.4/pPM->fNRoot;
    else
      pMa->pLitter->fRootCN =(float)20.0;// ep 250399 fRootCN=0.1 overestimates NLitter


     // Berechnen schichtmaessiger Anteil. Zur Vereinfachung 
     // Variable pCL->fCLitter benutzt um Anteile zu uebergeben. 

    for (pSL = pSo->pSLayer->pNext,
       pCL = pCh->pCLayer->pNext,i2=0;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext,i2++)
    {
      actDepth = actDepth + (pSL->fThickness / 10);
      RootProp =  (float)exp((float)-3.0 * actDepth / (pSo->fDepth / (float)10.0));
      RootSum += RootProp;

      pMat1Local[i2] = RootProp;
    }

    // Anteilsmaessige Verteilung auf die Bodenschichten.
  /* auskommentiert da fuer mehrmals gemaehte Zwischenfrucht ungeeignet
  /* LEACHN ist reines Entzugsmodell, kein C-Produktionsmodell!
    for (pSL = pSo->pSLayer->pNext,
       pCL = pCh->pCLayer->pNext,i2=0;
                         ((pSL->pNext != NULL)&&
                          (pCL->pNext != NULL));
         pSL = pSL->pNext,
         pCL = pCL->pNext,i2++)
    {
      factor = pMat1Local[i2] / RootSum;
          amount = pMa->pLitter->fRootC * factor;

      pCL->fCLitter += amount;
      pCL->fNLitter += amount / pMa->pLitter->fRootCN;
      
      if((pCL->fCLitter>EPSILON)&&(pCL->fNLitter>EPSILON))  
        {
          pCL->fLitterCN = pCL->fCLitter / pCL->fNLitter;          
        }             
        else
          pCL->fLitterCN =(float)0.1;
    }*/    
     free(pMat1Local);                                        

  }

    return 1;
}   // Ende Berechnung




/********************************************************************/
/* dll-Funktion:   dllCHRootSystem_LEACH                            */
/* Beschreibung:   Wurzelwachstum nach LeachN                       */
/*         ch/gsf                   24.4.95                         */
/********************************************************************/
int WINAPI _loadds dllCHRootSystem_LEACH(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
    if (SimStart(pTi))
       {
       introduce((LPSTR)"dllCHRootSystem_LEACH");
       }

    if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return 0;

    if ((pPl != NULL)&&((pPl->pPMeasure == NULL)||(pPl->pPMeasure->pNext == NULL)))
    {
    Message(3, ERROR_NO_PLANT_MEASURE);
    return -1;
    }
    
    RootMaizeLeach(exp_p);

    return 1;
}   // Ende Berechnung
 
 
/********************************************************************/
/* dll-Funktion:   dllCHActTransp_LEACH                                 */
/* Beschreibung:   Pflanzenwasserhaushalt nach LeachN               */
/*         ch/gsf                   24.4.95                         */
/********************************************************************/

int iWaterUptakeModel; //StefanAchatz extern-Variable in ReadMod gelesen

int WINAPI _loadds dllCHActTransp_LEACH(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
    if (SimStart(pTi))
       {
       introduce((LPSTR)"dllCHActTransp_LEACH");
       }

    if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return 0;

   // if ((pPl != NULL)&&((pPl->pPMeasure == NULL)||(pPl->pPMeasure->pNext == NULL)))
   // {
   // Message(3, ERROR_NO_PLANT_MEASURE);
   // return -1;
   // }

  //PotentialTranspiration(exp_p); // pot. Transpiration auskommentiert //ep/gsf 130899
                   // jetzt durch dllCHPotTransp_LEACH aufgerufen
  switch(iWaterUptakeModel)
  {
  case 1: 
    WaterUptakeNimahHanks(exp_p);
    break;
  case 2: 
    WaterUptakeFeddes(exp_p);    //StefanAchatz did that
    break;
  case 3: 
    WaterUptakeVanGenuchten(exp_p);//ebenso;
    break;
  default: WaterUptakeNimahHanks(exp_p);
  }

    return 1;
}   // Ende Berechnung

/********************************************************************/
/* dll-Funktion:   dllCHNitrogenUptake_LEACH                        */
/* Beschreibung:   N-Aufnahme der Pflanzen nach LeachN              */
/*         ch/gsf                   10.5.95                         */
/********************************************************************/
int WINAPI _loadds dllCHNitrogenUptake_LEACH(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
    if (SimStart(pTi))
       {
       introduce((LPSTR)"dllCHNitrogenUptake_LEACH");
       }

    if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return 0;

    if ((pPl != NULL)&&((pPl->pPMeasure == NULL)||(pPl->pPMeasure->pNext == NULL)))
    {
    Message(3, ERROR_NO_PLANT_MEASURE);
    return -1;
    }

   NUptake(exp_p);
   return 1;
}   // Ende Berechnung


 
/********************************************************************/
/* dll-Funktion:   dllCHTranspiration                               */
/* Beschreibung:                                                    */
/*         ch/gsf                   10.5.95                         */
/********************************************************************/
int WINAPI _loadds dllCHPotTransp_LEACH(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
    if (SimStart(pTi))
       {
       introduce((LPSTR)"dllCHPotTransp_LEACH");
       }
    	
	   
	//if (NewDay(pTi))
	//{
    //Interception(exp_p);
	//}

    PotentialTranspiration(exp_p);

   return 1;
}  /* Ende  Berechnung    */



/***********************************************************************/
/* dll-Funktion:   dllCHPlantLeach                                     */
/* Beschreibung:   Pflanzenteil: Wachstum, Wasserhaushalt, N-Aufnahme  */
/*         ch/gsf                   10.5.95                            */
/***********************************************************************/
int WINAPI _loadds dllCHPlantLeach(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
    if (SimStart(pTi))
       {
       introduce((LPSTR)"dllCHPlantLeach");
       }

    if ((pMa->pSowInfo == NULL)||(pPl == NULL)) return 0;

    if ((pPl != NULL)&&((pPl->pPMeasure == NULL)||(pPl->pPMeasure->pNext == NULL)))
    {
    Message(3, ERROR_NO_PLANT_MEASURE);
    return -1;
    }

   GrowMaizeLeach(exp_p);
   WaterUptakeNimahHanks(exp_p);
   PotentialTranspiration(exp_p);
   
   NUptake(exp_p);
   return 1;
}   // Ende Berechnung

/***********************************************************************************************
 * dll-Funktion:   dllEPPlaH2OFeddes
 ***********************************************************************************************/
int WINAPI _loadds dllEPPlaH2OFeddes(EXP_POINTER)
{                              
 if (SimStart(pTi)) introduce((LPSTR)"dllEPPlaH2OFeddes");
 WaterUptakeFeddes(exp_p);    //StefanAchatz

 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllEPPlaH2OVanGenuchten
 ***********************************************************************************************/
int WINAPI _loadds dllEPPlaH2OVanGenuchten(EXP_POINTER)
{                              
 if (SimStart(pTi)) introduce((LPSTR)"dllEPPlaH2OVanGenuchten");
 WaterUptakeVanGenuchten(exp_p);// StefanAchatz

 return 1;
}


/*******************************************************************************
** EOF */

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  THE FOLLOWING FUNCTIONS FROM MODULE - GECROS.C
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
extern int CALLBACK _loadds PhasicDevelopment_GECROS(EXP_POINTER);
//extern int CALLBACK _loadds Main_GECROS(EXP_POINTER);
extern int CALLBACK _loadds Init_GECROS(EXP_POINTER);
extern int CALLBACK _loadds Photosynthesis_GECROS(EXP_POINTER);
extern int CALLBACK _loadds BiomassGrowth_GECROS(EXP_POINTER);
extern int CALLBACK _loadds CanopyFormation_GECROS(EXP_POINTER);
extern int CALLBACK _loadds RootSystemFormation_GECROS(EXP_POINTER);
//extern float CALLBACK _loadds PotentialTranspiration_GECROS(EXP_POINTER);
//extern int CALLBACK _loadds ActualTranspiration_GECROS(EXP_POINTER);
extern int CALLBACK _loadds PotentialNitrogenUptake_GECROS(EXP_POINTER);
extern int CALLBACK _loadds ActualNitrogenUptake_GECROS(EXP_POINTER);
extern int CALLBACK _loadds OrganSenescence_GECROS(EXP_POINTER);
extern int CALLBACK _loadds PlantHeightGrowth_GECROS(EXP_POINTER);
extern int CALLBACK _loadds NitrogenFixation_GECROS(EXP_POINTER);

extern int WINAPI   allocateGECROSVariables();
extern int WINAPI   freeAllocatedGECROSMemory();
extern int iGECROS;
extern int SoilWaterFlow_GECROS(EXP_POINTER);
extern int SoilNitrogen_GECROS(EXP_POINTER);


//=============================================================================================
// DLL-Function:  dllEPMain_GECROS
// Description:   Phasic development module 
// Updating Date: 07.04.2008/ep                   
//=============================================================================================
/*
int WINAPI  dllEPMain_GECROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
    
  if SimulationStarts
         introduce((LPSTR)"dllEPDevelopment_GECROS");

  if(pPl==NULL) return 1;

  if (SimulationStarts && (iGECROS <= (int)0))
    {
	 allocateGECROSVariables();
	 iGECROS = (int)1;
	 //Initialise_GECROS(exp_p);
	}
  

        
  //if NewDayAndPlantGrowing PhasicDevelopment_GECROS(exp_p);
  if NewDayAndPlantGrowing
    {
     //Init_GECROS(exp_p);
	 //PhasicDevelopment_GECROS(exp_p);
	 Main_GECROS(exp_p);
	}
  //SoilWaterFlow_GECROS(exp_p);
  //ActualNitrogenUptake_GECROS(exp_p);
  //SoilNitrogen_GECROS(exp_p);

  if (SimulationEnds && (iGECROS == (int)1))
  {
   freeAllocatedGECROSMemory();
   iGECROS = (int)0;
  }

  DevelopmentCheckAndPostHarvestManagement(exp_p);
       
    return 1;
  }
  */
  //=============================================================================================
// DLL-Function:  dllEPDevelopment_GECROS
// Description:   Phasic development module 
// Updating Date: 07.04.2008/ep                   
//=============================================================================================
int WINAPI  dllEPDevelopment_GECROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
    
  if SimulationStarts
         introduce((LPSTR)"dllEPDevelopment_GECROS");

  if(pPl==NULL) return 1;

  if (SimulationStarts && (iGECROS <= (int)0))
    {
	 allocateGECROSVariables();
	 iGECROS = (int)1;
	 //Init_GECROS(exp_p);
	}
  
  if ((pMa->pSowInfo != NULL)&&(pTi->pSimTime->fTimeAct == (float)pMa->pSowInfo->iDay)) 
    Init_GECROS(exp_p);

        
  //if NewDayAndPlantGrowing PhasicDevelopment_GECROS(exp_p);
  if NewDayAndPlantGrowing
    {
     //Init_GECROS(exp_p);
	 PhasicDevelopment_GECROS(exp_p);
	 //Main_GECROS(exp_p);
	}
  //SoilWaterFlow_GECROS(exp_p);
  //ActualNitrogenUptake_GECROS(exp_p);
  //SoilNitrogen_GECROS(exp_p);

  if (SimulationEnds && (iGECROS == (int)1))
  {
   freeAllocatedGECROSMemory();
   iGECROS = (int)0;
  }

  DevelopmentCheckAndPostHarvestManagement(exp_p);
       
    return 1;
  }


//=============================================================================================
// DLL-Function:  dllEPRootSystem_GECROS
// Description:   Root system formation module 
// Updating Date: 07.04.2008/ep                   
//=============================================================================================
int WINAPI  dllEPRootSystem_GECROS(EXP_POINTER)
  {
   DECLARE_COMMON_VAR
 
  if SimulationStarts
    introduce((LPSTR)"dllEPRootSystem_GECROS");

  if (SimulationStarts && (iGECROS <= (int)0))
  {
   allocateGECROSVariables();
   iGECROS =(int)5;
  }
 
  if (SimulationStarts && (iGECROS > (int)0))
  {
   iGECROS = (int)5;
  }

  ///*
  if NewDayAndPlantGrowing
    RootSystemFormation_GECROS(exp_p);
  //*/

  if (SimulationEnds && (iGECROS == (int)5)) 
  {
   freeAllocatedGECROSMemory();
   iGECROS = (int)0;
  }

 
 return 1;
 }


//=============================================================================================
// DLL-Function:  dllEPPhotosynthesis_GECROS 
// Description:  Daily canopy photosynthesis according to Wang (1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEPPhotosynthesis_GECROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
   
  if SimulationStarts
    introduce((LPSTR)"dllEPPhotosynthesis_GECROS");

  if (SimulationStarts && (iGECROS <= (int)0))
  {
   allocateGECROSVariables();
   iGECROS =(int)2;
  }
 
  if (SimulationStarts && (iGECROS > (int)0))
  {
   iGECROS = (int)2;
  }

  ///*
  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde
    //if (pPl->pDevelop->iDayAftEmerg > 1)
      Photosynthesis_GECROS(exp_p);
    }
  //*/
   pWa->fPotETdt                = pWa->fPotETDay * pTi->pTimeStep->fAct;

// SG 20111107: fPotTraDay verwenden, da pPl->pPltWater->fPotTranspDay nicht ausserhalb von 
//              balance.c gesetzt werden darf (wird dort jeden Zeitschritt aufaddiert!)
//   pPl->pPltWater->fPotTranspdt = pPl->pPltWater->fPotTranspDay * pTi->pTimeStep->fAct;
   pPl->pPltWater->fPotTranspdt = fPotTraDay * pTi->pTimeStep->fAct;



   //////////////////////////////////////////////////////////////////////////////////////////
   //SG 20111122:
   // pPl->pRoot->fUptakeR sollte hier berechnet werden, da sonst fActTraDay vom Vortag verwendet wird, da dllEPPhotosynthesis_GECROS()
   // erst nach dllEPActTransp_GECROS() aufgerufen wird
   // --> Wenn hier aktiv, muss der entsprechende Abschnitt in dllEPActTransp_GECROS() auskommentiert werden!

//   if SimulationStarts
//    {
//		int L;
//
//       introduce((LPSTR)"dllEWActTransp_GECROS");
//
//	   pPl->pPltWater->fPotTranspDay = (float)0;
//
//	   for (L=1;L<=pSo->iLayers-2;L++)
//			afActLayRootWatUpt[L]=(float)0;
//	}
//	
//	
//	if (((int)NewDay(pTi))&&((pPl!=NULL))) pPl->pRoot->fUptakeR =(float)0.0;
//	
//
//	if NewDayAndPlantGrowing
//	{                   
//	 int L;
//	 float fExtWat;
//
//	 PSWATER		pSWL=pSo->pSWater->pNext;
//	 PWLAYER		pSLW=pWa->pWLayer->pNext; 
//	 PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;
//     
//	 pPl->pRoot->fUptakeR =(float)0.0;
//// SG 20111107: pPl->pPltWater->fPotTranspDay und pPl->pPltWater->fActTranspDay dürfen nicht ausserhalb von 
////              balance.c gesetzt werden (werden dort in jedem Zeitschritt aufaddiert!)
////	 pPl->pPltWater->fPotTranspDay = fPotTraDay;
//	 pPl->pPltWater->fPotTranspDay = (float)0.0;
//	 pPl->pPltWater->fActTranspDay = (float)0.0;
//		
//	 for (L=1;L<=pSo->iLayers-2;L++)
//	 {
//		 //SG 20111107: pLR->fActLayWatUpt auf 0 initialisieren
//		 pLR->fActLayWatUpt = (float)0.0;
//		 if(pPl->pRoot->fDepth > (float)0)
//		 //if((pPl->pRoot->fDepth > (float)0)&& (pSLW->fContAct > pSWL->fContPWP + (float)0.002))
//		 //if((pPl->pRoot->fDepth > (float)0)&& (pSLW->fContAct > (float)0.05))
//		 {
//          if(L*pSo->fDeltaZ<=pPl->pRoot->fDepth*(float)10)
//		  {
//	        pLR->fActLayWatUpt = fActTraDay*(float)min((float)1,pSo->fDeltaZ/(pPl->pRoot->fDepth*(float)10));
//
//		    //SG 20111122: es darf nur bis zum PWP entleert werden
//			fExtWat = max(0.0,pSLW->fContAct-pSWL->fContPWP);
//	        pLR->fActLayWatUpt = (float)min(fExtWat, pLR->fActLayWatUpt);
//		  }
//	      else if (((L-1)*pSo->fDeltaZ<=pPl->pRoot->fDepth*(float)10)
//			      &&(pPl->pRoot->fDepth*(float)10 < L*pSo->fDeltaZ))
//		  {
//            //SG 20111107: Anteil Wasseraufnahme der untersten durchwurzelten Schicht
//			//pLR->fActLayWatUpt = fActTraDay*(float)min((float)1,
//			//	               (L*pSo->fDeltaZ-pPl->pRoot->fDepth*(float)10)/(pPl->pRoot->fDepth*(float)10));
//	        pLR->fActLayWatUpt = fActTraDay*(float)min((float)1,
//				               (pPl->pRoot->fDepth*(float)10 - (L-1)*pSo->fDeltaZ)/(pPl->pRoot->fDepth*(float)10));
//
//            //SG 20111122: es darf nur bis zum PWP entleert werden
//			fExtWat = max(0.0,pSLW->fContAct-pSWL->fContPWP);
//	        pLR->fActLayWatUpt = (float)min(fExtWat, pLR->fActLayWatUpt);
//		  }
//		 
//		 }
//		 else
//	      pLR->fActLayWatUpt = (float)0;
//	 
//	  pPl->pRoot->fUptakeR += pLR->fActLayWatUpt;
//	  afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
//      
//	  pSWL=pSWL->pNext;
//      pSLW=pSLW->pNext;
//	  pLR =pLR ->pNext;
//	 }
//
//		   
//	}	//	end if NewDayAndPlantGrowing
//
//
//    
//	if ((PlantIsGrowing)&&(jGECROS==(int)0))
//    {
//
////				PSLAYER		pSL	=pSo->pSLayer->pNext;
//				PWLAYER		pSLW=pWa->pWLayer->pNext; 
////				PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;
//
//				int L;
//                ///*
//			    for (L=1;L<=pSo->iLayers-2;L++)
//			    	{
//					pSLW->fContAct -= afActLayRootWatUpt[L]
//						              /pSo->fDeltaZ
//									  *pTi->pTimeStep->fAct; //mm/mm
//                    
////			      	pSL =pSL ->pNext;
//			      	pSLW=pSLW->pNext;
////			      	pLR =pLR ->pNext;
//					}
//               //*/
//
//     } //end if plant is growing   	
    
	// end SG
	////////////////////////////////////////////////////////////////////////////////////

  if (SimulationEnds && (iGECROS == (int)2)) 
  {
   freeAllocatedGECROSMemory();
   iGECROS = (int)0;
  }

  return 1;
  }


//=============================================================================================
// DLL-Function:  dllEWPotTransp_SUCROS
// Description:    Daily canopy potential transpiration (Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
/* extern float CALLBACK _loadds PotentialTranspiration_SUCROS(EXP_POINTER);

int WINAPI  dllEWPotTransp_SUCROS(EXP_POINTER)
	{
	 DECLARE_COMMON_VAR
 
	if SimulationStarts
       introduce((LPSTR)"dllEWPotTransp_SUCROS");

	if ((int)NewDay(pTi)) fPotTraDay=(float)0;

	if NewDayAndPlantGrowing
	{
	 //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
	 if (pPl->pDevelop->iDayAftEmerg > 0)
	 fPotTraDay=PotentialTranspiration_SUCROS(exp_p);                                     
	}	
	 
	 if PlantIsGrowing
    	{
		  float DeltaT;
		  DeltaT=pTi->pTimeStep->fAct;

		  pPl->pPltWater->fPotTranspdt = fPotTraDay * DeltaT;
	    }
   	return 1;
	}

*/
  
//=============================================================================================
// DLL-Function:  dllEPActTransp_GECROS
// Description:    Daily water uptake and water stress factors (CERES and Wang,1997)                                   
// Updating Date:    12.08.97                   
//=============================================================================================
int WINAPI  dllEPActTransp_GECROS(EXP_POINTER)
	{
	DECLARE_COMMON_VAR   

	//static double vSum;
	//double v,v1,vSin,vSumSin;
	
	if SimulationStarts
    {
		int L;

       introduce((LPSTR)"dllEWActTransp_GECROS");

	   pPl->pPltWater->fPotTranspDay = (float)0;

	   for (L=1;L<=pSo->iLayers-2;L++)
			afActLayRootWatUpt[L]=(float)0;
	}
	
	
	if (((int)NewDay(pTi))&&((pPl!=NULL))) pPl->pRoot->fUptakeR =(float)0.0;
	

	if NewDayAndPlantGrowing
	{                   
	 int L;
	 float fExtWat;

	 PSWATER		pSWL=pSo->pSWater->pNext;
	 PWLAYER		pSLW=pWa->pWLayer->pNext; 
	 PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;

	 //SG 20111122:
	 // pPl->pRoot->fUptakeR wird hier mit fActTraDay vom Vortag berechnet, da dllEPPhotosynthesis_GECROS()
	 // erst nach dllEPActTransp_GECROS() aufgerufen wird
     
	 pPl->pRoot->fUptakeR =(float)0.0;
// SG 20111107: pPl->pPltWater->fPotTranspDay und pPl->pPltWater->fActTranspDay dürfen nicht ausserhalb von 
//              balance.c gesetzt werden (werden dort in jedem Zeitschritt aufaddiert!)
//	 pPl->pPltWater->fPotTranspDay = fPotTraDay;
	 pPl->pPltWater->fPotTranspDay = (float)0.0;
	 pPl->pPltWater->fActTranspDay = (float)0.0;
		
	 for (L=1;L<=pSo->iLayers-2;L++)
	 {
		 //SG 20111107: pLR->fActLayWatUpt auf 0 initialisieren
		 pLR->fActLayWatUpt = (float)0.0;
		 if(pPl->pRoot->fDepth > (float)0)
		 //if((pPl->pRoot->fDepth > (float)0)&& (pSLW->fContAct > pSWL->fContPWP + (float)0.002))
		 //if((pPl->pRoot->fDepth > (float)0)&& (pSLW->fContAct > (float)0.05))
		 {
          if(L*pSo->fDeltaZ<=pPl->pRoot->fDepth*(float)10)
		  {
	        pLR->fActLayWatUpt = fActTraDay*(float)min((float)1,pSo->fDeltaZ/(pPl->pRoot->fDepth*(float)10));

		    //SG 20111122: es darf nur bis zum PWP entleert werden
			fExtWat = max(0.0,pSLW->fContAct-pSWL->fContPWP);
	        pLR->fActLayWatUpt = (float)min(fExtWat, pLR->fActLayWatUpt);
		  }
	      else if (((L-1)*pSo->fDeltaZ<=pPl->pRoot->fDepth*(float)10)
			      &&(pPl->pRoot->fDepth*(float)10 < L*pSo->fDeltaZ))
		  {
            //SG 20111107: Anteil Wasseraufnahme der untersten durchwurzelten Schicht
			//pLR->fActLayWatUpt = fActTraDay*(float)min((float)1,
			//	               (L*pSo->fDeltaZ-pPl->pRoot->fDepth*(float)10)/(pPl->pRoot->fDepth*(float)10));
	        pLR->fActLayWatUpt = fActTraDay*(float)min((float)1,
				               (pPl->pRoot->fDepth*(float)10 - (L-1)*pSo->fDeltaZ)/(pPl->pRoot->fDepth*(float)10));

            //SG 20111122: es darf nur bis zum PWP entleert werden
			fExtWat = max(0.0,pSLW->fContAct-pSWL->fContPWP);
	        pLR->fActLayWatUpt = (float)min(fExtWat, pLR->fActLayWatUpt);
		  }
		 
		 }
		 else
	      pLR->fActLayWatUpt = (float)0;
	 
	  pPl->pRoot->fUptakeR += pLR->fActLayWatUpt;
	  afActLayRootWatUpt[L]=pLR->fActLayWatUpt;
      
	  pSWL=pSWL->pNext;
      pSLW=pSLW->pNext;
	  pLR =pLR ->pNext;
	 }

		   
	}	//	end if NewDayAndPlantGrowing


    
	if ((PlantIsGrowing)&&(jGECROS==(int)0))
    {

//				PSLAYER		pSL	=pSo->pSLayer->pNext;
				PWLAYER		pSLW=pWa->pWLayer->pNext; 
//				PLAYERROOT 	pLR	=pPl->pRoot->pLayerRoot;

				int L;
                ///*
			    for (L=1;L<=pSo->iLayers-2;L++)
			    	{
					pSLW->fContAct -= afActLayRootWatUpt[L]
						              /pSo->fDeltaZ
									  *pTi->pTimeStep->fAct; //mm/mm
                    
//			      	pSL =pSL ->pNext;
			      	pSLW=pSLW->pNext;
//			      	pLR =pLR ->pNext;
					}
               //*/

     } //end if plant is growing   	
    


   	return 1;
	}	
  

//=============================================================================================
// DLL-Function:  dllEPBiomassGrowth_GECROS
// Description:   Maitenance and biomass growth (Wang,1997)                                   
// Updating Date: 07.04.2008/ep                   
//=============================================================================================
int WINAPI  dllEPBiomassGrowth_GECROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR

  if SimulationStarts
       introduce((LPSTR)"dllEPBiomassGrowth_GECROS");

  if (SimulationStarts && (iGECROS <= (int)0))
  {
   allocateGECROSVariables();
   iGECROS =(int)3;
  }
 
  if (SimulationStarts && (iGECROS > (int)0))
  {
   iGECROS = (int)3;
  }

  ///*
  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
      BiomassGrowth_GECROS(exp_p);
    }
  //*/


  if (SimulationEnds && (iGECROS == (int)3)) 
  {
   freeAllocatedGECROSMemory();
   iGECROS = (int)0;
  }


     return 1;
  }


//=============================================================================================
// DLL-Function:  dllEPCanopyForm_GECROS
// Description:   Canopy formation module                                    
// Updating Date: 07.04.08/ep                   
//=============================================================================================
int WINAPI  dllEPCanopyForm_GECROS(EXP_POINTER)
  {
  DECLARE_COMMON_VAR
 
  if SimulationStarts
       introduce((LPSTR)"dllEPCanopyForm_GECROS");

  if (SimulationStarts && (iGECROS <= (int)0))
  {
   allocateGECROSVariables();
   iGECROS =(int)4;
  }
 
  if (SimulationStarts && (iGECROS > (int)0))
  {
   iGECROS = (int)4;
  }

  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
	  {
	   CanopyFormation_GECROS(exp_p);
	  }
    }

  if (SimulationEnds && (iGECROS == (int)4)) 
  {
   freeAllocatedGECROSMemory();
   iGECROS = (int)0;
  }


  return 1;
  }

//=============================================================================================
// DLL-Function:  dllSGOrganSenesz_GECROS
// Description:   Canopy formation module                                    
// Updating Date: 07.04.08/ep                   
//=============================================================================================
int WINAPI  dllEPOrganSenesz_GECROS(EXP_POINTER)
  {
    DECLARE_COMMON_VAR
  
  	int 	L,L1,NLAYR;
    float   fCumDepth,TRLDF,fThickness;
	float	fDeadRootC, fDeadRootN;

	PLAYERROOT		pLR	  = pPl->pRoot->pLayerRoot;
	PSLAYER 		pSL	  = pSo->pSLayer;
    PCLAYER			pCL;

	PGECROSCARBON   pGPltC = pGecrosPlant->pGecrosCarbon;
	PGECROSNITROGEN pGPltN = pGecrosPlant->pGecrosNitrogen;

 
  if SimulationStarts
       introduce((LPSTR)"dllEPOrganSenesz_GECROS");

  if (SimulationStarts && (iGECROS <= (int)0))
  {
   allocateGECROSVariables();
   iGECROS =(int)10;
  }
 
  if (SimulationStarts && (iGECROS > (int)0))
  {
   iGECROS = (int)10;
  }


  if NewDayAndPlantGrowing
    {
    //Wird nur berechnet wenn der Feldaufgang erreicht wurde 
    if (pPl->pDevelop->iDayAftEmerg > 0)
		{
			OrganSenescence_GECROS(exp_p);

	        // Dead biomass to soil litter pools (sg 2005/11/14)

			//leaves
			pCh->pCProfile->fCLitterSurf += pGPltC->fCLeafLossR;
			pCh->pCProfile->fNLitterSurf += pGPltN->fNLeafLossR;

			//roots
			//if(pPl->pDevelop->fStageSUCROS>=pPl->pGenotype->fBeginSenesDvs)
			{
				NLAYR=pSo->iLayers-2;
				L = 0;
				fCumDepth = (float)0.0;

				while ((fCumDepth<=pPl->pRoot->fDepth)&&(L<NLAYR))
				{
					L++;
					fThickness =(float)0.1*pSL->fThickness; //cm
					fCumDepth +=(float)0.1*pSL->fThickness; //cm
					if (L<NLAYR-1) pSL =pSL ->pNext;
				}
				L1 = L;

				//Total Root Length Density Factor
				TRLDF =(float)0.0;
				pLR	  =pPl->pRoot->pLayerRoot;
				for (L=1;L<=L1;L++)
				{
					TRLDF += pLR->fLengthDensFac;
					pLR    = pLR->pNext;
				}

				fDeadRootC = pGPltC->fCRootLossR;
				fDeadRootN = pGPltN->fNRootLossR;

				pSL	= pSo->pSLayer->pNext;
				pLR	= pPl->pRoot->pLayerRoot;
				pCL = pCh->pCLayer->pNext;
				for (L=1;L<=L1;L++)
				{
					fThickness =(float)0.1*pSL->fThickness; //cm
			        
					if(TRLDF > (float)0.0)
					{
						pCL->fCLitter += fDeadRootC * pLR->fLengthDensFac/TRLDF;
						pCL->fNLitter += fDeadRootN * pLR->fLengthDensFac/TRLDF;
					}

					pSL = pSL->pNext;
					pLR = pLR->pNext;
					pCL = pCL->pNext;
				} // End for(...)
			} //End dead root weight to soil litter pools
		} // End iDayAftEmerg > 0
    }



  if (SimulationEnds && (iGECROS == (int)10)) 
  {
   freeAllocatedGECROSMemory();
   iGECROS = (int)0;
  }

    
  return 1;
  }
  
  //=============================================================================================
// DLL-Function:	dllEPNitrogenDemand_GECROS
// Description:		calculation of nitrogen demand  			                            
// Updating Date:  	07.04.08/ep                   
//=============================================================================================
int WINAPI dllEPNitrogenDemand_GECROS(EXP_POINTER)
	{
	DECLARE_COMMON_VAR
 
	if SimulationStarts
       introduce((LPSTR)"dllEPNitrogenDemand_GECROS");

    if (SimulationStarts && (iGECROS <= (int)0))
    {
     allocateGECROSVariables();
     iGECROS =(int)8;
    }
 
    if (SimulationStarts && (iGECROS > (int)0))
    {
     iGECROS = (int)8;
    }

    ///*
	if NewDayAndPlantGrowing
		{
		//Wird nur berechnet wenn der Feldaufgang erreicht wurde 
		if (pPl->pDevelop->iDayAftEmerg > 0)
		 {
          NitrogenFixation_GECROS(exp_p);
          PotentialNitrogenUptake_GECROS(exp_p);
		 }
		}
    //*/

    if (SimulationEnds && (iGECROS == (int)8)) 
    {
     freeAllocatedGECROSMemory();
     iGECROS = (int)0;
    }

	return 1;
	}

//=============================================================================================
// DLL-Function:	dllEPNitrogenUptake_GECROS
// Description:		calculation of actual nitrogen 			                            
// Updating Date:  	07.04.08/ep                   
//=============================================================================================
int WINAPI dllEPNitrogenUptake_GECROS(EXP_POINTER)
	{
	DECLARE_COMMON_VAR
 
	if SimulationStarts
       introduce((LPSTR)"dllEPNitrogenUptake_GECROS");

    if (SimulationStarts && (iGECROS <= (int)0))
    {
     allocateGECROSVariables();
     iGECROS =(int)9;
    }
 
    if (SimulationStarts && (iGECROS > (int)0))
    {
     iGECROS = (int)9;
    }

    ///*
	if NewDayAndPlantGrowing
		{
		ActualNitrogenUptake_GECROS(exp_p);
		}
    //*/

    if (SimulationEnds && (iGECROS == (int)9)) 
    {
     freeAllocatedGECROSMemory();
     iGECROS = (int)0;
    }



    ///*
    if PlantIsGrowing
    	{
		int L;
		PLAYERROOT	pLR	=pPl->pRoot->pLayerRoot;
		PCLAYER 	pSLN=pCh->pCLayer->pNext;

		for (L=1;L<=pSo->iLayers-2;L++)
			{
		    //Check the whether there are roots in this layer:
		    //if (pLR->fLengthDens==(float)0.0)		break;

			//Nitrogen in layer L: SNO3,SNH4 (kg N/ha)
			pSLN->fNO3N=pSLN->fNO3N-pLR->fActLayNO3NUpt*pTi->pTimeStep->fAct;
			pSLN->fNH4N=pSLN->fNH4N-pLR->fActLayNH4NUpt*pTi->pTimeStep->fAct;
			//pSLN->fNO3N=pSLN->fNO3N-min(pSLN->fNO3N,pLR->fActLayNO3NUpt)*pTi->pTimeStep->fAct;
			//pSLN->fNH4N=pSLN->fNH4N-min(pSLN->fNH4N,pLR->fActLayNH4NUpt)*pTi->pTimeStep->fAct;

			pLR =pLR ->pNext;
			pSLN=pSLN->pNext;
			}
    	}
    //*/
	return 1;
	}

