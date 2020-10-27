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
1	1	-39.80	-73.25	19
*LastName	FirstName	Street
ROTS	Département Calvados	 Region Valdivia	
*PLZ	City	Tel.	Fax.
5090000	Valdivia	-99	-99
=============================================================================
*Plot Data
10002
*PlotID	Name		Size	Num.Subplots
1	Valdivia	1	1
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
WH		-99		230808		1000		74				1000		16
===================================================================================================================================
*Cultivated Crop
Nr.	Bez.	Variety	Sowing	Row-	Seed-	Plant	Emerge	Harvest	Date	max.Root.	max. Root.	Removal
	Art 	name	depth	space	density	Sowing	Date	Date	max.	Date		Depth	Yes=1
									Biom.			No=0
	Code			m		m		seed/m2	Datum	Datum	Datum	Datum	Datum		m

10005	1	:Number of crops
1 WH	Bacan-WW	0.03	0.16	350	$Saattermin 	$TerminAuflaufen	   $TerminErnteNutzung	-99		-99			1.2			0
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
10009	16	:Number of Irrigation applications
220908	20	 IR004		0	0	0	0	0	0
061008	19	 IR004		0	0	0	0	0   0
171008	19	 IR004		0	0	0	0	0   0
311008	20	 IR004		0	0	0	0	0   0
141108	19	 IR004		0	0	0	0	0   0
181108	20	 IR004		0	0	0	0	0   0
271108	20	 IR004		0	0	0	0	0   0
021208	19	 IR004		0	0	0	0	0   0
061208	22	 IR004		0	0	0	0	0   0
111208	21	 IR004		0	0	0	0	0	0
141208	21	 IR004		0	0	0	0	0	0
181208	19	 IR004		0	0	0	0	0   0
261208	23	 IR004		0	0	0	0	0   0
311208	21	 IR004		0	0	0	0	0	0
030109	19	 IR004		0	0	0	0	0   0
080109	20	 IR004		0	0	0	0	0   0

==================================================================================================================
*SIMULATIONS LAYERS (equidistant)
	Num.layers	Num.Horizons
	(10,100)	(1 cm, 50 cm)

Layer	Num.Layers	Clay	Silt	Sand	Organ.	Bulk.Density	Stone.cont	pH
number	per.horizon	wgt.%	wgt.%	wgt.%	wgt.%	kg/dm3	    Vol.%	1

10010	42	5			
1	1	10.4	68.8	20.8	14	0.58	0	5.8
2	1	5.9	72.7	21.4	7.85	0.73	0	4.8
3	2	5.9	72.7	21.4	7.85	0.73	0	4.8
4	2	1	56.1	42.9	2.96	0.97	0	5.4
5	2	1	56.1	42.9	2.96	0.97	0	5.4
6	2	1	42.7	56.3	1.71	1.13	0	5.6
7	2	1	42.7	56.3	1.71	1.13	0	5.6
8	2	1	42.7	56.3	1.3	1.17	0	5.6
9	4	3.6	36.7	59.7	1.3	1.22	0	5.8
10	4	4.7	35	60.3	1.3	1.23	0	5.8
11	4	4.7	35	60.3	1.3	1.23	0	5.8
12	4	2.2	29	68.8	1.3	1.23	0	5.8
13	4	2.2	29	68.8	1.3	1.27	0	5.8
14	4	2.2	29	68.8	1.3	1.27	0	5.8
15	4	2.2	29	68.8	1.3	1.27	0	5.8

==================================================================================================================				
*START VALUES				
	 DateSampling 				
				
Layer	 Num.Layers 	WaterCont	MatrixPot	Soiltemp	NH4		NO3		Root.Density	
number	 per.horizon 	Vol.%		kPa			°C			kg/ha	kg/ha	kg/dm3	
				
10011	$DatumStart	42	:Number of soil layers			
1	1	52	-99	10	8.7000	8.7000	-99
2	1	52	-99	10	10.9500	10.950	-99
3	2	52	-99	10	14.6000	14.600	-99
4	2	47	-99	10	9.7000	9.7000	-99
5	2	47	-99	10	9.7000	9.7000	-99
6	2	48	-99	10	5.6500	5.6500	-99
7	2	48	-99	10	5.6500	5.6500	-99
8	2	45	-99	10	2.3400	2.3400	-99
9	4	45	-99	10	2.4400	2.4400	-99
10	4	45	-99	10	2.4600	2.4600	-99
11	4	45	-99	10	2.4600	2.4600	-99
12	4	45	-99	10	2.4600	2.4600	-99
13	4	45	-99	10	2.5400	2.5400	-99
14	4	45	-99	10	2.5400	2.5400	-99
15	4	45	-99	10	2.5400	2.5400	-99
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
***	Expert-N	Ende Eingabedatei VALDIVIA	***
==================================================================================================================
