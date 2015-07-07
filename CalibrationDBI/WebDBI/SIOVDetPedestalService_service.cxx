#ifndef SIOVDETPEDESTALSERVICE_CXX
#define SIOVDETPEDESTALSERVICE_CXX

#include "SIOVDetPedestalService.h"

namespace lariov{

  SIOVDetPedestalService::SIOVDetPedestalService(fhicl::ParameterSet const& pset, art::ActivityRegistry& reg) 
  : fProvider(pset.get<fhicl::ParameterSet>("DetPedestalRetrievalAlg"))
  {
  
    //register callback to update local database cache before each event is processed
    reg.sPreProcessEvent.watch(&SIOVDetPedestalService::PreProcessEvent, *this);
  }

}//end namespace lariov

#endif
