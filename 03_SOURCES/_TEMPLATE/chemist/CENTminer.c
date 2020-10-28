 #include <math.h>
 
#include <stdlib.h>
#include <stdio.h>
#include  <xinclexp.h>
#include "century.h"
extern int SimStart(PTIME);			// from time1.c
extern int NewDay(PTIME);
extern	int		WINAPI Message(long, LPSTR);


float CENTminer(EXP_POINTER)// returns total daily NNM [g/m2] = mineralisation - immobilisation
{
// used in loops over all layer
PCLAYER		pCL;
PSLAYER		pSL;
PWLAYER		pWL;
PHLAYER		pHL;

int const TimeStepsPerDay=4;
float dtDecompDC=(float)1.0f/365.f/TimeStepsPerDay;	//fraction of year per timestep
static struct fixedparameter fixed;
static struct parameter par;
float C=0.0, N=0.0, lig=0.0, Navail=0.0, oldlig=0.0, oldsC, sC=0.0, sN=0.0, newlig=0.0, mC=0.0, mN=0.0, ret; //partition litter into struc/metab
float tcflow; //total c flow
float defacSRFC, anerb;
int i,iday;
float *pdecf;
float rneww1[2],								//ratio C/N fine branches to: [0]SOM1 [1]SOM2
	  rneww2[2],								//ratio C/N large wood    to: [0]SOM1 [1]SOM2
	  rneww3[2],								//ratio C/N coarse roots  to: [0]SOM1 [1]SOM2
	  rnewas[2],								//ratio C/N above ground structural  to: [0]SOM1 [1]SOM2
	  rnewbs[2];								//ratio C/N below ground structural  to: [0]SOM1 [1]SOM2
float radds1;
float CtoSOM2, CtoSOM1, co2loss;
float orgNflow=0.0, minNflow=0.0;		
float mineralFlow;// Mineralization associated with respiration
float cfmes1,rcemet,cfsfs2,rceto2,rceto3,rceto1,cfs1s3,cfs1s2,cfs2s1,cfs2s3,cfs3s1;
float microbeC; //C flow out of surface microbe pool 
float efftext;
float frNH4, frNO3;
float dummy;
float MinByWood,MinByStructLitter,MinByMetabLitter,MinBySOM; //Mineralisation by pools [kg/ha]
float ImmByWood,ImmByStructLitter,ImmByMetabLitter,ImmByInput,ImmBySOM; //Immobilisation by pools [kg/ha]
 
FILE *outf;

if(SimulationStarted)
{
//set fixed parameters (values from Century file 'ffixed.100' for forests) 
	fixed = SetFixedParameters();
//read site parameter from 'century.cfg' file
	par = ReadParameter();

//********************************************************************************************
 // Initialize pools
//********************************************************************************************
 // A) Wood
	//wood 1: fine branch //aus *.cfg [kg/ha] gpm2TOkgpha
		pCh->pCProfile->fCBranchLitterSurf = par.wood1C;
		pCh->pCProfile->fNBranchLitterSurf = par.wood1C / par.cerfor[2][FBRCH];//calciv_dc line 216 
	//wood 2: large wood  //aus *.cfg [kg/ha]
		pCh->pCProfile->fCStemLitterSurf = par.wood2C;
		pCh->pCProfile->fNStemLitterSurf = par.wood2C / par.cerfor[2][LWOOD];//calciv_dc line 216 
	//wood 3: coarse roots //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCGrossRootLitter = pCL->fCManure;
			pCL->fNGrossRootLitter = pCL->fNManure;
		}
 // B) Litter
	//Belowground = fine root litter //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCFineRootLitter = pCL->fCLitter;
			pCL->fNFineRootLitter = pCL->fNLitter /(float)3.0; //Adjustment from workbook 2.b.
		}
	//Aboveground = leaves litter	//aus *.cfg
		pCh->pCProfile->fCLeafLitterSurf = par.SurfaceLitterC;
		pCh->pCProfile->fNLeafLitterSurf = par.SurfaceLitterC / par.SurfaceLitterCtoN;

	//SOM soil //aus xnm file Marke 20003
		for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pCL=pCL->pNext, pWL=pWL->pNext, i++)
		{
			pCL->fCHumusFast   = pSL->fCHumus * 0.03f;	//percentages from century workbook Ch. II
			pCL->fNHumusFast   = pSL->fNHumus * 0.03f / 0.7f;   
			pCL->fCHumusSlow   = pSL->fCHumus * 0.65f;  
			pCL->fNHumusSlow   = pSL->fNHumus * 0.65f / 1.1615f;//neu berechnet, so das alles N aufgeteilt ist  
			pCL->fCHumusStable = pSL->fCHumus * 0.32f;  
			pCL->fNHumusStable = pSL->fNHumus * 0.32f / 0.7f;  
		}
	//SOM1 srfc //aus *.cfg
		pCh->pCProfile->fCMicLitterSurf = par.SurfaceLitterC;
		pCh->pCProfile->fNMicLitterSurf = par.SurfaceLitterC / par.SurfaceLitterCtoN;
}

if (NewDay(pTi))
{

//********************************************************************************************
// 0) Initialize 
//********************************************************************************************

	for(pCL=pCh->pCLayer;pCL->pNext!=NULL;pCL=pCL->pNext)	 
	{	  
		pCL->fMinerR = 0.0;
		pCL->fNImmobR = 0.0;
		pCL->fCO2ProdR = 0.0;
		pCL->fCO2C=0.0;
		pCL->fHumusMinerR = 0.0;
		pCL->fLitterMinerR = 0.0;
		pCL->fNLitterImmobR = 0.0;
	}
	pCh->pCProfile->fNHumusImmobDay = 0.0;
	MinByWood=MinByStructLitter=MinByMetabLitter=MinBySOM=0.0; 
	ImmByWood=ImmByStructLitter=ImmByMetabLitter=ImmByInput=ImmBySOM=0.0; 
	DailyCorrection=0.0;

//********************************************************************************************
// I) Calculate temperature and water factor for decomposition, do this daily 
//********************************************************************************************
//	a) Surface pools: defacSRFC, anerb 
		decf20cm(&defacSRFC, &anerb, fixed, par.drain, exp_p);
//	b) Soil layers:   pdecf[i]
		pdecf =(float *)malloc((pSo->iLayers-2) * sizeof (float));
		for(i=0,pSL=pSo->pSLayer->pNext,pHL=pHe->pHLayer->pNext, pWL=pWa->pWLayer->pNext;
			pSL->pNext!=NULL;pSL=pSL->pNext, pHL=pHL->pNext, pWL=pWL->pNext, i++)
		{
			pdecf[i]=decfSL(pSL->fSand,(pWL->fContAct/pSL->fPorosity),pHL->fSoilTemp,fixed,exp_p);
		}

//********************************************************************************************


//********************************************************************************************
// II) Input litter (leaves to surface, fine roots to soil)
//********************************************************************************************
// a) surface litter
 		C=pCh->pCProfile->fCLeafLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNLeafLitterSurf * kgphaTOgpm2;
		lig=par.wdlig[LEAF];
		Navail=0.0; //no direct adsorbtion in surface litter
		oldlig = pCh->pCProfile->fLignFracStrLitSurf;
		oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
		if(C>0.0f && N>0.0f) 
		{
			ret = PartitionResidue(C,N/C,lig,SRFC,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,fixed); 		
			pCh->pCProfile->fCLeafLitterSurf = 0.0; //all litter is now in struc/metab pools
			pCh->pCProfile->fNLeafLitterSurf = 0.0;
			pCh->pCProfile->fCStrLitterSurf += sC * gpm2TOkgpha;
			pCh->pCProfile->fNStrLitterSurf += sN * gpm2TOkgpha;
			pCh->pCProfile->fLignFracStrLitSurf = newlig;
			pCh->pCProfile->fCMtbLitterSurf += mC * gpm2TOkgpha;
			pCh->pCProfile->fNMtbLitterSurf += mN * gpm2TOkgpha;
		}
	 
// b) soil litter (fine roots)
		for(i=0,pCL=pCh->pCLayer->pNext; pCL->pNext!=NULL;pCL=pCL->pNext,i++)
		{
 			C=pCL->fCFineRootLitter * kgphaTOgpm2;
			N=pCL->fNFineRootLitter * kgphaTOgpm2;								
			lig=par.wdlig[LEAF];
			Navail=(pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //direct adsorbtion from first 20 cm 
			oldlig = pCh->pCProfile->fLignFracStrLitSurf;
			oldsC=pCh->pCProfile->fCStrLitterSurf* kgphaTOgpm2;
			if(C>0.0f && N>0.0f) 
			{
				ret = PartitionResidue(C,N/C,lig,SOIL,Navail,&mC,&mN,oldsC,&sC,&sN,oldlig,&newlig,fixed); 			
				//direct adsorbtion of mineral N
				if(ret>0.0)
				{
				ret = min (ret, (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
				frNH4 = pCL->fNH4N / (pCL->fNO3N + pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
				frNO3 = pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N); 
				pCL->fNH4N -= frNH4  
							* ret * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
				pCL->fNO3N -= frNO3
							* ret * gpm2TOkgpha;
				pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
				pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
				pCL->fNImmobR += ret * gpm2TOkgpha;
				ImmByInput += ret * gpm2TOkgpha;
				if(pCL->fNO3N <0.0 ) Message(1,"Warning: NO3 @ input negative!");	 
				if(pCL->fNH4N <0.0 ) Message(1,"Warning: NH4N @ input negative!");	 
	
				}
				//all litter is now in struc/metab pools
				pCL->fCFineRootLitter = 0.0; 
				pCL->fNFineRootLitter = 0.0;
				pCL->fCStrcLitter += sC * gpm2TOkgpha;
				pCL->fNStrcLitter += sN * gpm2TOkgpha;
				pCL->fLignFracStrcLit  = newlig;
				pCL->fCMtbLitter += mC * gpm2TOkgpha;
				pCL->fNMtbLitter += mN * gpm2TOkgpha;
			}
		}//loop over soil layers

//************************************************************************************************
// IV)  Calculates the decomposition ratio.
// 	Determine C/N of new material entering 'Box B'. 	
// 	The C/N ratios for structural and wood are computed once per day here;
//************************************************************************************************
if(par.site == 1) //forest
{
// Fine branch
// Ratio for new SOM1 from decomposition of Fine Branches
		C=pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
		rneww1[0]= AboveGroundDecompRatio(C,N,2.0f,fixed);
// Ratio for new SOM2 from decomposition of Fine Branches
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
		radds1=fixed.rad1p[0] + fixed.rad1p[1] * 
			( AboveGroundDecompRatio (C,N,2.5f,fixed)- fixed.pcemic[1]);
		rneww1[1]= rneww1[0]+ radds1;
// Large Wood
// Ratio for new SOM1 from decomposition of Large Wood
		C=pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
	    rneww2[0] =AboveGroundDecompRatio(C,N,2.0f,fixed);
// Ratio for new SOM2 from decomposition of Large Wood
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
	    rneww2[1] = rneww2[0] + fixed.rad1p[0] + fixed.rad1p[1] * 
			( AboveGroundDecompRatio (C,N,2.0f,fixed)- fixed.pcemic[1]);
	    rneww2[1] =  max (rneww2[1],fixed.rad1p[2]);
// Coarse Roots
// Ratio for new SOM1 from decomposition of Coarse Roots
		rneww3[0] = fixed.varat1[0];
// Ratio for new SOM2 from decomposition of Coarse Roots
		rneww3[1] = fixed.varat2[1]; 
}//forest
// Surface Structural
// Ratios for new som1 from decomposition of AG structural
		C=pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
		N=pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
		rnewas[0] = AboveGroundDecompRatio(C,N,2.5f,fixed);
// Ratio for new SOM2 from decomposition of AG strutural
		radds1=fixed.rad1p[0] + fixed.rad1p[1] * 
		( AboveGroundDecompRatio (C,N,2.5f,fixed)- fixed.pcemic[1]);
		rnewas[1] =max( fixed.rad1p[2], rnewas[0] + radds1);
// Soil Structural
// Ratios for new som1 from decomposition of BG structural
		rnewbs[0] = fixed.varat1[0];
// Ratio for new SOM2 from decomposition of BG strutural
		rnewbs[1] = fixed.varat2[0];


//   ***************************************************************************************
//		Decomposition Routines (CALL: simsom_dc.cpp line 280), do 4 times a day
//		 IV) DecomposeLitter  
//		  V) DecomposeWood  
//		 VI) DecomposeSOM  
//				Steps:       
//				1. Surface SOM1 decomposes to SOM2 with CO2 loss.    
//				2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss and leaching.         
//				3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss       
//				4. SOM3 decomposes to soil SOM1 with CO2 loss.
//   ***************************************************************************************
 
for(iday=1;iday<=TimeStepsPerDay;iday++)  //4 times a day
{
//************************************************************************************************
// IV) Decompose Litter
//************************************************************************************************

//SURFACE STRUCTURAL Material to SOM1[SRFC] and SOM2
	C		= pCh->pCProfile->fCStrLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNStrLitterSurf * kgphaTOgpm2;
	lig		= pCh->pCProfile->fLignFracStrLitSurf;
	Navail  = getNtoDepth(20.0f,exp_p);  //top 20cm 
	if (C >= 10e-7) 
	{
	tcflow=min(C , fixed.strmax[SRFC]* defacSRFC * fixed.dec1[SRFC] //C flow from A to B
		       * (float)exp(-fixed.pligst[SRFC]*lig) ) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rnewas[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCStrLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;   
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rnewas[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,  20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow, 20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByStructLitter += minNflow * gpm2TOkgpha; ;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( (float)ret, 20.0f, exp_p);			//B += minflow
		ImmByStructLitter +=  ret * gpm2TOkgpha; ;
		} 

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
	 
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStrLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rnewas[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStrLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStrLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByStructLitter += minNflow * gpm2TOkgpha; ;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByStructLitter +=  ret * gpm2TOkgpha; ;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7

//SOIL STRUCTURAL Material to SOM1[SOIL] and SOM2
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
		pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCStrcLitter * kgphaTOgpm2; 
	N		= pCL->fNStrcLitter * kgphaTOgpm2; 
	lig		= pCL->fLignFracStrcLit;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
	if (C >= 10e-7) 
	{
	tcflow=min(C , fixed.strmax[SOIL]* pdecf[i] *  anerb  * fixed.dec1[SOIL] //C flow from A to B
		       * (float)exp(-fixed.pligst[SOIL]*lig) ) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rnewbs[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCL->fCStrcLitter -= tcflow * gpm2TOkgpha;
		pCL->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; ;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rnewbs[1],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		MinByStructLitter += minNflow * gpm2TOkgpha; ;	 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if (minNflow > 1e-7)
		{
		pCL->fCHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4 *  (float)fabs(minNflow) * gpm2TOkgpha; //Mineral -= minflow
		pCL->fNO3N -= frNO3 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		pCL->fNImmobR  +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		ImmByStructLitter +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		}
		}
 
		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SOIL];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
		 
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast += CtoSOM1 * gpm2TOkgpha;;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNStrcLitter -= mineralFlow * gpm2TOkgpha; 
		MinByStructLitter += mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNStrcLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNStrcLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByStructLitter += minNflow * gpm2TOkgpha;  
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral -= minflow				
		pCL->fNO3N -= frNO3 *  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByStructLitter +=  (float)fabs(minNflow) * gpm2TOkgpha; 
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha; 
 		}
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7
	}//loop over all layer

 
//METABOLIC surface to SOM1
	C = pCh->pCProfile->fCMtbLitterSurf * kgphaTOgpm2;
	N = pCh->pCProfile->fNMtbLitterSurf * kgphaTOgpm2;
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-6)
	{
	// Determine C/E ratios for flows to SOM1
		rcemet = AboveGroundDecompRatio(C,N,2.5f,fixed);

	// If decomposition can occur
	if (CanDecompose(C,N,rcemet,Navail) )
	{
		// C Flow
		// Compute total C flow out of metabolic into SOM1[layer]
		tcflow = C * defacSRFC * fixed.dec2[SRFC] * anerb * dtDecompDC;
		// Make sure metab does not go negative.
		tcflow = min ( tcflow, C );
		co2loss = tcflow * fixed.pmco2[SRFC];

		cfmes1 = tcflow - co2loss;
		pCh->pCProfile->fCMtbLitterSurf -= cfmes1 * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM1(cfmes1,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNMtbLitterSurf -= mineralFlow * gpm2TOkgpha; 
		MinByMetabLitter += mineralFlow * gpm2TOkgpha; 
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(cfmes1,rcemet,C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNMtbLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM1(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNMtbLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByMetabLitter += minNflow * gpm2TOkgpha;
		}
	if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow), 20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM1( ret, 20.0f, exp_p);			//B += minflow
		ImmByMetabLitter += ret * gpm2TOkgpha;
		}
	}// if candecomp
	} //if (pCh->pCProfile->fCMtbLitterSurf >= 10e-6)


//METABOLIC soil to SOM1
	// Determine C/E ratios for flows to SOM1
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	if(getNtoDepth(20.0f,exp_p) <= 0.0f)			rcemet=fixed.varat1[0]; //set ratio to max. allowed
 	else if(getNtoDepth(20.0f,exp_p) <= fixed.varat1[2])	rcemet=fixed.varat1[1]; //set ratio to min. allowed
	else	//  aminrl > 0 and <= varat(2,element)
	rcemet=(1.0f - getNtoDepth(20.0f,exp_p)/fixed.varat1[2])*(fixed.varat1[0]-fixed.varat1[1])+fixed.varat1[1];

	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCMtbLitter * kgphaTOgpm2;
	N		= pCL->fNMtbLitter * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; // 
	if (C >= 10e-7)
	{	
	// If decomposition can occur
	if (CanDecompose(C,N,rcemet,Navail) )
		{
		// C Flow
		// Compute total C flow out of metabolic into SOM1[layer]
		tcflow = C * pdecf[i] * fixed.dec2[SOIL] * anerb * dtDecompDC;
		// Make sure metab does not go negative.
		tcflow = min ( tcflow, C );	    //gross  C flow to SOM1
		if(tcflow > 10e-7)
		{
		co2loss=tcflow * fixed.pmco2[SOIL];
		tcflow -= co2loss;					//net  C flow to SOM1
		 
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast  += tcflow * gpm2TOkgpha; 
		pCL->fCMtbLitter  -= tcflow * gpm2TOkgpha; 
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		//mineralFlow = min (mineralFlow,Navail );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNMtbLitter -= mineralFlow * gpm2TOkgpha; 
		MinByMetabLitter += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// Compute and schedule N flow from A to B.
		ScheduleNFlow(tcflow,rnewbs[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNMtbLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNMtbLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByMetabLitter += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N);
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByMetabLitter +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		}//if(CtoSOM1 > 10e-7)
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)
	} // loop over all layer

//_______________________________________________________________________________________

//		 V) DecomposeWood  
//_______________________________________________________________________________________

if(par.site == 1) //forest
{
// FINE BRANCHES
	C		= pCh->pCProfile->fCBranchLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNBranchLitterSurf * kgphaTOgpm2;
	lig		= par.wdlig[1];
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-7) 
	{
	tcflow=C * defacSRFC * par.decw1 * (float)exp(-fixed.pligst[SRFC]*lig) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww1[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCBranchLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww1[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( ret,20.0f, exp_p);			//B += minflow	
		ImmByWood +=  ret * gpm2TOkgpha;
		} 
	
		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1

		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNBranchLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww1[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNBranchLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNBranchLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 

// LARGE WOOD	
	C		= pCh->pCProfile->fCStemLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNStemLitterSurf * kgphaTOgpm2;
	lig		= par.wdlig[2];
	Navail  = getNtoDepth(20.0f,exp_p);
	if (C >= 10e-7) 
	{
	tcflow= C * defacSRFC * par.decw2 * (float)exp(-fixed.pligst[SRFC]*lig) * dtDecompDC;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww2[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		pCh->pCProfile->fCStemLitterSurf -= tcflow * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(CtoSOM2,20.0f, exp_p);
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		mineralFlow = FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// Compute and schedule N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww2[1],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);			 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation 
		{
		pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow
		FlowNintoSOM2( ret, 20.0f, exp_p);			//B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		} 

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SRFC];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
	 
		// C flow
		CO2intoSOIL(co2loss,20.0,exp_p);
		pCh->pCProfile->fCMicLitterSurf += CtoSOM1 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow=FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNStemLitterSurf		 -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;

		// N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww2[0],C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNStemLitterSurf -= orgNflow * gpm2TOkgpha; //A -= orgflow
		pCh->pCProfile->fNMicLitterSurf += orgNflow * gpm2TOkgpha; //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNStemLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinByWood += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);//Mineral -= minflow
		pCh->pCProfile->fNMicLitterSurf +=  ret * gpm2TOkgpha;		 //B += minflow
		ImmByWood +=  ret * gpm2TOkgpha;
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 

// COARSE ROOTS	
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
		pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCGrossRootLitter * kgphaTOgpm2;
	N		= pCL->fNGrossRootLitter * kgphaTOgpm2;
	dummy=C/N;

	lig		= par.wdlig[3] ;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //
	if (C >= 10e-7) 
	{
	tcflow= C * pdecf[i] * par.decw3 * (float)exp(-fixed.pligst[SOIL]* lig) * dtDecompDC * anerb;
	// Decompose structural into som1 and som2 with CO2 loss. (Decompose Lignin)
	// See if Box A can decompose to SOM1.
	// If it can go to som1, it will also go to som2.
	// If it can't go to som1, it can't decompose at all.
	if (CanDecompose (C,N,rneww3[0],Navail))
	{
		// Decompose Box A to SOM2
		// -----------------------
		// C Flow
		CtoSOM2 = tcflow*lig;	    //gross  C flow to SOM2
		co2loss = CtoSOM2 * fixed.rsplig;
		CtoSOM2 -= co2loss;//net C flow to SOM2

		// C flow
		pCL->fCGrossRootLitter -= tcflow * gpm2TOkgpha;
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusSlow  += CtoSOM2 * gpm2TOkgpha;
		
		// Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// N flow from A to B.
		ScheduleNFlow(CtoSOM2,rneww3[1],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByWood += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByWood +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}

		// Decompose Box A to SOM1
		// -----------------------
		// C Flow
		CtoSOM1 = tcflow - CtoSOM2 - co2loss;	    //gross  C flow to SOM1
		if(CtoSOM1 > 10e-7)
		{
		co2loss = CtoSOM1 * fixed.p1co2a[SOIL];
		CtoSOM1 -= co2loss;							//net  C flow to SOM1
		 
		// C flow
		pCL->fCO2ProdR	+= co2loss * gpm2TOkgpha;
		pCL->fCO2C		+= co2loss * gpm2TOkgpha;
		pCL->fCHumusFast  += CtoSOM1 * gpm2TOkgpha;;
		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNGrossRootLitter -= mineralFlow * gpm2TOkgpha; 
		MinByWood += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// N flow from A to B.
		ScheduleNFlow(CtoSOM1,rneww3[0],C,N,Navail,&orgNflow, &minNflow);
		pCL->fNGrossRootLitter -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNGrossRootLitter -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinByWood += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmByWood +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		}//if(CtoSOM1 > 10e-7)
	}//if Candecompose	 
	}//C >= 10e-7 
	}//loop over all layer
}//if site is a forest
//_______________________________________________________________________________________
//		 VI) DecomposeSOM (dtDecomp, anerb, defac);
//_______________________________________________________________________________________

//	VI) 1. Surface SOM1 decomposes to SOM2 with CO2 loss.   
	C		= pCh->pCProfile->fCMicLitterSurf * kgphaTOgpm2;
	N		= pCh->pCProfile->fNMicLitterSurf * kgphaTOgpm2;
	Navail  = getNtoDepth(20.0f,exp_p);
if( C >= 10e-7 ) 
{
//	determine C/N ratios for flows to SOM2
	rceto2=C/N + fixed.rad1p[0] +  fixed.rad1p[1]*(C/N - fixed.pcemic[1]);
	rceto2=max(rceto2,fixed.rad1p[2]);
//  If decomposition can occur, schedule flows associated with /
//  respiration and decomposition
	if (CanDecompose (C, N , rceto2, Navail) )
	{
		// Decompose Box A to SOM2
		// -----------------------
		// Calculate C Flow
		// total C flow out of surface microbes.
		microbeC=C * defacSRFC * fixed.dec3[SRFC] * dtDecompDC;    
		co2loss = microbeC * fixed.p1co2a[SRFC];
		// cfsfs2 is C Flow from SurFace som1 to Som2
		cfsfs2 = microbeC - co2loss;

		//C flow
		pCh->pCProfile->fCMicLitterSurf -= microbeC * gpm2TOkgpha;
		CO2intoSOIL(co2loss,20.0,exp_p);
		FlowCintoSOM2(cfsfs2,20.0f, exp_p);
		
		//Mineralization associated with respiration
		mineralFlow = co2loss * N/C;
		mineralFlow = FlowNintoMineralSoil(mineralFlow,20.0f,0.0,exp_p);
		pCh->pCProfile->fNMicLitterSurf -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha;
		//N flow from A to B.
		ScheduleNFlow(cfsfs2,rceto2,C,N,Navail,&orgNflow, &minNflow);
		pCh->pCProfile->fNMicLitterSurf -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		FlowNintoSOM2(orgNflow,20.0f, exp_p);	 //B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCh->pCProfile->fNMicLitterSurf -= minNflow * gpm2TOkgpha;	//A -= minflow
		FlowNintoMineralSoil(minNflow,20.0f,0.0f,exp_p);	//Mineral += minflow
		MinBySOM += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		//Flow can be limited by available N, this is done in FlowNfromMineralSoil
		//the return value gives the restricted flow of mineral N		 
		ret = FlowNfromMineralSoil( (float)fabs(minNflow),20.0f,-1,exp_p);	//Mineral -= minflow	
		FlowNintoSOM2( ret, 20.0f, exp_p);			//B += minflow
		ImmBySOM +=  ret * gpm2TOkgpha;
		}
	}//if canDecompose
} // if amount is significant

 

//	VI) 2. Soil SOM1 decomposes to SOM2 and SOM3 with CO2 loss

	// Determine C/E ratios for flows to SOM2 and SOM3  
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	rceto2=rceto3=0.0f;
	if(getNtoDepth(20.0f, exp_p) <=0.0f ){rceto2 = fixed.varat2[0];	rceto3 = fixed.varat3[0];} //set to maximum allowed ??<=??
	else if (getNtoDepth(20.0f, exp_p) > fixed.varat2[2]){
		rceto2 = fixed.varat2[1];rceto3 = fixed.varat3[1];} //set to minimal allowed
	else // >0 and <= varat2_2
	{
		rceto2 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat2[2])*(fixed.varat2[0]-fixed.varat2[1])+fixed.varat2[1];
		rceto3 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat3[2])*(fixed.varat3[0]-fixed.varat3[1])+fixed.varat3[1];
	}
	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusFast * kgphaTOgpm2;
	N		= pCL->fNHumusFast * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; 
	if (C >= 10e-7)
	{	
	// If soil som1 can decompose to som2, it will also go to som3.
	// If it can't go to som2, it can't decompose at all.
	// If decomposition can occur
	if (CanDecompose(C,N,rceto2,Navail) )
		{
		//C Flow
		efftext = fixed.peftxa + fixed.peftxb * pSL->fSand*0.01f;
		microbeC =C * pdecf[i] * fixed.dec3[SOIL] * dtDecompDC * anerb * efftext;//cltfac=1 for no cultivation
		co2loss = microbeC * fixed.p1co2a[SOIL];
		cfs1s3 = microbeC * (fixed.ps1s3[0]+fixed.ps1s3[1]*pSL->fClay*0.01f) *
	    			(fixed.animp * (1.0f - anerb) + 1.0f); // C to SOM3 	
			// SOM2 gets what's left of microbeC.
		cfs1s2 = microbeC - co2loss - cfs1s3;
		//C flow  
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusStable  += cfs1s3 * gpm2TOkgpha;
		pCL->fCHumusSlow  += cfs1s2 * gpm2TOkgpha;
		pCL->fCHumusFast   -= microbeC * gpm2TOkgpha; 		
 		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusFast -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha; 
		pCL->fMinerR += mineralFlow * gpm2TOkgpha; 
		// to SOM3
		// N flow from A to B.
		ScheduleNFlow(cfs1s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
	    pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusStable  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		
		//to SOM2
		//N flow from A to B.
		ScheduleNFlow(cfs1s2,rceto2,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusFast -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusSlow  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusFast -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusSlow  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;		
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
		
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-7)
	} // loop over all layer


//	VI)	3. SOM2 decomposes to soil SOM1 and SOM3 with CO2 loss   

	// Determine C/E ratios for flows to SOM1 and SOM3  
	// This is done by analysing the first 20cm of soil, but used in all XN layers.
	rceto1= 0.0f;rceto3= 0.0f;
	if(getNtoDepth(20.0f, exp_p) <=0.0f ){rceto1 = fixed.varat1[0];rceto3 = fixed.varat3[0];}  //set to maximum allowed  
	else if (getNtoDepth(20.0f, exp_p) > fixed.varat1[2]){ rceto1 = fixed.varat1[1]; rceto3 = fixed.varat3[1];}  //set to minimal allowed
	else // >0 and <= varat1_2
	{	rceto1 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat1[2])*(fixed.varat1[0]-fixed.varat1[1])+fixed.varat1[1];
		rceto3 = (1.0f-getNtoDepth(20.0f, exp_p)/fixed.varat3[2])*(fixed.varat3[0]-fixed.varat3[1])+fixed.varat3[1];
	}

	//loop over all layer
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusSlow * kgphaTOgpm2;
	N		= pCL->fNHumusSlow * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; 
	if (C >= 10e-7)
	{	
	// If soil som2 can decompose to som1, it will also go to som3.
	// If it can't go to som1, it can't decompose at all.
	// If decomposition can occur
	if (CanDecompose(C,N,rceto1,Navail) )
		{
		// C Flow
		tcflow=C * pdecf[i] * fixed.dec5 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
		co2loss = tcflow * fixed.p2co2;
		cfs2s3 = tcflow * (fixed.ps2s3[0]+fixed.ps2s3[1]*pSL->fClay*0.01f) *
	    (fixed.animp * (1.0f - anerb) + 1.0f);
		cfs2s1 = tcflow - co2loss - cfs2s3;//SOM1 gets what's left of tcflow.
		// C flow  
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusFast    += cfs2s1 * gpm2TOkgpha;
		pCL->fCHumusStable  += cfs2s3 * gpm2TOkgpha;
		pCL->fCHumusSlow    -= tcflow * gpm2TOkgpha; 		
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;	
		mineralFlow = min(mineralFlow, pCL->fNHumusSlow * kgphaTOgpm2);
		pCL->fNH4N			+= mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusSlow	-= mineralFlow * gpm2TOkgpha; 
		MinBySOM			+= mineralFlow * gpm2TOkgpha;
		pCL->fMinerR		+= mineralFlow * gpm2TOkgpha ;
		//SB! Fehler in N Bilanz im Logfile, nicht in der Grafischen Ausgabe,
		//nur hier... funktioniert, wenn oberen 4 Zeilen auskommentiert sind.
		//Fehler erscheint sogar täglich, wenn die unteren beiden Zeilen auskommentiert werden ???

		//pCL->fNH4N	+= 0.1;
		//pCL->fMinerR	+= 0.1;

		if (pCL->fNO3N < 0)		Message(1,"Warning: neg. Mineral NO3N 3a");		
		if (pCL->fNH4N < 0)		Message(1,"Warning: neg. Mineral NH4N 3a");
		//to SOM3
		// N flow from A to B.
		ScheduleNFlow(cfs2s3,rceto3,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusStable  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusStable  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}

		//to SOM1
		// N flow from A to B.
		ScheduleNFlow(cfs2s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusSlow -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fNHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusSlow -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha;
		}
		if(pTi->pSimTime->iJulianDay==245 && iday==2 && i==15)
		{
			int dummy=666;
		}

		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fNHumusFast  +=  (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4   //Mineral -= minflow
							*  (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							*  (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM +=  (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR +=  (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}		
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-7)
 
	} // loop over all layer


//	IV)	4. SOM3 decomposes to soil SOM1 with CO2 loss.

	// Determine C/E ratios for flows to SOM1 : already done
	//loop
	for(i=0,pSL=pSo->pSLayer->pNext,pCL=pCh->pCLayer->pNext;
	pSL->pNext!=NULL;i++,pSL=pSL->pNext,pCL=pCL->pNext)
	{
	C		= pCL->fCHumusStable * kgphaTOgpm2;
	N		= pCL->fNHumusStable * kgphaTOgpm2;
	Navail  = (pCL->fNO3N + pCL->fNH4N)* kgphaTOgpm2; //

	if (C >= 10e-7)
	{	
	// If decomposition can occur
	if (CanDecompose(C,N,rceto1,Navail) )
		{
		// Calculate C Flow
		// C flow out of metabolic into SOM1[layer]
		tcflow = C * pdecf[i] * fixed.dec4 * dtDecompDC * anerb;//cltfac2=1 for no cultivation
		co2loss=tcflow * fixed.p3co2;
		cfs3s1 = tcflow - co2loss;
		 
		// C flow
		pCh->pCLayer->fCO2ProdR += co2loss * gpm2TOkgpha;
		pCh->pCLayer->fCO2C += co2loss * gpm2TOkgpha;
		pCL->fCHumusFast   += cfs3s1 * gpm2TOkgpha;;
		pCL->fCHumusStable  -= tcflow * gpm2TOkgpha;;	
		// Mineralization associated with respiration
		mineralFlow=co2loss * N/C;
		mineralFlow = min (mineralFlow,pCL->fNHumusStable * kgphaTOgpm2 );
		pCL->fNH4N += mineralFlow * gpm2TOkgpha; // all to ammonium
		pCL->fNHumusStable -= mineralFlow * gpm2TOkgpha; 
		MinBySOM += mineralFlow * gpm2TOkgpha;
		pCL->fMinerR += mineralFlow * gpm2TOkgpha;
		// N flow from A to B.
		ScheduleNFlow(cfs3s1,rceto1,C,N,Navail,&orgNflow, &minNflow);
		pCL->fNHumusStable -= orgNflow * gpm2TOkgpha;	//A -= orgflow
		pCL->fCHumusFast  += orgNflow * gpm2TOkgpha;	//B += orgflow
		if( minNflow >= 0.0f)//Mineralisation
		{
		pCL->fNHumusStable -= minNflow * gpm2TOkgpha;	//A -= minflow
		pCL->fNH4N += minNflow * gpm2TOkgpha; 	//Mineral += minflow // all to ammonium;
		MinBySOM += minNflow * gpm2TOkgpha;
		pCL->fMinerR += minNflow * gpm2TOkgpha; 
		}
		if ( minNflow < 0.0f)//Immobilisation
		{
		minNflow = (float)min (fabs(minNflow), (pCL->fNH4N + pCL->fNH4N)*kgphaTOgpm2 ); //limited by availibility
		if(minNflow > 1e-7)
		{
		pCL->fCHumusFast  += (float)fabs(minNflow) * gpm2TOkgpha;			//B += minflow
		frNH4 = pCL->fNH4N / (pCL->fNO3N+pCL->fNH4N); //calculate the factor in extra step -> if not: numerical errors appear!!
		frNO3 = pCL->fNO3N / (pCL->fNO3N+pCL->fNH4N); 
		pCL->fNH4N -= frNH4  //Mineral -= minflow
							* (float)fabs(minNflow) * gpm2TOkgpha;				//Mineral N is donated by Ammonium and Nitrate	
		pCL->fNO3N -= frNO3
							* (float)fabs(minNflow) * gpm2TOkgpha;	
		pCL->fNO3N = (pCL->fNO3N < 0.0) ? (float)0.0 : pCL->fNO3N; //numerical issue
		pCL->fNH4N = (pCL->fNH4N < 0.0) ? (float)0.0 : pCL->fNH4N; //numerical issue
		ImmBySOM += (float)fabs(minNflow) * gpm2TOkgpha;
		pCL->fNImmobR += (float)fabs(minNflow) * gpm2TOkgpha;
		}
		}
	} // if candecomp
	} //if (pCh->pCLayer->fCMtbLitter  >= 10e-6)

	if (pCL->fNO3N < 0)		Message(1,"Warning: neg. Mineral NO3N ");		
	if (pCL->fNH4N < 0)		Message(1,"Warning: neg. Mineral NH4N ");
 
	} // loop over all layer

} //4 times a day

//*******************************************************************************
//CEAN UP HEAP
free(pdecf);
//*******************************************************************************

	
//*******************************************************************************
//	write ExpertN variables 
//	for result files and Denitrification modul
//*******************************************************************************
//write mineralisation/immobilisation by pools into first real XN layer
//values could be calculated per layer, but thats not implemented yet.
pCh->pCLayer->pNext->fHumusMinerR   = MinBySOM ;
pCh->pCLayer->pNext->fLitterMinerR  = MinByStructLitter + MinByMetabLitter+ MinByWood;	 
// fNHumusImmobR is not summed up in XN, use Day instead!!
pCh->pCProfile->fNHumusImmobDay    = ImmBySOM ;
pCh->pCLayer->pNext->fNLitterImmobR = ImmByInput + ImmByStructLitter
										 + ImmByMetabLitter+ ImmByWood - DailyCorrection;
//write humus and FOS für Ceres-N Denitrifikation
for(pCL=pCh->pCLayer->pNext, pSL = pSo->pSLayer->pNext ; pSL->pNext != NULL ;
												pSL=pSL->pNext,pCL=pCL->pNext) 
{
		pSL->fCHumus = pCL->fCHumusSlow + pCL->fCHumusFast + pCL->fCHumusStable;
		//like Mineralisation_Fagus: just the fine root litter is written to FOM[i], no coarse roots
		pCL->afCOrgFOMFrac[1]= pCL->fCMtbLitter + pCL->fCStrcLitter; 
}

//*******************************************************************************
// write resultfile: "century.out"
//******************************************************************************* 
//open file
//if(outf==NULL) ... exception handling
if (pTi->pSimTime->fTimeAct == 0)
	outf = fopen("result/century.out", "w");	//Create file
else outf = fopen("result/century.out", "a+");	//append  

fprintf(outf, "%f ",pTi->pSimTime->fTimeAct);
//Variables
//Profile
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fNImmobR ); 
	fprintf(outf, "%f ",ImmByInput );
	fprintf(outf, "%f ",ImmBySOM );
	fprintf(outf, "%f ",ImmByStructLitter );
	fprintf(outf, "%f ",ImmByMetabLitter );
	fprintf(outf, "%f ",ImmByWood );
	//8
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fMinerR ); 
	fprintf(outf, "%f ",MinBySOM );
	fprintf(outf, "%f ",MinByStructLitter );
	fprintf(outf, "%f ",MinByMetabLitter );
	fprintf(outf, "%f ",MinByWood );
	//13ff. NNM
	fprintf(outf, "%f ",pCh->pCLayer->pNext->fMinerR - pCh->pCLayer->pNext->fNImmobR );
	fprintf(outf, "%f ",MinBySOM - ImmBySOM);
	fprintf(outf, "%f ",MinByStructLitter -ImmByStructLitter);
	fprintf(outf, "%f ",MinByMetabLitter -ImmByMetabLitter);
	fprintf(outf, "%f ",MinByWood -ImmByWood);
	//18: CumCorrection
	fprintf(outf, "%f ",CumCorrection);
	//new line
	fprintf(outf, "\n"); 
	//close file
	fclose(outf);

}// if new day
return 0.0;
}


struct fixedparameter SetFixedParameters()
{
struct fixedparameter fixed;
	fixed.aneref[0]=1.5f;
	fixed.aneref[1]=3.0f;
	fixed.aneref[2]=0.3f;
	fixed.pprpts[0]=0.0f;
	fixed.pprpts[1]=1.0f;
	fixed.pprpts[2]=0.8f;
	fixed.varat1[0]=18.0f;
	fixed.varat1[1]=8.0f;
	fixed.varat1[2]=2.0f;
	fixed.varat2[0]=40.0f;
	fixed.varat2[1]=12.0f;
	fixed.varat2[2]=2.0f;
	fixed.varat3[0]=20.0f;
	fixed.varat3[1]=6.0f;
	fixed.varat3[2]=2.0f;
	fixed.animp=5.0f;
	fixed.damr[0]=0.0f;
	fixed.damr[1]=0.02f;
	fixed.damrmn=15.0f;
	fixed.dec1[0]=3.9f;
	fixed.dec1[1]=4.9f;
	fixed.dec2[0]=14.8f;
	fixed.dec2[1]=18.5f;	
	fixed.dec3[0]=6.0f;
	fixed.dec3[1]=7.3f;	
	fixed.p1co2a[0]=0.6f;	
	fixed.p1co2a[1]=0.17f;
	fixed.pligst[0]=3.0f;	
	fixed.pligst[1]=3.0f;
	fixed.spl[0]=0.85f;
	fixed.spl[1]=0.013f;
	fixed.strmax[0]=5000.0f;
	fixed.strmax[1]=5000.0f;
	fixed.pmco2[0]=0.55f;
	fixed.pmco2[1]=0.55f;
	
	fixed.ps1s3[0]=0.003f;
	fixed.ps1s3[1]=0.032f;
	fixed.ps2s3[0]=0.003f;
	fixed.ps2s3[1]=0.009f;
	fixed.dec4=0.0045f;
	fixed.dec5=0.2f;
	fixed.favail=0.9f;
	fixed.frfrac[0]= 0.01f;//      For trees only:frfrac[0] - minimum possible allocation fraction to fine roots
	fixed.frfrac[1]= 0.8f;//                    (tree.100)
//        frfrac[1] - maximum possible allocation fraction to fine roots
	fixed.p2co2=0.55f;
	fixed.p3co2=0.55f;
	fixed.pabres=100.0f;
	fixed.pcemic[0]=16.0f;
	fixed.pcemic[1]=10.0f;
	fixed.pcemic[2]=0.02f;
	fixed.peftxa=0.25f;
	fixed.peftxb=0.75f;
	fixed.rad1p[0]=12.0f;
	fixed.rad1p[1]=3.0f;
	fixed.rad1p[2]=5.0f;
	fixed.rcestr=200.0f;
	fixed.rictrl=0.015f;
	fixed.riint=0.8f;
	fixed.rsplig=0.3f;
	fixed.teff[0]= 15.4f;	//different in daily version, see online manual or Daycent code
	fixed.teff[1]= 11.75f;
	fixed.teff[2]= 29.70f,
	fixed.teff[3]= 0.031f;
return fixed;
}


struct parameter ReadParameter()
{
    struct parameter par;
	FILE * Ctree;
	char str [100];
	Ctree=fopen("param/century.cfg", "r");
	if (Ctree == NULL) Message(1,"Error opening century.cfg file");	               
		fgets(str,100,Ctree);par.site = strtol(str,NULL,10);
		fgets(str,100,Ctree);par.drain= (float)strtod(str,NULL);	
		fgets(str,100,Ctree);par.maxt= (float)strtod(str,NULL);	
		fgets(str,100,Ctree);par.SurfaceSOM1C= (float)strtod(str,NULL);	
		fgets(str,100,Ctree);par.SurfaceSOM1CtoN= (float)strtod(str,NULL);	
		fgets(str,100,Ctree);par.SurfaceLitterC= (float)strtod(str,NULL);	
		fgets(str,100,Ctree);par.SurfaceLitterCtoN= (float)strtod(str,NULL);
		fgets(str,100,Ctree);
		fgets(str,100,Ctree);par.decw1 = (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.decw2= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.decw3= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[0][0]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[0][1]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[0][2]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[0][3]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[0][4]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[1][0]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[1][1]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[1][2]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[1][3]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[1][4]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[2][0]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[2][1]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[2][2]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[2][3]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.cerfor[2][4]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.wdlig[0]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.wdlig[1]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.wdlig[2]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.wdlig[3]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.wdlig[4]= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.wood1C= (float)strtod(str,NULL);
		fgets(str,100,Ctree);par.wood2C= (float)strtod(str,NULL);

	fclose(Ctree);	
	return par;
	}


/********************************************************************
Partition Residue: Input: source pool of residue, C amount, ratio residue: N/C, lignin fraction
Partions the residue into structual and metabolic and schedules flows.

returns direct adsorbed mineral N [g/m2]

NB: no direct adsorption of Surface litter
    arguments: C/N/lig of TMP struc/metab layer
*********************************************************************/

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
						struct fixedparameter fixed)
{
float ratioCtotalN; // C/N ratio after direct adsorption
float fractionN; //  fraction of nitrogen in residue after direct adsorption.
float rlnres;	//Lignin/nitrogen ratio of residue
float frmet;  //fractio of C that goes to metabolic
float caddm, cadds; //total C added to metabolic/structural
float  amtadded;
float fligst;		// fligst is the lig. fraction of incoming structural residue
float N=C*recres; //N content in residue
float directadsorbedN=0.0f; //amount of Mineral N from mineral pool that is adsorbed by the residue
						//return value
float dummy; //debugger
if (C >= 10e-7)
{
	// Direct absorption of mineral element by residue
	// (mineral will be transferred to donor compartment and then
	// partitioned into structural and metabolic using flow routines.)
	// If minerl(SRFC,e) is negative then directAbsorbE = zero.

     if (Navailable < 0.0f) directadsorbedN=0.0f; 
	 else
	 {
		directadsorbedN=fixed.damr[layer]*Navailable*(float)min(1.0, C/fixed.pabres); //Change  it to min?? In Daycent: max makes no sense
	 }
    
	// If C/E ratio is too low, transfer just enough to make
	// C/E of residue = damrmn
	if (N+directadsorbedN <= 0.0f) ratioCtotalN=0.0f;
	else ratioCtotalN=C/(N+directadsorbedN);
	if(ratioCtotalN < fixed.damrmn) 
	{
		directadsorbedN = C/fixed.damrmn - N;
	}
	directadsorbedN= max(directadsorbedN,0.0f);

	//Partition carbon into structural and metabolic fraction of residue
	fractionN= (N+directadsorbedN) / (C*2.5f);
	
	//Carbon added to metabolic
	rlnres=frlig/fractionN;		// Lignin/nitrogen ratio of residue
	frmet= fixed.spl[0] + fixed.spl[1]*rlnres;//fractio of C that goes to metabolic
    // Make sure the fraction of residue which is lignin isn't
    // greater than the fraction which goes to structural. 
	frmet=min(frmet, 1.0f-frlig);
	frmet=max(frmet, 0.2f); // at least 20% goes to metabolic
	//Compute amounts to flow
	caddm=max(0.0f,C*frmet); // C added to metabolic
	cadds= C - caddm; //C added to structural
    // Adjust lignin content of structural.
    // fligst is the fraction of incoming structural residue
    // which is lignin;
    // Changed maximum fraction from .6 to 1.0  -lh 1/93
	fligst=(float)min(frlig*C/cadds, 1.0);
	//ligninFraction = (oldlig + newlig) / (oldC + addC);
	*newlig= (oldlig*oldstrucC + fligst*cadds ) / 
						(oldstrucC + cadds);
	dummy=*newlig;

	// C flow to structural / metabolic [layer]
	*newstrucC = cadds;
	*newmetabC = caddm;
	
	//N flow to struc/metab
	//structural
	// 14.10.2008 SB: insert min() to avoid amtadded>N
	//amtadded=cadds/fixed.rcestr; 
	amtadded=min( (N+directadsorbedN), cadds/fixed.rcestr); 
	*newstrucN =  amtadded;
	//metabolical
	amtadded=N + directadsorbedN - amtadded ;
	*newmetabN =  amtadded;
}// if C is significant

	return directadsorbedN;
}

//sum all available mineral N=Nitrate+Ammonium to depth
// return g/m2
float getNtoDepth (float depth, EXP_POINTER)
{
// Calculate N amount to depth [g/m2]
PSLAYER  pSL=pSo->pSLayer;// used to iterate over the original list
PCLAYER  pCH=pCh->pCLayer;
float N=0.0f; //return value
float d=0.0f;
for(pSL=pSL->pNext,pCH=pCH->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext,pCH=pCH->pNext) // exclude virtual layer at top and bottom
{
	d+= pSL->fThickness*0.1f;
	if (d <= depth)
	{
		N += pCH->fNO3N   
		   + pCH->fNH4N;
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < depth))
	{
		N += pCH->fNO3N   *(pSL->fThickness * 0.1f-(d-depth))/(pSL->fThickness*0.1f)
		   + pCH->fNH4N *(pSL->fThickness * 0.1f-(d-depth))/(pSL->fThickness*0.1f);
	} 
}
N *= kgphaTOgpm2;
return N;
}

float FlowNfromMineralSoil(float Ndemand,		// amount of mineral N wanted (gE/m^2) 
						float depth,			// soil depth to give mineral E (cm)
						float useNitrateFrac,	// max fraction removed from NOx pools range = (0-1),
												// -1 = remove in proportion to nitrate/ammonium available
						EXP_POINTER)	
													
{	
	PSLAYER  pSL=pSo->pSLayer->pNext;// XN Soil layer start with layer 1
	PCLAYER  pCL=pCh->pCLayer->pNext;
 
	float d=0.0f,removed=0.0f,totalNremoved=0.0f;
	int i;
	float nitrateFraction, ammoniumFraction;

	//	FlowNFromSoil
	//	Schedule a flow for a mineral element from the top simDepth of soil.
	//	Demand is limited by availability in the total soil.
	//	so no layer is completely depleted.
	//	Return the total amount of flow scheduled.
	if( getNtoDepth(depth, exp_p)< 0.0f) return 0;
	
	Ndemand= min(Ndemand, getNtoDepth (depth, exp_p));
	for(i= 0; pSL != NULL ; i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
	{
	// nitrate and ammonium fractions
	if(pCL->fNO3N <= 0.0 && pCL->fNH4N <= 0.0) 
	{
		totalNremoved = 0.0;
		break;
	}
	else
	{
		if (useNitrateFrac < 0.0f)
		{		  
			nitrateFraction = pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N);
			ammoniumFraction =1.0f - nitrateFraction;
		}
		else   // 0.0 <= useNitrateFrac <= 1.0;
		{
			nitrateFraction = min( useNitrateFrac, 
							 pCL->fNO3N / (pCL->fNO3N + pCL->fNH4N) );
			ammoniumFraction = 1.0f - nitrateFraction;
		}
			
		if(pCL->fNO3N <=0.0f)
		{
			nitrateFraction=0.0f;
			ammoniumFraction=1.0f;
		}
		if(pCL->fNH4N <=0.0f)
		{
			nitrateFraction=1.0f;
			ammoniumFraction=0.0f;
		}			
		//Flow from Soil
		d+= pSL->fThickness*0.1f;
		if (d <= depth)	//full layer
		{
			//nitrate
			if(pCL->fNO3N > 0.0f)
			{
			removed=Ndemand * nitrateFraction * (pSL->fThickness*0.1f)/depth;
			removed=min(removed, (pCL->fNO3N*kgphaTOgpm2)); 
			pCL->fNO3N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
			//ammonium
			if(pCL->fNH4N > 0.0f)
			{
			removed=Ndemand * ammoniumFraction * (pSL->fThickness*0.1f)/depth;
			removed=min(removed, (pCL->fNH4N*kgphaTOgpm2)); 
			pCL->fNH4N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
		} 
		if (d > depth && ((d-pSL->fThickness*0.1f) < depth))  //fractional layer
		{
			//nitrate
			if(pCL->fNO3N > 0.0f)
			{
			removed=Ndemand * nitrateFraction * ((pSL->fThickness*0.1f)-(d-depth))/depth;
			//removed=min(removed, (pCL->fNO3N*kgphaTOgpm2)); 
			pCL->fNO3N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
			//ammonium
			if(pCL->fNH4N > 0.0f)
			{
			removed=Ndemand * ammoniumFraction * ((pSL->fThickness*0.1f)-(d-depth))/depth;
			removed=min(removed, (pCL->fNH4N*kgphaTOgpm2)); 
			pCL->fNH4N -= removed * gpm2TOkgpha;
			pCL->fNImmobR += removed * gpm2TOkgpha;
			totalNremoved +=removed;
			}
		}
		if(pCL->fNO3N <0.0) pCL->fNO3N = 0.0;//numerical issue
 		if(pCL->fNH4N <0.0) pCL->fNH4N = 0.0;//numerical issue
	}//there is some mineral N at all
	}//loop over layer
		
	return totalNremoved;
}
float FlowNintoMineralSoil(float Ntoadd,			// amount of mineral N to add (gN/m^2)
							float depth,				// soil depth to receive mineral E (cm)
						float fractionToNitrate,	// fraction of N to go to Nitrate range = (0-1) 
						EXP_POINTER)	
													
{
 
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext; 
 
float d=0.0f,toadd=0.0f,debug;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		//nitrate
		toadd=Ntoadd * fractionToNitrate * (pSL->fThickness*0.1f)/depth;
		pCL->fNO3N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
		//ammonium
		toadd=Ntoadd * (1.0f-fractionToNitrate) * (pSL->fThickness*0.1f)/depth;
		pCL->fNH4N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		//nitrate
		debug=4;
		toadd=Ntoadd * fractionToNitrate * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNO3N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
		//ammonium
		toadd=Ntoadd * (1.0f-fractionToNitrate) * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNH4N += toadd * gpm2TOkgpha;
		pCL->fMinerR += toadd * gpm2TOkgpha;
	}
}

	return Ntoadd;
}

 
 /* Calculates if Immobilization or Mineralization occurs and calculates flows 
   from A to B and from/to the mineral pool.
  
*/
void ScheduleNFlow(				
	float CFlowAtoB, 
	float FlowRatioCtoN,
	float AC,float AN,	//C and N content of Box A (source)
	float availMinN,	//available mineral N for possible Immobilisation
	//return values
	float *orgNflow,
	float *minNflow		//negative value for immobilisation
	)
{
	float const NFlowFromA=CFlowAtoB * AN/AC; //proportional zum C Fluß, abhängig vom
 															//C/N Verhältniss in A																	//!!i.A.: nicht alles kommt in B an
//Immobilization 
	if(CFlowAtoB/NFlowFromA > FlowRatioCtoN)
	{
		// organic N flow from A to B
		*orgNflow = NFlowFromA;
		//calculate Immobilization
		*minNflow = -(float)min( availMinN, CFlowAtoB/FlowRatioCtoN - NFlowFromA);//neg value for immobilization
		//Schedule flow from mineral pool to pool B (immobilization flow)
	}
//Mineralization
	if(CFlowAtoB/NFlowFromA <= FlowRatioCtoN)
	{
		// N flow from A to B
	    *orgNflow = CFlowAtoB/FlowRatioCtoN;  
		//Schedule flow from pool A to mineral pool
		*minNflow = NFlowFromA - CFlowAtoB/FlowRatioCtoN; //pos. value for mineralization
	}

}

int CanDecompose (
	float const C,		// total C in Box A
	float const N,		//   N in Box A
	float const ratio, //   C/N,  ratios of new material being added to Box B
	float availMinN)	 
{
    int canDo = 1;	// flag for N = true
	// If there is no available mineral E
	if(N>=0){
	if ( availMinN <=10e-5 )
	{
	    // if C/E of Box A > C/E of new material
	    if (C / N > ratio)
	    {
		// Immobilization is necessary therefore
		// Box A cannot decompose to Box B.
		canDo = 0; //false
	    }
    }
	}
	if (N<=0) canDo=0; //false

    return ( canDo );
}
 

void decf20cm(	float * defac, float * anerb, //return values
				struct fixedparameter fixed,
				float drain,
				EXP_POINTER
				 )
{
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	float avgWFPS,a,b,c,d,e1,e2,base1,base2,wfunc,tfunc;
	float DailyPET,xh20,ratioH2OtoPET;
// a)Determine texture index (COARSE,MEDIUM, FINE, VERY FINE); 
	if (getSandtoDepth(20.0f,exp_p) > 70.0 ) textureindex=COARSE;
	if (getSandtoDepth(20.0f,exp_p) <= 70.0 && getSandtoDepth(20.0f,exp_p) >= 30.0  ) textureindex=MEDIUM;
	if (getSandtoDepth(20.0f,exp_p) < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"
 
// b)Determine water factor
	avgWFPS=getWFPStoDepth(20.0f, exp_p); //water filled pore space to 20cm depth									   
	switch(textureindex){
		case COARSE: a=0.55f; b=1.70f; c=-0.007f; d=3.22f; break;
		case MEDIUM: a=0.60f; b=1.27f; c=0.012f; d=2.84f;break;
		case FINE: a=0.60f; b=1.27f; c=0.012f; d=2.84f; break;//Daycent: no values here. ??
		case VERYFINE: a=0.60f; b=1.27f; c=0.012f; d=2.84f; break; //Where can it be set to VERYFINE?
	}
	//exponent bases
	base1=(avgWFPS - b)/(a-b);
	base2=(avgWFPS - c)/(a-c);
	//exponents
	e1=d*(b-a)/(a-c);
	e2=d;
    //water factor
	wfunc= (float)pow(base1,e1)*(float)pow(base2,e2);
	if(wfunc > 1.0f) wfunc=1.0;

// c)Determine temperature factor 
//	 Century uses temperature at 5cm depth
	tfunc=( fixed.teff[1] + (fixed.teff[2]/
		(float)PI)*(float)atan((float)PI*fixed.teff[3]*(pHe->pHLayer->pNext->fSoilTemp - fixed.teff[0])) ) 
		 /( fixed.teff[1] + (fixed.teff[2]/
		 (float)PI)*(float)atan((float)PI*fixed.teff[3]*(30.0f - fixed.teff[0])) ); // normalized to 30°C
	tfunc=min(1.2f,max(0.01f,tfunc));

// d)Compute Decomposition factor
	*defac=max(0.0f, wfunc*tfunc);
	*defac=wfunc*tfunc;

// e)Determine factor of anaerobic conditions 
	//drain: the fraction of excess water lost by drainage.
    //Anaerobic conditions (high soil water content) cause decomposition to decrease. 
	//The soil drainage factor (site parameter DRAIN) allows a soil to have differing degrees of wetness 
	//(e.g., DRAIN=1 for well drained sandy soils and DRAIN=0 for a poorly drained clay soil).
	DailyPET=0.1f* pWa->fPotETDay;
	if(DailyPET<=0.01f) DailyPET=0.01f;
    ratioH2OtoPET = (getH2OtoDepth(-1.0,exp_p) + 0.1f*pCl->pWeather->fRainAmount) //+ pMa->pIrrigation->fAmount causes fault
		                  /DailyPET;  
	*anerb=1.0;
		if(pCl->pWeather->fTempAve > 2.0 && ratioH2OtoPET > fixed.aneref[0]){
			xh20=(ratioH2OtoPET-fixed.aneref[0])*0.1f*pWa->fPotETDay*(1.0f-drain);
			if(xh20>0.0f){
			*anerb=max(fixed.aneref[2],
			( ((1.0f-fixed.aneref[2])/(fixed.aneref[0]-fixed.aneref[1])) * xh20/(0.1f*pWa->fPotETDay) + 1.0f ));
			}
		}
}//decf20cm

// decomposition factor of soil layer
float decfSL(
				float sand, // [%] in Soil layer
				float WFPS, // Water filled pore space = pWL->fContAct / pSo->pSLayer->fPorosity
				float SoilTemp, //pHe->pHLayer->fSoilTemp
				struct fixedparameter fixed,	
				EXP_POINTER
				 )
{
	float defac;  //return value
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	float a,b,c,d,e1,e2,base1,base2,wfunc,tfunc;
 
// a)Determine texture index (COARSE,MEDIUM, FINE, VERY FINE); 
	if (sand > 70.0 ) textureindex=COARSE;
	if (sand <= 70.0 && getSandtoDepth(20.0f,exp_p) >= 30.0  ) textureindex=MEDIUM;
	if (sand < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"
 
// b)Determine water factor										
	switch(textureindex){
		case COARSE:	a=0.55f; b=1.70f; c=-0.007f;d=3.22f; break;
		case MEDIUM:	a=0.60f; b=1.27f; c=0.012f; d=2.84f; break;
		case FINE:		a=0.60f; b=1.27f; c=0.012f; d=2.84f; break; //Daycent: no values here. ??
		case VERYFINE:	a=0.60f; b=1.27f; c=0.012f; d=2.84f; break; //Where can it be set to VERYFINE?
	}
	//exponent bases
	base1=(WFPS - b)/(a-b);
	base2=(WFPS - c)/(a-c);
	//exponents
	e1=d*(b-a)/(a-c);
	e2=d;
    //water factor
	wfunc= (float)pow(base1,e1)*(float)pow(base2,e2);
	if(wfunc > 1.0f) wfunc=1.0;

// c)Determine temperature factor 
//	 Century uses temperature at 5cm depth
	tfunc=( fixed.teff[1] + (fixed.teff[2]/(float)PI)*(float)((float)PI*fixed.teff[3]*(SoilTemp - fixed.teff[0])) ) 
		 /( fixed.teff[1] + (fixed.teff[2]/(float)PI)*(float)atan((float)PI*fixed.teff[3]*(30.0f     - fixed.teff[0])) ); // normalized to 30°C
	tfunc=min(1.2f,max(0.01f,tfunc));

// d)Compute Decomposition factor
	defac=max(0.0f, wfunc*tfunc);
	defac=wfunc*tfunc;
return defac;
}//decf soil layer

//returns percentage of sand content 
float getSandtoDepth(float depth, EXP_POINTER)
{
float sand=0.0f,weightingFactorFullLayer=0.0f; //return value
float d=0.0f;
PSLAYER  pSL=pSo->pSLayer; // used to iterate over the original list
								
for(pSL=pSL->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext)   // exclude virtual layer at top and bottom
{
	d+= pSL->fThickness*0.1f;
	weightingFactorFullLayer=pSL->fThickness * 0.1f /depth;
	if (d <= depth)
	{
		sand += pSL->fSand * weightingFactorFullLayer; 
	}
		if (d > depth && ((d-pSL->fThickness*0.1f) < depth))
	{
		sand +=  pSL->fSand *  
			(pSL->fThickness * 0.1f-(d-depth))/depth ;
	} 
}
return sand;
}

float getWFPStoDepth(float depth, EXP_POINTER)
{
float WFPS=0.0f,weightingFactorFullLayer=0.0f;
float d=0.0f;
PSLAYER  pSL=pSo->pSLayer; // used to iterate over the original list
PWLAYER  pWA=pWa->pWLayer;							
for(pSL=pSL->pNext,pWA=pWA->pNext; pSL->pNext != NULL ;  pSL=pSL->pNext,pWA=pWA->pNext)   // exclude virtual layer at top and bottom
{
	weightingFactorFullLayer=pSL->fThickness * 0.1f /depth;

	d += pSL->fThickness*0.1f; //[cm]
	if (d <= depth)
	{
		WFPS += pWA->fContAct / pSo->pSLayer->fPorosity  * weightingFactorFullLayer; 
										 //	use direct input value: pSo->pSLayer->fPorosity. 
										 //Daycent: porosity=1-bulk density/2.65f
	}
		if (d > depth && ((d-pSL->fThickness*0.1f) < depth)) //Fractional Layer
	{
		WFPS +=  pWA->fContAct / pSo->pSLayer->fPorosity *  
			     (pSL->fThickness * 0.1f-(d-depth))/depth ; 
	} 
} 
// if (WFPS <0.0f || WFPS >1.0f) ExpertN Warnung ausgeben 
return WFPS; //0..1
}

// getH2OtoDepth 
// depth= -1 : return water content of all layers
float getH2OtoDepth (float depth, EXP_POINTER)
{
// Calculate H2O amount to root depth
PWLAYER  pWL=pWa->pWLayer;// used to iterate over the original list
PSLAYER  pSL=pSo->pSLayer;// used to iterate over the original list
float h2o=0.0f; //return value
float d=0.0f;
for( pWL = pWL->pNext, pSL=pSL->pNext;
	pWL->pNext != NULL && pSL->pNext != NULL ;  pWL = pWL->pNext, pSL=pSL->pNext)  // exclude virtual layer at top and bottom	
{
	if (depth <0.0)
	{
		h2o += pWL->fContAct * pSL->fThickness * 0.1f;

	}
	else //depth > 0
	{
		d+= pSL->fThickness*0.1f;
		if (d <= depth) //full layer
		{
			h2o += pWL->fContAct * pSL->fThickness * 0.1f; // H20 [cm]
		}
			if (d > depth && ((d-pSL->fThickness*0.1f) < depth)) //fractional layer
		{
			h2o += pWL->fContAct *  (pSL->fThickness * 0.1f-(d-depth))/depth ; // H20 [cm];
		} 
	}
}
return h2o; //[cm]
}


//	AboveGroundDecompRatio
// 	Calculates the aboveground decomposition ratio.
// 	Determine C/N of new material entering 'Box B'.
// 	
// 	The C/N ratios for structural and wood can be computed once;
// 	they then remain fixed throughout the run.  The ratios for
// 	metabolic and som1 may vary and must be recomputed each time step.
// 	Returns the C/N ratio of new N;  = function of the N content of the
// 	material being decomposed.
float AboveGroundDecompRatio (float totalCinA ,float totalNinA,float biofac,//amount of C and N in box A
																			//biomass factor: 2.0 for wood, 2.5 else
							 struct fixedparameter fixed)
	{
	float ratio;
	float cemicb;
	float const biomass = totalCinA * biofac;	// biomass factor=2.0 for wood
	float const econt =				// E/C ratio in box A
	 biomass <= 1.0e-10f ?	0.0f : totalNinA / biomass;
	//		TDecomposition.cpp line 173ff:
// cemicb = slope of the regression line for C/E of som1
//    where pcemicMax = maximum C/E of new som1
//      pcemicMin = minimum C/E of new som1
//      pcemic3 = minimum E content of decomposing
//                material that gives minimum C/E   //    pcemic is a fixed parameter
	cemicb =( fixed.pcemic[1] - fixed.pcemic[0]) / fixed.pcemic[2];	
	ratio  = ( econt > fixed.pcemic[2] ? fixed.pcemic[1] : fixed.pcemic[0] + econt * cemicb);
	return ratio;
	}

float FlowCintoSOM2(float Ctoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ctoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fCHumusSlow += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ctoadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fCHumusSlow += toadd * gpm2TOkgpha;
	}
}
	return Ctoadd;
}

float FlowNintoSOM2(float Ntoadd,			// amount of organic N to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ntoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fNHumusSlow += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ntoadd *  ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNHumusSlow += toadd * gpm2TOkgpha;
	}
}
	return Ntoadd;
}
float FlowCintoSOM1(float Ctoadd,			// amount of C to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ctoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fCHumusFast += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ctoadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fCHumusFast += toadd * gpm2TOkgpha;
	}
}
	return Ctoadd;
}

float FlowNintoSOM1(float Ntoadd,			// amount of organic N to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext) 
{
	d += pSL->fThickness *0.1f;
	if (d <= depth)
	{
		toadd = Ntoadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fNHumusFast += toadd * gpm2TOkgpha; //SOM2
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=Ntoadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fNHumusFast += toadd * gpm2TOkgpha;
	}
}
	return Ntoadd;
}

float CO2intoSOIL(float CO2toadd,			// amount of organic N to add (gN/m^2)
					float depth,				// soil depth to receive mineral E (cm)
					EXP_POINTER)														
{
PSLAYER  pSL=pSo->pSLayer->pNext; // XN Soil layer start with layer 1
PCLAYER  pCL=pCh->pCLayer->pNext;  
float d=0.0f,toadd=0.0f;
int i;

for(i=0 ; pSL->pNext != NULL ;i++, pSL=pSL->pNext,pCL=pCL->pNext ) 
{
	d += pSL->fThickness *0.1f; //cm
	if (d <= depth)
	{
		toadd = CO2toadd *  (pSL->fThickness*0.1f)/depth;
		pCL->fCO2C		+= toadd * gpm2TOkgpha; // [kg/ha]
		pCL->fCO2ProdR	+= toadd * gpm2TOkgpha; // [kg/ha]
	} 
	if (d > depth && ((d-pSL->fThickness*0.1f) < (depth-10e-7)))  //fractional layer
	{
		toadd=CO2toadd  * ((pSL->fThickness*0.1f)-(d-depth))/depth;
		pCL->fCO2C		+= toadd * gpm2TOkgpha;// [kg/ha]
		pCL->fCO2ProdR	+= toadd * gpm2TOkgpha; // [kg/ha]
	}
}
	return CO2toadd;
}

//Following funktions are for debugging purpose
//it returns the total mineral N in TMP Clayers [kg/ha]
float getN(PCLAYER  pCL)
{ 
float N=0;
for(pCL=pCL->pNext; pCL->pNext != NULL ;pCL=pCL->pNext) 
{
	N += pCL->fNO3N + pCL->fNH4N;
}
return N;
}

float getNH4(PCLAYER  pCL)
{ 
float N=0;
for(pCL=pCL->pNext; pCL->pNext != NULL ;pCL=pCL->pNext) 
{
	N +=  pCL->fNH4N;
}
return N;
}

float getNO3(PCLAYER  pCL)
{ 
float N=0;
for(pCL=pCL->pNext; pCL->pNext != NULL ;pCL=pCL->pNext) 
{
	N += pCL->fNO3N  ;
}
return N;
}
