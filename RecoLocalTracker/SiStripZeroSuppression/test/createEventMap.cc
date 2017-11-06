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
    if(argc != 2)
        throw std::runtime_error("Bad number of arguments!");

    TString input = argv[1];
    
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
    myfile.open ("eventMapVR2.txt");

    uint32_t nrOfFiles = in_stream.size();


    //define variables to be read from tree
    vector<unsigned int>  *moduleKeys = NULL; //@MJ@ TODO why pointer????
    uint32_t eventNr;
    std::map< uint32_t, std::pair<uint32_t, uint32_t> > eventMap; 


    TFile* f;   
    TTree* t;   
    //loop over files
    for(uint32_t file=0; file<nrOfFiles; file++)
    {
        f = TFile::Open(in_stream.at(file));
        cout << "first map filling" << " file " << file  << endl;

        //read tree
        t = dynamic_cast<TTree*>(f->Get("baselineAna/timeTree"));
        
        t->SetBranchAddress("moduleKeys",&moduleKeys);
        t->SetBranchAddress("eventNr",&eventNr);

        //cout << "time2: " << time(NULL) << endl;
        Int_t nentries = (Int_t)t->GetEntries();
    
        //loop over events    
        //cout << "time3: " << time(NULL) << endl;
        for (Int_t i=0; i<nentries; i++) 
        {
            t->GetEntry(i);
        //cout << "time4: " << time(NULL) << endl;
            //if no HV continue
            if(moduleKeys->size() == 0)
                continue;
         
              //@MJ@ TODO obsolete now
             //fill and sort vector of event nr
             //sortedEvents.push_back(eventNr);
    
             //map of: eventnr,file,positoon in file
             eventMap.insert(make_pair(eventNr, make_pair(file,i)));

        }
        //TODO go thorugh file and write output!!!!!
        //cout << "time4: " << time(NULL) << endl;
        //clear the tree
        t->Delete();
        t = NULL;
        f->Close();
        f->Delete();
        f= NULL;
    }
   
    for (std::map< uint32_t, pair<uint32_t, uint32_t> >::iterator it=eventMap.begin(); it!=eventMap.end(); ++it)
    {
    myfile << it->first << " " << " " << it->second.first << " " << it->second.second << endl;
    }

 
    myfile.close();
}




