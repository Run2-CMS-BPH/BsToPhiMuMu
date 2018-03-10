//----------------------------------------------
// SELECTOR CODE FOR BSTOPHIMUMU ANALYSIS 
// @author: N.Sahoo, NISER, BHUBANESWAR

// to do: add phi momentum info to the analyzer
// 2017-07-11: added phi angle info 
// 2018-03-02: (a) commented out the extra print statements, (b) added trigger efficiency print statements
// 2018-03-09: added more counters to dump more infos at each step  (for ex. how many candidates per event after selection ?)
//----------------------------------------------
// further checks to do:
// - why sel. and trg. eff. is so low in case of J/psi and Psi' ? CHECK FOR SIGNAL, PSI ?
// - somehow the numbers after selector and at AOD don't match (double-check!!)
//----------------------------------------------

#define SingleBsToPhiMuMuSelector_cxx

#include <iostream>
#include <sstream>
#include <map>
#include "SingleBsToPhiMuMuSelector.h"
#include <TH2.h>
#include <TStyle.h>
#include <TProof.h>
#include <string.h>
#include <TLorentzVector.h>
#include <TVector3.h>

//-------------------
// Global Constants
//-------------------
const double PHI_MASS  = 1.01946; // GeV
const double PHI_WIDTH = 0.0508; // GeV 
const double MUON_MASS = 0.10565837;
const double KAON_MASS = 0.493677;

//-------------------------
// user defined variables
//-------------------------
TDatime t_begin_ , t_now_ ;
int n_processed_, n_selected_; 
int n_triggers0, n_triggers1;

int n_total_, n_passMuonID_, n_passSelCut_, n_passBestB_; 

TTree *tree_; 

//--------------------------------
// Branch variables for new tree
//--------------------------------
int    Nb             = 0;
double Mumumass       = 0;
double Mumumasserr    = 0;
double Phimass        = 0;
double Kmpt           = 0;
double Kppt           = 0;
double Kmtrkdcasigbs  = 0;
double Kptrkdcasigbs  = 0;

double Bmass          = 0;
double Bpt            = 0;
double Beta           = 0;
double Bphi           = 0;
double Bvtxcl         = 0;
double Blxysig        = 0;
double Bcosalphabs    = 0;
double Bcosalphabs2d  = 0;
double Bctau          = 0;

double Q2             = 0;
double dimupt         = 0;
double dimueta        = 0;
double CosThetaL      = 999;
double CosThetaK      = 999;
double Phi            = 999;
int    Triggers       = 0;

// Branches for Generator level information
double  genBPt       = 0;
double  genBEta      = 0;
double  genBPhi      = 0;

double  genMupPt     = 0;
double  genMupEta    = 0;
double  genMupPhi    = 0;
double  genMumPt     = 0;
double  genMumEta    = 0;
double  genMumPhi    = 0;

double  gendimuPt    = 0;
double  gendimuEta   = 0;
double  gendimuPhi   = 0;

double  genQ2        = 0;
double  genCosThetaL = 999;
double  genCosThetaK = 999;
double  genPhi       = 999;


void ClearEvent()
{//{{{
  Nb             = 0;
  Mumumass       = 0;
  Mumumasserr    = 0;
  Phimass        = 0;
  Kmpt           = 0;
  Kppt           = 0;
  Kmtrkdcasigbs  = 0;
  Kptrkdcasigbs  = 0;
        
  Bmass          = 0;
  Bpt            = 0;
  Beta           = 0;
  Bphi           = 0;
  Bvtxcl         = 0;
  Blxysig        = 0;
  Bcosalphabs    = 0;
  Bcosalphabs2d  = 0;
  Bctau          = 0;

  Q2             = 0;
  dimupt         = 0;
  dimueta        = 0;
  CosThetaL      = 999;
  CosThetaK      = 999;
  Phi            = 999;
  Triggers       = 0;

  //mc
  genBPt         = 0;
  genBEta        = 0;
  genBPhi        = 0;

  genMupPt       = 0;
  genMupEta      = 0;
  genMupPhi      = 0;
  genMumPt       = 0;
  genMumEta      = 0;
  genMumPhi      = 0;
    
  gendimuPt      = 0;
  gendimuEta     = 0;
  gendimuPhi     = 0;

  genQ2          = 0;
  genCosThetaL   = 999;
  genCosThetaK   = 999;
  genPhi         = 999;

}//}}}

void str_replace(std::string& str, const std::string& oldStr, const std::string& newStr)
{//{{{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
    {
      str.replace(pos, oldStr.length(), newStr);
      pos += newStr.length();
    }
}//}}}

string get_option_value(string option, string name)
{//{{{
  vector<string> args;
  istringstream f(option);
  string s;    
  while (getline(f, s, ';')) {
    args.push_back(s);
  }
  
  string value; 
  for(vector<string>::iterator it = args.begin(); it != args.end(); ++it) {
    value = *it; 
    unsigned found = value.find(name);
    if (found == 0) {
      str_replace(value, name+"=", ""); 
      break; 
    }
  }
  return value; 
}//}}}

void SingleBsToPhiMuMuSelector::Begin(TTree * /*tree*/)
{//{{{
  t_begin_.Set(); 
  printf("\n ---------- Begin Job ---------- \n");
  t_begin_.Print();

  n_processed_ = 0;
  n_selected_  = 0;

  n_triggers0 = 0;
  n_triggers1 = 0;

  n_total_        = 0; 
  n_passMuonID_   = 0; 
  n_passSelCut_   = 0; 
  n_passBestB_    = 0;

}//}}}



void SingleBsToPhiMuMuSelector::SlaveBegin(TTree * /*tree*/)
{

   string option = GetOption();
   tree_ = new TTree("tree", "tree"); 
   tree_->Branch("Mumumass"      , &Mumumass      , "Mumumass/D");
   tree_->Branch("Mumumasserr"   , &Mumumasserr   , "Mumumasserr/D");
   tree_->Branch("Phimass"       , &Phimass       , "Phimass/D");
   tree_->Branch("Kmpt"          , &Kmpt          , "Kmpt/D");
   tree_->Branch("Kppt"          , &Kppt          , "Kppt/D");
   tree_->Branch("Kmtrkdcasigbs" , &Kmtrkdcasigbs , "Kmtrkdcasigbs/D");
   tree_->Branch("Kptrkdcasigbs" , &Kptrkdcasigbs , "Kptrkdcasigbs/D");

   tree_->Branch("Bmass"         , &Bmass         , "Bmass/D");
   tree_->Branch("Bpt"           , &Bpt           , "Bpt/D");
   tree_->Branch("Beta"          , &Beta          , "Beta/D");
   tree_->Branch("Bphi"          , &Bphi          , "Bphi/D");

   tree_->Branch("Bvtxcl"        , &Bvtxcl        , "Bvtxcl/D");
   tree_->Branch("Blxysig"       , &Blxysig       , "Blxysig/D");
   tree_->Branch("Bcosalphabs"   , &Bcosalphabs   , "Bcosalphabs/D");
   tree_->Branch("Bcosalphabs2d" , &Bcosalphabs2d , "Bcosalphabs2d/D");
   tree_->Branch("Bctau"         , &Bctau         , "Bctau/D");

   tree_->Branch("Q2"            , &Q2            , "Q2/D");
   tree_->Branch("dimupt"        , &dimupt        , "dimupt/D");
   tree_->Branch("dimueta"       , &dimueta       , "dimueta/D");
   tree_->Branch("CosThetaL"     , &CosThetaL     , "CosThetaL/D");
   tree_->Branch("CosThetaK"     , &CosThetaK     , "CosThetaK/D");
   tree_->Branch("Phi"           , &Phi           , "Phi/D");
   tree_->Branch("Triggers"      , &Triggers      , "Triggers/I");

   string datatype = get_option_value(option, "datatype");
   std::map<string,int> maptype;
   maptype.insert(std::pair<string,int>("data",1));
   maptype.insert(std::pair<string,int>("mc.lite",2));
   maptype.insert(std::pair<string,int>("mc.hlt",998));
   maptype.insert(std::pair<string,int>("mc",999));
   switch (maptype[datatype]) {
   case 1:
     break;
   case 2:
     tree_->Branch("genBPt"       , &genBPt       , "genBPt/D");
     tree_->Branch("genBEta"      , &genBEta      , "genBEta/D");
     tree_->Branch("genBPhi"      , &genBPhi      , "genBPhi/D");
     tree_->Branch("genMupPt"     , &genMupPt     , "genMupPt/D");
     tree_->Branch("genMupEta"    , &genMupEta    , "genMupEta/D");
     tree_->Branch("genMupPhi"    , &genMupPhi    , "genMupPhi/D");
     tree_->Branch("genMumPt"     , &genMumPt     , "genMumPt/D");
     tree_->Branch("genMumEta"    , &genMumEta    , "genMumEta/D");
     tree_->Branch("genMumPhi"    , &genMumPhi    , "genMumPhi/D");

     tree_->Branch("gendimuPt"    , &gendimuPt    , "gendimuPt/D");
     tree_->Branch("gendimuEta"   , &gendimuEta   , "gendimuEta/D");
     tree_->Branch("gendimuPhi"   , &gendimuPhi   , "gendimuPhi/D");

     tree_->Branch("genQ2"        , &genQ2        , "genQ2/D");
     tree_->Branch("genCosThetaL" , &genCosThetaL , "genCosThetaL/D");
     tree_->Branch("genCosThetaK" , &genCosThetaK , "genCosThetaK/D");
     tree_->Branch("genPhi"       , &genPhi       , "genPhi/D");
     break;
   case 998:
     break;
   case 999:
     break;
   default:
     printf("No compatible datatype found. Please check use following types...\n\t\t[");
     for (std::map<string,int>::iterator iType = maptype.begin(); iType != maptype.end(); iType++){
       if (iType->second != 0) printf("%s,",iType->first.c_str());
     }
     printf("]\n");
     break;
   }

   fOutput->AddAll(gDirectory->GetList()); 

}

Bool_t SingleBsToPhiMuMuSelector::Process(Long64_t entry)
{

  ClearEvent();

  string option = GetOption();
  string datatype = get_option_value(option, "datatype"); 
  string cut = get_option_value(option, "cut"); 

  GetEntry(entry); 
  n_processed_ += 1; 
  Nb = nb; 

  if (triggernames->size() == 0) n_triggers0++;
  if (triggernames->size() == 1) n_triggers1++;

  if (datatype != "data") SaveGen();

  int i = SelectB(cut); 
  if (i != -1) n_passBestB_++;
  if ( i != -1 && (datatype == "data" || istruebs->at(i)) ) {
  //if ( i != -1 && (datatype == "data")) {
    printf("Entry#%lld, candidate#%d is selected.\n",entry,i);
    n_selected_ += 1; 
    SaveEvent(i);     
  }

  tree_->Fill();   
  return kTRUE;

}

void SingleBsToPhiMuMuSelector::SlaveTerminate()
{

}

void SingleBsToPhiMuMuSelector::Terminate()
{

  string option = GetOption();
  TString outfile = get_option_value(option, "ofile"); 
  //string outfile = get_option_value(option, "ofile"); 
  //printf("option=%s\n",option.c_str());
  //printf("outfile=%s",outfile.Data());
    
  TFile file(outfile.Data(), "recreate"); 
  //TFile file(outfile.c_str(), "recreate"); 
  fOutput->Write();

  t_now_.Set(); 
  printf(" \n ---------- End Job ---------- \n" ) ;
  t_now_.Print();  
  printf(" processed: %i \n selected: %i \n \
           duration: %i sec \n rate: %g evts/sec\n",
	 n_processed_, n_selected_, 
	 t_now_.Convert() - t_begin_.Convert(), 
	 float(n_processed_)/(t_now_.Convert()-t_begin_.Convert()) );

  printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
  printf("Triggers0 : %i\n", n_triggers0);
  printf("Triggers1 : %i\n", n_triggers1);
  printf("total events = %i\n", n_total_);
  printf("#evts passsing muonID = %i\n", n_passMuonID_);
  printf("muonID efficiency = %i/%i = %.2f % \n", n_passMuonID_, n_total_, 100*float(n_passMuonID_)/float(n_total_));
  printf("#evts passing selection cuts = %i\n", n_passSelCut_);
  printf("#cands/evt = %i/%i = %.2f %\n", n_passSelCut_, n_total_, 100*float(n_passSelCut_)/float(n_total_));
  printf("#evts passing best B sel = %i\n", n_passBestB_);
  printf("sel. efficiency = %i/%i = %.2f % \n", n_selected_, n_processed_, 100*float(n_selected_)/float(n_processed_));
  printf("trg. efficiency = %i/%i = %.2f % \n", n_triggers1, n_triggers0+n_triggers1, 100*float(n_triggers1)/float(n_triggers0+n_triggers1));
  printf("++++++++++++++++++++++++++++++++++\n");
}

int SingleBsToPhiMuMuSelector::SelectB(string cut)
{//{{{

  int best_idx = -1; 
  double best_bvtxcl = 0.0; 

  if (cut == "cut0") {
    for (int i = 0; i < nb; i++) {

      n_total_++;

      if ( ! HasGoodDimuon(i) ) continue; 
      n_passMuonID_++;
      n_passSelCut_++;

      if (bvtxcl->at(i) > best_bvtxcl) {
	best_bvtxcl = bvtxcl->at(i); 
	best_idx = i; 
      }
    }

  }else if (cut == "nocut") {
    for (int i = 0; i < nb; i++) {
      if (bvtxcl->at(i) > best_bvtxcl) {
	best_bvtxcl = bvtxcl->at(i); 
	best_idx = i; 
      }
    }
  }else if (cut == "genonly") {
    best_idx = -1;
  }else{
    printf("WARNING: Unknown cut, apply 'genonly' by default.\n");
    best_idx = -1;
  }

  return best_idx;
}//}}}

bool SingleBsToPhiMuMuSelector::HasGoodDimuon(int i)
{//{{{
 
  if ( // new soft muon id
      mumisgoodmuon->at(i)
      && mupisgoodmuon->at(i) 
      && mumntrklayers->at(i) > 5  // 2012 Data
      && mupntrklayers->at(i) > 5  // 2012 Data 
      // && mumntrkhits->at(i) > 10 
      // && mupntrkhits->at(i) > 10 
      && mumnpixlayers->at(i) > 0  // 1,0 (old,new)
      && mupnpixlayers->at(i) > 0  // 1,0 (old,new) 
      //&& mumnormchi2->at(i) < 1.8 
      //&& mupnormchi2->at(i) < 1.8 
      && mumtrkqual->at(i)==1
      && muptrkqual->at(i)==1
      && fabs(mumdxyvtx->at(i)) < 0.3  // 3,0.3 (old,new)
      && fabs(mupdxyvtx->at(i)) < 0.3  // 3,0.3 (old,new)
      && fabs(mumdzvtx->at(i)) < 20   // 30,20 (old,new) 
      && fabs(mupdzvtx->at(i)) < 20   // 30,20 (old,new)
       ) return true; 
  return false; 
}//}}}


void SingleBsToPhiMuMuSelector::SaveEvent(int i)
{//{{{

  TLorentzVector B_4vec, Phi_4vec, Mup_4vec, Mum_4vec, Km_4vec, Kp_4vec, buff1, buff2, buff3;
  B_4vec.SetXYZM(bpx->at(i),bpy->at(i),bpz->at(i),bmass->at(i));
  Phi_4vec.SetXYZM(kmpx->at(i)+kppx->at(i),kmpy->at(i)+kppy->at(i),kmpz->at(i)+kppz->at(i),phimass->at(i));
  Mup_4vec.SetXYZM(muppx->at(i),muppy->at(i),muppz->at(i),MUON_MASS);
  Mum_4vec.SetXYZM(mumpx->at(i),mumpy->at(i),mumpz->at(i),MUON_MASS);
  Km_4vec.SetXYZM(kmpx->at(i),kmpy->at(i),kmpz->at(i),KAON_MASS);
  Kp_4vec.SetXYZM(kppx->at(i),kppy->at(i),kppz->at(i),KAON_MASS);

  Bmass = bmass->at(i); 
  Bvtxcl = bvtxcl->at(i); 
  Blxysig = (blsbs->at(i)/blsbserr->at(i)); 
  Bcosalphabs = bcosalphabs->at(i); 
  Bcosalphabs2d = bcosalphabs2d->at(i);
  Bctau = bctau->at(i); 

  Kmpt = sqrt( (kmpx->at(i))*(kmpx->at(i)) + (kmpy->at(i))*(kmpy->at(i)) );
  Kppt = sqrt( (kppx->at(i))*(kppx->at(i)) + (kppy->at(i))*(kppy->at(i)) );

  Bpt  = B_4vec.Pt(); 
  Beta = B_4vec.Eta();
  Bphi = B_4vec.Phi();

  Mumumass    = mumumass->at(i); 
  Mumumasserr = mumumasserr->at(i); 
  Phimass     = phimass->at(i); 

  Kmtrkdcasigbs = fabs( kmtrkdcabs->at(i)/kmtrkdcabserr->at(i) ); 
  Kptrkdcasigbs = fabs( kptrkdcabs->at(i)/kptrkdcabserr->at(i) ); 
    
  Q2 = pow(mumumass->at(i),2);

  buff1 = B_4vec;
  buff2 = Mup_4vec+Mum_4vec;

  dimupt  = buff2.Pt();
  dimueta = buff2.Eta();

  buff1.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());

  // review this part again ??
  //  if ( Bchg > 0){
    buff3 = Mum_4vec; 
    //  }else{
    //buff3 = Mup_4vec;
    //  }
  buff3.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());
  CosThetaL = buff1.Vect().Dot(buff3.Vect())/buff1.Vect().Mag()/buff3.Vect().Mag();
    
  buff1 = B_4vec;
  buff2 = Phi_4vec;
  buff1.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());
  buff3 = Km_4vec; // double-check 
  buff3.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());
  CosThetaK = buff1.Vect().Dot(buff3.Vect())/buff1.Vect().Mag()/buff3.Vect().Mag();

  // add phi angle
  TVector3 boostB = B_4vec.BoostVector();
  Mum_4vec.Boost(-boostB);
  Mup_4vec.Boost(-boostB);
  Km_4vec.Boost(-boostB);
  Kp_4vec.Boost(-boostB);
  TVector3 MuMuPlane = Mum_4vec.Vect().Cross(Mup_4vec.Vect());   /// cross product between mu- and mu+ vectors                                       
  /////cout << "cross product mag. of muons (at RECO level) = " << MuMuPlane.Mag() << endl;   /// print statement for x-check
  TVector3 PhiPlane = Kp_4vec.Vect().Cross(Km_4vec.Vect());
  /////cout << "cross product mag. of kaons (at RECO level) = " << PhiPlane.Mag() << endl;    /// print statement for x-check
  if (MuMuPlane.Cross(PhiPlane).Dot(-B_4vec.Vect()) > 0.0)
    Phi = MuMuPlane.Angle(PhiPlane);
  else
    Phi = -MuMuPlane.Angle(PhiPlane);

  Triggers = triggernames->size();

}//}}}

void SingleBsToPhiMuMuSelector::SaveGen()
{//{{{

  TLorentzVector genB_4vec, genPhi_4vec, genMup_4vec, genMum_4vec, genKm_4vec, genKp_4vec, buff1, buff2, buff3;
  genB_4vec.SetXYZM(genbpx,genbpy,genbpz,5.3668);
  genPhi_4vec.SetXYZM(genphipx,genphipy,genphipz,PHI_MASS);
  genMup_4vec.SetXYZM(genmuppx,genmuppy,genmuppz,MUON_MASS);
  genMum_4vec.SetXYZM(genmumpx,genmumpy,genmumpz,MUON_MASS);
  genKm_4vec.SetXYZM(genkmpx,genkmpy,genkmpz,KAON_MASS);
  genKp_4vec.SetXYZM(genkppx,genkppy,genkppz,KAON_MASS);

  genBPt       = genB_4vec.Pt();
  genBEta      = genB_4vec.Eta();
  genBPhi      = genB_4vec.Phi();
  genMupPt     = genMup_4vec.Pt();
  genMupEta    = genMup_4vec.Eta();
  genMupPhi    = genMup_4vec.Phi();
  genMumPt     = genMum_4vec.Pt();
  genMumEta    = genMum_4vec.Eta();
  genMumPhi    = genMum_4vec.Phi();

  genQ2        = (genMup_4vec+genMum_4vec).Mag2();
    
  buff1        = genB_4vec;
  buff2        = genMup_4vec+genMum_4vec;

  gendimuPt    = buff2.Pt();
  gendimuEta   = buff2.Eta();
  gendimuPhi   = buff2.Phi();

  buff1.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());
  //  if (genBChg > 0){
    buff3 = genMum_4vec;//Take mu- to avoid extra minus sign.
    //}else{
    //buff3 = genMup_4vec;
    //}
  buff3.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());
  genCosThetaL = buff1.Vect().Dot(buff3.Vect())/buff1.Vect().Mag()/buff3.Vect().Mag();
    
  buff1 = genB_4vec;
  buff2 = genPhi_4vec;
  buff1.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());
  buff3 = genKm_4vec; // double check
  buff3.Boost(-buff2.X()/buff2.T(),-buff2.Y()/buff2.T(),-buff2.Z()/buff2.T());
  genCosThetaK = buff1.Vect().Dot(buff3.Vect())/buff1.Vect().Mag()/buff3.Vect().Mag();
 
  // add phi angle
  TVector3 boostB = genB_4vec.BoostVector();
  genMum_4vec.Boost(-boostB);
  genMup_4vec.Boost(-boostB);
  genKm_4vec.Boost(-boostB);
  genKp_4vec.Boost(-boostB);
  TVector3 MuMuPlane = genMum_4vec.Vect().Cross(genMup_4vec.Vect());   /// cross product between mu- and mu+ vectors
  /////cout << "cross product mag. of muons (at GEN level) = " << MuMuPlane.Mag() << endl;  ////  print statement for x-check
  TVector3 PhiPlane = genKp_4vec.Vect().Cross(genKm_4vec.Vect());
  /////cout << "cross product mag. of kaons (at GEN level) = " << PhiPlane.Mag() << endl;   ////  print statement for x-check
  if (MuMuPlane.Cross(PhiPlane).Dot(-genB_4vec.Vect()) > 0.0) 
    genPhi = MuMuPlane.Angle(PhiPlane);
  else                                                        
    genPhi = -MuMuPlane.Angle(PhiPlane);

}//}}}


#ifndef __CINT__ 
#include <algorithm>

char* get_option(char ** begin, char ** end, const std::string & option)
{//{{{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)  return *itr;
  return 0;
}//}}}

bool option_exists(char** begin, char** end, const std::string& option)
{//{{{
  return std::find(begin, end, option) != end;
}//}}}

void print_usage()
{//{{{
  cerr << "Usage: SingleBsToPhiMuMuSelector datatype cut infile outfile [-n] [-s] [-j] [-h]\n"
       << "  datatype: data, mc, mc.lite, mc.hlt\n"
       << "  cut     : cut0, nocut, genonly.\n"
       << "Options: \n" 
       << "  -h \t\tPrint this info.\n"
       << "  -n \t\tNumber of entries.\n" 
       << "  -s \t\tStarting run number.\n"
       << "  -j \t\tNumber of workers.\n" 
       << endl; 
}//}}}

int main(int argc, char** argv) {
  if ( (argc < 3) or option_exists(argv, argv+argc, "-h") ){
    print_usage() ;  
    return -1; 
  }

  TString datatype = argv[1]; 
  TString cut      = argv[2]; 
  TString infile   = argv[3]; 
  TString outfile  = argv[4]; 

  Printf("datatype: '%s'", datatype.Data());
  Printf("cut: '%s'", cut.Data());
  Printf("input file: '%s'", infile.Data());
  Printf("output file: '%s'", outfile.Data());

  TChain *ch = new TChain("ntuple/tree"); 
  ///TChain *ch = new TChain("tree"); 
  ch->Add(infile.Data()); 

  char *j = get_option(argv, argv+argc, "-j");
  if (j) {
    TProof::Open(Form("workers=%s", j));
    ch->SetProof(); 
  }

  Long64_t nentries = 1000000000; 
  char * n = get_option(argv, argv+argc, "-n");  
  if (n){
    nentries = atoi(n);
  }
    
  int     iStart = 0;
  char *s = get_option(argv, argv+argc, "-s");
  if (s) {
    iStart = atoi(s);
    if (iStart > ch->GetEntries()){
      printf("ERROR: Number of entries is %lld.\n",ch->GetEntries());
      return -1;
    }
  }

  TString option; 
  option.Form("datatype=%s;cut=%s;ofile=sel_%s_%s_%s_s%d.root", datatype.Data(), cut.Data(), outfile.Data(), datatype.Data(), cut.Data(), iStart); 
    
  // It's not allowed to run with fat trees!
  if (datatype.Data() == "mc" && (!(s) || !(n))){
    printf("WARNING: You must specify #entries(-n) and start run(-s) for datatype '%s'.\n",datatype.Data());
    return -1;
  }
    
  ch->Process("SingleBsToPhiMuMuSelector.cc+", option, nentries, iStart); 

  gSystem->Exit(0);

  return 0 ;
}

#endif
