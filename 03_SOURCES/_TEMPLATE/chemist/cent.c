//===========================================================================================
//									adapted CENTURY MODEL
//  								 SB - 2008/2009
//===========================================================================================
#include  <memory.h>
#include	"xinclexp.h"
#include	"xn_def.h"   
#include	"xh2o_def.h"  
#include	"century.h"
 
extern int SimStart(PTIME);			// from time1.c
extern int NewDay(PTIME);
extern int introduce(LPSTR lpName);	// from util_fct.c
#define SimulationStarted  		(SimStart(pTi))


//============================================================================================
//             DLL-Funktionen                       
//============================================================================================
int WINAPI  dllMiner_CENT(EXP_POINTER); //Implementation of Daycent's Mineralization  Model
int WINAPI  dllNitri_CENT(EXP_POINTER); //Implementation of Daycent's Nitrification   Model
int WINAPI  dllDenit_CENT(EXP_POINTER); //Implementation of Daycent's Denitrification Model


//=============================================================================================
// DLL-Function:  DayCent Decomposition          
//=============================================================================================
int WINAPI  dllMiner_CENT(EXP_POINTER)
  {
//	float n2,n1;
	if SimulationStarted introduce((LPSTR)"dllCentury");
	// Start of every days tasks
	
	
	
	if (NewDay(pTi)) 
	{	
		CENTminer(exp_p);  //actual call of Daycent's Decomposition submodel	
	}


	 return 0;
  }


//=============================================================================================
// DLL-Function:  DayCent Nitrification
//=============================================================================================
int WINAPI  dllNitri_CENT(EXP_POINTER)
 {
	 
	if (NewDay(pTi))
	{
	Nitrify(exp_p); //daily loss of NH4 due to nitrification [gN/m2]
					//NH4 goes to NH3, N2O and N0
	}
	return 0;
 }

//=============================================================================================
// DLL-Function:  DayCent Nitrification
//=============================================================================================
int WINAPI  dllDenit_CENT(EXP_POINTER)
 {
	 
	if (NewDay(pTi))
	{
	Denitrify(exp_p); //daily loss of NH4 due to nitrification [gN/m2]
					//NH4 goes to NH3, N2O and N0
	}
	return 0;
 }


