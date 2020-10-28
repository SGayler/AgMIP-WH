
//	Modul zum Allokieren von Speicherplatz für die 
//	GECROS-spezifischen Variablenstrukturen
//
//	Definition der Strukturen siehe gecros.h

//Standard C-Functions Librarys
#include <windows.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xinclexp.h"
#include "gecros.h"


int WINAPI allocateGECROSVariables();
int WINAPI freeAllocatedMemory();

int WINAPI allocGecrosParameter();
int WINAPI allocGecrosGenotype();
int WINAPI allocGecrosBiomass();
int WINAPI allocGecrosCarbon();
int WINAPI allocGecrosNitrogen();
int WINAPI allocGecrosCanopy();
int WINAPI allocGecrosSoil();

int WINAPI freeGecrosGenotype(PGECROSGENOTYPE);

PGECROSGENOTYPE		pGecrosGenotype;
PGECROSPARAMETER	pGecrosParameter;
PGECROSBIOMASS		pGecrosBiomass;
PGECROSCARBON		pGecrosCarbon;
PGECROSNITROGEN		pGecrosNitrogen;
PGECROSCANOPY		pGecrosCanopy;
PGECROSSOIL         pGecrosSoil;



int WINAPI allocateGECROSVariables()
{	
		/*
	    GlobalFree(pGecrosParameter);
		GlobalFree(pGecrosGenotype);
		GlobalFree(pGecrosBiomass);
		GlobalFree(pGecrosCarbon);
		GlobalFree(pGecrosNitrogen);
		GlobalFree(pGecrosCanopy);
		GlobalFree(pGecrosSoil);	
	
		GlobalFree(pGecrosPlant);
		GlobalUnlock(pGecrosPlant);
		*/

		pGecrosPlant = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosPlant));
	
		allocGecrosParameter();
		allocGecrosGenotype();
		allocGecrosBiomass();
		allocGecrosCarbon();
		allocGecrosNitrogen();
		allocGecrosCanopy();
		allocGecrosSoil();

		pGecrosPlant->pGecrosParameter = pGecrosParameter;
		pGecrosPlant->pGecrosGenotype = pGecrosGenotype;
		pGecrosPlant->pGecrosBiomass = pGecrosBiomass;
		pGecrosPlant->pGecrosCarbon  = pGecrosCarbon;
		pGecrosPlant->pGecrosNitrogen = pGecrosNitrogen;
		pGecrosPlant->pGecrosCanopy = pGecrosCanopy;
		pGecrosPlant->pGecrosSoil = pGecrosSoil;

	return 1;
}



int WINAPI allocGecrosParameter()
{
	GlobalUnlock(pGecrosParameter);
	pGecrosParameter = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosParameter));
	return 1;
}


int WINAPI allocGecrosGenotype()
{
	GlobalUnlock(pGecrosGenotype);

	pGecrosGenotype = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosGenotype));

	return 1;
}


int WINAPI allocGecrosBiomass()
{
	GlobalUnlock(pGecrosBiomass);
	pGecrosBiomass = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosBiomass));
	return 1;
}



int WINAPI allocGecrosCarbon()
{
	GlobalUnlock(pGecrosCarbon);
	pGecrosCarbon = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosCarbon));
	return 1;
}

int WINAPI allocGecrosNitrogen()
{
	GlobalUnlock(pGecrosNitrogen);
	pGecrosNitrogen = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosNitrogen));
	return 1;
}

int WINAPI allocGecrosCanopy()
{
	GlobalUnlock(pGecrosCanopy);
	pGecrosCanopy = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosCanopy));
	return 1;
}

int WINAPI allocGecrosSoil()
{
	GlobalUnlock(pGecrosSoil);
	pGecrosSoil = GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(struct stGecrosSoil));
	return 1;
}



int WINAPI freeAllocatedGECROSMemory()
{
	HGLOBAL hGlobal;
	
	PGECROSPARAMETER pGParameter;
	PGECROSGENOTYPE	 pGGenotype;
	PGECROSBIOMASS	 pGBiomass;
	PGECROSCARBON    pGCarbon;
	PGECROSNITROGEN  pGNitrogen;
	PGECROSCANOPY    pGCanopy;
	PGECROSSOIL      pGSoil;

		pGParameter	= pGecrosPlant->pGecrosParameter;
		pGGenotype	= pGecrosPlant->pGecrosGenotype;
		pGBiomass	= pGecrosPlant->pGecrosBiomass;
		pGCarbon	= pGecrosPlant->pGecrosCarbon;
		pGNitrogen	= pGecrosPlant->pGecrosNitrogen;
		pGCanopy	= pGecrosPlant->pGecrosCanopy;
		pGSoil	    = pGecrosPlant->pGecrosSoil;
        
        GlobalUnlock(pGParameter);
		hGlobal=GlobalFree(pGParameter);
		pGParameter=NULL;

        GlobalUnlock(pGGenotype);
	    hGlobal=GlobalFree(pGGenotype);
		pGGenotype=NULL;

		GlobalUnlock(pGBiomass);
		hGlobal=GlobalFree(pGBiomass);
		pGBiomass=NULL;

		GlobalUnlock(pGCarbon);
		hGlobal=GlobalFree(pGCarbon);
		pGCarbon=NULL;

		GlobalUnlock(pGNitrogen);
		hGlobal=GlobalFree(pGNitrogen);
		pGNitrogen=NULL;

		GlobalUnlock(pGCanopy);
		hGlobal=GlobalFree(pGCanopy);
		pGCanopy=NULL;

		GlobalUnlock(pGSoil);
		hGlobal=GlobalFree(pGSoil);
		pGSoil=NULL;

		GlobalUnlock(pGecrosPlant);
		hGlobal=GlobalFree(pGecrosPlant);
		pGecrosPlant=NULL;


	return 1;
}






