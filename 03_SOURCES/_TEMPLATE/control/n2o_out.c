/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:  Christian Haberbosch
 *
 *------------------------------------------------------------------------------
 *
 * Description:  Additional Output routines
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: n2o_out.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 20:58
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Using arrays with defined length instead of malloc calls.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/control
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
 *   21.07.97
 *
*******************************************************************************/

#include <time.h>
#include <windows.h>
#include <crtdbg.h>
#include "xinclexp.h"
#include "xh2o_def.h"
#include "xn_def.h"

#include "xlanguag.h"     

#define LONG_THIN_LINE "----------------------------------------------------------------------------------------------------------------------"
#define LONG_FAT_LINE  "======================================================================================================================"
  
/* from xfileout.c */  
extern int  fout_floatNorm(HANDLE,float,int,int);
extern int  fout_line(HANDLE, int);
extern int  fout_space(HANDLE, int);
extern int  fout_long(HANDLE, long far *);
//extern int  fout_double(HANDLE,double far *,int);
extern int  fout_int(HANDLE,int far *,int);
extern int  fout_string(HANDLE,LPSTR);
extern int FloatToCharacter(LPSTR,float,int);

extern int DateToString(long lDate,LPSTR lpDate);

extern HANDLE  OpenF(char far * lptname);
extern void   CloseF(HANDLE *i_phFile);

extern int SimEnd(PTIME);

/* from util_fct.c */
extern int WINAPI Message(long, LPSTR);
            

/*==============================================================================
Static Type Definitions and Constants
==============================================================================*/


/*==============================================================================
Public Method Interface Description
==============================================================================*/

int WINAPI PrintN2OResult(EXP_POINTER);


/*==============================================================================
Public Method Implementation
==============================================================================*/


//------------------------------------------------------------------------------
int WINAPI PrintN2OResult(EXP_POINTER)
// Result File with nitrous oxide related variables, but also Water Content and NO3.
//------------------------------------------------------------------------------
{ 
  DECLARE_COMMON_VAR
  DECLARE_POINTER  

  static HANDLE hFile = INVALID_HANDLE_VALUE;

  float f1, f2, f3, f4, f5, f6;
  float f7,f8, f9, f10;
  int   i1, i2;
  char  szFileName[18];
  char  szDate[11];
  char  acDummy2[100];

  N_ZERO_LAYER
  
  if (hFile == INVALID_HANDLE_VALUE)
  {  
    if (strlen(pLo->pFieldplot->acName)<2)
    {
      lstrcpyn(szFileName,"expertn",7);
    }
    else
    {
      lstrcpyn(szFileName,pLo->pFieldplot->acName,7);
    }
    
    // test  pTi->pSimTime->lStopDate = 120394;       
    itoa( (int)(pTi->pSimTime->lStopDate % 100),szDate,10);
    lstrcat(szFileName,szDate);

    strcpy(acDummy2,szFileName);
    strcat(acDummy2,".rfx");   
    hFile = OpenF(acDummy2);

    if (hFile != INVALID_HANDLE_VALUE)
    {        
      strcat(acDummy2," opened nox result file.");
      Message(0, acDummy2);


      fout_string(hFile, LONG_FAT_LINE);
      fout_line(hFile, 1);     

      fout_string(hFile, "EXPERT-N : NOx Result File.");

      fout_string(hFile, " ");
      _strdate(acDummy2);            
      fout_string(hFile, acDummy2);

      fout_string(hFile, " ");

      _strtime(acDummy2);            
      fout_string(hFile, acDummy2);

      fout_line(hFile, 2);     

      fout_string(hFile, "Simulation: ");
      
      fout_string(hFile, szFileName); 
      
      fout_string(hFile,"\t\tDifferent Depths: I=[0-30cm], II=[30-60cm], III=[60-90cm], A=[0-15cm], B=[15-30cm]");

      fout_line(hFile, 1);

      fout_string(hFile, LONG_FAT_LINE);
      fout_line(hFile, 1);     

      fout_string(hFile,"Date\t\t   ");
      //fout_string(hFile,"SimTime\t");
      fout_string(hFile,"N2OEmis\t  ");
      fout_string(hFile,"NDenit     ");

      fout_string(hFile,"H2OI ");
      fout_string(hFile,"H2OII ");
      fout_string(hFile,"H2OIII ");
      fout_string(hFile,"H2OA ");
      fout_string(hFile,"H2OB ");

      fout_string(hFile,"NO3I ");
      fout_string(hFile,"NO3II ");
      fout_string(hFile,"NO3III ");
      fout_string(hFile,"NO3A ");
      fout_string(hFile,"NO3B ");

      fout_string(hFile,"\tDaily ");
      fout_string(hFile,"Profile ");
      fout_string(hFile,"Drained ");

      fout_string(hFile,"\tRewet ");
      fout_string(hFile,"Freezing\t");
      fout_line(hFile, 1);

      fout_string(hFile,"\t\t\t[kgN/ha]\t\t\t\t[Vol%]\t\t\t[kgN/ha]\t\t\t   N2O-N [gN/ha]");
      fout_line(hFile, 1);
      fout_string(hFile, LONG_THIN_LINE);
       
      fout_line(hFile, 1);
    } // file opened
    else
    {
      Message(1,FILEOPEN_ERROR_TXT);
    }
  }
  
  if (hFile != INVALID_HANDLE_VALUE)
  {
    DateToString(pTi->pSimTime->lTimeDate,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    /*
    fout_long(hFile, &(pTi->pSimTime->lTimeDate));
    fout_string(hFile,"\t");

    gcvt((double)pTi->pSimTime->fTimeAct,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");
    */
    
    gcvt((double)pCP->dN2OEmisCum,5,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");
                                  
    gcvt((double)pCP->dNO3DenitCum,5,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");


    // Anzahl der Schichten in den ersten 30cm:
    i1 = (int)(300 / pSo->fDeltaZ);
    i2 = (int)(150 / pSo->fDeltaZ);

    f1 =                    
    f2 =                    
    f3 =                    
    f4 =                    
    f5 =                    
    f6 = 
    f7 =                    
    f8 = 
    f9 =                    
    f10 = (float)0;                     

    for  (N_SOIL_LAYERS)
    { 
      if (iLayer*pSo->fDeltaZ <= 150)
	  {
		f7 += pWL->fContAct + pWL->fIce; 
		f9 += pCL->fNO3N;
	  } 
	  else if (iLayer*pSo->fDeltaZ <= 300)
	  {
		f8  += pWL->fContAct + pWL->fIce; 
		f10 += pCL->fNO3N;	  
	  }
	

      if (iLayer*pSo->fDeltaZ <= 300)
      {
        f1 += pWL->fContAct + pWL->fIce; 
        f4 += pCL->fNO3N; 
      }
      else if (iLayer*pSo->fDeltaZ <= 600)
      {
        f2 += pWL->fContAct + pWL->fIce; 
        f5 += pCL->fNO3N; 
      }
      else if (iLayer*pSo->fDeltaZ <= 900)
      {
        f3 += pWL->fContAct + pWL->fIce; 
        f6 += pCL->fNO3N; 
      }
	}

    f1 *= (float)100; 
    f2 *= (float)100; 
    f3 *= (float)100; 
    f7 *= (float)100; 
    f8 *= (float)100;  

    f1 /= (float)i1; 
    f2 /= (float)i1; 
    f3 /= (float)i1; 
    f7 /= (float)i2; 
    f8 /= (float)i2; 

    gcvt((double)f1,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    gcvt((double)f2,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    gcvt((double)f3,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

	gcvt((double)f7,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

	gcvt((double)f8,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    gcvt((double)f4,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    gcvt((double)f5,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    gcvt((double)f6,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

	gcvt((double)f9,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

	gcvt((double)f10,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");
    // ep 040299
	// ep 151009
    gcvt((double)pCP->fN2OEmisDay*(double)1000,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    gcvt((double)pCh->pCBalance->fN2ONProfile*(double)1000,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    gcvt(pCh->pCProfile->dN2ODrainCum*(double)1000,4,acDummy2);
    fout_string(hFile, acDummy2);
    fout_string(hFile,"\t");

    if (
      (DENIT_REWET_FACTOR > 1 + EPSILON) // this is corr.rewet
      ||
      (DENIT_FROST_FACTOR > 1 + EPSILON)   // this is corr.frost in the 1. layer
      ||
      (DENIT_FROST_FACTOR < 1 - EPSILON)   // this is corr.frost in the 1. layer
      )
    {
      gcvt((double)DENIT_REWET_FACTOR,4,acDummy2);
      fout_string(hFile, acDummy2);
      fout_string(hFile,"\t");

      gcvt((double)DENIT_FROST_FACTOR,4,acDummy2);
      fout_string(hFile, acDummy2);
      fout_string(hFile,"\t");
    }

    fout_line(hFile, 1);

    if (SimEnd(pTi)) CloseF( &hFile );

  } /* data output */

  return 1;
}

                                     
/*******************************************************************************
** EOF */
