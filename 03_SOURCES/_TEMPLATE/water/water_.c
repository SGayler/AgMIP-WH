/**********************************************************************************/
/*                                                                                */
/*  module  :   WATER.C                                                           */
/*  purpose :   Liefert DLL mit Wasserhaushalts-Modulen                           */
/*  date  :     20.9.95                                                           */
/*              15.4.96   dm, Implementierung der neuen Variablenstruktur         */
/*                                                                                */
/**********************************************************************************/

// Sammlung aller Header-Dateien:
#include  "xinclexp.h"   
#include  "xh2o_def.h"  
  
#define SimulationStarts (SimStart(pTi))
#define PlantIsGrowing    ((pPl->pDevelop->bPlantGrowth==TRUE)&&(pPl->pDevelop->bMaturity==FALSE))
#define SimulationEnds   (SimEnd(pTi))


// aus util_fct.c
extern int SimStart(PTIME);
extern int SimEnd(PTIME);
extern int NewDay(PTIME);
extern int NewTenthDay(PTIME);
extern int introduce(LPSTR lpName);

// from water\wasser1.c
extern int WINAPI Wasser1(EXP_POINTER,int);
// from water\wasser1h.c
extern int WINAPI Water_Hydrus_6(EXP_POINTER,int);
// from water\steady.c
extern int WINAPI Water_Steady(EXP_POINTER);
// from water\water2.c
extern signed short int WINAPI WasserflussRitchie(EXP_POINTER);
extern signed short int WINAPI UngesaettigterWasserflussRitchie(EXP_POINTER);

extern int iGECROS;
extern int jGECROS;
// from water\water3.c
extern int SoilWaterFlow_GECROS(EXP_POINTER);
//from gecros_alloc.c
extern int WINAPI   allocateGECROSVariables();
extern int WINAPI   freeAllocatedGECROSMemory();


// from water\runoff.c
extern signed short int WINAPI RunOffSCS(EXP_POINTER);
extern signed short int WINAPI Hangneigung(EXP_POINTER);
extern signed short int WINAPI RunOff_Pe(EXP_POINTER);

//from snow.c
extern int WINAPI Snow(EXP_POINTER);
extern int WINAPI Snow2(EXP_POINTER);

// from water\evapotra.c
extern int WINAPI aktevap1(EXP_POINTER);
extern int WINAPI potevap1(EXP_POINTER);
extern int WINAPI potevap2(EXP_POINTER);
extern int WINAPI potevap3(EXP_POINTER,float x,float y);
extern int WINAPI potevapotraPenman(EXP_POINTER);
extern int WINAPI potevapotraPan(EXP_POINTER);
extern int WINAPI potevapotraHaude(EXP_POINTER);
extern int WINAPI potevapotraPenMonteith(EXP_POINTER);
extern int WINAPI potevapotraPenman_SPASS(EXP_POINTER);
extern int WINAPI potevapotraCanada(EXP_POINTER);
extern int WINAPI potevapYin(EXP_POINTER);
extern int WINAPI aktevapYin(EXP_POINTER);
extern signed short int WINAPI potEvapoTranspirationRitchie(EXP_POINTER);
extern signed short int WINAPI potEvaporationRitchie(EXP_POINTER);
extern signed short int WINAPI aktEvaporationRitchie(EXP_POINTER);
extern int WINAPI Interception(EXP_POINTER);
extern float WINAPI CropCoeff(EXP_POINTER);
extern int WINAPI DualCropCoeff(EXP_POINTER,float*,float*);


/***********************************************/
/*               dll-Funktionen                */              
/***********************************************/
int WINAPI _loadds dllTSRunoffSCS(EXP_POINTER);
int WINAPI _loadds dllTSRunoffSlopeSCS(EXP_POINTER);
int WINAPI _loadds dllCSRunoff(EXP_POINTER);

int WINAPI _loadds dllEPWaterHydrus(EXP_POINTER);
int WINAPI _loadds dllEPWaterPDESolver(EXP_POINTER);
int WINAPI _loadds dllTSWaterRitchie(EXP_POINTER);
int WINAPI _loadds dllCHWaterLeach(EXP_POINTER);
int WINAPI _loadds dllEPWaterSteady(EXP_POINTER);
int WINAPI _loadds dllEPWaterGECROS(EXP_POINTER);

int WINAPI _loadds dllEPBotBCWaterTable(EXP_POINTER);
int WINAPI _loadds dllEPBotBCFreeDrain(EXP_POINTER);
int WINAPI _loadds dllEPBotBCZeroFlux(EXP_POINTER);
int WINAPI _loadds dllEPBotBCLysimeter(EXP_POINTER);

//snow
int WINAPI _loadds dllEPSnow(EXP_POINTER);
int WINAPI _loadds dllCSSnow(EXP_POINTER);
int WINAPI _loadds dllCSSnow2(EXP_POINTER);

int WINAPI _loadds dllCHPotETPenman(EXP_POINTER);
int WINAPI _loadds dllEPPotETRitchie(EXP_POINTER);
int WINAPI _loadds dllCHPotETPenmanCrop(EXP_POINTER);
int WINAPI _loadds dllEPPotETPenMon(EXP_POINTER);
int WINAPI _loadds dllEPPotETPenMonCrop(EXP_POINTER);
int WINAPI _loadds dllEPPotETHaude(EXP_POINTER);
int WINAPI _loadds dllEPPotETCanada(EXP_POINTER);
int WINAPI _loadds dllTSPotETRitchie(EXP_POINTER);
int WINAPI _loadds dllEPPotETPenman_SPASS(EXP_POINTER);
int WINAPI _loadds dllEPPotETPan(EXP_POINTER);
int WINAPI _loadds dllEPPotETPanCrop(EXP_POINTER);
int WINAPI _loadds dllEPPotETYin(EXP_POINTER);


int WINAPI _loadds dllEPPotEvap01(EXP_POINTER);
int WINAPI _loadds dllEPPotEvapRitchie(EXP_POINTER);
int WINAPI _loadds dllCKPotEvap03(EXP_POINTER);

int WINAPI _loadds dllCHAktEvap01(EXP_POINTER);
int WINAPI _loadds dllTSAktEvapRitchie(EXP_POINTER);
int WINAPI _loadds dllEPAktEvapYin(EXP_POINTER);
/**********************************************************************************/
/* dll-Funktion:   dllCHWaterLeach                                                    */
/* Beschreibung:   Wasserhaushalt nach Leach                                      */
/*                 ep                                                             */
/*                 ch/gsf   4.8.95                                                */
/**********************************************************************************/
int WINAPI _loadds dllCHWaterLeach(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCHWaterLeach");
        }
    #endif
        
    Wasser1(exp_p, 0);

return 1;
}   // Ende Berechnung



/**********************************************************************************/
/* dll-Funktion:   dllEPWaterHydrus                                               */
/* Beschreibung:   Wasserhaushalt nach Hydrus                                     */
/*                 ep/gsf   4.3.97                                                */
/**********************************************************************************/
int iWaterMobImm;

int WINAPI _loadds dllEPWaterHydrus(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPWaterHydrus");
        }
    #endif

//MOB_IMM = (int)0;
Water_Hydrus_6(exp_p,iWaterMobImm);


return 1;
}   // Ende Berechnung

/**********************************************************************************/
/* dll-Funktion:   dllEPWaterPDESolver                                            */
/* Beschreibung:   ?                                                              */
/*                 Ch. Bauer: dummy, da in ExpertN referenziert                   */
/**********************************************************************************/
int WINAPI _loadds dllEPWaterPDESolver(EXP_POINTER)
{ 
 DECLARE_COMMON_VAR
  #ifdef LOGFILE                            
    if (SimStart(pTi))
    {
      introduce((LPSTR)"dllEPWaterPDESolver");
    }
  #endif

	//Wasser1(exp_p, 1);
       
  return 1;
}   // Ende Berechnung

/**********************************************************************************/
/* dll-Funktion:   dllEPWaterSteady                                               */
/* Beschreibung:                                                                  */
/*                 ep/gsf   4.3.97                                                */
/**********************************************************************************/
int WINAPI _loadds dllEPWaterSteady(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPWaterSteady");
        }
    #endif
        
    Water_Steady(exp_p);

return 1;
}   // Ende Berechnung


/**********************************************************************************/
/* dll-Funktion:   dllEPWaterGECROS                                               */
/* Beschreibung:                                                                  */
/* Autor:          ep/hmgu  4.7.08                                                */
/**********************************************************************************/
int WINAPI _loadds dllEPWaterGECROS(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPWaterGECROS");
        }
    #endif

    if (SimulationStarts && (iGECROS <= (int)0))
	{
	 allocateGECROSVariables();
	 iGECROS=(int)2;
	}

	if (SimulationStarts && (iGECROS > (int)0))
    {
     iGECROS = (int)6;
	 jGECROS = (int)1;
    }

    SoilWaterFlow_GECROS(exp_p);

    if (SimulationEnds && (iGECROS == (int)6))
	{
	 freeAllocatedGECROSMemory();
	 iGECROS = (int)0;
	}

return 1;
}   // Ende Berechnung
/**********************************************************************************/
/* dll-Funktion:   dllCSSnow                                            */
/* Beschreibung:                                        */
/*                 cs/gsf   11.2.00                                                */
/**********************************************************************************/
int WINAPI _loadds dllCSSnow(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCSSnow");
        }
    #endif
        
Snow(exp_p);

return 1;
}   // Ende Berechnung

/**********************************************************************************/
/* dll-Funktion:   dllCSSnow2                                            */
/* Beschreibung:                                        */
/*                 cs/gsf   11.2.00                                                */
/**********************************************************************************/
int WINAPI _loadds dllCSSnow2(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCSSnow2");
        }
    #endif
        
Snow2(exp_p);

return 1;
}   // Ende Berechnung

/**********************************************************************************/
/* dll-Funktion:   dllTSWaterRitchie                                              */
/* Beschreibung:   Wasserhaushalt nach Ritchie                                    */
/*                                                                                */
/*                 ts   14.1.97                                                   */
/**********************************************************************************/
int WINAPI _loadds dllTSWaterRitchie(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSWaterRitchie");
        }
    #endif
        
    WasserflussRitchie(exp_p);
    UngesaettigterWasserflussRitchie(exp_p);

return 1;
}   // Ende Berechnung

/**********************************************************************************/
/* dll-Funktion:   dllTSRunoffSCS                                                 */
/* Beschreibung:   Wasserhaushalt nach  Soil Conservation Service                 */
/*                                                                                */
/*                 ts   14.1.97                                                   */
/**********************************************************************************/
int WINAPI _loadds dllTSRunoffSCS(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSRunoffSCS");
        }
    #endif
        
    RunOffSCS(exp_p);     
    

return 1;
}   // Ende Berechnung


/**********************************************************************************/
/* dll-Funktion:   dllTSRunoffSlopeSCS                                               */
/* Beschreibung:   Wasserhaushalt nach  Soil Conservation Service                 */
/*                                                                                */
/*                 ts   14.1.97                                                   */
/**********************************************************************************/
int WINAPI _loadds dllTSRunoffSlopeSCS(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSRunoffSlopeSCS");
        }
    #endif
        
    RunOffSCS(exp_p);
    Hangneigung(exp_p);    
    
return 1;
}   // Ende Berechnung


/**********************************************************************************/
/* dll-Funktion:   dllCSRunoff                                                    */
/* Beschreibung:   Oberflaechenabfluss nach Dyck & Peschke                        */
/*                                                                                */
/*                 cs  02.03.00                                                   */
/**********************************************************************************/
int WINAPI _loadds dllCSRunoff(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSRunoffSlopeSCS");
        }
    #endif
        
    RunOff_Pe(exp_p);
    //Hangneigung(exp_p);    

return 1;
}   // Ende Berechnung


/**********************************************************************************/
/* dll-Funktion:   dllEPBotBCWaterTable                                           */
/* Beschreibung:   Bottom Boundary Condition: Water Table                         */
/*                                                                                */
/*                 ep  20.06.08                                                   */
/**********************************************************************************/
int WINAPI _loadds dllEPBotBCWaterTable(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPBotBCWaterTable");
        }
    #endif
        
    if (SimStart(pTi))
	{
		//SG 20110419:
		if(pCl->pWeather->fWaterTable > (float)0)
			pWa->iBotBC = (int)5;
		else if ((pWa->fGrdWatLevel > (float)0)&&(pWa->fGrdWatLevel <= pSo->fDepth))     
            pWa->iBotBC = (int)2;
	    else
	    {
            pWa->fGrdWatLevel = pSo->fDepth;
            pWa->iBotBC       = (int)2;
	    }

	  //if ((pCl->pWeather->fWaterTable > (float)0)&&(pCl->pWeather->fWaterTable <= pSo->fDepth)) 
	  //  pWa->iBotBC = (int)5;
	  //else if ((pWa->fGrdWatLevel > (float)0)&&(pWa->fGrdWatLevel <= pSo->fDepth))     
      //  pWa->iBotBC = (int)2;
	  //else
	  //{
      //  pWa->fGrdWatLevel = pSo->fDepth;
      //  pWa->iBotBC       = (int)2;
	  //}

	}//if SimStart

return 1;
}   // Ende


/**********************************************************************************/
/* dll-Funktion:   dllEPBotBCFreeDrain                                            */
/* Beschreibung:   Bottom Boundary Condition: Water Table                         */
/*                                                                                */
/*                 ep  20.06.08                                                   */
/**********************************************************************************/
int WINAPI _loadds dllEPBotBCFreeDrain(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPBotBCFreeDrain");
        }
    #endif
        
    if (SimStart(pTi)) pWa->iBotBC = (int)1;
	

return 1;
}   // Ende


/**********************************************************************************/
/* dll-Funktion:   dllEPBotBCZeroFlux                                             */
/* Beschreibung:   Bottom Boundary Condition: Zero Flux                           */
/*                                                                                */
/*                 ep  20.06.08                                                   */
/**********************************************************************************/
int WINAPI _loadds dllEPBotBCZeroFlux(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPBotBCZeroFlux");
        }
    #endif
        
    if (SimStart(pTi)) pWa->iBotBC = (int)3;
	

return 1;
}   // Ende


/**********************************************************************************/
/* dll-Funktion:   dllEPBotBCLysimeter                                            */
/* Beschreibung:   Bottom Boundary Condition: Lysimeter                           */
/*                                                                                */
/*                 ep  20.06.08                                                   */
/**********************************************************************************/
int WINAPI _loadds dllEPBotBCLysimeter(EXP_POINTER)
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPBotBCLysimeter");
        }
    #endif
        
    if (SimStart(pTi)) pWa->iBotBC = (int)4;
	

return 1;
}   // Ende

                                 
/**********************************************************************************/
/* dll-Funktion:   dllEPPotETPAN                                              */
/*                 ep/gsf            11.4.02                                      */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotETPan(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotETPAN");
        }
    #endif
  
 potevapotraPan(exp_p);
  
 if (NewDay(pTi)) Interception(exp_p);

return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllEPPotETPAN                                              */
/*                 ep/gsf            11.4.02                                      */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotETPanCrop(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 float f1;

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotETPAN");
        }
    #endif
  
 potevapotraPan(exp_p);
 
 if (pPl != NULL)
 {
  f1=CropCoeff(exp_p); 
  pWa->fPotETdt *= f1;
  if (NewDay(pTi)) pWa->fPotETDay *= f1;    
 }

 if (NewDay(pTi)) Interception(exp_p);
  
return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllEPPotETYin                                                  */
/* Beschreibung:   potentielle Evaporation nach Yin/GECROS                        */
/*                 ep/hmgu           04.07.08                                     */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotETYin(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotETYin");
        }
    #endif
/*
 if ((pPl != NULL)&& PlantIsGrowing)
 {
  return 1;
 }
 else
 {
  potevapYin(exp_p);
 }
*/
		
  potevapYin(exp_p);

  if (NewDay(pTi)) Interception(exp_p);

return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllCHPotETPenman                                                 */
/* Beschreibung:   Wasserhaushalt nach Leach                                      */
/*                 ch/gsf            28.8.95                                      */
/**********************************************************************************/                                    
int WINAPI _loadds dllCHPotETPenman(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCHPotETPenman");
        }
    #endif
  
 potevapotraPenman(exp_p);

 //if (NewDay(pTi)) Interception(exp_p);
  
return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllTSPotETRitchie                                            */
/* Beschreibung:   potentielle Evaporation nach Ritchie                           */
/*                 ts/tum            14.1.97                                      */
/**********************************************************************************/                                    
int WINAPI _loadds dllTSPotETRitchie(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSPotETRitchie");
        }
    #endif

    potEvapoTranspirationRitchie(exp_p);

	//if (NewDay(pTi)) Interception(exp_p);
  
return 1;
}
/**********************************************************************************/
/* dll-Funktion:   dllEPPotEvapRitchie                                            */
/* Beschreibung:   potentielle Evaporation nach Ritchie                           */
/*                 ts/tum, ep/gsf    14.1.97, 22.7.99                             */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotEvapRitchie(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotEvapRitchie");
        }
    #endif

    potevap2(exp_p);
	//potEvaporationRitchie(exp_p);

	if (NewDay(pTi)) Interception(exp_p);
  
return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllCHAktEvap01                                                  */
/* Beschreibung:   Wasserhaushalt nach Leach                                      */
/*                 ep/gsf            28.8.95                                      */
/**********************************************************************************/
int WINAPI _loadds  dllCHAktEvap01(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
       if (SimStart(pTi))
          {
           introduce((LPSTR)"dllCHAktEvap01");
          }
    #endif

    aktevap1(exp_p);

return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllEPAktEvapYin                                                */
/* Beschreibung:   Wasserhaushalt nach GECROS                                     */
/*                 ep/hmgu            04.07.08                                    */
/**********************************************************************************/
int WINAPI _loadds  dllEPAktEvapYin(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
       if (SimStart(pTi))
          {
           introduce((LPSTR)"dllEPAktEvapYin");
          }
    #endif

 //if ((pPl != NULL)&& PlantIsGrowing)
 {
  //pWa->pEvap->fActR = (float)0;  
  //return 1;
 }
 //else
 {
  aktevapYin(exp_p);
 }

return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllTSAktEvapRitchie                                            */
/* Beschreibung:   potentielle Evaporation nach Ritchie                           */
/*                 ts/gsf            14.1.97                                      */
/**********************************************************************************/                                    
int WINAPI _loadds dllTSAktEvapRitchie(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSAktEvapRitchie");
        }
    #endif

    aktEvaporationRitchie(exp_p);
  
return 1;
}



/**********************************************************************************/
/* dll-Funktion:   dllCHPotETPenmanCrop                                                */
/* Beschreibung:   Wasserhaushalt nach Leach                                      */
/*                 ep/gsf            28.8.95                                      */
/**********************************************************************************/
int WINAPI _loadds  dllCHPotETPenmanCrop(EXP_POINTER)
{
 DECLARE_COMMON_VAR

 float f1;
 
 #ifdef LOGFILE                            
    if (SimStart(pTi))
    {
     introduce((LPSTR)"dllCHPotETPenmanCrop");
    }
 #endif
                                            
 if (pPl == NULL)
 {
  f1 = (float)0.5;
 }
 else
 {
 //f1 = (float)0.5 + pPl->pCanopy->fLAI/(float)8;
 f1=CropCoeff(exp_p);
 }
	 
 potevapotraPenman(exp_p);

 pWa->fPotETDay = pCl->pWeather->fPanEvap * f1;    


 //if (NewDay(pTi)) Interception(exp_p);

return 1;
}


/**********************************************************************************/
/* dll-Funktion:   dllEPPotETPenman_SPASS                                         */
/* Beschreibung:   Wasserhaushalt nach Leach                                      */
/*                 ep/gsf            25.5.99                                      */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotETPenman_SPASS(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotETPenman_SPASS");
        }
    #endif
  
 potevapotraPenman_SPASS(exp_p);

 //if (NewDay(pTi)) Interception(exp_p);
  
return 1;
}


/**********************************************************************************/
/* dll-Funktion:   dllEPPotETPenMon                                               */
/* Beschreibung:                                                                  */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotETPenMon(EXP_POINTER)
{
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotETPenMon");
        }
    #endif

 potevapotraPenMonteith(exp_p);

 //if (NewDay(pTi)) Interception(exp_p);


return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllEPPotETPenMonCrop                                           */
/* Beschreibung:   Potentielle Evapotranspiration nach Penman-Monteith            */
/*                 mit dualem Bestandeskoeffizienten                              */
/*                 ep/gsf            28.9.07                                      */
/**********************************************************************************/
float x,y;

int WINAPI _loadds  dllEPPotETPenMonCrop(EXP_POINTER)
{
 DECLARE_COMMON_VAR
 
 #ifdef LOGFILE                            
    if (SimStart(pTi))
    {
     introduce((LPSTR)"dllEPPotETPenMonCrop");
    }
 #endif
                                            
 potevapotraPenMonteith(exp_p);
 
 if ((pPl != NULL)&& (PlantIsGrowing))
 {
  //if (NewDay(pTi)) 
  {
   DualCropCoeff(exp_p,&x,&y); 
   //pWa->fPotETDay *= x;
   pWa->fPotETDay=pCl->pWeather->fPanEvap * x;
   pWa->fPotETdt*=x;
   //pWa->fGrdWatLevel=x;//only to test
  }
 }

 //if (NewDay(pTi)) Interception(exp_p);
	 
return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllEPPotETHaude                                                */
/* Beschreibung:                                                                  */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotETHaude(EXP_POINTER)
{
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotETHaude");
        }
    #endif

 potevapotraHaude(exp_p);

return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllEPPotETCanada                                               */
/* Beschreibung:                                                                  */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotETCanada(EXP_POINTER)
{
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotETCanada");
        }
    #endif

 potevapotraCanada(exp_p);

 //if (NewDay(pTi)) Interception(exp_p);

return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllEPPotEvap01                                                  */
/* Beschreibung:                                                                  */
/**********************************************************************************/                                    
int WINAPI _loadds dllEPPotEvap01(EXP_POINTER)
{
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPPotEvap01");
        }
    #endif

   	potevap1(exp_p);

	if (NewDay(pTi)) Interception(exp_p);

	//pWa->pEvap->fPotR -= (float)0.5*pPl->pPltWater->fInterceptDay;
	
return 1;
}

/**********************************************************************************/
/* dll-Funktion:   dllCKPotEvap03                                                 */
/* Beschreibung:                                                                  */
/**********************************************************************************/
int WINAPI _loadds dllCKPotEvap03(EXP_POINTER)
{
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCKPotEvap03");
        }
    #endif
    
    //if (NewTenthDay(pTi)) 
    //if (NewDay(pTi)) 
	{	
	DualCropCoeff(exp_p,&x,&y);
   	potevap3(exp_p,x,y);
	pWa->fGrdWatLvlPot=x;// only to test
	}

	if (NewDay(pTi)) Interception(exp_p);
	
return 1;
}