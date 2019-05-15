
#ifndef LepE_calibr_h
#define LepE_calibr_h

#include "TFile.h"
#include "TH1D.h"
#include "TString.h"

const TString 0506e_mc   = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_Ariadne_Low_Q2_NC_DIS_0506e.list.root";
const TString 0506e_data = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_0506e.list.root";
const TString 0607p_mc   = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_Ariadne_Low_Q2_NC_DIS_0607p.list.root";
const TString 0607p_data = "/nfs/dust/zeus/group/mykytaua/Jets_nfs/2019.05.15/control_plots_clean/root/output_Sample_0607p.list.root";

class Calibrator {
private:
	TH1D* hdataElecE_R; //to avoid additional memory usege
	//TH1D* hmcElecE; 
public:
	Calibrator(TString period)
	{
		if(period == "0405e" || period == "06e" || period == "0506e" || period == "05e"){
			TFile* fdata = new TFile(0506e_data);  
        		TFile* fmc = new TFile(0506e_mc);
		}

		else if(period == "0607p" || period == "06p" || period == "07p"){
			TFile* fdata = new TFile(0607p_data);  
        		TFile* fmc = new TFile(0607p_mc);
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
