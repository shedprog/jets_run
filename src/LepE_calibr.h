
#ifndef LepE_calibr_h
#define LepE_calibr_h

#include <stdlib.h>
#include <string>
#include "TFile.h"
#include "TH1D.h"
#include "TString.h"

TString path0506e_mc   = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_Ariadne_Low_Q2_NC_DIS_0506e.list.root";
TString path0506e_data = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_0506e.list.root";
TString path0607p_mc   = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_Ariadne_Low_Q2_NC_DIS_0607p.list.root";
TString path0607p_data = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_0607p.list.root";

class Calibrator {
private:
	TH1D* hdataElecE_R; //to avoid additional memory usege
	//TH1D* hmcElecE; 
public:
	Calibrator(TString period)
	{
		TFile* fdata;
		TFile* fmc;

		if(period == "0405e" || period == "06e" || period == "0506e" || period == "05e"){
			fdata = new TFile(path0506e_data);  
        		fmc = new TFile(path0506e_mc);
		}

		else if(period == "0607p" || period == "06p" || period == "07p"){
			fdata = new TFile(path0607p_data);  
        		fmc = new TFile(path0607p_mc);
		}
		else{
			std::cout<<"No such period!"<<"\n";
		}
		
		hdataElecE_R = (TH1D*)fdata->Get("hElecE");
		TH1D* hmcElecE   = (TH1D*)fmc->Get("hElecE");
		////hmcElecE->Scale(hmcElecE->GetEntries()/hdataElecE_R->GetEntries());
		hdataElecE_R->Divide(hmcElecE);
	}

	Float_t getWeight(Float_t leptonE)
	{
		int k = hdataElecE_R->GetXaxis()->FindBin(leptonE);
		return hdataElecE_R->GetBinContent(k);
	}

	
};

#endif
