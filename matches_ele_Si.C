/////Amilkar Quintero
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

	//Float_t weight = lumi /total;
	return 1; //  return weight;
}

//--->START MAIN PROGRAM
//________________________________________________________________________________
void matches_ele_Si(const Char_t *eachfile= "/pnfs/desy.de/dphep/online/zeus/z/ntup/07p/v08b/mc/root/zeusmc.hfix627.h14207.djangoh.cdm.pos.q24.gnor2515.root", 
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

	TH2D* h2EEleRC_MC  = new TH2D("h2EEleRC_MC", "Reco vs True (E, best candidate)", 100, 0, 50, 100, 0, 50);
	h2EEleRC_MC->GetXaxis()->SetTitle("E_{RC}");
	h2EEleRC_MC->GetYaxis()->SetTitle("E_{MC}");

	TH2D* h2EtEleRC_MC = new TH2D("h2EtEleRC_MC", "Reco vs True (Et, best candidate)", 100, 0, 20, 100, 0, 20);
	h2EtEleRC_MC->GetXaxis()->SetTitle("E_{T, RC}");
	h2EtEleRC_MC->GetYaxis()->SetTitle("E_{T, MC}");

	TH2D* h2ElecThetaRC_MC = new TH2D("hElecThetaRC_MC", "Reco vs True (Angle #theta DIS electron)", 100, 130., 180., 100, 130., 180.);
	h2ElecThetaRC_MC->GetXaxis()->SetTitle("#theta_{RC}");
	h2ElecThetaRC_MC->GetYaxis()->SetTitle("#theta_{MC}");

	TH2D* h2ElecQ2RC_MC = new TH2D("h2ElecQ2RC_MC","Reco vs True (Q^{2})", q2end-q2start, q2start, q2end, q2end-q2start, q2start, q2end);
	h2ElecQ2RC_MC->GetXaxis()->SetTitle("Q^{2}_{RC}");
	h2ElecQ2RC_MC->GetYaxis()->SetTitle("Q^{2}_{MC}");

	TH2D* h2ElecXRC_MC = new TH2D("h2ElecXRC_MC","Reco vs True (x Bjorken)", 200, 2.e-5, 2.e-1, 200, 2.e-5, 2.e-1);
	h2ElecXRC_MC->GetXaxis()->SetTitle("x_{RC}");
	h2ElecXRC_MC->GetYaxis()->SetTitle("x_{MC}");

	TH2D* h2ElecyRC_MC = new TH2D("h2ElecyRC_MC","Reco vs True (Inelasticity y)", 200, 0.0, 1.0, 200, 0.0, 1.0);
	h2ElecyRC_MC->GetXaxis()->SetTitle("y_{RC}");
	h2ElecyRC_MC->GetYaxis()->SetTitle("y_{MC}");

	// Additional histograms for 0.04 < y < 0.4* and 0.4 < y < 0.95

	// _____________ 0.04 < y < 0.4* _________________

	TH2D* first_h2EEleRC_MC  = new TH2D("first_h2EEleRC_MC", "Reco vs True - E (0.04 < y < 0.4)", 100, 0, 50, 100, 0, 50);
	h2EEleRC_MC->GetXaxis()->SetTitle("E_{RC}");
	h2EEleRC_MC->GetYaxis()->SetTitle("E_{MC}");

	TH2D* first_h2EtEleRC_MC = new TH2D("first_h2EtEleRC_MC", "Reco vs True - Et (0.04 < y < 0.4)", 100, 0, 20, 100, 0, 20);
	h2EtEleRC_MC->GetXaxis()->SetTitle("E_{T, RC}");
	h2EtEleRC_MC->GetYaxis()->SetTitle("E_{T, MC}");

	TH2D* first_h2ElecThetaRC_MC = new TH2D("first_hElecThetaRC_MC", "Reco vs True - #theta (0.04 < y < 0.4)", 100, 130., 180., 100, 130., 180.);
	h2ElecThetaRC_MC->GetXaxis()->SetTitle("#theta_{RC}");
	h2ElecThetaRC_MC->GetYaxis()->SetTitle("#theta_{MC}");

	TH2D* first_h2ElecQ2RC_MC = new TH2D("first_h2ElecQ2RC_MC","Reco vs True - Q^{2} (0.04 < y < 0.4)", q2end-q2start, q2start, q2end, q2end-q2start, q2start, q2end);
	h2ElecQ2RC_MC->GetXaxis()->SetTitle("Q^{2}_{RC}");
	h2ElecQ2RC_MC->GetYaxis()->SetTitle("Q^{2}_{MC}");

	TH2D* first_h2ElecXRC_MC = new TH2D("first_h2ElecXRC_MC","Reco vs True - x (0.04 < y < 0.4)", 200, 2.e-5, 2.e-1, 200, 2.e-5, 2.e-1);
	h2ElecXRC_MC->GetXaxis()->SetTitle("x_{RC}");
	h2ElecXRC_MC->GetYaxis()->SetTitle("x_{MC}");

	TH2D* first_h2ElecyRC_MC = new TH2D("first_h2ElecyRC_MC","Reco vs True y (0.04 < y < 0.4)", 200, 0.0, 1.0, 200, 0.0, 1.0);
	h2ElecyRC_MC->GetXaxis()->SetTitle("y_{RC}");
	h2ElecyRC_MC->GetYaxis()->SetTitle("y_{MC}");

	// _____________ 0.4 < y < 0.95 _________________
	TH2D* second_h2EEleRC_MC  = new TH2D("second_h2EEleRC_MC", "Reco vs True - E (0.4 < y < 0.95)", 100, 0, 50, 100, 0, 50);
	h2EEleRC_MC->GetXaxis()->SetTitle("E_{RC}");
	h2EEleRC_MC->GetYaxis()->SetTitle("E_{MC}");

	TH2D* second_h2EtEleRC_MC = new TH2D("second_h2EtEleRC_MC", "Reco vs True - Et (0.4 < y < 0.95)", 100, 0, 20, 100, 0, 20);
	h2EtEleRC_MC->GetXaxis()->SetTitle("E_{T, RC}");
	h2EtEleRC_MC->GetYaxis()->SetTitle("E_{T, MC}");

	TH2D* second_h2ElecThetaRC_MC = new TH2D("second_hElecThetaRC_MC", "Reco vs True - #theta (0.4 < y < 0.95)", 100, 130., 180., 100, 130., 180.);
	h2ElecThetaRC_MC->GetXaxis()->SetTitle("#theta_{RC}");
	h2ElecThetaRC_MC->GetYaxis()->SetTitle("#theta_{MC}");

	TH2D* second_h2ElecQ2RC_MC = new TH2D("second_h2ElecQ2RC_MC","Reco vs True - Q^{2} (0.4 < y < 0.95)", q2end-q2start, q2start, q2end, q2end-q2start, q2start, q2end);
	h2ElecQ2RC_MC->GetXaxis()->SetTitle("Q^{2}_{RC}");
	h2ElecQ2RC_MC->GetYaxis()->SetTitle("Q^{2}_{MC}");

	TH2D* second_h2ElecXRC_MC = new TH2D("second_h2ElecXRC_MC","Reco vs True - x (0.4 < y < 0.95)", 200, 2.e-5, 2.e-1, 200, 2.e-5, 2.e-1);
	h2ElecXRC_MC->GetXaxis()->SetTitle("x_{RC}");
	h2ElecXRC_MC->GetYaxis()->SetTitle("x_{MC}");

	TH2D* second_h2ElecyRC_MC = new TH2D("second_h2ElecyRC_MC","Reco vs True y (0.4 < y < 0.95)", 200, 0.0, 1.0, 200, 0.0, 1.0);
	h2ElecyRC_MC->GetXaxis()->SetTitle("y_{RC}");
	h2ElecyRC_MC->GetYaxis()->SetTitle("y_{MC}");


	// Add Cal_empz and zufo_empz comperiros
	TH2D* h2_cal_empz = new TH2D("h2_cal_empz","Cal_empz vs Empz(zufo)", 200, 35, 65, 35, 65);
	h2_cal_empz->GetXaxis()->SetTitle("Cal_empz");	
	h2_cal_empz->GetYaxis()->SetTitle("Empz(zufo)");

	TH1D* cal_empz_min_Empz = new TH1D("cal_empz_min_Empz","cal_empz_min_Empz - Empz",200, -100,100);
	cal_empz_min_Empz->GetXaxis()->SetTitle("cal_empz-Empz(zufo)");


	// TProfile* profEEleRC_MC  = new TProfile("profEEleRC_MC", "Reco vs True (E, profile, best candidate)", 100, 0, 50, 0, 50);
	// TProfile* profEtEleRC_MC = new TProfile("profEtEleRC_MC", "Reco vs True (Et, profile, best candidate)", 100, 0, 20, 0, 20);


	//Decorrelation
	//TH1D* hDecorrPhi[5];   TH2D* h2PtDecorrPhi[5];
	//for(Int_t ijet =0; ijet < 5; ijet++){ 
	//hDecorrPhi[ijet]    = new TH1D(Form("hDecorrPhi_%d"   ,ijet),
	//Form("Jet - electron decorrelation angle, %d jets",ijet+1), 60,TMath::Pi()/2,TMath::Pi());
	//h2PtDecorrPhi[ijet] = new TH2D(Form("h2PtDecorrPhi_%d",ijet),
	//Form("P_{T} of the Jet vs angle, %d jets",ijet+1), 60,TMath::Pi()/2,TMath::Pi(),110,0,55);
	//}
	//TH2D* h2JetElecPhi  = new TH2D("h2JetElecPhi" ,"Jet vs electron #phi angle", 60,-TMath::Pi(),TMath::Pi(), 60,0,2*TMath::Pi());
	////END Define Histograms <--2--


	///////Start filling the histograms from the trees  --3-->
	Int_t count = 0;
	Long64_t nentries = firstJet->GetEntriesFast();
	cout << "Number of events before the cuts: " << nentries << endl;
	for (Long64_t jentry=0; jentry<nentries;jentry++) {
		JetOrange->GetEntry(jentry);

		if (JetOrange->Sierror) {
			std::cout << "Sierror != 0" << std::endl;
			continue;
		}

		float Ee_RC = JetOrange->Siecorr[0][2];
		float Ee_MC = JetOrange->Mc_pfsl[3];
		
		TLorentzVector e_RC;
		TLorentzVector e_MC;
		float px_MC = JetOrange->Mc_pfsl[0];
		float py_MC = JetOrange->Mc_pfsl[1];
		float pz_MC = JetOrange->Mc_pfsl[2];
		float E_MC = JetOrange->Mc_pfsl[3];
		e_MC.SetPxPyPzE(px_MC, py_MC, pz_MC, E_MC);

		float Et_RC_min, E_RC_min;
		float dR_min = 1;
		int iele = 0;
		for (int i = 0; i < JetOrange->Sincand; ++i) {
			float E_RC = JetOrange->Siecorr[i][2];
			float theta_RC = JetOrange->Sith[i];
			float eta_RC = -log(tan(theta_RC / 2.));
			float pt_RC = JetOrange->Sipt[i];
			float phi_RC = JetOrange->Siph[i];
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
			continue;

		/////////////////////Define Cuts --3.1-->
		/////Using only the electron with the highest probability Siq2el[0]
		/////Using only the highest jet in the event: Kt_etajet_b[0]
		/////Phase Space 
		if(JetOrange->Siq2el[iele]<q2start || JetOrange->Siq2el[iele]>q2end) continue;
		if(JetOrange->Siq2el[iele]<10) continue;
		/////Cleanning cuts
		if(JetOrange->Zvtx<-40 || JetOrange->Zvtx>40 || JetOrange->Zvtx==0)    continue;
		if(JetOrange->Cal_empz<35 || JetOrange->Cal_empz>65) continue;  
		//Calculate E-pz from zufo
		Float_t Empz = 0.;
		for(Int_t zloop=0; zloop<JetOrange->Nzufos; zloop++){
			TLorentzVector v(JetOrange->Zufo[zloop][0], JetOrange->Zufo[zloop][1], 
					JetOrange->Zufo[zloop][2], JetOrange->Zufo[zloop][3]);
			Empz += JetOrange->Zufo[zloop][3] - JetOrange->Zufo[zloop][2];
		}
		//cout << Form("Calorimeter: %f     Zufo: %f     Diff: %f",JetOrange->Cal_empz,Empz,JetOrange->Cal_empz - Empz) << endl;
		if(Empz < 35. || Empz > 65.) continue;
		if(JetOrange->Siyel[iele] > 0.95) continue;
		if(JetOrange->Siyjb[iele] < 0.04) continue;
		if(JetOrange->Cal_pt / TMath::Sqrt(JetOrange->Cal_et) > 2.5) continue;
		/////Electron cuts
		if(JetOrange->Siecorr[iele][2] < 10) continue;
		if(JetOrange->Sith[iele]*180.0/TMath::Pi() < 140 || JetOrange->Sith[iele]*180.0/TMath::Pi() > 180.0) continue;
		if(JetOrange->Sipos[iele][2] < -148. && JetOrange->Sipos[iele][0] > -14. 
				&& JetOrange->Sipos[iele][0] < 12. && JetOrange->Sipos[iele][1] > 90.) continue;                //Chimney cut
		if(sqrt(JetOrange->Sipos[iele][0]*JetOrange->Sipos[iele][0] + JetOrange->Sipos[iele][1]*JetOrange->Sipos[iele][1]) < 20.0) continue;
		/////Triggers
		if(period == "0405e" && !(JetOrange->Tltw[2] & (1 << 1)) ) continue;      //SPP02
		if(period == "06e"   && !(JetOrange->Tltw[2] & (1 << 8)) ) continue;      //SPP09
		if(period == "0607p" && !(JetOrange->Tltw[2] & (1 << 8)) ) continue;      //SPP09
		/////Jet selection
		if(JetOrange->Kt_njet_b <= 0) continue;
		if(JetOrange->Kt_etajet_b[0]<-1.5 || JetOrange->Kt_etajet_b[0]>1.8) continue;
		if(JetOrange->Kt_etjet_b[0] < 2.5) continue;
		/////////////////////END Define Cuts <--3.1--

		// Ivan's 2D plots
		h2EEleRC_MC->Fill(E_RC_min, e_MC.E());
		h2EtEleRC_MC->Fill(Et_RC_min, e_MC.Et());

		// Additional variables
		float theta_MC = TMath::Pi() + TMath::ATan( TMath::Sqrt(px_MC*px_MC+py_MC*py_MC) / pz_MC);
		h2ElecThetaRC_MC->Fill(JetOrange->Sith[iele]*180.0/TMath::Pi(),theta_MC*180.0/TMath::Pi());

		TString method("EM");

		if (method=="EM"){
			h2ElecQ2RC_MC->Fill(JetOrange->Siq2el[iele],JetOrange->Mc_q2);
			h2ElecXRC_MC->Fill(JetOrange->Sixel[iele],JetOrange->Mc_x);
			h2ElecyRC_MC->Fill(JetOrange->Siyel[iele],JetOrange->Mc_y);
		}
		if (method=="JB"){
			h2ElecQ2RC_MC->Fill(JetOrange->Siq2jb[iele],JetOrange->Mc_q2);
			h2ElecXRC_MC->Fill(JetOrange->Sixjb[iele],JetOrange->Mc_x);
			h2ElecyRC_MC->Fill(JetOrange->Siyjb[iele],JetOrange->Mc_y);
		}
		if (method=="DA"){
			h2ElecQ2RC_MC->Fill(JetOrange->Siq2da[iele],JetOrange->Mc_q2);
			h2ElecXRC_MC->Fill(JetOrange->Sixda[iele],JetOrange->Mc_x);
			h2ElecyRC_MC->Fill(JetOrange->Siyda[iele],JetOrange->Mc_y);
		}

		// Additional histograms for 0.04 < y < 0.4* and 0.4 < y < 0.95 

		if(JetOrange->Siyjb[iele] > 0.04 && JetOrange->Siyel[iele] < 0.4)
		{
			first_h2EEleRC_MC->Fill(E_RC_min, e_MC.E());
			first_h2EtEleRC_MC->Fill(Et_RC_min, e_MC.Et());

			// Additional variables
			theta_MC = TMath::Pi() + TMath::ATan( TMath::Sqrt(px_MC*px_MC+py_MC*py_MC) / pz_MC);
			first_h2ElecThetaRC_MC->Fill(JetOrange->Sith[iele]*180.0/TMath::Pi(),theta_MC*180.0/TMath::Pi());

			if (method=="EM"){
				first_h2ElecQ2RC_MC->Fill(JetOrange->Siq2el[iele],JetOrange->Mc_q2);
				first_h2ElecXRC_MC->Fill(JetOrange->Sixel[iele],JetOrange->Mc_x);
				first_h2ElecyRC_MC->Fill(JetOrange->Siyel[iele],JetOrange->Mc_y);
			}
			if (method=="JB"){
				first_h2ElecQ2RC_MC->Fill(JetOrange->Siq2jb[iele],JetOrange->Mc_q2);
				first_h2ElecXRC_MC->Fill(JetOrange->Sixjb[iele],JetOrange->Mc_x);
				first_h2ElecyRC_MC->Fill(JetOrange->Siyjb[iele],JetOrange->Mc_y);
			}
			if (method=="DA"){
				first_h2ElecQ2RC_MC->Fill(JetOrange->Siq2da[iele],JetOrange->Mc_q2);
				first_h2ElecXRC_MC->Fill(JetOrange->Sixda[iele],JetOrange->Mc_x);
				first_h2ElecyRC_MC->Fill(JetOrange->Siyda[iele],JetOrange->Mc_y);
			}
		}

		if(JetOrange->Siyel[iele] >= 0.4 && JetOrange->Siyel[iele] < 0.95)
		{
			second_h2EEleRC_MC->Fill(E_RC_min, e_MC.E());
			second_h2EtEleRC_MC->Fill(Et_RC_min, e_MC.Et());

			// Additional variables
			theta_MC = TMath::Pi() + TMath::ATan( TMath::Sqrt(px_MC*px_MC+py_MC*py_MC) / pz_MC);
			second_h2ElecThetaRC_MC->Fill(JetOrange->Sith[iele]*180.0/TMath::Pi(),theta_MC*180.0/TMath::Pi());

			if (method=="EM"){
				second_h2ElecQ2RC_MC->Fill(JetOrange->Siq2el[iele],JetOrange->Mc_q2);
				second_h2ElecXRC_MC->Fill(JetOrange->Sixel[iele],JetOrange->Mc_x);
				second_h2ElecyRC_MC->Fill(JetOrange->Siyel[iele],JetOrange->Mc_y);
			}
			if (method=="JB"){
				second_h2ElecQ2RC_MC->Fill(JetOrange->Siq2jb[iele],JetOrange->Mc_q2);
				second_h2ElecXRC_MC->Fill(JetOrange->Sixjb[iele],JetOrange->Mc_x);
				second_h2ElecyRC_MC->Fill(JetOrange->Siyjb[iele],JetOrange->Mc_y);
			}
			if (method=="DA"){
				second_h2ElecQ2RC_MC->Fill(JetOrange->Siq2da[iele],JetOrange->Mc_q2);
				second_h2ElecXRC_MC->Fill(JetOrange->Sixda[iele],JetOrange->Mc_x);
				second_h2ElecyRC_MC->Fill(JetOrange->Siyda[iele],JetOrange->Mc_y);
			}
		}

		h2_cal_empz->Fill(JetOrange->Cal_empz,Empz);
		cal_empz_min_Empz->Fill(JetOrange->Cal_empz-Empz);
		// profEEleRC_MC->Fill(E_RC_min, e_MC.E());
		// profEtEleRC_MC->Fill(Et_RC_min, e_MC.Et());

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

	fout.Close();
	/////END Save histograms in a file <--5-- 

	TDatime now1;
	now1.Print();
}
