#ifndef cuts_h
#define cuts_h

#include <stdlib.h>
#include <string>
#include "TString.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "JetOrange2018.h"

#define q2start 10
#define q2end 350

class JetOrangeSelect : public JetOrange2018 {

public:
JetOrangeSelect(TTree *tree, Bool_t isdata):JetOrange2018(tree,isdata){};

bool CheckCuts_true(TString period){

    /////////////////////Define Cuts --3.1-->
    /////Using only the electron with the highest probability Siq2el[0]
    /////Using only the highest jet in the event: Kt_etajet_b[0]
    /////Phase Space 
    if(Mc_q2<q2start || Mc_q2>q2end) return false;
    /////Cleanning cuts
    //if(Mc_vtx[2]<-40 || Mc_vtx[2]>40 || Zvtx==0)return false;   //Vertex is detector component Achim 19/Jun/2019
    //if(Cal_empz<35 || Cal_empz>65) return false;  
    //Calculate E-pz from zufo
    Float_t Empz = Cal_empz;
    //for(Int_t zloop=0; zloop<Nzufos; zloop++){
     // TLorentzVector v(Zufo[zloop][0], Zufo[zloop][1], 
     // Zufo[zloop][2], Zufo[zloop][3]);
     // Empz += Zufo[zloop][3] - Zufo[zloop][2];
     // }
    //  cout << Form("Calorimeter: %f     Zufo: %f     Diff: %f",Cal_empz,Empz,Cal_empz - Empz) << endl;
    //  if(Empz < 35. || Empz > 65.) return false;   
    if(Mc_y > 0.7)  return false;   //Change from 0.95 to 0.7 , first prel. meeting
    if(Mc_y < 0.04) return false;
    //if(Cal_pt / TMath::Sqrt(Cal_et) > 2.5) return false;
    /////Electron cuts
    TLorentzVector lepton_final;
    lepton_final.SetPxPyPzE( Mc_pfsl[0], Mc_pfsl[1], Mc_pfsl[2], Mc_pfsl[3]);
    if(lepton_final.E() < 10 ) return false;  //Remove the upper energy cut
    if(lepton_final.Theta()*180.0/TMath::Pi() < 140 || lepton_final.Theta()*180.0/TMath::Pi() > 180.0) return false;
    //if(Sipos[0][2] < -148. && Sipos[0][0] > -14. 
    //   && Sipos[0][0] < 12. && Sipos[0][1] > 90.) return false;                //Chimney cut
    //if(sqrt(Sipos[0][0]*Sipos[0][0] + Sipos[0][1]*Sipos[0][1]) < 20.0) return false;
    //if (Sienin[0] > 0.1*(Siein[0] + Sienin[0]) ) return false; // Energy in cone                                                                                        
    /////Triggers
    //if(period == "0405e" && !(Tltw[2] & (1 << 1)) ) return false;      //SPP02
    //if(period == "06e"   && !(Tltw[2] & (1 << 8)) ) return false;      //SPP09
    //if(period == "0607p" && !(Tltw[2] & (1 << 8)) ) return false;      //SPP09
    
    /////Jet selection
    Int_t highEt_jet = 0;
    if(Nhmjets <= 0) return false;
    TLorentzVector jet_initial, jet_compare, jet_final;
    jet_initial.SetPxPyPzE( 0,0,0,0);
    for(int high = 0; high < Nhmjets; high++){
     
      jet_compare.SetPxPyPzE( Pxhmjet[high], Pyhmjet[high], 
            Pzhmjet[high],  Ehmjet[high]);
      if(jet_compare.Et() > jet_initial.Et()) {
  highEt_jet = high;
  jet_initial.SetPxPyPzE(Pxhmjet[high], Pyhmjet[high], 
             Pzhmjet[high],  Ehmjet[high]);
      }
    }
    jet_final.SetPxPyPzE( Pxhmjet[highEt_jet], Pyhmjet[highEt_jet], 
        Pzhmjet[highEt_jet],  Ehmjet[highEt_jet]);
    //cout << "The maximum E_T jet is: " << highEt_jet << endl;
    // if(highEt_jet != 0) nothighEt++;
    if(TMath::Abs(jet_final.Eta()) > 1.0 ) return false;
    //if(Kt_etajet_b[0]<-1.5 || Kt_etajet_b[0]>1.8) return false;
    if(jet_final.E() < 2.5) return false;
    if(jet_final.Pt() > 30) return false;
    /////////////////////END Define Cuts <--3.1--

    return true;
}

bool CheckCuts_reco(TString period){
    /////////////////////Define Cuts --3.1-->
    /////Using only the electron with the highest probability Siq2el[0]
    /////Using only the highest jet in the event: Kt_etajet_b[0]
    /////Phase Space 
    if(Siq2el[0]<q2start || Siq2el[0]>q2end) return false;
    /////Cleanning cuts
    if(Zvtx<-40 || Zvtx>40 || Zvtx==0)    return false;
    if(Cal_empz<35 || Cal_empz>65) return false;  
    //Calculate E-pz from zufo
    Float_t Empz = 0;
    for(Int_t zloop=0; zloop<Nzufos; zloop++){
      std::cout<<Nzufos<<"\n";
      TLorentzVector v(Zufo[zloop][0], Zufo[zloop][1], 
           Zufo[zloop][2], Zufo[zloop][3]);
      Empz += Zufo[zloop][3] - Zufo[zloop][2];
    }
    //  cout << Form("Calorimeter: %f     Zufo: %f     Diff: %f",Cal_empz,Empz,Cal_empz - Empz) << endl;
    if(Empz < 35. || Empz > 65.) return false;   
    if(Siyel[0] > 0.7) return false;  //Change from 0.95 to 0.7 for version 7 of this code
    if(Siyjb[0] < 0.04) return false;
    if(Cal_pt / TMath::Sqrt(Cal_et) > 2.5) return false;
    /////Electron cuts
    //if(Siecorr[0][2] < 10 || Siecorr[0][2] > 25) return false;   //Do not use high limit cut
    if(Siecorr[0][2] < 10) return false;
    if(Sith[0]*180.0/TMath::Pi() < 140 || Sith[0]*180.0/TMath::Pi() > 180.0) return false;  ///Need to reduce 140 to 60 (future)
    if(Sipos[0][2] < -148. && Sipos[0][0] > -14. 
       && Sipos[0][0] < 12. && Sipos[0][1] > 90.) return false;                //Chimney cut
    if(sqrt(Sipos[0][0]*Sipos[0][0] + Sipos[0][1]*Sipos[0][1]) < 20.0) return false;
    if (Sienin[0] > 0.1*(Siein[0] + Sienin[0]) ) return false; // Energy in cone
    if(Siprob[0] < 0.9) return false;  
    /////Triggers
    if(period == "0405e" && !(Tltw[2] & (1 << 1)) ) return false;      //SPP02
    if(period == "06e"   && !(Tltw[2] & (1 << 8)) ) return false;      //SPP09
    if(period == "0607p" && !(Tltw[2] & (1 << 8)) ) return false;      //SPP09
    /////Jet selection
    if(Kt_njet_b <= 0) return false;
    if(TMath::Abs(Kt_etajet_b[0]) > 1.0 ) return false;
    //if(Kt_etajet_b[0]<-1.5 || Kt_etajet_b[0]>1.8) return false;  //Use this in the future
    if(Kt_etjet_b[0] < 2.5) return false;
    Float_t JetPt = sqrt(Kt_etjet_b[0]*Kt_etjet_b[0]-Kt_masjet_b[0]*Kt_masjet_b[0]) ;
    if(JetPt > 30) return false;
    /////////////////////END Define Cuts <--3.1--

    return true;
}

bool CheckCuts(TString period){

    	 //this->GetEntry(jentry);	  
    	 
	 ////////////://///////Define Cuts --3.1-->
    	 /////Using only the electron with the highest probability Siq2el[0]
    	 /////Using only the highest jet in the event: Kt_etajet_b[0]
    	
	 /////Phase Space 
    	    if(Siq2el[0]<q2start || Siq2el[0]>q2end) return false;
    	 //if(Siq2el[0]<10) return false;
         
	 /////Cleanning cuts
          if(Zvtx<-40 || Zvtx>40 || Zvtx==0)    return false;
          if(Cal_empz<35 || Cal_empz>65) return false;  
          
	  //Calculate E-pz from zufo
          Float_t Empz = 0.;
          for(Int_t zloop=0; zloop<Nzufos; zloop++){
            //TLorentzVector v(Zufo[zloop][0], Zufo[zloop][1], 
            //  	       Zufo[zloop][2], Zufo[zloop][3]);
            Empz += Zufo[zloop][3] - Zufo[zloop][2];
          }
          //cout << Form("Calorimeter: %f     Zufo: %f     Diff: %f",JetOrange->Cal_empz,Empz,JetOrange->Cal_empz - Empz) << endl;
          if(Empz < 35. || Empz > 65.) return false;
          if(Siyel[0] > 0.95) return false;
          if(Siyjb[0] < 0.04) return false;
          if(Cal_pt / TMath::Sqrt(Cal_et) > 2.5) return false;
          
	  /////Electron cuts
          if(Siecorr[0][2] < 10) return false;
          if(Sith[0]*180.0/TMath::Pi() < 140 || Sith[0]*180.0/TMath::Pi() > 180.0) return false;
          if(Sipos[0][2] < -148. && Sipos[0][0] > -14. 
             && Sipos[0][0] < 12. && Sipos[0][1] > 90.) return false;                //Chimney cut
          if(sqrt(Sipos[0][0]*Sipos[0][0] + Sipos[0][1]*Sipos[0][1]) < 20.0) return false;
          
	  /////Triggers
          if(period == "0405e" && !(Tltw[2] & (1 << 1)) ) return false;      //SPP02
          if(period == "06e"   && !(Tltw[2] & (1 << 8)) ) return false;      //SPP09
          if(period == "0607p" && !(Tltw[2] & (1 << 8)) ) return false;      //SPP09
          
	  /////Jet selection
          if(Kt_njet_b <= 0) return false;
          if(Kt_etajet_b[0]<-1.0 || Kt_etajet_b[0]>1.0) return false;
          if(Kt_etjet_b[0] < 2.5) return false;
          
          /////////////////////END Define Cuts <--3.1--
          
          // Addition DIS cuts
          Bool_t dis_cuts=true;
          
	  // Energy in cone
          if (Sienin[0] < 0.1*(Siein[0] + Sienin[0]));
          else dis_cuts=false;

          // Lepton probability
          if (Siprob[0] > 0.9);
          else dis_cuts=false;
          
	  // Lepton track 
	  // if (JetOrange->Sidca[0] < 20. && JetOrange->Sidca[0] >= 0.);
	  // else {TVector3 leptonPosition(JetOrange->Sipos[0][0], JetOrange->Sipos[0][1], JetOrange->Sipos[0][2]);
          // if (leptonPosition.Theta() < 0.4 || leptonPosition.Theta() > 2.721);
	  // else dis_cuts=false;}

          if(dis_cuts==false) return false;

          return true;
  }
};

#endif
