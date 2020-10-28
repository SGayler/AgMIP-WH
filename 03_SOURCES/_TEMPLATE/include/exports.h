/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 2 $
 *
 * $History: exports.h $
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/include
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
*******************************************************************************/

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  LIBRARY EXPORT - MAIN PLANT PROCESS FUNCTIONS
//------------------------------------------------------------------------------------------------------------
int DLL PhasicDevelopment_SPASS				(EXPERT_N_POINTERS);
int DLL Photosynthesis_SPASS				(EXPERT_N_POINTERS);
int DLL BiomassGrowth_SPASS					(EXPERT_N_POINTERS);
int DLL CanopyFormation_SPASS				(EXPERT_N_POINTERS);
int DLL RootSystemFormation_SPASS			(EXPERT_N_POINTERS);
int DLL OrganSenescence_SPASS				(EXPERT_N_POINTERS);
int DLL WaterCapture_SPASS					(EXPERT_N_POINTERS);
int DLL NitrogenCapture_SPASS				(EXPERT_N_POINTERS);
int DLL CropMaintenance_SPASS				(EXPERT_N_POINTERS);

int DLL COMVR_TO_ZSTAGE_AND_CERES(EXPERT_N_POINTERS);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  LIBRARY EXPORT - SUB PROCESS FUNCTIONS
//------------------------------------------------------------------------------------------------------------
//Biological processes
float DLL RelativeTemperatureResponse_SPASS	(float fTemp, float fMinTemp, float fOptTemp, float fMaxTemp);
float DLL ThermalDevelopment_SPASS	(float fTempMin,float fTempMax,PRESPONSE pResp,float fMinTemp,float fOptTemp,float fMaxTemp);
float DLL DailyVernalization_SPASS	(float fTempMin,float fTempMax,PRESPONSE pResp,float fMinTemp,float fOptTemp,float fMaxTemp);
float DLL VernalizationEffect_SPASS	(float fCumVern, float fBasVern, float fCrtVern);
float DLL Photoperiodism_SPASS		(float fDaylen,PRESPONSE pResp, float fOptDaylen,float fSensF);

//Climate/weather processes
float DLL Daylength_SPASS			(float fLatitude, int iJulianDay);
float DLL HourlyTemperature_SPASS	(int iOclock, float fTempMax, float fTempMin);


//Assimilation,dissimilation and growth
float DLL LeafMaxGrossPhotosynthesis_SPASS(float fPgmmax,float fTemp,float fCO2,float fCO2Cmp,float fCO2R,
										   PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc);

float DLL LeafLightUseEfficiency_SPASS(float fMaxLUE, float fTemp, float fCO2, LPSTR pType);
float DLL CanopyGrossPhotosynthesis_SPASS(float fPgMax, float fLUE, float fLAI, 
										  float fLatitude,int nDay,float fHour, float fPARdir,float fPARdif);
float DLL DailyCanopyGrossPhotosythesis_SPASS(LPSTR pType,float fPgmmax,float fMaxLUE,float fCO2Cmp,float fCO2R,
											  PRESPONSE pResp, PCARDTEMP pCardTmp, PORGANNC pLfNc, float fLAI,
											  float fLatitude,int nDay,
											  float fRad,float fTmpMax,float fTmpMin,float fCO2);

int DLL AssimilatePartition_SPASS(EXPERT_N_POINTERS);
int DLL OrganBiomassGrowth_SPASS(EXPERT_N_POINTERS);
int DLL ReserveTranslocation_SPASS(EXPERT_N_POINTERS);

//Root growth										  
int DLL RootExtension_SPASS(EXPERT_N_POINTERS);
int DLL RootLengthDensity_SPASS(EXPERT_N_POINTERS);         


//Water Uptake
float DLL CanopyWaterInterception_SPASS(float fLAI,float fRainfall);
float DLL PotentialTranspiration_SPASS(EXPERT_N_POINTERS);
int   DLL ActualTranspiration_SPASS(EXPERT_N_POINTERS);
int   DLL PlantWaterStress_SPASS(EXPERT_N_POINTERS);

//Nitrogen uptake
int DLL PlantNitrogenStress_SPASS			(EXPERT_N_POINTERS);
int DLL NitrogenUptake_SPASS				(EXPERT_N_POINTERS);
int DLL PlantNitrogenTranslocation_SPASS	(EXPERT_N_POINTERS);
int DLL PlantNitrogenDemand_SPASS			(EXPERT_N_POINTERS);
int DLL NitrogenConcentrationLimits_SPASS	(EXPERT_N_POINTERS);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  TOOL FUNCTIONS
//------------------------------------------------------------------------------------------------------------
float DLL AFGENERATOR(float fInput, RESPONSE* pfResp);
double DLL LIMIT(double v1, double v2, double x);

int DLL 	ReadSymbol(HANDLE,LPSTR);

int DLL 	ReadDate(HANDLE,LPSTR);

int	DLL 	ReadInt(HANDLE);
long DLL 	ReadLong(HANDLE);
float DLL 	ReadFloat(HANDLE);
double DLL 	ReadDouble(HANDLE);
long double DLL ReadLongDouble(HANDLE);

int DLL DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex);
int DLL LongToDate(long dValue,LPSTR lpDate,BOOL bFullYearNumber);
                                                              
int DLL StringGetFromTo(LPSTR lpStr1,LPSTR lpStr2,int nBeg,int nEnd);                                                              
int DLL FloatToString(float fNumber,int iDecimal,LPSTR lpString);
int DLL FloatToWindow(HDC hdc,int X,int Y,float fNumber,int iDecimal);
int DLL FloatWrite(HANDLE hFile,float fNumber,int iDecimal);
int DLL EqualLenWrite(HANDLE hFile, float fValue, int Len);


int SPASS_Genotype_Read(HANDLE hInstance);
