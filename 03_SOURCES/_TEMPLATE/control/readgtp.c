//
// Created by Irene on 24.07.2017.
////////////////////////////////////////////////////////////////////////////////////////////////
// read gtp file with SKIP_TO_marker Macros used in readmod
// reads also Gecros parameters (80001 - 90010)
//
//SPASS Genotype Data Read after Enli Wang 22.03.1995
//
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////
//

//#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "defporting.h"
//#include "Alloc.h"
//#include "Readfiles.h"

//Module defined include files
#include "trace.h"
#include "fileio.h"
#include "expbasic.h"
#include "xinclexp.h"
#include "xn_def.h"   
#include "xlanguag.h"     

/*==============================================================================
Static Method Interface Description (OLD VERSION)
==============================================================================*/

//------------------------------------------------------------------------------
static int SkipWhiteSpaces( /*
  Skips white spaces (blanks and tabs) within the transferred data buffer.
  Parameter: */
  char **io_ppcData ); /*
    Pointer to data buffer to be investigated. After the method call the
    pointer points to the first non white space character within the data buffer.
Return Value:
  Count of white spaces which have been skipped.
------------------------------------------------------------------------------*/



int SPASS_Genotype_Read(void);
// extern PPLANT    GetPlantPoi(void); //Irene Witte
extern PPLANT  WINAPI  GetPlantPoi(void);
int Init99InputGECROS(void);


int   iGECROS = (int)-1;
int   iGECROSDefault = (int)-1; //SG20140710: fuer GECROS im rfs-Modus
int   jGECROS = (int)0;
float fGECROS = (float)-99;
float fparLEGUME,fparC3C4,fparDETER,fparSLP,fparLODGE,fparVERN;
float fparEG,fparCFV,fparYGV,fparFFAT,fparFLIG,fparFOAC,fparFMIN;
float fparTBD,fparTOD,fparTCD,fparTSEN,fparLWIDTH,fparRDMX,fparCDMHT;
float fparLNCI,fparCCFIX,fparNUPTX,fparSLA0,fparSLNMIN,fparRNCMIN,fparSTEMNC;
float fparINSP,fparSPSP,fparEPSP,fparEAJMAX,fparXVN,fparXJN,fparTHETA;
float fparSEEDW,fparSEEDNC,fparBLD,fparHTMX,fparMTDV,fparMTDR,fparPSEN;
//SG 20180410: additional parameters for J. Rabe sugarbeet model
float fparSINKBEET,fparEFF,fparCFS; //for sugar beet
float fparPMEH,fparPMES,fparESDI,fparWRB;

float fparCO2A,fparCOEFR,fparCOEFV,fparCOEFT;
float fparFCRSH,fparFNRSH;
float fparPNPRE,fparCB,fparCX,fparTM;

float fparCLAY,fparWCMIN,fparWCPWC,fparPRFDPT,fparSD1,fparTCT,fparTCP;
float fparWCMAX, fparRSS;

float fparBIOR,fparHUMR;
float fparPNLS;
float fparDRPM,fparDPMR0,fparRPMR0,fparTOC,fparBHC,fparFBIOC;


double		DVR[11];
#define SKIP_TO_MARKER(XXX) \
        lByteNo = search_marker_gtp(hpcReadData, XXX, &iLines); \
        if (lByteNo > 0) hpcReadData  += lByteNo; \
        if (lByteNo < 0){        \
        hpcReadData = hpcData; iLines = 0;}  \
        else

#define  GET_INT \
  lByteNo = read_ac_int_gtp(hpcReadData, &i1Wert); \
  if (lByteNo > 0) { \
    hpcReadData += lByteNo; \
  }

#define  GET_FLOAT(x) \
  lByteNo = read_ac_float(hpcReadData, &f1Wert); \
  if (lByteNo > 0){ \
    hpcReadData += lByteNo; \
    x=f1Wert;} \



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
// Ueberpruefungen
// **********************************************************************************


#define GET_TEST_FLOAT(value,limit0,limit1,name) \
  GET_FLOAT(value);\
  f1=(float)limit0;\
  f2=(float)limit1;\
  if ((value < limit0)||(value > limit1)) { \
    char szBuf[100]; \
    sprintf( szBuf, "Val = %e, Min = %e, Max = %e", value, f1, f2 ); \
    value = (float)-99;} \



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  TOOL FUNCTIONS
//------------------------------------------------------------------------------------------------------------int

extern float  AFGENERATOR(float fInput, RESPONSE* pfResp);
extern double  LIMIT(double v1, double v2, double x);

extern int  ReadSymbol(FILE *hFile,LPSTR);
//SG 20111019
extern float  ReadFloatOrNewLine(FILE *hFile);

extern int  ReadDate(FILE *hFile,LPSTR);

extern int  ReadInt(FILE *hFile);
extern long  ReadLong(FILE *hFile);
//extern float  ReadFloat(FILE *hFile);
extern double  ReadDouble(FILE *hFile);

extern int   WINAPI Message(long, LPSTR);


//zusaetzliche Variablen fuer SUCROS:
extern float fBaseTemperature1;
extern float fCriticalTemperatureSum1;
extern float fBaseTemperature2;
extern float fCriticalTemperatureSum2;
extern RESPONSE	PmaxAge[21];

//SG 20110810: CO2 als Input von *.xnm (fuer AgMIP)
extern float fAtmCO2;
extern float fO1;


int SPASS_Genotype_Read(void)
{
    FILE    *hFile=NULL;
    char    acDummy[81];
    char    acDummy2[MAX_FILE_STRINGLEN+10+2];
    int		i=0,iValue=0;
    long 	longCode=(long)0;
    char 	cChar[20];
    long    ulFileSize=(long)0;
    long    bResultReadFile=(long)0;
    char    *hpcData=NULL;
    char    *hpcReadData=NULL;
    char    *hpcstart=NULL;
    int     lByteNo = 0;
    int     iLines = 0;
    int     iNumb=0;
    int     i1Wert=0;
    float   f1Wert=(float)0;
    float   f1=(float)0.0,f2=(float)0.0;


    PPLANT pPl=GetPlantPoi();

    //===================================================================================
    //The genotype file name
    //===================================================================================
    // char	cGenFileName[]="param\\WHEAT.GTP\0";
    char	cGenFileName[30];

    //printf("Genotype name %s\n",pPl->pGenotype->acCropCode );

	strcpy(cGenFileName,"param\\");

    if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))
    {
        //lstrcpy(cGenFileName,Fparam);
        strcat(cGenFileName,"WHEAT.GTP\0");
    }
    else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))
            ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"S")))
        {
            strcat(cGenFileName,"BARLEY.GTP\0");
        }
        else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))
        {
            //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"POTATO.GTP\0");
        }
        else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
        {
        //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"MAIZE.GTP\0");
        }
        else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SF"))
        {
            //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"SUNFLOWER.GTP\0");
        }
        else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RY"))
            ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SR")))
        {
            //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"RYE.GTP\0");
        }
        else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"TC"))
        {
            //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"TRITICALE.GTP\0");
        }
        else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"RP"))
        {
            //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"RAPE.GTP\0");
        }
        else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))
        {
            //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"SUGARBEET.GTP\0");
        }

		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PH"))
			strcat(cGenFileName,"PHACELIA.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"OR"))
			strcat(cGenFileName,"Oilradish.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"CV"))
			strcat(cGenFileName,"Vetch.GTP\0");
		else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SY"))
			strcat(cGenFileName,"SOYBEAN.GTP\0");

        else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WM"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"GC"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"OR"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"AL"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"IR"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BR")))
        {
            //lstrcpy(cGenFileName,Fparam);
            strcat(cGenFileName,"ZWIFRU.GTP\0");
            //lstrcpy(cGenFileName,"param\\ZWIFRU.GTP\0");
        }

        else if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BE"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BW"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SP"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"FW"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"AP"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PI"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"KW"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"OA"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"EW"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"AS"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"HO"))
                //||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SY"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"LI"))
                ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MA")))
            return 1;

        //else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BE"))
        //lstrcpy(cGenFileName,"param\\BEECH.GTP\0");
        //else if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SP"))
        //lstrcpy(cGenFileName,"param\\SPRUCE.GTP\0");

    else
    {
        //lstrcpy(cGenFileName,Fparam);
        strcat(cGenFileName,"PLANT.GTP\0");
    }

    //===================================================================================
    //Read Genotype data
    //===================================================================================

    hFile=fopen(cGenFileName, "r");

    //Display elements
    if ( hFile == INVALID_HANDLE_VALUE )
    {
        printf("Genotype file not found ! File Open Error");
        exit (1);
        return 0;
    }
    else
        fseek(hFile, 0,SEEK_END );
        ulFileSize=ftell(hFile);
        rewind(hFile);

        hpcData = (char *)calloc((ulFileSize+2),sizeof(char));
        if( hpcData == NULL )
        {
            printf("Error: Memory Allocierung fuer %s fehlgeschlagen \n",acDummy2);
            fclose( hFile );
            exit(1);
            return FALSE;

        }
        else
        //    hpcData = (char *)calloc((ulFileSize+2),sizeof(char));
        //    if( hpcData == NULL )
        //    {
        //        printf("Memory Allocierung fuer %s fehlgeschlagen \n",acDummy2);
        //        fclose( hFile );
        //        exit(1);
        ////return FALSE;
        //    }
        //    else
			bResultReadFile=fread(hpcData,1, ulFileSize,hFile);

        if( !bResultReadFile )
        {
            printf("%s Datei NICHT gelesen\n",acDummy2);
            free(hpcData);
            hpcData=NULL;
            exit(1);
            return(FALSE);
        }

	Message(1,"Reading GENOTYPE Data ");

    //hpcReadData = hpcData + ulNoOfBytes;
    hpcReadData=hpcData+ulFileSize;
    *hpcReadData='\0';
    hpcReadData=hpcData;
    hpcstart=hpcData;


    fclose( hFile );
    //=======================================================================
    //	Name and Type
    //=======================================================================
    SKIP_TO_MARKER(2000001)
    {

        GET_INT;
        GET_INT;
        iValue=i1Wert;
        SKIP_LINE;
 //       int y=0;

        if (iValue>0)
        {
            //GET_STRING(cChar);
            GET_STRING(cChar);
            if(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,cChar))
//          Message(0,"Plant error");

            if(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"GL"))
                lstrcpy(pPl->pGenotype->acCropCode,  	cChar);

            if(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,"RY"))
                lstrcpy(pPl->pGenotype->acCropCode,"WH\0");

            //GET_STRING(cChar); // lstrcpy(pPl->pGenotype->acCropName,  	cChar);
            GET_STRING(cChar);
            GET_STRING(cChar);	lstrcpy(pPl->pGenotype->acEcoType,cChar);
            GET_STRING(cChar);	lstrcpy(pPl->pGenotype->acDaylenType,cChar);
            GET_STRING(cChar);	lstrcpy(pPl->pGenotype->acLvAngleType,cChar);
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fPlantHeight=f1Wert;
            // printf("test\n");
        }
    }

    //=======================================================================
    //	Photosynthesis Response
    //=======================================================================
    //Ps parameters
    SKIP_TO_MARKER(2000010) {

        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            //SG 20110907: read different values for different cultivars
            GET_STRING(cChar);
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fMaxGrossPs =f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fLightUseEff =f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fMinRm = f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fCO2CmpPoint = f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fCiCaRatio = f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fPsCurveParam = f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fDaylengthCoeff =f1Wert;
            GET_FLOAT(f1Wert)
            SKIP_LINE;

            for (i = 1; i < iValue; i++) {
                GET_STRING(cChar);

                if (!(lstrcmp((LPSTR) pPl->pGenotype->acVarietyName, cChar)))
                {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fMaxGrossPs = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fLightUseEff = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fMinRm =f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fCO2CmpPoint =f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fCiCaRatio = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fPsCurveParam = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fDaylengthCoeff = f1Wert;
                    SKIP_LINE;
                }
                else {
                     SKIP_LINE;
                   //GET_FLOAT(f1Wert);
                   // GET_FLOAT(f1Wert);
                   // GET_FLOAT(f1Wert);
                   // GET_FLOAT(f1Wert);
                   // GET_FLOAT(f1Wert);
                   // GET_FLOAT(f1Wert);
                   // GET_FLOAT(f1Wert);
                } //end if Variety
            }    //end for
            //printf("test1\n");
        }
    }/*Ende marker 2000010*/
    //----------------------------------------------------------
    //	Photosynthesis Temperature Response
    SKIP_TO_MARKER(2000011)
    {

        GET_INT;
        GET_INT;
        iValue = i1Wert;

        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempMinPs = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempOptPs = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempMaxPs = f1Wert;

        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->PmaxTemp[0].fInput = (float) iValue;
            pPl->pGenotype->PmaxTemp[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PmaxTemp[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PmaxTemp[i].fOutput = f1Wert;
            }
        }
    }/* Ende marker 2000011*/
    //----------------------------------------------------------
    //	Photosynthesis: development stage response

    SKIP_TO_MARKER(2000012) {

        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        PmaxAge[0].fInput = (float) iValue;
        PmaxAge[0].fOutput = (float) iValue;

        if (iValue > 0) {
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                PmaxAge[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                PmaxAge[i].fOutput = f1Wert;
            }
        }
    }/*Ende marker 2000012*/
    //=======================================================================
    //	Maintenance and Growth
    //=======================================================================
    //Maintenance Respiration Coefficients
    SKIP_TO_MARKER(2000020) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0)
        {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaintLeaf = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaintStem = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaintRoot = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaintStorage = f1Wert;
        }
    }/*Ende marker 2000020*/

    //----------------------------------------------------------
    //Growth Respiration CO2 production Factor
    SKIP_TO_MARKER(2000021) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fCO2EffLeaf = f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fCO2EffStem = f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fCO2EffRoot = f1Wert;
            GET_FLOAT(f1Wert)
            pPl->pGenotype->fCO2EffStorage = f1Wert;
        }
    }/*Ende marker 2000021*/
    //----------------------------------------------------------
    //Growth Efficiency
    SKIP_TO_MARKER(2000022) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fGrwEffLeaf = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fGrwEffStem = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fGrwEffRoot = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fGrwEffStorage = f1Wert;
        }
    }/*Ende marker 2000022*/

    //=======================================================================
    //	Phenological Development
    //=======================================================================
    //Development Coefficients
    SKIP_TO_MARKER(2000030) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            GET_STRING(cChar);

            GET_FLOAT(f1Wert);
            pPl->pGenotype->fDevelopConstant1 = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fDevelopConstant2 = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fOptVernDays = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fDaylenSensitivity = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fOptDaylen = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->iVernCoeff = (int) f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fPhyllochronInterval = f1Wert;
            SKIP_LINE;

            for (i = 1; i < iValue; i++) {
                GET_STRING(cChar);
                if (!(lstrcmp((LPSTR) pPl->pGenotype->acVarietyName, cChar))) {

                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fDevelopConstant1 = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fDevelopConstant2 = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fOptVernDays = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fDaylenSensitivity = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fOptDaylen = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->iVernCoeff = (int) f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fPhyllochronInterval = f1Wert;
                    SKIP_LINE;
                } else {
                    SKIP_LINE;
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                } //end if Variety
            }    //end for
            //printf("raus aus for ?\n");
        }    //end if iValue

    }/*End of marker 2000030*/


    //----------------------------------------------------------
    //Development Rate vs Temperature (Vegetative)

    SKIP_TO_MARKER(2000031) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;


        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempMinDevVeg = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempOptDevVeg = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempMaxDevVeg = f1Wert;

        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->DevRateVegTemp[0].fInput = (float) iValue;
            pPl->pGenotype->DevRateVegTemp[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->DevRateVegTemp[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->DevRateVegTemp[i].fOutput = f1Wert;
            }
        }
    }

    //----------------------------------------------------------
    //Development Rate vs Temperature (Reproductive)

    SKIP_TO_MARKER(2000032) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;

            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMinDevRep = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempOptDevRep = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMaxDevRep = f1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->DevRateRepTemp[0].fInput = (float) iValue;
                pPl->pGenotype->DevRateRepTemp[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->DevRateRepTemp[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->DevRateRepTemp[i].fOutput = f1Wert;
                }
            }
        }/*Ende marker 2000032*/
    //----------------------------------------------------------
    //Vernalization Rate vs Temperature
        SKIP_TO_MARKER(2000033) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;

            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMinVern = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempOptVern = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMaxVern = f1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->VernRateTemp[0].fInput = (float) iValue;
                pPl->pGenotype->VernRateTemp[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->VernRateTemp[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->VernRateTemp[i].fOutput = f1Wert;
                }
            }
        }/*Ende marker 2000033*/
    //----------------------------------------------------------
    //Development Rate vs Photoperiod (Vegetative)
        SKIP_TO_MARKER(2000034) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->DevRateVegPhtP[0].fInput = (float) iValue;
                pPl->pGenotype->DevRateVegPhtP[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->DevRateVegPhtP[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->DevRateVegPhtP[i].fOutput = f1Wert;
                }
            }
        } /*Ende marker 2000034*/


    //----------------------------------------------------------
    if((lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))&&(lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB"))) {
            SKIP_TO_MARKER(2000035) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            SKIP_LINE;
                   //Development Rate vs EC-Stadium

                if (iValue > 0) {
                    for (i = 0; i <= 10; i++) {
                        GET_FLOAT(f1Wert);
                        DVR[i] = f1Wert;
                    }
                } else
                    DVR[0] = (float) -1.0;
            }/*Ende marker 2000035*/
        }/*Ende if potato*///end (!="PT")&&(!="BS")

        //----------------------------------------------------------
        //Stage dependend parameters for the CERES - model
    SKIP_TO_MARKER(2000036) {
        SKIP_LINE;

        for (i=0;i<=8;i++) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->pStageParam->afTempMax[i] = f1Wert;
        }
        SKIP_LINE;
        for (i=0;i<=8;i++) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->pStageParam->afTempOpt[i] = f1Wert;
        }
        SKIP_LINE;
        for (i=0;i<=8;i++) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->pStageParam->afTempBase[i] = f1Wert;
        }
        SKIP_LINE;
        for (i=0;i<=8;i++) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->pStageParam->afThermalTime[i] = f1Wert;
        }
        SKIP_LINE;
        for (i=0;i<=8;i++) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->pStageParam->afMaxDevR[i] = f1Wert;
        }
    }/*Ende marker 2000036*/
    //----------------------------------------------------------
    //Development Coefficients for the SUCROS - model
    SKIP_TO_MARKER(2000037) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            GET_STRING(cChar);

            GET_FLOAT(f1Wert);
            fBaseTemperature1 = f1Wert;
            GET_FLOAT(f1Wert);
            fCriticalTemperatureSum1 = f1Wert;
            GET_FLOAT(f1Wert);
            fBaseTemperature2 = f1Wert;
            GET_FLOAT(f1Wert);
            fCriticalTemperatureSum2 = f1Wert;
            SKIP_LINE;

            for (i = 1; i < iValue; i++) {

                GET_STRING(cChar);


                if (!(lstrcmp((LPSTR) pPl->pGenotype->acVarietyName, cChar))) {
                    GET_FLOAT(f1Wert);
                    fBaseTemperature1 = f1Wert;
                    GET_FLOAT(f1Wert);
                    fCriticalTemperatureSum1 = f1Wert;
                    GET_FLOAT(f1Wert);
                    fBaseTemperature2 = f1Wert;
                    GET_FLOAT(f1Wert);
                    fCriticalTemperatureSum2 = f1Wert;
                    SKIP_LINE;
                } else {
                    SKIP_LINE;
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);

                } //end if Variety
            }    //end for
        }    //end if iValue
    }/*Ende skip to marker 2000037*/

    //=======================================================================
    //	Morphology
    //=======================================================================
    SKIP_TO_MARKER(2000040) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            GET_STRING(cChar);
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaxLfInitRate = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaxLfAppRate = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTillerDemand = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fStorageOrganNumCoeff = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fSpecLfWeight = f1Wert;
            SKIP_LINE;

            for (i = 1; i < iValue; i++) {

                GET_STRING(cChar);

                if (!(lstrcmp((LPSTR) pPl->pGenotype->acVarietyName, cChar))) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fMaxLfInitRate = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fMaxLfAppRate = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fTillerDemand = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fStorageOrganNumCoeff = f1Wert;
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->fSpecLfWeight = f1Wert;
                    SKIP_LINE;
                } else {
                    SKIP_LINE;
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                    //GET_FLOAT(f1Wert);
                } //end if Variety
            }    //end for
        }    //end if iValue
    }/* ende marker 2000040*/

    //----------------------------------------------------------
    //Specific leaf weight
    SKIP_TO_MARKER(2000041) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->SpecLfWght[0].fInput = (float) iValue;
            pPl->pGenotype->SpecLfWght[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->SpecLfWght[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->SpecLfWght[i].fOutput = f1Wert;
            }
        }
    }/*ende marker 2000041*/
    //----------------------------------------------------------
    //Leaf appearance rate
    SKIP_TO_MARKER(2000042) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;

        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempMinLfApp = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempOptLfApp = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fTempMaxLfApp = f1Wert;

        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->LfAppRateTemp[0].fInput = (float) iValue;
            pPl->pGenotype->LfAppRateTemp[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->LfAppRateTemp[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->LfAppRateTemp[i].fOutput = f1Wert;
            }
        }
    }/*Ende marker 2000042*/
    //=======================================================================
    //	Carbohydrate Partitioning
    //=======================================================================
    //Root
    SKIP_TO_MARKER(2000050) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->PartRoot[0].fInput = (float) iValue;
            pPl->pGenotype->PartRoot[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartRoot[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartRoot[i].fOutput = f1Wert;
            }
        }
    }/*Ende marker 20000050*/
    //----------------------------------------------------------
    //Leaves
    SKIP_TO_MARKER(2000051) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->PartLeaf[0].fInput = (float) iValue;
            pPl->pGenotype->PartLeaf[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartLeaf[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartLeaf[i].fOutput = f1Wert;
            }
        }
    }
    //----------------------------------------------------------
    //Stems
    SKIP_TO_MARKER(2000052) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->PartStem[0].fInput = (float) iValue;
            pPl->pGenotype->PartStem[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartStem[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartStem[i].fOutput = f1Wert;
            }
        }
    }
    //----------------------------------------------------------
    //Storage organs
    SKIP_TO_MARKER(2000053) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->PartStorage[0].fInput = (float) iValue;
            pPl->pGenotype->PartStorage[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartStorage[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->PartStorage[i].fOutput = f1Wert;
            }
        }
    }

    //=======================================================================
    //	Storage Growth and Reserve Translocation
    //=======================================================================

    if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"PT"))||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SB")))
    {
        //Parameters
        SKIP_TO_MARKER(1000060) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fReserveRatio = f1Wert;
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fRelResvGrwRate = f1Wert;
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fRelStorageFillRate = f1Wert;
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fRelStorageNFillRate = f1Wert;
            }
        }

//	//----------------------------------------------------------
//	//Lag phase for reserve accumulation
            SKIP_TO_MARKER(1000061) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->ReservGrwLag[0].fInput = (float) iValue;
                pPl->pGenotype->ReservGrwLag[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->ReservGrwLag[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->ReservGrwLag[i].fOutput = f1Wert;
                }
            }
        }/*Ende marker 1000061*/

        //----------------------------------------------------------
        //Time constant for reserve translocation
        SKIP_TO_MARKER(1000062) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->ReservTransTc[0].fInput = (float) iValue;
                pPl->pGenotype->ReservTransTc[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->ReservTransTc[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->ReservTransTc[i].fOutput = f1Wert;
                }
            }
        }

        //----------------------------------------------------------
        //Lag phase for storage filling
        SKIP_TO_MARKER(1000063) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            SKIP_LINE;
            if (iValue > 0) {
                pPl->pGenotype->SoFillLag[0].fInput = (float) iValue;
                pPl->pGenotype->SoFillLag[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoFillLag[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoFillLag[i].fOutput = f1Wert;
                }
            }
        }
        //----------------------------------------------------------
        //Storage Fill Rate
        SKIP_TO_MARKER(1000064) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;


            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMinSoFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempOptSoFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMaxSoFill = f1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->SoFillRateTemp[0].fInput = (float) iValue;
                pPl->pGenotype->SoFillRateTemp[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoFillRateTemp[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoFillRateTemp[i].fOutput = f1Wert;
                }
            }
        }

        //----------------------------------------------------------
        //Storage N Fill Rate
        SKIP_TO_MARKER(1000065) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;

            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMinSoNFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempOptSoNFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMaxSoNFill = f1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->SoNFillRateTemp[0].fInput = (float) iValue;
                pPl->pGenotype->SoNFillRateTemp[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoNFillRateTemp[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoNFillRateTemp[i].fOutput = f1Wert;
                }
            }
        }/*Ende marker 10000065*/
    }	//end Potato
    else	//other crops
    {
        //Parameters
    SKIP_TO_MARKER(2000060) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            SKIP_LINE;

            if (iValue > 0) {

                GET_STRING(cChar);
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fReserveRatio = f1Wert;
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fRelResvGrwRate = f1Wert;
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fRelStorageFillRate = f1Wert;
                GET_FLOAT(f1Wert);
                pPl->pGenotype->fRelStorageNFillRate = f1Wert;
                if (!lstrcmp((LPSTR) pPl->pGenotype->acCropCode, (LPSTR) "SF")) {
                    GET_FLOAT(f1Wert);
                    fO1 = f1Wert;
                }
                SKIP_LINE;

                for (i = 1; i < iValue; i++) {

                    GET_STRING(cChar);


                    if (!(lstrcmp((LPSTR) pPl->pGenotype->acVarietyName, cChar))) {
                        GET_FLOAT(f1Wert);
                        pPl->pGenotype->fReserveRatio = f1Wert;
                        GET_FLOAT(f1Wert);
                        pPl->pGenotype->fRelResvGrwRate = f1Wert;
                        GET_FLOAT(f1Wert);
                        pPl->pGenotype->fRelStorageFillRate = f1Wert;
                        GET_FLOAT(f1Wert);
                        pPl->pGenotype->fRelStorageNFillRate = f1Wert;
                        if (!lstrcmp((LPSTR) pPl->pGenotype->acCropCode, (LPSTR) "SF")) {
                            GET_FLOAT(f1Wert);
                            fO1 = f1Wert;
                        }
                        SKIP_LINE;

                    } else {
	                    SKIP_LINE;
                        //GET_FLOAT(f1Wert);
                        //GET_FLOAT(f1Wert);
                        //GET_FLOAT(f1Wert);
                        //GET_FLOAT(f1Wert);
                        //if (!lstrcmp((LPSTR) pPl->pGenotype->acCropCode, (LPSTR) "SF")) {
                        //    GET_FLOAT(f1Wert);
                        //    fO1 = f1Wert;
                        //}
                    } //end if Variety
                }    //end for
            }    //end if iValue
        }// end of marker 2000060

        //----------------------------------------------------------
        //Storage Fill Rate
        SKIP_TO_MARKER(2000061) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;

            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMinSoFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempOptSoFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMaxSoFill = f1Wert;
            SKIP_LINE;

            if (iValue > 0) {
                pPl->pGenotype->SoFillRateTemp[0].fInput = (float) iValue;
                pPl->pGenotype->SoFillRateTemp[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoFillRateTemp[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoFillRateTemp[i].fOutput = f1Wert;
                }
            }
        }
        //----------------------------------------------------------
        //Storage N Fill Rate
        SKIP_TO_MARKER(2000062) {
            GET_INT;
            GET_INT;
            iValue = i1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMinSoNFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempOptSoNFill = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fTempMaxSoNFill = f1Wert;

            SKIP_LINE;
            if (iValue > 0) {
                pPl->pGenotype->SoNFillRateTemp[0].fInput = (float) iValue;
                pPl->pGenotype->SoNFillRateTemp[0].fOutput = (float) iValue;

                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoNFillRateTemp[i].fInput = f1Wert;
                }
                SKIP_LINE;
                for (i = 1; i <= iValue; i++) {
                    GET_FLOAT(f1Wert);
                    pPl->pGenotype->SoNFillRateTemp[i].fOutput = f1Wert;
                }
            }
        }    //end of marker 2000062
    }//end of other crops
    //=======================================================================
    //	TISSUE NITROGEN CONTENT
    //=======================================================================
    //Leaf Opt Nc
    SKIP_TO_MARKER(2000070) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->OptLvNc[0].fInput = (float) iValue;
            pPl->pGenotype->OptLvNc[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->OptLvNc[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->OptLvNc[i].fOutput = f1Wert;
            }
        }
    }
    //----------------------------------------------------------
    //Stem Opt Nc
    SKIP_TO_MARKER(2000071) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->OptStNc[0].fInput = (float) iValue;
            pPl->pGenotype->OptStNc[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->OptStNc[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->OptStNc[i].fOutput = f1Wert;
            }
        }
    }
    //----------------------------------------------------------
    //Root Opt Nc
    SKIP_TO_MARKER(2000072) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->OptRtNc[0].fInput = (float) iValue;
            pPl->pGenotype->OptRtNc[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->OptRtNc[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->OptRtNc[i].fOutput = f1Wert;
            }
        }
    }
    //----------------------------------------------------------
    //Leaf Min Nc
    SKIP_TO_MARKER(2000073) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->MinLvNc[0].fInput = (float) iValue;
            pPl->pGenotype->MinLvNc[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->MinLvNc[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->MinLvNc[i].fOutput = f1Wert;
            }
        }
    }
    //----------------------------------------------------------
    //Stem Min Nc
    SKIP_TO_MARKER(2000074) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->MinStNc[0].fInput = (float) iValue;
            pPl->pGenotype->MinStNc[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->MinStNc[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->MinStNc[i].fOutput = f1Wert;
            }
        }
    }

    //----------------------------------------------------------
    //Root Min Nc
    SKIP_TO_MARKER(2000075) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->MinRtNc[0].fInput = (float) iValue;
            pPl->pGenotype->MinRtNc[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->MinRtNc[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->MinRtNc[i].fOutput = f1Wert;
            }
        }
    }
    //=======================================================================
    //	Root Growth and Activity
    //=======================================================================
    //Parameters
    SKIP_TO_MARKER(2000080) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaxRootExtRate = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fRootLengthRatio = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaxWuptRate = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaxNuptRate = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fMaxRootDepth = f1Wert;
        }
    }
    //----------------------------------------------------------
    //Root Extension Rate vs Temperature
    SKIP_TO_MARKER(2000081) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;

        GET_FLOAT(f1Wert);
        pPl->pGenotype->fRootExtTempMin = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fRootExtTempOpt = f1Wert;
        GET_FLOAT(f1Wert);
        pPl->pGenotype->fRootExtTempMax = f1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->RootExtRateTemp[0].fInput = (float) iValue;
            pPl->pGenotype->RootExtRateTemp[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->RootExtRateTemp[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->RootExtRateTemp[i].fOutput = f1Wert;
            }
        }
    }

    //----------------------------------------------------------
    //Root Extension Rate vs Soil moisture
    SKIP_TO_MARKER(2000082) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;
        if (iValue > 0) {
            pPl->pGenotype->RootExtRateWc[0].fInput = (float) iValue;
            pPl->pGenotype->RootExtRateWc[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->RootExtRateWc[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->RootExtRateWc[i].fOutput = f1Wert;
            }
        }
    }


    //=======================================================================
    //	Senesence
    //=======================================================================
    //Parameters
    SKIP_TO_MARKER(2000090) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fBeginSenesDvs = f1Wert;
            GET_FLOAT(f1Wert);
            pPl->pGenotype->fBeginShadeLAI = f1Wert;
        }
    }

    //----------------------------------------------------------
    //Relative death rate temperature leaves
    SKIP_TO_MARKER(2000091) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->LvDeathRateTemp[0].fInput = (float) iValue;
            pPl->pGenotype->LvDeathRateTemp[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->LvDeathRateTemp[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->LvDeathRateTemp[i].fOutput = f1Wert;

            }
        }
    }
    //----------------------------------------------------------
    //Relative death rate temperature leaves
    SKIP_TO_MARKER(2000092) {
        GET_INT;
        GET_INT;
        iValue = i1Wert;
        SKIP_LINE;

        if (iValue > 0) {
            pPl->pGenotype->RtDeathRateTemp[0].fInput = (float) iValue;
            pPl->pGenotype->RtDeathRateTemp[0].fOutput = (float) iValue;

            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->RtDeathRateTemp[i].fInput = f1Wert;
            }
            SKIP_LINE;
            for (i = 1; i <= iValue; i++) {
                GET_FLOAT(f1Wert);
                pPl->pGenotype->RtDeathRateTemp[i].fOutput = f1Wert;
            }
        }
    }


////////////////////////////////////////////////////////////////////////////////////
//GECROS Input Parameter 80000 - 80010
////////////////////////////////////////////////////////////////////////////////////
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80000
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/


/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80001
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80001)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {

            iGECROS = (int)0;
            fGECROS = (float)1;

            GET_TEST_FLOAT(fparLEGUME , -2, +2, "");
            GET_TEST_FLOAT(fparC3C4   , -2, +2, "");
            GET_TEST_FLOAT(fparDETER  , -2, +2, "");
            GET_TEST_FLOAT(fparSLP    , -2, +2, "");
            GET_TEST_FLOAT(fparLODGE  , -2, +2, "");
            GET_TEST_FLOAT(fparVERN   , -2, +2, "");

        }//if pPl!=NULL

    }//80001

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80002
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80002)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {
            iGECROS = (int)0;
            fGECROS = (float)2;

            GET_TEST_FLOAT(fparYGV  , 0, 1, "");
            GET_TEST_FLOAT(fparCFV  , 0, 1, "");
            GET_TEST_FLOAT(fparEG   , 0, 1, "");
            GET_TEST_FLOAT(fparFFAT , 0, 1, "");
            GET_TEST_FLOAT(fparFLIG , 0, 1, "");
            GET_TEST_FLOAT(fparFOAC , 0, 1, "");
            GET_TEST_FLOAT(fparFMIN , 0, 1, "");

        }//if pPl!=NULL

    }//80002

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80003
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80003)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {
            iGECROS = (int)0;
            fGECROS = (float)3;

            GET_TEST_FLOAT(fparLWIDTH , 0, 1,    "");
            GET_TEST_FLOAT(fparCDMHT  , 0, 1000, "");
            GET_TEST_FLOAT(fparRDMX   , 0, 500,  "");
            GET_TEST_FLOAT(fparTBD    , 0, 100,  "");
            GET_TEST_FLOAT(fparTOD    , 0, 100,  "");
            GET_TEST_FLOAT(fparTCD    , 0, 100,  "");
            GET_TEST_FLOAT(fparTSEN   , 0, 100,  "");

        }//if pPl!=NULL

    }//80003

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80004
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80004)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {
            iGECROS = (int)0;
            fGECROS = (float)4;

            GET_TEST_FLOAT(fparNUPTX  , 0, 10, "");
            GET_TEST_FLOAT(fparRNCMIN , 0, 1,  "");
            GET_TEST_FLOAT(fparSTEMNC , 0, 1,  "");
            GET_TEST_FLOAT(fparSLNMIN , 0, 1,  "");
            GET_TEST_FLOAT(fparLNCI   , 0, 10, "");
            GET_TEST_FLOAT(fparSLA0   , 0, 1,  "");
            GET_TEST_FLOAT(fparCCFIX  , 0, 100,"");

        }//if pPl!=NULL

    }//80004

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80005
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80005)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {
            iGECROS = (int)0;
            fGECROS = (float)5;

            GET_TEST_FLOAT(fparINSP   , -180, 180, "");
            GET_TEST_FLOAT(fparSPSP   , 0, 2,      "");
            GET_TEST_FLOAT(fparEPSP   , 0, 2,      "");
            GET_TEST_FLOAT(fparEAJMAX , 0, 200000, "");
            GET_TEST_FLOAT(fparXVN    , 0, 1000,   "");
            GET_TEST_FLOAT(fparXJN    , 0, 1000,   "");
            GET_TEST_FLOAT(fparTHETA  , 0, 10,     "");

        }//if pPl!=NULL

    }//80005

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80006
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80006)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {
            iGECROS = (int)0;
            fGECROS = (float)6;

            GET_TEST_FLOAT(fparSEEDW , 0, 10, "");
            GET_TEST_FLOAT(fparSEEDNC, 0, 1,  "");
            GET_TEST_FLOAT(fparBLD   , 0, 90, "");
            GET_TEST_FLOAT(fparHTMX  , 0, 10, "");
            GET_TEST_FLOAT(fparMTDV  , 0, 100,"");
            GET_TEST_FLOAT(fparMTDR  , 0, 100,"");
            GET_TEST_FLOAT(fparPSEN  ,-10, 10,"");

        }//if pPl!=NULL

    }//80006

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80007
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80007)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {
            iGECROS = (int)0;
            fGECROS = (float)7;

            GET_TEST_FLOAT(fparPMEH , 0, 1, "");
            GET_TEST_FLOAT(fparPMES , 0, 1, "");
            GET_TEST_FLOAT(fparESDI , 0, 2, "");
            GET_TEST_FLOAT(fparWRB  , 0, 10,"");

        }//if pPl!=NULL

    }//80007


/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80008
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80008)
    {
        GET_INT;
        SKIP_LINE;

        pPl = GetPlantPoi();

        if (pPl!=NULL)
        {
            iGECROS = (int)0;
            fGECROS = (float)8;

            GET_TEST_FLOAT(fparCO2A  , 0, 1000, "");
            GET_TEST_FLOAT(fparCOEFR , 0, 10, "");
            GET_TEST_FLOAT(fparCOEFV , 0, 10, "");
            GET_TEST_FLOAT(fparCOEFT , 0, 10, "");
            GET_TEST_FLOAT(fparFCRSH , 0, 1,  "");
            GET_TEST_FLOAT(fparFNRSH , 0, 1,  "");
            SKIP_LINE;
            GET_TEST_FLOAT(fparPNPRE , 0, 10, "");
            GET_TEST_FLOAT(fparCB    , 0, 10, "");
            GET_TEST_FLOAT(fparCX    , 0, 10, "");
            GET_TEST_FLOAT(fparTM    , 0, 10, "");

        }//if pPl!=NULL

    }//80008

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80009
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80009)
    {
        GET_INT;
        SKIP_LINE;

        iGECROS = (int)0;
        fGECROS = (float)9;

        GET_TEST_FLOAT(fparCLAY  , 0, 100,  "");
        GET_TEST_FLOAT(fparWCMIN , 0, 1,    "");
        GET_TEST_FLOAT(fparWCPWC , 0, 2000, "");
        GET_TEST_FLOAT(fparPRFDPT, 0, 2000, "");
        GET_TEST_FLOAT(fparSD1   , 0, 100,  "");
        GET_TEST_FLOAT(fparTCT   , 0, 10,   "");
        GET_TEST_FLOAT(fparTCP   , 0, 10,   "");
        GET_TEST_FLOAT(fparBIOR  , 0, 10,   "");
        GET_TEST_FLOAT(fparHUMR  , 0, 10,   "");

        if (fparWCPWC >= fparPRFDPT*(float)10)
        {
            printf(" 1 PWC >= ProfileDepth !!! then PWC=ProfileDepth/5 ");
            fparWCPWC = fparPRFDPT*(float)2;
        }

    }//80009

/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80010
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/
    SKIP_TO_MARKER(80010)
    {
        GET_INT;
        SKIP_LINE;

        iGECROS = (int)0;
        fGECROS = (float)10;

        GET_TEST_FLOAT(fparPNLS  , 0, 1,   "");
        GET_TEST_FLOAT(fparDRPM  , 0, 100, "");
        GET_TEST_FLOAT(fparDPMR0 , 0, 100, "");
        GET_TEST_FLOAT(fparRPMR0 , 0, 100, "");
        GET_TEST_FLOAT(fparTOC   , 0, 50000, "");
        GET_TEST_FLOAT(fparBHC   , 0, 50000, "");
        GET_TEST_FLOAT(fparFBIOC , 0, 1,   "");

    } //80010

//SG 20180412: additional parameters for GECROS-SB (J. Rabe sugarbeet model)
/*
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  80011
 >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
*/
SKIP_TO_MARKER(80011)
{
	GET_INT;
	SKIP_LINE;

	pPl = GetPlantPoi();

	if (pPl != NULL)
	{
		iGECROS = (int)0;
		fGECROS = (float)11;

		GET_TEST_FLOAT(fparSINKBEET, 0, 2000, "");
		GET_TEST_FLOAT(fparEFF, 0, 100, "");
		GET_TEST_FLOAT(fparCFS, 0, 1, "");

		
	}//if pPl!=NULL

}//80011




    //fclose( hFile );
    free(hpcstart);
//exit(1);
    return 1;
}

int Init99InputGECROS(void)
{
    jGECROS = (int)0;
//80008
    fparCO2A=(float)-99,fparCOEFR=(float)-99,fparCOEFV=(float)-99,fparCOEFT=(float)-99;
    fparFCRSH=(float)-99,fparFNRSH=(float)-99;
    fparPNPRE=(float)-99,fparCB=(float)-99,fparCX=(float)-99,fparTM=(float)-99;

//80009
    fparCLAY=(float)-99,fparWCMIN=(float)-99,fparWCPWC=(float)-99;
    fparPRFDPT=(float)-99,fparSD1=(float)-99,fparTCT=(float)-99,fparTCP=(float)-99;
    fparBIOR=(float)-99,fparHUMR=(float)-99;

    fparWCMAX=(float)-99, fparRSS=(float)-99;

//80010
    fparPNLS=(float)-99,fparDRPM=(float)-99,fparDPMR0=(float)-99,fparRPMR0=(float)-99;
    fparTOC=(float)-99,fparBHC=(float)-99,fparFBIOC=(float)-99;

    return 1;
}


#define ERROR_MAXLINE  3000

//-----------------------------------------------------------------------------
long search_marker_gtp( char *i_pcData, long i_lMarker, int *i_piLines2 )
//-----------------------------------------------------------------------------
{
  long lVal = 0;
  long lRet = 0;
  long lByteNo = 0;
  
  while( (lVal != i_lMarker) && ((*i_piLines2)++ < ERROR_MAXLINE) )
  {
    lRet += lByteNo;
    lByteNo = (long) read_ac_long_gtp(i_pcData,(long far*) &lVal);
    lByteNo = skip_ac_line(i_pcData,1);
    if (lByteNo < 0) 
    {
      return -1;
    }
    else
    {
      i_pcData += lByteNo;
    }
  }
  if( lVal != i_lMarker )
  {
    return -1;
  }
  return lRet;

} // search_marker


//------------------------------------------------------------------------------
int read_ac_int_gtp( char *hpcRead, int * piWert )
//------------------------------------------------------------------------------
{
  #define MAX_INT_DIGIT_GTP     8
  char aciValue[MAX_INT_DIGIT_GTP];
  int i = 0;
  int j = SkipWhiteSpaces( &hpcRead );

  /* sign  or valid number */
  if( (*hpcRead == '+') || (*hpcRead == '-')
   || ( (*hpcRead >= 48) && (*hpcRead <= 57) ) )
  {
    aciValue[i] = *hpcRead;
    i++;
    j++;
    hpcRead++;

    /* while valid number or max digit count overflow */
    while ((*hpcRead >= 48) && (*hpcRead <=57) && (i < MAX_INT_DIGIT_GTP))
    {
      aciValue[i] = *hpcRead;
      i++;
      j++;
      hpcRead++;
    }

    /* check errors */
    if (*hpcRead < 1)
    {
      MessageBox (NULL,(char * ) " Error in DataArray",(char * ) "read_ac_int", MB_ICONSTOP);;
      i = MAX_INT_DIGIT_GTP;
    }
  } // if sign or valid number

  /* error  if no valid character e.g. \n\r else return <invalid??>; */

  /* convert data */
  if (i<=MAX_INT_DIGIT_GTP)
  {
    aciValue[i] =(char)0;
    *piWert= atoi(aciValue);
  }
  return j;

} // read_ac_int

//------------------------------------------------------------------------------
int read_ac_long_gtp( char *hpcRead, long *plWert )
//------------------------------------------------------------------------------
{
  #define MAX_LONG_DIGIT     11
  #define MAX_INT_DIGIT_GTP  8

  char aclValue[MAX_LONG_DIGIT];
  int i = 0;
  int j = SkipWhiteSpaces( &hpcRead );

  /* sign or valid number */
  if( (*hpcRead == '+') || (*hpcRead == '-')
   || ( (*hpcRead >= 48) && (*hpcRead <=57) ) )
  {
    aclValue[i] = *hpcRead;
    i++;
    j++;
    hpcRead++;

    /* while valid number or max digit count overflow */
    while ((*hpcRead >= 48) && (*hpcRead <=57) && (i < MAX_INT_DIGIT_GTP))
    {
      aclValue[i] = *hpcRead;
      i++;
      j++;
      hpcRead++;
    }

    /* check errors */
    if (*hpcRead < 1)
    {
      MessageBox (NULL,(char * ) " Error in DataArray",(char * ) "read_ac_long_gtp", MB_ICONSTOP);;
      i = MAX_LONG_DIGIT;
    }
  } // if sign or valid number

  /* convert data */
  if (i<=MAX_LONG_DIGIT)
  {
    aclValue[i] =(char)0;
    *plWert= atol(aclValue);
  }
  return j;

} // read_ac_long
  
/*==============================================================================
Static Method Implementation (OLD VERSION)
==============================================================================*/

//------------------------------------------------------------------------------
static int SkipWhiteSpaces( char **io_ppcData )
//------------------------------------------------------------------------------
{
  int j = 0;
  while( (**io_ppcData == ' ') || (**io_ppcData == '\t') )
  {
    (*io_ppcData)++;
    j++;
  }
  return j;

} // SkipWhiteSpaces

/*******************************************************************************
** EOF */

