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

    if(argc != 4)
        throw std::runtime_error("Bad number of arguments!");

    TString input = argv[1];
    string sfrom = argv[2]; 
    string sto = argv[3];
    uint32_t from = stoi(sfrom); 
    uint32_t to = stoi(sto);

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
    vector< vector<uint16_t> > *saturStripsMtx = NULL;
    vector< vector<uint16_t> > *ADCOfSaturStripsMtx = NULL;
    vector< vector<uint8_t> >  *flatBl = NULL;
    ULong64_t timeT; 
    uint32_t bunchNr; 
    uint32_t eventNr; 
    
    //define intermediate variables
    vector< vector<unsigned int> >  moduleKeysINTER;
    vector< vector<unsigned int> >  moduleValuesINTER;
    vector< vector< vector<float> > > baselinesMtxINTER;
    vector< vector< vector<uint16_t> > > chargeOfClustersMtxINTER;
    vector< vector< vector<uint16_t> > > centerOfClustersMtxINTER;
    vector< vector< vector<uint16_t> > > saturStripsMtxINTER;
    vector< vector< vector<uint16_t> > > ADCOfSaturStripsMtxINTER;
    vector< vector< vector<uint8_t> > >  flatBlINTER;
    vector< ULong64_t > timeTINTER; 
    vector< uint32_t > bunchNrINTER; 
    vector< uint32_t > eventNrINTER;
 
    //define output variables
    vector<unsigned int>  moduleKeysOUT;
    vector<unsigned int>  moduleValuesOUT;
    vector< vector<float> > baselinesMtxOUT;
    vector< vector<uint16_t> > chargeOfClustersMtxOUT;
    vector< vector<uint16_t> > centerOfClustersMtxOUT;
    vector< vector<uint16_t> > saturStripsMtxOUT;
    vector< vector<uint16_t> > ADCOfSaturStripsMtxOUT;
    vector< vector<uint8_t> >  flatBlOUT;
    ULong64_t timeTOUT; 
    uint32_t bunchNrOUT; 
    uint32_t eventNrOUT;


    //other variables
    //vector<uint32_t> sortedEvents;
    std::map< uint32_t, std::pair<uint32_t, uint32_t> > eventMap;
    std::map< uint32_t, uint32_t > eventOrder;
    uint32_t eventPosition = 0; 
    int64_t mapPosition = -1; 
   
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
        t->SetBranchAddress("saturStripsMtx",&saturStripsMtx);
        t->SetBranchAddress("ADCOfSaturStripsMtx",&ADCOfSaturStripsMtx);
        t->SetBranchAddress("flatBl",&flatBl);
        t->SetBranchAddress("timeT",&timeT);
        t->SetBranchAddress("bunchNr",&bunchNr);
        t->SetBranchAddress("eventNr",&eventNr);

        Int_t nentries = (Int_t)t->GetEntries();
    
        //loop over events    
        for (Int_t i=0; i<nentries; i++) 
        {
            t->GetEntry(i);
            //if no HV continue
            if(moduleKeys->size() == 0)
                continue;
         
              //@MJ@ TODO obsolete now
             //fill and sort vector of event nr
             //sortedEvents.push_back(eventNr);
    
             //map of: eventnr,file,positoon in file
             eventMap.insert(make_pair(eventNr, make_pair(file,i)));

        }
        //clear the tree
        t->Delete();
        t = NULL;
    }
    
    //how many events?
    
    for(uint32_t file=0; file<f.size(); file++)
    {
        cout << "second map filling" << " file " << file << endl;
       //read from the tree again
        t = dynamic_cast<TTree*>(f.at(file)->Get("baselineAna/timeTree"));
        
        t->SetBranchAddress("moduleKeys",&moduleKeys);
        t->SetBranchAddress("moduleValues",&moduleValues);
        t->SetBranchAddress("baselinesMtx",&baselinesMtx);
        t->SetBranchAddress("chargeOfClustersMtx",&chargeOfClustersMtx);
        t->SetBranchAddress("centerOfClustersMtx",&centerOfClustersMtx);
        t->SetBranchAddress("saturStripsMtx",&saturStripsMtx);
        t->SetBranchAddress("ADCOfSaturStripsMtx",&ADCOfSaturStripsMtx);
        t->SetBranchAddress("flatBl",&flatBl);
        t->SetBranchAddress("timeT",&timeT);
        t->SetBranchAddress("bunchNr",&bunchNr);
        t->SetBranchAddress("eventNr",&eventNr);

        eventPosition = 0;
        mapPosition = -1;
        
        //read rom the map of events, if event in specified file, write its position in file and overall position into other map
        for (std::map< uint32_t, pair<uint32_t, uint32_t> >::iterator it=eventMap.begin(); it!=eventMap.end(); ++it)
        {
            //myfile << "filenr " << file << " map position: " << mapPosition << " BEROFE CUTTING: eventsorted: eventnr: " << it->first << " event file: " << it->second.first << " position in file: " << it->second.second << endl;
            mapPosition++;
            //cut on position in the map (range of events)
            if(mapPosition < from || mapPosition > to)
                continue;
            //myfile << "eventsorted: eventnr: " << it->first << " event file: " << it->second.first << " position in file: " << it->second.second << endl;
            if(it->second.first == file)
            {
                eventOrder.insert(pair<uint32_t,uint32_t>(it->second.second, eventPosition));
            }
            eventPosition++;
        }

        cout << "size of ordered amp: " << eventOrder.size();


        Int_t nentries = (Int_t)t->GetEntries();
        
        cout << "vectors filling now," << " file " << file << endl;
    
        //loop over events    
        for (Int_t i=0; i<nentries; i++) 
        {
             t->GetEntry(i);

             //fill vectors by some values for first file
             if(file == 0 && i == 0)
             {
                 for(uint32_t v= 0; v<(to-from+1); v++)
                 {
                     moduleKeysINTER.push_back( *moduleKeys );
                     moduleValuesINTER.push_back( *moduleValues);
                     baselinesMtxINTER.push_back( *baselinesMtx);
                     chargeOfClustersMtxINTER.push_back( *chargeOfClustersMtx);
                     centerOfClustersMtxINTER.push_back( *centerOfClustersMtx);
                     saturStripsMtxINTER.push_back( *saturStripsMtx);
                     ADCOfSaturStripsMtxINTER.push_back( *ADCOfSaturStripsMtx);
                     flatBlINTER.push_back( *flatBl);
                     timeTINTER.push_back( timeT); 
                     bunchNrINTER.push_back( bunchNr); 
                     eventNrINTER.push_back( eventNr);
                 }
             }

             //if not filled throw exception
             if(eventNrINTER.size() == 0)
                 throw  std::runtime_error("The vectors were not filled!!!");
        
             //find if there is element i and put it on right position into intermediate vector
             //myfile << "BEFORE: order in file: " << i << " total order: " << endl;
             std::map<uint32_t,uint32_t>::iterator it2 = eventOrder.find(i);
             if(it2 != eventOrder.end())
             {
                 //myfile << "order in file: " << i << " total order: " << it2->second << endl;
                 //cout << "event position: " << it2->second << endl;
                 moduleKeysINTER.at(it2->second) =  *moduleKeys;
                 moduleValuesINTER.at(it2->second) = *moduleValues;
                 baselinesMtxINTER.at(it2->second) = *baselinesMtx;
                 chargeOfClustersMtxINTER.at(it2->second) = *chargeOfClustersMtx;
                 centerOfClustersMtxINTER.at(it2->second) = *centerOfClustersMtx;
                 saturStripsMtxINTER.at(it2->second) = *saturStripsMtx;
                 ADCOfSaturStripsMtxINTER.at(it2->second) = *ADCOfSaturStripsMtx;
                 flatBlINTER.at(it2->second) = *flatBl;
                 timeTINTER.at(it2->second) = timeT; 
                 bunchNrINTER.at(it2->second) = bunchNr; 
                 eventNrINTER.at(it2->second) = eventNr;
             }

        }
        //clear
        eventPosition = 0;
        mapPosition = -1;
        eventOrder.clear();
        t->Delete();
        t = NULL;
    }

    eventMap.clear();
    //open output root file
    TString fileName = "sEventFile_" + sfrom + "_" + sto + ".root";
    sEventFile = new TFile(fileName,"RECREATE" );
    sEventTree = new TTree("sEventTree", "sEventTree"); 
   
    sEventTree->Branch("moduleKeysOUT", "std::vector<uint32_t>", &moduleKeysOUT);
    sEventTree->Branch("moduleValuesOUT", "std::vector<uint32_t>", &moduleValuesOUT);
    sEventTree->Branch("baselinesMtxOUT", "std::vector< std::vector<float> >", &baselinesMtxOUT);
    sEventTree->Branch("saturStripsMtxOUT", "std::vector< std::vector<uint16_t> >", &saturStripsMtxOUT);
    sEventTree->Branch("ADCOfSaturStripsMtxOUT", "std::vector< std::vector<uint16_t> >", &ADCOfSaturStripsMtxOUT);
    sEventTree->Branch("chargeOfClustersMtxOUT", "std::vector < std::vector<uint16_t> >", &chargeOfClustersMtxOUT);
    sEventTree->Branch("centerOfClustersMtxOUT", "std::vector < std::vector<uint16_t> >", &centerOfClustersMtxOUT);
    sEventTree->Branch("flatBlOUT", "std::vector < std::vector<uint8_t> >", &flatBlOUT);
    sEventTree->Branch("timeTOUT", &timeTOUT, "timeTOUT/l");
    sEventTree->Branch("eventNrOUT", &eventNrOUT, "eventNrOUT/i");
    sEventTree->Branch("bunchNrOUT", &bunchNrOUT, "bunchNrOUT/i");
  

        cout << "tree filling" << endl;
 
    // iterate through intermediate vectors and save values int tree 
    for(uint32_t out = 0; out < eventNrINTER.size(); out++)
    {
    
        moduleKeysOUT = moduleKeysINTER.at(out);
        moduleValuesOUT = moduleValuesINTER.at(out);
        baselinesMtxOUT = baselinesMtxINTER.at(out);
        chargeOfClustersMtxOUT = chargeOfClustersMtxINTER.at(out);
        centerOfClustersMtxOUT = centerOfClustersMtxINTER.at(out);
        saturStripsMtxOUT = saturStripsMtxINTER.at(out);
        ADCOfSaturStripsMtxOUT = ADCOfSaturStripsMtxINTER.at(out);
        flatBlOUT = flatBlINTER.at(out);
        timeTOUT = timeTINTER.at(out);
        bunchNrOUT = bunchNrINTER.at(out);
        eventNrOUT = eventNrINTER.at(out);

        myfile << "eventNrOut: " << eventNrOUT << " time: " << timeTOUT << endl;

        sEventTree->Fill();
    }

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




