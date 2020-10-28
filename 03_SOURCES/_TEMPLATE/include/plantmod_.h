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
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: plantmod.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:05
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * 
*******************************************************************************/

#ifndef _INC_PROJECTNAME_FILENAME_EXT
#define _INC_PROJECTNAME_FILENAME_EXT

#include  "xinclexp.h"
#include "EXPORTS.H"

#define EXPERT_N_POINTERS 	PCHEMISTRY pCh,  PCLIMATE pCl,PGRAPHIC  pGr, PHEAT pHe, PLOCATION pLo,\
							PMANAGEMENT pMa, PPLANT   pPl,PSPROFILE pSo, PTIME pTi, PWATER    pWa

#define exp_n_pointers		pCh,pCl,pGr,pHe,pLo,pMa,pPl,pSo,pTi,pWa

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Module Global Variables
HANDLE 	hModuleInstance; 

float	PLSC[20];

/************************************************************************************************************
     Extra Variables not founded in EXPERT_N System Variables
*************************************************************************************************************/
#define WHEAT		  "WHEAT"
#define BARLEY		"BARLEY"
#define MAIZE		  "MAIZE"
#define MILLET		"MILLET"
#define SORGHUM		"SORGHUM"
#define POTATO		"POTATO"
#define SUGARBEET	"SUGARBEET"
#define SOYBEAN		"SOYBEAN"
#define RICE		  "RICE"
#define COTTON		"COTTON"


#define CROP_IS_WHEAT   	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)WHEAT))
#define CROP_IS_BARLEY   	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)BARLEY))
#define CROP_IS_MAIZE   	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)MAIZE))
#define CROP_IS_MILLET   	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)MILLET))
#define CROP_IS_SORGUM  	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)SORGHUM))
#define CROP_IS_POTATO   	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)POTATO))
#define CROP_IS_SUGARBEET   (!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)SUGARBEET))
#define CROP_IS_SOYBEAN   	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)SOYBEAN))
#define CROP_IS_RICE   		(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)RICE))
#define CROP_IS_COTTON   	(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)COTTON))

#define CROP_IS_CEREAL   	((!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)WHEAT))||   \
							(!lstrcmp((LPSTR)GenParam.acTypeName,(LPSTR)BARLEY)))


// #define _AND_ &&
// #define _OR_  ||

typedef struct Outputs  * POUTPUT;
typedef struct Outputs
{
	float	fValue0;
	float	fValue1;
	float	fValue2;
	float	fValue3;
	float	fValue4;
	float	fValue5;
	float	fValue6;
	float	fValue7;
	float	fValue8;
	float	fValue9;
}
OUTPUT;


typedef struct CardinalTemperature  * PCARDTEMP;
typedef struct CardinalTemperature	//Cardinal temperture of biological processes
{
	float	fMinTemp;		//Minimum temperature (C)
	float	fOptTemp; 		//Optimal temperature (C)
	float	fMaxTemp; 		//Maximum temperature (C)
	float	fOptTempRange; 	//Range of the optimal temperature (from fOptTemp to fOptTemp+fOptTempRange)
}
CARDTEMP;

#define PCARDINALTEMPERATURES PCARDTEMP
#define CARDINALTEMPERATURE	  CARDTEMP

typedef struct Transpiration  * PTRANSP;
typedef struct Transpiration	//Maximum and actual transpiration
{
	float	fPotTransp;			//Potential transpiration of the crop  (mm)
	float	fActTransp; 		//Actual transpiration of the crop     (mm)
}
TRANSP;

#define PTRANSPIRATION PTRANSP
#define TRANSPIRATION  TRANSP

typedef struct OrganNitrogenConcentration  * PORGANNC;
typedef struct OrganNitrogenConcentration
{
	float	fMaxNc;    			//Maximum nitrogen concentration at luxus nitrogen level (%)
	float	fOptNc; 			//Optimum nitrogen concentration below which nitrogen stress occurs (%)
	float	fMinNc;  			//Minimum nitrogen concentration below which no growth occurs (%)
	float	fActNc;  			//Actual  nitrogen concentration of the organ (%)
	float	fMaxOptNc;			//The maximum value of fOptNc
}
ORGANNC;

#define PORGANNITROGENCONCENTRATION  PORGANNC
#define ORGANNITROGENCONCENTRATION   ORGANNC

typedef struct DevStage  * PDEVSTAGE;
typedef struct DevStage
{
	float	fStageVR;    			//Vegetative-Reproductive development stage (0-1-2)
	float	fStageDSC; 				//Development stage for cereals (0-10)
	float	fStageCERES;  			//Development stage used in CERES model (-)
	float	fZStage;  				//ZCK development stage code (0-100)
}
DEVSTAGE;

#define DEVELOPMENTSTAGE  	DEVSTAGE
#define PDEVELOPMENTSTAGE	PDEVSTAGE



typedef struct Response  * PRESPONSE;
typedef struct Response
{
	float	fInput;
	float	fOutput;
}
RESPONSE;

#ifndef MAX_STRINGLEN
#define MAX_STRINGLEN 40
#endif

typedef struct genparam  *PGENPARAM;
typedef struct genparam
{       
	char	acCropName[MAX_STRINGLEN];  //Name of the cultivar	
	char	acTypeName[MAX_STRINGLEN];	//Group name of the cultivar
	char	acEcoType[MAX_STRINGLEN];	//Ecotype of the cultivar
	char	acPathway[10];             	//Photosythesis Pathway (C3/C4/CAM)
	char	acLvAngleType[MAX_STRINGLEN];//Canopy structure (leaf angle distribution type)
	float	fPlantHeight;         		//Height of the plant at anthesis (m)
	float	fCO2CmpPoint; 				//CO2 compensation point at 20C (vppm)
	float	fCiCaRatio;	       			//Ratio of internal to external CO2 concentration
	float	fMaxGrossPs;	//Maximum net photosynthesis of the leaf (kg CO2/ha.h)
	float	fLightUseEff;	//Light use efficiency at low light (kg CO2/MJ)
	float	fMinRm;			//Minimum value of mesophyll resistance to CO2 (s/m)
	float	fDarkResp;		//Dark respiration rate at day time (kg CO2/ha.h) 
	float	fPsCurveParam;	//Photosynthesis light response curve number (-)     
	float	fTempMaxPs;		//Maximum temperature for photosynthesis process (C)
	float	fTempMinPs; 	//Minimum temperature for photosynthesis process (C)
	float	fTempOptPs;		//Optimum temperature for photosynthesis process (C)
	float	fMaintLeaf;		//Maintenance respiration coefficient leaves (1/d)
	float	fMaintStem;		//Maintenance respiration coefficient stems (1/d)
	float	fMaintRoot;		//Maintenance respiration coefficient roots (1/d)
	float	fMaintStorage;	//Maintenance respiration coefficient storage organs (1/d)
	float	fCO2EffLeaf;	//CO2 producing factor for leaf biomass growth (kgCO2/kg DM)
	float	fCO2EffStem;	//CO2 producing factor for stem biomass growth (kgCO2/kg DM)
	float	fCO2EffRoot;	//CO2 producing factor for root biomass growth (kgCO2/kg DM)
	float	fCO2EffStorage;	//CO2 producing factor for storage biomass growth (kgCO2/kg DM)
	float	fGrwEffLeaf;	//Growth efficiency for leaves (kgDM/kgCH2O)
	float	fGrwEffStem;	//Growth efficiency for stems (kgDM/kgCH2O)
	float	fGrwEffRoot;	//Growth efficiency for roots (kgDM/kgCH2O)
	float	fGrwEffStorage;	//Growth efficiency for storage organs (kgDM/kgCH2O)
	float	fDevelopConstant1;	//Minimum days of the vegetative phase (d)
	float	fDevelopConstant2;	//Minimum days of the reproductive phase (d)
	char	acDaylenType[10];	//Daylength Response Type (Long/Short/Medium)
  float	fDaylenSensitivity;	//Daylength sensitivity (0-1)
	float	fCritDaylen;		//Critical (minimum) Daylength (hr)
 	float	fOptDaylen; 		//Optimum Daylength (hr)
	float	fTempMaxDevVeg;		//Maximum temperature for the development in vegetative phase (C)
	float	fTempMinDevVeg;		//Minimum temperature for the development in vegetative phase (C)
	float	fTempOptDevVeg;		//Optimum temperature for the development in vegetative phase (C)
	float	fTempMaxDevRep;		//Maximum temperature for the development in reproductive phase (C)
	float	fTempMinDevRep;		//Minimum temperature for the development in vegetative phase (C)
	float	fTempOptDevRep;		//Optimum temperature for the development in vegetative phase (C)
  float	fCritVernDays;		//Critical (minimum) vernalization units reqired (d)
  float	fOptVernDays;  		//Optimal vernalization units (d)
  float	fVernSensitivity;   //Vernalization sensitivity (0-1)
	float	fTempMaxVern;		//Maximum temperature for vernalization (C)
	float	fTempOptVern;		//Optimum temperature for vernalization (C)
	float	fTempMinVern;		//Minimum temperature for vernalization (C)   
 	float	fTempMaxLfApp;		//Maximum temperature for Leaf appearance (C)
	float	fTempOptLfApp;		//Optimum temperature for Leaf appearance (C)
	float	fTempMinLfApp;		//Minimum temperature for Leaf appearance (C)   
	float	fMaxLfInitRate;			//Maximum initiation rate for leaf primodia (Primodia/d)
	float	fMaxLfAppRate;     		//Maximum leaf appearance rate (leaves/d)
	float	fTillerDemand;     		//Maximum demand of assimilate for a single stem (g/stem)
	float	fStorageOrganNumCoeff;	//Coefficient for estimating the number of storage organs (type-dependent)
	float	fSpecLfWeight;   		//Specific leaf weight (kg/ha leaf)
	float	fReserveRatio;   		//Ratio of reserves to stem dry weight at anthesis (-)  
	float	fRelResvGrwRate;  		//Relative reserve growth rate (1/d)
	float	fRelStorageFillRate;	//Relative growth rate of storage organs (plant-dependent)  
	float	fRelStorageNFillRate;	//Relative growth rate of storage organs Nitrogen(plant-dependent)  
	float	fTempMaxSoFill;     	//Maximum temperature for growth of storage organs (C)
	float	fTempOptSoFill;     	//Optimum temperature for growth of storage organs (C)	
	float	fTempMinSoFill;     	//Minimum temperature for growth of storage organs (C)   
	float	fTempMaxSoNFill;     	//Maximum temperature for growth of storage organs (C)
	float	fTempOptSoNFill;     	//Optimum temperature for growth of storage organs (C)	
	float	fTempMinSoNFill;     	//Minimum temperature for growth of storage organs (C)   
	float	fMaxRootExtRate;		//Maximum root extension rate (cm/d)
	float	fRootLengthRatio;		//Length per kg root (cm/g)
	float	fMaxWuptRate;			//Maximum water uptake rate of unit length of root (cm3/cm.d)
	float	fMaxNuptRate;			//Maximum nitrogen uptake rate of unit length of root (kgN/cm.d)
	float	fMaxRootDepth; 			//Maximum rooting depth (m)
	float	fBeginSenesDvs;   		//Development stage where senescense begins (-)
	float	fBeginShadeLAI;			//Critical leaf area where shading effects occur (1)   
	float	fRelDeathRateLv;		//Relative death rate leaves (1/d)
	float	fRelDeathRateRt;		//Relative death rate roots (1/d)
	RESPONSE	PmaxTemp[21];      	//Maximum photosynthesis rate to temperature (Temp-Pmax)
	RESPONSE	DevRateVegTemp[21];	//Development rate at Veg. phase to temp (Temp-Rdev)
	RESPONSE	DevRateRepTemp[21];	//Development rate at Rep. phase to temp (Temp-Rdev)
	RESPONSE	DevRateVegPhtP[21]; //Development rate to photoperiod (PhP-Rdev)
	RESPONSE	VernRateTemp[21];	//Rate of vernalization vs temperature 
	RESPONSE	PartRoot[21];		//Partitioning factor for root vs development stage
	RESPONSE	PartLeaf[21];		//Partitioning factor for Leaves vs development stage	
	RESPONSE	PartStem[21];      	//Partitioning factor for stems vs development stage
	RESPONSE	PartStorage[21];   	//Partitioning factor for Storage organs vs development stage  
	RESPONSE	ReservGrwLag[21];	//Lag factor for reserve growth vs development stage
	RESPONSE	ReservTransTc[21];	//Time constant for reserve translocation vs development stage
	RESPONSE	SoFillLag[21]; 		//Lag factor for growth of storage organs vs development stage
	RESPONSE	SoFillRateTemp[21];	//Relative growth rate of storage organs vs temperature
	RESPONSE	SoNFillRateTemp[21];//Relative growth rate of storage N vs temperature
	RESPONSE	MaxLvNc[21];		//Maximun leaf nitrogen content vs development stage
	RESPONSE	MaxStNc[21];        //Maximun stem nitrogen content vs development stage
	RESPONSE	MaxRtNc[21];        //Maximun root nitrogen content vs development stage
	RESPONSE	OptLvNc[21];       	//Optimum leaf nitrogen content vs development stage
	RESPONSE	OptStNc[21];       	//Optimum stem nitrogen content vs development stage
	RESPONSE	OptRtNc[21];        //Optimum root nitrogen content vs development stage
	RESPONSE	MinLvNc[21];		//Minimun leaf nitrogen content vs development stage
	RESPONSE	MinStNc[21];        //Minimun stem nitrogen content vs development stage
	RESPONSE	MinRtNc[21];     	//Minimun leaf nitrogen content vs development stage
	RESPONSE	RootExtRateTemp[21];//Root extension rate vs temperature
	RESPONSE	RootExtRateWc[21]; 	//Root extension rate vs soil water content
	RESPONSE	LvDeathRateTemp[21];//Relative leaf death rate vs temperature
	RESPONSE	RtDeathRateTemp[21];//Relative root death rate vs temperature
	RESPONSE	SpecLfWght[21];		//Specific leaf weight vs development stage
	RESPONSE	LfAppRateTemp[21];	//Relative leaf appearance rate vs temperature
 	float		fVernSensCeres;   	//Following are CERES Model Parameters
	float		fDaylenSensCeres;
	float		fGrainFillPeriod;
  float		fKernelNumCoeff;
  float		fKernelWghtCoeff;
  float		fSpikeNumCoeff;   
  float		fPHINT;
  //	STAGEPARAM	asStageParam[MAX_DEV_STAGES];	//Development parameters in every stage
  //	VARIETYINFO	asVarietyInfo[MAX_VARIETYINFO];	//Variety Information
	PGENPARAM	pNext;
	PGENPARAM	pPrev;
}
GENPARAM;  
	
	
typedef struct xPltN  * PXPLTN;
typedef struct xPltN
{
	float	fLeafNc;			//Actual nitrogen concentration of leaves 			(kg/kg)
	float	fStemNc;      		//Actual nitrogen concentration of stems  			(kg/kg)

	float	fLeafRelNc;			//Relative nitrogen concentration of leaves 		(kg/kg)
	float	fStemRelNc;      	//Relative nitrogen concentration of stems  		(kg/kg)
	float	fRootRelNc;        	//Relative nitrogen concentration of roots 		 	(kg/kg)
	float	fShootRelNc;        //Relative nitrogen concentration of shoot 			(kg/kg)

	float	fDeadLeafNc;
	float	fDeadStemNc;
	float	fDeadRootNc;

	float	fLeafResNc; 		//Residual nitrogen concentration of leaves (kg/kg)
	float	fStemResNc;			//Residual nitrogen concentration of stems 	(kg/kg)
	float	fRootResNc;         //Residual nitrogen concentration of roots 	(kg/kg)

	float	fLeafMaxNc;			//Maximum nitrogen concentration of leaves 	(kg/kg)
	float	fStemMaxNc;			//Maximum nitrogen concentration of stems  	(kg/kg)
	float	fRootMaxNc;         //Maximum nitrogen concentration of roots  	(kg/kg)
	//===========================================================================================
	// Nitrogen Demand
	//===========================================================================================
	float	fLeafNdem;	 		//Daily nitrogen demand of leaves 			(kg/ha/day)
	float	fStemNdem;			//Daily nitrogen demand of stems 			(kg/ha/day)
	float	fRootNdem;        	//Daily nitrogen demand of roots 			(kg/ha/day)
	float	fGrainNdem;        	//Daily nitrogen demand of grains 			(kg/ha/day)
	float	fShootNdem;        	//Total daily nitrogen demand of the shoot 	(kg/ha/day)
	float	fVegNdem;        	//Nitrogen demand of the vegetative parts 	(kg/ha/day)
	//===========================================================================================
	// Weight of Nitrogen
	//===========================================================================================
  float	fLeafNw;    		//Amount of nitrogen in live leaves 	(kg/ha)
  float	fStemNw;           	//Amount of nitrogen in live  stems 	(kg/ha)
  float   fRootNw;            //Amount of nitrogen in live  roots 	(kg/ha)
  float   fGrainNw;           //Amount of nitrogen in grains      	(kg/ha)  
  float   fVegNw;         	//Amount of nitrogen in vegetative part (kg/ha) 
  float   fGenNw;         	//Amount of nitrogen in generative part (kg/ha)
  float   fStoverNw;         	//Amount of nitrogen in the stover 		(kg/ha)
  float   fTotalNw;         	//Amount of nitrogen in the crop 		(kg/ha)

	float	fLeafNinc;    		//Amount of increased nitrogen due to uptake in leaves (kg/ha/day)
	float	fStemNinc;          //Amount of increased nitrogen due to uptake in stems (kg/ha/day)
	float	fRootNinc;          //Amount of increased nitrogen due to uptake in roots (kg/ha/day)
	float	fGrainNinc;         //Amount of increased nitrogen due to uptake in grains (kg/ha/day)
	float	fShootNinc;         //Amount of increased nitrogen due to uptake in shoots (kg/ha/day)

	float	fLeafNtransRate;	//Nitrogen loss rate due to translocation leaves (kg/ha/day)
	float	fRootNtransRate;	//Nitrogen loss rate due to translocation roots  (kg/ha/day)
	float	fStemNtransRate;	//Nitrogen loss rate due to translocation stems  (kg/ha/day)
	float	fShootNtransRate;	//Nitrogen loss rate due to translocation shoots  (kg/ha/day)

  float	fDeadLeafNw;		//Amount of nitrogen in dead leaves 	(kg/ha)
  float	fDeadRootNw;       	//Amount of nitrogen in dead  roots 	(kg/ha)
  float 	fDeadStemNw;  		//Amount of nitrogen in dead  stems 	(kg/ha)

  float	fLeafTransNw;		//Amount of translocatable nitrogen in leaves (kg/ha)
  float	fRootTransNw;       //Amount of translocatable nitrogen in roots  (kg/ha)
  float 	fStemTransNw;  		//Amount of translocatable nitrogen in stems  (kg/ha)
  float 	fStoverTransNw;  	//Amount of translocatable nitrogen in crop   (kg/ha)
  float 	fTotalTransNw;  	//Amount of translocatable nitrogen in crop   (kg/ha)

	float 	fStoverNwChange; 	//Daily change in stover N weight 	(kg/ha/day)
	float	fRootNwChange;		//Daily change in root N weight 	(kg/ha/day)
	float	fPotGrnNcumRate;	//Potential nitrogen accumulation rate of individual grains (mg/d)

	float	fLeafNlossRate;		//Nitrogen loss rate due to death of leaves (kg/ha/day)
	float	fRootNlossRate;		//Nitrogen loss rate due to death of roots  (kg/ha/day)
	float	fStemNlossRate;		//Nitrogen loss rate due to death of stems  (kg/ha/day)

	float	fStoverPoolN;		//Nitrogen pool in stove 	(kg/ha)
	float	fRootPoolN;			//Nitrogen pool in root 	(kg/ha)
	float  	fTotalPoolN;		//Totoal nitrogen pool 		(kg/ha)
}
XPLTN;

typedef struct xDevelop  * PXDEV;
typedef struct xDevelop
{
	float		fThermEffect;
	float		iDayAftAnthesis;
}
XDEV;


typedef struct xCanopy  * PXCAN;
typedef struct xCanopy
{
	float	fLeafPrimodiaNum;
	float 	fMainStemTillers;
	float	fLeafAgeDeathRate;
	float	fLeafShadeDeathRate;
	float	fRelLeafDeathRate; 
	float	fLeafAreaLossRate;
}
XCAN;


typedef struct xBiom  * PXBIOM;
typedef struct xBiom
{
	float	fStemReserveGrowRate;
	float	fStemReserveWeight; 
	float	fReserveTransRate;  
	float  	fLeafReserveWeight; 
	float	fLeafDeathRate; 
	float 	fRootDeathRate;
	float	fDeadLeafWeight;
	float	fDeadRootWeight;
	float	fDeadStemWeight;
	float 	fTotLeafWeight;
	float	fTotRootWeight;
	float	fTotStemWeight;
}
XBIOM;

typedef struct xPltW  * PPLTW;
typedef struct xPltW
{
	float	fPotUptake;
	float	fActUptake;
	float	fWsPartition;
}
XPLTW;

typedef struct xWeather  * PXWTH;
typedef struct xWeather
{
	float	fDaylength;
	float	fRadMax; 
}
XWTH;


OUTPUT	  Outputs;
XPLTW	 	  xPltW;
XPLTN	 	  xPltN;
XDEV		  xDev;
XCAN		  xCan;
XBIOM		  xBiom;
XWTH		  xWth;
GENPARAM 	GenParam;

#define CO2 400

/*******************************************************************************
** EOF */