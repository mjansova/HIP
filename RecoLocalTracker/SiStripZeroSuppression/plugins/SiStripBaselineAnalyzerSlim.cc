// -*- C++ -*-
//
// Package:    SiStripBaselineAnalyzerSlim
// Class:      SiStripBaselineAnalyzerSlim
// 
/**\class SiStripBaselineAnalyzerSlim SiStripBaselineAnalyzerSlim.cc Validation/SiStripAnalyzer/src/SiStripBaselineAnalyzerSlim.cc

 Description: <one line class summary>

 Implementation:
     <Notes on implementation>
*/
//
// Original Author:  Ivan Amos Cali
//         Created:  Mon Jul 28 14:10:52 CEST 2008
//
//
 

// system include files
#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdlib.h>


// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/DetSet.h"
#include "DataFormats/Common/interface/DetSetVector.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#include "FWCore/Utilities/interface/EDGetToken.h"

#include "DataFormats/SiStripDigi/interface/SiStripProcessedRawDigi.h"
#include "DataFormats/SiStripDigi/interface/SiStripRawDigi.h"
#include "DataFormats/SiStripCluster/interface/SiStripCluster.h"
#include "DataFormats/SiStripCluster/interface/SiStripClusterCollection.h"

#include "CondFormats/SiStripObjects/interface/SiStripPedestals.h"
#include "CondFormats/DataRecord/interface/SiStripPedestalsRcd.h"

#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripPedestalsSubtractor.h"
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripCommonModeNoiseSubtractor.h"
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripRawProcessingFactory.h"
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripRawProcessingAlgorithms.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

#include "DataFormats/Provenance/interface/RunLumiEventNumber.h"
#include "DataFormats/Provenance/interface/Timestamp.h"

//ROOT inclusion
#include "TROOT.h"
#include "TFile.h"
#include "TNtuple.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH1I.h"
#include "TH2F.h"
#include "TH2D.h"
#include "TF1.h"
#include "TProfile.h"
#include "TList.h"
#include "TString.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "THStack.h"

using namespace std;

//
// class decleration
//

class SiStripBaselineAnalyzerSlim : public edm::EDAnalyzer {
   public:
      explicit SiStripBaselineAnalyzerSlim(const edm::ParameterSet&);
      ~SiStripBaselineAnalyzerSlim();


   private:
      virtual void beginJob() override ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override ;
      
	  std::auto_ptr<SiStripPedestalsSubtractor>   subtractorPed_;
          edm::ESHandle<SiStripPedestals> pedestalsHandle;
          std::auto_ptr<SiStripRawProcessingAlgorithms> algorithms;
          std::vector<int> pedestals;
          uint32_t peds_cache_id;

	  uint32_t hitStripThreshold_;


         
          const edm::EDGetTokenT< edm::DetSetVector<SiStripProcessedRawDigi> > srcBaseline_; 
          const edm::EDGetTokenT< edm::DetSetVector<SiStripProcessedRawDigi> > srcBaselinePoints_; 
          const edm::EDGetTokenT< edm::DetSetVector<SiStripProcessedRawDigi> > srcAPVCM_; 
          const edm::EDGetTokenT< edm::DetSetVector<SiStripRawDigi> > srcProcessedRawDigi_; 
          const edm::EDGetTokenT< edm::DetSetVector<SiStripDigi> > srcZSdigi_; 
   	  const edm::EDGetTokenT< edmNew::DetSetVector<SiStripCluster> > clusToken;
   	  const edm::EDGetTokenT< edmNew::DetSetVector<SiStripCluster> > clusToken2;
          edm::Service<TFileService> fs_;
  
	  
	  TCanvas* Canvas_;
	  std::vector<TH1F> vProcessedRawDigiHisto_;
	  std::vector<TH1F> vBaselineHisto_;
          std::vector<TH1F> vBaselinePointsHisto_;
	  std::vector<TH1F> vClusterHisto_;
          //TH2D* h2Baselines_;
          /*TH1F* h1APVCM_;
          TH1F* h1APVCMres_;
          TH1F* h1RawDigis_;
          TH1F* h1PRawDigis_;
          TH1F* h1CRawDigis_;
          TH1F* pos_;
          TH1F* neg_;*/
	  
	  uint16_t nModuletoDisplay_;
	  uint16_t actualModule_;

          std::map<uint32_t, uint32_t> idx;
          std::vector<uint32_t> moduleKeys;
          std::vector<uint32_t> moduleValues;
          TFile *outFile;
          TTree *timeTree;
          ULong64_t timeT;
          UInt_t runNr;
          UInt_t eventNr;
          UInt_t bunchNr;

          uint32_t entry;
          uint32_t modentry;

             std::vector<uint8_t> positiveValuesCMNSMtx[14681];
             std::vector<uint8_t> negativeValuesPSMtx[14681];
             std::vector<uint8_t> maxADCOfClustersMtx[14681];
             std::vector<uint16_t> centerOfClustersMtx[14681];
             std::vector<uint16_t>  stripsTouchedOfClustersMtx[14681];
             std::vector<float>  baselineRMSMtx[14681];
             std::vector<float>  baselineCMRMSMtx[14681];
             //std::vector<float>  PSSlope10Mtx[14681];
             std::vector<float>  PSSlope20Mtx[14681];
             //std::vector<float>  ZSRMS10Mtx[14681];
             //std::vector<float>  ZSRMS20Mtx[14681];
             std::vector<float> baselinesMtx[14681];
             std::vector<float>  chargeOfClustersMtx[14681];

};


SiStripBaselineAnalyzerSlim::SiStripBaselineAnalyzerSlim(const edm::ParameterSet& conf):

  srcBaseline_(consumes< edm::DetSetVector<SiStripProcessedRawDigi> >( conf.getParameter<edm::InputTag>( "srcBaseline" ))),
  srcBaselinePoints_(consumes< edm::DetSetVector<SiStripProcessedRawDigi> >( conf.getParameter<edm::InputTag>( "srcBaselinePoints" ))),
  srcAPVCM_(consumes< edm::DetSetVector<SiStripProcessedRawDigi> >( conf.getParameter<edm::InputTag>( "srcAPVCM" ))),
  srcProcessedRawDigi_(consumes< edm::DetSetVector<SiStripRawDigi> >( conf.getParameter<edm::InputTag>( "srcProcessedRawDigi" ))),
  //srcZSdigi_(consumes< edm::DetSetVector<SiStripDigi> >( conf.getParameter<edm::InputTag>( "srcZSdigi" ))),
  clusToken(consumes< edmNew::DetSetVector<SiStripCluster> >( edm::InputTag("siStripClusters"))),
  clusToken2(consumes< edmNew::DetSetVector<SiStripCluster> >( edm::InputTag("siStripClusters2")))
{
   
  subtractorPed_ = SiStripRawProcessingFactory::create_SubtractorPed(conf.getParameter<edm::ParameterSet>("Algorithms"));
  algorithms = SiStripRawProcessingFactory::create(conf.getParameter<edm::ParameterSet>("Algorithms"));
  nModuletoDisplay_ = conf.getParameter<uint32_t>( "nModuletoDisplay" );
  hitStripThreshold_ = conf.getParameter<uint32_t>( "hitStripThreshold" );

  /*h2Baselines_ = fs_->make<TH2D>("BaselinesVsModules","BaselinesVsModules", 1324, -300, 1024, 15000, 0, 15000);
  h2Baselines_->SetXTitle("Baselines [adc]");
  h2Baselines_->SetYTitle("Modules");
  h2Baselines_->SetLineWidth(2);
  h2Baselines_->SetLineStyle(2);
  */
  /*h1APVCM_ = fs_->make<TH1F>("APVCM","APVCM", 10000, 0, 1);
  h1APVCM_->SetXTitle("strips");
  h1APVCM_->SetYTitle("[ADC]");
  h1APVCM_->SetLineWidth(2);
  h1APVCM_->SetLineStyle(2);
 
  h1APVCMres_ = fs_->make<TH1F>("APVCMres","APVCMres", 10000, 0, 1);
  h1APVCMres_->SetXTitle("strips");
  h1APVCMres_->SetYTitle("[ADC]");
  h1APVCMres_->SetLineWidth(2);
  h1APVCMres_->SetLineStyle(2);
 
  h1RawDigis_ = fs_->make<TH1F>("rawdigis","rawdigis", 2048, 0, 2048);
  h1RawDigis_->SetXTitle("nr");
  h1RawDigis_->SetYTitle("[adc]");
  h1RawDigis_->SetLineWidth(2);
  h1RawDigis_->SetLineStyle(2);
  
  h1PRawDigis_ = fs_->make<TH1F>("pedestal rawdigis","pedestal rawdigis", 2048, -1024, 1024);
  h1PRawDigis_->SetXTitle("nr");
  h1PRawDigis_->SetYTitle("[adc]");
  h1PRawDigis_->SetLineWidth(2);
  h1PRawDigis_->SetLineStyle(2);

  h1CRawDigis_ = fs_->make<TH1F>(" cmn rawdigis","cmn rawdigis", 2048, -1024, 1024);
  h1CRawDigis_->SetXTitle("nr");
  h1CRawDigis_->SetYTitle("[adc]");
  h1CRawDigis_->SetLineWidth(2);
  h1CRawDigis_->SetLineStyle(2);

  pos_ = fs_->make<TH1F>("pos","pos", 128, 0, 128);
  pos_->SetXTitle("strips");
  pos_->SetYTitle("[ADC]");
  pos_->SetLineWidth(2);
  pos_->SetLineStyle(2);
 
  neg_ = fs_->make<TH1F>("neg","neg", 128, 0, 128);
  neg_->SetXTitle("strips");
  neg_->SetYTitle("[ADC]");
  neg_->SetLineWidth(2);
  neg_->SetLineStyle(2);*/
  entry =0;
  modentry =0;

  timeTree = fs_->make<TTree>("timeTree","timeTree");
  timeTree->Branch("eventNr", &eventNr, "eventNr/i");
  timeTree->Branch("bunchNr", &bunchNr, "bunchNr/i");
  timeTree->Branch("timeT", &timeT, "timeT/l");
  timeTree->Branch("moduleKeys", "std::vector<uint32_t>", &moduleKeys);
  timeTree->Branch("moduleValues", "std::vector<uint32_t>", &moduleValues);
}


SiStripBaselineAnalyzerSlim::~SiStripBaselineAnalyzerSlim()
{
 
   

}

void
SiStripBaselineAnalyzerSlim::analyze(const edm::Event& e, const edm::EventSetup& es)
{

   entry++;
   using namespace edm;

   subtractorPed_->init(es); 
 
   edm::Handle< edm::DetSetVector<SiStripRawDigi> > moduleRawDigi;
   e.getByToken(srcProcessedRawDigi_,moduleRawDigi);

   //edm::Handle< edm::DetSetVector<SiStripDigi> > moduleZSdigi;
   //e.getByToken(srcZSdigi_,moduleZSdigi);

 
   edm::Handle<edm::DetSetVector<SiStripProcessedRawDigi> > moduleBaseline;
   e.getByToken(srcBaseline_, moduleBaseline); 

   edm::Handle<edm::DetSetVector<SiStripProcessedRawDigi> > moduleBaselinePoints;
   e.getByToken(srcBaselinePoints_, moduleBaselinePoints); 
   
   edm::Handle<edmNew::DetSetVector<SiStripCluster> > clusters;
   e.getByToken(clusToken, clusters);
   
   edm::Handle<edmNew::DetSetVector<SiStripCluster> > NCclusters;
   e.getByToken(clusToken2, NCclusters);

   char detIds[20];
   char evs[20];
   char runs[20];    
   char times[20];    



   edm::RunNumber_t const run = e.id().run();
   edm::EventNumber_t const event = e.id().event();
   int bunch = e.bunchCrossing();
   runNr = static_cast<UInt_t>(run);
   eventNr = static_cast<UInt_t>(event);
   bunchNr = static_cast<UInt_t>(bunch);
   
   edm::Timestamp const timestamp = e.time();
   edm::TimeValue_t timeVal = timestamp.value();
   timeT = static_cast<ULong64_t>(timeVal);


   uint32_t counter=0; 
   edm::DetSetVector<SiStripProcessedRawDigi>::const_iterator itDSBaseline;
   itDSBaseline = moduleBaseline->begin();
   edm::DetSetVector<SiStripRawDigi>::const_iterator itRawDigis = moduleRawDigi->begin();
   
     edm::Handle<edm::DetSetVector<SiStripProcessedRawDigi> > moduleCM;
     edm::InputTag CMLabel("siStripZeroSuppression:APVCM");
          e.getByToken(srcAPVCM_,moduleCM);
    
 
   for (; itRawDigis != moduleRawDigi->end(); ++itRawDigis) {
      if(actualModule_ > nModuletoDisplay_) {return;}
      uint32_t detId = itRawDigis->id;
      
      sprintf(detIds,"%u", detId);
      sprintf(evs,"%llu", event);
      sprintf(runs,"%u", run);
      sprintf(times,"%llu", timeT);
      
      uint32_t mapSize = idx.size();
      idx.insert(std::pair<uint32_t, uint32_t>(detId,mapSize+1));

      if(entry == 1)
      {
      
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_baselinesMtx").c_str(), "std::vector<float>" , &baselinesMtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_positiveValuesCMNSMtx").c_str(), "std::vector<uint8_t>" , &positiveValuesCMNSMtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_negativeValuesPSMtx").c_str(), "std::vector<uint8_t>" , &negativeValuesPSMtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_maxADCOfClustersMtx").c_str(), "std::vector<uint8_t>" , &maxADCOfClustersMtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_centerOfClustersMtx").c_str(), "std::vector<uint16_t>" , &centerOfClustersMtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_stripsTouchedOfClustersMtx").c_str(), "std::vector<uint16_t>" , &stripsTouchedOfClustersMtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_baselineRMSMtx").c_str(), "std::vector<float>" , &baselineRMSMtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_baselineCMRMSMtx").c_str(), "std::vector<float>" , &baselineCMRMSMtx[idx.find(detId)->second]);
          //timeTree->Branch((std::to_string(idx.find(detId)->second) + "_PSSlope10Mtx").c_str(), "std::vector<float>" , &PSSlope10Mtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_PSSlope20Mtx").c_str(), "std::vector<float>" , &PSSlope20Mtx[idx.find(detId)->second]);
          //timeTree->Branch((std::to_string(idx.find(detId)->second) + "_ZSRMS10Mtx").c_str(), "std::vector<float>" , &ZSRMS10Mtx[idx.find(detId)->second]);
          //timeTree->Branch((std::to_string(idx.find(detId)->second) + "_ZSRMS20Mtx").c_str(), "std::vector<float>" , &ZSRMS20Mtx[idx.find(detId)->second]);
          timeTree->Branch((std::to_string(idx.find(detId)->second) + "_chargeOfClustersMtx").c_str(), "std::vector<float>" , &chargeOfClustersMtx[idx.find(detId)->second]);
      }
	  
	if(itDSBaseline->id != detId){
		itDSBaseline = moduleBaseline->find(detId);
                if(itDSBaseline->id != detId){ itDSBaseline++; continue; }
	}	  
      
    
      actualModule_++;
	  

      edm::DetSet<SiStripRawDigi>::const_iterator itRaw = itRawDigis->begin();
      //cout << "raw digis: " << std::distance(itRawDigis->begin(), itRawDigis->end()) << "size: " << itRawDigis->size() << endl;
      bool restAPV[6] = {0,0,0,0,0,0};
      uint16_t strip =0, totADC=0;
      int minAPVRes = 7, maxAPVRes = -1;
      for(;itRaw != itRawDigis->end(); ++itRaw, ++strip){
            float adc = itRaw->adc();
            totADC+= adc;
            if(strip%128 ==127){
                //std::cout << "totADC " << totADC << std::endl;
                       int APV = strip/128;
                       //   if(totADC!= 0){ @MJ@ TODO
                     if(true){
                      restAPV[APV] = true;
                      totADC =0;
                     if(APV>maxAPVRes) maxAPVRes = APV;
                     if(APV<minAPVRes) minAPVRes = APV;
                      }
                      }
                      }
                      //uint16_t bins =768;
                     float minx = -0.5, maxx=767.5;
                    if(minAPVRes !=7){
                   //minx = minAPVRes * 128 -0.5;
                   //maxx = maxAPVRes * 128 + 127.5;
                                                                                                                                                                                                                           //bins = maxx-minx;
                                                                                                                                                                                                                                 }


      std::vector<int16_t> ProcessedRawDigis(itRawDigis->size());
      std::vector<int16_t> ProcessedRawDigisPedSub(itRawDigis->size());
      std::vector<int16_t> ProcessedRawDigisCMNSubAndBot(itRawDigis->size());
      subtractorPed_->subtract( *itRawDigis, ProcessedRawDigis, ProcessedRawDigisPedSub, ProcessedRawDigisCMNSubAndBot);

      edm::DetSet<SiStripProcessedRawDigi>::const_iterator  itBaseline;
      itBaseline = itDSBaseline->begin();
            
      std::vector<int16_t>::const_iterator itProcessedRawDigis;
      std::vector<int16_t>::const_iterator itProcessedRawDigisPedSub = ProcessedRawDigisPedSub.begin();
      std::vector<int16_t>::const_iterator itProcessedRawDigisCMNSubAndBot = ProcessedRawDigisCMNSubAndBot.begin();
     
      strip =0;
      baselinesMtx[idx.find(detId)->second].clear();
      positiveValuesCMNSMtx[idx.find(detId)->second].clear();
      negativeValuesPSMtx[idx.find(detId)->second].clear();
      baselineRMSMtx[idx.find(detId)->second].clear();
      baselineCMRMSMtx[idx.find(detId)->second].clear();
      //PSSlope10Mtx[idx.find(detId)->second].clear();
      PSSlope20Mtx[idx.find(detId)->second].clear();
      std::vector<float> rms;  
      std::vector<float> rmscm;  
      float adcbl = 0;
      uint8_t position = 0;
      uint8_t positiveStrips = 0;
      uint8_t negativeStrips = 0;
      std::multimap <float, uint8_t> digiMap;
      std::map <uint8_t, float> map10p;
      std::map <uint8_t, float> map20p;
      TGraph graph10p;
      TGraph graph20p;


      for(itProcessedRawDigis = ProcessedRawDigis.begin();itProcessedRawDigis != ProcessedRawDigis.end(); itProcessedRawDigis++, itProcessedRawDigisPedSub++, itProcessedRawDigisCMNSubAndBot++){ 
       	if(restAPV[strip/128]){
            counter++;
            float adc = *itProcessedRawDigis;
            float adcPS = *itProcessedRawDigisPedSub;
            float adcCMNS = *itProcessedRawDigisCMNSubAndBot;
            /*if(counter<38400)
            {
                h1RawDigis_->Fill(adc);
                h1PRawDigis_->Fill(adcPS); //TODO
                h1CRawDigis_->Fill(adcCMNS);
            }*/
            if(strip==0)
            {
                position = 0;
                adcbl = (itBaseline+1)->adc();
                digiMap.clear();
                positiveStrips = 0;
                negativeStrips = 0;
            }
            if(adcbl==-333)
               std::cout << "you made a mistake!!!!" << std::endl;
            if(strip%128 == 0) //first strip
            {
                adcbl = (itBaseline+1)->adc(); //127 offset
                position = 0;
                digiMap.clear();
                positiveStrips = 0;
                negativeStrips = 0;
                graph10p = TGraph(115);
                graph20p = TGraph(102);
                baselinesMtx[idx.find(detId)->second].push_back(adcbl); //@MJ@ TODO be aware, here at 0 position weird value to recognize flatness
                rms.clear();
                rmscm.clear();
            }
            digiMap.insert(make_pair(adcPS,position));
            position++;
            //first remove bottom of map
            if(adcCMNS>0)
            {
                positiveStrips++;
            }
            if(adcPS<=0)
            {
                negativeStrips++;
            }
            rms.push_back(adc);
            rmscm.push_back(adcCMNS);
            if((strip+1)%128 == 0) //last strip
            {
                std::sort(rms.begin(), rms.end());
                float sum = 0;
                float av = 0;
                float RMSstrips = rms.size()-26; //20%
                for(uint32_t r=0; r<RMSstrips; r++)
                {
                    sum +=rms.at(r);
                }
                av = sum/RMSstrips;
                               
                float rmsSum =0;
                float rmsRes =0;
                for(uint32_t v=0; v<RMSstrips; v++)
                {
                    rmsSum += pow(rms.at(v)-av,2);
                }
                
                rmsRes = sqrt(rmsSum/RMSstrips);
                baselineRMSMtx[idx.find(detId)->second].push_back(rmsRes);
             
                std::sort(rmscm.begin(), rmscm.end());
                float sumcm = 0;
                float avcm = 0;
                float RMSstripscm = rmscm.size()-26; //20%
                for(uint32_t r=0; r<RMSstripscm; r++)
                {
                    sumcm +=rmscm.at(r);
                }
                avcm = sumcm/RMSstripscm;
                               
                float rmsSumcm =0;
                float rmsRescm =0;
                for(uint32_t v=0; v<RMSstripscm; v++)
                {
                    rmsSumcm += pow(rmscm.at(v)-avcm,2);
                }
                
                rmsRescm = sqrt(rmsSumcm/RMSstripscm);
                baselineCMRMSMtx[idx.find(detId)->second].push_back(rmsRescm);

                if(positiveStrips > 128)
                    cout << "sth fishy in here, there should be max 128 positive strips" << endl;
                if(negativeStrips > 128)
                    cout << "sth fishy in here, there should be max 128 negative strips" << endl;
                positiveValuesCMNSMtx[idx.find(detId)->second].push_back(positiveStrips);
                negativeValuesPSMtx[idx.find(detId)->second].push_back(negativeStrips);
                positiveStrips = 0;
                negativeStrips = 0;


                uint32_t mapMember = 0;
                for(std::multimap <float, uint8_t>::iterator itD = digiMap.begin(); itD != digiMap.end(); itD++)
                {
                    //10%
                    //if(mapMember <115)
                    //    map10p.insert(make_pair(itD->second, itD->first));
                    //20% 
                    if(mapMember <102)
                        map20p.insert(make_pair(itD->second, itD->first));

                    mapMember++;
                }
                digiMap.clear();   

                /*uint32_t counter10 = 0;
                for(std::map <uint8_t, float>::iterator it10 = map10p.begin(); it10 != map10p.end(); it10++)
                {
                    counter10++;
                    //10%
                    graph10p.SetPoint(counter10,it10->first, it10->second);
                }*/
                uint32_t counter20 = 0;
                for(std::map <uint8_t, float>::iterator it20 = map20p.begin(); it20 != map20p.end(); it20++)
                {
                    counter20++;
                    //20%
                    graph20p.SetPoint(counter20,it20->first, it20->second);

                }
                
                //TF1 *f10 = new TF1("f10","[0]+[1]*x", 0 , 127);
                //f10->SetParameters(128,0);
                //graph10p.Fit("f10", "CQ");

                
                TF1 *f20 = new TF1("f20","[0]+[1]*x", 0 , 127);
                f20->SetParameters(128,0);
                graph20p.Fit("f20", "CQ");

                //PSSlope10Mtx[idx.find(detId)->second].push_back(f10->GetParameter(1));
                PSSlope20Mtx[idx.find(detId)->second].push_back(f20->GetParameter(1));

                //map10p.clear();
                map20p.clear();
                //delete f10;
                delete f20;
                
            }
	    ++itBaseline;
	 }
	++strip;
      }
      ++itDSBaseline;

/*
      //uint32_t found = 0;
      uint8_t p10 = 115;
      uint8_t p20 = 120;
      ZSRMS10Mtx[idx.find(detId)->second].clear();
      ZSRMS20Mtx[idx.find(detId)->second].clear();
      edm::DetSetVector<SiStripDigi>::const_iterator itZSdigis = moduleZSdigi->begin();
     
      for(; itZSdigis != moduleZSdigi->end(); ++itZSdigis){
          if(itZSdigis->id == detId)
          {
          //found++;
          std::map<uint16_t, uint16_t> ZSmap;
          ZSmap.clear();
          for( edm::DetSet<SiStripDigi>::const_iterator ZSdigis = itZSdigis->begin(); ZSdigis != itZSdigis->end(); ++ZSdigis) {
                  ZSmap.insert(make_pair(ZSdigis->strip(), ZSdigis->adc()));//how does the strip look like, does it start with zero and end with 5??
        }
        //cout << "maxx " << maxx << endl;
        for(uint32_t el = 0; el <((uint32_t)(maxx+0.5)); el++)
        { 
            std::map<uint16_t, uint16_t>::iterator it = ZSmap.find(el);
            if(it == ZSmap.end())
               ZSmap.insert(make_pair(el,0));
        }
        //cout << "ZS map size " << ZSmap.size() << endl;
        if(ZSmap.size()%128 != 0)
            cout << "problem in here, map should have consist 128*n elements!!: " << endl;
        if(ZSmap.size()/128 != 4 && ZSmap.size()/128 != 6)
            cout << "problem in here, map should have consist of 4 or 6 APVs!! it is:  "<< ZSmap.size()/128 << endl;
        uint32_t nrOfAPVs = ZSmap.size()/128;
        std::map<uint16_t, uint16_t>::iterator ZSvalues = ZSmap.begin();
        vector<int16_t> digisInAPV;
        for(uint32_t a = 0; a<nrOfAPVs; a++)
        {
            float sum10 = 0;
            float sum20 = 0;
            float av10 = 0;
            float av20 = 0;
            std::transform( ZSvalues, std::next(ZSvalues, 128), std::back_inserter( digisInAPV ), [](std::pair<uint16_t, uint16_t> const & r) { return r.second; } );
            ZSvalues = std::next(ZSvalues, 128);
            std::sort(digisInAPV.begin(), digisInAPV.end());
            for(uint32_t vect = 0; vect< digisInAPV.size(); vect++)
            {
                if(vect<p10)
                    sum10 +=digisInAPV.at(vect);
                if(vect<p20)
                    sum20 +=digisInAPV.at(vect);
            }        
            av10 = sum10/p10;
            av20 = sum20/p20;
                               
                float rmsSum10 =0;
                float rmsRes10 =0;
                float rmsSum20 =0;
                float rmsRes20 =0;
                for(uint32_t v=0; v<p10; v++)
                {
                    rmsSum10 += pow(digisInAPV.at(v)-av10,2);
                }
                for(uint32_t v=0; v<p20; v++)
                {
                    rmsSum20 += pow(digisInAPV.at(v)-av20,2);
                }
                
                rmsRes10 = sqrt(rmsSum10/p10);
                rmsRes20 = sqrt(rmsSum20/p20);

                ZSRMS10Mtx[idx.find(detId)->second].push_back(rmsRes10);
                ZSRMS20Mtx[idx.find(detId)->second].push_back(rmsRes20);
            
            digisInAPV.clear();
        }
        

       }
      }
      */
      //if(found==0)
      //{
                //ZSRMS10Mtx[idx.find(detId)->second].push_back(0);
                //ZSRMS20Mtx[idx.find(detId)->second].push_back(0);
      //}

      for(uint32_t ccol = 0; ccol<1; ccol++  )
      {
          maxADCOfClustersMtx[idx.find(detId)->second].clear();
          centerOfClustersMtx[idx.find(detId)->second].clear();
          stripsTouchedOfClustersMtx[idx.find(detId)->second].clear();
          chargeOfClustersMtx[idx.find(detId)->second].clear();
          uint16_t clusCharge = 0;
          uint16_t clusCenter = 0;
          edmNew::DetSetVector<SiStripCluster>::const_iterator itClusters;
          edmNew::DetSetVector<SiStripCluster>::const_iterator itClustersEnd;
          if(ccol == 0)
          {
              itClusters = clusters->begin();
              itClustersEnd = clusters->end();
          }
          else
              throw std::runtime_error("There is no more than 2 cluster collections for now!!!");

	  for ( ; itClusters != itClustersEnd; ++itClusters ){
		for ( edmNew::DetSet<SiStripCluster>::const_iterator clus =	itClusters->begin(); clus != itClusters->end(); ++clus){
		  if(itClusters->id() == detId){
		      //int firststrip = clus->firstStrip();
                      clusCharge = clus->charge();
                      clusCenter = clus->barycenter();
     		      strip=0;
                      uint8_t maxc = 0;
		      for( auto itAmpl = clus->amplitudes().begin(); itAmpl != clus->amplitudes().end(); ++itAmpl){
		          ++strip; 
                          
                          maxc = *itAmpl > maxc ? *itAmpl : maxc;
                      }
                if(ccol == 0)
                {
                      maxADCOfClustersMtx[idx.find(detId)->second].push_back(maxc);
                      centerOfClustersMtx[idx.find(detId)->second].push_back(clusCenter);
                      stripsTouchedOfClustersMtx[idx.find(detId)->second].push_back(strip);
                      chargeOfClustersMtx[idx.find(detId)->second].push_back(clusCharge);
                      clusCharge = 0;
                      clusCenter = 0;
                      strip = 0;
                      maxc = 0;
                }
          else
              throw std::runtime_error("There is no more than 1 cluster collections for now!!!");
		    }
		}
	  
             }

      }
    


   }		
    

actualModule_ =0;  
std::transform( idx.begin(), idx.end(), std::back_inserter( moduleKeys ), [](std::pair<uint32_t, uint32_t> const & p) { return p.first; } );
std::transform( idx.begin(), idx.end(), std::back_inserter( moduleValues ), [](std::pair<uint32_t, uint32_t> const & r) { return r.second; } );
timeTree->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void SiStripBaselineAnalyzerSlim::beginJob()
{
  
  
actualModule_ =0;  
   
 
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SiStripBaselineAnalyzerSlim::endJob() {
}

//define this as a plug-in
DEFINE_FWK_MODULE(SiStripBaselineAnalyzerSlim);

