/*******************************************************************************
 *
 * Copyright (c) by Helmholtz Zentrum München
 *                  German Research Centre for Environmental Health
 *                  Institute of Soil Ecology
 *                  P.O.Box 1129
 *                  D-85758 Neuherberg
 *
 * Author: E. Priesack (ep)
 *
 *------------------------------------------------------------------------------
 *
 * Description: Calculation of Soil Water Flow for GEOSTEP following GECROS
 *
 *------------------------------------------------------------------------------
 *
*******************************************************************************/
#include  "gecros.h"
extern double WINAPI _loadds NOTNUL(double x);
extern double WINAPI _loadds INSW(double x,double y1,double y2);

int SoilWaterFlow_GECROS(EXP_POINTER)
{
 PGECROSSOIL  pGS = pGecrosPlant->pGecrosSoil;
 PWLAYER      pWL = pWa->pWLayer;
	
 extern float fActTraDay;
 extern double SD1;
 
 int iLayer;
 float f1 = (float)0;
 float DeltaZ = pSo->fDeltaZ;
 float fProfileDepth=(float)0;
 
 double RFIR;
 double WCI,MULTF,RDI,WUL,WLL;
 double WCMIN,WCMAX,WCPWC,TCP,TCT; 
 double WCUL,WCLL,RD;
 double RRUL,RRLL,RWUL,RWLL,RRD;
 double RWUG,DWSUP;
 double ATCAN,AESOIL;
 double WSWI,WINPUT;
 //double WCFC = (double)0.25;//test

 extern float fparPRFDPT,fparWCMIN,fparWCMAX,fparWCPWC,fparSD1,fparTCT,fparTCP;

/* input parameters */
      WSWI = (double)+1.;//test -1.; else +1.;
      WINPUT = (double)15.;

 //Soil parameters part I (from readmod marker 80009)
      if (fparPRFDPT ==(float)-99) pGS->fProfileDepth = (float)150.;
	  else pGS->fProfileDepth = fparPRFDPT;

      if (fparWCMIN ==(float)-99) WCMIN = (double)0.05;
	  else WCMIN    = (double)fparWCMIN;

      if (fparWCPWC ==(float)-99) WCPWC = (double)0.25;
	  else WCPWC    = (double)fparWCPWC/NOTNUL((double)fparPRFDPT*10);
      pGS->fPlantWaterCapacity = (float)WCPWC;

	  if (fparWCMAX ==(float)-99) WCMAX = (double)0.35;
	  else WCMAX    = (double)fparWCMAX;
      WCMAX = (double)pGS->fPlantWaterCapacity + WCMIN;
      //WCMAX = (double)0.35;//test
	  
	  if (fparSD1 ==(float)-99) SD1 = (double)25.0;
	  else SD1    = (double)fparSD1;

      if (fparTCT ==(float)-99) TCT = (double)4;
	  else TCT    = (double)fparTCT;

      if (fparTCP ==(float)-99) TCP = (double)1;
	  else TCP    = (double)fparTCP;

	  MULTF = (double)1;//pGS->fWaterContInitFac;


	  for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)
	  {
		  fProfileDepth+=DeltaZ;
	  }

/* initial */
      if (SimStart(pTi))
	  {
	     RDI    = max(2.,SD1);
	     if(pPl!=NULL) RDI = max(RDI,min(100,(double)pPl->pRoot->fDepth));
	     WCI    = WCPWC * MULTF;//pGS->fWaterContInitial;
	     //WCI    = WCFC * MULTF;//test
         WUL    = 10.*(WCI-WCMIN)*RDI;
         WLL    = 10.*(WCI-WCMIN)*((double)pGS->fProfileDepth-RDI);

		 fActTraDay = (float)0;
		 pWa->pEvap->fActR = (float)0;

         pGS->fWaterContUpperLayer = (float)WUL;
         pGS->fWaterContLowerLayer = (float)WLL;
		 pWa->pWBalance->fProfilStart = (float)(WUL + WLL);
		    
		 for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
         {
         pWL->fContAct = (float)(WUL + WLL)/fProfileDepth;
         }

         /*    
		 for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
         {
          if (iLayer*DeltaZ <= (float)(RDI*10.)) 
	        //pWa->pWLayer->fContAct=(float)WUL;
	        WUL += (double)pWL->fContAct*DeltaZ/NOTNUL(RDI*10.);
	      else
	        //pWa->pWLayer->fContAct=(float)WLL;
	        WLL += (double)pWL->fContAct*DeltaZ/NOTNUL(1500.-RDI*10.);
         }
         */
	  }// if SimStart


/* daily input */
      RFIR   = (double)pWa->pWBalance->fReservoir;//RAIN + IRRI
	  RFIR   = (double)pCl->pWeather->fRainAmount;
	  RDI    = max(2.,SD1);
	  if(pPl!=NULL)
	  {
        RD   = min(((double)fProfileDepth-10.)/10.,max(RDI,(double)pPl->pRoot->fDepth));// /10.;//[mm]->[cm]???
        //RD   = min(((double)fProfileDepth)/10.,max(RDI,(double)pPl->pRoot->fDepth));// /10.;//[mm]->[cm]???
	    RRD  = (double)pPl->pRoot->fDepthGrowR;// /10.;//[mm]->[cm]???
	  }
       
	  WUL    = (double)pGS->fWaterContUpperLayer;
	  WLL    = (double)pGS->fWaterContLowerLayer;

      //pWa->pEvap->fActR = min(pWa->pEvap->fActR,(float)(WUL*0.01));//Limitierung noch unklar 1% WUL oder höher???	  
	  AESOIL = (double)pWa->pEvap->fActR;
	  ATCAN  = (double)fActTraDay;//???
	  if (pPl != NULL) ATCAN  = (double)pPl->pRoot->fUptakeR;
	  //if ((pPl != NULL)&& (PlantIsGrowing)) ATCAN  = (double)pPl->pRoot->fUptakeR;//???

/* soil water */
      WCUL   = (WUL+WCMIN*10.*RD)/10./NOTNUL(RD);
      WCLL   = min(WCMAX, (WLL+WCMIN*10.*((double)pGS->fProfileDepth-RD))/10./NOTNUL((double)pGS->fProfileDepth-RD));
      //WCLL   = min(WCMAX, (WLL+WCMIN*10.*(150.-RD))/10./NOTNUL(150.-RD));//test

      RRUL   = min(10.*(WCMAX-WCUL)*                            RD /TCP, RFIR);
      RRLL   = min(10.*(WCMAX-WCLL)*((double)pGS->fProfileDepth-RD)/TCP, RFIR-RRUL);
      //RRLL   = min(10.*(WCMAX-WCLL)*(150.-RD)/TCP, RFIR-RRUL);//test

      RWUL   = RRUL+10.*(WCLL-WCMIN)*RRD-INSW(WSWI,0.,ATCAN+AESOIL);//+.1;
      //RWUL   = RRUL+10.*(WCLL-WCMIN)*RRD-AESOIL;//+.1;
      RWLL   = RRLL-10.*(WCLL-WCMIN)*RRD;
      RWUG   = max(0., RFIR-RRUL-RRLL);

      WUL    = max(0.,WUL+RWUL);
      WLL    = max(0.,WLL+RWLL);
      DWSUP  = INSW(WSWI, WINPUT, max(0.1,WUL/TCP+0.1));

/* daily output */
	  pGS->fWaterContUpperLayer = (float)WUL;
      pGS->fWaterContLowerLayer = (float)WLL;
      pGS->fETDailyWaterSupply  = (float)DWSUP;
	  pGS->fWaterFlowToLowLayer = (float)RRUL;

	  pWa->fInfiltR             = (float)RFIR;
	  pWa->fPercolR             = (float)RWUG;
	  //pWa->pEvap->fActR         = (float)AESOIL;
	  //pPl->pRoot->fUptakeR      = (float)ATCAN;
	  pWa->pWLayer->fFluxDens   = pWa->fInfiltR;//for fertilization in transp.c
      pWa->pWLayer->fFlux       = pWa->fInfiltR;//for fertilization in transp.c
      //Zeiger auf vorletzte Schicht setzen
	  //for (pWL = pWa->pWLayer;(pWL->pNext->pNext!=NULL);pWL = pWL->pNext);
      //pWL->fFlux                = pWa->fPercolR;//for leaching in transp.c

   ///*
   if (RD > 0)
   {
    for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
    {
     if (iLayer*DeltaZ <= (float)(RD*10.)) 
	    pWL->fContAct=(float)(WUL/(RD*10.));
	 else if (((float)RD*10 < iLayer*DeltaZ)&&(iLayer*DeltaZ <= (float)RD*10. + DeltaZ))
     {
        f1 = (iLayer*DeltaZ-(float)(RD*10.))/DeltaZ;
	    pWL->fContAct=((float)1-f1)*(float)(WUL/(RD*10.))
			          //+ f1*(float)(WLL/NOTNUL((double)fProfileDepth-RD*10.));
			          + f1*(float)(WLL/((double)fProfileDepth-RD*10.));
     }
	 else
		//pWL->fContAct=(float)(WLL/NOTNUL((double)fProfileDepth-RD*10.));
		pWL->fContAct=(float)(WLL/((double)fProfileDepth-RD*10.));

     }
   }
   else //*/
   {
    for (pWL=pWa->pWLayer->pNext,iLayer=1;pWL->pNext!=NULL;pWL=pWL->pNext,iLayer++)    
    {
     pWL->fContAct = (float)(WUL + WLL)/fProfileDepth;
    }
   }

	return 1;
}

/********************************************************************************************/
/* Procedur    :   SoilNitrogen_GECROS()                                                    */
/* Op.-System  :   DOS                                                                      */
/* Beschreibung:   Bodenstickstofftransport mit Mineralisierung                             */
/*                 Methode GECROS (Yin and van Laar 2005)                                   */
/*                                                                                          */
/*                 HMGU/ep              2008                                                */
/*                                                                                          */
/********************************************************************************************/
/* veränd. Var.		pCL->fCFOMFast        pCL->fNFOMFast                                    */
/********************************************************************************************/
#include  "gecros.h"
extern double WINAPI _loadds NOTNUL(double x);
extern double WINAPI _loadds FCNSW(double x,double y1,double y2,double y3);
extern double WINAPI _loadds INSW(double x,double y1,double y2);
extern double WINAPI _loadds LIMIT(double y1,double y2,double x);


int SoilNitrogen_GECROS(EXP_POINTER)
{
	  PGECROSNITROGEN  pGPltN = pGecrosPlant->pGecrosNitrogen;
	  PGECROSSOIL      pGS    = pGecrosPlant->pGecrosSoil;

      /*** for output to expertn ***/
      PSLAYER      pSL   = pSo->pSLayer->pNext;
      PWLAYER      pSLW  = pWa->pWLayer->pNext; 
      PCLAYER      pSLN  = pCh->pCLayer->pNext;
	  PCPROFILE    pCP   = pCh->pCProfile;

	  PLAYERROOT pLR;
      //if (pPl != NULL) pLR=pPl->pRoot->pLayerRoot;
      //PPLTNITROGEN pPltN = pPl->pPltNitrogen;


      /*** local variables ***/
	  double RD,RRD,RAIN;
	  double WCMAX,RFIR,RRUL,RWUG;
	  double DPM,RDPM,RPM,RRPM,BIO,RBIO,HUM,RHUM,LITC,LITN,DPN,RDPN,RPN,RRPN;
	  double DECDPM,DECRPM,DECBIO,DECHUM,CNDRPM,DECDPN,DECRPN;
	  double DPMR,RPMR,DPMRC,RPMRC,RESCO2;
	  double NAUL,NALL,NNUL,NNLL,RNAUL,RNALL,RNNUL,RNNLL;
	  //double NA,NN,NMINER;
	  double NITRUL,NITRLL,DENIUL,DENILL,FMUL,FMLL,NUPTA,NUPTN,NUPT,NDEM,NFIXR;
   	  double WUL,WLL,FERNA,SFERNA,RSFNA,VOLA,FERNN;
	  double CBH,FT,FM;
	  double FWS,NSUPAS,NSUPNS,NSUPA,NSUPN,NSUP;
	  double LAYNA,LAYNN;
	  double LEAUL,LEALL;
	  double MDN,MDNUL,MDNLL;
	  double MINAUL,MINALL,MINNUL,MINNLL;
	  double TSOIL,RTSOIL,TAVSS,TMAX,TMIN,DAVTMP,NAVTMP,DIFS;
      
	  /*** fixed parameters ***/ 
	  double DRPM, DPMR0, RPMR0, TOC, BHC, FBIOC;
	  double CLAY, WCMIN, WCPWC, TCP, TCT, BIOR, HUMR;
	  /*
	  double DRPM =(double)1.44, DPMR0 =(double)10., RPMR0 =(double)0.3, BIOR =(double)0.66, HUMR =(double)0.02;
	  double TOC   = (double)7193.0, BHC = (double) 3500.0, FBIOC = (double)0.03;
	  double CLAY  = (double)23.4, TCP = (double)1., TCT = (double)4., WCMIN = (double)0.05, WCPWC = (double)25.;
	  */
      double SD1 = (double)25.; 
      double RA = (double)1., RN = (double)1.;
      
	  //zum Testen:
	  //double WCFC  = (double)0.25;//test
	  double NINPA = (double)0., NINPN = (double)0.65;
	  double NSWI  = (double)+1.;//test: -1. sonst +1.
      
	  /*** for initialisation ***/
      extern float fparSD1,fparTOC,fparBHC,fparFBIOC;
	  extern float fparPRFDPT,fparCLAY,fparWCMIN,fparWCPWC,fparTCT,fparTCP;
	  extern float fparBIOR,fparHUMR,fparDRPM,fparDPMR0,fparRPMR0;
      extern float fparTOC,fparBHC,fparFBIOC;

	  //float fRA, fRN, fNAI, fNNI;

	  /*** for output to expertn ***/
	  int L;
	  float f1,f2,frd,fpd,DeltaZ;
	  DeltaZ = pSo->fDeltaZ;
	  if (pPl != NULL) pLR=pPl->pRoot->pLayerRoot;

	  //*** initialisation N contents
	  if (SimStart(pTi))
	  {
	   NAUL = (double)0;
	   NNUL = (double)0;
	   NALL = (double)0;
	   NNLL = (double)0;
       //pCP->dNO3LeachCum = (double)0;// in yinit.c
	   frd=(float)0;
	   fpd = (pSo->iLayers-2)*DeltaZ;

	   if (fparSD1==(float)-99) fparSD1 = (float)SD1;
	   
	   if (pPl!=NULL)
	   {
        pPl->pRoot->fDepth = (float)max(2.,(double)fparSD1);
        frd = min(fpd,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));
	   }
	   else
	   {
        frd = (float)max(2.,(double)fparSD1*10.);
	   }

	   f2  = (float)NOTNUL(fpd-frd)/DeltaZ;

	   for (L=1;L<=pSo->iLayers-2;L++)
       {
        if(L*DeltaZ <= frd) 
	    {
		 NAUL += (double)pSLN->fNH4N/10.;
		 NNUL += (double)pSLN->fNO3N/10.;
	    }
	    else if ((frd < L*DeltaZ)&&((L-1)*DeltaZ <= frd))
        {
         f1 = (L*DeltaZ-frd)/DeltaZ;
		 NAUL += (double)(pSLN->fNH4N*((float)1-f1))/10.;
		 NALL += (double)(pSLN->fNH4N*f1)/10.;
         NNUL += (double)(pSLN->fNO3N*((float)1-f1))/10.;
		 NNLL += (double)(pSLN->fNO3N*f1)/10.;
	    }
	    else
	    {
         NALL += (double)pSLN->fNH4N/10.;             
		 NNLL += (double)pSLN->fNO3N/10.;
	    }//if,else if,else

		pSLN=pSLN->pNext;
	   }//for

	  pGS->fNH4NContUpperLayer = (float)NAUL;
      pGS->fNH4NContLowerLayer = (float)NALL;
	  pGS->fNO3NContUpperLayer = (float)NNUL;
	  pGS->fNO3NContLowerLayer = (float)NNLL;

	  /*
	  DPMI   = ZERO
      RPMI   = TOC - BHC - DPMI
      BIOI   = FBIOC * TOC
      HUMI   = BHC - BIOI

	  DPNI   = 1./ 40.*DPMI
      RPNI   = 1./100.*RPMI
	  */

	  ///*
	  if (fparTOC==(float)-99) fparTOC = (float)7000;
	  TOC = (double)fparTOC;

	  if (fparBHC==(float)-99) fparBHC = (float)3500;
	  BHC = (double)fparBHC;

	  if (fparFBIOC==(float)-99) fparFBIOC = (float)0.03;
	  FBIOC = (double)fparFBIOC;

	  pGS->fDecPlantMatC = (float)0.;
	  pGS->fResPlantMatC = fparTOC - fparBHC - pGS->fDecPlantMatC;
	  pGS->fMicroBiomC   = fparFBIOC * fparTOC;
	  pGS->fHumusC       = fparBHC - fparFBIOC * fparTOC;

	  pGS->fDecPlantMatN = (float)1./(float)40.  * pGS->fDecPlantMatC;
      pGS->fResPlantMatN = (float)1./(float)100. * pGS->fResPlantMatC;
	  //*/

      /*
      NAI    = 2.
      NNI    = 2.
      NAULI  = (1.-EXP(-0.065*RDI))*NAI +     RDI/150. *RA
      NALLI  =     EXP(-0.065*RDI) *NAI + (1.-RDI/150.)*RA
      NNULI  = (1.-EXP(-0.065*RDI))*NNI +     RDI/150. *RN
      NNLLI  =     EXP(-0.065*RDI) *NNI + (1.-RDI/150.)*RN
      */
      /*
      fNAI = pGS->fNH4NContUpperLayer + pGS->fNH4NContLowerLayer;
	  fNNI = pGS->fNO3NContUpperLayer + pGS->fNO3NContLowerLayer;
	  fRA  = (float)RA;
	  fRN  = (float)RN;
	  fNAI = (float)2.;
	  fNNI = (float)2.;
	  pGS->fNH4NContUpperLayer = ((float)1.-(float)exp(-0.065*(double)frd))* fNAI +           frd/fparPRFDPT *fRA;
      pGS->fNH4NContLowerLayer =            (float)exp(-0.065*(double)frd) * fNAI +((float)1.-frd/fparPRFDPT)*fRA;
	  pGS->fNO3NContUpperLayer = ((float)1.-(float)exp(-0.065*(double)frd))* fNNI +           frd/fparPRFDPT *fRN;
	  pGS->fNO3NContLowerLayer =            (float)exp(-0.065*(double)frd) * fNNI +((float)1.-frd/fparPRFDPT)*fRN;
	  */

	  }//SimStart

      /* input from readmod soil parameters I & II: marker 80009, 80010*/
	  if (fparPRFDPT==(float)-99) fparPRFDPT = (float)150.;
	  pGS->fProfileDepth = fparPRFDPT;

      if (fparCLAY==(float)-99) fparCLAY = (float)23.4;
	  CLAY  = (double)fparCLAY;

	  if (fparWCMIN==(float)-99) fparWCMIN = (float)0.05;
	  WCMIN  = (double)fparWCMIN;

      if (fparWCPWC==(float)-99) fparWCPWC = (float)375;
	  WCPWC  = (double)fparWCPWC/NOTNUL((double)fparPRFDPT*10);
      pGS->fPlantWaterCapacity = (float)WCPWC;
      //pGS->fPlantWaterCapacity = (float)WCFC;//test

	  if (fparSD1==(float)-99) fparSD1 = (float)25.0;
	  SD1    = (double)fparSD1;

      if (fparTCT==(float)-99) fparTCT = (float)4;
	  TCT    = (double)fparTCT;

      if (fparTCP==(float)-99) fparTCP = (double)1;
	  TCP    = (double)fparTCP;

      if (fparBIOR==(float)-99) fparBIOR = (float)0.66;
	  BIOR   = (double)fparBIOR;

      if (fparHUMR==(float)-99) fparHUMR = (float)0.02;
	  HUMR   = (double)fparHUMR;

      if (fparDRPM==(float)-99) fparDRPM = (float)1.44;
	  DRPM   = (double)fparDRPM;

	  if (fparDPMR0==(float)-99) fparDPMR0 = (float)10.0;
	  DPMR0  = (double)fparDPMR0;

      if (fparRPMR0==(float)-99) fparRPMR0 = (float)0.3;
	  RPMR0  = (double)fparRPMR0;
	  


	  /*** input from expertn global variables ***/
	  if (pPl!=NULL) RD     = (double)pPl->pRoot->fDepth;
	  if (pPl!=NULL) RRD    = (double)pPl->pRoot->fDepthGrowR;
	  //NUPTN = (double)pPltN->fActNO3NUpt/10.0;//[kg N ha-1 d-1] --> [g N m-2 d-1]
      //NUPTA = (double)pPltN->fActNH4NUpt/10.0;//[kg N ha-1 d-1] --> [g N m-2 d-1]
	  RAIN   = (double)pCl->pWeather->fRainAmount;
	  //RFIR  = (double)pWa->pWBalance->fReservoir;//RAIN + IRRI
	  RFIR   = (double)pWa->fInfiltR;
      RWUG   = (double)pWa->fPercolR;

      /*** input from GECROS global variables ***/
	  RRUL   = (double)pGS->fWaterFlowToLowLayer;
	  DIFS   = (double)pGS->fDiffSoilAirTemp;
	  TSOIL  = (double)pGS->fSoilTemp;//pHe->pHBalance->fProfil ???;
      RTSOIL = (double)pGS->fSoilTempR;

      DPM  = (double)pGS->fDecPlantMatC;
	  RDPM = (double)pGS->fDecPltMCRate;
      RPM  = (double)pGS->fResPlantMatC;
	  RRPM = (double)pGS->fResPltMCRate;
	  BIO  = (double)pGS->fMicroBiomC;
	  HUM  = (double)pGS->fHumusC;
	  RBIO = (double)pGS->fMicBiomCRate;
	  RHUM = (double)pGS->fHumusCRate;
	  LITC = (double)pGS->fLitterC;
	  LITN = (double)pGS->fLitterN;

	  //DECDPM = (double)pGS->fDecPltMCDecR;
	  //DECRPM = (double)pGS->fResPltMCDecR;
	  //DECBIO = (double)pGS->fMicBiomCDecR;
	  //DECHUM = (double)pGS->fHumusCDecR;
	  //DECDPN = (double)pGS->fDecPltMNDecR;
	  //DECRPN = (double)pGS->fResPltMNDecR;

	  DPN    = (double)pGS->fDecPlantMatN;
      RDPN   = (double)pGS->fDecPltMNRate;
	  RPN    = (double)pGS->fResPlantMatN;
	  RRPN   = (double)pGS->fResPltMNRate;
	  CNDRPM = (double)pGS->fCNPlantMat;

	  WUL   = (double)pGS->fWaterContUpperLayer;
	  WLL   = (double)pGS->fWaterContLowerLayer;

      //*** NO3-N and NH4-N content ***
	  //*** upperlayer(UL) and lower layer (LL) ***
	  RNAUL  = (double)pGS->fNH4NContUpLayRate;
      RNALL  = (double)pGS->fNH4NContLwLayRate;
	  RNNUL  = (double)pGS->fNO3NContUpLayRate;
	  RNNLL  = (double)pGS->fNO3NContLwLayRate;
	  NAUL   = (double)0;//pGS->fNH4NContUpperLayer;
      NALL   = (double)0;//pGS->fNH4NContLowerLayer;
	  NNUL   = (double)0;//pGS->fNO3NContUpperLayer;
	  NNLL   = (double)0;//pGS->fNO3NContLowerLayer;

	  fpd = (pSo->iLayers-2)*DeltaZ;        
	  frd = min(fpd,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));

	  pSLN  = pCh->pCLayer->pNext;
	   for (L=1;L<=pSo->iLayers-2;L++)
       {
        if(L*DeltaZ <= frd) 
	    {
		 NAUL += (double)pSLN->fNH4N/10.;
		 NNUL += (double)pSLN->fNO3N/10.;
	    }
	    else if ((frd < L*DeltaZ)&&((L-1)*DeltaZ <= frd))
        {
         f1 = (L*DeltaZ-frd)/DeltaZ;
		 NAUL += (double)(pSLN->fNH4N*((float)1-f1))/10.;
		 NALL += (double)(pSLN->fNH4N*f1)/10.;
         NNUL += (double)(pSLN->fNO3N*((float)1-f1))/10.;
		 NNLL += (double)(pSLN->fNO3N*f1)/10.;
	    }
	    else
	    {
         NALL += (double)pSLN->fNH4N/10.;             
		 NNLL += (double)pSLN->fNO3N/10.;
	    }//if,else if,else

		pSLN=pSLN->pNext;
	   }//for

	  FERNA  = (double)pGS->fNH4NFertilizer;
	  SFERNA = (double)pGS->fNH4NVolatFert;
	  RSFNA  = (double)pGS->fNH4NVolatFertR;
	  VOLA   = (double)pGS->fNH4NVolatilizRate;
	  FERNN  = (double)pGS->fNO3NFertilizer;

	  NDEM  = (double)pGPltN->fNDemand;        	  
	  NFIXR = (double)pGPltN->fNFixationReserve;
    
	  /*** soil temperature ***/
	  TMAX = (double) pCl->pWeather->fTempMax;//[°C]
      TMIN = (double) pCl->pWeather->fTempMin;//[°C]
	  DAVTMP = 0.29*TMIN + 0.71*TMAX;
	  NAVTMP = 0.71*TMIN + 0.29*TMAX;

	  TAVSS  = ((DAVTMP+DIFS)+NAVTMP)/2.;
	  RTSOIL = (TAVSS - TSOIL)/TCT;

	  /*** environmental impact factors ***/
	  CBH    = 1.67*(1.85+1.60*exp(-0.0786*CLAY));
      FT     = 47.9/(1.  +exp(106./(TSOIL+18.3)));
      FM     = LIMIT(0.2, 1.0, 0.2+0.8*(WUL+WLL)/10./(double)pGS->fProfileDepth/(double)pGS->fPlantWaterCapacity);
      //FM     = LIMIT(0.2, 1.0, 0.2+0.8*(WUL+WLL)/10./(WCFC-WCMIN));//test
	  /*** Soil organic carbon ***/
	  CNDRPM = (DPM+RPM)/NOTNUL(DPN+RPN);

      DPMRC  = INSW(NNUL+NAUL+NNLL+NALL-RA-RN, 0., DPMR0);
      RPMRC  = INSW(NNUL+NAUL+NNLL+NALL-RA-RN, 0., RPMR0);

	  DPMR   = INSW(1./NOTNUL(CNDRPM)-1./8.5/(1.+CBH), DPMRC, DPMR0);
      RPMR   = INSW(1./NOTNUL(CNDRPM)-1./8.5/(1.+CBH), RPMRC, RPMR0);

      DECDPM = DPM*(1.-exp(-FT*FM*DPMR/365.))/TCP;
      DECRPM = RPM*(1.-exp(-FT*FM*RPMR/365.))/TCP;
      DECBIO = BIO*(1.-exp(-FT*FM*BIOR/365.))/TCP;
      DECHUM = HUM*(1.-exp(-FT*FM*HUMR/365.))/TCP;

	  RDPM   = LITC*DRPM/(1.+DRPM) - DECDPM;
      RRPM   = LITC*1.  /(1.+DRPM) - DECRPM;
      RBIO   = 0.46/(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM) - DECBIO;
      RHUM   = 0.54/(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM) - DECHUM;
	  RESCO2 = CBH /(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM);


	  /*** Soil organic nitrogen ***/
      DECDPN = DPN*(1.-exp(-FT*FM*DPMR/365.))/TCP;
      DECRPN = RPN*(1.-exp(-FT*FM*RPMR/365.))/TCP;

      RDPN   = LITN/(1.+ 40./DRPM/100.) - DECDPN;
      RRPN   = LITN/(1.+100.*DRPM/40. ) - DECRPN;



	  /*** Soil mineral nitrogen ***/
      MDN    = 1./8.5*(DECBIO+DECHUM)+ DECDPN+DECRPN -
               1./8.5/(1.+CBH)*(DECDPM+DECRPM+DECBIO+DECHUM);
	  MDNUL  = (1.-exp(-0.065*RD))*MDN;
      MDNLL  =     exp(-0.065*RD) *MDN;

      MINAUL = INSW(MDN,-min((NAUL-      RD /(double)pGS->fProfileDepth*RA)/TCP,-MDNUL),MDNUL);
      MINALL = INSW(MDN,-min((NALL-((double)pGS->fProfileDepth-RD)
		                                    /(double)pGS->fProfileDepth*RA)/TCP,-MDNLL),MDNLL);
      MINNUL = INSW(MDN,-min(NNUL/TCP,-MDNUL+MINAUL), 0.);
      MINNLL = INSW(MDN,-min(NNLL/TCP,-MDNLL+MINALL), 0.);

      FMUL   = LIMIT(0.2, 1.0, 0.2+0.8*WUL/10./      RD /(double)pGS->fPlantWaterCapacity);
      FMLL   = LIMIT(0.2, 1.0, 0.2+0.8*WLL/10./((double)pGS->fProfileDepth-RD)/(double)pGS->fPlantWaterCapacity);

	  NITRUL = max(0.,(NAUL+MINAUL*TCP-RD /(double)pGS->fProfileDepth*RA))*(1.-exp(-FT*FMUL*0.6/7.))/TCP;
      NITRLL = max(0.,(NALL+MINALL*TCP-
		               ((double)pGS->fProfileDepth-RD)/(double)pGS->fProfileDepth*RA))*(1.-exp(-FT*FMLL*0.6/7.))/TCP;
      			   
      DENIUL = .0005*max(0.,NNUL+MINNUL*TCP-RD /(double)pGS->fProfileDepth*RN)* RESCO2*(1.-exp(-0.065*RD));
      DENILL = .0005*max(0.,NNLL+MINNLL*TCP-
		                    ((double)pGS->fProfileDepth-RD)/(double)pGS->fProfileDepth*RN)* RESCO2*exp(-0.065*RD);
      
      FWS    = min(1., WUL/NOTNUL(RD*10.*(double)pGS->fPlantWaterCapacity));

      NSUPAS = max (0., NAUL+(MINAUL-NITRUL)*TCP-RD/(double)pGS->fProfileDepth*RA)/TCP;
      NSUPNS = max (0., NNUL+(MINNUL-DENIUL)*TCP-RD/(double)pGS->fProfileDepth*RN)/TCP*FWS;
      NSUPA  = INSW(NSWI, NINPA, NSUPAS);
      NSUPN  = INSW(NSWI, NINPN, NSUPNS);
	  NSUP   = NSUPA + NSUPN;

	  NUPTA  = min(NSUPA, NSUPA/NOTNUL(NSUP)*max(0.,NDEM-NFIXR/TCP));
      NUPTN  = min(NSUPN, NSUPN/NOTNUL(NSUP)*max(0.,NDEM-NFIXR/TCP));
      NUPT   = max(0., NUPTA + NUPTN + min(NDEM, NFIXR/TCP));

	  WCMAX  = (double)pGS->fPlantWaterCapacity + WCMIN;

      LEAUL  = max(0.,(NSUPN-NUPTN)*TCP-RD/(double)pGS->fProfileDepth*RN)
		       *min((RFIR-RRUL)/WCMAX/RD/10.,1.);
      LEALL  = max(0.,NNLL+(MINNLL-DENIUL)*TCP-((double)pGS->fProfileDepth-RD)/(double)pGS->fProfileDepth*RN)
		       *min(RWUG/WCMAX/((double)pGS->fProfileDepth-RD)/10.,1.);

      VOLA   = INSW (RAIN-1., 0.15, 0.) * SFERNA;
      SFERNA += RSFNA;
      RSFNA  = FERNA - SFERNA/3.;

      LAYNA  = RRD/((double)pGS->fProfileDepth-RD)*NALL;
      LAYNN  = RRD/((double)pGS->fProfileDepth-RD)*NNLL;
      LAYNA  = (double)0;
      LAYNN  = (double)0;

	 if (pMa->pNFertilizer != NULL)
     {
	  FERNA  = FCNSW((double)(pMa->pNFertilizer->iDay-(int)pTi->pSimTime->fTimeAct),0.,
		             (double)pMa->pNFertilizer->fNH4N/10.,0.);
	  FERNN  = FCNSW((double)(pMa->pNFertilizer->iDay-(int)pTi->pSimTime->fTimeAct),0.,
		             (double)pMa->pNFertilizer->fNO3N/10.,0.);
	  pCh->pCBalance->dNInputCum += (FERNA + FERNN)*(double)10;
      pCh->pCProfile->fNH4NSurf = (float)0;
      pCh->pCProfile->fNO3NSurf = (float)0;
	 }
	  /*
      FERNA  =FCNSW(FNA1T-DFS,0.,FNA1,0.)+FCNSW(FNA2T-DFS,0.,FNA2,0.)...
             +FCNSW(FNA3T-DFS,0.,FNA3,0.)+FCNSW(FNA4T-DFS,0.,FNA4,0.)...
             +FCNSW(FNA5T-DFS,0.,FNA5,0.)+FCNSW(FNA6T-DFS,0.,FNA6,0.)...
             +FCNSW(FNA7T-DFS,0.,FNA7,0.)+FCNSW(FNA8T-DFS,0.,FNA8,0.)
      FERNN  =FCNSW(FNN1T-DFS,0.,FNN1,0.)+FCNSW(FNN2T-DFS,0.,FNN2,0.)...
             +FCNSW(FNN3T-DFS,0.,FNN3,0.)+FCNSW(FNN4T-DFS,0.,FNN4,0.)...
             +FCNSW(FNN5T-DFS,0.,FNN5,0.)+FCNSW(FNN6T-DFS,0.,FNN6,0.)...
             +FCNSW(FNN7T-DFS,0.,FNN7,0.)+FCNSW(FNN8T-DFS,0.,FNN8,0.)
      */

	  //RNAUL  = FERNA+MINAUL       +LAYNA-INSW(NSWI,0.,NUPTA)-NITRUL-VOLA;
	  RNAUL  = FERNA+MINAUL       +LAYNA      -NITRUL-VOLA;
      RNALL  =       MINALL       -LAYNA      -NITRLL;
      //RNNUL  = FERNN+MINNUL+NITRUL+LAYNN-INSW(NSWI,0.,NUPTN)-DENIUL-LEAUL;
      RNNUL  = FERNN+MINNUL+NITRUL+LAYNN      -DENIUL-LEAUL;
      RNNLL  = LEAUL+MINNLL+NITRLL-LAYNN      -DENILL-LEALL;

      //state variables/pools: evaluation
	  TSOIL  += RTSOIL;

	  NAUL   += RNAUL;
      NALL   += RNALL;
      NNUL   += RNNUL;
      NNLL   += RNNLL;

	  //NA     = NAUL + NALL;
      //NN     = NNUL + NNLL;
      //NMINER = NA   + NN;

	  DPM    += RDPM;
      RPM    += RRPM;
      BIO    += RBIO;
      HUM    += RHUM;

	  DPN    += RDPN;
      RPN    += RRPN;

	  //TNLEA  = INTGRL (ZERO, LEALL)


	  /*** output ***/
	  pGS->fSoilTemp  = (float)TSOIL;//pHe->pHBalance->fProfil ???;
      pGS->fSoilTempR = (float)RTSOIL;

	  pGS->fDecPlantMatC = (float)DPM;
	  pGS->fDecPltMCRate = (float)RDPM;
      pGS->fResPlantMatC = (float)RPM;
	  pGS->fResPltMCRate = (float)RRPM;
	  pGS->fMicroBiomC   = (float)BIO;
	  pGS->fHumusC       = (float)HUM;
	  pGS->fMicBiomCRate = (float)RBIO;
	  pGS->fHumusCRate   = (float)RHUM;

	  pGS->fDecPlantMatN = (float)DPN;
      pGS->fDecPltMNRate = (float)RDPN;
	  pGS->fResPlantMatN = (float)RPN;
	  pGS->fResPltMNRate = (float)RRPN;
	  pGS->fCNPlantMat   = (float)CNDRPM;

      pGS->fNH4NContUpperLayer = (float)NAUL;
      pGS->fNH4NContLowerLayer = (float)NALL;
	  pGS->fNO3NContUpperLayer = (float)NNUL;
	  pGS->fNO3NContLowerLayer = (float)NNLL;
	  pGS->fNH4NContUpLayRate  = (float)RNAUL;
      pGS->fNH4NContLwLayRate  = (float)RNALL;
	  pGS->fNO3NContUpLayRate  = (float)RNNUL;
	  pGS->fNO3NContLwLayRate  = (float)RNNLL;

	  pGPltN->fNPlantSupply    = (float)NSUP;

	  /*** output to expertn ***/
	  fpd =(pSo->iLayers-2)*DeltaZ;

	  if (pPl!=NULL) 
		frd = min(fpd-DeltaZ,(float)NOTNUL((double)pPl->pRoot->fDepth*10.));
	  else
		frd = (float)max(2.,(double)fparSD1)*(float)10;

	  f2  = max((float)1,(fpd-frd)/DeltaZ);

	  pCP->fN2EmisR   = (float)0;
	  pCP->fNH3VolatR = (float)(VOLA*10);
	  pCP->fNO3LeachR = (float)(LEALL*10);
	  pCP->dNO3LeachCum += LEALL*10;

      pSLN  = pCh->pCLayer->pNext;
	  if (pPl!=NULL) pPl->pPltNitrogen->fActNH4NUpt = (float)0;
	  if (pPl!=NULL) pPl->pPltNitrogen->fActNO3NUpt = (float)0;
	  for (L=1;L<=pSo->iLayers-2;L++)
      {
       if(L*DeltaZ <= frd) 
	   {
		pSLN->fNH4N         = (float)NAUL*DeltaZ/frd*(float)10;
		pSLN->fNO3N         = (float)NNUL*DeltaZ/frd*(float)10;
        pSLN->fMinerR       = ((float)MINAUL+(float)MINNUL)*DeltaZ/frd*(float)10;
		pSLN->fNH4NitrR     = (float)NITRUL*DeltaZ/frd*(float)10;
        pSLN->fNO3DenitR    = (float)DENIUL*DeltaZ/frd*(float)10;
		pCP->fN2EmisR      += pSLN->fNO3DenitR/(float)24e-5;//Denitrifikation [kg ha-1] ^= N2 Emission [ug h-1 m-2]
        if (pPl!=NULL) pLR->fActLayNH4NUpt = (float)NUPTA*DeltaZ/frd*(float)10;
        if (pPl!=NULL) pLR->fActLayNO3NUpt = (float)NUPTN*DeltaZ/frd*(float)10;
	   }
	   else if ((frd < L*DeltaZ)&&(L*DeltaZ <= frd + DeltaZ))
       {
        f1 = (L*DeltaZ-frd)/DeltaZ;
        pSLN->fNH4N         = (((float)1-f1)*(float)NAUL*DeltaZ/frd+f1*(float)NALL/f2)*(float)10;             
        pSLN->fNO3N         = (((float)1-f1)*(float)NNUL*DeltaZ/frd+f1*(float)NNLL/f2)*(float)10;
        pSLN->fMinerR       = (((float)1-f1)*((float)MINAUL+(float)MINNUL)*DeltaZ/frd
			                   +f1*((float)MINALL+(float)MINNLL)/f2)*(float)10;
        pSLN->fNH4NitrR     = (((float)1-f1)*(float)NITRUL*DeltaZ/frd+f1*(float)NITRLL/f2)*(float)10;             
        pSLN->fNO3DenitR    = (((float)1-f1)*(float)DENIUL*DeltaZ/frd+f1*(float)DENILL/f2)*(float)10;
		pCP->fN2EmisR      += pSLN->fNO3DenitR/(float)24e-5;//Denitrifikation [kg ha-1] ^= N2 Emission [ug h-1 m-2]
        if (pPl!=NULL) pLR->fActLayNH4NUpt = (((float)1-f1)*(float)NUPTA*DeltaZ/frd)*(float)10;
        if (pPl!=NULL) pLR->fActLayNO3NUpt = (((float)1-f1)*(float)NUPTN*DeltaZ/frd)*(float)10;
	   }
	   else
	   {
        pSLN->fNH4N         = (float)NALL/f2*(float)10;             
		pSLN->fNO3N         = (float)NNLL/f2*(float)10;
        pSLN->fMinerR       = (((float)MINALL+(float)MINNLL)/f2)*(float)10;
		pSLN->fNH4NitrR     = ((float)NITRLL/f2)*(float)10;
        pSLN->fNO3DenitR    = ((float)DENILL/f2)*(float)10;
		pCP->fN2EmisR      += pSLN->fNO3DenitR/(float)24e-5;//Denitrifikation [kg ha-1] ^= N2 Emission [ug h-1 m-2]
		if (pPl!=NULL) pLR->fActLayNH4NUpt = (float)0;
        if (pPl!=NULL) pLR->fActLayNO3NUpt = (float)0;
	   }//if,else if,else

	   pSLN->fCO2ProdR    = (float)RESCO2/fpd;         
	   
	   if (pPl!=NULL) pPl->pPltNitrogen->fActNH4NUpt += pLR->fActLayNH4NUpt;
       if (pPl!=NULL) pPl->pPltNitrogen->fActNO3NUpt += pLR->fActLayNO3NUpt;

       pSL =pSL->pNext;
       pSLW=pSLW->pNext;
       pSLN=pSLN->pNext;
       if((pPl!=NULL)&&(pLR->pNext!=NULL)) pLR =pLR->pNext;
	  }//for

      //pGS->fNH4NContUpperLayer -= pPl->pPltNitrogen->fActNH4NUpt/(float)10; 
      //pGS->fNO3NContUpperLayer -= pPl->pPltNitrogen->fActNO3NUpt/(float)10; 
	  //pPltN->fActNH4NUpt = (float)NUPTA*(float)10;
      //pPltN->fActNO3NUpt = (float)NUPTN*(float)10;

      if (pPl!=NULL) pPl->pPltNitrogen->fActNUpt  = pPl->pPltNitrogen->fActNH4NUpt + pPl->pPltNitrogen->fActNO3NUpt;
      if (pPl!=NULL) pPl->pPltNitrogen->fActNUptR = pPl->pPltNitrogen->fActNUpt;//per day, i.e. dt=1 !


  return 0;
}
