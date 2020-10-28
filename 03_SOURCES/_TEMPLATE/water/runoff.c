/*******************************************************************************
 *
 * Copyright  (c) 
 *
 * Author:  Th.Schaaf
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Berechnung des oberflaechigen Abflusses.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: runoff.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 18:41
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/water
 * Using _MALLOC macro to detect memory leaks.
 *
 *   12.08.96
 * 
*******************************************************************************/

#include <crtdbg.h>
#include  "xinclexp.h"
#include  "xh2o_def.h" 
#include  "xtempdef.h"

/***********************************************************************/
/**                         Functionen                                **/
/***********************************************************************/

/* from util_fct.c */
extern int SimStart(PTIME);
extern int NewDay(PTIME);

signed short int WINAPI RunOffSCS(EXP_POINTER);
signed short int WINAPI InitCNSCS(EXP_POINTER);
signed short int WINAPI AktCNSCS(EXP_POINTER);
signed short int WINAPI Hangneigung(EXP_POINTER);

signed short int WINAPI RunOff_Pe(EXP_POINTER);

/*********************************************************************************/
/*  Name     : AktCNSCS                                                          */
/*                                                                               */
/*  Ein      : Zeiger auf Sammelvariable Boden, Wasser                           */
/*                                                                               */
/*  Aus      : Errorcode  OK                                                     */
/*  Funktion : Berechnung der Kurvennummmer aus den Eingabe                      */
/*             werten feucht/trocken/normal (Methode SCS)                        */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum    : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pSo->fCurvNumAct                                                  */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI AktCNSCS(EXP_POINTER)
{
 signed short int  OK=0;
	float  scstr,scsfe,tr,fe;
	float  minEvap,fakA,lufttrocken,w1;
	int i1;

 PSWATER	akt0;
 PCLAYER	akt1;
 PWLAYER	akt2;

	pWa->pWBalance->fReservoir += pWa->fPondWater;
	pWa->fPondWater = (float)0.0;

// if ((pCl->pWeather->fRainAmount + pWa->fPondWater) > (float) 0.0)
 if (pWa->pWBalance->fReservoir > (float) 0.0) 
 {
    scstr = (float)0.0;
    scsfe = (float)0.0;
    tr    = (float)0.0;
    fe    = (float)0.0;

	/* Berechnung Feuchtefaktor fuer Ammonifizierung wird hier benoetigt um die
	   Oberflaechenfeuchte zu ermitteln. die Berechnung ist identsich mit der im
	   Modul Mineralisation. */

		   /* Berechnung minimale Evaporationsverluste.*/
	minEvap = (float)0.9 - ((float)0.00038 * (((pSo->pSLayer->pNext->fThickness)/10) - (float)30.0)
                      				     * (((pSo->pSLayer->pNext->fThickness)/10) - (float)30.0));


    for (akt0 = pSo->pSWater->pNext,
         akt1 = pCh->pCLayer->pNext,
         akt2 = pWa->pWLayer->pNext,
         i1=1;
                                   ((akt0->pNext != NULL)&&
                                    (akt1->pNext != NULL)&&
                                    (akt2->pNext != NULL));
         akt0 = akt0->pNext,
         akt1 = akt1->pNext,
         akt2 = akt2->pNext,
         i1++)
	    {        
	      lufttrocken = (akt0->fContPWP * (float)0.5);
		  if (i1 == 1) 
	      lufttrocken = (akt0->fContPWP * minEvap);
	      
	      fakA =  ((akt2->fContAct - lufttrocken) / (akt0->fContFK - lufttrocken));
		  if (akt2->fContAct > akt0->fContFK)
		  {
			w1 = ((akt2->fContAct - akt0->fContFK) / (akt0->fContSatCER - akt0->fContFK));
			if (w1 < (float)0.0) w1=(float)0;
	        fakA = (float)1.0 - (w1 * (float)0.5);
		  }
		  if (fakA < (float)0.0)
		  {
		    fakA = (float)0.0;
		  }
		  akt1->fNH4NitrCoeff = fakA;
		}

    for (akt0 = pSo->pSWater->pNext,
         akt1 = pCh->pCLayer->pNext,
         akt2 = pWa->pWLayer->pNext;
                                   ((akt0->pNext != NULL)&&
 	                                (akt1->pNext != NULL)&&
                                    (akt2->pNext != NULL));
         akt0 = akt0->pNext,
		 akt1 = akt1->pNext,
         akt2 = akt2->pNext)

	 /*Berechnung der Feuchtebedingungen des gesamten Profiles um die 
	   eingelesene Kurvennumer den aktuellen Verhaeltnissen anzupassen.
	   Hierzu wird ein trockene (tr) und feuchte (fe) Situationen abgeprüft. 
	   Der Faktor fuer Ammonifizierung ist geeignet Aussagen ueber den Feucht-
	   zustand jeder Bodenschicht zu geben.     */
	    {
	      tr = ((akt2->fContAct - akt0->fContPWP)/(akt0->fContFK - akt0->fContPWP));
	      scstr  += (tr *  akt1->fNH4NitrCoeff);
		  if((akt0->fContSatCER - akt0->fContFK)<=(float)0)
		  {
           fe = (float)0;
		  }
		  else
		  {
	       fe = ((akt2->fContAct - akt0->fContFK)/(akt0->fContSatCER - akt0->fContFK));
		  }
	      scsfe += (fe * akt1->fNH4NitrCoeff);
	                                                     
	     }
	
	   /* Berechnung der aktuellen Kurvennummer unter Verwendung der ermittelten 
	     trockenen und feuchten Bedingungen.        */
	    
	    if (scstr >= (float)1.0)
	    	{
	    	  pSo->fCurvNumAct = pSo->fCurvNumNorm + ((pSo->fCurvNumWet - pSo->fCurvNumNorm) * scsfe);
		    }
	    else
	    	{
		      pSo->fCurvNumAct = pSo->fCurvNumDry + ((pSo->fCurvNumNorm - pSo->fCurvNumDry) * scstr);
		    }
	
	 /* bei Kurvennumer > 0 kein sinnvolles Ergebnis  */
	   if (pSo->fCurvNumAct <= (float)0.0)
		   {
		     pSo->fCurvNumAct = (float)0.99;
		   }
	   else
	   {     /* bei Kurvennumer > 100 gibt es negativen Abfluss  */
	     	if (pSo->fCurvNumAct > (float)100.0)
		     {
		        pSo->fCurvNumAct = (float)100.0;
		     }
	   }  

  }  /* Ende Abfrage Niederschlag -if (pCl->weather->fRainAmount>(float)0.0)-  */

 return OK;
 }  /*  Ende der Berechnung der Kurvennummer  */



/*********************************************************************************/
/*  Name     : RunOffSCS                                            */
/*                                                                               */
/*  Funktion : Berechnung OberflaechenAbfluss und                                */
/*             Infiltrationsgroesse in erste Bodenschicht (Methode SCS)          */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum    : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pWa->fPondWater                                                   */
/*             pWa->fRunOffR                                                     */
/*             pWa->fInfiltration                                                */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI RunOffSCS(EXP_POINTER)
{
 signed short int OK=0;
 float            f1,f2;
 float DeltaT       = pTi->pTimeStep->fAct;
 
if (NewDay(pTi))
{
 if (SimStart(pTi))  
 	{
	InitCNSCS(exp_p); 	
 	}

	AktCNSCS(exp_p);
	

	 if ((pWa->pWBalance->fReservoir > (float) 0.0)&&(pSo->fCurvNumAct >= 0.0))
	 {

 		/* Berechnung des Oberflaechenabflusses */
 
    	f1 = (float)254.0 * (((float)100.0 / pSo->fCurvNumAct) - (float)1.0);
		f2 = pWa->pWBalance->fReservoir - ((float)0.2 * f1);


	  	if (f2 > (float)0.0)
		{
			pWa->fRunOffR = (f2*f2)/(pWa->pWBalance->fReservoir + (float)0.8 * f1);			
		}
	
  	} /* Ende if */
 } /* Ende NewDay */
   	
 return OK;                                  
}  /*  Ende Berechnung Oberflaechenbilanz */


/*********************************************************************************/
/*  Name     : InitCNSCS                                                         */
/*                                                                               */
/*  Funktion : Bestimmung der normalen Kurvennummmer aus der Bodenart,           */
/*             der Bodenkundlichen Kartieranleitung und des                      */
/*             Handbuches Mais.                                                  */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum    : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pSo->fCurvNumNorm                                                 */
/*             pSo->fCurvNumDry                                                  */
/*             pSo->fCurvNumWet                                                  */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI InitCNSCS(EXP_POINTER)
{
 signed short int  OK=0;
	float einfluss,gesamteinfluss;

	PSLAYER		akt0;				

	gesamteinfluss = (float)0.0;			

    for (akt0 =pSo->pSLayer->pNext;
                  ((akt0->pNext != NULL));
         akt0 = akt0->pNext)
		{
		if (!lstrcmp(akt0->acSoilID,"Ts\0"))//Ts ep121207
			einfluss = (float)0.0;
		if (!lstrcmp(akt0->acSoilID,"Ts2\0"))
			einfluss = (float)0.0;
		if (!lstrcmp(akt0->acSoilID,"Ts3\0"))
			einfluss = (float)0.0;		  
		if (!lstrcmp(akt0->acSoilID,"Ts4\0"))
		    einfluss = (float)0.0;
		if (!lstrcmp(akt0->acSoilID,"Tu2\0"))
		    einfluss = (float)0.0;
		if (!lstrcmp(akt0->acSoilID,"Tl\0"))
		    einfluss = (float)0.0;
		if (!lstrcmp(akt0->acSoilID,"T\0"))
		    einfluss = (float)0.0;
		if (!lstrcmp(akt0->acSoilID,"S\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Tu\0"))//Tu ep121207
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Tu3\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Sl\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Sl2\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Sl3\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Sl4\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"St\0"))//St ep121207
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"St2\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"St3\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Uls\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Ls3\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Ls\0"))//Ls ep121207
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Ls4\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Lu\0")) 
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Lt3\0"))		
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Lt\0"))//Lt ep121207		
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Lts\0"))
			einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Slu\0"))
		    einfluss = (float)0.25;
		if (!lstrcmp(akt0->acSoilID,"Su\0"))//Su ep121207
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Su2\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Su3\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Su4\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Ul4\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Ut4\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Ls2\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Lt2\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Ltu\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Tu4\0"))
		    einfluss = (float)0.5;
		if (!lstrcmp(akt0->acSoilID,"Ul\0"))//Ul ep121207
			einfluss = (float)0.75;
		if (!lstrcmp(akt0->acSoilID,"Ul3\0"))
			einfluss = (float)0.75;
		if (!lstrcmp(akt0->acSoilID,"Ut\0"))//Ut ep121207
			einfluss = (float)0.75;
		if (!lstrcmp(akt0->acSoilID,"Ut3\0"))
			einfluss = (float)0.75;
		if (!lstrcmp(akt0->acSoilID,"U\0"))
			einfluss = (float)1.0;
		if (!lstrcmp(akt0->acSoilID,"Us\0"))
			einfluss = (float)1.0;
		if (!lstrcmp(akt0->acSoilID,"Ul2\0"))
			einfluss = (float)1.0;
		if (!lstrcmp(akt0->acSoilID,"Ut2\0"))
			einfluss = (float)1.0;


		gesamteinfluss += einfluss;
		
		}  /*  Ende Bestimmung des Einflusses der Bodenart */


		gesamteinfluss = (gesamteinfluss / (pSo->iLayers - 2));



  	/*  Einordnen in hydraulische Gruppen nach Handbuch Mais */

  	/*  Hydraulische Bodengruppe A */
	if (gesamteinfluss <= (float)0.25)
	{
	 pSo->fCurvNumNorm = (float)72.0;
	}							 
  	/*  Hydraulische Bodengruppe B */
	if ((gesamteinfluss > (float)0.25)&&
					(gesamteinfluss <= (float)0.5))
	{
	 pSo->fCurvNumNorm = (float)81.0;
	}							
  	/*  Hydraulische Bodengruppe C */
	if ((gesamteinfluss > (float)0.5)&&
					(gesamteinfluss <= (float)0.75))
	{
	 pSo->fCurvNumNorm = (float)88.0;
	}							
  	/*  Hydraulische Bodengruppe D */
	if (gesamteinfluss > (float)0.75)
	{
	 pSo->fCurvNumNorm = (float)91.0;
	}							
							
 /* Berechnung Kurvennummer unter feuchten Bodenverhaeltnissen */

	pSo->fCurvNumWet = pSo->fCurvNumNorm * ((float)2.4175 - ((float)0.0267 * pSo->fCurvNumNorm)
							+ ((float)0.00014 * (pSo->fCurvNumNorm * pSo->fCurvNumNorm)));
	if (pSo->fCurvNumWet > (float)100.0)
	{
		pSo->fCurvNumWet = (float)100.0;
	}	

 /* Berechnung Kurvennummer unter trockenen Bodenverhaeltnissen */

	pSo->fCurvNumDry = pSo->fCurvNumNorm * ((float)0.3067 + ((float)0.00168 * pSo->fCurvNumNorm));
	if (pSo->fCurvNumDry > (float)96.0)
	{
		pSo->fCurvNumDry = pSo->fCurvNumNorm * ((float)0.02 *pSo->fCurvNumNorm -(float)1.0);
	}	
	if (pSo->fCurvNumDry > (float)100.0)
	{
		pSo->fCurvNumDry = (float)100.0;
	}	
							
 return OK;
}  /*  Ende Berechnung der drei Kurvennummer */



/*********************************************************************************/
/*  Name     : Hangneigung                                            */
/*                                                                               */
/*  Funktion : Berechnung OberflaechenAbfluss und                                */
/*             Infiltrationsgroesse in erste Bodenschicht (Methode SCS)          */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum    : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pWa->fPondWater                                                   */
/*             pWa->fRunOffR                                                     */
/*             pWa->fInfiltration                                                */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI Hangneigung(EXP_POINTER)
{
 signed short int  OK=0;    
 float fPondDay;

 /* Bei Hangneigung muss der Oberflaechenabfluss als
 	Verlust angesehen werden. Handelt es sich um eine 
 	ebene Flaeche entsteht das Pondwasser, das am naechsten
 	Tag wieder infiltrieren kann. In welcher Hoehe Abfluss
 	auftritt haengt von der Hangneigung ab. */

if (NewDay(pTi))
{
  if  ((pLo->pFieldplot->fSlope >= (float)0.0)&& 
         (pLo->pFieldplot->fSlope < (float)1.0))
               fPondDay = pWa->fRunOffR;

  else if  ((pLo->pFieldplot->fSlope >= (float)1.0)&& 
         (pLo->pFieldplot->fSlope < (float)2.0))
           fPondDay = pWa->fRunOffR  * (float)0.9; 
           
  else if  ((pLo->pFieldplot->fSlope >= (float)2.0)&& 
         (pLo->pFieldplot->fSlope < (float)3.5))
          fPondDay= pWa->fRunOffR  * (float)0.75;
           
  else if  ((pLo->pFieldplot->fSlope >= (float)3.5)&& 
         (pLo->pFieldplot->fSlope < (float)5.0))
           fPondDay = pWa->fRunOffR  * (float)0.6;
           
  else if  ((pLo->pFieldplot->fSlope >= (float)5.0)&& 
         (pLo->pFieldplot->fSlope < (float)9.0))
           fPondDay = pWa->fRunOffR  * (float)0.5;
           
  else if  ((pLo->pFieldplot->fSlope >= (float)9.0)&& 
         (pLo->pFieldplot->fSlope < (float)12.0))
           fPondDay = pWa->fRunOffR  * (float)0.25;
           
  else if  ((pLo->pFieldplot->fSlope >= (float)12.0)&& 
         (pLo->pFieldplot->fSlope < (float)18.0))
           fPondDay = pWa->fRunOffR  * (float)0.1;
           
  else if  ((pLo->pFieldplot->fSlope >= (float)18.0)&& 
         (pLo->pFieldplot->fSlope < (float)27.0))
           fPondDay = pWa->fRunOffR  * (float)0.05;
           
  else if  ((pLo->pFieldplot->fSlope >= (float)27.0)&& 
         (pLo->pFieldplot->fSlope < (float)100.0))
           fPondDay = pWa->fRunOffR  * (float)0.0;
  else
           fPondDay = pWa->fRunOffR  * (float)1000.0;

  pWa->fRunOffR -= fPondDay;
  
  //pWa->fPondWater = fPondDay;   // cs 06.03.00 bleibt sonst nur innerhalb der 
                                  // subroutine, muß mit raus genommen werden!

  //pWa->fPondWater += fPondDay; //ep 260799 warum doppelt beruecksichtigt???
                                 //da vorher nirgends schon per pWa->fRunOffR
                                 //abgezogen!  

	}  /*Ende NewDay*/

 return OK;
 } /* Ende Hangneigung */




/*********************************************************************************/
/*  Name     : RunOff_Pe                                                         */
/*                                                                               */
/*  Funktion : Berechnung OberflaechenAbfluss und                                */
/*             Infiltration - Zweistufenmodell (nach Dyck/Peschke, 1995)         */
/*             (analog der von Schullla genutzten Version!                       */
/*  Autor    : C. Sambale 01.03.00                                               */
/*  Datum    : 02.03.00                                                          */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*             pWa->fPondWater                                                   */
/*             pWa->fRunOffR                                                     */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI RunOff_Pe(EXP_POINTER)
{
  DECLARE_TEMP_POINTER
  DECLARE_COMMON_VAR

// signed short int OK=0;
 int   iLayerAnz     = pSo->iLayers;
 float DeltaZ        = pSo->fDeltaZ;
// float DeltaT        = pTi->pTimeStep->fAct;
 float fP            = pWa->pWBalance->fReservoir;

 float        *pCondSat,*pFill,*fMatPotFF,*fPoros;
 int          iSat,ihelp,iSatLayer;
 float        fTSat,sum1,sum2,lS,rS,ahelp,bhelp,fInfiltration;
 
  PWBALANCE   pWB;
  pWB = pWa->pWBalance;

  TEMP_ZERO_LAYER
  
  H2O_ZERO_LAYER

pFill        = (float *) _MALLOC(iLayerAnz * sizeof(float)); //Sättigung minus akt. Wassergehalt
 memset(pFill,0x0,(iLayerAnz * sizeof(float)));
pCondSat        = (float *) _MALLOC(iLayerAnz * sizeof(float)); //Sättigung minus akt. Wassergehalt
 memset(pCondSat,0x0,(iLayerAnz * sizeof(float)));
fMatPotFF        = (float *) _MALLOC(iLayerAnz * sizeof(float)); //Zeitdauer bis zur Sättigung
 memset(fMatPotFF,0x0,(iLayerAnz * sizeof(float)));
fPoros        = (float *) _MALLOC(iLayerAnz * sizeof(float)); //Zeitdauer bis zur Sättigung
 memset(fPoros,0x0,(iLayerAnz * sizeof(float)));


if (NewDay(pTi)) //Berechnung in Tagesschrittweite, da von konst. 
                 //P-Intensität ausgengangen werden muß, ist zwar sehr ungenau,
{                //aber nicht zu ändern

if(fP>0)   //nur bei Niederschlag durchlaufen
{
 
  iSat = 0;
  iSatLayer = 0;
  for(H2O_SOIL_LAYERS)         // 
  {
   	 pFill[iLayer] = pSW->fContSat- pWL->fIce - pWL->fContAct;    // aufüllbares Porenvolumen, bei Bodenfrost reduziert
     fMatPotFF[iLayer] = pFill[iLayer] * (float)1000;          // Matrixpotential an der FeuchteFront (Schätzung, nach Schulla, 1997) 
     fPoros[iLayer] = pSL->fPorosity - pWL->fIce;

	if(pWL->fIce>(float)0)   // maximale Leitfähigkeit bei Bodenfrost reduziert!!
	 {
      ahelp            = MATRIX_POTENTIAL(pSW->fContSat-pWL->fIce); 
	  pCondSat[iLayer] = CONDUCTIVITY(ahelp);
	 }
	else
	 {        
	  pCondSat[iLayer] = pSW->fCondSat; 
	 }
  }


//Suche nach der Schicht in der die Sättigung das erste mal auftritt (Formel 20.25 Dyck/Peschke 'Grundlagen der Hydrologie, 3.auflage)    
   for(H2O_SOIL_LAYERS) if(iSat==0)         // 
  {
   sum1 = (float)0;
   sum2 = (float)0;
   ihelp=1;
   for(ihelp=1;ihelp<iLayer;ihelp++)
   {
	 sum1 = sum1 + DeltaZ * (pFill[iLayer] - pFill[ihelp]);
	 sum2 = sum2 + DeltaZ * ((float)1 / pCondSat[iLayer] - (float)1 / pCondSat[ihelp]);
   }
     lS = (fP / pCondSat[iLayer] -1)*(fP + sum1);
	 rS = pFill[iLayer] * (fMatPotFF[iLayer] + fP * sum2);
   if(lS>rS && iSat==0)
   {
	   iSat=1;
       iSatLayer = iLayer;
   }
  }
 
   if(iSat==1)
  {
    // Zeit bis zur Sättigung (Formel 20.26, Dyck/ Peschke)
   sum1 = (float)0;
   sum2 = (float)0;
   ihelp=1;
   for(ihelp=1;ihelp<iSatLayer;ihelp++)
    {
	 sum1 = sum1 + DeltaZ * (pFill[ihelp] - pFill[iSatLayer]) / fP;
	 sum2 = sum2 + fP * DeltaZ * ((float)1 / pCondSat[iSatLayer] - (float)1 / pCondSat[ihelp]);
    }
 
   fTSat = sum1 + pFill[iSatLayer]*(fMatPotFF[iSatLayer] + sum2) / ( fP * (fP/pCondSat[iSatLayer] -1));
   fTSat = min((float)1,fTSat);
 
   // Wassermenge die während der Sättigungsphase infiltiriert (Formel 20.31, Dyck/ Peschke)
   fInfiltration = fTSat * fP;   

    ahelp = (1-fTSat) * pCondSat[iSatLayer];
	bhelp = fInfiltration + (float)2 * pFill[iSatLayer] * fMatPotFF[iSatLayer];
	
   //Gesamtmenge des infiltrierenden Wassers (Formel 20.20)
   
   fInfiltration = ahelp / (float)2
	             + (float)sqrt(ahelp * ahelp / (float)4 + ahelp * bhelp + fInfiltration * fInfiltration);
   
   if(fInfiltration>fP) fInfiltration = fP; // Überprüfen ob Infiltration < Niederschlag (sozusagen als Sicherheit,wenn was schief gegangen sein sollte)
   

   pWa->fRunOffR = fP - fInfiltration;
   }
 }
 Hangneigung(exp_p);

 } // Ende NewDay 
   	
 return OK;                                  
}  //  Ende Berechnung Oberflaechenbilanz 

/*******************************************************************************
** EOF */