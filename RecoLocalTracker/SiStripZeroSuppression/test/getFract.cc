#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
#include <cmath>

#include "TNtuple.h"
#include "TROOT.h"
#include "TString.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TF2.h"
#include "TProfile.h"
#include "TCanvas.h"
#include "TFitResult.h"
#include "TPaveText.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TObject.h"
#include "TObjArray.h"
#include "THStack.h"
#include "TStyle.h"
#include "TROOT.h"
#include "THashList.h"
#include "TApplication.h"
#include "TGraph.h"
#include "TMath.h"
#include "TLegend.h"

using namespace std;

int main(int argc, char *argv[]){

       if(argc != 2)
        throw std::runtime_error("Bad number of arguments!");

    TString inputFle = argv[1];

TFile *f = new TFile(inputFle);

  //f->ls();
  
 //vector<TString> names = {"ChHIPPerMod", "ChPOSTPerMod","ChFakePerMod","ChCollPerMod", "MultHIPPerMod", "MultPOSTPerMod","MultFakePerMod","MultCollPerMod", "WiHIPPerMod", "WiPOSTPerMod","WiFakePerMod","WiCollPerMod", "nrOfClustersWiderThan10PerModuleHIP", "nrOfClustersWiderThan10PerModulePOST", "nrOfClustersWiderThan10PerModuleCOLL","nrOfClustersWiderThan10PerModuleFAKE","f_HIP" };
 vector<TString> names = {"f_HIP" };

for(uint32_t v=0; v<names.size(); v++)
{
 
 TH1F * h1 = (TH1F*)f->Get(names.at(v));
 uint32_t count = 0;
 float sumx = 0;
 for(uint32_t i=0; i<h1->GetNbinsX(); i++)
 {
     float x = h1->GetBinContent(i);
     if(x!=0)
     {
         sumx +=x;
         count++;
     }
 }
 cout << h1->GetName() << " " << (float) sumx/count << " sum " << sumx << " count " << count << endl;

}
}
