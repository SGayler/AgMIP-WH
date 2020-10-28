/*******************************************************************************
 *
 * Copyright  (c) 
 *
 * Author:  TS (TUM)
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Initialisierung der aequidistanten Schichten aus den eingelesenen
 *   Materialschichten.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 3 $
 *
 * $History: initsila.c $
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 20:56
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Using _MALLOC macro to detect memory leaks.
 * 
 * Date    : 20.02.97
 *
*******************************************************************************/


#include <crtdbg.h>
#include "xinclexp.h"
#include "export.h"
#include "xlanguag.h"     

#define MAX_MAT_LAYERS   15  //10
#define MAX_START_LAYERS 15  //10


int paiStartLayerGLOBAL[20];   //by fitting.c
/***********************************************************************/
/**                         Functionen                                **/
/***********************************************************************/

/* from util_fct.c */
extern int WINAPI Message(long, LPSTR);
extern int WINAPI Test_Range(float x, float x1, float x2, LPSTR c);

signed short int WINAPI distributeSimLayer(EXP_POINTER);


/*********************************************************************************/
/*  Name     : distributeSimLayer                                                */
/*                                                                               */
/*  Funktion : Verteilen der Werte fuer die Materialschichten auf die            */
/*             Simulationsschichten                                              */
/*             Dies schließt Verteilung der Startwerte mit ein.                  */
/*  Autor    : Th. Schaaf, C. Haberbosch                                         */
/*  Datum	 : 21.02.97, 23.7.97                                                 */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalen Variablen                 */
/*                                                                               */
/*                                                                               */
/*                                                                               */
/*********************************************************************************/
signed short int WINAPI distributeSimLayer(EXP_POINTER)
{
 DECLARE_COMMON_VAR

    char  acdummy1[80];
    LPSTR lpOut1= acdummy1; 

	float	fProfil,fStartProfil;
	float	f1,part1,part2;
	int		iSimLayerNum,i1,i2;
  	float 	*pMat1Local,*pMat2Local,*pMat3Local,*pMat4Local,*pMat5Local,
  			*pMat6Local,*pMat7Local,*pMat8Local,*pMat9Local,*pMat10Local,
  			*pMat11Local,*pMat12Local,*pMat14Local;
  	
  	typedef  char shortstring[5];
  	
  	shortstring *pMat13Local;
  	
  	int     aiStartLayer[MAX_START_LAYERS];		
	
/****************** Allgemeiner Teil Beginn ******************/
  
    PREALLAYER pRL;          

	PCLAYER			pCL = pCh->pCLayer;                
	PSLAYER			pSL = pSo->pSLayer;
	PSWATER			pWA = pSo->pSWater;
	PWLAYER			pWL = pWa->pWLayer;
	PHLAYER			pHL = pHe->pHLayer;
	PSTARTVALUE		pMS = &pMa->Startvalue;

	/* Einlesen der Maechtigkeit der Materialschichten wie sie 
	   in der Datenbank gespeichert sind und die Anzahl der 
	   Simulationsschichten fuer jede Materialschicht  */
    pRL = getpstRealLayer();   
    
    /* Überprüfe, ob Anzahl der eingelesenen Schichten korrekt ist */
    
    /* iLayerDim = 0 wenn von Sim-File eingelesen wird. */
    if (!pRL->iLayerDim)
    {
    return 1;
    }
    
    if (!Test_Range((float)pRL->iLayerDim,(float)1,(float)MAX_MAT_LAYERS,"iLayerDim"))
    {
    //char   acdummy[80];
    strcpy(acdummy,""); 
	strcpy(lpOut,acdummy);
	strcat(lpOut,"Horizon Number: "); 
    strcat(lpOut,INPUT_DATA_ERROR_TXT); 
    Message(3, lpOut);
    return -1;
    }

    if (!Test_Range((float)pRL->iStartLayerDim,(float)1,(float)MAX_START_LAYERS,"iStartLayerDim"))
    {
    //char   acdummy1[80];
	strcpy(acdummy1,"");
	strcpy(lpOut1,acdummy1);
	strcat(lpOut1,"Start Value Layers: "); 
    strcat(lpOut1,INPUT_DATA_ERROR_TXT); 
    Message(3, lpOut1);
    return -1;
    }
    
    
    i1 = max(pRL->iLayerDim, pRL->iStartLayerDim);
    
	/* Allokieren von Speicher um die Werte
	 	der Materialschichten zu speichern.*/
	pMat1Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat1Local,0x0,(i1 * sizeof(float)));
	pMat2Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat2Local,0x0,(i1 * sizeof(float)));
	pMat3Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat3Local,0x0,(i1 * sizeof(float)));
	pMat4Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat4Local,0x0,(i1 * sizeof(float)));
	pMat5Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat5Local,0x0,(i1 * sizeof(float)));
	pMat6Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat6Local,0x0,(i1 * sizeof(float)));
	pMat7Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat7Local,0x0,(i1 * sizeof(float)));
	pMat8Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat8Local,0x0,(i1 * sizeof(float)));
	pMat9Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat9Local,0x0,(i1 * sizeof(float)));
	pMat10Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat10Local,0x0,(i1 * sizeof(float)));
	pMat11Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat11Local,0x0,(i1 * sizeof(float)));
	pMat12Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat12Local,0x0,(i1 * sizeof(float)));
	
	pMat13Local      = (shortstring *) _MALLOC(i1 * sizeof(shortstring)*6);
	memset(pMat13Local,0x0,(i1 * sizeof(char)*6));	
	pMat14Local        = (float *) _MALLOC(i1 * sizeof(float));
	memset(pMat14Local,0x0,(i1 * sizeof(float)));	
                                         
                 
	 /* Berechnen der Profiltiefe und Aufaddieren der 
	 	Anzahl der Simulationsschichten jeder Materialschicht.  */
     for (i1= 0,iSimLayerNum=0 , fProfil = (float)0; i1< pRL->iLayerDim; i1++)
     {   
 		 fProfil        	    += pRL->paiMatLayerDepth[i1];
 		 iSimLayerNum			+= pRL->paiMatLayer[i1];
     }		                                               

     for (i1= 0, fStartProfil = (float)0; i1< pRL->iStartLayerDim; i1++)
     {   
		    if (!Test_Range((float)pRL->paiStartLayerDepth[i1],(float)1,(float)1e6,"paiStartLayerDepth"))
		    {
		    Message(3,INPUT_DATA_ERROR_TXT);

			/* Freigeben des allokierten Speichers.*/    	
	    	free(pMat1Local);                                      	
			free(pMat2Local);
	    	free(pMat3Local);
			free(pMat4Local);
			free(pMat5Local);
	    	free(pMat6Local);
	    	free(pMat7Local);
		 	free(pMat8Local);
    		free(pMat9Local);
    		free(pMat10Local);
    		free(pMat11Local);
    		free(pMat12Local);
    		free(pMat13Local);
    		free(pMat14Local);

		    return -1;
		    }

 		 fStartProfil		    += pRL->paiStartLayerDepth[i1];
     }		                                               


                                
    if (!Test_Range(fProfil,(float)1,(float)1e6,"Profile Depth"))
    {
    Message(3,INPUT_DATA_ERROR_TXT);

	 /* Freigeben des allokierten Speichers.*/    	
    free(pMat1Local);                                      	
    free(pMat2Local);
    free(pMat3Local);
    free(pMat4Local);
    free(pMat5Local);
    free(pMat6Local);
    free(pMat7Local);
    free(pMat8Local);
    free(pMat9Local);
    free(pMat10Local);
    free(pMat11Local);
    free(pMat12Local);
    free(pMat13Local);
    free(pMat14Local);
    	    	
    return -1;
    }

    /* Das darf nicht sein, denn ansonsten ist keine Information vorhanden, 
    	wie untere Schichten zu initialisieren sind: */
    if (fStartProfil < fProfil -  (float)1)
    {
    Message(1,START_DEPTH2SMALL_ERROR_TXT);
    Message(3,START_DATA_ERROR_TXT);
    return -1;
	}

    /* Das kann beabsichtigt sein: */
    if (fStartProfil > fProfil + (float)1)
    {
    Message(1,COMMENT_START_DEPTH2BIG_TXT);
	}

	 /* Die Simulationsschichten werden ergaenzt um die erste und letzte 
	    virtuelle Schicht, die fuer die Randbedingungen notwendig sind. Dies
	    ist die Gesamtanzahl an Simulationsschichten.  */
		iSimLayerNum += 2;
		pSo->iLayers = iSimLayerNum;

    if (!Test_Range((float)pSo->iLayers,(float)9,(float)301,"iLayers"))
    {
    Message(3,INPUT_DATA_ERROR_TXT);
    return -1;
    }

	 /* Berechnen der aequidistanten
	 	 Schichtdicke jeder Simulationsschicht  */
	 pSo->fDeltaZ = fProfil / (iSimLayerNum-2);

	 /* Die aequidistanten Schichtdicke wird fuer jede
	    Simulationsschicht gespeichert. Die erste und letzte
	    virtuelle Schichtdicke ist gleich 0. */

	 pSo->pSLayer->fThickness = (float)0.0;
     for ( pSL = pSo->pSLayer->pNext;pSL->pNext != NULL; pSL = pSL->pNext)
     {   
     		  pSL->fThickness  = pSo->fDeltaZ;
     }		                                               
	 pSL->fThickness = (float)0.0;
                 
                 
/* Berechne nun wie die Startwerte auf die numerischen Schichten verteilt werden sollen: */
                 
     for (i1= 0, fProfil = fStartProfil = (float)0; i1< pRL->iStartLayerDim; i1++)
     {   
 		 fProfil		    += pRL->paiStartLayerDepth[i1];

 		 i2 = (int)( (float)0.5 + (fProfil - fStartProfil) / pSo->fDeltaZ);

         if ( i2 < 1)
         {
         Message(1,START_DEPTH2SMALL_ERROR_TXT);
         i2 = 1;
         }
         
         fStartProfil       += i2 * pSo->fDeltaZ;

         if (fProfil != fStartProfil)
         {
         Message(1, COMMENT_START_DISTRIBUTE_TXT);
         }
 		 
 		 aiStartLayer[i1] = i2;

     }		                                               

	 for(i1=0;i1<pRL->iStartLayerDim;i1++)
		 paiStartLayerGLOBAL[i1] = aiStartLayer[i1];


                 
/****************** Allgemeiner Teil Ende ******************/

/**************** Beginn Block Prozent 'PSLAYER' ******************
	 In diesem Block werden die Variablen der Struktur 'PSLAYER'
	 beschrieben. Da es sich um Verhaeltniszahlen handelt ist
	 eine quantitative Verteilung nicht notwendig. 
******************************************************************/

	/* Die aus der Datenbank gelesenen Materialschicht werden auf das 
	   lokale Array geschrieben. Da die Materialschichten beim Einlesen 
	   aus der Datenbank auch auf die erste virtuelle Schicht geschrieben 
	   werden, beginnt die Schleife bei i1=0. Daraus ergibt sich, dass i1
	   kleiner sein muss als pRL->iLayerDim (5 Materialschichten bedeutet 
	   lesen bis 4 Simulationsschicht). (pSL!=NULL) ist Sicherheitsabfrage  */

	for (i1 = 0, pSL = pSo->pSLayer;
					 ((i1 < pRL->iLayerDim)&&(pSL!=NULL)); 
		pSL = pSL ->pNext,i1++)
	{
		pMat1Local[i1] = pSL->fClay;
		pMat2Local[i1] = pSL->fSilt;
		pMat3Local[i1] = pSL->fSand;
		pMat4Local[i1] = pSL->fBulkDens;
		pMat5Local[i1] = pSL->fBulkDensStart;
		pMat6Local[i1] = pSL->fRockFrac;
		pMat7Local[i1] = pSL->fImpedLiqA;
		pMat8Local[i1] = pSL->fImpedLiqB;
		pMat9Local[i1] = pSL->fpH;
		pMat10Local[i1] = pSL->fHumus;
		pMat11Local[i1] = pSL->fCHumus;
		pMat12Local[i1] = pSL->fNHumus;
		strcpy(pMat13Local[i1],pSL->acSoilID);		
		pMat14Local[i1] = pSL->fPorosity;
	}                 


	/* Die Variablen werden mit den auf den lokalen Arrays beschriebenen Werten
	   beschrieben. Dabei bleiben die erste und letzte virtuelle Simulationsschicht
	   unberuecksichtigt. Das Arrayelement wird durch die Materialschicht bestimmt.
	   Die anzahl, wie oft eine Arrayelement benutzt wird, haengt von der in der
	   Datenbank gespeicherten Anzahl von Simulationsschichten jeder Materialschicht ab.*/

	i1 = 0;
	i2 = pRL->paiMatLayer[i1];
		
	for (pSL = pSo->pSLayer->pNext;
					 (pSL->pNext != NULL); 
		pSL = pSL ->pNext)
	{
		if (i2 > 0)
		{
			pSL->fClay 			= pMat1Local[i1];
			pSL->fSilt 			= pMat2Local[i1];
			pSL->fSand 			= pMat3Local[i1];
			pSL->fBulkDens 		= pMat4Local[i1];
			pSL->fBulkDensStart = pMat5Local[i1];
			pSL->fRockFrac      = pMat6Local[i1];			
			pSL->fImpedLiqA     = pMat7Local[i1];			
			pSL->fImpedLiqB     = pMat8Local[i1];									
			pSL->fpH			= pMat9Local[i1];			
			pSL->fHumus			= pMat10Local[i1];
			pSL->fCHumus		= pMat11Local[i1];
			pSL->fNHumus		= pMat12Local[i1];						
		    
		    strcpy(pSL->acSoilID, pMat13Local[i1]);		
			pSL->fPorosity		= pMat14Local[i1];		
			i2 -= 1;
		}
		else
		{
		i1 += 1;
		i2 = pRL->paiMatLayer[i1];
			pSL->fClay 			= pMat1Local[i1];
			pSL->fSilt 			= pMat2Local[i1];
			pSL->fSand 			= pMat3Local[i1];
			pSL->fBulkDens 		= pMat4Local[i1];
			pSL->fBulkDensStart = pMat5Local[i1];
			pSL->fRockFrac      = pMat6Local[i1];			
			pSL->fImpedLiqA     = pMat7Local[i1];			
			pSL->fImpedLiqB     = pMat8Local[i1];									
			pSL->fpH			= pMat9Local[i1];			
			pSL->fHumus			= pMat10Local[i1];
			pSL->fCHumus		= pMat11Local[i1];
			pSL->fNHumus		= pMat12Local[i1];

		    strcpy(pSL->acSoilID, pMat13Local[i1]);		

			pSL->fPorosity		= pMat14Local[i1];		
			i2 -= 1;
		}
	}

	/* Die letzte virtuelle Simulationsschicht wird mit den Werten
	   der vorletzten Simulationsschicht beschrieben */

		pSL->fClay 			= pSL->pBack->fClay;
		pSL->fSilt 			= pSL->pBack->fSilt;
		pSL->fSand 			= pSL->pBack->fSand;
		pSL->fBulkDens 		= pSL->pBack->fBulkDens;
		pSL->fBulkDensStart = pSL->pBack->fBulkDensStart;
		pSL->fRockFrac      = pSL->pBack->fRockFrac;			
		pSL->fImpedLiqA     = pSL->pBack->fImpedLiqA;			
		pSL->fImpedLiqB     = pSL->pBack->fImpedLiqB;									
		pSL->fpH			= pSL->pBack->fpH;			
		pSL->fHumus			= pSL->pBack->fHumus;
		pSL->fCHumus		= pSL->pBack->fCHumus;
		pSL->fNHumus		= pSL->pBack->fNHumus;						
	    strcpy(pSL->acSoilID, pSL->pBack->acSoilID);		
		pSL->fPorosity		= pSL->pBack->fPorosity;


	/* Die erste virtuelle Simulationsschicht wird mit den Werten
	   der zweiten Simulationsschicht beschrieben */

		pSo->pSLayer->fClay 		 = pSo->pSLayer->pNext->fClay;
		pSo->pSLayer->fSilt 		 = pSo->pSLayer->pNext->fSilt;
		pSo->pSLayer->fSand 		 = pSo->pSLayer->pNext->fSand;
		pSo->pSLayer->fBulkDens 	 = pSo->pSLayer->pNext->fBulkDens;
		pSo->pSLayer->fBulkDensStart = pSo->pSLayer->pNext->fBulkDensStart;
		pSo->pSLayer->fRockFrac 	 = pSo->pSLayer->pNext->fRockFrac;
		pSo->pSLayer->fImpedLiqA 	 = pSo->pSLayer->pNext->fImpedLiqA;
		pSo->pSLayer->fImpedLiqB 	 = pSo->pSLayer->pNext->fImpedLiqB;
		pSo->pSLayer->fpH 			 = pSo->pSLayer->pNext->fpH;
		pSo->pSLayer->fHumus 		 = pSo->pSLayer->pNext->fHumus;
		pSo->pSLayer->fCHumus 		 = pSo->pSLayer->pNext->fCHumus;
		pSo->pSLayer->fNHumus 		 = pSo->pSLayer->pNext->fNHumus;				

	    strcpy(pSo->pSLayer->acSoilID, pSo->pSLayer->pNext->acSoilID);		
		pSo->pSLayer->fPorosity		 = pSo->pSLayer->pNext->fPorosity;


/****************** Ende Block Prozent 'PSLAYER' ******************/    	

  /* Die lokalen Arrays werden wieder mit Null initilaisieren */

    i1 = max(pRL->iLayerDim, pRL->iStartLayerDim);

	memset(pMat1Local,0x0,(i1 * sizeof(float)));
	memset(pMat2Local,0x0,(i1 * sizeof(float)));	
	memset(pMat3Local,0x0,(i1 * sizeof(float)));	
	memset(pMat4Local,0x0,(i1 * sizeof(float)));	
	memset(pMat5Local,0x0,(i1 * sizeof(float)));	
	memset(pMat6Local,0x0,(i1 * sizeof(float)));	
	memset(pMat7Local,0x0,(i1 * sizeof(float)));	
	memset(pMat8Local,0x0,(i1 * sizeof(float)));	
	memset(pMat9Local,0x0,(i1 * sizeof(float)));	
	memset(pMat10Local,0x0,(i1 * sizeof(float)));	
	memset(pMat11Local,0x0,(i1 * sizeof(float)));		
	memset(pMat12Local,0x0,(i1 * sizeof(float)));		
	memset(pMat13Local,0x0,(i1 * sizeof(float)));				


/**************** Beginn Block Prozent 'PSWATER' ******************
	 In dieser Struktur handelt es sich nur um Verhaeltniszahlen.
	 Eine quantitative Verteilung ist nicht notwendig. 
******************************************************************/

	/* Die aus der Datenbank gelesenen Materialschicht werden auf das 
	   lokale Array geschrieben. Da die Materialschichten beim Einlesen 
	   aus der Datenbank auch auf die erste virtuelle Schicht geschrieben 
	   werden, beginnt die Schleife bei i1=0. Daraus ergibt sich, dass i1
	   kleiner sein muss als pRL->iLayerDim (5 Materialschichten bedeutet 
	   lesen bis 4 Simulationsschicht). (pSL!=NULL) ist Sicherheitsabfrage  */

	for (i1 = 0, pWA = pSo->pSWater;
					 ((i1 < pRL->iLayerDim)&&(pWA!=NULL)); 
		pWA = pWA->pNext,i1++)
	{
		pMat1Local[i1]  = pWA->fContPWP;
		pMat2Local[i1]  = pWA->fContFK;
		pMat3Local[i1]  = pWA->fContSat;
		pMat4Local[i1]  = pWA->fMinPot;
		pMat5Local[i1]  = pWA->fCondSat;
		pMat6Local[i1]  = pWA->fContInflec;
		pMat7Local[i1]  = pWA->fPotInflec;
		pMat8Local[i1]  = pWA->fCampA;
		pMat9Local[i1]  = pWA->fCampB;
		pMat10Local[i1] = pWA->fVanGenA;
		pMat11Local[i1] = pWA->fVanGenN;
		pMat12Local[i1] = pWA->fContRes;
	}                 

	/* Die Variablen werden mit den auf den lokalen Arrays beschriebenen Werten
	   beschrieben. Dabei bleiben die erste und letzte virtuelle Simulationsschicht
	   unberuecksichtigt. Das Arrayelement wird durch die Materialschicht bestimmt.
	   Die anzahl, wie oft eine Arrayelement benutzt wird, haengt von der in der
	   Datenbank gespeicherten Anzahl von Simulationsschichten jeder Materialschicht ab.*/
	
	i1 = 0;
	i2 = pRL->paiMatLayer[i1];
		
	for (pWA = pSo->pSWater->pNext;
					 (pWA->pNext != NULL); 
		pWA = pWA->pNext)
	{
		if (i2 > 0)
		{
			pWA->fContPWP 	 = pMat1Local[i1];
			pWA->fContFK  	 = pMat2Local[i1];
			pWA->fContSat 	 = pMat3Local[i1];
			pWA->fMinPot  	 = pMat4Local[i1];
			pWA->fCondSat	 = pMat5Local[i1];
			pWA->fContInflec = pMat6Local[i1];			
			pWA->fPotInflec  = pMat7Local[i1];			
			pWA->fCampA		 = pMat8Local[i1];									
			pWA->fCampB		 = pMat9Local[i1];			
			pWA->fVanGenA 	 = pMat10Local[i1];			
			pWA->fVanGenN	 = pMat11Local[i1];									
			pWA->fContRes	 = pMat12Local[i1];									
			i2 -= 1;
		}
		else
		{
		i1 += 1;
		i2 = pRL->paiMatLayer[i1];
			pWA->fContPWP 	 = pMat1Local[i1];
			pWA->fContFK  	 = pMat2Local[i1];
			pWA->fContSat 	 = pMat3Local[i1];
			pWA->fMinPot  	 = pMat4Local[i1];
			pWA->fCondSat	 = pMat5Local[i1];
			pWA->fContInflec = pMat6Local[i1];			
			pWA->fPotInflec  = pMat7Local[i1];			
			pWA->fCampA		 = pMat8Local[i1];									
			pWA->fCampB		 = pMat9Local[i1];			
			pWA->fVanGenA 	 = pMat10Local[i1];			
			pWA->fVanGenN	 = pMat11Local[i1];									
			pWA->fContRes	 = pMat12Local[i1];									
			i2 -= 1;
		}
	}

	/* Die letzte virtuelle Simulationsschicht wird mit den Werten
	   der vorletzten Simulationsschicht beschrieben */

			pWA->fContPWP 	 = pWA->pBack->fContPWP;
			pWA->fContFK  	 = pWA->pBack->fContFK;
			pWA->fContSat 	 = pWA->pBack->fContSat;
			pWA->fMinPot  	 = pWA->pBack->fMinPot;
			pWA->fCondSat	 = pWA->pBack->fCondSat;
			pWA->fContInflec = pWA->pBack->fContInflec;
			pWA->fPotInflec  = pWA->pBack->fPotInflec;
			pWA->fCampA		 = pWA->pBack->fCampA;
			pWA->fCampB		 = pWA->pBack->fCampB;
			pWA->fVanGenA 	 = pWA->pBack->fVanGenA;
			pWA->fVanGenN	 = pWA->pBack->fVanGenN;
			pWA->fContRes	 = pWA->pBack->fContRes;


	/* Die erste virtuelle Simulationsschicht wird mit den Werten
	   der zweiten Simulationsschicht beschrieben */

		pSo->pSWater->fContPWP 		 = pSo->pSWater->pNext->fContPWP;
		pSo->pSWater->fContFK 		 = pSo->pSWater->pNext->fContFK;
		pSo->pSWater->fContSat 		 = pSo->pSWater->pNext->fContSat;
		pSo->pSWater->fMinPot 		 = pSo->pSWater->pNext->fMinPot;
		pSo->pSWater->fCondSat 		 = pSo->pSWater->pNext->fCondSat;
		pSo->pSWater->fContInflec 	 = pSo->pSWater->pNext->fContInflec;
		pSo->pSWater->fPotInflec 	 = pSo->pSWater->pNext->fPotInflec;
		pSo->pSWater->fCampA 		 = pSo->pSWater->pNext->fCampA;
		pSo->pSWater->fCampB 		 = pSo->pSWater->pNext->fCampB;
		pSo->pSWater->fVanGenA 		 = pSo->pSWater->pNext->fVanGenA;
		pSo->pSWater->fVanGenN 		 = pSo->pSWater->pNext->fVanGenN;
		pSo->pSWater->fContRes 		 = pSo->pSWater->pNext->fContRes;


/****************** Ende Block Prozent 'PSWATER' ******************/    	

  /* Die lokalen Arrays werden wieder mit Null initilaisieren */

    i1 = max(pRL->iLayerDim, pRL->iStartLayerDim);

	memset(pMat1Local,0x0,(i1 * sizeof(float)));
	memset(pMat2Local,0x0,(i1 * sizeof(float)));	
	memset(pMat3Local,0x0,(i1 * sizeof(float)));	
	memset(pMat4Local,0x0,(i1 * sizeof(float)));	
	memset(pMat5Local,0x0,(i1 * sizeof(float)));	
	memset(pMat6Local,0x0,(i1 * sizeof(float)));	
	memset(pMat7Local,0x0,(i1 * sizeof(float)));	
	memset(pMat8Local,0x0,(i1 * sizeof(float)));	
	memset(pMat9Local,0x0,(i1 * sizeof(float)));	
	memset(pMat10Local,0x0,(i1 * sizeof(float)));	
	memset(pMat11Local,0x0,(i1 * sizeof(float)));		
	memset(pMat12Local,0x0,(i1 * sizeof(float)));		
	memset(pMat13Local,0x0,(i1 * sizeof(float)));				


/**************** Beginn Block Prozent 'PWLAYER' ******************
	 In dieser Struktur handelt es sich nur um Verhaeltniszahlen.
	 Eine quantitative Verteilung ist nicht notwendig. 
******************************************************************/

	/* Die aus der Datenbank gelesenen Materialschicht werden auf das 
	   lokale Array geschrieben. Da die Materialschichten beim Einlesen 
	   aus der Datenbank auch auf die erste virtuelle Schicht geschrieben 
	   werden, beginnt die Schleife bei i1=0. Daraus ergibt sich, dass i1
	   kleiner sein muss als pRL->iStartLayerDim (5 Materialschichten bedeutet 
	   lesen bis 4 Simulationsschicht). (pSL!=NULL) ist Sicherheitsabfrage  */

	for (i1 = 0, pWL = pWa->pWLayer;
					 ((i1 < pRL->iStartLayerDim)&&(pWL!=NULL)); 
		pWL = pWL ->pNext,i1++)
	{
//		/* Stimmt der Simulationsstart nicht mit dem Termin fuer die
//		   Startwerte ueberein, werden die Pools mit 0.1 initialisiert.*/
//		
//		if ((int)pTi->pSimTime->fTimeAct == (int)pMS->iDay) 
//		{
			pMat1Local[i1]  = pWL->fContInit;
//		}
//		else
//		{
//			pMat1Local[i1]  = (float)0.1;
//		}
	}                 

	/* Die Variablen werden mit den auf den lokalen Arrays beschriebenen Werten
	   beschrieben. Dabei bleiben die erste und letzte virtuelle Simulationsschicht
	   unberuecksichtigt. Das Arrayelement wird durch die Materialschicht bestimmt.
	   Die Anzahl, wie oft eine Arrayelement benutzt wird, haengt von der in der
	   Datenbank gespeicherten Anzahl von Simulationsschichten jeder Materialschicht ab.*/

	i1 = 0;
	i2 = aiStartLayer[i1];
		
	for (pWL = pWa->pWLayer->pNext;
					 (pWL->pNext != NULL); 
		pWL = pWL ->pNext)
	{
		if (i2 > 0)
		{
			pWL->fContInit = pMat1Local[i1];
			i2 -= 1;
		}
		else
		{
		i1 += 1;
		i2 = aiStartLayer[i1];
			pWL->fContInit = pMat1Local[i1];
			i2 -= 1;
		}
	}

	/* Die letzte virtuelle Simulationsschicht wird mit den Werten
	   der vorletzten Simulationsschicht beschrieben */

			pWL->fContInit 	 = pWL->pBack->fContInit;

	/* Die erste virtuelle Simulationsschicht wird mit den Werten
	   der zweiten Simulationsschicht beschrieben */

		pWa->pWLayer->fContInit = pWa->pWLayer->pNext->fContInit;


/****************** Ende Block Prozent 'PWLAYER' ******************/    	


  /* Die lokalen Arrays werden wieder mit Null initilaisieren */

    i1 = max(pRL->iLayerDim, pRL->iStartLayerDim);

	memset(pMat1Local,0x0,(i1 * sizeof(float)));
	memset(pMat2Local,0x0,(i1 * sizeof(float)));	
	memset(pMat3Local,0x0,(i1 * sizeof(float)));	
	memset(pMat4Local,0x0,(i1 * sizeof(float)));	
	memset(pMat5Local,0x0,(i1 * sizeof(float)));	
	memset(pMat6Local,0x0,(i1 * sizeof(float)));	
	memset(pMat7Local,0x0,(i1 * sizeof(float)));	
	memset(pMat8Local,0x0,(i1 * sizeof(float)));	
	memset(pMat9Local,0x0,(i1 * sizeof(float)));	
	memset(pMat10Local,0x0,(i1 * sizeof(float)));	
	memset(pMat11Local,0x0,(i1 * sizeof(float)));		
	memset(pMat12Local,0x0,(i1 * sizeof(float)));		

  /* Interner Schichtzaehler auf null setzten */


/**************** Beginn Block Prozent 'PHLAYER' ******************
	 In dieser Struktur handelt es sich nur um Verhaeltniszahlen.
	 Eine quantitative Verteilung ist nicht notwendig. 
******************************************************************/

	/* Die aus der Datenbank gelesenen Materialschicht werden auf das 
	   lokale Array geschrieben. Da die Materialschichten beim Einlesen 
	   aus der Datenbank auch auf die erste virtuelle Schicht geschrieben 
	   werden, beginnt die Schleife bei i1=0. Daraus ergibt sich, dass i1
	   kleiner sein muss als pRL->iLayerDim (5 Materialschichten bedeutet 
	   lesen bis 4 Simulationsschicht). (pSL!=NULL) ist Sicherheitsabfrage  */

	for (i1 = 0, pHL = pHe->pHLayer;
					 ((i1 < pRL->iStartLayerDim)&&(pHL!=NULL)); 
		pHL = pHL ->pNext,i1++)
	{
//		/* Stimmt der Simulationsstart nicht mit dem Termin fuer die
//		   Startwerte ueberein, werden die Pools mit 0.1 initialisiert.*/
//		
//		if ((int)pTi->pSimTime->fTimeAct == (int)pMS->iDay) 
//		{
			pMat1Local[i1]  = pHL->fSoilTemp;
//		}
//		else
//		{
//			pMat1Local[i1]  = (float)10.0;
//		}
	}                 

	/* Die Variablen werden mit den auf den lokalen Arrays beschriebenen Werten
	   beschrieben. Dabei bleiben die erste und letzte virtuelle Simulationsschicht
	   unberuecksichtigt. Das Arrayelement wird durch die Materialschicht bestimmt.
	   Die anzahl, wie oft eine Arrayelement benutzt wird, haengt von der in der
	   Datenbank gespeicherten Anzahl von Simulationsschichten jeder Materialschicht ab.*/

	i1 = 0;
	i2 = aiStartLayer[i1];
		
	for (pHL = pHe->pHLayer->pNext;
					 (pHL->pNext != NULL); 
		pHL = pHL ->pNext)
	{
		if (i2 > 0)
		{
			pHL->fSoilTemp = pMat1Local[i1];
			i2 -= 1;
		}
		else
		{
		i1 += 1;
		i2 = aiStartLayer[i1];
			pHL->fSoilTemp = pMat1Local[i1];
			i2 -= 1;
		}
	}

	/* Die letzte virtuelle Simulationsschicht wird mit den Werten
	   der vorletzten Simulationsschicht beschrieben */

		pHL->fSoilTemp 	 = pHL->pBack->fSoilTemp;

	/* Die erste virtuelle Simulationsschicht wird mit den Werten
	   der zweiten Simulationsschicht beschrieben */

		pHe->pHLayer->fSoilTemp = pHe->pHLayer->pNext->fSoilTemp;


/****************** Ende Block Prozent 'PHLAYER' ******************/    	


  /* Die lokalen Arrays werden wieder mit Null initilaisieren */

  /* Die lokalen Arrays werden wieder mit Null initilaisieren */

    i1 = max(pRL->iLayerDim, pRL->iStartLayerDim);

	memset(pMat1Local,0x0,(i1 * sizeof(float)));
	memset(pMat2Local,0x0,(i1 * sizeof(float)));	
	memset(pMat3Local,0x0,(i1 * sizeof(float)));	
	memset(pMat4Local,0x0,(i1 * sizeof(float)));	
	memset(pMat5Local,0x0,(i1 * sizeof(float)));	
	memset(pMat6Local,0x0,(i1 * sizeof(float)));	
	memset(pMat7Local,0x0,(i1 * sizeof(float)));	
	memset(pMat8Local,0x0,(i1 * sizeof(float)));	
	memset(pMat9Local,0x0,(i1 * sizeof(float)));	
	memset(pMat10Local,0x0,(i1 * sizeof(float)));	
	memset(pMat11Local,0x0,(i1 * sizeof(float)));		
	memset(pMat12Local,0x0,(i1 * sizeof(float)));		

  /* Interner Schichtzaehler auf null setzten */


/**************** Beginn Block Prozent 'PCLAYER' ******************
	 In diesem Block werden die Variablen Struktur 'PCLAYER'
	 mengenmaessig auf die Simulationsschichten verteilt.
*******************************************************************/

	f1 = (float)0.0;


	/* Die aus der Datenbank gelesenen Materialschicht werden auf das 
	   lokale Array geschrieben. Da die Materialschichten beim Einlesen 
	   aus der Datenbank auch auf die erste virtuelle Schicht geschrieben 
	   werden, beginnt die Schleife bei i1=0. Daraus ergibt sich, dass i1
	   kleiner sein muss als pRL->iLayerDim (5 Materialschichten bedeutet 
	   lesen bis 4 Simulationsschicht). (pSL!=NULL) ist Sicherheitsabfrage  */

	for (i1 = 0, pCL = pCh->pCLayer;
					 ((i1 < pRL->iLayerDim)&&(pSL!=NULL)); 
		pCL = pCL ->pNext,i1++)
	{
	
		/* Stimmt der Simulationsstart nicht mit dem Termin fuer die
		   Startwerte ueberein, werden die Pools mit 0.1 initialisiert.*/
		
//		if ((int)pTi->pSimTime->fTimeAct == (int)pMS->iDay) 
//		{
			pMat1Local[i1]  = pCL->fNO3N;
			pMat2Local[i1]  = pCL->fNH4N;
			f1			   += pCL->fNO3N + pCL->fNH4N;
//		}
//		else
//		{
//			pMat1Local[i1]  = (float)0.1;
//			pMat2Local[i1]  = (float)0.1;
//			f1			   += (float)0.2;
//		}
		
	}                 

	/* Die Variablen werden mit den auf den lokalen Arrays beschriebenen Werten
	   beschrieben. Dabei bleiben die erste und letzte virtuelle Simulationsschicht
	   unberuecksichtigt. Das Arrayelement wird durch die Materialschicht bestimmt.
	   Die Anzahl, wie oft eine Arrayelement benutzt wird, haengt von der in der
	   Datenbank gespeicherten Anzahl von Simulationsschichten jeder Materialschicht ab.*/

	i1 = 0;                     
	i2 = 0;


	pCh->pCBalance->fNO3NProfile = (float)0.0;
	pCh->pCBalance->fNH4NProfile = (float)0.0;
	pCh->pCBalance->fNProfileStart = (float)0.0;


     for (SOIL_LAYERS1(pCL,pCh->pCLayer->pNext))
	{
	/* Berechnung der Menge, die auf jede Simulationsschicht
	   verteilt wird. Dies muss zu Beginn einer jeden Materialschicht
	   durchgefuehrt werden. */			
		if (i2 == 0)
		{
	
		/* Menge pro aequidistante Schicht ohne Beruecksichtigung eines
		   Restes aus der vorhergehenden Materialschicht */			
			part1 = pMat1Local[i1] / aiStartLayer[i1]; 
			part2 = pMat2Local[i1] / aiStartLayer[i1];
			
			i2    = aiStartLayer[i1];
			
			i1++;
		}

		/* i2 beschreibt die Anzahl der Simulationsschichten, die jeder 
		   Materialschicht in der Datenbank zugeordnet wurde. Nach dem 
		   Beschreiben einer Simulationsschicht wird dieser Zaehler verrringert. */
		  
		   i2--;


			pCL->fNO3N = part1;                      
			pCL->fNH4N = part2;                      

/* Debug:
	 		 itoa((int)((float)1000*pCL->fNO3N), lpOut, 10);
			 strcat((LPSTR)lpOut," g /ha NO3");
      		 Message(1, lpOut);
*/

		pCh->pCBalance->fNO3NProfile += pCL->fNO3N;
		pCh->pCBalance->fNH4NProfile += pCL->fNH4N;
		pCh->pCBalance->fNProfileStart += pCL->fNO3N + pCL->fNH4N;
	}
                  
       if ((f1 < pCh->pCBalance->fNProfileStart - 1)||(f1 > pCh->pCBalance->fNProfileStart + 1))
       {
    	Message(1,START_BALANCE_ERROR_TXT); 
    	return -1;
       }
	/* Nachdem die Mengen der Variablen auf die Simulationsschichten verteilt worden sind
	   muessen sowohl der erste als auch die letzte virtuelle Simulationsschicht beschrieben
	   werden. Dies geschieht durch kopieren der zweiten bzw. vorletzten Simulationsschicht.*/

			/* Die letzte virtuelle Simulationsschicht wird mit den Werten
			   der vorletzten Simulationsschicht beschrieben */

				pCL->fNO3N = pCL->pBack->fNO3N;
				pCL->fNH4N = pCL->pBack->fNH4N;

			/* Die erste virtuelle Simulationsschicht wird mit den Werten
			   der zweiten Simulationsschicht beschrieben */

				pCh->pCLayer->fNO3N = pCh->pCLayer->pNext->fNO3N;
				pCh->pCLayer->fNH4N = pCh->pCLayer->pNext->fNH4N;

/****************** Ende Block Prozent 'PCLAYER' ******************/    	


	 /* Freigeben des allokierten Speichers.*/    	
    	free(pMat1Local);                                      	
    	free(pMat2Local);
    	free(pMat3Local);
    	free(pMat4Local);
    	free(pMat5Local);
    	free(pMat6Local);
    	free(pMat7Local);
    	free(pMat8Local);
    	free(pMat9Local);
    	free(pMat10Local);
    	free(pMat11Local);
    	free(pMat12Local);
    	free(pMat13Local);
    	free(pMat14Local);
    	    	
   return 1;
     
}  // Ende Verteilen auf aequidistante Sim.schichten


/*******************************************************************************
** EOF */
