/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author: 
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *------------------------------------------------------------------------------
 *
 * 
*******************************************************************************/

#include <windows.h>
#include <math.h>
#include  "xinclexp.h"
#include  "gecros.h"

extern PPLANT  WINAPI  GetPlantPoi(void);

//extern from toolslib:
extern double WINAPI _loadds LIMIT(double v1, double v2, double x);
extern double WINAPI _loadds NOTNUL(double x);
extern double WINAPI _loadds INSW(double x, double y1, double y2);
extern double WINAPI _loadds REAAND(double x, double y);
extern double WINAPI _loadds REANOR(double x, double y);
extern double abspowerDBL(double, double);

//extern from miner.c
extern int SoilNitrogen_GECROS(EXP_POINTER);
//extern from ceres.c
extern float PlantTemperature(EXP_POINTER);
extern float Vernalization_CERES(EXP_POINTER);
//extern from spass.c
extern float WINAPI _loadds DailyVernalization_SPASS(float fTempMin,float fTempMax,PRESPONSE pResp,
													 float fMinTemp,float fOptTemp,float fMaxTemp);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Export Funtions developed based on GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//int   WINAPI _loadds Main_GECROS(EXP_POINTER);
int   WINAPI _loadds Init_GECROS(EXP_POINTER);
int   WINAPI _loadds PhasicDevelopment_GECROS(EXP_POINTER);
int   WINAPI _loadds Photosynthesis_GECROS(EXP_POINTER);
int   WINAPI _loadds BiomassGrowth_GECROS(EXP_POINTER);
int   WINAPI _loadds CanopyFormation_GECROS(EXP_POINTER);
int   WINAPI _loadds RootSystemFormation_GECROS(EXP_POINTER);
int   WINAPI _loadds OrganSenescence_GECROS(EXP_POINTER);
int   WINAPI _loadds PotentialNitrogenUptake_GECROS(EXP_POINTER);
int   WINAPI _loadds ActualNitrogenUptake_GECROS(EXP_POINTER);
int   WINAPI _loadds NitrogenFixation_GECROS(EXP_POINTER);
int   PlantParametersGECROS(LPSTR,LPSTR);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      External Funtions 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
extern float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Internal Funtions 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int    Astronomy_GECROS(int iJulianDay, double vLatitude);//, double INSP);
double DailyThermalDayUnit_GECROS(double DS,double TMAX,double TMIN,double DIF,double DAYL,
								  double TBD,double TOD,double TCD,double TSEN);
double Phenology_GECROS(double DS,double SLP,double DDLP,double SPSP,double EPSP,double PSEN,
                        double MTDV,double MTDR,double TDU, double OptVernDays);

double DailyCanopyGrossPhotosynthesis_GECROS(double SC,double SINLD,double COSLD,double DAYL,double DSINBE,
            double DDTR,double TMAX,double TMIN,double DVP,double WNM,double C3C4,double LAI,double TLAI,
            double HT,double LWIDTH,double RD,double SD1,double RSS,double BLD,double NLV,double TNLV,
            double SLNMIN,double DWSUP,double CO2A,double LS,double EAJMAX,double XVN,double XJN,double THETA,
            double WCUL,double FVPD,double *PPCAN,double *APCANS,double *APCANN,double *APCAN,double *PTCAN,
            double *ATCAN,double *PESOIL,double *AESOIL,double *DIFS,double *DIFSU,double *DIFSH,double *DAPAR);

double kdiff(double vLAI,double BL,double SCP);
double kbeam(double SINB,double BL);
double refl(double SCP,double KB);
double ligab(double SCP,double KB,double KBP,double KDP,double PCB,double PCD, 
             double IB0,double ID0,double LAI,int IW);
double pan(double SLNT,double SLNMIN,double LAI,double KN,double KB,int IW);
double difla(double NRADC,double PT,double RBH,double RT);

int    PotentialLeafPhotosynthesis_GECROS(double FRAC,double DAYTMP,double DVP,double CO2A,double C3C4,
                                          double FVPD,double PAR,double NP,double RBW,double RBH,double RT,
                                          double ATRJ,double ATMTR,double EAJMAX,double XVN,double XJN,double THETA, 
                                          double *PLF,double *PT,double *RSW,double *NRADC,double *SLOPEL);

double PotentialEvaporation_GECROS(double DAYTMP,double DVP,double RSS,double RTS,double RBWS,double RBHS,
                                   double ATRJS,double ATMTR,double PT1,double WSUP1, double *NRADS);

double PotEvaporTransp_GECROS(double RSW,double RT,double RBW,double RBH,double ATRJ,double ATMTR,
                              double FRAC,double TLEAF,double DVP,double SLOPE,double VPD);

double NetLeafAbsRad(double ATRJ,double ATMTR,double FRAC,double TLEAF,double DVP);

double internalCO2(double TLEAF,double DVP,double FVPD,double CO2A,double C3C4);

double gcrsw(double PLEAF,double RDLEAF,double TLEAF,double CO2A,double CO2I,double RBW,double RT);

double photo(double C3C4,double PAR,double TLEAF,double CO2I,double NP,
             double EAJMAX,double XVN,double XJN,double THETA);

double darkr(double TLEAF,double NP,double XVN);

int    ActualLeafPhotosynthesis_GECROS(double DAYTMP,double PAR,double DVP,double CO2A,double C3C4,
                                       double FVPD,double NRADC,double AT,double PT,double RT,
                                       double RBH,double RBW,double RSW,double SLOPEL,double NP,
                                       double NPN,double EAJMAX,double XVN,double XJN,double THETA, 
                                       double *PLFAS,double *PLFAN,double *ADIF);

double betaf(double DVR, double TE, double TX, double TI);

int    sinkg(double DS, double SSG,double TOTC,double YG,double FD,double DCDR,double DCS,double DELT,
                 double *DCDC,double *DCD,double *FLWC);

int    rnacc(double FNSH,double NUPT,double RWST,double STEMNC,double LNCMIN,double RNCMIN,
                 double LNC,double RNC,double NLV,double NRT,double WLV,double WRT,double DELT,
                 double CB,double CX,double TM,double DS,double SEEDNC,double RWSO,double LNLV,
                 double LNRT,double *RNRT,double *RNST,double *RNLV,double *RTNLV,double *RNSO);

int WINAPI _loadds LeafAreaGrowth_GECROS(EXP_POINTER);
int WINAPI _loadds PlantHeightGrowth_GECROS(EXP_POINTER);
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Variable definition
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double PPCAN,APCANS,APCANN,APCAN,PTCAN,ATCAN,PESOIL,AESOIL,DIFS,DIFSU,DIFSH,DAPAR;
static double SC,DS0,SINLD,COSLD,DAYL,DDLP,DSINB,DSINBE;//Astronomy
//double FPE,FNRADS,NRADS,PE;//Evapotranspiration
//double DIFS; 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Model Coefficients/MODEL INPUTS 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//double SCV   =      0.20;  //Scattering coefficient of leaves for visible radiation (PAR)
//double AMX   =     40.0;   //Assimilation rate at light saturation (kg CO2/ ha leaf/h )
//double EFF   =      0.45;  //Initial light use efficiency (kg CO2/J/ha/h m2 s )
//double KDIF  =      0.60;  //Extinction coefficient for diffuse light 
//double k     =      0.50;  //The average extinction coefficient for visible and near infrared radiation

///*
//%%% Crop parameters for pea (Pisum sativum L.) %%%
double LEGUME = -1.; // LEGUME = 1. for leguminous crops;   = -1. for non-leguminous crops.
double C3C4   = 1.; // C3C4   = 1. for C3 crops;           = -1. for C4 crops.
double DETER  = 1.;// DETER  = 1. for determinate crops;  = -1. for indeterminate crops.
double SLP    = -1.;// SLP    = 1. for short-day crops;    = -1. for long-day crops.
double LODGE  = 1.; // LODGE  = 1. for cases with lodging; = -1. for cases without lodging.
double VERN   =-1;  // VERN = 1. vernalization model SPASS; = 0. model CERES; =-1. without vernalization.

double EG     =0.35;// efficiency germination (Par.)                (-)       crop  table 2, p46
double CFV    =0.48;//carbon frac. in vegetative organs (Par.)   (g C g-1)   crop  table 2, p46
                     //pGPltC->fCFracVegetative=(float)CFV;;
double YGV    =0.81;//growth efficiency of veg. organs (Par.)    (g C g-1 C)  crop  table 2, p46
                    //pGPar->fGrwEffVegOrg=(float)YGV;
double FFAT   =0.02;//fat frac.          in storage organ (Par.)   (g g-1)    crop  table 2, p46 
double FLIG   =0.06;//lignin frac.       in storage organ (Par.)   (g g-1)    crop  table 2, p46
double FOAC   =0.04;//organic acid frac. in storage organ (Par.)   (g g-1)    crop  table 2, p46
double FMIN   =0.03;//minerals frac.     in storage organ (Par.)   (g g-1)    crop  table 2, p46
double LNCI   =0.055;//initial N conc. in living leaves             (g N g-1)  crop  table 3, p47
double TBD    = 0.0;//base temperature for phenology               (°C)       crop  table 3, p47 
double TOD    = 27.6;//optimum temperature for phenology           (°C)       crop  table 3, p47
double TCD    = 36.0;//ceiling temperature for phenology           (°C)       crop  table 3, p47
double TSEN   = 0.409;//curvature for temperature response         (-)        deflt table 4, p49
double SPSP   = 0.2; //Dev.stage start of photo-sensitive phase    (-)        deflt table 4, p49
double EPSP   = 0.7; //Dev.stage end of photo-sensitive phase      (-)        deflt table 4, p49
double INSP   = -2.0;//Incl. of sun angle for photoper.daylgth.  (degree)     const table 1, p45
double LWIDTH = 0.025;//Leaf width                                  (m)        crop  table 3, p47
double RDMX   = 100;//maximal root depth                           (cm)       crop  table 3, p47
                    // -> pGPar->fRootDepthMax
double CDMHT=345.;//stem dry weight per unit of plant height    (g m-2 m-1)   crop  table 3, p47
double PMEH=0.8;//sigm.crv.inflec.frac. in ent. plthght.grwth.per. (-)        deflt table 4, p49   
double PMES=0.5;//sigm.crv.inflec.frac. in entire seed grwth.per.  (-)        deflt table 4, p49
double ESDI=1.35;//ESD for indeterminate crops                     (-)        deflt table 4, p49
       //-> ESD = INSW(DETER, ESDI, 1.); = (double)pGPPar->fStageEndSeedNum; 
       //ESD development stage for end of seed number determining period
double CCFIX=6.;//carbon cost of symbiotic nitrogen fixation    (g C g-1 N)   const table 1, p45
                //-> pGPltC->fCNFixCost;
double NUPTX=0.65;//maximum crop nitrogen uptake                (g N m-2 d-1) deflt table 4, p49
                  //-> pGPltN->fNUptMax;
double SLA0 = 0.022;//specific leaf area constant               (m2 leaf g-1) crop  table 3, p47
double SLNMIN = 0.5;//min. specific leaf N content              (g N m-2)     crop  table 3, p47
double RNCMIN = 0.005;//min. N conc. in roots                   (g N g-1)     crop  table 3, p47
                      //-> pPltN->fRootMinConc;
double STEMNC = 0.008;//nitrogen conc. in stems                 (g N g-1)     crop  table 3, p47
                      //-> pPltN->fStemActConc;
double WRB = 0.25;//critical root weight density           (g m-2 cm-1 depth) const table 1, p45 (others)
double EAJMAX = 48041.88;//activation energy of JMAX            (J mol-1)      crop  table 3, p47
double XVN = 62.;//slope of lin.rel. btw. VCMAX and leaf N   (umol s-1 g-1 N) crop specific ???     
double XJN = 124.;//slope of lin.rel. btw. VJMAX and leaf N  (umol s-1 g-1 N) crop specific ???
double THETA = 0.7;//convexity for light response electron 
                   //transport (J2) in photosynthesis              (-)        const table 1, p45 (photo)

//%*********genotype specific parameters for cv. SOLARA ***********************************************
double SEEDW=0.215;//Seed weight                               (g seed-1)   
double SEEDNC=0.05;//Standard seed (storage organ) N conc.    (g N g-1)
double BLD=50.;//Leaf angle from horizontal                      (degree)
double HTMX=0.705;//Maximal crop height                           (m)
double MTDV =34.76;//Minimum thermal days for vegetative phase   (d) 
double MTDR =23.09;//Minimum thermal days for reproductive phase (d) 
double PSEN = -0.0;//Photoperiod sensitivity (+ for SD, - for LD) (h-1)

//%** Soil parameters
//double PNLS=1.
//double CLAY=23.4; WCMIN=0.05; WCFC=0.25; WCMAX=0.35
//PARAM DRPM=1.44; DPMR0=10.; RPMR0=0.3; BIOR=0.66; HUMR=0.02
//PARAM TOC=7193.; BHC=3500.; FBIOC=0.03; RN=1.; RA=1.
double RSS = 100.;//soil resistance, equiv.to stomatal resistance [s m-1] 
double SD1 = 25.;  //thickness of upper soil layer [cm] //TCT=4.; TCP=1.; MULTF=1.
double TCP = 1.;
 
//%** Sensitivity-analysis options
double CO2A = 350., COEFR = 1., COEFV = 1., COEFT = 5.;
double FCRSH = 0.5, FNRSH = 0.63;
double PNPRE = 0.7, CB = 0.75, CX = 1., TM = 1.5;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//*/
extern int   iGECROS;
extern float fGECROS;
extern float fparLEGUME,fparC3C4,fparDETER,fparSLP,fparLODGE,fparVERN;
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

 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

extern float fPotTraDay, fActTraDay;
int iDevStageOld=0;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      BiomassGrowth_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds BiomassGrowth_GECROS(EXP_POINTER)
      {
      PWLAYER      pWL   = pWa->pWLayer;

      PPLTCARBON   pPltC = pPl->pPltCarbon;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PGENOTYPE    pGen  = pPl->pGenotype;
      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
      PCANOPY      pCan  = pPl->pCanopy;

      PGECROSBIOMASS    pGPltB = pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON     pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN   pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY     pGCan  = pGecrosPlant->pGecrosCanopy;
      PGECROSPARAMETER  pGPar  = pGecrosPlant->pGecrosParameter;
	  PGECROSSOIL       pGS    = pGecrosPlant->pGecrosSoil;
      //float            fTemp;
      
	  double CFO,YGO,LNCMIN;
      /*
      double FPRO,FCAR;
      double CLVI,CRTI,NPL,NLVI,NRTI;
      double LAII,SLNBI,RDI,HTI;
      */
      double DS,TDU,CTDU;
      double RCLV,ASSA,FCSH,FCLV,LCLV;//,YGV;
      double RCSST,FCSST;
      double RCSRT,FCRVR,LCRT;
      double RCSO,FCSO,CREMS,CREMR;
      double CLV,CLVD,CLVDS,LVDS,CSST,CSO,CSRT,CRTD,CSH,CRT,CTOT;
      double HI,WLV,WST,WSO,WRT,WSH,WSHH,WLVD,WRTD,WTOT;//,CFV;
      double RWST,RWSO,RWLV,RWRT;
      double CRVS,CRVR,RCRVS,RCRVR;

      double FNSH,NUPT,LNC,RNC,NLV,NRT,DELT;
      //double CB,CX,TM,STEMNC;
      double LNLV,LNRT;
      double RNRT,RNST,RNLV,RTNLV,RNSO;
      double NST,NSO,TNLV,NLVD,NRTD,NSH;
      double NSHH,NTOT;//,CFV;
      double HNC,PNC,ONC;
      double PSO;
      double KW,KN,NBK,TLAI;
      //double DXT,SLN,SLNT,SLNB,SLNNT,LAI;
      double SLN,SLNT,SLNNT,SLNBC,LAI;
      double NCR,NDEMA,DERI,SHSA;//,YGV,NUPTX;

      double DCSS,DCSR,ESD;//locals
      double LAIC,LAIN,FLWCS,FLWCT;//input
      double CSRTN;//input
      double DCDS;//input & output
      double FCRVS;//locals
      double GAP,CREMSI,CREMRI,RNRES,NRES;//locals
      double NREOE,NREOF;//input & output
      double RNREOE,RNREOF;//locals
      double TSN,TSW;//output
      double DVR,DCDSC,DCDSR,DCDTR;
      double RDCDSR,FDS,RDCDTR,IFSH,FDH,DCDT,DCDTC,DCST;//,YGV;
      
      double LWLVM,LWLV;
      double KCRN,LWRT;

      double RMUN,RMUA,RMUS,RMLD,RMUL,RRMUL,RMRE,RM,RX,RG,RESTOT;
      //double CCFIX;//CFO;
      double NUPTN,NUPTA;//,YGV;
      double NFIX;

      //double RLAI;//,CFV;
      double RSLNB,SLNB;

      //double NDEM,NDEMP,RNDEMP,NSUP,NSUPP,RNSUPP,NFIXE,NFIXD,NFIXT,NFIXR,RNFIXR;//TCP;
	  double NDEM,NDEMP,RNDEMP;
	  //double NDEM;
	  double NSUPP,RNSUPP;

      double APCAN=(double)0;
      double DIFS=(double)0;
      /*
      int       iJulianDay;
      double    dLatitude;
      
	  double      DVS[]={0.0, 0.4, 0.55, 0.656, 0.91, 1.00, 1.15, 1.50, 1.95, 2.0};
      float       VR[10];
      int i;
      */
      double RMN;//,YGV;
      //double NDEMD,NDEMAD,HNCCR,SHSAN;
      //double FWS,NAUL,NNUL,NSUPA,NSUPN;
      //double RA,RD,RN,WCFC,WCMIN,WUL;

      //double HT,RHT;
	  double DCDTP,RDCDTP;
      //double KR,RD,RRD;

      /*
	  int iLayer;
      float DeltaZ=pSo->fDeltaZ;
	  float f1;
	  double DWSUP,WCMIN,WUL,WLL;
      */
	  //double WCMIN;
      double TMIN,TMAX,DAVTMP,NAVTMP,TAVSS;

      DELT = (double)1;
/*	  
      if ((pMa->pSowInfo != NULL)&&(pTi->pSimTime->fTimeAct == (float)pMa->pSowInfo->iDay)) 
        Init_GECROS(exp_p);

//-------------------------------------------------------------------------------------

 
    //%-- Photoperiod, solar constant and daily extraterrestrial radiation
      iJulianDay  = (int)pTi->pSimTime->iJulianDay;
      dLatitude   = (double)pLo->pFarm->fLatitude;
      
      Astronomy_GECROS(iJulianDay, dLatitude);

//-------------------------------------------------------------------------------------

      DS   = (double)pDev->fStageSUCROS;
      TMAX = (double)pCl->pWeather->fTempMax;    //[°C]
      TMIN = (double)pCl->pWeather->fTempMin;    //[°C]
	  DIFS = (double)pGS->fDiffSoilAirTemp;       //[°C]
      TBD  = (double)fparTBD;//base temperature for phenology     [°C]  crop  table 3, p47 
      TOD  = (double)fparTOD;//optimum temperature for phenology  [°C]  crop  table 3, p47
      TCD  = (double)fparTCD;//ceiling temperature for phenology  [°C]  crop  table 3, p47
      TSEN = (double)fparTSEN;//curvature for temperature response [-]  deflt table 4, p49

	  //%-- Developmental stage (DS) & cumulative thermal units (CTDU)      
      TDU =(double)0;
      TDU=DailyThermalDayUnit_GECROS(DS,TMAX,TMIN,DIFS,DAYL,TBD,TOD,TCD,TSEN);

	  DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU);
      pDev->fDevR = (float)DVR;//rate

      
    //%--  Phenological development
      if (pDev->fStageSUCROS<0)
      {
      //initially zero
       pDev->fStageSUCROS= (float)0.0;
       pDev->iDayAftEmerg = 0;
       pDev->fCumTDU = (float)0.0;

       CTDU =(double)0;
       DS =(double)0;
      }
      else
      {
       DS = (double)pDev->fStageSUCROS;   
       //DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU);
       
	  //output
       //pDev->fDevR = (float)DVR;
       //pDev->fStageSUCROS += pDev->fDevR;
       //pDev->fCumTDU += (float)TDU;

	  //output from fStageSUCROS to pDev->fStageWang
      for (i=0;i<10;i++) VR[i]=(float)DVS[i];
      if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<=VR[1]))
            pDev->fDevStage=(float)(10.0*(1.0+(pDev->fStageSUCROS-VR[0])/(VR[1]-VR[0])));
      if ((pDev->fStageSUCROS>VR[1])&&(pDev->fStageSUCROS<=VR[2]))
            pDev->fDevStage=(float)(10.0*(2.0+(pDev->fStageSUCROS-VR[1])/(VR[2]-VR[1])));
      if ((pDev->fStageSUCROS>VR[2])&&(pDev->fStageSUCROS<=VR[3]))
            pDev->fDevStage=(float)(10.0*(3.0+(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2])));
      if ((pDev->fStageSUCROS>VR[3])&&(pDev->fStageSUCROS<=VR[4]))
            pDev->fDevStage=(float)(10.0*(4.0+(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3])));
      if ((pDev->fStageSUCROS>VR[4])&&(pDev->fStageSUCROS<=VR[5]))
            pDev->fDevStage=(float)(10.0*(5.0+(pDev->fStageSUCROS-VR[4])/(VR[5]-VR[4])));
      if ((pDev->fStageSUCROS>VR[5])&&(pDev->fStageSUCROS<=VR[6]))
            pDev->fDevStage=(float)(10.0*(6.0+(pDev->fStageSUCROS-VR[5])/(VR[6]-VR[5])));
      if ((pDev->fStageSUCROS>VR[6])&&(pDev->fStageSUCROS<=VR[7]))
            pDev->fDevStage=(float)(10.0*(7.0+(pDev->fStageSUCROS-VR[6])/(VR[7]-VR[6])));
      if ((pDev->fStageSUCROS>VR[7])&&(pDev->fStageSUCROS<=VR[8]))
            pDev->fDevStage=(float)(10.0*(8.0+(pDev->fStageSUCROS-VR[7])/(VR[8]-VR[7])));
      if ((pDev->fStageSUCROS>VR[8])&&(pDev->fStageSUCROS<=VR[9]))
            pDev->fDevStage=(float)(10.0*(9.0+0.2*(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8])));
      if (pDev->fStageSUCROS>VR[9])
            pDev->fDevStage=(float)92.0; 

     }// if (pDev->fStageSUCROS>=0) else
*/
//-------------------------------------------------------------------------------------
      //%-- Canopy photosynthesis, transpiration and soil evaporation
      //Photosynthesis_GECROS(exp_p);

//-------------------------------------------------------------------------------------
//*/

	  DS   = (double)pDev->fStageSUCROS;
      DVR  = (double)pDev->fDevR;//rate
	  TDU  = (double)pDev->fDTT;
	  CTDU = (double)pDev->fCumTDU;

      TMAX = (double)pCl->pWeather->fTempMax;    //[°C]
      TMIN = (double)pCl->pWeather->fTempMin;    //[°C]
	  DIFS = (double)pGS->fDiffSoilAirTemp;       //[°C]
      TBD  = (double)fparTBD;//base temperature for phenology     [°C]  crop  table 3, p47 
      TOD  = (double)fparTOD;//optimum temperature for phenology  [°C]  crop  table 3, p47
      TCD  = (double)fparTCD;//ceiling temperature for phenology  [°C]  crop  table 3, p47
      TSEN = (double)fparTSEN;//curvature for temperature response [-]  deflt table 4, p49

	  //%-- Developmental stage (DS) & cumulative thermal units (CTDU)      
      //input
      CLV    = (double)pGPltC->fCLeaf;
      CLVD   = (double)pGPltC->fCLeafDead;
      CLVDS  = (double)pGPltC->fCLeafDeadSoil;
      CSST   = (double)pGPltC->fCStem;
      CSO    = (double)pGPltC->fCStorage;
      CSRT   = (double)pGPltC->fCStrctRoot;
      CRTD   = (double)pGPltC->fCRootDead;
        
      //CFV    = (double)pGPltC->fCFracVegetative;
      CFO    = (double)pGPltC->fCFracStorage;
      CRVS   = (double)pGPltC->fCStemReserve;
      CRVR   = (double)pGPltC->fCRootReserve;
      
      //%** Biomass formation
      WLV    = CLV  / CFV;
      WST    = CSST / CFV + CRVS/0.444;
      WSO    = CSO  / CFO;
      WSH    = WLV  + WST + WSO;
      WRT    = CSRT / CFV + CRVR/0.444;
      WTOT   = WSH  + WRT;

      WLVD   = CLVD / CFV;
      WRTD   = CRTD / CFV;
      WSHH   = WSH  + (WLVD-CLVDS/CFV);
      HI     = WSO  / WSHH;
 
      CSH    = CLV  + CSST + CRVS + CSO;
      CRT    = CSRT + CRVR;
      CTOT   = CSH  + CRT;

      //output
      pPltB->fLeafWeight      = (float)(WLV*10); //[g m-2] --> [kg ha-1]
      pPltB->fStemWeight      = (float)(WST*10); //[g m-2] --> [kg ha-1]
      pGPltB->fStorageWeight  = (float)WSO;
      pPltB->fRootWeight      = (float)(WRT*10); //[g m-2] --> [kg ha-1]
      pGPltB->fShootWeight    = (float)WSH;
      pGPltB->fShtWghtExShLvs = (float)WSHH;
      pPltB->fTotalBiomass    = (float)(WTOT*10);//[g m-2] --> [kg ha-1]
      pGPltB->fHarvestIndex   = (float)HI;

      pPltB->fGrainWeight     = (float)(WSO*10); //[g m-2] --> [kg ha-1]
      pPltB->fStovWeight      = pPltB->fLeafWeight+pPltB->fStemWeight;

      pGPltB->fLeafDeadWeight = (float)(WLVD); 
      pGPltB->fRootDeadWeight = (float)(WRTD);

      pGPltC->fCShoot        = (float)CSH;
      pGPltC->fCRoot         = (float)CRT;
      pGPltC->fCPlant        = (float)CTOT;


	  //%** input
      LNCMIN = (double)pPltN->fLeafMinConc;
      NLV    = (double)pPltN->fLeafCont;
      NST    = (double)pPltN->fStemCont;
      NSO    = (double)pGPltN->fNStorageCont;
// SG20110909
	  NRT    = (double)pPltN->fRootCont/10.0; //[g/m2]<--[kg/ha]
//    NRT    = (double)pPltN->fRootCont;
 
      //%** Nitrogen concentration of biomass
      NSH    = NST + NLV + NSO;
      NSHH   = NSH +(WLVD-CLVDS/CFV)*LNCMIN;
      NTOT   = NSH + NRT;


      LNC   = NLV / WLV;
      RNC   = NRT / WRT;
      HNC   = NSH / WSH;
      ONC   = INSW(-WSO, NSO/NOTNUL(WSO), 0.);
      PNC   = NTOT/ WTOT;

	  pGPltN->fNShootCont     = (float)NSH;
      pPltN->fLeafActConc     = (float)LNC;
      pPltN->fRootActConc     = (float)RNC;
      pPltN->fTotalCont       = (float)NTOT;
      pGPltN->fShootActConc   = (float)HNC;
      pGPltN->fStorageActConc = (float)ONC;
      pGPltN->fTotalActConc   = (float)PNC;
      
      ///*
      //%** Extinction coefficient of nitrogen and wind
      LAIC = (double)pGCan->fLAICdeterm;//input
      TNLV = (double)pGPltN->fNLeafTotCont;//Input
      TLAI = LAIC + CLVD /CFV*SLA0;
      
      KW   = kdiff(TLAI,BLD*3.141592654/180.,0.2);
      KN   = KW*(TNLV-SLNMIN*TLAI);
      NBK  = SLNMIN*(1.-exp(-KW*TLAI));
      KN   = 1./TLAI*log((KN+NBK)/(KN*exp(-KW*TLAI)+NBK));
      //*/
	  ///*
      NREOE  = (double)pGPltN->fNRemobToSeedE;//Input
      NREOF  = (double)pGPltN->fNRemobToSeedF;//Input

      ESD    = INSW(DETER, ESDI, 1.);
      NRES   = NREOF + (NREOE-NREOF)*(ESD-1.)/NOTNUL(min(DS,ESD)-1.);
      TSN    = NRES/PNPRE/SEEDNC/SEEDW;
      TSW    = WSO/NOTNUL(TSN)*1000.;
      PSO    = 6.25*WSO*ONC;

      pGPltB->fTotalSeedNum    = (float)TSN;
//SG20110909: für Ausgabe in *.rfp
	  pPl->pCanopy->fGrainNum = (float)TSN / pMa->pSowInfo->fPlantDens; //[grains m-2]
      pGPltB->f1000GrainWeight = (float)TSW; 
      pGPltN->fSeedProtein     = (float)PSO;
	  //*/
	  ///*

      KCRN   = -log(0.05)/6.3424/CFV/WRB/RDMX;
      CSRTN  = 1./KCRN*log(1.+KCRN*max(0.,(NRT*CFV-CRVR*RNCMIN))/RNCMIN);

      LCRT   = max(min(CSRT-1.E-4,CSRT-min(CSRTN,CSRT)),0.)/DELT;//rate
      LWRT   = LCRT/CFV;
      LNRT   = LWRT*RNCMIN;
      //*/
      ///*
      LAIC   = (double)pGCan->fLAICdeterm;//input
      LAIN   = log(1.+KN*max(0.,NLV)/SLNMIN)/KN;
      LAI    = min(LAIN, LAIC);
      TLAI   = LAIC + CLVD /CFV*SLA0;	  
      //if(LAIN>=LAIC) TLAI = LAIC + CLVD /CFV*SLA0;
      //
      //%** output ***
	  //
      pGPltC->fCStrctRootN=(float)CSRTN;
      pGCan->fLAINdeterm = (float)LAIN;
      pGCan->fLAIGreen   = (float)LAI;
      pGCan->fLAITotal   = (float)TLAI;
      //*/ //leicht erhöhte Biomasse
      /*
      LAIC   = (double)pGCan->fLAICdeterm;//input
      LAIN   = (double)pGCan->fLAINdeterm;
      LAI    = (double)pGCan->fLAIGreen;
      TLAI   = (double)pGCan->fLAITotal;	  
      */

      //%-- Canopy photosynthesis, transpiration and soil evaporation
      //Photosynthesis_GECROS(exp_p);

      //DS   = (double)pDev->fStageSUCROS;
      TMAX = (double) pCl->pWeather->fTempMax;    //[°C]
      TMIN = (double) pCl->pWeather->fTempMin;    //[°C]
	  DIFS = (double)pGS->fDiffSoilAirTemp;       //[°C]
	  /*
      TBD    = (double)fparTBD;//base temperature for phenology          (°C)       crop  table 3, p47 
      TOD    = (double)fparTOD;//optimum temperature for phenology       (°C)       crop  table 3, p47
      TCD    = (double)fparTCD;//ceiling temperature for phenology       (°C)       crop  table 3, p47
      TSEN   = (double)fparTSEN;//curvature for temperature response     (-)        deflt table 4, p49
      */
	  //TDU=DailyThermalDayUnit_GECROS(DS,TMAX,TMIN,DIFS,DAYL,TBD,TOD,TCD,TSEN);
      
	  //DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU);
      //pDev->fDevR = (float)DVR;//rate

      ///*
      //%** Specific leaf nitrogen and its profile in the canopy
      ///*
      SLN    = NLV/LAI;
      SLNT   = NLV             *KN/(1.-exp(-KN*LAI));
      SLNBC  = NLV*exp(-KN*LAI)*KN/(1.-exp(-KN*LAI));
      SLNNT  = (NLV+0.001*NLV) *KN/(1.-exp(-KN*LAI));
      
      SLNB   = (double) pGPltN->fNLeafSpecificContBottom;
      RSLNB  = (SLNBC-SLNB)/DELT;//rate
        
      ///*
      APCAN  = (double)pPltC->fGrossPhotosynR;
      RMUL   = (double)pGPltC->fUptRespCost;//integral
      RMRE   = max(min(44./12.*0.218*(NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL), 0.);
      RMN    = min(44./12.*0.218*(1.001*NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL);
      RMN    = max(0., min(APCAN-1.E-5,RMUL) + max(RMN, 0.));
      RM     = max(0., min(APCAN-1.E-5,RMUL) + RMRE);

      pGPltC->fMaintRespRN  = (float)RMN;
      pPltC->fMaintRespR    = (float)RM;
/*
      NDEMP  = (double)pGPltN->fNDmndPrev;
      NSUPP  = (double)pGPltN->fNPlantSuppPrev; 
      NSUP   = (double)pGPltN->fNPlantSupply;
      RNSUPP = (NSUP-NSUPP)/DELT;//rate

      NFIXD  = max(0., NDEMP-NSUPP);// /DELT;
      NFIXE  = max(0., APCAN-1.E-5-RM)/NOTNUL(CCFIX)*12./44.;
      NFIX   = INSW(LEGUME, 0., min(NFIXE, NFIXD));//rate
      RX     = 44./12.*(CCFIX*NFIX);
      pGPltC->fFixRespCost  = (float)RX;
*/          
///*
	  //NitrogenFixation_GECROS(exp_p);

	  NFIX   = (double)pGPltN->fNPlantSuppPrev;
      NSUPP  = (double)pGPltN->fNPlantSuppPrev; 
      RNSUPP = (double)pGPltN->fNPltSppPrvChangeR;//rate
	  RX     = (double)pGPltC->fFixRespCost;

//*/
      TMAX   = (double) pCl->pWeather->fTempMax;    //[°C]
      TMIN   = (double) pCl->pWeather->fTempMin;    //[°C]
	  DIFS   = (double)pGS->fDiffSoilAirTemp;       //[°C]
	  DAVTMP = 0.29*TMIN + 0.71*TMAX;
      NAVTMP = 0.71*TMIN + 0.29*TMAX;
      TAVSS  = ((DAVTMP+DIFS)+NAVTMP)/2.;	  
      LVDS   = (CLVD-CLVDS)/10.*(TAVSS-TBD)/(TOD-TBD);//rate
 
      ASSA   = (double)pPl->pPltCarbon->fGrossPhotosynR - RM - RX;
      pPltC->fNetPhotosynR = (float)ASSA;
      pGPltC->fNetStdgCropPhotosynR = pGPltC->fGrossStdgCropPhotosynR -(float)(RM+RX);// erhöht Biomasse
      
	  ///*
      pGPltN->fNLeafSpecificCont=(float)SLN;
      
	  PotentialNitrogenUptake_GECROS(exp_p);

	  RMN   = (double)pGPltC->fMaintRespRN;
	  DERI  = (double)pGPltB->fShtToRtActivity;
      //SHSA   = 12./44.*YGV*((double)pPl->pPltCarbon->fGrossPhotosynR -RM -RX)/ CSH;
      SHSA  = (double)pGPltB->fShootActivity;
      //SHSA  = max((double)pGPltB->fShootActivity,0.0000001);
      NDEM  = (double)pGPltN->fNDemand;
      NDEMA = (double)pGPltN->fNDmndActDrv;
      pPltN->fTotalDemand      = (float)NDEM * (float)10;// [g N m-2] = 10.0 [kg N ha-1] 
      //*/
	  /* 
      //%** Crop nitrogen demand and uptake (g N m-2 d-1)
      SHSA   = 12./44.*YGV*((double)pPl->pPltCarbon->fGrossPhotosynR -RM -RX)/ CSH;
      SHSAN  = 12./44.*YGV*((double)pGPltC->fGrossStdgCropPhotosynRN -RMN-RX)/ CSH;
      pGPltB->fShootActivity   = (float)SHSA;
      
      DERI   = max(0.,(SHSAN - SHSA)/(0.001*NTOT/CTOT));
      pGPltB->fShtToRtActivity = (float)DERI;
      ///*
      //%-- Nitrogen partitioning between shoots and roots
      HNCCR  = LNCI*exp(-0.4*DS);
      NDEMD  = INSW(DS-1., WSH*(HNCCR-HNC)*(1.+NRT/NSH)/DELT, 0.);
      NDEMA  = CRT * SHSA*SHSA/NOTNUL(DERI);
      NDEMAD = INSW(LNC-1.5*LNCI, max(NDEMA, NDEMD), 0.);
      NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,NDEMAD), 0.);
      
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	 SG20140312:
	 Vorschlag von Joachim Ingwersen zur Simulation der Winterung
    if DS < DSCRIT // DSCRIT = 0.25
    	NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD))
    else
    	NDEM   = INSW(SLNMIN-SLN+1.E-5,  min(NUPTX,NDEMAD))
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
   
      //NFIXR  = (double)pGPltN->fNFixationReserve;
      //RNFIXR = NFIX - min(NDEM,NFIXR/TCP);//rate
      //pGPltN->fNFixReserveChangeR = (float)RNFIXR;
           
      ///*
      pGPltN->fNDmndActDrv     = (float)NDEMA;
      pGPltN->fNDemand         = (float)NDEM;
      //pPltN->fTotalDemand      = (float)NDEM / (float)10;// 1.0 [kg N ha-1] = [g N m-2]  
      pPltN->fTotalDemand      = (float)NDEM * (float)10;// [g N m-2] = 10.0 [kg N ha-1] 
      //*/

	  NDEMP  = (double)pGPltN->fNDmndPrev;	
      RNDEMP = (NDEM-NDEMP)/DELT;//rate
      pGPltN->fNDmndPrvChangeR = (float)RNDEMP;

      
//-----------------------------------------------------------------------------------------

      NCR   = INSW(SLNT-SLNMIN,0.,min(NUPTX,NDEMA))
             /(YGV*(double)pGPltC->fNetStdgCropPhotosynR*12./44.);
      FNSH  = 1./(1.+NCR*DERI/NOTNUL(SHSA)*CSH/CRT*NRT/NSH);
      FCSH  = 1./(1.+NCR*DERI/NOTNUL(SHSA));

      pGPltN->fNToCFracNewBiomass = (float)NCR;
	  pGPltN->fNFracPartShoot     = (float)FNSH;
      pGPltC->fCFracPartToShoot   = (float)FCSH;

      NUPTN  = (double)pPltN->fActNO3NUpt/10;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      NUPTA  = (double)pPltN->fActNH4NUpt/10;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      NUPT   = (double)pPltN->fActNUpt/10;//[kg N ha-1 d-1] --> [g N m-2 d-1]

      //%** Maintenance and total respiration (g CO2 m-2 d-1)
      RMUN   = 44./12.*2.05*NUPTN;
      RMUA   = 44./12.*0.17*NUPTA;
      RMUS   = 0.06* 0.05/0.454*YGV*ASSA;
      RMLD   = 0.06*(1.-FCSH)*ASSA;
      RRMUL  = (RMUN+RMUA+RMUS+RMLD-RMUL)/DELT;//rate
	  pGPltC->fUptRespCostR = (float)RRMUL;
      ///*
      //*/
      YGO    = (double)pGen->fGrwEffStorage;//parameter
	  DCDSR  = (double)pGPltC->fCShortFallDemandSeed;

      DCSS   = 12./44.*    FCSH *ASSA;
      DCSR   = 12./44.*(1.-FCSH)*ASSA;
      pGPltC->fCDlySupplyShoot  = (float)DCSS;
      pGPltC->fCDlySupplyRoot   = (float)DCSR;

      FDS = betaf(DVR,1.,PMES*1.,LIMIT(1.,2.,DS)-1.);
      sinkg(DS,1.,TSN*SEEDW*CFO,YGO,FDS,DCDSR,DCSS,DELT,&DCDSC,&DCDS,&FLWCS);

      CRVS   = (double)pGPltC->fCStemReserve;//Input
      CRVR   = (double)pGPltC->fCRootReserve;//Input
      GAP    = max(0., DCDS-DCSS);
      CREMSI = min(0.94*CRVS, CRVS/NOTNUL(CRVS+CRVR)*GAP)/0.94;
      CREMRI = min(0.94*CRVR, CRVR/NOTNUL(CRVS+CRVR)*GAP)/0.94;
      CREMS  = INSW(DCDS-DCSS, 0., CREMSI);
      CREMR  = INSW(DCDS-DCSS, 0., CREMRI);

      pGPltC->fCSeedGrowthGap   = (float)GAP;
      pGPltC->fCStemToStorageRI = (float)CREMSI;
      pGPltC->fCRootToStorageRI = (float)CREMRI;
      pGPltC->fCStemToStorageR  = (float)CREMS;
      pGPltC->fCRootToStorageR  = (float)CREMR;
	  //pGPltC->fCStemReserve     = (float)CRVS;
	  //pGPltC->fCRootReserve     = (float)CRVR;


      pGPltC->fCShortFallDemandSeed  = (float)DCDSR;
      pGPltC->fCFlowToSink           = (float)FLWCS;
      pGPltC->fCDlyDemandStorage     = (float)DCDS;
      pGPltC->fCActDemandSeed        = (float)DCDSC;



	  //%** Daily carbon flow for structural stem growth
      DCDTR  = (double)pGPltC->fCShortFallDemandStem;
      DCDTP  = (double)pGPltC->fCPrvDemandStem;

      DCST   = DCSS - FLWCS;
	  IFSH   = LIMIT(0.,1.,DCST/NOTNUL(DCDTP));
      FDH = betaf(DVR,(1.+ESD)/2.,PMEH*(1.+ESD)/2.,min((1.+ESD)/2.,DS));      
      sinkg(DS,0.,CDMHT*HTMX*CFV,YGV,FDH*IFSH,DCDTR,DCST,DELT,&DCDTC,&DCDT,&FLWCT);

	  RDCDTP = (DCDTC-DCDTP)/DELT;//rate

      //%-- Output
      pGPltC->fCDlySupplyStem        = (float)DCST;
      pGPltC->fCFlowToStem           = (float)FLWCT;
      pGPltC->fCDlyDemandStem        = (float)DCDT;
      pGPltC->fCActDemandStem        = (float)DCDTC;
      pGecrosPlant->pGecrosCarbon->fCPrvDmndStmChangeR = (float)RDCDTP;


      //FCSST  = INSW(DS-(ESD+0.2), FLWCT/NOTNUL(DCSS), 0.);
      //SG20130304: verhindern, dass alles in Stängel und nichts in Blätter geht:
	  FCSST  = min(0.5,INSW(DS-(ESD+0.2), FLWCT/NOTNUL(DCSS), 0.));
      FCSO   = FLWCS/NOTNUL(DCSS);
      FCLV   = REAAND(LAIN-LAIC,ESD-DS)*(1.-FCSO-FCSST);
        
      FCRVS  = max(0.,1. - FCLV - FCSO - FCSST);
      FCRVR  = INSW(CSRTN-CSRT, 1., 0.);

      //output
      pGPltC->fCFracPartToLeaf        = (float)FCLV;
      pGPltC->fCFracPartToStem        = (float)FCSST;
      pGPltC->fCFracPartToStorage     = (float)FCSO;
      pGPltC->fCFracPartToStemReserve = (float)FCRVS;
      pGPltC->fCFracPartToRootReserve = (float)FCRVR;
      ///*
      ESD    = INSW(DETER, ESDI, 1.);
      LWLVM  = (LAIC-min(LAIC,LAIN))/SLA0/DELT;
//SG 20110801: geringeres Absterben der Blätter wg. Messdaten PAK 346 (Kraichgau)
	  LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.01*WLV);  //original: 0.03
//    LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.03*WLV);
      LCLV   = LWLV*CFV;//rate
      LNLV   = min(LWLV,LWLVM)*LNCMIN + (LWLV-min(LWLV,LWLVM))*LNC;//rate
      LCRT   = max(min(CSRT-1.E-4,CSRT-min(CSRTN,CSRT)),0.)/DELT;//rate
      LWRT   = LCRT/CFV;
      LNRT   = LWRT*RNCMIN;//rate
      //*/

      pGPltC->fCStrctRootN=(float)CSRTN;
      pGPltB->fRootWeightLossR = (float)LWRT;
      pGPltC->fCRootLossR = (float)LCRT;
      pGPltN->fNRootLossR = (float)LNRT;

      RCLV   = 12./44.*ASSA*    FCSH *    FCLV  *YGV - LCLV;//rate
      RCSST  = 12./44.*ASSA*    FCSH *    FCSST *YGV;//rate
      RCSO   = 12./44.*ASSA*FCSH*FCSO*YGO + 0.94*(CREMS+CREMR)*YGO;//rate
      RCSRT  = 12./44.*ASSA*(1.-FCSH)*(1.-FCRVR)*YGV - LCRT;//rate
      RCRVS  = FCRVS*DCSS - CREMS;//rate
      RCRVR  = FCRVR*DCSR - CREMR;//rate
      RDCDSR = max(0.,(DCDSC-RCSO/YGO))-(FLWCS-min(DCDSC,DCSS));//rate
      RDCDTR = max(0.,(DCDTC-RCSST/YGV))-(FLWCT-min(DCDTC,DCST));//rate


      RWLV   = RCLV / CFV;
      RWST   = RCSST/ CFV + RCRVS/0.444;
      RWSO   = RCSO / CFO;
      RWRT   = RCSRT/ CFV + RCRVR/0.444;

      //output
      pGPltB->fLeafWeightLossR = (float)LWLV;
      pGPltC->fCLeafLossR = (float)LCLV;
      pGPltN->fNLeafLossR = (float)LNLV;

      pGPltB->fRootWeightLossR = (float)LWRT;
      pGPltC->fCRootLossR = (float)LCRT;
      pGPltN->fNRootLossR = (float)LNRT;
 

      pGPltC->fCLeafChangeR    = (float)RCLV;
      pGPltC->fCStemChangeR    = (float)RCSST;
      pGPltC->fCRootChangeR    = (float)RCSRT;
      pGPltC->fCStorageChangeR = (float)RCSO;

      pPltB->fStemGrowR       = (float)RWST;
      pGPltB->fStorageGrowR   = (float)RWSO;
      pPltB->fLeafGrowR       = (float)RWLV;
      pPltB->fRootGrowR       = (float)RWRT;
  
      pGPltC->fCRootRsrvChangeR = (float)RCRVR;
      pGPltC->fCStemRsrvChangeR = (float)RCRVS;
      pGPltC->fCShortFallDemandStemR = (float)RDCDTR;
      pGPltC->fCShortFallDemandSeedR = (float)RDCDSR;

      RNRES  = NUPT-(LNCMIN*(RCLV+LCLV)+RNCMIN*(RCSRT+LCRT)+STEMNC*RCSST)/CFV;
      RNREOE = INSW (DS-ESD, RNRES, 0.);
      RNREOF = INSW (DS-1.0, RNRES, 0.);

      RG     = 44./12.*((1.-YGV)/YGV*(RCLV+RCSST+RCSRT+LCLV+LCRT)+(1.-YGO)/YGO* RCSO);
      RESTOT = RM+RX+RG + 44./12.*0.06*(CREMS+CREMR); 


      //%**output maintenance respiration       
      pGPltN->fNRemobToSeedR   = (float)RNRES;
      pGPltN->fNRemobToSeedER  = (float)RNREOE;//rate
      pGPltN->fNRemobToSeedFR  = (float)RNREOF;//rate

      pPltC->fGrowthRespR = (float)RG;
      pPltC->fTotRespR    = (float)RESTOT;

	  // N accumulation rates of plant organs
      rnacc(FNSH,NUPT,RWST,STEMNC,LNCMIN,RNCMIN,LNC,RNC,NLV,NRT,WLV,WRT,DELT,
            CB,CX,TM,DS,SEEDNC,RWSO,LNLV,LNRT,&RNRT,&RNST,&RNLV,&RTNLV,&RNSO);

      //%-- output
      pGPltN->fNRootAccumR    = (float)RNRT;//rate
      pGPltN->fNStemAccumR    = (float)RNST;//rate
      pGPltN->fNLeafAccumR    = (float)RNLV;//rate
      pGPltN->fNLeafTotAccumR = (float)RTNLV;//rate
      pGPltN->fNStorageAccumR = (float)RNSO;//rate

	  //LeafAreaGrowth_GECROS(exp_p);
	  //PlantHeightGrowth_GECROS(exp_p);
      //CanopyFormation_GECROS(exp_p);
	  //RootSystemFormation_GECROS(exp_p);
	  /* 
	  //%---rate of LAI driven by carbon supply
      RLAI   =  INSW(RWLV, max(-LAIC+(double)1.E-5,SLA0*RWLV), SLA0*RWLV);//rate
      //%---rate of LAI driven by nitrogen during juvenile phase    
      if ((LAIC < (double)1)&&(DS < (double)0.5))
      RLAI  = (SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV);
      //RLAI   = max(0.,(SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV));//rate    
      pCan->fLAGrowR     = (float)RLAI;
      */

	  DS   = max(0., DS + DVR);
	  pDev->fStageSUCROS = (float)DS;
	  //pDev->fDevStage    = (float)DS;

	  CTDU   = max(0., CTDU + TDU);
	  pDev->fCumTDU  = (float)CTDU;

	        //%** Carbon accumulation
      CLV    = max(0.01,CLV   + RCLV);
	  pGPltC->fCLeaf = (float)CLV;

      CLVD   = (CLVD  + LCLV);
	  pGPltC->fCLeafDead = (float)CLVD;

      CLVDS  = (CLVDS + LVDS);
      pGPltC->fCLeafDeadSoil = (float)CLVDS; 

      CSST   = (CSST  + RCSST);
	  pGPltC->fCStem = (float)CSST;

      CSO    = (CSO   + RCSO);
	  pGPltC->fCStorage =(float)CSO;

      CSRT   = (CSRT  + RCSRT); 
	  pGPltC->fCStrctRoot =(float)CSRT;

      CRTD   = (CRTD  + LCRT);
      pGPltC->fCRootDead = (float)CRTD;

	  NRT    = max(0,NRT+RNRT);
 // SG20110909
	  pPltN->fRootCont = (float)NRT*10.0; //[g/m2] --> [kg/ha]
//    pPltN->fRootCont = (float)NRT; 

	  //SG20110909: für Ausgabe in *.rfp
	  pPltN->fStovCont  = (float)(NSH*10.0); //[g/m2] --> [kg/ha]
      pPltN->fGrainCont = (float)(NSO*10.0); //[g/m2] --> [kg/ha]

      NST = (double)pPltN->fStemCont;
	  NST    = max(0,NST+RNST);
      pPltN->fStemCont = (float)NST;
        
      NLV    = max(0,NLV+RNLV);
      pPltN->fLeafCont = (float)NLV;
        
      TNLV   = max(0,TNLV+RTNLV);
      pGPltN->fNLeafTotCont = (float)TNLV;
        
      NSO    = max(0,NSO+RNSO);
      pGPltN->fNStorageCont = (float)NSO;
      
	  NLVD =(double)pGPltN->fNLeafDeadCont;
      NLVD   = max(0,NLVD+LNLV);
      pGPltN->fNLeafDeadCont = (float)NLVD;

      NRTD = (double)pGPltN->fNRootDeadCont; 
      NRTD   = max(0,NRTD+LNRT);
      pGPltN->fNRootDeadCont = (float)NRTD;

      CRVS   = max(0,CRVS+RCRVS);
	  pGPltC->fCStemReserve     = (float)CRVS;

      CRVR   = max(0,CRVR+RCRVR);
	  pGPltC->fCRootReserve     = (float)CRVR;

      NREOE  = max(0,NREOE+RNREOE);
	  pGPltN->fNRemobToSeedE   = (float)NREOE;

      NREOF  = max(0,NREOF+RNREOF);
      pGPltN->fNRemobToSeedF   = (float)NREOF;

      DCDSR  = max(0,DCDSR+RDCDSR);
      pGPltC->fCShortFallDemandSeed  = (float)DCDSR;

      DCDTR  = max(0,DCDTR+RDCDTR);
      pGPltC->fCShortFallDemandStem  = (float)DCDTR;

      SLNB   = max(0,SLNB+RSLNB);
      pGPltN->fNLeafSpecificContBottom = (float)SLNB;

      //LAIC   = max(0,LAIC+RLAI);
      //pGCan->fLAICdeterm = (float)LAIC;

      RMUL   = max(0,RMUL+RRMUL);
      pGPltC->fUptRespCost  = (float)RMUL;
 
      /*
      NDEMP  = max(0,NDEMP+RNDEMP);
      pGPltN->fNDmndPrev          = (float)NDEMP;
      
      NSUPP  = max(0,NSUPP+RNSUPP);
      pGPltN->fNPlantSuppPrev     = (float)NSUPP;
      
      NFIXT  = (double)pGPltN->fNFixationTotal;
      NFIXT  = max(0,NFIXT+NFIX);
	  pGPltN->fNFixationTotal     = (float)NFIXT;

      NFIXR  = max(0,NFIXR+RNFIXR);
      pGPltN->fNFixationReserve   = (float)NFIXR;
      */
      DCDTP  = max(0,DCDTP+RDCDTP);
      pGecrosPlant->pGecrosCarbon->fCPrvDemandStem = (float)DCDTP;

	  /*
      HT     =(double)pPl->pCanopy->fPlantHeight; 
      RHT    = (double)pGCan->fPlantHeightGrowR;
      HT     = max(0,HT+RHT);
	  RHT    = min(HTMX-HT, FDH*HTMX*IFSH);//rate
      pPl->pCanopy->fPlantHeight = (float)HT;
      pGCan->fPlantHeightGrowR   = (float)RHT;
      
      KR     = -log(0.05)/RDMX;
      RD     =(double)pPl->pRoot->fDepth; 
      RRD    = INSW(RD-RDMX, min((RDMX-RD)/DELT,(RWRT+LWRT)/(WRB+KR*(WRT+WRTD))), 0.);//rate
      RD     = max(0,RD+RRD);
      pPl->pRoot->fDepthGrowR = (float)RRD; 
      pPl->pRoot->fDepth = (float)RD;
      */
	  /*
	  TDAPAR = MAX(0.,INTGRL(TDAPAR, DAPAR, DELT))
      TPCAN  = MAX(0.,INTGRL(TPCAN, APCAN, DELT))
      TRESP  = MAX(0.,INTGRL(TRESP, RESTOT, DELT))
      TTCAN  = MAX(0.,INTGRL(TTCAN, ATCAN, DELT))
      TNUPT  = MAX(0.,INTGRL(TNUPT, NUPT, DELT))
      */

	  //-------------------------------------------------------------------------------------

      return 1;
      }//Growth_GECROS




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Astronomy_GECROS (from the SUCROS model)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates astronomic daylength,           *
*           diurnal radiation characteristics such as the daily        *
*           integral of sine of solar elevation and solar constant.    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DOY     R4  Daynumber (Jan 1st = 1)                       -      I  *
*  LAT     R4  Latitude of the site                        degree   I  *
*  INSP    R4  Inclination of sun angle for computing DDLP degree   I  *
*  SC      R4  Solar constant                             J m-2 s-1 O  *
*  SINLD   R4  Seasonal offset of sine of solar height       -      O  *
*  COSLD   R4  Amplitude of sine of solar height             -      O  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)       h      O  *
*  DDLP    R4  Photoperiodic daylength                       h      O  *
*  DSINBE  R4  Daily total of effective solar height       s d-1    O  *
*                                                                      *
*  FATAL ERROR CHECKS (execution terminated, message)                  *
*  condition: LAT > 67, LAT < -67                                      *
*                                                                      *
*  FILE usage : none                                                   *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int Astronomy_GECROS(int iJulianDay, double dLatitude)//, double INSP)
      {
      double DEC,AOB;
      int    nHiWord;
      //double INSP;
	  double RAD;
      INSP = (double)-2;//dInclSunAngle;//Inclination of sun angle
      RAD  = (double)(PI/180);
      //Check on input range of parameters
      if (dLatitude>67.0)     nHiWord=0; //Latitude too high
      if (dLatitude<-67.0)    nHiWord=0; //Latitude too low

      //Declination of the sun as function of iJulianDaynumber (iJulianDay)
      DEC = -asin( sin(23.45*RAD)*cos(2.0*PI*(iJulianDay+10.0)/365.0));
      
      //SINLD, COSLD and AOB are intermediate variables
      SINLD = sin(RAD*dLatitude)*sin(DEC);
      COSLD = cos(RAD*dLatitude)*cos(DEC);
      AOB   = SINLD/COSLD;
      
      //iJulianDaylength (DAYL) 
      DAYL   = 12.0*(1.0+2.0*asin(AOB)/PI);
      DDLP   = 12.0*(1.+2.*asin((-sin(INSP*RAD)+SINLD)/COSLD)/PI);
      //SINB = SINLD + COSLD*cos(2.0*PI(fSolarTime-12.0)/24.0), fSolarTime [h]
      DSINB  = 3600.0*(DAYL*SINLD+24.0*COSLD*sqrt(1.0-AOB*AOB)/PI);//daily integral over SINB
      DSINBE = 3600.0*(DAYL*(SINLD+0.4*(SINLD*SINLD+COSLD*COSLD*0.5))
                  +12.0*COSLD*(2.0+3.0*0.4*SINLD)*sqrt(1.0-AOB*AOB)/PI);//daily integral 
      //for the calculation of actual global radiation from daily global radiation (input)

      //Solar constant (SC) and daily extraterrestrial radiation (DS0) 
      SC  = 1367.0*(1.0+0.033*cos(2.0*PI*((double)iJulianDay-10.0)/365.0));
      DS0 = SC*DSINB;
    
      return 1;
      }
      

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      DailyThermalDayUnit_GECROS 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates the daily amount of thermal day *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DS      R4  Development stage                            -       I  *
*  TMAX    R4  Daily maximum temperature                    oC      I  *
*  TMIN    R4  Daily minimum temperature                    oC      I  *
*  DIF     R4  Daytime plant-air temperature differential   oC      I  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)      h       I  *
*  TBD     R4  Base temperature for phenology               oC      I  *
*  TOD     R4  Optimum temperature for phenology            oC      I  *
*  TCD     R4  Ceiling temperature for phenology            oC      I  *
*  TSEN    R4  Curvature for temperature response           -       I  *
*  TDU     R4  Daily thermal-day unit                       -       O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double DailyThermalDayUnit_GECROS(double DS,double TMAX,double TMIN,double DIF,double DAYL,
								  double TBD,double TOD,double TCD,double TSEN)
    {
      PGECROSSOIL     pGS    = pGecrosPlant->pGecrosSoil;
	  //PPLANT          pPl    = GetPlantPoi();

	  int i;
      double TMEAN,TT,SUNRIS,SUNSET,TD,TU,TDU;
	  //double TBD0,TOD0,TCD0;
      
      //%---timing for sunrise and sunset
      SUNRIS = (double)12.0 - (double)0.5*DAYL;
      SUNSET = (double)12.0 + (double)0.5*DAYL;

      //%---mean daily temperature
      DIF    = max((double)0.0,DIF);//DIF soil-air temperature difference
        
      TMEAN  = (TMAX + TMIN)/(double)2.0;
      TT     = (double)0.0;

      //%---diurnal course of temperature
      for (i=1;i<=24;i++)
         {
          if ((i >= SUNRIS)&&(i <= SUNSET))
                TD = TMEAN+DIF+(double)0.5*fabs(TMAX-TMIN)*cos((double)0.2618*(double)(i-14));
          else
                TD = TMEAN    +(double)0.5*fabs(TMAX-TMIN)*cos((double)0.2618*(double)(i-14));
        

   //%---assuming development rate at supra-optimum temperatures during
   //    the reproductive phase equals that at the optimum temperature     
      if (DS > (double)1.0)
          TD = min(TD,TOD);
      else
          TD = TD;
   
   //%---vernalisation for wheat and barley
      /*
	  TBD0=TBD;
	  TCD0=TCD;
	  TOD0=TOD;

	  if ((!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"BA")))
	  //if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))
	  { 
      if((DS>=(double)0.0)&&(DS < (double)0.4))
      //if(DS < (double)0.4)
	  {
      TBD=(double)-1.3;
	  TCD=(double)15.7;	  
	  TOD=(double) 4.9;
	  }//parameter of Streck et al. (2003) AFM115_139-150
	  else
	  {
	   TBD=TBD0;
	   TCD=TCD0;
	   TOD=TOD0;
	  }
	  }// Vernalization
	  */

	  //%---instantaneous thermal unit based on bell-shaped temperature response
      if ((TD < TBD)||(TD > TCD))
         TU = (double)0.0;
      else
         TU = pow(((TCD-TD)/(TCD-TOD))*pow((TD-TBD)/(TOD-TBD),(TOD-TBD)/(TCD-TOD)),TSEN);
            
      TT = TT + TU/(double)24.0;

        }//end for

      //%---daily thermal unit
      TDU = TT;


      return TDU;
      }



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Phenology_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates the daily amount of thermal day *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DS      R4  Development stage                            -       I  *
*  TMAX    R4  Daily maximum temperature                    oC      I  *
*  TMIN    R4  Daily minimum temperature                    oC      I  *
*  DIF     R4  Daytime plant-air temperature differential   oC      I  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)      h       I  *
*  TBD     R4  Base temperature for phenology               oC      I  *
*  TOD     R4  Optimum temperature for phenology            oC      I  *
*  TCD     R4  Ceiling temperature for phenology            oC      I  *
*  TSEN    R4  Curvature for temperature response           -       I  *
*  TDU     R4  Daily thermal-day unit                       -       O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double Phenology_GECROS(double DS,double SLP,double DDLP,double SPSP,double EPSP,double PSEN,
                                    double MTDV,double MTDR,double TDU, double OptVernDays)
{
 PPLANT  pPl = GetPlantPoi();
 //double DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU,DVR;
 double MOP,DLP,EFP,DVR;
 double CVU, EFV;

 double BasVernDays; //for Vernalization following SPASS

      //%---determining if it is for short-day or long-day crop
      if (SLP < (double)0.0)
         {
           MOP =(double)18.0; //minimum optimum photoperiod for long-day crop
           DLP = min(MOP,DDLP);
         }
      else
      {
            MOP =(double)11.0; //minimum optimum photoperiod for short-day crop
            DLP = max(MOP,DDLP);
      }

      //%---effect of photoperiod on development rate
       if ((DS < SPSP)||(DS > EPSP)) 
          EFP = (double)1.0;
       else
          EFP = max((double)0.0, (double)1.0-PSEN*(DLP-MOP));

      //%---development rate of vegetative and reproductive phases
       if ((DS>=(double)0.0)&&(DS<(double)1.0))
          DVR = (double)1./MTDV*TDU*EFP;
       else
          DVR = (double)1./MTDR*TDU;
      ///*
	  //%---vernalisation--- wheat and barley
	   EFV = (double) 1;
	  //if (!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))
	  //if ((!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"WH"))||(!lstrcmp(pPl->pGenotype->acCropCode,(LPSTR)"BA")))
	  if (fparVERN > (float)0)
	  {
		CVU = (double) pPl->pDevelop->fCumVernUnit;

		//EP20100708 nach Streck et al. (2002)
//		EFV = abspowerDBL(CVU,(double)5)/(abspowerDBL((double)22.5,(double)5)+abspowerDBL(CVU,(double)5)); 
		//STRECK, N.A.; WEISS, A.; BAENZINGER, P.S. A generalized vernalization response function for winter wheat. 
		//Agronomy Journal, v.95, p.155-159, 2003.

		//SG 20110801: nach SPASS (Wang)
		BasVernDays = 0.174*pPl->pGenotype->fOptVernDays; 
		if (pPl->pGenotype->fOptVernDays==(float)0.0)
			EFV = 1.0;
		else
			EFV = (CVU-BasVernDays)/(pPl->pGenotype->fOptVernDays-BasVernDays);
	
		EFV =min(1.0,max(0.0,EFV));
	  }
 
	  if(fparVERN == (float)0) EFV = (double) pPl->pDevelop->fVernEff;

	  if(fparVERN < (float)0) EFV = (double) 1;

	  //if (DS<(double)0.4) DVR = (double)1./MTDV*TDU*EFP*EFV;
	  if ((DS>=(double)0.0)&&(DS<(double)0.4)) 
		  DVR = (double)1./MTDV*TDU*EFP*EFV;
	  if ((DS>=(double)0.4)&&(DS<(double)1.0)) 
		  DVR = (double)1./MTDV*TDU*EFP;
	  if (DS>=(double)1.0)                     
		  DVR = (double)1./MTDR*TDU;
	  
      return DVR;
}

      
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Photosynthesis_GECROS (Gross Photosynthesis and Transpiration) 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds Photosynthesis_GECROS(EXP_POINTER)
      {
      PSWATER     pSW  = pSo->pSWater;
      PWLAYER     pWL  = pWa->pWLayer;
      PWEATHER    pWth = pCl->pWeather; 
      PCANOPY     pCan = pPl->pCanopy;
      PPLTCARBON  pCbn = pPl->pPltCarbon;
      
      PGECROSCARBON   pGPltC = pGecrosPlant->pGecrosCarbon;
	  PGECROSSOIL     pGS    = pGecrosPlant->pGecrosSoil;

      RESPONSE VLS[]={0., 0., 2.5, 0.};
      char pType[5];      //SG/17/05/99: wg. Unterscheidung der Pflanzenarten
	  //double dLatitude;
	  double PPCAN,APCANS,APCANN,APCAN,PTCAN,ATCAN,PESOIL,AESOIL,DIFS,DIFSU,DIFSH,DAPAR;
      double d1,d2;
      double DDTR,TMAX,TMIN,DVP,WNM,C3C4,LAI,LAIC,TLAI,HT,RD,NLV,TNLV;
      double FVPD,LS;
      double WCUL,DWSUP,WUL,WCMIN;
	  //double TCP;
	  //*
      int iLayer;
	  float f1;
	  double WLL,PWP;
	  float fProfileDepth=(float)0;
	  //*/
	  float DeltaZ = pSo->fDeltaZ;
	  extern float  fparWCMIN;
      //SD1,RSS,BLD,LWIDTH,SLNMIN,CO2A,EAJMAX,XVN,XJN,THETA

	  if (fparWCMIN ==(float)-99) WCMIN = (double)0.05;
	  else WCMIN    = (double)fparWCMIN;


      //if (pPl->pDevelop->fStageSUCROS<(float)0.0)
      //      return 0;


//Check whether a C3 or C4 crop
      lstrcpy(pType,"C3\0");

      if ((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MI"))
            ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"MZ"))
            ||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"SG")))

            lstrcpy(pType,"C4\0");

//Astronomy
      //iJulianDay  = (int)pTi->pSimTime->iJulianDay;
      //dLatitude   = (double)pLo->pFarm->fLatitude;

      //Astronomy_GECROS(iJulianDay, dLatitude);



//input parameters
      DDTR = (double) pCl->pWeather->fSolRad*1E6; //[J/(m2*d)]
      TMAX = (double) pCl->pWeather->fTempMax;    //[°C]
      TMIN = (double) pCl->pWeather->fTempMin;    //[°C]

      d1   = (double)0.611 * exp((double)17.269*TMAX/((double)237.3+TMAX));
      d2   = (double)0.611 * exp((double)17.269*TMIN/((double)237.3+TMIN));
      DVP  = (double) pCl->pWeather->fHumidity/((double)50./d1+(double)50./d2);
	         //daily mean vapour pressure [kPa], FAO Report 1990 by Martin Smith, Annex V, eq. (15)
      //DVP  = (double) pCl->pWeather->fHumidity;//test
      WNM  = max((double)0.1,(double)pCl->pWeather->fWindSpeed);  //[m s-1]      

      if ((0==lstrcmp(pType,"C3\0"))||(0==lstrcmp(pType,"c3\0")))   
            C3C4=(double)1; //C3 plants
      else
            C3C4=(double)-1;//C4 plants

      LAI  = (double) pGecrosPlant->pGecrosCanopy->fLAIGreen;//[m2 m-2]
      LAIC = (double) pGecrosPlant->pGecrosCanopy->fLAICdeterm;//[m2 m-2]
      TLAI = (double) pGecrosPlant->pGecrosCanopy->fLAITotal;//[m2 m-2]
      HT   = (double) pPl->pCanopy->fPlantHeight; //[m]
      //LW   = (double) pPl->pCanopy->fLeafWidth; //[m]
      //RD   = max(1.0,(double)pPl->pRoot->fDepth); //[cm]

//-----------------------------------------------------------------------------------------------

	  RD   = (double)pPl->pRoot->fDepth; //[cm] 
      RD   = max(RD,max(2.,SD1));

      ///*    
	  WUL  = (double)0;
      WLL  = (double)0;
      PWP  = (double)0;

//SG 20111103: first layer pSW=pSo->pSWater->pNext (see xh2o_def.h)
	 //for (pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater,iLayer=1;
	 //    pWL->pNext!=NULL,pSW->pNext!=NULL;pWL=pWL->pNext,pSW=pSW->pNext,iLayer++)    
	 for (pWL=pWa->pWLayer->pNext,pSW=pSo->pSWater->pNext,iLayer=1;
		 pWL->pNext!=NULL,pSW->pNext!=NULL;pWL=pWL->pNext,pSW=pSW->pNext,iLayer++)    
    {
     if (iLayer*DeltaZ <= (float)(RD*10.))
	 {
	    WUL += (double)(pWL->fContAct*DeltaZ);
	    PWP += (double)(pSW->fContPWP*DeltaZ);
	 }
	 else if (((float)RD*10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= (float)RD*10.+DeltaZ))
     {
        f1 = (iLayer*DeltaZ-(float)(RD*10.))/DeltaZ;
	    WUL+= (double)(pWL->fContAct*((float)1-f1)*DeltaZ);
	    PWP+= (double)(pSW->fContPWP*((float)1-f1)*DeltaZ);
		WLL+= (double)(pWL->fContAct*f1*DeltaZ);
     }
	 else
		WLL+= (double)pWL->fContAct*DeltaZ;
     }//for
	 //*/
      ///*
//SG 20111103: 
	/*WUL ist hier der Gesamtwassergehalt in der durchwurzelten Bodenschicht.
	 In der Gecros-Dokumentation allerdings nur der Anteil, der das Wasser "which is 
	 unavailable for removal by evapotranspiration" übersteigt.
	 Deshalb muss meiner Meinung nach PWP von WUL abgezogen werden.*/
	DWSUP  = max(0.1,(WUL-PWP)/TCP+0.1); //SG
//	DWSUP  = max(0.1,WUL/TCP+0.1); //EP
	  if (PWP>WUL) DWSUP=0.1;

	  if (pGS->fWaterContUpperLayer==(float)0) pGS->fWaterContUpperLayer = (float)WUL;
      if (pGS->fWaterContLowerLayer==(float)0) pGS->fWaterContLowerLayer = (float)WLL;
      //if (pGS->fETDailyWaterSupply ==(float)0) pGS->fETDailyWaterSupply  = (float)DWSUP;
//SG 20111103: 
	/* pGS->fWaterContUpperLayer und pGS->fWaterContLowerLayer werden in "ActualNitrogenUptake_GECROS" 
	benötigt und müssen deshalb immer (nicht nur beim ersten Zeitschritt) berechnet werden
	*/
	  pGS->fWaterContUpperLayer = (float)WUL;
      pGS->fWaterContLowerLayer = (float)WLL;
      pGS->fETDailyWaterSupply  = (float)DWSUP;
      //*/
//-----------------------------------------------------------------------------------------------

      //SD1  = Parameter (soil depth of upper layer) //[cm]
      //RSS Parameter (soil resistance equiv. to stomata resistance)
      //BLD Parameter (leaf angle from horizontal)
      TNLV = (double)pGecrosPlant->pGecrosNitrogen->fNLeafTotCont;
      NLV  = (double)pPl->pPltNitrogen->fLeafCont;
      //SLNMIN Parameter (minimum leaf N content for Photosynthesis)

      //DWSUP = (double)pGS->fETDailyWaterSupply;//[mm d-1] 
      //DWSUP= (double)(pWa->pWLayer->pNext->fContAct*pSo->pSLayer->pNext->fThickness);//[mm d-1] 
      //define for upper layer -> to do !!! ep20071016

      //CO2A Parameter (ambient CO2 concentration)
      LS   = INSW(LODGE,(double)0.,(double)AFGENERATOR(pPl->pDevelop->fStageSUCROS,VLS));//LS = (double)0.;
      //EAJMAX Parameter (Farquahar Photosynthesís model)
      //XVN Parameter (Farquhar Photosynthesis model)
      //XJN Parameter (Farquhar Photosynthesis model)
      //THETA Parameter (Farquhar Photosynthesis model)
      //WUL  = (double)pGS->fWaterContUpperLayer;//[mm]
//SG 20111103: 
	/*Wie oben: WUL ist hier der Gesamtwassergehalt in der durchwurzelten Bodenschicht.
	 In der Gecros-Dokumentation allerdings nur der Anteil, der das Wasser "which is 
	 unavailable for removal by evapotranspiration" übersteigt.
	 Deshalb muss meiner Meinung nach WCMIN*10*RD (=PWP) nicht noch zu WUL dazu addiert werden.*/
//	  WCUL = (WUL+WCMIN*10.*RD)/10./NOTNUL(RD);
	  WCUL = WUL/10./NOTNUL(RD); //[m3 m-3]
      //WCUL = (double)pWa->pWLayer->pNext->fContAct;//Water content of upper layer [mm mm-1] 
      //define for upper layer -> to do !!! ep20071016

      FVPD = INSW(C3C4,(double)0.195127,(double)0.116214);

      //gross photosynthesis rate      
      pCbn->fGrossPhotosynR = (float)DailyCanopyGrossPhotosynthesis_GECROS(SC,SINLD,COSLD,DAYL,DSINBE,
          DDTR,TMAX,TMIN,DVP,WNM,C3C4,LAIC,TLAI,HT,LWIDTH,RD,SD1,RSS,BLD,NLV,TNLV,SLNMIN,
          DWSUP,CO2A,LS,EAJMAX,XVN,XJN,THETA,WCUL,FVPD, &PPCAN,&APCANS,&APCANN,&APCAN,
                 &PTCAN,&ATCAN,&PESOIL,&AESOIL,&DIFS,&DIFSU,&DIFSH,&DAPAR);

      //%** output ***
      fPotTraDay                       = (float)PTCAN;
      fActTraDay                       = (float)ATCAN;
      pWa->pEvap->fPotDay              = (float)PESOIL;
      pWa->pEvap->fActDay              = (float)AESOIL;
      pGPltC->fGrossStdgCropPhotosynR  = (float)APCANS;
      pGPltC->fGrossStdgCropPhotosynRN = (float)APCANN;
	  pPl->pPltCarbon->fGrossPhotosynR = (float)APCAN;

	  pGS->fDiffSoilAirTemp            = (float)DIFS;

	  //%** output expertn ***
	  pWa->pEvap->fPotR                = (float)PESOIL;
	  pWa->pEvap->fActR                = (float)AESOIL;
// SG 20111107: pPl->pPltWater->fPotTranspDay und pPl->pPltWater->fActTranspDay dürfen nicht ausserhalb von 
//              balance.c gesetzt werden (werden dort in jedem Zeitschritt aufaddiert!)
//	  pPl->pPltWater->fPotTranspDay    = (float)PTCAN;
	  //pPl->pRoot->fUptakeR             = (float)ATCAN;
//	  pPl->pPltWater->fActTranspDay    = (float)ATCAN;
	  pWa->fPotETDay                   = (float)(PESOIL+PTCAN);
      
	  pWa->fPotETdt                    = pWa->fPotETDay * pTi->pTimeStep->fAct;
      pPl->pPltWater->fPotTranspdt     = pPl->pPltWater->fPotTranspDay * pTi->pTimeStep->fAct;


          return 1;
    }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Daily Canopy gross photosynthesis 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: The subroutine calculates daily total gross photosynthesis *
*           and transpiration by performing a Gaussian integration     *
*           over time. At five different times of the day, temperature *
*           and radiation are computed to determine assimilation       *
*           and transpiration whereafter integration takes place.      *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SC      R4  Solar constant                             J m-2 s-1 I  *
*  SINLD   R4  Seasonal offset of sine of solar height    -         I  *
*  COSLD   R4  Amplitude of sine of solar height          -         I  *
*  DAYL    R4  Astronomic daylength (base = 0 degrees)    h         I  *
*  DSINBE  R4  Daily total of effective solar height      s d-1     I  *
*  DDTR    R4  Daily global radiation                     J m-2 d-1 I  *
*  TMAX    R4  Daily maximum temperature                  oC        I  *
*  TMIN    R4  Daily minimum temperature                  oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  WNM     R4  daily average wind speed (>=0.1 m/s)       m s-1     I  *
*  C3C4    R4  Crop type (=1 for C3, -1 for C4 crops)     -         I  *
*  LAI     R4  (green)Leaf area index                     m2 m-2    I  *
*  TLAI    R4  Total Leaf area index                      m2 m-2    I  *
*  HT      R4  Plant height                               m         I  *
*  LWIDTH  R4  Leaf width                                 m         I  *
*  RD      R4  Rooting depth                              cm        I  *
*  SD1     R4  Depth of evaporative upper soil layer      cm        I  *
*  RSS     R4  Soil resistance,equivalent to leaf stomata s m-1     I  *
*  BLD     R4  Leaf angle from horizontal                 degree    I  *
*  KN      R4  Leaf nitrogen extinction coefficient       m2 m-2    I  *
*  KW      R4  Windspeed extinction coefficient in canopy m2 m-2    I  *
*  SLN     R4  Average leaf nitrogen content in canopy    g m-2     I  *
*  SLNT    R4  Top-leaf nitrogen content                  g m-2     I  *
*  SLNN    R4  Value of SLNT with small plant-N increment g m-2     I  *
*  SLNMIN  R4  Minimum or base SLNT for photosynthesis    g m-2     I  *
*  DWSUP   R4  Daily water supply for evapotranspiration  mm d-1    I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  LS      R4  Lodging severity                           -         I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax & leaf N   umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  WCUL    R4  Water content of the upper soil layer      m3 m-3    I  *
*  FVPD    R4  Slope for linear effect of VPD on Ci/Ca    (kPa)-1   I  *
*  PPCAN   R4  Potential canopy CO2 assimilation          g m-2 d-1 O  *
*  APCANS  R4  Actual standing-canopy CO2 assimilation    g m-2 d-1 O  *
*  APCANN  R4  APCANS with small plant-N increment        g m-2 d-1 O  *
*  APCAN   R4  Actual canopy CO2 assimilation             g m-2 d-1 O  *
*  PTCAN   R4  Potential canopy transpiration             mm d-1    O  *
*  ATCAN   R4  Actual canopy transpiration                mm d-1    O  *
*  PESOIL  R4  Potential soil evaporation                 mm d-1    O  *
*  AESOIL  R4  Actual soil evaporation                    mm d-1    O  *
*  DIFS    R4  Daytime average soil-air temp. difference  oC        O  *
*  DIFSU   R4  Daytime aver. sunlit leaf-air temp. diff.  oC        O  *
*  DIFSH   R4  Daytime aver. shaded leaf-air temp. diff.  oC        O  *
*  DAPAR   R4  Daily PAR absorbed by crop canopy          J m-2 d-1 O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double DailyCanopyGrossPhotosynthesis_GECROS(double SC,double SINLD,double COSLD,double DAYL,double DSINBE,
          double DDTR,double TMAX,double TMIN,double DVP,double WNM,double C3C4,double LAI,double TLAI,
            double HT,double LWIDTH,double RD,double SD1,double RSS,double BLD,double NLV,double TNLV,
            double SLNMIN,double DWSUP,double CO2A,double LS,double EAJMAX,double XVN,double XJN,double THETA,
            double WCUL,double FVPD,double *PPCAN,double *APCANS,double *APCANN,double *APCAN,double *PTCAN,
            double *ATCAN,double *PESOIL,double *AESOIL,double *DIFS,double *DIFSU,double *DIFSH,double *DAPAR)
      {
      //Input: SC,SINLD,COSLD,DAYL,DSINBE, s.o. und Astronomy; 
      //Input: DDTR=pWth->fSolRad,TMAX=pWth->fTempMax,TMIN=pWth->fTempMin, DVP aus pCl->pWeather->fHumidity;
      //Input: WNM=pWth->fWindSpeed,C3C4 Param,LAIC=pGCan->fLAIGreen,TLAI=pGCan->fLAITotal,
      //Input: HT=pPl->pCanopy->fPlantHeight,LWIDTH Param,RD=pPl->pRoot->fDepth;
      //Input: SD1 Param,RSS Param,BLD Param;
      //Input: KN,KW: lokal eingefügt ---> s.u. ep170407 
      //Input: SLN,SLNT,SLNNT: lokal eingefügt ---> s.u. ep190407
      //Input: SLNMIN Param;
      //Input: DWSUP soil water supply ---> to do !
      //Input: CO2A Param, LS lodging severity Param =INSW(LODGE, (double)0., AFGEN (VLS,DS));
      //Input: EAJMAX Param, XVN Param, XJN Param, THETA Param;
      //Input: WCUL water content upper soil layer ---> to do !
      //Input: FVPD Param = INSW(C3C4, 0.195127, 0.116214);

      //Output: PPCAN,APCANS,APCANN,APCAN,PTCAN,ATCAN,PESOIL,AESOIL,DIFS,DIFSU,DIFSH,DAPAR

      int     i,j;
      double  KBPPAR,KBPNIR;
      double  SUNRIS,HOUR,PAR,NIR,ATMTR,FRDF,PARDF,PARDR,NIRDF,NIRDR,NRADS;
      double  SINB,DTR,DAYTMP,WSUP,WSUP1,WND,PSPAR,PSNIR;
      double  BL,KB,SCPPAR,SCPNIR,KDPPAR,KDPNIR,PCBPAR,PCBNIR,PCDPAR,PCDNIR;
      double  RT,RTS,KW,KN,NBK,SLNT,NPSU,NPSH,SLNN,NPSUN,NPSHN;
      double  APARSU,APARSH,ANIRSU,ANIRSH,APAR,ATRJSU,ATRJSH,ATRJS;
      
      //%---boundary layer resistance for canopy, sunlit and shaded leaves
      double  FRSU,FRSH,GBHLF,GBHC,GBHSU,GBHSH,RBHSU,RBWSU,RBHSH,RBWSH,RBHS,RBWS;

      double  PLFSU,PTSU,RSWSU,NRADSU,SLOPSU;
      double  PLFSH,PTSH,RSWSH,NRADSH,SLOPSH,IPP,IPT,PT1,IPE;
      double  IAE,IAT,ATSU,ATSH,ADIFSU,ADIFSH,ADIFS,IAPS,IAPNN,IAP;

      double  ACO2I,IPPL,SLN,IRDL,ARSWSU,IAPL,IAPN;
      double  PASSU,PANSU,PASSH,PANSH;
      
      int     nGauss=5;
      double  xGauss[]={0.0469101, 0.2307534, 0.5000000, 0.7692465, 0.9530899};
      double  wGauss[]={0.1184635, 0.2393144, 0.2844444, 0.2393144, 0.1184635};
    
      //%---output-variables set to zero and five different times of a day(HOUR)
        *PPCAN  = 0.;
        *APCANS = 0.;
        *APCANN = 0.;
        *APCAN  = 0.;
        *PTCAN  = 0.;
        *ATCAN  = 0.;
        *PESOIL = 0.;
        *AESOIL = 0.;
        *DIFS   = 0.;
        *DIFSU  = 0.;
        *DIFSH  = 0.;
        *DAPAR  = 0.;


        for (i=1;i<=nGauss;i++)
        {
         j=i-1;

         //%---timing for sunrise
         SUNRIS = 12.0 - 0.5*DAYL;

         //%---specifying the time (HOUR) of a day
         //(at HOUR, radiation is computed and used to compute assimilation)
         HOUR = SUNRIS+DAYL*xGauss[j];
             
         //%---sine of solar elevation
         SINB  = max(0.0, SINLD+COSLD*cos(2.0*PI*(HOUR-12.0)/24.0));
            
         //%---daytime course of radiation  [J m-2 d-1] --> [J m-2 s-1]
         DTR   = DDTR*(SINB*SC/1367.)/DSINBE;

         //%---daytime course of air temperature 
         DAYTMP= TMIN+(TMAX-TMIN)*sin(PI*(HOUR+DAYL/2.-12.)/(DAYL+3.));

         //%---daytime course of water supply  [mm d-1] --> [mm s-1]
         WSUP  = DWSUP*(SINB*SC/1367.)/DSINBE;
         WSUP1 = WSUP*SD1/RD;
         WSUP1 = min(WSUP,WSUP1);
         //%---daytime course of wind speed
         WND   = WNM;   //!no diurnal fluctuation is assumed here

         //%---total incoming PAR and NIR
         PAR   = 0.5*DTR;
         NIR   = 0.5*DTR;

         //%---diffuse light fraction (FRDF) from atmospheric transmission (ATMTR)
         ATMTR = PAR/(0.5*SC*SINB);
             
         if (ATMTR<=0.22) 
           FRDF = 1.0;
         else
         {
          if ((ATMTR>0.22)&&(ATMTR<=0.35)) 
            FRDF = 1.0-6.4*(ATMTR-0.22)*(ATMTR-0.22); 
          else
            FRDF = 1.47-1.66*ATMTR;
         }
                  
         FRDF = max (FRDF, 0.15+0.85*(1.0-exp(-0.1/SINB)));
            
            
         //%---incoming diffuse PAR (PARDF) and direct PAR (PARDR)
         PARDF = PAR * FRDF;
         PARDR = PAR - PARDF;

         //%---incoming diffuse NIR (NIRDF) and direct NIR (NIRDR)
         NIRDF = NIR * FRDF;
         NIRDR = NIR - NIRDF;

         //%---extinction and reflection coefficients
         BL    = BLD*PI/(double)180.;   //leaf angle, conversion to radians

         KB    = kbeam(SINB,BL);

         SCPPAR = (double)0.2;           //leaf scattering coefficient for PAR
         SCPNIR = (double)0.8;           //leaf scattering coefficient for NIR
      
         KDPPAR = kdiff(TLAI,BL,SCPPAR);
         KDPNIR = kdiff(TLAI,BL,SCPNIR);

         KBPPAR = KB*sqrt((double)1.-SCPPAR);//scattered beam radiation extinction coefficient for PAR
         KBPNIR = KB*sqrt((double)1.-SCPNIR);//scattered beam radiation extinction coefficient for NIR
         
         PCBPAR = refl(SCPPAR,KB);//canopy beam radiation PAR reflection coefficient
         PCBNIR = refl(SCPNIR,KB);//canopy beam radiation NIR reflection coefficient

         PCDPAR = (double)0.057;          //canopy diffuse PAR reflection coefficient
         PCDNIR = (double)0.389;          //canopy diffuse NIR reflection coefficient

         //%---turbulence resistance for canopy (RT) and for soil (RTS)
         RT     = (double)0.74*pow(log((2.-0.7*HT)/(0.1*HT)),(double)2)/(pow((double)0.4,(double)2)*WND);
         RTS    = (double)0.74*pow(log(56.),(double)2)/(pow((double)0.4,(double)2)*WND);

         //%---fraction of sunlit and shaded components in canopy
         FRSU   = 1./KB/LAI*(1.- exp(-KB*LAI));
         FRSH   = 1.-FRSU;

         //Input: Extinction coefficient of nitrogen and wind ep170407
         KW     = kdiff(TLAI,BLD*3.141592654/180.,0.2);
            
         KN     = KW*(TNLV-SLNMIN*TLAI);
         NBK    = SLNMIN*(1.-exp(-KW*TLAI));

         KN     = 1./TLAI*log((KN+NBK)/(KN*exp(-KW*TLAI)+NBK));

         //Input: Leaf N-content ep190407
         SLNT   = NLV*KN/((double)1.-exp(-KN*LAI));
         SLNN   = (NLV+(double)0.001*NLV)*KN/((double)1.-exp(-KN*LAI));
         SLN    = NLV/LAI;
         

         //%---boundary layer resistance for canopy, sunlit and shaded leaves
         GBHLF  = 0.01*sqrt(WND/LWIDTH);
         GBHC   = (1.-exp(- 0.5*KW    *LAI))/(0.5*KW   )*GBHLF;
         GBHSU  = (1.-exp(-(0.5*KW+KB)*LAI))/(0.5*KW+KB)*GBHLF;
         GBHSH  = GBHC - GBHSU;

         RBHSU  = 1./GBHSU;    //boundary layer resistance to heat,sunlit part
         RBWSU  = 0.93*RBHSU;  //boundary layer resistance to H2O, sunlit part
         RBHSH  = 1./GBHSH;    //boundary layer resistance to heat,shaded part
         RBWSH  = 0.93*RBHSH;  //boundary layer resistance to H2O, shaded part

         //%---boundary layer resistance for soil
         RBHS   = 172.*sqrt(0.05/max(0.1,WND*exp(-KW*TLAI)));
         RBWS   = 0.93*RBHS;

         //%---photosynthetically active nitrogen for sunlit and shaded leaves
         NPSU   = pan(SLNT,SLNMIN,LAI,KN,KB,0);
         NPSH   = pan(SLNT,SLNMIN,LAI,KN,KB,1);
         NPSUN  = pan(SLNN,SLNMIN,LAI,KN,KB,0);
         NPSHN  = pan(SLNN,SLNMIN,LAI,KN,KB,1);
         //%---absorbed PAR and NIR by sunlit leaves and shaded leaves
         APARSU = ligab(SCPPAR,KB,KBPPAR,KDPPAR,PCBPAR,PCDPAR,PARDR,PARDF,LAI,0);
         APARSH = ligab(SCPPAR,KB,KBPPAR,KDPPAR,PCBPAR,PCDPAR,PARDR,PARDF,LAI,1);
         ANIRSU = ligab(SCPNIR,KB,KBPNIR,KDPNIR,PCBNIR,PCDNIR,NIRDR,NIRDF,LAI,0);
         ANIRSH = ligab(SCPNIR,KB,KBPNIR,KDPNIR,PCBNIR,PCDNIR,NIRDR,NIRDF,LAI,1);
         APAR   = APARSU+APARSH;

         //%---absorbed total radiation (PAR+NIR) by sunlit and shaded leaves
         ATRJSU = APARSU+ANIRSU;
         ATRJSH = APARSH+ANIRSH;

         //%---absorbed total radiation (PAR+NIR) by soil
         PSPAR  = 0.1;//!soil PAR reflection
         PSNIR  = INSW(WCUL-(double)0.5,(double)0.52-(double)0.68*WCUL,(double)0.18);//!soil NIR reflection
         ATRJS  = (1.-PSPAR)*(PARDR*exp(-KBPPAR*TLAI)+PARDF*exp(-KDPPAR*TLAI))
                 +(1.-PSNIR)*(NIRDR*exp(-KBPNIR*TLAI)+NIRDF*exp(-KDPNIR*TLAI));

         //%---instantaneous potential photosynthesis and transpiration
         PotentialLeafPhotosynthesis_GECROS(FRSU,DAYTMP,DVP,CO2A,C3C4,FVPD,APARSU,NPSU,RBWSU,RBHSU,
                                            RT*FRSU,ATRJSU,ATMTR,EAJMAX,XVN,XJN,THETA, 
                                            &PLFSU,&PTSU,&RSWSU,&NRADSU,&SLOPSU);

         PotentialLeafPhotosynthesis_GECROS(FRSH,DAYTMP,DVP,CO2A,C3C4,FVPD,APARSH,NPSH,RBWSH,RBHSH,
                                           RT*FRSH,ATRJSH,ATMTR,EAJMAX,XVN,XJN,THETA, 
                                           &PLFSH,&PTSH,&RSWSH,&NRADSH,&SLOPSH);
         IPP    = PLFSU+ PLFSH;
         IPT    = PTSU + PTSH;
         PT1    = IPT  * SD1/RD;

         //%---instantaneous potential soil evaporation 
         IPE    = PotentialEvaporation_GECROS(DAYTMP,DVP,RSS,RTS,RBWS,RBHS,ATRJS,ATMTR,PT1,WSUP1,&NRADS);

         //%---instantaneous actual soil evaporation, actual canopy transpiration and photosynthesis
         IAE    = min(IPE,IPE/(PT1+IPE)*WSUP1);
         IAT    = min(IPT,PT1/(PT1+IPE)*WSUP1+WSUP-WSUP1);
         ATSU   = PTSU/IPT*IAT;
         ATSH   = PTSH/IPT*IAT;

         ADIFS = difla(NRADS,IAE,RBHS,RTS);
      
         ActualLeafPhotosynthesis_GECROS(DAYTMP,APARSU,DVP,CO2A,C3C4,FVPD,NRADSU,ATSU,PTSU,
                                         RT*FRSU,RBHSU,RBWSU,RSWSU,SLOPSU,NPSU,NPSUN,
                                         EAJMAX,XVN,XJN,THETA, &PASSU,&PANSU,&ADIFSU);
         ActualLeafPhotosynthesis_GECROS(DAYTMP,APARSH,DVP,CO2A,C3C4,FVPD,NRADSH,ATSH,PTSH,
                                        RT*FRSH,RBHSH,RBWSH,RSWSH,SLOPSH,NPSH,NPSHN,
                                        EAJMAX,XVN,XJN,THETA, &PASSH,&PANSH,&ADIFSH);
         IAPS   = PASSU + PASSH;
         IAPN   = PANSU + PANSH;

         //%---canopy photosynthesis if there is lodging
         ACO2I  = internalCO2(DAYTMP+ADIFSU,DVP,FVPD,CO2A,C3C4);
         //ASVP = noetig???
         IPPL   = photo(C3C4,((double)1.-SCPPAR)*PAR,DAYTMP+ADIFSU,ACO2I,SLN-SLNMIN,EAJMAX,XVN,XJN,THETA);
         IRDL   = darkr(DAYTMP+ADIFSU,SLN-SLNMIN,XVN);
         ARSWSU = (PTSU-ATSU)*(SLOPSU*(RBHSU+RT*FRSU)+(double).067*(RBWSU+RT*FRSU))/ATSU/(double).067
                  +PTSU/ATSU*RSWSU;
         IAPL   = (((double)1.6*RSWSU+(double)1.3*RBWSU+RT*FRSU)/((double)1.6*ARSWSU+1.3*RBWSU+RT*FRSU)
                   *(IPPL-IRDL)+IRDL)*((double)1.-exp(-LAI));
         IAP    = min(IAPS, (1.-LS)*IAPS+LS*IAPL);
         IAPNN  = min(IAPN, (1.-LS)*IAPN+LS*IAPL*IAPN/IAPS);
                  

          //---integration of assimilation rate and transpiration rate to a daily total
         *PPCAN  = *PPCAN  + IPP   * wGauss[j];
         *APCANS = *APCANS + IAPS  * wGauss[j];
         *APCANN = *APCANN + IAPNN * wGauss[j];
         *APCAN  = *APCAN  + IAP   * wGauss[j];
         *PTCAN  = *PTCAN  + IPT   * wGauss[j];
         *ATCAN  = *ATCAN  + IAT   * wGauss[j];
         *PESOIL = *PESOIL + IPE   * wGauss[j];
         *AESOIL = *AESOIL + IAE   * wGauss[j];
         *DIFS   = *DIFS   + ADIFS * wGauss[j];
         *DIFSU  = *DIFSU  + ADIFSU* wGauss[j];
         *DIFSH  = *DIFSH  + ADIFSH* wGauss[j];
         *DAPAR  = *DAPAR  + APAR  * wGauss[j];

           //DTGA = DTGA+FGROS*wGauss[j];

      }//for

      *PPCAN  = *PPCAN  * DAYL * (double)3600.;
      *APCANS = *APCANS * DAYL * (double)3600.;
      *APCANN = *APCANN * DAYL * (double)3600.;
      *APCAN  = *APCAN  * DAYL * (double)3600.;
      *PTCAN  = *PTCAN  * DAYL * (double)3600.;
      *ATCAN  = *ATCAN  * DAYL * (double)3600.;
      *PESOIL = *PESOIL * DAYL * (double)3600.;
      *AESOIL = *AESOIL * DAYL * (double)3600.;
      *DIFS   = *DIFS;
      *DIFSU  = *DIFSU;
      *DIFSH  = *DIFSH;
      *DAPAR  = *DAPAR  * DAYL * (double)3600.;
          
      //DTGA = DTGA * DAYL;

      
     return *APCAN;//DTGA;
    }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// kdiff
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*----------------------------------------------------------------------*
*  Purpose: This subroutine calculates extinction coefficient for      *
*           diffuse radiation.                                         *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  LAI     R4  Total leaf area index                      m2 m-2    I  *
*  BL      R4  Leaf angle (from horizontal)               radians   I  *
*  SCP     R4  Leaf scattering coefficient                -         I  *
*  KDP     R4  Diffuse radiation extinction coefficient   m2 m-2    O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double kdiff(double vLAI, double BL, double SCP)
      {
        //PI    = 3.141592654
        double KB15,KB45,KB75,KDP;

         //%---extinction coefficient of beam lights from 15, 45 and 75o elevations
         KB15 = (double)kbeam(sin(15.*PI/180.),BL);
         KB45 = (double)kbeam(sin(45.*PI/180.),BL);
         KB75 = (double)kbeam(sin(75.*PI/180.),BL);

         //%---diffuse light extinction coefficient
         KDP   = (double)-1./vLAI*log((double)0.178*exp(-KB15*pow((double)1.-SCP,(double)0.5)*vLAI)
                         +(double)0.514*exp(-KB45*pow((double)1.-SCP,(double)0.5)*vLAI)
                         +(double)0.308*exp(-KB75*pow((double)1.-SCP,(double)0.5)*vLAI));

       return KDP;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// kbeam
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*----------------------------------------------------------------------*
*  Purpose: This subroutine calculates extinction coefficient for      *
*           direct beam radiation.                                     *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SINB    R4  Sine of solar elevation                    -         I  *
*  BL      R4  Leaf angle (from horizontal)               radians   I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    O  *
*----------------------------------------------------------------------*/
double kbeam(double SINB,double BL)
      {
       double B,OAV,KB;

        //%---solar elevation in radians
        B      = asin(SINB);

       //%---average projection of leaves in the direction of a solar beam
       if (SINB >= sin(BL))
          OAV = SINB*cos(BL);
       else
          OAV = (double)2./(double)3.141592654*(SINB*cos(BL)*asin(tan(B)/tan(BL))
               + pow(pow(sin(BL),(double)2)-pow(SINB,(double)2),(double)0.5));

       //%---beam radiation extinction coefficient
       KB     = OAV/SINB;

       return KB;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// refl
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates reflection coefficients.        *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SCP     R4  Leaf scattering coefficient                -         I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    I  *
*  KBP     R4  Scattered beam radiation extinction coeff. m2 m-2    O  *
*  PCB     R4  Canopy beam radiation reflection coeff.    -         O  *
*----------------------------------------------------------------------*/
double refl(double SCP, double KB)
      {
       double PH,PCB;

        //%---canopy reflection coefficient for horizontal leaves
        PH  = ((double)1.-sqrt((double)1.-SCP))/((double)1.+sqrt((double)1.-SCP));

       //%---Canopy beam radiation reflection coefficient
        PCB = (double)1.-exp((double)-2.*PH*KB/((double)1.+KB));

       return PCB;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ligab
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates absorbed light for sunlit and   *
*           shaded leaves.                                             *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SCP     R4  Leaf scattering coefficient                -         I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    I  *
*  KBP     R4  Scattered beam radiation extinction coeff. m2 m-2    I  *
*  KDP     R4  Diffuse radiation extinction coefficient   m2 m-2    I  *
*  PCB     R4  Canopy beam radiation reflection coeff.    -         I  *
*  PCD     R4  Canopy diffuse radiation reflection coeff. -         I  *
*  IB0     R4  Incident direct-beam radiation             J m-2 s-1 I  *
*  ID0     R4  Incident diffuse radiation                 J m-2 s-1 I  *
*  LAI     R4  (green)Leaf area index                     m2 m-2    I  *
*  ISU     R4  Absorbed radiation by sunlit leaves        J m-2 s-1 O  *
*  ISH     R4  Absorbed radiation by shaded leaves        J m-2 s-1 O  *
*----------------------------------------------------------------------*/
double ligab(double SCP,double KB,double KBP,double KDP,double PCB,double PCD, 
             double IB0,double ID0,double LAI, int IW)
      {
       double IC,ISU,ISH,RV;
        //%---total absorbed light by canopy
        IC     = (1.-PCB)*IB0*(1.-exp(-KBP*LAI))
                +(1.-PCD)*ID0*(1.-exp(-KDP*LAI));

       //%---absorbed light by sunlit and shaded fractions of canopy
        ISU    = (1.-SCP)*IB0*(1.-exp(-KB *LAI))+(1.-PCD)*ID0/(KDP+KB)*
                 KDP*(1.-exp(-(KDP+KB)*LAI))+IB0*((1.-PCB)/(KBP+KB)*KBP*
                 (1.-exp(-(KBP+KB)*LAI))-(1.-SCP)*(1.-exp(-2.*KB*LAI))/2.);
        ISH    = IC-ISU;

      if (IW==0) RV = ISU;
      else       RV = ISH;

       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// pan
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates photosynthetically active       *
*           nitrogen content for sunlit and shaded parts of canopy.    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  SLNT    R4  Top-leaf nitrogen content                  g m-2     I  *
*  SLNMIN  R4  Minimum or base SLNT for photosynthesis    g m-2     I  *
*  LAI     R4  (green)Leaf area index                     m2 m-2    I  *
*  KN      R4  Leaf nitrogen extinction coefficient       m2 m-2    I  *
*  KB      R4  Direct beam radiation extinction coeff.    m2 m-2    I  *
*  NPSU    R4  Photosynthet. active N for sunlit leaves   g m-2     O  *
*  NPSH    R4  Photosynthet. active N for shaded leaves   g m-2     O  *
*----------------------------------------------------------------------*/
double pan(double SLNT, double SLNMIN, double LAI, double KN, double KB, int IW)
      {
       double NPC,NPSU,NPSH,RV;
       //%---total photosynthetic nitrogen in canopy
       NPC   = SLNT*(1.-exp(-KN*LAI))/KN-SLNMIN*LAI;

       //%---photosynthetic nitrogen for sunlit and shaded parts of canopy
       NPSU  = SLNT*(1.-exp(-(KN+KB)*LAI))/(KN+KB)
              -SLNMIN*(1.-exp(-KB*LAI))/KB;
       NPSH  = NPC-NPSU;

       if (IW==0) RV = NPSU;
       else       RV = NPSH;

       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// difla
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates leaf(canopy)-air temperature    *
*           differential.                                              *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 I  *
*  PT      R4  Potential leaf transpiration               mm s-1    I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  DIF     R4  Leaf-air temperature difference            oC        O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double difla(double NRADC,double PT,double RBH,double RT)
      {
       double DIF;

       double LHVAP  = 2.4e+6; //latent heat of water vaporization (J/kg)
       double VHCA   = 1200.;  //volumetric heat capacity (J/m3/oC)

        DIF = LIMIT(-25., 25., (NRADC-LHVAP*PT)*(RBH+RT)/VHCA);

       return DIF;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Potential Leaf Photosynthesis
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates potential leaf photosynthesis    *
*          and transpiration.                                          *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  FRAC    R4  Fraction of leaf classes (sunlit vs shaded) -        I  *
*  DAYTMP  R4  Air temperature                            oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  C3C4    R4  Crop type (=1. for C3, -1 for C4 crops)    -         I  *
*  FVPD    R4  Slope for linear effect of VPD on Ci/Ca    (kPa)-1   I  *
*  PAR     R4  Absorbed photosynth. active radiation      J m-2 s-1 I  *
*  NP      R4  Photosynthetically active N content        g m-2     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  ATRJ    R4  Absorbed global radiation                  J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax  & leaf N  umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  PLF     R4  Potential leaf photosynthesis              gCO2/m2/s O  *
*  PT      R4  Potential leaf transpiration               mm s-1    O  *
*  RSW     R4  Potential stomatal resistance to water     s m-1     O  *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 O  *
*  SLOPEL  R4  Slope of saturated vapour pressure curve   kPa oC-1  O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int  PotentialLeafPhotosynthesis_GECROS(double FRAC,double DAYTMP,double DVP,double CO2A,double C3C4,
                                double FVPD,double PAR,double NP,double RBW,double RBH,double RT,
                                        double ATRJ,double ATMTR,double EAJMAX,double XVN,double XJN,double THETA, 
                                         double *PLF,double *PT,double *RSW,double *NRADC,double *SLOPEL)
    {
     double SVP,SVPL,VPD,SLOPE,FRSW,FPLF,FLRD,FNRADC,FPT,FDIF,TLEAF,CO2I,FCO2I,LRD;
      
        //%---first-round calculation to determine leaf temperature
      FCO2I   = internalCO2(DAYTMP,DVP,FVPD,CO2A,C3C4);
      FPLF    = photo(C3C4,PAR,DAYTMP,FCO2I,NP,EAJMAX,XVN,XJN,THETA);
      FLRD    = darkr(DAYTMP,NP,XVN);

      SVP     = (double)0.611*exp((double)17.4*DAYTMP/(DAYTMP+(double)239.));
      VPD     = max((double)0., SVP-DVP);
      SLOPE   = (double)4158.6*SVP/pow(DAYTMP+(double)239.,(double)2);

      FRSW    = gcrsw(FPLF,FLRD,DAYTMP,CO2A,FCO2I,RBW,RT);
      FNRADC  = NetLeafAbsRad(ATRJ,ATMTR,FRAC,DAYTMP,DVP);
      FPT     = PotEvaporTransp_GECROS(FRSW,RT,RBW,RBH,ATRJ,ATMTR,FRAC,DAYTMP,DVP,SLOPE,VPD);
      FDIF    = difla(FNRADC,FPT,RBH,RT); 

      TLEAF   = DAYTMP + FDIF;

      //%---second-round calculation to determine potential photosynthesis and transpiration
      CO2I    = internalCO2(TLEAF,DVP,FVPD,CO2A,C3C4);
      *PLF    = photo(C3C4,PAR,TLEAF,CO2I,NP,EAJMAX,XVN,XJN,THETA);
      LRD     = darkr(TLEAF,NP,XVN);

      SVPL    = (double)0.611*exp((double)17.4*TLEAF/(TLEAF+(double)239.));
      *SLOPEL = (SVPL-SVP)/NOTNUL(TLEAF-DAYTMP);

      *RSW    = gcrsw(*PLF,LRD,TLEAF,CO2A,CO2I,RBW,RT);

      *NRADC  = NetLeafAbsRad(ATRJ,ATMTR,FRAC,TLEAF,DVP);
      *PT     = PotEvaporTransp_GECROS(*RSW,RT,RBW,RBH,ATRJ,ATMTR,FRAC,TLEAF,DVP,*SLOPEL,VPD);

     return 1;
    }

      
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Potential Evaporation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* SUBROUTINE PEVAP                                                     *
* Purpose: This subroutine calculates potential soil evaporation.      *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DAYTMP  R4  Air temperature                            oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  RSS     R4  Soil resistance,equivalent to leaf stomata s m-1     I  *
*  RTS     R4  Turbulence resistance for soil             s m-1     I  *
*  RBWS    R4  Soil boundary layer resistance to water    s m-1     I  *
*  RBHS    R4  Soil boundary layer resistance to heat     s m-1     I  *
*  ATRJS   R4  Absorbed global radiation by soil          J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  PT1     R4  Potential leaf transpiration using water   mm s-1    I  *
*              from upper evaporative soil layer                       *
*  WSUP1   R4  Water supply from upper evaporative soil   mm s-1    I  *
*              layer for evapotranspiration                            *
*  PESOIL  R4  Potential soil evaporation                 mm s-1    O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double PotentialEvaporation_GECROS(double DAYTMP,double DVP,double RSS,double RTS,double RBWS,double RBHS,
                                   double ATRJS,double ATMTR,double PT1,double WSUP1, double *NRADS)
      {
       double SVP,VPD,SLOPE,SVPS,SLOPES,FPESOL,FAESOL,FDIFS,TAVS;
       double FPE,FNRADS,PE,PESOIL;
        //%--- first-round calculation to estimate soil surface temperature (TAVS)
        SVP    = (double)0.611*exp((double)17.4*DAYTMP/(DAYTMP+(double)239.));
        VPD    = max((double)0., SVP-DVP);
        SLOPE  = (double)4158.6 * SVP/pow(DAYTMP + (double)239.,(double)2);
        FNRADS = NetLeafAbsRad(ATRJS,ATMTR,1.,DAYTMP,DVP);
        FPE    = PotEvaporTransp_GECROS(RSS,RTS,RBWS,RBHS,ATRJS,ATMTR,1.,DAYTMP,DVP,SLOPE,VPD);
        FPESOL = max((double)0., FPE);
        FAESOL = min(FPESOL,FPESOL/(PT1+FPESOL)*WSUP1);
        FDIFS  = difla(FNRADS,FAESOL,RBHS,RTS);
        TAVS   = DAYTMP + FDIFS;

        //%---second-round calculation to estimate potential soil evaporation
        SVPS   = (double)0.611*exp((double)17.4*TAVS/(TAVS+(double)239.));
        SLOPES = (SVPS-SVP)/NOTNUL(FDIFS);

        *NRADS = NetLeafAbsRad(ATRJS,ATMTR,1.,TAVS,DVP);
        PE     = PotEvaporTransp_GECROS(RSS,RTS,RBWS,RBHS,ATRJS,ATMTR,1.,TAVS,DVP,SLOPES,VPD);
        PESOIL = max((double)0., PE);

       return PESOIL;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Potential Evapotranspiration
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates leaf transpiration, using the    *
*          Penman-Monteith equation                                    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  RSW     R4  Potential stomatal resistance to water     s m-1     I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  ATRJ    R4  Absorbed global radiation                  J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  FRAC    R4  Fraction of leaf classes (sunlit vs shaded)-         I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  SLOPE   R4  Slope of saturated vapour pressure curve   kPa oC-1  I  *
*  VPD     R4  Saturation vapour pressure deficit of air  kPa       I  *
*  PT      R4  Potential leaf transpiration               mm s-1    O  *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double PotEvaporTransp_GECROS(double RSW,double RT,double RBW,double RBH,double ATRJ,double ATMTR,
                              double FRAC,double TLEAF,double DVP,double SLOPE,double VPD)
      { 
       double RV,PT,NRADC,PSR,PTR,PTD;

        //%---some physical constants
       double LHVAP  = 2.4E+6;    //latent heat of water vaporization(J/kg)
       double VHCA   = 1200.;     //volumetric heat capacity (J/m3/oC)
       double PSYCH  = 0.067;     //psychrometric constant (kPa/oC)

        // *---net absorbed radiation
        NRADC  = NetLeafAbsRad(ATRJ,ATMTR,FRAC,TLEAF,DVP);

        //%---intermediate variable related to resistances
        PSR    = PSYCH*(RBW+RT+RSW)/(RBH+RT);

        //%---radiation-determined term
        PTR    = NRADC*SLOPE        /(SLOPE+PSR)/LHVAP;

        //%---vapour pressure-determined term
        PTD    = (VHCA*VPD/(RBH+RT))/(SLOPE+PSR)/LHVAP;

        //%---potential evaporation or transpiration
        PT     = max((double)1.E-10,PTR+PTD);

        //%---return value
        RV     = PT;
       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Net Leaf Absorbed Radiation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates net leaf absorbed radiation      *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  ATRJ    R4  Absorbed global radiation                  J m-2 s-1 I  *
*  ATMTR   R4  Atmospheric transmissivity                 -         I  *
*  FRAC    R4  Fraction of leaf classes (sunlit vs shaded)-         I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double NetLeafAbsRad(double ATRJ,double ATMTR,double FRAC,double TLEAF,double DVP)
      {
       double RV,CLEAR,BBRAD,RLWN,NRADC;

        //%---Stefan-Boltzmann constant(J/m2/s/K4)
       double BOLTZM = 5.668E-8;  

        //%---net absorbed radiation
        CLEAR  = max((double)0., min((double)1., (ATMTR-(double)0.25)/(double)0.45));//sky clearness
        BBRAD  = BOLTZM*pow(TLEAF +(double)273.,(double)4);
        RLWN   = BBRAD*((double)0.56-(double)0.079*sqrt(DVP*(double)10.))*((double)0.1
                 +(double)0.9*CLEAR)*FRAC;
        NRADC  = ATRJ - RLWN;

        //%---return value
        RV = NRADC;
 
       return RV;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// internal CO2  
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates the internal CO2 concentration  *
*           as affected by vapour pressure deficit.                    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  FVPD    R4  Slope for linear effect of VPDL on Ci/Ca   (kPa)-1   I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  C3C4    R4  Crop type (=1. for C3, -1 for C4 crops)    -         I  *
*  SVPL    R4  Saturated vapour pressure of leaf          kPa       O  *
*  CO2I    R4  intercellular CO2 concentration            ml m-3    O  *
*----------------------------------------------------------------------*/
double internalCO2(double TLEAF,double DVP,double FVPD,double CO2A,double C3C4)
      {
       double SVPL,VPDL,KMC25,KMO25,O2,EAVCMX,EAKMC,EAKMO,EARD,RDVX25;
       double KMC,KMO,GAMMAX,RDVCX,GAMMA0,GAMMA,RCICA,CO2I;
        
        //%---air-to-leaf vapour pressure deficit
        SVPL   = (double)0.611*exp((double)17.4*TLEAF/(TLEAF+(double)239.));
        VPDL   = max((double)0., SVPL - DVP);

        //%---Michaelis-Menten const. for CO2 at 25oC (umol/mol)
        KMC25  = INSW(C3C4,(double)650.,(double)404.9); //greater KMC25 for C4 than C3

        //%---Michaelis-Menten const. for O2 at 25oC (mmol/mol)
        KMO25  = INSW(C3C4,(double)450.,(double)278.4); //greater KMO25 for C4 than C3

        //%---CO2 compensation point in absence of dark respiration (GAMMAX)
        O2     = (double) 210.;    //oxygen concentration(mmol/mol)
        EAVCMX = (double)65330.;   //energy of activation for Vcmx(J/mol)
        EAKMC  = (double)79430.;   //energy of activation for KMC (J/mol)
        EAKMO  = (double)36380.;   //energy of activation for KMO (J/mol)
        EARD   = (double)46390.;   //energy of activation for dark respiration(J/mol)
        RDVX25 = (double)0.0089;   //ratio of dark respiration to Vcmax at 25oC

        KMC    = KMC25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMC/(double)8.314);
        KMO    = KMO25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMO/(double)8.314);
        GAMMAX = (double)0.5*exp(-(double)3.3801+(double)5220./(TLEAF+(double)273.)/(double)8.314)*O2*KMC/KMO;

        //%---CO2 compensation point (GAMMA)
        RDVCX  = RDVX25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*(EARD-EAVCMX)/(double)8.314);
        GAMMA0 = (GAMMAX+RDVCX*KMC*((double)1.+O2/KMO))/((double)1.-RDVCX);
        GAMMA  = INSW (C3C4, GAMMA0/(double)10., GAMMA0);

        //%---internal/ambient CO2 ratio, based on data of Morison & Gifford (1983)
        RCICA  = (double)1.-((double)1.-GAMMA/CO2A)*((double)0.14+FVPD*VPDL);

        //%---intercellular CO2 concentration
        CO2I   = RCICA * CO2A;
      
       return CO2I;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Leaf CO2 conductance (GC) and stomatal resistance to water (RSW)
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates overall leaf conductance        *
*           for CO2 (GC) and the stomatal resistance to water (RSW).   *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  PLEAF   R4  Gross leaf photosynthesis                  gCO2/m2/s I  *
*  RDLEAF  R4  Leaf dark respiration                      gCO2/m2/s I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  CO2I    R4  Internal CO2 concentration                 ml m-3    I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RSW     R4  Potential stomatal resistance to water     s m-1     O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double gcrsw(double PLEAF,double RDLEAF,double TLEAF,double CO2A,double CO2I,double RBW,double RT)
      {
       double GC,RSW;

        //%---potential conductance for CO2
        GC  = (PLEAF-RDLEAF)*((double)273.+TLEAF)/(double)0.53717/(CO2A-CO2I);

        //%---potential stomatal resistance to water
        RSW = max((double)1E-10, (double)1./GC-RBW*(double)1.3-RT)/(double)1.6;
        
       return RSW;
      }

      
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Photosyntesis Model 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
*  Purpose: This subroutine calculates leaf photosynthesis, based on   *
*           a renewed Farquhar biochemistry (cf Yin et al.2004.        *
*           Plant, Cell & Environment 27:1211-1222)                    *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  C3C4    R4  Crop type (=1. for C3, -1. for C4 crops)   -         I  *
*  PAR     R4  Leaf absorbed photosynth. active radiance  J m-2 s-1 I  *
*  TLEAF   R4  Leaf temperature                           oC        I  *
*  CO2I    R4  Intercellular CO2 concentration            ml m-3    I  *
*  NP      R4  Photosynthetically active leaf N content   g m-2     I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax  & leaf N  umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  PLEAF   R4  Gross leaf photosynthesis                  gCO2/m2/s O  *
*  RDLEAF  R4  Leaf dark respiration                      gCO2/m2/s O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

double photo(double C3C4,double PAR,double TLEAF,double CO2I,double NP,
                   double EAJMAX,double XVN,double XJN,double THETA)
      {
       double KMC25,KMO25,O2,EAVCMX,EAKMC,EAKMO,DEJMAX,SJ,PHI2M,HH,UPAR,KMC,KMO,GAMMAX;
       double VCT,JT,VCMX,JMAX,FPSEUD,ZZ,CC,SF,FQ,FCYC,ALPHA2,X,J2,VC,VJ,ALF,PLEAF;
	   double KTMP;

        //%---Michaelis-Menten constants for CO2 and O2 at 25oC
        if(C3C4 < (double)0.)
        {
         KMC25  = (double)650.;   //greater KMC25 for C4 than C3; unit:(umol/mol)
         KMO25  = (double)450.;   //greater KMO25 for C4 than C3; unit:(mmol/mol)
        }
        else
      {
         KMC25  = (double)404.9;  //unit:(umol/mol)
         KMO25  = (double)278.4;  //unit:(mmol/mol)
      }
      

        //%---other constants related to the Farquhar-type photosynthesis model
        O2     = (double)210.;    //oxygen concentration(mmol/mol)
        EAVCMX = (double)65330.;  //energy of activation for Vcmx(J/mol)
        EAKMC  = (double)79430.;  //energy of activation for KMC (J/mol)
        EAKMO  = (double)36380.;  //energy of activation for KMO (J/mol)
        DEJMAX = (double)200000.; //energy of deactivation for JMAX (J/mol)
 //     DEJMAX = (double)152044.; //energy of deactivation for JMAX (J/mol) - Müller et al., Ecol. Modelling 183 (2005), 183–210
        SJ     = (double)650.;    //entropy term in JT equation (J/mol/K)
        PHI2M  = (double)0.85;    //maximum electron transport efficiency of PS II
        HH     = (double)3.;      //number of protons required to synthesise 1 ATP

		KTMP   = (double)1.;

        //%---PAR photon flux in umol/m2/s absorbed by leaf photo-systems
        UPAR   = (double)4.56*PAR;//4.56 conversion factor in umol/J

        //%---Michaelis-Menten constants for CO2 and O2 respectively
        KMC    = KMC25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMC/(double)8.314);
        KMO    = KMO25*exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAKMO/(double)8.314);

        //%---CO2 compensation point in the absence of dark respiration
        GAMMAX = (double)0.5*exp(-(double)3.3801+(double)5220./(TLEAF+(double)273.)/(double)8.314)*O2*KMC/KMO;

        //%---Arrhenius function for the effect of temperature on carboxylation
        VCT    = exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAVCMX/(double)8.314);

        //%---function for the effect of temperature on electron transport
        JT     = exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EAJMAX/(double)8.314)*
                ((double)1.+exp(SJ/(double)8.314-DEJMAX/(double)298./(double)8.314))/
                ((double)1.+exp(SJ/(double)8.314-(double)1./(TLEAF+(double)273.)*DEJMAX/(double)8.314));

		//if (TLEAF < (double)5) KTMP = max((double)0.0001,TLEAF*TLEAF*TLEAF*TLEAF/(double)625);
		//if (TLEAF < (double)5) KTMP = max((double)0.0001,TLEAF*TLEAF/(double)25);
		//if (TLEAF <=(double)0) KTMP = (double)0.0001;

        //%---maximum rates of carboxylation(VCMX) and of electron transport(JMAX)
        VCMX   = XVN*VCT*NP*KTMP;
        JMAX   = XJN*JT *NP*KTMP;

        //%---CO2 concentration at carboxylation site & electron pathways and their stoichiometries
        FPSEUD = (double)0.;           //assuming no pseudocyclic e- transport
        if (C3C4 < (double)0.)
        {
         ZZ   = (double)0.2;          //CO2 leakage from bundle-sheath to mesophyll
         CC   = (double)10.*CO2I;     //to mimic C4 CO2 concentrating mechanism
         SF   = (double)2.*(CC-GAMMAX)/((double)1.-ZZ);
         FQ   = (double)1.-FPSEUD-(double)2.*((double)4.*CC+(double)8.*GAMMAX)/HH/(SF+(double)3.*CC+(double)7.*GAMMAX);
         FCYC = FQ;
      }
        else
      {
         CC   = CO2I;
         SF   = (double)0.;
         FQ   = (double)0.;
         FCYC = (double)1.-(FPSEUD*HH*(SF+(double)3.*CC+(double)7.*GAMMAX)/((double)4.*CC+(double)8.*GAMMAX)+1.)/
                           (HH*(SF+(double)3.*CC+(double)7.*GAMMAX)/((double)4.*CC+(double)8.*GAMMAX)-(double)1.);
        }

       //%--- electron transport rate in dependence on PAR photon flux
       ALPHA2 = ((double)1.-FCYC)/((double)1.+((double)1.-FCYC)/PHI2M);
       X      = ALPHA2*UPAR/max((double)1.E-10,JMAX);
       J2     = JMAX*((double)1.+X-pow(pow((double)1.+X,(double)2)-(double)4.*X*THETA,(double)0.5))/2./THETA;

       //%---rates of carboxylation limited by Rubisco and electron transport
       VC     = VCMX * CC/(CC + KMC*(O2/KMO+(double)1.));
       VJ     =   J2 * CC*((double)2.+FQ-FCYC)/HH/(SF+(double)3.*CC+(double)7.*GAMMAX)/((double)1.-FCYC);

       //%---gross rate of leaf photosynthesis
       ALF    = ((double)1.-GAMMAX/CC)*min(VC,VJ);
       PLEAF  = max((double)1.E-10, ((double)1.E-6)*44.*ALF);

      return PLEAF;
     }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Dark Respiration Model 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
double darkr(double TLEAF,double NP,double XVN)
      {
       double EARD,RDVX25,RDT,RDLEAF;
        
        //%---rate of leaf dark respiration
        EARD   = (double)46390.;  //energy of activation for dark respiration(J/mol)
        RDVX25 = (double)0.0089;  //ratio of dark respiration to Vcmax at 25oC
        RDT    = exp(((double)1./(double)298.-(double)1./(TLEAF+(double)273.))*EARD/(double)8.314);
        RDLEAF = ((double)1.E-6)*(double)44. *RDVX25*(XVN*NP) * RDT;
        
       return RDLEAF;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Actual Leaf Photosynthesis
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*---------------------------------------------------------------------*
* Purpose: This subroutine calculates actual leaf photosynthesis when  *
*          water stress occurs.                                        *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DAYTMP  R4  Air temperature                            oC        I  *
*  PAR     R4  Absorbed photosynth. active radiation      J m-2 s-1 I  *
*  DVP     R4  Vapour pressure                            kPa       I  *
*  CO2A    R4  Ambient CO2 concentration                  ml m-3    I  *
*  C3C4    R4  Crop type (=1. for C3, -1 for C4 crops)    -         I  *
*  FVPD    R4  Slope for linear effect of VPD on Ci/Ca    (kPa)-1   I  *
*  NRADC   R4  Net leaf absorbed radiation                J m-2 s-1 I  *
*  AT      R4  Actual leaf transpiration                  mm s-1    I  *
*  PT      R4  Potential leaf transpiration               mm s-1    I  *
*  RT      R4  Turbulence resistance                      s m-1     I  *
*  RBH     R4  Leaf boundary layer resistance to heat     s m-1     I  *
*  RBW     R4  Leaf boundary layer resistance to water    s m-1     I  *
*  RSW     R4  Potential stomatal resistance to water     s m-1     I  *
*  SLOPEL  R4  Slope of saturated vapour pressure curve   kPa oC-1  I  *
*  NP      R4  Photosynthet. active leaf N content        g m-2     I  *
*  NPN     R4  NP with small plant-N increment            g m-2     I  *
*  EAJMAX  R4  Energy of activation for Jmax              J mol-1   I  *
*  XVN     R4  Slope of linearity between Vcmax & leaf N  umol/g/s  I  *
*  XJN     R4  Slope of linearity between Jmax  & leaf N  umol/g/s  I  *
*  THETA   R4  Convexity for light response of e-transport   -      I  *
*  PLFAS   R4  Actual leaf photosynthesis                 gCO2/m2/s O  *
*  PLFAN   R4  PLFAS with small plant-N increment         gCO2/m2/s O  *
*  ADIF    R4  Actual leaf-air temperature difference     oC        O  *
*----------------------------------------------------------------------*/
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int ActualLeafPhotosynthesis_GECROS(double DAYTMP,double PAR,double DVP,double CO2A,double C3C4,
                                double FVPD,double NRADC,double AT,double PT,double RT,
                                double RBH,double RBW,double RSW,double SLOPEL,double NP,
                                double NPN,double EAJMAX,double XVN,double XJN,double THETA, 
                                 double *PLFAS,double *PLFAN,double *ADIF)
    {
     double PSYCH,ATLEAF,ARSW,ACO2I,APLFN,ARDN,APLF,ARD;

      PSYCH  = (double)0.067;    //psychrometric constant (kPa/oC)

      //%---leaf temperature if water stress occurs
      *ADIF   = difla(NRADC,AT,RBH,RT);
      ATLEAF = DAYTMP + *ADIF;

      //%---stomatal resistance to water if water stress occurs
      ARSW   = (PT-AT)*(SLOPEL*(RBH+RT)+PSYCH*(RBW+RT))/AT/PSYCH+PT/AT*RSW;

      //%---potential photosynthesis at the new leaf temperature
      ACO2I  = internalCO2(ATLEAF,DVP,FVPD,CO2A,C3C4);
        // SVPA noetig???
      APLFN  = photo(C3C4,PAR,ATLEAF,ACO2I,NPN,EAJMAX,XVN,XJN,THETA);
      ARDN   = darkr(ATLEAF,NPN,XVN);

      APLF   = photo(C3C4,PAR,ATLEAF,ACO2I,NP,EAJMAX,XVN,XJN,THETA);
      ARD    = darkr(ATLEAF,NP,XVN);

      //%---actual photosynthesis under water stress condition
      *PLFAS  = ((double)1.6*RSW+(double)1.3*RBW+RT)/((double)1.6*ARSW+(double)1.3*RBW+RT)*(APLF-ARD)+ARD;
      *PLFAN  = ((double)1.6*RSW+(double)1.3*RBW+RT)/((double)1.6*ARSW+(double)1.3*RBW+RT)*(APLFN-ARDN)+ARDN;

     return 1;
    }      

/*----------------------------------------------------------------------*
*  SUBROUTINE BETAF                                                    *
*  Purpose: This subroutine calculates the dynamics of expected growth *
*           of sinks, based on the beta sigmoid growth equation        *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DVR     R4  Development rate                            d-1      I  *
*  TE      R4  Stage at which sink growth stops            -        I  *
*  TX      R4  Stage at which sink growth rate is maximal  -        I  *
*  TI      R4  Stage of a day considered                   -        I  *
*  FD      R4  Relative expected growth of a sink at a day d-1      O  *
*----------------------------------------------------------------------*/
double betaf(double DVR, double TE, double TX, double TI)
{
 double FD;
  
 FD = DVR*(2.*TE-TX)*(TE-TI)/TE/(TE-TX)/(TE-TX)*abspowerDBL((TI/TE),(TX/(TE-TX)));

return FD;
}

/*----------------------------------------------------------------------*
*  SUBROUTINE SINKG                                                    *
*  Purpose: This subroutine calculates carbon demand for sink growth.  *
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  DS      R4  Development stage                           -        I  *
*  SSG     R4  Stage at which sink growth starts           -        I  *
*  TOTC    R4  Total carbon in a sink at end of its growth g C/m2   I  *
*  YG      R4  Growth efficiency                           g C/g C  I  *
*  FD      R4  Relative expected growth of a sink at a day d-1      I  *
*  DCDR    R4  Shortfall of C demand in previous days      g C/m2   I  *
*  DCS     R4  Daily C supply for sink growth              g C/m2/d I  *
*  DELT    R4  Time step of integration                    d        I  *
*  DCDC    R4  C demand of the current day                 g C/m2/d O  *
*  DCD     R4  Daily C demand for sink growth              g C/m2/d O  *
*  FLWC    R4  Flow of current assimilated C to sink       g C/m2/d O  *
*----------------------------------------------------------------------*/
int sinkg(double DS, double SSG,double TOTC,double YG,double FD,double DCDR,double DCS,double DELT,
              double *DCDC,double *DCD,double *FLWC)
{
 //%---expected demand for C of the current time step
      *DCDC   = INSW (DS-SSG, 0., TOTC/YG*FD);

 //%---total demand for C at the time step considered
      *DCD    = *DCDC + max(0.,DCDR)/DELT;

 //%---flow of current assimilated carbon to sink
      *FLWC   = min(*DCD, DCS);

return 1;
}


/*----------------------------------------------------------------------*
*  SUBROUTINE RNACC                                                    *
*  Purpose: This subroutine calculates rate of N accumulation in organs*
*                                                                      *
*  FORMAL PARAMETERS:  (I=input,O=output,C=control,IN=init,T=time)     *
*                                                                      *
*  name   type meaning                                    units  class *
*  ----   ---- -------                                    -----  ----- *
*  FNSH    R4  Fraction of new N partitioned to shoot       -       I  *
*  NUPT    R4  Nitrogen uptake at a time step               gN/m2/d I  *
*  RWST    R4  Rate of stem weight                          g/m2/d  I  *
*  STEMNC  R4  Nitrogen concentration in stem               gN/g    I  *
*  LNCMIN  R4  Minimum N concentration in leaf              gN/g    I  *
*  RNCMIN  R4  Minimum N concentration in root              gN/g    I  *
*  LNC     R4  Nitrogen concentration in leaf               gN/g    I  *
*  RNC     R4  Nitrogen concentration in root               gN/g    I  *
*  NLV     R4  Canopy (green)leaf N content                 gN/m2   I  *
*  NRT     R4  (living)root N content                       gN/m2   I  *
*  WLV     R4  Canopy (green)leaf weight                    g/m2    I  *
*  WRT     R4  (living)Root weight                          g/m2    I  *
*  DELT    R4  Time step of simulation                      d       I  *
*  CB      R4  Factor for initial N concent. of seed-fill   -       I  *
*  CX      R4  Factor for final N concent. of seed-fill     -       I  *
*  TM      R4  DS when transition from CB to CX is fastest  -       I  *
*  DS      R4  Development stage                            -       I  *
*  SEEDNC  R4  Standard seed N concentration                gN/g    I  *
*  RWSO    R4  growth rate of seed                          g/m2/d  I  *
*  LNLV    R4  Loss rate of NLV due to senescence           gN/m2/d I  *
*  LNRT    R4  Loss rate of NRT due to senescence           gN/m2/d I  *
*  RNRT    R4  rate of N accumulation in root               gN/m2/d O  *
*  RNST    R4  rate of N accumulation in stem               gN/m2/d O  *
*  RNLV    R4  rate of N accumulation in leaf               gN/m2/d O  *
*  RTNLV   R4  Positive value of RNLV                       gN/m2/d O  *
*  RNSO    R4  rate of N accumulation in seed(storage organ)gN/m2/d O  *
*----------------------------------------------------------------------*/
int rnacc(double FNSH,double NUPT,double RWST,double STEMNC,double LNCMIN,double RNCMIN,
              double LNC,double RNC,double NLV,double NRT,double WLV,double WRT,double DELT,
              double CB,double CX,double TM,double DS,double SEEDNC,double RWSO,double LNLV,
              double LNRT,double *RNRT,double *RNST,double *RNLV,double *RTNLV,double *RNSO)
{
 double NSHN,NLVA,NRTA,NTA;
 double CDS,ENSNC,NGS,NONC;
 double NLVN,GNLV;
 double NRTN,GNRT;
 
      //%---amount of N partitioned to shoot
      NSHN   = FNSH * NUPT;

      //%---leaf N (NLVA) or root N (NRTA) available for remobilization
      NLVA   = INSW(LNCMIN-LNC, NLV-WLV*LNCMIN, 0.) / DELT;
      NRTA   = INSW(RNCMIN-RNC, NRT-WRT*RNCMIN, 0.) / DELT;
      NTA    = NLVA + NRTA;

      //%---rate of N accumulation in stem
      *RNST   = RWST * INSW(-NTA,STEMNC,0.);

      //%---expected N dynamics during seed(storage organ) filling
      CDS    = CB+(CX-CB)*(4.-TM-DS)/(2.-TM)*abspowerDBL((DS-1.),(1./(2.-TM)));
      ENSNC  = LIMIT(CB,CX,CDS) * SEEDNC;

      //%---rate of N accumulation in seed
      NGS    = NSHN - *RNST - ENSNC*RWSO;
      NONC   = max(0.,INSW(NTA+NGS,(NTA+NSHN-*RNST)/NOTNUL(RWSO),ENSNC));
      *RNSO   = RWSO*NONC;

      //%---rate of N accumulation in leaf
      NLVN   = INSW(NTA+NGS,-NLVA-LNLV,-NLVA/NOTNUL(NTA)*(-NGS)-LNLV);
      GNLV   = INSW(NGS, NLVN, NSHN-*RNST-*RNSO-LNLV);
      *RNLV   = max(-NLV+1.E-7, GNLV);
      *RTNLV  = max(0., *RNLV);

      //%---rate of N accumulation in root
      NRTN   = INSW(NTA+NGS, NUPT-NSHN-NRTA-LNRT,NUPT-NSHN-NRTA/NOTNUL(NTA)*(-NGS)-LNRT);
      GNRT   = INSW(NGS, NRTN, NUPT-NSHN-LNRT);
      *RNRT   = max(-NRT+5.E-8, GNRT);

return 1;
}

  
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Init_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	  
  int   WINAPI _loadds Init_GECROS(EXP_POINTER)
     {
      //PPLTCARBON   pPltC = pPl->pPltCarbon;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PGENOTYPE    pGen  = pPl->pGenotype;
      PBIOMASS     pPltB = pPl->pBiomass;
      //PDEVELOP     pDev  = pPl->pDevelop;
      PCANOPY      pCan  = pPl->pCanopy;

      //PGECROSBIOMASS    pGPltB = pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON     pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN   pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY     pGCan  = pGecrosPlant->pGecrosCanopy;
      PGECROSPARAMETER  pGPar  = pGecrosPlant->pGecrosParameter;
	  PGECROSSOIL       pGS    = pGecrosPlant->pGecrosSoil;


      double FPRO,FCAR;
      double CFO,YGO,LNCMIN;
      double CLVI,CRTI,NPL,NLVI,NRTI;
      double LAII,SLNBI,RDI,HTI;

	  double WCMIN;

      double DELT = (double)1;


      //input for initialisation
      NPL = (double) pMa->pSowInfo->fPlantDens;

	  if (fGECROS == (float)0) PlantParametersGECROS(pPl->pGenotype->acCropCode,pPl->pGenotype->acVarietyName);
      

	  //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      //      Model Coefficients from readmod  
      //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
	  ///*
	  if(fGECROS !=(float)-99)
	  {
      // Crop type parameters (from readmod marker 80001)
      LEGUME = (double)fparLEGUME;// LEGUME = 1. for leguminous crops;   = -1. for non-leguminous crops.
      C3C4   = (double)fparC3C4;  // C3C4   = 1. for C3 crops;           = -1. for C4 crops.
      DETER  = (double)fparDETER; // DETER  = 1. for determinate crops;  = -1. for indeterminate crops.
      SLP    = (double)fparSLP;   // SLP    = 1. for short-day crops;    = -1. for long-day crops.
      LODGE  = (double)fparLODGE; // LODGE  = 1. for cases with lodging; = -1. for cases without lodging.
      VERN  =  (double)fparVERN; // VERN = 1. vernalization model SPASS; 
	                            //  VERN = 0. vernalization model CERES; =-1. without veralization.
      
      // Vegetative and generative biomass growth parameters (from readmod marker 80002)
      EG     = (double)fparEG;//efficiency germination (Par.)            (-)        crop  table 2, p46
      CFV    = (double)fparCFV;//carbon frac. in veg. organs (Par.)    (g C g-1)    crop  table 2, p46
                           //pGPltC->fCFracVegetative=(float)CFV;;
      YGV    = (double)fparYGV;//growth efficiency of veg.organs(Par.) (g C g-1 C)  crop  table 2, p46
                          //pGPar->fGrwEffVegOrg=(float)YGV;
      FFAT   = (double)fparFFAT;//fat frac.        in storage organ (Par.)   (g g-1)    crop  table 2, p46 
      FLIG   = (double)fparFLIG;//lignin frac. in storage organ (Par.)   (g g-1)    crop  table 2, p46
      FOAC   = (double)fparFOAC;//org.acid frac. in storage organ (Par.) (g g-1)    crop  table 2, p46
      FMIN   = (double)fparFMIN;//minerals frac. in storage organ (Par.) (g g-1)    crop  table 2, p46
      
      //Morphology and phenology parameters (from readmod marker 80003)
      TBD    = (double)fparTBD;//base temperature for phenology          (°C)       crop  table 3, p47 
      TOD    = (double)fparTOD;//optimum temperature for phenology       (°C)       crop  table 3, p47
      TCD    = (double)fparTCD;//ceiling temperature for phenology       (°C)       crop  table 3, p47
      TSEN   = (double)fparTSEN;//curvature for temperature response     (-)        deflt table 4, p49
      LWIDTH = (double)fparLWIDTH;//Leaf width                           (m)        crop  table 3, p47
      RDMX   = (double)fparRDMX;//maximal root depth                     (cm)       crop  table 3, p47
                          // -> pGPar->fRootDepthMax
      CDMHT  = (double)fparCDMHT;//stem dry weight per plant height   (g m-2 m-1)   crop  table 3, p47
      
      //Nitrogen (from readmod marker 80004)
      NUPTX  = (double)fparNUPTX;//maximum crop nitrogen uptake       (g N m-2 d-1) deflt table 4, p49
      //pGPltN->fNUptMax    = fparNUPTX;
      RNCMIN = (double)fparRNCMIN;//min. N conc. in roots             (g N g-1)     crop  table 3, p47
      pPltN->fRootMinConc = fparRNCMIN;
      STEMNC = (double)fparSTEMNC;//nitrogen conc. in stems           (g N g-1)     crop  table 3, p47
      pPltN->fStemActConc = fparSTEMNC;
      SLNMIN = (double)fparSLNMIN;//min. specific leaf N content      (g N m-2)     crop  table 3, p47
      LNCI   = (double)fparLNCI;//initial N conc. in living leaves    (g N g-1)     crop  table 3, p47
      SLA0   = (double)fparSLA0;//specific leaf area constant         (m2 leaf g-1) crop  table 3, p47
      CCFIX  = (double)fparCCFIX;//C cost of symbiotic N fixation     (g C g-1 N)   const table 1, p45
      //pGPltC->fCNFixCost = fparCCFIX;

      //Photoperiod and photosynthesis parameters (from readmod marker 80005)
      INSP   = (double)fparINSP;//Inclination of sun angle for calcu-   
                                //-lating photoperiodic daylength      (degree)     const table 1, p45
      SPSP   = (double)fparSPSP;//Dev.stage start of photo-sens. phase   (-)        deflt table 4, p49
      EPSP   = (double)fparEPSP;//Dev.stage end of photo-sens. phase     (-)        deflt table 4, p49
      EAJMAX = (double)fparEAJMAX;//activation energy of JMAX        (J mol-1)      crop  table 3, p47
      XVN    = (double)fparXVN;//slope of VCMAX vs. leaf N         (umol s-1 g-1 N) crop specific ???     
      XJN    = (double)fparXJN;//slope of VJMAX vs. leaf N         (umol s-1 g-1 N) crop specific ???
      THETA  = (double)fparTHETA;//convexity for light response  
                //electron transport (J2) in photosynthesis              (-)        const table 1, p45 (photo)

      //Genotype specific parameters (from readmod marker 80006)
      SEEDW  = (double)fparSEEDW;//Seed weight                        (g seed-1)   
      SEEDNC = (double)fparSEEDNC;//Standard seed N conc.             (g N g-1)
      BLD    = (double)fparBLD;//Leaf angle from horizontal            (degree)
      HTMX   = (double)fparHTMX;//Maximal crop height                    (m)
      MTDV   = (double)fparMTDV;//Minimum thermal days for veg. phase    (d) 
      MTDR   = (double)fparMTDR;//Minimum thermal days for reprod. phase (d) 
      PSEN   = (double)fparPSEN;//Photoperiod sensitivity 
                                //  (+ for SD, - for LD)                (h-1)

      //Default parameters (from readmod marker 80007)
      PMEH   = (double)fparPMEH;//fraction of sigmoid curve inflexion 
                                //in entire height growth period         (-)        deflt table 4, p49   
      PMES   = (double)fparPMES;//fraction of sigmoid curve inflexion
                                //in entire seed growth period           (-)        deflt table 4, p49
      ESDI   = (double)fparESDI;//ESD for indeterminate crops            (-)        deflt table 4, p49
             //-> ESD = INSW(DETER, ESDI, 1.); = (double)pGPPar->fStageEndSeedNum; 
             //ESD development stage for end of seed number determining period
      WRB    = (double)fparWRB;//critical root weight density    (g m-2 cm-1 depth) const table 1, p45 (others)

	  //if (fGECROS < (float)9) {}

	  //SG20140710: für GECROS im rfs-Modus auskommentiert
	  //fGECROS = (float)-99;

      }//*/ 

      //Parameters for sensitivity-analysis options (from readmod marker 80008)
	  if (fparCO2A ==(float)-99) CO2A = (double)350.0;
	  else CO2A   = (double)fparCO2A;

      //SG 20111114: //CO2-conc. from database (for AgMIP-project)
          CO2A = (pCl->pWeather->fSnow > 0? pCl->pWeather->fSnow:CO2A);
	  
	  if (fparCOEFR ==(float)-99) COEFR = (double)1.0;
	  else COEFR  = (double)fparCOEFR;
	  
	  if (fparCOEFV ==(float)-99) COEFV = (double)1.0;
	  else COEFV  = (double)fparCOEFV;
	  
	  if (fparCOEFT ==(float)-99) COEFT = (double)5.0;
	  else COEFT  = (double)fparCOEFT;
	  
	  if (fparFCRSH ==(float)-99) FCRSH = (double)0.5;
	  else FCRSH  = (double)fparFCRSH;
	  
	  if (fparFNRSH ==(float)-99) FNRSH = (double)0.63;
	  else FNRSH  = (double)fparFNRSH;
	  
	  if (fparPNPRE ==(float)-99) PNPRE = (double)0.7;
	  else PNPRE  = (double)fparPNPRE;
	  
	  if (fparCB ==(float)-99) CB = (double)0.75;
	  else CB  = (double)fparCB;
	  
	  if (fparCX ==(float)-99) CX = (double)1.0;
	  else CX  = (double)fparCX;
	  
	  if (fparTM ==(float)-99) TM = (double)1.5;
	  else TM  = (double)fparTM;

	       
	  
	  //Soil parameters part I (from readmod marker 80009)  
	  if (fparPRFDPT ==(float)-99) pGS->fProfileDepth = (float)150.0;
	  else pGS->fProfileDepth = fparPRFDPT;
      
	  if (fparRSS ==(float)-99) RSS = (double)100.0;
	  else RSS    = (double)fparRSS;
	  //soil resistance, equiv.to stomatal resistance [s m-1]     

	  if (fparSD1 ==(float)-99) SD1 = (double)25.0;
	  else SD1    = (double)fparSD1;
	  	
      if (fparTCP ==(float)-99) TCP = (double)1;
	  else TCP    = (double)fparTCP;

	  if (fparWCMIN ==(float)-99) WCMIN = (double)0.05;
	  else WCMIN    = (double)fparWCMIN;

	  	  
	  if(pGPar->iniGecros==0)
      {
        
      FPRO   = (double)6.25*SEEDNC;
      FCAR   = (double)1.-FPRO-FFAT-FLIG-FOAC-FMIN;
      CFO    = (double)0.444*FCAR + (double)0.531*FPRO + (double)0.774*FFAT
                 +(double)0.667*FLIG +(double)0.368*FOAC;
      YGO    = CFO/((double)1.275*FCAR + (double)1.887*FPRO + (double)3.189*FFAT
                       + (double)2.231*FLIG + (double)0.954*FOAC)* (double)30./(double)12.;

      CLVI   = NPL * SEEDW * CFO * EG * FCRSH;
      CRTI   = NPL * SEEDW * CFO * EG * (1.-FCRSH);

      NLVI   = LNCI* CLVI/CFV;
      NRTI   = NPL * SEEDW * EG * LNCI * FCRSH/FNRSH - NLVI;

      LNCMIN = SLA0*SLNMIN;
      LAII   = CLVI/CFV*SLA0;
      SLNBI  = NLVI/LAII;

      RDI    = max((double)2.,(double)pMa->pSowInfo->fSowDepth);
      //RDI    = max((double)2., SD1);//test
      HTI    = HTMX/(double)1000.;
    
      //output
      pGPltC->fCFracStorage = (float)CFO;
      pGen->fGrwEffStorage  = (float)YGO;

      //pGPltC->fCFracVegetative = (float)CFV;	  
	  //pGPar->fGrwEffVegOrg  = (float)YGV;

      pGPltC->fCLeaf        = (float)CLVI;
      pGPltC->fCStrctRoot   = (float)CRTI;

      pPltN->fLeafCont      = (float)NLVI;
      pGPltN->fNLeafTotCont = (float)NLVI;
//SG20110909
	  pPltN->fRootCont      = (float)(NRTI*10.0); //[g/m2] --> [kg/ha]
//    pPltN->fRootCont      = (float)NRTI; 

      pPltN->fLeafMinConc   = (float)LNCMIN;
      pPltN->fRootMinConc   = (float)RNCMIN;
      pGCan->fLAICdeterm    = (float)LAII;
      pGCan->fLAITotal      = (float)LAII;
      pGCan->fLAIGreen      = (float)LAII;
      pGPltN->fNLeafSpecificContBottom = (float)SLNBI;
      
	  //pPltN->fStemActConc   = (float)STEMNC;

      pPl->pRoot->fDepth    = (float)RDI;
      pCan->fPlantHeight    = (float)HTI;

	  pPltB->fLeafWeight   = (float)0;
      pPltB->fRootWeight   = (float)0;   
      pPltB->fStovWeight   = (float)0;
      pPltB->fTotalBiomass = (float)0;
	  pPltN->fLeafActConc  = (float)0;
      pPltN->fRootActConc  = (float)0;
	  pPltN->fStemActConc  = (float)0;

	  pGPar->iniGecros = (int) 1;
      }
//-------------------------------------------------------------------------------------
  
  return 1;
  }
      
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//    PlantParametersGECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   PlantParametersGECROS(LPSTR species,LPSTR variety)
{
  char spec[30], var[30];
  extern float fparLEGUME,fparC3C4,fparDETER,fparSLP,fparLODGE,fparVERN;
  extern float fparEG,fparCFV,fparYGV,fparFFAT,fparFLIG,fparFOAC,fparFMIN;
  extern float fparTBD,fparTOD,fparTCD,fparTSEN,fparLWIDTH,fparRDMX,fparCDMHT;
  extern float fparLNCI,fparCCFIX,fparNUPTX,fparSLA0,fparSLNMIN,fparRNCMIN,fparSTEMNC;
  extern float fparINSP,fparSPSP,fparEPSP,fparEAJMAX,fparXVN,fparXJN,fparTHETA;
  extern float fparSEEDW,fparSEEDNC,fparBLD,fparHTMX,fparMTDV,fparMTDR,fparPSEN;
  extern float fparPMEH,fparPMES,fparESDI,fparWRB;
  extern float fparPNPRE;

  strcpy(spec,(LPSTR)species);
  strcpy(var, (LPSTR)variety);

  if (!strcmp(spec,"RY")) strcpy(spec,"WH");
  
  if (!strcmp(spec,"WH"))
  {
   fparLEGUME = (float)-1;
   fparC3C4   = (float)+1;
   fparDETER  = (float)+1;
   fparSLP    = (float)+1;
   fparLODGE  = (float)-1;
   fparVERN   = (float)-1;

   //table 2, p.46
   fparYGV    = (float)0.81;
   fparCFV    = (float)0.48;
   fparEG     = (float)0.25;
   fparFFAT   = (float)0.02;
   fparFLIG   = (float)0.06;
   fparFOAC   = (float)0.02;
   fparFMIN   = (float)0.02;

   //table 3, p.47
   fparTBD    = (float)0.0;
   fparTOD    = (float)25.0;
   fparTCD    = (float)37.0;
   fparLWIDTH = (float)0.01;
   fparCDMHT  = (float)460.;
   fparRDMX   = (float)130.;
   fparSLNMIN = (float)0.35;
   fparLNCI   = (float)0.05;
   fparRNCMIN = (float)0.005;
   fparSTEMNC = (float)0.01;
   fparSLA0   = (float)0.028;
   fparEAJMAX = (float)48270;

   //genotype specific	   
   fparSEEDW  = (float)0.4;
   fparSEEDNC = (float)0.05;
   fparBLD    = (float)50.0;
   fparHTMX   = (float)1.2;
   fparMTDV   = (float)40.0;
   fparMTDR   = (float)50.0;
   fparPSEN   = (float)-0.0;
  }

  if (!strcmp(spec,"BA"))
  {
   fparLEGUME = (float)-1;
   fparC3C4   = (float)+1;
   fparDETER  = (float)+1;
   fparSLP    = (float)-1;
   fparLODGE  = (float)-1;
   fparVERN   = (float)-1;

   //table 2, p.46
   fparYGV    = (float)0.81;
   fparCFV    = (float)0.48;
   fparEG     = (float)0.25;
   fparFFAT   = (float)0.01;
   fparFLIG   = (float)0.04;
   fparFOAC   = (float)0.02;
   fparFMIN   = (float)0.04;

   //table 3, p.47
   fparTBD    = (float)0.0;
   fparTOD    = (float)25.0;
   fparTCD    = (float)37.0;
   fparLWIDTH = (float)0.01;
   fparCDMHT  = (float)450;
   fparRDMX   = (float)130.;
   fparSLNMIN = (float)0.3;
   fparLNCI   = (float)0.05;
   fparRNCMIN = (float)0.005;
   fparSTEMNC = (float)0.01;
   fparSLA0   = (float)0.031;
   fparEAJMAX = (float)30200;

   //genotype specific	   
   fparSEEDW  = (float)0.4;
   fparSEEDNC = (float)0.05;
   fparBLD    = (float)50.0;
   fparHTMX   = (float)1.2;
   fparMTDV   = (float)40.0;
   fparMTDR   = (float)50.0;
   fparPSEN   = (float)-0.0;
  }

  if (!strcmp(spec,"MZ"))
  {
   fparLEGUME = (float)-1;
   fparC3C4   = (float)-1;
   fparDETER  = (float)+1;
   fparSLP    = (float)-1;
   fparLODGE  = (float)-1;
   fparVERN   = (float)-1;

   //table 2, p.46
   fparYGV    = (float)0.81;
   fparCFV    = (float)0.48;
   fparEG     = (float)0.25;
   fparFFAT   = (float)0.05;
   fparFLIG   = (float)0.11;
   fparFOAC   = (float)0.01;
   fparFMIN   = (float)0.01;

   //table 3, p.47
   fparTBD    = (float)8.0;
   fparTOD    = (float)30.0;
   fparTCD    = (float)42.0;
   fparLWIDTH = (float)0.05;
   fparCDMHT  = (float)570.;
   fparRDMX   = (float)145.;
   fparSLNMIN = (float)0.25;
   fparLNCI   = (float)0.05;
   fparRNCMIN = (float)0.005;
   fparSTEMNC = (float)0.008;
   fparSLA0   = (float)0.022;
   fparEAJMAX = (float)70890;

   //genotype specific	   
   fparSEEDW  = (float)0.38;
   fparSEEDNC = (float)0.05;
   fparBLD    = (float)50.0;
   fparHTMX   = (float)2.7;
   fparMTDV   = (float)40.0;
   fparMTDR   = (float)50.0;
   fparPSEN   = (float)-0.0;
  }

  if (!strcmp(spec,"PT"))
  {
   fparLEGUME = (float)-1;
   fparC3C4   = (float)+1;
   fparDETER  = (float)+1;
   fparSLP    = (float)+1;
   fparLODGE  = (float)-1;
   fparVERN   = (float)-1;

   //table 2, p.46
   fparYGV    = (float)0.81;
   fparCFV    = (float)0.48;
   fparEG     = (float)0.25;
   fparFFAT   = (float)0.0;
   fparFLIG   = (float)0.03;
   fparFOAC   = (float)0.05;
   fparFMIN   = (float)0.05;

   //table 3, p.47
   fparTBD    = (float)0.0;
   fparTOD    = (float)25.0;
   fparTCD    = (float)37.0;
   fparLWIDTH = (float)0.025;
   fparCDMHT  = (float)170.;
   fparRDMX   = (float)100.;
   fparSLNMIN = (float)0.35;
   fparLNCI   = (float)0.05;
   fparRNCMIN = (float)0.005;
   fparSTEMNC = (float)0.01;
   fparSLA0   = (float)0.033;
   fparEAJMAX = (float)50000;//???

   //genotype specific	   
   fparSEEDW  = (float)0.4;
   fparSEEDNC = (float)0.05;
   fparBLD    = (float)50.0;
   fparHTMX   = (float)1.2;
   fparMTDV   = (float)40.0;
   fparMTDR   = (float)50.0;
   fparPSEN   = (float)-0.0;

  }

  if (!strcmp(spec,"SF"))
  {
   fparLEGUME = (float)-1;
   fparC3C4   = (float)+1;
   fparDETER  = (float)+1;
   fparSLP    = (float)+1;
   fparLODGE  = (float)-1;
   fparVERN   = (float)-1;

   //table 2, p.46
   fparYGV    = (float)0.81;
   fparCFV    = (float)0.48;
   fparEG     = (float)0.45;
   fparFFAT   = (float)0.29;
   fparFLIG   = (float)0.05;
   fparFOAC   = (float)0.03;
   fparFMIN   = (float)0.03;

   //table 3, p.47
   fparTBD    = (float)8.0;
   fparTOD    = (float)30.0;
   fparTCD    = (float)42.0;
   fparLWIDTH = (float)0.1;
   fparCDMHT  = (float)270.;
   fparRDMX   = (float)145.;
   fparSLNMIN = (float)0.3;
   fparLNCI   = (float)0.05;
   fparRNCMIN = (float)0.005;
   fparSTEMNC = (float)0.01;
   fparSLA0   = (float)0.025;
   fparEAJMAX = (float)50000;//???

   //genotype specific	   
   fparSEEDW  = (float)0.4;
   fparSEEDNC = (float)0.05;
   fparBLD    = (float)50.0;
   fparHTMX   = (float)1.2;
   fparMTDV   = (float)40.0;
   fparMTDR   = (float)50.0;
   fparPSEN   = (float)-0.0;

  }

  if (!strcmp(spec,"RP"))
  {
   fparLEGUME = (float)-1;
   fparC3C4   = (float)+1;
   fparDETER  = (float)+1;
   fparSLP    = (float)+1;
   fparLODGE  = (float)-1;
   fparVERN   = (float)-1;

   //table 2, p.46
   fparYGV    = (float)0.81;
   fparCFV    = (float)0.48;
   fparEG     = (float)0.45;
   fparFFAT   = (float)0.48;
   fparFLIG   = (float)0.05;
   fparFOAC   = (float)0.02;
   fparFMIN   = (float)0.04;

   //table 3, p.47
   fparTBD    = (float)0.0;
   fparTOD    = (float)25.0;
   fparTCD    = (float)37.0;
   fparLWIDTH = (float)0.25;
   fparCDMHT  = (float)285.;
   fparRDMX   = (float)130.;
   fparSLNMIN = (float)0.3;
   fparLNCI   = (float)0.05;
   fparRNCMIN = (float)0.005;
   fparSTEMNC = (float)0.01;
   fparSLA0   = (float)0.030;
   fparEAJMAX = (float)50000;//???

   //genotype specific	   
   fparSEEDW  = (float)0.4;
   fparSEEDNC = (float)0.05;
   fparBLD    = (float)50.0;
   fparHTMX   = (float)2.7;
   fparMTDV   = (float)40.0;
   fparMTDR   = (float)50.0;
   fparPSEN   = (float)-0.0;
  }


  if (!strcmp(spec,"BS"))
  {
   fparLEGUME = (float)-1;
   fparC3C4   = (float)+1;
   fparDETER  = (float)+1;
   fparSLP    = (float)+1;
   fparLODGE  = (float)-1;
   fparVERN   = (float)-1;

   //table 2, p.46
   fparYGV    = (float)0.81;
   fparCFV    = (float)0.48;
   fparEG     = (float)0.25;
   fparFFAT   = (float)0.0;
   fparFLIG   = (float)0.05;
   fparFOAC   = (float)0.04;
   fparFMIN   = (float)0.04;

   //table 3, p.47
   fparTBD    = (float)0.0;
   fparTOD    = (float)25.0;
   fparTCD    = (float)37.0;
   fparLWIDTH = (float)0.08;
   fparCDMHT  = (float)150.;
   fparRDMX   = (float)120.;
   fparSLNMIN = (float)0.25;
   fparLNCI   = (float)0.05;
   fparRNCMIN = (float)0.005;
   fparSTEMNC = (float)0.01;
   fparSLA0   = (float)0.02;
   fparEAJMAX = (float)50000;//???

   //genotype specific	   
   fparSEEDW  = (float)0.4;
   fparSEEDNC = (float)0.05;
   fparBLD    = (float)50.0;
   fparHTMX   = (float)1.2;
   fparMTDV   = (float)40.0;
   fparMTDR   = (float)50.0;
   fparPSEN   = (float)-0.0;
  }

   //photosynthesis parameters genotype specific ???
   //fparXJN = 2 * fparXVN !!! see p.44: in general XVN = 60 ?
   fparXVN    =(float)62.0;
   fparXJN    =(float)124.0;


   //table 1, p.45
   fparTHETA = (float)0.7;
   fparINSP  = (float)-2;
   fparWRB   = (float)0.25;
   fparCCFIX = (float)6;

   //table 4, p.49
   fparPMEH  = (float)0.8;
   fparPMES  = (float)0.5;
   fparTSEN  = (float)1.0;
   fparPNPRE = (float)0.8;
   fparTSEN  = (float)1.0;
   fparSPSP  = (float)0.2;
   fparEPSP  = (float)0.7;
   fparESDI  = (float)1.35;
   fparNUPTX = (float)0.5; 
	   


return 1;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      PhasicDevelopment_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

int WINAPI _loadds PhasicDevelopment_GECROS(EXP_POINTER)
    {
      PDEVELOP      pDev = pPl->pDevelop;
	  PGECROSSOIL   pGS  = pGecrosPlant->pGecrosSoil;

      int    iJulianDay;
      double dLatitude;
      double DS,TDU,CTDU,DVR;
	  double TMAX,TMIN,DIFS,TBD,TOD,TCD,TSEN;
	  float  fTempMax,fTempMin,fMaxTemp,fOptTemp,fMinTemp; 

      double DVS[]={0.0, 0.4, 0.55, 0.656, 0.91, 1.00, 1.15, 1.50, 1.95, 2.0};
	  float  VR[10];
      int i;


    //%-- Photoperiod, solar constant and daily extraterrestrial radiation
      iJulianDay  = (int)pTi->pSimTime->iJulianDay;
      dLatitude   = (double)pLo->pFarm->fLatitude;
      
      Astronomy_GECROS(iJulianDay, dLatitude);

//-------------------------------------------------------------------------------------

      DS   = (double)pDev->fStageSUCROS;
      TMAX = (double)pCl->pWeather->fTempMax;    //[°C]
      TMIN = (double)pCl->pWeather->fTempMin;    //[°C]
	  DIFS = (double)pGS->fDiffSoilAirTemp;       //[°C]
      TBD  = (double)fparTBD;//base temperature for phenology     [°C]  crop  table 3, p47 
      TOD  = (double)fparTOD;//optimum temperature for phenology  [°C]  crop  table 3, p47
      TCD  = (double)fparTCD;//ceiling temperature for phenology  [°C]  crop  table 3, p47
      TSEN = (double)fparTSEN;//curvature for temperature response [-]  deflt table 4, p49

	  //%-- Developmental stage (DS) & cumulative thermal units (CTDU)      
      TDU =(double)0;
      TDU=DailyThermalDayUnit_GECROS(DS,TMAX,TMIN,DIFS,DAYL,TBD,TOD,TCD,TSEN);
      pDev->fDTT = (float)TDU;
	 
	  //Vernalization Process nach CERES
	  if (VERN == (double)0)
	  {
	   PlantTemperature(exp_p);     
	   Vernalization_CERES(exp_p);
	  }
      
 
	  //Vernalization Process nach SPASS
	  if (VERN > (double)0)
	  {
	   fTempMin=pCl->pWeather->fTempMin;
	   fTempMax=pCl->pWeather->fTempMax;
	   fMinTemp= pPl->pGenotype->fTempMinVern;	
	   fOptTemp= pPl->pGenotype->fTempOptVern;
	   fMaxTemp= pPl->pGenotype->fTempMaxVern; 
       
	   //fMinTemp= (float) -1.3;	
	   //fOptTemp= (float)  4.9;
	   //fMaxTemp= (float) 15.7; 
	   
	   pDev->fVernUnit = DailyVernalization_SPASS(fTempMin,fTempMax,pPl->pGenotype->VernRateTemp,
		                                         fMinTemp,fOptTemp,fMaxTemp);
	   pDev->fCumVernUnit += pDev->fVernUnit; 
      }//
       
	  DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU,pPl->pGenotype->fOptVernDays);
      pDev->fDevR = (float)DVR;//rate

      
    //%--  Phenological development
      if (pDev->fStageSUCROS<0)
      {
      //initially zero
       pDev->fStageSUCROS= (float)0.0;
       pDev->iDayAftEmerg = 0;
       pDev->fCumTDU = (float)0.0;

       CTDU =(double)0;
       DS =(double)0;
      }
      else
      {
       DS = (double)pDev->fStageSUCROS;   
       //DVR=Phenology_GECROS(DS,SLP,DDLP,SPSP,EPSP,PSEN,MTDV,MTDR,TDU);
       
	  //output
       //pDev->fDevR = (float)DVR;
       //pDev->fStageSUCROS += pDev->fDevR;
       //pDev->fCumTDU += (float)TDU;

	  //output from fStageSUCROS to pDev->fStageWang
      for (i=0;i<10;i++) VR[i]=(float)DVS[i];
      if ((pDev->fStageSUCROS>=VR[0])&&(pDev->fStageSUCROS<=VR[1]))
            pDev->fDevStage=(float)(10.0*(1.0+(pDev->fStageSUCROS-VR[0])/(VR[1]-VR[0])));
      if ((pDev->fStageSUCROS>VR[1])&&(pDev->fStageSUCROS<=VR[2]))
            pDev->fDevStage=(float)(10.0*(2.0+(pDev->fStageSUCROS-VR[1])/(VR[2]-VR[1])));
      if ((pDev->fStageSUCROS>VR[2])&&(pDev->fStageSUCROS<=VR[3]))
            pDev->fDevStage=(float)(10.0*(3.0+(pDev->fStageSUCROS-VR[2])/(VR[3]-VR[2])));
      if ((pDev->fStageSUCROS>VR[3])&&(pDev->fStageSUCROS<=VR[4]))
            pDev->fDevStage=(float)(10.0*(4.0+(pDev->fStageSUCROS-VR[3])/(VR[4]-VR[3])));
      if ((pDev->fStageSUCROS>VR[4])&&(pDev->fStageSUCROS<=VR[5]))
            pDev->fDevStage=(float)(10.0*(5.0+(pDev->fStageSUCROS-VR[4])/(VR[5]-VR[4])));
      if ((pDev->fStageSUCROS>VR[5])&&(pDev->fStageSUCROS<=VR[6]))
            pDev->fDevStage=(float)(10.0*(6.0+(pDev->fStageSUCROS-VR[5])/(VR[6]-VR[5])));
      if ((pDev->fStageSUCROS>VR[6])&&(pDev->fStageSUCROS<=VR[7]))
            pDev->fDevStage=(float)(10.0*(7.0+(pDev->fStageSUCROS-VR[6])/(VR[7]-VR[6])));
      if ((pDev->fStageSUCROS>VR[7])&&(pDev->fStageSUCROS<=VR[8]))
            pDev->fDevStage=(float)(10.0*(8.0+(pDev->fStageSUCROS-VR[7])/(VR[8]-VR[7])));
      if ((pDev->fStageSUCROS>VR[8])&&(pDev->fStageSUCROS<=VR[9]))
            pDev->fDevStage=(float)(10.0*(9.0+0.2*(pDev->fStageSUCROS-VR[8])/(VR[9]-VR[8])));
      if (pDev->fStageSUCROS>VR[9])
            pDev->fDevStage=(float)92.0; 

     }// if (pDev->fStageSUCROS>=0) else


//-------------------------------------------------------------------------------------


	return 1;
    }




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      BiomassGrowth_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*
int WINAPI _loadds CropMaintenance_GECROS(EXP_POINTER);
int WINAPI _loadds BiomassAndCarbonAccum_GECROS(EXP_POINTER);
int WINAPI _loadds NitrogenAccum_GECROS(EXP_POINTER);
int WINAPI _loadds BiomassAndCarbonPartition_GECROS(EXP_POINTER);


int WINAPI _loadds BiomassGrowth_GECROS(EXP_POINTER)
      {
      PGENOTYPE      pGen = pPl->pGenotype;
      PBIOMASS      pBiom= pPl->pBiomass;

      
      BiomassAndCarbonAccum_GECROS(exp_p);
      NitrogenAccum_GECROS(exp_p);
	  //OrganSenescence_GECROS(exp_p);
      BiomassAndCarbonPartition_GECROS(exp_p);
      CropMaintenance_GECROS(exp_p);

      return 1;
      }
*/




//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Respiration and CropMaintenance
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds CropMaintenance_GECROS(EXP_POINTER)
      {       
      double RMUN,RMUA,RMUS,RMLD,RMUL,RRMUL,RMRE,RM,RX,RG,RESTOT;
      //double CCFIX;//CFO;
      double NUPTN,NUPTA,ASSA,APCAN;//,YGV;
      double FCSH,NTOT,WSH,LNCMIN,WRT,RNCMIN;
      double YGO,NFIX,RCLV,RCSST,RCSRT,LCLV,LCRT;
      double RCSO,CREMS,CREMR;
	  double DELT=(double)1;

      PPLTCARBON   pPltC  = pPl->pPltCarbon;
      PPLTNITROGEN pPltN  = pPl->pPltNitrogen;
      PGENOTYPE    pGen   = pPl->pGenotype;
      PBIOMASS     pPltB = pPl->pBiomass;
 
      PGECROSNITROGEN  pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCARBON    pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSPARAMETER pGPar = pGecrosPlant->pGecrosParameter;
	  PGECROSBIOMASS   pGPltB = pGecrosPlant->pGecrosBiomass;

	  //-------------------------------------------------------------------------------------

      //%**input
      NUPTN  = (double)pPltN->fActNO3NUpt/10.0;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      NUPTA  = (double)pPltN->fActNH4NUpt/10.0;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      RMUL   = (double)pGPltC->fUptRespCost;
      RRMUL  = (double)pGPltC->fUptRespCostR;
      ///*
      //YGV    = (double)pGPar->fGrwEffVegOrg;
      ASSA   = (double)pPltC->fNetPhotosynR;
      FCSH   = (double)pGPltC->fCFracPartToShoot;//pPltB->fPartFracShoot;
      NTOT   = (double)pPltN->fTotalCont;
      WSH    = (double)pGPltB->fShootWeight;//(pPltB->fLeafWeight + pPltB->fStemWeight + pPltB->fGrainWeight);
      LNCMIN = (double)pPltN->fLeafMinConc;
      WRT    = (double)pPltB->fRootWeight/10;//[g m-2] <-- [kg ha-1]
      RNCMIN = (double)pPltN->fRootMinConc;//Parameter
	  //*/
      APCAN  = (double)pPltC->fGrossPhotosynR;
      //CCFIX  = (double)pGPltC->fCNFixCost;//Parameter
      NFIX   = (double)pGPltN->fNFixation;
	  ///*
      RCLV   = (double)pGPltC->fCLeafChangeR;
      RCSST  = (double)pGPltC->fCStemChangeR;
      RCSRT  = (double)pGPltC->fCRootChangeR;
      LCLV   = (double)pGPltC->fCLeafLossR;
      LCRT   = (double)pGPltC->fCRootLossR;
      YGO    = (double)pGen->fGrwEffStorage;
      RCSO   = (double)pGPltC->fCStorageChangeR;
      CREMR  = (double)pGPltC->fCRootToStorageR;
      CREMS  = (double)pGPltC->fCStemToStorageR;
	  //*/
        
      //%** Maintenance and total respiration (g CO2 m-2 d-1)
      RMUN   = 44./12.*2.05*NUPTN;
      RMUA   = 44./12.*0.17*NUPTA;
      RMUS   = 0.06* 0.05/0.454*YGV*ASSA;
      RMLD   = 0.06*(1.-FCSH)*ASSA;

      RMUL   = max(0,RMUL+RRMUL);
      RRMUL  = (RMUN+RMUA+RMUS+RMLD - RMUL)/DELT;
      //RMUL   = RMUN+RMUA+RMUS+RMLD;

      RMRE   = max(min(44./12.*0.218*(NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL), 0.);
      
      RM     = max(0., min(APCAN-1.E-5,RMUL) + RMRE);
      RX     = 44./12.*(CCFIX*NFIX);
      RG     = 44./12.*((1.-YGV)/YGV*(RCLV+RCSST+RCSRT+LCLV+LCRT)+(1.-YGO)/YGO*RCSO);

      RESTOT = RM+RX+RG + 44./12.*0.06*(CREMS+CREMR); 


      //%**output maintenance respiration       
      pPltC->fTotRespR      = (float)RESTOT;
      pPltC->fMaintRespR    = (float)RM;
      pPltC->fGrowthRespR   = (float)RG;
      pGPltC->fFixRespCost  = (float)RX;
      pGPltC->fUptRespCost  = (float)RMUL;
      pGPltC->fUptRespCostR = (float)RRMUL;
      
      //Assimilation rates
      pPltC->fNetPhotosynR = pPltC->fGrossPhotosynR -(float)(RM+RX);
      pGPltC->fNetStdgCropPhotosynR = pGPltC->fGrossStdgCropPhotosynR -(float)(RM+RX);

//-------------------------------------------------------------------------------------
      
      return 1;
      }
      

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      BiomassPartition_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds BiomassAndCarbonPartition_GECROS(EXP_POINTER)
      {
      double FCSH,DCSS,DCSR,ESD;//locals
      double FCLV,FCSO,FCSST;
      double ASSA,NCR,DERI,SHSA,LAIC,LAIN,DS,FLWCS,FLWCT;//input
      double CSRT,CSRTN;//input
      double DCDS,CRVS,CRVR;//input & output
      double FCRVS,CREMS,FCRVR,CREMR;//locals
      double GAP,RCRVS,RCRVR,CREMSI,CREMRI,RNRES,NRES;//locals
      double NUPT,LNCMIN,RCLV,LCLV,RNCMIN,RCSST,RCSRT,LCRT;//input //,CFV;
      double WSO;//input
      double NREOE,NREOF;//input & output
      double RNREOE,RNREOF;//locals
      double TSN,TSW;//output
      double DVR,DELT,DCDSC,DCDSR,DCDTR,CFO,YGO,RCSO;
      double RDCDSR,FDS,RDCDTR,IFSH,FDH,DCDT,DCDTC,DCST,DCDTP;//,YGV;

      PPLTCARBON   pPltC  = pPl->pPltCarbon;
      PPLTNITROGEN pPltN  = pPl->pPltNitrogen;
      PGENOTYPE    pGen   = pPl->pGenotype;
      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev   = pPl->pDevelop;
 
      PGECROSBIOMASS   pGPltB = pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON    pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN  pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY    pGCan = pGecrosPlant->pGecrosCanopy;
      PGECROSPARAMETER pGPar = pGecrosPlant->pGecrosParameter;


      
      //input
      //NCR    = (double)pGPltN->fNToCFracNewBiomass;//Input 
      NCR    = (double)pGPltN->fNToCFracNewBiomPrev;//Input 
      DERI   = (double)pGPltB->fShtToRtActivity;//Input 
      SHSA   = (double)pGPltB->fShootActivity;//Input

      LAIC   = (double)pGCan->fLAICdeterm;//Input
      LAIN   = (double)pGCan->fLAINdeterm;//Input
      DS     = (double)pDev->fStageSUCROS;//Input
      
      FLWCS  = (double)pGPltC->fCFlowToSink;//Input & Output
      FLWCT  = (double)pGPltC->fCFlowToStem;//Input & Output
      DCSS   = (double)pGPltC->fCDlySupplyShoot;//Input & Output

      CSRT   = (double)pGPltC->fCStrctRoot;//Input
      CSRTN  = (double)pGPltC->fCStrctRootN;//Input

      //ESD    = (double)pGPar->fStageEndSeedNum;
      ESD    = INSW(DETER, ESDI, 1.);
	  FCSO   = (double)pGPltC->fCFracPartToStorage;
      FCSST  = (double)pGPltC->fCFracPartToStem;
      
      //%** Carbon partitioning among organs and reserve pools
      FCSH   = 1./(1.+NCR*DERI/NOTNUL(SHSA));
        
      FCLV   = REAAND(LAIN-LAIC,ESD-DS)*(1.-FCSO-FCSST);
      FCSST  = INSW(DS-(ESD+0.2), FLWCT/NOTNUL(DCSS), 0.);
      FCSO   = FLWCS/NOTNUL(DCSS);
        
      FCRVS  = max(0.,1. - FCLV - FCSO - FCSST);
      FCRVR  = INSW(CSRTN-CSRT, 1., 0.);

      //output
      pGPltC->fCFracPartToShoot       = (float)FCSH;
      pGPltC->fCFracPartToLeaf        = (float)FCLV;
      pGPltC->fCFracPartToStem        = (float)FCSST;
      pGPltC->fCFracPartToStorage     = (float)FCSO;
      pGPltC->fCFracPartToStemReserve = (float)FCRVS;
      pGPltC->fCFracPartToRootReserve = (float)FCRVR;

//-------------------------------------------------------------------------------------
      
	  //% -- input
      ASSA   = (double)pPltC->fNetPhotosynR;

      DCDS   = (double)pGPltC->fCDlyDemandStorage;//Input

      CRVS   = (double)pGPltC->fCStemReserve;//Input & Output
      RCRVS  = (double)pGPltC->fCStemRsrvChangeR;//Input & Output
      FCRVS  = (double)pGPltC->fCFracPartToStem;//Input & Output

      DCSS   = (double)pGPltC->fCDlySupplyShoot;//Input & Output
      CREMS  = (double)pGPltC->fCStemToStorageR;//Input & Output
      CREMSI = (double)pGPltC->fCStemToStorageRI;//Input

      CRVR   = (double)pGPltC->fCRootReserve;//Input & Output
      RCRVR  = (double)pGPltC->fCRootRsrvChangeR;//Input & Output
      FCRVR  = (double)pGPltC->fCFracPartToRootReserve;//Output

      DCSR   = (double)pGPltC->fCDlySupplyRoot;//Input & Output
      CREMR  = (double)pGPltC->fCRootToStorageR;
      CREMRI = (double)pGPltC->fCRootToStorageRI;
      GAP    = (double)pGPltC->fCSeedGrowthGap;
      
      //%** Dynamics of carbon-reserve pool in stems and roots
      CRVS   = max(0,CRVS+RCRVS);
      RCRVS  = FCRVS*DCSS - CREMS;
      CREMS  = INSW(DCDS-DCSS, 0., CREMSI);

      CRVR   = max(0,CRVR+RCRVR);
      RCRVR  = FCRVR*DCSR - CREMR;
      CREMR  = INSW(DCDS-DCSS, 0., CREMRI);
      
      CREMSI = min(0.94*CRVS, CRVS/NOTNUL(CRVS+CRVR)*GAP)/0.94;
      CREMRI = min(0.94*CRVR, CRVR/NOTNUL(CRVS+CRVR)*GAP)/0.94;
      GAP    = max(0., DCDS-DCSS);

	  //%-- output
      pGPltC->fCStemReserve     = (float)CRVS;
      pGPltC->fCStemRsrvChangeR = (float)RCRVS;
      pGPltC->fCStemToStorageR  = (float)CREMS;

      pGPltC->fCRootReserve     = (float)CRVR;
      pGPltC->fCRootRsrvChangeR = (float)RCRVR;
      pGPltC->fCRootToStorageR  = (float)CREMR;

      pGPltC->fCStemToStorageRI = (float)CREMSI;
      pGPltC->fCRootToStorageRI = (float)CREMRI;
      pGPltC->fCSeedGrowthGap   = (float)GAP;

//-------------------------------------------------------------------------------------


      //%** Carbon supply from current photo-assimilates for shoot & root growth      
      DCSS   = 12./44.*    FCSH *ASSA;
      DCSR   = 12./44.*(1.-FCSH)*ASSA;
      pGPltC->fCDlySupplyShoot  = (float)DCSS;
      pGPltC->fCDlySupplyRoot   = (float)DCSR;

      
//-------------------------------------------------------------------------------------

	  //%--input
      NREOE  = (double)pGPltN->fNRemobToSeedE;//Input & Output
      NREOF  = (double)pGPltN->fNRemobToSeedF;//Input & Output
      RNRES  = (double)pGPltN->fNRemobToSeedR;//Input & Output
      RNREOE = (double)pGPltN->fNRemobToSeedER;//Input & Output
      RNREOF = (double)pGPltN->fNRemobToSeedFR;//Input & Output
      
	  LNCMIN = (double)pPltN->fLeafMinConc;  //Input
      RCSRT  = (double)pGPltC->fCRootChangeR;//Input
      LCRT   = (double)pGPltC->fCRootLossR;  //Input
	  RNCMIN = (double)pPltN->fRootMinConc;  //Input
	  STEMNC = (double)pPltN->fStemActConc;  //Input
	  RCSST  = (double)pGPltC->fCStemChangeR;//Input
      NUPT   = (double)pPltN->fActNUpt/10;//[g m-2] <-- [kg ha-1]
	  RCLV   = (double)pGPltC->fCLeafChangeR;
	  LCLV   = (double)pGPltC->fCLeafLossR;
      WSO    = (double)pGPltB->fStorageWeight;
	  YGO    = (double)pGen->fGrwEffStorage;
	  RCSO   = (double)pGPltC->fCStorageChangeR;
	  DELT   = (double)1;
      DS     = (double)pDev->fStageSUCROS;

      ESD    = INSW(DETER, ESDI, 1.);
 
      //%** Estimation of total seed number, and 1000-seed weight
      NREOE  = max(0,NREOE+RNREOE);
      NREOF  = max(0,NREOF+RNREOF);
      RNREOE = INSW (DS-ESD, RNRES, 0.);
      RNREOF = INSW (DS-1.0, RNRES, 0.);
      RNRES  = NUPT-(LNCMIN*(RCLV+LCLV)+RNCMIN*(RCSRT+LCRT)+STEMNC*RCSST)/CFV;
      NRES   = NREOF + (NREOE-NREOF)*(ESD-1.)/NOTNUL(min(DS,ESD)-1.);
      TSN    = NRES/PNPRE/SEEDNC/SEEDW;
      TSW    = WSO/NOTNUL(TSN)*1000.;
    
      //%-- Output
      pGPltN->fNRemobToSeedR   = (float)RNRES;
      pGPltN->fNRemobToSeedE   = (float)NREOE;
      pGPltN->fNRemobToSeedF   = (float)NREOF;
      pGPltN->fNRemobToSeedER  = (float)RNREOE;
      pGPltN->fNRemobToSeedFR  = (float)RNREOF;
      pGPltB->fTotalSeedNum    = (float)TSN;
      pGPltB->f1000GrainWeight = (float)TSW;

//-------------------------------------------------------------------------------------

      //% -- Input
      DVR    = (double)pDev->fDevR;
      DS     = (double)pDev->fStageSUCROS;
      //PMES   = (double)pGPar->fPMES;//Parameter
      TSN    = (double)pGPltB->fTotalSeedNum;
      //SEEDW  = (double)pGPar->fSeedWeight;//Parameter
      CFO    = (double)pGPltC->fCFracStorage;
      YGO    = (double)pGen->fGrwEffStorage;

      DCSS   = (double)pGPltC->fCDlySupplyShoot;
	  DCDSR  = (double)pGPltC->fCShortFallDemandSeed;
      RDCDSR = (double)pGPltC->fCShortFallDemandSeedR;
     
      //%** Daily carbon flow for seed filling
      FDS = betaf(DVR,1.,PMES*1.,LIMIT(1.,2.,DS)-1.);
      sinkg(DS,1.,TSN*SEEDW*CFO,YGO,FDS,DCDSR,DCSS,DELT,&DCDSC,&DCDS,&FLWCS);
      DCDSR  = max(0,DCDSR+RDCDSR);
      RDCDSR = max(0.,(DCDSC-RCSO/YGO))-(FLWCS-min(DCDSC,DCSS));
      //%-- Output
      pGPltC->fCActDemandSeed        = (float)DCDSC;
      pGPltC->fCDlyDemandStorage     = (float)DCDS;
      pGPltC->fCFlowToSink           = (float)FLWCS;
      pGPltC->fCShortFallDemandSeed  = (float)DCDSR;
      pGPltC->fCShortFallDemandSeedR = (float)RDCDSR;

//-------------------------------------------------------------------------------------

      //%-- Input
      //ESD   = (double)pGPar->fStageEndSeedNum;
      //PMEH   = (double)pGPar->fPMEH;
      //CFV    = (double)pGPltC->fCFracVegetative;
      //YGV    = (double)pGPar->fGrwEffVegOrg;
      DCDTC  = (double)pGPltC->fCActDemandStem;
      DCDTR  = (double)pGPltC->fCShortFallDemandStem;
      RDCDTR = (double)pGPltC->fCShortFallDemandStemR;

	  DCST   = (double)pGPltC->fCDlySupplyStem;
	  DCDTP  = (double)pGPltC->fCPrvDemandStem;
	  IFSH   = LIMIT(0.,1.,DCST/NOTNUL(DCDTP));

      //%** Daily carbon flow for structural stem growth
      DCST   = DCSS - FLWCS;
      FDH = betaf(DVR,(1.+ESD)/2.,PMEH*(1.+ESD)/2.,min((1.+ESD)/2.,DS));      
      sinkg(DS,0.,CDMHT*HTMX*CFV,YGV,FDH*IFSH,DCDTR,DCST,DELT,&DCDTC,&DCDT,&FLWCT);
      DCDTR  = max(0,DCDTR+RDCDTR);
      RDCDTR = max(0., (DCDTC-RCSST/YGV))-(FLWCT-min(DCDTC,DCST));

      //%-- Output
      pGPltC->fCDlySupplyStem        = (float)DCST;
      pGPltC->fCActDemandStem        = (float)DCDTC;
      pGPltC->fCDlyDemandStem        = (float)DCDT;
      pGPltC->fCFlowToStem           = (float)FLWCT;
      pGPltC->fCShortFallDemandStem  = (float)DCDTR;
      pGPltC->fCShortFallDemandStemR = (float)RDCDTR;

//-------------------------------------------------------------------------------------



      return 1;
      }



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Nitrogen Accumulation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds NitrogenAccum_GECROS(EXP_POINTER)
{
 double FNSH,NUPT,RWST,LNCMIN,RNCMIN,LNC,RNC,NLV,NRT,WLV,WRT;
 //double CB,CX,TM;
 double DS,RWSO,LNLV,LNRT;
 double RNRT,RNST,RNLV,RTNLV,RNSO;
 double NST,NSO,TNLV,NLVD,NRTD,NSH;
 double NSHH,WLVD,CLVDS,NTOT;//,CFV;
 double HNC,PNC,ONC,WSH,WSO,WTOT;
 double PSO;
 double KW,KN,NBK,TLAI;
 //double DXT,SLN,SLNT,SLNB,SLNNT,LAI;
 double SLN,SLNT,LAI;
 double NCR,NDEMA,DERI,SHSA,CSH,CRT;//,YGV,NUPTX;
 //double F0;
 double RSLNB,SLNB,SLNBC,SLNNT;
 //double SEEDNC,STEMNC;
 double DELT = (double)1;

      PPLTCARBON   pPltC = pPl->pPltCarbon;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PGENOTYPE    pGen  = pPl->pGenotype;
      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
      PCANOPY      pCan  = pPl->pCanopy;
 
      PGECROSBIOMASS  pGPltB = pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON   pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY   pGCan = pGecrosPlant->pGecrosCanopy;
      PGECROSPARAMETER pGPar = pGecrosPlant->pGecrosParameter;

 //-------------------------------------------------------------------------------------

      //%** Nitrogen accumulation
//SG20110909
	  NRT    = (double)pPltN->fRootCont/10.0; //[g/m2] <-- [kg/ha]
//    NRT    = (double)pPltN->fRootCont; 
      RNRT   = (double)pGPltN->fNRootAccumR;
      NRT    = max(0,NRT+RNRT);
      pPltN->fRootCont = (float)(NRT*10.0); //[g/m2] --> [kg/ha]
      //pPltN->fRootCont = (float)NRT;
        
      NST    = (double)pPltN->fStemCont;
      RNST   = (double)pGPltN->fNStemAccumR;
      NST    = max(0,NST+RNST);
      pPltN->fStemCont = (float)NST;
        
      NLV    = (double)pPltN->fLeafCont;
      RNLV   = (double)pGPltN->fNLeafAccumR;
      NLV    = max(0,NLV+RNLV);
      pPltN->fLeafCont = (float)NLV;
        
      TNLV   = (double)pGPltN->fNLeafTotCont;
      RTNLV  = (double)pGPltN->fNLeafTotAccumR;
      TNLV   = max(0,TNLV+RTNLV);
      pGPltN->fNLeafTotCont = (float)TNLV;
        
      NSO    = (double)pGPltN->fNStorageCont;
      RNSO   = (double)pGPltN->fNStorageAccumR;
      NSO    = max(0,NSO+RNSO);
      pGPltN->fNStorageCont = (float)NSO;
        
      NLVD   = (double)pGPltN->fNLeafDeadCont;
      LNLV   = (double)pGPltN->fNLeafLossR;
      NLVD   = max(0,NLVD+LNLV);
      pGPltN->fNLeafDeadCont = (float)NLVD;
        
      NRTD   = (double)pGPltN->fNRootDeadCont;
      LNRT   = (double)pGPltN->fNRootLossR;
      NRTD   = max(0,NRTD+LNRT);
        
      NSH    = NST + NLV + NSO;
      pGPltN->fNShootCont = (float)NSH;
        
      WLVD   = (double)pGPltB->fLeafDeadWeight;
      CLVDS  = (double)pGPltC->fCLeafDeadSoil;
	  LNCMIN = (double)pPltN->fLeafMinConc;
      //CFV    = (double)pGPltC->fCFracVegetative;
      NSHH   = NSH +(WLVD-CLVDS/CFV)*LNCMIN;
      pGPltN->fNShootContSoil = (float)NSHH;
        
      NTOT   = NSH + NRT;
      pPltN->fTotalCont = (float)NTOT;
              
//-------------------------------------------------------------------------------------
      
	  //%-- input: nitrogen accumulation function rnacc
      FNSH   = (double)pGPltN->fNFracPartShoot;
      NUPT   = (double)pPltN->fActNUpt/10;//[g m-2] <-- [kg ha-1]
      LNC    = (double)pPltN->fLeafActConc;
      RNC    = (double)pPltN->fRootActConc;
	  ///*
      //STEMNC = (double)pPltN->fStemActConc;
      LNCMIN = (double)pPltN->fLeafMinConc;
      RNCMIN = (double)pPltN->fRootMinConc;
      NLV    = (double)pPltN->fLeafCont;
          //SG20110909
	  NRT    = (double)pPltN->fRootCont/10.0;  //[g m-2] <-- [kg ha-1]
          //NRT    = (double)pPltN->fRootCont;
      WLV    = (double)pPltB->fLeafWeight/10;//[g m-2] <-- [kg ha-1]
      WRT    = (double)pPltB->fRootWeight/10;//[g m-2] <-- [kg ha-1]
      //CB     = (double)pGPltN->fNInitFacSeedF;
      //CX     = (double)pGPltN->fNFinalFacSeedF;
      //TM     = (double)pGPltN->fNFastSeedFDevStage;
      DS     = (double)pDev->fStageSUCROS;
      //SEEDNC = (double)pGPltN->fNSeedConc;
      RWSO   = (double)pGPltB->fStorageGrowR;
      RWST   = (double)pPltB->fStemGrowR;
      LNLV   = (double)pGPltN->fNLeafLossR;
      LNRT   = (double)pGPltN->fNRootLossR;
      //*/
      // N accumulation rates of plant organs
      rnacc(FNSH,NUPT,RWST,STEMNC,LNCMIN,RNCMIN,LNC,RNC,NLV,NRT,WLV,WRT,DELT,
            CB,CX,TM,DS,SEEDNC,RWSO,LNLV,LNRT,&RNRT,&RNST,&RNLV,&RTNLV,&RNSO);

      //%-- output
      pGPltN->fNRootAccumR    = (float)RNRT;
      pGPltN->fNStemAccumR    = (float)RNST;
      pGPltN->fNLeafAccumR    = (float)RNLV;
      pGPltN->fNLeafTotAccumR = (float)RTNLV;
      pGPltN->fNStorageAccumR = (float)RNSO;

//-------------------------------------------------------------------------------------
      
      //input
      //NUPTX  = (double)pGPltN->fNUptMax;
      NDEMA  = (double)pGPltN->fNDmndActDrv;
      SLNT   = (double)pGPltN->fNLeafSpecificContTop;//SLNT
      ///*  
      //YGV    = (double)pGPar->fGrwEffVegOrg;//Input Parameter
      DERI   = (double)pGPltB->fShtToRtActivity;//Input
      SHSA   = (double)pGPltB->fShootActivity;//Input
      CSH    = (double)pGPltC->fCShoot;
      CRT    = (double)pGPltC->fCRoot;
      NCR    = (double)pGPltN->fNToCFracNewBiomass;//Input
	  
	  pGPltN->fNToCFracNewBiomPrev = pGPltN->fNToCFracNewBiomass;
      //*/ 
	  //%-- Nitrogen partitioning between shoots and roots
      FNSH  = 1./(1.+NCR*DERI/NOTNUL(SHSA)*CSH/CRT*NRT/NSH);
      NCR   = INSW(SLNT-SLNMIN,0.,min(NUPTX,NDEMA))
                  /NOTNUL(YGV*(double)pGPltC->fNetStdgCropPhotosynR*12./44.);

      //%-- output
      pGPltN->fNFracPartShoot     = (float)FNSH;
      pGPltN->fNToCFracNewBiomass = (float)NCR;

//-------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------

      //%** input
	  /*
      NLV    = (double)pPltN->fLeafCont;
      NRT    = (double)pPltN->fRootCont;
      NSH    = (double)pGPltN->fNShootCont;
      NSO    = (double)pGPltN->fNStorageCont;
      NTOT   = (double)pPltN->fTotalCont;
      */
	  ///*
      //WLV    = (double)pPltB->fLeafWeight/10;
      //WRT    = (double)pPltB->fRootWeight/10;
      WSH    = (double)pGPltB->fShootWeight;
      WSO    = (double)pGPltB->fStorageWeight;
      WTOT   = (double)pPltB->fTotalBiomass/10;
      //*/
      
      //%** Nitrogen concentration of biomass
	  LNC   = NLV / WLV;
      RNC   = NRT / WRT;
      HNC   = NSH / WSH;
      PNC   = NTOT/ WTOT;
      ONC   = INSW(-WSO, NSO/NOTNUL(WSO), 0.);

      pPltN->fLeafActConc     = (float)LNC;
      pPltN->fRootActConc     = (float)RNC;
      pGPltN->fShootActConc   = (float)HNC;
      pGPltN->fStorageActConc = (float)ONC;
      pGPltN->fTotalActConc   = (float)PNC;
    
//-------------------------------------------------------------------------------------

	  //%** Amount of seed protein
      PSO    = 6.25*WSO*ONC;
      pGPltN->fSeedProtein = (float)PSO;

//-------------------------------------------------------------------------------------
 
      //%** Extinction coefficient of nitrogen and wind
      TLAI = (double)pGCan->fLAITotal;//BLD = 
      TNLV = (double)pGPltN->fNLeafTotCont;//SLNMIN =
      
      KW   = kdiff(TLAI,BLD*3.141592654/180.,0.2);
      KN   = KW*(TNLV-SLNMIN*TLAI);
      NBK  = SLNMIN*(1.-exp(-KW*TLAI));
      KN   = 1./TLAI*log((KN+NBK)/(KN*exp(-KW*TLAI)+NBK));

      //%** Specific leaf nitrogen and its profile in the canopy
      LAI  = (double)pGCan->fLAIGreen;

      SLN    = NLV/LAI;
      SLNT   = NLV             *KN/(1.-exp(-KN*LAI));
      SLNBC  = NLV*exp(-KN*LAI)*KN/(1.-exp(-KN*LAI));
      SLNNT  = (NLV+0.001*NLV) *KN/(1.-exp(-KN*LAI));
      
      SLNB   = (double) pGPltN->fNLeafSpecificContBottom;
      RSLNB  = (double) pGPltN->fNLeafSpecificContBottomChangeR;
      SLNB   = max(0,SLNB+RSLNB);
      RSLNB  = (SLNBC-SLNB)/DELT;
      
      pGPltN->fNLeafSpecificCont = (float)SLN;
      pGPltN->fNLeafSpecificContTop = (float)SLNT;
      pGPltN->fNLeafSpecificContBottom = (float)SLNB;
      pGPltN->fNLeafSpecificContBottomChangeR = (float)RSLNB;

//-------------------------------------------------------------------------------------


return 1;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Biomass Growth and Carbon Accumulation
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds BiomassAndCarbonAccum_GECROS(EXP_POINTER)
     {
      double RCLV,ASSA,FCSH,FCLV,LCLV;//,YGV;
      double RCSST,FCSST;
      double RCSRT,FCRVR,LCRT;
      double RCSO,FCSO,YGO,CREMS,CREMR;
      double CLV,CLVD,CLVDS,LVDS,CSST,CSO,CSRT,CRTD,CSH,CRT,CTOT;
      double HI,WLV,WST,WSO,CFO,WRT,WSH,WSHH,WLVD,WRTD,WTOT;//,CFV;
      double RWST,RWSO,RWLV,RWRT;
      double CRVS,CRVR,RCRVS,RCRVR;

      PPLTCARBON   pPltC = pPl->pPltCarbon;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PGENOTYPE    pGen  = pPl->pGenotype;
      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
      PCANOPY      pCan  = pPl->pCanopy;
 
      PGECROSBIOMASS  pGPltB = pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON   pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY   pGCan = pGecrosPlant->pGecrosCanopy;
      PGECROSPARAMETER pGPar = pGecrosPlant->pGecrosParameter;


	  //input
	  
      CLV    = (double)pGPltC->fCLeaf;
      CLVD   = (double)pGPltC->fCLeafDead;
      CLVDS  = (double)pGPltC->fCLeafDeadSoil;
      CSST   = (double)pGPltC->fCStem;
      CSO    = (double)pGPltC->fCStorage;
      CSRT   = (double)pGPltC->fCStrctRoot;
      CRTD   = (double)pGPltC->fCRootDead;
        
      WSO    = (double)pGPltB->fStorageWeight;
      WSHH   = (double)pGPltB->fShtWghtExShLvs;
      WLVD   = (double)pGPltB->fLeafDeadWeight;
      //CFV    = (double)pGPltC->fCFracVegetative;
      CFO    = (double)pGPltC->fCFracStorage;
      CRVS   = (double)pGPltC->fCStemReserve;
      CRVR   = (double)pGPltC->fCRootReserve;
      RCRVS  = (double)pGPltC->fCStemRsrvChangeR;
      RCRVR  = (double)pGPltC->fCRootRsrvChangeR;

      RCLV   = (double)pGPltC->fCLeafChangeR;
      RCSST  = (double)pGPltC->fCStemChangeR;
      RCSO   = (double)pGPltC->fCStorageChangeR;
      RCSRT  = (double)pGPltC->fCRootChangeR;
    
      //%** Biomass formation
      WLV    = CLV  / CFV;
      WST    = CSST / CFV + CRVS/0.444;
      WSO    = CSO  / CFO;
      WRT    = CSRT / CFV + CRVR/0.444;
      WSH    = WLV  + WST + WSO;
      WSHH   = WSH  + (WLVD-CLVDS/CFV);
      WTOT   = WSH  + WRT;
      HI     = WSO  / WSHH;

        
      RWST   = RCSST/ CFV + RCRVS/0.444;
      RWSO   = RCSO / CFO;
      RWLV   = RCLV / CFV;
      RWRT   = RCSRT/ CFV + RCRVR/0.444;
        
      WLVD   = CLVD / CFV;
      WRTD   = CRTD / CFV;

      //output
      pGPltB->fHarvestIndex   = (float)HI;
      pPltB->fLeafWeight      = (float)(WLV*10); //[g m-2] --> [kg ha-1]
      pPltB->fStemWeight      = (float)(WST*10); //[g m-2] --> [kg ha-1]
      pGPltB->fStorageWeight  = (float)WSO;
      pPltB->fRootWeight      = (float)(WRT*10); //[g m-2] --> [kg ha-1]
      pGPltB->fShootWeight    = (float)WSH;
      pGPltB->fShtWghtExShLvs = (float)WSHH;
      pPltB->fTotalBiomass    = (float)(WTOT*10);//[g m-2] --> [kg ha-1]

      pPltB->fGrainWeight     = (float)(WSO*10); //[g m-2] --> [kg ha-1]
      pPltB->fStovWeight      = pPltB->fLeafWeight+pPltB->fStemWeight;


      pPltB->fStemGrowR       = (float)RWST;
      pGPltB->fStorageGrowR   = (float)RWSO;
      pPltB->fLeafGrowR       = (float)RWLV;
      pPltB->fRootGrowR       = (float)RWRT;

      pGPltB->fLeafDeadWeight = (float)WLVD;
      pGPltB->fRootDeadWeight = (float)WRTD;
      
//-------------------------------------------------------------------------------------
      
	  //input
	  ///*
      CLVD   = (double)pGPltC->fCLeafDead;
      CRTD   = (double)pGPltC->fCRootDead;
      //*/
	  LCLV   = (double)pGPltC->fCLeafLossR;
      LVDS   = (double)pGPltC->fCLeafDeadSoilR;
      LCRT   = (double)pGPltC->fCRootLossR;
      
      //%** Carbon accumulation
      CLV    = max(0.01,CLV   + RCLV);
      CLVD   = (CLVD  + LCLV);
      CLVDS  = (CLVDS + LVDS);
      CSST   = (CSST  + RCSST);
      CSO    = (CSO   + RCSO);
      CSRT   = (CSRT  + RCSRT);
      CRTD   = (CRTD  + LCRT);
  
      CSH    = CLV  + CSST + CRVS + CSO;
      CRT    = CSRT + CRVR;
      CTOT   = CSH  + CRT;

      //output
      pGPltC->fCLeaf         = (float)CLV;
      pGPltC->fCLeafDead     = (float)CLVD;
      pGPltC->fCLeafDeadSoil = (float)CLVDS;
      pGPltC->fCStem         = (float)CSST;
      pGPltC->fCStorage      = (float)CSO;
      pGPltC->fCStrctRoot    = (float)CSRT;
      pGPltC->fCRootDead     = (float)CRTD;
      pGPltC->fCShoot        = (float)CSH;
      pGPltC->fCRoot         = (float)CRT;
      pGPltC->fCPlant        = (float)CTOT;

//-------------------------------------------------------------------------------------
        
      //input
      ASSA   = (double)pPltC->fNetPhotosynR;
      FCSH   = (double)pGPltC->fCFracPartToShoot;//pPltB->fPartFracShoot;
      FCLV   = (double)pGPltC->fCFracPartToLeaf;
      //YGV    = (double)pGPar->fGrwEffVegOrg;
      FCSST  = (double)pGPltC->fCFracPartToStem;
      FCRVR  = (double)pGPltC->fCFracPartToRootReserve;
      FCSO   = (double)pGPltC->fCFracPartToStorage;
      YGO    = (double)pGen->fGrwEffStorage;
      CREMS  = (double)pGPltC->fCStemToStorageR;
      CREMR  = (double)pGPltC->fCRootToStorageR;
      

      //%** Carbon production rates
      RCLV   = 12./44.*ASSA*    FCSH *    FCLV  *YGV - LCLV;
      RCSST  = 12./44.*ASSA*    FCSH *    FCSST *YGV;
      RCSRT  = 12./44.*ASSA*(1.-FCSH)*(1.-FCRVR)*YGV - LCRT;
      RCSO   = 12./44.*ASSA*FCSH*FCSO*YGO + 0.94*(CREMS+CREMR)*YGO;

      //output
      pGPltC->fCLeafChangeR    = (float)RCLV;
      pGPltC->fCStemChangeR    = (float)RCSST;
      pGPltC->fCRootChangeR    = (float)RCSRT;
      pGPltC->fCStorageChangeR = (float)RCSO;

//-------------------------------------------------------------------------------------
 
      return 1;
     }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      CanopyFormation_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//int WINAPI _loadds LeafAreaGrowth_GECROS(EXP_POINTER);
//int WINAPI _loadds PlantHeightGrowth_GECROS(EXP_POINTER);


int WINAPI _loadds CanopyFormation_GECROS(EXP_POINTER)
      {
       LeafAreaGrowth_GECROS(exp_p);
       PlantHeightGrowth_GECROS(exp_p);

       return 1;
      }


int WINAPI _loadds LeafAreaGrowth_GECROS(EXP_POINTER)
{
 double TLAI,KN,KW,NBK,LAIN,NLV,TNLV,RLAI,RWLV,DS,CLVD, CLVDS, DLAI, DLAIS;//,CFV;
 double LAI,LAIC,RNLV,RSLNB,SLNB;

      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
	  PCANOPY      pCan  = pPl->pCanopy;

      PGECROSCARBON   pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY   pGCan = pGecrosPlant->pGecrosCanopy;

//-------------------------------------------------------------------------------------
      
      //%** Leaf area development
	  ///*
      RWLV   = (double)pPltB->fLeafGrowR;
      NLV    = (double)pPltN->fLeafCont;
      RNLV   = (double)pGPltN->fNLeafAccumR;
	  TNLV   = (double)pGPltN->fNLeafTotCont;
      SLNB   = (double)pGPltN->fNLeafSpecificContBottom; 
      RSLNB  = (double)pGPltN->fNLeafSpecificContBottomChangeR; 
      CLVD   = (double)pGPltC->fCLeafDead;
	  CLVDS  = (double)pGPltC->fCLeafDeadSoil;
      DS     = (double)pDev->fStageSUCROS;
	  //*/
      LAIC   = (double)pGCan->fLAICdeterm;	  
      RLAI   = (double)pCan->fLAGrowR;
	  TLAI   = (double)pGCan->fLAITotal;
 
	  SLA0   = (double)fparSLA0;
      CFV    = (double)fparCFV;

	  KW     = kdiff(TLAI,BLD*3.141592654/180.,0.2);
	  KN     = KW*(TNLV-SLNMIN*TLAI);
	  NBK    = SLNMIN*(1.-exp(-KW*TLAI));
      KN     = 1./TLAI*log((KN+NBK)/(KN*exp(-KW*TLAI)+NBK));

      
      //%---rate of LAI driven by carbon supply
      RLAI   =  INSW(RWLV, max(-LAIC+(double)1.E-5,SLA0*RWLV), SLA0*RWLV);

      //%---rate of LAI driven by nitrogen during juvenile phase    
      if ((LAIC < (double)1)&&(DS < (double)0.5))
      //RLAI  = (SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV);
      RLAI   = max(0.,(SLNB*RNLV-NLV*RSLNB)/SLNB/(SLNB+KN*NLV));
    
      //%**********************************************************

      LAIC   = max(0,LAIC+RLAI);

	  LAIN   = log(1.+KN*max(0.,NLV)/SLNMIN)/KN;
	  LAI    = min(LAIN, LAIC);

	  //CFV  = (double)pGPltC->fCFracVegetative;
      //SLA    = LAI/WLV;
      DLAI   = (CLVD-CLVDS)/CFV*SLA0;//original
	  DLAIS  = CLVDS/CFV*SLA0;

      //TLAI   = LAIC + CLVD /CFV*SLA0;//original
	  TLAI   = LAIC + DLAI;

      //if(LAIN>=LAIC) TLAI   = LAIC + CLVD /CFV*SLA0;//original
      if(LAIN>=LAIC) TLAI   = LAIC + DLAI;
      
	  //%** output ***
      pCan->fLAGrowR     = (float)RLAI;
      pGCan->fLAICdeterm = (float)LAIC;
	  pPl->pCanopy->fLAI = (float)TLAI;
	  //SG 20111108: Test
	  //pPl->pCanopy->fLAI = pPl->pBiomass->fLeafWeight*(float)SLA0/(float)10.0;

      /*
	  pGCan->fLAINdeterm = (float)LAIN;
      pGCan->fLAIGreen   = (float)LAI;
      pGCan->fLAITotal   = (float)TLAI;
      */
//-------------------------------------------------------------------------------------

      return 1;
}



int WINAPI _loadds  PlantHeightGrowth_GECROS(EXP_POINTER)
    {
     double HT,DS,DVR,DCDTC,DCST,DCDTP,FDH,ESD,IFSH,RHT;
     double DELT =(double)1;
     double RDCDTP; 
//-------------------------------------------------------------------------------------
      ///*  
      //%-- input
      DS    = (double)pPl->pDevelop->fStageSUCROS;
      DVR   = (double)pPl->pDevelop->fDevR;
      DCDTC = (double)pGecrosPlant->pGecrosCarbon->fCActDemandStem;
      DCST  = (double)pGecrosPlant->pGecrosCarbon->fCDlySupplyStem;
	  //*/
      DCDTP  = (double)pGecrosPlant->pGecrosCarbon->fCPrvDemandStem;
	  RDCDTP = (double)pGecrosPlant->pGecrosCarbon->fCPrvDmndStmChangeR;
      HT     = (double)pPl->pCanopy->fPlantHeight;
	  RHT    = (double)pGecrosPlant->pGecrosCanopy->fPlantHeightGrowR;

      //%** Plant height or stem length (m)
      
	  ESD    = INSW(DETER, ESDI, 1.);
      FDH    = betaf(DVR,(1.+ESD)/2.,PMEH*(1.+ESD)/2.,min((1.+ESD)/2.,DS));

      DCDTP  = max(0,DCDTP+RDCDTP);
      RDCDTP = (DCDTC-DCDTP)/DELT;
      IFSH   = LIMIT(0.,1.,DCST/NOTNUL(DCDTP));

      HT     = max(0,HT+RHT);
	  RHT    = min(HTMX-HT, FDH*HTMX*IFSH);//rate

      //%-- output
      pPl->pCanopy->fPlantHeight = (float)HT;
      pGecrosPlant->pGecrosCanopy->fPlantHeightGrowR   = (float)RHT;

      //pGecrosPlant->pGecrosCarbon->fCPrvDemandStem     = (float)DCDTC;
      //pGecrosPlant->pGecrosCarbon->fCPrvDemandStem     = (float)DCDTP;
	  //pGecrosPlant->pGecrosCarbon->fCPrvDmndStmChangeR = (float)RDCDTP;

//-------------------------------------------------------------------------------------


      return 1;
    }



//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      Root depth 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds RootSystemFormation_GECROS(EXP_POINTER)
      {
      double RD,RWRT,LWRT,WRT,WRTD,KR,RRD;
      double DELT = (double)1;
      //double RDMX;

//-------------------------------------------------------------------------------------
      
      //input
      RD     = (double)pPl->pRoot->fDepth;
      //RRD    = (double)pPl->pRoot->fDepthGrowR;
	  ///*
      //RDMX   = (double)pGecrosPlant->pGecrosParameter->fRootDepthMax;
      RWRT   = (double)pPl->pBiomass->fRootGrowR;
      LWRT   = (double)pGecrosPlant->pGecrosBiomass->fRootWeightLossR;
      WRT    = (double)pPl->pBiomass->fRootWeight/10;//[g m-2] <-- [kg ha-1]
      WRTD   = (double)pGecrosPlant->pGecrosBiomass->fRootDeadWeight;
      //*/
      //%** Rooting depth (cm)
	  KR     = -log(0.05)/RDMX;
      RRD    = INSW(RD-RDMX, min((RDMX-RD)/DELT,(RWRT+LWRT)/(WRB+KR*(WRT+WRTD))), 0.);//rate
      RD     = max(0,RD+RRD);

      //output
      pPl->pRoot->fDepthGrowR = (float)RRD; 
      pPl->pRoot->fDepth = (float)RD;

//-------------------------------------------------------------------------------------

      return 1;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      OrganSenescense_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds OrganSenescence_GECROS(EXP_POINTER)
      {   
      double ESD,LWLVM,LWLV,LCLV,LNLV;
      double LAIC,LAIN,WLV,DS;//,CFV,RDMX;
      double LNC,LNCMIN,RNCMIN;
      double KCRN,CSRTN,LWRT,LCRT,LNRT;
      double NRT,CRVR,CSRT;
	  double DELT = (double)1;

      PBIOMASS     pPltB = pPl->pBiomass;
      PDEVELOP     pDev  = pPl->pDevelop;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;

      PGECROSBIOMASS  pGPltB = pGecrosPlant->pGecrosBiomass;
      PGECROSCARBON   pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCANOPY   pGCan = pGecrosPlant->pGecrosCanopy;
      PGECROSPARAMETER pGPar = pGecrosPlant->pGecrosParameter;

//-------------------------------------------------------------------------------------
      
      //Senescence
      //%-- input --
	  ///*
      LAIC   = (double)pGCan->fLAICdeterm;
      LAIN   = (double)pGCan->fLAINdeterm;
      WLV    = (double)pPltB->fLeafWeight/10;//[kg ha-1] --> [g m-2]
      DS     = (double)pDev->fStageSUCROS;
      //CFV    = (double)pGPltC->fCFracVegetative;
      //RDMX   = (double)pGPar->fRootDepthMax;
      LNC    = (double)pPltN->fLeafActConc;
      LNCMIN = (double)pPltN->fLeafMinConc;
      RNCMIN = (double)pPltN->fRootMinConc;

// SG20110909
	  NRT    = (double)pPltN->fRootCont/10.0; //[g/m2] <-- [kg/ha]
//    NRT    = (double)pPltN->fRootCont;
      CRVR   = (double)pGPltC->fCRootReserve;
      CSRT   = (double)pGPltC->fCStrctRoot;
      //*/

      //%** Leaf senescence
      ESD    = INSW(DETER, ESDI, 1.);

      LWLVM  = (LAIC-min(LAIC,LAIN))/SLA0/DELT;
    //SG 20110801:
	  LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.01*WLV); //original 0.03
  //  LWLV   = min(WLV-1.E-5, LWLVM+REANOR(ESD-DS,LWLVM)*0.03*WLV);
      LCLV   = LWLV*CFV;
      LNLV   = min(LWLV,LWLVM)*LNCMIN + (LWLV-min(LWLV,LWLVM))*LNC;
      //LWLV   = 0.0;
      //LCLV   = 0.0;
      //LNLV   = 0.0;

      //%** Root senescence
      KCRN   = -log(0.05)/6.3424/CFV/WRB/RDMX;
      CSRTN  = 1./KCRN*log(1.+KCRN*max(0.,(NRT*CFV-CRVR*RNCMIN))/RNCMIN);
      LCRT   = max(min(CSRT-1.E-4,CSRT-min(CSRTN,CSRT)),0.)/DELT;
      LWRT   = LCRT/CFV;
      LNRT   = LWRT*RNCMIN;
      //LWRT   = 0.0;
      //LCRT   = 0.0;
      //LNRT   = 0.0;

      //%** output
      pGPltB->fLeafWeightLossR = (float)LWLV;
      pGPltC->fCLeafLossR = (float)LCLV;
      pGPltN->fNLeafLossR = (float)LNLV;

      pGPltC->fCStrctRootN=(float)CSRTN;
      pGPltB->fRootWeightLossR = (float)LWRT;
      pGPltC->fCRootLossR = (float)LCRT;
      pGPltN->fNRootLossR = (float)LNRT;

//-------------------------------------------------------------------------------------

      return 1;
      }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      NitrogenFixation_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int   WINAPI _loadds NitrogenFixation_GECROS(EXP_POINTER)
{
 double NDEM,NSUP,APCAN,RM,NFIXE,CCFIX,NFIXD,NFIX,NFIXT,NFIXR,TCP;
 double NDEMP,RNDEMP,NSUPP,RNSUPP;
 double DELT = (double)1;
 double RNFIXR;
 double RX;

      PPLTCARBON   pPltC  = pPl->pPltCarbon;
      PPLTNITROGEN pPltN  = pPl->pPltNitrogen;
 
      PGECROSNITROGEN pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSCARBON   pGPltC = pGecrosPlant->pGecrosCarbon;

     
//-------------------------------------------------------------------------------------

      //%** input ***
      NDEM   = (double)pGPltN->fNDemand;
	  NDEMP  = (double)pGPltN->fNDmndPrev;
	  RNDEMP = (double)pGPltN->fNDmndPrvChangeR;

      NSUP   = (double)pGPltN->fNPlantSupply;
	  NSUPP  = (double)pGPltN->fNPlantSuppPrev;
	  RNSUPP = (double)pGPltN->fNPltSppPrvChangeR;

      APCAN  = (double)pPltC->fGrossPhotosynR;
      RM     = (double)pPltC->fMaintRespR;
      CCFIX  = (double)pGPltC->fCNFixCost;
      NFIXT  = (double)pGPltN->fNFixationTotal;
      NFIXR  = (double)pGPltN->fNFixationReserve;
      RNFIXR = (double)pGPltN->fNFixReserveChangeR;
       
	  CCFIX  = (double)fparCCFIX;

      TCP    = (double)1;//time constant

      //%** Nitrogen fixation (g N m-2 d-1)
	  RNDEMP = (NDEM-NDEMP)/DELT;
      NDEMP  = max(0,NDEMP+RNDEMP);
	  RNSUPP = (NSUP-NSUPP)/DELT;
	  NSUPP  = max(0,NSUPP+RNSUPP);

      NFIXE  = max(0., APCAN-1.E-5-RM)/NOTNUL(CCFIX)*12./44.;
      NFIXD  = max(0., NDEMP-NSUPP);// /DELT;
      //NFIXD  = max(0., NDEM-NSUP);
      NFIX   = INSW (LEGUME, 0., min(NFIXE, NFIXD));
      NFIXT  = max(0,NFIXT+NFIX);
      RNFIXR = NFIX - min(NDEM,NFIXR/TCP);
      NFIXR  = max(0,NFIXR+RNFIXR);

	  RX     = 44./12.*(CCFIX*NFIX);

 
      //%** output
      //pGPltN->fNDemand            = (float)NDEM;
      pGPltN->fNDmndPrev          = (float)NDEMP;
      pGPltN->fNDmndPrvChangeR    = (float)RNDEMP;

      //pGPltN->fNPlantSupply       = (float)NSUP;
      pGPltN->fNPlantSuppPrev     = (float)NSUPP;
      pGPltN->fNPltSppPrvChangeR  = (float)RNSUPP;

      pGPltN->fNFixation          = (float)NFIX;
      pGPltN->fNFixationTotal     = (float)NFIXT;
      pGPltN->fNFixationReserve   = (float)NFIXR;
      pGPltN->fNFixReserveChangeR = (float)RNFIXR;

	  pGPltC->fFixRespCost        = (float)RX;

//-------------------------------------------------------------------------------------

      return 1;
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      PotentialNitrogenUptake_WAVE
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds PotentialNitrogenUptake_GECROS(EXP_POINTER)
   {
      double RX,RMN,NTOT,WSH,LNCMIN,WRT,RNCMIN,APCAN,RMUL,RMRE;
      double SHSA,SHSAN,RM,CSH,APCANN;//,YGV;
      double DERI,CTOT,NDEMA,CRT,NDEMD,DS,HNC,NRT,NSH,HNCCR;
      double NDEMAD,LNC,SLN,NDEM;
      double DELT = (double)1;

      PBIOMASS     pPltB = pPl->pBiomass;
      PPLTCARBON   pPltC = pPl->pPltCarbon;
      PPLTNITROGEN pPltN = pPl->pPltNitrogen;
      PDEVELOP     pDev  = pPl->pDevelop;
 
      PGECROSCARBON    pGPltC = pGecrosPlant->pGecrosCarbon;
      PGECROSNITROGEN  pGPltN = pGecrosPlant->pGecrosNitrogen;
      PGECROSBIOMASS   pGPltB = pGecrosPlant->pGecrosBiomass;
      PGECROSCANOPY    pGCan = pGecrosPlant->pGecrosCanopy;
      PGECROSPARAMETER pGPar = pGecrosPlant->pGecrosParameter;

//-------------------------------------------------------------------------------------
      ///*
      //%** input ***
      NTOT   = (double)pPltN->fTotalCont;
      WSH    = (double)pGPltB->fShootWeight;
      WRT    = (double)pPltB->fRootWeight/10; //[g m-2] <-- [kg ha-1]
      LNCMIN = (double)pPltN->fLeafMinConc;
      RNCMIN = (double)pPltN->fRootMinConc;//Parameter
      APCAN  = (double)pPltC->fGrossPhotosynR;
      RX     = (double)pGPltC->fFixRespCost;
      RMUL   = (double)pGPltC->fUptRespCost;
      RM     = (double)pPltC->fMaintRespR;
      APCANN = (double)pGPltC->fGrossStdgCropPhotosynRN;
      RMN    = (double)pGPltC->fMaintRespRN;
      CTOT   = (double)pGPltC->fCPlant;
      CSH    = (double)pGPltC->fCShoot;
      CRT    = (double)pGPltC->fCRoot;
      DS     = (double)pDev->fStageSUCROS;
      //YGV    = (double)pGPar->fGrwEffVegOrg;

      HNC    = (double)pGPltN->fShootActConc;
      NSH    = (double)pGPltN->fNShootCont;
//SG 20110909
	  NRT    = (double)pPltN->fRootCont/10.0; //[g m-2] <-- [kg ha-1]
//    NRT    = (double)pPltN->fRootCont/10.0;
      LNC    = (double)pPltN->fLeafActConc;
      //LAI    = (double)pGCan->fLAIGreen;
      //NLV    = (double)pPltN->fLeafCont;
      SLN    = (double)pGPltN->fNLeafSpecificCont;
      //NUPTX  = (double)pGPltN->fNUptMax;
      //*/

	  //%** Crop nitrogen demand and uptake (g N m-2 d-1)
      RMRE   = max(min(44./12.*0.218*(NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL), 0.);
      RMN    = min(44./12.*0.218*(1.001*NTOT-WSH*LNCMIN-WRT*RNCMIN),APCAN-1.E-5-RMUL);
      RMN    = max(0., min(APCAN-1.E-5,RMUL) + max(RMN, 0.));
      RM     = max(0., min(APCAN-1.E-5,RMUL) + RMRE);

	  SHSA   = 12./44. * YGV*(APCAN -RM -RX)/ CSH;
      SHSAN  = 12./44. * YGV*(APCANN-RMN-RX)/ CSH;

      DERI   = max(0.,(SHSAN - SHSA)/(0.001*NTOT/CTOT));
      //DERI   = (double)fabs((SHSAN - SHSA)/(0.001*NTOT/CTOT));

      //%-- Nitrogen partitioning between shoots and roots
      HNCCR  = LNCI*exp(-0.4*DS);
      NDEMD  = INSW(DS-1., WSH*(HNCCR-HNC)*(1.+NRT/NSH)/DELT, 0.);
      NDEMA  = CRT * SHSA*SHSA/NOTNUL(DERI);
      NDEMAD = INSW(LNC-1.5*LNCI, max(NDEMA, NDEMD), 0.);
      NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,NDEMAD), 0.);

   //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// SG20140312:
	// Vorschlag von Joachim Ingwersen zur Simulation der Winterung
    if((!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WH"))             // here we have to 
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"BA"))  // distinguish between
				||(!lstrcmp((LPSTR)pPl->pGenotype->acCropCode,(LPSTR)"WR"))) // winter and summer crops --> DB!!!
	{
		if (DS < 0.25) // DSCRIT = 0.25
    		NDEM   = INSW(SLNMIN-SLN+1.E-5, min(NUPTX,0.01*NDEMAD), 0.);
		else
    		NDEM   = INSW(SLNMIN-SLN+1.E-5,  min(NUPTX,NDEMAD), 0.);
	}
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
      //%** output
      pGPltC->fMaintRespRN     = (float)RMN;
      pGPltB->fShtToRtActivity = (float)DERI;
      pGPltB->fShootActivity   = (float)SHSA;

      pGPltN->fNDmndActDrv     = (float)NDEMA;
	  pGPltN->fNDemand         = (float)NDEM;
      //pPltN->fTotalDemand      = (float)NDEM / (float)10;// 1.0 [kg N ha-1] = [g N m-2]  
      pPltN->fTotalDemand      = (float)NDEM * (float)10;// [g N m-2] = 10.0 [kg N ha-1] 
        

       return 1;
      }


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//      ActualNitrogenUptake_GECROS
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds ActualNitrogenUptake_GECROS(EXP_POINTER)
   {
      int L,iL, iLayer;
      float fDepth, fnh4p, fno3p, fh2o, fpwc;
      float fnh4sup, fno3sup, fws, fnsup, fnh4up, fno3up, fnup;
      float frd, fpor, fndem, fnh4, fno3;
	  float fpd, f1, f2;
      float DeltaZ= pSo->fDeltaZ;

	  float fwcfcwcmin = (float)0.0;//sg/hmgu 20090326
      float fnh4r=(float)10.;//*10 ??? //--> ganzes Profil [kg(N)/ha], unabhängig von bulk density und Mächtigkeit
      float fno3r=(float)10.;//1.8,6.4;// *10 ??? --> ganzes Profil [kg(N)/ha]
	  //sg/hmgu 20090326 vgl. in CERES:
      //float fnh4r=(float)0.5 *(float)0.01*pSo->pSLayer->fBulkDens*DeltaZ; //--> schichtweise
      //float fno3r=(float)0.25*(float)0.01*pSo->pSLayer->fBulkDens*DeltaZ;


      PPLTNITROGEN      pPltN = pPl->pPltNitrogen;
      PLAYERROOT        pLR   = pPl->pRoot->pLayerRoot;
      PSLAYER           pSL   = pSo->pSLayer->pNext;
      PWLAYER           pSLW  = pWa->pWLayer->pNext; 
      PCLAYER           pSLN  = pCh->pCLayer->pNext;
      PSWATER           pSW   = pSo->pSWater->pNext;

      PGECROSNITROGEN  pGPltN = pGecrosPlant->pGecrosNitrogen;
	  PGECROSSOIL      pGS    = pGecrosPlant->pGecrosSoil;

    /*
      FWS    = MIN(1., WUL/(RD*10.*(WCFC-WCMIN)))
      NSUPAS = MAX (0., NAUL+(MINAUL-NITRUL)*TCP-RD/150.*RA)/TCP
      NSUPNS = MAX (0., NNUL+(MINNUL-DENIUL)*TCP-RD/150.*RN)/TCP*FWS
	  NSUPA  = INSW(NSWI, NINPA, NSUPAS)
      NSUPN  = INSW(NSWI, NINPN, NSUPNS)
      NSUPA  = MAX (0., NAUL+(MINAUL-NITRUL)*TCP-RD/150.*RA)/TCP
      NSUPN  = MAX (0., NNUL+(MINNUL-DENIUL)*TCP-RD/150.*RN)/TCP*FWS
      NSUP   = NSUPA + NSUPN

      NUPTA  = MIN(NSUPA, NSUPA/NOTNUL(NSUP)*MAX(0.,NDEM-NFIXR/TCP))
      NUPTN  = MIN(NSUPN, NSUPN/NOTNUL(NSUP)*MAX(0.,NDEM-NFIXR/TCP))
      NUPT   = MAX(0., NUPTA + NUPTN + MIN(NDEM, NFIXR/TCP))
    */

      L = 1;
      fDepth =(float)0;
      fh2o = fpor = fnh4p = fno3p = (float)0;

      while (((pPl->pRoot->fDepth*(float)10)>=fDepth)&&(L<=pSo->iLayers-2))// if pPl->pRoot->fDepth in [cm]
         {
            fnh4p += pSLN->fNH4N;
            fno3p += pSLN->fNO3N;
            fh2o  += pSLW->fContAct*DeltaZ;
            fpor  += pSL->fPorosity*DeltaZ;

			fwcfcwcmin += (pSW->fContFK-pSW->fContPWP)*DeltaZ;// sg/hmgu 20090326
            fDepth += DeltaZ;

            L++;
            pSLN = pSLN->pNext;
			pSLW = pSLW->pNext;
            pSL  = pSL->pNext;
         }
      iL=L-2;

	/////////////////////////////////////////////////////////////////////////
	//SG 20111103: letzte Schicht nur anteilsmäßig

	fh2o = fpor = fnh4p = fno3p = fwcfcwcmin = (float)0;
	fno3r = fnh4r = (float)0.0;
	
	for (pSLW=pWa->pWLayer->pNext,pSL=pSo->pSLayer->pNext,pSW=pSo->pSWater->pNext,pSLN  = pCh->pCLayer->pNext,iLayer=1;
		pSLW->pNext!=NULL,pSL!=NULL,pSW->pNext!=NULL,pSLN->pNext!=NULL;pSLW=pSLW->pNext,pSL=pSL->pNext,pSW=pSW->pNext,pSLN=pSLN->pNext,iLayer++)    
    {
     if (iLayer*DeltaZ <= pPl->pRoot->fDepth*(float)10)
	 {
	    fnh4p += pSLN->fNH4N;
        fno3p += pSLN->fNO3N;
        fh2o  += max(0.0,(pSLW->fContAct-pSW->fContPWP))*DeltaZ;
        fpor  += pSL->fPorosity*DeltaZ;
		fwcfcwcmin += (pSW->fContFK-pSW->fContPWP)*DeltaZ;

		//SG 20111103: non-extractable mineral nitrogen from bulk density and thickness 
		fnh4r +=(float)0.5 *(float)0.01*pSL->fBulkDens*DeltaZ; //--> schichtweise
		fno3r +=(float)0.25*(float)0.01*pSL->fBulkDens*DeltaZ;
	 }
	 else if ((pPl->pRoot->fDepth*(float)10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= pPl->pRoot->fDepth*(float)10.+DeltaZ))
     {
		f1 = (iLayer*DeltaZ-pPl->pRoot->fDepth*(float)10)/DeltaZ;

	    fnh4p += pSLN->fNH4N*((float)1-f1);
        fno3p += pSLN->fNO3N*((float)1-f1);
        fh2o  += max(0.0,(pSLW->fContAct-pSW->fContPWP))*DeltaZ*((float)1-f1);
        fpor  += pSL->fPorosity*DeltaZ*((float)1-f1);
		fwcfcwcmin += (pSW->fContFK-pSW->fContPWP)*DeltaZ*((float)1-f1);

 		fnh4r +=(float)0.5 *(float)0.01*pSL->fBulkDens*DeltaZ*((float)1-f1); //--> schichtweise
		fno3r +=(float)0.25*(float)0.01*pSL->fBulkDens*DeltaZ*((float)1-f1);
}
	 else
	 {
	    break;
	 }
	}//for
	// End SG 20111103
	/////////////////////////////////////////////////////////////////////////
	
	  frd  = pPl->pRoot->fDepth/pGS->fProfileDepth;
      fpwc = fpor/(float)NOTNUL((double)pPl->pRoot->fDepth)/(float)10;
      //fpwc = fwcfcwcmin;// sg/hmgu 20090326 
	  //SG 20111103: pGS->fPlantWaterCapacity wird nicht gesetzt. Deshalb fws aus schichtweiser Berechnung:
      fws  =(float) min((float)1.,fh2o/fwcfcwcmin);
	  //fws    = (float)min((float)1., pGS->fWaterContUpperLayer/NOTNUL(pPl->pRoot->fDepth
	  //                                                *(float)10.*pGS->fPlantWaterCapacity));

      //fnh4sup = max((float)0.,fnh4p-frd*fnh4r);
      //fno3sup = max((float)0.,fno3p-frd*fno3r)*fws;
      //SG 20111103: non-extractable nitrogen now related to single soil layers:
	  fnh4sup = max((float)0.,fnh4p-fnh4r);
      fno3sup = max((float)0.,fno3p-fno3r)*fws;
      fnsup   = fnh4sup + fno3sup;
	  pGPltN->fNPlantSupply = fnsup/10;//[kg N ha-1] --> [g N m-2]

      fndem   = max((float)0.,pGPltN->fNDemand-pGPltN->fNFixationReserve)*10;// [g N m-2] --> [kg N ha-1]
      fnh4up  = min(fnh4sup,fnh4sup/(float)NOTNUL((double)fnsup)*fndem); 
      fno3up  = min(fno3sup,fno3sup/(float)NOTNUL((double)fnsup)*fndem);
      fnup    = max((float)0.,fnh4up+fno3up+min(pGPltN->fNDemand,pGPltN->fNFixationReserve)*10); 


    if (pGPltN->fNDemand<=(float)0)
    {
     pSL = pSo->pSLayer->pNext;// sg/hmgu 20090326
     
	 for (L=1;L<=pSo->iLayers-2;L++)
        {
         pLR->fActLayNO3NUpt = (float)0;
         pLR->fActLayNH4NUpt = (float)0;

         pSL=pSL->pNext;
         pLR=pLR->pNext;
        }
     
      pPltN->fActNO3NUpt = (float)0;
      pPltN->fActNH4NUpt = (float)0;
      pPltN->fActNUptR = (float)0;
    }
    else
/*
    {
      pPltN->fActNO3NUpt = (float)0;
      pPltN->fActNH4NUpt = (float)0;

      pPltN = pPl->pPltNitrogen;
      pLR   = pPl->pRoot->pLayerRoot;
      pSL   = pSo->pSLayer->pNext;
      pSLW  = pWa->pWLayer->pNext; 
      pSLN  = pCh->pCLayer->pNext;
      //frd = max((float)1.,pPl->pRoot->fDepth*(float)10);
      fDepth = (float)DeltaZ;

	  for (L=1;L<=pSo->iLayers-2;L++)
         {
          // sg/hmgu 20090326
          frd = min((float)DeltaZ,max((float)0.0,pPl->pRoot->fDepth*(float)10.0-fDepth+(float)DeltaZ));
          frd = min((float)1.,frd/pPl->pRoot->fDepth/(float)10.0);

          fnh4 = min(fnh4up*frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
          fno3 = min(fno3up*frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
		  //fnh4 = min(fnh4up*DeltaZ/frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/frd));
          //fno3 = min(fno3up*DeltaZ/frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/frd));
          //fnh4 = min(fnh4up*frd,max((float)0.,pSLN->fNH4N-fnh4r));
          //fno3 = min(fno3up*frd,max((float)0.,pSLN->fNO3N-fno3r));
          //fnh4 = min(fnh4up/frd,pSLN->fNH4N-fnh4r/frd);
          //fno3 = min(fno3up/frd,pSLN->fNO3N-fno3r/frd);

		  if((pPl->pRoot->fDepth*(float)10)>=(fDepth-DeltaZ))
          {
             pLR->fActLayNH4NUpt = fnh4;
             pLR->fActLayNO3NUpt = fno3; 
             pPltN->fActNH4NUpt += fnh4;
             pPltN->fActNO3NUpt += fno3;
          }
          else
          {
             pLR->fActLayNH4NUpt = (float)0;
             pLR->fActLayNO3NUpt = (float)0; 
          }

          //uptake from soil --> in plant.c
          //pSLN->fNH4N -= pLR->fActLayNH4NUpt;
          //pSLN->fNO3N -= pLR->fActLayNO3NUpt;

		  
          fDepth += DeltaZ;// sg/hmgu

          pSL =pSL->pNext;
          pSLW=pSLW->pNext;
          pSLN=pSLN->pNext;
          pLR =pLR->pNext;
         }//for
      }
//*/
///*
	{
      //+++ output to expertn +++//
	  fpd =(pSo->iLayers-2)*DeltaZ;
	  frd = min(fpd-DeltaZ,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));
	  f2  = max((float)1,(fpd-frd)/DeltaZ);

      pSLN  = pCh->pCLayer->pNext;
      pLR   = pPl->pRoot->pLayerRoot;
      pPltN = pPl->pPltNitrogen;
	  pPltN->fActNH4NUpt = (float)0;
	  pPltN->fActNO3NUpt = (float)0;

	  for (L=1;L<=pSo->iLayers-2;L++)
      {
       //fnh4 = min(fnh4up*DeltaZ/frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       //fno3 = min(fno3up*DeltaZ/frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       //fnh4 = min(fnh4up*DeltaZ/frd,max((float)0.,pSLN->fNH4N));
       //fno3 = min(fno3up*DeltaZ/frd,max((float)0.,pSLN->fNO3N));
       //fnh4 = fnh4up*DeltaZ/frd;
       //fno3 = fno3up*DeltaZ/frd;
       //fnh4 = fnh4up*(pSLN->fNH4N/fnh4p)*iL*DeltaZ/frd;
       //fno3 = fno3up*(pSLN->fNO3N/fno3p)*iL*DeltaZ/frd;
       //fnh4 = min(fnh4up*(pSLN->fNH4N/fnh4p)*fDepth/frd,max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       //fno3 = min(fno3up*(pSLN->fNO3N/fno3p)*fDepth/frd,max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       fnh4 = min(fnh4up*(pSLN->fNH4N/fnh4p),max((float)0.,pSLN->fNH4N-fnh4r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       fno3 = min(fno3up*(pSLN->fNO3N/fno3p),max((float)0.,pSLN->fNO3N-fno3r*DeltaZ/pGS->fProfileDepth/(float)10.0));
       //fnh4 = fnh4up*(pSLN->fNH4N/fnh4p);
       //fno3 = fno3up*(pSLN->fNO3N/fno3p);

       if(L*DeltaZ <= frd) 
	   {
        pLR->fActLayNH4NUpt = fnh4;//*DeltaZ/frd;
        pLR->fActLayNO3NUpt = fno3;//*DeltaZ/frd;
	   }
	   else if ((frd < L*DeltaZ)&&(L*DeltaZ <= frd + DeltaZ))
       {
        f1 = (L*DeltaZ-frd)/DeltaZ;
        pLR->fActLayNH4NUpt = (((float)1-f1)*fnh4);//*DeltaZ/frd);
        pLR->fActLayNO3NUpt = (((float)1-f1)*fno3);//*DeltaZ/frd);
	   }
	   else
	   {
		pLR->fActLayNH4NUpt = (float)0;
        pLR->fActLayNO3NUpt = (float)0;
	   }//if,else if,else
	   
	   pPltN->fActNH4NUpt += pLR->fActLayNH4NUpt;
       pPltN->fActNO3NUpt += pLR->fActLayNO3NUpt;

       if(pLR->pNext!=NULL)  pLR =pLR->pNext;
	   if(pSLN->pNext!=NULL) pSLN=pSLN->pNext;

	  }//for
	}//*/

      pPltN->fActNUpt= pPltN->fActNH4NUpt + pPltN->fActNO3NUpt;
      pPltN->fActNUptR=pPltN->fActNUpt;//per day, i.e. dt=1 !

      return 1;    
      }


/*******************************************************************************
** EOF */