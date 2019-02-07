`/////Amilkar Quintero
////Temple University
////October 2018
//
//v2, 2 Nov 2018
//Fill the histograms per event to apply the weight
//
//v3, 5 Nov 2018
//- Better way to apply the weight (No weight applied now)
//- New binning and ranges 
//- Include Electron phi histogram
//- Include decorrelation histograms
//
//v4, 10 Nov 2018
//- Loop over each event
//- Modified the orange.h (done with MakeClass()) to accomodate data and MC
//
//v5, 26 Nov 2018
//- Change names of orange to JetDecorr2018
//- Use the cuts suggested in the email of 13 November 2018
//
//v5_1, 11 Dec 2018
//- Changes due comments after the meeting today 
//- Apply "chimney" cut, -1.5 < eta < 1.8, yJB>0.04 and Pt/sqrt(Et)<2.5
//- Remove Q2 high limit.Sinistra might only go to 10000
//- Calculate E-pz from Zufo.
//

#define JetOrange2018_cxx
#include <stdlib.h>
#include <string>
#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TString.h"
#include "TDatime.h"
#include "TMath.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "JetOrange2018.h"

using namespace std;



Float_t CalcWeight(TString sample){
  Float_t lumi=1.0;
  Float_t total=1.0;
  //if(sample.Contains("0405e"))  { lumi=273.4; total=571.2;}
  //if(sample.Contains("06e"))    { lumi=108.5; total=571.2;}
  //if(sample.Contains("ele2005")){ lumi=134.5; total=571.2;}
  // if(sample.Contains("ele2006")){ lumi=54.5 ; total=571.2;}
  
  Float_t weight = lumi /total;
  return 1.56; //  return weight;
}

//--->START MAIN PROGRAM
//________________________________________________________________________________
void MakeHist_v5_1(const Char_t *eachfile= "~/Desktop/zeusmc.hfix627.h1391.0607p.q4.ari_2911.root ", 
		   //const Char_t *eachfile= "~/Desktop/data_07p_61843_61843_04.root ", 
		   const Char_t *outdir="./"){

  //gROOT->cd();
  TDatime now;                                          //Set time in Root
  now.Print();
  //gErrorIgnoreLevel=kError;       //This gets rid of the warnings
  gErrorIgnoreLevel=kFatal;       //This gets rid of the errors
  //There are some unused branched for data or MC

  /////////////////////Read all files --1-->
  TFile f(eachfile); if (f.IsZombie())  {cout<< "Problems with file: " << eachfile << endl; return;}
  string eachfile2 = eachfile;
  size_t found = eachfile2.find("/data_");
  Bool_t isdata = found != std::string::npos ? true : false;
  Int_t runnumber = 0;
  Char_t runnumber_size[10];
  Char_t sample[50];
  Float_t Weight = 1.0;
  if(isdata){
    cout << "Data" << endl;
    sscanf(&eachfile2[found],"/data_%[^_]_%d_%[^.].root",sample,&runnumber,runnumber_size);
    cout<< "Sample: " << sample << endl;
    cout << "Run Number: " << runnumber << endl;
    //cout<< size << endl;
  } else{
    cout << "MC" << endl;
    found = eachfile2.find("/zeusmc.");
    if (found == std::string::npos) {cout << "Problem finding the MC file" << endl; return;}
    sscanf(&eachfile2[found],"/zeusmc.%s",sample);
    cout << "Sample MC: " << sample << endl;
    Weight = CalcWeight(sample);
    cout << "Weight   : " << Weight << endl; 
  }

  TTree *firstJet = (TTree*)f.Get("orange");
  if (!firstJet) {cout<< "Problems with file: " << eachfile << endl; return;}
  JetOrange2018 *JetOrange = new JetOrange2018();
  JetOrange->Init(firstJet);
  JetOrange->SetData(isdata);
 
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
  TH1D* hJetMult   = new TH1D("hJetMult", "Jets Multiplicity "  , 11,0,11);
  TH1D* hEmpz      = new TH1D("hEmpz"   , "E_{M} - p_{Z}"       , 100,20,70);
  TH1D* hGamma     = new TH1D("hGamma"  , "cos(#gamma_{h})"     , 120,-1,-0.6);
  TH1D* hPtEt      = new TH1D("hPtEt"   , "P_{T} / #sqrt(E_{T}): from Zufo", 90,0,3);
  TH1D* hDiffEmpz  = new TH1D("hDiffEmpz", "E - p_{Z}: Zufo - Cal", 200,-20,20);
  //Electron quantities
  TH1D* hElecTheta = new TH1D("hElecTheta", "Angle #theta of the DIS electron", 120, 130., 190.);
  TH1D* hElecPhi   = new TH1D("hElecPhi"  , "Angle #phi of the DIS electron"  , 60,-TMath::Pi(),TMath::Pi());
  TH1D* hElecE     = new TH1D("hElecE"    , "Energy of the DIS electron"      , 100, 10., 60.);
  TH1D* hElecProb  = new TH1D("hElecProb" , "Sinistra probability"            , 200, 0.89, 1.);
  TH1D* hElecy     = new TH1D("hElecy"    , "Inelasticity JB method"          , 220, -0.1, 1.);
  TH2D* h2ElecPos  = new TH2D("h2ElecPos" , "Sinistra probability"            , 800,-200,200,800,-200,200);
  //Jets
  TH1D* hJetEt   = new TH1D("hJetEt"  ,"Jets Transverse Energy"  , 110,0,55);
  TH1D* hJetMass = new TH1D("hJetMass","Jets Mass"               , 110,0,55);
  TH1D* hJetPt   = new TH1D("hJetPt"  ,"Jets Transverse Momentum", 110,0,55);
  TH1D* hJetEta  = new TH1D("hJetEta" ,"Jets Eta"                , 100,-2.5,2.5);
  TH1D* hJetPhi  = new TH1D("hJetPhi" ,"Jets Phi"                , 60,0,2*TMath::Pi());
  //Decorrelation
  TH1D* hDecorrPhi[5];   TH2D* h2PtDecorrPhi[5];
  for(Int_t ijet =0; ijet < 5; ijet++){ 
    hDecorrPhi[ijet]    = new TH1D(Form("hDecorrPhi_%d"   ,ijet),
				   Form("Jet - electron decorrelation angle, %d jets",ijet+1), 60,TMath::Pi()/2,TMath::Pi());
    h2PtDecorrPhi[ijet] = new TH2D(Form("h2PtDecorrPhi_%d",ijet),
				   Form("P_{T} of the Jet vs angle, %d jets",ijet+1), 60,TMath::Pi()/2,TMath::Pi(),110,0,55);
  }
  TH2D* h2JetElecPhi  = new TH2D("h2JetElecPhi" ,"Jet vs electron #phi angle", 60,-TMath::Pi(),TMath::Pi(), 60,0,2*TMath::Pi());
  ////END Define Histograms <--2--
  
  
  ///////Start filling the histograms from the trees  --3-->
  Int_t count = 0;
  Long64_t nentries = firstJet->GetEntriesFast();
  cout << "Number of events before the cuts: " << nentries << endl;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
    JetOrange->GetEntry(jentry);
    
    /////////////////////Define Cuts --3.1-->
    /////Using only the electron with the highest probability Siq2el[0]
    /////Using only the highest jet in the event: Kt_etajet_b[0]
    /////Phase Space 
    if(JetOrange->Siq2el[0]<q2start || JetOrange->Siq2el[0]>q2end) continue;
    //if(JetOrange->Siq2el[0]<10) continue;
    /////Cleanning cuts
    if(JetOrange->Zvtx<-40 || JetOrange->Zvtx>40 || JetOrange->Zvtx==0)    continue;
    //if(JetOrange->Cal_empz<35 || JetOrange->Cal_empz>65) continue;  
    //Calculate E-pz from zufo
    Float_t Empz = 0.;
    for(Int_t zloop=0; zloop<JetOrange->Nzufos; zloop++){
      TLorentzVector v(JetOrange->Zufo[zloop][0], JetOrange->Zufo[zloop][1], 
		       JetOrange->Zufo[zloop][2], JetOrange->Zufo[zloop][3]);
      Empz += JetOrange->Zufo[zloop][3] - JetOrange->Zufo[zloop][2];
    }
    //cout << Form("Calorimeter: %f     Zufo: %f     Diff: %f",JetOrange->Cal_empz,Empz,JetOrange->Cal_empz - Empz) << endl;
    if(Empz < 35. || Empz > 65.) continue;
    // if(JetOrange->Siyel[0] > 0.95) continue;
    // if(JetOrange->Siyjb[0] < 0.04) continue;
    // if(JetOrange->Cal_pt / TMath::Sqrt(JetOrange->Cal_et) > 2.5) continue;
    /////Electron cuts
    if(JetOrange->Siecorr[0][2] < 10) continue;
    if(JetOrange->Sith[0]*180.0/TMath::Pi() < 140 || JetOrange->Sith[0]*180.0/TMath::Pi() > 180.0) continue;
    // if(JetOrange->Sipos[0][2] < -148. && JetOrange->Sipos[0][0] > -14. 
       // && JetOrange->Sipos[0][0] < 12. && JetOrange->Sipos[0][1] > 90.) continue;                //Chimney cut
    if(sqrt(JetOrange->Sipos[0][0]*JetOrange->Sipos[0][0] + JetOrange->Sipos[0][1]*JetOrange->Sipos[0][1]) < 20.0) continue;
    /////Triggers
    if(period == "0405e" && !(JetOrange->Tltw[2] & (1 << 1)) ) continue;      //SPP02
    if(period == "06e"   && !(JetOrange->Tltw[2] & (1 << 8)) ) continue;      //SPP09
    if(period == "0607p" && !(JetOrange->Tltw[2] & (1 << 8)) ) continue;      //SPP09
    /////Jet selection
    if(JetOrange->Kt_njet_b <= 0) continue;
    // if(JetOrange->Kt_etajet_b[0]<-1.5 || JetOrange->Kt_etajet_b[0]>1.8) continue;
    if(JetOrange->Kt_etjet_b[0] < 2.5) continue;
    
    /////////////////////END Define Cuts <--3.1--
    

    /////////////////////Fill histograms --3.2-->
    //------------Event
    hVertexZ->Fill(JetOrange->Zvtx                          ,Weight); 
    h2Q2x   ->Fill(JetOrange->Sixel[0],JetOrange->Siq2el[0] ,Weight);
    hQ2     ->Fill(JetOrange->Siq2el[0]                     ,Weight);
    hx      ->Fill(JetOrange->Sixel[0]                      ,Weight);
    hJetMult->Fill(JetOrange->Kt_njet_b                     ,Weight);
    hEmpz   ->Fill(Empz                                     ,Weight);
    //hEmpz   ->Fill(JetOrange->Cal_empz                      ,Weight);
    hGamma  ->Fill(TMath::Cos(JetOrange->Cc_gamma)          ,Weight);
    hPtEt   ->Fill(JetOrange->Cal_pt/sqrt(JetOrange->Cal_et),Weight);
    hDiffEmpz->Fill(Empz - JetOrange->Cal_empz              ,Weight);
    //------------Electron 
    hElecTheta->Fill(JetOrange->Sith[0]*180.0/TMath::Pi()         ,Weight);
    hElecPhi  ->Fill(JetOrange->Siph[0]                           ,Weight);
    hElecE    ->Fill(JetOrange->Siecorr[0][2]                     ,Weight);
    hElecProb ->Fill(JetOrange->Siprob[0]                         ,Weight);
    hElecy    ->Fill(JetOrange->Siyjb[0]                          ,Weight);
    h2ElecPos ->Fill(JetOrange->Sipos[0][0],JetOrange->Sipos[0][1],Weight);
    //------------Jet
    hJetEt  ->Fill(JetOrange->Kt_etjet_b[0] ,Weight);
    hJetMass->Fill(JetOrange->Kt_masjet_b[0],Weight);
    Float_t JetPt = sqrt(JetOrange->Kt_etjet_b[0]*JetOrange->Kt_etjet_b[0]-JetOrange->Kt_masjet_b[0]*JetOrange->Kt_masjet_b[0]) ;
    hJetPt  ->Fill(JetPt                    ,Weight);
    hJetEta ->Fill(JetOrange->Kt_etajet_b[0],Weight);
    hJetPhi ->Fill(JetOrange->Kt_phijet_b[0],Weight);

    //------------Correlation
    Float_t ElectronPhi = JetOrange->Siph[0] > 0. ? JetOrange->Siph[0] : 2*TMath::Pi() + JetOrange->Siph[0];  
    Float_t DecorrPhi   = TMath::Abs( JetOrange->Kt_phijet_b[0] - ElectronPhi) ;
    if(DecorrPhi > TMath::Pi()) DecorrPhi = 2*TMath::Pi() - DecorrPhi ;

    for(Int_t ijet =0; ijet < 5; ijet++){ 
      if(JetOrange->Kt_njet_b > ijet) {
	hDecorrPhi[ijet]   ->Fill(DecorrPhi       ,Weight);
	h2PtDecorrPhi[ijet]->Fill(DecorrPhi, JetPt,Weight);
      }
    }
    /////////////////////END Fill histograms <--3.2--
    count++;
  }
  cout << "Number of events after the cuts : " << count << endl;
  ///END Start filling the histograms from the trees  <--3--


  /////////////////////Save histograms in a file --5-->    
  TString outdir1(outdir);
  TString outfile(outdir1+"/Hist_"+period+"_"+runnumber_size+".root");
  TString outfileMC(outdir1+"/Hist_"+sample);
  TFile fout(isdata ? outfile : outfileMC,"RECREATE");
  //Event
  hVertexZ->Write(); h2Q2x->Write(); hQ2->Write(); hx->Write(); hJetMult->Write(); 
  hEmpz->Write(); hGamma->Write();  hPtEt->Write(); hDiffEmpz->Write();
  //Electron
  hElecTheta->Write(); hElecPhi->Write(); hElecE->Write(); hElecProb->Write(); hElecy->Write(); h2ElecPos->Write();
  //Jet
  hJetEt->Write(); hJetPt->Write(); hJetMass->Write(); hJetEta->Write(); hJetPhi->Write();
  //Decorrelation
  for(Int_t ijet =0; ijet < 5; ijet++){ hDecorrPhi[ijet]->Write();   h2PtDecorrPhi[ijet]->Write();}
  fout.Close();
  /////END Save histograms in a file <--5-- 


  TDatime now1;
  now1.Print();
}


/*
  lumi_data[0] = 134.15997;   //0405e
  lumi_data[1] = 54.79574;    //06e
  lumi_data[2] = 142.93778;   //0607p

 } else if (cyear.find("0607p") != std::string::npos) {
                        b0607p = true;
                        datalum = 135.33;
                        pol = 0.0327561;

                        q2lumi[0] = 4.334;
                        q2lumi[1] = 7.635;
                        q2lumi[2] = 986.3;
                        q2lumi[3] = 1712;
                        q2lumi[4] = 5047;
                        q2lumi[5] = 8445;
                        q2lumi[6] = 33470;
                        q2lumi[7] = 91990;
                        q2lumi[8] = 315000;
                        q2lumi[9] = 1061000;
                        q2lumi[10] = 1740000;
                        q2lumi[11] = 8747000;



 } else if (cyear.find("05e") != std::string::npos) {
                        b05e = true;
                        datalum = 133.424;
                        pol = -0.0740036;

                        q2lumi[0] = 0;
                        q2lumi[1] = 0;
                        q2lumi[2] = 981.9;
                        q2lumi[3] = 1674;
                        q2lumi[4] = 4600;
                        q2lumi[5] = 6950;
                        q2lumi[6] = 23020;
                        q2lumi[7] = 48290;
                        q2lumi[8] = 117400;
                        q2lumi[9] = 321700;
                        q2lumi[10] = 462700;
                        q2lumi[11] = 2141000;


     } else if (cyear.find("06e") != std::string::npos) {
                        b06e = true;
                        datalum = 52.1276;
                        pol = 0.0953296;

                        q2lumi[0] = 0;
                        q2lumi[1] = 0;
                        q2lumi[2] = 284.8;
                        q2lumi[3] = 485.5;
                        q2lumi[4] = 1380;
                        q2lumi[5] = 2225;
                        q2lumi[6] = 7367;
                        q2lumi[7] = 14860;
                        q2lumi[8] = 46980;
                        q2lumi[9] = 107200;
                        q2lumi[10] = 463000;
                        q2lumi[11] = 2141000;
 */
