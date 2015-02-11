#ifndef IOVDATA_CONSTANTS_H
#define IOVDATA_CONSTANTS_H

#include <TTimeStamp.h>
#include <string>
#include <climits>
#include <limits>
#include <time.h>
namespace lariov {

  enum ValueType_t {
    kSTRING,
    kSHORT,
    kINT,
    kLONG,
    kFLOAT,
    kDOUBLE,
    kTIMESTAMP,
    kUNKNOWN
  };

  const std::string kTREE_PREFIX = "iov";
  
  const int kMAX_INT = std::numeric_limits<int>::max();
  const TTimeStamp kMAX_TIME = TTimeStamp(kMAX_INT,kMAX_INT);
  const TTimeStamp kMIN_TIME = TTimeStamp(0,0);
}
#endif
