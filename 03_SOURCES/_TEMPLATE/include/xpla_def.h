/*********************************************************************************/
/*                                                                               */
/*   PLA_DEF.H                                                                   */
/*   Header for plant related routines                                           */
/*                                                                               */
/*                                                                               */
/*   Authors: C.Haberbosch (ch), D.Maier (dm), E.Priesack (ep),                  */
/*                                                                               */
/*            GSF- National Research Center for Environment and Health           */
/*            Institute of Soil Ecology                                          */
/*            P.O.Box 1129                                                       */
/*            D-85758 Neuherberg                                                 */
/*            GERMANY                                                            */
/*                                                                               */
/*   Date: March 1997                                                            */
/*                                                                               */
/*   Changes:                                                                    */
/*                                                                               */
/*********************************************************************************/

// this define is used by the interpolating plant model
#define TIME_DURING_PLANT_MEASUREMENTS    ((pPM != NULL) && (pPM->pNext != NULL)\
                     && ((int)(SimTime) >= pPM->iDay) && ((int)(SimTime) <= pPM->pNext->iDay)\
                     && (pPM->fNAboveBiomass <= pPM->pNext->fNAboveBiomass)\
                     && (pPM->fLAI <= pPM->pNext->fLAI))


