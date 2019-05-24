
#ifndef LepE_calibr_h
#define LepE_calibr_h

#include <stdlib.h>
#include <string>
#include "TFile.h"
#include "TH1D.h"
#include "TString.h"

class Calibrator {
private:
	TH1D* hdata_R; //to avoid additional memory usege
	//TH1D* hmcElecE; 
public:
	Calibrator(const Char_t* path_data, const Char_t* path_mc)
	{
		TFile* fdata;
		TFile* fmc;

		fdata = new TFile(path_data);  
        	fmc = new TFile(path_mc);

		// by Lepton Energy
		//hdata_R = (TH1D*)fdata->Get("hElecE");
		//TH1D* hmc   = (TH1D*)fmc->Get("hElecE");
	
		// by Lepton Angle
	        hdata_R = (TH1D*)fdata->Get("hElecPhi");
		TH1D* hmc = (TH1D*)fmc->Get("hElecPhi");	

		// by Jet Angle
	        //hdata_R = (TH1D*)fdata->Get("hJetPhi");
		//TH1D* hmc = (TH1D*)fmc->Get("hJetPhi" );	

		////hmcElecE->Scale(hmcElecE->GetEntries()/hdataElecE_R->GetEntries());
		hdata_R->Divide(hmc);
	}

	//Here, X any variable
	Float_t getWeight(Float_t X)
	{
		int k = hdata_R->GetXaxis()->FindBin(X);
		return hdata_R->GetBinContent(k);
	}

	
};

#endif
