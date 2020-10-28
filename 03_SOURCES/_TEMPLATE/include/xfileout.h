/*******************************************************************************
 *
 * Copyright 2001 - 2002 (c) by ZeusSoft, Ing. Buero Bauer
 *                              www.zeussoft.de
 *                              Tel: 0700 zeussoft
 *
 * Author:  Christian Bauer
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 1 $
 *
 * $History: xfileout.h $
 * 
 * *****************  Version 1  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 13:48
 * Created in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * 
*******************************************************************************/

#ifndef _INC_EXPERTN_XFILEOUT_H
#define _INC_EXPERTN_XFILEOUT_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/


/*==============================================================================
Public Method Interface Description
==============================================================================*/

extern BOOL XOpenValidExpOutputfiles(void);
extern int  XCloseValidExpOutputfiles(void);    

extern int XToggleAllDSSATFileStatus(void);
extern int XToggleExpFWaterStatus(void);
extern int XToggleExpFHeatStatus(void);
extern int XToggleExpFNitroStatus(void);
extern int XToggleExpFCarbonStatus(void);
extern int XToggleExpFPlantStatus(void);
extern int XToggleExpFBalanceStatus(void);

extern int XFWriteExpertDaily(void);

extern HANDLE OpenF( char *i_szFileName );
extern void   CloseF( HANDLE *i_phFile );

extern int fout_floatNorm(HANDLE, float f1,int iDecimal, int iVorkomma);
extern int fout_line(HANDLE, int iNr);
extern int fout_space(HANDLE, int iNr);
extern int fout_long(HANDLE, long far *plWert);
extern int foutDate_long(HANDLE, long far *plWert);
extern int fout_int(HANDLE hFile, int far *piWert, int Laenge);
extern int fout_string(HANDLE hFile, LPSTR lpString);
extern int FloatToCharacter(LPSTR lptOut,float fNumber,int iDecimal);


#endif // #ifndef _INC_EXPERTN_XFILEOUT_H

/*******************************************************************************
** EOF */
