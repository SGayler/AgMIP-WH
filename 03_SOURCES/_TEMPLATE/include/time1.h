/*******************************************************************************
 *
 * Copyright (c) by ZeusSoft, Ing. Buero Bauer
 *                  Gewerbepark 28
 *                  D-83670 Bad Heilbrunn
 *                  www.zeussoft.de
 *
 *-------------------------------------------------------------------------------
 *
 * Contents:
 *
 *-------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: time1.h $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:30
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/include
 * Public method interface description.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/include
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 * *****************  Version 1  *****************
 * User: Christian Bauer Date: 13.11.01   Time: 10:15
 * Created in $/ExpertN/ModLib/ModLib/include
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_TIME1_H
#define _INC_EXPERTN_TIME1_H

#include "xtime.h"

/*==============================================================================
Public Method Interface Description
==============================================================================*/

//------------------------------------------------------------------------------
extern int SimStart( /*
<method description should be placed here>
Parameter: */
  PTIME pz ); /*
    <description of the parameter with the allowed range should be placed here>
Return Value:
  <description of the return values and its range should placed here>
------------------------------------------------------------------------------*/


#endif // #ifndef _INC_EXPERTN_TIME1_H

/*******************************************************************************
** EOF */