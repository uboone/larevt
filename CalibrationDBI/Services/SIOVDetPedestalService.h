#ifndef SIOVDETPEDESTALSERVICE_H
#define SIOVDETPEDESTALSERVICE_H

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "CalibrationDBI/Services/IDetPedestalService.h"
#include "CalibrationDBI/WebDBI/DetPedestalRetrievalAlg.h"

namespace lariov{

  /**
     \class SIOVDetPedestalService
     art service implementation of IDetPedestalService.  Implements 
     a detector pedestal retrieval service for database scheme in which 
     all elements in a database folder share a common interval of validity
  */
  class SIOVDetPedestalService : public IDetPedestalService {
  
    public:
    
      SIOVDetPedestalService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      ~SIOVDetPedestalService(){}
      
      void PreProcessEvent(const art::Event& evt) {
        fProvider.Update(evt.time().value());
      }
     
    private:
    
      const IDetPedestalProvider& DoGetPedestalProvider() const override {
        std::cout<<"SIOVDetPedestalService::DoGetPedestalProvider"<<std::endl;
        return fProvider;
      }    
    
      DetPedestalRetrievalAlg fProvider;
  };
}//end namespace lariov
      
DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVDetPedestalService, lariov::IDetPedestalService, LEGACY)
      
#endif
