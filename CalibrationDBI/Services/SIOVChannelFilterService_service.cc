#ifndef SIOVCHANNELFILTERSERVICE_CC
#define SIOVCHANNELFILTERSERVICE_CC

#include "art/Framework/Services/Registry/ServiceMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"
#include "art/Framework/Principal/Event.h"
#include "fhiclcpp/ParameterSet.h"
#include "CalibrationDBI/Interface/IChannelFilterService.h"
#include "CalibrationDBI/Providers/SIOVChannelFilterProvider.h"

namespace lariov{

  /**
     \class SIOVChannelFilterService
     art service implementation of IChannelFilterService.  Implements 
     a channel status retrieval service for database scheme in which 
     all elements in a database folder share a common interval of validity
  */
  class SIOVChannelFilterService : public IChannelFilterService {
  
    public:
    
      SIOVChannelFilterService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg);
      ~SIOVChannelFilterService(){}
      
      void PreProcessEvent(const art::Event& evt); 
     
    private:
    
      const IChannelFilterProvider& DoGetFilter() const override {
        return fProvider;
      }    
      
      const IChannelFilterProvider* DoGetFilterPtr() const override {
        return &fProvider;
      }
    
      SIOVChannelFilterProvider fProvider;
      bool fFindNoisyChannels;
  };
}//end namespace lariov
      
DECLARE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelFilterService, lariov::IChannelFilterService, LEGACY)
      

namespace lariov{

  SIOVChannelFilterService::SIOVChannelFilterService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg) 
  : fProvider(pset.get<fhicl::ParameterSet>("ChannelFilterRetrievalAlg"))
  {
    
    fFindNoisyChannels = pset.get<bool>("FindNoisyChannels",false);
    
    //register callback to update local database cache before each event is processed
    //reg.sPreProcessEvent.watch(&SIOVChannelFilterService::PreProcessEvent, *this);
    reg.sPreProcessEvent.watch(this, &SIOVChannelFilterService::PreProcessEvent);
  }
  
  void SIOVChannelFilterService::PreProcessEvent(const art::Event& evt) {
        fProvider.Update(evt.time().value());
	
	if (fFindNoisyChannels) {
	  //retrieve raw digits and update noisy channels
	}
      }

}//end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVChannelFilterService, lariov::IChannelFilterService)

#endif
