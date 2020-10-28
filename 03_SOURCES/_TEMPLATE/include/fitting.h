#include <stdlib.h>
#include <stdio.h> 
#include <math.h>

#define SQR(x) ((x)*(x))
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

#define mPI 3.1415926535

#define MINRELSTEP 0.005
#define MAXITER 50
#define MAXANGLE 30

#define MINDELTA 1.0e-7
#define fDISPL (float)0.002


#define MAXNOPAR  20
#define MAXNODATA 3000

#define iTRUE !0
#define iFALSE 0

#define MAXPARNAMLENGTH 20

#define INPUTFILENAME   "c:\\expertn.32b\\fitting_I"
#define PARAMETERMARKER "PARAMETER"
#define LEVELMARKER		"LEVEL"
#define maxnoMatLayers		25
#define maxnoLevels			10
#define maxnoMeasureLayers	25
#define maxnoDates			600


typedef char ParNamType[MAXPARNAMLENGTH];

typedef int (funcmodelcall)(float*, int, float*, float*, int);
typedef int (printdat)(int,float*);
typedef int (printpar)(int,float*,float);

typedef float fvec[MAXNOPAR];
typedef float fmat[MAXNOPAR][MAXNOPAR];
typedef int   ivec[MAXNOPAR];

//#define MAR_OUTPUT_FILE_NAME "c:\\expertn.32b\\fitres\\marlev.out"

#define iFLUSSO 1
#define iFLUSSU 2
#define iFL_OU  3
#define iMENGE  4
#define iKONZ   5

					    
typedef struct 
{
	char	acName[20];
	int		iDim;			//iDim = 0 => tats‰chl. Dimension Levelabh‰ngig, 
	int		iFittingFlag;
	float   fMinVal;
	float   fMaxVal;
	float	afValueInSection[maxnoMatLayers];
	float   afValueInMatLayer[maxnoMatLayers];
	float   afMeasureValueInMatLayer[maxnoMatLayers];
	char    aacNumberednames[maxnoMatLayers][20];
	int		aiMARLEVIndex[maxnoMatLayers];
	int		iMARLEVdataIndex;
} Parameter;

typedef struct
{
	int	inoSections;
	int ainoLayersInSection[maxnoMatLayers];
} Level;

typedef struct
{
	char    acName[20];
	int		used;
	int		iKind; // 1=Menge, 2=Konzentration, 3=kum. Fluﬂ unten, 4=kum Fluﬂ oben
	float	afMeasuredepth[maxnoMeasureLayers];
	int		aiNumLayer[maxnoMeasureLayers];
	float   afFracLayer[maxnoMeasureLayers];
	long	alDates[maxnoDates];
	float	aafMeasureValues[maxnoDates][maxnoMeasureLayers];
	float   aafSimValues[maxnoDates][maxnoMeasureLayers];
	int		aaiMARLEVIndex[maxnoDates][maxnoMeasureLayers];
	int		inoMeasureLayers;
	int		inoDates;
	int		iActDateIndex;
	float   fCumValueO; //nur ben¸tzt, falls Fluﬂvariable, da dann kumulativ
	float   fCumValueU;
	int		iCumDays;
} MeasureVariable	 ;

int		Cholesky(fmat, fvec, fvec, int);
int		CholeskyDecomp(fmat, fvec, int);
int		CholeskySolve(fmat,fvec,fvec,fvec,int);
int		matinv(fmat, fmat, int);
void	MarlevLog(char* s, float lamda, float fStepFrac, int inoiter);


int marlev(float *afpar, float *afparmin, float *afparmax, char **asnam, int inopar, 
		   float *afx_data, float *afy_data, float *af_weights,int inodata,
		   funcmodelcall *model, printdat *prdat, printpar *prpar);

int simann(float *afpar, float *afparmin, float *afparmax, char **asnam, int inopar, 
		   float *afx_data, float *afy_data, float *af_weights,int inodata,
		   funcmodelcall *model, printdat *prdat, printpar *prpar);





int		_matherr( struct _exception *except );

FILE *marStream;
FILE *logStream;

#define LogFilename     "marlev.log"
