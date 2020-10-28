/*******************************************************************
 *
 *              Defines für Temp
 *
 * Name        : temp_def.h
 * Autor       : Christian Haberbosch
 * Beschreibung: Sammlung der im Projekt temp benötigten Defines
 * Stand       : 10.10.95
 ******************************************************************/


// aus expertn\chris\util\utility.c
extern int NewDay(PTIME);
extern int SimStart(PTIME);      

/*-------EXPMATH.C ----------------*/
extern float   abspower(float x, float y);
extern double  abspowerDBL(double x, double y);

#define THETA_MIN (float)0.1 

#define FREEZING_OR_THAWING (pWa->pWLayer->fIce > EPSILON)

#define DENSITY_WATER   (float)1     /* Dichte von Wasser [kg/l] */
#define DENSITY_ICE     (float)0.92  /* Dichte von Eis    [kg/l] */
#define DENSITY_MINERAL (float)2.65  /* Dichte von Mineralien   [kg/l] */
#define DENSITY_HUMUS   (float)1.30  /* Dichte von org. Substanz   [kg/l] */
    
#define SPECCAP_WATER   (float)4192  /* Spez. Waermekap. von Wasser      [J/(kg K)] */
#define SPECCAP_ICE     (float)2050  /* Spez. Waermekap. von Eis         [J/(kg K)] */
#define SPECCAP_MINERAL (float)750   /* Spez. Waermekap. von Mineralien  [J/(kg K)] */
#define SPECCAP_HUMUS   (float)1926  /* Spez. Waermekap. Soil organic matter  [J/(kg K)] */

#define LAT_HEAT_MELT   (float)334       /* Spezifische Schmelzwaerme (Kuchling, p. 603) */
                                         /* [kJ/kg]        */

//#define TEMP_OUTPUT  (NewDay(lpz))&&((lpx->DllPlot == 0)||(lpx->DllPlot == 9))              
#define TEMP_OUTPUT  ((pGr->iDllGraphicNum == 0)||(pGr->iDllGraphicNum == 9))              

#define PLOT_DT2 1
                // 1: PlotIce
                // 2: PlotBalance   


/*
#define DECLARE_TEMP_VAR \
        int   iLayer; \
        int   iLayerAnz = pSo->iLayers;\
        float DeltaT      = pTi->pTimeStep->fAct;\
        float DeltaTOld   = pTi->pTimeStep->fOld;\
        float DeltaZ      = pSo->fDeltaZ;\
        LPSTR lpOut       = (LPSTR)"";\
        float DayTime     = pTi->pSimTime->fTimeDay; \
        float SimTime     = pTi->pSimTime->fTimeAct;
*/

// HB, HL, SL, SW, WL
#define DECLARE_TEMP_POINTER \
        PSLAYER           pSL;\
        PSWATER           pSW;\
        PWLAYER           pWL;\
        PHLAYER           pHL; \
        PHBALANCE         pHB;


#define TEMP_ZERO_LAYER \
        pSL = pSo->pSLayer,\
        pSW = pSo->pSWater,\
        pWL = pWa->pWLayer,\
        pHL = pHe->pHLayer,\
        pHB = pHe->pHBalance,\
        iLayer = 0;                   


#define TEMP_FIRST_LAYER \
        pSL = pSo->pSLayer->pNext,\
        pSW = pSo->pSWater->pNext,\
        pWL = pWa->pWLayer->pNext,\
        pHL = pHe->pHLayer->pNext,\
        pHB = pHe->pHBalance,\
        iLayer = 1;                   
        
// HL, SL, SW, WL
#define TEMP_UNTIL_BEFORE_LAST \
        ((pSL->pNext != NULL)&&\
         (pSW->pNext != NULL)&&\
         (pWL->pNext != NULL)&&\
         (pHL->pNext != NULL));


#define TEMP_UNTIL_LAST \
        ((pSL != NULL)&&\
         (pSW != NULL)&&\
         (pWL != NULL)&&\
         (pHL != NULL));

          
#define NEXT_TEMP_LAYER \
        pSL = pSL->pNext,\
        pSW = pSW->pNext,\
        pWL = pWL->pNext,\
        pHL = pHL->pNext,\
        iLayer++    


// Alle hier relevanten Zeiger werden von Schicht 1 bis n-1 benutzt:
#define TEMP_SOIL_LAYERS  TEMP_FIRST_LAYER TEMP_UNTIL_BEFORE_LAST NEXT_TEMP_LAYER

// Alle hier relevanten Zeiger werden von Schicht 0 bis n-1 benutzt:
#define TEMP_ALL_BUT_NOT_LAST  TEMP_ZERO_LAYER TEMP_UNTIL_BEFORE_LAST NEXT_TEMP_LAYER
                               
// Alle hier relevanten Zeiger werden von Schicht 0 bis n benutzt:
#define TEMP_ALL_LAYERS  TEMP_ZERO_LAYER TEMP_UNTIL_LAST NEXT_TEMP_LAYER

// Alle hier relevanten Zeiger werden von Schicht 1 bis n benutzt:
#define TEMP_ALL_BUT_NOT_FIRST  TEMP_ZERO_LAYER TEMP_UNTIL_BEFORE_LAST NEXT_TEMP_LAYER
                               
                               
                            
                            