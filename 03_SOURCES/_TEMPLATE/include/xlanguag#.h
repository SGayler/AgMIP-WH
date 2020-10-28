/**********************************************************************************
 *
 *         E X P E R T - N
 *
 * Name			: xlanguag.h
 * Autor		: Christian Haberbosch
 * Beschreibung	: English and german language defines
 * Stand		: 25.11.97
 ***********************************************************************************/

#define ENGLISH    
//#undef   GERMAN   


#ifdef   ENGLISH
#undef   GERMAN   

/**********************************************************************************************
 * content  :   English Version EXPERT-N  DLL
 * date		:   25.11.97
 * author   :   Ch.Haberbosch 
 ********************************************************************************************
 */


#define TITLE_LOG_1   "   Simtime\tMessage                   "
/*********************************************************************************************
   Outputfiles
   WATER-Definitions
 ********************************************************************************************/

#define TITLE_WATER_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t   (7)\t    (8)\t    (9)\t    (10)\t    (11)\t    (12)\t   (13)\t   (14)\t   (15)\t   (16)\t   (17)\t   (18)\t   (19)\t   (20)\t   (21)\t   (22)\t   (23)\t   (24)\t (25)\t\t (26)\t\t   (27)"
//#define TITLE_WATER_LINE_2 "  Date \t c_Prec\t c_Inf\t c_Run\t c_pET\t c_pEvp\t c_aEvp\t c_pTrs\t c_aTrs\t c_Icpt\t c_Drn\t WStor\t  WC_30\t WC_60\t WC_90\t WC_120\t pnd_H2O\t d_Prec\t d_Inf\t d_Run\t d_pET\t  d_pEvp\t  d_aEvp\t d_pTrs\t d_aTrs\t d_Icpt\t d_Drn"
//SG20140918 für Daisy:
#define TITLE_WATER_LINE_2 "  Date \t c_Prec\t c_Inf\t c_Run\t c_pET\t c_pEvp\t c_aEvp\t c_pTrs\t c_aTrs\t c_Icpt\t c_Drn\t WStor\t  WC_30\t WC_60\t WC_90\t WC_120\t pnd_H2O\t d_Prec\t d_Inf\t d_Run\t d_aET\t  d_pEvp\t  d_aEvp\t d_pTrs\t d_aTrs\t d_Icpt\t d_Drn"
                  
#define TITLE_WATER_1   "(1)  Date                                 [DDMMYY]"
#define TITLE_WATER_2   "(2)  Cum. Precipitation                   [mm]      [###.##]"
#define TITLE_WATER_3   "(3)  Cum. Infiltration                    [mm]      [###.##]"
#define TITLE_WATER_4   "(4)  Cum. Runoff                          [mm]      [###.##]"
#define TITLE_WATER_5   "(5)  Cum. Pot. Evapotranspiration         [mm]      [###.##]"
#define TITLE_WATER_6   "(6)  Cum. Pot. Evaporation                [mm]      [###.##]"
#define TITLE_WATER_7   "(7)  Cum. Act. Evaporation                [mm]      [###.##]"
#define TITLE_WATER_8   "(8)  Cum. Pot. Transpiration              [mm]      [###.##]"
#define TITLE_WATER_9   "(9)  Cum. Act. Transpiration              [mm]      [###.##]"
#define TITLE_WATER_10  "(10) Cum. Interception                    [mm]      [###.##]"
#define TITLE_WATER_11  "(11) Cum. Drainwater                      [mm]      [###.##]"
#define TITLE_WATER_12  "(12) Water Storage in Soil Profile        [mm]      [###.##]"
#define TITLE_WATER_13  "(13) Water Content in 0 - 30cm Depth      [Vol%]    [###.##]"
#define TITLE_WATER_14  "(14) Water Content 30- 60cm Depth         [Vol%]    [###.##]"
#define TITLE_WATER_15  "(15) Water Content 60- 90cm Depth         [Vol%]    [###.##]"
#define TITLE_WATER_16  "(16) Water Content 90-120cm Depth         [Vol%]    [###.##]"
#define TITLE_WATER_17  "(17) Pond Water                           [mm]      [###.##]"
#define TITLE_WATER_18  "(18) Daily Precipitation                  [mm]      [###.##]"
#define TITLE_WATER_19  "(19) Daily Infiltration                   [mm]      [###.##]"
#define TITLE_WATER_20  "(20) Daily Runoff                         [mm]      [###.##]"
//#define TITLE_WATER_21  "(21) Daily Pot. Evapotranspiration        [mm]      [###.##]"
//SG20140918 für Daisy:
#define TITLE_WATER_21  "(21) Daily Act. Evapotranspiration        [mm]      [###.##]"
#define TITLE_WATER_22  "(22) Daily Pot. Evaporation               [mm]      [###.##]"
#define TITLE_WATER_23  "(23) Daily Act. Evaporation               [mm]      [###.##]"
#define TITLE_WATER_24  "(24) Daily Pot. Transpiration             [mm]      [###.##]"
#define TITLE_WATER_25  "(25) Daily Act. Transpiration             [mm]      [###.##]"
#define TITLE_WATER_26  "(26) Daily Interception                   [mm]      [###.##]"
#define TITLE_WATER_27  "(27) Daily Drainwater                     [mm]      [###.##]"

#define TITLE_WATER_NUM 27

   
   
/*********************************************************************************************
   Outputfiles
   HEAT-Definitions
 ********************************************************************************************/

#define TITLE_HEAT_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)"
#define TITLE_HEAT_LINE_2 "  Date \t AirTmp \t   5cm\t   10cm\t   20cm\t   50cm\t  100cm"

#define TITLE_HEAT_1 "(1) Date                                [DDMMYY]"
#define TITLE_HEAT_2 "(2) Air Temperature                     [°C] [###.##]"
#define TITLE_HEAT_3 "(3) Soil Temperature in    5cm Depth    [°C] [###.##]"
#define TITLE_HEAT_4 "(4) Soil Temperature in   10cm Depth    [°C] [###.##]"
#define TITLE_HEAT_5 "(5) Soil Temperature in   20cm Depth    [°C] [###.##]"
#define TITLE_HEAT_6 "(6) Soil Temperature in   50cm Depth    [°C] [###.##]"
#define TITLE_HEAT_7 "(7) Soil Temperature in  100cm Depth    [°C] [###.##]"
#define TITLE_HEAT_8 "All temperatures are mean daily values."

#define TITLE_HEAT_NUM 7



/*********************************************************************************************
   Outputfiles
   NITRO-Definitions
 ********************************************************************************************/

#define TITLE_NITRO_LINE_1 "   (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)\t   (10)\t   (11)\t   (12)\t   (13)\t   (14)\t   (15)\t   (16)\t   (17)\t   (18)\t   (19)\t   (20)\t   (21)\t   (22)\t   (23)\t   (24)\t   (25)\t   (26)\t   (27)\t   (28)\t   (29)\t   (30)\t   (31)"
#define TITLE_NITRO_LINE_2 "  Date \tc_NO3Leach\tc_NH4Leach\tc_NR_L\tc_NR_H\tc_NR_t\t  c_Nitr\tc_Deni\tc_Imob\tc_N2O \tc_NO  \tc_N2  \tc_NH3 \tNO3_30\tNO3_60\tNO3_90\t NO3120\t NO3_t\t NH4-N\t d_NO3Leach\t d_NH4Leach\t d_NR_L\t d_NR_H\t d_NR_t\t d_Nitr\t d_Deni\t d_Imob\t d_N2O\t  d_NO\t   d_N2\t  d_NH3"

#define TITLE_NITRO_1   "(1)  Date                                 [DDMMYY]"
#define TITLE_NITRO_2   "(2)  Cum. NO3-N Leaching                  [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_3   "(3)  Cum. NH4-N Leaching                  [kg NH4-N/ha] [###.##]"
#define TITLE_NITRO_4   "(4)  Cum. N Release from Litter           [kg N/ha]     [###.##]"
#define TITLE_NITRO_5   "(5)  Cum. N Release from Humus            [kg N/ha]     [###.##]"
#define TITLE_NITRO_6   "(6)  Total Cum. Nitrogen Release          [kg N/ha]     [###.##]"
#define TITLE_NITRO_7   "(7)  Cum. Nitrified Nitrogen              [kg N/ha]     [###.##]"
#define TITLE_NITRO_8   "(8)  Cum. Denitrified Nitrogen            [kg N/ha]     [###.##]"
#define TITLE_NITRO_9   "(9)  Cum. Immobilized Nitrogen            [kg N/ha]     [###.##]"
#define TITLE_NITRO_10  "(10) Cum. N2O-N Emission                  [kg N2O-N/ha] [###.##]"
#define TITLE_NITRO_11  "(11) Cum. NO-N Emission                   [kg N2O-N/ha] [###.##]"
#define TITLE_NITRO_12  "(12) Cum. N2-N Emission                   [kg N2O-N/ha] [###.##]"
#define TITLE_NITRO_13  "(13) Cum. NH3-N Emission                  [kg N2O-N/ha] [###.##]"
#define TITLE_NITRO_14  "(14) NO3-N Content in 0- 30cm Depth       [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_15  "(15) NO3-N Content in 30- 60cm Depth      [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_16  "(16) NO3-N Content in 60- 90cm Depth      [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_17  "(17) NO3-N Content in 90-120cm Depth      [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_18  "(18) NO3-N Content in Profile             [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_19  "(19) NH4-N Content in Profile             [kg NH4-N/ha] [###.##]"
#define TITLE_NITRO_20  "(20) Daily NO3-N Leaching                 [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_21  "(21) Daily NH4-N Leaching                 [kg NO3-N/ha] [###.##]"
#define TITLE_NITRO_22  "(22) Daily N Release from Litter          [kg N/ha]     [###.##]"
#define TITLE_NITRO_23  "(23) Daily N Release from Humus           [kg N/ha]     [###.##]"
#define TITLE_NITRO_24  "(24) Total Daily N Release                [kg N/ha]     [###.##]"
#define TITLE_NITRO_25  "(25) Daily Nitrified Nitrogen             [kg N/ha]     [###.##]"
#define TITLE_NITRO_26  "(26) Daily Denitrified Nitrogen           [kg N/ha]     [###.##]"
#define TITLE_NITRO_27  "(27) Daily Immobilized Nitrogen           [kg N/ha]     [###.##]"
#define TITLE_NITRO_28  "(28) Daily N2O-N Emission                 [kg N2O-N/ha] [###.##]"
#define TITLE_NITRO_29  "(29) Daily NO-N Emission                  [kg N2O-N/ha] [###.##]"
#define TITLE_NITRO_30  "(30) Daily N2-N Emission                  [kg N2O-N/ha] [###.##]"
#define TITLE_NITRO_31  "(31) Daily NH3-N Emission                 [kg NH3-N/ha] [###.##]"

#define TITLE_NITRO_NUM 31    


/*********************************************************************************************
   Outputfiles
   CARBO-Definitions
 ********************************************************************************************/

#define TITLE_CARBO_LINE_1 "   (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)\t   (10)\t   (11)\t   (12)\t   (13)\t   (14)\t   (15)\t   (16)\t   (17)\t   (18)\t   (19)\t   (20)\t   (21)\t   (22)\t   (23)\t   (24)\t   (25)\t   (26)\t   (27)"
#define TITLE_CARBO_LINE_2 "  Date \tc_CO2_Em\tc_CO2_S\tc_CH4_Im\td_CO2_Em\td_CO2_S\td_CH4_Im\tC_Litter\tC_Manure\tC_Humus\tC_LitSur\tC_ManSur\tC_HumSur\tC_StandR\tC_SMicB\tN_Litter\tN_Manure\tN_Humus\tN_LitSur\tN_ManSur\tN_HumSur\tN_StandR\tN_SMicB\t  N_DON\t  C_DOC\t  c_DON_L\tc_DOC_L"

#define TITLE_CARBO_1   "(1)  Date                                  [DDMMYY]"
#define TITLE_CARBO_2   "(2)  Cum.  CO2 Emission from Soil          [kg CO2-C/ha] [###.##]"
#define TITLE_CARBO_3   "(3)  Cum.  CO2 Emission from Surface-Pool  [kg CO2-C/ha] [###.##]"
#define TITLE_CARBO_4   "(4)  Cum.  CH4 Immission                   [kg CH4-C/ha] [###.##]"
#define TITLE_CARBO_5   "(5)  Daily CO2 Emission from Soil          [kg CO2-C/ha] [###.##]"
#define TITLE_CARBO_6   "(6)  Daily CO2 Emission from Surface-Pool  [kg CO2-C/ha] [###.##]"
#define TITLE_CARBO_7   "(7)  Daily CH4 Immission                   [kg CH4-C/ha] [###.##]"
#define TITLE_CARBO_8   "(8)  Carbon Content in Litter              [kg C/ha]     [###.##]"
#define TITLE_CARBO_9   "(9)  Carbon Content in Manure              [kg C/ha]     [###.##]"
#define TITLE_CARBO_10  "(10) Carbon Content in Humus               [kg C/ha]     [###.##]"
#define TITLE_CARBO_11  "(11) Carbon Content in Surface-Litter      [kg C/ha]     [###.##]"
#define TITLE_CARBO_12  "(12) Carbon Content in Surface-Manure      [kg C/ha]     [###.##]"
#define TITLE_CARBO_13  "(13) Carbon Content in Surface-Humus       [kg C/ha]     [###.##]"
#define TITLE_CARBO_14  "(14) Carbon Content in Standing Residues   [kg C/ha]     [###.##]"
#define TITLE_CARBO_15  "(15) Carbon Content in Soil Microbes       [kg C/ha]     [###.##]"
#define TITLE_CARBO_16  "(16) Nitrogen Content in Litter            [kg N/ha]     [###.##]"
#define TITLE_CARBO_17  "(17) Nitrogen Content in Manure            [kg N/ha]     [###.##]"
#define TITLE_CARBO_18  "(18) Nitrogen Content in Humus             [kg N/ha]     [###.##]"
#define TITLE_CARBO_19  "(19) Nitrogen Content in Surface-Litter    [kg N/ha]     [###.##]"
#define TITLE_CARBO_20  "(20) Nitrogen Content in Surface-Manure    [kg N/ha]     [###.##]"
#define TITLE_CARBO_21  "(21) Nitrogen Content in Surface-Humus     [kg N/ha]     [###.##]"
#define TITLE_CARBO_22  "(22) Nitrogen Content in Standing Residues [kg C/ha]     [###.##]"
#define TITLE_CARBO_23  "(23) Nitrogen Content in Soil Microbes     [kg N/ha]     [###.##]"
#define TITLE_CARBO_24  "(24) Nitrogen Content in DON               [kg N/ha]     [###.##]"
#define TITLE_CARBO_25  "(25) Carbon   Content in DOC               [kg C/ha]     [###.##]"
#define TITLE_CARBO_26  "(26) Cum. DON Leaching                     [kg N/ha]     [###.##]"
#define TITLE_CARBO_27  "(27) Cum. DOC Leaching                     [kg C/ha]     [###.##]"


#define TITLE_CARBO_28  "(28) Carbon Content in HumusSlow (SOM1)    [kg C/ha]     [###.##]"
#define TITLE_CARBO_29  "(29) Nitrogen Content in HumusSlow (SOM1)  [kg N/ha]     [###.##]"
#define TITLE_CARBO_30  "(30) Carbon Content in HumusFast (SOM2)    [kg C/ha]     [###.##]"
#define TITLE_CARBO_31  "(31) Nitrogen Content in HumusFast (SOM2)  [kg N/ha]     [###.##]"

#define TITLE_CARBO_NUM 31

/*********************************************************************************************
   Outputfiles
   PLANT-Definitions
 ********************************************************************************************/

#define TITLE_PLANT_LINE_1 "   (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)\t   (10)\t   (11)\t   (12)\t   (13)\t   (14)\t   (15)\t   (16)\t   (17)\t   (18)\t   (19)\t   (20)\t   (21)"
#define TITLE_PLANT_LINE_2 "  Date \t   Dev\t  LAI \t num_P/T\t Root_d\t   Root_BM\t Vag_BM\t gen_BM\t BM_t \t N_Root\t N_agBM\t N_gBM\t c_NUpt\t %N_Root\t %N_agBM\t %N_gBM\t d_NUpt\t d_potT\t d_actT\t d_actET\t c_actT"

#define TITLE_PLANT_1   "(1)  Date                                     [DDMMYY]"
#define TITLE_PLANT_2   "(2)  Development Stage                        [1]         [###.##]"
#define TITLE_PLANT_3   "(3)  Leaf Area Index                          [1]         [###.##]"
#define TITLE_PLANT_4   "(4)  Number of Plants / Tillers               [Plants/ha] [###.##]"
#define TITLE_PLANT_5   "(5)  Rooting Depth                            [cm]        [###.##]"
#define TITLE_PLANT_6   "(6)  Root Biomass                             [kg/ha]     [###.##]"
#define TITLE_PLANT_7   "(7)  Veget. Above-Ground Biomass              [kg/ha]     [###.##]"
#define TITLE_PLANT_8   "(8)  Generative Biomass                       [kg/ha]     [###.##]"
#define TITLE_PLANT_9   "(9)  Total Biomass                            [kg/ha]     [###.##]"
#define TITLE_PLANT_10  "(10) Nitrogen Content in Roots                [kg N/ha]   [###.##]"
#define TITLE_PLANT_11  "(11) Nitrogen Content in Above-Ground Biomass [kg N/ha]   [###.##]"
#define TITLE_PLANT_12  "(12) Nitrogen Content in Generative Biomass   [kg N/ha]   [###.##]"
#define TITLE_PLANT_13  "(13) Total Cum. N-Uptake                      [kg N/ha]   [###.##]"
#define TITLE_PLANT_14  "(14) Nitrogen Conc. in Roots                  [%]         [###.##]"
#define TITLE_PLANT_15  "(15) Nitrogen Conc. in Above-Ground Biomass   [%]         [###.##]"
#define TITLE_PLANT_16  "(16) Nitrogen Conc. in Generative Biomass     [%]         [###.##]"
#define TITLE_PLANT_17  "(17) Total Daily N-Uptake                     [kg N/ha]   [###.##]"
#define TITLE_PLANT_18  "(18) Daily Pot. Transpiration                 [mm]        [###.##]"
#define TITLE_PLANT_19  "(19) Daily Act. Transpiration                 [mm]        [###.##]"
#define TITLE_PLANT_20  "(20) Daily Act. Evapotranspiration            [mm]        [###.##]"
#define TITLE_PLANT_21  "(21) Cum. Act. Transpiration                  [mm]        [###.##]"

#define TITLE_PLANT_NUM 21 
         


/*********************************************************************************************
   Outputfiles
   Balance-Definition
 ********************************************************************************************/
         
#define TITLE_BALANCE_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)"
#define TITLE_BALANCE_LINE_2 "  Date\t BalH2O\t c_InpH2O\t c_OutH2O\t H2O_t\t Bal_N\t c_InpN\t c_OutN\t N_t"

#define TITLE_BALANCE_1 "(1) Date                  [DDMMYY]"
#define TITLE_BALANCE_2 "(2) Water Balance         [mm]         [###.##]"
#define TITLE_BALANCE_3 "(3) Cum. Water Input      [mm]         [###.##]"
#define TITLE_BALANCE_4 "(4) Cum. Water Output     [mm]         [###.##]"
#define TITLE_BALANCE_5 "(5) Water and Ice in Soil [mm]         [###.##]"
#define TITLE_BALANCE_6 "(6) Nitrogen Balance      [kg N/ha]    [###.##]"
#define TITLE_BALANCE_7 "(7) Cum. Nitrogen Input   [kg N/ha]    [###.##]"
#define TITLE_BALANCE_8 "(8) Cum. Nitrogen Output  [kg N/ha]    [###.##]"
#define TITLE_BALANCE_9 "(9) Nitrogen in Soil      [kg N/ha]    [###.##]"
#define TITLE_BALANCE_NUM 9

/*********************************************************************************************
   Outputfiles
   GisData-Definition
 ********************************************************************************************/
         
#define TITLE_GISDATA_LINE_1 " (1)\t(2)\t(3)\t(4)\t(5)\t(6)\t(7)\t(8)\t(9)\t(10)\t(11)\t(12)\t(13)\t(14)\t(15)\t(16)\t(17)\t(18)\t(19)\t(20)\t(21)\t(22)"
#define TITLE_GISDATA_LINE_2 "Date\t ArNr\tFarmNr\t ParNa\t PC\t CC\t MC\t nit_tot\t am_tot\t c_NLe\t c_NRel\t c_Pre\t c_Inf\t c_run\t c_aEvp\t c_aTrs\t c_Drai\t pond\t NinDrai\t 0-30\t 30-60\t 60-90"
							         	  	   	     	        	                         	    	  
#define TITLE_GISDATA_1 "(1) Last day of Simulationperiode                               [DDMMYY]"
#define TITLE_GISDATA_2 "(2) Area number                                                 [####]"
#define TITLE_GISDATA_3 "(3) Farm number                                                 [##########]"  
#define TITLE_GISDATA_4 "(4) Parcel name                                                         "  
#define TITLE_GISDATA_5 "(5) Previous crop                                                       "       
#define TITLE_GISDATA_6 "(6) Cath crop                                                           "       
#define TITLE_GISDATA_7 "(7) Main Crop                                                           "
#define TITLE_GISDATA_8 "(8) Nitrate-N Content in Profile [kg NO3-N/ha]                  [###.##]"
#define TITLE_GISDATA_9 "(9) Ammonium-N Content in Profile [kg NH4-N/ha]                 [###.##]"
#define TITLE_GISDATA_10 "(10) Cum. Nitrate-N Leaching [kg NO3-N/ha]                     [###.##]"
#define TITLE_GISDATA_11 "(11) Total Cum. Nitrogen Release [kg N/ha]                     [###.##]"
#define TITLE_GISDATA_12 "(12) Cumulative Precipitation [mm]                             [###.##]"
#define TITLE_GISDATA_13 "(13) Cumulative Infiltration [mm]                              [###.##]"
#define TITLE_GISDATA_14 "(14) Cumulative Runoff [mm]                                    [###.##]"
#define TITLE_GISDATA_15 "(15) Cumulative act. Evaporation [mm]                          [###.##]"
#define TITLE_GISDATA_16 "(16) Cumulative act. Transpiration [mm]                        [###.##]"
#define TITLE_GISDATA_17 "(17) Cumulative Drainwater [mm]                                [###.##]"
#define TITLE_GISDATA_18 "(18) Pond water [mm]                                           [###.##]"	
#define TITLE_GISDATA_19 "(19) N-Concentration in Drainwater [mg/l]                      [###.#]"	
#define TITLE_GISDATA_20 "(20) Nitrate-N Content in  0-30cm [kg NO3-N/ha]                [###.##]"
#define TITLE_GISDATA_21 "(21) Nitrate-N Content in 30-60cm [kg NO3-N/ha]                [###.##]"
#define TITLE_GISDATA_22 "(22) Nitrate-N Content in 60-90cm [kg NO3-N/ha]                [###.##]"
#define TITLE_GISDATA_NUM 9


////////////////////////////////////////////////////////////////////////////////////////////////////
//SG20150210: für Scott Demyan - Ausgabe der DAISY-Pools SOM1 und SOM2

/*********************************************************************************************
   Outputfiles
   DAISY-Definitions
 ********************************************************************************************/

#define TITLE_DAISY_LINE_1 " (1)\t(2)\t(3)\t(4)\t(5)\t(6)\t(7)\t(8)\t(9)\t(10)\t(11)\t(12)\t(13)\t(14)\t(15)\t(16)\t(17)\t(18)\t(19)\t(20)\t(21)\t(22)\t(23)\t(24)\t(25)\t(26)\t(27)\t(28)\t(29)\t(30)\t(31)\t(32)\t(33)\t(34)\t(35)\t(36)\t(37)\t(38)\t(39)\t(40)\t(41)\t(42)\t(43)\t(44)\t(45)\t(46)\t(47)\t(48)\t(49)\t(50)\t(51)\t(52)\t(53)\t(54)\t(55)\t(56)\t(57)\t(58)\t(59)\t(60)\t(61)\t(62)\t(63)\t(64)\t(65)\t(66)\t(67)\t(68)\t(69)\t(70)\t(71)\t(72)\t(73)"
#define TITLE_DAISY_LINE_2 " Date\tC_AOM1_30\tC_AOM1_60\tC_AOM1_90\tN_AOM1_30\tN_AOM1_60\tN_AOM1_90\tC_AOM1\tN_AOM1\tC_AOM2_30\tC_AOM2_60\tC_AOM2_90\tN_AOM2_30\tN_AOM2_60\tN_AOM2_90\tC_AOM2\tN_AOM2\tC_BOM1_30\tC_BOM1_60\tC_BOM1_90\tN_BOM1_30\tN_BOM1_60\tN_BOM1_90\tC_BOM1\tN_BOM1\tC_BOM2_30\tC_BOM2_60\tC_BOM2_90\tN_BOM2_30\tN_BOM2_60\tN_BOM2_90\tC_BOM2\tN_BOM2\tC_BOMD_30\tC_BOMD_60\tC_BOMD_90\tN_BOMD_30\tN_BOMD_60\tN_BOMD_90\tC_BOMD\tN_BOMD\tC_BOM_30\tC_BOM_60\tC_BOM_90\tN_BOM_30\tN_BOM_60\tN_BOM_90\tC_BOM\tN_BOM\tC_SOM1_30\tC_SOM1_60\tC_SOM1_90\tN_SOM1_30\tN_SOM1_60\tN_SOM1_90\tC_SOM1\tN_SOM1\tC_SOM2_30\tC_SOM2_60\tC_SOM2_90\tN_SOM2_30\tN_SOM2_60\tN_SOM2_90\tC_SOM2\tN_SOM2\tC_Total_30\tC_Total_60\tC_Total_90\tN_Total_30\tN_Total_60\tN_Total_90\tC_Total\tN_Total"

//#define TITLE_DAISY_LINE_1 "  Test1"
//#define TITLE_DAISY_LINE_2 "  Test2"

#define TITLE_DAISY_1   "(1)  Date                                       [DDMMYY]"
#define TITLE_DAISY_2   "(2)  C in Fresh organic matter Slow (AOM1) in  0- 30cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_3   "(3)  C in Fresh organic matter Slow (AOM1) in 30- 60cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_4   "(4)  C in Fresh organic matter Slow (AOM1) in 60- 90cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_5   "(5)  N in Fresh organic matter Slow (AOM1) in  0- 30cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_6   "(6)  N in Fresh organic matter Slow (AOM1) in 30- 60cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_7   "(7)  N in Fresh organic matter Slow (AOM1) in 60- 90cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_8   "(8)  Carbon Content in AOM1 (Profile)           [kg C/ha] [###.##]"
#define TITLE_DAISY_9   "(9)  Nitrogen Content in AOM1 (Profile)         [kg N/ha] [###.##]"
#define TITLE_DAISY_10  "(10) C in Fresh organic matter Fast (AOM2) in  0- 30cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_11  "(11) C in Fresh organic matter Fast (AOM2) in 30- 60cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_12  "(12) C in Fresh organic matter Fast (AOM2) in 60- 90cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_13  "(13) N in Fresh organic matter Fast (AOM2) in  0- 30cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_14  "(14) N in Fresh organic matter Fast (AOM2) in 30- 60cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_15  "(15) N in Fresh organic matter Fast (AOM2) in 60- 90cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_16  "(16) Carbon Content in AOM2 (Profile)           [kg C/ha] [###.##]"
#define TITLE_DAISY_17  "(17) Nitrogen Content in AOM2 (Profile)         [kg N/ha] [###.##]"
#define TITLE_DAISY_18  "(18) C in MicBiomSlow (BOM1) in  0- 30cm Depth  [kg C/ha] [###.##]"
#define TITLE_DAISY_19  "(19) C in MicBiomSlow (BOM1) in 30- 60cm Depth  [kg C/ha] [###.##]"
#define TITLE_DAISY_20  "(20) C in MicBiomSlow (BOM1) in 60- 90cm Depth  [kg C/ha] [###.##]"
#define TITLE_DAISY_21  "(21) N in MicBiomSlow (BOM1) in  0- 30cm Depth  [kg N/ha] [###.##]"
#define TITLE_DAISY_22  "(22) N in MicBiomSlow (BOM1) in 30- 60cm Depth  [kg N/ha] [###.##]"
#define TITLE_DAISY_23  "(23) N in MicBiomSlow (BOM1) in 60- 90cm Depth  [kg N/ha] [###.##]"
#define TITLE_DAISY_24  "(24) Carbon Content in BOM1 (Profile)           [kg C/ha] [###.##]"
#define TITLE_DAISY_25  "(25) Nitrogen Content in BOM1 (Profile)         [kg N/ha] [###.##]"
#define TITLE_DAISY_26  "(26) C in MicBiomFast (BOM2) in  0- 30cm Depth  [kg C/ha] [###.##]"
#define TITLE_DAISY_27  "(27) C in MicBiomFast (BOM2) in 30- 60cm Depth  [kg C/ha] [###.##]"
#define TITLE_DAISY_28  "(28) C in MicBiomFast (BOM2) in 60- 90cm Depth  [kg C/ha] [###.##]"
#define TITLE_DAISY_29  "(29) N in MicBiomFast (BOM2) in  0- 30cm Depth  [kg N/ha] [###.##]"
#define TITLE_DAISY_30  "(30) N in MicBiomFast (BOM2) in 30- 60cm Depth  [kg N/ha] [###.##]"
#define TITLE_DAISY_31  "(31) N in MicBiomFast (BOM2) in 60- 90cm Depth  [kg N/ha] [###.##]"
#define TITLE_DAISY_32  "(32) Carbon Content in BOM2 (Profile)           [kg C/ha] [###.##]"
#define TITLE_DAISY_33  "(33) Nitrogen Content in BOM2 (Profile)         [kg N/ha] [###.##]"
#define TITLE_DAISY_34  "(34) C in MicBiomDenit (BOMD) in  0- 30cm Depth [kg C/ha] [###.##]"
#define TITLE_DAISY_35  "(35) C in MicBiomDenit (BOMD) in 30- 60cm Depth [kg C/ha] [###.##]"
#define TITLE_DAISY_36  "(36) C in MicBiomDenit (BOMD) in 60- 90cm Depth [kg C/ha] [###.##]"
#define TITLE_DAISY_37  "(37) N in MicBiomDenit (BOMD) in  0- 30cm Depth [kg N/ha] [###.##]"
#define TITLE_DAISY_38  "(38) N in MicBiomDenit (BOMD) in 30- 60cm Depth [kg N/ha] [###.##]"
#define TITLE_DAISY_39  "(39) N in MicBiomDenit (BOMD) in 60- 90cm Depth [kg N/ha] [###.##]"
#define TITLE_DAISY_40  "(40) Carbon Content in BOMD (Profile)           [kg C/ha] [###.##]"
#define TITLE_DAISY_41  "(41) Nitrogen Content in BOMD (Profile)         [kg N/ha] [###.##]"
#define TITLE_DAISY_42  "(42) C in MicBiom (BOM) in  0- 30cm Depth       [kg C/ha] [###.##]"
#define TITLE_DAISY_43  "(43) C in MicBiom (BOM) in 30- 60cm Depth       [kg C/ha] [###.##]"
#define TITLE_DAISY_44  "(44) C in MicBiom (BOM) in 60- 90cm Depth       [kg C/ha] [###.##]"
#define TITLE_DAISY_45  "(45) N in MicBiom (BOM) in  0- 30cm Depth       [kg N/ha] [###.##]"
#define TITLE_DAISY_46  "(46) N in MicBiom (BOM) in 30- 60cm Depth       [kg N/ha] [###.##]"
#define TITLE_DAISY_47  "(47) N in MicBiom (BOM) in 60- 90cm Depth       [kg N/ha] [###.##]"
#define TITLE_DAISY_48  "(48) Carbon Content in BOM (Profile)            [kg C/ha] [###.##]"
#define TITLE_DAISY_49  "(49) Nitrogen Content in BOM (Profile)          [kg N/ha] [###.##]"
#define TITLE_DAISY_50  "(50) C in HumusSlow (SOM1) in  0- 30cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_51  "(51) C in HumusSlow (SOM1) in 30- 60cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_52  "(52) C in HumusSlow (SOM1) in 60- 90cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_53  "(53) N in HumusSlow (SOM1) in  0- 30cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_54  "(54) N in HumusSlow (SOM1) in 30- 60cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_55  "(55) N in HumusSlow (SOM1) in 60- 90cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_56  "(56) Carbon Content in SOM1 (Profile)           [kg C/ha] [###.##]"
#define TITLE_DAISY_57  "(57) Nitrogen Content in SOM1 (Profile)         [kg N/ha] [###.##]"
#define TITLE_DAISY_58  "(58) C in HumusFast (SOM2) in  0- 30cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_59  "(59) C in HumusFast (SOM2) in 30- 60cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_60  "(60) C in HumusFast (SOM2) in 60- 90cm Depth    [kg C/ha] [###.##]"
#define TITLE_DAISY_61  "(61) N in HumusFast (SOM2) in  0- 30cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_62  "(62) N in HumusFast (SOM2) in 30- 60cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_63  "(63) N in HumusFast (SOM2) in 60- 90cm Depth    [kg N/ha] [###.##]"
#define TITLE_DAISY_64  "(64) Carbon Content in SOM2 (Profile)           [kg C/ha] [###.##]"
#define TITLE_DAISY_65  "(65) Nitrogen Content in SOM2 (Profile)         [kg N/ha] [###.##]"
#define TITLE_DAISY_66  "(66) Total organic C in  0- 30cm Depth          [kg C/ha] [###.##]"
#define TITLE_DAISY_67  "(67) Total organic C in 30- 60cm Depth          [kg C/ha] [###.##]"
#define TITLE_DAISY_68  "(68) Total organic C in 60- 90cm Depth          [kg C/ha] [###.##]"
#define TITLE_DAISY_69  "(69) Total organic N in  0- 30cm Depth          [kg N/ha] [###.##]"
#define TITLE_DAISY_70  "(70) Total organic N in 30- 60cm Depth          [kg N/ha] [###.##]"
#define TITLE_DAISY_71  "(71) Total organic N in 60- 90cm Depth          [kg N/ha] [###.##]"
#define TITLE_DAISY_72  "(72) Total organic C (Profile)                  [kg C/ha] [###.##]"
#define TITLE_DAISY_73  "(73) Total organic N (Profile)                  [kg N/ha] [###.##]"

#define TITLE_DAISY_NUM 73
////////////////////////////////////////////////////////////////////////////////////////////////////

#define START_VALUE_WRITTEN_TXT "Start Value File Written."
#define HYDRAULIC_PROPERTIES_WRITTEN_TXT   "Hydraulic Properties File Written."
#define END_OF_SIM_TXT "End of simulation."


#define ERROR_TXT "Error"
#define ABORT_SIM_TXT "Abort Simulation"



// **********************************************************************************
// Messages
// **********************************************************************************

/********************************************************************************************
         Errors: 
*********************************************************************************************/
/* Memory Errors: */
#define   ALLOCATION_ERROR_TXT          "Allocation Error"

/* Read Errors: */
#define   FILEOPEN_ERROR_TXT   	        "File Open Error!"
#define   READ_VALUE_ERROR_TXT          "Read Error! Value in Input File not found!"
#define   SEARCHMARKER_ERROR_TXT        "Input File Read Error! Missing Marker: "
#define   READ_MODFILE_ERROR_TXT        "Parameter File Read Error! " 

#define   INPUT_DATA_ERROR_TXT          "Input Data Error!" 
#define   START_DATA_ERROR_TXT          "Start Value Error!" 
#define   START_BALANCE_ERROR_TXT       "Mass Balance Error in Start Values"
#define   CORRECT_MEASURE_VALUE_TXT     "Mass Balance Error in Correct Measure Value"

#define   START_DEPTH2SMALL_ERROR_TXT   "Depth of Start Profile Unsufficient"
/* Water Related Errors: */
#define   ERROR_IN_WATER_BALANCE_TXT    "Error in Water Balance "
#define   ERROR_H2O_PDE_ITERATION       "Water Flow PDE Solver Did Not Converge."
#define   ERROR_RESERVOIR_NEGATIV       "Water Reservoir Negative."
#define   ERROR_MAX_EVAP_TOO_HIGH       "Max. Evaporation Rate Too High."
#define   ERROR_INFILTRATION_TOO_HIGH   "Infiltration Too High."

/* Nitrogen Related Errors: */
#define   ERROR_IN_N_BALANCE_TXT        "Error in Nitrogen Balance "

/* Plant Related Errors: */
#define   ERROR_NO_PLANT_MEASURE        "Error in Plant Model: 2 Plant Measurements Required."
#define   ERROR_NO_ROOTS                "Error in Water Uptake: No Plant Roots."
#define   ERROR_H2O_UPTAKE_TOO_HIGH     "Error: Water Uptake Too High."
#define   ERROR_H2O_UPTAKE_TOO_LOW      "Error: Water Uptake Too Low."

/* Heat Related Errors: */
#define   ERROR_FREEZING_EXCEEDS_H2O    "Freezing Exceeds Available Water "

/* Other Errors: */
#define   RANGE_ERROR_TXT      	        "Variable Out of Valid Range!"
#define   ERROR_IN_FUNCTION_TXT         "Error in Module "
#define   ERROR_WEATHER_DATA            "Error in Weather Data."
#define   ERROR_NOT_POSITIVE_TXT   	    "Variable 0 or Negative!"

/********************************************************************************************
         Comments: 
*********************************************************************************************/

/* Management Related Comments: */
#define   COMMENT_FERTILIZER_TXT        "Fertilization: "
#define   COMMENT_TILLAGE_TXT           "Tillage: "
#define   COMMENT_IRRIGATION_TXT        "Irrigation: "


/* Water Related Comments: */
#define   COMMENT_PONDING_RUNS_OFF      "mm Ponding Becomes RunOff."
#define   COMMENT_SET_PONDING           "mm Water is Ponding."

/* Plant Related Comments: */

#define   COMMENT_GROWING_START         "Start of Growing Season"
#define   COMMENT_GROWING_END      	    "End of Growing Season"

/* Nitrogen Related Comments: */
#define   COMMENT_REWET_TXT             "% of Maximal Rewetting Event."

/* System Related Comments: */
#define   COMMENT_STARTVALUES_READ      "Start Values Read."

#define   COMMENT_START_DEPTH2BIG_TXT   "Start Profile Deeper Than Actual Profile."
#define   COMMENT_START_DISTRIBUTE_TXT  "Start Values have to be Distributed!"

#define   COMMENT_INCLUDED_TXT          " Included."





#define   COMMENT_HYDRAULIC_FUNCT_TXT   "Hydraulic Functions: "
#define   ERROR_INCLUDED_TXT            "Error! No Hydraulic Functions Selected!"
#define   COMMENT_FREEZING_START_TXT   	"Start of Freezing "
#define   COMMENT_FREEZING_END_TXT      "End of Freezing "
#define   COMMENT_THAWING_START_TXT     "Start of Thawing "
#define   COMMENT_PLANT_VALUES_TXT      "Potential Plant Values: "
#define   ERROR_GENOTYPE _FILE_TXT      "Genotype File Not Found ! File Open Error"
#define   COMMENT_READING _GENOTYPE_TXT	"Reading Genotype Data "
#define   COMMENT_CONDUCTIVITY_TXT      "Hydraulic Conductivity near 0!"
#define   COMMENT_M_POTENTIAL_LOW_TXT   "Matric Potential in Layer 0 Very Low!")
#define   COMMENT_M_POTENTIAL_HIGH_TXT  "Matric Potential to High in Soil!"


#endif




/**********************************************************************************************
 * content  :   Deutsche Sprach-Version für EXPERT-N  DLL
 * date		:   25.11.97
 * author   :   C.Haberbosch
 ********************************************************************************************
 */
#ifdef   GERMAN
#undef   ENGLISH

#define TITLE_LOG_1   "   Sim.-Zeit\t Nachricht                   "
/*********************************************************************************************
   Outputfiles
   WATER-Definitions
 ********************************************************************************************/

#define TITLE_WATER_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)\t   (10)\t   (11)\t   (12)\t   (13)\t   (14)\t   (15)\t   (16)\t   (17)\t   (18)\t   (19)"
#define TITLE_WATER_LINE_2 "Datum \t   k_NS\t k_Inf\tk_Ober\tk_pEvp\tk_aEvp\tk_aTrs\tk_Sick\tWG_30\tWG_60\tWG_90\tWG_120\tOb_H2O\ttgl_NS\t t_Inf\tt_Ober\tt_aEvp\tt_aTrs\tt_Sick"
                  
#define TITLE_WATER_1   "(1)   Datum des Simulationstages                 [TTMMJJ]"
#define TITLE_WATER_2   "(2)   Kumulativer Niederschlag                   [mm]      [###.##]"
#define TITLE_WATER_3   "(3)   Kumulative Infiltration         	          [mm]      [###.##]"
#define TITLE_WATER_4   "(4)   Kumulativer Oberflächenabfluss      	      [mm]      [###.##]"
#define TITLE_WATER_5   "(5)   Kumulative potentielle Evaporation      	  [mm]      [###.##]"
#define TITLE_WATER_6   "(6)   Kumulative aktuelle Evaporation            [mm]      [###.##]"
#define TITLE_WATER_7   "(7)   Kumulative aktuelle Transpiration          [mm]      [###.##]"
#define TITLE_WATER_8   "(8)   Kumulative Sickerwassermenge               [mm]      [###.##]"
#define TITLE_WATER_9   "(9) Wassergehalt in der Bodenschicht 0 - 30cm    [Vol%]    [###.##]"
#define TITLE_WATER_10 "(10) Wassergehalt in der Bodenschicht 30- 60cm    [Vol%]    [###.##]"
#define TITLE_WATER_11 "(11) Wassergehalt in der Bodenschicht 60- 90cm    [Vol%]    [###.##]"
#define TITLE_WATER_12 "(12) Wassergehalt in der Bodenschicht 90-120cm    [Vol%]    [###.##]"
#define TITLE_WATER_13 "(13) Oberflächenwasser                            [mm]      [###.##]"
#define TITLE_WATER_14 "(14) Täglicher Niederschlag                       [mm]      [###.##]"
#define TITLE_WATER_15 "(15) Tägliche Infiltration                        [mm]      [###.##]"
#define TITLE_WATER_16 "(16) Täglicher Oberfächenabfluss                  [mm]      [###.##]"
#define TITLE_WATER_17 "(17) Tägliche aktuelle Evaporation                [mm]      [###.##]"
#define TITLE_WATER_18 "(18) Tägliche aktuelle Transpiration              [mm]      [###.##]"
#define TITLE_WATER_19 "(19) Tägliches Sickerwasser                       [mm]      [###.##]"

#define TITLE_WATER_NUM 19

   
   
/*********************************************************************************************
   Outputfiles
   HEAT-Definitions
 ********************************************************************************************/

#define TITLE_HEAT_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)"
#define TITLE_HEAT_LINE_2 "Datum \tLuftt.\t    5cm\t   10cm\t   20cm\t   50cm\t 100cm"

#define TITLE_HEAT_1 "(1) Datum des Simulationstages           [TTMMJJ]"
#define TITLE_HEAT_2 "(2) Lufttemperatur                       [°C] [###.##]"
#define TITLE_HEAT_3 "(3) Bodentemperatur in    5cm Tiefe      [°C] [###.##]"
#define TITLE_HEAT_4 "(4) Bodentemperatur in   10cm Tiefe      [°C] [###.##]"
#define TITLE_HEAT_5 "(5) Bodentemperatur in   20cm Tiefe      [°C] [###.##]"
#define TITLE_HEAT_6 "(6) Bodentemperatur in   50cm Tiefe      [°C] [###.##]"
#define TITLE_HEAT_7 "(7) Bodentemperatur in  100cm Tiefe      [°C] [###.##]"
#define TITLE_HEAT_8 "Die Temperaturen sind Tagesdurchschnittswerte."

#define TITLE_HEAT_NUM 7



/*********************************************************************************************
   Outputfiles
   NITRO-Definitions
 ********************************************************************************************/

#define TITLE_NITRO_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)\t   (10)\t   (11)\t   (12)\t   (13)\t   (14)\t   (15)\t   (16)\t   (17)\t   (18)\t   (19)\t   (20)\t   (21)\t   (22)\t   (23)"
#define TITLE_NITRO_LINE_2 "Datum \tk_NVer\tk_NFoS\tk_NFhu\t k_NFg\tk_Nitr\tk_Deni\tk_Imob\t k_N2O\tNO3_30\tNO3_60\tNO3_90\tNO3120\tNO3_gs\t NH4-N\tt_NAus\tt_NFoS\tt_NFhu\t t_NFg\tt_Nitr\tt_Deni\tt_Imob\t t_N2O"

#define TITLE_NITRO_1   "(1)   Datum des Simulationstages                                      [TTMMJJ]"
#define TITLE_NITRO_2   "(2)   Kumulierte Nitrat-N-Verlagerung unterhalb des def. Bodenprofils [kg N/ha]       [###.##]"
#define TITLE_NITRO_3   "(3)   Kumulierte N-Freisetzung aus der frischen organischen Substanz  [kg N/ha]       [###.##]"
#define TITLE_NITRO_4   "(4)   Kumulierte N-Freisetzung aus der Humusfraktion                  [kg N/ha]       [###.##]"
#define TITLE_NITRO_5   "(5)   Kumulative Stickstoff-Freisetzung insgesamt                     [kg N/ha]       [###.##]"
#define TITLE_NITRO_6   "(6)   Kumulative Nitrifizierungsmenge                                 [kg N/ha]       [###.##]"
#define TITLE_NITRO_7   "(7)   Kumulative Denitrifizierungsmenge               	               [kg N/ha]       [###.##]"
#define TITLE_NITRO_8   "(8)   Kumulative Immobilisierungsmenge                                [kg N/ha]       [###.##]"
#define TITLE_NITRO_9   "(9)   Kumulativ ausgegaste N2O-Menge                                  [kg N2O-N/ha]   [###.##]"
#define TITLE_NITRO_10 "(10) Nitrat-N-Menge in der Bodenschicht 0 - 30cm      	               [kg NO3-N/ha]   [###.##]"
#define TITLE_NITRO_11 "(11) Nitrat-N-Menge in der Bodenschicht 30- 60cm                       [kg NO3-N/ha]   [###.##]"
#define TITLE_NITRO_12 "(12) Nitrat-N-Menge in der Bodenschicht 60- 90cm                       [kg NO3-N/ha]   [###.##]"
#define TITLE_NITRO_13 "(13) Nitrat-N-Menge in der Bodenschicht 90-120cm                       [kg NO3-N/ha]   [###.##]"
#define TITLE_NITRO_14 "(14) Nitrat-N-Menge im Gesamtprofil            	                       [kg NO3-N/ha]   [###.##]"
#define TITLE_NITRO_15 "(15) Ammonium-N-Menge im Gesamtprofil                                  [kg NH4-N/ha]   [###.##]"
#define TITLE_NITRO_16 "(16) Tägliche Nitrat-N-Verlagerung unterhalb des def. Bodenprofils     [kg N/ha]       [###.##]"
#define TITLE_NITRO_17 "(17) Tägliche N-Freisetzung aus der frischen organischen Substanz      [kg N/ha]       [###.##]"
#define TITLE_NITRO_18 "(18) Tägliche N-Freisetzung aus dem Humus                              [kg N/ha]       [###.##]"
#define TITLE_NITRO_19 "(19) Tägliche N-Freisetzung insgesamt                                  [kg N/ha]       [###.##]"
#define TITLE_NITRO_20 "(20) Täglich nitrifizierte N-Menge                                     [kg N/ha]       [###.##]"
#define TITLE_NITRO_21 "(21) Tägliche denitrifizierte N-Menge                                  [kg N/ha]       [###.##]"
#define TITLE_NITRO_22 "(22) Tägliche immobilisierte N-Menge                                   [kg N/ha]       [###.##]"
#define TITLE_NITRO_23 "(23) Täglich ausgegaste N2O-Menge                                      [kg N2O-N/ha]   [###.##]"

#define TITLE_NITRO_NUM 23    




/*********************************************************************************************
   Outputfiles
   PLANT-Definitions
 ********************************************************************************************/

#define TITLE_PLANT_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)\t   (10)\t   (11)\t   (12)\t   (13)\t   (14)\t   (15)\t   (16)\t   (17)\t   (18)\t   (19)\t   (20)"
#define TITLE_PLANT_LINE_2 "Datum \tEntStd\t    BFI\tPTZ\tWT\tWBM\tObV_BM\tgen_BM\tBM_ges\tN_W\tN_ObBM\t N_gnBM\tk_NAuf\t%N_Wur\t%N_oBM\t%N_gBM\tt_NAuf\tt_potT\tt_aktT\tk_aktT"

#define TITLE_PLANT_1   "(1)   Datum des Simulationstages                          	 [TTMMJJ]"
#define TITLE_PLANT_2   "(2)   Entwicklungsstadium                                 	 [1]		[###.##]"
#define TITLE_PLANT_3   "(3)   Blattflächenindex                                   	 [1]		[###.##]"
#define TITLE_PLANT_4   "(4)   Pflanzen-/Triebzahl                                 	 [Stück/ha] [###.##]"
#define TITLE_PLANT_5   "(5)   Durchwurzelungstiefe                                	 [cm]       [###.##]"
#define TITLE_PLANT_6   "(6)   Wurzelbiomasse                  	                   	 [kg/ha]    [###.##]"
#define TITLE_PLANT_7   "(7)   Oberirdisch vegetative Biomasse         	           	 [kg/ha]    [###.##]"
#define TITLE_PLANT_8   "(8)   Generative Biomasse                                 	 [kg/ha]    [###.##]"
#define TITLE_PLANT_9   "(9)   Gesamtbiomasse                  	                   	 [kg/ha]    [###.##]"
#define TITLE_PLANT_10 "(10) Stickstoffmenge in den Wurzeln                        	 [kg N/ha]  [###.##]"
#define TITLE_PLANT_11 "(11) Stickstoffmenge in der oberirdischen Biomasse         	 [kg N/ha]  [###.##]"
#define TITLE_PLANT_12 "(12) Stickstoffmenge in der generativen Biomasse           	 [kg N/ha]  [###.##]"
#define TITLE_PLANT_13 "(13) Kumulative Stickstoffaufnahme gesamt                  	 [kg N/ha]  [###.##]"
#define TITLE_PLANT_14 "(14) Stickstoffkonzentration in den Wurzeln                	 [%]        [###.##]"
#define TITLE_PLANT_15 "(15) Stickstoffkonz. in der oberirdisch vegetativen Biomasse [%]        [###.##]"
#define TITLE_PLANT_16 "(16) Stickstoffkonzentration in der generativen Biomasse   	 [%]        [###.##]"
#define TITLE_PLANT_17 "(17) Tägliche gesamte Stickstoffaufnahme                   	 [kg N/ha]  [###.##]"
#define TITLE_PLANT_18 "(18) Tägliche potentielle Transpiration                    	 [mm]       [###.##]"
#define TITLE_PLANT_19 "(19) Tägliche aktuelle Transpiration                       	 [mm]       [###.##]"
#define TITLE_PLANT_20 "(20) Kumulative aktuelle Transpiration                     	 [mm]       [###.##]"

#define TITLE_PLANT_NUM 20 
         


/*********************************************************************************************
   Outputfiles
   BALANCE-Definitions
 ********************************************************************************************/
         
#define TITLE_BALANCE_LINE_1 "    (1)\t    (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)"
#define TITLE_BALANCE_LINE_2 "Datum \tBilH2O\tInpH2O\tOutH2O\t H2O_g\t Bil_N\t Inp_N\t Out_N\t N_ges"

#define TITLE_BALANCE_1 "(1) Datum des Simulationstages         [DDMMYY]"
#define TITLE_BALANCE_2 "(2) Wasser-Bilanz                      [mm]         [###.##]"
#define TITLE_BALANCE_3 "(3) Kum. Wasser-Eintrag                [mm]         [###.##]"
#define TITLE_BALANCE_4 "(4) Kum. Wasser-Austrag                [mm]         [###.##]"
#define TITLE_BALANCE_5 "(5) Wasser und Eis im Boden            [mm]         [###.##]"
#define TITLE_BALANCE_6 "(6) Stickstoff-Bilanz         	        [kg N/ha]    [###.##]"
#define TITLE_BALANCE_7 "(7) Kum. Stickstoff-Eintrag          	[kg N/ha]    [###.##]"
#define TITLE_BALANCE_8 "(8) Kum. Stickstoff-Austrag          	[kg N/ha]    [###.##]"
#define TITLE_BALANCE_9 "(9) Stickstoff im Boden          	    [kg N/ha]    [###.##]"
#define TITLE_BALANCE_NUM 9


/*********************************************************************************************
   Outputfiles
   GisData-Definition
 ********************************************************************************************/
         
#define TITLE_GISDATA_LINE_1 "  (1)\t   (2)\t    (3)\t    (4)\t    (5)\t    (6)\t    (7)\t    (8)\t    (9)\t    (10)\t    (11)\t    (12)\t    (13)\t    (14)\t    (15)\t    (16)\t    (17)   (18)"
#define TITLE_GISDATA_LINE_2 "Datum\t  Gbnr\t  BetrNr\t  TeilN\t    VF\t     HF\t   NO3_gs\t NH4-N\t k_NVer\t k_NFg\t    k_NS\t   k_Inf\t  k_Ober\t  k_aEvp\t  k_aTrs\t  k_Sick\t  Ob_H2O\t  NinH2O"
							         	  	   	     	        	                         	    	  
#define TITLE_GISDATA_1 "(1) Letzter Tag des Simulationszeiraumes             				 [DDMMYY]"
#define TITLE_GISDATA_2 "(2) Gebietsnummer          								                       [####]"
#define TITLE_GISDATA_3 "(3) Betriebsnummer                                                                [##########]"  
#define TITLE_GISDATA_3 "(4) Teilschlagname                                                          "  
#define TITLE_GISDATA_4 "(5) Vorfrucht                                                               "       
#define TITLE_GISDATA_5 "(6) Hauptfrucht                                                             "
#define TITLE_GISDATA_6 "(7) Nitrat-N-Menge im Gesamtprofil   							     [kg NO3-N/ha] [###.##]"
#define TITLE_GISDATA_7 "(8) Ammonium-N-Menge im Gesamtprofil                                [kg NH4-N/ha] [###.##]"
#define TITLE_GISDATA_8 "(9)  Kumulierte Nitrat-N-Verlagerung unterhalb des def.Bodenprofils [kg N/ha]     [###.##]"
#define TITLE_GISDATA_9 "(10) Kumulative Stickstoff-Freisetzung insgesamt                    [kg N/ha]     [###.##]"
#define TITLE_GISDATA_9 "(11) Kumulativer Niederschlag                      			     [mm]          [###.##]"
#define TITLE_GISDATA_9 "(12) Kumulative Infiltration                       				 [mm]          [###.##]"
#define TITLE_GISDATA_9 "(13) Kumulativer Oberflächenabfluss                				 [mm]          [###.##]"
#define TITLE_GISDATA_9 "(14) Kumulative aktuelle Evaporation               				 [mm]          [###.##]"
#define TITLE_GISDATA_9 "(15) Kumulative aktuelle Transpiration             				 [mm]          [###.##]"
#define TITLE_GISDATA_9 "(16) Kumulative Sickerwassermenge                  				 [mm]          [###.##]"
#define TITLE_GISDATA_9 "(17) Oberflächenwasser                             				 [mm]          [###.##]"	
#define TITLE_GISDATA_9 "(18) durchschnittliche N-Konzentration im Sickerwasser				 [mg/l]        [###.#]"	
#define TITLE_GISDATA_NUM 9


#define START_VALUE_WRITTEN_TXT          "Startwert-Datei wurde geschrieben."
#define HYDRAULIC_PROPERTIES_WRITTEN_TXT "Datei der hydraulischen Funktionen wurde geschrieben."
#define END_OF_SIM_TXT                   "Ende der Simulation."


#define ERROR_TXT                        "Fehler"
#define ABORT_SIM_TXT                    "Simulation wird abgebrochen"


// **********************************************************************************
// Messages
// **********************************************************************************

/********************************************************************************************
         Errors: 
*********************************************************************************************/
/* Memory Errors: */
#define   ALLOCATION_ERROR_TXT          "Speicherzuordnungsfehler!"

/* Read Errors: */
#define   FILEOPEN_ERROR_TXT   	        "Fehler beim Öffnen der Datei"
#define   READ_VALUE_ERROR_TXT          "Lesefehler! Wert in Eingabedatei nicht gefunden!"
#define   SEARCHMARKER_ERROR_TXT        "Fehler beim Lesen der Eingabedatei! Fehlende Markierung: "
#define   READ_MODFILE_ERROR_TXT        "Fehler beim Lesen der Parameter-Datei! " 

#define   INPUT_DATA_ERROR_TXT          "Fehler in Eingabedaten!" 
#define   START_DATA_ERROR_TXT          "Fehler im Anfangswert!" 

/* Water Related Errors: */
#define   ERROR_IN_WATER_BALANCE_TXT    "Fehler in Wasserbilanz "
#define   ERROR_H2O_PDE_ITERATION       "Wasserfluß PDE-Löser konvergiert nicht"
#define   ERROR_RESERVOIR_NEGATIV       "Wasserreservoir negativ."
#define   ERROR_MAX_EVAP_TOO_HIGH       "Max. Evaporationsrate zu hoch."
#define   ERROR_INFILTRATION_TOO_HIGH   "Infiltration zu hoch."

/* Nitrogen Related Errors: */
#define   ERROR_IN_N_BALANCE_TXT        "Fehler in N-Bilanz "

/* Plant Related Errors: */
#define   ERROR_NO_PLANT_MEASURE        "Fehler im Pflanzenmodell: 2 Pflanzen-Meßwerte werden benötigt."
#define   ERROR_NO_ROOTS                "Fehler in Wasseraufnahme: keine Pflanzenwurzeln vorhanden."
#define   ERROR_H2O_UPTAKE_TOO_HIGH     "Fehler: Wasseraufnahme zu hoch."
#define   ERROR_H2O_UPTAKE_TOO_LOW      "Fehler: Wasseraufnahme zu gering."

/* Heat Related Errors: */
#define   ERROR_FREEZING_EXCEEDS_H2O    "Frieren übersteigt Wassergehalt "

/* Other Errors: */
#define   RANGE_ERROR_TXT      	        "Variable außerhalb des gültigen Bereichs!"
#define   ERROR_IN_FUNCTION_TXT         "Fehler in Modul "
#define   ERROR_WEATHER_DATA            "Fehler in Wetterdaten."
#define   ERROR_NOT_POSITIVE_TXT   	    "Variable 0 oder negativ!"

/********************************************************************************************
         Comments: 
*********************************************************************************************/

/* Management Related Comments: */
#define   COMMENT_FERTILIZER_TXT        "Düngung: "
#define   COMMENT_TILLAGE_TXT           "Bodenbearbeitung: "
#define   COMMENT_IRRIGATION_TXT        "Beregnung: "

/* Water Related Comments: */
#define   COMMENT_PONDING_RUNS_OFF      "mm Stauwasser fließt oberflächlich ab."
#define   COMMENT_SET_PONDING      	    "mm Wasser ist Stauwasser."

/* Plant Related Comments: */

#define   COMMENT_GROWING_START         "Start der Wachstumsperiode"
#define   COMMENT_GROWING_END      	    "Ende der Wachstumsperiode"

/* Nitrogen Related Comments: */
#define   COMMENT_REWET_TXT             "% der maximalen Wiederbefeuchtung."

/* System Related Comments: */
#define   COMMENT_STARTVALUES_READ      "Startwerte eingelesen."
#define   COMMENT_HYDRAULIC_FUNCT_TXT   "Hydraulische Funktionen: "
#define   ERROR_INCLUDED_TXT            "Fehler! Keine hydraulische Funktion ausgewählt "
#define   COMMENT_FREEZING _START_TXT   "Beginn der   Frostperiode "
#define   COMMENT_FREEZING_END_TXT      "Ende der Frostperiode "
#define   COMMENT_THAWING _START_TXT   	"Beginn der Tauperiode"
#define   COMMENT_PLANT_VALUES _TXT     "Potentielle Pflanzendaten: "
#define   ERROR_GENOTYPE _FILE_TXT      "Genotyp-Datei nicht gefunden! Fehler beim Öffnen"
#define   COMMENT_READING _GENOTYPE_TXT	"Lese Daten zum Genotyp ein."
#define   COMMENT_CONDUCTIVITY_TXT      "Hydraulische Leitfähigleit nahe 0!"
#define   COMMENT_M_POTENTIAL_LOW_TXT   "Matrix Potential in Schicht 0 sehr gering!")
#define   COMMENT_M_POTENTIAL_HIGH_TXT  "Matrix Potential des Bodens zu hoch!"


#endif

