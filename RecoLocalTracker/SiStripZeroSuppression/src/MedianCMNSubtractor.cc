#include <iostream>
#include "RecoLocalTracker/SiStripZeroSuppression/interface/MedianCMNSubtractor.h"

void MedianCMNSubtractor::subtract(const uint32_t& detId,const uint16_t& firstAPV, std::vector<int16_t>& digis) {subtract_(detId,firstAPV,digis);}
void MedianCMNSubtractor::subtract(const uint32_t& detId,const uint16_t& firstAPV, std::vector<float>& digis) {subtract_(detId,firstAPV, digis);}

template<typename T> 
inline
void MedianCMNSubtractor::
subtract_(const uint32_t& detId,const uint16_t& firstAPV, std::vector<T>& digis){
  //std::cout << "start of pedestal subtraction: " << std::endl; 
  std::vector<T> tmp;  tmp.reserve(128);  
  std::vector<T> tmpbot;  tmpbot.reserve(128);  
  typename std::vector<T>::iterator  
    strip( digis.begin() ), 
    end(   digis.end()   ),
    endAPV;

  //std::cout << "digi 1 : " << *digis.begin() << std::endl;
  
  typename std::vector<T>  stripbotvec = digis; 
  typename std::vector<T>::iterator  stripbot = stripbotvec.begin() ; 
  typename std::vector<T>::iterator  endAPVbot;


  //std::cout << "bot digi before bottom  : " << *stripbot << std::endl;
    while(stripbot < stripbotvec.end()) //bottom all
    {
      if(/*fedmode_ &&*/ *stripbot < 0) //FED bottoms out at 0
      {
        *stripbot=0;
        //std::cout << "FED mode" << std::endl;
      }
        stripbot++;
    }
  //std::cout << "digi 2 : " << *digis.begin() << std::endl;
  
  stripbot = stripbot - std::distance(strip,end);
  //std::cout << "bot digi after bottom  : " << *stripbot << std::endl;
  _vmedians.clear();

  
  while( strip < end ) {
    //std::cout << "starting strip: " << *strip << " starting strip bot: " << *stripbot << std::endl;
    endAPV = strip+128; tmp.clear();
    endAPVbot = stripbot+128; tmpbot.clear();
    tmp.insert(tmp.end(),strip,endAPV);
    tmpbot.insert(tmpbot.end(),stripbot,endAPVbot);
    const float offset = median(tmp);
    const float offsetbot = median(tmpbot);

    _vmedians.push_back(std::pair<short,float>((strip-digis.begin())/128+firstAPV,offset));
    //std::cout << "position: " << (strip-digis.begin())/128+firstAPV << " offset without bottom: " << offset << " OFFSET WITH BOTTOM: " << offsetbot << std::endl;
    
 uint32_t counter = 0; 
    while (strip < endAPV) {
      if(*stripbot < 0)
          throw std::runtime_error("FED mode, digi should not be less than zero!");
      if(offsetbot < 0)
          throw std::runtime_error("FED mode, baseline should not be less than zero!");
      *strip = static_cast<T>(*stripbot-offsetbot); //@MJ@ TODO put this back!!!!!
      strip++;
      stripbot++;
    }
    //std::cout << "strips soubtracted: " << counter << std::endl;
  }
  //std::cout << "end of pedestal subtraction: " << std::endl; 
}


void MedianCMNSubtractor::
bottomAndSubtract(std::vector<int16_t>& digis){
  std::vector<int16_t> tmp;  tmp.reserve(128);  
  std::vector<int16_t>::iterator  
    strip( digis.begin() ), 
    end(   digis.end()   ),
    endAPV;
  
    uint32_t dist =  std::distance(strip,end); 
    while(strip < end) //bottom all
    {
      if(*strip < 0) //FED bottoms out at 0
      {
        *strip=0;
      }
        strip++;
    }
  //std::cout << "digi 2 : " << *digis.begin() << std::endl;
  
  strip = strip - dist; //return back
  
  while( strip < end ) {
    endAPV = strip+128; tmp.clear();
    tmp.insert(tmp.end(),strip,endAPV);
    const float offset = median(tmp);

    
    while (strip < endAPV) {
      *strip = static_cast<int16_t>(*strip-offset);
      strip++;
    }

  }
}

