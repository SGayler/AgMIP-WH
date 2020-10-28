/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:  C. Sperr
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Include file for Expert-N library development includes structures and
 *   function definitions for exported functions and types.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 5 $
 *
 * $History: export.h $
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:04
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Zur Stabilisierung des Codes Methoden Prototypen Definition in Header
 * Files verschoben statt über extern Declarationen festgelegt.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:28
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * Import CfgFile functions by header file.
 * 
 *   07.03.97
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_EXPORT_H
#define _INC_EXPERTN_EXPORT_H

/*  static pointer to provide real layer - simulation layer information */
/*  during initialization / pedotransfer functionality                  */
typedef struct realLayer{
  int   iLayerDim;
  int  *paiMatLayer; 
  int  *paiMatLayerDepth;
  int  iStartLayerDim;
  int  *paiStartLayerDepth;
} REALLAYER; 
typedef REALLAYER *PREALLAYER; 

/* this function is exported from ExpertN application e.g. for XInit.DLL          */
PREALLAYER  WINAPI _loadds  getpstRealLayer();                                 

#endif // #ifndef _INC_EXPERTN_EXPORT_H

/*******************************************************************************
** EOF */