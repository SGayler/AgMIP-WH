/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *    Encapsulated functions to print out Expert-N simulation results on
 *    different Files:
 *
 *      water:    filename.rfw
 *      heat:     filename.rfh
 *      nitrogen: filename.rfn
 *      carbon:   filename.rfc
 *      plant:    filename.rfp
 *      balance:  filename.rfb
 *      Protocol: filename.log
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 8 $
 *
 * $History: xfileout.c $
 * 
 * *****************  Version 8  *****************
 * User: Christian Bauer Date: 24.01.02   Time: 15:45
 * Updated in $/Projekte/ExpertN/ModLib/control
 * OpenFile only, if file is not already open to avoid
 * FILE_SHARE_VIOLATION error.
 * 
 * *****************  Version 7  *****************
 * User: Christian Bauer Date: 24.01.02   Time: 11:38
 * Updated in $/Projekte/ExpertN/ModLib/control
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:01
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/control
 * Zur Stabilisierung des Codes Methoden Prototypen Definition in Header
 * Files verschoben statt über extern Declarationen festgelegt.
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 25.12.01   Time: 10:02
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Carbon Result File mit Carbon Header (statt Nitrogen Header).
 * Unterteilung in statische und öffentliche Methoden.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:25
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Removed unused methods.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/control
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 *   12.08.94   / Sperr Christoph
 *   18.08.94   / Sperr C.
 *   18.02.97   / Sonnenbichler, Andreas
 *   17.07.01   / Priesack, Eckart
 *
*******************************************************************************/

#include <windows.h>
#include <time.h>
#include <crtdbg.h>
#include "trace.h"
#include "xinclexp.h"
#include "xlanguag.h"
#include "xmemory.h"
#include "xfileout.h"


// extern: aus xplot.c
extern int EndDay(PTIME);

// extern: aus XDataOut.c
extern int far pascal out_water(HANDLE hFileWater, EXP_POINTER);
extern int far pascal out_heat(HANDLE hFileHeat, EXP_POINTER);
extern int far pascal out_nitrogen(HANDLE hFileNitro, EXP_POINTER);
extern int far pascal out_carbon(HANDLE hFileNitro, EXP_POINTER);
extern int far pascal out_plant(HANDLE hFilePlant, EXP_POINTER);
extern int far pascal out_balance(HANDLE hFileBalance, EXP_POINTER);
extern int far pascal out_DSSAT_Sum(HANDLE hFileDSSATSum, EXP_POINTER);
extern int far pascal out_DSSAT_Growth(HANDLE hFileDSSATGrowth, EXP_POINTER);
extern int far pascal out_DSSAT_Carbon(HANDLE hFileDSSATCarbon, EXP_POINTER);
extern int far pascal out_DSSAT_Water(HANDLE hFileDSSATWater, EXP_POINTER);
extern int far pascal out_DSSAT_Nitrogen(HANDLE hFileDSSATNitrogen, EXP_POINTER);


/*==============================================================================
Public Variables
==============================================================================*/

extern HANDLE  hFileLog = INVALID_HANDLE_VALUE;


/*=============================================================================
Static Method Interface Description
=============================================================================*/

static int XwriteDSSATSumHeader(void);  
static int XwriteDSSATWaterHeader(void);
static int XwriteDSSATCarbonHeader(void);
static int XwriteDSSATNitrogenHeader(void);
static int XwriteDSSATGrowthHeader(void);

static int XwriteExpLogHeader(void);
static int XwriteExpWaterHeader(void);
static int XwriteExpHeatHeader(void);
static int XwriteExpNitroHeader(void);
static int XwriteExpCarboHeader(void);
static int XwriteExpPlantHeader(void);
static int XwriteExpBalanceHeader(void);

static void fout_KopfWater(HANDLE);
static void fout_KopfLog(HANDLE);
static void fout_KopfHeat(HANDLE);
static void fout_KopfNitro(HANDLE);
static void fout_KopfCarbon(HANDLE);
static void fout_KopfPlant(HANDLE);
static void fout_KopfBalance(HANDLE);
static void fout_KopfDSSAT_Sum(HANDLE, EXP_POINTER);
static void fout_KopfDSSAT_Growth(HANDLE, EXP_POINTER);
static void fout_KopfDSSAT_Carbon(HANDLE, EXP_POINTER);
static void fout_KopfDSSAT_Water(HANDLE, EXP_POINTER);
static void fout_KopfDSSAT_Nitrogen(HANDLE, EXP_POINTER);


/*==============================================================================
Static Variables
==============================================================================*/

static HANDLE  hDSSATSumFile = INVALID_HANDLE_VALUE;
static HANDLE  hDSSATWaterFile = INVALID_HANDLE_VALUE;
static HANDLE  hDSSATGrowthFile = INVALID_HANDLE_VALUE;
static HANDLE  hDSSATCarbonFile = INVALID_HANDLE_VALUE;
static HANDLE  hDSSATNitrogenFile = INVALID_HANDLE_VALUE;

static HANDLE  hFileWater = INVALID_HANDLE_VALUE;
static HANDLE  hFileHeat = INVALID_HANDLE_VALUE;
static HANDLE  hFileNitrogen = INVALID_HANDLE_VALUE;
static HANDLE  hFileCarbon = INVALID_HANDLE_VALUE;
static HANDLE  hFilePlant = INVALID_HANDLE_VALUE;
static HANDLE  hFileBalance = INVALID_HANDLE_VALUE;
    

static BOOL    bDSSATAll       = FALSE;
static BOOL    bExpertFWater   = TRUE;
static BOOL    bExpertFHeat    = TRUE;
static BOOL    bExpertFNitro   = TRUE;
static BOOL    bExpertFCarbo   = TRUE;
static BOOL    bExpertFPlant   = TRUE;
static BOOL    bExpertFBalance = TRUE; 



/*==============================================================================
Public Method Implementation
==============================================================================*/

//------------------------------------------------------------------------------
BOOL XOpenValidExpOutputfiles( void )
// Call all file open functions for expert-N - Output file system
//------------------------------------------------------------------------------
{ 
  char       szNumVal[11];
  char       szFile[18];
  char       szPath[25];
  char       szPathWithExt[29];
  PLOCATION  pLo = GetLocationPoi();
  PTIME      pTi = GetTimePoi();

  _ASSERTE( pLo != NULL );
  _ASSERTE( pTi != NULL );

  lstrcpyn( szFile, pLo->pFieldplot->acName, 7 );
  itoa( (int)(pTi->pSimTime->lStopDate % 100),szNumVal,10);
  lstrcat(szFile,szNumVal);
  
  strcpy(szPath,"result\\");
  strcat(szPath,szFile);

  if( hFileLog == INVALID_HANDLE_VALUE )
  {
    strcpy(szPathWithExt,szPath);
    strcat(szPathWithExt,".log");
    hFileLog = OpenF(szPathWithExt);
  }
 
  if( hFileWater == INVALID_HANDLE_VALUE )
  {
    strcpy(szPathWithExt,szPath);
    strcat(szPathWithExt,".rfw");
    hFileWater = OpenF(szPathWithExt);
  }
    
  if( hFileHeat == INVALID_HANDLE_VALUE )
  {
    strcpy(szPathWithExt,szPath);
    strcat(szPathWithExt,".rfh");
    hFileHeat = OpenF(szPathWithExt);
  }
    
  if( hFileNitrogen == INVALID_HANDLE_VALUE )
  {
    strcpy(szPathWithExt,szPath);
    strcat(szPathWithExt,".rfn");
    hFileNitrogen = OpenF(szPathWithExt);
  }

  if( hFileCarbon == INVALID_HANDLE_VALUE )
  {
    strcpy(szPathWithExt,szPath);
    strcat(szPathWithExt,".rfc");
    hFileCarbon = OpenF(szPathWithExt);
  }

  if( hFileBalance == INVALID_HANDLE_VALUE )
  {
    strcpy(szPathWithExt,szPath);
    strcat(szPathWithExt,".rfb"); 
    hFileBalance = OpenF(szPathWithExt);
  }
    
  if( hFilePlant == INVALID_HANDLE_VALUE )
  {
    strcpy(szPathWithExt,szPath);
    strcat(szPathWithExt,".rfp");
    hFilePlant = OpenF(szPathWithExt);
  }
       
  if( ( hFileLog       == INVALID_HANDLE_VALUE )
   || ( hFileWater     == INVALID_HANDLE_VALUE )
   || ( hFileHeat      == INVALID_HANDLE_VALUE )
   || ( hFileNitrogen  == INVALID_HANDLE_VALUE )
   || ( hFileCarbon    == INVALID_HANDLE_VALUE )
   || ( hFileBalance   == INVALID_HANDLE_VALUE )
   || ( hFilePlant     == INVALID_HANDLE_VALUE ) )
  {
    XCloseValidExpOutputfiles();
    return FALSE;
  }

  XwriteExpLogHeader();
  XwriteExpWaterHeader();
  XwriteExpHeatHeader();
  XwriteExpNitroHeader();
  XwriteExpCarboHeader();
  XwriteExpPlantHeader();
  XwriteExpBalanceHeader();

  return TRUE;
}

//------------------------------------------------------------------------------
int XCloseValidExpOutputfiles(void)    
//------------------------------------------------------------------------------
{
  if( hFileLog != INVALID_HANDLE_VALUE )
  {
    CloseF( &hFileLog );
  }
  if( hFileWater != INVALID_HANDLE_VALUE )
  {
    CloseF( &hFileWater );
  }
  if( hFileHeat != INVALID_HANDLE_VALUE )
  {
    CloseF( &hFileHeat );
  }
  if( hFileNitrogen != INVALID_HANDLE_VALUE )
  {
    CloseF( &hFileNitrogen );
  }
  if( hFileCarbon != INVALID_HANDLE_VALUE )
  {
    CloseF( &hFileCarbon );
  }
  if( hFileBalance != INVALID_HANDLE_VALUE )
  {
    CloseF( &hFileBalance );
  }
  if( hFilePlant != INVALID_HANDLE_VALUE )
  {
    CloseF( &hFilePlant );
  }
  return TRUE;
}                   


//------------------------------------------------------------------------------
int XToggleAllDSSATFileStatus(void)
//------------------------------------------------------------------------------
{ 
  bDSSATAll = !bDSSATAll;
  return 0;
}                                    

//------------------------------------------------------------------------------
int XToggleExpFWaterStatus(void)
//------------------------------------------------------------------------------
{ 
  bExpertFWater = !bExpertFWater; 
  return 0;
}

//------------------------------------------------------------------------------
int XToggleExpFHeatStatus(void)
//------------------------------------------------------------------------------
{ 
  bExpertFHeat = !bExpertFHeat; 
  return 0;
}

//------------------------------------------------------------------------------
int XToggleExpFNitroStatus(void)
//------------------------------------------------------------------------------
{ 
  bExpertFNitro = !bExpertFNitro; 
  return 0;
}

//------------------------------------------------------------------------------
int XToggleExpFCarbonStatus(void)
//------------------------------------------------------------------------------
{ 
  bExpertFCarbo = !bExpertFCarbo; 
  return 0;
}

//------------------------------------------------------------------------------
int XToggleExpFPlantStatus(void)
//------------------------------------------------------------------------------
{ 
  bExpertFPlant = !bExpertFPlant;
  return 0;
}                          

//------------------------------------------------------------------------------
int XToggleExpFBalanceStatus(void)
//------------------------------------------------------------------------------
{ 
  bExpertFBalance = !bExpertFBalance;
  return 0;
}                          



//------------------------------------------------------------------------------
int XwriteExpertDaily(void)
// Write simulation results to all enabled Output files
//------------------------------------------------------------------------------
{
  int iRet=1;          
  static BOOL bfirst;   // dummy, um ersten Aufruf der Ausgabefkt. zu kennzeichnen.

  PGRAPHIC    pGr = GetGraphicPoi();
  PMANAGEMENT pMa = GetManagementPoi();
  PLOCATION   pLo = GetLocationPoi();
  PTIME       pTi = GetTimePoi();
  PSPROFILE   pSo = GetSoilPoi();
  PCHEMISTRY  pCh = GetChemistryPoi();
  PPLANT      pPl = GetPlantPoi();
  PCLIMATE    pCl = GetClimatePoi();
  PWATER      pWa = GetWaterPoi();
  PHEAT       pHe = GetHeatPoi();
 
  if (EndDay(pTi))
  {  
    if (bDSSATAll)
    {
      out_DSSAT_Sum(hDSSATSumFile,exp_p);
      out_DSSAT_Growth(hDSSATGrowthFile,exp_p);
      out_DSSAT_Water(hDSSATWaterFile,exp_p);
      out_DSSAT_Carbon(hDSSATCarbonFile,exp_p);
      out_DSSAT_Nitrogen(hDSSATNitrogenFile,exp_p);
    }
    if (bExpertFWater && hFileWater!=NULL)  
        out_water(hFileWater,exp_p);
    if (bExpertFHeat && hFileHeat!=NULL) 
        out_heat(hFileHeat,exp_p);
    if (bExpertFNitro && hFileNitrogen!=NULL)
       out_nitrogen(hFileNitrogen,exp_p);
    if (bExpertFCarbo && hFileCarbon!=NULL)
       out_carbon(hFileCarbon,exp_p);
    if (bExpertFPlant && hFilePlant!=NULL)   
         out_plant(hFilePlant,exp_p);
    if (bExpertFBalance && hFileBalance!=NULL)   
         out_balance(hFileBalance,exp_p);          
    
    bfirst=FALSE;              
  }

  return iRet;
}

//------------------------------------------------------------------------------
HANDLE OpenF( char *i_szFileName )
//------------------------------------------------------------------------------
{
  HANDLE hFile = INVALID_HANDLE_VALUE;

  hFile = CreateFile(
    /* lpFileName             */  i_szFileName
    /* dwDesiredAccess        */ ,GENERIC_READ | GENERIC_WRITE
    /* dwShareMode            */ ,0
    /* lpSecurityAttributes   */ ,NULL
    /* dwCreationDistribution */ ,OPEN_ALWAYS
    /* dwFlagsAndAttributes   */ ,FILE_ATTRIBUTE_NORMAL
    /* hTemplateFile          */ ,NULL );  

  M_TRACE_MESSAGEBOX_IF(
    /* Condition */  hFile == INVALID_HANDLE_VALUE
    /* Method    */ ,"CreateFile()"
    /* AddInfo1  */ ,i_szFileName
    /* AddInfo2  */ ,TraceLastError2Str(TRACE_GET_LAST_ERROR)
    /* AddInfo3  */ ,NULL );

  if( GetLastError() == ERROR_ALREADY_EXISTS )
  {
    SetLastError(NO_ERROR);
  }

  return hFile;
}

//------------------------------------------------------------------------------
void CloseF( HANDLE *i_phFile )
//------------------------------------------------------------------------------
{ 
  _ASSERTE( i_phFile != NULL );

  if ( *i_phFile != INVALID_HANDLE_VALUE )
  {
    fout_line( *i_phFile, 1 );
    CloseHandle( *i_phFile );
    *i_phFile = INVALID_HANDLE_VALUE;
  }
}

//------------------------------------------------------------------------------
int  fout_floatNorm(HANDLE hFile, float f1,int iDecimal, int iVorkomma)
//------------------------------------------------------------------------------
{
  int i,j, Laenge;
  DWORD dwBytesWritten = 0;
  char ch1[30];
  char out[30];
  char * lptOut = out;
  char * ch = ch1;


  if (hFile != INVALID_HANDLE_VALUE)
  {
      memset(out,0,10);

      j = FloatToCharacter(ch,f1,iDecimal);

      if(iDecimal < 1) Laenge = iVorkomma;
      else             Laenge = iVorkomma + iDecimal + 1;

      i = Laenge - j;                    
      
      /* ch, 27.5.97 */
      if (i>0)
      {
        memset(out,32,i);
        lstrcpy(out + i,ch);
      }                                           
      else
        lstrcpy(out,ch);

      WriteFile(
        /* hFile                  */ hFile,
        /* lpBuffer               */ out,
        /* nNumberOfBytesToWrite  */ Laenge,
        /* lpNumberOfBytesWritten */ &dwBytesWritten,
        /* lpOverlapped           */ NULL );  
  }
  //---------------  only if hFile was opened by OpenDbgFile()

 return 0;
}


        
//------------------------------------------------------------------------------
int  fout_line(HANDLE hFile, int iNr)
//------------------------------------------------------------------------------
{
  char  ch1[2];
  int  i =0;
  DWORD dwBytesWritten = 0;

  if  (hFile != INVALID_HANDLE_VALUE)
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
          /* lpOverlapped           */ NULL );  
        i++;
      }

  }
  //---------------  only if hFile was opened by OpenDbgFile()

  return i;
}


//------------------------------------------------------------------------------
int  fout_space(HANDLE hFile, int iNr)
//------------------------------------------------------------------------------
{
  int i;
  DWORD dwBytesWritten = 0;
  char acDummy[2];

  if (hFile != INVALID_HANDLE_VALUE)
  {
    acDummy[0] = 0x20;
    acDummy[1] =(char)0;
    for (i=0;i<iNr;i++)
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


//------------------------------------------------------------------------------
int  fout_long(HANDLE hFile, long far *plWert)
//------------------------------------------------------------------------------
{
  int i;
  DWORD dwBytesWritten = 0;
  char ch1[50];
  char * ch = ch1;

  if (hFile != INVALID_HANDLE_VALUE)
  {
    wsprintf((LPSTR)ch,"%ld",*plWert);
    i = strlen(ch);
    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ ch,
      /* nNumberOfBytesToWrite  */ i,
      /* lpNumberOfBytesWritten */ &dwBytesWritten,
      /* lpOverlapped           */ NULL );  
  }
  //---------------  only if hFile was opened by OpenDbgFile()

  return 0;
}

//------------------------------------------------------------------------------
int  foutDate_long(HANDLE hFile, long far *plWert)
//------------------------------------------------------------------------------
{
  DWORD  dwBytesWritten = 0;
  char   szDate[7];

  _ASSERTE( *plWert < 31122100 );

  if( hFile != INVALID_HANDLE_VALUE )
  {
    wsprintf(szDate,"%06ld",*plWert);
    WriteFile(
      /* hFile                  */  hFile
      /* lpBuffer               */ ,szDate
      /* nNumberOfBytesToWrite  */ ,6
      /* lpNumberOfBytesWritten */ ,&dwBytesWritten
      /* lpOverlapped           */ ,NULL );  
  }

  return 0;
}

//------------------------------------------------------------------------------
int  fout_int(HANDLE hFile, int far *piWert, int Laenge)
//------------------------------------------------------------------------------
{
  char acdummy[30];
  char *pacdummy = acdummy;

  int i;
  char out[30];
  char * lptOut = out;


  if (hFile != INVALID_HANDLE_VALUE)
  {
    memset(out,0,30);

    wsprintf((LPSTR)pacdummy,"%d",*piWert);
    i = Laenge - strlen(pacdummy);
    memset(out,32,i);
    lstrcpy(out + i,pacdummy);

    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ out,
      /* nNumberOfBytesToWrite  */ Laenge,
      /* lpNumberOfBytesWritten */ &i,
      /* lpOverlapped           */ NULL );  
  }
  return i;
}



//------------------------------------------------------------------------------
int  fout_string(HANDLE hFile, LPSTR lpString)
//------------------------------------------------------------------------------
{
  int i;
  DWORD dwBytesWritten = 0;
  //char ch1[375]; 
  char ch1[700]; //SG 20160801
  char * ch = ch1;

  if (hFile != INVALID_HANDLE_VALUE)
  {
    wsprintf((LPSTR)ch,"%s",lpString);
    i = strlen(ch);
    WriteFile(
      /* hFile                  */ hFile,
      /* lpBuffer               */ ch,
      /* nNumberOfBytesToWrite  */ i,
      /* lpNumberOfBytesWritten */ &dwBytesWritten,
      /* lpOverlapped           */ NULL );  
  }
  //---------------  only if hFile was opened by OpenDbgFile()

 return 0;
}

//------------------------------------------------------------------------------
int FloatToCharacter(LPSTR lptOut,float fNumber,int iDecimal)
//------------------------------------------------------------------------------
  {
  char    lpTxt[80];
  int   iBefore,iAfter,multiplicator,position;
    int    iFirstChar  = 1;
    int   iMaxDecimal = 4;

    position = 0;

    if (iDecimal > iMaxDecimal)
      iDecimal = iMaxDecimal;
    if(fNumber < 0)
      {
      iFirstChar= -1;      position = 1;      lpTxt[0]='-';
      fNumber = fNumber * iFirstChar;
      }

    multiplicator = (int)pow (10,iDecimal);

  iBefore = 0;  iAfter  = 0;

  iBefore = (int)floor(fNumber);
  iAfter  = (int)floor((fNumber - iBefore)* multiplicator +0.5);

  if(iAfter == multiplicator)
    {
    iBefore = iBefore + 1;
    iAfter = 0;
    }

  if(iDecimal == 0)
    wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d", iBefore);
  if(iDecimal == 1)
    wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%01d", iBefore,iAfter);
  if(iDecimal == 2)
    wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%02d", iBefore,iAfter);
  if(iDecimal == 3)
    wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%03d", iBefore,iAfter);
  if(iDecimal == 4)
    wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%04d", iBefore,iAfter);
  if(iDecimal == 5)
    wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%05d", iBefore,iAfter);

  //TextOut(hdc,X,Y,lpTxt,lstrlen(lpTxt));
    lstrcpy(lptOut,(LPSTR)lpTxt);

  return lstrlen(lpTxt);
}


/*==============================================================================
Static Method Implementation
==============================================================================*/

//------------------------------------------------------------------------------
int XwriteDSSATSumHeader(void)  
// Write simulation results to all enabled Output files
//------------------------------------------------------------------------------
{ 
  int iRet=0;           
  fout_line(hDSSATSumFile, 1);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteDSSATWaterHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hDSSATWaterFile, 1);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteDSSATCarbonHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hDSSATCarbonFile, 1);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteDSSATNitrogenHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;          
  fout_line(hDSSATNitrogenFile, 1);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteDSSATGrowthHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hDSSATGrowthFile, 1);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteExpLogHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hFileLog,1);
  fout_KopfLog(hFileLog);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteExpWaterHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hFileWater,1);
  fout_KopfWater(hFileWater);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteExpHeatHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hFileHeat, 1);
  fout_KopfHeat(hFileHeat); 
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteExpNitroHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hFileNitrogen,1);
  fout_KopfNitro(hFileNitrogen);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteExpCarboHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hFileCarbon,1);
  fout_KopfCarbon(hFileCarbon);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteExpPlantHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hFilePlant, 1);
  fout_KopfPlant(hFilePlant);
  return iRet;
}

//------------------------------------------------------------------------------
int XwriteExpBalanceHeader(void)
//------------------------------------------------------------------------------
{ int iRet=0;
  fout_line(hFileBalance, 1);
  fout_KopfBalance(hFileBalance);
  return iRet;
}


//------------------------------------------------------------------------------
void fout_KopfLog(HANDLE hFile)
//------------------------------------------------------------------------------
{ 
  char acDummy[15]; 
    
    fout_string(hFile,"E X P E R T - N:  LOG-FILE"); 
    fout_line(hFile, 1);
      _strdate(acDummy);            
    fout_string(hFile,"Date: "); fout_string(hFile, acDummy); fout_line(hFile, 1);
      _strtime(acDummy);            
    fout_string(hFile,"Time: "); fout_string(hFile, acDummy); 
    fout_line(hFile, 1); fout_line(hFile, 1);
                              
    fout_string(hFile, TITLE_LOG_1);  fout_line(hFile, 1);
}

//------------------------------------------------------------------------------
void fout_KopfWater(HANDLE hFile)
//------------------------------------------------------------------------------
 { 
  char acDummy[15]; 
  int i;
    
    fout_string(hFile,"E X P E R T - N:  OUTPUTFILES"); fout_line(hFile, 1);
    fout_string(hFile,"---> Water <---"); fout_line(hFile, 1);
      _strdate(acDummy);            
    fout_string(hFile,"Date: "); fout_string(hFile, acDummy); fout_line(hFile, 1);
      _strtime(acDummy);            
    fout_string(hFile,"Time: "); fout_string(hFile, acDummy); 
    fout_line(hFile, 1); fout_line(hFile, 1);
                              
    fout_string(hFile, TITLE_WATER_1);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_2);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_3);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_4);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_5);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_6);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_7);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_8);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_9);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_10); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_11); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_12); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_13); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_14); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_15); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_16); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_17); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_18); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_19); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_20); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_WATER_21); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_WATER_22); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_WATER_23); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_WATER_24); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_WATER_25); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_WATER_26); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_WATER_27); fout_line(hFile, 1); 

    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_WATER_LINE_1); fout_line(hFile, 1);
    fout_string(hFile, TITLE_WATER_LINE_2); fout_line(hFile, 1);
        
    for (i=0; i<=TITLE_WATER_NUM*(6+2+4); i++) fout_string(hFile,"-");
      fout_line(hFile, 1);
  
}


//------------------------------------------------------------------------------
void fout_KopfHeat(HANDLE hFile)
//------------------------------------------------------------------------------
{        
 char acDummy[15];
 unsigned int i; 
   
    fout_string(hFile,"E X P E R T - N:  OUTPUTFILES"); fout_line(hFile, 1);
    fout_string(hFile,"---> Heat <---"); fout_line(hFile, 1);
    _strdate(acDummy);            
    fout_string(hFile,"Date: "); fout_string(hFile, acDummy); fout_line(hFile, 1);
    _strtime(acDummy);            
    fout_string(hFile,"Time: "); fout_string(hFile, acDummy); 
    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_HEAT_1);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_2);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_3);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_4);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_5);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_6);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_7);  fout_line(hFile, 1);
    fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_8);  fout_line(hFile, 1);
    
    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_HEAT_LINE_1); fout_line(hFile, 1);
    fout_string(hFile, TITLE_HEAT_LINE_2); fout_line(hFile, 1);
              
    for (i=0; i<=TITLE_HEAT_NUM*(6+2); i++) fout_string(hFile,"-");
      fout_line(hFile, 2);
}


//------------------------------------------------------------------------------
void fout_KopfNitro(HANDLE hFile)
//------------------------------------------------------------------------------
{
  char acDummy[15]; 
  unsigned int i;
                          
    fout_string(hFile,"E X P E R T - N:  OUTPUTFILES"); fout_line(hFile, 1);                     
    fout_string(hFile,"---> Nitrogen <---"); fout_line(hFile, 1);
    _strdate(acDummy);            
    fout_string(hFile,"Date: "); fout_string(hFile, acDummy); fout_line(hFile, 1);
    _strtime(acDummy);            
    fout_string(hFile,"Time: "); fout_string(hFile, acDummy); 
    fout_line(hFile, 1); fout_line(hFile,1);                                   
                                      
    fout_string(hFile, TITLE_NITRO_1);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_2);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_3);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_4);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_5);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_6);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_7);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_8);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_9);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_10); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_11); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_12); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_13); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_14); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_15); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_16); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_17); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_18); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_19); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_NITRO_20); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_21); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_22); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_23); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_24); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_25); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_26); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_27); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_28); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_29); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_30); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_31); fout_line(hFile, 1);
    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_NITRO_LINE_1); fout_line(hFile, 1);
    fout_string(hFile, TITLE_NITRO_LINE_2); fout_line(hFile, 1);
                                           
      
   for (i=0; i<=TITLE_NITRO_NUM*(6+2); i++) fout_string(hFile,"-");
      fout_line(hFile, 1);
  
}


//------------------------------------------------------------------------------
void fout_KopfCarbon(HANDLE hFile)
//------------------------------------------------------------------------------
{
  char acDummy[15]; 
  unsigned int i;
                          
    fout_string(hFile,"E X P E R T - N:  OUTPUTFILES"); fout_line(hFile, 1);                     
    fout_string(hFile,"---> Carbon <---"); fout_line(hFile, 1);
    _strdate(acDummy);            
    fout_string(hFile,"Date: "); fout_string(hFile, acDummy); fout_line(hFile, 1);
    _strtime(acDummy);            
    fout_string(hFile,"Time: "); fout_string(hFile, acDummy); 
    fout_line(hFile, 1); fout_line(hFile,1);                                   
                                      
    fout_string(hFile, TITLE_CARBO_1);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_2);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_3);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_4);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_5);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_6);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_7);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_8);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_9);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_10); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_11); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_12); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_13); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_14); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_15); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_16); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_17); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_18); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_19); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_CARBO_20); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_21); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_22); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_23); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_24); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_25); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_26); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_27); fout_line(hFile, 1);
    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_CARBO_LINE_1); fout_line(hFile, 1);
    fout_string(hFile, TITLE_CARBO_LINE_2); fout_line(hFile, 1);
                                           
      
   for (i=0; i<=TITLE_CARBO_NUM*(6+2); i++) fout_string(hFile,"-");
      fout_line(hFile, 1);
  
}

//------------------------------------------------------------------------------
void fout_KopfPlant(HANDLE hFile)
//------------------------------------------------------------------------------
{    
 char acDummy[15];
 unsigned int i;
                          
    fout_string(hFile,"E X P E R T - N:  OUTPUTFILES"); fout_line(hFile, 1);                     
    fout_string(hFile,"---> Plant <---"); fout_line(hFile, 1);
    _strdate(acDummy);            
    fout_string(hFile," Date: "); fout_string(hFile, acDummy); fout_line(hFile, 1);
    _strtime(acDummy);            
    fout_string(hFile," Time: "); fout_string(hFile, acDummy); 
    fout_line(hFile, 1); fout_line(hFile, 1); 
    
    fout_string(hFile, TITLE_PLANT_1);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_2);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_3);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_4);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_5);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_6);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_7);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_8);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_9);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_10); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_11); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_12); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_13); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_14); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_15); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_16); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_17); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_18); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_19); fout_line(hFile, 1); 
    fout_string(hFile, TITLE_PLANT_20); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_21); fout_line(hFile, 1);
    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_PLANT_LINE_1); fout_line(hFile, 1);
    fout_string(hFile, TITLE_PLANT_LINE_2); fout_line(hFile, 1);
     
   for (i=0; i<=TITLE_PLANT_NUM*(6+2); i++) fout_string(hFile,"-");
      fout_line(hFile, 1);

}


//------------------------------------------------------------------------------
void fout_KopfBalance(HANDLE hFile)
//------------------------------------------------------------------------------
{
 char acDummy[15];
 unsigned int i;
                          
    fout_string(hFile,"E X P E R T - N:  OUTPUTFILES"); fout_line(hFile, 1);                      
    fout_string(hFile,"---> Balance <---"); fout_line(hFile, 1);
    _strdate(acDummy);            
    fout_string(hFile,"Date: "); fout_string(hFile, acDummy); fout_line(hFile, 1);
    _strtime(acDummy);            
    fout_string(hFile,"Time: "); fout_string(hFile, acDummy); 
    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_BALANCE_1);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_2);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_3);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_4);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_5);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_6);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_7);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_8);  fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_9);  fout_line(hFile, 1);
    fout_line(hFile, 1); fout_line(hFile, 1);
    
    fout_string(hFile, TITLE_BALANCE_LINE_1); fout_line(hFile, 1);
    fout_string(hFile, TITLE_BALANCE_LINE_2); fout_line(hFile, 1);
    
    
    for (i=0; i<=TITLE_BALANCE_NUM*(6+2); i++) fout_string(hFile,"-");
      fout_line(hFile, 1);

}


//------------------------------------------------------------------------------
void fout_KopfDSSAT_Sum(HANDLE hFileDSSATSum, EXP_POINTER)
//------------------------------------------------------------------------------
{
  /*
  long lStart;
//  lStart = lps->StartSim;

  fout_line(hFileDSSATSum,1);
  fout_string(hFileDSSATSum, "*");
  fout_space(hFileDSSATSum, 1);
//  fout_string(hFileDSSATSum, lps->lPtrBetrieb->Name);
//  fout_string(hFileDSSATSum, lps->lPtrTeilSchlag->TSName);
  fout_long(hFileDSSATSum, &lStart);
  fout_line(hFileDSSATSum,1);
  fout_line(hFileDSSATSum,1);

  fout_string(hFileDSSATSum, "@RN INSIEXNOCG TN ROC CR TITLE  ID-FIELD   SDAT  PDAT  ADAT  MDAT  HDAT  DWAP  CWAM  HWAM  HWAH  BWAH  HWUM  G#AM  ");
  fout_string(hFileDSSATSum, "G#UM  IR#M  IRRC  PREC  ETAC  ROFC  DRNC  SWXM  NI#M  NITC  NFXC  NLCC  NIAM  NUCM  HNAM  CNAM  RECM  ONAM  OCAM");

  fout_line(hFileDSSATSum,1);
  fout_line(hFileDSSATSum,1);
  */
  return;
}


//------------------------------------------------------------------------------
void fout_KopfDSSAT_Growth(HANDLE hFileDSSATGrowth, EXP_POINTER)
//------------------------------------------------------------------------------
{
  /*
  long lStart;
//  lStart = lps->StartSim;

  fout_line(hFileDSSATGrowth,1);
  fout_string(hFileDSSATGrowth, "*");
  fout_space(hFileDSSATGrowth, 10);
  fout_string(hFileDSSATGrowth, "RN");
  fout_space(hFileDSSATGrowth, 2);
//  fout_string(hFileDSSATGrowth, lps->lPtrBetrieb->Name);
//  fout_string(hFileDSSATGrowth, lps->lPtrTeilSchlag->TSName);
  fout_long(hFileDSSATGrowth, &lStart);
  fout_line(hFileDSSATGrowth,1);
  fout_line(hFileDSSATGrowth,1);

  fout_string(hFileDSSATGrowth, "@DATE   DAP  L#SD  GSTD  LAID  LWAD  SWAD  HWAD  RWAD  CWAD  H#AD  HWGD  HIAD");

  fout_line(hFileDSSATGrowth,1);
  fout_line(hFileDSSATGrowth,1);
  */
  return;
}


//------------------------------------------------------------------------------
void fout_KopfDSSAT_Carbon(HANDLE hFileDSSATCarbon, EXP_POINTER)
//------------------------------------------------------------------------------
{
  /*
  long lStart;
//  lStart = lps->StartSim;

  fout_line(hFileDSSATCarbon,1);
  fout_string(hFileDSSATCarbon, "*");
  fout_space(hFileDSSATCarbon, 10);
  fout_string(hFileDSSATCarbon, "RN");
  fout_space(hFileDSSATCarbon, 2);
//  fout_string(hFileDSSATCarbon, lps->lPtrBetrieb->Name);
//  fout_string(hFileDSSATCarbon, lps->lPtrTeilSchlag->TSName);
  fout_long(hFileDSSATCarbon, &lStart);
  fout_line(hFileDSSATCarbon,1);
  fout_line(hFileDSSATCarbon,1);

  fout_string(hFileDSSATCarbon, "@DATE   DAP  SOCD  TWAD  LI%D  PHAD  CMAD  CGRD  GRAD  MRAD  CHAD  CL%D  CS%D");

  fout_line(hFileDSSATCarbon,1);
  fout_line(hFileDSSATCarbon,1);
  */
  return;
}


//------------------------------------------------------------------------------
void fout_KopfDSSAT_Water(HANDLE hFileDSSATWater, EXP_POINTER)
//------------------------------------------------------------------------------
{
  /*
  long lStart;
//  lStart = lps->StartSim;

  fout_line(hFileDSSATWater,1);
  fout_string(hFileDSSATWater, "*");
  fout_space(hFileDSSATWater, 10);
  fout_string(hFileDSSATWater, "RN");
  fout_space(hFileDSSATWater, 2);
//  fout_string(hFileDSSATWater, lps->lPtrBetrieb->Name);
//  fout_string(hFileDSSATWater, lps->lPtrTeilSchlag->TSName);
  fout_long(hFileDSSATWater, &lStart);
  fout_line(hFileDSSATWater,1);
  fout_line(hFileDSSATWater,1);

  fout_string(hFileDSSATWater, "@DATE   DAP  EPAA  ETAA  EOAA  SWXD  ROFC  DRNC  PREC  IRRC  SRAA  TMXA  TMNA");

  fout_line(hFileDSSATWater,1);
  fout_line(hFileDSSATWater,1);
  */
  return;
 }


//------------------------------------------------------------------------------
void fout_KopfDSSAT_Nitrogen(HANDLE hFileDSSATNitrogen, EXP_POINTER)
//------------------------------------------------------------------------------
{ 
  /*
  long lStart;
//  lStart = lps->StartSim;

  fout_line(hFileDSSATNitrogen,1);
  fout_string(hFileDSSATNitrogen, "*");
  fout_space(hFileDSSATNitrogen, 10);
  fout_string(hFileDSSATNitrogen, "RN");
  fout_space(hFileDSSATNitrogen, 2);
//  fout_string(hFileDSSATNitrogen, lps->lPtrBetrieb->Name);
//  fout_string(hFileDSSATNitrogen, lps->lPtrTeilSchlag->TSName);
  fout_long(hFileDSSATNitrogen, &lStart);
  fout_line(hFileDSSATNitrogen,1);
  fout_line(hFileDSSATNitrogen,1);

  fout_string(hFileDSSATNitrogen, "@DATE   DAP  CNAD  HNAD  VNAD  HN%D  VN%D  NAPC  NFXC  NUPC  NLCC  NIAD  NOAD");

  fout_line(hFileDSSATNitrogen,1);
  fout_line(hFileDSSATNitrogen,1);
  */
  
  return;
}
 

/*******************************************************************************
** EOF */