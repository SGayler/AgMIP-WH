//	Function: Nitrify
//
//	Description:
//	Calculates the amount of ammonium that is converted 
//	to nitrate , N2 and NO 
//	during the nitrification process.

//	steps:
//	I)		Calculate total NH4 loss
//  II)		partition to Nitrate, N2, NO 
//	III)	partition Nitrate and Ammonium to soil layers
//
//  Modell: Parton et al.: Model for NOx and N2O Emissions, Journal of Geophysical
//			Research, vol. 106, (2001)
//
//  See also DayCent reference 'NOx Submodel'
//
#include  <xinclexp.h>
#include "Century.h"
extern	int		WINAPI Message(long, LPSTR);
extern int SimStart(PTIME);			// from time1.c
extern int NewDay(PTIME);


double  Nitrify (EXP_POINTER)
{
	PSLAYER  pSL=pSo->pSLayer->pNext;	// used to iterate over the linked list
	PWLAYER  pWL=pWa->pWLayer->pNext;	// used to iterate over the linked list
	PCLAYER  pCL=pCh->pCLayer->pNext;	// used to iterate over the linked list
	PHLAYER  pHL=pHe->pHLayer->pNext;	// used to iterate over the linked list
	PSWATER  pSW=pSo->pSWater->pNext;
	enum  TSoilTextureIndex textureindex=UNDEFINED;
	static float ppt2lastweeks[14];//the daily rain amount of the last 2 weeks [cm] is needed for
								//the calculation of the NOx pulse due to rain on dry soil
	float sumppt;//rain amount of the last 2 weeks [cm]
	static float NOxPulse[12]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};//NOx pulse factor of the next 12 days 
								//index 0 is tomorrow
	static struct parameter par;
	double NH4_to_NO3 = 0.0,ret=0.0;			// amount of NH4 converted to NO3 [kg/ha]
	double newNO3 = 0.0;			// new NO3 left after Trace gas production(gN/m^2/day)
    float const MaxNitrifRate = 0.10f;	// max fraction of ammonium that goes to NO3 during nitrification
    float const Ha_to_SqM = 0.0001f;	// factor to convert ha to sq meters
	float const min_ammonium = 0.1f;	// min. total ammonium in soil layer (kgN/ha)
    //float const min_ammonium = 0.15f;	// min. total ammonium in soil (gN/m^2)
	int i,j;
	float a, b, c, d, base1, base2, e1,e2;
//	float avgWFPS = getWFPStoDepth(20.0f, exp_p); //water filled pore space to 20cm depth
	float  WFPS;
	double fNwfps, fNsoilt, fNph;
	double base_flux, 
			diffusivity;				//	normalized diffusivity in aggregate soil media, units 0-1:
										//	ratio of gas diffusivity through soil to gas diffusivity through air
										//	at optimum water content.
	float  A[4];						// parameters to parton-innis functions
	//  fraction  N2O to NO of new NO3 that is converted 
	double kPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
    double NO_N2O_ratio = 0.0;  		// NO/N2O ratio <= 1.0
	double fluxN2Onit = 0.0;  			// nitrification N2O flux (kgN/ha/day)
	double fluxNOnit = 0.0;				// flux nitrified NO (kgN/ha/day)
	double const fractionN03toN2O = 0.02;  // frac of new NO3 that goes to N2O		
	double potentialFluxNOnit;			// maximum possible nitrification NO flux (gN/m^2/day)
	double NH4_to_NO = 0.0; 			// ammonium converted to NO to reach
										//    potential NO flux (kgN/ha/day)
	double NNM;

if(SimulationStarted)
{
	//read site parameter from 'century.cfg' file
	par = ReadParameter();
}

if (NewDay(pTi)) //Trace gas modell is called daily
{
//***********************************************************************************
// 0) Initialize
//***********************************************************************************
pCh->pCProfile->fN2OEmisR = 0.0;
pCh->pCProfile->fNOEmisR = 0.0;

//***********************************************************************************	 
//  multiplier NOx flux based on recent rain events and snowpack
// increase of NO due to moisture and rain >= 1.0
//***********************************************************************************
//shift the daily values and add current day's ppt [cm]
for(j=0;j<13;j++)//shift the daily values and add current day's ppt [cm]
{
	ppt2lastweeks[j]=ppt2lastweeks[j+1];
}
ppt2lastweeks[13] = pCl->pWeather->fRainAmount*0.1f;
//sum of rain during last 2 weeks:
sumppt=0.0;
for(j=0;j<14;j++)//shift the daily values and add current day's ppt [cm]
{
	sumppt += ppt2lastweeks[j]; 
}
//get pulse factor for today
if(pTi->pSimTime->iJulianDay > 14)
{
kPrecipNO = NOxPulse[0];

for(j=0;j<11;j++)//shift the daily values and add factor 1.0 for the last day
{
	NOxPulse[j]=NOxPulse[j+1];
}
NOxPulse[11]=1.0;

//calculate future pulses
//new pulse, if sum is lower than 1cm and no snow
if (sumppt <= 1.0 && (pCl->pWeather->fSnow==0.0) )
{
	if(pCl->pWeather->fRainAmount*0.1f <= 0.5//low rain event
		&& pCl->pWeather->fRainAmount*0.1f > 0.1)
	{
		for(j=0;j<1;j++)//low rain event pulses last 1 day
		{
			NOxPulse[j] = max(NOxPulse[j], 11.19f *  (float)exp(-0.805f * (j+2)) );
		}
	}
		if(pCl->pWeather->fRainAmount*0.1f > 0.5//medium rain event
		&& pCl->pWeather->fRainAmount*0.1f <= 1.5)
	{
		for(j=0;j<5;j++)//medium rain event pulses last 5 days
		{
			NOxPulse[j] = max(NOxPulse[j], 14.68f * (float)exp(-0.384f * (j+2)) );
		}
	}
	if(pCl->pWeather->fRainAmount*0.1f > 1.5)//high rain event
	{
		for(j=0;j<12;j++)//large rain event pulses last 12 days
		{
			NOxPulse[j] = max(NOxPulse[j], 18.46f * (float)exp(-0.208f * (j+1)) );
		}
	}
}
}//simulated days >14
else//at the first 14 days
{
	kPrecipNO = 1.0;
}

//***********************************************************************************
// I) total NH4 loss
//***********************************************************************************

//Loop over all soil layer
for(i=0,pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,pWL=pWa->pWLayer->pNext,pHL=pHe->pHLayer->pNext,pSW=pSo->pSWater->pNext; 
	pSL->pNext!=NULL; pCL=pCL->pNext,pSL=pSL->pNext,pWL=pWL->pNext,pHL=pHL->pNext,pSW=pSW->pNext,i++)
{
	NH4_to_NO3 = 0.0;
	// Determine texture index (COARSE,MEDIUM, FINE, VERY FINE); 
	if (pSL->fSand > 70.0 ) textureindex=COARSE;
	if (pSL->fSand <= 70.0 && pSL->fSand >= 30.0  ) textureindex=MEDIUM;
	if (pSL->fSand < 30.0 ) textureindex=FINE; //Daycent: "currently VERYFINE cannot be set from knowing sand content"

	if (pCL->fNH4N >= min_ammonium)
		{
		//  Compute the effect of WFPS on Nitrification (0-1)
		WFPS = pWL->fContAct / pSL->fPorosity;
        switch (textureindex)
        {
            case COARSE:	a = 0.5f;	b = 0.0f;	c = 1.5f;	d = 4.5f;	break;
            case FINE:  
            case VERYFINE:	a = 0.65f;	b = 0.0f;	c = 1.2f;	d = 2.5f;	break;
            case MEDIUM:
            default:		a = 0.65f;	b = 0.0f;	c = 1.2f;	d = 2.5f;	break;
        }
		//
        base1 =((WFPS - b) / (a - b));  
        base2 =((WFPS - c) / (a - c)); 
        e1 = d * ((b - a) / (a - c));
        e2 = d;
        fNwfps = pow (base1, e1) * pow (base2, e2);

		//  Soil temperature effect on Nitrification (0-1)		
        A[0] = par.maxt;	// Long term avg max monthly air temp. of hottest month
        A[1] = -5.0; A[2] = 4.5; A[3] = 7.0;
        fNsoilt = f_gen_poisson_density (pHL->fSoilTemp, A);

		//  Compute pH effect on nitrification
        A[0] = 5.0f;
        A[1] = 0.56f;
        A[2] = 1.0f;
        A[3] = 0.45f;		 
        fNph = f_arctangent (pSL->fpH,A); //pH Wert 
       
		// Ammonium that goes to nitrate during nitrification.
		// base flux in Daycent program code:
        //base_flux = 0.1 * Ha_to_SqM;	// 0.1 gN/ha/day 
		//base flux from Parton et al. (2001): 20% of NNM-Ammonium
		NNM= (pCL->fMinerR - pCL->fNImmobR);
		base_flux=  max(0.0, NNM * 0.2f);
		NH4_to_NO3 = (pCL->fNH4N - NNM) * MaxNitrifRate * fNph * fNwfps * fNsoilt + base_flux;	//[kg/ha]
		NH4_to_NO3 = max( 0.0f, NH4_to_NO3 );  
		if(pCL->fNH4N < (float)NH4_to_NO3)
		{
			Message(1,"Nitrification error: more nitrified than available");
		}
		ret +=  NH4_to_NO3;	
	}//if (ammonium >= min_ammonium)
 
//***********************************************************************************
    newNO3 = NH4_to_NO3; //[kg/ha]
	if (newNO3 > 1.0E-10)
    {   

//***********************************************************************************
// II) Partition to Nitrate, N2, NO 
//***********************************************************************************
	//Diffusivity
	//	Returns the normalized diffusivity in aggregate soil media, units 0-1:
	//	ratio of gas diffusivity through soil to gas diffusivity through air
	//	at optimum water content.		

		diffusivity=Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,WFPS);
							//  [1]        , [kg/dm3=g/cm3]´, [1],        ,[1]
	// ratio NO:N2O
		NO_N2O_ratio = 15.23 + ( 35.45 *
				atan ( 0.676 * PI * (10.0f * diffusivity - 1.86f) ) ) / PI; 
	// N2O flux
		fluxN2Onit = newNO3 * fractionN03toN2O;
		newNO3 -= fluxN2Onit;
	// maximum possible nitrification NO flux (kg/ha/day)
		potentialFluxNOnit = fluxN2Onit * NO_N2O_ratio * kPrecipNO;
	 
	if (potentialFluxNOnit <= newNO3) //NO is taken from newNO3
	{
	    fluxNOnit = potentialFluxNOnit;
	    newNO3 -= fluxNOnit;
	}
	else
	{
	    // take N out of ammonimum to get max fluxNOtotal possible
	    NH4_to_NO =  potentialFluxNOnit - newNO3 ;	 
		NH4_to_NO = min(NH4_to_NO,pCL->fNH4N); //restrain  flow
		//pCL->fNH4N -= (float)NH4_to_NO; //perform flow below
		ret += NH4_to_NO;
		fluxNOnit = newNO3 + NH4_to_NO;
	    newNO3 = 0.0;
	 }
//*********************************************************************
// Write fluxes to XN variables
//		fluxNOnit and fluxN2Onit  -  [kgN/ha/day]
//*********************************************************************	
	// 1. Subtract Ammonium: 
		pCL->fNH4N -= (float)NH4_to_NO3;
		pCL->fNH4N -= (float)NH4_to_NO;
	// 2. Add NO3
		pCL->fNO3N += (float)newNO3;
		pCL->fNH4NitrR = (float)newNO3 ;//[kg/ha/day]
				//* (float)1e5 / (float)24.0 ;//[ug/m2*h] ??
	// 3. Gaseous Emissions  
		//this does work, but use is without setting fN2OEmisR in Profile
		//pCL->fN2ON  += fluxN2Onit;  //[kg/ha]
		//pCL->fNxOyN += fluxNOnit;  //[kg/ha]
 
		//XN: NxOx Emission has to be balanced somehow for the graphical output.
		//XN: just do it the same way as for N2O
		//till it is fixed, write the NOx emission to N2O emission
 //		pCh->pCProfile->fNOEmisR  += (float)(fluxNOnit) * 1e5 / 24.0 /(float)pTi->pTimeStep->fAct;  //[ug/m2*h] per timestep
		pCh->pCProfile->fN2OEmisR += (float)(fluxN2Onit + fluxNOnit)* (float)1e5 / (float)24.0 /(float)pTi->pTimeStep->fAct;    //[ug/m2*h] per timestep
	

	}//if (newNO3 > 1.0E-10)

	if (pCL->fNO3N < 0 || pCL->fNH4N < 0)
	{
		Message(1,"Error Nitrify: neg. Mineral N");
	}
}//loop over layer
}//daily

return ret;
}
//************ end of function Nitrify****************

float Diffusivity (
	float const A,		// fraction of soil bed volume occupied by
				//   field capacity
				//   (intra-aggregate pore space, 0-1)
	float const bulkDen,	// bulk density of soil (g/cm^3)
	float const porosity,	// porosity fraction (0-1)
	float const wfps)	// Water Filled Pore Space fraction
				// (volumetric swc/porosity, 0-1)
{
	float  pfc,vfrac,theta_V,theta_P,theta_A,s_wat,sw_p; //explained below
	float tp1, tp2, tp3, tp4, tp5, tp6, tp7, tp8;  // intermediate variables
    // normalized diffusivity in aggregate soil media (0-1)
    float dDO = 0.0f; //return value
	 // volumetric air content fraction
    float const vac = min (1.0f, max (0.0f, porosity - A) );
    if ( vac > 0.0f )	// undersaturated?
    {
	// soil water content as % of field capacity (%)
	// (this value can be > 100%)
	pfc = wfps * 100.0f  / (A / porosity);

	// volumetric fraction
	vfrac = ( pfc >= 100.0f ? (wfps * porosity - A) / vac : 0.0f );

	// volumetric water content of the soil bed volume
	// float my_theta_V = wfps * (1.0 - bulkden/PARTDENS);
	// float my_theta_V = wfps * porosity;
	theta_V = ( pfc < 100.0f ? (pfc / 100.0f) * A : A + min (vfrac, 1.0f) * vac );

	// volume H2O per unit bed volume in inter-aggregate pore space
	theta_P = (theta_V < A) ? 0.0f : theta_V - A;

	// volume H2O per unit bed volume in intra-aggregate pore space
	theta_A = (theta_V > A) ? A : theta_V;

	// fractional liquid saturation of the A component of total pore volume
	s_wat = min ( 1.0f, theta_V / A );

	// fractional liquid saturation of the P component of total pore volume
	sw_p =  min ( 1.0f, theta_P / vac );

	if (1.0f - s_wat > 0.0f)
		tp1 = (float)pow( (1.0f - s_wat), 2.0f );
	else
		tp1 = 0.0f;
	tp2 = (A - theta_A) / (A + (1.0f - porosity));
	if ( tp2 > 0.0f )
		tp3 = (float)pow( tp2, (0.5f * tp2 + 1.16f) );
	else
		tp3 = 0.0f;
	tp4 = 1.0f - (float)pow( vac, (0.5f * vac + 1.16f) );
	tp5 = vac - theta_P;
	if (tp5 > 0.0f)
		tp6 = (float)pow(tp5, (0.5f * tp5 + 1.16f));
	else
		tp6 = 0.0f;
	tp7 = (float)pow((1.0f - sw_p), 2.0f);
	tp8 = max( 0.0f,
		( (tp1 * tp3 * tp4 * (tp5 - tp6)) /
		(1.0E-6f + (tp1 * tp3 * tp4) + tp5 - tp6) * 1.0E7f) );

	// normalized diffusivity in aggregate soil media (0-1)
	dDO = max(0.0f, (tp8 / 1.0E7f + tp7 * tp6) );
    }
	
	if(dDO < 0 || dDO > 1.0 ) Message(1,"Warning - diffusivity out of range");
    
	return dDO;
}

 


float f_arctangent(float x, float A[])
    {
      return(A[1] + (A[2] / (float)PI) *
             (float)atan(PI * (double)A[3] * (double)(x - A[0])));
    }

float f_gen_poisson_density(float x, float A[])
    {
      double tmp1, tmp2, tmp3;

      if (A[1] == A[0]) {
        return(0.0f);
      }

      tmp1 = (double)((A[1] - x) / (A[1] - A[0]));
      if (tmp1 <= 0.0) {
        return 0.0f;
      }

      tmp2 = 1.0 - pow(tmp1, (double)A[3]);
      tmp3 = pow(tmp1, (double)A[2]);
      return (float)(exp((double)A[2] * tmp2 / (double)A[3]) * tmp3);
    }


