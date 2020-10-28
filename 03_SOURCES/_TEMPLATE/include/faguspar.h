//Input-File für die Pflanzenparameter
//ExpertN-Einheiten

#define	EKL			2.0				//
#define	DRESP		0.03			//thickness of the respiring wood portion [m]
#define RADM		0.25			//final brest hight radius of the tree [m]
#define FBRAF		0.1				//final branch fraction [-]

#define	LEXT		0.65			//leaf layer related extinction coefficient
#define	TOPT	   18.0				//optimal temperature for photosynthesis [°C]
#define	TPO		   -2.0				//minimal temperature for photosynthesis [°C]
#define	CPZ			(float)340.0	//kg(C)/m^3

#define SPRELF		(float)1.4		//Faktoren für Erhaltungsatmung
#define SPREFR		(float)1.4
#define SPREWD		(float)0.2
#define SPRERT		(float)1.0
#define	GRSP		(float)1.2		//Faktor für Wachstumsatmung [kg(C)/kg(ASSI C)]
#define TNORM	    (float)25.0
#define TO		    (float)-3.0

#define ASRLOC		(float)0.2
#define NIRLOC		(float)0.2
#define STWLSR		(float)0.001
#define BGRLSR		(float)0.008
#define FRTLSR		(float)1.0

#define TREE0	    (float)1344.0	//initial number of trees [trees/ha]
#define STWD0	    (float)60000.0	//initial stemwood [kg(C)/ha]
#define AGE0	    (float)60.0		//initial age of stand [yr]
#define LEAF0		(float)0.0
#define FRUT0		(float)0.0
#define ASSI0		(float)5200.0	//initial assimalte pool [kg(C)/ha]

#define DETASSI0	1000.0		//Anfangsmasse der das Blattwachstum initiierenden Assimilate [kg(C)/ha]
#define POTFRU0 	80.0		//Anfangsmasse der das Fruchtwachstum initiierenden Assimilate [kg(C)/ha]
#define TTM1	  130.0	
#define TTM2	  645.0
#define FOTFAC		1.0
#define FELLPAR		1.0

#define CLFN1		(float)0.094	//kritische Blatt-N-Konzentration, Stadium 1, N/C
#define CLFN2		(float)0.054    //kritische Blatt-N-Konzentration, Stadium 2, N/C
#define CLFN3		(float)0.048	//kritische Blatt-N-Konzentration, Stadium 3, N/C
#define CLFN4		(float)0.03		//N-Konzentration in frischer Blattstreu, N/C
#define CRTN		(float)0.03		//kritische Feinwurzel-N-Konzentration, N/C

#define CBTN		(float)0.007	//kritische Äste/Zweige-N-Konzentration, N/C
#define CSTN		(float)0.0033	//kritische Stammholz-N-Konzentration, N/C
#define CGRTN		(float)0.0055	//kritische Grobwurzel-N-Konzentration, N/C
#define NPOOL0		(float)250.0	//initial N pool [kg(N)/ha]


#define DECRT		(float)0.015
#define DECRTFR		(float)0.015
#define DMOD		(float)1.8




#define RWUMX		(float)0.03	//maximum water uptake by roots in a layer [cm^3(Water)/cm(Root)]