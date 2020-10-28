/**************************************************************************************/
/*                                                                                    */
/*  module  :   chemistr.C                                                            */       
/*  purpose :   Liefert DLL mit N-Dynamik Modulen                                     */
/*  date  :     18.9.95                                                               */
/*                                                                                    */
/*                     dm          01.8.96 Implementieren der neuen Variablenstruktur */
/*                     ch          14.1.97 Sichtung der dll-Funktionen                */
/**************************************************************************************/

/* Sammlung aller Header-Dateien */
#include  "xinclexp.h"    
#include  "xn_def.h"  

#define SimulationStarts (SimStart(pTi))
#define PlantIsGrowing    ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
#define SimulationEnds   (SimEnd(pTi))

/*************************************************************/
/*                       DLL Funktionen                      */
/*************************************************************/

/* from miner.c: */
int WINAPI _loadds dllCHMineralisation_LEACHN(EXP_POINTER);
int WINAPI _loadds dllABMineralisation_NCSOIL(EXP_POINTER);
int WINAPI _loadds dllABMineralisation_DAISY(EXP_POINTER);
int WINAPI _loadds dllTSMineralisation_NSIM(EXP_POINTER);
int WINAPI _loadds dllABMineralisation_DNDC(EXP_POINTER);
int WINAPI _loadds dllABMineralisation_NITS(EXP_POINTER);
int WINAPI _loadds dllABMineralisation_XN(EXP_POINTER);//ehem. dllABMinOMI
//int WINAPI _loadds dllSGMineralisation_CERES(EXP_POINTER);//aus FAGUS
int WINAPI _loadds dllABMineralisation_XNCERES(EXP_POINTER);
int WINAPI _loadds dllSBMineralisation_DAYCENT(EXP_POINTER);

/* from nitrif.c: */
int WINAPI _loadds dllCHNitrification_LEACHN(EXP_POINTER);
int WINAPI _loadds dllCHNitrification_XN(EXP_POINTER);
int WINAPI _loadds dllEPNitrification_XNCR(EXP_POINTER);
int WINAPI _loadds dllTSNitrification_CERES(EXP_POINTER);
int WINAPI _loadds dllSBNitrification_DAYCENT(EXP_POINTER);
//int WINAPI _loadds dllTSNitrification_NSIM(EXP_POINTER);


/* from denit.c: */
int WINAPI _loadds dllCHDenitrification_LEACHN(EXP_POINTER);
int WINAPI _loadds dllTSDenitrification_CERES(EXP_POINTER);
int WINAPI _loadds dllCHDenitrification_SOILN(EXP_POINTER);
int WINAPI _loadds dllCHDenitrification_DNDC(EXP_POINTER);
int WINAPI _loadds dllCHDenitrification_XN(EXP_POINTER);
int WINAPI _loadds dllCHDenitrification_XNV(EXP_POINTER);
int WINAPI _loadds dllSBDenitrification_DAYCENT(EXP_POINTER);

int WINAPI _loadds dllCHDenSoend1(EXP_POINTER);
int WINAPI _loadds dllCHDenSoend2(EXP_POINTER);
int WINAPI _loadds dllEPN2OProdIBW(EXP_POINTER);

/* from ntrans.c: */
int WINAPI _loadds dllEPSoluteTransport_LEACHN(EXP_POINTER);   
int WINAPI _loadds dllTSSoluteTransport_CERES(EXP_POINTER);
int WINAPI _loadds dllCHSoluteTransport_XN(EXP_POINTER);
int WINAPI _loadds dllEPSoluteTransport_XNPDE(EXP_POINTER);

int WINAPI _loadds dllEPVolatilisation_LEACHN(EXP_POINTER);
int WINAPI _loadds dllTSVolatilisation_CERES(EXP_POINTER);

int WINAPI _loadds dllEPHydrolysis_LEACHN(EXP_POINTER);
int WINAPI _loadds dllTSHydrolysis_NSIM(EXP_POINTER);

int WINAPI _loadds dllCHDeposition_XN(EXP_POINTER);

int WINAPI _loadds dllEPSoilNitrogen_GECROS(EXP_POINTER);


/*************************************************************/
/*                     externe Funktionen                    */
/*************************************************************/

/* from util_fct.c */
extern int SimStart(PTIME);
extern int SimEnd(PTIME);
extern int NewDay(PTIME);
extern int introduce(LPSTR lpName);

/* from miner.c */
extern int WINAPI MinerLEACHN(EXP_POINTER);
extern int WINAPI MinerNCSOIL(EXP_POINTER);
extern int WINAPI MinerDAISY(EXP_POINTER);
//extern int WINAPI MinerCERES(EXP_POINTER);//aus FAGUS
extern int WINAPI MinerXNCERES(EXP_POINTER);//mit C-Umsatz
extern int WINAPI MinerDNDC(EXP_POINTER);
extern int WINAPI MinerNITS(EXP_POINTER);
extern int WINAPI MinerOMI(EXP_POINTER);
extern int WINAPI MinerDAYCENT(EXP_POINTER);

extern int WINAPI HydroLEACHN(EXP_POINTER);
extern int WINAPI HydroNSIM(EXP_POINTER);

extern signed short int WINAPI MineralisationCERES(EXP_POINTER);
extern signed short int WINAPI ImmobilisationCERES(EXP_POINTER);
//extern signed short int WINAPI MinerNSIM(EXP_POINTER);
//extern signed short int WINAPI ImmobNSIM(EXP_POINTER);

extern int WINAPI StandingPoolDecrease(EXP_POINTER);
extern int WINAPI BiomassCoverRelation(EXP_POINTER);

/* from nitrif.c */
extern int WINAPI NitriLEACHN(EXP_POINTER);
extern int WINAPI NitriCERES(EXP_POINTER);
extern int WINAPI NitriDAYCENT(EXP_POINTER);
extern int WINAPI NitriConst(EXP_POINTER);
/* from denit.c */
extern int WINAPI DenitXN(EXP_POINTER, float, float, int, int);
extern int WINAPI DenitLi(EXP_POINTER, int, int);
extern int WINAPI DenitSoend1(EXP_POINTER);
extern int WINAPI DenitSoend2(EXP_POINTER);

extern int WINAPI N2OProdIBW(EXP_POINTER);
extern BOOL bDenitRed;

extern int WINAPI DenitCERES(EXP_POINTER);
//extern int WINAPI DenitNSIM(EXP_POINTER);
extern int WINAPI DenitDAYCENT(EXP_POINTER);

/* from Transp.c */
extern int WINAPI NTransp(EXP_POINTER, int N2O_IN_SOLUTION);
extern int WINAPI NVolat1(EXP_POINTER);
extern int WINAPI NVolat2(EXP_POINTER);
extern int WINAPI EmitAllN2O(EXP_POINTER);
extern int WINAPI NConstDeposition(EXP_POINTER);
 
extern signed short int WINAPI NTransportCERES(EXP_POINTER);

//from water.c
extern int iGECROS;
// from chemist\miner.c
extern int SoilNitrogen_GECROS(EXP_POINTER);
extern int iGECROS;
//from gecros_alloc.c
extern int WINAPI   allocateGECROSVariables();
extern int WINAPI   freeAllocatedGECROSMemory();

static float afNH[MAXSCHICHT],afNO[MAXSCHICHT];

/***********************************************************************************************
 * dll-Funktion:   dllCHMineralisation_LEACHN
 * Beschreibung:   Mineralisation nach LEACHN mit einzulesenden lpx-Raten.
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllCHMineralisation_LEACHN(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) 
 	{
	introduce((LPSTR)"dllCHMineralisation_LEACHN");
	}
         
 BiomassCoverRelation(exp_p);
 StandingPoolDecrease(exp_p);
 MinerLEACHN(exp_p);

 return 1;
}   
                       

/***********************************************************************************************
 * dll-Funktion:   dllABMineralisation_NCSOIL
 * Beschreibung:   Mineralisation nach Li (Kompositionsmodell nach Molina)
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllABMineralisation_NCSOIL(EXP_POINTER)
{  
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi))          
    {
     introduce((LPSTR)"dllCHMineralisation_NCSOIL");
    }  
 MinerNCSOIL(exp_p);

 return 1;
}  

/***********************************************************************************************
 * dll-Funktion:   dllABMineralisation_DAISY
 * Beschreibung:   Mineralisation nach 
 *                 Daisy (Hansen, Jensen,Nielsen, Svendsen, 1990)
 *         ch/gsf                   20.4.95                    
 *
 ***********************************************************************************************/
int WINAPI _loadds dllABMineralisation_DAISY(EXP_POINTER)
{   
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) 
    {
     introduce((LPSTR)"dllCHMineralisation_DAISY");
    }
 
 MinerDAISY(exp_p);

 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllABMineralisation_DNDC
 * Beschreibung:   Mineralisation nach 
 *                 DNDC (Li, 1992)
 *         ab/gsf                   15.06.01                    
 *
 ***********************************************************************************************/
///*
int WINAPI _loadds dllABMineralisation_DNDC(EXP_POINTER)
{   
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) 
    {
     introduce((LPSTR)"dllABMineralisation_DNDC");
    }
 
 MinerDNDC(exp_p);

 return 1;
}
//*/

/***********************************************************************************************
 * dll-Funktion:   dllABMinNITS
 * Beschreibung:   Mineralisation nach 
 *                 ()
 *         ab/gsf                   15.06.01                    
 *
 ***********************************************************************************************/
///*
int WINAPI _loadds dllABMineralisation_NITS(EXP_POINTER)
{   
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) 
    {
     introduce((LPSTR)"dllABMineralisation_NITS");
    }
 
 MinerNITS(exp_p);

 return 1;
}
//*/

/***********************************************************************************************
 * dll-Funktion:   dllABMinXN
 * Beschreibung:   Mineralisation nach 
 *                 ()
 *         ab/gsf                   15.06.01                    
 *
 ***********************************************************************************************/
///*
int WINAPI _loadds dllABMineralisation_XN(EXP_POINTER)
{   
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) 
    {
     introduce((LPSTR)"dllABMineralisation_XN");
    }
 
 MinerOMI(exp_p);

 return 1;
}
//*/

/***********************************************************************************************
 * dll-Funktion:   dllSGMinCERES
 * Beschreibung:   Mineralisation nach 
 *                 ()
 *         sg/gsf                   15.06.01                    
 *
 ***********************************************************************************************/
/*
int WINAPI _loadds dllSGMineralisation_CERES(EXP_POINTER)
{   
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) 
    {
     introduce((LPSTR)"dllSGMineralisation_CERES");
    }
 
 MinerCERES(exp_p);

 return 1;
}
*/

/***********************************************************************************************
 * dll-Funktion:   dllABMinXNCERES
 * Beschreibung:   Mineralisation nach 
 *                 ()
 *         ab/gsf                   15.06.01                    
 *
 ***********************************************************************************************/
///*
int WINAPI _loadds dllABMineralisation_XNCERES(EXP_POINTER)
{   
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) 
    {
     introduce((LPSTR)"dllABMineralisation_XNCERES");
    }
 
 MinerXNCERES(exp_p);

 return 1;
}
//*/

/***********************************************************************************************
 * dll-Funktion:   dllSBMineralisation_DAYCENT()
 * Beschreibung:   Mineralisation nach DAYCENT/CENTURY
 *                 
 * Author      :   sb/hmgu         25.06.0ß                    
 *
 ***********************************************************************************************/
///*
int WINAPI _loadds dllSBMineralisation_DAYCENT(EXP_POINTER)
{   
 DECLARE_COMMON_VAR
 
 int L;

 PCLAYER pCL = pCh->pCLayer->pNext;
 
 if (SimStart(pTi)) 
    {
     introduce((LPSTR)"dllSBMineralisation_DAYCENT");
    }

 if(NewDay(pTi))
 {
 for (L=1;L<=pSo->iLayers-2;L++)
 {
   afNH[L]= pCL->fNH4N;
   afNO[L]= pCL->fNO3N;

   pCL=pCL->pNext;
 }
 }

 MinerDAYCENT(exp_p);

 if(NewDay(pTi))
 {
 pCL = pCh->pCLayer->pNext;
 for (L=1;L<=pSo->iLayers-2;L++)
 {
   afNH[L] = (pCL->fNH4N - afNH[L]);
   afNO[L] = (pCL->fNO3N - afNO[L]);
   pCL->fNH4N   = - afNH[L] + pCL->fNH4N;
   pCL->fNO3N   = - afNO[L] + pCL->fNO3N;
   pCL->fNH4N  += afNH[L] * DeltaT;
   pCL->fNO3N  += afNO[L] * DeltaT;

   pCL=pCL->pNext;
 }
 }
 else
 {
 pCL = pCh->pCLayer->pNext;
 for (L=1;L<=pSo->iLayers-2;L++)
 {
   pCL->fNH4N += afNH[L] * DeltaT;
   pCL->fNO3N += afNO[L] * DeltaT;

   pCL=pCL->pNext;
 }
 }

 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllTSMineralisation_NSIM
 * Beschreibung:   Mineralisation nach Ritchie
 *         ts/gsf                   14.1.97
 ***********************************************************************************************/
//int WINAPI _loadds dllTSMinRitchie(EXP_POINTER)
int WINAPI _loadds dllTSMineralisation_NSIM(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllTSMineralisation_NSIM");// dllTSMinNSIM");
         
 MineralisationCERES(exp_p);
 ImmobilisationCERES(exp_p);
 //MinerNSIM(exp_p);
 //ImmobNSIM(exp_p);

 return 1;
}   
                       


/***********************************************************************************************
 * dll-Funktion:   dllCHDenitrification_LEACHN
 * Beschreibung:   Denitrifikation nach LEACHM mit einzulesenden lpx-Raten.
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllCHDenitrification_LEACHN(EXP_POINTER)
{                              
 DECLARE_COMMON_VAR
 

 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenitrification_LEACHN");


 DenitXN(exp_p, (float)1, (float)0, 7, 3); 	/*  N2/(N2+N2O) = 1, => No N2O
    										REWETTING_ENHANCES_MAX_DEN +1
    										REWETTING_RELEASES_NO3     +2 
    										REWETTING_REDUCES_O2	   +4

    										NO3_KIN_SUPRALINEAR
    										NO3_KIN_MM 	=> Leach Kinetics
    										*/                                     


 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllCHDenitrification_SOILN
 * Beschreibung:   Denitrifikation nach Johnsson 
 *				   mit angenommenem N2:N2O Verhältnis 1:1
 *				   und sofortiger Emission
 *					
 *         ch/gsf                   17.7.97
 ***********************************************************************************************/
int WINAPI _loadds dllCHDenitrification_SOILN(EXP_POINTER)
{                              
 DECLARE_COMMON_VAR
 

 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenitrification_SOILN");

 DenitXN(exp_p, (float)0.5, (float)0, 7, 1); 	/*  N2/(N2+N2O) = 0.5, 
    										REWETTING_ENHANCES_MAX_DEN +1
    										REWETTING_RELEASES_NO3     +2 
    										REWETTING_REDUCES_O2	   +4

    										nur NO3_KIN_MM +1 
    										*/                                     
 EmitAllN2O(exp_p);

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllTSDenitrification_CERES
 * Beschreibung:   Mineralisation nach Ritchie
 *         ts/gsf                   14.1.97
 ***********************************************************************************************/
int WINAPI _loadds dllTSDenitrification_CERES(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllTSDenitrification_CERES");
         
 DenitCERES(exp_p);

 return 1;
}   



/*********************************************************************
 * dll-Funktion:   dllCHDenitrification_XNV
 * Beschreibung:   Denitrifikation wie Denit1 mit Abbau von N2O zu N2 
 *
 *         ch/gsf                   20.11.96
 
 ********************************************************************/
int WINAPI _loadds dllCHDenitrification_XNV(EXP_POINTER)
{
 DECLARE_COMMON_VAR                           
   
 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenitrification_XNV");

 DenitXN(exp_p, (float)0, (float)pCh->pCParam->pNext->fN2ORedMaxR, 7, 1); 	
                                            /*  N2 = 0, N2O -> N2 Reduction
    										REWETTING_ENHANCES_MAX_DEN +1
    										REWETTING_RELEASES_NO3     +2 
    										REWETTING_REDUCES_O2	   +4
    										nur NO3_KIN_MM +1 
    										*/
 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllCHDenitrification_XN
 * Beschreibung:   Denitrifikation nach LeachN mit Aufteilung N2/N2O und 
 *				   Denitrifikationsverstärkung 
 * 				   NO3-Freigabe
 * 				   und O2-Reduzierung
 *				   bei Wiederbefeuchtung
 *
 *         ch/gsf         16.11.96
 ********************************************************************/
int WINAPI _loadds dllCHDenitrification_XN(EXP_POINTER)
{
 DECLARE_COMMON_VAR                           
   
 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenitrification_XN");

 DenitXN(exp_p, (float)0.5,(float)0, 7, 1); 	/*  N2/(N2+N2O) = 0.5, 
    										REWETTING_ENHANCES_MAX_DEN +1
    										REWETTING_RELEASES_NO3     +2 
    										REWETTING_REDUCES_O2	   +4
    										nur NO3_KIN_MM +1             
    										*/
 EmitAllN2O(exp_p);
   										
 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllSBDenitrification_DAYCENT()
 *
 * Beschreibung:   Denitrifikation nach DAYCENT/CENTURY
 * 
 * Author:         sb/hmgu      25.06.09
 ***********************************************************************************************/
int WINAPI _loadds dllSBDenitrification_DAYCENT(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllSBDenitrification_DAYCENT");
         
 DenitDAYCENT(exp_p);

 EmitAllN2O(exp_p);

 return 1;
}   


/***********************************************************************************************
 * dll-Funktion:   dllCHNitrification_LEACHN
 * Beschreibung:   Nitrifikation nach Leach mit einzulesenden lpx-Raten.
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllCHNitrification_LEACHN(EXP_POINTER)
{
 DECLARE_COMMON_VAR                             
 
 if (SimStart(pTi))
 {
  introduce((LPSTR)"dllCHNitrification_LEACHN");
     
  pCh->pCParam->fNitrifNH4ToN2O	 = (float)0; // Anteil des N2O an nitrifiziertem NH4 
 }
    
 NitriLEACHN(exp_p);

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllEPNitrification_XNCR
 * Beschreibung:   Constant nitrification
 *         ep/gsf                   14.1.97
 ***********************************************************************************************/
int WINAPI _loadds dllEPNitrification_XNCR(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllEPNitrification_XNCR");
         
 NitriConst(exp_p);

 return 1;
}   


/***********************************************************************************************
 * dll-Funktion:   dllTSNitrification_CERES
 * Beschreibung:   Mineralisation nach Ritchie
 *         ts/gsf                   14.1.97
 ***********************************************************************************************/
int WINAPI _loadds dllTSNitrification_CERES(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllTSNitrification_CERES");
         
 NitriCERES(exp_p);

 return 1;
}   


/***********************************************************************************************
 * dll-Funktion:   dllCHNitrification_XN
 * Beschreibung:   Nitrifikation nach Leach mit einzulesenden lpx-Raten.
 *				   Zusätzlich: N2O-Produktion während Nitrifikation.
 *
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllCHNitrification_XN(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) 
 {
  introduce((LPSTR)"dllCHNitrification_XN");
        
  pCh->pCParam->fNitrifNH4ToN2O = (float)0.01; // Anteil des N2O an nitrifiziertem NH4
     							
 }
    
 NitriLEACHN(exp_p);

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllSBNitrification_DAYCENT()
 * Beschreibung:   Nitrifikation nach DAYCENT
 *
 * Author      :   sb/hmgu       25.06.09
 ***********************************************************************************************/
int WINAPI _loadds dllSBNitrification_DAYCENT(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllSBNitrification_DAYCENT");
         
 NitriDAYCENT(exp_p);

 return 1;
}   





/***********************************************************************************************
 * dll-Funktion:   dllCHBDenitrification_DNDC
 * Beschreibung:   Denitrifikation nach Li
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllCHDenitrification_DNDC(EXP_POINTER)
{                  
 DECLARE_COMMON_VAR
 
 int Li_N2O_Emission = 0;
 int iRewet = 0;

 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenitrification_DNDC");
    
 DenitLi(exp_p,iRewet, Li_N2O_Emission);

 return 1;
}
 
 
 

/***********************************************************************************************
 * dll-Funktion:   dllCHDenSoend1
 * Beschreibung:   Denitrifikation nach Söndgerath
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllCHDenSoend1(EXP_POINTER)
{                              
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenSoend1");

 DenitSoend1(exp_p);

 return 1;
}
 
/***********************************************************************************************
 * dll-Funktion:   dllCHDenSoend2
 * Beschreibung:   Denitrifikation nach Söndgerath
 *         ch/gsf                   20.4.95
 ***********************************************************************************************/
int WINAPI _loadds dllCHDenSoend2(EXP_POINTER)
{                              
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenSoend2");

 DenitSoend2(exp_p);

 return 1;
}

 
/***********************************************************************************************
 * dll-Funktion:   dllEPSoluteTransport_LEACHN
 ***********************************************************************************************/
int WINAPI _loadds dllEPSoluteTransport_LEACHN(EXP_POINTER)
{                              
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllEPSoluteTransport_LEACHN");

 NTransp(exp_p, 0);

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllCHSoluteTransport_XN
 ***********************************************************************************************/
int WINAPI _loadds dllCHSoluteTransport_XN(EXP_POINTER)
{                              
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllCHSoluteTransport_XN: ");

 NTransp(exp_p, 1);

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllTSSoluteTransport_CERES
 * Beschreibung:   N Transport nach Ritchie
 *         ts/gsf                   14.1.97
 ***********************************************************************************************/
int WINAPI _loadds dllTSSoluteTransport_CERES(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllTSSoluteTransport_CERES");
         
 NTransportCERES(exp_p);

 return 1;
}   

/***********************************************************************************************
 * dll-Funktion:   dllEPSoilNitrogen_GECROS
 * Beschreibung:   Soil Nitrogen Transport and Transformation GECROS
 * Autor:          ep/hmgu   04.07.08
 ***********************************************************************************************/
int WINAPI _loadds dllEPSoilNitrogen_GECROS(EXP_POINTER)
{
 DECLARE_COMMON_VAR
         
 if (SimStart(pTi)) introduce((LPSTR)"dllEPSoilNitrogen_GECROS");

 if (SimulationStarts && (iGECROS <= (int)0))
 {
  allocateGECROSVariables();
  iGECROS =(int)7;
 }
 
 if (SimulationStarts && (iGECROS > (int)0))
 {
   iGECROS = (int)7;
 }

 if (NewDay(pTi)) SoilNitrogen_GECROS(exp_p);

 if (SimulationEnds && (iGECROS == (int)7)) 
 {
  freeAllocatedGECROSMemory();
  iGECROS = (int)0;
 }


 return 1;
}   



/***********************************************************************************************
 * dll-Funktion:   dllEPHydrolysis_LEACHN
 ***********************************************************************************************/
int WINAPI _loadds dllEPHydrolysis_LEACHN(EXP_POINTER)
{
 DECLARE_COMMON_VAR     
         
 if (SimStart(pTi)) introduce((LPSTR)"dllEPHydrolysis_LEACHN");

 HydroLEACHN(exp_p);

 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllEPVolatilisation_LEACHN
 ***********************************************************************************************/
int WINAPI _loadds dllEPVolatilisation_LEACHN(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllEPVolatilisation_LEACHN");

 NVolat1(exp_p);

 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllTSVolatilisation_CERES
 ***********************************************************************************************/
int WINAPI _loadds dllTSVolatilisation_CERES(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllTSVolatilisation_CERES");

 NVolat2(exp_p);

 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllTSHydrolysis_NSIM
 ***********************************************************************************************/
int WINAPI _loadds dllTSHydrolysis_NSIM(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllTSHydrolysis_NSIM");

 HydroNSIM(exp_p);

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllCHDeposition_XN
 ***********************************************************************************************/
int WINAPI _loadds dllCHDeposition_XN(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 if (SimStart(pTi)) introduce((LPSTR)"dllCHDeposition_XN");

 NConstDeposition(exp_p);

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllCHNTransOS
 *				   wie dllCHNTrans, aber ohne gelöstes N2O.
 ********************************************************************/
int WINAPI _loadds dllCHNTransOS(EXP_POINTER)
{
 DECLARE_COMMON_VAR
                              
 if (SimStart(pTi)) introduce((LPSTR)"dllCHNTransOS");

 NTransp(exp_p, 0);

 return 1;
}

/***********************************************************************************************
 * dll-Funktion:   dllEPN2OProdIBW
 * Beschreibung:   N2O Produktion Regression Solling
 *         ep/gsf                   11.05.99
 ***********************************************************************************************/
int WINAPI _loadds dllEPN2OProdIBW(EXP_POINTER)
{                              
 DECLARE_COMMON_VAR

 if (SimStart(pTi))
 {
  introduce((LPSTR)"dllEPN2OProdIBW");
  bDenitRed=FALSE;
 }

 N2OProdIBW(exp_p);
 
 EmitAllN2O(exp_p);
 
 return 1;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *			Varianten obiger DLL-Funktionen:
 *
 *
 
/-* from n_fct.c *-/
extern int WINAPI AddPool(int,EXP_POINTER);
extern int WINAPI Tillage(EXP_POINTER);

/-***********************************************************************************************
 * dll-Funktion:   dllCHDenN2W
 * Beschreibung:   Denitrifikation nach LeachN mit Aufteilung N2/N2O und 
 *					Denitrifikationsverstärkung 
 * 					und NO3-Freigabe
 *					bei Wiederbefeuchtung
 *
 *         ch/gsf                   20.4.95
 ***********************************************************************************************-/
int WINAPI _loadds dllCHDenN2W(EXP_POINTER)
{
   DECLARE_COMMON_VAR
                              
   if (SimStart(pTi)) introduce((LPSTR)"dllCHDenN2W");

    DenitXN(exp_p, (float)0.5, (float)0, 3, 3); 	/-*  N2/(N2+N2O) = 0.5, 
    									   	                    REWETTING_ENHANCES_MAX_DEN +1
    										                    REWETTING_RELEASES_NO3     +2 *-/
   return 1;
}

/-***********************************************************************************************
 * dll-Funktion:   dllCHDenN2W
 * Beschreibung:   Denitrifikation nach LeachN mit Aufteilung N2/N2O und 
 *					echter Michaelis-Menten-Kinetik

 *         ch/gsf                   20.11.96
 ********************************************************************-/
int WINAPI _loadds dllCHDenLeachMM(EXP_POINTER)
{
  DECLARE_COMMON_VAR
                              
  if (SimStart(pTi)) introduce((LPSTR)"dllCHDenLeachMM");

    DenitXN(exp_p, (float)0.5, (float)0, 0, 1); 	/-*  N2/(N2+N2O) = 0.5, 
    										                    nur NO3_KIN_MM +1 *-/
  return 1;
}


/-***********************************************************************************************
 * dll-Funktion:   dllCHDenLeachMMw
 * Beschreibung:   Denitrifikation nach LeachN mit Aufteilung N2/N2O und 
 *					echter Michaelis-Menten-Kinetik

 *         ch/gsf                   20.11.96
 ********************************************************************-/
int WINAPI _loadds dllCHDenLeachMMw(EXP_POINTER)
{ 
  DECLARE_COMMON_VAR
                              
  if (SimStart(pTi)) introduce((LPSTR)"dllCHDenLeachMMw");

    DenitXN(exp_p, (float)0.5, (float)0, 1, 1); 	/-*  N2/(N2+N2O) = 0.5, 
    										                    REWETTING_ENHANCES_MAX_DEN +1
    										                    nur NO3_KIN_MM +1 
    										                *-/
  return 1;
}

/-***********************************************************************************************
 * dll-Funktion:   dllCHDenLiE
 * Beschreibung:   Denitrifikation nach Li, Empirische N2O-Emission nach Li
 *         ch/gsf                   20.4.95
 ***********************************************************************************************-/
int WINAPI _loadds dllCHDenLiE(EXP_POINTER)
{                  
 DECLARE_COMMON_VAR
 
 int Li_N2O_Emission = 1;
                             
 if (SimStart(pTi)) introduce((LPSTR)"dllCHDenLiE(1)"); 
    
 DenitLi(exp_p,Li_N2O_Emission);

 return 1;
}

/-***********************************************************************************************
 * dll-Funktion:   dllCHDenit2
 * Beschreibung:   Denitrifikation nach LeachN mit Aufteilung N2/N2O und 
 *					Denitrifikationsverstärkung 
 * 					und O2-Reduzierung
 *					bei Wiederbefeuchtung
 *
 *         ch/gsf         16.11.96
 ********************************************************************-/
int WINAPI _loadds dllCHDenit2(EXP_POINTER)
{
   DECLARE_COMMON_VAR                           
   
   if (SimStart(pTi)) introduce((LPSTR)"dllCHDenit2");

    DenitXN(exp_p, (float)0.5,(float)0, 5, 1); 	/-*  N2/(N2+N2O) = 0.5, 
    										REWETTING_ENHANCES_MAX_DEN +1
    										REWETTING_REDUCES_O2		+4
    										nur NO3_KIN_MM +1             
    										*-/
    										
  return 1;
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *			Noch zu ueberpruefen sind:
 *
 *
*/


/***********************************************************************************************
 * dll-Funktion:   dllEPSoluteTransport_XNPDE;
 ***********************************************************************************************/
int WINAPI _loadds dllEPSoluteTransport_XNPDE(EXP_POINTER)
{                              
 if (SimStart(pTi)) introduce((LPSTR)"dllEPSoluteTransport_XNPDE (empty now)");

 return 1;
}


/***********************************************************************************************
 * dll-Funktion:   dllEPCH4Imis
 ***********************************************************************************************/
int WINAPI _loadds dllEPCH4Imis(EXP_POINTER)
{                              
 if (SimStart(pTi)) introduce((LPSTR)"dllEPCH4Imis (empty now)");

 return 1;
}


