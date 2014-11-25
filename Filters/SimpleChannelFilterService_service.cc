/**
 * @file   SimpleChannelFilterService_service.cc
 * @brief  Service for channel quality info
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   November 24th, 2014
 * @see    SimpleChannelFilterService.h
 */

// Our header
#include "Filters/SimpleChannelFilterService.h"


// C/C++ standard libraries
#include <vector>
#include <algorithm> // std::copy()
#include <iterator> // std::inserter()

// Framework libraries
#include "fhiclcpp/ParameterSet.h"
#include "messagefacility/MessageLogger/MessageLogger.h"


namespace filter {

  //----------------------------------------------------------------------------
  SimpleChannelFilterService::SimpleChannelFilterService
    (fhicl::ParameterSet const& pset, art::ActivityRegistry& reg):
    ChannelFilterServiceInterface(pset, reg)
  {
    SetFilter(new SimpleChannelFilter(pset));
    
    mf::LogInfo("SimpleChannelFilterService") << "Loaded from configuration:"
      << "\n  - " << SetOfBadChannels().size() << " bad channels"
      << "\n  - " << SetOfNoisyChannels().size() << " noisy channels"
      ;
    
  } // SimpleChannelFilterService::SimpleChannelFilterService()
  
  
  //----------------------------------------------------------------------------
  DEFINE_ART_SERVICE_INTERFACE_IMPL(filter::SimpleChannelFilterService, filter::ChannelFilterServiceInterface)
  
} // namespace filter
