
#ifndef LepE_calibr_h
#define LepE_calibr_h

#include "TFile.h"
#include "TH1D.h"

class Calibrator {
private:
	TH1D* hdataElecE_R; //to avoid additional memory usege
	//TH1D* hmcElecE; 
public:
	Calibrator(const Char_t *datapath, const Char_t *mcpath)
	{
		TFile* fdata = new TFile(datapath);  
        	TFile* fmc = new TFile(mcpath);

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
