/*********7*****************************************************************************
 *
 *   Steady01.C
 *   Wasserflussrechnung im Profil  Steady State
 *   Autor : E. Priesack 9.11.95
 *           
 *********************************************************************************
 */

/* INCLUSION of HEADER-FILES */
#include "xinclexp.h"
#include "xlanguag.h"     

#include "xh2o_def.h"



int WINAPI Water_Steady(EXP_POINTER)
{

  DECLARE_COMMON_VAR


  DECLARE_H2O_POINTER

  float DeltaZ = pSo->fDeltaZ;

/********************************************************************************************
 *                    WASSERGEHALTE Fluss und FUSSDICHTE
 *  Schicht 1  bis   max -1
 * --------------------------------------------------------
 */

  for(H2O_ZERO_LAYER H2O_UNTIL_BEFORE_LAST NEXT_H2O_LAYER)
  
   {
    /* --------------------------------------------------------*/
    /*     Wassergehalte     im Profil                         */
    /* --------------------------------------------------------*/
  	pWL->fContAct = pWL->fContOld;
    /* --------------------------------------------------------*/
    /*    Wasser - Flussdichte    im Profil                    */
    /* --------------------------------------------------------*/
  	pWL->fFluxDens = (float)0.0;//pCl->pWeather->fRainAmount;
    /* --------------------------------------------------------*/
    /* Fluss pro Schicht   */
    /* --------------------------------------------------------*/
  	pWL->fFlux = pWL->fFluxDens * DeltaT;

  } /* for alle Schichten Flussberechnungen    */
/*
 * for alle Schichten Flussberechnungen
 ****************************************
 */
 /************
  *    Fluss aus der letzten Schicht bedeutet Auswaschung = DrainW
  */
  pWa->fLeaching = pWL->pBack->fFlux;
  
 /********************************************************************************************
*/
 /* --------------------------------------------------------*/
 return 1;




}

