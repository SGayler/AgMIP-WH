/*********************************************************************************
 *
 *   WATER1H.C
 *
 *   Function: Calculation of Soil Water Flow 
 *   
 *   Method:   adapted for ExpertN following HYDRUS 6.0 
 *
 *   Author:   E.Priesack (ep) 
 *    
 *             GSF- National Research Center for Environment and Health
 *             Institute of Soil Ecology
 *             P.O.Box 1129
 *             D-85758 Neuherberg
 *             GERMANY
 *
 *   Date:     November 1996
 *
 *   Changes:  ep 4.03.97
 *
 ********************************************************************************/
#include <stdio.h>
//#include <process.h>


/* INCLUSION of HEADER-FILES */
#include "xinclexp.h"
#include "xlanguag.h"     
#include "xh2o_def.h"


// NTabD 50 + MAXSCHICHT 100 exceeds 64K limit!
#define  NTabD 			    250


typedef float Tableau[NTabD][MAXSCHICHT];

#define LOGFILE



FILE *FluxFile;
char acFluxName[80] = "";


float TiefenTab[500];
int   iSickerWasserAusgabe;


FILE *WGFile;
FILE *MPFile;
char acWGName[80] = "";
char acMPName[80] = "";

float WGMPTiefenTab[500];
extern float fCapillRiseR;
int   iWassergehaltAusgabe;

/*
float TiefenTab[] = {(float)-99, 
					 (float)0,
					 (float)10,
					 (float)50,
					 (float)300.0, 
					 (float)310.0, 
					 (float)320.0, 
					 (float)330.0,
					 (float)340.0,
					 (float)350.0,
					 (float)-99};
*/

/* EXTERNAL FUNCTIONS */

/* from util_fct.c */
extern float  abspower(float,float);
extern double abspowerDBL(double,double);
extern float signum(float x, float y);
extern int WINAPI Message(long, LPSTR);

/* from time1.c */
extern int NewDay(PTIME);
extern int SimStart(PTIME);      
extern int NewTenthDay(PTIME);
extern int EndDay(PTIME pz);

/* from h2o_fct.c */
extern double ZD_PotIm(double,double,double,double,double,
					   double,double,double,double,double,
                       double,double,double,double,double,
					   double,double,double,double,double);
 /* yinit.c */
extern int initSoilPotential(EXP_POINTER);


int WINAPI Water_Hydrus_6(EXP_POINTER,int);
int GenMat(EXP_POINTER, Tableau, Tableau, Tableau);
int SetMat(EXP_POINTER, Tableau, Tableau, Tableau);
int SetBC(EXP_POINTER);
int ChangeBC(EXP_POINTER);
int SetEq(EXP_POINTER);
int SolveEq(EXP_POINTER);
int TestCvg(EXP_POINTER);
int TmCont(EXP_POINTER);
int Veloc(int,double);
float Fqh(float,float,float);

/* STATIC VARIABLES */
static double FAR P[MAXSCHICHT],R[MAXSCHICHT],S[MAXSCHICHT];
static double FAR dCosAlf;//,ddt,ddtOld,dbltAct,dbltNew,dbltOld,dblt;
static float FAR ThN[MAXSCHICHT],ThO[MAXSCHICHT],hNew[MAXSCHICHT],hOld[MAXSCHICHT];
static float FAR Sink[MAXSCHICHT];
static float FAR hTab[NTabD];
static float FAR ThR[MAXSCHICHT],ThS[MAXSCHICHT],hSat[MAXSCHICHT],ConSat[MAXSCHICHT];
static float FAR Con[MAXSCHICHT],ConO[MAXSCHICHT],Cap[MAXSCHICHT],hTemp[MAXSCHICHT];
static float FAR rTop,rTopOld,rBot,rBotOld,hTop,hTopOld,hBot,hBotOld,vTop,vBot,v[MAXSCHICHT];
static float FAR KodBot, KBOld, KodTop, KTOld, rMin, rMax;
//static int   iTop,iBot,iTopOld,iBotOld,iTab;
static BOOL  TopInF,BotInF,WLayer,AtmBC,SeepF,qGWLF,FreeD;
static BOOL  BckspF,MinStep,ItCrF, ConvgF;

static float FAR t,tOld,tNew,tFix,tDay,tFix,tMax,dt,dtOld,DeltaTOld,DeltaT,DeltaZ;
static float FAR dtW,dtMin,dtMax,dtMaxW,dtMaxC,dtInit,dtOpt,dMul1,dMul2;
static int   Iter, ItCum, ItMax, ItMin, IGoTo; 
static float vTopFlux,vBotFlux,vFlux[MAXSCHICHT];//ck 20151225

/************************************************************************/
/*
//////////|||||||||||||||||||||||\\\\\\\\\\\\\\\\\\\\
//||||||||Source files Hydrus 6.0||||||||||||||||||||
\\\\\\\\\\|||||||||||||||||||||||////////////////////
*/
/************************************************************************/
int WINAPI Water_Hydrus_6(EXP_POINTER,int MOB_IMM)
{
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER
 

 int   N=pSo->iLayers-1;
 float f1;
 float afTS[MAXSCHICHT],afTR[MAXSCHICHT];
 float fTheta,fSink,fECDT;
 float fPotIm,fEpsZD;
 float fIrrAm=(float)0, fTheta50=(float)0, fThetaCrit=(float)0;
 extern int iIrrAm;
  
 static  Tableau  ConTab, CapTab, STab;
 
 PEVAP pEV = pWa->pEvap;

 H2O_ZERO_LAYER
 
 DeltaT=pTi->pTimeStep->fAct;
 DeltaZ=pSo->fDeltaZ;
///*
 if(MOB_IMM == (int)1)
 {
  for (H2O_ALL_BUT_NOT_FIRST)
  {
   afTS[iLayer]=pSW->fContSat;
   afTR[iLayer]=pSW->fContRes;
   //pSW->fContMobSat=(float)0.25*afTS[iLayer];
   //pSW->fContImmSat=(float)0.75*afTS[iLayer];
   //pSW->fMobImmEC=(float)0.142857;//24;//1440;//24;//0.00275;
   pSW->fContSat=pSW->fContMobSat;
   pSW->fContRes=(float)0;
  }
 }
//*/
 if (SimStart(pTi))
 {
	 //iTop=2; 
	 //iBot=3;
	 //iTab=1;

	 dCosAlf=(double)1;
	 fCapillRiseR = (float)0;


	 //*********** top boundary condition ****************************************************************
	 AtmBC =TRUE;//TRUE if variable upper boundary conditions given by atmospheric conditions

	 KodTop=(float)-1;//positive for Dirichlet, negative for Neumann bc
	 TopInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3
	 WLayer=FALSE;//TRUE if water can accumulate at the soil surface without run off, then KodTop negative
	 
	 //*********  bottom boundary condition **************************************************************
     if (pWa->iBotBC == (int) 1)// free drainage
	 {
	  KodBot=(float)-1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(float)3;//-5;
	  BotInF=FALSE;//TRUE if time dependent bc, then KodTop= +3 or -3 
	  FreeD=TRUE;  //TRUE if free drainage is considered as lower bc, then KodBot=-5
	  SeepF=FALSE; //TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
	  qGWLF=FALSE; //TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
	 }

     if (pWa->iBotBC == (int) 2)// constant ground water level
	 {
	  KodBot=(float)+1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(float)3;//-5;
	  BotInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3 
	  FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
	  SeepF=FALSE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
	  qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
	 }

	 if (pWa->iBotBC == (int) 3)// no flux
	 {
	  KodBot=(float)-1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(float)3;//-5;
	  BotInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3 
	  FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
	  SeepF=FALSE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
	  qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
	 }

	 if (pWa->iBotBC == (int) 4)// lysimeter
	 {
	  KodBot=(float)-1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(float)3;//-5;
	  BotInF=FALSE;//TRUE if time dependent bc, then KodTop= +3 or -3 
	  FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
	  SeepF=TRUE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
	  qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
	 }

	 if (pWa->iBotBC == (int) 5)// dynamic ground water table
	 {
	  KodBot=(float)+1;//positive for Dirichlet, negative for Neumann bc	 //KodBot=(float)3;//-5;
	  BotInF=TRUE;//TRUE if time dependent bc, then KodTop= +3 or -3 
	  FreeD=FALSE;//TRUE if free drainage is considered as lower bc, then KodBot=-5
	  SeepF=FALSE;//TRUE if seepage face is to be considered as lower bc (e.g. for lysimeter), then KodBot=-2
	  qGWLF=FALSE;//TRUE if discharge-groundwater level relationship 'q(GWL)' is applied, then KodTop=-7
	 }
	 //******************** input modification boundary conditions ***************************************
	 if (TopInF) KodTop=signum((float)3,KodTop);//positive for Dirichlet, negative for Neumann bc
	 if (AtmBC)  KodTop=(float)-4;
	 if (WLayer) KodTop=(float)fabs((double)KodTop);
	 if (BotInF) KodBot=signum((float)3,KodBot);//positive for Dirichlet, negative for Neumann bc
	 if (qGWLF)  KodBot=(float)-7;
	 if (FreeD)  KodBot=(float)-5;
	 if (SeepF)  KodBot=(float)-2;
	 KTOld=KodTop;
	 KBOld=KodBot;

	 //******************** input time control information ***************************************
	 ItMin=3;
	 ItMax=20;
	 ItCum=0;
	 dMul1=(float)1.2;
	 dMul2=(float)0.8;
	 dtMin=(float)1.e-6;
	 //SG 20120213 
	 // für Pareto: dtMin = 1.e-6 --> 1.e-4
	 // (weniger "Hänger" bei der Optimisierung)
	 // dtMin=(float)1.e-4;
	 dtMax=(float).5;
	 dtMaxW=(float).5;
	 dtMaxC=(float).5;
	 dtInit=(float)5.e-2;
	 dtOpt=(float)0.01;
	 rMin=(float)1.e-32;
	 rMax=(float)1.e+10;
	 
	 tOld=(float)0;//SimTime;
	 tNew=DeltaT;//SimTime+DeltaT;
	 DeltaTOld=DeltaT;
	 dtOld=dt=(float)max((double)DeltaT/(double)3,(double)1.e-8);
	 t=tOld+dt;//SimTime+dt;
	 tMax=(float)1;//(float)(pTi->pSimTime->iSimDuration+(int)1);
		 
     //******************* generate hydraulic function tables **********************************
	 GenMat(exp_p,ConTab,CapTab,STab);     

     //******************* initialize matric potentials and water contents *********************
	 for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL!=NULL;pWL=pWL->pNext,iLayer++)    
	 {
	     hNew[iLayer] = pWL->fMatPotAct;
		 hOld[iLayer] = pWL->fMatPotOld;
		 
		 ThN[iLayer]  = pWL->fContAct;
		 ThO[iLayer]  = pWL->fContOld;
	 }	// end for
///*
    if(MOB_IMM == (int)1)
    {
     for (H2O_ALL_BUT_NOT_FIRST)
     {
      pWL->fContMobAct=pWL->fContAct*pSW->fContMobSat/afTS[iLayer];//pSW->fContSat;
      pWL->fContMobOld=pWL->fContMobAct;
      pWL->fContImmAct=pWL->fContAct*pSW->fContImmSat/afTS[iLayer];//pSW->fContSat;
      pWL->fContImmOld=pWL->fContImmAct;
      ThN[iLayer]= pWL->fContMobAct;
	  ThO[iLayer]= pWL->fContMobOld;
	 } 
    }
	else //*/
	{
    for (H2O_ALL_BUT_NOT_FIRST)
     {
 	  pWL->fContMobAct=pWL->fContMobOld=pWL->fContImmAct=pWL->fContImmOld=(float)0;
	 }
	}
 }	 
 else // not SimStart
 { 
     tOld=(float)0;//SimTime;
     tNew=DeltaT;//SimTime+DeltaT;
     //dtOld=
	 dt=(float)max((double)DeltaT/(double)3,(double)1.e-8);
     t=tOld+dt;//SimTime+dt;

 } 

 
 //***************************** defining sink term (includes root water uptake) **********************
 for (H2O_ALL_BUT_NOT_FIRST)
 {
 ///* 
  switch(MOB_IMM)
  {
  case 0://Without mobile-immobile distinction
  // this is the sink term in Richards Equation: 
  Sink[iLayer] =-(pWL->fContAct-WATER_CONTENT(pWL->fMatPotAct))/DeltaT;//[1/day]
  break;
  
  case 1://mobile-immobile first order exchange
  
  //Sink[iLayer] =(pWL->fContAct-pWL->fContMobAct-pWL->fContImmOld)/DeltaT;//[1/day]
  
  //
  fTheta = pWL->fContMobAct/pSW->fContMobSat*(pSW->fContImmSat-afTR[iLayer])
		                       -(pWL->fContImmAct-afTR[iLayer]); 
  fSink = pWL->fContMobAct/pWL->fContAct* 
		  (pWL->fContAct-pWL->fContMobAct-pWL->fContImmAct);
  Sink[iLayer] =pSW->fMobImmEC*fTheta-fSink/DeltaT;
  //
  break;
  
  case 2://mobile-immobile bimodal hydraulic functions
  // this is the sink term in Richards Equation: 
  Sink[iLayer] =-(pWL->fContAct-WATER_CONTENT(pWL->fMatPotAct))/DeltaT; // [1/day]
  break;
  }//switch */
  //Sink[iLayer] =-(pWL->fContAct-WATER_CONTENT(pWL->fMatPotAct))/DeltaT;//[1/day]
 }//for
 

 //ck: 20151125
 vTopFlux=0.0;
 vBotFlux=0.0;
    {
        int i;
        for (i=0;i<MAXSCHICHT;i++)
            {
                vFlux[i] = 0.0;
            }
    }
    
 for (pWL=pWa->pWLayer,iLayer=0; pWL!=NULL; pWL=pWL->pNext,iLayer++)
        {
            pWL->fFluxDensOld = pWL->fFluxDens;            
        }
 
 Iter=0;
 IGoTo=0;

 ItLoop: //! beginning of iteration loop ---------------------------
 
 if (IGoTo<2)
   {
    //Water_BC_In(exp_p);
	SetBC(exp_p);
	Iter=0;
	/*
	if ((NewDay(pTi))&&(IGoTo<1)&&
	((float)fabs((double)rTopOld-(double)rTop)>(float)1.e-8))
	{
	 MinStep=TRUE;
	 dt=dtMin;
	 dtMax=dtMin;//*(float)1.e+2;
	 t=tOld+dt;
	}
	*/
   }

 if (IGoTo<1)
   {
	 for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL!=NULL;pWL=pWL->pNext,iLayer++)    
	 {
	  hTemp[iLayer]=hNew[iLayer]+(hNew[iLayer]-hOld[iLayer])*dt/dtOld;
	  hOld[iLayer]=hNew[iLayer];
	  hNew[iLayer]=hTemp[iLayer];
	  ThO[iLayer]=ThN[iLayer];
	 }	// end for
   } //end IGoTo<1


 //******* Generate terms of matrix equation and solve by Gauss elimination **********************

 SetMat(exp_p,ConTab,CapTab,STab);
 
 SetEq(exp_p);
 
 ChangeBC(exp_p);

 //*************************** begin iterative solution ***************************************************
 for (H2O_ALL_BUT_NOT_FIRST) //i=iLayer
 {
  hTemp[iLayer]=hNew[iLayer];
 }

 SolveEq(exp_p);

 Iter =Iter+1;
 ItCum=ItCum+1;

 
 //Convergency criterion
 TestCvg(exp_p);

 //Continue iteration or redo with smaller time step 
 if ((IGoTo<3)&&(ItCrF==FALSE)) goto ItLoop;

 // ck: ep: 20151225
            vTopFlux+=(-(Con[2]+Con[1])/(float)2*((hNew[2]-hNew[1])/DeltaZ/(float)2-(float)dCosAlf)
                       +((ThN[1]-ThO[1])/dt+Sink[1])*DeltaZ)*dt;
            // orig. Hydrus:
            //vBotFlux+=(-(Con[N]+Con[N-1])/(float)2*((hNew[N]-hNew[N-1])/DeltaZ/(float)2-(float)dCosAlf)
            //            -((ThN[N]-ThO[N])/dt+Sink[N])*DeltaZ)*dt;
            // Eckart Priesack (verbessert Wasserbilanz)
			vBotFlux+=(-(Con[N]+Con[N-1])/(float)2*((hNew[N]-hNew[N-1])/DeltaZ/(float)2-(float)dCosAlf))*dt;
			
   		    Veloc(pSo->iLayers-1,dCosAlf);
            
            {
                int i;
                for (i=0;i<MAXSCHICHT;i++)
                    {
                        vFlux[i] += v[i]*dt;
                    }
            }



 //Next dt until t=tNew where tNew=SimTime+DeltaT
 if (((float)fabs((double)t-(double)tNew)>(float)0.001*dt)&&(IGoTo<3))
 {
  tOld=t;
  dtOld=dt;
  KTOld=KodTop;
  KBOld=KodBot;
  
  TmCont(exp_p);
  t=t+dt;
  
  Iter=0;
  IGoTo=0;

  goto ItLoop;
 }

 //**************************** evaluation based on calculated new matric potentials **********************
 
// root water uptake
 if (pPl != NULL) 
 {
  PLAYERROOT  pLR = pPl->pRoot->pLayerRoot;

  pPl->pRoot->fUptakeR = (float)0;
 
  for (
	   pSL = pSo->pSLayer->pNext,pSW = pSo->pSWater->pNext,
	   pWL = pWa->pWLayer->pNext,pLR = pPl->pRoot->pLayerRoot->pNext,
	   iLayer = 1;                   
      
       ((pSL->pNext != NULL)&&(pSW->pNext != NULL)&&(pWL->pNext != NULL)&&
        (pLR->pNext != NULL));

	   pSL = pSL->pNext,pSW = pSW->pNext,pWL = pWL->pNext,pLR = pLR->pNext,
       iLayer++    
      )           

    {
	 pLR->fActLayWatUpt = Sink[iLayer];
     ///*
	 if(MOB_IMM == (int)1)
     {
      pLR->fActLayWatUpt = -(pWL->fContAct-pWL->fContMobAct-pWL->fContImmAct)/DeltaT;
	 }
	 //*/
	 //It is assumed that H2O uptake only takes place, when soil is not freezing or thawig 
	 if ((pWL->fIce < pWL->fIceOld + EPSILON)&&(pWL->fIce > pWL->fIceOld - EPSILON))
	 {
	  pPl->pRoot->fUptakeR +=  pLR->fActLayWatUpt * DeltaZ;  // [mm/day] 
	 }
    }
 }// end of root water uptake

 for (H2O_ALL_BUT_NOT_FIRST)
    {
     ///*  
     switch(MOB_IMM)
	 {
	  case 0:
	  pWL->fContOld = WATER_CONTENT(pWL->fMatPotAct);
      pWL->fMatPotAct=hNew[iLayer];
	  pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
	  break;
	  
	  case 1:
	  pWL->fContMobOld = WATER_CONTENT(pWL->fMatPotAct);
	  pWL->fContOld = pWL->fContMobOld+pWL->fContImmAct;
      
	  pWL->fMatPotAct=hNew[iLayer];
	  pWL->fContMobAct = WATER_CONTENT(pWL->fMatPotAct);

	  fTheta = pWL->fContMobAct/pSW->fContMobSat*
		      (pSW->fContImmSat-afTR[iLayer])+afTR[iLayer]; 
	  fECDT = pSW->fMobImmEC*DeltaT/((float)1+pSW->fMobImmEC*DeltaT);
	  fSink = pWL->fContImmAct/pWL->fContAct*(pWL->fContAct-pWL->fContOld);
	  pWL->fContImmOld = pWL->fContImmAct;
	  pWL->fContImmAct = fECDT*fTheta+(fSink+pWL->fContImmOld)
		                              /((float)1+pSW->fMobImmEC*DeltaT);

	  pWL->fContAct = pWL->fContMobAct+pWL->fContImmAct;
	  break;
	  
	  case 2:
	  pWL->fContOld = WATER_CONTENT(pWL->fMatPotAct);
      pWL->fMatPotAct=hNew[iLayer];
	  pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);

	  pWL->fContImmOld = pWL->fContImmAct;
	  fEpsZD=(float)0.0005;
      fPotIm=(float)ZD_PotIm((double)pWL->fMatPotAct,(double)pWL->fContAct,(double)pSW->fCondSat,
                     (double)pSW->fContSat,(double)pSW->fContRes,(double)pSW->fVanGenA,
                     (double)pSW->fVanGenN,(double)pSW->fVanGenM,(double)pSW->fCampA,
                     (double)pSW->fCampB,(double)pWL->fMatPotAct,(double)fEpsZD,(double)pSW->fMinPot,
                     (double)pWL->fMatPotOld,(double)pWL->fContOld,(double)pSW->fVanGenA2,
                     (double)pSW->fVanGenN2,(double)pSW->fVanGenM2,(double)pSW->fBiModWeight1,
                     (double)pSW->fBiModWeight2);
	  
	  if(fPotIm==pWL->fMatPotAct)
	  {
	   #ifdef LOGFILE
       {
        Message(0,"Wrong value for immobile matric potential");
       } 
       #endif
	  }

	  pWL->fContImmAct = WATER_CONTENT(fPotIm);

	  pWL->fContMobOld = pWL->fContMobAct;
	  pWL->fContMobAct = pWL->fContAct-pWL->fContImmAct;

	  break;
	 }//switch */
      
	  /*
	  pWL->fContOld = WATER_CONTENT(pWL->fMatPotAct);
      pWL->fMatPotAct=hNew[iLayer];
	  pWL->fContAct = WATER_CONTENT(pWL->fMatPotAct);
	  */
	  /*
	 //ep 20120203 AGMIP Maize Irrigation (first 50cm for DeltaZ=5cm)
	 if (iLayer<=10) 
	 {
	  fTheta50+=pWL->fContAct;
	  fThetaCrit+=(float)0.6*(pSW->fContFK-pSW->fContPWP);
	 }*/

    }//for
 
 //vTop=-(Con[2]+Con[1])/(float)2*((hNew[2]-hNew[1])/DeltaZ/(float)2-(float)dCosAlf)
 //     +((ThN[1]-ThO[1])/dt+Sink[1])*DeltaZ;
// vBot=-(Con[N]+Con[N-1])/(float)2*((hNew[N]-hNew[N-1])/DeltaZ/(float)2-(float)dCosAlf)
//      -((ThN[N]-ThO[N])/dt+Sink[N])*DeltaZ;
 
 //vTop=-(Con[2]+Con[1])/(float)2*((hNew[2]-hNew[1])/DeltaZ/(float)2-(float)dCosAlf);
 //vBot=-(Con[N]+Con[N-1])/(float)2*((hNew[N]-hNew[N-1])/DeltaZ/(float)2-(float)dCosAlf);
 //vBot=-(Con[N-1]+Con[N-2])/(float)2*((hNew[N-1]-hNew[N-2])/DeltaZ/(float)2-(float)dCosAlf);

 // ck:
    if (ConvgF==TRUE)
    {
    vTop=vTopFlux/DeltaT;
    vBot=vBotFlux/DeltaT;
    {
                int i;
                for (i=0;i<MAXSCHICHT;i++)
                    {
                        v[i] = vFlux[i]/DeltaT;
                    }
    }
    } else
    {
    vTop = 0.0;
    vBot = 0.0;
    {
                    int i;
                    for (i=0;i<pSo->iLayers;i++)
                        {
                            v[i] = 0.0;
                        }
    }
    }

 // Water flow into/from top layer means infiltration/ evaporation (depending on sign)
 H2O_ZERO_LAYER                     
 /*
 //ep 20120203 AGMIP Maize Irrigation
 pPl->pPltWater->fInterceptDay=(float)0;
 if(fTheta50 < fThetaCrit)
 {
  //fIrrAm = pMa->pIrrigation->fAmount;
  fIrrAm = (float)iIrrAm;
  pWa->pWBalance->fReservoir+=fIrrAm;
  pPl->pPltWater->fInterceptDay=fIrrAm/DeltaT;
 }
*/
 //f1=max(pCl->pWeather->fRainAmount-max(vTop,(float)0),(float)0);
 //f1=max(pCl->pWeather->fRainAmount-pWa->fRunOffR-max(vTop,(float)0),(float)0);
 f1=max(pWa->pWBalance->fReservoir-pWa->fRunOffR-max(vTop,(float)0),(float)0);
 pWa->pEvap->fActR = min(-min(vTop,(float)0)+f1,pWa->pEvap->fPotR);
 //pWa->pEvap->fActR = -min(vTop,(float)0)+f1;
 pWa->fInfiltR     =  max(vTop,(float)0)+f1;
 pWa->fPercolR     =  max(vBot,(float)0);
 fCapillRiseR      = -min(vBot,(float)0);
 //Maximal actual evaporation rate
 pWL->pNext->fHydrCond = NEXT_CONDUCTIVITY(pWL->pNext->fMatPotAct); // first layer    
 pWL->fHydrCond        = pWL->pNext->fHydrCond;  // zero layer same value as first layer    
 pEV->fMaxR	= pWL->fHydrCond*((float)300000+pWL->pNext->fMatPotAct)/DeltaZ;//pSo->fDeltaZ;

 //Water flux densities and water flow
 pWL->fFluxDens=vTop;
 pWL->fFlux = pWL->fFluxDens * DeltaT;

 Veloc(pSo->iLayers-1,dCosAlf);

 for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
   {
    pWL->fFluxDens=v[iLayer];
    pWL->fFlux = pWL->fFluxDens * DeltaT;
   }
 pWL->fFluxDens=vBot;
 pWL->fFlux = pWL->fFluxDens * DeltaT;
 pWL->pBack->fFluxDens=vBot;
 pWL->pBack->fFlux = pWL->pBack->fFluxDens * DeltaT;

 // update hydraulic conductivities for root water uptake (Nimah and Hanks 1973)
for (H2O_SOIL_LAYERS) //  Schicht i = 1 bis  n-1
  {
    /*     unsaturated hydraulic conductivities */
    pWL->fHydrCond = (iLayer < pSo->iLayers - 1)
                   ? ((CONDUCTIVITY(pWL->fMatPotOld) 
				       + NEXT_CONDUCTIVITY(pWL->pNext->fMatPotOld))/ (float)2.0)
                   : CONDUCTIVITY(pWL->fMatPotOld);                     // [mm/day]

  } // for all soil layers     
///*
if(MOB_IMM == (int) 1)
 {
  for (H2O_ALL_BUT_NOT_FIRST)
  {
   pSW->fContSat=afTS[iLayer];
   pSW->fContRes=afTR[iLayer];
  }
 }
//*/

 return 1;
}  //   end  


/************************************************************************/
int GenMat(EXP_POINTER, Tableau ConTab, Tableau CapTab, Tableau STab)
{
  DECLARE_COMMON_VAR
  DECLARE_H2O_POINTER
 
  float x0,xN,dx;
  float fh;
  int i,iM;

  H2O_ZERO_LAYER
   
  hTab[0]=-(float)1e-8;
  hTab[NTabD-1]=-(float)1e+8;

  x0=(float)log10(-(double)hTab[0]);
  xN=(float)log10(-(double)hTab[NTabD-1]);
  dx=(xN-x0)/(float)(NTabD-1);
 
  for (i=0; i<=NTabD-1; i++)
  {
   hTab[i]=-abspower((float)10,(x0+i*dx));
  } 

  for (H2O_ALL_BUT_NOT_FIRST) //iM --> iLayer
  {
   //Material M=MatNum[i]
   iM=iLayer;
   ConSat[iM]=pSW->fCondSat;
   ThS[iM]=pSW->fContSat;
   ThR[iM]=pSW->fContRes;
   //fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(float)1;
   fh=ThR[iM]+(ThS[iM]-ThR[iM])*(float)1;
   hSat[iM]=MATRIX_POTENTIAL(fh);


   for (i=0; i<=NTabD-1; i++)
     {
        ConTab[i][iM]=CONDUCTIVITY(hTab[i]);
        CapTab[i][iM]=CAPACITY(hTab[i]);
        STab[i][iM]=(WATER_CONTENT(hTab[i])-ThR[iM])/(ThS[iM]-ThR[iM]);
                    // -pSW->fContRes)/(pSW->fContSat-pSW->fContRes);
     }
  } /* for: soil layers */ 
/*
  for (H2O_ALL_BUT_NOT_FIRST) // M=1,NMat
  {
    ConSat[iLayer]=pSW->fCondSat;
    ThS[iLayer]=pSW->fContSat;
    ThR[iLayer]=pSW->fContRes;                                                                                           
    fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(float)1;
    hSat[iLayer]=MATRIX_POTENTIAL(fh);
  }
*/
  return 1;
} /* end GenMat */

/************************************************************************/
int SetMat(EXP_POINTER, Tableau ConTab, Tableau CapTab, Tableau STab)
{
  DECLARE_COMMON_VAR
  // DECLARE_H2O_POINTER
  PSLAYER     pSL;
  PSWATER     pSW;
  PWLAYER     pWL;
  PWBALANCE   pWB;

  float hi1,hi2;
  float x0,xN,dx,dh,hiM,hTabM;
  float Coni,Capi,SSSi;
  int   M,iT;
  int   NumNP = pSo->iLayers-1;

  H2O_ZERO_LAYER

  hTabM=hTab[NTabD-1];
  x0=(float)log10((double)(-hTab[0]));
  xN=(float)log10((double)(-hTabM));
  dx=(xN-x0)/(float)(NTabD-1);

  for (H2O_ALL_BUT_NOT_FIRST) //i=iLayer
  {
       // Material M=MatNum(i)
	   M=iLayer;
       // Hysterese
/*       if(Kappa(i).eq.-1) 
	   {   
		hi1=min(hTemp(i)/Ah(i),hSat(M))
		hi2=min(hNew(i)/Ah(i),hSat(M))
	   }
       if(Kappa(i).eq.+1) hiM=min(hNew(i)/Ah(i)/AhW(M),hSat(M))
*/
       
		hi1=(float)min((double)hTemp[iLayer],(double)hSat[M]);
		hi2=(float)min((double)hNew[iLayer],(double)hSat[M]);
	   	hiM=(float)0.1*hi1+(float)0.9*hi2;
	   //hiM=min(hNew[iLayer],hSat[M]);
       if((hi1>=hSat[M])&&(hi2>=hSat[M]))
       {
           Coni=ConSat[M];
	   }
       else 
	   {
		if ((hiM>hTab[NTabD-1])&&(hiM<=hTab[0]))
	    {
		   iT=(int)(((float)log10((double)(-hiM))-x0)/dx);
           dh=(hiM-hTab[iT])/(hTab[iT+1]-hTab[iT]);
           Coni=ConTab[iT][M]+(ConTab[iT+1][M]-ConTab[iT][M])*dh;
        }
	    else
        {
           Coni=CONDUCTIVITY(hiM);
	    }
       }

	   if(hiM>=hSat[M])
	   {
	      Capi=(float)0;
          SSSi=(float)1; 
       } 
       else 
       {
         if((hiM>=hTab[NTabD-1])&&(hiM<=hTab[0]))
         {
             iT=(int)(((float)log10((double)(-hiM))-x0)/dx);
             dh=(hiM-hTab[iT])/(hTab[iT+1]-hTab[iT]);
             Capi=CapTab[iT][M]+(CapTab[iT+1][M]-CapTab[iT][M])*dh;
             SSSi=STab[iT][M]+(STab[iT+1][M]-STab[iT][M])*dh;
         } 
         else
         {
             Capi=CAPACITY(hiM);
             SSSi=(WATER_CONTENT(hiM)-ThR[M])/(ThS[M]-ThR[M]);
         }
       }      

       Coni=CONDUCTIVITY(hiM);
       Capi=CAPACITY(hiM);
       SSSi=(WATER_CONTENT(hiM)-ThR[M])/(ThS[M]-ThR[M]);
       
	   Con[iLayer]=Coni;
       Cap[iLayer]=Capi;

       if(Iter==0) ConO[iLayer]=Con[iLayer];

       ThN[iLayer]=ThR[M]+SSSi*(ThS[M]-ThR[M]);

  }// soil layers 

  return 1;
}  //   end  


/************************************************************************/
int SetBC(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER
 
 float hCritA,fh;
 H2O_ZERO_LAYER

 fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(float)0.0002;
 hCritA=MATRIX_POTENTIAL(fh);
  
 if (TopInF==TRUE)
 { 
   rTopOld=rTop;
   rTop=(float)fabs((double)pWa->pWBalance->fReservoir)
        -(float)fabs((double)pWa->fRunOffR)
	    -(float)fabs((double)pWa->pEvap->fActR);
   //if((float)fabs((double)rTopOld-(double)rTop)>(float)1.e-8) MinStep=TRUE;
   
   //hTopOld=;
   //hTop=;
   //if(abs(hTopOld-hTop)>(float)1.e-8) MinStep=TRUE;
   //rRoot=;
 }

 if (BotInF==TRUE)
 {
   rBotOld=rBot;
   rBot=(float)0;
   if (pWa->iBotBC == (int)3) rBot=(float)-0.0;//no flux 
   //if(abs(rBotOld-rBot)>(float)1.e-8) MinStep=TRUE;
   hBotOld=hBot;
   hBot=(float)99;
   
   pWa->fGrdWatLvlPot = pSo->fDepth - pWa->fGrdWatLevel + pSo->fDeltaZ/(float)2;
   if (pWa->iBotBC == (int)2) hBot=pWa->fGrdWatLvlPot;
   //if(abs(hBotOld-hBot)>(float)1.e-8) MinStep=TRUE;
   if (pWa->iBotBC == (int)5) hBot=-pCl->pWeather->fWaterTable + pSo->fDeltaZ/(float)2;
 }

 return 1;
}  /*   end  */

/************************************************************************/
int ChangeBC(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER

 float hCritS,hCritA,fh;
 int   N=pSo->iLayers-1;

 H2O_ZERO_LAYER

 //critical parameters
 fh=pSW->fContRes+(pSW->fContSat-pSW->fContRes)*(float)0.0002;
 hCritA=MATRIX_POTENTIAL(fh);
 hCritS=(float)0;

//   Seepage face at the bottom
       if(SeepF==TRUE)
         {
          if(KodBot>=(float)0)
            {
             //vBot=(Con[N-1]+Con[N])/(float)2*((hNew[N-1]-hNew[N])/DeltaZ+(float)dCosAlf)
             //     -((ThN[N]-ThO[N])/dt+Sink[N])*DeltaZ/(float)2;
			 
			 vBot=-(Con[N]+Con[N-1])/(float)2*((hNew[N]-hNew[N-1])/DeltaZ/(float)2-(float)dCosAlf);

             if(vBot<(float)0)
               {
                KodBot=-(float)2;
                rBot=(float)0;
               }
            }   
          else
            {
             if(hNew[N]>=(float)0)
               {
                KodBot=(float)2;
                hBot=(float)0;
               }
            } /* if KodBot >=0 */
         } /* if SeepF */

//   Atmospheric boundary condition
       if(TopInF==TRUE)
         {
          if(KodTop>=(float)0)
            {
             vTop=-(Con[2]+Con[1])/(float)2*((hNew[2]-hNew[1])/DeltaZ-(float)dCosAlf)
                  +((ThN[1]-ThO[1])/dt+Sink[1])*DeltaZ/(float)2;
			 
			 vTop=-(Con[2]+Con[1])/(float)2*((hNew[2]-hNew[1])/DeltaZ/(float)2-(float)dCosAlf)
                  +((ThN[1]-ThO[1])/dt+Sink[1])*DeltaZ;


             if((fabs((double)vTop)>fabs((double)rTop)) || (vTop*rTop<=(float)0))
               {
                KodTop=-(float)4;
               }
            }  
          else
            { 
             if((hNew[1]>hCritS) && (WLayer==FALSE))
               {
                KodTop=(float)4;
                hTop=(float)hCritS;
               } 
            
             if (hNew[1]<=hCritA) 
               {
                KodTop=(float)4;
                hTop=hCritA;
               }
            } /* if KodTop >=0 */
         } /* if TopInF */



 return 1;
}  //   end  

/************************************************************************/
int SetEq(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER

 double dx,ddt;
 double dCosAlf,dxA,dxB,ConA,ConB,ST,RT,PT;
 int   N=pSo->iLayers-1;
 int   i;
 // SG 20110414
 float fAqh=(float)- 0.973,fBqh=(float)-0.038; // übernommen von Christine Klier aus Expert-N 4.01
 //float fAqh=(float)1,fBqh=(float)1;//Input from '*.xnm' file
 float fGWL=(float)1;//Input from database: ground-water table
  
 H2O_ZERO_LAYER

 ddt=(double)dt;

 for (i=0;i<=N;i++)
 {
  P[i]=(double)0;
  R[i]=(double)0;
  S[i]=(double)0;
 }
         

      dCosAlf=(double)1;
//    dxA=x(2)-x(1)
      dxA=(double)DeltaZ;//pSo->fDeltaZ; 
      dxA=(double)2*(double)DeltaZ;//pSo->fDeltaZ; 
      dxB=dxA;
	  dx=dxA/(double)2;

//upper boundary
      ConB=((double)Con[1]+(double)Con[2])/(double)2;
      RT=ConB/dxB+(double)Cap[1]*dx/ddt;
      ST=-ConB/dxB;

      PT=(double)Cap[1]*dx*(double)hNew[1]/ddt
		 -((double)ThN[1]-(double)ThO[1])*dx/ddt-(double)Sink[1]*dx-ConB*dCosAlf;
	  PT+=(double)rTop;

	  if(WLayer==TRUE) // then ! surface reservoir BC
	  {
	   if(hNew[1]>(float)0)
	   {
		RT=RT+(double)1/ddt;
        PT=PT+max((double)hOld[1],(double)0)/ddt;
       } 
       else
       {
        PT=PT+max((double)hOld[1],(double)0)/ddt;
       } 
	  }
	  
	  P[1]=PT;
      S[1]=ST;
	  R[1]=RT;


      //profile
      for (i=2; i<=N-1; i++)
        {
//         dxA=x(i)-x(i-1)
//         dxB=x(i+1)-x(i)
//         dx=(dxA+dxB)/(float)2;
         ConA=((double)Con[i]+(double)Con[i-1])/(double)2;
         ConB=((double)Con[i]+(double)Con[i+1])/(double)2;
         P[i]=(ConA-ConB)*dCosAlf-(double)Sink[i]*dx;
         P[i]+=((double)Cap[i]*(double)hNew[i]
			   -((double)ThN[i]-(double)ThO[i]))*dx/ddt;
         R[i]=(double)Cap[i]*dx/ddt+ConA/dxA+ConB/dxB;
         S[i]=-ConB/dxB;
        }/* for */

//lower boundary
//      dxB=x(N)-x(N-1)
//      dx=dxB/(float)2;
      ConA=((double)Con[N]+(double)Con[N-1])/(double)2;

      if(FreeD==TRUE) // free drainage
	  {
	   R[N]=ConA/dxA;
	   S[N]=-R[N];
	   P[N]=(double)0;
	  }
	  else
	  {
       R[N]=(double)Cap[N]*dx/ddt+ConA/dxA;
       S[N]=-ConA/dxA;
	   if(qGWLF) rBot=Fqh(fGWL,fAqh,fBqh);// outflow function BC
       P[N]=-ConA*dCosAlf-(double)Sink[N]*dx;//dxB instead of dx
       P[N]=ConA*dCosAlf+(double)Sink[N]*dx;//dxB instead of dx
       P[N]+=(double)Cap[N]*dx*(double)hNew[N]/ddt
		     -((double)ThN[N]-(double)ThO[N])*dx/ddt;
	   P[N]-=rBot; // flux BC
	  }


  return 1;
}  //   end  

/************************************************************************/
float Fqh(float z, float a, float b)
{//flux q at lower boundary in relation to ground-water table z
 float q;
 q = a * (float)exp((double)b * fabs((double)z));

 return q;
}

/************************************************************************/
//|||||||||||||||||||||||Solves Matrix Equation
int SolveEq(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 // DECLARE_H2O_POINTER

 int   N=pSo->iLayers-1;
 int   i;



 //Solve tridiagonal linear system
 

 //forward
 if(KodTop>(float)0)
 {
  P[2]-=S[1]*(double)hTop;
 }
 else
 {
  if(fabs(R[1])<rMin) R[1]=rMin; 
  P[2]-=S[1]/R[1]*P[1];
  R[2]-=S[1]/R[1]*S[1];
 }

 for(i=3; i<=N-1; i++)
 {
  if(fabs(R[i-1])<rMin) R[i-1]=rMin; 
  P[i]-=S[i-1]/R[i-1]*P[i-1];
  R[i]-=S[i-1]/R[i-1]*S[i-1];
 } 
 
 if (KodBot>(float)0)
 {
  P[N-1]-=S[N-1]*(double)hBot;
 }
 else
 {
  if(fabs(R[N-1])<rMin) R[N-1]=rMin; 
  P[N]-=S[N-1]/R[N-1]*P[N-1];
  R[N]-=S[N-1]/R[N-1]*S[N-1];
 } 
 
 
 //backward
 if(fabs(R[N-1])<rMin) R[N-1]=rMin; 

 if (KodBot>(float)0)
 {
  hNew[N]=hBot;
  hNew[N-1]=(float)(P[N-1]/R[N-1]);
 }
 else
 {
  hNew[N]=(float)(P[N]/R[N]);
  hNew[N-1]=(float)((P[N-1]-S[N-1]*(double)hNew[N])/R[N-1]);
 }
 

 for (i=N-2; i>1; i--)
 { 
  if(fabs(R[i])<rMin) R[i]=rMin; 
  hNew[i]=(float)((P[i]-S[i]*hNew[i+1])/R[i]);
 }


 if (KodTop>(float)0)
 {
  hNew[1]=hTop;
 }
 else
 {
  if(fabs(R[1])<rMin) R[1]=rMin; 
  hNew[1]=(float)((P[1]-S[1]*hNew[2])/R[1]);
 }
 
 for (i=1; i<N; i++)
 {
	 if(fabs(hNew[i])>rMax)
	 {
      if(hNew[i]>=(float)0)
	  {
	   hNew[i]=(float)fabs(rMax);
	  }
	  else
	  {
      hNew[i]=-(float)fabs(rMax);
	  }
	 }
 }

 return 1;
}  //   end  


/************************************************************************/
int TestCvg(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 DECLARE_H2O_POINTER
 
 float Eps=(float)0;
 float Eph=(float)0;

 //float Tol=(float)1.0e-6;
 float Tol=(float)1.0e-5;
 //float Tol=(float)1.0e-4;
 //float Tol=(float)0.001;//orig.: Tol=0.001;
 //float Tol=(float)0.01;

 //float Tolh=(float)0.01;
 float Tolh=(float)0.001;//orig.: Tolh=0.1;
 //float Tolh=(float)1;
 //float Tolh=(float)10;
 
 int m,i;
 int NumNP=pSo->iLayers-1;

 H2O_ZERO_LAYER

 ItCrF=TRUE;
 ConvgF=TRUE;

 for (i=NumNP; (i>0)&&(ItCrF==TRUE); i--)
 {
     //m=MatNum(i)
     m=i;                                 
     if((hTemp[i]<hSat[m])&&(hNew[i]<hSat[m]))
     {
      //Eps=abs(Cap(i)*(hNew(i)-hTemp(i))/(ThS(m)-ThR(m))/ATh(i))
      Eps=(float)fabs((double)(Cap[i]*(hNew[i]-hTemp[i])/(ThS[i]-ThR[i])));
     } 
     else
     {
      //Eph=abs(hNew(i)-hTemp(i))/Ah(i)
      Eph=(float)fabs((double)(hNew[i]-hTemp[i]));
     }

     if((Eps>Tol) || (Eph>Tolh))
     {
      ItCrF=FALSE;
     }
 
 } /* for */
         
 if(ItCrF==TRUE) 
 {
   for (i=1; i<=NumNP; i++)
   { 
	ThN[i]=ThN[i]+Cap[i]*(hNew[i]-hTemp[i]);
   }
 }          
 else
 { 
   if (Iter<ItMax)
   {
	IGoTo=(int)2;
   }
   else
   {
	if (dt<=dtMin)
	{	
	 ConvgF=FALSE;
	 IGoTo=(int)3; //!give up
     pTi->pSimTime->fTimeAct=(float)(pTi->pSimTime->iSimDuration+(int)5); //!give up
     pTi->pTimeStep->fAct=(float)1;

#ifdef LOGFILE
 {
  // wsprintf(acdummy,"*        wasser1(): ConvgF=%d  => PDE-solver did not converge.",(int)0); 
 Message(0,ERROR_H2O_PDE_ITERATION);
 } 
#endif

	}
	else
    {
     for (i=1; i<=NumNP; i++)
     {
     //Hysterese  if(IHyst.gt.0) Kappa(i)=KappaO(i)
      hNew[i]=hOld[i];
	  hTemp[i]=hOld[i];
      ThN[i]=ThO[i];
     }
      
	 KodTop=KTOld;
	 KodBot=KBOld;
	 dt=(float)max((double)dt/(double)3,(double)dtMin);
	 dtOpt=dt;
	 t=tOld+dt;
     IGoTo=(int)1; // !try smaller time step
    
	} /* else dt<dtMin */
   } /* else Iter<MaxIt */     
  } /* else ItCrF */  

 return 1;
}  //   end  



/************************************************************************/
int TmCont(EXP_POINTER)
{
 DeltaT=pTi->pTimeStep->fAct;

 if (MinStep==TRUE)
 {
  dtMax=(float)min((double)dtMax,min((double)dtMaxC,(double)dtInit));
  dtOpt=dtMax;
  MinStep=FALSE;
 }
 else
 {
  //dtMax=(float)min((double)dtMaxW,(double)dtMaxC);
  dtMax=DeltaT;
 }

 //tFix=(float)min((double)tNew,min((double)tDay,(double)tMax));
 tFix=(float)min((double)tNew,(double)tMax);

 if((Iter<=ItMin)&&((tFix-t)>dMul1*dtOpt))
    dtOpt=(float)min((double)dtMax,(double)dMul1*(double)dtOpt);
 

 if (Iter>=ItMax) dtOpt=(float)max((double)dtMin,(double)dMul2*(double)dtOpt);
 
 dt=(float)min((double)dtOpt,(double)tFix-(double)t);
 dt=(float)min((double)((tFix-t)/(int)((tFix-t)/dt)),(double)dtMax);
 if(((tFix-t)!=dt)&&(dt>(tFix-t)/(float)2)) dt=(tFix-t)/(float)2;

return 1;
}  //   end  


/************************************************************************/

int Veloc(int N,double dCosAlf)
{
//      real*8 Con,ConO
//      dimension hNew(*),x(*),Con(*),ConO(*),v(*)
 float dxA,dxB,ConA,ConB,vA,vB;
 int i;
//      dxA=x(2)-x(1)
      dxA=dxB=DeltaZ;
      ConA=(Con[1]+Con[2])/2;
      v[1]=-ConA*(hNew[2]-hNew[1])/dxA+ConA*(float)dCosAlf;
      for(i=2; i<=N-1; i++)
        {
//        dxB=x(i+1)-x(i)
//        dxA=x(i)-x(i-1)
         ConB=(Con[i]+Con[i+1])/2;
         ConA=(Con[i]+Con[i-1])/2;
         vB=-ConB*((hNew[i+1]-hNew[i])/dxB-(float)dCosAlf);
         vA=-ConA*((hNew[i]-hNew[i-1])/dxA-(float)dCosAlf);
         v[i]=(vA*dxA+vB*dxB)/(dxA+dxB);
        }
//      dxB=x(N)-x(N-1)
      ConB=(Con[N]+Con[N-1])/2;
      v[N]=-ConB*(hNew[N]-hNew[N-1])/dxB+ConB*(float)dCosAlf;

  return 1;
} /*  end  Veloc */


