/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:      C.Sperr
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Prieack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Include file for Expert-N library development.
 *   Simulation time objects.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: xtime.h $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:33
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * Avoid compile error "Simtime already defined".
 * 
 *   27.11.95
 *   letzte Aenderung 01.12.96
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XTIME_H
#define _INC_EXPERTN_XTIME_H

typedef struct
{
  float  fTimeAct;  
  float  fTimeOld;  
  float  fTimeDay;
  long   lTimeDate;
  int    iJulianDay;  
  float  fTimeTenDay;  
  int    iSimDuration;  
  int    iStartJulianDay;
  long   lStartDate;  
  long   lStopDate;  
  int    iStartCentury;
  int    iStopCentury;
  int    iTimeFlag1;
  int    iTimeFlag2;
} SIMTIME, *PSIMTIME;

typedef struct
{
  float  fAct;
  float  fOld;
  float  fMax;
  float  fOpt;
  float  fMin;
  float  fWaterOpt;
  float  fChemOpt;
  float  fWaterDist;
  float  fChemDist;
} TIMESTEP, *PTIMESTEP;

typedef struct
{
  PSIMTIME   pSimTime;
  PTIMESTEP  pTimeStep;
} TIME, *PTIME;


#endif // #ifndef _INC_EXPERTN_XTIME_H

/******************************************************************************
** EOF */