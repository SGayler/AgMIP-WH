/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author: ch / gsf
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 7 $
 *
 * $History: util_fct.c $
 * 
 * *****************  Version 7  *****************
 * User: Christian Bauer Date: 23.01.02   Time: 14:08
 * Updated in $/Projekte/ExpertN/ModLib/control
 * Anbindung an Datenbank auch unter Win XP möglich. Fitting in Gang
 * gesetzt.
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:01
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/control
 * "xinfowin.h" gibts nicht mehr.
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 25.12.01   Time: 10:01
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Komfortablere Fehlermeldungen.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 21:11
 * Updated in $/Projekte/ExpertN/ModLib/ModLib/control
 * Using arrays with defined length instead of malloc calls.
 * 
 * *****************  Version 2  *****************
 * User: Christian Bauer Date: 14.11.01   Time: 13:45
 * Updated in $/ExpertN/ModLib/ModLib/control
 * Compilation der Module ohne Warnings (unreferenced local variables,
 * type mismatches, etc.). Verwenden der Win32 basierter Dateioperationen
 * (z.B. CreateFile anstelle von OpenFile). hFile vom HANDLE und nicht
 * mehr vom Typ HFILE.
 *
 *   dm, 11.4.96   Implementierung der neuen Variablenstruktur
 * 
*******************************************************************************/

#include <crtdbg.h>
#include <stdio.h>
#include "trace.h"
#include "xinclexp.h"
#include "xlanguag.h"
#include "infowin.h"

// aus System
extern int WINAPI _loadds Logout(float, LPSTR); // System Function

extern  PTIME WINAPI GetTimePoi(void);         

// Funktionsdeklaration
int introduce(LPSTR);

float  abspower(float, float);
double roundDBL(double, short int);
double abspowerDBL(double, double);
float  signum(float,float);

int   WINAPI Message(long, LPSTR);
int   WINAPI Set_At_Least_Epsilon(float *x, LPSTR c);
int   WINAPI Test_Range(float x, float x1, float x2, LPSTR c);
float WINAPI Polygon4(float, float, float,float,float ,float ,float ,float ,float);
float WINAPI RelAnteil(float, float);

int DateToString(long lDate,LPSTR lpDate);


//-----------------------------------------------------------------------------
int WINAPI Message( long i_lErrCode, LPSTR i_szMessage )
/* displays comments, warnings errors

   errcode  type             action
   ------------------------------------------
   0        Hinweis          Log-File
   1        Hinweis/Warnung  Log-File, Sim.-Fenster
   2        Fehler           Log-File, Sim.-Fenster, Message-Box
   3        Schwerer Fehler  Log-File, Sim.-Fenster, Message-Box, Sim.-Abbruch
-----------------------------------------------------------------------------*/
{
  PTIME   pTi = GetTimePoi();
  float   SimTime = pTi->pSimTime->fTimeAct;
  char    lpOut[100];
  int     iMessageStrLen = strlen(i_szMessage);

  if( i_szMessage == NULL )
  {
    return -1;
  }

  DateToString( pTi->pSimTime->lTimeDate, lpOut );
   
  switch( i_lErrCode )
  {
    case 0:
      strcat(lpOut, "    ");
      if (iMessageStrLen >= (int)(sizeof(lpOut)-strlen(lpOut)) )
      {
        iMessageStrLen = sizeof(lpOut)-strlen(lpOut);
      }
      strncat( lpOut, i_szMessage, iMessageStrLen );
      Logout(SimTime,lpOut);
      break;
    case 1:
      strcat(lpOut, "    ");
      if (iMessageStrLen >= (int)(sizeof(lpOut)-strlen(lpOut)) )
      {
        iMessageStrLen = sizeof(lpOut)-strlen(lpOut);
      }
      strncat( lpOut, i_szMessage, iMessageStrLen );
      ExpScrLogout(lpOut);
      Logout(SimTime,lpOut);
      break;
    case 2:
      MessageBox (NULL, i_szMessage, ERROR_TXT, MB_OK);
      strcat(lpOut, " !   ");
      if (iMessageStrLen >= (int)(sizeof(lpOut)-strlen(lpOut)) )
      {
        iMessageStrLen = sizeof(lpOut)-strlen(lpOut);
      }
      strncat( lpOut, i_szMessage, iMessageStrLen );
      ExpScrLogout(lpOut);
      Logout(SimTime,lpOut);
      break;
    case 3:
      MessageBox (NULL, i_szMessage, ABORT_SIM_TXT, MB_OK); 
      strcat(lpOut, " !!   ");
      if (iMessageStrLen >= (int)(sizeof(lpOut)-strlen(lpOut)) )
      {
        iMessageStrLen = sizeof(lpOut)-strlen(lpOut);
      }
      strncat( lpOut, i_szMessage, iMessageStrLen );
      ExpScrLogout(lpOut);
      Logout(SimTime,lpOut);

      /* EXIT Anweisung */
      pTi->pSimTime->fTimeAct = (float)9999; // => Abort Simulation.
      break;
  }    

  return 1;

} // Message
                       

//-----------------------------------------------------------------------------
int DateToString( long lDate, LPSTR szDate )
// Converts date ttmmyy in "tt.mm.yy"
//-----------------------------------------------------------------------------
{
  char szTemp[20];

  _ASSERTE( szDate != NULL );

  szDate[0] = '\0';

  ltoa( lDate, szTemp, 10 );
  
  if( lDate >= 100000 )
  {
    strncat( szDate, &szTemp[0], 2);
    strcat ( szDate, "/" );
    strncat( szDate, &szTemp[2], 2);
    strcat ( szDate, "/" );
    strncat( szDate, &szTemp[4], 2);
  }
  else if( lDate >= 10000 )  
  {
    strcat ( szDate, "0" );
    strncat( szDate, &szTemp[0], 1);
    strcat ( szDate, "/" );
    strncat( szDate, &szTemp[1], 2);
    strcat ( szDate, "/" );
    strncat( szDate, &szTemp[3], 2);
  }
  else if (lDate == 0)
  {
    strcpy( szDate, "00/00/00" );
  }
  else
  {
    strcpy( szDate, "??/??/??" );
  }
    
  return 1;
}

//-----------------------------------------------------------------------------
double  roundDBL(double x, short int i)
//-----------------------------------------------------------------------------
{
 double z1,z2,z3=0.0,z4=1.0,z5=1.0,sign = 1.0;
 short int i1;

  if (x < 0.00)
  {
    sign = -1.0;
    x   *= -1.0;
  }
  for (i1=1;i1<=i;i1++)
  {
    z4 *= 10.0;
  }
  for (i1=1;i1<=i;i1++)
  {
    z5 /= 10.0;
  }

  z1 = x*z4;
  z2 =(((double)(long int)(x*z4)) +0.5);

  if (z1>= z2)
  { z3 = ((double)((long int)z2 +1) * z5);
  }
  else
  {
    z3 = ((double)((long int)z2) *z5);
  }

  return (sign * z3);

};

//-----------------------------------------------------------------------------
float  abspower(float x, float y)
//-----------------------------------------------------------------------------
{
 float sign = (float)1.0,z=(float)0.0;
 if (x < (float)0.0)
 {
   sign = (float)-1.0;
   x *= (float)-1.0;
 }

 if (y < (float)0.0)
 {
   if (x != (float)0.0)
   x = (float)1.0/x;
   y *= (float)-1.0;
 }

 z = (float)pow((double)x,(double)y);
 return  z;
}


//-----------------------------------------------------------------------------
double  abspowerDBL(double x, double y)
//-----------------------------------------------------------------------------
{
  double sign,z;
  if (x < 0.0)
  {
    sign = -1.0;
    x  *= -1.0;
  }

  if (y < 0.0)
  {
    y *= -1.0;
    if (x != (double) 0.0)
    {
      x = 1.0/x;
    }
  }
  z = (double)pow((double)x,(double)y);
  return z;
}
    
float  signum(float x, float y)
//-----------------------------------------------------------------------------
{
 float sign = (float)1.0, z=(float)0.0;
 
 if (y < (float)0.0) sign = (float)-1.0;
 if (x < (float)0.0) z = x *(float)-1.0;

 z= sign*x;

 return  z;
}

//-----------------------------------------------------------------------------
int WINAPI Set_At_Least_Epsilon(float *x, LPSTR c)
// Setzt x auf EPSILON falls x<EPSILON und gibt Meldung aus
//-----------------------------------------------------------------------------
{
  char  acdummy[80];
     
  if (*x < EPSILON)
  {
    *x = EPSILON;
    strcpy(acdummy, ERROR_NOT_POSITIVE_TXT);
    strcat(acdummy, c);
    Message(0,acdummy);

    return 0;
  }
    
  return 1;
}


//-----------------------------------------------------------------------------
int WINAPI Test_Range(float x, float x1, float x2, LPSTR c) /*
Return Value:
  0 .. Variable ausserhalb des Bereichs
  1 .. Variable plausibel.
-----------------------------------------------------------------------------*/
{
  static char incorrect[10][80] = { { 0 } };
  int    i;

  if( (x < x1) || (x > x2) )
  { 
    for( i = 0; (i < 10) && (strlen(incorrect[i]) > 1); i++ )
    {
      if( strcmp(incorrect[i],c) == 0 )
      {
        return 0;
      }
    }
  
    if (i < 10)
    {
      wsprintf(incorrect[i], c);
    }
  
    #if 0
    {
      char szBuf[100];
      sprintf( szBuf, "Val = %e, Min = %e, Max = %e", x, x1, x2 );
      M_TRACE_MESSAGEBOX(RANGE_ERROR_TXT,c,szBuf,NULL);
    }
    #endif

    return 0;
  }
  return 1;
}
   
//-----------------------------------------------------------------------------
float WINAPI RelAnteil(float x, float y) /*
Von zwei echt positiven Zahlen x, y wird der rel. Anteil von x an der
Summe x+y berechnet.
-----------------------------------------------------------------------------*/
{
  float z;
  z = ((x > EPSILON)&&(y > EPSILON))?
      x / (x + y)
    : (float)0;
  return z;
}

    
//-----------------------------------------------------------------------------
float WINAPI Polygon4(float x, float x0, float y0, float x1,float y1,float x2,float y2,float x3,float y3)
// Polygonzug mit 4 Eckpunkten als Korrekturfunktion für Feuchte und Temperatur.
//-----------------------------------------------------------------------------
{
  float y = (float)-1;

  y = (x >= x3 ) ?
      y3
   : (x <= x0 ) ?
       y0
     : ((x >= x2 )&&(x3-x2 > EPSILON)) ?
         y2 + (y3 - y2) * (x - x2)/(x3-x2)
       : ((x >= x1 )&&(x2-x1 > EPSILON)) ?
           y1 + (y2 - y1) * (x - x1)/(x2-x1)
         : ((x >= x0 )&&(x1-x0 > EPSILON)) ?
             y0 + (y1 - y0) * (x - x0)/(x1-x0)
           : ((x >= x2 )&&(x3 >= x2)) ?
               y2
             : ((x >= x1 )&&(x2 >= x1)) ?
                 y1
               : ((x >= x0 )&&(x1 >= x0)) ?
                   y0 
                 : (float)-99;

  if (y == -99)
  {
    static BOOL  s_bErrorReported = FALSE;
    if( !s_bErrorReported )
    {
      char   szNumVal[50];
      sprintf( szNumVal, "%e", x );

      M_TRACE_MESSAGEBOX( "Polygon4()", "value out of range", szNumVal, NULL );
      s_bErrorReported = TRUE;
    }
    return y0;
  } 

  return y;
}


//-----------------------------------------------------------------------------
int introduce(LPSTR lpName)   
// Schreibt Inhalt von lpName in Logout-File
//-----------------------------------------------------------------------------
{                             
  char   acdummy[80];
          
  strncpy( acdummy, lpName, 60 );
  strcat( acdummy, COMMENT_INCLUDED_TXT );
    
  Message(0,acdummy);
       
  return 1; 
}
                       
/*******************************************************************************
** EOF */
