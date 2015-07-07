#ifndef IDETPEDESTALSERVICE_H
#define IDETPEDESTALSERVICE_H

#include "art/Framework/Services/Registry/ServiceMacros.h"

//forward declarations
namespace lariov {
  class IDetPedestalProvider;
}

namespace lariov{

  /**
     \class IDetPedestalService
     art service interface for detector pedestal conditions retrieval
  */
  class IDetPedestalService {
  
    public:

      //retrieve pedestal provider
      virtual const IDetPedestalProvider& GetPedestalProvider() const = 0;
  };
}//end namespace lariov

DECLARE_ART_SERVICE_INTERFACE(lariov::IDetPedestalService, LEGACY)

#endif
