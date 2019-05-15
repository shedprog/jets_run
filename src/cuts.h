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
JetOrangeSelect():JetOrange2018(){};

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
