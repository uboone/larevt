#ifndef SIOVDETPEDESTALSERVICE_CXX
#define SIOVDETPEDESTALSERVICE_CXX

#include "SIOVDetPedestalService.h"

namespace lariov{

  SIOVDetPedestalService::SIOVDetPedestalService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg) 
  : fProvider(pset.get<fhicl::ParameterSet>("DetPedestalRetrievalAlg"))
  {
    std::cout<<"SIOVDetPedestalService: entering constructor!"<<std::endl;
    //register callback to update local database cache before each event is processed
    //reg.sPreProcessEvent.watch(&SIOVDetPedestalService::PreProcessEvent, *this);
    reg.sPreProcessEvent.watch(this, &SIOVDetPedestalService::PreProcessEvent);
    std::cout<<"SIOVDetPedestalService: exiting constructor!"<<std::endl;
  }

}//end namespace lariov

DEFINE_ART_SERVICE_INTERFACE_IMPL(lariov::SIOVDetPedestalService, lariov::IDetPedestalService)

#endif
