// vim: set sw=4 sts=4 filetype=cpp fdm=marker et: 

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   Bs2PhiMuMu FITTER CODE (INSPIRED FROM k*+mu+mu- FITTER CODE)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//       Author: Niladri Sahoo <Niladri.Sahoo@cern.ch> 
//       Sec. Author: Deepak Kumar Sahoo <deepak.kumar.sahoo@cern.ch>
//       Created:   [N/A] 
// ------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <sys/stat.h>
#include <math.h>
#include <string.h>
#include <getopt.h> // passing unix-like arguments

#include <TSystem.h>
#include <TStyle.h>
#include <TF1.h>
#include <TF2.h>
#include <TH1.h>
#include <TH2.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TMath.h>
#include <TMinuit.h>
#include <TRandom3.h>
#include <TFile.h>
#include <TPad.h> 
#include <TCanvas.h> 
#include <TChain.h> 
#include <TChainElement.h>
#include <TPaveText.h>
#include <TLatex.h>
#include <TLine.h>
#include <TString.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TLorentzVector.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>

#include <RooConstVar.h>
#include <RooRealVar.h>
#include <RooAbsPdf.h>
#include <RooAddPdf.h>
#include <RooGaussian.h>
#include <RooBifurGauss.h>
#include <RooChebychev.h> 
#include <RooGenericPdf.h> 
#include <RooExponential.h>
#include <RooBernstein.h>
#include <RooPolynomial.h>
#include <RooPoisson.h>
#include <RooExtendPdf.h>
#include <RooProdPdf.h>
#include <RooDataHist.h>
#include <RooDataSet.h>
#include <RooAbsData.h>
#include <RooFitResult.h>
#include <RooPlot.h>
#include <RooAddition.h>
#include <RooProduct.h>
#include <RooMinuit.h>
#include <RooWorkspace.h>
#include <RooRandom.h>
#include <RooMultiVarGaussian.h>

#include "tools.h" 
#define PI 3.14159265358979;

using namespace std; 
using namespace RooFit;


//      1~6, 1~19 with peak regions excluded.


// Tags configration
bool redirectStdout = false;
bool is7TeVCheck = false; // Using 2011 efficiency map.
bool gKeepParam = true;
int isCDFcut = 4; // -1 for off, 1 for cdf, 2 for LHCb . 3 for 16Aug reOptimization
bool isToy=false; // For toy, no CDF cut is applied
TChain *ch=new TChain("tree");
TString plotpath="./plots";
TString iwspacepath=".";
TString iCombBkgWspacepath=".";
TString owspacepath=".";
TString idatacardpath=".";
TString odatacardpath=".";
TString ologpath=".";
double  scaleFactor=1.;
//Constants, Fit results for efficiency, etc.. //{{{
const int nSummaryBins = 2;
int summaryBin[nSummaryBins] = {7,8};
const int nQ2Ranges = 9;

char genQ2range[nQ2Ranges][128] = {"genQ2 <4.30 && genQ2 > 1.00",//Bin 0                                                                    
                                   "genQ2 < 8.68 && genQ2 > 4.30",//Bin 1                                                                      
                                   "genQ2 <10.09 && genQ2 > 8.68",//Jpsi (Bin 2)                                                         
                                   "genQ2 <12.86 && genQ2 >10.09",//Bin 3                                                                   
                                   "genQ2 <14.18 && genQ2 >12.86",//Psi2S (Bin 4)                                                              
                                   "genQ2 <16.00 && genQ2 >14.18",//Bin 5                                                                   
                                   "genQ2 <19.00 && genQ2 >16.00",//Bin 6                                                                      
                                   "genQ2 < 6.00 && genQ2 > 1.00",// Bin 7 Summary Bin 1
				   "(genQ2 <8.68 && genQ2 > 1.00) || (genQ2 > 10.09 && genQ2 < 12.86) || (genQ2 >14.18 && genQ2 <19.00)"};// Bin 7 Summary Bin 2   


char q2range[nQ2Ranges][128] = {"Q2 <4.30 && Q2 > 1.00",//Bin 0                                                                        
				"Q2 < 8.68 && Q2 > 4.30",//Bin 1                                                                      
				"Q2 <10.09 && Q2 > 8.68",//Jpsi (Bin 2)                                                                      
				"Q2 <12.86 && Q2 >10.09",//Bin 3                                                                       
				"Q2 <14.18 && Q2 >12.86",//Psi2S (Bin 4)                                                                     
				"Q2 <16.00 && Q2 >14.18",//Bin 5                                                                       
				"Q2 <19.00 && Q2 >16.00",//Bin 6                                                                      
				"Q2 < 6.00 && Q2 > 1.00",// Bin 7 Summary Bin 1                                                        
				"(Q2 <8.68 && Q2 > 1.00) || (Q2 > 10.09 && Q2 < 12.86) || (Q2 >14.18 && Q2 <19.00)"};//Bin 8 Summary Bin 2

char q2rangeLatex[nQ2Ranges][32] = {" 1.00 < q^{2} < 4.30",
                                    " 4.30 < q^{2} < 8.68",
                                    " 8.68 < q^{2} <10.09",
                                    "10.09 < q^{2} <12.86",
                                    "12.86 < q^{2} <14.18",
                                    "14.18 < q^{2} <16.00",
                                    "16.00 < q^{2} <19.00",
                                    " 1.00 < q^{2} < 6.00",
				    " 1.00 < q^{2} <19.00"};

double q2rangedn[nQ2Ranges] = {1.00 , 4.30 , 8.68  , 10.09 , 12.86 , 14.18 , 16.00 , 1.00 , 1.00};
double q2rangeup[nQ2Ranges] = {4.30 , 8.68 , 10.09 , 12.86 , 14.18 , 16.00 , 19.00 , 6.00 , 19.00};
char nTriggeredPath[3][32] = {"Triggers == 0", "Triggers == 1", "Triggers >= 1"};

const double brangedn[6] = {4.76,4.76,5.38,5.18,4.76,5.38};
const double brangeup[6] = {5.80,5.18,5.80,5.38,5.18,5.80};

char phiMassWindow[3][512] ={"Phimass >0",
			      "Phimass > 1.01 && Phimass < 1.03",
			      "Phimass > 1.00 && Phimass < 1.04"
                             };

char mumuMassWindow[11][512] = { "Mumumass > 0",
				 "(Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-3.5*Mumumasserr)",
				 "(Mumumass < 3.096916+3*Mumumasserr && Mumumass > 3.096916-5*Mumumasserr) || (Mumumass < 3.686109+3*Mumumasserr && Mumumass > 3.686109-3*Mumumasserr)",
				 "(Mumumass*Mumumass<8.68 && Bmass-Mumumass>2.182+0.16 && Bmass-Mumumass<2.182-0.16) || (Mumumass*Mumumass>10.09 && Mumumass*Mumumass<12.86 && Bmass-Mumumass>1.593+0.06 && Bmass-Mumumass<1.593-0.06) || (Mumumass*Mumumass>14.18 && Bmass-Mumumass>1.593+0.06 && Bmass-Mumumass<1.593-0.06)",
				 "(Mumumass*Mumumass < 8.68 && ( Bmass-Mumumass < 2.182+0.16 || Bmass-Mumumass > 2.182-0.16)) || (Mumumass*Mumumass>8.68 && Mumumass*Mumumass<10.09) || (Mumumass*Mumumass > 10.09 && Mumumass < 12.86 && ( Bmass-Mumumass < 1.593+0.06 || Bmass-Mumumass > 1.593-0.06)) || (Mumumass*Mumumass>12.86 && Mumumass*Mumumass<14.08) || (Mumumass*Mumumass > 14.08 && (Bmass-Mumumass > 1.593-0.06 || Bmass-Mumumass < 1.593+0.06))",
				 "Mumumass > 0",
				 "Mumumass > 0",
				 "Mumumass > 0",
				 "Mumumass > 0",
				 "abs(Bmass-Mumumass-2.270)>0.170 && abs(Bmass-Mumumass-1.681)>0.080",
                                 "abs(Bmass-Mumumass-2.270)>0.170 && abs(Bmass-Mumumass-1.681)>0.080"
};//None,Signal,Background(Jpsi,Psip),none,none, none, none, none, none, optimization(Anti-radiation), optimization(Anti-radiation)

double genFl    [nQ2Ranges]={0.759  , 0.644  , 0.517  , 0.446  , 0.393  , 0.365  , 0.340  , 0.746  , 0.526};
double genFlerr [nQ2Ranges]={0.001024  , 0.000921  , 0.001565  , 0.001090  , 0.001569  , 0.001378  , 0.001378  , 0.000842  , 0.000472};
double genA6  [nQ2Ranges]={-0.001  ,  0.001  , 0.003  , -0.001  , -0.000  , -0.004  , 0.003  , -0.001  , -0.000};
double genA6err[nQ2Ranges]={0.001222  , 0.001166  , 0.002132  , 0.001558  , 0.002341  , 0.002111  , 0.002154  , 0.001012  , 0.000638};

//////// CHECK ???
/*
std::string f_accXrecoEff_ord0[11] = { // default values
  "1.956257e+04*((1.012873e-04*exp(-0.5*((CosThetaL-(-5.149376e-02))/2.691253e-01)**2)+1.753291e-05*exp(-0.5*((CosThetaL-(-5.081329e-01))/1.020913e-01)**2)+3.139890e-05*exp(-0.5*((CosThetaL-(3.949585e-01))/1.798940e-01)**2))*(4.014027e-05+2.536507e-06*CosThetaK+8.510964e-05*CosThetaK**2-3.901914e-05*CosThetaK**3-1.417547e-04*CosThetaK**4+1.895490e-05*CosThetaK**5+6.712456e-05*CosThetaK**6))",
  "1.725772e+04*((-2.266294e-04*exp(-0.5*((CosThetaL-(9.934612e-02))/4.703605e-01)**2)+2.869562e-04*exp(-0.5*((CosThetaL-(1.864693e-01))/3.975143e-01)**2)+1.012292e-04*exp(-0.5*((CosThetaL-(-3.403547e-01))/3.092047e-01)**2))*(4.817225e-05-1.221187e-05*CosThetaK+7.967950e-05*CosThetaK**2-9.963951e-06*CosThetaK**3-1.175369e-04*CosThetaK**4+5.346328e-06*CosThetaK**5+4.290681e-05*CosThetaK**6))",
  "1.573860e+04*((7.863660e-05+1.520838e-05*CosThetaL+1.307824e-05*CosThetaL**2-1.650353e-05*CosThetaL**3-2.071539e-04*CosThetaL**4+4.045347e-06*CosThetaL**5+1.199631e-04*CosThetaL**6)*(5.370302e-05-1.857967e-05*CosThetaK+7.086538e-05*CosThetaK**2+2.280521e-05*CosThetaK**3-1.069952e-04*CosThetaK**4-2.815009e-05*CosThetaK**5+4.768032e-05*CosThetaK**6))",
  "7.753289e+04*((1.387479e-05+5.878928e-07*CosThetaL+2.241570e-06*CosThetaL**2+7.570859e-06*CosThetaL**3-7.891024e-06*CosThetaL**4-6.702975e-06*CosThetaL**5-6.409915e-06*CosThetaL**6)*(1.352108e-05-2.846590e-06*CosThetaK-8.026189e-06*CosThetaK**2+5.090735e-08*CosThetaK**3+2.544798e-05*CosThetaK**4-1.366480e-06*CosThetaK**5-1.990846e-05*CosThetaK**6))",
  "1.494280e+04*((6.466738e-05+1.022291e-05*CosThetaL+1.688319e-05*CosThetaL**2+5.501163e-06*CosThetaL**3+2.432088e-06*CosThetaL**4-1.026686e-05*CosThetaL**5-4.418552e-05*CosThetaL**6)*(6.722853e-05-1.926792e-05*CosThetaK+2.183848e-06*CosThetaK**2+7.007306e-06*CosThetaK**3+2.034740e-06*CosThetaK**4-8.668490e-06*CosThetaK**5-7.503849e-06*CosThetaK**6))",
  "8.668694e+04*((9.827671e-06-2.141635e-06*CosThetaL+2.141257e-05*CosThetaL**2+1.525517e-05*CosThetaL**3-5.638121e-05*CosThetaL**4-1.573321e-05*CosThetaL**5+3.998330e-05*CosThetaL**6)*(1.132230e-05-2.538736e-06*CosThetaK+1.643913e-06*CosThetaK**2-2.488876e-06*CosThetaK**3-2.674132e-06*CosThetaK**4+3.154697e-06*CosThetaK**5+1.116817e-06*CosThetaK**6))",
  "9.981592e+03*((8.686959e-05+1.496039e-05*CosThetaL+8.509338e-06*CosThetaL**2-5.139799e-05*CosThetaL**3+1.438151e-04*CosThetaL**4+4.123038e-05*CosThetaL**5-1.314166e-04*CosThetaL**6)*(1.031246e-04-3.997859e-05*CosThetaK-2.343429e-05*CosThetaK**2+5.256345e-05*CosThetaK**3+4.963996e-05*CosThetaK**4-4.482207e-05*CosThetaK**5-3.512403e-05*CosThetaK**6))",
  "7.117826e+03*((1.271731e-04-1.007771e-05*CosThetaL+3.140771e-05*CosThetaL**2+2.657901e-05*CosThetaL**3+3.761582e-05*CosThetaL**4-7.479025e-06*CosThetaL**5-3.801289e-05*CosThetaL**6)*(1.405696e-04-3.175964e-05*CosThetaK+4.895380e-06*CosThetaK**2-5.000845e-06*CosThetaK**3-1.694965e-05*CosThetaK**4+1.181410e-05*CosThetaK**5+1.087538e-05*CosThetaK**6))",
  "1.797396e+04*((9.449931e-05+1.072792e-06*CosThetaL-2.153855e-04*CosThetaL**2+5.704460e-06*CosThetaL**3+1.324597e-04*CosThetaL**4-7.380861e-06*CosThetaL**5-1.004489e-05*CosThetaL**6)*(4.500441e-05-7.011887e-06*CosThetaK+8.432796e-05*CosThetaK**2-2.055242e-05*CosThetaK**3-1.296130e-04*CosThetaK**4+1.050718e-05*CosThetaK**5+5.280551e-05*CosThetaK**6))",
  "7.117826e+03*((1.271731e-04-1.007771e-05*CosThetaL+3.140771e-05*CosThetaL**2+2.657901e-05*CosThetaL**3+3.761582e-05*CosThetaL**4-7.479025e-06*CosThetaL**5-3.801289e-05*CosThetaL**6)*(1.405696e-04-3.175964e-05*CosThetaK+4.895380e-06*CosThetaK**2-5.000845e-06*CosThetaK**3-1.694965e-05*CosThetaK**4+1.181410e-05*CosThetaK**5+1.087538e-05*CosThetaK**6))",
    "1.725772e+04*((-2.266294e-04*exp(-0.5*((CosThetaL-(9.934612e-02))/4.703605e-01)**2)+2.869562e-04*exp(-0.5*((CosThetaL-(1.864693e-01))/3.975143e-01)**2)+1.012292e-04*exp(-0.5*((CosThetaL-(-3.403547e-01))/3.092047e-01)**2))*(4.817225e-05-1.221187e-05*CosThetaK+7.967950e-05*CosThetaK**2-9.963951e-06*CosThetaK**3-1.175369e-04*CosThetaK**4+5.346328e-06*CosThetaK**5+4.290681e-05*CosThetaK**6))"
};
*/

//////// MODIFY ????

// Lumi = Nreco/(cross section*branch factor*filter efficiency.                                                                               
// BF_BsToPhiMuMu = 7.6E-7 (2016)                                                                                                             
// BF_BsToPhiJpsi = 1.07E-3 (2016)                                                                                                            
// BF_BsToPhiPsi2S = 5.4E-4 (2016)                                                                                                            
// BF_JpsiToMuMu  = 5.96E-2                                                                                                                   
// BF_Psi2sToMuMu= 7.90E-3                                                                                                                    
//BF_PhiToKK = 0.489      

double datasetLumi[4] = {35.9,72088.167,566.578,1799.856};//data, BsToPhiMuMu(36027.158 + 36061.009), BuToPhiJpsi(566.578), BuToPhiPsi2S(1799.856)                                                                     
//}}}                                                                          

double readParam(int iBin, const char parName[], int iColumn, double defVal=0., double forceReturn=999.)
{//{{{
    // Remark: first value is at iColumn=0.
    if (forceReturn != 999.) return forceReturn;

    std::vector<double> output;
    char lineBuff[1024];
    char *valBuff;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("%s/fitParameters%d.txt",idatacardpath.Data(),iBin),"r");
    if (!fp){
        printf("WARNING: readParam, missing parameter files, by default return %f.\n",defVal);
        return defVal;
    }
    while(fgets(lineBuff,1024,fp) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            printf("INFO: readParam, matched %s!\n",valBuff);
            valBuff = strtok(NULL," ");
            while(valBuff != NULL){
                //output.push_back(stof(valBuff));//stof if c++11 function, use other function
                if (strcmp(valBuff,"nan") == 0 || strcmp(valBuff,"inf") == 0 ){
                    output.push_back(defVal);
                }else{
                    output.push_back(std::atof(valBuff));
                }
                valBuff = strtok(NULL," ");
            }
            break;
        }
        memset(lineBuff,' ',1024*sizeof(char));
    }
    fclose(fp);
    
    if (iColumn < output.size() ){
        printf("INFO: readParam, get %s[%d]=%e\n",parName,iColumn,output.at(iColumn));
        return output.at(iColumn);
    }else{
        printf("WARNING: readParam, empty column! Return %s[%d]=defVal=%f.\n",parName,iColumn,defVal);
        return defVal;
    }
}//}}}

RooRealVar* readParam(const char parName[], const char wspacePathName[])
{//{{{
    TFile *f_wspace = new TFile(wspacePathName);
    RooWorkspace *wspace = (RooWorkspace*)f_wspace->Get("wspace");
    if (wspace){
        RooRealVar *var = (RooRealVar*)wspace->var(parName);
        if (var != 0){
            return var;
        }else{
            printf("ERROR\t\t: %s cannot be found in %s\n", parName, wspacePathName);
            return 0;
        }
    }else{
        printf("ERROR\t\t: wspace cannot be found in %s\n", wspacePathName);
        return 0;
    }
}//}}}

std::string readParam(int iBin, const char parName[], string defVal="", string forceReturn="defaultForceReturn")
{//{{{
    // Remark: first value is at iColumn=0.
    if (forceReturn != "defaultForceReturn") return forceReturn;

    string output;
    char lineBuff[1024];
    char *valBuff;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("%s/fitParameters%d.txt",idatacardpath.Data(),iBin),"r");
    if (!fp){
        printf("WARNING: readParam, missing parameter files, by default return %s.",defVal.c_str());
        return defVal;
    }
    while(fgets(lineBuff,1024,fp) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            printf("INFO: readParam, matched %s!\n",valBuff);
            valBuff = strtok(NULL,"\n");
            output=string(valBuff);
            break;
        }
        memset(lineBuff,' ',1024*sizeof(char));
    }
    fclose(fp);
    
    if (output != ""){
        printf("INFO: readParam, get %s=%s\n",parName,output.c_str());
        return output;
    }else{
        printf("WARNING: readParam, empty item! Return %s=defVal=%s.\n",parName, defVal.c_str());
        return defVal;
    }
}//}}}

void writeParam(int iBin, const char parName[], double *val, int nVal=2, bool overwrite=true)
{//{{{
    if ( !overwrite ) return;

    struct stat fiBuff;
    FILE *fi = 0;
    if (stat(TString::Format("%s/fitParameters%d.txt",odatacardpath.Data(),iBin),&fiBuff) == 0){
        rename(TString::Format("%s/fitParameters%d.txt",odatacardpath.Data(),iBin),TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin));
        fi = fopen(TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin),"r");
    }else{
        fi = fopen(TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin),"w");
    }
    
    bool parExist = false;
    char lineBuff[1024];
    char *valBuff = 0;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("%s/fitParameters%d.txt",odatacardpath.Data(),iBin),"w");
    while(fgets(lineBuff,1024,fi) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            fprintf(fp,"%s",parName);
            int iVal = 0;
            while(iVal < nVal){
                fprintf(fp," %e",val[iVal]);
                iVal++;
            }
            fprintf(fp,"\n");
            parExist = true;
        }else{
            fprintf(fp,"%s",lineBuff);
            valBuff = strtok(NULL," ");
            while( valBuff != NULL ){
                fprintf(fp," %s",valBuff);
                valBuff = strtok(NULL," ");
            }
        }
        memset(lineBuff,' ',512*sizeof(char));
    }
    if (parExist == false){
        fprintf(fp,"%s",parName);
        int iVal = 0;
        while(iVal < nVal){
            fprintf(fp," %e",val[iVal]);
            iVal++;
        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    fclose(fi);
    remove(TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin));
}//}}}

void writeParam(int iBin, const char parName[], string instring, bool overwrite=true)
{//{{{
    if ( !overwrite ) return;

    struct stat fiBuff;
    FILE *fi = 0;
    if (stat(TString::Format("%s/fitParameters%d.txt",odatacardpath.Data(),iBin),&fiBuff) == 0){
        rename(TString::Format("%s/fitParameters%d.txt",odatacardpath.Data(),iBin),TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin));
        fi = fopen(TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin),"r");
    }else{
        fi = fopen(TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin),"w");
    }
    
    bool parExist = false;
    char lineBuff[1024];
    char *valBuff = 0;
    memset(lineBuff,' ',1024*sizeof(char));
    FILE *fp = fopen(TString::Format("%s/fitParameters%d.txt",odatacardpath.Data(),iBin),"w");
    while(fgets(lineBuff,1024,fi) != NULL ){
        valBuff = strtok(lineBuff," ");
        if ( strcmp(valBuff,parName) == 0 ){
            fprintf(fp,"%s %s\n", parName, instring.c_str());
            parExist = true;
        }else{
            fprintf(fp,"%s",lineBuff);
            valBuff = strtok(NULL," ");
            while( valBuff != NULL ){
                fprintf(fp," %s",valBuff);
                valBuff = strtok(NULL," ");
            }
        }
        memset(lineBuff,' ',512*sizeof(char));
    }
    if (parExist == false){
        fprintf(fp,"%s %s\n", parName, instring.c_str());
    }
    fclose(fp);
    fclose(fi);
    remove(TString::Format("%s/fitParameters%d.txt.temp",odatacardpath.Data(),iBin));
    return;
}//}}}

void switchRedirectStdio(const char outfile[]="_stdio", const char mode[]="a", FILE *tty=stdout)
{//{{{
    // This function works ONLY on unix-like system.
    // Or the stdout cannot be restored after running freopen.
    //
    if (!redirectStdout) return;

    struct stat fiBuff;
    if (stat("/dev/tty",&fiBuff) != 0){
        printf("WARNING\t\t: \"/dev/tty\" is NOT available\n");
        return;
    }

    if (strcmp(mode,"a")*strcmp(mode,"w")!=0){
        freopen("/dev/tty","a",stdout);
        freopen("/dev/tty","a",stderr);
        printf("ERROR\t\t: Only mode \"a\" and \"w\" are supported in switchRedirectStdio\n");
        printf("\t\t: Stdout/stderr now restored to screen\n");
    }

    if (strcmp(outfile,"_stdio")==0){
        printf("INFO\t\t: Direct stdout/stderr to screen.\n");
        freopen("/dev/tty","a",stdout);
        freopen("/dev/tty","a",stderr);
    }else{
        printf("INFO\t\t: Redirect stdout/stderr to \"%s\".\n",outfile);
        freopen(outfile,mode,tty);
    }
    return;
}//}}}

// Physically allowed ranges from AN2014_129_v14, p25.
// Transformation rule comes from AN2014_129_v14, p28.

////// need to modify a bit ..
double toUnboundedFl(double fl){
  return TMath::Tan((fl-0.5)*TMath::Pi());         ////// fl_ubd = tan(fl-0.5)*pi 
}

double toBoundedFl(double fl_ubd){
  return 0.5+TMath::ATan(fl_ubd)/TMath::Pi();     ///// fl = 0.5 + tan-1 (fl_ubd)/pi 
}

////// NOT SURE, MODIFY ????
/*
double toUnboundedA6(double a6, double fl){
  return TMath::Tan(2./3.*a6*TMath::Pi()/(1-fl));    
}
*/

double toUnboundedA6(double a6, double fl){
  return TMath::Tan(a6/2*(1-fl))*TMath::Pi();      ///// a6_ubd = tan(a6/2*(1-fl))*pi                                                                                   
}

/*
double toBoundedA6(double a6_ubd, double fl_ubd){
  return 3./2.*(0.5-TMath::ATan(fl_ubd)/TMath::Pi())*TMath::ATan(a6_ubd)/TMath::Pi();    
}
*/

//// updated 
double toBoundedA6(double a6_ubd, double fl_ubd){
  return (0.5-TMath::ATan(fl_ubd)/TMath::Pi())*2*TMath::ATan(a6_ubd)/TMath::Pi() ;  /// a6 = (0.5-tan-1(fl_ubd)/pi)*2*(tan-1(a6_ubd)/pi)
}


bool printPdf2D(RooAbsPdf &pdf, RooRealVar &cosK, RooRealVar &cosL, const char oName[]="")
{//{{{                                                                                
  bool isPositivePDF = true;
  TCanvas *canvas = new TCanvas();
  TH2D *h2 = new TH2D("h2","",200,-1,1,200,-1,1);

  for( double iCosThetaK=-1; iCosThetaK < 1; iCosThetaK+=0.01){
    cosK.setVal(iCosThetaK);
    for( double iCosThetaL=-1; iCosThetaL < 1; iCosThetaL+=0.01){
      cosL.setVal(iCosThetaL);
      double val = pdf.getVal(RooArgSet(cosL,cosK));
      if (val < 0) isPositivePDF = false;
      h2->Fill(iCosThetaL,iCosThetaK,val);
    }
  }

  h2->SetMinimum(0.);
  h2->Draw("COLZ");
  if (strcmp(oName,"") != 0){
    canvas->Print(TString::Format("%s/%s.pdf",plotpath.Data(),oName));
  }

  return isPositivePDF;
}//}}}                                                                                                                                     

TF2  *f2_fcn = NULL;
double model_2D(double *x, double *par)
{//{{{                                                                                                       
  double xx = x[0];
  double yy = x[1];
  for (int i = 0; i < f2_fcn->GetNpar(); i++) f2_fcn->SetParameter(i,par[i]);
  return f2_fcn->Eval(xx,yy);
}//}}}                   

TH2F *h2_fcn = NULL;

void fcn_binnedChi2_2D(int &npar, double *gin, double &f, double *par, int iflag)
{//{{{                                                                                              
  f=0;
  for (int i = 1; i <= h2_fcn->GetNbinsX(); i++) {
    for (int j = 1; j <= h2_fcn->GetNbinsY(); j++) {
      int gBin = h2_fcn->GetBin(i,j);
      double x[2] = {h2_fcn->GetXaxis()->GetBinCenter(i),h2_fcn->GetYaxis()->GetBinCenter(j)};
      double measure  = h2_fcn->GetBinContent(gBin);
      double error    = h2_fcn->GetBinError(gBin);

      for (int k = 0; k < f2_fcn->GetNpar(); k++){

	f2_fcn->SetParameter(k,par[k]);
      }
      double xi = h2_fcn->GetXaxis()->GetBinLowEdge(i);
      double xf = h2_fcn->GetXaxis()->GetBinUpEdge(i);
      double yi = h2_fcn->GetYaxis()->GetBinLowEdge(j);
      double yf = h2_fcn->GetYaxis()->GetBinUpEdge(j);                                                                  
     
      f += pow( (f2_fcn->Integral(xi,xf,yi,yf)/(xf-xi)/(yf-yi)-measure)/error,2);
    }
  }
}//}}}                                                                                                 



//____________________________________________________________________________________
//costhetaK equation                                                                                                                        
std::vector<double> fl_gen_bin(int iBin, const char outfile[] = "fl_gen")
{//{{{                                                                                                                                       
  bool test = false;

  RooRealVar genCosThetaK("genCosThetaK", "cos#theta_{K}", -1, 1);
  RooRealVar genQ2("genQ2","q^{2}",0.5,20.);
  RooRealVar Q2("Q2","q^{2}",0.5,20.);
  RooRealVar fl("fl", "F_{L}", 0.5, -0.5, 1.5);

  RooGenericPdf f("f", "(3/4*(1-fl)*(1-genCosThetaK*genCosThetaK)) + (3/2*fl*(genCosThetaK*genCosThetaK)) " , RooArgSet(genCosThetaK,fl) );
  RooDataSet* data;

  if (test){
    fl.setVal(0.5);
    data = f.generate(RooArgSet(genCosThetaK,Q2), 10000);
  }else{
    data = new RooDataSet("data","data",ch,RooArgSet(genCosThetaK,genQ2),genQ2range[iBin],0);
  }

  f.fitTo(*data,Extended(kTRUE), Minos(kTRUE));

  RooPlot* framecosk = genCosThetaK.frame();
  data->plotOn(framecosk);
  f.plotOn(framecosk);

  // Draw the frame on the canvas                                                                                                              
  TCanvas *c = new TCanvas("c");
  framecosk->SetTitle("");
  framecosk->Draw();

  TLatex *t1 = new TLatex();
  t1->SetNDC();
  t1->DrawLatex(.40,.85,TString::Format("%s",genQ2range[iBin]));
  t1->DrawLatex(.40,.79,TString::Format("F_{L}=%5.3f#pm%5.3f",fl.getVal(),fl.getError()));

  c->SaveSource(TString::Format("%s/%s_bin%d.cc",plotpath.Data(),outfile,iBin));
  c->Print(TString::Format("%s/%s_bin%d.pdf",plotpath.Data(),outfile,iBin));

  delete c;
  delete t1;
  delete data;

  std::vector<double> outvect;
  outvect.push_back(fl.getVal());
  outvect.push_back(fl.getError());
  return outvect;
}//}}}                 


void fl_gen(const char outfile[] = "fl_gen")
{//{{{                                                                                                                                         

  TCanvas *c = new TCanvas();
  TH2F *frame = new TH2F("frame","",18,1,19,10,0,1.5);
  frame->SetStats(kFALSE);
  frame->SetXTitle("q^{2} [(GeV)^{2}]");
  frame->SetYTitle("F_{L}");
  frame->Draw();

  double x[8]={1.5,3.15,6.49,9.385,11.475,13.52,15.09,17.5};
  double xerr[8]={0.5,1.15,2.09,0.705,1.385,0.66,0.91,1.5};
  double yfl[8],yerrfl[8];

  std::vector<double> vbin;
  for(int ibin = 0; ibin < 8; ibin++){
    vbin = fl_gen_bin(ibin);
    yfl[ibin]       =vbin.at(0);
    yerrfl[ibin]    =vbin.at(1);
  }

  // Check input data                                                                                                                          
  for(int ibin = 0; ibin < 8; ibin++){
    printf("yfl [%d]=%6.4f +- %6.4f\n",ibin,yfl[ibin],yerrfl[ibin]);
  }

  TGraphAsymmErrors *g_fl  = new TGraphAsymmErrors(8,x,yfl,xerr,xerr,yerrfl,yerrfl);
  g_fl->Draw("P*");
  c->SaveSource(TString::Format("%s/%s.cc",plotpath.Data(),outfile));
  c->Print(TString::Format("%s/%s.pdf",plotpath.Data(),outfile));

  delete g_fl;
  delete frame;
  delete c;
}//}}}           



//_________________________________________________________________________________

std::vector<double> angular_gen_bin(int iBin, const char outfile[] = "angular_gen")
{//{{{

    RooRealVar genCosThetaK("genCosThetaK", "cos#theta_{K}", -1., 1.);
    RooRealVar genCosThetaL("genCosThetaL", "cos#theta_{L}", -1., 1.);
    RooRealVar genQ2("genQ2","q^{2}",0.5,20.);
    RooRealVar genPhiAng("genPhiAng", "Phi", -TMath::Pi(), TMath::Pi());
    RooRealVar fl("fl", "F_{L}", genFl[iBin], 0.1, 0.9);
    RooRealVar a6("a6", "A_{6}", genA6[iBin], -0.75, 0.75);
    RooRealVar s3("s3", "S_{3}", -0.75, 0.75);
    RooRealVar a9("a9", "A_{9}", -0.75, 0.75);


    RooRealVar s4("s4", "S_{4}", -0.75, 0.75);
    RooRealVar s7("s7", "S_{7}", -0.5, 0.5);

    // CP asymmetries observables                                 
    RooRealVar a5("a5", "A_{5}", -0.75, 0.75);
    RooRealVar a8("a8", "A_{8}", -0.75, 0.75);

    RooRealVar nsig("nsig","nsig",1E6,1E2,1E9);
    RooRealVar nbkg("nbkg","nbkg",10,0.1,1E6);
    
    //simplified costhetal costhetak equation (phi integrating out)
    RooGenericPdf f_sig("f_sig", "9/16*( (1/2*(1-fl)*(1-genCosThetaK*genCosThetaK)*(1+genCosThetaL*genCosThetaL)) + (2*fl*genCosThetaK*genCosThetaK*(1-genCosThetaL*genCosThetaL)) + (a6*(1-genCosThetaK*genCosThetaK)*genCosThetaL) )" , RooArgSet(genCosThetaK,genCosThetaL,fl,a6));


    
    //long costhetal costhetak equation (phi integrating out)
    //RooGenericPdf f_sig("f_sig", "9/16*(3/4*(1-fl)*(1-genCosThetaK*genCosThetaK)+fl*genCosThetaK*genCosThetaK+1/4*(1-fl)*(1-genCosThetaK*genCosThetaK)*(2*genCosThetaL*genCosThetaL-1)-fl*genCosThetaK*genCosThetaK*(2*genCosThetaL*genCosThetaL-1)+a6*(1-genCosThetaK*genCosThetaK)*genCosThetaL)", RooArgSet(genCosThetaK,genCosThetaL,fl,a6));

    //integrating out costhetaK and costhetaL
	//        RooGenericPdf f_sig1("f_sig1", "1/(2*TMath::Pi())+1/(2*TMath::Pi())*s3*TMath::Cos(2*genPhiAng)+1/(2*TMath::Pi())*a9*TMath::Sin(2*genPhiAng)", RooArgSet(genPhiAng,s3,a9));

    //    RooGenericPdf f_sig1("f_sig1", "1/(2*(3.142))+1/(2*(3.142))*s3*TMath::Cos(2*genPhiAng)+1/(2*(3.142))*a9*TMath::Sin(2*genPhiAng)", RooArgSet(genPhiAng,s3,a9)); 


    //    RooArgSet s(genCosThetaK,genCosThetaL,genPhiAng,fl,s3,s4,s7);
    //s.add(RooArgSet(a5,a6,a8,a9));
    
    //RooGenericPdf f_sig("f_sig", "9/(32*(3.142))*(3/4*(1-fl)*(1-genCosThetaK*genCosThetaK)+fl*genCosThetaK*genCosThetaK+1/4*(1-fl)*(1-genCosThetaK*genCosThetaK)*(2*genCosThetaL*genCosThetaL-1)-fl*genCosThetaK*genCosThetaK*(2*genCosThetaL*genCosThetaL-1)+s3*(1-genCosThetaK*genCosThetaK)*(1-genCosThetaL*genCosThetaL)*TMath::Cos(2*genPhiAng)+s4*4*genCosThetaK*genCosThetaL*TMath::Sqrt(1-genCosThetaK*genCosThetaK)*TMath::Sqrt(1-genCosThetaL*genCosThetaL)*TMath::Cos(genPhiAng)+a5*2*genCosThetaK*TMath::Sqrt(1-genCosThetaK*genCosThetaK)*TMath::Sqrt(1-genCosThetaL*genCosThetaL)*TMath::Cos(genPhiAng)+a6*(1-genCosThetaK*genCosThetaK)*genCosThetaL+a8*4*genCosThetaK*genCosThetaL*TMath::Sqrt(1-genCosThetaK*genCosThetaK)*TMath::Sqrt(1-genCosThetaL*genCosThetaL)*TMath::Sin(genPhiAng)+a9*(1-genCosThetaK*genCosThetaK)*(1-genCosThetaL*genCosThetaL)*TMath::Sin(2*genPhiAng))", s);


    RooExtendPdf f("f","",f_sig,nsig);
    //    RooExtendPdf f1("f1","",f_sig1,nsig);

    //RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(genCosThetaK,genCosThetaL,genPhiAng,genQ2),genQ2range[iBin],0);
    RooDataSet *data = new RooDataSet("data","data",ch,RooArgSet(genCosThetaK,genCosThetaL,genQ2),genQ2range[iBin],0);


    RooFitResult *f_fitresult = f.fitTo(*data,Extended(kTRUE),Save(kTRUE),Minimizer("Minuit"),Minos(kFALSE),NumCPU(4));
    //RooFitResult *f_fitresult1 = f1.fitTo(*data1,Extended(kTRUE),Save(kTRUE),Minimizer("Minuit"),Minos(kFALSE),NumCPU(4));
    



    // Draw the frame on the canvas
    TCanvas* c = new TCanvas("c");
    RooPlot* framecosk = genCosThetaK.frame(); 
    data->plotOn(framecosk,Binning(100)); 
    f.plotOn(framecosk); 

    framecosk->SetTitle("");
    framecosk->SetMinimum(0);
    framecosk->Draw();

    TLatex *t1 = new TLatex();
    t1->SetNDC();
    double fixNDC = -0.5;
    switch(iBin){
        case 10:
            fixNDC = 0.;
            break;
        default:
            break;
    }

    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",genQ2range[iBin]));
    t1->DrawLatex(.35,.80+fixNDC,TString::Format("F_{L}=%6.4f#pm%8.6f",fl.getVal(),fl.getError()));
    t1->DrawLatex(.35,.74+fixNDC,TString::Format("A_{6}=%6.4f#pm%8.6f",a6.getVal(),a6.getError()));
    c->SaveSource(TString::Format("%s/%s_cosk_bin%d.cc",plotpath.Data(),outfile,iBin));
    c->Print(TString::Format("%s/%s_cosk_bin%d.png",plotpath.Data(),outfile,iBin));
    c->Print(TString::Format("%s/%s_cosk_bin%d.pdf",plotpath.Data(),outfile,iBin));

    //
    RooPlot* framecosl = genCosThetaL.frame(); 
    data->plotOn(framecosl,Binning(100)); 
    f.plotOn(framecosl); 

    framecosl->SetTitle("");
    framecosl->SetMinimum(0);
    framecosl->Draw();

    fixNDC = -0.5;
    switch(iBin){
        default:
            break;
    }


    t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",genQ2range[iBin]));
    t1->DrawLatex(.35,.80+fixNDC,TString::Format("F_{L}=%6.4f#pm%8.6f",fl.getVal(),fl.getError()));
    t1->DrawLatex(.35,.74+fixNDC,TString::Format("A_{6}=%6.4f#pm%8.6f",a6.getVal(),a6.getError()));
    c->Update();
    c->SaveSource(TString::Format("%s/%s_cosl_bin%d.cc",plotpath.Data(),outfile,iBin));
    c->Print(TString::Format("%s/%s_cosl_bin%d.png",plotpath.Data(),outfile,iBin));
    c->Print(TString::Format("%s/%s_cosl_bin%d.pdf",plotpath.Data(),outfile,iBin));

    //##################################                                                                                        

    // Make 2-D plot
    TH1 *h1 = data->createHistogram("genCosThetaL,genCosThetaK", 100, 100);
    h1->Draw("LEGO2");
    c->Update();
    c->SaveSource(TString::Format("%s/%s_2D_bin%d.cc",plotpath.Data(),outfile,iBin));
    c->Print(TString::Format("%s/%s_2D_bin%d.png",plotpath.Data(),outfile,iBin));
    c->Print(TString::Format("%s/%s_2D_bin%d.pdf",plotpath.Data(),outfile,iBin));


    // clear
    delete t1;
    delete c;
    delete data;
    
    //write output
    double outputp[4] = {0,0,0,0};
    outputp[0] = fl.getVal();
    outputp[1] = fl.getError();
    outputp[2] = fl.getErrorLo();
    outputp[3] = fl.getErrorHi();
    writeParam(iBin,"fl_gen",outputp,4);
    outputp[0] = a6.getVal();
    outputp[1] = a6.getError();
    outputp[2] = a6.getErrorLo();
    outputp[3] = a6.getErrorHi();
    writeParam(iBin,"a6_gen",outputp,4);
    outputp[0] = s3.getVal();
    outputp[1] = s3.getError();
    outputp[2] = s3.getErrorLo();
    outputp[3] = s3.getErrorHi();
    writeParam(iBin,"s3_gen",outputp,4);
    outputp[0] = a9.getVal();
    outputp[1] = a9.getError();
    outputp[2] = a9.getErrorLo();
    outputp[3] = a9.getErrorHi();
    writeParam(iBin,"a9_gen",outputp,4);

    std::vector<double> output;
    output.push_back(fl.getVal());
    output.push_back(fl.getError());
    output.push_back(a6.getVal());
    output.push_back(a6.getError());
    output.push_back(s3.getVal());
    output.push_back(s3.getError());
    output.push_back(a9.getVal());
    output.push_back(a9.getError());
    return output;

}//}}}

void angular_gen(const char outfile[] = "angular_gen")
{//{{{
    bool doFit = true; // Turn to true if you want to fit again.
    printf("INFO\t\t: You'll need UNFILTERED MC to be the input data.\n");

    TCanvas *c = new TCanvas();

    int nWorkBins = 9;
    int workBins[] = {0,1,2,3,4,5,6,7,8};
    double x[nWorkBins];
    double xerr[nWorkBins];
    double ya6[nWorkBins],yerra6Lo[nWorkBins],yerra6Hi[nWorkBins],yfl[nWorkBins],yerrflLo[nWorkBins],yerrflHi[nWorkBins];

    for(int iBin = 0; iBin < nWorkBins; iBin++){
        x[iBin] = (q2rangeup[workBins[iBin]]+q2rangedn[workBins[iBin]])/2;
        xerr[iBin] = (q2rangeup[workBins[iBin]]-q2rangedn[workBins[iBin]])/2;
    }


    if (doFit){
        for(int ibin = 0; ibin < nWorkBins; ibin++){
            angular_gen_bin(workBins[ibin],outfile);
        }
    }


    // plotting
    for(int ibin = 0; ibin < nWorkBins-2; ibin++){
        yfl[ibin] = -100;
        yerrflLo[ibin] = 0;
        yerrflHi[ibin] = 0;
        ya6[ibin] = -100;
        yerra6Lo[ibin] = 0;
        yerra6Hi[ibin] = 0;
        yfl[ibin]           = readParam(workBins[ibin],"fl_gen",0);
        yerrflLo[ibin]      = readParam(workBins[ibin],"fl_gen",2);
        yerrflHi[ibin]      = readParam(workBins[ibin],"fl_gen",3);
        if (yerrflHi[ibin] == -1){
            yerrflHi[ibin] = 0;
        }
        if (yerrflLo[ibin] == -1){
            yerrflLo[ibin] = 0;
        }
        if (yerrflHi[ibin] == yerrflLo[ibin]){
            yerrflHi[ibin] = 0;
            yerrflLo[ibin] = 0;
        }
        ya6[ibin]          = readParam(workBins[ibin],"a6_gen",0);
        yerra6Lo[ibin]     = readParam(workBins[ibin],"a6_gen",2);
        yerra6Hi[ibin]     = readParam(workBins[ibin],"a6_gen",3);
        if (yerra6Hi[ibin] == -1){
            yerra6Hi[ibin] = 0;
        }
        if (yerra6Lo[ibin] == -1){
            yerra6Lo[ibin] = 0;
        }
        if (yerra6Hi[ibin] == yerra6Lo[ibin]){
            yerra6Hi[ibin] = 0;
            yerra6Lo[ibin] = 0;
        }
        printf("ya6[%d]=%6.4f + %6.4f - %6.4f\n",ibin,ya6[ibin],yerra6Hi[ibin],yerra6Lo[ibin]);
        printf("yfl [%d]=%6.4f + %6.4f - %6.4f\n",ibin,yfl[ibin],yerrflHi[ibin],yerrflLo[ibin]);
    }

    TGraphAsymmErrors *g_fl  = new TGraphAsymmErrors(nWorkBins,x,yfl,xerr,xerr,yerrflLo,yerrflHi);
    g_fl->SetTitle("");
    g_fl->GetXaxis()->SetTitle("q^{2} [(GeV)^{2}]");
    g_fl->GetYaxis()->SetTitle("F_{L}");
    g_fl->GetYaxis()->SetRangeUser(0,1);
    g_fl->SetFillColor(2);
    g_fl->SetFillStyle(3001);
    g_fl->Draw("a2");
    g_fl->Draw("P");
    c->SaveSource(TString::Format("%s/%s_fl.cc",plotpath.Data(),outfile));
    c->Print(TString::Format("%s/%s_fl.png",plotpath.Data(),outfile));
    c->Print(TString::Format("%s/%s_fl.pdf",plotpath.Data(),outfile));
    c->Clear();

    TGraphAsymmErrors *g_a6 = new TGraphAsymmErrors(nWorkBins,x,ya6,xerr,xerr,yerra6Lo,yerra6Hi);
    g_a6->SetTitle("");
    g_a6->GetXaxis()->SetTitle("q^{2} [(GeV)^{2}]");
    g_a6->GetYaxis()->SetTitle("A_{6}");
    g_a6->GetYaxis()->SetRangeUser(-1,1);
    g_a6->SetFillColor(2);
    g_a6->SetFillStyle(3001);
    g_a6->Draw("a2");
    g_a6->Draw("P");
    c->Update();
    c->SaveSource(TString::Format("%s/%s_a6.cc",plotpath.Data(),outfile));
    c->Print(TString::Format("%s/%s_a6.png",plotpath.Data(),outfile));
    c->Print(TString::Format("%s/%s_a6.pdf",plotpath.Data(),outfile));


}//}}}


//_________________________________________________________________________________
//Fit parameters of acceptance and efficiency using TMinuit instead of RooFit.

std::vector<double> acceptance(int iBin) // acceptance. Not used in standard procedure, just for check...
{//{{{

  printf("Evaluate acceptance efficiency for bin#%d\n",iBin);

  double accUpperBound = 0.09;//upper bound of Y axis
  double gQ2 = 0;
  double gCosThetaK = 0;
  double gCosThetaL = 0;
  double gmuppt = 0;
  double gmupeta= 0;
  double gmumpt = 0;
  double gmumeta= 0;

  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("genQ2"         , 1);
  ch->SetBranchStatus("genCosTheta*"  , 1);
  ch->SetBranchStatus("genMu*"        , 1);
  ch->SetBranchAddress("genQ2"        , &gQ2);
  ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
  ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
  ch->SetBranchAddress("genMupPt"     , &gmuppt);
  ch->SetBranchAddress("genMupEta"    , &gmupeta);
  ch->SetBranchAddress("genMumPt"     , &gmumpt);
  ch->SetBranchAddress("genMumEta"    , &gmumeta);

  // Fill histograms
  int nbinsL = 20;
  int nbinsK = 20;
  TH2F h2_ngen("h2_ngen","h2_ngen",nbinsL,-1.,1,nbinsK,-1,1);//no.of generated events
  TH2F h2_nacc("h2_nacc" ,"h2_nacc" ,nbinsL,-1,1,nbinsK,-1,1); //no.of events after singlemuon selection at gen level
  // Read data
  for (int entry = 0; entry < ch->GetEntries(); entry++) {
    ch->GetEntry(entry);
    if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
    if (gQ2 > q2rangeup[2] && gQ2 < q2rangedn[2]) continue;//jpsi
    if (gQ2 > q2rangeup[4] && gQ2 < q2rangedn[4]) continue;//psi2s
    h2_ngen.Fill(gCosThetaL,gCosThetaK);//filling no.of generated events
    if ( fabs(gmumeta) < 2.5 && fabs(gmupeta) < 2.5 && gmumpt > 2.5 && gmuppt > 2.5 ) h2_nacc.Fill(gCosThetaL,gCosThetaK);//events after singlemuon selection
  }


  // Calculate acceptance
  TH2F h2_acc("h2_acc","",nbinsL,-1,1,nbinsK,-1,1);
  h2_acc.SetAxisRange(0.,1.,"Z");
  for (int i = 1; i <= nbinsL; i++) {
    for (int j = 1; j <= nbinsK; j++) {
      // Fill acceptance
      if (h2_ngen.GetBinContent(i,j) == 0) {
	printf("WARNING: Acceptance(%d,%d)=%f/%f\n",i,j,h2_nacc.GetBinContent(i,j),h2_ngen.GetBinContent(i,j));
	h2_acc.SetBinContent(i,j,0.);
	h2_acc.SetBinError(i,j,1.);
      }else{
	//	cout<<"no.of events generated: "<<h2_ngen.GetBinContent(i,j)<<endl;
	//cout<<"no.of events after single muon selection: "<<h2_nacc.GetBinContent(i,j)<<endl;
	h2_acc.SetBinContent(i,j,h2_nacc.GetBinContent(i,j)/h2_ngen.GetBinContent(i,j));//acceptance efficiency= h2_nacc/h2_ngen
	//cout<<"acceptance efficiency is : "<<h2_acc.GetBinContent(i,j)<<endl;
	if (h2_nacc.GetBinContent(i,j) != 0){
	  h2_acc.SetBinError(i,j,sqrt(h2_acc.GetBinContent(i,j)*(1.-h2_acc.GetBinContent(i,j))/h2_ngen.GetBinContent(i,j)));
	}else{
	  h2_acc.SetBinError(i,j,sqrt(0.05/h2_ngen.GetBinContent(i,j)));
	}
	// printf("INFO: Angular bin(%d,%d)= %f +- %f ( %f / %f).\n",i,j,h2_acc.GetBinContent(i,j),h2_acc.GetBinError(i,j),h2_nacc.GetBinContent(i,j),h2_ngen.GetBinContent(i,j));
      }
    }
  }
  printf("INFO: h2_acc built.\n");
    
  // Using pure TMinuit
  int nPar = 20;//20 parameters
  TMinuit *gMinuit = new TMinuit(nPar);
  h2_fcn = &h2_acc;
  gMinuit->SetFCN(fcn_binnedChi2_2D);

  TF2 f2_model("f2_model","([0]+[1]*y+[2]*(3*y**2-1)/2+[3]*(5*y**3-3*y)/2)+([4]+[5]*y+[6]*(3*y**2-1)/2+[7]*(5*y**3-3*y)/2)*x**2+([8]+[9]*y+[10]*(3*y**2-1)/2+[11]*(5*y**3-3*y)/2)*x**3+([12]+[13]*y+[14]*(3*y**2-1)/2+[15]*(5*y**3-3*y)/2)*x**4+([16]+[17]*y+[18]*(3*y**2-1)/2+[19]*(5*y**3-3*y)/2)*x**6",-1.,1.,-1.,1.);
  f2_fcn = &f2_model;
  gMinuit->DefineParameter( 0, "k0l0",  .06,  1E-3,    -1E+0, 1E+2);//defining parameter(par no, par name, initia value, initial error,lower limit(-1E+0=-1), upper limit(1E+2=100) 
  gMinuit->DefineParameter( 1, "k1l0",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 2, "k2l0",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 3, "k3l0",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 4, "k0l2",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 5, "k1l2",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 6, "k2l2",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 7, "k3l2",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 8, "k0l3",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter( 9, "k1l3",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter(10, "k2l3",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter(11, "k3l3",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter(12, "k0l4",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter(13, "k1l4",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter(14, "k2l4",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter(15, "k3l4",   0.,  1E-3,    -1E+1, 1E+2);
  gMinuit->DefineParameter(16, "k0l6",   0.,  1E-3,    -1E+2, 1E+4);
  gMinuit->DefineParameter(17, "k1l6",   0.,  1E-3,    -1E+2, 1E+4);
  gMinuit->DefineParameter(18, "k2l6",   0.,  1E-3,    -1E+2, 1E+4);
  gMinuit->DefineParameter(19, "k3l6",   0.,  1E-3,    -1E+2, 1E+4);
  if (iBin == 0) {
    gMinuit->Command("SET PARM 9 0");//set parameter here parameter 9 is set "0"
    gMinuit->Command("SET PARM 10 0");
    gMinuit->Command("SET PARM 11 0");
    gMinuit->Command("SET PARM 12 0");
    gMinuit->Command("FIX 9");//parameter 9is fixed throughout for bin 0
    gMinuit->Command("FIX 10");
    gMinuit->Command("FIX 11");
    gMinuit->Command("FIX 12");
  }else if (iBin == 1) {
    gMinuit->Command("SET PARM 9 0");
    gMinuit->Command("SET PARM 10 0");
    gMinuit->Command("SET PARM 11 0");
    gMinuit->Command("SET PARM 12 0");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 9");
    gMinuit->Command("FIX 10");
    gMinuit->Command("FIX 11");
    gMinuit->Command("FIX 12");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
    /*   
  }else if (iBin == 2 || iBin == 4 ) {                                                                                                         
    gMinuit->Command("SET PARM 17 0");                                                                                                         
    gMinuit->Command("SET PARM 18 0");                                                                                                         
    gMinuit->Command("SET PARM 19 0");                                                                                                         
    gMinuit->Command("SET PARM 20 0");                                                                                                         
    gMinuit->Command("FIX 17");                                                                                                                
    gMinuit->Command("FIX 18");                                                                                                               
    gMinuit->Command("FIX 19");                                                                                                           
    gMinuit->Command("FIX 20");
    
  }else if (iBin == 3 || iBin == 5 ) {                                                                                                        
    gMinuit->Command("SET PARM 1 0");                                                                                                         
    gMinuit->Command("SET PARM 2 0");                                                                                                         
    gMinuit->Command("SET PARM 3 0");                                                                                                         
    gMinuit->Command("SET PARM 4 0");                                                                                                         
    gMinuit->Command("FIX 1");                                                                                                                
    gMinuit->Command("FIX 2");                                                                                                                
    gMinuit->Command("FIX 3");                                                                                                                
    gMinuit->Command("FIX 4");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
    gMinuit->Command("SET PARM 5 0");
    gMinuit->Command("SET PARM 6 0");
    gMinuit->Command("SET PARM 7 0");
    gMinuit->Command("SET PARM 8 0");
    gMinuit->Command("FIX 5");
    gMinuit->Command("FIX 6");
    gMinuit->Command("FIX 7");
    gMinuit->Command("FIX 8");*/    
  }else if (iBin > 1 && iBin < 6 ) {
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else{
    gMinuit->Command("SET PARM 13 0");
    gMinuit->Command("SET PARM 14 0");
    gMinuit->Command("SET PARM 15 0");
    gMinuit->Command("SET PARM 16 0");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 13");
    gMinuit->Command("FIX 14");
    gMinuit->Command("FIX 15");
    gMinuit->Command("FIX 16");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }

  gMinuit->Command("MINI");
  gMinuit->Command("MINI");
  gMinuit->Command("IMPROVE");
  gMinuit->Command("MINOS");

  double arrPar[nPar];
  double arrParErr[nPar];
  for (int iPar = 0; iPar < nPar; iPar++) gMinuit->GetParameter(iPar,arrPar[iPar],arrParErr[iPar]);
  for (int iPar = 0; iPar < nPar; iPar++) f2_model.SetParameter(iPar,arrPar[iPar]);

  // Prepare draw
  double chi2Val=0;
  fcn_binnedChi2_2D(nPar, 0, chi2Val, arrPar, 0);
  printf("Chi2=%f \n",chi2Val);
    
  TCanvas canvas("canvas");
  TLatex *latex = new TLatex();
  latex->SetNDC();
    
  // Draw efficiency
  h2_acc.SetStats(0);
  h2_acc.SetMinimum(0.);
  h2_acc.SetMaximum(accUpperBound);//y axis maximum(0.9)
  h2_acc.SetTitleOffset(2,"XY");
  h2_acc.SetXTitle("genCosThetaL");//X axis title
  h2_acc.SetYTitle("genCosThetaK");//Y axis title
  h2_acc.SetZTitle("Acceptance");
  h2_acc.Draw("LEGO2");//lego drawing option
  latex->DrawLatex(0.35,0.95,TString::Format("Acceptance in Bin%d",iBin));
    
  // Draw FitResult
  f2_model.SetTitle("");
  f2_model.SetMaximum(accUpperBound);
  f2_model.SetLineWidth(1);
  latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
  latex->DrawLatex(0.01,0.90,TString::Format("DoF = %d",nbinsK*nbinsL-gMinuit->GetNumFreePars()));
  f2_model.Draw("SURF SAME ");//Plotting the model function on same canvas
  canvas.SaveSource(TString::Format("%s/acceptance_2D_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/acceptance_2D_bin%d.pdf",plotpath.Data(),iBin));

  //// Draw compare
  TH2F h2_compFit("h2_compFit","",nbinsL,-1,1,nbinsK,-1,1);//comparision
  for (int i = 1; i <= nbinsL; i++) {//thetaL
    for (int j = 1; j <= nbinsK; j++) {//thetaK
      if (h2_acc.GetBinContent(i,j) != 0){
	h2_compFit.SetBinContent(i,j,f2_model.Eval(h2_acc.GetXaxis()->GetBinCenter(i),h2_acc.GetYaxis()->GetBinCenter(j))/h2_acc.GetBinContent(i,j));//compare histogram = f2_model/h2_acc
      }else{
	h2_compFit.SetBinContent(i,j,0.);
      }
    }
  }
  h2_compFit.SetMinimum(0.);//Y axis minimum 0
  h2_compFit.SetStats(0);//No stats box
  h2_compFit.SetTitleOffset(2,"XY");//XY Title
  h2_compFit.SetXTitle("genCosThetaL");//X axis title
  h2_compFit.SetYTitle("genCosThetaK");//Y axis title
  h2_compFit.Draw("LEGO2");//Lego draw option
  latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
  latex->DrawLatex(0.01,0.90,TString::Format("DoF = %d",nbinsK*nbinsL-gMinuit->GetNumFreePars()));
  latex->DrawLatex(0.30,0.95,TString::Format("acceptance_{measured} / acceptance_{fit} in Bin%d",iBin));
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/acceptance_compFit_2D_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/acceptance_compFit_2D_bin%d.pdf",plotpath.Data(),iBin));
    
  // Draw significance
  TH1F h_pull("Deviation/Error","",15,-3.,3.);//pull distribution = (fitted-input)/error
  h_pull.SetXTitle("Significance of deviation");
  h_pull.SetYTitle("Angular bins");
  for (int i = 1; i <= nbinsL; i++) {//thetaL
    for (int j = 1; j <= nbinsK; j++) {//thetaK
      double _xlo = h2_acc.GetXaxis()->GetBinLowEdge(i);
      double _xhi = h2_acc.GetXaxis()->GetBinUpEdge(i);
      double _ylo = h2_acc.GetYaxis()->GetBinLowEdge(j);
      double _yhi = h2_acc.GetYaxis()->GetBinUpEdge(j);
      if (h2_nacc.GetBinContent(i,j) != 0){
	h_pull.Fill((f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_acc.GetBinContent(i,j))/h2_acc.GetBinError(i,j));
//(f2_model-h2_acc)/h2_acc error
      }
    }
  }
  h_pull.Draw();
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/acceptance_sigma_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/acceptance_sigma_bin%d.pdf",plotpath.Data(),iBin));

  // Clear
  delete latex;
  delete gMinuit;

  //prepare output
  std::vector<double> output;
  for (int iPar = 0; iPar < nPar; iPar++){
    output.push_back(arrPar[iPar]);
    output.push_back(arrParErr[iPar]);
        
    printf("%f,",arrPar[iPar]);
    if (iPar+1 >= nPar) printf("\n");
  }
  for (int i = 0; i < output.size(); i=i+2) {
    printf("%f,",output[i+1]);
    if (i+2 >= output.size()) printf("\n");
  }
  return output;
}//}}}


std::vector<double> recoEff(int iBin) // reconstruction efficiency. Not used in standard procedure, just for check...
{//{{{
  printf("Evaluate reconstruction efficiency for bin#%d\n",iBin);
  double effUpperBound = 0.015;
  if (iBin == 3 || iBin == 5) effUpperBound = 0.002;
  // double effUpperBound = 0.03; 

  double BMass = 0;
  double Mumumass = 0;
  double Mumumasserr = 0;
  double gQ2 = 0;
  double gCosThetaK = 0;
  double gCosThetaL = 0;
  double CosThetaK = 0;
  double CosThetaL = 0;
  double gmuppt = 0;
  double gmupeta= 0;
  double gmumpt = 0;
  double gmumeta= 0;
  int    triggers=0;

  /// define counters
  int nacc(0), nreco(0);

  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("Bmass"         , 1);
  ch->SetBranchStatus("Mumumass"      , 1);
  ch->SetBranchStatus("Mumumasserr"   , 1);
  ch->SetBranchStatus("Triggers"      , 1);
  ch->SetBranchStatus("genQ2"         , 1);
  ch->SetBranchStatus("genCosTheta*"  , 1);
  ch->SetBranchStatus("genMu*"        , 1);
  ch->SetBranchAddress("Bmass"        , &BMass);
  ch->SetBranchAddress("Mumumass"     , &Mumumass);
  ch->SetBranchAddress("Mumumasserr"  , &Mumumasserr);
  ch->SetBranchAddress("genQ2"        , &gQ2);
  ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
  ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
  ch->SetBranchAddress("CosThetaK"    , &CosThetaK);
  ch->SetBranchAddress("CosThetaL"    , &CosThetaL);
  ch->SetBranchAddress("genMupPt"     , &gmuppt);
  ch->SetBranchAddress("genMupEta"    , &gmupeta);
  ch->SetBranchAddress("genMumPt"     , &gmumpt);
  ch->SetBranchAddress("genMumEta"    , &gmumeta);
  ch->SetBranchAddress("Triggers"     , &triggers);

  // Fill histograms
  float thetaKBins[6]={-1,-0.7,0.,0.4,0.8,1};
  float thetaLBins[7]={-1,-0.7,-0.3,0.,0.3,0.7,1};
  TH2F h2_nacc("h2_nacc" ,"h2_nacc" ,6,thetaLBins,5,thetaKBins); //no.of events after single muon selection
   TH2F h2_nreco("h2_nreco","h2_nreco",6,thetaLBins,5,thetaKBins);
  int nLBins = 20;
  int nKBins = 20;
  //TH2F h2_nacc("h2_nacc","h2_nacc",nLBins,-1,1,nKBins,-1,1);
  //TH2F h2_nreco("h2_nreco","h2_nreco",nLBins,-1,1,nKBins,-1,1);
  TH1F h_naccL("h_naccL" ,"h_naccL" ,nLBins,-1,1);
  TH1F h_nrecoL("h_nrecoL","h_nrecoL",nLBins,-1,1);
  TH1F h_naccK("h_naccK" ,"h_naccK" ,nKBins,-1,1);
  TH1F h_nrecoK("h_nrecoK","h_nrecoK",nKBins,-1,1);
  h_naccL.SetStats(0);
  h_naccL.SetMinimum(0.);
  h_naccL.SetXTitle("CosThetaL");
  h_naccL.SetYTitle("#Events/0.2");
  h_nrecoL.SetStats(0);
  h_nrecoL.SetMinimum(0.);
  h_nrecoL.SetXTitle("CosThetaL");
  h_nrecoL.SetYTitle("#Events/0.2");
  h_naccK.SetStats(0);
  h_naccK.SetMinimum(0.);
  h_naccK.SetXTitle("CosThetaK");
  h_naccK.SetYTitle("#Events/0.2");
  h_nrecoK.SetStats(0);
  h_nrecoK.SetMinimum(0.);
  h_nrecoK.SetXTitle("CosThetaK");
  h_nrecoK.SetYTitle("#Events/0.2");

  for (int entry = 0; entry < ch->GetEntries(); entry++) {
    ch->GetEntry(entry);

    if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
    if (gQ2 > q2rangeup[2] && gQ2 < q2rangedn[2]) continue;//jpsi
    if (gQ2 > q2rangeup[4] && gQ2 < q2rangedn[4]) continue;//psi2s
    //if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;

    if ( fabs(gmumeta) < 2.5 && fabs(gmupeta) < 2.5 && gmumpt > 2.5 && gmuppt > 2.5 ){
      h2_nacc.Fill(gCosThetaL,gCosThetaK);
      h_naccL.Fill(gCosThetaL);
      h_naccK.Fill(gCosThetaK);
      nacc++;
    }
    if (triggers > 0 && BMass != 0 && ((Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-3.5*Mumumasserr)) ){
      h2_nreco.Fill(CosThetaL,CosThetaK);
      h_nrecoL.Fill(CosThetaL);
      h_nrecoK.Fill(CosThetaK);
      nreco++;
    }
  }
//  double recoeff = nacc/nreco;
  cout << ">>>>>>>"<< nacc << "\t" << nreco << endl;

  // Calculate efficiency
  TH2F h2_rec("h2_rec","",6,thetaLBins,5,thetaKBins);//reco efficiency
  //  TH2F h2_rec("h2_rec","",nLBins,-1,1,nKBins,-1,1);
  h2_rec.SetMinimum(0.);//Y axis minimum
  h2_rec.SetTitleOffset(2,"XY");
  h2_rec.SetXTitle("CosThetaL");
  h2_rec.SetYTitle("CosThetaK");
  for (int i = 1; i <= nLBins; i++) {
    for (int j = 1; j <= nKBins; j++) {
  //for (int i = 1; i <= 6; i++) {
  //  for (int j = 1; j <= 5; j++) {
      // Build from MC samples
      if (h2_nacc.GetBinContent(i,j) == 0 || h2_nreco.GetBinContent(i,j) == 0) {
	printf("WARNING: Efficiency(%d,%d)=0, set error to be 1.\n",i,j);
	h2_rec.SetBinContent(i,j,0.);
	h2_rec.SetBinError(i,j,1.);
      }else{
	h2_rec.SetBinContent(i,j,h2_nreco.GetBinContent(i,j)/h2_nacc.GetBinContent(i,j));//reco efficiency= h2_nreco/h2_nacc
	h2_rec.SetBinError(i,j,sqrt(h2_rec.GetBinContent(i,j)*(1-h2_rec.GetBinContent(i,j))/h2_nacc.GetBinContent(i,j)));
	printf("INFO: Efficiency(%d,%d)=%f +- %f.\n",i,j,h2_rec.GetBinContent(i,j),h2_rec.GetBinError(i,j));
      }

    }
  }

  //  int nentries_ = h2_rec.GetEntries();
  //cout << "\n=> total entries in the data BsToPhiMuMu tree = " << nentries_ << endl;
  



  // Using pure TMinuit
  int nPar = 20;
  TMinuit *gMinuit = new TMinuit(nPar);
  h2_fcn = &h2_rec;
  gMinuit->SetFCN(fcn_binnedChi2_2D);
    
  // Use Legendre polynomial for better convergance
  // 1,x,(3x^2-1)/2,(5x^3-3x)/2
  //  TF2 f2_model("f2_model","([0]+[1]*y+[2]*y**2+[3]*y**3)+([4]+[5]*y+[6]*y**2+[7]*y**3)*x**2+([8]+[9]*y+[10]*y**2+[11]*y**3)*x**3+([12]+[13]*y+[14]*y**2+[15]*y**3)*x**4+([16]+[17]*y+[18]*y**2+[19]*y**3)*x**6",-1.,1.,-1.,1.);
  TF2 f2_model("f2_model","([0]+[1]*y+[2]*(3*y**2-1)/2+[3]*(5*y**3-3*y)/2)+([4]+[5]*y+[6]*(3*y**2-1)/2+[7]*(5*y**3-3*y)/2)*x**2+([8]+[9]*y+[10]*(3*y**2-1)/2+[11]*(5*y**3-3*y)/2)*x**3+([12]+[13]*y+[14]*(3*y**2-1)/2+[15]*(5*y**3-3*y)/2)*x**4+([16]+[17]*y+[18]*(3*y**2-1)/2+[19]*(5*y**3-3*y)/2)*x**6",-1.,1.,-1.,1.);
  f2_fcn = &f2_model;
  gMinuit->DefineParameter( 0, "k0l0",  .01,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 1, "k1l0",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 2, "k2l0",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 3, "k3l0",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 4, "k0l2", 1E-2,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 5, "k1l2",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 6, "k2l2",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 7, "k3l2",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 8, "k0l3",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 9, "k1l3",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter(10, "k2l3",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter(11, "k3l3",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter(12, "k0l4",-1E-2,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter(13, "k1l4",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter(14, "k2l4",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter(15, "k3l4",   0.,  1E-3,    -1E+1, 1E+1);
  gMinuit->DefineParameter(16, "k0l6", 1E-2,  1E-3,    -1E+2, 1E+3);
  gMinuit->DefineParameter(17, "k1l6",   0.,  1E-3,    -1E+2, 1E+3);
  gMinuit->DefineParameter(18, "k2l6",   0.,  1E-3,    -1E+2, 1E+3);
  gMinuit->DefineParameter(19, "k3l6",   0.,  1E-3,    -1E+2, 1E+3);

  if (iBin == 0) {
    gMinuit->Command("SET PARM 9 0");
    gMinuit->Command("SET PARM 10 0");
    gMinuit->Command("SET PARM 11 0");
    gMinuit->Command("SET PARM 12 0");
    gMinuit->Command("FIX 9");
    gMinuit->Command("FIX 10");
    gMinuit->Command("FIX 11");
    gMinuit->Command("FIX 12");
  }else if (iBin == 1) {
    gMinuit->Command("SET PARM 9 0");
    gMinuit->Command("SET PARM 10 0");
    gMinuit->Command("SET PARM 11 0");
    gMinuit->Command("SET PARM 12 0");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 9");
    gMinuit->Command("FIX 10");
    gMinuit->Command("FIX 11");
    gMinuit->Command("FIX 12");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else if (iBin > 1 && iBin < 6 ) {
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else{
    gMinuit->Command("SET PARM 13 0");
    gMinuit->Command("SET PARM 14 0");
    gMinuit->Command("SET PARM 15 0");
    gMinuit->Command("SET PARM 16 0");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 13");
    gMinuit->Command("FIX 14");
    gMinuit->Command("FIX 15");
    gMinuit->Command("FIX 16");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }

  gMinuit->Command("MINI");
  gMinuit->Command("MINI");
  gMinuit->Command("IMPROVE");
  gMinuit->Command("MINOS");

  double arrPar[nPar];
  double arrParErr[nPar];
  for (int iPar = 0; iPar < nPar; iPar++) gMinuit->GetParameter(iPar,arrPar[iPar],arrParErr[iPar]);
  for (int iPar = 0; iPar < nPar; iPar++) f2_model.SetParameter(iPar,arrPar[iPar]);
    
  // Prepare draw
  //added
  double chi2Val=0;
  fcn_binnedChi2_2D(nPar, 0, chi2Val, arrPar, 0);
  printf("Chi2=%f \n",chi2Val);

  TCanvas canvas("canvas");
  TLatex *latex = new TLatex();
  latex->SetNDC();
    
  // Draw efficiency
  h2_rec.SetStats(1);// no stats box
  h2_rec.SetMaximum(effUpperBound);//Y axis maximum
  h2_rec.Draw("LEGO2");
  latex->DrawLatex(0.35,0.95,TString::Format("#varepsilon_{RECO} in Bin%d",iBin));
    
  // Draw FitResult
  f2_model.SetTitle("");
  f2_model.SetMaximum(effUpperBound);//model(function) efficiency maximum
  f2_model.SetLineWidth(1);
  //added
  latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
  latex->DrawLatex(0.01,0.90,TString::Format("DoF = %d",gMinuit->GetNumFreePars()));  

  
  f2_model.Draw("SURF SAME ");
  canvas.SaveSource(TString::Format("%s/recoEff_2D_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/recoEff_2D_bin%d.pdf",plotpath.Data(),iBin));

  //// Draw compare
  TH2F h2_compFit("h2_compFit","",6,thetaLBins,5,thetaKBins);//compare histogram
  h2_compFit.SetTitleOffset(2,"XY");
  h2_compFit.SetXTitle("genCosThetaL");
  h2_compFit.SetYTitle("genCosThetaK");
  for (int i = 1; i <= 6; i++) {//thetaL
    for (int j = 1; j <= 5; j++) {//thetaK
      if (h2_rec.GetBinContent(i,j) != 0){
	h2_compFit.SetBinContent(i,j,f2_model.Eval(h2_rec.GetXaxis()->GetBinCenter(i),h2_rec.GetYaxis()->GetBinCenter(j))/h2_rec.GetBinContent(i,j));
	//h2_comparefit= f2_model/h2_rec (pdf/data) 
      }else{
	h2_compFit.SetBinContent(i,j,0.);
      }
    }
  }
  h2_compFit.SetMinimum(0.);//Y axis minimum
  h2_compFit.SetStats(0);//no statbox
  h2_compFit.Draw("LEGO2");
  latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
  latex->DrawLatex(0.3,0.95,TString::Format("#varepsilon_{RECO,fit} / #varepsilon_{RECO,measured} in Bin%d",iBin));
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/recoEff_compFit_2D_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/recoEff_compFit_2D_bin%d.pdf",plotpath.Data(),iBin));

  // Draw significance of deviation
  TH1F h_pull("Deviation/Error","",15,-3.,3.);//pull distribution(significance)
  h_pull.SetXTitle("Significance of deviation");
  h_pull.SetYTitle("Angular bins");
  for (int i = 1; i <= 6; i++) {//thetaL
    for (int j = 1; j <= 5; j++) {//thetaK
      double _xlo = h2_rec.GetXaxis()->GetBinLowEdge(i);
      double _xhi = h2_rec.GetXaxis()->GetBinUpEdge(i);
      double _ylo = h2_rec.GetYaxis()->GetBinLowEdge(j);
      double _yhi = h2_rec.GetYaxis()->GetBinUpEdge(j);
      if (h2_rec.GetBinContent(i,j) != 0){
	h_pull.Fill((f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_rec.GetBinContent(i,j))/h2_rec.GetBinError(i,j));
	//significance = fitted pdf (f2_model) - data(h2_rec)/data error
      }
    }
  }
  h_pull.Draw();
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/recoEff_sigma_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/recoEff_sigma_bin%d.pdf",plotpath.Data(),iBin));

  // Clear
  delete latex;
  delete gMinuit;

  //prepare output
  std::vector<double> output;
  for (int iPar = 0; iPar < nPar; iPar++){
    output.push_back(arrPar[iPar]);
    output.push_back(arrParErr[iPar]);
        
    printf("%f,",arrPar[iPar]);
    if (iPar+1 >= nPar) printf("\n");
  }
  for (int i = 0; i < output.size(); i=i+2) {
    printf("%f,",output[i+1]);
    if (i+2 >= output.size()) printf("\n");
  }
  return output;
}//}}}


void createAcceptanceHist() // create acceptance histogram from UNFILTERED GEN.
{//{{{

  //  double accUpperBound = 0.09;//initializing
  double accUpperBound = 0.1;//Bin 0 and 6
  double gQ2 = 0;
  double gCosThetaK = 0;
  double gCosThetaL = 0;
  double gmuppt = 0;
  double gmupeta= 0;
  double gmupphi= 0;
  double gmumpt = 0;
  double gmumeta= 0;
  double gmumphi= 0;

  
  TChain *treein=new TChain("tree");
 treein->Add("/afs/cern.ch/work/d/dsahoo/scratch/Run2_BsToPhiMuMu_Updated/CMSSW_8_0_24/src/Bphysics/BsToPhiMuMu/plugins/unfiltered-genmc-ntuple/sel_BsToPhiMuMu_mc_Genonly_ntuple_mc_genonly_s0.root"); // fixed input

  if (treein == NULL) gSystem->Exit(0);
  treein->SetBranchStatus("*",0);
  treein->SetBranchStatus("genQ2"         , 1);
  treein->SetBranchStatus("genCosTheta*"  , 1);
  treein->SetBranchStatus("genMu*"        , 1);
  treein->SetBranchAddress("genQ2"        , &gQ2);
  treein->SetBranchAddress("genCosThetaK" , &gCosThetaK);
  treein->SetBranchAddress("genCosThetaL" , &gCosThetaL);
  treein->SetBranchAddress("genMupPt"     , &gmuppt);
  treein->SetBranchAddress("genMupEta"    , &gmupeta);
  treein->SetBranchAddress("genMupPhi"    , &gmupphi);
  treein->SetBranchAddress("genMumPt"     , &gmumpt);
  treein->SetBranchAddress("genMumEta"    , &gmumeta);
  treein->SetBranchAddress("genMumPhi"    , &gmumphi);
  
  // Create histograms
  TFile *fout = new TFile("acceptance_13TeV.root","RECREATE");
  float thetaKBins[6]={-1,-0.7,0.,0.4,0.8,1};
  float thetaLBins[7]={-1,-0.7,-0.3,0.,0.3,0.7,1};
  TH2F *h2_ngen[nQ2Ranges];
  TH2F *h2_nacc[nQ2Ranges];
  TH2F *h2_acc[nQ2Ranges];
  TH2F *h2_ngen_fine[nQ2Ranges];
  TH2F *h2_nacc_fine[nQ2Ranges];
  TH2F *h2_acc_fine[nQ2Ranges];
  TH1F *h_ngenL_fine[nQ2Ranges];
  TH1F *h_naccL_fine[nQ2Ranges];
  TH1F *h_accL_fine[nQ2Ranges];
  TH1F *h_ngenK_fine[nQ2Ranges];
  TH1F *h_naccK_fine[nQ2Ranges];
  TH1F *h_accK_fine[nQ2Ranges];
  for(int iBin = 0; iBin < nQ2Ranges; iBin++){
    h2_ngen[iBin] = new TH2F(TString::Format("h2_ngen_bin%d",iBin),"h2_ngen",6,thetaLBins,5,thetaKBins);
    h2_nacc[iBin] = new TH2F(TString::Format("h2_nacc_bin%d",iBin) ,"h2_nacc" ,6,thetaLBins,5,thetaKBins); 
    h2_acc [iBin] = new TH2F(TString::Format("h2_acc_bin%d",iBin),"",6,thetaLBins,5,thetaKBins);
    h2_ngen_fine[iBin] = new TH2F(TString::Format("h2_ngen_fine_bin%d",iBin),"h2_ngen",20,-1,1,20,-1,1);
    h2_nacc_fine[iBin] = new TH2F(TString::Format("h2_nacc_fine_bin%d",iBin) ,"h2_nacc" ,20,-1,1,20,-1,1); 
    h2_acc_fine[iBin]  = new TH2F(TString::Format("h2_acc_fine_bin%d",iBin),"",20,-1,1,20,-1,1);
    h_ngenL_fine[iBin] = new TH1F(TString::Format("h_ngenL_fine_bin%d",iBin),"h_ngenL",20,-1,1);
    h_naccL_fine[iBin] = new TH1F(TString::Format("h_naccL_fine_bin%d",iBin) ,"h_naccL" ,20,-1,1); 
    h_accL_fine[iBin]  = new TH1F(TString::Format("h_accL_fine_bin%d",iBin),"",20,-1,1);
    h_ngenK_fine[iBin] = new TH1F(TString::Format("h_ngenK_fine_bin%d",iBin),"h_ngenK",20,-1,1);
    h_naccK_fine[iBin] = new TH1F(TString::Format("h_naccK_fine_bin%d",iBin) ,"h_naccK" ,20,-1,1); 
    h_accK_fine[iBin]  = new TH1F(TString::Format("h_accK_fine_bin%d",iBin),"",20,-1,1);
    h2_ngen[iBin]->SetTitleOffset(2,"XYZ");
    h2_ngen[iBin]->SetXTitle("genCosThetaL");
    h2_ngen[iBin]->SetYTitle("genCosThetaK");
    h2_ngen[iBin]->SetZTitle("Generated events");
    h2_nacc[iBin]->SetTitleOffset(2,"XY");
    h2_nacc[iBin]->SetXTitle("genCosThetaL");
    h2_nacc[iBin]->SetYTitle("genCosThetaK");
    h2_nacc[iBin]->SetZTitle("Events in acceptance");
    h2_acc [iBin]->SetStats(0);
    h2_acc [iBin]->SetMinimum(0.);
    h2_acc [iBin]->SetMaximum(accUpperBound);
    h2_acc [iBin]->SetTitleOffset(2,"XY");
    h2_acc [iBin]->SetXTitle("genCosThetaL");
    h2_acc [iBin]->SetYTitle("genCosThetaK");
    h2_acc [iBin]->SetZTitle("Acceptance");
    h2_ngen_fine[iBin]->SetTitleOffset(2,"XYZ");
    h2_ngen_fine[iBin]->SetXTitle("genCosThetaL");
    h2_ngen_fine[iBin]->SetYTitle("genCosThetaK");
    h2_ngen_fine[iBin]->SetZTitle("Generated events");
    h2_nacc_fine[iBin]->SetTitleOffset(2,"XY");
    h2_nacc_fine[iBin]->SetXTitle("genCosThetaL");
    h2_nacc_fine[iBin]->SetYTitle("genCosThetaK");
    h2_nacc_fine[iBin]->SetZTitle("Events in acceptance");
    h2_acc_fine [iBin]->SetStats(0);
    h2_acc_fine [iBin]->SetMinimum(0.);
    h2_acc_fine [iBin]->SetMaximum(accUpperBound);
    h2_acc_fine [iBin]->SetTitleOffset(2,"XY");
    h2_acc_fine [iBin]->SetXTitle("genCosThetaL");
    h2_acc_fine [iBin]->SetYTitle("genCosThetaK");
    h2_acc_fine [iBin]->SetZTitle("Acceptance");
    h_ngenL_fine[iBin]->SetXTitle("genCosThetaL");
    h_ngenL_fine[iBin]->SetZTitle("Generated events");
    h_naccL_fine[iBin]->SetXTitle("genCosThetaL");
    h_naccL_fine[iBin]->SetZTitle("Events in acceptance");
    h_accL_fine [iBin]->SetStats(0);
    h_accL_fine [iBin]->SetMinimum(0.);
    h_accL_fine [iBin]->SetMaximum(accUpperBound);
    h_accL_fine [iBin]->SetXTitle("genCosThetaL");
    h_accL_fine [iBin]->SetZTitle("Acceptance");
    h_ngenK_fine[iBin]->SetXTitle("genCosThetaK");
    h_ngenK_fine[iBin]->SetZTitle("Generated events");
    h_naccK_fine[iBin]->SetXTitle("genCosThetaK");
    h_naccK_fine[iBin]->SetZTitle("Events in acceptance");
    h_accK_fine [iBin]->SetStats(0);
    h_accK_fine [iBin]->SetMinimum(0.);
    h_accK_fine [iBin]->SetMaximum(accUpperBound);
    h_accK_fine [iBin]->SetXTitle("genCosThetaK");
    h_accK_fine [iBin]->SetZTitle("Acceptance");
  }

  // Fill histograms
  // Read data
  for (int entry = 0; entry < treein->GetEntries(); entry++) {
  treein->GetEntry(entry);
  for(int iBin = 0; iBin < nQ2Ranges; iBin++){
      if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
      if (gQ2 > q2rangeup[2] && gQ2 < q2rangedn[2]) continue;//jpsi 
      if (gQ2 > q2rangeup[4] && gQ2 < q2rangedn[4]) continue;//psi2s
      h2_ngen[iBin]->Fill(gCosThetaL,gCosThetaK);//filling histogram with events generated
      h2_ngen_fine[iBin]->Fill(gCosThetaL,gCosThetaK);
      h_ngenL_fine[iBin]->Fill(gCosThetaL);
      h_ngenK_fine[iBin]->Fill(gCosThetaK);
      if ( fabs(gmupeta) < 2.5 && gmuppt > 2.5 && fabs(gmumeta) < 2.5 && gmumpt > 2.5){ //cuts applied at gen level(single muon selections) 
	h2_nacc[iBin]->Fill(gCosThetaL,gCosThetaK);//filling histogram with evenets at gen level
	h2_nacc_fine[iBin]->Fill(gCosThetaL,gCosThetaK);
	h_naccL_fine[iBin]->Fill(gCosThetaL);
	h_naccK_fine[iBin]->Fill(gCosThetaK);
      }
    }
  }

  for(int iBin = 0; iBin < nQ2Ranges; iBin++){
    // Calculate acceptance
    //h2_acc[iBin]->SetAxisRange(0.,1.,"Z");
    for (int i = 1; i <= 6; i++) {
      for (int j = 1; j <= 5; j++) {
	// Fill acceptance
	if (h2_ngen[iBin]->GetBinContent(i,j) == 0) {
	  printf("WARNING: Acceptance(%d,%d)=%f/%f\n",i,j,h2_nacc[iBin]->GetBinContent(i,j),h2_ngen[iBin]->GetBinContent(i,j));
	  h2_acc[iBin]->SetBinContent(i,j,0.);
	  h2_acc[iBin]->SetBinError(i,j,1.);
	}else{
	  h2_acc[iBin]->SetBinContent(i,j,h2_nacc[iBin]->GetBinContent(i,j)/h2_ngen[iBin]->GetBinContent(i,j));
//acceptance = events after singlemuon selections at gen level divided by events generated
//	  cout<<"Acceptance efficiency: "<<h2_acc[iBin]->GetBinContent(i,j)<<endl;
	  if (h2_nacc[iBin]->GetBinContent(i,j) != 0){
	    h2_acc[iBin]->SetBinError(i,j,sqrt(h2_acc[iBin]->GetBinContent(i,j)*(1.-h2_acc[iBin]->GetBinContent(i,j))/h2_ngen[iBin]->GetBinContent(i,j)));//acceptance error
	  }else{
	    h2_acc[iBin]->SetBinError(i,j,0.);
	  }
	}
      }
    }
    printf("INFO: h2_acc_bin%d built.\n",iBin);
        
    //h2_acc_fine[iBin]->SetAxisRange(0.,1.,"Z");
    for (int i = 1; i <= 20; i++) {//L
      for (int j = 1; j <= 20; j++) {//K
	// Fill acceptance
	if (h2_ngen_fine[iBin]->GetBinContent(i,j) == 0) {
	  h2_acc_fine[iBin]->SetBinContent(i,j,0.);
	  h2_acc_fine[iBin]->SetBinError(i,j,1.);
	}else{
	  h2_acc_fine[iBin]->SetBinContent(i,j,h2_nacc_fine[iBin]->GetBinContent(i,j)/h2_ngen_fine[iBin]->GetBinContent(i,j));
	  if (h2_nacc_fine[iBin]->GetBinContent(i,j) != 0){
	    h2_acc_fine[iBin]->SetBinError(i,j,sqrt(h2_acc_fine[iBin]->GetBinContent(i,j)*(1.-h2_acc_fine[iBin]->GetBinContent(i,j))/h2_ngen_fine[iBin]->GetBinContent(i,j)));
	  }else{
	    h2_acc_fine[iBin]->SetBinError(i,j,0.);
	  }
	}
                
      }
            
      //acceptance in 1D (costheta L and costheta K)
      // 1-D
      if (h_ngenL_fine[iBin]->GetBinContent(i) == 0) {
	h_accL_fine[iBin]->SetBinContent(i,0.);
	h_accL_fine[iBin]->SetBinError(i,1.);
      }else{
	h_accL_fine[iBin]->SetBinContent(i,h_naccL_fine[iBin]->GetBinContent(i)/h_ngenL_fine[iBin]->GetBinContent(i));
	if (h_naccL_fine[iBin]->GetBinContent(i) != 0){
	  h_accL_fine[iBin]->SetBinError(i,sqrt(h_accL_fine[iBin]->GetBinContent(i)*(1.-h_accL_fine[iBin]->GetBinContent(i))/h_ngenL_fine[iBin]->GetBinContent(i)));
	}else{
	  h_accL_fine[iBin]->SetBinError(i,0.);
	}
      }
            
    }
    for (int i = 1; i <= 20; i++) {//K
      // 1-D
      if (h_ngenK_fine[iBin]->GetBinContent(i) == 0) {
	h_accK_fine[iBin]->SetBinContent(i,0.);
	h_accK_fine[iBin]->SetBinError(i,1.);
      }else{
	h_accK_fine[iBin]->SetBinContent(i,h_naccK_fine[iBin]->GetBinContent(i)/h_ngenK_fine[iBin]->GetBinContent(i));
	if (h_naccK_fine[iBin]->GetBinContent(i) != 0){
	  h_accK_fine[iBin]->SetBinError(i,sqrt(h_accK_fine[iBin]->GetBinContent(i)*(1.-h_accK_fine[iBin]->GetBinContent(i))/h_ngenK_fine[iBin]->GetBinContent(i)));
	}else{
	  h_accK_fine[iBin]->SetBinError(i,0.);
	}
      }
    }
    printf("INFO: h2_acc_fine_bin%d built.\n",iBin);
  }
  fout->Write();
  fout->Close();
}//}}}


void createRecoEffHist(int iBin) // create reco efficiency histogram from official MC sample.
{//{{{
  printf("Evaluate reconstruction efficiency for bin#%d\n",iBin);

  //  double effUpperBound = 0.002;//initializing
  double effUpperBound = 0.015;
  if (iBin == 3 || iBin == 5) effUpperBound = 0.002;
  double BMass = 0;
  double Mumumass = 0;
  double Mumumasserr = 0;
  double gQ2 = 0;
  double gCosThetaK = 0;
  double gCosThetaL = 0;
  double CosThetaK = 0;
  double CosThetaL = 0;
  double gmuppt = 0;
  double gmupeta= 0;
  double gmumpt = 0;
  double gmumeta= 0;
  int triggers=0;

  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("Bmass"         , 1);
  ch->SetBranchStatus("Mumumass"      , 1);
  ch->SetBranchStatus("Mumumasserr"   , 1);
  ch->SetBranchStatus("genQ2"         , 1);
  ch->SetBranchStatus("genCosTheta*"  , 1);
  ch->SetBranchStatus("genMu*"        , 1);
  ch->SetBranchStatus("Triggers"      , 1);
  ch->SetBranchStatus("CosTheta*"  , 1);
  ch->SetBranchAddress("Bmass"        , &BMass);
  ch->SetBranchAddress("Mumumass"     , &Mumumass);
  ch->SetBranchAddress("Mumumasserr"  , &Mumumasserr);
  ch->SetBranchAddress("genQ2"        , &gQ2);
  ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
  ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
  ch->SetBranchAddress("CosThetaK"    , &CosThetaK);
  ch->SetBranchAddress("CosThetaL"    , &CosThetaL);
  ch->SetBranchAddress("genMupPt"     , &gmuppt);
  ch->SetBranchAddress("genMupEta"    , &gmupeta);
  ch->SetBranchAddress("genMumPt"     , &gmumpt);
  ch->SetBranchAddress("genMumEta"    , &gmumeta);
  ch->SetBranchAddress("Triggers"     , &triggers);

  int nacc(0), nreco(0);
  
  // Fill histograms
  const int nKBins = 20;
  const int nLBins = 20;

  TH2F h2_nacc("h2_nacc","h2_nacc",nLBins,-1,1,nKBins,-1,1);//acceptance histogram 
  TH2F h2_nreco("h2_nreco","h2_nreco",nLBins,-1,1,nKBins,-1,1);//reco histogram
  for (int entry = 0; entry < ch->GetEntries(); entry++) {
    ch->GetEntry(entry);
    if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
    if (gQ2 > q2rangeup[2] && gQ2 < q2rangedn[2]) continue;//jpsi
    if (gQ2 > q2rangeup[4] && gQ2 < q2rangedn[4]) continue;//psi2s  

    if ( fabs(gmumeta) < 2.5 && fabs(gmupeta) < 2.5 && gmumpt > 2.5 && gmuppt > 2.5 ) {
      h2_nacc.Fill(gCosThetaL,gCosThetaK);
//acceptance at gen level with singlemuon selection
      nacc++;}
    if (triggers > 0 && BMass != 0 && ((Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-3.5*Mumumasserr)) ){
      h2_nreco.Fill(CosThetaL,CosThetaK);//reco w/o Jpsi and Psip 
      nreco++;
    }
  }
  cout << ">>>>>>>"<< nacc << "\t" << nreco << endl;

  // Calculate efficiency
  TH2F h2_rec("h2_rec","",nLBins,-1,1,nKBins,-1,1);
  for (int i = 1; i <= nLBins; i++) {
    for (int j = 1; j <= nKBins; j++) {
      // Build from MC samples
      if (h2_nacc.GetBinContent(i,j) == 0 || h2_nreco.GetBinContent(i,j) == 0) {
	printf("WARNING: Efficiency(%d,%d)=0, set error to be 1.\n",i,j);
	h2_rec.SetBinContent(i,j,0.);
	h2_rec.SetBinError(i,j,1.);
      }else{
	h2_rec.SetBinContent(i,j,h2_nreco.GetBinContent(i,j)/h2_nacc.GetBinContent(i,j));
	//cout<<"No.of events after single muon selection: "<<h2_nacc.GetBinContent(i,j)<<endl;                                                   
	// cout<<"No.of events after reconstruction: "<<h2_nreco.GetBinContent(i,j)<<endl; 
	//	cout<<"Reco efficiency : "<<h2_rec.GetBinContent(i,j)<<endl;//added
//reco effieciency=  no.of events at reco level divided by no.of events after single muon at gen level
	h2_rec.SetBinError(i,j,sqrt(h2_rec.GetBinContent(i,j)*(1-h2_rec.GetBinContent(i,j))/h2_nreco.GetBinContent(i,j)));
	printf("INFO: Efficiency(%d,%d)=%f +- %f.\n",i,j,h2_rec.GetBinContent(i,j),h2_rec.GetBinError(i,j));
      }
    }
  }
  //  printf("Efficiency(%d,%d)=%f +- %f.\n",h2_rec.GetBinContent(CosThetaL,CosThetaK),h2_rec.GetBinError(CosThetaL,CosThetaK));

  h2_rec.SetTitleOffset(2,"XY");
  h2_rec.SetXTitle("CosThetaL");
  h2_rec.SetYTitle("CosThetaK");
  h2_rec.SetMaximum(effUpperBound);//added
  h2_rec.SetStats(1);
  gStyle->SetOptStat("nemr");
  h2_rec.SetMinimum(0.);
  
  
  TH1F h_recL("h_recL","",nLBins,-1,1);
  for (int i = 1; i <= nLBins; i++) {
    double nacc = 0;
    double nreco = 0;
    for (int j = 1; j <= nKBins; j++) {
      nacc+= h2_nacc.GetBinContent(i,j);
      nreco+= h2_nreco.GetBinContent(i,j);
    }
    if (nacc !=0 ){
      h_recL.SetBinContent(i,nreco/nacc);
      h_recL.SetBinError(i,sqrt(h_recL.GetBinContent(i)*(1-h_recL.GetBinContent(i))/nacc));
    }else{
      h_recL.SetBinContent(i,0);
      h_recL.SetBinError(i,1);
    }
  }
  h_recL.SetStats(1);
  //  h_recL.SetStats("e");
  h_recL.SetMinimum(0.);
  h_recL.SetMaximum(effUpperBound);//added
  h_recL.SetXTitle("CosThetaL");

  //  cout<<h2_rec.GetYaxis()<<endl;
  //  cout<<h2_rec.GetEntries()<<endl; 
  /*TPaveText* paveText = new TPaveText(0.75, 0.82, 1., 1., "NDC"); 
  paveText->SetBorderSize(0);
  paveText->SetFillColor(kWhite);
  paveText->AddText(Form("entries = %.0f " , nreco)); 
  paveText->Draw(); 
  */
  TH1F h_recK("h_recK","",nKBins,-1,1);
  for (int i = 1; i <= nKBins; i++) {
    double nacc = 0;
    double nreco = 0;
    for (int j = 1; j <= nLBins; j++) {
      nacc+= h2_nacc.GetBinContent(j,i);
      nreco+= h2_nreco.GetBinContent(j,i);
    }
    if (nacc !=0 ){
      h_recK.SetBinContent(i,nreco/nacc);
      //      cout<<"acceptance efficiency on cosk: "<<h_recK.GetBinContent(i)<<endl;//added
      h_recK.SetBinError(i,sqrt(h_recK.GetBinContent(i)*(1-h_recK.GetBinContent(i))/nacc));
    }else{
      h_recK.SetBinContent(i,0);
      h_recK.SetBinError(i,1);
    }
  }
  h_recK.SetStats(1);
  h_recK.SetMaximum(effUpperBound);//added
  h_recK.SetMinimum(0.);
  h_recK.SetXTitle("CosThetaK");
  
  // Print
  TCanvas canvas("canvas");
  //  h2_rec.Draw("LEGO2 TEXT");
  h2_rec.Draw("LEGO2");
  canvas.SaveSource(TString::Format("%s/recoEff_2D_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/recoEff_2D_bin%d.pdf",plotpath.Data(),iBin));
  
  h_recL.Draw("TEXT");
  h_recL.Draw();
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/recoEff_cosl_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/recoEff_cosl_bin%d.pdf",plotpath.Data(),iBin));
  //  h_recK.Draw("TEXT");
  h_recK.Draw();
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/recoEff_cosk_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/recoEff_cosk_bin%d.pdf",plotpath.Data(),iBin));
}//}}}


std::string accXrecoEff2(int iBin, bool keepParam = true) // acceptance*reconstruction efficiency
{//{{{
  switchRedirectStdio(TString::Format("%s/accXrecoEff2_stdout_bin%d.log",odatacardpath.Data(),iBin).Data(),"w",stdout);
  switchRedirectStdio(TString::Format("%s/accXrecoEff2_stderr_bin%d.log",odatacardpath.Data(),iBin).Data(),"w",stderr);

  printf("Evaluate full efficiency for bin#%d\n",iBin);
  // cut3-8TeV
  double effUpperBound = 0.00015;

  if (iBin == 3 || iBin == 5) effUpperBound = 2e-5;
  double BMass = 0;
  double PhiMass = 0;
  double Mumumass = 0;
  double Mumumasserr = 0;
  double gQ2 = 0;
  double gCosThetaK = 0;
  double gCosThetaL = 0;
  double CosThetaK = 0;
  double CosThetaL = 0;
  double gmuppt = 0;
  double gmupeta= 0;
  double gmupphi= 0;
  double gmumpt = 0;
  double gmumeta= 0;
  double gmumphi= 0;
  int    triggers=0;


  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("Bmass"         , 1);
  ch->SetBranchStatus("Phimass"      , 1);
  ch->SetBranchStatus("Mumumass"      , 1);
  ch->SetBranchStatus("Mumumasserr"   , 1);
  ch->SetBranchStatus("genQ2"         , 1);
  ch->SetBranchStatus("genCosTheta*"  , 1);
  ch->SetBranchStatus("CosTheta*"  , 1);
  ch->SetBranchStatus("genMu*"        , 1);
  ch->SetBranchStatus("Triggers"      , 1);
  ch->SetBranchAddress("Bmass"        , &BMass);
  ch->SetBranchAddress("Phimass"      , &PhiMass);
  ch->SetBranchAddress("Mumumass"     , &Mumumass);
  ch->SetBranchAddress("Mumumasserr"  , &Mumumasserr);
  ch->SetBranchAddress("genQ2"        , &gQ2);
  ch->SetBranchAddress("genCosThetaK" , &gCosThetaK);
  ch->SetBranchAddress("genCosThetaL" , &gCosThetaL);
  ch->SetBranchAddress("CosThetaK"    , &CosThetaK);
  ch->SetBranchAddress("CosThetaL"    , &CosThetaL);
  ch->SetBranchAddress("genMupPt"     , &gmuppt);
  ch->SetBranchAddress("genMupEta"    , &gmupeta);
  ch->SetBranchAddress("genMupPhi"    , &gmupphi);
  ch->SetBranchAddress("genMumPt"     , &gmumpt);
  ch->SetBranchAddress("genMumEta"    , &gmumeta);
  ch->SetBranchAddress("genMumPhi"    , &gmumphi);
  ch->SetBranchAddress("Triggers"     , &triggers);

  // Load acceptance
  TFile f_acc("acceptance_13TeV.root");//loading acceptance efficiency root which is created by createAcceptanceHist function
  TH2F *h2_acc = (TH2F*)f_acc.Get(TString::Format("h2_acc_bin%d",iBin));//2D acceptance hist (gencosthetaK, gencosthetaL)
  TH1F *h_accL = (TH1F*)f_acc.Get(TString::Format("h_accL_fine_bin%d",iBin));//1D acceptance hist(gencosthetaL)
  TH1F *h_accK = (TH1F*)f_acc.Get(TString::Format("h_accK_fine_bin%d",iBin));//1D acceptance hist(gencosthetaK)
  TH2F *h2_ngen = (TH2F*)f_acc.Get(TString::Format("h2_ngen_bin%d",iBin));//2D generated events hist (gencosthetaK, gencosthetaL)
  TH1F *h_ngenL = (TH1F*)f_acc.Get(TString::Format("h_ngenL_fine_bin%d",iBin));//1D generated events hist (gencosthetaL)
  TH1F *h_ngenK = (TH1F*)f_acc.Get(TString::Format("h_ngenK_fine_bin%d",iBin));//1D generated events hist (gencosthetaK)

  // Fill histograms
  float thetaKBins[6]={-1,-0.7,0.,0.4,0.8,1};//for 2D histogram
  float thetaLBins[7]={-1,-0.7,-0.3,0.,0.3,0.7,1};//for 2D histogram
  TH2F h2_nacc("h2_nacc" ,"h2_nacc" ,6,thetaLBins,5,thetaKBins); 
  TH2F h2_nreco("h2_nreco","h2_nreco",6,thetaLBins,5,thetaKBins);
  int nLBins = 20;// The same value as h_acc( for 1D histogram)
  int nKBins = 20;//for 1D histogram
  TH1F h_naccL("h_naccL" ,"h_naccL" ,nLBins,-1,1); 
  TH1F h_nrecoL("h_nrecoL","h_nrecoL",nLBins,-1,1);
  TH1F h_naccK("h_naccK" ,"h_naccK" ,nKBins,-1,1); 
  TH1F h_nrecoK("h_nrecoK","h_nrecoK",nKBins,-1,1);
  h_naccL.SetStats(0);
  h_naccL.SetMinimum(0.);
  h_naccL.SetXTitle("CosThetaL");
  h_naccL.SetYTitle("#Events/0.2");
  h_nrecoL.SetStats(0);
  h_nrecoL.SetMinimum(0.);
  h_nrecoL.SetXTitle("CosThetaL");
  h_nrecoL.SetYTitle("#Events/0.2");
  h_naccK.SetStats(0);
  h_naccK.SetMinimum(0.);
  h_naccK.SetXTitle("CosThetaK");
  h_naccK.SetYTitle("#Events/0.2");
  h_nrecoK.SetStats(0);
  h_nrecoK.SetMinimum(0.);
  h_nrecoK.SetXTitle("CosThetaK");
  h_nrecoK.SetYTitle("#Events/0.2");
  for (int entry = 0; entry < ch->GetEntries(); entry++) {
    ch->GetEntry(entry);
    if (BMass < 5.1 || BMass > 5.6) continue;  ///////// added @NS 
    if (gQ2 > q2rangeup[2] && gQ2 < q2rangedn[2]) continue;//jpsi
    if (gQ2 > q2rangeup[4] && gQ2 < q2rangedn[4]) continue;//psi2s
    if (gQ2 > q2rangeup[iBin] || gQ2 < q2rangedn[iBin]) continue;
    //no.of events after single muon selection at gen level
    if ( fabs(gmupeta) < 2.5 && gmuppt > 2.5 && fabs(gmumeta) < 2.5 && gmumpt > 2.5){
      h2_nacc.Fill(gCosThetaL,gCosThetaK);
      h_naccL.Fill(gCosThetaL);
      h_naccK.Fill(gCosThetaK);
      //no.of events at reco level   /////// phimass cut added @NS
      if ( triggers > 0 && BMass != 0 && (PhiMass > 1.01 && PhiMass < 1.03) && ((Mumumass > 3.096916+3.5*Mumumasserr || Mumumass < 3.096916-5.5*Mumumasserr) && (Mumumass > 3.686109+3.5*Mumumasserr || Mumumass < 3.686109-3.5*Mumumasserr)) ){
      ////////////      if ( triggers > 0 && BMass != 0 ){
      ////////////      if ( BMass != 0 ){
	// isCDFcut: 1 for CDF . 2 for LHCb . 3 for 16Aug reOptimization . 4 for sel_v3p5
	if (isCDFcut == 0){
	  h2_nreco.Fill(CosThetaL,CosThetaK);
	  h_nrecoL.Fill(CosThetaL);
	  h_nrecoK.Fill(CosThetaK);
	}else if (isCDFcut==1){
	  if( (iBin < 3 && fabs(BMass-Mumumass-2.182)>0.16) || (iBin ==4 && fabs(BMass-Mumumass-1.593)>0.06) || (iBin > 5 && fabs(BMass-Mumumass-1.593)>0.06) ){
	    h2_nreco.Fill(CosThetaL,CosThetaK);
	    h_nrecoL.Fill(CosThetaL);
	    h_nrecoK.Fill(CosThetaK);
	  }
	}else if (isCDFcut==2 || isCDFcut==3){
	  h2_nreco.Fill(CosThetaL,CosThetaK);
	  h_nrecoL.Fill(CosThetaL);
	  h_nrecoK.Fill(CosThetaK);
	}else if (isCDFcut==4){         ////////// ???????
	  //Antiradiation cut
	  if( fabs(BMass-Mumumass-2.270)>0.170 && fabs(BMass-Mumumass-1.681)>0.08 ){   
	    h2_nreco.Fill(CosThetaL,CosThetaK);
	    h_nrecoL.Fill(CosThetaL);
	    h_nrecoK.Fill(CosThetaK);
	  }
	}

      }
    }
  }

  // Calculate efficiency
  TH1F h_recL("h_recL","",nLBins,-1,1);//filing 1D costhetaL reconstruction effiency
  for (int i = 1; i <= nLBins; i++) {
    h_recL.SetBinContent(i,h_nrecoL.GetBinContent(i)/h_naccL.GetBinContent(i));
    h_recL.SetBinError(i,sqrt(h_recL.GetBinContent(i)*(1-h_recL.GetBinContent(i))/h_naccL.GetBinContent(i)));
  }
  TH1F h_recK("h_recK","",nKBins,-1,1);//filing 1D costhetaK reconstruction effiency
  for (int i = 1; i <= nKBins; i++) {
    h_recK.SetBinContent(i,h_nrecoK.GetBinContent(i)/h_naccK.GetBinContent(i));
    h_recK.SetBinError(i,sqrt(h_recK.GetBinContent(i)*(1-h_recK.GetBinContent(i))/h_naccK.GetBinContent(i)));
  }

  //total efficiency 
  TH2F h2_eff("h2_eff","",6,thetaLBins,5,thetaKBins);
  for (int i = 1; i <= 6; i++) {//L
    for (int j = 1; j <= 5; j++) {//K
      // Build from MC samples
      if (h2_nacc.GetBinContent(i,j) == 0 || h2_nreco.GetBinContent(i,j) == 0) {
	printf("WARNING: Efficiency(%d,%d)=0, set error to be 1.\n",i,j);
	h2_eff.SetBinContent(i,j,0.);
	h2_eff.SetBinError(i,j,1.);
      }else{
	h2_eff.SetBinContent(i,j,h2_nreco.GetBinContent(i,j)/h2_nacc.GetBinContent(i,j)*h2_acc->GetBinContent(i,j));
	h2_eff.SetBinError(i,j,h2_eff.GetBinContent(i,j)*sqrt(-1./h2_nacc.GetBinContent(i,j)+1./h2_nreco.GetBinContent(i,j)+pow(h2_acc->GetBinError(i,j)/h2_acc->GetBinContent(i,j),2)));
	printf("INFO: Efficiency(%d,%d)=%f +- %f.\n",i,j,h2_eff.GetBinContent(i,j),h2_eff.GetBinError(i,j));
      }
    }
  }

  TH1F h_effL("h_effL","",nLBins,-1,1);
  for (int i = 1; i <= nLBins; i++) {
    if (h_naccL.GetBinContent(i) == 0 || h_nrecoL.GetBinContent(i) == 0) {
      printf("WARNING: EfficiencyL(%d)=0, set error to be 1.\n",i);
      h_effL.SetBinContent(i,0.);
      h_effL.SetBinError(i,1.);
    }else{
      h_effL.SetBinContent(i,h_recL.GetBinContent(i)*h_accL->GetBinContent(i));
      h_effL.SetBinError(i,h_effL.GetBinContent(i)*h_recL.GetBinError(i)/h_recL.GetBinContent(i));
      printf("INFO: EfficiencyL(%d)=%f +- %f.\n",i,h_effL.GetBinContent(i),h_effL.GetBinError(i));
    }
  }
  h_effL.SetStats(0);
  h_effL.SetMinimum(0.);
  h_effL.SetXTitle("CosThetaL");
  h_effL.SetYTitle("Efficiency");

  TH1F h_effK("h_effK","",nKBins,-1,1);
  for (int i = 1; i <= nKBins; i++) {
    if (h_naccK.GetBinContent(i) == 0 || h_nrecoK.GetBinContent(i) == 0) {
      printf("WARNING: EfficiencyK(%d)=0, set error to be 1.\n",i);
      h_effK.SetBinContent(i,0.);
      h_effK.SetBinError(i,1.);
    }else{
      h_effK.SetBinContent(i,h_recK.GetBinContent(i)*h_accK->GetBinContent(i));
      h_effK.SetBinError(i,h_effK.GetBinContent(i)*h_recK.GetBinError(i)/h_recK.GetBinContent(i));
      printf("INFO: EfficiencyK(%d)=%f +- %f.\n",i,h_effK.GetBinContent(i),h_effK.GetBinError(i));
    }
  }
  h_effK.SetStats(0);
  h_effK.SetMinimum(0.);
  h_effK.SetXTitle("CosThetaK");
  h_effK.SetYTitle("Efficiency");

  // Quick check order0 (decoupled)
  TF1 *f_effK_ord0 = new TF1("f_effK_ord0","pol6",-1,1);//y, pol6
  TF1 *f_effL_ord0 = 0;
  switch ( iBin ){
  case 0:
  case 1:
  case 8:
  case 12://summaryBin[1]
    f_effL_ord0 = new TF1("f_effL_ord0","[2]*exp(-0.5*((x-[0])/[1])**2)+[5]*exp(-0.5*((x-[3])/[4])**2)+[8]*exp(-0.5*((x-[6])/[7])**2)",-1,1);//x
    f_effL_ord0->SetParameter(1,0.5);// width must be non-zero.
    f_effL_ord0->SetParameter(4,0.5);// width must be non-zero.
    f_effL_ord0->SetParameter(7,0.5);// width must be non-zero.
    break;
  default:
    f_effL_ord0 = new TF1("f_effL_ord0","pol6",-1,1);//x

  }
  h_effL.Fit("f_effL_ord0","S");
  h_effK.Fit("f_effK_ord0","S");
  
  // Using pure TMinuit for order1+
  int nPar = 21;
  TMinuit *gMinuit = new TMinuit(nPar);
  h2_fcn = &h2_eff;
  gMinuit->SetFCN(fcn_binnedChi2_2D);
  
  
  // 3 Gaussians or 6th order polynomial as 0th order
  // (0~3th order Legendre poly of CosThetaK)*(0~6th order power poly of CosThetaL) as 1st order
  TString f2_model_format_ord0 = TString::Format("(%e*exp(-0.5*((x-(%e))/%e)**2)%+e*exp(-0.5*((x-(%e))/%e)**2)%+e*exp(-0.5*((x-(%e))/%e)**2))*(%e%+e*y%+e*y**2%+e*y**3%+e*y**4%+e*y**5%+e*y**6)",
						 f_effL_ord0->GetParameter(2),\
						 f_effL_ord0->GetParameter(0),\
						 f_effL_ord0->GetParameter(1),\
						 f_effL_ord0->GetParameter(5),\
						 f_effL_ord0->GetParameter(3),\
						 f_effL_ord0->GetParameter(4),\
						 f_effL_ord0->GetParameter(8),\
						 f_effL_ord0->GetParameter(6),\
						 f_effL_ord0->GetParameter(7),\
						 f_effK_ord0->GetParameter(0),\
						 f_effK_ord0->GetParameter(1),\
						 f_effK_ord0->GetParameter(2),\
						 f_effK_ord0->GetParameter(3),\
						 f_effK_ord0->GetParameter(4),\
						 f_effK_ord0->GetParameter(5),\
						 f_effK_ord0->GetParameter(6));
  switch(iBin){
  case 0:
  case 1:
  case 8:
  case 12:// summaryBin[1]
    break;
  default:
    f2_model_format_ord0 = TString::Format("(%e%+e*x%+e*x**2%+e*x**3%+e*x**4%+e*x**5%+e*x**6)*(%e%+e*y%+e*y**2%+e*y**3%+e*y**4%+e*y**5%+e*y**6)",
					   f_effL_ord0->GetParameter(0),\
					   f_effL_ord0->GetParameter(1),\
					   f_effL_ord0->GetParameter(2),\
					   f_effL_ord0->GetParameter(3),\
					   f_effL_ord0->GetParameter(4),\
					   f_effL_ord0->GetParameter(5),\
					   f_effL_ord0->GetParameter(6),\
					   f_effK_ord0->GetParameter(0),\
					   f_effK_ord0->GetParameter(1),\
					   f_effK_ord0->GetParameter(2),\
					   f_effK_ord0->GetParameter(3),\
					   f_effK_ord0->GetParameter(4),\
					   f_effK_ord0->GetParameter(5),\
					   f_effK_ord0->GetParameter(6));
    break;
  }
  printf("DEBUG\t\t: f2_model_format_ord0=%s\n",f2_model_format_ord0.Data());
  TString f2_model_format_ord1 = "([0]+[1]*y+[2]*(3*y**2-1)/2+[3]*(5*y**3-3*y)/2)+([4]+[5]*y+[6]*(3*y**2-1)/2+[7]*(5*y**3-3*y)/2)*x**2+([8]+[9]*y+[10]*(3*y**2-1)/2+[11]*(5*y**3-3*y)/2)*x**3+([12]+[13]*y+[14]*(3*y**2-1)/2+[15]*(5*y**3-3*y)/2)*x**4+([16]+[17]*y+[18]*(3*y**2-1)/2+[19]*(5*y**3-3*y)/2)*x**6";
  TF2 f2_model_ord0("f2_model_ord0",f2_model_format_ord0,-1.,1.,-1.,1.);
  TF2 f2_model_ord1("f2_model_ord1",f2_model_format_ord1,-1.,1.,-1.,1.);
  TF2 f2_model("f2_model",TString::Format("[20]*%s*(1+%s)",f2_model_format_ord0.Data(),f2_model_format_ord1.Data()).Data(),-1.,1.,-1.,1.);
  f2_fcn = &f2_model;
  gMinuit->DefineParameter( 0, "k0l0",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 1, "k1l0",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 2, "k2l0",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 3, "k3l0",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 4, "k0l2",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 5, "k1l2",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 6, "k2l2",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 7, "k3l2",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 8, "k0l3",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter( 9, "k1l3",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter(10, "k2l3",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter(11, "k3l3",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter(12, "k0l4",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter(13, "k1l4",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter(14, "k2l4",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter(15, "k3l4",   0.,  1E-4,    -1E+1, 1E+1);
  gMinuit->DefineParameter(16, "k0l6",   0.,  1E-4,    -1E+2, 1E+3);
  gMinuit->DefineParameter(17, "k1l6",   0.,  1E-4,    -1E+2, 1E+3);
  gMinuit->DefineParameter(18, "k2l6",   0.,  1E-4,    -1E+2, 1E+3);
  gMinuit->DefineParameter(19, "k3l6",   0.,  1E-4,    -1E+2, 1E+3);
  gMinuit->DefineParameter(20, "ord0",8000.,  1E-2,       10, 1E+6);


  // Calculte normalize factor for 0th order
  for (int i = 1; i < 21; i++) {
    gMinuit->Command(TString::Format("SET PARM %d 0",i));
    gMinuit->Command(TString::Format("FIX %d",i));
  }
  gMinuit->Command("MINI");
  gMinuit->Command("MINI");
  gMinuit->Command("MINOS");
  gMinuit->Command("FIX 21");

  // Start processing 1st order
  for (int i = 1; i < 21; i++) {gMinuit->Command(TString::Format("REL %d",i));}
  if (iBin == 0 ) {
    gMinuit->Command("SET PARM 9 0");
    gMinuit->Command("SET PARM 10 0");
    gMinuit->Command("SET PARM 11 0");
    gMinuit->Command("SET PARM 12 0");
    gMinuit->Command("FIX 9");
    gMinuit->Command("FIX 10");
    gMinuit->Command("FIX 11");
    gMinuit->Command("FIX 12");

    gMinuit->Command("FIX 13");
    gMinuit->Command("FIX 14");
    gMinuit->Command("FIX 15");
    gMinuit->Command("FIX 16");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else if (iBin == 1) {
    gMinuit->Command("SET PARM 9 0");
    gMinuit->Command("SET PARM 10 0");
    gMinuit->Command("SET PARM 11 0");
    gMinuit->Command("SET PARM 12 0");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 9");
    gMinuit->Command("FIX 10");
    gMinuit->Command("FIX 11");
    gMinuit->Command("FIX 12");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else if (iBin == 7 || iBin == 9) {
    gMinuit->Command("SET PARM 13 0");
    gMinuit->Command("SET PARM 14 0");
    gMinuit->Command("SET PARM 15 0");
    gMinuit->Command("SET PARM 16 0");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 13");
    gMinuit->Command("FIX 14");
    gMinuit->Command("FIX 15");
    gMinuit->Command("FIX 16");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else if (iBin == 10) {
    gMinuit->Command("SET PARM 9 0");
    gMinuit->Command("SET PARM 13 0");
    gMinuit->Command("SET PARM 15 0");
    gMinuit->Command("SET PARM 16 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 9");
    gMinuit->Command("FIX 13");
    gMinuit->Command("FIX 15");
    gMinuit->Command("FIX 16");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else if (iBin > 1 || iBin == summaryBin[1]) {
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }else{
    gMinuit->Command("SET PARM 13 0");
    gMinuit->Command("SET PARM 14 0");
    gMinuit->Command("SET PARM 15 0");
    gMinuit->Command("SET PARM 16 0");
    gMinuit->Command("SET PARM 17 0");
    gMinuit->Command("SET PARM 18 0");
    gMinuit->Command("SET PARM 19 0");
    gMinuit->Command("SET PARM 20 0");
    gMinuit->Command("FIX 13");
    gMinuit->Command("FIX 14");
    gMinuit->Command("FIX 15");
    gMinuit->Command("FIX 16");
    gMinuit->Command("FIX 17");
    gMinuit->Command("FIX 18");
    gMinuit->Command("FIX 19");
    gMinuit->Command("FIX 20");
  }

  gMinuit->Command("MINI");
  gMinuit->Command("MINI");
  gMinuit->Command("IMPROVE");
  gMinuit->Command("MINOS");
  
  double arrPar[nPar];
  double arrParErr[nPar];
  for (int iPar = 0; iPar < nPar; iPar++) gMinuit->GetParameter(iPar,arrPar[iPar],arrParErr[iPar]);
  for (int iPar = 0; iPar < nPar; iPar++) f2_model.SetParameter(iPar,arrPar[iPar]);

  double covMatrix[nPar][nPar];
  gMinuit->mnemat(&covMatrix[0][0],nPar);
  TVectorD cenVec(nPar);
  double covMatrix1D[nPar*nPar];
  TMatrixDSym errMtx(0,nPar-1, covMatrix1D);//arrPar[nPar-1] is normalization.
  for (int iPar=0; iPar<nPar; iPar++){
    cenVec[iPar] = arrPar[iPar];
    for (int jPar=0; jPar<nPar; jPar++){
      if (arrPar[iPar] == 0 || arrPar[jPar] == 0){
	errMtx[iPar][jPar] = 1e-30; // Just pick an extermely small number.
      }else{
	errMtx[iPar][jPar] = covMatrix[iPar][jPar];
      }
    }
  }
  cenVec.Print();
  errMtx.Print();
  
  // Draw and write config
  TCanvas canvas("canvas");
  TLatex *latex = new TLatex();
  latex->SetNDC();
  double chi2Val=0;
  fcn_binnedChi2_2D(nPar, 0, chi2Val, arrPar, 0);
  printf("Chi2(Bin center)=%f \n",chi2Val);
        
  h_effL.Draw();
  ///if (iBin == 0) h_effL.SetMaximum(0.001);
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/accXrecoEff2_cosl_order0_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/accXrecoEff2_cosl_order0_bin%d.pdf",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/accXrecoEff2_cosl_order0_bin%d.png",plotpath.Data(),iBin));
  h_effK.Draw();
  canvas.Update();
  canvas.SaveSource(TString::Format("%s/accXrecoEff2_cosk_order0_bin%d.cc",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/accXrecoEff2_cosk_order0_bin%d.pdf",plotpath.Data(),iBin));
  canvas.Print(TString::Format("%s/accXrecoEff2_cosk_order0_bin%d.png",plotpath.Data(),iBin));
  
  if (keepParam){
    //// Draw 1-D
    h_naccL.Draw();
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_naccL_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_naccL_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_naccL_bin%d.png",plotpath.Data(),iBin));
    h_naccK.Draw();
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_naccK_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_naccK_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_naccK_bin%d.png",plotpath.Data(),iBin));

    h_nrecoL.Draw();
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_nrecoL_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_nrecoL_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_nrecoL_bin%d.png",plotpath.Data(),iBin));
    h_nrecoK.Draw();
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_nrecoK_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_nrecoK_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_nrecoK_bin%d.png",plotpath.Data(),iBin));
        
    h_recL.SetStats(0);
    h_recL.SetMinimum(0.);
    h_recL.SetXTitle("CosThetaL");
    h_recL.Draw("TEXT");
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_recoEffL_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_recoEffL_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_recoEffL_bin%d.png",plotpath.Data(),iBin));

    h_recK.SetStats(0);
    h_recK.SetMinimum(0.);
    h_recK.SetXTitle("CosThetaK");
    h_recK.Draw("TEXT");
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_recoEffK_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_recoEffK_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_recoEffK_bin%d.png",plotpath.Data(),iBin));
        
    TH1F h_theoK("h_theoK" ,"h_theoK" ,nKBins,-1,1); 
    h_theoK.SetStats(0);
    h_theoK.SetMinimum(0.);
    h_theoK.SetXTitle("CosThetaK");
    h_theoK.SetYTitle("#Events / 0.2");
    for (int kBin = 1; kBin <= nKBins; kBin++) {
      h_theoK.SetBinContent(kBin,h_ngenK->GetBinContent(kBin)*h_effK.GetBinContent(kBin));
      if (h_effK.GetBinContent(kBin) != 0){
	h_theoK.SetBinError(kBin,h_theoK.GetBinContent(kBin)*h_effK.GetBinError(kBin)/h_effK.GetBinContent(kBin));
      }else{
	h_theoK.SetBinError(kBin,sqrt(h_theoK.GetBinContent(kBin)));
      }
    }
    h_theoK.Draw();
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_theoK_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_theoK_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_theoK_bin%d.png",plotpath.Data(),iBin));

    TH1F h_theoL("h_theoL" ,"h_theoL" ,nLBins,-1,1); 
    h_theoL.SetStats(0);
    h_theoL.SetMinimum(0.);
    h_theoL.SetXTitle("CosThetaL");
    h_theoL.SetYTitle("#Events / 0.2");
    for (int lBin = 1; lBin <= nLBins; lBin++) {
      h_theoL.SetBinContent(lBin,h_ngenL->GetBinContent(lBin)*h_effL.GetBinContent(lBin));
      if (h_effL.GetBinContent(lBin) != 0){
	h_theoL.SetBinError(lBin,h_theoL.GetBinContent(lBin)*h_effL.GetBinError(lBin)/h_effL.GetBinContent(lBin));
      }else{
	h_theoL.SetBinError(lBin,sqrt(h_theoL.GetBinContent(lBin)));
      }
    }
    h_theoL.Draw();
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_theoL_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_theoL_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_theoL_bin%d.png",plotpath.Data(),iBin));
        
    //// Draw 2-D
    h2_eff.SetMinimum(0.);
    h2_eff.SetTitleOffset(2,"XY");
    h2_eff.SetXTitle("CosThetaL");
    h2_eff.SetYTitle("CosThetaK");
    h2_eff.SetStats(0);
    h2_eff.SetMaximum(effUpperBound);
    h2_eff.Draw("LEGO2");
    latex->DrawLatex(0.35,0.95,TString::Format("#varepsilon in Bin%d",iBin));
    
    // Draw FitResult
    f2_model.SetTitle("");
    f2_model.SetMaximum(effUpperBound);
    f2_model.SetLineWidth(1);
    f2_model.Draw("SURF SAME ");
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_2D_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_2D_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_2D_bin%d.png",plotpath.Data(),iBin));

    //// Draw compare
    TH2F h2_compFit("h2_compFit","",6,thetaLBins,5,thetaKBins);
    h2_compFit.SetTitleOffset(2,"XY");
    h2_compFit.SetXTitle("CosThetaL");
    h2_compFit.SetYTitle("CosThetaK");
    TH2F h2_pullFit("h2_pullFit","",6,thetaLBins,5,thetaKBins);
    h2_pullFit.SetTitleOffset(1,"XY");
    h2_pullFit.SetXTitle("CosThetaL");
    h2_pullFit.SetYTitle("CosThetaK");
    for (int i = 1; i <= 6; i++) {//thetaL
      for (int j = 1; j <= 5; j++) {//thetaK
	if (h2_eff.GetBinContent(i,j) != 0){
	  h2_compFit.SetBinContent(i,j,f2_model.Eval(h2_eff.GetXaxis()->GetBinCenter(i),h2_eff.GetYaxis()->GetBinCenter(j))/h2_eff.GetBinContent(i,j));
	  double _xlo = h2_eff.GetXaxis()->GetBinLowEdge(i);
	  double _xhi = h2_eff.GetXaxis()->GetBinUpEdge(i);
	  double _ylo = h2_eff.GetYaxis()->GetBinLowEdge(j);
	  double _yhi = h2_eff.GetYaxis()->GetBinUpEdge(j);
	  h2_pullFit.SetBinContent(i,j,(f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_eff.GetBinContent(i,j))/h2_eff.GetBinError(i,j));
	}else{
	  h2_compFit.SetBinContent(i,j,0.);
	  h2_pullFit.SetBinContent(i,j,0.);
	}
      }
    }
    h2_compFit.SetMinimum(0.);
    h2_compFit.SetStats(0);
    h2_compFit.Draw("LEGO2");
    latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
    latex->DrawLatex(0.3,0.95,TString::Format("#varepsilon_{fit} / #varepsilon_{measured} in Bin%d",iBin));
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_compFit_2D_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_compFit_2D_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_compFit_2D_bin%d.png",plotpath.Data(),iBin));
        
    h2_pullFit.SetStats(0);
    h2_pullFit.Draw("COLZ TEXT");
    latex->DrawLatex(0.01,0.95,TString::Format("#chi^{2} = %f",chi2Val));
    latex->DrawLatex(0.3,0.95,TString::Format("(#varepsilon_{fit} - #varepsilon_{measured})/Error in Bin%d",iBin));
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_pullFit_2D_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_pullFit_2D_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_pullFit_2D_bin%d.png",plotpath.Data(),iBin));

    // Draw significance of deviation
    TH1F h_pull("Deviation/Error","",15,-3.,3.);
    h_pull.SetXTitle("Significance of deviation");
    for (int i = 1; i <= 6; i++) {//thetaL
      for (int j = 1; j <= 5; j++) {//thetaK
	double _xlo = h2_eff.GetXaxis()->GetBinLowEdge(i);
	double _xhi = h2_eff.GetXaxis()->GetBinUpEdge(i);
	double _ylo = h2_eff.GetYaxis()->GetBinLowEdge(j);
	double _yhi = h2_eff.GetYaxis()->GetBinUpEdge(j);
	if (h2_eff.GetBinContent(i,j) != 0){
	  h_pull.Fill((f2_model.Integral(_xlo,_xhi,_ylo,_yhi)/(_xhi-_xlo)/(_yhi-_ylo)-h2_eff.GetBinContent(i,j))/h2_eff.GetBinError(i,j));
	}
      }
    }
    h_pull.Draw();
    canvas.Update();
    canvas.SaveSource(TString::Format("%s/accXrecoEff2_sigma_bin%d.cc",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_sigma_bin%d.pdf",plotpath.Data(),iBin));
    canvas.Print(TString::Format("%s/accXrecoEff2_sigma_bin%d.png",plotpath.Data(),iBin));

    delete latex;
        
  }
    
  // prepare output
  string out_accXrecoEff2_ord0;
  out_accXrecoEff2_ord0 = TString::Format("%e*(%s)",arrPar[20],f2_model_format_ord0.Data());
  while(out_accXrecoEff2_ord0.find("*y") !=std::string::npos ){
    out_accXrecoEff2_ord0.replace(out_accXrecoEff2_ord0.find("*y"), 2, "*CosThetaK");
  }
  while(out_accXrecoEff2_ord0.find("*x") !=std::string::npos ){
    out_accXrecoEff2_ord0.replace(out_accXrecoEff2_ord0.find("*x"), 2, "*CosThetaL");
  }
  while(out_accXrecoEff2_ord0.find("(x") !=std::string::npos ){
    out_accXrecoEff2_ord0.replace(out_accXrecoEff2_ord0.find("(x"), 2, "(CosThetaL");
  }
  printf("\"%s\",\n",out_accXrecoEff2_ord0.c_str());
  writeParam(iBin,"f_accXrecoEff_ord0",out_accXrecoEff2_ord0);
  writeParam(iBin,"accXrecoEff2",arrPar,20,true);
  writeParam(iBin,"accXrecoEff2Err",arrParErr,20,true);
  // write to wspace as well
  if (keepParam){
    RooRealVar CosThetaK("CosThetaK", "cos#theta_{K}", -1., 1.);
    RooRealVar CosThetaL("CosThetaL", "cos#theta_{L}", -1., 1.);
    RooRealVar fl("fl", "F_{L}", genFl[iBin], -100, 100.);// unbounded fl
    RooRealVar a6("a6", "A_{6}", genA6[iBin], -100., 100.);// unbounded afb
    RooRealVar recK0L0("recK0L0","recK0L0",arrPar[ 0]);
    RooRealVar recK1L0("recK1L0","recK1L0",arrPar[ 1]);
    RooRealVar recK2L0("recK2L0","recK2L0",arrPar[ 2]);
    RooRealVar recK3L0("recK3L0","recK3L0",arrPar[ 3]);
    RooRealVar recK0L2("recK0L2","recK0L2",arrPar[ 4]);
    RooRealVar recK1L2("recK1L2","recK1L2",arrPar[ 5]);
    RooRealVar recK2L2("recK2L2","recK2L2",arrPar[ 6]);
    RooRealVar recK3L2("recK3L2","recK3L2",arrPar[ 7]);
    RooRealVar recK0L3("recK0L3","recK0L3",arrPar[ 8]);
    RooRealVar recK1L3("recK1L3","recK1L3",arrPar[ 9]);
    RooRealVar recK2L3("recK2L3","recK2L3",arrPar[10]);
    RooRealVar recK3L3("recK3L3","recK3L3",arrPar[11]);
    RooRealVar recK0L4("recK0L4","recK0L4",arrPar[12]);
    RooRealVar recK1L4("recK1L4","recK1L4",arrPar[13]);
    RooRealVar recK2L4("recK2L4","recK2L4",arrPar[14]);
    RooRealVar recK3L4("recK3L4","recK3L4",arrPar[15]);
    RooRealVar recK0L6("recK0L6","recK0L6",arrPar[16]);
    RooRealVar recK1L6("recK1L6","recK1L6",arrPar[17]);
    RooRealVar recK2L6("recK2L6","recK2L6",arrPar[18]);
    RooRealVar recK3L6("recK3L6","recK3L6",arrPar[19]);
    RooRealVar effNorm("effNorm","effNorm",arrPar[20]);
    recK0L0.setError(arrPar[ 0]);
    recK1L0.setError(arrPar[ 1]);
    recK2L0.setError(arrPar[ 2]);
    recK3L0.setError(arrPar[ 3]);
    recK0L2.setError(arrPar[ 4]);
    recK1L2.setError(arrPar[ 5]);
    recK2L2.setError(arrPar[ 6]);
    recK3L2.setError(arrPar[ 7]);
    recK0L3.setError(arrPar[ 8]);
    recK1L3.setError(arrPar[ 9]);
    recK2L3.setError(arrPar[10]);
    recK3L3.setError(arrPar[11]);
    recK0L4.setError(arrPar[12]);
    recK1L4.setError(arrPar[13]);
    recK2L4.setError(arrPar[14]);
    recK3L4.setError(arrPar[15]);
    recK0L6.setError(arrPar[16]);
    recK1L6.setError(arrPar[17]);
    recK2L6.setError(arrPar[18]);
    recK3L6.setError(arrPar[19]);
    effNorm.setError(arrPar[20]);
    RooArgSet f_sigA_argset(CosThetaL,CosThetaK);
    f_sigA_argset.add(RooArgSet(fl,a6));
    f_sigA_argset.add(RooArgSet(recK0L0,recK1L0,recK2L0,recK3L0));
    f_sigA_argset.add(RooArgSet(recK0L2,recK1L2,recK2L2,recK3L2));
    RooArgSet f_sigA_MK_argset(CosThetaK);
    f_sigA_MK_argset.add(RooArgSet(fl));
    TString f_sigA_format;
    TString f_sigA_MK_format;

    ///////// WRONG, MODIFY ?????
    //////TString f_ang_format = "9/16*( (1/2*(1-fl)*(1-genCosThetaK*genCosThetaK)*(1+genCosThetaL*genCosThetaL)) + (2*fl*genCosThetaK*genCosThetaK*(1-genCosThetaL*genCosThetaL)) + (a6*(1-genCosThetaK*genCosThetaK)*genCosThetaL) )";// unbounded fl, afb, transformed as.

    TString f_ang_format = "9/16*( (1/2*(0.5+TMath::ATan(fl)/TMath::Pi())*(1-CosThetaK**2)*(1+CosThetaL**2)) + (2*(0.5+TMath::ATan(fl)/TMath::Pi())*CosThetaK**2*(1-CosThetaL**2)) + ((0.5-TMath::ATan(fl)/TMath::Pi())*2*(TMath::ATan(a6)/TMath::Pi())*(1-CosThetaK**2)*CosThetaL) )";  // unbounded fl, afb, transformed as.

    //// K* mu+ mu- 
    ////TString f_ang_format = "9/16*((2/3*fs+4/3*as*2*sqrt(3*fs*(1-fs)*(0.5+TMath::ATan(fl)/TMath::Pi()))*CosThetaK)*(1-CosThetaL**2)+(1-fs)*(2*(0.5+TMath::ATan(fl)/TMath::Pi())*CosThetaK**2*(1-CosThetaL**2)+0.5*(0.5-TMath::ATan(fl)/TMath::Pi())*(1-CosThetaK**2)*(1+CosThetaL**2)+4/3*(3/2*(1/2-TMath::ATan(fl)/TMath::Pi())*TMath::ATan(afb)/TMath::Pi())*(1-CosThetaK**2)*CosThetaL))";// unbounded fl, afb, transformed as.


    TString f_accXrecoEff2_ord0 = out_accXrecoEff2_ord0;
    TString f_accXrecoEff2_format, f_accXrecoEff2_L0, f_accXrecoEff2_L2, f_accXrecoEff2_L3, f_accXrecoEff2_L4, f_accXrecoEff2_L6;
    f_accXrecoEff2_L0 = "(recK0L0+recK1L0*CosThetaK+recK2L0*(3*CosThetaK**2-1)/2+recK3L0*(5*CosThetaK**3-3*CosThetaK)/2)";
    f_accXrecoEff2_L2 = "(recK0L2+recK1L2*CosThetaK+recK2L2*(3*CosThetaK**2-1)/2+recK3L2*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**2";
    f_accXrecoEff2_L3 = "(recK0L3+recK1L3*CosThetaK+recK2L3*(3*CosThetaK**2-1)/2+recK3L3*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**3";
    f_accXrecoEff2_L4 = "(recK0L4+recK1L4*CosThetaK+recK2L4*(3*CosThetaK**2-1)/2+recK3L4*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**4";
    f_accXrecoEff2_L6 = "(recK0L6+recK1L6*CosThetaK+recK2L6*(3*CosThetaK**2-1)/2+recK3L6*(5*CosThetaK**3-3*CosThetaK)/2)*CosThetaL**6";

    /////// MODIFY ???
    /////////TString f_ang_MK_format = "3/4*((2/3*fs+4/3*as*CosThetaK)+(1-fs)*(2*(1/2+TMath::ATan(fl)/TMath::Pi())*CosThetaK**2+(1/2-TMath::ATan(fl)/TMath::Pi())*(1-CosThetaK**2)))";// unbounded fl, integrated over cosThetaL
    /////TString f_ang_MK_format = "(27/32)*TMath::Pi()*(1-CosThetaK**2+(0.5+TMath::ATan(fl)/TMath::Pi())*((7/3)*CosThetaK**2-1))";// unbounded fl, integrated over cosThetaL

    //// taken from paper 
    TString f_ang_MK_format = "(3/4*(0.5-TMath::ATan(fl)/TMath::Pi())*(1-CosThetaK**2)+3/2*(0.5+TMath::ATan(fl)/TMath::Pi())*CosThetaK**2)";// unbounded fl, integrated over cosThetaL

    TString f_accXrecoEff2_MK_format = TString::Format("(%e%+e*CosThetaK%+e*CosThetaK**2%+e*CosThetaK**3%+e*CosThetaK**4%+e*CosThetaK**5%+e*CosThetaK**6)",
						       f_effK_ord0->GetParameter(0),\
						       f_effK_ord0->GetParameter(1),\
						       f_effK_ord0->GetParameter(2),\
						       f_effK_ord0->GetParameter(3),\
						       f_effK_ord0->GetParameter(4),\
						       f_effK_ord0->GetParameter(5),\
						       f_effK_ord0->GetParameter(6));
    f_sigA_MK_format = TString::Format("%s*%s", f_accXrecoEff2_MK_format.Data(), f_ang_MK_format.Data());

    if (iBin == 0 || iBin == 10 || iBin == summaryBin[1]) {
      f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
      f_sigA_argset.add(RooArgSet(recK0L6,recK1L6,recK2L6,recK3L6));
      f_sigA_format = TString::Format("%s*(1+%s+%s+%s+%s)*%s",f_accXrecoEff2_ord0.Data(),f_accXrecoEff2_L0.Data(),f_accXrecoEff2_L2.Data(),f_accXrecoEff2_L4.Data(),f_accXrecoEff2_L6.Data(),f_ang_format.Data());
    }else if (iBin == 1) {
      f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
      f_sigA_format = TString::Format("%s*(1+%s+%s+%s)*%s",f_accXrecoEff2_ord0.Data(),f_accXrecoEff2_L0.Data(),f_accXrecoEff2_L2.Data(),f_accXrecoEff2_L4.Data(),f_ang_format.Data());
    }else if (iBin == 7) {
      f_sigA_argset.add(RooArgSet(recK0L3,recK1L3,recK2L3,recK3L3));
      f_sigA_format = TString::Format("%s*(1+%s+%s+%s)*%s",f_accXrecoEff2_ord0.Data(),f_accXrecoEff2_L0.Data(),f_accXrecoEff2_L2.Data(),f_accXrecoEff2_L3.Data(),f_ang_format.Data());
    }else if ((iBin > 1 && iBin < 6) ){ 
      f_sigA_argset.add(RooArgSet(recK0L3,recK1L3,recK2L3,recK3L3));
      f_sigA_argset.add(RooArgSet(recK0L4,recK1L4,recK2L4,recK3L4));
      f_sigA_format = TString::Format("%s*(1+%s+%s+%s+%s)*%s",f_accXrecoEff2_ord0.Data(),f_accXrecoEff2_L0.Data(),f_accXrecoEff2_L2.Data(),f_accXrecoEff2_L3.Data(),f_accXrecoEff2_L4.Data(),f_ang_format.Data());
    }else{
      f_sigA_argset.add(RooArgSet(recK0L3,recK1L3,recK2L3,recK3L3));
      f_sigA_format = TString::Format("%s*(1+%s+%s+%s)*%s",f_accXrecoEff2_ord0.Data(),f_accXrecoEff2_L0.Data(),f_accXrecoEff2_L2.Data(),f_accXrecoEff2_L3.Data(),f_ang_format.Data());
    }
    // angular map of signal
    RooGenericPdf f_sigA("f_sigA", f_sigA_format, f_sigA_argset);
    RooGenericPdf f_sigA_MK("f_sigA_MK", f_sigA_MK_format, f_sigA_MK_argset);

    // angular map of signal
    RooWorkspace *wspace = new RooWorkspace("wspace","wspace");
    wspace->import(f_sigA);
    wspace->import(f_sigA_MK);
    wspace->import(effNorm);
    wspace->import(errMtx,"errMtx");
    wspace->writeToFile(TString::Format("%s/wspace_sigA_bin%d.root",owspacepath.Data(),iBin),true);
  }
    
  delete gMinuit;// delete before return.
  switchRedirectStdio("_stdio");
  return out_accXrecoEff2_ord0.c_str();
}//}}}


//_______________________________________________________________________________

void angular2D_bin(int iBin, const char outfile[] = "angular2D")
{//{{{
  // Remark: You must use RooFit!! It's better in unbinned fit.
  //         Extended ML fit is adopted by Mauro, just follow!!
  if (iBin == 2 || iBin == 4) return;
  switchRedirectStdio(TString::Format("%s/angular2D_bin_stdout_bin%d.log",odatacardpath.Data(),iBin).Data(),"w",stdout);
  switchRedirectStdio(TString::Format("%s/angular2D_bin_stderr_bin%d.log",odatacardpath.Data(),iBin).Data(),"w",stderr);

  // Read data
  ch->SetBranchStatus("*",0);
  ch->SetBranchStatus("Bmass"         , 1);
  ch->SetBranchStatus("Phimass"       , 1);
  ch->SetBranchStatus("Mumumass"      , 1);
  ch->SetBranchStatus("Mumumasserr"   , 1);
  ch->SetBranchStatus("CosTheta*"     , 1);
  ch->SetBranchStatus("Q2"            , 1);
  ch->SetBranchStatus("Triggers"      , 1);
  RooRealVar Bmass("Bmass","m_{#phi#mu^{+}#mu^{-}}", 5.1, 5.6);
  RooRealVar Phimass("Phimass","m_{#phi(1020)}", 0., 2.);
  RooRealVar CosThetaK("CosThetaK"     , "cos#theta_{K}"       , -1. , 1.   ) ;
  RooRealVar CosThetaL("CosThetaL"     , "cos#theta_{L}"       , -1. , 1.   ) ;
  RooRealVar Mumumass("Mumumass","M^{#mu#mu}",0.,10.);
  RooRealVar Mumumasserr("Mumumasserr","Error of M^{#mu#mu}",0.,10.);
  RooRealVar Q2("Q2","q^{2}",0.5,20.);
  RooRealVar Triggers("Triggers","",0,100);

  TFile *f_wspace_sigA = new TFile(TString::Format("%s/wspace_sigA_bin%d.root",iwspacepath.Data(),iBin));
  RooWorkspace *wspace_sigA = (RooWorkspace*)f_wspace_sigA->Get("wspace");
  RooGenericPdf *f_sigA = 0;
  RooRealVar *fl = 0;
  RooRealVar *a6 = 0;
  if (wspace_sigA){
    printf("@@@@ opening root file \n");
    f_sigA = (RooGenericPdf*)wspace_sigA->pdf("f_sigA");
    fl = (RooRealVar*)wspace_sigA->var("fl");
    a6 = (RooRealVar*)wspace_sigA->var("a6");
    fl->setRange(-100,100);// unbounded fl
    ////////////////fl->setVal(0.6);
    fl->setConstant(kFALSE);
    a6->setRange(-100,100);// unbounded afb
    ///////////////a6->setVal(0.9);
    a6->setConstant(kFALSE);
  }else{
    printf("ERROR\t\t: Please have wspace_sigA_bin?.root prepared.\n");
    return;
  }

  RooRealVar nsig("nsig","nsig",1E4,1E2,1E8);
  RooExtendPdf f_ext("f_ext","f_ext",*f_sigA,nsig);
    
  // Get data and apply unbinned fit
  int mumuMassWindowBin = 1+2*isCDFcut;
  if (isCDFcut < 0) mumuMassWindowBin=0;
  RooDataSet *data = new RooDataSet("data", "data", ch, RooArgSet(Q2, Bmass, Phimass, Mumumass, Mumumasserr, CosThetaK, CosThetaL, Triggers),TString::Format("(%s) && (%s) && (%s) && (%s)", nTriggeredPath[2], q2range[iBin], phiMassWindow[1], mumuMassWindow[mumuMassWindowBin]), 0);
  if (data->sumEntries() == 0){
    return;
  }
  printf("@@@@@ RooDataset formed\n");


  // Fitting procedure in TMinuit
  double isMigradConverge[2] = {-1,0};
  double isMinosValid = -1;
  RooAbsReal *nll = f_ext.createNLL(*data,Extended(kTRUE),Offset(kFALSE),NumCPU(4));
  RooMinuit minuit(*nll);
  printf("INFO\t\t: Start MIGRAD loop\n");
  for(int iLoop = 0; iLoop < 10; iLoop++){
    isMigradConverge[0] = minuit.migrad();
    printf("INFO\t\t: MIGRAD return code=%.0f\n",isMigradConverge[0]);
    if (isMigradConverge[0] == 0) break;
  }
  isMigradConverge[1] = minuit.save()->minNll();
  writeParam(iBin, "migrad2D", isMigradConverge);
  double isHesseValid = minuit.hesse();
  writeParam(iBin, "hesse2D", &isHesseValid, 1);
  minuit.save();
  double val[4]={0,0,0,0};
  val[0] = fl->getVal();val[1] = fl->getError();val[2]=fl->getErrorLo();val[3]=fl->getErrorHi();
  writeParam(iBin, "fl_hesse2D", val, 4);
  val[0] = a6->getVal();val[1] = a6->getError();val[2]=a6->getErrorLo();val[3]=a6->getErrorHi();
  writeParam(iBin, "a6_hesse2D",val, 4);
  printf("INFO\t\t: Start MINOS loop\n");
  for(int iLoop = 0; iLoop < 5; iLoop++){
    isMinosValid = minuit.minos(RooArgSet(*a6,*fl));
    printf("INFO\t\t: MINOS return code=%.0f\n",isMinosValid);
    if (isMinosValid == 0) break;
  }
  writeParam(iBin, "minos2D", &isMinosValid, 1);

  // Draw the frame on the canvas
  TCanvas* c = new TCanvas("c");
  TLatex *t1 = new TLatex();
  t1->SetNDC();
    
  RooPlot* framecosk = CosThetaK.frame(); 
  data->plotOn(framecosk,Binning(20)); 
  f_ext.plotOn(framecosk);


  /* 
  if (false) { // Draw dashed line using generator level values
    double buffFl = fl->getVal();
    double buffA6 = a6->getVal();
    fl->setVal(genFl[iBin]); a6->setVal(genA6[iBin]);
    f_ext.plotOn(framecosk, LineColor(2),LineWidth(2),LineStyle(2)); 
    fl->setVal(buffFl); a6->setVal(buffA6);
  }
  */
  framecosk->SetTitle("");
  framecosk->SetMinimum(0);
  framecosk->Draw();

  double fixNDC = 0;
  if (iBin > 3) fixNDC = -0.5;

  ////// MODIFY ??
  double fl_bdd[3] = {toBoundedFl(fl->getVal()),toBoundedFl(fl->getVal()+fl->getErrorHi()),toBoundedFl(fl->getVal()+fl->getErrorLo())};
  fl_bdd[1] -= fl_bdd[0];
  fl_bdd[2] -= fl_bdd[0];

  ///// MODIFY ??
  double a6_bdd[3] = {toBoundedA6(a6->getVal(),fl->getVal()),toBoundedA6(a6->getVal()+a6->getErrorHi(),fl->getVal()),toBoundedA6(a6->getVal()+a6->getErrorLo(),fl->getVal())};
  a6_bdd[1] -= a6_bdd[0];
  a6_bdd[2] -= a6_bdd[0];
  t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
  t1->DrawLatex(.35,.80+fixNDC,TString::Format("F_{L,ubd}=%.3f%+5f%+5f",fl_bdd[0],fl_bdd[1],fl_bdd[2]));
  t1->DrawLatex(.35,.74+fixNDC,TString::Format("A_{6,ubd}=%.3f%+5f%+5f",a6_bdd[0],a6_bdd[1],a6_bdd[2]));
  c->SaveSource(TString::Format("%s/%s_cosk_bin%d.cc",plotpath.Data(),outfile,iBin));
  c->Print(TString::Format("%s/%s_cosk_bin%d.pdf",plotpath.Data(),outfile,iBin));

  // Draw projection to CosThetaL
  RooPlot* framecosl = CosThetaL.frame(); 
  data->plotOn(framecosl,Binning(20)); 
  f_ext.plotOn(framecosl); 
  if (false) { // put generator level curve for comparison
    double buffFl = fl->getVal();
    double buffA6 = a6->getVal();
    fl->setVal(genFl[iBin]); a6->setVal(genA6[iBin]);
    f_ext.plotOn(framecosl, LineColor(2),LineWidth(2),LineStyle(2)); 
    fl->setVal(buffFl); a6->setVal(buffA6);
  }
  framecosl->SetTitle("");
  framecosl->SetMinimum(0);
  framecosl->Draw();

  fixNDC = 0.;
  t1->DrawLatex(.35,.86+fixNDC,TString::Format("%s",q2range[iBin]));
  t1->DrawLatex(.35,.80+fixNDC,TString::Format("F_{L,ubd}=%.3f%+5f%+5f",fl_bdd[0],fl_bdd[1],fl_bdd[2]));
  t1->DrawLatex(.35,.74+fixNDC,TString::Format("A_{6,ubd}=%.3f%+5f%+5f",a6_bdd[0],a6_bdd[1],a6_bdd[2]));
  c->Update();
  c->SaveSource(TString::Format("%s/%s_cosl_bin%d.cc",plotpath.Data(),outfile,iBin));
  c->Print(TString::Format("%s/%s_cosl_bin%d.pdf",plotpath.Data(),outfile,iBin));

  // Make 2-D plot
  TH1 *h1 = data->createHistogram("CosThetaL,CosThetaK", 6, 5);
  h1->SetXTitle("CosThetaL");
  h1->SetYTitle("CosThetaK");
  h1->Draw("LEGO2");
  c->Update();
  c->SaveSource(TString::Format("%s/%s_bin%d.cc",plotpath.Data(),outfile,iBin));
  c->Print(TString::Format("%s/%s_bin%d.pdf",plotpath.Data(),outfile,iBin));
    
  if (true){
    RooWorkspace *wspace = new RooWorkspace("wspace","wspace");
    nsig.setConstant(kTRUE);
    wspace->import(nsig);
    wspace->import(f_ext);
    wspace->writeToFile(TString::Format("%s/wspace_angular2D_bin%d.root",owspacepath.Data(),iBin),true);
  }

  //write output
  double output[4] = {0,0,0,0};
  output[0] = fl->getVal();
  output[1] = fl->getError();
  output[2] = fl->getErrorLo();
  output[3] = fl->getErrorHi();
  writeParam(iBin,"fl_2d",output,4);
  output[0] = a6->getVal();
  output[1] = a6->getError();
  output[2] = a6->getErrorLo();
  output[3] = a6->getErrorHi();
  writeParam(iBin,"a6_2d",output,4);
    
  // clear
  delete t1;
  delete c;
  delete data;

  switchRedirectStdio("_stdio");

  return;
}//}}}


void angular2D(const char outfile[] = "angular2D", bool doFit=false) // 2D check for signal MC at RECO level
{//{{{
  int nWorkBins = 5;
  int workBins[] = {0,1,3,5,6};
  double x[nWorkBins];
  double xerr[nWorkBins];
  double ya6[nWorkBins],yerra6Lo[nWorkBins],yerra6Hi[nWorkBins],yfl[nWorkBins],yerrflLo[nWorkBins],yerrflHi[nWorkBins];
  for(int iBin = 0; iBin < nWorkBins; iBin++){
    x[iBin] = (q2rangeup[workBins[iBin]]+q2rangedn[workBins[iBin]])/2;
    xerr[iBin] = (q2rangeup[workBins[iBin]]-q2rangedn[workBins[iBin]])/2;
  }

  if (doFit){
    for(int ibin = 0; ibin < nWorkBins; ibin++){
      angular2D_bin(workBins[ibin],outfile);
    }
  }

  // Checkout input data
  for(int ibin = 0; ibin < nWorkBins; ibin++){
    yfl[ibin] = -100;
    yerrflLo[ibin] = 0;
    yerrflHi[ibin] = 0;
    ya6[ibin] = -100;
    yerra6Lo[ibin] = 0;
    yerra6Hi[ibin] = 0;
    yfl[ibin]           = toBoundedFl(readParam(workBins[ibin],"fl_2d",0));
    yerrflLo[ibin]      = fabs(toBoundedFl(readParam(workBins[ibin],"fl_2d",0)+readParam(workBins[ibin],"fl_2d",2))-yfl[ibin]);
    yerrflHi[ibin]      = fabs(toBoundedFl(readParam(workBins[ibin],"fl_2d",0)+readParam(workBins[ibin],"fl_2d",3))-yfl[ibin]);
    if (yerrflHi[ibin] == -1){
      yerrflHi[ibin] = 0;
    }
    if (yerrflLo[ibin] == -1){
      yerrflLo[ibin] = 0;
    }
    ya6[ibin]          = toBoundedA6(readParam(workBins[ibin],"a6_2d",0),readParam(ibin,"fl_2d",0));
    yerra6Lo[ibin]     = fabs(toBoundedA6(readParam(workBins[ibin],"a6_2d",0)+readParam(workBins[ibin],"a6_2d",2),readParam(workBins[ibin],"fl_2d",0))-ya6[ibin]);
    yerra6Hi[ibin]     = fabs(toBoundedA6(readParam(workBins[ibin],"a6_2d",0)+readParam(workBins[ibin],"a6_2d",3),readParam(workBins[ibin],"fl_2d",0))-ya6[ibin]);
    if (yerra6Hi[ibin] == -1){
      yerra6Hi[ibin] = 0;
    }
    if (yerra6Lo[ibin] == -1){
      yerra6Lo[ibin] = 0;
    }
    printf("ya6[%d]=%6.4f + %6.4f - %6.4f\n",workBins[ibin],ya6[ibin],yerra6Hi[ibin],yerra6Lo[ibin]);
    printf("yfl [%d]=%6.4f + %6.4f - %6.4f\n",workBins[ibin],yfl[ibin],yerrflHi[ibin],yerrflLo[ibin]);
  }

  // Draw
  TCanvas *c = new TCanvas("c");

  TGraphAsymmErrors *g_fl  = new TGraphAsymmErrors(nWorkBins,x,yfl,xerr,xerr,yerrflLo,yerrflHi);
  g_fl->SetTitle("");
  g_fl->GetXaxis()->SetTitle("q^{2} [(GeV)^{2}]");
  g_fl->GetYaxis()->SetTitle("F_{L}");
  g_fl->GetYaxis()->SetRangeUser(0,1);
  g_fl->SetFillColor(2);
  g_fl->SetFillStyle(3001);
  g_fl->Draw("a2");
  g_fl->Draw("P");
  double work_genFl[nWorkBins];
  double work_genFlerr[nWorkBins];
  for(int ibin = 0; ibin < nWorkBins; ibin++){
    work_genFl[ibin] = genFl[workBins[ibin]];
    work_genFlerr[ibin] = genFlerr[workBins[ibin]];
  }
  TGraphAsymmErrors *gen_fl  = new TGraphAsymmErrors(nWorkBins,x,work_genFl,xerr,xerr,work_genFlerr,work_genFlerr);
  gen_fl->SetMarkerStyle(21);
  gen_fl->SetFillColor(4);
  gen_fl->SetFillStyle(3001);
  gen_fl->Draw("P2 same");
  c->SaveSource(TString::Format("%s/%s_fl.cc",plotpath.Data(),outfile));
  c->Print(TString::Format("%s/%s_fl.pdf",plotpath.Data(),outfile));
  c->Clear();

  TGraphAsymmErrors *g_a6 = new TGraphAsymmErrors(nWorkBins,x,ya6,xerr,xerr,yerra6Lo,yerra6Hi);
  g_a6->SetTitle("");
  g_a6->GetXaxis()->SetTitle("q^{2} [(GeV)^{2}]");
  g_a6->GetYaxis()->SetTitle("A_{6}");
  g_a6->GetYaxis()->SetRangeUser(-1,1);
  g_a6->SetFillColor(2);
  g_a6->SetFillStyle(3001);
  g_a6->Draw("a2");
  g_a6->Draw("P");
  double work_genA6[nWorkBins];
  double work_genA6err[nWorkBins];
  for(int ibin = 0; ibin < nWorkBins; ibin++){
    work_genA6[ibin] = genA6[workBins[ibin]];
    work_genA6err[ibin] = genA6err[workBins[ibin]];
  }
  TGraphAsymmErrors *gen_a6 = new TGraphAsymmErrors(nWorkBins,x,work_genA6,xerr,xerr,work_genA6err,work_genA6err);
  gen_a6->SetMarkerStyle(21);
  gen_a6->SetFillColor(4);
  gen_a6->SetFillStyle(3001);
  gen_a6->Draw("P2 same");
  c->SaveSource(TString::Format("%s/%s_a6.cc",plotpath.Data(),outfile));
  c->Print(TString::Format("%s/%s_a6.pdf",plotpath.Data(),outfile));
}//}}}






//_________________________________________________________________________________

void printListOfTChainElements(TChain *chain){
    TObjArray *fileElements=chain->GetListOfFiles();
    int nFiles = fileElements->GetEntries();
    //printf("DEBUG\t\t: %d files in the chain\n",nFiles);
    TIter next(fileElements);
    TChainElement *chEl=0;
    for( int entry=0; entry < nFiles; entry++ ) {
        chEl=(TChainElement*)next();
        printf("%s\n",chEl->GetTitle());
    }
    printf("DEBUG\t\t: %d files in the chain\n",nFiles);
}

void printHelpMessage(){
    printf("Usage              : ./fit Function infile [--options <arguments>]\n");
    printf("Functions          :\n");
    printf("    bmass                 Fit to mass spectrum using a double Gaussian signal and Chebyshev bkg.\n");
    printf("    fl_gen                Derive F_{L} from cosThetaK distribution at GEN level.\n");
    printf("    angular_gen           Derive F_{L} and A_{6} from angular distribution at GEN level.\n");
    printf("    acceptance           Get 2D acceptance efficiency map from signal simulation.\n");
    printf("    accXrecoEff           Get 2D efficiency map from signal simulation.\n");
    printf("    angular2D             Same as angular_gen, but fit to signal MC at RECO level with efficiency correction, bkg component is NOT considered.\n");
    printf("    angular3D_1a_Sm       Leading step1 to angular3D, determine signal shape from simulation.\n");
    printf("    angular3D_1b_YpPm     Leading step2 to angular3D, determine mass spectrum of peaking bkg from simulation.\n");
    printf("    angular3D_2a_PkPl     Leading step3 to angular3D, determine angular dist. of peaking bkg from simulation.\n");
    printf("    angular3D_prior       Leading step4 to angular3D, fit to data sideband to get initial values of combinatorial bkg.\n");
    printf("    angular3D_bins        Derive F_{L} and A_{6} by fitting to mass and angular distribution for each q2 bin.\n");
    printf("    angular3D             Draw F_{L} and A_{6} values in q2 bins.\n");
    printf("Expert functions   :\n")  ;
    printf("    createToys            Generate toys or split MC samples. Please check source code.\n");
    printf("    scanNLL               In case of low statistics, scan log(L) for better error determination. (VERY SLOW!)\n");
    printf("    createFCToys          Create toys for Feldman-Cousins method (Takes a long while!)\n");
    printf("    FCScan                Feldman-Cousins method for better error determination under the constraints of physical boundaries.\n");
    printf("Options     :\n");
    printf("    --help                Show this help message.\n");
    printf("    --keeplog             Redirect some messages to odatacardpath/???.log and odatacardpath/???.err.\n");
    printf("    --keepparam           Keep fit parameters.\n");
    printf("    --plotpath            Path to output plots.                               [ Default: \"./plots\" ]\n");
    printf("    --iwspacepath         Path to input  wspaces.                             [ Default: \".\" ]\n");
    printf("    --iCombBkgWspacepath  Path to input  wspaces of combinatorial background. [ Default: \".\" ]\n");
    printf("    --owspacepath         Path to output wspaces.                             [ Default: \".\" ]\n");
    printf("    --idatacardpath       Path to input  datacards.                           [ Default: \".\" ]\n");
    printf("    --odatacardpath       Path to output datacards.                           [ Default: \".\" ]\n");
    printf("    --iallpath            Path to input  datacards/wspaces.                   [ Default: \".\" ]\n");
    printf("    --oallpath            Path to output plots/datacards/wspaces.             [ Default: \".\" ]\n");
    printf("    --scale               Modify the scale factor of input. Negative for toys.[ Default: 1 ]\n");
    printf("Remark             :\n");
    printf("    1. Outputs will be by default stored in ./plots, please keep the directory.\n");
    printf("    2. Wildcard is allowed for infile. But you must quote infile like \"inputData_Run*.root\"!\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));
    RooRandom::randomGenerator()->SetSeed(rand());
    gStyle->SetOptStat(0);

    // Tags
    is7TeVCheck = false;   
    // less than 0 noCut, 1 for CDF . 2 for LHCb . 3 for 16Aug reOptimization . 4 for sel_v3p5
    isToy = false;
    isToy ? isCDFcut*=-1 : isCDFcut=4;
    //int nToy=500;
    int nWorkBins = 9;
    //int workBins[] = {0,1,2,3,4,5,6,7,8};


    // Help message
    if (argc <= 2) {
        printHelpMessage();
        return 0;
    }

    // main
    TString func    = argv[1];
    TString infile  = argv[2];
    
    // Processing arguments
    char* l_opt_arg;
    const char short_options[] = "hl";
    static struct option long_options[] = {
        {"help"               , 0 , NULL ,'h'} ,
        {"keeplog"            , 0 , NULL ,'l'} ,
	{"keepparam"          , 0 , NULL ,'p'} ,
	//	{"keepparam"          , 1 , NULL , 'p'} ,//added
        {"plotpath"           , 1 , NULL , 1 } ,
        {"idatacardpath"      , 1 , NULL , 2 } ,
        {"odatacardpath"      , 1 , NULL , 3 } ,
        {"iwspacepath"        , 1 , NULL , 4 } ,
        {"iCombBkgWspacepath" , 1 , NULL , 5 } ,
        {"owspacepath"        , 1 , NULL , 6 } ,
        {"iallpath"           , 1 , NULL , 7 } ,
        {"oallpath"           , 1 , NULL , 8 } ,
        {"scale"              , 1 , NULL , 9 } ,
        {NULL                 , 0 , NULL , 0 }
    };
    int arg;
    while ((arg = getopt_long(argc,argv,short_options,long_options,NULL)) != -1){
        switch (arg) {  
            case 'h':   
                printHelpMessage();
                return 0;
                break;  
            case 'l':
                struct stat fiBuff;
                if (stat("/dev/tty",&fiBuff)==0) redirectStdout = true;
                printf("INFO\t\t: Stdout/stderr are redirected to log files...\n");
                break;
            case 'p':
                gKeepParam = true;
                break;
            case 1:   
                l_opt_arg = optarg;
                plotpath=l_opt_arg;
                break;  
            case 2:   
                l_opt_arg = optarg;
                idatacardpath=l_opt_arg;
                break;  
            case 3:   
                l_opt_arg = optarg;
                odatacardpath=l_opt_arg;
                break;  
            case 4:   
                l_opt_arg = optarg;
                iwspacepath=l_opt_arg;
                iCombBkgWspacepath=l_opt_arg;
                break;  
            case 5:
                l_opt_arg = optarg;
                iCombBkgWspacepath=l_opt_arg;
                break;
            case 6:   
                l_opt_arg = optarg;
                owspacepath=l_opt_arg;
                break;  
            case 7:   
                l_opt_arg = optarg;
                idatacardpath=l_opt_arg;
                iwspacepath=l_opt_arg;
                iCombBkgWspacepath=l_opt_arg;
                break;  
            case 8:
                l_opt_arg = optarg;
                plotpath=l_opt_arg;
                odatacardpath=l_opt_arg;
                owspacepath=l_opt_arg;
                break;
            case 9:
                l_opt_arg = optarg;
                scaleFactor=atof(l_opt_arg);
                if (scaleFactor < 0){
                    isToy=true;
                    scaleFactor=fabs(scaleFactor);
                }
                break;
            default:
                printf("WARNING\t\t: %d is not a valid argument. Program terminates!\n",arg);
                abort();
        }
    }
    printf("INFO\t\t: ScaleFactor for input data is %.3f\n",scaleFactor);
    printf("INFO\t\t: Plots will be stored to %s\n",plotpath.Data());
    printf("INFO\t\t: Datacards will be stored to %s\n",odatacardpath.Data());
    printf("INFO\t\t: Workspaces will be stored to %s\n",owspacepath.Data());
  
    if (func == "fl_gen"){
      ch->Add(infile.Data());
      if (ch == NULL) return 1;
      const char outfile[]="fl_gen";
      fl_gen(outfile);
    }else if (func == "angular_gen"){
      ch->Add(infile.Data());
      if (ch == NULL) return 1;
      const char outfile[]="angular_gen";
      angular_gen(outfile);

    }else if (func == "acceptance"){
      ch->Add(infile.Data());
      if (ch == NULL) return 1;
      int nTempWorkBins = 9;
      int tempWorkBins[] = {0,1,2,3,4,5,6,7,8};
      for (int iBin = 0; iBin < nTempWorkBins; iBin++) {
        acceptance(tempWorkBins[iBin]);
      }

    }else if (func == "recoEff"){
      ch->Add(infile.Data());
      if (ch == NULL) return 1;
      int nTempWorkBins = 5;
      int tempWorkBins[] = {0,1,3,5,6};
      for (int iBin = 0; iBin < nTempWorkBins; iBin++) {
        recoEff(tempWorkBins[iBin]);
      }

    }else if (func == "createAcceptanceHist"){ 
      if (true) createAcceptanceHist();

    }else if (func == "createRecoEffHist"){
      ch->Add(infile.Data());
      if (ch == NULL) return 1;
      int nTempWorkBins = 5;
      int tempWorkBins[] = {0,1,3,5,6};
      for (int iBin = 0; iBin < nTempWorkBins; iBin++) {
        createRecoEffHist(tempWorkBins[iBin]);
      }

    }else if (func == "accXrecoEff"){
      // For experts only. Set to true if no given acceptance_13TeV.root and modify the input filepath in the funciton.
      if (true) createAcceptanceHist();
      ch->Add(infile.Data());
      printListOfTChainElements(ch);
      if (ch == NULL) return 1;
      int nTempWorkBins = 9;
      int tempWorkBins[] = {0,1,2,3,4,5,6,7,8};
      for (int iBin = 0; iBin < nTempWorkBins; iBin++) {
	accXrecoEff2(tempWorkBins[iBin],gKeepParam);

      }

      }else if (func == "angular2D"){
        static char wantDoFit[10];
        while(strcmp(wantDoFit,"y")*strcmp(wantDoFit, "n") != 0){
	  printf("Do you want to redo fitting? [y/n]:");
	  scanf("%19s",wantDoFit);
        }
        bool doFit = false;
        if (strcmp(wantDoFit,"y")==0){
	  doFit=true;
	  ch->Add(infile.Data());
	  if (ch == NULL) return 1;
        }
	const char outfile[]="angular2D";
	angular2D(outfile,doFit);
        //for (int iBin = 2; iBin < 9; iBin++) {
        //        //    if (iBin == 3 || iBin == 5) continue;
        //                //    angular2D_bin(iBin);
        //                        //}
	
    }else if (func == "test"){
      ch->Add(infile.Data());
      printListOfTChainElements(ch);
      //if (ch == NULL) return 1;
      const char outfile[]="test";
      for (int iBin = 0; iBin < nWorkBins; iBin++) {
      }
    }else if(func =="test2"){
      int nTempWorkBins = 7;
      int tempWorkBins[] = {0,1,3,5,6,7,8};
      for (int iBin = 0; iBin < nTempWorkBins; iBin++) {
	/////createRecoEffHist(tempWorkBins[iBin]);
	angular2D_bin(iBin);
      }
    }else{ 
      cerr << "No function available for: " << func.Data() << endl; 
    }
    printf("%lld entries processed.\n",ch->GetEntries());
    
    return 0 ;
}
