#include <stdlib.h>
#include <stdio.h> 
#include <math.h>

#define SQR(x) (x)*(x)
#define SWAP(a,b) {temp=(a);(a)=(b);(b)=temp;}

#define mPI 3.1415926535

#define MINRELSTEP 0.001
#define MAXITER 50
#define MAXANGLE 45

#define MINDELTA 1.0e-7
#define fDISPL (float)0.01


#define MAXNOPAR  20
#define MAXNODATA 1000

#define iTRUE !0
#define iFALSE 0

#define MAXPARNAMLENGTH 20

typedef char ParNamType[MAXPARNAMLENGTH];

typedef int (funcmodelcall)(float*, int, float*, float*, int);
typedef int (printdat)();
typedef int (printpar)();

typedef float fvec[MAXNOPAR];
typedef float fmat[MAXNOPAR][MAXNOPAR];
typedef int   ivec[MAXNOPAR];
					    

int		Cholesky(fmat, fvec, fvec, int);
int		CholeskyDecomp(fmat, fvec, int);
int		CholeskySolve(fmat,fvec,fvec,fvec,int);
int		matinv(fmat, fmat, int);
void	MarlevLog(char* s, float lamda, float fStepFrac, int inoiter);


int		marlev(float*, float*, float*, char**, int, float*, float*, float*, int, 
			   funcmodelcall*, printdat*);
			   /*
int		marlev(float*, char**, int, float*, float*, float*, int, 
			   funcmodelcall*);
				 */
int		_matherr( struct _exception *except );


FILE *marStream;
FILE *logStream;
FILE *datStream;

#define LogFilename     "marlev.log"
#define DATA_FILE_NAME	"marlev.dat"