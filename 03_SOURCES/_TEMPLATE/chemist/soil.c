/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                                          %
% module      :   soil.C                                                                  %
% description :                                                                            %
%                                                                                          %
%                                                                                          %
%                                                                                          %
% Date        :   30.09.97                                                                 %
%                                                                                          %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/* Sammlung aller Header-Dateien */
#include "xinclexp.h"     
#include "xn_def.h"     
#include "xh2o_def.h"     


/*********************************/
/*  external procedures          */
/*********************************/

extern float abspower(float, float);
extern int   NewDay(PTIME);
extern int   SimStart(PTIME);


/*********************************************************************************/
/**                 Interne Functionen                                          **/
/*********************************************************************************/
signed short int WINAPI NaturRueck(EXP_POINTER);
signed short int WINAPI Bioturbation(EXP_POINTER);              
signed short int aktLagerung(EXP_POINTER);
signed short int aktHydraulBodPara(EXP_POINTER);
signed short int aktMinRate(EXP_POINTER);

static	float   aforigPWP[MAXSCHICHT];
static	float   aforigFK[MAXSCHICHT];
static	float   aforigSatCER[MAXSCHICHT];		

/*********************************************************************************/
/*  Name     : NaturRueck                                                        */
/*                                                                               */
/*  Funktion : Durch den Niederschlag findet uebeer das ganze Jahr eine Sackung  */
/*             des Bodens statt. Diese wird mit dieser Funktion beruecksichtigt. */
/*             Die Sackungsberechnung erfolgt nach dem Modell EPIC. Die Aktual-  */
/*             isierung der Lagerungsdichte hat auf die hydraulischen Bodenpara- */
/*             meter Einfluss.                                                   */
/*                                                                               */                                                                                   
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI NaturRueck(EXP_POINTER)
{
 signed short int OK=0;

/* Derzeit die Veraenderung der Mineralisationsbedingungen nur 
	aufgrund der Veraenderung der Lagerungsdichte integriert. */

    if (NewDay(pTi))
    {
		OK += aktLagerung(exp_p); 
		OK += aktHydraulBodPara(exp_p); 
	 	OK += aktMinRate(exp_p); 
	}

  return OK;
}      /*===== Ende der Natuerlichen Rueckverfestigung =============*/



/*********************************************************************************/
/*  Name     : aktLagerung                                                       */
/*                                                                               */
/*  Funktion : Interne Methode zur Berechnung der taeglichen Rueckverfestigung   */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int aktLagerung(EXP_POINTER)
{
 signed short int OK=0;

 	PSLAYER pSL;  
 	PWLAYER	pWL;

    float   sakfak,wasser,lagletzteSchicht,diff;
	float	lagdichteEnt[MAXSCHICHT];
    int     i1,i2;

   		/* Lokales Array wird initialisiert */
	    for (pSL = pSo->pSLayer->pNext;
	                 (pSL->pNext->pNext != NULL);
	         pSL = pSL->pNext)
		{                         
			lagletzteSchicht = pSL->fBulkDens;                   
		}/* Ende lokale Initialisierung */
		
		
	 	/* Berechnung der entgueltigen Lagerungsdichte derzeit lokal, 
	 		da kein globale Variable vorhanden. */
	    for (pSL = pSo->pSLayer->pNext,i2=0;
	                 (pSL->pNext->pNext != NULL);
	         pSL = pSL->pNext,i2++)
		{
			diff = ((lagletzteSchicht - pSL->fBulkDensStart) / (float ) 10.0);
			if (diff < (float)0.0) 
			{ diff = (float)0.0;}
			lagdichteEnt[i2] = diff;
		}/* Ende Initialisierung entgueltigen Lagerungsdichte*/
		
	    for (pSL = pSo->pSLayer->pNext,
	    	 pWL = pWa->pWLayer->pNext,i1=0;
	                 ((pSL->pNext->pNext != NULL)&&
	                  (pWL->pNext->pNext != NULL));
	         pSL = pSL->pNext,
	         pWL = pWL->pNext,i1++)
		{
		 wasser = pWL->fFlux;
	
		 if (wasser > (float)0.0)
		 {
		/* ===========Sackungsberechung ===================*/
		
           sakfak = (((wasser / abspower(((pSL->fThickness / (float)10.0)),(float)0.6))
           				* ((float)1.0 + (((float)2.0 * pSL->fSand) 
           				/ (pSL->fSand + (((float)exp((float)8.597 - ((float)0.075 * pSL->fSand)))))))));

                                                        
			pSL->fBulkDens = pSL->fBulkDens + (((pSL->fBulkDensStart + lagdichteEnt[i1]) - pSL->fBulkDens)
							 * (sakfak / (sakfak + (float)exp(((float)3.375 - (i1 * (float)0.1) - ((float)0.008835 * ((pSL->fThickness / (float)10.0))))))));

		/* Aufgrund der Zunahme der Lagerungsdichte ergeben sich fuer die
		   Mineralisation der Humusfraktion schlechtere Verhaeltnisse. Daher 
		   wird die Rate hier wieder neu berechnet. */

		 }  /* Ende Wasserfluss vorhanden > 0 */

		} /*===== Ende der schichtweisen Berechnung =============*/
							
  return OK;
}      /*===== Ende der Berechnung der neuen Lagerungsdichte =============*/

/*********************************************************************************/
/*  Name     : aktHydraulBodPara                                                 */
/*                                                                               */
/*  Funktion : Interne Methode zur taeglichen Aktualisierung der hydraulichen    */
/*             Bodenparameter. Aufgrund der sich taeglichen Veraenderung der     */
/*             Lagerungsdichte und des Gehaltes an organischer Substanz muessen  */
/*             die hydraulischen Bodenparameter aktualisiert werden.             */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int aktHydraulBodPara(EXP_POINTER)
{
 signed short int OK=0;

	int		i1;
	float   afkorrPWP[MAXSCHICHT];
	float   afkorrFK[MAXSCHICHT];
	float   afkorrSAT[MAXSCHICHT];		
	float	fBulkLd3 = (float)1.575;
	float	korec;	

 	PSLAYER pSL;  
 	PSWATER	pSW;

      if (SimStart(pTi)) 
      {
	    for (pSW = pSo->pSWater->pNext,i1=0;
	                 (pSW->pNext->pNext != NULL);
	         pSW = pSW->pNext,i1++)
		{
			aforigPWP[i1] = pSW->fContPWP;
			aforigFK[i1]  =	pSW->fContFK;
			aforigSatCER[i1] = pSW->fContSatCER;
		}
    
      }
    
	    for (pSL = pSo->pSLayer->pNext,i1=0;

	                 (pSL->pNext->pNext != NULL);
	         pSL = pSL->pNext,i1++)
		{
			if (!lstrcmp(pSL->acSoilID,"S\0"))
			{
			afkorrPWP[i1] = (float)0.0133;
			afkorrFK[i1]  =	(float)0.0267;
			afkorrSAT[i1] = (float)0.1333;
			}
			if ((!lstrcmp(pSL->acSoilID,"Su\0"))|| //Su ep121207
				(!lstrcmp(pSL->acSoilID,"Su2\0"))||
				(!lstrcmp(pSL->acSoilID,"Su3\0"))||
				(!lstrcmp(pSL->acSoilID,"Su4\0")))
			{
			afkorrPWP[i1] = (float)0.004;
			afkorrFK[i1]  =	(float)0.0933;
			afkorrSAT[i1] = (float)0.2267;
			}
			if (!lstrcmp(pSL->acSoilID,"Sl\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.06;
			afkorrSAT[i1] = (float)0.2;
			}
			if (!lstrcmp(pSL->acSoilID,"Sl2\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.24;
			}
			if (!lstrcmp(pSL->acSoilID,"Slu\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.0933;
			afkorrSAT[i1] = (float)0.1867;
			}
			if (!lstrcmp(pSL->acSoilID,"Sl3\0"))
			{
			afkorrPWP[i1] = (float)-0.0267;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.2133;
			}
			if (!lstrcmp(pSL->acSoilID,"Sl4\0"))
			{
			afkorrPWP[i1] = (float)-0.0267;
			afkorrFK[i1]  =	(float)0.093;
			afkorrSAT[i1] = (float)0.2267;
			}
			if (!lstrcmp(pSL->acSoilID,"St\0"))
			{
			afkorrPWP[i1] = (float)0.055;
			afkorrFK[i1]  =	(float)0.10;
			afkorrSAT[i1] = (float)0.18;
			}
			if (!lstrcmp(pSL->acSoilID,"St2\0"))
			{
			afkorrPWP[i1] = (float)0.0533;
			afkorrFK[i1]  =	(float)0.12;
			afkorrSAT[i1] = (float)0.2;
			}
			if (!lstrcmp(pSL->acSoilID,"St3\0"))
			{
			afkorrPWP[i1] = (float)0.04;
			afkorrFK[i1]  =	(float)0.133;
			afkorrSAT[i1] = (float)0.24;
			}
			if (!lstrcmp(pSL->acSoilID,"U\0"))
			{
			afkorrPWP[i1] = (float)0.0133;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.1867;
			}
			if (!lstrcmp(pSL->acSoilID,"Us\0"))
			{
			afkorrPWP[i1] = (float)-.0133;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.1867;
			}
			if (!lstrcmp(pSL->acSoilID,"Ul2\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.08;
			}
			if (!lstrcmp(pSL->acSoilID,"Uls\0"))
			{
			afkorrPWP[i1] = (float)0.0133;
			afkorrFK[i1]  =	(float)0.093;
			afkorrSAT[i1] = (float)0.2;
			}
			if (!lstrcmp(pSL->acSoilID,"Ul\0"))//Ul ep121207
			{
			afkorrPWP[i1] = (float)-0.0133;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.1867;
			}
			if (!lstrcmp(pSL->acSoilID,"Ul3\0"))
			{
			afkorrPWP[i1] = (float)-0.0133;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.1867;
			}
			if (!lstrcmp(pSL->acSoilID,"Ul4\0"))
			{
			afkorrPWP[i1] = (float)-0.0267;
			afkorrFK[i1]  =	(float)0.067;
			afkorrSAT[i1] = (float)0.1733;
			}

			if (!lstrcmp(pSL->acSoilID,"Ut2\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.08;
			}
			if ((!lstrcmp(pSL->acSoilID,"Ut\0"))|| //Ut ep121207
			   (!lstrcmp(pSL->acSoilID,"Ut3\0")))
			{
			afkorrPWP[i1] = (float)-0.0133;
			afkorrFK[i1]  =	(float)0.08;
			afkorrSAT[i1] = (float)0.1867;
			}

			if (!lstrcmp(pSL->acSoilID,"Ut4\0"))
			{
			afkorrPWP[i1] = (float)-0.0267;
			afkorrFK[i1]  =	(float)0.067;
			afkorrSAT[i1] = (float)0.1733;
			}
			if (!lstrcmp(pSL->acSoilID,"Ls2\0"))
			{
			afkorrPWP[i1] = (float)-0.0267;
			afkorrFK[i1]  =	(float)0.093;
			afkorrSAT[i1] = (float)0.2133;
			}

			if ((!lstrcmp(pSL->acSoilID,"Ls3\0"))||
				(!lstrcmp(pSL->acSoilID,"Ls4\0"))||
				(!lstrcmp(pSL->acSoilID,"Ls\0"))) //Ls ep121207
			{
			afkorrPWP[i1] = (float)-0.0133;
			afkorrFK[i1]  =	(float)0.093;
			afkorrSAT[i1] = (float)0.2;
			}
			if (!lstrcmp(pSL->acSoilID,"Lu\0"))
			{
			afkorrPWP[i1] = (float)-0.0133;
			afkorrFK[i1]  =	(float)0.093;
			afkorrSAT[i1] = (float)0.01867;
			}
			if ((!lstrcmp(pSL->acSoilID,"Lt2\0"))||
				(!lstrcmp(pSL->acSoilID,"Lt3\0"))||
				(!lstrcmp(pSL->acSoilID,"Lt\0")))//Lt ep 121207
			{
			afkorrPWP[i1] = (float)0.04;
			afkorrFK[i1]  =	(float)0.133;
			afkorrSAT[i1] = (float)0.2133;
			}
			if (!lstrcmp(pSL->acSoilID,"Ltu\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.12;
			afkorrSAT[i1] = (float)0.2;
			}
			if (!lstrcmp(pSL->acSoilID,"Lts\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.133;
			afkorrSAT[i1] = (float)0.2267;
			}
			if ((!lstrcmp(pSL->acSoilID,"Ts\0"))|| //Ts ep121207
				(!lstrcmp(pSL->acSoilID,"Ts2\0"))||
				(!lstrcmp(pSL->acSoilID,"Ts3\0"))||
				(!lstrcmp(pSL->acSoilID,"Ts4\0")))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.133;
			afkorrSAT[i1] = (float)0.2267;
			}
			if ((!lstrcmp(pSL->acSoilID,"Tu\0"))|| //Tu ep121207   
				(!lstrcmp(pSL->acSoilID,"Tu2\0"))||			
				(!lstrcmp(pSL->acSoilID,"Tu3\0")))			
			{
			afkorrPWP[i1] = (float)0.04;
			afkorrFK[i1]  =	(float)0.133;
			afkorrSAT[i1] = (float)0.2133;
			}
			if (!lstrcmp(pSL->acSoilID,"Tu4\0"))
			{
			afkorrPWP[i1] = (float)0.0;
			afkorrFK[i1]  =	(float)0.12;
			afkorrSAT[i1] = (float)0.2;
			}
			if (!lstrcmp(pSL->acSoilID,"Tl\0"))
			{
			afkorrPWP[i1] = (float)-0.12;
			afkorrFK[i1]  =	(float)0.133;
			afkorrSAT[i1] = (float)0.2133;
			}
			if (!lstrcmp(pSL->acSoilID,"T\0"))
			{
			afkorrPWP[i1] = (float)0.0133;
			afkorrFK[i1]  =	(float)0.133;
			afkorrSAT[i1] = (float)0.2133;
			}
		}

	    for (pSW = pSo->pSWater->pNext,
	    	 pSL = pSo->pSLayer->pNext,i1=0;
	                 ((pSW->pNext->pNext != NULL)&&
	                  (pSL->pNext->pNext != NULL));
	          pSW = pSW->pNext,
	          pSL = pSL->pNext,i1++)
		{
			korec = (fBulkLd3 - pSL->fBulkDens) * afkorrPWP[i1];
			pSW->fContPWP = aforigPWP[i1] + korec;
			
			korec = (fBulkLd3 - pSL->fBulkDens) * afkorrFK[i1];
			pSW->fContFK = aforigFK[i1] + korec;

			korec = (fBulkLd3 - pSL->fBulkDens) * afkorrSAT[i1];
			pSW->fContSatCER = aforigSatCER[i1] + korec;
            //pSW->fContSatCER = max(pSW->fContFK,pSW->fContSatCER)+(float)0.01;
		}

  return OK;
}      /*===== Ende der Berechnung  =============*/

/*********************************************************************************/
/*  Name     : aktMinRate                                                        */
/*                                                                               */
/*  Funktion : Interne Methode zur taeglichen Aktualisierung der Mineralisations-*/
/*             rate der Humusfraktion.                                           */
/*                                                                               */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int aktMinRate(EXP_POINTER)
{
 signed short int OK=0;

	float wert,wasser;

 	PSLAYER pSL;  
 	PCLAYER	pCL;
 	PSWATER pSW;
	PWLAYER pWL;

    for (pSL = pSo->pSLayer->pNext,
         pSW = pSo->pSWater->pNext,
         pCL = pCh->pCLayer->pNext,
         pWL = pWa->pWLayer->pNext;
                                   ((pSL->pNext != NULL)&&
                                    (pSW->pNext != NULL)&&
                                    (pCL->pNext != NULL)&&
                                    (pWL->pNext != NULL));
         pSL = pSL->pNext,
         pSW = pSW->pNext,
         pCL = pCL->pNext,
         pWL = pWL->pNext)
    { 
		 wasser = pWL->fFlux;
	
		 if (wasser > (float)0.0)
		 {
		  wert = (pSW->fContSatCER - pSW->fContFK) * (float)10.0;
		  if (wert<(float)0) wert=(float)0;
	      pCL->fHumusMinerMaxR = (abspower((float)10.0, wert) * (float)0.000013);
	     }
	} /* Ende der schichtweisen Berechnung der neuen Mineralisationsrate fuer Humus */

  return OK;                                                       
}      /*===== Ende der Berechnung  =============*/


/*********************************************************************************/
/*  Name     : Bioturbation                                                      */
/*                                                                               */
/*  Funktion : Interne Methode zur Berechnung der Einmischung oberirdischer      */
/*             Ernterueckstaende durch Bodenlebewesen. Die eingemischten Stoffe  */
/*             werden den Pools der ersten Bodenschicht zugeordnet. Methode      */
/*             Schaaf.                                                           */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pCh->pCLayer->fCLitter                                            */
/*             pCh->pCLayer->fNLitter                                            */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI Bioturbation(EXP_POINTER)
{
 signed short int  OK=0;
 
	float	biofak = (float)0.0;
	float	cFrei = (float)0.0;
	float	nFrei = (float)0.0;

	/*  Funktionsaufruf einmal täglich */
	if (NewDay(pTi))
	{
  	/*  Ermittlung des Einmischfaktore in Abhaengigkeit vom Niederschlag und 
  	    der aktuellen Bodenfeucht der ersten Bodenschicht */
	if (pCl->pWeather->fRainAmount > (float)0.0)
		biofak = (float)0.01;
	else
		biofak = (float)0.001;

/*	
 Versuch einer Beruecksichtigung der aktuellen Feuchte:

	if ((pWa->pWLayer->pNext->fContAct > pSo->pSWater->pNext->fContFK) ||
		(pWa->pWLayer->pNext->fContAct < pSo->pSWater->pNext->fContPWP))
	{biofak = (float)0.0001;}
*/

  	/*  Einmischung des C-Pools der Oberflaeche (nullte Schicht) in erste Schicht */

		if (pCh->pCProfile->fCLitterSurf > (float)0.0)
		{
		  cFrei = pCh->pCProfile->fCLitterSurf * biofak;
		  pCh->pCProfile->fCLitterSurf -= cFrei;
		  pCh->pCLayer->pNext->fCLitter += cFrei;

		} /* Ende C-Pool */

  	/*  Einmischung des N-Pools der Oberflaeche (nullte Schicht) in erste Schicht */
		if (pCh->pCProfile->fNLitterSurf > (float)0.0)
		{
		  nFrei = pCh->pCProfile->fNLitterSurf * biofak;
		  pCh->pCProfile->fNLitterSurf -= nFrei;
		  pCh->pCLayer->pNext->fNLitter += nFrei;	
		}/* Ende N-Pool */               
	} /* Ende NewDay */
	
  return OK;
}  /*  Ende Berechnung Bioturbation */


