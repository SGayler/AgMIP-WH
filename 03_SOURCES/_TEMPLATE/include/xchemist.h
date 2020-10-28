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
 * $History: xchemist.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:06
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung ch, 04.08.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XCHEMIST_H
#define _INC_EXPERTN_XCHEMIST_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

#define MAX_DEPTH_PER_MEASURE  10 //ep 170999 maximal number of start value horizons = 10

typedef struct cLayer  *PCLAYER;
typedef struct cLayer
	{
	float	fNO3N;
	float	fNO3NAds;
	float	fNH4N;
	float	fNH4NAds;
	float	fNH4NGas;
	float	fUreaN;
	float	fUreaNAds;
	float	fDOC;
	float	fDOCAds;
	float	fDON;
	float	fDONAds;
    float   fDOMCN;
	float	fNH4Manure;
	float	fNManure;
	float	fCManure;
	float	fManureCN;
	float	fNLitter;
	float	fCLitter;
	float	fLitterCN;
	float	fN2ON;
	float   fN2ONGasConc;
	float   fN2ONGasConcOld;
	float	fN2N;
	float   fN2NGasConc;
	float   fN2NGasConcOld;
	float   fNON;
	float   fNONGasConc;
	float   fNONGasConcOld;
	float	fCO2C;
	float	fCsolC;
	float	fNsolC;
	float	fCFreeFOM;
	float	fCFreeHumus;
	float	afNOrgFOMFrac[3];
	float	afCOrgFOMFrac[3];
	float   fNFOMVeryFast;
	float   fCFOMVeryFast;
	float	fNFOMFast;
	float	fCFOMFast;
	float	fNFOMSlow;
	float	fCFOMSlow;
	float	fNMicBiomFast;
	float	fCMicBiomFast;
	float	fNMicBiomSlow;
	float	fCMicBiomSlow;
	float	fNMicBiomDenit;
	float	fCMicBiomDenit;
    float   fNHumusFast;
    float   fCHumusFast;
    float   fNHumusSlow;
    float   fCHumusSlow;
    float   fNHumusStable;
    float   fCHumusStable;
    float   fNStrcLitter;
    float   fCStrcLitter;
    float   fLignFracStrcLit;
    float   fNMtbLitter;
    float   fCMtbLitter;
    float   fNGrossRootLitter;
    float   fCGrossRootLitter;
    float   fNFineRootLitter;
    float   fCFineRootLitter;

	float	fChemX;
	float	fNO3Nmgkg;
	float	fNH4Nmgkg;
	float	fUreaNmgkg;
	float	fChemXmgkg;
	float	fNO3NSoilConc;
	float	fNO3NAdsConc;
	float	fNH4NSoilConc;
	float	fNH4NAdsConc;
	float	fNH4NGasConc;
	float	fUreaNSoilConc;
	float	fUreaNAdsConc;
	float	fUreaNGasConc;
	float	fDOCSoilConc;
	float	fDOCAdsConc;
	float	fDONSoilConc;
	float	fDONAdsConc;
    float	fNO3NSoilConcOld;
	float	fNO3NAdsConcOld;
	float	fNH4NSoilConcOld;
	float	fNH4NAdsConcOld;
	float	fNH4NGasConcOld;
	float	fUreaNSoilConcOld;
	float	fUreaNAdsConcOld;
	float	fUreaNGasConcOld;
	float	fDOCSoilConcOld;
	float	fDOCAdsConcOld;
	float	fDONSoilConcOld;
	float	fDONAdsConcOld;
	float   fDOCSltConcMob;
    float   fDOCSltConcImm;
	float   fDOCAdsConcMob;
    float   fDOCAdsConcImm;
	float   fDOCSltConcMobOld;
    float   fDOCSltConcImmOld;
	float   fDOCAdsConcMobOld;
    float   fDOCAdsConcImmOld;

	float	fNO3DenitMaxR;
	float	fN2ODenitMaxR;
	float	fNH4NitrMaxR;
	float	fUreaHydroMaxR;
	float	fLitterMinerMaxR;
	float	fManureMinerMaxR;
	float	fHumusMinerMaxR;
    float   fHumusToDONMaxR;
    float   fLitterToDONMaxR;
    float   fManureToDONMaxR;
    float   fDONToHumusMaxR;
    float   fDONToLitterMaxR;
    float   fDONMinerMaxR;
	float	fNO3DenitR;
	float	fN2ODenitR;
	float	fNH4NitrR;
	float	fUreaHydroR;
	float   fMinerR;
	float	fLitterMinerR;
	float	fManureMinerR;
	float	fHumusMinerR;
    float   fHumusToDONR;
    float   fLitterToDONR;
    float   fManureToDONR;
    float   fDONToHumusR;
    float   fDONToLitterR;
    float   fDONMinerR;
	float   fFOMVeryFastDecMaxR;
	float	fFOMFastDecMaxR;
	float	fFOMSlowDecMaxR;
	float	fMicBiomFastDecMaxR;
	float	fMicBiomSlowDecMaxR;
	float	fBioMFastNonlinDecMaxR;
	float	fBioMSlowNonlinDecMaxR;
	float	fHumusFastMaxDecMaxR;
	float	fHumusSlowMaxDecMaxR;
	float	fUreaNVolatR;
	float	fNH4VolatR;
	float	fNO3VolatR;
	float   fNImmobR;
	float   fNLitterImmobR;
	float   fNManureImmobR;
	float   fNHumusImmobR;
	float	fCO2ProdR;
	float	afFOMDecR[3];
	float	fHumusDecR;
	float	fNO3DenitDay;
	float	fN2ODenitDay;
	float	fNH4NitrDay;
	float	fNImmobDay;
	float	fUreaHydroDay;
	float	fNManureMinerDay;
	float	fNLitterMinerDay;
	float	fNHumusMinerDay;
	float	fCManureMinerDay;
	float	fCLitterMinerDay;
	float	fCHumusMinerDay;
	float	fCO2ProdDay;
	float	fUreaLeachDay;
	float	fNH4LeachDay;
	float	fNO3LeachDay;
	float	fUreaVolatDay;
	float	fNH4VolatDay;
	float	fNO3VolatDay;
	float	fNO3DenitCum;
	float	fN2ODenitCum;
	float	fNH4NitrCum;
	float	fNImmobCum;
	float	fUreaHydroCum;
	float	fNManureMinerCum;
	float	fNLitterMinerCum;
	float	fNHumusMinerCum;
	float	fCManureMinerCum;
	float	fCLitterMinerCum;
	float	fCHumusMinerCum;
	float	fCO2ProdCum;

	float	fNAnorg;
	float	fNH4NitrCoeff;
	float	fCNCoeff;
	float	fH2ONitrCoeff;
	float	fpHCoeff;
	float	fTempCoeff;
	float	fTempCoeffOld;
	float   fGasDiffCoef;
	float   fLiqDiffCoef;
	float	fH2ONitrCoeff_2;
	float	fPpmTOkg;
	float   fNH4ToN2OR;
	float   fNH4ToN2OCum;
	float   fMicBiomCN;
	float   fN2ONGasFlux;
    float   fN2ONGasFluxOld;
    float   fGasDiffusiv;
    float   fN2OKh;
	float	fCSoilMicBiom;
	float	fNSoilMicBiom;
	PCLAYER pNext;
	PCLAYER pBack;
	}
	CLAYER;


typedef struct nMeasure   *PNMEASURE;
typedef struct nMeasure
	{
	long	lDate;
	int		iDay;
	float	afDepth[MAX_DEPTH_PER_MEASURE];
	float	afNO3N[MAX_DEPTH_PER_MEASURE];
	float	afNH4N[MAX_DEPTH_PER_MEASURE];
	float	afN2ON[MAX_DEPTH_PER_MEASURE];
	float	afN2ONEmis[MAX_DEPTH_PER_MEASURE];
	float	afNH4NEmis;
	PNMEASURE  pNext;
	PNMEASURE  pBack;
	}
	NMEASURE;

typedef struct cBalance   *PCBALANCE;
typedef struct cBalance
	{
    float   fNO3NProfile;
	float   fNH4NProfile;
	float   fUreaNProfile;
	float   fNONProfile;
	float   fN2ONProfile;
	float   fN2NProfile;
	
	float	fNProfileStart;
	double	dNProfile;
	double	dNBalance;
	double	dNBalCorrect;
	
	float	fCProfileStart;
	double	dCProfile;
	double	dCBalance;
	

	double	dCInputCum;
	double	dCOutputCum;
	double	dNInputCum;
	double	dNOutputCum;

	float   fNInputSoil;
	float   fNOutputSoil;
	float   fNBalanceSoil;
  }
	CBALANCE;

typedef struct cProfile   *PCPROFILE;
typedef struct cProfile
	{
	float	fNO3NSurf;
	float	fNH4NSurf;
	float	fUreaNSurf;
	float	fDOCSurf;
	float	fDONSurf;
	float	fCNDOMSurf;
	float	fDryMatterLitterSurf;
	float	fNLitterSurf;
	float	fCLitterSurf;
	float	fCNLitterSurf; 
	float	fDryMatterManureSurf;
	float	fNH4ManureSurf;
	float	fNManureSurf;
	float	fCManureSurf;
	float	fCNManureSurf; 
	float	fDryMatterHumusSurf;
	float	fCHumusSurf;
	float	fNHumusSurf;
	float	fCNHumusSurf;
	float   fCStandCropRes;
	float   fNStandCropRes;
	float   fCStrLitterSurf;
	float   fNStrLitterSurf;
	float   fLignFracStrLitSurf;
	float   fCMtbLitterSurf;
	float   fNMtbLitterSurf;
	float   fCMicLitterSurf;
	float   fNMicLitterSurf;
	float   fCLeafLitterSurf;
	float   fNLeafLitterSurf;
	float   fCBranchLitterSurf;
	float   fNBranchLitterSurf;
	float   fCStemLitterSurf;
	float   fNStemLitterSurf;
	float	fCLitter;
	float	fNLitter;
	float	fCNLitter; 
	float	fCManure;
	float	fNManure;
	float	fCNManure; 
	float	fCHumus;
	float	fNHumus;
	float	fCNHumus;
    float   fDON;
    float   fDOC;
	float	fCSoilMicBiom;
	float	fNSoilMicBiom;
	float	fCNSoilMicBiom;
	float   fLigFracLeaf;
	float   fLigFracBranch;
	float   fLigFracStem;
	float   fLigFracFineRoot;
	float   fLigFracCoarseRoot;
	float   fFineBranchDecR;
	float   fLargeWoodDecR;
	float   fCoarseRootDecR;

	float	fNLitterMinerDay;
	float	fNManureMinerDay;
	float	fNHumusMinerDay;
	float	fNMinerDay;
 	float   fCO2ProdDay;
	float   fNLitterImmobDay;
	float   fNManureImmobDay;
	float   fNHumusImmobDay;
	float	fNImmobDay;
	float	fUreaHydroDay;
	float	fNH4NitrDay;
 	float   fNH4ToN2ODay;
	float	fNO3DenitDay;
	float	fN2ODenitDay;
	float	fN2OEmisDay;
	float   fNOEmisDay;
	float	fN2EmisDay;
	float	fNH3VolatDay;
	float	fCO2EmisDay;
	float	fCO2SurfEmisDay;
	float	fCH4ImisDay;
	float	fUreaLeachDay;
	float	fNH4LeachDay;
	float	fNO3LeachDay;
 	float   fN2ODrainDay;
	float	fNTotalLeachDay;
 	float   fDONLeachDay;
 	float   fDOCLeachDay;
 	float	fNUptakeDay;    

	double	dNLitterMinerCum;
	double	dNManureMinerCum;
	double	dNHumusMinerCum;
	double	dNMinerCum;
	double  dNLitterImmobCum;
	double  dNManureImmobCum;
	double  dNHumusImmobCum;
	double	dNImmobCum;
	double  dNetNMinerCum30;
	double	dCMinerCum;	
	double	dCO2ProdCum;
 	double  dUreaHydroCum;
	double	dNH4NitrCum;
 	double  dNH4ToN2OCum;
	double	dNO3DenitCum;
	double	dN2ODenitCum;
	double	dN2OEmisCum;
	double  dNOEmisCum;   
	double	dN2EmisCum;
	double	dNH3VolatCum;
	double	dCO2EmisCum;
	double	dCO2SurfEmisCum;
	double	dCH4ImisCum;
	double  dN2ODrainCum;
	double	dUreaLeachCum;
	double	dNH4LeachCum;
	double	dNO3LeachCum;
	double	dNTotalLeachCum;
 	double  dDONLeachCum;
 	double  dDOCLeachCum;
 	double	dNUptakeCum;    

	float   fCO2EmisR;
	float   fCO2SurfEmisR;
	float   fCH4ImisR;
 	float   fNO3LeachR;
	float   fN2OEmisR;
	float   fNOEmisR;
	float   fN2EmisR;
	float   fNFertilizerR;
	float   fNH3VolatR;
	float   fNH3VolatMaxR;
 	float   fNH4ToN2OR;

	}
	CPROFILE;


typedef struct CParam   *PCPARAM;
typedef struct CParam
	{
	float	fMolDiffCoef;
	float	fMinerEffFac;
	float	fMinerHumFac;
	float	fBioMassFastCN;
	float	fBioMassSlowCN;
	float	fBioMassDenitCN;
	float	fMinerSatActiv;
	float	fMinerTempB;
	float	fMinerQ10;
	float	fMinerThetaMin;
	float	fDenitThetaMin;
	float	fDenitKsNO3;
	float	fDenitKsCsol;
	float	fMaintCoeff;
	float	fDenitMaintCoeff;
	float	fYieldCoeff;
	float	fDenitYieldCoeff;
	float	fBioFastMaxGrowR;
	float	fBioSlowMaxGrowR;
	float	fDenitMaxGrowR;
	float	fDispersivity;
	float	fNitrifPot;
	float   fNitrifNO3NH4Ratio;
	float   fNitrifNH4ToN2O;
	float   fN2ORedMaxR;
	float   fDenitReWet;
	float   fDenitFreeze;
	float   fDenitThaw;
	float	afKd[6];
	float	afKh[6];
	float   fMobImmSltEC;
	float   fMobAdsSiteFrac;
	PCPARAM  pNext;
	PCPARAM  pBack;
	}
	CPARAM;


/*********************************************************************************************
*             SammelVariable   Chemistry
***********************************************************************************************/
typedef struct chemistry *PCHEMISTRY;
typedef struct chemistry
	{
	PCLAYER			pCLayer;
	PNMEASURE		pNMeasure;
	PCBALANCE		pCBalance;
	PCPROFILE		pCProfile;
	PCPARAM			pCParam;
	}
	CHEMISTRY;



#endif // #ifndef _INC_EXPERTN_XCHEMIST_H

/*******************************************************************************
** EOF */