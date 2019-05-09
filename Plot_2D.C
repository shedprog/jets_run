#include <string>
#include "Riostream.h"
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TString.h"
#include "TDatime.h"
#include "TMath.h"
#include "TStyle.h"

template <typename T,typename T1,typename T2>
void histPlot(T hist1,T1 hist2,T2 hist3){
  TCanvas *canvas = new TCanvas("canvas","Event",0,0,2000,900);
  // gPad->SetLogz();
  canvas->cd();
  canvas->Divide(3,1);
  canvas->cd(1);
  canvas->cd(1)->SetLogz();
  hist1->Draw("colz");
  canvas->cd(2);
  canvas->cd(2)->SetLogz();
  hist2->Draw("colz");
  canvas->cd(3);
  canvas->cd(3)->SetLogz();
  hist3->Draw("colz");
  gSystem->Exec("mkdir -p ./JB_3");
  canvas->SaveAs(Form("./JB_3/%s.png",hist1->GetName()));
  delete canvas;
}

//--->START MAIN PROGRAM
//________________________________________________________________________________
void Plot_2D(const Char_t *eachfile= "./EM_RC.root"){

  gROOT->SetBatch(kTRUE); // To turn off screen output
  //  gROOT->cd();
  TDatime now;                                          //Set time in Root
  now.Print();
  gStyle->SetOptDate(0);
  gStyle->SetOptStat(0);
  
  /////Read files --1-->
  TFile *fdata = new TFile(eachfile);  

  Bool_t REBIN = 0;    
  // Int_t binXX = 5;
  
  ////Define Histograms --2-->

  TH2D* h2EEleRC_MC	     		= (TH2D*)fdata->Get("h2EEleRC_MC");
  TH2D* h2EtEleRC_MC	 		= (TH2D*)fdata->Get("h2EtEleRC_MC");
  TH2D* hElecThetaRC_MC	 		= (TH2D*)fdata->Get("hElecThetaRC_MC");
  TH2D* h2ElecQ2RC_MC    		= (TH2D*)fdata->Get("h2ElecQ2RC_MC");
  TH2D* h2ElecXRC_MC	 		= (TH2D*)fdata->Get("h2ElecXRC_MC");
  TH2D* h2ElecyRC_MC	 		= (TH2D*)fdata->Get("h2ElecyRC_MC");

  TH2D* first_h2EEleRC_MC		= (TH2D*)fdata->Get("first_h2EEleRC_MC");
  TH2D* first_h2EtEleRC_MC		= (TH2D*)fdata->Get("first_h2EtEleRC_MC");
  TH2D* first_hElecThetaRC_MC	= (TH2D*)fdata->Get("first_hElecThetaRC_MC");
  TH2D* first_h2ElecQ2RC_MC		= (TH2D*)fdata->Get("first_h2ElecQ2RC_MC");
  TH2D* first_h2ElecXRC_MC		= (TH2D*)fdata->Get("first_h2ElecXRC_MC");
  TH2D* first_h2ElecyRC_MC		= (TH2D*)fdata->Get("first_h2ElecyRC_MC");

  TH2D* second_h2EEleRC_MC		= (TH2D*)fdata->Get("second_h2EEleRC_MC");
  TH2D* second_h2EtEleRC_MC		= (TH2D*)fdata->Get("second_h2EtEleRC_MC");
  TH2D* second_hElecThetaRC_MC 	= (TH2D*)fdata->Get("second_hElecThetaRC_MC");
  TH2D* second_h2ElecQ2RC_MC 	= (TH2D*)fdata->Get("second_h2ElecQ2RC_MC");
  TH2D* second_h2ElecXRC_MC		= (TH2D*)fdata->Get("second_h2ElecXRC_MC");
  TH2D* second_h2ElecyRC_MC    = (TH2D*)fdata->Get("second_h2ElecyRC_MC");

  std::vector<TH2D*> hists_full  =  { 
                                      h2EEleRC_MC,
                      								h2EtEleRC_MC,
                      								hElecThetaRC_MC,
                      								h2ElecQ2RC_MC,
                      								h2ElecXRC_MC,
                      								h2ElecyRC_MC
                                    };

  std::vector<TH2D*> hists_first =  {
                      								first_h2EEleRC_MC,
                      								first_h2EtEleRC_MC,
                      								first_hElecThetaRC_MC,
                      								first_h2ElecQ2RC_MC,
                      								first_h2ElecXRC_MC,
                      								first_h2ElecyRC_MC

                                    };                             
  std::vector<TH2D*> hists_second =  {
                      								second_h2EEleRC_MC,
                      								second_h2EtEleRC_MC,
                      								second_hElecThetaRC_MC,
                      								second_h2ElecQ2RC_MC,
                      								second_h2ElecXRC_MC,
                                      second_h2ElecyRC_MC
                                     };
  
  //   for(TH2D* hist : hists){
  //  		histPlot(hist);
		// }
  for (int i=0; i<hists_full.size(); i++) histPlot(hists_full[i],hists_first[i],hists_second[i]);
  

}