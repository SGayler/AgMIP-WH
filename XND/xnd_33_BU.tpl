==================================================================================================================
***	Expert-N		Start Eingabedatei	***
==================================================================================================================
=============================================================================
** Simulation Object
=============================================================================
*Simulation data
10000
*Start Date (ddmmyy)	End Date (ddmmyy)
$DatumStart 	$DatumEnde
=============================================================================
*Farm Data
10001
*FarmID	No.Plots	Latitude	Longitude	Altitude
1	1	-34.60	-58.50	26
*LastName	FirstName	Street
ROTS	Département Calvados	 Region Valdivia	
*PLZ	City	Tel.	Fax.
5090000	Valdivia	-99	-99
=============================================================================
*Plot Data
10002
*PlotID	Name		Size	Num.Subplots
1	Buenos Aires	1	1
=============================================================================
=============================================================================
*Subplot data
10003
*ID	Name	Princ.Crop	Weatherfile	Parameterfile	ConfigFile
1	$Name	Crop		$xnw			$xnm			$xnc
*AreaNum.	Size	Slope	Exposition
1	1	0	S
=============================================================================
=============================================================================
*Plant Data
*Harvest Residues of preceding crop (before simulation starts)

Crop	Yield	harvest-	Crop-	 	C/N Crop-		root-		C/N root-
				date		residues	Residues		residues	residues
		kg/ha	Datum		kg/ha		1/1				kg/ha		1/1
10004
WH 		7000	$DatumStart		1000		80			2000		21
===================================================================================================================================
*Cultivated Crop
Nr.	Bez.	Variety	Sowing	Row-	Seed-	Plant	Emerge	Harvest	Date	max.Root.	max. Root.	Removal
	Art 	name	depth	space	density	Sowing	Date	Date	max.	Date		Depth	Yes=1
									Biom.			No=0
	Code			m		m		seed/m2	Datum	Datum	Datum	Datum	Datum		m
10005	1	:Number of crops
1 WH	Bacan-WW		0.03	0.16	250		$Saattermin 	$TerminAuflaufen	   $TerminErnteNutzung	-99		-99			1.2			0
===================================================================================================================================
MANAGEMENT DATA
===================================================================================================================================
*Mineral Fertilization (Pro Massnahme)
Date	Fertilizer	Fertilizer	N	NO3-N	NH4-N	Amid-N
Datum	Name	Code	kg/ha	kg/ha	kg/ha	kg/ha

10006	$no_min_fert	:Number of MINERAL Fertilizer Applications
$min_fert_table
=============================================================================
*Organic Fertilization (Pro Massnahme)
Date	Org_fertilizer	Org_fertilizer	DM	N	NH4-N	Org.Sub.
Datum	Name	Code	kg/ha	kg/ha	kg/ha	kg/ha

10007	0	:Number of ORGANIC Fertilizer Applications
=============================================================================
*Soil Management
Date	Depth	Equip.		Equip.
Datum	m	Name		Code

10008	0	:Number of soil cultivation activities
=============================================================================
*Irrigation (Pro Massnahme)
Date	Amount	Irrig
Datum	mm	Code

	10009	1	:Number of Irrigation applications
300809	15	IR004	0.6250	0	0	0	0	0

==================================================================================================================
*SIMULATIONS LAYERS (equidistant)
	Num.layers	Num.Horizons
	(10,100)	(1 cm, 50 cm)

Layer	Num.Layers	Clay	Silt	Sand	Organ.	Bulk.Density	Stone.cont	pH
number	per.horizon	wgt.%	wgt.%	wgt.%	wgt.%	kg/dm3	    Vol.%	1

10010	42	5			
1	1	27.8	60.3	11.9	1		1.41	0	5.4
2	1	27.8	60.3	11.9	1		1.41	0	5.4
3	2	27.8	60.3	11.9	0.6		1.42	0	6.1
4	2	27.8	60.3	11.9	0.6		1.21	0	6.5
5	2	57.9	38.6	3.5		0.4		1.25	0	6.9
6	2	57.9	38.6	3.5		0.4		1.25	0	6.9
7	2	57.9	38.6	3.5		0.4		1.25	0	6.9
8	2	57.9	38.6	3.5		0.1		1.2		0	7.1
9	4	57.9	38.6	3.5		0.1		1.2		0	7.1
10	4	22.2	65.2	12.6	0.05	1.18	0	7.3
11	4	22.2	65.2	12.6	0.05	1.18	0	7.3
12	4	22.2	65.2	12.6	0.05	1.18	0	8.3
13	4	22.2	65.2	12.6	0.05	1.18	0	8.3
14	4	22.2	65.2	12.6	0.05	1.18	0	8.3
15	4	22.2	65.2	12.6	0.05	1.18	0	8.3

==================================================================================================================				
*START VALUES				
	 DateSampling 				
				
Layer	 Num.Layers 	WaterCont	MatrixPot	Soiltemp	NH4		NO3		Root.Density	
number	 per.horizon 	Vol.%		kPa			°C			kg/ha	kg/ha	kg/dm3	
				
10011	$DatumStart	42	:Number of soil layers			
1	1	0.311	-99	10	14.1000	14.100	-99
2	1	0.311	-99	10	14.1000	14.100	-99
3	2	0.305	-99	10	14.2000	14.200	-99
4	2	0.32	-99	10	6.0500	6.0500	-99
5	2	0.409	-99	10	6.2500	6.2500	-99
6	2	0.409	-99	10	6.2500	6.2500	-99
7	2	0.409	-99	10	3.7500	3.7500	-99
8	2	0.398	-99	10	2.4000	2.4000	-99
9	4	0.398	-99	10	2.4000	2.4000	-99
10	4	0.361	-99	10	2.3600	2.3600	-99
11	4	0.361	-99	10	2.3600	2.3600	-99
12	4	0.361	-99	10	2.3600	2.3600	-99
13	4	0.361	-99	10	2.3600	2.3600	-99
14	4	0.361	-99	10	2.3600	2.3600	-99
15	4	0.361	-99	10	2.3600	2.3600	-99

==================================================================================================================
*WATER READINGS
Number of Events	Number of measurement depths (max. 5)
Measuring depths (cm)
Date	Water.cont	Matrix.Pot
Datum	Vol.%	kPa

10012	0	0
	
==================================================================================================================
*NITROGEN MEASUREMENTS
Number of Events	Number of measurement depths (max. 5)
Measuring depths (cm)
Date	Nitrat-N	Ammonium-N
Datum	kg/ha	kg/ha

10013	0	0
==================================================================================================================
*Plant Measurements I
Termin/	Entw.	BFI	Boden	Blatt	Trieb	Pflanzen	Wurzel	Verd.	Kum. N
Datum	Stad.	bedeck.	anz.	zahl	hoehe	tiefe	faktor	Aufnahme
DD.MM.YY EC 1/1	1/1	1/1	1/1	1/m2	cm		cm	1/1	kg/ha

10014	0	:Number of measurement dates
					
==================================================================================================================
*Plant Measurements II
-----------------Dry matter-----------------/------N-content in % the dry matter------

Termin	Blätter	Halme	Frucht	Wurzel	Obere	BlÃtter	Halme/	Frucht	Wurzel	Obere
Datum		Stengel		Biom.	N	Stengel	N	N	Biom.N
dd.mm.yy	kg/ha	kg/ha	kg/ha	kg/ha	kg/ha	%	%	%	%	%
-----------------------------------------------------------------------------------------------

10015	0	:Number of measurement dates
					
==================================================================================================================
***	Expert-N	Ende Eingabedatei BUONES AIRES	***
==================================================================================================================
