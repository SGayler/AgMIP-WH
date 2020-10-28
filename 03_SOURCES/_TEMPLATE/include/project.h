/*******************************************************************************
 *
 * Copyright (c) by Christoph Sperr
 *
 *------------------------------------------------------------------------------
 *
 * Contents: All project relevant definitions and defines
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 5 $
 *
 * $History: project.h $
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 25.12.01   Time: 10:03
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * Übergabeparameter "PSIMPROJECT" aus Methoden entfernt, da es eh nur ein
 * SimProject gleichzeitig geben kann und dieses Objekt als statische
 * Variable im Modul Project gekapselt ist.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:30
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * Memory handle of type HGLOBAL.
 * Marker defines combined in one file (base system).
 * Prototypes of public methods.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/include
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 *   13.11.96
 *
 ******************************************************************************/

#ifndef _INC_EXPERTN_PROJECT_H
#define _INC_EXPERTN_PROJECT_H

#include "sql.h"      // core level
#include "sqlext.h"   // extended odbc-function  level 


/*==============================================================================
Public Type Definitions and Constants
==============================================================================*/

#define MAX_NAME_LEN     100
#define NAME_LEN          50 
#define SHORT_NAME_LEN    10

// simulation object structure describes the items necessary for
// one simulation run

typedef struct simobject  * PSIMOBJECT;

typedef struct simobject {
  HGLOBAL       hMemThis;
  char          szPrjName[NAME_LEN];
  long          lAutoTeSch;
  DATE_STRUCT   stStartDate;
  DATE_STRUCT   stStopDate;
  double        dFarmNo;
  char          szFarmName[NAME_LEN];
  int           iYear;
  char          szSchlagNo[SHORT_NAME_LEN];
  char          szSchlagName[NAME_LEN];
  char          szTeilSNo[SHORT_NAME_LEN];
  char          szTeilSName[NAME_LEN];
  char          szSoilProfile[NAME_LEN];
  char          szWeatherStation[NAME_LEN];
  char          szCfgFile[MAX_NAME_LEN]; 
  char          szModFile[MAX_NAME_LEN];    
  int           iHeight;                 
  float         fSlope;  
  char          szExposure[SHORT_NAME_LEN];
  char          szRegionNo[SHORT_NAME_LEN];
  char          szSimFile[MAX_NAME_LEN];
  char          szWthFile[MAX_NAME_LEN];
  char          szField[MAX_NAME_LEN];
  char          szCompany[MAX_NAME_LEN];
  char          szWFile[MAX_NAME_LEN];
  char          szNFile[MAX_NAME_LEN];
  char          szHFile[MAX_NAME_LEN];
  char          szPFile[MAX_NAME_LEN];
  PSIMOBJECT    pFirst;
  PSIMOBJECT    pBack;
  PSIMOBJECT    pNext;
} SIMOBJECT;

// Simulation Projects 
typedef struct simproject  * PSIMPROJECT;
typedef struct simproject {
  HGLOBAL      hMemThis;                          
  int          iFitting;
  int          iDataSource;
  char         szPrjName[MAX_NAME_LEN];
  PSIMOBJECT   pCurrentSimObj;
  PSIMOBJECT   pSimObj;
} SIMPROJECT;


/*==============================================================================
Public Method Interface Description
==============================================================================*/

//------------------------------------------------------------------------------
extern PSIMPROJECT getProjectRoot( void );
/* <method interface description should be placed here>
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
extern BOOL ValidProject( void );
/* <method interface description should be placed here>
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
extern BOOL loadDBProject( HWND i_hwnd );
/* <method interface description should be placed here>
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
extern int readDBProjectItems( LPSTR i_lpszPrjName );
/* <method interface description should be placed here>
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
extern void resetProject( void );
/* <method interface description should be placed here>
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
extern int switchSimObject( void );
/* <method interface description should be placed here>
------------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
extern BOOL readProject(
/* The project file will be opened and its data content will be read.
   If the content of the project file was OK (all necessary object items
   found) the memory to keep the project definition will be allocated.
Parameters: */
  char  *i_szPrjPath ); /*
    Full path name of the project definition file (including device and directory).
Return Values:
  TRUE .... Success (valid project file read)
  FALSE ... Error (no valid project file or wrong syntax)
------------------------------------------------------------------------------*/

#endif // #ifndef _INC_EXPERTN_PROJECT_H

/*******************************************************************************
** EOF */
