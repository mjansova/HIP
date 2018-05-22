#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream> 
#include <cmath>
#include <exception>
#include <ctime>

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

        cout << "time1: " << time(NULL) << endl;

    if(argc != 6)
        throw std::runtime_error("Bad number of arguments!");

    TString input = argv[1];
    string sfrom = argv[2]; 
    string sto = argv[3];
    string spartition = argv[4];
    string slayer = argv[5];
    uint32_t from = stoi(sfrom); 
    uint32_t to = stoi(sto);
    uint32_t partition = stoi(spartition);
    uint32_t layer = stoi(slayer);
    //uint32_t evCut = 187614;
    uint32_t evCut = 120000;
    //uint32_t evCut = 10;

    
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
    //ofstream myfile;
    //myfile.open ("eventtnrs.txt");
    TFile *sEventFile = NULL;
    TTree *sEventTree = NULL;

    //push back files  //@MJ@ TODO read files from steering and open them on DPM

    //Tree to be read from
   
    TTree* t = NULL;
    TFile* f = NULL;
    uint32_t nrOfFiles = in_stream.size();
    //define variables to be read from tree
    vector<unsigned int>  *moduleKeys = NULL; //@MJ@ TODO why pointer????
    vector<unsigned int>  *moduleValues = NULL;
    vector< vector<float> > *baselinesMtx = NULL;
    vector< vector<uint16_t> > *chargeOfClustersMtx = NULL;
    vector< vector<uint16_t> > *centerOfClustersMtx = NULL;
    vector< vector<uint16_t> > *stripsTouchedOfClustersMtx = NULL;
    vector< vector<uint8_t> > *maxADCOfClustersMtx = NULL;
    vector< vector<float> > *baselineRMSMtx = NULL;
    vector< std::vector<float> >  *baselineCMRMSMtx = NULL;
    vector< std::vector<float> >  *PSSlope20Mtx = NULL;
    vector< std::vector<uint8_t> > *positiveValuesCMNSMtx = NULL;
    vector< std::vector<uint8_t> > *negativeValuesPSMtx = NULL;

    ULong64_t timeT; 
    uint32_t bunchNr; 
    uint32_t eventNr; 
    
    //define intermediate variables
    vector< vector<unsigned int> >  moduleKeysINTER;
    vector< vector<unsigned int> >  moduleValuesINTER;
    vector< vector< vector<float> > > baselinesMtxINTER;
    vector< vector< vector<uint16_t> > > chargeOfClustersMtxINTER;
    vector< vector< vector<uint16_t> > > centerOfClustersMtxINTER;
    vector< vector< vector<uint16_t> > > stripsTouchedOfClustersMtxINTER;
    vector< vector< vector<uint8_t> > > maxADCOfClustersMtxINTER;
    vector< vector< vector<float> > > baselineRMSMtxINTER;
    vector< vector< std::vector<float> > >  baselineCMRMSMtxINTER;
    vector< vector< std::vector<float> > >  PSSlope20MtxINTER;
    vector< vector< std::vector<uint8_t> > > positiveValuesCMNSMtxINTER;
    vector< vector< std::vector<uint8_t> > > negativeValuesPSMtxINTER;
    vector< ULong64_t > timeTINTER; 
    vector< uint32_t > bunchNrINTER; 
    vector< uint32_t > eventNrINTER;
 
    //define output variables
    unsigned int  moduleKeyOUT;
    unsigned int  moduleValueOUT;
    vector< vector<float> > baselinesMtxOUT;
    vector< vector<uint16_t> > chargeOfClustersMtxOUT;
    vector< vector<uint16_t> > centerOfClustersMtxOUT;
    vector< vector<uint16_t> > stripsTouchedOfClustersMtxOUT;
    vector< vector<uint8_t> > maxADCOfClustersMtxOUT;
    vector< vector<float> > baselineRMSMtxOUT;
    vector< std::vector<float> >  baselineCMRMSMtxOUT;
    vector< std::vector<float> >  PSSlope20MtxOUT;
    vector< std::vector<uint8_t> > positiveValuesCMNSMtxOUT;
    vector< std::vector<uint8_t> > negativeValuesPSMtxOUT;
    vector<ULong64_t> timesTOUT; 
    vector<uint32_t> bunchNrsOUT; 
    vector<uint32_t> eventNrsOUT; 

    //other variables
    //vector<uint32_t> sortedEvents;
    std::map< uint32_t, std::pair<uint32_t, uint32_t> > eventMap;
    std::map< uint32_t, uint32_t > eventOrder;
    uint32_t eventPosition = 0; 
    int64_t mapPosition = -1; 
   
    std::ifstream          myfile2("eventMapVR2.txt");

    std::string   myline;
    while(std::getline(myfile2, myline))
    {
        std::vector<uint32_t>   mylineData;
        std::stringstream  mylineStream(myline);

        uint32_t myvalue;

        while(mylineStream >> myvalue)
        {
            mylineData.push_back(myvalue);
        }

        eventMap.insert(make_pair(mylineData.at(0), make_pair(mylineData.at(1),mylineData.at(2))));
        mylineData.clear();
    }  

    
        evCut = evCut > eventMap.size() ? eventMap.size() : evCut;
        //cout << "event cut " << evCut << endl;
    //how many events?
    for(uint32_t file=0; file<nrOfFiles; file++)
    {
        f = TFile::Open(in_stream.at(file)); 
        //cout << "time5: " << time(NULL) << endl;
        cout << "second map filling" << " file " << file << endl;
       //read from the tree again
        t = dynamic_cast<TTree*>(f->Get("baselineAna/timeTree"));
        
        t->SetBranchAddress("moduleKeys",&moduleKeys);
        t->SetBranchAddress("moduleValues",&moduleValues);
        t->SetBranchAddress("baselinesMtx",&baselinesMtx);
        t->SetBranchAddress("chargeOfClustersMtx",&chargeOfClustersMtx);
        t->SetBranchAddress("centerOfClustersMtx",&centerOfClustersMtx);
        t->SetBranchAddress("stripsTouchedOfClustersMtx", &stripsTouchedOfClustersMtx);
        t->SetBranchAddress("maxADCOfClustersMtx", &maxADCOfClustersMtx);
        t->SetBranchAddress("baselineRMSMtx",&baselineRMSMtx);
        t->SetBranchAddress("baselineCMRMSMtx",&baselineCMRMSMtx);
        t->SetBranchAddress("PSSlope20Mtx",&PSSlope20Mtx);
        t->SetBranchAddress("positiveValuesCMNSMtx",&positiveValuesCMNSMtx);
        t->SetBranchAddress("negativeValuesPSMtx",&negativeValuesPSMtx);
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
            //if(mapPosition < from || mapPosition > to)
                //continue;
            //myfile << "eventsorted: eventnr: " << it->first << " event file: " << it->second.first << " position in file: " << it->second.second << endl;
            if(it->second.first == file)
            {
                eventOrder.insert(pair<uint32_t,uint32_t>(it->second.second, eventPosition));
            }
            eventPosition++;
        }

        //cout << "time6: " << time(NULL) << endl;
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
                     /*//moduleKeysINTER.resize(to-from+1);
                     //moduleValuesINTER.push_back( *moduleValues);
                     baselinesMtxINTER.resize(to-from+1);
                     chargeOfClustersMtxINTER.resize(to-from+1);
                     centerOfClustersMtxINTER.resize(to-from+1);
                     //saturStripsMtxINTER.push_back( *saturStripsMtx);
                     //ADCOfSaturStripsMtxINTER.push_back( *ADCOfSaturStripsMtx);
                     timeTINTER.resize(to-from+1); 
                     bunchNrINTER.resize(to-from+1); 
                     eventNrINTER.resize(to-from+1);
                     */
                     
                     moduleKeysINTER.resize(evCut);
                     moduleValuesINTER.resize(evCut);
                     baselinesMtxINTER.resize(evCut);
                     chargeOfClustersMtxINTER.resize(evCut);
                     centerOfClustersMtxINTER.resize(evCut);
                     timeTINTER.resize(evCut); 
                     bunchNrINTER.resize(evCut); 
                     eventNrINTER.resize(evCut);
                     stripsTouchedOfClustersMtxINTER.resize(evCut);
                     maxADCOfClustersMtxINTER.resize(evCut);
                     baselineRMSMtxINTER.resize(evCut);
                     baselineCMRMSMtxINTER.resize(evCut);
                     PSSlope20MtxINTER.resize(evCut);
                     positiveValuesCMNSMtxINTER.resize(evCut);
                     negativeValuesPSMtxINTER.resize(evCut);
                     //myCsSrtripsTouchedMtxINTER.resize(evCut);
                     //myCsSrtripsTouchedMtxINTER.resize(evCut);
                     //myCsChargeMtxINTER.resize(evCut);
                     //myCsMaxChargeMtxINTER.resize(evCut);
                     //myCsCenterMtxINTER.resize(evCut);
             }

             //if not filled throw exception
             if(eventNrINTER.size() == 0)
                 throw  std::runtime_error("The vectors were not filled!!!");
        
             //find if there is element i and put it on right position into intermediate vector
             //myfile << "BEFORE: order in file: " << i << " total order: " << endl;
             std::map<uint32_t,uint32_t>::iterator it2 = eventOrder.find(i);
             if(it2 != eventOrder.end() && it2->second < evCut)
             {
                 //cout << "in here i" << endl;
                 for(uint32_t mod = 0; mod <moduleKeys->size(); mod++)
                 {
                 //cout << "in here ii" << endl;
                    uint32_t position = moduleValues->at(mod) - 1;

                 //cout << "in here iii" << endl;
                    //cout << "subdet id: " << int((moduleKeys->at(position)>>25)&0x7) << " layer: " << int((moduleKeys->at(position)>>14)&0x7) << endl;
                 if(partition == 3 || partition == 5 ) //barrels
                 {
                    if((int((moduleKeys->at(position)>>25)&0x7) != partition) || (int((moduleKeys->at(position)>>14)&0x7) != layer))
                        continue;
                 }
                 else if(partition == 4 )
                 {
                    if((int((moduleKeys->at(position)>>25)&0x7) != partition) || (int((moduleKeys->at(position)>>11)&0x3) != layer))
                        continue;
                 }
                 else if(partition == 6 )
                 {
                    if((int((moduleKeys->at(position)>>25)&0x7) != partition) || (int((moduleKeys->at(position)>>14)&0xF) != layer))
                        continue;
                 }
                 else
                 {
                     continue;
                 }
                 //cout << "in here iv" << endl;

                    static const uint32_t start = position;

                 //cout << "in here v" << endl;
                    if(position - start + 1 < from || position - start +1 > to) //start modules from one, really same amount of modules for everything???
                        continue;
                 //cout << "in here vi" << endl;

                    //myfile << "order in file: " << i << " total order: " << it2->second << endl;
                    //cout << "event position: " << it2->second << endl;
                    //
                 /*
                 moduleKeysINTER.at(it2->second) =  *moduleKeys;
                 moduleValuesINTER.at(it2->second) = *moduleValues;
                 baselinesMtxINTER.at(it2->second) = *baselinesMtx;
                 chargeOfClustersMtxINTER.at(it2->second) = *chargeOfClustersMtx;
                 centerOfClustersMtxINTER.at(it2->second) = *centerOfClustersMtx;
                 saturStripsMtxINTER.at(it2->second) = *saturStripsMtx;
                 ADCOfSaturStripsMtxINTER.at(it2->second) = *ADCOfSaturStripsMtx;
                 flatBlINTER.at(it2->second) = *flatBl;
                 eventNrINTER.at(it2->second) = eventNr;
                 */

                 //myfile << "module position: " << position << " det id: " << moduleKeys->at(position) << endl;
                    //cout << "moduleValuesINTER " << moduleValuesINTER.size() << endl;                    
                    //cout << " moduleValues " << moduleValues->size() << endl;                    
                    //cout << " moduleKeys " << moduleKeys->size() << endl;                    
                    moduleValuesINTER.at(it2->second).push_back(moduleValues->at(position)); //@MJ@ TODO I do not need this that much!
                    moduleKeysINTER.at(it2->second).push_back(moduleKeys->at(position)) ;
                    if( baselinesMtx->size() != 0 )
                    {
			 //cout << "in here b" << endl;
			 //cout << " baselinesMtxINTER " << baselinesMtxINTER.size() << "  baselinesMtx " << baselinesMtx->size()  << endl;                    
			    baselinesMtxINTER.at(it2->second).push_back(baselinesMtx->at(position));
			 //cout << "in here c" << endl;
			    chargeOfClustersMtxINTER.at(it2->second).push_back(chargeOfClustersMtx->at(position));
			 //cout << "in here d" << endl;
			    centerOfClustersMtxINTER.at(it2->second).push_back(centerOfClustersMtx->at(position));
			 //cout << "in here e" << endl;
			    stripsTouchedOfClustersMtxINTER.at(it2->second).push_back(stripsTouchedOfClustersMtx->at(position));
			 //cout << "in here f" << endl;
			    maxADCOfClustersMtxINTER.at(it2->second).push_back(maxADCOfClustersMtx->at(position));
			 //cout << "in here g" << endl;
			    baselineRMSMtxINTER.at(it2->second).push_back(baselineRMSMtx->at(position));
			 //cout << "in here h" << endl;
			    baselineCMRMSMtxINTER.at(it2->second).push_back(baselineCMRMSMtx->at(position));
			 //cout << "in here i" << endl;
			    PSSlope20MtxINTER.at(it2->second).push_back(PSSlope20Mtx->at(position));
			 //cout << "in here j" << endl;
			    positiveValuesCMNSMtxINTER.at(it2->second).push_back(positiveValuesCMNSMtx->at(position));
			 //cout << "in here k" << endl;
			    negativeValuesPSMtxINTER.at(it2->second).push_back(negativeValuesPSMtx->at(position));
                    }
                    //myCsSrtripsTouchedMtxINTER.at(it2->second).push_back(myCsSrtripsTouchedMtx->at(position));
                    //myCsChargeMtxINTER.at(it2->second).push_back(myCsChargeMtx->at(position));
                    //myCsMaxChargeMtxINTER.at(it2->second).push_back(myCsMaxChargeMtx->at(position));
                    //myCsCenterMtxINTER.at(it2->second).push_back(myCsCenterMtx->at(position));
                //data.saturStripsMtxINTER.push_back(saturStripsMtx->at(mod+1ule));
                //data.ADCOfSaturStripsMtxINTER.push_back(ADCOfSaturStripsMtx->at(module));
                //data.flatBlINTER.push_back(flatBl->at(module));
                 }

                 //myfile << "size of modules: " << baselinesMtxINTER.at(it2->second).size() << endl;
                 //cout << "in here l" << endl;
                 timeTINTER.at(it2->second) = timeT; 
                 //cout << "in here m" << endl;
                 bunchNrINTER.at(it2->second) = bunchNr; 
                 //cout << "in here n" << endl;
                 eventNrINTER.at(it2->second) = eventNr;
                 //cout << "in here o" << endl;
             }

        }
        //clear
        eventPosition = 0;
        mapPosition = -1;
        eventOrder.clear();
        t->Delete();
        t = NULL;
        f->Close();
        f->Delete();
        f= NULL;
    }

    eventMap.clear();
    //open output root file
    TString fileName = "/opt/sbg/scratch1/cms/mjansova/store/tmp/outputThesis/sEventFileModules_" + sfrom + "_" + sto + "_part_" + spartition + "_l_" + slayer+ ".root";
    sEventFile = new TFile(fileName,"RECREATE" );
    sEventTree = new TTree("sEventTree", "sEventTree"); 
   
  

     cout << " before tree filling" << endl;
    sEventTree->Branch("moduleKeyOUT", &moduleKeyOUT, "moduleKeyOUT/i");
    sEventTree->Branch("moduleValueOUT", &moduleValueOUT, "moduleValueOUT/i");
    sEventTree->Branch("baselinesMtxOUT", "std::vector< std::vector<float> >", &baselinesMtxOUT);
    //sEventTree->Branch("saturStripsMtxOUT", "std::vector< std::vector<uint16_t> >", &saturStripsMtxOUT);
    //sEventTree->Branch("ADCOfSaturStripsMtxOUT", "std::vector< std::vector<uint16_t> >", &ADCOfSaturStripsMtxOUT);
    sEventTree->Branch("chargeOfClustersMtxOUT", "std::vector < std::vector<uint16_t> >", &chargeOfClustersMtxOUT);
    sEventTree->Branch("centerOfClustersMtxOUT", "std::vector < std::vector<uint16_t> >", &centerOfClustersMtxOUT);
    sEventTree->Branch("stripsTouchedOfClustersMtxOUT", "std::vector < std::vector<uint16_t> >", &stripsTouchedOfClustersMtxOUT);
    sEventTree->Branch("maxADCOfClustersMtxOUT", "std::vector < std::vector<uint8_t> >", &maxADCOfClustersMtxOUT);
    sEventTree->Branch("baselineRMSMtxOUT", "std::vector < std::vector<float> >", &baselineRMSMtxOUT);
    sEventTree->Branch("positiveValuesCMNSMtxOUT", "std::vector< std::vector<uint8_t> >" , &positiveValuesCMNSMtxOUT);
    sEventTree->Branch("negativeValuesPSMtxOUT", "std::vector< std::vector<uint8_t> >" , &negativeValuesPSMtxOUT);
    sEventTree->Branch("baselineCMRMSMtxOUT", "std::vector< std::vector<float> >" , &baselineCMRMSMtxOUT);
    sEventTree->Branch("PSSlope20MtxOUT", "std::vector< std::vector<float> >" , &PSSlope20MtxOUT);
    //sEventTree->Branch("myCsSrtripsTouchedMtxOUT", "std::vector < std::vector<uint16_t> >", &myCsSrtripsTouchedMtxOUT);
    //sEventTree->Branch("myCsChargeMtxOUT", "std::vector < std::vector<float> >", &myCsChargeMtxOUT);
    //sEventTree->Branch("myCsMaxChargeMtxOUT", "std::vector < std::vector<uint16_t> >", &myCsMaxChargeMtxOUT);
    //sEventTree->Branch("myCsCenterMtxOUT", "std::vector < std::vector<uint16_t> >", &myCsCenterMtxOUT);
    //sEventTree->Branch("flatBlOUT", "std::vector < std::vector<uint8_t> >", &flatBlOUT);
    sEventTree->Branch("timesTOUT", "std::vector< ULong64_t >", &timesTOUT);
    sEventTree->Branch("eventNrsOUT", "std::vector< UInt_t >", &eventNrsOUT);
    sEventTree->Branch("bunchNrsOUT", "std::vector< UInt_t >", &bunchNrsOUT);
     cout << "tree filling" << endl;
 
    // iterate through intermediate vectors and save values int tree 
    //
    for(uint32_t TOBmodule = 0; TOBmodule<baselinesMtxINTER.at(0).size(); TOBmodule++)
    {
       //cout << "here1" << endl;
       moduleKeyOUT = moduleKeysINTER.at(0).at(TOBmodule);
       moduleValueOUT = moduleValuesINTER.at(0).at(TOBmodule);
    
       //cout << "here2" << endl;
        for(uint32_t out = 0; out < eventNrINTER.size(); out++)
        {
            if(baselinesMtxINTER.at(out).size() == 0)
                continue;

       //cout << "here3" << endl;
            eventNrsOUT.push_back(eventNrINTER.at(out));
            timesTOUT.push_back(timeTINTER.at(out));
            bunchNrsOUT.push_back(bunchNrINTER.at(out));
         
       //cout << "here4" << endl;
            baselinesMtxOUT.push_back(baselinesMtxINTER.at(out).at(TOBmodule));
            chargeOfClustersMtxOUT.push_back(chargeOfClustersMtxINTER.at(out).at(TOBmodule));
            centerOfClustersMtxOUT.push_back(centerOfClustersMtxINTER.at(out).at(TOBmodule)); 
      // cout << "here5" << endl;
            stripsTouchedOfClustersMtxOUT.push_back(stripsTouchedOfClustersMtxINTER.at(out).at(TOBmodule));
            maxADCOfClustersMtxOUT.push_back(maxADCOfClustersMtxINTER.at(out).at(TOBmodule));
            baselineRMSMtxOUT.push_back(baselineRMSMtxINTER.at(out).at(TOBmodule));
            positiveValuesCMNSMtxOUT.push_back(positiveValuesCMNSMtxINTER.at(out).at(TOBmodule));
            negativeValuesPSMtxOUT.push_back(negativeValuesPSMtxINTER.at(out).at(TOBmodule));
            baselineCMRMSMtxOUT.push_back(baselineCMRMSMtxINTER.at(out).at(TOBmodule));
            PSSlope20MtxOUT.push_back(PSSlope20MtxINTER.at(out).at(TOBmodule));
       //cout << "here6" << endl;
            //myCsSrtripsTouchedMtxOUT.push_back(myCsSrtripsTouchedMtxINTER.at(out).at(TOBmodule));
            //myCsChargeMtxOUT.push_back(myCsChargeMtxINTER.at(out).at(TOBmodule));
            //myCsMaxChargeMtxOUT.push_back(myCsMaxChargeMtxINTER.at(out).at(TOBmodule));
            //myCsCenterMtxOUT.push_back(myCsCenterMtxINTER.at(out).at(TOBmodule));

       //cout << "here7" << endl;
            //myfile << "eventNrOut: " << eventNrOUT << " time: " << timeTOUT << endl;
        }

        //myfile << "event nrs size " << eventNrsOUT.size();

        //for(uint32_t log = 0; log < eventNrsOUT.size(); log++)
        //{
        //   myfile << "event vector entry: " << log << " value: " << eventNrsOUT.at(log) << "  time " << timesTOUT.at(log) << endl;
        //}

        sEventTree->Fill();
        
        eventNrsOUT.clear();
        timesTOUT.clear();
        bunchNrsOUT.clear();
         
        baselinesMtxOUT.clear();
        chargeOfClustersMtxOUT.clear();
        centerOfClustersMtxOUT.clear();
        stripsTouchedOfClustersMtxOUT.clear();
        maxADCOfClustersMtxOUT.clear();
        baselineRMSMtxOUT.clear();
        positiveValuesCMNSMtxOUT.clear();
        negativeValuesPSMtxOUT.clear();
        baselineCMRMSMtxOUT.clear();
        PSSlope20MtxOUT.clear();
        //myCsSrtripsTouchedMtxOUT.clear();
        //myCsChargeMtxOUT.clear();
        //myCsMaxChargeMtxOUT.clear();
        //myCsCenterMtxOUT.clear();

    }

    sEventFile->Write();
    
    //myfile.close();
        cout << "time7: " << time(NULL) << endl;
}




