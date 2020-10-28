/*********************************************************************************/
/*                                                                               */
/*   N_DEF.H                                                                   */
/*   Header for chemistry related routines                                           */
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
/*   Date: March 1997                                                            */
/*                                                                               */
/*   Changes:                                                                    */
/*                                                                               */
/*********************************************************************************/

typedef struct ncorr  *LPNCORR;
struct ncorr  {
        float             DNrel,DNO3rel,DN2Orel;
        float             Feucht;
        float             Temp;
        float             NO3;
        float             ph,ph2;
        float             Csol;
        float             Lehm; 
        float             Frost; 
        float             WiederBefeucht;
        float             Rate;
              };     

/*********************************************************************************/
/* section of abbreviations:                                                     */
/*********************************************************************************/

// CL, CP, HL, SL, SW, WL 
#define DECLARE_N_POINTER \
            PSLAYER      pSL;\
            PCLAYER      pCL;\
            PCPROFILE    pCP;\
            PCPARAM      pPA;\
            PSWATER      pSW;\
            PWLAYER      pWL;\
            PHLAYER      pHL;\
            PNFERTILIZER pNF;


#define DECLARE_POINTER DECLARE_N_POINTER

// Temporary!!! Use these variables for output of special N2O factors
// (the variable fBioSlowMaxGrowR is not used anywhere else.)
#define DENIT_FROST_FACTOR pCh->pCParam->pNext->pNext->pNext->fBioSlowMaxGrowR
#define DENIT_REWET_FACTOR pCh->pCParam->pNext->pNext->fBioSlowMaxGrowR

// **********************************************************************************
//    Hilfsfunktionen
// **********************************************************************************


// **********************************************************************************
//    LongPointer und Variablen-Abkürzungen
// **********************************************************************************


// iLayer, CL, CP, HL, SL, SW, WL, PA, HL
#define  N_FIRSTLAYER  iLayer=1,\
              pSL = pSo->pSLayer->pNext,\
              pCL = pCh->pCLayer->pNext,\
              pSW = pSo->pSWater->pNext,\
              pWL = pWa->pWLayer->pNext,\
              pPA = pCh->pCParam->pNext,\
              pHL = pHe->pHLayer->pNext;
              
                     
#define  N_STOPLAYER \
             ((iLayer<pSo->iLayers-1)&&\
              (pSL->pNext!=NULL)&&\
              (pCL->pNext!=NULL)&&\
              (pSW->pNext!=NULL)&&\
              (pWL->pNext!=NULL)&&\
              (pPA->pNext!=NULL)&&\
              (pHL->pNext!=NULL));
              
#define  N_NEXTLAYER \
              iLayer++,\
              pSL = pSL->pNext,\
		      pCL = pCL->pNext,\
		      pSW = pSW->pNext,\
		      pWL = pWL->pNext,\
		      pPA = pPA->pNext,\
		      pHL = pHL->pNext

#define  N_ZERO_LAYER  \
              pSL = pSo->pSLayer,\
              pCL = pCh->pCLayer,\
              pCP = pCh->pCProfile,\
              pPA = pCh->pCParam,\
              pSW = pSo->pSWater,\
              pWL = pWa->pWLayer,\
              pHL = pHe->pHLayer,\
              pNF = pMa->pNFertilizer;              

#define  N_SOIL_LAYERS         N_FIRSTLAYER N_STOPLAYER N_NEXTLAYER 

                                            
