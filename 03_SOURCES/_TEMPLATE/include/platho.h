//////////////////////////////////////////////////////////
//	#defines und Variablen für PLATHO					//
//////////////////////////////////////////////////////////

//Funktionen
extern float AFGENERATOR(float fInput, RESPONSE* pfResp);

//Zusätzliche Definitionen und Variablen für das Modell PLATHO:

#define EXP_POINTER2 PCHEMISTRY pCh,PCLIMATE pCl,PGRAPHIC pGr,PHEAT pHe,PLOCATION pLo,PMANAGEMENT pMa,PPLANT pPlant,PSPROFILE pSo,PTIME pTi,PWATER pWa
#define exp_p2 pCh,pCl,pGr,pHe,pLo,pMa,pPlant,pSo,pTi,pWa

#define NewDayAndPlantGrowing2 	(NewDay(pTi)&&(pPlant->pDevelop->bPlantGrowth==TRUE)&&(pPlant->pDevelop->bMaturity==FALSE))
#define PlantIsGrowing2 		((pPlant->pDevelop->bPlantGrowth==TRUE)&&(pPlant->pDevelop->bMaturity==FALSE))

#define MAXPLANTS 20
#define MAXSOILLAYERS 40
#define MAXTIMESTEP 10000


#define BARLEY		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"BA"))
#define WHEAT		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"WH"))
#define POTATO		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"PT"))
#define SUNFLOWER	if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"SF"))
#define MAIZE		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"MZ"))
#define BEECH		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"BE"))
#define SPRUCE		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"SP"))
#define LOLIUM		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"IR"))
#define ALFALFA		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"AL"))
#define APPLE		if(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"AP"))

// Luzerne nur zu Testzwecken zu den ANNUALS geschrieben!
// Sobald Luzerne richtig modelliert wird, darf "AL" nur noch bei den HERBS stehen!!!
#define ANNUALS		if((!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"BA"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"SF"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"IR"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"PT"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"MZ"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"AL")))
#define HERBS		if((!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"BA"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"SF"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"IR"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"PT"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"MZ"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"AL")))
#define TREES		if((!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"BE"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"SP"))||(!lstrcmp((LPSTR)pPlant->pGenotype->acCropCode,(LPSTR)"AP")))

//Matrixpotentiale	
#define Psi1  (float)-630.0		//ab hier Staunässe
#define Psi2  (float)-6300.0		//Beginn des Wasserstress
#define Psi3  (float)-150000.0	//untere Grenze für Wasseraufnahme (Permanenter Welkepunkt)


//Dichte des Pflanzenbestandes
float	fPlantDensity;
//Mittlerer Abstand zwischen den Individuen:
float	fMeanDistance;


//Struktur PPLATHOPLANT: Definition spezieller PLATHO - Variablen

	
typedef struct stPools * PPOOLS;
typedef struct stPools
{
	float	fCarbohydrates;
	float	fProteins;
	float	fLignins;
	float	fLipids;
	float	fOrganicAcids;
	float	fAminoAcids;
	float	fNucleotides;
	float	fMinerals;
	float	fFracC;
}
POOLS;


typedef struct stPlantMaterial *PPLANTMATERIAL;
typedef struct stPlantMaterial  
	{
		PPOOLS	pFineRoot;
		PPOOLS	pGrossRoot;
		PPOOLS	pStem;
		PPOOLS	pBranch;
		PPOOLS	pLeaf;
		PPOOLS	pFruit;
		PPOOLS	pTuber;
	}
	PLANTMATERIAL;


typedef struct stPlathoGenotype * PPLATHOGENOTYPE;
typedef struct stPlathoGenotype
	{
		float fMaxPlantGrowR; // [1/d]
		float fLeafFlushR; // [1/d]
		float fFineRootFlushR; // [1/d]

		float fVcmaxSu;
		float fJmaxSu;
		float fVcmaxSh;
		float fJmaxSh;
		RESPONSE afVcmaxSu[21];
		RESPONSE afJmaxSu[21];
		RESPONSE afVcmaxSh[21];
		RESPONSE afJmaxSh[21];
		float fFCalphaSu;
		float fFCalphaSh;
		float fTheta;

		float fFineRootLossR;
		float fGrossRootLossR;
		float fStemLossR;
		float fBranchLossR;
		float fLeafLossR;
		float fFruitLossR;
		float fTuberLossR;

		float fAssRlcFineRoot;
		float fAssRlcGrossRoot;
		float fAssRlcStem;
		float fAssRlcBranch;
		float fAssRlcLeaf;
		float fAssRlcFruit;
		float fAssRlcTuber;

		float fAssUseRate; //relocation rate of CH2O from assimilate pool [1/d]
		float fNTransRate; //relocation rate of mobile nitrogen [1/d]
		
		RESPONSE MaxGrossPsSu[21];
		RESPONSE MaxGrossPsSh[21];

		RESPONSE MinBrNc[21];
		RESPONSE MinGRtNc[21];
		RESPONSE MinTuberNc[21];
		RESPONSE MinFruitNc[21];
		RESPONSE OptBrNc[21];
		RESPONSE OptGRtNc[21];
		RESPONSE OptTuberNc[21];
		RESPONSE OptFruitNc[21];
		RESPONSE MaxBrNc[21];
		RESPONSE MaxGRtNc[21];
		RESPONSE MaxTuberNc[21];
		RESPONSE MaxFruitNc[21];

		PPLANTMATERIAL pPltMaterial;

		float	fProteinsTurnoverR;
		float	fLipidsTurnoverR;
		float	fNucleotidesTurnoverR;
		float	fMineralsLeakageR;
		float	fMaintMetabol;
		float	fMaintMetabolTimeDelay;
		float	fO3DefCompTurnoverR;
		float	fLfPathDefCompTurnR;
		float	fRtPathDefCompTurnR;

		float	fSpecGrossRootLength;
		float	fRootExtTempMin;
		float	fRootExtTempOpt;
		float	fRootExtTempMax;

		float	fRelHeightMaxLeaf;
		float	fRelDepthMaxRoot;

		float	fKDiffuse;
		float	fLAImax;

	}
	PLATHOGENOTYPE;


typedef struct stPlathoBiomass * PPLATHOBIOMASS;
typedef struct stPlathoBiomass
	{
		float fWoodInc;
		float fFRootFrac;
		float fGRootFrac;
		float fLeafFrac;
		float fBranchDeathRate;
		float fGRootDeathRate;
		float fSeedWeight;
		float KFRoot;
		float KGRoot;
		float KStem;
		float KBranch;
		float KLeaf;
		float KFruit;
		float KTuber;
//		float fVegAbvEnd1;
		float fVegAbvEnd2;
		float fLeafEnd3;
		float fFruitEnd4;
		float fAssRelocR;
		float fNRelocR;
	}
	PLATHOBIOMASS;


typedef struct stPlathoDevelop * PPLATHODEVELOP;
typedef struct stPlathoDevelop  
	{
		int	iStagePlatho;
	}
	PLATHODEVELOP;

typedef struct stPlathoMycorrhiza* PPLATHOMYCORRHIZA;
typedef struct stPlathoMycorrhiza  
	{
		float	fDegMycoShort;
		float	fDegMycoMedium;
		float	fDegMycoLong;
		float	fCostMycoShort;
		float	fCostMycoMedium;
		float	fCostMycoLong;
		float	fBenefitMycoShort;
		float	fBenefitMycoMedium;
		float	fBenefitMycoLong;
	}
	PLATHOMYCORRHIZA;


typedef struct stPlathoRootLayer *PPLATHOROOTLAYER;
typedef struct stPlathoRootLayer
	{
	float	fGRLengthDens;
	float	fGRLengthDensR;
	float	fGRLengthDensFac;
	float	fRootArea;
	float	fWCompCoeff;
	float	fNCompCoeff;

    PPLATHOMYCORRHIZA pPMycorrhiza;

	PPLATHOROOTLAYER pNext;
	PPLATHOROOTLAYER pBack;
	}
	PLATHOROOTLAYER;

typedef struct stLeafLayer
	{
		double SpecLeafWeight;
		double LeafAreaDensity;
		double LeafAreaCum;
		double LAICum;
		double LeafWeightCum;
		double LeafNConc;
	}
	LEAFLAYER;


typedef struct stPlathoMorphology * PPLATHOMORPHOLOGY;
typedef struct stPlathoMorphology  
	{
		float	fPlantArea;
		float	CoeffCompUpper;
		float	CoeffCompLeft;
		float	CoeffCompRight;
		float	CoeffCompLower;
		RESPONSE	fBranchFrac[21];
		RESPONSE	fUGrWoodFrac[21];
		float	fStemDensity;
		float	fStemHeightR;
		float	fSpecStemLength;
		float	fMinSpecStemLength;
		float	fMaxSpecStemLength;
		float	fHeightToDiameter;
		float	fHtoDmax;
		RESPONSE fHDmax[21];
		float	fHtoDmin;
		double	MeanSpecLfWeight;
		float	fCrownStemRatio;
		float	fCrownToStem0;
		float	fElastCSR;
		float	fCRegulation;
		float	fNRegulation;
		float	fStemDiameter;
		float	fStemDiameterOld;
		float	fPlantDiameter;
		float	fFineRootToLeafRatio;
		float	fLeafToStemRatio;
		float	fMaxLeafToWoodRatio;
		float	fFruitToLeafRatio;
		LEAFLAYER   aLeafLayer[50];
		PPLATHOROOTLAYER  pPlathoRootLayer;
	}
	PLATHOMORPHOLOGY;


typedef struct stPlathoNitrogen * PPLATHONITROGEN;
typedef struct stPlathoNitrogen 
	{
		float	fNTotalOptConc;
		float	fNTotalActConc;
		float   fNDemandForGrowth;

		float	fFruitMinConc;

		float	fGrossRootTransNw;
		float	fBranchTransNw;
		float	fFruitTransNw;
		float	fTuberTransNw;
		float	fNSeedReserves;

		float	fFruitNtransRate;
		float	RNO3U[MAXSOILLAYERS];
		float	RNH4U[MAXSOILLAYERS];
	}
	PLATHONITROGEN;


typedef struct stPlathoPartitioning * PPLATHOPARTITIONING;
typedef struct stPlathoPartitioning 
	{
		float	fTotalDemandForGrowthAndDefense;

		float	fCostsForMaintenance;
		float	fCostsForBiomassGrowth;
		float	fCostsForMetabolism;
		float	fCostsForDefenseAndRepair;
        float   fCostsForMycorrhiza;

		float	fCostsForMaintenanceCum;
		float	fCostsForBiomassGrowthCum;
		float	fCostsForDefenseAndRepairCum;
        float   fCostsForMycorrhizaCum;
		float	fCumPhotosynthesis;
		float	fCumMaintenanceRespiration;
		float	fCumGrowthRespiration;
		float	fCumTotalRespiration;
		float	fCumLitterLossC;
		float	fCumLitterLossN;

		float	fDailyPhotosynthesis;
		float	fDailyMaintResp;
		float	fDailyGrowthResp;

		float	fDailyGrowth;
		float	fDailyMaintenance;
		float	fDailyDefense;

		float	fDemandForDefenseAndRepair;
		float	fDemForLfPathDef;
		float	fDemForRtPathDef;
		float	fDemForO3Defense;

		float	fDetLeaf;
		float	fPotFru;
		float	fDetJohannis;

		float	fCBalance;
		float	fNBalance;
	}
	PLATHOPARTITIONING;


typedef struct stPlantStress * PPLANTSTRESS;
typedef struct stPlantStress
	{
		//competition for light and nutrients
		float	fCompCoeff;
		float	fWCC;
		float	fNCC;
		float	fNShortage;
		float	fCShortage;

		//O3-Stress
		float	fO3StressIntensity;
		float	fO3Sensitivity;
		float	fO3ConcLeaf;
		float	fO3LeafConcCum;
		float	fO3LeafConcCumCrit;
		float	fCritO3Conc;		//[µg(O3)/kg(TS)]

		float	fO3ConstDefense;	//[%/100]
		float	fO3IndDefense;		//[%/100/(µg(O3)/kg(TS))]
		float	fO3DamageRate;		//[(1/d)/(µg(O3)/kg(TS))]
		float	fO3DefenseEff;		//[µg(O3)/kg]
		float	fO3Beta;			//[-]

		float	fO3DefCompTurnoverR;//[1/d]

		float	fO3DefCompConc;		//[kg/kg(TS)]
		float	fO3DefCompR;		//[kg/d]
		float	fO3DefCompCont;		//[kg]

		//leaf pathogenes
		float	fLfPathConstDefense;	//[%/100]
		float	fLfPathIndDefense;	//[(%/100)/(m2(infected)/m2)]
		float	fLfPathDamageRate;	//[(1/d)/(m2(infected)/m2)]
		float	fLfPathDefenseEff;	//[(1/d)/(kg(def.comp.)/kg(TS))]
		float	fLfPathBeta;			//[-]
		float	fCritLfPath;			//[m2(infected)/m2(leaf)]

		float	fLfPathDefCompTurnoverR;//[1/d]

		float	fLfPathDefCompConc;		//[kg/kg(TS)]
		float	fLfPathDefCompR;		//[kg/d]
		float	fLfPathDefCompCont;		//[kg]

		//root pathogenes
		float	fRtPathConstDefense;	//[%/100]
		float	fRtPathIndDefense;	//[(%/100)/(m2(infected)/m2)]
		float	fRtPathDamageRate;	//[(1/d)/(m2(infected)/m2)]
		float	fRtPathDefenseEff;	//[(1/d)/(kg(def.comp.)/kg(TS))]
		float	fRtPathBeta;			//[-]
		float	fCritRtPath;			//[cm(infected)/cm(root)]

		float	fRtPathDefCompTurnoverR;//[1/d]

		float	fRtPathDefCompConc;		//[kg/kg(TS)]
		float	fRtPathDefCompR;		//[kg/d]
		float	fRtPathDefCompCont;		//[kg]
	}
	PLANTSTRESS;


typedef struct stPlantNeighbours * PPLANTNEIGHBOURS;
typedef struct stPlantNeighbours
	{
	 PPLANT	pPlantLeft;
	 PPLANT	pPlantRight;
	 PPLANT	pPlantUpper;
	 PPLANT	pPlantLower;
	}
	PLANTNEIGHBOURS;


typedef struct stPlathoPlant * PPLATHOPLANT;
typedef struct stPlathoPlant
{
	PPLATHOGENOTYPE			pPlathoGenotype;
	PPLATHODEVELOP			pPlathoDevelop;
	PPLATHOBIOMASS			pPlathoBiomass;
	PPLATHOPARTITIONING		pPlathoPartitioning;
	PPLATHOMORPHOLOGY		pPlathoMorphology;
	PPLATHONITROGEN			pPlathoNitrogen;
	PPLANTSTRESS			pPlantStress;
	PPLANTNEIGHBOURS		pPlantNeighbours;
//	PPLATHOPLANT			pNext;
//	PPLATHOPLANT			pBack;
}
PLATHOPLANT;


// Defininition spezieller PLATHO-Variablen:
typedef struct stPlathoClimate   * PPLATHOCLIMATE;
typedef struct stPlathoClimate  
	{
	float	 fSimTime;
	float	 fRad;
	float	 fPAR;
	float	 fTemp;
	float	 fHumidity;
	float	 fCO2;
	float	 fO3;

	PPLATHOCLIMATE    pNext;
	PPLATHOCLIMATE    pBack;
	}
	PLATHOCLIMATE;


/*
typedef struct stStress * PSTRESS;
typedef struct stStress  
	{
		float	fCO2Day;
		float	fO3ConcMin;
		float	fO3ConcMax;
		float	fLeafPathogenes;	//[m2(infected)/m2(leaf)]
		float	fRootPathogenes;	//[cm(infected)/cm(root)]

		PSTRESS pNext;
		PSTRESS pBack;
}
STRESS;

typedef struct stPlathoScenario * PPLATHOSCENARIO;
typedef struct stPlathoScenario  
	{
		int		iRows;
		int		iColumns;
		char	acResolution[5];

		PSTRESS pStress;
	}
	PLATHOSCENARIO;
  */

typedef struct stPlathoScenario * PPLATHOSCENARIO;
typedef struct stPlathoScenario  
	{
		int		iRows;
		int		iColumns;
		int		nPlants;
		char	acResolution[5];

		float	fCO2Day;
		float	fO3ConcMin;
		float	fO3ConcMax;
		float	fLeafPathogenes;	//[m2(infected)/m2(leaf)]
		float	fRootPathogenes;	//[cm(infected)/cm(root)]

		BOOL	abPlatho_Out[100];
	}
	PLATHOSCENARIO;



typedef struct stGrowthEfficiency * PGROWTHEFFICIENCY;
typedef struct stGrowthEfficiency
	{
		float fCarbohydrates;
		float fProteins;
		float fLipids;
		float fLignins;
		float fOrganicAcids;
		//float fAminoAcids;
		//float fNucleotides;
		float fMinerals;
		float fProteinsN;
		float fO3DefComp;
		float fLfPathDefComp;
		float fRtPathDefComp;

		float fProteinsNitRed;
		float fAminoAcidsNitRed;
		float fNucleotidesNitRed;
	}
	GROWTHEFFICIENCY;

typedef struct stCO2Required * PCO2REQUIRED;
typedef struct stCO2Required
	{
		float fCarbohydrates;
		float fProteins;
		float fLipids;
		float fLignins;
		float fOrganicAcids;
		//float fAminoAcids;
		//float fNucleotides;
		float fMinerals;
		float fProteinsN;
		float fO3DefComp;
		float fLfPathDefComp;
		float fRtPathDefComp;

		float fProteinsNitRed;
		float fAminoAcidsNitRed;
		float fNucleotidesNitRed;
	}
	CO2REQUIRED;


typedef struct stCarbon * PCARBON;
typedef struct stCarbon
	{
		float fCarbohydrates;
		float fProteins;
		float fLipids;
		float fLignins;
		float fOrganicAcids;
		//float fAminoAcids;
		//float fNucleotides;
		float fMinerals;
		float fO3DefComp;
		float fLfPathDefComp;
		float fRtPathDefComp;
	}
	CARBON;


typedef struct stBiochemistry * PBIOCHEMISTRY;
typedef struct stBiochemistry
	{
		PGROWTHEFFICIENCY	pGrowthEfficiency;
		PCO2REQUIRED		pCO2Required;
		PCARBON				pCarbon;
	}
	BIOCHEMISTRY;

typedef struct stPlathoModules * PPLATHOMODULES;
typedef struct stPlathoModules  
	{
		int		iFlagPhotosynthesis;
		int		iFlagNitrogenDistribution;
		int		iFlagSpecLeafArea;
        int		iFlagElasticity;
        int		iFlagH2OEffectPhot;
        int		iFlagNEffectPhot;
        int		iFlagO3EffectPhot;
        int		iFlagCH2OEffectPhot;
        int		iFlagNEffectGrw;
        int		iFlagLeafPathEffPhot;
        int		iFlagRootPathEffUpt;
        int		iFlagMycorrhiza;
	}
	PLATHOMODULES;



	
PPLATHOSCENARIO			pPlathoScenario;
PPLATHOMODULES			pPlathoModules;
PPLATHOCLIMATE			pPlathoClimate;
PPLATHOPLANT			pPlathoPlant[MAXPLANTS];
PBIOCHEMISTRY			pBiochemistry;

