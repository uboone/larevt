#ifndef TIMESTAMPDECODER_H
#define TIMESTAMPDECODER_H

#include "IOVTimeStamp.h"

namespace lariov {

  class TimeStampDecoder {
  
    public:
    
      TimeStampDecoder() {}
      virtual ~TimeStampDecoder();
      
      static IOVTimeStamp DecodeTimeStamp(std::uint64_t ts);
  };
}

#endif    
