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
1	1		1		49.204		-0.453		67
*LastName	FirstName	Street
ROTS	Département Calvados	 Region Normandie	
*PLZ	City		Tel.	Fax.
14980	Root		-99		-99
=============================================================================
*Plot Data
10002
*PlotID	Name			Size	Num.Subplots
1		Arvalis Rots	1		1
=============================================================================
=============================================================================
*Subplot data
10003
*ID	Name	Princ.Crop	Weatherfile	Parameterfile		ConfigFile
1	$Name	Crop		$xnw			$xnm			$xnc
*AreaNum.	Size	Slope	Exposition
1		1	0	S
=============================================================================
=============================================================================
*Plant Data
*Harvest Residues of preceding crop (before simulation starts)

Fruchtart	Ertrag	Ernte-		Ernte-		C/N Ernte-	Wurzel-		C/N Wurzel-
			termin	rückstände	rückstände	rückstände	rückstände
			kg/ha	Datum		kg/ha		1/1			kg/ha		1/1
10004
WH 		7000	$DatumStart		1000		80			2000		21
===================================================================================================================================
*Cultivated Crop
Nr.	Bez.Variety	Sowing	Row-	Seed-	Plant	Emerge	Harvest	Date	max.Root.	max. Root.	Removal
	Art	name	depth	space	density	Sowing	Date	Date	max.	Date		Depth		Yes=1
										Biom.					No=0
	Code		m		m		seed/m2	Datum	Datum	Datum	Datum	Datum		m

10005	1	:Number of crops
1	WH	Apache-WW	0.03	0.17	300	    $Saattermin 	$TerminAuflaufen	   $TerminErnteNutzung	-99		-99			1.2			0
===================================================================================================================================
MANAGEMENT DATA
===================================================================================================================================
*Mineral Fertilization (Pro Massnahme)
Date	Fertilizer	Fertilizer	N		NO3-N	NH4-N	Amid-N
Datum	Name		Code		kg/ha	kg/ha	kg/ha	kg/ha

10006	$no_min_fert	:Number of MINERAL Fertilizer Applications
$min_fert_table
=============================================================================
*Organic Fertilization (Pro Massnahme)
Date	Org_fertilizer	Org_fertilizer	DM	N	NH4-N	Org.Sub.
Datum	Name		Code		kg/ha	kg/ha	kg/ha	kg/ha

10007	0	:Number of ORGANIC Fertilizer Applications
=============================================================================
*Soil Management
Date	Depth	Equip.			Equip.
Datum	m	Name			Code

10008	0	:Number of soil cultivation activities
=============================================================================
*Irrigation (Pro MaÃŸnahme)
Date	Amount	Irrig
Datum	mm	Code

10009	0	:Number of Irrigation applications
		
==================================================================================================================
*SIMULATIONS LAYERS (equidistant)
	Num.layers	Num.Horizons
	(10,100)	(1 cm, 50 cm)

Layer	Num.Layers	Clay	Silt	Sand	Organ.	Bulk.Density	Stone.cont	pH
number	per.horizon	wgt.%	wgt.%	wgt.%	wgt.%	kg/dm3		    Vol.%		1

10010	30		5						
1		5			12.00	65.00	21.4	1.70	1.35		-99		-99
2		13			15.00	65.00	19.8	0.30	1.350		-99		-99
3		12			25.00	55.00	20.0	0.10	1.600		-99		-99
							
==================================================================================================================								
*START VALUES								
	 DateSampling 							
								
Layer	 Num.Layers 	WaterCont	MatrixPot	Soiltemp	NH4		NO3		Root.Density	
number	 per.horizon 	Vol.%		kPa			 °C			kg/ha	kg/ha	kg/dm3	
								
10011	$DatumStart	30	:Number of soil layers					
1		5				-99		-99				-99			-99	-99	-99	
2		13				-99		-99				-99			-99	-99	-99	
3		12				-99		-99				-99			-99	-99	-99	

==================================================================================================================
*WATER READINGS
Number of Events	Number of measurement depths (max. 5)
Measuring depths (cm)
Date	Water.cont	Matrix.Pot
Datum	Vol.%		kPa

10012	0	0
		
==================================================================================================================
*NITROGEN MEASUREMENTS
Number of Events	Number of measurement depths (max. 5)
Measuring depths (cm)
Date	Nitrat-N	Ammonium-N
Datum	kg/ha		kg/ha

10013	0	0
==================================================================================================================
*Plant Measurements I
Termin/		Entw.	BFI	Boden	Blatt	Trieb	Pflanzen	Wurzel	Verd.	Kum. N
Datum		Stad.		bedeck.	anz.	zahl	hoehe		tiefe	faktor	Aufnahme
DD.MM.YY EC 1/1		1/1	1/1		1/1		1/m2	cm			cm		1/1		kg/ha

10014	0	:Number of measurement dates
									
==================================================================================================================
*Plant Measurements II
-----------------Dry matter-----------------/------N-content in % the dry matter------

Termin		Blätter	Halme	Frucht	Wurzel	Obere	BlÃtter	Halme/	Frucht	Wurzel	Obere
Datum			Stengel			Biom.	N	Stengel	N	N	Biom.N
dd.mm.yy	kg/ha	kg/ha	kg/ha	kg/ha	kg/ha	%	%	%	%	%
-----------------------------------------------------------------------------------------------

10015	0	:Number of measurement dates
										
==================================================================================================================
***	Expert-N Ende Eingabedatei	ROTS ***
==================================================================================================================
