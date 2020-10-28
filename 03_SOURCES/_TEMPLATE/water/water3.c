/*******************************************************************************
 *
 * Copyright (c) by GSF- National Research Center for Environment and Health
 *                  Institute of Soil Ecology
 *                  P.O.Box 1129
 *                  D-85758 Neuherberg
 *
 * Author: C. Haberbosch (ch)
 *
 *------------------------------------------------------------------------------
 *
 * Description: Calculation of Soil Water Flow during frost
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 2 $
 *
 * $History: water3.c $
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/water
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 *   September 1997
 *
*******************************************************************************/


/* INCLUSION of HEADER-FILES */
#include "xinclexp.h"
#include "xh2o_def.h"
#include "xtempdef.h"

#define PlantIsGrowing    ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))


/* EXTERNAL FUNCTIONS */

/* from util_fct.c */
extern int NewDay(PTIME);
extern int SimStart(PTIME);      
extern int NewTenthDay(PTIME);
extern int   WINAPI Set_Minimal_Time_Step(PTIME);

extern int   WINAPI Message(long, LPSTR);
extern int introduce(LPSTR lpName);

/* from h2o_fct.c */
extern float WaterCapacity(EXP_POINTER);

/* from water1.c */
extern int Freezing(EXP_POINTER);

extern int WINAPI Wasser1(EXP_POINTER, int);

/* from util_fct.c */
extern int   WINAPI Message(long, LPSTR);


/* INTERNAL FUNCTIONS */
int WaterFrost(EXP_POINTER);
int SoilWaterFlow_GECROS(EXP_POINTER);
/***********************************************/
/*               dll-Funktionen                */              
/***********************************************/
int WINAPI _loadds dllCHWaterLeachFrost(EXP_POINTER);
//int WINAPI _loadds dllEPSoilWaterFlow(EXP_POINTER);               


/**********************************************************************************/
/* dll-Funktion:   dllCHWaterLeachFrost                                           */
/* Beschreibung:   Wasserhaushalt nach Leach, bei Frost simplifizierter Ansatz    */
/*                                                                                */
/*                 ch/gsf   23.9.97                                               */
/**********************************************************************************/
int WINAPI _loadds dllCHWaterLeachFrost(EXP_POINTER)
{        
  DECLARE_COMMON_VAR
  // DECLARE_H2O_POINTER
  PSLAYER     pSL;
  PSWATER     pSW;
  PWLAYER     pWL;

  static int iFrost = 0;

  #ifdef LOGFILE                            
    if (SimStart(pTi))
    {
      introduce((LPSTR)"dllCHWaterLeachFrost");
    }
  #endif

	if (iFrost)
	{
    WaterFrost(exp_p);
  }
  else
  {
    Wasser1(exp_p, 0);
  }

        
    if ((!iFrost))//&&(!SimStart(pTi)))
    {  
    // Does frost start?
    if ((pHe->fFreezingDepth*500 > pSo->fDeltaZ)||(pWa->pWLayer->pNext->fIce > (float)2 * pWa->pWLayer->pNext->fContAct))
	    {                   
	    Message(0,"Start of Frost");           
	    iFrost = 1;
	    }
    }
    else
    {
    // Does frost end?
    if ((pHe->fFreezingDepth*1000 <= pSo->fDeltaZ)&&(pWa->pWLayer->pNext->fIce < pWa->pWLayer->pNext->fContAct))
	    {                   
	    Message(0,"End of Frost");           
	    iFrost = 0;

		  for (H2O_SOIL_LAYERS)   // Layers 1 to n-1
			{                           
			pWL->fMatPotOld =  MATRIX_POTENTIAL(pWL->fContOld);
			pWL->fMatPotAct =  MATRIX_POTENTIAL(pWL->fContAct);
			}
		
		Set_Minimal_Time_Step(pTi);
	    }
	}
	
  return 1;
}   // Ende Berechnung

               

/********************************************************************************************/
/*                                                                                          */
/*        Name:      WaterFrost()                                                           */
/*                                                                                          */
/*        Function:  Calculates soil water flow during frost                                */
/*                                                                                          */
/*        Method:                                                                           */
/*        Authors:   ch, 23.09.97                                                           */
/*                                                                                          */
/********************************************************************************************/
/*  changed variables:                                                                      */
/********************************************************************************************/
int WaterFrost(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER

 PHLAYER           pHL;
 PHBALANCE         pHB;

 float f1;
 
 H2O_ZERO_LAYER

//*****************************************************************************
// 2. Sets water fluxes To 0
//*****************************************************************************
      
  for (SOIL_LAYERS0(pWL,pWa->pWLayer))
  	 {
  	 pWL->fFlux  = pWL->fFluxDens = (float)0;
     pWL->fContOld =  pWL->fContAct;
  	 }
    
    
    	pWa->fPercolR     =
        pWa->fInfiltR     =
        pWa->pEvap->fActR = 
	    pWa->pEvap->fMaxR        = (float)0;

 
            
// ***************************************************************
// 2. Change Water Content
// ***************************************************************
     Freezing(exp_p);      

/*  for (H2O_SOIL_LAYERS)   
   {
	if ((pWL->fIce > pWL->fIceOld + EPSILON)||(pWL->fIce < pWL->fIceOld - EPSILON))
	{
		pSW->fContSat    = max(pSL->fPorosity - pWL->fIce, THETA_MIN);
		
		
		/-* To calculate correct waterbalance the actual soil water content is recalculated  
		considering the change in ice content *-/
		pWL->fContAct   -= (pWL->fIce - pWL->fIceOld) / (float)1.1;
	}	 
   } /-* for */
     
     

// ***************************************************************
// 1. Rain Event
// ***************************************************************
     
  if (pWB->fReservoir > EPSILON)
    {

/* the very first simple approach: do not infiltrate, wait.*/
	    pWa->fPondWater  += pWB->fReservoir;
	    pWB->fReservoir  = (float)0;
    }
    

  if (pWa->fPondWater  > EPSILON)
    {
    
/* the second simple approach: fill up the first layers until frozen layer. */
  for (TEMP_SOIL_LAYERS)   
  {
  f1 = (pSW->fContSat - pWL->fContAct) * pSo->fDeltaZ;
  
  if (f1 > 0.02 * pSo->fDeltaZ)
  {
  f1 =  min(pWa->fPondWater, f1 - (float)0.01 * pSo->fDeltaZ);
  
  pWa->fPondWater -= f1;
  
  pWL->fContAct += f1/pSo->fDeltaZ;     
  
  pWa->fInfiltR += f1/DeltaT; 
  }
  
  if ((pHL->fSoilTemp < 0)||(pWa->fPondWater < EPSILON)) 
  	{
  	break; // stops if layer is really frozen.
  	}
  }

	    
    }


// ***************************************************************
// 2. Evaporation
// ***************************************************************

// Pot-evaporation is only limited by lack of water in the first layers.     

  if (pWa->pEvap->fPotR > EPSILON)
  {
  f1 = pWa->pEvap->fPotR * DeltaT;
  
  for (TEMP_SOIL_LAYERS)   
  {
  if ((pWL->fContAct - pSW->fContPWP)*pSo->fDeltaZ > f1)
    {
  	pWL->fContAct -= f1/pSo->fDeltaZ;
  	f1 = (float)0;
  	break;
  	}     
  	else
	    if (pWL->fContAct > pSW->fContPWP)
	    {
	  	f1 -= (pWL->fContAct - pSW->fContPWP) * pSo->fDeltaZ;
	  	pWL->fContAct = pSW->fContPWP;
	  	}     
  
	  if (pHL->fSoilTemp < 0) 
		  	break; // stops if layer is really frozen.
    
    } /* for */
    
    pWa->pEvap->fActR = pWa->pEvap->fPotR - f1/DeltaT;  

  }   /* evaporation */



 return 1;
}

/*******************************************************************************
 *
 * Copyright (c) by Helmholtz Zentrum München
 *                  German Research Centre for Environmental Health
 *                  Institute of Soil Ecology
 *                  P.O.Box 1129
 *                  D-85758 Neuherberg
 *
 * Author: E. Priesack (ep)
 *
 *------------------------------------------------------------------------------
 *
 * Description: Calculation of Soil Water Flow for GEOSTEP following GECROS
 *
 *------------------------------------------------------------------------------
 *
*******************************************************************************/
#include  "gecros.h"
extern double WINAPI _loadds NOTNUL(double x);
extern double WINAPI _loadds INSW(double x,double y1,double y2);

int SoilWaterFlow_GECROS(EXP_POINTER)
{
 PGECROSSOIL  pGS = pGecrosPlant->pGecrosSoil;
 PWLAYER      pWL = pWa->pWLayer;
	
 extern float fActTraDay;
 extern double SD1;
 
 int iLayer;
 float f1 = (float)0;
 float DeltaZ = pSo->fDeltaZ;
 float fProfileDepth=(float)0;
 
 double RFIR;
 double WCI,MULTF,RDI,WUL,WLL;
 double WCMIN,WCMAX,WCPWC,TCP,TCT; 
 double WCUL,WCLL,RD;
 double RRUL,RRLL,RWUL,RWLL,RRD;
 double RWUG,DWSUP;
 double ATCAN,AESOIL;
 double WSWI,WINPUT;
 //double WCFC = (double)0.25;//test

 extern float fparPRFDPT,fparWCMIN,fparWCMAX,fparWCPWC,fparSD1,fparTCT,fparTCP;

/* input parameters */
      WSWI = (double)+1.;//test -1.; else +1.;
      WINPUT = (double)15.;

 //Soil parameters part I (from readmod marker 80009)
      if (fparPRFDPT ==(float)-99) pGS->fProfileDepth = (float)150.;
	  else pGS->fProfileDepth = fparPRFDPT;

      if (fparWCMIN ==(float)-99) WCMIN = (double)0.05;
	  else WCMIN    = (double)fparWCMIN;

      if (fparWCPWC ==(float)-99) WCPWC = (double)0.25;
	  else WCPWC    = (double)fparWCPWC/NOTNUL((double)fparPRFDPT*10);
      pGS->fPlantWaterCapacity = (float)WCPWC;

	  if (fparWCMAX ==(float)-99) WCMAX = (double)0.35;
	  else WCMAX    = (double)fparWCMAX;
      WCMAX = (double)pGS->fPlantWaterCapacity + WCMIN;
      //WCMAX = (double)0.35;//test
	  
	  if (fparSD1 ==(float)-99) SD1 = (double)25.0;
	  else SD1    = (double)fparSD1;

      if (fparTCT ==(float)-99) TCT = (double)4;
	  else TCT    = (double)fparTCT;

      if (fparTCP ==(float)-99) TCP = (double)1;
	  else TCP    = (double)fparTCP;

	  MULTF = (double)1;//pGS->fWaterContInitFac;


	  for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)
	  {
		  fProfileDepth+=DeltaZ;
	  }

/* initial */
      if (SimStart(pTi))
	  {
	     RDI    = max(2.,SD1);
	     if(pPl!=NULL) RDI = max(RDI,min(100,(double)pPl->pRoot->fDepth));
		 if(pPl!=NULL) pPl->pRoot->fDepth = (float)RDI;

	     WCI    = WCPWC * MULTF;//pGS->fWaterContInitial;
	     //WCI    = WCFC * MULTF;//test
         WUL    = 10.*(WCI-WCMIN)*RDI;
         WLL    = 10.*(WCI-WCMIN)*((double)pGS->fProfileDepth-RDI);

		 fActTraDay = (float)0;
		 pWa->pEvap->fActR = (float)0;

         pGS->fWaterContUpperLayer = (float)WUL;
         pGS->fWaterContLowerLayer = (float)WLL;
		 pWa->pWBalance->fProfilStart = (float)(WUL + WLL);
		    
		 for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
         {
         pWL->fContAct = (float)(WUL + WLL)/fProfileDepth;
         }

         /*    
		 for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
         {
          if (iLayer*DeltaZ <= (float)(RDI*10.)) 
	        //pWa->pWLayer->fContAct=(float)WUL;
	        WUL += (double)pWL->fContAct*DeltaZ/NOTNUL(RDI*10.);
	      else
	        //pWa->pWLayer->fContAct=(float)WLL;
	        WLL += (double)pWL->fContAct*DeltaZ/NOTNUL(1500.-RDI*10.);
         }
         */
	  }// if SimStart


/* daily input */
      RFIR   = (double)pWa->pWBalance->fReservoir;//RAIN + IRRI
	  RFIR   = (double)pCl->pWeather->fRainAmount;
	  RDI    = max(2.,SD1);

	  if(pPl!=NULL)
	  {
        RD   = min(((double)fProfileDepth-10.)/10.,max(RDI,(double)pPl->pRoot->fDepth));// /10.;//[mm]->[cm]???
        //RD   = min(((double)fProfileDepth)/10.,max(RDI,(double)pPl->pRoot->fDepth));// /10.;//[mm]->[cm]???
	    RRD  = (double)pPl->pRoot->fDepthGrowR;// /10.;//[mm]->[cm]???
	  }
       
	  WUL    = (double)pGS->fWaterContUpperLayer;
	  WLL    = (double)pGS->fWaterContLowerLayer;

      //pWa->pEvap->fActR = min(pWa->pEvap->fActR,(float)(WUL*0.01));//Limitierung noch unklar 1% WUL oder höher???	  
	  AESOIL = (double)pWa->pEvap->fActR;
	  ATCAN  = (double)fActTraDay;//(double)pPl->pPltWater->fActTranspDay;//???
	  if (pPl != NULL) ATCAN  = (double)pPl->pRoot->fUptakeR;
	  //if ((pPl != NULL)&& (PlantIsGrowing)) ATCAN  = (double)pPl->pRoot->fUptakeR;//???

/* soil water */
      WCUL   = (WUL+WCMIN*10.*RD)/10./NOTNUL(RD);
      WCLL   = min(WCMAX, (WLL+WCMIN*10.*((double)pGS->fProfileDepth-RD))/10./NOTNUL((double)pGS->fProfileDepth-RD));
      //WCLL   = min(WCMAX, (WLL+WCMIN*10.*(150.-RD))/10./NOTNUL(150.-RD));//test

      RRUL   = min(10.*(WCMAX-WCUL)*                            RD /TCP, RFIR);
      RRLL   = min(10.*(WCMAX-WCLL)*((double)pGS->fProfileDepth-RD)/TCP, RFIR-RRUL);
      //RRLL   = min(10.*(WCMAX-WCLL)*(150.-RD)/TCP, RFIR-RRUL);//test

      RWUL   = RRUL+10.*(WCLL-WCMIN)*RRD-INSW(WSWI,0.,ATCAN+AESOIL);//+.1;
      //RWUL   = RRUL+10.*(WCLL-WCMIN)*RRD-AESOIL;//+.1;
      RWLL   = RRLL-10.*(WCLL-WCMIN)*RRD;
      RWUG   = max(0., RFIR-RRUL-RRLL);

      WUL    = max(0.,WUL+RWUL);
      WLL    = max(0.,WLL+RWLL);
      DWSUP  = INSW(WSWI, WINPUT, max(0.1,WUL/TCP+0.1));

/* daily output */
	  pGS->fWaterContUpperLayer = (float)WUL;
      pGS->fWaterContLowerLayer = (float)WLL;
      pGS->fETDailyWaterSupply  = (float)DWSUP;
	  pGS->fWaterFlowToLowLayer = (float)RRUL;

	  pWa->fInfiltR             = (float)RFIR;
	  pWa->fPercolR             = (float)RWUG;
	  //pWa->pEvap->fActR         = (float)AESOIL;
	  //pPl->pRoot->fUptakeR      = (float)ATCAN;
	  pWa->pWLayer->fFluxDens   = pWa->fInfiltR;//for fertilization in transp.c
      pWa->pWLayer->fFlux       = pWa->fInfiltR;//for fertilization in transp.c
      //Zeiger auf vorletzte Schicht setzen
	  //for (pWL = pWa->pWLayer;(pWL->pNext->pNext!=NULL);pWL = pWL->pNext);
      //pWL->fFlux                = pWa->fPercolR;//for leaching in transp.c

   ///*
   
   //RD = (double)pPl->pRoot->fDepth;

   if (RD > 0)
   {
    for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
    {
     if (iLayer*DeltaZ <= (float)(RD*10.)) 
	    pWL->fContAct=(float)(WUL/(RD*10.));
	 else if (((float)RD*10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= (float)RD*10.+DeltaZ))
     {
        f1 = (iLayer*DeltaZ-(float)(RD*10.))/DeltaZ;
	    pWL->fContAct=((float)1-f1)*(float)(WUL/(RD*10.))
			          //+ f1*(float)(WLL/NOTNUL((double)fProfileDepth-RD*10.));
			          + f1*(float)(WLL/((double)fProfileDepth-RD*10.));
     }
	 else
		//pWL->fContAct=(float)(WLL/NOTNUL((double)fProfileDepth-RD*10.));
		pWL->fContAct=(float)(WLL/((double)fProfileDepth-RD*10.));

     }
   }
   else //*/
   {
    for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
    {
     pWL->fContAct = (float)(WUL + WLL)/fProfileDepth;
    }
   }

	return 1;
}