#ifndef SIOVDETPEDESTALSERVICE_CC
#define SIOVDETPEDESTALSERVICE_CC

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "CalibrationDBI/Interface/IDetPedestalService.h"
#include "CalibrationDBI/Providers/DetPedestalRetrievalAlg.h"

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
        // This is a temporary kludge to allow microboone to analyze early data 
	// which did not have a proper timestamp in the daq header.
	// NOTE: it would be nice if there was a way to check that the art::Event
	// is microboone or not so that this kludge does not affect other experiments
        if (evt.isRealData() && evt.run() < 183) {
	  std::uint64_t kludge_stamp = 1430000000000000000; //yes, there really needs to be 16 zeroes
	  fProvider.Update(kludge_stamp);
        }
	else fProvider.Update(evt.time().value());
      }
     
    private:
    
      const IDetPedestalProvider& DoGetPedestalProvider() const override {
        return fProvider;
      }    
    
      DetPedestalRetrievalAlg fProvider;
  };
}//end namespace lariov
      
DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVDetPedestalService, lariov::IDetPedestalService, LEGACY)
      

namespace lariov{

  SIOVDetPedestalService::SIOVDetPedestalService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg) 
  : fProvider(pset.get<fhicl::ParameterSet>("DetPedestalRetrievalAlg"))
  {
    //register callback to update local database cache before each event is processed
    //reg.sPreProcessEvent.watch(&SIOVDetPedestalService::PreProcessEvent, *this);
    reg.sPreProcessEvent.watch(this, &SIOVDetPedestalService::PreProcessEvent);
  }

}//end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVDetPedestalService, lariov::IDetPedestalService)

#endif
