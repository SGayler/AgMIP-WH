/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author: Christian Haberbosch, Chr.Sperr    05.03.97
 *
 *------------------------------------------------------------------------------
 *
 * Description:  reads user defined mod-file
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 6 $
 *
 * $History: readmod.c $
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 24.01.02   Time: 11:37
 * Updated in $/Projekte/ExpertN/ModLib/control
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 25.12.01   Time: 10:00
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Komfortablere Fehlermeldungen.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:02
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
*******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include "trace.h"
#include "fileio.h"
#include "expbasic.h"
#include "xinclexp.h"
#include "xn_def.h"   
#include "xlanguag.h"     

#define  DATA_FOR_EACH_LAYER (1)
// #define  DATA_FOR_EACH_LAYER ((i1Wert == 0)||(i1Wert>8))
#define  DATA_FOR_EACH_HORIZON ((i1Wert > 1)&&(i1Wert<=8))

// #ifndef MESSAGE_LENGTH
#define MESSAGE_LENGTH  70
// #endif  
               
// **********************************************************************************
//    Einlesen
// **********************************************************************************

#define  GET_INT \
  lByteNo = read_ac_int(hpcReadData, &i1Wert); \
  if (lByteNo > 0) { \
    hpcReadData += lByteNo; \
  } 
#define  GET_FLOAT(x) \
  lByteNo = read_ac_float(hpcReadData, &f1Wert); \
  if (lByteNo > 0) { \
    hpcReadData += lByteNo; \
    x = f1Wert; \
  } 
#define  GET_STRING(x) \
  lByteNo = read_ac_string(hpcReadData, acDummy); \
  if (lByteNo > 0) { \
    hpcReadData += lByteNo; \
    strcpy(x,acDummy); \
  } 
#define  SKIP_LINE \
  lByteNo = skip_ac_line(hpcReadData,1); \
  if (lByteNo > 0) { \
    hpcReadData += lByteNo; \
    (iLines)++; \
  }

// **********************************************************************************
// Überprüfungen   
// **********************************************************************************


#define GET_TEST_FLOAT(value,limit0,limit1,name) \
  GET_FLOAT(value); \
  f1=(float)limit0;\
  f2=(float)limit1;\
  if ((value < limit0)||(value > limit1)) { \
    char szBuf[100]; \
	sprintf( szBuf, "Val = %e, Min = %e, Max = %e", value, f1, f2 ); \
	M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,name,szBuf,NULL);\
    value = (float)-99; \
  }


// **********************************************************************************
// Einlese-Utilities
// **********************************************************************************

#define SKIP_TO_MARKER(XXX) \
        lByteNo = search_marker(hpcReadData, XXX, &iLines); \
        if (lByteNo > 0) hpcReadData  += lByteNo; \
        if (lByteNo < 0){        \
        hpcReadData = hpcData; iLines = 0;}  \
        else                       




/* externe Funktionen */
/*
 * util_fct.c
 */          
extern int WINAPI Message(long,LPSTR);
extern int WINAPI Test_Range(float x, float x1, float x2, LPSTR c);
extern PPLANT  WINAPI  GetPlantPoi(void);        


/* Funktions-Deklarationen: */
int WINAPI ReadModFile(EXP_POINTER);

/***********************************************************************************************
 * Procedur    :   ReadModFile()
 * Op.-System  :   DOS
 * Beschreibung:   Liest Mod-file ein.
 *
 * Autor:          ch/gsf           23.7.97
 * nach:           READ_EX_DATA.c von C.Sperr
 ***********************************************************************************************
 */
extern int   iInterceptionModel;
extern float fIceptPar,fIceptStor;//,fRelIntercept;
extern float fIceptPar1,fIceptPar2,fIceptPar3,fIceptPar4,fIceptPar5;
extern RESPONSE fIceptPar0DC[21],fIceptPar1DC[21],fIceptPar2DC[21],
                fIceptPar3DC[21],fIceptPar4DC[21],fIceptPar5DC[21];

extern float afRSPar[8]; //StefanAchatz
extern float fh50Global;
extern float fp1Global;
extern int   iWaterUptakeModel;
extern int   iWaterMobImm;

extern float TiefenTab[];
extern float WGMPTiefenTab[];
extern int   iSickerWasserAusgabe;
extern int   iWassergehaltAusgabe;

extern float PrintDepthTab[];
extern float WCPrintDepthTab[];
extern float WPPrintDepthTab[];
extern float STPrintDepthTab[];
extern float NCPrintDepthTab[];
extern int   iSoilWaterFluxOutput;
extern int   iSoilWaterContentOutput;
extern int   iSoilWaterMatricPotOutput;
extern int   iSoilTemperatureOutput;
extern int   iSoilNitrogenContentOutput;

extern float afConstRootDens[MAXSCHICHT];
extern int   iIsConstRootDens;

extern int   iStandingResidues;
extern int   iSoilCover;
extern int   iCropCover;
extern int   iDualCropFactor;

extern int   iForest;

extern float fRedTAmpYear;
float fSCEff=(float)-99;
float fCanopyClosure=(float)-99;
float fCropCoverFrac=(float)-99; 

/////////////////////////////////////////////////////////////////////////////////////////////////////
//GECROS-Block auskommentiert, da GECROS-Parameter jetzt über gtp eingelesen werden (Siehe readgtp.c)
/////////////////////////////////////////////////////////////////////////////////////////////////////
//int   iGECROS = (int)-1;
//int   iGECROSDefault = (int)-1; //SG20140710: für GECROS im rfs-Modus
//int   jGECROS = (int)0;
//float fGECROS = (float)-99;
//float fparLEGUME,fparC3C4,fparDETER,fparSLP,fparLODGE,fparVERN;
//float fparEG,fparCFV,fparYGV,fparFFAT,fparFLIG,fparFOAC,fparFMIN;
//float fparTBD,fparTOD,fparTCD,fparTSEN,fparLWIDTH,fparRDMX,fparCDMHT;
//float fparLNCI,fparCCFIX,fparNUPTX,fparSLA0,fparSLNMIN,fparRNCMIN,fparSTEMNC;
//float fparINSP,fparSPSP,fparEPSP,fparEAJMAX,fparXVN,fparXJN,fparTHETA;
//float fparSEEDW,fparSEEDNC,fparBLD,fparHTMX,fparMTDV,fparMTDR,fparPSEN;
////SG 20180410: additional parameters for J. Rabe sugarbeet model
//float fparSINKBEET,fparEFF,fparCFS; //for sugar beet
//float fparPMEH,fparPMES,fparESDI,fparWRB;
//
//float fparCO2A,fparCOEFR,fparCOEFV,fparCOEFT;
//float fparFCRSH,fparFNRSH;
//float fparPNPRE,fparCB,fparCX,fparTM;
//
//float fparCLAY,fparWCMIN,fparWCPWC,fparPRFDPT,fparSD1,fparTCT,fparTCP;
//float fparWCMAX, fparRSS;
//
//float fparBIOR,fparHUMR;
//float fparPNLS;
//float fparDRPM,fparDPMR0,fparRPMR0,fparTOC,fparBHC,fparFBIOC;
//
//int WINAPI Init99InputGECROS(void);
///////////////////////////////////////////////////////////////////////////////////////////////////

int iInterceptionModel = (int) -99;

//SG 20110810: CO2 als Input (für AgMIP)
float fAtmCO2;
int   iIrrAm;

//SG 20130917: für AgMIP-Soil für schichtweise Ausgabe im restart-file
int iNumb_lch, iNumb_wct, iNumb_mpt, iNumb_hct, iNumb_nct;

//EP 20160427 Temperaturabhaengigkeit für DAISY Abbauraten (für Scott Demyan)
int iTempCorr=(int)0;
float fParCAOM1Q10, fParCAOM2Q10, fParCBOM1Q10, fParCBOM2Q10, fParCSOM1Q10, fParCSOM2Q10;
float fParDBOM1Q10, fParDBOM2Q10, fParMBOM1Q10, fParMBOM2Q10;

float fParBOM1, fParSOM1, fParSOM2, fParEff;
float fEff_AOM1, fEff_AOM2, fEff_BOM1, fEff_BOM2, fEff_SOM1, fEff_SOM2;   /* org.:  0.6 */

//-----------------------------------------------------------------------------
int WINAPI ReadModFile(
  PCHEMISTRY  pCh
 ,PCLIMATE    pCl
 ,PGRAPHIC    pGr
 ,PHEAT       pHe
 ,PLOCATION   pLo
 ,PMANAGEMENT pMa
 ,PPLANT      pPl
 ,PSPROFILE   pSo
 ,PTIME       pTi
 ,PWATER      pWa )
//-----------------------------------------------------------------------------
{
  // DECLARE_COMMON_VAR  ! information hiding ! (stoert bei Fehlersuche)
  // int           OK = 1;
  int           iLayer = 0;
  char          lpOut[80];

  // DECLARE_N_POINTER  ! information hiding ! (stoert bei Fehlersuche)
  PSLAYER       pSL = pSo->pSLayer;
  PCLAYER       pCL = pCh->pCLayer;
  PCPROFILE     pCP = pCh->pCProfile;
  PCPARAM       pPA = pCh->pCParam;
  PSWATER       pSW = pSo->pSWater;
  PWLAYER       pWL = pWa->pWLayer;
  PHLAYER       pHL = pHe->pHLayer;
  PNFERTILIZER  pNF = pMa->pNFertilizer;
  PPLANT        pPL;

  HANDLE        hFile;
  char          acDummy[81];
  char          acDummy2[MAX_FILE_STRINGLEN+10+2];
  long          ulNoOfBytes;
  long          ulNoOfBytesRead;
  HANDLE        hData;
  char          *hpcData;
  char          *hpcReadData;
  int           i,iNumb,i1Wert,iModel;
  float         f1Wert;
  long          lByteNo = 0;
  int           iLines = 0;
  float         f1,f2;


  // acDummy2 = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.
  // acFileName = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.
  // iLines = (int *) _MALLOC(sizeof(int)); 
  // ANMERKUNG CH. BAUER:
  // Ich konnte keinen Grund finden, warum die Variablen bei jedem neuen
  // Aufruf auf dem Heap allociert werden muessen. Ich konnte vielmehr nicht
  // die Stelle finden, bei der der allokierte Speicher wieder freigegeben wird.

  if (strlen(pLo->pFieldplot->acModFileName)<2)   
  {
     return 1;
  }
  else
  {
    strcpy(acDummy2,"param\\");
    lstrcat(acDummy2,pLo->pFieldplot->acModFileName);
  }
        
  hFile = CreateFile(
    /* lpFileName             */  acDummy2
    /* dwDesiredAccess        */ ,GENERIC_READ
    /* dwShareMode            */ ,0
    /* lpSecurityAttributes   */ ,NULL
    /* dwCreationDistribution */ ,OPEN_EXISTING
    /* dwFlagsAndAttributes   */ ,FILE_ATTRIBUTE_NORMAL
    /* hTemplateFile          */ ,NULL );
  
  if ( hFile == INVALID_HANDLE_VALUE )
  {                  
    if( GetLastError() == ERROR_FILE_NOT_FOUND )
    {
      SetLastError( NO_ERROR );
    }
    strcpy(lpOut,(LPSTR)"ReadModFile(): ");
    strcat(lpOut,(LPSTR)FILEOPEN_ERROR_TXT);
    Message(1, (LPSTR)lpOut);
    return  -1;    
  }
    
  /*  + GET size of file and  allocate adjusted memory */

  ulNoOfBytes=GetFileSize(hFile,NULL);//64000;
   /* ch !!! this function does not work: _filelength(hFile); */
 
  hData = GlobalAlloc(GMEM_MOVEABLE,(ulNoOfBytes + 1)*  sizeof(char));
  if(hData == NULL)
  {
    strcpy(lpOut,READ_MODFILE_ERROR_TXT);
    strcat(lpOut,(LPSTR)ALLOCATION_ERROR_TXT);
          
    Message(2,lpOut);
    return  -999;     /* allocation failed */
  }
  
  hpcData = (char *) GlobalLock(hData);

  // lRet = _hread(hFile,(void *) hpcData,ulNoOfBytes);
  ReadFile(
    /* hFile                */ hFile,
    /* lpBuffer             */ hpcData,
    /* nNumberOfBytesToRead */ ulNoOfBytes,
    /* lpNumberOfBytesRead  */ &ulNoOfBytesRead,
    /* lpOverlapped         */ NULL );
  CloseHandle( hFile );

  hpcReadData = hpcData + ulNoOfBytes;
  *hpcReadData = '\0';
  
  hpcReadData = hpcData;
   
/* ************************************* Reading section ********************************************* */
                                           
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  10008 Eckart Priesack, Modellwahl fuer Interzeption
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
  SKIP_TO_MARKER(10008)
  {
  GET_INT;
  GET_INT;
  iModel=i1Wert;
  GET_INT;
  iNumb=(int)min(i1Wert,(int)20);
  SKIP_LINE;           

  switch(iModel) 
  {
  case 0:
      iInterceptionModel = 0;// constant fraction of TRpot 

	  fIceptPar1DC[0].fInput=(float)iNumb;
	  fIceptPar1DC[0].fOutput=(float)iNumb;

	  fIceptPar1DC[1].fInput=(float)0;

	  for(i=1;i<=iNumb;i++)
      {
	   //if(i>1)
	   {
	   GET_INT;
	   fIceptPar1DC[i].fInput=(float)i1Wert;
	   }
	   GET_FLOAT(fIceptPar1DC[i].fOutput);
       SKIP_LINE;
	  }
    break;
  case 1: 
      iInterceptionModel = 1;//Rutter
	    
	  fIceptPar1DC[0].fInput=(float)iNumb;
	  fIceptPar1DC[0].fOutput=(float)iNumb;
	  fIceptPar2DC[0].fInput =(float)iNumb;
	  fIceptPar2DC[0].fOutput=(float)iNumb;
	  fIceptPar3DC[0].fInput =(float)iNumb;
	  fIceptPar3DC[0].fOutput=(float)iNumb;
	  fIceptPar4DC[0].fInput =(float)iNumb;
	  fIceptPar4DC[0].fOutput=(float)iNumb;
	  fIceptPar5DC[0].fInput =(float)iNumb;
	  fIceptPar5DC[0].fOutput=(float)iNumb;

	  //fIceptPar1DC[1].fInput=(float)0;
	  //fIceptPar2DC[1].fInput=(float)0;
	  //fIceptPar3DC[1].fInput=(float)0;
	  //fIceptPar4DC[1].fInput=(float)0;
	  //fIceptPar5DC[1].fInput=(float)0;

	  for(i=1;i<=iNumb;i++)
      {
	   //if(i>1)
	   {
	   GET_INT;
	   fIceptPar1DC[i].fInput=(float)i1Wert;
	   fIceptPar2DC[i].fInput=(float)i1Wert;
	   fIceptPar3DC[i].fInput=(float)i1Wert;
	   fIceptPar4DC[i].fInput=(float)i1Wert;
	   fIceptPar5DC[i].fInput=(float)i1Wert;
	   }
	   GET_FLOAT(fIceptPar1DC[i].fOutput);
       GET_FLOAT(fIceptPar2DC[i].fOutput);
       GET_FLOAT(fIceptPar3DC[i].fOutput);
       GET_FLOAT(fIceptPar4DC[i].fOutput);
       GET_FLOAT(fIceptPar5DC[i].fOutput);

       SKIP_LINE;
	  }
	  fIceptStor=fIceptPar5DC[1].fOutput;
    break;
  case 2: 
      iInterceptionModel = 2;//Gash 

	  fIceptPar1DC[0].fInput =(float)iNumb;
	  fIceptPar1DC[0].fOutput=(float)iNumb;
	  fIceptPar3DC[0].fInput =(float)iNumb;
	  fIceptPar3DC[0].fOutput=(float)iNumb;
	  fIceptPar5DC[0].fInput =(float)iNumb;
	  fIceptPar5DC[0].fOutput=(float)iNumb;

	  //fIceptPar1DC[1].fInput=(float)0;
	  //fIceptPar3DC[1].fInput=(float)0;
	  //fIceptPar5DC[1].fInput=(float)0;
	  
	  for(i=1;i<=iNumb;i++)
      {
	   //if(i>1)
	   {
	   GET_INT;
	   fIceptPar1DC[i].fInput=(float)i1Wert;
	   fIceptPar3DC[i].fInput=(float)i1Wert;
	   fIceptPar5DC[i].fInput=(float)i1Wert;
	   }
	   GET_FLOAT(fIceptPar1DC[i].fOutput);
       GET_FLOAT(fIceptPar3DC[i].fOutput);
       GET_FLOAT(fIceptPar5DC[i].fOutput);

       SKIP_LINE;
	  }
    break;
  case 3:
    iInterceptionModel = 3; //Hoyningen-Huene & Braden
	   GET_INT;
       GET_FLOAT(fIceptPar);
       SKIP_LINE;
    break;
  default: iInterceptionModel = 0; //Default = constant fraction of TRpot
  }//end Model Selection 
  
  }

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  10009 ChristophSambale/EckartPriesack, Irrigation Data
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
  SKIP_TO_MARKER(10009) //Irrigation
  {
  GET_INT;
  GET_INT;
    iNumb = i1Wert;
    SKIP_LINE;
    for(iLayer=1;iLayer<=iNumb;iLayer++)
    {
      GET_INT;
        pMa->pIrrigation->lDate = i1Wert;
        pMa->pIrrigation->iDay = (int)DateToDays(pTi->pSimTime->lStartDate, i1Wert);
      GET_FLOAT(f1Wert);
        pMa->pIrrigation->fAmount = f1Wert;
      GET_STRING(acDummy);
        strcpy(pMa->pIrrigation->acName,acDummy);
      GET_STRING(acDummy);
        strcpy(pMa->pIrrigation->acCode,acDummy);

      GET_FLOAT(f1Wert);
        pMa->pIrrigation->fRate = f1Wert;
      GET_FLOAT(f1Wert);
        pMa->pIrrigation->fNO3N = f1Wert*pMa->pIrrigation->fAmount/(float)100;
      GET_FLOAT(f1Wert);
        pMa->pIrrigation->fNH4N = f1Wert*pMa->pIrrigation->fAmount/(float)100;
      GET_FLOAT(f1Wert);
        pMa->pIrrigation->fNorgManure  = f1Wert*pMa->pIrrigation->fAmount/(float)100;
      GET_FLOAT(f1Wert);
        pMa->pIrrigation->fCorgManure  = f1Wert*pMa->pIrrigation->fAmount/(float)100;
	  GET_FLOAT(f1Wert);
        pMa->pIrrigation->fDON        = f1Wert*pMa->pIrrigation->fNorgManure;
        pMa->pIrrigation->fNorgManure = ((float)1-f1Wert)*pMa->pIrrigation->fNorgManure;
        pMa->pIrrigation->fDOC        = f1Wert*pMa->pIrrigation->fCorgManure;
        pMa->pIrrigation->fCorgManure = ((float)1-f1Wert)*pMa->pIrrigation->fCorgManure;


      if(pMa->pIrrigation->pNext!=NULL) pMa->pIrrigation =pMa->pIrrigation->pNext;      

      SKIP_LINE;
    }
    if(pMa->pIrrigation!=NULL) for(;pMa->pIrrigation->pBack!=NULL;pMa->pIrrigation=pMa->pIrrigation->pBack);
  }

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  10010
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(10010)     
    {
    SKIP_LINE;           

     for (SOIL_LAYERS1(pSL,pSo->pSLayer->pNext))
     {
       GET_INT;

   if (i1Wert > 0)
       {
    GET_FLOAT(f1Wert);
    GET_TEST_FLOAT(pSL->fClay, 0, 100,  "= fClay")       
    GET_TEST_FLOAT(pSL->fSilt, 0, 100,  "= fSilt")       
    GET_TEST_FLOAT(pSL->fSand, 0, 100,  "= fSand")       
    
    GET_TEST_FLOAT(f1, 0, 100,  "= ?")       // !!! ignore Gew %
    
    GET_TEST_FLOAT(pSL->fBulkDens, 1, 1e4,  "= fBulkDens")       
    GET_TEST_FLOAT(pSL->fRockFrac, 0, 100,  "= fRockFrac")       
    GET_TEST_FLOAT(pSL->fpH, 0, 10,  "= pH")       
    
    //pSL->fBulkDens/= (float)1000; // [kg/m3] => [kg/l]
    pSL->fRockFrac/= (float)100;  // [Vol %] => [1]

                                         
    SKIP_LINE;           
        }
       else
        if (iLayer == 1)
        {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
        }
        else
        {
           pSL->fClay      = pSL->pBack->fClay;
           pSL->fSilt      = pSL->pBack->fSilt;
           pSL->fSand      = pSL->pBack->fSand;
           pSL->fBulkDens  = pSL->pBack->fBulkDens;
           pSL->fRockFrac  = pSL->pBack->fRockFrac;
        pSL->fpH        = pSL->pBack->fpH;
           }
     }  
    }
                                           
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  10011
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(10011)
    {
   PLAYERROOT pLR;

   SKIP_LINE;           
  
   if (DATA_FOR_EACH_LAYER)

      for (N_SOIL_LAYERS)
    {
     GET_INT;
         
     if (i1Wert > 0)
    {
     GET_FLOAT(f1Wert);
     GET_TEST_FLOAT(pWL->fContInit, 0, 100,  "= Water Content")       
       // richtig: 
       pWL->fContInit /=(float)100;

         GET_FLOAT(f1); 
         
       GET_TEST_FLOAT(pHL->fSoilTemp, -40,40, "= SoilTemp")       
         GET_TEST_FLOAT(pCL->fNH4N , 0, 600, "= NH4")       
         GET_TEST_FLOAT(pCL->fNO3N , 0, 600, "= NO3")       

       if (pPl == NULL)
       { GET_FLOAT(f1);} 
           else
       { GET_TEST_FLOAT(afConstRootDens[iLayer], 0, 100, "");}

         SKIP_LINE;
         }
      else
        if (iLayer == 1)
        {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
        }
        else
        {
           pWL->fContInit        = pWL->pBack->fContInit;
           pHL->fSoilTemp        = pHL->pBack->fSoilTemp;
           pCL->fNH4N            = pCL->pBack->fNH4N;
           pCL->fNO3N            = pCL->pBack->fNO3N;
        }    
       }
  if (pPl != NULL)    
    for(SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
    {
     pLR->fLengthDens = afConstRootDens[iLayer];
    }

     Message(1,(LPSTR)COMMENT_STARTVALUES_READ);   

   }     


/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  10020
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(10020)     
    {
    SKIP_LINE;           

     for (N_SOIL_LAYERS)          
     {
    GET_INT;
      
     if (i1Wert > 0)
    {
    lByteNo = read_ac_string(hpcReadData,lpOut); 
    if (lByteNo > 0) {hpcReadData += lByteNo; strcpy(pSL->acSoilID,lpOut);}
    
    GET_TEST_FLOAT(pSW->fContPWP, 1, 100,  "= fContPWP")       
    GET_TEST_FLOAT(pSW->fContFK, 1, 100,  "= fContFK")       
	GET_TEST_FLOAT(pSL->fPorosity, 1, 100,  "= fPorosity")       
    //GET_TEST_FLOAT(pSW->fMinPot, -1e4, 0,  "= fMinPot")       
    GET_TEST_FLOAT(pSW->fCondSat, 1, 1e7,  "= fCondSat")       
    
    /* Dies passiert hier !!! */
    pSW->fContPWP /= (float)100;
    pSW->fContFK /= (float)100;
    pSL->fPorosity /= (float)100;
	//SG 20111017: pSW->fContSat = pSL->fPorosity, da sonst theta_sat nicht über *.xnm vorgegeben werden kann:
	//pSW->fContSat = pSL->fPorosity;  //SG20150929: theta_sat jetzt von Marker 10021 (Z. 684)

    /*    
    if (pSW->fMinPot == (float)-99) pSW->fMinPot = (float)-2;
    else
      pSW->fMinPot*=(float)102.2;
    */                                     
    SKIP_LINE;           
    }      
      else
        if (iLayer == 1)
          {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
          }
            else
           {
           pSL->fPorosity      = pSL->pBack->fPorosity;
           pSW->fContPWP       = pSW->pBack->fContPWP;
           pSW->fContFK        = pSW->pBack->fContFK;
           //pSW->fMinPot        = pSW->pBack->fMinPot;
           pSW->fCondSat       = pSW->pBack->fCondSat;
           }    
     }  
    }
                                           

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  10021
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(10021)     
    {
    SKIP_LINE;           

     for (SOIL_LAYERS1(pSW, pSo->pSWater->pNext))
     {
       GET_INT;

     if (i1Wert > 0)
    {
       //GET_TEST_FLOAT(pSW->fContInflec, 1, 100,  "= fContInflec")       
       //GET_TEST_FLOAT(pSW->fPotInflec, -1e5,0,   "= fPotInflec")       
       GET_TEST_FLOAT(pSW->fContRes, 0, 100,  "= fContRes")
       GET_TEST_FLOAT(pSW->fContSat, 0, 100,  "= fContSat") // SG20150929: ContSat jetzt von Marker 10021!!
       GET_TEST_FLOAT(pSW->fCampA,-1e5, 0,   "= fCampA")       
       GET_TEST_FLOAT(pSW->fCampB,   0, 100, "= fCampB")      
	   GET_TEST_FLOAT(pSW->fVanGenA, 0, 1,   "= fVanGenA")       
       GET_TEST_FLOAT(pSW->fVanGenN, 1, 10,  "= fVanGenN")       
       GET_TEST_FLOAT(pSW->fVanGenM, 0, 10,  "= fVanGenM")   // in VG_HCond, VG_DWCap, VG_WCont und VG_MPotl  = n-1/n gesetzt   
       GET_TEST_FLOAT(pSW->fMualemP,-10, 10, "= fMualemP")       
       GET_TEST_FLOAT(pSW->fMualemQ, 0, 10,  "= fMualemQ")   // wird in VG_HCond = 1 gesetzt    
       GET_TEST_FLOAT(pSW->fMualemR, 0, 10,  "= fMualemR")   // wird in VG_HCond = 2 gesetzt    

    /* Dies passiert hier !!! */                 
    //pSW->fContInflec/=(float)100;
    //pSW->fPotInflec*=(float)102.2;            
    pSW->fContRes/=(float)100;
	pSW->fContSat/=(float)100; 
    pSW->fCampA*=(float)102.2; // [kPa] to [mm]
	pSW->fVanGenA*=(float)0.1; // [1/cm] to [1/mm]

// SG20111025: "p" (="l") soll anstelle von VanGenM über xnm/rfs eingelesen werden können:	
	//SG 20150929: "p" (="l") wird auf "m" geschrieben, damit es in CONDUCTIVITY (=HCond) übergeben wird!
	pSW->fVanGenM = pSW->fMualemP; //PROVISORIUM!!! 
    //SG 20111024: auskommentiert für Pareto
	//pSW->fVanGenM = pSW->fMualemR; //PROVISORIUM!!! ep 160408
    
	if (iLayer == 1) // for the upper boundary condition
	{
     pSW->pBack->fContRes = pSW->fContRes;
     pSW->pBack->fContSat = pSW->fContSat;
     pSW->pBack->fCampA = pSW->fCampA;
     pSW->pBack->fCampB = pSW->fCampB;
     pSW->pBack->fVanGenA = pSW->fVanGenA;
     pSW->pBack->fVanGenN = pSW->fVanGenN;
     pSW->pBack->fVanGenM = pSW->fVanGenM;
     pSW->pBack->fMualemP = pSW->fMualemP;
     pSW->pBack->fMualemQ = pSW->fMualemQ;
     pSW->pBack->fMualemR = pSW->fMualemR;
	}
	
	if(iLayer==pSo->iLayers-2) // for the lower boundary condition 
	{
     pSW->pNext->fContRes = pSW->fContRes;
     pSW->pNext->fContSat = pSW->fContSat;
     pSW->pNext->fCampA = pSW->fCampA;
     pSW->pNext->fCampB = pSW->fCampB;
     pSW->pNext->fVanGenA = pSW->fVanGenA;
     pSW->pNext->fVanGenN = pSW->fVanGenN;
     pSW->pNext->fVanGenM = pSW->fVanGenM;
     pSW->pNext->fMualemP = pSW->fMualemP;
     pSW->pNext->fMualemQ = pSW->fMualemQ;
     pSW->pNext->fMualemR = pSW->fMualemR;
	}

	SKIP_LINE;           
    }
      else
        if (iLayer == 1)
          {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
          }
            else
           {
           //pSW->fContInflec    = pSW->pBack->fContInflec;
           //pSW->fPotInflec     = pSW->pBack->fPotInflec;
           pSW->fContRes       = pSW->pBack->fContRes;
           pSW->fContSat       = pSW->pBack->fContSat;
           pSW->fCampA         = pSW->pBack->fCampA;
           pSW->fCampB         = pSW->pBack->fCampB;
           pSW->fVanGenA       = pSW->pBack->fVanGenA;
           pSW->fVanGenN       = pSW->pBack->fVanGenN;
           pSW->fVanGenM       = pSW->pBack->fVanGenM;
           pSW->fMualemP       = pSW->pBack->fMualemP;
           pSW->fMualemQ       = pSW->pBack->fMualemQ;
           pSW->fMualemR       = pSW->pBack->fMualemR;
           }    
    
     }  
    }
                                           
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  10022
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(10022)     
    {
	GET_INT;  
	GET_INT;
     iWaterMobImm=i1Wert;
    SKIP_LINE;
     
	 for (SOIL_LAYERS1(pSW, pSo->pSWater->pNext))
     {
       GET_INT;

     if (i1Wert > 0)
    {
       GET_TEST_FLOAT(pSW->fContImmSat, 0, 1,  "= fContImmSat")
       GET_TEST_FLOAT(pSW->fMobImmEC, 0.04,365,  "= fMobImmEC")       
       GET_TEST_FLOAT(pSW->fBiModWeight2, 0, 1,  "= fBiModWeight2")       
       GET_TEST_FLOAT(pSW->fVanGenA2, 0, 1,  "= fVanGenA2")       
       GET_TEST_FLOAT(pSW->fVanGenN2, 0, 10,  "= fVanGenN2")       
       GET_TEST_FLOAT(pSW->fVanGenM2, 0, 10,  "= fVanGenM2")       

    /* Dies passiert hier !!! */                 
    if(pSW->fContSat!=(float)-99)
	{
	 pSW->fContImmSat*=pSW->fContSat;
	 pSW->fContMobSat=pSW->fContSat-pSW->fContImmSat;
	}
	if (pSW->fMobImmEC > (float)0) pSW->fMobImmEC=(float)1/pSW->fMobImmEC;
	pSW->fBiModWeight1 =(float)1 - pSW->fBiModWeight2;
	pSW->fVanGenA2*=(float)0.1; // [1/cm] to [1/mm]
    
	if (iLayer == 1) // for the upper boundary condition
	{
     pSW->pBack->fContMobSat = pSW->fContMobSat;
     pSW->pBack->fContImmSat = pSW->fContImmSat;
     pSW->pBack->fMobImmEC = pSW->fMobImmEC;
     pSW->pBack->fBiModWeight1 = pSW->fBiModWeight1;
     pSW->pBack->fBiModWeight2 = pSW->fBiModWeight2;
     pSW->pBack->fVanGenA2 = pSW->fVanGenA2;
     pSW->pBack->fVanGenN2 = pSW->fVanGenN2;
     pSW->pBack->fVanGenM2 = pSW->fVanGenM2;
	}

	if(iLayer==pSo->iLayers-2) // for the lower boundary condition 
	{
     pSW->pNext->fContMobSat = pSW->fContMobSat;
     pSW->pNext->fContImmSat = pSW->fContImmSat;
     pSW->pNext->fMobImmEC = pSW->fMobImmEC;
     pSW->pNext->fBiModWeight1 = pSW->fBiModWeight1;
     pSW->pNext->fBiModWeight2 = pSW->fBiModWeight2;
     pSW->pNext->fVanGenA2 = pSW->fVanGenA2;
     pSW->pNext->fVanGenN2 = pSW->fVanGenN2;
     pSW->pNext->fVanGenM2 = pSW->fVanGenM2;
	}
	
    SKIP_LINE;           
    }
    else
        if (iLayer == 1)
          {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
          }
        else
          {
           pSW->fContImmSat    = pSW->pBack->fContImmSat;
           pSW->fContMobSat    = pSW->pBack->fContMobSat;
           pSW->fMobImmEC      = pSW->pBack->fMobImmEC;
           pSW->fBiModWeight2  = pSW->pBack->fBiModWeight2;
           pSW->fVanGenA2      = pSW->pBack->fVanGenA2;
           pSW->fVanGenN2      = pSW->pBack->fVanGenN2;
           pSW->fVanGenM2      = pSW->pBack->fVanGenM2;
          }    
    
     }  
    }

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	10017 wurde verteilt auf 10010 (pH) und 20003 (Humus C und N)
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 

 
SKIP_TO_MARKER(10017)
    {
    SKIP_LINE;           

     for (SOIL_LAYERS1(pSL,pSo->pSLayer->pNext))
     {
       GET_INT;

     if (i1Wert > 0)
    {
    GET_FLOAT(f1); 
      
    GET_TEST_FLOAT(pSL->fpH, 0, 10,  "= pH")       
    GET_TEST_FLOAT(pSL->fCHumus, 1, 1e7,  "= Humus C")       
    GET_TEST_FLOAT(pSL->fNHumus, 1, 1e6,  "= Humus N")       
    
    f1 = pSL->fBulkDens * pSL->fThickness * 100 * ((float)1 -  pSL->fRockFrac); 
    
    if (f1 < EPSILON)
    {               
          char szBuf[100];
          sprintf( szBuf, "Val = %e, Max = %e", f1, EPSILON );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,szBuf,NULL,NULL);
    f1 = EPSILON;     
    }
        
    // Here Corg (weight%) should be in the Humus variable. 
    // This will be reconverted in the init-file by initHumus():
    pSL->fCHumus /= f1;
    pSL->fNHumus /= f1;

    SKIP_LINE;           
    }
      else
        if (iLayer == 1)
          {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
          }
            else
           {
           pSL->fpH        = pSL->pBack->fpH;
           pSL->fCHumus    = pSL->pBack->fCHumus;
           pSL->fNHumus    = pSL->pBack->fNHumus;
           }    
     }  
    }
*/                                           

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20000
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
iForest = (int)0;
SKIP_TO_MARKER(20000)
{
  SKIP_LINE;           
  GET_INT;        
       
  if (i1Wert > 0)
  {
     GET_TEST_FLOAT(pCh->pCProfile->fCBranchLitterSurf , 0,10000, "= Branch Surface Litter C")       
     GET_TEST_FLOAT(pCh->pCProfile->fNBranchLitterSurf , 0, 1000, "= Branch Surface Litter N")       
     GET_TEST_FLOAT(pCh->pCProfile->fCStemLitterSurf , 0,10000, "= Stem Surface Litter C")       
     GET_TEST_FLOAT(pCh->pCProfile->fNStemLitterSurf , 0, 1000, "= Stem Surface Litter N")       
     GET_TEST_FLOAT(pCh->pCProfile->fLigFracLeaf  , 0, 1e4, "= Lignin Leaf")       
     GET_TEST_FLOAT(pCh->pCProfile->fLigFracBranch  , 0, 1e3, "= Lignin Branch")       
     GET_TEST_FLOAT(pCh->pCProfile->fLigFracStem  , 0, 1e3, "= Lignin Stem")       
     GET_TEST_FLOAT(pCh->pCProfile->fLigFracFineRoot  , 0, 1e3, "= Lignin Fine Root")       
     GET_TEST_FLOAT(pCh->pCProfile->fLigFracCoarseRoot  , 0, 1e3, "= Lignin Coarse Root")       
     GET_TEST_FLOAT(pCh->pCProfile->fFineBranchDecR  , 0, 1e3, "= Fine Branch DecR")       
     GET_TEST_FLOAT(pCh->pCProfile->fLargeWoodDecR  , 0, 1e3, "= Large Wood DecR")       
     GET_TEST_FLOAT(pCh->pCProfile->fCoarseRootDecR  , 0, 1e3, "= Coarse Root DecR")       
   iForest = (int)1;
  }
  else
  {
     char szBuf[100];
     wsprintf( szBuf, "Val = %d, Min = %d", i1Wert, 0 );
     M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"Nr",szBuf,NULL);
  }
} /* 20000 */ 



/*
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20001
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20001)
   {
    SKIP_LINE;           

     for (N_SOIL_LAYERS)          
     {
    GET_INT;
      
     if (i1Wert > 0)
    {
      GET_TEST_FLOAT(pPA->fMinerEffFac, 0, 1, "= MinerEff")       
      GET_TEST_FLOAT(pPA->fMinerHumFac, 0, 1, "= MHumAnt")       
      GET_TEST_FLOAT(pCL->fMicBiomCN, 1, 50, "= MinerCN")       
      GET_TEST_FLOAT(pPA->fMinerTempB, 0, 40, "= Temp0")       
      GET_TEST_FLOAT(pPA->fMinerQ10, 0, 10, "= Q10")       
      GET_TEST_FLOAT(pPA->fMinerThetaMin, 0, 1, "= Theta0")       
      GET_FLOAT(f1)     /* Dummy */
      GET_TEST_FLOAT(pPA->fMinerSatActiv, 0, 1, "= SatAktiv")       
      GET_TEST_FLOAT(pPA->fNitrifNO3NH4Ratio, 1, 50, "= NO3:NH4 bei Nitrifikation")           
      }
        else
        if (iLayer == 1)
          {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
          }
            else
              {  
           pPA->fMinerEffFac    = pPA->pBack->fMinerEffFac;
           pPA->fMinerHumFac    = pPA->pBack->fMinerHumFac;
           pCL->fMicBiomCN    = pCL->pBack->fMicBiomCN;
           pPA->fMinerTempB    = pPA->pBack->fMinerTempB;
           pPA->fMinerQ10     = pPA->pBack->fMinerQ10;  
           pPA->fMinerThetaMin   = pPA->pBack->fMinerThetaMin;
          }
       }
   
   }
     
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20002
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20002)
    {
    GET_INT;      
    GET_INT;        
    SKIP_LINE;           

    for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
       {
        GET_INT;
   
     if (i1Wert > 0)
    {
        GET_TEST_FLOAT(pCL->fUreaHydroMaxR, 0, 10, "= UreaHy")       
        GET_TEST_FLOAT(pCL->fNH4NitrMaxR, 0, 10, "= Nitrif")       
        GET_TEST_FLOAT(pCL->fNO3DenitMaxR, 0, 50, "= DeNitrif")       
        GET_TEST_FLOAT(pCL->fLitterMinerMaxR, 0, 1, "= MinerLit")       
        GET_TEST_FLOAT(pCL->fManureMinerMaxR, 0, 1, "= MinerMan")       
        GET_TEST_FLOAT(pCL->fHumusMinerMaxR, 0, 1, "= MinerHum")       
        SKIP_LINE;
        }
        else
        if (iLayer == 1)
          {
          char szBuf[100];
          wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
          M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
          }
            else
          {
          pCL->fUreaHydroMaxR   = pCL->pBack->fUreaHydroMaxR;
          pCL->fNH4NitrMaxR     = pCL->pBack->fNH4NitrMaxR;
          pCL->fNO3DenitMaxR     = pCL->pBack->fNO3DenitMaxR;
          pCL->fLitterMinerMaxR   = pCL->pBack->fLitterMinerMaxR;
          pCL->fManureMinerMaxR   = pCL->pBack->fManureMinerMaxR;
          pCL->fHumusMinerMaxR   = pCL->pBack->fHumusMinerMaxR;
          }  /* for all layers */
        
        
        }  /* for all layers */
        
    } /* 20002 */

    
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20003
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20003)
    {
    GET_INT;      
    GET_INT;        
    SKIP_LINE;  

    if (i1Wert != 1) //if not forest
	{
    if (DATA_FOR_EACH_LAYER)
       for (N_SOIL_LAYERS)
       {
         GET_INT; 

       if (i1Wert > 0)
       {
           GET_TEST_FLOAT(pCL->fCLitter , 0,16000, "= Litter C")       
           GET_TEST_FLOAT(pCL->fNLitter , 0, 1600, "= Litter N")       
           GET_TEST_FLOAT(pCL->fCManure , 0,16000, "= Manure C")       
           GET_TEST_FLOAT(pCL->fNManure , 0, 1600, "= Manure N")       
           GET_TEST_FLOAT(pSL->fCHumus  , 0, 1e7,  "= Humus C")       
           GET_TEST_FLOAT(pSL->fNHumus  , 0, 1e6,  "= Humus N")   
    
           f1 = pSL->fBulkDens * pSL->fThickness * 100 * ((float)1 -  pSL->fRockFrac); 
    
           if (f1 < EPSILON)
           {               
            char szBuf[100];
            sprintf( szBuf, "Val = %e, Max = %e", f1, EPSILON );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,szBuf,NULL,NULL);
            f1 = EPSILON;     
           }
        
           // Here Corg (weight%) should be in the Humus variable. 
           // This will be reconverted in the init-file by initHumus():
           pSL->fCHumus /= f1;
           pSL->fNHumus /= f1;
               
         SKIP_LINE;
         }
         else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
              else
           {             
           pCL->fCLitter  = pCL->pBack->fCLitter;
           pCL->fNLitter  = pCL->pBack->fNLitter;
           pCL->fCManure  = pCL->pBack->fCManure;
           pCL->fNManure  = pCL->pBack->fNManure;
           pSL->fCHumus    = pSL->pBack->fCHumus;
               pSL->fNHumus    = pSL->pBack->fNHumus;
        }
       
       }  /* for  */

	}
	else //if forest
	{
    if (DATA_FOR_EACH_LAYER)
       for (N_SOIL_LAYERS)
       {
         GET_INT; 

       if (i1Wert > 0)
       {
           GET_TEST_FLOAT(pCL->fCFineRootLitter , 0,16000, "= Fine Root Litter C")       
           GET_TEST_FLOAT(pCL->fNFineRootLitter , 0, 1600, "= Fine Root Litter N")       
           GET_TEST_FLOAT(pCL->fCGrossRootLitter , 0,16000, "= Gross Root Litter C")       
           GET_TEST_FLOAT(pCL->fNGrossRootLitter , 0, 1600, "= Gross Root Litter N")       
           GET_TEST_FLOAT(pSL->fCHumus  , 0, 1e7,  "= Humus C")       
           GET_TEST_FLOAT(pSL->fNHumus  , 0, 1e6,  "= Humus N")       
    
           f1 = pSL->fBulkDens * pSL->fThickness * 100 * ((float)1 -  pSL->fRockFrac); 
    
           if (f1 < EPSILON)
           {               
            char szBuf[100];
            sprintf( szBuf, "Val = %e, Max = %e", f1, EPSILON );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,szBuf,NULL,NULL);
            f1 = EPSILON;     
           }
        
           // Here Corg (weight%) should be in the Humus variable. 
           // This will be reconverted in the init-file by initHumus():
           pSL->fCHumus /= f1;
           pSL->fNHumus /= f1;
               
         SKIP_LINE;
         }
         else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
              else
           {             
           pCL->fCFineRootLitter  = pCL->pBack->fCFineRootLitter;
           pCL->fNFineRootLitter  = pCL->pBack->fNFineRootLitter;
           pCL->fCGrossRootLitter = pCL->pBack->fCGrossRootLitter;
           pCL->fNGrossRootLitter = pCL->pBack->fNGrossRootLitter;

           pSL->fCHumus = pSL->pBack->fCHumus;
           pSL->fNHumus = pSL->pBack->fNHumus;
        }
       
       }  /* for  */
	}// else forest

   } /* 20003 */
 

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20004
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20004)
{
  SKIP_LINE;           
  GET_INT;        
       
  if (i1Wert > 0)
  {
     GET_TEST_FLOAT(pCh->pCProfile->fCLitterSurf , 0,12000, "= Surface Litter C")   //SG 20130917 6000 --> 12000 for AgMIP-Soil    
     GET_TEST_FLOAT(pCh->pCProfile->fNLitterSurf , 0, 1200, "= Surface Litter N")   //SG 20130917 600  --> 1200 for AgMIP-Soil        
     GET_TEST_FLOAT(pCh->pCProfile->fCManureSurf , 0,6000, "= Surface Manure C")       
     GET_TEST_FLOAT(pCh->pCProfile->fNManureSurf , 0, 600, "= Surface Manure N")       
     GET_TEST_FLOAT(pCh->pCProfile->fCHumusSurf  , 0, 1e4, "= Surface Humus C")       
     GET_TEST_FLOAT(pCh->pCProfile->fNHumusSurf  , 0, 1e3, "= Surface Humus N")       
  }
  else
  {
     char szBuf[100];
     wsprintf( szBuf, "Val = %d, Min = %d", i1Wert, 0 );
     M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"Nr",szBuf,NULL);
  }
} /* 20004 */ 


/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20005
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
iStandingResidues = 0;
SKIP_TO_MARKER(20005)
    {
    SKIP_LINE;           
    GET_INT;        
       
  if (i1Wert > 0)
       {
           GET_TEST_FLOAT(pCh->pCProfile->fCStandCropRes , 0,6000, "= Standing Crop Residues C")       
           GET_TEST_FLOAT(pCh->pCProfile->fNStandCropRes , 0, 600, "= Standing Crop Residues N")       
         iStandingResidues = 1;
         }
         else
            {
            Message(1,RANGE_ERROR_TXT);
            }
    } /* 20005 */ 

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	20006
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20006)
    {
    GET_INT;      
    GET_INT;        
    SKIP_LINE;           

    for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
       {
        GET_INT;
	 
        if (i1Wert > 0)
		{
        GET_TEST_FLOAT(pCL->fHumusToDONMaxR, 0, 10, "= HumusDON")       
        GET_TEST_FLOAT(pCL->fLitterToDONMaxR, 0, 10, "= LitterDON")       
        GET_TEST_FLOAT(pCL->fManureToDONMaxR, 0, 10, "= ManureDON")       
        GET_TEST_FLOAT(pCL->fDONToHumusMaxR, 0, 10, "= DONHumus")       
        GET_TEST_FLOAT(pCL->fDONToLitterMaxR, 0, 10, "= DONLitter")       
        GET_TEST_FLOAT(pCL->fDONMinerMaxR, 0, 10, "= MinerDON")       
        SKIP_LINE;
        }
        else
	      if (iLayer == 1)
	      	{
	      	Message(1,RANGE_ERROR_TXT);
	      	}
            else
	        {
	        pCL->fHumusToDONMaxR 	= pCL->pBack->fHumusToDONMaxR;
	        pCL->fLitterToDONMaxR 	= pCL->pBack->fLitterToDONMaxR;
	        pCL->fManureToDONMaxR 	= pCL->pBack->fManureToDONMaxR;
	        pCL->fDONToHumusMaxR 	= pCL->pBack->fDONToHumusMaxR;
	        pCL->fDONToLitterMaxR 	= pCL->pBack->fDONToLitterMaxR;
	        pCL->fDONMinerMaxR 	    = pCL->pBack->fDONMinerMaxR;
	        }  /* for all layers */
        
        
        }  /* for all layers */
        
    } /* 20006 */

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20007
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20007)
    {
  SKIP_LINE;  

    for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
      {
       GET_INT; 

       if (i1Wert > 0)
         {
    
         GET_TEST_FLOAT(pCL->fFOMSlowDecMaxR,  0, 1, "= SDR_AOM1")       
         GET_TEST_FLOAT(pCL->fFOMFastDecMaxR,  0, 1, "= SDR_AOM2")
         GET_TEST_FLOAT(pCL->fFOMVeryFastDecMaxR,  0, 1, "= SDR_AOM3")
         GET_TEST_FLOAT(pCL->fMicBiomSlowDecMaxR,  0, 1, "= SDR_BOM1")       
         GET_TEST_FLOAT(pCL->fMicBiomFastDecMaxR,  0, 1, "= SDR_BOM2")
         GET_TEST_FLOAT(pCL->fHumusSlowMaxDecMaxR,  0, 1, "= SDR_SOM1")       
         GET_TEST_FLOAT(pCL->fHumusFastMaxDecMaxR,  0, 1, "= SDR_SOM2")
         SKIP_LINE;
         }
            else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
                else
              {  
           pCL->fFOMSlowDecMaxR      = pCL->pBack->fFOMSlowDecMaxR;
           pCL->fFOMFastDecMaxR      = pCL->pBack->fFOMFastDecMaxR;
		   pCL->fFOMVeryFastDecMaxR  = pCL->pBack->fFOMVeryFastDecMaxR;
           pCL->fMicBiomSlowDecMaxR  = pCL->pBack->fMicBiomSlowDecMaxR;
           pCL->fMicBiomFastDecMaxR  = pCL->pBack->fMicBiomFastDecMaxR;
           pCL->fHumusSlowMaxDecMaxR = pCL->pBack->fHumusSlowMaxDecMaxR;  
           pCL->fHumusFastMaxDecMaxR = pCL->pBack->fHumusFastMaxDecMaxR;
          }
       }
    }  

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20008
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20008)
    {
    SKIP_LINE;           

       for (N_SOIL_LAYERS)
       {
         GET_INT; 

         if (i1Wert > 0)
           {
           GET_TEST_FLOAT(pCL->fCFOMSlow , 0, 1e6, "= fCFOMSlow")       
           GET_TEST_FLOAT(pCL->fCFOMFast , 0, 1e6, "= fCFOMFast")
           GET_TEST_FLOAT(pCL->fCFOMVeryFast , 0, 1e6, "= fCFOMVeryFast")       
           GET_TEST_FLOAT(pCL->fCMicBiomSlow , 0, 1e6, "= fCMicBiomSlow")       
           GET_TEST_FLOAT(pCL->fCMicBiomFast , 0, 1e6, "= fCMicBiomFast")       
           GET_TEST_FLOAT(pCL->fCMicBiomDenit , 0, 1e6, "= fCMicBiomDenit")       
           GET_TEST_FLOAT(pCL->fCHumusStable , 0, 1e7, "= fCHumusStable")       
           GET_TEST_FLOAT(pCL->fCHumusSlow , 0, 1e6, "= fCHumusSlow")       
           GET_TEST_FLOAT(pCL->fCHumusFast , 0, 1e6, "= fCHumusFast")       
           GET_TEST_FLOAT(pCL->fCsolC , 0, 1e6, "= fCsolC")       
           SKIP_LINE;
           }  
           else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
            else
             {
           pCL->fCFOMSlow    = pCL->pBack->fCFOMSlow;
           pCL->fCFOMFast    = pCL->pBack->fCFOMFast;
           pCL->fCFOMVeryFast= pCL->pBack->fCFOMVeryFast;
           pCL->fCMicBiomSlow  = pCL->pBack->fCMicBiomSlow;
           pCL->fCMicBiomFast  = pCL->pBack->fCMicBiomFast;
           pCL->fCMicBiomDenit  = pCL->pBack->fCMicBiomDenit;
           pCL->fCHumusStable  = pCL->pBack->fCHumusStable;
           pCL->fCHumusSlow  = pCL->pBack->fCHumusSlow;
           pCL->fCHumusFast  = pCL->pBack->fCHumusFast;
           pCL->fCsolC      = pCL->pBack->fCsolC;
             }
       }  /* for  */ 

   }    


     
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20009
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20009)
   {
    GET_INT;      
    GET_INT;        
    SKIP_LINE;           

    if (DATA_FOR_EACH_LAYER)
    for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
      {
       GET_INT; 
       
       if (i1Wert > 0)
         {
         GET_TEST_FLOAT(pCL->fNFOMSlow, 0, 4000, "= fNFOMSlow")       
         GET_TEST_FLOAT(pCL->fNFOMFast, 0, 4000, "= fNFOMFast")
         GET_TEST_FLOAT(pCL->fNFOMVeryFast, 0, 4000, "= fNFOMVeryFast")       
         GET_TEST_FLOAT(pCL->fNMicBiomSlow, 0, 400, "= fNMicBiomSlow")       
         GET_TEST_FLOAT(pCL->fNMicBiomFast, 0, 400, "= fNMicBiomFast")       
         GET_TEST_FLOAT(pCL->fNMicBiomDenit, 0, 400, "= fNMicBiomDenit")
         GET_TEST_FLOAT(pCL->fNHumusStable, 0, 40000, "= fNHumusStable")       
         GET_TEST_FLOAT(pCL->fNHumusSlow, 0, 40000, "= fNHumusSlow")       
         GET_TEST_FLOAT(pCL->fNHumusFast, 0, 40000, "= fNHumusFast")       
         SKIP_LINE;
         }
         else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
              else
             {
           pCL->fNFOMSlow      = pCL->pBack->fNFOMSlow;
           pCL->fNFOMFast      = pCL->pBack->fNFOMFast;
           pCL->fNFOMVeryFast  = pCL->pBack->fNFOMVeryFast;
           pCL->fNMicBiomSlow  = pCL->pBack->fNMicBiomSlow;
           pCL->fNMicBiomFast  = pCL->pBack->fNMicBiomFast;
           pCL->fNMicBiomDenit = pCL->pBack->fNMicBiomDenit;
           pCL->fNHumusStable  = pCL->pBack->fNHumusStable;
           pCL->fNHumusSlow    = pCL->pBack->fNHumusSlow;
           pCL->fNHumusFast    = pCL->pBack->fNHumusFast;
             }
      } /* for */
   }  


/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20010
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20010)
   {
    GET_INT;      
    SKIP_LINE;           

    for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
      {
       GET_INT; 

       if (i1Wert > 0)
         {
         GET_TEST_FLOAT(pCL->fN2ON , 0, 1e6, "= N2ON ")       
         GET_TEST_FLOAT(pCL->fN2N , 0, 1e6, "= N2N ")       
         SKIP_LINE;
         }
         else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
              else
             {             
             pCL->fN2ON   = pCL->pBack->fN2ON;
             pCL->fN2N   = pCL->pBack->fN2N;
             }
      } /* for */
   }    



/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20011
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20011)
    {
  SKIP_LINE;  

    for (SOIL_LAYERS1(pPA,pCh->pCParam->pNext))
      {
       GET_INT; 

       if (i1Wert > 0)
         {
    
        GET_TEST_FLOAT(pPA->fDenitKsNO3,  0.1, 500, "= KsDenNO3")
        GET_TEST_FLOAT(pPA->fDenitKsCsol,  0.1, 100, "= KsDenC")
        GET_TEST_FLOAT(pPA->fDenitThetaMin,  0, 1, "= Theta0Den")
        GET_TEST_FLOAT(pPA->fDenitMaxGrowR,  0, 100, "= BioNO3max")
        GET_TEST_FLOAT(pPA->fMaintCoeff,  0, 1, "= MC")
        SKIP_LINE;
         }
            else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
                else
              {          
              pPA->fDenitKsNO3     = pPA->pBack->fDenitKsNO3;
              pPA->fDenitKsCsol    = pPA->pBack->fDenitKsCsol;
              pPA->fDenitThetaMin   = pPA->pBack->fDenitThetaMin;   
              pPA->fDenitMaxGrowR   = pPA->pBack->fDenitMaxGrowR;
              pPA->fMaintCoeff     = pPA->pBack->fMaintCoeff;
              }  
      }
    }  
           
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20012
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20012)
    {
  SKIP_LINE;  

    for (SOIL_LAYERS1(pPA,pCh->pCParam->pNext))
      {
       GET_INT; 

       if (i1Wert > 0)
         {
    
      GET_TEST_FLOAT(pPA->fN2ORedMaxR,   0, 10000, "= N2OReductionMaxR")
      GET_TEST_FLOAT(pPA->fDenitFreeze,  0, 10000, "= FreezeFactorDen")
      GET_TEST_FLOAT(pPA->fDenitThaw,  0, 10000, "= ThawFactorDen")
      GET_TEST_FLOAT(pPA->fDenitReWet,  -1000, 1000, "= ReWetFactorDen")
          }
            else
          if (iLayer == 1)
            {
            char szBuf[100];
            wsprintf( szBuf, "Val = %d, Min = %d, Max = %d", iLayer, 1, 1 );
            M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"iLayer",szBuf,NULL);
            }
                else
              {          
              pPA->fN2ORedMaxR     = pPA->pBack->fN2ORedMaxR;
              pPA->fDenitFreeze    = pPA->pBack->fDenitFreeze;
              pPA->fDenitThaw     = pPA->pBack->fDenitThaw;   
              pPA->fDenitReWet     = pPA->pBack->fDenitReWet;
              }  
      }
    } 

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	20013  NH4-Volatilitaetsrate
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20013)
{
	pCh->pCProfile->fNH3VolatMaxR = (float)0;
    pCh->pCProfile->fCH4ImisR = (float)0;
    pCh->pCProfile->fCH4ImisDay = (float)0;

	GET_INT;      
    GET_INT;        
    SKIP_LINE;           
	GET_TEST_FLOAT(pCh->pCProfile->fNH3VolatMaxR, 0, 10, "");
	GET_TEST_FLOAT(pCh->pCProfile->fCH4ImisR, 0, 100, "");
	GET_TEST_FLOAT(pCh->pCProfile->fCH4ImisDay, 0, 100, "");
	SKIP_LINE;

    pCh->pCProfile->fCH4ImisR /= (float)365;
    pCh->pCProfile->fCH4ImisDay /= (float)365;

}     
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	20014
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20014)
    {
     GET_INT;      
	 GET_INT;
     pGr->iDllGraphicNum  = i1Wert;
    } 

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	20015
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20015)
    {
     GET_INT;      
	 GET_INT;
	 SKIP_LINE;
	 GET_INT;
     iIrrAm  = i1Wert;
    } 

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20020 StefanAchatz, Modellwahl fuer WaterUptake
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20020)
{
  int iloc;
  float xx;
    SKIP_LINE;           
    GET_INT;
  switch(i1Wert) 
  {
  case 1:
    iWaterUptakeModel = 1; //Nimah-Hanks
    break;
  case 2: 
    iWaterUptakeModel = 2; //Feddes
    for(iloc=1; iloc<8; iloc++) 
    {
      GET_FLOAT(xx);
      afRSPar[iloc] = xx;
    }
	//SG20150930 bzw. Irene Witte - Feddes kann optional mit Kompensation gerechnet werden
	// xx < 0 (oder weglassen): CSI = 1 --> keine Kompensation
	// 0 < xx < 1: CSI = xx --> Kompensation
	// (CSI = compensation index)
      GET_FLOAT(xx);
      afRSPar[0] = xx;

    break;
  case 3:
    iWaterUptakeModel = 3; //Van Genuchten
    GET_FLOAT(fh50Global);
    GET_FLOAT(fp1Global);
    break;
  default: iWaterUptakeModel = 1; //Default = Nimah-Hanks
  }
  
}

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  20021 for Scott Demyan, Modellparameter fuer Temperaturabhaengigkeit der Raten
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(20021)
{
 GET_INT;
 GET_INT;
 SKIP_LINE;           
      
  if (i1Wert > 0)
  {
     GET_TEST_FLOAT(fParCAOM1Q10 , 0,10, "= Q10 Value AOM1 decay rate")       
     GET_TEST_FLOAT(fParCAOM2Q10 , 0,10, "= Q10 Value AOM2 decay rate")       
     GET_TEST_FLOAT(fParCBOM1Q10 , 0,10, "= Q10 Value BOM1 decay rate")       
     GET_TEST_FLOAT(fParCBOM2Q10 , 0,10, "= Q10 Value BOM2 decay rate")       
     GET_TEST_FLOAT(fParCSOM1Q10 , 0,10, "= Q10 Value SOM1 decay rate")       
     GET_TEST_FLOAT(fParCSOM2Q10 , 0,10, "= Q10 Value SOM2 decay rate")       
	 SKIP_LINE;
     GET_TEST_FLOAT(fParDBOM1Q10 , 0,10, "= Q10 Value BOM1 death rate")       
     GET_TEST_FLOAT(fParDBOM2Q10 , 0,10, "= Q10 Value BOM2 death rate")       
     GET_TEST_FLOAT(fParMBOM1Q10 , 0,10, "= Q10 Value AOM2 maintenance rate")       
     GET_TEST_FLOAT(fParMBOM2Q10 , 0,10, "= Q10 Value AOM2 maintenance rate")       
  }
  //else
  //{
  //   char szBuf[100];
  //   wsprintf( szBuf, "Val = %d, Min = %d", i1Wert, 0 );
  //   M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,"Nr",szBuf,NULL);
  //}

  iTempCorr=i1Wert; 

}


SKIP_TO_MARKER(20022)
{
 GET_INT;
 SKIP_LINE;           
      
	GET_TEST_FLOAT(fParBOM1 , 0,1, "= partitioning coefficient fBOM1")       
	GET_TEST_FLOAT(fParSOM1 , 0,1, "= partitioning coefficient fSOM1")       
	GET_TEST_FLOAT(fParSOM2 , 0,1, "= partitioning coefficient fSOM2")       
	GET_TEST_FLOAT(fParEff  , 0,1, "= fEff")       

}

SKIP_TO_MARKER(20023)
{
 GET_INT;
 SKIP_LINE;           
      
	GET_TEST_FLOAT(fEff_AOM1 , 0,1, "= fEff_AOM1")       
	GET_TEST_FLOAT(fEff_AOM2 , 0,1, "= fEff_AOM2")       
	GET_TEST_FLOAT(fEff_BOM1 , 0,1, "= fEff_BOM1")       
	GET_TEST_FLOAT(fEff_BOM2 , 0,1, "= fEff_BOM2")       
	GET_TEST_FLOAT(fEff_SOM1 , 0,1, "= fEff_SOM1")       
	GET_TEST_FLOAT(fEff_SOM2 , 0,1, "= fEff_SOM2")       

}

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  30010
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
   // Schichttiefen für Sickerwasserausgabe in ~.lch - Datei
   iSoilWaterFluxOutput = 0;
   SKIP_TO_MARKER(30010)     
   {
     GET_INT;
     GET_INT;
     iNumb_lch=(int)min((double)i1Wert,(double)10);

     SKIP_LINE;           
     
     PrintDepthTab[0] = (float)0;
     for(i=1;i<iNumb_lch+1;i++)
     {
       GET_FLOAT(PrintDepthTab[i]);
       SKIP_LINE;           
     }
     PrintDepthTab[0] = (float)-99;
     PrintDepthTab[iNumb_lch+1] = (float)-99;
     iSoilWaterFluxOutput = 1;
     
   } /* marker */

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  30020
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
   // Schichttiefen für Wassergehaltausgabe in ~.wct - Datei
   iSoilWaterContentOutput = 0;
   SKIP_TO_MARKER(30020)     
   {
     GET_INT;
     GET_INT;
               iNumb_wct=(int)min((double)i1Wert,(double)10);

     SKIP_LINE;           
     WCPrintDepthTab[0] = (float)0;
     for(i=1;i<iNumb_wct+1;i++)
     {
       GET_FLOAT(WCPrintDepthTab[i]);
       SKIP_LINE;           
     }
     WCPrintDepthTab[0] = (float)-99;
     WCPrintDepthTab[iNumb_wct+1] = (float)-99;
     iSoilWaterContentOutput = 1;
     
   } /* marker */

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  30030
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
   // Schichttiefen für Matrixpotentialausgabe in ~.mpt - Datei
   iSoilWaterMatricPotOutput = 0;
   SKIP_TO_MARKER(30030)     
   {
     GET_INT;
     GET_INT;
     iNumb_mpt=(int)min((double)i1Wert,(double)10);

     SKIP_LINE;           
     WPPrintDepthTab[0] = (float)0;
     for(i=1;i<iNumb_mpt+1;i++)
     {
       GET_FLOAT(WPPrintDepthTab[i]);
       SKIP_LINE;           
     }
     WPPrintDepthTab[0] = (float)-99;
     WPPrintDepthTab[iNumb_mpt+1] = (float)-99;
     iSoilWaterMatricPotOutput = 1;
     
   } /* marker */

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  30040
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
   // Schichttiefen für Bodentemperaturausgabe in ~.hct - Datei
   iSoilTemperatureOutput = 0;
   SKIP_TO_MARKER(30040)     
   {
     GET_INT;
     GET_INT;
               iNumb_hct=(int)min((double)i1Wert,(double)10);

     SKIP_LINE;           
     STPrintDepthTab[0] = (float)0;
     for(i=1;i<iNumb_hct+1;i++)
     {
       GET_FLOAT(STPrintDepthTab[i]);
       SKIP_LINE;           
     }
     STPrintDepthTab[0] = (float)-99;
     STPrintDepthTab[iNumb_hct+1] = (float)-99;
     iSoilTemperatureOutput = 1;
     
   } /* marker */

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  30050
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
   // Schichttiefen für Bodenstickstoffausgabe in ~.nct - Datei
   iSoilNitrogenContentOutput = 0;
   SKIP_TO_MARKER(30050)     
   {
     GET_INT;
     GET_INT;
               iNumb_nct=(int)min((double)i1Wert,(double)10);

     SKIP_LINE;           
     NCPrintDepthTab[0] = (float)0;
     for(i=1;i<iNumb_nct+1;i++)
     {
       GET_FLOAT(NCPrintDepthTab[i]);
       SKIP_LINE;           
     }
     NCPrintDepthTab[0] = (float)-99;
     NCPrintDepthTab[iNumb_nct+1] = (float)-99;
     iSoilNitrogenContentOutput = 1;
     
   } /* marker */

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  40001  konstante Wurzelverteilung
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
iIsConstRootDens = 0;
SKIP_TO_MARKER(40001)
{
  PLAYERROOT pLR;
  pPl = GetPlantPoi();

    GET_INT;      
    GET_INT;        
    SKIP_LINE;           
  
    if (DATA_FOR_EACH_LAYER)
  {
    //for (N_SOIL_LAYERS)
    for(SOIL_LAYERS1(pLR,pPl->pRoot->pLayerRoot))
    {
      GET_INT; 
      
      if (i1Wert > 0)
      {
        GET_TEST_FLOAT(afConstRootDens[iLayer] , 0, 100, "");
        //GET_TEST_FLOAT(pLR->fLengthDens, 0, 100, "");
        pLR->fLengthDens = afConstRootDens[iLayer];

        SKIP_LINE;
      }
    }  /* for  */ 
    iIsConstRootDens = 1;
//    pPl->pDevelop->fDevStage = 3.9;
  }

}
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  40002  konstantes fCropCoverFrac
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(40002)
{
  iCropCover=(int)1;
  iSoilCover=(int)0;
  fSCEff = (float)-99;
  fCanopyClosure = (float) -99;
  fCropCoverFrac = (float) -99;
  
  pPL = GetPlantPoi();
  
  if((pPL)&&(pPL->pCanopy->fCropCoverFrac > (float)0)) iCropCover=(int)0;

  GET_INT;      
  GET_INT;        
  SKIP_LINE;           
  
  GET_TEST_FLOAT(fCropCoverFrac , -100, 1, "");
  GET_TEST_FLOAT(pSo->fSoilCover , -100, 1, "");
  GET_TEST_FLOAT(fSCEff, -100, 1, "");
  GET_TEST_FLOAT(fCanopyClosure, -100, 1, "");
  SKIP_LINE;

  if(fCropCoverFrac<(float)0) fCropCoverFrac=(float)-99;;
  if(pPL->pCanopy->fCropCoverFrac<(float)0) pPL->pCanopy->fCropCoverFrac=fCropCoverFrac;

  if(pSo->fSoilCover < (float)0) pSo->fSoilCover=(float)-99;

  if(fSCEff < (float)0) fSCEff=(float)-99;

  if(fCanopyClosure < (float)0) fCanopyClosure=(float)-99;

  if(fCropCoverFrac >= (float)0) iCropCover=(int)1;
  if(pSo->fSoilCover > (float)0) iSoilCover=(int)1;
}     

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	40003  dual crop factor
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(40003)
{
  iDualCropFactor=(int)0;
  pPL = GetPlantPoi();

  GET_INT;
  GET_INT;
  //if ((i1Wert==(int)1)||(i1Wert==(int)2)) iDualCropFactor=i1Wert;
  if (i1Wert>=(int)1) iDualCropFactor=i1Wert; //SG20160805
  SKIP_LINE;           

  if(pPL)
  {
  switch(iDualCropFactor)
  { 
  case 1:
      GET_TEST_FLOAT(pWa->pEvap->fCropBasalIni , 0, 10, "");
	  GET_TEST_FLOAT(pWa->pEvap->fCropBasalMid , 0, 10, "");
	  GET_TEST_FLOAT(pWa->pEvap->fCropBasalEnd , 0, 10, "");
    break;

  case 2:
  case 3:
      GET_TEST_FLOAT(pWa->pEvap->fCropBasalIni , 0, 10, "");
	  GET_TEST_FLOAT(pWa->pEvap->fCropBasalMid , 0, 10, "");
	  GET_TEST_FLOAT(pWa->pEvap->fCropBasalEnd , 0, 10, "");
      GET_TEST_FLOAT(pWa->pEvap->fCropTimeIni ,  0, 365,"");
	  GET_TEST_FLOAT(pWa->pEvap->fCropTimeDev ,  0, 365,"");
	  GET_TEST_FLOAT(pWa->pEvap->fCropTimeMid ,  0, 365,"");
	  GET_TEST_FLOAT(pWa->pEvap->fCropTimeEnd ,  0, 365,"");
  break;
  }
  }//if pPl     

}

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	40004  Reduktion der Jahrestemperaturamplitude
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(40004)
{
	fRedTAmpYear=(float)0;

	GET_INT;      
    GET_INT;        
    SKIP_LINE;           
	GET_TEST_FLOAT(fRedTAmpYear, 0, 1, "");
	SKIP_LINE;
}     


/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	50001  Atmosphärischer CO2-Gehalt (ppm)
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
SKIP_TO_MARKER(50001)
{
	GET_INT;      
    SKIP_LINE;           
	GET_TEST_FLOAT(fAtmCO2, 0, 1000, "");
}     


//////////////////////////////////////////////////////////////////////////////////////
////GECROS Input Parameter 80000 - 80010
//////////////////////////////////////////////////////////////////////////////////////
//Init99InputGECROS();
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80000
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80000)     
//    {    
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//
//    GET_INT;
//    GET_INT;
//    SKIP_LINE;           
//
//	//SG20140715: for running GECROS in rfs-mode
//	iGECROSDefault = i1Wert;
//      
//    if (i1Wert == 0)
//    {
//    lByteNo = read_ac_string(hpcReadData,lpOut); 
//    if (lByteNo > 0) {hpcReadData += lByteNo; 
//	if(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,lpOut)) Message(1,"Plant error: genotype : in *.xnm file, marker 80000");
//	lstrcpy((LPSTR)pPl->pGenotype->acCropCode,lpOut);}
//
//    lByteNo = read_ac_string(hpcReadData,lpOut); 
//    if (lByteNo > 0) {hpcReadData += lByteNo; 
//	lstrcpy((LPSTR)pPl->pGenotype->acCropName,lpOut);}
//
//	lByteNo = read_ac_string(hpcReadData,lpOut); 
//    if (lByteNo > 0) {hpcReadData += lByteNo; 
//	if(lstrcmp((LPSTR)pPl->pGenotype->acVarietyName,lpOut)) Message(1,"Plant error: variety : in *.xnm file, marker 80000");
//	lstrcpy((LPSTR)pPl->pGenotype->acVarietyName,lpOut);}
//
//	lByteNo = read_ac_string(hpcReadData,lpOut); 
//    if (lByteNo > 0) {hpcReadData += lByteNo; 
//	lstrcpy((LPSTR)pPl->pGenotype->acEcoType,lpOut);}
//
//	lByteNo = read_ac_string(hpcReadData,lpOut); 
//    if (lByteNo > 0) {hpcReadData += lByteNo; 
//	lstrcpy((LPSTR)pPl->pGenotype->acDaylenType,lpOut);}
//
//	lByteNo = read_ac_string(hpcReadData,lpOut); 
//    if (lByteNo > 0) {hpcReadData += lByteNo; 
//	lstrcpy((LPSTR)pPl->pGenotype->acLvAngleType,lpOut);}
//
//	iGECROS = (int)0;
//    fGECROS = (float)0;
//	}
//	else
//	{
//    iGECROS = (int)-1;
//	Message(1,"iGECROS != 0 ");
//	//SG20140710: für GECROS im rfs-Modus
//	iGECROSDefault = -1;
//	}
//
//    }//if pPl!=NULL
//
//	}
//
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80001
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80001)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//
//    if (pPl!=NULL)
//    {
//
//	 iGECROS = (int)0;
//     fGECROS = (float)1;
//
//	 GET_TEST_FLOAT(fparLEGUME , -2, +2, "");
//	 GET_TEST_FLOAT(fparC3C4   , -2, +2, "");
//	 GET_TEST_FLOAT(fparDETER  , -2, +2, "");
//	 GET_TEST_FLOAT(fparSLP    , -2, +2, "");
//	 GET_TEST_FLOAT(fparLODGE  , -2, +2, "");
//	 GET_TEST_FLOAT(fparVERN   , -2, +2, "");
//
//	}//if pPl!=NULL
//
//	}//80001
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80002
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80002)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//	 iGECROS = (int)0;
//     fGECROS = (float)2;
//
//	 GET_TEST_FLOAT(fparYGV  , 0, 1, "");
//	 GET_TEST_FLOAT(fparCFV  , 0, 1, "");
//	 GET_TEST_FLOAT(fparEG   , 0, 1, "");
//	 GET_TEST_FLOAT(fparFFAT , 0, 1, "");
//	 GET_TEST_FLOAT(fparFLIG , 0, 1, "");
//	 GET_TEST_FLOAT(fparFOAC , 0, 1, "");
//	 GET_TEST_FLOAT(fparFMIN , 0, 1, "");
//
//	}//if pPl!=NULL
//
//	}//80002
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80003
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80003)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//	 iGECROS = (int)0;
//     fGECROS = (float)3;
//
//	 GET_TEST_FLOAT(fparLWIDTH , 0, 1,    "");
//	 GET_TEST_FLOAT(fparCDMHT  , 0, 20000, ""); //SG20170321: for Sugarbeet
//	 GET_TEST_FLOAT(fparRDMX   , 0, 500,  "");
//	 GET_TEST_FLOAT(fparTBD    , 0, 100,  "");
//	 GET_TEST_FLOAT(fparTOD    , 0, 100,  "");
//	 GET_TEST_FLOAT(fparTCD    , 0, 100,  "");
//	 GET_TEST_FLOAT(fparTSEN   , 0, 100,  "");
//
//	}//if pPl!=NULL
//
//	}//80003
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80004
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80004)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//	 iGECROS = (int)0;
//     fGECROS = (float)4;
//
//	 GET_TEST_FLOAT(fparNUPTX  , 0, 10, "");
//	 GET_TEST_FLOAT(fparRNCMIN , 0, 1,  "");
//	 GET_TEST_FLOAT(fparSTEMNC , 0, 1,  "");
//	 GET_TEST_FLOAT(fparSLNMIN , 0, 1,  "");
//	 GET_TEST_FLOAT(fparLNCI   , 0, 10, "");
//	 GET_TEST_FLOAT(fparSLA0   , 0, 1,  "");
//	 GET_TEST_FLOAT(fparCCFIX  , 0, 100,"");
//
//	}//if pPl!=NULL
//
//	}//80004
//	
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80005
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80005)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//	 iGECROS = (int)0;
//     fGECROS = (float)5;
//
//	 GET_TEST_FLOAT(fparINSP   , -180, 180, "");
//	 GET_TEST_FLOAT(fparSPSP   , 0, 2,      "");
//	 GET_TEST_FLOAT(fparEPSP   , 0, 2,      "");
//	 GET_TEST_FLOAT(fparEAJMAX , 0, 200000, "");
//	 GET_TEST_FLOAT(fparXVN    , 0, 1000,   "");
//	 GET_TEST_FLOAT(fparXJN    , 0, 1000,   "");
//	 GET_TEST_FLOAT(fparTHETA  , 0, 10,     "");
//
//	}//if pPl!=NULL
//
//	}//80005
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80006
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80006)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//	 iGECROS = (int)0;
//     fGECROS = (float)6;
//
//	 GET_TEST_FLOAT(fparSEEDW , 0, 25, "");
//	 GET_TEST_FLOAT(fparSEEDNC, 0, 1,  "");
//	 GET_TEST_FLOAT(fparBLD   , 0, 90, "");
//	 GET_TEST_FLOAT(fparHTMX  , 0, 10, "");
//	 GET_TEST_FLOAT(fparMTDV  , 0, 200,"");
//	 GET_TEST_FLOAT(fparMTDR  , 0, 200,"");
//	 GET_TEST_FLOAT(fparPSEN  ,-10, 10,"");
//
//	}//if pPl!=NULL
//
//	}//80006
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80007
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80007)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//	 iGECROS = (int)0;
//     fGECROS = (float)7;
//
//	 GET_TEST_FLOAT(fparPMEH , 0, 1, "");
//	 GET_TEST_FLOAT(fparPMES , 0, 1, "");
//	 GET_TEST_FLOAT(fparESDI , 0, 2, "");
//	 GET_TEST_FLOAT(fparWRB  , 0, 10,"");
//
//	}//if pPl!=NULL
//
//	}//80007
//	
//
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80008
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80008)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
//	pPl = GetPlantPoi();
//    
//    if (pPl!=NULL)
//    {
//	 iGECROS = (int)0;
//     fGECROS = (float)8;
//
//	 GET_TEST_FLOAT(fparCO2A  , 0, 1000, "");
//	 GET_TEST_FLOAT(fparCOEFR , 0, 10, "");
//	 GET_TEST_FLOAT(fparCOEFV , 0, 10, "");
//	 GET_TEST_FLOAT(fparCOEFT , 0, 10, "");
//	 GET_TEST_FLOAT(fparFCRSH , 0, 1,  "");
//	 GET_TEST_FLOAT(fparFNRSH , 0, 1,  "");
//     SKIP_LINE;           
//	 GET_TEST_FLOAT(fparPNPRE , 0, 10, "");
//	 GET_TEST_FLOAT(fparCB    , 0, 10, "");
//	 GET_TEST_FLOAT(fparCX    , 0, 10, "");
//	 GET_TEST_FLOAT(fparTM    , 0, 10, "");
//
//	}//if pPl!=NULL
//
//	}//80008
//	
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80009
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80009)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
// 	iGECROS = (int)0;
//    fGECROS = (float)9;
//
//	GET_TEST_FLOAT(fparCLAY  , 0, 100,  "");
//	GET_TEST_FLOAT(fparWCMIN , 0, 1,    "");
//	GET_TEST_FLOAT(fparWCPWC , 0, 2000, "");
//	GET_TEST_FLOAT(fparPRFDPT, 0, 2000, "");
//	GET_TEST_FLOAT(fparSD1   , 0, 100,  "");
//	GET_TEST_FLOAT(fparTCT   , 0, 10,   "");
//	GET_TEST_FLOAT(fparTCP   , 0, 10,   "");
//	GET_TEST_FLOAT(fparBIOR  , 0, 10,   "");
//	GET_TEST_FLOAT(fparHUMR  , 0, 10,   "");
//
//	if (fparWCPWC >= fparPRFDPT*(float)10)
//	{
//     Message(1,"PWC >= ProfileDepth !!! then PWC=ProfileDepth/5 ");
//     fparWCPWC = fparPRFDPT*(float)2;
//	}
//
//	}//80009
//	
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80010
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80010)     
//    {    
//	GET_INT;
//    SKIP_LINE;           
//
// 	iGECROS = (int)0;
//    fGECROS = (float)10;
//
//	GET_TEST_FLOAT(fparPNLS  , 0, 1,   "");
//	GET_TEST_FLOAT(fparDRPM  , 0, 100, "");
//	GET_TEST_FLOAT(fparDPMR0 , 0, 100, "");
//	GET_TEST_FLOAT(fparRPMR0 , 0, 100, "");
//	GET_TEST_FLOAT(fparTOC   , 0, 50000, "");
//	GET_TEST_FLOAT(fparBHC   , 0, 50000, "");
//	GET_TEST_FLOAT(fparFBIOC , 0, 1,   "");
//
//	}//80010
//
////SG 20180412: additional parameters for GECROS-SB (J. Rabe sugarbeet model)
///*
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//  80011
// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
//*/
//SKIP_TO_MARKER(80011)
//{
//	GET_INT;
//	SKIP_LINE;
//
//	pPl = GetPlantPoi();
//
//	if (pPl != NULL)
//	{
//		iGECROS = (int)0;
//		fGECROS = (float)11;
//
//		GET_TEST_FLOAT(fparSINKBEET, 0, 2000, "");
//		GET_TEST_FLOAT(fparEFF, 0, 100, "");
//		GET_TEST_FLOAT(fparCFS, 0, 1, "");
//
//		
//	}//if pPl!=NULL
//
//}//80011

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//ENDE READMODFILE
//////////////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////////////////

    GlobalUnlock(hData);
    GlobalFree(hData);
                          
  return iLines;
}   /* ReadModFile */

                                           
//int WINAPI Init99InputGECROS(void)
//{
//jGECROS = (int)0;
////80008
//fparCO2A=(float)-99,fparCOEFR=(float)-99,fparCOEFV=(float)-99,fparCOEFT=(float)-99;
//fparFCRSH=(float)-99,fparFNRSH=(float)-99;
//fparPNPRE=(float)-99,fparCB=(float)-99,fparCX=(float)-99,fparTM=(float)-99;
//
////80009
//fparCLAY=(float)-99,fparWCMIN=(float)-99,fparWCPWC=(float)-99;
//fparPRFDPT=(float)-99,fparSD1=(float)-99,fparTCT=(float)-99,fparTCP=(float)-99;
//fparBIOR=(float)-99,fparHUMR=(float)-99;
//
//fparWCMAX=(float)-99, fparRSS=(float)-99;
//
////80010
//fparPNLS=(float)-99,fparDRPM=(float)-99,fparDPMR0=(float)-99,fparRPMR0=(float)-99;
//fparTOC=(float)-99,fparBHC=(float)-99,fparFBIOC=(float)-99;
//
//return 1;
//}
         
/******************************************************************************
** EOF */
