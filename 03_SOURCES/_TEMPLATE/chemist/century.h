#define SRFC 0
#define SOIL 1
#define NUMLAYERS 2
#define SimulationStarted  		(SimStart(pTi))
enum TSoilTextureIndex
{
	UNDEFINED = 0,
	COARSE  = 1,		// texture type for sandy soil
	MEDIUM = 2,		// texture type for medium (loamy) soil
	FINE = 3, 		// texture type for fine soil
	VERYFINE = 4 		// texture type for volcanic soil
};
enum TForestParts
{
	LEAF,	// leaf forest part; array index
	FROOT,	// fine root forest part
	FBRCH,	// fine branch forest part
	LWOOD,	// large wood forest part
	CROOT,	// coarse root forest part
	FPARTS	// number of parts in the forest system
};
//Conversion factors
#define kgphaTOgpm2 0.1f
#define gpm2TOkgpha 10.0f
//structures
struct fixedparameter  
{
	float teff[4],
		aneref[3],
		rad1p[3],
		pcemic[3], 
		dec3[2],
		dec5,
		dec4,
		dec1[2],
		dec2[2],
		p1co2a[2],
		p2co2,
		p3co2,
		varat1[3],
		varat2[3],
		varat3[3],
		peftxa,
		peftxb,
		ps1s3[2],
		ps2s3[2],
		animp,
		pligst[2],
		strmax[2],
		rsplig,
		damr[2],
		damrmn,
		pabres,
		spl[2],
		rcestr,
		pprpts[3],
		frfrac[2],
		favail,	
		rictrl,
	    riint,
		pmco2[2];
};

struct parameter
{
	int site;
float decw1,
	decw2,
	decw3,
	wdlig[5],
	cerfor[3][5],
	drain,
	SurfaceLitterC,SurfaceLitterCtoN,
	SurfaceSOM1C, SurfaceSOM1CtoN,
	wood1C, wood2C,
	maxt;

};



//functions
float getNtoDepthTMP (PCLAYER pCH,float depth, EXP_POINTER);
float CENTminer(EXP_POINTER);
struct fixedparameter SetFixedParameters();
struct parameter ReadParameter();
float PartitionResidue(	
					    float C,			// C Content g/m2 of litter 
					    float recres,		//  residue: ratio N/C
					    float frlig,		// // fraction of incoming material = lignin.
					    int layer,			// SRFC or SOIL
					    float Navailable,   //available N, either in first 20 or in XN soil layer
                                            //for direct adsorbtion
                        float *newmetabC,      //return values: new C,N of struc and metab pool,
                        float *newmetabN,      
						float oldstrucC,
                        float *newstrucC,
                        float *newstrucN,
						float oldlig,
						float *newlig,		//               adjusted lignin content of structural pool
						struct fixedparameter fixed);
float getNtoDepth (float depth, EXP_POINTER);
 void ScheduleNFlow(				
	float CFlowAtoB, 
	float FlowRatioCtoN,
	float AC,float AN,	//C and N content of Box A (source)
	float availMinN,	//available mineral N for possible Immobilisation
	//return values
	float *orgNflow,
	float *minNflow		//negative value for immobilisation
	);
float FlowNintoMineralSoil(float Ntoadd,			// amount of mineral N to add (gN/m^2)
							float depth,				// soil depth to receive mineral E (cm)
							float fractionToNitrate,	// fraction of N to go to Nitrate range = (0-1) 
							EXP_POINTER);
float FlowNfromMineralSoil(float Ndemand,		// amount of mineral N wanted (gE/m^2)
						float depth,			// soil depth to give mineral E (cm)
						float useNitrateFrac,	// max fraction removed from NOx pools range = (0-1),
												// -1 = remove in proportion to nitrate/ammonium available
						EXP_POINTER);
int CanDecompose (
	float const C,		// total C in Box A
	float const N,		//   N in Box A
	float const ratio, //   C/N,  ratios of new material being added to Box B
	float availMinN);
float getSandtoDepth(float depth, EXP_POINTER);
float getWFPStoDepth(float depth, EXP_POINTER);
float getH2OtoDepth (float depth, EXP_POINTER);
void decf20cm(	float * defac, float * anerb, //return values
				struct fixedparameter fixed,
				float drain,
				EXP_POINTER
				 );
float decfSL(	float sand, // [%] in Soil layer
				float WFPS, // Water filled pore space = pWL->fContAct / pSo->pSLayer->fPorosity
				float SoilTemp, //pHe->pHLayer->fSoilTemp
				struct fixedparameter fixed,	
				EXP_POINTER
				 );
float AboveGroundDecompRatio(float totalCinA ,float totalNinA,//amount of C and N in box A
							 float biofac,					  //biomass factor: 2.0 for wood, 2.5 else
							 struct fixedparameter fixed);

float FlowCintoSOM2(float Ctoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER);
float FlowNintoSOM2(float Ntoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER);
float FlowCintoSOM1(float Ctoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER);
float FlowNintoSOM1(float Ntoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER);
float CO2intoSOIL(float CO2toadd,			// amount of organic N to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER); 
float getN(PCLAYER  pCL);
float getNH4(PCLAYER  pCL);
float getNO3(PCLAYER  pCL);

//Nitrification
float f_arctangent(float x, float A[]);
float f_gen_poisson_density(float x, float A[]);

float Diffusivity (
	float const A,		// fraction of soil bed volume occupied by
				//   field capacity
				//   (intra-aggregate pore space, 0-1)
	float const bulkDen,	// bulk density of soil (g/cm^3)
	float const porosity,	// porosity fraction (0-1)
	float const wfps);	// Water Filled Pore Space fraction
				// (volumetric swc/porosity, 0-1)
double  Nitrify (EXP_POINTER);
double  Denitrify (EXP_POINTER);
float f_gen_poisson_density(float x, float A[]);
float f_arctangent(float x, float A[]);