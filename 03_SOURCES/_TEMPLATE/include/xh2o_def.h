/*********************************************************************************/
/*                                                                               */
/*   H2O_DEF.H                                                                   */
/*   Header for water related routines                                           */
/*                                                                               */
/*                                                                               */
/*   Authors: C.Haberbosch (ch), D.Maier (dm), E.Priesack (ep),                  */
/*                                                                               */
/*            GSF- National Research Center for Environment and Health           */
/*            Institute of Soil Ecology                                          */
/*            P.O.Box 1129                                                       */
/*            D-85758 Neuherberg                                                 */
/*            GERMANY                                                            */
/*                                                                               */
/*   Date: October 1997                                                          */
/*                                                                               */
/*   Changes:                                                                    */
/*                                                                               */
/*********************************************************************************/


/* delay [day] after which ponding will be treated as runoff:                    */
#define PONDING_TIME (float)1.0

/* constant rain rate:                                                           */
#define CONST_RAIN_RATE (float)100.0

/* Tolerable mass balance error [mm] within a time step:                         */
#define ERROR_TOLERANCE (float)1

/* Minimal (not freezing) water content                                          */
#define THETA_MIN (float)0.1

/* function call for pedo transfer function:                                     */
#define PTF(i) (float)PedoTransferCampbell(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP, i)
//#define PTF(i) (float)PedoTransferRawlsBrakensiek(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP, i)
//#define PTF(i) (float)PedoTransferVereecken(dClayF, dSiltF, dSandF, dBD, dCarbonF, dSP, i)


/* function call for hydraulic conductivity:                                     */
///*
#define CONDUCTIVITY(x) (\
                     (float)HCond((double)x,\
                     (double)pWL->fContAct,\
                     (double)pSW->fCondSat,\
                     (double)pSL->fPorosity,\
                     (double)pSW->fContRes,\
                     (double)pSW->fVanGenA,\
                     (double)pSW->fVanGenN,\
                     (double)pSW->fVanGenM,\
                     (double)pSW->fCampA,\
                     (double)pSW->fCampB,\
                     (double)pSW->fPotInflec,\
                     (double)pSW->fContInflec,\
                     (double)pSW->fMinPot,\
                     (double)pWL->fMatPotOld,\
                     (double)pWL->fContOld,\
                     (double)pSW->fVanGenA2,\
                     (double)pSW->fVanGenN2,\
                     (double)pSW->fVanGenM2,\
                     (double)pSW->fBiModWeight1,\
                     (double)pSW->fBiModWeight2))
                     

/* function call for hydraulic conductivity of next layer:                       */
///*
#define NEXT_CONDUCTIVITY(x)  (\
                     (float)HCond((double)x,\
                     (double)pWL->pNext->fContAct,\
                     (double)pSW->pNext->fCondSat,\
                     (double)pSL->pNext->fPorosity,\
                     (double)pSW->pNext->fContRes,\
                     (double)pSW->pNext->fVanGenA,\
                     (double)pSW->pNext->fVanGenN,\
                     (double)pSW->pNext->fVanGenM,\
                     (double)pSW->pNext->fCampA,\
                     (double)pSW->pNext->fCampB,\
                     (double)pSW->pNext->fPotInflec,\
                     (double)pSW->pNext->fContInflec,\
                     (double)pSW->pNext->fMinPot,\
                     (double)pWL->pNext->fMatPotOld,\
                     (double)pWL->pNext->fContOld,\
                     (double)pSW->pNext->fVanGenA2,\
                     (double)pSW->pNext->fVanGenN2,\
                     (double)pSW->pNext->fVanGenM2,\
                     (double)pSW->pNext->fBiModWeight1,\
                     (double)pSW->pNext->fBiModWeight2))
                                          

/* function call for water content:                                             */
///*
#define WATER_CONTENT(x)  (float)WCont((double)x,\
                     (double)pWL->fContAct,\
                     (double)pSW->fCondSat,\
                     (double)pSW->fContSat,\
                     (double)pSW->fContRes,\
                     (double)pSW->fVanGenA,\
                     (double)pSW->fVanGenN,\
                     (double)pSW->fVanGenM,\
                     (double)pSW->fCampA,\
                     (double)pSW->fCampB,\
                     (double)pSW->fPotInflec,\
                     (double)pSW->fContInflec,\
                     (double)pSW->fMinPot,\
                     (double)pWL->fMatPotOld,\
                     (double)pWL->fContOld,\
                     (double)pSW->fVanGenA2,\
                     (double)pSW->fVanGenN2,\
                     (double)pSW->fVanGenM2,\
                     (double)pSW->fBiModWeight1,\
                     (double)pSW->fBiModWeight2)

/* function call for matrix potential:  */
///*
#define MATRIX_POTENTIAL(x) (float) MPotl((double)pWL->fMatPotAct,\
                     (double)x,\
                     (double)pSW->fCondSat,\
                     (double)pSW->fContSat,\
                     (double)pSW->fContRes,\
                     (double)pSW->fVanGenA,\
                     (double)pSW->fVanGenN,\
                     (double)pSW->fVanGenM,\
                     (double)pSW->fCampA,\
                     (double)pSW->fCampB,\
                     (double)pSW->fPotInflec,\
                     (double)pSW->fContInflec,\
                     (double)pSW->fMinPot,\
                     (double)pWL->fMatPotOld,\
                     (double)pWL->fContOld,\
                     (double)pSW->fVanGenA2,\
                     (double)pSW->fVanGenN2,\
                     (double)pSW->fVanGenM2,\
                     (double)pSW->fBiModWeight1,\
                     (double)pSW->fBiModWeight2)

/* function call for water capacity:  */
///*
#define CAPACITY(x) (float)DWCap((double)x,\
                     (double)pWL->fContAct,\
                     (double)pSW->fCondSat,\
                     (double)pSW->fContSat,\
                     (double)pSW->fContRes,\
                     (double)pSW->fVanGenA,\
                     (double)pSW->fVanGenN,\
                     (double)pSW->fVanGenM,\
                     (double)pSW->fCampA,\
                     (double)pSW->fCampB,\
                     (double)pSW->fPotInflec,\
                     (double)pSW->fContInflec,\
                     (double)pSW->fMinPot,\
                     (double)pWL->fMatPotOld,\
                     (double)pWL->fContOld,\
                     (double)pSW->fVanGenA2,\
                     (double)pSW->fVanGenN2,\
                     (double)pSW->fVanGenM2,\
                     (double)pSW->fBiModWeight1,\
                     (double)pSW->fBiModWeight2)

/*********************************************************************************/
/* section of abbreviations:                                                     */
/*********************************************************************************/

//  ML, PW, SL, SW, WB, WL
#define DECLARE_H2O_POINTER \
              PSLAYER     pSL; \
              PSWATER     pSW;\
              PWLAYER     pWL;\
              PWBALANCE   pWB;
              // PLAYERROOT  pLR = pPl->pRoot->pLayerRoot;


#define H2O_ZERO_LAYER \
        pSL = pSo->pSLayer,\
        pSW = pSo->pSWater,\
        pWL = pWa->pWLayer,\
        pWB = pWa->pWBalance,\
        iLayer = 0;                   

//        pLR = pPl->pRoot->pLayerRoot,

#define H2O_FIRST_LAYER \
        pSL = pSo->pSLayer->pNext,\
        pSW = pSo->pSWater->pNext,\
        pWL = pWa->pWLayer->pNext,\
        iLayer = 1;                   
//        pLR = pPl->pRoot->pLayerRoot->pNext,
        
#define H2O_UNTIL_BEFORE_LAST \
         ((pSL->pNext != NULL)&&\
          (pSW->pNext != NULL)&&\
          (pWL->pNext != NULL));
//          (pLR->pNext != NULL));
          
#define H2O_UNTIL_LAST \
         ((pSL != NULL)&&\
          (pSW != NULL)&&\
          (pWL != NULL));
//          (pLR != NULL)&&
          
#define NEXT_H2O_LAYER \
           pSL = pSL->pNext,\
           pSW = pSW->pNext,\
           pWL = pWL->pNext,\
           iLayer++    
//           pLR = pLR->pNext,

//  layers  1 to n-1 :
#define H2O_SOIL_LAYERS  H2O_FIRST_LAYER H2O_UNTIL_BEFORE_LAST NEXT_H2O_LAYER

//  layers  0 to n :
#define H2O_ALL_LAYERS  H2O_ZERO_LAYER H2O_UNTIL_LAST NEXT_H2O_LAYER

//  layers  1 to n :
#define H2O_ALL_BUT_NOT_FIRST  H2O_FIRST_LAYER H2O_UNTIL_LAST NEXT_H2O_LAYER
                               

/* from h2o_fct.c */
/* Hydraulic Functions */
typedef double  (* PFCT)(double,double,double,double,double,
                         double,double,double,double,double,
                         double,double,double,double,double,
                         double,double,double,double,double);


extern PFCT		WCont,
				HCond,
				DWCap,
				MPotl;


extern double BetaI(double,double,double);
extern double Beta(double,double);
extern double BetaCF(double,double,double);
extern double GammLN(double); 

extern double DDummy(double,double,double,double,double,double,double,double,double,double,
                     double,double,double,double,double,double,double,double,double,double);
extern double FDummy(double,double,double,double,double,double,double,double,double,double,
                     double,double,double,double,double,double,double,double,double,double);


typedef double  (* PCF)(double,double,double,double,double,
                         double,double,double,double,double,
                         double,double,double,double,double,
                         double,double,double,double,double);

extern PCF  AFct,BFct,CFct,DFct,EFct,FFct;
