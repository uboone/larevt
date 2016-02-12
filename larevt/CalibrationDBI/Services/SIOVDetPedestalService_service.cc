#ifndef SIOVDETPEDESTALSERVICE_CC
#define SIOVDETPEDESTALSERVICE_CC

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "larevt/CalibrationDBI/Interface/DetPedestalService.h"
#include "larevt/CalibrationDBI/Providers/DetPedestalRetrievalAlg.h"

namespace lariov{

  /**
     \class SIOVDetPedestalService
     art service implementation of DetPedestalService.  Implements 
     a detector pedestal retrieval service for database scheme in which 
     all elements in a database folder share a common interval of validity
  */
  class SIOVDetPedestalService : public DetPedestalService {
  
    public:
    
      SIOVDetPedestalService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      ~SIOVDetPedestalService(){}
      
      void PreProcessEvent(const art::Event& evt) {
        fProvider.Update(evt.time().value());
      }
     
    private:
    
      const DetPedestalProvider& DoGetPedestalProvider() const override {
        return fProvider;
      }    
    
      DetPedestalRetrievalAlg fProvider;
  };
}//end namespace lariov
      
DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVDetPedestalService, lariov::DetPedestalService, LEGACY)
      

namespace lariov{

  SIOVDetPedestalService::SIOVDetPedestalService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg) 
  : fProvider(pset.get<fhicl::ParameterSet>("DetPedestalRetrievalAlg"))
  {
    //register callback to update local database cache before each event is processed
    //reg.sPreProcessEvent.watch(&SIOVDetPedestalService::PreProcessEvent, *this);
    reg.sPreProcessEvent.watch(this, &SIOVDetPedestalService::PreProcessEvent);
  }

}//end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVDetPedestalService, lariov::DetPedestalService)

#endif
