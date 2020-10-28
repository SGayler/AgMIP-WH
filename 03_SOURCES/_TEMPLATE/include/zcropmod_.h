/*******************************************************************************
 *
 * Copyright  (c) 
 *
 * Author:  
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 6 $
 *
 * $History: zcropmod.h $
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 23.01.02   Time: 14:08
 * Updated in $/Projekte/ExpertN/ModLib/include
 * Anbindung an Datenbank auch unter Win XP möglich. Fitting in Gang
 * gesetzt.
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:10
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:34
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * Removed EPSILON (already defined in one of the defines file)
 * 
*******************************************************************************/

#include <windows.h>
#include "defines.h"

/************************************************************************************************************
     Extra Variables not founded in EXPERT_N System Variables
*************************************************************************************************************/
// #define CALLBACK _loadds   CALLBACK _loadds  

#define C3    "C3"
#define C4    "C4"
#define CAM    "CAM"

#define COOL    "COOL"
#define TEMPERATE  "TEMPERATE"
#define WARM    "WARM"
#define TROPICAL  "TROPICAL"

#define LEGUME    "LEGUME"
#define WHEAT    "WHEAT"
#define BARLEY    "BARLEY"
#define MAIZE    "MAIZE"
#define MILLET    "MILLET"
#define SORGHUM    "SORGHUM"
#define POTATO    "POTATO"
#define SOYBEAN    "SOYBEAN"
#define RICE    "RICE"
#define COTTON    "COTTON"
#define GROUNDNUT  "GROUNDNUT"
#define SWEETPOTATO  "SWEETPOTATO"
#define SUNFLOWER  "SUNFLOWER"
#define SUGARBEET  "SUGARBEET"
#define SUGARCANE  "SUGARCANE"
#define CASSAVA    "CASSAVA"

#define LDP      "Long Day Plant"
#define SDP      "Short Day Plant"
#define DNP      "Daylength neutral plant"
#define IDP      "Intermediate daylength plant"
#define DIP      "Daylength Insensitive plant"

#define GET_DATA  "GET"
#define SET_DATA  "SET"
#define SET_ALL    "SET ALL"

#define GET_PLANTHEIGHT    0
#define GET_LEAFWIDTH    1

#define GET_DAYLENGTH    0
#define GET_PHOTOPERIOD    1
  
#define GET_DAILYAVERAGE  0
#define GET_DAYTIME      1
#define GET_NIGHTTIME    2

char pCrop[40];

#define CROP_IS_(X)    ((!lstrcmp(pCrop,X)) ? TRUE : FALSE)
#define EQUAL(name,X)  ((!lstrcmp(name, X)) ? TRUE : FALSE) 


#define CROP_IS_CEREAL     ((!lstrcmp((LPSTR)pCrop,(LPSTR)WHEAT))||   \
              (!lstrcmp((LPSTR)pCrop,(LPSTR)BARLEY)))



// #define _AND_ &&
// #define _OR_  ||


typedef struct ZSowInformation  * PZSOWINFO;
typedef struct ZSowInformation  //Maximum and actual transpiration
  {
  float  fSeedDepth;      
  float  fSeepDensity;     
  float  fRowWidth;     
  }
  ZSOWINFO;


typedef struct ZTranspiration  * PZTRP;
typedef struct ZTranspiration  //Maximum and actual transpiration
  {
  float  fPotTransp;      //Potential transpiration of the crop  (mm)
  float  fActTransp;     //Actual transpiration of the crop     (mm)
  float  fInterceptedWater; 
  }
  ZTRP;

#define PZTRANSPIRATION PZTRP
#define ZTRANSPIRATION  ZTRP

typedef struct ZOrganNitrogen  * PZN;
typedef struct ZOrganNitrogen
  {
  float  fMaxNc;          //Maximum nitrogen concentration at luxus nitrogen level (%)
  float  fOptNc;       //Optimum nitrogen concentration below which nitrogen stress occurs (%)
  float  fMinNc;        //Minimum nitrogen concentration below which no growth occurs (%)
  float  fActNc;        //Actual  nitrogen concentration of the organ (%)
  float  fDemand;      //The maximum value of fOptNc
  }
  ZN;

#define PZORGANNITROGEN PZN
#define ZORGANNITROGEN  ZN
 

typedef struct ZPlantNitrogen  * PZPLTN;
typedef struct ZPlantNitrogen
  {     
  ZN Leaf;
  ZN Stem;
  ZN Root;
  ZN Storage; 
  float fDemand;
  }
  ZPLTN;

#define ZPLANTNITROGEN   ZPLTN
#define PZPLANTNITROGEN  PZPLTN

typedef struct ZPlantNc  * PZPLTNC;
typedef struct ZPlantNc
  {     
  float fLeaf;
  float fStem;
  float fRoot;
  float fStorage;
  }
  ZPLTNC;

typedef struct ZResponse  * PZRESPONSE;
typedef struct ZResponse
  {
  float  fX;
  float  fY;
  }
  ZRESPONSE;

typedef PZRESPONSE  PZRSPNS;

typedef struct ZPartitionVsDvs  * PZPARTDVS;
typedef struct ZPartitionVsDvs
  {
  ZRESPONSE AssimPartRoot[20];
  ZRESPONSE AssimPartLeaf[20];
  ZRESPONSE AssimPartStem[20];
  ZRESPONSE AssimPartStorage[20];
  }
  ZPARTDVS;


typedef struct ZStage  * PZSTAGE;
typedef struct ZStage
  {
  float  fStageVR;  //The internal VR stage value (-1.0 - 2.0)
  float  fStageEC;   //The EC stage value (0-10) as output
  float  fStageWE;    //The WE stage value (0.0-10.0)
  float  fStageAX;  //The auxiliary stage value
  float  fCumPVDs;   //Cumulative vernalization days
  float  fDevRate;
  float  fThermEff;
  float  fPhotopEff;
  float  fVernEff;
  }
  ZSTAGE;

typedef PZSTAGE  PZDVS;
typedef ZSTAGE  ZDVS;

typedef struct ZPhotosynthesisParam  * PZPHOTOSYNPARAM;
typedef struct ZPhotosynthesisParam
  {
  char  pType[30];
  float  fPgmmax;
  float  fMaxLUE;
  float  fCO2Cmp;
  float  fCO2R;
  float  fMinTmp;
  float  fOptTmp;
  float  fMaxTmp;
  float  fKDIF;
  PZRESPONSE pTmpRsp;
  }
  ZPHOTOSYNPARAM;

typedef PZPHOTOSYNPARAM    PZPHP;
typedef ZPHOTOSYNPARAM    ZPHP;


typedef struct ZCarbonExchange  * PZCO2;
typedef struct ZCarbonExchange
  {
  float   fGrossPhotosynthesis;
  float   fNetPhotosynthesis;
  float   fMaintenanceRespiration;
  float   fGrowthRespiration;
  float  fLeafMaintenance;
  float  fStemMaintenance;
  float  fRootMaintenance;
  float  fStorageMaintenance;
  float  fLeafGrowthRespiration;
  float  fStemGrowthRespiration;
  float  fRootGrowthRespiration;
  float  fStorageGrowthRespiration;
  }
  ZCO2;


typedef struct ZDevelopmentParam  * PZDEVPARAM;
typedef struct ZDevelopmentParam
  {
  char  pType[30];
  float  fPDD2;
  float  fPhotopSens;
  float  fVernReq;

  char  pDaylengthType[30];
  float  fOptPhotop;
  float  fPDD1;
  float  fMinTmp1;
  float  fOptTmp1;
  float  fMaxTmp1;
  float  fMinTmp2;
  float  fOptTmp2;
  float  fMaxTmp2;

  float   fMinTmpVn;
  float   fOptTmpVn;
  float  fMaxTmpVn;
  
  float   fSprGrwRate;
  float    fSeedDepth;
  float   fSeedDensity;
  
  PZRESPONSE pTmpRsp1;
  PZRESPONSE pTmpRsp2;
  PZRESPONSE pTmpRspVn;
  PZRESPONSE pPhpRsp;
  }
  ZDEVPARAM;

typedef PZDEVPARAM  PZDEVP;
typedef ZDEVPARAM  ZDEVP;

typedef struct ZOrgans  * PZORGAN;
typedef struct ZOrgans
  {
  float  fMainStemLeaves;  //Leaf number on main stem
  float  fMainStemNodes;    //Node number on main stem
  float  fTillers;      //Tiller number per plant
  float  fStorageOrgans;    //Number of storage organs per plant
  }
  ZORGAN;

typedef struct ZCanopy  * PZCAN;
typedef struct ZCanopy
  {
  float  fLAI;
  float  fGrwLAI;
  float  fDieLAI;
  float  fTotalLAI;
  float  fPltHeight;
  ZORGAN  Organs;
  }
  ZCAN;

typedef PZCAN  PZCANOPY;
typedef ZCAN  ZCANOPY;

typedef struct ZAssimilatePartition  * PZPART;
typedef struct ZAssimilatePartition
  {
  float  fLeaf;
  float  fStem;
  float  fRoot;
  float  fStorage;
  }
  ZPART;

typedef struct ZGrowth  * PZBIOM;
typedef struct ZGrowth
  {
  float  fBmLeaf;
  float  fBmStem;
  float  fBmRoot;
  float  fBmStorage;
  float  fBmReserve;
  float  fBmDeadLeaf;
  float  fBmDeadStem;
  float  fBmDeadRoot;
  float  fGrwLeaf;
  float  fGrwStem;
  float  fGrwRoot;
  float  fGrwStorage;
  float  fGrwReserve;
  float  fDieLeaf;
  float  fDieStem;
  float  fDieRoot;
  float  fDieStorage;
  }
  ZBIOM;

typedef ZBIOM  ZGROWTH;
typedef PZBIOM  PZGROWTH;

typedef struct ZBiomass  * PZBM;
typedef struct ZBiomass
  {
  float  fLeaf;
  float  fStem;
  float  fRoot;
  float  fStorage;
  float  fDeadLeaf;
  }
  ZBM;

typedef ZBM    ZBIOMASS;
typedef PZBM  PZBIOMASS;

typedef struct ZMaintenance  * PZMAINT;
typedef struct ZMaintenance
  {
  float  fLeaf;
  float  fStem;
  float  fRoot;
  float  fStorage;  
  float  fMetab;
  float  fTotal;
  }
  ZMAINT;

typedef struct ZGrowthEfficiency  * PZGRWEFF;
typedef struct ZGrowthEfficiency
  {
  float  fEffLeaf;
  float  fEffStem;
  float  fEffRoot;
  float  fEffStorage;  
  float  fCO2Leaf;
  float  fCO2Stem;
  float  fCO2Root;
  float  fCO2Storage;
  }
  ZGRWEFF;

typedef struct ZRootParam  * PZROOTPARAM;
typedef struct ZRootParam
  {
  float  fSpcRootLength;
  float  fMaxDepth;
  float  fMaxExtRate;
  float  fMinTmp;
  float  fOptTmp;
  float  fMaxTmp;
  float  fMaxUptRateWater;
  float  fMaxUptRateNitrogen; 
  float  fRelDeathRate;
  float  fUptCoeffNO3;
  float  fUptCoeffNH4;
  }
  ZROOTPARAM;

typedef PZROOTPARAM   PZRTPARAM;
typedef ZROOTPARAM     ZRTPARAM;


typedef struct ZRoot  * PZROOT;
typedef struct ZRoot
  {
  float  fDepth;
  float  fExtensionRate;
  float   fMaxDepth;
  float  fWaterUptake;
  float  fNitrogenUptake;
  float  fNO3Uptake;
  float  fNH4Uptake;
  float  fCumWaterUptake;
  float  fCumNitrogenUptake;
  float  fLengthDensity[MAXSCHICHT];
  float  fGrwLength[MAXSCHICHT];
  float  fDieLength[MAXSCHICHT];
  float  fLayerWaterUptake[MAXSCHICHT];
  float  fLayerNitrogenUptake[MAXSCHICHT];
  float  fLayerNO3Uptake[MAXSCHICHT];
  float  fLayerNH4Uptake[MAXSCHICHT];
  }
  ZROOT;


typedef struct ZSoilType  * PZSOILTYPE;
typedef struct ZSoilType
  {
  float  fThickness;
  float  fBulkDensity;
  float  fSatWaterContent;
  float  fFieldCapacity;
  float  fWiltingPoint;
  float  fpH;
  }
  ZSOILTYPE;


typedef struct ZSoil  * PZSOIL;
typedef struct ZSoil
  {
  ZSOILTYPE  sType[MAXSCHICHT];
  float    fTemperature[MAXSCHICHT];
  float    fWaterContent[MAXSCHICHT];
  float    fNO3N[MAXSCHICHT];
  float    fNH4N[MAXSCHICHT];
  float    fNitrogenContent[MAXSCHICHT];
  }
  ZSOIL;




//================== Climate and Weather Part ==========================================
  
float CALLBACK _loadds DaylengthAndPhotoperiod(float fLatitude, int nJulianDay, int nID);
float CALLBACK _loadds ExtraterrestrialRadiation(float fLatitude, int nJulianDay);
float CALLBACK _loadds DailyAverageTemperature(float fTmpMax,float fTmpMin,int nID);
float CALLBACK _loadds HourlyTemperature(int iOclock, float fTempMax, float fTempMin);


//================== Phasic Development Part ==========================================
float CALLBACK _loadds RelativeTemperatureResponse(float fTemp, float fMinTmp, float fOptTmp, float fMaxTmp);

float CALLBACK _loadds PhenologicalDevelopment(LPSTR pCrop,float fTmpMax,float fTmpMin, float fPhotop,
                       PZDVS pStage,PZSOWINFO pSowInfo,PZDEVP pDevPa);

float CALLBACK _loadds DevelopmentBeforeEmergence(LPSTR pCrop,float fTmpMax,float fTmpMin,
                     PZDVS pStage,PZSOWINFO pSowInfo,PZDEVP pDevPa);

float CALLBACK _loadds DevelopmentAfterEmergence(LPSTR pCrop,float fTmpMax,float fTmpMin,float fPhotop,
                    PZDVS pStage,PZDEVP pDevPa);

float CALLBACK _loadds ThermalEffect(float fTmpMin,float fTmpMax,float fMinTmp,float fOptTmp,float fMaxTmp,PZRESPONSE pResp);
float CALLBACK _loadds VernalizationEffect(LPSTR pCrop,float fTmpMin,float fTmpMax,PZDVS pStage,PZDEVP pDevPa);
float CALLBACK _loadds DailyVernalization(float fTmpMin,float fTmpMax,float fMinTmp,float fOptTmp,float fMaxTmp,PZRESPONSE pResp);
float CALLBACK _loadds PhotoperiodismEffect(LPSTR pType, float fDaylen,float fOptDaylen,float fSensF,PZRESPONSE pResp);

int    CALLBACK _loadds GetDefaultDevelopmentParam(LPSTR pCrop,PZDEVP pDevPa);

float CALLBACK _loadds DevelopmentSpecial_Default(LPSTR,PZDVS,float,float,float,PZDEVP);
float CALLBACK _loadds DevelopmentSpecial_Potato (LPSTR,PZDVS,float,float,float,PZDEVP);
float CALLBACK _loadds DevelopmentSpecial_Wheat  (LPSTR,PZDVS,float,float,float,PZDEVP);


//================== Photosynthesis Part===============================================
float CALLBACK _loadds MomentaryCanopyGrossPhotosythesis(LPSTR,float,float,int,float,float,float,float,float,float,PZPHP);
float CALLBACK _loadds DailyCanopyGrossPhotosythesis(LPSTR pCrop,float fLAI, float fLatitude,int nDay,
               float fRad,float fTmpMax,float fTmpMin,float fCO2,float fLfNc,PZPHP pPh);

int   CALLBACK _loadds GetDefaultPhotosynthesisParam(LPSTR,PZPHOTOSYNPARAM);
float CALLBACK _loadds LeafMaxGrossPhotosynthesis(LPSTR,float,float,float,PZPHP); 
float CALLBACK _loadds LeafLightUseEfficiency(LPSTR,float, float,PZPHP);


//================== Repiration and Biomass Growth ====================================

int CALLBACK _loadds AssimilatePartitioning(LPSTR,float,float,PZTRP,PZPART);
int CALLBACK _loadds DefaultAssimilatePartitioning(LPSTR,float,float,PZTRP,PZPART);
int CALLBACK _loadds GetDefaultCropMaintenanceCoefficient(LPSTR,PZMAINT);
int CALLBACK _loadds  GetDefaultGrowthEfficiency(LPSTR,PZGRWEFF);

float CALLBACK _loadds CropMaintenance(LPSTR,float,float,float,PZBIOMASS,PZPLTNC,PZMAINT);
float CALLBACK _loadds OrganBiomassGrowth(LPSTR,float,float,float,float,float,PZTRP,PZPLTNC,PZPART,PZGROWTH,PZCO2);
float CALLBACK _loadds ReserveGrowth(LPSTR pCrop,float fStageVR,PZBIOMASS pBiom);

float CALLBACK _loadds GetDailyLeafMaintenance(LPSTR pCrop,float fLAI,float fStageVR,float fTemp, float fLfNc);

float CALLBACK _loadds BiomassGrowth(LPSTR     pCrop,
              float     fStageVR, 
              float     fStageAX,
              float     fGrossPs,
              float     fTmpAve,
              PZBIOMASS  pBiom,
              PZTRP     pTransp,
              PZPLTNC   pPltNc,
              PZGROWTH  pGrw,
              PZCO2     pCO2);
//================== Canopy Formation and Senecence ====================================

float CALLBACK _loadds LeafAreaGrowth(LPSTR pCrop,float fLeafGrwRate,float fStageVR,PZTRP pTransp,float fLfNc);
float CALLBACK _loadds CanopyFormation(LPSTR pCrop, float fStageVR, float fLeafGrwRate,float fStemGrwRate,
             PZTRP pTransp,PZPLTNC pPltNc,PZCAN pCan);


float CALLBACK _loadds GetDefaultSpecificLeafArea(LPSTR,float);
float CALLBACK _loadds GetDefaultPlantSize(LPSTR pCrop,float fStageVR,int nID);

int CALLBACK _loadds Senescence(LPSTR pCrop,float fStageVR,PZTRP pTransp,PZPLTNC pPltNc,
           PZGROWTH pGrw,PZCAN pCan,PZROOT pRt);

//================== Root System Formation =============================================
int CALLBACK _loadds RootSystemFormation(LPSTR pCrop,float fRootGrwRate,PZROOT pRt,PZSOIL pSoil);
int CALLBACK _loadds  RootExtension(LPSTR pCrop,float fRootGrwRate,PZROOT pRt,PZSOIL pSoil);
int CALLBACK _loadds  RootLengthDensity(LPSTR pCrop,float fRootGrwRate,PZROOT pRt,PZSOIL pSoil);

int CALLBACK _loadds GetDefaultRootParam(LPSTR pCrop,PZROOTPARAM pRtParam);

//================== Water Uptake ======================================================
int CALLBACK _loadds DailyWaterCapture(LPSTR pCrop,float fLAI,float fStageVR,float fLat,float fAlt,int nDay,
            float fRad,float fTmpMax,float fTmpMin,float fRH,float fWind,float fRainfall,float fCO2,
           PZSOIL pSoil,PZROOT pRt,PZTRP pTransp,float fLfNc,PZPHP pPhP);

float CALLBACK _loadds DailyCanopyWaterInterception(float fLAI,float fRainfall);
float CALLBACK _loadds DailyPotentialTranspiration(LPSTR pCrop,float fLAI,float fStageVR, 
            float fLat,float fAlt,int nDay,
            float fRad,float fTmpMax,float fTmpMin,float fRH,float fWind,float fCO2,
            float fLfNc,PZPHP pPhP);
int CALLBACK _loadds DailyRootWaterUptake(LPSTR pCrop,float fLAI,float fPotTransp,PZROOT pRt, PZSOIL pSoil);

float CALLBACK _loadds AverageLeafBoundaryLayerResistance(LPSTR pCrop,float fWind);
float CALLBACK _loadds AverageCanopyTurbulentResistance(LPSTR pCrop,float fLAI,float fStageVR,float fWind);
float CALLBACK _loadds AverageMinimumLeafResistance(LPSTR pCrop,float fLAI,float fStageVR,
                  float fMaxCrpPs,float fMaxLfPs,float fMaintLf,
                  float fDayl,float fTmpMax, float fTmpMin,float fWind,float fCO2);

//================== Nitrogen Uptake ====================================================
float CALLBACK _loadds DailyNitrogenCapture(LPSTR pCrop,float fStageVR,PZGROWTH pGrw,PZPLTN pPltN,PZROOT pRt,PZSOIL pSoil);

float CALLBACK _loadds DailyNitrogenDemand(LPSTR pCrop, float fStageVR, PZBIOM pBm, PZPLTN pPltN);
float CALLBACK _loadds DailyNitrogenUptake(LPSTR pCrop,float fNDemand, PZROOT pRt,PZSOIL pSoil);
float CALLBACK _loadds DailyNitrogenTranslocation(LPSTR pCrop,float fStageVR,float fNitrogenUptake,PZBIOM pBm,PZPLTN pPltN);
float CALLBACK _loadds DailyNitrogenFixation(LPSTR pCrop,PZROOT pRt);

int   CALLBACK _loadds GetDefaultPlantNitrogenConcentrationLimits(LPSTR pCrop, float fStageVR, PZPLTN pPltN);


//================== Summary Function Part =================================================
int CALLBACK _loadds Initialization(LPSTR pCrop, int nSowOrEmerg, PZSOWINFO pSowInfo,
             PZDVS pStg,PZGROWTH pGrw,PZCANOPY pCan,
             PZROOT pRt, PZPLTNC pPltNc,PZSOIL pSoil);


//================== Tool Function Part =================================================
int CALLBACK _loadds GetOrganBiomass(PZBIOMASS pBiom, PZGROWTH pGrw);
int CALLBACK _loadds GetOrganNitrogenConcentration(PZPLTNC pPltNc,PZPLTN pPltN);
int CALLBACK _loadds SetOrganNitrogenConcentration(float fLfNc,float fStNc,float fRtNc,float fSoNc,PZPLTN pPltN);

float  CALLBACK _loadds ZFGENERATOR(float fInput, ZRESPONSE* pfResp);
float  CALLBACK _loadds ZFGENERATOR1(float fInput, double N,double* X, double* Y);
double CALLBACK _loadds ZLIMIT(double v1, double v2, double x); 

/*******************************************************************************
** EOF */
