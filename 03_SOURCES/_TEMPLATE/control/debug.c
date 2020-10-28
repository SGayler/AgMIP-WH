/*******************************************************************************
 *
 * Copyright (c) by 
 *
 *------------------------------------------------------------------------------
 *
 * Contents:
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: debug.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:34
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Using _MALLOC macro to detect memory leaks.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/control
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 * C. Sperr    05.05.94
 *
 ******************************************************************************/
 
#include <windows.h>
#include <time.h>
#include <string.h>
#include <crtdbg.h>

// für TestOutput
#include "xinclexp.h"

 
HANDLE  hFile = INVALID_HANDLE_VALUE;

char    acDummy[500];

BOOL  Dbg_DoubleToText(double, LPSTR, int);
BOOL  Dbg_FloatToText(float, LPSTR, int);
char  *Dbg_GetDate(int);
char  *Dbg_GetTime(int);

/* util_fct.c */
extern int   WINAPI Message(long, LPSTR);

// aus util_fkt.c
extern int SimStart(PTIME);      
extern int introduce(LPSTR lpName);


// Funktionsdeklaration

HANDLE OpenDebugF(void);
int    CloseDebugF(void);
int    fwriteDbg_line(int);
int    fwriteDbg_space(int);
int    fwriteDbg_int(int far *);
int    fwriteDbg_long(long far *);
int    fwriteDbg_float(float far *,int);
int    fwriteDbg_double(double far *,int);
int    fwriteDbg_string(LPSTR );

// Prototypes
int FloatToText(float fX, LPSTR lpTxt, int iWidth);

int WINAPI TestOutput(EXP_POINTER);


/**********************************************************************************************
 *
 *  CloseDebugF()
 *  C.Sperr  05.05.94
 **********************************************************************************************
 */
int CloseDebugF(void)
{ 
  BOOL bRet = FALSE;
  int iRet = -999;

  if ( hFile != INVALID_HANDLE_VALUE )
  {
    fwriteDbg_line(1);
              
    #ifdef INFO
      wsprintf(acDummy," Debug Files Closed  ");
      MessageBox(NULL,acDummy," DEBUG", MB_ICONINFORMATION);
    #endif
              
    if ( CloseHandle( hFile ) )
    {
      iRet = 0;
    }
    hFile = INVALID_HANDLE_VALUE;
  }
  return   iRet;
}



/**********************************************************************************************
 *
 *  OpenDebugF()
 *  C.Sperr  05.05.94
 **********************************************************************************************
 */
HANDLE OpenDebugF(void)
{
  if (hFile == INVALID_HANDLE_VALUE)
  { 
    hFile = CreateFile(
      /* lpFileName             */ "debug.tmp",
      /* dwDesiredAccess        */ GENERIC_READ | GENERIC_WRITE,
      /* dwShareMode            */ 0,
      /* lpSecurityAttributes   */ NULL,
      /* dwCreationDistribution */ CREATE_ALWAYS,
      /* dwFlagsAndAttributes   */ FILE_ATTRIBUTE_NORMAL,
      /* hTemplateFile          */ NULL );
 
    if( hFile != INVALID_HANDLE_VALUE )
    {
      fwriteDbg_string("-Debug Opened : -<debug.tmp>- \n");
      fwriteDbg_string(Dbg_GetDate(0));
      fwriteDbg_string("         ");
      fwriteDbg_string(Dbg_GetTime(0));
      fwriteDbg_string("\n");
           
      #ifdef INFO
        wsprintf(acDummy," -<Debug Files Open>- ");
        MessageBox(NULL,acDummy,"DLL -Link", MB_ICONINFORMATION);
      #endif
      MessageBeep(MB_ICONINFORMATION);
    }
  }

  return hFile;
}


/**********************************************************************************************
 *
 *  fwriteDbg_line
 *  C.Sperr  05.05.94
 **********************************************************************************************
 */
int   fwriteDbg_line(int iNr)
{
  char  ch1[2];
  int   i = 0;
  DWORD dwBytesWritten = 0;

  if ( hFile != INVALID_HANDLE_VALUE )
  {
    ch1[0]= 0x0D;   //  carriage return
    ch1[1]= 0x0A;   //  line feed
    for (i=0;i<iNr;i++)
    {
      WriteFile(
        /* hFile                  */ hFile,
        /* lpBuffer               */ ch1,
        /* nNumberOfBytesToWrite  */ 2,
        /* lpNumberOfBytesWritten */ &dwBytesWritten,
        /* lpOverlapped 	        */ NULL );

      i++;
    }
  }
  //---------------  only if hFile was opened by OpenDbgFile()

  return i;
}

/***********************************************************************************************
 *
 *   fwriteDbg_space
 *
 *   C.Sperr  05.05.94
 **********************************************************************************************
 */
int   fwriteDbg_space(int iNr)
{
  int i;
  DWORD dwBytesWritten = 0;
  char acDummy[2];

  if ( hFile != INVALID_HANDLE_VALUE )
  {
    acDummy[0] = 0x20;
    acDummy[1] =(char)0;
    for (i=0; i<iNr; i++)
    {
      WriteFile(
        /* hFile                  */ hFile,
        /* lpBuffer               */ acDummy,
        /* nNumberOfBytesToWrite  */ 1,
        /* lpNumberOfBytesWritten */ &dwBytesWritten,
        /* lpOverlapped           */ NULL );
    }
  }
  //---------------  only if hFile was opened by OpenDbgFile()

  return i;
}

/***********************************************************************************************
 *
 *   fwriteDbg_int
 *
 *   C.Sperr  05.05.94
 **********************************************************************************************
 */
int   fwriteDbg_int(int far *piWert)
{
  int  idummy;
  DWORD dwBytesWritten = 0;
  char acdummy[50];
  char *pacdummy = acdummy;

  if ( hFile != INVALID_HANDLE_VALUE )
  {
    wsprintf((LPSTR)pacdummy,"%d  ",*piWert);
    idummy = strlen(pacdummy);
    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ pacdummy,
      /* nNumberOfBytesToWrite  */ idummy,
      /* lpNumberOfBytesWritten */ &dwBytesWritten,
      /* lpOverlapped           */ NULL );
  }
  return idummy;
}


/**********************************************************************************************
 *
 *   writeDbg_long
 *
 *  C.Sperr  05.05.94
 *
 **********************************************************************************************
 */
int   fwriteDbg_long(long far *plWert)
{ 
  int i;
  DWORD dwBytesWritten = 0;
  char ch1[50];
  char * ch = ch1;

  if ( hFile != INVALID_HANDLE_VALUE )
  {
    wsprintf((LPSTR)ch,"%d  ",*plWert);
    i = strlen(ch);
    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ ch,
      /* nNumberOfBytesToWrite  */ i,
      /* lpNumberOfBytesWritten */ &dwBytesWritten,
      /* lpOverlapped 	        */ NULL );
  }
  //---------------  only if hFile was opened by OpenDbgFile()

  return 0;
}

/**********************************************************************************************
 *
 *  fwriteDbg_float
 *
 *  C. Sperr  / 05.05.94
 **********************************************************************************************
 */
int   fwriteDbg_float(float far *pfWert,int iDec)
{
  int i;
  DWORD dwBytesWritten = 0;
  char ch1[50];
  float f1 = *pfWert;
  char * ch = ch1;
  if ( hFile != INVALID_HANDLE_VALUE )
  {       
    Dbg_FloatToText(f1,ch,iDec);
    i = strlen(ch);
    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ ch,
      /* nNumberOfBytesToWrite  */ i,
      /* lpNumberOfBytesWritten */ &dwBytesWritten,
      /* lpOverlapped 	        */ NULL );
  }
  return 0;
}


/**********************************************************************************************
 *
 *   fwriteDbg_double
 *
 *   C. Sperr  /   05.05.94
 *
 **********************************************************************************************
 */
int   fwriteDbg_double(double far *pdWert,int iDec)
{
  int i;
  DWORD dwBytesWritten = 0;
  double lf1;
  char ch1[50];
  char * ch;

  ch = ch1;
  lf1  = *pdWert;
  if ( hFile != INVALID_HANDLE_VALUE )
  {   
    Dbg_DoubleToText(lf1,ch,iDec);
    i = strlen(ch);
    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ ch,
      /* nNumberOfBytesToWrite  */ i,
      /* lpNumberOfBytesWritten */ &dwBytesWritten,
      /* lpOverlapped 	        */ NULL );
  }
  //---------------  only if hFile was opened by OpenDbgFile()

  return 0;
}

/**********************************************************************************************
 *
 *   fwriteDbg_string
 *
 *   C.Sperr   05.05.94
 **********************************************************************************************
 */
int   fwriteDbg_string(LPSTR lpString)
{
  int i;
  DWORD dwBytesWritten = 0;
  char ch1[100];
  char * ch = ch1;

  if ( hFile != INVALID_HANDLE_VALUE )
  {
    wsprintf((LPSTR)ch,"%s  ",lpString);
    i = strlen(ch);
    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ ch,
      /* nNumberOfBytesToWrite  */ i,
      /* lpNumberOfBytesWritten */ &dwBytesWritten,
      /* lpOverlapped 	        */ NULL );
  }
  //---------------  only if hFile was opened by OpenDbgFile()

 return 0;
}


/********************************************************************************************
 *
 *  FloatToString
 *
 *  C. Sperr   05.05.94
 *******************************************************************************************
 */
BOOL Dbg_FloatToText(float fX, LPSTR lpTxt, int iWidth)
{
  int i;
  float dec;
  char   cSign[2]="\0";
  char   ac[50],acCtrl[20];
  long   lX1, lX2;

  /**********************
   *   Vorkommastellen
   */
  if (fX < 0.0)    {   fX *= (float)-1.0;
                       lstrcpy(cSign,"-");
                   }
  else lstrcpy(cSign,"+");
 /*--*/
  if ((fX >= 1.0) || (fX <= -1.0))    lX1 = (long) fX;
  else                                lX1 = 0;

  /*******************************
   *   Nachkommastellen
   */
  for (i=0,dec =(float)1.0;i<iWidth;i++) dec *= (float)10;   // pow

  lX2 = (long) (((fX-(float)lX1)* dec)+0.5);
  lX2 = (lX2>0) ? lX2 : lX2 *(-1);

  lstrcpy(lpTxt,cSign);
  wsprintf(ac,(LPSTR)"%ld",lX1);
  lstrcat(lpTxt,ac);
  lstrcat(lpTxt,".");
  wsprintf(acCtrl,"%s%d%d%s","%",0,iWidth,"ld");
  wsprintf(ac,acCtrl,lX2);
  lstrcat(lpTxt,ac);

  return TRUE;
}
/*******************************************************************************************
 *  End of Dbg_FloatToText
 *******************************************************************************************
 */

/********************************************************************************************
 *
 *  DoubleToString
 *
 *  C. Sperr   05.05.94
 *******************************************************************************************
 */
BOOL Dbg_DoubleToText(double dX, LPSTR lpTxt, int iWidth)
{
  int i;
  double dec;
  char   cSign[2]="\0";
  char   ac[10];
  char   acCtrl[20];
  long lX1, lX2;

  /**********************
   *   Vorkommastellen
   */
  if (dX < 0.0)    {   dX *= -1.0;
                       lstrcpy(cSign,"-");
                   }
  else lstrcpy(cSign,"+");
 /*--*/
  if ((dX >= 1.0) || (dX <= -1.0))    lX1 = (long) dX;
  else                                lX1 = 0;

  /**********************
   *   Nachkommastellen
   */
  for (i=0,dec =1.0;i<iWidth;i++) dec *= 10;

  lX2 = (long) (((dX-(double)lX1)* dec)+0.5);
  lX2 = (lX2>0) ? lX2 : lX2 *(-1);

  lstrcpy(lpTxt,cSign);
  wsprintf(ac,(LPSTR)"%ld",lX1);
  lstrcat(lpTxt,ac);
  lstrcat(lpTxt,".");
  wsprintf(acCtrl,"%s%d%d%s","%",0,iWidth,"ld");
  wsprintf(ac,acCtrl,lX2);
  lstrcat(lpTxt,ac);

  return TRUE;
}
/*******************************************************************************************
 *  End of Dbg_DoubleToText
 *******************************************************************************************
 */

/*******************************************************************************************
 *  Dbg_GetDate
 *******************************************************************************************
 */
char *Dbg_GetDate(int iFormat)
{
  static char acDate1[10];
  static char acDate2[10];

  _strdate(acDate1);

  switch(iFormat)
  {
    case 0:                              // German format
        acDate2[0]=acDate1[3];
        acDate2[1]=acDate1[4];
        acDate2[2]=(char)'.';
        acDate2[3]=acDate1[0];
        acDate2[4]=acDate1[1];
        acDate2[5]=(char)'.';
        acDate2[6]=acDate1[6];
        acDate2[7]=acDate1[7];
        acDate2[8]=(char) 0;
        break;

    case 1:                              // English format
        acDate2[0]=acDate1[0];
        acDate2[1]=acDate1[1];
        acDate2[2]=(char)'-';
        acDate2[3]=acDate1[3];
        acDate2[4]=acDate1[4];
        acDate2[5]=(char)'-';
        acDate2[6]=acDate1[6];
        acDate2[7]=acDate1[7];
        acDate2[8]=(char) 0;
        break;
  }

  return acDate2;
}
/********************************************************************************************/
/*  End of Dbg_GetDate                                                                          */
/********************************************************************************************/

/******************************************************************************************
 *  Dbg_GetTime
 *******************************************************************************************
 */
char *Dbg_GetTime(int iLength)
{
  static char acTime[10];

  _strtime(acTime);

  if (iLength == 0)
    acTime[5]=(char) 0;
  else
    acTime[8]=(char) 0;

  return acTime;
}
/********************************************************************************************
 *  End of Dbg_GetTime
 *******************************************************************************************
 */




/********************************************************************************
 * dll-Funktion:   dllASTestOutput
 ********************************************************************************/
int WINAPI _loadds dllASTestOutput(EXP_POINTER)  
{ 
 DECLARE_COMMON_VAR

   if (SimStart(pTi))
      {
       introduce((LPSTR)"dllASTestOutput");
      }
                                            
     TestOutput(exp_p);       
     
 return OK;
}   


/**********************************************************************************/
/*                                                                                */
/*  module  :   TestOutput                                                        */
/*  purpose :   Testet die Grafikfunktionen und die Dateiausgabe von ExpertN      */
/*  date  :     as, 23.05.97                                                      */
/*              ch,  7.10.97                                                      */
/*              Initialisiere auf TestWerte gemäß GRA_1097.xls vom 7.10.97        */
/*                                                                                */
/**********************************************************************************/
int WINAPI TestOutput(EXP_POINTER)
{
  
  DECLARE_COMMON_VAR
  PWLAYER pWL;
  PCLAYER pCL;
  PSLAYER pSL;
  PHLAYER pHL;
  PLAYERROOT     pLR;

  
  pCl->pWeather->fRainAmount  = (float)1;
  pCl->pWeather->fTempAve     = (float)2;
  
  pWa->fInfiltDay             = (float)4;
  
  pCl->fCumRain               = (float)6;
  pWa->pWBalance->fActCumEvap = (float)7; 
  pWa->fRunOffCum             = (float)9;
  pWa->fCumInfiltration       = (float)10;
  pWa->fCumLeaching           = (float)11;
  
  pWa->fPotETCum              = (float)12;
  pWa->pWBalance->fPotCumEvap = (float)13; 
  pPl->pPltWater->fCumDemand  = (float)14;
  pWa->fActETCum              = (float)15;
  
	pWa->fRunOffDay             = (float)5;
  	pWa->fActTranspCum          = (float)8; 

  for (SOIL_LAYERS0(pWL,pWa->pWLayer))
        {
        pWL->fContAct = (float)0.15 + (float)0.001*iLayer;
        pWL->fContInit = (float)18 + (float)0.1*iLayer;
        }
  
  pWa->pWBalance->fBalance    = (float)16;
  pWa->pWBalance->fProfil     = (float)17;
  pWa->pWBalance->fProfileIce = (float)61;
  pWa->pWBalance->fInput      = (float)18;
  pPl->pPltWater->fCumUptake  = (float)19;      
  
  for (SOIL_LAYERS0(pWL,pWa->pWLayer))
       {
        pWL->fMatPotAct = (float)20 + (float)0.1*iLayer;
        pWL->fFluxDay   = (float)21 + (float)0.1*iLayer;
        pWL->fHydrCond  = (float)22 + (float)0.1*iLayer;
       }   
      
  for (SOIL_LAYERS0(pCL,pCh->pCLayer))
      { 
       pCL->fNO3N            = (float)23 + (float)0.1*iLayer;
       pCL->fNH4N            = (float)24 + (float)0.1*iLayer;
      }                             
      
  pCh->pCBalance->fNO3NProfile     = (float)62;
  pCh->pCBalance->fNH4NProfile     = (float)63;

  pCh->pCProfile->fUreaHydroDay    = (float)25;
  pCh->pCProfile->fNLitterMinerDay = (float)26;
  pCh->pCProfile->fNManureMinerDay = (float)27;
  pCh->pCProfile->fNHumusMinerDay  = (float)28;
  pCh->pCProfile->fNH4NitrDay      = (float)29;
  pCh->pCProfile->fNO3DenitDay     = (float)30;
  pCh->pCProfile->fNImmobDay       = (float)31;
  
  pCh->pCProfile->dNO3LeachCum     = (double)32;
  pCh->pCProfile->dNMinerCum       = (double)33;
  pCh->pCProfile->dNO3DenitCum     = (double)34;
  pCh->pCProfile->dNH4NitrCum      = (double)35;
  pCh->pCProfile->dNImmobCum       = (double)36;
  pCh->pCProfile->dNUptakeCum      = (double)37;
  
  pCh->pCBalance->dNBalance        = (double)38;
  pCh->pCBalance->dNProfile        = (double)39;
  pCh->pCBalance->dNInputCum       = (double)40;
  pCh->pCProfile->dN2OEmisCum      = (double)41;
  pCh->pCProfile->dNH3VolatCum     = (double)42;
  pCh->pCProfile->dNTotalLeachCum  = (double)43;
  
  pCh->pCProfile->fN2OEmisDay      = (float)44;
  pCh->pCProfile->fNH3VolatDay     = (float)45;

  pCh->pCProfile->fCH4ImisDay      = (float)46;
  pCh->pCProfile->fCO2EmisDay      = (float)47;
  pCh->pCProfile->dCH4ImisCum      = (double)48;
  pCh->pCProfile->dCO2EmisCum      = (double)49;
  
  for (pCL=pCh->pCLayer->pNext; pCL->pNext->pNext!=NULL; pCL=pCL->pNext)  
      { 
       pCL->fCLitter    = (float)500;
       pCL->fCManure    = (float)510;
       pCL->fNLitter    = (float)55;
       pCL->fNManure    = (float)54;
      }                             
      
  for (pSL=pSo->pSLayer->pNext; pSL->pNext->pNext!=NULL; pSL=pSL->pNext)  
      { 
       pSL->fCHumus     = (float)520;
       pSL->fNHumus     = (float)53;
      }                             

  pCh->pCProfile->fNHumusImmobDay  = (float)56;
  pCh->pCProfile->fNManureImmobDay = (float)57;
  pCh->pCProfile->fNLitterImmobDay = (float)58;

/****************************************************************************
 ****************************************************************************
 *				PLANT
 */                      
       if (pPl!=NULL){

	pPl->pDevelop->fDevStage = (float)1;  
	pPl->pDevelop->fStageSUCROS = (float)2;  

	  /*************************************************************************
       *   2. Biomass Distribution
       */                                                                        
	pPl->pBiomass->fGrainWeight = (float)3000; 
	pPl->pBiomass->fLeafWeight = (float)4000; 
	pPl->pBiomass->fStemWeight = (float)5000; 
	pPl->pBiomass->fRootWeight = (float)6000; 
	pPl->pBiomass->fStovWeight = (float)7000;

	pPl->pCanopy->fLAI = (float)8;
	pPl->pCanopy->fCropCoverFrac = (float)9;

	pPl->pCanopy->fTillerNumSq = (float)10; 
	pPl->pCanopy->fTillerNum   = (float)201;
	pPl->pRoot->fDepth         = (float)202;
	pPl->pBiomass->fTotalBiomass = (float)203;
	pPl->pPltNitrogen->fRootCont = (float)204;
	pPl->pPltNitrogen->fStovCont = (float)205;  
	pPl->pPltNitrogen->fVegActConc = (float)206;
	pPl->pPltNitrogen->fActNUpt    = (float)207;

  for (SOIL_LAYERS0(pLR, pPl->pRoot->pLayerRoot))   
			  {
			pLR->fLengthDens = (float)11 + (float)0.1*iLayer;
			  }

	pPl->pPltWater->fPotTranspDay = (float)16;
    
	pPl->pPltNitrogen->fCumActNUpt = (float)19;
	pPl->pPltWater->fCumUptake = (float)20;

	pPl->pPltNitrogen->fTopsOptConc = (float)21;  
	pPl->pPltNitrogen->fTopsActConc = (float)22;
	pPl->pPltNitrogen->fGrainConc = (float)23;

// ch, noch nicht definiert: 	pPl->pPltNitrogen->fLeafNc = 24;
// ch, noch nicht definiert: 	pPl->pPltNitrogen->fStemNc = 25;

	pPl->pPltNitrogen->fRootActConc = (float)26;
        } // if plant

/****************************************************************************
 ****************************************************************************
 *				TEMPERATURE
 */                      

	pCl->pWeather->fTempAve = (float)1;
      
  for (SOIL_LAYERS0(pHL, pHe->pHLayer))   
	{
	  pHL->fSoilTempAve = (float)2 + (float)0.1*iLayer;
	}
       
  for (SOIL_LAYERS0(pWL, pWa->pWLayer))   
			{
		pWL->fIce = (float)3 + (float)0.1*iLayer;
			}    


/****************************************************************************
 ****************************************************************************
 *				DLL
 */                      
	pGr->fPlotDT11 = (float)11;
	pGr->fPlotDT12 = (float)12;
	pGr->fPlotDT13 = (float)13;
	pGr->fPlotDT14 = (float)14;
	pGr->fPlotDT15 = (float)15;
	pGr->fPlotDT16 = (float)16;

	pGr->fPlotDT21 = (float)21;
	pGr->fPlotDT22 = (float)22;
	pGr->fPlotDT23 = (float)23;
	pGr->fPlotDT24 = (float)24;
	pGr->fPlotDT25 = (float)25;
	pGr->fPlotDT26 = (float)26;

	strcpy(pGr->acPlotDZ1Titel, "DZ1 Titel");
	strcpy(pGr->acPlotZ1Text, "Z1 Text");
                                     
    strcpy(pGr->acPlotDZ2Titel, "DZ2 Titel");
	strcpy(pGr->acPlotZ2Text, "Z2 Text");
                                     
	strcpy(pGr->acDT1Titel, "DT1 Titel");
	strcpy(pGr->acPlot11Text, "Plot 11 Text");
	strcpy(pGr->acPlot12Text, "Plot 12 Text");
	strcpy(pGr->acPlot13Text, "Plot 13 Text");
	strcpy(pGr->acPlot14Text, "Plot 14 Text");
	strcpy(pGr->acPlot15Text, "Plot 15 Text");
	strcpy(pGr->acPlot16Text, "Plot 16 Text");

	strcpy(pGr->acDT2Titel, "DT2 Titel");
	strcpy(pGr->acPlot21Text, "Plot 21 Text");
	strcpy(pGr->acPlot22Text, "Plot 22 Text");
	strcpy(pGr->acPlot23Text, "Plot 23 Text");
	strcpy(pGr->acPlot24Text, "Plot 24 Text");
	strcpy(pGr->acPlot25Text, "Plot 25 Text");
	strcpy(pGr->acPlot26Text, "Plot 26 Text");


  for (SOIL_LAYERS0(pWL, pWa->pWLayer))   
  {
     pGr->fPlotDZ1[iLayer]  =  (float)1 + (float)0.1*iLayer;
     pGr->fPlotDZ2[iLayer]  =  (float)2 + (float)0.1*iLayer; 
  }             
  
  /* Zusätzliche Inis, as, 110697 */
    pWa->fPondWater     = (float)101;
    pWa->pEvap->fActDay = (float)102;
    pWa->fLeaching      = (float)103;
    
    pWa->pWBalance->fOutput    = (float)104;
    pCh->pCBalance->dNOutputCum = (double)105;
    
    for (pCL=pCh->pCLayer->pNext; pCL->pNext->pNext!=NULL; pCL=pCL->pNext)  
      { 
        pCL->fNLitterMinerCum  = (float)106;
        pCL->fNHumusMinerCum   = (float)107;
      }
 
    pCh->pCProfile->fNMinerDay        = (float)111;   
    pCh->pCProfile->fNO3LeachDay      = (float)112;
    pWa->fActTranspDay                = (float)17;        
            
 return 1;     
}

  
                         
                         
                         
int FloatToText(float fX, LPSTR lpTxt, int iWidth)
// Funktion zur Wandlung von Float->Text. Ursprungsverfasser unbekannt.
// wesentlich abgeändert: as, 17.07.97
{
  int    i;
  float  dec;
  char   cSign[2]="\0";
  char   *ac;
  long   lX1, lX2;
               
  ac=_MALLOC(sizeof(char)*50);          // Speicher für ac allokieren   
  if (ac == NULL) return -1;           // wenn fehlgeschlagen, Fault!
  
  if (fX < 0.0)    {   fX *= (float)-1.0;    // Vorzeichen auslesen und Betrag bilden.
                       lstrcpy(cSign,"-");   // Vorzeichen vermerken
                   }
  
  lX1 = (long) fX;                     // Vorkommastellen auslesen und nach lX1 schreiben
  
  for (i=0,dec=(float)1;i<iWidth;i++) dec *= (float)10;   // Exponent ermitteln.
  lX2 = (long) (((fX-(float)lX1)* dec)+0.5);    // Nachkommastellen ermitteln und nach lX2
  
  strcpy(lpTxt,cSign);
  ltoa(lX1, ac, 10);
  strcat(lpTxt,ac);
  if (iWidth>0) {                               // Nachkommas dazu, wenn erwünscht.
                  strcat(lpTxt,".");
                  ltoa(lX2, ac, 10);
                  for (i=strlen(ac);i<iWidth;i++) strcat(lpTxt, "0"); 
                  /* ^ Da bei der Ermittlung der Nachkommastellen führende Nullen im Nach-
                       kommata durch die Potenzierung verlorengehen, müssen sie hier wieder
                       eingefügt werden. Und zwar soviele, wie der Gesamtlänge fehlen. */
                  strcat(lpTxt,ac);
                }
  free(ac);
  return 1;
}
                         

/***************************************************************************************
 * Hilfsfunktion: DoubleToText                                                         *
 * Beschreibung: Wandelt eine Doublezahl in einen String um.                           *
 *                                                                                     *
 * Autor: Andreas Sonnenbichler                                                        *
 * Kennungen: as, 17/18.07.97                                                          *
 *                                                                                     *
 ***************************************************************************************/                         
int DoubleToText(double dX, char *lpTxt, int iWidth)
{
  int    i;
  float  dec;
  char   cSign[2]="\0";
  char   *ac;
  long   lX1, lX2;
               
  ac=_MALLOC(sizeof(char)*80);          // Speicher für ac allokieren   
  if (ac == NULL) return -1;           // wenn fehlgeschlagen, Fault!
  
  if (dX < (double)0) {   dX *= (float)-1.0;   // Vorzeichen auslesen und Betrag bilden.
                          strcpy(cSign,"-");   // Vorzeichen vermerken
                      }
  
  lX1 = (long) dX;                    // Vorkommastellen auslesen und nach lX1 schreiben
  
  for (i=0,dec=(float)1;i<iWidth;i++) dec *= (float)10;   // Exponent ermitteln.
  lX2 = (long) (((dX-(double)lX1)* dec)+0.5);    // Nachkommastellen ermitteln und nach lX2
  
  strcpy(lpTxt,cSign);
  ltoa(lX1, ac, 10);
  strcat(lpTxt,ac);
  if (iWidth>0) {                               // Nachkommas dazu, wenn erwünscht.
                  strcat(lpTxt,".");
                  ltoa(lX2, ac, 10);
                  for (i=lstrlen(ac);i<iWidth;i++) strcat(lpTxt, "0"); 
                  /* ^ Da bei der Ermittlung der Nachkommastellen führende Nullen im Nach-
                       kommata durch die Potenzierung verlorengehen, müssen sie hier wieder
                       eingefügt werden. Und zwar soviele, wie der Gesamtlänge fehlen. */
                  strcat(lpTxt,ac);
                }
  free(ac);
  return 1;
} /* EOF DoubleToText */
                    

/******************************************************************************
** EOF */
