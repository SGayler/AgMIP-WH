/*******************************************************************************
 *
 * Copyright 2001 - 2002 (c) by GSF- National Research Center for Environment and Health
 *                              Institute of Soil Ecology
 *                              P.O.Box 1129
 *                              D-85758 Neuherberg
 *                              GERMANY
 *
 * Author:  C.Haberbosch (ch), D.Maier (dm), E.Priesack (ep),
 *          C.Sperr (cs), S.Storm (ss)
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Calculation of Soil Water Flow.
 *   Modified approach following LEACHM 3.1
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 5 $
 *
 * $History: water1.c $
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 23.01.02   Time: 14:08
 * Updated in $/Projekte/ExpertN/ModLib/water
 * Anbindung an Datenbank auch unter Win XP möglich. Fitting in Gang
 * gesetzt.
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:24
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/water
 * Compiler Warning "Verwendung nicht initialisierter Variablen" entfernt.
 * 
 *   Date:  November 1996
*******************************************************************************/

#include "xinclexp.h"
#include "xlanguag.h"     
#include "xh2o_def.h"


/* SWITCHES */

/* This switch turns on a recalculation of hydraulic conductivites 
   during the iteration solving Richards equation (not in LEACHM) */
#undef  NEW_ITERATION
/* This switch causes recalculation of soil water capacities only
   every 0.1 day */
#undef CHECK_CAPACITY_ONLY_NEWTENTHDAY

/* this switch causes the water transport function to keep matric potential 
	in physically reasonable limits */
#undef CHECK_MATRIC_POTENTIAL

/* this switch causes the pde-solver to keep hydraulic conductivity bigger than 0 */
#undef KEEP_CONDUCTIVITY_POSITIVE

/* EXTERNAL FUNCTIONS */

extern PFCT			WCont,
					HCond,
					DWCap,
					MPotl;

/* from time1.c */
extern int NewDay(PTIME);
extern int SimStart(PTIME);      
extern int NewTenthDay(PTIME);

/* from util_fct.c */
extern int WINAPI Test_Range(float x, float x1, float x2, LPSTR c);
extern int WINAPI Message(long, LPSTR);
extern int WINAPI Set_At_Least_Epsilon(float *x, LPSTR c);

/* from h2o_fct.c */
extern double DDummy(double,double,double,double,double,
					 double,double,double,double,double,
                     double,double,double,double,double,
					 double,double,double,double,double);
extern float WaterCapacity(EXP_POINTER);

/* from runoff.c */
extern signed short int WINAPI RunOffSCS(EXP_POINTER);

/* ep Nov.96 from water1h.c */
//extern int Solve_Richards_Hydrus(EXP_POINTER);
/* from h2o_fct.c */

/* PedoTransferFunctions */
extern double PedoTransferCampbell(double,double,double,double,double,double, int);
extern double PedoTransferVereecken(double,double,double,double,double,double, int);

/* INTERNAL FUNCTIONS */
int   WINAPI Wasser1(EXP_POINTER, int);
int   WINAPI SolvePD(EXP_POINTER);

float UpperBoundary(EXP_POINTER);
int   SolveRG(EXP_POINTER);
//int   Freezing(EXP_POINTER);
int   CalculateSink(EXP_POINTER);

extern BOOL  FreeD=TRUE;

/*******************************************************************************************
 *
 *  Name     :  CalculateSink()
 *
 *  Function :  Calculates water sink due to water uptake by plants and soil water freezing 
 *				from change in soil water contents since last calculation of soil water flow
 *              assuming freezing and plant water uptake do not occur at the same time
 *
 *              ch 7.1.97
 *
 *******************************************************************************************/
int CalculateSink(EXP_POINTER)
{ 
  DECLARE_H2O_POINTER
  //PWLAYER     pWL;
  PLAYERROOT  pLR;
  
  int iLayer;

  H2O_ZERO_LAYER

  if (pPl == NULL) return -1;
  if (pPl->pRoot == NULL) return -1;                               
  if (pTi->pTimeStep->fAct < EPSILON) return -1;
  //if (pHe->fFreezingDepth > EPSILON) return -1;

  
  pPl->pRoot->fUptakeR = (float)0;
	  
  //for (SOIL2_LAYERS1(pWL,pWa->pWLayer->pNext,pLR,pPl->pRoot->pLayerRoot->pNext))
  for( pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,pWL = pWa->pWLayer->pNext,
       pLR = pPl->pRoot->pLayerRoot->pNext,iLayer = 1;
	   ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL)&&(pLR->pNext != NULL));
       pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,pLR = pLR->pNext,iLayer++)    
  {
	/* this is the sink term in Richards Equation: */ 
		  //pLR->fActLayWatUpt=(pWL->fContOld-pWL->fContAct)/pTi->pTimeStep->fAct; /* [1/day] */
		  //pLR->fActLayWatUpt=(WATER_CONTENT(pWL->fMatPotOld)-pWL->fContAct)/pTi->pTimeStep->fAct; /* [1/day] */
		  pLR->fActLayWatUpt=(WATER_CONTENT(pWL->fMatPotAct)-pWL->fContAct)/pTi->pTimeStep->fAct; /* [1/day] */
	/* It is assumed that H2O uptake only takes place, when soil is not freezing or thawing */
		if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON))
		{
		  pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * pSo->fDeltaZ;  /* [mm/day] */
		 }
      } /* for */
  
  
return 1;
}


   
/*******************************************************************************************/
/*                                                                                         */
/*  Name     :  UpperBoundary()                                                            */
/*  Function :  Calculates upper boundary condition                                        */
/*  Authors  :  cs, 24.08.94                                                               */
/*              ch, 25.09.95                                                               */
/*                                                                                         */
/*******************************************************************************************/
/*  veränd. Var.:		pWa->pEvap->fActR                                                  */
/*						pWa->pEvap->fMaxR                                                  */
/*						pWL->fHydrCond                                                     */
/*						pWL->pNext->HydrCond                                               */
/*						pWL->fMatPotAct                                                    */
/*						pWL->fMatPotOld                                                    */
/*                                                                                         */
/*******************************************************************************************/
float UpperBoundary(EXP_POINTER)
{ 
  DECLARE_H2O_POINTER
  DECLARE_COMMON_VAR
  
  float     PotX, PotLow, PotHigh;
  float     fSurfaceFlux,fInfilt2, fDeviation;
  int       iterat;
  float     fMat2;
  
  H2O_ZERO_LAYER

//******************************************************************
// 1. case: Infiltration
//          consider ponding ?
//******************************************************************
/*   if (fLeachPonding > EPSILON)
    {
    /-/******************************************************************
    /- 1a.  Ponding
    /-******************************************************************
	    fMat2 = pWL->fMatPotOld = pWL->fMatPotAct =  (float)0.0;
       }
     else                             
    */
    //******************************************************************
    // 1b. Infiltration without Ponding
    //******************************************************************
      if (pWa->fPotInfiltration > EPSILON)
       {
      
//      fSurfaceFlux > EPSILON, since DeltaT<=1
        fSurfaceFlux = min(pWa->fInfiltR, pWa->fPotInfiltration / DeltaT); // [mm/day]
 
    //******************************************************************
    // Calculation of matric potential in layer 0 to guarantee
    // occurring of a flux of fSurfaceFlux in layer 1
  
        // matric potential to calculated for layer 0
        PotX = min(pWL->fMatPotOld, (float)-100.0);            // [mm]
        //? better:  PotX = (float)2*lpMPO->vorig;
             
        // Set interval limits for iteration:
        PotLow = pSW->fMinPot;                                 // [mm]
        PotHigh = (float)-3e5;                            // [mm]
    
        // calculates hydraulic conductivity of layer 1
        pWL->pNext->fHydrCond = NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld);    // [mm/day]

              
        //*********** Iteration  ***************************    
        for (iterat=1;iterat<= 60;iterat++)   
          {
           pWL->fHydrCond = (CONDUCTIVITY(PotX) + pWL->pNext->fHydrCond)/(float)2.0;
    
           fInfilt2 = pWL->fHydrCond
                    * ((float) 1 - (pWL->pNext->fMatPotOld - PotX)/ pSo->fDeltaZ);   // [mm/day]
    
           fDeviation = (float)fabs((float)1.0 - (fInfilt2/fSurfaceFlux));     // [-]
    
           if (fDeviation < (float)0.001) 
             {  iterat += 200;} // exit
           else  
             if ((PotHigh - PotLow  < (float)0.0001)&&(PotLow - PotHigh  < (float)0.0001)) 
               { iterat += 100;} // exit
             else  
               { 
                if (fInfilt2 < fSurfaceFlux) 
                  {
                    PotHigh  = PotX;
                    PotX     = (PotX + PotLow)/(float)2.0;
                  } /* diminish matric potential of layer 0 */
                else  
                  {     
                    PotLow  = PotX;
                    PotX    = (PotX + PotHigh)/(float)2.0;
                  } /* diminish infiltration */
            
              } /* else fDeviation */
          }  /* end of iteration loop: iterat > 60 */
           
        if (iterat < 100)
           {
             Message(0,ERROR_H2O_PDE_ITERATION);           
           }

        pWL->fMatPotOld  =  PotX;
        pWL->fMatPotAct  =  PotX;
            
        // von Neumann condition for Crank-Nicolson scheme:                      
        fMat2 = (float) (((fSurfaceFlux - pWL->fHydrCond)      // [mm]
              * 2.0
              * pSo->fDeltaZ /pWL->fHydrCond)
              -  pWL->fMatPotOld
              +  pWL->pNext->fMatPotOld);

       } // infiltration without ponding   
    else           

    //******************************************************************
    // 2. case:  Evaporation
    //******************************************************************
      {
       if ((pWa->pEvap->fActR * DeltaT <  EPSILON)||(pWa->pEvap->fPotR * DeltaT <  EPSILON))
         {
          //fMat2 = (float)-1 * pSo->fDeltaZ;
          ///*
		   fMat2 = min((float)-1 * pSo->fDeltaZ ,(float)(-2) * pSo->fDeltaZ      // [mm]
                - pWL->fMatPotOld + pWL->pNext->fMatPotOld);
          //*/                                           
          // 15.11. no flux condition                                                     
          pWL->fMatPotAct = pWL->pNext->fMatPotAct - pSo->fDeltaZ;                  
          pWL->fMatPotOld = pWL->pNext->fMatPotOld - pSo->fDeltaZ;                  
 
         }  /* no evaporation, no flux */
       else 
         {
           /* layer 0 */
           pWL->fHydrCond = CONDUCTIVITY(pWL->fMatPotOld);       // [mm/day]
    
           /* layer 1 */
           pWL->pNext->fHydrCond = NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld);
    
          /* layer 0 = average of layer 1 and layer 0   */
           pWL->fHydrCond = (pWL->fHydrCond + pWL->pNext->fHydrCond)/ (float)2.0;
    
          /* comparison of evaporation fluxdensity with maximal possible  */
           pWa->pEvap->fMaxR = pWL->fHydrCond
                            * ((float)3000 *(float)102.2 + pWL->pNext->fMatPotOld)
                             / pSo->fDeltaZ;    // [mm]
    
           if (pWa->pEvap->fActR > pWa->pEvap->fMaxR )  
           // Limiting: soil hydraulic conductivity!
             {  
              /*
              #ifdef LOGFILE
               if ((pWa->pEvap->fActR - pWa->pEvap->fMax)*DeltaT > (float)1)
                  {
                   Message(0,ERROR_MAX_EVAP_TOO_HIGH); 
                  }              
              #endif 
              */
              
              fMat2 = - (float)3000 *(float)102.2 - pWL->pNext->fMatPotOld;    // [mm]
              pWa->pEvap->fActR = pWa->pEvap->fMaxR;                   // [mm/day]
             } 
           else        
             {// Limiting: evaporation rate!
    
              Set_At_Least_Epsilon(&pWL->fHydrCond, "fHydrCond");

              fMat2 = (- pWa->pEvap->fActR - pWL->fHydrCond)                  // [mm]
                      * (float)2.0 * pSo->fDeltaZ / pWL->fHydrCond
                      - pWL->fMatPotOld + pWL->pNext->fMatPotOld;

             }
    
         }   /* else evaporation  positive */
    }       /*  else no infiltration */

  return fMat2;
}  //  end    UpperBoundary
  

 
/******************************************************************************/
/*        Name:     SolveRG()                                                 */
/*                                                                            */
/*        Function: Calculates matric potentials and hydraulic conductivities */
/*                  by numerically solving Richards equation                  */
/*                                                                            */
/*        Authors:  ch 26.09.95, ep 15.04.94, cs 13.11.92, as 28.11.96        */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
/*   veränd. Var.:		pWL->fContAct                                         */
/*						pWL->fDiffCap                                         */
/*						pWL->fHydrCond                                        */
/*						pWL->fMatPotAct                                       */
/*						pWL->pBack->fMatPotAct                                */
/******************************************************************************/
int SolveRG(EXP_POINTER)
{
 double   F1[MAXSCHICHT],G1[MAXSCHICHT];
 double   dR33, dR35;
 float    vorPot[MAXSCHICHT],fMPAct,fMPOld,diff;
 int      iterat;
 int      iR9;
 
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER
 
 PLAYERROOT  pLR;
 H2O_ZERO_LAYER
 
//******************************************************************************
// 1. Set upper boundary condition (F1(0), G1(0))
// *****************************************************************************

   G1[0] = UpperBoundary(exp_p);

   #ifdef EVATRA_OUTPUT
      Deb_EvaTra(exp_p, (float)G1[0]);
   #endif
  
   F1[0] = (float)1;   
   
   /* if a different upper boundary for ponding is considered chose
   (fInfilt > (float)999)?  
                 (float)0 : (float)1;  */

// *****************************************************************************
// 2. Store old matric potentials, recalculate hydr. conductivities
// *****************************************************************************
  
  for (H2O_ALL_LAYERS)   // Layers 0 to n
  {
    /* originally in ExpertN or LEACHM: within iteration loop, but MatPotOld does not change. */
    pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
                   ? ((CONDUCTIVITY(pWL->fMatPotOld) + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld))
                       / (float)2.0)
                   : CONDUCTIVITY(pWL->fMatPotOld);                     // [mm/day]

    vorPot[iLayer] = pWL->fMatPotOld;                                   // [mm]
    
     // obsolete: F1[iLayer]   = G1[iLayer] = (float)0;
  
#ifdef KEEP_CONDUCTIVITY_POSITIVE
  	if ( pWL->fHydrCond < EPSILON)
  	{         
  	// better stop using this solver in such situations !!!
  	Message(0,"Hydraulic Conductivity near 0!");

  	pWL->fHydrCond = EPSILON;

  	}
#endif

  } 

//******************************************************************************
// 3. Solver of PDE
// *****************************************************************************
 
 for (diff = (float)9999.9,iterat=1;((iterat <= 20)&&(diff >= (float)2.0*DeltaT)); iterat++)
 {

 
 /******************************************************************************
  *           Hydraulic conductivities of soil profile   
  *           Differential water capacities of profile
  */
  for (H2O_ALL_LAYERS)   // Layer 0 to n
  {
    #ifdef NEW_ITERATION
        pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
                       ? ((CONDUCTIVITY(pWL->fMatPotAct) + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotAct))
                         / (float)2.0)
                       : CONDUCTIVITY(pWL->fMatPotAct);            // [mm/day]
    #endif                

    fMPAct = min(pWL->fMatPotAct,pSW->fMinPot);                // [mm]
    fMPOld = min(pWL->fMatPotOld,pSW->fMinPot);                // [mm]

  
   pWL->fDiffCap =((float)CAPACITY(fMPAct) + (float)CAPACITY(fMPOld))/(float)2;

  } // for  hydr. cond., diff. water capacities

/*******************************************************************************
 *                           Matrix - Coefficients
 *   Layer 1  to   n-1
 */
   if (pPl != NULL)
   {
   pLR = pPl->pRoot->pLayerRoot->pNext;
   }

  for (H2O_SOIL_LAYERS)
  {
   dR33 = ((double)(2.0*pSo->fDeltaZ*pSo->fDeltaZ* pWL->fDiffCap)            // [mm/day]
          / (double)DeltaT)
          + (double)pWL->fHydrCond + (double)pWL->pBack->fHydrCond;

   dR35 =  (double)pWL->fHydrCond * (double)(pWL->pNext->fMatPotOld);        // [mm*mm/day]
   dR35 += (double)pWL->pBack->fHydrCond * (double)(pWL->pBack->fMatPotOld);
   dR35 += (dR33 - (double)2.0*(double)pWL->fHydrCond - (double)2.0*(double)pWL->pBack->fHydrCond)
              *((double)(pWL->fMatPotOld));
   dR35 += (double)2.0*(double)pSo->fDeltaZ*
             ((double)pWL->pBack->fHydrCond - (double)pWL->fHydrCond);

   dR35 -= (double)((float)2*(WATER_CONTENT(pWL->fMatPotOld)-pWL->fContAct)/pTi->pTimeStep->fAct*pSo->fDeltaZ*pSo->fDeltaZ);
   /*
   if (pPl != NULL)
   {
	   if (pLR != NULL)
	   {
	   dR35 -= (double)((float)2 * pLR->fActLayWatUpt *pSo->fDeltaZ * pSo->fDeltaZ);
	   pLR = pLR->pNext;
	   }
   }
   */
   /*******************************************************************************
    * ----------------------------   Forward solution   ------------------------- */  

   F1[iLayer] = (double)pWL->fHydrCond/(dR33-((double)pWL->pBack->fHydrCond*F1[iLayer-1]));    // [-]

   G1[iLayer] =  dR35 + ((double)pWL->pBack->fHydrCond * G1[iLayer-1]);    // [mm]
   G1[iLayer] /= dR33 - ((double)pWL->pBack->fHydrCond * F1[iLayer-1]);
  } // for

/*******************************************************************************
 *                       Pointer to last element
 *                               i = n
 *
 * ===================  Lower boundary condition   ==================
 *                          
 */

  if (pWa->iBotBC <= (int)0) pWa->iBotBC = (int)1; //initialisation if not done before
                                                   //to default situation: "free drainage"

  if (pWa->iBotBC == (int)4) // lysimeter
  {
   pWa->fGrdWatLvlPot = pSo->fDepth - pWa->fGrdWatLevel + pSo->fDeltaZ/(float)2;
   if (pWL->pBack->fMatPotAct > pWa->fGrdWatLvlPot - pSo->fDeltaZ) iR9 = (int)2;
   else iR9 = (int)3;
  }
  

  if (pWa->iBotBC != (int)4) iR9 = pWa->iBotBC;

  if ((iR9 == (int)2)||(iR9 == (int)5)) // water table
  {
    pWL->fMatPotAct = pWa->fGrdWatLvlPot;
    pWL->fMatPotOld = pWa->fGrdWatLvlPot;
  }

  if (iR9 == (int)1) // free drainage
  pWL->fMatPotAct = ((float)G1[iLayer - 1]  - pWL->fMatPotOld + pWL->pBack->fMatPotOld)
                  / ((float)1.0 - (float)F1[iLayer - 1]);        // [mm]

  if (iR9 == (int)3) // zero flux  
  pWL->fMatPotAct = ((float)G1[iLayer - 1] + (float)2.0*pSo->fDeltaZ - pWL->fMatPotOld + pWL->pBack->fMatPotOld)
                  / ((float)1.0 - (float)F1[iLayer - 1]);        // [mm]

  pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);



/*******************************************************************************
 *
 *                       ---- Backward solution ----------
 *                                 from 
 *                              i = n-1 to 0
 */ 
// Pointer pWL to  list element n, i=n
   for ( diff=(float)0.0; 
        ((iLayer>0)&&(pWL->pBack!=NULL));
          iLayer--,pWL = pWL->pBack)    
      {
       pWL->pBack->fMatPotAct = (float)(F1[iLayer-1] * (double)(pWL->fMatPotAct)
                              + G1[iLayer-1]);
    
       if(iLayer>1)   diff += (float)fabs((double)(pWL->pBack->fMatPotAct - vorPot[iLayer-1]));

       vorPot[iLayer-1] = pWL->pBack->fMatPotAct;
      } /* for backwards  */

 }  /* end iteration (iterat = 1,20)   */

return 1;
} // SolveRG()
 

/********************************************************************************************/
/*                                                                                          */
/*        Name:      Water1()                                                               */
/*                                                                                          */
/*        Function:  Calculates soil water flow                                             */
/*                                                                                          */
/*        Method:    Follows the methods of the model LeachM V 3.1 given in watflo.for      */
/*                   By Hutson & Wagenet (1992)                                             */
/*                                                                                          */
/*        Authors:   ch, 04.10.95                                                           */
/*                                                                                          */
/********************************************************************************************/
/*  changed variables:      pCL->pWeather->fRainBegin       	pWL->fHydrCond              */
/*							pWa->fRunOffCum                     pWL->fFluxDens              */
/*							pWa->fInfiltR                       pWL->fFlux                  */
/*							pWa->fPondWater                     pWL->fFluxDensOld           */
/*							pWa->fRunOffR                       pWL->fMaxEvapFD             */
/*							pWa->fPercolR                       pSW->fPotInflec             */
/*							pWa->pEvap->fActR                   pSW->fContSat               */
/*							pWL->fContAct                       pSW->fContInflec            */
/*							pWL->fContOld                       pWB->fProfileIce            */
/*							pWL->fMatPotAct                     pWB->fProfilStart           */
/*                         	pWL->fMatPotOld                     pWB->fProfil                */
/********************************************************************************************/
int WINAPI Wasser1(EXP_POINTER, int iFlagSolver)
{
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER

 PEVAP     pEV = pWa->pEvap;
 
  /* util_fct.c */
 extern float  abspower(float, float);
 extern double abspowerDBL(double, double);

 /* yinit.c */
 extern int initSoilPotential(EXP_POINTER);



 float f1, f2;
 
/* STATIC VARIABLES */
static float fLeachPonding;
static float fLostPondCum;
static float fPondDay;
 
 H2O_ZERO_LAYER
 

            
// ****************************************************************************
// 1. Initialisation at begin of simulation
// ****************************************************************************
if (SimStart(pTi))    
   {
    fLeachPonding           = (float)0.0;
    fLostPondCum            = (float)0.0;
    fPondDay                = (float)1.0;
	    
	initSoilPotential(exp_p);  // newly calculation of soil water potential

   } // t=0 (SimStart)
            
            
// ***************************************************************
// 2. Ponding 
// ***************************************************************

   if ( //(NewDay(pTi)) &&
   		(SimTime >= fPondDay) && (fLeachPonding > EPSILON))
       {
        fLostPondCum     += fLeachPonding;                       // [mm]
                
        // Ponding as RunOff
        pWa->fRunOffDay  += fLeachPonding;                   // [mm]          
        pWa->fRunOffCum  += fLeachPonding;                   // [mm]          
        // pWa->fRunOffR     = fLeachPonding / DeltaT;          // [mm/day]

        #ifdef LOGFILE
          {
           if (fLeachPonding >= (float)10)
               {
	           itoa((int)(fLeachPonding+(float)0.5),lpOut,10); 
 		       strcat(lpOut,COMMENT_PONDING_RUNS_OFF); 
               Message(0,lpOut);
               }
          } 
        #endif            
     
        fLeachPonding = (float)0;
        /* if ponding changes upper boundary set flag here: fInfilt = CONST_RAIN_RATE; */     
     }

            
// ***************************************************************
// 3.  Rain event: 
// ***************************************************************

  if (pWB->fReservoir > EPSILON)
    {
  /*
     // Delete Ponding if rain event:
     if (fLeachPonding > EPSILON)
       {
        pWB->fReservoir -= fLeachPonding;
        
        if (pWB->fReservoir < (float)0)
        {                              
        pWB->fReservoir = (float)0;
        }

        fLostPondCum    += fLeachPonding;
                
        // Ponding as RunOff
        pWa->fRunOffDay += fLeachPonding;
        pWa->fRunOffCum += fLeachPonding;
       
	    fLeachPonding  = (float)0;
	    
       }
  */
  
//  sets time limit for end of infiltration
// ***************************************************************

    fPondDay = (int)SimTime + PONDING_TIME;

//  diminish amount of infiltration by runoff 
//*****************************************************************************
    
    if (NewDay(pTi))
    	{
    	/* this rate is not multiplied with DeltaT since it will be subtracted only once. */
    	pWB->fReservoir -= pWa->fRunOffR;
    	
    	pWB->fReservoir -=  pWa->fPondWater;
    	}
    
//   calculates water uptake capacity at begin of rain event:
// ***************************************************************
    pWa->fPotInfiltration = pWB->fReservoir;

    if (pWa->fPotInfiltration < (float)0)
        {                              
        pWa->fPotInfiltration = (float)0;
        }
    
    f1 = WaterCapacity(exp_p);
    
    if ( f1 < pWa->fPotInfiltration)
      {
        
        f2 = pWa->fPotInfiltration - f1;    

        pWa->fPotInfiltration   = f1;    /* Ponding */ 
        pWa->fInfiltR   = pWa->fPotInfiltration;

        fLeachPonding += f2;
        

           if (f2 >= (float)10)
               {
	           itoa((int)f2,lpOut,10); 
 		       strcat(lpOut,COMMENT_SET_PONDING); 
               Message(0,lpOut);
               }

#undef PONDING

	#ifndef PONDING
	                fLostPondCum     += fLeachPonding;                       // [mm]
	                
	        // Ponding as RunOff
	        pWa->fRunOffDay  += fLeachPonding;                   // [mm]          
	        pWa->fRunOffCum  += fLeachPonding;                   // [mm]          
	        // pWa->fRunOffR     = fLeachPonding / DeltaT;          // [mm/day]
	
	           if (fLeachPonding >= (float)10)
	               {
		           itoa((int)(fLeachPonding+(float)0.5),lpOut,10); 
	 		       strcat(lpOut,COMMENT_PONDING_RUNS_OFF); 
	               Message(0,lpOut);
	               }
	     
	        fLeachPonding = (float)0;
	#endif
        
      }
       else    /* f1 >= pWB->fReservoir */
	      {
	        pWa->fInfiltR = CONST_RAIN_RATE;   // standard value 100 mm/d    
	        

	      }
	
        pWB->fReservoir = (float)0;
 
 } // 3. Regen
 
 else

//   recalculates water uptake capacity during infiltration
//   every 0.1 day:
// ***************************************************************

 //if (NewTenthDay(pTi))
    {   
    /* Adds ponding to infiltration */
    if (fLeachPonding > EPSILON)
       {
    	pWa->fPotInfiltration    += fLeachPonding;
        fLeachPonding  = (float)0;
       }   
    
    /* Infiltration */
    if (pWa->fPotInfiltration > EPSILON)
      {
       f1 = WaterCapacity(exp_p);   // [mm] calculated once a day
       
       if ( f1 < pWa->fPotInfiltration)
         {
          pWa->fInfiltR   = f1;    /* Ponding */   // [mm/day]
  
          fLeachPonding = pWa->fPotInfiltration - f1;

          pWa->fPotInfiltration    = f1;    
        
         }
       else
         {
          pWa->fInfiltR = CONST_RAIN_RATE; /* standard value  100 mm/d  */
         } 
      }  // Infiltration
   
    }   // evt. NewTenthday 
 


/****************************************************************************** */
/* 4a. Caculates water sink/source due to freezing/thawing:                     */
/*     Berechne ThetaSat, Theta aktuell                                         */

     //Freezing(exp_p);      

/****************************************************************************** */
/* 4b. Calculates plant water uptake:                                           */
/*     lpPDZ->fwasser                                                           */

     CalculateSink(exp_p);      


//*****************************************************************************
// 5. Calculates new matrix potentials and unsaturated hydraulic conductivities 
//*****************************************************************************

	if (iFlagSolver==0)
      {
      SolveRG(exp_p); 
      }
	   else    
	  {
      //SolvePD(exp_p); 
      }
        
//*****************************************************************************
// 6. Calculates new water contents and water fluxes
//*****************************************************************************
    H2O_ZERO_LAYER
    
	// Test !!!
	  	if ( pWL->fMatPotAct < -1e6)
	  	{                          
#ifdef CHECK_MATRIC_POTENTIAL
	  	pWL->fMatPotAct = (float)-1e6;
#endif
	  	// better stop using this solver in such situations
	  	Message(0,"Matric Potential in Layer 0 very low!");
	  	}

    // Schicht 0:
    pWL->fFluxDens = pWL->fHydrCond
                       * ((float) 1 - (pWL->pNext->fMatPotAct
                       + pWL->pNext->fMatPotOld
                       - pWL->fMatPotAct
                       - pWL->fMatPotOld)
                       / ((float)2.0 * pSo->fDeltaZ)); // [mm/day]
      
    pWL->fFlux     = pWL->fFluxDens * DeltaT;          // [mm]
    
    
    /********************************************************************/
    /*  Diminishing of infiltration water:                              */
    
    if (pWL->fFlux > (float) 0)
    {

     if (pWL->fFlux > pWa->fPotInfiltration)
       {

     #ifdef LOGFILE                            
       if (pWL->fFlux >= pWa->fPotInfiltration + (float)0.1)
          {
          Message(0,ERROR_INFILTRATION_TOO_HIGH); 
          } 
     #endif
       
     pWL->fFlux     = pWa->fPotInfiltration;
     pWL->fFluxDens = pWL->fFlux / DeltaT;  

     pWa->fPotInfiltration   = (float)0;
    } // Fluss > SickerW
    else
     pWa->fPotInfiltration  -= pWL->fFlux;
    
   } /* Flux > 0 */  
    
     
    for (SOIL_LAYERS0(pWL,pWa->pWLayer))
      { 
       pWL->fFluxDensOld = pWL->fFluxDens;
      }
	
    for (H2O_SOIL_LAYERS) //  Schicht i = 1 bis  n-1
    {

	  	if ( pWL->fMatPotAct < -1e6)
	  	{                          

#ifdef CHECK_MATRIC_POTENTIAL
	  	pWL->fMatPotAct = (float)-1e6;
#endif

	  	// better stop using this solver in such situations
	  	Message(0,"Matric Potential very low!");
	  	
	  	}

	  	if ( pWL->fMatPotAct > pSW->fMinPot)
	  	{                          

#ifdef CHECK_MATRIC_POTENTIAL
	  	pWL->fMatPotAct = pSW->fMinPot;
#endif

	  	// better stop using this solver in such situations
	  	Message(0,"Matric Potential to high in soil!");
	  	
	  	}

    /*     Water contents of soil profile */                        
    pWL->fContOld = WATER_CONTENT(pWL->fMatPotOld);
	pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
                 
    /*     unsaturated hydraulic conductivities */
    pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
                   ? ((CONDUCTIVITY(pWL->fMatPotOld) + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld))
                     / (float)2.0)
                   : CONDUCTIVITY(pWL->fMatPotOld);                     // [mm/day]



    /*    Water flux densities within profile */                   
    pWL->fFluxDens = pWL->fHydrCond 
                   * ((float) 1 - (pWL->pNext->fMatPotAct
                   + pWL->pNext->fMatPotOld
                   - pWL->fMatPotAct
                   - pWL->fMatPotOld)
                   / ((float)2.0 * pSo->fDeltaZ));

    // Water flow per layer   
    pWL->fFlux = pWL->fFluxDens * DeltaT;


	} // for all soil layers     
    
    // This is here, because system does not update it anymore!
    pWL->fMatPotOld = pWL->fMatPotAct;
 
    
    //Water flow from bottom layer = Leaching
    pWa->fPercolR = pWL->pBack->fFluxDens;   


   // Water flow into/from top layer means infiltration/ evaporation (depending on sign)
    H2O_ZERO_LAYER                     

    if (pWL->fFluxDens  >= (float) 0)                         // zero layer
       {
        pWa->fInfiltR     = pWL->fFluxDens;
        pWa->pEvap->fActR = (float)0;
       }

    else
       {
        pWa->fInfiltR     = (float)0;
        pWa->pEvap->fActR = ((float)-1 * pWL->fFluxDens);
       }

    
// ****************************************************************************
// 7. Calculates unsaturated hydraulic conductivity of top layer for evaporation
// ****************************************************************************                     

   pWL->pNext->fHydrCond = CONDUCTIVITY(pWL->pNext->fMatPotAct);   // first layer    

   pWL->fHydrCond        = pWL->pNext->fHydrCond;  // zero layer = first layer    

   pEV->fMaxR = pWL->fHydrCond * 
            ((float)3000 * (float)102.2 + pWL->pNext->fMatPotAct) / pSo->fDeltaZ;


 return 1;
}

/*******************************************************************************
** EOF */