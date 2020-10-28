//////////////////////////////////////////////////////////
//	#defines und Variablen für GECROS					//
//////////////////////////////////////////////////////////


//Struktur PGECROSPLANT: Definition spezieller GECROS - Variablen

	
typedef struct stGecrosGenotype * PGECROSGENOTYPE;
typedef struct stGecrosGenotype
	{
		float fMaxPlantGrowR; // [1/d]

		float fVcmaxSu;
		float fJmaxSu;
		float fVcmaxSh;
		float fJmaxSh;
		float fLAImax;

	}
	GECROSGENOTYPE;


typedef struct stGecrosBiomass * PGECROSBIOMASS;
typedef struct stGecrosBiomass
	{
		float fStorageWeight;//WSO [g m-2] dry weight of storage organs 
		float fStorageGrowR;//RWSO [g m-2 d-1] rate of increase in storage organs
		float fTotalSeedNum;//TSN
		float f1000GrainWeight;//TSW
		float fShtToRtActivity;//DERI 
		float fShootActivity;//SHSA

		float fStemWeightChangeR;//RWST 
		float fLeafDeadWeight;//WLVD [g m-2] dry weight of dead leaves
		float fLeafWeightLossR;//LWLV [g m-2 d-1] rate of weight loss in leaves due to senescence
		float fShootWeight;//WSH [g m-2] dry weight of shoot (above-ground) organs
		float fShtWghtExShLvs;//WSHH [g m-2] dry weight of shoot organs excluding shedded leaves
		float fRootDeadWeight;////WRTD [g m-2] dry weight of dead roots
		float fRootWeightLossR;//LWRT [g m-2 d-1] rate of weight loss in roots due to senescence

		float fHarvestIndex;//HI [g g-1] harvest index
		
	}
	GECROSBIOMASS;


typedef struct stGecrosParameter * PGECROSPARAMETER;
typedef struct stGecrosParameter  
	{
		int	  iStageGecros;
		int   iniGecros;
		float fStageEndSeedNum;//ESD
		float fStageEndSeedNumI;//ESDI
		float fGrwEffVegOrg;//YGV [g C g-1 C] growth efficiency of veg. organs (Genotype)    

		float fNFracRemobPrevE;//PNPRE - (Plant Nitrogen)
		float fSeedWeight;//SEEDW [g seed-1] (Plant Biomass)
		float fNSeedActConc;//SEEDNC [g N g-1] (Plant Nitrogen)

		float fPMEH;//PMEH [-] (Plant Biomass)
		float fPMES;//PMES [-] (Plant Biomass)

		float fStemPerHeightDryWeight;//CDMHT [g m-2 m-1] (Plant Biomass)
		float fPlantHeightMax;//HTMX [m] (Plant Biomass)
		float fRootDepthMax;//RDMX [cm] (Plant Biomass)
	}
	GECROSPARAMETER;

typedef struct stGecrosCarbon * PGECROSCARBON;
typedef struct stGecrosCarbon 
	{
        float   fCPlant;//CTOT [g C m-2] total C amount of living plant
		float   fCLeaf;//CLV [g C m-2] C amount of living leaves
		float   fCLeafDead;//CLVD [g C m-2] C amount of dead leaves
        float   fCLeafDeadSoil;//CLVDS [g C m-2] C amount of dead leaves already litter in soil
		float   fCLeafDeadSoilR;//LVDS  [g C m-2 d-1] transfer rate of C amount of dead leaves to soil
		float   fCShoot;//CSH [g C m-2] amount of C in living shoot organs
	    float   fCStem;//CSST [g C m-2] amount of C in structural stems
		float   fCStorage;//CSO [g C m-2] amount of C in storage organs
		float   fCFracVegetative;//CFV [-] C fraction in vegetative organs
		float   fCFracStorage;//CFO [-] C fraction in storage organs
	    float   fCRoot;//CRT [g C m-2] amount of C in living roots (incl. reserves)
	    float   fCRootDead;//CRTD [g C m-2] amount of C in dead roots 
	    float   fCStrctRoot;//CSRT [g C m-2] amount of C in structural roots
	    float   fCStrctRootN;//CSRTN N-determined CSRT
		float   fCStemReserve;//CRVS [g C m-2] C amount in stem reserves
		float   fCRootReserve;//CRVR [g C m-2] C amount in root reserves

		float   fCFracPartToLeaf;//FCLV [-] fraction of new shoot C partitioned to leaves
		float   fCFracPartToStem;//FCSST [-] fraction of new shoot C partitioned to str. stems
		float   fCFracPartToShoot;//FCSH [-] fraction of new C partitioned to shoot
		float   fCFracPartToRoot;
		float   fCFracPartToRootReserve;//FCRVR [-] fraction of new root C partitioned to root reserves
        float   fCFracPartToStemReserve;//FCRVS [-] fraction of new shoot C partitioned to stem reserves
        float   fCFracPartToStorage;//FCSO [-] fraction of new C partitioned to storage organs

		float   fCFlowToSink;//FLWC [g C m-2 d-1] flow of new C to sink
		float   fCFlowToStorage;//FLWCS [g C m-2 d-1] flow of new C to storage organs
		float   fCFlowToStem;//FLWCT [g C m-2 d-1] flow of new C to structural stems

		float   fCDlyDemandSink;
		float   fCDlyDemandOrgan;
		float   fCDlyDemandStorage;//DCDS [g C m-2 d-1] daily C demand for filling of storage organs
		float   fCActDemandSeed;//DCDSC [g C m-2 d-1] actual C demand for seed filling 
		float   fCDlyDemandShoot;
		float   fCDlyDemandRoot;
		float   fCDlyDemandStem;//DCDT [g C m-2 d-1] daily C demand for structural stems
		float   fCActDemandStem;//DCDTC [g C m-2 d-1] actual C demand for structural stems
		float   fCPrvDemandStem;//DCDTP [g C m-2 d-1] previous C demand for structural stems
		float   fCPrvDmndStmChangeR;//RDCDTP [g C m-2 d-1] previous C demand for structural stems change rate

		float   fCShortFallDemandSeed; //DCDSR [g C m-2] short fall C demand for seed fill in previous time steps
		float   fCShortFallDemandSeedR;//RDCDSR [g C m-2] change rate of short fall C demand for seed fill 
		                               //in previous time steps
		float   fCShortFallDemandStem; //DCDTR [g C m-2] short fall C demand for structural stems in previous time steps
		float   fCShortFallDemandStemR; //RDCDTR [g C m-2] change rate of short fall C demand for structural stems 
		                               //in previous time steps
		float   fCDlySupplySink;
		float   fCDlySupplyShoot;//DCSS [g C m-2 d-1] daily new C supply for shoot growth
		float   fCDlySupplyRoot;//DCSR [g C m-2 d-1] daily new C supply for root growth
		float   fCDlySupplyStem;//DCST [g C m-2 d-1] daily new C supply for stem growth

        float   fCLeafChangeR;//RCLV [g C m-2 d-1] rate of change in C amount of living leaves
	    float   fCStemChangeR;//RCSST [g C m-2 d-1] rate of change in C amount of structural stems
		float   fCStemRsrvChangeR;//RCRVS [g C m-2 d-1] rate of change in C amount of stem reserves
		float   fCRootChangeR;//RCSRT [g C m-2 d-1] rate of change in C amount of structural roots
		float   fCRootRsrvChangeR;//RCRVR [g C m-2 d-1] rate of change in C amount of root reserves
		float   fCStorageChangeR;//RCSO [g C m-2 d-1] rate of change in C amount of storage organs
	    float   fCLeafLossR;//LCLV [g C m-2 d-1] rate of C loss in leaves due to senescence
	    float   fCRootLossR;//LCRT [g C m-2 d-1] rate of C loss in roots due to senescence
        float   fCRootToStorageR;//CREMR [g C m-2 d-1] remobilized C from root reserves to storage organs
        float   fCStemToStorageR;//CREMS [g C m-2 d-1] remobilized C from stem reserves to storage organs
        float   fCRootToStorageRI;//CREMRI [g C m-2 d-1] remob. C from root reserves to storage organs intermediate
        float   fCStemToStorageRI;//CREMSI [g C m-2 d-1] remob. C from stem reserves to storage organs intermediate
		float   fCSeedGrowthGap;//GAP [g C m-2 d-1] gap between C supply and C demand for seed growth 

		float   fNetStdgCropPhotosynR;//APCANS-RM-RX
		float   fGrossStdgCropPhotosynR;//APCANS [g CO2 m-2 d-1] Act. standing crop CO2 assimilation
		float   fGrossStdgCropPhotosynRN;//APCANN [g CO2 m-2 d-1] APCANS with small plant-N increment
		float   fMaintRespRN;//RMN [g CO2 m-2 d-1] maintenance respiration rate with small plant-N increment
		float   fFixRespCost;//RX [g CO2 m-2 d-1] respirative cost of N2 fixation
		float   fUptRespCost;//RMUL [g CO2 m-2 d-1] resp. cost of phloem loading and uptake of minerals and N
		float   fUptRespCostR;//RRMUL rate of change of RMUL
		float   fCNFixCost;//CCFIX [g C g-1 N] carbon cost of symbiotic nitrogen fixation 
	}
	GECROSCARBON;

typedef struct stGecrosNitrogen * PGECROSNITROGEN;
typedef struct stGecrosNitrogen 
	{
		float   fNExtCoeff;//KN
        float   fNFixation;//NFIX [g N m-2 d-1] amount of symbiotically fixed N     
        //float   fNFixationDemand;//NFIXD [g N m-2 d-1] crop demand determined NFIX     
        //float   fNFixationEnergy;//NFIXE [g N m-2 d-1] available energy determined NFIX    
        float   fNFixationReserve;//NFIXR[g N m-2] reserve pool of symbiotically fixed N
		float   fNFixReserveChangeR;//RNFIXR
        float   fNFixationTotal;//NFIXT [g N m-2] total fixed N during growth

		float   fNToCFracNewBiomass;//NCR [g N g-1 C]
        float   fNToCFracNewBiomPrev;//NCRP [g N g-1 C]
		float   fNFracPartShoot;//FNSH [-] fracion of new N partitioned to shoot

		float   fNSeedConc;//SEEDNC [g N g-1] standard seed (storage organ) N concentration

		float   fNRemobToSeedR;//RNRES [g N m-2] vegetative-organ N remobilizable for seed growth
		                                        //rate
		float   fNRemobToSeedF;//NREOF [g N m-2] vegetative-organ N remobilizable for seed growth 
		                                       //till seed fill starts
		float   fNRemobToSeedE;//NREOE [g N m-2] vegetative-organ N remobilizable for seed growth 
		                                       //till end of seed number determining period
		float   fNRemobToSeedFR;//RNREOF [g N m-2] rate of change for vegetative-organ N remobilizable  
		                                 //for seed growth till seed fill starts 
		float   fNRemobToSeedER;//RNREOE [g N m-2] rate of change for vegetative-organ N remobilizable 
		                                 //for seed growth till end of seed number determining period
        float   fNInitFacSeedF;//CB [-] factor for initial N concentration of seed fill
		float   fNFinalFacSeedF;//CX [-] factor for final N concentration of seed fill
		float   fNFastSeedFDevStage;//TM [-] development stage when transition from CB to CX is fastest

		float   fNLeafLossR;//LNLV [g N m-2 d-1] rate of loss of leaf N amount due to senescence
		float   fNRootLossR;//LNRT [g N m-2 d-1] rate of loss of root N amount due to senescence

		float   fNShootCont;//NSH [g N m-2] content of N in living shoot
		float   fNShootContSoil;//NSHH [g N m-2] content of N in shoot excl. dead leaves in soil
		float   fNStorageCont;//NSO [g N m-2] content of N in storage organs

		float   fNLeafTotCont;//TNLV [g N m-2] content of total N in leaves (incl. dead leaves)
		float   fNLeafDeadCont;//NLVD [g N m-2] content of N in dead leaves 
		float   fNRootDeadCont;//NRTD [g N m-2] content of N in dead roots
		float   fNLeafSpecificCont;//SLN
		float   fNLeafSpecificContTop;//SLNT
		float   fNLeafSpecificContBottom;//SLNB
		float   fNLeafSpecificContBottomChangeR;//RSLNB

		float   fShootActConc;//HNC in xplant.h aufnehmen unter NITROGEN!
		float   fTotalActConc;//PNC
		float   fStorageActConc;//ONC
		float   fSeedProtein;//PSO

		float   fNLeafAccumR;//RNLV
		float   fNStemAccumR;//RNST
		float   fNStorageAccumR;//RNSO
		float   fNRootAccumR;//RNRT
		float   fNLeafTotAccumR;//RTNLV

		float   fNUptMax;//NUPTX [g N m-2 d-1] maximum crop N uptake 
		float   fNDmndActDrv;//NDEMA [g N m-2 d-1] activity-driven crop N demand

		float   fNDemand;//NDEM
		float   fNDmndPrev;//NDEMP
		float   fNDmndPrvChangeR;//RNDEMP

		float   fNPlantSupply;//NSUP [g N m-2 d-1] N supply to crop
		float   fNPlantSuppPrev;//NSUPP [g N m-2 d-1] previous N supply to crop
		float   fNPltSppPrvChangeR;//RNSUPP
	}
	GECROSNITROGEN;


typedef struct stGecrosCanopy * PGECROSCANOPY;
typedef struct stGecrosCanopy 
	{
		float fLAIGreen;//LAI
		float fLAITotal;//TLAI
		float fLAICdeterm;//LAIC C-determined LAI
		float fLAINdeterm;//LAIN N-determined LAI
		float fPlantHeightGrowR;//RHT
	}
    GECROSCANOPY;

typedef struct stGecrosSoil * PGECROSSOIL;
typedef struct stGecrosSoil 
	{
		float fProfileDepth;//150.
		float fWaterContUpperLayer;//WUL
		float fWaterContLowerLayer;//WLL
		float fWaterContInitial;//WCI
		float fWaterContInitFac;//MULTF
		float fPlantWaterCapacity;//PWC
		float fETDailyWaterSupply;//DWSUP
		float fWaterFlowToLowLayer;//RRUL

        float fSoilTemp;//TSOIL
		float fSoilTempR;//RTSOIL
        
		float fTotalOrgCarbon;//TOC
		float fMicroBiomC;//BIO
		float fMicBiomCRate;//RBIO
		float fMicBiomCDecR;//DECBIO
		float fHumusC;//HUM
		float fHumusCRate;//RHUM
		float fHumusCDecR;//DECHUM
		float fDecPlantMatC;//DPM
		float fDecPltMCRate;//RDPM
		float fDecPltMCDecR;//DECDPM
		float fDecPlantMatN;//DPN
		float fDecPltMNRate;//RDPN
		float fDecPltMNDecR;//DECDPN
		float fResPlantMatC;//RPM
		float fResPltMCRate;//RRPM
		float fResPltMCDecR;//DECRPM
		float fResPlantMatN;//RPN
		float fResPltMNRate;//RRPN
		float fResPltMNDecR;//DECRRPN
		float fCNPlantMat;//CNDRPM
		float fLitterC;//LITC
		float fLitterN;//LITN
		float fNH4NContUpperLayer;//NAUL
		float fNH4NContLowerLayer;//NALL
		float fNO3NContUpperLayer;//NNUL
        float fNO3NContLowerLayer;//NNLL
		float fNH4NContUpLayRate;//RNAUL
		float fNH4NContLwLayRate;//RNALL
		float fNO3NContUpLayRate;//RNNUL
        float fNO3NContLwLayRate;//RNNLL
		float fNH4NContProfile;//NA
		float fNO3NContProfile;//NN
		float fNH4NFertilizer;//FERNA
		float fNH4NVolatFert;//SFERNA
		float fNH4NVolatFertR;//RSFNA
		float fNH4NVolatilizRate;//VOLA
		float fNO3NFertilizer;//FERNN

		float fDiffSoilAirTemp;//DIFS
	}
    GECROSSOIL;

typedef struct stGecrosPlant * PGECROSPLANT;
typedef struct stGecrosPlant
{
	PGECROSPARAMETER		pGecrosParameter;
	PGECROSGENOTYPE			pGecrosGenotype;
	PGECROSBIOMASS			pGecrosBiomass;
	PGECROSCARBON			pGecrosCarbon;
	PGECROSNITROGEN			pGecrosNitrogen;
	PGECROSCANOPY			pGecrosCanopy;
	PGECROSSOIL             pGecrosSoil;
//	PGECROSPLANT			pNext;
//	PGECROSPLANT			pBack;
}
GECROSPLANT;

PGECROSPLANT			pGecrosPlant;
