/*******************************************************************************
 *
 * Copyright (c) by Thomas Schaaf
 *
 *------------------------------------------------------------------------------
 *
 * Contents:  Additional Output routines
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: gisdaout.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:39
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Using _MALLOC macro to detect memory leaks.
 * Detected memory leak (zwischenfrucht) removed. 
 * Using rather arrays with fixed size instead of mallocs.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/control
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 *
 *   07.02.98
 * 
 ******************************************************************************/

#include <time.h>
#include <crtdbg.h>

#include "xlanguag.h"
#include "xinclexp.h"
#include "xh2o_def.h"
#include "xn_def.h"
#include "project.h"

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

/* from util_fct.c */
extern int WINAPI Message(long, LPSTR);


/*==============================================================================
Public Method Interface Description
==============================================================================*/

int WINAPI PrintGisData(EXP_POINTER,int);


/*==============================================================================
Static Type Definitions and Constants
==============================================================================*/

#define THIN_LINE "----------------------------------------------------------------------"
#define FAT_LINE  "======================================================================"
#define LONG_THIN_LINE "----------------------------------------------------------------------------------------------------------------------"
#define LONG_FAT_LINE  "======================================================================================================================"


/*==============================================================================
Static Variables
==============================================================================*/

float   *Layer;


/*==============================================================================
Public Method Implementation
==============================================================================*/

//-----------------------------------------------------------------------------
void writeLayerSections(EXP_POINTER,float* Layer)
//-----------------------------------------------------------------------------
{
  DECLARE_COMMON_VAR
  // int hFile;

  PCLAYER    pCL = pCh->pCLayer->pNext;
  PSLAYER    pSL = pSo->pSLayer->pNext;

  int  s;

  // unsigned int i;
  float z1 = (float)0;    /* Startwert Simschicht */
  float z2 = (float)0;      /* Endwert Simschicht */
 

  /* Umrechnen auf 30, 60 90 cm Schichten. */
 
  if (Layer == NULL) return /*0*/;
  
  for (s=0;s<=3;s++)          /* Ausgabeschichten werden auf Null gesetzt */
      Layer[s] = (float)0;
      
  s = 1;                      /* s jetzt 1, daher while-wend bis s<=4 !!! */
  
  while (s<=4)
  { 
     if (pCL->pNext == NULL || pSL->pNext == NULL) 
        {
        //Layer[s-1] *= 300 / (z2 - (s-1)*300);    // Interpolation fehlende Werte
        if (fabs((double)z2-(double)s*300) > EPSILON ) Layer[s-1] = (float)-99;    // ... oder -99 ?
        for (s++;s<=4;s++) Layer[s-1]=(float)-99;
        return /*1*/;              /* Abbruchbedingung, wenn weniger Sim-Schichten als                             Ausgabeschichten */
        }
        
     z1 = z2;
     z2 += pSL->fThickness;   
     
     if (z2 <= s * 300)       /* Simschicht ganz in (s-1)-ter Ausgabeschicht */
       {
       Layer[s-1] += pCL->fNO3N;
       }
     else if ((z2 > s*300) && (z2 <= (s+1)*300))
       {                      /* Simschicht überschreitet eine Ausgabeschichtgrenze */
       Layer[s-1]+=(300*s-z1)/(pSL->fThickness)*pCL->fNO3N;
       s++; if (s>4) return /*1*/;/* wird anteilsmäßig aufgeteilt */                                  
       Layer[s-1]+=(z2-300*(s-1))/(pSL->fThickness)*pCL->fNO3N;
       }
     else if ((z2 > (s+1)*300) && (z2 <= (s+2)*300))
       {                      /* Simschicht überschreitet zwei Ausgabeschichtgrenzen */              
       Layer[s-1]+=(300*s-z1)/(pSL->fThickness)*pCL->fNO3N;
       s++; if (s>4) return /*1*/;     
       Layer[s-1]+=pCL->fNO3N;
       s++; if (s>4) return /*1*/;
       Layer[s-1]+=(z2-300*(s-1))/(pSL->fThickness)*pCL->fNO3N;
       }
     else if (z2 > (s+2)*300)
       {                      /* Simschicht überschreitet mehr als 2 Ausgabeschichtgrenzen */
       Layer[s-1]+=(300*s-z1)/(pSL->fThickness)*pCL->fNO3N;
       s++; if (s>4) return /*1*/;
       Layer[s-1]+=pCL->fNO3N;
       s++; if (s>4) return /*1*/;
       Layer[s-1]+=pCL->fNO3N;
       s++; if (s>4) return /*1*/;
       if (z2 <= s*300)  // da s jetzt um 3 erhöht, d.h. mind. 4 !!!
         Layer[s-1]+=(z2-300*(s-1))/(pSL->fThickness)*pCL->fNO3N;
       else       
         {
         Layer[s-1]+=pCL->fNO3N;
         return /*1*/;
         }
       }
       
   
     pCL = pCL->pNext;
     pSL = pSL->pNext;
  }                                                    
   
  return;
}

//-----------------------------------------------------------------------------
int WINAPI PrintGisData(EXP_POINTER, int iMessageStatus)
/* Gibt  die Werte aus, die für eine erneute Simulation als Startwerte benutzt
   werden koennen.
-----------------------------------------------------------------------------*/
{ 
  DECLARE_COMMON_VAR
  HANDLE      hFile;
  PWBALANCE   pWB = pWa->pWBalance; 
  PCBALANCE   pCB = pCh->pCBalance;
  PCPROFILE   pCF = pCh->pCProfile;
  PFARM       pPF = pLo->pFarm;
  PFIELDPLOT  pFP = pLo->pFieldplot;
  PSIMPROJECT pPrj;
  PCLAYER     pCL = pCh->pCLayer->pNext;
  PSLAYER     pSL = pSo->pSLayer->pNext;

  char  szFileName[18];
  char  szPath[29];
  char  teilname[51];
  char  vorfrucht[21];
  char  zwischenfrucht[21];
  char  hauptfrucht[21];
  char  szDate[11];
  char  szTime[11];
  
  float mgProm2;
  float z1 = (float)0;    /* Startwert Simschicht */
  float z2 = (float)0;      /* Endwert Simschicht */
 

  Layer = _MALLOC(sizeof(float)*4); 

  pPrj = getProjectRoot();             
  
  /* Umrechnen auf 30, 60 90 cm Schichten. */
  writeLayerSections(exp_p,Layer);

  /* Erstellen des Dateinamens durch Verwendung der ersten 7
     Zeichen des Teilschlagnamens, der ergaenzt wird durch das
     Jahr des Simulationsendes. Danach wird das Verzeichnis an-
     gegeben in welches die Datei geschrieben werden soll. */
  
  if (strlen(pPrj->szPrjName)<2)
  {
    lstrcpyn(szFileName,"expertn",7);
  }
  else
  {
    lstrcpyn(szFileName,pPrj->szPrjName,7);
  }
   
  itoa( (int)(pTi->pSimTime->lStopDate % 100),szDate,10);
  lstrcat(szFileName,szDate);
  strcpy(szPath,"result\\");
  strcat(szPath,szFileName);
  strcat(szPath,".rfg");

  /* Oeffnen der Datei nur dann wenn das erste Objekt eines
     Projektes bearbeitet wird. */

  if (pPrj->pSimObj->szTeilSName == pPrj->pCurrentSimObj->szTeilSName)
  {
    hFile = CreateFile(
      /* lpFileName             */  szPath
      /* dwDesiredAccess        */ ,GENERIC_WRITE
      /* dwShareMode            */ ,0
      /* lpSecurityAttributes   */ ,NULL
      /* dwCreationDistribution */ ,CREATE_ALWAYS
      /* dwFlagsAndAttributes   */ ,FILE_ATTRIBUTE_NORMAL
      /* hTemplateFile          */ ,NULL );
    if (hFile != INVALID_HANDLE_VALUE)
    {
      fout_string(hFile,"E X P E R T - N:  OUTPUTFILES"); fout_line(hFile, 1);                      
      fout_string(hFile,"---> Gis-Data <---"); fout_line(hFile, 1);
      _strdate(szDate);            
      fout_string(hFile,"Date: "); fout_string(hFile, szDate); fout_line(hFile, 1);
      _strtime(szTime);            
      fout_string(hFile,"Time: "); fout_string(hFile, szTime); 
      fout_line(hFile, 1); fout_line(hFile, 1);

      fout_string(hFile, TITLE_GISDATA_1);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_2);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_3);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_4);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_5);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_6);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_7);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_8);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_9);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_10);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_11);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_12);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_13);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_14);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_15);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_16);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_17);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_18);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_19);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_20);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_21);  fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_22);  fout_line(hFile, 1);
      fout_line(hFile, 1);

      fout_string(hFile, TITLE_GISDATA_LINE_1); fout_line(hFile, 1);
      fout_string(hFile, TITLE_GISDATA_LINE_2); fout_line(hFile, 1);
      fout_string(hFile, LONG_THIN_LINE);  
      fout_line(hFile, 1);

      if (strlen(pFP->acName)<2)
      {lstrcpyn(teilname,"unbekannt",9);}
      else
      {lstrcpyn(teilname,pFP->acName,50);}

      if (strlen(pFP->acPrevCrop)<2)
      {lstrcpyn(vorfrucht,"unbekannt",9);}
      else
      {lstrcpyn(vorfrucht,pFP->acPrevCrop,20);}

      if (strlen(pFP->acCatchCrop)<2)
      {lstrcpyn(zwischenfrucht,"unbekannt",9);}
      else
      {lstrcpyn(zwischenfrucht,pFP->acCatchCrop,20);}

      
      if (pPl != NULL)
      {
        if (strlen(pPl->pGenotype->acCropName)<2)
        {lstrcpyn(hauptfrucht,"unbekannt",9);}
        else
        {lstrcpyn(hauptfrucht,pPl->pGenotype->acCropName,20);}
      }
      else
      {
        lstrcpyn(hauptfrucht,"keine",9);
      }


      mgProm2 = ((float)pCF->dNO3LeachCum * (float)100) / pWa->fCumLeaching;

      //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);
      fout_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
      fout_int(hFile, &(pFP->iRegionNumber), 4); fout_string(hFile, "\t");
      fout_long(hFile, &(pPF->lFarmNumber)); fout_string(hFile, "\t");
      fout_string(hFile, teilname); fout_string(hFile, "\t");                 
      fout_string(hFile, vorfrucht); fout_string(hFile, "\t");
      fout_string(hFile, zwischenfrucht); fout_string(hFile, "\t");
      fout_string(hFile, hauptfrucht); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pCB->fNO3NProfile, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pCB->fNH4NProfile, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, (float)pCF->dNO3LeachCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, (float)pCF->dNMinerCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pCl->fCumRain, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fCumInfiltration, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fRunOffCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWB->fActCumEvap, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fActTranspCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fCumLeaching, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fPondWater, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, mgProm2, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, Layer[0], 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, Layer[1], 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, Layer[2], 2, 3);
      fout_line(hFile, 1);
    }
  }
  else
  {

    hFile = CreateFile(
      /* lpFileName             */  szPath
      /* dwDesiredAccess        */ ,GENERIC_WRITE
      /* dwShareMode            */ ,0
      /* lpSecurityAttributes   */ ,NULL
      /* dwCreationDistribution */ ,OPEN_ALWAYS
      /* dwFlagsAndAttributes   */ ,FILE_ATTRIBUTE_NORMAL
      /* hTemplateFile          */ ,0 );
    
    if (hFile != INVALID_HANDLE_VALUE)
    {
      SetFilePointer(hFile,0,NULL,FILE_END);
    
      if (strlen(pFP->acName)<2)
      {lstrcpyn(teilname,"unbekannt",9);}
      else
      {lstrcpyn(teilname,pFP->acName,50);}
      if (strlen(pFP->acPrevCrop)<2)
      {lstrcpyn(vorfrucht,"unbekannt",9);}
      else
      {lstrcpyn(vorfrucht,pFP->acPrevCrop,20);}
      
      if (strlen(pFP->acCatchCrop)<2)
      {lstrcpyn(zwischenfrucht,"unbekannt",9);}
      else
      {lstrcpyn(zwischenfrucht,pFP->acCatchCrop,20);}

      if (pPl != NULL)
      {
        if (strlen(pPl->pGenotype->acCropName)<2)
        {lstrcpyn(hauptfrucht,"unbekannt",9);}
        else
        {lstrcpyn(hauptfrucht,pPl->pGenotype->acCropName,20);}
      }
      else
      {
        lstrcpyn(hauptfrucht,"keine",9);
      }


      mgProm2 = ((float)pCF->dNO3LeachCum * (float)100) / pWa->fCumLeaching;

      //if (pTi->pSimTime->lTimeDate < 100000) fout_space(hFile, 1);
      fout_long(hFile, &(pTi->pSimTime->lTimeDate)); fout_string(hFile, "\t");
      fout_int(hFile, &(pFP->iRegionNumber), 4); fout_string(hFile, "\t");
      fout_long(hFile, &(pPF->lFarmNumber)); fout_string(hFile, "\t");
      fout_string(hFile, teilname); fout_string(hFile, "\t");                 
      fout_string(hFile, vorfrucht); fout_string(hFile, "\t");
      fout_string(hFile, zwischenfrucht); fout_string(hFile, "\t");
      fout_string(hFile, hauptfrucht); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pCB->fNO3NProfile, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pCB->fNH4NProfile, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, (float)pCF->dNO3LeachCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, (float)pCF->dNMinerCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pCl->fCumRain, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fCumInfiltration, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fRunOffCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWB->fActCumEvap, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fActTranspCum, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fCumLeaching, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, pWa->fPondWater, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, mgProm2, 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, Layer[0], 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, Layer[1], 2, 3); fout_string(hFile, "\t");
      fout_floatNorm(hFile, Layer[2], 2, 3);

      fout_line(hFile, 1);
    }
  }

  CloseHandle( hFile );
    
  free(Layer);

  return 1;

} // PrintGisData

/*******************************************************************************
** EOF */
