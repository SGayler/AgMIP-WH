/*********************************************************************************
 *
 *   H2O_FCT.C
 *   Material functions of soil
 *
 *   ch, 4.11.95  WaterCapacity, SoilDeclination
 *   dm, 11.4.96  New variables 
 *   ep, 27.11.95, 12.4.96, 1.10.96 , 15.04.02 Adding new hydraulic functions
 *   ep, 14.3.03, 15.7.04, 3.8.04 Adding new functions: PTF v. Teepe, ZD_PotIm
 ********************************************************************************/

#include "xinclexp.h"
#include "xh2o_def.h"


// from util_fct.c
extern float  abspower(float, float);
extern double abspowerDBL(double, double);


double BC_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double BC_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double BC_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double BC_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

double BG_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double BG_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double BG_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double BG_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

double HC_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double HC_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double HC_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double HC_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

double CH_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double CH_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double CH_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double CH_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

double VG_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double VG_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double VG_DWCap(double,double, double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double VG_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

double M2_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double M2_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double M2_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double M2_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

double BetaI(double,double,double);
double Beta(double,double);
double BetaCF(double,double,double);
double GammLN(double);

double C2_WCont(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double C2_HCond(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double C2_DWCap(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);
double C2_MPotl(double,double,double,double,double,double,double,double,double,double,
                double,double,double,double,double,double,double,double,double,double);

double DDummy(double,double,double,double,double,double,double,double,double,double,
              double,double,double,double,double,double,double,double,double,double);

double FDummy(double,double,double,double,double,double,double,double,double,double,
              double,double,double,double,double,double,double,double,double,double);

double MDummy(double,double,double,double,double,double,double,double,double,double,
              double,double,double,double,double,double,double,double,double,double);

int pegasus(double,double,double,double,double,double,double,double,double,double,
            double,double,double,double,double,double,double,double,double,double
            ,double*,double*,double*,int*);

double MDummy2(double,double,double,double,double,double,double,double,double,double,
              double,double,double,double,double,double,double,double,double,double);

int pegasus2(double,double,double,double,double,double,double,double,double,double,
            double,double,double,double,double,double,double,double,double,double
            ,double*,double*,double*,int*);

double ZD_PotIm(double,double,double,double,double,double,double,double,double,double,
              double,double,double,double,double,double,double,double,double,double);


float WaterCapacity(EXP_POINTER);

/* PedoTransferFunctions */
double PedoTransferCampbell(double,double,double,double,double,double,int);
double PedoTransferVereecken(double,double,double,double,double,double,int);
double PedoTransferRawlsBrakensiek(double,double,double,double,double,double,int);
double PedoTransferScheinost(double,double,double,double,double,double,int);
double PedoTransferScheinostBimodal(double,double,double,double,double,double,int);
double PedoTransferTeepe(double,double,double,double,double,double,int);




/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BC_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double BC_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double RP=(double)1.0;    /* pore interaction parameter  */
    double f1=(double)0.0,f2=(double)0.0,f3=(double)1.0;

    if (Hakt <= Ca)
    {
     f1 = ((double)2.0 + (((double)2.0 + RP)/Cb));
     f2 = Ksat * (double)abspowerDBL((Ca/Hakt),f1);
    }
    else 
    {
     f2 = Ksat;
    }
     f2=max(f2,(double)1e-300);

  return f2;
}  /*  end      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BC_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BC_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   double  f1=(double)0.0,f2=(double)0.0;
    f1 = min(Hmin,Hakt);

    if ((f1-Ca) <= (double)0.0)
    {
     f2 = Tmin+(Tsat-Tmin)* (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
    }
    else
    {
     f2 = Tsat;
    }
    
  return f2;
}  /*  end  */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BC_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.6.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BC_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double  f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;

    f1 = min(Hmin,Hakt);

    if ((f1 - Ca) <= (double)0.0)
     {
      f2 = (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
      f2 *= ((double)-1.0/(Cb*f1)); 
      f2 *= (Tsat-Tmin);
      f2=max(f2,(double)1e-37); 
     }
    else /*f1-Ca > 0.0 */
     {
      f2 = (double)0.0;
     }

  return f2;
}  /*  end   */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : BC_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *             according to Brooks and Corey (1964)       
 *  Author   : ep 19.10.93, ep 25.02.97
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double BC_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   
  double  f1=(double)0.0,f2=(double)0.0;

  f1 = (Takt-Tmin)/(Tsat-Tmin);

  f2 = Ca * ((double)abspowerDBL(f1,((double)-1.0 * Cb)));
      
  f2=max(f2,(double)-1e37);
      
  return f2;
}  /*  end MPotl      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BG_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             according to Gardner (1958)                */
/*                                                        */
/*  Author   : ep 15.04.02                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double BG_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double GA=Alpha2;
    double GB=N2;
    double f1=(double)0.0,f2=(double)0.0,f3=(double)1.0;

    Hakt *=(double)0.1; // [mm] to [cm]
    
    if (Hakt <= (double)0.0)
    {
    f1 = (double)1.0 + (double)abspowerDBL((-Hakt * GA),GB);
    f2 = Ksat/f1;
    }
    else 
    {
    f2 = Ksat;
    }
    
    f2=max(f2,(double)1e-300);

  return f2;
}  /*  end      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BG_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             according to Brutsaert (1966)              */
/*                                                        */
/*  Author   : ep 15.04.02                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BG_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 double n,Alfa,Qr,Qs,Qm,Qee,Qees,Hs,HH,FQ;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      //Alfa=(double)10*Alpha;
      n=N;
      Qm=Tsat;

      //Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL((double)1/Qees-(double)1,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         Qee=(double)1/((double)1+abspowerDBL(-Alfa*HH,n));
         FQ=max(Qr+(Qm-Qr)*Qee,(double)1e-37);
        }
      else
        {
         FQ=Qs;
        }
          
 return FQ;
}  /*  end van Genuchten water content  */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : BG_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             according to Brutsaert (1966)              */
/*                                                        */
/*  Author   : ep 15.04.02                                */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double BG_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 double n,Alfa,Qr,Qs,Qm,Qees,Hs,HH,C1,C2,FC;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      //Alfa=(double)10*Alpha;
      n=N;
      Qm=Tsat;

      //Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL((double)1/Qees-(double)1,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         C1=abspowerDBL((double)1+abspowerDBL(-Alpha*HH,n),-(double)2);
         C2=(Qm-Qr)*n*abspowerDBL(Alpha,n)*abspowerDBL(-HH,n-(double)1)*C1;
         FC=max(C2,(double)1e-300);
        } 
      else
        {
         FC=(double)0;
        } 

      //FC=(double)0.1 * FC; //[cm] to [mm]

 return FC;
}  /* end van Genuchten  DWC*/

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : BG_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *             according to Brutsaert (1966)      
 *  Author   : ep 15.04.02
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double BG_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 double n,Alfa,Qr,Qs,Qm,Qe,Qee,QeeM,HMin,HH,FH;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      //Alfa=(double)10*Alpha;
      n=N;
      Qm=Tsat;
      Qe=(Takt-Tmin)/(Tsat-Tmin);

      HMin=-abspowerDBL((double)1e300,((double)1/n));
      QeeM=abspowerDBL((double)1+abspowerDBL(-Alpha*HMin,n),-(double)1);
      Qee=min(max(Qe*(Qs-Qr)/(Qm-Qr),QeeM),(double).999999999999999);
      HH=(double)-1/Alpha*abspowerDBL(abspowerDBL(Qee,(double)-1)-(double)1
                                     ,(double)1/n); 
      FH=max(HH,(double)-1e37);
      //FH=(double)10.0 * FH; //[cm] to [mm]
 return FH;
}  /*  end VGMPotl  */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : HC_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             after Hutson und Cass (1992) LEACHN        */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double HC_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double RP=(double)1.0;    /* pore interaction parameter  */
    double f1,f2,f3;

    if (Hakt <= Hc)
    {
     f1 = ((double)2.0 + (((double)2.0 + RP)/Cb));
     f2 = Ksat * (double)abspowerDBL((Ca/Hakt),f1);
    }
    else 
    {
///*
     f1 =((HC_WCont(Hakt,Takt,Ksat,Tsat,Tmin,Alpha,N,M,Ca,Cb,
                    Hc,Tc,Hmin,Hvor,Tvor,Alpha2,N2,M2,W1,W2)-Tmin)/(Tsat-Tmin));
//*/
//     f1 = (Takt-Tmin)/(Tsat-Tmin); 
     f3 =  (double)2.0*Cb + (double)2.0 + RP;  /* ==Cb(2.0+((2.0+RP)/Cb)) */
     f2 = Ksat * (double)(abspowerDBL(f1,f3));
    }    
     f2=max(f2,(double)1e-300);

  return f2;
}  /*  end      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : HC_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             after Hutson und Cass (1992) LEACHN        */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double HC_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   double  f1=(double)0.0,f2=(double)0.0;
    f1 = min(Hmin,Hakt);

    if ((f1-Hc) <= (double)0.0)
    {
     f2 = Tmin + (Tsat-Tmin) * (double)(abspowerDBL(f1/Ca,(double)-1.0/Cb));
    }
    else
    {
     f2 =((Tsat-Tmin)*((double)1.0 - (Tc-Tmin)/(Tsat-Tmin))*f1*f1);
     f2 =(f2/((Ca*Ca)*((double)(abspowerDBL((Tc-Tmin)/(Tsat-Tmin),(double)-2.0*Cb)))));
     f2 = Tsat - f2;
    }
    
  return f2;
}  /*  end  */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : HC_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             after Hutson und Cass (1992) LEACHN        */
/*                                                        */
/*  Author   : cs 22.6.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double HC_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double  f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;

    f1 = min(Hmin,Hakt);

    if ((f1 - Hc) <= (double)0.0)
     {
      f2 = (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
      f2 *= ((double)-1.0*Tsat/(Cb*f1));
      f2=max(f2,(double)1e-37); 
     }
    else /*f1-Hc > 0.0 */
     {
      f3 = Ca*Ca*((double)abspowerDBL((Tc/Tsat),((double)-2.0*Cb)));
      f2 = (double)-2.0*Tsat*((double)1.0-(Tc/Tsat))*f1/f3;

     }

  return f2;
}  /*  end   */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : HC_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *  Author   : ep 19.10.93, ep 25.02.97
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double HC_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   
  double  f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;
  f1 = Takt/Tsat;

  if (Takt <= Tc)
    {
     f3 = Ca * ((double)abspowerDBL(f1,((double)-1.0 * Cb)));
    }
  else //if (Takt > Tc)
    { 
     f2 = Tc/Tsat;
     f3 =  (double)sqrt((double)((double)1.0 - f1)) * Ca;
     f3 *= (double)(abspowerDBL(f2,((double)-1.0 * Cb)));
     f3 /=((double)sqrt((double)((double)1.0 - f2)));
    }
      
  f3=max(f3,(double)-1e37);
      
  return f3;
}  /*  end MPotl      */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_HCond()                                            */
/*                                                                   */
/*  Function : Calculation of hydraulic unsaturated conductivity     */
/*             following van Genuchten (1980) and Mualem (1976)      */
/*             HYDRUS 5.0                                            */
/*  Author   : ep 27.11.95                                           */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
///*

//SG 20111025: Für Pareto-Analyse:
// Bisher wurde der Parameter "p" (="l") des Van-Genuchten-Mualem-Modells 
// fix auf 1/2 gesetzt. Um diesen Parameter zum Anpassen der Leitfähigkeitskurve 
// verwenden zu können, wird Ppar nun anstelle von VanGenM übergeben. 
// ==> in *.xnm muss "p" (="l") in die Spalte für VanGenM geschrieben werden.
// Das van Genuchten "m" bleibt 1-1/n, "r" ist immer = 1!

//!!! Was passiert, wenn kein *.xnm-file verwendet wird? !!!

//double VG_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
//                double Alpha, double N, double M, double Ca, double Cb,
//                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
//                double Alpha2, double N2, double M2, double W1, double W2)
double VG_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double Ppar, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)
{
 //Hydraulic conductivity
 double m,n,Alfa,Qe,Qee,Qees,Qs,Qr,Qm,Hs,HH;
 double FFQ,FFQs,Kr,Ks,FK;
//SG 20111025: Ppar wird anstelle von VanGenM übergeben 
// double Ppar=(double)0.5; 
 double QPar=(double)1.0;
 double Rpar=(double)2.0;

      Qr=Tmin;
      Qs=Tsat;
      Qm=Tsat;
      Alfa=Alpha;
      n=N;
      m=(double)1-(double)1/n;
      Ks=Ksat;
// SG 20111025: für Pareto-Analyse wird Ppar nun anstelle von VanGenM übergeben	  
//	  if(M>=(double)1)Rpar=M;

//      Hakt=(double) 0.1 * Hakt; // [mm] to [cm]
            
      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL(abspowerDBL(Qees,(double)-1/m)-(double)1
                            ,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         Qee=abspowerDBL((double)1+abspowerDBL(-Alfa*HH,n),-m);
         Qe=(Qm-Qr)/(Qs-Qr)*Qee;
         FFQ =(double)1-abspowerDBL((double)1-abspowerDBL(Qee,(double)1/m),m);
         FFQs=(double)1-abspowerDBL((double)1-abspowerDBL(Qees,(double)1/m),m);
         if(FFQ<=(double)0) FFQ=m*abspowerDBL(Qee,(double)1/m);
         Kr=abspowerDBL(Qe,Ppar)*abspowerDBL(FFQ/FFQs,Rpar);
         FK=max(Ks*Kr,(double)1e-300);
        }  
      else
        {
         FK=Ks;
        }
 return FK;

}  /*  van Genuchten/Mualem conductivity */



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_HCond()                                            */
/*                                                                   */
/*  Function : Calculation of hydraulic unsaturated conductivity     */
/*             following van Genuchten (1980) and Mualem (1976)      */
/*             and RETC (van Genuchten et al. 1991) for general m    */
/*  Author   : ep 1.12.96 modified 28.03.03                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
double VG_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{
 double alfa,n,m, u,v,se,s,t;
 double K=(double)0.0;
 double p,q,r;

 p = (double) 0.5;//0.5;//Mualem p=0.5, Burdine p=2
 q = (double) 0.5;//0.5;//1;//vanGenuchten-Mualem q=1, vanGenuchten-Burdine q=2
 r = (double) 2;//3;//Mualem r=2, Burdine r=1

 alfa = Alpha;
 n = N;
 m = M;
 //m=(double)1-(double)1/n;

// Hakt=(double)0.1 * Hakt; // [mm] to [cm]
 
 if (Hakt < (double)0.0)
 {
   u  = m + (double)q/n;
   v  = (double)1 - (double)q/n;// v>0 i.e. q/n<1 i.e. n>q !!!

   se = abspowerDBL(((double)1 + abspowerDBL((alfa * -Hakt),n)),-m);
   se = min(se,(double)1);
   
   s  = (double)1/((double)1 + abspowerDBL((alfa * -Hakt),n));
   t  = BetaI(u,v,s);
   
   K = Ksat * abspowerDBL(se,p) * abspowerDBL(t,r);
  }
  else
  {
   K = Ksat;
  }
   
 return K;
 
}  /*  van Genuchten/Mualem conductivity */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_WCont()                                            */
/*                                                                   */
/*  Function : Calculation of volumetric water content               */
/*             following van Genuchten (1980) and HYDRUS 5.0         */
/*                                                                   */
/*  Author   : ep  27.11.95                                          */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
               
double VG_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)
{
 double m,n,Alfa,Qr,Qs,Qm,Qee,Qees,Hs,HH,FQ;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      n=N;
      Qm=Tsat;

//      Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

      m=(double)1-(double)1/n;
      //m=M;
      
      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL(abspowerDBL(Qees,(double)-1/m)-(double)1
                                     ,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         Qee=abspowerDBL((double)1+abspowerDBL(-Alfa*HH,n),-m);
         FQ=max(Qr+(Qm-Qr)*Qee,(double)1e-37);
        }
      else
        {
         FQ=Qs;
        }
          
 return FQ;
}  /*  end van Genuchten water content  */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_DWCap()                                          */
/*                                                                 */
/*  Function : Calculation of differential water capacity          */
/*             following van Genuchten (1980) and HYDRUS 5.0       */
/*  Author   : ep 27.11.95                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double VG_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 double m,n,Alfa,Qr,Qs,Qm,Qees,Hs,HH,C1,C2,FC;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      n=N;
      Qm=Tsat;

//      Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

      m=(double)1-(double)1/n;
      //m=M;
      
      Qees=min((Qs-Qr)/(Qm-Qr),(double).999999999999999);
      Hs=(double)-1/Alfa*abspowerDBL(abspowerDBL(Qees,(double)-1/m)-(double)1
                            ,(double)1/n); 
      if(Hakt<Hs)
        {
         HH=max(Hakt,-abspowerDBL((double)1e300,(double)1/n));
         C1=abspowerDBL((double)1+abspowerDBL(-Alpha*HH,n),-m-(double)1);
         C2=(Qm-Qr)*m*n*abspowerDBL(Alpha,n)*abspowerDBL(-HH,n-(double)1)*C1;
         FC=max(C2,(double)1e-300);
        } 
      else
        {
         FC=(double)0;
        } 

//      FC=(double)0.1 * FC; //[cm] to [mm]

 return FC;
}  /* end van Genuchten  DWC*/




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : VG_MPotl()                                          */
/*  Function : Calculation of matric potential from                */
/*             a given volumetric water content                    */
/*             following van Genuchten (1980) and HYDRUS 5.0       */
/*                                                                 */
/*  Autor    : ep, 12.4.96                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double VG_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
 double m,n,Alfa,Qr,Qs,Qm,Qe,Qee,QeeM,HMin,HH,FH;

      Qr=Tmin;
      Qs=Tsat;
      Alfa=Alpha;
      n=N;
      Qm=Tsat;
      Qe=(Takt-Tmin)/(Tsat-Tmin);

      m=(double)1-(double)1/n;
      //m=M;
      
      HMin=-abspowerDBL((double)1e300,((double)1/n));
      QeeM=abspowerDBL((double)1+abspowerDBL(-Alfa*HMin,n),-m);
      Qee=min(max(Qe*(Qs-Qr)/(Qm-Qr),QeeM),(double).999999999999999);
      HH=(double)-1/Alfa*abspowerDBL(abspowerDBL(Qee,(double)-1/m)-(double)1
                                     ,(double)1/n); 
      FH=max(HH,(double)-1e37);
//      FH=(double)10.0 * FH; //[cm] to [mm]
 return FH;
}  /*  end VGMPotl  */




/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : CH_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double CH_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double RP=(double)1.0;    /* pore interaction parameter  */
    double f1=(double)0.0,f2=(double)0.0,f3=(double)1.0;

    if (Hakt <= Ca)
    {
     f1 = ((double)2.0 + (((double)2.0 + RP)/Cb));
     f2 = Ksat * (double)abspowerDBL((Ca/Hakt),f1);
    }
    else 
    {
     f2 = Ksat;
    }
     f2=max(f2,(double)1e-300);

  return f2;
}  /*  end      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : CH_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.5.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double CH_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   double  f1=(double)0.0,f2=(double)0.0;
    f1 = min(Hmin,Hakt);

    if ((f1-Ca) <= (double)0.0)
    {
     f2 = Tmin+(Tsat-Tmin)* (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
    }
    else
    {
     f2 = Tsat;
    }
    
  return f2;
}  /*  end  */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : CH_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             according to Brooks and Corey (1964)       */
/*                                                        */
/*  Author   : cs 22.6.92, ep 23.11.95                    */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double CH_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double  f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;

    f1 = min(Hmin,Hakt);

    if ((f1 - Ca) <= (double)0.0)
     {
      f2 = (double)(abspowerDBL((f1/Ca),((double)-1.0/Cb)));
      f2 *= ((double)-1.0/(Cb*f1)); 
      f2 *= (Tsat-Tmin);
      f2=max(f2,(double)1e-37); 
     }
    else /*f1-Ca > 0.0 */
     {
      f2 = (double)0.0;
     }

  return f2;
}  /*  end   */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : CH_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *             according to Brooks and Corey (1964)       
 *  Author   : ep 19.10.93, ep 25.02.97
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double CH_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   
  double  f1=(double)0.0,f2=(double)0.0;

  f1 = (Takt-Tmin)/(Tsat-Tmin);

  f2 = Ca * ((double)abspowerDBL(f1,((double)-1.0 * Cb)));
      
  f2=max(f2,(double)-1e37);
      
  return f2;
}  /*  end MPotl      */



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : C2_HCond()                                 */
/*                                                        */
/*  Function : Calculation of                             */
/*             unsaturated hydraulic conductivity         */
/*             according to bimodal Campbell              */
/*                                                        */
/*  Author   : ep 6.10.97                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


double C2_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double RP=(double)0.5;    /* pore interaction parameter  */
    double f0=(double)0.0,f7=(double)0.0,f8=(double)0.0;
    double f1=(double)0.0,f2=(double)0.0,f3=(double)0.0;
    double f4=(double)0.0,f5=(double)0.0,f6=(double)0.0;
    
    Takt=C2_WCont(Hakt,Takt,Ksat,Tsat,Tmin,Alpha,N,M,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,Alpha2,N2,M2,W1,W2);

    W1=Tsat;
//    f8=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0* Ca)));
//    Ksat=(double)0.1*Ksat;
    f8=(double)-40.0;
    if (Hakt > f8)
    {
     f0 = Ksat;
    }
    else if ((Hakt <= f8)&&(Hakt > Hc))
    {
     f2 = (double)abspowerDBL((Tc/W1),(Cb+(double)1.0));
     f3 = (double)abspowerDBL((Tc/W1),(Ca+(double)1.0));
     f4 = f2/(Hc*(Cb+(double)1.0))+((double)1.0-f3)/((double)-40.0*(Ca+(double)1.0));
     f5 = f4 * f4;
     f6 = (double)abspowerDBL((Takt/W1),(Ca+(double)1.0));
     f7 = f2/(Hc*(Cb+(double)1.0))+(f6-f3)/((double)-40.0*(Ca+(double)1.0));
     f0 = Ksat * (double)abspowerDBL((Takt/W1),RP)/f5 * f7 *f7;
    }
    else //if (Hakt <= Hc)
    {
     f1 = ((double)2.0 + (((double)2.0 + RP)/Cb));
     f2 = (double)abspowerDBL((Tc/W1),(Cb+(double)1.0));
     f3 = (double)abspowerDBL((Tc/W1),(Ca+(double)1.0));
     f4 = f2/(Hc*(Cb+(double)1.0))+((double)1.0-f3)/((double)-40.0*(Ca+(double)1.0));
     f5 = f4 * f4;
     f6 = (double)2.0 * Cb + (double)2.0 + RP;
     f7 = (double)abspowerDBL((Tc/W1),f6)/f5;
     f0 = Ksat * f7 * (double)abspowerDBL((Hc/Hakt),f1)/((Hc*(Cb+(double)1.0))*(Hc*(Cb+(double)1.0)));
     f0 = Ksat * (double)abspowerDBL((Takt/W1),f6)/f5/((Hc*(Cb+(double)1.0))*(Hc*(Cb+(double)1.0)));
    }
 
    f0=max(f0,(double)1e-300);

  return f0;
}  /*  end      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : C2_WCont()                                 */
/*                                                        */
/*  Function : Calculation of volumetric water content    */
/*             for a given matric potential               */
/*             according to bimodal Campbell              */
/*                                                        */
/*  Author   : ep 6.10.97                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double C2_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   double  f1=(double)0.0,f2=(double)0.0;
    double  Hb=(double)0.0;

    f1 = min(Hmin,Hakt);
    Hb=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0* Ca)));
//    Hb=(double)-40.0;
    Hb= min(Hmin,Hb);

    if (f1 > Hb)
    {
     f2=Tsat;
//     f2=W1;
    }
    else if ((f1 <= Hb)&&(f1 > Hc))
    {
//     f2 = Tsat * (double)(abspowerDBL((f1/(double)-40.0),((double)-1.0/Ca)));
     f2 = W1 * (double)(abspowerDBL((f1/(double)-40.0),((double)-1.0/Ca)));
    }
    else
    {
//     f2 = Tsat * (double)(abspowerDBL((f1/Hc),((double)-1.0/Cb)));
     f2 = Tc * (double)(abspowerDBL((f1/Hc),((double)-1.0/Cb)));
    }
    
  return f2;
}  /*  end  */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : C2_DWCap()                                 */
/*                                                        */
/*  Function : Calculation of differential water capacity */
/*             according to bimodal Campbell              */
/*                                                        */
/*  Author   : ep 6.10.97                                 */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double C2_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{
    double  f1=(double)0.0,f2=(double)0.0;
	double Hb=(double)0.0;

    f1 = min(Hmin,Hakt);
    Hb=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0* Ca)));
//    Hb=(double)-40.0;
	Hb= min(Hmin,Hb);
    if (f1 > Hb)
     {
      f2 = (double)0.0;
     }
//    else if ((f1 <= (double)-40.0)&&(f1 > Hc))
    else if (f1 > Hc)
     {
      f2 = (double)(abspowerDBL((f1/(double)-40.0),((double)-1.0/Ca)));
      f2 *= ((double)-1.0/(Ca*f1)); 
      f2 *= W1;
      f2=max(f2,(double)1e-37); 
     }
    else //f1 <= Hc 
     {
      f2 = (double)(abspowerDBL((f1/Hc),((double)-1.0/Cb)));
      f2 *= ((double)-1.0/(Cb*f1)); 
      f2 *= Tc;
      f2=max(f2,(double)1e-37); 
     }

  return f2;
} //  end   

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 *  Name     : C2_MPotl()
 *  Function : Calculation of matric potential
 *             from volumetric water content
 *             according to Bimodal Campbell       
 *  Author   : ep 6.10.97
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
double C2_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha, double N, double M, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double W1, double W2)

{   
  double  f1=(double)0.0,f2=(double)0.0;
  double Hb=(double)0.0;

  Hb=(double)-40.0 * (double)(abspowerDBL((Tsat/W1),((double)-1.0 * Ca)));
//  Hb=(double)-40.0;
  Hb= min(Hmin,Hb);

  if (Takt >= Tsat)
  {
   f2 = Hb;
  }
  else if (Takt > Tc)
  {
   f2 = (double)-40.0 * ((double)abspowerDBL(Takt/W1,((double)-1.0 * Ca)));
  }
  else  
  {
   f2 = Hc * ((double)abspowerDBL(Takt/Tc,((double)-1.0 * Cb)));
  } 
      
  f2=max(f2,(double)-1e37);
      
  return f2;
}  /*  end MPotl      */



/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   19.10.93                                                                     */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double DDummy(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
              double Alpha, double N, double M, double Ca, double Cb,
              double Hc, double Tc, double Hmin, double Hvor, double Tvor,
              double Alpha2, double N2, double M2, double W1, double W2)

{ double Dakt=(double)0.0;


 /*      Dakt=  (  WasserGehalt(Hakt,Hvor,Tsat,Ca,Cb,Tc,Hmin)

                 - WasserGehalt(Hvor,Hvor,Tsat,Ca,Cb,Tc,Hmin)  */


 /*      Dakt=  ( VGWCont(Hakt,Alpha,N,Tmin,Tsat,Ksat,ddummy1,ddummy2,ddummy3)

              - VGWCont(Hvor,Alpha,N,Tmin,Tsat,Ksat,ddummy1,ddummy2,ddummy3)

              )/DELT;  */


  return Dakt;
}  /*  end      */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   19.10.93                                                                     */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double FDummy (double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
               double Alpha, double N, double M, double Ca, double Cb,
               double Hc, double Tc, double Hmin, double Hvor, double Tvor,
               double Alpha2, double N2, double M2, double W1, double W2)

{ 
  double Fakt=(double)0.0; 
  double d0=(double)1, d1=(double)1;

  Fakt=Tvor-Takt;

  return Fakt;
}  /*  Ende      */




/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
double M2_WCont(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{ 
  double WG = (double)0.0;
  double q = (double)0;//0;//vanGenuchten-Mualem q=1, vanGenuchten-Burdine q=2
  double em1,em2,se;
  
//   Hakt=(double) 0.1 * Hakt; // [mm] to [cm]

   em1 = (double)1 - q/N1;
   em2 = (double)1 - q/N2;

   se = Weight1 * abspowerDBL(((double)1 + abspowerDBL((Alpha1 * -Hakt),N1)),-em1)+
        Weight2 * abspowerDBL(((double)1 + abspowerDBL((Alpha2 * -Hakt),N2)),-em2);

 if (Hakt < (double)0.0)
 {
   WG = Tmin + (Tsat - Tmin) * se;
 }
 else
 {
   WG = Tsat;
 }
  return WG;

}  /*  Ende WasserGehalt      */


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  ep 1.10.96 & 21.3.03                                                          */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

///*
double M2_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{
 double tll,ttl,se,s1,s2,em1,em2,t1,t2;
 double K=(double)0;
 double p,q,r;

 p = (double) 0.5;//0;//Mualem p=0.5, Burdine p=2
 q = (double) 0;//0.25;//0.5;//vanGenuchten-Mualem q=1, vanGenuchten-Burdine q=2
 r = (double) 2;//3;//Mualem r=2, Burdine r=1

// Hakt=(double)0.1 * Hakt; // [mm] to [cm]
 
 if (Hakt < (double)0)
 {
   em1 = (double)1 - q/N1;
   em2 = (double)1 - q/N2;
   
   s1 = abspowerDBL(((double)1 + abspowerDBL((Alpha1 * -Hakt),N1)),-em1);
   s2 = abspowerDBL(((double)1 + abspowerDBL((Alpha2 * -Hakt),N2)),-em2);

   se  = Weight1 * s1 + Weight2 * s2;
   se  = min(se,(double)1);
      
   if (s1<=(double)0) 
   {t1 = em1 * abspowerDBL(s1,((double)1/em1));}
   else
   {t1 = (double)1-abspowerDBL(((double)1-abspowerDBL(s1,((double)1/em1))),em1);}

   if (s2<=(double)0) 
   {t2 = em2 * abspowerDBL(s2,((double)1/em2));}
   else
   {t2 = (double)1-abspowerDBL(((double)1-abspowerDBL(s2,((double)1/em2))),em2);}

   tll = Weight1 * Alpha1 * t1 + Weight2 * Alpha2 * t2;
   ttl = Weight1 * Alpha1 + Weight2 * Alpha2;

   K = Ksat * abspowerDBL(se,p) * abspowerDBL(tll/ttl,r);
  }
  else
  {
   K = Ksat;
  }
   
 return K;
 
}  /*  Bimodale van Genuchten  Leitf*/


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  ep 1.10.96 & 28.3.03                                                          */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*
double M2_HCond(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{
 double tll,ttl,se,s1,s2,em1,em2,p1,p2,q1,q2;
 double K=(double)0.0;
 double p,q,r;

 p = (double) 0.5;//0.5;//Mualem p=0.5, Burdine p=2
 q = (double) 0.5;//0.5;//vanGenuchten-Mualem q=1, vanGenuchten-Burdine q=2
 r = (double) 2;//3;//Mualem r=2, Burdine r=1


// Hakt=(double)0.1 * Hakt; // [mm] to [cm]
 
 if (Hakt < (double)0.0)
 {
   em1 = (double)M1;
   em2 = (double)M2;
   p1  = em1 + (double)q/N1;
   p2  = em2 + (double)q/N2;
   q1  = (double)1 - (double)q/N1;
   q2  = (double)1 - (double)q/N2;
   se  = Weight1 * abspowerDBL(((double)1 + abspowerDBL((Alpha1 * -Hakt),N1)),-em1)+
         Weight2 * abspowerDBL(((double)1 + abspowerDBL((Alpha2 * -Hakt),N2)),-em2);
   se  = min(se,(double)1.0);
   s1  = (double)1/((double)1 + abspowerDBL((Alpha1 * -Hakt),N1));
   s2  = (double)1/((double)1 + abspowerDBL((Alpha2 * -Hakt),N2));

   tll = Weight1 * Alpha1 * em1 * BetaI(p1,q1,s1) * Beta(p1,q1) + 
         Weight2 * Alpha2 * em2 * BetaI(p2,q2,s2) * Beta(p2,q2);
   ttl = Weight1 * Alpha1 * em1 * Beta(p1,q1) + Weight2 * Alpha2 * em2 * Beta(p2,q2);
   
   //K = Ksat * abspowerDBL(se, (double) 0.5) * tll/ttl*tll/ttl;
   K = Ksat * abspowerDBL(se,p) * abspowerDBL(tll/ttl,r);
  }
  else
  {
   K = Ksat;
  }
   
 return K;
 
}  /*  Bimodale van Genuchten  Leitf*/




 /* Beta-Funktionen */
 
 double BetaI(double A, double B, double X)
 {
  double BT,BTI;
  
  if(X<=(double)0) 
  {
    X=(double)0;
  }
  
  if(X>=(double)1) 
  {
    X=(double)1;
  }

  if((X<=(double)0)||(X>=(double)1)) 
  {
    BT=(double)0;
  }
  else
  {
    BT=exp(GammLN(A+B)-GammLN(A)-GammLN(B)+A*log(X)+B*log((double)1-X));
  }
  
  if(X<((A+(double)1)/(A+B+(double)2)))
  {
    BTI=BT*BetaCF(A,B,X)/A;
  }
  else
  {
    BTI=(double)1-BT*BetaCF(B,A,(double)1-X)/B;
  }    
  
  return BTI;
 } /* BetaI */
 
 
 double BetaCF(double A, double B, double X)
 {             
  int i;
  int itmax=100;
  double BTCF;
  double eps=(double)3e-7;
  double am=(double)1, bm=(double)1, az=(double)1,azold=(double)0;
  double qab,qap,qam,bz,em,tem,d,ap,bp,app,bpp;
  
  qab=A+B;
  qap=A+(double)1;
  qam=A-(double)1;
  bz=(double)1-qab*X/qap;
  
  for(i=1;(i<itmax)&&(fabs(az-azold)>=eps*fabs(az));i++)
  {
   em=(double)i;
   tem=em+em;
   d=em*(B-em)*X/((qam+tem)*(A+tem));
   ap=az+d*am;
   bp=bz+d*bm;
   d=-(A+em)*(qab+em)*X/((A+tem)*(qap+tem));
   app=ap+d*az;
   bpp=bp+d*bz;
   azold=az;
   am=ap/bpp;
   bm=bp/bpp;
   az=app/bpp;
   bz=(double)1;
  }
  
  BTCF=az;
   
  return BTCF;
 } /* BetaCF */
 
  
 double Beta(double A,double B)
 {
  double BT;
 
  BT=exp(GammLN(A+B)-GammLN(A)-GammLN(B));
  
  return BT;
 } /* Beta */
 
 
 double GammLN(double Y)
 {
  double cf1=(double) 76.18009173;
  double cf2=(double)-86.50532033;
  double cf3=(double) 24.01409822;
  double cf4=(double)-1.231739516;
  double cf5=(double) .120858003e-2;
  double cf6=(double)-.536382e-5;
  
  double stp=(double)2.50662827465;
  double X,tmp,ser,GMLN;
  
  X=Y-(double)1;
  tmp=X+(double)5.5;
  tmp=(X+(double)0.5)*log(tmp)-tmp;
  ser=(double)1+cf1/(X+(double)1)+cf2/(X+(double)2)+cf3/(X+(double)3)+
      cf4/(X+(double)4)+cf5/(X+(double)5)+cf6/(X+(double)6);
  GMLN=tmp+log(stp*ser);
  
  return GMLN;
 } /* GammLN */
  
   

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   1.10.96                                                                      */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double M2_DWCap(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)
{
 double s1,s2,t1,t2,se,em1,em2;
 double Wakt,Wvor;
 double DWC=(double)0.0;
 double dd = (double) 1;
 double tolp = (double)0.01;
 
 double q = (double)0;//0;//vanGenuchten-Mualem q=1, vanGenuchten-Burdine q=2
 
 if (fabs(Hakt-Hvor) >= tolp)
 {
   Wakt = M2_WCont(Hakt,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
                   Alpha2,N2,M2,Weight1,Weight2);
   Wvor = M2_WCont(Hvor,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
                   Alpha2,N2,M2,Weight1,Weight2);
   DWC = (Wakt-Wvor)/(Hakt-Hvor);
 }
 else   
 {

//   Hakt=(double)0.1 * Hakt; // [mm] to [cm]
 
   if (Hakt < (double)0.0)
   {
     em1 = (double)1 - q/N1;
     em2 = (double)1 - q/N2;
   
     t1  = (double)1 + abspowerDBL((Alpha1*-Hakt),N1);
     t2  = (double)1 + abspowerDBL((Alpha2*-Hakt),N2);
   
     s1  = abspowerDBL((Alpha1 * - Hakt),(N1 - (double)1));
     s2  = abspowerDBL((Alpha2 * - Hakt),(N2 - (double)1));

     se = Weight1 * N1 * em1 * Alpha1 * abspowerDBL(t1,(-em1-(double)1)) * s1 +
          Weight2 * N2 * em2 * Alpha2 * abspowerDBL(t2,(-em2-(double)1)) * s2 ;

//   t  = (double)1 + abspowerDBL((Alpha * -Hakt),N);
//   s  = abspowerDBL((Alpha * - Hakt),(N - (double)1));
//   w  = abspowerDBL(((Tsat-Tmin)*t),((double)1/N -(double)2))
//        * (N - (double)1)* Alpha * s;

     DWC = (Tsat-Tmin) * se;
   }
   else
   {
     DWC = (double)1.0e-6;
   }
 
//   DWC=(double) 0.1 * DWC; 
 }

 return DWC;
}  /*  van Genuchten  DWC*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : M2_MPotl()                                          */
/*  Function : Determination of matric potential from given vol.   */
/*             water content using a zero search routine           */
/*                                                                 */
/*  Autor    : ep 19.03.03                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double M2_MPotl(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)

{
 double FH;
 double x1,x2,f2;
 int i1,iter;
 
 x1=0;
 x2=-(double)1e+9;
 
 i1=pegasus(Hakt,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
              Alpha2,N2,M2,Weight1,Weight2,&x1,&x2,&f2,&iter);
 
 //if((i1>=(int)0)&&(i1<(int)2))
 if(i1==0)
 {FH=*&x2;}
 else
 {FH=Hakt;}
 
 //FH=*&x2;
 return FH;
}  /*  end M2_MPotl  */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*  Name     : ZD_PotIm()                                          */
/*  Function : Determination of matric potential from given vol.   */
/*             water content using a zero search routine           */
/*                                                                 */
/*  Autor    : ep 19.03.03                                         */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double ZD_PotIm(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
                double Alpha1, double N1, double M1, double Ca, double Cb,
                double Hc, double Tc, double Hmin, double Hvor, double Tvor,
                double Alpha2, double N2, double M2, double Weight1, double Weight2)

{
 double FH;
 double x1,x2,f2;
 int i1,iter;
 
 x1=0;
 x2=-(double)1e+9;
 
 i1=pegasus2(Hakt,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
              Alpha2,N2,M2,Weight1,Weight2,&x1,&x2,&f2,&iter);
 
 //if((i1>=(int)0)&&(i1<(int)2))
 if(i1==0)
 {FH=*&x2;}
 else
 {FH=Hakt;}
 
 //FH=*&x2;
 return FH;
}  /*  end M2_MPotl  */

   
   
/*********************************************************************
 *                                                                   *
 *  Name     : WaterCapacity()                                       *
 *  Funktion : Berechnet Wasseraufnahmekapazität in den ersten       *
 *             ungesättigten Schichten.                              *
 *              ch 25.9.95                                           *
 *  nach:       C. Sperr   / 24.08.94                                *
 *                                                                   *
 *********************************************************************/
float WaterCapacity(EXP_POINTER)
{ 
  DECLARE_COMMON_VAR
  DECLARE_H2O_POINTER
  
  float  fThetaMax, fMinLeitf, WasserKap;
  int    SaettSchicht;
  float DeltaZ       = pSo->fDeltaZ;
   
  pWB = pWa->pWBalance; 
   
  fMinLeitf    = pSo->pSWater->fCondSat;
  WasserKap    = (float)0.0;
  SaettSchicht = pSo->iLayers;

  for (H2O_SOIL_LAYERS) // i= 1 bis n-1
      {
         // gibt die minimale gesättigte Leitfähigkeit in einer der Schichten an.
         fMinLeitf = min(fMinLeitf,pSW->fCondSat); 

         // Suche nach erster gesättigter Schicht oder nach einer Eis-haltigen Schicht:
         if (SaettSchicht > iLayer)
           {
             // Addiere Kapazität bis zur ersten ges. Schicht
             fThetaMax = (pWL->fIce < EPSILON) ?
                         WATER_CONTENT((float)0.999 * pSW->fMinPot)
                       :
                         // pSW->fContSat;
                         // Test !!! Keine Infiltration in Eishaltige Schicht
                         pWL->fContAct;
                         
             if (fThetaMax > pWL->fContAct)
                WasserKap += (fThetaMax - pWL->fContAct) * DeltaZ;
             else
                SaettSchicht = iLayer;
                                 

             if ((pSW->fCondSat < CONST_RAIN_RATE) || 
                 (pWL->fContAct >= (float)0.999 * pWL->fContMax)||
                 (pWL->fIce > EPSILON))
                 {
                   SaettSchicht = iLayer;
                 }
                   
           }   /* Saettschicht > iLayer */

       } // for
         
  return WasserKap;
} // WaterCapacity()

/************************************************************/
/* Name:       double PedoTransferCampbell()			    */
/*									                		*/
/* Function:   Calculates coefficients for Brooks&Corey-    */
/*             Campbell Soil Hydraulic Functions 		    */	
/*                                                          */
/* Reference:  CAMPBELL, G.S.: Soil Physics with Basic      */
/*             Elsevier, Amsterdam-Oxford-New York-Tokio,   */
/*             pp. 150, 1985                                */
/*                                   				        */
/* Authors:    mb 95, ep 02.97                              */  				
/************************************************************/

double PedoTransferCampbell(double dClayF, double dSiltF, double dSandF, 
                            double dBD, double dCarbonF, double dPS, int i)
{
 double dDIAMETER_LIMIT_CLAY = (double) 0.002; //0.002 [mm] acc. Campbell (1985) p.9
 double dDIAMETER_LIMIT_SILT = (double) 0.05;  //0.05  [mm] acc. Campbell (1985) p.9
 double dDIAMETER_LIMIT_SAND = (double) 2.0;   //2.0   [mm] acc. Campbell (1985) p.9

	double		dDG;
	double		dSigmaG;
	double		dSum;
    double      d1,d2,d3;
	double		dA;
	double		dB;
	double		dA1;
	double		dA2;
	double		dPsiEs;
	double		x;
	double		y;
	double		dFactorDay;

    double      dThsat=dPS;
    double      dThres=(double)0;
    double      dCampA; 
    double      dCampB; 
    double      dKsat; 
    double      dPTF; 

	/********************************************************************************/
	/* Mean geometric particle-size diameter: dg [mm]                               */
	/* Calculated according to:                                                     */
	/* SHIRAZI, M.A. & L. BOERSMA: A Unifying Quantitative Analysis of Soil Texture */
	/* In: Soil Sci. Soc. Am. J., Vol. 48, 1984, S. 142-147                         */
	/********************************************************************************/
	dSum   = (double)0;
	dA     = (double)0;
	dDG    = (double)0;
    d1     = log(((double)0.0 + dDIAMETER_LIMIT_CLAY)/(double)2);
    d2     = log((dDIAMETER_LIMIT_CLAY+dDIAMETER_LIMIT_SILT)/(double)2);
    d3     = log((dDIAMETER_LIMIT_SILT+dDIAMETER_LIMIT_SAND)/(double)2);
	dSum   = dClayF * d1 + dSiltF * d2 + dSandF * d3;
	dA     = (double)0.01 * dSum;
	dDG = exp(dA);

	/********************************************************************************/
	/* Geometric standard deviation of particle-size diameter: SigmaG               */
	/* Calculated according to:                                                     */
	/* SHIRAZI, M.A. & L. BOERSMA: A Unifying Quantitative Analysis of Soil Texture */
	/* In: Soil Sci. Soc. Am. J., Vol. 48, 1984, S. 142-147                         */
	/********************************************************************************/
	dSum    = (double)0;
	dB      = (double)0;
	dSigmaG = (double)0;
	dSum    = dClayF * d1 * d1 + dSiltF * d2 * d2 + dSandF * d3 * d3;
	dB      = sqrt((double)0.01 * dSum - (dA * dA));
	dSigmaG = exp(dB);


	/*********************************************************************/
	/* Campbell B from: dg and SigmaG                                    */
	/* Calculated according to:                                          */
	/* CAMPBELL, G.S.: Soil Physics with Basic                           */
	/* Elsevier, Amsterdam-Oxford-New York-Tokio, pp. 150, 1985          */
	/*********************************************************************/
	dB=(double)1 / sqrt(dDG) + ((double)0.2 * dSigmaG);
	dCampB = dB;


	/*********************************************************************/
	/* Air Entry Value aus:                                              */
	/* Calculated according to:                                          */
	/* Berechnung nach:                                                  */
	/* CAMPBELL, G.S.: Soil Physics with Basic                           */
	/* Elsevier, Amsterdam-Oxford-New York-Tokio, pp. 150, 1985          */
	/*********************************************************************/
	dPsiEs = (double)-0.5 * ((double)1 / sqrt(dDG));
	x = dBD / (double)1.3;
	y = (double)0.67 * dB;
	dCampA = (dPsiEs * pow(x, y));
	dCampA = dCampA * 102.5; // convert from [kPa] to [mm]


	/*********************************************************************/
	/* Saturated Coductivity Ksat                                        */
	/* Calculated according to:                                          */
	/* CAMPBELL, G.S.: Soil Physics with Basic                           */
	/* Elsevier, Amsterdam-Oxford-New York-Tokio, pp. 150, 1985          */
	/*********************************************************************/
	dA1 = (double)3.92 * pow((double)10, (double)-5);
	dA2 = (pow(((double)1.3/dBD), ((double)1.3 * dB)));
	dA = dA1 * dA2;
	dB = exp(((double)-6.9 * (dClayF/(double)100)) - ((double)3.7 * (dSiltF/(double)100)));
	/* (m/s) */
	dKsat = dA * dB;

	/* m/s -> mm/d */
	/* m = 1000 mm */
	/* s = d/(60*60*24) */
	dFactorDay = (double)1000 * (double)60 * (double)60 * (double)24;
	dKsat = dKsat * dFactorDay;

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=dCampA;
    else if (i==4)  dPTF=dCampB;
    else if (i==5)  dPTF=-(double)1/dCampA * (double)10; /* [1/mm] to [1/cm] */
	                /* van Genuchten alpha in [1/cm] */ 
    else if (i==6)  dPTF=(double)1/dCampB + (double)1;
    else if (i==7)  dPTF=(double)1-(double)1/((double)1/dCampB + (double)1);
    else            dPTF=dKsat;

	return dPTF;

} /* end PedoTransferCampbell() */


/************************************************************************/
/* Name:       double PedoTransferRawlsBrakensiek() 	                */
/*									                		            */
/* Function:   Calculates coefficients for van Genuchten                */
/*             and Brooks&Corey-Campbell Soil Hydraulic Functions 	    */
/*                                                                      */
/* Reference:  Rawls, W.J., Gish, T.J., & Brakensiek, D.L.              */
/*             Estimating the soil water retention from soil physical   */
/*             properties and characteristics                           */
/*             Advances in Soil Science 16, 213-234                     */
/*             Springer Verlag, New York, 1991                          */	        
/*                                                                      */
/* Authors:    mb 95, ep 02.97                                          */  				
/************************************************************************/

double PedoTransferRawlsBrakensiek(double dClayF, double dSiltF, double dSandF, 
                                   double dBD, double dCarbonF, double dPS, int i)

{
	double		d1;
	double		d2;
	double		d3;
	double		d4;
	double		d5;

    double      dCampA; 
    double      dCampB; 
    double      dThsat; 
	double      dThres;
	double		dPTF;


    /* saturated Theta */
    d1 = -(double)0.0015 * dSandF -(double)0.0022 * dClayF + (double)0.984 * dPS;
    d2 = (double)0.0001 * dClayF * dClayF +(double)0.0036 * dSandF * dPS -(double)0.011 * dClayF * dPS;
    d3 = -(double)0.0001 * dClayF * dClayF * dPS -(double)0.0024 * dPS * dPS * dSandF;
    d3 = d3 +(double)0.012 * dPS * dPS * dClayF;
    dThsat = (double)0.01162 + d1 + d2 + d3; 

    /* residual Theta */
	d1 = (double)0.0009 * dSandF +(double)0.005 * dClayF +(double)0.029 * dPS;
    d2 = -(double)0.00015 * dClayF * dClayF - (double)0.001 * dSandF * dPS;
    d3 = -(double)0.0002 * dClayF * dClayF * dPS * dPS;
    d4 = (double)0.0003 * dClayF * dClayF * dPS - (double)0.002 * dClayF * dPS * dPS;
	dThres = -(double) 0.018 + d1 + d2 + d3 + d4;

	/* Brooks-Corey bubbling pressure [mm]*/
	d1 = (double)0.185 * dClayF - (double) 2.484 * dPS;
	d2 = -(double)0.002*dClayF*dClayF-(double)0.044*dSandF*dPS-(double)0.6175*dClayF*dPS;
	d3 = (double)0.00144*dSandF*dSandF*dPS*dPS -(double)0.009*dClayF*dClayF*dPS*dPS;
	d4 = -(double)0.000013 *dSandF*dSandF*dClayF;
	d4 = d4 +(double)0.009*dClayF*dClayF*dPS -(double)0.0007*dSandF*dSandF*dPS;
	d4 = d4 +(double)0.000005*dClayF*dClayF*dSandF +(double)0.5*dPS*dPS*dClayF;
	d5 = (double)5.34 + d1 + d2 + d3 + d4;
	/* Log. nat. */
	d1 = exp((double)1);
	dCampA = -(double)10 * pow(d1, d5);	/* inverse logarithm & [cm] to [mm]*/

	/* Brooks-Corey pore size distribution index*/
	d1 = (double)0.018 * dSandF -(double)1.062 * dPS;
	d2 = -(double)0.00005*dSandF*dSandF -(double)0.003*dClayF*dClayF +(double)1.111*dPS*dPS;
	d2 = d2 -(double)0.031*dSandF*dPS; 
	d3 = (double)0.0003*dSandF*dSandF*dPS*dPS -(double)0.006*dClayF*dClayF*dPS*dPS;
    d3 = d3 -(double)0.000002 *dSandF*dSandF*dClayF +(double)0.008*dClayF*dClayF*dPS;
    d3 = d3 -(double)0.007*dPS*dPS*dClayF;
	d4 = -(double)0.784 + d1 + d2 + d3;
	/* Log. nat. */
	d1 = exp((double)1);
	dCampB = (double)1/pow(d1,d4);   /* entlogarithmiert */

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=dCampA;
    else if (i==4)  dPTF=dCampB;
    else if (i==5)  dPTF=-(double)1/dCampA * (double)10; /* [1/mm] to [1/cm] */
	                /* van Genuchten alpha in [1/cm] */
    else if (i==6)  dPTF=(double)1/dCampB + (double)1;
    else if (i==7)  dPTF=(double)1-(double)1/((double)1/dCampB + (double)1);
//    else            dPTF=dKsat;

	return dPTF;

} /* end PedoTransferRawlsBrakensiek() */


/************************************************************************/
/* Name:       double PedoTransferVereecken() 	                        */
/*									                		            */
/* Function:   Calculates coefficients for Brutsaert-Gardner            */
/*             Soil Hydraulic Functions 			            		*/	
/*                                                                      */
/* Reference:  Vereecken, H., Maes, J., Feyen, J. & Darius, P. (1989):  */
/*             Estimating the soil moisture retention characteristic    */
/*             from texture, bulk density and carbon content.           */
/*             Soil Science 148, 389-403.                      			*/	        
/*             Vereecken, H., Maes, J., Feyen, J. (1990): Estimating    */
/*             unsaturated hydraulic conductivity from easily measured  */
/*             soil properties.                                         */
/*             Soil Science 149, 1-12.                                  */
/*                                                                      */
/* Authors:    mb 95, ep 02.97                                          */  				
/************************************************************************/

double PedoTransferVereecken(double dClayF, double dSiltF, double dSandF, 
                             double dBD, double dCarbonF, double dPS, int i)

{
	double		d1;
	double		d2;
	double		d3;
	double		d4;
	double		d5;

	double      dVGA;
	double      dVGN; 
	double      dThsat;
	double      dThres;
	double		dPTF;
    double      dKsat;
    double      dGA;
    double      dGB;

    /* saturated Theta */
	d1 = (double)0.283 * dBD;
	d2 = (double)0.001 * dClayF;
    dThsat = (double)0.81 - d1 + d2;

    /* residual Theta */
	d1 = (double) 0.005 * dClayF;
    d2 = (double) 0.014 * dCarbonF;
	dThres = (double) 0.015 + d1 + d2;

	/* Van Genuchten Alpha */
	d1 = (double)0.025 * dSandF;
	d2 = (double)0.351 * dCarbonF;
	d3 = (double)2.617 * dBD;
	d4 = (double)0.023 * dClayF;
	d5 = (double)-2.486 + d1 - d2 - d3 - d4;
	/* Log. nat. */
	d1 = exp((double)1);
	dVGA = pow(d1, d5);		   /* entlogarithmiert */
    /* van Genuchten alpha in [1/cm] */
	//dVGA = (double)0.1 * dVGA; /* convert [1/cm] to [1/mm] */
    
	/* Van Genuchten N */
	d1 = (double)0.009 * dSandF;
	d2 = (double)0.013 * dClayF;
	d3 = (double)0.00015 * dSandF * dSandF;
	d4 = (double)0.053 - d1 - d2 + d3;
	/* Log. nat. */
	d1 = exp((double)1);
	dVGN = pow(d1, d4);   /* entlogarithmiert */

    /* Ksat */
    d1 = (double)0.96 * log(dClayF);
    d2 = (double)0.66 * log(dSandF);
    d3 = (double)0.46 * log(dCarbonF);
    d4 = (double)8.43 * dBD;
    d5 = (double)20.62 - d1 - d2 - d3 - d4;
	d1 = exp((double)1);
	dKsat = pow(d1, d5) * (double)10;   /* entlogarithmiert, [cm/d] to [mm/d] */

    /* Gardner A */
    d1 = (double)0.01877 * dSandF;
    d2 = (double)0.058 * dClayF;
    d3 = (double)-0.73 - d1 - d2;
	d1 = exp((double)1);
	dGA = pow(d1, d3);  /* entlogarithmiert, [1/cm] !!! */
    
    /* Gardner B */
    d1 = (double)0.194 * log(dClayF);
    d2 = (double)0.0489 * log(dSiltF);
    d3 = (double)1.186 - d1 - d2;
	d1 = exp((double)1);
	dGB = pow(d1, d3); /* entlogarithmiert */

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=-(double)1/dVGA;
    else if (i==4)  dPTF=(double)1/(dVGN-(double)1);
    else if (i==5)  dPTF=dVGA;
    else if (i==6)  dPTF=dVGN;
    else if (i==7)  dPTF=(double)1;
    else if (i==8)  dPTF=dKsat;
    else if (i==9)  dPTF=dGA;
    else if (i==10) dPTF=dGB;

	return dPTF;

} /* end PedoTransferVereecken() */

/************************************************************************/
/* Name:       double PedoTransferScheinost() 	                        */
/*									                		            */
/* Function:   Calculates coefficients for Brutsaert-Gardner            */
/*             Soil Hydraulic Functions 			            		*/	
/*                                                                      */
/* Reference:  Scheinost, A.C., Sinowski, W.,Auerswald, K. (1997):      */
/*             Regionalization of soil water retention curves in a      */
/*             highly variable soilscape, I. Developing a new pedo-     */
/*             transfer function. Geoderma 78, 129-143.                 */
/*                                                                      */
/*             Vereecken, H., Maes, J., Feyen, J. (1990): Estimating    */
/*             unsaturated hydraulic conductivity from easily measured  */
/*             soil properties. Soil Science 149, 1-12.                 */
/*                                                                      */
/*                                                                      */
/* Author:    ep 15.04.02                                               */  				
/************************************************************************/

double PedoTransferScheinost(double dClayF, double dSiltF, double dSandF, 
                             double dBD, double dCarbonF, double dPS, int i)

{
 double dDIAMETER_LIMIT_CLAY = (double)  0.002;//0.002 /* [mm] */
 double dDIAMETER_LIMIT_SILT = (double)  0.063;//0.063
 double dDIAMETER_LIMIT_SAND = (double)  2.0;//2.0
 double dDIAMETER_LIMIT_ROCK = (double) 63.0;//63.0

    double		d1;
	double		d2;
	double		d3;
	double		d4;
     
    double      dSum;
    double      dDG;
    double      dSigmaG;
    double      dRockF, d5, d6, dCL, dST, dSD, dRK;

	double      dVGA;
	double      dVGN; 
	double      dThsat;
	double      dThres;
	double		dPTF;
    double      dKsat;
    double      dGA;
    double      dGB;


	if (i<8)
    {
    /* dRockF from dBD used as input for rock fraction in [%] as for clay, silt and sand */
    dRockF=dBD;
    /*************************************************************************/
	/* Mean geometric particle-size diameter: DG [mm]                        */
	/*                                                                       */
    /* References: Shirazi,M.A., Boersma,L. Hart, W. (1988): A Unifying      */
    /*             Quantitative Analysis of Soil Texture: Improvement of     */
    /*             Precision and Extension of Scale                          */
    /*             Soil Sci.Soc.Am.J.,52, 181-190                            */
    /*                                                                       */
    /*             Sinowski, W. (1994): Die dreidimensionale Variabilität    */
    /*             von Bodeneigenscaften. Dissertation an der TU-München     */
    /*             FAM-Bericht 7, Shaker Verlag, Aachen.                     */
    /*                                                                       */
	/*************************************************************************/
    d5     = max((double)100,dClayF+dSiltF+dSandF+dRockF+dCarbonF);
    dCL    = dClayF/d5;
    dST    = dSiltF/d5;
    dSD    = dSandF/d5;
    dRK    = dRockF/d5;
	
    dSum   = (double)0;
	dDG    = (double)0;
    d1     = log(sqrt((double)4e-5*dDIAMETER_LIMIT_CLAY));
    d2     = log(sqrt(dDIAMETER_LIMIT_CLAY*dDIAMETER_LIMIT_SILT));
    d3     = log(sqrt(dDIAMETER_LIMIT_SILT*dDIAMETER_LIMIT_SAND));
    d4     = log(sqrt(dDIAMETER_LIMIT_SAND*dDIAMETER_LIMIT_ROCK));
	dSum   = dCL * d1 + dST * d2 + dSD * d3 + dRK * d4;
	d5  = dSum;
    dDG = exp(d5);

    /*************************************************************************/
	/* Geometric standard deviation of particle-size diameter: SigmaG        */
	/*                                                                       */
    /* References: Shirazi,M.A., Boersma,L. Hart, W. (1988): A Unifying      */
    /*             Quantitative Analysis of Soil Texture: Improvement of     */
    /*             Precision and Extension of Scale                          */
    /*             Soil Sci.Soc.Am.J.,52, 181-190                            */
    /*                                                                       */
    /*             Sinowski, W. (1994): Die dreidimensionale Variabilität    */
    /*             von Bodeneigenscaften. Dissertation an der TU-München     */
    /*             FAM-Bericht 7, Shaker Verlag, Aachen.                     */
    /*                                                                       */
	/*************************************************************************/
	dSum = (double)0;
	dSum   = dSum + dCL * d1 * d1;
	dSum   = dSum + dST * d2 * d2;
	dSum   = dSum + dSD * d3 * d3;
	dSum   = dSum + dRK * d4 * d4;
	d6 = sqrt(dSum - (d5 * d5));
	dSigmaG = exp(d6);

    /* saturated Theta */
	d1 = (double)0.85 * dPS;
	d2 = (double)0.13 * dCL;
    dThsat = d1 + d2;

    /* residual Theta */
	d1 = (double) 0.52 * dCL;
    d2 = (double) 1.6 * dCarbonF/(float)100;//weight [%] to [kg/kg]  
	dThres = d1 + d2;

    /* Van Genuchten Alpha */
    dVGA = (double)0.00025 + (double)0.0043 * dDG; /* van Genuchten alpha in [1/cm] */
	//dVGA = (double)0.1 * dVGA; /* convert [1/cm] to [1/mm] */
    
	/* Van Genuchten N */
	dVGN = (double)0.39 + (double)2.2/dSigmaG;
    
    }
    else
    {
    /* Ksat from dBD input used for Ksat in [mm/d] */
    dKsat = dBD;
    
    /* Gardner A */
    d1 = (double)0.019 * dSandF;
    d2 = (double)0.050 * dClayF;
    d3 = (double)0.506 * log((double)0.1 * dKsat); /* converted to [cm/d] */
    d4 = (double)-2.64 - d1 + d2 + d3;
	d1 = exp((double)1);
	dGA = pow(d1, d4);   /* entlogarithmiert, [1/cm] !!! */
    
    /* Gardner B */
    d1 = (double)0.194 * log(dClayF);
    d2 = (double)0.0489 * log(dSiltF);
    d3 = (double)1.186 - d1 - d2;
	d1 = exp((double)1);
	dGB = pow(d1, d3);   /* entlogarithmiert */
    }
    
    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=-(double)1/dVGA;
    else if (i==4)  dPTF=(double)1/(dVGN-(double)1);
    else if (i==5)  dPTF=dVGA; /* van Genuchten alpha in [1/cm] */
    else if (i==6)  dPTF=dVGN;
    else if (i==7)  dPTF=(double)1;
    else if (i==8)  dPTF=dKsat;
    else if (i==9)  dPTF=dGA;
    else if (i==10) dPTF=dGB;

	return dPTF;

} /* end PedoTransferScheinost() */


/************************************************************************/
/* Name:       double PedoTransferScheinostBimodal() 	                */
/*									                		            */
/* Function:   Calculates coefficients for bimodal                      */
/*             soil hydraulic functions 			            		*/	
/*                                                                      */
/* Reference:  Scheinost, A.C.                                          */
/*             Pedotransfer-Funktionen zum Wasser- und Stoffhaushalt    */
/*             einer Bodenlandschaft                                    */
/*             FAM-Bericht6, Verlag Shaker, Aachen 1995                 */
/*                                                                      */
/*                                                                      */
/* Author:    ep 15.03.03                                               */  				
/************************************************************************/

double PedoTransferScheinostBimodal(double dClayF, double dSiltF, double dSandF, 
                                    double dBD, double dCarbonF, double dPS, int i)

{
 double dDIAMETER_LIMIT_CLAY = (double)  0.002;//0.001;//0.001//0.002 /* [mm] */
 double dDIAMETER_LIMIT_SILT = (double)  0.063;//0.032;//0.026//0.063
 double dDIAMETER_LIMIT_SAND = (double)  2.0;//0.84;//1.025//2.0
 double dDIAMETER_LIMIT_ROCK = (double)  63.0;//4.0;//63.0

    double		d1, d2, d3, d4;
    double      dSum;
    double      dDG;
    double      dSigmaG;
    double      dRockF, d5, d6, dCL, dST, dSD, dRK, dCB;

	double      dVGA;
	double      dVGN; 
	double      dThsat;
    double      dThtex;
	double      dThres;
	double		dW2;
	double		dPTF;
    double      dKsat;

    /* dRockF from dBD used as input for rock fraction in [%] as for clay, silt and sand */
    dRockF=dPS;
    d5     = max((double)100,dClayF+dSiltF+dSandF+dRockF+dCarbonF);
    dCL    = dClayF/d5;
    dST    = dSiltF/d5;
    dSD    = dSandF/d5;
    dRK    = dRockF/d5;
    dCB    = dCarbonF/d5;
    dCB *= (double)1000;//weight [%] to [g/kg]

    /* saturated Theta */
    dThsat = (double)1 - (double)0.38 * dBD;

    /* textural saturated Theta */
    dThtex = (double)0.86 - 0.34 * dBD + (double)0.14 * dCL;//dClayF/(double)100;

    /* residual Theta */
	d1 = (double) 0.43 * dCL;
    d2 = (double) 0.0016 * dCB;
	//d1 = (double) 0.43 * dClayF/(double)100;
    //d2 = (double) 0.0016 * dCarbonF/(double)100;
	dThres = (double) 0.05 + d1 + d2;

    /*************************************************************************/
	/* Mean geometric particle-size diameter: DG [mm]                        */
	/*                                                                       */
    /* References: Shirazi,M.A., Boersma,L. Hart, W. (1988): A Unifying      */
    /*             Quantitative Analysis of Soil Texture: Improvement of     */
    /*             Precision and Extension of Scale                          */
    /*             Soil Sci.Soc.Am.J.,52, 181-190                            */
    /*                                                                       */
    /*             Sinowski, W. (1994): Die dreidimensionale Variabilität    */
    /*             von Bodeneigenscaften. Dissertation an der TU-München     */
    /*             FAM-Bericht 7, Shaker Verlag, Aachen.                     */
    /*                                                                       */
	/*************************************************************************/
    d5     = max((double)100,dClayF+dSiltF+dSandF+dRockF+dCarbonF);
    dCL    = dClayF/d5;
    dST    = dSiltF/d5;
    dSD    = dSandF/d5;
    dRK    = dRockF/d5;
	
    dSum   = (double)0;
	dDG    = (double)0;
    d1     = log10(sqrt((double)4e-5*dDIAMETER_LIMIT_CLAY));
    d2     = log10(sqrt(dDIAMETER_LIMIT_CLAY*dDIAMETER_LIMIT_SILT));
    d3     = log10(sqrt(dDIAMETER_LIMIT_SILT*dDIAMETER_LIMIT_SAND));
    d4     = log10(sqrt(dDIAMETER_LIMIT_SAND*dDIAMETER_LIMIT_ROCK));
	dSum   = dCL * d1 + dST * d2 + dSD * d3 + dRK * d4;
	d5  = dSum;
    dDG = pow((double)10,d5);

    /*************************************************************************/
	/* Geometric standard deviation of particle-size diameter: SigmaG        */
	/*                                                                       */
    /* References: Shirazi,M.A., Boersma,L. Hart, W. (1988): A Unifying      */
    /*             Quantitative Analysis of Soil Texture: Improvement of     */
    /*             Precision and Extension of Scale                          */
    /*             Soil Sci.Soc.Am.J.,52, 181-190                            */
    /*                                                                       */
    /*             Sinowski, W. (1994): Die dreidimensionale Variabilität    */
    /*             von Bodeneigenscaften. Dissertation an der TU-München     */
    /*             FAM-Bericht 7, Shaker Verlag, Aachen.                     */
    /*                                                                       */
	/*************************************************************************/
	dSum = (double)0;
	dSum   = dSum + dCL * d1 * d1;
	dSum   = dSum + dST * d2 * d2;
	dSum   = dSum + dSD * d3 * d3;
	dSum   = dSum + dRK * d4 * d4;
	d6 = sqrt(dSum - (d5 * d5));
	dSigmaG = pow((double)10,d6);

    /* Van Genuchten Alpha2 */
    dVGA = -(double)1.05 + (double)0.68*log10(dDG) - (double)0.023*dSigmaG -(double)0.5*dBD;
    dVGA = pow((double)10,dVGA); /* van Genuchten alpha in [1/cm] */
    
    //dVGA = -(double)1.05 + (double)0.68*log(dDG) - (double)0.023*dSigmaG -(double)0.5*dBD;
    //dVGA = exp(dVGA);
    
	/* Van Genuchten N2 */
	dVGN = (double)0.26 - (double) 0.17 * log10(dVGA) - (double)0.0059 * dSigmaG;
	//dVGN = (double)0.26 - (double) 0.17 * log(dVGA) - (double)0.0059 * dSigmaG;

    /* w2 */
    dW2 = (dThtex-dThres)/(dThsat-dThres);

    /* Ksat */
    dKsat = (double)3.1 + (double) 0.6 * log10(dDG);
    dKsat = pow((double)10,dKsat)*(double)10; /* entlogarithmiert, [cm/d] to [mm/d] */

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=dThtex;
    else if (i==4)  dPTF=dW2;
    else if (i==5)  dPTF=dVGA;
    else if (i==6)  dPTF=dVGN;    
    else if (i==8)  dPTF=dKsat;


	return dPTF;

} /* end PedoTransferScheinostBimodal() */


/************************************************************************/
/* Name:       double PedoTransferTeepe()    	                        */
/*									                		            */
/* Function:   Calculates coefficients for van Genuchten-Mualem         */
/*             Soil Hydraulic Functions 			            		*/	
/*                                                                      */
/* Reference:  Teepe, R., Dilling, H., Beese, F. (2003):                */
/*             Estimating water retention curves of forest soils        */
/*             from soil texture and bulk density                       */
/*             J. Plant Nutr. Soil Sci. 166, 111-119.                   */	        
/*                                                                      */
/* Authors:    ep/gsf 15.07.04                                          */  				
/************************************************************************/

double PedoTransferTeepe(double dClayF, double dSiltF, double dSandF, 
                             double dBD, double dCarbonF, double dPS, int i)

{
	double		d1;
	double		d2;
	double		d3;
	double		d4;
	double		d5;

	double      dVGA;
	double      dVGN; 
	double      dThsat;
	double      dThpwp;
	double      dThres;
	double		dPTF;

    /* saturated Theta */
	d1 = (double)0.36686 * dBD;
    dThsat = (double)0.9786 - d1;

    /* permanent wilting point Theta */
	d1 = (double) 0.021736 * sqrt(dClayF);
    d2 = (double) 0.000943 * dSandF;
	dThpwp = (double) 0.11125 + d1 - d2;

	/* Van Genuchten Alpha */
	d1 = (double)4.433 * dBD;
	d2 = (double)0.066 * dSandF/dBD + (double)3.683 * sqrt(dSandF) 
		+ (double)0.0016 * dSandF * dSandF - (double)1.8643 * log(dSandF);
	d3 = (double)0.002 * dSiltF * dSiltF + (double)0.0359 * dSiltF/dBD 
		+ (double)3.6916 * sqrt(dSiltF) - (double)1.575 * log(dSiltF);
	d4 = (double)0.47 * dClayF;
	d5 = (double)55.576 - d1 - d2 - d3 - d4;
	/* Log. nat. */
	d1 = exp((double)1);
	dVGA = pow(d1, d5);		   /* entlogarithmiert */
    /* van Genuchten alpha in [1/cm] */
	//dVGA = (double)0.1 * dVGA; /* convert [1/cm] to [1/mm] */
    
	/* Van Genuchten N */
	d1 = (double)0.00027395 * dSandF * dSandF;
	d2 = (double)0.01637 * dSiltF;
	d3 = (double)-2.8497 + d1 + d2;
	/* Log. nat. */
	d1 = exp((double)1);
	dVGN = pow(d1, d3) + (double)1;   /* entlogarithmiert */

    /* residual Theta */
    d1 = (double)164000;
	d2 = (double)1 - (double)1/dVGN;
	d3 = abspowerDBL((double)1+abspowerDBL(dVGA*d1,dVGN),d2);
    dThres = max((double)0,(d3 * dThpwp - dThsat)/(d3 - (double)1 + (double)1e-9));

    if      (i==1)  dPTF=dThsat;
    else if (i==2)  dPTF=dThres;
    else if (i==3)  dPTF=-(double)1/dVGA;
    else if (i==4)  dPTF=(double)1/(dVGN-(double)1);
    else if (i==5)  dPTF=dVGA;
    else if (i==6)  dPTF=dVGN;
    else if (i==7)  dPTF=d2;

	return dPTF;

} /* end PedoTransferTeepe() */




#define ITERMAX 300
#define ABSERR (float)0
#define RELERR (float)1e-6
#define FKTERR (float)1e-6

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   14.03.03                                                                     */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double MDummy (double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
               double Alpha1, double N1, double M1, double Ca, double Cb,
               double Hc, double Tc, double Hmin, double Hvor, double Tvor,
               double Alpha2, double N2, double M2, double Weight1, double Weight2)

{ 
  double Fakt=(double)0.0; 
  double d0=(double)1, d1=(double)1;

  Fakt=Takt-M2_WCont(Hakt,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
                           Alpha2,N2,M2,Weight1,Weight2);

  return Fakt;
}  /*  Ende      */




int pegasus(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
            double Alpha1, double N1, double M1, double Ca, double Cb,
            double Hc, double Tc, double Hmin, double Hvor, double Tvor,
            double Alpha2, double N2, double M2, double W1, double W2
            ,double *x1,double *x2,double *f2,int *iter)
{
 double f1,x3,f3,s12,fabs();
 double h1,h2;

 int res = 2;
 
 *iter = 0;
 h1=*x1;
 h2=*x2;
 
  f1=MDummy(h1,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
             Alpha2,N2,M2,W1,W2);
  *f2=MDummy(h2,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
             Alpha2,N2,M2,W1,W2);

if (f1*(*f2)>(double) 0) return (-1);

if (f1*(*f2)==(double) 0)
{
 if (f1==(double)0) {*x2=*x1; *f2=(double)0;}
 return(0);
}

while (*iter<=ITERMAX)
{
    (*iter)++;

    s12 = (*f2 - f1)/(*x2 - *x1);
    
    x3 = *x2 - *f2/s12;
    f3=MDummy(x3,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
        Alpha2,N2,M2,W1,W2);

    if (*f2 * f3 <= (double)0)
    {
        *x1=*x2;
        f1=*f2;
    }
    else
    {
        f1 *= *f2/(*f2 + f3);
    }

    *x2=x3;
    *f2=f3;

    if (fabs(*f2)<FKTERR)
    {
        res=(int)0;
        break;
    }

    if ( fabs(*x2-*x1) <= fabs(*x2) * RELERR + ABSERR )
    {
        res=(int)1;
        break;
    }

}//while
    
if (fabs(f1) <= fabs(*x2))
{
   *x2=*x1;
   *f2=f1;
}

return(res);
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*   14.03.03                                                                     */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

double MDummy2 (double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
               double Alpha1, double N1, double M1, double Ca, double Cb,
               double Hc, double Tc, double Hmin, double Hvor, double Tvor,
               double Alpha2, double N2, double M2, double Weight1, double Weight2)

{ 
  double Fakt=(double)0.0; 
  double KHakt,eps;

  eps=Tc;
  KHakt=M2_HCond(Hc,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
                           Alpha2,N2,M2,Weight1,Weight2);

  Fakt=eps*KHakt-M2_HCond(Hakt,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hc,Tc,Hmin,Hvor,Tvor,
                           Alpha2,N2,M2,Weight1,Weight2);

  return Fakt;
}  /*  Ende      */




int pegasus2(double Hakt, double Takt, double Ksat, double Tsat, double Tmin,
            double Alpha1, double N1, double M1, double Ca, double Cb,
            double Hc, double Tc, double Hmin, double Hvor, double Tvor,
            double Alpha2, double N2, double M2, double W1, double W2
            ,double *x1,double *x2,double *f2,int *iter)
{
 double f1,x3,f3,s12,fabs();
 double h1,h2;

 int res = 2;
 
 *iter = 0;
 h1=*x1;
 h2=*x2;
 
  f1=MDummy2(h1,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hakt,Tc,Hmin,Hvor,Tvor,
             Alpha2,N2,M2,W1,W2);
  *f2=MDummy2(h2,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hakt,Tc,Hmin,Hvor,Tvor,
             Alpha2,N2,M2,W1,W2);

if (f1*(*f2)>(double) 0) return (-1);

if (f1*(*f2)==(double) 0)
{
 if (f1==(double)0) {*x2=*x1; *f2=(double)0;}
 return(0);
}

while (*iter<=ITERMAX)
{
    (*iter)++;

    s12 = (*f2 - f1)/(*x2 - *x1);
    
    x3 = *x2 - *f2/s12;
    f3=MDummy2(x3,Takt,Ksat,Tsat,Tmin,Alpha1,N1,M1,Ca,Cb,Hakt,Tc,Hmin,Hvor,Tvor,
        Alpha2,N2,M2,W1,W2);

    if (*f2 * f3 <= (double)0)
    {
        *x1=*x2;
        f1=*f2;
    }
    else
    {
        f1 *= *f2/(*f2 + f3);
    }

    *x2=x3;
    *f2=f3;

    if (fabs(*f2)<FKTERR)
    {
        res=(int)0;
        break;
    }

    if ( fabs(*x2-*x1) <= fabs(*x2) * RELERR + ABSERR )
    {
        res=(int)1;
        break;
    }

}//while
    
if (fabs(f1) <= fabs(*x2))
{
   *x2=*x1;
   *f2=f1;
}

return(res);
}