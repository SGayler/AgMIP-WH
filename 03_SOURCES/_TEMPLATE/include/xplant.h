/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:      Sperr Chr.
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Priesack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Simulation time objects
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: xplant.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:08
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung 01.03.98
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XPLANT_H
#define _INC_EXPERTN_XPLANT_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

/***********************************************************************************************
      				 Crop Genotype Infomation Part
***********************************************************************************************/

#define MAX_FILE_STRINGLEN    30
#define MAX_DEPTH_PER_MEASURE 10 //ep 170999 maximal number of start value horizons 
#define MAX_DEVSTAGE          10 //ep 170799 maximal number of development stages

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  INCLUDE FILES, DEFINED VARIABLES AND TYPES 
//------------------------------------------------------------------------------------------------------------
//Expert_N System Data Variables

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Type Definitions
// #define DLL  CALLBACK _loadds

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


typedef struct stvarietyInfo  * PVARIETYINFO;
typedef struct stvarietyInfo
	{
	int				iCode;
	char			acName[MAX_FILE_STRINGLEN];
	int 			iValue;             
	PVARIETYINFO    pNext;
	PVARIETYINFO    pBack;
	}
	STVARIETYINFO;

typedef struct ststageParam  *PSTAGEPARAM;
typedef struct ststageParam
	{
	float	afTempMax[MAX_DEVSTAGE];
	float	afTempOpt[MAX_DEVSTAGE];
	float	afTempBase[MAX_DEVSTAGE];
	float	afThermalTime[MAX_DEVSTAGE];
	float	afMaxDevR[MAX_DEVSTAGE];
//ep 170799	
	PSTAGEPARAM   pNext;
	PSTAGEPARAM   pBack;
	}
	STSTAGEPARAM;

typedef struct Response  * PRESPONSE;
typedef struct Response
	{
	float	fInput;
	float	fOutput;
	}
	RESPONSE;


typedef struct stgenotype  *PGENOTYPE;
typedef struct stgenotype
	{
	char	acTypeName[MAX_FILE_STRINGLEN];
	char	acEcoType[MAX_FILE_STRINGLEN];
	char	acCropName[MAX_FILE_STRINGLEN];
	char	acCropCode[MAX_FILE_STRINGLEN];
	char	acVarietyName[MAX_FILE_STRINGLEN];
	int		iVarietyCode;
	int		iBYSKID;
	int		iC_Pathway;
	float	fPhyllochronInterval;
	float	fCriticalDaylength;
	float	fDaylengthCoeff;
	int		iDaylengthTyp;
	int		iVernCoeff;
	float	fGrainNumCoeff;
	float	fGrainFillCoeff;
	float	fGrainFillStage;
	float	fMaxPhotosynR;
	float	fPhotosynDefic;
	float	fDarkResp;
	float	fRootExtTempMin;
	float	fRootExtTempOpt;
	float	fRootExtTempMax;
    //float   fTempOpt;
    //float   fTempMax;
    //float   fTempBase;
	float	fTempQ10;
	char	acStageName[MAX_FILE_STRINGLEN];

	char	acLvAngleType[MAX_FILE_STRINGLEN];//Canopy structure (leaf angle distribution type)
	float	fPlantHeight;         		//Height of the plant at anthesis (m)
	float	fCO2CmpPoint; 				//CO2 compensation point at 20C (vppm)
	float	fCiCaRatio;	       			//Ratio of internal to external CO2 concentration
	float	fMaxGrossPs;	//Maximum net photosynthesis of the leaf (kg CO2/ha.h)
	float	fLightUseEff;	//Light use efficiency at low light (kg CO2/MJ)
	float	fMinRm;			//Minimum value of mesophyll resistance to CO2 (s/m)
	float	fPsCurveParam;	//Photosynthesis light response curve number (-)     
	float	fTempMaxPs;		//Maximum temperature for photosynthesis process (C)
	float	fTempMinPs; 	//Minimum temperature for photosynthesis process (C)
	float	fTempOptPs;		//Optimum temperature for photosynthesis process (C)
	float	fMaintLeaf;		//Maintenance respiration coefficient leaves (1/d)
	float	fMaintStem;		//Maintenance respiration coefficient stems (1/d)
	float	fMaintRoot;		//Maintenance respiration coefficient roots (1/d)
	float	fMaintStorage;	//Maintenance respiration coefficient storage organs (1/d)
	float	fMaintBranch;	//Maintenance respiration coefficient branches (1/d)
	float	fMaintGrossRoot;//Maintenance respiration coefficient gross roots (1/d)
	float	fMaintTuber;	//Maintenance respiration coefficient tubers (1/d)
	float	fCO2EffLeaf;		//CO2 producing factor for leaf biomass growth (kgCO2/kg DM)
	float	fCO2EffStem;		//CO2 producing factor for stem biomass growth (kgCO2/kg DM)
	float	fCO2EffRoot;		//CO2 producing factor for root biomass growth (kgCO2/kg DM)
	float	fCO2EffStorage;		//CO2 producing factor for storage biomass growth (kgCO2/kg DM)
	float	fCO2EffBranch;		//CO2 producing factor for branch biomass growth (kgCO2/kg DM)
	float	fCO2EffGrossRoot;	//CO2 producing factor for gross root biomass growth (kgCO2/kg DM)
	float	fCO2EffTuber;		//CO2 producing factor for tuber biomass growth (kgCO2/kg DM)
	float	fGrwEffLeaf;		//Growth efficiency for leaves (kgDM/kgCH2O)
	float	fGrwEffStem;		//Growth efficiency for stems (kgDM/kgCH2O)
	float	fGrwEffRoot;		//Growth efficiency for roots (kgDM/kgCH2O)
	float	fGrwEffStorage;		//Growth efficiency for storage organs (kgDM/kgCH2O)
	float	fGrwEffBranch;		//Growth efficiency for branches (kgDM/kgCH2O)
	float	fGrwEffGrossRoot;	//Growth efficiency for gross roots (kgDM/kgCH2O)
	float	fGrwEffTuber;		//Growth efficiency for tubers (kgDM/kgCH2O)
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
	float	fTempMinDevRep;		//Minimum temperature for the development in reproductive phase (C)
	float	fTempOptDevRep;		//Optimum temperature for the development in reproductive phase (C)
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

	PSTAGEPARAM	 pStageParam;
	PVARIETYINFO pVarietyInfo;
    PGENOTYPE pNext;
	PGENOTYPE pBack;

	}
	STGENOTYPE;

/***********************************************************************************************
      	 Crop Development
***********************************************************************************************/

typedef struct stdevelop  *PDEVELOP;
typedef struct stdevelop
	{
	int	iDayAftSow;
	int	iDayAftEmerg;
	int	iStageCERES;
	float	fStageXCERES;
	float	fStageWang;
	float	fDevStage;
	float	fStageSUCROS;
	float	fDevR;
	float	fDTT;
	float	fDaylengthEff;
	float	fVernUnit;
	float	fVernEff;
	float	fCumDTT;
	float	fSumDTT;
	float	fCumTDU;
	float	fCumPhyll;
	float	fCumVernUnit;
	float	fPhysTime;
	float	fCumPhysTime;
	float	fPhysTimeFct;
	float	fPhysTimeExp;
	float	fTimeOfStage1;
	float	fTimeOfStage2;
	float	fTimeOfStage3;
	float	fTimeOfStage4;
	float	fTimeOfStage5;
	float	fTimeOfStage6;
	float	fAge;

	float	fThermEffect;
	float	iDayAftAnthesis;

	BOOL	bMaturity;
	BOOL	bPlantGrowth;

    PDEVELOP pNext;
	PDEVELOP pBack;
	}
	STDEVELOP;


/***********************************************************************************************
      				 Crop Growth Part
***********************************************************************************************/
typedef struct stbiomass  *PBIOMASS;
typedef struct stbiomass
	{
	float	fTotalBiomass;
	float	fBiomassAbvGround;
	float	fStovWeight;
	float	fRootWeight;
	float	fGrossRootWeight;
	float	fTuberWeight;
	float	fStemWeight;
	float	fBranchWeight;
	float	fLeafWeight;
	float	fFruitWeight;
	float	fGrainWeight;
	float	fWoodWeight;
	float	fGreenLeafWeight;
	float	fSenesLeafWeight;
	float	fDeadRootWeight ;
	float	fMaxBiomGrowR;
	float	fBiomGrowR;
	float	fMaxShootGrowR;
	float	fMaxRootGrowR;
	float	fRootGrowR;
	float	fMaxGrossRootGrowR;
	float	fGrossRootGrowR;
	float	fMaxTuberGrowR;
	float	fTuberGrowR;
	float	fMaxLeafGrowR;
	float	fLeafGrowR;
	float	fMaxStemGrowR;
	float	fStemGrowR;
	float	fMaxBranchGrowR;
	float	fBranchGrowR;
	float	fMaxFruitGrowR;
	float	fFruitGrowR;
	float	fGrainGrowR;
	float	fRelGrainFillR;
	float	fPlantTopFrac;
	float	fCapRoot;
	float	fCapStem;
	float	fCapLeaf;
	float	fCapFruit;
	float	fCapGrain;
	float	fAttritionRootR;
	float	fAttrtionStemR;
	float	fAttritionLeafR;
	float	fAttritionFruitR;
	float	fAttritionGrainR;
	float	fRedFracRoot;
	float	fRedFracStem;
	float	fRedFracLeaf;
	float	fRedFracFruit;
	float	fRedFracGrain;
	float	fPartFracRoot;
	float	fPartFracGrossRoot;
	float	fPartFracTuber;
	float	fPartFracStem;
	float	fPartFracBranch;
	float	fPartFracLeaf;
	float	fPartFracFruit;
	float	fPartFracGrain;
	float	fSeedReserv;

	float	fStemReserveGrowRate;
	float	fStemReserveWeight; 
	float	fReserveTransRate;  
	float  	fLeafReserveWeight; 
	float	fFruitDeathRate; 
	float	fLeafDeathRate; 
	float	fBranchDeathRate; 
	float	fStemDeathRate;
	float 	fRootDeathRate;
	float 	fGrossRootDeathRate;
	float 	fTuberDeathRate;
	float	fDeadLeafWeight;
	float	fDeadStemWeight;
	float 	fTotLeafWeight;
	float	fTotRootWeight;
	float	fTotStemWeight;

	PBIOMASS  pNext;
	PBIOMASS  pBack;
	}                      
	STBIOMASS;
	

/***********************************************************************************************
      				 Plant Canopy  
***********************************************************************************************/

typedef struct stcanopy   *PCANOPY;
typedef struct stcanopy
	{
	float	fLAI;
	float	fPlantHeight;
	float	fLeafAppearR;
	float	fLeafTipNum;
	float	fExpandLeafNum;
	float	fLAGrowR;
	float	fLALossR;
	float	fNormLeafSenesR;
	float	fLeafSenesR;
	float	fPlantLA;
	float	fPlantSenesLA;
	float	fPlantGreenLA;
	float	fTillerNum;
	float	fTillerNumOld;
	float	fTillerNumSq;
	float	fSenesTillerNumSq;
	float	fEarNum;
	float	fEarNumSq;
	float	fGrainNum;
	float	fGrainNumSq;
	float	fPlantDensity;
	float	fCropCoverFrac;

	float	fLeafPrimodiaNum;
	float 	fMainStemTillers;
	float	fLeafAgeDeathRate;
	float	fLeafShadeDeathRate;
	float	fRelLeafDeathRate; 
	float	fLeafAreaLossRate;

	PCANOPY  pNext;
	PCANOPY  pBack;
	}
	STCANOPY;

/***********************************************************************************************
      	 Plant Root
***********************************************************************************************/
typedef struct stroot  		 *PROOT;
typedef struct stlayerRoot 	 *PLAYERROOT;

 
typedef struct stlayerRoot
	{
	float	fLengthDens;
	float	fLengthDensR;
	float	fLengthDensFac;
	float	fPotWatUpt;
	float	fActWatUpt;
	float	fPotLayWatUpt;
	float	fActLayWatUpt;
	float	fWatStressRoot;
	float	fPotNO3NUpt;
	float	fActNO3NUpt;
	float	fPotLayNO3NUpt;
	float	fActLayNO3NUpt;
	float	fPotNH4NUpt;
	float	fActNH4NUpt;
	float	fPotLayNH4NUpt;
	float	fActLayNH4NUpt;
	float	fPotNUpt;
	float	fActNUpt;
	float	fPotLayNUpt;
	float	fActLayNUpt;
	float	fNStressRoot;
	PLAYERROOT 	pNext;
	PLAYERROOT 	pBack;
	}
	STLAYERROOT;

 typedef struct stroot
	{
	float	fDepth;
	float	fDepthGrowR;
	float	fMaxDepth;
    float   fUptakeDay;
	float   fUptakeR;
	float	fTotalLength;
   	PLAYERROOT    pLayerRoot;
	PROOT  pNext;
	PROOT  pBack;
	}
	STROOT;

/***********************************************************************************************
      	 Plant Carbon
***********************************************************************************************/

typedef struct stplantCarbon  *PPLTCARBON;
typedef struct stplantCarbon
	{
	float	fGrossPhotosynR;
	float	fNetPhotosynR;
	float	fPhotorespR;
	float	fMaintRespR;
	float	fGrowthRespR;
	float	fDarkRespR;
	float	fTotRespR;
	float	fNetPhotosyn;
	float	fMainRespRRoot;
	float	fMainRespRGrossRoot;
	float	fMainRespRTuber;
	float	fMainRespRStem;
	float	fMainRespRBranch;
	float	fMainRespRLeaf;
	float	fMainRespRGrain;
	float	fGrowRespRRoot;
	float	fGrowRespRGrossRoot;
	float	fGrowRespRTuber;
	float	fGrowRespRStem;
	float	fGrowRespRBranch;
	float	fGrowRespRLeaf;
	float	fGrowRespRGrain;
	float	fRedFctPhotosynLeaf;
	float	fRedFctPhotosynStem;
	float	fRedFctPhotosynGrain;
	float	fCAssimilatePool;
	PPLTCARBON  pNext;
	PPLTCARBON  pBack;
	}
	STPLTCARBON;


/***********************************************************************************************
      				 Plant Water 
***********************************************************************************************/

typedef struct stplantWater  *PPLTWATER;
typedef struct stplantWater
	{
	float	fTotCont;
	float	fTopsCont;
	float	fRootCont;
	float	fLeafCont;
	float	fStemCont;
	float	fGrainCont;
	float	fPanFac;
	float	fPotTranspdt;
 	float	fActTranspdt;
	float	fPotTranspDay;
	float	fActTranspDay;
	float	fInterceptDay;
	float	fShortage;
	float	fRefDemand;
 	float	fCumDemand;
	float	fCumUptake;
	float	fCumShortage;
	float	fStressFacPhoto;
	float	fStressFacLeaf;
	float	fStressFacTiller;

	float	fPotUptakedt;
	float	fActUptakedt;
	float	fStressFacPartition;
	PPLTWATER  pNext;
	PPLTWATER  pBack;
	}
 	STPLTWATER;


/***********************************************************************************************
      				Plant Nitrogen 
***********************************************************************************************/

typedef struct stplantNitrogen  *PPLTNITROGEN;
typedef struct stplantNitrogen
	{
	float	fNPool;

	float	fTopsMinConc;
	float	fTopsActConc;
	float	fTopsOptConc;
	float	fVegActConc;
	float	fVegMinConc;

	float	fFruitMinConc;
	float	fFruitOptConc;
	float	fFruitActConc;

	float	fLeafMaxConc;
	float	fLeafMinConc;
	float	fLeafOptConc;
	float	fLeafActConc;

	float	fStemMaxConc;
	float	fStemMinConc;
	float	fStemOptConc;
	float	fStemActConc;
	
	float	fBranchMinConc;
	float	fBranchOptConc;
	float	fBranchActConc;
	
	float	fRootMaxConc;
	float	fRootMinConc;
	float	fRootOptConc;
	float	fRootActConc;

	float	fGrossRootMinConc;
	float	fGrossRootOptConc;
	float	fGrossRootActConc;

	float	fTuberMinConc;
	float	fTuberOptConc;
	float	fTuberActConc;

	float	fGrainConc;
	float	fGrainConcChange;

	float	fRootCont;
	float	fGrossRootCont;
	float	fTuberCont;
	float	fStemCont;
	float	fBranchCont;
	float	fLeafCont;
	float	fGrainCont;
	float	fStovCont;
	float	fTotalCont;

	float	fPotGrnNcumRate;	//Potential nitrogen accumulation rate of individual grains (mg/d)

	float	fStovContChange;
	float	fRootContChange;

	float	fStovNLabile;
	float	fRootNLabile;
	float	fStovNPool;
	float	fRootNPool;
	float	fTotalNLabile;
	float	fTotalNPool;

	float	fLeafDemand;
	float	fStemDemand;
	float	fBranchDemand;
	float	fRootDemand;
	float	fGrossRootDemand;
	float	fTuberDemand;
	float	fGrainDemand;
	float	fShootDemand;
	float	fTotalDemand;
	float	fCumTotalDemand;
	float	fRevDemand;

	float	fPotNO3NUpt;
	float	fActNO3NUpt;
	float	fPotNH4NUpt;
	float	fActNH4NUpt;
	float	fPotNUpt;
	float	fActNUpt;
	float	fCumPotNUpt;
	float	fCumActNUpt;
	float	fMaxNUptR;
	float	fOptNUptR;
	float	fActNUptR;

	float	fLeafRelNc;			//Relative nitrogen concentration of leaves 		(kg/kg)
	float	fStemRelNc;      	//Relative nitrogen concentration of stems  		(kg/kg)
	float	fRootRelNc;        	//Relative nitrogen concentration of roots 		 	(kg/kg)
	float	fShootRelNc;        //Relative nitrogen concentration of shoot 			(kg/kg)

	float	fLeafNinc;    		//Amount of increased nitrogen due to uptake in leaves (kg/ha/day)
	float	fStemNinc;          //Amount of increased nitrogen due to uptake in stems (kg/ha/day)
	float	fBranchNinc;        //Amount of increased nitrogen due to uptake in stems (kg/ha/day)
	float	fRootNinc;          //Amount of increased nitrogen due to uptake in roots (kg/ha/day)
	float	fGrossRootNinc;     //Amount of increased nitrogen due to uptake in roots (kg/ha/day)
	float	fTuberNinc;         //Amount of increased nitrogen due to uptake in roots (kg/ha/day)
	float	fGrainNinc;         //Amount of increased nitrogen due to uptake in grains (kg/ha/day)
	float	fShootNinc;         //Amount of increased nitrogen due to uptake in shoots (kg/ha/day)

	float	fLeafNtransRate;		//Nitrogen loss rate due to translocation leaves (kg/ha/day)
	float	fRootNtransRate;		//Nitrogen loss rate due to translocation roots  (kg/ha/day)
	float	fGrossRootNtransRate;	//Nitrogen loss rate due to translocation roots  (kg/ha/day)
	float	fTuberNtransRate;		//Nitrogen loss rate due to translocation roots  (kg/ha/day)
	float	fBranchNtransRate;		//Nitrogen loss rate due to translocation roots  (kg/ha/day)
	float	fStemNtransRate;		//Nitrogen loss rate due to translocation stems  (kg/ha/day)
	float	fShootNtransRate;		//Nitrogen loss rate due to translocation shoots  (kg/ha/day)

    float	fDeadLeafNw;		//Amount of nitrogen in dead leaves 	(kg/ha)
    float	fDeadRootNw;       	//Amount of nitrogen in dead  roots 	(kg/ha)
    float 	fDeadStemNw;  		//Amount of nitrogen in dead  stems 	(kg/ha)

    float	fLeafTransNw;		//Amount of translocatable nitrogen in leaves (kg/ha)
    float	fRootTransNw;       //Amount of translocatable nitrogen in roots  (kg/ha)
    float 	fStemTransNw;  		//Amount of translocatable nitrogen in stems  (kg/ha)
    float 	fStoverTransNw;  	//Amount of translocatable nitrogen in crop   (kg/ha)
    float 	fTotalTransNw;  	//Amount of translocatable nitrogen in crop   (kg/ha)

	float	fLeafNlossRate;			//Nitrogen loss rate due to death of leaves (kg/ha/day)
	float	fStemNlossRate;			//Nitrogen loss rate due to death of stems  (kg/ha/day)
	float	fBranchNlossRate;		//Nitrogen loss rate due to death of leaves (kg/ha/day)
	float	fRootNlossRate;			//Nitrogen loss rate due to death of roots  (kg/ha/day)
	float	fGrossRootNlossRate;	//Nitrogen loss rate due to death of roots  (kg/ha/day)
	float	fTuberNlossRate;		//Nitrogen loss rate due to death of stems  (kg/ha/day)

	float	fNStressFac;
	float	fNStressPhoto;
	float	fNStressLeaf;
	float	fNStressTiller;
	float	fNStressGrain;


	PPLTNITROGEN  pNext;
	PPLTNITROGEN  pBack;
	}
 	STPLTNITROGEN;

/***********************************************************************************************
      	Plant Microclimate
***********************************************************************************************/

typedef struct stplantClimate  *PPLTCLIMATE;
typedef struct stplantClimate
	{
	float	fCanopyTemp;
	float	fCanopyInterceptRad;
	float	fCanopyReflecRad;
	float	fCanopyTransmRad;
	float	fCanopyAbsorbRad;
	PPLTCLIMATE  pNext;
	PPLTCLIMATE  pBack;
	
	}
	STPLTCLIMATE;

/***********************************************************************************************
      	Plant Model Parameter 
***********************************************************************************************/

typedef struct stModelParam  *PMODELPARAM;
typedef struct stModelParam
	{
	int		iEmergenceDay;
	int		iHarvestDay;
	long	lEmergenceDate;
	long	lHarvestDate;
	float	fHarvestYield;
	char	cResidueCarryOff;
	float	fMaxRootDepth;
	int	    iMaxAboveBiomassDay;
	int 	iMaxRootBiomassDay;
	long	lMaxAboveBiomassDate;
	long	lMaxRootBiomassDate;
	float	fTasselLeafNum;
	float	fMaxLeafNum;
	float	fMaxLA;
	float	fMinLeafWeight;
	float	fMinStemWeight;
	float	fMaxStemWeight;
	float	fSingleTillerStemWeight;
	float	fMaxCumNUpt;
	float	fMaxCumWatUpt;
	float	fHardnessIndex;
	int		iColdKill ;
	float	fBarleyGrwParam;
	float	fMaxCropCoverfrac;
	PMODELPARAM  pNext;
	PMODELPARAM  pBack;
	}
	STMODELPARAM;

typedef struct stPltMeasure   * PPLTMEASURE;
typedef struct stPltMeasure
	{
	long	lDate;
	int		iDay;	
	float	fDevStage;
	float	fLAI;
	float	fAboveBiomass;
	float	fVegBiomass;
	float	fGenBiomass;
	float	fStemWeight;
	float	fLeafWeight;
	float	fGrainWeight;
	float	fRootWeight;
	float	fNConcAbvBiomass;
	float	fNAboveBiomass;
	float	fNVegBiomass;
	float	fNGenBiomass;
	float	fNStem;
	float	fNLeaf;
	float	fNGrain;
	float	fNRoot;
	float	fTotalN;
	float	fRootDepth;
	float	fTillers;
	float	fHeight;
	float	fCropCoverFrac;
    float   fCropETCoeff;
	int		iLeafNum;

	PPLTMEASURE pNext;
	PPLTMEASURE pBack;
	} 
	STPLTMEASURE;


/***********************************************************************************************
		    	Plant 
***********************************************************************************************/

typedef struct stplant  *PPLANT;
typedef struct stplant
	{                           
 	PGENOTYPE	   pGenotype;
	PDEVELOP	   pDevelop;
	PBIOMASS	   pBiomass;
	PCANOPY		   pCanopy;
	PROOT		   pRoot;
	PPLTCARBON	   pPltCarbon;
	PPLTWATER	   pPltWater;
	PPLTNITROGEN   pPltNitrogen;
	PPLTCLIMATE	   pPltClimate;	
	PMODELPARAM    pModelParam;
	PPLTMEASURE	   pPMeasure;
	PPLANT   pNext;
	PPLANT   pBack;
 	}
	STPLANT;


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Module Global Variables
HANDLE 	hModuleInstance; 
float	PLSC[20];
OUTPUT	Outputs;

#define CO2 400 

#endif // #ifndef _INC_EXPERTN_XPLANT_H

/*******************************************************************************
** EOF */