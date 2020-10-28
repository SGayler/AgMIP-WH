//	Function: Denitrify
//
//	Description:

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

double  Denitrify (EXP_POINTER)
{
int i,j;
PSLAYER  pSL=pSo->pSLayer->pNext;	// used to iterate over the linked list
PCLAYER	 pCL=pCh->pCLayer->pNext;
PWLAYER  pWL=pWa->pWLayer->pNext;	 
PSWATER  pSW=pSo->pSWater->pNext;
static struct parameter par;
static float ppt2lastweeks[14];//the daily rain amount of the last 2 weeks [cm] is needed for
								//the calculation of the NOx pulse due to rain on dry soil
float sumppt;//rain amount of the last 2 weeks [cm]
static float NOxPulse[12]={1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};//NOx pulse factor of the next 12 days 
double d[1000]; //depth of the lower end of soil layers
double k[1000]; //distribution factor for soil layers
double co2PPM[1000]; //newCO2 [ppm]
double grams_soil[1000]; //grams of soil in a layer in a 1m x 1m section
float  nitratePPM[1000];//soil nitrate (NO3-) concentration (ppm)
double fluxNTotal[1000];// soil (N2+N2O) denitrif. flux by lyr ( gN/m^2 )
double fluxTotalDenitPPM; // total (N2+N2O) denitrif. flux (ppm N/day)
double gramsSoil;   		// soil mass in layer (g m-2)
double sum=0.0;		//normalization factor for soil exp. distribution
double fluxN2Odenit[1000];	// N2O flux from denitrification (gN/m2/day)
double fluxN2denit[1000];	// N2 flux from denitrification (gN/m2/day)
double fluxNOdenit[1000];	// NO flux from denitrification (gN/m2/day)
float  porosity; 
float  wfps_fc,dD0_fc;	// water filled pore space at field capacity (0-1)
double WFPS_threshold;//  water filled pore space threshold (0-1)
double WFPS;
double co2PPM_correction;
double a;
float  fDno3;		// Nitrate effect on denitrification
double fDco2;		//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
double fDwfps, M, x_inflection;		// wfps effect on denitrification (fDwfps, 0-1?)
double fRno3_co2, k1;//  Nitrate effect on the ratio of N2 to N2O
double fRwfps;		// WFPS effect on the N2/N2O Ratio
double ratioN2N2O;	// N2:N2O Ratio
double fluxN2O ;
double excess = 0.0;	// amount of N flux in excess of what
				//   can actually be removed from soil (gN/m^2)
double totalFlux;
double n2oFrac ; // N2O fraction of flux
double n2Frac  ; // N2 fraction of flux
double minNitrate_final;
double fluxOut;
double kPrecipNO; //multiplier NOx flux based on recent rain events and snowpack
double potentialFluxDenitrifNO;	//   maximum possible denitrification NO flux
								//   based on NO/N2O and kPrecipNO (gN/m^2/day)
double NO_N2O_ratio;//ratio NO:N2O
//constants
	float const ug_per_gram = 1.0E6f;		// micrograms per gram (ppm)
	float const grams_per_ug = 1.0E-6f;		// grams per microgram
	// min. nitrate concentration required in a lyr for trace gas calc. (ppm N)
	double const minNitratePPM = 0.1;
	// min. allowable nitrate per lyr at end of day (ppm N)
	double const minNitratePPM_final = 0.05;
	float const cm2m2 = 100000.0f;	// cm^2 per m^2
	float A[4] =	        // parameters to parton-innis functions
		{ 9.23f, 1.556f, 76.91f, 0.00222f };
    double const tolerance = 1.0E-30;


if (NewDay(pTi))
{
	//  multiplier NOx flux based on recent rain events and snowpack
    // increase of NO due to moisture and rain >= 1.0
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
		if(pCl->pWeather->fRainAmount*0.1f <= 0.5)//low rain event
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

	//*********************************************************************
	//exponential distribution in layers
	//*********************************************************************
	d[0]=0;  
	k[0]=(float)1.0 - (float)exp(-0.1*pSo->pSLayer->pNext->fThickness);
	sum = k[0];
	for(pSL=pSo->pSLayer->pNext,i=1; i<pSo->iLayers-2; i++, pSL= pSL->pNext)
	{	
		d[i]= d[i-1] + pSL->fThickness;
		k[i]= (float)exp(-0.1*d[i-1]) - (float)exp(-0.1*d[i]);
		sum += k[i];
	}
	//normalize
	for(pSL=pSo->pSLayer->pNext,i=1; i<=pSo->iLayers-2; i++, pSL= pSL->pNext)
	{	
		k[i] /= sum;
	}

	//*********************************************************************
	// Convert newCO2 (g/m2) to co2PPM[] (ppm) and distrbute it
    // through the soil profile  
	//*********************************************************************
	for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext,pCL=pCL->pNext)
	{
		gramsSoil = pSL->fBulkDens  * pSL->fThickness * 0.1   * 1.0e4f;//soil mass in layer (g m-2) //SB! check
		if(gramsSoil <= 0.0) Message(1, "Denitrify Error: Devison by 0");
		co2PPM[i] = k[i] * (pCL->fCO2C*kgphaTOgpm2 ) / gramsSoil * 1.0E6f;	
		if(co2PPM[i]<1e-30) co2PPM[i]=0.0;
	}

	//*********************************************************************
    // grams of soil in a layer in a 1m x 1m section
	//*********************************************************************
	for(pSL=pSo->pSLayer->pNext,i=0; i<pSo->iLayers-2; i++, pSL= pSL->pNext)
	{
		grams_soil[i] = pSL->fBulkDens  * pSL->fThickness * 0.1 * cm2m2;
	}

	//*********************************************************************
	// soil nitrate (NO3-) concentration (ppm)
	//*********************************************************************
	for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2; i++,pCL=pCL->pNext)
	{
		nitratePPM[i] = pCL->fNO3N * kgphaTOgpm2 / (float)grams_soil[i] * ug_per_gram;
	}

	//********************************************************************* 
	// denitrification loop
    //  Dentrification occurs over all layers
	//*********************************************************************
    //  Convert nitrate (gN/m2) to nitratePPM (ppm N)
	for(pCL=pCh->pCLayer->pNext,pSL=pSo->pSLayer->pNext,pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater->pNext,i=0; 
		i<pSo->iLayers-2; 
		i++, pSL= pSL->pNext,pCL=pCL->pNext,pWL=pWL->pNext,pSW=pSW->pNext)
    {
		if (nitratePPM[i] < minNitratePPM) continue;

		// normalized diffusivity in aggregate soil
		//   media, at a standard field capacity (0-1)
		//dD0_fc = diffusiv(&stdfieldc, &stdbulkd, &wfps_fc);
		//dD0 calc changed 6/20/00 -mdh
		// water filled pore space at field capacity (0-1)
		porosity = 1.0f - pSL->fBulkDens / (float)2.65;
		wfps_fc = pSW->fContFK / porosity;
		wfps_fc = max ( 0.0f, wfps_fc );
		wfps_fc = min ( wfps_fc, 1.0f );
		dD0_fc = Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,wfps_fc);
		if(dD0_fc < 0 || dD0_fc > 1.0 ) Message(1,"Warning - Denitrification: dD0_fc out of range");

		// water filled pore space threshold (0-1)	
		WFPS_threshold =
			(dD0_fc >= 0.15f) ? 0.80f : (dD0_fc * 250.0f + 43.0f) / 100.0f;
		if(WFPS_threshold < 0 || WFPS_threshold > 1.0 ) 
			Message(1,"Warning - Denitrification: WFPS_threshold out of range");
		WFPS = pWL->fContAct / pSL->fPorosity;

		// CO2 correction factor when WFPS has reached threshold
		if (WFPS <= WFPS_threshold)
		{
			co2PPM_correction = co2PPM[i];
		}
		else
		{
			a = (dD0_fc >= 0.15f) ? 0.004f : (-0.1f * dD0_fc + 0.019f);
			co2PPM_correction = co2PPM[i] * (1.0f + a * (WFPS - WFPS_threshold) * 100.0f);
		}
		if(co2PPM_correction < 0.0f) 
		{
			Message(1,"Warning - Denit.: co2PPM_correction < 0.0");
		}

		// Nitrate effect on denitrification
		// denitrification flux due to soil nitrate (ppm N/day)
		// Changed NO3 effect on denitrification based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		// fDno3 = 1.15 * std::pow(nitratePPM(layer), 0.57);
		fDno3 = max( 0.0f, f_arctangent(nitratePPM[i], A) );

		//  Carbon Dioxide effect on denitrification (fDco2, ppm N)
		//  Changed CO2 effect on denitrification based on
		//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		fDco2 = max(0.0,(0.1 * pow(co2PPM_correction, 1.3) - minNitratePPM) );

		// wfps effect on denitrification (fDwfps, 0-1?)
		// Changed wfps effect on denitrification based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		M =  min(0.113f, dD0_fc) * (-1.25) + 0.145;
		x_inflection = 9.0 - M * co2PPM_correction;
		fDwfps = max ( 0.0, 0.45 +
			(atan(0.6 *  3.14159265358979  * (10.0 * WFPS - x_inflection))) / 3.14159265358979  );

		//  N fluxes (N2 + N2O) for the current layer, ppm N
		fluxTotalDenitPPM = // total (N2+N2O) denitrif. flux (ppm N/day)
			(fDno3 < fDco2) ? fDno3 : fDco2;
		if(fluxTotalDenitPPM < 0.0)Message(1,"Warning - Denit.: fluxTotalDenitPPM < 0.0");
		// Minimum value for potential denitrification in simulation layers.
	    fluxTotalDenitPPM = max(0.066, fluxTotalDenitPPM);
		fluxTotalDenitPPM *= fDwfps;

		//  Nitrate effect on the ratio of N2 to N2O
		//  Maximum N2/N2O ratio soil respiration function
		//  Changed the NO3 and CO2 effect on the N2/N2O ratio based on
		//  paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		k1 = max (1.5, 38.4 - 350 * dD0_fc);
			if(k1 < 0.0) Message(1,"Warning - Denit.: k1 < 0.0f");
		fRno3_co2 = max (0.16 * k1,
				k1 * exp(-0.8 * nitratePPM[i] / co2PPM[i]) );
			if(fRno3_co2 < 0.0) Message(1,"Warning - Denit.: fRno3_co2 < 0.0");

		// WFPS effect on the N2/N2O Ratio
		// Changed wfps effect on the N2/N2O ratio based on
		// paper Del Grosso et. al, GBC, in press.  -mdh 5/16/00
		fRwfps = max (0.1f, 0.015f * WFPS * 100.0f - 0.32f);

		// N2:N2O Ratio
		// ratioN2N2O = (fRno3 < fRco2) ? fRno3 : fRco2;
		// ratioN2N2O *= fRwfps;
		ratioN2N2O = max(0.1, fRno3_co2 * fRwfps);

		// Compute N2 and N2O flux by layer (fluxNTotal(layer))
		// convert ppm N to gN/m^2
		fluxNTotal[i] = fluxTotalDenitPPM * grams_soil[i] * grams_per_ug;
			if(fluxNTotal[i] > 10.0)Message(1,"Warning - Denit.: fluxNTotal > 10.0");
		fluxN2O = fluxNTotal[i] / (ratioN2N2O + 1.0);
		fluxN2Odenit[i] = fluxN2O;
		fluxN2denit[i]	= max(0.0, fluxNTotal[i] - fluxN2O);
			if(fluxN2Odenit[i] < 0.0)Message(1,"Warning - Denit.: fluxN2Odenit < 0.0");
			if(fluxN2denit[i] < 0.0)Message(1,"Warning - Denit.: fluxN2denit < 0.0");
    } // denitrification loop

	//********************************************************************* 
    //  Reduce nitrate in soil by the amount of N2-N N2O-N that is lost
    //  Do not let nitrate in any layer go below minNitratePPM_final
	//********************************************************************* 
	for(pCL=pCh->pCLayer->pNext,i=0; i<pSo->iLayers-2;pCL=pCL->pNext, i++)
    {
	totalFlux = fluxN2Odenit[i] + fluxN2denit[i];
	if (totalFlux > 1.0E-30)	// have any flux?
	{
		n2oFrac = fluxN2Odenit[i] / totalFlux; // N2O fraction of flux
		n2Frac  = fluxN2denit[i]  / totalFlux; // N2 fraction of flux

	    minNitrate_final = minNitratePPM_final * grams_soil[i] * grams_per_ug;
	    if ( nitratePPM[i] < minNitratePPM )	// No flux from layer?
	    {
			excess += fluxNTotal[i];
	    }
	    else if (pCL->fNO3N*kgphaTOgpm2 - fluxNTotal[i] > minNitrate_final )
	    {
		// remove N in calculated trace gas flux from the layer
			pCL->fNO3N -= (float)fluxNTotal[i] * gpm2TOkgpha;
	    }
	    else
	    {
			// reduce trace gas flux in layer so soil N won't fall below min
			// actual total trace gas flux (gN/m^2)
			fluxOut =
				(nitratePPM[i] - minNitratePPM_final) *
				grams_soil[i] * grams_per_ug;
			excess += fluxNTotal[i] - fluxOut;
			pCL->fNO3N = (float)minNitrate_final * gpm2TOkgpha;
	    }
		fluxN2Odenit[i] -= n2oFrac * excess;
		fluxN2denit[i]  -= n2Frac * excess;
		if(fluxN2Odenit[i] < -1.0e-8)Message(1,"Warning - Denit.: fluxN2Odenit < -1.0e-8");
		if(fluxN2denit[i] < -1.0e-8)Message(1,"Warning - Denit.: fluxN2denit < -1.0e-8");
	}
    else //no Flux: totalFlux < 1.0E-30
    {
		fluxN2Odenit[i] = 0.0;
		fluxN2denit[i] = 0.0;
    }

//********************************************************************* 
// --- Partition denitratrification fluxes ---
//********************************************************************* 
	// For denitrification, kPrecipNO is >= 1.0 -mdh 6/22/00
	// potentialFluxDenitrifNO =
	//   maximum possible denitrification NO flux
	//   based on NO/N2O and kPrecipNO (gN/m^2/day)

	//*********************************************************************
	// ratio NO:N2O and NO flux
	//*********************************************************************
	WFPS = pWL->fContAct / pSL->fPorosity;
	NO_N2O_ratio = 15.23 + ( 35.45 *
		atan ( 0.676f * 3.141592f * 
		(10.0f * Diffusivity(pSW->fContFK,pSL->fBulkDens,pSL->fPorosity,(float)WFPS) - 1.86f) )
		) / 3.141592f;
		if(NO_N2O_ratio < 0.0)Message(1,"Warning - Denitrification: NO_N2O_ratio < 0"); 
	if (iForest == 0)// true if agricultural system or grassland  				 
	{	
		NO_N2O_ratio *= 0.20;
	}
    potentialFluxDenitrifNO = fluxN2Odenit[i] * NO_N2O_ratio *  min (1.0, kPrecipNO);
	if(potentialFluxDenitrifNO < 0.0) Message(1,"Warning - Denit.: potentialFluxDenitrifNO < 0");

	if (potentialFluxDenitrifNO <= pCL->fNH4N * kgphaTOgpm2)//  Take all N out of NH4
	{
		fluxNOdenit[i] = potentialFluxDenitrifNO;
		pCL->fNH4N -= (float)potentialFluxDenitrifNO * gpm2TOkgpha;
	}
    else// NH4 limits; some N from N2O
    {
		// Convert all ammonium to NO
		fluxNOdenit[i] = pCL->fNH4N * kgphaTOgpm2;
		potentialFluxDenitrifNO -= pCL->fNH4N * kgphaTOgpm2;
		pCL->fNH4N = 0.0;
		// convert some N2O to NO
		if (potentialFluxDenitrifNO <= fluxN2Odenit[i])
		{
			fluxNOdenit[i] += potentialFluxDenitrifNO;
			fluxN2Odenit[i] -= potentialFluxDenitrifNO;
		}
    }// NH4 limits; some N from N2O

	 // Check for very small values
    if(fluxNOdenit[i] < 0.0)Message(1,"Warning - Denit.: fluxNOdenit < 0");;
    if (fluxNOdenit[i] < tolerance)
	fluxNOdenit[i] = 0.0;
    if(fluxN2Odenit[i] < 0.0f)Message(1,"Warning - Denit.: fluxN2Odenit < 0");;
    if (fluxN2Odenit[i] < tolerance)
	fluxN2Odenit[i] = 0.0f;
    if(fluxN2denit[i] < 0.0f)Message(1,"Warning - Denit.: fluxN2denit < 0");;
    if (fluxN2denit[i] < tolerance)
	fluxN2denit[i] = 0.0f;

//*********************************************************************
// Write fluxes to XN variables
//	fluxN2denit[i]	-	[g/m2/day]
//	fluxN2Odenit[i]	-	[g/m2/day]
//	fluxNOdenit[i]	-	[g/m2/day]
//*********************************************************************
pCL->fNO3DenitR = (float)(fluxN2Odenit[i] + fluxNOdenit[i] +fluxN2denit[i]) 
							* gpm2TOkgpha; //[kg/ha/day]
						//* (float)1e6 / (float)24.0 /(float)pTi->pTimeStep->fAct;//[ug/m2*h] per timestep

pCh->pCProfile->fN2OEmisR += (float)(fluxN2Odenit[i] + fluxNOdenit[i] +fluxN2denit[i])
					* (float)1e6 / (float)24.0 /(float)pTi->pTimeStep->fAct;    //[ug/m2*h] per timestep
		//XN: NxOx Emission has to be balanced somehow for the graphical output.
		//XN: just do it the same way as for N2O
		//till it is fixed, write the NOx emission to N2O emission
 //		pCh->pCProfile->fNOEmisR  += (float)(fluxNOnit) * 1e5 / 24.0 /(float)pTi->pTimeStep->fAct;  //[ug/m2*h] per timestep

//also fN2EmisR is not summed up!

}  // for layer
}//new Day
return 0;
}