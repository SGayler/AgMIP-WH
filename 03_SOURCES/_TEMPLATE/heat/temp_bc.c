/*********************************************************************************************/
/* Modul         : TEMP_BC.c                                                                */      
/* Inhalt        : Boundary conditions for heat module                                       */
/*                                                                                           */
/* Author        : EP, CH                                                                        */
/*                                                                                           */
/* Date          : 25.09.96 obere Randbedingung aus Energiebilanz                            */
/*                          nach Horton and Chung (1991).                                    */
/*                 02.10.96 dm, Implementierung der neuen Variablenstruktur                  */
/*					14.1.97 ch, Sichtung der Funktionen										 */
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
extern int   WINAPI Set_At_Least_Epsilon(float *x, LPSTR c);

extern int WINAPI Test_Range(float x, float x1, float x2, LPSTR c);

/* function declarations: */
float WINAPI Get_Horton_UpperBC(EXP_POINTER);
float WINAPI Get_Horton_Surface_Energy_Balance(EXP_POINTER, float fTS);
float WINAPI Get_Daisy_LowerBC(EXP_POINTER);


/*********************************************************************************************/
/*                                                                                           */
/*   Get_Horton_UpperBC(exp_p);                                                              */
/*                                                                                           */
/*   Berechnung der Temperaturbedingungen                                                    */
/*   am oberen Profilrand                                                                    */
/*   für Daisy-Temperaturmodell                                                              */
/*   Aufruf bei NewDay                                                                       */
/*                                                                                           */
/*   ep/gsf 18.9.96                                                                          */
/*   as, 27.11.96  Header: Einf.Var.Änd.                                                     */
/*                                                                                           */
/*********************************************************************************************/
/*   veränd. Var.			pSO->fSoilAlbedo                                                 */
/*                                                                                           */
/*********************************************************************************************/
float WINAPI Get_Horton_UpperBC(EXP_POINTER)
{
  PWLAYER  pWL = pWa->pWLayer;
  PHLAYER  pHL = pHe->pHLayer;

  int   id;
  float f1, f2, fPi=(float)3.141592654;
  float fT,fA,fB,fM,fRA,fRB,fRM; 
  float fUpperTemp;

//float fOmega, fZ, fTempAir, fTempAmp;

/* ******************************************************************* */
/* Plausibilitätstest durchführen                                      */
/* ******************************************************************* */
if (SimStart(pTi))
  {
  } /* t=0 */
  
fT = pHL->fSoilTemp;
  
/*  
if ((RESERVOIR > (float)0)||(pWa->fInfiltR > (float)0))
  {
   //  fT         = pCl->pWeather->fTempAve; 
       fUpperTemp = fT; 
  }      
else
  {
*/
   // Albedo
   if (pWL->fContAct >= (float)0.25)
      {pSo->fSoilAlbedo = (float) 0.1;}
      
   else if (pWL->pNext->fContAct <= (float)0.1)
      {pSo->fSoilAlbedo = (float) 0.25;}
      
   else
      {pSo->fSoilAlbedo = (float) 0.35 - pWL->pNext->fContAct;}
    
   // Gesamt-Albedo
   if (pSo->fAlbedo == (float)0.0)
      {pSo->fAlbedo = pSo->fSoilAlbedo;}

   fA = pHL->pNext->fSoilTemp; 
   fB = pHL->pNext->fSoilTemp; 
   f1 = (float) 1;
   f2 = (float) 1;
    
   for (id=0; (f1>=(float)0)&&(id<=25); id++)
    {
      fA = fA - (float) 0.5;
      fB = fB + (float) 0.5;
      fRA=Get_Horton_Surface_Energy_Balance(exp_p,fA); 
      fRB=Get_Horton_Surface_Energy_Balance(exp_p,fB); 
      f1 = fRA * fRB;
    }
   
   if (f1 < (float) 0)
    {
      fM=(fA+fB)/(float)2.0;
      for (id=0;((fM-fA)>(float)0.01)&&(id<=25); id++)
      {
        fRM=Get_Horton_Surface_Energy_Balance(exp_p,fM);
        f2=fRA*fRM; 
        if (f2 >(float) 0)
        { fA=fM; }
        else
        { fB=fM; }
        fM=(fA+fB)/(float)2.0;
      }

      if ((fM-fA)>(float)0.01)
       {
       fM = fT;
//     fM = pCl->pWeather->fTempAve;
       }
      
    }
   else
    {
     fM = fT;
//   fM = pCl->pWeather->fTempAve;
    }
      
   fUpperTemp= fM;
/*
  } 
*/  
return fUpperTemp;
}
 



/*********************************************************************************************/
/*                                                                                           */
/*   Get_Horton_Surface_Energy_Balance(all_lp,fTS);                                          */
/*                                                                                           */
/*   Berechnung der Energiebilanz                                                            */
/*   am oberen Profilrand                                                                    */
/*   für Daisy-Temperaturmodell                                                              */
/*   Aufruf bei NewDay                                                                       */
/*                                                                                           */
/*   ep/gsf 25.9.96                                                                          */
/*********************************************************************************************/
float WINAPI Get_Horton_Surface_Energy_Balance(EXP_POINTER, float fTS)
{
  DECLARE_TEMP_POINTER
  DECLARE_COMMON_VAR
  
  PWEATHER pCW = pCl->pWeather;

  float DeltaZ       = pSo->fDeltaZ;
  float DayTime      = pTi->pSimTime->fTimeDay; 
  
  float fA, fC, fD, fE, fH, fL, fP, fR, fS, fT;
  float fRN, fSH, fHS, fLH, fGS, fHK, fHC, fBR;
  float fLAI,fLEXT;
  
  /* depth [mm]-->[m] */
  DeltaZ /= (float) 1000.0;

  	if (pPl == NULL)
	{
	    fLAI = (float)0;
	}
	else
	{
		fLAI = pPl->pCanopy->fLAI;
		//Albedo???
	}
	
	fLEXT=(float)0.65;


  TEMP_ZERO_LAYER

     fT = pHL->fSoilTemp;
//   fT = pCl->pWeather->fTempAve; 

     /*average daily dew point temperature */
     fD = (pCW->fHumidity / (float)100.0)
        * (float)6.11 * (float) exp((double)((float)17.269 * fT) / ((double)((float)237.3 + fT)));   
     fD = ((float) 234.67 * (float) log10((double)fD)-(float)184.2)
        / ((float) 8.233 - (float)log10((double)fD));

     fD = (DeltaT < (float)0.2) ?
          fD + ((pCW->fTempMax - pCW->fTempMin) / (float)2) * (float)sin((float)2 * (float)PI * (DayTime-(float)0.25))
        : fD;

     /* surface emissivity */      
     fE = (float)0.9 + (float)0.18 * pWL->pNext->fContAct;

     /* absolute air humidity [kg/m^3] */ 
     fH = (float) 1.323 * (float) exp((double)17.27 *(double)(fD/((float)237.3 + fD)))
        / (fT + (float) 273.16); 

     /* Stefan-Boltzmann Constant */
     fS = (float) 5.67e-8; 

     /* longwave sky irridiance */
     fR = fS * abspower((fT + (float)273.16), (float)4.0)
        * ((float) 0.605 + (float) 1.777 * abspower(fH, (float)0.5)); 

     /* roughness length */
     fL = (float)0.01; 

     /* aerodynamic boundary layer resistance */
     fA = abspower((float)log((double)2.0/(double)fL),(float)2.0)/((float)0.16 * pCW->fWindSpeed);   
     
     /* air heat capacity */
     fC = (float) 1154.8 + (float) 303.16 /(fT+(float)273.16);     

     /* soil surface matrix potential [m] */
     fP = min((float)0, pWL->pNext->fMatPotAct/(float)1000.0);     

     /* net radiation */ 
     fRN = ((float)1.0 - pSo->fAlbedo) * pCW->fSolRad 
         * (float) 11.57                                   /*SolRad [MJ/d/m^2] --> [J/s/m^2] */
         + fR - fE * fS * abspower((fTS + (float)273.16), (float)4.0);
	 
	 fRN*=(float)exp(-(double)fLEXT*(double)fLAI);
     
     /* sensible heat flux */
     fSH = fC * (fTS - fT) / fA;
     
     /* air humidity at soil surface [kg/m^3] */ 
     fHS = (float) 1.323 * (float) exp((double)17.27 * (double)(fTS/((float)237.3 + fTS)))
         / (fTS + (float) 273.16); 
         
     fHS = fHS *(float) exp((double)fP/((double)46.97 * (double)(fTS + (float) 273.16)));

     /*latent heat flux */
     fLH = ((fHS-fH)/fA/(float)1000.0) * ((float)2.49463e9 - (float)2.247e6 * fTS);
     
     /* soil heat flux density at soil surface */
     fHK = (float) 0.5 * (pHL->pNext->fConduct + pHL->pNext->pNext->fConduct)
         * (float) 1000.0 /(float) 86400.0; 
         
     fHC = (float) 0.5 * (pHL->pNext->fCapacity + pHL->pNext->pNext->fCapacity) 
         * (float) 1000.0; 
         
     fGS = fHK * (fTS - pHL->pNext->pNext->fSoilTemp) / DeltaZ 
         + fHC * (fTS - pHL->pNext->fSoilTemp) * DeltaZ / (DeltaT * (float)86400.0) / (float)2.0;

     /* energy balance at soil surface [J/s/m^2 = W/m^2]*/    
     fBR = fRN - fSH - fLH - fGS;    /* residual of */ 
     
//   fBR = fRN-fSH-fGS;              /* residual of */

return fBR;
}        



/*********************************************************************************/
/*                                                                               */
/*   Get_Daisy_LowerBC(exp_p);                                                   */
/*                                                                               */
/*   Berechnung der Temperaturbedingungen                                        */
/*   am unteren Profilrand                                                       */
/*   für Daisy-Temperaturmodell                                                  */
/*   Aufruf bei NewDay                                                           */
/*                                                                               */
/*   ( Sinusförmige Dirichletbed. mit jährlicher Periode)                        */
/*                                                                               */
/*   ch, 12.10.95                                                                */
/*   as, 27.11.96		Header: Variablenänderungen erfaßt.                      */
/*                                                                               */
/*********************************************************************************/
/*  veränd. Var.:		pCl->pAverage->fYearTemp                                 */
/*						pCl->pAverage->fMonthTempAmp                             */
/*                                                                               */
/*********************************************************************************/
float WINAPI Get_Daisy_LowerBC(EXP_POINTER)
{
  DECLARE_COMMON_VAR
  PHLAYER pHL;

  static float  fCond_To_Cap;
  
  float fD, fOmega, fZ,
        fMiddleCap,fMiddleCond,
        fLowerTemp,
        fDayMaxTemp = (float)190; /*! ch Schätzwert. */
                                  /* Tag-Nr. mit max. Luft-Temperatur */
                                  
  
/* ******************************************************************* */
/* Plausibilitätstest durchführen */
/* ******************************************************************* */
if (SimStart(pTi))
  {
  /*
  Da Mittlere Jahres-Temp. und -Amplitude nicht eingelesen werden, 
  werden diese Werte fuer Scheyern hier gesetzt.
  */

  //pCl->pAverage->fYearTemp      = (float)7.4;//fuer Scheyern
  //pCl->pAverage->fMonthTempAmp  = (float)10.4;//(float)10.4;//fuer Scheyern  
  pCl->pAverage->fMonthTempAmp  = (float)6;//fuer Solling  

          
  /* ******************************************************************* */
  /*  Quotient aus mittlerer Waerme-Kapazitaet und Konduktivität berechnen */
  /* ******************************************************************* */
  fMiddleCap = fMiddleCond = (float)0;
  
  for (SOIL_LAYERS1(pHL, pHe->pHLayer->pNext))
         {
          fMiddleCap  += pHL->fCapacity;
          fMiddleCond += pHL->fConduct;      
         }                                                              
        
          Set_At_Least_Epsilon(&fMiddleCap, "fMiddleCap");

  fCond_To_Cap = fMiddleCond/fMiddleCap;
  } /* t=0 */
        
fOmega = (float) 2 * (float)PI / (float)365;          /*[1/d] */

fD = (float)sqrt((float)2 *fCond_To_Cap /fOmega);     /*[m] */

fZ = (pSo->iLayers-2) * pSo->fDeltaZ / (float) 1000;  /*[m] */
  
/* ******************************************************************* */
/* Formel für untere RB ( Lösung der vereinfachten Wärmeleitungs-DGL)  */
/* ******************************************************************* */
fLowerTemp = pCl->pAverage->fYearTemp
           + pCl->pAverage->fMonthTempAmp
           * (float)exp(-fZ/fD)
           * (float)cos((double)fOmega*(pTi->pSimTime->iJulianDay - fDayMaxTemp) - fZ/fD);
                   
return fLowerTemp;
}
    

    





     
     
