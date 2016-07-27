#include <fstream>
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

int main(int argc, char *argv[])
{

       if(argc != 2)
        throw std::runtime_error("Bad number of arguments!");

    TString inputFle = argv[1];

    ofstream myfile;
    myfile.open ("plot_log.txt");

    ofstream evfile;
    evfile.open ("hip_events_l1.txt");

    TFile *f = new TFile(inputFle);
    // baselineAna
    TTree *t1 = dynamic_cast<TTree*>(f->Get("sEventTree"));

    //unsigned int  moduleKeyOUT = NULL;
    unsigned int  moduleValueOUT = NULL;
    vector< vector<float> > *baselinesMtxOUT = NULL;
    vector< vector<uint16_t> > *chargeOfClustersMtxOUT = NULL;
    vector< vector<uint16_t> > *centerOfClustersMtxOUT = NULL;
    vector< vector<uint16_t> > *stripsTouchedOfClustersMtxOUT = NULL;
    vector< vector<uint8_t> > *maxADCOfClustersMtxOUT = NULL;
    vector< vector<float> > *baselineRMSMtxOUT = NULL;
    vector< vector<float> > *baselineCMRMSMtxOUT = NULL;
    vector< vector<float> > *PSSlope20MtxOUT = NULL;
    vector< vector<uint8_t> > *positiveValuesCMNSMtxOUT = NULL;
    vector< vector<uint8_t> > *negativeValuesPSMtxOUT = NULL;
    vector<ULong64_t> *timesTOUT = NULL;
    vector<uint32_t> *bunchNrsOUT = NULL;
    vector<uint32_t> *eventNrsOUT = NULL;
 

    //t1->SetBranchAddress("moduleKeyOUT",&moduleKeyOUT);
    t1->SetBranchAddress("moduleValueOUT",&moduleValueOUT);
    t1->SetBranchAddress("baselinesMtxOUT",&baselinesMtxOUT);
    t1->SetBranchAddress("chargeOfClustersMtxOUT",&chargeOfClustersMtxOUT);
    t1->SetBranchAddress("centerOfClustersMtxOUT",&centerOfClustersMtxOUT);
    t1->SetBranchAddress("stripsTouchedOfClustersMtxOUT",&stripsTouchedOfClustersMtxOUT);
    t1->SetBranchAddress("maxADCOfClustersMtxOUT",&maxADCOfClustersMtxOUT);
    t1->SetBranchAddress("baselineRMSMtxOUT",&baselineRMSMtxOUT);
    t1->SetBranchAddress("timesTOUT",&timesTOUT);
    t1->SetBranchAddress("bunchNrsOUT",&bunchNrsOUT);
    t1->SetBranchAddress("eventNrsOUT",&eventNrsOUT);
    t1->SetBranchAddress("positiveValuesCMNSMtxOUT", &positiveValuesCMNSMtxOUT);
    t1->SetBranchAddress("negativeValuesPSMtxOUT", &negativeValuesPSMtxOUT);
    t1->SetBranchAddress("baselineCMRMSMtxOUT", &baselineCMRMSMtxOUT);
    t1->SetBranchAddress("PSSlope20MtxOUT", &PSSlope20MtxOUT);
 
    //other variables
    vector<ULong64_t> timeSorted;
    vector<Long64_t> timeSortedDiff;
    std::map<ULong64_t,uint32_t> timeMap;
    std::map<ULong64_t,uint32_t> timeMapDiff;
    std::map< uint32_t, uint32_t > detIdMap; //keep this
    //TCanvas* c;

    TH1::SetDefaultSumw2();    

    struct histo 
    {
        TH1D* baselineHist;
        TH1D* clusHist;
    } histos;

    vector< TH1F > moduleHIP;
    TH1F oneHIP;
    vector< TH1F > moduleHIP2;
    TH1F oneHIP2;

    vector<TH1F> moduleCHRG;
    vector <TH1F> moduleMULT;
    TH1F clusCH;
    TH1F clusMult;
    vector<TH1F> maxmoduleCHRG;
    vector <TH1F> maxmoduleMULT;
    TH1F maxclusCH;
    TH1F maxclusMult;
    vector <TH1F> cleanedmoduleMULT;
    vector<TH1F> cleanedmaxmoduleCHRG;
    vector<TH1F> cleanedmoduleCHRG;
    TH1F cleanedclusMult;
    TH1F cleanedmaxclusCH;
    TH1F cleanedclusCH;

    uint8_t lTrain = 70;
    uint16_t trainOffset = 716;
    uint32_t actMod=0;
    //TH1F* blBefore = new TH1F("blBefore", "blBefore", lTrain,-lTrain,0);

     //TH1F* SATvsNSATclusHIP = new TH1F("SAT vs NSAT", "SAT vs NSAT", 2, 0, 2);

     TH1F* maxCHHIP = new TH1F("max CH HIP", "max CH HIP", 300, 0, 300);

     TH1F* CHdistrPRE = new TH1F("CH PRE", "CH PRE", 5000, 0, 10000);
     TH1F* CHdistrHIP = new TH1F("CH HIP", "CH HIP", 5000, 0, 10000);
     TH1F* CHdistrPOST = new TH1F("CH POST", "CH POST", 5000, 0, 10000);

     TH1F* CHdistrSTlow = new TH1F("CH ST low", "CH ST low", 5000, 0, 10000);
     TH1F* CHdistrSTmed = new TH1F("CH ST med", "CH ST med", 5000, 0, 10000);
     TH1F* CHdistrSThig = new TH1F("CH ST hig", "CH ST hig", 5000, 0, 10000);


     TH1F* oneClusCHHIP = new TH1F("oneClusCHHIP", "oneClusCHHIP", 1000, 0, 10000);
     TH1F* twoClusCHHIP = new TH1F("twoClusCHHIP", "twoClusCHHIP", 1000, 0, 10000);

     TH1F* CHBXmin3 = new TH1F("CHBXmin3", "CHBXmin3", 1000, 0, 10000);
     TH1F* CHBXmin25 = new TH1F("CHbxmin25", "CHbxmin25", 1000, 0, 10000);
     CHBXmin3->SetTitle("");
     CHBXmin3->GetYaxis()->SetTitle("count");
     CHBXmin3->GetXaxis()->SetTitle("charge[ADC]");
     CHBXmin25->SetTitle("");
     CHBXmin25->GetYaxis()->SetTitle("count");
     CHBXmin25->GetXaxis()->SetTitle("charge[ADC]");
     
     TH1F* multBXmin3 = new TH1F("multBXmin3", "multBXmin3", 20, 0, 20);
     TH1F* multBXmin10 = new TH1F("multBXmin3", "multBXmin3", 20, 0, 20);

     TH1F* RMSupTO2p5POST = new TH1F("RMSupTO2p5POST", "RMSupTO2p5POST", 2*lTrain, -lTrain, lTrain);
     TH1F* RMSmoreTHAN2p5POST = new TH1F("RMSmoreTHAN2p5POST", "RMSmoreTHAN2p5POST", 2*lTrain, -lTrain, lTrain);
     TH1F* RMSrestPOST = new TH1F("RMSrestPOST", "RMSmoreTHAN2p5POST", 2*lTrain, -lTrain, lTrain);
     TH1F* RMSdivisionPOST = new TH1F("RMSdivisionPOST", " RMSdivisionPOST", 2*lTrain,-lTrain, lTrain);
     RMSdivisionPOST->SetTitle("");
     RMSdivisionPOST->GetYaxis()->SetTitle("RMS<2.5/RMS<4");
     RMSdivisionPOST->GetXaxis()->SetTitle("bx");

     TH1F* RMSdivisionallPOST = new TH1F("RMSdivisionallPOST", " RMSdivisionallPOST", 2*lTrain,-lTrain, lTrain);

     TH1F* saturCH = new TH1F("saturCH", "saturCH", 2*lTrain, -lTrain, lTrain);
     TH1F* nonsaturCH = new TH1F("nonsaturCH", "nonsaturCH", 2*lTrain, -lTrain, lTrain);
     TH1F* saturation = new TH1F("saturation", "saturation", 2*lTrain, -lTrain, lTrain);
     saturation->SetTitle("");
     saturation->GetYaxis()->SetTitle("fraction of saturated clusters");
     saturation->GetXaxis()->SetTitle("bx");

     TH1F* chargeUnc = new TH1F("normal clus average charge", "normal clus average charge", 5000, 0, 10000);
     chargeUnc->SetTitle("");
     chargeUnc->GetYaxis()->SetTitle("count");
     chargeUnc->GetXaxis()->SetTitle("charge[ADC]");
     TH1F* chargeUnc2 = new TH1F("first event clus charge", "first event clus charge", 5000, 0, 10000);
     chargeUnc2->SetTitle("");
     chargeUnc2->GetYaxis()->SetTitle("count");
     chargeUnc2->GetXaxis()->SetTitle("charge[ADC]");
     TH1F* chargeUnc3 = new TH1F("other events charge", "other events charge", 5000, 0, 10000);
     chargeUnc3->SetTitle("");
     chargeUnc3->GetYaxis()->SetTitle("count");
     chargeUnc3->GetXaxis()->SetTitle("charge[ADC]");
     TH1F* multUnc = new TH1F("mu", "mu", 1000, 0, 100);
     TH1F* chargePOST = new TH1F("HIP clus charge", "HIP clus charge", 5000, 0, 10000);
     chargePOST->SetTitle("");
     chargePOST->GetYaxis()->SetTitle("count");
     chargePOST->GetXaxis()->SetTitle("charge[ADC]");

     TH1F* HIPf =  new TH1F("f_HIP", "f_HIP", 15000, 0, 15000);
     HIPf->SetTitle("");
     HIPf->GetYaxis()->SetTitle("fraction");
     HIPf->GetXaxis()->SetTitle("module");
     TH1F* boffD =  new TH1F("boffD", "boffD", 15000, 0, 15000);
     TH1F* boffU =  new TH1F("boffU", "boffU", 15000, 0, 15000);
     TH1F* nHIP =  new TH1F("nHIP", "nHIP", 15000, 0, 15000);
     TH1F* nHIP2 =  new TH1F("nHIP2", "nHIP2", 15000, 0, 15000);
     TH1F* fPeak =  new TH1F("fPeak", "fPeak", 15000, 0, 15000);
     TH1F* fPeakBefore =  new TH1F("fPeakBefore", "fPeakBefore", 15000, 0, 15000);
     TH1F* satBefore =  new TH1F("sat", "sat", 6, -6, 0);
     TH2F* baseline2D = new TH2F("baseline 2D", "baseline 2D", 2*lTrain, -lTrain, lTrain, 700  , -200, 500 );
     baseline2D->SetTitle("");
     baseline2D->GetYaxis()->SetTitle("baseline[ADC]");
     baseline2D->GetXaxis()->SetTitle("bunch crossing");


     TH1F blPerMod;
     TH1F* meanBlPerMod =  new TH1F("meanBlPerMod", "meanBlPerMod", 15000, 0, 15000);
     TH2F* meanBlPerModVsfHIP = new TH2F("meanBlPerModVsfHIP", "meanBlPerModVsfHIP", 700, -200, 500, 200  , 0, 0.015 );
     TH1F blPerModHIP;
     TH1F* meanBlPerModHIP =  new TH1F("meanBlPerModHIP", "meanBlPerModHIP", 15000, 0, 15000);
     meanBlPerModHIP->SetTitle("");
     meanBlPerModHIP->GetYaxis()->SetTitle("mean baseline for BX=0 [ADC]");
     meanBlPerModHIP->GetXaxis()->SetTitle("module");
     meanBlPerMod->SetTitle("");
     meanBlPerMod->GetYaxis()->SetTitle("mean baseline[ADC]");
     meanBlPerMod->GetXaxis()->SetTitle("module");

     TH2F* meanBlPerModVsfHIPHIP = new TH2F("meanBlPerModVsfHIPHIP", "meanBlPerModVsfHIPHIP", 700, -200, 500, 200  , 0, 0.015 );


     //2D plots + cor pfofile ///////////////////////////////////////////////////////////////////////////////
     TH2F* baselineVsRMSall = new TH2F("baselineVsRMSall", "baselineVsRMSall", 700, -200, 500, 200  , 0, 50 );
     TProfile* PbaselineVsRMSall = new TProfile("PbaselineVsRMSall", "PbaselineVsRMSall", 700, -200, 500, "S" );
     baselineVsRMSall ->SetTitle("");
     baselineVsRMSall ->GetXaxis()->SetTitle("baseline[ADC]");
     baselineVsRMSall ->GetYaxis()->SetTitle("RMS of raw digis");
     PbaselineVsRMSall ->SetTitle("");
     PbaselineVsRMSall ->GetXaxis()->SetTitle("baseline[ADC]");
     PbaselineVsRMSall ->GetYaxis()->SetTitle("RMS of raw digis");

     vector<TH2F*>baselineVsRMSallPerMod;

     TH2F* baselineVsRMSPRE = new TH2F("baselineVsRMSPRE", "baselineVsRMSPRE", 700, -200, 500, 200  , 0, 50 );
     TProfile* PbaselineVsRMSPRE = new TProfile("PbaselineVsRMSPRE", "PbaselineVsRMSPRE", 700, -200, 500, "S");

     TH2F* baselineVsRMSHIP = new TH2F("baselineVsRMSHIP", "baselineVsRMSHIP", 700, -200, 500, 200  , 0, 50 );
     TProfile* PbaselineVsRMSHIP = new TProfile("PbaselineVsRMSHIP", "PbaselineVsRMSHIP", 700, -200, 500, "S" );

     TH2F* baselineVsRMSPOST = new TH2F("baselineVsRMSPOST", "baselineVsRMSPOST", 700, -200, 500, 200  , 0, 50 );
     TProfile* PbaselineVsRMSPOST = new TProfile("PbaselineVsRMSPOST", "PbaselineVsRMSPOST", 700, -200, 500, "S");

     TH2F* BXvsRMS = new TH2F("BXvsRMS", "BXvsRMS", 2*lTrain, -lTrain, lTrain, 100  , 0, 100 );
     TProfile* PBXvsRMS = new TProfile("PBXvsRMS", "PBXvsRMS", 2*lTrain, -lTrain, lTrain, "S" );
     BXvsRMS->SetTitle("");
     BXvsRMS->GetYaxis()->SetTitle("RMS of raw digis");
     BXvsRMS->GetXaxis()->SetTitle("BX");
     PBXvsRMS->SetTitle("");
     PBXvsRMS->GetYaxis()->SetTitle("RMS of raw digis");
     PBXvsRMS->GetXaxis()->SetTitle("BX");


     TH2F* BXvsRMSCHl600 = new TH2F("BXvsRMSCHl600", "BXvsRMSCHl600", 2*lTrain, -lTrain, lTrain, 100  , 0, 50 );
     TProfile* PBXvsRMSCHl600 = new TProfile("BXvsRMSCHl600", "BXvsRMSCHl600", 2*lTrain, -lTrain, lTrain, "S" );

     TH2F* BXvsRMSCHm600 = new TH2F("BXvsRMSCHm600", "BXvsRMSCHm600", 2*lTrain, -lTrain, lTrain, 100  , 0, 50 );
     TProfile* PBXvsRMSCHm600 = new TProfile("BXvsRMSCHm600", "BXvsRMSCHm600", 2*lTrain, -lTrain, lTrain, "S" );

     TH2F* RMSHIPvsRMSPOST = new TH2F("RMSHIPvsRMSPOST", "RMSHIPvsRMSPOST",  100  , 0, 50,100  , 0, 50 );
     TProfile* PRMSHIPvsRMSPOST = new TProfile("RMSHIPvsRMSPOST", "RMSHIPvsRMSPOST", 100  , 0, 50, "S" );
     
     TH2F* BXvsRMSbot = new TH2F("BXvsRMSbot", "BXvsRMSbot", 2*lTrain, -lTrain, lTrain, 100  , 0, 20 );
     TProfile* PBXvsRMSbot = new TProfile("PBXvsRMSbot", "PBXvsRMSbot", 2*lTrain, -lTrain, lTrain, "S" );
     BXvsRMSbot->SetTitle("");
     BXvsRMSbot->GetYaxis()->SetTitle("RMS after pedestal and baseline subtraction");
     BXvsRMSbot->GetXaxis()->SetTitle("BX");
     PBXvsRMSbot->SetTitle("");
     PBXvsRMSbot->GetYaxis()->SetTitle("RMS after pedestal and baseline subtraction");
     PBXvsRMSbot->GetXaxis()->SetTitle("BX");

     TH1F* RMSall =  new TH1F("RMSall", "RMSall", 200, 0, 50);
     RMSall->SetTitle("");
     RMSall->GetXaxis()->SetTitle("RMS of raw digis");
     RMSall->GetYaxis()->SetTitle("counts");
     TH1F* RMS =  new TH1F("RMS", "RMS", 200, 0, 50);

     TH1F* stripsBZps =  new TH1F("stripsBZps", "stripsBZps", 128, 0, 128);
     TH1F* stripsAZcmns =  new TH1F("stripsAZcmns", "stripsAZcmns", 128, 0, 128);
     TH2F* BXvsBZ = new TH2F("BXvsBZ", "BXvsBZ", 2*lTrain, -lTrain, lTrain, 128  , 0, 128 );
     TProfile* PBXvsBZ = new TProfile("PBXvsBZ", "PBXvsBZ", 2*lTrain, -lTrain, lTrain );
     BXvsBZ->SetTitle("");
     BXvsBZ->GetXaxis()->SetTitle("bx");
     BXvsBZ->GetYaxis()->SetTitle("strips below zero after pedestal subtraction");
     PBXvsBZ->SetTitle("");
     PBXvsBZ->GetXaxis()->SetTitle("bx");
     PBXvsBZ->GetYaxis()->SetTitle("strips below zero after pedestal subtraction");
     TH2F* BXvsAZ = new TH2F("BXvsAZ", "BXvsAZ", 2*lTrain, -lTrain, lTrain, 128  , 0, 128 );
     TProfile* PBXvsAZ = new TProfile("PBXvsAZ", "PBXvsAZ", 2*lTrain, -lTrain, lTrain );


     TH2F* BXvsCH = new TH2F("BXvsCH", "BXvsCH", 2*lTrain, -lTrain, lTrain, 10000  , 0, 20000 );
     TProfile* PBXvsCH = new TProfile("PBXvsCH", "PBXvsCH", 2*lTrain, -lTrain, lTrain );
     BXvsCH->GetYaxis()->SetTitle("charge[ADC]");
     BXvsCH->GetXaxis()->SetTitle("bunch crossing");
     PBXvsCH->GetYaxis()->SetTitle("charge[ADC]");
     PBXvsCH->GetXaxis()->SetTitle("bunch crossing");

     TH2F* BXvsCH2 = new TH2F("BXvsCH2", "BXvsCH2", 2*lTrain, -lTrain, lTrain, 10000  , 0, 20000 );
     TProfile* PBXvsCH2 = new TProfile("PBXvsCH2", "PBXvsCH2", 2*lTrain, -lTrain, lTrain );
     BXvsCH2->GetYaxis()->SetTitle("charge[ADC]");
     BXvsCH2->GetXaxis()->SetTitle("bunch crossing");
     PBXvsCH2->GetYaxis()->SetTitle("charge[ADC]");
     PBXvsCH2->GetXaxis()->SetTitle("bunch crossing");

     TH2F* cleanedBXvsCH2 = new TH2F("cleanedBXvsCH2", "cleanedBXvsCH2", 2*lTrain, -lTrain, lTrain, 10000  , 0, 20000 );
     TProfile* cleanedPBXvsCH2 = new TProfile("cleanedPBXvsCH2", "cleanedPBXvsCH2", 2*lTrain, -lTrain, lTrain );
     cleanedBXvsCH2->GetYaxis()->SetTitle("charge[ADC]");
     cleanedBXvsCH2->GetXaxis()->SetTitle("bunch crossing");
     cleanedPBXvsCH2->GetYaxis()->SetTitle("charge[ADC]");
     cleanedPBXvsCH2->GetXaxis()->SetTitle("bunch crossing");

     TH2F* BXvsCHsel1 = new TH2F("BXvsCHsel1", "BXvsCHsel1", 2*lTrain, -lTrain, lTrain, 10000  , 0, 20000 );
     TProfile* PBXvsCHsel1 = new TProfile("PBXvsCHsel1", "PBXvsCHsel1", 2*lTrain, -lTrain, lTrain );
     BXvsCHsel1->GetYaxis()->SetTitle("charge[ADC]");
     BXvsCHsel1->GetXaxis()->SetTitle("bunch crossing");
     PBXvsCHsel1->GetYaxis()->SetTitle("charge[ADC]");
     PBXvsCHsel1->GetXaxis()->SetTitle("bunch crossing");


     TH2F* BXvsCHsel2 = new TH2F("BXvsCHsel2", "BXvsCHsel2", 2*lTrain, -lTrain, lTrain, 10000  , 0, 20000 );
     TProfile* PBXvsCHsel2 = new TProfile("PBXvsCHsel2", "PBXvsCHsel2", 2*lTrain, -lTrain, lTrain );
     BXvsCHsel2->GetYaxis()->SetTitle("charge[ADC]");
     BXvsCHsel2->GetXaxis()->SetTitle("bunch crossing");
     PBXvsCHsel2->GetYaxis()->SetTitle("charge[ADC]");
     PBXvsCHsel2->GetXaxis()->SetTitle("bunch crossing");

     TH2F* BXvsCHall = new TH2F("BXvsCHall", "BXvsCHall", 100, 310, 410, 10000  , 0, 20000 );
     TProfile* PBXvsCHall = new TProfile("PBXvsCHall", "PBXvsCHall", 100, 310, 410 );
     BXvsCHall->GetYaxis()->SetTitle("charge[ADC]");
     BXvsCHall->GetXaxis()->SetTitle("bunch crossing");
     PBXvsCHall->GetYaxis()->SetTitle("charge[ADC]");
     PBXvsCHall->GetXaxis()->SetTitle("bunch crossing");

     TH2F* BXvsCHall2 = new TH2F("BXvsCHall2", "BXvsCHall2", 100, 310, 410, 10000  , 0, 20000 );
     TProfile* PBXvsCHall2 = new TProfile("PBXvsCHall2", "PBXvsCHall2", 100, 310, 410 );
     
     TH2F* baselineVsMaxCHPRE = new TH2F("baselineVsMaxCHPRE", "baselineVsMaxCHPRE", 700, -200, 500, 1000  , 0, 300 );
     TProfile* PbaselineVsMaxCHPRE = new TProfile("PbaselineVsMaxCHPRE", "PbaselineVsMaxCHPRE", 700, -200, 500, "S" );

     TH2F* baselineVsMaxCHHIP = new TH2F("baselineVsMaxCHHIP", "baselineVsMaxCHHIP", 700, -200, 500, 1000  , 0, 300 );
     TProfile* PbaselineVsMaxCHHIP = new TProfile("PbaselineVsMaxCHHIP", "PbaselineVsMaxCHHIP", 700, -200, 500, "S" );

     TH2F* baselineVsMaxCHPOST = new TH2F("baselineVsMaxCHPOST", "baselineVsMaxCHPOST", 700, -200, 500, 1000  , 0, 300 );
     TProfile* PbaselineVsMaxCHPOST = new TProfile("PbaselineVsMaxCHPOST", "PbaselineVsMaxCHPOST", 700, -200, 500, "S" );

     TH2F* BXvsMaxCH = new TH2F("BXvsMaxCH", "BXvsMaxCH", 2*lTrain, -lTrain, lTrain, 1000  , 0, 1000 );
     TProfile* PBXvsMaxCH = new TProfile("PBXvsMaxCH", "PBXvsMaxCH", 2*lTrain, -lTrain, lTrain, "S" );

     TH2F* WIvsCH = new TH2F("WIvsCH", "WIvsCH", 20, 0, 20, 10000  , 0, 20000 );
     TProfile* PWIvsCH = new TProfile("PWIvsCH", "PWIvsCH", 20, 0, 20, "S");

     TH2F* BXvsWI = new TH2F("BXvsWI", "BXvsWI", 2*lTrain, -lTrain, lTrain, 100, 0, 100);
     TProfile* PBXvsWI = new TProfile("PBXvsWI", "PBXvsWI", 2*lTrain, -lTrain, lTrain, "S");
     BXvsWI->SetTitle("");
     BXvsWI->GetYaxis()->SetTitle("width of cluster");
     BXvsWI->GetXaxis()->SetTitle("bx");
     PBXvsWI->SetTitle("");
     PBXvsWI->GetYaxis()->SetTitle("width of cluster");
     PBXvsWI->GetXaxis()->SetTitle("bx");

     TH2F* RMSvsCHall = new TH2F("RMSvsCHall", "RMSvsCHall", 200, 0, 50, 10000  , 0, 20000 );
     TProfile* PRMSvsCHall = new TProfile("PRMSvsCHall", "PRMSvsCHall", 200, 0, 50, "S" );

     TH2F* RMSvsCMULTall = new TH2F("RMSvsMULTall", "RMSvsMULTall", 200, 0, 50, 20  , 0, 20 );
     TProfile* PRMSvsCMULTall = new TProfile("PRMSvsMULTall", "PRMSvsMULTall", 200, 0, 50, "S" );

     TH2F* RMSvsSTall = new TH2F("RMSvsSTall", "RMSvsSTall", 200, 0, 50, 128  , 0, 128 );
     TProfile* PRMSvsSTall = new TProfile("PRMSvsSTall", "PRMSvsSTall", 200, 0, 50, "S" );

     TH2F* RMSvsCHPRE = new TH2F("RMSvsCHPRE", "RMSvsCHPRE", 200, 0, 50, 10000  , 0, 20000 );
     TH2F* RMSvsCHHIP = new TH2F("RMSvsCHHIP", "RMSvsCHHIP", 200, 0, 50, 10000  , 0, 20000 );
     TH2F* RMSvsCHPOST = new TH2F("RMSvsCHPOST", "RMSvsCHPOST", 200, 0, 50, 10000  , 0, 20000 );
     TH2F* RMSvsCMULTPRE = new TH2F("RMSvsMULTPRE", "RMSvsMULTPRE", 200, 0, 50, 20  , 0, 20 );
     TH2F* RMSvsCMULTHIP = new TH2F("RMSvsMULTHIP", "RMSvsMULTHIP", 200, 0, 50, 20  , 0, 20 );
     TH2F* RMSvsCMULTPOST = new TH2F("RMSvsMULTPOST", "RMSvsMULTPOST", 200, 0, 50, 20  , 0, 20 );
     TH2F* RMSvsSTPRE = new TH2F("RMSvsSTPRE", "RMSvsSTPRE", 200, 0, 50, 128  , 0, 128 );
     TH2F* RMSvsSTHIP = new TH2F("RMSvsSTHIP", "RMSvsSTHIP", 200, 0, 50, 128  , 0, 128 );
     TH2F* RMSvsSTPOST = new TH2F("RMSvsSTPOST", "RMSvsSTPOST", 200, 0, 50, 128  , 0, 128 );

     TProfile* PRMSvsCHPRE = new TProfile("PRMSvsCHPRE", "PRMSvsCHPRE", 200, 0, 50, "S" );
     TProfile* PRMSvsCHHIP = new TProfile("PRMSvsCHHIP", "PRMSvsCHHIP", 200, 0, 50, "S" );
     TProfile* PRMSvsCHPOST = new TProfile("PRMSvsCHPOST", "PRMSvsCHPOST", 200, 0, 50, "S" );
     TProfile* PRMSvsCMULTPRE = new TProfile("PRMSvsMULTPRE", "PRMSvsMULTPRE", 200, 0, 50, "S" );
     TProfile* PRMSvsCMULTHIP = new TProfile("PRMSvsMULTHIP", "PRMSvsMULTHIP", 200, 0, 50, "S" );
     TProfile* PRMSvsCMULTPOST = new TProfile("PRMSvsMULTPOST", "PRMSvsMULTPOST", 200, 0, 50, "S" );
     TProfile* PRMSvsSTPRE = new TProfile("PRMSvsSTPRE", "PRMSvsSTPRE", 200, 0, 50, "S" );
     TProfile* PRMSvsSTHIP = new TProfile("PRMSvsSTHIP", "PRMSvsSTHIP", 200, 0, 50, "S" );
     TProfile* PRMSvsSTPOST = new TProfile("PRMSvsSTPOST", "PRMSvsSTPOST", 200, 0, 50, "S" );
     //////////////////////////////////////////////////////////////////////////////////////////////////////////
     //
     TProfile* baselineP = new TProfile("baseline profile", "baseline profile", 2*lTrain, -lTrain, lTrain, "S");
     baselineP->SetTitle("");
     baselineP->GetYaxis()->SetTitle("baseline[ADC]");
     baselineP->GetXaxis()->SetTitle("bunch crossing");
    //TH1F* allBaselines = new TH1F("baselines", "baselines", 287369, 0, 287368);
    //std::map<uint32_t, vector< TGraph*> > moduleMap;
   
     /*TH2F* stripsBZvsCHPRE  = new TH2F("stripsBZvsCHPRE", "stripsBZvsCHPRE", 128, 0, 128, 10000  , 0, 20000 );
     TProfile* PstripsBZvsCHPRE  = new TProfile("PstripsBZvsCHPRE", "PstripsBZvsCHPRE", 128, 0, 128, "S" );
     TH2F* stripsBZvsMaxCHPRE  = new TH2F("stripsBZvsMaxCHPRE", "stripsBZvsMaxCHPRE", 128, 0, 128, 300  , 0, 300 );
     TProfile* PstripsBZvsMaxCHPRE  = new TProfile("PstripsBZvsMaxCHPRE", "PstripsBZvsMaxCHPRE", 128, 0, 128, "S" );
     TH2F* stripsBZvsMULTPRE  = new TH2F("stripsBZvsMULTPRE", "stripsBZvsMULTPRE", 128, 0, 128, 20 , 0, 20 );
     TProfile* PstripsBZvsMULTPRE  = new TProfile("PstripsBZvsMULTPRE", "PstripsBZvsMULTPRE", 128, 0, 128, "S" );

     TH2F* stripsBZvsCHHIP  = new TH2F("stripsBZvsCHHIP", "stripsBZvsCHHIP", 128, 0, 128, 10000  , 0, 20000 );
     TProfile* PstripsBZvsCHHIP  = new TProfile("PstripsBZvsCHHIP", "PstripsBZvsCHHIP", 128, 0, 128, "S" );
     TH2F* stripsBZvsMaxCHHIP  = new TH2F("stripsBZvsMaxCHHIP", "stripsBZvsMaxCHHIP", 128, 0, 128, 300  , 0, 300 );
     TProfile* PstripsBZvsMaxCHHIP  = new TProfile("PstripsBZvsMaxCHHIP", "PstripsBZvsMaxCHHIP", 128, 0, 128, "S" );
     TH2F* stripsBZvsMULTHIP  = new TH2F("stripsBZvsMULTHIP", "stripsBZvsMULTHIP", 128, 0, 128, 20 , 0, 20 );
     TProfile* PstripsBZvsMULTHIP  = new TProfile("PstripsBZvsMULTHIP", "PstripsBZvsMULTHIP", 128, 0, 128, "S" );

     TH2F* stripsBZvsCHPOST  = new TH2F("stripsBZvsCHPOST", "stripsBZvsCHPOST", 128, 0, 128, 10000  , 0, 20000 );
     TProfile* PstripsBZvsCHPOST  = new TProfile("PstripsBZvsCHPOST", "PstripsBZvsCHPOST", 128, 0, 128, "S" );
     TH2F* stripsBZvsMaxCHPOST  = new TH2F("stripsBZvsMaxCHPOST", "stripsBZvsMaxCHPOST", 128, 0, 128, 300  , 0, 300 );
     TProfile* PstripsBZvsMaxCHPOST  = new TProfile("PstripsBZvsMaxCHPOST", "PstripsBZvsMaxCHPOST", 128, 0, 128, "S" );
     TH2F* stripsBZvsMULTPOST  = new TH2F("stripsBZvsMULTPOST", "stripsBZvsMULTPOST", 128, 0, 128, 20 , 0, 20 );
     TProfile* PstripsBZvsMULTPOST  = new TProfile("PstripsBZvsMULTPOST", "PstripsBZvsMULTPOST", 128, 0, 128, "S" );

     TH2F* stripsAZvsCHPRE  = new TH2F("stripsAZvsCHPRE", "stripsAZvsCHPRE", 128, 0, 128, 10000  , 0, 20000 );
     TProfile* PstripsAZvsCHPRE  = new TProfile("PstripsAZvsCHPRE", "PstripsAZvsCHPRE", 128, 0, 128, "S" );
     TH2F* stripsAZvsMaxCHPRE  = new TH2F("stripsAZvsMaxCHPRE", "stripsAZvsMaxCHPRE", 128, 0, 128, 300  , 0, 300 );
     TProfile* PstripsAZvsMaxCHPRE  = new TProfile("PstripsAZvsMaxCHPRE", "PstripsAZvsMaxCHPRE", 128, 0, 128, "S" );
     TH2F* stripsAZvsMULTPRE  = new TH2F("stripsAZvsMULTPRE", "stripsAZvsMULTPRE", 128, 0, 128, 20 , 0, 20 );
     TProfile* PstripsAZvsMULTPRE  = new TProfile("PstripsAZvsMULTPRE", "PstripsAZvsMULTPRE", 128, 0, 128, "S" );

     TH2F* stripsAZvsCHHIP  = new TH2F("stripsAZvsCHHIP", "stripsAZvsCHHIP", 128, 0, 128, 10000  , 0, 20000 );
     TProfile* PstripsAZvsCHHIP  = new TProfile("PstripsAZvsCHHIP", "PstripsAZvsCHHIP", 128, 0, 128, "S" );
     TH2F* stripsAZvsMaxCHHIP  = new TH2F("stripsAZvsMaxCHHIP", "stripsAZvsMaxCHHIP", 128, 0, 128, 300  , 0, 300 );
     TProfile* PstripsAZvsMaxCHHIP  = new TProfile("PstripsAZvsMaxCHHIP", "PstripsAZvsMaxCHHIP", 128, 0, 128, "S" );
     TH2F* stripsAZvsMULTHIP  = new TH2F("stripsAZvsMULTHIP", "stripsAZvsMULTHIP", 128, 0, 128, 20 , 0, 20 );
     TProfile* PstripsAZvsMULTHIP  = new TProfile("PstripsAZvsMULTHIP", "PstripsAZvsMULTHIP", 128, 0, 128, "S" );

     TH2F* stripsAZvsCHPOST  = new TH2F("stripsAZvsCHPOST", "stripsAZvsCHPOST", 128, 0, 128, 10000  , 0, 20000 );
     TProfile* PstripsAZvsCHPOST  = new TProfile("PstripsAZvsCHPOST", "PstripsAZvsCHPOST", 128, 0, 128, "S" );
     TH2F* stripsAZvsMaxCHPOST  = new TH2F("stripsAZvsMaxCHPOST", "stripsAZvsMaxCHPOST", 128, 0, 128, 300  , 0, 300 );
     TProfile* PstripsAZvsMaxCHPOST  = new TProfile("PstripsAZvsMaxCHPOST", "PstripsAZvsMaxCHPOST", 128, 0, 128, "S" );
     TH2F* stripsAZvsMULTPOST  = new TH2F("stripsAZvsMULTPOST", "stripsAZvsMULTPOST", 128, 0, 128, 20 , 0, 20 );
     TProfile* PstripsAZvsMULTPOST  = new TProfile("PstripsAZvsMULTPOST", "PstripsAZvsMULTPOST", 128, 0, 128, "S" );

     TH2F* slopevsCHPRE  = new TH2F("slopevsCHPRE", "slopevsCHPRE", 401, -1, 1, 10000  , 0, 20000 );
     TProfile* PslopevsCHPRE  = new TProfile("PslopevsCHPRE", "PslopevsCHPRE", 401, -1, 1, "S" );
     TH2F* slopevsMaxCHPRE  = new TH2F("slopevsMaxCHPRE", "slopevsMaxCHPRE", 401, -1, 1, 300  , 0, 300 );
     TProfile* PslopevsMaxCHPRE  = new TProfile("PslopevsMaxCHPRE", "PslopevsMaxCHPRE", 401, -1, 1, "S" );
     TH2F* slopevsMULTPRE  = new TH2F("slopevsMULTPRE", "slopevsMULTPRE", 401, -1, 1, 20 , 0, 20 );
     TProfile* PslopevsMULTPRE  = new TProfile("PslopevsMULTPRE", "PslopevsMULTPRE", 401, -1, 1, "S" );

     TH2F* slopevsCHHIP  = new TH2F("slopevsCHHIP", "slopevsCHHIP", 401, -1, 1, 10000  , 0, 20000 );
     TProfile* PslopevsCHHIP  = new TProfile("PslopevsCHHIP", "PslopevsCHHIP", 401, -1, 1, "S" );
     TH2F* slopevsMaxCHHIP  = new TH2F("slopevsMaxCHHIP", "slopevsMaxCHHIP", 401, -1, 1, 300  , 0, 300 );
     TProfile* PslopevsMaxCHHIP  = new TProfile("PslopevsMaxCHHIP", "PslopevsMaxCHHIP", 401, -1, 1, "S" );
     TH2F* slopevsMULTHIP  = new TH2F("slopevsMULTHIP", "slopevsMULTHIP", 401, -1, 1, 20 , 0, 20 );
     TProfile* PslopevsMULTHIP  = new TProfile("PslopevsMULTHIP", "PslopevsMULTHIP", 401, -1, 1, "S" );

     TH2F* slopevsCHPOST  = new TH2F("slopevsCHPOST", "slopevsCHPOST", 401, -1, 1, 10000  , 0, 20000 );
     TProfile* PslopevsCHPOST  = new TProfile("PslopevsCHPOST", "PslopevsCHPOST", 401, -1, 1, "S" );
     TH2F* slopevsMaxCHPOST  = new TH2F("slopevsMaxCHPOST", "slopevsMaxCHPOST", 401, -1, 1, 300  , 0, 300 );
     TProfile* PslopevsMaxCHPOST  = new TProfile("PslopevsMaxCHPOST", "PslopevsMaxCHPOST", 401, -1, 1, "S" );
     TH2F* slopevsMULTPOST  = new TH2F("slopevsMULTPOST", "slopevsMULTPOST", 401, -1, 1, 20 , 0, 20 );
     TProfile* PslopevsMULTPOST  = new TProfile("PslopevsMULTPOST", "PslopevsMULTPOST", 401, -1, 1, "S" );

     TH2F* RMSbotvsCHPRE  = new TH2F("RMSbotvsCHPRE", "RMSbotvsCHPRE", 200, 0, 20, 10000  , 0, 20000 );
     TProfile* PRMSbotvsCHPRE  = new TProfile("PRMSbotvsCHPRE", "PRMSbotvsCHPRE", 200, 0, 20, "S" );
     TH2F* RMSbotvsMaxCHPRE  = new TH2F("RMSbotvsMaxCHPRE", "RMSbotvsMaxCHPRE", 200, 0, 20, 300  , 0, 300 );
     TProfile* PRMSbotvsMaxCHPRE  = new TProfile("PRMSbotvsMaxCHPRE", "PRMSbotvsMaxCHPRE", 200, 0, 20, "S" );
     TH2F* RMSbotvsMULTPRE  = new TH2F("RMSbotvsMULTPRE", "RMSbotvsMULTPRE", 200, 0, 20, 20 , 0, 20 );
     TProfile* PRMSbotvsMULTPRE  = new TProfile("PRMSbotvsMULTPRE", "PRMSbotvsMULTPRE", 200, 0, 20, "S" );

     TH2F* RMSbotvsCHHIP  = new TH2F("RMSbotvsCHHIP", "RMSbotvsCHHIP", 200, 0, 20, 10000  , 0, 20000 );
     TProfile* PRMSbotvsCHHIP  = new TProfile("PRMSbotvsCHHIP", "PRMSbotvsCHHIP", 200, 0, 20, "S" );
     TH2F* RMSbotvsMaxCHHIP  = new TH2F("RMSbotvsMaxCHHIP", "RMSbotvsMaxCHHIP", 200, 0, 20, 300  , 0, 300 );
     TProfile* PRMSbotvsMaxCHHIP  = new TProfile("PRMSbotvsMaxCHHIP", "PRMSbotvsMaxCHHIP", 200, 0, 20, "S" );
     TH2F* RMSbotvsMULTHIP  = new TH2F("RMSbotvsMULTHIP", "RMSbotvsMULTHIP", 200, 0, 20, 20 , 0, 20 );
     TProfile* PRMSbotvsMULTHIP  = new TProfile("PRMSbotvsMULTHIP", "PRMSbotvsMULTHIP", 200, 0, 20, "S" );

     TH2F* RMSbotvsCHPOST  = new TH2F("RMSbotvsCHPOST", "RMSbotvsCHPOST", 200, 0, 20, 10000  , 0, 20000 );
     TProfile* PRMSbotvsCHPOST  = new TProfile("PRMSbotvsCHPOST", "PRMSbotvsCHPOST", 200, 0, 20, "S" );
     TH2F* RMSbotvsMaxCHPOST  = new TH2F("RMSbotvsMaxCHPOST", "RMSbotvsMaxCHPOST", 200, 0, 20, 300  , 0, 300 );
     TProfile* PRMSbotvsMaxCHPOST  = new TProfile("PRMSbotvsMaxCHPOST", "PRMSbotvsMaxCHPOST", 200, 0, 20, "S" );
     TH2F* RMSbotvsMULTPOST  = new TH2F("RMSbotvsMULTPOST", "RMSbotvsMULTPOST", 200, 0, 20, 20 , 0, 20 );
     TProfile* PRMSbotvsMULTPOST  = new TProfile("PRMSbotvsMULTPOST", "PRMSbotvsMULTPOST", 200, 0, 20, "S" );

     
     TH2F* RMSbotvsStripsBZ  = new TH2F("RMSbotvsStripsBZ", "RMSbotvsStripsBZ", 200 , 0, 20, 128, 0, 128 );
     TProfile* PRMSbotvsStripsBZ  = new TProfile("PRMSbotvsStripsBZ", "PRMSbotvsStripsBZ", 200, 0, 20, "S" );
     TH2F* RMSbotvsStripsAZ  = new TH2F("RMSbotvsStripsAZ", "RMSbotvsStripsAZ", 200 , 0, 20, 128, 0, 128 );*/
     TH2F* slopevsCPPOST  = new TH2F("slopevsCPPOST", "slopevsCPPOST", 401, -1, 1, 140, 250, 390  );
     TProfile* PslopevsCPPOST  = new TProfile("PslopevsCPPOST", "PslopevsCPPOST", 401, -1, 1, "S" );
     TH2F* slopevsCPHIP  = new TH2F("slopevsCPHIP", "slopevsCPHIP", 401, -1, 1, 140, 250, 390  );
     TProfile* PslopevsCPHIP  = new TProfile("PslopevsCPHIP", "PslopevsCPHIP", 401, -1, 1, "S" );
     //TProfile* PRMSbotvsStripsAZ  = new TProfile("PRMSbotvsStripsAZ", "PRMSbotvsStripsAZ", 200, 0, 20, "S" );
     TH2F* RMSbotvsSlope  = new TH2F("RMSbotvsSlope", "RMSbotvsSlope", 1000 , 0, 12, 401, -1, 1 );
     TProfile* PRMSbotvsSlope  = new TProfile("PRMSbotvsSlope", "PRMSbotvsSlope", 1000, 0, 12, "S" );
     RMSbotvsSlope->SetTitle("");
     RMSbotvsSlope->GetYaxis()->SetTitle("slope");
     RMSbotvsSlope->GetXaxis()->SetTitle("RMS");

     vector<TH1F*> satBlPerMod;
     TProfile* PsatBlPerMod  = new TProfile("PsatBlPerMod", "PsatBlPerMod", 15000, 1, 15000, "S" );

    ULong64_t previousBC = 0;
    ULong64_t prevBunch = 0;
    ULong64_t prevBunch2 = 0;
    ULong64_t deltaBunch = 0;
    ULong64_t deltaBunch2 = 0;
    ULong64_t previousTIME = 0; 

    const uint16_t bunchOffset = 333;
    bool HIP = false;
    uint8_t HIPpoint = 0;
    bool HIP2 = false;
    uint8_t HIPpoint2 = 0;
    double dbx = 0;

    Int_t nentries = (Int_t)t1->GetEntries();
    cout << "number of enties: " << nentries << endl;
   
    TGraph* graph = new TGraph(nentries);
    TGraph* graph2 = new TGraph(nentries);
     //TH1D* timeDiff = NULL;
    //timeDiff = new TH1D("timeDiff", "timeDiff", 150000, 0, 150000); //@MJ@ TODO this is nasty!
    //@MJ@ TODO label: x(tdif) in microseconds!!!
    //read all entries and fill the histograms
    for (Int_t m=0; m<nentries; m++) //ATTENTION looping over modules not events
    {
                //cout << "herei " << endl;
         double total = 0;
         double ofHIP = 0;
         double ofHIP2 = 0;
         double boffDown = 100;
         double boffUp = 0;
         uint32_t peak = 0;
         uint32_t multTot = 0;
         uint32_t satStripBef = 0;
         uint32_t peakBef = 0;
         uint32_t multTotBef = 0;
         blPerMod =  TH1F("blPerMod", "blPerMod", 700, -200, 500);
         blPerModHIP =  TH1F("blPerModHIP", "blPerModHIP", 700, -200, 500);
         TString modName = to_string(moduleValueOUT); 
         if(m<5)
         {
             satBlPerMod.push_back(new TH1F(modName + "_bl", modName + "_bl", 200, -200, 0));
             baselineVsRMSallPerMod.push_back(new TH2F("baselineVsRMS_" + modName, "baselineVsRMS_" + modName, 700, -200, 500, 200  , 0, 50 ));
             satBlPerMod.at(m)->SetTitle(""); //TODO module Key
             satBlPerMod.at(m)->GetYaxis()->SetTitle("count");
             satBlPerMod.at(m)->GetXaxis()->SetTitle("baseline [ADC]");
             baselineVsRMSallPerMod.at(m)->SetTitle(""); //TODO module Key
             baselineVsRMSallPerMod.at(m)->GetYaxis()->SetTitle("RMS");
             baselineVsRMSallPerMod.at(m)->GetXaxis()->SetTitle("baseline [ADC]");
         }

        uint32_t satStrip = 0;

         if(m>25) //how many modules
             break;

         t1->GetEntry(m);

                //cout << "hereii " << endl;

         float min = 0;
         float min2 = 0;
         //for(uint32_t l = 0; l<10000; l++)
         //{
             //if(baselinesMtxOUT->at(l).at(2) < min) //sooo ugly!!!!
             //{
             //    min2 = min;
             //    min = baselinesMtxOUT->at(l).at(2);
             //}
         //}

         uint32_t bunch = 0;
         for(uint32_t i=0; i< eventNrsOUT->size(); i++)
         {
             if(i == 0)
             {
                 previousTIME = timesTOUT->at(i); 
             }


                //cout << "hereiii " << endl;
             total++;
             //if(i>100) break;
         
             if(previousBC > bunchNrsOUT->at(i) && timesTOUT->at(i) - previousTIME > 1 )
             {
                 trainOffset = 715;
                 bunch += (bunchNrsOUT->at(i) - bunchOffset) + trainOffset + (71-(previousBC - bunchOffset));
             }
             else if(previousBC > bunchNrsOUT->at(i) && timesTOUT->at(i) - previousTIME <= 1)
             {
                 trainOffset = 715; //@MJ@ TODO get precise offset numbers!!!!!
                 bunch += (bunchNrsOUT->at(i) - bunchOffset) + trainOffset + (71-(previousBC - bunchOffset));
             }
             else
             {
                 if(i==0)
                     bunch = 0;
                else if(eventNrsOUT->at(i) - eventNrsOUT->at(i-1) > 1 ) //TODO workaround if not all events present
                {
                     trainOffset = 715; //@MJ@ TODO get precise offset numbers!!!!!
                     bunch += (bunchNrsOUT->at(i) - bunchOffset) + trainOffset + (71-(previousBC - bunchOffset));
                }

                 else
                     bunch += bunchNrsOUT->at(i) - bunchNrsOUT->at(i-1);
             }

                //cout << "hereiii " << endl;
            
             //myfile << "event: " <<  eventNrsOUT->at(i) << " bunch " << bunch  <<" bx: " << bunchNrsOUT->at(i) << " time: " << timesTOUT->at(i) << endl;
             if(i != 0 )
             {
                 deltaBunch = bunch - prevBunch; 
                 deltaBunch2 = bunch - prevBunch2; 
             }
             previousBC = bunchNrsOUT->at(i); 
             previousTIME = timesTOUT->at(i); 
             //myfile << "event: " << eventNrsOUT->at(i) << " previousBC: " << previousBC <<" buncCrossing: " << bunchNrsOUT->at(i) << " bunch: " << bunch << " time: " << timesTOUT->at(i) << endl;
             ///previousBC = bunchNrOUT;
         
             if(m==0)
             {
                 graph->SetName("bunch crossing vs time");
                 graph->SetPoint(i+1, bunch , timesTOUT->at(i));
             }       
         
                //cout << "hereiiii " << endl;
        //bunch crossing vs baseline (3rd APV)
        //for(uint32_t ent = 0; ent < moduleKeysOUT->size(); ent++ ) //@MJ@ TODO user defined
        //{

            if(m == 0)
            {
                //Graphs
                graph2->SetName((TString) static_cast<ostringstream*>( &(ostringstream() << m) )->str()); //@MJ@ TODO real det ID!
                graph2->SetTitle("bunch crossing vs baseline APV(3)");
                graph2->GetYaxis()->SetTitle("baseline[ADC]");
                graph2->GetXaxis()->SetTitle("bunch crossing + train offset");

                //Histos
                //TString hName = "h" + (TString) static_cast<ostringstream*>( &(ostringstream() << m) )->str();
		//histos.baselineHist = new TH1D( hName + "bl", hName + "bl", 6006, 0, 768768);
		//histos.baselineHist->SetMaximum(300);
		//histos.clusHist = new TH1D( hName + "clus", hName + "clus", 768768, 0, 768768);

                //canvases
                //c = new TCanvas(hName, hName);
            }
                //cout << "here1 " << endl;
            std::vector<float> baselines =  baselinesMtxOUT->at(i);
            std::vector<uint16_t> clusterCen =  centerOfClustersMtxOUT->at(i);
            std::vector<uint16_t> clusterADC =  chargeOfClustersMtxOUT->at(i);
            std::vector<uint16_t> clusterStrips =  stripsTouchedOfClustersMtxOUT->at(i);
            std::vector<uint8_t> clusterMax =  maxADCOfClustersMtxOUT->at(i);
            std::vector<float> baselinesRMS =  baselineRMSMtxOUT->at(i);
            vector<float> RMSbot = baselineCMRMSMtxOUT->at(i);
            vector<float> slope = PSSlope20MtxOUT->at(i);
            vector<uint8_t> stripsAZ = positiveValuesCMNSMtxOUT->at(i);
            vector<uint8_t> stripsBZ = negativeValuesPSMtxOUT->at(i);

                //cout << "hereiiiii " << endl;
            //for(uint32_t strip = 0; strip<clusterCen.size(); strip++)
            //{
            //     histos.clusHist->SetBinContent(i*768+clusterCen.at(strip), clusterADC.at(strip));//+b*10);
            //}
            //uint32_t point = i*baselines.size(); //@MJ TODO@
            for(uint32_t b = 0; b<baselines.size(); b++)
            {
                //cout << "here2 " << endl;
                if(i==0)
                    myfile << "module size: " << baselines.size() << endl;
                if(b==2) // && i < 50)
                {
     
                uint32_t clus = 0;
                uint32_t CCharge = 0;
                uint32_t CStrips = 0;
                uint32_t CMultiplicity = 0;
                uint32_t maxCCharge = 0;
                uint32_t maxCStrips = 0;
                blPerMod.Fill(baselines.at(b));
                stripsBZps->Fill(stripsBZ.at(b));
                stripsAZcmns->Fill(stripsAZ.at(b));
                for(std::vector<uint16_t>::iterator it = clusterCen.begin(); it != clusterCen.end(); ++it)
                {
                   if(*it > 255 && *it < 384)
                   {
                      CCharge +=  clusterADC.at(clus);
                      CStrips += clusterStrips.at(clus);
                      CMultiplicity++;
                      BXvsCHall2->Fill(bunchNrsOUT->at(i) , clusterADC.at(clus));
                      PBXvsCHall2->Fill(bunchNrsOUT->at(i) , clusterADC.at(clus));
                   
                      if(maxCCharge < clusterADC.at(clus))
                      {
                          maxCCharge =  clusterADC.at(clus);
                          maxCStrips = clusterStrips.at(clus);

                      }
                      
                   }
                   clus++;
                }
                if(maxCCharge != 0)
                { 
                      RMSvsCHall->Fill(baselineRMSMtxOUT->at(i).at(b), maxCCharge); //@MJ@ TODO total cluster charge!!!
                      RMSvsSTall->Fill(baselineRMSMtxOUT->at(i).at(b), maxCStrips);
                      PRMSvsCHall->Fill(baselineRMSMtxOUT->at(i).at(b), maxCCharge); //@MJ@ TODO total cluster charge!!!
                      PRMSvsSTall->Fill(baselineRMSMtxOUT->at(i).at(b), maxCStrips );
                      //myfile << "cluster found!!" <<endl;
                      BXvsCHall->Fill(bunchNrsOUT->at(i) ,maxCCharge);
                      PBXvsCHall->Fill(bunchNrsOUT->at(i) ,maxCCharge);
                }
                RMSvsCMULTall->Fill(baselineRMSMtxOUT->at(i).at(b),  CMultiplicity);
                PRMSvsCMULTall->Fill(baselineRMSMtxOUT->at(i).at(b),  CMultiplicity);

                baselineVsRMSall->Fill(baselines.at(b), baselineRMSMtxOUT->at(i).at(b));
                PbaselineVsRMSall->Fill(baselines.at(b), baselineRMSMtxOUT->at(i).at(b));
                if(m<5)
                    baselineVsRMSallPerMod.at(m)->Fill(baselines.at(b), baselineRMSMtxOUT->at(i).at(b));
                RMSall->Fill( baselineRMSMtxOUT->at(i).at(b));
                graph2->SetPoint(i, bunch, baselines.at(b));//+b*10);
                if(maxCStrips <3)
                {
                    CHdistrSTlow->Fill(maxCCharge);
                }
                if(maxCStrips >=3 &&maxCStrips <6 )
                {
                    CHdistrSTmed->Fill(maxCCharge);
                }
                if(maxCStrips >=6)
                {
                    CHdistrSThig->Fill(maxCCharge);
                }
                //if(baselines.at(b) > -200 && baselines.at(b) < -150)
                //if(baselines.at(b) < -200 && baselines.at(b) > -210)
                //if(baselines.at(b) < (min + 2*(-min+min2)) && baselines.at(b) <0)
                //cout << "here3 " << endl;
                if(baselinesRMS.at(b) < 2.5 && baselines.at(b) < -5 )
                {//@MJ@ TODO later trigger on HIP
                    ofHIP++;
                //cout << "here4 " << endl;
                    if(HIP==true)
                        goto next;
                    HIP = true;
                    HIPpoint = 1;
                    prevBunch = bunch;           
                    oneHIP = TH1F("baseline after HIP", "baseline after HIP", 2*lTrain, -lTrain ,lTrain);
                    clusCH = TH1F("clusCH", "clusCH", 2*lTrain, -lTrain ,lTrain);
                    clusMult = TH1F("clusMult", "clusMult", 2*lTrain, -lTrain ,lTrain);
                    maxclusCH = TH1F("maxclusCH", "maxclusCH", 2*lTrain, -lTrain ,lTrain);
                    cleanedclusCH = TH1F("cleanedclusCH", "cleanedclusCH", 2*lTrain, -lTrain ,lTrain);
                    cleanedmaxclusCH = TH1F("cleanedmaxclusCH", "cleanedmaxclusCH", 2*lTrain, -lTrain ,lTrain);
                    maxclusMult = TH1F("maxclusMult", "maxclusMult", 2*lTrain, -lTrain ,lTrain);
                    cleanedclusMult = TH1F("clusMultwo333", "cleanedclusMultwo333", 2*lTrain, -lTrain ,lTrain);
                    oneHIP.SetBinContent(1+lTrain, baselines.at(b));
                    BXvsRMS->Fill((Double_t)0, (baselineRMSMtxOUT->at(i).at(b)) );
                    baselineVsRMSHIP->Fill(baselines.at(b), baselineRMSMtxOUT->at(i).at(b));
                    PBXvsRMS->Fill((Double_t)0, (baselineRMSMtxOUT->at(i).at(b)) );
                    PbaselineVsRMSHIP->Fill(baselines.at(b), baselineRMSMtxOUT->at(i).at(b));
                    blPerModHIP.Fill(baselines.at(b));
                    BXvsBZ->Fill((Double_t)0, stripsBZ.at(b));
                    PBXvsBZ->Fill((Double_t)0, stripsBZ.at(b));
                    BXvsAZ->Fill((Double_t)0, stripsAZ.at(b));
                    PBXvsAZ->Fill((Double_t)0, stripsAZ.at(b));
                    BXvsRMSbot->Fill((Double_t)0, (RMSbot.at(b)) );
                    PBXvsRMSbot->Fill((Double_t)0, (RMSbot.at(b)) );
                    baseline2D->Fill((Double_t)0, baselines.at(b));
                    baselineP->Fill((Double_t)0, baselines.at(b));
                    if(m<5)
                        satBlPerMod.at(m)->Fill(baselines.at(b));
                    PsatBlPerMod->Fill(moduleValueOUT,baselines.at(b));
                    //set baseline from previous point
                    if(i>0 && bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) > 0 && bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) < lTrain)
                        oneHIP.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, baselinesMtxOUT->at(i-1).at(b));
                    oneHIP.SetMinimum(-200);
                    oneHIP.SetMaximum(500);
                    RMS->Fill( baselineRMSMtxOUT->at(i).at(b));
                    //myfile  << "HIP p1, baseline"<<  baselines.at(b) << " bunch " << bunch <<" time " << timesTOUT->at(i) << " evNR " << eventNrsOUT->at(i)  << endl;
                    
                    //evfile << moduleKeyOUT << " " << eventNrsOUT->at(i) << " " << "HIP" << endl;  //I need to save module ID!!!!
              
                    uint32_t counter = 0;
                    uint32_t chrg = 0;
                    uint32_t mult = 0;
                    uint32_t strp = 0;
                    uint32_t maxchrg = 0;
                    uint32_t maxstrp = 0;
                    uint32_t maxcen = 0;
                    uint32_t maxmaxchrg = 0;
                //cout << "here5 " << endl;
                    for(std::vector<uint16_t>::iterator it = clusterCen.begin(); it != clusterCen.end(); ++it)
                    {
                      if(*it > 255 && *it < 384)
                      {
                          chrg +=  clusterADC.at(counter);
                          strp +=  clusterStrips.at(counter);
                          mult++;
                          BXvsCH2->Fill((Double_t)0 , clusterADC.at(counter));
                          PBXvsCH2->Fill((Double_t)0 , clusterADC.at(counter));
                          if(bunchNrsOUT->at(i) > 334)
                          {
                              cleanedBXvsCH2->Fill((Double_t)0 , clusterADC.at(counter));
                              cleanedPBXvsCH2->Fill((Double_t)0 , clusterADC.at(counter));
                          }
                          if(maxchrg < clusterADC.at(counter))
                          {
                              maxchrg =  clusterADC.at(counter);
                              maxstrp = clusterStrips.at(counter);
                              maxmaxchrg = clusterMax.at(counter);
                              maxcen = clusterCen.at(counter);

                          }
                          if(clusterMax.at(counter) > 253)
                          {
                              peak++; //@MJ@ TODO to be checked
                              satStrip++;
                          }
                          multTot++;
                      }
                      counter++;
                    }
                    if(maxchrg>0)
                    {
                          RMSvsCHHIP->Fill(baselineRMSMtxOUT->at(i).at(b), maxchrg); //@MJ@ TODO total cluster charge!!!
                          RMSvsSTHIP->Fill(baselineRMSMtxOUT->at(i).at(b), maxstrp);
                          PRMSvsCHHIP->Fill(baselineRMSMtxOUT->at(i).at(b), maxchrg); //@MJ@ TODO total cluster charge!!!
                          PRMSvsSTHIP->Fill(baselineRMSMtxOUT->at(i).at(b), maxstrp);
                          BXvsCH->Fill((Double_t)0 ,maxchrg);
                          BXvsMaxCH->Fill((Double_t)0 , maxmaxchrg);
                          WIvsCH->Fill(maxstrp , maxchrg);
                          BXvsWI->Fill((Double_t)0, maxstrp);
                          PBXvsCH->Fill((Double_t)0 ,maxchrg);
                          PBXvsMaxCH->Fill((Double_t)0 , maxmaxchrg);
                          PWIvsCH->Fill(maxstrp , maxchrg);
                          PBXvsWI->Fill((Double_t)0, maxstrp);
                          baselineVsMaxCHHIP->Fill(baselinesMtxOUT->at(i).at(b), maxmaxchrg);
                          PbaselineVsMaxCHHIP->Fill(baselinesMtxOUT->at(i).at(b), maxmaxchrg);
                          maxCHHIP->Fill(maxmaxchrg);
                          CHdistrHIP->Fill(maxchrg);
                                /*stripsBZvsCHHIP->Fill( stripsBZ.at(b), maxchrg);
                                PstripsBZvsCHHIP->Fill( stripsBZ.at(b), maxchrg);
                                stripsBZvsMaxCHHIP->Fill( stripsBZ.at(b), maxmaxchrg);
                                PstripsBZvsMaxCHHIP->Fill( stripsBZ.at(b), maxmaxchrg);
                                stripsAZvsCHHIP->Fill( stripsAZ.at(b), maxchrg);
                                PstripsAZvsCHHIP->Fill( stripsAZ.at(b), maxchrg);
                                stripsAZvsMaxCHHIP->Fill( stripsAZ.at(b), maxmaxchrg);
                                PstripsAZvsMaxCHHIP->Fill( stripsAZ.at(b), maxmaxchrg);
                                slopevsCHHIP->Fill( slope.at(b), maxchrg);
                                PslopevsCHHIP->Fill( slope.at(b), maxchrg);
                                slopevsMaxCHHIP->Fill( slope.at(b), maxmaxchrg);
                                PslopevsMaxCHHIP->Fill( slope.at(b), maxmaxchrg);*/
                                slopevsCPHIP->Fill(slope.at(b), maxcen);
                                PslopevsCPHIP->Fill(slope.at(b), maxcen);
                        if(maxchrg>600)
                        {
                            BXvsRMSCHm600->Fill((Double_t)0, baselineRMSMtxOUT->at(i).at(b) );
                            PBXvsRMSCHm600->Fill((Double_t)0, baselineRMSMtxOUT->at(i).at(b) );
                        }
                        else
                        {
                            BXvsRMSCHl600->Fill((Double_t)0, baselineRMSMtxOUT->at(i).at(b) );
                            PBXvsRMSCHl600->Fill((Double_t)0, baselineRMSMtxOUT->at(i).at(b) );
                        }
                        if(mult == 1)  
                        oneClusCHHIP->Fill(maxchrg);
                        if(mult == 2)  
                        twoClusCHHIP->Fill(maxchrg);
                          if(maxmaxchrg>253)
                          {
                              //SATvsNSATclusHIP->Fill(0.5);
                              saturCH->Fill((Double_t)0);
                          }
                          else
                          {
                              //SATvsNSATclusHIP->Fill(1.5);
                          }
                              nonsaturCH->Fill((Double_t)0);
                    }                 
                                /*stripsBZvsMULTHIP->Fill( stripsBZ.at(b), mult); 
                                PstripsBZvsMULTHIP->Fill( stripsBZ.at(b), mult);
                                stripsAZvsMULTHIP->Fill( stripsAZ.at(b), mult); 
                                PstripsAZvsMULTHIP->Fill( stripsAZ.at(b), mult);
                                slopevsMULTHIP->Fill( slope.at(b), mult); 
                                PslopevsMULTHIP->Fill( slope.at(b), mult);
                    RMSvsCMULTHIP->Fill(baselineRMSMtxOUT->at(i).at(b), mult );
                    PRMSvsCMULTHIP->Fill(baselineRMSMtxOUT->at(i).at(b), mult );
                        RMSbotvsStripsBZ->Fill( RMSbot.at(b),stripsBZ.at(b));
                        PRMSbotvsStripsBZ->Fill( RMSbot.at(b),stripsBZ.at(b));
                        RMSbotvsStripsAZ->Fill( RMSbot.at(b), stripsBZ.at(b));
                        PRMSbotvsStripsAZ->Fill( RMSbot.at(b),stripsBZ.at(b));
                        PRMSbotvsStripsAZ->Fill( RMSbot.at(b), slope.at(b));*/
                        RMSbotvsSlope->Fill( RMSbot.at(b), slope.at(b));
                    if(mult != 0)
                    {
                        clusCH.SetBinContent(1+lTrain, chrg/mult);
                        clusMult.SetBinContent(1+lTrain, mult);
                        maxclusCH.SetBinContent(1+lTrain, maxchrg);
                        maxclusMult.SetBinContent(1+lTrain, 1);
                        if(bunchNrsOUT->at(i) > 334)
                        {
                            cleanedclusCH.SetBinContent(1+lTrain, chrg/mult);
                            cleanedclusMult.SetBinContent(1+lTrain, mult);
                            cleanedmaxclusCH.SetBinContent(1+lTrain, maxchrg);
                        }
                        else
                        {
                            cleanedclusCH.SetBinContent(1+lTrain, 0);
                            cleanedclusMult.SetBinContent(1+lTrain, 0);
                            cleanedmaxclusCH.SetBinContent(1+lTrain, 0);
                        }
                    }
                    else
                    {
                        clusCH.SetBinContent(1+lTrain, -1);
                        clusMult.SetBinContent(1+lTrain, -1);
                        maxclusCH.SetBinContent(1+lTrain, -1);
                        maxclusMult.SetBinContent(1+lTrain, -1);
                        if(bunchNrsOUT->at(i) > 334)
                        {
                            cleanedclusCH.SetBinContent(1+lTrain, -1);
                            cleanedclusMult.SetBinContent(1+lTrain, -1);
                            cleanedmaxclusCH.SetBinContent(1+lTrain, -1);
                        }
                        else
                        {
                            cleanedclusCH.SetBinContent(1+lTrain, 0);
                            cleanedclusMult.SetBinContent(1+lTrain, 0);
                            cleanedmaxclusCH.SetBinContent(1+lTrain, 0);
                        }
                    }
                    
                //cout << "here6 " << endl;
                    if(i>0 && bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) > 0 && bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) < lTrain)
                    {
                        //myfile << "previous point" << endl;
                        uint32_t counterBef = 0;
         uint32_t chrgBef = 0;
         uint32_t multBef = 0;
         uint32_t strpBef = 0;
         uint32_t maxchrgBef = 0;
         uint32_t maxmaxchrgBef = 0;
         uint32_t maxstrpBef = 0;
                        if((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) != 0)
                        {
                        BXvsRMS->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i),baselineRMSMtxOUT->at(i-1).at(b));
                        PBXvsRMS->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i),baselineRMSMtxOUT->at(i-1).at(b));
                        }
                        RMS->Fill( baselineRMSMtxOUT->at(i-1).at(b));
                        baselineVsRMSPRE->Fill(baselinesMtxOUT->at(i-1).at(b), baselineRMSMtxOUT->at(i-1).at(b));
                        PbaselineVsRMSPRE->Fill(baselinesMtxOUT->at(i-1).at(b), baselineRMSMtxOUT->at(i-1).at(b));
                    BXvsBZ->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), negativeValuesPSMtxOUT->at(i-1).at(b));
                    PBXvsBZ->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), negativeValuesPSMtxOUT->at(i-1).at(b));
                    BXvsAZ->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), positiveValuesCMNSMtxOUT->at(i-1).at(b));
                    PBXvsAZ->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), positiveValuesCMNSMtxOUT->at(i-1).at(b));
                    BXvsRMSbot->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), baselineCMRMSMtxOUT->at(i-1).at(b));
                    PBXvsRMSbot->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), baselineCMRMSMtxOUT->at(i-1).at(b));
                    baseline2D->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), baselinesMtxOUT->at(i-1).at(b));
                    baselineP->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), baselinesMtxOUT->at(i-1).at(b));
                        
                        for(std::vector<uint16_t>::iterator it2 = centerOfClustersMtxOUT->at(i-1).begin(); it2 != centerOfClustersMtxOUT->at(i-1).end(); ++it2)
                        {
                //cout << "here7 " << endl;
                            if(*it2 > 255 && *it2 < 384)
                            {
                                chrgBef += chargeOfClustersMtxOUT->at(i-1).at(counterBef);
                                strpBef += stripsTouchedOfClustersMtxOUT->at(i-1).at(counterBef);
                                multBef++;
                                BXvsCH2->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) , chargeOfClustersMtxOUT->at(i-1).at(counterBef));
                                PBXvsCH2->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) , chargeOfClustersMtxOUT->at(i-1).at(counterBef));
                               if(bunchNrsOUT->at(i-1) > 334)
                               {
                                     cleanedBXvsCH2->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) , chargeOfClustersMtxOUT->at(i-1).at(counterBef));
                                     cleanedPBXvsCH2->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) , chargeOfClustersMtxOUT->at(i-1).at(counterBef));
                                }
                               if(maxchrg < chargeOfClustersMtxOUT->at(i-1).at(counterBef))
                               {
                                  maxchrgBef =  chargeOfClustersMtxOUT->at(i-1).at(counterBef);
                                  maxstrpBef = stripsTouchedOfClustersMtxOUT->at(i-1).at(counterBef);
                                  maxmaxchrgBef = maxADCOfClustersMtxOUT->at(i-1).at(counterBef);

                                }
                                if(maxADCOfClustersMtxOUT->at(i-1).at(counterBef) > 253  && bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) < 15 )
                                {
                                   peakBef++;
                                   satStripBef++;
                                }
                                multTotBef++;
                            }
                            counterBef++;
                        }
                        if(maxchrgBef>0)
                        { 
                                RMSvsCHPRE->Fill(baselineRMSMtxOUT->at(i-1).at(b), maxchrgBef); //@MJ@ TODO total cluster charge!!!
                                RMSvsSTPRE->Fill(baselineRMSMtxOUT->at(i-1).at(b), maxstrpBef);
                                PRMSvsCHPRE->Fill(baselineRMSMtxOUT->at(i-1).at(b), maxchrgBef); //@MJ@ TODO total cluster charge!!!
                                PRMSvsSTPRE->Fill(baselineRMSMtxOUT->at(i-1).at(b), maxstrpBef );
                                BXvsCH->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) ,maxchrgBef);
                                BXvsMaxCH->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) , maxmaxchrgBef);
                                WIvsCH->Fill(maxstrpBef , maxchrgBef);
                                BXvsWI->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), maxstrpBef);
                                PBXvsCH->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) , maxchrgBef);
                                PBXvsMaxCH->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i) , maxmaxchrgBef);
                                PWIvsCH->Fill(maxstrpBef , maxchrgBef);
                                PBXvsWI->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), maxstrpBef);
                                baselineVsMaxCHPRE->Fill(baselinesMtxOUT->at(i-1).at(b), maxmaxchrgBef);
                                PbaselineVsMaxCHPRE->Fill(baselinesMtxOUT->at(i-1).at(b), maxmaxchrgBef);
                                /*stripsBZvsCHPRE->Fill( negativeValuesPSMtxOUT->at(i-1).at(b), maxchrgBef);
                                PstripsBZvsCHPRE->Fill( negativeValuesPSMtxOUT->at(i-1).at(b), maxchrgBef);
                                stripsBZvsMaxCHPRE->Fill( negativeValuesPSMtxOUT->at(i-1).at(b), maxmaxchrgBef);
                                PstripsBZvsMaxCHPRE->Fill( negativeValuesPSMtxOUT->at(i-1).at(b), maxmaxchrgBef);
                                stripsAZvsCHPRE->Fill( positiveValuesCMNSMtxOUT->at(i-1).at(b), maxchrgBef);
                                PstripsAZvsCHPRE->Fill( positiveValuesCMNSMtxOUT->at(i-1).at(b), maxchrgBef);
                                stripsAZvsMaxCHPRE->Fill( positiveValuesCMNSMtxOUT->at(i-1).at(b), maxmaxchrgBef);
                                PstripsAZvsMaxCHPRE->Fill( positiveValuesCMNSMtxOUT->at(i-1).at(b), maxmaxchrgBef);
                                slopevsCHPRE->Fill( PSSlope20MtxOUT->at(i-1).at(b), maxchrgBef);
                                PslopevsCHPRE->Fill( PSSlope20MtxOUT->at(i-1).at(b), maxchrgBef);
                                slopevsMaxCHPRE->Fill( PSSlope20MtxOUT->at(i-1).at(b), maxmaxchrgBef);
                                PslopevsMaxCHPRE->Fill( PSSlope20MtxOUT->at(i-1).at(b), maxmaxchrgBef);*/
                        if(maxchrgBef>600)
                        {
                            BXvsRMSCHm600->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), (baselineRMSMtxOUT->at(i-1).at(b)) );
                            PBXvsRMSCHm600->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), (baselineRMSMtxOUT->at(i-1).at(b)) );
                        }
                        else
                        {
                            BXvsRMSCHl600->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), (baselineRMSMtxOUT->at(i-1).at(b)) );
                            PBXvsRMSCHl600->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i), (baselineRMSMtxOUT->at(i-1).at(b)) );
                        }

                                if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) == 3 && multBef == 2)
                                    CHBXmin3->Fill(maxchrgBef);
                                if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) == 25 && multBef == 2)
                                    CHBXmin25->Fill(maxchrgBef);
                                if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) < 7)
                                   CHdistrPRE->Fill(maxchrgBef);
                        }
                          if(maxmaxchrgBef>253)
                          {
                              saturCH->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i));
                          }
                              nonsaturCH->Fill((Double_t)bunchNrsOUT->at(i-1)-bunchNrsOUT->at(i));

                                /*stripsBZvsMULTPRE->Fill( negativeValuesPSMtxOUT->at(i-1).at(b), multBef); 
                                PstripsBZvsMULTPRE->Fill( negativeValuesPSMtxOUT->at(i-1).at(b), multBef);
                                stripsAZvsMULTPRE->Fill( positiveValuesCMNSMtxOUT->at(i-1).at(b), multBef); 
                                PstripsAZvsMULTPRE->Fill( positiveValuesCMNSMtxOUT->at(i-1).at(b), multBef);
                                slopevsMULTPRE->Fill( PSSlope20MtxOUT->at(i-1).at(b), multBef); 
                                PslopevsMULTPRE->Fill( PSSlope20MtxOUT->at(i-1).at(b), multBef);
                        RMSvsCMULTPRE->Fill(baselineRMSMtxOUT->at(i-1).at(b), multBef );
                        PRMSvsCMULTPRE->Fill(baselineRMSMtxOUT->at(i-1).at(b), multBef );
                        RMSbotvsStripsBZ->Fill( baselineCMRMSMtxOUT->at(i-1).at(b),negativeValuesPSMtxOUT->at(i-1).at(b));
                        PRMSbotvsStripsBZ->Fill( baselineCMRMSMtxOUT->at(i-1).at(b),negativeValuesPSMtxOUT->at(i-1).at(b));
                        RMSbotvsStripsAZ->Fill( baselineCMRMSMtxOUT->at(i-1).at(b), positiveValuesCMNSMtxOUT->at(i-1).at(b));
                        PRMSbotvsStripsAZ->Fill( baselineCMRMSMtxOUT->at(i-1).at(b),positiveValuesCMNSMtxOUT->at(i-1).at(b));
                        RMSbotvsSlope->Fill( baselineCMRMSMtxOUT->at(i-1).at(b), PSSlope20MtxOUT->at(i-1).at(b));
                        PRMSbotvsStripsAZ->Fill( baselineCMRMSMtxOUT->at(i-1).at(b), PSSlope20MtxOUT->at(i-1).at(b));*/
                        RMSbotvsSlope->Fill( baselineCMRMSMtxOUT->at(i-1).at(b), PSSlope20MtxOUT->at(i-1).at(b));
                            if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) == 3)
                                multBXmin3->Fill(multBef);
                            if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) == 10)
                                multBXmin10->Fill(multBef);
                        if(multBef != 0)
                        {
                            clusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, chrgBef/multBef);
                            clusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, multBef);
                            maxclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, maxchrgBef);
                            maxclusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, 1);
                            if(bunchNrsOUT->at(i-1) > 334)
                            {
                                cleanedclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, chrgBef/multBef);
                                cleanedmaxclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, maxchrgBef);
                                cleanedclusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, multBef);
                            }
                            else
                            {
                                cleanedclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1,  0);
                                cleanedmaxclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, 0);
                                cleanedclusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, 0);
                            }
                        }
                        else
                        {
                            clusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, -1);
                            clusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, -1);
                            maxclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, -1);
                            maxclusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, -1);
                            if(bunchNrsOUT->at(i-1) > 334)
                            {
                                cleanedclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, -1);
                                cleanedmaxclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, -1);
                                cleanedclusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, -1);
                            }
                            else
                            {
                                cleanedclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1,  0);
                                cleanedmaxclusCH.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, 0);
                                cleanedclusMult.SetBinContent(lTrain-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, 0);
                            }
                        }
                        //evfile << moduleKeyOUT << " " << eventNrsOUT->at(i) << " " << "PRE" << endl;  //I need to save module ID!!!!
                        //
                //cout << "here8 " << endl;
                        if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) < 15 && baselinesMtxOUT->at(i-1).at(b) < -150)
                        {
                            Double_t fill = (Double_t)(((Double_t)bunchNrsOUT->at(i-1))-((Double_t)bunchNrsOUT->at(i)));
                            //myfile << "filled small" << endl;
                            //myfile << "filled value " << fill << endl;
                            //blBefore->Fill((Double_t)fill);
                            //myfile << "filled value 2 " << fill << endl;
                            if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) == 3)
                            {
                                uint8_t result = 5;
                                if(baselinesMtxOUT->at(i-1).at(b) - baselinesMtxOUT->at(i).at(b) < 0)
                                    result = 1;
                                else if(baselinesMtxOUT->at(i-1).at(b) < (min + 2*(-min+min2)))
                                    result = 2;
                                else 
                                    result = 3;

                               satBefore->Fill(result);
                            } 
                        }
                        else if(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) < 15)
                        {
                            Double_t weight = 1/3;
                            Double_t fill2 = -1;
                            //blBefore->Fill(fill2, weight);
                            //myfile << "filled large" << endl;    
                        }
                    }
                    
                }
                else if(HIP==true && deltaBunch < lTrain)
                {
                //cout << "here9 " << endl;
                    next:
                    HIPpoint++;
                    oneHIP.SetBinContent(deltaBunch+1+lTrain, baselines.at(b));
                    uint32_t chrgAf = 0;
                    uint32_t multAf = 0;
                    uint32_t strpAf = 0;
                    uint32_t maxchrgAf = 0;
                    uint32_t maxcenAf = 0;
                    uint32_t maxmaxchrgAf = 0;
                    uint32_t maxstrpAf = 0;
                    uint32_t counter = 0;
                //cout << "here9.1 " << endl;
                    if(deltaBunch != 0)
                    {
                    BXvsRMS->Fill(deltaBunch,baselineRMSMtxOUT->at(i).at(b));
                    PBXvsRMS->Fill(deltaBunch,baselineRMSMtxOUT->at(i).at(b));
                    
                    RMS->Fill( baselineRMSMtxOUT->at(i).at(b));
                    baselineVsRMSPOST->Fill(baselines.at(b), baselineRMSMtxOUT->at(i).at(b));
                    PbaselineVsRMSPOST->Fill(baselines.at(b), baselineRMSMtxOUT->at(i).at(b));
                    BXvsBZ->Fill(deltaBunch, stripsBZ.at(b));
                    PBXvsBZ->Fill(deltaBunch, stripsBZ.at(b));
                    BXvsAZ->Fill(deltaBunch, stripsAZ.at(b));
                    PBXvsAZ->Fill(deltaBunch, stripsAZ.at(b));
                    BXvsRMSbot->Fill(deltaBunch, RMSbot.at(b));
                    PBXvsRMSbot->Fill(deltaBunch, RMSbot.at(b));
                    baseline2D->Fill(deltaBunch, baselines.at(b));
                    baselineP->Fill(deltaBunch, baselines.at(b));
                    }
                //cout << "here9.2 " << endl;
                   
                    if(baselineRMSMtxOUT->at(i).at(b) < 2.5) 
                        RMSupTO2p5POST->Fill(deltaBunch);
                    if(baselineRMSMtxOUT->at(i).at(b) < 4) 
                        RMSmoreTHAN2p5POST->Fill(deltaBunch);
                    RMSrestPOST->Fill(deltaBunch);
                    if(HIPpoint == 2 && i>1)
                    {
                        RMSHIPvsRMSPOST->Fill(baselineRMSMtxOUT->at(i-1).at(b), baselineRMSMtxOUT->at(i).at(b));
                        PRMSHIPvsRMSPOST->Fill(baselineRMSMtxOUT->at(i-1).at(b), baselineRMSMtxOUT->at(i).at(b));
                    }
                    if(HIPpoint == 3 && i>2)
                    {
                        RMSHIPvsRMSPOST->Fill(baselineRMSMtxOUT->at(i-2).at(b), baselineRMSMtxOUT->at(i).at(b));
                        PRMSHIPvsRMSPOST->Fill(baselineRMSMtxOUT->at(i-2).at(b), baselineRMSMtxOUT->at(i).at(b));
                    }
                //cout << "here9.3 " << endl;
                    for(std::vector<uint16_t>::iterator it = clusterCen.begin(); it != clusterCen.end(); ++it)
                    {
                      if(*it > 255 && *it < 384)
                      {
                          chrgAf +=  clusterADC.at(counter);
                          strpAf +=  clusterStrips.at(counter);
                          BXvsCH2->Fill(deltaBunch , clusterADC.at(counter));
                          PBXvsCH2->Fill(deltaBunch , clusterADC.at(counter));
                          if(deltaBunch < 21 && baselineRMSMtxOUT->at(i).at(b) < 2.5)
                          {
                               BXvsCHsel1->Fill(deltaBunch , clusterADC.at(counter));
                               PBXvsCHsel1->Fill(deltaBunch , clusterADC.at(counter));
                          }

                          if(deltaBunch < 21 && baselines.at(b) > 0)
                          {
                              if(slope.at(b) < -0.15 || slope.at(b) > 0.15 )
                              {
                                   BXvsCHsel2->Fill(deltaBunch , clusterADC.at(counter));
                                   PBXvsCHsel2->Fill(deltaBunch , clusterADC.at(counter));
                              }
                          }
                          if(bunchNrsOUT->at(i) > 334)
                          {
                              cleanedBXvsCH2->Fill(deltaBunch , clusterADC.at(counter));
                              cleanedPBXvsCH2->Fill(deltaBunch , clusterADC.at(counter));
                          }
                          if(maxchrgAf < clusterADC.at(counter))
                          {
                                  maxchrgAf =  clusterADC.at(counter);
                                  maxstrpAf = clusterStrips.at(counter);
                                  maxmaxchrgAf = clusterMax.at(counter);
                                  maxcenAf = clusterCen.at(counter);

                          }
                          //cout << "charge of cluster: " << clusterADC.at(counter) << endl;
                          multAf++;
                      }
                      counter++;
                    }
                //cout << "here9.4 " << endl;
                    if(maxchrgAf>0)
                    {
                          RMSvsCHPOST->Fill(baselineRMSMtxOUT->at(i).at(b), maxchrgAf); //@MJ@ TODO total cluster charge!!!
                          RMSvsSTPOST->Fill(baselineRMSMtxOUT->at(i).at(b), maxstrpAf );
                          PRMSvsCHPOST->Fill(baselineRMSMtxOUT->at(i).at(b), maxchrgAf); //@MJ@ TODO total cluster charge!!!
                          PRMSvsSTPOST->Fill(baselineRMSMtxOUT->at(i).at(b), maxstrpAf );
                          BXvsCH->Fill(deltaBunch , maxchrgAf);
                          BXvsMaxCH->Fill(deltaBunch, maxmaxchrgAf);
                          WIvsCH->Fill(maxstrpAf , maxchrgAf);
                          BXvsWI->Fill(deltaBunch, maxstrpAf);
                          PBXvsCH->Fill(deltaBunch , maxchrgAf);
                          PBXvsMaxCH->Fill(deltaBunch , maxmaxchrgAf);
                          PWIvsCH->Fill(maxstrpAf , maxchrgAf);
                          PBXvsWI->Fill(deltaBunch, maxstrpAf);
                          baselineVsMaxCHPOST->Fill(baselinesMtxOUT->at(i).at(b), maxmaxchrgAf);
                          PbaselineVsMaxCHPOST->Fill(baselinesMtxOUT->at(i).at(b), maxmaxchrgAf);
                          CHdistrPOST->Fill(maxchrgAf);
                                /*stripsBZvsCHPOST->Fill( stripsBZ.at(b), maxchrgAf);
                                PstripsBZvsCHPOST->Fill( stripsBZ.at(b), maxchrgAf);
                                stripsBZvsMaxCHPOST->Fill( stripsBZ.at(b), maxmaxchrgAf);
                                PstripsBZvsMaxCHPOST->Fill( stripsBZ.at(b), maxmaxchrgAf);
                                stripsAZvsCHPOST->Fill( stripsAZ.at(b), maxchrgAf);
                                PstripsAZvsCHPOST->Fill( stripsAZ.at(b), maxchrgAf);
                                stripsAZvsMaxCHPOST->Fill( stripsAZ.at(b), maxmaxchrgAf);
                                PstripsAZvsMaxCHPOST->Fill( stripsAZ.at(b), maxmaxchrgAf);
                                slopevsCHPOST->Fill( slope.at(b), maxchrgAf);
                                PslopevsCHPOST->Fill( slope.at(b), maxchrgAf);
                                slopevsMaxCHPOST->Fill( slope.at(b), maxmaxchrgAf);
                                PslopevsMaxCHPOST->Fill( slope.at(b), maxmaxchrgAf);*/
                                slopevsCPPOST->Fill(slope.at(b), maxcenAf);
                                PslopevsCPPOST->Fill(slope.at(b), maxcenAf);
                    }
                                /*stripsBZvsMULTPOST->Fill( stripsBZ.at(b), multAf); 
                                PstripsBZvsMULTPOST->Fill( stripsBZ.at(b), multAf);
                                stripsAZvsMULTPOST->Fill( stripsAZ.at(b), multAf); 
                                PstripsAZvsMULTPOST->Fill( stripsAZ.at(b), multAf);
                                slopevsMULTPOST->Fill( slope.at(b), multAf); 
                                PslopevsMULTPOST->Fill( slope.at(b), multAf);
                    RMSvsCMULTPOST->Fill(baselineRMSMtxOUT->at(i).at(b), multAf );
                    PRMSvsCMULTPOST->Fill(baselineRMSMtxOUT->at(i).at(b), multAf );
                        RMSbotvsStripsBZ->Fill( RMSbot.at(b),stripsBZ.at(b));
                        PRMSbotvsStripsBZ->Fill( RMSbot.at(b),stripsBZ.at(b));
                        RMSbotvsStripsAZ->Fill( RMSbot.at(b), stripsBZ.at(b));
                        PRMSbotvsStripsAZ->Fill( RMSbot.at(b),stripsBZ.at(b));
                        PRMSbotvsStripsAZ->Fill( RMSbot.at(b), slope.at(b));*/
                        RMSbotvsSlope->Fill( RMSbot.at(b), slope.at(b));
                //cout << "here9.5 " << endl;
                        if(maxchrgAf>600)
                        {
                            BXvsRMSCHm600->Fill(deltaBunch, (baselineRMSMtxOUT->at(i).at(b)) );
                            PBXvsRMSCHm600->Fill(deltaBunch, (baselineRMSMtxOUT->at(i).at(b)) );
                        }
                        else
                        {
                            BXvsRMSCHl600->Fill(deltaBunch, (baselineRMSMtxOUT->at(i).at(b)) );
                            PBXvsRMSCHl600->Fill(deltaBunch, (baselineRMSMtxOUT->at(i).at(b)) );
                        }
                          if(maxmaxchrgAf>253)
                          {
                              saturCH->Fill(deltaBunch);
                          }
                              nonsaturCH->Fill(deltaBunch);
                //cout << "here9.6 " << endl;
                    if(multAf != 0)
                    {                    
                        clusCH.SetBinContent(deltaBunch+1+lTrain, chrgAf/multAf);
                        clusMult.SetBinContent(deltaBunch+1+lTrain, multAf);
                        maxclusCH.SetBinContent(deltaBunch+1+lTrain, maxchrgAf);
                        maxclusMult.SetBinContent(deltaBunch+1+lTrain, 1);
                        if(bunchNrsOUT->at(i) > 334)
                        {
                            cleanedclusCH.SetBinContent(deltaBunch+1+lTrain, chrgAf/multAf);
                            cleanedclusMult.SetBinContent(deltaBunch+1+lTrain, multAf);
                            cleanedmaxclusCH.SetBinContent(deltaBunch+1+lTrain, maxchrgAf);
                        }
                        else
                        {
                            cleanedclusCH.SetBinContent(deltaBunch+1+lTrain, 0);
                            cleanedclusMult.SetBinContent(deltaBunch+1+lTrain, 0);
                            cleanedmaxclusCH.SetBinContent(deltaBunch+1+lTrain, 0);
                        }
                         if(deltaBunch < lTrain)
                            chargePOST->Fill(chrgAf/multAf);
                    }
                    else
                    {
                        clusCH.SetBinContent(deltaBunch+1+lTrain, -1);
                        clusMult.SetBinContent(deltaBunch+1+lTrain, -1);
                        maxclusCH.SetBinContent(deltaBunch+1+lTrain, -1);
                        maxclusMult.SetBinContent(deltaBunch+1+lTrain, -1);
                        if(bunchNrsOUT->at(i) > 334)
                        {
                            cleanedclusCH.SetBinContent(deltaBunch+1+lTrain, -1);
                            cleanedclusMult.SetBinContent(deltaBunch+1+lTrain, -1);
                            cleanedmaxclusCH.SetBinContent(deltaBunch+1+lTrain, -1);
                        }
                        else
                        {
                            cleanedclusCH.SetBinContent(deltaBunch+1+lTrain, 0);
                            cleanedclusMult.SetBinContent(deltaBunch+1+lTrain, 0);
                            cleanedmaxclusCH.SetBinContent(deltaBunch+1+lTrain, 0);
                        }
                    }
                    //evfile << moduleKeyOUT << " " << eventNrsOUT->at(i) << " " << "POST" << endl;  //I need to save module ID!!!!
                    
                    //myfile  << "HIP p other, db" << deltaBunch << " bunch " << bunch<< " prevBunch " << prevBunch << " baseline " << baselines.at(b) << " time " << timesTOUT->at(i) << " evNR " << eventNrsOUT->at(i)<< endl;
                    //
                //cout << "here10 " << endl;
                }
                else
                {

                    //if(baselines.at(b) < -210)
                    if(HIPpoint > 0)
                    {
                //cout << "here11 " << endl;
                        
                        moduleHIP.push_back(oneHIP);
                        moduleCHRG.push_back(clusCH);
                        moduleMULT.push_back(clusMult);
                        maxmoduleCHRG.push_back(maxclusCH);
                        maxmoduleMULT.push_back(maxclusMult);
                        cleanedmoduleCHRG.push_back(cleanedclusCH);
                        cleanedmoduleMULT.push_back(cleanedclusMult);
                        cleanedmaxmoduleCHRG.push_back(cleanedmaxclusCH);
                        //delete oneHIP;
                    }
                    //allBaselines->SetBinContent(eventNrsOUT.at(i), baselines.at(b)); only for one module
                    //pushback and delete Graph
                    HIP =false;
                    HIPpoint = 0;
                    /*uint32_t chrg = 0;
                    uint32_t mul = 0;
                    */

                    //resolution on charge and multiplicity
                    if(i< 100000)
                    {


                //cout << "here12 " << endl;
                        uint32_t chrgDist = 0;
                        uint32_t multDist = 0;
                        uint32_t counterDist = 0;
                        for(std::vector<uint16_t>::iterator it = clusterCen.begin(); it != clusterCen.end(); ++it)
                        {
                            if(*it > 255 && *it < 384)
                            {
                                chrgDist +=  clusterADC.at(counterDist);
                                if(bunchNrsOUT->at(i) <335)
                                chargeUnc2->Fill(clusterADC.at(counterDist));
                                else
                                chargeUnc3->Fill(clusterADC.at(counterDist));
                                //cout << "charge of cluster: " << clusterADC.at(counter) << endl;
                                multDist++;
                            }
                            counterDist++;
                        }
                        multUnc->Fill(multDist);
                        if(multDist != 0)
                            chargeUnc->Fill(chrgDist/multDist);
                    }
                }
                

                if(baselines.at(b) > (min + 2*(-min+min2)) && baselines.at(b) < (min + 50 + 2*(-min+min2)))
                {//@MJ@ TODO later trigger on HIP
                    ofHIP2++;
                    HIP2 = true;
                    HIPpoint2 = 1;
                    prevBunch2 = bunch;
                    dbx = -8.997*log((71.05-baselines.at(b))/303.7);//@MJ@ TODO recompute
                    if(dbx < boffDown)
                        boffDown = dbx;
                    if(dbx > boffUp)
                        boffUp = dbx;
                    //myfile << "dbx " << dbx << endl;          
                    oneHIP2 = TH1F("baseline after HIP", "baseline after HIP", 2*lTrain, -lTrain ,lTrain);
                    oneHIP2.SetBinContent(dbx+lTrain, baselines.at(b));
                    //set baseline from previous point
                    if(i>0 && bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) > 0 && bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1) < lTrain)
                        oneHIP2.SetBinContent(lTrain+dbx-(bunchNrsOUT->at(i)-bunchNrsOUT->at(i-1))+1, baselinesMtxOUT->at(i-1).at(b));
                    oneHIP2.SetMinimum(-200);
                    oneHIP2.SetMaximum(500);
                    
                    //evfile << moduleKeyOUT << " " << eventNrsOUT->at(i) << " " << "HIP" << endl;  //I need to save module ID!!!!
              
                    }
                    
                }
                else if(HIP2==true && deltaBunch2 < lTrain)
                {
                    HIPpoint2++;
                    oneHIP2.SetBinContent(deltaBunch2+dbx+1+lTrain, baselines.at(b));
               
                }
                else
                {

                    //if(baselines.at(b) < -210)
                         //myfile << "some nonsense in here" << endl;
                    if(HIPpoint2 > 0)
                    {
                        
                        moduleHIP2.push_back(oneHIP2);
                        //delete oneHIP;
                    }
                    //allBaselines->SetBinContent(eventNrsOUT.at(i), baselines.at(b)); only for one module
                    //pushback and delete Graph
                    HIP2 =false;
                    HIPpoint2 = 0;

                }

                //histos

                //histos.baselineHist->SetBinContent(i*baselines.size()+b, baselines.at(b));//+b*10);
            }
        }
        double eff = ofHIP/total;
        if(eff !=  0)
        {
                //cout << "here14 " << endl;
        myfile << "peak: " << peak << " multTot: " << multTot << endl; 
        HIPf->SetBinContent(moduleValueOUT, eff);
        boffD->SetBinContent(moduleValueOUT, boffDown);
        boffU->SetBinContent(moduleValueOUT, boffUp);
        nHIP->SetBinContent(moduleValueOUT, ofHIP);
        nHIP2->SetBinContent(moduleValueOUT, ofHIP2);

        float blMean = blPerMod.GetMean();
        float blMeanHIP = blPerModHIP.GetMean();
        meanBlPerMod->SetBinContent(moduleValueOUT, blMean);
        meanBlPerModHIP->SetBinContent(moduleValueOUT, blMeanHIP);
        meanBlPerModVsfHIP->Fill(blMean, eff);
        meanBlPerModVsfHIPHIP->Fill(blMeanHIP, eff);

        RMSdivisionPOST->Divide(RMSupTO2p5POST, RMSmoreTHAN2p5POST);
        RMSdivisionallPOST->Divide(RMSupTO2p5POST, RMSrestPOST);
        saturation->Divide(saturCH,nonsaturCH);
        if(multTot != 0)
        {
            double fpk = ((double)peak)/((double)multTot);
            myfile << "fpk: " << fpk << endl; 
            fPeak->SetBinContent(moduleValueOUT, fpk ); //@MJ@ TODO this is wrong!
        }
        if(multTotBef != 0)
        {
            double fpkBef = ((double)peakBef)/((double)multTotBef);
            myfile << "fpkBef: " << fpkBef << endl; 
            fPeakBefore->SetBinContent(moduleValueOUT, fpkBef);
        }
        //HIPboff->SetBinContent(m+1, boff);
        //myfile << "ofHIP " << ofHIP <<" total " << total << " fraction" << eff << endl;
        actMod++;
        } 
    //}
        //std::myfile << "event: " << eventNr << " time: " << timeT << std::endl;
    }



    TString rootOut = inputFle+"_analysis.root";
    TFile fi(rootOut,"RECREATE");

   /* TCanvas* cHIP = new TCanvas("CHIP", "cHIP");
    cHIP->cd();
    for(uint32_t v = 0 ; v<moduleHIP.size(); v++)
    {
        
        //std::cout << "drawing CHIP" << endl;
        if(v==0)
        {
           
           moduleHIP.at(v).SetTitle("");
           moduleHIP.at(v).GetYaxis()->SetTitle("baseline[ADC]");
           moduleHIP.at(v).GetXaxis()->SetTitle("bunch crossing");
           moduleHIP.at(v).Draw("hist e");
        }
        else
           moduleHIP.at(v).Draw("hist same e");
           
    }
    cHIP->Write(); */
    
   /* TCanvas* cHIP2 = new TCanvas("CHIP2", "cHIP2");
    cHIP2->cd();
    for(uint32_t v2 = 0 ; v2<moduleHIP2.size(); v2++)
    {
        
        //std::cout << "drawing CHIP" << endl;
        if(v2==0)
        {
           
           moduleHIP2.at(v2).SetTitle("");
           moduleHIP2.at(v2).GetYaxis()->SetTitle("baseline[ADC]");
           moduleHIP2.at(v2).GetXaxis()->SetTitle("bunch crossing");
           moduleHIP2.at(v2).Draw("hist e");
        }
        else
           moduleHIP2.at(v2).Draw("hist same e");
           
    }
    cHIP2->Write();*/
    cout << "drawing" << endl;
    /*TCanvas* cHIP2 = new TCanvas("CHIP2", "cHIP2");
    cHIP2->cd();
    for(uint32_t v2 = 0 ; v2<moduleHIP2.size(); v2++)
    {
        
        //std::cout << "drawing CHIP" << endl;
        if(v2==0)
           moduleHIP2.at(v2)->Draw("hist");
        else
           moduleHIP2.at(v2)->Draw("hist same");
           
    }
    cHIP2->Write();
    */

    TCanvas* cHIPa = new TCanvas("CHIPa", "cHIPa");
    cHIPa->cd();
    float binContent = 0;
    uint32_t binEntry = 0;
    uint32_t bb = 0;
    TH1F* HIPaver = new TH1F("baseline average", "baseline average",2*lTrain , -lTrain , lTrain ); //@MJ@ TODO do this better!!!!
    TH1F* HIPa = new TH1F("HIPa", "HIPa", 10 ,0 ,11 ); //@MJ@ TODO do this better!!!!

    TF1 *myfit = new TF1("myfit","[0]-exp(-x/[1])", 0, 11);

    uint32_t ne = 0;
    if(moduleHIP.size() != 0)
    {
    for(uint32_t b = 0; b<moduleHIP.at(0).GetNbinsX(); b++ )
    {
        for(uint32_t hi = 0 ; hi<moduleHIP.size(); hi++)
        {
        
            if( moduleHIP.at(hi).GetBinContent(b+1) != 0)
            {
                binContent += moduleHIP.at(hi).GetBinContent(b+1);
                binEntry++;
            }
           
        }

        float diff = binContent/binEntry;
        float ediff = 10/sqrt(binEntry);
        //cout << "bin content: " << binContent << " ,binEntry: " << binEntry << " diff: " << diff<< " ediff: " << ediff<< endl; 
        if(binEntry != 0)
        {
            HIPaver->SetBinContent(b+1, diff);
            HIPaver->SetBinError(b+1, 10/sqrt(binEntry));

            if(b>2 && b<14)
            {
                bb++;
                HIPa->SetBinContent(bb, diff);
                HIPa->SetBinError(bb, 10/sqrt(binEntry));
                ne++;
                
            }
        }
        binEntry = 0;
        binContent = 0;
    }
    }

    TGraph *g = new TGraph(ne);
    uint32_t t = 0;
    for(uint32_t xx = 0; xx<HIPa->GetNbinsX(); xx++ )
    {
        t++;
        if(HIPa->GetBinContent(xx+1) != 0)
            g->SetPoint(t-1, xx+1, HIPa->GetBinContent(xx+1));
    }
    
    TH1F* HIPcharge = new TH1F("clus charge", "clus charge", 2*lTrain ,-lTrain ,lTrain ); //@MJ@ TODO do this better!!!!
    TH1F* HIPmult = new TH1F("clus mult", "clus mult", 2*lTrain ,-lTrain ,lTrain ); //@MJ@ TODO do this better!!!!
    TH1F* maxHIPcharge = new TH1F("max clus charge", "max clus charge", 2*lTrain ,-lTrain ,lTrain ); //@MJ@ TODO do this better!!!!
    TH1F* maxHIPmult = new TH1F("max clus mult", "max clus mult", 2*lTrain ,-lTrain ,lTrain ); //@MJ@ TODO do this better!!!!
    TH1F* cleanedHIPcharge = new TH1F("cleanedclus charge", "cleanedclus charge", 2*lTrain ,-lTrain ,lTrain ); //@MJ@ TODO do this better!!!!
    TH1F* cleanedHIPmult = new TH1F("cleanedclus mult", "cleanedclus mult", 2*lTrain ,-lTrain ,lTrain ); //@MJ@ TODO do this better!!!!
    TH1F* cleanedmaxHIPcharge = new TH1F("cleanedmax clus charge", "cleanedmax clus charge", 2*lTrain ,-lTrain ,lTrain ); //@MJ@ TODO do this better!!!!
    float binContentCH = 0;
    float binContentMULT = 0;
    float maxbinContentCH = 0;
    float maxbinContentMULT = 0;
    uint32_t binEntryM = 0;
    uint32_t binEntryCH = 0;
    uint32_t cleanedbinEntryM = 0;
    uint32_t cleanedbinEntryCH = 0;
    float cleanedbinContentCH = 0;
    float cleanedbinContentMULT = 0;
    float cleanedmaxbinContentCH = 0;
    float binContentCHERR = 0;
    float binContentMULTERR = 0;
    if(moduleCHRG.size() != 0)
    {
    for(uint32_t b = 0; b<moduleCHRG.at(0).GetNbinsX(); b++ )
    {

        //myfile << "b: " << b+1 << endl;
        for(uint32_t hi = 0 ; hi<moduleCHRG.size(); hi++)
        {
        
            if( moduleCHRG.at(hi).GetBinContent(b+1) != 0)
            {
                if(moduleCHRG.at(hi).GetBinContent(b+1) != -1)
                {
                    binContentCH += moduleCHRG.at(hi).GetBinContent(b+1);
                    binContentMULT += moduleMULT.at(hi).GetBinContent(b+1);
                    maxbinContentCH += maxmoduleCHRG.at(hi).GetBinContent(b+1);
                    maxbinContentMULT += maxmoduleMULT.at(hi).GetBinContent(b+1);
                    binEntryCH++;
                }
                binEntryM++; //all baselines
            }
           
        }
        for(uint32_t hii = 0 ; hii<cleanedmoduleCHRG.size(); hii++)
        {
        
            if( cleanedmoduleCHRG.at(hii).GetBinContent(b+1) != 0)
            {
                if(cleanedmoduleCHRG.at(hii).GetBinContent(b+1) != -1)
                {
                    cleanedbinContentCH += cleanedmoduleCHRG.at(hii).GetBinContent(b+1);
                    cleanedbinContentMULT += cleanedmoduleMULT.at(hii).GetBinContent(b+1);
                    cleanedmaxbinContentCH += cleanedmaxmoduleCHRG.at(hii).GetBinContent(b+1);
                    cleanedbinEntryCH++;
                }
                cleanedbinEntryM++; //all baselines
            }
           
        }


        float diffCH = binContentCH/binEntryCH;
        float diffMULT = binContentMULT/binEntryM;
        float maxdiffCH = maxbinContentCH/binEntryCH;
        float maxdiffMULT = maxbinContentMULT/binEntryM;
        float cleaneddiffCH = cleanedbinContentCH/cleanedbinEntryCH;
        float cleaneddiffMULT = cleanedbinContentMULT/cleanedbinEntryM;
        float cleanedmaxdiffCH = cleanedmaxbinContentCH/cleanedbinEntryCH;
        //float ediffCH = 10/sqrt(binEntry);
        //out << "bin content: " << binContent << " ,binEntry: " << binEntry << " diff: " << diff<< " ediff: " << ediff<< endl; 
        if(binEntryCH != 0)
        {
            //myfile << "blcont " << blcont << " blerr " << blerr << "rel err: " << relerr << endl;

            HIPcharge->SetBinContent(b+1, diffCH);
            HIPcharge->SetMinimum(0);
            float chrgcont =  HIPcharge->GetBinContent(b+1);
            HIPcharge->SetBinError(b+1, (331)/sqrt(binEntryCH));
            
            maxHIPcharge->SetBinContent(b+1, maxdiffCH);
            maxHIPcharge->SetMinimum(0);
            maxHIPcharge->SetBinError(b+1, (331)/sqrt(binEntryCH));
            //float blcont = HIPcharge->GetBinContent(b+1);
            //loat blerr = HIPcharge->GetBinError(b+1);
            //float relerr = abs(blerr/blcont);

            HIPmult->SetBinContent(b+1, diffMULT); //fix this!!!!
            float multcont = HIPmult->GetBinContent(b+1);
            HIPmult->SetMinimum(0); 
            HIPmult->SetBinError(b+1, (1)/sqrt(binEntryM));
            maxHIPmult->SetBinContent(b+1, maxdiffMULT); //fix this!!!!
            maxHIPmult->SetMinimum(0); 
            maxHIPmult->SetBinError(b+1, (1)/sqrt(binEntryM));
            //HIPaver->SetBinError(b+1, 10/sqrt(binEntry));
        }
        if(cleanedbinEntryCH != 0)
        {
            cleanedHIPcharge->SetBinContent(b+1, cleaneddiffCH);
            cleanedHIPcharge->SetMinimum(0);
            cleanedHIPcharge->SetBinError(b+1, (331)/sqrt(cleanedbinEntryCH));
            
            cleanedmaxHIPcharge->SetBinContent(b+1, cleanedmaxdiffCH);
            cleanedmaxHIPcharge->SetMinimum(0);
            cleanedmaxHIPcharge->SetBinError(b+1, (331)/sqrt(cleanedbinEntryCH));

            cleanedHIPmult->SetBinContent(b+1, cleaneddiffMULT); //fix this!!!!
            cleanedHIPmult->SetMinimum(0); 
            cleanedHIPmult->SetBinError(b+1, (1)/sqrt(cleanedbinEntryM));

        }
        binEntryCH = 0;
        binEntryM = 0;
        binContentCH = 0;
        binContentMULT = 0;
        maxbinContentCH = 0;
        maxbinContentMULT = 0;
        binContentCHERR = 0;
        binContentMULTERR = 0;
        cleanedbinEntryM = 0;
        cleanedbinEntryCH = 0;
        cleanedbinContentCH = 0;
        cleanedbinContentMULT = 0;
        cleanedmaxbinContentCH = 0;
    }
    }
    //HIPa->Fit("myfit");
    //g->Fit("myfit");
    //HIPa->Draw();
    //g->Draw();
    //myfit->Draw("same");
    //cHIPa->Write();
    
    HIPaver->SetTitle("");
    HIPaver->GetYaxis()->SetTitle("baseline[ADC]");
    HIPaver->GetXaxis()->SetTitle("bunch crossing");
    HIPaver->Write();
    graph->Draw("*");
    graph->Write();
    graph2->Draw("*");
    graph2->Write();

    HIPcharge->SetTitle("");
    HIPcharge->GetYaxis()->SetTitle("cluster charge[ADC]");
    HIPcharge->GetXaxis()->SetTitle("bunch crossing");
    HIPcharge->Write();

    maxHIPcharge->SetTitle("");
    maxHIPcharge->GetYaxis()->SetTitle("max cluster charge[ADC]");
    maxHIPcharge->GetXaxis()->SetTitle("bunch crossing");
    maxHIPcharge->Write();

    cleanedHIPcharge->SetTitle("");
    cleanedHIPcharge->GetYaxis()->SetTitle("cluster charge");
    cleanedHIPcharge->GetXaxis()->SetTitle("bunch crossing");
    cleanedHIPcharge->Write();

    cleanedmaxHIPcharge->SetTitle("");
    cleanedmaxHIPcharge->GetYaxis()->SetTitle("max cluster charge");
    cleanedmaxHIPcharge->GetXaxis()->SetTitle("bunch crossing");
    cleanedmaxHIPcharge->Write();


    HIPmult->SetTitle("");
    HIPmult->GetYaxis()->SetTitle("cluster multiplicity");
    HIPmult->GetXaxis()->SetTitle("bunch crossing");
    HIPmult->Write();

    maxHIPmult->SetTitle("");
    maxHIPmult->GetYaxis()->SetTitle("max cluster multiplicity");
    maxHIPmult->GetXaxis()->SetTitle("bunch crossing");
    maxHIPmult->Write();
    

    
    cleanedHIPmult->SetTitle("");
    cleanedHIPmult->GetYaxis()->SetTitle("cluster multiplicity");
    cleanedHIPmult->GetXaxis()->SetTitle("bunch crossing");
    cleanedHIPmult->Write();
    

    chargeUnc->Write();

    chargeUnc2->Write();
    chargeUnc3->Write();
    chargePOST->Write();
    multUnc->Write();
    HIPf->Write();
    nHIP->Write();
    nHIP2->Write();
    boffD->Write();
    boffU->Write();
    //blBefore->Write("hist e");
    satBefore->Write();
    fPeak->Write();
    fPeakBefore->Write();

    baseline2D->Write();
    baselineP->Write();
    baselineVsRMSall->Write();
    baselineVsRMSPRE->Write();
    baselineVsRMSHIP->Write();
    baselineVsRMSPOST->Write();
    PbaselineVsRMSall->Write();
    PbaselineVsRMSPRE->Write();
    PbaselineVsRMSHIP->Write();
    PbaselineVsRMSPOST->Write();
    for(uint32_t k = 0; k< baselineVsRMSallPerMod.size(); k++)
    {
        baselineVsRMSallPerMod.at(k)->Write();
    }
    RMS->Write();
    RMSall->Write();
    BXvsRMS->Write();
    BXvsCH->Write();
    BXvsCHsel1->Write();
    BXvsCHsel2->Write();
    BXvsMaxCH->Write();
    WIvsCH->Write();
    BXvsWI->Write();
    PBXvsRMS->Write();
    PBXvsCH->Write();
    PBXvsCHsel1->Write();
    PBXvsCHsel2->Write();
    PBXvsMaxCH->Write();
    PWIvsCH->Write();
    PBXvsWI->Write();
    RMSvsCHall->Write(); //@MJ@ TODO total cluster charge!!!
    RMSvsCMULTall->Write();
    RMSvsSTall->Write();
    PRMSvsCHall->Write(); //@MJ@ TODO total cluster charge!!!
    PRMSvsCMULTall->Write();
    PRMSvsSTall->Write();
    RMSvsCHPRE->Write(); //@MJ@ TODO total cluster charge!!!
    RMSvsCHHIP->Write(); //@MJ@ TODO total cluster charge!!!
    RMSvsCHPOST->Write(); //@MJ@ TODO total cluster charge!!!
    RMSvsCMULTPRE->Write();
    RMSvsCMULTHIP->Write();
    RMSvsCMULTPOST->Write();
    RMSvsSTPRE->Write();
    RMSvsSTHIP->Write();
    RMSvsSTPOST->Write();
    PRMSvsCHPRE->Write(); //@MJ@ TODO total cluster charge!!!
    PRMSvsCHHIP->Write(); //@MJ@ TODO total cluster charge!!!
    PRMSvsCHPOST->Write(); //@MJ@ TODO total cluster charge!!!
    PRMSvsCMULTPRE->Write();
    PRMSvsCMULTHIP->Write();
    PRMSvsCMULTPOST->Write();
    PRMSvsSTPRE->Write();
    PRMSvsSTHIP->Write();
    PRMSvsSTPOST->Write();
    BXvsCHall->Write();
    PBXvsCHall->Write();
    BXvsCHall2->Write();
    PBXvsCHall2->Write();
    baselineVsMaxCHPRE->Write();
    baselineVsMaxCHHIP->Write();
    baselineVsMaxCHPOST->Write();
    PbaselineVsMaxCHPRE->Write();
    PbaselineVsMaxCHHIP->Write();
    PbaselineVsMaxCHPOST->Write();
    RMSHIPvsRMSPOST->Write();
    PRMSHIPvsRMSPOST->Write();

    //SATvsNSATclusHIP->Write();
    maxCHHIP->Write();
    CHdistrPRE->Write();
    CHdistrHIP->Write();
    CHdistrPOST->Write();
    CHdistrSTlow->Write();
    CHdistrSTmed->Write();
    CHdistrSThig->Write();


/*                                stripsBZvsCHPRE->Write();
                                PstripsBZvsCHPRE->Write();
                                stripsBZvsMaxCHPRE->Write();
                                PstripsBZvsMaxCHPRE->Write();
                                stripsBZvsMULTPRE->Write(); 
                                PstripsBZvsMULTPRE->Write();
                                stripsAZvsCHPRE->Write();
                                PstripsAZvsCHPRE->Write();
                                stripsAZvsMaxCHPRE->Write();
                                PstripsAZvsMaxCHPRE->Write();
                                stripsAZvsMULTPRE->Write(); 
                                PstripsAZvsMULTPRE->Write();
                                slopevsCHPRE->Write();
                                PslopevsCHPRE->Write();
                                slopevsMaxCHPRE->Write();
                                PslopevsMaxCHPRE->Write();
                                slopevsMULTPRE->Write(); 
                                PslopevsMULTPRE->Write();
 
                                stripsBZvsCHHIP->Write();
                                PstripsBZvsCHHIP->Write();
                                stripsBZvsMaxCHHIP->Write();
                                PstripsBZvsMaxCHHIP->Write();
                                stripsBZvsMULTHIP->Write(); 
                                PstripsBZvsMULTHIP->Write();
                                stripsAZvsCHHIP->Write();
                                PstripsAZvsCHHIP->Write();
                                stripsAZvsMaxCHHIP->Write();
                                PstripsAZvsMaxCHHIP->Write();
                                stripsAZvsMULTHIP->Write(); 
                                PstripsAZvsMULTHIP->Write();
                                slopevsCHHIP->Write();
                                PslopevsCHHIP->Write();
                                slopevsMaxCHHIP->Write();
                                PslopevsMaxCHHIP->Write();
                                slopevsMULTHIP->Write(); 
                                PslopevsMULTHIP->Write();

                                stripsBZvsCHPOST->Write();
                                PstripsBZvsCHPOST->Write();
                                stripsBZvsMaxCHPOST->Write();
                                PstripsBZvsMaxCHPOST->Write();
                                stripsBZvsMULTPOST->Write(); 
                                PstripsBZvsMULTPOST->Write();
                                stripsAZvsCHPOST->Write();
                                PstripsAZvsCHPOST->Write();
                                stripsAZvsMaxCHPOST->Write();
                                PstripsAZvsMaxCHPOST->Write();
                                stripsAZvsMULTPOST->Write(); 
                                PstripsAZvsMULTPOST->Write();
                                slopevsCHPOST->Write();
                                PslopevsCHPOST->Write();
                                slopevsMaxCHPOST->Write();
                                PslopevsMaxCHPOST->Write();
                                slopevsMULTPOST->Write(); 
                                PslopevsMULTPOST->Write();



                        RMSbotvsStripsBZ->Write();
                        PRMSbotvsStripsBZ->Write();
                        RMSbotvsStripsAZ->Write();
                        PRMSbotvsStripsAZ->Write();*/
                        RMSbotvsSlope->Write();
                        PRMSbotvsSlope->Write();
                        //PRMSbotvsStripsAZ->Write();

     oneClusCHHIP->Write();
     twoClusCHHIP->Write();
     multBXmin3->Write();
     multBXmin10->Write();
     CHBXmin3->Write();
     CHBXmin25->Write();
        
     meanBlPerMod->Write();
     meanBlPerModVsfHIP->Write();

     meanBlPerModHIP->Write();
     meanBlPerModVsfHIPHIP->Write();

     RMSupTO2p5POST->Write();
     RMSmoreTHAN2p5POST->Write();
     RMSrestPOST->Write();
     RMSdivisionPOST->Write();
     RMSdivisionallPOST->Write();

     BXvsRMSCHm600->Write();
     PBXvsRMSCHm600->Write();
     BXvsRMSCHl600->Write();
     PBXvsRMSCHl600->Write();

    saturCH->Write();
    nonsaturCH->Write();
    saturation->Write();

    slopevsCPHIP->Write();
    PslopevsCPHIP->Write();
    slopevsCPPOST->Write();
    PslopevsCPPOST->Write();

    BXvsBZ->Write();
    PBXvsBZ->Write();
    BXvsAZ->Write();
    PBXvsAZ->Write();

   BXvsRMSbot->Write();
   PBXvsRMSbot->Write();

   stripsBZps->Write();
   stripsAZcmns->Write();
    BXvsCH2->Write();
    PBXvsCH2->Write();
    cleanedBXvsCH2->Write();
    cleanedPBXvsCH2->Write();

    for(int32_t c = 0; c< satBlPerMod.size(); c++)
    {
        satBlPerMod.at(c)->Write();
    }
    PsatBlPerMod->Write();




    myfile.close();
    evfile.close();

}




