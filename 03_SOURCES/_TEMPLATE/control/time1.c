/*******************************************************************************
 *
 * Copyright 2001 - 2002 (c) by ZeusSoft, Ing. Buero Bauer
 *                              www.zeussoft.de
 *                              Tel: 0700 zeussoft
 *
 * Author:  Chr. Sperr
 *          modifiziert: C. Haberbosch
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Modul Zeitschrittsteuerung
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: time1.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:04
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * DateToDays and NextDay removed (already implemented in "expbasic")
 * 
 *   15.05.94, 9.10.95, 17.1.96 miner. Duengung.
 *   29.01.96  SickerR statt SickerW als Reservoir, das zu infiltrieren ist.
 *   30.01.96  zeitwechsel2 für input()
 *   15.04.96 dm, Implementierung der neuen Variablenstruktur
 *
*******************************************************************************/

#include <windows.h>
#include "time1.h"
#include "expbasic.h"
#include "xinclexp.h"
#include "xlanguag.h"     

#undef MINIMAL_TIMESTEP_WHEN_FREEZING

/* from util_fct.c  */
extern float    abspower(float x, float y);
//extern BOOL bMaturity,bPlantGrowth;

/* from manag.c */
extern signed short int WINAPI TSSowing(EXP_POINTER);
extern signed short int WINAPI TSComparePreCropDate(EXP_POINTER);
extern signed short int WINAPI TSCompareStartValueDate(EXP_POINTER);
extern signed short int WINAPI TSCorrectMeasureValue(EXP_POINTER);

/* from balance.c */
extern int WINAPI Set_Day_Zero(EXP_POINTER);

/* from util_fct.c */
extern int   WINAPI Message(long, LPSTR);

/* defined functions */
int WINAPI Set_Minimal_Time_Step(PTIME);
int WINAPI Time1(EXP_POINTER);
int  CalculateTimeStep(EXP_POINTER);

int  NewDay(PTIME);
int  NewTenthDay(PTIME);


/***********************************************************************************/
/*                                                                                 */
/*  module  :   NewDay.c                                                           */
/*  author  :   ch / gsf:                                                          */
/*                                                                                 */
/*  result:   1 if the simulation time is actually in a new day.                   */
/*            0 if the old simulation time was already at the current day.         */
/*                                                                                 */
/***********************************************************************************/

int NewDay(PTIME pz)
{       
 return ((pz->pSimTime->fTimeAct == (float) 0)
     || ((int)(pz->pSimTime->fTimeOld) != (int)(pz->pSimTime->fTimeAct)))?
         1 : 0;
}

int EndDay(PTIME pz)
{
 /*
  return ( (int)(pz->pSimTime->fTimeAct + pz->pTimeStep->fAct + EPSILON) !=
          (int)(pz->pSimTime->fTimeAct + EPSILON) ) ?
        1 : 0;
  */
  return (pz->pSimTime->fTimeDay + pz->pTimeStep->fAct + EPSILON) >= (float)1?
        1 : 0;
}



/***********************************************************************************/
/*                                                                                 */
/*  module  :   NewTenthDay.c                                                      */
/*  author  :   ch / gsf:                                                          */
/*                                                                                 */
/*  result:   1 if the simulation time is actually in a new 10th of the day.       */
/*            0 if the old simulation time was already at the                      */
/*              current 10th of the day.                                           */
/*                                                                                 */
/***********************************************************************************/
int NewTenthDay(PTIME pz)
{
return ((pz->pSimTime->fTimeAct == (float) 0)
     || ((int)((float)10*(pz->pSimTime->fTimeOld + EPSILON)) != (int)((float)10*(pz->pSimTime->fTimeAct + EPSILON))))?
         1 : 0;
 }

/***********************************************************************************/
/*                                                                                 */
/*  module  :   SimStart.c                                                         */
/*  author  :   ch / gsf:                                                          */
/*                                                                                 */
/*  result:   1 if it is the first time step of the simulation.                    */
/*            0 else                                                               */
/*                                                                                 */
/***********************************************************************************/
int  SimStart(PTIME pz)
{ 
 return (pz->pSimTime->fTimeAct == (float) 0)?  1 : 0;
}

/***********************************************************************************/
/*                                                                                 */
/*  module  :   SimEnd.c                                                           */
/*  author  :   ch / gsf:                                                          */
/*                                                                                 */
/*  result:   1 if it is the last time step of the simulation.                     */
/*            0 else                                                               */
/*                                                                                 */
/***********************************************************************************/
int  SimEnd(PTIME pz)
{ 
 //return ((long)(pz->pSimTime->fTimeAct+1+1.1*pz->pTimeStep->fAct) > pz->pSimTime->iSimDuration)?  1 : 0;
 return ((long)(pz->pSimTime->fTimeAct+1.001*pz->pTimeStep->fAct) >= pz->pSimTime->iSimDuration)?  1 : 0;
}

/*************************************************************************************/
/*  Name     : Time1()                                                               */
/*  Autor    : Christian Haberbosch                                                  */
/*  Datum    : 17.01.97                                                              */
/*                                                                                   */
/*  Nach:    : Zeit1()                                                               */
/*  Autor    : Sperr Christoph                                                       */
/*  Datum    : 15.05.94                                                              */
/*                                                                                   */
/*************************************************************************************/
/*  veränd. Var.:   pTi->pTimeStep->fAct                                             */
/*					pTi->pSimTime->fTimeAct                                          */
/*					pTi->pSimTime->fTimeDay                                          */
/*					pTi->pSimTime->fTimeOld                                          */
/*					pTi->pSimTime->fTimeTenDay                                       */
/*************************************************************************************/
int WINAPI Time1(EXP_POINTER)
{ 
  DECLARE_COMMON_VAR

  PSIMTIME   time = pTi->pSimTime;
  PTIMESTEP  step = pTi->pTimeStep;
  PWLAYER    pWL;
  PSLAYER    pSL;
  PWBALANCE  pWB  = pWa->pWBalance;

  float f1 = (float)0;

  /* Set OLD variables: */
  step->fOld      = step->fAct;
  time->fTimeOld  = time->fTimeAct;     

  /* This flag is for graphics only: */
  pGr->bRedraw = 0;
  
  /*ch, old water content has to be set in water model:  pWL->fContOld   = pWL->fContAct; */
  /*ch, Das alte Potential sollte auch im wasser-Teil gesetzt werden:*/
  for (SOIL_LAYERS0(pWL,pWa->pWLayer))
  	   pWL->fMatPotOld = pWL->fMatPotAct;

  /* Set new time */
  if ((time->fTimeDay + step->fAct < (float)1 - EPSILON)
    //&&(((time->fTimeAct+step->fAct-(int)(time->fTimeAct+step->fAct))>EPSILON)
	  &&(((time->fTimeDay+step->fAct-(int)(time->fTimeDay+step->fAct))>EPSILON)
		 ||(time->fTimeDay<(float)0.9)))	  
	{
	/* same day: */
	//time->fTimeDay = time->fTimeAct+step->fAct-(int)(time->fTimeAct+step->fAct);
	time->fTimeDay = time->fTimeDay+step->fAct-(int)(time->fTimeDay+step->fAct);
	time->fTimeTenDay = time->fTimeDay
			- ((float)((int)(time->fTimeDay*(float)10)))/(float)10;
	time->fTimeAct = (int)(time->fTimeAct) + time->fTimeDay;

	if (((time->fTimeAct)>(float)0)&&(time->fTimeAct-(int)(time->fTimeAct)==(float)0))
		time->fTimeAct -= (float)0.5*step->fAct;
	///*
	if ((time->fTimeDay == (float)0)&&(step->fAct == (float)0))
		{
		// Abfrage ob Pflanzenmodell beginnen soll, hier beim Simulatiosstart //ep 20090216
		TSSowing(exp_p);//ep 220605 
		//ep 090701 TSComparePreCropDate(exp_p);
		TSCompareStartValueDate(exp_p);
		TSCorrectMeasureValue(exp_p);
		}
	//*/
	}
	else
    {                    
      /* NewDay */      
	  /* increase 1 day: */
		 if((long)time->fTimeAct < time->iSimDuration)
		 {

			  if((double)time->fTimeDay + (double)step->fAct>(double)((double)1-EPSILON))
			  { 
				   if(time->fTimeAct-(int)(time->fTimeAct)==(float)0)
					 time->fTimeAct += step->fAct;
				   else
					 time->fTimeAct = (float)((int)time->fTimeAct + (int)1);
			  }
			  else
			  {
					if(time->fTimeAct+step->fAct-(int)(time->fTimeAct+step->fAct)<EPSILON)
					{
						time->fTimeDay = time->fTimeAct+step->fAct-(int)(time->fTimeAct+step->fAct);
						time->fTimeAct += step->fAct;
						time->fTimeDay =(float)0.999;
						time->fTimeAct -=(float)0.001;
						step->fAct = (float)0.001;
						return 1;
					}
					else
						time->fTimeAct = (float)((int)time->fTimeAct + (int)1);
			  }
		  }

		  /* correction of  step->fAct: */           
		  step->fAct = (float)1 - time->fTimeDay;

		  time->fTimeDay = (float)0;        
		  time->fTimeTenDay = (float)0;
      

	   /*---------------------------------------------*/
	   /* solange Zeit kleiner Simulationszeit        */
	   /* OK = 1  d.h. weitersimulieren               */
	   /*---------------------------------------------*/
   
	   /* sollte sein: if ((long)time->fTimeAct >= time->iSimDuration  + 1)*/
	   //f1 = max(step->fAct,(float)1);
	   //if (f1 == (float)1) f1=(float)0;
	   //if ((long)(time->fTimeAct+f1) >= time->iSimDuration)   // Abbruchkriterium
	   if ((long)(time->fTimeAct+1) > time->iSimDuration)   // Abbruchkriterium
		{
		return 0;
		}

		/* Set all daily values  = 0 */
		Set_Day_Zero(exp_p);

		/* calculate actual date: */
		time->lTimeDate = NextDate(time->lTimeDate);
      
		while ((pCl->pWeather->pNext != NULL)&&(pCl->pWeather->lDate !=  time->lTimeDate))
				pCl->pWeather = pCl->pWeather->pNext;
		
		if (pCl->pWeather->lDate !=  time->lTimeDate)
	 	{
			Message(3,ERROR_WEATHER_DATA);
			return 0;
		}
	  
		/* set the pool of water that may infiltrate, runoff or stay on the surface. */
		pWa->pWBalance->fReservoir = pCl->pWeather->fRainAmount;


		if ((time->iJulianDay  > 366)||((int)(time->lTimeDate/100) == 101))
		{
			/* it is 1.1. */
			time->iJulianDay = 1;                       
		}
		else
       		time->iJulianDay ++;

		if (pPl != NULL)
		{
			if (pPl->pModelParam->lHarvestDate == time->lTimeDate)
	    	{
	    		pPl->pDevelop->bMaturity=TRUE;
		  		if ((pPl->pNext != NULL) && 
					(pMa->pSowInfo->pNext != NULL) //&&
				//(pPl->pGenotype->pNext != NULL)
				)
				{
				//pPl = pPl->pNext;
				pMa->pSowInfo     = pMa->pSowInfo->pNext;
				
				pPl->pGenotype    = pPl->pGenotype->pNext;
	        	pPl->pDevelop     = pPl->pDevelop->pNext;
				pPl->pBiomass     = pPl->pBiomass->pNext;
				pPl->pCanopy      = pPl->pCanopy->pNext;
				pPl->pRoot        = pPl->pRoot->pNext;
				pPl->pPltCarbon   = pPl->pPltCarbon->pNext;
				pPl->pPltWater    = pPl->pPltWater->pNext;
				pPl->pPltNitrogen = pPl->pPltNitrogen->pNext;
				pPl->pPltClimate = pPl->pPltClimate->pNext;
				pPl->pModelParam = pPl->pModelParam->pNext;

				if (pPl->pPMeasure!=NULL) pPl->pPMeasure = pPl->pPMeasure->pNext;

				pPl->pNext = pPl->pNext->pNext;
				pPl->pBack = pPl;
				
				}
			}

			if ((pPl->pPMeasure!=NULL)&&(pPl->pPMeasure->pNext!=NULL))
			{//now next pPM assigned in time.c //ep 250907
				if((pPl->pPMeasure->pNext->lDate == pTi->pSimTime->lTimeDate)
				&&(pPl->pPMeasure->pNext->pNext!=NULL))
					pPl->pPMeasure = pPl->pPMeasure->pNext;
			}

		}
		/* Taegliche Abfrage ob 
			Pflanzenmodell beginnen soll. */
		TSSowing(exp_p);//ep 220605 
		//ep 090701 TSComparePreCropDate(exp_p);
		TSCompareStartValueDate(exp_p);
		TSCorrectMeasureValue(exp_p);
  
     } /* NewDay */

  /* Calculate new time step: */
  if (step->fMin + EPSILON < step->fMax)
	 {
	  /* variable time step: */
	  CalculateTimeStep(exp_p);
	 }
  else
	 {                                
	  /* constant time step: */
	  step->fAct = step->fMax;
	 }


   /*--------------------------------------------*/
   /*****           LPBODEN              *********/
   /*--------------------------------------------*/

    for (SOIL2_LAYERS0(pWL, pWa->pWLayer, pSL, pSo->pSLayer))
        {
         pWL->fSoilAirVol = pSL->fPorosity - pWL->fContAct;
        }

	//SG 20151024: Runden auf 6 Stellen wegen anderer Definition von float ab VS 2011
	step->fAct =(int)((step->fAct+0.0000005)*1000000)/1000000.0;
	step->fAct = (float)((int)((step->fAct+0.0000005)*1000000))/1000000.0;

 return 1;
 }    



/*************************************************************************************/
/*  Name     : CalculateTimeStep()                                                   */
/*  Autor    : Christian Haberbosch                                                  */
/*  Datum    : 17.01.97                                                              */
/*                                                                                   */
/*************************************************************************************/
/*  veränd. Var.:   pTi->pTimeStep->fAct                                             */
/*					pTi->pSimTime->iTimeFlag1                                        */
/*					pTi->pSimTime->iTimeFlag2                                        */
/*************************************************************************************/
int CalculateTimeStep(EXP_POINTER)
{
  float         endRegen;
  float         f1,f4,maxFDichte,mf,dlz, maxdt;
    
  PSIMTIME      time = pTi->pSimTime;
  PTIMESTEP     step = pTi->pTimeStep;
  PWLAYER       pWL  = pWa->pWLayer;
  PWBALANCE  pWB  = pWa->pWBalance;

  maxFDichte = (float)-1.0;

  dlz        = pSo->fDeltaZ;
  mf         = (float)0.0;

  maxdt      = step->fMax;
  
  /*---------------------------------------------*/
  /* maximalen Wasserfluss im Profil ermitteln   */
  /*---------------------------------------------*/
  if(time->fTimeAct == (float)0.0)
    {
      maxFDichte = (float)1e10;
    }
  
  else
    {
     for (pWL = pWa->pWLayer; pWL->pNext->pNext != NULL; pWL = pWL->pNext)
        {
          mf = pWL->fFluxDens;
          if (mf < (float)0.0) mf *=(float)-1.0;        /*  ABS(mf)  */
          maxFDichte = max(mf,maxFDichte);
        } /* end for pWL */
    } /*  end else   Anfangszeit     */

   // ch, An Anfang gesetzt.
   /********************************************************************************
    *  notwendige Positionierung da identische Abfrage in Wasser.c/Oberflächenwasser
    *  und dort setzen lpw->SickerW
    *  Flags würden dann nicht mehr gesetzt
    *  Flags zur Zeitsteuerung setzen
    *********************************************************************************/
 
    if ((pCl->pWeather->fRainAmount > EPSILON) && NewDay(pTi))
       {
         time->iTimeFlag1 = 1;
         time->iTimeFlag2 = 0;     
       }/*  Regenereignis  */

    #ifdef FREEZING_SETS_MINIMAL_TIMESTEP
     // Frost-/Tau- Ereignis
     if ((pCl->pWeather->pBack != NULL) &&(NewDay(pTi)) &&
         (pCl->pWeather->fTempAve * pCl->pWeather->pBack->fTempAve < (float)0))
        {                                                 // d.h. sgn(Temp) != sgn(Temp gestern)
         time->iTimeFlag1 = 1;
         time->iTimeFlag2 = 0;     
        }/*  Frieren/Tauen */
    #endif

    if ((pWB->fReservoir > EPSILON) && (pWa->pWLayer->fFluxDens > EPSILON))
      {
       endRegen = (pWB->fReservoir / pWa->pWLayer->fFluxDens) * (float)1.01;
      }
      
    else
       endRegen = (float)1;
      
     /* if FDichte   */

     /*--------------------------------------*/
     /*  Grenzwerte Zeitschritt f3,f2        */
     /*--------------------------------------*/
     
    f1 = step->fOpt * dlz;

    f4 = step->fMax;

    /*--------------------------------------*/
    /*  Flags setzen  / Zeitsteuermax       */
    /*--------------------------------------*/

    if (time->iTimeFlag1 == 1)  
       {
         maxdt = step->fMin * (abspower((float)1.4,(float)time->iTimeFlag2));
         time->iTimeFlag2++;
       } /* if Zeitflag1 ...*/

    if (time->iTimeFlag2 == (int)60)  
       {
         maxdt = step->fMax;
         time->iTimeFlag1= 0;
       } /* if Zeitflag1 ...*/

    /*-----------------------------------------*
     *  Zeitschritt / aktuelle SimulationsZeit *
     *-----------------------------------------*/
    step->fAct = (float)min(f4,maxdt);
    step->fAct = (float)min(step->fAct,endRegen);
    step->fAct = (float)min(step->fAct,step->fMax);

    if (time->fTimeTenDay + step->fAct > (float)0.1)
       {                      
         step->fAct = (float)0.1 - time->fTimeTenDay;
       } 
        
     if (maxFDichte != (float)0.0)
        step->fAct = (float)min(step->fAct, f1 / maxFDichte);


  //   if (maxFDichte != (float)0.0)
	 //{
		// float f1_rounded, maxFDichte_rounded;
		// float f1_r2, f1_r7, maxFDichte_r7;
		// int   f1_int;
		// long	 maxFDichte_int;

		//f1_int = f1*10000000;
		//maxFDichte_int = maxFDichte*10000000;

		//f1_rounded = f1_int/10000000.0;
		//maxFDichte_rounded = maxFDichte_int/10000000.0;

		////f1_r2 = floorf(f1*100 + .5)/100;
		//f1_r7 = floorf((int)(f1*100000) + .5)/100000;
		//maxFDichte_r7 = floorf((int)(maxFDichte*100000) + .5)/100000;

		//step->fAct = (float)min(step->fAct, f1_r7 / maxFDichte_r7);
	 //}

        step->fAct = (float)max(step->fAct,step->fMin);
        
return 1;
}



/***********************************************************************************/
/*                                                                                 */
/*  module  :   Set_Minimal_Time_Step.c                                            */
/*  author  :   ch / gsf:                                                          */
/*                                                                                 */
/*  funktion:   Setzt Zeitflags für Zeitschrittsteuerung                           */
/*                                                                                 */
/*                                                                                 */
/***********************************************************************************/
/* veränderte Variablen:	pTi->pSimTime->iTimeFlag1                              */
/*							pTi->pSimTime->iTimeFlag1                              */
/***********************************************************************************/
int WINAPI Set_Minimal_Time_Step(PTIME pTi)
{
 
    pTi->pSimTime->iTimeFlag1 = 1;
    pTi->pSimTime->iTimeFlag2 = 1;
    

    return 1;
}

/*******************************************************************************
** EOF */