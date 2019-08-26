#include <string>
#include "Riostream.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TString.h"
#include "TDatime.h"
#include "TMath.h"
#include "TStyle.h"

/*
TF1* FitRatio(TH1D *hdataElecE_R)
{
	
  //	TCanvas *c = new TCanvas("c","Event",0,0,1300,900);
	TF1 *pol = new TF1("pol","[0]+[1]*x+[2]*x**2+[3]*x**3", 0.0, 60.0);
	//pol->SetParameter(0,0,0);
	hdataElecE_R->Fit("pol");
	//pol->Draw("AP");
	return pol;

}
*/

//--->START MAIN PROGRAM
//________________________________________________________________________________
void Plot_Purity(const Char_t *mc_true_p= "./output_Sample_0607p.list.root",
		   const Char_t *mc_reco_p= "./output_Sample_Ariadne_Low_Q2_NC_DIS_0607p.list.root"){
 
  // gROOT->SetBatch(kTRUE); // To turn off screen output
  gROOT->SetStyle("ATLAS");

  //  gROOT->cd();
  TDatime now;                                          //Set time in Root
  now.Print();
  // gStyle->SetOptDate(0);
  // gStyle->SetOptStat(0);
  
  /////Read files --1-->
  TFile *ftrue = new TFile(mc_true_p);  
  TFile *freco = new TFile(mc_reco_p);

  /////END Read files <--1--
  Bool_t RESCALE = 0;
  Bool_t RESCALE_INT = 1;

  Bool_t REBIN = 0;    
  Int_t binXX = 5;
  
  ////Define Histograms --2-->
  
  //Event Quantities
  //Decorrelation
   TH1D* hDecorrPhi_true[5];
   TH2D* h2PtDecorrPhi_true[5];
   TH2D* h2ElDecorrPhi_true[5];
   TH2D* h2Q2DecorrPhi_true[5];

   TH1D* hDecorrPhi_reco[5];
   TH2D* h2PtDecorrPhi_reco[5];
   TH2D* h2ElDecorrPhi_reco[5];
   TH2D* h2Q2DecorrPhi_reco[5];

   for(Int_t ijet =0; ijet < 5; ijet++){
    hDecorrPhi_true[ijet] = (TH1D*)ftrue->Get(Form("hDecorrPhi_%d"   ,ijet));
    h2PtDecorrPhi_true[ijet] = (TH2D*)ftrue->Get(Form("h2PtDecorrPhi_%d",ijet));
    h2ElDecorrPhi_true[ijet] = (TH2D*)ftrue->Get(Form("h2ElDecorrPhi_%d",ijet));
    h2Q2DecorrPhi_true[ijet] = (TH2D*)ftrue->Get(Form("h2Q2DecorrPhi_%d",ijet));

    hDecorrPhi_reco[ijet] = (TH1D*)freco->Get(Form("hDecorrPhi_%d"   ,ijet));
    h2PtDecorrPhi_reco[ijet] = (TH2D*)freco->Get(Form("h2PtDecorrPhi_%d",ijet));
    h2ElDecorrPhi_reco[ijet] = (TH2D*)freco->Get(Form("h2ElDecorrPhi_%d",ijet));
    h2Q2DecorrPhi_reco[ijet] = (TH2D*)freco->Get(Form("h2Q2DecorrPhi_%d",ijet));
   } 

   //Q2 part

   //Q2 intervals
   int q2_1 = h2Q2DecorrPhi_true[0]->GetYaxis()->FindBin(10.0);
   int q2_2 = h2Q2DecorrPhi_true[0]->GetYaxis()->FindBin(50.0);
   int q2_3 = h2Q2DecorrPhi_true[0]->GetYaxis()->FindBin(150.0);
   int q2_4 = h2Q2DecorrPhi_true[0]->GetYaxis()->FindBin(350.0);

   //Q2 diagrams
   TH1D* h2Q2DecorrPhi_true_first[5];
   TH1D* h2Q2DecorrPhi_true_second[5];
   TH1D* h2Q2DecorrPhi_true_third[5];

   TH1D* h2Q2DecorrPhi_reco_first[5];
   TH1D* h2Q2DecorrPhi_reco_second[5];
   TH1D* h2Q2DecorrPhi_reco_third[5];

   TH1D* h2Q2DecorrPhi_first_R[5];
   TH1D* h2Q2DecorrPhi_second_R[5];
   TH1D* h2Q2DecorrPhi_third_R[5];

   for(Int_t ijet = 0; ijet < 5; ijet++){
   h2Q2DecorrPhi_true_first[ijet]  = (TH1D*) h2Q2DecorrPhi_true[ijet]->ProjectionX(Form("Q2 first true %d",ijet),q2_1,q2_2);
   h2Q2DecorrPhi_true_second[ijet] = (TH1D*) h2Q2DecorrPhi_true[ijet]->ProjectionX(Form("Q2 second true %d",ijet),q2_2,q2_3);
   h2Q2DecorrPhi_true_third[ijet]  = (TH1D*) h2Q2DecorrPhi_true[ijet]->ProjectionX(Form("Q2 third true %d",ijet),q2_3,q2_4);

   h2Q2DecorrPhi_reco_first[ijet]  = (TH1D*) h2Q2DecorrPhi_reco[ijet]->ProjectionX(Form("Q2 first reco %d",ijet),q2_1,q2_2);
   h2Q2DecorrPhi_reco_second[ijet] = (TH1D*) h2Q2DecorrPhi_reco[ijet]->ProjectionX(Form("Q2 second reco %d",ijet),q2_2,q2_3);
   h2Q2DecorrPhi_reco_third[ijet]  = (TH1D*) h2Q2DecorrPhi_reco[ijet]->ProjectionX(Form("Q2 third reco %d",ijet),q2_3,q2_4);

   h2Q2DecorrPhi_first_R[ijet] = (TH1D*) h2Q2DecorrPhi_reco_first[ijet]->Clone();
   h2Q2DecorrPhi_second_R[ijet] = (TH1D*) h2Q2DecorrPhi_reco_second[ijet]->Clone();
   h2Q2DecorrPhi_third_R[ijet] = (TH1D*) h2Q2DecorrPhi_reco_third[ijet]->Clone();

   h2Q2DecorrPhi_first_R[ijet]->Divide(h2Q2DecorrPhi_true_first[ijet]);
   h2Q2DecorrPhi_second_R[ijet]->Divide(h2Q2DecorrPhi_true_second[ijet]);
   h2Q2DecorrPhi_third_R[ijet]->Divide(h2Q2DecorrPhi_true_third[ijet]);
   }

   TCanvas *c0 = new TCanvas("c0","Event",0,0,1300,900);
   c0->cd();
   // c0->Divide(3,1);

   h2Q2DecorrPhi_first_R[0]->SetMarkerStyle(23);
   h2Q2DecorrPhi_first_R[0]->SetFillColor(1);
   h2Q2DecorrPhi_first_R[0]->SetLineColor(1);
   h2Q2DecorrPhi_first_R[0]->SetMarkerColor(1);
   h2Q2DecorrPhi_first_R[0]->Draw("e1c");


   h2Q2DecorrPhi_second_R[0]->SetMarkerStyle(24);
   h2Q2DecorrPhi_second_R[0]->SetFillColor(4);
   h2Q2DecorrPhi_second_R[0]->SetLineColor(4);
   h2Q2DecorrPhi_second_R[0]->SetMarkerColor(4);
   h2Q2DecorrPhi_second_R[0]->Draw("e1cSame");

   h2Q2DecorrPhi_third_R[0]->SetMarkerStyle(25);
   h2Q2DecorrPhi_third_R[0]->SetFillColor(2);
   h2Q2DecorrPhi_third_R[0]->SetLineColor(2);
   h2Q2DecorrPhi_third_R[0]->SetMarkerColor(2);
   h2Q2DecorrPhi_third_R[0]->Draw("e1cSame");

   c0->Update();

   std::cout << "Press Enter to Continue";
   std::cin.ignore();


}