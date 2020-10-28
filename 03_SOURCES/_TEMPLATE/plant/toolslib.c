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
 * $Revision: 4 $
 *
 * $History: toolslib.c $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:22
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/plant
 * Methoden sollten auch im Fehlerfall einen initialisierten Resultwert
 * zurückgeben.
 * Unsinnige Defines entfernt (DLL wird überdies bereits verwendet).
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/plant
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 * 
*******************************************************************************/

#include <windows.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include  "xinclexp.h"

//==========================================================================
// Function Prototypes
//==========================================================================
/*
int 		ReadSymbol(HANDLE,LPSTR);
int 		ReadDate(HANDLE,LPSTR);

int			ReadInt(HANDLE);
long		ReadLong(HANDLE);
float		ReadFloat(HANDLE);
double		ReadDouble(HANDLE);
long double ReadLongDouble(HANDLE);

int DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex);
int LongToDate(long dValue,LPSTR lpDate,BOOL bFullYearNumber);
                                                              
int StringGetFromTo(LPSTR lpStr1,LPSTR lpStr2,int nBeg,int nEnd);                                                              
int FloatToString(float fNumber,int iDecimal,LPSTR lpString);
int FloatToWindow(HDC hdc,int X,int Y,float fNumber,int iDecimal);
int FloatWrite(HANDLE hFile,float fNumber,int iDecimal);

int EqualLenWrite(HANDLE hFile, float fValue, int Len);

float LIMIT(float v1, float v2, float x);

*/

float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp);

int WINAPI _loadds FloatToString(float fNumber,int iDecimal,LPSTR lpString);
double	WINAPI _loadds	ReadDouble(HANDLE);
//SG 20111019
float WINAPI _loadds ReadFloatOrNewLine(HANDLE);

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Function: float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp)
//Author:	Enli Wang
//Date:		07.11.1996
//Purpose:	This function interpolates linearly a value from a RESPONSE table using input fInput
//Inputs:	1. fInput	- the input value (-)
//			2. pfResp	- Pointer to the first element of the given RESPONSE table	
//Outputs:	The interpolated value at fInput (-)
//Functions Called: 
//Reference:1. Wang,Enli. xxxx.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
float WINAPI _loadds AFGENERATOR(float fInput, RESPONSE* pfResp)
	{
	RESPONSE *pResp, *pResp1;
	float	 fSlope,fOutput;
	int	  	 nPoints,i;
	
	nPoints = (int)(pfResp->fInput); 

	pResp = pfResp + 1;
  	if (fInput<=pResp->fInput)
		return pResp->fOutput;
        
	pResp = pfResp + nPoints;
	if (fInput>=pResp->fInput)
		return pResp->fOutput;

	pResp = pfResp + 1;   
	pResp1= pResp  + 1;
	for (i=1;i<nPoints;i++)
		{
		if (	((fInput>=pResp->fInput)&&(fInput<pResp1->fInput))
			||  ((fInput<=pResp->fInput)&&(fInput>pResp1->fInput))   )
			break;
		else
			{
			pResp ++;
			pResp1++;
			}
		}
                       
    fSlope = (pResp1->fOutput- pResp->fOutput)/(pResp1->fInput-pResp->fInput);                   
	fOutput= pResp->fOutput + fSlope*(fInput-pResp->fInput);

	return fOutput;
	}	


double WINAPI _loadds FCNSW(double x, double v1, double v2, double v3)
	{                      
	double vReturn;
		                                               
	if (x<0)	vReturn=v1;
	if (x==0)	vReturn=v2;
	if (x>0)	vReturn=v3; 
	
	return vReturn;
	}

double WINAPI _loadds LIMIT(double v1, double v2, double x)
	{                      
	double vReturn;
	
	if (v1>v2)	v1=v2;
	                                               
	if (x<=v1)			vReturn=v1;
	if (x>=v2)			vReturn=v2;
	if ((x>v1)&&(x<v2))	vReturn=x; 
	
	return vReturn;
	}

double WINAPI _loadds NOTNUL(double x)
	{                      
	double vReturn;
	
	if (x==(double)0)	vReturn=(double)1;
	else                vReturn=x;                               
	
	return vReturn;
	}

double WINAPI _loadds INSW(double x, double y1, double y2)
	{                      
	double vReturn;
	
	if (x<(double)0)	vReturn=y1;
	if (x>=(double)0)	vReturn=y2;
	
	return vReturn;
	}
	
double WINAPI _loadds REAAND(double x, double y)
	{                      
	double vReturn;
	
	if ((x>(double)0)&&(y>(double)0))
	{
	 vReturn=(double)1;
	}
	else
	{
	 vReturn=(double)0;
	}
	
	return vReturn;
	}

double WINAPI _loadds REANOR(double x, double y)
	{                      
	double vReturn;
	
	if ((x<=(double)0)&&(y<=(double)0))
	{
	 vReturn=(double)1;
	}
	else
	{
	 vReturn=(double)0;
	}
	
	return vReturn;
	}

int WINAPI _loadds EqualLenWrite(HANDLE hFile, float fValue, int Len)
{
	int 	i,nTxtLen,nRes;
  DWORD dwBytesWritten;
	char 	Txt[20],Txt0[11];

	nTxtLen=FloatToString(fValue, 2, Txt0);
	nRes=10-nTxtLen;
	memset(Txt,0,20);
  for (i=1; i<=nRes; i++)
  {
    lstrcat(Txt," ");
  }
  lstrcat(Txt,Txt0);
  WriteFile(
    /* hFile                  */ hFile,
    /* lpBuffer               */ Txt,
    /* nNumberOfBytesToWrite  */ 10,
    /* lpNumberOfBytesWritten */ &dwBytesWritten,
    /* lpOverlapped 	        */ NULL );	

	return 1;
}

//==========================================================================
// Variables
//==========================================================================

int 	nLoop,nBreak,nSymbol;
char	cData[256],cChar[2];
	
char    cChar0[2]={"0\0"};
char 	cChar9[2]={"9\0"};
char    cCharLowerA[2]={"a\0"};
char 	cCharLowerZ[2]={"z\0"};
char    cCharUpperA[2]={"A\0"};
char 	cCharUpperZ[2]={"Z\0"};

//==========================================================================
// int Read a Symbol
//==========================================================================
int WINAPI _loadds ReadSymbol(HANDLE hFile,LPSTR lpSymbol)
{            
	DWORD  dwBytesRead = 0;
  nLoop 	= 0;
	nBreak	= 0;            

	memset(cData,0,80); 
	
	while (nLoop<2000)
	{
    ReadFile(
      /* hFile                */ hFile,
      /* lpBuffer             */ cChar,
      /* nNumberOfBytesToRead */ 1,
      /* lpNumberOfBytesRead  */ &dwBytesRead,
      /* lpOverlapped         */ NULL );	
    cChar[1]='\0';
			
		if ( lstrcmp(cChar," \0")
      && lstrcmp(cChar,"\t")
      && lstrcmp(cChar,"\r")
      && lstrcmp(cChar,"\n") )
		{
			lstrcat((LPSTR)cData,(LPSTR)cChar);
			nBreak++;	
			
			if (nBreak >100) 
        nBreak =1;
		}
	  else
  	{
		  if (nBreak !=0 )
			  nLoop=10000;
		}
	}
		
  lstrcpy(lpSymbol,(LPSTR)cData);
    
	return lstrlen(cData);
}

//SG 20111019
//==========================================================================
// Reads the last Float Value of a line, if present; if there is no entry, 
// return before reading the first value of the following line. 
//==========================================================================
float WINAPI _loadds ReadFloatOrNewLine(HANDLE hFile)
{            
  DWORD dwBytesRead = 0;
  BOOL  bValue=0;

	nLoop 	=0;
	nSymbol	=0;
	nBreak	=0;            

	memset(cData,0,80); 
	
	while (nLoop<2000)
	{
    ReadFile(
      /* hFile                */ hFile,
      /* lpBuffer             */ cChar,
      /* nNumberOfBytesToRead */ 1,
      /* lpNumberOfBytesRead  */ &dwBytesRead,
      /* lpOverlapped         */ NULL );	
    cChar[1]='\0';
			
		if (  (lstrcmp(cChar,cChar0)>=0) && (lstrcmp(cChar,cChar9)<=0)
			||(lstrcmp(cChar,"+\0") ==0) || (lstrcmp(cChar,"-\0") ==0)
			||(lstrcmp(cChar,".\0") ==0) || (lstrcmp(cChar,"e\0") ==0)
			||(lstrcmp(cChar,"E\0") ==0) || (lstrcmp(cChar,"d\0") ==0)
			||(lstrcmp(cChar,"D\0") ==0) )  
		{
			lstrcat((LPSTR)cData,(LPSTR)cChar);
			nBreak++;	
			bValue = 1;
			
			if (  (lstrcmp(cChar,"+\0") ==0) || (lstrcmp(cChar,"-\0") ==0)
			    ||(lstrcmp(cChar,".\0") ==0) || (lstrcmp(cChar,"e\0") ==0)
			    ||(lstrcmp(cChar,"E\0") ==0) || (lstrcmp(cChar,"d\0") ==0)
			    ||(lstrcmp(cChar,"D\0") ==0))
			{
				nSymbol++;
			}   

			if (nSymbol>100) 
				nSymbol=1;

			if (nBreak >100) 
				nBreak =1;	
		}
		else
		{
			if (nBreak==nSymbol)
			{
				memset(cData,0,80); 
				nLoop=0;

				if((bValue==0)&&(!lstrcmp(cChar,"\r") || !lstrcmp(cChar,"\n")))
					return (float)0.0;
			}
			else
			{
				nLoop=10000;

				if((bValue==0)&&(!lstrcmp(cChar,"\r") || !lstrcmp(cChar,"\n")))
					return (float)0.0;
			}
		}
	}
	return (float)atof(cData);
}

//==========================================================================
// int Read a Date
//==========================================================================
int WINAPI _loadds ReadDate(HANDLE hFile,LPSTR lpDate)
{            
  DWORD dwBytesRead = 0;
	nLoop 	=0;
	nSymbol	=0;
	nBreak	=0;            

	memset(cData,0,80); 
	
	while (nLoop<2000)
	{
    ReadFile(
      /* hFile                */ hFile,
      /* lpBuffer             */ cChar,
      /* nNumberOfBytesToRead */ 1,
      /* lpNumberOfBytesRead  */ &dwBytesRead,
      /* lpOverlapped         */ NULL );	
    cChar[1]='\0';
			
		if ( ( lstrcmp(cChar,cChar0) >= 0 )
      && ( lstrcmp(cChar,cChar9) <= 0 )
			|| ( lstrcmp(cChar,".\0") == 0 )
      || ( lstrcmp(cChar,"/\0") == 0 ) )
		{
			lstrcat((LPSTR)cData,(LPSTR)cChar);
			nBreak++;	
			
			if ((lstrcmp(cChar,".\0") ==0) || (lstrcmp(cChar,"/\0") ==0) )
				nSymbol++;
			    
			if (nSymbol>100) nSymbol=1;	
			if (nBreak >100) nBreak =1;	
		}
		else
		{
			if (nBreak==nSymbol)
			{
				memset(cData,0,80); 
				nLoop=0;
			}
			else
				nLoop=10000;
		}
	}
		
  lstrcpy(lpDate,(LPSTR)cData);
    
	return lstrlen(cData);
}

//==========================================================================
// int Read a Integer Value
//==========================================================================
int WINAPI _loadds ReadInt(HANDLE hFile)
{            
  DWORD dwBytesRead = 0;
	nLoop 	=0;
	nSymbol	=0;
	nBreak	=0;            

	memset(cData,0,80); 
	
	while (nLoop<2000)
	{
    ReadFile(
      /* hFile                */ hFile,
      /* lpBuffer             */ cChar,
      /* nNumberOfBytesToRead */ 1,
      /* lpNumberOfBytesRead  */ &dwBytesRead,
      /* lpOverlapped         */ NULL );	
    cChar[1]='\0';
			
		if ( ( lstrcmp(cChar,cChar0) >= 0 )
      && ( lstrcmp(cChar,cChar9) <= 0 )
			|| ( lstrcmp(cChar,"+\0") == 0 )
      || ( lstrcmp(cChar,"-\0") == 0 ) )
		{
			lstrcat((LPSTR)cData,(LPSTR)cChar);
			nBreak++;	
			
			if ( (lstrcmp(cChar,"+\0")==0) || (lstrcmp(cChar,"-\0")==0) )
		    nSymbol++;
			    
			if (nSymbol>100) 
        nSymbol=1;	
			if (nBreak >100) 
        nBreak =1;	
		}
		else
		{
			if (nBreak==nSymbol)
			{
				memset(cData,0,80); 
				nLoop=0;
			}
			else
				nLoop=10000;
		}
	}
		
	return atoi(cData);
}

//==========================================================================
// long Read a Long Integer Value
//==========================================================================
long WINAPI _loadds ReadLong(HANDLE hFile)
{            
  DWORD dwBytesRead = 0;
	nLoop 	=0;
	nSymbol	=0;
	nBreak	=0;            

	memset(cData,0,80); 
	
	while (nLoop<2000)
	{
    ReadFile(
      /* hFile                */ hFile,
      /* lpBuffer             */ cChar,
      /* nNumberOfBytesToRead */ 1,
      /* lpNumberOfBytesRead  */ &dwBytesRead,
      /* lpOverlapped         */ NULL );	
    cChar[1]='\0';
			
		if ( ( lstrcmp(cChar,cChar0) >= 0 )
      && ( lstrcmp(cChar,cChar9) <=0 )
			|| ( lstrcmp(cChar,"+\0") == 0 )
      || ( lstrcmp(cChar,"-\0") == 0 ) )
		{
			lstrcat((LPSTR)cData,(LPSTR)cChar);
			nBreak++;	
			
			if ( (lstrcmp(cChar,"+\0")==0) || (lstrcmp(cChar,"-\0")==0) )
			  nSymbol++;
			    
			if (nSymbol>100) 
        nSymbol=1;	
			if (nBreak >100) 
        nBreak =1;
		}
		else
		{
			if (nBreak==nSymbol)
			{
				memset(cData,0,80); 
				nLoop=0;
			}
			else
				nLoop=10000;
		}
	}
		
	return atol(cData);
}


//==========================================================================
// float Read a Float Value
//==========================================================================
float WINAPI _loadds ReadFloat(HANDLE hFile)
	{
	return (float)ReadDouble(hFile);
	}

//==========================================================================
// double  Read a Double Value
//==========================================================================
double WINAPI _loadds ReadDouble(HANDLE hFile)
{            
  DWORD dwBytesRead = 0;
	nLoop 	=0;
	nSymbol	=0;
	nBreak	=0;            

	memset(cData,0,80); 
	
	while (nLoop<2000)
	{
    ReadFile(
      /* hFile                */ hFile,
      /* lpBuffer             */ cChar,
      /* nNumberOfBytesToRead */ 1,
      /* lpNumberOfBytesRead  */ &dwBytesRead,
      /* lpOverlapped         */ NULL );	
    cChar[1]='\0';
			
		if (  (lstrcmp(cChar,cChar0)>=0) && (lstrcmp(cChar,cChar9)<=0)
			||(lstrcmp(cChar,"+\0") ==0) || (lstrcmp(cChar,"-\0") ==0)
			||(lstrcmp(cChar,".\0") ==0) || (lstrcmp(cChar,"e\0") ==0)
			||(lstrcmp(cChar,"E\0") ==0) || (lstrcmp(cChar,"d\0") ==0)
			||(lstrcmp(cChar,"D\0") ==0) )  
		{
			lstrcat((LPSTR)cData,(LPSTR)cChar);
			nBreak++;	
			
			if (  (lstrcmp(cChar,"+\0") ==0) || (lstrcmp(cChar,"-\0") ==0)
			    ||(lstrcmp(cChar,".\0") ==0) || (lstrcmp(cChar,"e\0") ==0)
			    ||(lstrcmp(cChar,"E\0") ==0) || (lstrcmp(cChar,"d\0") ==0)
			    ||(lstrcmp(cChar,"D\0") ==0))
      {
		    nSymbol++;
			}    
			if (nSymbol>100) 
        nSymbol=1;	
			if (nBreak >100) 
        nBreak =1;	
		}
		else
		{
			if (nBreak==nSymbol)
			{
				memset(cData,0,80); 
				nLoop=0;
			}
			else
				nLoop=10000;
		}
	}
	return atof(cData);
}

//==========================================================================
// long double  Read a Long Double Value
//==========================================================================
long double WINAPI _loadds ReadLongDouble(HANDLE hFile)
{            
  DWORD dwBytesRead = 0;
	nLoop 	=0;
	nSymbol	=0;
	nBreak	=0;            

	memset(cData,0,80); 
	
	while (nLoop<2000)
	{
    ReadFile(
      /* hFile                */ hFile,
      /* lpBuffer             */ cChar,
      /* nNumberOfBytesToRead */ 1,
      /* lpNumberOfBytesRead  */ &dwBytesRead,
      /* lpOverlapped         */ NULL );	
    cChar[1]='\0';
			
		if (  (lstrcmp(cChar,cChar0)>=0) && (lstrcmp(cChar,cChar9)<=0)
			||(lstrcmp(cChar,"+\0") ==0) || (lstrcmp(cChar,"-\0") ==0)
			||(lstrcmp(cChar,".\0") ==0) || (lstrcmp(cChar,"e\0") ==0)
			||(lstrcmp(cChar,"E\0") ==0) || (lstrcmp(cChar,"d\0") ==0)
			||(lstrcmp(cChar,"D\0") ==0) )  
		{
			lstrcat((LPSTR)cData,(LPSTR)cChar);
			nBreak++;	
			
			if (  (lstrcmp(cChar,"+\0") ==0) || (lstrcmp(cChar,"-\0") ==0)
			    ||(lstrcmp(cChar,".\0") ==0) || (lstrcmp(cChar,"e\0") ==0)
			    ||(lstrcmp(cChar,"E\0") ==0) || (lstrcmp(cChar,"d\0") ==0)
			    ||(lstrcmp(cChar,"D\0") ==0))
      {
			  nSymbol++;
      }
			    
			if (nSymbol>100)
        nSymbol=1;	
			if (nBreak >100)
        nBreak =1;
		}
		else
		{
			if (nBreak==nSymbol)
			{
				memset(cData,0,80); 
				nLoop=0;
			}
			else
				nLoop=10000;
		}
	}
	return atol(cData);
}  


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//	Function (int DaysToDate):  transforms the julian day in the year to character date.
//		iYear		- int type. The year number (>=0, 1965,1966,1993,1994, etc.).
//		iJulianDay	- int type. The number of julian days (1<=iJulianDay<=366)
//		lpDate		- LPSTR.	Contains the transformed date (Date.Month.yyyy).
//      iIndex      - int type. 1= Day to Date.  2= Date to Days.
//	Return value:	iIndex=1, Return= The length of lpDate.
//                  iIndex=2, Return= Julian Days.
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
int WINAPI _loadds DateChange(int iYear, int iJulianDay,LPSTR lpDate, int iIndex)
{
	int	  i, iMonth, iDaysOfMonth[13], iDaysInMonth, iDays, iReturn;
  char  szBuffer[6];


	for (iMonth=1; iMonth<=12; iMonth++)
    iDaysOfMonth[iMonth]=31;

  iDaysOfMonth[4]=iDaysOfMonth[6]=iDaysOfMonth[9]=iDaysOfMonth[11]=30;
	iDaysOfMonth[2]=28;
	if (0==(iYear%4))
    iDaysOfMonth[2]=29;

	switch(iIndex)
	{
		case 1://Julian Days to Charactor Date

			if ((iYear<0)||(iJulianDay>366)||(iJulianDay<1))
				return 0;

			iMonth=iDays=0;
			while (iDays<iJulianDay)
			{
				iMonth=iMonth+1;
				iDays=iDays+iDaysOfMonth[iMonth];
			}
			iDaysInMonth=iDaysOfMonth[iMonth]-(iDays-iJulianDay);


			if ((iMonth<10)&&(iDaysInMonth<10))
					wsprintf(lpDate,  (LPSTR)"%02u.%02u.%u", iDaysInMonth,iMonth,iYear);
			if ((iMonth>=10)&&(iDaysInMonth<10))
				wsprintf(lpDate,  (LPSTR)"%02u.%u.%u", iDaysInMonth,iMonth,iYear);
			if ((iMonth<10)&&(iDaysInMonth>=10))
				wsprintf(lpDate,  (LPSTR)"%u.%02u.%u", iDaysInMonth,iMonth,iYear);
			if ((iMonth>=10)&&(iDaysInMonth>=10))
				wsprintf(lpDate,  (LPSTR)"%u.%u.%u", iDaysInMonth,iMonth,iYear);


			return lstrlen(lpDate);

		case 2: //Character Date to Julian Days
                                                              
     	memcpy((LPSTR)szBuffer,lpDate,6);
			szBuffer[5]='\0';
			iDaysInMonth=atoi(szBuffer);
      szBuffer[0]=szBuffer[1]=szBuffer[2]='0';
	    iMonth=atoi(szBuffer);

	    if ((iDaysInMonth<1)||(iDaysInMonth>31)||(iMonth<1)||(iMonth>12))
		    	iReturn=0;
	    else
	   	{
	    	iDays=0;
	    	for (i=1; i<iMonth; i++)
	    		iDays=iDays+iDaysOfMonth[i];
				iJulianDay=iDays+iDaysInMonth;
				iReturn=iJulianDay;
			}

	   	return iReturn;
	}
  return -1;
}

//======================================================================
//Long to data
//======================================================================
int WINAPI _loadds LongToDate(long dValue,LPSTR lpDate,BOOL bFullYearNumber)
	{
	char 	cData[5];
	int 	iValue;
	
	lstrcpy(lpDate,"\0");
	
	if (bFullYearNumber==TRUE)
		{
		//Get the day
		iValue=(int)(dValue/1000000L);
		_itoa(iValue,cData,10);
		
		if (iValue<10)     
			lstrcat(lpDate,"0\0");

		lstrcat(lpDate,cData);
		lstrcat(lpDate,".\0");
		  		
		//Get the month
		iValue=(int)(dValue/10000L-dValue/1000000L*100L);
		_itoa(iValue,cData,10);

		if (iValue<10)     
			lstrcat(lpDate,"0\0");

		lstrcat(lpDate,cData);
		lstrcat(lpDate,".\0");
		  		
		//Get the year
		iValue=(int)(dValue-dValue/10000*10000L);
		_itoa(iValue,cData,10);
		lstrcat(lpDate,cData);
		}
	
	if (bFullYearNumber==FALSE)
		{
		//Get the day
		iValue=(int)(dValue/10000L);
		_itoa(iValue,cData,10);

		if (iValue<10)     
			lstrcat(lpDate,"0\0");

		lstrcat(lpDate,cData);
		lstrcat(lpDate,".\0");
		  		
		//Get the month
		iValue=(int)(dValue/100L-dValue/10000L*100L);
		_itoa(iValue,cData,10);

		if (iValue<10)     
			lstrcat(lpDate,"0\0");

		lstrcat(lpDate,cData);
		lstrcat(lpDate,".\0");
		  		
		//Get the year
		iValue=1900+(int)(dValue-dValue/100*100L);
		_itoa(iValue,cData,10);

		lstrcat(lpDate,cData);
		}
	
	return lstrlen(lpDate);
	}
		  		

/********************************************************************************************
 					int FloatToString()
*********************************************************************************************/
int WINAPI _loadds FloatToString(float fNumber,int iDecimal,LPSTR lpString)
	{
	char  	lpTxt[80];
	int 	iBefore,iAfter,multiplicator,position;
    int		iFirstChar	= 1;
    int 	iMaxDecimal = 4;

    position = 0;

    if (iDecimal > iMaxDecimal)
    	iDecimal = iMaxDecimal;
    if(fNumber < 0)
    	{
    	iFirstChar= -1;    	position = 1;    	lpTxt[0]='-';
    	fNumber = fNumber * iFirstChar;
    	}

    multiplicator = (int)pow (10,iDecimal);

	iBefore = 0;	iAfter  = 0;

	iBefore = (int)floor(fNumber);
	iAfter  = (int)floor((fNumber - iBefore)* multiplicator +0.5);

	if(iAfter == multiplicator)
		{
		iBefore = iBefore + 1;
		iAfter = 0;
		}

	if(iDecimal == 0)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d", iBefore);
	if(iDecimal == 1)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%01d", iBefore,iAfter);
	if(iDecimal == 2)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%02d", iBefore,iAfter);
	if(iDecimal == 3)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%03d", iBefore,iAfter);
	if(iDecimal == 4)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%04d", iBefore,iAfter);
	if(iDecimal == 5)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%05d", iBefore,iAfter);
	
	lstrcpy(lpString,lpTxt);

	return lstrlen(lpTxt);
	}

/********************************************************************************************
 					int FloatToWindow()
*********************************************************************************************/
int WINAPI _loadds FloatToWindow(HDC hdc,int X,int Y,float fNumber,int iDecimal)
	{
	char  	lpTxt[80];

	FloatToString(fNumber,iDecimal,lpTxt);
		
	TextOut(hdc,X,Y,lpTxt,lstrlen(lpTxt));

	return lstrlen(lpTxt);
	}
/********************************************************************************************
 					int StringGetFromTo()
*********************************************************************************************/
int WINAPI _loadds StringGetFromTo(LPSTR lpStr1,LPSTR lpStr2,int nBeg,int nEnd)
	{
	LPSTR lpS;
	
	lpS=lpStr1+nBeg-1;
	
	lstrcpyn(lpStr2,lpS,nEnd-nBeg+1+1); 
	
	return lstrlen(lpStr2);
	}
	
/********************************************************************************************
 					void FloatToWindow()
*********************************************************************************************/
int WINAPI _loadds FloatWrite(HANDLE hFile,float fNumber,int iDecimal)
{
	char  lpTxt[80];
  DWORD dwBytesWritten;
	int   iBefore, iAfter, multiplicator, position;
  int   iFirstChar	= 1;
  int   iMaxDecimal = 4;

  position = 0;

  if (iDecimal > iMaxDecimal)
   	iDecimal = iMaxDecimal;
  
  if(fNumber < 0)
 	{
   	iFirstChar= -1;
    position = 1;
    lpTxt[0]='-';
   	fNumber = fNumber * iFirstChar;
 	}

  multiplicator = (int)pow (10,iDecimal);

	iBefore = 0;
  iAfter  = 0;

	iBefore = (int)floor(fNumber);
	iAfter  = (int)floor((fNumber - iBefore)* multiplicator +0.5);

	if(iAfter == multiplicator)
	{
		iBefore = iBefore + 1;
		iAfter = 0;
	}

	if(iDecimal == 0)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d", iBefore);
	if(iDecimal == 1)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%01d", iBefore,iAfter);
	if(iDecimal == 2)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%02d", iBefore,iAfter);
	if(iDecimal == 3)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%03d", iBefore,iAfter);
	if(iDecimal == 4)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%04d", iBefore,iAfter);
	if(iDecimal == 5)
		wsprintf((LPSTR)&lpTxt[position], (LPSTR)"%d.%05d", iBefore,iAfter);

  WriteFile(
    /* hFile                  */ hFile,
    /* lpBuffer               */ lpTxt,
    /* nNumberOfBytesToWrite  */ lstrlen(lpTxt),
    /* lpNumberOfBytesWritten */ &dwBytesWritten,
    /* lpOverlapped 	        */ NULL );	

	return lstrlen(lpTxt);
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//  TOOL FUNCTIONS
//------------------------------------------------------------------------------------------------------------
long DaysBetweenTwoLongDate(long lDate1, long lDate2)
	{
    int		nYear,nYear1,nDays,nDays1,nDays2;
	long 	lDaysBetween;
	char  	cDate[11];
	
	//Simulation Begein
	LongToDate(lDate1,cDate,FALSE);
	nYear=(int)(lDate1-lDate1/100L*100L);
	nDays  =DateChange(nYear,0,cDate,2);
	nDays1 =DateChange(nYear,0,"31.12\0",2);
	
	//Sowing date
	LongToDate(lDate2,cDate,FALSE);
	nYear1  =(int)(lDate2-lDate2/100L*100L);
   	nDays2 = DateChange(nYear,0,cDate,2);

    if (nYear1==nYear) 	nDays  =nDays2-nDays;
    if (nYear1>nYear)   nDays=nDays1-nDays+nDays2;
        
	lDaysBetween = nDays+1;
	 
	return lDaysBetween;
	}
	
/*******************************************************************************
** EOF */