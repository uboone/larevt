#ifndef IOVDATA_CONSTANTS_H
#define IOVDATA_CONSTANTS_H

#include "IOVTimeStamp.h"
#include <string>
#include <limits>
namespace lariov {

  const std::string kTREE_PREFIX = "iov";
  
  //do not set substamp to anything other than 0 just incase its digit limit is changed in IOVTimeStamp
  const IOVTimeStamp kMAX_TIME = IOVTimeStamp( std::numeric_limits<unsigned long>::max(), 0);
  const IOVTimeStamp kMIN_TIME = IOVTimeStamp(0,0);
}
#endif
