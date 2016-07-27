#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream> 
#include <cmath>
#include <exception>

#include "TNtuple.h"
#include "TROOT.h"
#include "TString.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TF1.h"
#include "TF2.h"
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

    if(argc != 3)
        throw std::runtime_error("Bad number of arguments!");

    TString input = argv[1];
    string smodule = argv[2];
    uint32_t module = stoi(smodule);

    vector<TString> in_stream;
    
    ifstream is;
    is.open(input);
    if( is.is_open() )
    {
        //char * sline;
        TString tsline("");
        Int_t line = 0;
        string strline;
        while(getline(is, strline)) {
            tsline = strline;
            if(tsline.BeginsWith(" ")) continue;
            if(tsline.Length()==0)     continue;
            in_stream.push_back(tsline);
            line++;
        }
        is.close();
    }

    //uint32_t nrOfVars = in_stream.at(0).Atof(); example!
    //files to write in
    ofstream myfile;
    myfile.open ("eventtnrs.txt");
    TFile *sEventFile = NULL;
    TTree *sEventTree = NULL;

    //push back files  //@MJ@ TODO read files from steering and open them on DPM
    vector<TFile*> f;
    for(uint32_t in=0; in< in_stream.size(); in++)
    {
        cout << "reading files: " << in_stream.at(in) << endl;
        f.push_back(TFile::Open(in_stream.at(in)));
    }

    //Tree to be read from
    TTree* t = NULL;

    //define variables to be read from tree
    vector<unsigned int>  *moduleKeys = NULL; //@MJ@ TODO why pointer????
    vector<unsigned int>  *moduleValues = NULL;
    vector< vector<float> > *baselinesMtx = NULL;
    vector< vector<uint16_t> > *chargeOfClustersMtx = NULL;
    vector< vector<uint16_t> > *centerOfClustersMtx = NULL;
    //vector< vector<uint16_t> > *saturStripsMtx = NULL;
    //vector< vector<uint16_t> > *ADCOfSaturStripsMtx = NULL;
    //vector< vector<uint8_t> >  *flatBl = NULL;
    ULong64_t timeT; 
    uint32_t bunchNr; 
    uint32_t eventNr; 
    
    //define intermediate variables
    struct intermediate
    {
        vector<unsigned int>  moduleKeysINTER;
        vector<unsigned int>  moduleValuesINTER;
        vector< vector<float> > baselinesMtxINTER;
        vector< vector<uint16_t> > chargeOfClustersMtxINTER;
        vector< vector<uint16_t> > centerOfClustersMtxINTER;
        //vector< vector<uint16_t> > saturStripsMtxINTER;
        //vector< vector<uint16_t> > ADCOfSaturStripsMtxINTER;
        //vector< vector<uint8_t> >  flatBlINTER;
        ULong64_t timeTINTER; 
        uint32_t bunchNrINTER; 
        //uint32_t eventNrINTER;
    } data;
 
    //define output variables
    vector<unsigned int>  moduleKeysOUT;
    vector<unsigned int>  moduleValuesOUT;
    vector< vector<float> > baselinesMtxOUT;
    vector< vector<uint16_t> > chargeOfClustersMtxOUT;
    vector< vector<uint16_t> > centerOfClustersMtxOUT;
    //vector< vector<uint16_t> > saturStripsMtxOUT;
    //vector< vector<uint16_t> > ADCOfSaturStripsMtxOUT;
    //vector< vector<uint8_t> >  flatBlOUT;
    ULong64_t timeTOUT; 
    uint32_t bunchNrOUT; 
    uint32_t eventNrOUT; 

    //other variables
    //vector<uint32_t> sortedEvents;
    std::map< uint32_t, intermediate > eventMap;
    //std::map< uint32_t, uint32_t > eventOrder;
    //uint32_t eventPosition = 0; 
   
    //loop over files
    for(uint32_t file=0; file<f.size(); file++)
    {
        cout << "first map filling" << " file " << file << endl;

        //read tree
        t = dynamic_cast<TTree*>(f.at(file)->Get("baselineAna/timeTree"));
        
        t->SetBranchAddress("moduleKeys",&moduleKeys);
        t->SetBranchAddress("moduleValues",&moduleValues);
        t->SetBranchAddress("baselinesMtx",&baselinesMtx);
        t->SetBranchAddress("chargeOfClustersMtx",&chargeOfClustersMtx);
        t->SetBranchAddress("centerOfClustersMtx",&centerOfClustersMtx);
        //t->SetBranchAddress("saturStripsMtx",&saturStripsMtx);
        //t->SetBranchAddress("ADCOfSaturStripsMtx",&ADCOfSaturStripsMtx);
        //t->SetBranchAddress("flatBl",&flatBl);
        t->SetBranchAddress("timeT",&timeT);
        t->SetBranchAddress("bunchNr",&bunchNr);
        t->SetBranchAddress("eventNr",&eventNr);

        Int_t nentries = (Int_t)t->GetEntries();
    
        //loop over events    
        for (Int_t i=0; i<nentries; i++) 
        {
            data.moduleKeysINTER.clear();
            data.moduleValuesINTER.clear();
            data.baselinesMtxINTER.clear();
            data.chargeOfClustersMtxINTER.clear();
            data.centerOfClustersMtxINTER.clear();
            t->GetEntry(i);
            //if no HV continue
            if(moduleKeys->size() == 0)
                continue;
                     
            data.moduleKeysINTER.push_back(moduleKeys->at(module)) ;
            data.moduleValuesINTER.push_back(moduleValues->at(module));
            data.baselinesMtxINTER.push_back(baselinesMtx->at(module));
            data.chargeOfClustersMtxINTER.push_back(chargeOfClustersMtx->at(module));
            data.centerOfClustersMtxINTER.push_back(centerOfClustersMtx->at(module));
            //data.saturStripsMtxINTER.push_back(saturStripsMtx->at(module));
            //data.ADCOfSaturStripsMtxINTER.push_back(ADCOfSaturStripsMtx->at(module));
            //data.flatBlINTER.push_back(flatBl->at(module));
            data.timeTINTER = timeT; 
            data.bunchNrINTER = bunchNr; 
         
             //@MJ@ TODO obsolete now
             //fill and sort vector of event nr
             //sortedEvents.push_back(eventNr);
    
             //map of: eventnr,file,positoon in file
             eventMap.insert(make_pair(eventNr, data));
            myfile << "eventNrIn: " << eventNr << " time: " << timeT << endl;

        }
        //clear the tree
        t->Delete();
        t = NULL;
    }
    
    //how many events?
    //uint32_t mapSize = eventMap.size();
    

    //open output root file
    sEventFile = new TFile("sEventFile_All.root","RECREATE" );
    sEventTree = new TTree("sEventTree", "sEventTree"); 
   
    sEventTree->Branch("moduleKeysOUT", "std::vector<uint32_t>", &moduleKeysOUT);
    sEventTree->Branch("moduleValuesOUT", "std::vector<uint32_t>", &moduleValuesOUT);
    sEventTree->Branch("baselinesMtxOUT", "std::vector< std::vector<float> >", &baselinesMtxOUT);
    //sEventTree->Branch("saturStripsMtxOUT", "std::vector< std::vector<uint16_t> >", &saturStripsMtxOUT);
    //sEventTree->Branch("ADCOfSaturStripsMtxOUT", "std::vector< std::vector<uint16_t> >", &ADCOfSaturStripsMtxOUT);
    sEventTree->Branch("chargeOfClustersMtxOUT", "std::vector < std::vector<uint16_t> >", &chargeOfClustersMtxOUT);
    sEventTree->Branch("centerOfClustersMtxOUT", "std::vector < std::vector<uint16_t> >", &centerOfClustersMtxOUT);
    //sEventTree->Branch("flatBlOUT", "std::vector < std::vector<uint8_t> >", &flatBlOUT);
    sEventTree->Branch("timeTOUT", &timeTOUT, "timeTOUT/l");
    sEventTree->Branch("eventNrOUT", &eventNrOUT, "eventNrOUT/i");
    sEventTree->Branch("bunchNrOUT", &bunchNrOUT, "bunchNrOUT/i");
  

        cout << "tree filling" << endl;
 

    for (std::map< uint32_t, intermediate >::iterator it=eventMap.begin(); it!=eventMap.end(); ++it)
    {
         
        moduleKeysOUT = it->second.moduleKeysINTER;
        moduleValuesOUT = it->second.moduleValuesINTER;
        baselinesMtxOUT = it->second.baselinesMtxINTER;
        chargeOfClustersMtxOUT = it->second.chargeOfClustersMtxINTER;
        centerOfClustersMtxOUT = it->second.centerOfClustersMtxINTER;
        //saturStripsMtxOUT = saturStripsMtxINTER.at(out);
        //ADCOfSaturStripsMtxOUT = ADCOfSaturStripsMtxINTER.at(out);
        //flatBlOUT = flatBlINTER.at(out);
        timeTOUT = it->second.timeTINTER;
        bunchNrOUT = it->second.bunchNrINTER;
        eventNrOUT = it->first;

        myfile << "eventNrOut: " << eventNrOUT << " time: " << timeTOUT << endl;

        sEventTree->Fill();
        
    }
    eventMap.clear();

    sEventFile->Write();

    //@MJ@ TODO obsolete now!!
    //write range of events
    //std::sort (sortedEvents.begin(), sortedEvents.end());

    //for(uint32_t all=0; all< sortedEvents.size(); all++)
    //{
    //    myfile << "event number: " << sortedEvents.at(all) << endl;
    //}
    
    myfile.close();
}




