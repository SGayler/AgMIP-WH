/*******************************************************************************
 *
 * Copyright  (c) 
 *
 * Author:  
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 7 $
 *
 * $History: control.c $
 * 
 * *****************  Version 7  *****************
 * User: Christian Bauer Date: 24.01.02   Time: 15:49
 * Updated in $/Projekte/ExpertN/ModLib/control
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 24.01.02   Time: 11:37
 * Updated in $/Projekte/ExpertN/ModLib/control
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 23.01.02   Time: 14:08
 * Updated in $/Projekte/ExpertN/ModLib/control
 * Anbindung an Datenbank auch unter Win XP möglich. Fitting in Gang
 * gesetzt.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:34
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * No real change. Just reformatted for easier reading.
 * 
*******************************************************************************/

#include "xinclexp.h"   
#include "infowin.h"
#include "expert.h"
#include "trace.h"

// from util_fkt.c
extern int introduce(LPSTR lpName);

// from time1.c
extern int SimStart(PTIME);      
extern int SimEnd(PTIME);
extern int NewDay(PTIME);      
extern int EndDay(PTIME);      

extern int WINAPI Time1(EXP_POINTER);

// from manag.c
extern int WINAPI TSSoilLoosing(EXP_POINTER);                                                    
extern int WINAPI TSSoilMixing(EXP_POINTER);                                                    
extern int WINAPI TSFertilizer(EXP_POINTER);                                                    
extern int WINAPI TSFertilizerLoss(EXP_POINTER);                                                    
extern int WINAPI TSIrrigation(EXP_POINTER);                                                    
extern int WINAPI TSIrrigationLoss(EXP_POINTER);                                                    

extern int WINAPI DNDCSoilMixing(EXP_POINTER);
extern int WINAPI CERESSoilMixing(EXP_POINTER);

extern int WINAPI LAFertilizer(EXP_POINTER);                                                    

// from xoutFile.c
extern int  XwriteExpertDaily(void);      

extern int WINAPI PrintStart(EXP_POINTER, int);
extern int WINAPI PrintLayerResult(EXP_POINTER);
extern int WINAPI PrintN2OResult(EXP_POINTER);
extern int WINAPI PrintHydraulics(EXP_POINTER,int);
extern int WINAPI PrintGisData(EXP_POINTER,int);

extern int WINAPI cum_bil1(EXP_POINTER);


// from ReadModFile.c 
extern int WINAPI ReadModFile(EXP_POINTER);

// from xinit.c
extern int  WINAPI initData(EXP_POINTER); 
extern int  WINAPI init99Data(EXP_POINTER); 
extern short int WINAPI distributeSimLayer(EXP_POINTER);

// balance.c
extern int WINAPI cum_bil1(EXP_POINTER);


// dll-Funktionen:                              
int WINAPI _loadds dllCHTimeVar(EXP_POINTER);
int WINAPI _loadds dllCHTimeConst(EXP_POINTER);
int WINAPI _loadds dllCHTimeConst2(EXP_POINTER);
int WINAPI _loadds dllCHTimeVar2(EXP_POINTER);

int WINAPI _loadds dllCSInputData(EXP_POINTER);
int WINAPI _loadds dllTSInitiation(EXP_POINTER);  

int WINAPI _loadds dllDMCumBil(EXP_POINTER);

int WINAPI _loadds dllASOutput(EXP_POINTER);
int WINAPI _loadds dllCHGraph(EXP_POINTER);

int WINAPI _loadds dllCHOutStart(EXP_POINTER);
int WINAPI _loadds dllCHOutEnd(EXP_POINTER);
int WINAPI _loadds dllCHOutN2O(EXP_POINTER);
                                                  
int WINAPI _loadds dllTSSoilLoosing(EXP_POINTER);                                                    
int WINAPI _loadds dllTSSoilMixing(EXP_POINTER);                                                    
int WINAPI _loadds dllTSFertilizer(EXP_POINTER);                                                    
int WINAPI _loadds dllTSFertilizerLoss(EXP_POINTER);                                                    
int WINAPI _loadds dllTSIrrigation(EXP_POINTER);                                                    
int WINAPI _loadds dllTSIrrigationLoss(EXP_POINTER);                                                    

int WINAPI _loadds dllABDNDCSoilMixing(EXP_POINTER);                                                    
int WINAPI _loadds dllABCERESSoilMixing(EXP_POINTER);                                                    

int WINAPI _loadds dllCHDummy(EXP_POINTER);                                                    

//************************ für Fitting ergänzt: *********************************

//Folgende dll-Funktionen ersetzen dllTSInitiation bei Fitting-Einsatz von ExpertN
int WINAPI _loadds InitFittingML(EXP_POINTER); //Marquardt Levenberg
int WINAPI _loadds InitFittingSA(EXP_POINTER); //Simulated annealing
int WINAPI _loadds SohypFitML(EXP_POINTER);     //Marquardt Levenberg
int WINAPI _loadds SohypFitSA(EXP_POINTER);     //Simulated annealing
int WINAPI _loadds PlotObjFunc(EXP_POINTER);   //Ausgabe der Objektfunktion in .plo-Datei  
int WINAPI _loadds HPPlotObjFunc(EXP_POINTER); //dito für Hydraulic Property-Fit
                    
//aus Fitting.c 
extern int Fitting(EXP_POINTER,int);
extern int SetParameter(EXP_POINTER);
extern int PlotFittingObjectfunction(EXP_POINTER);
extern int PlotHPObjectfunction(EXP_POINTER);
extern int bFittingRunning;
extern int bFittingLoaded;
extern int bSetParameter;
extern int WriteMeasureTables(EXP_POINTER);

//aus HPFitting.c 
extern int HypFitting(EXP_POINTER,int);


/*==============================================================================
Static Method Implementation
==============================================================================*/
 
//------------------------------------------------------------------------------
int WINAPI _loadds InitFittingML(EXP_POINTER)
//------------------------------------------------------------------------------
{
  int ii;

  bFittingLoaded = 1;
  if (bFittingRunning==0)
  {
    bFittingRunning = 1;
    Fitting(exp_p,0);
    bFittingRunning = 0;     
  } 
  ii = dllTSInitiation(exp_p);
  bFittingLoaded = 1;
  return ii;
}

//------------------------------------------------------------------------------
int WINAPI _loadds InitFittingSA(EXP_POINTER)
//------------------------------------------------------------------------------
{
  int ii;
  bFittingLoaded = 1;
  if (bFittingRunning==0) {
    bFittingRunning = 1;
    Fitting(exp_p,1);
    bFittingRunning = 0;     
  } 
   ii = dllTSInitiation(exp_p);
  bFittingLoaded = 1;
  return ii;
}

//------------------------------------------------------------------------------
int WINAPI _loadds PlotObjFunc(EXP_POINTER)
//------------------------------------------------------------------------------
{
  int ii;
  bFittingLoaded = 1;
  if (bFittingRunning==0) {
    bFittingRunning = 1;
    PlotFittingObjectfunction(exp_p);
    bFittingRunning = 0;     
  } 
   ii = dllTSInitiation(exp_p);
  bFittingLoaded = 1;
  return ii;
}

//------------------------------------------------------------------------------
int WINAPI _loadds HPPlotObjFunc(EXP_POINTER)
//------------------------------------------------------------------------------
{
  int ii;
  bFittingLoaded = 1;
  if (bFittingRunning==0) {
    bFittingRunning = 1;
    PlotHPObjectfunction(exp_p);
    bFittingRunning = 0;     
  } 
   ii = dllTSInitiation(exp_p);
  bFittingLoaded = 1;
  return ii;
}

//------------------------------------------------------------------------------
int WINAPI _loadds SohypFitML(EXP_POINTER)
//------------------------------------------------------------------------------
{
  int ii;
  bFittingLoaded = 1;
  if (bFittingRunning==0) {
    bFittingRunning = 1;
    HypFitting(exp_p,0);
    bFittingRunning = 0;     
  } 
   ii = dllTSInitiation(exp_p);
  bFittingLoaded = 1;
  return ii;
}

//------------------------------------------------------------------------------
int WINAPI _loadds SohypFitSA(EXP_POINTER)
//------------------------------------------------------------------------------
{
  int ii;
  bFittingLoaded = 1;
  if (bFittingRunning==0) {
    bFittingRunning = 1;
    HypFitting(exp_p,1);
    bFittingRunning = 0;     
  } 
   ii = dllTSInitiation(exp_p);
  bFittingLoaded = 1;
  return ii;
}

//------------------------------------------------------------------------------
int WINAPI _loadds dllCHTimeVar(EXP_POINTER)  
// variable time control following Leach
//------------------------------------------------------------------------------
{ 
 DECLARE_COMMON_VAR

  static int i=0;                    
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
    i = 1;
        introduce((LPSTR)"dllCHTimeVar");
        }
    #endif
 
      pTi->pTimeStep->fMin      = (float)1e-5;
      pTi->pTimeStep->fMax      = (float)1e-3;//0.005;
 
 OK = Time1(exp_p);

 return OK;
}   // Ende Berechnung
 
//------------------------------------------------------------------------------
int WINAPI _loadds dllCHTimeVar2(EXP_POINTER)  
// variable time control following Leach
//------------------------------------------------------------------------------
{ 
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
        introduce((LPSTR)"dllCHTimeVar2");
        }
    #endif
 
      pTi->pTimeStep->fMin      = (float)1e-3;//1e-3;
      pTi->pTimeStep->fMax      = (float)0.01;//0.05;//0.1;

 
 OK = Time1(exp_p);

 return OK;
}   // Ende Berechnung



//------------------------------------------------------------------------------
int WINAPI _loadds dllCHTimeConst(EXP_POINTER)  
// Time Control with constant time step = 1 day.
//------------------------------------------------------------------------------
{ 
 DECLARE_COMMON_VAR
 
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
        introduce((LPSTR)"dllCHTimeConst");
        }
    #endif

      pTi->pTimeStep->fMin      = (float)1;
      pTi->pTimeStep->fMax      = (float)1;

 
 OK = Time1(exp_p);

 return OK;
}   // Ende Berechnung

//------------------------------------------------------------------------------
int WINAPI _loadds dllCHTimeConst2(EXP_POINTER)  
// Time Control with constant time step = 0.1 day.
//------------------------------------------------------------------------------
{ 
 DECLARE_COMMON_VAR
 
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
        introduce((LPSTR)"dllCHTimeConst2");
        }
    #endif
 
      pTi->pTimeStep->fMin      = (float)0.1;//(float)1.0/(float)24.0;//(float)0.1;
      pTi->pTimeStep->fMax      = (float)0.1;//(float)1.0/(float)24.0;//(float)0.1;

 
 OK = Time1(exp_p);

 return OK;
}   // Ende Berechnung



//------------------------------------------------------------------------------
int WINAPI _loadds dllDMCumBil(EXP_POINTER)
// Kumulation und Bilanzierung
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllDMCumBil");
        }
    #endif
        
    OK = cum_bil1(exp_p);

return OK;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllASOutput(EXP_POINTER) /*
Gibt die Logdateien für Expert-N und DSSAT aus.
Anm.: DSSAT derzeit noch nicht.

DLL-Funktionsname: <dllASOutput>
zugehörige Dateien: - Out1File.c   Zuständig für Öffnen, Schließen
                              der Dateien und Aufruf der Ausgabefunktionen.                         *
                    - Out1File.h   Header zu Out1File.c
                    - Out1Data.c   Beinhaltet Routinen zum Schreiben
                             des Logkopfes und der Wertausgabe
                             in die Logs; zudem noch Hilfsfunktionen.                          *
                    - Out1Layr.c   Zuständig für die Transformation
                               der Schichtzahlen bei WasserGH,
                               Nitrat und Temperatur.
------------------------------------------------------------------------------*/
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi)) introduce((LPSTR)"dllASOutput");
    #endif
        
    XwriteExpertDaily();

 return 1;
}

//------------------------------------------------------------------------------
int WINAPI _loadds dllCSInputData(EXP_POINTER)  
//------------------------------------------------------------------------------
{ 
 DECLARE_COMMON_VAR
 
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
        introduce((LPSTR)"dllCSInputData");
        }
    #endif
 
  // readExpertN_DB();

 return OK;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllTSInitiation(EXP_POINTER)  
//------------------------------------------------------------------------------
{ 
  int    OK            = 1;
  // int    iLayer        = 0;
  // char   lpOut[80];
  // float  DeltaT      = pTi->pTimeStep->fAct;
  // float  SimTime     = pTi->pSimTime->fTimeAct;
 
  #ifdef LOGFILE                            
    if (SimStart(pTi)) 
    {
      introduce((LPSTR)"dllTSInitiation");
    }
  #endif
        
  OK = distributeSimLayer(exp_p); 

  if (OK > 0) 
  {
    OK = init99Data(exp_p);
       
    if (OK > 0)
    {
      OK = ReadModFile(exp_p);
    }
       
    if (OK < 0) 
    {
       /* In case of any read error return value is negative. 
         But this does not need to be an error! */
    }
       
    if(bFittingLoaded && bFittingRunning && bSetParameter)
    {
      SetParameter(exp_p);
    }

    OK = initData(exp_p);
  }        
     
  OK = WriteMeasureTables(exp_p);
                   
  bFittingLoaded = 0;
                   
  return OK;

} // dllTSInitiation

//------------------------------------------------------------------------------
int WINAPI _loadds dllCHOutStart(EXP_POINTER)
//------------------------------------------------------------------------------
{ 
 int ii=(int)1;
   #ifdef LOGFILE                            
      if (SimStart(pTi)) 
        {
        introduce((LPSTR)"dllCHOutStart");
     PrintStart(exp_p,1);
     PrintHydraulics(exp_p,1);
    PrintLayerResult(exp_p);//ep 090301
    }

    if (EndDay(pTi))
      {
    PrintLayerResult(exp_p);//ep 090301
     PrintN2OResult(exp_p);//ep 210100
    if ((long)pTi->pSimTime->fTimeAct == pTi->pSimTime->fTimeAct) ii=(int)0;//ep 280301
      if ((long)pTi->pSimTime->fTimeAct + ii >= pTi->pSimTime->iSimDuration)// Abbruchkriterium
     {
      PrintStart(exp_p,2);  // 2 causes the function to wait.
        PrintGisData(exp_p,2);  // 2 causes the function to wait.
     }
     }
    #endif
        
 return 1;
}   // Ende Berechnung

//------------------------------------------------------------------------------
int WINAPI _loadds dllCHOutEnd(EXP_POINTER)
//------------------------------------------------------------------------------
{ 
    #ifdef LOGFILE                            
      if (SimStart(pTi)) 
        {
        introduce((LPSTR)"dllCHOutEnd");
        }
    #endif
        
    if (EndDay(pTi))
      {
      if (pTi->pSimTime->fTimeAct + 1 >= pTi->pSimTime->iSimDuration)   // Abbruchkriterium
     {
     PrintStart(exp_p,1);
     }
     }
         
 return 1;
}   // Ende Berechnung

//------------------------------------------------------------------------------
int WINAPI _loadds dllCHOutEndWait(EXP_POINTER)
//------------------------------------------------------------------------------
{ 
    #ifdef LOGFILE                            
      if (SimStart(pTi)) 
        {
        introduce((LPSTR)"dllCHOutEndWait");
        }
    #endif
        
    if (EndDay(pTi))
      {
      if (pTi->pSimTime->fTimeAct + 1 >= pTi->pSimTime->iSimDuration)   // Abbruchkriterium
     {
     PrintStart(exp_p,2);        // 2 causes the function to wait.
     }
     }
         
 return 1;
}   // Ende Berechnung

//------------------------------------------------------------------------------
int WINAPI _loadds dllCHOutN2O(EXP_POINTER)
//------------------------------------------------------------------------------
{ 
    #ifdef LOGFILE                            
      if (SimStart(pTi)) 
        {
        introduce((LPSTR)"dllCHOutN2O");
     PrintStart(exp_p,1);
        }
    #endif
        
    if (EndDay(pTi))
     {
     PrintN2OResult(exp_p);
     }
             
 return 1;
}   // Ende Berechnung

//------------------------------------------------------------------------------
int WINAPI _loadds dllTSSoilLoosing(EXP_POINTER)
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSSoilLoosing");
        }
    #endif
        
    TSSoilLoosing(exp_p);

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllTSSoilMixing(EXP_POINTER)
// Mischung- und Einarbeitungswirkung durch Bodenbearbeitung.
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSSoilMixing");
        }
    #endif
        
    TSSoilMixing(exp_p);

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllABDNDCSoilMixing(EXP_POINTER)
// Einarbeitung von org. Substanz und Durchmischung
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllABDNDCSoilMixing");
        }
    #endif
        
    DNDCSoilMixing(exp_p); 

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllABCERESSoilMixing(EXP_POINTER)
// Einarbeitung von org. Substanz und Durchmischung
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllABCERESSoilMixing");
        }
    #endif
        
    CERESSoilMixing(exp_p); 

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllTSFertilizer(EXP_POINTER)
// Mineralische und organische Duengung
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSFertilizer");
        }
    #endif
        
    TSFertilizer(exp_p); 

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllLAFertilizer(EXP_POINTER)
// Mineralische und organische Duengung
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllLAFertilizer");
        }
    #endif
        
    LAFertilizer(exp_p); 

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllTSFertilizerLoss(EXP_POINTER)
// Beruecksichtigung von Duengungsverlusten
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSFertilizerLoss");
        }
    #endif
        
    TSFertilizerLoss(exp_p);

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllTSIrrigation(EXP_POINTER)
// Integration von Beregnungsmassnahmen
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSIrrigation");
        }
    #endif
        
    TSIrrigation(exp_p);

return 1;
}   // Ende Berechnung


//------------------------------------------------------------------------------
int WINAPI _loadds dllTSIrrigationLoss(EXP_POINTER)
// Beruecksichtigung von Verlusten die bei Beregnung entstehen.
//------------------------------------------------------------------------------
{        
 DECLARE_COMMON_VAR
 
    #ifdef LOGFILE                            
      if (SimStart(pTi))
        {
         introduce((LPSTR)"dllTSIrrigationLoss");
        }
    #endif
        
//    (exp_p);

return 1;
}   // Ende Berechnung

//------------------------------------------------------------------------------
int WINAPI _loadds dllCHDummy(EXP_POINTER)
//------------------------------------------------------------------------------
{        
  return 1;
}

/*******************************************************************************
** EOF */