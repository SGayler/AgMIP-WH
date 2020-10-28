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
 * $History: startout.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:03
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
 *   21.07.97
 *
*******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <crtdbg.h>

#include "xlanguag.h"
#include "xinclexp.h"
#include "xh2o_def.h"
#include "xn_def.h"


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


/* from util_fct.c */
extern int WINAPI Message(long, LPSTR);
            
extern float afConstRootDens[MAXSCHICHT];
extern float afRSPar[8]; 
extern float fh50Global;
extern float fp1Global;
extern int   iWaterMobImm;
extern int   iWaterUptakeModel;
extern int   iIsConstRootDens;

//SG 20130917: für AgMIP-Soil für schichtweise Ausgabe im restart-file
extern int iNumb_lch, iNumb_wct, iNumb_mpt, iNumb_hct, iNumb_nct;
int i;

//SG 20140710: for GECROS in rfs-mode
extern int   iGECROS, iGECROSDefault;
extern float fGECROS;
extern float fparLEGUME,fparC3C4,fparDETER,fparSLP,fparLODGE,fparVERN;
int iLEGUME,iC3C4,iDETER,iSLP,iLODGE,iVERN;;
extern float fparEG,fparCFV,fparYGV,fparFFAT,fparFLIG,fparFOAC,fparFMIN;
extern float fparTBD,fparTOD,fparTCD,fparTSEN,fparLWIDTH,fparRDMX,fparCDMHT;
extern float fparLNCI,fparCCFIX,fparNUPTX,fparSLA0,fparSLNMIN,fparRNCMIN,fparSTEMNC;
extern float fparINSP,fparSPSP,fparEPSP,fparEAJMAX,fparXVN,fparXJN,fparTHETA;
extern float fparSEEDW,fparSEEDNC,fparBLD,fparHTMX,fparMTDV,fparMTDR,fparPSEN;
extern float fparPMEH,fparPMES,fparESDI,fparWRB;
extern float fparCO2A,fparCOEFR,fparCOEFV,fparCOEFT,fparFCRSH,fparFNRSH;
extern float fparPNPRE,fparCB,fparCX,fparTM;
extern float fparPRFDPT;
extern float fparCLAY,fparWCMIN,fparWCPWC,fparRSS,fparSD1,fparTCT,fparTCP,fparBIOR,fparHUMR;
extern float fparPNLS,fparDRPM,fparDPMR0,fparRPMR0,fparTOC,fparBHC,fparFBIOC;


int WINAPI PrintStart(EXP_POINTER,int);
int WINAPI PrintHydraulics(EXP_POINTER,int);
int WINAPI PrintLayerResult(EXP_POINTER);

  FILE *WFFile;
  FILE *WCFile;
  FILE *WPFile;
  FILE *STFile;
  FILE *NOFile;
  FILE *NHFile;
  FILE *DONFile;
  FILE *DOCFile;

  char acWFName[80] = "";
  char acWCName[80] = "";
  char acWPName[80] = "";
  char acSTName[80] = "";
  char acNOName[80] = "";
  char acNHName[80] = "";
  char acDONName[80] = "";
  char acDOCName[80] = "";

float PrintDepthTab[12];
float WCPrintDepthTab[12];
float WPPrintDepthTab[12];
float STPrintDepthTab[12];
float NCPrintDepthTab[12];
int   iSoilWaterFluxOutput;
int   iSoilWaterContentOutput;
int   iSoilWaterMatricPotOutput;
int   iSoilTemperatureOutput;
int   iSoilNitrogenContentOutput;


#ifndef MESSAGE_LENGTH
  #define MESSAGE_LENGTH  70
#endif  

#define THIN_LINE "----------------------------------------------------------------------"
#define FAT_LINE  "======================================================================"
#define LONG_THIN_LINE "----------------------------------------------------------------------------------------------------------------------"
#define LONG_FAT_LINE  "======================================================================================================================"

#define DOUT(x) if (x < EPSILON) x = (float) 0;\
                if (x < 0.05) x = (float) 0;\
                else if (x < 0.1)  x = (float) 0.1;\
                if (x < (float)1) gcvt((double)x,2,acDummy2);\
	            else if (x < (float)10) gcvt((double)x,3,acDummy2);\
	            else if (x < (float)100) gcvt((double)x,4,acDummy2);\
	            else if (x < (float)1000) gcvt((double)x,4,acDummy2);\
	            else gcvt((double)x,5,acDummy2);

/*******************************************************************************************
 *
 *  Name     : PrintStart()
 *  Funktion : Gibt  die Werte aus,
 *				die für eine erneute Simulation als Startwerte benutzt werden koennen.
 *
 *              ch 23.7.97
 *
 ******************************************************************************************
 */
int WINAPI PrintStart(EXP_POINTER, int iMessageStatus)
{ 
  DECLARE_COMMON_VAR
  // DECLARE_POINTER  
  PSLAYER      pSL;
  PCLAYER      pCL;
  // PCPROFILE    pCP;
  PCPARAM      pPA;
  PSWATER      pSW;
  PWLAYER      pWL;
  PHLAYER      pHL;
  // PNFERTILIZER pNF;

  HANDLE hFile;

  int iStrLength;
  double f1;
  char *acFileName;                           
  char *acDummy2; 

  acDummy2 = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.
  acFileName      = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.


  if (strlen(pLo->pFieldplot->acName)<2)
  {
    lstrcpyn(acFileName,"expertn",7);
  }
  else
  {
    lstrcpyn(acFileName,pLo->pFieldplot->acName,7);
  }
    
  // test  pTi->pSimTime->lStopDate = 120394;       
  itoa( (int)(pTi->pSimTime->lStopDate % 100),acDummy2,10);
  lstrcat(acFileName,acDummy2);

    
  strcpy(acDummy2,"param\\");
  strcat(acDummy2,acFileName);
  strcat(acDummy2,".rfs");
  hFile = OpenF(acDummy2);


  if (hFile == INVALID_HANDLE_VALUE)
  {
    //Message(1,FILEOPEN_ERROR_TXT);
  }

  else
  {        
	 
    strcat(acDummy2," opened for start value file.");
    Message(0, acDummy2);


    fout_string(hFile, FAT_LINE);
    fout_line(hFile, 1);     
		
    fout_string(hFile, "EXPERT-N : Start Values, generated by dllCHOutStart.");

    fout_string(hFile, " ");
    _strdate(acDummy2);            
    fout_string(hFile, acDummy2);

    fout_string(hFile, " ");

    _strtime(acDummy2);            
    fout_string(hFile, acDummy2);

    fout_line(hFile, 2);     

    fout_string(hFile, "SimFile: ");
    
    fout_string(hFile, acFileName); 
    
    fout_line(hFile, 1);
    
    fout_string(hFile, "SimDate: ");
    fout_long(hFile, &(pTi->pSimTime->lTimeDate));
    fout_line(hFile, 1);

    fout_string(hFile, "SimTime: ");
    fout_floatNorm(hFile, pTi->pSimTime->fTimeAct, 2, 3);

    fout_line(hFile, 2);     

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	  10010
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "BASIC SOIL PROPERTIES");//FOR SIMULATION LAYERS (equidistant)");
      fout_string(hFile, "  simulation result of  ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Layer\t|Num.\t|Clay\t|Silt\t|Sand\t|Org.\t|Bulk\t|Rock\t|pH-Value");
      fout_line(hFile, 1);
      fout_string(hFile, "     \t|steps|    \t|    \t|    \t|Mat.\t|Dens.|Frac.|      ");
      fout_line(hFile, 1);
      fout_string(hFile, "\t|1/1\t|Wght%|Wght%|Wght%|Wght%|g/cm3|Vol%\t|1/1");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, ";10010 this marker is a comment only");
      fout_line(hFile, 1);

      for (N_SOIL_LAYERS)
      {
        fout_int(hFile, &(iLayer), 3);

        fout_string(hFile,"\t");
        fout_string(hFile,"1");
        fout_string(hFile,"\t");

        gcvt((double)pSL->fClay,4,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        gcvt((double)pSL->fSilt ,4,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        gcvt((double)pSL->fSand ,4,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        if (pSL->fBulkDens * pSL->fThickness * 100 * ((float)1 -  pSL->fRockFrac) > EPSILON)
          f1 = pSL->fCHumus /((float)0.58 * pSL->fBulkDens * pSL->fThickness * 100 * ((float)1 -  pSL->fRockFrac)) ; // kg -> Gew %
        else
          f1 = (float)-99;
    
        if (f1<(float)1)
          gcvt((double)f1 ,2,acDummy2);
        else
          gcvt((double)f1 ,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        f1 = pSL->fBulkDens;// [kg/dm3]
        gcvt((double)f1 ,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        f1 = pSL->fRockFrac*(float)100;// [Vol.%]
        gcvt((double)f1 ,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        gcvt((double)pSL->fpH,3,acDummy2);
        fout_string(hFile, acDummy2);

        fout_line(hFile, 1);
      }
    }

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	10011
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
	{
    PLAYERROOT pLR;
    if (pPl != NULL) pLR=pPl->pRoot->pLayerRoot;
	 
	fout_string(hFile, FAT_LINE);
        
    fout_line(hFile, 1);
	fout_string(hFile, "START VALUES SOIL PROFILE simulation result of  ");
    fout_string(hFile, acFileName); 
    fout_line(hFile, 1);

	//fout_string(hFile, "10018 ");
    //fout_line(hFile, 1);
	//fout_string(hFile, "Date ");
    //fout_line(hFile, 1);
    //fout_long(hFile, &(pTi->pSimTime->lTimeDate));
    //fout_line(hFile, 1);
	fout_string(hFile, THIN_LINE);
    fout_line(hFile, 1);
	fout_string(hFile, "Layer\t|Num.\t|Water|Matric\t|Soil \t|Amount|Amount\t|Root"); 
    fout_line(hFile, 1);
    fout_string(hFile, "\t|st.\t|Cont.|Pot.\t\t|Temp.\t|NH4-N |NO3-N\t|dens.");
    fout_line(hFile, 1);
    fout_string(hFile, "\t|1/1\t|Vol%\t|kPa\t\t|°C\t\t|kgN/ha|kgN/ha\t|1/1 ");
    fout_line(hFile, 1);
	fout_string(hFile, THIN_LINE);
    fout_line(hFile, 1);
//	fout_string(hFile, ";10011 this marker is a comment only");
	fout_string(hFile, "10011");
    fout_line(hFile, 1);     
    
    
    for(N_SOIL_LAYERS)
    {
	fout_int(hFile, &(iLayer), 3);
        fout_string(hFile, "\t");

    fout_string(hFile,"1");
       fout_string(hFile,"\t");

    f1 = (double)pWL->fContAct *(double)100;
    gcvt(f1,3,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile, "\t");

    f1 = pWL->fMatPotAct/(float)102.2; //[mm] -> [kPa]
    if(f1>(float)-100) gcvt((double)f1,3,acDummy2);
	else if (f1>(float)-1000) gcvt((double)f1,4,acDummy2);
	else gcvt((double)f1,4,acDummy2);
       fout_string(hFile, acDummy2);
    if (f1<(float)-100) fout_string(hFile, "\t");
    else fout_string(hFile, "\t\t");
	
    gcvt((double)pHL->fSoilTemp,4,acDummy2);
       fout_string(hFile, acDummy2);
       fout_string(hFile, "\t\t");

	DOUT(pCL->fNH4N)
    //gcvt((double)pCL->fNH4N,4,acDummy2);
       fout_string(hFile, acDummy2);
       fout_string(hFile, "\t");

	DOUT(pCL->fNO3N)
    //gcvt((double)pCL->fNO3N,4,acDummy2);
       fout_string(hFile, acDummy2);
       fout_string(hFile, "\t\t");

    if (pPl == NULL)
       fout_string(hFile,"\t-99\t");
	else
	  {
	   if(iIsConstRootDens)
		  {
		   gcvt((double)afConstRootDens[iLayer],4,acDummy2);
		  }
	   else
	      {
           if(pLR->pNext!=NULL) pLR=pLR->pNext;
           gcvt((double)pLR->fLengthDens,4,acDummy2);
	      }
	   fout_string(hFile, acDummy2);
	  }//(pPl != NULL)
       
	   fout_line(hFile, 1);
    }
}	

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    10020
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "SOIL PHYSICAL PARAMETERS");
      fout_string(hFile, "  simulation result of  ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Layer\t|Soil\t\t|Wilting\t|Field\t|Total Pore\t|Satur. Hydr.");
      fout_line(hFile, 1);
      fout_string(hFile, "\t|type\t\t|Point\t|Capacity\t|Volume\t|Conductivity");
      fout_line(hFile, 1);
      fout_string(hFile, "\t|ADV\t\t|Vol.%\t|Vol.%\t|Vol.%\t|mm/d");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
	  //SG 20140704: Marker 10020 auch im .rfs aktiv!
	  fout_string(hFile, "10020");    // neu
	  //fout_string(hFile, ";10020 this marker is a comment only"); // alt
      fout_line(hFile, 1);

      for (N_SOIL_LAYERS)
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");

        strncpy(acDummy2, pSL->acSoilID,4);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t\t");

        f1 = pSW->fContPWP*(float)100;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t\t");
                                     
        f1 = pSW->fContFK*(float)100;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t\t");
                                     
        f1 = pSL->fPorosity*(float)100;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t\t");
        /*              
        f1 = pSW->fMinPot/(float)102.2*(float)1000;//mm -> kPa -> Pa
        gcvt((double)f1,2,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t\t");
        */              
        gcvt((double)pSW->fCondSat,5,acDummy2);
        fout_string(hFile, acDummy2);

        fout_line(hFile, 1);
      }
    }

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    10021
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "SOIL HYDRAULIC CURVE PARAMETERS");
      fout_string(hFile, "  simulation result of  ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Layer\t|  Water\t|Campbell\t|van Genuchten\t\t|Mualem");
      fout_line(hFile, 1);
      fout_string(hFile, "\t|Res.\t|Sat.\t|A\t|B\t|alfa\t\t|n\t|m\t|p\t|q\t|r");
      fout_line(hFile, 1);
      fout_string(hFile, "\t|  Vol.%\t|kPa\t|1/1\t|1/cm\t\t|1/1\t|1/1\t|1/1\t|1/1\t|1/1");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
	  //SG 20140704: Marker 10021 auch im .rfs aktiv!
	  fout_string(hFile, "10021");    // neu
	  //fout_string(hFile, ";10021 this marker is a comment only"); // alt
      fout_line(hFile, 1);

      for (N_SOIL_LAYERS)
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");
        /*
        f1 = pSW->fContInflec*(float)100;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");
                                      
        f1 = pSW->fPotInflec/(float)102.2;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");
        */                              
        f1 = pSW->fContRes*(float)100;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");
      
        f1 = pSW->fContSat*(float)100;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");
      
        f1 = pSW->fCampA/(float)102.2;
        if (f1<(float)1)
          gcvt((double)f1,2,acDummy2);
        else
          gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");
                                     
        gcvt((double)pSW->fCampB,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        f1 = pSW->fVanGenA*(float)10;// 1/mm to 1/cm
        //if (pSW->fVanGenA==(float)0.1) f1 = pSW->fVanGenA2*(float)10;//falls bimodale PTF
                                                                     //nach Scheinost 1995
        if ((f1<-98.9)&&(f1>-99.1))
          fout_string(hFile,"-99\t");
        else
        { 
          gcvt((double)f1,3,acDummy2);
          fout_string(hFile, acDummy2);
          fout_string(hFile,"\t");
        }
       
        f1 = pSW->fVanGenN;
        //if (f1==(float)5)f1 = pSW->fVanGenN2;//falls bimodale PTF nach Scheinost 1995
	    //gcvt((double)pSW->fVanGenN,3,acDummy2);
	    gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        //gcvt((double)pSW->fVanGenM,3,acDummy2);
        //fout_string(hFile, acDummy2);
        //fout_string(hFile,"\t");

        gcvt(0.5,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

		//SG 20140704: aktiviert für fitting im .rfs-Mode
        
        //gcvt((double)pSW->fMualemP,3,acDummy2);
        //fout_string(hFile, acDummy2);
        //fout_string(hFile,"\t");

		// anstelle von pSW->fMualemP:
		gcvt((double)pSW->fVanGenM,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");


        gcvt((double)pSW->fMualemQ,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        gcvt((double)pSW->fMualemR,3,acDummy2);
        fout_string(hFile, acDummy2);
        

        fout_line(hFile, 1);
      }
    }

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    10022
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "SOIL HYDRAULIC PARAMETERS IMMOBILE REGION");
      fout_string(hFile, "  simulation result of ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Layer\t| Immob.\t|Exch.\t||Immob.\t|van Genuchten\t\t");
      fout_line(hFile, 1);
      fout_string(hFile, "\t| Frac.\t|Time\t\t||Weight\t|alfa2\t|n2\t|m2");
      fout_line(hFile, 1);
      fout_string(hFile, "\t|  1/1\t| d\t\t|| 1/1   \t|1/cm\t\t|1/1\t|1/1");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
	  if (iWaterMobImm > 0) 
		  fout_string(hFile, "10022    ");
	  else 
		  fout_string(hFile, ";10022 this marker is a comment only");
	  fout_int(hFile,&(iWaterMobImm), 1);
      fout_line(hFile, 1);

      for (N_SOIL_LAYERS)
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");

		f1 = pSW->fContImmSat/pSW->fContSat;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t\t");
      
        f1 = (float)1;
		if (pSW->fMobImmEC > 0) f1 = (float)1/pSW->fMobImmEC;
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");
      
        f1 = pSW->fBiModWeight2;
        gcvt((double)f1,2,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t\t");
                                     
        f1 = pSW->fVanGenA2*(float)10;// 1/mm to 1/cm
        gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");
       
        f1 = pSW->fVanGenN2;
	    gcvt((double)f1,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        gcvt((double)pSW->fVanGenM2,3,acDummy2);
        fout_string(hFile, acDummy2);
        fout_string(hFile,"\t");

        fout_line(hFile, 1);
      }
    }


    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20001
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "MINERALIZATION MODEL PARAMETERS");
      fout_line(hFile, 1);
  
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
  
      fout_string(hFile, "Layer |Effic:HumF:MinCN |Temp0 :Q10 |Theta0 :Theta1 :SatActiv |NO3:NH4");
      fout_line(hFile, 1);
  
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
  
      fout_string(hFile, "20001");
      fout_line(hFile, 1);     
      
      
      for (SOIL_LAYERS1(pPA,pCh->pCParam->pNext))
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");
        
        if (pPA->fMinerEffFac < EPSILON)
          pPA->fMinerEffFac = (float) 0;
        gcvt((double)pPA->fMinerEffFac,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pPA->fMinerHumFac < EPSILON)
          pPA->fMinerHumFac = (float) 0;
        gcvt((double)pPA->fMinerHumFac,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pCh->pCLayer->pNext->fMicBiomCN < EPSILON)
          pCh->pCLayer->pNext->fMicBiomCN = (float) 0;
        gcvt((double)pCh->pCLayer->pNext->fMicBiomCN,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
  
        if (pPA->fMinerTempB < EPSILON)
          pPA->fMinerTempB = (float) 0;
        gcvt((double)pPA->fMinerTempB,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pPA->fMinerQ10 < EPSILON)
          pPA->fMinerQ10 = (float) 0;
        gcvt((double)pPA->fMinerQ10,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pPA->fMinerThetaMin < EPSILON)
          pPA->fMinerThetaMin = (float) 0;
        gcvt((double)pPA->fMinerThetaMin,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
  
        // Dummy
        fout_string(hFile,"-99\t");
      
        if (pPA->fMinerSatActiv < EPSILON)
          pPA->fMinerSatActiv = (float) 0;
        gcvt((double)pPA->fMinerSatActiv,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pPA->fNitrifNO3NH4Ratio < EPSILON)
          pPA->fNitrifNO3NH4Ratio = (float) 0;
        gcvt((double)pPA->fNitrifNO3NH4Ratio,4,acDummy2);
        fout_string(hFile,  acDummy2);
      
        fout_line(hFile, 1);
      }
    }
  	
  
    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20002
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
  
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "TRANSFORMATION RATES MODEL PARAMETERS");
      fout_line(hFile, 1);
  
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
  
      fout_string(hFile, "Layer | UreaHy : Nitrif. : Denitrif. : MinerLit : MinerMan : MinerHum");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
  
      fout_string(hFile, "20002");
      fout_line(hFile, 1);     
      
      
      for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");
  
      
        if (pCL->fUreaHydroMaxR < EPSILON)
          pCL->fUreaHydroMaxR = (float) 0;
        gcvt((double)pCL->fUreaHydroMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pCL->fNH4NitrMaxR < EPSILON)
          pCL->fNH4NitrMaxR = (float) 0;
        gcvt((double)pCL->fNH4NitrMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pCL->fNO3DenitMaxR < EPSILON)
          pCL->fNO3DenitMaxR = (float) 0;
        gcvt((double)pCL->fNO3DenitMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
  
        if (pCL->fLitterMinerMaxR < EPSILON)
          pCL->fLitterMinerMaxR = (float) 0;
        gcvt((double)pCL->fLitterMinerMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
            fout_string(hFile,"\t");
     
        if (pCL->fManureMinerMaxR < EPSILON)
          pCL->fManureMinerMaxR = (float) 0;
        gcvt((double)pCL->fManureMinerMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
     
        if (pCL->fHumusMinerMaxR < EPSILON)
          pCL->fHumusMinerMaxR = (float) 0;
        gcvt((double)pCL->fHumusMinerMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
     
        fout_line(hFile, 1);
      }
    }  
    
    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20003
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "START VALUES C-,N-POOLS    simulation result of  ");
      //fout_string(hFile, "ONLY FOR TEST: C-,N-POOLS will NOT be read from this file");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Layer\t|Litter C\tLitter N\t|Manure C\tManure N\t|Humus C\tHumus N");
      fout_line(hFile, 1);
      fout_string(hFile, "\t|kg C/ha\tkg N/ha\t|kg C/ha\tkg N/ha\t|kg C/ha\tkg N/ha");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "20003");
      // fout_string(hFile, ";20003 this marker is a comment only");
      fout_line(hFile, 1);     
    
    
      for (SOIL2_LAYERS1(pCL,pCh->pCLayer->pNext,pSL,pSo->pSLayer->pNext))
      //for (N_SOIL_LAYERS)
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");

   
        DOUT(pCL->fCLitter)
        fout_string(hFile,acDummy2);
        fout_string(hFile,"\t\t");
   
        DOUT(pCL->fNLitter)
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t");
   
        DOUT(pCL->fCManure)
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t");

        DOUT(pCL->fNManure)
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t");

        DOUT(pSL->fCHumus)
        fout_string(hFile,  acDummy2);
        iStrLength=strlen(acDummy2);
        if (iStrLength < (int)6)
          fout_string(hFile,"\t\t");
        else
          fout_string(hFile,"\t");

        DOUT(pSL->fNHumus)
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t");

        fout_line(hFile, 1);
      }
    }  
    
    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20004
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "START VALUES SURFACE C-,N-POOLS    simulation result of  ");
      //fout_string(hFile, "ONLY FOR TEST: C-,N-POOLS will NOT be read from this file");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Surf.\t|Litter C\tLitter N\t|Manure C\tManure N\t|Humus C\tHumus N");
      fout_line(hFile, 1);
      fout_string(hFile, "Layer\t|kg C/ha\tkg N/ha\t|kg C/ha\tkg N/ha\t|kg C/ha\tkg N/ha");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
     
      fout_string(hFile, "20004");
      // fout_string(hFile, ";20004 this marker is a comment only");
      fout_line(hFile, 1);     
     
      {
        iLayer=(int)1;                                
        fout_int(hFile,&(iLayer), 2);                 
        fout_string(hFile,"\t");                      
                                                      
                                                      
        DOUT(pCh->pCProfile->fCLitterSurf)            
        fout_string(hFile,acDummy2);                  
        fout_string(hFile,"\t\t");                    
                                                      
        DOUT(pCh->pCProfile->fNLitterSurf)            
        fout_string(hFile,  acDummy2);                
        fout_string(hFile,"\t\t");                    
                                                      
        DOUT(pCh->pCProfile->fCManureSurf)            
        fout_string(hFile,  acDummy2);                
        fout_string(hFile,"\t\t");                    
                                                      
        DOUT(pCh->pCProfile->fNManureSurf)            
        fout_string(hFile,  acDummy2);                
        fout_string(hFile,"\t\t");                    
                                                      
        DOUT(pCh->pCProfile->fCHumusSurf)             
        fout_string(hFile,  acDummy2);                
        if (pCh->pCProfile->fCHumusSurf < (float)1000)
          fout_string(hFile,"\t\t");                  
        else                                          
          fout_string(hFile,"\t");                    
                                                      
        DOUT(pCh->pCProfile->fNHumusSurf)             
        fout_string(hFile,  acDummy2);                
        fout_string(hFile,"\t\t");                    
                                                      
        fout_line(hFile, 1);                          
      }
    }

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20005
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "START VALUES STANDING CROP C-,N-RESIDUES simulation result of  ");
      // fout_string(hFile, "ONLY FOR TEST: C-, N-POOLS will NOT be read from this file");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Stand\t|Residue C\tResidue N\t|");
      fout_line(hFile, 1);
      fout_string(hFile, "Crops\t|kg C/ha\t kg N/ha\t|");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
    
      fout_string(hFile, "20005");
      // fout_string(hFile, ";20005 this marker is a comment only");
      fout_line(hFile, 1);     
      
      {
        iLayer=(int)1;
        fout_int(hFile,&(iLayer), 2);
        fout_string(hFile,"\t");
      
      
        DOUT(pCh->pCProfile->fCStandCropRes)
        fout_string(hFile,acDummy2);
        fout_string(hFile,"\t\t");
      
        DOUT(pCh->pCProfile->fNStandCropRes)
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t");
      
        fout_line(hFile, 1);
      }
    }
  
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	20006
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/ 
  {

    fout_string(hFile, FAT_LINE);
    fout_line(hFile, 1);
	fout_string(hFile, "DON MAXIMAL TRANSFORMATION RATES ");
    fout_line(hFile, 1);

	fout_string(hFile, THIN_LINE);
    fout_line(hFile, 1);

    fout_string(hFile, "Layer | HumusDON : LitterDON : ManureDON : DONHumus : DONLitter : MinerDON");
    fout_line(hFile, 1);
	fout_string(hFile, THIN_LINE);
    fout_line(hFile, 1);

    fout_string(hFile, "20006");
      fout_line(hFile, 1);     
    
    
     for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
     {
	  fout_int(hFile, &(iLayer), 3);
      fout_string(hFile,"\t");

    
      if (pCL->fHumusToDONMaxR < EPSILON) 
		  pCL->fHumusToDONMaxR = (float) 0;
      gcvt((double)pCL->fHumusToDONMaxR,4,acDummy2);
	  fout_string(hFile,  acDummy2);
      fout_string(hFile,"\t");
    
      if (pCL->fLitterToDONMaxR < EPSILON) 
		  pCL->fLitterToDONMaxR = (float) 0;
      gcvt((double)pCL->fLitterToDONMaxR,4,acDummy2);
      fout_string(hFile,  acDummy2);
      fout_string(hFile,"\t");
    
      if (pCL->fManureToDONMaxR < EPSILON) 
		  pCL->fManureToDONMaxR = (float) 0;
      gcvt((double)pCL->fManureToDONMaxR,4,acDummy2);
      fout_string(hFile,  acDummy2);
      fout_string(hFile,"\t");

      if (pCL->fDONToHumusMaxR < EPSILON) 
		  pCL->fDONToHumusMaxR = (float) 0;
      gcvt((double)pCL->fDONToHumusMaxR,4,acDummy2);
      fout_string(hFile,  acDummy2);
      fout_string(hFile,"\t");

      if (pCL->fDONToLitterMaxR < EPSILON) 
		  pCL->fDONToLitterMaxR = (float) 0;
      gcvt((double)pCL->fDONToLitterMaxR,4,acDummy2);
      fout_string(hFile,  acDummy2);
      fout_string(hFile,"\t");

      if (pCL->fDONMinerMaxR < EPSILON)
		  pCL->fDONMinerMaxR = (float) 0;
      gcvt((double)pCL->fDONMinerMaxR,4,acDummy2);
      fout_string(hFile,  acDummy2);

      fout_line(hFile, 1);
      }
    }  

  
    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20007
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */
    {

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
     
	  fout_string(hFile, "RATE CONSTANTS (DAISY,DNDC,NCSOIL)");
      fout_line(hFile, 1);

	  fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

	  fout_string(hFile, "Layer	FOMSlow : FOMFast : FOMVeryFast : MicBiomSlow : MicBiomFast : HumusSlow : HumusFast");
      fout_line(hFile, 1);

	  fout_string(hFile, "       1/d       1/d         1/d            1/d            1/d          1/d        1/d  ");
      fout_line(hFile, 1);
	  fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "20007");
      fout_line(hFile, 1);     
      
      for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile, "\t");
     
      
        if (pCL->fFOMSlowDecMaxR < EPSILON)
          pCL->fFOMSlowDecMaxR = (float) 0;
        gcvt((double)pCL->fFOMSlowDecMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
      
        if (pCL->fFOMFastDecMaxR < EPSILON) 
          pCL->fFOMFastDecMaxR = (float) 0;
        gcvt((double)pCL->fFOMFastDecMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

		if (pCL->fFOMVeryFastDecMaxR < EPSILON) 
		  pCL->fFOMVeryFastDecMaxR = (float) 0;
        gcvt((double)pCL->fFOMFastDecMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fMicBiomSlowDecMaxR < EPSILON) 
          pCL->fMicBiomSlowDecMaxR = (float) 0;
        gcvt((double)pCL->fMicBiomSlowDecMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pCL->fMicBiomFastDecMaxR < EPSILON) 
          pCL->fMicBiomFastDecMaxR = (float) 0;
        gcvt((double)pCL->fMicBiomFastDecMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pCL->fHumusSlowMaxDecMaxR < EPSILON) 
          pCL->fHumusSlowMaxDecMaxR = (float) 0;
        gcvt((double)pCL->fHumusSlowMaxDecMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pCL->fHumusFastMaxDecMaxR < EPSILON) 
          pCL->fHumusFastMaxDecMaxR = (float) 0;
        gcvt((double)pCL->fHumusFastMaxDecMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
      
        fout_line(hFile, 1);
      }
    }
	
    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20008
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */
    {

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
	  fout_string(hFile, "START VALUES SOIL C-POOLS (DAISY, DNDC, NCSOIL) simulation result of  ");
      fout_string(hFile, acFileName);
      fout_line(hFile, 1);
	  fout_string(hFile, "Layer | AOM1  | AOM2  | AOM3  | BOM1  | BOM2  | BOMD | SOM0 | SOM1  | SOM2  | Csol");
      fout_line(hFile, 1);
      fout_string(hFile, "[kg C/ha] ");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "20008");  
      fout_line(hFile, 1);
     
      for (N_SOIL_LAYERS) 
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile, "\t");
     
        if (pCL->fCFOMSlow < EPSILON) 
          pCL->fCFOMSlow = (float) 0;
        gcvt((double)pCL->fCFOMSlow,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        if (pCL->fCFOMFast < EPSILON) 
          pCL->fCFOMFast = (float) 0;
        gcvt((double)pCL->fCFOMFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
        
        if (pCL->fCFOMVeryFast < EPSILON)
          pCL->fCFOMVeryFast = (float) 0;
        gcvt((double)pCL->fCFOMVeryFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fCMicBiomSlow < EPSILON) 
          pCL->fCMicBiomSlow = (float) 0;
        gcvt((double)pCL->fCMicBiomSlow,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        if (pCL->fCMicBiomFast < EPSILON) 
          pCL->fCMicBiomFast = (float) 0;
        gcvt((double)pCL->fCMicBiomFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        if (pCL->fCMicBiomDenit < EPSILON) 
          pCL->fCMicBiomDenit = (float) 0;
        gcvt((double)pCL->fCMicBiomDenit,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        if (pCL->fCHumusStable < EPSILON) 
          pCL->fCHumusStable = (float) 0;
        gcvt((double)pCL->fCHumusStable,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        if (pCL->fCHumusSlow < EPSILON) 
          pCL->fCHumusSlow = (float) 0;
        gcvt((double)pCL->fCHumusSlow,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        if (pCL->fCHumusFast < EPSILON) 
          pCL->fCHumusFast = (float) 0;
        gcvt((double)pCL->fCHumusFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        if (pCL->fCsolC < EPSILON) 
          pCL->fCsolC = (float) 0;
        gcvt((double)pCL->fCsolC,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
       
        fout_line(hFile, 1);
      }
    }

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20009
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */
    {
	  fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
	  fout_string(hFile, "START VALUES SOIL N-POOLS  (DAISY, DNDC, NCSOIL) simulation result of  ");
      fout_string(hFile, acFileName);
      fout_line(hFile, 1);
	  fout_string(hFile, "Layer | AOM1  | AOM2  | AOM3  | BOM1  | BOM2  | BOMD | SOM0 | SOM1  | SOM2  | Csol");
      fout_line(hFile, 1);
	  fout_string(hFile, "[kg N/ha] ");
      fout_line(hFile, 1);
	  fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
	  fout_string(hFile, "20009");  
      fout_line(hFile, 1);

      for (N_SOIL_LAYERS) 
      {
	    fout_int(hFile, &(iLayer), 3);
        fout_string(hFile, "\t");

        if (pCL->fNFOMSlow < EPSILON)
		  pCL->fNFOMSlow = (float) 0;
        gcvt((double)pCL->fNFOMSlow,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fNFOMFast < EPSILON)
		  pCL->fNFOMFast = (float) 0;
        gcvt((double)pCL->fNFOMFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
    
        if (pCL->fNFOMVeryFast < EPSILON)
		  pCL->fNFOMVeryFast = (float) 0;
        gcvt((double)pCL->fNFOMVeryFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
    
        if (pCL->fNMicBiomSlow < EPSILON) 
		  pCL->fNMicBiomSlow = (float) 0;
        gcvt((double)pCL->fNMicBiomSlow,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fNMicBiomFast < EPSILON) 
		  pCL->fNMicBiomFast = (float) 0;
        gcvt((double)pCL->fNMicBiomFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fNMicBiomDenit < EPSILON) 
		  pCL->fNMicBiomDenit = (float) 0;
        gcvt((double)pCL->fNMicBiomDenit,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fNHumusStable < EPSILON)
		  pCL->fNHumusStable = (float) 0;
        gcvt((double)pCL->fNHumusStable,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fNHumusSlow < EPSILON)
		  pCL->fNHumusSlow = (float) 0;
        gcvt((double)pCL->fNHumusSlow,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

        if (pCL->fNHumusFast < EPSILON) 
		  pCL->fNHumusFast = (float) 0;
        gcvt((double)pCL->fNHumusFast,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");

     /* if (pCL->fNsolC < EPSILON)
	      pCL->fNsolC = (float) 0;
        gcvt((double)pCL->fCsolC,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
   */
    fout_line(hFile, 1);
      }
    }


    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20010
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "START VALUES  N2O, N2      simulation result of  ");
      fout_line(hFile, 1);
      fout_string(hFile, "Layer\tN2O\tN2");
      fout_line(hFile, 1);
      fout_string(hFile, "[kg N/ha]");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "20010");  
      fout_line(hFile, 1);
    
      for (N_SOIL_LAYERS) 
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile, "\t");
    
        if (pCL->fN2ON < EPSILON) 
          pCL->fN2ON = (float) 0;
        gcvt((double)pCL->fN2ON,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
    
        if (pCL->fN2N < EPSILON) 
          pCL->fN2N = (float) 0;
        gcvt((double)pCL->fN2N,4,acDummy2);
        fout_string(hFile,  acDummy2);
    
        fout_line(hFile, 1);
      }
    }
  

  
    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20011
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
   
      fout_string(hFile, "DENITRIFICATION MODEL PARAMETERS");
      fout_line(hFile, 1);
   
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
   
      fout_string(hFile, "Layer\tKsNO3\tKsC\tTheta0\tBioGrowth\tBio-MC");
      fout_line(hFile, 1);
      fout_string(hFile, "[kg N/ha]: [kg C/ha ]: [1]: [1/day]: [ ]");
      fout_line(hFile, 1);
   
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
   
      fout_string(hFile, "20011");
      fout_line(hFile, 1);     
      
      for (SOIL_LAYERS1(pPA,pCh->pCParam->pNext))
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");
    
        if (pPA->fDenitKsNO3 < EPSILON) 
          pPA->fDenitKsNO3 = (float) 0;
        gcvt((double)pPA->fDenitKsNO3,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
        
        if (pPA->fDenitKsCsol < EPSILON) 
          pPA->fDenitKsCsol = (float) 0;
        gcvt((double)pPA->fDenitKsCsol,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
        
        if (pPA->fDenitThetaMin < EPSILON) 
          pPA->fDenitThetaMin = (float) 0;
        gcvt((double)pPA->fDenitThetaMin,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
        
        if (pPA->fDenitMaxGrowR < EPSILON) 
          pPA->fDenitMaxGrowR = (float) 0;
        gcvt((double)pPA->fDenitMaxGrowR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
        
        if (pPA->fMaintCoeff < EPSILON) 
          pPA->fMaintCoeff = (float) 0;
        gcvt((double)pPA->fMaintCoeff,4,acDummy2);
        fout_string(hFile,  acDummy2);
     
        fout_line(hFile, 1);
      }
    }
	

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20012
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
    
      fout_string(hFile, "EVENTS MODEL PARAMETERS");
      fout_line(hFile, 1);
    
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
    
      fout_string(hFile, "Denitrification");
      fout_line(hFile, 1);
    
      fout_string(hFile, "N2O->N2  | Freezing | Thawing  |  Rewet");
      fout_line(hFile, 1);
    
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
    
      fout_string(hFile, "20012");
      fout_line(hFile, 1);     
      
      for (SOIL_LAYERS1(pPA,pCh->pCParam->pNext))
      {
        fout_int(hFile, &(iLayer), 3);
        fout_string(hFile,"\t");
        
        if (pPA->fN2ORedMaxR < EPSILON) 
          pPA->fN2ORedMaxR = (float) 0;
        gcvt((double)pPA->fN2ORedMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pPA->fDenitFreeze < EPSILON) 
          pPA->fDenitFreeze = (float) 0;
        gcvt((double)pPA->fDenitFreeze,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
      
        if (pPA->fDenitThaw < EPSILON) 
          pPA->fDenitThaw = (float) 0;
        gcvt((double)pPA->fDenitThaw,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t");
        
        if (pPA->fDenitReWet < EPSILON) 
          pPA->fDenitReWet = (float) 0;
        gcvt((double)pPA->fDenitReWet,4,acDummy2);
        fout_string(hFile,  acDummy2);
        
        fout_line(hFile, 1);
      }
    }

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	20013
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
	{
	fout_string(hFile, FAT_LINE);
    fout_line(hFile, 1);

	fout_string(hFile, "Max. NH3-N Volatilisation | NH4-N Deposition | NO3-N Deposition");
    fout_line(hFile, 1);

    fout_string(hFile, "         [1/d]            |   [kg N /ha /a]  |   [kg N /ha /a] ");
    fout_line(hFile, 1);

	fout_string(hFile, THIN_LINE);
    fout_line(hFile, 1);

    fout_string(hFile, "20013");
    fout_line(hFile, 1);     
    
        if (pCh->pCProfile->fNH3VolatR < EPSILON)
	      pCh->pCProfile->fNH3VolatR = (float) 0;
        gcvt((double)pCh->pCProfile->fNH3VolatMaxR,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t\t\t");
    
		gcvt((double)pCh->pCProfile->fCH4ImisR*(double)365,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t\t");
    
		gcvt((double)pCh->pCProfile->fCH4ImisDay*(double)365,4,acDummy2);
        fout_string(hFile,  acDummy2);
        fout_string(hFile,"\t\t\t");

    fout_line(hFile, 1);
    }
	

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    20016
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
     
      fout_string(hFile, "OUTPUT PARAMETER");
      fout_line(hFile, 1);
     
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);
     
     
      fout_string(hFile, "20016");
      fout_line(hFile, 1);     
      
     
      itoa( pGr->iDllGraphicNum,acDummy2,10);
     
      fout_string(hFile,  acDummy2);
      fout_line(hFile, 1);
    }
	
	/*
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	20020
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
	{
      int iloc;

	  fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);

	  fout_string(hFile, "Water Uptake (1=NimahHanks,2=Feddes,3=VanGenuchten) | Parameters");
      fout_line(hFile, 1);

	  fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "20020");
      fout_line(hFile, 1);
    
      switch(iWaterUptakeModel) 
	  {
	    case 1: //Nimah-Hanks
	        fout_int(hFile,&(iWaterUptakeModel), 1);
            fout_string(hFile,"\t");
            fout_line(hFile, 1);
		break;
	
		case 2: //Feddes
	        fout_int(hFile,&(iWaterUptakeModel), 1);
            fout_string(hFile,"\t");
            
			for(iloc=1; iloc<8; iloc++) 
		    {
		      gcvt((double)afRSPar[iloc],8,acDummy2);
              fout_string(hFile,  acDummy2);
              fout_string(hFile," ");
		    }
            
			//SG20150930: Für Irene Witte - Feddes-alpha kann optional = 1 (const.) gesetzt werden
			// --> Schalter muss im rfs-Mode mit ausgegeben werden
			if ((double)afRSPar[0] < 0) 
			{
			gcvt((double)afRSPar[0],8,acDummy2);
              fout_string(hFile,  acDummy2);
              fout_string(hFile," ");
			}

            
			fout_line(hFile, 1);
		break;
	
		case 3: //Van Genuchten
	        fout_int(hFile,&(iWaterUptakeModel), 1);
            fout_string(hFile,"\t");
            
			gcvt((double)fh50Global,4,acDummy2);
            fout_string(hFile,  acDummy2);
            fout_string(hFile,"\t");
            
			gcvt((double)fp1Global,4,acDummy2);
            fout_string(hFile,  acDummy2);
            fout_string(hFile,"\t");
            
			fout_line(hFile, 1);
		break;
	  }
    }

	    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    30010
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    if(iSoilWaterFluxOutput){

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "Output depths leachate water ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Schichttiefen (mm) für Sickerwasserausgabe in ~.lch");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "30010");
	  fout_string(hFile,"\t");
	  fout_int(hFile, &(iNumb_lch), 2);
	  fout_line(hFile, 1);  
    
    
      for (i=1;i<iNumb_lch+1;i++)
      {
		  int iDepth;
        //fout_int(hFile, &(i), 2);
        //fout_string(hFile,"\t");
		iDepth = (int)PrintDepthTab[i];
        fout_int(hFile, &iDepth, 4);

		//DOUT(PrintDepthTab[i])
        //fout_string(hFile,acDummy2);
        //fout_string(hFile,"\t\t");
   
        fout_line(hFile, 1);
      }
    }  
 
    /*
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    30020
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    if(iSoilWaterContentOutput){

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "Output depths soil water content ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Schichttiefen (mm) für Wassergehaltausgabe in ~.wct");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "30020");
	  fout_string(hFile,"\t");
	  fout_int(hFile, &(iNumb_wct), 2);
	  fout_line(hFile, 1);  
    
    
      for (i=1;i<iNumb_wct+1;i++)
      {
		  int iDepth;
        //fout_int(hFile, &(i), 2);
        //fout_string(hFile,"\t");
		iDepth = (int)WCPrintDepthTab[i];
        fout_int(hFile, &iDepth, 4);

		//DOUT(PrintDepthTab[i])
        //fout_string(hFile,acDummy2);
        //fout_string(hFile,"\t\t");
   
        fout_line(hFile, 1);
      }
    }  
 
    /*
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    30030
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    if(iSoilWaterMatricPotOutput){

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "Output depths matric potential ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Schichttiefen (mm) für Wasserpotentialausgabe in ~.mpt");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "30030");
	  fout_string(hFile,"\t");
	  fout_int(hFile, &(iNumb_mpt), 2);
	  fout_line(hFile, 1);  
    
    
      for (i=1;i<iNumb_mpt+1;i++)
      {
		  int iDepth;
        //fout_int(hFile, &(i), 2);
        //fout_string(hFile,"\t");
		iDepth = (int)WPPrintDepthTab[i];
        fout_int(hFile, &iDepth, 4);

		//DOUT(PrintDepthTab[i])
        //fout_string(hFile,acDummy2);
        //fout_string(hFile,"\t\t");
   
        fout_line(hFile, 1);
      }
    }  
 
    /*
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    30040
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    if(iSoilTemperatureOutput){

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "Output depths soil heat ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Schichttiefen (mm) für Bodenwärmeausgabe in ~.hct");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "30040");
	  fout_string(hFile,"\t");
	  fout_int(hFile, &(iNumb_hct), 2);
	  fout_line(hFile, 1);  
    
    
      for (i=1;i<iNumb_hct+1;i++)
      {
		  int iDepth;
        //fout_int(hFile, &(i), 2);
        //fout_string(hFile,"\t");
		iDepth = (int)STPrintDepthTab[i];
        fout_int(hFile, &iDepth, 4);

		//DOUT(PrintDepthTab[i])
        //fout_string(hFile,acDummy2);
        //fout_string(hFile,"\t\t");
   
        fout_line(hFile, 1);
      }
    }  
 
    /*
	>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    30050
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    if(iSoilNitrogenContentOutput){

      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
      fout_string(hFile, "Output depths soil nitrogen ");
      fout_string(hFile, acFileName); 
      fout_line(hFile, 1);
      fout_string(hFile, "Schichttiefen (mm) für Bodenstickstoffausgabe in ~.lch");
      fout_line(hFile, 1);
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);

      fout_string(hFile, "30050");
	  fout_string(hFile,"\t");
	  fout_int(hFile, &(iNumb_nct), 2);
	  fout_line(hFile, 1);  
    
    
      for (i=1;i<iNumb_nct+1;i++)
      {
		  int iDepth;
        //fout_int(hFile, &(i), 2);
        //fout_string(hFile,"\t");
		iDepth = (int)NCPrintDepthTab[i];
        fout_int(hFile, &iDepth, 4);

		//DOUT(PrintDepthTab[i])
        //fout_string(hFile,acDummy2);
        //fout_string(hFile,"\t\t");
   
        fout_line(hFile, 1);
      }
    }  


//	// SG20140710
//	// for running the GECROS model in rfs-mode
//
//	if(iGECROSDefault==0){
//		/* 80000 */
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "GECROS INPUT PARAMETERS (default parameter values)");
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Marker for crop type using default values to parameterize GECROS (iGECROS = 0)");
//		fout_string(hFile, ";80000 iGECROS");
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, " Type	   Name	Variety	Ecotype   Daylength Type    Leaf Angle Type");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Code)  	                  (Code)        (Code)           (Code)");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80000     0");
//	    fout_line(hFile, 1);
//	 // fout_string(hFile, "WH	  Winterweizen    Skalmeje	 COOL		   LDP		  SPH");
//	 // fout_line(hFile, 1);
//
//		strcpy(acDummy2,pPl->pGenotype->acCropCode);
//		fout_string(hFile,  acDummy2);
//		fout_string(hFile,"\t");
//
//		strcpy(acDummy2,pPl->pGenotype->acCropName);
//		fout_string(hFile,  acDummy2);
//		fout_string(hFile,"\t");
//
//		strcpy(acDummy2,pPl->pGenotype->acVarietyName);
//		fout_string(hFile,  acDummy2);
//		fout_string(hFile,"\t");
//
//		strcpy(acDummy2,pPl->pGenotype->acEcoType);
//		fout_string(hFile,  acDummy2);
//		fout_string(hFile,"\t");
//
//		strcpy(acDummy2,pPl->pGenotype->acDaylenType);
//		fout_string(hFile,  acDummy2);
//		fout_string(hFile,"\t");
//
//		strcpy(acDummy2,pPl->pGenotype->acLvAngleType);
//		fout_string(hFile,  acDummy2);
//		fout_string(hFile,"\t");
//
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//	}
//    
//
//	if(fGECROS >= 1){
//
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "GECROS INPUT PARAMETERS (user specified parameter values)");
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Markers for using user specified values to parameterize GECROS");
//	    fout_line(hFile, 1);
//		fout_string(hFile, ";80001 crop type specification");
//	    fout_line(hFile, 1);
//		fout_string(hFile, ";80002 - 80004 crop type specific parameters");
//	    fout_line(hFile, 1);
//		fout_string(hFile, ";80005 - 80006 genotype specific parameters");
//	    fout_line(hFile, 1);
//		fout_string(hFile, ";80007 - 80008 default and sensitivity parameters");
//	    fout_line(hFile, 1);
//		fout_string(hFile, ";80009 - 80010 soil parameters");
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		/* 80001 */
//		fout_string(hFile, "Crop Type");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "legume     C3/C4    determinate    short-/long-day     lodging   vernalisation");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "+1/-1      +1/-1       +1/-1           +1/-1            +1/-1        +1/-1");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80001");
//	    fout_line(hFile, 1);
//
////		fout_string(hFile, "-1		+1		+1		    -1		   -1           +1");
//
//		iLEGUME = (int)fparLEGUME;
//		fout_int(hFile, &(iLEGUME), 2);
//        fout_string(hFile,"\t");
//
//		iC3C4 = (int)fparC3C4;
//		fout_int(hFile, &(iC3C4), 2);
//        fout_string(hFile,"\t");
//
//		iDETER = (int)fparDETER;
//		fout_int(hFile, &(iDETER), 2);
//        fout_string(hFile,"\t");
//
//		iSLP = (int)fparSLP;
//		fout_int(hFile, &(iSLP), 2);
//        fout_string(hFile,"\t");
//
//		iLODGE = (int)fparLODGE;
//		fout_int(hFile, &(iLODGE), 2);
//        fout_string(hFile,"\t");
//
//		iVERN = (int)fparVERN;
//		fout_int(hFile, &(iVERN), 2);
//        fout_string(hFile,"\t");
//
//
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//
//		/* 80002 */			 
//		fout_string(hFile, "Vegetative and Generative Biomass");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, " veg.biom.      veg.biom.    germin.    seed biomass composition fractions");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "growth effic.  carbon frac.  effic.    lipids  lignin  org.acids  minerals");
//	    fout_line(hFile, 1);
//		fout_string(hFile, " [g C g-1 C]    [g C g-1]	[-]	     [-]    [-]       [-]       [-]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80002");
//	    fout_line(hFile, 1);
//					    		   	 
//		gcvt((double)fparYGV,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparCFV,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparEG,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparFFAT,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparFLIG,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparFOAC,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparFMIN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//
//
//		/* 80003 */			 
//		fout_string(hFile, "Morphology and Phenology");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "leaf   stem dry   max.root      temperature response curve for phenology");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "width   weight     depth         base   optimum   ceiling   curvature");
//	    fout_line(hFile, 1);
//		fout_string(hFile, " [m]  [g m-2 m-1]  [cm]          [°C]     [°C]      [°C]      [-]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80003");
//	    fout_line(hFile, 1);
//					    		   	 
//		gcvt((double)fparLWIDTH,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparCDMHT,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparRDMX,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparTBD,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparTOD,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparTCD,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparTSEN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//	    fout_line(hFile, 1);
//
//
//		/* 80004 */			 
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Nitrogen");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  maximal      minimal   minimal   minimal   initial   specific    N fixation");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  N-uptake     root N    stem N    leaf N    leaf N    leaf area     cost");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "[g N m-2 d-1] [g N g-1] [g N g-1] [g N m-2] [g N g-1]  [m2 g-1]    [g C g-1 N]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80004");
//	    fout_line(hFile, 1);
//					    		   	 
//		gcvt((double)fparNUPTX,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparRNCMIN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparSTEMNC,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparSLNMIN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparLNCI,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparSLA0,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparCCFIX,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//	    fout_line(hFile, 1);
//
//
//		/* 80005 */			 
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Photoperiod and Photosynthesis");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "inclination of  Dev.Stage   Dev.Stage  activation  slope   slope   convex.for");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "sun angle for  at start of  at end of   energy of  VCMAX   VJMAX   light resp.");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "photoperiodic   photosens.  photosens.   JMAX       vs.     vs.     electron");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  daylength      phase        phase               leaf N  leaf N   transport");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  [degree]        [-]          [-]     [J mol-1]  [umol s-1 g-1 N]   [-]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80005");
//	    fout_line(hFile, 1);
//					    		   	 
//   		gcvt((double)fparINSP,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparSPSP,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparEPSP,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparEAJMAX,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparXVN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparXJN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparTHETA,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//	    fout_line(hFile, 1);
//
// 
//		/* 80006 */			 
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Genotype Parameters");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  Seed     Seed N     Leaf angle   Max.crop  Min. thermal days of  Photoperiod");
//	    fout_line(hFile, 1);
//		fout_string(hFile, " weight     conc.   frm.horizontal  height   veg. & reprod. phase  sensitivity");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "[g seed-1] [g N g-1]   [degree]       [m]      [d]       [d]          [h-1]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80006");
//	    fout_line(hFile, 1);
//					    		   	 
// 		gcvt((double)fparSEEDW,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparSEEDNC,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparBLD,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparHTMX,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparMTDV,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparMTDR,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparPSEN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//	    fout_line(hFile, 1);
//
//
//		/* 80007 */			 
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Default Parameters Values of rather high uncertainty");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "    Fraction of sigmoid         Dev.Stage of seed number     Critical root");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  curve inflexion in entire      determining period for      weight density");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "height and seed growth period    indeterminate crops");
//	    fout_line(hFile, 1);
//		fout_string(hFile, " [-]        [-]                        [-]               [g m-2 cm-1 depth]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80007");
//	    fout_line(hFile, 1);
//					    		   	 
// 		gcvt((double)fparPMEH,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//        gcvt((double)fparPMES,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//        gcvt((double)fparESDI,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//        gcvt((double)fparWRB,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//	    fout_line(hFile, 1);
//
//
//		/* 80008 */			 
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Parameters for sensitivity-analysis options");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "   Ambient         Factors for change in             Initial fraction in shoot");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  CO2 conc.   radiation  temperature  vapour press.       C         N");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "[umol mol-1]    [-]         [°C]        [-]          [g C g-1 C] [g N g-1 N]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, " Seed N from    Factor N conc. of seed fill   Dev.Stage of fastest transition");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "non-struct. N        initial      final      from initial to final factor");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "     [-]              [-]          [-]             [-]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "80008");
//	    fout_line(hFile, 1);
//					    		   	 
//		gcvt((double)fparCO2A,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//        gcvt((double)fparCOEFR,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparCOEFV,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparCOEFT,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparFCRSH,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//		gcvt((double)fparFNRSH,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//
//	    fout_line(hFile, 1);
//
//		gcvt((double)fparPNPRE,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//		
//		gcvt((double)fparCB,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparCX,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");             
//		
//		gcvt((double)fparTM,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");
//	    fout_line(hFile, 1);
//
//
//		/* 80009 */			 
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Soil parameters I");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Soil     Min.    Plant    Soil    upper   time constant for  Decomp.   Decomp.");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Clay    Water    Water   profile  soil    dynamics of soil    rate      rate");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "frac.  Content  Capacity  depth   depth   temp.    transf.  microbes   humus");
//	    fout_line(hFile, 1);
//		fout_string(hFile, " [%]     [-]     [mm]     [cm]    [cm]     [d]      [d]      [yr-1]    [yr-1]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, ";80009");
//	    fout_line(hFile, 1);
//		
//		gcvt((double)fparCLAY,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparWCMIN,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparWCPWC,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparPRFDPT,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparSD1,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparTCT,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparTCP,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparBIOR,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparHUMR,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//	    fout_line(hFile, 1);
//		
//
//		/* 80010 */			 
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Soil parameters II");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, "Fraction  Ratio decomp. Decomposition rates   Soil total  Initial   Frac. of");
//		fout_line(hFile, 1);
//		fout_string(hFile, "leaf to   to resistant   of plant materials   organic C    value     BIO in");
//	    fout_line(hFile, 1);
//		fout_string(hFile, " litter    plant mat.  decomposable  resistant   TOC      BIO+HUM  initial TOC");
//	    fout_line(hFile, 1);
//		fout_string(hFile, "  [-]        [-]     [g C m-2 d-1][g C m-2 d-1][g C m-2] [g C m-2]    [-]");
//	    fout_line(hFile, 1);
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, ";80010");
//	    fout_line(hFile, 1);
//  		
//		gcvt((double)fparPNLS,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparDRPM,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparDPMR0,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparRPMR0,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparTOC,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparBHC,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//		
//		gcvt((double)fparFBIOC,4,acDummy2);
//        fout_string(hFile,  acDummy2);
//        fout_string(hFile,"\t");          
//	    fout_line(hFile, 1);
//		
//
//		fout_string(hFile, THIN_LINE);
//	    fout_line(hFile, 1);
//		fout_string(hFile, FAT_LINE);
//	    fout_line(hFile, 1);
//
//	}

    fout_string(hFile, FAT_LINE);
    fout_line(hFile, 1);

    fout_string(hFile, "End of SIM-file ");
    fout_string(hFile, acFileName); 
    fout_line(hFile, 1);

    fout_string(hFile, FAT_LINE);
    fout_line(hFile, 1);


    CloseHandle( hFile );

    //Message(1,START_VALUE_WRITTEN_TXT);

    if (iMessageStatus == 2)
    {
      //MessageBox (NULL, END_OF_SIM_TXT, "INFO", MB_OK);
    }
  }  
  
  free(acDummy2);
  free(acFileName);

  return 1;

} /* PrintStart */
  


/*******************************************************************************************
 *
 *  Name     : PrintHydraulics()
 *  Funktion : Gibt  die Werte aus,
 *				die für eine erneute Simulation als Startwerte benutzt werden koennen.
 *
 *              ch 23.7.97
 *
 ******************************************************************************************
 */
int WINAPI PrintHydraulics(EXP_POINTER, int iMessageStatus)
{ 
  DECLARE_COMMON_VAR
  // DECLARE_POINTER  
  PSLAYER      pSL;
  PCLAYER      pCL;
  // PCPROFILE    pCP;
  PCPARAM      pPA;
  PSWATER      pSW;
  PWLAYER      pWL;
  PHLAYER      pHL;
  // PNFERTILIZER pNF;

  HANDLE hFile;

  int i1;           
  double f1, f2, f3, f4;
  char *acFileName;                           
  char *acDummy2; 


  acDummy2 = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.
  acFileName = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.


  if (strlen(pLo->pFieldplot->acName)<2)
  {
    lstrcpyn(acFileName,"expertn",7);
  }
  else
  {
    lstrcpyn(acFileName,pLo->pFieldplot->acName,7);
  }
        
  // test  pTi->pSimTime->lStopDate = 120394;       
  itoa( (int)(pTi->pSimTime->lStopDate % 100),acDummy2,10);
  lstrcat(acFileName,acDummy2);

    
  strcpy(acDummy2,"param\\");
  strcat(acDummy2,acFileName);
  strcat(acDummy2,".hp");
  hFile = OpenF(acDummy2);


  if (hFile == INVALID_HANDLE_VALUE)
  {
    /* something goes wrong! */
    Message(1,FILEOPEN_ERROR_TXT);
  }

  else
  {        
    strcat(acDummy2," opened for start value file.");
    Message(0, acDummy2);


    fout_string(hFile, FAT_LINE);
    fout_line(hFile, 1);     
		
    fout_string(hFile, "EXPERT-N : Start Values, generated by dllCHOutStart.");

    fout_string(hFile, " ");
    _strdate(acDummy2);            
    fout_string(hFile, acDummy2);

    fout_string(hFile, " ");

    _strtime(acDummy2);            
    fout_string(hFile, acDummy2);

    fout_line(hFile, 2);     

    fout_string(hFile, "SimFile: ");
       
    fout_string(hFile, acFileName); 
       
    fout_line(hFile, 1);
       
    fout_string(hFile, "SimDate: ");
    fout_long(hFile, &(pTi->pSimTime->lTimeDate));
    fout_line(hFile, 1);

    fout_string(hFile, "SimTime: ");
    fout_floatNorm(hFile, pTi->pSimTime->fTimeAct, 2, 3);

    fout_line(hFile, 2);     

    /*
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    Hydraulic Functions
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
    */ 
    {
      fout_string(hFile, FAT_LINE);
      fout_line(hFile, 1);
     
      fout_string(hFile, "Hydraulic Functions");
      fout_line(hFile, 1);
     
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);                   
      
      fout_string(hFile, "Layer | Matric Head | Water Content  | Conductivity  | Diff. Wat. Cap.");
      fout_line(hFile, 1);
	  fout_string(hFile, " [1]  |    [mm]     |      [1]       |    [mm/d]     |    [1/mm] ");
      fout_line(hFile, 1);
     
      fout_string(hFile, THIN_LINE);
      fout_line(hFile, 1);                   
     
      for (N_SOIL_LAYERS) 
      {
        for(f1 = (double)0, i1=0; i1<70; i1++)
        {                  
          f2 = WATER_CONTENT(f1);
          f3 = CONDUCTIVITY(f1);
          f4 = CAPACITY(f1);
          
          fout_int(hFile, &(iLayer), 3);
          fout_string(hFile, "\t");
         
          gcvt(f1,4,acDummy2);
          fout_string(hFile,  acDummy2);
          fout_string(hFile, "\t");
          gcvt(f2,4,acDummy2);
          fout_string(hFile,  acDummy2);
          fout_string(hFile, "\t");
          gcvt(f3,4,acDummy2);
          fout_string(hFile,  acDummy2);
          fout_string(hFile, "\t");
          gcvt(f4,4,acDummy2);
          fout_string(hFile,  acDummy2);
          
          fout_line(hFile, 1);
      
          if (i1)
            f1 *= 1.33352143; // 10^(1/8)
          else
            f1 = (double)-1;
        }
        fout_string(hFile, THIN_LINE);
        fout_line(hFile, 1);                   
      }
    }

    CloseHandle( hFile );
    Message(iMessageStatus,HYDRAULIC_PROPERTIES_WRITTEN_TXT);
  }  

  free(acDummy2);
  free(acFileName);

  return 1;

} /* Print Hydraulic Properties */


/*******************************************************************************************
 *
 *  Name     : StartLayerResult()
 *  Funktion : Ausgabe von Wasser, Temperatur und Stickstoffwerten
 *			   fuer benutzerdefinierte Profiltiefen (Eingabe im ~.mod File
 *
 *              ep/gsf 9.3.01
 *
 ******************************************************************************************
 */
int WINAPI PrintLayerResult(EXP_POINTER)
{ 
  DECLARE_COMMON_VAR
  // DECLARE_POINTER  
  // PSLAYER      pSL;
  PCLAYER      pCL;
  // PCPROFILE    pCP;
  // PCPARAM      pPA;
  // PSWATER      pSW;
  PWLAYER      pWL;
  PHLAYER      pHL;
  // PNFERTILIZER pNF;

  //HANDLE hFile;

  //int iStrLength;
  //double f1;
  //char *acFileName;                           
  //char *acDummy2;
  char buffer[20];
  
  int i,j,NN;
  float fz0,fz1,fF0,fF1,fF,fG0,fG1,fG,fH0,fH1,fH,fI0,fI1,fI;
  //float f1;
  /*
  float PrintDepthTab[12];
  float WCPrintDepthTab[12];
  float MPPrintDepthTab[12];
  float STPrintDepthTab[12];
  float NCPrintDepthTab[12];

  int   iSoilWaterFluxOutput;
  int   iSoilWaterContentOutput;
  int   iSoilWaterMatricPotOutput;
  int   iSoilTemperatureOutput;
  int   iSoilNitrogenContentOutput;
  */

  extern int SimStart(PTIME);      
  extern int EndDay(PTIME pz);

  float DeltaZ=pSo->fDeltaZ;

  /*
  acDummy2 = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.
  acFileName      = _MALLOC(sizeof(char)*(MESSAGE_LENGTH+10+2));  // Speicher allokieren.


  if (strlen(pLo->pFieldplot->acName)<2)
  {
    lstrcpyn(acFileName,"expertn",7);
  }
  else
  {
    lstrcpyn(acFileName,pLo->pFieldplot->acName,7);
  }
        
  // test  pTi->pSimTime->lStopDate = 120394;       
  itoa( (int)(pTi->pSimTime->lStopDate % 100),acDummy2,10);
  lstrcat(acFileName,acDummy2);

    
  strcpy(acDummy2,"result\\");
  strcat(acDummy2,acFileName);
  strcat(acDummy2,".rfs");
  hFile = OpenF(acDummy2);

  */
  
  if (SimStart(pTi))
  {
    // Ausgabefile für Sickerwasserflüsse öffnen, Kopf schreiben
    if (iSoilWaterFluxOutput)
    {
      acWFName[0] = 0;
      strcat(acWFName,"result/");
      strcat(acWFName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acWFName,buffer);
      strcat(acWFName,".lch");
      WFFile = fopen( acWFName, "w" );
      for( i=1; !(PrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( WFFile, "%2i",  i );
        fprintf( WFFile, "%s",   ".  output depth = "  );
        fprintf( WFFile, "%6.3f",PrintDepthTab[i]);
        fprintf( WFFile, "%s\n",   "  [mm]"  );
      }
      fclose(WFFile);
    }

    if (iSoilWaterContentOutput)
    {
      acWCName[0] = 0;
      strcat(acWCName,"result/");
      strcat(acWCName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acWCName,buffer);
      strcat(acWCName,".wct");
      WCFile = fopen( acWCName, "w" );
      for( i=1; !(WCPrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( WCFile, "%2i",  i );
        fprintf( WCFile, "%s",   ".  output depth = "  );
        fprintf( WCFile, "%6.3f",WCPrintDepthTab[i]);
        fprintf( WFFile, "%s\n",   "  [mm]"  );
      }
      fclose(WCFile);
    }

    if (iSoilWaterMatricPotOutput)
    {
      acWPName[0] = 0;
      strcat(acWPName,"result/");
      strcat(acWPName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acWPName,buffer);
      strcat(acWPName,".mpt");
      WPFile = fopen( acWPName, "w" );
      for( i=1; !(WPPrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( WPFile, "%2i",  i );
        fprintf( WPFile, "%s",   ".  output depth = "  );
        fprintf( WPFile, "%9.1f",WPPrintDepthTab[i]);
        fprintf( WFFile, "%s\n",   "  [mm]"  );
      }
      fclose(WPFile);
    }

    if (iSoilTemperatureOutput)
    {
      acSTName[0] = 0;
      strcat(acSTName,"result/");
      strcat(acSTName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acSTName,buffer);
      strcat(acSTName,".hct");
      STFile = fopen( acSTName, "w" );
      for( i=1; !(STPrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( STFile, "%2i",  i );
        fprintf( STFile, "%s",   ".  output depth = "  );
        fprintf( STFile, "%6.3f",STPrintDepthTab[i]);
        fprintf( WFFile, "%s\n",   "  [mm]"  );
      }
      fclose(STFile);
    }

    if (iSoilNitrogenContentOutput)
    {
      acNOName[0] = 0;
      strcat(acNOName,"result/");
      strcat(acNOName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acNOName,buffer);
      strcat(acNOName,".no3");
      NOFile = fopen( acNOName, "w" );
      for( i=1; !(NCPrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( NOFile, "%2i",  i );
        fprintf( NOFile, "%s",   ".  output depth = "  );
        fprintf( NOFile, "%6.3f",NCPrintDepthTab[i]);
        fprintf( WFFile, "%s\n",   "  [mm]"  );
      }
      fclose(NOFile);

      acNHName[0] = 0;
      strcat(acNHName,"result/");
      strcat(acNHName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acNHName,buffer);
      strcat(acNHName,".nh4");
      NHFile = fopen( acNHName, "w" );
      for( i=1; !(NCPrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( NHFile, "%2i",  i );
        fprintf( NHFile, "%s",   ".  output depth = "  );
        fprintf( NHFile, "%6.3f",NCPrintDepthTab[i]);
        fprintf( WFFile, "%s\n",   "  [mm]"  );
      }
      fclose(NHFile);

      acDONName[0] = 0;
      strcat(acDONName,"result/");
      strcat(acDONName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acDONName,buffer);
      strcat(acDONName,".don");
      //strcat(acDONName,".son"); // SG 20130917 for AgMIP-Soil
      DONFile = fopen( acDONName, "w" );
      for( i=1; !(NCPrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( DONFile, "%2i",  i );
        fprintf( DONFile, "%s",   ".  output depth = "  );
        fprintf( DONFile, "%6.3f",NCPrintDepthTab[i]);
        fprintf( DONFile, "%s\n",   "  [mm]"  );
      }
      fclose(DONFile);

	  acDOCName[0] = 0;
      strcat(acDOCName,"result/");
      strcat(acDOCName,pLo->pFieldplot->acName);
      _itoa(pTi->pSimTime->lStopDate % 100,buffer,10);
      strcat(acDOCName,buffer);
      strcat(acDOCName,".doc");
      //strcat(acDOCName,".soc"); // SG 20130917 for AgMIP-Soil
      DOCFile = fopen( acDOCName, "w" );
      for( i=1; !(NCPrintDepthTab[i]==(float)-99.0) ; i++)
      {
        fprintf( DOCFile, "%2i",  i );
        fprintf( DOCFile, "%s",   ".  output depth = "  );
        fprintf( DOCFile, "%6.3f",NCPrintDepthTab[i]);
        fprintf( DOCFile, "%s\n",   "  [mm]"  );
      }
      fclose(DOCFile);

    }
  } //Simstart


  //Falls letzter Zeitschritt dieses Tages erreicht, Flux in Datei schreiben
  if (EndDay(pTi) && iSoilWaterFluxOutput)
  {
    WFFile = fopen( acWFName, "a" );
    fprintf( WFFile, "%06i\t", pTi->pSimTime->lTimeDate);

    for( i=1; !(PrintDepthTab[i]==(float)-99.0) ; i++)
    {
      if(PrintDepthTab[i]>=(float)0 && PrintDepthTab[i]<DeltaZ*(pSo->iLayers-1))
      {
        NN = (int)(PrintDepthTab[i]/DeltaZ);
        fz0 = NN*DeltaZ;
        fz1 = (NN+1)*DeltaZ;
        for(pWL = pWa->pWLayer, j=0; j<NN; j++)
          pWL = pWL->pNext;
        fF0 = pWL->fFluxDay;
        fF1 = pWL->pNext->fFluxDay;
        fF  = (fF1*(PrintDepthTab[i]-fz0) + fF0*(fz1-PrintDepthTab[i]))/DeltaZ;
      }
      else
        fF = (float)-99.0;
      fprintf( WFFile, "%6.3f\t", fF);
    }
    fprintf( WFFile, "\n" );
    fclose(WFFile);
  } // end if (EndDay && iSoilWaterFluxOutput)


  if (EndDay(pTi) && iSoilWaterContentOutput)
  {
    WCFile = fopen( acWCName, "a" );
    fprintf( WCFile, "%06i\t", pTi->pSimTime->lTimeDate);

    for( i=1; !(WCPrintDepthTab[i]==(float)-99.0) ; i++)
    {
      if(WCPrintDepthTab[i]>=(float)0 && WCPrintDepthTab[i]<DeltaZ*(pSo->iLayers-1))
      {
        NN = (int)(WCPrintDepthTab[i]/DeltaZ);
        fz0 = NN*DeltaZ;
        fz1 = (NN+1)*DeltaZ;
        for(pWL = pWa->pWLayer, j=0; j<NN; j++)
          pWL = pWL->pNext;
        fF0 = pWL->fContAct;
        fF1 = pWL->pNext->fContAct;
        fF  = (fF1*(WCPrintDepthTab[i]-fz0) + fF0*(fz1-WCPrintDepthTab[i]))/DeltaZ;
      }
      else
      {
        fF = (float)-99.0;
      }
      fprintf( WCFile, "%6.3f\t", fF);
    }
    fprintf( WCFile, "\n" );
    fclose(WCFile);
  } // end if EndDay

  if (EndDay(pTi) && iSoilWaterMatricPotOutput)
  {
    WPFile = fopen( acWPName, "a" );
    fprintf( WPFile, "%06i\t", pTi->pSimTime->lTimeDate);

    for( i=1; !(WPPrintDepthTab[i]==(float)-99.0) ; i++)
    {
      if(WPPrintDepthTab[i]>=(float)0 && WPPrintDepthTab[i]<DeltaZ*(pSo->iLayers-1))
      {
        NN = (int)(WPPrintDepthTab[i]/DeltaZ);
        fz0 = NN*DeltaZ;
        fz1 = (NN+1)*DeltaZ;
        for(pWL = pWa->pWLayer, j=0; j<NN; j++)
          pWL = pWL->pNext;
        fF0 = pWL->fMatPotAct;
        fF1 = pWL->pNext->fMatPotAct;
        fF  = (fF1*(WPPrintDepthTab[i]-fz0) + fF0*(fz1-WPPrintDepthTab[i]))/DeltaZ;
      }
      else
      {
        fF = (float)-99.0;
      }
      fprintf( WPFile, "%8.2f\t", fF);
    }
    fprintf( WPFile, "\n" );
    fclose(WPFile);
  } // end if EndDay

  if (EndDay(pTi) && iSoilTemperatureOutput)
  {
    STFile = fopen( acSTName, "a" );
    fprintf( STFile, "%06i\t", pTi->pSimTime->lTimeDate);

    for( i=1; !(STPrintDepthTab[i]==(float)-99.0) ; i++)
    {
      if(STPrintDepthTab[i]>=(float)0 && STPrintDepthTab[i]<DeltaZ*(pSo->iLayers-1))
      {
        NN = (int)(STPrintDepthTab[i]/DeltaZ);
        fz0 = NN*DeltaZ;
        fz1 = (NN+1)*DeltaZ;
        for(pHL = pHe->pHLayer, j=0; j<NN; j++)
          pHL = pHL->pNext;
        fF0 = pHL->fSoilTemp;
        fF1 = pHL->pNext->fSoilTemp;
        fF  = (fF1*(STPrintDepthTab[i]-fz0) + fF0*(fz1-STPrintDepthTab[i]))/DeltaZ;
      }
      else
      {
        fF = (float)-99.0;
      }
      fprintf( STFile, "%6.2f\t", fF);
    }
    fprintf( STFile, "\n" );
    fclose(STFile);
  } // end if EndDay

  if (EndDay(pTi) && iSoilNitrogenContentOutput)
  {
    NOFile = fopen( acNOName, "a" );
    fprintf( NOFile, "%06i\t", pTi->pSimTime->lTimeDate);
    NHFile = fopen( acNHName, "a" );
    fprintf( NHFile, "%06i\t", pTi->pSimTime->lTimeDate);
    DONFile = fopen( acDONName, "a" );
	fprintf( DONFile, "%06i\t", pTi->pSimTime->lTimeDate);
    DOCFile = fopen( acDOCName, "a" );
	fprintf( DOCFile, "%06i\t", pTi->pSimTime->lTimeDate);

    for( i=1; !(NCPrintDepthTab[i]==(float)-99.0) ; i++)
    {
      if(NCPrintDepthTab[i]>=(float)0 && NCPrintDepthTab[i]<DeltaZ*(pSo->iLayers-1))
      {
        NN = (int)(NCPrintDepthTab[i]/DeltaZ);
        fz0 = NN*DeltaZ;
        fz1 = (NN+1)*DeltaZ;
	for(pCL = pCh->pCLayer, j=0; j<NN; j++)
        	pCL = pCL->pNext;

//SG 20130917: für AgMIP-Soil Ausgabe:
//	for(pCL = pCh->pCLayer, pSL = pSo->pSLayer, j=0; j<NN; j++)
	//	{
        //	pCL = pCL->pNext;
	//		pSL = pSL->pNext;
	//	}


        fF0 = pCL->fNO3NSoilConc;
        fF1 = pCL->pNext->fNO3NSoilConc;
        fF  = (fF1*(NCPrintDepthTab[i]-fz0) + fF0*(fz1-NCPrintDepthTab[i]))/DeltaZ;
        fG0 = pCL->fNH4NSoilConc;
        fG1 = pCL->pNext->fNH4NSoilConc;
        fG  = (fG1*(NCPrintDepthTab[i]-fz0) + fG0*(fz1-NCPrintDepthTab[i]))/DeltaZ;
        fH0 = pCL->fDONSoilConc;
        fH1 = pCL->pNext->fDONSoilConc;
        fH  = (fH1*(NCPrintDepthTab[i]-fz0) + fH0*(fz1-NCPrintDepthTab[i]))/DeltaZ;
        fI0 = pCL->fDOCSoilConc;
        fI1 = pCL->pNext->fDOCSoilConc;
        fI  = (fI1*(NCPrintDepthTab[i]-fz0) + fI0*(fz1-NCPrintDepthTab[i]))/DeltaZ;
		
	//SG 20130917: für AgMIP-Soil Ausgabe von SON und SOC in *.don bzw. *.doc
	//fH0 = (pCL->fNLitter + pSL->fNHumus + pCL->fNManure)/pSL->fThickness/pSL->fBulkDens*1e-2; // [%]
	//fH1 = (pCL->pNext->fNLitter + pSL->pNext->fNHumus + pCL->pNext->fNManure)/pSL->fThickness/pSL->fBulkDens*1e-2; // [%]
        //fH  = (fH1*(NCPrintDepthTab[i]-fz0) + fH0*(fz1-NCPrintDepthTab[i]))/DeltaZ;
        //fI0 = (pCL->fCLitter + pSL->fCHumus + pCL->fCManure)/pSL->fThickness/pSL->fBulkDens*1e-2; // [%]
        //fI1 = (pCL->pNext->fCLitter + pSL->pNext->fCHumus + pCL->pNext->fCManure)/pSL->fThickness/pSL->fBulkDens*1e-2; // [%]
        //fI  = (fI1*(NCPrintDepthTab[i]-fz0) + fI0*(fz1-NCPrintDepthTab[i]))/DeltaZ;
		}
		else
		{
			fF = (float)-99.0;
			fG = (float)-99.0;
			fH = (float)-99.0;
			fI = (float)-99.0;
		}

		fprintf( NOFile, "%6.2f\t", fF);
		fprintf( NHFile, "%6.2f\t", fG);
		fprintf( DONFile, "%6.2f\t", fH);
		fprintf( DOCFile, "%6.2f\t", fI);
	}
	fprintf( NOFile, "\n" );
	fclose(NOFile);
	fprintf( NHFile, "\n" );
	fclose(NHFile);
	fprintf( DONFile, "\n" );
	fclose(DONFile);
	fprintf( DOCFile, "\n" );
	fclose(DOCFile);
 } // end if EndDay

  return 1;
} /* PrintLayerResult */

/*******************************************************************************
** EOF */