=============================================================================
***	Expert-N		Start Eingabedatei	***
=============================================================================
=============================================================================
** Test-File for Simulation Object
=============================================================================
*Simulation data
10000
*Start Date (ddmmyy)	End Date (ddmmyy)
$DatumStart 	$DatumEnde
=============================================================================
*Farm Data
10001
*FarmID	No.Plots	Latitude	Longitude	Altitude
1	1	-43.70	172.30	10.0
*LastName	FirstName	Street
XXXXX	XXXXX	 XXXXX
*PLZ	City	Tel.	Fax.
-99	XXXXX	-99	-99
=============================================================================
*Plot Data
10002
*PlotID	Name	Size	Num.Subplots
1	Leeston     	1	1	
=============================================================================
=============================================================================
*Subplot data
10003
*ID	Name	Princ.Crop	Weatherfile	Parameterfile		ConfigFile
1	$Name	Crop		$xnw			$xnm			$xnc
*AreaNum.	Size	Slope	Exposition
1	1	0	S
=============================================================================
=============================================================================
*Plant Data
*Harvest Residues of preceding crop (before simulation starts)

Crop	Yield	harvest-	Crop-	C/N Crop-	root-	C/N root-
		date		residues	Residues	residues	residues
	kg/ha	Datum	kg/ha	1/1	kg/ha	1/1
10004
Pea	-99	-99	-99	-99	-99	-99
===============================================================================
*Cultivated Crop
Nr.	Bez.	Variety	Sowing	Row-	Seed-	Plant	Emerge	Harvest	Date	max.Root.	max. Root.	Removal
	Art	name	depth	space	density	Sowing	Date	Date	max.	Date	Depth	Yes=1
									Biom.			No=0
		Code	m	m	seed/m2	Datum	Datum	Datum	Datum	Datum	m
10005	1	 :Number of crops
1	WH	Wakanui	0.04	0.150	50.0	$Saattermin 	$TerminAuflaufen	   $TerminErnteNutzung	-99		-99			1.5			0
===============================================================================
MANAGEMENT DATA
===============================================================================
*Mineral Fertilization (Pro Measure)
Date	Fertilizer	Fertilizer	N	NO3-N	NH4-N	Amid-N
Datum	Name	Code	kg/ha	kg/ha	kg/ha	kg/ha

10006	$no_min_fert	:Number of MINERAL Fertilizer Applications
$min_fert_table
=============================================================================
*Organic Fertilization (Per measure)
Date	Org_fertilizer	Org_fertilizer	DM	N	NH4-N	Org.Sub.
Datum		Name	Code	kg/ha	kg/ha	kg/ha	kg/ha

10007	0	   :Number of ORGANIC Fertilizer Applications
=============================================================================
*Soil Management
Date	Depth	Equip.	Equip.
Datum	m	Name	Code

10008	0	   :Number of soil cultivation activities
=============================================================================
*Irrigation (per action)
Date	Amount	Name	Code	Rate	NO3	NH4	OrgC	OrgN
Datum		mm		Code	mm/h			

10009	0	: Number of Irrigation applications
==============================================================================
*SIMULATIONS LAYERS (equidistant)
	Num.layers	Num.Horizons
	(10,100)	(1 cm, 50 cm)

Layer	Num.Layers	Clay	Silt	Sand	Organ.	Bulk.Density	Stone.cont	pH
number	per.horizon	wgt.%	wgt.%	wgt.%	wgt.%	kg/dm3	Vol.%	1

10010	30	5	: Date Sampling and Number of soil layers
1	2	45.8	25.6	28.6	6.90	1.30	0	-99
2	4	45.8	25.6	28.6	5.17	1.30	0	-99
3	4	45.8	25.6	28.6	3.45	1.30	0	-99
4	4	45.8	25.6	28.6	1.72	1.30	0	-99
5	4	45.8	25.6	28.6	1.72	1.30	0	-99
6	4	45.8	25.6	28.6	1.72	1.30	0	-99
7	4	45.8	25.6	28.6	1.55	1.30	0	-99
8	4	45.8	25.6	28.6	1.55	1.30	0	-99
==================================================================================================================	
*START VALUES
	 Date	Sampling
Layer	 Num.Layers 	WaterCont	MatrixPot	Soiltemp	NH4	NO3	Root.Density	
number	 per.horizon 	Vol.%	kPa	°C	kg/ha	kg/ha	kg/dm3	
								
10011	210212	30		:Date Sampling and Number of soil layers		
1	10	30.00	-99.0	16.0	16.500	66.000	-99
2	10	30.00	-99.0	12.0	9.000	36.000	-99
3	10	30.00	-99.0	10.0	4.500	18.000	-99

==================================================================================================================
*WATER READINGS
Number of Events	Number of measurement depths (max. 5)
Measuring depths    (cm)
Date	Water.cont	Matrix.Pot
Datum	Vol.%		kPa

10012	0	0	   : 		
	
==================================================================================================================
*NITROGEN MEASUREMENTS
Number of Events	Number of measurement depths (max. 5)
Measuring depths (cm)
Date		Nitrat-N	Ammonium-N
Datum		kg/ha		kg/ha
10013	0	0
	
==================================================================================================================

*Plant Measurements I
Termin	Entw.	BFI	Boden	Blatt	Trieb	Pflanzen	Wurzel	Verd.		um. N
Datum	Stad.	bedeck.	anz.	zahl	hoehe	tiefe	faktor	Aufnahme
DD.MM.YY	 EC	 1/1	1/1		1/1	1/1	1/m2	cm	cm	1/1	kg/ha
10014	0
	
==================================================================================================================

*Plant Measurements II
10015	0
	
==================================================================================================================
***	Expert-N	Ende Eingabedatei LEESTON	***
==================================================================================================================
