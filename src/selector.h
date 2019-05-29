
//selector class inherent JetOrange2018_cxx

#define JetOrange2018_cxx
#include <stdlib.h>
#include <string>
#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TString.h"
#include "TDatime.h"
#include "TMath.h"
#include "TROOT.h"
#include "TLorentzVector.h"
#include "cuts.h"
#include "LepE_calibr.h"

//using namespace std;

#define q2start 10
#define q2end 350

#ifndef Selector_h
#define Selector_h

//Try to re-weight by lepton energy
//Float_t CalcWeight(Float_t val)
//{
//	return 1.20214 - 9.38496e-02*val + 4.07696e-03*val*val - 5.45586*val*val*val;
//}

namespace ControlPlot {
class Selector : public JetOrangeSelect {
private:  

//  Int_t q2start = 10;
//  Int_t q2end   = 350;
  
  //Control plots histograms

  //Event quantities
  TH1D* hVertexZ;  
  TH2D* h2Q2x;     
  TH1D* hQ2;       
  TH1D* hx;        
  TH1D* hJetMult;  
  TH1D* hEmpz;     
  TH1D* hGamma;    
  TH1D* hPtEt;     
  TH1D* hDiffEmpz; 
  //Electron quant
  TH1D* hElecTheta;
  TH1D* hElecPhi;  
  TH1D* hElecE;    
  TH1D* hElecProb; 
  TH1D* hElecy;    
  TH2D* h2ElecPos; 
  //Jets
  TH1D* hJetEt;
  TH1D* hJetMass;
  TH1D* hJetPt; 
  TH1D* hJetEta;
  TH1D* hJetPhi;

  TH1D* hDecorrPhi[5];
  TH2D* h2PtDecorrPhi[5];

public:

  Calibrator* BinCalibr;

  Selector(TTree *tree) : JetOrangeSelect(tree) {};

  void InitHists(){
	 
	  //Event quantities
	  hVertexZ   = new TH1D("hVertexZ", "Vertex Z"            , 80,-40,40);
	  h2Q2x      = new TH2D("h2Q2x"   , "Q^{2} vs x"          , 200, 2.e-5, 2.e-1, q2end-q2start, q2start, q2end);
	  hQ2        = new TH1D("hQ2"     , "Q^{2}"               , q2end-q2start, q2start, q2end);
	  hx         = new TH1D("hx"      , "Momentum fraction x" , 200, 2.e-5, 2.e-1);
	  hJetMult   = new TH1D("hJetMult", "Jets Multiplicity "  , 11,0,11);
	  hEmpz      = new TH1D("hEmpz"   , "E_{M} - p_{Z}"       , 100,20,70);
	  hGamma     = new TH1D("hGamma"  , "cos(#gamma_{h})"     , 120,-1,-0.6);
	  hPtEt      = new TH1D("hPtEt"   , "P_{T} / #sqrt(E_{T}): from Zufo", 100,0,5);
	  hDiffEmpz  = new TH1D("hDiffEmpz", "E - p_{Z}: Zufo - Cal", 200,-20,20);
	  //Electron quantities
	  hElecTheta = new TH1D("hElecTheta", "Angle #theta of the DIS electron", 120, 130., 190.);
	  hElecPhi   = new TH1D("hElecPhi"  , "Angle #phi of the DIS electron"  , 60,-TMath::Pi(),TMath::Pi());
	  hElecE     = new TH1D("hElecE"    , "Energy of the DIS electron"      , 100, 10., 60.);
	  hElecProb  = new TH1D("hElecProb" , "Sinistra probability"            , 200, 0.89, 1.);
	  hElecy     = new TH1D("hElecy"    , "Inelasticity JB method"          , 250, -0.5, 1.);
	  h2ElecPos  = new TH2D("h2ElecPos" , "Sinistra probability"            , 800,-200,200,800,-200,200);
	  //Jets
	  hJetEt   = new TH1D("hJetEt"  ,"Jets Transverse Energy"  , 110,0,55);
	  hJetMass = new TH1D("hJetMass","Jets Mass"               , 110,0,55);
	  hJetPt   = new TH1D("hJetPt"  ,"Jets Transverse Momentum", 110,0,55);
	  hJetEta  = new TH1D("hJetEta" ,"Jets Eta"                , 100,-2.5,2.5);
	  hJetPhi  = new TH1D("hJetPhi" ,"Jets Phi"                , 60,0,2*TMath::Pi());
	  //Decorrelation
	
	  for(Int_t ijet =0; ijet < 5; ijet++){ 
	    hDecorrPhi[ijet]    = new TH1D(Form("hDecorrPhi_%d"   ,ijet),
					   Form("Jet - electron decorrelation angle, %d jets",ijet+1), 60,TMath::Pi()/2,TMath::Pi());
	    h2PtDecorrPhi[ijet] = new TH2D(Form("h2PtDecorrPhi_%d",ijet),
					   Form("P_{T} of the Jet vs angle, %d jets",ijet+1), 60,TMath::Pi()/2,TMath::Pi(),110,0,55);
	  }
	  TH2D* h2JetElecPhi  = new TH2D("h2JetElecPhi" ,"Jet vs electron #phi angle", 60,-TMath::Pi(),TMath::Pi(), 60,0,2*TMath::Pi());
	  ////END Define Histograms <--2--
  };

  void FillHists(bool isdata, bool isCalibr){
    
            //--------Correlation Angle------------
	    Float_t ElectronPhi = Siph[0] > 0. ? Siph[0] : 2*TMath::Pi() + Siph[0];  
	    Float_t DecorrPhi   = TMath::Abs( Kt_phijet_b[0] - ElectronPhi) ;
	    if(DecorrPhi > TMath::Pi()) DecorrPhi = 2*TMath::Pi() - DecorrPhi ;

	    Float_t Weight = 1.0;	
	    //if(!isdata && isCalibr) Weight=BinCalibr->getWeight(Siecorr[0][2]); //Lepton Energy
	    //if(!isdata && isCalibr) Weight=BinCalibr->getWeight(Siph[0]) ; //Lepton phi
	    //if(!isdata && isCalibr) Weight=BinCalibr->getWeight(Kt_phijet_b[0]); //Jet phi
	    if(!isdata && isCalibr) Weight=BinCalibr->getWeight(DecorrPhi); //by Decorrelation angle

	    Float_t Empz = 0.;
	    for(Int_t zloop=0; zloop<Nzufos; zloop++){
	      //TLorentzVector v(Zufo[zloop][0], Zufo[zloop][1], 
	      //	       Zufo[zloop][2], Zufo[zloop][3]);
	      Empz += Zufo[zloop][3] - Zufo[zloop][2];
	    }
	
	    //------------Event
	    hVertexZ->Fill(Zvtx                          ,Weight); 
	    h2Q2x   ->Fill(Sixel[0],Siq2el[0] ,Weight);
	    hQ2     ->Fill(Siq2el[0]                     ,Weight);
	    hx      ->Fill(Sixel[0]                      ,Weight);
	    hJetMult->Fill(Kt_njet_b                     ,Weight);
	    hEmpz   ->Fill(Empz                                     ,Weight);
	    //hEmpz   ->Fill(JetOrange->Cal_empz                      ,Weight);
	    hGamma  ->Fill(TMath::Cos(Cc_gamma)          ,Weight);
	    hPtEt   ->Fill(Cal_pt/sqrt(Cal_et),Weight);
	    hDiffEmpz->Fill(Empz - Cal_empz              ,Weight);
	    //------------Electron 
	    hElecTheta->Fill(Sith[0]*180.0/TMath::Pi()         ,Weight);
	    hElecPhi  ->Fill(Siph[0]                           ,Weight);
	    hElecE    ->Fill(Siecorr[0][2]                     ,Weight);
	    hElecProb ->Fill(Siprob[0]                         ,Weight);
	    hElecy    ->Fill(Siyjb[0]                          ,Weight);
	    h2ElecPos ->Fill(Sipos[0][0],Sipos[0][1],Weight);
	    //------------Jet
	    hJetEt  ->Fill(Kt_etjet_b[0] ,Weight);
	    hJetMass->Fill(Kt_masjet_b[0],Weight);
	    Float_t JetPt = sqrt(Kt_etjet_b[0]*Kt_etjet_b[0]-Kt_masjet_b[0]*Kt_masjet_b[0]) ;
	    hJetPt  ->Fill(JetPt                    ,Weight);
	    hJetEta ->Fill(Kt_etajet_b[0],Weight);
	    hJetPhi ->Fill(Kt_phijet_b[0],Weight);
	
	    //------------Correlation
	    for(Int_t ijet =0; ijet < 5; ijet++){ 
	      if(Kt_njet_b > ijet) {
		hDecorrPhi[ijet]   ->Fill(DecorrPhi       ,Weight);
		h2PtDecorrPhi[ijet]->Fill(DecorrPhi, JetPt,Weight);
	      }
	    }
	    /////////////////////END Fill histograms <--3.2--
  }

  void WriteHists(const Char_t* outdir,TString period, Char_t* runnumber_size, Char_t* sample, Bool_t isdata){
  	/////////////////////Save histograms in a file --5-->    
  	TString outdir1(outdir);
  	TString outfile(outdir1+"/Hist_"+period+"_"+runnumber_size+".root");
  	TString outfileMC(outdir1+"/Hist_"+sample);
  	TFile fout(isdata ? outfile : outfileMC,"RECREATE");
  	//Event
  	hVertexZ->Write();
  	h2Q2x->Write();
  	hQ2->Write();
  	hx->Write();
  	hJetMult->Write(); 
  	hEmpz->Write();
  	hGamma->Write();
  	hPtEt->Write();
  	hDiffEmpz->Write();
  	//Electron
  	hElecTheta->Write();
  	hElecPhi->Write();
  	hElecE->Write();
  	hElecProb->Write();
  	hElecy->Write();
  	h2ElecPos->Write();
  	//Jet
  	hJetEt->Write();
  	hJetPt->Write();
  	hJetMass->Write();
  	hJetEta->Write();
  	hJetPhi->Write();
  	//Decorrelation
  	for(Int_t ijet =0; ijet < 5; ijet++){
  	        hDecorrPhi[ijet]->Write();
  	        h2PtDecorrPhi[ijet]->Write();
  	}
  	fout.Close();
  	/////END Save histograms in a file <--5-- 
  }
  

};
};

namespace Plot2D {

class Selector : public JetOrangeSelect {
public:

  TH2D* h2EEleRC_MC;
  TH2D* h2EtEleRC_MC;
  TH2D* h2ElecThetaRC_MC;
  TH2D* h2ElecQ2RC_MC;
  TH2D* h2ElecXRC_MC;
  TH2D* h2ElecyRC_MC;
  // Additional histograms for 0.04 < y < 0.4* and 0.4 < y < 0.95
  // _____________ 0.04 < y < 0.4* _________________
  TH2D* first_h2EEleRC_MC;
  TH2D* first_h2EtEleRC_MC;
  TH2D* first_h2ElecThetaRC_MC; 
  TH2D* first_h2ElecQ2RC_MC;
  TH2D* first_h2ElecXRC_MC;
  TH2D* first_h2ElecyRC_MC;
  // _____________ 0.4 < y < 0.95 _________________
  TH2D* second_h2EEleRC_MC;
  TH2D* second_h2EtEleRC_MC;
  TH2D* second_h2ElecThetaRC_MC;
  TH2D* second_h2ElecQ2RC_MC;
  TH2D* second_h2ElecXRC_MC;
  TH2D* second_h2ElecyRC_MC;
  // Add Cal_empz and zufo_empz comperiros
  TH2D* h2_cal_empz;
  TH1D* cal_empz_min_Empz;

  float Ee_RC;
  float Ee_MC;
  float px_MC;
  float py_MC;
  float pz_MC;
  float E_MC;
  float Et_RC_min, E_RC_min;
  float E_RC;
  float theta_RC;
  float eta_RC;
  float pt_RC;
  float phi_RC;
  int iele; 

  TLorentzVector e_RC;
  TLorentzVector e_MC;
  
  Selector():JetOrangeSelect() {};

  void InitHists(){
  
	h2EEleRC_MC  = new TH2D("h2EEleRC_MC", "Reco vs True (E, best candidate)", 100, 0, 50, 100, 0, 50);
	h2EEleRC_MC->GetXaxis()->SetTitle("E_{RC}");
	h2EEleRC_MC->GetYaxis()->SetTitle("E_{MC}");

	h2EtEleRC_MC = new TH2D("h2EtEleRC_MC", "Reco vs True (Et, best candidate)", 100, 0, 20, 100, 0, 20);
	h2EtEleRC_MC->GetXaxis()->SetTitle("E_{T, RC}");
	h2EtEleRC_MC->GetYaxis()->SetTitle("E_{T, MC}");

	h2ElecThetaRC_MC = new TH2D("hElecThetaRC_MC", "Reco vs True (Angle #theta DIS electron)", 100, 130., 180., 100, 130., 180.);
	h2ElecThetaRC_MC->GetXaxis()->SetTitle("#theta_{RC}");
	h2ElecThetaRC_MC->GetYaxis()->SetTitle("#theta_{MC}");

	h2ElecQ2RC_MC = new TH2D("h2ElecQ2RC_MC","Reco vs True (Q^{2})", q2end-q2start, q2start, q2end, q2end-q2start, q2start, q2end);
	h2ElecQ2RC_MC->GetXaxis()->SetTitle("Q^{2}_{RC}");
	h2ElecQ2RC_MC->GetYaxis()->SetTitle("Q^{2}_{MC}");

	h2ElecXRC_MC = new TH2D("h2ElecXRC_MC","Reco vs True (x Bjorken)", 200, 2.e-5, 2.e-1, 200, 2.e-5, 2.e-1);
	h2ElecXRC_MC->GetXaxis()->SetTitle("x_{RC}");
	h2ElecXRC_MC->GetYaxis()->SetTitle("x_{MC}");

	h2ElecyRC_MC = new TH2D("h2ElecyRC_MC","Reco vs True (Inelasticity y)", 200, 0.0, 1.0, 200, 0.0, 1.0);
	h2ElecyRC_MC->GetXaxis()->SetTitle("y_{RC}");
	h2ElecyRC_MC->GetYaxis()->SetTitle("y_{MC}");

	// Additional histograms for 0.04 < y < 0.4* and 0.4 < y < 0.95

	// _____________ 0.04 < y < 0.4* _________________

	first_h2EEleRC_MC  = new TH2D("first_h2EEleRC_MC", "Reco vs True - E (0.04 < y < 0.4)", 100, 0, 50, 100, 0, 50);
	h2EEleRC_MC->GetXaxis()->SetTitle("E_{RC}");
	h2EEleRC_MC->GetYaxis()->SetTitle("E_{MC}");

	first_h2EtEleRC_MC = new TH2D("first_h2EtEleRC_MC", "Reco vs True - Et (0.04 < y < 0.4)", 100, 0, 20, 100, 0, 20);
	h2EtEleRC_MC->GetXaxis()->SetTitle("E_{T, RC}");
	h2EtEleRC_MC->GetYaxis()->SetTitle("E_{T, MC}");

	first_h2ElecThetaRC_MC = new TH2D("first_hElecThetaRC_MC", "Reco vs True - #theta (0.04 < y < 0.4)", 100, 130., 180., 100, 130., 180.);
	h2ElecThetaRC_MC->GetXaxis()->SetTitle("#theta_{RC}");
	h2ElecThetaRC_MC->GetYaxis()->SetTitle("#theta_{MC}");

	first_h2ElecQ2RC_MC = new TH2D("first_h2ElecQ2RC_MC","Reco vs True - Q^{2} (0.04 < y < 0.4)", q2end-q2start, q2start, q2end, q2end-q2start, q2start, q2end);
	h2ElecQ2RC_MC->GetXaxis()->SetTitle("Q^{2}_{RC}");
	h2ElecQ2RC_MC->GetYaxis()->SetTitle("Q^{2}_{MC}");

	first_h2ElecXRC_MC = new TH2D("first_h2ElecXRC_MC","Reco vs True - x (0.04 < y < 0.4)", 200, 2.e-5, 2.e-1, 200, 2.e-5, 2.e-1);
	h2ElecXRC_MC->GetXaxis()->SetTitle("x_{RC}");
	h2ElecXRC_MC->GetYaxis()->SetTitle("x_{MC}");

	first_h2ElecyRC_MC = new TH2D("first_h2ElecyRC_MC","Reco vs True y (0.04 < y < 0.4)", 200, 0.0, 1.0, 200, 0.0, 1.0);
	h2ElecyRC_MC->GetXaxis()->SetTitle("y_{RC}");
	h2ElecyRC_MC->GetYaxis()->SetTitle("y_{MC}");

	// _____________ 0.4 < y < 0.95 _________________
	second_h2EEleRC_MC  = new TH2D("second_h2EEleRC_MC", "Reco vs True - E (0.4 < y < 0.95)", 100, 0, 50, 100, 0, 50);
	h2EEleRC_MC->GetXaxis()->SetTitle("E_{RC}");
	h2EEleRC_MC->GetYaxis()->SetTitle("E_{MC}");

	second_h2EtEleRC_MC = new TH2D("second_h2EtEleRC_MC", "Reco vs True - Et (0.4 < y < 0.95)", 100, 0, 20, 100, 0, 20);
	h2EtEleRC_MC->GetXaxis()->SetTitle("E_{T, RC}");
	h2EtEleRC_MC->GetYaxis()->SetTitle("E_{T, MC}");

	second_h2ElecThetaRC_MC = new TH2D("second_hElecThetaRC_MC", "Reco vs True - #theta (0.4 < y < 0.95)", 100, 130., 180., 100, 130., 180.);
	h2ElecThetaRC_MC->GetXaxis()->SetTitle("#theta_{RC}");
	h2ElecThetaRC_MC->GetYaxis()->SetTitle("#theta_{MC}");

	second_h2ElecQ2RC_MC = new TH2D("second_h2ElecQ2RC_MC","Reco vs True - Q^{2} (0.4 < y < 0.95)", q2end-q2start, q2start, q2end, q2end-q2start, q2start, q2end);
	h2ElecQ2RC_MC->GetXaxis()->SetTitle("Q^{2}_{RC}");
	h2ElecQ2RC_MC->GetYaxis()->SetTitle("Q^{2}_{MC}");

	second_h2ElecXRC_MC = new TH2D("second_h2ElecXRC_MC","Reco vs True - x (0.4 < y < 0.95)", 200, 2.e-5, 2.e-1, 200, 2.e-5, 2.e-1);
	h2ElecXRC_MC->GetXaxis()->SetTitle("x_{RC}");
	h2ElecXRC_MC->GetYaxis()->SetTitle("x_{MC}");

	second_h2ElecyRC_MC = new TH2D("second_h2ElecyRC_MC","Reco vs True y (0.4 < y < 0.95)", 200, 0.0, 1.0, 200, 0.0, 1.0);
	h2ElecyRC_MC->GetXaxis()->SetTitle("y_{RC}");
	h2ElecyRC_MC->GetYaxis()->SetTitle("y_{MC}");


	// Add Cal_empz and zufo_empz comperiros
	h2_cal_empz = new TH2D("h2_cal_empz","Cal_empz vs Empz(zufo)", 200, 35.0, 65.0,200, 35.0, 65.0);
	h2_cal_empz->GetXaxis()->SetTitle("Cal_empz");	
	h2_cal_empz->GetYaxis()->SetTitle("Empz(zufo)");

	cal_empz_min_Empz = new TH1D("cal_empz_min_Empz","cal_empz_min_Empz - Empz",200, -100.0,100.0);
	cal_empz_min_Empz->GetXaxis()->SetTitle("cal_empz-Empz(zufo)");


	// TProfile* profEEleRC_MC  = new TProfile("profEEleRC_MC", "Reco vs True (E, profile, best candidate)", 100, 0, 50, 0, 50);
	// TProfile* profEtEleRC_MC = new TProfile("profEtEleRC_MC", "Reco vs True (Et, profile, best candidate)", 100, 0, 20, 0, 20);
  }

  bool CheckCuts(Long64_t jentry, TString period){
	if (Sierror) {
		std::cout << "Sierror != 0" << std::endl; return false;
	}

	Ee_RC = Siecorr[0][2];
	Ee_MC = Mc_pfsl[3];

	px_MC = Mc_pfsl[0];
	py_MC = Mc_pfsl[1];
	pz_MC = Mc_pfsl[2];
	E_MC = Mc_pfsl[3];
	e_MC.SetPxPyPzE(px_MC, py_MC, pz_MC, E_MC);
	
	float dR_min = 1;
	iele = 0;
	for (int i = 0; i < Sincand; ++i) {
		E_RC = Siecorr[i][2];
		theta_RC = Sith[i];
		eta_RC = -log(tan(theta_RC / 2.));
		pt_RC = Sipt[i];
		phi_RC = Siph[i];
		//float phi_RC = JetOrange->Siph[0] > 0. ? JetOrange->Siph[0] : 2*TMath::Pi() + JetOrange->Siph[0];
		e_RC.SetPtEtaPhiE(pt_RC, eta_RC, phi_RC, E_RC);
		float dR = e_RC.DeltaR(e_MC);
		if (dR < dR_min) {
			dR_min = dR;
			Et_RC_min = e_RC.Et();
			E_RC_min  = e_RC.E();
			iele = i;
		}
	}
	if (dR_min > 0.5)
		return false;

	return JetOrangeSelect::CheckCuts(period);
	  
  }

  void FillHists(const TString method = "EM"){
      
     Float_t Empz = 0.;
     for(Int_t zloop=0; zloop<Nzufos; zloop++){
       //TLorentzVector v(Zufo[zloop][0], Zufo[zloop][1], 
       //  	       Zufo[zloop][2], Zufo[zloop][3]);
       Empz += Zufo[zloop][3] - Zufo[zloop][2];
     }

     /////////////////////END Define Cuts <--3.1--

     // Ivan's 2D plots
     h2EEleRC_MC->Fill(E_RC_min, e_MC.E());
     h2EtEleRC_MC->Fill(Et_RC_min, e_MC.Et());
     
     // Additional variables
     float theta_MC = TMath::Pi() + TMath::ATan( TMath::Sqrt(px_MC*px_MC+py_MC*py_MC) / pz_MC);
     h2ElecThetaRC_MC->Fill(Sith[iele]*180.0/TMath::Pi(),theta_MC*180.0/TMath::Pi());
     
     //TString method("EM");
     
     if (method=="EM"){
     	h2ElecQ2RC_MC->Fill(Siq2el[iele],Mc_q2);
     	h2ElecXRC_MC->Fill(Sixel[iele],Mc_x);
     	h2ElecyRC_MC->Fill(Siyel[iele],Mc_y);
     }
     if (method=="JB"){
     	h2ElecQ2RC_MC->Fill(Siq2jb[iele],Mc_q2);
     	h2ElecXRC_MC->Fill(Sixjb[iele],Mc_x);
     	h2ElecyRC_MC->Fill(Siyjb[iele],Mc_y);
     }
     if (method=="DA"){
     	h2ElecQ2RC_MC->Fill(Siq2da[iele],Mc_q2);
     	h2ElecXRC_MC->Fill(Sixda[iele],Mc_x);
     	h2ElecyRC_MC->Fill(Siyda[iele],Mc_y);
     }
     
     // Additional histograms for 0.04 < y < 0.4* and 0.4 < y < 0.95 
     
     if(Siyjb[iele] > 0.04 && Siyel[iele] < 0.4)
     {
     	first_h2EEleRC_MC->Fill(E_RC_min, e_MC.E());
     	first_h2EtEleRC_MC->Fill(Et_RC_min, e_MC.Et());
     
     	// Additional variables
     	theta_MC = TMath::Pi() + TMath::ATan( TMath::Sqrt(px_MC*px_MC+py_MC*py_MC) / pz_MC);
     	first_h2ElecThetaRC_MC->Fill(Sith[iele]*180.0/TMath::Pi(),theta_MC*180.0/TMath::Pi());
     
     	if (method=="EM"){
     		first_h2ElecQ2RC_MC->Fill(Siq2el[iele],Mc_q2);
     		first_h2ElecXRC_MC->Fill(Sixel[iele],Mc_x);
     		first_h2ElecyRC_MC->Fill(Siyel[iele],Mc_y);
     	}
     	if (method=="JB"){
     		first_h2ElecQ2RC_MC->Fill(Siq2jb[iele],Mc_q2);
     		first_h2ElecXRC_MC->Fill(Sixjb[iele],Mc_x);
     		first_h2ElecyRC_MC->Fill(Siyjb[iele],Mc_y);
     	}
     	if (method=="DA"){
     		first_h2ElecQ2RC_MC->Fill(Siq2da[iele],Mc_q2);
     		first_h2ElecXRC_MC->Fill(Sixda[iele],Mc_x);
     		first_h2ElecyRC_MC->Fill(Siyda[iele],Mc_y);
     	}
     }
     
     if(Siyel[iele] >= 0.4 && Siyel[iele] < 0.95)
     {
     	second_h2EEleRC_MC->Fill(E_RC_min, e_MC.E());
     	second_h2EtEleRC_MC->Fill(Et_RC_min, e_MC.Et());
     
     	// Additional variables
     	theta_MC = TMath::Pi() + TMath::ATan( TMath::Sqrt(px_MC*px_MC+py_MC*py_MC) / pz_MC);
     	second_h2ElecThetaRC_MC->Fill(Sith[iele]*180.0/TMath::Pi(),theta_MC*180.0/TMath::Pi());
     
     	if (method=="EM"){
     		second_h2ElecQ2RC_MC->Fill(Siq2el[iele],Mc_q2);
     		second_h2ElecXRC_MC->Fill(Sixel[iele],Mc_x);
     		second_h2ElecyRC_MC->Fill(Siyel[iele],Mc_y);
     	}
     	if (method=="JB"){
     		second_h2ElecQ2RC_MC->Fill(Siq2jb[iele],Mc_q2);
     		second_h2ElecXRC_MC->Fill(Sixjb[iele],Mc_x);
     		second_h2ElecyRC_MC->Fill(Siyjb[iele],Mc_y);
     	}
     	if (method=="DA"){
     		second_h2ElecQ2RC_MC->Fill(Siq2da[iele],Mc_q2);
     		second_h2ElecXRC_MC->Fill(Sixda[iele],Mc_x);
     		second_h2ElecyRC_MC->Fill(Siyda[iele],Mc_y);
     	}
     }
     
     h2_cal_empz->Fill(Cal_empz,Empz);
     cal_empz_min_Empz->Fill(Cal_empz-Empz);
     // profEEleRC_MC->Fill(E_RC_min, e_MC.E());
     // profEtEleRC_MC->Fill(Et_RC_min, e_MC.Et());
     
     /////////////////////END Fill histograms <--3.2--
  }

  void WriteHists(const Char_t* outdir,TString period, Char_t* runnumber_size, Char_t* sample, Bool_t isdata){
  	/////////////////////Save histograms in a file --5-->
 	
	TString outdir1(outdir);
	TString outfile(outdir1+"/Hist_"+period+"_"+runnumber_size+".root");
	TString outfileMC(outdir1+"/Hist_"+sample);
	TFile fout(isdata ? outfile : outfileMC,"RECREATE");
	//Jet
	h2EEleRC_MC->Write();
	h2EtEleRC_MC->Write();
	h2ElecThetaRC_MC->Write();
	h2ElecQ2RC_MC->Write();
	h2ElecXRC_MC->Write();
	h2ElecyRC_MC->Write();
	// profEEleRC_MC->Write();
	// profEtEleRC_MC->Write();

	first_h2EEleRC_MC->Write();
	first_h2EtEleRC_MC->Write();
	first_h2ElecThetaRC_MC->Write();
	first_h2ElecQ2RC_MC->Write();
	first_h2ElecXRC_MC->Write();
	first_h2ElecyRC_MC->Write();

	second_h2EEleRC_MC->Write();
	second_h2EtEleRC_MC->Write();
	second_h2ElecThetaRC_MC->Write();	second_h2ElecQ2RC_MC->Write();
	second_h2ElecQ2RC_MC->Write();
	second_h2ElecXRC_MC->Write();
	second_h2ElecyRC_MC->Write();

	h2_cal_empz->Write();
	cal_empz_min_Empz->Write();

	fout.Close();
	/////END Save histograms in a file <--5--   
  }    

};
};
#endif
