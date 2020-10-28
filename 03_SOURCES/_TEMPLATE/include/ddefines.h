/*******************************************************************************
 *
 * Copyright (c) by 
 *
 * Author:  C. Sperr
 *
 *------------------------------------------------------------------------------
 *
 * Description: 
 *
 *   Symbolic expressions for project Expert-N.
 *
 *------------------------------------------------------------------------------
 *
 * $Revision: 8 $
 *
 * $History: defines.h $
 * 
 * *****************  Version 8  *****************
 * User: Christian Bauer Date: 23.01.02   Time: 14:03
 * Updated in $/Projekte/ExpertN/ExpertN/Include
 * 
 * *****************  Version 7  *****************
 * User: Christian Bauer Date: 30.12.01   Time: 14:54
 * Updated in $/Projekte/ExpertN.3.0/ExpertN/Include
 * Icon IDs nach resource.h verschoben
 * 
 * *****************  Version 6  *****************
 * User: Christian Bauer Date: 29.12.01   Time: 15:38
 * Updated in $/Projekte/ExpertN.3.0/ExpertN/Include
 * INT_PTR ist in Visual C 4.2 nicht bekannt.
 * 
 * *****************  Version 5  *****************
 * User: Christian Bauer Date: 25.12.01   Time: 9:26
 * Updated in $/Projekte/ExpertN/ExpertN/Include
 * MenüPunkt "ResultFiles->Carbon" ergänzt.
 * 
 * *****************  Version 3  *****************
 * User: Christian Bauer Date: 14.12.01   Time: 16:41
 * Updated in $/Projekte/ExpertN/ExpertN/Include
 * #defines for MODEL_ID added
 * 
 *   13.02.94
 *
*******************************************************************************/

#ifndef _INC_EXPERTN_DEFINES_H
#define _INC_EXPERTN_DEFINES_H

#define _MALLOC(_size) _malloc_dbg(_size,_NORMAL_BLOCK,__FILE__,__LINE__)

#if _MSC_VER > 1020
#else
  #define INT_PTR BOOL
#endif

#define kPaTOmm (1000./9.80665)
#define PaTOmm (1./9.80665)
/// 11.95 >>>>>>>>>>>>>>>>
#define MAX_STRINGLEN  25  // stringlength of names from file  
#define MAX_FILE_STRINGLEN  30  // stringlength of names from file 
#define MAX_DEPTH_PER_MEASURE 10 //maximal number of start value horizons
#define MAX_VARIETYINFO  10
#define MAX_DEV_STAGES  10
#define MAX_GEN_PARAM  5 

#define MAX_LABEL_STRING     80   // xgraphic.h

//// <<<<<<<<<<<<<<< 11.95
 
#define WHITE_CURVE 99
/*
 *   Simulation Data Limits
 */

#define FRUCHT_ANZAHL         3
#define MAXSCHICHT           300//100
#define MAX_SIM_DAYS         73050//36525//10960
#define MAXREGENTAGE         MAX_SIM_DAYS
#define MAXTEMPERATURTAGE    MAX_SIM_DAYS
#define MAXDUENGER           100
#define MAXPANTAGE           MAX_SIM_DAYS
#define MAXOBERFLAECHEN       1
#define MAXPFLANZEN           5
#define MAXUMBRUCH           10


/*
 *    plot list member : plotlist.c
 */
#define INIT_PLOT_TITLE  "New Plot"
#define MAX_PLOTPAR_MEMBER     30

/*
 *  User Message parameter for Graphic dialog handling
 * note : this parameter must not be lower zero
 * LPARAM : signed long <==> long : unsigned
 */
#define USER_PLOT_LPARAM     9000

/*
 *  configuration dialog - system
 *    Used by CONF.RC/ 04.01.94
 */
#define COMBO_WIDTH          80
#define COMBO_ONE            20
#define COMBO_TWO            30
#define COMBO_THREE          40
#define COMBO_FOUR           50
#define COMBO_FIVE           60
#define COMBO_SIX            70
#define COMBO_SEVEN          80
#define COMBO_EIGHT          90
#define COMBO_TEN           110
#define COMBO_TWENTY        210

#define COMBO_MAX_FUNCTION_GROUPS           310

#define DLG_CFG_X0            50
#define DLG_CFG_Y0            30

#define DLG_CFG_0_DX         300
#define DLG_CFG_0_DY         228

#define DLG_CFG_X1         DLG_CFG_X0
#define DLG_CFG_Y1         105

#define DLG_CFG_X2         DLG_CFG_X0
#define DLG_CFG_Y2         DLG_CFG_Y0

#define CFG_0_FONT   8, "Helv"

#define EXPERTN_MODEL_ID_LEACHN        0
#define EXPERTN_MODEL_ID_NSIM          1
#define EXPERTN_MODEL_ID_USERDEFINED   2

typedef enum {
  /*  0 */  ID_INPUT
  /*  1 */ ,ID_INIT
  /*  2 */ ,ID_TIME
  /*  3 */ ,ID_INTERCEPT
  /*  4 */ ,ID_RUNOFF  
  /*  5 */ ,ID_POT_EV  
  /*  6 */ ,ID_ACT_EV  
  /*  7 */ ,ID_POT_TR  
  /*  8 */ ,ID_ACT_TR  
  /*  9 */ ,ID_PHENOL  
  /* 10 */ ,ID_PHOTOSY 
  /* 11 */ ,ID_BIOMASS 
  /* 12 */ ,ID_LEAF    
  /* 13 */ ,ID_ROOT    
  /* 14 */ ,ID_GROUND  
  /* 15 */ ,ID_FLOW_H2O
  /* 16 */ ,ID_SOILTEMP
  /* 17 */ ,ID_N_DEPOS 
  /* 18 */ ,ID_NFLOW   
  /* 19 */ ,ID_N_HYDRO 
  /* 20 */ ,ID_N_MINER 
  /* 21 */ ,ID_N_NITRI 
  /* 22 */ ,ID_PNDEMAND
  /* 23 */ ,ID_PNUPTAKE
  /* 24 */ ,ID_PSENESC 
  /* 25 */ ,ID_N_DENIT 
  /* 26 */ ,ID_N_VOLAT 
  /* 27 */ ,ID_CH4EMISS
  /* 28 */ ,ID_BALANCE 
  /* 29 */ ,ID_FITTING 
  /* 30 */ ,ID_HYDRAULIC        
  /* 31 */ ,ID_PDGL
  /* 32 */ ,ID_PDGL_N
  /* 33 */ ,ID_PDGL_SOILTEMP
  /* 34 */ ,ID_PDGL_H2O
  /* 35 */ ,ID_POTEVTR 
  /* 36 */ ,ID_SNOW    
  /* 37 */ ,ID_SOILLOOS
  /* 38 */ ,ID_SOILMIX 
  /* 39 */ ,ID_FERT    
  /* 40 */ ,ID_FERTLOSS
  /* 41 */ ,ID_IRRIG   
  /* 42 */ ,ID_IRRIGLOSS
  /* 43 */ ,ID_PEDOTRANS
  /* 44 */ ,ID_OUTPUT
  /* 45 */ ,ID_MODEL
  /* 46 */ ,MAX_FUNCTION_GROUPS 
  /* 47 */ ,ID_UNDEFINED
} ID_FUNCTION_GROUP;

/* group reference  switches used in exp_dlg.c  to append any new dll - functions  */
/* assigned to the load-dll-proc dialogbox group - combolist                       */
#define MAXGROUP                     20
#define MAXGROUPNAMELENGTH           40
#define MAXGROUPMEMBERNAMELENGTH     40
#define MAX_DLL_FCT_LIST             46
#define MAX_FUNCTIONS_PER_GROUP      11

#define PRINT_FUNCTION_GROUPS        19
#define MAX_GROUP_NAME_LENGTH        40
#define MAX_FUNCTION_NAME_LENGTH     40
#define ID_CHANGED                   30


#define IDC_RADIO1                      1000
#define IDC_RADIO2                      1001
#define IDC_RADIO3                      1002
#define IDAPPEND                        1004
#define ID_FITTBTN                      1005
#define ID_INITBTN                      1006
#define ID_INPUTBTN                     1007
#define IDC_BUTTON1                     1090
#define IDC_BUTTON2                     1091
#define IDC_BUTTON3                     1092
#define IDC_BUTTON4                     1093
#define IDC_BUTTON5                     1094
#define IDC_EDIT1                       1010
#define IDC_EDIT2                       1011
#define IDC_EDIT3                       1012
#define IDC_EDIT4                       1013
#define IDC_EDIT5                       1014
#define IDC_EDIT6                       1015
#define IDC_EDIT7                       1016
#define IDC_EDIT8                       1017
#define IDC_EDIT9                       1018
#define IDC_EDIT10                      1019
#define IDC_EDIT11                      1020
#define IDC_EDIT12                      1021
#define IDC_EDIT13                      1022
#define IDC_EDIT14                      1023
#define IDC_EDIT15                      1024
#define IDC_EDIT16                      1025
#define IDC_TXT1                        1030
#define IDC_TXT2                        1031
#define IDC_TXT3                        1032
#define IDC_TXT4                        1033
#define IDC_TXT5                        1034
#define IDC_TXT6                        1035
#define IDC_TXT8                        1036
#define IDC_TXT9                        1037
#define IDC_TXT10                       1038
#define IDC_TXT12                       1039
#define IDC_TXT13                       1040
#define IDC_TXT14                       1041
#define ID_DEFAULTBOX                   1100
#define IDC_COMBO1                      2001
#define IDC_COMBO2                      2002
#define IDC_COMBO3                      2003
#define IDC_COMBO4                      2004
#define IDC_COMBO5                      2005
#define IDC_COMBO6                      2006
#define IDC_COMBO7                      2007
#define IDC_COMBO8                      2008
#define IDC_STATIC                      -1

/*--------------<constants>---------------------*/
#define  PI                        3.141593

#define SIMULATION                110

/*  expert-n configuration dialogbox identifier : exp_dlg.c */
#define AUSWAHLDLG                300
#define ID_ZEITCOMBO              301
#define ID_WACHSTUMCOMBO          302
#define ID_EVAPOTCOMBO            303
#define ID_EVAAKTCOMBO            304
#define ID_TRANSPOTCOMBO          305
#define ID_TRANSAKTCOMBO          306
#define ID_PFLANZENCOMBO          307
#define ID_WASSERFLUSSCOMBO       308
#define ID_HYDRAULIKCOMBO         309
#define ID_LEACH                  310
#define ID_N_SIM                  311
#define ID_BODENTEMPCOMBO         312
#define ID_PFL_N_COMBO            313
#define ID_N_FLUSS_COMBO          314
#define ID_N_UMWANDL_COMBO        315
#define ID_PDGLCOMBO              316
#define ID_ALIGN_FCTCOMBO         317
/*
 **********************************
 */ 
#define ID_HELP                   318
/* 
 **********************************
 */ 

/* expert-n main menu items */
 

#define IDM_DATEI                   1000
#define IDM_D_DEF                   1001
#define IDM_D_AENDERN               1002
#define IDM_D_LOAD_DLL              1003
#define IDM_D_NAME_DLL              1004
#define IDM_D_LOAD_PROC             1005

#define IDM_D_OFFNEN                1050
#define IDM_D_SPEICHERN             1100
#define IDM_D_SPEICHERNUNTER        1150       
#define IDM_D_PRJLOAD               1151
#define IDM_D_PRJEDIT               1152
#define IDM_D_PRJSAVE               1153
#define IDM_D_PRJSAVEAS             1154

#define IDM_D_PRN_INST              1160
#define IDM_D_BEENDEN               1250
#define IDM_SIMULATION              2100
#define IDM_SAVE_STAT               2101
#define IDM_LOAD_STAT               2102
#define IDM_SHOW_STAT               2103
#define IDM_START                   3000
#define IDM_STOP                    3500
#define IDM_PAUSE                   4000
#define IDM_F_DSSAT                 4500
#define IDM_F_WATER                 4501
#define IDM_F_HEAT                  4502
#define IDM_F_NITRO                 4503
#define IDM_F_PLANT                 4504
#define IDM_F_BALAN                 4505
#define IDM_F_CARBON                4506
#define IDM_CFG_PLANT               4507
#define IDM_CFG_WATER               4508
#define IDM_CFG_HEAT                4509
#define IDM_CFG_NITRO               4510
#define IDM_CFG_MANAGEMENT          4511
#define IDM_CFG_CONTROL             4512
#define IDM_CFG_COMPLETE            4513

#define IDM_GRAPHIK                 5000
#define IDM_G_ACT_PLOTS             5001   
#define IDM_G_CLOSE_PLOTS           5002  
#define IDM_G_THZ                   5050
#define IDM_G_THT                   5100
#define IDM_G_MEAS_H2O              5101
#define IDM_G_THP                   5150
#define IDM_G_THPLOG                5151
#define IDM_G_DWC                   5152
#define IDM_G_NDT                   5200
#define IDM_G_ADT                   5250
#define IDM_G_EDT                   5300
#define   IDM_G_H2OET               5301
#define   IDM_G_H2OBAL              5302
#define   IDM_G_H2OPOT              5303
#define   IDM_G_H2OFLUX             5304
#define   IDM_G_H2OCOND             5305

#define IDM_G_TEMPHZ                5400
#define IDM_G_TEMPHT                5410
#define   IDM_G_TEMPICE            5411

#define IDM_G_DEVSTAGE              5451
#define IDM_G_BIOMASS               5452
#define IDM_G_LAI                   5453
#define IDM_G_TILLERS               5454
#define IDM_G_ROOTLENGTH            5455
#define IDM_G_TRANSP                5456
#define IDM_G_NUPTAKE               5457
#define IDM_G_NCONC                 5458
#define   IDM_G_ROOTLENGTHDT        5459

#define IDM_G_NITRODZ               5600
#define IDM_G_NITRODT               5610
#define IDM_G_MEAS_NO3              5611
#define IDM_G_NITROTR               5620
#define   IDM_G_NITROCUM            5630
#define   IDM_G_NITROBAL            5631
#define   IDM_G_NITROGAS            5632
#define   IDM_G_NITROCUMGAS         5633
#define   IDM_G_NITROPOOLS          5634
#define   IDM_G_NITROCN             5635
#define   IDM_G_NITROMINER          5636

#define IDM_G_DUMMYDT1               5700
#define IDM_G_DUMMYDZ1               5710
#define IDM_G_DUMMYDT2               5720
#define IDM_G_DUMMYDZ2               5730



#define IDM_HILFE                 6000
#define IDM_H_INDEX                    6050
#define IDM_H_UEBEREXPERTN             6150
#define IDM_H_TEST                   6151
#define IDM_H_ZOOMPLOT                 6250

/*  dialog ID s    */
#define ID_CONF_DLG                     100
#define ID_CONF_TIME_DLG                101
#define ID_CONF_WATER_DLG               102
#define ID_CONF_HYD_DLG                 103
#define ID_CONF_PDGL_DLG                104
#define ID_CONF_NITR_DLG                105
#define ID_CONF_NLEACH_DLG              106
#define ID_CONF_NFERT_DLG               107
#define IDD_DIALOG7                     109
#define ID_CONF_PLANT_DLG               110
#define ID_CONF_TEMP_DLG                111
#define IDD_DIALOG1                     112
#define IDD_DIALOG2                     113
#define ID_CTRL_DLG1                    114
#define ID_CTRL_DLG2                    115
#define ID_CTRL_DLG3                    116
#define ID_CTRL_DLG4                    117
#define ID_CTRL_DLG5                    118
#define ID_CTRL_DLG6                    119
#define ID_CTRL_DLG7                    120
#define ID_CTRL_DLG8                    121
#define ID_PRINT                        122 /* WM_Command flag in call back units */
#define ID_Y_SCALE                      123 /* WM_Command flag in call back units */
#define ID_X_SCALE                      124 /* WM_Command flag in call back units */
#define CONTROLNR                       125 /* info dialog to show time and simulation status */
#define ID_CONF_FITTING_DLG        126
#define ID_PRJ_DLG            127
#define ID_CONF_INIT_DLG        128
#define ID_CONF_INPUT_DLG        129

#define ID_DBPRJ_LOAD_DLG        130

////#define FIRSTINFODLG                    124 /* info dialog */
#define PLOTNITRODT                      0
#define PLOTNITRODZ                      1
#define PLOTNITROTR                      2
#define PLOTNITROBAL                 22
#define PLOTDUMMYDT1                     3
#define PLOTDUMMYDZ1                     4
#define PLOTDUMMYDT2                 23
#define PLOTDUMMYDZ2                 24
#define PLOT22                           5
#define PLOT33                           6
#define PLOT44                           7
#define PLOT55                           8
#define PLOT66                           9
#define PLOT77                          10
#define PLOTMSG                         12
#define PLOTSOILMSG                     13
#define PLOTSOILTEMP                    14
#define PLOTLOGHYD                      18
#define PLOTLOGHYDLEIT                  19
#define PLOTLOGHYDLEITPOT               20
#define PLOTDWC                         21
#define PLOTDEVSTAGE                    25
#define PLOTBIOMASS                     26
#define PLOTLAI                         27
#define PLOTTILLERS                     28
#define PLOTROOTLENGTH                  29
#define PLOTTRANSP                      30
#define PLOTNUPTAKE                     31
#define PLOTNCONC                       32
#define PLOTMEASH2O                     33
#define PLOTMEASNO3                     34

#define   PLOTH2OET                     35
#define   PLOTH2OBAL                    36
#define   PLOTH2OPOT                    37
#define   PLOTH2OFLUX                   38
#define   PLOTH2OCOND                   39
#define   PLOTTEMP_ICE                  40
#define   PLOTNITROCUM                  41
#define   PLOTNITROGAS                  42
#define   PLOTNITROCUMGAS               43
#define   PLOTNITROPOOLS                44
#define   PLOTNITROCN                   45
#define   PLOTNITROMINER                46

#define   PLOTROOTLENGTHDT              47

/*  plot dialog position - coordinates x,y,dx,dy on the screen   */
#define PLOT_XL_POS                   0
#define PLOT_YL_POS                  50
#define PLOT_XR_POS                  225
#define PLOT_YR_POS                  50
#define PLOT_WIDTH                   220
#define PLOT_HEIGHT                  300



#define IDS_ERR_REGISTER_CLASS   1
#define IDS_ERR_CREATE_WINDOW    2


#define MAXLINES                       500 // Max. Anzahl Textzeilen
#define MAXLINELENGTH                   90 // Max. Zeilenl"nge
#define MAXDTARRAY                     200
#define MAXDTARRAYMEMBER               900
#define MAXMETHODNR                     20
#define MAXDZARRAY                     200
#define MAXDOUBARRAY                    10

#define MAXPLOTS              48  /// cs  10.10.97 40

#define MAXPROCNAMELENGTH     80

#define DLLPROCDLG                  200
#define DLL_PROC_DLG                 201
#define PROCNAME                     202

/*-------------------------<device contexts>------------------------------*/
#define    MAX_PENS        15
#define    MAX_S_BRUSHES        15
#define    MAX_B_S_PENS         10
#define    MAX_GRAF_PENS        16
#define    MAX_EXP_FONTS        11

/*-------------------------<define different colors-----------------------*/
#define PC_RED    RGB(255,0,0)
#define PC_LTRED  RGB(120,0,0)
#define PC_GREEN  RGB(0,255,0)
#define PC_LTGREEN  RGB(0,120,0)
#define PC_BLUE   RGB(0,0,255)
#define PC_YELLOW RGB(255,255,0)
#define PC_LTBLUE RGB(0,255,255)
#define PC_VIOLET RGB(255,0,255)
#define PC_WHITE  RGB(255,255,255)
#define PC_BLACK  RGB(0,0,0)
#define PC_COL_8  RGB(100,50,0)
#define PC_COL_9  RGB(50,100,0)
#define PC_COL_10 RGB(0,50,100)
#define PC_COL_11 RGB(150,150,0)
#define PC_COL_12 RGB(150,0,150)
#define PC_COL_13 RGB(0,150,150)
#define PC_COL_14 RGB(110,0,0)
#define PC_COL_15 RGB(130,70,250)
#define PC_COL_16 RGB(110,110,110)


/*-------------------------<No of error>------------------------------*/

#define  NO_FILE         0 
#define  NO_READ         1 
#define  NO_WRITE        2 
#define  NO_READ_DATA    3 
#define  NO_WRITE_DATA   4
#define  NO_MEMORY       5
#define  NO_IMPLEMENT    6
#define  NO_CALL         7

#endif // #ifndef _INC_EXPERTN_DEFINES_H

/******************************************************************************
** EOF */
