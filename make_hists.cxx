//selector class inherent JetOrange2018_cxx
#include "./src/selector.h"

void make_hists(const Char_t *eachfile= "~/Desktop/zeusmc.hfix627.h1391.0607p.q4.ari_2911.root ", 
	  const Char_t *outdir="./")
{

  //gROOT->cd();
  TDatime now;                                          //Set time in Root
  now.Print();
  //gErrorIgnoreLevel=kError;       //This gets rid of the warnings
  gErrorIgnoreLevel=kFatal;       //This gets rid of the errors
  //There are some unused branched for data or MC

  /////////////////////Read all files --1-->
  TFile f(eachfile); if (f.IsZombie())  {std::cout<< "Problems with file: " << eachfile << std::endl; return;}
  std::string eachfile2 = eachfile;
  size_t found = eachfile2.find("/data_");
  Bool_t isdata = found != std::string::npos ? true : false;
  Int_t runnumber = 0;
  Char_t runnumber_size[10];
  Char_t sample[50];
  Float_t Weight = 1.0;
  if(isdata){
    std::cout << "Data" << std::endl;
    sscanf(&eachfile2[found],"/data_%[^_]_%d_%[^.].root",sample,&runnumber,runnumber_size);
    std::cout<< "Sample: " << sample << std::endl;
    std::cout << "Run Number: " << runnumber << std::endl;
    //cout<< size << endl;
  } else{
    std::cout << "MC" << std::endl;
    found = eachfile2.find("/zeusmc.");
    if (found == std::string::npos) {std::cout << "Problem finding the MC file" << std::endl; return;}
    sscanf(&eachfile2[found],"/zeusmc.%s",sample);
    std::cout << "Sample MC: " << sample << std::endl;
    // Weight = CalcWeight(sample);
    // cout << "Weight   : " << Weight << endl; 
  }

  TTree *firstJet = (TTree*)f.Get("orange");
  if (!firstJet) {std::cout<< "Problems with file: " << eachfile << std::endl; return;}
  
  ControlPlot::Selector *JetOrange = new ControlPlot::Selector();
  //Plot2D::Selector *JetOrange = new Plot2D::Selector();
  
  JetOrange->Init(firstJet);
  JetOrange->SetData(isdata);
  JetOrange->InitHists();
  
  TString period;
  if(runnumber>=47010 && runnumber <=51245)  period = "04p";
  if(runnumber>=52258 && runnumber <=57123)  period = "0405e";
  if(runnumber>=58207 && runnumber <=59947)  period = "06e";
  if(runnumber>=60005 && runnumber <=62639) period = "0607p";
  if(!isdata) period=sample;

  ///////Start filling the histograms from the trees  --3-->
  Int_t count = 0;
  Long64_t nentries = firstJet->GetEntriesFast();
  std::cout << "Number of events before the cuts: " << nentries << std::endl;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    if(JetOrange->CheckCuts(jentry,period)) continue;
    JetOrange->FillHists(isdata);
    count++;
  }

  std::cout << "Number of events after the cuts : " << count << std::endl;
  ///END Start filling the histograms from the trees  <--3--

  JetOrange->WriteHists(outdir,period,runnumber_size,sample,isdata);

  TDatime now1;
  now1.Print(); 
}
