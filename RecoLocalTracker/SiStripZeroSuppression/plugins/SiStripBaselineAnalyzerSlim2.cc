// -*- C++ -*-
//
// Package:    SiStripBaselineAnalyzerSlim2
// Class:      SiStripBaselineAnalyzerSlim2
// 
/**\class SiStripBaselineAnalyzerSlim2 SiStripBaselineAnalyzerSlim2.cc Validation/SiStripAnalyzer/src/SiStripBaselineAnalyzerSlim2.cc

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

class SiStripBaselineAnalyzerSlim2 : public edm::EDAnalyzer {
   public:
      explicit SiStripBaselineAnalyzerSlim2(const edm::ParameterSet&);
      ~SiStripBaselineAnalyzerSlim2();


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
          //TH1F* h1APVCM_;
          //TH1F* h1APVCMres_;
          //TH1F* h1RawDigis_;
          //TH1F* h1PRawDigis_;
          //TH1F* h1CRawDigis_;
	  
	  uint16_t nModuletoDisplay_;
	  uint16_t actualModule_;

          std::map<uint32_t, uint32_t> idx;

          std::vector<uint32_t> moduleKeys;
          std::vector<uint32_t> moduleValues;
          std::vector< std::vector<float> > baselinesMtx;
          std::vector< std::vector<float> > baselineRMSMtx;
          std::vector< std::vector<float> >  baselineCMRMSMtx;
          std::vector< std::vector<float> >  PSSlope20Mtx;
          std::vector< std::vector<uint8_t> > positiveValuesCMNSMtx;
          std::vector< std::vector<uint8_t> > negativeValuesPSMtx;
          std::vector< std::vector<float> > chargeOfClustersMtx;
          std::vector< std::vector<uint16_t> > centerOfClustersMtx;
          std::vector< std::vector<uint16_t> > stripsTouchedOfClustersMtx;
          std::vector< std::vector<uint8_t> > maxADCOfClustersMtx;
  
          TFile *outFile;
          TTree *timeTree;
          ULong64_t timeT;
          UInt_t runNr;
          UInt_t eventNr;
          UInt_t bunchNr;

          uint32_t entry;
          uint32_t modentry;

};


SiStripBaselineAnalyzerSlim2::SiStripBaselineAnalyzerSlim2(const edm::ParameterSet& conf):

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
  
  h1APVCM_ = fs_->make<TH1F>("APVCM","APVCM", 2000, -1000, 1000);
  h1APVCM_->SetXTitle("strips");
  h1APVCM_->SetYTitle("[ADC]");
  h1APVCM_->SetLineWidth(2);
  h1APVCM_->SetLineStyle(2);
 
  h1APVCMres_ = fs_->make<TH1F>("APVCMres","APVCMres", 2000, -1000, 1000);
  h1APVCMres_->SetXTitle("strips");
  h1APVCMres_->SetYTitle("[ADC]");
  h1APVCMres_->SetLineWidth(2);
  h1APVCMres_->SetLineStyle(2);
 */ 
  /*h1RawDigis_ = fs_->make<TH1F>("rawdigis","rawdigis", 38400, 0, 38401);
  h1RawDigis_->SetXTitle("nr");
  h1RawDigis_->SetYTitle("[adc]");
  h1RawDigis_->SetLineWidth(2);
  h1RawDigis_->SetLineStyle(2);
  
  h1PRawDigis_ = fs_->make<TH1F>("pedestal rawdigis","pedestal rawdigis", 38400, 0, 38401);
  h1PRawDigis_->SetXTitle("nr");
  h1PRawDigis_->SetYTitle("[adc]");
  h1PRawDigis_->SetLineWidth(2);
  h1PRawDigis_->SetLineStyle(2);

  h1CRawDigis_ = fs_->make<TH1F>(" cmn rawdigis","cmn rawdigis", 38400, 0, 38401);
  h1CRawDigis_->SetXTitle("nr");
  h1CRawDigis_->SetYTitle("[adc]");
  h1CRawDigis_->SetLineWidth(2);
  h1CRawDigis_->SetLineStyle(2);
*/
  entry =0;

  timeTree = fs_->make<TTree>("timeTree","timeTree");
  

  //outFile = new TFile("timeInfos.root","recreate");
  //timeTree = new TTree("timeTree","timeTree");
  timeTree->Branch("eventNr", &eventNr, "eventNr/i");
  timeTree->Branch("bunchNr", &bunchNr, "bunchNr/i");
  timeTree->Branch("timeT", &timeT, "timeT/l");
  timeTree->Branch("moduleKeys", "std::vector<uint32_t>", &moduleKeys);
  timeTree->Branch("moduleValues", "std::vector<uint32_t>", &moduleValues);
  timeTree->Branch("baselinesMtx", "std::vector< std::vector<float> >", &baselinesMtx);
  timeTree->Branch("chargeOfClustersMtx", "std::vector < std::vector<float> >", &chargeOfClustersMtx);
  timeTree->Branch("centerOfClustersMtx", "std::vector < std::vector<uint16_t> >", &centerOfClustersMtx);
  timeTree->Branch("stripsTouchedOfClustersMtx", "std::vector < std::vector<uint16_t> >", &stripsTouchedOfClustersMtx);
  timeTree->Branch("maxADCOfClustersMtx", "std::vector < std::vector<uint8_t> >", &maxADCOfClustersMtx);
  timeTree->Branch("baselineRMSMtx", "std::vector< std::vector<float> >", &baselineRMSMtx);
  timeTree->Branch("positiveValuesCMNSMtx", "std::vector< std::vector<uint8_t> >" , &positiveValuesCMNSMtx);
  timeTree->Branch("negativeValuesPSMtx", "std::vector< std::vector<uint8_t> >" , &negativeValuesPSMtx);
  timeTree->Branch("baselineCMRMSMtx", "std::vector< std::vector<float> >" , &baselineCMRMSMtx);
  timeTree->Branch("PSSlope20Mtx", "std::vector< std::vector<float> >" , &PSSlope20Mtx);
}


SiStripBaselineAnalyzerSlim2::~SiStripBaselineAnalyzerSlim2()
{
 
   

}

void
SiStripBaselineAnalyzerSlim2::analyze(const edm::Event& e, const edm::EventSetup& es)
{

   entry++;
   //std::cout << "NEW EVENT!" << std::endl; 
   moduleKeys.clear();
   moduleValues.clear();
   baselinesMtx.clear();
   chargeOfClustersMtx.clear();
   centerOfClustersMtx.clear();
   stripsTouchedOfClustersMtx.clear();
   maxADCOfClustersMtx.clear();
   baselineRMSMtx.clear();
   positiveValuesCMNSMtx.clear();
   negativeValuesPSMtx.clear();
   baselineCMRMSMtx.clear();
   PSSlope20Mtx.clear();
   //ADCOfSaturStripsMtx.clear();
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
   //std::cout << "timeT: " << timeT << std::endl;
   //static const ULong64_t t = timeT;


   uint32_t counter=0; 
   edm::DetSetVector<SiStripProcessedRawDigi>::const_iterator itDSBaseline;
   itDSBaseline = moduleBaseline->begin();
   edm::DetSetVector<SiStripRawDigi>::const_iterator itRawDigis = moduleRawDigi->begin();
   
     edm::Handle<edm::DetSetVector<SiStripProcessedRawDigi> > moduleCM;
     edm::InputTag CMLabel("siStripZeroSuppression:APVCM");
          e.getByToken(srcAPVCM_,moduleCM);
    
 
   for (; itRawDigis != moduleRawDigi->end(); ++itRawDigis) {
      //std::cout << "modules to display: " << nModuletoDisplay_ << std::endl;
      if(actualModule_ > nModuletoDisplay_) {return;}
      uint32_t detId = itRawDigis->id;
      
      //std::cout << "time : " << time << "t diff" << tDiff << std::endl;
      sprintf(detIds,"%u", detId);
      sprintf(evs,"%llu", event);
      sprintf(runs,"%u", run);
      sprintf(times,"%llu", timeT);
      
      uint32_t mapSize = idx.size();
      idx.insert(std::pair<uint32_t, uint32_t>(detId,mapSize+1));

	  
//	std::cout << "bas id: " << itDSBaseline->id << " raw id: " << detId << std::endl;
	if(itDSBaseline->id != detId){
		itDSBaseline = moduleBaseline->find(detId);
                //std::cout << "before continue" << std::endl;
                if(itDSBaseline->id != detId){ itDSBaseline++; continue; }
                //std::cout << "after continue" << std::endl;
//                else std::cout << "Resynched..." << std::endl;
	}	  
      
    
      actualModule_++;
      //std::cout << "event nr: " << e.id().event() << "run nr: " <<  e.id().run()<< std::endl;
      //std::cout << "processing module N: " << actualModule_<< " detId: " << detId << " event: "<< event << std::endl; 
	 

	  
      edm::DetSet<SiStripRawDigi>::const_iterator itRaw = itRawDigis->begin(); 
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
      int32_t num = 0;
      int32_t denom = 0;
      uint32_t charge = 0;  
      uint32_t totCharge = 0;  
      std::vector<float> blsInModule;  
      std::vector<float> baselineRMS;
      std::vector<float> rms;  
      std::vector<float> rmscm;  
      std::vector<uint8_t> positiveInMod;
      std::vector<uint8_t>negativeInMod;
      std::vector<float> cmbaselineRMS;
      std::vector<float> slopeInMod;
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
                blsInModule.push_back(adcbl); //@MJ@ TODO be aware, here at 0 position weird value to recognize flatness

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
                baselineRMS.push_back(rmsRes);
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
                cmbaselineRMS.push_back(rmsRescm);

                if(positiveStrips > 128)
                    cout << "sth fishy in here, there should be max 128 positive strips" << endl;
                if(negativeStrips > 128)
                    cout << "sth fishy in here, there should be max 128 negative strips" << endl;
                positiveInMod.push_back(positiveStrips);
                negativeInMod.push_back(negativeStrips);
                positiveStrips = 0;
                negativeStrips = 0;

                //cout << "map all strips size is: " << digiMap.size() << endl;

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

                //cout << "fit constant: " << f10->GetParameter(0) << " slope: " <<  f10->GetParameter(1) << endl;
                
                TF1 *f20 = new TF1("f20","[0]+[1]*x", 0 , 127);
                f20->SetParameters(128,0);
                graph20p.Fit("f20", "CQ");

                //PSSlope10Mtx[idx.find(detId)->second].push_back(f10->GetParameter(1));
                slopeInMod.push_back(f20->GetParameter(1));

            //if(f10->GetParameter(0)< 0)
            //{
            //    graph10p.Write();
                
            //}
                //map10p.clear();
                map20p.clear();
                //delete f10;
                delete f20;
                

            }
            //std::cout << "det Id: " << detId << "strip " << strip << " baseline: " << itBaseline->adc() << std::endl;
	    ++itBaseline;
	 }
	++strip;
      }
      //tBaseline_.at((idx.find(detId)->second)-1)->Fill(tDiff, num/denom);
      baselinesMtx.push_back(blsInModule);	  
      //saturStripsMtx.push_back(saturStrips);
      //ADCOfSaturStripsMtx.push_back(ADCOfSaturStrips);
      baselineRMSMtx.push_back(baselineRMS);
      positiveValuesCMNSMtx.push_back(positiveInMod);
      negativeValuesPSMtx.push_back(negativeInMod);
      baselineCMRMSMtx.push_back(cmbaselineRMS);
      PSSlope20Mtx.push_back(slopeInMod);
      //myCsSrtripsTouchedMtx.push_back(myCsSrtripsTouched);
      //myCsChargeMtx.push_back(myCsCharge);
      //myCsMaxChargeMtx.push_back(myCsMaxCharge);
      //myCsCenterMtx.push_back(myCsCenter);
      ++itDSBaseline;


                                  
      for(uint32_t ccol = 0; ccol<1; ccol++  )
      {
          int nclust = 0;
          uint16_t clusCharge = 0;
          uint16_t clusCenter = 0;
          std::vector<float> clustersCharges;
          std::vector<uint16_t> clustersCenters;
          std::vector<uint16_t> clustersStripsTouched;
          std::vector<uint8_t> clustersMaxADC;
          edmNew::DetSetVector<SiStripCluster>::const_iterator itClusters;
          edmNew::DetSetVector<SiStripCluster>::const_iterator itClustersEnd;
          if(ccol == 0)
          {
              itClusters = clusters->begin();
              itClustersEnd = clusters->end();
          }
          else if(ccol == 1)
          {
              throw std::runtime_error("only one clustering now");
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
                      //clusCharge += (*itAmpl);
                      }
                      clustersCharges.push_back(clusCharge);
                      clustersCenters.push_back(clusCenter);
                      clustersStripsTouched.push_back(strip);
                      clustersMaxADC.push_back(maxc);
                      clusCharge = 0;
                      clusCenter = 0;
                      strip = 0;
                      maxc = 0;
		    }
		}
	  
             }

      if(ccol == 0)
      {
          chargeOfClustersMtx.push_back(clustersCharges);
          centerOfClustersMtx.push_back(clustersCenters);
          stripsTouchedOfClustersMtx.push_back(clustersStripsTouched);
          maxADCOfClustersMtx.push_back(clustersMaxADC);
      }
      //else if(ccol == 1)
      //{
          //NCchargeOfClustersMtx.push_back(clustersCharges);
          //NCcenterOfClustersMtx.push_back(clustersCenters);
          //NCstripsTouchedOfClustersMtx.push_back(clustersStripsTouched);
          //NCmaxADCOfClustersMtx.push_back(clustersMaxADC);
      //}
      }
    


   }		
    

actualModule_ =0;  
//tDiff =3;
//std::cout << "t DIff: " << tDiff << std::endl;

//std::cout << "tL: " << timeL << " t " << timeT << std::endl;
std::transform( idx.begin(), idx.end(), std::back_inserter( moduleKeys ), [](std::pair<uint32_t, uint32_t> const & p) { return p.first; } );
std::transform( idx.begin(), idx.end(), std::back_inserter( moduleValues ), [](std::pair<uint32_t, uint32_t> const & r) { return r.second; } );
timeTree->Fill();
}


// ------------ method called once each job just before starting event loop  ------------
void SiStripBaselineAnalyzerSlim2::beginJob()
{
  
  
actualModule_ =0;  
   
 
}

// ------------ method called once each job just after ending the event loop  ------------
void 
SiStripBaselineAnalyzerSlim2::endJob() {
     
}

//define this as a plug-in
DEFINE_FWK_MODULE(SiStripBaselineAnalyzerSlim2);

