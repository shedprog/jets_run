////Mykyta Shchedrolosiev
////Based on:
////Amilkar Quintero
////Temple University
////2018
////v3, 5 Nov 2018
//- Better way to apply the weight (No weight applied now)
//- New binning and ranges 
//- Include Electron phi histogram
//- Include decorrelation histograms
//

//Define a fill the histograms from the "orange" tree

#include <stdlib.h>
#include "Riostream.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TString.h"
#include "TDatime.h"
#include "TMath.h"
#include "TStyle.h"

#include <TTree.h>
#include <TH2D.h>
#include <TCut.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

TCut read_cuts(string) {

  string line;
  ifstream myfile ("cuts.txt");
  
  std::vector<TCut> all_cuts;
  
  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    { 
      // Ignore commented lines 
      if ((line[0]!='/' && line[1]!='/') && line[0] != '#')
      all_cuts.push_back(line.c_str());
    }
  myfile.close();
  }
  else cout<< "Unable to read cuts"; 
  
  TCut cut;
  for (unsigned i=0; i<all_cuts.size(); i++)
  cut = cut && all_cuts.at(i);
  
  return cut;
}

//--->START MAIN PROGRAM
//________________________________________________________________________________
int make_hists(//const Char_t *eachfile= "~/Desktop/zeusmc.hfix627.h1391.0607p.q4.ari_2911.root ", 
		 const Char_t *eachfile= "~/Desktop/data_07p_61843_61843_04.root ", 
		 const Char_t *outdir="./"){
                                         
  //gROOT->cd();
  TDatime now;                                          //Set time in Root
  now.Print();
  //  gErrorIgnoreLevel=kError;       //This gets rid of the warnings


  /////////////////////Read all files --1-->
  TFile f(eachfile); if (f.IsZombie())  {cout<< "Problems with file: " << eachfile << endl; return 0;}
  string eachfile2 = eachfile;
  size_t found = eachfile2.find("/data_");
  Bool_t isdata = found != std::string::npos ? true : false;
  Int_t runnumber = 0;
  Char_t runnumber_size[10];
  Char_t sample[50];
  //  Char_t *sampleMC="";
  if(isdata){
    cout << "Data" << endl;
    sscanf(&eachfile2[found],"/data_%[^_]_%d_%[^.].root",sample,&runnumber,runnumber_size);
    cout<< "Sample: " << sample << endl;
    cout << "Run Number: " << runnumber << endl;
    //cout<< size << endl;
  } else{
    cout << "MC" << endl;
    found = eachfile2.find("/zeusmc.");
    if (found == std::string::npos) {cout << "Problem finding the MC file" << endl; return 0;}
    sscanf(&eachfile2[found],"/zeusmc.%s",sample);
    cout << "Sample MC: " << sample << endl;
  }

  TTree *firstJet = (TTree*)f.Get("orange");
  if (!firstJet) {cout<< "Problems with file: " << eachfile << endl; return 0;}

  TString period;
  if(runnumber>=47010 && runnumber <=51245)  period = "04p";
  if(runnumber>=52258 && runnumber <=57123)  period = "0405e";
  if(runnumber>=58207 && runnumber <=59947)  period = "06e";
  if(runnumber>=60005 && runnumber <= 62639) period = "0607p";
  if(!isdata) period=sample;
  /////END Reading all files <--1--


  /////////////////////Define Histograms --2-->  
  Int_t q2start = 10;
  Int_t q2end   = 350;
  //Event quantities
  TH1D* hVertexZ   = new TH1D("hVertexZ", "Vertex Z"            , 80,-40,40);
  TH2D *h2Q2x      = new TH2D("h2Q2x"   , "Q^{2} vs x"          , 200, 2.e-5, 2.e-1, q2end-q2start, q2start, q2end);
  TH1D* hQ2        = new TH1D("hQ2"     , "Q^{2}"               , q2end-q2start, q2start, q2end);
  TH1D* hx         = new TH1D("hx"      , "Momentum fraction x" , 200, 2.e-5, 2.e-1);
  TH1D* hJetMult   = new TH1D("hJetMult", "Jets Multiplicity"   , 11,0,11);
  TH1D* hEmpz      = new TH1D("hEmpz"   , "E_{M} - p_{Z}"       , 100,20,70);
  TH1D* hGamma     = new TH1D("hGamma"  , "cos(#gamma_{h})"     , 120,-1,0.4);
  TH1D* hPtEt      = new TH1D("hPtEt"   , "P_{T} / #sqrt(E_{T})", 100,0,5);
  //Electron quantities
  TH1D* hElecTheta = new TH1D("hElecTheta", "Angle #theta of the DIS electron", 120, 130., 190.);
  TH1D* hElecPhi   = new TH1D("hElecPhi"  , "Angle #phi of the DIS electron"  ,  60,-TMath::Pi(),TMath::Pi());
  TH1D* hElecE     = new TH1D("hElecE"   , "Energy of the DIS electron"       , 100, 10., 60.);
  TH1D* hElecProb  = new TH1D("hElecProb", "Sinistra probability"             , 200, 0.89, 1.);
  TH1D* hElecy     = new TH1D("hElecy"   , "Lepton inelasticity"              , 250, -0.5, 1.);
  TH2D* h2ElecPos  = new TH2D("h2ElecPos", "Lepton Position"                  , 800,-200,200,800,-200,200);
  //Jets
  TH1D* hJetEt   = new TH1D("hJetEt"  ,"Jets Transverse Energy"  ,110,0,55);
  TH1D* hJetMass = new TH1D("hJetMass","Jets Mass"               ,110,0,55);
  TH1D* hJetPt   = new TH1D("hJetPt"  ,"Jets Transverse Momentum",110,0,55);
  TH1D* hJetEta  = new TH1D("hJetEta" ,"Jets Eta"                ,100,-2.5,2.5);
  TH1D* hJetPhi  = new TH1D("hJetPhi" ,"Jets Phi"                , 60,0,2*TMath::Pi());
  //Decorrelation
  TH1D* hDecorrPhi    = new TH1D("hDecorrPhi","Jet - electron decorrelation angle", 60,TMath::Pi()/2,TMath::Pi());
  TH2D* h2PtDecorrPhi = new TH2D("h2PtDecorrPhi" ,"P_{T} of the Jet vs decorrelation angle",60,TMath::Pi()/2,TMath::Pi(),110,0,55);
  TH2D* h2JetElecPhi  = new TH2D("h2JetElecPhi" ,"Jet vs electron #phi angle", 60,-TMath::Pi(),TMath::Pi(), 60,0,2*TMath::Pi());
  ////END Define Histograms <--2--


  // /////////////////////Define Cuts --3-->
  // //Using only the electron with the highest probability Siq2el[0]
  // //Using only the highest jet in the event: Kt_etajet_a[0]
  // /////Phase Space
  // TCut cQ2 = "Siq2el[0]>10 && Siq2el[0]<350";         //[0] is the electron with the high probability of being DIS
  // //TCut cGm = "TMath::Abs(TMath::Cos(Cc_gamma)) < 0.65";
  // /////Cleaning Cuts
  // TCut cVertexZ = "Zvtx>-40 && Zvtx<40 && Zvtx!=0";
  // //TCut cEmpz    = "Cc_empz>38 && Cc_empz<65"; 
  // TCut cEmpz    = "Cal_empz>35 && Cal_empz<65";
  // /////Electron Cuts
  // TCut cSiecorr = "Siecorr[0][2] > 10";
  // TCut cSith    = "Sith[0]*180.0/TMath::Pi() > 140 && Sith[0]*180.0/TMath::Pi() < 180.0";
  // //TCut cEx      = "TMath::Abs(Sipos[0][0]) > 14.8 && Sipos[0][1] < 12.5";
  // //TCut cEy      = "Sipos[0][1] > 12.5";
  // //TCut cEp      = "!(TMath::Abs(Sipos[0][0]) < 14.8 && Sipos[0][1] > -14.6 && Sipos[0][1] < 12.5)";
  // TCut cEp      = "TMath::Sqrt(Sipos[0][0]*Sipos[0][0] + Sipos[0][1]*Sipos[0][1]) > 20.0";
  // TCut cEye     = "Siyel[0] > 0.05 && Siyel[0]<0.4";
  // TCut cPtEt    = "Cal_pt/sqrt(Cal_et) < 2.0";
  // TCut cElec    = cSiecorr && cSith && cEp && cEye;// && cPtEt; // && cEy;
  
  /////Triggers
  TCut cTltw;
  if(period == "0405e") cTltw = "(Tltw[2] & (1 << 1))";      //SPP02
  if(period == "06e"  ) cTltw = "(Tltw[2] & (1 << 8))";      //SPP09
  if(period == "0607p") cTltw = "(Tltw[2] & (1 << 8))";      //SPP09
  
  // /////Jet Selection
  // //TCut cJet     = "Kt_etajet_b[0]>-1.5 && Kt_etajet_b[0]<1.8 && Kt_etjet_b[0]>2.5";
  // TCut cJet     = "TMath::Abs(Kt_etajet_b[0]) < 1.0";
  // //TCut cJet     = "Kt_etajet_b[0]>-0.7 && Kt_etajet_b[0]<1.1 && Kt_etjet_b[0]>2.5";
  // /////TakeInfo
  // //TCut cEvt     = "Evtake_iwant > 0 && Mvdtake!=0 && Stttake!=0";
  // //
  // TCut cAll     = cQ2 && cVertexZ && cEmpz && cElec && cTltw && cJet;
  // /////END define Cuts <--3--
  TCut cAll = cTltw && read_cuts("./cuts.data");

  ///////Start filling the histograms from the trees  --4-->
  cout << "Number of events before the cuts: " << firstJet->GetEntries(    ) << endl;
  cout << "Number of events after the cuts : " << firstJet->GetEntries(cAll) << endl;
  if(firstJet->GetEntries(cAll) == 0 && isdata)       {cout << "No jets in run: " << runnumber_size << endl; return 0;}
  else if(firstJet->GetEntries(cAll) == 0 && !isdata) {cout << "No jets in run: " << sample         << endl; return 0;}
  
  //------------Event
  firstJet->Draw("Zvtx                      >> hVertexZ"  , cAll, "goff"); 
  firstJet->Draw("Siq2el[0]:Sixel[0]        >> h2Q2x"     , cAll, "goff"); 
  firstJet->Draw("Siq2el[0]                 >> hQ2"       , cAll, "goff"); 
  firstJet->Draw("Sixel[0]                  >> hx"        , cAll, "goff");
  firstJet->Draw("Kt_njet_b                 >> hJetMult"  , cAll, "goff");
  firstJet->Draw("Cal_empz                  >> hEmpz"     , cAll, "goff");
  firstJet->Draw("cos(Cc_gamma)             >> hGamma"    , cAll, "goff");
  firstJet->Draw("Cal_pt/sqrt(Cal_et)       >> hPtEt"     , cAll, "goff");

  //------------Electron 
  firstJet->Draw("Sith[0]*180.0/TMath::Pi() >> hElecTheta", cAll, "goff"); 
  firstJet->Draw("Siph[0]                   >> hElecPhi"  , cAll, "goff"); 
  firstJet->Draw("Siecorr[0][2]             >> hElecE"    , cAll, "goff"); 
  firstJet->Draw("Siprob[0]                 >> hElecProb" , cAll, "goff"); 
  firstJet->Draw("Sipos[0][1]:Sipos[0][0]   >> h2ElecPos" , cAll, "goff"); 
  firstJet->Draw("Siyel[0]                  >> hElecy"    , cAll, "goff"); 

  //------------Jet
  firstJet->Draw("Kt_etjet_b[0]  >> hJetEt"  , cAll, "goff");
  firstJet->Draw("Kt_masjet_b[0] >> hJetMass", cAll, "goff");
  firstJet->Draw("sqrt(Kt_etjet_b[0]*Kt_etjet_b[0]-Kt_masjet_b[0]*Kt_masjet_b[0]) >> hJetPt", cAll, "goff");
  firstJet->Draw("Kt_etajet_b[0] >> hJetEta" , cAll, "goff");
  firstJet->Draw("Kt_phijet_b[0] >> hJetPhi" , cAll, "goff");

  //------------Decorrelation
  //firstJet->Draw("abs( Kt_phijet_b[0] - (Siph[0] > 0 ? Siph[0] : 2*TMath::Pi() + Siph[0]) ) >> hDecorrPhi"  , cAll, "goff"); 
  firstJet->Draw("abs( Kt_phijet_b[0] - Siph[0])                     >>  hDecorrPhi", 
		 "Siph[0] > 0 && abs(Kt_phijet_b[0] - Siph[0])<TMath::Pi()" && cAll, "goff"); 
  firstJet->Draw("abs( Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]) )  >>+ hDecorrPhi", 
		 "Siph[0] < 0 && abs(Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]))<TMath::Pi()" && cAll, "goff"); 
  firstJet->Draw("2*TMath::Pi() - abs( Kt_phijet_b[0] - Siph[0]) >>+ hDecorrPhi", 
		 "Siph[0] > 0 && abs(Kt_phijet_b[0] - Siph[0])>TMath::Pi()" && cAll, "goff"); 
  firstJet->Draw("2*TMath::Pi() - abs( Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]) )  >>+ hDecorrPhi", 
		 "Siph[0] < 0 && abs(Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]))>TMath::Pi()" && cAll, "goff"); 

  firstJet->Draw("sqrt(Kt_etjet_b[0]*Kt_etjet_b[0]-Kt_masjet_b[0]*Kt_masjet_b[0]):abs( Kt_phijet_b[0] - Siph[0]) >>  h2PtDecorrPhi", 
		 "Siph[0] > 0 && abs(Kt_phijet_b[0] - Siph[0])<TMath::Pi()" && cAll, "goff"); 
  firstJet->Draw("sqrt(Kt_etjet_b[0]*Kt_etjet_b[0]-Kt_masjet_b[0]*Kt_masjet_b[0]):abs( Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]) )  >>+ h2PtDecorrPhi", 
		 "Siph[0] < 0 && abs(Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]))<TMath::Pi()" && cAll, "goff"); 
  firstJet->Draw("sqrt(Kt_etjet_b[0]*Kt_etjet_b[0]-Kt_masjet_b[0]*Kt_masjet_b[0]):2*TMath::Pi() - abs( Kt_phijet_b[0] - Siph[0]) >>+ h2PtDecorrPhi", 
		 "Siph[0] > 0 && abs(Kt_phijet_b[0] - Siph[0])>TMath::Pi()" && cAll, "goff"); 
  firstJet->Draw("sqrt(Kt_etjet_b[0]*Kt_etjet_b[0]-Kt_masjet_b[0]*Kt_masjet_b[0]):2*TMath::Pi() - abs( Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]) )  >>+ h2PtDecorrPhi",
		 "Siph[0] < 0 && abs(Kt_phijet_b[0] - (2*TMath::Pi() + Siph[0]))>TMath::Pi()" && cAll, "goff");   
    ///END Start filling the histograms from the trees  <--4--
  

  /////////////////////Save histograms in a file --5-->    
  TString outdir1(outdir);
  TString outfile(outdir1+"/Hist_"+period+"_"+runnumber_size+".root");
  TString outfileMC(outdir1+"/Hist_"+sample);
  TFile fout(isdata ? outfile : outfileMC,"RECREATE");
  
  //Event
  hVertexZ->Write(); h2Q2x->Write(); hQ2->Write(); hx->Write(); 
  hJetMult->Write(); hEmpz->Write(); hGamma->Write(); hPtEt->Write();
  
  //Electron
  hElecTheta->Write(); hElecPhi->Write(); hElecE->Write(); 
  hElecProb->Write(); hElecy->Write(); h2ElecPos->Write();
  
  //Jet
  hJetEt->Write(); hJetPt->Write(); hJetMass->Write(); hJetEta->Write(); hJetPhi->Write();

  //Decorrelation
  hDecorrPhi->Write();   h2PtDecorrPhi->Write();
  fout.Close();
  /////END Save histograms in a file <--5-- 

  TDatime now1;
  now1.Print();

  return 0;
}


 //"Siq2el[0]>10 && Siq2el[0]<350 && Zvtx>-40 && Zvtx<40 && Zvtx!=0 && Cal_empz>35 && Cal_empz<65 && Siecorr[0][2] > 10 && Sith[0]*180.0/TMath::Pi() > 140 && Sith[0]*180.0/TMath::Pi() < 180.0 && TMath::Abs(Sipos[0][0]) > 14.8 && (Sipos[0][1] < -14.6 || Sipos[0][1] > 12.5) && (Tltw[2] & (1 << 8)) && Kt_etajet_b[0]>-1.5 && Kt_etajet_b[0]<1.8 && Kt_etjet_b[0]>2.5

 //"Siq2el[0]>10 && Siq2el[0]<350 && Zvtx>-40 && Zvtx<40 && Zvtx!=0 && Cal_empz>35 && Cal_empz<65 && Siecorr[0][2] > 10 && Sith[0]*180.0/TMath::Pi() > 140 && Sith[0]*180.0/TMath::Pi() < 180.0 && TMath::Sqrt(Sipos[0][0]*Sipos[0][0] + Sipos[0][1]*Sipos[0][1]) > 20.0 && (Tltw[2] & (1 << 8)) && Kt_etajet_b[0]>-1.5 && Kt_etajet_b[0]<1.8 && Kt_etjet_b[0]>2.5
