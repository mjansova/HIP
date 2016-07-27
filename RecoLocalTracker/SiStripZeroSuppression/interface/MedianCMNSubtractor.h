#ifndef RECOLOCALTRACKER_SISTRIPZEROSUPPRESSION_SISTRIPMEDIANCOMMONMODENOISESUBTRACTION_H
#define RECOLOCALTRACKER_SISTRIPZEROSUPPRESSION_SISTRIPMEDIANCOMMONMODENOISESUBTRACTION_H
#include "RecoLocalTracker/SiStripZeroSuppression/interface/SiStripCommonModeNoiseSubtractor.h"

class MedianCMNSubtractor : public SiStripCommonModeNoiseSubtractor {
  
  friend class SiStripRawProcessingFactory;
  friend class SiStripPedestalsSubtractor;
  
 public:
  
  void subtract(const uint32_t&,const uint16_t&, std::vector<int16_t>&);
  void subtract(const uint32_t&,const uint16_t&,std::vector<float>&);
  void bottomAndSubtract(std::vector<int16_t>& digis);
 
 private:
  
  template<typename T> void subtract_(const uint32_t&,const uint16_t&,std::vector<T>&);
  MedianCMNSubtractor(){};
  
};
#endif
