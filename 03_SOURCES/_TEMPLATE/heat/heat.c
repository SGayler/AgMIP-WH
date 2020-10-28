/**********************************************************************************************
 *                                                                                            *
 *  module  :   heat.c                                                                        *
 *  purpose :   Liefert DLL mit Waermehaushalts-Modulen                                       *
 *  date  :     20.09.95                                                                      *
 *                                                                                            *
 *              02.10.96 dm, Implementierung der neuen Variablenstruktur                      *
 **********************************************************************************************/

 /* Sammlung aller Header-Dateien: */
#include "xinclexp.h"   
#include "xtempdef.h"


// aus util_fct.c
extern int introduce(LPSTR lpName);

/* from time1.c */
extern int SimStart(PTIME);      

// aus temp01.c
extern int WINAPI BodenTempTi(EXP_POINTER);

// aus ht02new.c
signed short int WINAPI WaermeFlussWilliams(EXP_POINTER);

// aus temp_dai.c
extern int WINAPI Temp_D(EXP_POINTER, int Frost_Rad_Flag);
extern int WINAPI ConstTemp(EXP_POINTER);
//aus BoFrost.c
extern int WINAPI Temp_SHAW(EXP_POINTER, int Frost_Rad_Flag);
// aus output.ccp
extern int WINAPI Deb_Temp(EXP_POINTER);


/*************************************************/
/*             dll-Funktionen                    */
/*************************************************/
int WINAPI _loadds dllCHTempDaisy(EXP_POINTER);
int WINAPI _loadds dllEPTempTillotson(EXP_POINTER);
int WINAPI _loadds dllCSTempSHAW(EXP_POINTER);

int WINAPI _loadds dllCHTempDF(EXP_POINTER);
int WINAPI _loadds dllCHTempDRF(EXP_POINTER);
int WINAPI _loadds dllCHTestCond(EXP_POINTER);

int WINAPI _loadds dllTSTempWilliams(EXP_POINTER);  
int WINAPI _loadds dllSAConstTemp(EXP_POINTER);

/********************************************************/
/* module  :   dllEPTempTillotson                       */
/********************************************************/
int WINAPI _loadds dllEPTempTillotson(EXP_POINTER)  
{
  DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllEPTempTillotson");
        }
    #endif
   
  OK = BodenTempTi(exp_p);
  return OK;
}   /* Ende Berechnung */

  
/********************************************************/
/* module  :   dllCHTempDF                          */
/********************************************************/
int WINAPI _loadds dllCHTempDF(EXP_POINTER)  
{
  DECLARE_COMMON_VAR
  
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCHTempDF");
        }
    #endif

  OK = Temp_D(exp_p, 1);  // 1: mit Frost, ohne Strahlung

  return OK;
}   /* Ende Berechnung */

/********************************************************/
/* module  :   dllTSTempWilliams                        */
/********************************************************/
int WINAPI _loadds dllTSTempWilliams(EXP_POINTER)  
{
  DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSTempWilliams");
        }
    #endif

  OK = WaermeFlussWilliams(exp_p); 

  return OK;
}   /* Ende Berechnung */

 


    
/********************************************************/
/* module  :   dllEPTempDRF                              */
/********************************************************/
int WINAPI _loadds dllCHTempDRF(EXP_POINTER)  
{
  DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCHTempDRF");
        }
    #endif

  OK = Temp_D(exp_p, 3);  // 3: mit Frost, mit Strahlung

  return OK;
}   /* Ende Berechnung */

      
/********************************************************/
/* module  :   dllEPTempDOF                             */
/********************************************************/
int WINAPI _loadds dllCHTempDaisy(EXP_POINTER)  
{
  DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCHTempDaisy");
        }
    #endif

  OK = Temp_D(exp_p, 0); // 0: ohne Frost, ohne Strahlung

  return OK;
}   /* Ende Berechnung */

 
  
int WINAPI _loadds dllCHTestCond(EXP_POINTER)
{
        
if (SimStart(pTi))
     {
     /*Test_Cond(exp_p); */
     }

  return 1;
}   /* Ende Berechnung */



/********************************************************/
/* module  :   dllSAConstTemp                           */
/********************************************************/
int WINAPI _loadds dllSAConstTemp(EXP_POINTER)
{
  DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllSAConstTemp");
        }
    #endif

  OK = ConstTemp(exp_p); 

  return OK;
}

/********************************************************/
/* module  :   dllCSTempSHAW                            */
/********************************************************/
int WINAPI _loadds dllCSTempSHAW(EXP_POINTER)  
{
  DECLARE_COMMON_VAR

    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllCSTempSHAW");
        }
    #endif

  //OK = Temp_SHAW(exp_p,0); // 0: ohne Frost, ohne Strahlung
  OK = Temp_SHAW(exp_p,1); // 1: mit Frost, ohne Strahlung

  return OK;
}   /* Ende Berechnung */
