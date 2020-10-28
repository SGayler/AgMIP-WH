/*******************************************************************************
 *
 * Copyright  (c) by 
 *
 * Author:      C.Sperr
 * Definitions: M.Baldioli,T.Engel,C. Haberbosch,E.Prieack,T.Schaaf,E.Wang
 *
 *------------------------------------------------------------------------------
 *
 * Description:
 *
 *   Simulation time objects
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 4 $
 *
 * $History: xgraphic.h $
 * 
 * *****************  Version 4  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 12:07
 * Updated in $/Projekte/ExpertN.3.0/ModLib/ModLib/include
 * Doppelte Definition der Strukuren bei mehrfachem Includieren der
 * Headerdatei vermeiden.
 * 
 *   Date:  27.11.95
 *   letzte Aenderung 01.12.96
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_XGRAPHIC_H
#define _INC_EXPERTN_XGRAPHIC_H

/*==============================================================================
Public Type Definitions, Constants, Macros
==============================================================================*/

#define MAX_LABEL_STRING     80 

typedef struct graphic  *   PGRAPHIC;

typedef struct graphic  {

	int    bInit	      ; //
	int	bRedraw       ; // int	   Graphik neuzeichnen, falls TRUE; wird danach automatisch rückgesetzt
	
	int    iDllGraphicNum;	// dm, 2.5.96
	int    iOutputFileNum;	// dm, 2.5.96
	
 	double dPlotDZ1XMax  ; // double  Graphik - Skala Maximum  ------
	double dPlotDZ1XMin  ; // double  Graphik - Skala Minimum  ------
	int	iPlotDZ1Xdec  ; // int	   Anzahl der Nachkommastellen in der Achsenbeschriftung
	double dPlotDZ1Xdiv  ; // double  Anzahl der Rasterlinien  ------
	float	fPlotDZ1[100]  ; // float   Tiefenabhängige Graphik  Meßwert - Array	    -----
 	
 	double dPlotDZ2XMax  ; // double  Tiefenabhängige Graphik 2  Maximalwert der Skala
	double dPlotDZ2XMin  ; // double  Tiefenabhängige Graphik 2   Minimalwert der Skal
	int	iPlotDZ2Xdec  ; // int	   Tiefenabhängige Graphik 2	    ------
	double dPlotDZ2Xdiv  ; // double  Tiefenabhängige Graphik 2	    ------
	float	fPlotDZ2 [100]; // float   Tiefenabhängige Graphik  Meßwert - Array
  	
 	double dPlotDT1YMax  ; // double  Zeitabhängige Graphik 1  Maximalwert der Skala
	double dPlotDT1YMin  ; // double  Zeitabhängige Graphik 1  Minimalwert der Skala
	int	iPlotDT1Ydec  ; // int	   Zeitabhängige Graphik 1  Skaleneinteilungen
	double dPlotDT1Ydiv  ; // double  Zeitabhängige Graphik 1  Skaleneinteilungen
	float	fPlotDT11     ; // float   Graphik 1 Kurve 1	    ------
	float	fPlotDT12     ; // float   Graphik 1 Kurve 2	    ------
	float	fPlotDT13     ; // float   Graphik 1 Kurve 3	    ------
	float	fPlotDT14     ; // float   Graphik 1 Kurve 4	    ------
	float	fPlotDT15     ; // float   Graphik 1 Kurve 5	    ------
	float	fPlotDT16     ; // float   Graphik 1 Kurve 6	    ------
 	
 	double dPlotDT2YMax  ; // double  Zeitabhängige Graphik 2  Maximalwert der Skala
	double dPlotDT2YMin  ; // double  Zeitabhängige Graphik 2  Minimalwert der Skala
	int	iPlotDT2Ydec  ; // int	   Zeitabhängige Graphik 2  Skaleneinteilungen
	double dPlotDT2Ydiv  ; // double  Zeitabhängige Graphik 2  Skaleneinteilungen
	float	fPlotDT21     ; // float   Graphik 2 Kurve 1	    ------
	float	fPlotDT22     ; // float   Graphik 2 Kurve 2	    ------
	float	fPlotDT23     ; // float   Graphik 2 Kurve 3	    ------
	float	fPlotDT24     ; // float   Graphik 2 Kurve 4	    ------
	float	fPlotDT25     ; // float   Graphik 2 Kurve 5	    ------
	float	fPlotDT26     ; // float   Graphik 2 Kurve 6	    ------

	char	acPlotDZ1Titel[MAX_LABEL_STRING]; // char    Titel der tiefenabhängigen Graphik 1     ------
	char	acPlotZ1Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik Z1 

	char	acPlotDZ2Titel[MAX_LABEL_STRING]; // char    Titel der tiefenabhängigen Graphik 2     ------	
	char	acPlotZ2Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik Z2

	char	acDT1Titel[MAX_LABEL_STRING]    ; // char    Titel für zeitabhängigen Graphik 1	    ------
	char	acPlot11Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 1 Kurve 1   ------
	char	acPlot12Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 1 Kurve 2   ------
	char	acPlot13Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 1 Kurve 3   ------
	char	acPlot14Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 1 Kurve 4   ------
	char	acPlot15Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 1 Kurve 5   ------
	char	acPlot16Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 1 Kurve 6   ------

	char	acDT2Titel[MAX_LABEL_STRING]    ; // char    Titel für zeitabhängigen Graphik 2	    ----
	char	acPlot21Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 2 Kurve 1   ------
	char	acPlot22Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 2 Kurve 2   ------
	char	acPlot23Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 2 Kurve 3   ------
	char	acPlot24Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 2 Kurve 4   ------
	char	acPlot25Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 2 Kurve 5   ------
	char	acPlot26Text[MAX_LABEL_STRING]  ; // char    Beschriftung Graphik 2 Kurve 6   ------

}  GRAPHIC;

#endif // #ifndef _INC_EXPERTN_XGRAPHIC_H

/*******************************************************************************
** EOF */