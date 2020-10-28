/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:  
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Sammlung der Modellansaetze zur Berechnung der potentiellen und
 *   aktuellen Evapotranspiration.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: evapotra.c $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:23
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/water
 * Fehler durch nicht initialisierte Variablen behoben.
 * Methoden sollten auch im Fehlerfall einen initialisierten Resultwert
 * zurückgeben.
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * 
 *   Zusammenfuehren der Funktionen erfolgte am 12.03.97
 *
*******************************************************************************/

#include "xinclexp.h"
#include "xh2o_def.h"
#include "xlanguag.h"     

#define PSY (float)0.663           // PSY  Muss fuer hohe Standorte evt. reduziert werden.
#define AF (float)0.75             // AF = 1-Albedo.  0.75 fuer Vegetationsflaeche
#define CONST_A (float)0.22        // Konstanten zur Berechnung der aktuellen Sonnenscheindauer
#define CONST_B (float)0.55        // Durschschnittswerte fuer Raum München
#define SQR(x) ((x)*(x))

#define MINIMAL_TIMESTEP_CAPACITY_MODEL (float)0.1

#define PlantIsGrowing  ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))


/* from util_fct.c */
extern float abspower(float x, float y);
extern int NewTenthDay(PTIME);
extern int NewDay(PTIME);
extern int SimStart(PTIME);      
extern int WINAPI Message(long, LPSTR);
extern int WINAPI Test_Range(float x, float x1, float x2, LPSTR c);
extern double WINAPI _loadds LIMIT(double v1, double v2, double x);

// from toolslib.c
extern double WINAPI _loadds INSW(double x, double y1, double y2);
extern double WINAPI _loadds NOTNUL(double x);

/* from LEACH.c */
extern float WINAPI LAI2CropCover(float fLAI);

/* from SPASS.c */
extern float WINAPI _loadds DaylengthAndPhotoperiod_SPASS(float fLatitude, int nJulianDay, int nID);
extern float WINAPI _loadds ExtraterrestrialRadiation_SPASS(float fLatitude, int nJulianDay);
extern float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp);

//from gecros.c
extern double difla(double NRADC,double PT,double RBH,double RT);

// Funktionsdeklaration
float WINAPI SolPartTime(PTIME pTi);
int WINAPI SolRadSunDur(EXP_POINTER);

int WINAPI potevapotraHaude(EXP_POINTER);
int WINAPI potevapotraMakkink(EXP_POINTER);
int WINAPI potevapotraPenman(EXP_POINTER);
int WINAPI potevapotraPenMonteith(EXP_POINTER);
int WINAPI potevapotraCanada(EXP_POINTER);

int WINAPI Interception(EXP_POINTER);
float WINAPI CropCoeff(EXP_POINTER);
int WINAPI DualCropCoeff(EXP_POINTER,float *x, float *y);
int WINAPI SingleCropCoeff(EXP_POINTER,float *x, float *y); //SG20160805
int WINAPI PTran(float,float,float,float,float,float,float,float,float,float,float,float *,float *);

int WINAPI potevap1(EXP_POINTER);//potevapCC-CF //canopy closure and crop factor
int WINAPI potevap2(EXP_POINTER);//potevapLAI  //leaf area index
int WINAPI potevap3(EXP_POINTER,float x,float y);//potevapDCC //dual crop coefficient
int WINAPI potevapYin(EXP_POINTER);

int WINAPI aktevap1(EXP_POINTER);
int WINAPI aktevapYin(EXP_POINTER);


signed short int WINAPI potEvapoTranspirationRitchie(EXP_POINTER);
signed short int WINAPI potEvaporationRitchie(EXP_POINTER);
signed short int WINAPI aktEvaporationRitchie(EXP_POINTER);

extern int iCropCover;
extern float fPotTraDay, fActTraDay;

float fActEvapDay;

int   iDualCropFactor;
float fCropBasalIni, fCropBasalMid, fCropBasalEnd;
float fCropTimeIni, fCropTimeDev, fCropTimeMid, fCropTimeEnd;

/*********************************************************************************************/
/*                                                                                           */
/*   Modul   : Evapotranspiration  Methode LeachN  /Penman/.....s.u.                         */
/*   Name    : ET_PENMA.c                                                                    */
/*   Autor   : cs/age bis 94                                                                 */
/*             ch 6/95                                                                       */
/*             ch: Vergleiche mit Quelltext von Rainer Kloss 6.5.94                          */
/*                 cropfactor nach swatre                                                    */
/*             ch 19.01.96, Ueberpruefen der Eingabe-Daten                                   */
/*             dm 15.04.96, Implementierung der neuen Variablenstruktur                      */
/*             dm 15.11.96, Penman, pot. Evaporation durch pot. Evapotranspiration ersetzt   */  
/*                                                                                           */
/*   Inhalt  : Berechnung der potentiellen Evaporation                                       */
/*             aktuelle Evap. ist tatsaechlicher oberer Ausfluß                              */
/*********************************************************************************************/

/*********************************************************************************************/
/*                                                                                           */
/*   Modul   : Interne Funktion                                                              */
/*   Name    : SolPartTime(PTIME pTi)                                                        */
/*   Autor   :                                                                               */
/*                                                                                           */
/*********************************************************************************************/
float WINAPI SolPartTime(PTIME pTi)
{
 DECLARE_COMMON_VAR

 float  y;
 double T1, T2;     
 double DayTime = (double) pTi->pSimTime->fTimeDay; 
 double DT      = (double) pTi->pTimeStep->fAct;

 if ((DayTime<=(double)0.3)||((DayTime>(double)0.8)&&((DayTime-DT)>=(double)0.8)))
  {
   T1 = (double)0.0;
   T2 = (double)0.0;
  }
 else
  {
   T1 = max((double)0.0,(DayTime - DT - (double)0.3));
   T2 = min((double)0.5,(DayTime - (double)0.3));
  } /* else DayTime  */

 if (DeltaT < (float)0.11)
   {y = (float)(cos((double)(2.0*PI)*T1)-cos((double)(2.0*PI)*T2))/(float)2.0;}
 else
   {y = DeltaT;}
 //y = DeltaT;

 return y;
}

/*********************************************************************************************/
/*                                                                                           */
/*   Modul   : Interne Funktion                                                              */
/*   Name    : SolPartTime(PTIME pTi)                                                        */
/*   Autor   :                                                                               */
/*                                                                                           */
/*********************************************************************************************/
int WINAPI SolRadSunDur(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 PWEATHER  		pWE = pCl->pWeather;

 float   fJ, fdr, fdelta, fphi, ftemp, fomegas;
 float	 fRa, fN, fn;
 float   fas=(float)0.25, fbs=(float)0.5;

 fJ = (float)pTi->pSimTime->iJulianDay;
		
 fdr = (float)1 + (float)0.033*(float)cos((float)0.0172*fJ);	     //equation (21)
		
 fdelta = (float)0.409*(float)sin((float)0.0172*fJ - (float)1.39);   //equation (22)
 fphi   = pLo->pFarm->fLatitude*(float)PI/(float)180;								
		
 ftemp = -(float)tan(fphi)*(float)tan(fdelta);					     //equation (20)
 if(     ftemp <= (float)-1)	fomegas = (float)PI;
 else if(ftemp >= (float)1 )	fomegas = (float)0;
 else                           fomegas = (float)acos(ftemp);
		
 fRa = (float)37.6*fdr*(fomegas*(float)sin(fphi)*(float)sin(fdelta)  //equation (19)
	  + (float)cos(fphi)*(float)cos(fdelta)*(float)sin(fomegas));
 fN  = (float)24/(float)PI * fomegas;							     //equation (25)
		

 if(pWE->fSolRad >= (float)0)
 {
  // Berechnung der aktuellen Sonnenscheindauer                      //equation (52)
			fn  = pWE->fSolRad/fRa;
			fn -= fas;// a_s=0.25
			fn *= fN;
			fn /= fbs;// b_s=0.50
			fn = min(fN,fn);
			fn = max((float)0, fn);
			//fn = min((float)16,fn);
		    if (pWE->fSunDuration < (float)0)
               pWE->fSunDuration = fn; //Expert-N Variable
 }
 else
 {
   if (pWE->fSunDuration >= (float)0)
   {
		    // Berechnung der aktuellen Globalstrahlung //equation (52)
            fn = pWE->fSunDuration;
			pWE->fSolRad = fRa*(fn/fN * fbs + fas);
   }
   else
   {
   //Fehlermeldung
   }
 }
	
 return 1;
}


/***********************************************************************************************/
/* Funktion    :   potevapotraPenman                                                           */
/* Beschreibung:   Potentielle Evaporation nach Penman                                         */                      
/*                                                                                             */
/* ep/gsf          20.10.94                                                                    */
/* ch/gsf          20.06.95                                                                    */
/*                 07.11.95  neue Syntax                                                       */
/*                 19.01.96  Korrektur,	Ueberpruefen der Eingabe-Daten.                        */
/*                                                                                             */
/* Berechnung der potentiellen Verdunstung nach Penman (Entwurf VDI 3786 Blatt 13 S.25)        */
/*                                                                                             */
/* Eingabe:       Die folgenden Groessen aus Messwerten in 2m Höhe:                            */
/*  			  T  Tagesmittel Durchschnittstemperatur                                       */
/*                RF rel. Luftfeuchte                                                          */
/*                V  Windgeschw.                                                               */
/*                G  Globalstrahlung                                                           */
/*                S  akuelle Sonnenscheindauer                                                 */
/*                                                                                             */
/***********************************************************************************************/
/* veränd. Var.:	pLo->pFarm->fLatitude                                                      */
/*					pWE->fHumidity                                                             */
/*                  pWE->PanEvap                                                               */
/*					pWE->fSatDef                                                               */
/*					pWE->fSolRad                                                               */
/*					pWE->fSunDuration                                                          */
/*					pWE->fWindSpeed                                                            */
/*                  pWa->fPotETDay                                                             */
/*                  pWa->fPotETdt                                                              */
/*                  pEV->fPotR                                                                 */
/***********************************************************************************************/
int WINAPI  potevapotraPenman(EXP_POINTER)
{
 
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER

 PWEATHER  		pWE = pCl->pWeather;
 PEVAP     		pEV = pWa->pEvap;
// PCANOPY        pPC;
  
 float EvTeiler;
 float f1;

 float   fs01,fs02,fs0,fs1,fs2;
 double  ddk,dx;
 float   frk,frl,fft,ffe,ffn,fdt;
 float   ffu, fgs, fw0, fg0;
 

 H2O_ZERO_LAYER

 //if((pWE->fPanEvap < 0) || (pWE->fPanEvap > 500) && NewDay(pTi))
 if(NewDay(pTi))
   {
	SolRadSunDur(exp_p);

	if (!Test_Range(pWE->fHumidity,(float)0,(float)100,"fHumidity"))
		pWE->fHumidity = (float)77;
	
	if (!Test_Range(pWE->fWindSpeed,(float)0,(float)100,"fWindSpeed"))
		pWE->fWindSpeed = (float)0;
	    
	if (!Test_Range(pLo->pFarm->fLatitude,(float)20,(float)70,"geographical latidude"))
		pLo->pFarm->fLatitude = (float)48.3;
      
    if (!Test_Range(pWE->fSolRad,(float)0,(float)40,"global radiation"))
 	    pWE->fSolRad = (float)12;         
        	  
	if (!Test_Range(pWE->fSunDuration,(float)0,(float)24,"sun duration"))
	    pWE->fSunDuration = (float)7;


	//   Saettigungsdampfdruck
	fdt = (float)((float)6.11* exp((double)17.269 * (pWE->fTempAve)
          / ((double)237.3 + pWE->fTempAve)));   


	// fs1 Steigung des Saettigungsdampfdruckes   
    fs1= (float)((float)6.11* exp((double)17.269*(pWE->fTempAve + (double)1)
          / ((double)238.3+pWE->fTempAve)));
          
    fs1 -= fdt;  

    fs2 = fs1/(fs1+ PSY);

	//    fgs   Gesamtstrahlung [ J/ cm2]        0 - 4000
	//          heuristisch: um die Rechenbeispiele rekonstruieren 
	//          zu können.
	//
	//			SolRad          [ MJ / qm]       0 - 40


    fgs = pWE->fSolRad*(float)100; // [MJ/qm] --> [J/cm2]

	//    frk kurzwellige Strahlungsbilanz 
    frk = AF * fgs /(float)251.0;


	//    Temperatur Faktor
    fft =  (float)1.98e-9 * abspower((float)273.0 + pWE->fTempAve,(float)4.0);
          
	//    Feuchte Faktor
    ffe = (float)0.34 - (float) 0.044 *
           abspower(fdt * pWE->fHumidity /(float)100.0,(float)0.5);  
                             
    dx   = (double)0.9856 * pTi->pSimTime->iJulianDay - (double)2.796;
    ddk  = (double)asin((double)0.3978 
         *  sin(GradToRad*(dx -(double)77.369 +(double)1.916 *sin(GradToRad*dx) ))); // dx: Grad -> Rad 
    fs01 = (float)tan(pLo->pFarm->fLatitude * GradToRad);   // GeoBreite Grad -> Rad
    fs02 = (float)tan(ddk); // ddk: Rad

	//   Max. mögliche Sonnenscheindauer
    fw0 = (float)acos(-fs01*fs02) / GradToRad; // Rad -> Grad
    fs0 = (float)0.13333 * fw0;
         
    Test_Range(fs0,(float)0,(float)24,"fs0\n in et_penma.c");
                                              

	if ((pWE->fSunDuration < (float)0) || NewDay(pTi))
	   {
		// Berechnung der aktuellen Sonnenscheindauer
 
		//  extraterrestrische Strahlung 
		fg0 = (float)(sin((double)pLo->pFarm->fLatitude * GradToRad) 
		    * sin(ddk)   
		    * fw0               
		    * (float)0.0175);

		fg0 += (float)(cos((double)pLo->pFarm->fLatitude* GradToRad) 
		     * cos(ddk)   
		     * sin((double)fw0 * GradToRad));

		fg0 *= (float)10444;
                     
		// Gesamtstrahlung wird hier in [Wh/m2] benutzt!                     
		pWE->fSunDuration  = pWE->fSolRad*(float)1000/((float)3.6*fg0);
		pWE->fSunDuration -= CONST_A;
		pWE->fSunDuration *= fs0;
		pWE->fSunDuration /= CONST_B;
        
        pWE->fSunDuration  = min(fs0,pWE->fSunDuration);
        
		pWE->fSunDuration  = max((float)0,pWE->fSunDuration);
		pWE->fSunDuration  = min((float)16,pWE->fSunDuration);
	   }
                                              
	//    Bewölkung Faktor
    ffn=(float) 0.1 + (float) 0.9 * pWE->fSunDuration/fs0;
    

	//    frl langzwellige Strahlungsbilanz 
    frl = fft * ffe * ffn;


	// 		Sättigungsdefizit
	if (pWE->fSatDef<(float)0)   
       {
   		pWE->fSatDef = ((float)1.0 - pWE->fHumidity/(float)100.0) * fdt;
       }
    
	//	   Windeinfluss
    ffu = (float)0.27 * ((float)1.0 + (float)0.864 * pWE->fWindSpeed);

    f1  = fs2 * (frk - frl);
    f1 += ((float)1.0 - fs2) * pWE->fSatDef * ffu;
    
    pWE->fPanEvap = max(f1,(float)0);
    
    pWa->fPotETDay = pWE->fPanEvap; //ch * lpk->PanFaktor;
	 
   } // end if NewDay && Range of pWE->fPanEvap	
	

   EvTeiler = (DeltaT < (float)0.11)? 
	 		 SolPartTime(pTi)
	 		 : (float)1;
	 
   pWa->fPotETdt = EvTeiler * pWa->fPotETDay;

 return  1;

}  /* Ende Berechnung der potentiellen Evapotranspiration nach Penman (VDI) */
          

/***********************************************************************************************/
/* Funktion:       aktevap1                                                                    */
/* Beschreibung:   Berechnet akt. Evaporation                                                  */
/*                 Berechnet Kum. akt. Evap.                                                   */
/*                                                                                             */
/* ch/gsf          7.11.95                                                                     */
/*            as, 28.11.96  Einf. veränd. Var. im Header                                       */
/*                                                                                             */
/***********************************************************************************************/
/* veränd. Var.:		pEv->fActR                                                             */
/*                                                                                             */
/***********************************************************************************************/                   
int WINAPI  aktevap1(EXP_POINTER)
{
  PEVAP     pEV = pWa->pEvap;
  
  /* Actual Evaporation is the minimum of potential and maximal evaporation */

  if (pEV->fMaxR > pEV->fPotR)
  {
   pEV->fActR = pEV->fPotR;
  }
  else
  {
  pEV->fActR = pEV->fMaxR;						  
  }

  return 1;
}  /* Ende akt. Evaporations - Berechnung    */


/***********************************************************************************************/
/* Funktion:       aktevapYin                                                                  */
/* Beschreibung:   Berechnet akt. Evaporation                                                  */
/*                                                                                             */
/* ep/hmgu         04.07.08                                                                    */
/*                                                                                             */
/***********************************************************************************************/
/* veränd. Var.:		pEv->fActR                                                             */
/*                                                                                             */
/***********************************************************************************************/                   
int WINAPI  aktevapYin(EXP_POINTER)
{
 DECLARE_COMMON_VAR

 PWEATHER  pWE = pCl->pWeather;
 PEVAP     pEV = pWa->pEvap;
 PWLAYER   pWL=pWa->pWLayer;

 extern double SD1;

 float fpl  = (float)max((float)2.,(float)SD1);//(float)5;// plow layer 25 cm deep
 float fWUL = (float)0;// water content upper layer
 float fWLL = (float)0;// water content upper layer
 float f1;
 //float fLAI, fCropCover;
 float DeltaZ = pSo->fDeltaZ;

 float fDAYTMP, fSVP, fDVP, fDVPTmax, fDVPTmin, fVPD;
 float fWND, fGR, fSC;
 float fDWSUP,fWSUP;

 double DEC,SINLD,COSLD,AOB,DAYL,DDLP,DSINB,DSINBE;
 double HOUR, DAYFRAC, SUNRIS, SINB;
 double INSP = (double)-2;
 double dLatitude;

   fDAYTMP = pWE->fTempAve;
   fWND  = pCl->pWeather->fWindSpeed;  //[m s-1]

   fGR     = pWE->fSolRad*(float)1E6;
   fSC     = (float)1367*((float)1.+(float)0.033*(float)cos(2.0*PI*
	                                      ((double)pTi->pSimTime->iJulianDay-10.)/365.0));

   fDVPTmax = (float)0.611*(float)exp((float)17.4*pWE->fTempMax/(pWE->fTempMax+(float)239));
   fDVPTmin = (float)0.611*(float)exp((float)17.4*pWE->fTempMin/(pWE->fTempMin+(float)239));
   fDVP     = (float)0.5*(fDVPTmax + fDVPTmin);
   fSVP     = (float)0.611*(float)exp((float)17.4*pWE->fTempAve/(pWE->fTempAve+(float)239));
   fVPD     = max(0,fSVP-fDVP);

    for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL,iLayer<=pSo->iLayers-2;pWL=pWL->pNext,iLayer++)    
    {
     if (iLayer*DeltaZ <= fpl*(float)10.) 
	    fWUL += pWL->fContAct* DeltaZ;
	 else if ((fpl*(float)10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= fpl*(float)10. + DeltaZ))
     {
        f1 = (iLayer*DeltaZ-fpl*(float)10.)/DeltaZ;
		fWUL += ((float)1-f1)*pWL->fContAct*DeltaZ;
		fWLL += f1*pWL->fContAct*DeltaZ;
     }
	 else
		//pWL->fContAct=(float)(WLL/NOTNUL((double)fProfileDepth-RD*10.));
		fWLL += pWL->fContAct*DeltaZ;

     }//for

	fDWSUP = max((float)0.1,fWUL+(float)0.1);

	if (DeltaT < (float)1)
	{
	  //Declination of the sun as function of iJulianDaynumber (iJulianDay)
      DEC = -asin( sin(23.45*PI/180)*cos(2.0*PI*(double)(pTi->pSimTime->iJulianDay+10)/365.0));
      dLatitude = (double)pLo->pFarm->fLatitude;
      //SINLD, COSLD and AOB are intermediate variables
      SINLD = sin(PI/180*dLatitude)*sin(DEC);
      COSLD = cos(PI/180*dLatitude)*cos(DEC);
      AOB   = SINLD/COSLD;
      
      //iJulianDaylength (DAYL) 
      DAYL   = 12.0*(1.0+2.0*asin(AOB)/PI);
      DDLP   = 12.0*(1.+2.*asin((-sin(INSP*PI/180)+SINLD)/COSLD)/PI);
      //SINB = SINLD + COSLD*cos(2.0*PI(fSolarTime-12.0)/24.0), fSolarTime [h]
      DSINB  = 3600.0*(DAYL*SINLD+24.0*COSLD*sqrt(1.0-AOB*AOB)/PI);//daily integral over SINB
      DSINBE = 3600.0*(DAYL*(SINLD+0.4*(SINLD*SINLD+COSLD*COSLD*0.5))
                  +12.0*COSLD*(2.0+3.0*0.4*SINLD)*sqrt(1.0-AOB*AOB)/PI);//daily integral
	  //%---timing for sunrise
      SUNRIS = 12.0 - 0.5*DAYL;

      //%---specifying the time (HOUR) of a day
      //(at HOUR, radiation is computed and used to compute assimilation)
	  if (pTi->pSimTime->fTimeDay*(float)12 > (float)SUNRIS) 
	  {
	   DAYFRAC= ((double)pTi->pSimTime->fTimeDay*24.-SUNRIS)/24.;
	  }
	  else
	  {
	   DAYFRAC = (double)0;
	  }

      HOUR = SUNRIS+DAYL*DAYFRAC;
             
      //%---sine of solar elevation
      SINB  = max(0.0, SINLD+COSLD*cos(2.0*PI*(HOUR-12.0)/24.0));

	  fWSUP  = fDWSUP*(float)((SINB*fSC/1367.)/DSINBE);

	  pEV->fActR=min(pWa->fPotETdt,pWa->fPotETdt/(fVPD+pWa->fPotETdt)*fWSUP*fpl/(float)150.);
	}
	//else
	{
      fWSUP = fDWSUP;
	  pEV->fActR=min(pWa->fPotETDay,pWa->fPotETDay/(fVPD+pWa->fPotETDay)*fWSUP*fpl/(float)150.);
	}
 ///*
    if ((pPl != NULL)&& PlantIsGrowing)
    {
      //fLAI       = pPl->pCanopy->fLAI;//[m2 m-2]
	  //fCropCover = LAI2CropCover(fLAI);
	  //pEV->fActDay *= (1-fCropCover);
	  //pEV->fActDay *= (float)0.5;
	  pEV->fActR = pEV->fActDay;
	  //pEV->fActR *= (float)0.1;
    }
 //*/
  return 1;
}  /* Ende akt. Evaporations - Berechnung    */

/*********************************************************************************/
/*                                                                               */
/*  Modul Evapotranspiration nach Ritchie aus NSIM/CERES                         */
/*                                                                               */
/*  Name    : WATER.C                                                            */
/*  Inhalt  : Berechnung der potentiellen und aktuellen                          */
/*            Evapotranspiration, Evaporation und Transpiration                  */
/*            mit Zeitfilter                                                     */
/*                                                                               */
/*  Autor   : Th. Schaaf                                                         */
/*  Datum   : 12.09.96                                                           */
/*********************************************************************************/

/*********************************************************************************/
/*  Name     : potEvapoTranspirationRitchie()                                    */
/*                                                                               */
/*  Funktion : Berechnung der potentiellen Evapotranspiration                    */
/*             Methode nach NSIM                                                 */
/*             (neue Aufteilung Evapotranspiration, Evaporation, Transpiration)  */
/*                                                                               */
/*  Autor    : Th. Schaaf, E.Priesack                                            */
/*  Datum    : 12.08.96, 22.07.99                                                */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pSo->fAlbedo                                                      */
/*             pWa->fPotETDay                                                     */                             
/*             pWa->pEvap->fPotR                                                 */
/*             pWa->pEvap->fPotDay                                               */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI potEvapoTranspirationRitchie(EXP_POINTER)
{
 	signed short int OK=0;
	int	    stadium;
  	float	temp,ev,LAI,f1;
    float	DeltaT       = pTi->pTimeStep->fAct;

    if (NewDay(pTi))
   {
	temp = ((float)0.6 * pCl->pWeather->fTempMax)
            + ((float)0.4 * pCl->pWeather->fTempMin);

	if (pCl->pWeather->fSolRad < (float)0)
	{
	  SolRadSunDur(exp_p);
	  if (!Test_Range(pCl->pWeather->fSolRad,(float)0,(float)40,"global radiation"))
 	  pCl->pWeather->fSolRad = (float)12;
	}
 /*Festlegung des Gesamtalbedos in Abhaengigkeit von der physiologischen Entwicklung 
   der Pflanze. Wenn es keine Pflanze bzw. kein Speicher fuer die Struktur Pflanze
   allokiert wurde, wird das Entwicklungsstadium mit 0 initialisiert. */

	if (pPl == NULL)
	{
		stadium = 0;
	    LAI = (float)0;
	}
	else
	{
		stadium = pPl->pDevelop->iStageCERES;
		LAI = pPl->pCanopy->fLAI;
	}

     pSo->fAlbedo = pSo->fSoilAlbedo;

     if ((stadium>0)&&(stadium <= 6))
     {
       if (stadium < 5)
       {
         pSo->fAlbedo = (float)0.23 - ((float)0.23 - pSo->fSoilAlbedo)
                        *(float)exp((double)((float)-0.75 * LAI));
		} // Ende if (pPl->Develop->iStageCERES < 5)
       else
       {
         pSo->fAlbedo = (float)0.23 +
                       ((LAI - (float) 4.0) *
                       (LAI - (float) 4.0)/(float)160.0);
       }// Ende else (pPl->Develop->iStageCERES < 5) 
	} // Ende if (pPl->Develop->iStageCERES <= 6)

 
     if (pCl->pWeather->fSnow > (float) 0.5)
     {
       pSo->fAlbedo = (float)0.6;
     } //Ende if (pCl->pWeather->fSnow > (float) 0.5) 


 	 /* Einfluss der Strahlung in Abhaengigkeit vom Gesamtalbedo    */
     ev =  max((float)0,pCl->pWeather->fSolRad) * ((float)4.88e-3 -((float)4.37e-3
     		 * pSo->fAlbedo)) * (temp + (float)29.0);

 
 	 /* Drei Temp.stufen: groesser 24 Grad; kleiner 5 Grad;
 	  								zwischen 5 und 24 Grad      */
     if (pCl->pWeather->fTempMax > (float)24.0)
     {
     pWa->fPotETDay = ev * ((pCl->pWeather->fTempMax - (float)24.0)
                              * (float)0.05 + (float)1.1);
     }  // Ende if (pCl->pWeather->fTempMax > (float)24.0) 

     else if(pCl->pWeather->fTempMax < (float)5.0) //Temp. kleiner als 5 Grad
         {
     pWa->fPotETDay = ev *  (float)0.01 *
                        ((float)exp((double)
                    ((float)0.18*(pCl->pWeather->fTempMax + (float)20.0))));
         }  // Ende Temp. kleiner als 5 Grad
         
         else  //Temp. zwischen 5 und 24 Grad
         {
     pWa->fPotETDay = ev *(float)1.1;
         }  // Ende Temp. zwischen 5 und 24 Grad

	/* Einfluss des Blattfaechenindexes: groesser 1; kleiner 1    */
     if (LAI > (float)1.0) //Blattflaechenindex groesser 1
     {
        f1 = ev * (float)exp((double)((float)-0.4 * LAI));
     	pWa->pEvap->fPotR = (float)min((double)f1,(double)pWa->fPotETDay);
	 }  // Ende Blattflaechenindex groesser 1
     else  // Blattflaechenindex kleiner oder gleich 1
     {
        pWa->pEvap->fPotR = pWa->fPotETDay * 
        					((float)1.0 - ((float)0.43 * LAI));
     } // Ende Blattflaechenindex kleiner oder gleich 1

	pWa->pEvap->fPotDay = pWa->pEvap->fPotR;
                         
    } /* Ende NewDay */

	pWa->fPotETdt=pWa->fPotETDay*DeltaT;

  return OK;
}

/*********************************************************************************/
/*  Name     : potEvaporationRitchie()                                           */
/*                                                                               */
/*  Funktion : Berechnung der potentiellen Evaporation                           */
/*             Methodik  NSIM                                                    */
/*             (neue Aufteilung Evapotranspiration, Evaporation, Transpiration)  */
/*                                                                               */
/*  Autor    : Th. Schaaf, E. Priesack                                           */
/*  Datum    : 12.08.96, 22.07.99                                                */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pSo->fAlbedo                                                      */
/*             pWa->pEvap->fPotR                                                 */
/*             pWa->pEvap->fPotDay                                               */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI potEvaporationRitchie(EXP_POINTER)
{
 	signed short int OK=0;
	int	    stadium;
  	float	temp,ev,LAI,f1;
    float	DeltaT       = pTi->pTimeStep->fAct;

    if (NewDay(pTi))
    {
    temp = ((float)0.6 * pCl->pWeather->fTempMax)
            + ((float)0.4 * pCl->pWeather->fTempMin);

	if (pCl->pWeather->fSolRad < (float)0)
	{
	  SolRadSunDur(exp_p);
	  if (!Test_Range(pCl->pWeather->fSolRad,(float)0,(float)40,"global radiation"))
 	  pCl->pWeather->fSolRad = (float)12;
	}
 /*Festlegung des Gesamtalbedos in Abhaengigkeit von der physiologischen Entwicklung 
   der Pflanze. Wenn es keine Pflanze bzw. kein Speicher fuer die Struktur Pflanze
   allokiert wurde, wird das Entwicklungsstadium mit 0 initialisiert. */

	if (pPl == NULL)
	{
		stadium = 0;
	    LAI = (float)0;
	}
	else
	{
		stadium = pPl->pDevelop->iStageCERES;
		LAI = pPl->pCanopy->fLAI;
	}

     pSo->fAlbedo = pSo->fSoilAlbedo;

     if (stadium <= 6)
     {
       if (stadium < 5)
       {
         pSo->fAlbedo = (float)0.23 - ((float)0.23 - pSo->fSoilAlbedo)
                        *(float)exp((double)((float)-0.75 * LAI));
		} // Ende if (pPl->Develop->iStageCERES < 5)
       else
       {
         pSo->fAlbedo = (float)0.23 +
                       ((LAI - (float) 4.0) *
                       (LAI - (float) 4.0)/(float)160.0);
       }// Ende else (pPl->Develop->iStageCERES < 5) 
	} // Ende if (pPl->Develop->iStageCERES <= 6)

 
     if (pCl->pWeather->fSnow > (float) 0.5)
     {
       pSo->fAlbedo = (float)0.6;
     } //Ende if (pCl->pWeather->fSnow > (float) 0.5) 


 	 /* Einfluss der Strahlung in Abhaengigkeit vom Gesamtalbedo    */
     ev =  pCl->pWeather->fSolRad * ((float)4.88e-3 -((float)4.37e-3
     		 * pSo->fAlbedo)) * (temp + (float)29.0);

	/* Einfluss des Blattfaechenindexes: groesser 1; kleiner 1    */
     if (LAI > (float)1.0) //Blattflaechenindex groesser 1
     {
        f1 = ev * (float)exp((double)((float)-0.4 * LAI));
     	pWa->pEvap->fPotR = (float)min((double)f1,(double)pWa->fPotETDay);
	 }  // Ende Blattflaechenindex groesser 1
     else  // Blattflaechenindex kleiner oder gleich 1
     {
        pWa->pEvap->fPotR = pWa->fPotETDay * 
        					((float)1.0 - ((float)0.43 * LAI));
     } // Ende Blattflaechenindex kleiner oder gleich 1

	pWa->pEvap->fPotDay = pWa->pEvap->fPotR;
                         
    } /* Ende NewDay */


  return OK;
}



/*********************************************************************************/
/*  Name     : aktEvaporationRitchie                                             */
/*                                                                               */
/*  Funktion : Belegung der Systemvariablen zur Evaporation mit neu errechneten  */
/*             potentiellen und aktuellen Simulationswerten Methodik NSIM        */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum    : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*             pWa->pEvap->fCumStage1                                            */
/*             pWa->pEvap->fTimeStage1                                           */
/*             pWa->pEvap->fCumStage2                                            */
/*             pWa->pEvap->fTimeStage2                                           */                             
/*             pWa->pEvap->fActDay                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI aktEvaporationRitchie(EXP_POINTER)
{
 	signed short int OK=0;
	float infiltEvapo,infiltration, wasrel,f1,aktEvapdt;
    float DeltaT       = pTi->pTimeStep->fAct;

	static BOOL NoCalculated;

	PWBALANCE  pWB  = pWa->pWBalance;



	if (NewDay(pTi))
	{

 	/* Beruecksichtigung der aktuellen Bodenfeuchte der ersten Bodenschicht
 	   bei der Berechnung der aktuellen Evaporation*/
    wasrel = (pWa->pWLayer->pNext->fContAct - pSo->pSWater->pNext->fContPWP)
             /(pSo->pSWater->pNext->fContFK - pSo->pSWater->pNext->fContPWP);

    wasrel = max((float)0.0,wasrel);


    if(wasrel>= (float)1.0)
    {
		pWa->pEvap->fCumStage1  =(float)0.0;
		pWa->pEvap->fCumStage2  =(float)0.0;
		pWa->pEvap->fTimeStage2 = (float)0.0;
        
    }  // Ende: if wasrel   

    if ((wasrel < (float)1.0)&&(wasrel >= (float)0.9))
    {
		if (pWa->pEvap->fMax > (float)10.0)
		{
			pWa->pEvap->fCumStage1  = ((float)1.0 / ((float)10.0-wasrel*(float)10.0))*pWa->pEvap->fMax;
			pWa->pEvap->fCumStage2  = (float)0.0;
			pWa->pEvap->fTimeStage2 = (float)0.0;		
		}
		else
		{
			pWa->pEvap->fCumStage1  = (float)100.0 - (wasrel * (float)100.0);
			pWa->pEvap->fCumStage2  = (float)0.0;
			pWa->pEvap->fTimeStage2 = (float)0.0;
		}
        
    }  // Ende: if wasrel   
    else
    {
		pWa->pEvap->fCumStage1  = pWa->pEvap->fMax;
		pWa->pEvap->fCumStage2  = (((float)25.0 - ((float)27.8 * wasrel)));
		pWa->pEvap->fTimeStage2 = ((pWa->pEvap->fCumStage2 * pWa->pEvap->fCumStage2)/((float)3.5 * (float)3.5));   
    }  // Ende: else wasrel   

 
	/* Berechnung des fuer Infiltration in die erste Schicht
 	   zur Verfuegung stehenden Wassers. */		

	infiltration = pWB->fReservoir - pWa->fRunOffR;
	
    infiltration -= pWa->fPondWater; /* [mm] */
	
	if (infiltration < (float)0)
	{                        
		infiltration = (float)0;               
	
		#ifdef LOGFILE
		 { 
		  Message(0,ERROR_RESERVOIR_NEGATIV);
		 } 
		#endif
	}
  
 	
	infiltEvapo = infiltration;
	NoCalculated = TRUE;

	/* Zur Berechnung der Evaporation stehen drei Bedingungen zur Verfuegung.
	Treffen die ersten beiden Moeglichkeiten nicht zu, wird nach der dritten
	MNethode berechnet.*/

	// 1. Berechnungsweise
   if ((pWa->pEvap->fCumStage1 >= pWa->pEvap->fMax) 
   							&& (infiltEvapo >= pWa->pEvap->fCumStage2))
   {
       infiltEvapo  -=  pWa->pEvap->fCumStage2;
       pWa->pEvap->fCumStage1  =  pWa->pEvap->fMax - infiltEvapo;
       pWa->pEvap->fTimeStage2 =  (float)0.0;
                       
       if(infiltEvapo > pWa->pEvap->fMax)
       {
          pWa->pEvap->fCumStage1 = (float)0.0;
       }
       pWa->pEvap->fCumStage1  += pWa->pEvap->fPotDay;

       if(pWa->pEvap->fCumStage1 > pWa->pEvap->fMax)
       {
          aktEvapdt=  pWa->pEvap->fPotDay - ((float)0.4
          						 * (pWa->pEvap->fCumStage1 - pWa->pEvap->fMax));
          pWa->pEvap->fCumStage2 = (float)0.6 * (pWa->pEvap->fCumStage1 - pWa->pEvap->fMax);

          pWa->pEvap->fTimeStage2 = (((pWa->pEvap->fCumStage2)/(float)(3.50))
          								*(pWa->pEvap->fCumStage2)/(float)(3.50));
       }  //  Ende pWa->pEvap->fCumStage1 > pWa->pEvap->fMax
       else
       {
          aktEvapdt= pWa->pEvap->fPotDay;
       }  // Ende  pWa->pEvap->fCumStage1 ist groesser als pWa->pEvap->fMax
       
    NoCalculated = FALSE;

    }  //  Ende infiltEvapo >= pWa->pEvap->fCumStage2
 
    // 2. Berechnungsweise
   if ((pWa->pEvap->fCumStage1 >= pWa->pEvap->fMax) 
   							&& (infiltEvapo < pWa->pEvap->fCumStage2))    
     {
      pWa->pEvap->fTimeStage2  += (float)1.0;
      aktEvapdt = ((float)sqrt((double)pWa->pEvap->fTimeStage2) * (float)3.5)
						       								 - pWa->pEvap->fCumStage2;
      if (infiltEvapo > (float)0.0)
       	{
       	  f1 = (float)0.8 * infiltEvapo;
       	  if (f1 <= aktEvapdt)
       	  	{
       	      f1 = aktEvapdt + infiltEvapo;
       	  	}
       	  	if(f1 > pWa->pEvap->fPotDay)
       	  	{
       	     f1 = pWa->pEvap->fPotDay;
       	  	}
     	   	aktEvapdt = f1;
      	}  // Ende  infiltEvapo > 0.0

      	else  //infiltEvapo <= 0
       	{
       	   aktEvapdt = min(aktEvapdt, pWa->pEvap->fPotDay);
       	} // Ende  infiltEvapo <= 0.0

       pWa->pEvap->fCumStage2 += (aktEvapdt - infiltEvapo);

       pWa->pEvap->fTimeStage2 = (pWa->pEvap->fCumStage2 * pWa->pEvap->fCumStage2)
                                /((float)(3.50*3.50));
      NoCalculated = FALSE;
     }

    // 3. Berechnungsweise
	if (NoCalculated)
   {
      if (infiltEvapo >= pWa->pEvap->fCumStage1)
      {
        pWa->pEvap->fCumStage1 = (float)0.0;
      }
      else
      {
        pWa->pEvap->fCumStage1 -= infiltEvapo;
      } // Ende else Infiltration >= fCumStage1

    pWa->pEvap->fCumStage1 += pWa->pEvap->fPotDay;

    if(pWa->pEvap->fCumStage1 > pWa->pEvap->fMax)
      {
         aktEvapdt = pWa->pEvap->fPotDay -
                               ((float)0.4 * (pWa->pEvap->fCumStage1 - pWa->pEvap->fMax));
         pWa->pEvap->fCumStage2 = ((float)0.6 * (pWa->pEvap->fCumStage1 - pWa->pEvap->fMax));
         pWa->pEvap->fTimeStage2 = ((pWa->pEvap->fCumStage2 / (float)3.50) 
         									*(pWa->pEvap->fCumStage2 / (float)3.50));
  	  }  // Ende pWa->pEvap->fCumStage1 > pWa->pEvap->fMax

  	 else   // pWa->pEvap->fCumStage1 <= pWa->pEvap->fMax
  	  {
    	 aktEvapdt = pWa->pEvap->fPotDay;
  	   }

   	} //  Ende NoCalculated
  
    //pWa->pEvap->fActDay = aktEvapdt;
	fActEvapDay	= aktEvapdt;
     /* Uebergabe auf Systemvariable. Bei variablen Zeitschritten 
     	muss hier der Zeitschritt berueckichtigt werden. */
	//	pWa->pEvap->fActR = pWa->pEvap->fActDay; 

    } /* Ende NewDay */

	pWa->pEvap->fActDay = fActEvapDay;
	pWa->pEvap->fActR = pWa->pEvap->fActDay; 

	return OK;
	}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : potevapotraPAN()                           */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int WINAPI potevapotraPan(EXP_POINTER)
{

 DECLARE_COMMON_VAR

 PWEATHER  		pWE = pCl->pWeather;
 PEVAP     		pEV = pWa->pEvap;
 
 float   EvTeiler;

 DeltaT     = pTi->pTimeStep->fAct;


 if(NewDay(pTi)) 
	 pWa->fPotETDay = (float)max((float)0.0,pWE->fPanEvap);
	 
	

 EvTeiler = (DeltaT < (float)0.11)? 
	 		SolPartTime(pTi)
	 		: (float)1;
	 
 pWa->fPotETdt = EvTeiler * pWa->fPotETDay;

 return  1;

}  /* Ende Berechnung der potentiellen Evapotranspiration aus der Kesselverdunstung */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : potevapotraHaude()                         */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int WINAPI potevapotraHaude(EXP_POINTER)
{

 DECLARE_COMMON_VAR

 PWEATHER  		pWE = pCl->pWeather;
 PEVAP     		pEV = pWa->pEvap;
  
 float   EvTeiler, fHaudeFac;
 float   Pi=(float)3.141592654;	
 float   fSD1,fSD2,fSatDef;

 DeltaT     = pTi->pTimeStep->fAct;

  if(NewDay(pTi))
  {

//  Berechnung der potentiellen Verdunstung nach Haude (Entwurf VDI 3786 Blatt 13 S.27)
    fSD1 = (float)(6.11 * exp(17.269*(double)pWE->fTempMax
         /(237.3+(double)pWE->fTempMax)));
//    fSD1 = (float)(6.11 * exp(17.269*(double)pWE->fTempAve
//         /(237.3+(double)pWE->fTempAve)));
//    fSD1 = (float)(6.11 * exp(17.62*(double)pWE->fTempAve
//         /(243.12+(double)pWE->fTempAve)));
	fSD2 =((float)1.0 - pWE->fHumidity/(float) 100.0);
    fSatDef=fSD1*fSD2;

//  Verdunstung aus der Wasserschale (Kesselverdunstung)
	pWE->fPanEvap  = (float) 0.5 * fSatDef;//0.5 fuer Verdunstung aus bedeckter Wasserschale
	    
	if (pPl != NULL)
	{
    // plant pan factor or Haude factor	

	 fHaudeFac = (float)0.11; //Brache

        if (!lstrcmp(pPl->pGenotype->acCropCode,"WH\0")) //Winterweizen
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.17;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.24;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.33;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.41;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.37;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.28;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.15;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.11;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"BA\0")) //Wintergerste
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.17;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.24;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.37;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.38;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.32;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.22;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.15;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.11;
		}
		
		if (!lstrcmp(pPl->pGenotype->acCropCode,"MZ\0")) //Mais
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.17;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.21;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.24;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.25;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.26;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.21;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.18;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.11;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"OA\0")) //Hafer
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.15;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.34;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.44;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.45;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.30;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.19;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.11;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"RY\0")) //Roggen
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.17;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.23;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.30;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.36;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.36;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.27;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.15;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.11;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"SB\0")) //Zuckerruebe
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.15;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.23;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.30;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.37;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.33;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.26;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.20;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.11;
		}

		if ((!lstrcmp(pPl->pGenotype->acCropCode,"FW\0"))
			||(!lstrcmp(pPl->pGenotype->acCropCode,"SP\0"))) //Fichte
		{   
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.08;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.04;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.14;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.35;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.39;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.34;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.31;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.25;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.20;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.13;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.07;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.05;
		}

		if ((!lstrcmp(pPl->pGenotype->acCropCode,"BW\0"))
			||(!lstrcmp(pPl->pGenotype->acCropCode,"BE\0"))) //Buche
		{   
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fHaudeFac = (float)0.01;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.00;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.04;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.10;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.23;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.28;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.32;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.26;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.17;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.10;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.01;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.00;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"SY\0")) //Soja
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))

				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fHaudeFac = (float)0.17;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fHaudeFac = (float)0.28;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fHaudeFac = (float)0.37;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fHaudeFac = (float)0.28;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fHaudeFac = (float)0.24;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fHaudeFac = (float)0.11;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fHaudeFac = (float)0.11;
		}
      }
	  else
	  {
	   fHaudeFac = (float) 0.11; // Brache
	  }

     pWa->fPotETDay = fHaudeFac * fSatDef*(float)1.2; 
   }

  EvTeiler = (DeltaT < (float)0.11)? SolPartTime(pTi) : (float)1;
  pWa->fPotETdt = EvTeiler * pWa->fPotETDay;

  return  0;
}  /* Ende pot Evapotat.- Berechnung    */
//-----------------------------------------------------------------------------


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : potevapotraPenMonteith()                   */
/*							                              */
/*	nach FAO - Report 									  */
/*	Rome, Italy, 28-31 May 1990							  */
/*														  */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int WINAPI potevapotraPenMonteith(EXP_POINTER)
{
    DECLARE_COMMON_VAR
		
	PWEATHER  		pWE = pCl->pWeather;
	PEVAP     		pEV = pWa->pEvap;
	
	static float	fTempYesterday;
	float	fP, flamda, fgamma, fra, frc, fgammastern;
	float	fea_Tmax, fea_Tmin, fea, fed, fea_Tave;
	float   fDelta, fJ, fdr, fdelta, fphi, ftemp, fomegas;
	float	fRa, fN, fn, fRns, fRnl, fRn, fG;
	float   fETaero, fETrad;
	float   fas=(float)0.25, fbs=(float)0.5;
	float	EvTeiler;
	
	if(SimStart(pTi))
		fTempYesterday = (float)-99;
	
	if(NewDay(pTi))
	{
		fP     = (float)101.3;										//equation (6) const.
		flamda = (float)2.45;										//equation (2)
		fgamma = (float)0.00163*fP/flamda;							//equation (4)
		
		fra = (float)208/(pWE->fWindSpeed + (float)1e-8);           //equation (40) grass
		frc = (float)200/(float)2.88;                               //equation (35) grass
	    fgammastern = fgamma*((float)1+frc/fra);					//equation (41)

        if (pPl == NULL) 
        {    
        //frc=(float)200/((float)24*(float)0.05);
	    //fgammastern = fgamma*((float)1+frc/fra);					//equation (41)
	    fgammastern = fgamma;
        }
		else
		{
		if PlantIsGrowing //ep 260406
		{
		//frc = (float)200/(pPl->pCanopy->fLAI + (float)1e-8);		//equation (32)
		//frc = min(frc, (float)200/((float)24*(float)0.05));
	    //fgammastern = fgamma*((float)1+frc/fra);					//equation (41)
        fgammastern = fgamma*((float)1+(float)0.34*pWE->fWindSpeed);//equation (42) grass
        }
		else
		{    
        //frc=(float)200/((float)24*(float)0.05);
	    //fgammastern = fgamma*((float)1+frc/fra);					//equation (41)
	    fgammastern = fgamma;
        }// Plant is not growing 
		}//pPl != NULL

		fea_Tmax = (float)0.611*(float)exp((float)17.27*pWE->fTempMax / (pWE->fTempMax+(float)237.3));
		fea_Tmin = (float)0.611*(float)exp((float)17.27*pWE->fTempMin / (pWE->fTempMin+(float)237.3));
		                                                            //equation (10)
		
		fea = (float)0.5*(fea_Tmax + fea_Tmin);					    //equation (17)
		fed = (float)2.0*pWE->fHumidity*(float)0.01/( (float)1/fea_Tmin + (float)1/fea_Tmax );
		                                                            //equation (14)
		fea_Tave = (float)0.611*(float)exp((float)17.27*pWE->fTempAve / (pWE->fTempAve+(float)237.3));
		fDelta = (float)4098*fea_Tave/SQR(pWE->fTempAve+(float)237.3);//equation (3)
		pWE->fSatDef = fea - fed;
		//------------------------------------------------------------------------------------- 
		fETaero =      fgamma / (fDelta+fgammastern)
			* (float)900 / (pWE->fTempAve+(float)273) * pWE->fWindSpeed
			* (fea - fed);									          //equation (48) 
		//-------------------------------------------------------------------------------------
		
		
		fJ = (float)pTi->pSimTime->iJulianDay;
		
		fdr = (float)1 + (float)0.033*(float)cos((float)0.0172*fJ);	       //equation (21)
		
		fdelta = (float)0.409*(float)sin((float)0.0172*fJ - (float)1.39);  //equation (22)
		fphi   = pLo->pFarm->fLatitude*(float)PI/(float)180;								
		
		ftemp = -(float)tan(fphi)*(float)tan(fdelta);					   //equation (20)
		if(     ftemp <= (float)-1)	fomegas = (float)PI;
		else if(ftemp >= (float)1 )	fomegas = (float)0;
		else						fomegas = (float)acos(ftemp);
		
		fRa = (float)37.6*fdr*(fomegas*(float)sin(fphi)*(float)sin(fdelta) //equation (19)
			+ (float)cos(fphi)*(float)cos(fdelta)*(float)sin(fomegas));
		fN  = (float)24/(float)PI * fomegas;							   //equation (25)
		
        if(pWE->fSolRad >= (float)0)
		{
			// Berechnung der aktuellen Sonnenscheindauer //equation (52)
			if (fRa != (float)0) fn  = pWE->fSolRad/fRa;
			else fn = (float)0;
			fn -= fas;// a_s=0.25
			fn *= fN;
			fn /= fbs;// b_s=0.50
			fn = min(fN,fn);
			fn = max((float)0, fn);
			//fn = min((float)16,fn);
		    if (pWE->fSunDuration < (float)0)
               pWE->fSunDuration = fn; //Expert-N Variable
		 }
		 else
		 {
          if (pWE->fSunDuration >= (float)0)
		  {
		    // Berechnung der aktuellen Globalstrahlung //equation (52)
            fn = pWE->fSunDuration;
			pWE->fSolRad = fRa*(fn/fN * fbs + fas);
		  }
		 }//fSolRad>=0
		
		if (fN != (float)0)
		{
		    fRns = (float)0.77*(fas + fbs * fn/fN)*fRa; //equation (55) grass
		
		    fRnl = (float)2.45e-9 * ((float)0.9*fn/fN + (float)0.1)
			      * ((float)0.34 - (float)0.14*(float)sqrt(fed))
			      * ( (float)pow(273.0+pWE->fTempMax, 4.0) 
			         +(float)pow(273.0+pWE->fTempMin, 4.0)); //equation (63)
		}
		else
		{
         fRns = (float)0;
		 fRnl = (float)0;
		}
		
		fRn = fRns - fRnl;	//equation (50)
		
		fG = (float)0;
		if(fTempYesterday != (float)-99) 
			fG = (float)0.38*(pWE->fTempAve - fTempYesterday);	//equation (65)
		
		fTempYesterday = pWE->fTempAve;	//da alle Berechnungen nur tageweise 
                                        //ist kein if(EndDay) nötig
				
		fETrad = (float)0.408*fDelta*(fRn - fG)/(fDelta + fgammastern);//equation (49) 
				
		pWE->fPanEvap = fETaero + fETrad;			//equation (31)
		
        pWa->fPotETDay = pWE->fPanEvap;  
	}//NewDay
	
	
	//EvTeiler = SolPartTime(pTi);
	
	///*
	EvTeiler = (DeltaT < (float)0.11)? 
		SolPartTime(pTi)
		: (float)DeltaT;
	//*/

	pWa->fPotETdt = EvTeiler * pWE->fPanEvap;
	
	return  1;	
}



int WINAPI _loadds potevapotraPenman_SPASS(EXP_POINTER)
{
 double vGlobalRadiation,vLongRadIn,vLongRadOut,vRadMax,vNetRad,vDry;
 double vPotTr,vPotTrRad,vPotTrDry;
 double vSatVP,vSatVPave,vActVP,vSlope,vWind,vWindDay,vWindR;
 double vTempDay,vTempAve,vRH,vDaylen,vCa,vCi;
 double vLeafWidth,vPltHght,vLAI,vPSgross,vPn,vMaxPs,vMaintLv;
 double Rbv,Rcv,Rlv,Rlvm,Rlvx,Rch,Rbh,vZr,vZo,vZd;
 double vBoltzm,vPsych,vLH,vVHair,vAlbedo,vPsych1;
 float  EvTeiler;
 float  DeltaT = pTi->pTimeStep->fAct;
 
 PWEATHER	  pWth=pCl->pWeather;   
 PEVAP       pEvp = pWa->pEvap;
 PDEVELOP	  pDev=pPl->pDevelop; 
 PPLTCARBON	pPlC=pPl->pPltCarbon;

	
 if(NewDay(pTi))	
 {
    if (pCl->pWeather->fSolRad < (float)0)
    {
     SolRadSunDur(exp_p);
     if (!Test_Range(pCl->pWeather->fSolRad,(float)0,(float)40,"global radiation"))
       pCl->pWeather->fSolRad = (float)12;
    }

	vGlobalRadiation = (double)	pWth->fSolRad*1E6;
	vTempDay		 = (double)  ((float)0.71*pWth->fTempMax+(float)0.29*pWth->fTempMin);
	vTempAve		 = (double)  ((float)0.5*(pWth->fTempMax+pWth->fTempMin));
	vRH				 = (double)	pWth->fHumidity;
	vWind			 = (double)	pWth->fWindSpeed;
	vDaylen			 = (double) DaylengthAndPhotoperiod_SPASS(pLo->pFarm->fLatitude,
		                                                (int)pTi->pSimTime->iJulianDay,1);
	vRadMax 		 = (double) ExtraterrestrialRadiation_SPASS(pLo->pFarm->fLatitude, 
		                                                  (int)pTi->pSimTime->iJulianDay);

	if (vWind == -99.00) vWind=2.0;
	if (vRH   == -99.00) vRH=65.0;
	vWindR = max(0.2, 1.333*vWind);

  //______________________________________________________________________________
	//Constants
    vBoltzm = 4.8972E-3;//Stefan-Boltznan constant   (J /m2/d/K4)
    vPsych	= 0.67;		//Psychrometric instrument constant (mbar/C)	 
    vLH		= 2.4E6;	//Latent heat of evaporation of water (J/kg H2O)
    vVHair	= 1200;		//Volumetric heat capacity of the air (J/m2/K)
    vAlbedo = 0.25;     //Leaf albedo
	
	/* planted or nearly bare soil	*/
	if (pPl == NULL)
	{
	vLAI	= (double)0;
	vPsych1 = vPsych;
	Rbh     = (double)0;
	Rcv     = 0.74*pow(log((2 - 0.1)/0.05),2)/(0.16*vWindR);
	}
	else
	{
	vLAI	= (double)	pPl->pCanopy->fLAI;
	//______________________________________________________________________________
	//Boundary Layer Resistance                                 
	vWindDay = 1.333*max(0.2,vWind);
	vLeafWidth = 0.01; //m
	
	Rbv = 0.5*172*sqrt(vLeafWidth/(vWindDay*0.6)); 
	//______________________________________________________________________________
	//Canopy resistance
	vPltHght = (double)pPl->pGenotype->fPlantHeight*
		                 min(1.0, (double)pDev->fStageSUCROS)/100.0;
	
	vZd = max(0.10, 0.63*vPltHght);
	vZo = max(0.05, 0.10*vPltHght);
	vZr = max(2.00, 1.0+vPltHght);
	
	Rcv = 0.74*max(1.0,vLAI)*pow(log((vZr-vZd)/vZo),2)/(0.16*vWindR);
	//______________________________________________________________________________
	//Leaf resistance
	vCa 	= (double)CO2;
	vCi 	= vCa*(double)pPl->pGenotype->fCiCaRatio;
	vMaxPs 	= (double)pPl->pGenotype->fMaxGrossPs;
	vMaintLv= (double)pPlC->fMainRespRLeaf;
	vPSgross= (double)pPlC->fGrossPhotosynR;
	
	Rlvm = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(0.9*(vMaxPs+1E-9))/1.6-10.0;
	Rlvx = 2000.0; //s/m
	
	vPn = max(0.0, vPSgross/vDaylen - 0.25*(vMaintLv/0.75)/24.0);
	vPn = vPn/min(2.5, vLAI);
	Rlv = 70.714/(1+vTempDay/273.0)*(vCa-vCi)/(vPn+1E-9)/1.6-Rbv-Rcv;
	
	Rlv = (double)LIMIT((float)Rlvm,(float)Rlvx,(float)Rlv);
    //______________________________________________________________________________
    //Apparent Psychrometric constant (mbar/C)
	Rbh = Rbv/0.93;
	Rch = Rcv;
	
	vPsych1	= vPsych*(Rlv+Rbv+Rcv)/(Rbh+Rch);//Psychrometric instrument constant (mbar/C)	 


	} //else: planted or nearly bare soil
	//______________________________________________________________________________
	//Saturated and actural vapour pressure of the air (mbar)
	vSatVP = 0.1*6.11*exp(17.4*vTempDay/(vTempDay+239.0));
	vSlope = 4158.6*10*vSatVP/((vTempDay+239.0)*(vTempDay+239.0));

	vSatVPave = 0.1*6.11*exp(17.4*vTempAve/(vTempAve+239.0));
	vActVP = vSatVPave*vRH/100.0;
	//______________________________________________________________________________
	//Radiation Term 
	vLongRadIn =vBoltzm*pow((vTempDay+273.0),4)*(0.618+0.00365*sqrt(10*vActVP));
	vLongRadOut=vBoltzm*pow((vTempDay+273.0),4);
 
 	vNetRad = (1.0-vAlbedo)*vGlobalRadiation
		-(vLongRadOut-vLongRadIn)*(vGlobalRadiation/(0.75*vRadMax*1E6))*(vDaylen/24.0);

	vPotTrRad = vNetRad/vLH*(vSlope/(vSlope+vPsych1));
	//______________________________________________________________________________
	//Drying Term
	vDry = 10.0*(vSatVP-vActVP)*vVHair/(Rbh+Rch)*(vDaylen/24.0);
	vPotTrDry=vDry/vLH/(vSlope+vPsych1)*(24.0*3600.0);
	
	vPotTr  = vPotTrRad*(1.0-exp(-0.5*vLAI)) + vPotTrDry*min(2.5,vLAI);


    // daily potential evapotranspiration 
	pWa->fPotETDay = (float)vPotTrRad + (float)(vPotTrDry*(1.0+min(2.5,vLAI)));//max(1.0,min(2.5,vLAI)));

	// daily potential evaporation 
    // pEvp->fPotDay = pWa->fPotETDay - (float)vPotTr;
	pWth->fPanEvap = pWa->fPotETDay - (float)vPotTr; 
	// Missbraucht da pWa->pEvap->fPotDay
	// in balance.c zu Null gesetzt wird!!

 }// if NewDay

 // timestep potential (evapo-)transpiration 
 EvTeiler = (DeltaT < (float)0.11)? 
	 			SolPartTime(pTi)
	 			: (float)1;

 pWa->fPotETdt = EvTeiler * pWa->fPotETDay;


 // daily potential evaporation 
 //pEvp->fPotDay = pWa->fPotETDay - (float)vPotTr;
 pEvp->fPotDay = pWth->fPanEvap;
	
 // potential evaporation rate
 if(DeltaT > EPSILON)
 {           
  pEvp->fPotR = EvTeiler * pEvp->fPotDay/ DeltaT;// [mm/day]
  //pEvp->fPotR = EvTeiler * pWth->fPanEvap/ DeltaT;// [mm/day]
 }

return 1 ;
}


/***********************************************************************************************/
/* Funktion:       potevapotraCanada                                                           */
/* Beschreibung:   Berechnet pot. Evapotranspiration aus max. und min. Tagestemperatur         */
/*                 nach Baier and Robertson (1996) Can. J. Soil Sci. 76, 251-261: eq. 1        */
/*                                                                                             */
/* ep/gsf          8.02.99                                                                     */
/*                                                                                             */
/***********************************************************************************************/
/* veränd. Var.:		pWa->fPotETDay                                                         */
/*			            pWa->fPotETdt                                                          */
/*						pEV->fPotDay                                                           */
/*                      pEV->fPotR                                                             */
/*                                                                                             */
/***********************************************************************************************/                   

int WINAPI potevapotraCanada(EXP_POINTER)
{
 PWEATHER  		pWE = pCl->pWeather;
 PEVAP     		pEV = pWa->pEvap;

 float  fETPC;
 float  EvTeiler;
 float  DeltaT = pTi->pTimeStep->fAct;
 
 double dPi,dPiTheta,dLat,dDayF,dSolarC,dTheta;
 double dDec,dH1,dRadV,dQnot,dEQ1;

 dPi      = (double) 3.141592654;
 dPiTheta = (double)2 * dPi/(double)365;
 dLat	  = (double)pLo->pFarm->fLatitude * dPi /(double)180;
 dDayF    = (double)24 /((double)2 * dPi);            
 dSolarC  = (double)1.94 * (double) 60;
 dTheta	  =	(double)pTi->pSimTime->iJulianDay * dPiTheta;

     
 dDec  =   (double)0.3964 + (double)3.631 * sin(dTheta)
	     - (double)22.97 * cos(dTheta)
         + (double)0.03838 * sin((double)2 * dTheta)
         - (double)0.3885  * cos((double)2 * dTheta)  
         + (double)0.07659 * sin((double)3 * dTheta)
		 - (double)0.1587  * cos((double)3 * dTheta)  
         - (double)0.01021 * cos((double)4 * dTheta) ;     
 
 dDec  = dDec * dPi /(double)180;                 

 dH1   = acos((-sin(dLat)*sin(dDec))/(cos(dLat)*cos(dDec)));

 dRadV =   (double)1 - (double)0.0009464 * sin(dTheta)
	     - (double)0.01671 * cos(dTheta) 
	     - (double)0.0001489 * cos((double)2 * dTheta)
         - (double)0.00002917 * sin((double)3 * dTheta)
		 - (double)0.00003438 * cos((double)4 * dTheta) ;
 
 dQnot =   (double)2 * dSolarC * dDayF/(dRadV*dRadV)
	     * (sin(dLat) * sin(dDec) * dH1 + cos(dLat) * cos(dDec) * sin(dH1));
	 
 dEQ1  =   (((double)pWE->fTempMax * (double)1.8 + (double)32) * (double)0.928
	     + ((double)pWE->fTempMax - (double)pWE->fTempMin) * (double)1.6794
         + (double)0.0486 * dQnot - (double)87.03) * (double)0.08636 ;

 fETPC  = (float) max((double)0,dEQ1);	 
 

 /* daily potential evapotranspiration */
 pWa->fPotETDay = fETPC;

	


 /* timestep potential (evapo-)transpiration */
 EvTeiler = (DeltaT < (float)0.11)? 
	 		SolPartTime(pTi)
	 		: (float)1;
	 
 pWa->fPotETdt = EvTeiler * pWa->fPotETDay;


 return 1;

}



/***********************************************************************************************/
/* Funktion:       potevap1                                                                    */
/* Beschreibung:   Berechnet die potentielle Evaporations-Rate                                 */
/*                 und die taegliche potentielle Evaporation                                   */
/*                                                                                             */
/* Author:         ep/gsf   21.05.99 zusammengefasst aus alten Versionen                       */
/*                                                                                             */
/***********************************************************************************************/
/* veränd. Var.:		pEV->fPotR                                                             */
/*                      pEV->fPotDay                                                           */
/***********************************************************************************************/                  
int WINAPI potevap1(EXP_POINTER)
{
 PEVAP     		pEV = pWa->pEvap;

 extern float  fSCEff, fCanopyClosure;

 float	DeltaT  = pTi->pTimeStep->fAct;
 float  fBodBedGrad = (float)0;//ep 05.02.01
 /* Aufspaltung der Evapotranspiration in Evaporation und Transpiration */     

	 if (pPl == NULL)
	 {
		if(fSCEff < (float)0) fSCEff=(float)1.0;
		
		if(DeltaT > EPSILON)
		{           
		  pEV->fPotR = pWa->fPotETdt / DeltaT;    // [mm/day]
		  pEV->fPotR = ((float)1-pSo->fSoilCover*fSCEff)*pWa->fPotETdt / DeltaT;    // [mm/day]
		  //pEV->fPotR = abspower((float)1-pSo->fSoilCover,fSCEff)*pWa->fPotETdt / DeltaT;    // [mm/day]
		}
		 
		//pEV->fPotDay = pWa->fPotETDay;
		pEV->fPotDay = ((float)1-pSo->fSoilCover*fSCEff)*pWa->fPotETDay;
		//pEV->fPotDay = abspower((float)1-pSo->fSoilCover,fSCEff)*pWa->fPotETDay;
	 }
	 else
	 {	
	    //if ((pPl->pCanopy->fCropCoverFrac <= (float)0)&&(pPl->pCanopy->fLAI > EPSILON)) 
	    if (pPl->pCanopy->fCropCoverFrac <= (float)0) 
		{
	     fBodBedGrad = LAI2CropCover(pPl->pCanopy->fLAI);
	     //pPl->pCanopy->fCropCoverFrac = LAI2CropCover(pPl->pCanopy->fLAI);
	    }             
		else 
		{
		 fBodBedGrad = max((float)0,pPl->pCanopy->fCropCoverFrac);
		 //pPl->pCanopy->fCropCoverFrac = max((float)0,pPl->pCanopy->fCropCoverFrac);
		}

		if(fSCEff < (float)0) fSCEff=(float)1.0;
		if(fCanopyClosure < (float)0) fCanopyClosure=(float)1.0;
		//fBodBedGrad = fBodBedGrad + ((float)1 - fBodBedGrad) * pSo->fSoilCover;
		//fBodBedGrad = fBodBedGrad + ((float)1 - fBodBedGrad) * abspower(pSo->fSoilCover,fSCEff);
		//fBodBedGrad = (fBodBedGrad + ((float)1 - fBodBedGrad) * pSo->fSoilCover*fSCEff)*fCanopyClosure;
        if(iCropCover == 1)
		fBodBedGrad = fBodBedGrad*fCanopyClosure+((float)1-fBodBedGrad*fCanopyClosure)*pSo->fSoilCover*fSCEff;

		//fBodBedGrad = pPl->pCanopy->fCropCoverFrac*fCanopyClosure
		              //+((float)1-pPl->pCanopy->fCropCoverFrac*fCanopyClosure)*pSo->fSoilCover*fSCEff;

		if(DeltaT > EPSILON)
		{           
		 pEV->fPotR = pWa->fPotETdt * ((float)1 - min((float)1,fBodBedGrad))/DeltaT;// [mm/day]
		}
		 
        pEV->fPotDay = pWa->fPotETDay * ((float)1 - min((float)1,fBodBedGrad));
	 }
	//-------------ende pPl == NULL,else -------------------------

 return  1;

}  /* Ende Berechnung der potentiellen Evaporation     */




/***********************************************************************************************/
/* Funktion:       potevap2                                                                    */
/* Beschreibung:   Berechnet die potentielle Evaporations-Rate                                 */
/*                 und die taegliche potentielle Evaporation                                   */
/*                                                                                             */
/* Author:         ep/gsf   21.05.99 zusammengefasst aus alten Versionen                       */
/*                                                                                             */
/***********************************************************************************************/
/* veränd. Var.:		pEV->fPotR                                                             */
/*                      pEV->fPotDay                                                           */
/***********************************************************************************************/ 
int WINAPI potevap2(EXP_POINTER)
{
	int	    stadium;
  	float	temp,ev,LAI,f1;
    float	DeltaT = pTi->pTimeStep->fAct;

    if (NewDay(pTi))
    {
    temp = ((float)0.6 * pCl->pWeather->fTempMax)
            + ((float)0.4 * pCl->pWeather->fTempMin);

 /*Festlegung des Gesamtalbedos in Abhaengigkeit von der physiologischen Entwicklung 
   der Pflanze. Wenn es keine Pflanze bzw. kein Speicher fuer die Struktur Pflanze
   allokiert wurde, wird das Entwicklungsstadium mit 0 initialisiert. */

	if (pPl == NULL)
	{
		stadium = 0;
	    LAI = (float)0;
	}
	else
	{
		stadium = pPl->pDevelop->iStageCERES;
		LAI = pPl->pCanopy->fLAI;
	}

     pSo->fAlbedo = pSo->fSoilAlbedo;

     if (stadium <= 6)
     {
       if (stadium < 5)
       {
         pSo->fAlbedo = (float)0.23 - ((float)0.23 - pSo->fSoilAlbedo)
                        *(float)exp((double)((float)-0.75 * LAI));
		} // Ende if (pPl->Develop->iStageCERES < 5)
       else
       {
         pSo->fAlbedo = (float)0.23 +
                       ((LAI - (float) 4.0) *
                       (LAI - (float) 4.0)/(float)160.0);
       }// Ende else (pPl->Develop->iStageCERES < 5) 
	} // Ende if (pPl->Develop->iStageCERES <= 6)

 
     if (pCl->pWeather->fSnow > (float) 0.5)
     {
       pSo->fAlbedo = (float)0.6;
     } //Ende if (pCl->pWeather->fSnow > (float) 0.5) 


 	 /* Einfluss der Strahlung in Abhaengigkeit vom Gesamtalbedo    */
     ev =  max((float)0,pCl->pWeather->fSolRad) * ((float)4.88e-3 -((float)4.37e-3
     		 * pSo->fAlbedo)) * (temp + (float)29.0);

 
	/* Einfluss des Blattfaechenindexes: groesser 1; kleiner 1    */
     if (LAI > (float)1.0) //Blattflaechenindex groesser 1
     {
       f1 = ev * (float)exp((double)((float)-0.4 * LAI));
       pWa->pEvap->fPotR = (float)min((double)f1,(double)pWa->fPotETDay);
	 }  // Ende Blattflaechenindex groesser 1
     else  // Blattflaechenindex kleiner oder gleich 1
     {
       pWa->pEvap->fPotR = pWa->fPotETDay * 
        					((float)1.0 - ((float)0.43 * LAI));
     } // Ende Blattflaechenindex kleiner oder gleich 1

	pWa->pEvap->fPotDay = pWa->pEvap->fPotR;
                         
    } /* Ende NewDay */


  return 1;
}


/***********************************************************************************************/
/* Funktion:       potevap3                                                                    */
/* Beschreibung:   Berechnet die potentielle Evaporations-Rate                                 */
/*                 und die taegliche potentielle Evaporation                                   */
/*                 nach FAO - Report 56  Crop evapotranspiration with Dual Crop Coefficient    */
/*                                                                                             */
/* Author:         C.Loos/gsf   21.03.06                                                       */
/*                                                                                             */
/***********************************************************************************************/

int WINAPI potevap3(EXP_POINTER,float fCropMax, float fCropBasal)
{
 PEVAP     		pEV = pWa->pEvap;
 PWEATHER	    pWE = pCl->pWeather; 

float  DeltaT  = pTi->pTimeStep->fAct;
float  fPflBedGrad = (float)0;//ep 05.02.01

extern float  fSCEff, fCanopyClosure;

if(fSCEff < (float)0) fSCEff=(float)1.0;
if(fCanopyClosure < (float)0) fCanopyClosure=(float)1.0;

  /* Berechung der potentiellen Evaporation nach Expert-N Eq. 54
     pPl->pCanopy->fCropCoverFrac wird aus Datenbank eingelesen, wenn kein Wert wird Bedeckungsgrad
	 über LAI ausgerechnet; wenn gemulcht wird, steht (fBodBedGrad * pSo->fSoilCover*fSCEff) weniger
	 Fläche für Evaporation zur Verfügung */


	 if ((pPl != NULL)&&(PlantIsGrowing))
	 {	
	    if ((pPl->pCanopy->fCropCoverFrac <= (float)0)&&(pPl->pCanopy->fLAI > EPSILON)) 
		{
         fPflBedGrad = LAI2CropCover(pPl->pCanopy->fLAI);
	     //pPl->pCanopy->fCropCoverFrac = LAI2CropCover(pPl->pCanopy->fLAI);
	    }             
		else 
		{
         fPflBedGrad  = max((float)0,pPl->pCanopy->fCropCoverFrac);
		 //pPl->pCanopy->fCropCoverFrac = max((float)0,pPl->pCanopy->fCropCoverFrac);
		}

		fPflBedGrad = fPflBedGrad*fCanopyClosure; 
			         //+((float)1-fPflBedGrad*fCanopyClosure)*pSo->fSoilCover*fSCEff;

		//fPflBedGrad = pPl->pCanopy->fCropCoverFrac*fCanopyClosure; 
			         //+((float)1-pPl->pCanopy->fCropCoverFrac*fCanopyClosure)*pSo->fSoilCover*fSCEff;

		if((DeltaT > EPSILON)&& (fCropMax > EPSILON))
        pEV->fPotR = pWa->fPotETdt/fCropMax/DeltaT * ((float)1-pSo->fSoilCover*fSCEff)
		            * min((fCropMax - fCropBasal),((float)1 - fPflBedGrad)*fCropMax);
		//pEV->fPotR = pWa->fPotETdt * ((float)1.0 - fBodBedGrad)* fCropMax/DeltaT;
		if(fCropMax > EPSILON)
		pEV->fPotDay = pWa->fPotETDay/fCropMax * ((float)1-pSo->fSoilCover*fSCEff)
			          * min((fCropMax - fCropBasal),((float)1 - fPflBedGrad)*fCropMax);	
		//pEV->fPotDay = pWa->fPotETDay *((float)1.0 - fBodBedGrad)* fCropMax;

	 }
	 else
	 {
		if(DeltaT > EPSILON)
		{           
		  //pEV->fPotR = pWa->fPotETdt / DeltaT;    // [mm/day]
		  pEV->fPotR = ((float)1-pSo->fSoilCover*fSCEff)*pWa->fPotETdt / DeltaT;    // [mm/day]
		  //pEV->fPotR = abspower((float)1-pSo->fSoilCover,fSCEff)*pWa->fPotETdt / DeltaT;    // [mm/day]
		}
		 
		//pEV->fPotDay = pWa->fPotETDay;
		pEV->fPotDay = ((float)1-pSo->fSoilCover*fSCEff)*pWa->fPotETDay;
		//pEV->fPotDay = abspower((float)1-pSo->fSoilCover,fSCEff)*pWa->fPotETDay;
	 }

	//-------------ende pPl != NULL,else -------------------------

 return  1;

}  /* Ende Berechnung der potentiellen Evaporation mit dual crop coefficient */

/***********************************************************************************************/
/* Funktion:       potevapYin                                                                  */
/* Beschreibung:   Berechnet die potentielle Evaporations-Rate                                 */
/*                 und die taegliche potentielle Evaporation                                   */
/*                                                                                             */
/* Author:         ep/gsf   21.05.99 zusammengefasst aus alten Versionen                       */
/*                                                                                             */
/***********************************************************************************************/
/* veränd. Var.:		pEV->fPotR                                                             */
/*                      pEV->fPotDay                                                           */
/***********************************************************************************************/
extern double PotentialEvaporation_GECROS(double DAYTMP,double DVP,double RSS,double RTS,
										  double RBWS,double RBHS,double ATRJS,double ATMTR,
										  double PT1,double WSUP1, double *NRADS);

int WINAPI potevapYin(EXP_POINTER)
{
 PEVAP    pEV = pWa->pEvap;
 PWEATHER pWE = pCl->pWeather;       
 PWLAYER  pWL=pWa->pWLayer;

 int   iLayer;
 float DeltaT  = pTi->pTimeStep->fAct;
 //float fBodBedGrad = (float)0;//ep 05.02.01

 float fDAYTMP, fSVP, fDVP, fDVPTmax, fDVPTmin, fSLOPE, fVPD;
 float fRSS, fRTS, fRBWS, fRBHS, fATRJS, fATMTR;
 float fWND, fGR, fSC, fPSNIR;
 float fWCUL, fWSUP;
 //float fPE,fNRADS;
 double dPE,NRADS;
 //float fSLOPES, fPES, fAES;
 //float fDIFS, fTAVS, fSVPS;
 float fLAI;


 float fpl  = (float)25;// plow layer 25 cm deep
 float fWUL = (float)0;// water content upper layer
 float fWLL = (float)0;// water content upper layer
 float f1;
 float DeltaZ = pSo->fDeltaZ;

    for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL,iLayer<=pSo->iLayers-2;pWL=pWL->pNext,iLayer++)    
    {
     if (iLayer*DeltaZ <= fpl*(float)10.) 
	    fWUL += pWL->fContAct* DeltaZ;
	 else if ((fpl*(float)10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= fpl*(float)10. + DeltaZ))
     {
        f1 = (iLayer*DeltaZ-fpl*(float)10.)/DeltaZ;
		fWUL += ((float)1-f1)*pWL->fContAct*DeltaZ;
		fWLL += f1*pWL->fContAct*DeltaZ;
     }
	 else
		//pWL->fContAct=(float)(WLL/NOTNUL((double)fProfileDepth-RD*10.));
		fWLL += pWL->fContAct*DeltaZ;

     }//for
	  
 fWSUP = max((float)0.1,fWUL+(float)0.1);

 
 if (NewDay(pTi))
 {	 
   fDAYTMP = pWE->fTempAve;
   fWND  = pCl->pWeather->fWindSpeed;  //[m s-1]
   //fWCUL   = pWa->pWLayer->pNext->fContAct;
   //fWSUP = (float)1;//DWSUP;//-> to do !!! ep20071016
   fWCUL   = fWSUP/fpl;

   fGR     = pWE->fSolRad*(float)1E6;
   fSC     = (float)1367*((float)1.+(float)0.033*(float)cos(2.0*PI*
	                                      ((double)pTi->pSimTime->iJulianDay-10.)/365.0));

   fDVPTmax = (float)0.611*(float)exp((float)17.4*pWE->fTempMax/(pWE->fTempMax+(float)239));
   fDVPTmin = (float)0.611*(float)exp((float)17.4*pWE->fTempMin/(pWE->fTempMin+(float)239));
   fDVP     = (float)0.5*(fDVPTmax + fDVPTmin);
   fSVP     = (float)0.611*(float)exp((float)17.4*pWE->fTempAve/(pWE->fTempAve+(float)239));
   fVPD     = max(0,fSVP-fDVP);
   fSLOPE   = (float)4158.6 * fSVP/((fDAYTMP+(float)239.)*(fDAYTMP+(float)239.));

   fRSS  = (float)100;//[s m-1]
   fRTS  = (float)0.74*(float)pow(log((double)56.),(double)2)/((float)pow((double)0.4,(double)2)*fWND);
   
   
   if ((pPl != NULL)&& (PlantIsGrowing))
   {
     fLAI  = pPl->pCanopy->fPlantLA;     //[m2 m-2]
     //fKW   = kdiff(fTLAI,BLD*3.141592654/180.,0.2);
	 pWa->fPotETDay = pEV->fPotDay + fPotTraDay;
   }
   else
   {   
	 fRBWS = (float)172.* (float)sqrt(0.05/max(0.1,(double)fWND));
     fRBHS = (float)0.93* fRBWS;
     fPSNIR= (float)INSW((double)fWCUL-(double)0.5,(double)0.52-(double)0.68*(double)fWCUL,(double)0.18);
     fATRJS= ((float)0.9 + ((float)1-fPSNIR))*(float)0.5*fGR;
     fATMTR= fGR/fSC;


	 dPE = PotentialEvaporation_GECROS((double)fDAYTMP,(double)fDVP,(double)fRSS,(double)fRTS,
		                               (double)fRBWS,(double)fRBHS,(double)fATRJS,(double)fATMTR,
									   (double)1,(double)fWSUP, &NRADS);
 
     //PTran(fRSS,fRTS,fRBWS,fRBHS,fATRJS,fATMTR,(float)1.,fDAYTMP,fDVP,fSLOPE,fVPD,&fPE,&fNRADS);

	 pEV->fPotDay = (float)max(0,dPE);
	 
	 pWa->fPotETDay = pEV->fPotDay;
   }
 }//if NewDay

 pWa->fPotETdt=pWa->fPotETDay*DeltaT;	
 	
 pEV->fPotR = pEV->fPotDay; // [mm/day];


 return  1;

}  /* Ende Berechnung der potentiellen Evaporation     */



 int WINAPI PTran(float RSW,float RT,float RBW,float RBH,float ATRJ,float ATMTR,float FRAC,
                   float TLEAF,float DVP,float SLOPE,float VPD,float *PT,float *NRADC)
      { 
	   float CLEAR, BBRAD, RLWN;
       float PSR, PTR, PTD;

        //*---some physical constants
       float LHVAP  = (float)2.4E+6;    //latent heat of water vaporization(J/kg)
       float VHCA   = (float)1200.;     //volumetric heat capacity (J/m3/oC)
       float PSYCH  = (float)0.067;     //psychrometric constant (kPa/oC)        
       float BOLTZM = (float)5.668E-8;  //Stefan-Boltzmann constant(J/m2/s/K4)  

        //*---net absorbed radiation
        CLEAR  = max((float)0., min((float)1., (ATMTR-(float)0.25)/(float)0.45));//sky clearness
        BBRAD  = BOLTZM*(float)pow((double)TLEAF +(double)273.,(double)4);
        RLWN   = BBRAD*((float)0.56-(float)0.079*(float)sqrt(DVP*(float)10.))*((float)0.1
                 +(float)0.9*CLEAR)*FRAC;
        *NRADC = ATRJ - RLWN;

        //*---intermediate variable related to resistances
        PSR    = PSYCH*(RBW+RT+RSW)/(RBH+RT);

        //*---radiation-determined term
        PTR    = *NRADC*SLOPE        /(SLOPE+PSR)/LHVAP;

        //*---vapour pressure-determined term
        PTD    = (VHCA*VPD/(RBH+RT))/(SLOPE+PSR)/LHVAP;

        //*---potential evaporation or transpiration
        *PT    = max((float)1.E-10,PTR+PTD);

       return 1;
      }




/***********************************************************************************************/
/* Funktion:       CropCoeff                                                                   */
/* Beschreibung:   Berechnet Bestandeskoeffzienten                                             */
/*                 nach DVWK 1996  Crop Evapotranspiration with  Crop Coefficient              */
/*                                                                                             */
/* Author:         C.Loos/gsf   21.03.06                                                       */
/*                                                                                             */
/***********************************************************************************************/
float WINAPI CropCoeff(EXP_POINTER)
{

 DECLARE_COMMON_VAR

 PWEATHER  		pWE = pCl->pWeather;
 PEVAP     		pEV = pWa->pEvap;
 
 float fCropCoeff = (float)0.1; //Brache

 	if ((pPl != NULL)&&(PlantIsGrowing))
	{
        if (!lstrcmp(pPl->pGenotype->acCropCode,"WH\0")) //Winterweizen
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fCropCoeff = (float)0.9;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fCropCoeff = (float)1;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fCropCoeff = (float)1.15;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fCropCoeff = (float)1.35;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fCropCoeff = (float)1.3;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fCropCoeff = (float)1;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fCropCoeff = (float)0.65;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"BA\0")) //Wintergerste
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fCropCoeff = (float)0.95;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fCropCoeff = (float)1;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fCropCoeff = (float)1.3;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fCropCoeff = (float)1.4;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fCropCoeff = (float)1.35;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fCropCoeff = (float)1;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fCropCoeff = (float)0.65;
		}
		
		if (!lstrcmp(pPl->pGenotype->acCropCode,"MZ\0")) //Mais
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fCropCoeff = (float)0.8;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fCropCoeff = (float)1.2;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fCropCoeff = (float)1.4;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fCropCoeff = (float)1.4;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fCropCoeff = (float)1.0;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fCropCoeff = (float)0.1;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"OA\0")) //Hafer
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fCropCoeff = (float)0.7;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fCropCoeff = (float)1.1;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fCropCoeff = (float)1.4;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fCropCoeff = (float)1.35;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fCropCoeff = (float)0.95;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fCropCoeff = (float)0.1;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"RY\0")) //Roggen
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fCropCoeff = (float)0.85;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fCropCoeff = (float)0.9;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fCropCoeff = (float)1.2;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fCropCoeff = (float)1.3;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fCropCoeff = (float)1.25;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fCropCoeff = (float)0.95;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fCropCoeff = (float)0.65;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fCropCoeff = (float)0.65;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"SB\0")) //Zuckerruebe
		{
		    if((pTi->pSimTime->iJulianDay > 0)&&(pTi->pSimTime->iJulianDay <= 31))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 31)&&(pTi->pSimTime->iJulianDay <= 59))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 59)&&(pTi->pSimTime->iJulianDay <= 90))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 90)&&(pTi->pSimTime->iJulianDay <= 120))
				fCropCoeff = (float)0.5;
		    if((pTi->pSimTime->iJulianDay > 120)&&(pTi->pSimTime->iJulianDay <= 151))
				fCropCoeff = (float)0.75;
		    if((pTi->pSimTime->iJulianDay > 151)&&(pTi->pSimTime->iJulianDay <= 181))
				fCropCoeff = (float)1.1;
		    if((pTi->pSimTime->iJulianDay > 181)&&(pTi->pSimTime->iJulianDay <= 212))
				fCropCoeff = (float)1.3;
		    if((pTi->pSimTime->iJulianDay > 212)&&(pTi->pSimTime->iJulianDay <= 243))
				fCropCoeff = (float)1.25;
		    if((pTi->pSimTime->iJulianDay > 243)&&(pTi->pSimTime->iJulianDay <= 273))
				fCropCoeff = (float)1.1;
		    if((pTi->pSimTime->iJulianDay > 273)&&(pTi->pSimTime->iJulianDay <= 304))
				fCropCoeff = (float)0.85;
		    if((pTi->pSimTime->iJulianDay > 304)&&(pTi->pSimTime->iJulianDay <= 334))
				fCropCoeff = (float)0.1;
		    if((pTi->pSimTime->iJulianDay > 334)&&(pTi->pSimTime->iJulianDay <= 366))
				fCropCoeff = (float)0.1;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"BW\0"))
		{
		fCropCoeff = pWa->pEvap->fCropBasalIni;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"FW\0"))
		{
		fCropCoeff = pWa->pEvap->fCropBasalIni;
		}

      }
	  else
	  {
	   fCropCoeff = (float) 0.1; // Brache
	  }

	  if (pPl != NULL)
	  {
	   if (!lstrcmp(pPl->pGenotype->acCropCode,"BW\0"))
		{
		fCropCoeff = pWa->pEvap->fCropBasalIni;
		}

		if (!lstrcmp(pPl->pGenotype->acCropCode,"FW\0"))
		{
		fCropCoeff = pWa->pEvap->fCropBasalIni;
		}

	  }

 return fCropCoeff;

}//Ende CropCoeff


/***********************************************************************************************/
/* Funktion:       DualCropCoeff                                                               */
/* Beschreibung:   Berechnet Bestandeskoeffzienten                                             */
/*                 nach FAO - Report 56  Crop Evapotranspiration with Dual Crop Coefficient    */
/*                                                                                             */
/* Author:         C.Loos/gsf   21.03.06                                                       */
/*                                                                                             */
/***********************************************************************************************/
int WINAPI DualCropCoeff(EXP_POINTER,float *fCropCoeffMax,float *fCropCoeffBase)
{
 PEVAP     		pEV = pWa->pEvap;
 PWEATHER	    pWE = pCl->pWeather; 

float fKappa;
float fCropMax,fCropBasal;
//float fCBI, fCBM, fCBE;
float fStageIni, fStageDev, fStageMid, fStageEnd;
float fMinRelHumidity;

float  DeltaT  = pTi->pTimeStep->fAct;

fCropMax=(float)1;
fCropBasal=(float)1;

fCropBasalIni = pWa->pEvap->fCropBasalIni;
fCropBasalMid = pWa->pEvap->fCropBasalMid;
fCropBasalEnd = pWa->pEvap->fCropBasalEnd;
fCropTimeIni  = pWa->pEvap->fCropTimeIni;
fCropTimeMid  = pWa->pEvap->fCropTimeMid;
fCropTimeEnd  = pWa->pEvap->fCropTimeEnd;
fCropTimeDev  = pWa->pEvap->fCropTimeDev;


if (pPl != NULL)
{
 if (iDualCropFactor<(int)2)
 {
    
/*	 Werte für Entwicklungszeit auf Basis von LAI und Development Stages aus Pflanzenwachstumsmodellen*/

	 fStageIni = (float)0.25; // wenn LAI = 0.25 bzw. SoilCover = 10.6 %
	 fStageDev = (float)3.0;  // wenn LAI = 3 bzw. SoilCover = 74 %
	 fStageMid = pPl->pGenotype->fBeginSenesDvs;  // wenn fStageSUCROS = start of senescence	
     fStageEnd = (float)2.0;   // wenn fStageSUCROS = harvest	

 /* Berechnung der Basalen Crop Coefficients nach Entwicklungsstadien der Pflanze; Werte für Ini,Mid,End nach 
    FAO; */
 }

  if (iDualCropFactor==(int)0)
  {
     fCropBasalIni = (float)0.15;

     if((!lstrcmp(pPl->pGenotype->acCropCode,"WH\0"))
	  ||(!lstrcmp(pPl->pGenotype->acCropCode,"BA\0"))
	  ||(!lstrcmp(pPl->pGenotype->acCropCode,"OA\0"))
      ||(!lstrcmp(pPl->pGenotype->acCropCode,"MZ\0"))) //Winterweizen,Wintergerste,Hafer,Mais
	    {	
		   fCropBasalMid = (float)1.10;
		   fCropBasalEnd = (float)0.15;		   		   
		}
     else if (!lstrcmp(pPl->pGenotype->acCropCode,"SB\0")) //Zuckerruebe
		{
		   fCropBasalMid = (float)1.15;
		   fCropBasalEnd = (float)0.90;
		}

     else if (!lstrcmp(pPl->pGenotype->acCropCode,"PT\0"))  //Kartoffel
		{
		   fCropBasalMid = (float)1.10;
		   fCropBasalEnd = (float)0.65;
		}

     else if (!lstrcmp(pPl->pGenotype->acCropCode,"SF\0")) //Sonnenblumen       
        { 
           fCropBasalMid = (float)1.10;
		   fCropBasalEnd = (float)0.25;
		}

     else if (!lstrcmp(pPl->pGenotype->acCropCode,"SY\0")) //Soja
		{
           fCropBasalMid = (float)1.10;
		   fCropBasalEnd = (float)0.30;
		}
     else
       {
		   fCropBasalMid = (float)1.10;
		   fCropBasalEnd = (float)0.15;
	   }
   }


   fMinRelHumidity = (float)100.0 * ((float)exp((float)17.269 * pWE->fTempMin/
                     ((float)237.3 + pWE->fTempMin))/ (float)exp((float)17.269 * pWE->fTempMax/
                     ((float)237.3 + pWE->fTempMax)));

   fCropBasalMid += ((float)0.04 * (pWE->fWindSpeed -(float)2.0) - 
		             (float)0.004 * (fMinRelHumidity -(float)45.0)) *			   
			         (float)pow((pPl->pCanopy->fPlantHeight/(float)3.0),(float)0.3);

   fCropBasalEnd += ((float)0.04 * (pWE->fWindSpeed -(float)2.0) - 
		             (float)0.004 * (fMinRelHumidity -(float)45.0)) *			   
			         (float)pow((pPl->pCanopy->fPlantHeight/(float)3.0),(float)0.3);
     
   /* Nach Allen (2000)soll die Abgleichung für fCropBasalMid und End (siehe oben) für Werte von
   Wind 1-6 m/s und MinRelHumidity von 20-80 % durchgeführt werden; hier wird keine Einschränkung 
   durchgeführt, deshalb ist es nötig v.a. fCropBasalEnd größer 0 zu setzen, da wenn Wind < 1 und
   RelHum > 80 -> fCropBasalEnd < 0 */

   if(fCropBasalMid < (float)0.0)
      fCropBasalMid = (float)0.0;
       
   if(fCropBasalEnd < (float)0.0)
	  fCropBasalEnd = (float)0.0;

   if (iDualCropFactor<(int)2)
   {
    if(pPl->pCanopy->fLAI < fStageIni)
		   fCropBasal = fCropBasalIni;

	else if((pPl->pCanopy->fLAI >= fStageIni) && (pPl->pCanopy->fLAI < fStageDev))
		   fCropBasal = fCropBasalIni + (fCropBasalMid - fCropBasalIni) * 
		               (pPl->pCanopy->fLAI - fStageIni)/(fStageDev - fStageIni);

	else if((pPl->pCanopy->fLAI >= fStageDev) && (pPl->pDevelop->fStageSUCROS < fStageMid))
		   fCropBasal = fCropBasalMid;

	else if((pPl->pDevelop->fStageSUCROS >= fStageMid) && (pPl->pDevelop->fStageSUCROS <= fStageEnd))
		   fCropBasal = fCropBasalMid + (fCropBasalEnd - fCropBasalMid) * 
		                (pPl->pDevelop->fStageSUCROS - fStageMid)/(fStageEnd - fStageMid);
   }//end (iDualCropFactor<2)
   else
   {
    if((float)pTi->pSimTime->iJulianDay < fCropTimeIni)
		   fCropBasal = fCropBasalIni;

	else if(((float)pTi->pSimTime->iJulianDay >= fCropTimeIni)&&((float)pTi->pSimTime->iJulianDay < fCropTimeDev))
		   fCropBasal = fCropBasalIni + (fCropBasalMid - fCropBasalIni) * 
		               ((float)pTi->pSimTime->iJulianDay - fCropTimeIni)/(fCropTimeDev - fCropTimeIni);

	else if(((float)pTi->pSimTime->iJulianDay >= fCropTimeDev)&&((float)pTi->pSimTime->iJulianDay < fCropTimeMid))
		   fCropBasal = fCropBasalMid;

	else if(((float)pTi->pSimTime->iJulianDay >= fCropTimeMid)&&((float)pTi->pSimTime->iJulianDay <= fCropTimeEnd))
		   fCropBasal = fCropBasalMid + (fCropBasalEnd - fCropBasalMid) * 
		                ((float)pTi->pSimTime->iJulianDay - fCropTimeMid)/(fCropTimeEnd - fCropTimeMid);

   }//end (iDualCropFactor>=2)


   
 
 fKappa = (float) 1.2; // bisher nur ein Wert, eigentlich Fallunterscheidung  
	                       // vorgesehen (1.1 für Regenhäufigkeit < 4 Tage)

/* oberes Limit für Evaporation und Transpiration auf Grund der Energie-Bilanz-Differenz */
 fCropMax = max((fKappa + ((float)0.04 * (pWE->fWindSpeed -(float)2.0) - 
		       (float)0.004 * (fMinRelHumidity -(float)45.0)) *			   
			   (float)pow((pPl->pCanopy->fPlantHeight/(float)3.0),(float)0.3)),
			   (fCropBasal +(float)0.05));

 } //End If(pPl != NULL)

 *fCropCoeffMax=fCropMax;
 *fCropCoeffBase=fCropBasal;

 return  1;
}// end DualCropCoeff

/***********************************************************************************************/
/* Funktion:       CropCoeff                                                                   */
/* Beschreibung:   Berechnet Bestandeskoeffzienten                                             */
/*                 nach FAO - Report 56  Crop Evapotranspiration                               */
/*                                                                                             */
/* Author:         C.Loos/gsf    21.03.06                                                      */
/* Modification:   S.Gayler/UHoh 05.08.16                                                      */
/*                                                                                             */
/***********************************************************************************************/
int WINAPI SingleCropCoeff(EXP_POINTER,float *fCropCoeffMax,float *fCropCoeffBase)
{
 PEVAP     		pEV = pWa->pEvap;
 PWEATHER	    pWE = pCl->pWeather; 

float fKappa;
//float fCropMax,fCropBasal;
//float fCBI, fCBM, fCBE;
float fStageIni, fStageDev, fStageMid, fStageEnd;
float fMinRelHumidity;

float  DeltaT  = pTi->pTimeStep->fAct;

//fCropMax=(float)1;
//fCropBasal=(float)1;

static float fCropMax= (float)1.0;
static float fCropBasal= (float)0.15;


fCropBasalIni = pWa->pEvap->fCropBasalIni;
fCropBasalMid = pWa->pEvap->fCropBasalMid;
fCropBasalEnd = pWa->pEvap->fCropBasalEnd;
fCropTimeIni  = pWa->pEvap->fCropTimeIni;
fCropTimeMid  = pWa->pEvap->fCropTimeMid;
fCropTimeEnd  = pWa->pEvap->fCropTimeEnd;
fCropTimeDev  = pWa->pEvap->fCropTimeDev;


if (pPl != NULL)
{
 if (iDualCropFactor<(int)2)
 {
    
/*	 Werte für Entwicklungszeit auf Basis Development Stages aus Pflanzenwachstumsmodellen*/

	 fStageIni = (float)0.1; // wenn fStageSUCROS = 0.1
	 fStageDev = (float)0.5;  // wenn fStageSUCROS = 0.5 
	 fStageMid = pPl->pGenotype->fBeginSenesDvs;  // wenn fStageSUCROS = start of senescence	
     fStageEnd = (float)2.1;   // wenn fStageSUCROS = harvest	

 /* Berechnung der Basalen Crop Coefficients nach Entwicklungsstadien der Pflanze; Werte für Ini,Mid,End nach 
    FAO; */
 }

  if (iDualCropFactor==(int)0)
  { //FAO 56 Table 12
     fCropBasalIni = (float)0.15;

     if((!lstrcmp(pPl->pGenotype->acCropCode,"WH\0"))
	  ||(!lstrcmp(pPl->pGenotype->acCropCode,"BA\0"))
	  ||(!lstrcmp(pPl->pGenotype->acCropCode,"OA\0"))) //Winterweizen,Wintergerste,Hafer,Mais
	    {	
           fCropBasalIni = (float)0.3;
		   fCropBasalMid = (float)1.15;
		   fCropBasalEnd = (float)0.25;		   		   
		}
	 else if (!lstrcmp(pPl->pGenotype->acCropCode,"MZ\0"))
	 {
           fCropBasalIni = (float)0.3;
		   fCropBasalMid = (float)1.20;
		   fCropBasalEnd = (float)0.5;		   		   
	 }
     else if (!lstrcmp(pPl->pGenotype->acCropCode,"SB\0")) //Zuckerruebe
		{
           fCropBasalIni = (float)0.35;
		   fCropBasalMid = (float)1.20;
		   fCropBasalEnd = (float)0.70;
		}

     else if (!lstrcmp(pPl->pGenotype->acCropCode,"PT\0"))  //Kartoffel
		{
           fCropBasalIni = (float)0.5;
		   fCropBasalMid = (float)1.15;
		   fCropBasalEnd = (float)0.75;
		}

     else if (!lstrcmp(pPl->pGenotype->acCropCode,"SF\0")) //Sonnenblumen       
        { 
           fCropBasalIni = (float)0.35;
           fCropBasalMid = (float)1.10;
		   fCropBasalEnd = (float)0.35;
		}

     else if (!lstrcmp(pPl->pGenotype->acCropCode,"SY\0")) //Soja
		{
           fCropBasalIni = (float)0.4;
           fCropBasalMid = (float)1.15;
		   fCropBasalEnd = (float)0.50;
		}
     else
       {
           fCropBasalIni = (float)0.3;
		   fCropBasalMid = (float)1.10;
		   fCropBasalEnd = (float)0.25;
	   }
   }


  fMinRelHumidity = (float)100.0 * ((float)exp((float)17.269 * pWE->fTempMin/
                     ((float)237.3 + pWE->fTempMin))/ (float)exp((float)17.269 * pWE->fTempMax/
                     ((float)237.3 + pWE->fTempMax)));
/* 
   fCropBasalMid += ((float)0.04 * (pWE->fWindSpeed -(float)2.0) - 
		             (float)0.004 * (fMinRelHumidity -(float)45.0)) *			   
			         (float)pow((pPl->pCanopy->fPlantHeight/(float)3.0),(float)0.3);

   fCropBasalEnd += ((float)0.04 * (pWE->fWindSpeed -(float)2.0) - 
		             (float)0.004 * (fMinRelHumidity -(float)45.0)) *			   
			         (float)pow((pPl->pCanopy->fPlantHeight/(float)3.0),(float)0.3);
 */    
   /* Nach Allen (2000)soll die Abgleichung für fCropBasalMid und End (siehe oben) für Werte von
   Wind 1-6 m/s und MinRelHumidity von 20-80 % durchgeführt werden; hier wird keine Einschränkung 
   durchgeführt, deshalb ist es nötig v.a. fCropBasalEnd größer 0 zu setzen, da wenn Wind < 1 und
   RelHum > 80 -> fCropBasalEnd < 0 */

   if(fCropBasalMid < (float)0.0)
      fCropBasalMid = (float)0.0;
       
   if(fCropBasalEnd < (float)0.0)
	  fCropBasalEnd = (float)0.0;

   if (iDualCropFactor<(int)2)
   {
    if(pPl->pDevelop->fStageSUCROS < fStageIni)
		   fCropBasal = fCropBasalIni;

	else if((pPl->pDevelop->fStageSUCROS >= fStageIni) && (pPl->pDevelop->fStageSUCROS < fStageDev))
		   fCropBasal = fCropBasalIni + (fCropBasalMid - fCropBasalIni) * 
		               (pPl->pDevelop->fStageSUCROS - fStageIni)/(fStageDev - fStageIni);

	else if((pPl->pDevelop->fStageSUCROS >= fStageDev) && (pPl->pDevelop->fStageSUCROS < fStageMid))
		   fCropBasal = fCropBasalMid;

	else if((pPl->pDevelop->fStageSUCROS >= fStageMid) && (pPl->pDevelop->fStageSUCROS <= fStageEnd))
		   fCropBasal = fCropBasalMid + (fCropBasalEnd - fCropBasalMid) * 
		                (pPl->pDevelop->fStageSUCROS - fStageMid)/(fStageEnd - fStageMid);
   }//end (iDualCropFactor<2)
   else if (iDualCropFactor == (int)2)
   {
    if((float)pTi->pSimTime->iJulianDay < fCropTimeIni)
		   fCropBasal = fCropBasalIni;

	else if(((float)pTi->pSimTime->iJulianDay >= fCropTimeIni)&&((float)pTi->pSimTime->iJulianDay < fCropTimeDev))
		   fCropBasal = fCropBasalIni + (fCropBasalMid - fCropBasalIni) * 
		               ((float)pTi->pSimTime->iJulianDay - fCropTimeIni)/(fCropTimeDev - fCropTimeIni);

	else if(((float)pTi->pSimTime->iJulianDay >= fCropTimeDev)&&((float)pTi->pSimTime->iJulianDay < fCropTimeMid))
		   fCropBasal = fCropBasalMid;

	else if(((float)pTi->pSimTime->iJulianDay >= fCropTimeMid)&&((float)pTi->pSimTime->iJulianDay <= fCropTimeEnd))
		   fCropBasal = fCropBasalMid + (fCropBasalEnd - fCropBasalMid) * 
		                ((float)pTi->pSimTime->iJulianDay - fCropTimeMid)/(fCropTimeEnd - fCropTimeMid);

   }//end (iDualCropFactor==2)
   else
   {    
	   if(NewDay(pTi))
	   {
		   if(pPl->pDevelop->fStageSUCROS < fCropTimeIni)
			   fCropBasal = fCropBasalIni;

			else if((pPl->pDevelop->fStageSUCROS >= fCropTimeIni)&&(pPl->pDevelop->fStageSUCROS < fCropTimeDev))
				   fCropBasal = fCropBasalIni + (fCropBasalMid - fCropBasalIni) * 
							   (pPl->pDevelop->fStageSUCROS - fCropTimeIni)/(fCropTimeDev - fCropTimeIni);

			else if((pPl->pDevelop->fStageSUCROS >= fCropTimeDev)&&(pPl->pDevelop->fStageSUCROS < fCropTimeMid))
				   fCropBasal = fCropBasalMid;

			else if((pPl->pDevelop->fStageSUCROS >= fCropTimeMid)&&(pPl->pDevelop->fStageSUCROS <= fCropTimeEnd))
				   fCropBasal = fCropBasalMid + (fCropBasalEnd - fCropBasalMid) * 
								(pPl->pDevelop->fStageSUCROS - fCropTimeMid)/(fCropTimeEnd - fCropTimeMid);
	   }
	}//end (iDualCropFactor>=3)


   
 
 fKappa = (float) 1.2; // bisher nur ein Wert, eigentlich Fallunterscheidung  
	                       // vorgesehen (1.1 für Regenhäufigkeit < 4 Tage)

/* oberes Limit für Evaporation und Transpiration auf Grund der Energie-Bilanz-Differenz */
 fCropMax = max((fKappa + ((float)0.04 * (pWE->fWindSpeed -(float)2.0) - 
		       (float)0.004 * (fMinRelHumidity -(float)45.0)) *			   
			   (float)pow((pPl->pCanopy->fPlantHeight/(float)3.0),(float)0.3)),
			   (fCropBasal +(float)0.05));

 } //End If(pPl != NULL)

 *fCropCoeffMax=fCropMax;
 *fCropCoeffBase=fCropBasal;

 return  1;
}// end SingleCropCoeff


/**********************************************************************************************/
/*                                                                                            */
/*  Name     : Interception                                                                   */
/*                                                                                            */
/*             0) Konstanter Anteil der protentiellen Tranpisration                           */
/*             1) Rutter Modell                                                               */
/*             2) Gash Modell                                                                 */
/*             3) Modell nach Hoyningen-Huene und Braden                                      */
/*                                                                                            */
/*                                                                                            */
/*  Autor    : ep/gsf                                                                         */
/*  Datum    : 16.3.06                                                                        */
/*                                                                                            */
/**********************************************************************************************/

int   iInterceptionModel;
float fIceptStor,fRelIntercept;
float fIceptPar,fIceptPar1,fIceptPar2,fIceptPar3,fIceptPar4,fIceptPar5;
RESPONSE fIceptPar0DC[21],fIceptPar1DC[21],fIceptPar2DC[21],
          fIceptPar3DC[21],fIceptPar4DC[21],fIceptPar5DC[21];

int WINAPI Interception(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 PPLTWATER   pPW;
 PEVAP       pEV;
 PCANOPY     pPC;
 PWBALANCE   pWB; 

 float fPotTr;
 float fPsat, fPrec, fEvap, fDrain, fIcept, fLAI, fPlcv;
 float fTime;
 int   iYears;
             
 if (pPl == NULL) return -1;     
  
 pPW = pPl->pPltWater;         
 pPC = pPl->pCanopy;         
 pEV = pWa->pEvap;
 pWB = pWa->pWBalance;

 fPrec  = pWB->fReservoir;
 fEvap  = (float)0;
 fDrain = (float)0;
 fIcept = (float)0;

 fPotTr = max(0,pWa->fPotETDay - pWa->pEvap->fPotDay);
 iYears  = (int)(pTi->pSimTime->fTimeAct/(float)365.2525);
 fTime  = pTi->pSimTime->fTimeAct-(float)iYears*(float)365.2525;

 fIceptPar1 = AFGENERATOR(fTime,fIceptPar1DC);
 fIceptPar2 = AFGENERATOR(fTime,fIceptPar2DC);
 fIceptPar3 = AFGENERATOR(fTime,fIceptPar3DC);
 fIceptPar4 = AFGENERATOR(fTime,fIceptPar4DC);
 fIceptPar5 = AFGENERATOR(fTime,fIceptPar5DC);


 switch(iInterceptionModel)
  {
   case 0:
   
	   fRelIntercept = (float) 0;//0.25;
       
       if ((!lstrcmp(pPl->pGenotype->acCropCode,"BW\0"))
		   ||(!lstrcmp(pPl->pGenotype->acCropCode,"BE\0"))) //Buche
	   {
        fRelIntercept = (float) 0.18;
	   }
       
       if ((!lstrcmp(pPl->pGenotype->acCropCode,"FW\0"))
		   ||(!lstrcmp(pPl->pGenotype->acCropCode,"SP\0"))) //Fichte
	   {
        fRelIntercept = (float) 0.29;
	   }

	   fIcept = fRelIntercept * fPotTr; 
   break;

   case 1: // Rutter Model

	   if(fIceptStor<fIceptPar3)
	   {
       fDrain = (float)0;
	   }
	   else
	   {
       fDrain = fIceptPar2*(fIceptStor-fIceptPar3);
	   }

       if(fIceptStor<fIceptPar3)
	   {
       fEvap = fIceptPar4*pWa->fPotETDay*fIceptStor/fIceptPar3;
	   }
	   else
	   {
       fEvap = min(fIceptStor,pWa->fPotETDay);// statt ETpot hier ETwet einsetzen!!!
       //fEvap = min(fIceptStor,pWa->fPotETDay/(float)1.1);// hier z.B. ETwet=ETpot/1.1  
       }
       
	   fIcept = ((float)1-fIceptPar1)*fPrec-fDrain-fEvap;
	   fIceptStor+=fIcept;
	   fIceptStor=max(fIceptStor,0);
	   fIcept += fEvap;
   break;

   case 2: // Gash Model

	   if ((double)1-(double)(fIceptPar5/((float)1-fIceptPar1))>(double)1e-4)
	   {
       fPsat = -fIceptPar3/fIceptPar5*(float)log((double)1-(double)(fIceptPar5/((float)1-fIceptPar1)));
	   }
	   else
	   {
       fPsat = fIceptPar3/fIceptPar5;
	   }

	   if (fPrec < fPsat)
	   {
       fIcept = ((float)1-fIceptPar1)*fPrec;
	   }
	   else
	   {
       //fIcept = ((float)1-fIceptPar1) * fPsat + fIceptPar5 * (fPrec-fPsat);
       fIcept = ((float)1-fIceptPar1)*(fPsat + fIceptPar5 * (fPrec-fPsat));
	   }
   break;

   case 3: // Hoyningen-Huene & Braden Model

	   fLAI   = pPl->pCanopy->fLAI;
	   fPlcv  = (float)min((double)fLAI/(double)3,(double)1);

       fIceptPar = (float)max((double)fIceptPar,(double)0.000001);
	   fIcept = fIceptPar * fLAI * ((float)1-fIceptPar1*fLAI/(fIceptPar1*fLAI+fPlcv*fPrec));
   break;

  }// end switch

  fIcept = max(fIcept,0);
  
  if(pWB->fReservoir >(float)0)
  {
   fIcept = pWB->fReservoir - max(pWB->fReservoir-fIcept,0);
   pPW->fInterceptDay  = fIcept;
   pWB->fReservoir -= fIcept;
  }
  
  pWB->fReservoir += fDrain;

  /*
  if(fIcept<=fPotTr)
  {
  fRelIntercept = min(1,fIcept/(fPotTr+(float)1e-9));
  }
  else
  {
  fRelIntercept = (float)1;
  }
  
  //falls Interzeption als Interzeptionsverdunstung gesehen (für die Ausgabe und Bilanz):
  pPW->fInterceptDay  = fRelIntercept*fPotTr;
  //*/

  //falls Interzeption unterhalb der Interzeptionskapazität: keine Traufe!
  //if(fIcept<=fIceptPar3)
  //{
  // pPW->fInterceptDay  = fIcept;//möglicherweise > pot. ET
  //}


  //der Anteil der nicht verdunstet kommt als durchfallender Niederschlag auf den Boden 
  if (pPW->fInterceptDay < fIcept) pWB->fReservoir += (fIcept-pPW->fInterceptDay);

  //fPotTraDay = pWa->fPotETDay - pWa->pEvap->fPotDay - pPl->pPltWater->fIntercept;
  //fPotTraDay *= ((float)1 - fRelIntercept);  
  //pPW->fPotTranspdt *= ((float)1 - fRelIntercept);
  //pWa->pEvap->fPotDay -= (float)0.5*pPl->pPltWater->fInterceptDay;

  return 1;
} /*  ende   Interception */

/*******************************************************************************
** EOF */