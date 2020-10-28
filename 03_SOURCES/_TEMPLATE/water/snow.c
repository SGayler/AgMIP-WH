/*********************************************************************************************/
/* Modul         : SNOW.c                                                                    */      
/* Inhalt        : Schneemodelle                                                             */
/*                                                                                           */
/* Author        : cs                                                                        */
/*                                                                                           */
/* Date          : 11.02.2000 Anfang                                                         */
/*                 2 Modellansätz (SChneeschmelze mit und ohne Windeinfluß                   */
/*                                                                                           */
/*														                                     */
/*********************************************************************************************/

 /* Sammlung aller Header-Dateien: */
#include "xinclexp.h"   
#include "xtempdef.h"
#include "xh2o_def.h"

#undef LOG_FREEZING       

/* extern functions */

/* from time1.c */
extern int NewDay(PTIME);
extern int SimStart(PTIME);      

/* from util_fct.c */
extern float   abspower(float x, float y);

/* function declarations: */
int WINAPI Snow(EXP_POINTER);
int WINAPI Snow2(EXP_POINTER);
int WINAPI Snow3(EXP_POINTER);

/* Variablen zur Übergabe an die Wasserroutinen*/
float fSnowInput,fSnowOutflow,fSnowStorage,fSnowLiquid,fSnowDensity,fSnowAge,fSnowHeight;
float fSnowOutflowDay,fSnowStorageDay,fSnowLiquidDay,fSnowDensity,fSnowHeight;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : Snow()                                     */
/*                                                        */
/*  Function : Schneemodell, einfache Schneeakkumulation, */
/*             oberhalb einer Grenztemperatur,            */
/*			   SChneeschmelze Temperaturindexverfahren    */ 
/*                                                        */
/*  Author   : cs 10.02.00                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int WINAPI Snow(EXP_POINTER)
{
 DECLARE_COMMON_VAR
//  DECLARE_H2O_POINTER  

static float TempGrenzI = (float)0.5; // Grenztemperatur REgen-Schnee (bei Niederschlag) °C
static float TempGrenzO = (float)0.;  // Grenztemperatur Schneeschmelze °C
static float TempTrans  = (float)1.;  // Übergangsbereich SChnee REgen  °c
static float MeltRate   = (float)1.8;  // Schmelzrate	mm/(°C day)


if (NewDay(pTi))
 {
 //fSnowInput = (float)0.;  // für neuen Tag Schneemodell zurücksetzen
 //fSnowOutflow = (float)0.;
 
   // input in den Schneespeicher
   fSnowInput = min((pCl->pWeather->fRainAmount * ( TempGrenzI + TempTrans/(float)2. - 
	              pCl->pWeather->fTempAve ) / ( (float)2. *TempTrans )),pCl->pWeather->fRainAmount);

   if(fSnowInput<(float)0.)
	{
 	fSnowInput=(float)0.;
    }

   fSnowStorage += fSnowInput; 

   fSnowOutflow = max((float)0.,min((MeltRate *
	              (pCl->pWeather->fTempAve-TempGrenzO)),fSnowStorage));
   fSnowStorage -= fSnowOutflow;

   pWa->pWBalance->fReservoir=pWa->pWBalance->fReservoir-fSnowInput+fSnowOutflow;
   
   pCl->pWeather->fSnow = (float)0;
 //  if(fSnowStorage>EPSILON) pCl->pWeather->fSnow = (float)1;
 }

 return 1;
}  /*  end  */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : Snow2()                                    */
/*                                                        */
/*  Function : Schneemodell, einfache Schneeakkumulation, */
/*             oberhalb einer Grenztemperatur,            */
/*			   SChneeschmelze                             */ 
/*             Temperatur-Wind-indexverfahren             */
/*  Author   : cs 11.02.00                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int WINAPI Snow2(EXP_POINTER)
{
 DECLARE_COMMON_VAR
//  DECLARE_H2O_POINTER  
static float TempGrenzI   = (float)0.5; // Grenztemperatur REgen-Schnee (bei Niederschlag) °C
static float TempGrenzO   = (float)0.;  // Grenztemperatur Schneeschmelze °C
static float TempTrans    = (float)1.;  // Übergangsbereich SChnee REgen  °c
static float TempMeltRate = (float)1.;  // Schmelzrate	mm/(°C day)
static float WindMeltRate = (float)1.;  // SChmelzrate mm/(°C m/s day)
float  fSnowDensityMin		= (float)100; //minimale Schneedichte kg/m³
float  fSnowDensityMax		= (float)500; //maximale Schneedichte kg/m³


if (NewDay(pTi))
 {
 //fSnowInput = (float)0.;  // für neuen Tag Schneemodell zurücksetzen
 //fSnowOutflow = (float)0.;
 
   // input in den Schneespeicher
   fSnowInput = min((pCl->pWeather->fRainAmount * ( TempGrenzI + TempTrans/(float)2. - 
	              pCl->pWeather->fTempAve ) / ( (float)2. *TempTrans )),pCl->pWeather->fRainAmount);

   if(fSnowInput<(float)0.)
	{
 	fSnowInput=(float)0.;
    }
   //#############################################################
   
   if(fSnowStorage>(float)0 ||  fSnowInput>(float)0)
   {
		fSnowDensity  = (fSnowStorage * fSnowDensity + fSnowInput * fSnowDensityMin) /
				   (fSnowStorage + fSnowInput) ;


		if(-(float)10 < pCl->pWeather->fTempAve) 
		fSnowDensity = fSnowDensity + ((float)10 + pCl->pWeather->fTempAve) * (float)20;
  
		if(fSnowDensity>fSnowDensityMax) fSnowDensity=fSnowDensityMax;
		if(fSnowDensity<fSnowDensityMin) fSnowDensity=fSnowDensityMin;


		fSnowStorage += fSnowInput; 

		fSnowOutflow = max((float)0.,min((TempMeltRate+WindMeltRate*pCl->pWeather->fWindSpeed)
	              * (pCl->pWeather->fTempAve-TempGrenzO),fSnowStorage));
		fSnowStorage -= fSnowOutflow;
   
		pWa->pWBalance->fReservoir=pWa->pWBalance->fReservoir-fSnowInput+fSnowOutflow;

		fSnowHeight = (float)1000 * fSnowStorage / fSnowDensity; //[mm]
    }
	else
    {
		fSnowHeight		= (float)0;
		fSnowOutflow	= (float)0;
		fSnowStorage	= (float)0;
		fSnowDensity	= (float)0;
    }
 }

 return 1;
}  /*  end  */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : Snow3()                                    */
/*                                                        */
/*  Function : Schneemodell, einfache Schneeakkumulation, */
/*             oberhalb einer Grenztemperatur,            */
/*			   SChneeschmelze                             */ 
/*             kombiniertes Verfahren nach Anderson (1973)*/
/*             übernommen aus SChulla (1998)
/*  Author   : cs 31.03.00                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
 int WINAPI Snow3(EXP_POINTER)
{
 DECLARE_COMMON_VAR

//float DeltaT      = pTi->pTimeStep->fAct;
float	DayTime     = pTi->pSimTime->fTimeDay;
static float TempGrenzI   = (float)0.5; // Grenztemperatur REgen-Schnee (bei Niederschlag) °C
static float TempGrenzO   = (float)0.;  // Grenztemperatur Schneeschmelze °C
static float TempTrans    = (float)1.;  // Übergangsbereich SChnee REgen  °c
static float TempMeltRate = (float)1.;  // Schmelzrate	mm/(°C day)
static float WindMeltRate = (float)0.8;  // SChmelzrate mm/(°C m/s day)
static float SnowLiquidStorage = (float)0.27;  // relativer Anteil an Wasser der im Schnee gespeichert werden kann
static float MinRadMeltRate   = (float)1.5;		//minimum radiation melt factor      mm/d/C
static float MaxRadMeltRate   = (float)2.5;		//maximum radiation melt factor      mm/d/C
static float ReFreezCof  = (float)1;		// koeffizient für das wiedergefrieren des flüssigen Wassers
static float izero;
float  M_r,M_s,M_e,M_p,Mneg,fTemp,r,psych,Es,MeltRate,RadMeltRate;
float  r_a,r_b,r_c,r_d;
float  fSnowDensityMin		= (float)100; //minimale Schneedichte kg/m³
float  fSnowDensityMax		= (float)500; //maximale Schneedichte kg/m³


if(SimStart(pTi))
 {
	fSnowLiquid = (float)0;
	izero = (float)1;
}

if(izero>(float)0.5)
{
	fSnowOutflowDay=fSnowStorageDay=fSnowLiquidDay=(float)0; //Tageswerte null setzen
	izero = (float)0;
}

if (NewDay(pTi))
{
  // pWa->pWBalance->fReservoir=pWa->pWBalance->fReservoir + fSnowOutflowDay;

// input in den Schneespeicher
   fSnowInput = min((pCl->pWeather->fRainAmount * ( TempGrenzI + TempTrans/(float)2. - 
	              pCl->pWeather->fTempAve ) / ( (float)2. *TempTrans )),pCl->pWeather->fRainAmount);
      

   if(fSnowInput<(float)0.)
   {
 	fSnowInput=(float)0.;
   }
   
   fSnowStorage += fSnowInput; 

   pWa->pWBalance->fReservoir=pWa->pWBalance->fReservoir - fSnowInput;
 
   izero = (float)1;
 }
 


 fTemp = pCl->pWeather->fTempAve 
         + ((pCl->pWeather->fTempMax - pCl->pWeather->fTempMin) / (float)2)
         * (float)sin((float)2.0 * (float)PI 
         * (DayTime - (float)0.25 ));
   

 // Schmelzrate


 M_r = (float)1.2 * fTemp;
 M_r = max((float)0,M_r); 


 M_s = (TempMeltRate + WindMeltRate * pCl->pWeather->fWindSpeed )
	   * (fTemp - TempGrenzO );
 M_s = max((float)0,M_s); 

 r =  (float)2501 - (float)2.37 * fTemp;          // spez Verdampfungsw„rme (J/g)
 psych = (float)1013 * (float)1.005 / ((float)0.622 * r);  //Psychrometerkonstante  
 Es = (float)6.11 * (float)exp(((float)17.3 * fTemp)/((float)237.3 + fTemp));

 M_e = (TempMeltRate + WindMeltRate * pCl->pWeather->fWindSpeed )*(Es - (float)6.11)/ psych;
 M_e = max((float)0,M_e); 

 M_p = (float)0.0125*(pCl->pWeather->fRainAmount - fSnowInput) * fTemp;
 M_p = max((float)0,M_p); 


 r_b = (MaxRadMeltRate - MinRadMeltRate)/(float)2 ; /* Amplitude der Sinusfunktion */
 r_a = MinRadMeltRate + r_b;  /* Verschiebung der Sinusfunktion entlang der Ordinate */
 r_c = (float)0.017214206; /* 2*pi/365 Anpassung der Periode der Sinusschwingung */
 r_d = -(float)80.0; /* Differenz zwischen dem 1.1. und dem 21.3, wenn sin(c*(d+tag)) == 0 */

 RadMeltRate = r_a + r_b * (float)sin(r_c*(r_d + pTi->pSimTime->iJulianDay));

 Mneg = ReFreezCof * RadMeltRate * (fTemp - TempGrenzO);  // Wiedergefrierrate
 Mneg = min(0,Mneg);

 MeltRate = (M_r + M_s + M_e + M_p + Mneg) * DeltaT;

 MeltRate = min(MeltRate,fSnowStorage);
 if(MeltRate<(float)0) MeltRate = max(MeltRate,-fSnowLiquid);

 fSnowLiquid += MeltRate;

 fSnowStorage -= MeltRate;
 
 fSnowOutflow = max((float)0,fSnowLiquid - SnowLiquidStorage * fSnowStorage);

 if(fSnowOutflow>(float)0) fSnowLiquid = SnowLiquidStorage * fSnowStorage;

 pWa->pWBalance->fReservoir=pWa->pWBalance->fReservoir + fSnowOutflow;

 
 fSnowOutflowDay += fSnowOutflow;	// Tagessumme Schneeschmelze
 fSnowStorageDay += fSnowStorage * DeltaT; //Tageesmittelwert Schneespeicher
 fSnowLiquidDay  += fSnowLiquid * DeltaT; //Tageesmittelwert fl. Anteil Schneespeicher


 return 1;
}  /*  end  */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : Snow4()                                    */
/*                                                        */
/*  Function : Schneemodell, nach SOIL-Modell             */


/*  Author   : cs 03.04.00                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/* int WINAPI Snow3(EXP_POINTER)
{
 DECLARE_COMMON_VAR

//float DeltaT      = pTi->pTimeStep->fAct;
float	DayTime     = pTi->pSimTime->fTimeDay;
static float TempGrenzI   = (float)0.5; // Grenztemperatur REgen-Schnee (bei Niederschlag) °C
static float TempGrenzO   = (float)0.;  // Grenztemperatur Schneeschmelze °C
static float TempTrans    = (float)1.;  // Übergangsbereich SChnee REgen  °c
static float TempMeltRate = (float)1.;  // Schmelzrate	mm/(°C day)
static float WindMeltRate = (float)0.8;  // SChmelzrate mm/(°C m/s day)
static float SnowLiquidStorage = (float)0.27;  // relativer Anteil an Wasser der im Schnee gespeichert werden kann
static float MinRadMeltRate   = (float)1.5;		//minimum radiation melt factor      mm/d/C
static float MaxRadMeltRate   = (float)2.5;		//maximum radiation melt factor      mm/d/C
static float ReFreezCof  = (float)1;		// koeffizient für das wiedergefrieren des flüssigen Wassers
static float izero;
float  M_t,M_r,M_e,M_p,Mneg,fTemp,r,psych,Es,MeltRate,RadMeltRate;
float  s1,s2,SnowAge;

if (NewDay(pTi))
{
 
 
	
if(SnowInput<(float)0.1)
{
	SnowAge = SnowAge + (float)1;
}
else
{
	SnowAge = (float)0;
}


 
 if(pCl->pWeather->fTempAve>(float)0)  //Tenmperaturterm Schneeschmelze
 {
	 M_t = pCl->pWeather->fTempAve * mt ;
 }
 else
 {
	 M_t=(float)0;
 }




 
 M_r = mrmin * ((float)1 + s1 * ((float)1 - (float)exp(-SnowAge * s2);*/
