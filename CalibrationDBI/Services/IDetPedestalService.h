#ifndef IDETPEDESTALSERVICE_H
#define IDETPEDESTALSERVICE_H

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include <iostream>

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

      virtual ~IDetPedestalService() = default;

      //retrieve pedestal provider
      const IDetPedestalProvider& GetPedestalProvider() const {
        std::cout<<"Calling implementation of DoGetPedestalProvider"<<std::endl;
        return this->DoGetPedestalProvider();
      }
      
    private:
      
      virtual const IDetPedestalProvider& DoGetPedestalProvider() const = 0;
  };
}//end namespace lariov

DECLARE_ART_SERVICE_INTERFACE(lariov::IDetPedestalService, LEGACY)

#endif
