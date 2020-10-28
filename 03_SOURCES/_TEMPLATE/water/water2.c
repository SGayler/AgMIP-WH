/******************************************************/
/**                                                  **/
/**  Modul     Wasser   Methode NSIM                 **/
/**                                                  **/
/**  Name    : water2                                **/
/**  Autoren : Th.Schaaf                             **/
/**                                                  **/
/**  Datum   : 12.08.96                              **/
/******************************************************/

#include  "xinclexp.h"
#include "xlanguag.h"     

#define MINIMAL_TIMESTEP_CAPACITY_MODEL (float)0.1

/***********************************************************************/
/**                         Functionen                                **/
/***********************************************************************/
/* from util_fct.c */
extern int	NewDay(PTIME);
extern int  SimStart(PTIME);

extern int   WINAPI Message(long, LPSTR);

signed short int WINAPI WasserflussRitchie(EXP_POINTER);
signed short int WINAPI UngesaettigterWasserflussRitchie(EXP_POINTER);


/*********************************************************************************/
/*  Name     : WasserflussRitchie                                                */
/*                                                                               */
/*  Funktion : Berechnung der Wassergehalte im Bodenprofil                       */
/*             Kapazitaets/Schichtenmodell   NSIM                                */
/*  Autor    : Th. Schaaf                                                        */
/*  Datum	 : 12.08.96                                                          */
/*                                                                               */
/*  Change   : Dokumentation der veraenderten globalern Variablen                */
/*             pWa->pWLayer->fContAct                                            */
/*             pWa->fLeaching                                                    */
/*                                                                               */
/*********************************************************************************/

signed short int WINAPI WasserflussRitchie(EXP_POINTER)
{
 signed short int OK=0;
 float  Rate,Rest,Sirate,minVersickerung;
 float	FluxTemp[MAXSCHICHT]={(float)0.0};
 int	i1;

 PWLAYER	pWL;
 PSWATER	pSW;
 PSLAYER	pSL;
 PWBALANCE  pWB  = pWa->pWBalance;

 float fContTemp ;
 float DeltaT       = pTi->pTimeStep->fAct;



    if (NewDay(pTi))
    {
	/* Berechnung des fuer Infiltration in die erste Schicht
 	   zur Verfuegung stehenden Wassers. Die Variable pWa->fRunOffR
 	   stellt den Runoff des Zeitschrittes dar. Da hier nur einmal 
 	   am Tag gerechnet wird muss durch den Zeitschritt dividiert 
 	   werden.*/
	
		pWB->fReservoir -= pWa->fRunOffR;
	    pWa->fPotInfiltration = pWB->fReservoir - pWa->fPondWater; 
		pWB->fReservoir -= pWa->fPotInfiltration;
		pWa->fInfiltR = pWa->fPotInfiltration;
		
		if (pWB->fReservoir < (float)0)
		{                        
			pWB->fReservoir = (float)0;
		
			#ifdef LOGFILE
			 {Message(0,ERROR_RESERVOIR_NEGATIV);
			 } 
			#endif
		}
	}

 
 if (DeltaT >= MINIMAL_TIMESTEP_CAPACITY_MODEL)
 {
      /* Das in die erste Bodenschicht infiltrierte Wasser wird auf den
         Fluss der ersten Bodenschicht geschrieben. ch, 17.6.97 Bei Leach 
         Ansatz wird Fluss der 0. Schicht als Infiltration angesehen! */
	    pWa->pWLayer->fFlux              = pWa->fInfiltR * DeltaT;
	    pWa->pWLayer->pNext->fFlux       = pWa->fInfiltR * DeltaT;
	    pWa->pWLayer->fFluxDens          = pWa->fInfiltR * DeltaT;
	    pWa->pWLayer->pNext->fFluxDens   = pWa->fInfiltR * DeltaT;
    
    
  /* Berechnung der minimalen Sickerrate im gesamten Profil */
     minVersickerung = (float) 1.0;

    for (pSW = pSo->pSWater->pNext;
          (pSW->pNext->pNext != NULL);
          pSW = pSW->pNext)
    {
		Sirate = max((float)0,(pSW->fContSatCER - pSW->fContFK) / pSW->fContSatCER);
    	if (Sirate < minVersickerung)
		{
		  minVersickerung = Sirate;
	 	}        
     } // Ende Berechnung der minimlaen Versickerungsrate

	  minVersickerung = minVersickerung * DeltaT;

 if (((pWa->fInfiltR * DeltaT) > (float)0.0)||(pWa->fPondWater > (float) 0.0))
 {  /* Es gibt eine Infiltration in die oberste Bodenschicht  */
 
	/* schichtweise Berechnung des Flusses aufgrund der 
		Infiltration in oberste Schicht */

	for(i1=0, pWL = pWa->pWLayer;
		(pWL->pNext != NULL);
		pWL = pWL->pNext,i1++)
	{              
		FluxTemp[i1] = pWL->fFlux;
	}	

     for (pWL = pWa->pWLayer->pNext,
          pSW = pSo->pSWater->pNext,
          pSL = pSo->pSLayer->pNext;
                                    ((pWL->pNext != NULL)&&
                                     (pSW->pNext != NULL)&&
                                     (pSL->pNext != NULL));
          pWL = pWL->pNext,
          pSW = pSW->pNext,
          pSL = pSL->pNext)
     {        
     
       if (pWL->fFlux == (float)0.0)
       {     /* wenn kein Fluss in akt. Schicht erfolgt */   
              
         if (pWL->fContAct >= (pSW->fContFK + (float)0.003))
         {    /* wenn akt. H2O-Gehalt nur wenig oberhalb von FK */
                                            /* !!! pWa->fInfiltration falsches Attribut!!!  */
           Rate = ((pWL->fContAct - pSW->fContFK) * minVersickerung
                                    * pSL->fThickness);
           pWL->fContAct -= (Rate / pSL->fThickness);

           pWL->fFlux = Rate;
           
           if (pWL->pNext->pNext != NULL)
           {
            pWL->pNext->fFlux  = pWL->fFlux;
           }  /* Ende von: if   pNext->pNext     */
                               
           }  /* Ende von: wenn akt W-Gehalt nur wenig oberhalb von FK */
           else
           {  /* wenn akt. H2O-Gehalt nicht oberhalb FK liegt */
           
            pWL->fFlux = (float)0.0;

            if (pWL->pNext->pNext != NULL)
               {
                pWL->pNext->fFlux  = (float)0.0; //pWL->fFlux;
               }  /* Ende von: if   pNext->pNext     */

           }  /* Ende von: wenn akt. H2O-Gehalt nicht oberhalb FK liegt */

       }  /* Ende von: wenn kein Fluss in akt. Schicht erfolgt */
       else
       {  /* Es gibt einen Fluss in die akt. Schicht */
       
          Rest = (pSW->fContSatCER - pWL->fContAct) * (pSL->fThickness);

          if (pWL->fFlux > Rest)
          {  /* bei Fluss in akt. Schicht wuerde akt H2O-Gehalt groesser Saettigung  */
                                                      
             Rate = max((float)0,(pSW->fContSatCER - pSW->fContFK)) * minVersickerung * pSL->fThickness;

			 fContTemp = pWL->fContAct;
             pWL->fContAct = pSW->fContSatCER - (Rate / pSL->fThickness);

             //pWL->fFlux  = pWL->fFlux  -  Rest  +  Rate;
			 pWL->fFlux  = pWL->fFlux - (pWL->fContAct-fContTemp)*pSL->fThickness;

             if (pWL->pNext->pNext != NULL)
             {
              pWL->pNext->fFlux  = pWL->fFlux;
             }  /* Ende von: if   pNext->pNext     */

          }  /* Ende von: bei Fluss in akt. Schicht wuerde akt H2O-Gehalt groesser Saettigung */
          else
          {  /* bei Fluss in akt. Schicht wuerde akt H2O-Gehalt kleiner/gleich Saettigung */
          
             pWL->fContAct += (pWL->fFlux / pSL->fThickness);

            if (pWL->fContAct >= (pSW->fContFK + (float)0.003))
             { /* bei Fluss in akt. Schicht liegt H2O-Gehalt zwischen FK und Saettigung */
             
                Rate = ((pWL->fContAct - pSW->fContFK) * minVersickerung
                              * pSL->fThickness);

                pWL->fContAct -= Rate / pSL->fThickness;

                pWL->fFlux = Rate;

                if (pWL->pNext->pNext != NULL)
                {
                   pWL->pNext->fFlux  = pWL->fFlux;
                }  /* if   pNext->pNext     */

             }  /* Ende von: bei Fluss in akt. Schicht liegt H2O-Gehalt 
             									zwischen FK und Saettigung */
             else
             { /* bei Fluss in akt. Schicht liegt H2O-Gehalt unterhalb FK */
             
                pWL->fFlux = (float)0.0;

                if (pWL->pNext->pNext != NULL)
                {
                   pWL->pNext->fFlux  = (float)0.0; //pWL->fFlux;

                }  /* if   pNext->pNext     */

             }  /* Ende von: bei Fluss in akt. Schicht liegt H2O-Gehalt unterhalb FK */

          }    /* Ende von: bei Fluss in akt. Schicht ist H2O-Gehalt kleiner der Saettigung */

       }/* Ende von: es erfolgt ein Fluss in die aktuelle Schicht*/

     }  /* Ende der schichtweisen Berechnung*/



  /*  Aktualisieren der Variable fuer den Zeitschritt fFluxDens */	

     for (pWL = pWa->pWLayer;
          pWL->pNext != NULL;
          pWL = pWL->pNext)
     {
          pWL->fFluxDens = pWL->fFlux/ DeltaT;
     }
 
 
 }
 else  /* Wenn keine Infiltration wird Fluss auf null gesetzt. */
 {
     for (pWL = pWa->pWLayer;
          pWL->pNext != NULL;
          pWL = pWL->pNext)
     {
          pWL->fFluxDens = pWL->fFlux = (float)0;
     }
 } /* Ende else pWa->Infiltration */

  /*  Der Fluss (Rate) aus der untersten Bodenschicht ist
      das Sickerwasser aus einem definierten Profil. Da letzte
      Schicht bei Kapazitaetsmodell nicht berechnet muss der
      Fluss aus der vorletzten Schicht verwendet werden.  */	

     pWa->fLeaching = pWL->pBack->fFlux; 
     pWa->fPercolR  = pWL->pBack->fFluxDens;   

 } /* Ende if DeltaT */

 return OK;

}      /*=====   Wasserfluss im Bodenprofil nach NSIM  =================*/




/**********************************************************************************/
/*  Name     : UngesaettigterWasserflussRitchie                                   */
/*                                                                                */
/*  Funktion : Die berechnete Evaporations wird aus der ersten Bodenschicht       */
/*             realisiert. Die Wasserverluste dieser Schicht werden bei Bedarf    */
/*             durch die darunterliegenden Schichten ausgeglichen. Diese Auf-     */
/*             waertsgerichtete Wasserbewegung wird berechnet. Darueber hinaus    */
/*             wird ein Wasserfluss berechnet. der auch ohne Infiltrationsereig-  */
/*             nis ein abwaertsgerichtete Wasserbewegung zulaesst.                */
/*                                                                                */
/*  Autor    : Th. Schaaf                                                         */
/*  Datum    : 12.08.96                                                           */
/*  Change   : Dokumentation der veraenderten globalern Variablen                 */
/*             pWa->pWLayer->fContAct                                             */
/*             pWa->pEvap->fActDay                                                */
/*             pWa->pWLayer->fFlux                                                */
/*             pWa->pWLayer->fContOld                                             */                             
/*                                                                                */
/**********************************************************************************/
signed short int WINAPI UngesaettigterWasserflussRitchie(EXP_POINTER)
{
	signed short int OK=0;
	int i1;
	static float aktEvapdt;
	float f1,grad,H2O1,H2O2,H2ODurch,capH201,capH202,diff,SumDlz,minEvap,
			H2OTemp[MAXSCHICHT]={(float)0.0},FluxTemp[MAXSCHICHT]={(float)0.0},
			FluxTempSAT[MAXSCHICHT]={(float)0.0},maxFlux;


	PSLAYER  pSL;
	PWLAYER	 pWL;
	PSWATER	 pSW;
	                                                                     
 float DeltaT       = pTi->pTimeStep->fAct;
 
 if (DeltaT >= MINIMAL_TIMESTEP_CAPACITY_MODEL)
 {
 
	minEvap = (float)0.9 - ((float)0.0000038 * ((pSo->pSLayer->pNext->fThickness) - (float)300)
                      				     * ((pSo->pSLayer->pNext->fThickness) - (float)300));

	minEvap = minEvap * DeltaT;

	/* Nach Errechnung der erfolgten Evaporation muss der Wassergehalt der
	   obersten Bodenschicht aktualisiert werden. */

   //Vom Wassergehalt der 1. Schicht wird akt. Evaporation abgezogen.
    pWa->pWLayer->pNext->fContAct 
          				-= (pWa->pEvap->fActR * DeltaT) 
          									/ pSo->pSLayer->pNext->fThickness;

	/* Ist aktueller Wassergehalt kleiner als Gehalt bei PWP * min Eavoration,
		wird nur das verfuegbare Wasser abgezogen.  */
	 if(pWa->pWLayer->pNext->fContAct < (pSo->pSWater->pNext->fContPWP * minEvap))
     {
	       f1 =  (pSo->pSWater->pNext->fContPWP * minEvap - pWa->pWLayer->pNext->fContAct)
	           									* pSo->pSLayer->pNext->fThickness;
	       pWa->pWLayer->pNext->fContAct = minEvap * pSo->pSWater->pNext->fContPWP;
	
	       pWa->pEvap->fActDay  -= f1 / DeltaT;
	       pWa->pEvap->fActR    -= f1 / DeltaT;
      } // Ende: akt. Wassergehalt ist kleiner Gehalt bei PWP * minEvap


  //============ Berechnung Wasserbewegung aufgrund Evaporation ================

	/* Nachdem die Evaporation aus der ersten Bodenschicht erfolgt ist
	   werden alle anderen Bodenschichten aktualisiert. Hierzu wird der
	   bisher berechnete Abwaertsfluss durch die Aufwaertsbewegung korregiert.
	   Daneben ist auch ein abwaertsgerichter Fluss moeglich, obwohl kein Niederschlag
	   erfolgt ist. Die Aufwaertsbewegung wird daher zunaechst auf die lokale Variable
	   FluxTemp geschrieben.Der temporaerer Wassergehalt wird mit dem aktuellen
	   Wassergehalt der Bodenschicht initialisiert.*/

	for(i1=1, pWL = pWa->pWLayer->pNext;
		(pWL->pNext != NULL);
		pWL = pWL->pNext,i1++)
	{              
		FluxTemp[i1] = (float)0.0;
        H2OTemp[i1] = pWL->fContAct;
	}	

	/* Wenn die Schichtdicke der ersten Schicht kleiner 5 cm ist 
		wird mit der zweiten Schicht begonnen. */
     if (pSo->pSLayer->pNext->fThickness < (float)50.0)      //(float)5.0) mm/cm!!!!!
     {
        pSL = pSo->pSLayer->pNext->pNext;  // Anfang = zweite Schicht
        pWL = pWa->pWLayer->pNext->pNext;
        pSW = pSo->pSWater->pNext->pNext;
        i1 = 2;
     }  // Schichtdicke < 5 cm

     else // Schichtdicke ist groesser gleich 5 cm
     {
        pSL = pSo->pSLayer->pNext;         // Anfang = erste Schicht
        pWL = pWa->pWLayer->pNext;
        pSW = pSo->pSWater->pNext;
        i1 = 1;
     }  //  Ende: Schichtdicke ist groesser gleich 5 cm

     for(  ;
         ((pSL->pNext!= NULL)&&      //  Anfangsschicht bis vorletzte
          (pWL->pNext!= NULL)&&
          (pSW->pNext!= NULL)&&
          (i1 < MAXSCHICHT));
                                   pSL = pSL->pNext,
                                   pWL = pWL->pNext,
                                   pSW = pSW->pNext,
                                   i1++)
    {
	   capH201 = pSW->fContFK - pSW->fContPWP;               //nFk obere Schicht
	   capH202 = pSW->pNext->fContFK - pSW->pNext->fContPWP; //nFk untere Schicht	   
       H2O1 = pWL->fContAct - pSW->fContPWP;                 //verfuegbares H2O obere Schicht
       H2O1 = max((float)0.0,H2O1);
       H2O2 = pWL->pNext->fContAct - pSW->pNext->fContPWP;   //verfuegbares H2O untere Schicht
       H2O2 = max((float)0.0,H2O2);
       SumDlz = pSL->fThickness + pSL->pNext->fThickness;    //Schichtdicke ober + untere
       
	/* Da bei Schichtdicken von 2 bis 3 cm Fehler aufgetreten sind wurde eine
	   Aenderung nach Wang (Dissertation Lehreinheit,1996) integriert. */       

		H2ODurch = ((H2O1 * pSL->fThickness) + (H2O2 * pSL->pNext->fThickness))/SumDlz;
        
        diff = (float)0.88 * (float)exp((double)((float)35.4 * H2ODurch));
     	diff = min(diff,(float)100.0);
		diff = diff * DeltaT;
		grad = (H2O2 - H2O1) / (SumDlz * (float)0.5);                                           

		FluxTemp[i1] = diff * grad;

	/* Fuer einen aufwaertsgerichteten Fluss werden hier positive Zahlen ermittelt.Bei
	   positiven Zahlen der Variable pWL->fFlux handelt es sich um abwaertgerichten Fluss.
	   Daher ist hier die Umwandlung des Vorzeichens von FluxTemp[i1] notwendig. */

		FluxTemp[i1] = FluxTemp[i1] * (float)-1.0;

		maxFlux = ((H2O2 - H2O1) * pSL->fThickness * pSL->pNext->fThickness) / SumDlz;

		if (fabs (FluxTemp[i1]) > fabs(maxFlux))
		{
			FluxTemp[i1] = maxFlux;		
		}

	/* Bei aufwaertsgerichtetem Fluss muss gewaehrleistet werden, dass bei Fluss in die 
	   obere Schicht der Wassergehalt nicht die Feldkapazitaet uebersteigt. Da der aufwaerts-
	   gerichtete Fluss negativ ist muss er fuer die Abfrage *-1 genommen werden. */

		if (FluxTemp[i1] < (float)0.0)
		{
			if ((((FluxTemp[i1] * (float)-1.0) / pSL->fThickness) + H2OTemp[i1]) > (pSW->fContFK))
			{
				FluxTemp[i1] = ((pSW->fContFK) - H2OTemp[i1]) * pSL->fThickness;
				FluxTemp[i1] = FluxTemp[i1] * (float)-1.0;
			}
		}

		
		     //StefanAchatz  bei abwaertigem Fluss darf 
			 //Saettigungswert nicht ueberschritten werden

		if (FluxTemp[i1] > (float)0.0)
		{
			if (((FluxTemp[i1] / pSL->fThickness) + H2OTemp[i1+1]) > (pSW->pNext->fContSatCER))
				FluxTemp[i1] = ((pSW->pNext->fContSatCER) - H2OTemp[i1+1]) * pSL->fThickness;
		}
		

	/* Bei aufwaertsgerichtetem Fluss wird H2OTemp[i1+1] erniedrigt und
	   H2OTemp[i1] erhoeht, da FluxTemp[i1] in einem solchen Fall negativ ist. */

		H2OTemp[i1]    -= FluxTemp[i1] / pSL->fThickness;
		H2OTemp[i1+1]  += FluxTemp[i1] / pSL->fThickness;

    } //  Ende der schichtweisen Flussberechnung im Profil

	/* Aktualisiern der aktuellen Wassergehalte nachdem die 
		berechneten Fluesse realisiert wurden. Ausserdem werden
		die abwaertsgerichteten und die aufwaertsgerichteten Fluesse
		miteinander verrechnet.
		Aufwaerts ==> negativ; Abwaerts ==> positiv. */       

    for(pWL= pWa->pWLayer->pNext,i1=1;
        pWL->pNext != NULL;
        pWL = pWL->pNext,i1++)
    {
       pWL->fContOld = pWL->fContAct;
       pWL->fContAct = H2OTemp[i1];
       pWL->fFlux += FluxTemp[i1];
       pWL->fFluxDens = pWL->fFlux/DeltaT;
    } // Ende der Aktualisierung der Wassergehalte

 } /* Ende if DeltaT */
 
 else
 { /* Kein Wassertransport bei zu geringem Zeitschritt! */
     for (pWL = pWa->pWLayer;
          pWL->pNext->pNext != NULL;
          pWL = pWL->pNext)
     {
          pWL->fFluxDens = pWL->fFlux = (float)0;
     }
 
     pWa->fLeaching = (float)0;
  }

   return OK;
   }  // Ende Ungesättigter Wasserfluss

